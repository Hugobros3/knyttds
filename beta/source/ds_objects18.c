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
#include "ds_music.h"


// BANK 18 [B18]
//=================================================================================================

int ds_b18_FishyA = 10;
int ds_b18_FishyAM[] = {2,4,4,4,1,1,4,4,4,2};
int ds_b18_FishyAD[] = {2,13,11,13,2,6,17,15,17,6};

int ds_b18_FishyB = 10;
int ds_b18_FishyBM[] = {1,4,4,1,5,5,1,4,4,1};
int ds_b18_FishyBD[] = {2,13,15,6,2,6,2,11,17,6};

int ds_b18_FishyC = 8;
int ds_b18_FishyCM[] = {1,2,7,1,1,7,2,1};
int ds_b18_FishyCD[] = {6,17,15,17,13,11,13,2};

int ds_b18_FishyD = 12;
int ds_b18_FishyDM[] = {1,5,2,1,6,1,2,5,1,1,6,1};
int ds_b18_FishyDD[] = {2,13,2,6,15,6,2,11,2,6,17,6};

int ds_b18_FishyE = 10;
int ds_b18_FishyEM[] = {1,1,4,4,2,1,1,4,5,1};
int ds_b18_FishyED[] = {6,15,17,11,2,6,17,15,13,11};

// FISHIES! [B18_O01-05]
//..........................................................................................
int _ds_objects_b18ofishy_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b18o01_manage(void *objp) {
   ds_t_object *object = objp;
   
  	ds_objects_lib_beh_followPathLR(object,1,ds_b18_FishyA,ds_b18_FishyAM,ds_b18_FishyAD,2,3,4);
   return 1;
}

int _ds_objects_b18o02_manage(void *objp) {
   ds_t_object *object = objp;
   
  	ds_objects_lib_beh_followPathLR(object,1,ds_b18_FishyB,ds_b18_FishyBM,ds_b18_FishyBD,4,3,4);
   return 1;
}

int _ds_objects_b18o03_manage(void *objp) {
   ds_t_object *object = objp;
   
  	ds_objects_lib_beh_followPathLR(object,1,ds_b18_FishyC,ds_b18_FishyCM,ds_b18_FishyCD,2,3,3);
   return 1;
}

int _ds_objects_b18o04_manage(void *objp) {
   ds_t_object *object = objp;
   
  	ds_objects_lib_beh_followPathLR(object,1,ds_b18_FishyD,ds_b18_FishyDM,ds_b18_FishyDD,2,3,3);
   return 1;
}

int _ds_objects_b18o05_manage(void *objp) {
   ds_t_object *object = objp;
   
  	ds_objects_lib_beh_followPathLR(object,1,ds_b18_FishyE,ds_b18_FishyEM,ds_b18_FishyED,4,3,5);
   return 1;
}

// WATER EYE + [B18_O06]
//..........................................................................................
int _ds_objects_b18o06_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b18o06_manage(void *objp) {
   ds_t_object *object = objp;
   
	switch (object->inner[0]) {
	   case 0:
	      // Nothing to do... (besides management)
	      object->inner[1] = 0;
	      object->inner[9] = 3;
			// However... check if I should promote to big baddie :-)
		   if ((ds_global_tick % 240) == 0) { // 400 / 1.6
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
					   ds_music_playSound("Homing Shot", 0, 0);
				      ds_t_object *particle;
					   for (i = 0; i < 2; i++) {
						   particle = ds_objects_createParticle(object->x + 8, object->y - 8, object->layer, 12);
				   	   ds_objects_lib_beh_particleMMF2_Init(particle,
									5 - PA_RandMinMax(0,10),
									-20,
									1,0,3);
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


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects18_assign(u8 obj, ds_t_object *object) {
   switch (obj) {
      case 1:
      	// Fishy A
      	object->fcreate = _ds_objects_b18ofishy_create;
      	object->fmanage = _ds_objects_b18o01_manage;
      	return 1;
      	break; // not really necessary...            
      case 2:
      	// Fishy B
      	object->fcreate = _ds_objects_b18ofishy_create;
      	object->fmanage = _ds_objects_b18o02_manage;
      	return 1;
      	break; // not really necessary...            
      case 3:
      	// Fishy C
      	object->fcreate = _ds_objects_b18ofishy_create;
      	object->fmanage = _ds_objects_b18o03_manage;
      	return 1;
      	break; // not really necessary...            
      case 4:
      	// Fishy D
      	object->fcreate = _ds_objects_b18ofishy_create;
      	object->fmanage = _ds_objects_b18o04_manage;
      	return 1;
      	break; // not really necessary...            
      case 5:
      	// Fishy E
      	object->fcreate = _ds_objects_b18ofishy_create;
      	object->fmanage = _ds_objects_b18o05_manage;
      	return 1;
      	break; // not really necessary...            
      case 6:
      	// Water Eye +
      	object->fcreate = _ds_objects_b18o06_create;
      	object->fmanage = _ds_objects_b18o06_manage;
      	return 1;
      	break; // not really necessary...            
   }      
	return 0;   
}
