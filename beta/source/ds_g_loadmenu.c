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
#include "ds_music.h"

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

int loadmenu_drawDown;
int loadmenu_drawUp;
char loadmenu_string[255];
char loadmenu_string2[255];

int loadmenu_actualSlot;
int loadmenu_eraseTries;

int loadmenu_btn[3];
int loadmenu_btnLoad;
int loadmenu_btnNew;
int loadmenu_btnErase;
int loadmenu_btnEraseText;
int loadmenu_btnBack;
int loadmenu_btnImaPreview;

int loadmenu_specialDSMode;
int loadmenu_600x240_x;
int loadmenu_600x240_y;
ds_t_15bpp loadmenu_600x240_bigImage;
ds_t_15bpp loadmenu_600x240_smallImageNOFREEME; // A weird remind that this image MUST NOT BE FREED
ds_t_15bpp loadmenu_NDS_upImage;
ds_t_15bpp loadmenu_NDS_downImage;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Button pushing)
//-------------------------------------------------------------------------------------------------

int _ds_loadmenu_goBack(void *btn) {
	ds_state_assignState(DSKNYTT_LEVELMENU); // Go back!
   return 1;
}   

int _ds_loadmenu_loadGame(void *btn) {
   //ds_state_var_setStr(...);  <-- Actually, the str we had is the str needed by the next state ;-)
	ds_state_assignState(DSKNYTT_LOADGAME);
	ds_state_var_setInt(0,DS_C_LOADGAME_GAME); // Da game!
	ds_state_var_setInt(1,1); // Reset everything
	ds_state_var_setInt(2,loadmenu_actualSlot); // Specific save state
   return 1;
}   

int _ds_loadmenu_loadGameNew(void *btn) {
   //ds_state_var_setStr(...);  <-- Actually, the str we had is the str needed by the next state ;-)
	ds_state_assignState(DSKNYTT_LOADGAME);
	ds_state_var_setInt(0,DS_C_LOADGAME_INTRO); // Da intro!
	ds_state_var_setInt(1,1); // Reset everything
	ds_state_var_setInt(2,loadmenu_actualSlot); // Specific save state... NEW!
   return 1;
}   

int _ds_loadmenu_clickSlotID(int slot) {   
   if (loadmenu_actualSlot == slot)
	   // Click myself again? Hide myself! :-D
   	loadmenu_actualSlot = -1;
   else 
   	// New loadslot...	
   	loadmenu_actualSlot = slot;
   	
   // Buttons... First, hide everything!
   ds_button_hide(loadmenu_btnLoad,1);
   ds_button_hide(loadmenu_btnNew,1);
   ds_button_hide(loadmenu_btnErase,1);
   if (loadmenu_actualSlot != -1) {
      // Show buttons... but buttons depend on existing savegame of slot
		sprintf(ds_global_string,"%s%s/%s %d.ini",DS_DIR_MAIN,DS_DIR_SAVE,ds_state_var_getStr(),loadmenu_actualSlot);
	   if (!ds_util_fileExists(ds_global_string)) { 
	      ds_button_hide(loadmenu_btnNew,0);
	   } else {
	      ds_button_hide(loadmenu_btnLoad,0);
	      ds_button_hide(loadmenu_btnErase,0);
	      ds_button_setInner(loadmenu_btnErase,slot);
	      loadmenu_eraseTries = 0;
         ds_button_updateText(loadmenu_btnErase,loadmenu_btnEraseText,"Erase");	      
		}      
	}    
	
	// Redraw!
	loadmenu_drawDown = 1;
   	
   return 1;
}   

int _ds_loadmenu_clickSlot(void *btn) {
   int id = ds_button_getID(btn);
   int slot = ds_button_getInner(id);
   
   return _ds_loadmenu_clickSlotID(slot);
}   

int _ds_loadmenu_eraseGame(void *btn) {
   switch (loadmenu_eraseTries) {
      case 0:
         ds_button_updateText(loadmenu_btnErase,loadmenu_btnEraseText,"Erase???");
         loadmenu_eraseTries++;
			loadmenu_drawDown = 1;         
         break;
      case 1:
         ds_button_updateText(loadmenu_btnErase,loadmenu_btnEraseText,"Erase!!!");
         loadmenu_eraseTries++;
			loadmenu_drawDown = 1;                  
         break;
      case 2:
         // Call the erase routine :-)
         sprintf(ds_global_string,"%s%s/%s %d.ini",DS_DIR_MAIN,DS_DIR_SAVE,ds_state_var_getStr(),loadmenu_actualSlot);
         remove(ds_global_string);
         // Empty things...
         _ds_loadmenu_clickSlotID(ds_button_getInner(ds_button_getID(btn)));         
         break;
   }   
   
   return 1;
}   

int _ds_loadmenu_showBigMap(void *btn) { // Remember... 600x240-256x102 (256x192)
   int x = ds_button_getLocalX_p(btn,ds_global_input.stylusx);
   int y = ds_button_getLocalY_p(btn,ds_global_input.stylusy);
   int realx = (x * ((600 * 100) / 256)) / 100;
   int realy = (y * ((240 * 100) / 102)) / 100;
	
	if (realx < (256 >> 1)) 
		loadmenu_600x240_x = 0;
	else if (realx >= (600 - (256 >> 1))) 
		loadmenu_600x240_x = 600 - 256;
	else loadmenu_600x240_x = realx - (256 >> 1);
	
	if (realy < (192 >> 1)) 
		loadmenu_600x240_y = 0;
	else if (realy >= (240 - (192 >> 1))) 
		loadmenu_600x240_y = 240 - 192;
	else loadmenu_600x240_y = realy - (192 >> 1);

	loadmenu_drawUp = 1;   
	
   return 1;
}

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Management)
//-------------------------------------------------------------------------------------------------

void _ds_g_loadmenu_checkDSMode() {
   sprintf(loadmenu_string,"%s%s/%s/%s",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr(),"InfoA.png");
	loadmenu_specialDSMode = ds_util_fileExists(loadmenu_string);   
}   

void _ds_g_loadmenu_loadImages() {
	if (loadmenu_specialDSMode) {
	   // We show the 2 special screens, specially crafted for the NDS!!!!!
	   sprintf(loadmenu_string,"%s%s/%s/%s",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr(),"InfoA.png");
	   ds_15bpp_load(loadmenu_string,&loadmenu_NDS_upImage,1,0);
	   sprintf(loadmenu_string,"%s%s/%s/%s",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr(),"InfoB.png");
	   ds_15bpp_load(loadmenu_string,&loadmenu_NDS_downImage,1,0);	   
	} else {
	   // Aiyaaaaa.... we need to keep the 600x240 screen
	   sprintf(loadmenu_string,"%s%s/%s/%s",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr(),"Info.png");
	   ds_15bpp_load(loadmenu_string,&loadmenu_600x240_bigImage,1,0);	   
	   ds_15bpp_resize(&loadmenu_600x240_smallImageNOFREEME,&loadmenu_600x240_bigImage,256,102);
	}     
}   

void _ds_g_loadmenu_paintImages() {
	if (loadmenu_specialDSMode) {
	   if (loadmenu_drawUp)
			ds_15bpp_putScreen(ds_global_getScreen1(),&loadmenu_NDS_upImage,0,0,1);
		if (loadmenu_drawDown)
			ds_15bpp_putScreen(ds_global_getScreen0(),&loadmenu_NDS_downImage,0,0,1);
	} else {
	   if (loadmenu_drawUp)
	   	ds_15bpp_putScreenPartial(ds_global_getScreen1(),&loadmenu_600x240_bigImage,0,0,loadmenu_600x240_x,loadmenu_600x240_y,256,192);
		if (loadmenu_drawDown)
	      ds_global_fillScreen(0,ds_global_getScreen0(),PA_RGB(31,31,31));
	}     
}   

void _ds_g_loadmenu_createButtons() {
	int i = 0;

	// Create "Slot" buttons	
	sprintf(loadmenu_string,"%s%s/Gui_btn_slot.png",DS_DIR_MAIN,DS_DIR_SYS);
	sprintf(loadmenu_string2,"%s%s/Gui_btn_slot2.png",DS_DIR_MAIN,DS_DIR_SYS);   
	for (i = 0; i < 3; i++) {
	   sprintf(ds_global_string,"Slot %d",i+1);   
	   loadmenu_btn[i] = ds_button_create(8 + (i * 84),128,72,16,0,loadmenu_string);	   
	   ds_button_addClick(loadmenu_btn[i],6,loadmenu_string2);
	   ds_button_setInner(loadmenu_btn[i],i + 1); // Save slots: 1 - 2 - 3
	   ds_button_addText(loadmenu_btn[i],0,0,ds_global_string,PA_RGB(0,0,0),2,1);	   
	   ds_button_addManagement(loadmenu_btn[i],_ds_loadmenu_clickSlot,0);
	   ds_button_addSound(loadmenu_btn[i], "Tiny Hit");
	}   	 
	
	// Create "Image preview" button
	if (!loadmenu_specialDSMode) {
	   loadmenu_btnImaPreview = ds_button_create(0,16,256,102,0,"");
	   ds_button_updateLocalImageForce(loadmenu_btnImaPreview,&loadmenu_600x240_smallImageNOFREEME);
	   ds_button_addManagement(loadmenu_btnImaPreview,_ds_loadmenu_showBigMap,1);
	   	// Since the image is stored inside a button (which frees the image when destroyed), 
			//  that's the reason it should not be freed.
	}   
	
	// Create "Load/Save/Erase" Buttons
	sprintf(loadmenu_string,"%s%s/Gui_btn_medium.png",DS_DIR_MAIN,DS_DIR_SYS);   
	sprintf(loadmenu_string2,"%s%s/Gui_btn_medium2.png",DS_DIR_MAIN,DS_DIR_SYS);   
	loadmenu_btnLoad = ds_button_create(80,152,96,16,0,loadmenu_string);
	ds_button_addClick(loadmenu_btnLoad,6,loadmenu_string2);
	ds_button_addText(loadmenu_btnLoad,0,0,"Load Game",PA_RGB(0,0,0),2,1);
	ds_button_addManagement(loadmenu_btnLoad,_ds_loadmenu_loadGame,0);
	ds_button_addSound(loadmenu_btnLoad, "Tiny Hit");
	ds_button_hide(loadmenu_btnLoad,1);
	loadmenu_btnNew = ds_button_create(80,164,96,16,0,loadmenu_string);
	ds_button_addClick(loadmenu_btnNew,6,loadmenu_string2);
	ds_button_addText(loadmenu_btnNew,0,0,"New Game",PA_RGB(0,0,0),2,1);
	ds_button_addManagement(loadmenu_btnNew,_ds_loadmenu_loadGameNew,0);
	ds_button_addSound(loadmenu_btnNew, "Tiny Hit");
	ds_button_hide(loadmenu_btnNew,1);
	sprintf(loadmenu_string,"%s%s/Gui_btn_mediumErase.png",DS_DIR_MAIN,DS_DIR_SYS);
	sprintf(loadmenu_string2,"%s%s/Gui_btn_mediumErase2.png",DS_DIR_MAIN,DS_DIR_SYS);
	loadmenu_btnErase = ds_button_create(80,176,96,16,0,loadmenu_string);
	ds_button_addClick(loadmenu_btnErase,6,loadmenu_string2);
	loadmenu_btnEraseText = ds_button_addText(loadmenu_btnErase,0,0,"Erase",PA_RGB(0,0,0),2,1);		
	ds_button_addManagement(loadmenu_btnErase,_ds_loadmenu_eraseGame,0);	
	ds_button_addSound(loadmenu_btnErase, "Tiny Hit");
	ds_button_hide(loadmenu_btnErase,1);
	
				
	// Create "Back" button
	sprintf(loadmenu_string,"%s%s/Gui_btn_small.png",DS_DIR_MAIN,DS_DIR_SYS);
	sprintf(loadmenu_string2,"%s%s/Gui_btn_small2.png",DS_DIR_MAIN,DS_DIR_SYS);   
	loadmenu_btnBack = ds_button_create(198,158,48,12,0,loadmenu_string);
	ds_button_addClick(loadmenu_btnBack,6,loadmenu_string2);
	ds_button_addText(loadmenu_btnBack,-1,0,"Back",PA_RGB(0,0,0),1,1);
	ds_button_addManagement(loadmenu_btnBack,_ds_loadmenu_goBack,0);
	ds_button_addSound(loadmenu_btnBack, "Tiny Hit");
}


   
//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------


/* Starts this subScreen */
void ds_g_loadmenu_start() {
   // Zero! Init variables :-)   
   loadmenu_actualSlot = -1;
   
   loadmenu_specialDSMode = 0;
	loadmenu_600x240_x = 0;
	loadmenu_600x240_y = 0;
	ds_15bpp_init(&loadmenu_600x240_bigImage);
	ds_15bpp_init(&loadmenu_600x240_smallImageNOFREEME);
	ds_15bpp_init(&loadmenu_NDS_upImage);
	ds_15bpp_init(&loadmenu_NDS_downImage);
	_ds_g_loadmenu_checkDSMode();
   
   // Load image system (Preview OR Normal)
   _ds_g_loadmenu_loadImages();
   
   // Now, create the buttons and subsystems
   _ds_g_loadmenu_createButtons();

	// Tells itself that it needs to be drawn
   loadmenu_drawUp = 1;
   loadmenu_drawDown = 1;
}   

/* Finish this subScreen */
void ds_g_loadmenu_finish() {
   // First, delete variables
   if (loadmenu_specialDSMode) {
      ds_15bpp_delete(&loadmenu_NDS_upImage);      
      ds_15bpp_delete(&loadmenu_NDS_downImage);
   } else {
      ds_15bpp_delete(&loadmenu_600x240_bigImage);      
	}      
   
	// Now, reset the button information
	ds_button_reset();   
	
	// This is an special moment: Fade to white, using the information from the screen buffers
	if (ds_state_getNextState() == DSKNYTT_LOADGAME) {
	   // <TODO> Fade to white
	} 
	
	// Also... if we are going to play a game, music need to be stopped!!!!!
	if (ds_state_getNextState() == DSKNYTT_LOADGAME) {
	   // ds_music_stopAllMusic();
	   // Note: WHY we do this here instead of in LOADGAME? Because of Juni dying and music NOT reloading ^_-
	} 	  
}   

/* Manages the input of the user */
void ds_g_loadmenu_input() {
   int click;
   
   /* Transform the user input into variables */
   ds_input_getInputPad(&ds_global_input);

	/* Check the buttons - Newpress & Held */
	if (ds_util_bitOne16(ds_global_input.Held,DS_C_IN_STYLUS)) {
	   ds_button_checkClick(ds_global_input.stylusx,ds_global_input.stylusy,0);
	} else {
		click = (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_STYLUS));
		if (ds_button_checkClick(ds_global_input.stylusx,ds_global_input.stylusy,click)) {
		   loadmenu_drawDown = 1;
		}   	   
	}      	
}   

/* Manages the state of the game */
void ds_g_loadmenu_state() {
   
}   

/* Paints this actual game state */
void ds_g_loadmenu_paint() {
   // Wait for the VRetrace
   PA_WaitForVBL();

   // If we need to paint...(lower screen)
   if ((loadmenu_drawDown) || (loadmenu_drawUp)) {
      _ds_g_loadmenu_paintImages();
      if (loadmenu_drawDown)
	      ds_button_paint(); // Buttons are only found in the lower screen
			   
      if (loadmenu_drawUp) 
	      ds_global_paintScreen(1,ds_global_getScreen1(),0,0);      
	   if (loadmenu_drawDown)
      	ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
      
      loadmenu_drawDown = 0;
      loadmenu_drawUp = 0;
   }   
}
