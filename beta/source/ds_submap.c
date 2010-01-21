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

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_util.h"
#include "ds_15bpp.h"
#include "ds_world.h"
#include "ds_map.h"
#include "ds_mapfile.h"

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES
//-------------------------------------------------------------------------------------------------

typedef struct _ds_tt_roomPreload {
   ds_t_room room;
   int x;
   int y;
   u32 offset;   
   int loaded;
} _ds_t_roomPreload;   


//-------------------------------------------------------------------------------------------------
// INTERNAL VARS
//-------------------------------------------------------------------------------------------------

_ds_t_roomPreload _ds_submap_room[4]; // 0 N - 1 E - 2 S - 3 W. <TODO> Shifts
int _ds_submap_roomLoaded;
int _ds_submap_roomState;
u32 _ds_submap_roomMinor;
int _ds_submap_roomMinorID;
FILEMAP _ds_submap_gzf;

//-------------------------------------------------------------------------------------------------
// MAPS
//-------------------------------------------------------------------------------------------------

int _ds_submap_getRoomOpt(int x, int y, ds_t_room *room) {
   int i;
   for (i = 0; i < 4; i++) {
		if ((x == _ds_submap_room[i].x) && (y == _ds_submap_room[i].y) && (_ds_submap_room[i].loaded)) {
		   (*room) = _ds_submap_room[i].room;
		   return 1;
		}   
   }   
   
   return 0;
}   

int ds_submap_loadRoom(int x, int y, ds_t_room *room) {
   FILEMAP gzf;
   int xx, yy;
   char string[255];
   int loaded = 0;
   
   // Cache...
   if (_ds_submap_getRoomOpt(x, y, room))
   	return 1;
   	
	sprintf(string,"%s/Map.bin",ds_global_world.dir);
	gzf = ds_mapfile_open(string, "rb", DS_C_FILE_AUTO);
	if (gzf == NULL) {
	   ds_global_errorAssign(DS_C_ERR_NOMAPBIN);
		ds_global_errorHalt(ds_global_world.dir);
	   //--HALT--//
	}

	//	Load by Index...
   u32 offset = ds_world_getOffsetRoom(x,y);
   if (offset != -1) {
		ds_mapfile_seek(gzf,offset,SEEK_SET); 
   	if (ds_mapfile_read(gzf,(void *) room,sizeof(ds_t_room)) == sizeof(ds_t_room)) {
   	   loaded = 1;   // Good! we found it on the index table
	   }
	}   
	
	// Load by Seek...
	if (loaded == 0) { // ??????????... OK, we will search for it...
		ds_mapfile_seek(gzf,0,SEEK_SET); 
		while (!ds_mapfile_eof(gzf)) {
		   // Advance one position
		   ds_util_getroomheader(gzf, &xx, &yy);
		   								   
		   if (ds_mapfile_read(gzf,(void *)room,sizeof(ds_t_room)) != sizeof(ds_t_room)) {
				break;
		   }	   
		   
			if ((x == xx) && (y == yy)) {
			   loaded = 1;
			   break; // Good! we found it by doing sequential search
			}			   
		}
	}	
	
	// Ends and returns...	
	ds_mapfile_close(gzf);
	return loaded;
}

void ds_submap_resetRoom(ds_t_room *room) {
   int xx, yy, zz;
   room->tilesetA = 0;
   room->tilesetB = 0;
   room->atmosA = 0;
   room->atmosB = 0;
   room->music = 0;
   room->background = 0;
   for (zz = 0; zz < 4; zz++) {
	   for (yy = 0; yy < 10; yy++) {
		   for (xx = 0; xx < 25; xx++) {
		      room->tilelayer[zz].tile[yy][xx] = 0;
		      room->objlayer[zz].obj[yy][xx] = 0;
		      room->objlayer[zz].bank[yy][xx] = 0;
   		}   	      
   	}   
   }   
}

/* Starts optimization subsystems */
void ds_submap_initOpt() {
   int i;
   for (i = 0; i < 4; i++) {
      _ds_submap_room[i].loaded = 0;
   }   
   _ds_submap_roomLoaded = 0;
   _ds_submap_roomState = 0;
   _ds_submap_roomMinor = 0xffffffff; // u32 max
   _ds_submap_roomMinorID = -1;   
   _ds_submap_gzf = NULL;
}   

/* Performs optimizations */
/* A) Preloads maps */
void ds_submap_doOpt() {
   int x;
   int y;   
   int i;

	switch (_ds_submap_roomState) {
	   case 0:
	      /* GET OFFSETS - INITS STUFF */	      
	      for (i = 0; i < 4; i++) {
		   	x = ds_global_map.x;
			   y = ds_global_map.y;  	         
			   ds_map_getBoundary(i,&x,&y);
			   _ds_submap_room[i].x = x;
			   _ds_submap_room[i].y = y;
			   _ds_submap_room[i].offset = ds_world_getOffsetRoom(_ds_submap_room[i].x,_ds_submap_room[i].y);
	      }   
	      
	      _ds_submap_roomState = 1;
	      
	      break;
	   case 1:
	      /* GET MINOR MAP (not loaded) */
	      _ds_submap_roomMinor = 0xffffffff; // u32 max
	      _ds_submap_roomMinorID = -1;	      
	      
	      for (i = 0; i < 4; i++) {
	         if ((_ds_submap_room[i].offset != -1) && (!_ds_submap_room[i].loaded)) {
	            if (_ds_submap_room[i].offset < _ds_submap_roomMinor) {
		            _ds_submap_roomMinor = _ds_submap_room[i].offset;
	            	_ds_submap_roomMinorID = i;
	          	}  	
	         }   
	      }   
	      if (_ds_submap_roomMinorID == -1)
	      	_ds_submap_roomState = 4; // Do nothing...
	      else 
	      	_ds_submap_roomState = 2; // (Pre)Load!!!	      	
	      break;	      
	   case 2:
	      /* OPEN FILE */
	      if (_ds_submap_gzf == NULL) {
				sprintf(ds_global_string,"%s/Map.bin",ds_global_world.dir);
				_ds_submap_gzf = ds_mapfile_open(ds_global_string, "rb", DS_C_FILE_AUTO);
				if (_ds_submap_gzf == NULL) {
					_ds_submap_roomState = 4; // Do nothing...
					break;
				}
			}			
			_ds_submap_roomState = 3;	      
	      
	      break;
	   case 3:
	      /* SEARCH MAPS */   
	      x = -1;
	      y = -1;
			if (!ds_mapfile_eof(_ds_submap_gzf)) {
			   // Advance one position
			   ds_util_getroomheader(_ds_submap_gzf, &x, &y);
			   								   
			   if (ds_mapfile_read(_ds_submap_gzf,&_ds_submap_room[_ds_submap_roomMinorID].room,sizeof(ds_t_room)) != sizeof(ds_t_room)) {
					_ds_submap_roomState = 4; // Do nothing...
					break;
			   }	   
				if ((x == _ds_submap_room[_ds_submap_roomMinorID].x) && (y == _ds_submap_room[_ds_submap_roomMinorID].y)) {
				   _ds_submap_room[_ds_submap_roomMinorID].loaded = 1;
				   _ds_submap_roomState = 1; // Search Next...
				}			   
			} else {
			   _ds_submap_roomState = 4; // Do nothing...
			}   
	      break;	      
	   case 4:
	      /* NOTHING TO DO... */
	      if (_ds_submap_gzf != NULL) {
				ds_mapfile_close(_ds_submap_gzf);
				_ds_submap_gzf = NULL;
			}				      
	      _ds_submap_roomLoaded = 1;			
	      break;	      
	} 	
}   

int ds_submap_inOpt() {
   return (!_ds_submap_roomLoaded);
}

/* Finishes the optimization process - even if it didn't provide any results */
void ds_submap_endOpt() {
   // Close the .map file (if it was open!!!)
   if (_ds_submap_gzf != NULL) {
		ds_mapfile_close(_ds_submap_gzf);
		_ds_submap_gzf = NULL;
	}				      
}      


//-------------------------------------------------------------------------------------------------
// TILESETS
//-------------------------------------------------------------------------------------------------

/* Deletes a tileset from the subsystem */
void ds_submap_deleteTileset(int tilesetType, ds_t_15bpp *ima) {
   // Simple Tileset Deletion
   ima->actualIndex = -1;
   ds_15bpp_delete(ima);
}   

/* Load a certain tileset from the subsystem */
int ds_submap_loadTileset(int tilesetType, u8 tileset, ds_t_15bpp *ima, int ispartial) {
   char filen[255];
   int isPng = 1;

	/* First, check if a raw exists */
	if ((tilesetType == DS_C_TILESETA) || (tilesetType == DS_C_TILESETB)) {
	   // Check first the type of Tileset
	   int isGlobal;
      sprintf(filen,"%s%s/Tileset%d.png",ds_global_world.dir,DS_DIR_TILE,tileset);
		isGlobal = (!ds_util_fileExists(filen));
		// Now check the existence of a RAW
		if (isGlobal)	
			sprintf(filen,"%s%s/T_%d.raw",DS_DIR_MAIN,DS_DIR_RAW,tileset);
		else 
			sprintf(filen,"%s%s/T_%s_%d.raw",DS_DIR_MAIN,DS_DIR_RAW,ds_global_world.basicdir,tileset);
		isPng = (!ds_util_fileExists(filen));
	} else if (tilesetType == DS_C_PANORAMA) {
	   // Check first the type of Gradient
	   int isGlobal;
      sprintf(filen,"%s%s/Gradient%d.png",ds_global_world.dir,DS_DIR_GRAD,tileset);
		isGlobal = (!ds_util_fileExists(filen));
		// Now check the existence of a RAW
		if (isGlobal)	
			sprintf(filen,"%s%s/G_%d.raw",DS_DIR_MAIN,DS_DIR_RAW,tileset);
		else 
			sprintf(filen,"%s%s/G_%s_%d.raw",DS_DIR_MAIN,DS_DIR_RAW,ds_global_world.basicdir,tileset);
		isPng = (!ds_util_fileExists(filen));
	}     
	
   /* Prepare file reading */
   if (isPng) {
	   switch (tilesetType) {
	      case DS_C_TILESETA:
	         sprintf(filen,"%s%s/Tileset%d.png",ds_global_world.dir,DS_DIR_TILE,tileset);
				if (!ds_util_fileExists(filen))
					sprintf(filen,"%s%s/Tileset%d.png",DS_DIR_MAIN,DS_DIR_TILE,tileset);
	         break;
	      case DS_C_TILESETB:
	         sprintf(filen,"%s%s/Tileset%d.png",ds_global_world.dir,DS_DIR_TILE,tileset);
				if (!ds_util_fileExists(filen))         
					sprintf(filen,"%s%s/Tileset%d.png",DS_DIR_MAIN,DS_DIR_TILE,tileset);			
	         break;
	      case DS_C_PANORAMA:
	         sprintf(filen,"%s%s/Gradient%d.png",ds_global_world.dir,DS_DIR_GRAD,tileset);
				if (!ds_util_fileExists(filen))                  
		         sprintf(filen,"%s%s/Gradient%d.png",DS_DIR_MAIN,DS_DIR_GRAD,tileset);			
	         break;
	   }
	}	   

	/* Load and Convert image */   		
	if (!ds_15bpp_load(filen, ima, isPng, ispartial)) // FIXME - Implement partial load
		return 0; // Here, we pass the error to the upper layers

   ima->actualIndex = tileset;      
   return 1;
}   


