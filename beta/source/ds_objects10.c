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
#include "ds_objects.h"

// BANK 10 [B10]
//=================================================================================================

// BLUE BOUNCER [B10_O01]
//..........................................................................................
int _ds_objects_b10o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);	
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
			
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b10o01_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_jumpSequence(object,0 - (1 << 10) - 250,0 - (2 << 10) - 100, 0 - (3 << 10), 0 - (4 << 10) + 200, 0 - (4 << 10) - 500, 5 << 10, 120, 0, 1);   
   return 1;
}

// GREY BOUNCER [B10_O02]
//..........................................................................................
int _ds_objects_b10o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);	
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b10o02_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_jumpSequence(object,0 - (1 << 10) - 250,0 - (2 << 10) - 100, 0 - (3 << 10), 0 - (4 << 10) + 200, 0 - (4 << 10) - 500, 5 << 10, 120, 1, 1);   
   return 1;
}

// RED BOUNCER [B10_O03]
//..........................................................................................
int _ds_objects_b10o03_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->inner[2] = 1; // Starts at specific jump substate <TODO> Function
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);	
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b10o03_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   int speed = 0;

	// Jump
	int old = object->inner[0];
	int jump = object->inner[2];
	ds_objects_lib_beh_jumpSequence(object,0 - (1 << 10) - 250,0 - (2 << 10) - 100, 0 - (3 << 10), 0 - (4 << 10) + 200, 0 - (4 << 10) - 500, 5 << 10, 120, 0, 1);   
	int new = object->inner[0];
	
	// Particle
	switch (jump) {
	   case 0:
	      speed = 30;
	      break;
	   case 1:
	      speed = 45;
	      break;
	   case 2:
	      speed = 60;
	      break;
	   case 3:
	      speed = 75;
	      break;
	   case 4:
	      speed = 90;
	      break;
	}   
	if ((old == 0) && (new == 1)) {
	   // L
		particle = ds_objects_createParticle(object->x + 6, 
							object->y - 18, 
							object->layer, 
							54);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					-14, 
					-speed, 
			 		10, 0, 3);				 		
	   // R
		particle = ds_objects_createParticle(object->x + 8, 
							object->y - 18, 
							object->layer, 
							54);
	   ds_objects_lib_beh_particleMMF2_Init(particle, 
					14, 
					-speed, 
			 		10, 0, 3);				 		
		ds_objects_setBlink(object, DS_C_GAMESTATUS_BLINK);
	} 		
	
   return 1;
}

// SMART BOUNCER [B10_O04]
//..........................................................................................
int _ds_objects_b10o04_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	ds_objects_lib_beh_jumpJuniJump_ini(object);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);	
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b10o04_manage(void *objp) {
   ds_t_object *object = objp;

	if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_DBLJUMP))
		ds_objects_lib_beh_jumpJuniJump(object,96,2,42,1,0);   
	else 
		ds_objects_lib_beh_jumpJuniJump(object,96,2,16,1,0);
   return 1;
}

// SMART BOUNCER MINI [B10_O05]
//..........................................................................................
int _ds_objects_b10o05_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	ds_objects_lib_beh_jumpJuniJump_ini(object);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);	
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b10o05_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_jumpJuniJump(object,96,2,8,1,0);   
   return 1;
}

// SMART WALKING BOUNCER [B10_O06]
//..........................................................................................
int _ds_objects_b10o06_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	ds_objects_lib_beh_jumpJuniJump_ini(object);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);	
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b10o06_manage(void *objp) {
   ds_t_object *object = objp;

	switch (object->inner[11]) {
	   case 0:
			ds_objects_lib_beh_diskMovementLR(object, 1, 
											 67, 67, // 1.12 Speed
											 4 * PA_RandMinMax(0,1), 1, 0, // Speed 0-4
											 6, 12, 13, 14,
											 67, 100,
											 0,
											 0,
											 3,
											 0,
											 0);
		   if (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_TJUMP)) {
		      if (ds_objects_lib_distanceJuniX(object,1,1) <= 96) {
		         object->inner[11] = 1;
  	            object->inner[0] = 2;
		   	}   
			}			
			break;
		case 1:
			if (ds_util_bitOne16(ds_global_juni.item,DS_C_JUNI_IT_DBLJUMP))
				ds_objects_lib_beh_jumpJuniJump(object,96,2,42,1,0);   
			else 
				ds_objects_lib_beh_jumpJuniJump(object,96,2,16,1,0);
			if (object->inner[0] == 0) {
	         object->inner[11] = 0;
            object->inner[0] = 0;
			}   
		   break;
	}   

	
   return 1;
}

// SMART WALKING BOUNCER MINI [B10_O07]
//..........................................................................................
int _ds_objects_b10o07_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	ds_objects_lib_beh_jumpJuniJump_ini(object);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);	
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b10o07_manage(void *objp) {
   ds_t_object *object = objp;

	switch (object->inner[11]) {
	   case 0:
			ds_objects_lib_beh_diskMovementLR(object, 1, 
											 67, 67, // 1.12 Speed
											 4 * PA_RandMinMax(0,1), 1, 0, // Speed 0-4
											 6, 12, 13, 14,
											 67, 100,
											 0,
											 0,
											 3,
											 0,
											 0);
			int rndval = 0;
			if ((ds_global_tick % 15) == 0)
				rndval = PA_RandMinMax(0,7);
	      if (rndval == 7) {
	         object->inner[11] = 1;
            object->inner[0] = 2;
	   	}   
			break;
		case 1:
			ds_objects_lib_beh_jumpJuniJump(object,96,2,24,1,0);
			if (object->inner[0] == 0) {
	         object->inner[11] = 0;
            object->inner[0] = 0;
			}   
		   break;
	}   
	
   return 1;
}

// GREEN BOUNCER [B10_O08]
//..........................................................................................
int _ds_objects_b10o08_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;

	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_WARN);	
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_RADAR);
			
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_OBJECT;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_b10o08_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_jumpSequence(object,0 - (1 << 10) - 500,0 - (2 << 10) - 350, 0 - (3 << 10) - 250, 0 - (4 << 10) - 50, 0, 5 << 10, 120, 0, 1);   
   return 1;
}

// BIG FLUFF [B10_O09]
//..........................................................................................
int _ds_objects_b10o09_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b10o09_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_cycle(object,2);
   return 1;
}

// BLUE FLUFF [B10_O10]
//..........................................................................................
int _ds_objects_b10o10_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b10o10_manage(void *objp) {
   ds_t_object *object = objp;

	ds_objects_lib_beh_cycle(object,2);
   return 1;
}

// OLD FLUFF [B10_O11]
//..........................................................................................
int _ds_objects_b10o11_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b10o11_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b10 = 4;
	int ds_b10A[] = {1,3,7,11};

	ds_objects_lib_beh_knyttNgestures(object,ds_b10,ds_b10A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}   

// REGULAR FLUFF [B10_O12]
//..........................................................................................
int _ds_objects_b10o12_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b10o12_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b10 = 4;
	int ds_b10A[] = {1,3,15,19};

	ds_objects_lib_beh_knyttNgestures(object,ds_b10,ds_b10A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}   

// GREEN FLUFF [B10_O13]
//..........................................................................................
int _ds_objects_b10o13_create(u8 bank, u8 obj, void *objp) {
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

int _ds_objects_b10o13_manage(void *objp) {
   ds_t_object *object = objp;
	int ds_b10 = 4;
	int ds_b10A[] = {1,3,12,29};

	ds_objects_lib_beh_knyttNgestures(object,ds_b10,ds_b10A,(60*3) + PA_RandMinMax(0,60*2),5,0);
   return 1;
}   

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_objects10_assign(u8 obj, ds_t_object *object) {
   switch (obj) {
      case 1:
         // Blue Bouncer
         object->fcreate = _ds_objects_b10o01_create;
         object->fmanage = _ds_objects_b10o01_manage;
         return 1;
         break; // not really necessary...
      case 2:
         // Grey Bouncer
         object->fcreate = _ds_objects_b10o02_create;
         object->fmanage = _ds_objects_b10o02_manage;
         return 1;
         break; // not really necessary...
      case 3:
         // Red Bouncer
         object->fcreate = _ds_objects_b10o03_create;
         object->fmanage = _ds_objects_b10o03_manage;
         return 1;
         break; // not really necessary...      
      case 4:
         // Smart Bouncer
         object->fcreate = _ds_objects_b10o04_create;
         object->fmanage = _ds_objects_b10o04_manage;
         return 1;
         break; // not really necessary...
      case 5:
         // Smart Bouncer Mini
         object->fcreate = _ds_objects_b10o05_create;
         object->fmanage = _ds_objects_b10o05_manage;
         return 1;
         break; // not really necessary...
      case 6:
         // Smart Walking Bouncer 
         object->fcreate = _ds_objects_b10o06_create;
         object->fmanage = _ds_objects_b10o06_manage;
         return 1;
         break; // not really necessary...
      case 7:
         // Smart Walking Bouncer Mini
         object->fcreate = _ds_objects_b10o07_create;
         object->fmanage = _ds_objects_b10o07_manage;
         return 1;
         break; // not really necessary...
      case 8:
         // Green Bouncer
         object->fcreate = _ds_objects_b10o08_create;
         object->fmanage = _ds_objects_b10o08_manage;
         return 1;
         break; // not really necessary...         
      case 9:
         // Big Fluff
         object->fcreate = _ds_objects_b10o09_create;
         object->fmanage = _ds_objects_b10o09_manage;
         return 1;
         break; // not really necessary...         
      case 10:
         // Blue Fluff
         object->fcreate = _ds_objects_b10o10_create;
         object->fmanage = _ds_objects_b10o10_manage;
         return 1;
         break; // not really necessary...                  
      case 11:
         // Old Fluff
         object->fcreate = _ds_objects_b10o11_create;
         object->fmanage = _ds_objects_b10o11_manage;
         return 1;
         break; // not really necessary...                           
      case 12:
         // Regular Fluff
         object->fcreate = _ds_objects_b10o12_create;
         object->fmanage = _ds_objects_b10o12_manage;
         return 1;
         break; // not really necessary...                           
      case 13:
         // Green Fluff
         object->fcreate = _ds_objects_b10o13_create;
         object->fmanage = _ds_objects_b10o13_manage;
         return 1;
         break; // not really necessary...                                    
	}			      
   return 0;
}   
