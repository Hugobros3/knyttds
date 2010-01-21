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

/* Juni... our heroine!!!! This unit manages all things related to Juni ^^ */
#ifndef _DS_JUNI_
#define _DS_JUNI_

#include "ds_global.h"

/* Initializes the "Juni" structure. Flag is active if movement information / internal information should be reset */
int ds_juni_init(int x, int y, int resetmov, int resetinner);

/* Resets the "Juni" structure */
int ds_juni_reset();

/* Manages Juni... Be Juni, my friend -:) */
void ds_juni_manage();

/* Get Juni X coordinate... including hologram coordinates :-P */
int ds_juni_getXVirtual();

/* Get Juni Y coordinate... including hologram coordinates :-P */
int ds_juni_getYVirtual();

/* Updates position && all the sprites of Juni */
void ds_juni_updateSprites(int newx, int newy);

/* Check if Juni is "on the ground" - walking, running, stopped */
int ds_juni_isOnTheGround();

/* Check if Juni is "on the ground" moving - walking, running */
int ds_juni_isOnTheGroundMoving();

/* Checks if Juni is facing to the right */
int ds_juni_faceRight();

#endif
