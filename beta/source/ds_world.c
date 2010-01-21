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

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_ini.h"
#include "ds_util.h"
#include "ds_mapfile.h"
#include "ds_customobj.h"
#include "ds_gamestatus.h"


//-------------------------------------------------------------------------------------------------
// INTERNAL VARIABLES
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------

int _ds_world_opt_loadMapIndex() {
   FILEMAP gzf;
   int xx, yy;
   u32 offset;
   ds_t_room room;
   int i;
   int d = 0;

	sprintf(ds_global_string,"%s/Map.bin",ds_global_world.dir);
	gzf = ds_mapfile_open(ds_global_string, "rb", DS_C_FILE_AUTO);
	if (gzf == NULL) {
	   ds_global_errorAssign(DS_C_ERR_NOMAPBIN);
		ds_global_errorHalt(ds_global_world.dir);
		//--HALT--//
	}
		
	while (!ds_mapfile_eof(gzf)) {
	   //sprintf(ds_global_string,"(%d)",++d); // DEBUG
	   // Advance one position
	   //PA_OutputText(1,0,0,ds_global_string); // DEBUG
	   ds_util_getroomheader(gzf, &xx, &yy);
	   //PA_OutputText(1,0,1,ds_global_string); // DEBUG
	   offset = ds_mapfile_tell(gzf);
	   //PA_OutputText(1,0,2,ds_global_string); // DEBUG
	   if (ds_mapfile_read(gzf,&room,sizeof(ds_t_room)) != sizeof(ds_t_room)) {
	      //ds_mapfile_close(gzf);
	      //return 0;
	      if (!ds_mapfile_eof(gzf)) {
	      	sprintf(ds_global_string,"%d/%d (%d/%d)",xx,yy,d,sizeof(ds_t_room));
	      	ds_global_errorHalt(ds_global_string);
	   	}   
	   }	   
	   //PA_OutputText(1,0,3,ds_global_string); // DEBUG
	   // Store in the structure
	   int cy = yy - 800;
	   int cx = xx - 800;
	   if ((cy < 0) || (cy > 400) || (cx < 0) || (cx > 400)) {
			   ds_global_errorAssign(DS_C_ERR_NOMAPBIN);
			   sprintf(ds_global_string,"Map.bin too big. Sorry!");
			   ds_global_errorHalt(ds_global_string);
			   //--HALT--//   	   	   	      
	   }   
	   if (ds_global_world.opt_mapIndex[yy - 800] == NULL) {
	   	ds_global_world.opt_mapIndex[yy - 800] = malloc(sizeof(ds_t_world_opt_mapindex));
	   	if (ds_global_world.opt_mapIndex[yy - 800] == NULL) {
	   	   // WTF!!!!!!!!!!!!!!!!!!!!!!!!!
			   ds_global_errorAssign(DS_C_ERR_NOMEMORY);
			   sprintf(ds_global_string,"CALL opt_loadMapIndex\n[%d]",yy);
			   ds_global_errorHalt(ds_global_string);
			   //--HALT--//   	   	   
	   	}   
	   	for (i=0; i < 400; i++) {
	   	   ds_global_world.opt_mapIndex[yy - 800]->offset[i] = -1;
	   	}   
		}  	
		//PA_OutputText(1,0,4,ds_global_string); // DEBUG
	   ds_global_world.opt_mapIndex[yy - 800]->offset[xx - 800] = offset;
	   //PA_OutputText(1,0,5,ds_global_string); // DEBUG
	}
	//PA_OutputText(1,0,6,"Closing"); // DEBUG
	ds_mapfile_close(gzf);
	//PA_OutputText(1,0,7,"Closed"); // DEBUG

	return 1;   
}    

void _ds_world_opt_destroyMapIndex() {
   int i;
   for (i=0; i < 400;i++) {
      if (ds_global_world.opt_mapIndex[i] != NULL) {
         free(ds_global_world.opt_mapIndex[i]);
         ds_global_world.opt_mapIndex[i] = NULL;
      }   	
   }   
}   

void *_ds_world_getSavegame(int saveslot) {
   void *savegame;
   // Now loads, and store the normal savegame. NOTE THAT if saveslot == -1, we load the basic game ;-)
   sprintf(ds_global_string,"%s%s/%s %d.ini",DS_DIR_MAIN,DS_DIR_SAVE,ds_global_world.basicdir,saveslot);
   if (!ds_util_fileExists(ds_global_string))
   	sprintf(ds_global_string,"%s/DefaultSavegame.ini",ds_global_world.dir);
   savegame = ds_ini_load(ds_global_string);
   if (savegame == NULL) {
      ds_global_errorAssign(DS_C_ERR_NOSAVEINI);
      return NULL;
   }
   return savegame;
}

/* Load Information related to the visited maps */
void _ds_world_loadVisited(void *savegame) {
   char _string[16];
	// First, cleans
   int i,j;
   for (i=0; i < 400; i++) {
      for (j=0; j < 50; j++) {
   		ds_global_world.opt_mapVisited[i].offset[j] = 0;
  		}
  		ds_global_world.opt_mapVisitedRow[i] = 0;
	}	   		

	// Now, loads
   for (i=0; i < 400; i++) {
      sprintf(_string,"Minimap(%d):Row",800 + i);
      strncpy(ds_global_string,ds_ini_getstring(savegame,_string,""),256);
      if (!PA_CompareText("",ds_global_string)) {
         ds_global_world.opt_mapVisitedRow[i] = 1;
         ds_util_str2bin(ds_global_world.opt_mapVisited[i].offset, ds_global_string);
      }   
   }
}

/* Save Information related to the visited maps */
void _ds_world_saveVisited(FILE *file) {
	
	int i;  
	
	// First and Last, saves
   for (i=0; i < 400; i++) {
  		if (ds_global_world.opt_mapVisitedRow[i]) {
  		   fprintf(file,"[Minimap(%d)]\r\n",800 + i); 
  		   ds_util_bin2str(ds_global_string, ds_global_world.opt_mapVisited[i].offset, 50);
  		   fprintf(file,"Row=%s\r\n",ds_global_string);
  		}   
	}
}      

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Inits the world structure */
void ds_world_init() {
   // Normal systems
   ds_global_world.loaded = 0;
   ds_global_world.worldini = NULL;
   ds_global_world.saveslot = 1;
   
   // Optimization subsystems
   int i;
   for (i=0; i < 400; i++) {
      ds_global_world.opt_mapIndex[i] = NULL;
   }   
   int j;
   for (i=0; i < 400; i++) {
      for (j=0; j < 50; j++) {
   		ds_global_world.opt_mapVisited[i].offset[j] = 0;
  		}
  		ds_global_world.opt_mapVisitedRow[i] = 0;
	}	   		
   
   // "Came from shift" and "ignore shift" systems - Init
   ds_global_world.shift = 0;
   ds_global_world.shift_cameFrom = 0;
   ds_global_world.shiftNoX = -1;
   ds_global_world.shiftNoY = -1;
   
   // CO subsystem
   ds_customobj_init();
}   

/* Load a certain world information into memory */
int ds_world_load(const char *world, int saveslot) {
   char string[255];
   int i;
	int flag;
	void *savegame;
   
   // Copy world information into directory
   sprintf(ds_global_world.dir,"%s%s/%s",DS_DIR_MAIN,DS_DIR_WORLD,world);
   sprintf(ds_global_world.basicdir,"%s",world);
   
   // First, load the "world" information into its structure, including CO
   sprintf(string,"%s/World.ini",ds_global_world.dir);
   ds_global_world.worldini = ds_ini_load(string);
   if (ds_global_world.worldini == NULL) {
      ds_global_errorAssign(DS_C_ERR_NOWORLDINI);
      ds_global_errorHalt(ds_ini_getError());
      //--HALT--//
   }   

   strncpy(ds_global_world.author,ds_ini_getstring(ds_global_world.worldini,"World:Author",""),250);
   strncpy(ds_global_world.name,ds_ini_getstring(ds_global_world.worldini,"World:Name",""),250);
   
   ds_customobj_load();

   // Now loads, and store the normal savegame
   savegame = _ds_world_getSavegame(saveslot);
   if (savegame == NULL) {
      ds_global_errorAssign(DS_C_ERR_NOSAVEINI);
		ds_global_errorHalt(ds_global_world.dir);
		//--HALT--//
   }
   
   ds_global_world.sv_xmap = ds_ini_getint(savegame,"Positions:X Map",1000);
   ds_global_world.sv_ymap = ds_ini_getint(savegame,"Positions:Y Map",1000);
   ds_global_world.sv_xjuni = ds_ini_getint(savegame,"Positions:X Pos",12);
   ds_global_world.sv_yjuni = ds_ini_getint(savegame,"Positions:Y Pos",5);
   ds_global_world.sv_gui = ds_ini_getint(savegame,"Positions:Gui",ds_gamestatus_getActualStatusScreen());
   ds_global_world.sv_items = 0;
   for (i = 0; i < DS_C_JUNI_IT_MAX_; i++) {
      sprintf(string,"Powers:Power%d",i);
      if (ds_ini_getint(savegame,string,0))
      	ds_global_world.sv_items = ds_util_bitSet16(ds_global_world.sv_items,i);
   }   
   ds_global_world.sv_flags = 0;
   i = 0;
   sprintf(string,"Flags:Flag%d",i);
   flag = ds_ini_getint(savegame,string,-1);
   while (flag != -1) {
      if (flag)
	      ds_global_world.sv_flags = ds_util_bitSet16(ds_global_world.sv_flags,i);
      i++;	      
	   sprintf(string,"Flags:Flag%d",i);
   	flag = ds_ini_getint(savegame,string,-1);      
   } 
   
   // Finally, loads the "visited" information
   _ds_world_loadVisited(savegame);
   
   // Destroy, specify saveslot
	ds_ini_destroy(savegame);	   
	savegame = NULL;
	
	ds_global_world.saveslot = (saveslot == -1)?1:saveslot; // From now on, this is the REAL Saveslot	
   
   // Time for the optimization subsystems! 
   // Map offsets - load the map offsets in the Map.bin (zip) files
   if (!_ds_world_opt_loadMapIndex()) {
      ds_global_errorAssign(DS_C_ERR_ZIPFAST);
		sprintf(ds_global_string,"CALL _ds_world_opt_loadMapIndex()");
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
 	}  	
 	
   // "Came from shift" systems - Init
   ds_global_world.shift_cameFrom = 0;
      
	return 1;   
}   

/* Updates the savegame, both in memory and in HDD - Parametrized */
int ds_world_updateSavegameParam(int mapx, int mapy, int junix, int juniy, u16 items, u16 flags, int gui) {
   int i;
   
   // Memory save
   ds_global_world.sv_xmap = mapx;
   ds_global_world.sv_ymap = mapy;
   ds_global_world.sv_xjuni = junix / 24;
   ds_global_world.sv_yjuni = juniy / 24;
   ds_global_world.sv_items = items;
   ds_global_world.sv_flags = flags;
   ds_global_world.sv_gui = gui;
   
   // Structure and disk save - DIRECTLY O_o (Faster that using the ini library) [OPT]
	FILE *file;
	sprintf(ds_global_string,"%s%s/%s %d.ini",DS_DIR_MAIN,DS_DIR_SAVE,ds_global_world.basicdir,ds_global_world.saveslot);
   file = fopen(ds_global_string,"w");
   if (file == NULL) {
      ds_global_errorAssign(DS_C_ERR_NOHDD);
		sprintf(ds_global_string,"(No space in flashcart / write error)");
		ds_global_errorHalt(ds_global_string);
		//--HALT--//
   }   
   
   fprintf(file,"[Positions]\r\n");
   fprintf(file,"X Map=%d\r\n",ds_global_world.sv_xmap);
   fprintf(file,"Y Map=%d\r\n",ds_global_world.sv_ymap);
   fprintf(file,"X Pos=%d\r\n",ds_global_world.sv_xjuni);
   fprintf(file,"Y Pos=%d\r\n",ds_global_world.sv_yjuni);
   fprintf(file,"Gui=%d\r\n",ds_global_world.sv_gui);
   fprintf(file,"[World]\r\n");   
   fprintf(file,"World Folder=%s\r\n",ds_global_world.basicdir);   
   fprintf(file,"[Powers]\r\n");   
   for (i = 0; i < DS_C_JUNI_IT_MAX_; i++) {
      fprintf(file,"Power%d=%d\r\n",i,ds_util_bitOne16(ds_global_world.sv_items,i));
   }   
   fprintf(file,"[Flags]\r\n");   
   for (i = 0; i < DS_C_JUNI_FLAG_MAX_; i++) {
      fprintf(file,"Flag%d=%d\r\n",i,ds_util_bitOne16(ds_global_world.sv_flags,i));
   }      
   fprintf(file,"\r\n");      
   
   _ds_world_saveVisited(file); // Saves the "visited" information
   
   fclose(file);
   
   return 1;
}

/* Updates the savegame, both in memory and in HDD - Basic parameters */
int ds_world_updateSavegameBasic(int x, int y) {
   return ds_world_updateSavegameParam(ds_global_map.x, 
											ds_global_map.y,
											x,
											y,
											ds_global_juni.item,
											ds_global_juni.flag,
											ds_gamestatus_getActualStatusScreen());
}   

/* Updates the savegame, both in memory and in HDD - No parameters */
int ds_world_updateSavegame() {
   return ds_world_updateSavegameParam(ds_global_map.x, 
											ds_global_map.y,
											ds_global_juni.x,
											ds_global_juni.y,
											ds_global_juni.item,
											ds_global_juni.flag,
											ds_gamestatus_getActualStatusScreen());
}   

/* Returns the offset of a certain room in the Map.bin file */
u32 ds_world_getOffsetRoom(int x, int y) {
   // Check first
   int cy = y - 800;
   int cx = x - 800;
   if ((cy < 0) || (cy > 400) || (cx < 0) || (cx > 400)) {
		   ds_global_errorAssign(DS_C_ERR_NOMAPBIN);
		   sprintf(ds_global_string,"Map.bin too big. Sorry!");
		   ds_global_errorHalt(ds_global_string);
		   //--HALT--//   	   	   	      
   }   
	// Execute the function
   if (ds_global_world.opt_mapIndex[y - 800] == NULL)
   	return -1;
   return ds_global_world.opt_mapIndex[y - 800]->offset[x - 800];
}

/* Returns if a room has been visited or not */
int ds_world_getVisitedRoom(int x, int y) {
   // Check first
   int cy = y - 800;
   int cx = x - 800;
   if ((cy < 0) || (cy > 400) || (cx < 0) || (cx > 400)) {
		   ds_global_errorAssign(DS_C_ERR_NOMAPBIN);
		   sprintf(ds_global_string,"Map.bin too big. Sorry!");
		   ds_global_errorHalt(ds_global_string);
		   //--HALT--//   	   	   	      
   }   
	// Execute the function
   return ds_util_bitOne8(
   			ds_global_world.opt_mapVisited[y - 800].offset[(x - 800) / 8],
   			(x - 800) % 8);
}

/* Sets a room as visited */
void ds_world_setVisitedRoom(int x, int y) {
   // Check first
   int cy = y - 800;
   int cx = x - 800;
   if ((cy < 0) || (cy > 400) || (cx < 0) || (cx > 400)) {
		   ds_global_errorAssign(DS_C_ERR_NOMAPBIN);
		   sprintf(ds_global_string,"Map.bin too big. Sorry!");
		   ds_global_errorHalt(ds_global_string);
		   //--HALT--//   	   	   	      
   }   
	// Execute the function
   ds_global_world.opt_mapVisited[y - 800].offset[(x - 800) / 8] = 
   	ds_util_bitSet8(
   			ds_global_world.opt_mapVisited[y - 800].offset[(x - 800) / 8],
   			(x - 800) % 8);
   			
   ds_global_world.opt_mapVisitedRow[y - 800] = 1;
}


/* Destroys a certain world information from memory */
void ds_world_destroy() {
   // Normal
   if (ds_global_world.worldini != NULL)
	   ds_ini_destroy(ds_global_world.worldini);
	   
	// Optimization
	_ds_world_opt_destroyMapIndex();
	
	// CO!
	ds_customobj_reset();
}
