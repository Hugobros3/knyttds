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

#include <stdlib.h>
#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_util.h"

/*
 <TODO> <FIXME> BUGS!!!! Mayne on SEEK, or TELL, but there are bugs *for sure*
*/

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

typedef struct ds_tt_mapbin {
   void *file;
   int type;
} ds_t_mapbin;   

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// EXTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Initializes the Mapfile system - call at the beginning of everything */
void ds_mapfile_init() {
}   

/* Opens a certain file */
void *ds_mapfile_open(char *path, char *mode, int specialmode) {
   ds_t_mapbin *_filep;
   char pathextra[255];
   sprintf(pathextra,"%s.raw",path);

   
   if (specialmode == DS_C_FILE_AUTO) {
      if ((ds_util_fileExists(pathextra)) && (ds_global_optimizationUncompress))
      	specialmode = DS_C_FILE_RAW;
      else
      	specialmode = DS_C_FILE_BIN;
   }
   
   _filep = malloc(sizeof(ds_t_mapbin));
   if (_filep == NULL)
   	return NULL;
   _filep->type = specialmode;
	switch (_filep->type) {
	   case DS_C_FILE_BIN:
	      _filep->file = gzopen(path,mode);
	      if (_filep->file == NULL) {
	         free(_filep);
	         return NULL;
	      }   
	      break;
	   case DS_C_FILE_RAW:
	      _filep->file = fopen(pathextra,mode);
	      if (_filep->file == NULL) {
	         free(_filep);
	         return NULL;
	      }   	      
	      break;
	}      
   return _filep;
}   

/* Seeks to a certain position */
z_off_t ds_mapfile_seek(void *file, z_off_t offset, int whence) {
   ds_t_mapbin *_filep;
   if (file == NULL) {
      return -1;
   }   
   _filep = (ds_t_mapbin *)file;
   if (_filep->type == DS_C_FILE_BIN) {
      return gzseek(_filep->file, offset, whence);
   } else {
      return fseek(_filep->file, offset, whence);
	}      
	return -1;
}   

/* Reads from the file */
int ds_mapfile_read(void *file, voidp buf, unsigned len) {
   ds_t_mapbin *_filep;
   if (file == NULL) {
      return -1;
   }   
   _filep = (ds_t_mapbin *)file;
   if (_filep->type == DS_C_FILE_BIN) {
      return gzread(_filep->file, buf, len);
   } else {
      return fread(buf, 1, len, _filep->file);
	}      
	return -1;
}

/* Closes the file */
int ds_mapfile_close(void *file) {
   ds_t_mapbin *_filep;
   int res = EOF;
	   
   if (file == NULL) {
      return res;
   }   
   _filep = (ds_t_mapbin *)file;
   if (_filep->type == DS_C_FILE_BIN) {
      res = gzclose(_filep->file);
      free(_filep);
   } else {
      res = fclose((FILE *)(_filep->file));
      free(_filep);
	}      
	return res;
}   

/* Checks EOF */
int ds_mapfile_eof(void *file) {
   ds_t_mapbin *_filep;
   if (file == NULL) {
      return 0;
   }   
   _filep = (ds_t_mapbin *)file;
   if (_filep->type == DS_C_FILE_BIN) {
      return gzeof(_filep->file);
   } else {
      return feof((FILE *)_filep->file);
	}      
	return 0;
}   

/* Gets character */
int ds_mapfile_getc(void *file) {
   ds_t_mapbin *_filep;
   
   if (file == NULL) {
      return -1;
   }   
   _filep = (ds_t_mapbin *)file;
   if (_filep->type == DS_C_FILE_BIN) {
      return gzgetc(_filep->file);
   } else {
      return fgetc((FILE *)(_filep->file));
	}      
	return -1;
}

/* Returns the starting position for the next read */
z_off_t ds_mapfile_tell(void *file) {
   ds_t_mapbin *_filep;
   fpos_t pos;

   if (file == NULL) {
      return -1;
   }   
   _filep = (ds_t_mapbin *)file;
   if (_filep->type == DS_C_FILE_BIN) {
      return gztell(_filep->file);
   } else {
      fgetpos(_filep->file,&pos);
      return pos;
      //return ftell(_filep->file);
	}      
	return -1;
}   

