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

/* Game "Utils" - Library of general purpose routines */
#ifndef _DS_UTIL_
#define _DS_UTIL_

#include "ds_global.h"
#include "ds_mapfile.h"

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Number Routines */
/*----------------*/

/* Creates a "Hex" string from a binary */
void ds_util_bin2str(char *dst, u8 *src, int len);

/* Creates a Binary array from a "Hex" string */
void ds_util_str2bin(u8 *dst, char *src);

/* Convert a number into a power of 2 */
int ds_util_convertPow2(int num);

/* File Routines */
/*---------------*/

/* Check if a certain file exists */
int ds_util_fileExists (char * fileName);

/* Get the date of a file */
long ds_util_fileGetDate (char * fileName);

/* Colour Routines */
/*-----------------*/

/* Alpha color! */
u16 ds_util_mixColorsAplha(u16 oldp, u16 newp, u8 alpha);

/* Alpha color 128! */
u16 ds_util_mixColorsAplha128(u16 oldp, u16 newp);

/* Headers Routines */
/*------------------*/

/* Gets header from opened room file Map.bin */
int ds_util_getroomheader(FILEMAP gzf, int *xpos, int *ypos);

/* SIN/COS Routines */
/*------------------*/

/* Fast and Low precision SIN, made by Michael Baczynski.
	Parameter is in Degrees. Need to change to PI. */
double ds_util_fastSin(double d);

/* Fast and Low precision COS, made by Michael Baczynski.
	Parameter is in Degrees. */
double ds_util_fastCos(double d);

/* ARRNUM Structure */
/*------------------*/

/* Resets an ArrNum structure */
void ds_util_arrNumReset(void *param);

/* Creates an ArrNum structure, who returns an unique ID number */
void *ds_util_arrNumInit(int size);

/* Frees an ArrNum structure */
void ds_util_arrNumFree(void *param);

/* Get the Number of "Elements" in an ArrNum structure */
int ds_util_arrNumElements(void *param);

/* Get the Max of "Elements" in an ArrNum structure */
int ds_util_arrNumMax(void *param);

/* Using the ArrNum structure, obtains an unique ID */
int ds_util_arrNum_Get(void *param);

/* Deletes an unique ID from the ArrNum structure */
void ds_util_arrNum_Del(void *param, int id);

/* Compare text, PAlib impl */
u8 PA_CompareText(char *text1, char *text2);

#endif
