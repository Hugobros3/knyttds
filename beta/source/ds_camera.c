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

/* Hm... some design about the camera

- MAIN Character
Juni: Game's about her!
Coordinates: When you want to see something different -:)

- WHO Influences
Juni: Basic movement, changes the camera.
Shoulder buttons: If they are pressed, the move the camera left/right. If not, come back to juni
Up/Down: Take a look to other parts of the game. Too bad the DS screen is so small...
Juni Running: You should see what is ahead of you... ^_-

- HOW Behaves - States
DS_C_CAM_JUNI: Camera is focusing on Juni.
DS_C_CAM_COORD: Camera is stored on a fixed coordinate.


*/

#include "ds_util_bit.h"
#include "ds_camera.h"
#include "ds_util.h"
#include "stdlib.h"

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES
//-------------------------------------------------------------------------------------------------

typedef struct ds_tt_camera{
   int x;
   int y;
   int type;
   int coordx;
   int coordy;
   int dx;
   int dy;   
   int changed;
} ds_t_camera;

//-------------------------------------------------------------------------------------------------
// INTERNAL VARS
//-------------------------------------------------------------------------------------------------

ds_t_camera _ds_camera_coord;

//-------------------------------------------------------------------------------------------------
// METHODS
//-------------------------------------------------------------------------------------------------

/* Initializes the values of the camera */
void ds_camera_initCamera(int x, int y) {
   _ds_camera_coord.x = x;
   _ds_camera_coord.y = y;
   _ds_camera_coord.coordx = x;
   _ds_camera_coord.coordy = y;   
   _ds_camera_coord.dx = 0;
   _ds_camera_coord.dy = 0;     
   _ds_camera_coord.changed = 0;
   _ds_camera_coord.type = DS_C_CAM_JUNI;
}   

/* Get the X coordinate of the camera */
int ds_camera_getX() {
   return _ds_camera_coord.x;
}   

/* Get the Y coordinate of the camera */
int ds_camera_getY() {
   return _ds_camera_coord.y;
}   

/* Set the absolute values of the camera */
void ds_camera_setXY(int x, int y) {
   _ds_camera_coord.x = x;
   _ds_camera_coord.y = y;
}

/* Set the camera position, based on the actual internal state, considering the main character on the coordinates provided */
void ds_camera_update(int junix, int juniy) {
   int actualX = 0;
   int actualY = 0;
   int distx = 0;
   int disty = 0;
   int distrel;   
   int velx;
   int vely;   
   
   switch (_ds_camera_coord.type) {
      case DS_C_CAM_JUNI:
         actualX = junix;
         actualY = juniy;         
         break;
      case DS_C_CAM_COORD:
         // <TODO> Fix usability :-P
         if ((_ds_camera_coord.dx == 0) && (_ds_camera_coord.dy == 0) && (!_ds_camera_coord.changed)) {
            // Go back to Juni!!!! - Both (Juni & Camera) in fixed position
            if ((junix > (600 - 128)) && (_ds_camera_coord.coordx > (600 - 128)))
            	_ds_camera_coord.coordx = junix;
            else if ((junix < 128) && (_ds_camera_coord.coordx < 128))
            	_ds_camera_coord.coordx = junix;            	
            if ((juniy > (240 - 96)) && (_ds_camera_coord.coordy > (240 - 96)))
            	_ds_camera_coord.coordy = juniy;
            else if ((juniy < 96) && (_ds_camera_coord.coordy < 96))
            	_ds_camera_coord.coordy = juniy;            	
            // Go back to Juni!!!! - Movement
            distx = _ds_camera_coord.coordx - (junix); // <TODO> centered
            disty = _ds_camera_coord.coordy - (juniy); // <TODO> centered
            distrel = (abs(distx)>abs(disty))?abs(distx):abs(disty);
            if (distrel <= (4)) {
               // Juni!!
               ds_camera_setType(DS_C_CAM_JUNI, junix, juniy);
	            _ds_camera_coord.coordx = junix;
	            _ds_camera_coord.coordy = juniy;               
            } else {
               if (abs(distx) > 256) velx = 16;
               else if (abs(distx) > 128) velx = 8;
               else if (abs(distx) > 80) velx = 4;
               else if (abs(distx) > 32) velx = 3;
               else if (abs(distx) > 4) velx = 2;
               else velx = 0;
               if (abs(disty) > 128) vely = 8;
               else if (abs(disty) > 64) vely = 4;
               else if (abs(disty) > 44) vely = 3;
               else if (abs(disty) > 16) vely = 2;
               else if (abs(disty) > 4) vely = 1;
               else vely = 0;               
               _ds_camera_coord.coordx -= (distx / abs(distx)) * velx;
               _ds_camera_coord.coordy -= (disty / abs(disty)) * vely;
            }  
         } else {
            // Move away!!! (but stay within the scroll limits :-P)
            _ds_camera_coord.coordx += _ds_camera_coord.dx;
            _ds_camera_coord.coordy += _ds_camera_coord.dy;
            if (_ds_camera_coord.dx != 0) {
	            if (_ds_camera_coord.coordx > (600 - 128))
	            	_ds_camera_coord.coordx = 600 - 128;
	            else if (_ds_camera_coord.coordx < 128)
	            	_ds_camera_coord.coordx = 128;            	
          	}  	
            if (_ds_camera_coord.dy != 0) {          	
	            if (_ds_camera_coord.coordy > (240 - 96))
	            	_ds_camera_coord.coordy = 240 - 96;
	            else if (_ds_camera_coord.coordy < 96)
	            	_ds_camera_coord.coordy = 96;            	            	
          	}  	
         }   
         
         // Actual change
         actualX = _ds_camera_coord.coordx;
         actualY = _ds_camera_coord.coordy;         
		   _ds_camera_coord.dx = 0; // Restart!
  			_ds_camera_coord.dy = 0; // Restart - yup!  			         
  			_ds_camera_coord.changed = 0; // Restart - yup^2!  			         
         break;         
   }   
   // X-coordinate
   if (actualX < 128) {
      _ds_camera_coord.x = 0;
   } else
   if (actualX > (600 - 128)) {
      _ds_camera_coord.x = 600 - 256;
   } else {
      _ds_camera_coord.x = actualX - 128;
   }    
   // Y-coordinate
   if (actualY < 96) {
      _ds_camera_coord.y = 0;
   } else
   if (actualY > (240 - 96)) {
      _ds_camera_coord.y = 240 - 192;
   } else {
      _ds_camera_coord.y = actualY - 96;
   }   
}

/* Set the type of management that the camera will use */
void ds_camera_setType(int type, int x, int y) {
   if ( type != _ds_camera_coord.type) {
	   _ds_camera_coord.type = type;
	   switch (_ds_camera_coord.type) {
	      case DS_C_CAM_JUNI:
	         break;
	      case DS_C_CAM_COORD:
			   _ds_camera_coord.coordx = x;
   			_ds_camera_coord.coordy = y;
			   _ds_camera_coord.dx = 0;
   			_ds_camera_coord.dy = 0;   			
   			_ds_camera_coord.changed = 1;
	         break;         
	   }      
	}   
}      

/* Indicates the camera that it should move in a certain direction in this frame */
void ds_camera_moveCoord(int x, int y, int factor) {
   _ds_camera_coord.dx += (x * factor);
	_ds_camera_coord.dy += (y * factor);   			   
	_ds_camera_coord.changed = 1;
}   
