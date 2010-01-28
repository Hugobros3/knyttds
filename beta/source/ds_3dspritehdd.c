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
#include "ds_global.h"
#include "ds_15bpp.h"


//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

typedef struct _ds_tt_spritehdd {
   ds_t_15bpp *image; // Stores the image
   int cache;   // If 1, do not delete me when loading a map ;-)
   int x;
   int y;       // Size of the sprite
} _ds_t_spritehdd;  

// Store pointers to all the sprites used in knytt stories
_ds_t_spritehdd spriteshdd[DS_C_MAX_BANK][DS_C_MAX_OBJ];
// Store pointers to Juni-related images
_ds_t_spritehdd spritesJuni[DS_C_JUNI_ST_MAX_];
// Store pointers to Juni-help-related images
_ds_t_spritehdd spritesJuniSpecial[DS_C_JUNI_SP_MAX_];
// Store pointers to particle-related imaged
_ds_t_spritehdd spritesParticles[DS_C_MAX_OBJ_PART];
// Store pointers to co-related imaged
_ds_t_spritehdd spritesCO[DS_C_MAX_OBJ_CO];


//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------
int _ds_3dspritehdd_load(_ds_t_spritehdd *sprite, char filen[255]) {

	sprite->image = malloc(sizeof(ds_t_15bpp));
	if (sprite->image == NULL) {
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		return 0; // No memoryyyyyyyyyyyyyy!!!!!
	}		
   ds_15bpp_init(sprite->image);		
   if (!ds_15bpp_load(filen,sprite->image,1,0)) {
      ds_global_errorAssign(DS_C_ERR_NOFOUND);
   	return 0; // X_X Aiyaaaaaaaaaaaaaaaaa!!!!!
 	}  	
   	
   return 1; // :-)
}   

int _ds_3dspritehdd_loadJuni(_ds_t_spritehdd *sprite, u8 oo) {
   char filen[255];
   sprintf(filen,"%s%s/Juni%d.png",DS_DIR_MAIN,DS_DIR_JUNI,oo);
   if (!_ds_3dspritehdd_load(sprite,filen)) {		
	   sprintf(ds_global_string,"Juni - %d",oo);
	   ds_global_errorHalt(ds_global_string);
	   //--HALT--//
   }   
   return 1;
} 

int _ds_3dspritehdd_loadJuniSpecial(_ds_t_spritehdd *sprite, u8 oo) {
   char filen[255];
   sprintf(filen,"%s%s/JuniSp%d.png",DS_DIR_MAIN,DS_DIR_JUNI,oo);
   if (!_ds_3dspritehdd_load(sprite,filen)) {
	   sprintf(ds_global_string,"JuniSpecial - %d",oo);
	   ds_global_errorHalt(ds_global_string);
	   //--HALT--//
   }   
   return 1;
}  

int _ds_3dspritehdd_loadNormal(_ds_t_spritehdd *sprite, u8 bank, u8 obj) {
   char filen[255];
  	sprintf(filen,"%s%s%d/Object%d.png",DS_DIR_MAIN,DS_DIR_OBJ,bank,obj);
   if (!_ds_3dspritehdd_load(sprite,filen)) {
	   sprintf(ds_global_string,"Object - %d,%d",bank,obj);
	   ds_global_errorHalt(ds_global_string);
	   //--HALT--//
   }   
   return 1;
}  	

int _ds_3dspritehdd_loadParticle(_ds_t_spritehdd *sprite, u8 obj) {
   char filen[255];
  	sprintf(filen,"%s%s/Part%d.png",DS_DIR_MAIN,DS_DIR_PART,obj);
   if (!_ds_3dspritehdd_load(sprite,filen)) {
	   sprintf(ds_global_string,"Particle - %d",obj);
	   ds_global_errorHalt(ds_global_string);
	   //--HALT--//
   }   
   return 1;
}

int _ds_3dspritehdd_loadCO(_ds_t_spritehdd *sprite, u8 obj, char *image, int *sx, int *sy) {
   char filen[255];
  	sprintf(filen,"%s%s/%s",ds_global_world.dir,DS_DIR_CO,image);

	sprite->image = malloc(sizeof(ds_t_15bpp));
	if (sprite->image == NULL) {
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		return 0; // No memoryyyyyyyyyyyyyy!!!!!
	}		
		
   ds_15bpp_init(sprite->image);
 
   if (!ds_15bpp_loadLimited(filen,sprite->image,sx,sy,
										DS_C_CO_MAXSX,DS_C_CO_MAXSY,DS_C_CO_MAXAREA)) { // <- SIZE LIMIT
   	return 0; // X_X Aiyaaaaaaaaaaaaaaaaa!!!!!
 	}  	
   return 1;
}

_ds_t_spritehdd *_ds_3dspritehdd_getSprite(int bank, int obj) {
   if (bank == DS_C_JUNI_BANK) {
	   return &(spritesJuni[obj]);
   } else if (bank == DS_C_JUNI_BANKSP) {
	   return &(spritesJuniSpecial[obj]);
   } else if (bank == DS_C_PART_BANK) {
	   return &(spritesParticles[obj]);	   
   } else if (bank == DS_C_CO_BANK) {
	   return &(spritesCO[obj]);	  
   } else {
	   return &(spriteshdd[bank][obj]);
	}	
}   

void _ds_3dspritehdd_getSpriteSize(int bank, int obj, int *x, int *y) {
   *x = 24;
   *y = 24;   
   switch (bank) {
      case 2: // Flyers
         if ((obj == 3) || (obj == 4)) {
            // Flies
            *x = 8;
            *y = 8;
         }   
         if ((obj == 8) || (obj == 13) || (obj == 23)) {
            // Particles
            *x = 8;
            *y = 8;
         }   
         break;
      case 4: // Meanies
         if (obj == 9) {
            // Spider
            *x = 48;
            *y = 24;
         }   
         break;
      case 5: // Shadow People
         *x = 24;
         *y = 48;
         break;
      case 6: // Traps
         if (obj == 1) {
            // Block
            *x = 96;
            *y = 24;
         }   
         if (obj == 5) {
            // Eater
            *x = 48;
            *y = 48;
         }   
         break;
      case 7: // Nature FX
         if (obj == 5) {
            // Ray of Light
            *x = 24;
            *y = 96;
         }   
         break;
      case 8: // Decoration
         if (obj == 7) {
            // Fire
            *x = 48;
            *y = 24;
         }   
         if (obj == 10) {
            // Flies
            *x = 8;
            *y = 8;
         }   
         break;
      case 12: // Ghosts
         if ((obj == 6) || (obj == 9)) {
            // Slimes
            *x = 48;
            *y = 24;
         }   
         if (obj == 10) {
            // Eater
            *x = 48;
            *y = 48;
         }   
         if (obj == 14) {
            // Ghost Trap
            *x = 25;
            *y = 72;
         }   
         break;
      case 13: // Robots
         if (obj == 3) {
            // Moving Laser
            *x = 24;
            *y = 96;
         }   
         if (obj == 4) {
            // Square Robot
            *x = 48;
            *y = 48;
         }   
         break;
      case 14: // Earth Beings
         if ((obj == 16) || (obj == 18)) {
            // Spikers
            *x = 64;
            *y = 64;
         }   
         break;
      case 15: // Objects and Areas
         if ((obj >= 13) && (obj <= 22)) {
            // Passwords (including insta-type)
            *x = 24;
            *y = 48;
         }   
         if (obj == 26) {
            // Password Timer
            *x = 24;
            *y = 48;
         }            
         if (obj == 39) {
            // Password Trigger
            *x = 24;
            *y = 48;
         }            
         break;
      case 17: // Creppy
         if (obj == 3) {
            // Water Eye
            *x = 24;
            *y = 48;
         }            
         if ((obj == 10) || (obj == 11) || (obj == 12)) {
            // Spikers
            *x = 64;
            *y = 64;
         }   
         break;
      case 18: // Water Creatures
         if (obj == 6) {
            // Water Eye +
            *x = 24;
            *y = 48;
         }            
         break;
   }   
}

void _ds_3dspritehdd_getParticleSize(int obj, int *x, int *y) {
   *x = 8;
   *y = 8;   
   switch (obj) {
      case 17: // Smoke A
      	*x = 16;
         break;
      case 18: // Smoke Dbl Jump
      	*x = 32;
      	*y = 16;
         break;
      case 19: // Save spot
      	*x = 24;
      	*y = 24;
         break;
      case 20: // Falling Leave
      	*x = 16;
         break;
      case 32: // SPECIAL - Light
      	*x = 24;
      	*y = 24;
         break;
      case 33: // SPECIAL - Light
      	*x = 24;
      	*y = 24;
         break;
      case 34: // SPECIAL - Light
      	*x = 24;
      	*y = 24;
         break;
      case 35: // SPECIAL - Light
      	*x = 24;
      	*y = 24;
         break;
      case 36: // SPECIAL - Light
      	*x = 24;
      	*y = 24;
         break;
      case 37: // SPECIAL - Light
      	*x = 24;
      	*y = 24;
         break;
      case 44:
      	*x = 16;
      	*y = 16;
         break;
      case 45: 
      	*x = 16;
      	*y = 16;
         break;
      case 46: 
      	*x = 96;
      	*y = 96;
         break;
      case 47: 
      	*x = 120;
      	*y = 120;
         break;
      case 48: 
      	*x = 24;
      	*y = 24;
         break;
      case 49: 
      	*x = 16;
      	*y = 16;
         break;
      case 50: 
      	*x = 16;
      	*y = 16;
         break;
      case 52: 
      	*x = 24;
      	*y = 24;
         break;
      case 53: 
      	*x = 24;
      	*y = 24;
         break;
      case 58: 
      	*x = 24;
      	*y = 24;
         break;
   }   
}      

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Obtains a pointer to the sprite bank/obj in the frame "frame" */
u16 *ds_3dspritehdd_getSprite(u8 bank, u8 obj, int frame) {
   u16 *ptr;
   _ds_t_spritehdd *sprite;
   sprite = _ds_3dspritehdd_getSprite(bank,obj);
   
   // Sanity Check
   if (sprite->image == NULL)
   	return NULL;
   
	// Get and return
	ptr = (sprite->image->png_screen) + (sprite->x * sprite->y * frame);
	return ptr;
}   

/* Returns the width (X-size) of the sprite bank/obj */
int ds_3dspritehdd_getWidth(u8 bank, u8 obj) {
   _ds_t_spritehdd *sprite;
   sprite = _ds_3dspritehdd_getSprite(bank,obj);

	// Get and return
	return sprite->x;
}   

/* Returns the height (Y-size) of the sprite bank/obj */
int ds_3dspritehdd_getHeight(u8 bank, u8 obj) {
   _ds_t_spritehdd *sprite;
   sprite = _ds_3dspritehdd_getSprite(bank,obj);
   
	// Get and return
	return sprite->y;
}   

/* Returns both width and height of the sprite bank/obj */
void ds_3dspritehdd_getXY(u8 bank, u8 obj, int *x, int *y) {
   _ds_t_spritehdd *sprite;
   sprite = _ds_3dspritehdd_getSprite(bank,obj);

	// Get X/Y
	*x =  sprite->x;
	*y =  sprite->y;
}   

/* Returns the max number of frames contained inside the sprite bank/obj */
int ds_3dspritehdd_getFrames(u8 bank, u8 obj) {
   _ds_t_spritehdd *sprite;
   sprite = _ds_3dspritehdd_getSprite(bank,obj);

	// Get and return
	return sprite->image->height / sprite->y;
}   

/* Initializes the 3d sprite "hard disk" system */
void ds_3dspritehdd_init() {
   int bb,oo;
   int x,y;
   _ds_t_spritehdd *sprite;
	
	// Initializes Sprite System   
   for (bb = 0; bb < DS_C_MAX_BANK; bb++) {
   	for (oo = 0; oo < DS_C_MAX_OBJ; oo++) {
   	   sprite = &(spriteshdd[bb][oo]);
   	   // Simply Inits
   		sprite->cache = 0;
   		_ds_3dspritehdd_getSpriteSize(bb,oo,&x,&y);
   		sprite->x = x;
   		sprite->y = y;
   		sprite->image = NULL;
  		} 
 	} 		
  	
	// Initializes Particle System   
   for (oo = 0; oo < DS_C_MAX_OBJ_PART; oo++) {
      sprite = &(spritesParticles[oo]);
      // Simply Inits
   	sprite->cache = 0;
		_ds_3dspritehdd_getParticleSize(oo,&x,&y);
   	sprite->x = x;
   	sprite->y = y;
   	sprite->image = NULL;
  	} 
  	
	// Initializes CO System   
   for (oo = 0; oo < DS_C_MAX_OBJ_CO; oo++) {
      sprite = &(spritesCO[oo]);
      // Simply Inits
   	sprite->cache = 0;
   	sprite->x = 24;
   	sprite->y = 24;
   	sprite->image = NULL;
  	} 
  	
  	// Preloads Juni Images (<TODO> "Preload")
  	for (oo = 0; oo < DS_C_JUNI_ST_MAX_; oo++) {
  	   sprite = &(spritesJuni[oo]);
  	   // Inits
  	   sprite->cache = 1; // Really not needed here...
   	sprite->x = 24;
   	sprite->y = 24;
   	// Loads
		_ds_3dspritehdd_loadJuni(sprite,oo); // <TODO> Error Mgmt when booting
  	}   
  	
  	// Preloads Juni Special Images (<TODO> "Preload")
  	for (oo = 0; oo < DS_C_JUNI_SP_MAX_; oo++) {
  	   sprite = &(spritesJuniSpecial[oo]);
  	   // Inits
  	   sprite->cache = 1; // Really not needed here...
   	sprite->x = 24;
   	sprite->y = 24;
   	// Loads
		_ds_3dspritehdd_loadJuniSpecial(sprite,oo); // <TODO> Error Mgmt when booting
  	}   
}   

/* Resets the 3d sprite "hard disk" system. Do not delete any images that should be inside the "cache" */
void ds_3dspritehdd_reset(int force) {
   int bb,oo;
   _ds_t_spritehdd *sprite;
   
   
   // Delete objects
   for (bb = 0; bb < DS_C_MAX_BANK; bb++)
   	for (oo = 0; oo < DS_C_MAX_OBJ; oo++) {
   	   sprite = &(spriteshdd[bb][oo]);
   	   if (sprite->image != NULL) {
   	      if ((sprite->cache == 0) || 
					 ((sprite->cache == 1) && (force == 1))) {
   	         ds_15bpp_delete(sprite->image);
   	         free(sprite->image);
   	         sprite->image = NULL;
		   		sprite->cache = 0;
   	      }
			}			   
  		} 		
  		
  	// Deletes particles
   for (oo = 0; oo < DS_C_MAX_OBJ_PART; oo++) {
      sprite = &(spritesParticles[oo]);
	   if (sprite->image != NULL) {
	      if ((sprite->cache == 0) || 
				 ((sprite->cache == 1) && (force == 1))) {
	         ds_15bpp_delete(sprite->image);
	         free(sprite->image);
	         sprite->image = NULL;
	   		sprite->cache = 0;
	      }
		}			   
  	} 		
  	
  	// Deletes CO
   for (oo = 0; oo < DS_C_MAX_OBJ_CO; oo++) {
      sprite = &(spritesCO[oo]);
	   if (sprite->image != NULL) {
	      if ((sprite->cache == 0) || 
				 ((sprite->cache == 1) && (force == 1))) {
	         ds_15bpp_delete(sprite->image);
	         free(sprite->image);
	         sprite->image = NULL;
	   		sprite->cache = 0;
	      }
		}			   
  	} 		
  		
  	// ...But Juni and her sprites is the main character! Do not delete her ;-) (she is always loaded while Knytt Stories DS runs)
}

/* Load a certain CO into the sprite "hard disk" system.
	Must be called before the normal ds_3dspritehdd_load */
int ds_3dspritehdd_loadCO(u8 obj, char *image, int *sx, int *sy) {
   int val = 0;
   _ds_t_spritehdd *sprite;
   sprite = _ds_3dspritehdd_getSprite(DS_C_CO_BANK,obj);
   
   // First, check if this sprite already exists
   if (sprite->image == NULL) {
      // Load CO!
      if (_ds_3dspritehdd_loadCO(sprite, obj, image, sx, sy)) {
   		sprite->x = (*sx);
   		sprite->y = (*sy);
   		val = 1;
 		} 	
	} else {
      val = 1; // If it already exists... it is OK!
   }   
	
	return val;	
}   


/* Load the bank/obj into the sprite "hard disk" system 
	CO sprites must be loaded before this point */
int ds_3dspritehdd_load(u8 bank, u8 obj) {
   int val = 0;
   _ds_t_spritehdd *sprite;
   sprite = _ds_3dspritehdd_getSprite(bank,obj);
   
   // First, check if this sprite already exists
   if (sprite->image == NULL) {
      // Loads!
      if (bank == DS_C_JUNI_BANK) {
         // Load a Juni-Related Image
         val = _ds_3dspritehdd_loadJuni(sprite,obj);
      } else 
      if (bank == DS_C_JUNI_BANKSP) {
         // Load a Juni-Related Image
         val = _ds_3dspritehdd_loadJuniSpecial(sprite,obj);
      } else       
      if (bank == DS_C_PART_BANK) {
         // Load a Particle
         val = _ds_3dspritehdd_loadParticle(sprite,obj);
      } else      
      if (bank == DS_C_CO_BANK) {
         // Assertion!
		   sprintf(ds_global_string,"CO Programming Error! \n use ds_3dspritehdd_loadCO");
		   ds_global_errorHalt(ds_global_string);		     
		   // -- HALT :-P
      } else							
		{   
         // Load a normal sprite
         val = _ds_3dspritehdd_loadNormal(sprite,bank,obj);
 		}  	
   } else {
      val = 1; // If it already exists... it is OK!
   }   
	
	// Return...
	return val;
}  
  
