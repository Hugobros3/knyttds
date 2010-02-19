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


// BANK 1 [B01]
//=================================================================================================


int _ds_objects_o01_manageWaterNormal_ini(ds_t_object *object) {
   object->inner[2] = 0;
   
   return 1;
}   

int _ds_objects_o01_manageWaterNormal(ds_t_object *object, int tick) {
/* 
	[2]:ACTUAL FRAME
*/ 
	if ((ds_global_tick % tick) == 0) {
		object->inner[2] = (object->inner[2] + 1) % ds_3dsprite_getMaxFrame(object->sprite);
	}		
	ds_3dsprite_setFrame(object->sprite,object->inner[2]);
//	ds_3dsprite_updateSprite(object->sprite);
	return 1;
}

int _ds_objects_o01_manageWaterfall_ini(ds_t_object *object) {
   object->inner[2] = 0;
	object->inner[3] = ds_3dsprite_getMaxFrame(object->sprite);

	ds_3dsprite_setAlpha(object->sprite, 128);
   
   return 1;
}   

int _ds_objects_o01_manageWaterfall(ds_t_object *object, int tick) {
/* 
	[2]:ACTUAL FRAME
	[3]:MAX FRAME
*/ 
	if ((ds_global_tick % tick) == 0) {
		object->inner[2] = (object->inner[2] + 1) % object->inner[3];
		ds_3dsprite_setFrame(object->sprite,object->inner[2]);
	}		
	return 1;
}

// WATERFALL [B01_O03] <- e.g.
//..........................................................................................
int _ds_objects_b01oWaterfall_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	_ds_objects_o01_manageWaterfall_ini(object);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b01oWaterfall_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Just cycle...
	_ds_objects_o01_manageWaterfall(object,2);
	
   // Everything went OK...
   return 1;
}   

// WATER TOP <NON LETHAL> [B01_O06] <- e.g.
//..........................................................................................
int _ds_objects_b01oWaterTop_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	// For optimizing, uncomment
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	_ds_objects_o01_manageWaterNormal_ini(object);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b01oWaterTop_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Manage the water!
   return _ds_objects_o01_manageWaterNormal(object,5);
}   

// STILL WATER [B01_O01] <- e.g.
//..........................................................................................
int _ds_objects_b01oStillWater_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	// For optimizing, uncomment
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b01oStillWater_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Manage the water!
	return ds_objects_lib_beh_cycleBackForth(object, 7); // 5 + Rnd(0,5)
}   

// STILL DEEP WATER [B01_O07] <- e.g.
//..........................................................................................
int _ds_objects_b01oStillDeepWater_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE);
	//object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_STATICHARMFUL); NO!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_FULLCOLLIDE);
	
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 0; // It is not managed!
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

// LAVA-Like [B01_O05] <- e.g.
//..........................................................................................
int _ds_objects_b01oLava_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	// For optimizing, uncomment
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b01oLava_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Manage the water!
	return ds_objects_lib_beh_cycle(object, 5);
}   

// BUBBLES [B01_O18]
//..........................................................................................
int _ds_objects_b01oBubble_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   object->inner[0] = -1; // Y position
   object->inner[1] = 32; // Alpha
   object->inner[2] = PA_RandMax(32); // Waiting Appear time
   object->inner[9] = object->x; // Original X position
   object->inner[10] = 0; // Waiting management time
   ds_3dsprite_setAlpha(object->sprite,object->inner[1]);
   ds_3dsprite_setX(object->sprite,300);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b01oBubble_manage(void *objp) {
   ds_t_object *object = objp;
   
   if (object->inner[10] > 0)
   	object->inner[10]--;
   if (object->inner[10] <= 0) {
      object->inner[10] = 7;

	   // Move the bubble and change the alpha
	   if (object->inner[0] == -1) {
			object->inner[2]--;
			if (object->inner[2] <= 0) {
			   object->inner[0] = 0;
			   ds_3dsprite_setX(object->sprite,object->inner[9]);
			}   
	   } else if (object->inner[0] < 8) {
			object->inner[1] += 24;      
			object->inner[0]++;
	   } else if (object->inner[0] < 16) {
	      object->inner[1] -= 24;
	      object->inner[0]++;
		} else {
		   ds_3dsprite_setX(object->sprite,300);
		   object->inner[0] = -1;
		   object->inner[1] = 32;
		   object->inner[2] = PA_RandMinMax(1,100);
		}       
		
		// Basic behaviour 
	   ds_3dsprite_setY(object->sprite,object->y - object->inner[0]);
		ds_3dsprite_setAlpha(object->sprite,object->inner[1]);
	}		
	   
   return 1;
}   

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects1_assign(u8 obj, ds_t_object *object) {
   if (
	     (obj == 7) || (obj == 8) || (obj == 9) ||
	     (obj == 11) || (obj == 17) || (obj == 21) || (obj == 24)
	   ) {
         object->fcreate = _ds_objects_b01oStillDeepWater_create;
         object->fmanage = NULL;
         return 1;	   
	} else 
   if (
	     (obj == 1) || (obj == 2) || (obj == 10) || (obj == 12) ||
	     (obj == 14) || (obj == 19) || (obj == 22)
	   ) {
         object->fcreate = _ds_objects_b01oStillWater_create;
         object->fmanage = _ds_objects_b01oStillWater_manage;
         return 1;	   
	} else 
   if (
	     (obj == 6) || (obj == 13) || (obj == 16)
	   ) {
         object->fcreate = _ds_objects_b01oWaterTop_create;
         object->fmanage = _ds_objects_b01oWaterTop_manage;
         return 1;	   
	} else 
   if (
	     (obj == 3) || (obj == 4) || (obj == 15) ||
		  (obj == 20) || (obj == 23)
	   ) {
         object->fcreate = _ds_objects_b01oWaterfall_create;
         object->fmanage = _ds_objects_b01oWaterfall_manage;
         return 1;	   
	} else 
   if (
	     (obj == 5)
	   ) {
         object->fcreate = _ds_objects_b01oLava_create;
         object->fmanage = _ds_objects_b01oLava_manage;
         return 1;	   
	} else 
   if (
	     (obj == 18)
	   ) {
         object->fcreate = _ds_objects_b01oBubble_create;
         object->fmanage = _ds_objects_b01oBubble_manage;
         return 1;	   
	}	  
   return 0;
}   
