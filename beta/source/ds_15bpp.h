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

/* 15bpp - Load images in 15bpp format into the memory subsystem.
	It allows both "raw" 15bpp images and "png" 16bpp images */
#ifndef _DS_15BPP_
#define _DS_15BPP_

#include <PA9.h>       // Include for PA_Lib
#include "ds_global.h"

/* Inits a 15bpp structure. */
void ds_15bpp_init(ds_t_15bpp *ima);

/* Init a 15bpp structure by using an existing "raw" (pointer) image. 
		Note that if the noDel parameter is active, this can be passed to _delete, 
			but the raw will not be freed by it */
void ds_15bpp_initRaw(ds_t_15bpp *ima, u16 *rawImage, int width, int height, int noDel);

/* Saves a 15bpp structure to the HDD, into a raw 15bpp file. 
	Also calls a callback function.
   NOTE: The Raw File is "tile-optimized" if indicated. */
int ds_15bpp_saveRawFileCallback(char *file, ds_t_15bpp *ima, int tileOptimized, void (*callback)());

/* Saves a 15bpp structure to the HDD, into a raw 15bpp file. 
   NOTE: The Raw File is "tile-optimized" if indicated. */
int ds_15bpp_saveRawFile(char *file, ds_t_15bpp *ima, int tileOptimized);

/* Loads a 15bpp structure from the HDD. The file can be either a png or a raw file.
		ispng - 1 if the file is a png, 0 if the file is a raw 15bpp 
		ispartial - 1 if the file (raw) is not completely loaded */
int ds_15bpp_load(char *file, ds_t_15bpp *ima, int ispng, int ispartial);

/* Loads a 15bpp structure from the HDD. 
	If the sprite size is too big, or the area is too big, 
	we load ONLY the first frame. */
int ds_15bpp_loadLimited(char *file, ds_t_15bpp *ima, int *sx, int *sy, 
									int maxsx, int maxsy, int maxArea);

/* Get a RAW Tile in a Partial image */
int ds_15bpp_getRawTile(ds_t_15bpp *ima, int tile);

/* Get if a RAW Tile is stored in a Partial image */
int ds_15bpp_storeRawTile(ds_t_15bpp *ima, int tile);

/* Resizes (reduces) a image into an empty (do not contains anything) image. Do not copy alpha information. No error checking */
int ds_15bpp_resize(ds_t_15bpp *dest,ds_t_15bpp *ori,int newx, int newy);

/* Destroys a 15bpp structure. */
void ds_15bpp_delete(ds_t_15bpp *ima);

/* Paints a 15bpp into a certain screen buffer. Ignores alpha. The image must be x4. No check for boundaries (!). */
void ds_15bpp_putScreen(u16 *screen, ds_t_15bpp *ima, int x, int y, int flush);

/* Paints a 15bpp **directly** into the screen. Ignores alpha. The image must be x4. No check for boundaries (!). */
void ds_15bpp_paintScreen(int dsscreen, ds_t_15bpp *ima, int x, int y, int flush);

/* Paints a 15bpp into a certain screen buffer, Checking Limits. Ignores alpha. The image must be x4. */
void ds_15bpp_putScreenCropped(u16 *screen, ds_t_15bpp *ima, int x, int y, int flush, int xi, int yi, int xf, int yf);

/* Paints a 15bpp **directly** into the screen. Checking Limits. Ignores alpha. The image must be x4. */
void ds_15bpp_paintScreenCropped(int dsscreen, ds_t_15bpp *ima, int x, int y, int flush, int xi, int yi, int xf, int yf);

/* Paints a 15bpp into a certain screen buffer. Ignores alpha. The image must be x4. No check for boundaries (!). */
void ds_15bpp_putScreenPartial(u16 *screen, ds_t_15bpp *ima, int x, int y, int lx, int ly, int sizex, int sizey);

/* Paints a 15bpp into a certain screen buffer. WITH alpha. No check for boundaries (!). */
void ds_15bpp_putScreenAlpha(u16 *screen, ds_t_15bpp *ima, int x, int y);

/* Paints a 15bpp into the **Screen**. WITH alpha. No check for boundaries (!). */
void ds_15bpp_paintScreenAlpha(int screen, ds_t_15bpp *ima, int x, int y);

#endif
