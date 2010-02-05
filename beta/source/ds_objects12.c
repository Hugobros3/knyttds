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
#include "ds_util.h"
#include "ds_world.h"
#include "ds_3dsprite.h"
#include "ds_map.h"
#include "ds_objects.h"
#include "ds_music.h"


// BANK 12 [B12]
//=================================================================================================

// GHOST A [B12_O01]
//..........................................................................................
int _ds_objects_b12o01_create(u8 bank, u8 obj, void *objp) {
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

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o01_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Move, Normal/Float Style ;-)
  	ds_objects_lib_beh_moveLeftRight(object,2,2,0,100,24,24*6,0*60,0*60,2,1,0,-1,1,0);   
	ds_objects_lib_beh_ghostFloat(object);
   
   // Everything went OK...
   return 1;
}

// GHOST B [B12_O02]
//..........................................................................................
int _ds_objects_b12o02_create(u8 bank, u8 obj, void *objp) {
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

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o02_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Move, Normal/Float Style ;-)
  	ds_objects_lib_beh_moveLeftRight(object,2,2,0,100,24,24*6,0*60,0*60,2,1,0,-1,1,0);   
  	ds_objects_lib_beh_ghostFloat(object);
   
   // Everything went OK...
   return 1;
}

// GHOST MUFF [B12_O03]
//..........................................................................................
int _ds_objects_b12o03_create(u8 bank, u8 obj, void *objp) {
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

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o03_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Move, Normal Style ;-)
  	ds_objects_lib_beh_moveLeftRight(object,2,3,0,70,24,128,1*60,2*60,2,1,0,-1,1,0);   
   
   // Everything went OK...
   return 1;
}

// GHOST BLOCK [B12_O05]
//..........................................................................................
int _ds_objects_b12o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_COLLIDE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o05_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Everything went OK...
   return 1;
}

// GHOST SLIME DROP [B12_O06]
//..........................................................................................
int _ds_objects_b12o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
  	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o06_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Ghost Behaviour - Move
	ds_objects_lib_beh_diskMovementLR(object, 1, 
									 -1, -1, // Constant Speed
									 6, 1, 0, // Speed 6
									 0, 0, 0, 0,
									 60, 10, // Change direction... rare
									 0,
									 0,
									 3,
									 0,
									 0);
									 
	// Internal Flags
	if ((ds_global_tick % 180) == 0)
		object->inner[11] = (object->inner[11] == 0)?1:0;
											    
   // Everything went OK...
   return 1;
}

int _ds_objects_b12o06_instance(void *objp) {
   ds_t_object *object = objp;   
   ds_t_object *particle;
   int chance;
		
   // Create Particle?
   if (object->inner[11]) {
      chance = ((ds_global_tick % 12) == 0); // Every 0.2
   } else {
      chance = ((ds_global_tick % 37) == 0); // Every 0.62
	}      
	if (chance) {
	   // Particle!
	   particle = ds_objects_createParticle(object->x + PA_RandMax(32), 
										object->y - 8, object->layer, 
										56);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
				0, 
				5, 
		 		5, 0, 3);
	} 		
	
	return 1;
}

// GHOST D [B12_O07]
//..........................................................................................
int _ds_objects_b12o07_create(u8 bank, u8 obj, void *objp) {
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

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o07_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Look juni, Float Style ;-)
  	ds_objects_lib_beh_cycleLook(object,3);   
  	ds_objects_lib_beh_ghostFloat(object);
   
   // Everything went OK...
   return 1;
}   

// GHOST E [B12_O08]
//..........................................................................................
int _ds_objects_b12o08_create(u8 bank, u8 obj, void *objp) {
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

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o08_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Cycles
  	ds_objects_lib_beh_cycle(object,4);   
   
   // Everything went OK...
   return 1;
}   

// GHOST SLIME [B12_O09]
//..........................................................................................
int _ds_objects_b12o09_create(u8 bank, u8 obj, void *objp) {
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

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o09_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Ghost Behaviour - Move
	ds_objects_lib_beh_diskMovementLR(object, 1, 
									 -1, -1, // Constant Speed
									 6, 1, 0, // Speed 6
									 0, 0, 0, 0,
									 60, 10, // Change direction... rare
									 0,
									 0,
									 3,
									 0,
									 0);
											    
   // Everything went OK...
   return 1;
}

// GHOST EATER [B12_O10]
//..........................................................................................

int _ds_objects_b12o10_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,90,90,0);

   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b12o10_manage(void *objp) {
   ds_t_object *object = objp;

	switch (object->inner[10]) {
	   case 0:
	      if (ds_objects_lib_stepObject(object)) {
	      	object->inner[10] = 1;
	      	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
			ds_music_playSound("Chomp", 0, 0);
	    	}  	
	      break;
	   case 1:
	      if (ds_objects_lib_beh_anim(object,2,1)) {
	         object->inner[10] = 2;
	         object->y -= (object->ys >> 1);
	         ds_3dsprite_setY(object->sprite,object->y);
	      }   
	      break;
	   case 2:
	      ds_objects_lib_beh_anim(object,2,1);
	      break;
	}      

   return 1;
}
   

// GHOST [B12_O11]
//..........................................................................................
int _ds_objects_b12o11_create(u8 bank, u8 obj, void *objp) {
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

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
	ds_objects_lib_beh_floatLeftRight_ini(object,1,1);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o11_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Move, ghost style ;-)
   ds_objects_lib_beh_floatLeftRight(object,2,2,20,48,120,600,0,0,2,2,
													0,0);
   
   // Everything went OK...
   return 1;
}

// GHOST DROP [B12_O12]
//..........................................................................................
int _ds_objects_b12o12_create(u8 bank, u8 obj, void *objp) {
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

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
	object->inner[10] = 45 + PA_RandMinMax(0,45);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o12_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Drop!
   switch (object->inner[0]) {
      case 0:
         // Drop?
			object->inner[10]--;
			if (object->inner[10] <= 0) {
			   // Reset
			   object->inner[10] = 40 + PA_RandMinMax(0,40);
			   // Next State
			   object->inner[0] = 1;
			   object->inner[1] = 0;
			   object->inner[9] = 3;
			}      
			break;
		case 1:
		   // Drop! - Move
         if (object->inner[9] > 0)
         	object->inner[9]--;
         if (object->inner[9] <= 0) {
            object->inner[1]++;
		      if (object->inner[1] >= ds_3dsprite_getMaxFrame(object->sprite)) {
		         object->inner[0] = 0; // Old state
		      	object->inner[1] = 0; // Old sprite
		    	}  	
		   	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
            object->inner[9] = 3;
			}   
			// Drop! - Drop
			if ((object->inner[1] == 5) && (object->inner[9] == 3)) {
			   // Create Particle
	         particle = ds_objects_createParticle(object->x + 8, 
												object->y - 7, object->layer, 
												56);
			   ds_objects_lib_beh_particleMMF2_Init(particle, 
						0, 
						5, 
				 		5, 0, 3);
			}   
		   break;
	}		
   											    
   // Everything went OK...
   return 1;
}

// GHOST SPIKER [B12_O13]
//..........................................................................................
int _ds_objects_b12o13_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o13_manage(void *objp) {
   ds_t_object *object = objp;
   int dir;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Moves
	switch (object->inner[0]) {
	   case 0:
	      // Wait...
	      if (ds_objects_lib_distanceJuniY(object,1,1) <= 4)
	      	object->inner[0] = 1;
	      break;
	   case 1:
	      // Move!
	      dir = (ds_objects_lib_distanceJuniX(object,1,0) < 0)?-1:1;
	      if (object->inner[2] != dir) {
	         object->inner[2] = dir;
	         object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], 1);
	         ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	         object->inner[9] = 3;
	      }   
	      object->x += (3 * object->inner[2]);
	      ds_3dsprite_setX(object->sprite,object->x);
			// Sprite...
	      if (object->inner[9] > 0)
	        	object->inner[9]--;
	      if (object->inner[9] <= 0) {
	         object->inner[1]++;
			   if (object->inner[1] >= ds_3dsprite_getMaxFrame_LR(object->sprite, object->inner[2], 1))
			   	object->inner[1] = ds_3dsprite_getIniFrame_LR(object->sprite, object->inner[2], 1);
				ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	         object->inner[9] = 3;
			}			   
	      // Stop?
	      if (!(ds_objects_lib_distanceJuniY(object,1,1) <= 4))
	      	object->inner[0] = 0;
	      break;
	}	
      											    
   // Everything went OK...
   return 1;
}

// GHOST TRAP [B12_O14]
//..........................................................................................
int _ds_objects_b12o14_create(u8 bank, u8 obj, void *objp) {
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

	// Ghost operations - just test
  	object->_deleteme = !(ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_EYE));
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o14_manage(void *objp) {
   ds_t_object *object = objp;
   
   // No Changes in alpha value for this trap :-)
   
   // Appear?
   if (ds_objects_lib_beh_appear(object, 60, 0, 1, 3, 1)) {
		ds_music_playSound("Ghost Rock", 0, 0);
   }

   // Everything went OK...
   return 1;
}

// BOO GHOST (Not real name, but it is similar) [B12_O15]
//..........................................................................................
int _ds_objects_b12o15_create(u8 bank, u8 obj, void *objp) {
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

	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b12o15_manage(void *objp) {
   ds_t_object *object = objp;
   int testx;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);
   
   // Moves
	switch (object->inner[0]) {
	   case 0:
	      // Ini
	      object->inner[0] = 1;
	      object->inner[2] = (PA_RandMax(99) < 50)?-1:1;
	      break;
	   case 1: // Normal
	      // Change Direction
	      testx = object->x + 12;
	      if ((object->inner[2] == 1)  && (testx >= 590)) {
	         object->inner[2] = -1;
	      } else if ((object->inner[2] == -1) && (testx <= 10)) {
	         object->inner[2] = 1;
	      }   
	      // Normal Move!
	      object->x += (2 * object->inner[2]);
	      ds_3dsprite_setX(object->sprite,object->x);
			// Sprite...
	      if (object->inner[9] > 0)
	        	object->inner[9]--;
	      if (object->inner[9] <= 0) {
	         object->inner[1]++;
			   if (object->inner[1] >= ((object->inner[2] == 1)?6:12))
			   	object->inner[1] = (object->inner[2] == 1)?0:6;
				ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	         object->inner[9] = 3;
			}			   
	      // DiveBomb!
  	      if (ds_objects_lib_distanceJuniX(object,1,1) <= 100) {
  	      	object->inner[7] = object->y;
  	      	object->inner[6] = 36;
	      	object->inner[0] = 2;
			   object->inner[1] = (object->inner[2] == 1)?12:18;
			   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	    	}  	
	      break;
	   case 2: // Divebomb
	      // Change Direction
	      testx = object->x + 12;
	      if ((object->inner[2] == 1)  && (testx >= 590)) {
	         object->inner[2] = -1;
	      } else if ((object->inner[2] == -1) && (testx <= 10)) {
	         object->inner[2] = 1;
	      }   
	      // Normal Move!
	      object->x += (2 * object->inner[2]);
	      object->y += (object->inner[6] >> 3);
	      ds_3dsprite_setX(object->sprite,object->x);
	      ds_3dsprite_setY(object->sprite,object->y);
			// Sprite...
	      if (object->inner[9] > 0)
	        	object->inner[9]--;
	      if (object->inner[9] <= 0) {
	         object->inner[1]++;
			   if (object->inner[1] >= ((object->inner[2] == 1)?18:24))
			   	object->inner[1] = (object->inner[2] == 1)?12:18;
				ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	         object->inner[9] = 3;
			}			   
	      // Ending the DiveBomb
	      object->inner[6]--;
  	      if (object->y < object->inner[7]) {
  	      	object->y = object->inner[7];
  	      	ds_3dsprite_setY(object->sprite,object->y);
	      	object->inner[0] = 1;
			   object->inner[1] = (object->inner[2] == 1)?0:6;
			   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	    	}  	
	      break;
	}
   
   // Everything went OK...
   return 1;
}

// GHOST BIRDIE [B12_O16]
//..........................................................................................
int _ds_objects_b12o16_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
	// Ghost operations
	ds_objects_lib_beh_ghostIni(object,1,30,120,30);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int ds_b12_FlyX = 25;
int ds_b12_FlyXS[] = {   8,  8,  8,  8,  8,  8,  8,  8,  7,  5,  3,  3,  8,  5,   7,   8,   8,   8,  7,  5,  4,  3,  3,  5, 180};
int ds_b12_FlyXMX[] = {323,304,278,238,167,108, 57, 29, 11,  4,  1,  5, 15, 35,  74, 141, 240, 298,350,365,371,372,368,357, 340};
int ds_b12_FlyXMY[] = {  0,-13,-24,-30,-28,-20,-17,-21,-28,-38,-52,-66,-80,-92,-107,-118,-112, -90,-77,-66,-53,-40,-30,-22, -12};

int _ds_objects_b12o16_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Changes the alpha value of the sprite, ghost style :-)
   ds_objects_lib_beh_ghost(object,30,120,30);

	// Follow path   
	ds_objects_lib_beh_followPixelPathLR(object, ds_b12_FlyX, 
											ds_b12_FlyXS, ds_b12_FlyXMX, ds_b12_FlyXMY,
											3,
											1);
   return 1;
}

// X-BLOCK [B12_O17]
//..........................................................................................
int _ds_objects_b12o17_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;

	// Ghost Block operations
	if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_EYE)) {
	   // I can pass through this place!!!!
		ds_map_copyFlagValue(0,object->xs,object->ys,object->x,object->y, &ds_global_map.tileMapCol);
	}   
	object->_deleteme = 1; // Alwayss delete...	
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}      

// HINT BLOCK [B12_O18] ... [B12_O21]
//..........................................................................................
int _ds_objects_b12hint_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;

	// Ghost Block operations
	if (!ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_EYE)) {
		object->_deleteme = 1; // Delete...	
	}		
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}      

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects12_assign(u8 obj, ds_t_object *object) {
   if ((obj >= 18) && (obj <= 21)) {
      // "Hint" Blocks
      object->fcreate = _ds_objects_b12hint_create;
      return 1;
   }   
   switch (obj) {
      case 1:
         // Ghost A
         object->fcreate = _ds_objects_b12o01_create;
         object->fmanage = _ds_objects_b12o01_manage;
         return 1;
         break; // not really necessary...
      case 2:
         // Ghost B
         object->fcreate = _ds_objects_b12o02_create;
         object->fmanage = _ds_objects_b12o02_manage;
         return 1;
         break; // not really necessary...
      case 3:
         // Ghost Muff
         object->fcreate = _ds_objects_b12o03_create;
         object->fmanage = _ds_objects_b12o03_manage;
         return 1;
         break; // not really necessary...
      case 5:
         // Ghost Block
         object->fcreate = _ds_objects_b12o05_create;
         object->fmanage = _ds_objects_b12o05_manage;
         return 1;
         break; // not really necessary...
      case 4:
         // Ghost C (= D)
         object->fcreate = _ds_objects_b12o07_create;
         object->fmanage = _ds_objects_b12o07_manage;
         return 1;
         break; // not really necessary...
      case 6:
         // Ghost Slime Drop
         object->fcreate = _ds_objects_b12o06_create;
         object->fmanage = _ds_objects_b12o06_manage;
      	object->finstance = _ds_objects_b12o06_instance;
         return 1;
         break; // not really necessary...
      case 7:
         // Ghost D
         object->fcreate = _ds_objects_b12o07_create;
         object->fmanage = _ds_objects_b12o07_manage;
         return 1;
         break; // not really necessary...
      case 8:
         // Ghost E
         object->fcreate = _ds_objects_b12o08_create;
         object->fmanage = _ds_objects_b12o08_manage;
         return 1;
         break; // not really necessary...
      case 9:
         // Ghost Slime
         object->fcreate = _ds_objects_b12o09_create;
         object->fmanage = _ds_objects_b12o09_manage;
         return 1;
         break; // not really necessary...
      case 10:
         // Ghost Eater
         object->fcreate = _ds_objects_b12o10_create;
         object->fmanage = _ds_objects_b12o10_manage;
         return 1;
         break; // not really necessary...
      case 11:
         // Ghost
         object->fcreate = _ds_objects_b12o11_create;
         object->fmanage = _ds_objects_b12o11_manage;
         return 1;
         break; // not really necessary...         
      case 12:
         // Ghost Drop
         object->fcreate = _ds_objects_b12o12_create;
         object->fmanage = _ds_objects_b12o12_manage;
         return 1;
         break; // not really necessary...         
      case 13:
         // Ghost Spiker
         object->fcreate = _ds_objects_b12o13_create;
         object->fmanage = _ds_objects_b12o13_manage;
         return 1;
         break; // not really necessary...         
      case 14:
         // Ghost Trap
         object->fcreate = _ds_objects_b12o14_create;
         object->fmanage = _ds_objects_b12o14_manage;
         return 1;
         break; // not really necessary...         
      case 15:
         // Boo Ghost
         object->fcreate = _ds_objects_b12o15_create;
         object->fmanage = _ds_objects_b12o15_manage;
         return 1;
         break; // not really necessary...         
      case 16:
         // Ghost Birdie
         object->fcreate = _ds_objects_b12o16_create;
         object->fmanage = _ds_objects_b12o16_manage;
         return 1;
         break; // not really necessary...         
      case 17:
         // X-Block
         object->fcreate = _ds_objects_b12o17_create;
         return 1;
         break; // not really necessary...         
      }   		      
   return 0;
}   
