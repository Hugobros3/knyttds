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

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_state.h"
#include "ds_world.h"
#include "ds_util.h"
#include "ds_gamestatus.h"
#include "ds_mapfile.h"
#include "ds_input.h"
#include "ds_15bpp.h"
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

/* Starts this subScreen */
void ds_g_map2raw_start() {
   // First... check 
   if (!ds_global_optimizationUncompress) {
		// No need :-) - ds_state_var_setStr(level->dir); 
		ds_state_assignState(DSKNYTT_LOADMENU);
		return;
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
   
   // Clear the lower screen...
   ds_global_fillScreen(0,ds_global_getScreen0(),PA_RGB(31,31,31));
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
   FILE *f,*ftmp;
   char dummybuffer[5];
   ds_t_room roombuffer;
   
   // First, say what you are going to do at this stage...
   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 0,  // base
										255, 80, // max
										"OPTIMIZATION A)\n Saving uncompressed map\n Please Wait...",PA_RGB(0,0,0), 2, 1); // Features
   ds_global_paintScreen(0,ds_global_getScreen0(),0,0);

   
   // OK, but check if the map already exists...
   // Note - this is already checked through the map.bin.dat check
   //sprintf(ds_global_string,"%s%s/%s/Map.bin.raw",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr());
   //if (ds_util_fileExists(ds_global_string)) {
	//   _ds_map2raw_state = _DS_MAP2RAW_ENDOK; // TODO - READCHECK
	//   return;
   //}
   
   // Now, check the date of the Map.bin file...
   sprintf(ds_global_string,"%s%s/%s/Map.bin.dat",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr());
   if (ds_util_fileExists(ds_global_string)) {
      // Get Map.bin date
      long dateMapBin = 0;
      sprintf(ds_global_string,"%s%s/%s/Map.bin",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr());
      dateMapBin = ds_util_fileGetDate(ds_global_string);
      // Get Map.bin.raw date
      long dateMapRaw = 0;
      sprintf(ds_global_string,"%s%s/%s/Map.bin.dat",DS_DIR_MAIN,DS_DIR_WORLD,ds_state_var_getStr());
		ftmp = fopen(ds_global_string, "rb");
		if (ftmp != NULL) {
		   fscanf(ftmp,"%ld",&dateMapRaw);
			fclose(ftmp);
		}
		if (dateMapBin != dateMapRaw) {
		   // Backup belongs to old level - update EVERYTHING
			_ds_map2raw_forcePNG = 1;   
		} else {
		   // OK, no need to continue
			_ds_map2raw_state = _DS_MAP2RAW_ENDOK; // TODO - READCHECK
			return;
		}     
   }
   
   // OK, from map.bin to map.bin.raw - opens begin and destination
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
	char source_image[255];
	ds_t_15bpp ima;
   int i;
   
   // First, say what you are going to do at this stage...
   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 80,  // base
										255, 160, // max
										"OPTIMIZATION B)\n Saving uncompressed tilesets\n Please Wait...",PA_RGB(0,0,0), 2, 1); // Features
   ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
   
	// For every tileset that was marked...
	for (i = 0; i < 256; i++) {
	   // Checked?
	   if (_ds_map2raw_tilesets[i]) {
	      sprintf(ds_global_string,"...Converting T %d...",i);
		   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 160,  // base
										255, 191, // max
										ds_global_string,PA_RGB(31,31,31), 2, 0); // Features
		   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 160,  // base
										255, 191, // max
										ds_global_string,PA_RGB(0,0,0), 2, 1); // Features
			ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
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
		   	   if (!ds_15bpp_saveRawFile(ds_global_string,&ima,1)) {
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
	   // Checked?
	   if (_ds_map2raw_gradients[i]) {
	      sprintf(ds_global_string,"...Converting G %d...",i);
		   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 160,  // base
										255, 191, // max
										ds_global_string,PA_RGB(31,31,31), 2, 0); // Features
		   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 160,  // base
										255, 191, // max
										ds_global_string,PA_RGB(0,0,0), 2, 1); // Features
			ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
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
		   	   if (!ds_15bpp_saveRawFile(ds_global_string,&ima,0)) {
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

  	_ds_map2raw_state = _DS_MAP2RAW_ENDOK;
}      

/* Manages the state of the game - OK! */
void ds_g_map2raw_state_ok() {
   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 168,  // base
										255, 191, // max
										"Successful!\n Touch - Press anything...",PA_RGB(0,0,0), 2, 1); // Features
   ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
	_ds_map2raw_state = _DS_MAP2RAW_WAIT;
}

/* Manages the state of the game - OK! */
void ds_g_map2raw_state_err() {
   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 168,  // base
										255, 191, // max
										"Unsuccessful (but you can continue)\n Touch - Press anything...",PA_RGB(31,0,0), 2, 1); // Features
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
	// Nothing to do here...
}
