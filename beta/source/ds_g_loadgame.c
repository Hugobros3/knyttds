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

/*
	State_Str = IF Loading First Game (Reset-[1]) World that must be loaded
					IF not Loading First Game, Cutscene that must be played
	State[0] = Type of LOADGAME, see constants @ds_global
	State[1] = Reset (1 - load again the world information, including certain subsystems)
	State[2] = Save Slot. -1 if no save slot
	State[3] = Sound to use
*/

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_state.h"
#include "ds_world.h"
#include "ds_util.h"
#include "ds_gamestatus.h"

/* Starts this subScreen */
void ds_g_loadgame_start() {
   // First, fades to white
	ds_global_fadeWhitePush(5);

	// Now, loads the world and initializes the game status
   if (ds_state_var_getInt(1)) {
      ds_gamestatus_initLoad();
	   if (!ds_world_load(ds_state_var_getStr(),ds_state_var_getInt(2)))
	   	ds_global_errorHalt("ds_world_load");
	}   
}   

/* Finish this subScreen */
void ds_g_loadgame_finish() {
   // We had no variables here...
}   

/* Manages the input of the user */
void ds_g_loadgame_input() {
   // Nothing to do...
}   

void _ds_g_loadgame_changeToGame() {
   int flag = 0;
	int sound = ds_state_var_getInt(3);
      
   // Changes Juni
   ds_global_juni.flag = ds_global_world.sv_flags;
   ds_global_juni.item = ds_global_world.sv_items;
   
   // Set Parameters
  	ds_state_var_setInt(0,ds_global_world.sv_xmap);
	ds_state_var_setInt(1,ds_global_world.sv_ymap);

	ds_state_var_setInt(2,ds_global_world.sv_xjuni * 24); // This is in absolute position
	ds_state_var_setInt(3,ds_global_world.sv_yjuni * 24); // This is in absolute position
	flag = ds_util_bitSet16(flag,1); // Only reset movement, nothing else
	ds_state_var_setInt(4,flag);
	ds_state_var_setInt(5,ds_global_world.sv_gui);
	ds_state_var_setInt(6,sound);
	// Change!
	ds_state_assignState(DSKNYTT_GAME);
}   

/* Manages the state of the game */
void ds_g_loadgame_state() {   
   // Change to the real game
	int sound = ds_state_var_getInt(3);
   switch (ds_state_var_getInt(0)) {
      case DS_C_LOADGAME_INTRO:
         ds_state_assignState(DSKNYTT_CUTSCENE);
         ds_state_var_setStr("Intro");
         ds_state_var_setInt(0,1); // Cutscene number 1
         ds_state_var_setInt(1,DS_C_CUTSCENE_INTRO); // Type INTRO
			ds_state_var_setInt(2,sound); // Sound to play after break
         break;      
      case DS_C_LOADGAME_CUTSCENE:
         ds_state_assignState(DSKNYTT_CUTSCENE);
         ds_state_var_setStr(ds_state_var_getStr());
         ds_state_var_setInt(0,1); // Cutscene number 1
         ds_state_var_setInt(1,DS_C_CUTSCENE_GAME); // Type GAME
			ds_state_var_setInt(2,sound); // Sound to play after break
         break;      
      case DS_C_LOADGAME_GAME:
         _ds_g_loadgame_changeToGame();
         break;
   }   
}   

/* Paints this actual game state */
void ds_g_loadgame_paint() {
   // Nothing to do...
}   
