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
	State_Str = IF Loading First Game (Reset-[1]) World that must be loaded
					IF not Loading First Game, Cutscene that must be played
	State[0] = Type of LOADGAME, see constants @ds_global
	State[1] = Reset (1 - load again the world information, including certain subsystems)
	State[2] = Save Slot. -1 if no save slot	
*/

/*
	NOTE: This state does not follow the behaviour of other states. In particular, 
	some substates (e.g. _DS_MAP2RAW_CHECK) will steal the CPU to perform the required 
	tasks, only returning to main() after the calculations have been finished. 
	As a result, it is necessary to put special calls to _paint() inside the loops
	of those substates.
*/

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_state.h"
#include "ds_world.h"
#include "ds_util.h"
#include "ds_gamestatus.h"
#include "ds_mapfile.h"
#include "ds_input.h"
#include "ds_15bpp.h"
#include "ds_3dspritehdd.h"
#include "PA_Text16bitBuffer.h"

#define _DS_MAP2RAW_CHECK 0
#define _DS_MAP2RAW_READCHECK 1
#define _DS_MAP2RAW_CHECKPNG 2
#define _DS_MAP2RAW_ENDOK 3
#define _DS_MAP2RAW_ENDERR 4
#define _DS_MAP2RAW_WAIT 5

int _ds_map2raw_state;
int _ds_map2raw_finish;
int _ds_map2raw_tilesets[256];
int _ds_map2raw_gradients[256];
int _ds_map2raw_raws;
int _ds_map2raw_forcePNG;
int _ds_map2raw_cntPaint;
int _ds_map2raw_nextVBlank;

/* Put this declaration here to be used by other functions */
void ds_g_map2raw_paint();

/* Checks condition to enter this state */
int ds_g_map2raw_condition(char *level) {
   FILE *ftmp;

   // First, if there is no optimization, do not enter
   if (!ds_global_optimizationUncompress) {
		return 0;
	}
	// Second, check if the raw files are there
	sprintf(ds_global_string,"%s%s/_%s.dat",DS_DIR_MAIN,DS_DIR_RAW,level);
   if (!ds_util_fileExists(ds_global_string)) {
      return 2;
	}	
   // Now, check the state of the files
   sprintf(ds_global_string,"%s%s/%s/Map.bin.dat",DS_DIR_MAIN,DS_DIR_WORLD,level);
   if (ds_util_fileExists(ds_global_string)) {
      // Get Map.bin date
      long dateMapBin = 0;
      sprintf(ds_global_string,"%s%s/%s/Map.bin",DS_DIR_MAIN,DS_DIR_WORLD,level);
      dateMapBin = ds_util_fileGetDate(ds_global_string);
      // Get Map.bin.raw date from map.bin.dat
      long dateMapRaw = 0;
      sprintf(ds_global_string,"%s%s/%s/Map.bin.dat",DS_DIR_MAIN,DS_DIR_WORLD,level);
		ftmp = fopen(ds_global_string, "rb");
		if (ftmp != NULL) {
		   fscanf(ftmp,"%ld",&dateMapRaw);
			fclose(ftmp);
		}
		if (dateMapBin != dateMapRaw) {
		   // The map changed... enter in the new state!
			return 2;   
		} else {
		   // OK, no need to enter in the new state
		   return 0;
		}     
   } else {
      // If the optimization file does not exist, we must enter
      return 1;
   }   
   
   // (To avoid warnings) In any other case, enter optimization
   return 1;
}   

/* Starts this subScreen */
void ds_g_map2raw_start() {
   // First... check (even if the probably did it outside, we must init variables :-P)
   int val = ds_g_map2raw_condition(ds_state_var_getStr());
   if (!val) {
      // Finishing state!
      _ds_map2raw_state = _DS_MAP2RAW_ENDOK;
		return;
   } else {
      // Check the value for forcePNG (if the map was changed, maybe the pngs were changed too)
      _ds_map2raw_forcePNG = (val == 2);
	}      
	
	// Inits the TileRaw variables
	int i;
	for (i = 0; i < 256; i++) {
	   _ds_map2raw_tilesets[i] = 0;
	   _ds_map2raw_gradients[i] = 0;	   
	}      
	_ds_map2raw_raws = 0;
		 
   // Now... continue
   _ds_map2raw_state = _DS_MAP2RAW_CHECK;
   _ds_map2raw_finish = 0;
   _ds_map2raw_forcePNG = 0;
   _ds_map2raw_cntPaint = 0;
   _ds_map2raw_nextVBlank = 0;
   
   // Clear the lower screen and fills standard text...
   ds_global_fillScreen(0,ds_global_getScreen0(),PA_RGB(31,31,31));
   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 0,  // base
										255, 80, // max
										"OPTIMIZING MAP",PA_RGB(0,0,0), 3, 1);
   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 140,  // base
										255, 160, // max
										"Please Wait...",PA_RGB(0,0,0), 2, 1);
}

/* Finish this subScreen */
void ds_g_map2raw_finish() {
   // We had no variables here...
}   

/* Manages the input of the user */
void ds_g_map2raw_input() {
   if (_ds_map2raw_state == _DS_MAP2RAW_WAIT) {
	   // Check if something is pressed...
	   ds_input_getInputPad(&ds_global_input);
	
		// Check...
		if (ds_global_input.Newpress != 0) {
		   _ds_map2raw_finish = 1;
		}   
	}	
}   

/* Manages the state of the game - Check */
void ds_g_map2raw_state_check() {
   FILEMAP gzf;
   FILE *f, *ftmp;
   char dummybuffer[5];
   ds_t_room roombuffer;
   
   // OK, now convert from map.bin to map.bin.raw - opens begin and destination
   sprintf(ds_global_string,"%s%s/%s/Map.bin",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr());
	gzf = ds_mapfile_open(ds_global_string, "rb", DS_C_FILE_BIN);
	if (gzf == NULL) {
	   _ds_map2raw_state = _DS_MAP2RAW_ENDERR;
	   return;
	}
	sprintf(ds_global_string,"%s%s/%s/Map.bin.raw",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr());
	f = fopen(ds_global_string, "wb");
	if (f == NULL) {
	   ds_mapfile_close(gzf);
	   _ds_map2raw_state = _DS_MAP2RAW_ENDERR;
	   return;
	}
	
	// Gets piece from gzf, store in f
	dummybuffer[0] = 0; // Map coord reading stops here :-)
	dummybuffer[1] = 0;
	dummybuffer[2] = 190; // 0xbe;
	dummybuffer[3] = 11; // 0x0b;
	dummybuffer[4] = 0;
	int x,y;
	while (!ds_mapfile_eof(gzf)) {
	   // Get information from one position
	   ds_util_getroomheader(gzf, &x, &y);	   								   
	   if (ds_mapfile_read(gzf,&roombuffer,sizeof(ds_t_room)) != sizeof(ds_t_room)) {
			break; // Sorry, I do not read garbage ^_-
	   }	   
	   // Stores it on the real file
	   fprintf(f,"x%dy%d",x,y); // xNUMyNUM string
	   fwrite(dummybuffer, 1, 5, f); // dummy 00 00 be 0b 00 string
	   fwrite(&roombuffer,1,sizeof(ds_t_room),f); // the room itself
	   // Stores info on _ds_map2raw_tile
	   if (!_ds_map2raw_tilesets[roombuffer.tilesetA]) {
	      _ds_map2raw_tilesets[roombuffer.tilesetA] = 1;
	      _ds_map2raw_raws++;
	   }   
	   if (!_ds_map2raw_tilesets[roombuffer.tilesetB]) {
	      _ds_map2raw_tilesets[roombuffer.tilesetB] = 1;
	      _ds_map2raw_raws++;
	   }   	   
	   if (!_ds_map2raw_gradients[roombuffer.background]) {
	      _ds_map2raw_gradients[roombuffer.background] = 1;
	      _ds_map2raw_raws++;
	   }   
	   // Shall we paint?
	   ds_g_map2raw_paint();
	} 
  	ds_mapfile_close(gzf);
  	fclose(f);
  	
  	// Saves .dat file
   long saveMapBin = 0;
   sprintf(ds_global_string,"%s%s/%s/Map.bin",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr());
   saveMapBin = ds_util_fileGetDate(ds_global_string);
   sprintf(ds_global_string,"%s%s/%s/Map.bin.dat",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr());
	ftmp = fopen(ds_global_string, "wb");
	if (ftmp != NULL) {
	   fprintf(ftmp,"%ld",saveMapBin);
		fclose(ftmp);
	}
  	
  	// Finishes
  	_ds_map2raw_state = _DS_MAP2RAW_CHECKPNG;
}

/* Manages the state of the game - Check */
void ds_g_map2raw_state_checkPng() {
	int isGlobal;
	FILE *ftmp;
	char source_image[255];
	ds_t_15bpp ima;
   int i;
      
	// For every tileset that was marked...
	for (i = 0; i < 256; i++) {
	   // Shall we paint?
	   ds_g_map2raw_paint();
	   // Checked?
	   if (_ds_map2raw_tilesets[i]) {
	      _ds_map2raw_raws--;
	      // OK! Now, check the type of tileset (world-specific or general)
         sprintf(ds_global_string,"%s%s/%s%s/Tileset%d.png",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr(),DS_DIR_TILE,i);
			isGlobal = (!ds_util_fileExists(ds_global_string));
			// Get the name of the raw...
			if (isGlobal)	
				sprintf(ds_global_string,"%s%s/T_%d.raw",DS_DIR_MAIN,DS_DIR_RAW,i);
			else 
				sprintf(ds_global_string,"%s%s/T_%s_%d.raw",DS_DIR_MAIN,DS_DIR_RAW,ds_state_var_getStr(),i);
			// If it doesn't exists (or forced)... create it!
			if ((!ds_util_fileExists(ds_global_string)) || (_ds_map2raw_forcePNG)) {
			   // First, get the name of the original
			   if (isGlobal)
			   	sprintf(source_image,"%s%s/Tileset%d.png",DS_DIR_MAIN,DS_DIR_TILE,i);
			   else
         		sprintf(source_image,"%s%s/%s%s/Tileset%d.png",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr(),DS_DIR_TILE,i);
			   // Now, load and save!!!!!
		   	if (ds_15bpp_load(source_image, &ima, 1, 0)) {
		   	   if (!ds_15bpp_saveRawFileCallback(ds_global_string,&ima,1,&ds_g_map2raw_paint)) {
		   	   //if (!ds_15bpp_saveRawFileCallback(ds_global_string,&ima,1,NULL)) {
		   	      ds_15bpp_delete(&ima);
		   	      _ds_map2raw_state = _DS_MAP2RAW_ENDERR;
		   	   	break;
		   	   }  
					ds_15bpp_delete(&ima); 
		   	} else {
	   	      _ds_map2raw_state = _DS_MAP2RAW_ENDERR;
	   	   	break;		   	   
				}     
			}   
	   }   
	}   
	// Now, for every gradient that was marked...
	for (i = 0; i < 256; i++) {
	   // Shall we paint?
	   ds_g_map2raw_paint();
	   // Checked?
	   if (_ds_map2raw_gradients[i]) {
	      _ds_map2raw_raws--;
	      // OK! Now, check the type of gradient (world-specific or general)
         sprintf(ds_global_string,"%s%s/%s%s/Gradient%d.png",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr(),DS_DIR_GRAD,i);
			isGlobal = (!ds_util_fileExists(ds_global_string));
			// Get the name of the raw...
			if (isGlobal)	
				sprintf(ds_global_string,"%s%s/G_%d.raw",DS_DIR_MAIN,DS_DIR_RAW,i);
			else 
				sprintf(ds_global_string,"%s%s/G_%s_%d.raw",DS_DIR_MAIN,DS_DIR_RAW,ds_state_var_getStr(),i);
			// If it doesn't exists... create it!
			if (!ds_util_fileExists(ds_global_string)) {
			   // First, get the name of the original
			   if (isGlobal)
			   	sprintf(source_image,"%s%s/Gradient%d.png",DS_DIR_MAIN,DS_DIR_GRAD,i);
			   else
         		sprintf(source_image,"%s%s/%s%s/Gradient%d.png",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr(),DS_DIR_GRAD,i);
			   // Now, load and save!!!!!
		   	if (ds_15bpp_load(source_image, &ima, 1, 0)) {
		   	   if (!ds_15bpp_saveRawFileCallback(ds_global_string,&ima,0,&ds_g_map2raw_paint)) {
		   	   //if (!ds_15bpp_saveRawFileCallback(ds_global_string,&ima,0,NULL)) {
		   	      ds_15bpp_delete(&ima);
		   	      _ds_map2raw_state = _DS_MAP2RAW_ENDERR;
		   	   	break;
		   	   }  
					ds_15bpp_delete(&ima); 
		   	} else {
	   	      _ds_map2raw_state = _DS_MAP2RAW_ENDERR;
	   	   	break;		   	   
				}     
			}   
	   }   
	}   
	
	// Now, point out that we saved the raws
	sprintf(ds_global_string,"%s%s/_%s.dat",DS_DIR_MAIN,DS_DIR_RAW,ds_state_var_getStr());
	ftmp = fopen(ds_global_string, "wb");
	if (ftmp != NULL) {
	   fprintf(ftmp,"%d",0);
		fclose(ftmp);
	}

  	_ds_map2raw_state = _DS_MAP2RAW_ENDOK;
}      

/* Manages the state of the game - OK! */
void ds_g_map2raw_state_ok() {
   // Just finishes...
	_ds_map2raw_state = _DS_MAP2RAW_WAIT;
	_ds_map2raw_finish = 1;
}

/* Manages the state of the game - OK! */
void ds_g_map2raw_state_err() {
   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 168,  // base
										255, 191, // max
										"No Space in HDD! Delete files in your flashcart!\n Touch - Press anything...",PA_RGB(31,0,0), 1, 1); // Features
   ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
	_ds_map2raw_state = _DS_MAP2RAW_WAIT;
}

/* Manages the state of the game - wait button - anything... */
void ds_g_map2raw_state_wait() {
	// If finish...
   if (_ds_map2raw_finish) {
		//ds_state_var_setStr(...); - no need, it is already assigned... 
		ds_state_assignState(DSKNYTT_LOADMENU);
	}	   
}

/* Manages the state of the game */
void ds_g_map2raw_state() {
   switch (_ds_map2raw_state) {
      case _DS_MAP2RAW_CHECK:
         ds_g_map2raw_state_check();
         break;
      case _DS_MAP2RAW_CHECKPNG:
         ds_g_map2raw_state_checkPng();
         break;
      case _DS_MAP2RAW_ENDERR:
         ds_g_map2raw_state_err();
         break;
      case _DS_MAP2RAW_ENDOK:
         ds_g_map2raw_state_ok();
         break;
      case _DS_MAP2RAW_WAIT:
         ds_g_map2raw_state_wait();
         break;
   }   
}

/* Paints this actual game state */
void ds_g_map2raw_paint() {
   u16 *ima;
   ds_t_15bpp ima15bpp;
   // Check if we allow to paint
   if ((PA_GetVcount() >= 0) && (PA_GetVcount() <= 192)) {
		_ds_map2raw_nextVBlank = 1; // When we arrive to the next VBlank, we can paint
   }   
	// Check if we are in state for painting something (after VBlank!)
	if ((PA_GetVcount() > 192) && (_ds_map2raw_nextVBlank)) {
	   _ds_map2raw_nextVBlank = 0; // Do not paint this anymore on this cycle
	   // Angle!
	   _ds_map2raw_cntPaint += 3; // One per 1/2 second
	   if (_ds_map2raw_cntPaint >= 360)
	   	_ds_map2raw_cntPaint = 0;
	   // Clear the lower screen...
		ds_global_fillScreenRange(0,ds_global_getScreen0(),PA_RGB(31,31,31),50,160);
      // Write Juni
      ima = ds_3dspritehdd_getSprite(DS_C_JUNI_BANK, DS_C_JUNI_ST_STOP_R, 0);
      ds_15bpp_initRaw(&ima15bpp,ima,24,24,1);
      ds_15bpp_putScreenAlpha(ds_global_getScreen0(),&ima15bpp,
			(256 >> 1) - (24 >> 1), (192 >> 1) - (24 >> 1));
      // Get the moving particles & other stuff
      if (_ds_map2raw_state == _DS_MAP2RAW_CHECK) {
         ima = ds_3dspritehdd_getSprite(DS_C_JUNI_BANKSP, DS_C_JUNI_SP_CYANGLOW, 0);
         sprintf(ds_global_string,"Please Wait (map)...");
      } else if (_ds_map2raw_state != _DS_MAP2RAW_WAIT) {
         ima = ds_3dspritehdd_getSprite(DS_C_JUNI_BANKSP, DS_C_JUNI_SP_CYANGLOW, 0);
         sprintf(ds_global_string,"Please Wait (%d to go)...",_ds_map2raw_raws);
		} else {
         ima = ds_3dspritehdd_getSprite(DS_C_JUNI_BANKSP, DS_C_JUNI_SP_REDGLOW, 0);
         sprintf(ds_global_string,"Optimization failed:");
		}        
      ds_15bpp_initRaw(&ima15bpp,ima,24,24,1);
      // Paint the moving particles
      int d = 32; // Distance from the center of Juni
      int dy = (int)(ds_util_fastSin(_ds_map2raw_cntPaint) * d);
      int dx = (int)(ds_util_fastCos(_ds_map2raw_cntPaint) * d);
      ds_15bpp_putScreenAlpha(ds_global_getScreen0(),&ima15bpp,
			(256 >> 1) - (24 >> 1) + dx, (192 >> 1) - (24 >> 1) + dy);      
		// Paint the countdown
	   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
											0, 140,  // base
											255, 160, // max
											ds_global_string,PA_RGB(0,0,0), 2, 1);

      // Paint
	   ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
   }   
}
