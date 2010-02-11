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

#include "ds_util_bit.h"
#include "pngconf.h"
#include "png.h"
#include "ds_global.h"
#include "ds_util.h"

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

typedef struct ds_tt_pngds {
   png_structp png_ptr;
   png_infop info_ptr;
   png_infop end_info;
   int width, height;
} ds_t_pngds;

int _ds_pngerror;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-------------------------------------------------------------------------------------------------

void _png_error(png_structp png_ptr, png_const_charp error_msg) {
   _ds_pngerror = 1;
}   

void _png_warning(png_structp png_ptr, png_const_charp warning_msg) {
   _ds_pngerror = 1;
}


int _ds_15bpp_loadpng(char *file, ds_t_pngds *info) {
   
   FILE *fpng;
   
   // "" CHECK!!!!
   if (file[0] == '\0') {
      return 0;
   }   
   
	/* Opens PNG */
	/*-----------*/		
	fpng = fopen(file,"rb");
	if (fpng == NULL) {
	   ds_global_errorAssign(DS_C_ERR_NOFOUND);
	   return 0;
	}
	
	/* Creates Structures */
	/*--------------------*/		
	// <TODO> Manage lngjmp, delete memory, close file, check if it is png
	info->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL,
      png_voidp_NULL, png_voidp_NULL);
	if (info->png_ptr == NULL) {
	   fclose(fpng);
	   ds_global_errorAssign(DS_C_ERR_NOTILESET);
	   return 0;
	}   
    info->info_ptr = png_create_info_struct(info->png_ptr);
    if (info->info_ptr == NULL)
    {
      fclose(fpng);
      png_destroy_read_struct(&(info->png_ptr),(png_infopp)NULL,(png_infopp)NULL);
      ds_global_errorAssign(DS_C_ERR_NOTILESET);
		return 0;
    }
    info->end_info = png_create_info_struct(info->png_ptr);
    if (info->end_info == NULL)
    {
      fclose(fpng); 
      png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),(png_infopp)NULL);
		ds_global_errorAssign(DS_C_ERR_NOTILESET);
	   return 0;
    }    
    png_set_error_fn(info->png_ptr, (png_voidp)file, _png_error, _png_warning);
    
    /* Read PNG */
    /*----------*/
    // Initial steps - header
	 png_init_io(info->png_ptr, fpng);
	 png_read_info(info->png_ptr, info->info_ptr);
	 info->width = png_get_image_width(info->png_ptr, info->info_ptr);
	 info->height = png_get_image_height(info->png_ptr, info->info_ptr);	
	 // Transformations
  	 if (info->info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
	 {
	 	 png_set_palette_to_rgb(info->png_ptr);
	 	 png_read_update_info(info->png_ptr, info->info_ptr);
	 }
	 // Start reading rows
	 info->info_ptr->row_pointers = malloc(info->height * sizeof(png_bytep));
	 if (info->info_ptr->row_pointers == NULL)
	 {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"_ds_15bpp_loadpng 1 \n M:%ld",info->height * sizeof(png_bytep));
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
		 fclose(fpng); 
		 png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),(png_infopp)NULL);
		 ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		 return 0;
	 }
	 int row;
	 for (row = 0; row < info->height; row++) {
		 info->info_ptr->row_pointers[row] = malloc(png_get_rowbytes(info->png_ptr,info->info_ptr));
		 if (info->info_ptr->row_pointers[row] == NULL)
		 {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"_ds_15bpp_loadpng 2 \n M:%ld",png_get_rowbytes(info->png_ptr,info->info_ptr));
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
			 fclose(fpng); 
			 png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),&(info->end_info));
			 ds_global_errorAssign(DS_C_ERR_NOMEMORY);
			 return 0;
		 }
	 }   
	 png_read_image(info->png_ptr, info->info_ptr->row_pointers); // ****READ PNG****
 	 info->info_ptr->free_me |= PNG_FREE_ROWS;
			// Why? See _ds_15bpp_loadpngLimited :-).
	 png_read_end(info->png_ptr, info->end_info);

	 /* Everything OK... or NOT */
	 /*-------------------------*/
	 fclose(fpng);
	 
	 if (_ds_pngerror) {
	    png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),&(info->end_info));
	 	 return 0;
	 } else {   
	 	 return 1;
	 }	 
}

int _ds_15bpp_loadpngLimited(char *file, ds_t_pngds *info, int *sx, int *sy, 
									int maxsx, int maxsy, int maxArea) {
   
   FILE *fpng;
   int partialLoad;
   
   // "" CHECK!!!!
   if (file[0] == '\0') {
      return 0;
   }   
   
	/* Opens PNG */
	/*-----------*/		
	fpng = fopen(file,"rb");
	if (fpng == NULL) {
	   ds_global_errorAssign(DS_C_ERR_NOFOUND);
	   return 0;
	}
	
	/* Creates Structures */
	/*--------------------*/		
	// <TODO> Manage lngjmp, delete memory, close file, check if it is png
	info->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL,
	   png_voidp_NULL, png_voidp_NULL);
	if (info->png_ptr == NULL) {
	   fclose(fpng);
	   ds_global_errorAssign(DS_C_ERR_NOTILESET);
	   return 0;
	}   
	info->info_ptr = png_create_info_struct(info->png_ptr);
	if (info->info_ptr == NULL)
	{
		fclose(fpng);
		png_destroy_read_struct(&(info->png_ptr),(png_infopp)NULL,(png_infopp)NULL);
		ds_global_errorAssign(DS_C_ERR_NOTILESET);
		return 0;
	}
	png_set_error_fn(info->png_ptr, (png_voidp)file, _png_error, _png_warning);
	
	/* Read PNG - Progressive + Checks */
	/*---------------------------------*/
	// Load First Header information
	png_init_io(info->png_ptr, fpng);
	png_read_info(info->png_ptr, info->info_ptr);
	info->width = png_get_image_width(info->png_ptr, info->info_ptr);
	info->height = png_get_image_height(info->png_ptr, info->info_ptr);	
	// Corrections against BAD CO that have a wrong size!
	if ((*sx) > info->width)
		*sx = info->width;
	if ((*sy) > info->height)
		*sy = info->height;
	// Check what we have to load
	partialLoad = 0;    
	if (((*sx) > maxsx) && ((*sy) > maxsy))
	 partialLoad = 1;
	else if ((info->width * info->height) > maxArea)
	 partialLoad = 1;
	if (partialLoad) {
	 info->width = *sx;
	 info->height = *sy;
	}   
	// If partial load, destroys. If not, tries to create end_info.
	if (partialLoad) {
	   free(info->end_info);
	   info->end_info = NULL;
	} else {
		info->end_info = png_create_info_struct(info->png_ptr);
		if (info->end_info == NULL)
		{
			fclose(fpng); 
			png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),(png_infopp)NULL);
			ds_global_errorAssign(DS_C_ERR_NOTILESET);
			return 0;
		}    
	}  
	// Continues loading
	// Perform some transformations
	if (info->info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(info->png_ptr);
		png_read_update_info(info->png_ptr, info->info_ptr);
	}	
	// Get the row_pointers structure
   info->info_ptr->row_pointers = malloc(info->height * sizeof(png_bytep));
	if (info->info_ptr->row_pointers == NULL)
	{
		fclose(fpng); 
		png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),&(info->end_info));
		ds_global_errorAssign(DS_C_ERR_NOTILESET);
		return 0;
	}
	int row;
	for (row = 0; row < info->height; row++)
		info->info_ptr->row_pointers[row] = NULL;
   // Load the row_pointers
   for (row = 0; row < info->height; row++) {
      // Load the whole row
   	info->info_ptr->row_pointers[row] = malloc(png_get_rowbytes(info->png_ptr,info->info_ptr));
		if (info->info_ptr->row_pointers[row] == NULL)
		{
			fclose(fpng); 
			png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),&(info->end_info));
			ds_global_errorAssign(DS_C_ERR_NOTILESET);
			return 0;
		}
   	png_read_row(info->png_ptr, info->info_ptr->row_pointers[row], NULL); 
   	// Now, USE ONLY the pixels we need :-P
   	if (partialLoad) {
   		int pixD = info->info_ptr->pixel_depth / 8; // Bytes per pixel
   		int rowD = pixD * info->width;
   		int rowR = png_get_rowbytes(info->png_ptr,info->info_ptr);
   		if (rowD < rowR) {
	   		png_byte *real_row = malloc(rowD);
	   		memcpy(real_row,info->info_ptr->row_pointers[row],rowD);
	   		free(info->info_ptr->row_pointers[row]);
	   		info->info_ptr->row_pointers[row] = real_row;
	  		} 		
 		}  	
	}   
	if (partialLoad) {
		// NOTE: *DO NOT* call to png_read_end, as there is nothing to read for us.
		// Special "HACK" sections
		info->info_ptr->height = info->height;
			// Why? Internally, the row_pointers structure have some ???? pointers,
			// so any attempt on freeing them would lead to disaster. However, by performing
			// this trick, the png will only free the exact amount of pixels we have loaded ^_- 
			// NOTE that this trick is tested only with libpng 1.2.8 (the one we have used).
		info->info_ptr->valid |= PNG_INFO_IDAT;
			// Why? We tell the library that the image is valid -:)
			// NOTE that this trick is tested only with libpng 1.2.8 (the one we have used).
		info->info_ptr->free_me |= PNG_FREE_ROWS;
			// Why? We have saved the rows within the png infrastructure. Now, we need to
			// tell the PNG subsystem about that :-)
			// NOTE that this trick is tested only with libpng 1.2.8 (the one we have used).
	} else {
	   // We finish loading things :-)
	   png_read_end(info->png_ptr, info->end_info);
	   // But... also, some "HACK" section here...
		info->info_ptr->valid |= PNG_INFO_IDAT;
			// Why? We tell the library that the image is valid -:)
			// NOTE that this trick is tested only with libpng 1.2.8 (the one we have used).
		info->info_ptr->free_me |= PNG_FREE_ROWS;
			// Why? We have saved the rows within the png infrastructure. Now, we need to
			// tell the PNG subsystem about that :-)
			// NOTE that this trick is tested only with libpng 1.2.8 (the one we have used).
	}   			
	   	
	/* Everything OK... or NOT */
	/*-------------------------*/
	fclose(fpng);
	
	if (_ds_pngerror) {
		png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),&(info->end_info));
		return 0;
	} else {   
		return 1;
	}	 
}

int _ds_15bpp_load15bpp(ds_t_pngds *info, ds_t_15bpp *ima) {   
   int i,j,val;
   png_bytepp row_pointers;
   const png_byte * row_ptr;
   unsigned char r=0, g=0, b=0, a=0;
   
   // <TODO> Parameter Check
   
   /* Stores Secondary Information */
   /*------------------------------*/
   ima->is_alpha = (info->info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA);
	ima->width = info->width;
	ima->height = info->height;
   
   /* Create Memory for Arrays */
   /*--------------------------*/
   ima->png_screen = malloc((info->width * info->height) * 2); // Stores words (u16), not bytes (u8)
   if (ima->png_screen == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"_ds_15bpp_load15bpp 1 \n M:%d",(info->width * info->height) * 2);
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return 0;
   }   
   ima->png_alpha = malloc(info->width * info->height); // Stores bytes (u8)
   if (ima->png_alpha == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"_ds_15bpp_load15bpp 2 \n M:%d",(info->width * info->height));
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
      free(ima->png_screen);
      ima->png_screen = NULL;
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return 0;
   }   
   
   /* Copies Information from png to arrays */
   /*---------------------------------------*/
   row_pointers = png_get_rows(info->png_ptr, info->info_ptr);
	for (j=0; j < info->height; j++) {
		row_ptr = row_pointers[j];
		for (i= 0; i < info->width; i++) {
		   switch(info->info_ptr->color_type) {
					case PNG_COLOR_TYPE_RGB:
						r = *row_ptr++;
						g = *row_ptr++;
						b = *row_ptr++;
						break;
					case PNG_COLOR_TYPE_RGB_ALPHA:
						r = *row_ptr++;
						g = *row_ptr++;
						b = *row_ptr++;
						a = *row_ptr++;
						break;		      
			}	
			// Stores normal information
			val = i + (j * info->width);
			ima->png_screen[val] = PA_RGB(r >> 3, g >> 3, b >> 3);
			// Stores transparent/alpha information
			if (ima->is_alpha)
				ima->png_alpha[val] = a;
			else {
			   ima->png_alpha[val] =
			    ((r == 0xff) && (g == 0x00) && (b == 0xff))?0x00:0xff; // Magenta == Transparency
			}   
			// <TODO> De-magentize an image
			if (ima->png_alpha[val] == 0x00)
				ima->png_screen[val] = 0;
		}
	}   

	if (!ima->is_alpha) {
		free(ima->png_alpha);
		ima->png_alpha = NULL;
	}			
	
	/* Everything OK... */
	/*------------------*/
	return 1;
}

int _ds_15bpp_load15bppCombined(char *file, ds_t_pngds *info, ds_t_15bpp *ima) {   
   FILE *fpng;
   
   // "" CHECK!!!!
   if (file[0] == '\0') {
      return 0;
   }   
	   
	/* Opens PNG */
	/*-----------*/		
	fpng = fopen(file,"rb");
	if (fpng == NULL) {
	   ds_global_errorAssign(DS_C_ERR_NOFOUND);
	   return 0;
	}
	
	/* Creates Structures */
	/*--------------------*/		
	// <TODO> Manage lngjmp, delete memory, close file, check if it is png
	info->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL,
	   png_voidp_NULL, png_voidp_NULL);
	if (info->png_ptr == NULL) {
	   fclose(fpng);
	   ds_global_errorAssign(DS_C_ERR_NOTILESET);
	   return 0;
	}   
	info->info_ptr = png_create_info_struct(info->png_ptr);
	if (info->info_ptr == NULL)
	{
		fclose(fpng);
		png_destroy_read_struct(&(info->png_ptr),(png_infopp)NULL,(png_infopp)NULL);
		ds_global_errorAssign(DS_C_ERR_NOTILESET);
		return 0;
	}
	png_set_error_fn(info->png_ptr, (png_voidp)file, _png_error, _png_warning);
	
	/* Start Read PNG - Progressive + Checks */
	/*---------------------------------------*/
	// Load First Header information
	png_init_io(info->png_ptr, fpng);
	png_read_info(info->png_ptr, info->info_ptr);
	info->width = png_get_image_width(info->png_ptr, info->info_ptr);
	info->height = png_get_image_height(info->png_ptr, info->info_ptr);	
	// Try to create end_info.
	info->end_info = png_create_info_struct(info->png_ptr);
	if (info->end_info == NULL)
	{
		fclose(fpng); 
		png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),(png_infopp)NULL);
		ds_global_errorAssign(DS_C_ERR_NOTILESET);
		return 0;
	}    

	// Continues loading
	// Perform some transformations
	if (info->info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(info->png_ptr);
		png_read_update_info(info->png_ptr, info->info_ptr);
	}	
	
	/* 15BPP - start preparing things */
	/*--------------------------------*/
   int i,j,val;
   png_bytep row_png;
   png_bytep row_ptr;
   unsigned char r=0, g=0, b=0, a=0;

	// Secondary information
   ima->is_alpha = (info->info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA);
	ima->width = info->width;
	ima->height = info->height;

	// Create memory for arrays
   ima->png_screen = malloc((info->width * info->height) * 2); // Stores words (u16), not bytes (u8)
   if (ima->png_screen == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"_ds_15bpp_load15bpp 1 \n M:%d",(info->width * info->height) * 2);
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
		fclose(fpng);
		png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),&(info->end_info));
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return 0;
   }   
   ima->png_alpha = malloc(info->width * info->height); // Stores bytes (u8)
   if (ima->png_alpha == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"_ds_15bpp_load15bpp 2 \n M:%d",(info->width * info->height));
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
		fclose(fpng);
		png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),&(info->end_info));
      free(ima->png_screen);
      ima->png_screen = NULL;
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return 0;
   }   
	
	/* Start Reading the PNG - Real read & Transformations */
	/*-----------------------------------------------------*/
   // Load the row_pointers
   for (j = 0; j < info->height; j++) {
      // Load the whole row
		//--------------------
   	row_png = malloc(png_get_rowbytes(info->png_ptr,info->info_ptr));
		if (row_png == NULL)
		{
			fclose(fpng); 
			png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),&(info->end_info));
			free(ima->png_screen);
			free(ima->png_alpha);
			ds_global_errorAssign(DS_C_ERR_NOMEMORY);
			return 0;
		}
   	png_read_row(info->png_ptr, row_png, NULL); 
		
      // Transform the row for 15bpp
		//----------------------------
		row_ptr = row_png;
		for (i= 0; i < info->width; i++) {
		   switch(info->info_ptr->color_type) {
					case PNG_COLOR_TYPE_RGB:
						r = *row_ptr++;
						g = *row_ptr++;
						b = *row_ptr++;
						break;
					case PNG_COLOR_TYPE_RGB_ALPHA:
						r = *row_ptr++;
						g = *row_ptr++;
						b = *row_ptr++;
						a = *row_ptr++;
						break;		      
			}	
			// Stores normal information
			val = i + (j * info->width);
			ima->png_screen[val] = PA_RGB(r >> 3, g >> 3, b >> 3);
			// Stores transparent/alpha information
			if (ima->is_alpha)
				ima->png_alpha[val] = a;
			else {
			   ima->png_alpha[val] =
			    ((r == 0xff) && (g == 0x00) && (b == 0xff))?0x00:0xff; // Magenta == Transparency
			}   
			// <TODO> De-magentize an image
			if (ima->png_alpha[val] == 0x00)
				ima->png_screen[val] = 0;
		}
		
      // Delete the png row
		//-------------------
		free(row_png);

	}   

	/* Finishes loading things */
	/*-------------------------*/	
   // We finish loading things :-)
	png_read_end(info->png_ptr, info->end_info);
	// But... also, some "HACK" section here...
	info->info_ptr->valid |= PNG_INFO_IDAT;
		// Why? We tell the library that the image is valid -:)
		// NOTE that this trick is tested only with libpng 1.2.8 (the one we have used).
		
	// Also, alpha details for final image
	if (!ima->is_alpha) {
		if (ima->png_alpha != NULL)
			free(ima->png_alpha);
		ima->png_alpha = NULL;
	}			

	   	
	/* Everything OK... or NOT */
	/*-------------------------*/
	fclose(fpng);
		
	if (_ds_pngerror) {
		png_destroy_read_struct(&(info->png_ptr),&(info->info_ptr),&(info->end_info));
		if (ima->png_screen != NULL) {
			free(ima->png_screen);
			ima->png_screen = NULL;
		}
		if (ima->png_alpha != NULL) {
			free(ima->png_alpha);
			ima->png_alpha = NULL;
		}
		return 0;
	} else {   
		return 1;
	}	 
}

int _ds_15bpp_load15bppLimited(ds_t_pngds *info, ds_t_15bpp *ima, int sx, int sy) {   
   int i,j,val;
   png_bytepp row_pointers;
   const png_byte * row_ptr;
   unsigned char r=0, g=0, b=0, a=0;
   int xFrames, yFrames, maxFrames;
   
   // <TODO> Parameter Check
   
   /* Stores Secondary Information */
   /*------------------------------*/
   ima->is_alpha = (info->info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA);
	ima->width = info->width;
	
	/* Calculate frames, sizes, etc */
	/*------------------------------*/
	xFrames = info->width / sx; 
	yFrames = info->height / sy;
	maxFrames = xFrames * yFrames;
	ima->height = maxFrames * sy;
   
   /* Create Memory for Arrays */
   /*--------------------------*/
   ima->png_screen = malloc((sx * sy * maxFrames) * 2); // Stores words (u16), not bytes (u8)
   if (ima->png_screen == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"_ds_15bpp_load15bppLimited 1 \n M:%d",(sx * sy * maxFrames) * 2);
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return 0;
   }   
   ima->png_alpha = malloc(sx * sy * maxFrames); // Stores bytes (u8)
   if (ima->png_alpha == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"_ds_15bpp_load15bppLimited 2 \n M:%d",(sx * sy * maxFrames));
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
      free(ima->png_screen);
      ima->png_screen = NULL;
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return 0;
   }   
   
   /* Copies Information from png to arrays */
   /*---------------------------------------*/
   row_pointers = png_get_rows(info->png_ptr, info->info_ptr);
	for (j = 0; j < (sy * yFrames); j++) { // Get only the important frames
		row_ptr = row_pointers[j];
		for (i = 0; i < (sx * xFrames); i++) {
		   switch(info->info_ptr->color_type) {
					case PNG_COLOR_TYPE_RGB:
						r = *row_ptr++;
						g = *row_ptr++;
						b = *row_ptr++;
						break;
					case PNG_COLOR_TYPE_RGB_ALPHA:
						r = *row_ptr++;
						g = *row_ptr++;
						b = *row_ptr++;
						a = *row_ptr++;
						break;		      
			}
			// Stores normal information
			val = (i % sx) + // pixel in row
					((i / sx) * sx * sy) + ((j % sy) * sx) + // Quadrant X / Y
					((j / sy) * sx * sy * xFrames); // complete row
				// Remember! Special coordinates
			ima->png_screen[val] = PA_RGB(r >> 3, g >> 3, b >> 3);
			// Stores transparent/alpha information
			if (ima->is_alpha)
				ima->png_alpha[val] = a;
			else {
			   ima->png_alpha[val] =
			    ((r == 0xff) && (g == 0x00) && (b == 0xff))?0x00:0xff; // Magenta == Transparency
			}   
			// <TODO> De-magentize an image
			if (ima->png_alpha[val] == 0x00)
				ima->png_screen[val] = 0;
		}
	}   

	if (!ima->is_alpha) {
		free(ima->png_alpha);
		ima->png_alpha = NULL;
	}			
	
	/* Everything OK... */
	/*------------------*/
	return 1;
}

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Inits a 15bpp structure. */
void ds_15bpp_init(ds_t_15bpp *ima) {
   // "Partial" Fields
   ima->f = NULL;
   ima->tile = NULL;
   // Normal Fields
   ima->is_alpha = 0;
   ima->actualIndex = -1;
   ima->width = 0;
   ima->height = 0;
   ima->loaded = 0;   
   ima->png_screen = NULL;
   ima->png_alpha = NULL;
   ima->deleteme = 1;
}   

/* Init a 15bpp structure by using an existing "raw" (pointer) image. 
		Note that if the noDel parameter is active, this can be passed to _delete, 
			but the raw will not be freed by it */
void ds_15bpp_initRaw(ds_t_15bpp *ima, u16 *rawImage, int width, int height, int noDel) {
   // "Partial" Fields
   ima->f = NULL;
   ima->tile = NULL;
   // Normal Fields
   ima->is_alpha = 0;
   ima->actualIndex = -1;
   ima->width = width;
   ima->height = height;
   ima->loaded = 1;   
   ima->png_screen = rawImage;
   ima->png_alpha = NULL;
   ima->deleteme = !noDel;
}   


/* Destroys a 15bpp structure. */
void ds_15bpp_delete(ds_t_15bpp *ima) {   
   if (!(ima->deleteme)) // If I came from a raw image, DO NOT DELETE ME!!!
   	return;
   	
   if (ima->f != NULL) {
      fclose(ima->f);
      ima->f = NULL;
   }
   if (ima->tile != NULL) {
      free(ima->tile);
      ima->tile = NULL;
   }   
   	
   if (ima->png_screen != NULL) {
      free(ima->png_screen);
      ima->png_screen = NULL;
   }   
   if (ima->png_alpha != NULL) {
      free(ima->png_alpha);
      ima->png_alpha = NULL;
   }
   
   ds_15bpp_init(ima);
}

/* Saves a 15bpp structure to the HDD, into a raw 15bpp file. 
	Also calls a callback function.
   NOTE: The Raw File is "tile-optimized" if indicated. */
int ds_15bpp_saveRawFileCallback(char *file, ds_t_15bpp *ima, int tileOptimized, void (*callback)()) {
   FILE *f;
   u16* ptr16;
   u8* ptr8;
   int s;
   int w,j,c;
   int offset;
	// Open file for saving
	f = fopen(file, "wb");
	if (f == NULL)
		// Ups...
		return 0;
		
	// Store image width / height
	s = 1;
	if (fwrite(&(ima->width),sizeof(int),s,f) != s) {
	   fclose(f);
	   return 0;
	}
	s = 1;   
	if (fwrite(&(ima->height),sizeof(int),s,f) != s) {
	   fclose(f);
	   return 0;
	}   
	
	// Store the image itself
	if (!tileOptimized) {
		s = (ima->width * ima->height);
		/*if (((s % 32) == 0) && (callback != NULL)) {
   	   for (c = 0; c < (s / 32); c++) {
				if (fwrite(ima->png_screen,sizeof(u16),32,f) != 32) {
				   fclose(f);
				   return 0;
				}   
				// Special: Call the callback function
				if (callback != NULL) {
					(*callback)();
				}			
			}			
		} else {*/
			if (fwrite(ima->png_screen,sizeof(u16),s,f) != s) {
			   fclose(f);
			   return 0;
			}
			// Special: Call the callback function
			if (callback != NULL) {
				(*callback)();
			}			
		//}   		
	} else {
	   // Linear tile write
	   w = 0;
	   for (c = 0; c < 128; c++) {
	      ptr16 = ima->png_screen;
	      offset = ((c % 16) * 24) + ((c / 16) * 384 * 24);
	      ptr16 += offset;
		   for (j = 0; j < 24; j++) {
				if (fwrite(ptr16,sizeof(u16),24,f) != 24) {
				   fclose(f);
				   return 0;
				}   		   
				ptr16 += 384; // Next line  
				w += 24; // We wrote these elements inside the file 
		   }   
			// Special: Call the callback function
			if (callback != NULL) {
				(*callback)();
			}			
	   }   
	   // "Dummy" write
	   offset = ((ima->width * ima->height) - (w));
	   ptr16 = ima->png_screen + w;
		if (fwrite(ptr16,sizeof(u16),offset,f) != offset) {
		   fclose(f);
		   return 0;
		}  	   
	}   	
	
	// Stores the alpha image... if it exists
	s = 1;
	if (fwrite(&(ima->is_alpha),sizeof(int),s,f) != s) {
	   fclose(f);
	   return 0;
	}   
	if (ima->is_alpha) {
	   if (!tileOptimized) {
		   s = (ima->width * ima->height);
		   if (fwrite(ima->png_alpha,sizeof(u8),s,f) != s) {
		   	fclose(f);
		   	return 0;
			}
			// Special: Call the callback function
			if (callback != NULL) {
				(*callback)();
			}
		} else {
		   // Linear tile write
		   w = 0;
		   for (c = 0; c < 128; c++) {
		      ptr8 = ima->png_alpha;
		      offset = ((c % 16) * 24) + ((c / 16) * 384 * 24);
		      ptr8 += offset;
			   for (j = 0; j < 24; j++) {
					if (fwrite(ptr8,sizeof(u8),24,f) != 24) {
					   fclose(f);
					   return 0;
					}   		   
					ptr8 += 384; // Next line  
					w += 24; // We wrote these elements inside the file 
			   }   
				// Special: Call the callback function
				if (callback != NULL) {
					(*callback)();
				}			
		   }   
		   // "Dummy" write
		   offset = ((ima->width * ima->height) - (w));
		   ptr8 = ima->png_alpha + w;
			if (fwrite(ptr8,sizeof(u8),offset,f) != offset) {
			   fclose(f);
			   return 0;
			}  	   
		}   	
	}   
	
	// Close the image
	fclose(f);
	
	return 1;
}   

/* Saves a 15bpp structure to the HDD, into a raw 15bpp file. 
   NOTE: The Raw File is "tile-optimized" if indicated. */
int ds_15bpp_saveRawFile(char *file, ds_t_15bpp *ima, int tileOptimized) {
   return ds_15bpp_saveRawFileCallback(file, ima, tileOptimized, NULL);
}
      
/* Loads a 15bpp raw structure from the HDD */
int ds_15bpp_loadRawFile(char *file, ds_t_15bpp *ima) {
   FILE *f;
   int s;
   
   // "" CHECK!!!!
   if (file[0] == '\0') {
      return 0;
   }   

	// Opens the file
	f = fopen(file,"rb");
	if (f == NULL) {
	   ds_global_errorAssign(DS_C_ERR_NOFOUND);
	   return 0;
	}
	
	// Load first parameters
	s = 1;
	if (fread(&(ima->width),sizeof(int),s,f) != s) {
	   fclose(f);
	   ds_global_errorAssign(DS_C_ERR_NOHDD);
	   return 0;
	}
	s = 1;   
	if (fread(&(ima->height),sizeof(int),s,f) != s) {
	   fclose(f);
	   ds_global_errorAssign(DS_C_ERR_NOHDD);
	   return 0;
	}   
	
	// Loads the image itself
   ima->png_screen = malloc((ima->width * ima->height) * 2); // Stores words (u16), not bytes (u8)
   if (ima->png_screen == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"ds_15bpp_loadRawFile 1 \n M:%d",(ima->width * ima->height) * 2);
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
      fclose(f);
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return 0;
   }   
	s = (ima->width * ima->height);
	if (fread(ima->png_screen,s,sizeof(u16),f) != sizeof(u16)) {
	   free(ima->png_screen);
	   fclose(f);
	   ds_global_errorAssign(DS_C_ERR_NOHDD);
	   return 0;
	}  
	
	// Loads the alpha image... if it exists. If not, create it.
	s = 1;
	if (fread(&(ima->is_alpha),sizeof(int),s,f) != s) {
	   free(ima->png_screen);
	   fclose(f);
	   ds_global_errorAssign(DS_C_ERR_NOHDD);
	   return 0;
	}   
   ima->png_alpha = malloc(ima->width * ima->height); // Stores bytes (u8)
   if (ima->png_alpha == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"ds_15bpp_loadRawFile 2 \n M:%d",(ima->width * ima->height));
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
      free(ima->png_screen);
      fclose(f);
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return 0;
   }   
	if (ima->is_alpha) {
	   s = (ima->width * ima->height);
	   if (fread(ima->png_alpha,s,sizeof(u8),f) != sizeof(u8)) {
	      free(ima->png_screen);
	      free(ima->png_alpha);
	   	fclose(f);
	   	ds_global_errorAssign(DS_C_ERR_NOHDD);
	   	return 0;
		}
	} else {
	   // Same process as in the png reading
	   // OPTIMIZATION - No need to do this, the "map" methods will do the task
	   /*int i;
	   for (i = 0; i < (ima->width * ima->height); i++) {
			if (ima->png_screen[i] == 0)
				ima->png_alpha[i] = 0x00;
			else 
				ima->png_alpha[i] = 0xff;	      
	   }*/   
	}     
	
	// Close the image
	fclose(f);
	
	return 1;
}

/* Loads a 15bpp INCOMPLETE raw structure from the HDD */
int ds_15bpp_loadRawFilePartial(char *file, ds_t_15bpp *ima, int tilesize) {
   int s;
   int i;
   
   // "" CHECK!!!!
   if (file[0] == '\0') {
      return 0;
   }   

	// Opens the file
	ima->f = fopen(file,"rb");
	if (ima->f == NULL) {
	   ds_global_errorAssign(DS_C_ERR_NOFOUND);
	   return 0;
	}
	setvbuf( ima->f, NULL, _IONBF, 0 ); // Used to speed up fseek operations
	
	// Load first parameters
	s = 1;
	if (fread(&(ima->width),sizeof(int),s,ima->f) != s) {
	   fclose(ima->f);
	   ima->f = NULL;
	   ds_global_errorAssign(DS_C_ERR_NOHDD);
	   return 0;
	}
	s = 1;   
	if (fread(&(ima->height),sizeof(int),s,ima->f) != s) {
	   fclose(ima->f);
	   ima->f = NULL;
	   ds_global_errorAssign(DS_C_ERR_NOHDD);
	   return 0;
	}   
	
	// "Loads" the image itself
   ima->png_screen = malloc((ima->width * ima->height) * 2); // Stores words (u16), not bytes (u8)
   if (ima->png_screen == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"ds_15bpp_loadRawFilePartial 1 \n M:%d",(ima->width * ima->height) * 2);
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
      fclose(ima->f);
      ima->f = NULL;
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return 0;
   }   
	s = (ima->width * ima->height) * sizeof(u16);
	if (fseek(ima->f,s,SEEK_CUR) != 0) {
	   free(ima->png_screen);
	   fclose(ima->f);
	   ima->f = NULL;
	   ds_global_errorAssign(DS_C_ERR_NOHDD);
	   return 0;
	}  
	
	// "Loads" the alpha image... if it exists. If not, create it.
	s = 1;
	if (fread(&(ima->is_alpha),sizeof(int),s,ima->f) != s) {
	   free(ima->png_screen);
	   fclose(ima->f);
	   ima->f = NULL;
	   ds_global_errorAssign(DS_C_ERR_NOHDD);
	   return 0;
	}   
   ima->png_alpha = malloc(ima->width * ima->height); // Stores bytes (u8)
   if (ima->png_alpha == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"ds_15bpp_loadRawFilePartial 2 \n M:%d",(ima->width * ima->height));
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
      free(ima->png_screen);
      fclose(ima->f);
      ima->f = NULL;
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
      return 0;
   }   
	if (ima->is_alpha) {
	   // All alpha operations will be done later
	   /*s = (ima->width * ima->height) * sizeof(u8);
	   if (fseek(ima->f,s,SEEK_CUR) != 0) {
	      free(ima->png_screen);
	      free(ima->png_alpha);
	   	fclose(ima->f);
	   	ima->f = NULL;
	   	ds_global_errorAssign(DS_C_ERR_NOHDD);
	   	return 0;
		}*/
	} else {
	   // Same process as in the png reading
	   // OPTIMIZATION - No need to do this, the "map" methods will do the task
	   /*for (i = 0; i < (ima->width * ima->height); i++) {
			if (ima->png_screen[i] == 0)
				ima->png_alpha[i] = 0x00;
			else 
				ima->png_alpha[i] = 0xff;	      
	   }*/   
	}     
	
	// DO NOT close the image :-P, but initialize the "tile" part
	ima->tile = malloc(tilesize);
	for (i=0;i<tilesize;i++)
		ima->tile[i] = 0;
		
	return 1;
}

/* Get a RAW Tile in a Partial image */
int ds_15bpp_getRawTile(ds_t_15bpp *ima, int tile) {
   int s;
   int offset = ((tile % 16) * 24) + ((tile / 16) * 384 * 24);
   int j;
   u16 *ptr16;
   u8 *ptr8;
   
   // "" CHECK!!!!
   if (ima->tile == NULL) {
      return 0;
   }   
	
	// Check how many bytes we need to seek for the image, seek
	s = sizeof(int) + // Width
		 sizeof(int) + // Height
		 ((tile * 24 * 24) * sizeof(u16)); // *OFFSET!*
	fseek(ima->f,s,SEEK_SET);
		 
	// Get the image
	ptr16 = ima->png_screen;
	ptr16 += offset;
	for (j = 0; j < 24; j++) {
	   if (fread(ptr16,sizeof(u16),24,ima->f) != 24) {
		   fclose(ima->f);
		   ima->f = NULL;
		   ds_global_errorAssign(DS_C_ERR_NOHDD);
		   return 0;
	   }   
	   ptr16 += 384; // Next line
	   //fseek(ima->f,360 * sizeof(u16),SEEK_CUR);
	}   
	
	//	Get the alpha
	if (ima->is_alpha) {		 
		// Check how many bytes we need to seek for the alpha, seek	
		s = sizeof(int) + // Width
			 sizeof(int) + // Height
			 (ima->width * ima->height) * sizeof(u16) + // Image
			 sizeof(int) + // is Alpha
			 ((tile * 24 * 24) * sizeof(u8)); // Offset alpha
		fseek(ima->f,s,SEEK_SET);
		// Get the alpha itself
	   ptr8 = ima->png_alpha;
		ptr8 += offset;
		for (j = 0; j < 24; j++) {
		   if (fread(ptr8,24,sizeof(u8),ima->f) != sizeof(u8)) {
			   fclose(ima->f);
			   ima->f = NULL;
			   ds_global_errorAssign(DS_C_ERR_NOHDD);
			   return 0;
		   }   
		   ptr8 += 384; // Next line
		   //fseek(ima->f,360 * sizeof(u8),SEEK_CUR);
		} 
	}
	
	// Signals that we got this tile
	ima->tile[tile] = 1;		

	// Finishes
	return 1;
}

/* Get if a RAW Tile is stored in a Partial image */
int ds_15bpp_storeRawTile(ds_t_15bpp *ima, int tile) {
   // "" CHECK!!!!
   if (ima->tile == NULL) {
      return 1; // :-P
   }   
	return ima->tile[tile];
}  

/* Loads a 15bpp structure from the HDD. The file can be either a png or a raw file.
		ispng - 1 if the file is a png, 0 if the file is a raw 15bpp 
		ispartial - 1 if the file (raw) is not completely loaded */
int ds_15bpp_load(char *file, ds_t_15bpp *ima, int ispng, int ispartial) {
   ds_t_pngds png;
   png.png_ptr = NULL;
   png.info_ptr = NULL;
   png.end_info = NULL;

	ds_15bpp_init(ima);
	_ds_pngerror = 0;
	
   if (ispng) {
		if (!_ds_15bpp_load15bppCombined(file, &png, ima)) {
   	   return 0;
	   }   
      png_destroy_read_struct(&png.png_ptr,&png.info_ptr,&png.end_info);
      ima->loaded = 1;
		
      /*if (!_ds_15bpp_loadpng(file,&png)) {
         // png_destroy_read_struct -> Done inside
   	   return 0;
	   }   
	   if (!_ds_15bpp_load15bpp(&png,ima)) {
	      png_destroy_read_struct(&png.png_ptr,&png.info_ptr,&png.end_info);
   	   return 0;
	   }   
      png_destroy_read_struct(&png.png_ptr,&png.info_ptr,&png.end_info);
      ima->loaded = 1;*/		
   } 
	else {
	   if (ispartial) {
	      if (!ds_15bpp_loadRawFilePartial(file,ima,128)) { // 128 (tiles per image)
	   	   return 0;
		   }
	   } else {   
	      if (!ds_15bpp_loadRawFile(file,ima)) {
	   	   return 0;
		   }   
		}   
	   ima->loaded = 1;
	}
	return 1;
}

/* Loads a 15bpp structure from the HDD. 
	If the sprite size is too big, or the area is too big, 
	we load ONLY the first frame. */
int ds_15bpp_loadLimited(char *file, ds_t_15bpp *ima, int *sx, int *sy, 
									int maxsx, int maxsy, int maxArea) {
   ds_t_pngds png;
   png.png_ptr = NULL;
   png.info_ptr = NULL;
   png.end_info = NULL;

	ds_15bpp_init(ima);
	_ds_pngerror = 0;
	
	// First step - Load the png, considering the limitations	
   if (!_ds_15bpp_loadpngLimited(file,&png,sx,sy,maxsx,maxsy,maxArea)) {
      // png_destroy_read_struct -> Done inside
	   return 0;
   }   
   // Second step - copy the png to the ima, considering the special tile arrangement	
   if (!_ds_15bpp_load15bppLimited(&png,ima,*sx,*sy)) {
      png_destroy_read_struct(&png.png_ptr,&png.info_ptr,&png.end_info);
	   return 0;
   }   
   png_destroy_read_struct(&png.png_ptr,&png.info_ptr,&png.end_info);
   
   // Finish
   ima->loaded = 1;
	return 1;
}

/* Resizes (reduces) a image into an empty (do not contain anything) image. Do not copy alpha information. No error checking */
int ds_15bpp_resize(ds_t_15bpp *dest,ds_t_15bpp *ori,int newx, int newy) {
   int i,j;
   u32 xtrav, ytrav;
   int factorx, factory;
   
   // Checks...
   if (!ori->loaded)
   	return 0;
   	
   // Inits...
   ds_15bpp_init(dest);
   dest->is_alpha = 0;
   dest->width = newx;
   dest->height = newy;
   dest->png_screen = malloc((dest->width * dest->height) * 2); // Stores words (u16), not bytes (u8);
   if (dest->png_screen == NULL) {
#ifdef DEBUG_KSDS
		sprintf(ds_global_string,"ds_15bpp_resize 1 \n M:%d",(dest->width * dest->height) * 2);
		ds_global_errorAssign(DS_C_ERR_NOMEMORY);
		ds_global_errorHalt(ds_global_string);
#endif
      ds_global_errorAssign(DS_C_ERR_NOMEMORY);
   	return 0;
 	}  	
   dest->png_alpha = NULL;
   
   // Converts... <TODO> Bilinear!!!!!!!!!!!
   factorx = (ori->width * 1000) / dest->width;
   factory = (ori->height * 1000) / dest->height;
   
   ytrav = 0;   
   for (j = 0; j < dest->height; j++) {
      xtrav = 0;
      for (i = 0; i < dest->width; i++) {
         dest->png_screen[i + (j * dest->width)] = ori->png_screen[(xtrav / 1000) + ((ytrav / 1000) * ori->width)];
         xtrav += factorx;
    	}  
		ytrav += factory;   
   }   
   
   // Ends...
   dest->loaded = 1;
   
   return 1;
}   

/* Paints a 15bpp into a certain screen buffer. Ignores alpha. The image must be x4. No check for boundaries (!). */
void ds_15bpp_putScreen(u16 *screen, ds_t_15bpp *ima, int x, int y, int flush) {
	u16 * ptrori;
	u16 * ptrdest;
	int j;
	
	if (flush)
		DC_FlushAll();  // <TODO> DC_FlushRange(src,len); - DC_InvalidateRange(dst,len);?         
	if (ima->loaded) {
		ptrori = (ima->png_screen);			
		ptrdest = (screen) + x;		
		for (j=0; j < ima->height; j++) {
		   //<TODO> If bugs, use memcopy <TODO> Check x4!!!!
			DMA_Copy((ptrori + (j * ima->width)),(void *)(ptrdest + ((j + y) * 600) ),ima->width >> 1,DMA_32NOW); 
			// Remember that the screen buffer is 600 long... is not a screen buffer "per se", is a MAP buffer -:)
		}
	}	
}

/* Paints a 15bpp **directly** into the screen. Ignores alpha. The image must be x4. No check for boundaries (!). */
void ds_15bpp_paintScreen(int dsscreen, ds_t_15bpp *ima, int x, int y, int flush) {
	u16 * ptrori;
	int j;
	
	if (flush)
		DC_FlushAll();  // <TODO> DC_FlushRange(src,len); - DC_InvalidateRange(dst,len);?         
	if (ima->loaded) {
		ptrori = (ima->png_screen);
		for (j=0; j < ima->height; j++) {
		   //<TODO> If bugs, use memcopy <TODO> Check x4!!!!
			DMA_Copy((ptrori + (j * ima->width)),
							(void *)(PA_DrawBg[dsscreen] + ((j + y) << 8) + x),
							ima->width >> 1,
							DMA_32NOW);
		}
	}	
}

/* Paints a 15bpp **directly** into the screen. Ignores alpha. The image must be x4. No check for boundaries (!).
	Also uses special buffer to indicate whether a row should be updated or not */
void ds_15bpp_paintScreenSpecial(int dsscreen, ds_t_15bpp *ima, int x, int y, int flush, u8 *buff) {
	u16 * ptrori;
	int j;
	
	if (flush)
		DC_FlushAll();  // <TODO> DC_FlushRange(src,len); - DC_InvalidateRange(dst,len);?         
	if (ima->loaded) {
		ptrori = (ima->png_screen);
		for (j=0; j < ima->height; j++) {
		   //<TODO> If bugs, use memcopy <TODO> Check x4!!!!
			if (buff[j])
				DMA_Copy((ptrori + (j * ima->width)),
								(void *)(PA_DrawBg[dsscreen] + ((j + y) << 8) + x),
								ima->width >> 1,
								DMA_32NOW);
		}
	}	
}

/* Paints a 15bpp into a certain screen buffer, Checking Limits. Ignores alpha. The image must be x4. */
void ds_15bpp_putScreenCropped(u16 *screen, ds_t_15bpp *ima, int x, int y, int flush, int xi, int yi, int xf, int yf) {
	u16 * ptrori;
	u16 * ptrdest;
	int j;
	int width = ima->width;
	int height = ima->height;
	int xc = 0;
	int yc = 0;
	
	if (flush)
		DC_FlushAll();  // <TODO> DC_FlushRange(src,len); - DC_InvalidateRange(dst,len);?         
	
	if (x < xi) {
	   width = ima->width - (xi - x);
	   if (width <= 0)
	   	return;
	   xc = xi - x;
	   x = xi;
	} else if (x >= (xf - ima->width)) {
	   width = xf - x;
	   if (width <= 0)
	   	return;
	}   
	if (y < yi) {
	   height = ima->height - (yi - y);
	   if (height <= 0)
	   	return;
	   yc = yi - y;
	   y = yi;
	} else if (y >= (yf - ima->height)) {
	   height = yf - y;
	   if (height <= 0)
	   	return;
	} 
		
	if (ima->loaded) {
		ptrori = (ima->png_screen + ((yc * ima->width)));			
		ptrdest = (screen) + x;		
		for (j=0; j < height; j++) {
		   //<TODO> If bugs, use memcopy <TODO> Check x4!!!!
			DMA_Copy((ptrori + (j * ima->width) + xc),(void *)(ptrdest + ((j + y) * 600) ),width,DMA_16NOW); 
			// Remember that the screen buffer is 600 long... is not a screen buffer "per se", is a MAP buffer -:)
		}
	}	
}

/* Paints a 15bpp **directly** into the screen. Checking Limits. Ignores alpha. The image must be x4. */
void ds_15bpp_paintScreenCropped(int dsscreen, ds_t_15bpp *ima, int x, int y, int flush, int xi, int yi, int xf, int yf) {
	u16 * ptrori;
	int j;
	int width = ima->width;
	int height = ima->height;
	int xc = 0;
	int yc = 0;

	if (x < xi) {
	   width = ima->width - (xi - x);
	   if (width <= 0)
	   	return;
	   xc = xi - x;
	   x = xi;
	} else if (x >= (xf - ima->width)) {
	   width = xf - x;
	   if (width <= 0)
	   	return;
	}  
	if (y < yi) {
	   height = ima->height - (yi - y);
	   if (height <= 0)
	   	return;
	   yc = yi - y;
	   y = yi;
	} else if (y >= (yf - ima->height)) {
	   height = yf - y;
	   if (height <= 0)
	   	return;
	} 
	
	if (flush)
		DC_FlushAll();  // <TODO> DC_FlushRange(src,len); - DC_InvalidateRange(dst,len);?         
	if (ima->loaded) {
		ptrori = (ima->png_screen + ((yc * ima->width)));
		for (j=0; j < height; j++) {
		   //<TODO> If bugs, use memcopy <TODO> Check x4!!!!
			DMA_Copy((ptrori + (j * ima->width) + xc),
							(void *)(PA_DrawBg[dsscreen] + ((j + y) << 8) + x),
							width,
							DMA_16NOW);
		}
	}	
}

/* Paints a 15bpp into a certain screen buffer. Ignores alpha. The image must be x4. No check for boundaries (!). */
void ds_15bpp_putScreenPartial(u16 *screen, ds_t_15bpp *ima, int x, int y, int lx, int ly, int sizex, int sizey) {
	u16 * ptrori;
	u16 * ptrdest;
	int j;

	//DC_FlushAll();  // <TODO> DC_FlushRange(src,len); - DC_InvalidateRange(dst,len);?         
	if (ima->loaded) {
		ptrori = (ima->png_screen) + lx + (ly * ima->width);			
		ptrdest = (screen) + x;		
		for (j=0; j < sizey; j++) {
		   //<TODO> If bugs, use memcopy <TODO> Check x4!!!!
			DMA_Copy((ptrori + (j * ima->width)),(void *)(ptrdest + ((j + y) * 600) ),sizex >> 1,DMA_32NOW); 
			// Remember that the screen buffer is 600 long... is not a screen buffer "per se", is a MAP buffer -:)
		}
	}	
}


/* Paints a 15bpp into a certain screen buffer. WITH alpha. No check for boundaries (!). */
void ds_15bpp_putScreenAlpha(u16 *screen, ds_t_15bpp *ima, int x, int y) {
	u16 * ptrori;
	u8 * ptrorialpha;
	u16 * ptrdest;
	int i,j;
	u16 pix, pix_old;
	u8 pix_alpha;	

	if (ima->png_alpha == NULL) {
		if (ima->loaded) {
		   ptrori = (ima->png_screen);
		   ptrdest = (screen) + x + (y * 600);
		   for (j = 0; j < ima->height; j++) {
		      for (i = 0; i < ima->width; i++) {
		         pix = *ptrori;
	   	      if (pix != 0) {
      	   		(*ptrdest) = pix;
	      		}   	
					ptrori++;
					ptrdest++;         
		      }   
		      ptrdest = (ptrdest) + 600 - ima->width; // Next row!
		   }   
		}
	} else {
		if (ima->loaded) {
		   ptrori = (ima->png_screen);
		   ptrorialpha = (ima->png_alpha);
		   ptrdest = (screen) + x + (y * 600);
		   for (j = 0; j < ima->height; j++) {
		      for (i = 0; i < ima->width; i++) {
					pix_alpha = *ptrorialpha;
					pix = *ptrori;	         
	   	      if (pix_alpha != 0x00) {
	      	      if (pix_alpha == 0xff)
	      	   		(*ptrdest) = pix;
	      	   	else {
	      	   	   pix_old = (*ptrdest);
	      	   	   (*ptrdest) = ds_util_mixColorsAplha(pix_old, pix, pix_alpha);
	      	   	}   
	      		}   	
					ptrori++;
					ptrorialpha++;
					ptrdest++;         
		      }   
		      ptrdest = (ptrdest) + 600 - ima->width; // Next row!
		   }   
		}	
	}		
}   

/* Paints a 15bpp into the **Screen**. WITH alpha. No check for boundaries (!). */
void ds_15bpp_paintScreenAlpha(int screen, ds_t_15bpp *ima, int x, int y) {
	u16 * ptrori;
	u8 * ptrorialpha;
	u16 * ptrdest;
	int i,j;
	u16 pix, pix_old;
	u8 pix_alpha;	

	if (ima->png_alpha == NULL) {
		if (ima->loaded) {
		   ptrori = (ima->png_screen);
		   ptrdest = (void *)(PA_DrawBg[screen] + (y << 8) + x);
		   for (j = 0; j < ima->height; j++) {
		      for (i = 0; i < ima->width; i++) {
					pix = *ptrori;	         
	   	      if (pix != 0) {
	     	   		(*ptrdest) = pix;
	      		}   	
					ptrori++;
					ptrdest++;         
		      }   
		      ptrdest = (ptrdest) + 256 - ima->width; // Next row!
		   }   
		}   
	} else {
		if (ima->loaded) {
		   ptrori = (ima->png_screen);
		   ptrorialpha = (ima->png_alpha);
		   ptrdest = (void *)(PA_DrawBg[screen] + (y << 8) + x);
		   for (j = 0; j < ima->height; j++) {
		      for (i = 0; i < ima->width; i++) {
					pix_alpha = *ptrorialpha;
					pix = *ptrori;	         
	   	      if (pix_alpha != 0x00) {
	      	      if (pix_alpha == 0xff)
	      	   		(*ptrdest) = pix;
	      	   	else {
	      	   	   pix_old = (*ptrdest);
	      	   	   (*ptrdest) = ds_util_mixColorsAplha(pix_old, pix, pix_alpha);
	      	   	}   
	      		}   	
					ptrori++;
					ptrorialpha++;
					ptrdest++;         
		      }   
		      ptrdest = (ptrdest) + 256 - ima->width; // Next row!
		   }   
		}	
	}		
}   

