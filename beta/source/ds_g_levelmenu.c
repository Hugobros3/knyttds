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
#include "ds_input.h"
#include "ds_linkedlist.h"
#include "ds_15bpp.h"
#include "ds_ini.h"
#include <sys/dir.h>

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

typedef struct ds_tt_levelInfo {
   int id;
   char author[255];
   char description[255];
   char dirima[255];
   char dir[255];
} ds_t_levelInfo;

typedef struct ds_tt_levelBtnInfo {
   int id;
   int imaid;
   int authorid;
   int descriptionid;
} ds_t_levelBtnInfo;

int levelmenu_draw;
int levelmenu_drawBtn;
DIR_ITER* _dirit_levels = NULL;
LList *_ds_levels;
char levelmenu_string[255]; // WHY???... Well, I prefer to do this here, less memory for the limited stack :-P
char levelmenu_string2[255]; // Same as before :-P
int levelmenu_actual;
int levelmenu_max;
ds_t_levelBtnInfo levelmenu_btn[3];
int levelmenu_btnLvlBack;
int levelmenu_btnLvlForth;
int levelmenu_btnLvlNum;
int levelmenu_btnLvlNum_Text;
int levelmenu_btnLvlDes;
int levelmenu_btnLvlDes_Text;
int levelmenu_btnBack;
ds_t_15bpp levelmenu_imaUp;
int levelmenu_numberlevels;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Button pushing)
//-------------------------------------------------------------------------------------------------

int _ds_levelmenu_goBack(void *btn) {
	ds_state_assignState(DSKNYTT_MAINMENU); // Go back!
   return 1;
}   

int _ds_levelmenu_btnBack(void *btn) {
   int i;
   
   if (levelmenu_actual > 1) {
      // Move
      levelmenu_actual--;
   	levelmenu_drawBtn = 1;
   	// Deselects all buttons
		for (i = 0; i < 3; i++) {
	   	ds_button_select(levelmenu_btn[i].id,0);
		}         	
		ds_button_updateText(levelmenu_btnLvlDes,levelmenu_btnLvlDes_Text,"");
	}   
   return 1;
}   

int _ds_levelmenu_btnForth(void *btn) {
   int i;
   
   if (levelmenu_actual < levelmenu_max) {
      levelmenu_actual++;
   	levelmenu_drawBtn = 1;
   	// Deselects all buttons
		for (i = 0; i < 3; i++) {
	   	ds_button_select(levelmenu_btn[i].id,0);
		}    	
		ds_button_updateText(levelmenu_btnLvlDes,levelmenu_btnLvlDes_Text,"");		
	}   

   return 1;
}   

int _ds_levelmenu_btnLevel(void *btn) {
   int id = ds_button_getID(btn);
   int selectedlocal = ds_button_getInner(id);
   int selected = selectedlocal + ((levelmenu_actual-1)*3);
   int i;
   ds_t_levelInfo *level = ds_linkedlist_getPos(_ds_levels, selected);
   
   if (ds_button_getSelect(id)) {
      // Load Level! (Load/Save Screen)  
		ds_state_var_setStr(level->dir); 
		ds_state_assignState(DSKNYTT_MAP2RAW); // Loadmenu?
	} else {
	   // De-Selects other buttons, load new button
		for (i = 0; i < 3; i++) {
	   	ds_button_select(levelmenu_btn[i].id,(i == selectedlocal));
		}      
   	// Also, changes the description button!
	   ds_button_updateText(levelmenu_btnLvlDes,levelmenu_btnLvlDes_Text,level->dir);
	   levelmenu_drawBtn = 1;	   			
	}   

   return 1;
}   


//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS (Management)
//-------------------------------------------------------------------------------------------------

int _ds_g_levelmenu_dir_startIteration() {
	sprintf(ds_global_string,"%s%s/",DS_DIR_MAIN,DS_DIR_WORLD);   
	_dirit_levels = diropen (ds_global_string);  
	return (_dirit_levels != NULL);
}   

int _ds_g_levelmenu_dir_getIteration(char *filename, struct stat *st) {
   int res = -1;
   
   // Iteration finished
   if (_dirit_levels == NULL)
   	return 0;
   	
   // Try to get some...
   while (res == -1) {
	   if (dirnext(_dirit_levels, filename, st) == 0) {
	      if ((st->st_mode & S_IFDIR) && (strncmp(filename,".",1) != 0) )
	      	res = 1;
	   } else {
	      res = 0;
		}      
	}	
   
   // Return...
   return res;
}  

void _ds_g_levelmenu_dir_endIteration() {
   dirclose (_dirit_levels);
}   

int _ds_g_levelmenu_storeInfo() {
	struct stat st;   
	ds_t_levelInfo *level;
	void *levelini;
	int numlevels = 0;
   
	if (_ds_g_levelmenu_dir_startIteration()) {
	   while (_ds_g_levelmenu_dir_getIteration(ds_global_string, &st)) {
	      // OK, got a directory! Now, time to store the info on a level :-D
	      level = malloc(sizeof(ds_t_levelInfo));
	      if (level == NULL)  {
      		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
				//sprintf(ds_global_string,...);
	   		ds_global_errorHalt(ds_global_string);
   		}   
	      sprintf(level->dir,"%s",ds_global_string);	      
	      sprintf(level->dirima,"%s%s/%s/Icon.png",DS_DIR_MAIN,DS_DIR_WORLD,level->dir);
	      // Get info from the ini... Note that we can use ds_global_string from now on :-D, we copied it on level->dir
	      sprintf(ds_global_string,"%s%s/%s/World.ini",DS_DIR_MAIN,DS_DIR_WORLD,level->dir);
	      levelini = ds_ini_loadSection(ds_global_string,"World");
	      if (levelini != NULL) { // We just want the "World" section, 'cause some .ini files are really HUGE!!!!
	         sprintf(level->author,"%s (%s)",ds_ini_getstring(levelini,"World:Name",level->dir),ds_ini_getstring(levelini,"World:Author",level->dir));
	         sprintf(level->description,"%s",ds_ini_getstring(levelini,"World:Description",level->dir));
	         ds_ini_destroy(levelini);
	      }   
	      // Store it!
	      level->id = ds_linkedlist_add(_ds_levels,level);
	      numlevels++;
	   }
		_ds_g_levelmenu_dir_endIteration();   
	}   
	return numlevels;
}  

void _ds_g_levelmenu_createButtons() {
	ds_t_levelInfo *level;
	int i = 0;

	// Create "level" buttons	
	sprintf(levelmenu_string,"%s%s/Gui_btn_level.png",DS_DIR_MAIN,DS_DIR_SYS);   
	sprintf(levelmenu_string2,"%s%s/Gui_btn_level2.png",DS_DIR_MAIN,DS_DIR_SYS);   
	for (i = 0; i < 3; i++) {
	   level = ds_linkedlist_getPos(_ds_levels,i);
	   levelmenu_btn[i].id = ds_button_create(12,8 + (i * 42),232,34,0,levelmenu_string);	   
	   ds_button_addClick(levelmenu_btn[i].id,6,levelmenu_string2);
	   ds_button_setInner(levelmenu_btn[i].id,i);
	   if (level != NULL) {
		   levelmenu_btn[i].imaid = ds_button_addImage(levelmenu_btn[i].id,2,2,level->dirima);
		   levelmenu_btn[i].authorid = ds_button_addText(levelmenu_btn[i].id,35,6,level->author,PA_RGB(0,0,0),1,0);
		   levelmenu_btn[i].descriptionid = ds_button_addText(levelmenu_btn[i].id,35,20,level->description,PA_RGB(9,9,9),1,0);	   
		   ds_button_addManagement(levelmenu_btn[i].id,_ds_levelmenu_btnLevel,0);
		} else {
		   ds_button_hide(levelmenu_btn[i].id,1);
		}   
	}   	 
	
	// Create "Navigation" Buttons
	sprintf(levelmenu_string,"%s%s/Gui_btn_back.png",DS_DIR_MAIN,DS_DIR_SYS);   
	sprintf(levelmenu_string2,"%s%s/Gui_btn_back2.png",DS_DIR_MAIN,DS_DIR_SYS);   
	levelmenu_btnLvlBack = ds_button_create(50,136,48,12,0,levelmenu_string);
	ds_button_addClick(levelmenu_btnLvlBack,6,levelmenu_string2);
	ds_button_addManagement(levelmenu_btnLvlBack,_ds_levelmenu_btnBack,0);
	sprintf(levelmenu_string,"%s%s/Gui_btn_forth.png",DS_DIR_MAIN,DS_DIR_SYS);
	sprintf(levelmenu_string2,"%s%s/Gui_btn_forth2.png",DS_DIR_MAIN,DS_DIR_SYS);   
	levelmenu_btnLvlForth = ds_button_create(150,136,48,12,0,levelmenu_string);
	ds_button_addClick(levelmenu_btnLvlForth,6,levelmenu_string2);
	ds_button_addManagement(levelmenu_btnLvlForth,_ds_levelmenu_btnForth,0);	
	
	levelmenu_btnLvlNum = ds_button_create(100,136,48,12,0,"");
	sprintf(levelmenu_string,"Page %d/%d",levelmenu_actual,levelmenu_max);
	levelmenu_btnLvlNum_Text = ds_button_addText(levelmenu_btnLvlNum,0,0,levelmenu_string,PA_RGB(0,0,0),1,1);
	levelmenu_btnLvlDes = ds_button_create(12,158,198 - 12,12,0,"");
	levelmenu_btnLvlDes_Text = ds_button_addText(levelmenu_btnLvlDes,0,0,"",PA_RGB(0,0,0),1,1);
	
	// Create "Back" button
	sprintf(levelmenu_string,"%s%s/Gui_btn_small.png",DS_DIR_MAIN,DS_DIR_SYS);   
	sprintf(levelmenu_string2,"%s%s/Gui_btn_small2.png",DS_DIR_MAIN,DS_DIR_SYS);   
	levelmenu_btnBack = ds_button_create(198,158,48,12,0,levelmenu_string);
	ds_button_addClick(levelmenu_btnBack,6,levelmenu_string2);
	ds_button_addText(levelmenu_btnBack,-1,0,"Back",PA_RGB(0,0,0),1,1);
	ds_button_addManagement(levelmenu_btnBack,_ds_levelmenu_goBack,0);
}

void _ds_g_levelmenu_updateScreen() {
	ds_t_levelInfo *level;
	int i;

	// Update Buttons	
	for (i = 0; i < 3; i++) {
	   level = ds_linkedlist_getPos(_ds_levels,i + ((levelmenu_actual-1) * 3) );
	   if (level != NULL) {
		   ds_button_hide(levelmenu_btn[i].id,0);
		   ds_button_updateImage(levelmenu_btn[i].id,levelmenu_btn[i].imaid,level->dirima);
		   ds_button_updateText(levelmenu_btn[i].id,levelmenu_btn[i].authorid,level->author);
		   ds_button_updateText(levelmenu_btn[i].id,levelmenu_btn[i].descriptionid,level->description);
		} else {
		   ds_button_hide(levelmenu_btn[i].id,1);
		}   
	}   	 

	// Update Level Counter   
	sprintf(levelmenu_string,"Page %d/%d",levelmenu_actual,levelmenu_max);	
	ds_button_updateText(levelmenu_btnLvlNum,levelmenu_btnLvlNum_Text,levelmenu_string);
}
   
//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------


/* Starts this subScreen */
void ds_g_levelmenu_start() {
   // Zero! Init variables :-)
   _ds_levels = ds_linkedlist_init();
   levelmenu_drawBtn = 0;
   
   // First, get the information from the files
   levelmenu_numberlevels = _ds_g_levelmenu_storeInfo();
   levelmenu_actual = 1;
   levelmenu_max = ((levelmenu_numberlevels-1) / 3) + 1;   
   
   // Paint the upper screen, which may have been deleted (cute!)
	sprintf(ds_global_string,"%s%s/Gui_screen_ini_up.png",DS_DIR_MAIN,DS_DIR_SYS);
	ds_15bpp_load(ds_global_string, &levelmenu_imaUp, 1,0);
	ds_15bpp_putScreen(ds_global_getScreen1(),&levelmenu_imaUp,0,0,1);
   ds_15bpp_delete(&levelmenu_imaUp);
   
   // Now, create the buttons
   _ds_g_levelmenu_createButtons();

	// Tells itself that it needs to be drawn
	levelmenu_draw = 1;
}   

/* Finish this subScreen */
void ds_g_levelmenu_finish() {
   // First, delete variables
   ds_linkedlist_finish(_ds_levels,1,1); // Free contents and memory, too!
   
	// Now, reset the button information
	ds_button_reset();   

	// Finish 	
	
	// Now, change screens
   //int flag = 0;
   //switch (ds_state_getNextState()) {
   //   case DSKNYTT_:
   //      break;
   //}   
}   

/* Manages the input of the user */
void ds_g_levelmenu_input() {
   int click;
   
   /* Transform the user input into variables */
   ds_input_getInputPad(&ds_global_input);

	/* Check the buttons */
	click = (ds_util_bitOne16(ds_global_input.Newpress,DS_C_IN_STYLUS));
	if (ds_button_checkClick(ds_global_input.stylusx,ds_global_input.stylusy,click)) {
	   levelmenu_drawBtn = 1;
	}   
	
	/* Change the state of the screen */
	if (levelmenu_drawBtn) {
	   levelmenu_drawBtn = 0;
	   _ds_g_levelmenu_updateScreen();
	   levelmenu_draw = 1;
	}   
}   

/* Manages the state of the game */
void ds_g_levelmenu_state() {
   
}   

/* Paints this actual game state */
void ds_g_levelmenu_paint() {
   // Wait for the VRetrace
   PA_WaitForVBL();

   // If we need to paint...(lower screen)
   if (levelmenu_draw) {
      levelmenu_draw = 0;
      ds_global_fillScreen(0,ds_global_getScreen0(),PA_RGB(31,31,31));      
      ds_button_paint();
      ds_global_paintScreen(1,ds_global_getScreen1(),0,0);
      ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
   }   
}
