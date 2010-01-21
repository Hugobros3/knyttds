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

/* Game "Utils" - Library of general purpose routines, BIT specific */
#ifndef _DS_UTIL_BIT_
#define _DS_UTIL_BIT_

#include <PA9.h>       // Include for PA_Lib

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Bit Routines */
/*--------------*/

/* Check if a bit is 1 - 8 bit */
int ds_util_bitOne8(u8 num, u8 pos);

/* Check if a bit is 1 - 16 bit */
int ds_util_bitOne16(u16 num, u8 pos);

/* Puts a bit in 1 - 8 bit	*/
u8 ds_util_bitSet8(u8 num, u8 pos);

/* Puts a bit in 1 - 16 bit */
u16 ds_util_bitSet16(u16 num, u8 pos);

/* Puts a bit in 0 - 8 bit */
u8 ds_util_bitDel8(u8 num, u8 pos);

/* Puts a bit in 0 - 16 bit */
u16 ds_util_bitDel16(u16 num, u8 pos);

#endif
