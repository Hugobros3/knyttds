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
#include "ds_map.h"
#include "ds_juni.h"
#include "ds_3dsprite.h"
#include "ds_world.h"

// BANK 15 [B15]
//=================================================================================================

int _ds_objects_b15opblock_manageA(void *objp);

void _ds_objects_b15_keys(ds_t_object *object, int item) {
/*
	[0] - State
	[1] - Sprite
	[8] - Waiting time
*/
   switch (object->inner[0]) {
      case 0:
         // JUNI IS NEAR???? HAS THE KEY?????
         if ((ds_objects_lib_isNearJuni(object,32,0)) &&
			    (ds_util_bitOne16(ds_global_juni.item,item))) {
			       
            // Open the door
		      ds_map_copyFlagValue(0,
							 ds_3dsprite_getXSize(object->sprite),
							 ds_3dsprite_getYSize(object->sprite),
							 ds_3dsprite_getX(object->sprite),
							 ds_3dsprite_getY(object->sprite),
							 &ds_global_map.tileMapCol);
            // Prepare to open the door
            object->inner[0] = 1;
            object->inner[1] = 0;            
            object->inner[8] = 2;
         }   
         break;
      case 1:
         // OPEN DOOR
         object->inner[8]--;
         if (object->inner[8] <= 0) {
            object->inner[1]++;
            if (object->inner[1] < ds_3dsprite_getMaxFrame(object->sprite)) {
            	ds_3dsprite_setFrame(object->sprite,object->inner[1]);
//					ds_3dsprite_updateSprite(object->sprite);
					object->inner[8] = 2;               
            } else {
               object->inner[0] = 2;
				}      
         }            
         break;
      case 2:
         // DESTROY DOOR
         object->_deleteme = 1;
         break;
	}	
}

void _ds_objects_b15_appear(ds_t_object *object, int distance) {
   int val = 0;
   if (distance > 96) {
      if (ds_3dsprite_getAlpha(object->sprite) != 0)
      	ds_3dsprite_setAlpha(object->sprite,0);
   } else {   
      val = 255 - ((distance * 26) / 10);
   	ds_3dsprite_setAlpha(object->sprite, val);
	}   
}

void _ds_objects_b15_disappear(ds_t_object *object, int distance) {
   int val = 0;
   if (distance > 96) {
      if (ds_3dsprite_getAlpha(object->sprite) != 255)
      	ds_3dsprite_setAlpha(object->sprite,255);
   } else {   
      val = ((distance * 26) / 10);
   	ds_3dsprite_setAlpha(object->sprite, val);
	}   
}   
 
void _ds_objects_b15_fastDissappear(ds_t_object *object, int distance) {
   int val = 0;
   if (distance > 36) {
      if (ds_3dsprite_getInvisible(object->sprite))
      	ds_3dsprite_markInvisible(object->sprite,0);
      if (ds_3dsprite_getAlpha(object->sprite) != 255)
      	ds_3dsprite_setAlpha(object->sprite,255);
   } else if (distance > 24) {
      val = 255 - ((36 - distance) * 21);
      if (ds_3dsprite_getInvisible(object->sprite))
      	ds_3dsprite_markInvisible(object->sprite,0);
   	ds_3dsprite_setAlpha(object->sprite, val);      
	} else {   
	   if (!ds_3dsprite_getInvisible(object->sprite))
      	ds_3dsprite_markInvisible(object->sprite,1);
	}   
}

void _ds_objects_b15_PasswordTriggered() {
   void *myiterator;
   ds_t_object *object;
   
   myiterator = ds_linkedlist_startIterator(&ds_global_objects);
   while ((object = ds_linkedlist_getIterator(&ds_global_objects,&myiterator)) != NULL) {
      if ((object->bank == 15) &&  // Our bank
			 ((object->obj >= 23) && (object->obj <= 24)) && // Our object
			 (object->inner[10] == 0)) { // Object is not being destroyed :-)
			// Dissapear, clean area, destroy "one cycle" flag
			object->fmanage = _ds_objects_b15opblock_manageA;    
			ds_map_copyFlagValue(0,object->xs,object->ys,object->x,object->y, &ds_global_map.tileMapCol);
			object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE);
		}			
   }   
}     

// FIXED BLOCKS [B15_O01] - [B15_O04], [B15_O12]
//..........................................................................................
int _ds_objects_b15ofixed_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_FULLCOLLIDE);
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

// DISSAPEARING BLOCKS [B15_O08] - [B15_O11]
//..........................................................................................
int _ds_objects_b15odiss_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b15odiss_manage(void *objp) {
   ds_t_object *object = objp;
   
	_ds_objects_b15_fastDissappear(object, ds_objects_lib_distanceJuniCorrected(object,1));
   
   return 1;
}   

// PASSWORD TRIGGERS [B15_O13] - [B15_O21]
//..........................................................................................
int _ds_objects_b15opass_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15opass_manage(void *objp) {
   ds_t_object *object = objp;   
	switch (object->inner[10]) {
	   case 0: // INIT
	   	object->inner[8] = object->obj; // Real password
	   	ds_global_map.passMax++; // One more password that needs to be known
	   	object->inner[9] = 13; // Actual guess (reset)
	   	ds_3dsprite_changeBankObj(object->sprite,object->bank,object->inner[9]);
	   	object->inner[10] = 1;
	      break;
	   case 1: // ACTIVATE!
	      if (ds_objects_lib_stepObject(object)) {
	      	object->inner[10] = 2;
	    		ds_3dsprite_changeBankObj(object->sprite,object->bank,39); // Animated INT! (Special) 	
	    	}  	
	      break;
	   case 2: // ANIMATE!
	   	if (ds_objects_lib_beh_cycleDie(object,3)) {
	   	   object->inner[0] = 0; // For further resets...
	   	   object->_deleteme = 0; // It is not over, snake... NOT YET!!!!!! -:)
	   	   object->inner[9]++; 
	   	   if (object->inner[9] == 22) // - / \ Cycle 
					object->inner[9] = 13;
				ds_3dsprite_changeBankObj(object->sprite,object->bank,object->inner[9]);
	   	   object->inner[10] = 3;
	   	}   
	      break;
	   case 3: // WAIT!!!
	      if (!ds_objects_lib_stepObject(object)) {
	      	object->inner[10] = 1;
	    	}  	
	      break;
	}      
	// Check compliance with GOOD PASSWORD
	if (object->inner[8] == object->inner[9]) {
	   ds_global_map.pass++; // Increment good password hits for this cycle
	}   
   
   return 1;
}   

// PASSWORD INSTA-TRIGGER [B15_O22]
//..........................................................................................
int _ds_objects_b15o22_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15o22_manage(void *objp) {
   ds_t_object *object = objp;   
	switch (object->inner[10]) {
	   case 0: // INIT
	   	object->inner[10] = 1;
	      break;
	   case 1: // ACTIVATE! (& Maybe OPEN)
	      if (ds_objects_lib_stepObject(object)) {
	      	object->inner[10] = 2;
	    		ds_3dsprite_changeBankObj(object->sprite,object->bank,39); // Animated INT! (Special) 	
	    		if (!object->inner[9]) {
	    		   ds_objects_addAfterManagementFunction((void *) _ds_objects_b15_PasswordTriggered); // Open!!!
	    		   object->inner[9] = 1;
	    		}   
	    	}  	
	      break;
	   case 2: // ANIMATE!
	   	if (ds_objects_lib_beh_cycleDie(object,3)) {
	   	   object->inner[0] = 0; // For further resets...
	   	   object->_deleteme = 0; // It is not over, snake... snake? snaaaaaaake!!!!!! (Too much MGS4... :-P)
				ds_3dsprite_changeBankObj(object->sprite,object->bank,22);
	   	   object->inner[10] = 3;
	   	}   
	      break;
	   case 3: // WAIT!!!
	      if (!ds_objects_lib_stepObject(object)) {
	      	object->inner[10] = 1;
	    	}  	
	      break;
	}      
   
   return 1;
}   

// UP WIND [B15_O05]
//..........................................................................................
int _ds_objects_b15o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE);	
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_EVENT;
   object->managed = 1;
   ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_UPWIND);   
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15o05_manage(void *objp) {
   ds_t_object *object = objp;
	
	// Cycle the wind   
	ds_objects_lib_beh_cycle(object, 3);
   
   return 1;
}   

// BLUE INVISIBLE [B15_O6]
//..........................................................................................
int _ds_objects_b15o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_FULLCOLLIDE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   ds_3dsprite_setAlpha(object->sprite,0); // Cannot see it!!!!
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15o06_manage(void *objp) {
   ds_t_object *object = objp;
   
	_ds_objects_b15_appear(object, ds_objects_lib_distanceJuniCorrected(object,1));
   
   return 1;
}   

// BLUE VISIBLE [B15_O7]
//..........................................................................................
int _ds_objects_b15o07_create(u8 bank, u8 obj, void *objp) {
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
   
   ds_3dsprite_setAlpha(object->sprite,255); // Can see it!!!!
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15o07_manage(void *objp) {
   ds_t_object *object = objp;
   
	_ds_objects_b15_disappear(object, ds_objects_lib_distanceJuniCorrected(object,1));
   
   return 1;
}

// PASSWORD BLOCK [B15_O23-24]
//..........................................................................................
int _ds_objects_b15opblock_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_COLLIDE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_3dsprite_setAlpha(object->sprite,192);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15opblock_manage(void *objp) {
   ds_t_object *object = objp;

	if ((!object->_deleteme) && // Guard against management after deletion (it WAS a onecycle)
	    (ds_global_map.pass == ds_global_map.passMax)) { // Password is known!!!! 
	   // Password achieved!
	   ds_objects_addAfterManagementFunction((void *) _ds_objects_b15_PasswordTriggered);
	}      
   
   return 1;
}

int _ds_objects_b15opblock_manageA(void *objp) { // Special for disappearing
   ds_t_object *object = objp;

	// Fade out!
	object->inner[10]+= 3;
	if (object->inner[10] < 192) {
	   ds_3dsprite_setAlpha(object->sprite,192 - object->inner[10]);
	} else {
	   object->_deleteme = 1;
	}      

   return 1;
}

// "BLINK" BLOCK [B15_O25]
//..........................................................................................
int _ds_objects_b15o25_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	object->inner[0] = PA_RandMinMax(0,1);
	object->inner[1] = PA_RandMinMax(96,192);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15o25_manage(void *objp) {
   ds_t_object *object = objp;
   
   if (!object->inner[0]) {
      object->inner[1]++;
      if (object->inner[1] > 253)
      	object->inner[0] = 1;
   } else {
      object->inner[1]--;
      if (object->inner[1] < 64)
      	object->inner[0] = 0;
	}
	ds_3dsprite_setAlpha(object->sprite,255 - object->inner[1]);      
   
   return 1;
}   

// TIMER PASSWORD [B15_O26]
//..........................................................................................
int _ds_objects_b15o26_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15o26_manage(void *objp) {
   ds_t_object *object = objp;
   
	switch (object->inner[10]) {
	   case 0: // INIT
	   	object->inner[10] = 1;
	      break;
	   case 1: // MANAGE 
	      if (!ds_objects_lib_stepObject(object)) {
	         // Cambiar a modo NO pisado
	         if (object->inner[4] == 1) {
	            object->inner[4] = 0;
					ds_3dsprite_setFrame(object->sprite,2 + (3 * object->inner[6]));            
	         }   
	      } else {
	         // Change to STEP mode
	         if (object->inner[4] == 0) {
	            object->inner[4] = 1;
	         }   
	         // Increase TICKS and LEVEL
	         object->inner[5]++; // Ticks
	         object->inner[6] = object->inner[5] / 60; // Level
	         // Sprite change, or end
	         if (object->inner[6] >= 5) {
	            ds_3dsprite_setFrame(object->sprite,1);
	            object->inner[10] = 2;
	         } else {
				   if (object->inner[1] > 0)
				   	object->inner[1]--;
				   if (object->inner[1] <= 0) {
				      object->inner[1] = 6; // Sprite timer;
				      object->inner[2] = (object->inner[2] + 1) % 2;
				      ds_3dsprite_setFrame(object->sprite,2 + (3 * object->inner[6]) + (object->inner[2] + 1));
					}			
				}     
			}      
	      break;
	   case 2: // TRIGGER
	   	ds_objects_addAfterManagementFunction((void *) _ds_objects_b15_PasswordTriggered);
	   	object->inner[10] = 3;
	      break;
	   case 3: // LOOP
	   	// Nothing to do...
	      break;
	}
	   
   return 1;
}      

// RED KEY BLOCK [B15_O27]
//..........................................................................................
int _ds_objects_b15o27_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_COLLIDE);
	object->inner[0] = 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15o27_manage(void *objp) {
   ds_t_object *object = objp;
   
   _ds_objects_b15_keys(object,DS_C_JUNI_IT_REDKEY);
   
   return 1;
}   

// YELLOW KEY BLOCK [B15_O28]
//..........................................................................................
int _ds_objects_b15o28_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_COLLIDE);
	object->inner[0] = 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15o28_manage(void *objp) {
   ds_t_object *object = objp;
   
   _ds_objects_b15_keys(object,DS_C_JUNI_IT_YELLOWKEY);
   
   return 1;
}   

// BLUE KEY BLOCK [B15_O29]
//..........................................................................................
int _ds_objects_b15o29_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_COLLIDE);
	object->inner[0] = 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15o29_manage(void *objp) {
   ds_t_object *object = objp;
   
   _ds_objects_b15_keys(object,DS_C_JUNI_IT_BLUEKEY);
   
   return 1;
}   

// VIOLET KEY BLOCK [B15_O30]
//..........................................................................................
int _ds_objects_b15o30_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_COLLIDE);
	object->inner[0] = 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b15o30_manage(void *objp) {
   ds_t_object *object = objp;
   
   _ds_objects_b15_keys(object,DS_C_JUNI_IT_VIOLETKEY);
   
   return 1;
}   

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects15_assign(u8 obj, ds_t_object *object) {
   if ((obj >= 1) && (obj <= 4)) {
      // Fixed blocks
      object->fcreate = _ds_objects_b15ofixed_create;
      return 1;
   }   
   if ((obj >= 8) && (obj <= 11)) {
      // Dissapearing blocks
      object->fcreate = _ds_objects_b15odiss_create;
      object->fmanage = _ds_objects_b15odiss_manage;
      return 1;
   }   
   if ((obj >= 13) && (obj <= 21)) {
      // Pass
      object->fcreate = _ds_objects_b15opass_create;
      object->fmanage = _ds_objects_b15opass_manage;
      return 1;
   }   
   if ((obj >= 23) && (obj <= 24)) {
      // Password Blocks
      object->fcreate = _ds_objects_b15opblock_create;
      object->fmanage = _ds_objects_b15opblock_manage;
      return 1;
   }   
   if (obj == 12) {
      // More fixed blocks
      object->fcreate = _ds_objects_b15ofixed_create;      
      return 1;
   }      
   switch (obj) {
      case 5:
         // Up Wind
         object->fcreate = _ds_objects_b15o05_create;
         object->fmanage = _ds_objects_b15o05_manage;
         return 1;
         break; // not really necessary...            
      case 6:
         // Blue Invisible
         object->fcreate = _ds_objects_b15o06_create;
         object->fmanage = _ds_objects_b15o06_manage;
         return 1;
         break; // not really necessary...      
      case 7:
         // Blue Visible
         object->fcreate = _ds_objects_b15o07_create;
         object->fmanage = _ds_objects_b15o07_manage;
         return 1;
         break; // not really necessary...      
      case 22:
         // Insta-Password
         object->fcreate = _ds_objects_b15o22_create;
         object->fmanage = _ds_objects_b15o22_manage;
         return 1;
         break; // not really necessary...
      case 25:
         // "Blip" Block
         object->fcreate = _ds_objects_b15o25_create;
         object->fmanage = _ds_objects_b15o25_manage;
         return 1;
         break; // not really necessary...
      case 26:
         // Timer Password
         object->fcreate = _ds_objects_b15o26_create;
         object->fmanage = _ds_objects_b15o26_manage;
         return 1;
         break; // not really necessary...
      case 27:
         // Red Key Block
         object->fcreate = _ds_objects_b15o27_create;
         object->fmanage = _ds_objects_b15o27_manage;
         return 1;
         break; // not really necessary...
      case 28:
         // Yellow Key Block
         object->fcreate = _ds_objects_b15o28_create;
         object->fmanage = _ds_objects_b15o28_manage;
         return 1;
         break; // not really necessary...
      case 29:
         // Blue Key Block
         object->fcreate = _ds_objects_b15o29_create;
         object->fmanage = _ds_objects_b15o29_manage;
         return 1;
         break; // not really necessary...
      case 30:
         // Violet Key Block
         object->fcreate = _ds_objects_b15o30_create;
         object->fmanage = _ds_objects_b15o30_manage;
         return 1;
         break; // not really necessary...
   }   
   return 0;
}   
