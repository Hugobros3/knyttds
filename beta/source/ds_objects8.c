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

// BANK 8 [B08]
//=================================================================================================

// HORIZONTAL GLOW [B08_O01]
//..........................................................................................
int _ds_objects_b08o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08o01_manage(void *objp) {
   ds_t_object *object = objp;

   if (object->inner[0] == 0) {
   	ds_objects_createParticle(object->x, object->y, object->layer, 32);
   	object->inner[0] = 1;
	}   
   
	ds_objects_lib_beh_cycle(object,6);
   
   return 1;
}   

// VERTICAL GLOW [B08_O02]
//..........................................................................................
int _ds_objects_b08o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08o02_manage(void *objp) {
   ds_t_object *object = objp;
   
   if (object->inner[0] == 0) {
   	ds_objects_createParticle(object->x, object->y, object->layer, 33);
   	object->inner[0] = 1;
	}      
	ds_objects_lib_beh_cycle(object,6);
   return 1;
}      

// LIGHT \ [B08_O03]
//..........................................................................................
int _ds_objects_b08o03_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08o03_manage(void *objp) {
   ds_t_object *object = objp;
   
   if (object->inner[0] == 0) {
   	ds_objects_createParticle(object->x, object->y, object->layer, 34);
   	object->inner[0] = 1;
	}      
   return 1;
}      

// LIGHT - [B08_O04]
//..........................................................................................
int _ds_objects_b08o04_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08o04_manage(void *objp) {
   ds_t_object *object = objp;
   
   if (object->inner[0] == 0) {
   	ds_objects_createParticle(object->x, object->y, object->layer, 35);
   	object->inner[0] = 1;
	}      
   return 1;
}      

// LIGHT / [B08_O05]
//..........................................................................................
int _ds_objects_b08o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08o05_manage(void *objp) {
   ds_t_object *object = objp;
   
   if (object->inner[0] == 0) {
   	ds_objects_createParticle(object->x, object->y, object->layer, 36);
   	object->inner[0] = 1;
	}      
   return 1;
}      

// Common Fire Routines [B08_O06][B08_O07][B08_O08]
//..........................................................................................
int _ds_objects_b08oFire_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_3dsprite_setAlpha(object->sprite,192);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08oFire_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_cycle(object,7);

   return 1;
}

// INTENSE LIGHT (SPHERE) [B08_O09]
//..........................................................................................
int _ds_objects_b08o09_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08o09_manage(void *objp) {
   ds_t_object *object = objp;
   
   if (object->inner[0] == 0) {
   	ds_objects_createParticle(object->x, object->y, object->layer, 37);
   	object->inner[0] = 1;
	}      
   return 1;
}

// SMALL LIGHT [B08_O10]
//..........................................................................................
int _ds_objects_b08o10_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	object->inner[10] = object->x;
	object->inner[11] = object->y;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08o10_manage(void *objp) {
   ds_t_object *object = objp;
   
	int ds_b08 = 5;
	int ds_b08A[] = {1,5,10,18,23};
	ds_objects_lib_beh_knyttNgestures(object,ds_b08,ds_b08A,3,6,0);
	
	// Special... change coordinates!
	if ((ds_global_tick % 15) == 0) {
	   ds_3dsprite_setX(object->sprite, object->inner[10] + PA_RandMinMax(0,13));
	   ds_3dsprite_setY(object->sprite, object->inner[11] + PA_RandMinMax(0,13));
	}   

   return 1;
}            

// Common Cycle Routines [B08_O11][B08_O12][B08_O13][B08_O14]
//..........................................................................................
int _ds_objects_b08oCycle_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08oCycle_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_cycle(object,5);

   return 1;
}

// EXPLODE [B08_O15]
//..........................................................................................
int _ds_objects_b08o15_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	object->inner[8] = object->x;
	object->inner[9] = object->y;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08o15_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_cycle(object,6);
	
	// Special... change coordinates!
	if ((ds_3dsprite_getFrame(object->sprite) == 0) && (object->inner[1] == 6)) {
	   ds_3dsprite_setX(object->sprite, object->inner[8] + PA_RandMinMax(0,24) - 12);
	   ds_3dsprite_setY(object->sprite, object->inner[9] + PA_RandMinMax(0,24) - 12);
	}   

   return 1;
}

// FALLING STUFF [B08_O16]
//..........................................................................................
int _ds_objects_b08o16_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b08o16_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   
   // Create falling "leaves"...
   if ((ds_global_tick % 25) == 0) {
      particle = ds_objects_createParticle(object->x + PA_RandMax(16), object->y - 12, object->layer, 50);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
						0, 
						10, 
				 		10, 0, 3);				 		

   }   
   
   // Everything went OK...
   return 1;
}

// MEGA EXPLODE [B08_O17]
//..........................................................................................
int _ds_objects_b08o17_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	object->inner[10] = 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b08o17_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   
   // State!
   object->inner[10]++;
	
	// Behaviour	
	if ((object->inner[10] % 5) == 0) {
	   int i;
		for (i = 0; i < 32; i++) {
			int s,sx,sy;
		   s = 50 + PA_RandMinMax(0,40);
		   int angle = 11.25 * i; // r=11.25
		   sy = (int)(ds_util_fastSin(angle) * s);
		   sx = (int)(ds_util_fastCos(angle) * s);
			particle = ds_objects_createParticle(object->x + 8, 
								object->y - 4, 
								object->layer, 
								40);
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		0, 10, 3);				 		
		} 	
	}		

	// State
	if (object->inner[10] > (30)) {
	   // Finished ;-)
	   object->_deleteme = 1;
	   return 1;
	}   

   return 1;
}            

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects8_assign(u8 obj, ds_t_object *object) {
   if ((obj >= 6) && (obj <= 8)) {
      // Fires
      object->fcreate = _ds_objects_b08oFire_create;
      object->fmanage = _ds_objects_b08oFire_manage;      
      return 1;
   } else
   if ((obj >= 11) && (obj <= 14)) {
      // Fires
      object->fcreate = _ds_objects_b08oCycle_create;
      object->fmanage = _ds_objects_b08oCycle_manage;      
      return 1;
   } else
   switch (obj) {
      case 1:
         // Horizontal Glow
         object->fcreate = _ds_objects_b08o01_create;
         object->fmanage = _ds_objects_b08o01_manage;
         return 1;
         break; // not really necessary...            
      case 2:
         // Vertical Glow
         object->fcreate = _ds_objects_b08o02_create;
         object->fmanage = _ds_objects_b08o02_manage;
         return 1;
         break; // not really necessary...            
      case 3:
         // Light \_
         object->fcreate = _ds_objects_b08o03_create;
         object->fmanage = _ds_objects_b08o03_manage;
         return 1;
         break; // not really necessary...            
      case 4:
         // Light -
         object->fcreate = _ds_objects_b08o04_create;
         object->fmanage = _ds_objects_b08o04_manage;
         return 1;
         break; // not really necessary...            
      case 5:
         // Light _/
         object->fcreate = _ds_objects_b08o05_create;
         object->fmanage = _ds_objects_b08o05_manage;
         return 1;
         break; // not really necessary...            
      case 9:
         // Intense Light (Sphere)
         object->fcreate = _ds_objects_b08o09_create;
         object->fmanage = _ds_objects_b08o09_manage;
         return 1;
         break; // not really necessary...            
      case 10:
         // Small Light
         object->fcreate = _ds_objects_b08o10_create;
         object->fmanage = _ds_objects_b08o10_manage;
         return 1;
         break; // not really necessary...            
      case 15:
         // Explode
         object->fcreate = _ds_objects_b08o15_create;
         object->fmanage = _ds_objects_b08o15_manage;
         return 1;
         break; // not really necessary...            
      case 16:
         // Fallin' Stuff
         object->fcreate = _ds_objects_b08o16_create;
         object->fmanage = _ds_objects_b08o16_manage;
         return 1;
         break; // not really necessary...           
      case 17:
         // MEGA-Explode
         object->fcreate = _ds_objects_b08o17_create;
         object->fmanage = _ds_objects_b08o17_manage;
         return 1;
         break; // not really necessary...            
   }
	return 0;   
}
