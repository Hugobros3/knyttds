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

/* 3D Sprite - Stores the information related to the sprites of the game 
		- The objects are the logical implementation of the entities of the game
		- The sprites are the physical implementation of the entities of the game 
		- NOTE: As an optimization, screen is considered as a 3D screen */
#ifndef _DS_3DSPRITE_
#define _DS_3DSPRITE_

#include <PA9.h>       // Include for PA_Lib

/* Initializes the 3d sprite system - Part 1 */
void ds_3dsprite_init1();

/* Initializes the 3d sprite system - Part 2 */
void ds_3dsprite_init2();

/* Initializes the 3d sprite system AFTER loading a game screen */
void ds_3dsprite_initAfterGameScreen();

/* Deletes a single sprite. If force == 1, also delete the HW sprite */
void ds_3dsprite_deleteSprite(int id, int force);

/* Resets the 3d sprite system. 
	force == 1 if we want to delete everything in the subsystems (but not Juni)*/
void ds_3dsprite_reset(int force);

/* Load the bank/obj object into certain screen coordinates x/y, returns the id of the specific sprite, -1 if error */
int ds_3dsprite_load(u8 bank, u8 obj, int x, int y, int isUnique);

/* Loads an "special" sprite (no bank no obj), where the texture is already given by the user */
int ds_3dsprite_loadSpecial(u16 *texSpecial, int xs, int ys, int x, int y);

/* "Cleans" the HW Sprite system, in case there are HW sprites around */
void ds_3dsprite_cleanHWSprites();

/* Change the obj stored inside a sprite 
		- Only useful for pre-existing stuff (Juni!) */
int ds_3dsprite_changeObj(int id, u8 obj);

/* Change the bank/obj stored inside a sprite 
      - It loads from the HDD if needed */
void ds_3dsprite_changeBankObj(int id, u8 bank, u8 obj);

/* Obtains a pointer to the sprite number "id" frame "frame" */
u16 *ds_3dsprite_getSpriteFrame(int id, int frame);

/* Obtains a pointer to the sprite number "id", with the actual frame*/
u16 *ds_3dsprite_getSprite(int id);

/* Returns the ABSOLUTE X-coordinate of the sprite number "id" */
int ds_3dsprite_getX(int id);

/* Returns the ABSOLUTE Y-coordinate of the sprite number "id" */
int ds_3dsprite_getY(int id);

/* Sets the ABSOLUTE X-coordinate of the sprite number "id" */
void ds_3dsprite_setX(int id, int x);

/* Sets the ABSOLUTE Y-coordinate of the sprite number "id" */
void ds_3dsprite_setY(int id, int y);

/* Sets the ABSOLUTE X/Y-coordinate of the sprite number "id" 
	applying corrections using a certain bit array.
	This is a HACK for special CO that will only affect Juni. */
void ds_3dsprite_setXY_HackCO(int id, int x, int y, void *flagHVoid, int flagSX, int flagSY);

/* Returns the SIZE X of the sprite number "id" */
int ds_3dsprite_getXSize(int id);

/* Returns the SIZE Y of the sprite number "id" */
int ds_3dsprite_getYSize(int id);

/* "Draws" all the sprite into the screen */
void ds_3dsprite_drawAll(int camX, int camY);

/* Returns the actual Frame of a certain sprite */
int ds_3dsprite_getFrame(int id);

/* Returns the maximum number of frames of a certain sprite */
int ds_3dsprite_getMaxFrame(int id);

/* Returns whether the sprite looks @ the right or not */
int ds_3dsprite_lookRight_LR(int id);

/* Returns the first frame of a certain sprite - if the sprite stores both left and right */
int ds_3dsprite_getIniFrame_LR(int sprite, int dir, int face);   

/* Returns the last frame (not included) of a certain sprite - if the sprite stores both left and right */
int ds_3dsprite_getMaxFrame_LR(int sprite, int dir, int face);

/* Sets the actual Frame of a certain sprite */
void ds_3dsprite_setFrame(int id, int frame);

/* Forces the priority of a certain sprite */
void ds_3dsprite_setPrio(int id, int prio);

/* Sets the alpha of a certain sprite */
void ds_3dsprite_setAlpha(int id, int alpha);

/* Gets the alpha of a certain sprite (0..255) */
u8 ds_3dsprite_getAlpha(int id);

/* Marks an sprite as invisible or visible */
void ds_3dsprite_markInvisible(int id, int invisible);

/* Checks if the sprite contains a HW sprite */
int ds_3dsprite_hasHWSprite(int id);

/* Gets "invisible" state */
int ds_3dsprite_getInvisible(int id);

#endif
