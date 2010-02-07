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

#include "ds_global.h"

/* Game "Map" - Management of the "Map" entity */
#ifndef _DS_G_MAP_
#define _DS_G_MAP_

/* Inits the map subsystem */
void ds_map_init();

/* Load the map and its associated information from the file subsystem */
int ds_map_loadHDD(int x, int y);

/* Use the already loaded data to paint the base map */
int ds_map_loadHDDPaint(int x, int y);

/* Do some operations after loading everything (expect the music) */
int ds_map_loadHDDPOST();

/* Provides the X-Y coordinates where Juni must go next, if she goes to another screen */
void ds_map_getBoundary(int boundary, int *mapx, int *mapy);

/* Copy a sprite to a flag subsystem */
void ds_map_copyFlag(u16 *ima,int xsize,int ysize,int x,int y, ds_t_mapFlag* flag);

/* Copy a value (0/1) to a flag subsystem */
void ds_map_copyFlagValue(u8 val,int xsize,int ysize,int x,int y, ds_t_mapFlag* flag);

/* Check if a sprite collides with a flag subsystem */
int ds_map_collideFlag(u16 *ima,int xsize,int ysize,int x,int y, ds_t_mapFlag* flag);

/* Check if a sprite collides with a flag subsystem in Cross mode (just check a internal cross) */
int ds_map_collideFlagCross(u16 *ima,int xsize,int ysize,int x,int y, ds_t_mapFlag* flag);

/* Check if a sprite collides with a flag subsystem - ONLY THE CENTER PIXEL */
int ds_map_collideFlagCenter(u16 *ima,int xsize,int ysize,int x,int y, ds_t_mapFlag* flag);

/* Check if a sprite is outside the map */
int ds_map_outOfMap(int xsize,int ysize,int x,int y);

/* Check if a sprite is outside the map - CENTER */
int ds_map_outOfMap_center(int xsize,int ysize,int x,int y);

/* Resets the map and its elements */
void ds_map_reset(int totalReset);

/* Manages the map  */
void ds_map_manage();

/* Returns "1" if there is a collision in that specific pixel */
int ds_map_coll(int x, int y);

/* Checks if this sprite has something solid in his feet or below its feet.
		It depends on the sprite, so it is not "Basic" */
int ds_map_collDown(int x, int y, int xsize, int ysize, int clash);

/* Checks if this sprite has something solid in his feet (23) or below its feet (24).
		It's "Basic" because manages 24y sprites, and it is not pixel-perfect, just checks below the sprite */
int ds_map_collDownBasic(int x, int y, int clash, int speedY);

/* Checks for "hitting the ceiling".
		It's "Basic" because manages 24y sprites, and it is not pixel-perfect, just checks over the sprite */
int ds_map_collUpBasic(int x, int y, int clash, int speedY);

/* Checks if the Sprite clashes in a x-based movement.
	It's "basic" because the caller decides where to check */
int ds_map_collMovBasic(int x, int y, int clash, int speedX);

/* Checks if the Sprite is still allowed to climb.
	It's "basic" because the caller decides where to check */
int ds_map_collClimbBasic(int x, int y, int clash);

/* Paints the map into the screen */
void ds_map_paint(u16 *screen, int x, int y, int dsscreen);

/* Checks if a certain position is equal to a certain bank/obj combination */
int ds_map_collBankObj(int x, int y, int bank, int obj);

#endif
