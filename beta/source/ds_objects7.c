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
#include "ds_juni.h"
#include "ds_objects.h"
#include "ds_3dsprite.h"


// BANK 7 [B07]
//=================================================================================================

// POURING FALLING LEAVES [B07_O01]
//..........................................................................................
int _ds_objects_b07o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->inner[0] = 0;
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o01_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create falling leaves...
   if ((ds_global_tick % 50) == 0) {
      ds_objects_createParticle(object->x + PA_RandMax(23), object->y, object->layer, 20);
   }   
   
   // Everything went OK...
   return 1;
}

// FALLING LEAVE [B07_O02]
//..........................................................................................
int _ds_objects_b07o02_create(u8 bank, u8 obj, void *objp) {
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
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o02_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create *A* falling leave, and kills itself...
   ds_objects_createParticle(object->x, object->y, object->layer, 20);
   
   // Everything went OK... too OK... (kills itself)
   object->_deleteme = 1;
   return 1;
}

// POURING FALLING SNOW [B07_O03]
//..........................................................................................
int _ds_objects_b07o03_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->inner[0] = 0;
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o03_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create falling snow...
	if ((ds_global_tick % 50) == 0) {
      ds_objects_createParticle(object->x + PA_RandMax(23), object->y - 12, object->layer, 21);
   }   
   
   // Everything went OK...
   return 1;
}

// FALLING SNOW [B07_O04]
//..........................................................................................
int _ds_objects_b07o04_create(u8 bank, u8 obj, void *objp) {
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
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o04_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create *A* falling leave, and kills itself...
   ds_objects_createParticle(object->x, object->y, object->layer, 21);
   
   // Everything went OK... too OK... (kills itself)
   object->_deleteme = 1;
   return 1;
}            

// RAY OF LIGHT [B07_O05]
//..........................................................................................
int _ds_objects_b07o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_ghostIni(object, 0, 30, 120, 30);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o05_manage(void *objp) {
   ds_t_object *object = objp;
   
   ds_objects_lib_beh_ghost(object, 30, 120, 30);
   
   return 1;
}            
 
// POURING PURPLE LEAVES [B07_O06]
//..........................................................................................
int _ds_objects_b07o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->inner[0] = 0;
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o06_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create falling leaves...
	if ((ds_global_tick % 50) == 0) {
      int rnd = PA_RandMinMax(0,2);
      ds_objects_createParticle(object->x + PA_RandMax(23), object->y, object->layer, 23 + (3*rnd));
   }   
   
   // Everything went OK...
   return 1;
}

// PURPLE LEAVE [B07_O07]
//..........................................................................................
int _ds_objects_b07o07_create(u8 bank, u8 obj, void *objp) {
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
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o07_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create *A* falling leave, and kills itself...
   ds_objects_createParticle(object->x, object->y, object->layer, 23);
   
   // Everything went OK... too OK... (kills itself)
   object->_deleteme = 1;
   return 1;
}           

// POURING RAIN [B07_O08]
//..........................................................................................
int _ds_objects_b07o08_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->inner[0] = 0;
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o08_manage(void *objp) {
   ds_t_object *object = objp;
   int part = PA_RandMax(2);
   
   // Create drops of rain...
   object->inner[0]--;
   if (object->inner[0] <= 0) {
      ds_objects_createParticle(object->x + PA_RandMax(23), object->y - 12, object->layer, part);
      object->inner[0] = 20 + PA_RandMax(20); // Less rain!...
   }   
   
   // Everything went OK...
   return 1;
}      

// RAIN DROP [B07_O09]
//..........................................................................................
int _ds_objects_b07o09_create(u8 bank, u8 obj, void *objp) {
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
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o09_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create *A* drop of rain, and kills itself...
   ds_objects_createParticle(object->x, object->y, object->layer, 0);
   
   // Everything went OK... too OK... (kills itself)
   object->_deleteme = 1;
   return 1;
}

// POURING GREEN LEAVES [B07_O10]
//..........................................................................................
int _ds_objects_b07o10_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->inner[0] = 0;
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o10_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create falling leaves...
   if ((ds_global_tick % 50) == 0) {
      int rnd = PA_RandMinMax(0,2);
      ds_objects_createParticle(object->x + PA_RandMax(23), object->y, object->layer, 22 + (3*rnd));
   }   
   
   // Everything went OK...
   return 1;
}

// GREEN LEAVE [B07_O11]
//..........................................................................................
int _ds_objects_b07o11_create(u8 bank, u8 obj, void *objp) {
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
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o11_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create *A* falling leave, and kills itself...
   ds_objects_createParticle(object->x, object->y, object->layer, 22);
   
   // Everything went OK... too OK... (kills itself)
   object->_deleteme = 1;
   return 1;
}           

// POURING WHITE LEAVES [B07_O12]
//..........................................................................................
int _ds_objects_b07o12_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->inner[0] = 0;
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o12_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create falling leaves...
   if ((ds_global_tick % 30) == 0) { // White produces much more leaves
      int rnd = PA_RandMinMax(0,2);
      ds_objects_createParticle(object->x + PA_RandMax(23), object->y, object->layer, 24 + (3*rnd));
   }   
   
   // Everything went OK...
   return 1;
}

// WHITE LEAVE [B07_O13]
//..........................................................................................
int _ds_objects_b07o13_create(u8 bank, u8 obj, void *objp) {
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
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o13_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create *A* falling leave, and kills itself...
   ds_objects_createParticle(object->x, object->y, object->layer, 24);
   
   // Everything went OK... too OK... (kills itself)
   object->_deleteme = 1;
   return 1;
}           
      
// POURING DUST [B07_O14]
//..........................................................................................
int _ds_objects_b07o14_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->inner[0] = 0;
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o14_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create dust...
   if ((ds_global_tick % 30) == 0) { // Lots of dust!
      ds_objects_createParticle(object->x + PA_RandMax(23), object->y, object->layer, 31);
   }   
   
   // Everything went OK...
   return 1;
}

// DUST [B07_O15]
//..........................................................................................
int _ds_objects_b07o15_create(u8 bank, u8 obj, void *objp) {
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
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o15_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Create *A* dust, and kills itself...
   ds_objects_createParticle(object->x, object->y, object->layer, 31);
   
   // Everything went OK... too OK... (kills itself)
   object->_deleteme = 1;
   return 1;
}           

// CLOUD [B07_O16]
//..........................................................................................
int _ds_objects_b07o16_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
      
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b07o16_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;

   // Check if Juni is stepping on this tile
   if (((ds_global_juni.x / 24) == object->xtile) && 
	    ((ds_global_juni.y / 24) == (object->ytile - 1))) {
		// Check for "walking" condition
		if (ds_juni_isOnTheGroundMoving()) {
		   // Time to put a cloud?
		   if ((ds_global_tick % 10) == 0) {
		      // Create cloud particle!
		      particle = ds_objects_createParticle(ds_global_juni.x, object->y - 24, object->layer, 51);
   		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						5 - PA_RandMinMax(0,10), 
						-5 - PA_RandMinMax(0,5), 
				 		0, 0, 3);				 		
				particle->inner[11] = 180;
				ds_3dsprite_setAlpha(particle->sprite,particle->inner[11]);
		   }   
		}   
   }   
   return 1;
}           

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects7_assign(u8 obj, ds_t_object *object) {
   switch (obj) {
      case 1:
         // Pouring Falling leaves
         object->fcreate = _ds_objects_b07o01_create;
         object->fmanage = _ds_objects_b07o01_manage;
         return 1;
         break; // not really necessary...
      case 2:
         // Falling leaf...
         object->fcreate = _ds_objects_b07o02_create;
         object->fmanage = _ds_objects_b07o02_manage;
         return 1;
         break; // not really necessary...         
      case 3:
         // Pouring Falling snow
         object->fcreate = _ds_objects_b07o03_create;
         object->fmanage = _ds_objects_b07o03_manage;
         return 1;
         break; // not really necessary...
      case 4:
         // Falling snow...
         object->fcreate = _ds_objects_b07o04_create;
         object->fmanage = _ds_objects_b07o04_manage;
         return 1;
         break; // not really necessary...         
      case 5:
         // Ray of light...
         object->fcreate = _ds_objects_b07o05_create;
         object->fmanage = _ds_objects_b07o05_manage;
         return 1;
         break; // not really necessary...         
      case 6:
         // Pouring Purple leaves
         object->fcreate = _ds_objects_b07o06_create;
         object->fmanage = _ds_objects_b07o06_manage;
         return 1;
         break; // not really necessary...
      case 7:
         // Purple leaf...
         object->fcreate = _ds_objects_b07o07_create;
         object->fmanage = _ds_objects_b07o07_manage;
         return 1;
         break; // not really necessary...         
      case 8:
         // Pouring Rain
         object->fcreate = _ds_objects_b07o08_create;
         object->fmanage = _ds_objects_b07o08_manage;
         return 1;
         break; // not really necessary...
      case 9:
         // Rain Drop
         object->fcreate = _ds_objects_b07o09_create;
         object->fmanage = _ds_objects_b07o09_manage;
         return 1;
         break; // not really necessary...         
      case 10:
         // Pouring Green leaves
         object->fcreate = _ds_objects_b07o10_create;
         object->fmanage = _ds_objects_b07o10_manage;
         return 1;
         break; // not really necessary...
      case 11:
         // Green leaf...
         object->fcreate = _ds_objects_b07o11_create;
         object->fmanage = _ds_objects_b07o11_manage;
         return 1;
         break; // not really necessary...         
      case 12:
         // Pouring White leaves
         object->fcreate = _ds_objects_b07o12_create;
         object->fmanage = _ds_objects_b07o12_manage;
         return 1;
         break; // not really necessary...
      case 13:
         // White leaf...
         object->fcreate = _ds_objects_b07o13_create;
         object->fmanage = _ds_objects_b07o13_manage;
         return 1;
         break; // not really necessary...         
      case 14:
         // Pouring Dust
         object->fcreate = _ds_objects_b07o14_create;
         object->fmanage = _ds_objects_b07o14_manage;
         return 1;
         break; // not really necessary...
      case 15:
         // Dust leaf...
         object->fcreate = _ds_objects_b07o15_create;
         object->fmanage = _ds_objects_b07o15_manage;
         return 1;
         break; // not really necessary...         
      case 16:
         // Cloud...
         object->fcreate = _ds_objects_b07o16_create;
         object->fmanage = _ds_objects_b07o16_manage;
         return 1;
         break; // not really necessary...         
   }   
   return 0;
}   
