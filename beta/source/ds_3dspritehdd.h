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

/* 3D Sprite - Stores the image information related to the sprites of the game.
   Also stores the information about Juni. Juni is known internally as DS_C_JUNI_BANK (255) */
#ifndef _DS_3DSPRITEHDD_
#define _DS_3DSPRITEHDD_

#include <PA9.h>       // Include for PA_Lib

/* Initializes the 3d sprite "hard disk" system */
void ds_3dspritehdd_init();

/* Resets the 3d sprite "hard disk" system. Do not delete any images that should be inside the "cache" 
		if force==1, deletes even the sprites in the cache O_o*/
void ds_3dspritehdd_reset(int force);

/* Load a certain CO into the sprite "hard disk" system.
	Must be called before the normal ds_3dspritehdd_load */
int ds_3dspritehdd_loadCO(u8 obj, char *image, int *sx, int *sy);

/* Load the bank/obj into the sprite "hard disk" system */
int ds_3dspritehdd_load(u8 bank, u8 obj);

/* Obtains a pointer to the sprite bank/obj in the frame "frame" */
u16 *ds_3dspritehdd_getSprite(u8 bank, u8 obj, int frame);

/* Returns the width (X-size) of the sprite bank/obj */
int ds_3dspritehdd_getWidth(u8 bank, u8 obj);

/* Returns the height (Y-size) of the sprite bank/obj */
int ds_3dspritehdd_getHeight(u8 bank, u8 obj);

/* Returns both width and height of the sprite bank/obj */
void ds_3dspritehdd_getXY(u8 bank, u8 obj, int *x, int *y);

/* Returns the max number of frames contained inside the sprite bank/obj */
int ds_3dspritehdd_getFrames(u8 bank, u8 obj);

#endif
