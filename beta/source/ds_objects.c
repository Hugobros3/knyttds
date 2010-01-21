/*TODO:
   
   Check Access of particles in this library
   Change spriteglobal in 3ds
   change sprites in 3ds_hdd
   
   Also...
   
   For Pass (buttons), maybe allow the change of sprite of one object
   	(load image into the 3dspritehdd system? Is easy, just load a sprite.
		 next time, the sprite will still be in the system and a change of screen
		 will reset the subsystem)*/

/*
Copyright (c) 2008 Rodrigo Roman and Ramon Roman (rrc2soft)
(Original Game "Knytt Stories" copyright (c) Nicklas "Nifflas" Nygren)

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

/*
	<TODO> Cache of objects (to not create the 3Dsprite again)
*/

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_map.h"
#include "ds_linkedlist.h"
#include "ds_util.h"
#include "ds_3dsprite.h"
#include "ds_world.h"
#include "ds_input.h"
#include "ds_particles0.h"
#include "ds_objects0.h"
#include "ds_objects1.h"
#include "ds_objects2.h"
#include "ds_objects3.h"
#include "ds_objects4.h"
#include "ds_objects5.h"
#include "ds_objects6.h"
#include "ds_objects7.h"
#include "ds_objects8.h"
#include "ds_objects9.h"
#include "ds_objects10.h"
#include "ds_objects11.h"
#include "ds_objects12.h"
#include "ds_objects13.h"
#include "ds_objects14.h"
#include "ds_objects15.h"
#include "ds_objects16.h"
#include "ds_objects17.h"
#include "ds_objects18.h"
#include "ds_objects255.h"
#include "ds_objects_lib.h"
#include <stdlib.h>

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

typedef struct _ds_tt_objectOC {
   LList objects; // Which objects belong to this list
   u8 bank; // which bank
   u8 obj; // which obj
   int semaphore; // -1 if the semaphore is empty, 0..n if someone reserved the OC.
   int instancephore; // >0 if specific instances want attention, 0 otherwise.
} _ds_t_objectOC; // For the management of the One-Cycle Objects  

ds_t_roomid objectDirect[4];
u8 objectManaged[DS_C_MAX_BANK][DS_C_MAX_OBJ];
u8 particleManaged[DS_C_MAX_OBJ_PART];
u8 coManaged[DS_C_MAX_OBJ_CO];
LList objectOneCycle;
LList objectManageAfter;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// OBJECT MANAGEMENT FUNCTION
//-------------------------------------------------------------------------------------------------

// BANK -1 (Template) [B00]
//.................................................................................................

/*int _ds_objects_bXXoXX_create(void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	ds_objects_lib_initObject(object);

	// Specific Operations
   object->type = DS_C_OBJ_;
   
   // Return 1 if I'm an event/item
   return ((object->type == DS_C_OBJ_EVENT) || (object->type == DS_C_OBJ_ITEM));
}   

int _ds_objects_bXXoXX_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Not managed? Bye!!!
   if (!object->managed)
   	return 1;
   
   // Everything went OK...
   return 1;
}   */


//-------------------------------------------------------------------------------------------------
// LOADING FUNCTION [BXX]
//-------------------------------------------------------------------------------------------------

int _ds_objects_assign(u8 bank, u8 obj, ds_t_object *object) {
   
   // On the beginning, I have nothing...
	object->fcreate = NULL;
	object->fmanage = NULL;
	object->fexecute = NULL;
	object->finstance = NULL;
	object->fcondition = NULL;

   switch (bank) {
      // BANK 0 - SYSTEM
      //----------------
      case 0: 
         return ds_objects0_assign(obj,object);
			break;   
			
      // BANK 1 - LIQUIDS
      //-------------------
      case 1: 
         return ds_objects1_assign(obj,object);
         break;
      // BANK 2 - FLYERS
      //-----------------------
      case 2: 
         return ds_objects2_assign(obj,object);
         break;         
      // BANK 3 - KNYTT & FLUFF
      //-----------------------
      case 3: 
         return ds_objects3_assign(obj,object);
         break;
      // BANK 4 - MEANIES
      //-----------------
      case 4: 
         return ds_objects4_assign(obj,object);
         break;
      // BANK 5 - SHADOWS
      //-----------------
      case 5: 
         return ds_objects5_assign(obj,object);
         break;
      // BANK 6 - TRAPS
      //---------------
      case 6: 
         return ds_objects6_assign(obj,object);
			break;         					
      // BANK 7 - NATURE FX
      //-------------------
      case 7: 
         return ds_objects7_assign(obj,object);
         break;
      // BANK 8 - DECORATION
      //--------------------
      case 8: 
         return ds_objects8_assign(obj,object);
         break;
      // BANK 9 - FLOWERS
      //-----------------
      case 9: 
         return ds_objects9_assign(obj,object);
         break;
      // BANK 10 - BOUNCERS
      //-------------------
      case 10: 
         return ds_objects10_assign(obj,object);
         break;         
      // BANK 11 - DISKS
      //----------------
      case 11: 
         return ds_objects11_assign(obj,object);
         break;         
      // BANK 12 - GHOSTS
      //-----------------
      case 12: 
         return ds_objects12_assign(obj,object);
			break;         
      // BANK 13 - ROBOTS
      //-----------------
      case 13: 
         return ds_objects13_assign(obj,object);
			break;         
      // BANK 14 - EARTH BEINGS
      //-----------------------
      case 14: 
         return ds_objects14_assign(obj,object);
			break;			
      // BANK 15 - OBJECTS AND AREAS
      //----------------------------
      case 15: 
         return ds_objects15_assign(obj,object);
			break;         			
      // BANK 16 - INVISIBLE
      //--------------------
      case 16: 
         return ds_objects16_assign(obj,object);
			break;         					
      // BANK 17 - CREEPY
      //-----------------
      case 17: 
         return ds_objects17_assign(obj,object);
			break;         					
      // BANK 18 - WATER CREATURES
      //--------------------------
      case 18: 
         return ds_objects18_assign(obj,object);
			break;         					
      // CHILDREN BANK - PARTICLES
      //--------------------------
      case DS_C_PART_BANK: 
         return ds_particles0_assign(obj,object);
			break;         			
      // CUSTOM OBJECTS BANK
      //--------------------------
      case DS_C_CO_BANK: 
         return ds_objects255_assign(obj,object);
			break;         			
   }   
   return 0;
}

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Resets part of the object subsystem - DirectID */
void _ds_objects_initDirect() {
   int xx,yy,zz;
   
   for (zz = 0; zz < 4; zz++) {
	   for (yy = 0; yy < 10; yy++) {
		   for (xx = 0; xx < 25; xx++) {
		      objectDirect[zz].id[yy][xx] = -1;
		   }
		}
	}			   
}

/* Resets part of the object subsystem - Object Managed */
void _ds_objects_initManaged() {
   int i,j;
   // Reset object managed
   for (j = 0; j < DS_C_MAX_BANK; j++) {
	   for (i = 0; i < DS_C_MAX_OBJ; i++) {
	      objectManaged[j][i] = 0;
  		}   	      
  	}
   for (i = 0; i < DS_C_MAX_OBJ_PART; i++) {
      particleManaged[i] = 0;
	}   	      

   for (i = 0; i < DS_C_MAX_OBJ_CO; i++) {
      coManaged[i] = 0;
	}   	      
  	
  	// Reset special subsytems
  	ds_global_map.pass = 0; // Resets the password system
}

void _ds_objects_oneCycle_add(ds_t_object *object) {   
   // First, seek a suitable _ds_t_objectOC
   _ds_t_objectOC *OC;
   void *myiterator;
   
   myiterator = ds_linkedlist_startIterator(&objectOneCycle);
   while ((OC = ds_linkedlist_getIterator(&objectOneCycle,&myiterator)) != NULL) {
      if ((object->bank == OC->bank) && (object->obj == OC->obj)) {
         break;
		}
   } 
	
	// No OC? Create, add
	if (OC == NULL) {
		OC = malloc(sizeof(_ds_t_objectOC));
		if (OC == NULL) {
		   // Ouch! No memory!
		   ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		   sprintf(ds_global_string,"CALL _ds_objects_oneCycle_add");
		   ds_global_errorHalt(ds_global_string);
		   //--HALT--//
		}
		// Create
		OC->bank = object->bank;
		OC->obj = object->obj;   
		OC->semaphore = -1;
		OC->instancephore = 0;
		ds_linkedlist_reset(&OC->objects);
		
		// Add
		ds_linkedlist_add(&objectOneCycle,OC);
	}    
	
	// Add the object to the actual OC
	ds_linkedlist_add(&OC->objects,object);
}

void _ds_objects_oneCycle_reset() {   
   _ds_t_objectOC *OC;
   void *myiterator;

	// Delete internal lists   
   myiterator = ds_linkedlist_startIterator(&objectOneCycle);
   while ((OC = ds_linkedlist_getIterator(&objectOneCycle,&myiterator)) != NULL) {
      ds_linkedlist_finish(&OC->objects,0,0); // Free neither contents nor lists
   } 
   
   // Delete list
	ds_linkedlist_finish(&objectOneCycle,1,0); // Deletes contents, but NOT the list
	
}   

/* Deletes a certain object. Note: object IS NOT FREED, we only free its contents */
int _ds_objects_deleteObject(ds_t_object *object) {   
   // No need to do anything... there are no pointers in the objects
   // Besides, the 3DSubsystems clean themselves ^_-
   
   // TODO: Delete the object from the OneCycle list
   
	return 1;
}   

/* Deletes an object from the system - usually called when an object deletes thyself */
void _ds_objects_deleteObjectSystem(ds_t_object *object) {
   int id_linkedlist = object->id;
   if (ds_objects_lib_iseventitem(object->type)) { // <TODO> objects whose fmanage == NULL   
		ds_global_map.room.objlayer[object->layer].obj[object->ytile][object->xtile] = 0;
	   ds_global_map.room.objlayer[object->layer].bank[object->ytile][object->xtile] = 0;
		objectDirect[object->layer].id[object->ytile][object->xtile] = -1;
	}	
	ds_3dsprite_deleteSprite(object->sprite, 1);
	_ds_objects_deleteObject(object);
	ds_linkedlist_delID(&ds_global_objects,id_linkedlist);
}

/* Post-manage an object right after loading it - e.g. to create/delete collision fields */
void _ds_objects_postLoad(ds_t_object *object) {
   // COLLISION - Fill a collision field
   if (ds_util_bitOne16(object->flags,DS_C_OBJ_F_COLLIDE)) {
      ds_map_copyFlag(ds_3dsprite_getSpriteFrame(object->sprite, 0),
							 ds_3dsprite_getXSize(object->sprite),
							 ds_3dsprite_getYSize(object->sprite),
							 ds_3dsprite_getX(object->sprite),
							 ds_3dsprite_getY(object->sprite),
							 &ds_global_map.tileMapCol);
   }
   if (ds_util_bitOne16(object->flags,DS_C_OBJ_F_FULLCOLLIDE)) {
      ds_map_copyFlagValue(1,
							 ds_3dsprite_getXSize(object->sprite),
							 ds_3dsprite_getYSize(object->sprite),
							 ds_3dsprite_getX(object->sprite),
							 ds_3dsprite_getY(object->sprite),
							 &ds_global_map.tileMapCol);
   }
   
	// STATICHARMFUL - Copy this to the main matrix, do not update ever
   if (ds_util_bitOne16(object->flags,DS_C_OBJ_F_STATICHARMFUL)) {
      ds_map_copyFlag(ds_3dsprite_getSpriteFrame(object->sprite, 0),
							 ds_3dsprite_getXSize(object->sprite),
							 ds_3dsprite_getYSize(object->sprite),
							 ds_3dsprite_getX(object->sprite),
							 ds_3dsprite_getY(object->sprite),
							 &ds_global_map.tileMapDam);
   }
}   

/* Inits the object subsystem */
void ds_objects_init() {
   ds_linkedlist_reset(&ds_global_objects);
   ds_linkedlist_reset(&objectOneCycle);
   ds_linkedlist_reset(&objectManageAfter);
	_ds_objects_initDirect();   
}   

/* Creates an object - being it object or particle */
void *_ds_objects_createObject(int xx, int yy, int zz, int bank, int obj, int manageRoom, int absoluteCoord) {
   ds_t_object *object = NULL;
   int id_ll;

   // OK... get some memory, if we hadn't created the object
	object = malloc(sizeof(ds_t_object));
	if (object == NULL) {
	   // Ouch! No memory!
	   ds_global_errorAssign(DS_C_ERR_NOMEMORY);
	   sprintf(ds_global_string,"CALL createObject A\n[%d,%d,%d][b:%d,o:%d]",xx,yy,zz,bank,obj);
	   ds_global_errorHalt(ds_global_string);
	   //--HALT--//
	}   
 	   
 	// 2) Load management functions
 	if (_ds_objects_assign(bank,obj,object)) {
	   if (absoluteCoord) {
	 	   object->x = xx;
	 	   object->y = yy;
	 	   object->xtile = xx / 24;
	 	   object->ytile = yy / 24; 	      
	   } else {
	 	   object->x = xx * 24;
	 	   object->y = yy * 24;
	 	   object->xtile = xx;
	 	   object->ytile = yy;			    	    	      
		}      
	   object->layer = zz;
	
 	   // 3) Executes initialization
 	   ds_global_errorAssign(DS_C_ERR_); // Resets error. Needed due to design error (detection of entity object)
 	   if (!object->fcreate(bank,obj,(void *)object)) {
 	      // 3.1) Error?
 	      if (ds_global_errorHappened()) {
 	      	// Argh! :-(
	   		sprintf(ds_global_string,"CALL createObject B\n[%d,%d,%d][b:%d,o:%d]",xx,yy,zz,bank,obj);
	   		ds_global_errorHalt(ds_global_string);
	   		//--HALT--//
 	    	}  	
 	      // 3.2) No? OK, then it is a "entity" object... delete from room
 	      if (manageRoom) {
    	   	ds_global_map.room.objlayer[zz].obj[yy][xx] = 0;
    	   	ds_global_map.room.objlayer[zz].bank[yy][xx] = 0;
    		}   
 	   }
 	   
		// 3.2a) Special: Update obj/bank if object changed it inside (e.g. shifts)   
		if (ds_global_map.room.objlayer[zz].obj[yy][xx] != 0) {
    	   	ds_global_map.room.objlayer[zz].obj[yy][xx] = object->obj;
    	   	ds_global_map.room.objlayer[zz].bank[yy][xx] = object->bank;
		}   
 	   // 3.3) Coord. Correction!! - If the sprite Y is < 24 AND NOT a CO, we need to put it on its place...
 	   if (bank != DS_C_CO_BANK) {
	 	   if (ds_3dsprite_getYSize(object->sprite) < 24) {
	 	      object->y += (24 - ds_3dsprite_getYSize(object->sprite));
	 	      ds_3dsprite_setY(object->sprite,object->y); // object->fcreate did this - now we need to do this again...
	 	   }    	    	   
 		}   
 		
 	   // 3.4 Layer Correction! - The sprite must be adjusted to its layer ;-)
 	   // ...UNLESS We are talking about particles!!!
 	   int priolayer = (object->layer < 2)?object->layer:(object->layer+1); // (0-1) 2 (3-4) [5]
 	   if (bank == DS_C_PART_BANK)
 	   	ds_3dsprite_setPrio(object->sprite, (DS_C_PRIO * priolayer) + 52); // + 52? more priority, different polyID
 	   else
   		ds_3dsprite_setPrio(object->sprite, (DS_C_PRIO * priolayer) + 1); // + 1? BG Map is 0.
 	   
 	   // 4) Saves object inside list, also saves id in "direct access" list
 	   id_ll = ds_linkedlist_add(&ds_global_objects,(void *)object);
 	   object->id = id_ll;
 	   if (ds_objects_lib_iseventitem(object->type)) { // <TODO> objects whose fmanage == NULL
 	   	if (manageRoom) 
    	   	objectDirect[zz].id[yy][xx] = id_ll;
    	}  
		 
		// 4.1) If the object is of type OneCycle or OneInstance, add it to the list
		if ((ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE)) ||
			 (ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE)))
			_ds_objects_oneCycle_add(object);
    	   
    	// 4.2) If object voted for self-deletion, then bye-bye object. If not, post-management
    	if (object->_deleteme)
    		_ds_objects_deleteObjectSystem(object);
    	else
			_ds_objects_postLoad(object);				    		
 	   
 	} else {
 	   if (manageRoom) {
 	   	// 2.a) Unmanaged Object... delete from the room
 	   	if (bank != DS_C_PART_BANK) {
	 	   	ds_global_map.room.objlayer[zz].obj[yy][xx] = 0;
 		   	ds_global_map.room.objlayer[zz].bank[yy][xx] = 0;
 			}   
 		   
 		   // 2.b) DELETE the object!!!!!
 		   free(object);
 		}   
 	}   
 	 	
 	return object;
}

/* Creates an object, for external purposes (e.g. particles) */
void *ds_objects_createParticle(int x, int y, int layer, int oo) {
   return _ds_objects_createObject(x,y,layer,DS_C_PART_BANK,oo,0,1); // Particles belong to creator's layer!
}   

/* Creates an object, for internal purposes (e.g. events, items, living beings) */
void *ds_objects_createObject(int x, int y, int layer, int bb, int oo) {
   return _ds_objects_createObject(x,y,layer,bb,oo,1,0);
}   

/* Includes a post-management function */
void ds_objects_addAfterManagementFunction(ds_t_fpostmanage func) {
   ds_linkedlist_add(&objectManageAfter,func);
}   

/* Preloads the objects from the world (room) */
int ds_objects_loadHDD() {
   int zz,xx,yy;
   u8 obj,bank;
   void *res;
   int nobj = 0;
   int safety = 0;

   // For every object in the world... Count!
   for (zz = 0; zz < 4; zz++) {
	   for (yy = 0; yy < 10; yy++) {
		   for (xx = 0; xx < 25; xx++) {
		      obj = ds_global_map.room.objlayer[zz].obj[yy][xx];
		      if (obj != 0)
					nobj++;
			}
		}				   
	}		
	
	// FIXME: SAFETY CHECK!! (Special Anti-Mushroom for Beta ;-) )
	if (nobj > (250 + 125)) {
	   // WTF!!!!!!!!!!!!!! Sorry, only bank 0 allowed on this screen :-O 
	   safety = 1;
	}   	
   
   // For every object in the world...
   for (zz = 0; zz < 4; zz++) {
	   for (yy = 0; yy < 10; yy++) {
		   for (xx = 0; xx < 25; xx++) {
		      obj = ds_global_map.room.objlayer[zz].obj[yy][xx];
		      bank = ds_global_map.room.objlayer[zz].bank[yy][xx];
		      // Safety check
		      if (safety) {
		         if ((obj != 0) && (bank != 0))
		         	continue;
		      }   
		      // First... something here?
		      if (obj != 0) {
		         // No? Creates an object
		         res = ds_objects_createObject(xx, yy, zz, bank, obj); 
					if (res == NULL) {
						ds_global_errorAssign(DS_C_ERR_NOMOREOBJECTS);
					   sprintf(ds_global_string,"CALL createObj\n[%d,%d,%d][b:%d,o:%d]",xx,yy,zz,bank,obj);
					   ds_global_errorHalt(ds_global_string);
					   //--HALT--//
		       	}  	
		      }   
   		}   	      
   	}   
   }   
   
   // Everything OK...
   return 1;
}   

/* Empties the actual state of the objects - a new room awaits us!!!! */
void ds_objects_reset() {
   int i;
   ds_t_object *object = NULL;
 
 	// Delete the internal fields of objects <TODO> Use Iteration
 	for (i = 0; i < ds_linkedlist_len(&ds_global_objects); i++) {
   	object = ds_linkedlist_getPos(&ds_global_objects,i);
   	_ds_objects_deleteObject(object);
	}   
	
	// Delete the contents of the list - but not the list itself
	ds_linkedlist_finish(&ds_global_objects,1,0);
	
	// Resets the direct access subsystem
	_ds_objects_initDirect();
	
	// Resets the OneCycle system
	_ds_objects_oneCycle_reset();
}

/* Activates the execution routine of an object that collided with Juni */
void ds_objects_collide(int layer, int xtile, int ytile) {
   int id = objectDirect[layer].id[ytile][xtile];
   ds_t_object *object = ds_linkedlist_get(&ds_global_objects,id,NULL);
   if (object != NULL) {
      if (object->fexecute != NULL) {
         // Execute!
      	object->fexecute((void *)object);
      	// Post-Management
      	if (object->_deleteme) {
      	   _ds_objects_deleteObjectSystem(object);
      	}   
   	}   
   }   
}   

void ds_objects_postmanage(ds_t_object *object) {
   // A.1) Delete thyself?
   if (object->_deleteme) {
      _ds_objects_deleteObjectSystem(object); // Self-node deletion is OK during iteration
      return;
   }   
   
   // A.2) Harm Juni? Copy information to the map. Also, check red glow
   if (ds_util_bitOne16(object->flags,DS_C_OBJ_F_WARN)) {
      // Red Glow
      if (ds_objects_lib_distanceJuni(object,0) < ds_global_juni.redGlow)
      	ds_global_juni.redGlow = ds_objects_lib_distanceJuni(object,0);
   }  	
   if ((ds_util_bitOne16(object->flags,DS_C_OBJ_F_HARMFUL)) && 
			(!ds_util_bitOne16(object->flags,DS_C_OBJ_F_INVISIBLE))) {
      // Damage Matrix
      int distX = ds_objects_lib_distanceJuniX(object, 0, 1);
      int distY = ds_objects_lib_distanceJuniY(object, 0, 1);
      if ((distX < (object->xs * 2)) && // Optimization:
          (distY < (object->ys * 2))) { // Only objets near Juni may harm her!!!!
         // Copy damage matrix
	      ds_map_copyFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
								 ds_3dsprite_getXSize(object->sprite),
								 ds_3dsprite_getYSize(object->sprite),
								 ds_3dsprite_getX(object->sprite),
								 ds_3dsprite_getY(object->sprite),
								 &ds_global_map.tileMapDamTemp);
      }   
	}   
	
	// A.3) Blink management
	if (object->blink > 0)
		object->blink--;
}   

/* Manages the objects of this screen */
void ds_objects_manage() {
	ds_t_object *object;   
   _ds_t_objectOC *OC;
	void *myiterator;
	ds_t_fpostmanage *postFunction;
   
   // First, empties the "I managed this type of object before" matrix + other things
   _ds_objects_initManaged();
   
   // Now, starts the management!!!
   myiterator = ds_linkedlist_startIterator(&ds_global_objects);
   while ((object = ds_linkedlist_getIterator(&ds_global_objects,&myiterator)) != NULL) {
      if (object->fmanage != NULL) {
         // Pretest: Already managed? OneCycle? 
         	// Note: Remember that OneInstance objects enter in this management area :-)
         u8 *oManaged;
         if (object->bank == DS_C_PART_BANK) 
         	oManaged = &(particleManaged[object->obj]);
         else if (object->bank == DS_C_CO_BANK) 
         	oManaged = &(coManaged[object->obj]);
         else
         	oManaged = &(objectManaged[object->bank][object->obj]);
         if (
				((ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE)) &&
         	 (*oManaged)) ||
				(ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE))
				 ) {
         	// Do nothing! Nothing?... at least the collision check!!!!!
            if ((ds_util_bitOne16(object->flags,DS_C_OBJ_F_HARMFUL)) && 
						(!ds_util_bitOne16(object->flags,DS_C_OBJ_F_INVISIBLE))) {
               // Damage Matrix
					int distX = ds_objects_lib_distanceJuniX(object, 0, 1);
					int distY = ds_objects_lib_distanceJuniY(object, 0, 1);
					if ((distX < (object->xs * 2)) && // Optimization:
					    (distY < (object->ys * 2))) { // Only objets near Juni may harm her!!!!
	               // Copy damage matrix
				      ds_map_copyFlag(ds_3dsprite_getSpriteFrame(object->sprite, ds_3dsprite_getFrame(object->sprite)),
											 ds_3dsprite_getXSize(object->sprite),
											 ds_3dsprite_getYSize(object->sprite),
											 ds_3dsprite_getX(object->sprite),
											 ds_3dsprite_getY(object->sprite),
											 &ds_global_map.tileMapDamTemp);
	            }   
         	}            	
         } else {
            // Managed...
            (*oManaged) = 1;
            
            // A) Execute Management and POSTManagement...
            object->fmanage((void *)object);
            ds_objects_postmanage(object);
         }   
      } else {
         // A) Execute *at least* POSTManagement...
         ds_objects_postmanage(object);
		}      
   }   
   
   // OK, special (OneCycle) Management
   myiterator = ds_linkedlist_startIterator(&objectOneCycle);
   while ((OC = ds_linkedlist_getIterator(&objectOneCycle,&myiterator)) != NULL) {
      // First, check if there is an object with the semaphore
      int rnd;
      if (OC->semaphore != -1) {
      	rnd = OC->semaphore; // No random, but semaphore
    	} else if (OC->instancephore > 0) {
    	   // Detect...
    	   int selit = PA_RandMinMax(0,OC->instancephore - 1); // Choose instance
		   void *myiterator = ds_linkedlist_startIterator(&OC->objects);
		   ds_t_object *objectOC;   
		   int it = 0;
		   int realit = 0;
		   rnd = -1;
		   while ((objectOC = ds_linkedlist_getIterator(&OC->objects,&myiterator)) != NULL) {
		      if (objectOC->fcondition != NULL) {
		         if (objectOC->fcondition((void *)objectOC)) { // Instance is eligible?
		            if (it == selit) { // Is the one we seek?
		            	rnd = realit; // Use this one!
		            	break;
		        		}  	
		            it++;
		         }   
		      }   
		      realit++;
		   }       	   
    	   // Empty...
    	   OC->instancephore = 0;
		} else {
    	   rnd = PA_RandMinMax(0,ds_linkedlist_len(&OC->objects) - 1); // Select by random!
		}     	
      // OK, execute one object
      object = ds_linkedlist_getPos(&OC->objects,rnd);
      if (object != NULL) {
         // First, check if DELETE!
         if (
				 (object->_deleteme) 
				 || 
			    ((!ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE)) && 
				  (!ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE)))
				 ) {
            ds_linkedlist_finish(&OC->objects,0,0);
         } else {   
            if (ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE)) {
	      		// Now, execute management and POSTManagement...
	      		object->fmanage((void *)object);
	      		ds_objects_postmanage(object);
	    		} else 
				if (ds_util_bitOne16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE)) {
	      		// This instance will execute an instance-specific manager...
	      		object->finstance((void *)object);
	      			// No need to postmanage, since it was executed before 
	    		}  	
	   	}   
   	}   
   } 
   
   // And now, post-management
   myiterator = ds_linkedlist_startIterator(&objectManageAfter);
   while ((postFunction = ds_linkedlist_getIterator(&objectManageAfter,&myiterator)) != NULL) {
      postFunction();
   }    
   ds_linkedlist_finish(&objectManageAfter,0,0);
}

/* Changes the semaphore of a certain OC - Internal method */
void _ds_objects_semaphore(ds_t_object *object, int semaphore) {
   _ds_t_objectOC *OC;
   ds_t_object *objectOC;
   void *myiterator;
   int it;
   
   // First, find the OC where the objects are stored
   myiterator = ds_linkedlist_startIterator(&objectOneCycle);
   while ((OC = ds_linkedlist_getIterator(&objectOneCycle,&myiterator)) != NULL) {
      if ((object->bank == OC->bank) && (object->obj == OC->obj)) {
         break;
		}
   }
   if (OC == NULL)
   	return;
   
   // First.5, if the semaphore is off, just turn it off
   if (!semaphore) {
      OC->semaphore = -1;
      return;
   }   
   
   // Second, get the index of the object and store it on the semaphore
   myiterator = ds_linkedlist_startIterator(&OC->objects);
   it = 0;
   while ((objectOC = ds_linkedlist_getIterator(&OC->objects,&myiterator)) != NULL) {
      if ((object->xtile == objectOC->xtile) && (object->ytile == objectOC->ytile)
		    && (object->layer == objectOC->layer)) {
         OC->semaphore = it;
         break;
		}
		it++;
   }   
}

/* Tells the system that a certain instance wants to be executed */
void ds_objects_instancephore(ds_t_object *object) {
   _ds_t_objectOC *OC;
   void *myiterator;
   
   // First, find the OC where the objects are stored
   myiterator = ds_linkedlist_startIterator(&objectOneCycle);
   while ((OC = ds_linkedlist_getIterator(&objectOneCycle,&myiterator)) != NULL) {
      if ((object->bank == OC->bank) && (object->obj == OC->obj)) {
         break;
		}
   }
   if (OC == NULL)
   	return;
   
   // OK, signal this event
   OC->instancephore++;
}


/* Changes the semaphore of a certain OC - ON */
void ds_objects_semaphoreON(ds_t_object *object) {
	_ds_objects_semaphore(object, 1);   
}

/* Changes the semaphore of a certain OC - OFF */
void ds_objects_semaphoreOFF(ds_t_object *object) {
	_ds_objects_semaphore(object, 0);	
}

/* Set the "blink" property of an object */
void ds_objects_setBlink(ds_t_object *object, u8 blink) {
   if (object != NULL)
   	object->blink = blink;
}   



/* 

- "One Cycle" Management (Type two)

OC Linked list of BO (BO-OCOCOC , BO-OCOCOC, ...)
Iteration:
   Do not manage OCs
2nd Iteration:
   Manage OCs

*/   
