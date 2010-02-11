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

#include <PA9.h>       // Include for PA_Lib
#include <sys/stat.h>
#include "ds_mapfile.h"

typedef struct _ds_tt_util_arrNum {
   u8* arr;  // Contains the used IDs
   int size; // Max size
   int Cnt;  // Actual position of the list
	int Num;  // Number of elements
	int Max;  // Maximum position with an element (*including* the last one)
	int Opt;  // Signals an optimization
} _ds_t_util_arrNum;

/* Creates a "Hex" string from a binary */
void ds_util_bin2str(char *dst, u8 *src, int len) {
   int i;
   int posDst = 0;
   for (i=0; i<len; i++) {
      dst[posDst] = (src[i] >> 4) + 65;
      dst[posDst+1] = (src[i] & 15) + 65;
      posDst += 2;
   }   
   dst[posDst] = '\0';
}   

/* Creates a Binary array from a "Hex" string */
void ds_util_str2bin(u8 *dst, char *src) {
   int i = 0;
   u8 val;
	while (i < strlen(src)) {
	   val = (((src[i] - 65) << 4) | (src[i+1] - 65));
	   dst[i / 2] = val;
	   i += 2;
	}   
}   

/* Convert a number into a power of 2 */
int ds_util_convertPow2(int num) {
   // I'm sure there is a better way to do this... ^_- (for and >>)
   if (num <= 8)
   	return 8;
   if (num <= 16)
   	return 16;   	
   if (num <= 32)
   	return 32;
   if (num <= 64)
   	return 64;
   if (num <= 128)
   	return 128;
   if (num <= 256)
   	return 256;
   return 256;
}      

/* Check if a certain file exists */
int ds_util_fileExists (char * fileName) {
   struct stat buf;
   int i = stat ( fileName, &buf );
   /* File found */
   if ( i == 0 )
	   return 1;
   return 0;       
}

/* Get the date of a file */
long ds_util_fileGetDate (char * fileName) {
   struct stat buf;
   struct tm* clock;
   long valSec;
   long valMin;
   long valHour;
   long valDay;
   long valYear;
   long val;
   
   int i = stat ( fileName, &buf );
   // File found? 
   if ( i == -1 )
	   return 0;
	   
	// Get Date, in format similar to Epoch
	clock = gmtime(&(buf.st_mtime));   
	valSec = clock->tm_sec;
	valMin = (60 * clock->tm_min);
	valHour = (3600 * clock->tm_hour);
	valDay = (86400 * clock->tm_yday);
	valYear = (31622400 * (clock->tm_year - 1970));
	val = valSec + valMin + valHour + valDay + valYear;
	   
   return val;       
}


/* Alpha color! */
/*u16 ds_util_mixColorsAplha(u16 oldp, u16 newp, u8 alpha) {
   u8 r = newp & 0x1f;
   u8 g = (newp >> 5) & 0x1f;
   u8 b = (newp >> 10) & 0x1f;
   u8 ro = oldp & 0x1f;
   u8 go = (oldp >> 5) & 0x1f;
   u8 bo = (oldp >> 10) & 0x1f;   
   return PA_RGB(((ro * (255 - alpha)) + (r * alpha)) >> 8,
	              ((go * (255 - alpha)) + (g * alpha)) >> 8,
					  ((bo * (255 - alpha)) + (b * alpha)) >> 8);
}*/

u16 ds_util_mixColorsAplha(u16 oldp, u16 newp, u8 alpha) {
   return PA_RGB((((oldp & 0x1f) * (255 - alpha)) + ((newp & 0x1f) * alpha)) >> 8,
	              ((((oldp >> 5) & 0x1f) * (255 - alpha)) + (((newp >> 5) & 0x1f) * alpha)) >> 8,
					  ((((oldp >> 10) & 0x1f) * (255 - alpha)) + (((newp >> 10) & 0x1f) * alpha)) >> 8);
}


/* Alpha color 128! */
u16 ds_util_mixColorsAplha128(u16 oldp, u16 newp) {
   u8 r = newp & 0x1f;
   u8 g = (newp >> 5) & 0x1f;
   u8 b = (newp >> 10) & 0x1f;
   u8 ro = oldp & 0x1f;
   u8 go = (oldp >> 5) & 0x1f;
   u8 bo = (oldp >> 10) & 0x1f;   
   return PA_RGB((ro + r) >> 1,
	              (go + g) >> 1,
					  (bo + b) >> 1);
}	  	  

/* Gets header from opened room file Map.bin */
int ds_util_getroomheader(FILEMAP gzf, int *xpos, int *ypos) {
   // <TODO> Credit!!!!
	int a;
	int posz = 0;
	int cval = 0;
	int cvalneg = 0;
	char dummy[10];

	// Get header
	while (!ds_mapfile_eof(gzf)) {
	   a = ds_mapfile_getc(gzf);
	   if (a == 'x') {
	      if (posz != 0) {
	         return 0; // Invalid header, x coord at an unexpected place - upper layers
	      }   
	   } 
		else
	   if (a == 'y') {
	      if (cvalneg) 
	      	*xpos = -cval;
	      else
	      	*xpos = cval;
     	   cval = 0;
		   cvalneg = 0;
	   } 
	   else 
	   if ((a >= '0') && (a <= '9')) {
	      cval = cval * 10 + (a - '0');
	   }   
	   else 
	   if (a == '-') {
	      cvalneg = 1;
	      cval = 0;
	   }   
	   else 
	   if (a == 0) {
	      if (cvalneg) 
	      	*ypos = -cval;
	      else
	      	*ypos = cval;
	      break; // GOTCHA!!!!!!!
	   }   
	   posz++;
	}   
		
	// Deletes "dummy" header
	if (ds_mapfile_eof(gzf)) { // ???? Error! (headers do not belong to the end of the file)
	   return 0;
	}   
	ds_mapfile_read(gzf,dummy,4);
	if (ds_mapfile_eof(gzf)) { // ???? Error! (headers do not belong to the end of the file)
	   return 0;
	}   
	return 1;
}   

/* Fast and Low precision SIN, made by Michael Baczynski.
	Parameter is in Degrees. Need to change to PI. */
double ds_util_fastSin(double d) {
   double sin;
   double x;
   
   //from degrees to PI
   x = d * 3.14159265/180.0;
   
	//always wrap input angle to -PI..PI
	if (x < -3.14159265)
	    x += 6.28318531;
	else
	if (x >  3.14159265)
	    x -= 6.28318531;
	
	//compute sine
	if (x < 0)
	    sin = 1.27323954 * x + .405284735 * x * x;
	else
	    sin = 1.27323954 * x - 0.405284735 * x * x;

	return sin;	
}

/* Fast and Low precision COS, made by Michael Baczynski
	Parameter is in Degrees. */
double ds_util_fastCos(double d) {
   double cos;
   double x;
   
   //from degrees to PI
   x = d * 3.14159265/180.0;
   
	//always wrap input angle to -PI..PI
	if (x < -3.14159265)
	    x += 6.28318531;
	else
	if (x >  3.14159265)
	    x -= 6.28318531;
	
	//compute cosine: sin(x + PI/2) = cos(x)
	x += 1.57079632;
	if (x >  3.14159265)
	    x -= 6.28318531;
	
	if (x < 0)
	    cos = 1.27323954 * x + 0.405284735 * x * x;
	else
	    cos = 1.27323954 * x - 0.405284735 * x * x;
	    
	return cos;
}

/* Resets an ArrNum structure */
void ds_util_arrNumReset(void *param) {
   if (param == NULL)
   	return;
   	
   _ds_t_util_arrNum *ptr = (_ds_t_util_arrNum *)param;
   
   int i;
   for (i = 0; i < ptr->size; i++) 
   	ptr->arr[i] = 0;
   ptr->Cnt = 0;
	ptr->Num = 0;
	ptr->Opt = 0;
	ptr->Max = -1;
}

/* Creates an ArrNum structure, who returns an unique ID number */
void *ds_util_arrNumInit(int size) {
   _ds_t_util_arrNum *ptr = (_ds_t_util_arrNum *) malloc(sizeof(_ds_t_util_arrNum));
   ptr->size = size;
   ptr->arr = (u8 *) malloc(ptr->size * sizeof(u8));
   ds_util_arrNumReset((void *) ptr);
   return (void *)ptr;
}

/* Frees an ArrNum structure */
void ds_util_arrNumFree(void *param) {
   if (param == NULL)
   	return;

   _ds_t_util_arrNum *ptr = (_ds_t_util_arrNum *)param;
   free(ptr->arr);
   free(param);
   param = NULL;
}

/* Get the Number of "Elements" in an ArrNum structure */
int ds_util_arrNumElements(void *param) {
   if (param == NULL)
   	return 0;

   _ds_t_util_arrNum *ptr = (_ds_t_util_arrNum *)param;
   
   return ptr->Num;
}

/* Get the Max of "Elements" in an ArrNum structure */
int ds_util_arrNumMax(void *param) {
   if (param == NULL)
   	return 0;

   _ds_t_util_arrNum *ptr = (_ds_t_util_arrNum *)param;
   
   return ptr->Max;
}

/* Using the ArrNum structure, obtains an unique ID */
int ds_util_arrNum_Get(void *param) {
   if (param == NULL)
   	return 0;

   int res;
   _ds_t_util_arrNum *ptr = (_ds_t_util_arrNum *)param;
   
   // Search next "free" position
   int arrpos = ptr->Cnt % ptr->size;
   while (ptr->arr[arrpos] != 0) {
      arrpos = (arrpos + 1) % ptr->size;
   }
	ptr->arr[arrpos] = 1;
	   
   // Store information
	res = arrpos;
	ptr->Max = (arrpos > ptr->Max)?arrpos:ptr->Max;
	ptr->Cnt = (arrpos + 1) % ptr->size;
	ptr->Num++;
	// Optimization - search for a better (lower) Cnt
	ptr->Opt++;
	if (ptr->Opt == 100) {
	   ptr->Opt = 0;
	   for (arrpos = 0; arrpos < ptr->Cnt; arrpos++) {
	      if (ptr->arr[arrpos] == 0) {
	         ptr->Cnt = arrpos;
	         break;
	      }   
	   }   
	}   
	return res;
}

/* Deletes an unique ID from the ArrNum structure */
void ds_util_arrNum_Del(void *param, int id) {
   if (param == NULL)
   	return;
   _ds_t_util_arrNum *ptr = (_ds_t_util_arrNum *)param;
   if (id >= ptr->size)
   	return;
   
   ptr->arr[id] = 0;
	ptr->Cnt = (id < ptr->Cnt)?id:ptr->Cnt; // Assuring a free place for next element :-D
	ptr->Num--;    // Normal, one element got away :-P
}

/* Compare text, PAlib impl */   
u8 PA_CompareText(char *text1, char *text2){
	u8 ok = 1; // Devrait etre bon
	u8 i = 0; 
	while(ok && text2[i]){
		if (text1[i] != text2[i]) ok = 0;
		i++;
	}
	return ok;
}

/* Compare Bounding Box - Adapted from John Amato routine @ Gamedev*/
int ds_boundingBox(int o1x, int o1y, int o1w, int o1h, 
									int o2x, int o2y, int o2w, int o2h) {
  
    int left1, left2;
    int right1, right2;
    int top1, top2;
    int bottom1, bottom2;

    left1 = o1x;
    left2 = o2x;
    right1 = o1x + o1w;
    right2 = o2x + o2w;
    top1 = o1y;
    top2 = o2y;
    bottom1 = o1y + o1h;
    bottom2 = o2y + o2h;

    if (bottom1 < top2) return 0;
    if (top1 > bottom2) return 0;

    if (right1 < left2) return 0;
    if (left1 > right2) return 0;

    return 1;

};
