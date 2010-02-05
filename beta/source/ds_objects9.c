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
#include "ds_music.h"


// BANK 9 [B09]
//=================================================================================================

// ROSE [B09_O01]
//..........................................................................................
int _ds_objects_b09o01_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b09o01_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;

	if (ds_objects_lib_beh_flower(object,0,200,0,215,36,6)) {
		ds_music_playSound("Tiny Shot", 0, 0);
	   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 38);
	   ds_objects_lib_beh_particleMMF2_Init(particle, (PA_RandMinMax(0,1)==0)?-10:10, 0 - PA_RandMinMax(25,60), 
						8, 10, 0);
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);						

	}   
   
   return 1;
}

// TIMED FLOWER [B09_O02]
//..........................................................................................
int _ds_objects_b09o02_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b09o02_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;

	if (ds_objects_lib_beh_flower(object,1,160,1,160,10,6)) {
		ds_music_playSound("Tiny Shot", 0, 0);
	   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 38);
	   ds_objects_lib_beh_particleMMF2_Init(particle, (PA_RandMinMax(0,1)==0)?-10:10, 0 - PA_RandMinMax(25,60), 
						8, 20, 0);
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);				

	}   
   
   return 1;
}

// HOMING FLOWER [B09_O03]
//..........................................................................................
int _ds_objects_b09o03_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b09o03_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;

	if (ds_objects_lib_beh_flower(object,1,135,2,1,1,6)) {
		ds_music_playSound("Homing Shot", 0, 0);
	   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 12);
	   ds_objects_lib_beh_particleMMF2_Init(particle,0,-10,1,0,3);
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);				

	}   
   
   return 1;
}   

// AGGRESSIVE FLOWER [B09_O04]
//..........................................................................................
int _ds_objects_b09o04_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b09o04_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;

	if (ds_objects_lib_beh_flower(object,0,270,0,285,10,6)) {
		ds_music_playSound("Fire Shot", 0, 0);
	   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 39);
	   ds_objects_lib_beh_particleMMF2_Init(particle, -40 + PA_RandMinMax(0,80), 0 - PA_RandMinMax(25,70), 
						10, 0, 3);
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);				

	}   
   
   return 1;
}      

// SMALL FLOWER [B09_O05]
//..........................................................................................
int _ds_objects_b09o05_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b09o05_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
	int oldstate = object->inner[0];

	if (ds_objects_lib_beh_flower(object,1,146,2,10,1,6)) {
		if ((object->inner[0] == 4) && (object->inner[0] != oldstate)) {
			ds_music_playSound("Tiny Shot", 0, 0);
		}
	   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 38);
	   ds_objects_lib_beh_particleMMF2_Init(particle, -30 + PA_RandMinMax(0,60), 0 - PA_RandMinMax(20,60), 
						8, 10, 0);
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);				

	}   
   
   return 1;
}   

   
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects9_assign(u8 obj, ds_t_object *object) {
   switch (obj) {
      case 1:
         // Rose
         object->fcreate = _ds_objects_b09o01_create;
         object->fmanage = _ds_objects_b09o01_manage;
         return 1;
         break; // not really necessary...            
      case 2:
         // Timed Flower
         object->fcreate = _ds_objects_b09o02_create;
         object->fmanage = _ds_objects_b09o02_manage;
         return 1;
         break; // not really necessary...            
      case 3:
         // Homing flower
         object->fcreate = _ds_objects_b09o03_create;
         object->fmanage = _ds_objects_b09o03_manage;
         return 1;
         break; // not really necessary...            
      case 4:
         // Aggressive flower
         object->fcreate = _ds_objects_b09o04_create;
         object->fmanage = _ds_objects_b09o04_manage;
         return 1;
         break; // not really necessary...            
      case 5:
         // Small Flower
         object->fcreate = _ds_objects_b09o05_create;
         object->fmanage = _ds_objects_b09o05_manage;
         return 1;
         break; // not really necessary...            
   }
	return 0;   
}
