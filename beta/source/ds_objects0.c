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
#include "ds_objects_lib.h"
#include "ds_objects.h"
#include "ds_util.h"
#include "ds_ini.h"
#include "ds_world.h"
#include "ds_state.h"
#include "ds_gamestatus.h"
#include "ds_3dsprite.h"
#include "ds_music.h"


// BANK 0 [B00]
//=================================================================================================

//>>>>>>>>>Special - Cycle management
//..........................................................................................
int _ds_objects_b00cycle_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Cycle through image
   ds_objects_lib_beh_cycle(object,5);
   
   // Everything went OK...
   return 1;
}


int _ds_objects_b00cycleSAV_manage(void *objp) {
   ds_t_object *object = objp;
   int waitTime = 2;
   
   // Cycle through image
   switch (object->inner[0]) {
      case 0:
         // INIT
         object->inner[1] = waitTime; // Waiting time
         object->inner[3] = 1; // Actual movement     
         object->inner[2] = (object->inner[3] - 1) * 3; // Frame             
         object->inner[0] = 1; // Next state
         break;
      case 1:
         // NORMAL
         object->inner[1]--;
         if (object->inner[1] <= 0) {
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);
         	object->inner[1] = waitTime; // Waiting time
         	object->inner[2]++;         	
         	if (object->inner[2] == (object->inner[3] * 3))
					object->inner[2] = (object->inner[3] - 1) * 3;      
         }   
			break;
      case 2:
         // INIT SAVE
         object->inner[1] = waitTime; // Waiting time
         object->inner[3] = 2; // Actual movement     
         object->inner[2] = (object->inner[3] - 1) * 3; // Frame             
         object->inner[0] = 3; // Next state
         break;
		case 3:
		   // SAVED
		   // Cycle
         object->inner[1]--;
         if (object->inner[1] <= 0) {
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);
         	object->inner[1] = waitTime; // Waiting time
         	object->inner[2]++;         	
         	if (object->inner[2] == (object->inner[3] * 3))
					object->inner[2] = (object->inner[3] - 1) * 3;      
         }
		   // Change
		   if ((ds_global_tick % 12) == 0) {
		   	object->inner[3]++;
		   	object->inner[2] = (object->inner[3] - 1) * 3;
		 	}  	
		   if (object->inner[3] == 7) {
		      object->inner[2] = 0;
		      object->inner[3] = 1;
		      object->inner[0] = 1;
		   }   	   	
		   break;
   }   
   
   int valA = (ds_global_tick % 64);
   ds_3dsprite_setAlpha(object->sprite,192 - ((valA < 32)?valA:(64 - valA)));
   
   // Everything went OK...
   return 1;
}

// SAVE POINT [B00_O01]
//..........................................................................................
int _ds_objects_b00o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_3dsprite_setAlpha(object->sprite,192);

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o01_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Save the game (only if pressing down)
   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_DOWN)) {
      // <TODO> Only in special states
   	ds_world_updateSavegameBasic(object->x,object->y);
	ds_music_playSound("Save Spot", 0, 0);
   	object->inner[0] = 2; // Change state	
	}   
   
   // Everything went OK...
   return 1;
}

// WIN [B00_O02]
//..........................................................................................
int _ds_objects_b00o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o02_execute(void *objp) {
   
   // Win the game!!!!!
   ds_state_assignState(DSKNYTT_CUTSCENE);   
   sprintf(ds_global_string,"%s",
				ds_ini_getstring(ds_global_world.worldini,
								ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_ENDING,0,0,0,0),
								"Ending")
				);
   ds_state_var_setStr(ds_global_string);
   ds_state_var_setInt(0,1); // Cutscene number 1
   ds_state_var_setInt(1,DS_C_CUTSCENE_END); // Type END   
   
   // Everything went OK...
   return 1;
}

// RUN POWERUP [B00_O03]
//..........................................................................................
int _ds_objects_b00o03_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;
   
   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_RUN))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o03_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_RUN);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// CLIMB POWERUP [B00_O04]
//..........................................................................................
int _ds_objects_b00o04_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;

   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_CLIMB))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o04_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_CLIMB);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// DOUBLE JUMP POWERUP [B00_O05]
//..........................................................................................
int _ds_objects_b00o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;
   
   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_DBLJUMP))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o05_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_DBLJUMP);
	object->_deleteme = 1;
	
	ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// JUMP POWERUP [B00_O06]
//..........................................................................................
int _ds_objects_b00o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;
   
   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_JUMP))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o06_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_JUMP);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// EYE POWERUP [B00_O07]
//..........................................................................................
int _ds_objects_b00o07_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;
   
   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_EYE))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o07_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_EYE);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// ENEMY DETECTOR POWERUP [B00_O08]
//..........................................................................................
int _ds_objects_b00o08_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;
   
   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_ENEMY))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o08_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_ENEMY);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// UMBRELLA POWERUP [B00_O09]
//..........................................................................................
int _ds_objects_b00o09_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;

   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_UMBRELLA))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o09_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_UMBRELLA);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// HOLOGRAM POWERUP [B00_O10]
//..........................................................................................
int _ds_objects_b00o10_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
   
   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_HOLOGRAM))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o10_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_HOLOGRAM);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// NO CLIMB [B00_O11]
//..........................................................................................
int _ds_objects_b00o11_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_NOCLIMB);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

// EYE SOUND [B00_O12]
//..........................................................................................
int _ds_objects_b00o12_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);		
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations - tell the system to make a sound - then kills itself
   object->type = DS_C_OBJ_EVENT;
   	// TODO: Sound
   object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}      

// STICKY [B00_O13]
//..........................................................................................
int _ds_objects_b00o13_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_STICKY);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

// SHIFT A [B00_O14]
//..........................................................................................
int _ds_objects_b00o14_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   int tmpini;
   
   // Special preview! Depending on the type of shift, I change the obj to another obj.
   // Why? Design failure. The engine does not support more than one sprite for a given
   // (bank, obj) pair.
   tmpini = ds_ini_getint(ds_global_world.worldini,
		   		ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_TYPE,
					DS_C_ELETYPE_A,0,0,0),
   				0);
   obj = 40 + tmpini;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

   tmpini = ds_ini_getboolean(ds_global_world.worldini,
		   		ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_VISIBLE,
					DS_C_ELETYPE_A,0,0,0),
   				1);
	if (!tmpini)		
		object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o14_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Executes the shifting mode
   ds_objects_lib_beh_shift(object, DS_C_ELETYPE_A);
   
   // Everything went OK...
   return 1;
}

// SHIFT B [B00_O15]
//..........................................................................................
int _ds_objects_b00o15_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   int tmpini;

   // Special preview! Depending on the type of shift, I change the obj to another obj.
   // Why? Design failure. The engine does not support more than one sprite for a given
   // (bank, obj) pair.
   tmpini = ds_ini_getint(ds_global_world.worldini,
		   		ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_TYPE,
					DS_C_ELETYPE_B,0,0,0),
   				0);
   obj = 40 + tmpini;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

   tmpini = ds_ini_getboolean(ds_global_world.worldini,
		   		ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_VISIBLE,
					DS_C_ELETYPE_B,0,0,0),
   				1);
	if (!tmpini)				
		object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o15_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Executes the shifting mode
   ds_objects_lib_beh_shift(object, DS_C_ELETYPE_B);
   
   // Everything went OK...
   return 1;
}

// SHIFT C [B00_O16]
//..........................................................................................
int _ds_objects_b00o16_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   int tmpini;

   // Special preview! Depending on the type of shift, I change the obj to another obj.
   // Why? Design failure. The engine does not support more than one sprite for a given
   // (bank, obj) pair.
   tmpini = ds_ini_getint(ds_global_world.worldini,
		   		ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_TYPE,
					DS_C_ELETYPE_C,0,0,0),
   				0);
   obj = 40 + tmpini;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

   tmpini = ds_ini_getboolean(ds_global_world.worldini,
		   		ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_SHIFT_VISIBLE,
					DS_C_ELETYPE_C,0,0,0),
   				1);
	if (!tmpini)				
		object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o16_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Executes the shifting mode
   ds_objects_lib_beh_shift(object, DS_C_ELETYPE_C);
   
   // Everything went OK...
   return 1;
}


// SIGN A [B00_O17]
//..........................................................................................
int _ds_objects_b00o17_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o17_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Write a dialogue...
   ds_objects_lib_beh_dialogue(object, DS_C_ELETYPE_A);
   
   // Everything went OK...
   return 1;
}

// SIGN B [B00_O18]
//..........................................................................................
int _ds_objects_b00o18_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o18_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Write a dialogue...
   ds_objects_lib_beh_dialogue(object, DS_C_ELETYPE_B);
   
   // Everything went OK...
   return 1;
}

// SIGN C [B00_O19]
//..........................................................................................
int _ds_objects_b00o19_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o19_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Write a dialogue...
   ds_objects_lib_beh_dialogue(object, DS_C_ELETYPE_C);
   
   // Everything went OK...
   return 1;
}

// WARPER [B00_O20]
//..........................................................................................
int _ds_objects_b00o20_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);		
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations - just change a map's flag - then kills itself
   object->type = DS_C_OBJ_EVENT;
   ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_WARP);
   object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

// RED KEY POWERUP [B00_O21]
//..........................................................................................
int _ds_objects_b00o21_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;
   
   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_REDKEY))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o21_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_REDKEY);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// YELLOW KEY POWERUP [B00_O22]
//..........................................................................................
int _ds_objects_b00o22_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;

   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_YELLOWKEY))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o22_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_YELLOWKEY);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// BLUE KEY POWERUP [B00_O23]
//..........................................................................................
int _ds_objects_b00o23_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;
   
   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_BLUEKEY))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o23_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_BLUEKEY);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// VIOLET KEY POWERUP [B00_O24]
//..........................................................................................
int _ds_objects_b00o24_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_ITEM;
   object->managed = 1;
   
   // ...self-deletion?
   if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_VIOLETKEY))
   	object->_deleteme = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b00o24_execute(void *objp) {
   ds_t_object *object = objp;
   
   // Get the item! - and delete it -:)
   ds_global_juni.item = ds_util_bitSet16(ds_global_juni.item,DS_C_JUNI_IT_VIOLETKEY);
   object->_deleteme = 1;
   
   ds_music_playSound("Powerup", 0, 0);
   
   // Everything went OK...
   return 1;
}

// NO JUMP [B00_O25]
//..........................................................................................
int _ds_objects_b00o25_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_NOJUMP);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

// RED GLOW [B00_O26]
//..........................................................................................
int _ds_objects_b00o26_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_NOJUMP);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

// CYAN GLOW - DARK [B00_O27]
//..........................................................................................
int _ds_objects_b00o27_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_CGLOWZ);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}      
      
// CYAN GLOW - LIGHT [B00_O28]
//..........................................................................................
int _ds_objects_b00o28_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_CGLOWA);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}      

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects0_assign(u8 obj, ds_t_object *object) {
/*
	<TODO>
		- Eye music (12)
			Why: ?????
*/
   switch (obj) {
      case 1:
         // Save Point
         object->fcreate = _ds_objects_b00o01_create;
         object->fmanage = _ds_objects_b00cycleSAV_manage;
         object->fexecute = _ds_objects_b00o01_execute;
         return 1;
         break; // not really necessary...
      case 2:
         // Win
         object->fcreate = _ds_objects_b00o02_create;
         object->fexecute = _ds_objects_b00o02_execute;
         return 1;
         break; // not really necessary...
      case 3:
         // Run Item
         object->fcreate = _ds_objects_b00o03_create;
         object->fmanage = _ds_objects_b00cycle_manage;
         object->fexecute = _ds_objects_b00o03_execute;
         return 1;
         break; // not really necessary...
      case 4:
         // Climb Item
         object->fcreate = _ds_objects_b00o04_create;
         object->fmanage = _ds_objects_b00cycle_manage;
         object->fexecute = _ds_objects_b00o04_execute;
         return 1;
         break; // not really necessary...
      case 5:
         // Double Jump Item
         object->fcreate = _ds_objects_b00o05_create;
         object->fmanage = _ds_objects_b00cycle_manage;
         object->fexecute = _ds_objects_b00o05_execute;
         return 1;
         break; // not really necessary...
      case 6:
         // Jump Item
         object->fcreate = _ds_objects_b00o06_create;
         object->fmanage = _ds_objects_b00cycle_manage;
         object->fexecute = _ds_objects_b00o06_execute;
         return 1;
         break; // not really necessary...
      case 7:
         // Eye Item
         object->fcreate = _ds_objects_b00o07_create;
         object->fmanage = _ds_objects_b00cycle_manage;
         object->fexecute = _ds_objects_b00o07_execute;
         return 1;
         break; // not really necessary...
      case 8:
         // Enemy Detector Item
         object->fcreate = _ds_objects_b00o08_create;
         object->fmanage = _ds_objects_b00cycle_manage;
         object->fexecute = _ds_objects_b00o08_execute;
         return 1;
         break; // not really necessary...
      case 9:
         // Umbrella Item
         object->fcreate = _ds_objects_b00o09_create;
         object->fexecute = _ds_objects_b00o09_execute;
         return 1;
         break; // not really necessary...
      case 10:
         // Hologram Item
         object->fcreate = _ds_objects_b00o10_create;
         object->fmanage = _ds_objects_b00cycle_manage;
         object->fexecute = _ds_objects_b00o10_execute;
         return 1;
         break; // not really necessary...
      case 11:
         // No Climb
         object->fcreate = _ds_objects_b00o11_create;
         return 1;
         break; // not really necessary...                  
      case 12:
         // Eye Sound
         object->fcreate = _ds_objects_b00o12_create;
         return 1;
         break; // not really necessary...                  
      case 13:
         // Sticky
         object->fcreate = _ds_objects_b00o13_create;
         return 1;
         break; // not really necessary...         
      case 14:
         // Shift A
         object->fcreate = _ds_objects_b00o14_create;
         object->fmanage = _ds_objects_b00cycle_manage;
         object->fexecute = _ds_objects_b00o14_execute;
         return 1;
         break; // not really necessary...
      case 15:
         // Shift B
         object->fcreate = _ds_objects_b00o15_create;
         object->fmanage = _ds_objects_b00cycle_manage;
         object->fexecute = _ds_objects_b00o15_execute;
         return 1;
         break; // not really necessary...
      case 16:
         // Shift C
         object->fcreate = _ds_objects_b00o16_create;
         object->fmanage = _ds_objects_b00cycle_manage;
         object->fexecute = _ds_objects_b00o16_execute;
         return 1;
         break; // not really necessary...         
      case 17:
         // Sign A
         object->fcreate = _ds_objects_b00o17_create;
         object->fexecute = _ds_objects_b00o17_execute;
         return 1;
         break; // not really necessary...
      case 18:
         // Sign B
         object->fcreate = _ds_objects_b00o18_create;
         object->fexecute = _ds_objects_b00o18_execute;
         return 1;
         break; // not really necessary...
      case 19:
         // Sign C
         object->fcreate = _ds_objects_b00o19_create;
         object->fexecute = _ds_objects_b00o19_execute;
         return 1;
         break; // not really necessary...
      case 20:
         // Warper
         object->fcreate = _ds_objects_b00o20_create;
         return 1;
         break; // not really necessary...         
      case 21:
         // Red Key Item
         object->fcreate = _ds_objects_b00o21_create;
         object->fexecute = _ds_objects_b00o21_execute;
         return 1;
         break; // not really necessary...
      case 22:
         // Yellow Key Item
         object->fcreate = _ds_objects_b00o22_create;
         object->fexecute = _ds_objects_b00o22_execute;
         return 1;
         break; // not really necessary...
      case 23:
         // Blue Key Item
         object->fcreate = _ds_objects_b00o23_create;
         object->fexecute = _ds_objects_b00o23_execute;
         return 1;
         break; // not really necessary...
      case 24:
         // Violet Key Item
         object->fcreate = _ds_objects_b00o24_create;
         object->fexecute = _ds_objects_b00o24_execute;
         return 1;
         break; // not really necessary...
      case 25:
         // No Jump
         object->fcreate = _ds_objects_b00o25_create;
         return 1;
         break; // not really necessary...         
      case 26:
         // Red Glow
         object->fcreate = _ds_objects_b00o26_create;
         return 1;
         break; // not really necessary...         
      case 27:
         // Cyan Glow - Dark
         object->fcreate = _ds_objects_b00o27_create;
         return 1;
         break; // not really necessary...         
      case 28:
         // Cyan Glow - Light
         object->fcreate = _ds_objects_b00o28_create;
         return 1;
         break; // not really necessary...         
      case 29:
         // Area A = Sign A for KSDS
         object->fcreate = _ds_objects_b00o17_create;
         object->fexecute = _ds_objects_b00o17_execute;
         return 1;
         break; // not really necessary...
      case 30:
         // Area B = Sign B for KSDS
         object->fcreate = _ds_objects_b00o18_create;
         object->fexecute = _ds_objects_b00o18_execute;
         return 1;
         break; // not really necessary...
      case 31:
         // Area C = Sign C for KSDS
         object->fcreate = _ds_objects_b00o19_create;
         object->fexecute = _ds_objects_b00o19_execute;
         return 1;
         break; // not really necessary...
   }
   return 0;
}   
