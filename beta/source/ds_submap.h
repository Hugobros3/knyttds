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


/* Game "Submap" - Load all map-related information from the HDD. Also, implements 
   certain cache strategies, if necessary */
#ifndef _DS_G_SUBMAP_
#define _DS_G_SUBMAP_

#include "ds_global.h"

/* Load a certain room from the subsystem */
int ds_submap_loadRoom(int x, int y, ds_t_room *room);

/* Resets a room (warning!, we enter into the void! ^_-) */
void ds_submap_resetRoom(ds_t_room *room);

/* Starts optimization subsystems */
void ds_submap_initOpt();

/* Performs optimizations */
void ds_submap_doOpt();

/* Returns if we are still performing optimizations */
int ds_submap_inOpt();

/* Finishes the optimization process - even if it didn't provide any results */
void ds_submap_endOpt();

/* Deletes a tileset from the subsystem */
void ds_submap_deleteTileset(int tilesetType, ds_t_15bpp *ima);

/* Load a certain tileset from the subsystem */
int ds_submap_loadTileset(int tilesetType, u8 tileset, ds_t_15bpp *ima, int ispartial);

#endif

