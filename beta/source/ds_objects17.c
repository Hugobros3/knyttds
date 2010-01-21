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
#include "ds_3dsprite.h"
#include "ds_global.h"
#include "ds_objects_lib.h"
#include "ds_objects.h"
#include "ds_util.h"
#include "ds_world.h"
#include "ds_map.h"
#include "ds_juni.h"

// BANK 17 [B17]
//=================================================================================================

// DARK SHOT [B17_O01]
//..........................................................................................
int _ds_objects_b17o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b17o01_manage(void *objp) {
   ds_t_object *object = objp;
   int pre_state;

	// Normal movement
	pre_state = object->inner[3];
	ds_objects_lib_beh_diskMovementB(object,
									60,60, // 1.00 - Change speed
									PA_RandMinMax(8,12),1,0, // "Simulates" (8..12)
									7,12, // Gesture
									60,100, // 1.00 - Change direction
									1, // Cycle Gesture
									0, // Don't Follow Juni
									3);
	// But... on change speed... special change!
	if (((ds_global_tick % 60) == 0) && (object->inner[3] != 0) && (pre_state != 0)) {
	   object->inner[3] = 0;
	}   

   return 1;
}

int _ds_objects_b17o01_instance(void *objp) {
   ds_t_object *object = objp;   
   ds_t_object *particle;
   int speedX;
   int speedY;
	int gravity;

	// If 23..32 when stopped, launch particle!	
	if ((object->inner[3] == 0) && (ds_global_tick % 60 >=23) && (ds_global_tick % 60 <= 32)) {
	   particle = ds_objects_createParticle(object->x + 8, object->y - 8, object->layer, 42);
	   gravity = 8; // Small correction from 12 
	   speedY = 20 + PA_RandMinMax(0,39); // 20...59
	   speedX = PA_RandMinMax(0,39); // 0..39?
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
						(PA_RandMinMax(0,1)==0)?(speedX):(-speedX), 
						-speedY, 
			 			gravity, 0, 3); 
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}
	
	return 1;	
}

// DARK CUTE [B17_O02]
//..........................................................................................
int _ds_objects_b17o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b17o02_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementB_Spike(object, 
									 60, 60, // 1.00 - change Speed
									 0, 1, 12, // 0 + R(1)*12
									 6, 11, // Sprites
									 255, 100, // Dir (chance - ALWAYS)
									 0,  // Not cycle
									 24,  // Distance									 
									 0,  // DO NOT Follow Juni
									 3); // Movement
   
   return 1;
}   

// WATER EYE [B17_O03]
//..........................................................................................
int _ds_objects_b17o03_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_3dsprite_markInvisible(object->sprite,1); // ON!

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b17o03_manage(void *objp) {
   ds_t_object *object = objp;
   
	switch (object->inner[0]) {
	   case 0:
	      // Nothing to do... (besides management)
	      object->inner[1] = 0;
	      object->inner[9] = 3;
			// However... check if I should promote to big baddie :-)
		   if ((ds_global_tick % 187) == 0) { // 300 / 1.6
		   	object->inner[0] = 1;
		   	ds_objects_semaphoreON(object);
		 	}  				
	      break;
	   case 1:
	      // Prepare the critter...
	      object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
			object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_INVISIBLE);
			ds_3dsprite_markInvisible(object->sprite,0); // OFF!
			object->inner[0] = 2;
	      break;
	   case 2:
	      	// Sprite movement
	         if (object->inner[9] > 0)
	         	object->inner[9]--;
	         if (object->inner[9] <= 0) {
	            object->inner[1]++;
			      if (object->inner[1] >= ds_3dsprite_getMaxFrame(object->sprite)) {
			         object->inner[1] = 0;
			      	object->inner[0] = 3;
			    	}  	
			   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	            object->inner[9] = 3;
					// Particle launch
					if (object->inner[1] == 18) {
					   int i;
				      ds_t_object *particle;
					   int speedX;
					   int speedY;
	   				int gravity;
					   for (i = 0; i < 8; i++) {
						   particle = ds_objects_createParticle(object->x + 8, object->y - 8, object->layer, 42);
						   gravity = 8; // Small correction from 12 
						   speedY = 20 + PA_RandMinMax(0,39); // 20...59
						   speedX = PA_RandMinMax(0,39); // 0..39?
						   ds_objects_lib_beh_particleMMF2_Init(particle, 
											(PA_RandMinMax(0,1)==0)?(speedX):(-speedX), 
											-speedY, 
								 			gravity, 0, 3); 
					   }   
					   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
					}   
				}				
	      break;
	   case 3:
	      // Finish the critter...
	      object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
			object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
			ds_3dsprite_markInvisible(object->sprite,1); // ON!
			ds_objects_semaphoreOFF(object);
			object->inner[0] = 0;
	      break;
	}   
   
   return 1;
}

// FLOOR SPITTER [B17_O04]
//..........................................................................................
int _ds_objects_b17o04_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b17o04_manage(void *objp) {
   ds_t_object *object = objp;

	if (ds_objects_lib_beh_flower(object,0,160,0,180,10,6)) { // 200 - 210 (Correction) 
		object->innerpar[0] = 1;
		ds_objects_instancephore(object); // Wanna have the chance to be executed!
	} else {
	   object->innerpar[0] = 0;
	}   	
   
   return 1;
}

int _ds_objects_b17o04_instance(void *objp) {
   ds_t_object *object = objp;   
   ds_t_object *particle;
   int speedX;
   int speedY;
	int gravity;
      
   if (object->innerpar[0] == 1) {
	   particle = ds_objects_createParticle(object->x + 8, object->y - 8, object->layer, 42);
	   gravity = 8; // Small correction from 12 
	   speedY = 30 + PA_RandMinMax(0,29); // 30...59
	   speedX = PA_RandMinMax(0,39); // 0..39?
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
						(PA_RandMinMax(0,1)==0)?(speedX):(-speedX), 
						-speedY, 
			 			gravity, 0, 3); 
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
   }   
   
   return 1;
}   

int _ds_objects_b17o04_condition(void *objp) {
   ds_t_object *object = objp;
   return object->innerpar[0];
}

// CEILING SPITTER [B17_O05]
//..........................................................................................
int _ds_objects_b17o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b17o05_manage(void *objp) {
   ds_t_object *object = objp;

	if (ds_objects_lib_beh_flower(object,0,190,0,210,19,6)) {  // 250 - 280 (Correction)
		object->innerpar[0] = 1;
		ds_objects_instancephore(object); // Wanna have the chance to be executed!
	} else {
	   object->innerpar[0] = 0;
	}   	
   
   return 1;
}

int _ds_objects_b17o05_instance(void *objp) {
   ds_t_object *object = objp;   
   ds_t_object *particle;
   int speedX;
   int speedY;
	int gravity;

   if (object->innerpar[0] == 1) {
	   particle = ds_objects_createParticle(object->x + 8, object->y, object->layer, 42);
	   gravity = 8; // Small correction from 12 
	   speedY = PA_RandMinMax(0,19); // 0...19
	   speedX = PA_RandMinMax(0,19); // 0..19?
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
						(PA_RandMinMax(0,1)==0)?(speedX):(-speedX), 
						speedY, 
			 			gravity, 0, 3); 
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
   }   
   
   return 1;
}

int _ds_objects_b17o05_condition(void *objp) {
   ds_t_object *object = objp;
   return object->innerpar[0];
}
   


// CREEPY FLIER [B17_O06]
//..........................................................................................
int _ds_objects_b17o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b17o06_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_flyAround(object,2,1*24,4*24,0,0,2,1,0,-1,2);
   
   return 1;
}

// FLOOR NINJA [B17_O07]
//..........................................................................................
int _ds_objects_b17o07_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b17o07_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;
   
   // Behaviour
	res = ds_objects_lib_beh_launcherMovementLR(object, 
									 0, 0, // Never change speed, direction
									 14, 1, 0,
									 60, 0, 20, // Change state every second, launch at 0.2 sec
									 8, 16, 24, 32,
									 0, // NO cycle gesture
									 (object->inner[0] == 2), // Follow Juni? Depends...
									 3);									 
	
   if (res) {
		int s = 40;
		double angle;
		if (object->inner[2] == 1) {
		   // Left!
		   angle = (PA_RandMinMax(0,99) < 50)?22.5:45;
		} else {
		   // Right!
		   angle = (PA_RandMinMax(0,99) < 50)?157.5:135;
		}   
		angle = 0 - angle;
	   int sy = (int)(ds_util_fastSin(angle) * s);
	   int sx = (int)(ds_util_fastCos(angle) * s);

	   particle = ds_objects_createParticle(object->x + 8, 
										object->y - 5, object->layer, 
										57);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
				sx, 
				sy, 
		 		5, 0, 3);
	}	
   
   return 1;
}

// RIGHT NINJA [B17_O08]
//..........................................................................................
int _ds_objects_b17o08_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b17o08_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;

   int res;
   
   // Behaviour
	res = ds_objects_lib_beh_disk(object, 0, // Y
									 0, // Never
									 9, 1, 0, // Speed 9 (1pix/60fps)
									 -1, // Do not change direction
									 3, 
									 8, 8);
									 
	if (object->inner[11] == 0) {
	   if ((ds_global_tick % 90) == 0) { // 150 / 1.6
	   	object->inner[0] = 2;
	   	object->inner[11] = 1;
	 	}  	
	} else {
	   if (res) {
	      object->inner[11] = 0;
	   }   
	}      
									 	
   if (res) {
		int s = 40;
		double angle;
	   // Right!
	   angle = (PA_RandMinMax(0,99) < 50)?157.5:135;
		angle = 0 - angle;
	   int sy = (int)(ds_util_fastSin(angle) * s);
	   int sx = (int)(ds_util_fastCos(angle) * s);

	   particle = ds_objects_createParticle(object->x + 8, 
										object->y - 5, object->layer, 
										57);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
				sx, 
				sy, 
		 		5, 0, 3);
	}	
   
   return 1;
}

// LEFT NINJA [B17_O09]
//..........................................................................................
int _ds_objects_b17o09_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b17o09_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;

   int res;
   
   // Behaviour
	res = ds_objects_lib_beh_disk(object, 0, // Y
									 0, // Never
									 9, 1, 0, // Speed 9 (1pix/60fps)
									 -1, // Do not change direction
									 3, 
									 8, 8);

	if (object->inner[11] == 0) {
	   if ((ds_global_tick % 100) == 0) { // 165 / 1.6
	   	object->inner[0] = 2;
	   	object->inner[11] = 1;
	 	}  	
	} else {
	   if (res) {
	      object->inner[11] = 0;
	   }   
	}      
									 	
   if (res) {
		int s = 40;
		double angle;
	   // Left!
	   angle = (PA_RandMinMax(0,99) < 50)?22.5:45;
		angle = 0 - angle;
	   int sy = (int)(ds_util_fastSin(angle) * s);
	   int sx = (int)(ds_util_fastCos(angle) * s);

	   particle = ds_objects_createParticle(object->x + 8, 
										object->y - 5, object->layer, 
										57);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
				sx, 
				sy, 
		 		5, 0, 3);
	}	
   
   return 1;
}


// DARK FLOOR SPIKER [B17_O10]
//..........................................................................................
int _ds_objects_b17o10_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b17o10_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_appear(object,80,100,2,3,0); // Corrected   

   return 1;
}

// L->R DARK WALL SPIKER [B17_O11]
//..........................................................................................
int _ds_objects_b17o11_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
   //object->x += 4; // 16 + 4
   ds_3dsprite_setX(object->sprite,object->x);
   object->y -= 40 - 20; // 5 * 8
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

int _ds_objects_b17o11_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_appear(object,80,100,2,3,0); // Corrected   

   return 1;
}

// R->L WALL SPIKER [B17_O12]
//..........................................................................................
int _ds_objects_b17o12_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
   object->x -= 40; // 16 + 4
   ds_3dsprite_setX(object->sprite,object->x);
   object->y -= 40 - 20; // 5 * 8
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

int _ds_objects_b17o12_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_appear(object,80,100,2,3,0); // Corrected   

   return 1;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects17_assign(u8 obj, ds_t_object *object) {
   switch (obj) {
      case 1:
      	// Dark Shot
      	object->fcreate = _ds_objects_b17o01_create;
      	object->fmanage = _ds_objects_b17o01_manage;
      	object->finstance = _ds_objects_b17o01_instance;
      	return 1;
      	break; // not really necessary...            
      case 2:
      	// Dark Cute
      	object->fcreate = _ds_objects_b17o02_create;
      	object->fmanage = _ds_objects_b17o02_manage;
      	return 1;
      	break; // not really necessary...            
      case 3:
      	// Water Eye
      	object->fcreate = _ds_objects_b17o03_create;
      	object->fmanage = _ds_objects_b17o03_manage;
      	return 1;
      	break; // not really necessary...            
      case 4:
      	// Floor Spitter
      	object->fcreate = _ds_objects_b17o04_create;
      	object->fmanage = _ds_objects_b17o04_manage;
      	object->finstance = _ds_objects_b17o04_instance;
      	object->fcondition = _ds_objects_b17o04_condition;
      	return 1;
      	break; // not really necessary...            
      case 5:
      	// Ceiling Spitter
      	object->fcreate = _ds_objects_b17o05_create;
      	object->fmanage = _ds_objects_b17o05_manage;
      	object->finstance = _ds_objects_b17o05_instance;
      	object->fcondition = _ds_objects_b17o05_condition;
      	return 1;
      	break; // not really necessary...            
      case 6:
      	// Creepy Flier
      	object->fcreate = _ds_objects_b17o06_create;
      	object->fmanage = _ds_objects_b17o06_manage;
      	return 1;
      	break; // not really necessary...            
      case 7:
      	// Floor Ninja
      	object->fcreate = _ds_objects_b17o07_create;
      	object->fmanage = _ds_objects_b17o07_manage;
      	return 1;
      	break; // not really necessary...            
      case 8:
      	// Right Ninja
      	object->fcreate = _ds_objects_b17o08_create;
      	object->fmanage = _ds_objects_b17o08_manage;
      	return 1;
      	break; // not really necessary...            
      case 9:
      	// Left Ninja
      	object->fcreate = _ds_objects_b17o09_create;
      	object->fmanage = _ds_objects_b17o09_manage;
      	return 1;
      	break; // not really necessary...            
      case 10:
      	// Dark Floor Spiker
      	object->fcreate = _ds_objects_b17o10_create;
      	object->fmanage = _ds_objects_b17o10_manage;
      	return 1;
      	break; // not really necessary...            
      case 11:
      	// L->R Dark Wall Spiker
      	object->fcreate = _ds_objects_b17o11_create;
      	object->fmanage = _ds_objects_b17o11_manage;
      	return 1;
      	break; // not really necessary...            
      case 12:
      	// R->L Dark Wall Spiker
      	object->fcreate = _ds_objects_b17o12_create;
      	object->fmanage = _ds_objects_b17o12_manage;
      	return 1;
      	break; // not really necessary...            
   }      
	return 0;   
}
