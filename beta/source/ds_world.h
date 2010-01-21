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

/* World - Access to the global information of the world */
#ifndef _DS_WORLD_
#define _DS_WORLD_

/* Inits the world structure */
void ds_world_init();

/* Load a certain world information into memory */
int ds_world_load(const char *world, int saveslot);

/* Updates the savegame, both in memory and in HDD */
int ds_world_updateSavegame();

/* Updates the savegame, both in memory and in HDD - Parametrized */
int ds_world_updateSavegameParam(int mapx, int mapy, int junix, int juniy, u16 items, u16 flags, int gui);

/* Updates the savegame, both in memory and in HDD - Basic parameters */
int ds_world_updateSavegameBasic(int x, int y);

/* Returns the offset of a certain room in the Map.bin file */
u32 ds_world_getOffsetRoom(int x, int y);

/* Returns if a room has been visited or not */
int ds_world_getVisitedRoom(int x, int y);

/* Sets a room as visited */
void ds_world_setVisitedRoom(int x, int y);

/* Destroys a certain world information from memory */
int ds_world_destroy();

#endif
