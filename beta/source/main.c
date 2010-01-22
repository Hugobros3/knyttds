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

/* Details to solve:
= Details / Bugs (Mayor)
	- Hunt and Destroy BO
		- Symptoms:
		   + "Cannot load map.bin" on changing screen
		   + Signs and (icon) objects cannot be touched
		- DELETE (-fbounds-check) from makefile

= Details / Bugs (Minor)
	- INI line too long if not CR at the end
	- COs!
		- Alpha in big CO must be implemented
		- Minimap management of big COs (respect layer order)
		- Special "we should load this and that 'cause memory is limited" for COs
	- Optimization CHECK (Seems that "out of cache" check does not work)
		LOCATED - file date does not work
	
= Details / Other
	- Include all critter/juni sounds into the engine
	- Optimize the source code to reduce the use of RAM.
	- Small fixes everywhere.
		- Umbrella & CO
		- Sound in cutscenes
		- Delete normal sound, not looping sound
		- Particles are stopped by umbrella
	- Include new PAlib, or include fixes into actual version.
   
= Details / Bugs (Design)
	- Repeated _changetogame routine in Cutscene/LoadGame (Design failure!)
	- Game state manages "object" -> "juni", but it is the opposite! (Design Failure!)
				
= OPTIMIZATION
	- After checking all objects...
		Check the minimap
		Check the "danger" flags
   -(VI)- Collision algorithm!
   -(VI)- bits as macros
   - 3DSprites?????
   	+ "Overflow" mechanism :-P
   - LoadMap?
   	+ TileOnDemand (Do not load ALL the tileset)
   		FIX partial load
   	+ Tilemap (...not that important)
   	/DONE/ Minimap Icons (once they are loaded, left them on the memory :-P)
   	/DONE/ GUI - General
   		= Have 3 methods: INIT (for starting things), LOAD (per game load), RESET (changes per screen)
   	/DONE/ GUI - Load the GUI (keep in memory)
   		= Keep the image in memory
   			NEED! Add an "end" method that deletes the GUI image :-).
   	/DONE/ GUI - Items! (Paint on GUI)
   		= Keep the icons in memory 
*/

// Includes
#include <PA9.h>       // Include for PA_Lib
#include <fat.h>
#include <string.h>
#include <sys/dir.h>
#include "ds_util_bit.h"
#include "ds_state.h"
#include "ds_global.h"
#include "ds_g_game.h"
#include "ds_g_loadgame.h"
#include "ds_g_mainmenu.h"
#include "ds_g_levelmenu.h"
#include "ds_g_loadmenu.h"
#include "ds_g_cutscene.h"
#include "ds_g_destroygame.h"
#include "ds_g_slideshow.h"
#include "ds_g_options.h"
#include "ds_g_map2raw.h"
#include "ds_3dsprite.h"
#include "ds_input.h"
#include "ds_util.h"
#include "ds_button.h"
#include "ds_music.h"
#include "ds_15bpp.h"
#include "ds_gamestatus.h"

//---------------------------------------------------------------- 
// Original code by Cydrak :-) - gbadev.org
#include <unistd.h>
#include <malloc.h> 
extern u8 __end__[];        // end of static code and data
extern u8 __eheap_end[];    // farthest point to which the heap will grow 

u8* getHeapStart() {
   return __end__;
}
u8* getHeapEnd() {
   return (u8*)sbrk(0);
}
u8* getHeapLimit() {
   return __eheap_end;
}
extern int getMemUsed() {
   struct mallinfo mi = mallinfo();
   return mi.uordblks;
}
extern int getMemFree() {
   struct mallinfo mi = mallinfo();
   return mi.fordblks + (getHeapLimit() - getHeapEnd());
}
int getMemFreeSpecial() {
	struct mallinfo mi = mallinfo();
	return mi.fordblks + (getHeapLimit() - getHeapEnd()) //getMemFree() 
		+ mi.uordblks // getMemUsed()
		- mi.arena;
}	
//---------------------------------------------------------------- 

void _mainNoFAT() {
   /*PA_CenterSmartText(1,0 , 0, 
								256, 192, 
								"NO DLDI", 
								1, 4, 0);	

   PA_CenterSmartText(0,0 , 0, 
								256, 64, 
								"DLDI Unavailable, Error while reading. \n Please patch this ROM with the DLDI tool.", 
								1, 2, 0);	
   PA_CenterSmartText(0,0 , 64, 
								256, 128, 
								"DLDI No disponible, error al leer. \n Por favor parchee esta ROM con \n la herramienta DLDI.", 
								1, 2, 0);	
   PA_CenterSmartText(0,0 , 128, 
								256, 192, 
								"http://chishm.drunkencoders.com/DLDI/", 
								1, 2, 0);									*/
	PA_OutputText(1,1,1,"Error: no dldi");							
   while (1) {
      // Hangs.... :-(
      PA_WaitForVBL();
   }   
}

// Show error "No DSKnyttStories" message, hangs
void _mainNoDirectory() {
   /* PA_CenterSmartText(1,0 , 0, 
								256, 192, 
								"NO /data/DS_KS", 
								1, 4, 0);	

   PA_CenterSmartText(0,0 , 0, 
								256, 64, 
								"Directory /data/DS_KS not found. \n Please download it and copy it \n to the flash cart.", 
								1, 2, 0);	
   PA_CenterSmartText(0,0 , 64, 
								256, 128, 
								"Directorio /data/DS_KS no existe. \n Por favor descarguelo y copielo \n en su flash cart.", 
								1, 2, 0);	
   PA_CenterSmartText(0,0 , 128, 
								256, 192, 
								"http://www.rrc2soft.com", 
								1, 2, 0);									*/
								
   while (1) {
      // Hangs.... :-(
      PA_WaitForVBL();
   }   
}

void _mainLoadSplashScreen() {
   ds_t_15bpp ima_upscreen;
   ds_t_15bpp ima_downscreen;
   char filen[255];

	// Nifflas's games...   
  	sprintf(filen,"%s%s/Gui_logo_nifflas.png",DS_DIR_MAIN,DS_DIR_SYS);
	ds_15bpp_load(filen, &ima_upscreen, 1, 0);
	
	// By rrc2soft...
  	sprintf(filen,"%s%s/Gui_logo_rrc.png",DS_DIR_MAIN,DS_DIR_SYS);
	ds_15bpp_load(filen, &ima_downscreen, 1, 0);
	
	// Put'em...
	ds_15bpp_paintScreen(1,&ima_upscreen,0,0,1);
	ds_15bpp_paintScreen(0,&ima_downscreen,0,0,1);
	
	// Wait a sec... ^_-
	int i;
	for (i = 0; i < 60*4; i++)
		PA_WaitForVBL();  
}   

// Function: _mainManageInput()
void _mainManageInput() 
{
	switch (ds_state_getState()) {
		case DSKNYTT_:
			break;
		case DSKNYTT_GAME :
			ds_g_game_input();
			break;
		case DSKNYTT_LOADGAME :
			ds_g_loadgame_input();
			break;
		case DSKNYTT_MAINMENU :
			ds_g_mainmenu_input();
			break;			
		case DSKNYTT_LEVELMENU :
			ds_g_levelmenu_input();
			break;						
		case DSKNYTT_LOADMENU :
			ds_g_loadmenu_input();
			break;					
		case DSKNYTT_CUTSCENE :
			ds_g_cutscene_input();
			break;								
		case DSKNYTT_DESTROYGAME :
			ds_g_destroygame_input();
			break;										
		case DSKNYTT_SLIDESHOW :
			ds_g_slideshow_input();
			break;													
		case DSKNYTT_OPTIONS :
			ds_g_options_input();
			break;																
		case DSKNYTT_MAP2RAW :
			ds_g_map2raw_input();
			break;																
	}	
} // end of _mainManageInput()

// Function: _mainManageGameState()
void _mainManageGameState() 
{
	switch (ds_state_getState()) {
		case DSKNYTT_:
			break;
		case DSKNYTT_GAME :
			ds_g_game_state();
			break;
		case DSKNYTT_LOADGAME :
			ds_g_loadgame_state();
			break;			
		case DSKNYTT_MAINMENU :
			ds_g_mainmenu_state();
			break;						
		case DSKNYTT_LEVELMENU :
			ds_g_levelmenu_state();
			break;									
		case DSKNYTT_LOADMENU :
			ds_g_loadmenu_state();
			break;								
		case DSKNYTT_CUTSCENE :
			ds_g_cutscene_state();
			break;											
		case DSKNYTT_DESTROYGAME :
			ds_g_destroygame_state();
			break;													
		case DSKNYTT_SLIDESHOW :
			ds_g_slideshow_state();
			break;																
		case DSKNYTT_OPTIONS :
			ds_g_options_state();
			break;
		case DSKNYTT_MAP2RAW :
			ds_g_map2raw_state();
			break;																							
	}	
} // end of _mainManageGameState()

// Function: _mainChangeGameState()
void _mainChangeGameState()
{
	// Finishes SubSystem
	switch (ds_state_getState()) {
		case DSKNYTT_:
			break;
		case DSKNYTT_GAME :
			ds_g_game_finish();
			break;
		case DSKNYTT_LOADGAME :
			ds_g_loadgame_finish();
			break;
		case DSKNYTT_MAINMENU :
			ds_g_mainmenu_finish();
			break;			
		case DSKNYTT_LEVELMENU :
			ds_g_levelmenu_finish();
			break;									
		case DSKNYTT_LOADMENU :
			ds_g_loadmenu_finish();
			break;								
		case DSKNYTT_CUTSCENE :
			ds_g_cutscene_finish();
			break;											
		case DSKNYTT_DESTROYGAME :
			ds_g_destroygame_finish();
			break;													
		case DSKNYTT_SLIDESHOW :
			ds_g_slideshow_finish();
			break;																
		case DSKNYTT_OPTIONS :
			ds_g_options_finish();
			break;																						
		case DSKNYTT_MAP2RAW :
			ds_g_map2raw_finish();
			break;
	}
  ds_state_updateState();
  // Starts new System
	switch (ds_state_getState()) {
		case DSKNYTT_:
			break;
		case DSKNYTT_GAME :
			ds_g_game_start();
			break;
		case DSKNYTT_LOADGAME :
			ds_g_loadgame_start();
			break;			
		case DSKNYTT_MAINMENU :
			ds_g_mainmenu_start();
			break;						
		case DSKNYTT_LEVELMENU :
			ds_g_levelmenu_start();
			break;									
		case DSKNYTT_LOADMENU :
			ds_g_loadmenu_start();
			break;							
		case DSKNYTT_CUTSCENE :
			ds_g_cutscene_start();
			break;											
		case DSKNYTT_DESTROYGAME :
			ds_g_destroygame_start();
			break;													
		case DSKNYTT_SLIDESHOW :
			ds_g_slideshow_start();
			break;																
		case DSKNYTT_OPTIONS :
			ds_g_options_start();
			break;
		case DSKNYTT_MAP2RAW :
			ds_g_map2raw_start();
			break;																						
	}
} // end of _mainChangeGameState()

// Function: _mainManagePaint()
void _mainManagePaint() 
{
	switch (ds_state_getState()) {
		case DSKNYTT_:
			break;
		case DSKNYTT_GAME :
			ds_g_game_paint();
			break;
		case DSKNYTT_LOADGAME :
			ds_g_loadgame_paint();
			break;
		case DSKNYTT_MAINMENU :
			ds_g_mainmenu_paint();
			break;
		case DSKNYTT_LEVELMENU :
			ds_g_levelmenu_paint();
			break;									
		case DSKNYTT_LOADMENU :
			ds_g_loadmenu_paint();
			break;								
		case DSKNYTT_CUTSCENE :
			ds_g_cutscene_paint();
			break;											
		case DSKNYTT_DESTROYGAME :
			ds_g_destroygame_paint();
			break;											
		case DSKNYTT_SLIDESHOW :
			ds_g_slideshow_paint();
			break;																
		case DSKNYTT_OPTIONS :
			ds_g_options_paint();
			break;																						
		case DSKNYTT_MAP2RAW :
			ds_g_map2raw_paint();
			break;
	}	
} // end of _mainManagePaint()



// Function: main()
int main(int argc, char ** argv)
{
   // Init system states
	PA_Init();    // Initializes PA_Lib
	PA_InitVBL(); // Initializes a standard VBL
	// PA_InitSound(); - Initializes the sound subsystem
	PA_InitRand(); // Initializes the random subsystem
	
	// Init Basic Graphics State
	PA_InitText(1,2);     // Text screen in the upper screen
	
	// Filesystem!
	PA_WaitForVBL();  PA_WaitForVBL();  PA_WaitForVBL();  // wait a few VBLs	
	if (!fatInit(32, true)) { //Initialize fat library
		// No fat?
		_mainNoFAT();
	}	
	//if (diropen(DIR_MAIN) == NULL) {
	//   _mainNoDirectory();
	//}   
	PA_WaitForVBL();  PA_WaitForVBL();  PA_WaitForVBL();  // wait a few VBLs	
		
	// Init Advanced Graphics State
	ds_3dsprite_init1();   // 3D System	
	PA_Init16bitBg(0,3);  // 16bit screen in the touch screen
	PA_Init16bitBg(1,3);  // 16bit screen in the upper screen
	PA_InitText(1,2);     // Text screen in the upper screen
	
	_mainLoadSplashScreen(); // Splash Screens!
	
	ds_3dsprite_init2();   // 3D System	part deux
	
	// Init subsystems	
	ds_music_init(); // Music Subsystem!!!
	ds_global_initVariables(); // Initializes internally objects, map, buttons...
	// After init global state, fades...
	ds_global_fadeWhitePush(10);
		
	// Init DSKnyttStories states
	ds_state_assignState(DSKNYTT_MAINMENU);  // <TODO> Start in the logo, not in the menu
	
	// <DEBUG> Timer management
	StartTime(true);
	ds_global_timer = NewTimer(false);
	
	// Infinite loop to keep the program running
	while (1)
	{
	   // <DEBUG> Timer...
  		PauseTimer(ds_global_timer);
		ResetTimer(ds_global_timer);
		UnpauseTimer(ds_global_timer);

	   // Change the actual State of the Game
	   if (ds_state_newState()) {
	     _mainChangeGameState();
		}   
	   
	   // Manage the input of the user in this State
      _mainManageInput();
      
      // Manage the state of the game itself
      if (!ds_state_newState())
      	_mainManageGameState();	
      	
      // Checks if lid is closed, manage cases where the ds should not be paused
      if (!ds_lid_ignore) {
	      PA_CheckLid();
	   } else {
		   if (PA_LidClosed()) {
		      if (ds_lid_open) {
		   		PA_SetScreenLight(0,0);
		   		PA_SetScreenLight(1,0);
		 		}  
				ds_lid_open = 0;
				// TODO: Music!!	
		 	} else {
		 	   if (!ds_lid_open) {
		   		PA_SetScreenLight(0,1);
		   		PA_SetScreenLight(1,1);
		  		} 		
		  		ds_lid_open = 1;
		  		// TODO: Music!!
			}		   	
		}			   
      
      // Manage Subsystems that are NOT dependent of the state :-)
      ds_music_manage();      
      
      // Paints... (includes vertical retrace :-D)
      _mainManagePaint();
		      		
		// Finally, one tic more :-)
		ds_global_tick++;
		
		// <DEBUG>
		sprintf(ds_global_string,"U: %d / F: %d   ", getMemUsed(), getMemFree());       		
		ds_gamestatus_debugOutput(1,0,11,ds_global_string,DS_C_STA_DEBUG);
		ds_gamestatus_debugOutput(1,0,11,ds_global_string,DS_C_STA_DEBUG2);		
	}
	
	return 0;	
} // End of main()
