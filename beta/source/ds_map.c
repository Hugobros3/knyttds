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

/* TODO FIX - NOTE! - This system has a "weird" optimization system: In case
   there is no memory for the tilesets, the ambiance music is stopped -:)
   (Need to find a better way to do it...)
*/

#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_submap.h"
#include "ds_util.h"
#include "ds_ini.h"
#include "ds_music.h"
#include "ds_15bpp.h"
#include "ds_3dsprite.h"
#include "ds_3dspritehdd.h"

int _debug = 0;

void _ds_map_createTileMap(ds_t_mapIma *screen, ds_t_mapFlag *flagC, ds_t_mapFlag *flagD, ds_t_mapFlag *flagH, ds_t_15bpp *t1, ds_t_15bpp *t2, ds_t_15bpp *back,  ds_t_room *room) {
   int i,j,t;
   int xx,yy;
   u8 tile;
   u16 pix;
   u16 pix_old;
   u8 pix_alpha;
   ds_t_15bpp *tA;
   int tileperrow;
   int val, valscr;
   int partialY, partialYScr, pY, pYScr, pXoffset;
   u16 *ptrScr;
   u16 *ptrPan;
	   
   /* 0) Initializes Flags */
   /*----------------------*/
	memset(flagC, 0, (600*240) >> 3); 							 // <TODO> Which one is better...
	//DMA_Copy(Blank, (void*)flagC, (600*240) >> 5, DMA_32NOW); // <TODO> Which one is better...
	memset(flagD, 0, (600*240) >> 3); 							 // <TODO> Which one is better...
	//DMA_Copy(Blank, (void*)flagD, (600*240) >> 5, DMA_32NOW); // <TODO> Which one is better...
	memset(flagH, 0, (600*240) >> 3); 							 // <TODO> Which one is better...
	//DMA_Copy(Blank, (void*)flagD, (600*240) >> 5, DMA_32NOW); // <TODO> Which one is better...
   
   /* A) Writes Panorama */
   /*--------------------*/
	ptrScr = (u16 *)screen;
	ptrPan = back->png_screen;
	for (j = 0; j < 240; j++) {
      for (i = 0; i < (600 / back->width); i++) {
      	//DMA_Copy(ptrPan,ptrScr,back->width >> 1,DMA_32NOW); // If things goes wrong, blame DMA! :-)
      	memcpy(ptrScr,ptrPan,back->width << 1); // Faster than DMA on HW!
      	ptrScr = ptrScr + back->width;
      }
      ptrPan = ptrPan + back->width;
	}
	
   /* DEBUG - TODO : Deletes tilemaps in memory in order to save RAM */
   ds_submap_deleteTileset(DS_C_PANORAMA,back);
	
   /* B) Writes Layers */
   /*------------------*/
   j = 0;
   while (j < 10) {// 240 / 24
   	i = 0;
   	while (i < 25) { // 600 / 24
   	   for (t = 0; t < 4; t++) {
   	      tile = room->tilelayer[t].tile[j][i];
				if ((tile != 0) && (tile != 128)) { // Not transparent
					// Get actual tile
					tA = (tile < 128)?t1:t2;
					tile = tile & 0x7f;
					// If RAW, load the tile dynamically
					if (!ds_15bpp_storeRawTile(tA,tile)) {
						if (!ds_15bpp_getRawTile(tA,tile))
							ds_global_errorHalt("Partial tile loading");
					}			
					tileperrow = tA->width / 24;
   	         // Draw tile on screen
   	         pY = ((tile / tileperrow) * 24 * tA->width);
   	         pYScr = ((j * 24) * 600);
   	         pXoffset = (tile % tileperrow) * 24;
   	         for (yy = 0; yy < 24; yy++) {
   	            partialY =  pY + (yy * tA->width);
   	            partialYScr =  pYScr + (yy * 600);
   	         	for (xx = 0; xx < 24; xx++) {   	            
   	         	   val = ((pXoffset) + xx) + partialY;
   	         	   valscr = ((i * 24) + xx) + partialYScr;
   	         	   pix = tA->png_screen[val];
   	         	   if (!tA->is_alpha)
   	         	   	pix_alpha = (tA->png_screen[val] == 0)?0x00:0xff;
   	         	   else
   	         	   	pix_alpha = tA->png_alpha[val];
   	         	   if (pix_alpha != 0x00) {
   	         	      if (pix_alpha == 0xff)
   	         	   		(*screen)[valscr] = pix;
   	         	   	else {
   	         	   	   pix_old = (*screen)[valscr];
   	         	   	   if (pix_alpha == 128)
   	         	   	   	(*screen)[valscr] = ds_util_mixColorsAplha128(pix_old, pix);
   	         	   	   else
   	         	   	   	(*screen)[valscr] = ds_util_mixColorsAplha(pix_old, pix, pix_alpha);
   	         	   	}   
   	         		}   
   	         		// Opt - Collision mark
   	         		if (t == 3) {
   	         		   if (pix_alpha != 0x00) // NOT == 0xff, for collision, alpha pixels count
   	         		   	(*flagC)[valscr >> 3] = ds_util_bitSet8((*flagC)[valscr >> 3],valscr & 0x07);
   	         		}   
   	         	}
					}			   
   	      }      	      
   	   }   	       
   	   // Next Tile...
   	   i++;
   	}
   	// Next Row...
		j++;
	}   
}

/* Inits the map subsystem */
void ds_map_init() {
   //ds_global_map.x/y...
   //ds_global_map.room...
   ds_global_map.tilesetA.png_screen = NULL;
   ds_global_map.tilesetA.png_alpha = NULL;
   ds_global_map.tilesetA.actualIndex = -1;
   ds_global_map.tilesetB.png_screen = NULL;
   ds_global_map.tilesetB.png_alpha = NULL;
   ds_global_map.tilesetB.actualIndex = -1;
   ds_global_map.panorama.png_screen = NULL;
   ds_global_map.panorama.png_alpha = NULL;
   ds_global_map.panorama.actualIndex = -1;
   //ds_global_map.tileMap...
   ds_global_map.flag = 0;
   ds_global_map.pass = 0;
   ds_global_map.passMax = 0;
}

/* Load the map and its associated information from the file subsystem */
int ds_map_loadHDD(int x, int y) {
	/* Load the map information from the Map.bin. If no map, "void" room!!!! */
	ds_submap_resetRoom(&ds_global_map.room);
	if (!ds_submap_loadRoom(x,y,&ds_global_map.room)) {
	   if (!ds_global_errorHappened()) {
		   ds_submap_resetRoom(&ds_global_map.room);
		} else {
		   sprintf(ds_global_string,"CALL ds_submap_loadRoom[%d,%d]",x,y);
		   ds_global_errorHalt(ds_global_string);
		   //--HALT--//
		}			   
	}   
	
	/* Copy obj/bank information in specific structure, so it can be queried later */
	int t;
	for (t = 0; t < 4; t++)
		ds_global_map.roomObj[t] = ds_global_map.room.objlayer[t];
	
	/* Prepares the map structure */
	ds_global_map.x = x;
	ds_global_map.y = y;
   ds_global_map.flag = 0;	
   
   /* Finishes... */
   //PA_OutputText(1,1,1,"(5) Done...                         ");   
   return 1;
}

/* Use the already loaded data to paint the base map */
int ds_map_loadHDDPaint(int x, int y) {
   /* Load the images from the tileset and panorama subsystem... but do not load those tilesets I already have!!!!! */
   if (ds_global_map.room.tilesetA != ds_global_map.tilesetA.actualIndex) {
      //PA_OutputText(1,1,1,"(1) Loading Tileset A...          ");
      ds_submap_deleteTileset(DS_C_TILESETA,&ds_global_map.tilesetA);
		if (!ds_submap_loadTileset(DS_C_TILESETA, ds_global_map.room.tilesetA,&ds_global_map.tilesetA,1)) {
		   ds_global_errorAssign(DS_C_ERR_); // "Consumes" error...
		   // TODO - FIX - WEIRD opt: Stop non-mp3 music!!!!
		   ds_music_playMusicAndAmbiance(ds_global_map.room.music,ds_global_map.room.atmosA,ds_global_map.room.atmosB,1);
		   if (!ds_submap_loadTileset(DS_C_TILESETA, ds_global_map.room.tilesetA,&ds_global_map.tilesetA,1)) {
		      ds_global_errorAssign(DS_C_ERR_); // "Consumes" error...
		      // F********CK!!!!!!... OK, *STOP* Music here
		      ds_music_playOnlyMusic(0);
		      if (!ds_submap_loadTileset(DS_C_TILESETA, ds_global_map.room.tilesetA,&ds_global_map.tilesetA,1)) {
		   		sprintf(ds_global_string,"CALL loadTilesetA[%d](%d,%d)",ds_global_map.room.tilesetA,ds_global_map.room.tilesetB,ds_global_map.room.background);
		   		ds_global_errorHalt(ds_global_string);	// Sorry... we really had no memory T_T
		   		//--HALT--//
		  		} 		
		 	}  	
		}
	}	
	if (ds_global_map.room.tilesetB != ds_global_map.tilesetB.actualIndex) {
   	//PA_OutputText(1,1,1,"(2) Loading Tileset B...          ");
	   ds_submap_deleteTileset(DS_C_TILESETB, &ds_global_map.tilesetB);
		if (!ds_submap_loadTileset(DS_C_TILESETB, ds_global_map.room.tilesetB,&ds_global_map.tilesetB,1)) {
		   ds_global_errorAssign(DS_C_ERR_); // "Consumes" error...
		   // TODO - FIX - WEIRD opt: Stop music!!!!
			ds_music_playMusicAndAmbiance(ds_global_map.room.music,ds_global_map.room.atmosA,ds_global_map.room.atmosB,1);
			if (!ds_submap_loadTileset(DS_C_TILESETB, ds_global_map.room.tilesetB,&ds_global_map.tilesetB,1)) {
			   ds_global_errorAssign(DS_C_ERR_); // "Consumes" error...
  		      // F********CK!!!!!!... OK, *STOP* Music here
		      ds_music_playOnlyMusic(0);	
				if (!ds_submap_loadTileset(DS_C_TILESETB, ds_global_map.room.tilesetB,&ds_global_map.tilesetB,1)) {
			   	sprintf(ds_global_string,"CALL loadTilesetB[%d](%d,%d)",ds_global_map.room.tilesetB,ds_global_map.room.tilesetA,ds_global_map.room.background);
			   	ds_global_errorHalt(ds_global_string);
			   	//--HALT--//
			 	}  	
		 	}  	
		}   
	}		
	
	if (ds_global_map.room.background != ds_global_map.panorama.actualIndex) {
		//PA_OutputText(1,1,1,"(3) Loading Panorama...          ");
  		ds_submap_deleteTileset(DS_C_PANORAMA, &ds_global_map.panorama);
		if (!ds_submap_loadTileset(DS_C_PANORAMA, ds_global_map.room.background,&ds_global_map.panorama,0)) {
		   ds_global_errorAssign(DS_C_ERR_); // "Consumes" error...
		   // TODO - FIX - WEIRD opt: Stop music!!!!
		   ds_music_playMusicAndAmbiance(ds_global_map.room.music,ds_global_map.room.atmosA,ds_global_map.room.atmosB,1);
		   if (!ds_submap_loadTileset(DS_C_PANORAMA, ds_global_map.room.background,&ds_global_map.panorama,0)) {
     		   ds_global_errorAssign(DS_C_ERR_); // "Consumes" error...
  		      // FUUUUUUUUCK!!!!!!... OK, *STOP* Music here
		      ds_music_playOnlyMusic(0);
			   if (!ds_submap_loadTileset(DS_C_PANORAMA, ds_global_map.room.background,&ds_global_map.panorama,0)) {
			   	sprintf(ds_global_string,"CALL loadPanorama[%d](%d,%d)",ds_global_map.room.background,ds_global_map.room.tilesetA,ds_global_map.room.tilesetB);
			   	ds_global_errorHalt(ds_global_string);
				   //--HALT--//
				}   
			}   
		}   
	}		
   //PA_OutputText(1,1,1,"(4) Creating Map information...          ");   
   /* Creates the Map information - TileMap, Collisions, Damage */
   _ds_map_createTileMap(&ds_global_map.tileMap,&ds_global_map.tileMapCol,
				&ds_global_map.tileMapDam,&ds_global_map.tileMapHide,
				&ds_global_map.tilesetA,&ds_global_map.tilesetB,&ds_global_map.panorama,
				&ds_global_map.room);
   //_ds_map_createCollision();
   //_ds_map_createDamage();

   
   /* Deletes tilemaps in memory in order to save RAM. For RAW files, finishes things */
   ds_submap_deleteTileset(DS_C_TILESETA,&ds_global_map.tilesetA);
   ds_submap_deleteTileset(DS_C_TILESETB,&ds_global_map.tilesetB);
   //ds_submap_deleteTileset(DS_C_PANORAMA,&ds_global_map.panorama); -> Done inside createTileMap

//	if (_debug)
//		ds_global_errorHalt("After debug...");
   
   /* Finishes... */
   //PA_OutputText(1,1,1,"(5) Done...                         ");   
   return 1;
}

/* Do some operations after loading everything (expect the music) */
void ds_map_loadHDDPOST() {
	ds_t_object *object;
	void *myiterator;
	u16* tileIma;
	int i,j,layerCnt;

	if (!ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_HASBIGCO))
		return;

	// Paints *big* CO in the map
	for (layerCnt = 0; layerCnt < 4; layerCnt++) {
	   myiterator = ds_linkedlist_startIterator(&ds_global_objects);
	   while ((object = ds_linkedlist_getIterator(&ds_global_objects,&myiterator)) != NULL) {
	      if ((object->bank == DS_C_CO_BANK) && 
			    (!ds_3dsprite_hasHWSprite(object->sprite)) &&
				 (object->layer == layerCnt)) {
	         tileIma = ds_3dspritehdd_getSprite(object->bank, object->obj, 0);
	         // Select one drawing mode
	         int xinioverflow;
	         int xiniima;
				if (object->x >= 0) {
				   xinioverflow = object->x;
				   xiniima = 0;
				} else {
					xinioverflow = 0;
					xiniima = object->xs + object->x;
				}			
	         int yinioverflow;
	         int yiniima;
				if (object->y >= 0) {
				   yinioverflow = object->y;
				   yiniima = 0;
				} else {
					yinioverflow = 0;
					yiniima = object->ys + object->y;
				}			
	         int xendoverflow = ((xinioverflow + object->xs) < 600)?object->xs:600 - xinioverflow;
	         int yendoverflow = ((yinioverflow + object->ys) < 240)?object->ys:240 - yinioverflow;
	         // Draw!
	         if (object->layer >= 2) {
	            // LAYER IN FRONT OF JUNI - Update "Hide" Flags
		         ds_t_mapFlag *flagH = &ds_global_map.tileMapHide;
		         for (j = yiniima; j < yendoverflow; j++) {
		            int yrow = (yinioverflow + j) * 600;
		            for (i = xiniima; i < xendoverflow; i++) {
		               u16 pix = tileIma[i + (j * object->xs)];
		               if (pix != 0) {
		                  int val = xinioverflow + i + yrow;
		               	ds_global_map.tileMap[val] = pix;
		               	(*flagH)[val >> 3] = ds_util_bitSet8((*flagH)[val >> 3],val & 0x07);
		             	}  	
		            }   
		         }   
		         // Signals that we have to take care with Juni's sprite
		         ds_global_map.flag = ds_util_bitSet16(ds_global_map.flag,DS_C_MAP_ISHIDE);
		  		} else {
		  		   // LAYER BEHIND JUNI - Just write
		         for (j = yiniima; j < yendoverflow; j++) {
		            int yrow = (yinioverflow + j) * 600;
		            for (i = xiniima; i < xendoverflow; i++) {
		               u16 pix = tileIma[i + (j * object->xs)];
		               if (pix != 0) {
		                  int val = xinioverflow + i + yrow;
		               	ds_global_map.tileMap[val] = pix;
		             	}  	
		            }   
		         }   
				}      
	      }   
		}	   
	}		
}   


/* Provides the X-Y coordinates where Juni must go next, if she goes to another screen */
void ds_map_getBoundary(int boundary, int *mapx, int *mapy) {
   // "Moves" map
   switch (boundary) {
      case -1:
         return;
         break; // I know, I do not need this... :-P
      case 0: // NORTH
      	(*mapy) = (*mapy) - 1;
      	break;
      case 1: // EAST
      	(*mapx) = (*mapx) + 1;
      	break;
      case 2: // SOUTH
      	(*mapy) = (*mapy) + 1;
      	break;
      case 3: // WEST
      	(*mapx) = (*mapx) - 1;      	
      	break;
    }
    
    // Check if there is any warp in this area...
    if (ds_util_bitOne16(ds_global_map.flag,DS_C_MAP_WARP)) {
       (*mapx) = ds_ini_getint(ds_global_world.worldini,
		   			ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_WARP_MAPX,boundary,0,0,0),
   					0)
   				  + (*mapx);
       (*mapy) = ds_ini_getint(ds_global_world.worldini,
		   			ds_ini_keyDictionary(ds_global_map.x,ds_global_map.y,DS_C_DICT_WARP_MAPY,boundary,0,0,0),
   					0)
   				  + (*mapy);
    }      
}   

/* Copy a sprite to a flag subsystem */
void ds_map_copyFlag(u16 *ima,int xsize,int ysize,int x,int y, ds_t_mapFlag* flag) {
   int i,j;
   int pos;
   
   for (j = 0; j < ysize; j++) {
   	for (i = 0; i < xsize; i++) {
   	   if (ima[i + (j * xsize)] != 0) {
   	   	pos = (x + i) + ((y + j) * 600);
   	   	(*flag)[pos >> 3] = ds_util_bitSet8((*flag)[pos >> 3],pos & 0x07);
   		} 
   	}   
 	}
} 

/* Copy a value (0/1) to a flag subsystem */
void ds_map_copyFlagValue(u8 val,int xsize,int ysize,int x,int y, ds_t_mapFlag* flag) {
   int i,j;
   int pos;
   
   for (j = 0; j < ysize; j++) {
   	for (i = 0; i < xsize; i++) {
  	   	pos = (x + i) + ((y + j) * 600);
  	   	if (val)
   	   	(*flag)[pos >> 3] = ds_util_bitSet8((*flag)[pos >> 3],pos & 0x07);
   	   else
	   	   (*flag)[pos >> 3] = ds_util_bitDel8((*flag)[pos >> 3],pos & 0x07);
   	}   
 	}
} 

/* Check if a sprite collides with a flag subsystem */
int ds_map_collideFlag(u16 *ima,int xsize,int ysize,int x,int y, ds_t_mapFlag* flag) {
   int i,j,yj,xi;
   int pos;
	
	if ((y >= 0) && ((y+ysize)<240) && (x >= 0) && ((x+xsize)<600)) {
	   // "No boundary" analysis
		for (j = 0; j < ysize; j++) {
	   	for (i = 0; i < xsize; i++) {
			   if (ima[i + (j * xsize)] != 0) { // "Pixel inside"
   		   	pos = (x + i) + ((y + j) * 600);
   		   	if (((*flag)[pos >> 3] >> (pos & 0x07)) & 1)
		   		//if (ds_util_bitOne8((*flag)[pos >> 3],pos & 0x07))
   		   		return 1;
				} 
			}   
		}
	} else {	   
	   // "Boundary" analysis 
	   for (j = 0; j < ysize; j++) {
	      yj = (y + j);
	      if ((yj >= 0) && (yj < 240)) { // Inside screen (Y)
		   	for (i = 0; i < xsize; i++) {
	   		   if (ima[i + (j * xsize)] != 0) { // "Pixel inside"
	   		      xi = (x + i);
	   		      if ((xi >= 0) && (xi < 600)) { // Inside screen (X)
		   		   	pos = xi + (yj * 600);
		   		   	if (((*flag)[pos >> 3] >> (pos & 0x07)) & 1)
	   		   		//if (ds_util_bitOne8((*flag)[pos >> 3],pos & 0x07))
		   		   		return 1;
		   		  	} 		
	   			} 
	   		}   
	 		}  	
	 	}
	}	 	
 	return 0;
}  

/* Checks if a pixel is out of the map */
int _ds_map_outOfMapPix(int x, int y) {
   return ((x >= 600) || (x < 0) || (y >= 240) || (y < 0));
}

/* Check if a sprite collides with a flag subsystem - ONLY THE CENTER PIXEL */
int ds_map_collideFlagCenter(u16 *ima,int xsize,int ysize,int x,int y, ds_t_mapFlag* flag) {
   int yj,xi;
   int pos;
   
   xi = x + (xsize >> 1);
   yj = y + (ysize >> 1);
   if (!_ds_map_outOfMapPix(xi, yj)) {
   	pos = xi + (yj * 600);
	   if (((*flag)[pos >> 3] >> (pos & 0x07)) & 1)
	   //if (ds_util_bitOne8((*flag)[pos >> 3],pos & 0x07))
	   	return 1;
 	}
 	
 	return 0;
}  


/* Check if a sprite is outside the map */
int ds_map_outOfMap(int xsize,int ysize,int x,int y) {   
   if (_ds_map_outOfMapPix(x,y))
   	return 1;
   else
   if (_ds_map_outOfMapPix(x + xsize,y))
   	return 1;
   else
   if (_ds_map_outOfMapPix(x,y + ysize))
   	return 1;
   else
   if (_ds_map_outOfMapPix(x + xsize,y + ysize))
   	return 1;		
   	
   return 0;
}  

/* Check if a sprite is outside the map - CENTER */
int ds_map_outOfMap_center(int xsize,int ysize,int x,int y) {   
   return (_ds_map_outOfMapPix(x + (xsize >> 1),y + (ysize >> 1)));
}  

/* Resets the map and its elements */
void ds_map_reset(int totalReset) {
   // Resets the room
   	// -> No need. Room structures are static.
   	
   // Reset the tileset and panorama subsystem
   if (totalReset) {
      // We are leaving the game... png's MUST be deleted
      ds_submap_deleteTileset(DS_C_TILESETA, &ds_global_map.tilesetA);
      ds_submap_deleteTileset(DS_C_TILESETB, &ds_global_map.tilesetB);
      ds_submap_deleteTileset(DS_C_PANORAMA, &ds_global_map.panorama);
   } else {   
   	// -> CACHE. Chance to use a "cache" for the next room. Deletion will be done by loadHDD
 	}  	
   	
   // Resets the tilemap
   	// -> No need, too! Tilemap is also a static structure
		
	// Reset other map structures
	ds_global_map.passMax = 0; // Resets the password system
}   

/* Manages the map  */
void ds_map_manage() {
   // Copy the Damage matrix to the temporal damage matrix (DMA -> MUCH better!)
   // Note... Copy ONLY! the part where Juni is... she is the only one affected by the damage matrix!
   int inix = ds_global_juni.x - 12;
   int endx = ds_global_juni.x + 36;
   int iniy = ds_global_juni.y - 12;
   int endy = ds_global_juni.y + 36;
   if (_ds_map_outOfMapPix(inix,ds_global_juni.y))
   	inix = 0;
   if (_ds_map_outOfMapPix(endx,ds_global_juni.y))
   	endx = 599;
   if (_ds_map_outOfMapPix(ds_global_juni.x,iniy))
   	iniy = 0;
   if (_ds_map_outOfMapPix(ds_global_juni.x,endy))
   	endy = 239;
   inix = inix >> 3;
   endx = endx >> 3;
   iniy = iniy;
   endy = endy;
	int i,j,pos;
	for (j = iniy; j <= endy; j++) {
		for (i = inix; i <= endx; i++) {
		   pos = i + (j * 75);
			ds_global_map.tileMapDamTemp[pos] = ds_global_map.tileMapDam[pos];
		}   	   
	}
	
	   //DMA_Copy((void*)ds_global_map.tileMapDam, (void*)ds_global_map.tileMapDamTemp, (600*240) >> 5, DMA_32NOW);
   //memcpy((void*)ds_global_map.tileMapDamTemp,(void*)ds_global_map.tileMapDam,(600*240) >> 3);
}

/* Returns "1" if there is a collision in that specific pixel */
int ds_map_coll(int x, int y) {
   int pos;
   if (_ds_map_outOfMapPix(x,y))
   	return 0; // <TODO> Optimize this!
   pos = x + (y * 600);
   return ds_util_bitOne8(ds_global_map.tileMapCol[pos >> 3],pos & 0x07);
}   

/* Paints the map into the screen */
void ds_map_paint(u16 *screen, int x, int y, int dsscreen) {
   ds_global_paintScreen(dsscreen,screen,x,y); // Just tell the system to paint into the main screen ^^
}

/* Checks if this sprite has something solid in his feet or below its feet.
		It depends on the sprite, so it is not "Basic" */
int ds_map_collDown(int x, int y, int xsize, int ysize, int clash) {
   int res = 0;
   int i;
   int xcheck, ycheck;

   ycheck = y+clash;
   for (i = (xsize / 3); i < ((xsize << 1)/ 3); i++) { // 1/3 -> 2/3
      xcheck = x + i;    
	   if (ds_map_coll(xcheck,ycheck)) {
	   	res = 1;
	   	break;
	 	}  	
	}	
	
	return res;
}  


/* Checks if this sprite has something solid in his feet (23) or below its feet (24).
		It's "Basic" because manages 24y sprites, and it is not pixel-perfect, just checks below the sprite */
int ds_map_collDownBasic(int x, int y, int clash) {
   int res = 0;
   int i;
   int xcheck, ycheck;

   ycheck = y+clash; // (23 for ey-I-am-inside-the-ground, 24 for gravity)
   for (i = 6; i < 18; i++) { // 7-15?
      xcheck = x + i;    
      //PA_Put16bitPixel(0,xcheck,ycheck,PA_RGB(0,0,31));
	   if (ds_map_coll(xcheck,ycheck)) {
	   	res = 1;
	   	break;
	 	}  	
	}	
	
	return res;
}  

/* Checks for "hitting the ceiling".
		It's "Basic" because manages 24y sprites, and it is not pixel-perfect, just checks over the sprite */
int ds_map_collUpBasic(int x, int y, int clash) {
   int res = 0;
   int i;
   int xcheck, ycheck;

   ycheck = y+clash;
   for (i = 6; i < 18; i++) { // -> These are the X-coord collisions!!!!!! - 
      xcheck = x + i;    
      //PA_Put16bitPixel(0,xcheck,ycheck,PA_RGB(0,0,31));
	   if (ds_map_coll(xcheck,ycheck)) {
	   	res = 1;
	   	break;
	 	}  	
	}	
	
	return res;
}  


/* Checks if the Sprite clashes in a x-based movement.
	It's "basic" because the caller decides where to check */
int ds_map_collMovBasic(int x, int y, int clash) {
   int res = 0;
   int j;
   int xcheck, ycheck;

   xcheck = x + clash;
   for (j = 7; j < 24 - 4; j++) {  // <TODO> "Ad-Teoriam", this is -3 Also, it is 4
      ycheck = y + j;    // e.g. y+4..x+20 
      //PA_Put16bitPixel(0,xcheck,ycheck,PA_RGB(0,0,31));
	   if (ds_map_coll(xcheck,ycheck)) {
	   	res = 1;
	   	break;
	 	}  	
	}	
	
	return res;
} 

/* Checks if the Sprite is still allowed to climb.
	It's "basic" because the caller decides where to check */
int ds_map_collClimbBasic(int x, int y, int clash) {
   int res = 0;
   int j;
   int xcheck, ycheck;

   xcheck = x + clash;
   for (j = 10; j < 24 - 4; j++) {
      ycheck = y + j;    // e.g. y+10..x+19 
      //PA_Put16bitPixel(0,xcheck,ycheck,PA_RGB(0,0,31));
	   if (ds_map_coll(xcheck,ycheck)) {
	   	res = 1;
	   	break;
	 	}  	
	}	
	
	return res;
} 	

/* Checks if a certain position is equal to a certain bank/obj combination */
int ds_map_collBankObj(int x, int y, int bank, int obj) {
   int cx,cy;
   int layer;
   
   // Inside the map?
   if (_ds_map_outOfMapPix(x,y))
   	return 0;
   cx = x / 24;
   cy = y / 24;
   
   // Test every object layer...
   for (layer = 0; layer < 4; layer++) {
		if (bank == ds_global_map.room.objlayer[layer].bank[cy][cx])
	   	if (obj == ds_global_map.room.objlayer[layer].obj[cy][cx])
	   		return 1;
	}  	
	return 0;
}

