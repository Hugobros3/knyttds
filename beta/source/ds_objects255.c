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
#include "ds_customobj.h"


// BANK 255 [B255] - Custom objects
//=================================================================================================

// CUSTOM OBJECT [B255]
//..........................................................................................
int _ds_objects_b255_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE);
		
	if (ds_objects_lib_initCustomObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific CO Operations
   object->type = DS_C_OBJ_OBJECT;
   ds_t_co *co = ds_customobj_get(object->obj);
   if (co == NULL)
   	ds_global_errorHalt("Error loading CO (b255)");
   object->x += co->Offset_X;
   object->y += co->Offset_Y; // Offset
   object->x -= (object->xs - 24) >> 1;
   object->y -= (object->ys - 24) >> 1; // Central location
   ds_3dsprite_setX(object->sprite,object->x);
   ds_3dsprite_setY(object->sprite,object->y);

   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b255_manage(void *objp) {
	int oldIma;
   ds_t_object *object = objp;
   ds_t_co *co = ds_customobj_get(object->obj);
   if (co == NULL)
   	return 1;

	// CO speed: 120 -> 1 sec.      
	/*
		[1] - Frame
		[2] - Speed
		[8] - Repeat
		[9] - Counter
	*/	

	switch (object->inner[0]) {
	   case 0:
	      // INIT
			// Manage certain parameters
	      // Init movement
	      object->inner[1] = co->Init_AnimFrom;
	      object->inner[2] = co->Init_AnimSpeed; // 120 -> 6 per second!
	      object->inner[8] = co->Init_AnimRepeat;
			//ds_3dsprite_setFrame(object->sprite,ds_3dsprite_getMaxFrame(object->sprite) - 1);	      
	      if (ds_3dsprite_getMaxFrame(object->sprite) != 1) // Considers 1-frame objects
	      	object->inner[0] = 1;
	      break;
	   case 1:
	      // PAINT
	      oldIma = object->inner[1];

			object->inner[9] += object->inner[2];
			if (object->inner[9] >= 1200) {
				while (object->inner[9] >= 1200) {
				   object->inner[9] -= 1200;
					object->inner[1]++;	            
			      if ((object->inner[1] > co->Init_AnimTo) || 
					    (object->inner[1] >= ds_3dsprite_getMaxFrame(object->sprite))) {
			         if (co->Init_AnimRepeat == 0) {
			            // Infinite
			      		object->inner[1] = co->Init_AnimLoopback;
			    		} else {
			    		   // Limited
			    		   if (object->inner[8] > 0) {
				    		   object->inner[8]--;
				    		   if (object->inner[8] > 0) {
				    		      object->inner[1] = co->Init_AnimLoopback;
				    		   } else {
				    		      object->inner[1]--;
								}     
			    			} else {
			    			   object->inner[1]--;
							}     
						}    	
					}			   
				}   
				if (oldIma != object->inner[1]) {
				   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
				}   
			}   
	      break;
	}	
	
   return 1;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects255_assign(u8 obj, ds_t_object *object) {   
   if (ds_customobj_exists(obj)) {
   	object->fcreate = _ds_objects_b255_create;
   	object->fmanage = _ds_objects_b255_manage;
      return 1;
   }      
	return 0;   
}
