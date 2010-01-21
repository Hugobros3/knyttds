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
	No need to use the state variables... :-)
*/

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_state.h"
#include "ds_world.h"
#include "ds_util.h"
#include "ds_gamestatus.h"
#include "ds_button.h"
#include "ds_15bpp.h"
#include "ds_input.h"
#include "ds_music.h"
#include "ds_3dsprite.h"

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

int btn_play;
int btn_tutorial;
int btn_getmorelevels;
int btn_options;
int btn_credits;
ds_t_15bpp ima_upscreen;

int mainmenu_draw;
int lowmenu_draw;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Management)
//-------------------------------------------------------------------------------------------------

int _ds_mainmenu_launchTutorial(void *btn) {

	ds_state_var_setStr("Nifflas & rrc2soft - Tutorial DS");
	ds_state_var_setInt(0,DS_C_LOADGAME_GAME); // No cutscenes for the tutorial ;-)
	ds_state_var_setInt(1,1);
	ds_state_var_setInt(2,-1);	
	ds_state_assignState(DSKNYTT_LOADGAME);
	
	// No music from the intro in the game... it will stop itself ^_-
	// ds_music_stopAllMusic();

   return 1;
}   

int _ds_mainmenu_launchLevelMenu(void *btn) {
	ds_state_assignState(DSKNYTT_LEVELMENU);
	
   return 1;
}   

int _ds_mainmenu_launchMoreLevels(void *btn) {
	ds_state_assignState(DSKNYTT_SLIDESHOW);
	ds_state_var_setStr("SH-MoreLevels");
	ds_state_var_setInt(0,1);
	ds_state_var_setInt(1,DSKNYTT_MAINMENU);
	ds_state_var_setInt(2,1);	
			
   return 1;
   
}   

int _ds_mainmenu_launchCredits(void *btn) {
	ds_state_assignState(DSKNYTT_SLIDESHOW);
	ds_state_var_setStr("SH-Credits");
	ds_state_var_setInt(0,1);
	ds_state_var_setInt(1,DSKNYTT_MAINMENU);
	ds_state_var_setInt(2,0);		
		
   return 1;
   
}   

int _ds_mainmenu_launchOptions(void *btn) {
	ds_state_assignState(DSKNYTT_OPTIONS);
		
   return 1;
   
}   

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------


/* Starts this subScreen */
void ds_g_mainmenu_start() {
	char filen[255];
	char filen2[255];
	
	// Paint the upper screen (cute!)
	sprintf(filen,"%s%s/Gui_screen_ini_up.png",DS_DIR_MAIN,DS_DIR_SYS);
	ds_15bpp_load(filen, &ima_upscreen, 1,0);
	
   // Start the game buttons (Bottom Screen)
	sprintf(filen,"%s%s/Gui_btn_medium.png",DS_DIR_MAIN,DS_DIR_SYS); 
	sprintf(filen2,"%s%s/Gui_btn_medium2.png",DS_DIR_MAIN,DS_DIR_SYS);     
	
	int btn_play = ds_button_create(80,32,96,16,0,filen);
	ds_button_addClick(btn_play, 6, filen2);
	ds_button_addText(btn_play,4,2,"Play",PA_RGB(0,0,0),2,1);
	ds_button_addManagement(btn_play,_ds_mainmenu_launchLevelMenu,0);
	int btn_tutorial = ds_button_create(80,60,96,16,0,filen);
	ds_button_addClick(btn_tutorial, 6, filen2);
	ds_button_addText(btn_tutorial,4,2,"Tutorial",PA_RGB(0,0,0),2,1);
	ds_button_addManagement(btn_tutorial,_ds_mainmenu_launchTutorial,0);
	int btn_getmorelevels = ds_button_create(80,88,96,16,0,filen);
	ds_button_addClick(btn_getmorelevels, 6, filen2);
	ds_button_addText(btn_getmorelevels,4,2,"Get More Levels",PA_RGB(0,0,0),2,1);
	ds_button_addManagement(btn_getmorelevels,_ds_mainmenu_launchMoreLevels,0);	
	int btn_options = ds_button_create(80,116,96,16,0,filen);
	ds_button_addClick(btn_options, 6, filen2);
	ds_button_addText(btn_options,4,2,"Options",PA_RGB(0,0,0),2,1);	
	ds_button_addManagement(btn_options,_ds_mainmenu_launchOptions,0);		
	int btn_credits = ds_button_create(80,144,96,16,0,filen);
	ds_button_addClick(btn_credits, 6, filen2);
	ds_button_addText(btn_credits,4,2,"Credits",PA_RGB(0,0,0),2,1);	
	ds_button_addManagement(btn_credits,_ds_mainmenu_launchCredits,0);		

	
	// Tells itself that it needs to be drawn
	mainmenu_draw = 1;
	lowmenu_draw = 0;
	
	// <HACK> "Cleans" the 3D sprite system, if there are sprites floating around :-)
	ds_3dsprite_cleanHWSprites();
	
	// Tell the Music subsystem to start playing the "knytt stories" tune   
	// Due to how the system works, it will play it only once ^_-
	ds_music_playOnlyMusic(20); // FIXME: 20 is the predefined knytt tune
}

/* Finish this subScreen */
void ds_g_mainmenu_finish() {   
   // Finish local variables
   ds_15bpp_delete(&ima_upscreen);
   
	// First, reset the button information
	ds_button_reset();   
	
	// Now, change screens
   //int flag = 0;
   //switch (ds_state_getNextState()) {
   //   case DSKNYTT_:
   //      break;
   //}   
}   

/* Manages the input of the user */
void ds_g_mainmenu_input() {
   int click;
   
   /* Transform the user input into variables */
   ds_input_getInputPad(&ds_global_input);

	/* Check the buttons */
	click = (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_STYLUS));
	if (ds_button_checkClick(ds_global_input.stylusx,ds_global_input.stylusy,click)) {
	   lowmenu_draw = 1;
	}   
}   

/* Manages the state of the game */
void ds_g_mainmenu_state() {
   
}   

/* Paints this actual game state */
void ds_g_mainmenu_paint() {
   // Wait for the VRetrace
   PA_WaitForVBL();
   
   // If we need to paint...(upper screen)
   if (mainmenu_draw) {
      // Up
		ds_15bpp_putScreen(ds_global_getScreen1(),&ima_upscreen,0,0,1);
		ds_global_paintScreen(1,ds_global_getScreen1(),0,0);   
   }
   // If we need to paint...(lower screen)
   if ((mainmenu_draw) || (lowmenu_draw)) {
      if (lowmenu_draw)
			lowmenu_draw = 0;
      // Down
      ds_global_fillScreen(0,ds_global_getScreen0(),PA_RGB(31,31,31));      
      ds_button_paint();
      ds_global_paintScreen(0,ds_global_getScreen0(),0,0);      
	}	
	// Fade issues
   if (mainmenu_draw) {
		mainmenu_draw = 0;
		// No fade! (If I came from starting the game...)
		ds_global_fadeWhitePop();
	}	
}
