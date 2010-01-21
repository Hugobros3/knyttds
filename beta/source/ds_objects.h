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

/* Objects - Manage the "living" entities in the knytt stories world */
#ifndef _DS_OBJECTS_
#define _DS_OBJECTS_

/* Inits the object subsystem */
void ds_objects_init();

/* Creates an object, for external purposes (e.g. particles) */
void *ds_objects_createParticle(int x, int y, int layer, int oo);   

/* Creates an object, for internal purposes (e.g. events, items, living beings) */
void *ds_objects_createObject(int x, int y, int layer, int bb, int oo);

/* Includes a post-management function */
void ds_objects_addAfterManagementFunction(ds_t_fpostmanage func);

/* Preloads the objects from the world (room) */
void ds_objects_loadHDD();

/* Empties the actual state of the objects - a new room awaits us!!!! */
void ds_objects_reset();

/* Activates the execution routine of an object that collided with Juni */
void ds_objects_collide(int layer, int xtile, int ytile);

/* Manages the objects of this screen */
void ds_objects_manage();

/* Tells the system that a certain instance wants to be executed */
void ds_objects_instancephore(ds_t_object *object);

/* Changes the semaphore of a certain OC - ON */
void ds_objects_semaphoreON(ds_t_object *object);

/* Changes the semaphore of a certain OC - OFF */
void ds_objects_semaphoreOFF(ds_t_object *object);

/* Set the "blink" property of an object */
void ds_objects_setBlink(ds_t_object *object, u8 blink);


#endif
