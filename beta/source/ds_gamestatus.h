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

/* GameStatus - Show status information during the gameplay */
#ifndef _DS_GAMESTATUS_
#define _DS_GAMESTATUS_

/* Starts the subsystem. Loads all the necessary images. */
void ds_gamestatus_initOnce();

/* Inits the gamestatus system. Sets up the variables needed by this system. Usually must be changed 
   only once in a playthrough */
void ds_gamestatus_initLoad();

/* Resets the gamestatus system. Must be done only once, when exiting a playthrough */
void ds_gamestatus_reset();

/* Finishes the gamestatus system.*/
void ds_gamestatus_finish();

/* Gets the status */
int ds_gamestatus_getStatus();

/* Forces a status */
int ds_gamestatus_forceStatus(int sta);

/* Gets the status Screen */
int ds_gamestatus_getActualStatusScreen();

/* Sets the status Screen */
void ds_gamestatus_setActualStatusScreen(int stat);

/* Inits the gamestatus system. Loads a certain status screen (-1 if we use the "global" status screen) */
int ds_gamestatus_launch(int stascr);

/* Updates the gamestatus system AFTER a launch */
int ds_gamestatus_launchAFTER(int stascr);

/* Updates the status screen, in a normal way */
int ds_gamestatus_updateScreen();

/* Change (Rotate) the status screen output */
void ds_gamestatus_changeMode();   

/* Change the status screen output to a special mode */
void ds_gamestatus_pushMode(int mode);

/* Leaves the special mode and goes back to the old gamestatus screen */
void ds_gamestatus_popMode();

/* Paints debug information */
void ds_gamestatus_debugOutput(u8 screen, u16 x, u16 y, char* text, int type);

/* Paints normal information */
void ds_gamestatus_normalOutput(u8 screen, u16 x, u16 y, char* text);


#endif
