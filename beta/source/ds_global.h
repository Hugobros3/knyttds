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


/* Global State of the Game. All relevant information is stored here */
#ifndef _DS_GLOBAL_
#define _DS_GLOBAL_

#include <PA9.h>       // Include for PA_Lib
#include "ds_linkedlist.h"

/* Debug Flag */
#define DEBUG_KSDS

//--------------------------------------
// CONSTANTS
//--------------------------------------

// Filesystem-related constants
#define DS_DIR_MAIN "/data/dsknytt"
#define DS_DIR_TILE "/Tilesets"
#define DS_DIR_GRAD "/Gradients"
#define DS_DIR_WORLD "/Worlds"
#define DS_DIR_SYS "/System"
#define DS_DIR_SAVE "/Saves"
#define DS_DIR_MUSIC "/Music"
#define DS_DIR_AMBIANCE "/Ambiance"
#define DS_DIR_CO "/custom objects"
#define DS_DIR_RAW "/_Raw"
#define DS_DIR_OBJ "/Objects/Bank"
#define DS_DIR_JUNI "/Objects/Juni"
#define DS_DIR_PART "/Objects/Particles"

// Tileset-related constants
#define DS_C_TILESETA 0
#define DS_C_TILESETB 1
#define DS_C_PANORAMA 2

// Particles - Bank information
#define DS_C_PART_BANK (DS_C_MAX_BANK - 1) // "Children" bank

// CO - Bank Information
#define DS_C_CO_BANK 255

// Map-B/O-related constants
#define DS_C_MAX_BANK 20
#define DS_C_MAX_OBJ 50
#define DS_C_MAX_OBJ_PART 100
#define DS_C_MAX_OBJ_CO 256

// Map-Flag-related constants
#define DS_C_MF_WALK 0
#define DS_C_MF_DIE 1

// Map-Layer Constants
#define DS_C_PRIO 100
#define DS_C_PRIOLMAX 5
#define DS_C_PRIOLJUNISP 2 // (0-1) 2 (3-4) [5]

// Map - Special (optimization) information
#define DS_C_MAP_WARP 0
#define DS_C_MAP_SPRING 1
#define DS_C_MAP_NOJUMP 2
#define DS_C_MAP_NOCLIMB 3
#define DS_C_MAP_STICKY 4
#define DS_C_MAP_UPWIND 5
#define DS_C_MAP_CGLOWA 6
#define DS_C_MAP_CGLOWZ 7
#define DS_C_MAP_ISHIDE 8
#define DS_C_MAP_HASCO 9
#define DS_C_MAP_HASBIGCO 10

// Error-related constants
#define DS_C_ERR_ -1
#define DS_C_ERR_NOTILESET 0
#define DS_C_ERR_NOMAPBIN 1
#define DS_C_ERR_NOMEMORY 2
#define DS_C_ERR_NOMOREOBJECTS 3
#define DS_C_ERR_NOWORLDINI 4
#define DS_C_ERR_NOSAVEINI 5
#define DS_C_ERR_ZIPFAST 6
#define DS_C_ERR_NOHDD 7
#define DS_C_ERR_NOFOUND 8
#define DS_C_ERR_BIGIMA 9

// Input-related constants
#define DS_C_IN_LEFT 0
#define DS_C_IN_RIGHT 1
#define DS_C_IN_UP 2
#define DS_C_IN_DOWN 3
#define DS_C_IN_TJUMP 4
#define DS_C_IN_THOLO 5
#define DS_C_IN_TUMBR 6
#define DS_C_IN_TWALK 7
#define DS_C_IN_L 8
#define DS_C_IN_R 9
#define DS_C_IN_START 10
#define DS_C_IN_SELECT 11
#define DS_C_IN_STYLUS 12

// Type of object constants
#define DS_C_OBJ_ 255
#define DS_C_OBJ_EVENT 0
#define DS_C_OBJ_ITEM 1
#define DS_C_OBJ_OBJECT 2
#define DS_C_OBJ_PARTICLE 3

// Object constants - Logical Behaviour
#define DS_C_OBJ_F_INVISIBLE 0
#define DS_C_OBJ_F_HARMFUL 1
#define DS_C_OBJ_F_STATICHARMFUL 2
#define DS_C_OBJ_F_COLLIDE 3
#define DS_C_OBJ_F_FULLCOLLIDE 4
#define DS_C_OBJ_F_ONETIME 5
#define DS_C_OBJ_F_WARN 6
#define DS_C_OBJ_F_GLOBAL_IMA 7
#define DS_C_OBJ_F_GLOBAL_MANAGE 8
#define DS_C_OBJ_F_GLOBAL_MANAGE_ONECYCLE 9
#define DS_C_OBJ_F_GLOBAL_MANAGE_ONEINSTANCE 10
#define DS_C_OBJ_F_GLOBAL_RADAR 11
#define DS_C_OBJ_F_WARN_SPECIAL 12

// Object constants - Arrays
#define DS_C_LIMITS_VAROBJ 12
#define DS_C_LIMITS_VARPOBJ 4

// Juni - Item-related constants
#define DS_C_JUNI_IT_RUN 0
#define DS_C_JUNI_IT_CLIMB 1
#define DS_C_JUNI_IT_DBLJUMP 2
#define DS_C_JUNI_IT_JUMP 3
#define DS_C_JUNI_IT_EYE 4
#define DS_C_JUNI_IT_ENEMY 5
#define DS_C_JUNI_IT_UMBRELLA 6
#define DS_C_JUNI_IT_HOLOGRAM 7
#define DS_C_JUNI_IT_REDKEY 8
#define DS_C_JUNI_IT_YELLOWKEY 9
#define DS_C_JUNI_IT_BLUEKEY 10
#define DS_C_JUNI_IT_VIOLETKEY 11
#define DS_C_JUNI_IT_MAX_ 12

// Juni - Flag-related constants
#define DS_C_JUNI_FLAG_MAX_ 10

// Juni - Bank/Obj-related constants & state constants
#define DS_C_JUNI_BANK 254
#define DS_C_JUNI_ST_STOP_R 0
#define DS_C_JUNI_ST_STOP_L 1
#define DS_C_JUNI_ST_WALK_R 2
#define DS_C_JUNI_ST_WALK_L 3
#define DS_C_JUNI_ST_RUN_R 4
#define DS_C_JUNI_ST_RUN_L 5
#define DS_C_JUNI_ST_FALL_R 6 // Note that there are some hacks related to the order of these constants (<HACK>)
#define DS_C_JUNI_ST_FALL_L 7
#define DS_C_JUNI_ST_JUMP_R 8
#define DS_C_JUNI_ST_JUMP_L 9
#define DS_C_JUNI_ST_CLIMB_R 10
#define DS_C_JUNI_ST_CLIMB_L 11
#define DS_C_JUNI_ST_FLY_R 12
#define DS_C_JUNI_ST_FLY_L 13
#define DS_C_JUNI_ST_MAX_ 14 
#define DS_C_JUNI_ST_DEAD 255

// Juni - Special sprite constants
#define DS_C_JUNI_BANKSP 253
#define DS_C_JUNI_SP_UMBRELLA 0
#define DS_C_JUNI_SP_REDGLOW 1
#define DS_C_JUNI_SP_CYANGLOW 2
#define DS_C_JUNI_SP_HOLOGRAM 3
#define DS_C_JUNI_SP_MAX_ 4

// Juni - Movement State Constants
#define DS_C_JUNI_MOVST_X_STOP 		100
#define DS_C_JUNI_MOVST_X_WALK 		101
#define DS_C_JUNI_MOVST_X_RUN 		102
#define DS_C_JUNI_MOVST_X_RUNSTOP   103
#define DS_C_JUNI_MOVST_X_RUNSKID 	104
#define DS_C_JUNI_MOVST_Y_STOP 		200
#define DS_C_JUNI_MOVST_Y_JUMP 		201
#define DS_C_JUNI_MOVST_Y_JUMPSOFT 	202
#define DS_C_JUNI_MOVST_Y_CLIMB 		203
#define DS_C_JUNI_MOVST_Y_SLIDE 		204
#define DS_C_JUNI_MOVST_Y_SLIDESOFT 205
#define DS_C_JUNI_MOVST_Y_FALL 		206
#define DS_C_JUNI_MOVST_Y_FLY			207
#define DS_C_JUNI_MOVST_Y_FLYEND		208
#define DS_C_JUNI_MOVST_Y_FLYSTART	209
#define DS_C_JUNI_MOVST_Y_FLYFAST	210

// Dictionary-related constants
#define DS_C_DICT_SIGN 10
#define DS_C_DICT_SHIFT_ONTOUCH 101
#define DS_C_DICT_SHIFT_MAPXREL 102
#define DS_C_DICT_SHIFT_MAPYREL 103
#define DS_C_DICT_SHIFT_POSXREL 104
#define DS_C_DICT_SHIFT_POSYREL 105
#define DS_C_DICT_SHIFT_QUANTIZE 106
#define DS_C_DICT_SHIFT_ABSOLUTETARGET 107
#define DS_C_DICT_SHIFT_DENYHOLOGRAM 108
#define DS_C_DICT_SHIFT_CUTSCENE 109
#define DS_C_DICT_SHIFT_SAVE 110
#define DS_C_DICT_SHIFT_FLAGON 111
#define DS_C_DICT_SHIFT_FLAGOFF 112
#define DS_C_DICT_SHIFT_TYPE 113
#define DS_C_DICT_SHIFT_VISIBLE 114
#define DS_C_DICT_FLAG 150
#define DS_C_DICT_FLAGWARPX 151
#define DS_C_DICT_FLAGWARPY 152
#define DS_C_DICT_WARP_MAPX 200
#define DS_C_DICT_WARP_MAPY 201
#define DS_C_DICT_ENDING 900
#define DS_C_DICT_GUI 1000

// A-B-C type constants
#define DS_C_ELETYPE_  0
#define DS_C_ELETYPE_A 1
#define DS_C_ELETYPE_B 2
#define DS_C_ELETYPE_C 3

// Status Screen type
#define DS_C_STA_ 100
#define DS_C_STA_MAP 0
#define DS_C_STA_RADAR 1
#define DS_C_STA_DEBUG 2
#define DS_C_STA_DEBUG2 3
#define DS_C_STA_RTN_MM 4

// Cutscene types
#define DS_C_CUTSCENE_INTRO 0
#define DS_C_CUTSCENE_GAME 1
#define DS_C_CUTSCENE_END 2

// Load Game types
#define DS_C_LOADGAME_INTRO 0
#define DS_C_LOADGAME_GAME 1
#define DS_C_LOADGAME_CUTSCENE 2

// File Types
#define DS_C_FILE_BIN 0
#define DS_C_FILE_RAW 1
#define DS_C_FILE_AUTO 2

// Other Constants
#define DS_C_GAMESTATUS_BLINK 18 // Time to blink when we are launching evil particles
#define DS_C_CO_MAXAREA 100000 // Max pixel area size admisible for CO sprites
#define DS_C_IMA_MAXAREA 300000 // Max pixel area size admisible for any image
#define DS_C_CO_MAXSX 128 // Max X pixel size admisible for CO sprites
#define DS_C_CO_MAXSY 128 // Max Y pixel size admisible for CO sprites

//--------------------------------------
// TYPES
//--------------------------------------

// Image-15bpp
//------------

typedef struct ds_tt_15bpp {
   // "Partial" files for special RAW files
   FILE *f;
   u8 *tile;
   // Normal 15bpp information
   int is_alpha;
   int actualIndex;
   u16 *png_screen;
   u8 *png_alpha;
   int width, height;
   int loaded;
   u8 deleteme;
} ds_t_15bpp;

// Map-Relevant Information (HDD)
//-------------------------------

typedef struct ds_tt_roomtile {
	u8 tile[10][25];
} ds_t_roomtile;

typedef struct ds_tt_roomobj {
   u8 obj[10][25];
   u8 bank[10][25];
} ds_t_roomobj;

typedef struct ds_tt_roomid {
	int id[10][25];
} ds_t_roomid;

typedef struct ds_tt_roompointer {
	void *id[10][25];
} ds_t_roompointer;

typedef struct ds_tt_room {
   ds_t_roomtile tilelayer[4]; // 0..3
   ds_t_roomobj objlayer[4]; // 4..7 (0..3)
   u8 tilesetA;
   u8 tilesetB;
   u8 atmosA;
   u8 atmosB;
   u8 music;
   u8 background;
} ds_t_room; // 3006 bytes

// Map-Relevant Information (Mem)
//-------------------------------
typedef u16 ds_t_mapIma[600 * 240];
typedef u8 ds_t_mapFlag[(600 * 240) >> 3];

typedef struct ds_tt_map {
   int x;           // Actual X (Map)
   int y;           // Actual Y (Map)
   ds_t_room room;  // Actual Room (NOTE: non-event objects are deleted)
   ds_t_roomobj roomObj[4]; // REAL objects original from the room
   ds_t_mapFlag tileMapCol; // Collision Flag
   ds_t_mapFlag tileMapDam; // Damage Flag
   ds_t_mapFlag tileMapDamTemp; // Damage Flag - Temporal
   ds_t_mapFlag tileMapHide; // "Hide" Flag (Used to hide juni from certain CO)
   ds_t_15bpp tilesetA; // Actual Image tilesetA (15bpp)
   ds_t_15bpp tilesetB; // Actual Image tilesetB (15bpp)
   ds_t_15bpp panorama; // Actual Image panorama (15bpp)
   ds_t_mapIma tileMap; // Image of the Actual map
   u16 flag;        // Special flag, stores special data (e.g. Warp)
   u16 pass;			// Special value, for passwords
   u16 passMax;		// Special value, for passwords
} ds_t_map;   

// Game Input
//-----------
typedef struct ds_tt_input {
   u16 Newpress;
   u16 Held;
   u16 Released;
   int stylusx;
   int stylusy;
} ds_t_input;

// Object-Related Information
//---------------------------
typedef int ds_t_finit(u8,u8,void *);
typedef int ds_t_fmanage(void *);
typedef int ds_t_fpostmanage();

typedef struct ds_tt_object {
	// Data
	u8 type;
	u8 layer;
	u8 bank;
	u8 obj;
	// Image & size
	int sprite;
	int xs;
	int ys;
	// Location (ABSOLUTE/RELATIVE)
	int x;
	int y;
	int xtile;  // Note: Relative position is the starting position. IS NOT UPDATED.
	int ytile;	// Note: Relative position is the starting position. IS NOT UPDATED.
   // Object Management
	ds_t_finit *fcreate;   
	ds_t_fmanage *fmanage;
	ds_t_fmanage *fexecute;
	ds_t_fmanage *finstance;
	ds_t_fmanage *fcondition;
	u8 managed;
	// Logical information
	u16 flags;
	// Special
	u8 blink; // Used to blink minimap if object has done something dangerous (e.g. launch particles)
	u8 _deleteme;
	int inner[DS_C_LIMITS_VAROBJ]; // Internal Variables - Basic
	int innerpar[DS_C_LIMITS_VARPOBJ]; // Internal Variables - Particles	
	u32 inner_tick; // Inner tick for special (e.g. particle) management. 0 is considered inactive (due to design).
	// Internal 
	int id; // ID inside object list
} ds_t_object;   

// Juni-Related Information
//-------------------------

typedef struct ds_tt_juni {
   // Items contained and special states
   u16 item;
   u16 flag;
   int umbrellaOn;
   int hologramOn;
   int redGlow;
   int cyanGlow;
   // Speed - physics
   int velX;
   int velY;
   int actualpix;
   int framepix; // <-NOTE: in certain cases, this might be a negative value (longer jumps)
   // state
   int state;
   int movstateX;
	int movstateY;
	int inDblJump;
	int inNoBtnJump;
   int x;  // Absolute
   int y;  // Absolute
   // Image(s)
   int sprite;
   int umb_sprite;
   int umb_state;
   int umb_dir;
   int redglow_sprite;
   int cyanglow_sprite;
   int holo_sprite;
   // Specials
   u8 killme;
} ds_t_juni;   

// World-Relevant Information (Mem)
//---------------------------------

typedef struct ds_tt_world_opt_mapindex {
   u32 offset[400]; // 800-1199 (X)
} ds_t_world_opt_mapindex;   

typedef struct ds_tt_world_opt_mapvisited {
   u8 offset[50]; // 800-1199 (X), 50*8=400
} ds_t_world_opt_mapvisited;   

typedef struct ds_tt_world {
   int loaded;
   // Map information
   char author[255];
   char name[255];
   char dir[255];
   char basicdir[255];
   // .INI information
   void *worldini;
   // Savegame information (stored **in memory** for easy access)
   int saveslot;
   int sv_xmap; 
   int sv_ymap;
   int sv_xjuni; // Relative position
   int sv_yjuni;
   u16 sv_items;
   u16 sv_flags;
   int sv_gui;
   // Optimization information - map indexes
   ds_t_world_opt_mapindex *opt_mapIndex[400]; // 800-1199 (Y)
   ds_t_world_opt_mapvisited opt_mapVisited[400]; // 800-1199 (Y). 20KB is OK :-P
   u8 opt_mapVisitedRow[400]; // Did I visit whis row?
   // "Shift" done
   int shift;
   // "Came from Shift" information
   int shift_cameFrom;
	int shiftx;
	int shifty;
	int shiftabc; 
	// "Ignore shift" information
	int shiftNoX;
	int shiftNoY;
} ds_t_world;   

// Custom Objects
//---------------

typedef struct ds_tt_co {
	char Image[100];
	int Init_AnimFrom;
   int Init_AnimTo;
   int Init_AnimSpeed;
	int Init_AnimLoopback;
   int Init_AnimRepeat;
   int Tile_Width;
   int Tile_Height;
	int Offset_X;
   int Offset_Y;
} ds_t_co;   


//--------------------------------------
// VARIABLES
//--------------------------------------

// Map information
ds_t_map ds_global_map;

// Lid information
int ds_lid_open;
int ds_lid_ignore;

// Internal error information
int ds_global_error;

// input
ds_t_input ds_global_input;

// Object list
LList ds_global_objects;

// Juni!!!!
ds_t_juni ds_global_juni;

// World!
ds_t_world ds_global_world;

// Tick information (incremented one per cycle (1/60th), used as an auxiliar counter)
u32 ds_global_tick;

// Timer information
u8 ds_global_timer;

// Special Str
char ds_global_string[256];

// Special Global Debug String
char ds_global_debug;

// Optimizations?
int ds_global_optimizationPreload;
int ds_global_optimizationUncompress;
int ds_global_optimizationMusic;

//--------------------------------------
// FUNCTIONS
//--------------------------------------

void ds_global_initVariables(); 

u16 *ds_global_getScreen1();

u16 *ds_global_getScreen0();

u16 *ds_global_getScreen(int screen);

void ds_global_paintScreen(int dsscreen, u16 *screen, int x, int y);

void ds_global_paintScreenCropped(int dsscreen, u16 *screen, int x, int y, int ys);

void ds_global_fillScreenRange(int dsscreen, u16 *screen, u16 color, int yi, int ye);

void ds_global_fillScreen(int dsscreen, u16 *screen, u16 color);

int ds_global_safeguardMem(int minmem);

void ds_global_breakpoint(char *where, int X);

void ds_global_errorHalt(char *where);

void ds_global_errorAssign(int error);

int ds_global_errorHappened();

void ds_global_fadeWhitePush(int speed);

void ds_global_fadeWhitePop();

void ds_global_fadeWhite_Ini(); 

int ds_global_fadeWhite_Continue(); 

void ds_global_fadeWhite_Finish();

int ds_global_fadeWhite_CHECK();

#endif
