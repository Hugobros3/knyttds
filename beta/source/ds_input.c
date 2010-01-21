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
#include "ds_util_bit.h"
#include "ds_util.h"
#include "ds_global.h"

u16 Lock; 
u8 _newpress;
u8 _held;
u8 _released;

/* Inits the library */
void ds_input_init(ds_t_input *input) {
   input->Newpress = 0;
   input->Held = 0;
   input->Released = 0;	   
   input->stylusx = 0;	   
   input->stylusy = 0;	   	   
   Lock = 0;
}

void _ds_input_change(ds_t_input *input, u8 val, int myheld) {
	_newpress = 0;
	_held = 0;
	_released = 0;
	if (myheld) {
	   if (ds_util_bitOne16(input->Released,val))
	   	_newpress = 1;
	   else if (ds_util_bitOne16(input->Newpress,val))
	   	_held = 1;
	   else if (ds_util_bitOne16(Lock,val))
	   	_held = 1;
	   else _newpress = 1;
	} else {
	   if (ds_util_bitOne16(Lock,val))
	   	_released = 1;
	   else if (ds_util_bitOne16(input->Released,val)) {}
	   else if (ds_util_bitOne16(input->Newpress,val))
	   	_released = 1;
	}   
	if (_newpress) {
	   input->Newpress = ds_util_bitSet16(input->Newpress,val);
	   input->Held = ds_util_bitDel16(input->Held,val);
	   Lock = ds_util_bitDel16(Lock,val);
	   input->Released = ds_util_bitDel16(input->Released,val);
	   if (val == DS_C_IN_STYLUS) {
		   input->stylusx = Stylus.X;	   
		   input->stylusy = Stylus.Y;	   	   	      
	   }	   
	}
	if (_held) {
	   input->Newpress = ds_util_bitDel16(input->Newpress,val);
	   input->Held = ds_util_bitSet16(input->Held,val);
	   Lock = ds_util_bitSet16(Lock,val);
	   input->Released = ds_util_bitDel16(input->Released,val);
	   if (val == DS_C_IN_STYLUS) {
		   input->stylusx = Stylus.X;	   
		   input->stylusy = Stylus.Y;	   	   	      
	   }   
	}      
	if (_released) {
	   input->Newpress = ds_util_bitDel16(input->Newpress,val);
	   input->Held = ds_util_bitDel16(input->Held,val);
	   Lock = ds_util_bitDel16(Lock,val);
	   input->Released = ds_util_bitSet16(input->Released,val);
	}      
}   

/* Return the values introduced by the pad */
void ds_input_getInputPad(ds_t_input *input) {
   // Inits input mechanism
   input->Held = 0;
   
   // Stores input - Every case
   _ds_input_change(input, DS_C_IN_LEFT, (Pad.Held.Left));
   _ds_input_change(input, DS_C_IN_RIGHT, (Pad.Held.Right));
   _ds_input_change(input, DS_C_IN_UP, (Pad.Held.Up));
   _ds_input_change(input, DS_C_IN_DOWN, (Pad.Held.Down));
   _ds_input_change(input, DS_C_IN_TJUMP, (Pad.Held.A));
   _ds_input_change(input, DS_C_IN_TWALK, (Pad.Held.B));
   _ds_input_change(input, DS_C_IN_TUMBR, (Pad.Held.X));
   _ds_input_change(input, DS_C_IN_THOLO, (Pad.Held.Y));
   _ds_input_change(input, DS_C_IN_L, (Pad.Held.L));
   _ds_input_change(input, DS_C_IN_R, (Pad.Held.R));
   _ds_input_change(input, DS_C_IN_START, (Pad.Held.Start));
   _ds_input_change(input, DS_C_IN_SELECT, (Pad.Held.Select));
   _ds_input_change(input, DS_C_IN_STYLUS, (Stylus.Held));
}   

/* Locks the "held" of certain buttons */
void ds_input_lock(u8 val) {
   Lock = ds_util_bitSet16(Lock,val);
}   


//---------------------------------------------------------------------------------------

/* Return the values introduced by the pad */
/* void ds_input_getInputPad(ds_t_input *input) {
   // Inits input mechanism
   input->Newpress = 0;
   input->Held = 0;
   input->Released = 0;	   
   
   // Stores input - Newpress
   if (Pad.Newpress.Left)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_LEFT);
   if (Pad.Newpress.Right)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_RIGHT);
   if (Pad.Newpress.Up)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_UP);
   if (Pad.Newpress.Down)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_DOWN);
   if (Pad.Newpress.A)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_TJUMP);
   if (Pad.Newpress.B)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_THOLO);
   if (Pad.Newpress.X)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_TUMBR);
   if (Pad.Newpress.Y)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_TWALK);
   if (Pad.Newpress.L)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_L);
   if (Pad.Newpress.R)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_R);
   if (Pad.Newpress.Start)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_START);
   if (Pad.Newpress.Select)
   	input->Newpress = ds_util_bitSet16(input->Newpress,DS_C_IN_SELECT);

   // Stores input - Held (if not locked)
   if ((Pad.Held.Left) && (!ds_util_bitOne16(Lock,DS_C_IN_LEFT)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_LEFT);
   	
   if ((Pad.Held.Right) && (!ds_util_bitOne16(Lock,DS_C_IN_RIGHT)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_RIGHT);
   	
   if ((Pad.Held.Up) && (!ds_util_bitOne16(Lock,DS_C_IN_UP)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_UP);
   	
   if ((Pad.Held.Down) && (!ds_util_bitOne16(Lock,DS_C_IN_DOWN)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_DOWN);
   	
   if ((Pad.Held.A) && (!ds_util_bitOne16(Lock,DS_C_IN_TJUMP)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_TJUMP);
   	
	if ((Pad.Held.B) && (!ds_util_bitOne16(Lock,DS_C_IN_THOLO)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_THOLO);
   	
	if ((Pad.Held.X) && (!ds_util_bitOne16(Lock,DS_C_IN_TUMBR)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_TUMBR);
   	
	if ((Pad.Held.Y) && (!ds_util_bitOne16(Lock,DS_C_IN_TWALK)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_TWALK);
   	
   if ((Pad.Held.L) && (!ds_util_bitOne16(Lock,DS_C_IN_L)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_L);
   	
   if ((Pad.Held.R) && (!ds_util_bitOne16(Lock,DS_C_IN_R)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_R);
   	
   if ((Pad.Held.Start) && (!ds_util_bitOne16(Lock,DS_C_IN_START)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_START);
   	
   if ((Pad.Held.Select) && (!ds_util_bitOne16(Lock,DS_C_IN_SELECT)))
   	input->Held = ds_util_bitSet16(input->Held,DS_C_IN_SELECT);

   // Stores input - Released. Also, unlocks the "helds"
   if (Pad.Released.Left) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_LEFT);
   	Lock = ds_util_bitDel16(Lock,DS_C_IN_LEFT);	
 	}  	
   if (Pad.Released.Right) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_RIGHT);
   	Lock = ds_util_bitDel16(Lock,DS_C_IN_RIGHT);
 	}  	
   if (Pad.Released.Up) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_UP);
   	Lock = ds_util_bitDel16(Lock,DS_C_IN_UP);
 	}  	
   if (Pad.Released.Down) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_DOWN);
   	Lock = ds_util_bitDel16(Lock,DS_C_IN_DOWN);   	
 	}  	
   if (Pad.Released.A) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_TJUMP);
		Lock = ds_util_bitDel16(Lock,DS_C_IN_TJUMP);
 	}  	
   if (Pad.Released.B) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_THOLO);
   	Lock = ds_util_bitDel16(Lock,DS_C_IN_THOLO);
 	}  	
   if (Pad.Released.X) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_TUMBR);
   	Lock = ds_util_bitDel16(Lock,DS_C_IN_TUMBR);
 	}  	
   if (Pad.Released.Y) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_TWALK);
   	Lock = ds_util_bitDel16(Lock,DS_C_IN_TWALK);
 	}  	
   if (Pad.Released.L) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_L);
	   Lock = ds_util_bitDel16(Lock,DS_C_IN_L);
 	}  	
   if (Pad.Released.R) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_R);
   	Lock = ds_util_bitDel16(Lock,DS_C_IN_R);
 	}  	
   if (Pad.Released.Start) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_START);
   	Lock = ds_util_bitDel16(Lock,DS_C_IN_START);
 	}  	
   if (Pad.Released.Select) {
   	input->Released = ds_util_bitSet16(input->Released,DS_C_IN_SELECT);
   	Lock = ds_util_bitDel16(Lock,DS_C_IN_SELECT);
 	}  	
}  */
