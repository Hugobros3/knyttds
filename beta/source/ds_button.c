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

#include "ds_15bpp.h"
#include "ds_linkedlist.h"
#include "PA_Text16bitBuffer.h"
#include "ds_music.h"


//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

typedef struct ds_tt_buttonText {
	// Content
	char text[255];
	u16 color;
	int size;
	int centered;
	int id;
	// Position
	int cx;
	int cy; 
} ds_t_buttonText;   

typedef struct ds_tt_buttonIma {
	// Content
	ds_t_15bpp ima;
	int id;
	// Position
	int cx;
	int cy; 
} ds_t_buttonIma;   

typedef struct ds_tt_button {
	// ID
	int id;
	// Position & size, screen & image
	int x;
	int y;
	int sx;
	int sy; 
	int screen;
	ds_t_15bpp image;
	char normalIma[255];
	// Management
	ds_t_fmanage *manage;
	u8 onManage;
	int manageNotClick;
	// Content <TODO> Convert this to a LL :-)
	ds_t_buttonText text[3];
	int ctext;
	ds_t_buttonIma ima[3];
	int cima;	
	// Others
	int hidden;
	int selected;
	// Variables
	int inner;
	// Special "onClick" image
	u8 onClick;
	char onClickIma[255];
	u8 onClickS;
	char onClickSnd[50];
	u8 onClickTimer;
} ds_t_button;   

LList *_ds_buttons;
int _ds_buttons_id;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------

void _ds_button_initButton(ds_t_button *btn) {
   btn->x = 0;
   btn->y = 0;
   btn->manage = NULL;
   btn->onManage = 0;
   btn->ctext = 0;
   btn->cima = 0;
   btn->hidden = 0;
   btn->selected = 0;
   btn->inner = 0;
   btn->onClick = 0;
   btn->onClickS = 0;
   btn->onClickTimer = 0;
   ds_15bpp_init(&(btn->image));
}   

ds_t_buttonIma *_ds_button_newIma(ds_t_button *btn) {
   ds_t_buttonIma *res;
   
   btn->ima[btn->cima].id = btn->cima;
   res = &(btn->ima[btn->cima]);
   
   btn->cima++;   
   return res;
}   

ds_t_buttonText *_ds_button_newText(ds_t_button *btn) {
   ds_t_buttonText *res;
   
   btn->text[btn->ctext].id = btn->ctext;
   res = &(btn->text[btn->ctext]);
   
   btn->ctext++;   
   return res;
}   

ds_t_buttonIma *_ds_button_getIma(ds_t_button *btn, int cima) {
   if (cima < btn->cima)
	   return &(btn->ima[cima]);
	else
		return NULL;
}   

ds_t_buttonText *_ds_button_getText(ds_t_button *btn, int ctext) {
   if (ctext < btn->ctext)
	   return &(btn->text[ctext]);
	else
		return NULL;
}   

void _ds_button_paintButton(ds_t_button *btn) {
   int i;
   ds_t_buttonIma *ima;
   ds_t_buttonText *text;
	
	// Paint image of the button   
	ds_15bpp_putScreen(ds_global_getScreen(btn->screen),&(btn->image),btn->x, btn->y,1);
	
	// Paint images   	
	i = 0;
	ima = _ds_button_getIma(btn,i);
	while (ima != NULL) {
	   ds_15bpp_putScreenAlpha(ds_global_getScreen(btn->screen),&(ima->ima),btn->x + ima->cx, btn->y + ima->cy);
	   i++;
	   ima = _ds_button_getIma(btn,i);
	}   
   
	// Paint text   	
	i = 0;
	text = _ds_button_getText(btn,i);
	while (text != NULL) {
	   if (!text->centered)
		   PA_SmartText16bBuf_DS(ds_global_getScreen(btn->screen), 
										btn->x + text->cx, btn->y + text->cy,  // base
										btn->x + btn->sx, btn->y + btn->sy, // max
										text->text, text->color, text->size, 1, 255); // Features
		else									
		   PA_CenterSmartText16bBuf_DS(ds_global_getScreen(btn->screen), 
										btn->x + text->cx, btn->y + text->cy,  // base
										btn->x + btn->sx, btn->y + btn->sy, // max
										text->text, text->color, text->size, 1); // Features		
	   i++;
	   text = _ds_button_getText(btn,i);
	}   
}   

void _ds_button_deleteContents(ds_t_button *btn) {
   int i;
   ds_t_buttonIma *ima;
   
   ds_15bpp_delete(&(btn->image));

	i = 0;
	ima = _ds_button_getIma(btn,i);
	while (ima != NULL) {
	   ds_15bpp_delete(&(ima->ima));
	   i++;
	   ima = _ds_button_getIma(btn,i);	   
	}	
}

int _ds_button_updateLocalImage(ds_t_button *btn, char *ima) {
   if (btn != NULL) {
      ds_15bpp_delete(&(btn->image));
      ds_15bpp_load(ima,&(btn->image),1,0); // If it fails, doesn't matter (just don't show)
   }   
   return 0;
}

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

void ds_button_init() {
   _ds_buttons = ds_linkedlist_init();
}   

void ds_button_reset() {
   void *it;
   ds_t_button *btn;

	// Delete the contents of the buttons   
   it = ds_linkedlist_startIterator(_ds_buttons);
   while ((btn = ds_linkedlist_getIterator(_ds_buttons,&it)) != NULL) {
      _ds_button_deleteContents(btn);
   }   
   
   // Now, Just delete the contents of this list :-) (but not the memory of the list)
   ds_linkedlist_finish(_ds_buttons,1,0);
}   

int ds_button_getID(void *btn) {
   if (btn != NULL)
   	return ((ds_t_button *)btn)->id;
   else
   	return -1;
}   

int ds_button_create(int x, int y, int sx, int sy, int screen, char *image) {
   ds_t_button *btn;
   
   // Creates the button and fills the contents
   btn = malloc(sizeof(ds_t_button));
   if (btn == NULL) {
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		sprintf(ds_global_string,"Create Button - %d/%d",x,y);
	   ds_global_errorHalt(ds_global_string);
   }   
   _ds_button_initButton(btn);
   btn->x = x;
   btn->y = y;
   btn->sx = sx;
   btn->sy = sy;
   btn->screen = screen;
	ds_15bpp_load(image,&(btn->image),1,0);
	sprintf(btn->normalIma,"%s",image);
				   
   btn->id = ds_linkedlist_add(_ds_buttons,btn);
   
   return btn->id;
}

int ds_button_addClick(int id, int time, char *ima) {
   ds_t_button *btn;   
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
		btn->onClick = time;
		sprintf(btn->onClickIma,"%s",ima);
      return btn->id;
   }   
   
   return -1;
}   

int ds_button_addSound(int id, char *snd) {
   ds_t_button *btn;   
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
		btn->onClickS = 1;
		sprintf(btn->onClickSnd,"%s",snd);
		return btn->id;
   }   
   
   return -1;
}
   

int ds_button_addImage(int id, int cx, int cy, char *ima) {
   ds_t_button *btn;   
   ds_t_buttonIma *btnIma;
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      btnIma = _ds_button_newIma(btn);
      btnIma->cx = cx;
      btnIma->cy = cy;      
      ds_15bpp_load(ima,&(btnIma->ima),1,0); // If it fails, doesn't matter (just don't show)
      return btnIma->id;
   }   
   
   return -1;
}   

int ds_button_addText(int id, int cx, int cy, char *text, u16 color, int size, int centered) {
   ds_t_button *btn;   
   ds_t_buttonText *btnText;
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      btnText = _ds_button_newText(btn);
      btnText->cx = cx;
      btnText->cy = cy;      
      btnText->color = color;     
		btnText->size = size;       
		btnText->centered = centered;
	  sprintf(btnText->text,"%s",text);
      return btnText->id;
   }   
   
   return -1;
}  

int ds_button_addManagement(int id, ds_t_fmanage manage, int manageNotClick) {
   ds_t_button *btn;   
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      btn->manage = manage;
      btn->manageNotClick = manageNotClick;
   }   
   return 1;
}   

void ds_button_setInner(int id, int inner) {
   ds_t_button *btn;   
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      btn->inner = inner;
   }   
}   

int ds_button_getInner(int id) {
   ds_t_button *btn;   
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      return btn->inner;
   }   
   return 0;   
}   

void ds_button_paint() {
   void *it;
   ds_t_button *btn;
   
   it = ds_linkedlist_startIterator(_ds_buttons);
   while ((btn = ds_linkedlist_getIterator(_ds_buttons,&it)) != NULL) {
      if (!btn->hidden)
	      _ds_button_paintButton(btn);
   }   
}   

int ds_button_checkClick(int stylusx,int stylusy, int onclick) {
   void *it;
   ds_t_button *btn;
   int execlick;
   int clicked;
   int res = 0;
   
   it = ds_linkedlist_startIterator(_ds_buttons);
   while ((btn = ds_linkedlist_getIterator(_ds_buttons,&it)) != NULL) {
      if (!btn->onManage) {
         // See if I have to manage it
	      execlick = (onclick)?1:btn->manageNotClick;
	      if ( (btn->manage != NULL) && (!btn->hidden) && (execlick) ) {
	         clicked = 0;
		      if ((stylusx >= btn->x) && (stylusx <= (btn->x + btn->sx)))
		      	if ((stylusy >= btn->y) && (stylusy <= (btn->y + btn->sy))) 
		      		clicked = 1;
		      if (clicked) {
	      	   // Signal Manage
	      		btn->onManage = 1; // Manages multiple buttons if they are in the same area ;-)
	      		// Start onClick
	      		if (btn->onClick != 0) {
	      		   btn->onClickTimer = btn->onClick;
	      		   _ds_button_updateLocalImage(btn,btn->onClickIma);
	      		   res = 1;
	      		} else {
	      		   btn->onClickTimer = 0; // Launch now!
					}     
	      	}		 
	      } 		
	   } 
		if (btn->onManage) {
	      // Manage!!!!! - Either manage "click" image or exe the manage method
	      if (btn->onClickTimer == 0) {
	      	// Restart image
	      	if (btn->onClick != 0) {
	      	   _ds_button_updateLocalImage(btn,btn->normalIma);
	      	   res = 1;
	      	}   
	      	// Manage
	      	btn->onManage = 0;
	      	btn->manage((void *)btn);
			if (btn->onClickS)
				ds_music_playSound(btn->onClickSnd,0,0);
	    	} else {
	    	   btn->onClickTimer--;
			}     	
		}      
   }   
   
   return res;
}   

int ds_button_hide(int id, int hidden) {
   ds_t_button *btn;   
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      btn->hidden = hidden;
   }   
   return 1;
}   

int ds_button_select(int id, int selected) {
   ds_t_button *btn;   
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      btn->selected = selected;
   }   
   return 1;
}   

/* Is this button selected? */
int ds_button_getSelect(int id) {
   ds_t_button *btn;   
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      return btn->selected;
   }   
   return 0;
}   

int ds_button_updateImage(int id, int imaid, char *ima) {
   ds_t_button *btn;   
   ds_t_buttonIma *btnIma;   
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      btnIma = _ds_button_getIma(btn,imaid);
      if (btnIma != NULL) {
         ds_15bpp_delete(&(btnIma->ima));
         ds_15bpp_load(ima,&(btnIma->ima),1,0); // If it fails, doesn't matter (just don't show)
      }   
   }   
   return 0;
}

int ds_button_updateLocalImage(int id, char *ima) {
   ds_t_button *btn;   
   int res = 0;
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
	   sprintf(btn->normalIma,"%s",ima);
	   res = _ds_button_updateLocalImage(btn, ima);
	}   
   return res;
}

int ds_button_updateLocalImageForce(int id, ds_t_15bpp *ima) {
   ds_t_button *btn;   
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      ds_15bpp_delete(&(btn->image));
      btn->image = *ima; // Copy the contents of the outside image
   }   
   return 0;
}

int ds_button_updateText(int id, int textid, char *text) {
   ds_t_button *btn;   
   ds_t_buttonText *btnText;
   
   btn = ds_linkedlist_get(_ds_buttons,id,NULL);
   if (btn != NULL) {
      btnText = _ds_button_getText(btn,textid);
      if (btnText != NULL) {
		 sprintf(btnText->text,"%s",text);
      }         
   }   
   return 0;   
}   

int ds_button_getLocalX_p(void *btnvoid, int x) {
   ds_t_button *btn = btnvoid;
   if (btn == NULL)
   	return 0;
   else
	   return x - btn->x;
} 

int ds_button_getLocalY_p(void *btnvoid, int y) {
   ds_t_button *btn = btnvoid;
   if (btn == NULL)   
   	return 0;
   else
	   return y - btn->y;
}  
