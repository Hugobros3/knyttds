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

	- GameStatus Design
		=When the "game" (screen) is loaded, the upper screen is fully updated.
		=Every cycle:
		   - (Small) Screen is refreshed.
		   - Objects (Radar) are painted in the screen
		=IF item is won/lost
			- Upper screen is full updated
*/

#include "ds_global.h"
#include "ds_util_bit.h"
#include "ds_15bpp.h"
#include "ds_util.h"
#include "ds_world.h"
#include "ds_3dspritehdd.h"
#include "ds_util_bit.h"
#include "ds_3dsprite.h"
#include "PA_Text16bitBuffer.h"


//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

#define _DS_C_CT_LAUNCH 0
#define _DS_C_CT_LAUNCHAFTER 1
#define _DS_C_CT_UPDATE 2
#define _DS_C_CT_CHANGEMODE 3
#define _RADARSIZE 102

int _ds_actualStatusScreen;
int _ds_statusType;
u32 _ds_statusType_var;
int _ds_stackedstatusType;


int _ds_statusItems;

ds_t_15bpp minimap_600x240_smallImage;
ds_t_15bpp minimap_256x192_menu;

ds_t_15bpp minimap_8x8_greenIcon;
ds_t_15bpp minimap_8x8_redIcon;
ds_t_15bpp minimap_8x8_blueIcon;
ds_t_15bpp minimap_8x8_yellowIcon;
ds_t_15bpp minimap_8x8_juniIcon;
ds_t_15bpp minimap_8x8_mapIn;
ds_t_15bpp minimap_8x8_mapIn2;
ds_t_15bpp minimap_8x8_mapOut;
ds_t_15bpp minimap_8x8_mapIn_m;
ds_t_15bpp minimap_8x8_mapIn2_m;
ds_t_15bpp minimap_8x8_mapOut_m;
ds_t_15bpp minimap_8x8_mapIn_l;
ds_t_15bpp minimap_8x8_mapIn2_l;
ds_t_15bpp minimap_8x8_mapOut_l;

ds_t_15bpp minimap_24x24_run;
ds_t_15bpp minimap_24x24_runOK;
ds_t_15bpp minimap_24x24_climb;
ds_t_15bpp minimap_24x24_climbOK;
ds_t_15bpp minimap_24x24_dbljump;
ds_t_15bpp minimap_24x24_dbljumpOK;
ds_t_15bpp minimap_24x24_jump;
ds_t_15bpp minimap_24x24_jumpOK;
ds_t_15bpp minimap_24x24_eye;
ds_t_15bpp minimap_24x24_eyeOK;
ds_t_15bpp minimap_24x24_enemy;
ds_t_15bpp minimap_24x24_enemyOK;
ds_t_15bpp minimap_24x24_umbrella;
ds_t_15bpp minimap_24x24_umbrellaOK;
ds_t_15bpp minimap_24x24_hologram;
ds_t_15bpp minimap_24x24_hologramOK;
ds_t_15bpp minimap_24x24_bluekey;
ds_t_15bpp minimap_24x24_bluekeyOK;
ds_t_15bpp minimap_24x24_violetkey;
ds_t_15bpp minimap_24x24_violetkeyOK;
ds_t_15bpp minimap_24x24_redkey;
ds_t_15bpp minimap_24x24_redkeyOK;
ds_t_15bpp minimap_24x24_yellowkey;
ds_t_15bpp minimap_24x24_yellowkeyOK;

u8 minimap_radarBuffer[_RADARSIZE];

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS - TODO
//-------------------------------------------------------------------------------------------------

int _ds_gamestatus_objMini(int obj, int bank) {
   int res = 0;
   
   if (obj == 0)
   	return res;

   switch (bank) {
      // BANK 1 - LIQUIDS
      //-------------------
      case 1: 
         if ((obj >= 1) && (obj <= 17))
         	res = 1;
         else 
         if ((obj >= 19) && (obj <= 24))
         	res = 1;
         break;
      // BANK 8 - DECORATION
      //--------------------
      case 8: 
         if ((obj >= 1) && (obj <= 6))
         	res = 1;
         else 
         if (obj == 9)
         	res = 1;
			else
         if ((obj >= 11) && (obj <= 14))
         	res = 1;         
         break;
      // BANK 15 - OBJ & AREAS
      //----------------------
      case 15: 
         if ((obj >= 1) && (obj <= 4))
         	res = 1;
			else
         if ((obj >= 8) && (obj <= 12))
         	res = 1;         
         break;
   }   
   
   return res;
}

int _ds_gamestatus_objAlpha(int obj, int bank) {
   int res = 0;
   
   if (obj == 0)
   	return res;
   
   switch (bank) {
      // BANK 1 - LIQUIDS
      //-------------------
      case 1:
         if ((obj == 3) || (obj == 4) || (obj == 15) || (obj == 20) || (obj == 23))
         	res = 1;
         break;
      // BANK 8 - DECORATION
      //--------------------
      case 8: 
         if ((obj >= 1) && (obj <= 6))
         	res = 1;
         else 
         if (obj == 9)
         	res = 1;
         break;
   }   
   
   return res;
}   
   

/* Paint special objects on the small screen. We only manage 24x24 objects */
void _ds_gamestatus_updateSmallScreen(int oriW, int oriH, int destW, int destH) {
   int i;
   int j;
   int t;
   int obj;
   int bank;
   int factorx, factory;
	int ytrav, xtrav;
	int realX, realY;
	int tileX, tileY;
	int tileNewX, tileNewY;
	int imaX, imaY;
	u16 *tileIma[4];
	u8 tileAlpha[4];
	
   factorx = (oriW * 1000) / destW;
   factory = (oriH * 1000) / destH;
   
   tileX = -1;
   tileY = -1;
   for (t = 0; t < 4; t++) {
      tileIma[t] = NULL;
      tileAlpha[t] = 0;
   }   

	// - Normal management   
   ytrav = 0;   
   for (j = 0; j < destH; j++) {
      xtrav = 0;
      for (i = 0; i < destW; i++) {
         // Get real cordinates
         realX = (xtrav / 1000);
         realY = (ytrav / 1000);
         // Get objects here if tile is new
         tileNewX = realX / 24;
         tileNewY = realY / 24;
         if ((tileX != tileNewX) || (tileY != tileNewY)) {
	         tileX = tileNewX;
	         tileY = tileNewY;
	   	   for (t = 0; t < 4; t++) {
	   	      obj = ds_global_map.roomObj[t].obj[tileY][tileX];
	   	      bank = ds_global_map.roomObj[t].bank[tileY][tileX];
	   	      if (_ds_gamestatus_objMini(obj,bank)) {
	   	         tileIma[t] = ds_3dspritehdd_getSprite(bank, obj, 0);
	   	         tileAlpha[t] = _ds_gamestatus_objAlpha(obj,bank);
	   	      } else {
	   	         // No image here
	   	         tileIma[t] = NULL;
	   	         tileAlpha[t] = 0;
					}     
	   	   }
         }   
         // Paint
         imaX = realX % 24;
         imaY = realY % 24;
         for (t = 0; t < 4; t++) {
            if (tileIma[t] != NULL) {
               u16 pix = tileIma[t][imaX + (imaY * 24)];
               if (pix != 0) {
                  if (tileAlpha[t])
                  	pix = ds_util_mixColorsAplha128(
								minimap_600x240_smallImage.png_screen[i + (j * destW)],
								pix);
	               minimap_600x240_smallImage.png_screen[i + (j * destW)] = 
							pix; // Only 24x24 images!
					}			
            }   
         }
         // Next Coord...
         xtrav += factorx;
    	}  
		ytrav += factory;   
   }   
} 

/* Paint special objects on the small screen. We only manage 24x24 objects AND COs */
void _ds_gamestatus_updateSmallScreenCO(int oriW, int oriH, int destW, int destH) {
   int i;
   int j;
   int t;
   int factorx, factory;
	int tileX, tileY;
	u16 *tileIma[4];
	u8 tileAlpha[4];
	
   factorx = (oriW * 1000) / destW;
   factory = (oriH * 1000) / destH;
   
   tileX = -1;
   tileY = -1;
   for (t = 0; t < 4; t++) {
      tileIma[t] = NULL;
      tileAlpha[t] = 0;
   }   

   // - CO management
	int layerCnt;
	ds_t_object *object;
	void *myiterator;
	for (layerCnt = 0; layerCnt < 4; layerCnt++) {
	   myiterator = ds_linkedlist_startIterator(&ds_global_objects);
	   while ((object = ds_linkedlist_getIterator(&ds_global_objects,&myiterator)) != NULL) {
			if ((object->bank != DS_C_CO_BANK) && (!_ds_gamestatus_objMini(object->obj,object->bank)))
				continue; // No problem, as assigment is on the while sentence ;-)
	      if ((ds_3dsprite_hasHWSprite(object->sprite)) &&
				 (object->layer == layerCnt)) {
	         tileIma[layerCnt] = ds_3dspritehdd_getSprite(object->bank, object->obj, 0);
	         tileAlpha[layerCnt] = _ds_gamestatus_objAlpha(object->obj,object->bank);
	         // Select one drawing mode
	         int xinioverflow;
	         int xiniima;
				if (object->x >= 0) {
				   xinioverflow = object->x;
				   xiniima = 0;
				} else {
					xinioverflow = 0;
					xiniima = object->xs + object->x;
				}			
	         int yinioverflow;
	         int yiniima;
				if (object->y >= 0) {
				   yinioverflow = object->y;
				   yiniima = 0;
				} else {
					yinioverflow = 0;
					yiniima = object->ys + object->y;
				}			
	         int xendoverflow = ((xinioverflow + object->xs) < oriW)?object->xs:oriW - xinioverflow;
	         int yendoverflow = ((yinioverflow + object->ys) < oriH)?object->ys:oriH - yinioverflow;
	         // Special "Same pixel" management
	         int xDest = -1;
	         int yDest = -1;
	         int xDestTmp;
	         int yDestTmp;
	         // Draw!
	         for (j = yiniima; j < yendoverflow; j++) {
	            // "Same pixel" check
	            yDestTmp = ((yinioverflow + j) * destH) / oriH; 
	            if (yDestTmp == yDest)
	            	continue;
	            yDest = yDestTmp;	
	            // continue painting
	            int yrow = (yDest) * destW;
	            for (i = xiniima; i < xendoverflow; i++) {
	               // "Same pixel" check
		            xDestTmp = ((xinioverflow + i) * destW) / oriW; 
		            if (xDestTmp == xDest)
		            	continue;
		            xDest = xDestTmp;	
	               // continue painting
	               u16 pix = tileIma[layerCnt][i + (j * object->xs)];
	               if (pix != 0) {
	                  int val = xDest + yrow;
	                  if (tileAlpha[layerCnt])
	                  	pix = ds_util_mixColorsAplha128(
									minimap_600x240_smallImage.png_screen[val],pix);
	               	minimap_600x240_smallImage.png_screen[val] = pix;
	             	}  	
	            }   
		  		}     
	      }   
		}	   
	}		
} 



//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Loads a particular item */
void _ds_gamestatus_loadItem(ds_t_15bpp *ima, int item, int owned) {
   
   // First, it gets the image 
	switch (item) {
	   case DS_C_JUNI_IT_RUN:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_run.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_run_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}      			
	      break;
	   case DS_C_JUNI_IT_CLIMB:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_climb.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_climb_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}     
	      break;
	   case DS_C_JUNI_IT_DBLJUMP:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_dbljump.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_dbljump_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}      
	      break;
	   case DS_C_JUNI_IT_JUMP:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_jump.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_jump_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}      
	      break;
	   case DS_C_JUNI_IT_EYE:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_eye.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_eye_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}     
	      break;
	   case DS_C_JUNI_IT_ENEMY:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_enemy.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_enemy_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}      
	      break;
	   case DS_C_JUNI_IT_UMBRELLA:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_umb.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_umb_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}      
	      break;
	   case DS_C_JUNI_IT_HOLOGRAM:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_holo.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_holo_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}      
	      break;
	   case DS_C_JUNI_IT_BLUEKEY:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_blue.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_blue_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}      
	      break;
	   case DS_C_JUNI_IT_VIOLETKEY:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_violet.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_violet_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}      
	      break;
	   case DS_C_JUNI_IT_REDKEY:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_red.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_red_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}      
	      break;
	   case DS_C_JUNI_IT_YELLOWKEY:
	      if (owned) {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_yellow.png",DS_DIR_MAIN,DS_DIR_SYS);
	      } else {
	         sprintf(ds_global_string,"%s%s/Gui_ico_it_yellow_.png",DS_DIR_MAIN,DS_DIR_SYS);
			}      
	      break;
	}      
	
	// Now, loads it
	ds_15bpp_init(ima);
   if (!ds_15bpp_load(ds_global_string, ima, 1, 0)) {
      ds_global_errorAssign(DS_C_ERR_NOFOUND);
		sprintf(ds_global_string,"Power Icon (%d)",item);
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
	}
}    


/* Starts the subsystem. Loads all the necessary images. */
void ds_gamestatus_initOnce() {
   char filen[255];
   
   // Init interval variables
   _ds_actualStatusScreen = -2;
   _ds_statusType = DS_C_STA_RADAR;
   _ds_stackedstatusType = DS_C_STA_;
   _ds_statusType_var = 0;

	// Init main images :-P
	ds_15bpp_init(&minimap_256x192_menu);
	ds_15bpp_init(&minimap_600x240_smallImage);

   // Init icons used in the status screen   
	ds_15bpp_init(&minimap_8x8_greenIcon);
	ds_15bpp_init(&minimap_8x8_redIcon);
	ds_15bpp_init(&minimap_8x8_blueIcon);
	ds_15bpp_init(&minimap_8x8_yellowIcon);
	ds_15bpp_init(&minimap_8x8_juniIcon);
	ds_15bpp_init(&minimap_8x8_mapIn);
	ds_15bpp_init(&minimap_8x8_mapIn2);
	ds_15bpp_init(&minimap_8x8_mapOut);
	ds_15bpp_init(&minimap_8x8_mapIn_m);
	ds_15bpp_init(&minimap_8x8_mapIn2_m);
	ds_15bpp_init(&minimap_8x8_mapOut_m);
	ds_15bpp_init(&minimap_8x8_mapIn_l);
	ds_15bpp_init(&minimap_8x8_mapIn2_l);
	ds_15bpp_init(&minimap_8x8_mapOut_l);

   // Load icons used in the status screen :-)
	// First, load the icons
	if (!minimap_8x8_blueIcon.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_blue.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_blueIcon, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"Blue MiniIcon");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	if (!minimap_8x8_greenIcon.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_green.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_greenIcon, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"Green MiniIcon");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	if (!minimap_8x8_redIcon.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_red.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_redIcon, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"Red MiniIcon");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	if (!minimap_8x8_juniIcon.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_juni.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_juniIcon, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"Juni MiniIcon");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	// First, load the icons
	if (!minimap_8x8_yellowIcon.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_yellow.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_yellowIcon, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"Yellow MiniIcon");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	if (!minimap_8x8_mapIn.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_mapIn.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_mapIn, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"MapIn");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	if (!minimap_8x8_mapIn2.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_mapIn2.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_mapIn2, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"MapIn2");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	if (!minimap_8x8_mapOut.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_mapOut.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_mapOut, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"MapOut");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}
	if (!minimap_8x8_mapIn_m.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_mapIn_m.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_mapIn_m, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"MapIn_m");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	if (!minimap_8x8_mapIn2_m.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_mapIn2_m.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_mapIn2_m, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"MapIn2_m");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	if (!minimap_8x8_mapOut_m.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_mapOut_m.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_mapOut_m, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"MapOut_m");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}
	if (!minimap_8x8_mapIn_l.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_mapIn_l.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_mapIn_l, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"MapIn_l");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	if (!minimap_8x8_mapIn2_l.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_mapIn2_l.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_mapIn2_l, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"MapIn2_l");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}	
	if (!minimap_8x8_mapOut_l.loaded) {
	   sprintf(filen,"%s%s/Gui_ico_mapOut_l.png",DS_DIR_MAIN,DS_DIR_SYS);
	   if (!ds_15bpp_load(filen, &minimap_8x8_mapOut_l, 1, 0)) {
	      ds_global_errorAssign(DS_C_ERR_NOFOUND);
			sprintf(ds_global_string,"MapOut_l");
			ds_global_errorHalt(ds_global_string);
			//--HALT--//
		}		
	}
	
	// Init/Load item icons of the game screen	
	_ds_gamestatus_loadItem(&minimap_24x24_run,DS_C_JUNI_IT_RUN,0);
	_ds_gamestatus_loadItem(&minimap_24x24_runOK,DS_C_JUNI_IT_RUN,1);
	_ds_gamestatus_loadItem(&minimap_24x24_climb,DS_C_JUNI_IT_CLIMB,0);
	_ds_gamestatus_loadItem(&minimap_24x24_climbOK,DS_C_JUNI_IT_CLIMB,1);
	_ds_gamestatus_loadItem(&minimap_24x24_dbljump,DS_C_JUNI_IT_DBLJUMP,0);
	_ds_gamestatus_loadItem(&minimap_24x24_dbljumpOK,DS_C_JUNI_IT_DBLJUMP,1);
	_ds_gamestatus_loadItem(&minimap_24x24_jump,DS_C_JUNI_IT_JUMP,0);
	_ds_gamestatus_loadItem(&minimap_24x24_jumpOK,DS_C_JUNI_IT_JUMP,1);
	_ds_gamestatus_loadItem(&minimap_24x24_eye,DS_C_JUNI_IT_EYE,0);
	_ds_gamestatus_loadItem(&minimap_24x24_eyeOK,DS_C_JUNI_IT_EYE,1);
	_ds_gamestatus_loadItem(&minimap_24x24_enemy,DS_C_JUNI_IT_ENEMY,0);
	_ds_gamestatus_loadItem(&minimap_24x24_enemyOK,DS_C_JUNI_IT_ENEMY,1);
	_ds_gamestatus_loadItem(&minimap_24x24_umbrella,DS_C_JUNI_IT_UMBRELLA,0);
	_ds_gamestatus_loadItem(&minimap_24x24_umbrellaOK,DS_C_JUNI_IT_UMBRELLA,1);
	_ds_gamestatus_loadItem(&minimap_24x24_hologram,DS_C_JUNI_IT_HOLOGRAM,0);
	_ds_gamestatus_loadItem(&minimap_24x24_hologramOK,DS_C_JUNI_IT_HOLOGRAM,1);
	_ds_gamestatus_loadItem(&minimap_24x24_bluekey,DS_C_JUNI_IT_BLUEKEY,0);
	_ds_gamestatus_loadItem(&minimap_24x24_bluekeyOK,DS_C_JUNI_IT_BLUEKEY,1);
	_ds_gamestatus_loadItem(&minimap_24x24_violetkey,DS_C_JUNI_IT_VIOLETKEY,0);
	_ds_gamestatus_loadItem(&minimap_24x24_violetkeyOK,DS_C_JUNI_IT_VIOLETKEY,1);
	_ds_gamestatus_loadItem(&minimap_24x24_redkey,DS_C_JUNI_IT_REDKEY,0);
	_ds_gamestatus_loadItem(&minimap_24x24_redkeyOK,DS_C_JUNI_IT_REDKEY,1);
	_ds_gamestatus_loadItem(&minimap_24x24_yellowkey,DS_C_JUNI_IT_YELLOWKEY,0);
	_ds_gamestatus_loadItem(&minimap_24x24_yellowkeyOK,DS_C_JUNI_IT_YELLOWKEY,1);

}   

/* Inits the gamestatus system. Sets up the variables needed by this system. Usually must be changed 
   only once in a playthrough */
void ds_gamestatus_initLoad() {
   _ds_actualStatusScreen = -2; // Since we load the game, we need to reload the GUI
   _ds_statusType = DS_C_STA_RADAR;
	int i;
	for (i = 0; i < _RADARSIZE; i++) {
		minimap_radarBuffer[i] = 1; // Update
	}	
}   

/* Resets the gamestatus system. Must be done *after* exiting a screen */
void ds_gamestatus_reset() {
   // Resets minimap
   if (minimap_600x240_smallImage.loaded)
   	ds_15bpp_delete(&minimap_600x240_smallImage);
   
   // Resets icons? No, they have to stay in memory

	// Radar update
	int i;
	for (i = 0; i < _RADARSIZE; i++) {
		minimap_radarBuffer[i] = 1; // Update
	}	
}

/* Finishes the gamestatus system.*/
void ds_gamestatus_finish() {
   // Resets minimap
   if (minimap_256x192_menu.loaded)
   	ds_15bpp_delete(&minimap_256x192_menu);
}


/* Gets the status */
int ds_gamestatus_getStatus() {
   return _ds_statusType;
}

/* Gets the status Screen */
int ds_gamestatus_getActualStatusScreen() {
  	return _ds_actualStatusScreen;   	
}


/* Sets the status Screen */
void ds_gamestatus_setActualStatusScreen(int stat) {
   _ds_actualStatusScreen = stat;
}

/* Forces a status */
int ds_gamestatus_forceStatus(int sta) {
   _ds_statusType = sta;
   return 1;
}      

/* Paints a particular item */
int _ds_gamestatus_paintItem(int global, int item, int owned) {
   int x = 0;
   int y = 0;
   ds_t_15bpp *ima = NULL; 
   
   // First, it gets the image 
	switch (item) {
	   case DS_C_JUNI_IT_RUN:
	      if (owned) {
	         ima = &minimap_24x24_runOK;
	      } else {
	         ima = &minimap_24x24_run;
			}      
			x = 8;
			y = 0;
	      break;
	   case DS_C_JUNI_IT_CLIMB:
	      if (owned) {
	         ima = &minimap_24x24_climbOK;
	      } else {
	         ima = &minimap_24x24_climb;
			}      
			x = 8 + 24;
			y = 0;			 
	      break;
	   case DS_C_JUNI_IT_DBLJUMP:
	      if (owned) {
	         ima = &minimap_24x24_dbljumpOK;
	      } else {
	         ima = &minimap_24x24_dbljump;
			}      
			x = 8 + (24 * 2);
			y = 0;			 
	      break;
	   case DS_C_JUNI_IT_JUMP:
	      if (owned) {
	         ima = &minimap_24x24_jumpOK;
	      } else {
	         ima = &minimap_24x24_jump;
			}      
			x = 8 + (24 * 3);
			y = 0;			 
	      break;
	   case DS_C_JUNI_IT_EYE:
	      if (owned) {
	         ima = &minimap_24x24_eyeOK;
	      } else {
	         ima = &minimap_24x24_eye;
			}      
			x = 8 + (24 * 4);
			y = 0;			 			 
	      break;
	   case DS_C_JUNI_IT_ENEMY:
	      if (owned) {
	         ima = &minimap_24x24_enemyOK;
	      } else {
	         ima = &minimap_24x24_enemy;
			}      
			x = 8 + (24 * 5);
			y = 0;			 			 
	      break;
	   case DS_C_JUNI_IT_UMBRELLA:
	      if (owned) {
	         ima = &minimap_24x24_umbrellaOK;
	      } else {
	         ima = &minimap_24x24_umbrella;
			}      
			x = 8 + (24 * 6);
			y = 0;			 			 
	      break;
	   case DS_C_JUNI_IT_HOLOGRAM:
	      if (owned) {
	         ima = &minimap_24x24_hologramOK;
	      } else {
	         ima = &minimap_24x24_hologram;
			}      
			x = 8 + (24 * 7);
			y = 0;			 			 
	      break;
	   case DS_C_JUNI_IT_BLUEKEY:
	      if (owned) {
	         ima = &minimap_24x24_bluekeyOK;
	      } else {
	         ima = &minimap_24x24_bluekey;
			}      
			x = 8 + (24 * 8);
			y = 4;			 			 
	      break;
	   case DS_C_JUNI_IT_VIOLETKEY:
	      if (owned) {
	         ima = &minimap_24x24_violetkeyOK;
	      } else {
	         ima = &minimap_24x24_violetkey;
			}      
			x = 8 + (24 * 9);
			y = 4;			 			 
	      break;
	   case DS_C_JUNI_IT_REDKEY:
	      if (owned) {
	         ima = &minimap_24x24_redkeyOK;
	      } else {
	         ima = &minimap_24x24_redkey;
			}      
			x = 8 + (24 * 8);
			y = 16;			 			 
	      break;
	   case DS_C_JUNI_IT_YELLOWKEY:
	      if (owned) {
	         ima = &minimap_24x24_yellowkeyOK;
	      } else {
	         ima = &minimap_24x24_yellowkey;
			}      
			x = 8 + (24 * 9);
			y = 16;			 			 
	      break;
	}      
	
	// Copy the image to the buffer/screen		
	if (global) {
		ds_15bpp_putScreenAlpha(ds_global_getScreen1(),ima,x,y);
	} else {
		ds_15bpp_paintScreenAlpha(1,ima,x,y);
	}   		
		
	return 1;
}

/* Executes an update to the upper screen. */
int _ds_gamestatus_updateScreen(int stascr, int param) {
   char filen[255];
	int generalGUI = 0;
	ds_t_object *object;   
	void *myiterator;
	int radarx, radary;
	int radardraw;
	int i,j,x,y;
	int global = 0;
	int change = 0;
	int item = 0;
	int minimap = 0;
	int paint = 0;
	int rc;
	
   char tempstr[255];
   char tempstr_1[255];
	sprintf(tempstr_1,"INIT: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
		
	// Zero - update parameters
	switch (param) {
		case _DS_C_CT_LAUNCH:
			global = 1;
			change = 0;
			item = 1;
			minimap = 1;
			paint = 0;
			break;
		case _DS_C_CT_LAUNCHAFTER:
			global = 0;
			change = 0;
			item = 0;
			minimap = 0;
			paint = 1;
			break;
		case _DS_C_CT_UPDATE:
			global = 0;
			change = 0;
			item = 0;
			minimap = 1;
			paint = 0;
			break;
		case _DS_C_CT_CHANGEMODE:
			global = 0;
			change = 1;
			item = 1;
			minimap = 1;
			paint = 1;
			break;
	}

   // First, if this is a global update, puts the new upper screen in the Screen-1 buffer. 
   //-------------------------------------------------------------------------------------
   if ((global) && (!change)) {
		// First, small fix for unknown GUIs
		if (stascr == -2)
			stascr = -1;
      if ((_ds_actualStatusScreen == -2) || (_ds_actualStatusScreen != stascr)) {
         // Delete the actual GUI screen, if already exists
   		if (minimap_256x192_menu.loaded)
   			ds_15bpp_delete(&minimap_256x192_menu);      
   		// Reload the GUI screen
		   if (stascr == -1) {
		      sprintf(filen,"%s%s/Gui_game.png",DS_DIR_MAIN,DS_DIR_SYS);
		      generalGUI = 1;
			} else {
		      sprintf(filen,"%s%s/Gui_%d.png",ds_global_world.dir,DS_DIR_SYS,stascr);
				if (!ds_util_fileExists(filen)) {
					sprintf(filen,"%s%s/Gui_game.png",DS_DIR_MAIN,DS_DIR_SYS);
					generalGUI = 1;
				}			
			}   
		   if (!ds_15bpp_load(filen, &minimap_256x192_menu, 1,0)) {
				sprintf(ds_global_string,"Upper Screen Gui (%d/%d)",_ds_actualStatusScreen,stascr);
				ds_global_errorHalt(ds_global_string);
				//--HALT--//
			}
			// Fills the actual name of the tale
			if (generalGUI) { 		
				PA_CenterSmartText16bBuf_RAW(minimap_256x192_menu.png_screen, 
											256,192,  // Image
											0, 136,  // base
											256, 168, // max
											ds_global_world.name, PA_RGB(0,0,0), 4, 1); // Features		
				PA_CenterSmartText16bBuf_RAW(minimap_256x192_menu.png_screen, 
											256,192,  // Image
											0, 168,  // base
											256, 184, // max
											ds_global_world.author, PA_RGB(0,0,0), 2, 1); // Features		
			}		
			// OK, I have this new one :-D
			_ds_actualStatusScreen = stascr;
		}   			

		// Now, display the status screen on the upper screen
		ds_15bpp_putScreen(ds_global_getScreen1(),&minimap_256x192_menu,0,0,1);
	}
	
	sprintf(tempstr_1,"LOAD: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
	
	// Now, put the items that Juni has
	//---------------------------------
	if (item) {
	   // ALL Items must be drawn
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_RUN, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_RUN)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_CLIMB, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_CLIMB)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_DBLJUMP, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_DBLJUMP)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_JUMP, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_EYE, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_EYE)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_ENEMY, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_ENEMY)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_UMBRELLA, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_UMBRELLA)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_HOLOGRAM, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_HOLOGRAM)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_BLUEKEY, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_BLUEKEY)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_VIOLETKEY, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_VIOLETKEY)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_REDKEY, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_REDKEY)) == 0)
	   	return 0;
	   if (_ds_gamestatus_paintItem((global || change), DS_C_JUNI_IT_YELLOWKEY, 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_YELLOWKEY)) == 0)
	   	return 0;
	   	
	   _ds_statusItems = ds_global_juni.item;
	} else {
	   // Only changed items must be drawn
	   if (_ds_statusItems != ds_global_juni.item) {
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_RUN) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_RUN))
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_RUN, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_RUN)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_CLIMB) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_CLIMB))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_CLIMB, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_CLIMB)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_DBLJUMP) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_DBLJUMP))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_DBLJUMP, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_DBLJUMP)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_JUMP) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_JUMP, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_EYE) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_EYE))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_EYE, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_EYE)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_ENEMY) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_ENEMY))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_ENEMY, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_ENEMY)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_UMBRELLA) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_UMBRELLA))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_UMBRELLA, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_UMBRELLA)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_HOLOGRAM) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_HOLOGRAM))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_HOLOGRAM, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_HOLOGRAM)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_BLUEKEY) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_BLUEKEY))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_BLUEKEY, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_BLUEKEY)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_VIOLETKEY) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_VIOLETKEY))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_VIOLETKEY, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_VIOLETKEY)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_REDKEY) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_REDKEY))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_REDKEY, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_REDKEY)) == 0)
			   	return 0;
	      if (ds_util_bitOne16(_ds_statusItems,DS_C_JUNI_IT_YELLOWKEY) != 
					ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_YELLOWKEY))			   	
			   if (_ds_gamestatus_paintItem(global, DS_C_JUNI_IT_YELLOWKEY, 
							ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_YELLOWKEY)) == 0)
			   	return 0;
			   	
			_ds_statusItems = ds_global_juni.item;
	   }   
	}
	
	sprintf(tempstr_1,"ITEMS: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
      	   
	// Now, updates the miniscreen
	//----------------------------
	if (minimap) {
		if (_ds_statusType == DS_C_STA_RADAR) {
			// RADAR
			//--------------------------------------------------------
			// First, copy the miniscreen onto the Screen Buffer (global) / Screen (local)
			if (global) {
				ds_15bpp_putScreen(ds_global_getScreen1(),&minimap_600x240_smallImage,0,32,
					1);
			} else {
				// We will update the screen part by part, need to save speed!
				ds_15bpp_paintScreenSpecial(1,&minimap_600x240_smallImage,0,32,1,minimap_radarBuffer);
				// Radar update
				for (rc = 0; rc < _RADARSIZE; rc++) {
					minimap_radarBuffer[rc] = 0; // DO NOT Update
				}	

			}   		
			// Now, get the objects and paint them in the screen
			ds_t_15bpp *minimap_8x8_ = NULL;
			myiterator = ds_linkedlist_startIterator(&ds_global_objects);
			while ((object = ds_linkedlist_getIterator(&ds_global_objects,&myiterator)) != NULL) {
				if (ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR)) {
					// Get *Corrected* coordinates
					radardraw = 1;
					//radarx = ((object->x + ((object->xs > 24)?(object->xs >> 1):0) ) * 256) / 600;
					radarx = ((object->x + (object->xs >> 1) ) * 256) / 600;
					if ((radarx < 8) || (radarx > (256 - 8)))
						radardraw = 0;
					//radary = ((object->y + ((object->ys > 24)?(object->ys >> 1):0) ) * 102) / 240;
					radary = ((object->y + (object->ys >> 1) ) * 102) / 240;
					if ((radary < 8) || (radary > (102 - 8)))
						radardraw = 0;         
					// Choose the type of icon
					if (object->type == DS_C_OBJ_ITEM) {
						minimap_8x8_ = &minimap_8x8_blueIcon;
					} else {
						if (object->blink > 0) {
							if (((ds_global_tick % 4) < 2) == 0)
								minimap_8x8_ = NULL;
							else 
								minimap_8x8_ = &minimap_8x8_redIcon;
						}   
						else {
							if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_ENEMY)) {
								if ((ds_util_bitOne16(object->flags,DS_C_OBJ_F_WARN)) ||
									 (ds_util_bitOne16(object->flags,DS_C_OBJ_F_WARN_SPECIAL))) {
									minimap_8x8_ = &minimap_8x8_yellowIcon;
								} else {   
									minimap_8x8_ = &minimap_8x8_greenIcon;
								} 		
							} else {
								minimap_8x8_ = &minimap_8x8_greenIcon;
							}     	
						}  	
					}
					// Draw
					if (minimap_8x8_ != NULL) {
						if (radardraw) {
							if (global) {      
								ds_15bpp_putScreen(ds_global_getScreen1(),minimap_8x8_,radarx,32 + (radary),0);
							} else {
								ds_15bpp_paintScreen(1,minimap_8x8_,radarx,32 + (radary),0);
							}      
						} else {			
							if (global) {      
								ds_15bpp_putScreenCropped(ds_global_getScreen1(),minimap_8x8_,radarx,32 + (radary),0,0,32,256,32 + 102);
							} else {
								ds_15bpp_paintScreenCropped(1,minimap_8x8_,radarx,32 + (radary),0,0,32,256,32 + 102);         
							}      
						} 
						for (rc = radary; rc < radary + 8; rc++) {
							minimap_radarBuffer[rc] = 1; // UPDATE
							if (radary >= _RADARSIZE)
								break;
						}	
					}			
				}   
			}   
			// Finally, paints juni!!!!
			radardraw = 1;
			radarx = ((ds_global_juni.x + 12) * 256) / 600;
			if ((radarx < 0) || (radarx >= (256 - 8)))
				radardraw = 0;
			radary = ((ds_global_juni.y + 12) * 102) / 240;
			if ((radary < 0) || (radary >= (102 - 8)))
				radardraw = 0;         
			if (radardraw) {
				if (global) {
					ds_15bpp_putScreen(ds_global_getScreen1(),&minimap_8x8_juniIcon,radarx,32 + (radary),0);
				} else {
					ds_15bpp_paintScreen(1,&minimap_8x8_juniIcon,radarx,32 + (radary),0);
				}     	
			} else {
				if (global) {
					ds_15bpp_putScreenCropped(ds_global_getScreen1(),&minimap_8x8_juniIcon,radarx,32 + (radary),0,0,32,256,32 + 102);
				} else {
					ds_15bpp_paintScreenCropped(1,&minimap_8x8_juniIcon,radarx,32 + (radary),0,0,32,256,32 + 102);
				}     	
			}  
			for (rc = radary; rc < radary + 8; rc++) {
				minimap_radarBuffer[rc] = 1; // UPDATE
				if (radary >= _RADARSIZE)
					break;
			}
		}	// END RADAR
		else if (_ds_statusType == DS_C_STA_MAP) {
			// MAP
			//--------------------------------------------------------
			// Variables
			ds_t_15bpp *mapIn = NULL;
			ds_t_15bpp *mapIn2 = NULL;
			ds_t_15bpp *mapOut = NULL;
			int yini = 0;
			int xini = 0;
			int xsize = 0;
			int ysize = 0;
			int xxi = 0;
			int xxe = 0;
			int yyi = 0;
			int yye = 0;
			int xcorr = 0;
			switch (ds_global_world.size) {
				case 0: // SMALL
					mapIn = &minimap_8x8_mapIn;
					mapIn2 = &minimap_8x8_mapIn2;
					mapOut = &minimap_8x8_mapOut;
					yini = -12;
					xini = -12;
					xsize = 14;
					ysize = 14;
					xxi = -10;
					xxe = 9;
					yyi = -4;
					yye = 4;
					break;
				case 1: // MEDIUM
					mapIn = &minimap_8x8_mapIn_m;
					mapIn2 = &minimap_8x8_mapIn2_m;
					mapOut = &minimap_8x8_mapOut_m;
					yini = -1;
					xini = -3;
					xsize = 10;
					ysize = 10;
					xxi = -13;
					xxe = 13;
					yyi = -5;
					yye = 6;
					xcorr = 1;
					break;
				case 2: // LARGE
					mapIn = &minimap_8x8_mapIn_l;
					mapIn2 = &minimap_8x8_mapIn2_l;
					mapOut = &minimap_8x8_mapOut_l;
					yini = -5;
					xini = -5;
					xsize = 7;
					ysize = 7;
					xxi = -20;
					xxe = 18;
					yyi = -8;
					yye = 8;
					xcorr = 1;
					break;
			}
			if (mapIn == NULL) {
				ds_global_errorHalt("ASS: Paint Radar");
				// ----- HALT -----
			}
			// Minimap!
			if ((global) || (change)) {
				y = yini;
				for (j = yyi; j <= yye; j++) {
					x = xini;
					for (i = xxi; i <= xxe; i++) {
						if (ds_world_getVisitedRoom(ds_global_map.x + i,ds_global_map.y + j)) {
							ds_15bpp_putScreenCropped(ds_global_getScreen1(),mapIn,x,32 + y,0,0,32,256,32 + 102);
						} else {
							ds_15bpp_putScreenCropped(ds_global_getScreen1(),mapOut,x,32 + y,0,0,32,256,32 + 102);
						}      
						x += xsize;
					}
					y += ysize;
				}			   
			} else {
				if ((ds_global_tick % 60) == 0) {
					ds_15bpp_paintScreen(1,mapIn,xini + (xsize * abs(xxi)) + xcorr,
						32 + yini + (ysize * abs(yyi)),0);
				} else if ((ds_global_tick % 60) == 30) {
					ds_15bpp_paintScreen(1,mapIn2,xini + (xsize * abs(xxi)) + xcorr,
						32 + yini + (ysize * abs(yyi)),0);
				}     
			}     
		}  // END MAP 
	}

	sprintf(tempstr_1,"MAP: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);

	
	// Updates the items stored by Juni
	//---------------------------------
	
	// And finally... paints!... but only if it was global or change :-)
	//------------------------------------------------------------------
	if (paint) {
   	ds_global_paintScreen(1,ds_global_getScreen1(),0,0); // <TODO> - Create a pSCropped 
   }  
   
	sprintf(tempstr_1,"PAINT: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
	
	//ds_global_errorHalt(tempstr);
   
   return 1;
}      

/* Launches the gamestatus system. Loads a certain status screen (-1 if we use the "global" status screen) */
int ds_gamestatus_launch(int stascr) {
	// Load the small screen
	ds_t_15bpp minimap_600x240;
	ds_15bpp_initRaw(&minimap_600x240,ds_global_map.tileMap, 600, 240, 1); 
		// No need to delete this one
	if (!ds_15bpp_resize(&minimap_600x240_smallImage,&minimap_600x240,256,102)) {
		sprintf(ds_global_string,"Minimap");
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
	}   
	
	// Paints the special objects in the small screen (NOTE: They should be already loaded)
   if (!ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_HASCO)) {
		_ds_gamestatus_updateSmallScreen(600, 240, 256, 102);
	} else {	
		_ds_gamestatus_updateSmallScreenCO(600, 240, 256, 102);
	}	

	// Finally, paints everything!
	int res = _ds_gamestatus_updateScreen(stascr, _DS_C_CT_LAUNCH);
	return res;
}

/* Updates the gamestatus system AFTER a launch */
int ds_gamestatus_launchAFTER(int stascr) {
	return _ds_gamestatus_updateScreen(stascr, _DS_C_CT_LAUNCHAFTER);
}

/* Updates the status screen, in a normal way */
int ds_gamestatus_updateScreen() {
   return _ds_gamestatus_updateScreen(-1, _DS_C_CT_UPDATE);
}   

#ifdef DEBUG_KSDS
void _finishMode(int mode) {
   switch (mode) {
      case DS_C_STA_MAP:
         break;
      case DS_C_STA_RADAR:
         break;
      case DS_C_STA_DEBUG:
         PA_ClearTextBg(1);
         break;         
      case DS_C_STA_DEBUG2:
         PA_ClearTextBg(1);
         break;         
   }   
}   
void _changeMode(int mode) {
   switch (mode) {
      case DS_C_STA_MAP:
         _ds_statusType = DS_C_STA_RADAR;
			int rc;
			for (rc = 0; rc < _RADARSIZE; rc++) {
				minimap_radarBuffer[rc] = 1; // DO NOT Update
			}	
         break;
      case DS_C_STA_RADAR:
         _ds_statusType = DS_C_STA_DEBUG;
         break;
      case DS_C_STA_DEBUG:
         _ds_statusType = DS_C_STA_DEBUG2;
         break;         
      case DS_C_STA_DEBUG2:
         _ds_statusType = DS_C_STA_MAP;
         break;         
   }   
}   
void _startMode(int mode) {
   switch (mode) {
      case DS_C_STA_MAP:
         _ds_gamestatus_updateScreen(-1, _DS_C_CT_CHANGEMODE);
         break;
      case DS_C_STA_RADAR:
         break;
      case DS_C_STA_DEBUG:
         break;         
      case DS_C_STA_DEBUG2:
         break;         
   }      
}   
/* Change the status screen output - MAP, RADAR, DEBUG */
void ds_gamestatus_changeMode() {
   // First, clean things depending on the status I had :-)
   _finishMode(_ds_statusType);
   // Changes status
   _changeMode(_ds_statusType);
   // Now, prepare things for the new status!
   _startMode(_ds_statusType);
}
#else
void _finishMode(int mode) {
   switch (mode) {
      case DS_C_STA_MAP:
         break;
      case DS_C_STA_RADAR:
         break;
   }   
}   
void _changeMode(int mode) {
   switch (mode) {
      case DS_C_STA_MAP:
         _ds_statusType = DS_C_STA_RADAR;
			int rc;
			for (rc = 0; rc < _RADARSIZE; rc++) {
				minimap_radarBuffer[rc] = 1; // Update
			}
         break;
      case DS_C_STA_RADAR:
         _ds_statusType = DS_C_STA_MAP;
         break;
   }   
}   
void _startMode(int mode) {
   switch (_ds_statusType) {
      case DS_C_STA_MAP:
         _ds_gamestatus_updateScreen(-1, _DS_C_CT_CHANGEMODE);
         break;
      case DS_C_STA_RADAR:
         break;
   }      
}   
/* Change the status screen output - MAP, RADAR */
void ds_gamestatus_changeMode() {
   // First, clean things depending on the status I had :-)
   _finishMode(_ds_statusType);
   // Changes status
   _changeMode(_ds_statusType);
   // Now, prepare things for the new status!
   _startMode(_ds_statusType);
}
#endif   

/* Change the status screen output to a special mode */
void ds_gamestatus_pushMode(int mode) {
   if (_ds_stackedstatusType == DS_C_STA_) {
		_ds_stackedstatusType = _ds_statusType;
			_finishMode(_ds_statusType);
	   _ds_statusType = mode;
	   	_startMode(_ds_statusType);
	}   
}

/* Leaves the special mode and goes back to the old gamestatus screen */
void ds_gamestatus_popMode() {
   	_finishMode(_ds_statusType);
   _ds_statusType = _ds_stackedstatusType;
   	_startMode(_ds_statusType);
   _ds_stackedstatusType = DS_C_STA_;
}

/* Paints normal information */
void ds_gamestatus_normalOutput(u8 screen, u16 x, u16 y, char* text) {
   PA_OutputSimpleText(screen,x,y + 4,text);
}

/* Paints debug information */
void ds_gamestatus_debugOutput(u8 screen, u16 x, u16 y, char* text, int type) {
   if (_ds_statusType == type) {
   	ds_gamestatus_normalOutput(screen,x,y,text);
	}   
}
   
/*

Map Scanner:
   Blit: 14x14, 18mapx7map / 20x9 (252 [2-2], 98 [2-2]).
   +PAINT: 
      for x-4,x+4
      for x-9,x+10
		INIX = -12 INIY = -12  
*/
