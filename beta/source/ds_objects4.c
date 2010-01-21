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

// BANK 4 [B04]
//=================================================================================================

void _ds_objects_b04_waitHarm(ds_t_object *object) {
/*
	[0] - State
	[1] - Sprite
	[8] - Waiting time
*/
   // CHECK JUNI
   if (ds_objects_lib_isNearJuni(object,24*3,1)) {
      if ((object->inner[0] != 2) && (object->inner[0] != 3)) {
         object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
         object->inner[0] = 2;
         object->inner[1] = 4;
         object->inner[8] = 2;
      }   
   } else {
      if ((object->inner[0] == 2) || (object->inner[0] == 3)) {
         object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_HARMFUL);
         object->inner[0] = 4;
         object->inner[1] = 8;         
         object->inner[8] = 2;
      }         
   }   
   	
   // STATE   
   switch (object->inner[0]) {
      case 0:
         // WAIT TO MOVE
         object->inner[8]--;
         if (object->inner[8] <= 0) {
            object->inner[8] = 5;
            object->inner[1] = 1;
            object->inner[0] = 1;
         }   
         break;
      case 1:
         // MOVE
         object->inner[8]--;
         if (object->inner[8] <= 0) {
				if (object->inner[1] > 3) {
				   object->inner[0] = 0;				   
				   object->inner[1] = 0;				   
	         	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//					ds_3dsprite_updateSprite(object->sprite);		   				   
				   object->inner[8] = 180 + PA_RandMax(180);
				} else {   
	         	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//					ds_3dsprite_updateSprite(object->sprite);         	
					object->inner[1]++;   	
            	object->inner[8] = 2;					
         	}   
         }            
         break;         
      case 2:
         // HARM!
         object->inner[8]--;
         if (object->inner[8] <= 0) {
				if (object->inner[1] > 8) {
				   object->inner[0] = 3;
				} else {   
	         	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//					ds_3dsprite_updateSprite(object->sprite);            
					object->inner[1]++;   
            	object->inner[8] = 2;
         	}               
         }         
         break;                  
      case 3:
         // STAY HARM!
         break;  
      case 4:
         // LEAVE HARM
         object->inner[8]--;
         if (object->inner[8] <= 0) {
				if (object->inner[1] < 4) {
				   object->inner[0] = 0;
				   object->inner[1] = 0;
	         	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//					ds_3dsprite_updateSprite(object->sprite);		   
				} else {   
	         	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//					ds_3dsprite_updateSprite(object->sprite);            
					object->inner[1]--;				   
            	object->inner[8] = 5;
         	}               
         }                  
         break;                  			                         
   }   
}   

// RED SCORCH [B04_O01]
//..........................................................................................
int _ds_objects_b04o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	// harmful!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   ds_objects_lib_beh_moveLeftRight_ini(object,1);
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o01_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Try to follow Juni and do "pupita" ^^
	ds_objects_lib_beh_moveLeftRight(object,2,2,0,100,0,0,0*60,1*60,2,1,1,-1,1,1);
   
   // Everything went OK...
   return 1;
}

// BLUE SIDE [B04_O02]
//..........................................................................................
int _ds_objects_b04o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o02_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object, 0, // Y
									-1,-1, // Never changes
									9,1,0, // Speed 9 (1pix/50fps)
									0,0,0,0, // Basic Gesture (no special)
									-1,-1, // Does not matter...
									1, // No Gesture
									1, // FOLLOW Juni
									3,
									0,
									0);

   // Everything went OK...
   return 1;
}      

// FAST, MEDIUM, BABY RUNNER [B04_O03]
//..........................................................................................
int _ds_objects_b04o03_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	// harmful!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o03_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_hunter(object, 
									 -12, 36, 0, 0, // -12...36
									 30, // Speed
									 1, 0, // Follow Juni, stop
									 3,
									 0,0,6,6,
									 6,6,12,12);   
   // Everything went OK...
   return 1;
}      
 
int _ds_objects_b04o04_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_hunter(object, 
									 -12, 36, 0, 0, // -12...36
									 20, // Speed
									 1, 0, // Follow Juni, stop
									 3,
									 0,0,5,5,
									 5,5,8,8);   
   // Everything went OK...
   return 1;
}

int _ds_objects_b04o05_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_hunter(object, 
									 -12, 36, 0, 0, // -12...36
									 10, // Speed
									 1, 0, // Follow Juni, stop
									 3,
									 0,0,5,5,
									 5,5,8,8);   
   // Everything went OK...
   return 1;
}

// DROP CRAWLER - GROUND [B04_O06]
//..........................................................................................
int _ds_objects_b04o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o06_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;
   int i;
   int maxi = 1;
   int lowerB = 90;
   int upperB = 90;
   int actualB;
   
   
	res = ds_objects_lib_beh_diskMovementLR(object, 1, // X
									-1,-1, // Never changes
									8,4,1, // Speed 8..12
									0,0,0,0, // Basic Gesture (no special)
									-1,-1, // Does not matter...
									1, // No Gesture
									1, // FOLLOW Juni
									3,
									0,
									0);
									
	if ((ds_global_tick % 113) == 0) {
	   if (ds_objects_lib_distanceJuniX(object,1,1) < 200) {
	      // Launch particles!
	      switch (res) {
	         case 0: // Normal
	         	lowerB = 78 - 20;
	         	upperB = 101 + 20;
	         	maxi = 3;
	            break;
	         case 1: // Left Wall
	         	lowerB = 34;
	         	upperB = 56;
	         	maxi = 2;
	            break;
	         case 2: // Right Wall
	         	lowerB = 124;
	         	upperB = 146;
	         	maxi = 2;
	            break;	            
	      }   
	      for (i = 0; i < maxi; i++) {
	         actualB = PA_RandMinMax(lowerB,upperB);
			   particle = ds_objects_createParticle(object->x + 8, object->y - 16, object->layer, 39);
			   ds_objects_lib_beh_particleMMF2_Init(particle, 
							50 * ds_util_fastCos(actualB), 
							-50 * ds_util_fastSin(actualB),
					 		10, 0, 3);	     
			}
			ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);		     
	   }   
	}   

   // Everything went OK...
   return 1;
}      

// DROP CRAWLER - CEILING [B04_O07]
//..........................................................................................
int _ds_objects_b04o07_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o07_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;
   int i;
   int maxi = 1;
   int lowerB = 90;
   int upperB = 90;
   int actualB;
   
	res = ds_objects_lib_beh_diskMovementLR(object, 1, // X
									-1,-1, // Never changes
									8,4,1, // Speed 8..12
									0,0,0,0, // Basic Gesture (no special)
									-1,-1, // Does not matter...
									1, // No Gesture
									1, // FOLLOW Juni
									3,
									0,
									0);

	if ((ds_global_tick % 96) == 0) { // Countdown of 80 in 50 fps
	   if (ds_objects_lib_distanceJuniX(object,1,1) < 100) {
	      // Launch particles!
	      switch (res) {
	         case 0: // Normal
	         	lowerB = 258 - 20;
	         	upperB = 281 + 20;
	         	maxi = 3;
	            break;
	         case 1: // Left Wall
	         	lowerB = 304;
	         	upperB = 336;
	         	maxi = 2;
	            break;
	         case 2: // Right Wall
	         	lowerB = 214;
	         	upperB = 236;
	         	maxi = 2;
	            break;	            
	      }   
	      for (i = 0; i < maxi; i++) {
	         actualB = PA_RandMinMax(lowerB,upperB);
			   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 39);
			   ds_objects_lib_beh_particleMMF2_Init(particle, 
							50 * ds_util_fastCos(actualB), 
							-5 * ds_util_fastSin(actualB),
					 		10, 0, 3);	     
			}
			ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);		     
	   }   
	}   

   // Everything went OK...
   return 1;
}

// SLOW SIDERUNNER [B04_O08]
//..........................................................................................
int _ds_objects_b04o08_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o08_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object, 0, // Y
									-1,-1, // Never changes
									4,1,0, // Speed 4 (0.5pix/50fps)
									0,0,0,0, // Basic Gesture (no special)
									-1,-1, // Does not matter...
									1, // No Gesture
									1, // FOLLOW Juni
									3,
									0,
									0);

   // Everything went OK...
   return 1;
}            

// SPIDER [B04_O09]
//..........................................................................................
int _ds_objects_b04o09_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	// harmful!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o09_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_hunter(object, 
									 -8, 8, 0, 0, // -8...8 - Corrected
									 40, // Speed
									 0, 1, // NOT follow Juni, continue
									 3,
									 0,12,0,12,
									 0,12,12,24);   
   // Everything went OK...
   return 1;
}

// LEFT FISH [B04_O10]
//..........................................................................................
int _ds_objects_b04o10_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b04o10_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res = ds_objects_lib_beh_disk(object, 0, // Y
									 0, // Never change speed
									 9, 1, 0, // Speed 9 (1pix/60fps)
									 129, // 215 / 1.6 (Same as instance change :-P)
									 3, 
									 6, 8);
									 
	if (res == 1) {
	   particle = ds_objects_createParticle(object->x + 8, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					20, 
					0, 
			 		0, 0, 3);
	   particle = ds_objects_createParticle(object->x + 8, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					20, 
					8, 
			 		0, 0, 3);
	   particle = ds_objects_createParticle(object->x + 8, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					20, 
					-8, 
			 		0, 0, 3);
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}

   return 1;
}

int _ds_objects_b04o10_instance(void *objp) {
   ds_t_object *object = objp;
   
   if ((ds_global_tick % 129) == 0) // 215 / 1.6
   	object->inner[0] = 2;
      
   return 1;
}

// RIGHT FISH [B04_O11]
//..........................................................................................
int _ds_objects_b04o11_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b04o11_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res = ds_objects_lib_beh_disk(object, 0, // Y
									 0, // Never
									 9, 1, 0, // Speed 9 (1pix/60fps)
									 117, // 196 / 1.6 (Same as instance change :-P)
									 3, 
									 6, 8);

	if (res == 1) {
	   particle = ds_objects_createParticle(object->x, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					-20, 
					0, 
			 		0, 0, 3);
	   particle = ds_objects_createParticle(object->x, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					-20, 
					8, 
			 		0, 0, 3);
	   particle = ds_objects_createParticle(object->x, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					-20, 
					-8, 
			 		0, 0, 3);
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}

   return 1;
}

int _ds_objects_b04o11_instance(void *objp) {
   ds_t_object *object = objp;
   
   if ((ds_global_tick % 117) == 0) // 196 / 1.6
   	object->inner[0] = 2;
      
   return 1;
}

// YELLOW SCORCH [B04_O12]
//..........................................................................................
int _ds_objects_b04o12_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o12_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;
   int maxi = 1;
   int lowerB = 90;
   int upperB = 90;
   int actualB;
   int vel;

	res = ds_objects_lib_beh_launcherMovementLR(object, 
									 0,0, // Never change speed, direction
									 8, 4, 1,
									 60, 0, 30, // Change state every second, launch at 0.5 sec
									 8, 16, 17, 18,
									 0, // NO cycle gesture
									 1, // Follow Juni
									 3);									 
	
   if (res) {
      // Launch particles!
      switch (object->inner[2]) { // Direction
         case -1: // Left
         	lowerB = 124;
         	upperB = 146;
         	maxi = 3;
            break;
         case 1: // Right
         	lowerB = 34;
         	upperB = 56;
         	maxi = 2;
            break;
      }   
      actualB = PA_RandMinMax(lowerB,upperB);
      vel = 20 + 
		      (ds_objects_lib_distanceJuniX(object,1,1) / 5) +
		      (ds_objects_lib_distanceJuniY(object,1,1) / 5);
	   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 45);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					vel * ds_util_fastCos(actualB), 
					-vel * ds_util_fastSin(actualB),
			 		10, 0, 3);	     
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   


   // Everything went OK...
   return 1;
}

// SIDE CRAWLER [B04_O13]
//..........................................................................................
int _ds_objects_b04o13_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o13_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object, 0, // Y
									-1,-1, // Never changes
									9,1,0, // Speed 9 (1pix/50fps)
									0,0,0,0, // Basic Gesture (no special)
									-1,-1, // Does not matter...
									1, // No Gesture
									1, // FOLLOW Juni
									3,
									0,
									0);

   // Everything went OK...
   return 1;
}                  
      

// 3LEG [B04_O14]
//..........................................................................................
int _ds_objects_b04o14_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	// harmful!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o14_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_hunter(object, 
									 0, 0, 45, 33, // Time 0.75, chance 33%
									 30, // Speed
									 0, 0, // NOT follow Juni, NOT continue
									 3,
									 0,0,0,0,
									 0,11,11,22);   
   // Everything went OK...
   return 1;
}

// RED SCORCH [B04_O15]
//..........................................................................................
int _ds_objects_b04o15_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o15_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;
   int maxi = 1;
   int lowerB = 90;
   int upperB = 90;
   int actualB;
   int vel;

	res = ds_objects_lib_beh_launcherMovementLR(object, 
									 0,0, // Never change speed, direction
									 8, 4, 1,
									 60, 0, 30, // Change state every second, launch at 0.5 sec
									 7, 14, 15, 16,
									 0, // NO cycle gesture
									 1, // Follow Juni
									 3);

   if (res) {
      // Launch particles!
      switch (object->inner[2]) { // Direction
         case -1: // Left
         	lowerB = 124;
         	upperB = 146;
         	maxi = 3;
            break;
         case 1: // Right
         	lowerB = 34;
         	upperB = 56;
         	maxi = 2;
            break;
      }   
      actualB = PA_RandMinMax(lowerB,upperB);
      vel = 20 + 
		      (ds_objects_lib_distanceJuniX(object,1,1) / 5) +
		      (ds_objects_lib_distanceJuniY(object,1,1) / 5);
	   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 45);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					vel * ds_util_fastCos(actualB), 
					-vel * ds_util_fastSin(actualB),
			 		10, 0, 3);	     
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   

   // Everything went OK...
   return 1;
}

// CARTERPILLAR [B04_O16]
//..........................................................................................
int _ds_objects_b04o16_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o16_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res = ds_objects_lib_beh_launcherMovementLR(object, 
									 0,0, // Never change speed, direction
									 8, 4, 1,
									 60, 0, 30, // Change state every second, launch at 0.5 sec
									 8, 16, 20, 24,
									 0, // NO cycle gesture
									 1, // Follow Juni
									 3);

	if (res) {
	   particle = ds_objects_createParticle(object->x + 6, object->y - 12, object->layer, 12);
	   ds_objects_lib_beh_particleMMF2_Init(particle,0,-20,2,0,3);
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   

   // Everything went OK...
   return 1;
}

// HEDGEHOD [B04_O17]
//..........................................................................................
int _ds_objects_b04o17_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o17_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object, 1, // X
									36,36, // "60, change speed
									0,2,5, // 0-5-10
									0,0,0,0, // Basic Gesture (no special)
									120,100, // 2", change dir
									1, // No Gesture
									0, // DON'T FOLLOW Juni
									3,
									0,
									0);

   // Everything went OK...
   return 1;
}                            

// KILLER BREAD! [B04_O18]
//..........................................................................................
int _ds_objects_b04o18_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	// Not hatmful...yet
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
	object->inner[0] = 0;
	object->inner[1] = 0;
	object->inner[8] = 0;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o18_manage(void *objp) {
   ds_t_object *object = objp;
   
   // If Juni near... harm!
   _ds_objects_b04_waitHarm(object);
   
   // Everything went OK...
   return 1;
}

// 3-SHOOTER [B04_O19]
//..........................................................................................
int _ds_objects_b04o19_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o19_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;
   int actualB = 90;
   int vel;
   int i;

	res = ds_objects_lib_beh_launcherMovementLR(object, 
									 47, 120, // change speed at 0"76, face player at 2 seconds
									 0, 10, 1,
									 0, 110, 60, // Distance 140, launch at 1 sec. Correction to be DS-Friendly
									 7, 7, 11, 11,
									 0, // NO cycle gesture
									 1, // Follow Juni
									 3);

	if (res) {
	   for (i = 0; i < 3; i++) {
	      if (i==0)
	      	actualB = 45;
	      else if (i==1)
	      	actualB = 90;
	      else 
	      	actualB = 135;
	      vel = PA_RandMinMax(30,59);
		   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 39);
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						vel * ds_util_fastCos(actualB), 
					   -vel * ds_util_fastSin(actualB),
				 		10, 0, 3);	     
	   }   
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   
									 
   // Everything went OK...
   return 1;
}                      

// 5-SHOOTER [B04_O20]
//..........................................................................................
int _ds_objects_b04o20_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b04o20_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;
   int actualB;
   int vel;
   int i;

	res = ds_objects_lib_beh_launcherMovementLR(object, 
									 50, 114, // change speed at 0"84, face player at 1"90 seconds
									 0, 10, 1,
									 0, 110, 60, // Distance 140, launch at 1 sec. Correction to be DS-Friendly
									 7, 7, 11, 11,
									 0, // NO cycle gesture
									 0, // DON'T Follow Juni
									 3);

	if (res) {
	   for (i = 0; i < 5; i++) {
	      if (i==0)
	      	actualB = 45;
	      else if (i==1)
	      	actualB = 78;
	      else if (i==2)
	      	actualB = 90;
	      else if (i==3)
	      	actualB = 102;
	      else
	      	actualB = 135;

	      vel = PA_RandMinMax(30,59);
		   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 39);
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						vel * ds_util_fastCos(actualB), 
					   -vel * ds_util_fastSin(actualB),
				 		10, 0, 3);	     
	   }   
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	} 
									 
   // Everything went OK...
   return 1;
}

// RIGHT SPITTER [B04_O21]
//..........................................................................................
int _ds_objects_b04o21_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b04o21_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res = ds_objects_lib_beh_disk(object, 0, // Y
									 0, // Never change speed
									 9, 1, 0, // Speed 9 (1pix/60fps)
									 129, // 215 / 1.6 (Same as instance change :-P)
									 3, 
									 6, 8);

	if (res == 1) {
	   particle = ds_objects_createParticle(object->x, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					-20, 
					0, 
			 		0, 0, 3);
	   particle = ds_objects_createParticle(object->x, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					-20, 
					8, 
			 		0, 0, 3);
	   particle = ds_objects_createParticle(object->x, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					-20, 
					-8, 
			 		0, 0, 3);
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}

   return 1;
}

int _ds_objects_b04o21_instance(void *objp) {
   ds_t_object *object = objp;
   
   if ((ds_global_tick % 129) == 0) // 215 / 1.6
   	object->inner[0] = 2;
      
   return 1;
}

// LEFT SPITTER [B04_O22]
//..........................................................................................
int _ds_objects_b04o22_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b04o22_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res = ds_objects_lib_beh_disk(object, 0, // Y
									 0, // Never change speed
									 9, 1, 0, // Speed 9 (1pix/60fps)
									 129, // 215 / 1.6 (Same as instance change :-P)
									 3, 
									 6, 8);
									 
	if (res == 1) {
	   particle = ds_objects_createParticle(object->x + 8, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					20, 
					0, 
			 		0, 0, 3);
	   particle = ds_objects_createParticle(object->x + 8, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					20, 
					8, 
			 		0, 0, 3);
	   particle = ds_objects_createParticle(object->x + 8, object->y, object->layer, 44);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					20, 
					-8, 
			 		0, 0, 3);
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   

   return 1;
}

int _ds_objects_b04o22_instance(void *objp) {
   ds_t_object *object = objp;
   
   if ((ds_global_tick % 129) == 0) // 215 / 1.6
   	object->inner[0] = 2;
      
   return 1;
}
                            

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects4_assign(u8 obj, ds_t_object *object) {
   switch (obj) {
      case 1:
         // Red Scorch
         object->fcreate = _ds_objects_b04o01_create;
         object->fmanage = _ds_objects_b04o01_manage;
         return 1;
         break; // not really necessary...
      case 2:
         // Blue Side
         object->fcreate = _ds_objects_b04o02_create;
         object->fmanage = _ds_objects_b04o02_manage;
         return 1;
         break; // not really necessary...
      case 3:
         // Fast Runner
         object->fcreate = _ds_objects_b04o03_create;
         object->fmanage = _ds_objects_b04o03_manage;
         return 1;
         break; // not really necessary...
      case 4:
         // Medium Runner
         object->fcreate = _ds_objects_b04o03_create;
         object->fmanage = _ds_objects_b04o04_manage;
         return 1;
         break; // not really necessary...
      case 5:
         // Baby Runner
         object->fcreate = _ds_objects_b04o03_create;
         object->fmanage = _ds_objects_b04o05_manage;
         return 1;
         break; // not really necessary...
      case 6:
         // Drop Crawler - Ground
         object->fcreate = _ds_objects_b04o06_create;
         object->fmanage = _ds_objects_b04o06_manage;
         return 1;
         break; // not really necessary...
      case 7:
         // Drop Crawler - Ceiling
         object->fcreate = _ds_objects_b04o07_create;
         object->fmanage = _ds_objects_b04o07_manage;
         return 1;
         break; // not really necessary...
      case 8:
         // Slow Siderunner
         object->fcreate = _ds_objects_b04o08_create;
         object->fmanage = _ds_objects_b04o08_manage;
         return 1;
         break; // not really necessary...
      case 9:
         // Spider
         object->fcreate = _ds_objects_b04o09_create;
         object->fmanage = _ds_objects_b04o09_manage;
         return 1;
         break; // not really necessary...
      case 10:
         // Left Fish
         object->fcreate = _ds_objects_b04o10_create;
         object->fmanage = _ds_objects_b04o10_manage;
         object->finstance = _ds_objects_b04o10_instance;
         return 1;
         break; // not really necessary...
      case 11:
         // Right Fish
         object->fcreate = _ds_objects_b04o11_create;
         object->fmanage = _ds_objects_b04o11_manage;
         object->finstance = _ds_objects_b04o11_instance;
         return 1;
         break; // not really necessary...
      case 12:
         // Yellow Scorch
         object->fcreate = _ds_objects_b04o12_create;
         object->fmanage = _ds_objects_b04o12_manage;
         return 1;
         break; // not really necessary...
      case 13:
         // Side Crawler
         object->fcreate = _ds_objects_b04o13_create;
         object->fmanage = _ds_objects_b04o13_manage;
         return 1;
         break; // not really necessary...
      case 14:
         // 3Leg
         object->fcreate = _ds_objects_b04o14_create;
         object->fmanage = _ds_objects_b04o14_manage;
         return 1;
         break; // not really necessary...
      case 15:
         // Red Scorch
         object->fcreate = _ds_objects_b04o15_create;
         object->fmanage = _ds_objects_b04o15_manage;
         return 1;
         break; // not really necessary...
      case 16:
         // Carterpillar
         object->fcreate = _ds_objects_b04o16_create;
         object->fmanage = _ds_objects_b04o16_manage;
         return 1;
         break; // not really necessary...
      case 17:
         // Hedgehod
         object->fcreate = _ds_objects_b04o17_create;
         object->fmanage = _ds_objects_b04o17_manage;
         return 1;
         break; // not really necessary...
      case 18:
         // Killer Bread
         object->fcreate = _ds_objects_b04o18_create;
         object->fmanage = _ds_objects_b04o18_manage;
         return 1;
         break; // not really necessary...
      case 19:
         // 3-Shooter
         object->fcreate = _ds_objects_b04o19_create;
         object->fmanage = _ds_objects_b04o19_manage;
         return 1;
         break; // not really necessary...
      case 20:
         // 5-Shooter
         object->fcreate = _ds_objects_b04o20_create;
         object->fmanage = _ds_objects_b04o20_manage;
         return 1;
         break; // not really necessary...
      case 21:
         // Right Spitter
         object->fcreate = _ds_objects_b04o21_create;
         object->fmanage = _ds_objects_b04o21_manage;
         object->finstance = _ds_objects_b04o21_instance;
         return 1;
         break; // not really necessary...
      case 22:
         // Left Spitter
         object->fcreate = _ds_objects_b04o22_create;
         object->fmanage = _ds_objects_b04o22_manage;
         object->finstance = _ds_objects_b04o22_instance;
         return 1;
         break; // not really necessary...
   }   
   return 0;
}   
