/*
Copyright (c) 2008 Rodrigo Roman and Ramon Roman (rrc2soft)
(Original Game "Knytt Stories" copyright (c) Nicklas "Nifflas" Nygren)

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of "rrc2soft" nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* 
	- Known bugs
	  + The GlobalPainted system does not work properly. I had to reset all
	    objects before checking update != -1
	    Probably because of problems with sprite->update
	    (Lines:
	       if (!_ds_3dsprite_getGlobalPainted(sprite)) { - setFrame
		  )
	  + It was a BO. Need to recheck the system.
*/

#include <PA9.h>       // Include for PA_Lib
#include "ds_util_bit.h"
#include "ds_3dspritehdd.h"
#include "ds_global.h"
#include "ds_util.h"
#include "ds_gamestatus.h"
#include "ds_map.h"
#include "ds_submap.h" // OPTIMIZATION

//-------------------------------------------------------------------------------------------------
// IMPORTANT DETAILS
//-------------------------------------------------------------------------------------------------
/*
F*** IMPORTANT - Render in advance in the 3D HW
	The Rendering Engine draws the various Polygons, and outputs them as BG0 layer to the 2D Video 
	controller (which may then output them to the screen, or to the video capture unit). The 
	Rendering part is done automatically by hardware, so the software has little influence on it.
	Rendering is done scanline-by-scanline, so there's only a limited number of clock cycles per 
	scanline, which is limiting the maximum number of polygons per scanline. However, due to the 
	48-line cache (see below), some scanlines are allowed to exceed that maximum.
	Rendering starts 48 lines in advance (while still in the Vblank period) (and does then 
	continue throughout the whole display period), the rendered data is written to a small cache 
	that can hold up to 48 scanlines.
	(Note that the 3D engine "stops" aquiring data from VRAM @ 192-48 approx. Check flags.)
	(Also, the 3D engine starts rendering in 262-48 (214), more or less)
	
ANOTHER DETAIL:
   Try NOT to load sprites that are going to delete themselves later ^_-

Bugs?
	- imageID of global sprites = -1?????????

*/
//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

// Max number of objects/sprites in the screen
#define _MAX_OBJSPRITE 1024

// Max size of the VRAM for object textures (a 256x256 texture)
#define _MAX_VRAMTEX 65536

// Max number of parts for an sprite
#define _MAX_PARTSSPRITE 64

// Flags for the sprites
#define _DS_C_3D_SPECIAL_GLOBAL 0		// Sprite is global?
#define _DS_C_3D_SPECIAL_INVISIBLE 1	// Sprite invisible?
#define _DS_C_3D_SPECIAL_UPDATE 2		// Update in HW - VRAM?
#define _DS_C_3D_SPECIAL_LOAD 3			// Special Update?
#define _DS_C_3D_SPECIAL_ALPHA 4			// 	Special - Update Alpha?
#define _DS_C_3D_SPECIAL_PRIO 5			// 	Special - Update priority?
#define _DS_C_3D_SPECIAL 7

// Type of Sprite
#define _DS_C_3D_TYPE_MIX    0
#define _DS_C_3D_TYPE_NORMAL 1
#define _DS_C_3D_TYPE_NOSPR  2

typedef struct _ds_tt_sprite_ {
   int type_sprite; // Type of sprite (used for pointer check)
} _ds_t_sprite_;

typedef struct _ds_tt_sprite_imaMix {
   int type_sprite; // Type of sprite (used for pointer check)
   int partsMix; // parts that compose this sprite
   //
   int imageIDMix[_MAX_PARTSSPRITE]; // textures for the special "mix" sprite
   u16* cacheMix[_MAX_PARTSSPRITE]; // cache for the "mix" sprites
   int xposMix[_MAX_PARTSSPRITE]; // X offsets for the special "mix" sprite
   int yposMix[_MAX_PARTSSPRITE]; // Y offsets for the special "mix" sprite
   int xsizeMix[_MAX_PARTSSPRITE]; // X size for the special "mix" sprite
   int ysizeMix[_MAX_PARTSSPRITE]; // Y size for the special "mix" sprite
} _ds_t_sprite_imaMix;

typedef struct _ds_tt_sprite_ima {
   int type_sprite; // Type of sprite (used for pointer check)
   //
   int imageID;  // associated texture to the sprite
   u16* cache; // cache for the 3Dsprite
} _ds_t_sprite_ima;

typedef struct _ds_tt_sprite_sprMix {
   int type_sprite; // Type of sprite (used for pointer check)
   int partsMix; // parts that compose this sprite
   //
   int imageMix[_MAX_PARTSSPRITE]; // sprites for the special "mix" sprite
} _ds_t_sprite_sprMix;

typedef struct _ds_tt_sprite_spr {
   int type_sprite; // Type of sprite (used for pointer check)
   //
	int image;  // which sprite we are managing here
} _ds_t_sprite_spr;

typedef struct _ds_tt_sprite_data {
   int xsize;
   int ysize;   // Size of the sprite   
   int xpos;
   int ypos;   // (Real/General) position of the sprite
   u8 bank;
   u8 obj;     // bank and type of the object
   u8 frame;   // actual frame of this sprite
   u8 alpha;   // Alpha value
   int prio;	// Priority
} _ds_t_sprite_data;

typedef struct _ds_tt_sprite {
   int id;     				// ID of the sprite, is the same as in the SpriteList
   u8 type_sprite; 			// Type of sprite
   void* imainfo; 			// Image Information - HW-related  (shared by globals)
   void* sprinfo;				// Image Information - Sprite-related  (local per sprite)
   _ds_t_sprite_data data; // Data for the sprite (position, etc)
   u8 flags; // Flags managing special (e.g. alpha/background/dialog...) sprites 
} _ds_t_sprite;  

// Store pointers to all the sprites used in one screen. No linked lists here :-P
_ds_t_sprite* spriteList[_MAX_OBJSPRITE];
void* spriteListArr; // Controls that the sprite have unique IDs
void* sprite3DListArr; // Controls that the 3D sprites have unique IDs
// Store pointers to all the "global sprites" structure. Also, no linked lists here ;-)
void* spriteGlobal[DS_C_MAX_BANK][DS_C_MAX_OBJ]; // * to IMA structure
void* particleGlobal[DS_C_MAX_OBJ_PART];			 // * to IMA structure
void* coGlobal[DS_C_MAX_OBJ_CO]; // * to IMA structure
u8 spriteGlobalPainted[DS_C_MAX_BANK][DS_C_MAX_OBJ];
u8 particleGlobalPainted[DS_C_MAX_OBJ_PART];
u8 coGlobalPainted[DS_C_MAX_OBJ_CO];
int _spriteLoad; // Number of LOAD sprites

// RAW Size of the VRAM (not considering optimizations)
int _vramSize3D = 0;

// Number of textures in this screen (for debug purposes)
int _maxtexture = 0;

// Big Screen - textures
int _screen3DTex;

// HACK - "Global sprites"
int _hack_glocalSprites = _MAX_OBJSPRITE - 2;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS - GENERAL MANAGEMENT
//-------------------------------------------------------------------------------------------------

/* Copy an image to a certain "cache" buffer. "Cache" buffer should be always bigger */
void _ds_3dsprite_copy(u16 * sbuffer, int sx, int sy, u16 * dbuffer, int dx, int dy, int copy, int prefill) {
   int i, j;
   // First, fill destination buffer with the "transparent" color.
   if (prefill)
   	for (i = 0; i < (dx * dy); i++)
   		dbuffer[i] = 0; // NOTA!!! - PALib transparent color is 0
   	
   // Now, Copy        
   if (copy) {
	   DC_FlushAll();  // <TODO> DC_FlushRange(src,len); - DC_InvalidateRange(dst,len);?         
	   for (j = 0; j < sy; j++) {
		   DMA_Copy(((sbuffer) + (j*sx)),((dbuffer) + (j*dx)),sx >> 1,DMA_32NOW);
		}
	}   
}

/* "Special" Copy an image to a certain "cache" buffer. 
		"Cache" buffer should be always bigger */
void _ds_3dsprite_copyEx(u16 * sbuffer, int sx, int sy, 
								 int sinix, int siniy, int ssx, int ssy, 
								 u16 * dbuffer, int dx, int dy, int copy, int prefill) {
   int i, j;
   // First, fill destination buffer with the "transparent" color.
   if (prefill)
   	for (i = 0; i < (dx * dy); i++)
   		dbuffer[i] = 0; // NOTA!!! - PALib transparent color is 0
   		
   // Some corrections for the X/Y coordinate (I have to copy everything, or not?)
   if ((siniy + ssy) > sy)
   	ssy = sy - siniy;
   if ((sinix + ssx) > sx)
   	ssx = sx - sinix;
   	
   // Now, Copy        
   if (copy) {
      DC_FlushAll();  // <TODO> DC_FlushRange(src,len); - DC_InvalidateRange(dst,len);?      
      if ((ssx % 4) == 0) {
		   for (j = siniy; j < (siniy + ssy); j++) {
			   DMA_Copy(((sbuffer) + (sinix) + (j*sx)),((dbuffer) + ((j-siniy)*dx)),ssx >> 1,DMA_32NOW);
			}
      } else 
		if ((ssx % 2) == 0) {
		   for (j = siniy; j < (siniy + ssy); j++) {
			   DMA_Copy(((sbuffer) + (sinix) + (j*sx)),((dbuffer) + ((j-siniy)*dx)),ssx,DMA_16NOW);			   
			}
      } else {
		   for (j = siniy; j < (siniy + ssy); j++) {
			   memcpy(((dbuffer) + ((j-siniy)*dx)),((sbuffer) + (sinix) + (j*sx)),ssx << 1);
			}
		}     	      
	}   
}

/* Returns the "real" X position of a sprite */
int _ds_3dsprite_realXS(_ds_t_sprite* sprite, int x, int xs) {
   return (x + (ds_util_convertPow2(xs) >> 1));
}   

/* Returns the "real" Y position of a sprite */
int _ds_3dsprite_realYS(_ds_t_sprite* sprite, int y, int ys) {
   return (y + (ds_util_convertPow2(ys) >> 1));
}   

/* Returns if the ima of this sprite (global) have been already updated in this iteration */
int _ds_3dsprite_getGlobalPainted(_ds_t_sprite* sprite) {
   if (sprite != NULL) {
      if ((sprite->data.bank == DS_C_PART_BANK) && (sprite->data.obj < DS_C_MAX_OBJ_PART)) {
      	return particleGlobalPainted[sprite->data.obj];
      } else if ((sprite->data.bank == DS_C_CO_BANK)) { // DS_C_MAX_OBJ_CO
      	return coGlobalPainted[sprite->data.obj];
      } else if ((sprite->data.bank < DS_C_MAX_BANK) && (sprite->data.obj < DS_C_MAX_OBJ)) {
      	return spriteGlobalPainted[sprite->data.bank][sprite->data.obj];
    	}  	
	}     
	return 0; // By defect (juni and special sprites), UPDATE :-)
}

/* Set that the ima of this sprite (global) have been already updated in this iteration */
void _ds_3dsprite_setGlobalPainted(_ds_t_sprite* sprite, u8 val) {
   if (sprite != NULL) {
      if ((sprite->data.bank == DS_C_PART_BANK) && (sprite->data.obj < DS_C_MAX_OBJ_PART)) {
      	particleGlobalPainted[sprite->data.obj] = val;
      } else if ((sprite->data.bank == DS_C_CO_BANK)) { // DS_C_MAX_OBJ_CO
      	coGlobalPainted[sprite->data.obj]= val;
      } else if ((sprite->data.bank < DS_C_MAX_BANK) && (sprite->data.obj < DS_C_MAX_OBJ)) {
      	spriteGlobalPainted[sprite->data.bank][sprite->data.obj] = val;
    	}  	
	}     
}

/* Special copy for the 3DScreen
   - This method is LIBRARY DEPENDENT!!!!!!!!!!! Changes in PALib = changing this method */
void _PA_3DUpdateGfxScreen3D(u16 texture, u16 *screen, int x, int y, int pa_3Dbanks) {   
   vramSetBankA(VRAM_A_LCD);  
   if (pa_3Dbanks) 
		vramSetBankB(VRAM_B_LCD); 
   
   //256 * 2 bytes (256 / 2)
   u16 *ptr = (screen) + x;
   
   int _initextures = ((textures[texture] & 0xFFFF) << 3);

   int j;
   DC_FlushAll();  // <TODO> DC_FlushRange(src,len); - DC_InvalidateRange(dst,len);?
   for (j = 0; j < 192; j++) {
	  //DMA_Copy((image), (void*)(0x06800000 + (_initextures), (mem_size>>2), DMA_32NOW);
	  DMA_Copy((ptr + ((j + y) * 600) ),
	  				(void*)(0x06800000 + (_initextures) + (j << 9)),
					256 >> 1,DMA_32NOW);
	  /*memcpy((void*)(0x06800000 + (_initextures) + (j << 9)),
	         (ptr + ((j + y) * 600) ),
				256 << 1);*/
   }
   if (pa_3Dbanks) vramSetBankB(VRAM_B_TEXTURE);  
   vramSetBankA(VRAM_A_TEXTURE);
}   

/* Creates the special sprite for the screen */
void _ds_3dsprite_createScreen3D() {
	
	_screen3DTex = PA_3DCreateTex(ds_global_map.tileMap, // Pointer doesn't matter, but it is needed
										256,256,TEX_16BITS); // 256 . 192
	PA_3DCreateSpriteFromTex(_MAX_OBJSPRITE - 1, 	// ^_-
										_screen3DTex, 		// Gfx...
										256, 192,
										0,   		      // Palette, unused in 16bit...
										256 >> 1, 192 >> 1); // X, Y SPRITE CENTER !   
	PA_3DSetSpritePrio(_MAX_OBJSPRITE - 1,DS_C_PRIO * 0);
	
}

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS - IMAGE INFORMATION MGMT ("MIX" & co)
//-------------------------------------------------------------------------------------------------

/* Check if the sprite should be partitioned, and create a "normal"/"mix" structure.
	Returns "1" if sprite should be loaded as an sprite */   
int _ds_3dsprite_imaInfo_partition(_ds_t_sprite* sprite, int createSpr, int createIma) {
   int mixed = 0;
   int i,j;
	int k;
   int x,y;
   int cnt;
   int tmp, tmp2;
   int xsize, ysize;
   
   if (sprite == NULL)
   	return 0; // Of course, we cannot do a HW sprite -:)
   	
   // 1) Check if we create a partitioned (mixed) sprite
   xsize = sprite->data.xsize;
   ysize = sprite->data.ysize;
   	
   // 1st CASE: Sprite is < 64x64. Make a normal sprite (if there are no optimizations)
   if ((xsize <= 64) && (ysize <= 64)) {
      if ((xsize > 24) && (ysize > 24)) {
      	if (_vramSize3D < _MAX_VRAMTEX)
      		mixed = 0; // Fit the texture to the size of the sprite
      	else mixed = 3; // Special "as small as possible" check
   	} else {
   	   mixed = 0; // For 24x24, fit the texture to the size of the sprite
		}     
   } else  
   // 2nd CASE: Sprite is Squared   
   if (xsize == ysize) {
      // 2nd.1st CASE: Sprite is bigger than 128x128
	   if ((xsize > 128) && (ysize > 128)) {
	      return 0; // Do not make a HW sprite
	   }      
	   // 2nd.2nd CASE: Sprite is 64x64 <= sprite <= 128. SPRITE MIX!
		mixed = 1;   
	// 3rd CASE: Sprite is Rectangular   
	} else {
	   // 3rd.1st CASE: Sprite area is bigger than a 128x128 (16384) sprite
	   if ((xsize * ysize) > 16384) {
	      return 0; // Do not make a HW sprite
	   }
		// 3rd.2nd CASE: Sprite area is smaller than a 128x128 (16384) sprite   
			// NOTE that the partition of the image can still fail on this particular case.
		mixed = 2;
	}  
		
	// 2) Get the i/j size for the partitioned sprite
	if (!mixed) {
	   // Size of the sprite
	   i = xsize;
	   j = ysize;
	} else if (mixed == 1) {
	   // Size for squared sprites: 32x32 - FIXME - Test 16x16
	   i = 32;
	   j = 32;
	} else if (mixed == 2) {
	   // Check "small" sprites first - rectangular X/Y
	   if ((xsize <= 8) || (ysize <= 8)) {
	      i = 8;
	      j = 8;
	   } else
	   if ((xsize <= 16) || (ysize <= 16)) {
	      i = 16;
	      j = 16;
	   } else
	   if ((xsize <= 32) || (ysize <= 32)) {
	      i = 32;
	      j = 32;
	   } else 
		if ((xsize == 64) || (ysize == 64)) { // Special for dialogue -:)
	      i = 64;
	      j = 64;
	   } else {
	   	// OK, now choose between 16x16 and 32x32 according to the size of the sprite ;-)
	   	tmp = (xsize > ysize)?xsize:ysize;
	   	if (tmp <= 128) {
	   	   i = 16;
	   	   j = 16;
	   	} else {
	   	   i = 32;
	   	   j = 32;
	   	}   
		}   
		// OK, but now check if the overall sprite size is smaller than 128x128 (16384) :-P
		tmp = ((xsize % i) == 0)?(xsize / i):(xsize / i) + 1;
		tmp = tmp * i; // Area X
		tmp2 = ((ysize % j) == 0)?(ysize / j):(ysize / j) + 1;
		tmp2 = tmp2 * j; // Area Y
	   if ((tmp*tmp2) > 16384)	
	   	return 0; // No HW sprite!!!!!!!
	} else if (mixed == 3) {
	   // Special optimization
	   i = 16;
	   j = 16;
	}    
		
	// 3) Create the normal/mixed sprite
	if (!mixed) { // NORMAL
		sprite->type_sprite = _DS_C_3D_TYPE_NORMAL;
		if (createSpr) {
		   sprite->sprinfo = (void *) malloc(sizeof(_ds_t_sprite_spr));
		   _ds_t_sprite_spr *spr = (_ds_t_sprite_spr *) sprite->sprinfo;
		   spr->type_sprite = sprite->type_sprite;
		   spr->image = -1;
		}   
	   if (createIma) {
	      sprite->imainfo = (void *) malloc(sizeof(_ds_t_sprite_ima));
	      _ds_t_sprite_ima *ima = (_ds_t_sprite_ima *) sprite->imainfo;
	      ima->type_sprite = sprite->type_sprite;
	      ima->imageID = -1;
	      ima->cache = (u16 *) malloc(ds_util_convertPow2(i) *
	      										ds_util_convertPow2(j) *
	      										sizeof(u16));
	   }  
	} else { // MIXED  
		sprite->type_sprite = _DS_C_3D_TYPE_MIX;
	
		// First, calculate "k" -> partsMix
	   k = 0;
	   x = 0;
	   y = 0;
	   cnt = 0;
	   while (!cnt) {
	      // First: Change row?
	      if ((xsize - x) <= 0) {
	         x = 0;
	         y += j;
	      }   
	      // Second: End sprite?
	      if ((ysize - y) <= 0) {
	         cnt = 1;
	      }   	     
			if (!cnt) {
				k++;
				x += i;				
				// Warning!!!! Check the MAX limit
				if (k == _MAX_PARTSSPRITE) {
					cnt = 1;
				}			
			}   
	   }   			
		// Initialize the other elements
		if (createSpr) {
		   sprite->sprinfo = (void *) malloc(sizeof(_ds_t_sprite_sprMix));
		   _ds_t_sprite_sprMix *sprMix = (_ds_t_sprite_sprMix *) sprite->sprinfo;
		   sprMix->type_sprite = sprite->type_sprite;
		   sprMix->partsMix = k;
		   for (cnt = 0; cnt < k; cnt++)
		   	sprMix->imageMix[cnt] = -1;
		}   
	   if (createIma) {
	      sprite->imainfo = (void *) malloc(sizeof(_ds_t_sprite_imaMix));
	      _ds_t_sprite_imaMix *imaMix = (_ds_t_sprite_imaMix *) sprite->imainfo;
	      imaMix->type_sprite = sprite->type_sprite;
	      imaMix->partsMix = k;
		   x = 0;
		   y = 0;
		   for (cnt = 0; cnt < k; cnt++) {
		      // Change row?
		      if ((xsize - x) <= 0) {
		         x = 0;
		         y += j;
		      }   
			   // Init elements
			   imaMix->imageIDMix[cnt] = -1;
				imaMix->xposMix[cnt] = x;
				imaMix->yposMix[cnt] = y;
				imaMix->xsizeMix[cnt] = i;
				imaMix->ysizeMix[cnt] = j;
	      	imaMix->cacheMix[cnt] = (u16 *) malloc(ds_util_convertPow2(i) *
	      											ds_util_convertPow2(j) *
	      											sizeof(u16));
				
				// Advance
				x += i;
		   }   	
	   }   		
	}	   
   return 1;
}

void _ds_3dsprite_imaInfo_deleteSpr(_ds_t_sprite_* ptrP, int forceSprite) {
   if (ptrP == NULL)
   	return;
   
	// Check: Is this a normal sprite, or a "mix" sprite?   
	if (ptrP->type_sprite != _DS_C_3D_TYPE_MIX) {
	   _ds_t_sprite_spr* spr = (_ds_t_sprite_spr*) ptrP;
	   
	   // Normal Sprite //
	   if (forceSprite) {
  			PA_3DDeleteSprite(spr->image);    	
  			ds_util_arrNum_Del(sprite3DListArr, spr->image);
  		}			
	} else {
	   _ds_t_sprite_sprMix* sprMix = (_ds_t_sprite_sprMix*) ptrP;
	   
	   // Mix Sprite //
	   int i;
		if (forceSprite) {
		   for (i = 0; i < sprMix->partsMix; i++)	{      
	      	PA_3DDeleteSprite(sprMix->imageMix[i]);
	      	ds_util_arrNum_Del(sprite3DListArr, sprMix->imageMix[i]);
	    	}  	
	   }
	}   
}
   
void _ds_3dsprite_imaInfo_deleteIma(_ds_t_sprite_* ptrP, int forceCache) {
   if (ptrP == NULL)
   	return;
   
	// Check: Is this a normal sprite, or a "mix" sprite?   
	if (ptrP->type_sprite != _DS_C_3D_TYPE_MIX) {
	   _ds_t_sprite_ima* ima = (_ds_t_sprite_ima*) ptrP;
	   
	   // Normal Sprite //
	   if (forceCache) {
	      if (ima->cache != NULL) {
	      	free(ima->cache);
	      	ima->cache = NULL;
	    	}  	
  		}  	
	} else {
	   _ds_t_sprite_imaMix* imaMix = (_ds_t_sprite_imaMix*) ptrP;
	   
	   // Mix Sprite //
	   int i;
	   if (forceCache) {
		   for (i = 0; i < imaMix->partsMix; i++) {
		      if (imaMix->cacheMix[i] != NULL)
		      	free(imaMix->cacheMix[i]);
		      	imaMix->cacheMix[i] = NULL;
		   }  
		}   
	}   
}

/* Deletes the imaInfo (Internal Sprites) structure. Global caches are ALWAYS preserved */   
void _ds_3dsprite_imaInfo_delete(_ds_t_sprite* sprite, int forceSprite, int forceCache) {

	// Delete contents of Ima and Spr
	_ds_3dsprite_imaInfo_deleteIma(sprite->imainfo,forceCache);
	_ds_3dsprite_imaInfo_deleteSpr(sprite->sprinfo,forceSprite);
		   
	// Delete Sprite Pointer
	if (sprite->sprinfo != NULL) {
		free(sprite->sprinfo);
		sprite->sprinfo = NULL;
	}		
	
	// Delete Image Pointer... ONLY IF NOT GLOBAL (As it was assigned in a global way)
	if (!ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL_GLOBAL))
		if (sprite->imainfo != NULL) {
			free(sprite->imainfo);
			sprite->imainfo = NULL;
		}			
}

/* Update the images contained within the imaInfo (Cache) */
void _ds_3dsprite_imaInfo_updateCache(_ds_t_sprite* sprite, u16 *imaParam) {
	u16 *ima;
	if (imaParam == NULL) 
		ima = ds_3dspritehdd_getSprite(sprite->data.bank,sprite->data.obj,sprite->data.frame);
	else 
		ima = imaParam;
	u8 invisible = ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL_INVISIBLE);
	_ds_t_sprite_ *ptr = (_ds_t_sprite_ *) sprite->imainfo;
	if (ptr == NULL)
		return;
	
	// Check: Is this a normal sprite, or a "mix" sprite?   
	if (ptr->type_sprite != _DS_C_3D_TYPE_MIX) {
	   _ds_t_sprite_ima* tex = (_ds_t_sprite_ima*) ptr;
	   
	   // Normal Sprite - //
	   _ds_3dsprite_copy(ima,sprite->data.xsize,sprite->data.ysize,tex->cache,
								ds_util_convertPow2(sprite->data.xsize),ds_util_convertPow2(sprite->data.ysize),
								!invisible,
								1);
	} else {
	   _ds_t_sprite_imaMix* texMix = (_ds_t_sprite_imaMix*) ptr;


	   
	   // Mix Sprite //
	   int i;
	   for (i = 0; i < texMix->partsMix; i++) {
		   _ds_3dsprite_copyEx(ima,sprite->data.xsize,sprite->data.ysize,
		   	texMix->xposMix[i],texMix->yposMix[i],texMix->xsizeMix[i],texMix->ysizeMix[i],
				texMix->cacheMix[i],texMix->xsizeMix[i],texMix->ysizeMix[i],
				!invisible,
				1);	      				
	   }   
	}
}

/* Update the images contained within the imaInfo (Gfx) */
void _ds_3dsprite_imaInfo_updateGfx(_ds_t_sprite* sprite) {
   _ds_t_sprite_ *ptr = (_ds_t_sprite_ *) sprite->imainfo;
	if (ptr == NULL)
		return;
   
	// Check: Is this a normal sprite, or a "mix" sprite?   
	if (ptr->type_sprite != _DS_C_3D_TYPE_MIX) {
	   _ds_t_sprite_ima* tex = (_ds_t_sprite_ima*) ptr;

	   // Normal Sprite - //
	   if (tex->imageID != -1) // avoid special cases
	   	PA_3DUpdateGfx(tex->imageID,tex->cache);  //ptrtex);
	} else {
	   _ds_t_sprite_imaMix* texMix = (_ds_t_sprite_imaMix*) ptr;	   
	   
	   // Mix Sprite //
	   int i;
	   for (i = 0; i < texMix->partsMix; i++) {
	      PA_3DUpdateGfx(texMix->imageIDMix[i],texMix->cacheMix[i]);
		}	
	}	   
}

/* Update the priorities of the sprite */
void _ds_3dsprite_prio(_ds_t_sprite* sprite, int prio) {
	// Check: Is this a normal sprite, or a "mix" sprite?   
	if (sprite->type_sprite == _DS_C_3D_TYPE_NORMAL) {
	   _ds_t_sprite_spr* spr = (_ds_t_sprite_spr*) sprite->sprinfo;
	   if (spr == NULL)
	   	return;
	   
	   // Normal Sprite //
	   sprite->data.prio = prio;
	   PA_3DSetSpritePrio(spr->image, sprite->data.prio);
	   PA_3DSetSpritePolyID(spr->image, sprite->data.prio / (DS_C_PRIO >> 1));
	} else if (sprite->type_sprite == _DS_C_3D_TYPE_MIX) {
	   _ds_t_sprite_sprMix* sprMix = (_ds_t_sprite_sprMix*) sprite->sprinfo;
	   if (sprMix == NULL)
	   	return;
	   
	   // Mix Sprite //
	   sprite->data.prio = prio;
	   int i;
		for (i = 0; i < sprMix->partsMix; i++)	{      
	      PA_3DSetSpritePrio(sprMix->imageMix[i],sprite->data.prio);
	      PA_3DSetSpritePolyID(sprMix->imageMix[i], sprite->data.prio / (DS_C_PRIO >> 1));
	   }
	}  
}

/* Update the alpha of the sprite */
void _ds_3dsprite_alpha(_ds_t_sprite* sprite, int alpha) {
	// Check: Is this a normal sprite, or a "mix" sprite?   
	if (sprite->type_sprite == _DS_C_3D_TYPE_NORMAL) {
	   _ds_t_sprite_spr* spr = (_ds_t_sprite_spr*) sprite->sprinfo;
	   if (spr == NULL)
	   	return;
	   
	   // Normal Sprite //
	   sprite->data.alpha = alpha;
	   PA_3DSetSpriteAlpha(spr->image,((alpha >> 3) == 0)?1:alpha >> 3);
	} else if (sprite->type_sprite == _DS_C_3D_TYPE_MIX) {
	   _ds_t_sprite_sprMix* sprMix = (_ds_t_sprite_sprMix*) sprite->sprinfo;
	   if (sprMix == NULL)
	   	return;
	   
	   // Mix Sprite //
	   sprite->data.alpha = alpha;
	   int i;
		for (i = 0; i < sprMix->partsMix; i++)	{      
		   PA_3DSetSpriteAlpha(sprMix->imageMix[i],((alpha >> 3) == 0)?1:alpha >> 3);
	   }
	}  
}

/* Update the HW position of the sprite */
void _ds_3dsprite_posXY(_ds_t_sprite* sprite, int corrx, int corry, int visible) {
   int x;
   int y;
   
	// Check: Is this a normal sprite, or a "mix" sprite?   
	if (sprite->type_sprite == _DS_C_3D_TYPE_NORMAL) {
	   _ds_t_sprite_spr* spr = (_ds_t_sprite_spr*) sprite->sprinfo;
	   if (spr == NULL)
	   	return;
	   
	   // Normal Sprite //
	   if (visible) {
			x = _ds_3dsprite_realXS(sprite,sprite->data.xpos,sprite->data.xsize) - corrx;
			y = _ds_3dsprite_realYS(sprite,sprite->data.ypos,sprite->data.ysize) - corry;
		} else {
		   x = 300;
		   y = 300;
		}   		
	   PA_3DSetSpriteXY(spr->image,x,y);
	} else if (sprite->type_sprite == _DS_C_3D_TYPE_MIX) {
	   _ds_t_sprite_imaMix* imaMix = (_ds_t_sprite_imaMix*) sprite->imainfo;
		_ds_t_sprite_sprMix* sprMix = (_ds_t_sprite_sprMix*) sprite->sprinfo;
		if (imaMix == NULL)
			return;	   
	   if (sprMix == NULL)
	   	return;
	   
	   // Mix Sprite //
	   int i;
		for (i = 0; i < imaMix->partsMix; i++)	{      
		   if (visible) {
				x = _ds_3dsprite_realXS(sprite,sprite->data.xpos + imaMix->xposMix[i],
												imaMix->xsizeMix[i]) - corrx;
				y = _ds_3dsprite_realYS(sprite,sprite->data.ypos + imaMix->yposMix[i],
												imaMix->ysizeMix[i]) - corry;
			} else {
			   x = 300;
			   y = 300;
			}   		
		   PA_3DSetSpriteXY(sprMix->imageMix[i],x,y);
	   }
	}
}

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Initializes the 3d sprite system - Part 1*/
// Start ALL the sprite subsystems. This process must be done only once.
void ds_3dsprite_init1() {
   int i,j;
   
   // FIRST!!!!!!!!!!!!!!!
	vramSetMainBanks(VRAM_A_TEXTURE,VRAM_B_TEXTURE,VRAM_C_SUB_BG,VRAM_D_MAIN_BG_0x06000000);
		// Changes the way the banks are assigned ;-) 
		// Why? I was not using a VRAM bank! And I need more memory!!!!
		// By setting up the banks this way, I can do PA_Init3D2Banks
		// And have more memory for the background and the sprites! :-D
		// which I need for the alpha blending ;-)
	   
   // Inits the System
   for (i = 0; i < _MAX_OBJSPRITE; i++) {
      spriteList[i] = NULL; 
   }   
   spriteListArr = ds_util_arrNumInit(_MAX_OBJSPRITE);
   
   sprite3DListArr = ds_util_arrNumInit(_MAX_OBJSPRITE);
	
	_spriteLoad = 0;

   for (j = 0; j < DS_C_MAX_BANK; j++) {
	   for (i = 0; i < DS_C_MAX_OBJ; i++) {
	      spriteGlobal[j][i] = NULL;
	      spriteGlobalPainted[j][i] = 0;
  		}   	      
  	}
   for (i = 0; i < DS_C_MAX_OBJ_PART; i++) {
      particleGlobal[i] = NULL;
      particleGlobalPainted[i] = 0;
	}   	      
   for (i = 0; i < DS_C_MAX_OBJ_CO; i++) {
      coGlobal[i] = NULL;
      coGlobalPainted[i] = 0;
	}   	         
   // Inits the 3D System and the 3D Sprites. HDD will be Initialized in part 2
   PA_Init3D2Banks();
   PA_Reset3DSprites2Banks();

}

/* Initializes the 3d sprite system - Part 2*/
// Starts the *subsystems* (all units below this one)
void ds_3dsprite_init2() {
   // Just call to the HDD 3DSprites ^_-
   ds_3dspritehdd_init();
}   
   

/* Initializes the 3d sprite system AFTER loading a game screen */
// Creates the Game Screen
void ds_3dsprite_initAfterGameScreen() {
   // I create the game screen
   _ds_3dsprite_createScreen3D();
}

/* Deletes a single sprite. Note that global caches are preserved. */
// Deletes everything, BUT check the following conditions
// If global, DO NOT delete cache
void ds_3dsprite_deleteSprite(int id) {
   int deletecache;
   int deletesprite;
   
   if (spriteList[id] != NULL) {
      
      // Check conditions
      deletecache = 1;
      if (ds_util_bitOne8(spriteList[id]->flags, _DS_C_3D_SPECIAL_GLOBAL))
      	deletecache = 0;
      	
      deletesprite = 1; // ALWAYS Delete the sprite
      
      // Call internal delete routine - REAL DELETION OF POINTERS IS HERE
      _ds_3dsprite_imaInfo_delete(spriteList[id], deletesprite, deletecache);

    	// Deletes internal stuff (e.g. the global flag :-P)
    	_ds_3dsprite_setGlobalPainted(spriteList[id],0);    	
    	
     	// Free the memory occupied by the sprite
      free(spriteList[id]);
      spriteList[id] = NULL;
      
      // Optimization - Tricks with the list management
      ds_util_arrNum_Del(spriteListArr, id);
   }   
}   

/* Resets the 3d sprite system. 
	force == 1 if we want to delete everything in the subsystems (but not Juni)*/
// Reset EVERYTHING, subsystems (PALib, 3DHDD) included.
// 	force is only used as a parameter for the subsystems (3DHDD)
void ds_3dsprite_reset(int force) {
   int i,j;
   _ds_t_sprite_ *tex;
   
   // Resets the Sprite list
   for (i = 0; i <= ds_util_arrNumMax(spriteListArr); i++) {
      ds_3dsprite_deleteSprite(i); // Deletes the SPRITES (not including global textures, later)
   }   
   
	ds_util_arrNumReset(spriteListArr);
	ds_util_arrNumReset(sprite3DListArr);
	
	_spriteLoad = 0;
   
   // Resets the SpriteGlobal list and the ParticleGlobal list (Including the *Painted lists)
   for (j = 0; j < DS_C_MAX_BANK; j++) {
	   for (i = 0; i < DS_C_MAX_OBJ; i++) {
			tex = (_ds_t_sprite_ *)spriteGlobal[j][i];
      	if (tex != NULL) {
      	   _ds_3dsprite_imaInfo_deleteIma(tex, 1); // GLOBAL CACHES!
	         free(spriteGlobal[j][i]);
   	      spriteGlobal[j][i] = NULL; 		
      	} 	
			spriteGlobalPainted[j][i] = 0;	
      }   
   }   
	for (i = 0; i < DS_C_MAX_OBJ_PART; i++) {
		tex = (_ds_t_sprite_ *)particleGlobal[i];
   	if (tex != NULL) {
   	   _ds_3dsprite_imaInfo_deleteIma(tex, 1); // GLOBAL CACHES!
         free(particleGlobal[i]);
	      particleGlobal[i] = NULL; 		
   	} 	
		particleGlobal[i] = 0;	
   }   
	for (i = 0; i < DS_C_MAX_OBJ_CO; i++) {
		tex = (_ds_t_sprite_ *)coGlobal[i];
   	if (tex != NULL) {
   	   _ds_3dsprite_imaInfo_deleteIma(tex, 1); // GLOBAL CACHES!
         free(coGlobal[i]);
	      coGlobal[i] = NULL; 		
   	} 	
		coGlobal[i] = 0;	
   }   
   
   // Reset the 3D Sprites and the HDD
   PA_Reset3DSprites2Banks(); 
   ds_3dspritehdd_reset(force);
   
   // Reset the HACK for the global sprites
   _hack_glocalSprites = _MAX_OBJSPRITE - 2;
}

/* Updates the image of an sprite - Internal Version */
// - spriteUpdate copy the image to the cache.
// - hardware update actually updates the HW 
// - hardware cache tells the system that the sprite should be updated in the next iteration
// - imaParam is used for special Sprites
void _ds_3dsprite_updateSpriteInner(_ds_t_sprite* sprite, int spriteUpdate, int hardwareUpdate, int hardwareCache, u16 *imaParam) {
	if (sprite != NULL) {
	   if (spriteUpdate) {
	      _ds_3dsprite_imaInfo_updateCache(sprite, imaParam);
			if (hardwareCache) {
			   sprite->flags = ds_util_bitSet8(sprite->flags, _DS_C_3D_SPECIAL_UPDATE);
				_maxtexture++;
			}			
		}			
		if ((hardwareUpdate) && 
			 (ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL_UPDATE))) {
			   _ds_3dsprite_imaInfo_updateGfx(sprite);
			   sprite->flags = ds_util_bitDel8(sprite->flags, _DS_C_3D_SPECIAL_UPDATE);
					 // OK, hardware updated :-D
		}   		
	}			
}   

/* Tell the system to update the image of a certain sprite (via id). Note: IT DOES NOT UPDATE THE HARDWARE, such
	operation is done in "DrawAll" */
// Updates the cache of the system and tells the system to update it later
void _ds_3dsprite_updateSpriteImage(_ds_t_sprite* sprite) {
   
	_ds_3dsprite_updateSpriteInner(sprite,1,0,1,NULL);
}

/* Provides a unique number with error checking */ 
int _ds_3dsprite_getUniqueNumber(void *param, int MAX, char *err) {
	if (ds_util_arrNumElements(param) >= MAX) {
      // Error :-(
      ds_global_errorAssign(DS_C_ERR_NOMOREOBJECTS);
		sprintf(ds_global_string,"In %s",err);
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
		return 0; // avoids warnings
	} else {
	   // Search next "free" position and provides
		return ds_util_arrNum_Get(param);
	}
	return 0; // avoids warnings
}   

/* Load the bank/obj object into certain screen coordinates x/y, returns the id of the specific sprite, -1 if error */
// 1) Load the sprite from HDD
// 2) Creates structure, fills data (including sprite number), inits other elements.
// 3) Creates / Reuses the cache subsystem. Textures CAN WAIT ;-)
// 4) Copy image to the cache
// 5) Creates the Sprite (Sprite + Texture)
//		notea: If cache (with texture) already existed, PA_3DCreateSpriteFromTex
//		noteb: global sprites must be created twice w/PA_3DCreateSpriteFromTex (HACK Vs PALib)
// 6) Final touches: no update, priority, sprite list
int ds_3dsprite_load(u8 bank, u8 obj, int x, int y, int isUnique) {
   _ds_t_sprite* sprite;
   void **ptr;
   int xsize2, ysize2;
   _ds_t_sprite_spr *spr;
   _ds_t_sprite_ima *ima;
   _ds_t_sprite_sprMix *sprMix;
   _ds_t_sprite_imaMix *imaMix;
   int i;
   
   // 0) ASSERTIONS: Only these banks can be global
   if ((bank != DS_C_PART_BANK) && (bank != DS_C_CO_BANK) && (bank > DS_C_MAX_BANK))
   	isUnique = 1;
   
   // 1) Load the sprite from the hdd
   if (!ds_3dspritehdd_load(bank,obj)) {
		sprintf(ds_global_string,"CALL ds_3dspritehdd_load\n(b:%d,o:%d)",bank,obj);
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
   }   
   
   // 2) Create the structure, load a couple of things :-)
   sprite = malloc(sizeof(_ds_t_sprite));
   if (sprite == NULL) {
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		sprintf(ds_global_string,"In ds_3dsprite_load\n(b:%d,o:%d)",bank,obj);
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
   }
	sprite->id = _ds_3dsprite_getUniqueNumber(spriteListArr, _MAX_OBJSPRITE, "ds_3dsprite_load");   
	sprite->imainfo = NULL;
	sprite->sprinfo = NULL;
   sprite->data.bank = bank;
   sprite->data.obj = obj;
   sprite->data.xsize = ds_3dspritehdd_getWidth(bank,obj);
   sprite->data.ysize = ds_3dspritehdd_getHeight(bank,obj);   
   xsize2 = ds_util_convertPow2(sprite->data.xsize);
   ysize2 = ds_util_convertPow2(sprite->data.ysize);
   sprite->data.xpos = x;
   sprite->data.ypos = y;
   sprite->data.alpha = 255; // = 31 for the internal structure of PALIB & NDS
   sprite->data.prio = 0;
   sprite->data.frame = 0;
   sprite->flags = 0; // Init Flags
   if (!isUnique)
   	sprite->flags = ds_util_bitSet8(sprite->flags,_DS_C_3D_SPECIAL_GLOBAL); // Global!   
   
	// 3) Creates/Reuses the cache subsystem
	int createSpr = 1; // Always	
	int createIma = 0; // If Global/NULL or Unique
	ptr = NULL;
	if (!isUnique) {   
	   if (bank == DS_C_PART_BANK)
	   	ptr = &(particleGlobal[obj]);
	   else 
	   if (bank == DS_C_CO_BANK)
	   	ptr = &(coGlobal[obj]);
		else
	   	ptr = &(spriteGlobal[bank][obj]);
	   if ((*ptr) == NULL) {
	      createIma = 1;
		}
	} else {
	   createIma = 1;
	}
	
	if (!_ds_3dsprite_imaInfo_partition(sprite,createSpr,createIma)) { // ***CREATION***
		// Sorry, we can't create a sprite here :_( - but we can still store something...
		sprite->type_sprite = _DS_C_3D_TYPE_NOSPR;
		spriteList[sprite->id] = sprite;
		return sprite->id;
	}	
	
	if ((!isUnique) && (createIma)) { // Put pointer to global IMA
		(*ptr) = sprite->imainfo;
	} else if ((!isUnique) && (!createIma)) { // Get pointer from global IMA
		sprite->imainfo = (*ptr);
	}	
	
	// 4) Copy image to cache
   _ds_3dsprite_updateSpriteInner(sprite,1,0,0,NULL);	
   
   // 5) Creates the sprite(s) itself/themselves
   if ((isUnique) || 					// Unique Sprite
		 ((!isUnique) && (createIma)) // Global, but just created (no texture)
		){
   	if (sprite->type_sprite == _DS_C_3D_TYPE_NORMAL) {
  		   spr = (_ds_t_sprite_spr *) sprite->sprinfo;
  		   ima = (_ds_t_sprite_ima *) sprite->imainfo;

   	   spr->image =
				_ds_3dsprite_getUniqueNumber(sprite3DListArr, _MAX_OBJSPRITE, "ds_3dsprite_load");
			ima->imageID = PA_3DCreateSprite(spr->image,(void*)ima->cache,
									xsize2,ysize2,
									TEX_16BITS,0,
									_ds_3dsprite_realXS(sprite,sprite->data.xpos,sprite->data.xsize),
									_ds_3dsprite_realYS(sprite,sprite->data.ypos,sprite->data.ysize)); // X, Y SPRITE CENTER */
			// "Shadow" sprite - due to PALib management of textures -_-
			if (!isUnique) {
		      PA_3DCreateSpriteFromTex(_hack_glocalSprites,
													ima->imageID,
													xsize2, ysize2,
													0,
													300,300); // X, Y SPRITE CENTER !
				_hack_glocalSprites--;
			}			
		} else {
  		   sprMix = (_ds_t_sprite_sprMix *) sprite->sprinfo;
  		   imaMix = (_ds_t_sprite_imaMix *) sprite->imainfo;		   
  		   
			for (i = 0; i < imaMix->partsMix; i++) {
			   sprMix->imageMix[i] = 
			   	_ds_3dsprite_getUniqueNumber(sprite3DListArr, _MAX_OBJSPRITE, "ds_3dsprite_load");
			   int xmix = _ds_3dsprite_realXS(sprite,sprite->data.xpos + imaMix->xposMix[i],
									imaMix->xsizeMix[i]);
			   int ymix = _ds_3dsprite_realYS(sprite,sprite->data.ypos + imaMix->yposMix[i],
									imaMix->ysizeMix[i]);
				imaMix->imageIDMix[i] = PA_3DCreateSprite(sprMix->imageMix[i],
													(void*)imaMix->cacheMix[i],
													imaMix->xsizeMix[i],
													imaMix->ysizeMix[i],
													TEX_16BITS,0,
													xmix,
													ymix);
				// "Shadow" sprite - due to PALib management of textures -_-
				if (!isUnique) {
			      PA_3DCreateSpriteFromTex(_hack_glocalSprites,
														imaMix->imageIDMix[i],
														imaMix->xsizeMix[i],
														imaMix->ysizeMix[i],
														0,
														300,300); // X, Y SPRITE CENTER !
					_hack_glocalSprites--;
				}			
			}     		   
		}
   } else { // Global but created before - load textures!
   	if (sprite->type_sprite == _DS_C_3D_TYPE_NORMAL) {
  		   spr = (_ds_t_sprite_spr *) sprite->sprinfo;
  		   ima = (_ds_t_sprite_ima *) sprite->imainfo;

   	   spr->image =
				_ds_3dsprite_getUniqueNumber(sprite3DListArr, _MAX_OBJSPRITE, "ds_3dsprite_load");
	      PA_3DCreateSpriteFromTex(spr->image,
												ima->imageID,
												xsize2, ysize2,
												0,
												_ds_3dsprite_realXS(sprite,sprite->data.xpos,sprite->data.xsize),
												_ds_3dsprite_realYS(sprite,sprite->data.ypos,sprite->data.ysize)); 
													// X, Y SPRITE CENTER */
		} else {
  		   sprMix = (_ds_t_sprite_sprMix *) sprite->sprinfo;
  		   imaMix = (_ds_t_sprite_imaMix *) sprite->imainfo;		   
  		   
			for (i = 0; i < imaMix->partsMix; i++) {
			   sprMix->imageMix[i] = 
			   	_ds_3dsprite_getUniqueNumber(sprite3DListArr, _MAX_OBJSPRITE, "ds_3dsprite_load");
			   int xmix = _ds_3dsprite_realXS(sprite,sprite->data.xpos + imaMix->xposMix[i],
									imaMix->xsizeMix[i]);
			   int ymix = _ds_3dsprite_realYS(sprite,sprite->data.ypos + imaMix->yposMix[i],
									imaMix->ysizeMix[i]);
		      PA_3DCreateSpriteFromTex(sprMix->imageMix[i],
													imaMix->imageIDMix[i],
													imaMix->xsizeMix[i],
													imaMix->ysizeMix[i],
													0,
													xmix,
													ymix); // X, Y SPRITE CENTER !
			}     		   
		}
	}
	
	// 6) The final touches! (no update, priority, spritelist)
	_ds_3dsprite_prio(sprite,sprite->data.prio);
	spriteList[sprite->id] = sprite;
		// No need to update, PAlib routines automatically updates the HW upon TEX creation
										
	// Z) Sayonara...
	return sprite->id;
}

/* Loads an "special" sprite (no bank no obj), where the texture is already given by the user. 
	NOTE: Changes in the texture during execution WILL NOT change the 3D sprite! */
// 1) Creates structure, fills data (including sprite number), inits other elements.
// 2) Creates / Reuses the cache subsystem. Textures CAN WAIT ;-)
// 3) Copy image to the cache - *literally*
// 4) Tell the system is a _DS_C_3D_SPECIAL_LOAD (create later)
// 5) Final touches: no update, priority, sprite list
int ds_3dsprite_loadSpecial(u16 *texSpecial, int xs, int ys, int x, int y) {
   _ds_t_sprite* sprite;
   int xsize2, ysize2;
      
   // 1) Create the structure, load a couple of things :-)
   sprite = malloc(sizeof(_ds_t_sprite));
   if (sprite == NULL) {
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		sprintf(ds_global_string,"In ds_3dsprite_loadSpecial\n");
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
   }
	sprite->id = _ds_3dsprite_getUniqueNumber(spriteListArr, _MAX_OBJSPRITE, "ds_3dsprite_load");   
	sprite->imainfo = NULL;
	sprite->sprinfo = NULL;
   sprite->data.xsize = xs;
   sprite->data.ysize = ys;
   xsize2 = ds_util_convertPow2(sprite->data.xsize);
   ysize2 = ds_util_convertPow2(sprite->data.ysize);
   sprite->data.xpos = x;
   sprite->data.ypos = y;
   sprite->data.bank = 0;
   sprite->data.obj = 0;
   sprite->data.alpha = 255; // = 31 for the internal structure of PALIB & NDS
   sprite->data.prio = 0;
   sprite->data.frame = 0;
   sprite->flags = 0; // Init Flags
	sprite->flags = ds_util_bitSet8(sprite->flags, _DS_C_3D_SPECIAL); // Special!   
	
	// 2) Creates the cache subsystem
	int createSpr = 1; // Always	
	int createIma = 1; // Always, here
	if (!_ds_3dsprite_imaInfo_partition(sprite,createSpr,createIma)) { // ***CREATION***
		// Here, any forbidden partition will cause an error :-)
      ds_global_errorAssign(DS_C_ERR_);
		sprintf(ds_global_string,"In ds_3dsprite_loadSpecial\nPartition");
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
	}	
	
	// 3) Copy image to cache, *literally*... So remember, special caches can be deleted!
   _ds_3dsprite_updateSpriteInner(sprite,1,0,0,texSpecial);	
   
	// 4) Should create the HW texture... directly from the image... and the sprite too!
	// ... BUT!!!!!... We will delay its uploading to after the VBlank
	sprite->flags = ds_util_bitSet8(sprite->flags, _DS_C_3D_SPECIAL_LOAD); // Special!
	_spriteLoad++;
	
	// 5) The final touches! (update, priority, spritelist)
	spriteList[sprite->id] = sprite;
	sprite->flags = ds_util_bitSet8(sprite->flags, _DS_C_3D_SPECIAL_UPDATE);
										
	// Z) Sayonara...
	return sprite->id;
}


/* "Cleans" the HW Sprite system, in case there are HW sprites around */
// Just call to the internal 3D system (PALib)
void ds_3dsprite_cleanHWSprites() {
   // This is a small "hack" for deleting the HW sprites that may be shown in the screen 
   PA_3DProcess();
}      

/* Change the obj stored inside a sprite 
      - At present it only works with the Juni-related images */
// Does not work with "Mix"/Special images
// Considers that the image has been already loaded
// 	(that's one of the reasons it only works with Juni-related imas :-P)
void ds_3dsprite_changeObj(int id, u8 obj) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   if (ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL))
   		return;
   	if (sprite->type_sprite != _DS_C_3D_TYPE_NORMAL)
   		return;
   		
	   sprite->data.obj = obj;
	   sprite->data.frame = 0;
	   _ds_3dsprite_updateSpriteImage(sprite);
	}   
}   

/* Change the bank/obj stored inside a sprite 
      - It loads from the HDD if needed */
// NOTE: Consider the special sprites
// NOTE2: Does not work with "Mix" sprites
void ds_3dsprite_changeBankObj(int id, u8 bank, u8 obj) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   if (ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL))
   		return;
   	if (sprite->type_sprite != _DS_C_3D_TYPE_NORMAL)
   		return;

   	sprite->data.bank = bank;
	   sprite->data.obj = obj;
	   if (!ds_3dspritehdd_load(sprite->data.bank,sprite->data.obj)) {
	      // AUCH!!!!
			sprintf(ds_global_string,"CALL ds_3dspritehdd_load\n(b:%d,o:%d)",bank,obj);
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
	   }   
	   sprite->data.frame = 0;
	   _ds_3dsprite_updateSpriteImage(sprite);
	}   
}   

/* Obtains a pointer to the sprite number "id" frame "frame" */
u16 *ds_3dsprite_getSpriteFrame(int id, int frame) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return NULL;
		
   return ds_3dspritehdd_getSprite(sprite->data.bank,sprite->data.obj,frame);
}

/* Obtains a pointer to the sprite number "id", with the actual frame*/
u16 *ds_3dsprite_getSprite(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return NULL;
		
   return ds_3dspritehdd_getSprite(sprite->data.bank,sprite->data.obj,sprite->data.frame);
}

/* Returns the ABSOLUTE X-coordinate of the sprite number "id" */
int ds_3dsprite_getX(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return -1;

   return sprite->data.xpos;
}   

/* Returns the ABSOLUTE Y-coordinate of the sprite number "id" */
int ds_3dsprite_getY(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return -1;

   return sprite->data.ypos;
}

/* Returns the SIZE X of the sprite number "id" */
int ds_3dsprite_getXSize(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return 0;

   return sprite->data.xsize;
}   

/* Returns the SIZE Y of the sprite number "id" */
int ds_3dsprite_getYSize(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return 0;

   return sprite->data.ysize;
}


/* Sets the ABSOLUTE X-coordinate of the sprite number "id" */
void ds_3dsprite_setX(int id, int x) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL)  {
	   sprite->data.xpos = x;
	}		
}   

/* Sets the ABSOLUTE Y-coordinate of the sprite number "id" */
void ds_3dsprite_setY(int id, int y) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   sprite->data.ypos = y;
	}		
}

/* Sets the ABSOLUTE X/Y-coordinate of the sprite number "id" 
	applying corrections using a certain bit array.
	This is a HACK for special CO that will only affect Juni. */
void ds_3dsprite_setXY_HackCO(int id, int x, int y, void *flagHVoid, int flagSX, int flagSY) {
   ds_t_mapFlag * flagH = (ds_t_mapFlag *) flagHVoid;
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL)  {
	   // First, UPDATE
	   _ds_3dsprite_updateSpriteImage(sprite);
	   // Now, draw
	   sprite->data.xpos = x;
	   sprite->data.ypos = y;
	   _ds_t_sprite_ima* ima = (_ds_t_sprite_ima*) sprite->imainfo;
	   if ((sprite->type_sprite == _DS_C_3D_TYPE_NORMAL) && (ima != NULL)) {
	      // Changes cache according to the flags
	      int i,j;
	      for (j = 0; j < sprite->data.ysize; j++) {
	  	      for (i = 0; i < sprite->data.xsize; i++) {
	  	         int valFlagH = (x + i) + ((y + j) * flagSX);
	  	         	// NOTE that FlagH can be actually outside the screen...
	  	         	//  -> that is, the flagH read can be out of bounds <-
	  	         	// ...but what we read (garbage) is not going to be shown
	  	         	// on the screen, so... ;-)
	  	         int valOBJ = i + j * ds_util_convertPow2(sprite->data.xsize);
	  	         if (ds_util_bitOne8((*flagH)[valFlagH >> 3],valFlagH & 0x07)) {
	  	            ima->cache[valOBJ] = 0;
	  	         }   	
		      }   
	      }   
	   }   
	}		
}   

/* Returns the actual Frame of a certain sprite */
int ds_3dsprite_getFrame(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return -1;

   return sprite->data.frame;
}   

/* Returns the maximum number of frames of a certain sprite */
int ds_3dsprite_getMaxFrame(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return 0;
		
	return ds_3dspritehdd_getFrames(sprite->data.bank,sprite->data.obj);
}   

/* Returns the first frame of a certain sprite - if the sprite stores both left and right */
int ds_3dsprite_getIniFrame_LR(int sprite, int dir, int face) {
	if ((dir == 1) || (!face))
		return 0; // First sprite of the right
	else   
		return ds_3dsprite_getMaxFrame(sprite) >> 1; // First sprite of the left
}   

/* Returns whether the sprite looks @ the right or not */
int ds_3dsprite_lookRight_LR(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   return (sprite->data.frame < (ds_3dsprite_getMaxFrame(id) >> 1));
   }   
   return 1;
}   

/* Returns the last frame (not included) of a certain sprite - if the sprite stores both left and right */
int ds_3dsprite_getMaxFrame_LR(int sprite, int dir, int face) {
	if ((dir == 1) || (!face))
		return ds_3dsprite_getMaxFrame(sprite) >> 1; // Last sprite of the right (NOT included)
	else   
		return ds_3dsprite_getMaxFrame(sprite); // Last sprite of the left (NOT included)
}

/* Sets the actual Frame of a certain sprite */
// Note: Does not work with "special" images
void ds_3dsprite_setFrame(int id, int frame) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   if (ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL))
   		return;

   	sprite->data.frame = frame;
   	
   	if (!ds_util_bitOne8(sprite->flags,_DS_C_3D_SPECIAL_GLOBAL)) {
   	   // Non-global sprites always need to be updated
			_ds_3dsprite_updateSpriteImage(sprite); // Update Cache Image!
		} else {
		   // Check if we need to update this sprite...
		   if (!_ds_3dsprite_getGlobalPainted(sprite)) {
		      _ds_3dsprite_setGlobalPainted(sprite,1); // We painted one global sprite!  
			   _ds_3dsprite_updateSpriteImage(sprite); // Update Cache Image!
		   } 
		}   			
   }   
}

/* Forces the priority of a certain sprite */
void ds_3dsprite_setPrio(int id, int prio) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   if (!ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL_LOAD)) {
	      _ds_3dsprite_prio(sprite,prio);
  		} else {		
  		   sprite->data.prio = prio;
		  	sprite->flags = ds_util_bitSet8(sprite->flags,_DS_C_3D_SPECIAL_PRIO);
	   }	  
   }   
}

/* Sets the alpha of a certain sprite */
void ds_3dsprite_setAlpha(int id, int alpha) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
		if (!ds_util_bitOne8(sprite->flags,_DS_C_3D_SPECIAL_LOAD)) {
		   _ds_3dsprite_alpha(sprite,alpha);
				// real alpha should be at least 1! :-P
		} else {
		   sprite->data.alpha = alpha;
		   sprite->flags = ds_util_bitSet8(sprite->flags,_DS_C_3D_SPECIAL_ALPHA);
		}   		
 	}  	
}   

/* Gets the alpha of a certain sprite (0..255) */
u8 ds_3dsprite_getAlpha(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   return sprite->data.alpha;
 	}  	
 	return 255;
}   

/* Marks an sprite as invisible or visible */
void ds_3dsprite_markInvisible(int id, int invisible) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   if (invisible)
	   	sprite->flags = ds_util_bitSet8(sprite->flags, _DS_C_3D_SPECIAL_INVISIBLE);
	   else
	   	sprite->flags = ds_util_bitDel8(sprite->flags, _DS_C_3D_SPECIAL_INVISIBLE);
 	}  	
}

/* Gets "invisible" state */
int ds_3dsprite_getInvisible(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   return ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL_INVISIBLE);
 	}  	
 	return 0;
}

/* Checks if the sprite contains a HW sprite */
int ds_3dsprite_hasHWSprite(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   return (sprite->type_sprite != _DS_C_3D_TYPE_NOSPR);
 	}  	
 	return 0;
}   

/* "Draws" all the sprites into the screen */
// Remember the _DS_C_3D_SPECIAL_LOAD case!
void ds_3dsprite_drawAll(int camX, int camY) {
   int i;
   int corrx;
   int corry;
   int _timer;
   _ds_t_sprite* sprite;

#ifdef DEBUG_KSDS         
   sprintf(ds_global_string,"3DDraw-Time: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
   ds_gamestatus_debugOutput(1,0,0,ds_global_string,DS_C_STA_DEBUG);
#endif
   // Just updates the 3D sprites ;-)
   for (i = 0; i <= ds_util_arrNumMax(spriteListArr); i++) {
      sprite = spriteList[i];
		if (sprite != NULL) {
		   if (!ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL)) {
		      corrx = camX;
		      corry = camY;
		   } else {
		      corrx = 0;
		      corry = 0;
		   }   
		   if (!ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL_INVISIBLE))
	   		_ds_3dsprite_posXY(sprite,corrx,corry,1); // Visible, with corrections
	   	else
	   		_ds_3dsprite_posXY(sprite,0,0,0); // Invisible = not seen :-)	   	
		}   
	}  
	
	// IDEA: Paint, before, and after the VBLANK :-). Use some registers (REG_VCOUNT?)

#ifdef DEBUG_KSDS			
   sprintf(ds_global_string,"UPD3D-Time: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());			
   ds_gamestatus_debugOutput(1,0,1,ds_global_string,DS_C_STA_DEBUG);
#endif   

   PA_3DProcess();
	
#ifdef DEBUG_KSDS   
   sprintf(ds_global_string,"3DPro-Time: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
   ds_gamestatus_debugOutput(1,0,2,ds_global_string,DS_C_STA_DEBUG);
#endif
      
   // <OPT> Do some optimizations before the VBlank, WAIT for VBlank
   if (ds_global_optimizationPreload) {
	   while (PA_GetVcount() > (192 - 48));
	   _timer = 2; // Safe mark
	   while ((PA_GetVcount() < ((192 - 48) - (_timer*2))) && (ds_submap_inOpt())) {
	      _timer = PA_GetVcount();
		   ds_submap_doOpt();
		   _timer = PA_GetVcount() - _timer;
		}
		while (PA_GetVcount() < (192 - 48 + 4)) { 
		}  	   
	} else {
	   while (PA_GetVcount() > (192 - 48)) { 
		}
	   while (PA_GetVcount() < (192 - 48 + 4)) { 
		}
	}   	
	
   // Now... paint map!!!!!!
#ifdef DEBUG_KSDS
   sprintf(ds_global_string,"3DPro-End: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());			
   ds_gamestatus_debugOutput(1,0,3,ds_global_string,DS_C_STA_DEBUG);
#endif
	
   _PA_3DUpdateGfxScreen3D(_screen3DTex, ds_global_map.tileMap, camX, camY, 1);
	
#ifdef DEBUG_KSDS
	sprintf(ds_global_string,"MapA-Time: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());       		
	ds_gamestatus_debugOutput(1,0,4,ds_global_string,DS_C_STA_DEBUG);
#endif
      
   // FIXME - reUpdate the 3D sprites, but only for a limited time ;-)
	int sprite3DVactual;
	int sprite3DVcount = PA_GetVcount();
	
   int updated = 0;
   for (i = 0; i <= ds_util_arrNumMax(spriteListArr); i++) {      
      sprite = spriteList[i];
		if (sprite != NULL) {
		   if (ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL_UPDATE)) { // Speed up things ;-)
		   	// SPECIAL Update
		   	if (ds_util_bitOne8(sprite->flags, _DS_C_3D_SPECIAL_LOAD)) {
		   	   // Creates sprite!
			   	if (sprite->type_sprite == _DS_C_3D_TYPE_NORMAL) {
			  		   _ds_t_sprite_spr *spr = (_ds_t_sprite_spr *) sprite->sprinfo;
			  		   _ds_t_sprite_ima *ima = (_ds_t_sprite_ima *) sprite->imainfo;
					   if (spr == NULL)
					   	continue;
					   if (ima == NULL)
					   	continue;
			
			   	   spr->image =
							_ds_3dsprite_getUniqueNumber(sprite3DListArr, _MAX_OBJSPRITE, "ds_3dsprite_load");
			   	   int xsize2, ysize2;
		   	      xsize2 = ds_util_convertPow2(sprite->data.xsize);
					   ysize2 = ds_util_convertPow2(sprite->data.ysize);
						ima->imageID = PA_3DCreateSprite(spr->image,(void*)ima->cache,
												xsize2,ysize2,
												TEX_16BITS,0,
												_ds_3dsprite_realXS(sprite,sprite->data.xpos,sprite->data.xsize),
												_ds_3dsprite_realYS(sprite,sprite->data.ypos,sprite->data.ysize)); // X, Y SPRITE CENTER */
					} else {
			  		   _ds_t_sprite_sprMix *sprMix = (_ds_t_sprite_sprMix *) sprite->sprinfo;
			  		   _ds_t_sprite_imaMix *imaMix = (_ds_t_sprite_imaMix *) sprite->imainfo;		   
					   if (sprMix == NULL)
					   	continue;
					   if (imaMix == NULL)
					   	continue;			  		   
					   	
						int i;
						for (i = 0; i < imaMix->partsMix; i++) {
						   sprMix->imageMix[i] = 
						   	_ds_3dsprite_getUniqueNumber(sprite3DListArr, _MAX_OBJSPRITE, "ds_3dsprite_load");
						   int xmix = _ds_3dsprite_realXS(sprite,sprite->data.xpos + imaMix->xposMix[i],
												imaMix->xsizeMix[i]);
						   int ymix = _ds_3dsprite_realYS(sprite,sprite->data.ypos + imaMix->yposMix[i],
												imaMix->ysizeMix[i]);
							imaMix->imageIDMix[i] = PA_3DCreateSprite(sprMix->imageMix[i],
																(void*)imaMix->cacheMix[i],
																imaMix->xsizeMix[i],
																imaMix->ysizeMix[i],
																TEX_16BITS,0,
																xmix,
																ymix);
						}     		   
					}		   	   
					// Update flags and alpha/priority		   	   
					sprite->flags = ds_util_bitDel8(sprite->flags, _DS_C_3D_SPECIAL_LOAD);
					_spriteLoad--;
					if (ds_util_bitOne8(sprite->flags,_DS_C_3D_SPECIAL_ALPHA)) {
					   int alphaint = sprite->data.alpha;
					   ds_3dsprite_setAlpha(sprite->id,alphaint);
					   sprite->flags = ds_util_bitDel8(sprite->flags, _DS_C_3D_SPECIAL_ALPHA);
					}   
					if (ds_util_bitOne8(sprite->flags,_DS_C_3D_SPECIAL_PRIO)) {
					   ds_3dsprite_setPrio(sprite->id,sprite->data.prio);
					   sprite->flags = ds_util_bitDel8(sprite->flags, _DS_C_3D_SPECIAL_PRIO);
					}   
		 		} else {
		 		   // FIRST! If I am a global sprite, nullify other updates from other sprites!
					if (ds_util_bitOne8(sprite->flags,_DS_C_3D_SPECIAL_GLOBAL))
		 		   	_ds_3dsprite_setGlobalPainted(sprite,0); // Clean flags, for next iteration!
		   		// Now, Normal Update
					_ds_3dsprite_updateSpriteInner(sprite,0,1,0,NULL);
				}			
				updated++;
				
				// Special test: If we have no time left, leave the updates for the next frame!
				if (!_spriteLoad) { // Only if there is no LOAD sprites left
					sprite3DVactual = PA_GetVcount();
					// Special fix for 0...262
					if (sprite3DVactual < sprite3DVcount)
						sprite3DVactual += 262;
					// Update only for a limited time (the value has been obtained empirically :-P)
					if ((sprite3DVactual - sprite3DVcount) >= 16)
						break; // Leave to the next frame!. It is OK, flags are not deleted ;-)
				}
			}			
		}   
	}  
	
	// Also, paints upper screen
#ifdef DEBUG_KSDS
	sprintf(ds_global_string,"VRAMUp-Time: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());       		
	ds_gamestatus_debugOutput(1,0,5,ds_global_string,DS_C_STA_DEBUG);
#endif
	
	ds_gamestatus_updateScreen();

#ifdef DEBUG_KSDS
   sprintf(ds_global_string,"GameS-Time: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());       		
   ds_gamestatus_debugOutput(1,0,6,ds_global_string,DS_C_STA_DEBUG);
#endif

	// Finishes...
#ifdef DEBUG_KSDS
	sprintf(ds_global_string,"Updated: [%d] %d (%d)          ",ds_util_arrNumMax(spriteListArr),updated,_maxtexture); 
	ds_gamestatus_debugOutput(1,0,7,ds_global_string,DS_C_STA_DEBUG);
	sprintf(ds_global_string,"DEBUG VRAM: [%d]",_vramSize3D); 
	ds_gamestatus_debugOutput(1,0,8,ds_global_string,DS_C_STA_DEBUG);	
#endif
	_maxtexture = 0;
}
   
/* Tells the 3DSprite system how muh memory we will need for the objects on this screen */ 
void ds_3dsprite_setObjVRAM(int size) {
   /*
   	This function requires an explanation. The amount of VRAM in the DS is limited, 
   	and there are some critters who will use a lot of extra VRAM due to the 
		"all 3D sprites must be square" requirement.
		The *mix* mechanism was used to avoid these situations, but all sprites smaller than
		64x64 (e.g. the croc) will use a whole 64x64 texture for themselves.
		This value is used to avoid such situations. If it is bigger than a certain threshold,
		all sprites bigger than 32x32 will use the *mix* approach with 16x16 textures.
		Of course, this mechanism "as is" is not perfect: it does not consider CO big sprites
		and GLOBAL sprites (I should have a special function for returning whether a sprite
		is global or not). Still, the only KS levels who will use this mechanism are those
		levels with plenty of overhead, and the optimization is worth.
   */
	_vramSize3D = size;  
}
