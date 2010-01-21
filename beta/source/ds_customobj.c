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
#include "ds_ini.h"
#include "ds_util.h"

char co_str[20];
char co_line[255];

ds_t_co *_listCO[256];

/* Inits the CO structure */
void ds_customobj_init() {
   int i;
	for (i = 0; i < 256; i++) {
	   _listCO[i] = NULL;
	}      
}

/* Resets the CO structure */
void ds_customobj_reset() {
   int i;
	for (i = 0; i < 256; i++) {
	   if (_listCO[i] != NULL) {
	      free(_listCO[i]);
	      _listCO[i] = NULL;
	   }   
	}      
}

/* Loads the CO structure from a certain .INI file */
void ds_customobj_load() {
   ds_t_co *myCO;
   
   if (ds_global_world.worldini == NULL) {
      return;
   }
   
	// Tries to find every CO in the ini file
	int i;
   for (i = 1; i < 255; i++) {
      sprintf(co_str,"Custom Object %d",i);
      if ((ds_ini_entry(ds_global_world.worldini, co_str)) && (_listCO[i] == NULL)) {
         myCO = malloc(sizeof(ds_t_co));
         
         // Custom object 'i' exists! let's get it...
         sprintf(co_line,"%s:Image",co_str);
         sprintf(myCO->Image,"%s",ds_ini_getstring(ds_global_world.worldini,co_line,"null"));
         sprintf(co_line,"%s:Init AnimFrom",co_str);
         myCO->Init_AnimFrom = ds_ini_getint(ds_global_world.worldini,co_line,0);
         sprintf(co_line,"%s:Init AnimTo",co_str);
         myCO->Init_AnimTo = ds_ini_getint(ds_global_world.worldini,co_line,0);
         sprintf(co_line,"%s:Init AnimSpeed",co_str);
         myCO->Init_AnimSpeed = ds_ini_getint(ds_global_world.worldini,co_line,500);
         sprintf(co_line,"%s:Init AnimLoopback",co_str);
         myCO->Init_AnimLoopback = ds_ini_getint(ds_global_world.worldini,co_line,0);
         sprintf(co_line,"%s:Init AnimRepeat",co_str);
         myCO->Init_AnimRepeat = ds_ini_getint(ds_global_world.worldini,co_line,0);
         sprintf(co_line,"%s:Tile Width",co_str);
         myCO->Tile_Width = ds_ini_getint(ds_global_world.worldini,co_line,24);
         sprintf(co_line,"%s:Tile Height",co_str);
         myCO->Tile_Height = ds_ini_getint(ds_global_world.worldini,co_line,24);
         sprintf(co_line,"%s:Offset X",co_str);
         myCO->Offset_X = ds_ini_getint(ds_global_world.worldini,co_line,0);
         sprintf(co_line,"%s:Offset Y",co_str);
         myCO->Offset_Y = ds_ini_getint(ds_global_world.worldini,co_line,0);
         
   		_listCO[i] = myCO;
      }   
   }   
}   

/* Checks for a certain CO */
int ds_customobj_exists(u8 obj) {
	return (_listCO[obj] != NULL);
}

/* Gets a certain CO */
ds_t_co *ds_customobj_get(u8 obj) {
   return _listCO[obj];
}   
