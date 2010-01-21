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


/* Game "Camera" - Manages the area that is visualized in the DS screen */
#ifndef _DS_CAMERA_
#define _DS_CAMERA_

//#include "ds_global.h"

#define DS_C_CAM_JUNI 0
#define DS_C_CAM_COORD 1


/* Initializes the values of the camera */
void ds_camera_initCamera(int x, int y);

/* Get the X coordinate of the camera */
int ds_camera_getX();

/* Get the Y coordinate of the camera */
int ds_camera_getY();

/* Set the absolute values of the camera */
void ds_camera_setXY(int x, int y);

/* Set the camera position, based on the actual internal state, considering the main character on the coordinates provided */
void ds_camera_update(int x, int y);

/* Set the type of management that the camera will use */
void ds_camera_setType(int type, int x, int y);

/* Indicates the camera that it should move in a certain direction in this frame */
void ds_camera_moveCoord(int x, int y);

#endif
