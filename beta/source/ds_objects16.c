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
#include "ds_util.h"
#include "ds_world.h"
#include "ds_map.h"

// BANK 16 [B16]
//=================================================================================================

// SPRING - JUMPER [B16_O01]
//..........................................................................................
int _ds_objects_b16o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_SPRING);
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

// KILLER AREAS [B16_O02-12]
//..........................................................................................
int _ds_objects_b16o02to12_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_STATICHARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   // <TODO> - Kills itself
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

// BLOCKING BLOCK [B16_O13]
//..........................................................................................
int _ds_objects_b16o13_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   ds_map_copyFlag(ds_3dsprite_getSprite(object->sprite),	
				 ds_3dsprite_getXSize(object->sprite),
				 ds_3dsprite_getYSize(object->sprite),
				 ds_3dsprite_getX(object->sprite),
				 ds_3dsprite_getY(object->sprite),
				 &ds_global_map.tileMapCol);
   object->type = DS_C_OBJ_EVENT;
   // <TODO> Kills itself
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects16_assign(u8 obj, ds_t_object *object) {
   if ((obj > 1) && (obj < 13)) {
      // Killer Item
      object->fcreate = _ds_objects_b16o02to12_create;
      return 1;               
   }   
   switch (obj) {
      case 1:
         // Spring - Jumper
         object->fcreate = _ds_objects_b16o01_create;
         return 1;         
         break; // not really necessary...
      case 13:
         // Blocking Block
         object->fcreate = _ds_objects_b16o13_create;
         return 1;         
         break; // not really necessary...         
      case 14:
         // Blocking Block (2)
         object->fcreate = _ds_objects_b16o13_create;
         return 1;         
         break; // not really necessary...                  
      case 15:
         // Blocking Block (3)
         object->fcreate = _ds_objects_b16o13_create;
         return 1;         
         break; // not really necessary...                  
      case 16:
         // Blocking Block (4)
         object->fcreate = _ds_objects_b16o13_create;
         return 1;         
         break; // not really necessary...                  
   }   
   return 0;
}   
