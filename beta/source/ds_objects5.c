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

// BANK 9 [B09]
//=================================================================================================

// SCARY SHADOW [B05_O01]
//..........................................................................................
int _ds_objects_b05o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	object->inner[0] = 8; // Alpha

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b05o01_manage(void *objp) {
   /* Scary images:
      - 0 - 3: look right
      - 4 - 7: look left
      - 8 - 9: look right scary
      - 10 - 11: look left scary
      */
   ds_t_object *object = objp;
   int lookLeft;
   
   // Left? Right?
   lookLeft = (ds_juni_getXVirtual() < object->x); 
	if (object->inner[1] != lookLeft) {
	   // Change!
	   if (lookLeft) {
	      object->inner[1] = 1;
	      if (object->inner[2]) {
	         object->inner[7] = 10; // SP
	         object->inner[8] = 10; // SPMIN
	         object->inner[9] = 11; // SPMAX
	      } else {
	         object->inner[7] = 4;
	         object->inner[8] = 4;
	         object->inner[9] = 7;
			}      
	   } else {
	      object->inner[1] = 0;
	      if (object->inner[2]) {
	         object->inner[7] = 8;
	         object->inner[8] = 8;
	         object->inner[9] = 9;
	      } else {
	         object->inner[7] = 0;
	         object->inner[8] = 0;
	         object->inner[9] = 3;
			}      
		}     
	}
	
	// Cycle
   if (object->inner[6] > 0)
   	object->inner[6]--;
   if (object->inner[6] <= 0) {
      object->inner[6] = 9;
      ds_3dsprite_setFrame(object->sprite,object->inner[7]);
      object->inner[7]++;
      if (object->inner[7] > object->inner[9])
      	object->inner[7] = object->inner[8];
	}		   
	
	// State!
	if (!object->inner[2]) { // Not Scary?
		if (ds_juni_faceRight() == lookLeft) {
			object->inner[0] += ((ds_global_tick % 2) == 0)?1:2;
			if (object->inner[0] >= 255) {
			   object->inner[0] = 255;
			   object->inner[1] = 2; // Force Change!
			   object->inner[2] = 1;
				object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL); // Pain!		   
			}   
		} else {
		   if (object->inner[0] > 8) {
		   	object->inner[0] -= ((ds_global_tick % 2) == 0)?1:2;
		 	} else {
		 	   object->inner[0] = 8;
			}     	
		}   		
		ds_3dsprite_setAlpha(object->sprite,object->inner[0]);
	} else { // Scary?
		if (ds_juni_faceRight() == !lookLeft) {
		   object->inner[1] = 2; // Force Change!
			object->inner[2] = 0;
			object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_HARMFUL); // Not Pain!
		}   
	}   	
   
   return 1;
}

// SHADOW DUDE/GIRL [B09_O02-03]
//..........................................................................................
int _ds_objects_b05owalk_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,0);
	ds_objects_lib_beh_ghostIni(object,0,16,200,30);	
	
	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b05owalk_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_ghost(object,30,120,30);	
	ds_objects_lib_beh_moveLeftRight(object,1,1,0,100,24,24*8,0*60,7*60,3,2,0,-1,1,1);
   
   return 1;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects5_assign(u8 obj, ds_t_object *object) {
   if (obj == 1) {
      // Scary Shadow
      object->fcreate = _ds_objects_b05o01_create;
      object->fmanage = _ds_objects_b05o01_manage;
      return 1;
   } else if ((obj >= 2) && (obj <= 3)) {
      // Shadow Dude / Girl
      object->fcreate = _ds_objects_b05owalk_create;
      object->fmanage = _ds_objects_b05owalk_manage;
      return 1;
	}      
	return 0;   
}
