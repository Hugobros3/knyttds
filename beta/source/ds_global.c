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

// Includes
#include <PA9.h>       // Include for PA_Lib
#include "ds_global.h"
#include "ds_state.h"
#include "ds_input.h"
#include "ds_map.h"
#include "ds_camera.h"
#include "ds_objects.h"
#include "ds_world.h"
#include "ds_button.h"
#include "ds_gamestatus.h"
#include "PA_Text16bitBuffer.h"

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

ds_t_mapIma _ds_global_UpperScreen;
int _ds_global_fadeWhite;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------

// Note: this code (__toncset*) is (c) Jasper Vijn 
typedef unsigned int uint;
 
#define BIT_MASK(len)       ( (1<<(len))-1 )

static inline u32 quad8(u8 x)   
{   x |= x<<8; return x | x<<16;    }


/*! This version mimics memset in functionality, with
    the benefit of working for VRAM as well. It is also
    slightly faster than the original memset.
    \param dst  Destination pointer.
    \param fill Word to fill with.
    \param size Fill-length in bytes.
    \note   The \a dst pointer and \a size need not be
        word-aligned. In the case of unaligned fills, \a fill
        will be masked off to match the situation. 
*/
void __toncset(void *dst, u32 fill, uint size) 
{ 
    if(size==0 || dst==NULL)
        return;
 
    uint left= (u32)dst&3;
    u32 *dst32= (u32*)(dst-left);
    u32 count, mask;
 
    // Unaligned head.
    if(left != 0)
    {
        // Adjust for very small stint.
        if(left+size<4) 
        {
            mask= BIT_MASK(size*8)<<(left*8);   
            *dst32= (*dst32 &~ mask) | (fill & mask);
            return;
        } 
        mask= BIT_MASK(left*8);
        *dst32= (*dst32 & mask) | (fill&~mask);
        dst32++;
        size -= 4-left;
    }
    // Main stint.
    count= size/4;
    uint tmp= count&3; 
    count /= 4;
    switch(tmp) {
        do {    *dst32++ = fill;
		    case 3:     *dst32++ = fill;
		    case 2:     *dst32++ = fill; 
		    case 1:     *dst32++ = fill;
		    case 0:     ; 
		  } while(count--); // Duff's Device!!! O_O
    }
    // Tail
    size &= 3;
    if(size)
    {
        mask= BIT_MASK(size*8);
        *dst32= (*dst32 &~ mask) | (fill & mask);
 	 }   
}


static inline void toncset16(void *dst, u16 src, uint size)
{   __toncset(dst, src|src<<16, size*2);            }

 

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

void ds_global_initVariables() {
   
   // ds_lid
   //-------
	ds_lid_open = 1;
	ds_lid_ignore = 0;
   
   // ds_global_error
   //----------------
   ds_global_error = DS_C_ERR_;
   
	// tick!
	//------
	ds_global_tick = 0;
	
	// Optimization
	//-------------
	ds_global_optimizationPreload = 0;
	ds_global_optimizationUncompress = 1;
	ds_global_optimizationMusic = 1;
	   
   // Subsystems
   //-----------
	ds_state_init();	
	ds_input_init(&ds_global_input);
	ds_map_init();
	ds_camera_initCamera(0,0);
	ds_objects_init();
	ds_world_init();
	ds_button_init();
	ds_gamestatus_initOnce();
	
	// Others
	//-------
	_ds_global_fadeWhite = 0;
	ds_global_debug = 0;
}   

u16 *ds_global_getScreen1() {
   return _ds_global_UpperScreen;
}   

u16 *ds_global_getScreen0() {
   return ds_global_map.tileMap;
}  
 
u16 *ds_global_getScreen(int screen) {
   if (screen == 0)
   	return ds_global_getScreen0();
   else
   	return ds_global_getScreen1();
}  

void ds_global_paintScreenCropped(int dsscreen, u16 *screen, int x, int y, int ys) {
	u16 * ptr;
	int j;
	
	ptr = (screen) + x;		
	for (j=0; j < ys; j++) {
	   //<TODO> If bugs, use memcopy 
		DMA_Copy((ptr + ((j + y) * 600) ),(void *)(PA_DrawBg[dsscreen] + ((j + y) << 8)),256 >> 1,DMA_32NOW); 
	}
}

void ds_global_paintScreen(int dsscreen, u16 *screen, int x, int y) {
	ds_global_paintScreenCropped(dsscreen, screen, x, y, 192);
}


void ds_global_fillScreen(int dsscreen, u16 *screen, u16 color) {
	u16 * ptr;
	int j;
	
	ptr = (screen);		
	for (j=0; j < 192; j++) {
	   //<TODO> If bugs, use memcopy 
		toncset16((ptr + ((j) * 600) ), color, 256);
	}
}

int ds_global_safeguardMem(int minmem) {
	return (getMemFree() < minmem);
}   

void ds_global_errorHalt(char *where) {
char error[255];
char memused[100];

	sprintf(memused,"U: %d / F: %d   ", getMemUsed(), getMemFree());       				

	// Write Error String   
   switch (ds_global_error) {
      case DS_C_ERR_: 
         sprintf(error,"[E00] Unknown Error \n %s \n %s",where,memused);
         break;
      case DS_C_ERR_NOTILESET: 
         sprintf(error,"[E01] Tileset not found \n %s \n %s",where,memused);
         break;
      case DS_C_ERR_NOMAPBIN: 
         sprintf(error,"[E02] File Map.bin not found \n %s \n %s",where,memused);
         break;
      case DS_C_ERR_NOMEMORY: 
         sprintf(error,"[E03] Not enough memory \n %s \n %s",where,memused);
         break;
      case DS_C_ERR_NOMOREOBJECTS:
         sprintf(error,"[E04] This room has too many objects \n %s \n %s",where,memused);
         break;         
      case DS_C_ERR_NOWORLDINI: 
         sprintf(error,"[E05] No World.ini \n %s \n %s",where,memused);
         break;
      case DS_C_ERR_NOSAVEINI:
         sprintf(error,"[E06] No Savegame.ini \n %s \n %s",where,memused);
         break;   
		case DS_C_ERR_ZIPFAST:
		   sprintf(error,"[E07] Incompatible Map.bin (idx)\n %s \n %s",where,memused);
		   break;      
		case DS_C_ERR_NOHDD:
		   sprintf(error,"[E08] Not enough disk space \n %s \n %s",where,memused);
		   break;
		case DS_C_ERR_NOFOUND:
		   sprintf(error,"[E09] Not Found \n %s \n %s",where,memused);
		   break;
		case DS_C_ERR_BIGIMA:
		   sprintf(error,"[E10] Image too big\nSorry, you can't play this story\n%s \n %s",where,memused);
		   break;
	}
	
	// Prepare JSOD
	PA_Reset3DSprites2Banks();
	PA_3DProcess();
	ds_global_fillScreen(1,ds_global_getScreen1(),PA_RGB(16,0,0));
	ds_global_fillScreen(0,ds_global_getScreen0(),PA_RGB(16,0,0));
	
	// Fill JSOD
	PA_SmartText16bBuf_DS(ds_global_getScreen(1), 
										80, 48,  // base
										255 - 32, 191, // max
										"          -- Ouch! Sorry!!!\n         /\n    MMMM\n   MMMMMM\n  MMMMMMM\n MDMM===M\n MDM=X=X\n MDM====M\n MDMM===M\n MNNM-MM\n  MNM-M\n  MM--M",
										PA_RGB(31,31,31), 1, 1, 255); // Features
	PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 0,  // base
										255, 64, // max
										"KSDS Internal Error",PA_RGB(31,31,31), 3, 1); // Features
   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 0,  // base
										255, 191, // max
										error,PA_RGB(31,31,31), 2, 1); // Features
	PA_CenterSmartText16bBuf_DS(ds_global_getScreen(0), 
										0, 191 - 32,  // base
										255, 191, // max
										"Please reboot your DS",PA_RGB(31,31,31), 1, 1); // Features	
	// Show JSOD
   ds_global_paintScreen(1,ds_global_getScreen1(),0,0);
   ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
   
	// End...
   PA_DisableSpecialFx(0);
  	PA_DisableSpecialFx(1);
   while(1) {
	   PA_WaitForVBL();
	}
}

void ds_global_breakpoint(char *where, int X) {
char error[255];
char memused[100];

	sprintf(memused,"U: %d / F: %d   ", getMemUsed(), getMemFree());       				
	sprintf(error,"<Breakpoint> \n %s \n %s",where,memused);
	
	
	// Prepare JSOB
	//PA_Reset3DSprites2Banks();
	//PA_3DProcess();
	ds_global_fillScreen(0,ds_global_getScreen1(),PA_RGB(0,0,16));
	ds_global_fillScreen(1,ds_global_getScreen0(),PA_RGB(0,0,16));
	
	// Fill JSOB
	PA_SmartText16bBuf_DS(ds_global_getScreen(0), 
										80, 48,  // base
										255 - 32, 191, // max
										"          -- Breakpoint!!!\n         /\n    MMMM\n   MMMMMM\n  MMMMMMM\n MDMM===M\n MDM=X=X\n MDM====M\n MDMM===M\n MNNM-MM\n  MNM-M\n  MM--M",
										PA_RGB(31,31,31), 1, 1, 255); // Features
	PA_CenterSmartText16bBuf_DS(ds_global_getScreen(1), 
										0, 0,  // base
										255, 64, // max
										"KSDS Breakpoint",PA_RGB(31,31,31), 3, 1); // Features
   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(1), 
										0, 0,  // base
										255, 191, // max
										error,PA_RGB(31,31,31), 2, 1); // Features
  	if (X) {
		PA_CenterSmartText16bBuf_DS(ds_global_getScreen(1), 
										0, 191 - 32,  // base
										255, 191, // max
										"Please press X...",PA_RGB(31,31,31), 1, 1); // Features	
	} else {
		PA_CenterSmartText16bBuf_DS(ds_global_getScreen(1), 
										0, 191 - 32,  // base
										255, 191, // max
										"Please press Y...",PA_RGB(31,31,31), 1, 1); // Features	
	}   	

	// Show JSOB
   ds_global_paintScreen(1,ds_global_getScreen1(),0,0);
   ds_global_paintScreen(0,ds_global_getScreen0(),0,0);
   
	// Wait...
   PA_DisableSpecialFx(0);
  	PA_DisableSpecialFx(1);
  	if (X) {
	   while (!Pad.Newpress.X) {
	      PA_WaitForVBL();
		}
	} else {
	   while (!Pad.Newpress.Y) {
	      PA_WaitForVBL();
		}
	}   	
}

void ds_global_errorAssign(int error) {
   ds_global_error = error;
}   

int ds_global_errorHappened() {
   return (ds_global_error != DS_C_ERR_);
}

#define REG_BLDY(screen) *(volatile u16*)(0x04000054 + (screen * 0x1000))

void ds_global_fadeWhitePush(int speed) {
   if (_ds_global_fadeWhite < 17) {
	   PA_EnableSpecialFx(0, SFX_BRIGHTINC, SFX_BG0 | SFX_BG1 | SFX_BG2 | SFX_BG3 | SFX_OBJ | SFX_BD, 0);
	   PA_EnableSpecialFx(1, SFX_BRIGHTINC, SFX_BG0 | SFX_BG1 | SFX_BG2 | SFX_BG3 | SFX_OBJ | SFX_BD, 0);
	   int i, j;
	   for (i = _ds_global_fadeWhite; i < 17; i++) {
	      REG_BLDY(0) = i;
	      REG_BLDY(1) = i;
	      for (j = 0; j < speed; j++) {
	      	PA_WaitForVBL();  
			}
	   }
	   _ds_global_fadeWhite = 17;
	}	   
} 

void ds_global_fadeWhitePop() {
   if (_ds_global_fadeWhite > 0) {
	   PA_DisableSpecialFx(0);
   	PA_DisableSpecialFx(1);
   	
	   _ds_global_fadeWhite = 0;
	}	   
}  

void ds_global_fadeWhite_Ini() {
   if (!_ds_global_fadeWhite) {
	   PA_EnableSpecialFx(0, SFX_BRIGHTINC, SFX_BG0 | SFX_BG1 | SFX_BG2 | SFX_BG3 | SFX_OBJ | SFX_BD, 0);
	   PA_EnableSpecialFx(1, SFX_BRIGHTINC, SFX_BG0 | SFX_BG1 | SFX_BG2 | SFX_BG3 | SFX_OBJ | SFX_BD, 0);
	   _ds_global_fadeWhite = 1;
      REG_BLDY(0) = _ds_global_fadeWhite;
      REG_BLDY(1) = _ds_global_fadeWhite;
	}	   
} 

int ds_global_fadeWhite_Continue() {
   if (_ds_global_fadeWhite < 17) {
	   _ds_global_fadeWhite++;
	   if (_ds_global_fadeWhite == 17)
	      return 0; // Finished
      REG_BLDY(0) = _ds_global_fadeWhite;
      REG_BLDY(1) = _ds_global_fadeWhite;
      return 1; // Continue
	} else {
	   return 0; // Do not continue
	}   
} 

void ds_global_fadeWhite_Finish() {
   if (_ds_global_fadeWhite > 0) {
	   PA_DisableSpecialFx(0);
   	PA_DisableSpecialFx(1);
   	
	   _ds_global_fadeWhite = 0;
	}	   
}  

int ds_global_fadeWhite_CHECK() {
   return (_ds_global_fadeWhite != 0);
}   
