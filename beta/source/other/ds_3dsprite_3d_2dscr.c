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

#include <PA9.h>       // Include for PA_Lib
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
*/
//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

// Max number of objects/sprites in the screen
#define _MAX_OBJSPRITE 1024

// Flags for the sprites
#define _DS_C_ALPHA_SPECIAL 0
#define _DS_C_ALPHA_LOADED 1

typedef struct _ds_tt_sprite_ima {
   u16 *cache; // cache for the 3Dsprite. NULL for global sprites   
   int imageID;  // associated texture to the sprite / ID of the list of global sprites
} _ds_t_sprite_ima;

typedef struct _ds_tt_sprite {
   _ds_t_sprite_ima imainfo; // Image Information   
   int image;  // which sprite we are managing here
   u8 xsize;
   u8 ysize;   // Size of the sprite   
   int xpos;
   int ypos;   // position of the sprite
   u8 bank;
   u8 obj;     // bank and typem of the object
   u8 frame;   // actual frame of this sprite
   u8 alpha;   // Alpha value
   int update;  // Whether the image(texture) should be updated or not, IN HARDWARE (VRAM)
   u8 invisible;// If the sprite is drawn on the screen or not
   u8 flags; // Flags managing special (e.g. alpha/background) sprites 
} _ds_t_sprite;  

// Store pointers to all the sprites used in one screen. No linked lists here :-P
_ds_t_sprite* spriteList[_MAX_OBJSPRITE];
int spriteListCnt; // Actual position of the list
int spriteListNum; // Number of sprites
int spriteListMax; // Maximum position with a sprite (*including* the last one)
// Store pointers to all the "global sprites" structure. Also, no linked lists here ;-)
_ds_t_sprite_ima* spriteGlobal[DS_C_MAX_BANK][DS_C_MAX_OBJ];

const unsigned char _texture[10 * 64 * 64 * 2] __attribute__ ((aligned (4)));
int _maxtexture = 0;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Copy an image to a certain "cache" buffer. "Cache" buffer should be always bigger */
void _ds_3dsprite_copy(u16 * sbuffer, int sx, int sy, u16 * dbuffer, int dx, int dy, int copy, int prefill) {
   int i, j;
   // First, fill destination buffer with the "transparent" color.
   if (prefill)
   	for (i = 0; i < (dx * dy); i++)
   		dbuffer[i] = 0; // NOTA!!! - El color transparente en la estructura de PALib es 0
   	
   // Now, Copy        
   if (copy) {
	   DC_FlushAll();  // <TODO> DC_FlushRange(src,len); - DC_InvalidateRange(dst,len);?         
	   for (j = 0; j < sy; j++) {
		   DMA_Copy(((sbuffer) + (j*sx)),((dbuffer) + (j*dx)),sx >> 1,DMA_32NOW);
		   //memcpy(((dbuffer) + (j*dx)),((sbuffer) + (j*sx)),sx << 1);
		}   
	}   
}   

/* Returns the "real" X position of a sprite */
int _ds_3dsprite_realX(_ds_t_sprite* sprite, int x) {
   return (x + (ds_util_convertPow2(sprite->xsize) >> 1));
}   

/* Returns the "real" Y position of a sprite */
int _ds_3dsprite_realY(_ds_t_sprite* sprite, int y) {
   return (y + (ds_util_convertPow2(sprite->ysize) >> 1));
}   

/* Returns the real "Tex" of this sprite (unique/shared) */
_ds_t_sprite_ima* _ds_3dsprite_obtainTex(_ds_t_sprite* sprite) {
   if (sprite->imainfo.cache != NULL) {
      return &(sprite->imainfo);
   } else {
      return spriteGlobal[sprite->bank][sprite->obj];
	}     
	return NULL; // Should never be here... just to avoid warnings :-) 
}   

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Initializes the 3d sprite system */
void ds_3dsprite_init() {
   int i,j;
   
   // FIRST!!!!!!!!!!!!!!!
	vramSetMainBanks(VRAM_A_TEXTURE,VRAM_B_TEXTURE,VRAM_C_SUB_BG,VRAM_D_MAIN_BG_0x06000000);
		// Changes the way the banks are assigned ;-) 
		// Why? I was not using a VRAM bank! And I need more memory!!!!
		// By setting up the banks this way, I can do PA_Init3D2Banks
		// And have more memory for the sprites! :-D
		// which I need for the alpha blending
   
   // Inits the System
   for (i = 0; i < _MAX_OBJSPRITE; i++) {
      spriteList[i] = NULL; 
   }   
   spriteListCnt = 0;
   spriteListNum = 0;
   spriteListMax = -1;
   for (j = 0; j < DS_C_MAX_BANK; j++) {
	   for (i = 0; i < DS_C_MAX_OBJ; i++) {
	      spriteGlobal[j][i] = NULL;
  		}   	      
  	}
   
   // Inits the 3D System and the 3D Sprites and the HDD
   PA_Init3D2Banks();
   PA_Reset3DSprites(); 
   ds_3dspritehdd_init();
}   

/* Deletes a single sprite. If force == 1, also delete the HW sprite */
void ds_3dsprite_deleteSprite(int id, int force) {
   if (spriteList[id] != NULL) {
      // Deletes the cache image of the sprite. Particles stay (sure will be used again ^_- in this room)
      if (spriteList[id]->imainfo.cache != NULL) {
      	free(spriteList[id]->imainfo.cache);
      	spriteList[id]->imainfo.cache = NULL;
    	}  	
    	// Deletes the sprite image
     	if (force)
     		PA_3DDeleteSprite(spriteList[id]->image);    	
     	// Free the memory occupied by the sprite
      free(spriteList[id]);
      spriteList[id] = NULL;
      // Optimization - Tricks with the list management
		spriteListCnt = (id < spriteListCnt)?id:spriteListCnt; // Assuring a free place for next sprite :-D
		spriteListNum--;    // Normal, one sprite got away :-P
		//spriteListMax  - no need to update this one, there are NULL checks around the place :-)      
   }   
}   

/* Resets the 3d sprite system. 
	force == 1 if we want to delete everything in the subsystems (but not Juni)*/
void ds_3dsprite_reset(int force) {
   int i,j;
   _ds_t_sprite_ima* tex;
   
   // Resets the Sprite list
   for (i = 0; i <= spriteListMax; i++) {
      ds_3dsprite_deleteSprite(i,0);
   }   
   spriteListCnt = 0;
   spriteListNum = 0;
   spriteListMax = -1;
   
   // Resets the SpriteGlobal list
   for (j = 0; j < DS_C_MAX_BANK; j++) {
	   for (i = 0; i < DS_C_MAX_OBJ; i++) {
			tex = spriteGlobal[j][i];
      	if (tex != NULL) {
         	if (tex->cache != NULL) {
         		free(tex->cache);
         		tex->cache = NULL;
       		}  	
	         free(spriteGlobal[j][i]);
   	      spriteGlobal[j][i] = NULL; 		
      	} 		
      }   
   }   
   
   // Reset the 3D Sprites and the HDD
   PA_Reset3DSprites(); 
   ds_3dspritehdd_reset(force);
}

/* Updates the image of an sprite - Internal Version */
void _ds_3dsprite_updateSpriteInner(_ds_t_sprite* sprite, _ds_t_sprite_ima* tex, int spriteUpdate, int hardwareUpdate, int hardwareCache) {
	u16 *ima;
	void *ptrtex;

	if (sprite != NULL) {
	   if (spriteUpdate) {
	   	ima = ds_3dspritehdd_getSprite(sprite->bank,sprite->obj,sprite->frame);
		   _ds_3dsprite_copy(ima,sprite->xsize,sprite->ysize,tex->cache,
									ds_util_convertPow2(sprite->xsize),ds_util_convertPow2(sprite->ysize),
									!sprite->invisible,
									1);
			if (hardwareCache) {
// <TODO> - Special cache...
//				ptrtex = (void *) &_texture + (64 * 64 * 2 * _maxtexture);
//				PA_OutputText(1,0,3,"%d %d %d",sprite->bank,sprite->obj,_maxtexture);
//				memcpy(ptrtex,tex->cache,ds_util_convertPow2(sprite->xsize)*ds_util_convertPow2(sprite->ysize)*2);
				sprite->update = _maxtexture; // Hardware needs to be updated...
				_maxtexture++;
			}			
		}			
		if ((hardwareUpdate) && (sprite->update != -1)) {
		   ptrtex = (void *) &_texture + (64 * 64 * 2 * sprite->update);
		   PA_3DUpdateGfx(tex->imageID,tex->cache);  //ptrtex);
		   sprite->update = -1; // OK, hardware updated :-D
		}   		
	}			
}   

/* Tell the system to update the image of a certain sprite (via id). Note: IT DOES NOT UPDATE THE HARDWARE, such
	operation is done in "DrawAll" */
void _ds_3dsprite_updateSpriteImage(_ds_t_sprite* sprite) {
   _ds_t_sprite_ima* tex;
   tex = _ds_3dsprite_obtainTex(sprite);
   
	_ds_3dsprite_updateSpriteInner(sprite,tex,1,0,1); // FIXME - Animations Galore!!!!!!!
}   


/* Load the bank/obj object into certain screen coordinates x/y, returns the id of the specific sprite, -1 if error */
int ds_3dsprite_load(u8 bank, u8 obj, int x, int y, int isUnique) {
   _ds_t_sprite* sprite;
   _ds_t_sprite_ima* tex;
   int xsize2, ysize2;
   int arrpos;
   
   // 1) Load the sprite from the hdd
   if (!ds_3dspritehdd_load(bank,obj)) {
      // Error :-(
      return -1;
   }   
   
   // 2) Create the structure, load a couple of things :-)
   sprite = malloc(sizeof(_ds_t_sprite));
   if (sprite == NULL) {
      // Error :-(
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return -1;
   }   
   sprite->xsize = ds_3dspritehdd_getWidth(bank,obj);
   sprite->ysize = ds_3dspritehdd_getHeight(bank,obj);
   xsize2 = ds_util_convertPow2(sprite->xsize);
   ysize2 = ds_util_convertPow2(sprite->ysize);
   sprite->xpos = x;
   sprite->ypos = y;
   sprite->bank = bank;
   sprite->obj = obj;
   sprite->alpha = 255; // = 31 for the internal structure of PALIB & NDS
   sprite->frame = 0;
   sprite->invisible = 0; // On creation, we should see this sprite
   sprite->flags = 0; // No flags on the beginning
	if (spriteListNum == _MAX_OBJSPRITE) {
      // Error :-(
      ds_global_errorAssign(DS_C_ERR_NOMOREOBJECTS);
      return -1;
	} else {
	   // Search next "free" position
	   arrpos = spriteListCnt % _MAX_OBJSPRITE;
	   while (spriteList[arrpos] != NULL) {
	      arrpos = (arrpos + 1) % _MAX_OBJSPRITE;
	   }   
	   // Store information
		sprite->image = arrpos;
		spriteListMax = (arrpos > spriteListMax)?arrpos:spriteListMax;
		spriteListCnt = (arrpos + 1) % _MAX_OBJSPRITE;
		spriteListNum++;
		// Optimization - search for a better (lower) spriteListCnt
		if (PA_RandMax(300) == 1) {
		   for (arrpos = 0; arrpos < spriteListCnt; arrpos++) {
		      if (spriteList[arrpos] == NULL) {
		         spriteListCnt = arrpos;
		         break;
		      }   
		   }   
		}   
	}   
	sprite->imainfo.cache = NULL;
	sprite->imainfo.imageID = -1;
	
	// 2a) Creates the HW Texture... but if it is not unique, then store in the spriteGlobal -- <TODO> Simplify
	if (isUnique) {
	   tex = &(sprite->imainfo);
	   tex->cache = malloc((xsize2 * ysize2) * 2); //16Bit
   	if (tex->cache == NULL) {
      	// Error :-(
	      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
   	   return -1;
	   }   
   	_ds_3dsprite_updateSpriteInner(sprite,tex,1,0,0);
	   tex->imageID = PA_3DCreateTex((void*)tex->cache,
									xsize2,ysize2,
									TEX_16BITS);
	} else {
	   if (spriteGlobal[bank][obj] == NULL) {
	      tex = malloc(sizeof(_ds_t_sprite_ima));
		   tex->cache = malloc((xsize2 * ysize2) * 2); //16Bit
   		if (tex->cache == NULL) {
	      	// Error :-(
		      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
   		   return -1;
		   }
	   	_ds_3dsprite_updateSpriteInner(sprite,tex,1,0,0);
		   tex->imageID = PA_3DCreateTex((void*)tex->cache,
										xsize2,ysize2,
										TEX_16BITS);
			spriteGlobal[bank][obj] = tex;
	   } else {
	      tex = spriteGlobal[bank][obj]; 
	   }   
	}   
	// 3) The final touch! - Creates the HW sprite
	PA_3DCreateSpriteFromTex(sprite->image, 					// Sprite number
										tex->imageID, 			// Gfx...
										xsize2, ysize2,
										0,   		// Palette, unused in 16bit...
										_ds_3dsprite_realX(sprite,sprite->xpos), _ds_3dsprite_realY(sprite,sprite->ypos)); // X, Y SPRITE CENTER !
	sprite->update = -1; // No need to have this flag active now, 3DCreateSpriteFromTex updates the HW -:)
	
	// 4) Add to the list...
	spriteList[sprite->image] = sprite;
										
	// Z) Sayonara...
	return sprite->image;
}

/* "Cleans" the HW Sprite system, in case there are HW sprites around */
void ds_3dsprite_cleanHWSprites() {
   // This is a small "hack" for deleting the HW sprites that may be shown in the screen 
   PA_3DProcess();
}      

/* Change the obj stored inside a sprite 
      - At present it only works with the Juni-related images */
void ds_3dsprite_changeObj(int id, u8 obj) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   sprite->obj = obj;
	   sprite->frame = 0;
	   _ds_3dsprite_updateSpriteImage(sprite);
	}   
}   


/* Obtains a pointer to the sprite number "id" frame "frame" */
u16 *ds_3dsprite_getSpriteFrame(int id, int frame) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return NULL;
		
   return ds_3dspritehdd_getSprite(sprite->bank,sprite->obj,frame);
}

/* Obtains a pointer to the sprite number "id", with the actual frame*/
u16 *ds_3dsprite_getSprite(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return NULL;
		
   return ds_3dspritehdd_getSprite(sprite->bank,sprite->obj,sprite->frame);
}

/* Returns the ABSOLUTE X-coordinate of the sprite number "id" */
int ds_3dsprite_getX(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return -1;

   return sprite->xpos;
}   

/* Returns the ABSOLUTE Y-coordinate of the sprite number "id" */
int ds_3dsprite_getY(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return -1;

   return sprite->ypos;
}

/* Returns the SIZE X of the sprite number "id" */
int ds_3dsprite_getXSize(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return 0;

   return sprite->xsize;
}   

/* Returns the SIZE Y of the sprite number "id" */
int ds_3dsprite_getYSize(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return 0;

   return sprite->ysize;
}


/* Sets the ABSOLUTE X-coordinate of the sprite number "id" */
void ds_3dsprite_setX(int id, int x) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL)  {
	   sprite->xpos = x;
		//PA_3DSetSpriteX(id,_ds_3dsprite_realX(sprite,sprite->xpos));
	}		
}   

/* Sets the ABSOLUTE Y-coordinate of the sprite number "id" */
void ds_3dsprite_setY(int id, int y) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   sprite->ypos = y;
		//PA_3DSetSpriteY(id,_ds_3dsprite_realY(sprite,sprite->ypos));
	}		
}   

/* "Draws" all the sprites into the screen */
void ds_3dsprite_drawAll(int camX, int camY) {
   int i;
   int x;
   int y;
   int _timer;
   _ds_t_sprite* sprite;
   
   // First of all... paint map!!!!!!
   sprintf(ds_global_string,"Time: %d (%d)      ", Tick(ds_global_timer),PA_GetVcount());			
   ds_gamestatus_debugOutput(1,0,5,ds_global_string);
   ds_map_paint(ds_global_map.tileMap, camX, camY, 0);
   sprintf(ds_global_string,"Time: %d (%d)      ", Tick(ds_global_timer),PA_GetVcount());
   ds_gamestatus_debugOutput(1,0,6,ds_global_string);
   
   // Just updates the 3D sprites ;-)
   for (i = 0; i <= spriteListMax; i++) {
      sprite = spriteList[i];
		if (sprite != NULL) {
		   x = _ds_3dsprite_realX(sprite,sprite->xpos) - camX;
		   y = _ds_3dsprite_realY(sprite,sprite->ypos) - camY;
		   if (!sprite->invisible)
	   		PA_3DSetSpriteXY(i,x,y);
	   	else
	   		PA_3DSetSpriteXY(i,300,300); // Invisible = not seen :-)	   	
		}   
	}  
	
	// IDEA: Paint, before, and after the VBLANK :-). Use some registers (REG_VCOUNT?)
			
   sprintf(ds_global_string,"Time: %d (%d)      ", Tick(ds_global_timer),PA_GetVcount());			
   ds_gamestatus_debugOutput(1,0,1,ds_global_string);
   
   PA_3DProcess();
   
   sprintf(ds_global_string,"Time: %d (%d)      ", Tick(ds_global_timer),PA_GetVcount());
   ds_gamestatus_debugOutput(1,0,2,ds_global_string);
   
   // <OPT> Do some optimizations before the VBlank
   if (ds_global_optimizationPreload) {
	   while (PA_GetVcount() > 192);
	   _timer = 2; // Safe mark
	   while ((PA_GetVcount() < (192 - (_timer*2))) && (ds_submap_inOpt())) {
	      _timer = PA_GetVcount();
		   ds_submap_doOpt();
		   _timer = PA_GetVcount() - _timer;
		}   
	}	
   
	PA_WaitForVBL();   	
	
   sprintf(ds_global_string,"Time: %d (%d)      ", Tick(ds_global_timer),PA_GetVcount());       		
   ds_gamestatus_debugOutput(1,0,3,ds_global_string);
   
   // FIXME - ReUpdate the 3D sprites
   int updated = 0;
   for (i = 0; i <= spriteListMax; i++) {      
      sprite = spriteList[i];
		if (sprite != NULL) {
		   if (sprite->update != -1) { // Speed up things ;-)
			   _ds_t_sprite_ima* tex;
			   tex = _ds_3dsprite_obtainTex(sprite);
				_ds_3dsprite_updateSpriteInner(sprite,tex,0,1,1); // FIXME - Animations Galore!!!!!!!
				updated++;
			}			
		}   
	}  
	sprintf(ds_global_string,"Updated: %d (%d)          ",updated,_maxtexture); 
	ds_gamestatus_debugOutput(1,0,0,ds_global_string);
	sprintf(ds_global_string,"Time: %d (%d)      ", Tick(ds_global_timer),PA_GetVcount());       		
	ds_gamestatus_debugOutput(1,0,4,ds_global_string);
	
	_maxtexture = 0;
}


/* Returns the actual Frame of a certain sprite */
int ds_3dsprite_getFrame(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return -1;

   return sprite->frame;
}   

/* Returns the maximum number of frames of a certain sprite */
int ds_3dsprite_getMaxFrame(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return 0;
		
	return ds_3dspritehdd_getFrames(sprite->bank,sprite->obj);
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
	   return (sprite->frame < (ds_3dsprite_getMaxFrame(id) >> 1));
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
void ds_3dsprite_setFrame(int id, int frame) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
     sprite->frame = frame;
	  _ds_3dsprite_updateSpriteImage(sprite);
   }   
}   

/* Sets the alpha of a certain sprite */
void ds_3dsprite_setAlpha(int id, int alpha) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   sprite->alpha = alpha;
   	PA_3DSetSpriteAlpha(sprite->image,sprite->alpha >> 3);
 	}  	
}   

/* Gets the alpha of a certain sprite (0..255) */
u8 ds_3dsprite_getAlpha(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   return sprite->alpha;
 	}  	
 	return 255;
}   

/* Marks an sprite as invisible or visible */
void ds_3dsprite_markInvisible(int id, int invisible) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   if (sprite->invisible != invisible) {
		   sprite->invisible = invisible;
			_ds_3dsprite_updateSpriteImage(sprite);
		}   
 	}  	
}

/* Post-processing of the 3d sprite subsystem, after loading a screen */
/* In this library, we use it for loading the "alpha channel" sprites */
/* Scrapped idea: I need too much memory for the extreme case!!!!! (11 * 8 * 32 * 32 * 2 = 180224) */
/*void ds_3dsprite_postLoadProcessing() {
	_ds_t_sprite *sprite;
	
	// We need to check the entire sprite list ^_-
   for (i = 0; i <= spriteListMax; i++) {      
      sprite = spriteList[i];
		if (sprite != NULL) {
		   // Check if this sprite has some alpha associated to it (and is not a special sprite)
		   if ((sprite->alpha != 255) && (!ds_util_bitOne8(sprite->flags,_DS_C_ALPHA_SPECIAL))) { 
		      // OMG! Alpha! We need to create a specific sprite for them
		      // FIXME Check that the sprite came from a "normal" item
		      ds_3dsprite_load( .. Need to pass parameters to do it ^_- .. );
			}			
		}   
	}  
}*/      
