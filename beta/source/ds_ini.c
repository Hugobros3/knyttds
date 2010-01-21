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

#include "iniparser.h"
#include "ds_global.h"

static char retstr[255]; // For returning chars;

void *ds_ini_load(const char * ininame) {
	return iniparser_load(ininame);
}

void *ds_ini_loadSection(const char * ininame,const char * section) {
	return iniparser_loadTillSection(ininame,section);
}

void ds_ini_destroy(void *d) {
   iniparser_freedict((dictionary *) d);
}   

int ds_ini_entry(void *ini, char* entry) {
	return iniparser_find_entry((dictionary *) ini,entry);
}

void *ds_ini_getError() {
	return iniparser_err();
}
	

int ds_ini_getboolean(void * d, const char * key, int notfound) {
	return iniparser_getboolean((dictionary *) d, key, notfound);
}

int ds_ini_getint(void * d, const char * key, int notfound) {
	return iniparser_getint((dictionary *) d, key, notfound);
}	

char *ds_ini_getstring(void * d, const char * key, char * def) {
	return iniparser_getstring((dictionary *) d, key, def);
}



/* Returns a char based on the A-B-C entry */
char _ds_ini_returnABC(int type) {
   switch (type) {
      case DS_C_ELETYPE_A:
         return 'A';
      case DS_C_ELETYPE_B:
         return 'B';
      case DS_C_ELETYPE_C:
         return 'C';         
   }   
   return 'Z';
}   

/* Returns a char based on the direction of a warp */
char _ds_ini_returnLRUD(int type) {
   switch (type) {
      case 0:
         return 'U';
      case 1:
         return 'R';
      case 2:
         return 'D';         
      case 3:
         return 'L';                 
   }   
   return 'Z';
}   


/* Returns a "key" for a dictionary */
char *ds_ini_keyDictionary(int x, int y, int type, int paramA, int paramB, int paramC, int paramD) {
   retstr[0] = '\0';
   switch (type) {
      case DS_C_DICT_SIGN:
         sprintf(retstr,"x%dy%d:Sign(%c)",x,y,_ds_ini_returnABC(paramA));
         break;
      case DS_C_DICT_SHIFT_ONTOUCH:
         sprintf(retstr,"x%dy%d:ShiftTouch(%c)",x,y,_ds_ini_returnABC(paramA));
         break;         
      case DS_C_DICT_SHIFT_MAPXREL:
         sprintf(retstr,"x%dy%d:ShiftXMap(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                  
      case DS_C_DICT_SHIFT_MAPYREL:
         sprintf(retstr,"x%dy%d:ShiftYMap(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                           
      case DS_C_DICT_SHIFT_POSXREL:
         sprintf(retstr,"x%dy%d:ShiftXPos(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                  
      case DS_C_DICT_SHIFT_POSYREL:
         sprintf(retstr,"x%dy%d:ShiftYPos(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                    
      case DS_C_DICT_SHIFT_QUANTIZE:
         sprintf(retstr,"x%dy%d:ShiftQuantize(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                    
      case DS_C_DICT_SHIFT_ABSOLUTETARGET:
         sprintf(retstr,"x%dy%d:ShiftAbsoluteTarget(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                             
      case DS_C_DICT_SHIFT_DENYHOLOGRAM:
         sprintf(retstr,"x%dy%d:ShiftDenyHologram(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                             
      case DS_C_DICT_SHIFT_CUTSCENE:
         sprintf(retstr,"x%dy%d:ShiftCutscene(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                             
      case DS_C_DICT_SHIFT_SAVE:
         sprintf(retstr,"x%dy%d:ShiftSave(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                             
      case DS_C_DICT_SHIFT_FLAGON:
         sprintf(retstr,"x%dy%d:ShiftFlagOn(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                             
      case DS_C_DICT_SHIFT_FLAGOFF:
         sprintf(retstr,"x%dy%d:ShiftFlagOff(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                             
      case DS_C_DICT_SHIFT_TYPE:
         sprintf(retstr,"x%dy%d:ShiftType(%c)",x,y,_ds_ini_returnABC(paramA));
         break;           
      case DS_C_DICT_SHIFT_VISIBLE:
         sprintf(retstr,"x%dy%d:ShiftVisible(%c)",x,y,_ds_ini_returnABC(paramA));
         break;           		                                  
      case DS_C_DICT_FLAG:
         sprintf(retstr,"x%dy%d:Flag(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                             
      case DS_C_DICT_FLAGWARPX:
         sprintf(retstr,"x%dy%d:FlagWarpX(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                             
      case DS_C_DICT_FLAGWARPY:
         sprintf(retstr,"x%dy%d:FlagWarpY(%c)",x,y,_ds_ini_returnABC(paramA));
         break;                                             
      case DS_C_DICT_WARP_MAPX:
         sprintf(retstr,"x%dy%d:WarpX(%c)",x,y,_ds_ini_returnLRUD(paramA));
         break;                  
      case DS_C_DICT_WARP_MAPY:
         sprintf(retstr,"x%dy%d:WarpY(%c)",x,y,_ds_ini_returnLRUD(paramA));
         break;       
		case DS_C_DICT_ENDING:
         sprintf(retstr,"x%dy%d:Ending",x,y);
         break;
		case DS_C_DICT_GUI:
         sprintf(retstr,"x%dy%d:Gui",x,y);
         break;       		                                                		                                         
   }   
   return retstr;
}
