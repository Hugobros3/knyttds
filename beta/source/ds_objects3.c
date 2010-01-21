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

// BANK 3 [B03]
//=================================================================================================

int _ds_objects3_lookAtJuni(ds_t_object *object) {
/*
	[0] - State
*/
	switch (object->inner[0]) {
	   case 0:
	      /* LOOK RIGHT... */
	      if (object->x > ds_global_juni.x) {
	         ds_3dsprite_setFrame(object->sprite,1); // Left
	         object->inner[0] = 1; // Look left state...
	      }   
	      break;
	   case 1:
	      /* LOOK LEFT... */
	      if (object->x < ds_global_juni.x) {
	         ds_3dsprite_setFrame(object->sprite,0); // Right
	         object->inner[0] = 0; // Look right state...
	      }   
	      break;	      
	}      
	return 1;
}   

// FRIENDLY [B03_O01]
//..........................................................................................
int _ds_objects_b03o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o01_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,3,0,70,24,128,1*60,2*60,2,1,0,-1,1,1);   
   return 1;
} 

// LAUNCH MUFF [B03_O02]
//..........................................................................................
int _ds_objects_b03o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o02_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object, 1, // X
									 120, 120, // Every 2 seconds? 
									 0, 2, 4, // R(2)*4
									 6, 12, 20, 28, // Sprites
									 -1, 20, // Do not change dir
									 1,  // Cycle
									 0, // No follow Juni
									 3, // Movement
									 0,
									 1); // Launch themselves!

   return 1;
}           

// BLUE MUFF [B03_O03]
//..........................................................................................
int _ds_objects_b03o03_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o03_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,1,1,0,100,24,72,0*60,3*60,4,1,0,-1,1,1);   
   return 1;
}

// RED MUFF [B03_O04]
//..........................................................................................
int _ds_objects_b03o04_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o04_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR_Spike(object, 
									 60, 30, // A second...
									 0, 2, 6, // 0+R(2)*6
									 6, 12, 16, 20, // Sprites
									 60, 33, // Dir (chance)
									 0,  // Not cycle
									 60,  // Distance
									 0,  // DO NOT follow Juni
									 3); // Movement 

   return 1;
}                     

// YELLOW MUFF [B03_O05]
//..........................................................................................
int _ds_objects_b03o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o05_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,1,1,0,100,48,64,2*60,3*60,1,2,1,-1,1,1);   
   return 1;
}

// ORANGE MUFF [B03_O06]
//..........................................................................................
int _ds_objects_b03o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o06_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR_Spike(object, 
									 60, 30, // A second...
									 0, 2, 6, // 0+R(2)*6
									 6, 12, 16, 20, // Sprites
									 60, 100, // Dir (chance)
									 0,  // Not cycle
									 60,  // Distance
									 1,  // Follow Juni
									 3); // Movement

   return 1;
}                        

// FLAT MUFF [B03_O07]
//..........................................................................................
int _ds_objects_b03o07_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o07_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,1,1,0,100,48,64,2*60,3*60,1,2,0,-1,1,1);   
   return 1;
}

// LAUNCH MUFF 2 [B03_O08]
//..........................................................................................
int _ds_objects_b03o08_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o08_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object, 1, // X
									 120, 120, // Every 2 seconds? 
									 0, 3, 3, // R(3)*3
									 5, 10, 18, 26, // Sprites
									 -1, 20, // Do not change dir
									 1,  // Cycle
									 0, // No follow Juni
									 3, // Movement
									 0,
									 1); // Launch themselves!

   return 1;
}              

// CUTE RABBIT [B03_O09]
//..........................................................................................
int _ds_objects_b03o09_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o09_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,2,0,100,24,72,0*60,1*60,2,1,0,-1,1,1);   
   return 1;
}   

// RABBIT [B03_O10]
//..........................................................................................
int _ds_objects_b03o10_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
				
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o10_manage(void *objp) {
   ds_t_object *object = objp;
   	
	ds_objects_lib_beh_diskMovementLR(object, 1, 
									 120, 120,
									 0, 2, 5, // 0-5-10
									 0, 0, 0, 0,
									 120, 20, // Chances to change direction
									 0,
									 0,
									 3,
									 0,
									 0);
	
   return 1;
}   

// PINK RABBIT [B03_O11]
//..........................................................................................
int _ds_objects_b03o11_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o11_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object, 1, 
									 120, 120,
									 0, 2, 10, // 0-10-20
									 0, 0, 0, 0,
									 120, 33, // Chances to change direction
									 0,
									 0,
									 3,
									 0,
									 0);
	
   return 1;
}   

// ESKIMO [B03_O12]
//..........................................................................................
int _ds_objects_b03o12_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o12_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b03 = 4;
	int ds_b03A[] = {1,3,12,21};

	ds_objects_lib_beh_knyttNgestures(object,ds_b03,ds_b03A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}


// ESKIMO 2 [B03_O13]
//..........................................................................................
int _ds_objects_b03o13_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b03o13_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b03 = 4;
	int ds_b03A[] = {1,3,11,17};

	ds_objects_lib_beh_knyttNgestures(object,ds_b03,ds_b03A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}   

// FOREST GIRL [B03_O14]
//..........................................................................................
int _ds_objects_b03o14_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o14_manage(void *objp) {
   ds_t_object *object = objp;

	_ds_objects3_lookAtJuni(object);
   return 1;
}   

// FOREST CHILD [B03_O15]
//..........................................................................................
int _ds_objects_b03o15_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o15_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,2,0,100,24,72,0*60,1*60,3,1,0,-1,1,1);   
   return 1;
}   

// HAMSTER [B03_O16]
//..........................................................................................
int _ds_objects_b03o16_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o16_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,4,0,100,24,72,0*60,1*60,2,1,0,-1,1,1);   
   return 1;
}   

// HAMSTER 2 [B03_O17]
//..........................................................................................
int _ds_objects_b03o17_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o17_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,4,0,100,24,72,0*60,1*60,2,1,0,-1,1,1);   
   return 1;
}   

// HAMSTER 3 [B03_O18]
//..........................................................................................
int _ds_objects_b03o18_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o18_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,4,0,100,24,72,0*60,1*60,2,1,0,-1,1,1);   
   return 1;
}

// PURPLE LIZARD [B03_O19]
//..........................................................................................
int _ds_objects_b03o19_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o19_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object, 1, // X
									 60, 60, // Every second? 
									 0, 3, 3, // R(3)*3
									 5, 10, 13, 16, // Sprites
									 120, 25, // Dir (chance)
									 0,  // Not cycle
									 0, // No follow juni
									 3, // Movement
									 0,
									 0);

   return 1;
}

// TALL WOMAN [B03_O20]
//..........................................................................................
int _ds_objects_b03o20_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o20_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object,1, // X 
									 120, 60, // Every second? 
									 0, 1, 9, // R(1)*9
									 16, 32, 47, 62, // Sprites
									 120, 20, // Dir (chance)
									 1,  // Cycle
									 0, // No follow Juni
									 3, // Movement
									 0,
									 0);

   return 1;
}               

// ELDER FROM "COLORS"!! (BEARD GUY) [B03_O21]
//..........................................................................................
int _ds_objects_b03o21_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b03o21_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b03 = 4;
	int ds_b03A[] = {1,3,10,25};

	ds_objects_lib_beh_knyttNgestures(object,ds_b03,ds_b03A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}      

// RED (HARMFUL) SPIKE [B03_O22]
//..........................................................................................
int _ds_objects_b03o22_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o22_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementB_Spike(object, 
									 60, 10, 
									 0, 1, 9, // 0 + R(1)*9
									 6, 11, // Sprites
									 120, 100, // Dir (chance - ALWAYS)
									 0,  // Not cycle
									 60,  // Distance									 
									 1,  // Follow Juni
									 3); // Movement

   return 1;
}                        

// RED (NON-HARMFUL) VIOLET [B03_O23]
//..........................................................................................
int _ds_objects_b03o23_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o23_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementB(object, 
									 60, 10, 
									 0, 1, 9, // 0 + R(1)*9
									 6, 0, // Sprites
									 120, 100, // Dir (chance - ALWAYS)
									 0,  // Not cycle
									 1,  // Follow Juni
									 3); // Movement

   return 1;
}            

// SPIKER VIOLET [B03_O24]
//..........................................................................................
int _ds_objects_b03o24_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o24_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementB(object, 
									 60, 60,
									 0, 1, 9, // R(1)*9
									 6, 9, // Sprites
									 90, 50, // Dir (chance)
									 0,  // Not cycle
									 0,  // Don't follow Juni
									 3); // Movement

   return 1;
}            

// RAT-LIKE [B03_O25]
//..........................................................................................
int _ds_objects_b03o25_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o25_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,1,1,0,100,48,64,2*60,3*60,1,2,1,-1,1,1);   
   return 1;
}   

// FOREST GUY [B03_O26]
//..........................................................................................
int _ds_objects_b03o26_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o26_manage(void *objp) {
   ds_t_object *object = objp;

	_ds_objects3_lookAtJuni(object);
   return 1;
}   

// ANOTHER GUY [B03_O27]
//..........................................................................................
int _ds_objects_b03o27_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b03o27_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b03 = 4;
	int ds_b03A[] = {1,3,12,23};

	ds_objects_lib_beh_knyttNgestures(object,ds_b03,ds_b03A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}      

// FOREST BOY [B03_O28]
//..........................................................................................
int _ds_objects_b03o28_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o28_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,2,0,100,24,72,0*60,1*60,3,1,0,-1,1,1);   
   return 1;
}

// WINGED GIRL 1 [B03_O29]
//..........................................................................................
int _ds_objects_b03o29_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o29_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b03 = 4;
	int ds_b03A[] = {1,3,9,15};

	ds_objects_lib_beh_knyttNgestures(object,ds_b03,ds_b03A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}   

// FLY ADULT [B03_O30]
//..........................................................................................
int _ds_objects_b03o30_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o30_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object,1, // X 
									 30, 30, // Less than a second...
									 6, 2, 7, // 6+R(2)*7
									 6, 12, 0, 0, // Sprites
									 120, 33, // Dir (chance)
									 0,  // Not cycle
									 0, // No follow Juni
									 3, // Movement
									 0,
									 0);

   return 1;
}            

// FLY BOY [B03_O31]
//..........................................................................................
int _ds_objects_b03o31_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o31_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,2,0,100,24,72,0*60,1*60,3,1,0,-1,1,1);   
   return 1;
}

// WINGED GIRL 2 [B03_O32]
//..........................................................................................
int _ds_objects_b03o32_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_b03o32_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b03 = 4;
	int ds_b03A[] = {1,2,7,13};

	ds_objects_lib_beh_knyttNgestures(object,ds_b03,ds_b03A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}      

// GREEN MUFF [B03_O33]
//..........................................................................................
int _ds_objects_b03o33_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o33_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR_Spike(object, 
									 60, 30, // A second...
									 0, 2, 9, // 0+R(2)*9
									 6, 12, 16, 20, // Sprites
									 120, 33, // Dir (chance)
									 0,  // Not cycle
									 60,  // Distance
									 0,  // DO NOT follow Juni
									 3); // Movement

   return 1;
}                  

// CAMEL [B03_O34]
//..........................................................................................
int _ds_objects_b03o34_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o34_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,3,0,100,24,72,0*60,3*60,3,1,0,-1,1,1);   
   return 1;
}   

// CAMEL RIDER [B03_O35]
//..........................................................................................
int _ds_objects_b03o35_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o35_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,2,0,100,24,72,0*60,3*60,4,1,0,-1,1,1);   
   return 1;
}

// GREEN LIZARD [B03_O36]
//..........................................................................................
int _ds_objects_b03o36_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o36_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object,1, // X 
									 60, 60, // Every second? 
									 0, 3, 3, // R(3)*3
									 5, 10, 13, 16, // Sprites
									 120, 33, // Dir (chance)
									 0,  // Not cycle
									 0, // No follow Juni
									 3, // Movement
									 0,
									 0);

   return 1;
}      

// BABY KNYTT [B03_O37]
//..........................................................................................
int _ds_objects_b03o37_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o37_manage(void *objp) {
   return 1;
}      

// CAVE GIRL [B03_O38]
//..........................................................................................
int _ds_objects_b03o38_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o38_manage(void *objp) {
   ds_t_object *object = objp;

	_ds_objects3_lookAtJuni(object);
   return 1;
}   

// CAVE DUDE [B03_O39]
//..........................................................................................
int _ds_objects_b03o39_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o39_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b03 = 4;
	int ds_b03A[] = {1,3,12,23};

	ds_objects_lib_beh_knyttNgestures(object,ds_b03,ds_b03A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}   

// CAVE BOY [B03_O40]
//..........................................................................................
int _ds_objects_b03o40_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_objects_lib_beh_moveLeftRight_ini(object,1);

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o40_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_moveLeftRight(object,2,2,0,100,24,72,0*60,1*60,3,1,0,-1,1,1);   
   return 1;
}   

// KNYTT GIRL [B03_O41]
//..........................................................................................
int _ds_objects_b03o41_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o41_manage(void *objp) {
   ds_t_object *object = objp;

	_ds_objects3_lookAtJuni(object);
   return 1;
}   

// CAT [B03_O42]
//..........................................................................................
int _ds_objects_b03o42_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o42_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b03 = 4;
	int ds_b03A[] = {1,3,10,14};

	ds_objects_lib_beh_knyttNgestures(object,ds_b03,ds_b03A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}   

// SMALL CAT [B03_O43]
//..........................................................................................
int _ds_objects_b03o43_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o43_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b03 = 4;
	int ds_b03A[] = {1,3,10,14};

	ds_objects_lib_beh_knyttNgestures(object,ds_b03,ds_b03A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}   

// GREY LIZARD [B03_O44]
//..........................................................................................
int _ds_objects_b03o44_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b03o44_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_diskMovementLR(object, 1, // X
									 60, 60, // Every second? 
									 0, 3, 3, // R(3)*3
									 5, 10, 13, 16, // Sprites
									 120, 20, // Dir (chance)
									 0,  // Not cycle
									 0, // No follow Juni
									 3, // Movement
									 0,
									 0);

   return 1;
}         

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects3_assign(u8 obj, ds_t_object *object) {
   switch (obj) {
      case 1:
         // Friendly
         object->fcreate = _ds_objects_b03o01_create;
         object->fmanage = _ds_objects_b03o01_manage;
         return 1;
         break; // not really necessary...            
      case 2:
         // Launch Muff
         object->fcreate = _ds_objects_b03o02_create;
         object->fmanage = _ds_objects_b03o02_manage;
         return 1;
         break; // not really necessary...      
      case 3:
         // Blue Muff
         object->fcreate = _ds_objects_b03o03_create;
         object->fmanage = _ds_objects_b03o03_manage;
         return 1;
         break; // not really necessary...      
      case 4:
         // Red Muff
         object->fcreate = _ds_objects_b03o04_create;
         object->fmanage = _ds_objects_b03o04_manage;
         return 1;
         break; // not really necessary...      
      case 5:
         // Yellow Muff
         object->fcreate = _ds_objects_b03o05_create;
         object->fmanage = _ds_objects_b03o05_manage;
         return 1;
         break; // not really necessary...               
      case 6:
         // Orange Muff
         object->fcreate = _ds_objects_b03o06_create;
         object->fmanage = _ds_objects_b03o06_manage;
         return 1;
         break; // not really necessary...      
      case 7:
         // Flat Muff
         object->fcreate = _ds_objects_b03o07_create;
         object->fmanage = _ds_objects_b03o07_manage;
         return 1;
         break; // not really necessary...               
      case 8:
         // Launch Muff 2
         object->fcreate = _ds_objects_b03o08_create;
         object->fmanage = _ds_objects_b03o08_manage;
         return 1;
         break; // not really necessary...      
      case 9:
         // Cute Rabbit
         object->fcreate = _ds_objects_b03o09_create;
         object->fmanage = _ds_objects_b03o09_manage;
         return 1;
         break; // not really necessary...               
      case 10:
         // Rabbit
         object->fcreate = _ds_objects_b03o10_create;
         object->fmanage = _ds_objects_b03o10_manage;
         return 1;
         break; // not really necessary...
      case 11:
         // Pink Rabbit
         object->fcreate = _ds_objects_b03o11_create;
         object->fmanage = _ds_objects_b03o11_manage;
         return 1;
         break; // not really necessary...                        
      case 12:
         // Eskimo
         object->fcreate = _ds_objects_b03o12_create;
         object->fmanage = _ds_objects_b03o12_manage;
         return 1;
         break; // not really necessary...
      case 13:
         // Eskimo 2
         object->fcreate = _ds_objects_b03o13_create;
         object->fmanage = _ds_objects_b03o13_manage;
         return 1;
         break; // not really necessary...         
      case 14:
         // Forest Girl
         object->fcreate = _ds_objects_b03o14_create;
         object->fmanage = _ds_objects_b03o14_manage;
         return 1;
         break; // not really necessary...                                    
      case 15:
         // Forest Child
         object->fcreate = _ds_objects_b03o15_create;
         object->fmanage = _ds_objects_b03o15_manage;
         return 1;
         break; // not really necessary...                                                               
      case 16:
         // Hamster
         object->fcreate = _ds_objects_b03o16_create;
         object->fmanage = _ds_objects_b03o16_manage;
         return 1;
         break; // not really necessary...                                                                                 
      case 17:
         // Hamster 2
         object->fcreate = _ds_objects_b03o17_create;
         object->fmanage = _ds_objects_b03o17_manage;
         return 1;
         break; // not really necessary...                                                                        
      case 18:
         // Hamster 3
         object->fcreate = _ds_objects_b03o18_create;
         object->fmanage = _ds_objects_b03o18_manage;
         return 1;
         break; // not really necessary...                                                                                 
      case 19:
         // Purple lizard
         object->fcreate = _ds_objects_b03o19_create;
         object->fmanage = _ds_objects_b03o19_manage;
         return 1;
         break; // not really necessary...                                                                                 
      case 20:
         // Tall woman
         object->fcreate = _ds_objects_b03o20_create;
         object->fmanage = _ds_objects_b03o20_manage;
         return 1;
         break; // not really necessary...                                                                                 
      case 21:
         // THE ELDER FROM "COLORS"!!! :-D (OK, official name is "Beard Guy")
         object->fcreate = _ds_objects_b03o21_create;
         object->fmanage = _ds_objects_b03o21_manage;
         return 1;
         break; // not really necessary...                           
      case 22:
         // Red (harmful) Spiker 
         object->fcreate = _ds_objects_b03o22_create;
         object->fmanage = _ds_objects_b03o22_manage;
         return 1;
         break; // not really necessary...                                                               
      case 23:
         // Red (non-harmful) Spiker 
         object->fcreate = _ds_objects_b03o23_create;
         object->fmanage = _ds_objects_b03o23_manage;
         return 1;
         break; // not really necessary...                                                               
      case 24:
         // Spiker Violet
         object->fcreate = _ds_objects_b03o24_create;
         object->fmanage = _ds_objects_b03o24_manage;
         return 1;
         break; // not really necessary...                                                               
      case 25:
         // Rat-Like
         object->fcreate = _ds_objects_b03o25_create;
         object->fmanage = _ds_objects_b03o25_manage;
         return 1;
         break; // not really necessary...                                                               
      case 26:
         // Forest Guy
         object->fcreate = _ds_objects_b03o26_create;
         object->fmanage = _ds_objects_b03o26_manage;
         return 1;
         break; // not really necessary...                           
      case 27:
         // Another Guy
         object->fcreate = _ds_objects_b03o27_create;
         object->fmanage = _ds_objects_b03o27_manage;
         return 1;
         break; // not really necessary...                  
      case 28:
         // Forest Boy
         object->fcreate = _ds_objects_b03o28_create;
         object->fmanage = _ds_objects_b03o28_manage;
         return 1;
         break; // not really necessary...                                                      
      case 29:
         // Winged Girl 1
         object->fcreate = _ds_objects_b03o29_create;
         object->fmanage = _ds_objects_b03o29_manage;
         return 1;
         break; // not really necessary...                                                      
      case 30:
         // Fly Adult
         object->fcreate = _ds_objects_b03o30_create;
         object->fmanage = _ds_objects_b03o30_manage;
         return 1;
         break; // not really necessary...                                             
      case 31:
         // Fly Boy
         object->fcreate = _ds_objects_b03o31_create;
         object->fmanage = _ds_objects_b03o31_manage;
         return 1;
         break; // not really necessary...                                             
      case 32:
         // Winged Girl 2
         object->fcreate = _ds_objects_b03o32_create;
         object->fmanage = _ds_objects_b03o32_manage;
         return 1;
         break; // not really necessary...                                                      
      case 33:
         // Green Muff
         object->fcreate = _ds_objects_b03o33_create;
         object->fmanage = _ds_objects_b03o33_manage;
         return 1;
         break; // not really necessary...                                                      
      case 34:
         // Camel
         object->fcreate = _ds_objects_b03o34_create;
         object->fmanage = _ds_objects_b03o34_manage;
         return 1;
         break; // not really necessary...                                                      
      case 35:
         // Camel Rider
         object->fcreate = _ds_objects_b03o35_create;
         object->fmanage = _ds_objects_b03o35_manage;
         return 1;
         break; // not really necessary...                                             
      case 36:
         // Green Lizard
         object->fcreate = _ds_objects_b03o36_create;
         object->fmanage = _ds_objects_b03o36_manage;
         return 1;
         break; // not really necessary...                                    
      case 37:
         // Baby Knytt
         object->fcreate = _ds_objects_b03o37_create;
         object->fmanage = _ds_objects_b03o37_manage;
         return 1;
         break; // not really necessary...                                    
      case 38:
         // Cave Girl
         object->fcreate = _ds_objects_b03o38_create;
         object->fmanage = _ds_objects_b03o38_manage;
         return 1;
         break; // not really necessary...                                    
      case 39:
         // Cave Dude
         object->fcreate = _ds_objects_b03o39_create;
         object->fmanage = _ds_objects_b03o39_manage;
         return 1;
         break; // not really necessary...                           
      case 40:
         // Cave Boy
         object->fcreate = _ds_objects_b03o40_create;
         object->fmanage = _ds_objects_b03o40_manage;
         return 1;
         break; // not really necessary...                                             
      case 41:
         // Knytt Girl
         object->fcreate = _ds_objects_b03o41_create;
         object->fmanage = _ds_objects_b03o41_manage;
         return 1;
         break; // not really necessary...                                    
      case 42:
         // Cat
         object->fcreate = _ds_objects_b03o42_create;
         object->fmanage = _ds_objects_b03o42_manage;
         return 1;
         break; // not really necessary...                           
      case 43:
         // Small Cat
         object->fcreate = _ds_objects_b03o43_create;
         object->fmanage = _ds_objects_b03o43_manage;
         return 1;
         break; // not really necessary...                                    
      case 44:
         // Grey Lizard
         object->fcreate = _ds_objects_b03o44_create;
         object->fmanage = _ds_objects_b03o44_manage;
         return 1;
         break; // not really necessary...                                    
   }   
   return 0;
}   
