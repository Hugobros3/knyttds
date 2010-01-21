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
#include "ds_objects.h"
#include "ds_objects_lib.h"
#include "ds_util.h"
#include "ds_world.h"
#include "ds_map.h"
#include "ds_3dsprite.h"
#include <math.h>

// PARTICLE "CHILDREN" BANK  [PART]
//=================================================================================================

void _ds_objects_p_manageMov(ds_t_object *object, int speedX, int speedY, int coll) {
}

void _ds_objects_p_manageSmokeInit(ds_t_object *object, int minspeedX, int maxspeedX, int minspeedY, int maxspeedY, int gravityX, int gravityY) {
   if (PA_RandMinMax(1,100) < 50)
   	object->inner[0] = (PA_RandMinMax(minspeedX,maxspeedX));
	else
		object->inner[0] = (0 - PA_RandMinMax(minspeedX,maxspeedX));
   if (PA_RandMinMax(1,100) < 50)
   	object->inner[1] = (PA_RandMinMax(minspeedY,maxspeedY));
	else
		object->inner[1] = (0 - (PA_RandMinMax(minspeedY,maxspeedY) << 1));
	object->inner[2] = 0;
	object->inner[3] = 0;
	object->inner[4] = gravityX;
	object->inner[5] = gravityY;
	object->inner[6] = 255; // Alpha
}		

void _ds_objects_p_manageSmoke(ds_t_object *object) {
   // First, changes the actual speed of Juni - soften X, more Y
   if (object->inner[0] < 0) {
      object->inner[0] += object->inner[4];
	} else {
	   object->inner[0] -= object->inner[4];
	}
	object->inner[1] -= object->inner[5];
	if (object->inner[1] < -1000)
		object->inner[1] = -1000;
	
	// Accumulates speed
	object->inner[2] += object->inner[0];
	object->inner[3] += object->inner[1];	
	
	// Now, changes the sprite
	ds_objects_lib_beh_particleMMF2_Move(object, object->inner[2] / 1024, object->inner[3] / 1024,0);	

	// (De)Accumulates speed
	object->inner[2] -= ((object->inner[2] / 1024) * 1024);
	object->inner[3] -= ((object->inner[3] / 1024) * 1024);
	
	// Alpha Smoke
	object->inner[6] -= 1;	
	ds_3dsprite_setAlpha(object->sprite,object->inner[6]);
}

/*void _ds_objects_p_manageLeaveInit(ds_t_object *object, int minSpeed, int maxSpeed, int iniDir, int changeDir, int spriteMov) {
   object->inner[0] = minSpeed; // Minimum Speed of the leave
   object->inner[1] = maxSpeed; // Maximum Speed of the leave
   object->inner[2] = 0; // Cumulative speed of the leave
   	// Remember: "At speed 100, it would move at 12.5 pixels per frame" (50 fps)
   	// Therefore: Speed 100 = 625 px/second => 10.41 pixels / frame
   	// Approx: Speed = 10, needs to move 1 pixel.
   object->inner[3] = iniDir; // Direction of the leave (0123? 1 => \, 2=> |, 3 => /)
   object->inner[4] = changeDir; // When the leave needs to change its direction
   	// Example: every 0.2 sec => every 12 "ticks"
   object->inner[9] = 0; // Actual Sprite
   object->inner[10] = spriteMov; // Sprite Speed
}
 
void _ds_objects_p_manageLeave(ds_t_object *object) {
   int movPix;
   
   // First, cycles the leave movement
   if (object->inner[10] != 0) {
   if ((ds_global_tick % object->inner[10]) == 0) {
	   object->inner[9]++;
	   if (object->inner[9] >= ds_3dsprite_getMaxFrame(object->sprite)) {
	      object->inner[9] = 0;
	   }  
		ds_3dsprite_setFrame(object->sprite,object->inner[9]);
   }
	}   
	
	// Now, changes its movement   
   if ((ds_global_tick % object->inner[4]) == 0) {
	   if (PA_RandMinMax(0,1) == 1)
	   	object->inner[4] = 1; // south-east
	   else 
	   	object->inner[4] = 3; // south-west
   }
   
   // Move the particle!
   if (PA_RandMinMax(0,1) == 0)
   	object->inner[2] += object->inner[0];
   else 
   	object->inner[2] += object->inner[1];
   movPix = object->inner[2] / 10; // REAL speed
   object->inner[2] -= (object->inner[2] / 10) * 10; // Works due to integer calculations...
   if (object->inner[4] == 1)
   	ds_objects_lib_beh_particleMMF2_Move(object, movPix, movPix, 1);	
   else 
   	ds_objects_lib_beh_particleMMF2_Move(object, -movPix, movPix, 1);   
}*/        

void _ds_objects_p_manageLeave2Init(ds_t_object *object, int minSpeed, int maxSpeed, int iniDir, int changeDir, int spriteMov) {
   object->inner[0] = minSpeed; // Minimum Speed of the leave
   object->inner[1] = maxSpeed; // Maximum Speed of the leave
   object->inner[2] = 0; // Cumulative speed of the leave
   	// Remember: "At speed 100, it would move at 12.5 pixels per frame" (50 fps)
   	// Therefore: Speed 100 = 625 px/second => 10.41 pixels / frame
   	// Approx: Speed = 10, needs to move 1 pixel.
   object->inner[3] = iniDir; // Direction of the leave (4 => \, 3 => |, 2 => /, 1 => -, 0 => \)
   object->inner[4] = changeDir; // When the leave needs to change its direction
   	// Example: every 0.2 sec => every 12 "ticks"
   object->inner[9] = 0; // Actual Sprite
   object->inner[10] = spriteMov; // Sprite Speed
}
 
void _ds_objects_p_manageLeave2(ds_t_object *object, int ul, int l, int dl, int d, int dr) {
   int movPix;
   
   // First, cycles the leave movement
   if (object->inner[10] != 0) {
   if ((ds_global_tick % object->inner[10]) == 0) {
	   object->inner[9]++;
	   if (object->inner[9] >= ds_3dsprite_getMaxFrame(object->sprite)) {
	      object->inner[9] = 0;
	   }  
		ds_3dsprite_setFrame(object->sprite,object->inner[9]);
   }
	}   
	
	// Now, changes its movement   
   if ((ds_global_tick % object->inner[4]) == 0) {
      int dir = PA_RandMinMax(1,dr);
     	if (dir <= ul)
      	object->inner[5] = 0; // up-left
      else if (dir <= l)
      	object->inner[5] = 1; // left
      else if (dir <= dl)
      	object->inner[5] = 2; // down-left
      else if (dir <= d)
      	object->inner[5] = 3; // down
      else 
	   	object->inner[5] = 4; // down-right
   }
   
   // Move the particle!
   if (PA_RandMinMax(0,1) == 0)
   	object->inner[2] += object->inner[0];
   else 
   	object->inner[2] += object->inner[1];
   movPix = object->inner[2] / 10; // REAL speed
   object->inner[2] -= (object->inner[2] / 10) * 10; // Works due to integer calculations...
   if (object->inner[5] == 0) // UL
   	ds_objects_lib_beh_particleMMF2_Move(object, -movPix, -movPix, 1);	
   else 
   if (object->inner[5] == 1) // L
   	ds_objects_lib_beh_particleMMF2_Move(object, -movPix, 0, 1);	
   else 
	if (object->inner[5] == 2) // DL
   	ds_objects_lib_beh_particleMMF2_Move(object, -movPix, movPix, 1);	
   else
   if (object->inner[5] == 3) // D
   	ds_objects_lib_beh_particleMMF2_Move(object, 0, movPix, 1);   
   else
   if (object->inner[5] == 4) // DR
   	ds_objects_lib_beh_particleMMF2_Move(object, movPix, movPix, 1);   
}        


// RAIN DROP 1 [O00]
//..........................................................................................
int _ds_objects_p_o00_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o00_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Let the rain fall...
	ds_objects_lib_beh_particleMMF2_Move(object, 0, 6, 1);   
   
   // Everything went OK...
   return 1;
}      

// RAIN DROP 2 [O01]
//..........................................................................................
int _ds_objects_p_o01_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o01_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Let the rain fall...
	ds_objects_lib_beh_particleMMF2_Move(object, 0, 5, 1);   
   
   // Everything went OK...
   return 1;
}      

// RAIN DROP 3 [O02]
//..........................................................................................
int _ds_objects_p_o02_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o02_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Let the rain fall...
	ds_objects_lib_beh_particleMMF2_Move(object, 0, 4, 1);   
   
   // Everything went OK...
   return 1;
}      

// PARTICLE FALLING [O03]
//..........................................................................................
int _ds_objects_p_o03_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
																								  // <TODO> Good stress test: Delete this flag ^_-
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o03_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Particles Falling!!!!
   ds_objects_lib_beh_particleMMF2(object,6); //ds_objects_lib_beh_particleFalling(object);   
   
   // Everything went OK...
   return 1;
}      

// FLYING BULLETS | (^) [O04]-[011]
//..........................................................................................
int _ds_objects_p_oflying_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
																								  // <TODO> Good stress test: Delete this flag ^_-
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_oflying_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Flying Bullet (Direction)
	ds_objects_lib_beh_particleFlyingBullet(object);   
   
   // Everything went OK...
   return 1;
}      

// PARTICLE FOLLOWING JUNI (Homing Bullet) [O12]
//..........................................................................................
int _ds_objects_p_o12_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
																								  // <TODO> Good stress test: Delete this flag ^_-
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o12_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Follow Juni at all costs!!!!
   ds_objects_lib_beh_particleMMF2Follow(object,6);
	//ds_objects_lib_beh_particleFollowing(object);   
   
   // Everything went OK...
   return 1;
}

// DEATH SMOKE! (Special Smoke) [O17]
//..........................................................................................
int _ds_objects_p_o17_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	//object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Initialize & Choose the smoke
	_ds_objects_p_manageSmokeInit(object,256,512,128,256,5,5);

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o17_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Follow Juni at all costs!!!!
	_ds_objects_p_manageSmoke(object);   
   
   // Everything went OK...
   return 1;
}

// DOUBLE JUMP SMOKE [O18]
//..........................................................................................
int _ds_objects_p_o18_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o18_manage(void *objp) {
   ds_t_object *object = objp;
   
 	ds_objects_lib_beh_cycleDie(object, 3);
    
   // Everything went OK...
   return 1;
}            

// SAVE SPOT [O19]
//..........................................................................................
int _ds_objects_p_o19_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->inner[0] = 160;	
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o19_manage(void *objp) {
   ds_t_object *object = objp;
   
	object->inner[0] -= 2;	
	ds_3dsprite_setAlpha(object->sprite,object->inner[0]);
	if (object->inner[0] <= 0) {
	   object->_deleteme = 1;
	}   
	    
   // Everything went OK...
   return 1;
}

// PARTICLE LEAVE (Bank 7 - Object 2) [O20]
//..........................................................................................
int _ds_objects_p_o20_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	_ds_objects_p_manageLeave2Init(object,5,8,1,12,5);
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o20_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Let the leave fall...
   _ds_objects_p_manageLeave2(object,0,0,40,60,100);   
   
   // Everything went OK...
   return 1;
}      

// PARTICLE SNOW (Bank 7 - Object 4) [O20]
//..........................................................................................
int _ds_objects_p_o21_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	_ds_objects_p_manageLeave2Init(object,3,6,1,12,0);
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o21_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Let the leave fall...
   _ds_objects_p_manageLeave2(object,0,0,40,60,100);   
   
   // Everything went OK...
   return 1;
}      
            
// GREEN LEAVE (Bank 7 - Object 11) [O22]
//..........................................................................................
int _ds_objects_p_o22_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	_ds_objects_p_manageLeave2Init(object,5,8,1,12,5);
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o22_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Let the leave fall...
   _ds_objects_p_manageLeave2(object,5,40,80,100,100);   
   
   // Everything went OK...
   return 1;
}      

// PURPLE LEAVE (Bank 7 - Object 7) [O23]
//..........................................................................................
int _ds_objects_p_o23_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	_ds_objects_p_manageLeave2Init(object,5,8,1,12,5);
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o23_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Let the leave fall...
   _ds_objects_p_manageLeave2(object,5,40,80,100,100);   
   
   // Everything went OK...
   return 1;
}      

// WHITE LEAVE (Bank 7 - Object 13) [O24]
//..........................................................................................
int _ds_objects_p_o24_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	_ds_objects_p_manageLeave2Init(object,4,7,1,12,5);
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o24_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Let the leave fall...
   _ds_objects_p_manageLeave2(object,0,0,36,70,100);      
   
   // Everything went OK...
   return 1;
}      

// DUST (Bank 7 - Object 15 G.14)) [O31]
//..........................................................................................
int _ds_objects_p_o31_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
	_ds_objects_p_manageLeave2Init(object,15,21,1,6,5);
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o31_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Let the leave fall...
   _ds_objects_p_manageLeave2(object,35,85,100,100,100);      
   
   // Everything went OK...
   return 1;
}

// LIGHT - HORIZONTAL GLOW (Bank 8 - Object 1) [O32]
//..........................................................................................
int _ds_objects_p_o32_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_3dsprite_setAlpha(object->sprite, 128);

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}         

// LIGHT - VERTICAL GLOW (Bank 8 - Object 2) [O33]
//..........................................................................................
int _ds_objects_p_o33_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_3dsprite_setAlpha(object->sprite, 128);

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

// LIGHT - LAMP GLOW, Others (Bank 8 - Object 3-5, 9) [O34-36(37)]
//..........................................................................................
int _ds_objects_p_o34_36_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	ds_3dsprite_setAlpha(object->sprite, 128);

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}

int _ds_objects_p_o34_36_manage(void *objp) {
   ds_t_object *object = objp;
   
	ds_objects_lib_beh_cycle(object,12);
	
   return 1;
}

// SEED [O38] & FIREBALL [O39, O45] & GREEN GLOW [O41]
//..........................................................................................
int _ds_objects_p_o38_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o38_manage(void *objp) {
   ds_t_object *object = objp;
   
   // MMF2-style particle
	ds_objects_lib_beh_particleMMF2(object,6);
   
   // Everything went OK...
   return 1;
}            

int _ds_objects_p_o39_manage(void *objp) {
   ds_t_object *object = objp;
   
   // MMF2-style particle
	ds_objects_lib_beh_particleMMF2(object,4); // This one is slower
   
   // Everything went OK...
   return 1;
}

// GLOW "DIE ON END" [O40]
//..........................................................................................
int _ds_objects_p_o40_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o40_manage(void *objp) {
   ds_t_object *object = objp;
   
   // MMF2-style particle
	ds_objects_lib_beh_particleMMF2(object,6);
	if (ds_objects_lib_beh_particleMMF2_isStopped(object)) {
	   object->_deleteme = 1;
	}   
   
   // Everything went OK...
   return 1;
}

// ALPHA GLOW [O42]
//..........................................................................................
int _ds_objects_p_o42_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o42_manage(void *objp) {
   ds_t_object *object = objp;
   
   // MMF2-style particle
	ds_objects_lib_beh_particleMMF2(object,6);
   
   // Everything went OK...
   return 1;
}

// GLOW "DIE ON END" TREMBLE [O43]
//..........................................................................................
int _ds_objects_p_o43_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o43_manage(void *objp) {
   ds_t_object *object = objp;
   
   // MMF2-style particle, trembling
   object->x += -1 + PA_RandMinMax(0,2);
   object->y += -1 + PA_RandMinMax(0,2);
	ds_objects_lib_beh_particleMMF2(object,6);
	if (ds_objects_lib_beh_particleMMF2_isStopped(object)) {
	   object->_deleteme = 1;
	}   
   
   // Everything went OK...
   return 1;
}

// WALL LAUNCHER PARTICLE [O44]
//..........................................................................................
int _ds_objects_p_o44_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o44_manage(void *objp) {
   ds_t_object *object = objp;
   
   // MMF2-style particle
	ds_objects_lib_beh_particleMMF2(object,6);
	if (!object->inner[11]) {
	   // (but increase speed by one [factor of 25] if needed :-P)
	   int aX = abs(object->inner[0]);
	   int aY = abs(object->inner[1]);
	   int fX = (aX > aY)?25:(25 * aX) / aY;
	   int fY = (aY > aX)?25:(25 * aY) / aX;
		if (object->inner[0] < 0)
			object->inner[0]-=fX;
		else 
			object->inner[0]+=fX;
		if (object->inner[1] < 0)
			object->inner[1]-=fY;
		else 
			object->inner[1]+=fY;
	}	
   
   // Everything went OK...
   return 1;
}            

// ELEMENTAL BANG! [O46-47]
//..........................................................................................
int _ds_objects_p_oBang_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_oBang_manage(void *objp) {
   ds_t_object *object = objp;
   
 	ds_objects_lib_beh_cycleDie(object, 3);
    
   // Everything went OK...
   return 1;
}                                

// ALPHA DISK [O48]
//..........................................................................................
int _ds_objects_p_o48_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o48_manage(void *objp) {
   ds_t_object *object = objp;

	object->inner[0] += 2;
	ds_3dsprite_setAlpha(object->sprite,object->inner[0]);
	if (object->inner[0] > 128)
		object->_deleteme = 1;
    
   // Everything went OK...
   return 1;
}   

// GENERATOR PARTICLE [O49]
//..........................................................................................
int _ds_objects_p_o49_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o49_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle = NULL;
   
   // MMF2-style particle
	ds_objects_lib_beh_particleMMF2(object,6);
	
	if (object->_deleteme) {
	   int i,s,sx,sy;
	   for (i = 0; i < 17; i++) {
	      s = 12;
	      int angle = 11.25 * i; // r=11.25
	      sy = (int)(ds_util_fastSin(angle) * s);
	      sx = (int)(ds_util_fastCos(angle) * s);
			particle = ds_objects_createParticle(object->x, 
								object->y + ((object->inner[11]==-1)?-12:0), 
								object->layer, 
								44);
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						(object->inner[11]==-1)?-sy:sy, 
				 		0, 0, 3);				 		
	   }		 
	}	
   
   // Everything went OK...
   return 1;
}

// FALLING PARTICLE [O50]
//..........................................................................................
int _ds_objects_p_o50_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o50_manage(void *objp) {
   ds_t_object *object = objp;
   
 	ds_objects_lib_beh_particleMMF2(object,6);
    
   // Everything went OK...
   return 1;
}                                
 
// CLOUD PARTICLE [O51]
//..........................................................................................
int _ds_objects_p_o51_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o51_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Normal management
 	ds_objects_lib_beh_particleMMF2(object,6);
 	
 	// Acceleration
   int aX = abs(object->inner[0]);
   int aY = abs(object->inner[1]);
   int fX = (aX > aY)?20:(20 * aX) / aY;
   int fY = (aY > aX)?20:(20 * aY) / aX;
	if (object->inner[0] < 0)
		object->inner[0]-=fX;
	else 
		object->inner[0]+=fX;
	if (object->inner[1] < 0)
		object->inner[1]-=fY;
	else 
		object->inner[1]+=fY;

 	// Alpha management
 	object->inner[11] -= 3;
 	if (object->inner[11] <= 1) {
 		object->_deleteme = 1;
	} else {
	  	ds_3dsprite_setAlpha(object->sprite,object->inner[11]);
	} 	
    
   // Everything went OK...
   return 1;
} 

// BIG PARTICLE [O52]
//..........................................................................................
int _ds_objects_p_o52_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA);
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o52_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   
 	ds_objects_lib_beh_particleMMF2(object,6);
 	
	if (object->_deleteme) {
	   int i,s,sx,sy;
	   for (i = 0; i < 32; i++) {
	      s = 24;
	      int angle = 11.25 * i; // r=11.25
	      sy = (int)(ds_util_fastSin(angle) * s);
	      sx = (int)(ds_util_fastCos(angle) * s);
			particle = ds_objects_createParticle(object->x + 12, 
								object->y - 12, 
								object->layer, 
								40);
		   ds_objects_lib_beh_particleMMF2_Init(particle, 
						sx, 
						sy, 
				 		0, 15, 3);				 		
	   }		 
	}	
    
   // Everything went OK...
   return 1;
}   

// ELECTRIC PARTICLE [O53]
//..........................................................................................
int _ds_objects_p_o53_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o53_manage(void *objp) {
   ds_t_object *object = objp;
   
   // MMF2-style particle
	ds_objects_lib_beh_particleMMF2(object,6);
   
   // Everything went OK...
   return 1;
}                                                                        

// FADE PARTICLE [O54]
//..........................................................................................
int _ds_objects_p_o54_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o54_manage(void *objp) {
   ds_t_object *object = objp;
   ds_t_object *particle;
   
   // MMF2-style particle
	ds_objects_lib_beh_particleMMF2(object,6);
	
	// Create new "red" particle
	particle = ds_objects_createParticle(object->x, object->y - 16, object->layer, 55);
								   
   // Everything went OK...
   return 1;
}                                                                        

// RED PARTICLE [O55]
//..........................................................................................
int _ds_objects_p_o55_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;
		
	object->inner[11] = 140;
	ds_3dsprite_setAlpha(object->sprite,object->inner[11]);

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o55_manage(void *objp) {
   ds_t_object *object = objp;
   
   // Fade away...
	object->inner[11] -= 40;
	if (object->inner[11] <= 0) {
		object->_deleteme = 1;
	} else {
	   ds_3dsprite_setAlpha(object->sprite,object->inner[11]);
	}   		
   
   // Everything went OK...
   return 1;
}                                                                        

// GHOST SLIME [O56]
//..........................................................................................
int _ds_objects_p_o56_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;

	// Ghost!   
  	ds_objects_lib_beh_ghostIni(object,1,30,120,30);

   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o56_manage(void *objp) {
   ds_t_object *object = objp;
   
   // MMF2-style particle
	ds_objects_lib_beh_particleMMF2(object,6);
	
	// Ghost!   
   ds_objects_lib_beh_ghost(object,30,120,30);
	
   // Everything went OK...
   return 1;
}

// NINJA STAR [O57]
//..........................................................................................
int _ds_objects_p_o57_create(u8 bank, u8 obj, void *objp) {
   ds_t_object *object = objp;
   
   // Initialize
	if (ds_objects_lib_initObject(bank, obj, object) == 0)
		return 0;
		
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_GLOBAL_IMA); // <TODO> All are the same!!!
	object->flags = ds_util_bitSet16(object->flags,DS_C_OBJ_F_HARMFUL);
		
	if (ds_objects_lib_initObjectImage(bank, obj, object) == 0)
		return 0;

	// Specific Operations
   object->type = DS_C_OBJ_PARTICLE;
   object->managed = 1;
   
   // Return 1 if I'm an event/item
   return ds_objects_lib_iseventitem(object->type);
}   

int _ds_objects_p_o57_manage(void *objp) {
   ds_t_object *object = objp;
   
   // MMF2-style particle
	ds_objects_lib_beh_particleMMF2(object,6);
	
   // Everything went OK...
   return 1;
}

                                                 
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/* Assign object data */
int ds_particles0_assign(u8 obj, ds_t_object *object) {
   if ((obj >= 4) && (obj <= 11)) {
      // Flying bullets
      object->fcreate = _ds_objects_p_oflying_create;
      object->fmanage = _ds_objects_p_oflying_manage;      
      return 1;
   } else
   if ((obj >= 13) && (obj <= 16)) {
      // Flying bullets - Bees
      object->fcreate = _ds_objects_p_oflying_create;
      object->fmanage = _ds_objects_p_oflying_manage;      
      return 1;
   } else     
   if ((obj >= 34) && (obj <= 37)) {
      // SPECIAL Light.
      object->fcreate = _ds_objects_p_o34_36_create;
      object->fmanage = _ds_objects_p_o34_36_manage;      
      return 1;
   } else     
   switch (obj) {
      case 0:
         // Rain Drop (1)
         object->fcreate = _ds_objects_p_o00_create;
         object->fmanage = _ds_objects_p_o00_manage;
         return 1;
         break; // not really necessary...
      case 1:
         // Rain Drop (2)
         object->fcreate = _ds_objects_p_o01_create;
         object->fmanage = _ds_objects_p_o01_manage;
         return 1;
         break; // not really necessary...
      case 2:
         // Rain Drop (3)
         object->fcreate = _ds_objects_p_o02_create;
         object->fmanage = _ds_objects_p_o02_manage;
         return 1;
         break; // not really necessary...         
      case 3:
         // Harmful Particle Falling
         object->fcreate = _ds_objects_p_o03_create;
         object->fmanage = _ds_objects_p_o03_manage;
         return 1;
         break; // not really necessary...                  
      case 12:
         // Particle that follows Juni
         object->fcreate = _ds_objects_p_o12_create;
         object->fmanage = _ds_objects_p_o12_manage;
         return 1;
         break; // not really necessary...                           
      case 17:
         // Smoke after Juni's death (Juni is made of smoke!)
         object->fcreate = _ds_objects_p_o17_create;
         object->fmanage = _ds_objects_p_o17_manage;
         return 1;
         break; // not really necessary...                           
      case 18:
         // Double Jump Smoke
         object->fcreate = _ds_objects_p_o18_create;
         object->fmanage = _ds_objects_p_o18_manage;
         return 1;
         break; // not really necessary...                           
      case 19:
         // Activate Save Spot
         object->fcreate = _ds_objects_p_o19_create;
         object->fmanage = _ds_objects_p_o19_manage;
         return 1;
         break; // not really necessary...                           
      case 20:
         // Falling Leave
         object->fcreate = _ds_objects_p_o20_create;
         object->fmanage = _ds_objects_p_o20_manage;
         return 1;
         break; // not really necessary...                           
      case 21:
         // Falling Snow
         object->fcreate = _ds_objects_p_o21_create;
         object->fmanage = _ds_objects_p_o21_manage;
         return 1;
         break; // not really necessary...                           
      case 22:
         // Green Leaf (1)
         object->fcreate = _ds_objects_p_o22_create;
         object->fmanage = _ds_objects_p_o22_manage;
         return 1;
         break; // not really necessary...                           
      case 23:
         // Purple Leaf (1)
         object->fcreate = _ds_objects_p_o23_create;
         object->fmanage = _ds_objects_p_o23_manage;
         return 1;
         break; // not really necessary...                           
      case 24:
         // White Leaf (1)
         object->fcreate = _ds_objects_p_o24_create;
         object->fmanage = _ds_objects_p_o24_manage;
         return 1;
         break; // not really necessary...                           
      case 25:
         // Green Leaf (2)
         object->fcreate = _ds_objects_p_o22_create;
         object->fmanage = _ds_objects_p_o22_manage;
         return 1;
         break; // not really necessary...                           
      case 26:
         // Purple Leaf (2)
         object->fcreate = _ds_objects_p_o23_create;
         object->fmanage = _ds_objects_p_o23_manage;
         return 1;
         break; // not really necessary...                           
      case 27:
         // White Leaf (2)
         object->fcreate = _ds_objects_p_o24_create;
         object->fmanage = _ds_objects_p_o24_manage;
         return 1;
         break; // not really necessary...                           
      case 28:
         // Green Leaf (3)
         object->fcreate = _ds_objects_p_o22_create;
         object->fmanage = _ds_objects_p_o22_manage;
         return 1;
         break; // not really necessary...                           
      case 29:
         // Purple Leaf (3)
         object->fcreate = _ds_objects_p_o23_create;
         object->fmanage = _ds_objects_p_o23_manage;
         return 1;
         break; // not really necessary...                           
      case 30:
         // White Leaf (3)
         object->fcreate = _ds_objects_p_o24_create;
         object->fmanage = _ds_objects_p_o24_manage;
         return 1;
         break; // not really necessary...                           
      case 31:
         // Dust
         object->fcreate = _ds_objects_p_o31_create;
         object->fmanage = _ds_objects_p_o31_manage;
         return 1;
         break; // not really necessary...                           
      case 32:
         // Horizontal Glow
         object->fcreate = _ds_objects_p_o32_create;
         return 1;
         break; // not really necessary...                           
      case 33:
         // Vertical Glow
         object->fcreate = _ds_objects_p_o33_create;
         return 1;
         break; // not really necessary...                           
      case 38:
         // Seed
         object->fcreate = _ds_objects_p_o38_create;
         object->fmanage = _ds_objects_p_o38_manage;
         return 1;
         break; // not really necessary...                           
      case 39:
         // Fireball (=seed)
         object->fcreate = _ds_objects_p_o38_create;
         object->fmanage = _ds_objects_p_o39_manage;
         return 1;
         break; // not really necessary...                           
      case 40:
         // White "die on end" particle
         object->fcreate = _ds_objects_p_o40_create;
         object->fmanage = _ds_objects_p_o40_manage;
         return 1;
         break; // not really necessary...                           
      case 41:
         // Green "basic" particle
         object->fcreate = _ds_objects_p_o38_create;
         object->fmanage = _ds_objects_p_o38_manage;
         return 1;
         break; // not really necessary...                           
      case 42:
         // "glow" particle
         object->fcreate = _ds_objects_p_o42_create;
         object->fmanage = _ds_objects_p_o42_manage;
         return 1;
         break; // not really necessary...                           
      case 43:
         // White "die on end"-tremble particle
         object->fcreate = _ds_objects_p_o43_create;
         object->fmanage = _ds_objects_p_o43_manage;
         return 1;
         break; // not really necessary...                           
      case 44:
         // Wall Launcher Particle
         object->fcreate = _ds_objects_p_o44_create;
         object->fmanage = _ds_objects_p_o44_manage;
         return 1;
         break; // not really necessary...                           
      case 45:
         // Special Firewall particle (=seed)
         object->fcreate = _ds_objects_p_o38_create;
         object->fmanage = _ds_objects_p_o38_manage;
         return 1;
         break; // not really necessary...                           
      case 46:
         // Bang! - Red
         object->fcreate = _ds_objects_p_oBang_create;
         object->fmanage = _ds_objects_p_oBang_manage;
         return 1;
         break; // not really necessary...                           
      case 47:
         // Bang! - Yellow
         object->fcreate = _ds_objects_p_oBang_create;
         object->fmanage = _ds_objects_p_oBang_manage;
         return 1;
         break; // not really necessary...                           
      case 48:
         // Alpha Disk
         object->fcreate = _ds_objects_p_o48_create;
         object->fmanage = _ds_objects_p_o48_manage;
         return 1;
         break; // not really necessary...                           
      case 49:
         // Generator Particle
         object->fcreate = _ds_objects_p_o49_create;
         object->fmanage = _ds_objects_p_o49_manage;
         return 1;
         break; // not really necessary...                           
      case 50:
         // Falling Particle
         object->fcreate = _ds_objects_p_o50_create;
         object->fmanage = _ds_objects_p_o50_manage;
         return 1;
         break; // not really necessary...                           
      case 51:
         // Cloud Particle
         object->fcreate = _ds_objects_p_o51_create;
         object->fmanage = _ds_objects_p_o51_manage;
         return 1;
         break; // not really necessary...                           
      case 52:
         // Big Particle
         object->fcreate = _ds_objects_p_o52_create;
         object->fmanage = _ds_objects_p_o52_manage;
         return 1;
         break; // not really necessary...                           
      case 53:
         // Electric Particle
         object->fcreate = _ds_objects_p_o53_create;
         object->fmanage = _ds_objects_p_o53_manage;
         return 1;
         break; // not really necessary...                           
      case 54:
         // Fade Particle
         object->fcreate = _ds_objects_p_o54_create;
         object->fmanage = _ds_objects_p_o54_manage;
         return 1;
         break; // not really necessary...                           
      case 55:
         // Red Particle
         object->fcreate = _ds_objects_p_o55_create;
         object->fmanage = _ds_objects_p_o55_manage;
         return 1;
         break; // not really necessary...                           
      case 56:
         // Ghost Slime
         object->fcreate = _ds_objects_p_o56_create;
         object->fmanage = _ds_objects_p_o56_manage;
         return 1;
         break; // not really necessary...                           
      case 57:
         // Ninja Star
         object->fcreate = _ds_objects_p_o57_create;
         object->fmanage = _ds_objects_p_o57_manage;
         return 1;
         break; // not really necessary...                           
   }   
   return 0;
}   
