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

/* Button - manage "Buttons" for the menus and others */
#ifndef _DS_BUTTON_
#define _DS_BUTTON_

#include "ds_global.h"

/* Inits the button subsystem. To be executed only ONCE */
void ds_button_init();

/* Resets the button subsystem. Execute in _finish if a button is managed in a screen */ 
void ds_button_reset();

/* Gets the ID of a button (void *) */
int ds_button_getID(void *btn);

/* Creates a button, returns its ID */
int ds_button_create(int x, int y, int sx, int sy, int screen, char *image); 

/* Adds an image that will be shown once a button is clicked */
int ds_button_addClick(int id, int time, char *ima);

/* Adds an image to a function */
int ds_button_addImage(int id, int cx, int cy, char *ima); 

/* Adds a text to a function */
int ds_button_addText(int id, int cx, int cy, char *text, u16 color, int size, int centered);

/* Adds a management function */
int ds_button_addManagement(int id, ds_t_fmanage manage, int manageNotClick); 

/* Changes the internal variable */
void ds_button_setInner(int id, int inner); 

/* Gets the internal variable */
int ds_button_getInner(int id); 

/* Paint the buttons in the screen buffers */
void ds_button_paint();

/* Check a possible click by the user. Returns 1 if screen needs to change */
int ds_button_checkClick(int stylusx,int stylusy, int onclick);

/* Tells a button to be hidden (or not) */
int ds_button_hide(int id, int hidden);

/* Selects the button */
int ds_button_select(int id, int selected);

/* Is this button selected? */
int ds_button_getSelect(int id);

/* Updates the local image of a button (deletes the old one, load new one) */
int ds_button_updateImage(int id, int imaid, char *ima);

/* Updates the internal image of a button (deletes the old one, load new one) */
int ds_button_updateLocalImage(int id, char *ima);

/* Updates the internal image of a button (deletes the old one, copy a preloaded one. No size checking!) */
int ds_button_updateLocalImageForce(int id, ds_t_15bpp *ima);

/* Updates the text of a button */
int ds_button_updateText(int id, int textid, char *text);

/* Gets the local x coordinate of a click (pointer parameter)*/
int ds_button_getLocalX_p(void *btnvoid, int x); 

/* Gets the local y coordinate of a click (pointer parameter)*/
int ds_button_getLocalY_p(void *btnvoid, int y);

#endif
