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
	State_Str = Cutscene name.
	State[0] = Cutscene number. Non-existing number will advance to the next state. Starts @ 1
	State[1] = Cutscene type. See constants @ ds_global
	State[2] = Sound to play when going back to the game, if applicable
*/

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_state.h"
#include "ds_world.h"
#include "ds_util.h"
#include "ds_gamestatus.h"
#include "ds_button.h"
#include "ds_input.h"
#include "ds_linkedlist.h"
#include "ds_15bpp.h"
#include "ds_music.h"
#include "ds_ini.h"
#include "ds_3dsprite.h"

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

int cutscene_drawDown;
int cutscene_drawUp;
char cutscene_string[255];
char cutscene_string2[255];

int cutscene_btnBack;
int cutscene_btnNext;
int cutscene_btnImaPreview;

int cutscene_specialDSMode;
int cutscene_normalMode;
int cutscene_600x240_x;
int cutscene_600x240_y;
ds_t_15bpp cutscene_600x240_bigImage;
ds_t_15bpp cutscene_600x240_smallImageNOFREEME; // A weird remind that this image MUST NOT BE FREED
												// WHY???? - It will be inside a button, who will free it ;-)
ds_t_15bpp cutscene_NDS_upImage;
ds_t_15bpp cutscene_NDS_downImage;

int cutscene_music;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Button pushing)
//-------------------------------------------------------------------------------------------------

int _ds_cutscene_previous(void *btn) {
   if (ds_state_var_getInt(0) > 1) {
   	ds_state_assignState(DSKNYTT_CUTSCENE);
	   ds_state_var_setInt(0,ds_state_var_getInt(0)-1);
	}   
   
   return 1;
}   

int _ds_cutscene_next(void *btn) {
   ds_state_assignState(DSKNYTT_CUTSCENE);
   ds_state_var_setInt(0,ds_state_var_getInt(0)+1);
   
   return 1;
}   

int _ds_cutscene_showBigMap(void *btn) { // Remember... 600x240-256x102 (256x192)
   int x = ds_button_getLocalX_p(btn,ds_global_input.stylusx);
   int y = ds_button_getLocalY_p(btn,ds_global_input.stylusy);
   int realx = (x * ((600 * 100) / 256)) / 100;
   int realy = (y * ((240 * 100) / 102)) / 100;
	
	if (realx < (256 >> 1)) 
		cutscene_600x240_x = 0;
	else if (realx >= (600 - (256 >> 1))) 
		cutscene_600x240_x = 600 - 256;
	else cutscene_600x240_x = realx - (256 >> 1);
	
	if (realy < (192 >> 1)) 
		cutscene_600x240_y = 0;
	else if (realy >= (240 - (192 >> 1))) 
		cutscene_600x240_y = 240 - 192;
	else cutscene_600x240_y = realy - (192 >> 1);

	cutscene_drawUp = 1;   
	
   return 1;
}

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Management)
//-------------------------------------------------------------------------------------------------

void _ds_g_cutscene_changeToGame() {
   int flag = 0;
	int sound = ds_state_var_getInt(2);

	// Stops all music
	ds_music_playOnlyMusic(0);
      
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

void _ds_g_cutscene_change() {
   switch (ds_state_var_getInt(1)) {
      case DS_C_CUTSCENE_INTRO:
         // Teleports Juni back to the game
         _ds_g_cutscene_changeToGame();
         break;
      case DS_C_CUTSCENE_GAME:
         // Teleports Juni back to the game - Executes some stuff, too
         _ds_g_cutscene_changeToGame();
         break;
      case DS_C_CUTSCENE_END:
         // Go back to the main menu - but before that, DESTROYYYYY!!!!!!!!!
         ds_state_assignState(DSKNYTT_DESTROYGAME);
         break;         
   }   
}   

int _ds_g_cutscene_checkDSMode() {
   sprintf(cutscene_string,"%s/%s/%s%d%s",ds_global_world.dir,ds_state_var_getStr(),
				"Scene",ds_state_var_getInt(0),"A.png");
	return ds_util_fileExists(cutscene_string);   
}   

int _ds_g_cutscene_checkNormalMode() {
   sprintf(cutscene_string,"%s/%s/%s%d%s",ds_global_world.dir,ds_state_var_getStr(),
				"Scene",ds_state_var_getInt(0),".png");
	return ds_util_fileExists(cutscene_string);   
}   

void _ds_g_cutscene_loadImages() {
	if (cutscene_specialDSMode) {
	   // We show the 2 special screens, specially crafted for the NDS!!!!!
	   sprintf(cutscene_string,"%s/%s/%s%d%s",ds_global_world.dir,ds_state_var_getStr(),
					"Scene",ds_state_var_getInt(0),"A.png");
	   ds_15bpp_load(cutscene_string,&cutscene_NDS_upImage,1,0);
	   sprintf(cutscene_string,"%s/%s/%s%d%s",ds_global_world.dir,ds_state_var_getStr(),
					"Scene",ds_state_var_getInt(0),"B.png");
	   ds_15bpp_load(cutscene_string,&cutscene_NDS_downImage,1,0);	   
	} else {
	   // Aiyaaaaa.... we need to keep the 600x240 screen
	   sprintf(cutscene_string,"%s/%s/%s%d%s",ds_global_world.dir,ds_state_var_getStr(),
					"Scene",ds_state_var_getInt(0),".png");
		ds_15bpp_load(cutscene_string,&cutscene_600x240_bigImage,1,0);
	   ds_15bpp_resize(&cutscene_600x240_smallImageNOFREEME,&cutscene_600x240_bigImage,256,102);
	}     
}   

void _ds_g_cutscene_paintImages() {
	if (cutscene_specialDSMode) {
	   if (cutscene_drawUp)
			ds_15bpp_putScreen(ds_global_getScreen1(),&cutscene_NDS_upImage,0,0,1);
		if (cutscene_drawDown)
			ds_15bpp_putScreen(ds_global_getScreen0(),&cutscene_NDS_downImage,0,0,1);
	} else {
	   if (cutscene_drawUp)
	   	ds_15bpp_putScreenPartial(ds_global_getScreen1(),&cutscene_600x240_bigImage,0,0,cutscene_600x240_x,cutscene_600x240_y,256,192);
		if (cutscene_drawDown)
	      ds_global_fillScreen(0,ds_global_getScreen0(),PA_RGB(31,31,31));
	}     
}   

void _ds_g_cutscene_createButtons() {

	// Create "Image preview" button
	if (!cutscene_specialDSMode) {
	   cutscene_btnImaPreview = ds_button_create(0,16,256,102,0,"");
	   ds_button_updateLocalImageForce(cutscene_btnImaPreview,&cutscene_600x240_smallImageNOFREEME);
	   ds_button_addManagement(cutscene_btnImaPreview,_ds_cutscene_showBigMap,1);
	   	// Since the image is stored inside a button (which frees the image when destroyed), 
			//  that's the reason it should not be freed.
	}   
				
	// Create "Back"/"Next" buttons ("Back" only if I can go back)
	if (ds_state_var_getInt(0) > 1) {
		sprintf(cutscene_string,"%s%s/Gui_btn_back.png",DS_DIR_MAIN,DS_DIR_SYS);   
		sprintf(cutscene_string2,"%s%s/Gui_btn_back2.png",DS_DIR_MAIN,DS_DIR_SYS);   
		cutscene_btnBack = ds_button_create(10,158,48,12,0,cutscene_string);
		ds_button_addClick(cutscene_btnBack,6,cutscene_string2);
		ds_button_addManagement(cutscene_btnBack,_ds_cutscene_previous,0);
		ds_button_addSound(cutscene_btnBack, "Tiny Hit");
	}	
	sprintf(cutscene_string,"%s%s/Gui_btn_forth.png",DS_DIR_MAIN,DS_DIR_SYS);   
	sprintf(cutscene_string2,"%s%s/Gui_btn_forth2.png",DS_DIR_MAIN,DS_DIR_SYS);   
	cutscene_btnNext = ds_button_create(198,158,48,12,0,cutscene_string);	
	ds_button_addClick(cutscene_btnNext,6,cutscene_string2);
	ds_button_addManagement(cutscene_btnNext,_ds_cutscene_next,0);
	ds_button_addSound(cutscene_btnNext, "Tiny Hit");
}


   
//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------


/* Starts this subScreen */
void ds_g_cutscene_start() {
   // Init variables :-)   
	cutscene_600x240_x = 0;
	cutscene_600x240_y = 0;
	ds_15bpp_init(&cutscene_600x240_bigImage);
	ds_15bpp_init(&cutscene_600x240_smallImageNOFREEME);
	ds_15bpp_init(&cutscene_NDS_upImage);
	ds_15bpp_init(&cutscene_NDS_downImage);
	cutscene_specialDSMode = _ds_g_cutscene_checkDSMode();
	cutscene_normalMode = _ds_g_cutscene_checkNormalMode();
	
   // BEFORE Anything... check whether I should change to another state!!!!
   //PA_OutputText(1,0,16,"                                        ");
   if ((!cutscene_specialDSMode) && (!cutscene_normalMode)) {
      _ds_g_cutscene_change();
   } else {      
	   // Load image system (Preview OR Normal)
   	_ds_g_cutscene_loadImages();
   
	   // Now, create the buttons and subsystems
   	_ds_g_cutscene_createButtons();

		// Tells itself that it needs to be drawn
   	cutscene_drawUp = 1;
	   cutscene_drawDown = 1;
	   
		// <HACK> "Cleans" the 3D sprite system, if there are sprites floating around :-)
		ds_3dsprite_cleanHWSprites();
		
		// Music! Cutscenes have their own music ;-)
		sprintf(cutscene_string,"Cutscene Music:%s",ds_state_var_getStr());
		sprintf(ds_global_string,ds_ini_getstring(ds_global_world.worldini,cutscene_string,"-1"));
		if (!PA_CompareText(ds_global_string,"-1")) {
			// There is something...
			cutscene_music = (int)strtol(ds_global_string, NULL, -1);
			if (cutscene_music != -1) {
				// Normal
				ds_music_playOnlyMusic(cutscene_music);
			} else {
				// Maybe Ambiance...
				ds_global_string[strlen(ds_global_string) - 1] = '\0';
				cutscene_music = (int)strtol(ds_global_string, NULL, -1);
				if (cutscene_music != -1) {
					// Ambiance!
					ds_music_playOnlyAmbiance(cutscene_music);
				}
			}
		}
	}   
	
	// Final touch... eliminates fade to white
	ds_global_fadeWhitePop();
}   

/* Finish this subScreen */
void ds_g_cutscene_finish() {
   // First, delete variables
   if (cutscene_specialDSMode) {
      ds_15bpp_delete(&cutscene_NDS_upImage);      
      ds_15bpp_delete(&cutscene_NDS_downImage);
   } else if (cutscene_normalMode) {
      ds_15bpp_delete(&cutscene_600x240_bigImage);      
	}      
   
	// Now, reset the button information
	ds_button_reset();
}   

/* Manages the input of the user */
void ds_g_cutscene_input() {
   int click;
   
   /* Transform the user input into variables */
   ds_input_getInputPad(&ds_global_input);

	/* Check the buttons - Newpress & Held */
	if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_STYLUS)) {
	   ds_button_checkClick(ds_global_input.stylusx,ds_global_input.stylusy,0);
	} else {
		click = (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_STYLUS));
		if (ds_button_checkClick(ds_global_input.stylusx,ds_global_input.stylusy,click)) {
		   cutscene_drawDown = 1;
		}   	   
	}      	
}   

/* Manages the state of the game */
void ds_g_cutscene_state() {
   
}   

/* Paints this actual game state */
void ds_g_cutscene_paint() {
   // Wait for the VRetrace
   PA_WaitForVBL();
   
   // If we need to paint...(lower screen)
   if ((cutscene_drawDown) || (cutscene_drawUp)) {
      _ds_g_cutscene_paintImages();
      if (cutscene_drawDown)
	      ds_button_paint(); // Buttons are only found in the lower screen
			   
      if (cutscene_drawUp) 
	      ds_global_paintScreen(1,ds_global_getScreen1(),0,0);      
	   if (cutscene_drawDown)
      	ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
      
      cutscene_drawDown = 0;
      cutscene_drawUp = 0;
   }
}
