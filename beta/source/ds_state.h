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

/* Game "State" - State of the game */
#ifndef _DS_STATE_
#define _DS_STATE_

//-------------------------------------------------------------------------------------------------
// CONSTANTS
//-------------------------------------------------------------------------------------------------

// Game State Constants
#define DSKNYTT_ 0
#define DSKNYTT_LOGO 1
#define DSKNYTT_MAINMENU 10
#define DSKNYTT_LEVELMENU 11
#define DSKNYTT_LOADMENU 12
#define DSKNYTT_SLIDESHOW 20
#define DSKNYTT_OPTIONS 30
#define DSKNYTT_GAME 100
#define DSKNYTT_LOADGAME 101
#define DSKNYTT_DESTROYGAME 102
#define DSKNYTT_CUTSCENE 200
#define DSKNYTT_MAP2RAW 300

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Inits the state system */
void ds_state_init();

/* Get the actual state of the Game */
int ds_state_getState();

/* Get the next state of the Game. Mostly for debug purposes */
int ds_state_getNextState();

/* Assign the actual state of the game, but does not change it */
void ds_state_assignState(int state);

/* Returns 1 if the state has changed. 0 otherwise */
int ds_state_newState();

/* Updates the state for the new, previously assigned state */
void ds_state_updateState();

/* Assigns var. - String */
void ds_state_var_setStr(char *str);

/* Gets var. - String */
char *ds_state_var_getStr();

/* Assigns var. - Int */
void ds_state_var_setInt(int who, int val);

/* Gets var. - Int */
int ds_state_var_getInt(int who);

/* Resets Array - Arr. Int */
void ds_state_var_resetArrInt();

/* Assigns var. - Arr. Int */
void ds_state_var_addArrInt(int val);   

/* Gets var. - lenght Arr. Int */
int ds_state_var_lenArrInt();

/* Gets var. - Arr. Int */
int ds_state_var_getArrInt(int pos);

/* Test for a val - Arr. Int */
int ds_state_var_inArrInt(int val);


#endif
