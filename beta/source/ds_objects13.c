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
#include "ds_world.h"
#include "ds_3dsprite.h"
#include "ds_map.h"
#include "ds_music.h"

#define _DS_C_LASER 93

// BANK 13 [B03]
//=================================================================================================

void _ds_objects_Laser_init(ds_t_object *object, int maxTime, int value) {
   object->inner[0] = value; // Actual state (0-Always, 1-on, 2-off)   
   object->inner[1] = 1; // Waiting time (Update next frame))
   object->inner[2] = (value == 2)?3:0; // Frame         
   object->inner[3] = maxTime >> 1; // Time for change... (initial)
	ds_3dsprite_setFrame(object->sprite,object->inner[2]);
	if ((value == 0) || (value == 1)) {
	   ds_3dsprite_markInvisible(object->sprite,0); // ON!
	   object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_INVISIBLE);
	} else {
	   ds_3dsprite_markInvisible(object->sprite,1); // OFF!
	   object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	}                     
}   

void _ds_objects_Laser_exe(ds_t_object *object, int waitTime, int maxTime) {
/* 
	[0]:STATE
	[1]:WAITING TIME
	[2]:ACTUAL FRAME
	[3]:TIME TO CHANGE STATE
*/ 
   switch (object->inner[0]) {
      case 0:
         // ALWAYS ON...
         object->inner[1]--;
         if (object->inner[1] <= 0) {
	      	object->inner[1] = waitTime; // Waiting time         	            
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);            
         	object->inner[2] = (object->inner[2] + 1) % 3;
         }            
         break;
      case 1:
         // ON...
         object->inner[1]--;
         if (object->inner[1] <= 0) {
         	object->inner[1] = waitTime; // Waiting time            
         	ds_3dsprite_setFrame(object->sprite,object->inner[2]);            
         	object->inner[2] = (object->inner[2] + 1) % 3;         	
         }      
         object->inner[3]--;
         if (object->inner[3] <= 0) {
            object->inner[3] = maxTime;
            object->inner[0] = 2;
            object->inner[2] = 3; // No laser...
            ds_3dsprite_setFrame(object->sprite,object->inner[2]);
            ds_3dsprite_markInvisible(object->sprite,1);
            object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
         }   
         break;         
      case 2:
         // OFF...
         object->inner[3]--;
         if (object->inner[3] <= 0) {
            object->inner[3] = maxTime;
            object->inner[0] = 1;
            object->inner[2] = 0; // Laser!!!            
            ds_3dsprite_setFrame(object->sprite,object->inner[2]);
            ds_3dsprite_markInvisible(object->sprite,0);
            object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_INVISIBLE);
         }   
         break;                  
	}	
}

// RED CANNON [B13_O01]
//..........................................................................................
int _ds_objects_b13o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN_SPECIAL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b13o01_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int gravity;

	// Just Cycle...
	ds_objects_lib_beh_cycle(object,5);
	
	// Throw particles, too!!!! 0-240 [60fp instead of 50fps], when 10-90 throw them!
	object->inner[10]++;
	if (object->inner[10] == 240) 
		object->inner[10] = 0;
	if ((object->inner[10] >= 10) && (object->inner[10] <= 90) &&
		 ((object->inner[10] % 2) == 0))	 {
	   particle = ds_objects_createParticle(object->x + 2, object->y - 12, object->layer, 42);
	   gravity = 10 + PA_RandMinMax(0,4); 
		ds_objects_lib_beh_particleMMF2_Init(particle, 
					-object->inner[10],
					-object->inner[10],
			 		gravity, 0, 3); 
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   	
	if (object->inner[10] == 10) {
		object->inner[11] = ds_music_playSound("Liquid", 0, 0);
	}
	if (object->inner[10] == 90) {
		ds_music_stopSoundChannel(object->inner[11]);
	}
   return 1;
}

// GREEN CANNON [B13_O02]
//..........................................................................................
int _ds_objects_b13o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN_SPECIAL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b13o02_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int speed;
   int gravity;

	// Just Cycle...
	ds_objects_lib_beh_cycle(object,5);
	
	// Throw particles, too!!!! 0-240 [60fp instead of 50fps], when 10-40 aim to juni!
	object->inner[10]++;
	if (object->inner[10] == 240) 
		object->inner[10] = 0;
	if ((object->inner[10] >= 10) && (object->inner[10] <= 40) &&
		 ((object->inner[10] % 2) == 0)) {
	   particle = ds_objects_createParticle(object->x + 2, object->y - 12, object->layer, 42);
	   gravity = 10 + PA_RandMinMax(0,4); 
	   speed = ((ds_objects_lib_distanceJuniX(object,1,1)) / 8) + 13 + PA_RandMinMax(0,5); // Correction
	   if (speed > 0) {
			ds_objects_lib_beh_particleMMF2_Init(particle, 
						-speed, 
						-speed, 
			 			gravity, 0, 3); 
		} 		
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   	
	if (object->inner[10] == 10) {
		object->inner[11] = ds_music_playSound("Liquid", 0, 0);
	}
	if (object->inner[10] == 40) {
		ds_music_stopSoundChannel(object->inner[11]);
	}
   return 1;
}

// MOVING LASER [B13_O03]
//..........................................................................................
int _ds_objects_b13o03_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b13o03_manage(void *objp) {
   ds_t_object *object = objp;
   int frameIni = 0;
   int frameMax = 0;
	int collided;
	int newx;
	// Management   
   switch (object->inner[0]) {
      case 0: // MOVE LEFT
      	// Details
		   frameIni = 0;
		   frameMax = 0;
			// Move?
      	newx = object->x - 1;
			collided = 0;				   
		   if ((newx < 0) || (newx >= (600 - object->xs))) {
		      collided = 1;
		   } else {
		   	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
									 object->xs,
									 object->ys,
									 newx,
									 object->y,
									 &ds_global_map.tileMapCol);
			}
      	// State change?
      	if (!collided) {
      	   // Just move
			   object->x = newx;
			   ds_3dsprite_setX(object->sprite,object->x);
      	} else {
      	   // Change state
				ds_music_playSound("Laser Machine A", 0, 0);
      	   object->inner[0] = 1;
      	   object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
      	   ds_3dsprite_setFrame(object->sprite,1);
			}      
         break;
      case 1: // MOVE RIGHT
      	// Details
		   frameIni = 1;
		   frameMax = ds_3dsprite_getMaxFrame(object->sprite);
			// Move?
      	newx = object->x + 2;
			collided = 0;				   
		   if ((newx < 0) || (newx >= (600 - object->xs))) {
		      collided = 1;
		   } else {
		   	collided = ds_map_collideFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
									 object->xs,
									 object->ys,
									 newx,
									 object->y,
									 &ds_global_map.tileMapCol);
			}
      	// State change?
      	if (!collided) {
      	   // Just move
			   object->x = newx;
			   ds_3dsprite_setX(object->sprite,object->x);
      	} else {
      	   // Change state
		   ds_music_playSound("Laser Machine B", 0, 0);
      	   object->inner[0] = 0;
      	   object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_HARMFUL);
      	   ds_3dsprite_setFrame(object->sprite,0);
			}      
		   // Sprite change?
		   if (!collided) {
			   if (object->inner[9] > 0)
			   	object->inner[9]--;
			   if (object->inner[9] <= 0) {
			      object->inner[1]++;
			      if (object->inner[1] >= frameMax)
			      	object->inner[1] = frameIni;
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
			      object->inner[9] = 3;
				}   
			}	
         break;
   }   
	
   return 1;
}

// SQUARE ROBOT [B13_O04]
//..........................................................................................
int _ds_objects_b13o04_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN_SPECIAL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b13o04_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int nextstate;

	switch (object->inner[0]) {
	   case 0:
	      // Wait...
	      object->inner[11]++;
	      if (object->inner[11] >= 180) { // 3 sec
	      	object->inner[11] = 0;
	      	object->inner[0] = 1; // Next State
	      }   
	      break;
	   case 1:
	      // Prepare to launch...
	      nextstate = 0;
		   if (object->inner[9] > 0)
		   	object->inner[9]--;
		   if (object->inner[9] <= 0) {
		      object->inner[1]++;
		      if (object->inner[1] >= ds_3dsprite_getMaxFrame(object->sprite)) {
		      	object->inner[1] = 0;
		      	nextstate = 1;
		    	}  	
		   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
		      object->inner[9] = 3;
			}  
			// Launch?
			if (nextstate) {
			   object->inner[0] = 2; // Next State
			}   
	      break;
	   case 2:
	      // Launch!!!!!
		   particle = ds_objects_createParticle(object->x + 12, object->y - 24, object->layer, 52);
		   int speedx = 30 - PA_RandMinMax(0,60); // Correction
		   int speedy = 80 + PA_RandMinMax(0,10); // Correction
			ds_objects_lib_beh_particleMMF2_Init(particle, 
						speedx, 
						-speedy, 
			 			20, 0, 3);
			ds_music_playSound("Cannon Shot", 0, 0);
			ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	      // Finish...
	      object->inner[0] = 0; // Next State
	      break;
	}   	
   return 1;
}

// BLACK RAINING ROBOT [B13_O05]
//..........................................................................................
int _ds_objects_b13o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN_SPECIAL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b13o05_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;

	ds_objects_lib_beh_squirrelMovementLR(object, 
									 24, 1, 0, // Speed 24
									 12, // Deceleration 12
									 120, 60, // Count 120, rand ---
									 1, // Follow Juni
									 3);
									 
	object->inner[11] = (object->inner[11] + 1) % 120;
	if (object->inner[11] == 0)
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	if ((object->inner[11] > 0) && (object->inner[11] < 68) && 
		 ((object->inner[11] % 2) == 0)) {
	   // Rain!
	   particle = ds_objects_createParticle(object->x + 8 + PA_RandMinMax(0,1), object->y, object->layer, 42);
	   int gravity = 10 + PA_RandMinMax(0,4); 
	   int speed = 0;
		ds_objects_lib_beh_particleMMF2_Init(particle, 
					0, 
					speed, 
		 			gravity, 0, 3); 
	}   
	if (object->inner[11] == 5) {
		object->inner[6] = ds_music_playSound("Liquid", 0, 0);
	}
	if (object->inner[11] == 68) {
		ds_music_stopSoundChannel(object->inner[6]);
		object->inner[6] = 0; // HACK - 6 is not used in squirrel movement O_o
	}


   return 1;
}      

// ROLLER [B13_O06]
//..........................................................................................
int _ds_objects_b13o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_FULLCOLLIDE);	
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b13o06_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;

	ds_objects_lib_beh_cycle(object,5);

	if ((ds_global_tick % 120) == 90) {
		ds_music_playSound("Roller A", 0, 0);
	}   									 

	if ((ds_global_tick % 120) == 0) {
		ds_music_playSound("Roller B", 0, 0);
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	   particle = ds_objects_createParticle(object->x + 18, object->y, object->layer, 53);
		ds_objects_lib_beh_particleMMF2_Init(particle, 
					15, 
					0, 
		 			0, 0, 3);
	}   									 

   return 1;
}

// ROTOROBO [B13_O13]
//..........................................................................................
int ds_b13_RotoRobo = 18;
int ds_b13_RotoRoboM[] = {4,1,4,1,4,3,7,1,5,4,2,4,2,3,5,1,1,2};
int ds_b13_RotoRoboD[] = {6,1,6,3,6,1,2,7,6,3,6,7,6,3,2,7,2,3};
int _ds_objects_b13o13_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN_SPECIAL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b13o13_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_followPath(object,1,ds_b13_RotoRobo,ds_b13_RotoRoboM,ds_b13_RotoRoboD,0,3);   
   return 1;
}

// LASER (TYPE 1) [B13_O08,O11]
//..........................................................................................

int _ds_objects_b13Laser1_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	_ds_objects_Laser_init(object,_DS_C_LASER,1); // Turned ON		

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b13Laser1_manage(void *objp) {
   ds_t_object *object = objp;
	
	// Try to make a reservation
	if (!object->inner[11]) {
		object->inner[11] = 1; // Enter only once
		if (!ds_objects_existphore(object)) {
			ds_objects_semaphoreON(object); // Become the sound manager
		}
	}
	// Continue
	object->inner[10] = object->inner[0];
	_ds_objects_Laser_exe(object,2,_DS_C_LASER);
   return 1;
}

// LASER (TYPE 0) [B13_O07,O10]
//..........................................................................................

int _ds_objects_b13Laser0_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	_ds_objects_Laser_init(object,_DS_C_LASER,2); // Turned OFF

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b13Laser0_manage(void *objp) {
   ds_t_object *object = objp;
	
	// Try to make a reservation
	if (!object->inner[11]) {
		object->inner[11] = 1; // Enter only once
		if (!ds_objects_existphore(object)) {
			ds_objects_semaphoreON(object); // Become the sound manager
		}
	}
	// Continue
	object->inner[10] = object->inner[0];
	_ds_objects_Laser_exe(object,2,_DS_C_LASER);
   return 1;
}

// LASER (CONSTANT) [B13_O09,O12]
//..........................................................................................

int _ds_objects_b13LaserC_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	//object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	_ds_objects_Laser_init(object,_DS_C_LASER,0); // Constant		

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b13LaserC_manage(void *objp) {
   ds_t_object *object = objp;

	// Never changes...
	_ds_objects_Laser_exe(object,2,_DS_C_LASER);
   return 1;
}

//.................................................

int _ds_objects_b13laser_instance(void *objp) {
   ds_t_object *object = objp;

	// If state changed, make sound!
	if (object->inner[10] != object->inner[0]) {
		if (object->inner[0] == 2)  { // OFF!
			ds_music_playSound("Laser Machine B", 0, 0);
		} else { // ON!
			ds_music_playSound("Laser Machine A", 0, 0);
		}
	}
	
   return 1;
}


// DARK CANNON [B13_O14]
//..........................................................................................
int _ds_objects_b13o14_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN_SPECIAL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b13o14_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int speed;
   int gravity;

	// Just Cycle...
	ds_objects_lib_beh_cycle(object,5);
	
	// Throw particles, too!!!! Every .68'' aim to juni!
	object->inner[10]++;
	if ((object->inner[10] >= 40) && (object->inner[10] <= 42)) {
	   particle = ds_objects_createParticle(object->x + 2, object->y - 12, object->layer, 42);
	   gravity = 20 + PA_RandMinMax(0,2);
	   speed = ((ds_objects_lib_distanceJuniX(object,1,1)) / 10) + 36 + PA_RandMinMax(0,2); // Correction
	   if (speed > 0) {
			ds_objects_lib_beh_particleMMF2_Init(particle, 
						-speed, 
						-speed, 
			 			gravity, 0, 3); 
		} 		
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   	
	if (object->inner[10] == 38) {
		object->inner[11] = ds_music_playSound("Liquid", 0, 0);
	}
	if (object->inner[10] == 43) {  // 0.68 / 1.6
		ds_music_stopSoundChannel(object->inner[11]);
		object->inner[10] = 0;
	}
	
   return 1;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects13_assign(u8 obj, ds_t_object *object) {
   switch (obj) {
      case 1:
         // Red Cannon
         object->fcreate = _ds_objects_b13o01_create;
         object->fmanage = _ds_objects_b13o01_manage;
         return 1;
         break; // not really necessary...         
      case 2:
         // Green Cannon
         object->fcreate = _ds_objects_b13o02_create;
         object->fmanage = _ds_objects_b13o02_manage;
         return 1;
         break; // not really necessary...         
      case 3:
         // Moving Laser
         object->fcreate = _ds_objects_b13o03_create;
         object->fmanage = _ds_objects_b13o03_manage;
         return 1;
         break; // not really necessary...         
      case 4:
         // Square Robot
         object->fcreate = _ds_objects_b13o04_create;
         object->fmanage = _ds_objects_b13o04_manage;
         return 1;
         break; // not really necessary...         
      case 5:
         // Black Raining Robot
         object->fcreate = _ds_objects_b13o05_create;
         object->fmanage = _ds_objects_b13o05_manage;
         return 1;
         break; // not really necessary...         
      case 6:
         // Roller
         object->fcreate = _ds_objects_b13o06_create;
         object->fmanage = _ds_objects_b13o06_manage;
         return 1;
         break; // not really necessary...         
      case 7:
         // Laser 0
         object->fcreate = _ds_objects_b13Laser0_create;
         object->fmanage = _ds_objects_b13Laser0_manage;
			object->finstance = _ds_objects_b13laser_instance;
         return 1;
         break; // not really necessary...
      case 8:
         // Laser 1
         object->fcreate = _ds_objects_b13Laser1_create;
         object->fmanage = _ds_objects_b13Laser1_manage;
			object->finstance = _ds_objects_b13laser_instance;
         return 1;
         break; // not really necessary...
      case 9:
         // Laser C
         object->fcreate = _ds_objects_b13LaserC_create;
         object->fmanage = _ds_objects_b13LaserC_manage;
         return 1;
         break; // not really necessary...      
      case 10:
         // Laser 0
         object->fcreate = _ds_objects_b13Laser0_create;
         object->fmanage = _ds_objects_b13Laser0_manage;
			object->finstance = _ds_objects_b13laser_instance;
         return 1;
         break; // not really necessary...
      case 11:
         // Laser 1
         object->fcreate = _ds_objects_b13Laser1_create;
         object->fmanage = _ds_objects_b13Laser1_manage;
			object->finstance = _ds_objects_b13laser_instance;
         return 1;
         break; // not really necessary...
      case 12:
         // Laser C
         object->fcreate = _ds_objects_b13LaserC_create;
         object->fmanage = _ds_objects_b13LaserC_manage;
         return 1;
         break; // not really necessary...      
      case 13:
         // Roto Robo
         object->fcreate = _ds_objects_b13o13_create;
         object->fmanage = _ds_objects_b13o13_manage;
         return 1;
         break; // not really necessary...
      case 14:
         // Dark Cannon
         object->fcreate = _ds_objects_b13o14_create;
         object->fmanage = _ds_objects_b13o14_manage;
         return 1;
         break; // not really necessary...         
	}			      
   return 0;
}   
