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
#include <math.h>

// BANK 11 [B11]
//=================================================================================================

// RED DISK [B11_O01]
//..........................................................................................
int _ds_objects_b11o01_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b11o01_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_disk(object, 1,
									 12, // 30 / 1.6
									 0, 3, 3,
									 55, // 88 / 1.6
									 3, 4, 0);
      
   return 1;
}

// SHOCK B [B11_O02]
//..........................................................................................
int _ds_objects_b11o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b11o02_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res = ds_objects_lib_beh_disk(object, 1, 
									 34, // 55 / 1.6
									 0, 3, 3,
									 -1, // ...
									 3, 4, 4);
	if (res) {
	   // Particle creation!
	   ds_music_playSound("DiscBullet", 0, 0);
	   int i,s,sx,sy;
	   for (i=0; i<17; i++) {
	      s = 30; // Speed - 30
		   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 40);
		   sx = ((90 - (11.25 * i)) * s) / 90; // r=11.25, sx=30
		   //sy = 0 - (s - abs(sx)); // r=11.25, sy=30 
		   if (i == 0) {
		      sy = 0;
		   } else if (i < 8) {
		      sy = 0 - ((i * 4) + 1);
			} else if (i == 8) {
			   sy = -30;
			} else if (i < 17) {
			   sy = 0 - (((i - ((i - 8) * 2)) * 4) + 1);
			} else {
			   sy = 0;
			}               
		   if (sx > 0)
		   	sx = sqrt((s*s) - (sy*sy));
		   else
				sx = 0 - sqrt((s*s) - (sy*sy));
		   //d2 = x2+y2
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		0, 20, 3);

	   }		 
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   
      
   return 1;
}

int _ds_objects_b11o02_instance(void *objp) {
   ds_t_object *object = objp;
   
   if ((ds_global_tick % 187) == 0) // 300 / 1.6
   	object->inner[0] = 2;
      
   return 1;
}

// PURPLE DISK [B11_O03]
//..........................................................................................
int _ds_objects_b11o03_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b11o03_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_disk(object, 1,
									 22, // 35 / 1.6
									 0, 3, 4,
									 40, // 65 / 1.6
									 3, 4, 0);
      
   return 1;
}

// PURPLE SPIKE [B11_O04]
//..........................................................................................
int _ds_objects_b11o04_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b11o04_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_disk(object, 1,
									 22, // 35 / 1.6
									 0, 3, 4,
									 40, // 65 / 1.6
									 3, 4, 0);
      
   return 1;
}

// RED SPIKE [B11_O05]
//..........................................................................................
int _ds_objects_b11o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b11o05_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_disk(object, 1,
									 22, // 35 / 1.6
									 0, 3, 3,
									 40, // 65 / 1.6
									 3, 4, 0);
      
   return 1;
}

// SHOCK [B11_O06]
//..........................................................................................
int _ds_objects_b11o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b11o06_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int res;

	res = ds_objects_lib_beh_disk(object, 1,
									 79, // 1.27 / 1.6
									 5, 2, 4,
									 -1, // ...
									 3, 4, 4);

	if (res) {
	   // Particle creation!
	   ds_music_playSound("DiscBullet", 0, 0);
	   int i,s,sx,sy;
	   for (i=0; i<17; i++) {
	      s = PA_RandMax(35) + 20; // Speed - RAND(35)+20
		   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 40);
		   if (i == 0) {
		      sy = 0;
		   } else if (i < 8) {
		      sy = 0 - ((s / 7) * i);
			} else if (i == 8) {
			   sy = -s;
			} else if (i < 16) {
			   sy = 0 - ((s / 7) * (16 - i));
			} else {
			   sy = 0;
			}       
			sx = (90 - (11.25 * i));        
		   if (sx > 0)
		   	sx = sqrt((s*s) - (sy*sy));
		   else
				sx = 0 - sqrt((s*s) - (sy*sy));
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		0, 20, 3);
	   }   
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}   
      
   return 1;
}

int _ds_objects_b11o06_instance(void *objp) {
   ds_t_object *object = objp;
   
   if ((ds_global_tick % 138) == 0) // 222 / 1.6
   	object->inner[0] = 2;
      
   return 1;
}

// BIG DISK [B11_O07]
//..........................................................................................
int _ds_objects_b11o07_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b11o07_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_disk(object, 1,
									 28, // 45 / 1.6
									 0, 3, 3,
									 103, // 1.65 / 1.6
									 3, 5, 0);
      
   return 1;
}

// TENTACLE DISK [B11_O08]
//..........................................................................................
int _ds_objects_b11o08_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b11o08_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_disk(object, 1,
									 22, // 35 / 1.6
									 0, 3, 3,
									 40, // 65 / 1.6
									 3, 4, 0);
      
   return 1;
}

// EVIL DISK [B11_O09]
//..........................................................................................
int _ds_objects_b11o09_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b11o09_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
	int res;
	
	res = ds_objects_lib_beh_disk(object, 1,
									 79, // 1.27 / 1.6
									 5, 2, 4,
									 -1, //...
									 3, 4, 4);
	if (res) {
	   // Particle creation!
	   ds_music_playSound("DiscBullet", 0, 0);
	   int i,s,sx,sy;
	   for (i=0; i<17; i++) {
	      s = 43; // Speed - 40... (more speed to be close to original KS)
		   particle = ds_objects_createParticle(object->x + 8, object->y - 4, object->layer, 41);
		   if (i == 0) {
		      sy = 0;
		   } else if (i < 8) {
		      sy = 0 - ((s / 7) * i);
			} else if (i == 8) {
			   sy = -s;
			} else if (i < 16) {
			   sy = 0 - ((s / 7) * (16 - i));
			} else {
			   sy = 0;
			}       
			sx = (90 - (11.25 * i));        
		   if (sx > 0)
		   	sx = sqrt((s*s) - (sy*sy));
		   else
				sx = 0 - sqrt((s*s) - (sy*sy));
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		10, 0, 3); // Less gravity, need to be similar to original...

	   }		 
	   ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	}         
   return 1;
}

int _ds_objects_b11o09_instance(void *objp) {
   ds_t_object *object = objp;
   
   if ((ds_global_tick % 188) == 0) // 300 / 1.6
   	object->inner[0] = 2;
      
   return 1;
}

// YELLOW DISK [B11_O10]
//..........................................................................................
int _ds_objects_b11o10_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b11o10_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_disk(object, 1,
									 28, // 45 / 1.6
									 0, 3, 9,
									 110, // 1.77 / 1.6
									 3, 4, 0);
      
   return 1;
}

// YELLOW^2 DISK - Yup, exclusive of KSDS! :-) [B11_O11]
//..........................................................................................
int _ds_objects_b11o11_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b11o11_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_disk(object, 1,
									 28, // 45 / 1.6
									 0, 3, 9,
									 110, // 1.77 / 1.6
									 3, 4, 0);
      
   return 1;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects11_assign(u8 obj, ds_t_object *object) {
   switch (obj) {
      case 1:
      	// RED DISK
      	object->fcreate = _ds_objects_b11o01_create;
      	object->fmanage = _ds_objects_b11o01_manage;
      	return 1;
      	break; // not really necessary...            
      case 2:
      	// SHOCK B
      	object->fcreate = _ds_objects_b11o02_create;
      	object->fmanage = _ds_objects_b11o02_manage;
      	object->finstance = _ds_objects_b11o02_instance;
      	return 1;
      	break; // not really necessary...            
      case 3:
      	// PURPLE DISK
      	object->fcreate = _ds_objects_b11o03_create;
      	object->fmanage = _ds_objects_b11o03_manage;
      	return 1;
      	break; // not really necessary...            
      case 4:
      	// PURPLE SPIKE
      	object->fcreate = _ds_objects_b11o04_create;
      	object->fmanage = _ds_objects_b11o04_manage;
      	return 1;
      	break; // not really necessary...            
      case 5:
      	// RED SPIKE
      	object->fcreate = _ds_objects_b11o05_create;
      	object->fmanage = _ds_objects_b11o05_manage;
      	return 1;
      	break; // not really necessary...            
      case 6:
      	// SHOCK
      	object->fcreate = _ds_objects_b11o06_create;
      	object->fmanage = _ds_objects_b11o06_manage;
      	object->finstance = _ds_objects_b11o06_instance;
      	return 1;
      	break; // not really necessary...            
      case 7:
      	// BIG DISK
      	object->fcreate = _ds_objects_b11o07_create;
      	object->fmanage = _ds_objects_b11o07_manage;
      	return 1;
      	break; // not really necessary...            
      case 8:
      	// TENTACLE DISK
      	object->fcreate = _ds_objects_b11o08_create;
      	object->fmanage = _ds_objects_b11o08_manage;
      	return 1;
      	break; // not really necessary...            
      case 9:
      	// EVIL DISK
      	object->fcreate = _ds_objects_b11o09_create;
      	object->fmanage = _ds_objects_b11o09_manage;
      	object->finstance = _ds_objects_b11o09_instance;
      	return 1;
      	break; // not really necessary...            
      case 10:
      	// YELLOW DISK
      	object->fcreate = _ds_objects_b11o10_create;
      	object->fmanage = _ds_objects_b11o10_manage;
      	return 1;
      	break; // not really necessary...            
      case 11:
      	// YELLOW DISK^2
      	object->fcreate = _ds_objects_b11o11_create;
      	object->fmanage = _ds_objects_b11o11_manage;
      	return 1;
      	break; // not really necessary...            

   }      
	return 0;   
}

/*

		   sx = ((90 - (11.25 * i)) * s) / 90; // r=11.25, sx=30
		   sy = 0 - (s - abs(sx)); // r=11.25, sy=30
		   if (sx > 0)
		   	sx = sqrt((s*s) - (sy*sy));
		   else
				sx = 0 - sqrt((s*s) - (sy*sy));
		   //d2 = x2+y2
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		0, 20, 3);

*/
