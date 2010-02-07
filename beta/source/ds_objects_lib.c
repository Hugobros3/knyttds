/*
Copyright (c) 2008 Rodrigo Roman and Ramon Roman (rrc2soft)
(Original Game "Knytt Stories" (c) Nicklas "Nifflas" Nygren)

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

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_3dsprite.h"
#include "ds_3dspritehdd.h"
#include "ds_util.h"
#include "ds_juni.h"
#include "ds_map.h"
#include "ds_ini.h"
#include "ds_dialogue.h"
#include "ds_state.h"
#include "ds_objects.h"
#include "ds_world.h"
#include "ds_customobj.h"
#include "ds_gamestatus.h"
#include "ds_music.h"
#include <stdlib.h>
#include <math.h>


/* Starts Object - Init variables. */
int ds_objects_lib_initObject(u8 bank, u8 obj, ds_t_object *object) {
   int i;
   
   object->managed = 0;
   object->bank = bank;
   object->obj = obj;
   object->flags = 0;
   object->_deleteme = 0;
   for (i = 0; i < DS_C_LIMITS_VAROBJ; i++) {
      object->inner[i] = 0;
   }
   for (i = 0; i < DS_C_LIMITS_VARPOBJ; i++) {
      object->innerpar[i] = 0;
   }   
   object->inner_tick = 0;
   object->blink = 0;
	
	return 1;
}   

/* Starts Object - Preloads its image/sprite */
int ds_objects_lib_initObjectImage(u8 bank, u8 obj, ds_t_object *object) {
      
	object->sprite = ds_3dsprite_load(bank,obj,object->x,object->y,!ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_IMA));
	if (object->sprite == -1) {
		sprintf(ds_global_string,"CALL ds_3dsprite_load\n(%d,%d)(b:%d,o:%d)",object->x,object->y,bank,obj);
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
	}
	
	object->xs = ds_3dsprite_getXSize(object->sprite);
	object->ys = ds_3dsprite_getYSize(object->sprite);	
		
	if (ds_util_bitOne16(object->flags,DS_C_OBJ_F_INVISIBLE)) {
		ds_3dsprite_markInvisible(object->sprite,1);
	}		
	
	return 1;
}

/* Starts CUSTOM Object - Preloads its image/sprite, but manage special CO quirks */
int ds_objects_lib_initCustomObjectImage(u8 bank, u8 obj, ds_t_object *object) {
	ds_t_co *co;
	   
	// Seeks the CO
	if (!ds_customobj_exists(obj)) {
		sprintf(ds_global_string,"CO not found in .ini\n(o:%d)",obj);
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
	}
	co = ds_customobj_get(obj);
   
   // Special preload
   if (!ds_3dspritehdd_loadCO(obj, co->Image, &(co->Tile_Width), &(co->Tile_Height))) {
		sprintf(ds_global_string,"CO load error@InitCOImage\n(o:%d)",obj);
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
   }   
   
   // Starts normal load   
	object->sprite = ds_3dsprite_load(bank,obj,object->x,object->y,!ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_IMA));
	if (object->sprite == -1) {
		sprintf(ds_global_string,"CALL ds_3dsprite_load\n(%d,%d)(b:%d,o:%d)",object->x,object->y,DS_C_CO_BANK,obj);
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
	}
	
	object->xs = ds_3dsprite_getXSize(object->sprite);
	object->ys = ds_3dsprite_getYSize(object->sprite);	
	
	if (ds_util_bitOne16(object->flags,DS_C_OBJ_F_INVISIBLE)) {
		ds_3dsprite_markInvisible(object->sprite,1);
	}
	
	// Changes a flag on the map (contains COs / big COs)
	ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_HASCO);
	if (!ds_3dsprite_hasHWSprite(object->sprite))
		ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_HASBIGCO);		
	
	return 1;
}

/* Returns 1 if the object is an event or an item */
int ds_objects_lib_iseventitem(u8 type) {
   return ((type == DS_C_OBJ_EVENT) || (type == DS_C_OBJ_ITEM));
}   

/* Returns a "corrected" phytagorean distance object <-> Juni. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) 
	Why "corrected"? to extend the range of certain objects (e.g. dissapearing cubes!)  */
int ds_objects_lib_distancePhyJuniCorrected(ds_t_object *object, int cvirtual) {
   int mX;
   int mY;
   int m;
   int correctedx = object->x + (object->xs >> 1); // for calculating the distance using the center of the object
   int correctedy = object->y + (object->ys >> 1);   
   if (cvirtual) {
	   mX = abs((ds_juni_getXVirtual() + 12) - correctedx);
   	mY = abs((ds_juni_getYVirtual() + 12) - correctedy);
   } else {   
	   mX = abs((ds_global_juni.x + 12) - correctedx);
   	mY = abs((ds_global_juni.y + 12) - correctedy);
	}	
	m = sqrt((mX*mX)+(mY*mY));
	return m;
}

/* Returns 1 if the object is near juni. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) */
int ds_objects_lib_isNearJuni(ds_t_object *object, int pix, int cvirtual) {
   return (ds_objects_lib_distancePhyJuniCorrected(object,cvirtual) < pix);
/*   int correctedx = object->x + (object->xs >> 1); // for calculating the distance using the center of the object
   int correctedy = object->y + (object->ys >> 1);
   if (cvirtual) {
	   return (( abs((ds_juni_getXVirtual() + 12) - correctedx) < pix) && 
				  ( abs((ds_juni_getYVirtual() + 12) - correctedy) < pix));
   } else {   
	   return (( abs((ds_global_juni.x + 12) - correctedx) < pix) && 
				  ( abs((ds_global_juni.y + 12) - correctedy) < pix));
	}*/	  
}

/* Returns the distance object <-> Juni. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) */
int ds_objects_lib_distanceJuni(ds_t_object *object, int cvirtual) {
   int mX;
   int mY;
   int correctedx = object->x + (object->xs >> 1); // for calculating the distance using the center of the object
   int correctedy = object->y + (object->ys >> 1);   
   if (cvirtual) {
	   mX = abs((ds_juni_getXVirtual() + 12) - correctedx);
   	mY = abs((ds_juni_getYVirtual() + 12) - correctedy);
   } else {   
	   mX = abs((ds_global_juni.x + 12) - correctedx);
   	mY = abs((ds_global_juni.y + 12) - correctedy);
	}	
   return (mX + mY);
} 

/* Returns a "corrected" distance object <-> Juni. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) 
	Why "corrected"? to extend the range of certain objects (e.g. dissapearing cubes!)  */
int ds_objects_lib_distanceJuniCorrected(ds_t_object *object, int cvirtual) {
   int mX;
   int mY;
   int correctedx = object->x + (object->xs >> 1); // for calculating the distance using the center of the object
   int correctedy = object->y + (object->ys >> 1);   
   if (cvirtual) {
	   mX = abs((ds_juni_getXVirtual() + 12) - correctedx);
   	mY = abs((ds_juni_getYVirtual() + 12) - correctedy);
   } else {   
	   mX = abs((ds_global_juni.x + 12) - correctedx);
   	mY = abs((ds_global_juni.y + 12) - correctedy);
	}	
	return (mX > mY)?((mX >> 2) + mY):(mX + (mY >> 2));
}

/* Returns the distance object <-> Juni X. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) */
int ds_objects_lib_distanceJuniX(ds_t_object *object, int cvirtual, int absolute) {
   int mX;
   int correctedx = object->x + (object->xs >> 1);
   if (cvirtual) {
	   mX = (ds_juni_getXVirtual() + 12) - correctedx;
   } else {   
	   mX = (ds_global_juni.x + 12) - correctedx;
	}	
	if (absolute)
   	return abs(mX);
   else
   	return mX;
} 

/* Returns the distance object <-> Juni Y. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) */
int ds_objects_lib_distanceJuniY(ds_t_object *object, int cvirtual, int absolute) {
   int mY;
   int correctedy = object->y + (object->ys >> 1);
   if (cvirtual) {
	   mY = (ds_juni_getYVirtual() + 12) - correctedy;
   } else {   
	   mY = (ds_global_juni.y + 12) - correctedy;
	}	
	if (absolute)
   	return abs(mY);
   else
   	return mY;
}

/* Check if an object is over an object */
int ds_objects_lib_stepObject(ds_t_object *object) {
	return (
				(ds_juni_isOnTheGround()) && 
	         (ds_objects_lib_distanceJuniX(object,0,1) < (object->xs >> 1)) &&
	         (ds_global_juni.y == object->y)
			  ); 
}
  
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/

/* Behaviour: "signal" dialog */
void ds_objects_lib_beh_dialogue(ds_t_object *object, int type) {
   ds_dialogue_update(type);
}   

/* Behaviour: "shift" - change map! */
void ds_objects_lib_beh_shift(ds_t_object *object, int type) {
   int doshift = 0;
   int newx, newy, dx, dy;
   int oldposx, oldposy, posx, posy;
   int quantize;
   int flag = 0;
   int absolute;
   int checktmp, checktmp2;
	int shiftsound;
   char _shift_string[255];
   int shiftToOtherMap = 0;
   
   // "Zero", if Hologram is in place and I deny holograms, then there is nothing to do
   checktmp = ds_ini_getboolean(ds_global_world.worldini,
		   		ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_DENYHOLOGRAM,type,0,0,0),
   				0);
   checktmp2 = ds_ini_getboolean(ds_global_world.worldini,
		   		ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_ONTOUCH,type,0,0,0),
   				0);
   if ((checktmp) && (checktmp2) && (ds_global_juni.hologramOn))
   	return;
   	
   // "Zero+", if this shift should be ignored, then it is ignored
   if (((object->x / 24) == ds_global_world.shiftNoX) && ((object->y / 24) == ds_global_world.shiftNoY))
   	return;
   
   // First, check the type of shift - by contact or by pressing "down"
   doshift = ds_ini_getboolean(ds_global_world.worldini,
		   		ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_ONTOUCH,type,0,0,0),
   				0);
   if (!doshift) {
      // Not by contact... OK, check if I'm pressing "down" *ONLY*
      // <TODO> Check contact of the sprite of Juni
      if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_DOWN)) {
         doshift = 1;
      }   
   }   
   if (doshift) {
      // Teleport - Map...
   	absolute = ds_ini_getboolean(ds_global_world.worldini,
		   				ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_ABSOLUTETARGET,type,0,0,0),
   						0);      
   	dx = ds_ini_getint(ds_global_world.worldini,
		   			ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_MAPXREL,type,0,0,0),
   					0);
   	dy = ds_ini_getint(ds_global_world.worldini,
		   			ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_MAPYREL,type,0,0,0),
   					0);
   	if (!absolute) {
	      newx = ds_global_map.x + dx;
	      newy = ds_global_map.y + dy;	      			
   	} else {
	      newx = dx;
	      newy = dy;	      			   	   
   	}   
   	// Teleport - Juni's Position
   	quantize = ds_ini_getboolean(ds_global_world.worldini,
		   				ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_QUANTIZE,type,0,0,0),
   						1);
   	if (quantize) {
   	   // Centers Juni in the actual tile
   	   oldposx = object->x;
   	   oldposy = object->y;   	   
   	} else {
   	   // Teleport without centering in the tile
   	   oldposx = ds_global_juni.x;
   	   oldposy = ds_global_juni.y;   	   
		}      
   	dx = (ds_ini_getint(ds_global_world.worldini,
		   			ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_POSXREL,type,0,0,0),
   					0) * 24);
   	dy = (ds_ini_getint(ds_global_world.worldini,
		   			ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_POSYREL,type,0,0,0),
   					0) * 24);
   	if (!absolute) {
	      posx = oldposx + dx;
	      posy = oldposy + dy;	      			
   	} else {
	      posx = dx;
	      posy = dy;	      			   	   
   	}   						
   		   
	   // Flags! Set Flags
   	sprintf(_shift_string,"%s",
					ds_ini_getstring(ds_global_world.worldini,
								ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_FLAGON,type,0,0,0),
								"")
				);
		if (!PA_CompareText("",_shift_string)) {
		   // Flag ON!
		   if ((_shift_string[0] >= '0') && (_shift_string[0] <= '9')) {
		      // Number of Flag!!!!
		      if (sscanf(_shift_string, "%d", &checktmp) > 0) {
		         ds_global_juni.flag = ds_util_bitSet16(ds_global_juni.flag,checktmp);
		      }   
		   } else {
		      // Juni's Power!
		      if (sscanf(_shift_string, "Power%d", &checktmp) > 0) {
		         ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,checktmp);
		      }   
			}      
		}   
   	sprintf(_shift_string,"%s",
					ds_ini_getstring(ds_global_world.worldini,
								ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_FLAGOFF,type,0,0,0),
								"")
				);
		if (!PA_CompareText("",_shift_string)) {
		   // Flag OFF!
		   if ((_shift_string[0] >= '0') && (_shift_string[0] <= '9')) {
		      // Number of Flag!!!!
		      if (sscanf(_shift_string, "%d", &checktmp) > 0) {
		         ds_global_juni.flag = ds_util_bitDel16(ds_global_juni.flag,checktmp);
		      }   
		   } else {
		      // Juni's Power!
		      if (sscanf(_shift_string, "Power%d", &checktmp) > 0) {
		         ds_global_juni.item = ds_util_bitDel16(ds_global_juni.item,checktmp);
		         // WARNING!!!!! - Now we need a "consistency" check for Umbrella/Hologram
		         // It is implemented on ds_juni_init.
		      }   
			}      
		}   
		// Load the sound for this particular teleport
		shiftsound = 1; // Default
   	sprintf(_shift_string,"%s",
					ds_ini_getstring(ds_global_world.worldini,
								ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_SOUND,type,0,0,0),
								"")
				);
		if (PA_CompareText(_shift_string,"None")) {
			shiftsound = 0; // None
		} else if (PA_CompareText(_shift_string,"Switch")) {
			shiftsound = 2; // Switch
		} else if (PA_CompareText(_shift_string,"Door")) {
			shiftsound = 3; // Door
		} else if (PA_CompareText(_shift_string,"Electronic")) {
			shiftsound = 4; // Electronic
		}
		// Finally..Teleport!!!!
   	sprintf(_shift_string,"%s",
					ds_ini_getstring(ds_global_world.worldini,
								ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_CUTSCENE,type,0,0,0),
								"")
				);
		// First, save game if should
   	checktmp = ds_ini_getboolean(ds_global_world.worldini,
	   		ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_SAVE,type,0,0,0),
				0);
		if (checktmp) {
		   ds_world_updateSavegameParam(newx, newy, 
					posx, posy, 
					ds_global_juni.item, ds_global_juni.flag,
					ds_gamestatus_getActualStatusScreen());
		}
		// Now, launch game/loadgame!
		if (PA_CompareText("",_shift_string)) {
		   // LAST DETAIL!!!!!! If the map is the same, just move Juni to a new place!!!!
		   if ((newx == ds_global_map.x) && (newy == ds_global_map.y)) {
				// LAST LAST!!!!!!! If I teleport *to the same tile I am*, do nothing!!!!!
				if ((posx == oldposx) && (posy == oldposy)) {
					ds_music_playSpecialSound(shiftsound); // Just play sound!
					return; // AND THAT's IT!!! (ds_global_world.shift = 0)
				} else {
					// Teleport!
					ds_juni_updateSprites(posx, posy); // <TODO> : call to ds_juni_init with flags
					// Also, tell the system that any shift "on arrival" WON'T work
					ds_global_world.shiftNoX = posx / 24; // Need Tile Pos!
					ds_global_world.shiftNoY = posy / 24;
					ds_music_playSpecialSound(shiftsound);
				}
		   } else {   
				// Teleport Case 1: Normal Teleport
				shiftToOtherMap = 1;
				ds_state_assignState(DSKNYTT_GAME);
		      ds_state_var_setInt(0,newx);
				ds_state_var_setInt(1,newy);
				ds_state_var_setInt(2,posx); // This is in absolute position
				ds_state_var_setInt(3,posy); // This is in absolute position
				flag = 0;
				ds_state_var_setInt(4,flag);
				ds_state_var_setInt(5,ds_gamestatus_getActualStatusScreen());
				ds_state_var_setInt(6,shiftsound); // Sound
			}			
		} else {
		   // Teleport Case 2: Cutscene Teleport
		   shiftToOtherMap = 1;
			ds_state_assignState(DSKNYTT_LOADGAME);
			ds_state_var_setStr(_shift_string); // Cutscene to launch
			ds_state_var_setInt(0,DS_C_LOADGAME_CUTSCENE); // Da game!
			ds_state_var_setInt(1,0); // NO Reset
			ds_state_var_setInt(2,ds_global_world.saveslot); // Specific save state
			ds_state_var_setInt(3,shiftsound); // Sound to use
		}      
      // Tells external systems that I am going to shift...
      ds_global_world.shift = 1;
		// ...and Other World details
		if (shiftToOtherMap) {	
			// "Came from shift" system - Point out!!!
		   ds_global_world.shift_cameFrom = 1;
		   ds_global_world.shiftx = ds_global_map.x;
		   ds_global_world.shifty = ds_global_map.y;
		   ds_global_world.shiftabc = type;
		} 
   }  
}

/* Behaviour - executes ghost nature (alpha cycle) */
void ds_objects_lib_beh_ghost(ds_t_object *object, int minA, int maxA, int rndA) {
   int change = 0;
   int alpha = ds_3dsprite_getAlpha(object->sprite);
   
   if (object->inner[11] > alpha) {
   	alpha++;
   	change = 1;
   } else if (object->inner[11] < alpha) {
   	alpha--;
   	change = -1;
 	}  	
   if (object->inner[11] == alpha) {
      if (change == 1)
      	object->inner[11] = minA + PA_RandMax(rndA);
      else
      	object->inner[11] = maxA + PA_RandMax(rndA);
   }   
   
   ds_3dsprite_setAlpha(object->sprite, alpha);
}  

/* Behaviour - Inits ghost nature (alpha cycle) */
void ds_objects_lib_beh_ghostIni(ds_t_object *object, int ghost, int minA, int maxA, int rndA) {
   // Ghost test?... No eye, no ghost
   if (ghost)
   	object->_deleteme = !(ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_EYE));

	// Ghost alpha values
	if (PA_RandMax(2) == 1)
		object->inner[11] = minA + PA_RandMax(rndA);
	else
		object->inner[11] = maxA + PA_RandMax(rndA);
		
   ds_3dsprite_setAlpha(object->sprite, 70);
}

/* Behaviour - executes ghost nature (float style) */
void ds_objects_lib_beh_ghostFloat(ds_t_object *object) {

	int newy = 0;
	
	object->inner[10]++;
	
	if (object->inner[10] < 2)
		newy = 0;
	else if (object->inner[10] < (2+6))	   
		newy = -1;
	else if (object->inner[10] < (2+6+12))	   
		newy = -2;
	else if (object->inner[10] < (2+6+12+6))	   
		newy = -1;
	else if (object->inner[10] < (2+6+12+6+2)) //28	   
		newy = 0;
	else if (object->inner[10] < (28+2))
		newy = 0;
	else if (object->inner[10] < (28+2+6))	   
		newy = 1;
	else if (object->inner[10] < (28+2+6+12))	   
		newy = 2;
	else if (object->inner[10] < (28+2+6+12+6))	   
		newy = 1;
	else if (object->inner[10] < (28+2+6+12+6+2)) {
		newy = 0;
		object->inner[10] = 0;
	}		

	ds_3dsprite_setY(object->sprite,object->y + newy);   
}     

/* Behaviour: MMF2 Particle - Init */
int ds_objects_lib_beh_particleMMF2_Init(ds_t_object *object, int speedX, int speedY, 
						int gravity, int deceleration, int spriteMov) {
   object->inner[0] = speedX * 100; // Speed (X) of the particle (*100)
   object->inner[1] = speedY * 100; // Speed (Y) of the particle (*100)

	object->inner[5] = speedX * 120; // Cumulative Deceleration X (*100) - corrected
	object->inner[6] = speedY * 120; // Cumulative Deceleration Y (*100) - corrected

	object->inner[2] = gravity; // Gravity (Y) of the particle
	if (gravity > 0) {
		object->inner[3] = deceleration; // Deceleration of the particle when falling | other
	} else {
	   if (deceleration == 0) {
			object->inner[3] = 0; // Deceleration X
			object->inner[4] = 0; // Deceleration Y
	   } else {   
			object->inner[3] = (object->inner[5] * deceleration) / 1000; // Deceleration X
			if ((object->inner[3] == 0) && (object->inner[5] != 0))
				object->inner[3] = (object->inner[5] > 0)?1:-1;
			object->inner[4] = (object->inner[6] * deceleration) / 1000; // Deceleration Y
			if ((object->inner[4] == 0) && (object->inner[6] != 0))
				object->inner[4] = (object->inner[6] > 0)?1:-1;
		}		
	}   

   object->inner[7] = 0; // Cumulative X speed of the particle (*100)
   object->inner[8] = 0; // Cumulative Y speed of the particle (*100)
   	// Remember: "At speed 100, it would move at 12.5 pixels per frame" (50 fps)
   	// Therefore: Speed 100 = 625 px/second => 10.41 pixels / frame
   	// Approx: Speed = 10, needs to move 1 pixel.
   object->inner[9] = 0; // Actual Sprite
   object->inner[10] = spriteMov; // Sprite Speed
   
   return 1;
}   

/* Behaviour: Set parameters to a particle following. Speeds are in x1024 */
int ds_objects_lib_beh_particleFollowing_Init(ds_t_object *object, int waitTime, int speedX, int speedY, int gravity, int gravityCorr, int maxspeedX, int maxspeedY) {
   if (object != NULL) {
      
/*      object->inner[0] = 0;
		object->inner[2] = speedX;
		object->inner[3] = speedY;
		object->inner[4] = maxspeedX << 10;	
		object->inner[5] = maxspeedY << 10;			
		object->inner[6] = gravity;
		object->inner[7] = gravityCorr;
		object->inner[8] = waitTime;  
		object->inner[9] = waitTime;*/
	}	
	return 1; 
}

/* Behaviour: Make 4 Following particles. */
int ds_objects_lib_par_make4Following(ds_t_object *object, int sec, int waitTime, int speedX, int speedY, int gravity, int gravityCorr, int maxspeedX, int maxspeedY) {
	int x;
	int y;
	ds_t_object *particle;
	
	if ((ds_global_tick % (60 * sec)) == 0) {
			ds_music_playSound("Homing Shot", 0, 0);
	      x = ds_3dsprite_getX(object->sprite) - 2;
	      y = ds_3dsprite_getY(object->sprite) - 16;
		   particle = ds_objects_createParticle(x + 7, y + 5, object->layer, 12);  // UL
		   ds_objects_lib_beh_particleMMF2_Init(particle,-12,-12,1,0,3);
		   particle = ds_objects_createParticle(x + 16, y + 5, object->layer, 12); // UR
			ds_objects_lib_beh_particleMMF2_Init(particle,12,-12,1,0,3);
		   particle = ds_objects_createParticle(x + 7, y + 18, object->layer, 12); // DL
		   ds_objects_lib_beh_particleMMF2_Init(particle,-12,12,1,0,3);
		   particle = ds_objects_createParticle(x + 16, y + 18, object->layer, 12);// DR
		   ds_objects_lib_beh_particleMMF2_Init(particle,12,12,1,0,3);
   }   
	return 1;
}   


/* Behaviour: Particle Following Juni */
/*int ds_objects_lib_beh_particleFollowing(ds_t_object *object) {
//	[0] - State
//	[1] - Sprite
//	[2] - SpeedX
//	[3] - SpeedY
//	[4] - MaxSpeedX	
//	[5] - MaxSpeedY	
//	[6] - Gravity
//	[7] - Gravity Correction (to allow an innacurate "follow juni")	
//	[8] - Waiting time
//	[9] - Max Waiting time	

   int collided = 0;
   int outofmap = 0;
   int gravityX;
   int gravityY;
   
   // TEST - PA_OutputText(1,0,15,"%d %d %d %d         ",object->inner[2],object->inner[3],object->x,object->y);
   
   switch (object->inner[0]) {
      case 0:
         // NORMAL MOVE
		   // Can I Move?         
		   if (object->inner[8] > 0)
		   	object->inner[8]--;
		   if (object->inner[8] <= 0) {
		      object->inner[8] = object->inner[9];
		      
		      // "Gravity" affects...
		      gravityX = object->x - ds_juni_getXVirtual();
		      gravityY = object->y - ds_juni_getYVirtual();
		      
		      // Gravity X
		      if (gravityX > 0) {
			      object->inner[2] -= object->inner[6];
			      object->inner[2] = ((0 - object->inner[4]) >= object->inner[2])?(0 - object->inner[4]):object->inner[2];
		   	} else {
			      object->inner[2] += object->inner[6];
			      object->inner[2] = (object->inner[4] <= object->inner[2])?object->inner[4]:object->inner[2];		   		 
		   	}   
		      
		      // Gravity Y
		      if (gravityY > 0) {
			      object->inner[3] -= object->inner[6];
			      object->inner[3] = ((0 - object->inner[5]) >= object->inner[3])?(0 - object->inner[5]):object->inner[3];
		   	} else {
			      object->inner[3] += object->inner[6];
			      object->inner[3] = (object->inner[5] <= object->inner[3])?object->inner[5]:object->inner[3];		   		 
		   	}   
		      
			   // Move entity
			   object->x += (object->inner[2] >> 10); // 1024, simulating floats
			   object->y += (object->inner[3] >> 10); // 1024, simulating floats
			   
				// Move the sprite itself
				ds_3dsprite_setX(object->sprite,object->x);
				ds_3dsprite_setY(object->sprite,object->y);
		
			   // Check "hitting the ground" AND "going outside the map"
		   	collided = ds_map_collideFlagCenter(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 ds_3dsprite_getX(object->sprite),
											 ds_3dsprite_getY(object->sprite),
											 &ds_global_map.tileMapCol);
				if (!collided) {
				   outofmap = ds_map_outOfMap(ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 ds_3dsprite_getX(object->sprite),
											 ds_3dsprite_getY(object->sprite));
				}   
				if (outofmap) {
				   // It ends for me...
				   object->_deleteme = 1;
				   return 1;
				} else if (collided) {
				   // It ends for me...
				   object->_deleteme = 1;
				   return 1;				   
				}   
			}   
			break;
		case 1:
		   if (object->inner[8] > 0)
		   	object->inner[8]--;
		   if (object->inner[8] <= 0) {
		      object->inner[8] = object->inner[9];   
			   object->inner[1]++;
			   if (object->inner[1] >= ds_3dsprite_getMaxFrame(object->sprite)) {
				   // It ends for me...
				   object->_deleteme = 1;
				   return 1;		      
			   }  
				ds_3dsprite_setFrame(object->sprite,object->inner[1]);
			}			
		   break;
	}			
	return 1;
}*/

/* Behaviour: Set parameters to an entity following. Speeds are in x1024 */
int ds_objects_lib_beh_entityFollowing_Init(ds_t_object *object, int waitTime, int speedX, int speedY, int gravity, int gravityCorr, int maxspeedX, int maxspeedY) {
   if (object != NULL) {
      object->inner[0] = 0;
		object->inner[2] = speedX;
		object->inner[3] = speedY;
		object->inner[10] = speedX;  
		object->inner[11] = speedY;		
		object->inner[4] = maxspeedX;	
		object->inner[5] = maxspeedY;			
		object->inner[6] = gravity;
		object->inner[7] = gravityCorr;
		object->inner[8] = waitTime;  
		object->inner[9] = waitTime;
	}	
	return 1; 
}

/* Behaviour: Entity Following (Floating) Juni. */
int ds_objects_lib_beh_entityFollowing(ds_t_object *object, int slowCollided) {
   /*
	[0] - State
	[1] - Sprite
	[2] - SpeedX
	[3] - SpeedY
	[4] - MaxSpeedX	
	[5] - MaxSpeedY	
	[6] - Gravity
	[7] - Gravity Correction (to allow an innacurate "follow juni")	
	[8] - Waiting time
	[9] - Max Waiting time	
	[10] - SpeedX-Accumulated
	[11] - SpeedY-Accumulated
*/
   int collided = 0;
   int outofmap = 0;
   int gravityX;
   int gravityY;
   int newx;
   int newy;
   int spdX;
   int spdY;
   
   switch (object->inner[0]) {
      case 0:
         // NORMAL MOVE
		   // Can I Move?         
		   if (object->inner[8] > 0)
		   	object->inner[8]--;
		   if (object->inner[8] <= 0) {
		      object->inner[8] = object->inner[9];
		      
		      // Move Sprite (Cycle)
			   object->inner[1]++;
			   if (object->inner[1] >= ds_3dsprite_getMaxFrame(object->sprite)) {
			      object->inner[1] = 0;
			   }  
				ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		      
		      // "Gravity" affects...
		      gravityX = object->x - ds_juni_getXVirtual();
		      gravityY = object->y - ds_juni_getYVirtual();
		      
		      // Gravity X
		      if (gravityX > 0) {
			      object->inner[2] -= object->inner[6];
			      object->inner[2] = ((0 - object->inner[4]) >= object->inner[2])?(0 - object->inner[4]):object->inner[2];
		   	} else {
			      object->inner[2] += object->inner[6];
			      object->inner[2] = (object->inner[4] <= object->inner[2])?object->inner[4]:object->inner[2];		   		 
		   	}   
		      
		      // Gravity Y
		      if (gravityY > 0) {
			      object->inner[3] -= object->inner[6];
				      object->inner[3] = ((0 - object->inner[5]) >= object->inner[3])?(0 - object->inner[5]):object->inner[3];
		   	} else {
			      object->inner[3] += object->inner[6];
			      object->inner[3] = (object->inner[5] <= object->inner[3])?object->inner[5]:object->inner[3];		   		 
		   	}   
		   	
		   	// Funny detail... if we are in a collision, we must go SLOWER!!!!!!
		   	collided = ds_map_collideFlagCenter(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 ds_3dsprite_getX(object->sprite),
											 ds_3dsprite_getY(object->sprite),
											 &ds_global_map.tileMapCol);		   	
		      
			   // Move entity with acumulated speed
			   if (slowCollided) {
				   object->inner[10] += object->inner[2] >> collided; // Accumulate Speed, corrected by collision
				   object->inner[11] += object->inner[3] >> collided;
				} else {
				   object->inner[10] += object->inner[2]; // Accumulate Speed
				   object->inner[11] += object->inner[3];
				}      
			   spdX = object->inner[10] / 1024;       // Get possible Speed
			   spdY = object->inner[11] / 1024;
			   newx = object->x + (spdX);// Change coordinates
			   newy = object->y + (spdY);
			   object->inner[10] = object->inner[10] - (spdX * 1024); // Update accumulated Speed
			   object->inner[11] = object->inner[11] - (spdY * 1024);
		
			   // Check "going outside the map"
				outofmap = ds_map_outOfMap(ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 newx,
											 newy);
				if (outofmap) {
				   // DO...NOT...MOVE! ^_-
				} else  {
				   // Move ^^ 
				   object->x = newx;
				   object->y = newy;
					ds_3dsprite_setX(object->sprite,object->x);
					ds_3dsprite_setY(object->sprite,object->y);				   
				}   		
			}   			
			break;
	}			
	return 1;
}

/* Behaviour: Set parameters to a flying bullet */
int ds_objects_lib_beh_particleFlyingBullet_Init(ds_t_object *object, int waitTime, int direction, int speed) {
   if (object != NULL) {
      object->inner[0] = 0;
		object->inner[2] = direction;
		object->inner[3] = speed;
		object->inner[8] = waitTime;  
		object->inner[9] = waitTime;
	}	
	return 1; 
}

/* Behaviour: Make 4 Flying Bullets. */
int ds_objects_lib_par_make4FlyingBullets(ds_t_object *object, int sec, int speed, int waitTime) {
	int x;
	int y;
	ds_t_object *particle;
	
	if ((ds_global_tick % (sec * 60)) == 0) {
			ds_music_playSound("Tiny Shot", 0, 0);
	      x = ds_3dsprite_getX(object->sprite) - 2;
	      y = ds_3dsprite_getY(object->sprite) - 16;
		   particle = ds_objects_createParticle(x + 16, y + 5, object->layer, 4 + 1); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,1,speed);
		   particle = ds_objects_createParticle(x + 16, y + 18, object->layer, 4 + 3); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,3,speed);
		   particle = ds_objects_createParticle(x + 7, y + 18, object->layer, 4 + 5); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,5,speed);		   
		   particle = ds_objects_createParticle(x + 7, y + 5, object->layer, 4 + 7); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,7,speed);		   
		   
		   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
   }   
	return 1;
}   

/* Behaviour: Make 6 Flying Bullets. */
int ds_objects_lib_par_make6FlyingBullets(ds_t_object *object, int sec, int speed, int waitTime) {
	int x;
	int y;
	ds_t_object *particle;
	
	if ((ds_global_tick % (sec * 60)) == 0) {
			ds_music_playSound("Tiny Shot", 0, 0);
	      x = ds_3dsprite_getX(object->sprite) - 2;
	      y = ds_3dsprite_getY(object->sprite) - 16;
		   particle = ds_objects_createParticle(x + 11, y + 5, object->layer, 4 + 0); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,0,speed);	      
		   particle = ds_objects_createParticle(x + 16, y + 5, object->layer, 4 + 1); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,1,speed);
		   particle = ds_objects_createParticle(x + 16, y + 18, object->layer, 4 + 3); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,3,speed);
		   particle = ds_objects_createParticle(x + 11, y + 18, object->layer, 4 + 4); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,4,speed);		   
		   particle = ds_objects_createParticle(x + 7, y + 18, object->layer, 4 + 5); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,5,speed);		   
		   particle = ds_objects_createParticle(x + 7, y + 5, object->layer, 4 + 7); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,7,speed);		   
		   
		   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
   }   
	return 1;
} 

/* Behaviour: Make 8 Flying Bullets. */
int ds_objects_lib_par_make8FlyingBullets(ds_t_object *object, int sec, int speed, int waitTime) {
	int x;
	int y;
	ds_t_object *particle;
	
	if ((ds_global_tick % (sec * 60)) == 0) {
			ds_music_playSound("Tiny Shot", 0, 0);
	      x = ds_3dsprite_getX(object->sprite) - 2;
	      y = ds_3dsprite_getY(object->sprite) - 16;
		   particle = ds_objects_createParticle(x + 11, y + 5, object->layer, 4 + 0); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,0,speed);	      
		   particle = ds_objects_createParticle(x + 16, y + 5, object->layer, 4 + 1); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,1,speed);
		   particle = ds_objects_createParticle(x + 16, y + 13, object->layer, 4 + 2); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,2,speed);		   
		   particle = ds_objects_createParticle(x + 16, y + 18, object->layer, 4 + 3); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,3,speed);
		   particle = ds_objects_createParticle(x + 11, y + 18, object->layer, 4 + 4); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,4,speed);		   
		   particle = ds_objects_createParticle(x + 7, y + 18, object->layer, 4 + 5); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,5,speed);		   
		   particle = ds_objects_createParticle(x + 7, y + 13, object->layer, 4 + 6); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,6,speed);		   		   
		   particle = ds_objects_createParticle(x + 7, y + 5, object->layer, 4 + 7); 
		   ds_objects_lib_beh_particleFlyingBullet_Init(particle,waitTime,7,speed);		   
		   
		   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
   }   
	return 1;
}   


/* Behaviour: Flying Bullet */
int ds_objects_lib_beh_particleFlyingBullet(ds_t_object *object) {
   /*
	[0] - State
	[2] - Direction
	[3] - Speed
	[8] - Waiting time
	[9] - Max Waiting time	
*/
   int collided = 0;
   int outofmap = 0;
   
   switch (object->inner[0]) {
      case 0:
         // NORMAL MOVE
		   // Can I Move?         
		   if (object->inner[8] <= 0) {
		      object->inner[8] = object->inner[9];
		      // Choose direction change
		      switch (object->inner[2]) {
		         case 0: // |^
		         	object->y-=object->inner[3];
		            break;
		         case 1: // />
		         	object->y-=object->inner[3];
		         	object->x+=object->inner[3];
		            break;
		         case 2: // ->
		         	object->x+=object->inner[3];
		            break;
		         case 3: // \>
		         	object->x+=object->inner[3];
		         	object->y+=object->inner[3];
		            break;
		         case 4: // |v
		         	object->y+=object->inner[3];
		            break;
		         case 5: // /<
		         	object->y+=object->inner[3];
		         	object->x-=object->inner[3];
		            break;
		         case 6: // -<
		         	object->x-=object->inner[3];
		            break;
		         case 7: // |\^
		         	object->x-=object->inner[3];
		         	object->y-=object->inner[3];
		            break;
		      }   
				// Move the sprite itself
				ds_3dsprite_setX(object->sprite,object->x);
				ds_3dsprite_setY(object->sprite,object->y);
		
			   // Check "hitting the ground" AND "going outside the map"
		   	collided = ds_map_collideFlagCenter(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 ds_3dsprite_getX(object->sprite),
											 ds_3dsprite_getY(object->sprite),
											 &ds_global_map.tileMapCol);
				if (!collided) {
				   outofmap = ds_map_outOfMap(ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 ds_3dsprite_getX(object->sprite),
											 ds_3dsprite_getY(object->sprite));
				}   
				if (outofmap) {
				   // It ends for me...
				   object->_deleteme = 1;
				   return 1;
				} else if (collided) {
				   // It ends for me...
				   object->_deleteme = 1;
				   return 1;				   
				   /* Next... allow particles to enter in the "fade away" state... for tests ^^
					object->inner[0] = 1;		   
					object->inner[1] = 0; 
					*/
				}   		      
			} else {
		   	object->inner[8]--;
			}   
   }   
   
   return 1;
}

/* Behaviour: Launch a bee bullet */
int ds_objects_lib_beh_beeBullet(ds_t_object *object, int indexBullet, int sec, int sec60, int maxBullet) {
   ds_t_object *particle;
   switch (object->innerpar[0]) {
      case 0:
         // Time to launch...
         //if ((ds_global_tick % (sec * 60)) == 0) {
         if (PA_RandMinMax(1,(sec * 60)) == 1) {
         	object->innerpar[0] = 1;
         	object->innerpar[1] = PA_RandMinMax(1,maxBullet);
         	object->innerpar[2] = PA_RandMinMax(sec60 / 5,sec60);
       	}  	
         break;
      case 1:
         // Sublaunch
         if (object->innerpar[2] == 0) {
            // Launch bee bullet
				ds_music_playSound("Tiny Shot", 0, 0);
            int beeRight = ds_3dsprite_lookRight_LR(object->sprite);
			   particle = ds_objects_createParticle((beeRight)?object->x+12:object->x+5, object->y, object->layer, (beeRight)?indexBullet:indexBullet+1); 
			   ds_objects_lib_beh_particleFlyingBullet_Init(particle,1,(beeRight)?3:5,3);		         
			   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
            // Prepare next bee bullet (or not)
            object->innerpar[1]--;
            if (object->innerpar[1] == 0) {
               object->innerpar[0] = 0;
            } else {
               object->innerpar[2] = PA_RandMinMax(sec60 / 5,sec60);
            }   
       	} else {
			   object->innerpar[2]--; 	         
			}   
         break;
   }   
	return 1; 
}   

/* Behaviour: Set parameters to a particle falling. SpeedY is in x1024 */
int ds_objects_lib_beh_particleFalling_Init(ds_t_object *object, int waitTime, int speedX, int speedY, int gravity, int maxspeedY) {
   if (object != NULL) {
      object->inner[0] = 0;
		object->inner[2] = speedX;
		object->inner[3] = speedY;
		object->inner[4] = maxspeedY << 10;	
		object->inner[5] = gravity;
		object->inner[8] = waitTime;  
		object->inner[9] = waitTime;
	}	
	return 1; 
}

/* Behaviour: Make 4 particles fall. NOTE that SpeedY is given in 1=1024 */
int ds_objects_lib_par_make4ParticleFalling(ds_t_object *object, int sec, int maxSpeedX, int maxSpeedY, int particles) {
	int x;
	int y;
	ds_t_object *particle;
/*
	[0] - Actual SpeedX
	[1] - Actual SpeedY
*/
	if ((ds_global_tick % (sec * 60)) == 0) {
	   object->innerpar[0] = PA_RandMinMax(1,maxSpeedX);
	   object->innerpar[1] = PA_RandMinMax(0,maxSpeedY);	   
		object->innerpar[2] = ds_music_playSound("Liquid", 0, 0);
	   object->inner_tick = ds_global_tick;
	}   
	if (object->inner_tick != 0) {
	   if ((ds_global_tick % 2) == 0) {
	      x = ds_3dsprite_getX(object->sprite);
	      y = ds_3dsprite_getY(object->sprite);
		   particle = ds_objects_createParticle(x + 14, y - 19, object->layer, 3); // UR
  		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						15 + PA_RandMinMax(0,1), 
						-15 - PA_RandMinMax(0,1), 
				 		5, 0, 0);				 		
		   //ds_objects_lib_beh_particleFalling_Init(particle,1,object->innerpar[0],0 - object->innerpar[1],100,5);
		   particle = ds_objects_createParticle(x + 4, y - 19, object->layer, 3); // UL
  		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						-15 - PA_RandMinMax(0,1),  
						-15 - PA_RandMinMax(0,1), 
				 		5, 0, 0);				 		
		   //ds_objects_lib_beh_particleFalling_Init(particle,1,0 - object->innerpar[0],0 - object->innerpar[1],100,5);
		   particle = ds_objects_createParticle(x + 14, y, object->layer, 3);  // DR
  		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						15 + PA_RandMinMax(0,1), 
						-3 - PA_RandMinMax(0,1), 
				 		5, 0, 0);				 		
		   //ds_objects_lib_beh_particleFalling_Init(particle,1,object->innerpar[0],1024 - object->innerpar[1],100,5);
		   particle = ds_objects_createParticle(x + 4, y, object->layer, 3); // DL
  		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						-15 - PA_RandMinMax(0,1), 
						-3 - PA_RandMinMax(0,1), 
				 		5, 0, 0);
		   //ds_objects_lib_beh_particleFalling_Init(particle,1,0 - object->innerpar[0],1024 - object->innerpar[1],100,5);	      
		   
		   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	   }   
	   if ((ds_global_tick - object->inner_tick) > (particles << 1)) {
	   	object->inner_tick = 0;
			ds_music_stopSoundChannel(object->innerpar[2]);
		}
	}   
	return 1;
}   

/* Behaviour: Make 2 particles fall. NOTE that SpeedY is given in 1=1024 */
int ds_objects_lib_par_make2ParticleFalling(ds_t_object *object, int sec, int maxSpeedX, int maxSpeedY, int particles) {
	int x;
	int y;
	ds_t_object *particle;
/*
	[0] - Actual SpeedX
	[1] - Actual SpeedY
*/
	if ((ds_global_tick % (sec * 60)) == 0) {
	   object->innerpar[0] = PA_RandMinMax(1,maxSpeedX);
	   object->innerpar[1] = PA_RandMinMax(0,maxSpeedY);	   
		object->innerpar[2] = ds_music_playSound("Liquid", 0, 0);
	   object->inner_tick = ds_global_tick;
	}   
	if (object->inner_tick != 0) {
	   if ((ds_global_tick % 2) == 0) {
	      x = ds_3dsprite_getX(object->sprite);
	      y = ds_3dsprite_getY(object->sprite);
		   particle = ds_objects_createParticle(x + 14, y - 19, object->layer, 3);  // UR
  		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						15 + PA_RandMinMax(0,1), 
						-20 - PA_RandMinMax(0,1), 
				 		5, 0, 0);				 		
		   //ds_objects_lib_beh_particleFalling_Init(particle,1,object->innerpar[0],0 - object->innerpar[1],100,5);
		   particle = ds_objects_createParticle(x + 4, y - 19, object->layer, 3); // UL
  		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						-15 - PA_RandMinMax(0,1), 
						-20 - PA_RandMinMax(0,1), 
				 		5, 0, 0);				 		
		   //ds_objects_lib_beh_particleFalling_Init(particle,1,0 - object->innerpar[0],0 - object->innerpar[1],100,5);
		   
		   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	   }   
	   if ((ds_global_tick - object->inner_tick) > (particles << 1)) {
	   	object->inner_tick = 0;
			ds_music_stopSoundChannel(object->innerpar[2]);
		}
	}   
	return 1;
}   

/* Behaviour: Launch particles. NOTE that SpeedY is given in 1=1024 */
int ds_objects_lib_par_launchParticles(ds_t_object *object, int sec, int when, int maxSpeedX, int maxSpeedY, int particles) {
	int x;
	int y;
	int distance;
	ds_t_object *particle;
/*
	[0] - Actual SpeedY
	[1] - Actual Gravity
*/

	// <TODO> FIXXXX!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	if ((ds_global_tick % (sec * 60)) == 0) {
	   distance = (object->x - ds_juni_getXVirtual());
	   if (distance < 0)
	   	distance = 24;
	   object->innerpar[1] = 100;
	   object->innerpar[0] = 2 << 10;
	   object->inner_tick = ds_global_tick;
	}   
	if (object->inner_tick != 0) {
	   if ((ds_global_tick % when) == 0) {
	      x = ds_3dsprite_getX(object->sprite);
	      y = ds_3dsprite_getY(object->sprite);
		   particle = ds_objects_createParticle(x + 7, y + 5, object->layer, 3); 
		   ds_objects_lib_beh_particleFalling_Init(particle,1,0 - maxSpeedX,0 - object->innerpar[0], object->innerpar[1] ,5);
		   
		   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	   }   
	   if ((ds_global_tick - object->inner_tick) > (particles << 1))
	   	object->inner_tick = 0;
	}   
	return 1;
}   


/* Behaviour: Particle falling */
int ds_objects_lib_beh_particleFalling(ds_t_object *object) {
   /*
	[0] - State
	[1] - Sprite
	[2] - SpeedX
	[3] - SpeedY
	[4] - MaxSpeedY	
	[5] - Gravity
	[8] - Waiting time
	[9] - Max Waiting time	
*/
   int collided = 0;
   int outofmap = 0;
   
   switch (object->inner[0]) {
      case 0:
         // NORMAL MOVE
		   // Can I Move?         
		   if (object->inner[8] > 0)
		   	object->inner[8]--;
		   if (object->inner[8] <= 0) {
		      object->inner[8] = object->inner[9];
		      
		      // Gravity affects...
		      object->inner[3] += object->inner[5];
		      object->inner[3] = (object->inner[4] <= object->inner[3])?object->inner[4]:object->inner[3];
		      
			   // Move entity
			   object->x += object->inner[2];
			   object->y += object->inner[3] >> 10; // 1024, simulating floats
			   
				// Move the sprite itself
				ds_3dsprite_setX(object->sprite,object->x);
				ds_3dsprite_setY(object->sprite,object->y);
		
			   // Check "hitting the ground" AND "going outside the map"
		   	collided = ds_map_collideFlagCenter(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 ds_3dsprite_getX(object->sprite),
											 ds_3dsprite_getY(object->sprite),
											 &ds_global_map.tileMapCol);
				if (!collided) {
				   outofmap = ds_map_outOfMap(ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 ds_3dsprite_getX(object->sprite),
											 ds_3dsprite_getY(object->sprite));
				}   
				if (outofmap) {
				   // It ends for me...
				   object->_deleteme = 1;
				   return 1;
				} else if (collided) {
				   // It ends for me...
				   object->_deleteme = 1;
				   return 1;				   
				   /* Next... allow particles to enter in the "fade away" state... for tests 
					object->inner[0] = 1;		   
					object->inner[1] = 0; 
					*/
				}   
			}   
			break;
		case 1:
		   if (object->inner[8] > 0)
		   	object->inner[8]--;
		   if (object->inner[8] <= 0) {
		      object->inner[8] = object->inner[9];   
			   object->inner[1]++;
			   if (object->inner[1] >= ds_3dsprite_getMaxFrame(object->sprite)) {
				   // It ends for me...
				   object->_deleteme = 1;
				   return 1;		      
			   }  
				ds_3dsprite_setFrame(object->sprite,object->inner[1]);
			}			
		   break;
	}			
	return 1;
}

/* Behaviour: MMF2 Particle - Generic Movement */
int ds_objects_lib_beh_particleMMF2_Move(ds_t_object *object, int speedX, int speedY, int coll) {
   int collided = 0;
   int outofmap = 0;
   
   // First, move entity
   object->x += speedX;
   object->y += speedY;
   
	// Move the sprite itself
	ds_3dsprite_setX(object->sprite,object->x);
	ds_3dsprite_setY(object->sprite,object->y);
	
   
   // Check "hitting the ground" (if applicable) AND "going outside the map"
   if (coll) {
		// Use a special collision test for particles
   	collided = ds_map_collideFlagCross(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
									 ds_3dsprite_getXSize(object->sprite),
									 ds_3dsprite_getYSize(object->sprite),
									 ds_3dsprite_getX(object->sprite),
									 ds_3dsprite_getY(object->sprite),
									 &ds_global_map.tileMapCol);
		if (!collided) {
		   outofmap = ds_map_outOfMap(ds_3dsprite_getXSize(object->sprite),
									 ds_3dsprite_getYSize(object->sprite),
									 ds_3dsprite_getX(object->sprite),
									 ds_3dsprite_getY(object->sprite));
		}   
		if ((collided) || (outofmap)) {
		   // It ends for me...
		   object->_deleteme = 1;
		   return collided;
		}   
	}		 
	return ((collided) || (outofmap));
}   
   
/* Behaviour: MMF2 Particle, Speed */
int ds_objects_lib_beh_particleMMF2_isStopped(ds_t_object *object) {
   if (object->inner[3] != 0) 
   	return ((object->inner[5] == 0) && (object->inner[6] == 0));
   else 
   	return ((object->inner[0] == 0) && (object->inner[1] == 0));
}   

/* Behaviour: MMF2 Particle */
int ds_objects_lib_beh_particleMMF2(ds_t_object *object, u8 slowdown) {
/*
   [0] Speed (X) of the particle (remember, * 100)
	[1] Speed (Y) of the particle (remember, * 100)
	[2] Gravity (Y) of the particle
	[3] Deceleration of the particle when falling | Deceleration - X
	[4] Deceleration - Y
	[5] Cumulative Deceleration - X
	[6] Cumulative Deceleration - Y
	[7] Cumulative X speed of the particle
   [8] Cumulative Y speed of the particle
   [9] Actual Sprite
   [10] Sprite Speed
	TODO - Deceleration
*/
   int movPixX;
   int movPixY;
   
   // SLOW DOWN!!!! - To achieve similar speeds of Knytt Stories
   if (slowdown >= 1) {
		if ((ds_global_tick % slowdown) == 0)
			return 0; 
	}			
   
   // First, cycles the particle movement
   if (object->inner[10] != 0) {
	   if ((ds_global_tick % object->inner[10]) == 0) {
		   object->inner[9]++;
		   if (object->inner[9] >= ds_3dsprite_getMaxFrame(object->sprite)) {
		      object->inner[9] = 0;
		   }  
			ds_3dsprite_setFrame(object->sprite,object->inner[9]);
	   }
	}
	
   // Move the particle!
   object->inner[7] += (object->inner[0] / 100);
	object->inner[8] += (object->inner[1] / 100);
   movPixX = object->inner[7] / 10; // REAL speed
   movPixY = object->inner[8] / 10; // REAL speed
   object->inner[7] -= (object->inner[7] / 10) * 10; // Works due to integer calculations...
   object->inner[8] -= (object->inner[8] / 10) * 10; // Works due to integer calculations...
   
   // Effects...
   if (object->inner[2] != 0) {
      object->inner[1] += (object->inner[2] * 20); // Gravity (internal) factor...
   }
   if (object->inner[2] != 0) {
   	// Gravity Deceleration
		if (object->inner[1] > 0) {   
		   if (object->inner[0] > 0) {
		   	object->inner[0] -= (object->inner[3] / 10) * 100; // Deceleration factor...
		   	if (object->inner[0] < 0)
		   		object->inner[0] = 0;
		 	} else if (object->inner[0] < 0) {
		   	object->inner[0] += (object->inner[3] / 10) * 100; // Deceleration factor...
		   	if (object->inner[0] > 0)
		   		object->inner[0] = 0;
		 	} 	
		}  
	} else {
	   // Normal Deceleration - X
	   if (object->inner[3] != 0) {
		   if (object->inner[0] > 0) {
		   	object->inner[0] = (object->inner[5] / 100) * 100; // Update Speed
		   	object->inner[5] -=  object->inner[3]; // Update Deceleration
		   	if ((object->inner[5] < 0) || (object->inner[0] == 0)) {
		   		object->inner[0] = 0;
		   		object->inner[5] = 0;
		  		} 		
		 	} else if (object->inner[0] < 0) {
		   	object->inner[0] = (object->inner[5] / 100) * 100; // Update Speed
		   	object->inner[5] -=  object->inner[3]; // Update Deceleration
		   	if ((object->inner[5] > 0) || (object->inner[0] == 0)) {
		   		object->inner[0] = 0;
		   		object->inner[5] = 0;
		  		} 		
		 	}  	   
		}	 	
	   // Normal Deceleration - Y
	   if (object->inner[4] != 0) {
		   if (object->inner[1] > 0) {
		   	object->inner[1] = (object->inner[6] / 100) * 100; // Update Speed
		   	object->inner[6] -=  object->inner[4]; // Update Deceleration
		   	if ((object->inner[6] < 0) || (object->inner[1] == 0)) {
		   		object->inner[1] = 0;
		   		object->inner[6] = 0;
		  		} 		
		 	} else if (object->inner[1] < 0) {
		   	object->inner[1] = (object->inner[6] / 100) * 100; // Update Speed
		   	object->inner[6] -=  object->inner[4]; // Update Deceleration
		   	if ((object->inner[6] > 0) || (object->inner[1] == 0)) {
		   		object->inner[1] = 0;
		   		object->inner[6] = 0;
		  		} 		
		 	}    	   
		}	 	
	}   		
	    

	// Move for real!
	return ds_objects_lib_beh_particleMMF2_Move(object, movPixX, movPixY, 1);
}   

/* Behaviour: MMF2 Particle Following Juni */
int ds_objects_lib_beh_particleMMF2Follow(ds_t_object *object, u8 slowdown) {
/*
   [0] (Initial) Speed (X) of the particle (remember, * 100)
	[1] (Initial) Speed (Y) of the particle (remember, * 100)
	[2] "Gravity" of the particle (speed correction)
	[3] ---------- (Deceleration, unused)
	[7] Cumulative X speed of the particle
   [8] Cumulative Y speed of the particle
   [9] Actual Sprite
   [10] Sprite Speed
	TODO - Deceleration
*/
   int movPixX;
   int movPixY;
   
   // SLOW DOWN!!!! - To achieve similar speeds of Knytt Stories
   if (slowdown >= 1) {
		if ((ds_global_tick % slowdown) == 0)
			return 0; 
	}			
   
   // First, cycles the particle movement
   if (object->inner[10] != 0) {
   if ((ds_global_tick % object->inner[10]) == 0) {
	   object->inner[9]++;
	   if (object->inner[9] >= ds_3dsprite_getMaxFrame(object->sprite)) {
	      object->inner[9] = 0;
	   }  
		ds_3dsprite_setFrame(object->sprite,object->inner[9]);
   }
	}
	
   // Move the particle!
   object->inner[7] += (object->inner[0] / 100);
	object->inner[8] += (object->inner[1] / 100);
   movPixX = object->inner[7] / 10; // REAL speed
   movPixY = object->inner[8] / 10; // REAL speed
   object->inner[7] -= (object->inner[7] / 10) * 10; // Works due to integer calculations...
   object->inner[8] -= (object->inner[8] / 10) * 10; // Works due to integer calculations...
   
   // Correct Speed
   int correctedx = object->x + (object->xs >> 1); 
   int correctedy = object->y + (object->ys >> 1);
	if ((ds_global_tick & 2) == 0) {
		object->inner[0] += ((ds_juni_getXVirtual() > correctedx)?object->inner[2]:-object->inner[2])*100;    
		object->inner[1] += ((ds_juni_getYVirtual() > correctedy)?object->inner[2]:-object->inner[2])*100;
	}	

	// Move for real!
	return ds_objects_lib_beh_particleMMF2_Move(object, movPixX, movPixY, 1);      
}   

/* Behaviour: Just Cycle. */
int ds_objects_lib_beh_cycle(ds_t_object *object, int waitTime) {
/* 
	[0]:STATE
	[1]:WAITING TIME
	[2]:ACTUAL FRAME
*/ 
	if (ds_3dsprite_getMaxFrame(object->sprite) == 1)
		return 1; // no need ;-)
   switch (object->inner[0]) {
      case 0:
         // INITIALIZE
         object->inner[1] = waitTime; // Waiting time
         object->inner[2] = 0; // Frame         
         object->inner[0] = 1; // Next state
         break;
      case 1:
         // WAITING TIME... + 1
         object->inner[1]--;
         if (object->inner[1] <= 0) {
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);            
//				ds_3dsprite_updateSprite(object->sprite);         	
         	object->inner[1] = waitTime; // Waiting time
         	object->inner[2]++;         	
         	if (object->inner[2] == ds_3dsprite_getMaxFrame(object->sprite))
					object->inner[2] = 0;      
         }   
         break;         
   }   
   // Everything went OK...
   return 1;
}

/* Behaviour: Cycle, looking at juni */
int ds_objects_lib_beh_cycleLook(ds_t_object *object, int waitTime) {
/* 
	[0]:STATE
	[1]:WAITING TIME
	[2]:ACTUAL FRAME
	[4]:LOOKING AT...
*/ 
	if (ds_3dsprite_getMaxFrame(object->sprite) == 1)
		return 1; // no need ;-)
   switch (object->inner[0]) {
      case 0:
         // INITIALIZE
         object->inner[1] = waitTime; // Waiting time
         object->inner[2] = 0; // Frame         
         object->inner[0] = 1; // Next state
         break;
      case 1:
         // WAITING TIME... + 1
         object->inner[1]--;
         if (object->inner[1] <= 0) {
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);            
//				ds_3dsprite_updateSprite(object->sprite);         	
         	object->inner[1] = waitTime; // Waiting time
         	object->inner[2]++;         	
		      if (object->inner[2] >= ds_3dsprite_getMaxFrame_LR(object->sprite,object->inner[4]==0?1:-1,1))
      			object->inner[2] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[4]==0?1:-1,1);
         }   
         // LOOK TEST...
			switch (object->inner[4]) {
			   case 0:
			      /* LOOK RIGHT... */
			      if (object->x > ds_global_juni.x) {
			         object->inner[4] = 1; // Look left state...
			         object->inner[2] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[4]==0?1:-1,1);
			      }   
			      break;
			   case 1:
			      /* LOOK LEFT... */
			      if (object->x < ds_global_juni.x) {
			         object->inner[4] = 0; // Look right state...
			         object->inner[2] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[4]==0?1:-1,1);
			      }   
			      break;	      
			}      
         break;         
   }   
   // Everything went OK...
   return 1;
}

/* Behaviour: Animation w/o cycle. */
int ds_objects_lib_beh_anim(ds_t_object *object, int waitTime, int startanim) {
/* 
	[0]:STATE
	[1]:WAITING TIME
	[2]:ACTUAL FRAME
*/ 
	int res = 0;
	
	if (ds_3dsprite_getMaxFrame(object->sprite) == 1)
		return 0; // no need ;-)
   switch (object->inner[0]) {
      case 0:
         // INITIALIZE
         object->inner[1] = waitTime; // Waiting time
         object->inner[2] = startanim; // Frame         
         object->inner[0] = 1; // Next state
         break;
      case 1:
         // WAITING TIME... + 1
         object->inner[1]--;
         if (object->inner[1] <= 0) {
            res = 1;
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);            
         	object->inner[1] = waitTime; // Waiting time
         	object->inner[2]++;         	
         	if (object->inner[2] == ds_3dsprite_getMaxFrame(object->sprite))
					object->inner[0] = 2; 
         }   
         break;
      case 2:
         // ETERNAL LOOP...
			break;         
   }   
   // Everything went OK...
   return res;
}

/* Behaviour: Cycle AND Die. Return 1 when dead */
int ds_objects_lib_beh_cycleDie(ds_t_object *object, int waitTime) {
/* 
	[0]:STATE
	[1]:WAITING TIME
	[2]:ACTUAL FRAME
*/ 
	int res = 0;
	
   switch (object->inner[0]) {
      case 0:
         // INITIALIZE
         object->inner[1] = waitTime; // Waiting time
         object->inner[2] = 0; // Frame         
         object->inner[0] = 1; // Next state
         break;
      case 1:
         // WAITING TIME... + 1
         object->inner[1]--;
         if (object->inner[1] <= 0) {
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);            
//				ds_3dsprite_updateSprite(object->sprite);         	
         	object->inner[1] = waitTime; // Waiting time
         	object->inner[2]++;         	
         	if (object->inner[2] == ds_3dsprite_getMaxFrame(object->sprite)) {
					object->_deleteme = 1;
					res = 1;
				}			
         }   
         break;         
   }   
   // Everything went OK...
   return res;
}   
   

/* Behaviour: Cycle back and forth */
int ds_objects_lib_beh_cycleBackForth(ds_t_object *object, int waitTime) {
/* 
	[0]:STATE
	[1]:WAITING TIME
	[2]:ACTUAL FRAME
*/ 
   switch (object->inner[0]) {
      case 0:
         // INITIALIZE
         object->inner[1] = waitTime; // Waiting time
         object->inner[2] = 0; // Frame         
         object->inner[0] = 1; // Next state
         break;
      case 1:
         // WAITING TIME... + 1
         object->inner[1]--;
         if (object->inner[1] <= 0) {
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);            
//				ds_3dsprite_updateSprite(object->sprite);         	
         	object->inner[1] = waitTime; // Waiting time
         	object->inner[2]++;         	
         	object->inner[0] = (object->inner[2] == ds_3dsprite_getMaxFrame(object->sprite))?2:1; // Next state            
         }   
         break;         
      case 2:
         // WAITING TIME... - 1
         object->inner[1]--;
         if (object->inner[1] <= 0) {
         	object->inner[2]--;
         	ds_3dsprite_setFrame(object->sprite,object->inner[2] % ds_3dsprite_getMaxFrame(object->sprite));
//				ds_3dsprite_updateSprite(object->sprite);
         	object->inner[1] = waitTime; // Waiting time
         	object->inner[0] = (object->inner[2] == 0)?1:2; // Next state
         }   
         break;         
   }   
   // Everything went OK...
   return 1;
}   

/* Behaviour: fly randomly */
void ds_objects_lib_beh_flyRandom(ds_t_object *object, int Speed, int waitingMovement, int waitingSpriteMovement, int waitingStop, int activeJuni) {
/*
	[0] - State
	[1] - Sprite
	[7] - Ignore Collision (optimization)	
	[8] - Waiting time
	[9] - Waiting spritetime
*/
	int move;
	int newx, newy;
	int collided;

	// Execute!
   switch (object->inner[0]) {
      case 0:
         // Movement?
         move = 0;
         if (activeJuni) {
            move = (ds_objects_lib_distanceJuni(object,1) < 24);
         } else {
	         object->inner[8]--;
   	      if (object->inner[8] <= 0)
   	      	move = 1;
   	   }   
   	   if (move) {
   	      // Let's move this critter!!!
   	      object->inner[0] = 1;
   	      object->inner[1] = 1;
   	      object->inner[7] = waitingStop;   	      
   	      object->inner[8] = waitingMovement;
   	      object->inner[9] = waitingSpriteMovement;
   	   }   
         break;
      case 1:
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
            // Always change sprite
	         if (object->inner[9] > 0)
   	      	object->inner[9]--;
      	   if (object->inner[9] <= 0) {            
	            ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[1] += 1;
	            if (object->inner[1] == ds_3dsprite_getMaxFrame(object->sprite))
	            	object->inner[1] = 1;
	            object->inner[9] = waitingSpriteMovement;
	         }   
	         // First, try to move            
				newx = object->x + PA_RandMinMax(0,2*Speed) - Speed;
				newy = object->y + PA_RandMinMax(0,2*Speed) - Speed;
				   
				collided = 0;				   
				if ((newx < 0) || (newx >= (600 - object->xs))) {
				   collided = 1;
				} else if ((newy < 0) || (newy >= (240 - object->ys))) {   
				   collided = 1;
				} else {
				 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 object->xs,
											 object->ys,
											 newx,
											 newy,
											 &ds_global_map.tileMapCol);
				      
				}  
				if (!collided) {
				   // Move
				   object->x = newx;
				   object->y = newy;					   
				   ds_3dsprite_setX(object->sprite,object->x);
				   ds_3dsprite_setY(object->sprite,object->y);					   				   
	      	}  
				object->inner[8] = waitingMovement; 
				// Note... shall we go down? - only when I have flown enough
				if (object->inner[7] > 0)
					object->inner[7]--;
				if (object->inner[7] == 0) {
					if (ds_map_collDown(object->x,object->y,
							ds_3dsprite_getXSize(object->sprite),ds_3dsprite_getYSize(object->sprite),
							ds_3dsprite_getYSize(object->sprite))) {
					   // The flyer want to relax... but let's see if Juni is around...
					   move = 0; // The opposite now... I don't wanna move! ^_-
					   if ((activeJuni) && (ds_objects_lib_distanceJuni(object,1) < 24))
					   	move = 1; // D***!!!!
					   if (!move) {
						   object->inner[0] = 0;
						   object->inner[1] = 0;
						   ds_3dsprite_setFrame(object->sprite,object->inner[1]);				   
						   object->inner[8] = PA_RandMinMax(waitingStop >> 1, waitingStop); 
						   object->inner[9] = 0;
						}   
					}   
				}			
			}			          
         break;
	}		
}   

/* Behaviour: fly around the screen */
void ds_objects_lib_beh_flyAround(ds_t_object *object, int Speed, 
											int minDist, int maxDist, 
											int minWaitingTime, int maxWaitingTime, int waitingMovement, int waitingSpriteMovement,
											int followJuni, int distanceJuni, int trembleSprite) {
/*
	[0] - State
	[1] - Sprite
	[2] - Distance X
	[3] - Distance Y
	[4] - Where to go X
	[5] - Where to go Y
	[6] - Actual Pix movement (max...0)
	[7] - "Slope"
	[8] - Waiting time
	[9] - Waiting spritetime
*/
	int distx = abs(ds_juni_getXVirtual() - object->x);
	int disty = abs(ds_juni_getYVirtual() - object->y);
	int distmax = (distx > disty)?distx:disty;
	int distmin = (distx < disty)?distx:disty;
	int collided;
	int letsmove;
	int newx = 0;
	int newy = 0;

   // Change sprite
   if (object->inner[9] > 0)
   	object->inner[9]--;
   if (object->inner[9] <= 0) {
      object->inner[1]++;
      if (object->inner[1] >= ds_3dsprite_getMaxFrame(object->sprite))
      	object->inner[1] = 0;
   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
      object->inner[9] = waitingSpriteMovement;
	}   
						
	// Execute!
   switch (object->inner[0]) {
      case 0:
         // STAY IN THE SAME AREA, TILL...
         if (object->inner[8] > 0) {
         	object->inner[8]--;
         	// Do not forget to tremble a bit!!!!!
			   ds_3dsprite_setX(object->sprite,object->x + PA_RandMinMax(0,trembleSprite) - (trembleSprite >> 1));
			   ds_3dsprite_setY(object->sprite,object->y + PA_RandMinMax(0,trembleSprite) - (trembleSprite >> 1));					            	
         } else  	
         if (object->inner[8] <= 0) {
            // Move... following Juni
            if (followJuni) {
	            // Should we move towards Juni?
               if (distanceJuni == -1) {
                  letsmove = 1; // Don't care about distance, just follow Juni
               } else if (distmin < distanceJuni) {
                  letsmove = 1; // Only if Juni is near enough...
					}      
				} else {
                  letsmove = 1; // Just move (c) :-)				   
				}
				// So... we move, or not????
				if (letsmove) {
				   // Distance & Position
				   if (followJuni)
				   	object->inner[6] = -1;
				   else if (minDist == maxDist) {
				   	object->inner[6] = minDist;
				 	}  	
				   else {
				   	object->inner[6] = PA_RandMinMax(minDist,maxDist);
				 	}  	
					if (followJuni) {
						object->inner[4] = ds_juni_getXVirtual();
					   object->inner[5] = ds_juni_getYVirtual();
				   	object->inner[2] = object->inner[4] - object->x;
					   object->inner[3] = object->inner[5] - object->y;
					} else {   				   
				   	object->inner[2] = ((PA_RandMinMax(0,2)-1) * object->inner[6]); // X-Dest
					   object->inner[3] = ((PA_RandMinMax(0,2)-1) * object->inner[6]); // Y-Dest
					   if ((object->inner[2] == 0) && (object->inner[3] == 0))
					   	object->inner[6] = minDist >> 4;
						object->inner[4] = object->x + object->inner[2];
					   object->inner[5] = object->y + object->inner[3];					   
					}   
					object->inner[7] = 0;
				   // State
				   object->inner[0] = 1;
				   object->inner[8] = waitingMovement;
				} else {
				   object->inner[8] = PA_RandMinMax(minWaitingTime,maxWaitingTime);
				}      
         }   
         break;
      case 1:
         // "MOVE" STATE
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
            if (followJuni) {
               // Change direction
					object->inner[4] = ds_juni_getXVirtual();
				   object->inner[5] = ds_juni_getYVirtual();
			   	object->inner[2] = object->inner[4] - object->x;
				   object->inner[3] = object->inner[5] - object->y;
				   object->inner[7] = 0;
               // Check movement
               if (distanceJuni == -1) {
                  letsmove = 1;
               } else if (distmin < distanceJuni) {
                  letsmove = 1; // Follow Juni!!! While she is near!
					} else {
					   letsmove = 0; // stop moving!
					} 
					// Too near Juni?
					if (letsmove)  
						if (distmax < 4)
							letsmove = 0; // Do not follow if I am too near... ^_-
            } else if (object->inner[6] == -1) {
               letsmove = 1;
            } else {
               object->inner[6]--;
               if (object->inner[6] >= 0) {
                  // Move :-)
               	letsmove = 1;
             	} else
               	letsmove = 0;
				}
				if (letsmove) {
				   // MOVEMENT "PER SE"
					// Movement!!				
					int i;
					int dx = object->inner[2];
					int dy = object->inner[3];
					newx = 0;
					newy = 0;
					for (i = 0; i < Speed; i++) {
						if (abs(dx) > abs(dy)) {	
							newx += (dx / abs(dx));
							object->inner[7] += abs(dy);
							if (object->inner[7] >= abs(dx)) {
							   object->inner[7] -= abs(dx);
							   newy += (dy / abs(dy));
							}   
						} else {
							newy += (dy / abs(dy));
							object->inner[7] += abs(dx);
							if (object->inner[7] >= abs(dy)) {
							   object->inner[7] -= abs(dy);
							   newx += (dx / abs(dx));
							}   						
						}			
					}			
					newx += object->x;
					newy += object->y;
				   
					collided = 0;				   
				   if ((newx < 0) || (newx >= (600 - object->xs))) {
				      collided = 1;
				   } else if ((newy < 0) || (newy >= (240 - object->ys))) {   
				      collided = 1;
				   } else {
				   	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 object->xs,
											 object->ys,
											 newx,
											 newy,
											 &ds_global_map.tileMapCol);
				      
				   }  
					if (collided) {
					   // "Stop" State
			         object->inner[0] = 2;
					} else {
					   // Great! - Move! - Also, tremble (fly)
					   object->x = newx;
					   object->y = newy;					   
					   ds_3dsprite_setX(object->sprite,object->x + PA_RandMinMax(0,trembleSprite) - (trembleSprite >> 1));
					   ds_3dsprite_setY(object->sprite,object->y + PA_RandMinMax(0,trembleSprite) - (trembleSprite >> 1));					   
					}   
           		object->inner[8] = waitingMovement;
				} else {
				  object->inner[0] = 2; // "Stop" state 
				}   	      
      	}   
         break;
      case 2:
         // "STOP" STATE
         object->inner[0] = 0;
		   object->inner[8] = PA_RandMinMax(minWaitingTime,maxWaitingTime);
			break;         
	}	
}   

/* Behaviour: fly around the screen, but try to *chof* Juni! This is a particular case of the flyAround method */
void ds_objects_lib_beh_flyAroundFaller(ds_t_object *object, int Speed, 
											int minDist, int maxDist, 
											int minWaitingTime, int maxWaitingTime, int waitingMovement, int waitingSpriteMovement,
											int distanceJuni, int trembleSprite) {
/*
	[0] - State
	[1] - Sprite
	[2] - Distance X
	[3] - Distance Y
	[4] - State "Chof"
	[6] - Actual Pix movement (max...0)
	[7] - "Slope"
	[8] - Waiting time
	[9] - Waiting spritetime
	
	"Faller" mechanism:
	   + "Select target" state. Chooses where to go 
	   	- If (Random(dist) && (Juni is below)), try to *chof* Juni (target = Juni VCoord)
	   	- Fly around the screen, but always try to go up
	   	. Change?
	   		- Always
	   + Normal state. 
	   	- Move to its destination
	   	. Change?
	   		- When finished
*/
	int distx = abs(ds_juni_getXVirtual() - object->x);
	int dir;
	int collided;
	int letsmove;
	int newx = 0;
	int newy = 0;

   // Change sprite (depends on the state *chof*-normal)
   if (object->inner[9] > 0)
   	object->inner[9]--;
   if (object->inner[9] <= 0) {
      object->inner[1]++;
      if (object->inner[1] >= ds_3dsprite_getMaxFrame_LR(object->sprite,object->inner[4]==0?1:-1,1))
      	object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[4]==0?1:-1,1);
   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
      object->inner[9] = waitingSpriteMovement;
	}   
						
	// Execute!
   switch (object->inner[0]) {
      case 0:
         // First, check if we should be *chof*-ing Juni
         if (object->inner[4] == 0) {
            // Check chances of *chof*-ing Juni
            if (
						((ds_juni_getYVirtual() - object->y) > 12) && // Over Juni...
						(distx < distanceJuni) && // Near enough (considering the X)
						(PA_RandMinMax(0,1) == 0) // Add a bit of randomness ^_- 1/2
					) {
               object->inner[4] = 1; // We should *chof* Juni!
            }     
         } else {
            object->inner[4] = 0; // After we try, we stop for a bit
         }            
			if (minDist == maxDist) {
		   	object->inner[6] = minDist;
		 	}  	
		   else {
		   	object->inner[6] = PA_RandMinMax(minDist,maxDist);
		 	}  	
			if (object->inner[4]) {
			   // Go for Juni!!!!
		   	object->inner[2] = ds_juni_getXVirtual() - object->x;
			   object->inner[3] = ds_juni_getYVirtual() - object->y;
			   object->inner[6] = sqrt((object->inner[2] * object->inner[2]) + (object->inner[3] * object->inner[3])); // d = sqrt(x^2+y^2)
			} else {   				   
			   // Go somewhere else... but always up! (or try to go up)
			   dir = PA_RandMinMax(0,2)-1;
		   	object->inner[2] = (dir * object->inner[6]); // X-Dest
		   	dir = PA_RandMinMax(0,2)-1;
		   	if (dir != 1)
				   object->inner[3] = (dir * object->inner[6]); // Y-Dest
				else 
					object->inner[3] = (dir * (object->inner[6] >> 4)); // Y-Dest / 16 (less movement)
			   if ((object->inner[2] == 0) && (object->inner[3] == 0))
			   	object->inner[6] = minDist >> 4;
			   else
				   object->inner[6] = sqrt((object->inner[2] * object->inner[2]) + (object->inner[3] * object->inner[3])); // d = sqrt(x^2+y^2)
			}   
			object->inner[7] = 0;
		   // State
		   object->inner[0] = 1;
		   object->inner[8] = waitingMovement >> object->inner[4]; // Fast if *Chof*!
		   // Sprite
		   object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[4]==0?1:-1,1);
         break;
      case 1:
         //PA_OutputText(1,0,5,"MOV: %d,%d (%d)[%d]       ",object->inner[2],object->inner[3],object->inner[6],object->inner[7]);
         // "MOVE" STATE
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
            object->inner[6]--;
            if (object->inner[6] >= 0) {
               // Move :-)
            	letsmove = 1;
          	} else
            	letsmove = 0;
				if (letsmove) {
				   // MOVEMENT "PER SE"
					// Movement!!				
					int i;					
					int dx = object->inner[2];
					int dy = object->inner[3];
					newx = 0;
					newy = 0;
					for (i = 0; i < Speed; i++) {
						if (abs(dx) > abs(dy)) {	
							newx += (dx / abs(dx));
							object->inner[7] += abs(dy);
							if (object->inner[7] >= abs(dx)) {
							   object->inner[7] -= abs(dx);
							   newy += (dy / abs(dy));
							}   
						} else {
							newy += (dy / abs(dy));
							object->inner[7] += abs(dx);
							if (object->inner[7] >= abs(dy)) {
							   object->inner[7] -= abs(dy);
							   newx += (dx / abs(dx));
							}   						
						}			
					}			
					newx += object->x;
					newy += object->y;
				   
					collided = 0;				   
				   if ((newx < 0) || (newx >= (600 - object->xs))) {
				      collided = 1;
				   } else if ((newy < 0) || (newy >= (240 - object->ys))) {   
				      collided = 1;
				   } else {
				   	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 object->xs,
											 object->ys,
											 newx,
											 newy,
											 &ds_global_map.tileMapCol);
				      
				   }  
					if (collided) {
					   // "Stop" State
			         object->inner[0] = 2;
					} else {
					   // Great! - Move! - Also, tremble (fly)
					   object->x = newx;
					   object->y = newy;					   
					   ds_3dsprite_setX(object->sprite,object->x + PA_RandMinMax(0,trembleSprite) - (trembleSprite >> 1));
					   ds_3dsprite_setY(object->sprite,object->y + PA_RandMinMax(0,trembleSprite) - (trembleSprite >> 1));					   
					}   
           		object->inner[8] = waitingMovement >> object->inner[4]; // Fast if *Chof*!
				} else {
				  object->inner[0] = 2; // "Stop" state 
				}   	      
      	}   
         break;
      case 2:
         // "STOP" STATE
         object->inner[0] = 0;
		   object->inner[8] = PA_RandMinMax(minWaitingTime,maxWaitingTime);
			break;         
	}	
}   

/* Behaviour: move left and right (INI) */
void ds_objects_lib_beh_moveLeftRight_ini(ds_t_object *object, int face) {
   object->inner[0] = 0;
   object->inner[2] = 1;
	object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[2], face);
}   

/* Behaviour: move left and right */
void ds_objects_lib_beh_moveLeftRight(ds_t_object *object, int minSpeed, int maxSpeed, int accIni, int accFin,
											int minDist, int maxDist, 
											int minWaitingTime, int maxWaitingTime, int waitingMovement, int waitingSpriteMovement,
											int followJuni, int distanceJuni, int face,
											int checkcollision) {
/*
	[0] - State
	[1] - Sprite
	[2] - Facing direction
	[3] - Speed
	[6] - Actual Pix movement
	[7] - Px to move. -1 = Follow Juni
	[8] - Waiting time
	[9] - Waiting spritetime
*/
	int letsmove = 0;
	int newx;
	int collided;
	int olddir;


   switch (object->inner[0]) {
      case 0:
         // "WHAT TO DO" STATE
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
            // Move... following Juni
            if (followJuni) {
               // Look at Juni
               olddir = object->inner[2];
               object->inner[2] = (object->x > ds_juni_getXVirtual())?-1:1; 
               if (olddir != object->inner[2])
	               object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], face);
	         	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//					ds_3dsprite_updateSprite(object->sprite);	               
	            // Should we move towards Juni?
               if (distanceJuni == -1) {
                  letsmove = 1; // Don't care about distance, just follow Juni
               } else if (abs(ds_juni_getXVirtual() - object->x) > distanceJuni) {
                  letsmove = 1; // Will follow Juni, but only until a certain point                  
					}      
				} else {
				   	object->inner[2] = (PA_RandMax(100) <= 50)?-1:1;
                  letsmove = 1; // Just move (c) :-)				   
				}
				// So... we move, or not????
				if (letsmove) {
				   // Speed
				   if (minSpeed == maxSpeed)
				   	object->inner[3] = minSpeed;
				   else
				   	object->inner[3] = PA_RandMinMax(minSpeed,maxSpeed);
				   // Distance
				   if (followJuni)
				   	object->inner[7] = -1;
				   else if (minDist == maxDist)
				   	object->inner[7] = minDist;
				   else
				   	object->inner[7] = PA_RandMinMax(minDist,maxDist);				   
				   object->inner[6] = 0;
				   // Sprite
				   object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], face);
				   // State
				   object->inner[0] = 1;
				   object->inner[8] = waitingMovement;
				   object->inner[9] = waitingSpriteMovement;
				} else {
				   object->inner[8] = PA_RandMinMax(minWaitingTime,maxWaitingTime);
				}      
         }   
         break;
      case 1:
         // "MOVE" STATE
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
            if (followJuni) {
               // Change direction
               olddir = object->inner[2];
               object->inner[2] = (object->x > ds_juni_getXVirtual())?-1:1; 
               if (olddir != object->inner[2])
	               object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], face);
               // Check movement
               if (distanceJuni == -1) {
                  letsmove = 1;
               } else if (abs(ds_juni_getXVirtual() - object->x) > distanceJuni) {
                  letsmove = 1; // Follow Juni!!!, but only until a certain point                  
					} else {
					   letsmove = 0; // stop moving!
					} 
					// Too near Juni?
					if (letsmove)  
						if (abs(ds_juni_getXVirtual() - object->x) < 4)
							letsmove = 0; // Do not follow if I am too near... ^_-
            } else if (object->inner[7] == -1) {
               letsmove = 1;
            } else {
               object->inner[6]++;
               if (object->inner[6] < object->inner[7]) {
                  // Move :-)
               	letsmove = 1;
             	} else
               	letsmove = 0;
				}
				if (letsmove) {
				   // MOVEMENT "PER SE"
				   // Change sprite
		         if (object->inner[9] > 0)
		         	object->inner[9]--;
		         if (object->inner[9] <= 0) {
		            object->inner[1]++;
		            if (object->inner[1] >= ds_3dsprite_getMaxFrame_LR(object->sprite, object->inner[2], face))
		            	object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], face);
		         	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//						ds_3dsprite_updateSprite(object->sprite);		   				   		            
		            object->inner[9] = waitingSpriteMovement;
					}   
					// Movement!!				
					collided = 0;
				   newx = object->x + (object->inner[3] * object->inner[2]);
				   if ((newx < 0) || (newx >= (600 - object->xs))) {
				      collided = 1;
				   } else if (checkcollision) {
				   	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 object->xs,
											 object->ys,
											 newx,
											 ds_3dsprite_getY(object->sprite),
											 &ds_global_map.tileMapCol);
				      
				   }  
					if (collided) {
					   // Change direction
					   object->inner[2] = (object->inner[2] == 1)?-1:1;
					   object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], face);
					   object->inner[9] = 0;
					} else {
					   // Great! - Move!
					   object->x = newx;
					   ds_3dsprite_setX(object->sprite,object->x);
					}   
				   // Finishing details - Acceleration, too
            	if (((object->inner[6] * 100) / object->inner[7]) < accIni)
            		object->inner[8] = waitingMovement >> 1;
            	else if (((object->inner[6] * 100) / object->inner[7]) > accFin)
            		object->inner[8] = waitingMovement << 1;
            	else
            		object->inner[8] = waitingMovement;
				} else {
				  object->inner[0] = 2; // "Stop" state 
				}   	      
      	}   
         break;
      case 2:
         // "STOP" STATE
         object->inner[0] = 0;
         object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], face);
		   object->inner[8] = PA_RandMinMax(minWaitingTime,maxWaitingTime);
        	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//			ds_3dsprite_updateSprite(object->sprite);		   
			break;         
	}	
}   

/* Behaviour: float left and right (INI) */
void ds_objects_lib_beh_floatLeftRight_ini(ds_t_object *object, int face, int dir) {
   object->inner[0] = 0;
   object->inner[2] = dir;
   object->inner[4] = face;
	object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[2], face);
}   


/* Behaviour: float left and right */
void ds_objects_lib_beh_floatLeftRight(ds_t_object *object, int minSpeed, int maxSpeed, int probSlower, int distSlower,
											int minDist, int maxDist, 
											int minWaitingTime, int maxWaitingTime, int waitingMovement, int waitingSpriteMovement,
											int collideWall, int waitAfterCollide) {
/*
	[0] - State
	[1] - Sprite
	[2] - Facing direction
	[3] - Speed
	[4] - "Face"
	[5] - State "moving slower"
	[6] - Actual Pix movement
	[7] - Px to move. -1 = Follow Juni
	[8] - Waiting time
	[9] - Waiting spritetime
*/
	int letsmove = 0;
	int newx;
	int collided;


   switch (object->inner[0]) {
      case 0:
         // "WHAT TO DO" STATE
         if (object->inner[8] <= 0) {
            if (maxDist != -1)
			   	object->inner[2] = (PA_RandMax(100) <= 50)?-1:1; // Choose direction only when floating freely
			   // Speed
			   if (minSpeed == maxSpeed)
			   	object->inner[3] = minSpeed;
			   else
			   	object->inner[3] = PA_RandMinMax(minSpeed,maxSpeed);
			   // Distance
			   if (minDist == maxDist)
			   	object->inner[7] = minDist;
			   else
			   	object->inner[7] = PA_RandMinMax(minDist,maxDist);				   
			   object->inner[6] = 0;
			   // Sprite
			   object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], object->inner[4]);
			   // State
			   object->inner[0] = 1;
			   object->inner[5] = 0;
			   object->inner[8] = waitingMovement;
			   object->inner[9] = waitingSpriteMovement;
         } else {
            object->inner[8]--;
         }   
         break;
      case 1:
         // "MOVE" STATE
         if (object->inner[8] <= 0) {
            object->inner[8] = waitingMovement;
				if (object->inner[7] == -1) {
               letsmove = 1;
            } else {
               object->inner[6]++;
               if (object->inner[6] < object->inner[7]) {
                  // Move :-)
               	letsmove = 1;
             	} else
               	letsmove = 0;
				}
				if (letsmove) {
				   // MOVEMENT "PER SE"
				   // Change sprite
		         if (object->inner[9] > 0)
		         	object->inner[9]--;
		         if (object->inner[9] <= 0) {
		            object->inner[1]++;
		            if (object->inner[1] >= ds_3dsprite_getMaxFrame_LR(object->sprite, object->inner[2], object->inner[4]))
		            	object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], object->inner[4]);
		         	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//						ds_3dsprite_updateSprite(object->sprite);		   				   		            
		            object->inner[9] = waitingSpriteMovement;
					}   
					// Movement!!				
					collided = 0;
					int velx = ( ((object->inner[5] > 0)?1:object->inner[3]) * object->inner[2]);
				   newx = object->x + velx;
				   if ((newx < 0) || (newx >= (600 - object->xs))) {
				      collided = 1;
				   } else if (collideWall) {
				   	/* OPT - Create a standard function */
						collided = ds_map_collMovBasic(newx,ds_3dsprite_getY(object->sprite),(object->inner[2] == 1)?object->xs:0,velx);
				   }   
					if (collided) {
					   // Fix collision position
					   int i;
					   int imax = abs(newx - object->x);
					   int corrB;
					   for (i = 0; i < imax; i++) {
					      corrB = (object->inner[2] == 1)?(24 - i):i;
	         			if (!ds_map_collMovBasic(newx,ds_3dsprite_getY(object->sprite),corrB,0)) {
	         			   int corr = (object->inner[2] == 1)?-1:1;
							   object->x = newx + (i * corr);
							   ds_3dsprite_setX(object->sprite,object->x);
	            			break;
	         			}   
	      			} 

					   // Change direction
					   object->inner[2] = (object->inner[2] == 1)?-1:1;
					   object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], object->inner[4]);
					   object->inner[9] = 0;
					   // And if we have to wait... WAIT!!!!!
					   if (waitAfterCollide) {
					      object->inner[0] = 2; // "Stop" state 
					   }   
					} else {
					   // Great! - Move!
					   object->x = newx;
					   ds_3dsprite_setX(object->sprite,object->x);
					}   
				   // Finishing details - Go slower if indicated
				   if (object->inner[5] <= 0) {
				      if (PA_RandMax(1000) < probSlower) {
				         object->inner[5] = PA_RandMax(distSlower >> 1) + (distSlower >> 1);
				      }   
					} else {
					   object->inner[5]--;
					}   
				} else {
				  object->inner[0] = 2; // "Stop" state 
				}   	      
      	} else {
      	   object->inner[8]--;
      	}   
         break;
      case 2:
         // "STOP" STATE
         object->inner[0] = 0;
         object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], object->inner[4]);
		   object->inner[8] = PA_RandMinMax(minWaitingTime,maxWaitingTime);
        	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//			ds_3dsprite_updateSprite(object->sprite);		   
			break;         
	}	
}

/* Behaviour: jump "intelligently" - INI */
void ds_objects_lib_beh_jumpJuniJump_ini(ds_t_object *object) {
   object->inner[0] = 0;		   
	object->inner[1] = 0;		            
}   

/* Behaviour: jump "intelligently" */
void ds_objects_lib_beh_jumpJuniJump(ds_t_object *object, int distJuni, int speedJump,
											int distJump, 
											int waitingMovementSurface, int waitingMovementJump) {
/*
	[0] - State
	[1] - Sprite
	[3] - SpeedY
	[6] - Actual Pix movement
	[7] - Px to move. -1 = Follow Juni
	[8] - Waiting time
	[9] - waiting time - sprite
*/

   switch (object->inner[0]) {
      case 0:
         // INIT
		   object->inner[0] = 1;		   
			object->inner[1] = 0;		            
			ds_3dsprite_setFrame(object->sprite,object->inner[1]);
			break;
      case 1:
         // "I AM JUMPING"
         if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_TJUMP)) {
            if (ds_objects_lib_distanceJuniX(object,1,1) <= distJuni)
	            object->inner[0] = 2;
	            object->inner[8] = waitingMovementSurface;
			}			
			break;
      case 2:			
         // "GET OUT OF THE GROUND" STATE
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
	        	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//				ds_3dsprite_updateSprite(object->sprite);	                        
            object->inner[1]++;
				if (object->inner[1] == ds_3dsprite_getMaxFrame(object->sprite)) {
					// Change and sound
				   object->inner[0] = 3;
				   object->inner[3] = speedJump;
				   object->inner[7] = distJump / speedJump;
				   object->inner[8] = waitingMovementJump;
				   ds_music_playSound("Bounce Lite", 0, 0);
				} else {
				   object->inner[8] = waitingMovementSurface;
				}			   
			}			
			break;
		case 3:
		   // "JUMP!!!" STATE (UP)
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
			   object->y = object->y - object->inner[3];
			   ds_3dsprite_setY(object->sprite,object->y);            
			   object->inner[7]--;
			   if (object->inner[7] <= 0) {
				   object->inner[0] = 4;
				   object->inner[7] = distJump / speedJump;
				   object->inner[8] = waitingMovementJump;
			   } else {   
					object->inner[8] = waitingMovementJump;			   
				}			
			}					   
			break;
		case 4:
		   // "JUMP!!!" STATE (DOWN)
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
			   object->y = object->y + object->inner[3];
			   ds_3dsprite_setY(object->sprite,object->y);            
			   object->inner[7]--;
			   if (object->inner[7] <= 0) {
				   // Cange state and sound
				   object->inner[0] = 0;
				   object->inner[7] = distJump;
				   object->inner[8] = waitingMovementJump;
				   ds_music_playSound("Bounce", 0, 0);
			   } else {   
					object->inner[8] = waitingMovementJump;			   
				}			
			}					   
			break;			
	}			
}	

/* Behaviour: jump in a sequence */
void ds_objects_lib_beh_jumpSequence(ds_t_object *object, 
											int sY1, int sY2, int sY3, int sY4, int sY5,
											int maxSpeedY,
											int gravity,
											int sequenceRandom, 
											int waitingTime) {
/*
	[0] - State
	[1] - Sprite
	[2] - Jump sequence
	[3] - SpeedY
	[4] - MaxSpeedY	
	[5] - Gravity
	[8] - Waiting time
*/

	int i;
	int collided;

   switch (object->inner[0]) {      
      case 0:
         // "PREPARE TO JUMP" STATE
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
            ds_3dsprite_setFrame(object->sprite,object->inner[1]);
            object->inner[1]++;
            if (object->inner[1] == ds_3dsprite_getMaxFrame(object->sprite)) {
               // Normal values
               object->inner[4] = maxSpeedY;
               object->inner[5] = gravity;
               // Jumping sequence!
               switch (object->inner[2]) {
                  case 0:
                     object->inner[3] = sY1;
                     break;
                  case 1:
                     object->inner[3] = sY2;
                     break;
                  case 2:
                     object->inner[3] = sY3;
                     break;
                  case 3:
                     object->inner[3] = sY4;
                     break;
                  case 4:
                     object->inner[3] = sY5;
                     break;                     
               }   
               // No sequence?
               if (object->inner[3] == 0) {
					   // Back to 0!
					   object->inner[0] = 0;
					   object->inner[1] = ds_3dsprite_getMaxFrame(object->sprite) - 1;
					   object->inner[2] = 0;					   
					   object->inner[8] = 0;                  
               } else {
						// Next sequence
						if (sequenceRandom) {
						   object->inner[2] = PA_RandMinMax(0,4);
						} else {
						   object->inner[2] = (object->inner[2] + 1) % 5;
						}   
						// Advance and make sound!
						object->inner[0] = 1;   
						object->inner[8] = waitingTime;
						ds_music_playSound("Bounce", 0, 0);
					}			
            } else {
               object->inner[8] = waitingTime;
            }   
			}			
			break;
      case 1:			
         // "JUMP" STATE
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
		      // Gravity affects...
		      object->inner[3] += object->inner[5];
		      object->inner[3] = (object->inner[4] <= object->inner[3])?object->inner[4]:object->inner[3];
		      
			   // Move entity
			   object->y += object->inner[3] >> 10; // 1024, simulating floats            
			   
			   // Move the sprite itself
				ds_3dsprite_setY(object->sprite,object->y);			   
				
			   // Check "hitting the ground"
		   	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 ds_3dsprite_getX(object->sprite),
											 ds_3dsprite_getY(object->sprite),
											 &ds_global_map.tileMapCol);
											 
				// If we hit ***the ground***, everything is over :-) - put object in shape
				if (collided) {
				   if (object->inner[3] > 0) {
				      // If we hit **the ground**, we must correct the position. Knytt stories has a bug when hitting the ceiling... but we will maintain it
				      for (i = 0; i < (object->inner[3] >> 10); i++) {
				         object->y -= 1;
				         ds_3dsprite_setY(object->sprite,object->y);
				         collided = ds_map_collDownBasic(ds_3dsprite_getX(object->sprite),ds_3dsprite_getY(object->sprite),24,0);
				         if (collided)
				         	break; // breaks the FOR
				      }   
				   }   
				   // Back to the state...
				   object->inner[0] = 0;
				   object->inner[1] = 0;
				   object->inner[3] = 0;
				   object->inner[8] = waitingTime;
				}   
				
				// Next...
				object->inner[8] = waitingTime;
			}			
			break;		
	}			
}	

/* Behaviour: follow a predefinded Path */
void ds_objects_lib_beh_followPath(ds_t_object *object, int speed, int sizeArray,
											int *arrMov, int *arrDir,
											int waitingMovement, int waitingSpriteMovement) {
/*
	[0] - State
	[1] - Sprite
	[2] - "State" (arrays)
	[6] - Actual Pix movement
	[7] - Actual Dir movement
	[8] - Waiting time
	[9] - Waiting sprite time	
*/

	int newx = 0;
	int newy = 0;	
   switch (object->inner[0]) {
      case 0:
         // INIT
		   object->inner[0] = 1;
		   object->inner[1] = 0;
		   object->inner[2] = 0;
		   object->inner[6] = (*(arrMov + object->inner[2]) * 24) / speed;
		   object->inner[7] = *(arrDir + object->inner[2]);
         break;
      case 1:
         // "FOLLOW THE ARRAY" STATE
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
            // Move
            switch (object->inner[7]) {
            	case 0:
						newy = -1;   
						break;
            	case 1:
					   newx = 1;
						newy = -1;   
						break;
            	case 2:
					   newx = 1;   
						break;
            	case 3:
					   newx = 1;
						newy = 1;   
						break;
            	case 4:
						newy = 1;   
						break;
            	case 5:
					   newx = -1;
						newy = 1;   
						break;
            	case 6:
					   newx = -1;
						break;
            	case 7:
					   newx = -1;
						newy = -1;   
						break;
            }   
			   object->x += (newx * speed);
			   object->y += (newy * speed);
			   ds_3dsprite_setX(object->sprite,object->x);            
			   ds_3dsprite_setY(object->sprite,object->y);            							   
			   object->inner[8] = waitingMovement;
			   // Change sprite
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;
	            if (object->inner[1] >= ds_3dsprite_getMaxFrame(object->sprite))
	            	object->inner[1] = 0;
	         	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//					ds_3dsprite_updateSprite(object->sprite);		   				   		            
	            object->inner[9] = waitingSpriteMovement;
				}   
				//  Array position			   
            object->inner[6]--;
            if (object->inner[6] == 0) {
	            object->inner[2] = (object->inner[2] + 1) % sizeArray;
   	         object->inner[6] = (*(arrMov + object->inner[2]) * 24) / speed;
      	      object->inner[7] = *(arrDir + object->inner[2]);
      	   }   
			}			
	}			
}

/* Behaviour: follow a predefinded Path, changing Left-Right */
void ds_objects_lib_beh_followPathLR(ds_t_object *object, int speed, int sizeArray,
											int *arrMov, int *arrDir,
											int waitingMovement, int waitingSpriteMovement,
											int softMov) {
/*
	[0] - State
	[1] - Sprite
	[2] - "State" (arrays)
	[4] - Dir
	[5] - Special Pix movement
	[6] - Actual Pix movement
	[7] - Actual Dir movement
	[8] - Waiting time
	[9] - Waiting sprite time	
*/

	int newx = 0;
	int newy = 0;
   switch (object->inner[0]) {
      case 0:
         // INIT
		   object->inner[0] = 1;
		   object->inner[1] = 0;
		   object->inner[2] = 0;
		   object->inner[4] = 0;
		   object->inner[5] = 0;
		   object->inner[6] = (*(arrMov + object->inner[2]) * 24) / speed;
		   object->inner[7] = *(arrDir + object->inner[2]);
		   object->inner[8] = 0;
		   object->inner[9] = 0;
         break;
      case 1:
         // "FOLLOW THE ARRAY" STATE
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
            // Move... Move? (Include special manipulations for slope)
            newx = 0;
            newy = 0;
            switch (object->inner[7]) { 
            	case 0:
						newy = -1;   
						break;
            	case 1:
					   newx = 1;
						newy = -1;   
						break;
            	case 2:
					   newx = 1;   
						break;
            	case 3:
					   newx = 1;
						newy = 1;   
						break;
            	case 4:
						newy = 1;   
						break;
            	case 5:
					   newx = -1;
						newy = 1;   
						break;
            	case 6:
					   newx = -1;
						break;
            	case 7:
            	   newx = -1;
						newy = -1;   
						break;
            	case 11: // Soft 1
            		newx = 1;
            	   if (!object->inner[5]) {   
							newy = -1;
						} 
						object->inner[5] = (object->inner[5] + 1) % softMov; 			   
						break;
            	case 13: // Soft 3
            		newx = 1;
            	   if (!object->inner[5]) {  
							newy = 1;
						} 
						object->inner[5] = (object->inner[5] + 1) % softMov; 			   
						break;
            	case 15: // Soft 5
            		newx = -1;
            	   if (!object->inner[5]) {   
							newy = 1;
						} 
						object->inner[5] = (object->inner[5] + 1) % softMov; 			   
						break;
            	case 17: // Soft 7
            		newx = -1;
            	   if (!object->inner[5]) {   
							newy = -1;
						} 
						object->inner[5] = (object->inner[5] + 1) % softMov; 			   
						break;
            }  
			   object->x += (newx * speed);
			   object->y += (newy * speed);
			   ds_3dsprite_setX(object->sprite,object->x);            
			   ds_3dsprite_setY(object->sprite,object->y);            							   
			   object->inner[8] = waitingMovement;
			   // Dir Control
			   if (((newx==1)?0:1) != object->inner[4]) {
			      object->inner[4] = ((newx==1)?0:1);
	            object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[4]==0?1:-1,1);
	            object->inner[9] = 0; // Change!
	         }   
			   // Change sprite
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;	            
			      if (object->inner[1] >= ds_3dsprite_getMaxFrame_LR(object->sprite,object->inner[4]==0?1:-1,1))
			      	object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[4]==0?1:-1,1);
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//					ds_3dsprite_updateSprite(object->sprite);		   				   		            
	            object->inner[9] = waitingSpriteMovement;
				}   
				//  Array position			   
            object->inner[6]--;
            if (object->inner[6] == 0) {
	            object->inner[2] = (object->inner[2] + 1) % sizeArray;
   	         object->inner[6] = (*(arrMov + object->inner[2]) * 24) / speed;
      	      object->inner[7] = *(arrDir + object->inner[2]);
      	   }   
			}			
	}			
}				

/* Behaviour: follow a predefinded PIXEL path, changing Left-Right */
void ds_objects_lib_beh_followPixelPathLR(ds_t_object *object, int sizeArray, 
											int* arrSpeed, int *arrMovX, int *arrMovY,
											int waitingSpriteMovement,
											int canStop) {
/*
	[0] - State
	[1] - Sprite
	[2] - "State" (arrays)
	[3] - Accumulated Slope X
	[4] - Accumulated Slope Y
	[5] - Accumulated Speed
	[6] - Counter
	[7] - Update Param
	[8] - Old DirX
	[9] - Waiting sprite time	
	[10] - Actual DirX for sprites, can be changed from outside
*/

	int movPixX, movPixY, movPixMax;
	int mX,mY;
	int dirX, dirY;
	int slope;
	int actArr, nextArr;
	int speed;
   switch (object->inner[0]) {
      case 0:
         // INIT
		   object->inner[0] = 1;
		   object->inner[1] = (canStop == 1);
		   object->inner[2] = 0; // First array position
			object->inner[3] = 0; // Accumulated Slope X
			object->inner[4] = 0; // Accumulated Slope Y
			object->inner[5] = 0; // Accumulated Speed
			object->inner[6] = 0; // Counter
         object->inner[7] = 1; // Update Param, NOW!	
		   object->inner[9] = 0; // Update Sprite NOW!
         break;
		case 1:
         // 1A) INIT THE ARRAY IF NO PARAMETERS ARE GIVEN
         actArr = object->inner[2];
         nextArr = (object->inner[2] + 1) % sizeArray;
			mX = (*(arrMovX + nextArr)) - (*(arrMovX + actArr));         
			mY = (*(arrMovY + nextArr)) - (*(arrMovY + actArr));         
         if (object->inner[7]) {
				object->inner[3] = 0; // Accumulated Slope X
				object->inner[4] = 0; // Accumulated Slope Y
				object->inner[5] = 0; // Accumulated Speed
				object->inner[6] = 0; // Counter
	         object->inner[7] = 0; // Update Param	
	         object->inner[10] = (mX >= 0)?1:-1;
      	}   
			dirX = (mX >= 0)?1:-1; 
			dirY = (mY >= 0)?1:-1;
			mX = abs(mX);
			mY = abs(mY);
			slope = (mY > mX)?mY:mX;      	
      	// 1B) MOVE SPRITE
			if (slope != 0) {
			   // There is movement
			   speed = (*(arrSpeed + actArr)) * 100; 
			   object->inner[5] += (speed / 100);
			   movPixMax = object->inner[5] / 10; // REAL speed
			   object->inner[5] -= (object->inner[5] / 10) * 10; // Works due to integer calculations...
				movPixX = 0; 
				movPixY = 0; 
				
  	      	int i;
	      	for (i = 0; i < movPixMax; i++) {
	      	   // Pixel movements
				   object->inner[3] += mX;
				   movPixX += dirX * (object->inner[3] / slope); // REAL movement
				   object->inner[3] -= (object->inner[3] / slope) * slope; // Works due to integer calculations...
				   object->inner[4] += mY;
				   movPixY += dirY * (object->inner[4] / slope); // REAL movement
				   object->inner[4] -= (object->inner[4] / slope) * slope; // Works due to integer calculations...      	   
				   // Check if we arrived to our destination
				   object->inner[6]++;
				   if (object->inner[6] >= slope) {
				      object->inner[7] = 1;
				      break; // Exits this loop
				   }   
	      	}            
			   object->x += movPixX;
			   object->y += movPixY;
			   ds_3dsprite_setX(object->sprite,object->x);            
			   ds_3dsprite_setY(object->sprite,object->y);
			} else {
			   // No movement! 
				speed = (*(arrSpeed + actArr)); 
				// First, change sprites
				if (object->inner[6] == 0) {
				   object->inner[10] = object->inner[8];
		      	object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[10],1);
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);				   
				} else if (object->inner[6] == (speed >> 1)) {
				   object->inner[10] = (object->inner[8] == 1)?-1:1;
		      	object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[10],1);
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
				}
				// Now, Just let time pass...
			   object->inner[6]++;
			   if (object->inner[6] >= speed) {
			      object->inner[7] = 1;
			   } else {
			      return; // No other things are necessary ;-)
			   }   
			}   		            							   
		   
			// 1D) UPDATE ARRAY
			if (object->inner[7]) {
			   // Next pos
			   object->inner[2] = (object->inner[2] + 1) % sizeArray;
				
				// Sprite!			   
	         int actArrNEW = object->inner[2];
	         int nextArrNEW = (object->inner[2] + 1) % sizeArray;
				int mXNEW = (*(arrMovX + nextArrNEW)) - (*(arrMovX + actArrNEW));
				if (mXNEW != 0) {
					int dirXNEW = (mXNEW >= 0)?1:-1;
				   if (dirXNEW != dirX) {
		            object->inner[1] = 
							ds_3dsprite_getIniFrame_LR(object->sprite,dirXNEW,1) + (canStop == 1);
		            object->inner[9] = 0; // Change!
		            dirX = dirXNEW; // In order to update the sprite in the next step!~ ;-) 
		         }
		         object->inner[8] = dirX;
		         object->inner[10] = dirX;
		      }   
			}

			// 1C) UPDATE SPRITE
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;	            
		      if (object->inner[1] >= ds_3dsprite_getMaxFrame_LR(object->sprite,object->inner[10],1))
		      	object->inner[1] = 
						ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[10],1) + (canStop == 1);
		   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
            object->inner[9] = waitingSpriteMovement;
			}   
	}			
}

/* Behaviour: do N gestures */
void ds_objects_lib_beh_knyttNgestures(ds_t_object *object, int sizeArray, int *arrG, int waitingTime, int waitingSpriteTime, int LR) {
/*
	[0] - State
	[1] - Sprite
	[2] - Gesture
	[7] - Max Gesture
	[8] - Waiting Time
	[9] - Waiting Sprite Time
*/
	switch (object->inner[0]) {
	   case 0:
	      /* INITIALIZATION */
	      object->inner[0] = 1; // New state
	      
	      object->inner[2] = -1; // Gesture (None)
	      if (!LR) {
	         object->inner[1] = 0; // Actual sprite
	   	} else {
	   	   object->inner[1] = PA_RandMinMax(0,1); // Actual sprite
			}      
	      ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	      object->inner[8] = waitingTime; // Waiting time
	      break;
	   case 1:
	      /* DO NOT MOVE */
	      object->inner[8]--;
	      if (object->inner[8] <= 0) {
	         // Change state
	         object->inner[0] = 2;
	         object->inner[2] = PA_RandMinMax(0,sizeArray - 2);
	         object->inner[1] = *(arrG + object->inner[2]);
	         object->inner[7] = *(arrG + object->inner[2] + 1);
		      object->inner[9] = waitingSpriteTime; // Waiting Sprite Time	         
	      }   
	      break;
	   case 2:
	      /* MOVEMENT N */
	      object->inner[9]--;
	      if (object->inner[9] <= 0) {
	         if (object->inner[1] < object->inner[7]) {	         
		         ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		         object->inner[1]++;
		         object->inner[9] = waitingSpriteTime;		         
				} else {
				   object->inner[0] = 0; // Initialization state
				}   
	      }   
	      break;	      
	}   
}   

/* Behaviour: do three gestures */
/*void ds_objects_lib_beh_knytt3gestures(ds_t_object *object, int sprite1, int sprite2, int sprite3, int sprite4, int waitingTime, int waitingSpriteTime) {

//	[0] - State
//	[1] - Sprite
//	[8] - Waiting Time
//	[9] - Waiting Sprite Time

	switch (object->inner[0]) {
	   case 0:
	      // INITIALIZATION
	      object->inner[0] = 1; // New state
	      object->inner[1] = 0; // Actual sprite
	      ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	      object->inner[8] = waitingTime; // Waiting time
	      break;
	   case 1:
	      // DO NOT MOVE 
	      object->inner[8]--;
	      if (object->inner[8] <= 0) {
	         // Change state
	         object->inner[0] = PA_RandMinMax(2,4);
	         if (object->inner[0] == 2)
	         	object->inner[1] = sprite1;
	         else if (object->inner[0] == 3)
	         	object->inner[1] = sprite2;	         
	         else if (object->inner[0] == 4)
	         	object->inner[1] = sprite3;	         
		      object->inner[9] = waitingSpriteTime; // Waiting Sprite Time	         
	      }   
	      break;
	   case 2:
	      // MOVEMENT 1 
	      object->inner[9]--;
	      if (object->inner[9] <= 0) {
	         if (object->inner[1] < sprite2) {	         
		         ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		         object->inner[1]++;
		         object->inner[9] = waitingSpriteTime;		         
				} else {
				   object->inner[0] = 0; // Initialization state
				}   
	      }   
	      break;	      
	   case 3:
	      // MOVEMENT 2 
	      object->inner[9]--;
	      if (object->inner[9] <= 0) {
	         if (object->inner[1] < sprite3) {	         
		         ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		         object->inner[1]++;
		         object->inner[9] = waitingSpriteTime;		         
				} else {
				   object->inner[0] = 0; // Initialization state
				}   
	      }   
	      break;	      	      
	   case 4:
	      // MOVEMENT 3
	      object->inner[9]--;
	      if (object->inner[9] <= 0) {
	         if (object->inner[1] < sprite4) {	         
		         ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		         object->inner[1]++;
		         object->inner[9] = waitingSpriteTime;		         
				} else {
				   object->inner[0] = 0; // Initialization state
				}   
	      }   
	      break;	      	      	      
	}   
}*/   

/* Behaviour: Flower behaviour. Returns 1 if we need to shoot a particle */
int ds_objects_lib_beh_flower(ds_t_object *object, int startCond, int startVal, int endCond, int endVal, 
											int waitingSeedTime, int waitingTime) {
/*
	[0] - State
	[1] - Sprite
	[7] - Max Seed
	[8] - Waiting Seed Time
	[9] - Waiting Sprite Time / Countdown
*/

	int res = 0;

	switch (object->inner[0]) {
	   case 0:
	      // INIT TRIGGER / OTHERS
	      switch (startCond) {
	         case 0: // Closer...
	         	break;
	         case 1: // Every X cycles (Countdown)...
         		object->inner[9] = startVal;
	         	break;
	      }   
	      object->inner[0] = 1;
	      break;
	   case 1:
	      // TRIGGER ATTACK
	      switch (startCond) {
	         case 0: // Closer...
	         	if (ds_objects_lib_isNearJuni(object, startVal, 1))
	         		object->inner[0] = 2;
	         	break;
	         case 1: // Every X cycles (Countdown)...
         		if (object->inner[9] > 0)
         			object->inner[9]--;
         		if (object->inner[9] <= 0)
         			object->inner[0] = 2;
	         	break;
	      }   
         if (object->inner[0] == 2) {
            object->inner[9] = waitingTime;
         }   
	      break;
	   case 2:
	      // OPEN FLOWER
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            ds_3dsprite_setFrame(object->sprite,object->inner[1]);
            object->inner[1]++;
            object->inner[9] = waitingTime;
            if (object->inner[1] == ds_3dsprite_getMaxFrame(object->sprite)) {
               object->inner[0] = 3;
               object->inner[8] = waitingSeedTime;
               object->inner[7] = endVal;
				}
			}						
	      break;
	   case 3:
	      // PERFORM ATTACK?
         if (object->inner[8] > 0)
         	object->inner[8]--;
         if (object->inner[8] <= 0) {
            object->inner[8] = waitingSeedTime;
            if (endCond == 2)
            	object->inner[7]--;
            res = 1;
			}
							      
	      // CHECK STOP CONDITIONS
	      switch (endCond) {
	         case 0: // Away...
	         	if (!ds_objects_lib_isNearJuni(object, endVal, 1))
	         		object->inner[0] = 4;
	         	break;
	         case 1: // Countdown...
         		if (object->inner[7] > 0)
         			object->inner[7]--;
         		if (object->inner[7] <= 0)
         			object->inner[0] = 4;
	         	break;
	         case 2: // No more seeds...
	         	if (object->inner[7] <= 0)
	         		object->inner[0] = 4;
	         	break;
	      }   
         if (object->inner[0] == 4) {
            object->inner[9] = waitingTime;
         }   
	      break;
	   case 4:
	      // CLOSE FLOWER
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]--;
            ds_3dsprite_setFrame(object->sprite,object->inner[1]);
            object->inner[9] = waitingTime;
            if (object->inner[1] == 0) {
               object->inner[0] = 0;
				}
			}						
	      break;
	}	
	return res;
}

/* Behaviour: Spike Behavour. Float towards an area */
int ds_objects_lib_beh_spikeFloat(ds_t_object *object, int minMov, int maxMov) {
/*
	[0] - State
	[7] - Partial Speed
	[8] - Direction
	[9] - Speed
*/

	int collided = 0;
	int outofmap = 0;
	int tmpx, tmpy;
	int movPix;

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[8] = 0;
	      object->inner[9] = PA_RandMinMax(minMov,maxMov);
	      object->inner[0] = 1;
	      break;
	   case 1:
	      // MOVE :-)
	      tmpx = object->x;
	      tmpy = object->y;
	      
   		object->inner[7] += object->inner[9];
			movPix = object->inner[7] / 10; // REAL speed
			object->inner[7] -= (object->inner[7] / 10) * 10; // Works due to integer calculations...
	      
	      if (movPix > 0) {
		      switch (object->inner[8]) {
		         case 0:
		            tmpy -= movPix;
		            break;
		         case 1:
		            tmpx += movPix;
		            break;
		         case 2:
		            tmpy += movPix;
		            break;
		         case 3:
		            tmpx -= movPix;
		            break;
		      }   
		      // Check Collision!
		   	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 tmpx,
											 tmpy,
											 &ds_global_map.tileMapCol);
				if (!collided) {
				   outofmap = ds_map_outOfMap_center(ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 tmpx,
											 tmpy);
				}   
				if (outofmap) {
				   // It ends for me...
				   object->_deleteme = 1;
				   return 1;
				} else if (collided) {
				   // Changes direction AND speed!
				   object->inner[8] = PA_RandMinMax(0,3);
				} else {
				   // Move :-)
					object->x = tmpx;
					object->y = tmpy;
					ds_3dsprite_setX(object->sprite,object->x);
					ds_3dsprite_setY(object->sprite,object->y);			   
				}     		      	      
			}			
	      break;
	}
	return 0;
}		

/* Behaviour: Trap Behaviour. Wait till Shoot. Return 1 if shoot. */
int ds_objects_lib_beh_trapShoot(ds_t_object *object, int near, int cont, int waitingSprite) {
/*
	[0] - State
	[1] - Sprite
	[2] - >> or << (sprite)
	[8] - Waiting time - Counter
	[9] - Waiting time - Sprite
*/

	int res = 0;

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[1] = 0;
	      object->inner[8] = 0;
	      object->inner[9] = waitingSprite;
	      object->inner[0] = 1;
	      break;
	   case 1:
	      // WAIT JUNI... (and decrease count)
	      if (object->inner[8] > 0) {
	         object->inner[8]--;
	      } else {   
		      if (ds_objects_lib_distanceJuniX(object, 1, 1) < near) {
		         object->inner[9] = waitingSprite;
		         object->inner[8] = 0;
		         object->inner[2] = 1;
		         object->inner[0] = 2;
		      }   
	      }
	      break;
	   case 2:
	      // SHOOT!
	      // But wait!... decrease counter first :-)
         object->inner[8]--;
         if (object->inner[8] <= 0) {
            // What I was doing... oh, yeah... SHOOT!     
			   if (object->inner[9] > 0)
			   	object->inner[9]--;
			   if (object->inner[9] <= 0) {
			      object->inner[9] = waitingSprite;
			      
			      // Move Sprite (Cycle)
			      if (object->inner[2] == 1) {
					   object->inner[1] += object->inner[2];
					   if (object->inner[1] == (ds_3dsprite_getMaxFrame(object->sprite) - 1)) {
					      object->inner[2] = -1;
					      res = 1;
					   }  
			      } else {
					   object->inner[1] += object->inner[2];
					   if (object->inner[1] == 0) {
					      object->inner[2] = 1;
					      object->inner[0] = 3;
					      object->inner[8] = cont;
					   }   
					}  
					ds_3dsprite_setFrame(object->sprite,object->inner[1]);    
				}			
			}			
	      break;
	   case 3:
	      // DECREASE COUNTER, CHECK POSITION
	      if (ds_objects_lib_distanceJuniX(object, 1, 1) < near) {
	         object->inner[8]--;
	         if (object->inner[8] <= 0) {
	            object->inner[9] = waitingSprite;
	            object->inner[8] = 0;
	            object->inner[2] = 1;
	            object->inner[0] = 2;
	         }   
	      } else {
	         object->inner[0] = 1;
			}   	      
	      break;
	}

	return res;
}   

/* Behaviour: Disk Behaviour. Moves. Return 1 if particle is launched. 
		Moves on X (useX) or Y (!useX)
		NOTE that is changed by the OneInstance method or external means. */
int ds_objects_lib_beh_disk(ds_t_object *object, int useX,
									 int changeSpeed, 
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int changeDirection,
									 int waitingSprite, 
									 int spritesM, int spritesG) {
/*
	[0] - State
	[1] - Sprite
	[2] - Dir
	[3] - Speed
	[4] - SpeedXY
	[9] - WaitSprite
*/

	int res = 0;
	int newP;
	int collided;
	int movPix;
	int posObj;
	int posTmp;

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[0] = 1;
	      object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
         object->inner[3] = changeSpeedS + 
						(PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
	      break;
	   case 1:
	      // MOVE
	      // Change Speed
	      if (changeSpeed != 0) {
		      if ((ds_global_tick % changeSpeed) == 0) {
		         object->inner[3] = changeSpeedS + 
								(PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
		      }
	   	}   
			// Change Direction
			if (changeDirection != -1) {
	      	if ((ds_global_tick % changeDirection) == 0) {
	         	object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
	      	}
	   	}   
			// Movement - How much we should move   
   		object->inner[4] += object->inner[3];
			movPix = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10; // Works due to integer calculations...
         // Movement - First, try to move            
         posObj = (useX)?object->x:object->y;
			newP = posObj + (movPix * object->inner[2]);
			if (object->inner[3] != 0) {		   
			   if (newP != posObj) {
			      posTmp = (useX)?(600 - object->xs):(240 - object->ys);
					collided = 0;				   
					if ((newP < 0) || (newP >= posTmp)) {
					   collided = 1;
					} else {
					 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
												 object->xs,
												 object->ys,
												 (useX)?newP:object->x,
												 (!useX)?newP:object->y,
												 &ds_global_map.tileMapCol);
					      
					}  
					if (!collided) {
					   // Move
					   if (useX) {
						   object->x = newP;
						   ds_3dsprite_setX(object->sprite,object->x);
						} else {
						   object->y = newP;
						   ds_3dsprite_setY(object->sprite,object->y);
						}      
		      	} else {
		      	   // Change Direction
		      	   object->inner[2] = (object->inner[2] == 1)?-1:1;
					}    
		    	}  	
				// Sprite Movement A
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;	            
			      if (object->inner[1] >= spritesM)
			      	object->inner[1] = 0;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[9] = waitingSprite;
				}   
	    	} else {
	    	   // Sprite Movement B
	    	   if (object->inner[1] != 0) {
			      object->inner[1] = 0;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	    	   }   
			}    	
			break;
		case 2:
		   // BEGIN - SHOOT PARTICLE
		   object->inner[0] = 3;
		   object->inner[1] = spritesM;
		   object->inner[9] = waitingSprite;
		   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		   break;
		case 3:
		   // SHOOT PARTICLE
		   // Advance and Shoot
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;	            
		      if (object->inner[1] >= (spritesM + spritesG)) {
		      	// SHOOT!
		      	res = 1;
		      	object->inner[0] = 1;
		      	object->inner[1] = 0;
		      	
		    	}  	
		   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
            object->inner[9] = waitingSprite;
			}   
			// ...Change Direction (maybe we found ourselves in this state)
			if (changeDirection != -1) {
	      	if ((ds_global_tick % changeDirection) == 0) {
	         	object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
	      	}
	   	}   
		   break;
	}
	return res;
}

void _inner_getRealVal(ds_t_object *object, int dir, int endR, int endL, int endRG, int endLG, int *realStart, int *realEnd) {
   // FIXME: Fusion B and LR methods
   if ((endRG == 0) && (endLG == 0)) {
   	*realEnd = ds_3dsprite_getMaxFrame_LR(object->sprite,(dir == 1),1);
   	*realStart = ds_3dsprite_getIniFrame_LR(object->sprite,(dir == 1),1);
   } else {
      if (endR == endL) {
			*realEnd = endR;
			*realStart = 0;
      } else {  
			*realEnd = ((dir == 1)?endR:endL);
			*realStart = ((dir == 1)?0:endR);
		}		
	}			
}

void _inner_getGestureVal(ds_t_object *object, int dir, int endR, int endL, int endRG, int endLG, int *realStart, int *realEnd) {
   // FIXME: Fusion B and LR methods! Put this method in practice!
   if ((endRG == 0) && (endLG == 0)) {
      // No real gestures...
   	*realEnd = ds_3dsprite_getMaxFrame_LR(object->sprite,(dir == 1),1);
   	*realStart = ds_3dsprite_getIniFrame_LR(object->sprite,(dir == 1),1);
   } else {
      if (endRG == endLG) {
			*realEnd = endRG;
			*realStart = endR;
      } else {  
			*realEnd = ((dir == 1)?endRG:endLG);
			*realStart = ((dir == 1)?endL:endRG);
		}		
	}			
}   
   

/* Behaviour: Disk-like Movement (L-R). Used by critters that need of disk-like movement.
	Moves on X (useX) or Y (!useX)
	Note: returns 1/2 (L/R) if followJuni && touches wall
	Note2: if cycleScreen, does not detect collision, and just moves. */
int ds_objects_lib_beh_diskMovementLR(ds_t_object *object, int useX, 
									 int changeSpeed, int changeSpeedStopped,
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int endR, int endL, int endRG, int endLG,
									 int changeDirection, int changeDirectionChance,
									 int cycleGesture,
									 int followJuni,
									 int waitingSprite,
									 int cycleScreen,
									 int launchAttack) {
/*
	[0] - State
	[1] - Sprite
	[2] - Dir
	[3] - Speed
	[4] - SpeedX/Y
	[9] - WaitSprite
*/

	int res = 0;
	int newP;
	int collided;
	int movPix;
   int realEnd;
   int realStart;
   int posObj;
   int posTmp;

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[0] = 1;
	      object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
	      _inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
	      object->inner[1] = realStart;
	      ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	      object->inner[3] = changeSpeedS + (PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
	      break;
	   case 1:
	      // MOVE
	      // Change Speed
	      if (changeSpeed != -1) {
	      	if ((ds_global_tick % ((object->inner[3]==0)?changeSpeedStopped:changeSpeed)) == 0) {
	         	object->inner[3] = changeSpeedS + 
								(PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
		      }
	   	}   
			// Change Direction
			if (followJuni) {
			   int posJuni = (useX)?ds_juni_getXVirtual():ds_juni_getYVirtual();
			   int posObj = (useX)?object->x:object->y;
			   int newdir = (posJuni < posObj)?-1:1;
			   if ((object->inner[2] != newdir) && (abs(posJuni-posObj) > 8)) {
	         		object->inner[2] = newdir;
		         	_inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
		      	   object->inner[1] = realStart;
		      	   object->inner[9] = 0;   			      
			   }   
			} else {  	   	
				if (changeDirection != -1) {
		      	if ( ((ds_global_tick % changeDirection) == 0) && 
					     (PA_RandMax(99) < changeDirectionChance) ) {
	         		object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
		         	_inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
		      	   object->inner[1] = realStart;
		      	   object->inner[9] = 0;   
		      	}
		   	}   
		 	}  	
			// Movement - How much we should move   
   		object->inner[4] += object->inner[3];
			movPix = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10; // Works due to integer calculations...
         // Movement - First, try to move 
			posObj = (useX)?object->x:object->y;           
			newP = posObj + (movPix * object->inner[2]);
			if (object->inner[3] != 0) {		   
			   // We must move!
			   if (newP != posObj) {
					collided = 0;
					posTmp = (useX)?(600 - object->xs):(240 - object->ys);				   
					if (cycleScreen) {
					   // Just test if we cycle the screen
					   if ((newP < 0) || (newP >= posTmp)) {
						   if (object->inner[2] == 1) {
								newP = 0;
							} else if (object->inner[2] == -1) {
								newP = posTmp - 1; 
							}			
						} 
					} else {
					   // Normal collision test   					
						if ((newP < 0) || (newP >= posTmp)) {
						   collided = 1;
						} else {
						 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
													 object->xs,
													 object->ys,
													 (useX)?newP:object->x,
													 (!useX)?newP:object->y,
													 &ds_global_map.tileMapCol);
						      
						}  
					}			
					if (!collided) {
					   // Move
					   if (useX) {
					   	object->x = newP;
					   	ds_3dsprite_setX(object->sprite,object->x);
					 	}  	
					   else {
					   	object->y = newP;
					   	ds_3dsprite_setY(object->sprite,object->y);
					 	}  	
		      	} else {
		      	   // Change Direction IF we don't follow Juni
		      	   if (!followJuni) {
			      	   object->inner[2] = (object->inner[2] == 1)?-1:1;
			      	   _inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
			      	   object->inner[1] = realStart;
			      	   object->inner[9] = 0;
		      		} else {
		      		   // We touched a wall... this is important for some critters :-P
		      		   res = (object->inner[2] == -1)?1:2;
						}      
					}    
		    	}  	
				// Sprite Movement
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;
	            _inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
			      if (object->inner[1] >= realEnd)
			      	object->inner[1] = realStart;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[9] = waitingSprite;
				}   
	    	} else {
	    	   // Change to gesture state, IF we have gestures
	    	   if ((endRG > 0) && (endLG > 0)) {
	    	      if (!launchAttack) {
	    	   		object->inner[0] = 2;
	    	 		} else {
	    	 		   object->inner[0] = 4;
					}   	
	    	   } else {
	    	      _inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
	    	      object->inner[1] = realStart;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	    	   }   
			}    	
			break;
		case 2:
		   // BEGIN - GESTURE
		   object->inner[0] = 3;
		   _inner_getGestureVal(object, object->inner[2], endR, endL, endRG, endLG, &realStart, &realEnd);
		   object->inner[1] = realStart;
		   object->inner[9] = waitingSprite;
		   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		   break;
		case 3:
		   // GESTURE
		   // Sprite
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;	            
            _inner_getGestureVal(object, object->inner[2], endR, endL, endRG, endLG, &realStart, &realEnd);
		      if (object->inner[1] >= realEnd) {
		      	if (cycleGesture) {
		      		object->inner[1] = realStart;
		      		ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		      	} else {
		      	   object->inner[1] = realEnd-1;
		      	}   
		    	} else {
		    	   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
				}    	
		   	
            object->inner[9] = waitingSprite;
			}   
			// Change state?
	      if ((ds_global_tick % changeSpeedStopped) == 0) {
	         object->inner[3] = changeSpeedS + 
							(PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
				if (object->inner[3] != 0) {
				   _inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
				   object->inner[1] = realStart;
				   object->inner[0] = 1;
				}   
	      }
		   break;
		case 4:
		   // BEGIN - GESTURE ATTACK
		   object->inner[0] = 5;
		   
			int posJuniGA = (useX)?ds_juni_getXVirtual():ds_juni_getYVirtual();
			int posObjGA = (useX)?object->x:object->y;
			object->inner[2] = (posJuniGA < posObjGA)?-1:1;		   
		   
		   _inner_getGestureVal(object, object->inner[2], endR, endL, endRG, endLG, &realStart, &realEnd);
		   object->inner[1] = realStart;
		   object->inner[9] = waitingSprite;
		   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
        	object->inner[3] = (changeSpeedR * changeSpeedM) * ((changeSpeedR * changeSpeedM) >> 1);
		   break;
		case 5:
		   // GESTURE ATTACK
		   // Movement
   		object->inner[4] += object->inner[3];
			movPix = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10; // Works due to integer calculations...         
			posObj = (useX)?object->x:object->y;           
			newP = posObj + (movPix * object->inner[2]);
			
			collided = 0;
		   if (newP != posObj) {
				posTmp = (useX)?(600 - object->xs):(240 - object->ys);				   
			   // Normal collision test   					
				if ((newP < 0) || (newP >= posTmp)) {
				   collided = 1;
				} else {
				 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 object->xs,
											 object->ys,
											 (useX)?newP:object->x,
											 (!useX)?newP:object->y,
											 &ds_global_map.tileMapCol);				      
				}  
				if (!collided) {
				   // Move
				   if (useX) {
				   	object->x = newP;
				   	ds_3dsprite_setX(object->sprite,object->x);
				 	}  	
				   else {
				   	object->y = newP;
				   	ds_3dsprite_setY(object->sprite,object->y);
				 	}  	
				}    
	    	}  	

		   // Sprite
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;	            
            _inner_getGestureVal(object, object->inner[2], endR, endL, endRG, endLG, &realStart, &realEnd);
		      if (object->inner[1] >= realEnd) {
		      	if (cycleGesture) {
		      		object->inner[1] = realStart;
		      		ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		      	} else {
		      	   object->inner[1] = realEnd-1;
		      	}   
		    	} else {
		    	   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
				}    	
            object->inner[9] = waitingSprite;
			}   
			// Change state?
			if (collided) {
			   // Correct position
			   int i;
			   int ix = ds_3dsprite_getX(object->sprite);
			   int iy = ds_3dsprite_getY(object->sprite);
			   int corrB;
			   if (useX) {
			   	ix = newP;
			   } else {
			   	iy = newP;
			 	}  	
			   for (i = 0; i < 24; i++) {
			      corrB = (object->inner[2] == 1)?(24 - i):i;
      			if (!ds_map_collMovBasic(ix,iy,corrB,0)) {
      			   int corr = (object->inner[2] == 1)?-1:1;
      			   if (useX) {
						   object->x += (i * corr);
						   ds_3dsprite_setX(object->sprite,object->x);
						} else {
						   object->y += (i * corr);
						   ds_3dsprite_setY(object->sprite,object->y);
						}   
         			break;
      			}   
   			} 
			   // New state
			   object->inner[2] = (object->inner[2] == 1)?-1:1;
			   _inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
			   object->inner[1] = realStart;
			   object->inner[3] = changeSpeedS + (PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
			   object->inner[0] = 1;
	      }
		   break;
	}
	return res;
}

/* Behaviour: Disk-like Movement (L-R) with spikes. 
	Used by critters that need of disk-like movement, but show spikes whenever Juni
	is near. */
int ds_objects_lib_beh_diskMovementLR_Spike(ds_t_object *object, 
									 int changeSpeed, int changeSpeedStopped,
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int endR, int endL, int endRS, int endLS,
									 int changeDirection, int changeDirectionChance,
									 int cycleGesture,
									 int distanceSpikes,
									 int followJuni,									 
									 int waitingSprite) {
/*
	[0] - State
	[1] - Sprite
	[2] - Dir
	[3] - Speed
	[4] - SpeedX
	[9] - WaitSprite
*/

	int res = 0;
	int newx;
	int collided;
	int movPixX;

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[0] = 1;
	      object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
	      if (object->inner[2] == 1)
	      	object->inner[1] = 0;
	      else
	      	object->inner[1] = endR;
	      object->inner[3] = changeSpeedS + (PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
	      break;
	   case 1:
	      // MOVE
	      // Change Speed
	      if (changeSpeed != -1) {
		      if ((ds_global_tick % ((object->inner[3]==0)?changeSpeedStopped:changeSpeed)) == 0) {
		         object->inner[3] = changeSpeedS + 
								(PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
		      }
	   	}   
			// Change Direction
			if (changeDirection != -1) {
	      	if ( ((ds_global_tick % changeDirection) == 0) && 
				     (PA_RandMax(99) < changeDirectionChance) ){
				   if (followJuni)
				   	object->inner[2] = (ds_juni_getXVirtual() < object->x)?-1:1;
				   else
	         		object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
	      	   object->inner[1] = ((object->inner[2] == 1)?0:endR);
	      	   object->inner[9] = 0;
	      	}
	   	}   
			// Movement - How much we should move   
   		object->inner[4] += object->inner[3];
			movPixX = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10; // Works due to integer calculations...
         // Movement - First, try to move            
			newx = object->x + (movPixX * object->inner[2]);
			if (object->inner[3] != 0) {		   
			   // We must move!
			   if (newx != object->x) {
					collided = 0;				   
					if ((newx < 0) || (newx >= (600 - object->xs))) {
					   collided = 1;
					} else {
					 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
												 object->xs,
												 object->ys,
												 newx,
												 object->y, // No movement on the Y
												 &ds_global_map.tileMapCol);
					      
					}  
					if (!collided) {
					   // Move
					   object->x = newx;
					   ds_3dsprite_setX(object->sprite,object->x);
		      	} else {
		      	   // Change Direction
		      	   object->inner[2] = (object->inner[2] == 1)?-1:1;
		      	   object->inner[1] = ((object->inner[2] == 1)?0:endR);
		      	   object->inner[9] = 0;
					}    
		    	}  	
				// Sprite Movement
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;	            
			      if (object->inner[1] >= ((object->inner[2] == 1)?endR:endL))
			      	object->inner[1] = ((object->inner[2] == 1)?0:endR);
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[9] = waitingSprite;
				}   
	    	} else {
    	      object->inner[1] = ((object->inner[2] == 1)?0:endR);
		   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
			}    	
			// However... what happens if Juni is near?????
			if (ds_objects_lib_isNearJuni(object,distanceSpikes,1)) {
			   object->inner[0] = 2;
			}   
			break;
		case 2:
		   // BEGIN - SPIKES
		   ds_music_playSound("Spike Up", 0, 0);
		   object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		   object->inner[0] = 3;
		   object->inner[1] = ((object->inner[2] == 1)?endL:endRS);
		   object->inner[9] = waitingSprite;
		   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		   break;
		case 3:
		   // SPIKES
		   // Sprite
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;	            
		      if (object->inner[1] >= ((object->inner[2] == 1)?endRS:endLS)) {
		      	if (cycleGesture) {
		      		object->inner[1] = ((object->inner[2] == 1)?endL:endRS);
		      		ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		      	} else {
		      	   object->inner[1] = ((object->inner[2] == 1)?endRS-1:endLS-1);
		      	}   
		    	} else {
		    	   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
				}    	
		   	
            object->inner[9] = waitingSprite;
			}   
			// Change state?
			if (!ds_objects_lib_isNearJuni(object,distanceSpikes+12,1)) {
			   object->inner[0] = 4;
			}   
		   break;
		case 4:
		   // END - SPIKES
		   ds_music_playSound("Spike Down", 0, 0);
		   object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_HARMFUL);
		   object->inner[0] = 1;
		   object->inner[1] = ((object->inner[2] == 1)?0:endR);
		   object->inner[9] = waitingSprite;
		   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		   break;
	}
	return res;
}

/* Behaviour: Disk-like Movement (B). Used by critters that need of disk-like movement. */
int ds_objects_lib_beh_diskMovementB(ds_t_object *object, 
									 int changeSpeed, int changeSpeedStopped,
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int end, int endG,
									 int changeDirection, int changeDirectionChance,
									 int cycleGesture,
									 int followJuni,
									 int waitingSprite) {
/*
	[0] - State
	[1] - Sprite
	[2] - Dir
	[3] - Speed
	[4] - SpeedX
	[9] - WaitSprite
*/

	int res = 0;
	int newx;
	int collided;
	int movPixX;

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[0] = 1;
	      object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
	      object->inner[1] = 0;
	      break;
	   case 1:
	      // MOVE
	      // Change Speed
	      if ((ds_global_tick % ((object->inner[3]==0)?changeSpeedStopped:changeSpeed)) == 0) {
	         object->inner[3] = changeSpeedS + 
							(PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
	      }
			// Change Direction
			if (changeDirection != -1) {
	      	if ( ((ds_global_tick % changeDirection) == 0) && 
				     (PA_RandMax(99) < changeDirectionChance) ){
				   if (followJuni)
				   	object->inner[2] = (ds_juni_getXVirtual() < object->x)?-1:1;
				   else
	         		object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
	      	   object->inner[1] = 0;
	      	   object->inner[9] = 0;
	      	}
	   	}   
			// Movement - How much we should move   
   		object->inner[4] += object->inner[3];
			movPixX = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10; // Works due to integer calculations...
         // Movement - First, try to move            
			newx = object->x + (movPixX * object->inner[2]);
			if (object->inner[3] != 0) {		   
			   // We must move!
			   if (newx != object->x) {
					collided = 0;				   
					if ((newx < 0) || (newx >= (600 - object->xs))) {
					   collided = 1;
					} else {
					 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
												 object->xs,
												 object->ys,
												 newx,
												 object->y, // No movement on the Y
												 &ds_global_map.tileMapCol);
					      
					}  
					if (!collided) {
					   // Move
					   object->x = newx;
					   ds_3dsprite_setX(object->sprite,object->x);
		      	} else {
		      	   // Change Direction
		      	   object->inner[2] = (object->inner[2] == 1)?-1:1;
		      	   object->inner[1] = 0;
		      	   object->inner[9] = 0;
					}    
		    	}  	
				// Sprite Movement
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;	            
			      if (object->inner[1] >= ((endG==0)?ds_3dsprite_getMaxFrame(object->sprite):end)) // Simplify parameters
			      	object->inner[1] = 0;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[9] = waitingSprite;
				}   
	    	} else {
	    	   // Change to gesture state, IF we have gestures
	    	   if (endG > 0)
	    	   	object->inner[0] = 2;
	    	   else {
	    	      object->inner[1] = 0;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	    	   }   
			}    	
			break;
		case 2:
		   // BEGIN - GESTURE
		   object->inner[0] = 3;
		   object->inner[1] = end;
		   object->inner[9] = waitingSprite;
		   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		   break;
		case 3:
		   // GESTURE
		   // Sprite
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;	            
		      if (object->inner[1] >= endG) {
		      	if (cycleGesture) {
		      		object->inner[1] = end;
		      		ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		      	} else {
		      	   object->inner[1] = endG-1;
		      	}   
		    	} else {
		    	   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
				}    	
		   	
            object->inner[9] = waitingSprite;
			}   
			// Change state?
	      if ((ds_global_tick % changeSpeedStopped) == 0) {
	         object->inner[3] = changeSpeedS + 
							(PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
				if (object->inner[3] != 0) {
				   object->inner[1] = 0;
				   object->inner[0] = 1;
				}   
	      }
		   break;
	}
	return res;
}

/* Behaviour: Disk-like Movement (B) with spikes. 
	Used by critters that need of disk-like movement, but show spikes whenever Juni
	is near. */
int ds_objects_lib_beh_diskMovementB_Spike(ds_t_object *object, 
									 int changeSpeed, int changeSpeedStopped,
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int end, int endS,
									 int changeDirection, int changeDirectionChance,
									 int cycleGesture,
									 int distanceSpikes,
									 int followJuni,									 
									 int waitingSprite) {
/*
	[0] - State
	[1] - Sprite
	[2] - Dir
	[3] - Speed
	[4] - SpeedX
	[9] - WaitSprite
*/

	int res = 0;
	int newx;
	int collided;
	int movPixX;

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[0] = 1;
	      object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
	   	object->inner[1] = 0;
	   case 1:
	      // MOVE
	      // Change Speed
	      if ((ds_global_tick % ((object->inner[3]==0)?changeSpeedStopped:changeSpeed)) == 0) {
	         object->inner[3] = changeSpeedS + 
							(PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
	      }
			// Change Direction
			if (changeDirection != -1) {
	      	if ( ((ds_global_tick % changeDirection) == 0) && 
				     (PA_RandMax(99) < changeDirectionChance) ){
				   if (followJuni)
				   	object->inner[2] = (ds_juni_getXVirtual() < object->x)?-1:1;
				   else
	         		object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
	      	   object->inner[9] = 0;
	      	}
	   	}   
			// Movement - How much we should move   
   		object->inner[4] += object->inner[3];
			movPixX = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10; // Works due to integer calculations...
         // Movement - First, try to move            
			newx = object->x + (movPixX * object->inner[2]);
			if (object->inner[3] != 0) {		   
			   // We must move!
			   if (newx != object->x) {
					collided = 0;				   
					if ((newx < 0) || (newx >= (600 - object->xs))) {
					   collided = 1;
					} else {
					 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
												 object->xs,
												 object->ys,
												 newx,
												 object->y, // No movement on the Y
												 &ds_global_map.tileMapCol);
					      
					}  
					if (!collided) {
					   // Move
					   object->x = newx;
					   ds_3dsprite_setX(object->sprite,object->x);
		      	} else {
		      	   // Change Direction
		      	   object->inner[2] = (object->inner[2] == 1)?-1:1;
		      	   object->inner[9] = 0;
					}    
		    	}  	
				// Sprite Movement
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;	            
			      if (object->inner[1] >= end)
			      	object->inner[1] = 0;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[9] = waitingSprite;
				}   
	    	} else {
    	      object->inner[1] = 0;
		   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
			}    	
			// However... what happens if Juni is near?????
			if (ds_objects_lib_isNearJuni(object,distanceSpikes,1)) {
			   object->inner[0] = 2;
			}   
			break;
		case 2:
		   // BEGIN - SPIKES
		   ds_music_playSound("Spike Up", 0, 0);
		   object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		   object->inner[0] = 3;
		   object->inner[1] = end;
		   object->inner[9] = waitingSprite;
		   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		   break;
		case 3:
		   // SPIKES
		   // Sprite
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;	            
		      if (object->inner[1] >= endS) {
		      	if (cycleGesture) {
		      		object->inner[1] = end;
		      		ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		      	} else {
		      	   object->inner[1] = endS-1;
		      	}   
		    	} else {
		    	   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
				}    	
		   	
            object->inner[9] = waitingSprite;
			}   
			// Change state?
			if (!ds_objects_lib_isNearJuni(object,distanceSpikes+12,1)) {
			   object->inner[0] = 4;
			}   
		   break;
		case 4:
		   // END - SPIKES
		   ds_music_playSound("Spike Down", 0, 0);
		   object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_HARMFUL);
		   object->inner[0] = 1;
		   object->inner[1] = 0;
		   object->inner[9] = waitingSprite;
		   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		   break;
	}
	return res;
}
				
/* Behaviour: Appear / Dissapear. May mark as harmful ;-) (2 implies always harmful) */
int ds_objects_lib_beh_appear(ds_t_object *object, int distNear, int distFar, int harm, int waitTime, int stuckForever) {
/* 
	[0]:State
	[1]:Waiting Time
	[2]:Actual Frame
*/ 
	int res = 0;

   switch (object->inner[0]) {
      case 0:
         // INITIALIZE
         object->inner[1] = waitTime; // Waiting time
         object->inner[2] = 0; // Frame         
         object->inner[0] = 1; // Next state
         break;
      case 1:
         // WAITING FOR JUNI...
   		if (ds_objects_lib_distancePhyJuniCorrected(object,1) < distNear) {
   		   if (harm == 1)
   		   	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
   		   object->inner[0] = 2; // Next state
   		   res = 1;
   		}  
			break; 
      case 2:
         // SHOWTIME!
         object->inner[1]--;
         if (object->inner[1] <= 0) {
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);            
         	object->inner[1] = waitTime; // Waiting time
         	object->inner[2]++;         	
         	if (object->inner[2] == ds_3dsprite_getMaxFrame(object->sprite)) {
         	   if (!stuckForever) {
						object->inner[0] = 3;      
					} else {
					   object->inner[0] = -1;      
					}   
				}			
         }   
         break;         
      case 3:
         // WAITING FOR JUNI...
   		if (ds_objects_lib_distancePhyJuniCorrected(object,1) > distFar) {
   		   object->inner[1] = waitTime; // Waiting time
   		   object->inner[0] = 4; // Next state	   
   		   res = 1;
   		}  
			break; 
      case 4:
         // GO BACK...
         object->inner[1]--;
         if (object->inner[1] <= 0) {
            object->inner[2]--; // It's OK because didn't correct it before :-)
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);            
         	object->inner[1] = waitTime; // Waiting time
         	if (object->inner[2] == 0) {
	   		   if (harm == 1)
	   		   	object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_HARMFUL);         	
					object->inner[0] = 1;      
				}			
         }   
         break;
   }   
   // Everything went OK...
   return res;
}

/* Behaviour: Particle Launch (Stop) Behaviour. Moves. Return 1 if particle is launched. */
int ds_objects_lib_beh_plaunchStop(ds_t_object *object, 
									 int countMax, int countFlash,
									 int waitingSprite) {
/*
	[0] - State
	[1] - Sprite
	[2] - Count
	[9] - WaitSprite
*/

	int res = 0;
	int cF = countFlash; 
	int cM = countMax;  

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[2] = cM;
	      object->inner[0] = 1;
	      break;
	   case 1:
	      // COUNTDOWN
	      // Go down...
	      object->inner[2]--;
	      if (object->inner[2] <= cF) {
	         object->inner[9] = waitingSprite;
	         object->inner[0] = 2;
	      }   
		   break;
		case 2:
		   // COUNTDOWN AND SHOOT
		   object->inner[2]--;
		   if (object->inner[2] == 0) {
		      // Shoot, return back
     	      object->inner[2] = cM;
     	      object->inner[1] = 0;
     	      ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		      object->inner[0] = 1;
				res = 1;
		   } else {  
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;	            
			      if (object->inner[1] >= ds_3dsprite_getMaxFrame(object->sprite)) {
						object->inner[1] = 1;
			    	}  	
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[9] = waitingSprite;
				}   
			}			
		   break;
	}
	return res;
}

/* Behaviour: Slime-like movement. With deceleration */
int ds_objects_lib_beh_slimeMovementLR(ds_t_object *object, 
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int deceleration,
									 int waitingSprite) {
/*
	[0] - State
	[1] - Sprite
	[2] - Dir
	[3] - Speed
	[4] - SpeedX
	[5] - Deceleration Factor
	[9] - WaitSprite
*/

	int res = 0;
	int newx;
	int collided;
	int movPixX;
   int realEnd;
   int realStart;
   int endR = 0;
   int endL = 0;

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[0] = 1; // New state
	      object->inner[2] = (PA_RandMax(99) < 50)?-1:1; // Direction
	      _inner_getRealVal(object,object->inner[2],endR,endL,0,0,&realStart,&realEnd);
	      object->inner[1] = realStart; // Sprites
	      object->inner[3] = changeSpeedS + (PA_RandMinMax(0,changeSpeedR) * changeSpeedM); // Speed
	      object->inner[3] *= 100; // Speed, corrected
	      object->inner[4] = 0; // Accumulated Speed
  			object->inner[5] = (object->inner[3] * deceleration) / 1000; // Deceleration X
			if ((object->inner[5] == 0) && (deceleration != 0))
				object->inner[5] = (object->inner[3] > 0)?1:-1;
			object->inner[9] = waitingSprite;
	      break;
	   case 1:
	      // PREPARE TO MOVE
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;
            _inner_getRealVal(object, object->inner[2], endR, endL, 0, 0,&realStart,&realEnd);
		      if (object->inner[1] >= realEnd) {
		         // Change sprite AND state
		      	object->inner[1] = realEnd - 1;
		      	object->inner[0] = 2; // New state!
		    	}  	
		   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
            object->inner[9] = waitingSprite;
			}   
			break;
	   case 2:
	      // MOVE
			// Movement - How much we should move, and how deceleration affects everything   
   		object->inner[4] += (object->inner[3] / 100);
			movPixX = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10;  // Works due to integer calculations...
			object->inner[3] -= object->inner[5]; // Deceleration decrease 
			if (object->inner[3] < 0)
				object->inner[3] = 0;
         // Movement - First, try to move            
			newx = object->x + (movPixX * object->inner[2]);
			if (object->inner[3] > 0) {		   
			   // We must move!
			   if (newx != object->x) {
					collided = 0;				   
					if ((newx < 0) || (newx >= (600 - object->xs))) {
					   collided = 1;
					} else {
					 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
												 object->xs,
												 object->ys,
												 newx,
												 object->y, // No movement on the Y
												 &ds_global_map.tileMapCol);
					      
					}  
					if (!collided) {
					   // Move
					   object->x = newx;
					   ds_3dsprite_setX(object->sprite,object->x);
		      	} else {
		      	   // Change Direction
		      	   object->inner[2] = (object->inner[2] == 1)?-1:1;
		      	   _inner_getRealVal(object, object->inner[2], endR, endL, 0, 0,&realStart,&realEnd);
		      	   object->inner[1] = realStart;
		      	   object->inner[9] = 0;
					}    
		    	}  	
	    	} else {
	    	   // Change State - Init again ("no memory" critter)
    	   	object->inner[0] = 0;
			}    	
			break;
	}
	return res;
}

/* Behaviour: Squirrel-like movement. With deceleration */
int ds_objects_lib_beh_squirrelMovementLR(ds_t_object *object, 
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int deceleration,
									 int countDir, int randDir,
									 int followJuni,									 
									 int waitingSprite) {
/*
	[0] - State
	[1] - Sprite
	[2] - Dir
	[3] - Speed
	[4] - SpeedX
	[5] - Deceleration Factor
	[9] - WaitSprite
	[10] - Count
*/

	int res = 0;
	int newx;
	int collided;
	int movPixX;
   int realEnd;
   int realStart;
   int endR = 0;
   int endL = 0;

	// ALWAYS DO THIS (BTW, if [10] it is not initialized, there is no problem :-), they are all ints)
	object->inner[10]--;

	// Normal state
	switch (object->inner[0]) {
	   case 0:
			// INIT
			object->inner[10] = countDir;	      
			object->inner[0] = 1; // New state
	      break;
	   case 1:
	      // START MOVEMENT INIT
	      // Check Count <= 0
	      if (object->inner[10] < 0) {
	         object->inner[10] = countDir;
	         // Move!
		      object->inner[0] = 2; // New state
		      if (followJuni == 1) {
		      	object->inner[2] = (object->x > ds_juni_getXVirtual())?-1:1; // Direction
		      } else if (followJuni == -1) {
		      	object->inner[2] = (object->x > ds_juni_getXVirtual())?1:-1; // Op. Direction
		      } else { // Others (e.g. 0)
		      	object->inner[2] = (PA_RandMax(99) < 50)?-1:1; // Direction
		   	}   
		      _inner_getRealVal(object,object->inner[2],endR,endL,0,0,&realStart,&realEnd);
		      object->inner[1] = realStart; // Sprites
		      object->inner[3] = changeSpeedS + (PA_RandMinMax(0,changeSpeedR) * changeSpeedM); // Speed
		      object->inner[3] *= 100; // Speed, corrected
		      object->inner[4] = 0; // Accumulated Speed
	  			object->inner[5] = (object->inner[3] * deceleration) / 1000; // Deceleration X
				if ((object->inner[5] == 0) && (deceleration != 0))
					object->inner[5] = (object->inner[3] > 0)?1:-1;
				object->inner[9] = waitingSprite;			
			} else {
				// Randomize direction (if not moving)
			   if ((ds_global_tick % randDir) == 0) {
			      object->inner[2] = (PA_RandMax(99) < 50)?-1:1; // Direction
			      _inner_getRealVal(object,object->inner[2],endR,endL,0,0,&realStart,&realEnd);
		   	   object->inner[1] = realStart; // Sprites
					ds_3dsprite_setFrame(object->sprite,object->inner[1]);   
				}			
			}   
	      break;
	   case 2:
	      // MOVE
			// Movement - How much we should move, and how deceleration affects everything   
   		object->inner[4] += (object->inner[3] / 100);
			movPixX = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10;  // Works due to integer calculations...
			object->inner[3] -= object->inner[5]; // Deceleration decrease 
			if (object->inner[3] < 0)
				object->inner[3] = 0;
         // Movement - First, try to move            
			newx = object->x + (movPixX * object->inner[2]);
			if (object->inner[3] > 0) {		   
			   // We must move!
			   if (newx != object->x) {
					collided = 0;				   
					if ((newx < 0) || (newx >= (600 - object->xs))) {
					   collided = 1;
					} else {
					 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
												 object->xs,
												 object->ys,
												 newx,
												 object->y, // No movement on the Y
												 &ds_global_map.tileMapCol);
					      
					}  
					if (!collided) {
					   // Move
					   object->x = newx;
					   ds_3dsprite_setX(object->sprite,object->x);
		      	} else {
		      	   // Change Direction
		      	   object->inner[2] = (object->inner[2] == 1)?-1:1;
		      	   _inner_getRealVal(object, object->inner[2], endR, endL, 0, 0,&realStart,&realEnd);
		      	   object->inner[1] = realStart;
		      	   object->inner[9] = 0;
					}    
		    	}  	
				// Sprite Movement
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;
	            _inner_getRealVal(object, object->inner[2], endR, endL, 0, 0,&realStart,&realEnd);
			      if (object->inner[1] >= realEnd)
			      	object->inner[1] = realStart;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[9] = waitingSprite;
				}
	    	} else {
	    	   // Change State - Init Movement
    	   	object->inner[0] = 1;
			}    	
			break;
	}
	return res;
}

/* Behaviour: Hunter Behaviour. Hunts Juni if conditions are met. */
int ds_objects_lib_beh_hunter(ds_t_object *object, 
									 int condYminus, int condYplus, int condTime, int condChance,
									 int speedX,
									 int followJuni, int keepOnRunning,
									 int waitingSprite, 
									 int iniRS, int iniLS, int endRS, int endLS,
									 int iniR, int iniL, int endR, int endL) {
/*
	[0] - State
	[1] - Sprite
	[2] - Dir
	[3] - Speed
	[4] - SpeedX
	[9] - WaitSprite
*/

	int res = 0; // Returns 1 if hunt starts!
	int newx;
	int collided = 0;
	int movPixX;
	int cond = 0;

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[0] = 1;
	      object->inner[2] = 1; // Always RIGHT
	      object->inner[1] = iniRS; // Initial sprite
	      object->inner[3] = speedX;
	      object->inner[9] = 0;
	      break;
	   case 1:
	      // WAIT till conditions are met...
	      // Change sprite
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;	            
		      if (object->inner[1] >= ((object->inner[2] == 1)?endRS:endLS)) // Simplify parameters
		      	object->inner[1] = (object->inner[2] == 1)?iniRS:iniLS;
		   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
            object->inner[9] = waitingSprite;
			}			
			// Conditions			
			cond = 0;
			if (condYminus != 0) {
			   // "Nearness" condition
			   int distY = ds_objects_lib_distanceJuniY(object, 1, 0);
			   cond = ((condYminus <= distY) && (distY <= condYplus));
			} else if (condTime != 0) {
			   // "Time" condition
			   cond = (((ds_global_tick % condTime) == 0) && (PA_RandMinMax(0,100) < condChance));  
			}      
			if (cond) {
			   // Time to move!
			   if (followJuni) {
			      int objXcorr = object->x + (object->xs >> 1);
			      object->inner[2] = (ds_juni_getXVirtual() < objXcorr)?-1:1;
			   }   
			   // New variables & state
			   object->inner[1] = (object->inner[2] == 1)?iniR:iniL;
			   object->inner[0] = 2;
			   res = 1;
			}   
	      break;
	   case 2:
	      // MOVE
			// Movement - How much we should move   
   		object->inner[4] += object->inner[3];
			movPixX = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10; // Works due to integer calculations...
         // Movement - First, try to move            
			newx = object->x + (movPixX * object->inner[2]);
			if (object->inner[3] != 0) {		   
			   // We must move!
			   if (newx != object->x) {
					collided = 0;				   
					if ((newx < 0) || (newx >= (600 - object->xs))) {
					   collided = 1;
					} else {
					 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
												 object->xs,
												 object->ys,
												 newx,
												 object->y, // No movement on the Y
												 &ds_global_map.tileMapCol);	      
					}  
					if (!collided) {
					   // Move
					   object->x = newx;
					   ds_3dsprite_setX(object->sprite,object->x);
		      	} else {
		      	   // Depends... Change Direction or not
		      	   if (!followJuni) {
		      	      if (keepOnRunning) {
		      	         // Run on the other direction
						      object->inner[2] = (object->inner[2] == 1)?-1:1;
						      object->inner[1] = (object->inner[2] == 1)?iniR:iniL;
						      object->inner[9] = 0;
		      	      } else {
		      	         // Change direction, but STOP
						      object->inner[2] = (object->inner[2] == 1)?-1:1;
						      object->inner[1] = (object->inner[2] == 1)?iniR:iniL;
						      object->inner[9] = 0;
						      object->inner[0] = 1; // Surprise! next iteration, go to stop
							}      
		      	   }   
					}    
		    	}  	
				// Sprite Movement
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;	            
			      if (object->inner[1] >= ((object->inner[2] == 1)?endR:endL)) // Simplify parameters
			      	object->inner[1] = (object->inner[2] == 1)?iniR:iniL;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[9] = waitingSprite;
				}   
	    	}   	
	    	// Conditions!
	    	cond = 1;
			if (condYminus != 0) {
			   // "Nearness" condition (only if followJuni or changed direction)
			   if ((followJuni) || ((keepOnRunning) && (collided))) {
				   int distY = ds_objects_lib_distanceJuniY(object, 1, 0);
				   cond = ((condYminus <= distY) && (distY <= condYplus));
				}   
			} else if (object->inner[0] == 1) {
			   // "Extra" - next state is "waiting" state
			   cond = 0;
			}        
			if (!cond) {
		      object->inner[1] = (object->inner[2] == 1)?iniRS:iniLS;
		      object->inner[0] = 1; // Surprise! next iteration, go to stop			   
			}   
			break;
	}
	return res;
}   

/* Behaviour: Launcher Movement (L-R). Critters with disk-like movement and particle launching. */
int ds_objects_lib_beh_launcherMovementLR(ds_t_object *object, 
									 int changeSpeed, int changeDirection,
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int timeChange, int distChange,
									 int timeLaunch,
									 int endR, int endL, int endRG, int endLG,
									 int cycleGesture,
									 int followJuni,									 
									 int waitingSprite) {
/*
	[0] - State
	[1] - Sprite
	[2] - Dir
	[3] - Speed
	[4] - SpeedX
	[8] - Launched
	[9] - WaitSprite
*/

	int res = 0;
	int newx;
	int collided;
	int movPixX;
   int realEnd;
   int realStart;
	int changeState = 0;

	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[0] = 1;
	      if (followJuni)
	      	object->inner[2] = (ds_juni_getXVirtual() < object->x)?-1:1;
	      else 
	      	object->inner[2] = (ds_juni_getXVirtual() < object->x)?1:-1;
	      _inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
	      object->inner[1] = realStart;
	      object->inner[3] = changeSpeedS + (PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
	      break;
	   case 1:
	      // "MOVE" STATE
	      // Change Speed
	      if (changeSpeed != 0) {
		      if ((ds_global_tick % changeSpeed) == 0) {
		         object->inner[3] = changeSpeedS + 
								(PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
		      }
	   	}   
	   	// Change Direction
	      if (changeDirection != 0) {
		      if ((ds_global_tick % changeDirection) == 0) {
			      if (followJuni)
			      	object->inner[2] = (ds_juni_getXVirtual() < object->x)?-1:1;
			      else 
			      	object->inner[2] = (ds_juni_getXVirtual() < object->x)?1:-1;
			   }  	
	   	}
			// Movement - How much we should move   
   		object->inner[4] += object->inner[3];
			movPixX = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10; // Works due to integer calculations...
         // Movement - Try to move...            
			newx = object->x + (movPixX * object->inner[2]);
			if (object->inner[3] != 0) {		   
			   // We must move!
			   if (newx != object->x) {
					collided = 0;				   
					if ((newx < 0) || (newx >= (600 - object->xs))) {
					   collided = 1;
					} else {
					 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
												 object->xs,
												 object->ys,
												 newx,
												 object->y, // No movement on the Y
												 &ds_global_map.tileMapCol);					      
					}  
					if (!collided) {
					   // Move
					   object->x = newx;
					   ds_3dsprite_setX(object->sprite,object->x);
		      	} else {
		      	   // Change Direction
		      	   object->inner[2] = (object->inner[2] == 1)?-1:1;
		      	   _inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
		      	   object->inner[1] = realStart;
		      	   object->inner[9] = 0;
					}    
		    	}  	
				// Sprite Movement
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;
	            _inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
			      if (object->inner[1] >= realEnd)
			      	object->inner[1] = realStart;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[9] = waitingSprite;
				}   
	    	}    	
	    	// Time/Distance Check! To change the actual state    	
	    	changeState = 0;
	    	if (timeChange != 0) {
		    	changeState = ((ds_global_tick % timeChange) == 0);
	  		} else if (distChange != 0) {
			   changeState = (ds_objects_lib_distanceJuni(object,1) < distChange);
			}    	
	    	if (changeState) {
	    	   // 2nd state...
		      object->inner[0] = 2;
		      object->inner[2] = (ds_juni_getXVirtual() < object->x)?-1:1; // Look at Juni (always)
		      _inner_getGestureVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
		      object->inner[1] = realStart;
		      object->inner[3] = 0;
		      object->inner[9] = waitingSprite;
		      ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		      object->inner[8] = 0; // Prepare to launch *once*...
	    	}   			
			break;
		case 2:
		   // "GESTURE" STATE
		   // Sprite
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;	            
            _inner_getGestureVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
		      if (object->inner[1] >= realEnd) {
		      	if (cycleGesture) {
		      		object->inner[1] = realStart;
		      		ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		      	} else {
		      	   object->inner[1] = realEnd-1;
		      	}   
		    	} else {
		    	   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
				}    	
		   	
            object->inner[9] = waitingSprite;
			}   
			// Time Check! Launch something...
	    	if (((ds_global_tick % timeLaunch) == 0) && (!object->inner[8])) {
	    	   if (timeChange != 0)
	    	   	// Do not launch things twice in a row when using time!!!!
	    	   	object->inner[8] = 1; 
	    	   res = 1;
	    	}   
			
	    	// Time/Distance Check! To change the actual state
	    	changeState = 0;
	    	if (timeChange != 0) {
		    	changeState = ((ds_global_tick % timeChange) == 0);
	  		} else if (distChange != 0) {
			   changeState = (ds_objects_lib_distanceJuni(object,1) > distChange + 20);
			}    	
	    	if (changeState) {
	    	   // 1st state...
		      object->inner[0] = 1;
		      if (followJuni)
		      	object->inner[2] = (ds_juni_getXVirtual() < object->x)?-1:1;
	   	   else 
	      		object->inner[2] = (ds_juni_getXVirtual() < object->x)?1:-1;
		      _inner_getRealVal(object, object->inner[2], endR, endL, endRG, endLG,&realStart,&realEnd);
	      	object->inner[1] = realStart;
		      object->inner[3] = changeSpeedS + (PA_RandMinMax(0,changeSpeedR) * changeSpeedM);
		      object->inner[9] = waitingSprite;
		      ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	    	}   
			break;
	}
	return res;
}

/* Behaviour: Partile Flier */
int ds_objects_lib_beh_flyParticle(ds_t_object *object,
									int changeSpeed, int changeDirection,
									int maxSpeed) {
/*
	[0] - State
	[1] - Sprite
	[2] - Dir (0-8, 9, 10)
	[3] - Speed
	[4] - SpeedX/Y
*/

	int movPix;
	int newx,newy;
	int collided;

	// Continue!
	switch (object->inner[0]) {
	   case 0:
	      // INIT
	      object->inner[0] = 1;
      	object->inner[2] = PA_RandMinMax(0,7); // Dir
	      object->inner[1] = object->inner[2];   // Sprite
	      object->inner[3] = (PA_RandMinMax(0,13) - 6); // Speed
	      if (object->inner[3] <= 0) {
	         object->inner[3] = 0;
	      }   
	      break;
	   case 1:
	      // "MOVE" STATE
	   	// Change Direction
	      if (changeDirection != 0) {
		      if ((ds_global_tick % changeDirection) == 0) {
					object->inner[2] = abs((object->inner[2] + PA_RandMinMax(0,4) - 2) % 8); // Dir
			   }  	
	   	}
	      // Change Speed
	      if (changeSpeed != 0) {
		      if ((ds_global_tick % changeSpeed) == 0) {
		         object->inner[3] += (PA_RandMinMax(0,8) - 4); // Speed
			      if (object->inner[3] <= 0) {
			         object->inner[3] = 0;
			      } else if (object->inner[3] >= maxSpeed) {
			         object->inner[3] = maxSpeed;
					}      
		      }
	   	}   
	   	// Change sprite
	   	int change = 0;
	      if (object->inner[3] <= 0) {
	         change = 8; // Sprite when stopped
	      } else if (object->inner[3] >= maxSpeed) {
	         change = 9; // Sprite when Max. speed
	      } else {
	         change = object->inner[2]; // Normal Sprite
			}      
	   	if (change != object->inner[1]) {
	   	   object->inner[1] = change;
	   		ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	  		} 		
	   		
			// Movement - How much we should move   
   		object->inner[4] += object->inner[3];
			movPix = object->inner[4] / 10; // REAL speed
			object->inner[4] -= (object->inner[4] / 10) * 10; // Works due to integer calculations...
         // Movement - Try to move...            
			newx = object->x;
			newy = object->y;
         switch (object->inner[2]) {
            case 0: // N
					newy -= movPix;
               break;
            case 1: // NE
					newx += movPix;
					newy -= movPix;
               break;
            case 2: // E
					newx += movPix;
               break;
            case 3: // SE
					newx += movPix;
					newy += movPix;
               break;
            case 4: // S
					newy += movPix;
               break;
            case 5: // SW
					newx -= movPix;
					newy += movPix;
               break;
            case 6: // W
					newx -= movPix;
               break;
            case 7: // NW
					newx -= movPix;
					newy -= movPix;
               break;
         }   
			if (object->inner[3] != 0) {		   
			   // We must move!
			   if ((newx != object->x) || (newy != object->y)) {
					collided = 0;				   
					if ((newx < 0) || (newx >= (600 - object->xs))) {
					   collided = 1;
					} else if ((newy < 0) || (newy >= (240 - object->ys))) {
					   collided = 1;
					} else {
					 	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
												 object->xs,
												 object->ys,
												 newx,
												 newy,
												 &ds_global_map.tileMapCol);					      
					}  
					if (!collided) {
					   // Move
					   object->x = newx;
					   object->y = newy;
					   ds_3dsprite_setX(object->sprite,object->x);
					   ds_3dsprite_setY(object->sprite,object->y);
		      	} else {
		      	   // Change Direction
		      	   object->inner[2] = (object->inner[2] + 4) % 8; // U-Turn!
		      	   object->inner[1] = object->inner[2];
					}    
		    	}  	
	    	}    	
			break;
	}	
	return 1;
}   

/* Behaviour: Elemental */
int ds_objects_lib_beh_elemental(ds_t_object *object, int spMul, int waitingSpriteTime, int damage) {
/*
	[0] - State
	[1] - Sprite
	[2] - Sprite State
	[9] - Waiting Sprite Time
*/
	// Normal
	switch (object->inner[0]) {
	   case 0:
	      /* INITIALIZATION */
	      object->inner[0] = 1; // New state
	      object->inner[2] = 0; // Sprite State
	      object->inner[9] = waitingSpriteTime >> 1; // Waiting time
	      break;
	   case 1:
	      /* TRANSITION */
	      object->inner[9]--;
	      if (object->inner[9] <= 0) {
	         if (object->inner[1] < spMul) {	         
		         ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		         object->inner[1]++;
		         object->inner[9] = waitingSpriteTime >> 1;		         
				} else {
				   // New State!
				   object->inner[2] = PA_RandMinMax(1,4);
				   object->inner[1] = object->inner[2] * spMul;
				   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
				   object->inner[9] = waitingSpriteTime;		         
				   object->inner[0] = 2; // Movement state
				}   
	      }   
	      break;
	   case 2:
	      /* MOVEMENT I */
	      object->inner[9]--;
	      if (object->inner[9] <= 0) {
	         if (object->inner[1] < ((object->inner[2] + 1) * spMul)) {	         
		         ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		         object->inner[1]++;
		         object->inner[9] = waitingSpriteTime;		         
				} else {
				   // New State!
				   object->inner[0] = 1; // Movement state
				   object->inner[1] = 0;
				   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
				   object->inner[9] = waitingSpriteTime >> 1;		         
				   object->inner[2] = 0; // Sprite State
				}   
	      }   
	      break;	      
	}   
	// Special KILL!!!!
	return ds_objects_lib_isNearJuni(object, damage, 1);
}
