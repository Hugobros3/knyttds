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
	- TODO: Load the sprite when having a dialogue!
		Due to bugs on the 3DSprites :-P
*/

#include "ds_global.h"
#include "ds_world.h"
#include "ds_ini.h"
#include "ds_3dsprite.h"
#include "ds_15bpp.h"
#include "ds_gamestatus.h"
#include "PA_Text16bitBuffer.h"

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

#define DS_C_DIALOGSTATE_ON   0
#define DS_C_DIALOGSTATE_OFF  1


typedef struct _ds_tt_dialogue {
   char string[255]; // String contained in the dialogue
   int state; // State of the dialog (0: not shown, 1: shown)
   int type; // type of dialogue shown (A-B-C)
   int managedFrame; // Indicates if we already shown a dialogue for this particular frame
} _ds_t_dialogue;  

_ds_t_dialogue ds_v_dialogue;

// Dialog - texture
int _dialog3DTex;
ds_t_15bpp _dialog3DIma;


//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------
void _ds_dialogue_insertCR(char *des, char *ori, int size, int width) {
	int posOri, posDes;
	int sep;
	int lx, cx, sx;
	u8 c;

   sep = -1;
   posOri = 0;
   posDes = 0;
   lx = 0;
   sx = 0;
	while ((posDes < 254) && (posOri < 254)) { // No overflow
	   // Get actual character
	   c = ori[posOri];
	   // First, if '\0', this is the end ;-)
	   if (c == '\0')
	   	break;
	   // Second, lenght check :-)
	   cx = pa_bittextdefaultsize[size][c];
	   if (lx + cx > width) {
	      // is there any separator?
	      if (sep == -1) {
	      	// Sorry, but the phrase was too long... MACHACA!!!!!
	      	des[posDes] = '\n';
	      	lx = 0;	         
	      } else {   
	      	// insert '\n' on last separator of destination, restart things
	      	des[sep] = '\n';
	      	des[posDes] = ori[posOri];
	      	lx = sx + cx;
	    	}  	
	    	posDes++; posOri++;
	   } else {
	      // Is a separator? Store
   	   if ((c == ' ') || (c == '-') || (c == '_')) {
		      sep = posOri;
		      sx = 0;
	   	}
	   	des[posDes] = ori[posOri];
	   	posDes++; posOri++;
	   	lx += cx;
	   	sx += cx;
		}      
	}   
	des[posDes] = '\0';
}

/* Inits subsystem */
int ds_dialogue_init() {
   // Basic Variables
   ds_v_dialogue.type = DS_C_ELETYPE_;
   ds_v_dialogue.string[0] = '\0';
   ds_v_dialogue.state = DS_C_DIALOGSTATE_OFF;
   
   // Texture
	_dialog3DTex = -1;
	ds_15bpp_init(&_dialog3DIma);
	
	return 1;
}   

/* Resets subsystem */
int ds_dialogue_reset() {
   // Cleaning text & variables...
   PA_ClearTextBg(1);

   ds_v_dialogue.type = DS_C_ELETYPE_;
   ds_v_dialogue.string[0] = '\0';
   ds_v_dialogue.state = DS_C_DIALOGSTATE_OFF;

   // Texture out... 3D not necessary, but memory...
   _dialog3DTex = -1; // Sprite will be "cleaned" by the 3D subsystem
   if (_dialog3DIma.loaded) 
   	ds_15bpp_delete(&_dialog3DIma);
   	
   return 1;
}   


/* Manages dialogue - At least it prepares the management of the dialogue. Things will be done while updating */
int ds_dialogue_manageFirst() {
   ds_v_dialogue.managedFrame = 0; // If someone calls _update, it means we have a dialogue!
   return 1;
}   

/* Updates existing dialogue with a certain type */
int ds_dialogue_update(int type) {
   int update = 0;
   char dictkey[256];
   
   if (!ds_v_dialogue.managedFrame) {
      ds_v_dialogue.managedFrame = 1; // Only once!
      // No previous dialog
	   if (ds_v_dialogue.type == DS_C_ELETYPE_) {
	      ds_v_dialogue.type = type;
	      ds_v_dialogue.state = DS_C_DIALOGSTATE_ON;
	      update = 1;
	   } else
	   // Different dialogs
	   if (ds_v_dialogue.type != type) {
	      ds_v_dialogue.type = type;
	      ds_v_dialogue.state = DS_C_DIALOGSTATE_ON;
	      update = 1;	      
	   } 
	   // Same dialog
		else {
	      ds_v_dialogue.state = DS_C_DIALOGSTATE_ON;
	   } 
		// UPDATE...
		if (update) {
		   // ... String!
		   sprintf(dictkey,"%s",
				ds_ini_keyDictionary(ds_global_map.x, ds_global_map.y, DS_C_DICT_SIGN, ds_v_dialogue.type, 0, 0, 0));
		   strncpy(ds_global_string,ds_ini_getstring(ds_global_world.worldini,dictkey,""),250);
		   _ds_dialogue_insertCR(ds_v_dialogue.string, ds_global_string, 1, 128 - 8);
		   
		   // Before updating... ok, let's check :-)
			if (_dialog3DIma.loaded) 
   			ds_15bpp_delete(&_dialog3DIma);
   		if (_dialog3DTex != -1)
	      	ds_3dsprite_deleteSprite(_dialog3DTex,1);
		   _dialog3DTex = -1;
		   
		   // ... Ima! Load!
		   sprintf(dictkey,"%s%s/Gui_game_dialog.png",DS_DIR_MAIN,DS_DIR_SYS);
		   ds_15bpp_load(dictkey,&_dialog3DIma,1,0);
		   
		   // Write text into Texture!!!!
		   PA_CenterSmartText16bBuf_RAW(_dialog3DIma.png_screen, 
   					128, 64,
						4, 4,  // base
						128 - 4, 64 - 4, // max
						ds_v_dialogue.string, PA_RGB(0,0,0), 1, 1); // Features		
		   
		   // Texture!
		   _dialog3DTex = ds_3dsprite_loadSpecial(_dialog3DIma.png_screen,
									128,64,
									64,32);
			ds_3dsprite_setPrio(_dialog3DTex, (DS_C_PRIO * DS_C_PRIOLMAX) + 1);
			//ds_3dsprite_setAlpha(_dialog3DTex,192);
		}     
   }   
   
   return 1;
}   

/* Paints dialogue. Also performs part of the management */
int ds_dialogue_paint() {
#ifdef DEBUG_KSDS
	sprintf(ds_global_string,"ST: %d TY: %d MF: %d",
							ds_v_dialogue.state,
							ds_v_dialogue.type,
							ds_v_dialogue.managedFrame);
	ds_gamestatus_debugOutput(1,0,9,ds_global_string,DS_C_STA_DEBUG);	
#endif
	if (!ds_v_dialogue.managedFrame) {
	   // Erase if needed
	   if (ds_v_dialogue.state == DS_C_DIALOGSTATE_ON) {
		   // Texture out... no memory...
			if (_dialog3DIma.loaded) 
   			ds_15bpp_delete(&_dialog3DIma);
   		if (_dialog3DTex != -1)   			
	      	ds_3dsprite_deleteSprite(_dialog3DTex,1);
	      _dialog3DTex = -1;
	   }   
	   // No dialog...
	   ds_v_dialogue.state = DS_C_DIALOGSTATE_OFF;
	   ds_v_dialogue.type = DS_C_ELETYPE_;
	} else {
	   // Check where is Juni, "Paint"!
	   if (ds_global_juni.y < (32 + 48 + 8))
	   	ds_3dsprite_setY(_dialog3DTex,192 - 48 - 32);
	   else
	   	ds_3dsprite_setY(_dialog3DTex,32);
	}         
	return 1;
}   

/* ScrapPad */

/*
			sprintf(ds_global_string,"%-) Tex? %d (%d) [%d]     ",
					(textures[ds_3dsprite_getTextureID(_dialog3DTex)]&(0xFFFF))>>1,
					used_mem3D[(textures[ds_3dsprite_getTextureID(_dialog3DTex)]&(0xFFFF))>>1],
					obj_per_gfx3D[ds_3dsprite_getTextureID(_dialog3DTex)]);
			ds_gamestatus_debugOutput(1,0,9,ds_global_string);
*/
