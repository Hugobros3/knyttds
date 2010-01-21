/* This library is taken from the PALib sources, and should have the same license as the 
	original PALib sources.
			- RRC2Soft */

#ifndef _PA_TEXT16BITBUFFER_
#define _PA_TEXT16BITBUFFER_

s16 PA_SmartText16bBuf_DS(u16 *screen, s16 basex, s16 basey, s16 maxx, s16 maxy, char* text, u16 color, u8 size, u8 transp, s32 limit);
s16 PA_CenterSmartText16bBuf_DS(u16 *screen, s16 basex, s16 basey, s16 maxx, s16 maxy, char* text, u16 color, u8 size, u8 transp);
s16 PA_CenterSmartText16bBuf_RAW(u16 *screen, s16 tamx, s16 tamy, s16 basex, s16 basey, s16 maxx, s16 maxy, char* text, u16 color, u8 size, u8 transp);


#endif
