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
#include "ds_3dsprite.h"
#include "ds_objects.h"

// BANK 2 [B02]
//=================================================================================================
   
// BLUE FLYER [B02_O01]
//..........................................................................................
int _ds_objects_b02o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o01_manage(void *objp) {
   ds_t_object *object = objp;
	ds_objects_lib_beh_flyAround(object,2,1*24,4*24,0,0,2,2,0,-1,2);
   return 1;
}   

// EVIL FLYER [B02_O02]
//..........................................................................................
int _ds_objects_b02o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <= This is an optimization
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o02_manage(void *objp) {
   ds_t_object *object = objp;
   // Fly..
	ds_objects_lib_beh_flyAround(object,2,1*24,4*24,0,0,2,2,0,-1,2);
	
	// Do evil deeds!!
	ds_objects_lib_par_make6FlyingBullets(object, 4, 2, 0);
   return 1;
}   

// FLIES [B02_O03-04]
//..........................................................................................
int _ds_objects_b02ofly_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Too small! - object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02ofly_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Fly..
	ds_objects_lib_beh_flyRandom(object, 1, 0, 0, 5 * 60, 1);
	
   return 1;
}   

// GREEN FLYER [B02_O05]
//..........................................................................................
int _ds_objects_b02o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);// <= This is an optimization
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o05_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Fly...
	ds_objects_lib_beh_flyAround(object,2,1*24,4*24,0,0,2,2,0,-1,2);
	
	// Attack!!!
	ds_objects_lib_par_make4FlyingBullets(object,3,2,0);
   return 1;
}   

// SHADOW FLYER [B02_O06]
//..........................................................................................
int _ds_objects_b02o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);// <= This is an optimization
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o06_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Fly...
	ds_objects_lib_beh_flyAround(object,2,1*24,4*24,0,0,2,2,0,-1,2);
	
	// And show your moves!
	ds_objects_lib_par_make4Following(object, 4, 1, (2 << 10), (2 << 10), 50, 0, 5, 5);
   return 1;
}   

// BROWN FLYER [B02_O07]
//..........................................................................................
int _ds_objects_b02o07_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o07_manage(void *objp) {
   ds_t_object *object = objp;
	ds_objects_lib_beh_flyAround(object,2,1*24,4*24,0,0,2,2,0,-1,2);
   return 1;
}

// PARTICLE FLYER [B02_O08-13-23]
//..........................................................................................
int _ds_objects_b02opart_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b02opart_manage(void *objp) {
   ds_t_object *object = objp;
	ds_objects_lib_beh_flyParticle(object,6,6,20);
   return 1;
}      

// YELLOW WASP [B02_O09]
//..........................................................................................
int _ds_objects_b02o09_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
	ds_objects_lib_beh_floatLeftRight_ini(object, 1, 1);
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o09_manage(void *objp) {
   ds_t_object *object = objp;
   /* Behaviour */
   ds_objects_lib_beh_floatLeftRight(object, 
											1, 1, 
											0, 0, -1, -1, 0, 0, 
											0, 1,
											1,0);
	if ((ds_global_tick % 12) < 3)
		ds_3dsprite_setY(object->sprite,object->y);
	else 
	if ((ds_global_tick % 12) < 6)
		ds_3dsprite_setY(object->sprite,object->y - 1);
	else 
	if ((ds_global_tick % 12) < 9)
		ds_3dsprite_setY(object->sprite,object->y - 2);
	else
	if ((ds_global_tick % 12) < 12)
		ds_3dsprite_setY(object->sprite,object->y - 1);
	/* Bullet */
	ds_objects_lib_beh_beeBullet(object, 13, 4, 30, 2);
   return 1;
}   

// RED WASP [B02_O10]
//..........................................................................................
int _ds_objects_b02o10_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
	ds_objects_lib_beh_floatLeftRight_ini(object, 1, 1);
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o10_manage(void *objp) {
   ds_t_object *object = objp;
   ds_objects_lib_beh_floatLeftRight(object, 
											4, 4, 
											0, 0, -1, -1, 0, 0, 
											2, 0,
											1,0);
	if ((ds_global_tick % 16) < 4)
		ds_3dsprite_setY(object->sprite,object->y);
	else 
	if ((ds_global_tick % 16) < 8)
		ds_3dsprite_setY(object->sprite,object->y - 1);
	else 
	if ((ds_global_tick % 16) < 12)
		ds_3dsprite_setY(object->sprite,object->y - 2);
	else
	if ((ds_global_tick % 16) < 16)
		ds_3dsprite_setY(object->sprite,object->y - 1);
	/* Bullet */
	ds_objects_lib_beh_beeBullet(object, 15, 3, 30, 3);											
   return 1;
}   

// HOMING CIRCLE [B02_O11]
//..........................................................................................
int _ds_objects_b02o11_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);// <= This is an optimization		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
			
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	ds_objects_lib_beh_entityFollowing_Init(object, 0, 0, 0, 50, 0, 512 + 128 + 128, 512 + 128 + 128);
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o11_manage(void *objp) {
   ds_t_object *object = objp;
   
   ds_objects_lib_beh_entityFollowing(object,1);
	
   return 1;
}   

// HOMING RECTANGLE [B02_O12]
//..........................................................................................
int _ds_objects_b02o12_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);// <= This is an optimization		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	ds_objects_lib_beh_entityFollowing_Init(object, 0, 0, 0, 50, 0, 512 + 128 + 128 + 128, 512 + 128 + 128 + 128);
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o12_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_entityFollowing(object,1);
	
   return 1;
}   

// TENTACLE FLYER [B02_O14]
//..........................................................................................
int _ds_objects_b02o14_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);	// <= This is an optimization	
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o14_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Normal
	ds_objects_lib_beh_flyAround(object,2,1*24,4*24,0,0,2,2,0,-1,2);
	
	// Particles!
	ds_objects_lib_par_make2ParticleFalling(object, 2, 2, (2 << 10), 15);
	
   return 1;
}   

// PROPELLER DUDE [B02_O15]
//..........................................................................................
int _ds_objects_b02o15_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);// <= This is an optimization
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o15_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Normal
	ds_objects_lib_beh_flyAround(object,2,0,3*24,0,30,2,2,0,-1,3);   
	
   return 1;
}   

// PROPELLER CHILD [B02_O16]
//..........................................................................................
int _ds_objects_b02o16_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);// <= This is an optimization
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o16_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Normal
	ds_objects_lib_beh_flyAround(object,2,0,3*24,0,30,2,2,0,-1,3);   
	
   return 1;
}   

// ORANGE PROPELLER CHILD [B02_O17]
//..........................................................................................
int _ds_objects_b02o17_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);// <= This is an optimization
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o17_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Normal
	ds_objects_lib_beh_flyAround(object,2,0,3*24,0,30,2,2,0,-1,3);   
	
   return 1;
}

// YELLOW ELEMENTAL [B02_O18]
//..........................................................................................
int _ds_objects_b02o18_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o18_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   
	if (ds_objects_lib_beh_elemental(object,6,6,120 >> 1)) {
	   // Create special particle
	   int corr = (120 - 24) >> 1;
      particle = ds_objects_createParticle(object->x - corr, object->y - corr, 
							object->layer, 47);
		// Bye bye Juni!!!!! (Insta-kill!)	   
		ds_global_juni.killme = 1;
 		// Also... kill myself! (optimization :-P)
		object->_deleteme = 1;
	}      
	
   return 1;
}      

// RED ELEMENTAL [B02_O19]
//..........................................................................................
int _ds_objects_b02o19_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o19_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   
	if (ds_objects_lib_beh_elemental(object,6,6,96 >> 1)) {
	   // Create special particle
	   int corr = (96 - 24) >> 1;
      particle = ds_objects_createParticle(object->x - corr, object->y - corr, 
							object->layer, 46);
		// Bye bye Juni!!!!! (Insta-kill!)
		ds_global_juni.killme = 1;
 		// Also... kill myself! (optimization :-P)
		object->_deleteme = 1;
	}      
	
   return 1;
}      

// GAP JUMPER A [B02_O20]
//..........................................................................................
int _ds_objects_b02o20_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
			
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	ds_objects_lib_beh_floatLeftRight_ini(object, 1, 1);
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o20_manage(void *objp) {
   ds_t_object *object = objp;
   ds_objects_lib_beh_floatLeftRight(object, 
											3, 3, 
											0, 0, -1, -1, 3 * 60, 3 * 60, 
											0, 0,
											1,1);
	if (object->inner[0] == 1) {
		if ((ds_global_tick % 16) < 4)
			ds_3dsprite_setY(object->sprite,object->y);
		else 
		if ((ds_global_tick % 16) < 8)
			ds_3dsprite_setY(object->sprite,object->y - 1);
		else 
		if ((ds_global_tick % 16) < 12)
			ds_3dsprite_setY(object->sprite,object->y - 2);
		else
		if ((ds_global_tick % 16) < 16)
			ds_3dsprite_setY(object->sprite,object->y - 1);
	}		
   return 1;
}   

// GAP JUMPER B [B02_O21]
//..........................................................................................
int _ds_objects_b02o21_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
			
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	ds_objects_lib_beh_floatLeftRight_ini(object, 1, -1);
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o21_manage(void *objp) {
   ds_t_object *object = objp;
   ds_objects_lib_beh_floatLeftRight(object, 
											3, 3, 
											0, 0, -1, -1, 3 * 60, 3 * 60, 
											0, 0,
											1,1);
	if (object->inner[0] == 1) {
		if ((ds_global_tick % 16) < 4)
			ds_3dsprite_setY(object->sprite,object->y);
		else 
		if ((ds_global_tick % 16) < 8)
			ds_3dsprite_setY(object->sprite,object->y - 1);
		else 
		if ((ds_global_tick % 16) < 12)
			ds_3dsprite_setY(object->sprite,object->y - 2);
		else
		if ((ds_global_tick % 16) < 16)
			ds_3dsprite_setY(object->sprite,object->y - 1);
	}		
   return 1;
}   

// QUAD FLYER [B02_O22]
//..........................................................................................
int _ds_objects_b02o22_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);	// <= This is an optimization	
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o22_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Normal
	ds_objects_lib_beh_flyAround(object,2,1*24,4*24,0,0,2,2,0,-1,2);
	
	// Particles!!!!!
	ds_objects_lib_par_make4ParticleFalling(object, 3, 2, (2 << 10), 10);
	
   return 1;
}   

// FALLER [B02_O24]
//..........................................................................................
int _ds_objects_b02o24_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
			
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o24_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Normal
	ds_objects_lib_beh_flyAroundFaller(object,2,1*24,5*24,0,0,2,1,24 * 4,2);
	
   return 1;
}

// FLYING (ARC) BIRDS [B02_O25-26-27-29]
//..........................................................................................
int _ds_objects_b02oflyArc_create(u8 bank, u8 obj, void *objp) {
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

int ds_b02_FlyUp = 14;
int ds_b02_FlyUpS[] = {10,  8,  6,  4,   2,   1,   1,   1,   2,   4,  6,  8,  10, 240};
int ds_b02_FlyUpMX[] = {0,  0,  0,  0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0};
int ds_b02_FlyUpMY[] = {0,-48,-75,-93,-103,-107,-115,-111,-107,-103,-93,-75, -48,   0};

int _ds_objects_b02o25_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_followPixelPathLR(object, ds_b02_FlyUp, 
											ds_b02_FlyUpS, ds_b02_FlyUpMX, ds_b02_FlyUpMY,
											3,
											1);
											
	int dir = ds_objects_lib_distanceJuniX(object,1,0);
	int iniSL = ds_3dsprite_getIniFrame_LR(object->sprite,-1,1);
	int lookingR = (object->inner[1] < iniSL);
	if ((dir >= 0) && (!lookingR)) {
	   object->inner[1] = object->inner[1] - iniSL;
	   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	   object->inner[10] = 1;
	} else if ((dir <= 0) && (lookingR)) {
	   object->inner[1] = object->inner[1] + iniSL;
	   ds_3dsprite_setFrame(object->sprite,object->inner[1]);
	   object->inner[10] = -1;
	}      
	
   return 1;
}

int ds_b02_FlyL = 20;
int ds_b02_FlyLS[] = {10, 10, 10, 10, 10, 10, 10, 10, 10,240, 10, 10, 10, 10, 10, 10, 10, 10, 10,240};
int ds_b02_FlyLMX[] = {0, 33, 75,135,203,285,352,418,468,524,524,468,418,352,285,203,135, 75, 33,  0};
int ds_b02_FlyLMY[] = {0,-10,-18,-28,-33,-33,-30,-22,-14,  0,  0,-14,-22,-30,-33,-33,-28,-18,-10,  0};

int _ds_objects_b02o26_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_followPixelPathLR(object, ds_b02_FlyL, 
											ds_b02_FlyLS, ds_b02_FlyLMX, ds_b02_FlyLMY,
											3,
											1);
   return 1;
}

int ds_b02_FlyR = 21;
int ds_b02_FlyRS[] = {20, 20,  20,  20,  20,  20,  20,  20,  20, 240,  20,  20,  20,  20,  20,  20,  20,  20, 20,20,240};
int ds_b02_FlyRMX[] = {0,-52,-111,-174,-240,-303,-372,-435,-489,-529,-529,-489,-435,-372,-303,-240,-174,-111,-52, 0,  0};
int ds_b02_FlyRMY[] = {0,-12, -21, -23, -25, -25, -23, -19, -10,   0,   0, -10, -19, -23, -25, -25, -23, -21,-12, 0,  0};

int _ds_objects_b02o27_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_followPixelPathLR(object, ds_b02_FlyR, 
											ds_b02_FlyRS, ds_b02_FlyRMX, ds_b02_FlyRMY,
											3,
											1);
   return 1;
}

int ds_b02_FlyX = 24;
int ds_b02_FlyXS[] = {   8,  6,  4,  3,  4,  4,  3,   3,   4,   6,   8,   8,   6,   4,   3,   3,   4,   6,  8,  6,  4,  4,  6,180};
int ds_b02_FlyXMX[] = {158,145,138,135,144,159,171, 178, 172, 158,  72,  36,  16,   4,   0,   4,  18,  55,173,189,196,194,174,158};
int ds_b02_FlyXMY[] = {  0,-11,-27,-54,-71,-83,-97,-116,-135,-152,-175,-175,-170,-162,-148,-130,-114,-104,-79,-68,-54,-39,-13,  0};

int _ds_objects_b02o29_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_followPixelPathLR(object, ds_b02_FlyX, 
											ds_b02_FlyXS, ds_b02_FlyXMX, ds_b02_FlyXMY,
											3,
											1);
   return 1;
}

// WALK BIRDIE [B02_O28]
//..........................................................................................
int _ds_objects_b02o28_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b02o28_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_diskMovementLR(object, 1, // Move on X 
									 48, 48, // On 0.8 sec, change speed
									 PA_RandMinMax(0,1) * (8 + (PA_RandMinMax(0,6))), 6, 0,
									 0,0,0,0, // Calculate gestures automatically
									 246, 100, // On 4.1 sec, change direction
									 1, // Always cycle
									 0, // Leave Juni alone
									 3,
									 0,
									 0);
   return 1;
}
   
// BLUE BIRDIE [B02_O30]
//..........................................................................................
int _ds_objects_b02o30_create(u8 bank, u8 obj, void *objp) {
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

int ds_b02_Blue = 7;
int ds_b02_BlueG[] = {2,8,14,20,26,29,32};

int _ds_objects_b02o30_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_knyttNgestures(object,ds_b02_Blue,ds_b02_BlueG,189,5,1);
   return 1;
}

// FAR AWAY BIRDS [B02_O31]
//..........................................................................................
int _ds_objects_b02o31_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b02o31_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_diskMovementLR(object, 1, // Move on X 
									 -1, 48, // NEVER speed
									 4, 5, 1,
									 0,0,0,0, // Calculate gestures automatically
									 -1, 100, // NEVER change direction
									 1, // Always cycle
									 0, // Leave Juni alone
									 3,
									 1, // CYCLE
									 0);
   return 1;
}

// HOMING HUNTER [B02_O32]
//..........................................................................................
int _ds_objects_b02o32_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
			
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	ds_objects_lib_beh_entityFollowing_Init(object, 0, 0, 0, 50, 0, 512 + 512 + 256, 512 + 512 + 256);
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b02o32_manage(void *objp) {
   ds_t_object *object = objp;
   
   ds_objects_lib_beh_entityFollowing(object,0);
	
   return 1;
}   

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects2_assign(u8 obj, ds_t_object *object) {
   if ((obj == 3) || (obj == 4)) {
      // Flyers
      object->fcreate = _ds_objects_b02ofly_create;
      object->fmanage = _ds_objects_b02ofly_manage;
		return 1;      
   }   
   switch (obj) {
      case 1:
         // Blue Flyer
         object->fcreate = _ds_objects_b02o01_create;
         object->fmanage = _ds_objects_b02o01_manage;
         return 1;
         break; // not really necessary...            
      case 2:
         // Evil Flyer
         object->fcreate = _ds_objects_b02o02_create;
         object->fmanage = _ds_objects_b02o02_manage;
         return 1;
         break; // not really necessary...            
      case 5:
         // Green Flyer
         object->fcreate = _ds_objects_b02o05_create;
         object->fmanage = _ds_objects_b02o05_manage;
         return 1;
         break; // not really necessary...            
      case 6:
         // Grey Flyer
         object->fcreate = _ds_objects_b02o06_create;
         object->fmanage = _ds_objects_b02o06_manage;
         return 1;
         break; // not really necessary...            
      case 7:
         // Brown Flyer
         object->fcreate = _ds_objects_b02o07_create;
         object->fmanage = _ds_objects_b02o07_manage;
         return 1;
         break; // not really necessary...            
      case 8:
         // Cave Particle I
         object->fcreate = _ds_objects_b02opart_create;
         object->fmanage = _ds_objects_b02opart_manage;
         return 1;
         break; // not really necessary...                     
      case 9:
         // Yellow Wasp
         object->fcreate = _ds_objects_b02o09_create;
         object->fmanage = _ds_objects_b02o09_manage;
         return 1;
         break; // not really necessary...                              
      case 10:
         // Red Wasp
         object->fcreate = _ds_objects_b02o10_create;
         object->fmanage = _ds_objects_b02o10_manage;
         return 1;
         break; // not really necessary...                                       
      case 11:
         // Homing Circle
         object->fcreate = _ds_objects_b02o11_create;
         object->fmanage = _ds_objects_b02o11_manage;
         return 1;
         break; // not really necessary...                                       
      case 12:
         // Homing Rectangle
         object->fcreate = _ds_objects_b02o12_create;
         object->fmanage = _ds_objects_b02o12_manage;
         return 1;
         break; // not really necessary...                                                
      case 13:
         // Cave Particle II
         object->fcreate = _ds_objects_b02opart_create;
         object->fmanage = _ds_objects_b02opart_manage;
         return 1;
         break; // not really necessary...                     
      case 14:
         // Tentacle Flyer
         object->fcreate = _ds_objects_b02o14_create;
         object->fmanage = _ds_objects_b02o14_manage;
         return 1;
         break; // not really necessary...            
      case 15:
         // Propeller Dude
         object->fcreate = _ds_objects_b02o15_create;
         object->fmanage = _ds_objects_b02o15_manage;
         return 1;
         break; // not really necessary...                     
      case 16:
         // Propeller Child
         object->fcreate = _ds_objects_b02o16_create;
         object->fmanage = _ds_objects_b02o16_manage;
         return 1;
         break; // not really necessary...                                       
      case 17:
         // Orange Propeller Child
         object->fcreate = _ds_objects_b02o17_create;
         object->fmanage = _ds_objects_b02o17_manage;
         return 1;
         break; // not really necessary...                              
      case 18:
         object->fcreate = _ds_objects_b02o18_create;
         object->fmanage = _ds_objects_b02o18_manage;
         return 1;
         break; // not really necessary...                     
      case 19:
         object->fcreate = _ds_objects_b02o19_create;
         object->fmanage = _ds_objects_b02o19_manage;
         return 1;
         break; // not really necessary...                              
      case 20:
         // Gap Jumper A
         object->fcreate = _ds_objects_b02o20_create;
         object->fmanage = _ds_objects_b02o20_manage;
         return 1;
         break; // not really necessary...                     
      case 21:
         // Gap Jumper B
         object->fcreate = _ds_objects_b02o21_create;
         object->fmanage = _ds_objects_b02o21_manage;
         return 1;
         break; // not really necessary...                                       
      case 22:
         // Quad Flyer
         object->fcreate = _ds_objects_b02o22_create;
         object->fmanage = _ds_objects_b02o22_manage;
         return 1;
         break; // not really necessary...                     
      case 23:
         // Cave (Light) Particle III
         object->fcreate = _ds_objects_b02opart_create;
         object->fmanage = _ds_objects_b02opart_manage;
         return 1;
         break; // not really necessary...                     
      case 25:
         // Fly Up
         object->fcreate = _ds_objects_b02oflyArc_create;
         object->fmanage = _ds_objects_b02o25_manage;
         return 1;
         break; // not really necessary...                                       
      case 26:
         // Fly Arc L
         object->fcreate = _ds_objects_b02oflyArc_create;
         object->fmanage = _ds_objects_b02o26_manage;
         return 1;
         break; // not really necessary...                                       
      case 27:
         // Fly Arc R
         object->fcreate = _ds_objects_b02oflyArc_create;
         object->fmanage = _ds_objects_b02o27_manage;
         return 1;
         break; // not really necessary...                                       
      case 28:
         // Walk birdie
			object->fcreate = _ds_objects_b02o28_create;
         object->fmanage = _ds_objects_b02o28_manage;
         return 1;
         break; // not really necessary...                                       
      case 29:
         // Flier 
         object->fcreate = _ds_objects_b02oflyArc_create;
         object->fmanage = _ds_objects_b02o29_manage;
         return 1;
         break; // not really necessary...                                       
      case 30:
         // Blue Bird
         object->fcreate = _ds_objects_b02o30_create;
         object->fmanage = _ds_objects_b02o30_manage;
         return 1;
         break; // not really necessary...                                       
      case 31:
         // Far Away Birds
         object->fcreate = _ds_objects_b02o31_create;
         object->fmanage = _ds_objects_b02o31_manage;
         return 1;
         break; // not really necessary...                                       
      case 32:
         // Homing Hunter
         object->fcreate = _ds_objects_b02o32_create;
         object->fmanage = _ds_objects_b02o32_manage;
         return 1;
         break; // not really necessary...                                       
   }   
   return 0;
}   
