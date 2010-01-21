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

#include "ds_util_bit.h"
#include <stdlib.h>
#include "ds_global.h"
#include "ds_3dsprite.h"
#include "ds_3dspritehdd.h"
#include "ds_util.h"
#include "ds_map.h"
#include "ds_input.h"
#include "ds_state.h"
#include "ds_objects.h"
#include "ds_objects_lib.h"
#include "ds_ini.h"
#include "ds_gamestatus.h"
#include "ds_camera.h"

// <TODO> - The system must be REDEFINED to fit the Box made by Juni
// <TODO> - Also, BUGS NEED TO BE FIXED!!!!! (Diagonals hacks, and so on)
// <TODO> - Besides, climb must be refined
// <TODO> - Oh, also forgot... Achieve a "natural" feeling in Juni's movement
// ...
// & <TODO> - Various types of collisions (not only "in the same tile as")

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

u8 _ds_juni_varDbl = 0;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Changes the state of "Juni" */
void _ds_juni_change(u8 state, u8 reset) {
   // Change State
   ds_global_juni.state = state;
   
   // Change Object
   ds_3dsprite_changeObj(ds_global_juni.sprite,state);
   	
	// Change specific details
	if (reset) {
		ds_global_juni.actualpix = 0;
		ds_global_juni.framepix = 0;		
		if ((state != DS_C_JUNI_ST_FALL_L) && (state != DS_C_JUNI_ST_FALL_R)) // Maintain the "CanDblJump flag"?
			ds_global_juni.inDblJump = 0;
		ds_global_juni.inNoBtnJump = 0; //  Special flag... that can be modified later :-)	
	}		
}

/* Checks if Juni is facing to the right */
int ds_juni_faceRight() {
   return ((ds_global_juni.state % 2) == 0); // HACK!!!! :-D
}

/* Checks if Juni is facing to the right (INTERNAL) */
int _ds_juni_faceRight() {
   return ds_juni_faceRight();
}

/* Checks if Juni is stepping onto a NOJUMP tile */
int _ds_juni_checkNoJump() {
   if (!ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_NOJUMP))
   	return 0; // No NOJUMP tiles in this map
   if (ds_map_collBankObj(ds_global_juni.x + (24 >> 1), ds_global_juni.y + (24), 0, 25)) { // NOJUMP Object
   	return 1;
	} 
   if (ds_map_collBankObj(ds_global_juni.x + (24 >> 1), ds_global_juni.y + (24 >> 1), 0, 25)) { // NOJUMP Object
   	// <TODO> - Touching a NOJUMP tile - Area of Juni
   	return 1;
	} 
	return 0;
}

/* Checks if Juni is stepping onto a STICKY tile */
int _ds_juni_checkSticky() {
   if (!ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_STICKY))
   	return 0; // No STICKY tiles in this map
   if (ds_map_collBankObj(ds_global_juni.x + (24 >> 1), ds_global_juni.y + (24), 0, 13)) { // STICKY Object
   	return 1;
	} 
   if (ds_map_collBankObj(ds_global_juni.x + (24 >> 1), ds_global_juni.y + (24 >> 1), 0, 13)) { // STICKY Object
   	// <TODO> - Touching a STICKY tile - Area of Juni
   	return 1;
	} 
	return 0;
}

/* Checks if Juni is touching a NOCLIMB tile */
int _ds_juni_checkNoClimb(int x, int y, int direction) {
   if (!ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_NOCLIMB))
   	return 0; // No NOCLIMB tiles in this map
   if (ds_map_collBankObj(x + direction, y + (24 >> 1), 0, 11)) { // NOCLIMB Object
   	return 1;
	} 
	return 0;
}

/* Checks if Juni is touching a CYAN GLOW tile. 1 low, 2 high */
int _ds_juni_checkCyanGlow() {
   int x = ds_global_juni.x;
   int y = ds_global_juni.y;
   if (ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_CGLOWA)) {
	   if (ds_map_collBankObj(x + (24 >> 1), y + (24 >> 1), 0, 28)) { // GLOW Object
   		return 2;
  		} 
	}	  		
	if (ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_CGLOWZ)) {
	   if (ds_map_collBankObj(x + (24 >> 1), y + (24 >> 1), 0, 27)) { // GLOW Object
   		return 1;
  		} 		
	} 
	return 0;
}


/* Checks if Juni is touching a UP WIND tile */
int _ds_juni_checkWind(int x, int y) {
   if (!ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_UPWIND))
   	return 0; // No UP WIND tiles in this map
   if (ds_map_collBankObj(x + (24 >> 1), y + 7, 15, 5) || 
		 ds_map_collBankObj(x + (24 >> 1), y + 23, 15, 5)) { // UP WIND Object
   	return 1;
	} 
	return 0;
}


/* Knows if Juni is actually running or not */
int _ds_juni_isRunning() {
   return (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_RUN))?
					(!ds_util_bitOne16(ds_global_input.Held,DS_C_IN_TWALK)):0;
}

/* Knows if Juni can "Double Jump" */
int _ds_juni_canDblJump() {
   return ((ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_DBLJUMP)) && 
			  (ds_global_juni.inDblJump));
}

/* Knows if Juni has the umbrella */
int _ds_juni_umbrellaOn() {
   return ((ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_UMBRELLA)) && 
			  (ds_global_juni.umbrellaOn));
}

/* Knows if Juni has the hologram */
int _ds_juni_hologramOn() {
   return ((ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_HOLOGRAM)) && 
			  (ds_global_juni.hologramOn));
}

/* Checks if Juni can climb */
int _ds_juni_canClimb() {
   return (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_CLIMB));
}

/* Corrects the speed of Juni due to umbrella jumping */
void _ds_juni_correctJumpUmbrella() {   
// FIXME!!!!!
if (_ds_juni_umbrellaOn()) {
	if (ds_global_juni.movstateY == DS_C_JUNI_MOVST_Y_JUMP) 
		ds_global_juni.actualpix = 2;
	if (ds_global_juni.movstateY == DS_C_JUNI_MOVST_Y_JUMPSOFT)
		ds_global_juni.actualpix = 2;
	}	
}      

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Updates position && all the sprites of Juni */
void ds_juni_updateSprites(int newx, int newy) {
	ds_global_juni.x = newx;
	ds_global_juni.y = newy;
	if (!ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_ISHIDE)) {
		ds_3dsprite_setX(ds_global_juni.sprite, newx);
		ds_3dsprite_setY(ds_global_juni.sprite, newy);
	} else {	
	   // NOTE: This is a *NASTY HACK* to allow Juni to go behind big COs.
	   // Yup, I know something like this should be done for all objects,
	   // but the mechanism we use (write to the cache of the 3Dsprite)
	   // is not valid for all objects, as many of them share the same 3D
	   // texture on the VRAM.
		// Sorry :-P. 
		ds_3dsprite_setXY_HackCO(ds_global_juni.sprite, newx, newy,
 			&ds_global_map.tileMapHide, 600, 240);
	} 		
	
	ds_3dsprite_setX(ds_global_juni.umb_sprite, newx);
	ds_3dsprite_setY(ds_global_juni.umb_sprite, newy);	
	
	ds_3dsprite_setX(ds_global_juni.redglow_sprite, newx);
	ds_3dsprite_setY(ds_global_juni.redglow_sprite, newy);	

	ds_3dsprite_setX(ds_global_juni.cyanglow_sprite, newx);
	ds_3dsprite_setY(ds_global_juni.cyanglow_sprite, newy);	

   if (!_ds_juni_hologramOn()) {
		ds_3dsprite_setX(ds_global_juni.holo_sprite, newx);
		ds_3dsprite_setY(ds_global_juni.holo_sprite, newy);	
   } else {
	}    
}

/* Check if Juni is "on the ground" - walking, running, stopped */
int ds_juni_isOnTheGround() {
   // <HACK>
   return (ds_global_juni.state < DS_C_JUNI_ST_FALL_R);
}  

/* Check if Juni is "on the ground" moving - walking, running */
int ds_juni_isOnTheGroundMoving() {
   if (!ds_juni_isOnTheGround()) {
   	return 0;
   } else if ((ds_global_juni.state != DS_C_JUNI_ST_STOP_L) && 
	           (ds_global_juni.state != DS_C_JUNI_ST_STOP_R)) {
      return 1;
   } else
   	return 0;
   return 0;
}  


/* Initializes the "Juni" structure. Flag is active if movement information / internal information should be reset */
int ds_juni_init(int x, int y, int resetmov, int resetinner) {
   // Basic initialization
   ds_global_juni.x = x;
   ds_global_juni.y = y;      
   ds_global_juni.killme = 0;
   
   if (resetinner) {
      ds_global_juni.item = 0;
      ds_global_juni.flag = 0;      
   }   
   if (resetmov) {
	   ds_global_juni.velX = 0;
	   ds_global_juni.velY = 0;
	   ds_global_juni.actualpix = 0;
		ds_global_juni.framepix = 0;		   
	   ds_global_juni.state = DS_C_JUNI_ST_STOP_R;
	   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_STOP;
	   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_STOP;
	   ds_global_juni.inDblJump = 0;
	   ds_global_juni.inNoBtnJump = 0;
	   ds_global_juni.umbrellaOn = 0;
	   ds_global_juni.umb_state = 0;
	   ds_global_juni.umb_dir = 0;
	}
	
	// CONSISTENCY CHECKS - Due to shifting, I may have lost an item.  
	ds_global_juni.hologramOn = 0;  // I *always* lose the hologram. Always ^_-
	if (!ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_UMBRELLA)) 
		ds_global_juni.umbrellaOn = 0; // No umbrella, cannot have it on.
   
   // Extended initialization - Normal and special sprites, including internal states
   ds_global_juni.sprite = ds_3dsprite_load(DS_C_JUNI_BANK,ds_global_juni.state,x,y,1); //<TODO> !!!!!
   ds_3dsprite_setPrio(ds_global_juni.sprite, (DS_C_PRIO * DS_C_PRIOLJUNISP) + 1); // 3? Layer 3 is "collision"
   ds_global_juni.umb_sprite = ds_3dsprite_load(DS_C_JUNI_BANKSP,DS_C_JUNI_SP_UMBRELLA,x,y,1); //<TODO> !!!!!   		     	
   ds_3dsprite_setPrio(ds_global_juni.umb_sprite, (DS_C_PRIO * DS_C_PRIOLJUNISP) + 5);
   if (!_ds_juni_umbrellaOn()) {
      ds_3dsprite_markInvisible(ds_global_juni.umb_sprite,1);
      ds_global_juni.umb_state = ds_3dsprite_getIniFrame_LR(ds_global_juni.umb_sprite,_ds_juni_faceRight(),1);
   } else {
      ds_global_juni.umb_state = ds_3dsprite_getMaxFrame_LR(ds_global_juni.umb_sprite,_ds_juni_faceRight(),1) - 1;
	}    
   ds_global_juni.cyanglow_sprite = ds_3dsprite_load(DS_C_JUNI_BANKSP,DS_C_JUNI_SP_CYANGLOW,x,y,1); //<TODO> !!!!!
   ds_3dsprite_setPrio(ds_global_juni.cyanglow_sprite, (DS_C_PRIO * DS_C_PRIOLJUNISP) + 10);
   	ds_3dsprite_markInvisible(ds_global_juni.cyanglow_sprite,1);
   ds_global_juni.redglow_sprite = ds_3dsprite_load(DS_C_JUNI_BANKSP,DS_C_JUNI_SP_REDGLOW,x,y,1); //<TODO> !!!!!
   ds_3dsprite_setPrio(ds_global_juni.redglow_sprite, (DS_C_PRIO * DS_C_PRIOLJUNISP) + 8);
   	ds_3dsprite_markInvisible(ds_global_juni.redglow_sprite,1);
   ds_global_juni.holo_sprite = ds_3dsprite_load(DS_C_JUNI_BANKSP,DS_C_JUNI_SP_HOLOGRAM,x,y,1); //<TODO> !!!!!
   ds_3dsprite_setPrio(ds_global_juni.holo_sprite, (DS_C_PRIO * DS_C_PRIOLJUNISP) + 1); // 3? Layer 3 is "collision"
   	ds_3dsprite_markInvisible(ds_global_juni.holo_sprite,1);    
/*      u16* ima = ds_3dsprite_getSpriteFrame(ds_global_juni.umb_sprite,7);
      int i;
      int j;
      for (j=0; j<24;j++) {
         for (i=0; i<24;i++) {
            
            PA_OutputText(1,i,j,"%d",(ima[i + (j*24)] != 0)?1:0);
         }
		}	
		while(1){}		   */
	
   return 1; // :-)
}   

/* Resets the "Juni" structure */
int ds_juni_reset() {
   // Nothing to do here... values should be stored, and sprites will be deleted by its subsystem
   // (besides, Juni does not "malloc" anything)
   return 1;
}   

/* Manages Juni's movement... Move like Juni, my friend -:) */
void _ds_juni_manageMovement() {
   int direction;
   int newx, newy, oldx, oldy;
   int newstate;
   int i;
   int ok, flag;
   int _inner_checknojump = 0;
   int diagonals;
      
   // (1) Manages Juni's movements from input
   //----------------------------------------
   if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_LEFT) || 
	    ds_util_bitOne16(ds_global_input.Held,DS_C_IN_RIGHT)) {
      // Normal Movement...
      if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_LEFT))
      	direction = -1; // Left
      else direction = 1; // Right	       
      // State "Normal" -> Walk/Run
      if ((ds_global_juni.state == DS_C_JUNI_ST_STOP_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_STOP_R)) {
			if (_ds_juni_isRunning()) {
		      newstate = (direction == -1)?DS_C_JUNI_ST_RUN_L:DS_C_JUNI_ST_RUN_R;
		      _ds_juni_change(newstate,1);			   
			   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_RUN;
			} else {
		      newstate = (direction == -1)?DS_C_JUNI_ST_WALK_L:DS_C_JUNI_ST_WALK_R;
		      _ds_juni_change(newstate,1);			   			   
			   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_WALK;
			}      
		} else
      // State "Walk" -> Walk/Run bis :-)
      if ((ds_global_juni.state == DS_C_JUNI_ST_WALK_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_WALK_R)) {					       
			if (_ds_juni_isRunning()) {
		      newstate = (direction == -1)?DS_C_JUNI_ST_RUN_L:DS_C_JUNI_ST_RUN_R;
		      _ds_juni_change(newstate,1);			   
			   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_RUN;
			} else {
		      if (((direction == 1) && (ds_global_juni.state == DS_C_JUNI_ST_WALK_L)) ||
				    ((direction == -1) && (ds_global_juni.state == DS_C_JUNI_ST_WALK_R))) {
			      newstate = (direction == -1)?DS_C_JUNI_ST_WALK_L:DS_C_JUNI_ST_WALK_R;
			      _ds_juni_change(newstate,1);			   			   
				   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_WALK;
				}   
			}      
		} else
      // State "Run" -> Walk/Run... but if change direction, skid :-)
      if ((ds_global_juni.state == DS_C_JUNI_ST_RUN_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_RUN_R)) {					       
			if (_ds_juni_isRunning()) {
		      if (((direction == 1) && (ds_global_juni.state == DS_C_JUNI_ST_RUN_L)) ||
				    ((direction == -1) && (ds_global_juni.state == DS_C_JUNI_ST_RUN_R))) {
			      newstate = (direction == -1)?DS_C_JUNI_ST_RUN_L:DS_C_JUNI_ST_RUN_R;
			      _ds_juni_change(newstate,1);			   			   
				   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_RUNSKID;
				}   
			} else {
		      newstate = (direction == -1)?DS_C_JUNI_ST_WALK_L:DS_C_JUNI_ST_WALK_R;
		      _ds_juni_change(newstate,1);			   			   
			   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_WALK;
			}      
		} else
		// State "Fall" -> Change movement, not state
      if ((ds_global_juni.state == DS_C_JUNI_ST_FALL_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_FALL_R)) {
			if (((direction == 1) && (ds_global_juni.state == DS_C_JUNI_ST_FALL_L)) ||
				 ((direction == -1) && (ds_global_juni.state == DS_C_JUNI_ST_FALL_R))) {
				newstate = (direction == -1)?DS_C_JUNI_ST_FALL_L:DS_C_JUNI_ST_FALL_R;
			   _ds_juni_change(newstate,0);			   			   
				ds_global_juni.movstateX = (_ds_juni_isRunning())?DS_C_JUNI_MOVST_X_RUNSKID:DS_C_JUNI_MOVST_X_WALK;
			} else {
			   ds_global_juni.movstateX = (_ds_juni_isRunning())?DS_C_JUNI_MOVST_X_RUN:DS_C_JUNI_MOVST_X_WALK;          
			} 
		} else
		// State "Fly" -> Change movement, not state
      if ((ds_global_juni.state == DS_C_JUNI_ST_FLY_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_FLY_R)) {
			if (((direction == 1) && (ds_global_juni.state == DS_C_JUNI_ST_FLY_L)) ||
				 ((direction == -1) && (ds_global_juni.state == DS_C_JUNI_ST_FLY_R))) {
				newstate = (direction == -1)?DS_C_JUNI_ST_FLY_L:DS_C_JUNI_ST_FLY_R;
			   _ds_juni_change(newstate,0);			   			   
				ds_global_juni.movstateX = (_ds_juni_isRunning())?DS_C_JUNI_MOVST_X_RUNSKID:DS_C_JUNI_MOVST_X_WALK;
			} else {
			   ds_global_juni.movstateX = (_ds_juni_isRunning())?DS_C_JUNI_MOVST_X_RUN:DS_C_JUNI_MOVST_X_WALK;          
			} 
		} else		
		// State "Jump" -> Change movement, not state
      if ((ds_global_juni.state == DS_C_JUNI_ST_JUMP_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_JUMP_R)) {
			if (((direction == 1) && (ds_global_juni.state == DS_C_JUNI_ST_JUMP_L)) ||
				 ((direction == -1) && (ds_global_juni.state == DS_C_JUNI_ST_JUMP_R))) {
				newstate = (direction == -1)?DS_C_JUNI_ST_JUMP_L:DS_C_JUNI_ST_JUMP_R;
			   _ds_juni_change(newstate,0);			   			   
				ds_global_juni.movstateX = (_ds_juni_isRunning())?DS_C_JUNI_MOVST_X_RUNSKID:DS_C_JUNI_MOVST_X_WALK;
			} else {
			   ds_global_juni.movstateX = (_ds_juni_isRunning())?DS_C_JUNI_MOVST_X_RUN:DS_C_JUNI_MOVST_X_WALK;          
			} 
		} else
		// State "Climb" -> Don't want to climb? Falls down. Fast.
      if ((ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_R)) {
			if (((direction == 1) && (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L)) ||
				 ((direction == -1) && (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_R))) {
				if (direction == 1)
					newstate = (_ds_juni_isRunning())?DS_C_JUNI_ST_RUN_R:DS_C_JUNI_ST_WALK_R;
				else
					newstate = (_ds_juni_isRunning())?DS_C_JUNI_ST_RUN_L:DS_C_JUNI_ST_WALK_L;
				_ds_juni_change(newstate,1);
			   ds_global_juni.movstateX = (_ds_juni_isRunning())?DS_C_JUNI_MOVST_X_RUN:DS_C_JUNI_MOVST_X_WALK;          
			   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_STOP;
			} 
		}
	} else
   if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_UP)) {
      // Check if we need to go climbing
      if (_ds_juni_canClimb()) {
         if ((ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L) || 
			    (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_R)) {
			   // Keep on climbing
			   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_CLIMB;
			} else {
			   // Climb?
	         flag = 0;
  				newx = ds_3dsprite_getX(ds_global_juni.sprite);
	   		newy = ds_3dsprite_getY(ds_global_juni.sprite);
		      if ((ds_global_juni.state == DS_C_JUNI_ST_STOP_R) ||
				    (ds_global_juni.state == DS_C_JUNI_ST_WALK_R) ||
				    (ds_global_juni.state == DS_C_JUNI_ST_RUN_R)) {
	   				newx = ds_3dsprite_getX(ds_global_juni.sprite) + 1;
	   				flag = (ds_map_collMovBasic(newx,newy,17) && !_ds_juni_checkNoClimb(newx,newy,17));
				}
		      if ((ds_global_juni.state == DS_C_JUNI_ST_STOP_L) ||
					 (ds_global_juni.state == DS_C_JUNI_ST_WALK_L) || 
		      	 (ds_global_juni.state == DS_C_JUNI_ST_RUN_L)) {
	   				newx = ds_3dsprite_getX(ds_global_juni.sprite) - 1;
	   				flag = (ds_map_collMovBasic(newx,newy,6) && !_ds_juni_checkNoClimb(newx,newy,6)); // Same check as collision, 2b
				}
				if (flag) {
					newstate = (newx < ds_3dsprite_getX(ds_global_juni.sprite))?DS_C_JUNI_ST_CLIMB_L:DS_C_JUNI_ST_CLIMB_R;
				   _ds_juni_change(newstate,1);			   			   		   
				   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_STOP;
				   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_CLIMB;
				}   
			}			
		}		    
      // If in certain states, "look up" (camera)
      if (ds_juni_isOnTheGround()) {
         // If we are on the ground, continue with the camera!!!!
         ds_camera_moveCoord(0,-1);
      }   
   } else  
   if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_DOWN)) {
      // Check if we need to go climbing
      if (_ds_juni_canClimb()) {
         if ((ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L) || 
			    (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_R)) {
			   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_SLIDE;
			} 
		}		          
      // If in certain states, "look down" (camera)
      if (ds_juni_isOnTheGround()) {
         // If we are on the ground, continue with the camera!!!!
         ds_camera_moveCoord(0,1);
      }         
   } else { // (ds_util_bitOne16(ds_global_input.Held,PLAYER DOES NOT HELD ANY MOV KEY!)) ^_-
      // State "Normal" -> ...
      if ((ds_global_juni.state == DS_C_JUNI_ST_STOP_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_STOP_R)) {
		} else
      // State "Walk" -> STOP
      if ((ds_global_juni.state == DS_C_JUNI_ST_WALK_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_WALK_R)) {
			// Freeze!
		   newstate = (ds_global_juni.state == DS_C_JUNI_ST_WALK_L)?DS_C_JUNI_ST_STOP_L:DS_C_JUNI_ST_STOP_R;
		   _ds_juni_change(newstate,1);
		   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_STOP;			   
		} else				   
      // State "Run" -> Decrease your speed... (done while stopping)
      if ((ds_global_juni.state == DS_C_JUNI_ST_RUN_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_RUN_R)) {
			// Stop!
		   newstate = (ds_global_juni.state == DS_C_JUNI_ST_RUN_L)?DS_C_JUNI_ST_STOP_L:DS_C_JUNI_ST_STOP_R;
		   _ds_juni_change(newstate,1);
			ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_RUNSTOP;		   
		} else  		
      // State "Fall" -> Keep on falling, but with less speed :-)
      if ((ds_global_juni.state == DS_C_JUNI_ST_FALL_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_FALL_R)) {
			ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_RUNSTOP;		   
		} else   		
      // State "Fly" -> Keep on falling, but with less speed :-)
      if ((ds_global_juni.state == DS_C_JUNI_ST_FLY_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_FLY_R)) {
			ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_RUNSTOP;		   
		} else   				
      // State "Jump" -> Keep on jumping, but with less speed :-)
      if ((ds_global_juni.state == DS_C_JUNI_ST_JUMP_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_JUMP_R)) {
			ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_RUNSTOP;
		}    
      // State "Climb" -> Landslide downfall! (<-meaningless, but wanted to write it - sounds cool! XD)
      if ((ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_R)) {
			ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_SLIDESOFT;
			ds_global_juni.actualpix = 0;
		}    		
   }   
   
   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_DOWN)) {
      // If in certain states, START "look down" (camera)
      if (ds_juni_isOnTheGround()) {
         // If we are on the ground, activate the camera!!!!
         ds_camera_setType(DS_C_CAM_COORD,ds_global_juni.x,ds_global_juni.y);
         ds_camera_moveCoord(0,1);
      }         
   }    
   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_UP)) {
      // If in certain states, START "look up" (camera)
      if (ds_juni_isOnTheGround()) {
         // If we are on the ground, activate the camera!!!!
         ds_camera_setType(DS_C_CAM_COORD,ds_global_juni.x,ds_global_juni.y);
         ds_camera_moveCoord(0,-1);
      }         
   }       
   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_L)) {
      // Camera!!!! We want to look to a certain direction
      ds_camera_setType(DS_C_CAM_COORD,ds_global_juni.x,ds_global_juni.y);
      ds_camera_moveCoord(-1,0);
   } else
   if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_L)) {
      // Camera!!!! Move!!!!
      ds_camera_moveCoord(-1,0);      
   }	  
   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_R)) {
      // Camera!!!! We want to look to a certain direction
      ds_camera_setType(DS_C_CAM_COORD,ds_global_juni.x,ds_global_juni.y);
      ds_camera_moveCoord(1,0);
   } else
   if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_R)) {
      // Camera!!!! Move!!!!
      ds_camera_moveCoord(1,0);      
   } 
	 
   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_TJUMP)) {
      // Jump Juni Jump!!!! (only once per click)
      // Normal (Note: Check NOJUMP existence)
      if ((ds_global_juni.state == DS_C_JUNI_ST_STOP_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_STOP_R)) {
		   if (!_ds_juni_checkNoJump()) {
				newstate = (ds_global_juni.state == DS_C_JUNI_ST_STOP_L)?DS_C_JUNI_ST_JUMP_L:DS_C_JUNI_ST_JUMP_R;
				_ds_juni_change(newstate,1);
				ds_global_juni.inDblJump = 1; // After state change. This means: I "may" DblJump later
				ds_global_juni.movstateY = (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP))?
														DS_C_JUNI_MOVST_Y_JUMP:DS_C_JUNI_MOVST_Y_JUMPSOFT;
				_ds_juni_correctJumpUmbrella(); // If Umbrella, jump is a bit slower
			}			
		} else
      // Walking
      if ((ds_global_juni.state == DS_C_JUNI_ST_WALK_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_WALK_R)) {
		   if (!_ds_juni_checkNoJump()) {		       
				newstate = (ds_global_juni.state == DS_C_JUNI_ST_WALK_L)?DS_C_JUNI_ST_JUMP_L:DS_C_JUNI_ST_JUMP_R;
				_ds_juni_change(newstate,1);
				ds_global_juni.inDblJump = 1; // After state change. This means: I "may" DblJump later
				ds_global_juni.movstateY = (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP))?
														DS_C_JUNI_MOVST_Y_JUMP:DS_C_JUNI_MOVST_Y_JUMPSOFT;
				_ds_juni_correctJumpUmbrella(); // If Umbrella, jump is a bit slower
			}			
		} else	
      // Running
      if ((ds_global_juni.state == DS_C_JUNI_ST_RUN_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_RUN_R)) {
		   if (!_ds_juni_checkNoJump()) {		       
				newstate = (ds_global_juni.state == DS_C_JUNI_ST_RUN_L)?DS_C_JUNI_ST_JUMP_L:DS_C_JUNI_ST_JUMP_R;
				_ds_juni_change(newstate,1);
				ds_global_juni.inDblJump = 1; // After state change. This means: I "may" DblJump later
				ds_global_juni.movstateY = (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP))?
														DS_C_JUNI_MOVST_Y_JUMP:DS_C_JUNI_MOVST_Y_JUMPSOFT;
				_ds_juni_correctJumpUmbrella(); // If Umbrella, jump is a bit slower
			}			
		} else
      // Climbing (special case, goes back some pixels)
      if ((ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_R)) {
		   direction = (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L)?-1:1;
			newstate = (direction == -1)?DS_C_JUNI_ST_JUMP_L:DS_C_JUNI_ST_JUMP_R;
			_ds_juni_change(newstate,1);
			ds_global_juni.inDblJump = 1; // After state change. This means: I "may" DblJump later
			ds_global_juni.movstateY = (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP))?
													DS_C_JUNI_MOVST_Y_JUMP:DS_C_JUNI_MOVST_Y_JUMPSOFT;
			_ds_juni_correctJumpUmbrella(); // If Umbrella, jump is a bit slower
			ds_3dsprite_setX(ds_global_juni.sprite, ds_3dsprite_getX(ds_global_juni.sprite) + (-3 * direction)); // Allows fast climbing :-)
		} else
		// Jump - Dbl Jump
      if ((ds_global_juni.state == DS_C_JUNI_ST_JUMP_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_JUMP_R)) {
		   if (_ds_juni_canDblJump()) {
				newstate = (ds_global_juni.state == DS_C_JUNI_ST_JUMP_L)?DS_C_JUNI_ST_JUMP_L:DS_C_JUNI_ST_JUMP_R;
				_ds_juni_change(newstate,1);		      
				ds_global_juni.movstateY = (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP))?
														DS_C_JUNI_MOVST_Y_JUMP:DS_C_JUNI_MOVST_Y_JUMPSOFT;
				_ds_juni_correctJumpUmbrella(); // If Umbrella, jump is a bit slower
				_ds_juni_varDbl = 1;
		   }   
		} else
		// Fly - Fly Faster (or double jump...)
      if ((ds_global_juni.state == DS_C_JUNI_ST_FLY_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_FLY_R)) {
		   if (_ds_juni_checkWind(ds_global_juni.x,ds_global_juni.y)) {
		      if (_ds_juni_canDblJump()) {
		         // Riding the wind + dbl jump = Fast Jump is even faster!!! (max value)
		         ds_global_juni.inDblJump = 0;
		         ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FLYFAST;
		         ds_global_juni.actualpix = 30;
		         ds_global_juni.framepix = 30;
		      } else {
		         // Basic Fast Jump...
		         ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FLYFAST;
				}     
		   } else {  
		      // Normal Double Jump...
			   if (_ds_juni_canDblJump()) {
					newstate = (_ds_juni_faceRight())?DS_C_JUNI_ST_JUMP_R:DS_C_JUNI_ST_JUMP_L;
					_ds_juni_change(newstate,1);		      
					ds_global_juni.movstateY = (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP))?
															DS_C_JUNI_MOVST_Y_JUMP:DS_C_JUNI_MOVST_Y_JUMPSOFT;
					_ds_juni_correctJumpUmbrella(); // If Umbrella, jump is a bit slower
					_ds_juni_varDbl = 1;
				}			
		   }   
		} else		
		// Fall - Dbl Jump
      if ((ds_global_juni.state == DS_C_JUNI_ST_FALL_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_FALL_R)) {
		   if (_ds_juni_canDblJump()) {
				newstate = (ds_global_juni.state == DS_C_JUNI_ST_FALL_L)?DS_C_JUNI_ST_JUMP_L:DS_C_JUNI_ST_JUMP_R;
				_ds_juni_change(newstate,1);		      
				ds_global_juni.movstateY = (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP))?
														DS_C_JUNI_MOVST_Y_JUMP:DS_C_JUNI_MOVST_Y_JUMPSOFT;
				_ds_juni_correctJumpUmbrella(); // If Umbrella, jump is a bit slower
				_ds_juni_varDbl = 1;
		   }   
		}		
   } else if (!ds_util_bitOne16(ds_global_input.Held,DS_C_IN_TJUMP)) {   
      // Don't Jump Juni, Don't Jump!!!!
      if ((ds_global_juni.state == DS_C_JUNI_ST_JUMP_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_JUMP_R)) {
		   if (ds_global_juni.movstateY == DS_C_JUNI_MOVST_Y_JUMP)
			   _inner_checknojump = 1;
		} 
	} else if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_TJUMP)) {   	
		// Fly - Fly Faster (no double jump... :-P)
      if ((ds_global_juni.state == DS_C_JUNI_ST_FLY_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_FLY_R)) {
		   if (_ds_juni_checkWind(ds_global_juni.x,ds_global_juni.y)) {
	         // Basic Fast Jump...
	         ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FLYFAST;
		   }
		}   
	}   
   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_TUMBR)) {
      // It's raining! Umbrella time...
      if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_UMBRELLA)) {
         if (_ds_juni_umbrellaOn()) {
            // Close...
            ds_global_juni.umb_state--; // Correct the actual sprite
            ds_global_juni.umbrellaOn = 0;
         } else {
            // Open
            ds_3dsprite_markInvisible(ds_global_juni.umb_sprite,0);
            ds_global_juni.umbrellaOn = 1;
            ds_global_juni.umb_state = ds_3dsprite_getIniFrame_LR(ds_global_juni.umb_sprite,_ds_juni_faceRight(),1);
            ds_global_juni.umb_dir = _ds_juni_faceRight(); // Should remember this :-)
			}      
   	}   
	}	
   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_THOLO)) {
      if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_HOLOGRAM)) {
	      // Hologram time! Put or quit. The position will manage itself when updating sprites :-)      
	      if (!_ds_juni_hologramOn()) {
				if ((ds_global_juni.state == DS_C_JUNI_ST_STOP_R) ||
			 	    (ds_global_juni.state == DS_C_JUNI_ST_WALK_R) ||
		 		    (ds_global_juni.state == DS_C_JUNI_ST_RUN_R)) {
		 		      ds_3dsprite_markInvisible(ds_global_juni.holo_sprite,0);
		 		      ds_3dsprite_setFrame(ds_global_juni.holo_sprite,0);
		 		      ds_global_juni.hologramOn = 1;
		 		      ds_3dsprite_setAlpha(ds_global_juni.sprite,128);
				} else
				if ((ds_global_juni.state == DS_C_JUNI_ST_STOP_L) ||
					 (ds_global_juni.state == DS_C_JUNI_ST_WALK_L) || 
				    (ds_global_juni.state == DS_C_JUNI_ST_RUN_L)) {
				      ds_3dsprite_markInvisible(ds_global_juni.holo_sprite,0);
		 		      ds_3dsprite_setFrame(ds_global_juni.holo_sprite,2);
						ds_3dsprite_setAlpha(ds_global_juni.sprite,128);
		 		      ds_global_juni.hologramOn = 1;
				}     
			} else {
			   ds_3dsprite_markInvisible(ds_global_juni.holo_sprite,1);
			   ds_3dsprite_setAlpha(ds_global_juni.sprite,255);
			   ds_global_juni.hologramOn = 0;
			}   	
		}			
	}	
	
	// 1.5a) Special case... if sticky... stop!!!!!
	if (ds_juni_isOnTheGround())
	if ((ds_global_juni.state != DS_C_JUNI_ST_STOP_L) && (ds_global_juni.state != DS_C_JUNI_ST_STOP_R))	
	if (_ds_juni_checkSticky()) {
	   newstate = (_ds_juni_faceRight())?DS_C_JUNI_ST_STOP_R:DS_C_JUNI_ST_STOP_L;
	   _ds_juni_change(newstate,1);
	   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_STOP;			   	   
	}   


	// <TODO> Climb + Wind
	// 1.5b) Special case... fly or not fly
	if ((ds_global_juni.state != DS_C_JUNI_ST_FLY_L) && (ds_global_juni.state != DS_C_JUNI_ST_FLY_R)) {
	   if (_ds_juni_umbrellaOn())
	   if (_ds_juni_checkWind(ds_global_juni.x,ds_global_juni.y)) {
	      // Wind! Let's ride it!
		   newstate = (_ds_juni_faceRight())?DS_C_JUNI_ST_FLY_R:DS_C_JUNI_ST_FLY_L;
		   _ds_juni_change(newstate,1);
	   	ds_global_juni.movstateY = (ds_global_juni.velY > 0)?DS_C_JUNI_MOVST_Y_FLYSTART:DS_C_JUNI_MOVST_Y_FLY; 
	   	if (ds_global_juni.movstateY == DS_C_JUNI_MOVST_Y_FLYSTART) {
	   	   ds_global_juni.actualpix = 0 - (ds_global_juni.velY);
	   	   ds_global_juni.framepix = ds_global_juni.actualpix;
	   	}   
	   }   
	} else {
	   if (!_ds_juni_umbrellaOn() || !_ds_juni_checkWind(ds_global_juni.x,ds_global_juni.y)) {
	      // No wind... time to stop our umbrella travels...
			ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FLYEND;
	   }   
	}      
	
	//sprintf(ds_global_string,"(%d)(%d,%d) - (%d,%d) = (%d,%d)        ",ds_global_juni.state,ds_global_juni.movstateX,ds_global_juni.movstateY,ds_global_juni.velX,ds_global_juni.velY,ds_global_juni.actualpix,ds_global_juni.framepix);
	//ds_gamestatus_debugOutput(1,0,4,ds_global_string);

	
	// 2) Set movements
	//-----------------
	switch (ds_global_juni.movstateX) {
		case DS_C_JUNI_MOVST_X_STOP:
		   	ds_global_juni.velX = 0;
		   	break;
		case DS_C_JUNI_MOVST_X_WALK:
		   	if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_LEFT))
		   		ds_global_juni.velX = ((ds_global_juni.actualpix % 2) == 0)?-1:-2; // 90px/sec		   
		   	else if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_RIGHT))
			   	ds_global_juni.velX = ((ds_global_juni.actualpix % 2) == 0)?1:2; // 90px/sec
		   	break;
		case DS_C_JUNI_MOVST_X_RUN:
		   	if (_ds_juni_umbrellaOn()) {
			   	if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_LEFT))
			   		ds_global_juni.velX = ((ds_global_juni.actualpix % 4) == 0)?-3:-2;
			   	else if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_RIGHT))
				   	ds_global_juni.velX = ((ds_global_juni.actualpix % 4) == 0)?3:2;
				} else {
			   	if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_LEFT))
			   		ds_global_juni.velX = ((ds_global_juni.actualpix % 5) == 0)?-2:-3; // 170px/sec
			   	else if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_RIGHT))
				   	ds_global_juni.velX = ((ds_global_juni.actualpix % 5) == 0)?2:3; // 170px/sec
		   	}   
		   	break;
		case DS_C_JUNI_MOVST_X_RUNSTOP:
		   	if (ds_global_juni.velX > 0)
		   		ds_global_juni.velX--;
		   	else
				if (ds_global_juni.velX < 0)
		   		ds_global_juni.velX++;
		   	else
		   		ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_STOP;
		   	break;
		case DS_C_JUNI_MOVST_X_RUNSKID:
		   	if (ds_global_juni.velX > 0)
		   		ds_global_juni.velX--;
		   	else
				if (ds_global_juni.velX < 0)
		   		ds_global_juni.velX++;
		   	else 
		   		ds_global_juni.movstateX = (_ds_juni_isRunning())?DS_C_JUNI_MOVST_X_RUN:DS_C_JUNI_MOVST_X_WALK;	
		   	break;
	}
	switch (ds_global_juni.movstateY) {
		case DS_C_JUNI_MOVST_Y_STOP:
		   	ds_global_juni.velY = 0;
		   	break;
		case DS_C_JUNI_MOVST_Y_JUMP:
		   	// Check if we stop jumping
		   	if ((_inner_checknojump) && (!ds_global_juni.inNoBtnJump)) { // Maintains actual speed, but changes type of Y
		   	   if (ds_global_juni.actualpix < 8)
		   	      ds_global_juni.actualpix = 8;
		   	   else if (ds_global_juni.actualpix < (8 + 4))
					   ds_global_juni.actualpix = (8 + 4);		   
		   	   else if (ds_global_juni.actualpix < (8 + 4 + 5))
					   ds_global_juni.actualpix = (8 + 4 + 5);		   
		   	   else if (ds_global_juni.actualpix < (8 + 14))
					   ds_global_juni.actualpix = (8 + 14);
					else if (ds_global_juni.actualpix < (8 + 14 + 7))
					   ds_global_juni.actualpix = (8 + 14 + 7);		   					   
				}			
				// Now, jump 
	   	   if (ds_global_juni.actualpix < 8) { // First 8 frames, jump up 24 pixels (+3)
	   	      if (ds_global_juni.actualpix < 5)
	   	      	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?-4:-3;
	   	      else 
	   	      	ds_global_juni.velY = -3;
   		   } else if (ds_global_juni.actualpix < (8 + 14)) { // Next 14 frames, jump other 24 pixels
				   if (ds_global_juni.actualpix < (8 + 4))
				   	ds_global_juni.velY = -2;
				   else if (ds_global_juni.actualpix < (8 + 4 + 5))
				   	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?-2:-1;
				   else ds_global_juni.velY = -1;
				} else if (ds_global_juni.actualpix < (8 + 14 + 7)) { // Next 7 frames, go up... a bit...
				   ds_global_juni.velY = ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?-1:0;
				} else if (ds_global_juni.actualpix < (8 + 14 + 7 + 4)) { // Next 4 frames, "fly"
				   ds_global_juni.velY = 0;
				} else {
					newstate = (ds_global_juni.state == DS_C_JUNI_ST_JUMP_L)?DS_C_JUNI_ST_FALL_L:DS_C_JUNI_ST_FALL_R;
					_ds_juni_change(newstate,1);
					ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FALL;
				}
		   	break;
		case DS_C_JUNI_MOVST_Y_JUMPSOFT:
	   		if (ds_global_juni.actualpix < 17) { // Next 15(17?) frames, jump a bit more than 24 pixels
				   if (ds_global_juni.actualpix < 6) // 4
				   	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?-3:-2;
				   else 
				   if (ds_global_juni.actualpix < 9) // 8
				   	ds_global_juni.velY = -2;
				   else 
				   if (ds_global_juni.actualpix < 12) // ---
				   	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?-2:-1;
				   else
				   	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?-1:0;
				} else {
					newstate = (ds_global_juni.state == DS_C_JUNI_ST_JUMP_L)?DS_C_JUNI_ST_FALL_L:DS_C_JUNI_ST_FALL_R;
					_ds_juni_change(newstate,1);
					ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FALL;
				}   
		   	break;
		case DS_C_JUNI_MOVST_Y_CLIMB:
		   	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?-1:-2;
		   	break;
		case DS_C_JUNI_MOVST_Y_SLIDESOFT:
				ds_global_juni.velY = ((ds_global_juni.actualpix % 3) == 0)?1:0;		   
		   	break;
		case DS_C_JUNI_MOVST_Y_SLIDE: // PS: Same as falling...<COPY IF CHANGE>
	   	   if (ds_global_juni.actualpix < 12) { // First 12 frames, fall 24 px
	   	      if (ds_global_juni.actualpix < 4)
	   	      	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?2:1;
	   	      else if (ds_global_juni.actualpix < 8)
	   	      	ds_global_juni.velY = 2;
	   	      else
	   	      	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?2:3;
	   	   } else if (ds_global_juni.actualpix < (12 + 6)) { // Next 6 frames, fall "24" px
				   ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?4:3;
				} else { // Simply, fall...
				   ds_global_juni.velY = 4;
				}		   
		   	break;		   	
		case DS_C_JUNI_MOVST_Y_FALL:
		   	if (_ds_juni_umbrellaOn()) {
		   		ds_global_juni.actualpix = 0; // Never fall!
		   		ds_global_juni.velY = 1;
		   	} else
	   	   if (ds_global_juni.actualpix < 12) { // First 12 frames, fall 24 px
	   	      if (ds_global_juni.actualpix < 4)
	   	      	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?2:1;
	   	      else if (ds_global_juni.actualpix < 8)
	   	      	ds_global_juni.velY = 2;
	   	      else
	   	      	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?2:3;
	   	   } else if (ds_global_juni.actualpix < (12 + 6)) { // Next 6 frames, fall "24" px
				   ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?4:3;
				} else { // Simply, fall...
				   ds_global_juni.velY = 4;
				}		   
		   	break;
		case DS_C_JUNI_MOVST_Y_FLY:
	   	   if (ds_global_juni.actualpix < 20) {
   	      	ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?-1:0;
	   	   } else if (ds_global_juni.actualpix < 40) {
				   ds_global_juni.velY = -1;
	   	   } else if (ds_global_juni.actualpix < 60) {
				   ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?-1:-2;				   
	   	   } else if (ds_global_juni.actualpix < 80) {
				   ds_global_juni.velY = -2;
	   	   } else if (ds_global_juni.actualpix < 100) {
				   ds_global_juni.velY = ((ds_global_juni.actualpix % 2) == 0)?-2:-3;				   
	   	   } else { //if (ds_global_juni.actualpix < 80) {
				   ds_global_juni.velY = -3;
				}			
		   	break;		   	
		case DS_C_JUNI_MOVST_Y_FLYEND:		   	
		   	if (ds_global_juni.velY >= 0) {
		   	   // Time to fall...
					newstate = (_ds_juni_faceRight())?DS_C_JUNI_ST_FALL_R:DS_C_JUNI_ST_FALL_L;
					_ds_juni_change(newstate,1);
					ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FALL;		   	   
		   	} else {   
			   	if ((ds_global_juni.actualpix % 7) == 0)
			   		ds_global_juni.velY++;
			  	} 		
		   	break;
		case DS_C_JUNI_MOVST_Y_FLYSTART:		   	
		   	ds_global_juni.velY = 1;
		   	if (ds_global_juni.actualpix >= 0) {
		   	   // Time to fly!
					ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FLY;		   	   
					ds_global_juni.actualpix = 0;
					ds_global_juni.framepix = 0;
		   	}
		   	break;		   	
		case DS_C_JUNI_MOVST_Y_FLYFAST:
	   	   if (ds_global_juni.actualpix < 10) {
   	      	ds_global_juni.velY = -1;
	   	   } else if (ds_global_juni.actualpix < 20) {
				   ds_global_juni.velY = -2;
	   	   } else if (ds_global_juni.actualpix < 30) {
				   ds_global_juni.velY = -3;				   
	   	   } else {
				   ds_global_juni.velY = -4;
	   	   } 		   
		   	break;
	}

	// 2a) Set movements
	//------------------
   oldx = ds_3dsprite_getX(ds_global_juni.sprite);
   oldy = ds_3dsprite_getY(ds_global_juni.sprite);
   newx = oldx + ds_global_juni.velX;
   newy = oldy + ds_global_juni.velY;
   
	//PA_OutputText(1,1,6,"(%d,%d)(%d,%d) - (%d,%d)[%d]       ",newx,newy,oldx,oldy,ds_global_juni.velX,ds_global_juni.velY,ds_global_juni.inDblJump);
	//PA_OutputText(1,1,7,"(%d,%d) - (%d,%d)[%d]          ",ds_global_juni.actualpix,ds_global_juni.framepix,ds_global_juni.movstateY,ds_global_juni.movstateX,ds_global_juni.state);

   
	// 2b) Clash with environment 
	// IMPORTANT: Corrections are done in X using the old Y...Avoid problems in diagonals!!!!
	//---------------------------------------------------------------------------------------
	
	// <TODO> <FIXME> PROBLEM: Diagonales!!!!!!!!!!!!!!
	// <TODO> <FIXME> The problem is THE BOX!!!!!!!!!!!!!!!!!!!!!!!!
	//          Need to redesign the collision markers (even sprites) for juni
	// In other words... it works, but the mess is too much XD
	diagonals = 0;
	
   // Clash with environment, Horizontal Speed. 17 ->, 5 <-.
	if (ds_global_juni.velX > 0) {
	   if (ds_map_collMovBasic(newx,newy,17)) {
	      // Collision! - Let's move Juni to a safe place...
	      ok = 0;
	      for (i = 1; i <= ds_global_juni.velX; i++) {
	         if (!ds_map_collMovBasic(newx,oldy,17 - i)) {
	            ok = 1;
	            newx = newx - i;
	            break;
	         }   
	      } 
			if (!ok) {
				diagonals++;  
				newx = oldx;
			}			
	      if ( (ok) && (_ds_juni_canClimb()) && 
					(ds_map_collClimbBasic(newx + 1,oldy,17)) && 
					(!_ds_juni_checkNoClimb(newx + 1,oldy,17))) {
	         // But... maybe juni should start Climbing?
		      if ((ds_global_juni.state == DS_C_JUNI_ST_JUMP_R) ||
				    (ds_global_juni.state == DS_C_JUNI_ST_FALL_R)) {
				       // Climb time!
					   newstate = DS_C_JUNI_ST_CLIMB_R;
					   _ds_juni_change(newstate,1);
					   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_STOP;
					   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_SLIDESOFT;									          
				}   
	      }
	   }   
	} else 
	if (ds_global_juni.velX < 0) {
	   if (ds_map_collMovBasic(newx,newy,6)) { // <TODO> 6 vs 5
	      // Collision! - Let's move Juni to a safe place...
	      ok = 0;
	      for (i = 1; i <= abs(ds_global_juni.velX); i++) {
	         if (!ds_map_collMovBasic(newx,oldy,6 + i)) { // <TODO> 6 vs 5
	            ok = 1;
	            newx = newx + i;
	            break;
	         }   
	      }   
			if (!ok) {
				diagonals++;  
				newx = oldx;
			}			
	      if ((ok) && (_ds_juni_canClimb()) && 
					(ds_map_collClimbBasic(newx - 1,oldy,6)) &&
					(!_ds_juni_checkNoClimb(newx-1, oldy,6)) ) { // <TODO> 6 vs 5
	         // But... maybe juni should start Climbing? :-) <TODO> Flying
		      if ((ds_global_juni.state == DS_C_JUNI_ST_JUMP_L) || 
					 (ds_global_juni.state == DS_C_JUNI_ST_FALL_L)) {
				       // Climb time!
					   newstate = DS_C_JUNI_ST_CLIMB_L;
					   _ds_juni_change(newstate,1);
					   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_STOP;
					   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_SLIDESOFT;									          
				}   
	      }
	   }
	}

	//PA_OutputText(1,1,8,"(%d,%d)(%d,%d) - (%d,%d)[%d]       ",newx,newy,oldx,oldy,ds_global_juni.velX,ds_global_juni.velY,ds_global_juni.inDblJump);
	//PA_OutputText(1,1,9,"(%d,%d) - (%d,%d)[%d]          ",ds_global_juni.actualpix,ds_global_juni.framepix,ds_global_juni.movstateY,ds_global_juni.movstateX,ds_global_juni.state);

	
   // Clash with environment, Vertical Speed. V 23. Also, no place to climb
	if (ds_global_juni.velY > 0) {
	   if (ds_map_collDownBasic(newx,newy,23)) {
	      // Collision! - Let's move Juni to a safe place...
	      ok = 0;
	      for (i = 1; i <= ds_global_juni.velY; i++) {
	         if (!ds_map_collDownBasic(newx,newy,23 - i)) {
	            ok = 1;
	            newy = newy - i;
	            break;
	         }   
	      }   
			if (!ok) {
				diagonals++;  
				newy = oldy;
			}			
	      if (ok == 1) {
	         // If Juni was falling... OK, now she must walk :-)
		      if ((ds_global_juni.state == DS_C_JUNI_ST_FALL_L) || 
				    (ds_global_juni.state == DS_C_JUNI_ST_FALL_R)) {
				   newstate = (ds_global_juni.state == DS_C_JUNI_ST_FALL_L)?DS_C_JUNI_ST_WALK_L:DS_C_JUNI_ST_WALK_R;
				   _ds_juni_change(newstate,1);
				   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_STOP;
				   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_WALK;									   
				} else
	         // What if Juni was climbing?... OK, now she must walk too :-)
		      if ((ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L) || 
				    (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_R)) {
				   newstate = (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L)?DS_C_JUNI_ST_WALK_L:DS_C_JUNI_ST_WALK_R;
				   _ds_juni_change(newstate,1);
				   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_STOP;
				   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_WALK;									   
				} 				
	      }   
	   }   
	} else
   // Clash with environment, Vertical Speed. ^ 7 (jump) ^5 (climb).
	if (ds_global_juni.velY < 0) {
      if ((ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_R)) {
		   // Climbing! Special Collision state and effects (stay climbing)
		   if (ds_map_collUpBasic(newx,newy,5)) {
		      // Collision! - Let's move Juni to a safe place...
		      ok = 0;
		      for (i = 1; i <= abs(ds_global_juni.velY); i++) {
		         if (!ds_map_collUpBasic(newx,newy,5 + i)) {
		            ok = 1;
		            newy = newy + i;
		            break;
		         }   
		      }   	
				if (!ok) {
					diagonals++;  
					newy = oldy;
				}			
			}				   
		} else {
		   // Normal! (Jumping/Flying) Special collision state and effects (fall)
		   if (ds_map_collUpBasic(newx,newy,7)) {
		      // Collision! - Let's move Juni to a safe place...
		      ok = 0;
		      for (i = 1; i <= abs(ds_global_juni.velY); i++) {
		         if (!ds_map_collUpBasic(newx,newy,7 + i)) {
		            ok = 1;
		            newy = newy + i;
		            break;
		         }   
		      }   
				if (!ok) {
					diagonals++;  
					newy = oldy;
				}			
		      if (ok == 1) {
		         // If Juni was jumping... now she shall lose some jumping power ;-)
			      if ((ds_global_juni.state == DS_C_JUNI_ST_JUMP_L) || 
					    (ds_global_juni.state == DS_C_JUNI_ST_JUMP_R)) {
					   if (ds_global_juni.actualpix < 8 + 14 + 7)
					   	ds_global_juni.actualpix = 8 + 14 + 7;
					   	ds_global_juni.framepix = 8 + 14 + 7;
					}      
		      }   
		   }   
		}   
	} 
	
	// DIAGONAL WARNING!!!!!!!!!! <FIXME>

	//PA_OutputText(1,1,10,"(%d,%d)(%d,%d) - (%d,%d)[%d]       ",newx,newy,oldx,oldy,ds_global_juni.velX,ds_global_juni.velY,ds_global_juni.inDblJump);
	//PA_OutputText(1,1,11,"(%d,%d) - (%d,%d)[%d]          ",ds_global_juni.actualpix,ds_global_juni.framepix,ds_global_juni.movstateY,ds_global_juni.movstateX,ds_global_juni.state);

	
	// Special clash - should I keep on climbing?
	if ((ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L) || 
		 (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_R)) {
	      // Can we still climb down/up?
	      direction = (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L)?-1:1;
	      if (!ds_map_collClimbBasic(newx + direction,newy,(direction==-1)?6:17)) { // Note: 4:19 to support non-perfect walls
         	// Ups! Either falls, or goes up
         	if (ds_global_juni.velY > 0) {
         	   // Falls
				   newstate = (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L)?DS_C_JUNI_ST_FALL_L:DS_C_JUNI_ST_FALL_R;
				   _ds_juni_change(newstate,1);
				   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FALL;				   
         	} else {
         	   // Goes up
					newstate = (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L)?DS_C_JUNI_ST_STOP_L:DS_C_JUNI_ST_STOP_R;
				   _ds_juni_change(newstate,1);
				   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_STOP; 
				   ds_global_juni.movstateX = DS_C_JUNI_MOVST_X_STOP; 
				   // HACK TIME!!!!!!!!!!!
				   newx += 4 * direction;
				   newy -= 2;
				   ds_global_juni.velX = direction;  // dirty HACK :-P - Allows check of inverse gravity
				}      
         }   
         // Yeah, but... what about NOCLIMB tiles?
	      else if (_ds_juni_checkNoClimb(newx + direction,newy,(direction==-1)?6:17) ) { 
         	   // Falls. Always Falls.
				   newstate = (ds_global_juni.state == DS_C_JUNI_ST_CLIMB_L)?DS_C_JUNI_ST_FALL_L:DS_C_JUNI_ST_FALL_R;
				   _ds_juni_change(newstate,1);
				   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FALL;				   	         
			}            
	}   

	// Special Clash - Spring! (also, by putting this check here, we do not jump in springs that are in the ground
	if (ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_SPRING)) {
		if ((ds_global_juni.state == DS_C_JUNI_ST_FALL_L) || 
			 (ds_global_juni.state == DS_C_JUNI_ST_FALL_R)) {
			if (ds_map_collBankObj(newx + (24 >> 1), newy + (24), 16, 1)) { // SPRING OBJECT - <TODO>: Limit to the upper part of the spring
				newstate = (ds_global_juni.state == DS_C_JUNI_ST_FALL_L)?DS_C_JUNI_ST_JUMP_L:DS_C_JUNI_ST_JUMP_R;
				_ds_juni_change(newstate,1);
				ds_global_juni.inDblJump = 1; // After state change. This means: I "may" DblJump later
				ds_global_juni.inNoBtnJump = 1; // Special - no need to maintain the button pushed in this one :-D
				ds_global_juni.movstateY = (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP))?
															DS_C_JUNI_MOVST_Y_JUMP:DS_C_JUNI_MOVST_Y_JUMPSOFT;		   
				newy = oldy; // cancel falling movement
			   if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_TJUMP)) {   
			      ds_global_juni.inNoBtnJump = 0; // OK, this jump, can be modulated...
			      if (ds_global_juni.movstateY == DS_C_JUNI_MOVST_Y_JUMP) {
			         // Advanced long jump
			         ds_global_juni.actualpix = -14; // -4:-3 * 7 = -49 (24 * 3) 
			      } else if (ds_global_juni.movstateY == DS_C_JUNI_MOVST_Y_JUMPSOFT) {
			         // Advanced short jump		         
			         ds_global_juni.actualpix = -12; // -2 * 12 = -24 (-24 * 1)
					}      
				}			
			}   	    
		}
	}	
	
	//PA_OutputText(1,1,12,"(%d,%d)(%d,%d) - (%d,%d)[%d]       ",newx,newy,oldx,oldy,ds_global_juni.velX,ds_global_juni.velY,ds_global_juni.inDblJump);
	//PA_OutputText(1,1,13,"(%d,%d) - (%d,%d)[%d]          ",ds_global_juni.actualpix,ds_global_juni.framepix,ds_global_juni.movstateY,ds_global_juni.movstateX,ds_global_juni.state);

	
	// 2c) Gravity
	//------------
	
   // Normal Gravity
   if (!ds_map_collDownBasic(newx,newy,24)) {
      // I am falling!!!... let's see what this means...

      // State "Normal" -> Falling time!!!!
      if ((ds_global_juni.state == DS_C_JUNI_ST_STOP_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_STOP_R)) {
		   newstate = (ds_global_juni.state == DS_C_JUNI_ST_STOP_L)?DS_C_JUNI_ST_FALL_L:DS_C_JUNI_ST_FALL_R;
		   _ds_juni_change(newstate,1);
			ds_global_juni.inDblJump = 1; // After state change. This means: I "may" DblJump later		   
		   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FALL;		   
		   
		} else
      // State "Walk" -> Falling time (bis)!!!!
      if ((ds_global_juni.state == DS_C_JUNI_ST_WALK_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_WALK_R)) {
		   newstate = (ds_global_juni.state == DS_C_JUNI_ST_WALK_L)?DS_C_JUNI_ST_FALL_L:DS_C_JUNI_ST_FALL_R;
		   _ds_juni_change(newstate,1);
			ds_global_juni.inDblJump = 1; // After state change. This means: I "may" DblJump later
		   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FALL;
		} else 
      // State "Run" -> Falling time (tris)!!!!
      if ((ds_global_juni.state == DS_C_JUNI_ST_RUN_L) || 
		    (ds_global_juni.state == DS_C_JUNI_ST_RUN_R)) {
		   newstate = (ds_global_juni.state == DS_C_JUNI_ST_RUN_L)?DS_C_JUNI_ST_FALL_L:DS_C_JUNI_ST_FALL_R;
		   _ds_juni_change(newstate,1);
			ds_global_juni.inDblJump = 1; // After state change. This means: I "may" DblJump later
		   ds_global_juni.movstateY = DS_C_JUNI_MOVST_Y_FALL;
		}		
   }
   
	//PA_OutputText(1,1,14,"(%d,%d)(%d,%d) - (%d,%d)[%d]       ",newx,newy,oldx,oldy,ds_global_juni.velX,ds_global_juni.velY,ds_global_juni.inDblJump);
	//PA_OutputText(1,1,15,"(%d,%d) - (%d,%d)[%d]          ",ds_global_juni.actualpix,ds_global_juni.framepix,ds_global_juni.movstateY,ds_global_juni.movstateX,ds_global_juni.state);

   
	// 2d) Inverse Gravity
	//--------------------
	   
   // Inverse Gravity - Let Juni jump stairs (or layer-3 grass :-P)
	if (ds_global_juni.velX != 0) {
	   if (ds_map_collDownBasic(newx,newy,23)) {
	      ok = 0;
	      for (i = 1; i <= 3; i++) {
	         if (!ds_map_collDownBasic(newx,newy,23 - i)) {
	            ok = 1;
	            newy = newy - i;
	            break;
	         }   
	      }   
	      if (!ok) {
	         // Wallswim! :-) - Go up
	         newy = oldy - 1;
	      }   
	   }
	}		   
      
   // 3) Manages Juni's animation
   //-----------------------------
   
   switch (ds_global_juni.state) {
		case DS_C_JUNI_ST_STOP_R:
		case DS_C_JUNI_ST_STOP_L:
		   	ds_3dsprite_setFrame(ds_global_juni.sprite,0);  // <TODO - Only once!>
		   	ds_global_juni.actualpix++;
		   	ds_global_juni.framepix++;
		   	break;
		case DS_C_JUNI_ST_WALK_R:
		case DS_C_JUNI_ST_WALK_L:
			   ds_3dsprite_setFrame(ds_global_juni.sprite,
						ds_global_juni.framepix % ds_3dsprite_getMaxFrame(ds_global_juni.sprite));
				ds_global_juni.actualpix++;
		   	if ((ds_global_juni.actualpix % 2) == 0)
		   		ds_global_juni.framepix++;						
		   	break;
		case DS_C_JUNI_ST_RUN_R:
		case DS_C_JUNI_ST_RUN_L:
				ds_3dsprite_setFrame(ds_global_juni.sprite,
						ds_global_juni.framepix % ds_3dsprite_getMaxFrame(ds_global_juni.sprite));		   
				ds_global_juni.actualpix++;
		   	if ((ds_global_juni.actualpix % 2) == 0)
		   		ds_global_juni.framepix++;				
		   	break;
		case DS_C_JUNI_ST_CLIMB_R:
		case DS_C_JUNI_ST_CLIMB_L:
		   	if (ds_global_juni.movstateY == DS_C_JUNI_MOVST_Y_CLIMB) {
					ds_3dsprite_setFrame(ds_global_juni.sprite,
							ds_global_juni.framepix % ds_3dsprite_getMaxFrame(ds_global_juni.sprite));		   
					ds_global_juni.actualpix++;
			   	if ((ds_global_juni.actualpix % 2) == 0)
			   		ds_global_juni.framepix++;				
		  		} else {
					ds_3dsprite_setFrame(ds_global_juni.sprite,ds_3dsprite_getMaxFrame(ds_global_juni.sprite) - 1); // <TODO - Only once!>
					ds_global_juni.actualpix++;
					ds_global_juni.framepix++;	
		  		}   
		   	break;
		case DS_C_JUNI_ST_FALL_R:
		case DS_C_JUNI_ST_FALL_L:
		   	if (ds_global_juni.framepix < 12)
		   		ds_3dsprite_setFrame(ds_global_juni.sprite,ds_global_juni.framepix >> 1);
		   	else {
		   	   // <TODO> 66-77-66-77-...
		   		ds_3dsprite_setFrame(ds_global_juni.sprite,
							((ds_global_juni.framepix % 2) == 0)?6:7);		   
		 		}  	
		   	ds_global_juni.actualpix++;
		   	ds_global_juni.framepix++;
		   	break;
		case DS_C_JUNI_ST_FLY_R:
		case DS_C_JUNI_ST_FLY_L:
		   		ds_3dsprite_setFrame(ds_global_juni.sprite,((ds_global_juni.framepix % 2) == 0)?0:1);
		   	ds_global_juni.actualpix++;
		   	ds_global_juni.framepix++;
		   	break;		   	
		case DS_C_JUNI_ST_JUMP_R:
		case DS_C_JUNI_ST_JUMP_L:
		   	if (ds_global_juni.framepix < 8)
		   		ds_3dsprite_setFrame(ds_global_juni.sprite,ds_global_juni.framepix >> 1);
		   	else
		   		ds_3dsprite_setFrame(ds_global_juni.sprite,4);
		   	ds_global_juni.actualpix++;
		   	ds_global_juni.framepix++;
		   	break;
   }   

   // (Z) Updates Juni's position, rewrites sprite
   //---------------------------------------------
   ds_juni_updateSprites(newx,newy);
}  

/* Manage the change of maps... travel like Juni, my friend -:) */
void _ds_juni_manageBoundaries() {
   int boundary = -1;
   int junix = ds_3dsprite_getX(ds_global_juni.sprite);
   int juniy = ds_3dsprite_getY(ds_global_juni.sprite);
   int mapx = ds_global_map.x;
   int mapy = ds_global_map.y;
   
   // Check the existence of a boundary
   if (junix > (600 - (24 >> 1)))
   	boundary = 1; // EAST
   else
   if (junix < (0 - (24 >> 1)))
   	boundary = 3; // WEST
	else
	if (juniy > (240 - (24 >> 1)))
		boundary = 2; // SOUTH
	else
	if (juniy < (0 - (24 >> 1)))
		boundary = 0; // NORTH
		
	// Calculates the X/Y where Juni must go now...
	ds_map_getBoundary(boundary, &mapx, &mapy);
   
   // If Juni moved to another map, it is time to make some state changes...
   switch (boundary) {
      case -1:
         return;
         break; // I know, I do not need this... :-P
      case 0: // NORTH
      	juniy = juniy + 240 - 2; // Small correction factor...
      	break;
      case 1: // EAST
      	junix = junix - 600 + 2; // Small correction factor...
      	break;
      case 2: // SOUTH
      	juniy = juniy - 240 + 2; // Small correction factor...
      	break;
      case 3: // WEST
      	junix = junix + 600 - 2; // Small correction factor...
      	break;
    }
 	 
	 // If I am here... time to change the map!!!!!  	
	 ds_state_assignState(DSKNYTT_GAME);
	 ds_state_var_setInt(0,mapx);
	 ds_state_var_setInt(1,mapy);
	 ds_state_var_setInt(2,junix);
	 ds_state_var_setInt(3,juniy);
	 ds_state_var_setInt(4,0);
}   

/* Checks if a Juni-rectangle hits with a certain object. Check is per-pixel */
int _ds_juni_checkCollSprite(int rectIniX, int rectIniY, int rectSX, int rectSY,
										u16 *ima, int imaIniX, int imaIniY, int imaSX, int imaSY) {
	// First, limits the area where we are going to make the tests
	int iniX, endX;
	iniX = (rectIniX < imaIniX)?imaIniX:rectIniX;
	endX = ((rectIniX + rectSX) < (imaIniX + imaSX))?(rectIniX + rectSX):(imaIniX + imaSX);
	int iniY, endY;
	iniY = (rectIniY < imaIniY)?imaIniY:rectIniY;
	endY = ((rectIniY + rectSY) < (imaIniY + imaSY))?(rectIniY + rectSY):(imaIniY + imaSY);
	
	// Now, make the test
	int i,j;
	for (j = iniY; j < endY; j++) {
		for (i = iniX; i < endX; i++) {
		   int val = (i - imaIniX) + ((j - imaIniY) * imaSX);
		   if (ima[val] != 0)
		   	return 1;
		}   
	}   
	
	return 0;
}   

/* Interacts with the objects of the world... Be friendly as Juni, my friend -:) */
void _ds_juni_manageObjectColl() {
   int i, j, layer;
   int junix = ds_3dsprite_getX(ds_global_juni.sprite);
	int juniy = ds_3dsprite_getY(ds_global_juni.sprite);
   int junicx = 0;
	int junicy = 0;
	int obj;
	int bank;
	int arrX[4];
	int arrY[4];
	int arrMax;
	int arrBefore;
	int noShiftDel = 1;
	
	// Inits internal variables
	arrMax = 0;
   
   // We need to test the 4 pixels that surround the image of Juni (8x16 inside a 24x24)
   for (i = 0; i < 4; i++) {
      switch (i) {
         case 0: 
         	if (_ds_juni_faceRight()) {
         	   // U-R
         	   junicx = (junix + 16) / 24;
         	   junicy = (juniy + 8) / 24;
         	} else {
         	   // U-L
         	   junicx = (junix + 8) / 24;
         	   junicy = (juniy + 8) / 24;
				}      
            break;
         case 1:
         	if (_ds_juni_faceRight()) {
         	   // D-R
         	   junicx = (junix + 16) / 24;
         	   junicy = (juniy + 23) / 24;
         	} else {
         	   // D-L
         	   junicx = (junix + 8) / 24;
         	   junicy = (juniy + 23) / 24;
				}      
            break;
         case 2:
         	if (_ds_juni_faceRight()) {
         	   // U-L
         	   junicx = (junix + 8) / 24;
         	   junicy = (juniy + 8) / 24;
         	} else {
         	   // U-R
         	   junicx = (junix + 16) / 24;
         	   junicy = (juniy + 8) / 24;
				}      
            break;
         case 3:
         	if (_ds_juni_faceRight()) {
         	   // D-L
         	   junicx = (junix + 8) / 24;
         	   junicy = (juniy + 23) / 24;
         	} else {
         	   // D-R
         	   junicx = (junix + 16) / 24;
         	   junicy = (juniy + 23) / 24;
				}      
            break;            
      }   
      // We analyzed this coordinate before?
      arrBefore = 0;
      for (j = 0; j < arrMax; j++) {
         if ((arrX[j] == junicx) && (arrY[j] == junicy)) {
         	arrBefore = 1;
         	//break;
       	}  	
      }   
      
      // Testing time!!!!
      if (!arrBefore) {
         // Write that we saw this before
         arrX[arrMax] = junicx;
         arrY[arrMax] = junicy;
         arrMax++;
         // Test every object layer...
	      for (layer = 0; layer < 4; layer++) {
		   	obj = ds_global_map.room.objlayer[layer].obj[junicy][junicx];
   			bank = ds_global_map.room.objlayer[layer].bank[junicy][junicx];
	   		if (obj != 0) {
					if (_ds_juni_checkCollSprite(junix + 8, juniy + 8, // Juni's pos
							8, 16, // Juni's size
							ds_3dspritehdd_getSprite(bank,obj,0), 
							junicx * 24,junicy * 24, 
							ds_3dspritehdd_getWidth(bank,obj),ds_3dspritehdd_getHeight(bank,obj))){
		   		   // Manages the collision
		   		   ds_objects_collide(layer, junicx, junicy);
		   		   // Special: if teleport, go away NOW!
		   		   if (ds_global_world.shift)
		   		   	break;
	   			}   
   			}   
 			}
		   // Special^2: if teleport, go away NOW!
		   if (ds_global_world.shift)
		   	break;
			// Mark special "no shift" management  	
			if ((ds_global_world.shiftNoX == junicx) && (ds_global_world.shiftNoY == junicy)) {
				noShiftDel = 0;
			}			
		} 		
   }
	// Special - manages the "No shift" condition   
	if (!ds_global_world.shift) {
		if (noShiftDel) {
	      ds_global_world.shiftNoX = -1;
		   ds_global_world.shiftNoY = -1;
		}   
	}		
}

/* Checks if Juni is damaged by an external entity... Be healthy as Juni, my friend -:) */
void _ds_juni_manageObjectDam() {
   int dam;
   dam = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(ds_global_juni.sprite, ds_3dsprite_getFrame(ds_global_juni.sprite)),
								 ds_3dsprite_getXSize(ds_global_juni.sprite),
								 ds_3dsprite_getYSize(ds_global_juni.sprite),
								 ds_3dsprite_getX(ds_global_juni.sprite),
								 ds_3dsprite_getY(ds_global_juni.sprite),
								 &ds_global_map.tileMapDamTemp);
	if ((dam) || (ds_global_juni.killme)) {
	   // Juni? Juniiii? Juniiiiiiiiiiiiiiiiiiiiiiiiiii!!!!!!!!!!!!!!!!!!! <- Otacon :-P
	   ds_global_juni.killme = 0; // OK. I am already dead...
	   // First, "goodbye" to the sprite of Juni and all its helpers
	   ds_3dsprite_markInvisible(ds_global_juni.sprite,1); // Juni, the master of hiding! ;-)
	   ds_3dsprite_markInvisible(ds_global_juni.redglow_sprite,1); 
	   ds_3dsprite_markInvisible(ds_global_juni.cyanglow_sprite,1);
	   ds_3dsprite_markInvisible(ds_global_juni.umb_sprite,1); 
	   ds_3dsprite_markInvisible(ds_global_juni.holo_sprite,1);
		// Second, "hi" to the "dead smoke"
		ds_t_object *particle;
	   int x = ds_global_juni.x + (24 >> 1);
	   int y = ds_global_juni.y + (24 >> 1);
	   int i;
	   for (i = 0; i < 10; i++) {
		   particle = ds_objects_createParticle(x - 8, y - 4, 0, 17); // Smoke!!!!
		   ds_3dsprite_setPrio(particle->sprite, (DS_C_PRIO * DS_C_PRIOLJUNISP));
		   ds_3dsprite_setFrame(particle->sprite, 
						PA_RandMinMax(0, ds_3dsprite_getMaxFrame(particle->sprite)-1));
		}   
	   // New Juni_is_dead state
	   ds_global_juni.state = DS_C_JUNI_ST_DEAD;
	   // Countdown!
	   ds_global_juni.actualpix = 60 * 3; // 3 seconds
	}
}

/* Manages "the glow" and others... Be bright and cautious like Juni, my friend -:) */
void _ds_juni_manageSpecials() {
   
   // GLOW
   //-----
   
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_ENEMY)) {
   	// First, Red Glow
      if (ds_global_juni.redGlow < (24*3)) {
      	ds_3dsprite_setAlpha(ds_global_juni.redglow_sprite,80);
      	ds_3dsprite_markInvisible(ds_global_juni.redglow_sprite,0);
      } else
      if (ds_global_juni.redGlow < (24*6)) {
      	ds_3dsprite_setAlpha(ds_global_juni.redglow_sprite,40);
      	ds_3dsprite_markInvisible(ds_global_juni.redglow_sprite,0);
      } else {
      	ds_3dsprite_setAlpha(ds_global_juni.redglow_sprite,0);
      	ds_3dsprite_markInvisible(ds_global_juni.redglow_sprite,1);
    	}  	
      // Second, Cyan glow <TODO> FIX Invisible! Too costly!!!!!
	   int mycyan = _ds_juni_checkCyanGlow();
	   if (mycyan == 1) {
	   	ds_3dsprite_setAlpha(ds_global_juni.cyanglow_sprite,60);
	   	ds_3dsprite_markInvisible(ds_global_juni.cyanglow_sprite,0);
	   } else if (mycyan == 2) {
	   	ds_3dsprite_setAlpha(ds_global_juni.cyanglow_sprite,120);
	   	ds_3dsprite_markInvisible(ds_global_juni.cyanglow_sprite,0);
	   } else {
	   	ds_3dsprite_setAlpha(ds_global_juni.cyanglow_sprite,0);
	   	ds_3dsprite_markInvisible(ds_global_juni.cyanglow_sprite,1);
	 	}  		
   } else {
      ds_3dsprite_setAlpha(ds_global_juni.redglow_sprite,0);
      ds_3dsprite_markInvisible(ds_global_juni.redglow_sprite,1);
      ds_3dsprite_setAlpha(ds_global_juni.cyanglow_sprite,0);
      ds_3dsprite_markInvisible(ds_global_juni.cyanglow_sprite,1);
	}      
	
	// HOLO-SPRITE
	//------------
	if (_ds_juni_hologramOn())
		ds_3dsprite_setAlpha(ds_global_juni.holo_sprite,128);
	else {
	   ds_3dsprite_setAlpha(ds_global_juni.holo_sprite,255);
	}	
	
	// UMBRELLA <TODO> Keep umbrella on "reset" teleport
	//---------
	if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_UMBRELLA)) {	
		int junidir = _ds_juni_faceRight();
		int iniumb = ds_3dsprite_getIniFrame_LR(ds_global_juni.umb_sprite, junidir,1);
		int endumb = ds_3dsprite_getMaxFrame_LR(ds_global_juni.umb_sprite, junidir,1);
		// Correct movement
		if (_ds_juni_faceRight() != ds_global_juni.umb_dir) {
		   // Wrong direction! Correct sprite ;-)
		   if (_ds_juni_faceRight()) { // From 3..6 to 0..3 
		      ds_global_juni.umb_state -= ds_3dsprite_getMaxFrame_LR(ds_global_juni.umb_sprite, 1,1);
		      if (ds_global_juni.umb_state == endumb)
		      	ds_global_juni.umb_state--;
				ds_3dsprite_setFrame(ds_global_juni.umb_sprite,ds_global_juni.umb_state);	   
//				ds_3dsprite_updateSprite(ds_global_juni.umb_sprite);	      
		   } else { // From 0..3 to 3..6
		   	ds_global_juni.umb_state += ds_3dsprite_getMaxFrame_LR(ds_global_juni.umb_sprite, 1,1);
		      if (ds_global_juni.umb_state == endumb)
		      	ds_global_juni.umb_state--;	   	
				ds_3dsprite_setFrame(ds_global_juni.umb_sprite,ds_global_juni.umb_state);	   
//				ds_3dsprite_updateSprite(ds_global_juni.umb_sprite);	      	   	
		 	}  	
		}   
		// Manage umbrella
		ds_global_juni.umb_dir = _ds_juni_faceRight();
	   if ((ds_global_juni.umbrellaOn == 0) && (ds_global_juni.umb_state == iniumb)) {
	      // Nothing to do... OK, maybe put the sprite invisible ;-)
	      ds_3dsprite_markInvisible(ds_global_juni.umb_sprite,1);
	   } else
		if ((ds_global_juni.umbrellaOn == 1) && (ds_global_juni.umb_state == endumb)) {
		   // Nothing to do...
		} else	
		if ((ds_global_juni.umbrellaOn == 0) && (ds_global_juni.umb_state > iniumb)) {
		   // Close the umbrella!!!!
		   if (ds_global_tick % 2 == 0) { // Every 5 "ticks"
				ds_3dsprite_setFrame(ds_global_juni.umb_sprite,ds_global_juni.umb_state);	   
				ds_global_juni.umb_state--;
//				ds_3dsprite_updateSprite(ds_global_juni.umb_sprite);
			}		
		} else
		if ((ds_global_juni.umbrellaOn == 1) && (ds_global_juni.umb_state < endumb)) {
		   // Open the umbrella!!!!
		   if (ds_global_tick % 2 == 0) { // Every 5 "ticks"
				ds_3dsprite_setFrame(ds_global_juni.umb_sprite,ds_global_juni.umb_state);	   
				ds_global_juni.umb_state++;
//				ds_3dsprite_updateSprite(ds_global_juni.umb_sprite);
			}		
		}
	}	
	
	// MENU OPTIONS (Juni-Related)
	//----------------------------
   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_START)) {
      ds_gamestatus_changeMode();
      #ifdef DEBUG_KSDS
      	// <DEBUG> Obtain ALL powerups!
		   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_UMBRELLA);
		   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_CLIMB);
		   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_JUMP);
		   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_RUN);
		   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_DBLJUMP);
		   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_EYE);
		   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_HOLOGRAM);
		   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_ENEMY);
	   #endif
   }
	
	// JUNI-SPRITES
	//-------------
	if (_ds_juni_varDbl) {
		ds_t_object *particle;
	   int x = ds_global_juni.x;
	   int y = ds_global_juni.y + (24 >> 1);
	   particle = ds_objects_createParticle(x - 8, y, 0, 18); // Jump Smoke!!!!
	   ds_3dsprite_setPrio(particle->sprite, (DS_C_PRIO * DS_C_PRIOLJUNISP));
	   ds_3dsprite_setFrame(particle->sprite,0);
	}    
}   

/* Manages special system keys (e.g. exit a story)... Always have an exit prepared like Juni, my friend -:) */
void _ds_juni_manageSpecialsSystem() {
	// MENU OPTIONS (System-Related)
	//------------------------------
   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_SELECT)) {
      // Start leaving this story
		ds_global_fadeWhite_Ini();      
		
   } else if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_SELECT)) {
      // Try to leave the story
      if (ds_global_tick % 10 == 0) {
         if (!ds_global_fadeWhite_Continue())
         	ds_state_assignState(DSKNYTT_DESTROYGAME);
      }
   } 
   if (ds_util_bitOne16(ds_global_input.Released,DS_C_IN_SELECT)) {
      // Continue the story
		ds_global_fadeWhite_Finish();      
		
   } 
}   


/* Manages the "dead and resurrection" of Juni... Achieve the nirvana like Juni, my friend -:) */
void _ds_juni_manageDead() {
   if (!ds_global_fadeWhite_CHECK()) { // Only of we are not fading (e.g. exiting game due to user input)
	   ds_global_juni.actualpix--;
	   if (ds_global_juni.actualpix <= 0) {
	      ds_state_assignState(DSKNYTT_LOADGAME);
	      ds_state_var_setInt(0,DS_C_LOADGAME_GAME);
	      ds_state_var_setInt(1,0); // No reset!!!
	      ds_state_var_setInt(2,ds_global_world.saveslot); // Save Slot
	   }   
	}   
}   

/* Manages Juni... Be Juni, my friend -:) */
void ds_juni_manage() {
   _ds_juni_varDbl = 0;
   
   // First, "dead" management
   if (ds_global_juni.state == DS_C_JUNI_ST_DEAD) {
   	_ds_juni_manageDead();
   	_ds_juni_manageSpecialsSystem();
   }   
   // Normal management!
   else {
   	_ds_juni_manageMovement();
	   _ds_juni_manageBoundaries();
	   _ds_juni_manageObjectColl();
	   _ds_juni_manageSpecials();
	   _ds_juni_manageSpecialsSystem();
	   _ds_juni_manageObjectDam();
	} 
}

/* Get Juni X coordinate... including hologram coordinates :-P */
int ds_juni_getXVirtual() {
   if (_ds_juni_hologramOn())
   	return ds_3dsprite_getX(ds_global_juni.holo_sprite);
   else
	   return ds_global_juni.x; // <TODO>
}   

/* Get Juni Y coordinate... including hologram coordinates :-P */
int ds_juni_getYVirtual() {
   if (_ds_juni_hologramOn())
   	return ds_3dsprite_getY(ds_global_juni.holo_sprite);
   else
	   return ds_global_juni.y; // <TODO>
}   