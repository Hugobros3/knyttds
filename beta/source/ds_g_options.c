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
	State_Str = World that must be loaded
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

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

int options_drawDown;

char options_string[255];

int options_btnBack;

int options_btnOptimization;
int options_btnOptimizationYesNo;
int options_btnOptimizationYesNo_Text;

int options_btnOptimization7z; // I not use 7z... but for the name, it is quite OK
int options_btnOptimization7zYesNo;
int options_btnOptimization7zYesNo_Text;

int options_btnOptimizationMus;
int options_btnOptimizationMusYesNo;
int options_btnOptimizationMusYesNo_Text;


//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Button pushing)
//-------------------------------------------------------------------------------------------------

int _ds_options_goBack(void *btn) {
	ds_state_assignState(DSKNYTT_MAINMENU); // Go back!
   return 1;
}   

int _ds_options_toggleOptimization(void *btn) {
   ds_global_optimizationPreload = !ds_global_optimizationPreload;
	if (ds_global_optimizationPreload) {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumGreen.png",DS_DIR_MAIN,DS_DIR_SYS);   
		ds_button_updateLocalImage(options_btnOptimizationYesNo,ds_global_string);
		ds_button_updateText(options_btnOptimizationYesNo,options_btnOptimizationYesNo_Text,"ON");
	} else {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumErase.png",DS_DIR_MAIN,DS_DIR_SYS);   
		ds_button_updateLocalImage(options_btnOptimizationYesNo,ds_global_string);
		ds_button_updateText(options_btnOptimizationYesNo,options_btnOptimizationYesNo_Text,"OFF");
	}   
	
	options_drawDown = 1;
   
   return 1;
}   

int _ds_options_toggleOptimization7z(void *btn) {
   ds_global_optimizationUncompress = !ds_global_optimizationUncompress;
	if (ds_global_optimizationUncompress) {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumGreen.png",DS_DIR_MAIN,DS_DIR_SYS);   
		ds_button_updateLocalImage(options_btnOptimization7zYesNo,ds_global_string);
		ds_button_updateText(options_btnOptimization7zYesNo,options_btnOptimization7zYesNo_Text,"ON");
	} else {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumErase.png",DS_DIR_MAIN,DS_DIR_SYS);   
		ds_button_updateLocalImage(options_btnOptimization7zYesNo,ds_global_string);
		ds_button_updateText(options_btnOptimization7zYesNo,options_btnOptimization7zYesNo_Text,"OFF");
	}   
	
	options_drawDown = 1;
   
   return 1;
}   

int _ds_options_toggleOptimizationMus(void *btn) {
   ds_global_optimizationMusic = !ds_global_optimizationMusic;
	if (ds_global_optimizationMusic) {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumGreen.png",DS_DIR_MAIN,DS_DIR_SYS);   
		ds_button_updateLocalImage(options_btnOptimizationMusYesNo,ds_global_string);
		ds_button_updateText(options_btnOptimizationMusYesNo,options_btnOptimizationMusYesNo_Text,"ON");
	} else {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumErase.png",DS_DIR_MAIN,DS_DIR_SYS);   
		ds_button_updateLocalImage(options_btnOptimizationMusYesNo,ds_global_string);
		ds_button_updateText(options_btnOptimizationMusYesNo,options_btnOptimizationMusYesNo_Text,"OFF");
	}   
	
	options_drawDown = 1;
   
   return 1;
}   

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Management)
//-------------------------------------------------------------------------------------------------

void _ds_g_options_createButtons() {
				
	// Create "Back" button
	sprintf(ds_global_string,"%s%s/Gui_btn_small.png",DS_DIR_MAIN,DS_DIR_SYS);   
	sprintf(options_string,"%s%s/Gui_btn_small2.png",DS_DIR_MAIN,DS_DIR_SYS);   
	options_btnBack = ds_button_create(198,158,48,12,0,ds_global_string);
	ds_button_addClick(options_btnBack,6,options_string);
	ds_button_addText(options_btnBack,-1,0,"Back",PA_RGB(0,0,0),1,1);
	ds_button_addManagement(options_btnBack,_ds_options_goBack,0);
	
	// Create "Optimizations" buttons
	sprintf(ds_global_string,"%s%s/Gui_btn_medium.png",DS_DIR_MAIN,DS_DIR_SYS);   
	
	options_btnOptimization7z = ds_button_create(24,32,96,16,0,ds_global_string);
	ds_button_addText(options_btnOptimization7z,-1,0,"Uncompress maps?",PA_RGB(0,0,0),1,1);
	ds_button_addManagement(options_btnOptimization7z,_ds_options_toggleOptimization7z,0);

	options_btnOptimizationMus = ds_button_create(24,64,96,16,0,ds_global_string);
	ds_button_addText(options_btnOptimizationMus,-1,0,"In-Game Music?",PA_RGB(0,0,0),1,1);
	ds_button_addManagement(options_btnOptimizationMus,_ds_options_toggleOptimizationMus,0);

/*	options_btnOptimization = ds_button_create(24,96,96,16,0,ds_global_string);
	ds_button_addText(options_btnOptimization,-1,0,"Preload maps?",PA_RGB(0,0,0),1,1);
	ds_button_addManagement(options_btnOptimization,_ds_options_toggleOptimization,0);	*/

	if (ds_global_optimizationUncompress) {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumGreen.png",DS_DIR_MAIN,DS_DIR_SYS);   
		options_btnOptimization7zYesNo = ds_button_create(136,32,96,16,0,ds_global_string);
		ds_button_addManagement(options_btnOptimization7zYesNo,_ds_options_toggleOptimization7z,0);		
		options_btnOptimization7zYesNo_Text = ds_button_addText(options_btnOptimization7zYesNo,-1,0,"ON",PA_RGB(0,0,0),1,1);
	} else {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumErase.png",DS_DIR_MAIN,DS_DIR_SYS);   
		options_btnOptimization7zYesNo = ds_button_create(136,32,96,16,0,ds_global_string);
		ds_button_addManagement(options_btnOptimization7zYesNo,_ds_options_toggleOptimization7z,0);				
		options_btnOptimization7zYesNo_Text = ds_button_addText(options_btnOptimization7zYesNo,-1,0,"OFF",PA_RGB(0,0,0),1,1);	   
	}   

	if (ds_global_optimizationMusic) {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumGreen.png",DS_DIR_MAIN,DS_DIR_SYS);   
		options_btnOptimizationMusYesNo = ds_button_create(136,64,96,16,0,ds_global_string);
		ds_button_addManagement(options_btnOptimizationMusYesNo,_ds_options_toggleOptimizationMus,0);		
		options_btnOptimizationMusYesNo_Text = ds_button_addText(options_btnOptimizationMusYesNo,-1,0,"ON",PA_RGB(0,0,0),1,1);
	} else {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumErase.png",DS_DIR_MAIN,DS_DIR_SYS);   
		options_btnOptimizationMusYesNo = ds_button_create(136,64,96,16,0,ds_global_string);
		ds_button_addManagement(options_btnOptimizationMusYesNo,_ds_options_toggleOptimizationMus,0);				
		options_btnOptimizationMusYesNo_Text = ds_button_addText(options_btnOptimizationMusYesNo,-1,0,"OFF",PA_RGB(0,0,0),1,1);	   
	}   

/*	if (ds_global_optimizationPreload) {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumGreen.png",DS_DIR_MAIN,DS_DIR_SYS);   
		options_btnOptimizationYesNo = ds_button_create(136,96,96,16,0,ds_global_string);
		ds_button_addManagement(options_btnOptimizationYesNo,_ds_options_toggleOptimization,0);		
		options_btnOptimizationYesNo_Text = ds_button_addText(options_btnOptimizationYesNo,-1,0,"ON",PA_RGB(0,0,0),1,1);
	} else {
		sprintf(ds_global_string,"%s%s/Gui_btn_mediumErase.png",DS_DIR_MAIN,DS_DIR_SYS);   
		options_btnOptimizationYesNo = ds_button_create(136,96,96,16,0,ds_global_string);
		ds_button_addManagement(options_btnOptimizationYesNo,_ds_options_toggleOptimization,0);				
		options_btnOptimizationYesNo_Text = ds_button_addText(options_btnOptimizationYesNo,-1,0,"OFF",PA_RGB(0,0,0),1,1);	   
	}   */
	
}
   
//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------


/* Starts this subScreen */
void ds_g_options_start() {
   
   // Now, create the buttons and subsystems
   _ds_g_options_createButtons();

	// Tells itself that it needs to be drawn
   options_drawDown = 1;
}   

/* Finish this subScreen */
void ds_g_options_finish() {
   
	// Reset the button information
	ds_button_reset();   
}   

/* Manages the input of the user */
void ds_g_options_input() {
   int click;
   
   /* Transform the user input into variables */
   ds_input_getInputPad(&ds_global_input);

	/* Check the buttons - Newpress & Held */
	if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_STYLUS)) {
	   ds_button_checkClick(ds_global_input.stylusx,ds_global_input.stylusy,0);
	} else {
		click = (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_STYLUS));
		if (ds_button_checkClick(ds_global_input.stylusx,ds_global_input.stylusy,click)) {
		   options_drawDown = 1;
		}   	   
	}      	
}   

/* Manages the state of the game */
void ds_g_options_state() {
   
}   

/* Paints this actual game state */
void ds_g_options_paint() {
   // Wait for the VRetrace
   PA_WaitForVBL();

   // If we need to paint...(lower screen)
   if (options_drawDown) {
      ds_global_fillScreen(0,ds_global_getScreen0(),PA_RGB(31,31,31));      
	   ds_button_paint(); // Buttons are only found in the lower screen
     	ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
      
      options_drawDown = 0;
   }   
}
