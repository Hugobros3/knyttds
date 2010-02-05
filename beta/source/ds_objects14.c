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

#include <stdlib.h>
#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_map.h"
#include "ds_objects.h"
#include "ds_objects_lib.h"
#include "ds_util.h"
#include "ds_world.h"
#include "ds_juni.h"
#include "ds_3dsprite.h"
#include "ds_music.h"


// BANK 14 [B14]
//=================================================================================================

// RED SLIME [B14_O01]
//..........................................................................................
int _ds_objects_b14o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o01_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_slimeMovementLR(object, 
									 12, 1, 3, // Speed 12-15
									 9, // Deceleration 9
									 3);

   return 1;
}

// GREEN SLIME [B14_O02]
//..........................................................................................
int _ds_objects_b14o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o02_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_slimeMovementLR(object, 
									 6, 1, 0, // Speed 6
									 9, // Deceleration 9
									 3);

   return 1;
}

// PURPLE SNAIL [B14_O03]
//..........................................................................................
int _ds_objects_b14o03_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o03_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object,1, // X
									93,93, // 1.55 - Change speed
									0,1,2, // "Simulates" ((0 or 1)*2)*(0 or 1)
									0,0,0,0, // Basic Gesture (no special)
									723,100, // 12.05 - Change direction
									0, // No Gesture
									0, // No follow juni
									3,
									0,
									0);
   
	//ds_objects_lib_beh_moveLeftRight(object,1,1,0,100,24,48,2*60,6*60,2,2,0,-1,1,1);   
   return 1;
}


// RED WORM [B14_O04]
//..........................................................................................
int _ds_objects_b14o04_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o04_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object,1, // X
									53,53, // 0.88 - Change speed
									0,1,2, // "Simulates" ((0 or 1)*2)*(0 or 1)
									0,0,0,0, // Basic Gesture (no special)
									545,100, // 9.09 - Change direction
									0, // No Gesture
									0, // No follow Juni
									3,
									0,
									0);
   
	//ds_objects_lib_beh_moveLeftRight(object,1,1,0,100,24,48,2*60,6*60,2,2,0,-1,1,1);   
   return 1;
}

// BOUNCING ROCK [B14_O05]
//..........................................................................................
int _ds_objects_b14o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o05_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_diskMovementB(object,
									44,44, // 0.70 - Change speed
									PA_RandMinMax(0,5),1,5, // "Simulates" (5+(0 or 1)*5)*(0 or 1)
									0,0, // Basic Gesture (no special)
									240,100, // 4.00 - Change direction
									0, // No Gesture
									0, // Don't Follow Juni
									3);
	   
	//ds_objects_lib_beh_moveLeftRight(object,1,2,0,100,48,240,0,3*60,4,1,0,-1,0,1);   
   return 1;
}

// GREY WORM [B14_O06]
//..........................................................................................
int _ds_objects_b14o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o06_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object,1, // X
									57,57, // 0.95 - Change speed
									0,1,2, // "Simulates" ((0 or 1)*2)*(0 or 1)
									0,0,0,0, // Basic Gesture (no special)
									485,100, // 8.09 - Change direction
									0, // No Gesture
									0, // No follow Juni
									3,
									0,
									0);
   
	//ds_objects_lib_beh_moveLeftRight(object,1,1,0,100,24,48,2*60,6*60,2,2,0,-1,1,1);   
   return 1;
}

// BOUNCING BIG ROCK [B14_O07]
//..........................................................................................
int _ds_objects_b14o07_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o07_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_diskMovementB(object,
									60,60, // 1.0 - Change speed
									PA_RandMinMax(0,5),1,5, // "Simulates" (5+(0 or 1)*5)*(0 or 1)
									0,0,
									300,100, // 5.00 - Change direction
									0,
									0,
									3);
   
	//ds_objects_lib_beh_moveLeftRight(object,1,2,0,100,48,240,0,3*60,2,2,0,-1,0,1);   
   return 1;
}   
   

// SQUIRREL [B14_O08]
//..........................................................................................
int _ds_objects_b14o08_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o08_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_squirrelMovementLR(object, 
									 15, 1, 0, // Speed 15
									 12, // Deceleration 12
									 240, 67, // Count 240, rand 1.12
									 0, // Don't follow Juni
									 3);

   return 1;
}

// DEADLY SQUIRREL [B14_O09]
//..........................................................................................
int _ds_objects_b14o09_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o09_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_squirrelMovementLR(object, 
									 22, 9, 1, // Speed 22-31
									 8, // Deceleration 12 (changed to be more similar to KS)
									 240, 67, // Count 240, rand 1.12
									 1, // Follow Juni
									 3);

   return 1;
}

// BLINKING ROCK [B14_O10]
//..........................................................................................
int _ds_objects_b14o10_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o10_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b14 = 4;
	int ds_b14A[] = {1,3,14,16};

	ds_objects_lib_beh_knyttNgestures(object,ds_b14,ds_b14A,111/*1.85*/,5,0);
   return 1;
}   

// BROWN SQUIRREL [B14_O11]
//..........................................................................................
int _ds_objects_b14o11_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o11_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_squirrelMovementLR(object, 
									 15, 1, 0, // Speed 15
									 12, // Deceleration 12
									 240, 77, // Count 240, rand 1.28
									 0, // Don't follow Juni
									 3);

   return 1;
}

// SMILEY [B14_O12]
//..........................................................................................
int _ds_objects_b14o12_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o12_manage(void *objp) {
   ds_t_object *object = objp;
   int dJ;
   int dir;
   
   // Normal...
	ds_objects_lib_beh_diskMovementLR(object,1, // X
									-1,-1, // Never changes
									10,1,0, // Speed 10
									0,0,0,0, // Basic Gesture (no special)
									-1,-1, // Never change direction
									0, // No Gesture
									1, // Follow Juni
									3,
									0,
									0);
									
	// With Special details :-)
	switch (object->inner[11]) {
	   case 0:
	      dJ = ds_objects_lib_distanceJuniX(object,1,0);
	      if (abs(dJ) < 64) {
	         // Next state
	         object->inner[11] = 1;
	         // Stop...
	         object->inner[0] = -1;
	         // ...and gesture
	         object->inner[2] = (dJ >= 0)?1:-1;
	         object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[2],1);
	         ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	      }   
	      break;
	   case 1:
	      // Change where the critter looks
	      dJ = ds_objects_lib_distanceJuniX(object,1,0);
	      dir = (dJ >= 0)?1:-1;
	      if (dir != object->inner[2]) {
	         object->inner[2] = dir;
	         object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite,object->inner[2],1);
	         ds_3dsprite_setFrame(object->sprite,object->inner[1]);	         
	      }   
	      // Move?
	      if (abs(dJ) >= 70) {
	         // Next state
	         object->inner[11] = 0;
	         // Continue
	         object->inner[0] = 0;
			}   	      
	      break;
	}   	
	
   return 1;
}

// GREEN RUNNER SMALL / LARGE [B14_O13-14]
//..........................................................................................
int _ds_objects_b14oGreenR_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o13_manage(void *objp) {
   ds_t_object *object = objp;

	// Calculate details
	int followJuni = 0;
	int speed = 0;
	int speedR = 0;
	int dJ = ds_objects_lib_distanceJuniX(object,1,0);
	int dCheck = 140;
	
	if (abs(dJ) >= dCheck) {
	   // FAR
	   followJuni = 1;
	   speed = 27;
	   speedR = 5;
	} else {
	   if (((ds_juni_faceRight()) && (dJ < 0)) ||
		    ((!ds_juni_faceRight()) && (dJ > 0))) {
	   	// NEAR - Looking @ Me
		   followJuni = -1;
		   speed = 27;
		   speedR = 5;
		} else {
	   	// NEAR - Ignoring Me
		   followJuni = 1;
		   speed = 37;
		   speedR = 1;
		}       
	}      
	
	// Behaviour   
	ds_objects_lib_beh_squirrelMovementLR(object, 
									 speed, speedR, 1, // Speed speed + speedR
									 speed >> 1, // Deceleration based on speed
									 60, 67, // Count 40, rand 1.12
									 followJuni, // follow Juni depending on distance
									 3);

   return 1;
}

int _ds_objects_b14o14_manage(void *objp) {
   ds_t_object *object = objp;

	// Calculate details
	int followJuni;
	int speed;
	int speedR;
	int dJ = ds_objects_lib_distanceJuniX(object,1,0);
	int dCheck = 80;
	
	if (abs(dJ) >= dCheck) {
	   // FAR
	   followJuni = 1;
	   speed = 17;
	   speedR = 5;
	} else {
	   if (((ds_juni_faceRight()) && (dJ > 0)) ||
		    ((!ds_juni_faceRight()) && (dJ < 0))) {
	   	// NEAR - Looking @ Me
		   followJuni = -1;
		   speed = 17;
		   speedR = 5;
		} else {
	   	// NEAR - Ignoring Me
		   followJuni = 1;
		   speed = 40;
		   speedR = 1;
		}       
	}      
	
	// Behaviour   
	ds_objects_lib_beh_squirrelMovementLR(object, 
									 speed, speedR, 1, // Speed speed + speedR
									 (speed + speedR) >> 1, // Deceleration based on speed
									 40, 67, // Count 40, rand 1.12
									 followJuni, // follow Juni depending on distance
									 3);

   return 1;
}   

// BROWN SNAIL [B14_O15]
//..........................................................................................
int _ds_objects_b14o15_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o15_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object,1, // X 
									127,127, // 2.12 - Change speed
									0,1,2, // "Simulates" ((0 or 1)*2)*(0 or 1)
									0,0,0,0, // Basic Gesture (no special)
									845,100, // 14.09 - Change direction
									0, // No Gesture
									0, // No follow Juni
									3,
									0,
									0);
   
	//ds_objects_lib_beh_moveLeftRight(object,1,1,0,100,24,48,2*60,6*60,2,2,0,-1,1,1);   
   return 1;
}

// FLOOR SPIKER [B14_O16]
//..........................................................................................
int _ds_objects_b14o16_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
   object->x -= 20; // 16 + 4
   ds_3dsprite_setX(object->sprite,object->x);
   object->y -= 40; // 5 * 8
   ds_3dsprite_setY(object->sprite,object->y);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	//object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	// \- FIXME - error due to coordinate correction

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o16_manage(void *objp) {
   ds_t_object *object = objp;
   
	if (ds_objects_lib_beh_appear(object,80,100,2,3,0)) { // Corrected   
		if (object->inner[0] == 2) {
			ds_music_playSound("Spike Up", 0, 0);
		} else {
			ds_music_playSound("Spike Down", 0, 0);
		}
	}
   return 1;
}

// AI OF SUPER MARIO (yup, that's its true name -:) ) [B14_O17]
//..........................................................................................
int _ds_objects_b14o17_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o17_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object,1, // X
									-1,-1, // Never changes
									8,1,0, // Speed 8
									0,0,0,0, // Basic Gesture (no special)
									-1,-1, // Never change direction
									0, // No Gesture
									0, // No follow Juni
									3,
									0,
									0);
   return 1;
}

// CEILING SPIKER [B14_O18]
//..........................................................................................
int _ds_objects_b14o18_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
   object->x -= 20; // 16 + 4
   ds_3dsprite_setX(object->sprite,object->x);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	//object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	// \- FIXME - error due to coordinate correction

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o18_manage(void *objp) {
   ds_t_object *object = objp;
   
	if (ds_objects_lib_beh_appear(object,120,140,2,3,0)) { // Corrected   
		if (object->inner[0] == 2) {
			ds_music_playSound("Spike Up", 0, 0);
		} else {
			ds_music_playSound("Spike Down", 0, 0);
		}
	}
	
   return 1;
}

// EVIL PECKABOO [B14_O19]
//..........................................................................................
int _ds_objects_b14o19_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o19_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_appear(object,80,100,1,3,0); // Corrected   

   return 1;
}

// NORMAL PECKABOO [B14_O20]
//..........................................................................................
int _ds_objects_b14o20_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b14o20_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_appear(object,80,100,0,3,0); // Corrected   

   return 1;
}

// UDLR-SHOT [B14_O21-24]
//..........................................................................................
int _ds_objects_b14o21_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

// U-SHOT
int _ds_objects_b14o21_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res =  ds_objects_lib_beh_plaunchStop(object,500,50,4); // 500
	if (res) {
	   // Particle creation!
		ds_music_playSound("DiscBullet", 0, 0);
	   int i,sx,sy;
	   for (i=0; i<25; i++) {
	      sx = -5 + PA_RandMax(10); // Speed - -5..5
	      sy = PA_RandMax(5) + 5; // Speed - RAND(5)+5
	      particle = ds_objects_createParticle(object->x + 8, object->y, object->layer, 43);
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		0, 4, 3);
	   }
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK); 
	}   
	
   return 1;
}

// D-SHOT
int _ds_objects_b14o22_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res =  ds_objects_lib_beh_plaunchStop(object,480,50,4); // 480
	if (res) {
	   // Particle creation!
		ds_music_playSound("DiscBullet", 0, 0);
	   int i,sx,sy;
	   for (i=0; i<25; i++) {
	      sx = -5 + PA_RandMax(10); // Speed - -5..5
	      sy = -(PA_RandMax(5) + 5); // Speed - RAND(5)+5
	      particle = ds_objects_createParticle(object->x + 8, object->y - 12, object->layer, 43);
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		0, 4, 3);
	   } 
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   

   return 1;
}

// R-SHOT
int _ds_objects_b14o23_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res =  ds_objects_lib_beh_plaunchStop(object,500,50,4); // 500
	if (res) {
	   // Particle creation!
		ds_music_playSound("DiscBullet", 0, 0);
	   int i,sx,sy;
	   for (i=0; i<25; i++) {
	      sx = PA_RandMax(5) + 5; // Speed - RAND(5)+5
	      sy = -5 + PA_RandMax(10); // Speed - -5..5
	      particle = ds_objects_createParticle(object->x + 12, object->y - 8, object->layer, 43);
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		0, 4, 3);
	   } 
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   

   return 1;
}

// L-SHOT
int _ds_objects_b14o24_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res =  ds_objects_lib_beh_plaunchStop(object,400,50,4); // 400
	if (res) {
	   // Particle creation!
		ds_music_playSound("DiscBullet", 0, 0);
	   int i,sx,sy;
	   for (i=0; i<25; i++) {
	      sx = -(PA_RandMax(5) + 5); // Speed - RAND(5)+5
	      sy = -5 + PA_RandMax(10); // Speed - -5..5
	      particle = ds_objects_createParticle(object->x + 4, object->y - 8, object->layer, 43);
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		0, 4, 3);
	   } 
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   

   return 1;
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects14_assign(u8 obj, ds_t_object *object) {
   switch (obj) {
      case 1:
         // Red slime
         object->fcreate = _ds_objects_b14o01_create;
         object->fmanage = _ds_objects_b14o01_manage;
         return 1;
         break; // not really necessary...      
      case 2:
         // Green slime
         object->fcreate = _ds_objects_b14o02_create;
         object->fmanage = _ds_objects_b14o02_manage;
         return 1;
         break; // not really necessary...               
      case 3:
         // Purple Snail
         object->fcreate = _ds_objects_b14o03_create;
         object->fmanage = _ds_objects_b14o03_manage;
         return 1;
         break; // not really necessary...      
      case 4:
         // Red worm
         object->fcreate = _ds_objects_b14o04_create;
         object->fmanage = _ds_objects_b14o04_manage;
         return 1;
         break; // not really necessary...               
      case 5:
         // Bouncing Rock
         object->fcreate = _ds_objects_b14o05_create;
         object->fmanage = _ds_objects_b14o05_manage;
         return 1;
         break; // not really necessary...      
      case 6:
         // Grey worm
         object->fcreate = _ds_objects_b14o06_create;
         object->fmanage = _ds_objects_b14o06_manage;
         return 1;
         break; // not really necessary...               
      case 7:
         // Bouncing Rock Big
         object->fcreate = _ds_objects_b14o07_create;
         object->fmanage = _ds_objects_b14o07_manage;
         return 1;
         break; // not really necessary...      
      case 8:
         // Squirrel
         object->fcreate = _ds_objects_b14o08_create;
         object->fmanage = _ds_objects_b14o08_manage;
         return 1;
         break; // not really necessary...
      case 9:
         // Deadly Squirrel
         object->fcreate = _ds_objects_b14o09_create;
         object->fmanage = _ds_objects_b14o09_manage;
         return 1;
         break; // not really necessary...
      case 10:
         // Blinking Rock
         object->fcreate = _ds_objects_b14o10_create;
         object->fmanage = _ds_objects_b14o10_manage;
         return 1;
         break; // not really necessary...
      case 11:
         // Brown Squirrel
         object->fcreate = _ds_objects_b14o11_create;
         object->fmanage = _ds_objects_b14o11_manage;
         return 1;
         break; // not really necessary...
      case 12:
         // Smiley
         object->fcreate = _ds_objects_b14o12_create;
         object->fmanage = _ds_objects_b14o12_manage;
         return 1;
         break; // not really necessary...
      case 13:
         // Green Runner Large
         object->fcreate = _ds_objects_b14oGreenR_create;
         object->fmanage = _ds_objects_b14o13_manage;
         return 1;
         break; // not really necessary...
      case 14:
         // Green Runner Small
         object->fcreate = _ds_objects_b14oGreenR_create;
         object->fmanage = _ds_objects_b14o14_manage;
         return 1;
         break; // not really necessary...         
      case 15:
         // Brown Snail
         object->fcreate = _ds_objects_b14o15_create;
         object->fmanage = _ds_objects_b14o15_manage;
         return 1;
         break; // not really necessary...      
      case 16:
         // Floor Spiker
         object->fcreate = _ds_objects_b14o16_create;
         object->fmanage = _ds_objects_b14o16_manage;
         return 1;
         break; // not really necessary...      
      case 17:
         // AI of Super Mario
         object->fcreate = _ds_objects_b14o17_create;
         object->fmanage = _ds_objects_b14o17_manage;
         return 1;
         break; // not really necessary...      
      case 18:
         // Ceiling Spiker
         object->fcreate = _ds_objects_b14o18_create;
         object->fmanage = _ds_objects_b14o18_manage;
         return 1;
         break; // not really necessary...      
      case 19:
         // Evil peckaboo
         object->fcreate = _ds_objects_b14o19_create;
         object->fmanage = _ds_objects_b14o19_manage;
         return 1;
         break; // not really necessary...      
      case 20:
         // Normal peckaboo
         object->fcreate = _ds_objects_b14o20_create;
         object->fmanage = _ds_objects_b14o20_manage;
         return 1;
         break; // not really necessary...      
      case 21:
         // D-shot
         object->fcreate = _ds_objects_b14o21_create;
         object->fmanage = _ds_objects_b14o21_manage;
         return 1;
         break; // not really necessary...      
      case 22:
         // U-shot
         object->fcreate = _ds_objects_b14o21_create;
         object->fmanage = _ds_objects_b14o22_manage;
         return 1;
         break; // not really necessary...      
      case 23:
         // R-shot
         object->fcreate = _ds_objects_b14o21_create;
         object->fmanage = _ds_objects_b14o23_manage;
         return 1;
         break; // not really necessary...      
      case 24:
         // L-shot
         object->fcreate = _ds_objects_b14o21_create;
         object->fmanage = _ds_objects_b14o24_manage;
         return 1;
         break; // not really necessary...      

   }   
   return 0;
}   
