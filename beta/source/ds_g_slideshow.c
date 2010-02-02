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
	State_Str = Slideshow name.
	State[0] = Slideshow number. Non-existing number will advance to the next state. Starts @ 1
	State[1] = State to return when the slideshow is finished
	State[2] = (1) "Single" slideshow type - one back button, (0) "Normal" slideshow type
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
#include "ds_3dsprite.h"

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

int slideshow_drawDown;
int slideshow_drawUp;

int slideshow_btnBack;
int slideshow_btnNext;
int slideshow_btnImaPreview;

int slideshow_specialDSMode;
int slideshow_normalMode;
int slideshow_600x240_x;
int slideshow_600x240_y;
ds_t_15bpp slideshow_600x240_bigImage;
ds_t_15bpp slideshow_600x240_smallImageNOFREEME; // A weird remind that this image MUST NOT BE FREED
ds_t_15bpp slideshow_NDS_upImage;
ds_t_15bpp slideshow_NDS_downImage;

char slideshow_string[255];

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Button pushing)
//-------------------------------------------------------------------------------------------------

int _ds_slideshow_previous(void *btn) {
   // Always go back!!!
   ds_state_assignState(ds_state_var_getInt(1));

   return 1;
}   

int _ds_slideshow_next(void *btn) {
   ds_state_assignState(DSKNYTT_SLIDESHOW);
   ds_state_var_setInt(0,ds_state_var_getInt(0)+1);
   
   return 1;
}   

int _ds_slideshow_showBigMap(void *btn) { // Remember... 600x240-256x102 (256x192)
   int x = ds_button_getLocalX_p(btn,ds_global_input.stylusx);
   int y = ds_button_getLocalY_p(btn,ds_global_input.stylusy);
   int realx = (x * ((600 * 100) / 256)) / 100;
   int realy = (y * ((240 * 100) / 102)) / 100;
	
	if (realx < (256 >> 1)) 
		slideshow_600x240_x = 0;
	else if (realx >= (600 - (256 >> 1))) 
		slideshow_600x240_x = 600 - 256;
	else slideshow_600x240_x = realx - (256 >> 1);
	
	if (realy < (192 >> 1)) 
		slideshow_600x240_y = 0;
	else if (realy >= (240 - (192 >> 1))) 
		slideshow_600x240_y = 240 - 192;
	else slideshow_600x240_y = realy - (192 >> 1);

	slideshow_drawUp = 1;   
	
   return 1;
}

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Management)
//-------------------------------------------------------------------------------------------------

void _ds_g_slideshow_change() {
   ds_state_assignState(ds_state_var_getInt(1));
}   

int _ds_g_slideshow_checkDSMode() {
   int iA;
   int iB;
   sprintf(ds_global_string,"%s%s/%s %d A.png",DS_DIR_MAIN,DS_DIR_SYS,ds_state_var_getStr(),
				ds_state_var_getInt(0));
	iA = ds_util_fileExists(ds_global_string);
   sprintf(ds_global_string,"%s%s/%s %d B.png",DS_DIR_MAIN,DS_DIR_SYS,ds_state_var_getStr(),
				ds_state_var_getInt(0));
	iB = ds_util_fileExists(ds_global_string);	
	return (iA || iB);
}   

int _ds_g_slideshow_checkNormalMode() {
   sprintf(ds_global_string,"%s%s/%s %d.png",DS_DIR_MAIN,DS_DIR_SYS,ds_state_var_getStr(),
				ds_state_var_getInt(0));
	return ds_util_fileExists(ds_global_string);   
}   

void _ds_g_slideshow_loadImages() {
	if (slideshow_specialDSMode) {
	   // We show the 2 special screens, specially crafted for the NDS!!!!!
	   sprintf(ds_global_string,"%s%s/%s %d A.png",DS_DIR_MAIN,DS_DIR_SYS,ds_state_var_getStr(),
					ds_state_var_getInt(0));
		if (ds_util_fileExists(ds_global_string))
		   if (ds_15bpp_load(ds_global_string,&slideshow_NDS_upImage,1,0))
		   	slideshow_drawUp = 1;
	   	
	   sprintf(ds_global_string,"%s%s/%s %d B.png",DS_DIR_MAIN,DS_DIR_SYS,ds_state_var_getStr(),
					ds_state_var_getInt(0));
		if (ds_util_fileExists(ds_global_string))					
		   if (ds_15bpp_load(ds_global_string,&slideshow_NDS_downImage,1,0))
			   slideshow_drawDown = 1;	   
	} else {
	   // Aiyaaaaa.... we need to keep the 600x240 screen
	   sprintf(ds_global_string,"%s%s/%s %d.png",DS_DIR_MAIN,DS_DIR_SYS,ds_state_var_getStr(),
					ds_state_var_getInt(0));
	   ds_15bpp_load(ds_global_string,&slideshow_600x240_bigImage,1,0);	   
	   ds_15bpp_resize(&slideshow_600x240_smallImageNOFREEME,&slideshow_600x240_bigImage,256,102);
	   slideshow_drawUp = 1;
	   slideshow_drawDown = 1;				   
	}     
}   

void _ds_g_slideshow_paintImages() {
	if (slideshow_specialDSMode) {
	   if (slideshow_drawUp)
			ds_15bpp_putScreen(ds_global_getScreen1(),&slideshow_NDS_upImage,0,0,1);
		if (slideshow_drawDown)
			ds_15bpp_putScreen(ds_global_getScreen0(),&slideshow_NDS_downImage,0,0,1);
	} else {
	   if (slideshow_drawUp)
	   	ds_15bpp_putScreenPartial(ds_global_getScreen1(),&slideshow_600x240_bigImage,0,0,slideshow_600x240_x,slideshow_600x240_y,256,192);
		if (slideshow_drawDown)
	      ds_global_fillScreen(0,ds_global_getScreen0(),PA_RGB(31,31,31));
	}     
}   

void _ds_g_slideshow_createButtons() {

	// Create "Image preview" button
	if (!slideshow_specialDSMode) {
	   slideshow_btnImaPreview = ds_button_create(0,16,256,102,0,"");
	   ds_button_updateLocalImageForce(slideshow_btnImaPreview,&slideshow_600x240_smallImageNOFREEME);
	   ds_button_addManagement(slideshow_btnImaPreview,_ds_slideshow_showBigMap,1);
	   	// Since the image is stored inside a button (which frees the image when destroyed), 
			//  that's the reason it should not be freed.
	}   
				
	// Create "Back"/"Next" buttons
	if (ds_state_var_getInt(2)) {
	   // Just one "back" button
		sprintf(ds_global_string,"%s%s/Gui_btn_small.png",DS_DIR_MAIN,DS_DIR_SYS);   
		sprintf(slideshow_string,"%s%s/Gui_btn_small2.png",DS_DIR_MAIN,DS_DIR_SYS);   
		slideshow_btnBack = ds_button_create(198,158,48,12,0,ds_global_string);
		ds_button_addClick(slideshow_btnBack,6,slideshow_string);
		ds_button_addText(slideshow_btnBack,-1,0,"Back",PA_RGB(0,0,0),1,1);
		ds_button_addManagement(slideshow_btnBack,_ds_slideshow_previous,0);
		ds_button_addSound(slideshow_btnBack, "Tiny Hit");
	} else {
	   // Normal buttons
		sprintf(ds_global_string,"%s%s/Gui_btn_small.png",DS_DIR_MAIN,DS_DIR_SYS);   
		sprintf(slideshow_string,"%s%s/Gui_btn_small2.png",DS_DIR_MAIN,DS_DIR_SYS);   
		slideshow_btnBack = ds_button_create(10,158,48,12,0,ds_global_string);
		ds_button_addClick(slideshow_btnBack,6,slideshow_string);
		ds_button_addText(slideshow_btnBack,-1,0,"Back",PA_RGB(0,0,0),1,1);
		ds_button_addManagement(slideshow_btnBack,_ds_slideshow_previous,0);
		ds_button_addSound(slideshow_btnBack, "Tiny Hit");
		sprintf(ds_global_string,"%s%s/Gui_btn_forth.png",DS_DIR_MAIN,DS_DIR_SYS);   
		sprintf(slideshow_string,"%s%s/Gui_btn_forth2.png",DS_DIR_MAIN,DS_DIR_SYS);   
		slideshow_btnNext = ds_button_create(198,158,48,12,0,ds_global_string);	
		ds_button_addClick(slideshow_btnNext,6,slideshow_string);
		ds_button_addManagement(slideshow_btnNext,_ds_slideshow_next,0);
		ds_button_addSound(slideshow_btnNext, "Tiny Hit");
	}   
}


   
//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------


/* Starts this subScreen */
void ds_g_slideshow_start() {
   // Init variables :-)   
	slideshow_600x240_x = 0;
	slideshow_600x240_y = 0;
	ds_15bpp_init(&slideshow_600x240_bigImage);
	ds_15bpp_init(&slideshow_600x240_smallImageNOFREEME);
	ds_15bpp_init(&slideshow_NDS_upImage);
	ds_15bpp_init(&slideshow_NDS_downImage);
	slideshow_specialDSMode = _ds_g_slideshow_checkDSMode();
	slideshow_normalMode = _ds_g_slideshow_checkNormalMode();
	
   // BEFORE Anything... check whether I should change to another state!!!!
   if ((!slideshow_specialDSMode) && (!slideshow_normalMode)) {
      _ds_g_slideshow_change();
   } else {      
	   // Load image system (Preview OR Normal)
   	_ds_g_slideshow_loadImages();
   
	   // Now, create the buttons and subsystems
   	_ds_g_slideshow_createButtons();

	}   
	
	// <HACK> "Cleans" the 3D sprite system, if there are sprites floating around :-)
	ds_3dsprite_cleanHWSprites();
}   

/* Finish this subScreen */
void ds_g_slideshow_finish() {
   // First, delete variables
   if (slideshow_specialDSMode) {
	   ds_15bpp_delete(&slideshow_NDS_upImage);      
      ds_15bpp_delete(&slideshow_NDS_downImage);
   } else if (slideshow_normalMode) {
      ds_15bpp_delete(&slideshow_600x240_bigImage);      
	}      
   
	// Now, reset the button information
	ds_button_reset();
}   

/* Manages the input of the user */
void ds_g_slideshow_input() {
   int click;
   
   /* Transform the user input into variables */
   ds_input_getInputPad(&ds_global_input);

	/* Check the buttons - Newpress & Held */
	if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_STYLUS)) {
	   ds_button_checkClick(ds_global_input.stylusx,ds_global_input.stylusy,0);
	} else {
		click = (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_STYLUS));
		if (ds_button_checkClick(ds_global_input.stylusx,ds_global_input.stylusy,click)) {
		   slideshow_drawDown = 1;
		}   	   
	}      	
}   

/* Manages the state of the game */
void ds_g_slideshow_state() {
   
}   

/* Paints this actual game state */
void ds_g_slideshow_paint() {
   // Wait for the VRetrace
   PA_WaitForVBL();

   // If we need to paint...(lower screen)
   if ((slideshow_drawDown) || (slideshow_drawUp)) {
      _ds_g_slideshow_paintImages();
      if (slideshow_drawDown)
	      ds_button_paint(); // Buttons are only found in the lower screen
			   
      if (slideshow_drawUp) 
	      ds_global_paintScreen(1,ds_global_getScreen1(),0,0);      
	   if (slideshow_drawDown)
      	ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
      
      slideshow_drawDown = 0;
      slideshow_drawUp = 0;
   }   
}
