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
DEBUG: 1001-1000, 24*4,24 ... tutorial
*/

/*
	State[0] = X-coordinate of the map that must be loaded
	State[1] = Y-coordinate of the map that must be loaded
	State[2] = X-coordinate of Juni (ABS)
	State[3] = Y-coordinate of Juni (ABS)
	<TODO> State[4] = Flags
					[0]-CENTER: 1, centers Juni in the destination coordinate
					[1]-RESETM: 1, resets Juni's movements
					[2]-RESETI: 1, resets Juni's internal info (e.g. items, flags)
	State[5] = GUI
	State[6] = Sound
*/

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_state.h"
#include "ds_map.h"
#include "ds_submap.h"
#include "ds_input.h"
#include "ds_objects.h"
#include "ds_3dsprite.h"
#include "ds_camera.h"
#include "ds_juni.h"
#include "ds_ini.h"
#include "ds_util.h"
#include "ds_15bpp.h"
#include "ds_dialogue.h"
#include "ds_world.h"
#include "ds_gamestatus.h"
#include "ds_music.h"

/* Changes the coordinates of this room according to the flags */
int _ds_g_game_checkflags(int type) {
   int i;
   int x = ds_state_var_getInt(0);
   int y = ds_state_var_getInt(1);
   int newx;
   int newy;
   
   // Get the condition
   i = (ds_ini_getint(ds_global_world.worldini,
		   ds_ini_keyDictionary(x,y,DS_C_DICT_FLAG,type,0,0,0),
   		-1));
   if (i == -1)
   	return 0; // Flag(type) doesn't exists
   	
   // Test the Condition
   if (ds_util_bitOne16(ds_global_juni.flag,i)) {
      // Apply Changes
      newx = x + (ds_ini_getint(ds_global_world.worldini,
		   				ds_ini_keyDictionary(x,y,DS_C_DICT_FLAGWARPX,type,0,0,0),
   					0));
      newy = y + (ds_ini_getint(ds_global_world.worldini,
		   				ds_ini_keyDictionary(x,y,DS_C_DICT_FLAGWARPY,type,0,0,0),
   					0));
   	ds_state_var_setInt(0,newx);
   	ds_state_var_setInt(1,newy);
   	
   	return 1; // CONDITION IS TRUE
   }   

	return 0; // Flag(type) is false
}    

/* Starts this subScreen */
void ds_g_game_start() {   
#ifdef DEBUG_KSDS
   char tempstr[255];
   char tempstr_1[255];
#endif
      
   /* BEFORE ANYTHING... I have to check the Flags of this room, and modify state[0][1] accordingly */
   if (!_ds_g_game_checkflags(DS_C_ELETYPE_A))
   	if (!_ds_g_game_checkflags(DS_C_ELETYPE_B))
   		_ds_g_game_checkflags(DS_C_ELETYPE_C);
	   
   /* Load the map and its associated information from the file subsystem */
#ifdef DEBUG_KSDS
	sprintf(tempstr_1,"INIT: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
#endif
   if (!ds_map_loadHDD(ds_state_var_getInt(0),ds_state_var_getInt(1))) {
      ds_global_errorHalt("ds_map_loadHDD");
   }  
#ifdef DEBUG_KSDS
	sprintf(tempstr_1,"loadHDD: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
#endif
		
   /* Paints the basic map */
   if (!ds_map_loadHDDPaint(ds_state_var_getInt(0),ds_state_var_getInt(1))) {
      ds_global_errorHalt("ds_map_loadHDD");
   }
#ifdef DEBUG_KSDS
	sprintf(tempstr_1,"loadHDDPaint: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
#endif
	
	/* Map loaded? I visited this map! Mark it */
	ds_world_setVisitedRoom(ds_state_var_getInt(0),ds_state_var_getInt(1));   

   /* Initialize some optimization subsystems */
   ds_submap_initOpt();
   
   /* Post-stuff after loading screen */
   ds_3dsprite_initAfterGameScreen();
   	   
   /* Load the objects from the world */
   ds_objects_loadHDD();
#ifdef DEBUG_KSDS
	sprintf(tempstr_1,"loadObjects: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
#endif
         
   /* Make the necessary changes to Juni */
   ds_juni_init(ds_state_var_getInt(2),ds_state_var_getInt(3),
						ds_util_bitOne16(ds_state_var_getInt(4),1),
						ds_util_bitOne16(ds_state_var_getInt(4),2));
#ifdef DEBUG_KSDS
	sprintf(tempstr_1,"IniJuni: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
#endif
	
	/* Dialog system too! */
	ds_dialogue_init();
#ifdef DEBUG_KSDS
	sprintf(tempstr_1,"IniDialogue: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
#endif
   
   /* Put the camera ;-) - Only necessary to tell the system the type of camera
		-> Why? It will update itself in the next iteration :-D */
   ds_camera_setType(DS_C_CAM_JUNI,ds_state_var_getInt(2),ds_state_var_getInt(3));
   
   /* After we have loaded everything (expect music), prepare some things on the map */
   ds_map_loadHDDPOST();
#ifdef DEBUG_KSDS
	sprintf(tempstr_1,"loadHDDPOST: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
#endif

   /* Launches the upper screen system :-D */
   //ds_gamestatus_setActualStatusScreen(ds_state_var_getInt(5));
   int statGUI = ds_ini_getint(ds_global_world.worldini,
								ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_GUI,0,0,0,0),
								ds_state_var_getInt(5));
	statGUI = (statGUI <= 0)?-1:statGUI; // Correction - 0 and ...-1 are the "global" gui
   if (!ds_gamestatus_launch(statGUI)) {
      ds_global_errorHalt("ds_gamestatus_launch");
   } 
#ifdef DEBUG_KSDS
	sprintf(tempstr_1,"IniGUI: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
#endif
               
   /* Launch some music! (or not ^_-) 
		Why it is here? - if no memory, then is OK :-P */
   ds_music_playMusicAndAmbiance(ds_global_map.room.music,
				ds_global_map.room.atmosA,ds_global_map.room.atmosB,
				0); // Both mp3 and raw can be played here
#ifdef DEBUG_KSDS
	sprintf(tempstr_1,"final: %ld\n ", Tick(ds_global_timer));
	strcat(tempstr,tempstr_1);
#endif
	
   /* And... if I came from a shift, do special things */
   if (ds_global_world.shift_cameFrom) {
		/* Play some sounds ;-) */
		ds_music_playSpecialSound(ds_state_var_getInt(6));
   }   
   ds_global_world.shift_cameFrom = 0;
	
	/* Paints something... */
	ds_gamestatus_launchAFTER(statGUI);
	   
   // Pops if fade to white!
	ds_global_fadeWhitePop();
}   

// Captures the screen!
void SetRegCapture(bool enable, uint8 mode, uint8 srcBlend, uint8 destBlend, uint8 bank, uint8 offset, uint8 size, uint8 source, uint8 srcOffset)
{
      uint32 value = 0; 
 		value |= enable << 31;
      value |= (mode & 0x3) << 29;
      value |= (srcOffset & 0x3) << 26;
      value |= (source & 0x3) << 24;
      value |= (size & 0x3) << 20;
      value |= (offset & 0x3) << 18;
      value |= (bank & 0x3) << 16;
      value |= (srcBlend & 0xF) << 8;
      value |= destBlend & 0xF; 
      REG_DISPCAPCNT = value;
} 

/* Finish this subScreen */
void ds_g_game_finish() {

	// FIRST!!! - Capture the screen on bank D (subscreen)
   vramSetBankD(VRAM_D_LCD);
   SetRegCapture(true, 0, 0, 31, 3, 1, 3, 1, 0); // Capture from C
   swiWaitForVBlank(); 
   swiWaitForVBlank(); 
   swiWaitForVBlank(); 
   vramSetBankD(VRAM_D_MAIN_BG_0x06000000);   
   
   /* Deletes certain things related to the map */
   ds_map_reset(0); // Simple map reset
   
   /* Deletes the things related to the objects */
   ds_objects_reset();
   
   /* No submaps optimizations! - if they were executing */
   ds_submap_endOpt();
   
   /* Resets the state of Juni */
   ds_juni_reset();
   
   /* Dialogues are out, too */
   ds_dialogue_reset();
   
   /* Camera? No need */
   
   /* Finally, deletes all the sprites of the screen... new screen will have new sprites */
   // Remember that dialog had ONE sprite!!!!
   ds_3dsprite_reset(1);
   
   /* ...and the upper screen subsystem needs to be reset */
   ds_gamestatus_reset(); 
   
   /* Resets the music subsystem */
   ds_music_reset();
   
   // DEBUG
   // PA_DisableSpecialFx(1);
   
   // Reset the *HW* sprites... so the captured subscreen is shown ^_-
   ds_3dsprite_cleanHWSprites();
}   

/* Manages the input of the user */
void ds_g_game_input() {
   /* Simply transform the user input into variables */
   ds_input_getInputPad(&ds_global_input);
}   

/* Manages the state of the game */
void ds_g_game_state() {
   
#ifdef DEBUG_KSDS   
   sprintf(ds_global_string,"ST: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
   ds_gamestatus_debugOutput(1,0,0,ds_global_string,DS_C_STA_DEBUG2);
#endif
   
   /* Initialize certain stuff */
   ds_global_juni.redGlow = 640;  // No Glow...
   ds_global_juni.cyanGlow = 640;  // No Glow...
   ds_global_world.shift = 0;     // We already shifted if we did so, thus init this variable!
   
   /* Manages Dialogue... in fact, prepares it for the interaction with Juni */   
   ds_dialogue_manageFirst();
#ifdef DEBUG_KSDS
	sprintf(ds_global_string,"ST<Dia>: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
	ds_gamestatus_debugOutput(1,0,1,ds_global_string,DS_C_STA_DEBUG2);   
#endif
	
   /* Manage the things that are specific to the map */
   ds_map_manage();
#ifdef DEBUG_KSDS
	sprintf(ds_global_string,"ST<Map>: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
	ds_gamestatus_debugOutput(1,0,2,ds_global_string,DS_C_STA_DEBUG2);
#endif
			   
   /* Manage all the objects and entities that have an specific handler */
   ds_objects_manage();
#ifdef DEBUG_KSDS
	sprintf(ds_global_string,"ST<Obj>: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
	ds_gamestatus_debugOutput(1,0,3,ds_global_string,DS_C_STA_DEBUG2);
#endif

   /* Manages Juni */   
   ds_juni_manage();
#ifdef DEBUG_KSDS
	sprintf(ds_global_string,"ST<Juni>: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
	ds_gamestatus_debugOutput(1,0,4,ds_global_string,DS_C_STA_DEBUG2);
#endif
}   

/* Paints this actual game state */
void ds_g_game_paint() {
#ifdef DEBUG_KSDS
	sprintf(ds_global_string,"PA: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
	ds_gamestatus_debugOutput(1,0,5,ds_global_string,DS_C_STA_DEBUG2);
#endif
	
   /* First, updates the system */
   ds_camera_update(ds_3dsprite_getX(ds_global_juni.sprite),ds_3dsprite_getY(ds_global_juni.sprite));
	
#ifdef DEBUG_KSDS
	sprintf(ds_global_string,"PA<Cam>: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
	ds_gamestatus_debugOutput(1,0,6,ds_global_string,DS_C_STA_DEBUG2);
#endif
		
   /* Paints the dialogue */
   ds_dialogue_paint();   
#ifdef DEBUG_KSDS
  	sprintf(ds_global_string,"PA<Dia>: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
	ds_gamestatus_debugOutput(1,0,7,ds_global_string,DS_C_STA_DEBUG2);
#endif
		
   /* Paints the main screen - sprites */
   ds_3dsprite_drawAll(ds_camera_getX(),ds_camera_getY());
#ifdef DEBUG_KSDS
  	sprintf(ds_global_string,"PA<Post3D>: %ld (%d)      ", Tick(ds_global_timer),PA_GetVcount());
	ds_gamestatus_debugOutput(1,0,8,ds_global_string,DS_C_STA_DEBUG2);
#endif
}
