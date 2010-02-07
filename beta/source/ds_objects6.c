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
#include "ds_objects.h"
#include "ds_objects_lib.h"
#include "ds_util.h"
#include "ds_world.h"
#include "ds_map.h"
#include "ds_juni.h"
#include "ds_music.h"


// BANK 6 [B06]
//=================================================================================================

// BLOCK [B06_O01]
//..........................................................................................
int _ds_objects_b06o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b06o01_manage(void *objp) {
   ds_t_object *object = objp;

	int wasCero = (object->inner[0] == 0);
	int direction = object->inner[2];
	ds_objects_lib_beh_disk(object, 0,
									 0, 
									 8, 1, 0,
									 -1,
									 1000000, // :-P 
									 0, 0);
	if (wasCero) {
	   object->inner[2] = -1;
	} else {
		if (direction != object->inner[2]) {
			if (object->inner[2] == -1) {
				ds_music_playSound("Machine Turn A", 0, 0);
				object->inner[3] = 8;
			} else { 
				ds_music_playSound("Machine Turn B", 0, 0);
				object->inner[3] = 16;
			}
		}
	}   

   
   return 1;
}

// SPIKEBLOCK [B06_O02-04]
//..........................................................................................
int _ds_objects_b06spikeblock_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	//object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b06spikeblockGREY_manage(void *objp) {
   ds_t_object *object = objp;
   
   ds_objects_lib_beh_spikeFloat(object, 10, 10);
   
   return 1;
}

int _ds_objects_b06spikeblockRED_manage(void *objp) {
   ds_t_object *object = objp;
   
   ds_objects_lib_beh_spikeFloat(object, 5, 15);
   
   return 1;
}

int _ds_objects_b06spikeblockWHITE_manage(void *objp) {
   ds_t_object *object = objp;
   
   ds_objects_lib_beh_spikeFloat(object, 15, 15);
   
   return 1;
}

// EATER [B06_O05]
//..........................................................................................
int _ds_objects_b06o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b06o05_manage(void *objp) {
   ds_t_object *object = objp;

	switch (object->inner[10]) {
	   case 0:
	      if (ds_objects_lib_stepObject(object)) {
		    ds_music_playSound("Chomp", 0, 0);
	      	object->inner[10] = 1;
	      	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	    	}  	
	      break;
	   case 1:
	      if (ds_objects_lib_beh_anim(object,2,1)) {
	         object->inner[10] = 2;
	         object->y -= (object->ys >> 1);
	         ds_3dsprite_setY(object->sprite,object->y);
	      }   
	      break;
	   case 2:
	      ds_objects_lib_beh_anim(object,2,1);
	      break;
	}      

   return 1;
}

// EVIL FIRE [B06_O06]
//..........................................................................................
int _ds_objects_b06oFire_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_3dsprite_setAlpha(object->sprite,192);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b06oFire_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   
	ds_objects_lib_beh_cycle(object,12);

	object->inner[10]++;
	if (object->inner[10] == 180) {
	   // Particle creation! Warn!	   
	   particle = ds_objects_createParticle(object->x, object->y, object->layer, 48);
		ds_3dsprite_setAlpha(particle->sprite,1);
	} else if (object->inner[10] == 240) {
	   object->inner[10] = 0;
	   
	   // Particle creation! Attack!	   
	   ds_music_playSound("Fire Shot", 0, 0);
		int dJX = ds_objects_lib_distanceJuniX(object, 1, 0); 
		int dJY = ds_objects_lib_distanceJuniY(object, 1, 0); 
		int dJ = ds_objects_lib_distancePhyJuniCorrected(object, 1);
		int s = 20; // Particle Speed
		int sx = (s * dJX) / dJ;
		int sy = (s * dJY) / dJ;
	   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 45);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		0, 0, 3);
		ds_3dsprite_setAlpha(particle->sprite,128);
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	} 
   return 1;
}


// STUFF DROPPER / SHOOTER [B06_O07-09]
//..........................................................................................
int _ds_objects_b06shooter_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b06shooter_manageDROP(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
	if (ds_objects_lib_beh_trapShoot(object, 5, 200, 2)) {
		ds_music_playSound("Mega Split", 0, 0);
	   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 49);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
						0, 
						30, 
				 		0, 0, 3);
		particle->inner[11] = -1;
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);	   
	}   
   return 1;
}   

int _ds_objects_b06shooter_manageSHOOT(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
	if (ds_objects_lib_beh_trapShoot(object, 72, 200, 2)) {
		ds_music_playSound("Mega Split", 0, 0);
	   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 49);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
						0, 
						-30, 
				 		0, 0, 3);
		particle->inner[11] = 1;
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);	   
	}      
   return 1;
}   

// SELF-DROPPER [B06_O08]
//..........................................................................................
int _ds_objects_b06o08_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b06o08_manage(void *objp) {
   ds_t_object *object = objp;

	switch (object->inner[11]) {
	   case 0:
	      // Pre-FALL
	      if (ds_objects_lib_distanceJuniX(object,1,1) <= 12) {
	         ds_objects_lib_beh_particleMMF2_Init(object, 
						0, 
						10, 
				 		10, 0, 0);
	         object->inner[11] = 1;
	      }   
	      break;
	   case 1:
	      // FALL
	      if (ds_objects_lib_beh_particleMMF2(object,6)) {
	         // It collided! - do not destroy it, just continue!
	         object->_deleteme = 0;
			 ds_music_playSound("Selfdrop", 0, 0);
	         // Also, put the critter in its place
	         int i;
		      for (i = 1; i <= (object->inner[0] / 100); i++) {
		         if (!ds_map_collDownBasic(object->x,object->y,23 - i,0)) {
		            object->y -= i;
		            ds_3dsprite_setY(object->sprite,object->y);
		            break;
		         }   
		      }   
	         // Next state...
	         object->inner[11] = 2;
	      }   
	      // Anyway, move the sprite
		   if ((ds_global_tick % 3) == 0) {
		      int mxF = ds_3dsprite_getMaxFrame(object->sprite);
		      if (object->inner[9] < mxF) {
				   object->inner[9]++;
				   if (object->inner[9] < mxF) {
				      ds_3dsprite_setFrame(object->sprite,object->inner[9]);
				   }  		         
		      }
		   }
	      break;
	   case 2:
	      // POST-FALL
	      	// Nothing to do...well, except with one thing: not harmful anymore :-)
	      object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_HARMFUL);
	      object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_WARN);
	      break;
	}   
   
   return 1;
}

// SPIKES [B06_O10-13]
//..........................................................................................
int _ds_objects_b06spike_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
	//object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b06spike_manage(void *objp) {
   ds_t_object *object = objp;

	if ((ds_util_bitOne16(object->flags,DS_C_OBJ_F_INVISIBLE)) && 
	    (ds_objects_lib_distanceJuni(object,1) < 60)) {
		ds_music_playSound("Spike Up", 0, 0);
		object->flags = ds_util_bitDel16(object->flags,DS_C_OBJ_F_INVISIBLE);
		ds_3dsprite_markInvisible(object->sprite,0);
	}		
	else if ((!ds_util_bitOne16(object->flags,DS_C_OBJ_F_INVISIBLE)) && 
	         (ds_objects_lib_distanceJuni(object,1) > 100)) {
			 ds_music_playSound("Spike Down", 0, 0);
   	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_INVISIBLE);
   	ds_3dsprite_markInvisible(object->sprite,1);
 	}  	
   
   return 1;
}   
   

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects6_assign(u8 obj, ds_t_object *object) {
   if ((obj >= 10) && (obj <= 13)) {
      // Spikes
      object->fcreate = _ds_objects_b06spike_create;
      object->fmanage = _ds_objects_b06spike_manage;
      return 1;               
   }
   switch (obj) {
      case 1:
         // Block
         object->fcreate = _ds_objects_b06o01_create;
         object->fmanage = _ds_objects_b06o01_manage;
         return 1;
         break; // not really necessary...            
      case 2:
         // SpikeBlock Grey
         object->fcreate = _ds_objects_b06spikeblock_create;
         object->fmanage = _ds_objects_b06spikeblockGREY_manage;
         return 1;
         break; // not really necessary...            
      case 3:
         // SpikeBlock Red
         object->fcreate = _ds_objects_b06spikeblock_create;
         object->fmanage = _ds_objects_b06spikeblockRED_manage;
         return 1;
         break; // not really necessary...            
      case 4:
         // SpikeBlock White
         object->fcreate = _ds_objects_b06spikeblock_create;
         object->fmanage = _ds_objects_b06spikeblockWHITE_manage;
         return 1;
         break; // not really necessary...            
      case 5:
         // Eater
         object->fcreate = _ds_objects_b06o05_create;
         object->fmanage = _ds_objects_b06o05_manage;
         return 1;
         break; // not really necessary...            
      case 6:
         // Evil Fire
         object->fcreate = _ds_objects_b06oFire_create;
         object->fmanage = _ds_objects_b06oFire_manage;
         return 1;
         break; // not really necessary...            
      case 7:
         // Stuff Dropper
         object->fcreate = _ds_objects_b06shooter_create;
         object->fmanage = _ds_objects_b06shooter_manageDROP;
         return 1;
         break; // not really necessary...            
      case 8:
         // Self-Dropper
         object->fcreate = _ds_objects_b06o08_create;
         object->fmanage = _ds_objects_b06o08_manage;
         return 1;
         break; // not really necessary...            
      case 9:
         // Up Shooter
         object->fcreate = _ds_objects_b06shooter_create;
         object->fmanage = _ds_objects_b06shooter_manageSHOOT;
         return 1;
         break; // not really necessary...            
   }
   return 0;
}
