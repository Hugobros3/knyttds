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
	No need to pass parameters :-)
*/

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_state.h"
#include "ds_world.h"
#include "ds_map.h"
#include "ds_util.h"
#include "ds_gamestatus.h"
#include "ds_music.h"

/* Starts this subScreen */
void ds_g_destroygame_start() {
	// Nothing to do...
}   

/* Finish this subScreen */
void ds_g_destroygame_finish() {
   // We had no variables here...
}   

/* Manages the input of the user */
void ds_g_destroygame_input() {
   // Nothing to do...
}   

/* Manages the state of the game */
void ds_g_destroygame_state() {
	// HERE! Here we destroy the game structures! :-D
	ds_world_destroy(); // World destroy!
	ds_map_reset(1); // Map total reset!
							// Objects? Reseted @ game finish
							// Juni? Reseted @ game finish
	
	// We also destroy any music that is hanging around!!!!!
	// ds_music_stopAllMusic();
	
   // Resets Debug! && Upper screen!
   if ((ds_gamestatus_getStatus() == DS_C_STA_DEBUG) || (ds_gamestatus_getStatus() == DS_C_STA_DEBUG2)) {
      PA_ClearTextBg(1);
      ds_gamestatus_forceStatus(DS_C_STA_RADAR);
   }   
   ds_gamestatus_finish();
	
	// Main menu! ^^
	ds_state_assignState(DSKNYTT_MAINMENU);
}   

/* Paints this actual game state */
void ds_g_destroygame_paint() {
   // Nothing to do...
}   
