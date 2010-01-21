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

typedef struct _ds_tt_sprite {
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
   u8 isUnique; // If the image is unique or the frame must be retrieved from a global structure
} _ds_t_sprite;  

// Store pointers to all the sprites used in one screen. No linked lists here :-P
_ds_t_sprite* spriteList[_MAX_OBJSPRITE];
int spriteListCnt; // Actual position of the list
int spriteListNum; // Number of sprites
int spriteListMax; // Maximum position with a sprite (*including* the last one)

// Store "frames" to all the "global sprites" structure. Also, no linked lists here ;-)
int spriteGlobal[DS_C_MAX_BANK][DS_C_MAX_OBJ];

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------

int _ds_3dsprite_getFrameInner(_ds_t_sprite* sprite) {
	return (sprite->isUnique)?sprite->frame:spriteGlobal[sprite->bank][sprite->obj];
}

/*
#define DUFF_DEVICE_8(aCount, aAction) \
{ \
  int count_ = (aCount); \
  int times_ = (count_ + 7) >> 3; \
  switch (count_ & 7){ \
    case 0: do { aAction; \
      case 7: aAction; \
      case 6: aAction; \
      case 5: aAction; \
      case 4: aAction; \
      case 3: aAction; \
      case 2: aAction; \
      case 1: aAction; \
    } while (–times_ > 0); \
  } \
}
*/

/* Copy a sprite to the main screen - Inside map, without Alpha */
void _ds_3dsprite_copyScreen_inside(_ds_t_sprite* sprite, int dsscreen, int x, int y) {
   int i,j;
   int xsini = 0; 
	int ysini = 0;
   int xs = sprite->xsize; 
	int ys = sprite->ysize;
   int xsr = sprite->xsize;
   int ysr = sprite->ysize;
   int xr = x;
   int yr = y;
   int jmul, ymul;
   u16 pix;
   u16 *spriteFrame = ds_3dspritehdd_getSprite(sprite->bank,sprite->obj,_ds_3dsprite_getFrameInner(sprite));
   
   for (j = ysini; j < ys; j++) {
      ymul = (yr + j) << 8; // OPT: 256 = NDS x-screen size
      jmul = j * xsr;
      
   	for (i = xsini; i < xs; i++) {
   	   pix = *(spriteFrame + i + jmul);
   	   if (pix != 0)
   			*(PA_DrawBg[dsscreen] + (xr + i) + ymul) = pix;
		}
	}	   
}  


/* Copy a sprite to the main screen - Outside map, without Alpha */
void _ds_3dsprite_copyScreen_outside(_ds_t_sprite* sprite, int dsscreen, int x, int y) {
   int i,j;
   int xsini = 0; 
	int ysini = 0;
   int xs = sprite->xsize; 
	int ys = sprite->ysize;
   int xsr = sprite->xsize;
   int ysr = sprite->ysize;
   int xr = x;
   int yr = y;
   int jmul, ymul;
   u16 pix;
   u16 *spriteFrame = ds_3dspritehdd_getSprite(sprite->bank,sprite->obj,_ds_3dsprite_getFrameInner(sprite));
   
   if (x < 0) {
      //xr = 0;
   	xsini = 0 - x;
   	//xs = sprite->xsize; 
   } else 
	if ((x + xsr) >= 256) {
	   //xr = x;
	   //xsini = 0;
   	xs = 256 - x;
	}
   if (y < 0) {
      //yr = 0;
   	ysini = 0 - y;
   	//ys = sprite->ysize; 
   } else 
	if ((y + ysr) >= 192) {
	   //yr = y;
	   //ysini = 0;
   	ys = 192 - y;
	}
   
   for (j = ysini; j < ys; j++) {
      ymul = (yr + j) << 8; // OPT: 256 = NDS x-screen size
      jmul = j * xsr;
   	for (i = xsini; i < xs; i++) {
   	   pix = *(spriteFrame + i + jmul);
   	   if (pix != 0)
   			*(PA_DrawBg[dsscreen] + (xr + i) + ymul) = pix;
		}
	}	   
}   


#define PA_Alpha128(newp,oldp) PA_RGB(((oldp & 0x1f) + (newp & 0x1f)) >> 1, \
	              						(((oldp >> 5) & 0x1f) + ((newp >> 5) & 0x1f)) >> 1, \
					  						(((oldp >> 10) & 0x1f) + ((newp >> 10) & 0x1f)) >> 1);

/* Copy a sprite to the main screen - Inside map, with Alpha */
// NOTE! Check http://www.slack.net/~ant/info/rgb_mixing.html
void _ds_3dsprite_copyScreen_insideAlpha(_ds_t_sprite* sprite, int dsscreen, int x, int y) {
   int i,j;
   int xsini = 0; 
	int ysini = 0;
   int xs = sprite->xsize; 
	int ys = sprite->ysize;
   int xsr = sprite->xsize;
   int ysr = sprite->ysize;
   int xr = x;
   int yr = y;
   int jmul, ymul;
   u16 pix;
   u16 pixalpha;
   u16 *spriteFrame = ds_3dspritehdd_getSprite(sprite->bank,sprite->obj,_ds_3dsprite_getFrameInner(sprite));
   u16 *screenP;
   u8 alpha = sprite->alpha;
   
   /*for (j = ysini; j < ys; j++) {
      ymul = (yr + j) << 8; // OPT: 256 = NDS x-screen size
      jmul = j * xsr;
   	for (i = xsini; i < xs; i++) {
   	   pix = *(spriteFrame + i + jmul);
   	   screenP = PA_DrawBg[dsscreen] + (xr + i) + ymul;
   	   pixalpha = *(screenP);
   	   //*(screenP) = ds_util_mixColorsAplha(pixalpha,pix,alpha);
   	   //*(screenP) = (pix + pixalpha) >> 1;
   	   //*(screenP) = PA_Alpha128(pixalpha,pix);
   	   *(screenP) = (((pix & 0x7bde) >> 1) + ((pixalpha & 0x7bde) >> 1) | 0x8000); // The BAD way
		}
	}*/	
	
	u32 pix32,pixalpha32;
	for (j = ysini; j < ys; j++) {
      ymul = (yr + j) << 8; // OPT: 256 = NDS x-screen size
      jmul = j * xsr;
   	for (i = xsini; i < xs; i+=2) {
   	   pix32 = *(u32 *)(spriteFrame + i + jmul);
   	   screenP = PA_DrawBg[dsscreen] + (xr + i) + ymul;
   	   pixalpha32 = *(u32 *)(screenP);
   	   *(u32 *)(screenP) = (((pix32 & 0x7bde7bde) >> 1) + ((pixalpha32 & 0x7bde7bde) >> 1) | 0x80008000); // The BAD way
		}
	}   
}  

/* Copy a sprite to the main screen - Outside map, with Alpha */
void _ds_3dsprite_copyScreen_outsideAlpha(_ds_t_sprite* sprite, int dsscreen, int x, int y) {
   int i,j;
   int xsini = 0; 
	int ysini = 0;
   int xs = sprite->xsize; 
	int ys = sprite->ysize;
   int xsr = sprite->xsize;
   int ysr = sprite->ysize;
   int xr = x;
   int yr = y;
   int jmul, ymul;
   u16 pix;
   u16 pixalpha;
   u16 *spriteFrame = ds_3dspritehdd_getSprite(sprite->bank,sprite->obj,_ds_3dsprite_getFrameInner(sprite));
   u16 *screenP;
   u8 alpha = sprite->alpha;
   
   if (x < 0) {
      //xr = 0;
   	xsini = 0 - x;
   	//xs = sprite->xsize; 
   } else 
	if ((x + xsr) >= 256) {
	   //xr = x;
	   //xsini = 0;
   	xs = 256 - x;
	}
   if (y < 0) {
      //yr = 0;
   	ysini = 0 - y;
   	//ys = sprite->ysize; 
   } else 
	if ((y + ysr) >= 192) {
	   //yr = y;
	   //ysini = 0;
   	ys = 192 - y;
	}
   
   for (j = ysini; j < ys; j++) {
      ymul = (yr + j) << 8; // OPT: 256 = NDS x-screen size
      jmul = j * xsr;
   	for (i = xsini; i < xs; i++) {
   	   pix = *(spriteFrame + i + jmul);
   	   screenP = PA_DrawBg[dsscreen] + (xr + i) + ymul;
   	   pixalpha = *(screenP);
   	   //*(screenP) = ds_util_mixColorsAplha(pixalpha,pix,alpha);
   	   //*(screenP) = (pix + pixalpha) >> 1;
   	   //*(screenP) = PA_Alpha128(pixalpha,pix);
   	   *(screenP) = (((pix & 0x7bde) >> 1) + ((pixalpha & 0x7bde) >> 1) | 0x8000); // The BAD way
		}
	}	   
}   


//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Initializes the 3d sprite system */
void ds_3dsprite_init() {
   int i,j;
   
   // Inits the System
   for (i = 0; i < _MAX_OBJSPRITE; i++) {
      spriteList[i] = NULL; 
   }   
   spriteListCnt = 0;
   spriteListNum = 0;
   spriteListMax = -1;
   for (j = 0; j < DS_C_MAX_BANK; j++) {
	   for (i = 0; i < DS_C_MAX_OBJ; i++) {
	      spriteGlobal[j][i] = 0;
  		}   	      
  	}

   // Inits the 3D System and the 3D Sprites and the HDD
   ds_3dspritehdd_init();
}   

/* Deletes a single sprite. If force == 1, also delete the HW sprite */
void ds_3dsprite_deleteSprite(int id, int force) {
   if (spriteList[id] != NULL) {
      // Deletes the cache image... but not in this visual engine
    	// Deletes the sprite image (NOT USED IN THIS IMPLEMENTATION OF THE VISUAL ENGINE)

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
   
   // Resets the Sprite list
   for (i = 0; i <= spriteListMax; i++) {
      ds_3dsprite_deleteSprite(i,0);
   }   
   spriteListCnt = 0;
   spriteListNum = 0;
   spriteListMax = -1;
   for (j = 0; j < DS_C_MAX_BANK; j++) {
	   for (i = 0; i < DS_C_MAX_OBJ; i++) {
	      spriteGlobal[j][i] = 0;
  		}   	      
  	}
      
   // Reset the 3D Sprites and the HDD
   ds_3dspritehdd_reset(force);
}

/* Load the bank/obj object into certain screen coordinates x/y, returns the id of the specific sprite, -1 if error */
int ds_3dsprite_load(u8 bank, u8 obj, int x, int y, int isUnique) {
   _ds_t_sprite* sprite;
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
	
	// 2a) HW Texture? NOT in this implementation of the visual engine... but need to check frame!
	sprite->isUnique = isUnique;
	if (!isUnique) {
	   spriteGlobal[bank][obj] = 0;
	}   
	sprite->update = -1; // No need to do anything here :-)
	
	// 4) Add to the list...
	spriteList[sprite->image] = sprite;
										
	// Z) Sayonara...
	return sprite->image;
}

/* "Cleans" the HW Sprite system, in case there are HW sprites around */
void ds_3dsprite_cleanHWSprites() {
   // This small hack is now pointless, so...
}      

/* Change the obj stored inside a sprite 
      - At present it only works with the Juni-related images */
void ds_3dsprite_changeObj(int id, u8 obj) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   sprite->obj = obj;
	   sprite->frame = 0; // Just a Juni-related image... no "isUnique" check
	   // It is OK with this, We will retrieve the sprite we need from the hdd module
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
		
   return ds_3dspritehdd_getSprite(sprite->bank,sprite->obj,
				(sprite->isUnique)?sprite->frame:spriteGlobal[sprite->bank][sprite->obj]);
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
	}		
}   

/* Sets the ABSOLUTE Y-coordinate of the sprite number "id" */
void ds_3dsprite_setY(int id, int y) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   sprite->ypos = y;
	}		
}

void ds_3dsprite_drawAll(int camX, int camY) {
   int layer;
   int i;
   int x, y;
   int xss, yss;
   int _timer;
   _ds_t_sprite* sprite;
      
   // First, let's let the system to update the screen ;-)
   // <HACK> VBlank = 192, I start earlier... So I can update better the graphics 
   while (PA_GetVcount() > 192);
   while (PA_GetVcount() < (192 - 30));
   //PA_WaitForVBL();
   
   // Now... paint map!!!!!!
   sprintf(ds_global_string,"Time (Map): %d (%d)      ", Tick(ds_global_timer),PA_GetVcount());			
   ds_gamestatus_debugOutput(1,0,0,ds_global_string);
   ds_map_paint(ds_global_map.tileMap, camX, camY, 0);
   sprintf(ds_global_string,"Time (Map): %d (%d)      ", Tick(ds_global_timer),PA_GetVcount());
   ds_gamestatus_debugOutput(1,0,1,ds_global_string);
   
   // Now... paint the different sprites onto the screen!
   sprintf(ds_global_string,"Time (Sprite): %d (%d)      ", Tick(ds_global_timer),PA_GetVcount());
   ds_gamestatus_debugOutput(1,0,2,ds_global_string);
   for (i = 0; i <= spriteListMax; i++) {
      sprite = spriteList[i];
		if (sprite != NULL) {
		   if (!sprite->invisible) {
			   x = sprite->xpos - camX;
			   y = sprite->ypos - camY;
			   xss = x + sprite->xsize;
			   yss = y + sprite->ysize;
			   if ((x >= 0) && (xss < 256) &&
			   	 (y >= 0) && (yss < 192)) {
			   	 if (sprite->alpha == 255)
			   	 	_ds_3dsprite_copyScreen_inside(sprite,0,x,y);
			   	 else
			   	 	_ds_3dsprite_copyScreen_insideAlpha(sprite,0,x,y);
			   } else
				if ((x < 256) || (xss >= 0) ||
			   	 (y < 192) || (yss >= 0)) {
			   	 if (sprite->alpha == 255)
			   	 	_ds_3dsprite_copyScreen_outside(sprite,0,x,y);
			   	 else
			   	 	_ds_3dsprite_copyScreen_outsideAlpha(sprite,0,x,y);
				}     
	  		} 		
		}   
	}  	
   sprintf(ds_global_string,"Time (Sprite): %d (%d)      ", Tick(ds_global_timer),PA_GetVcount());
   ds_gamestatus_debugOutput(1,0,3,ds_global_string);
}    

/* Returns the actual Frame of a certain sprite */
int ds_3dsprite_getFrame(int id) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite == NULL) 
		return -1;
	return (sprite->isUnique)?sprite->frame:spriteGlobal[sprite->bank][sprite->obj];
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
	   return (ds_3dsprite_getFrame(id) < (ds_3dsprite_getMaxFrame(id) >> 1));
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
	   if (sprite->isUnique)
			sprite->frame = frame;
		else spriteGlobal[sprite->bank][sprite->obj] = frame;
   }   
}   

/* Sets the alpha of a certain sprite */
void ds_3dsprite_setAlpha(int id, int alpha) {
   _ds_t_sprite* sprite;
   sprite = spriteList[id];
	if (sprite != NULL) {
	   sprite->alpha = alpha;
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
		}   
 	}  	
}   
