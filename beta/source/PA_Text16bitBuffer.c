/* This library is taken from the PALib sources, and should have the same license as the 
	original PALib sources.
	Note that it uses the compressed font of new palib versions, but implementing the old painting method.
			- RRC2Soft */

#include "arm9/PA_Text.h"
#include "arm9/PA_Draw.h"
#include "arm9/PA_TextBits.h"

void Letter16(u8 size, u16 *screen, u16 xscreen, u16 yscreen, u16 x, u16 y, char letter, u16 color);
void TranspLetter16(u8 size, u16 *screen, u16 xscreen, u16 yscreen, u16 x, u16 y, char letter, u16 color);
void NoLetter16(u8 size, u16 *screen, u16 xscreen, u16 yscreen, u16 x, u16 y, char letter, u16 color);

typedef void(*letterfp16)(u8 size, u16 *screen, u16 xscreen, u16 yscreen, u16 x, u16 y, char lettertemp, u16 color);

letterfp16 letters16b[3] = {(letterfp16)&Letter16, (letterfp16)&TranspLetter16, (letterfp16)&NoLetter16};

u8 B16b_actual_size = 0;

void PA_Put16bitPixelBuffer(u16* screen, u16 xscreen, u16 yscreen, int x, int y, u16 color) {
   // No boundary checkings
   screen[x + (y * xscreen)] = color;
}   

extern inline void PA_16bitDrawTile(u16* screen, u16 xscreen, u16 yscreen, u16 x, u16 y, s8 lx, s8 ly, u8 *data, u16 color){
	u16 i, j;
	for (j = 0; j < ly; j++){
		for (i = 0; i < lx; i++){
			PA_Put16bitPixelBuffer(screen, xscreen, yscreen, x+i, y+j, color);
		}
	}
}	

extern inline void PA_16bitDrawTileTransp(u16* screen, u16 xscreen, u16 yscreen, u16 x, u16 y, s8 lx, s8 ly, u8 *data, u16 color){
	u16 i, j;
	for (j = 0; j < ly; j++){
		for (i = 0; i < lx; i++){
			if(data[i+(j<<3)]){
				PA_Put16bitPixelBuffer(screen, xscreen, yscreen, x+i, y+j, color);
			}
		}
	}
}

void NoLetter16(u8 size, u16* screen, u16 xscreen, u16 yscreen, u16 x, u16 y, char letter, u16 color) {

}

void Letter16(u8 size, u16* screen, u16 xscreen, u16 yscreen, u16 x, u16 y, char letter, u16 color) {
u8 lx = pa_bittextdefaultsize[size][(u8)letter];
u8 ly = pa_bittextpoliceheight[size];
u8 *data;

if (ly > 8) {
	u16 firstpos = ((letter&31)<<1)+((letter >> 5)<<7);
	data = (u8*)(bit8_tiles[size]+(bittext_maps[size][firstpos]<<6));
	PA_16bitDrawTile(screen, xscreen, yscreen, x, y, 8, 8, data, color);
	data = (u8*)(bit8_tiles[size]+(bittext_maps[size][firstpos+64]<<6));
	PA_16bitDrawTile(screen, xscreen, yscreen, x, y+8, 8, ly-8, data, color);	
	firstpos ++;
	data = (u8*)(bit8_tiles[size]+(bittext_maps[size][firstpos]<<6));
	PA_16bitDrawTile(screen, xscreen, yscreen, x+8, y, lx-8, 8, data, color);
	data = (u8*)(bit8_tiles[size]+(bittext_maps[size][firstpos+64]<<6));
	PA_16bitDrawTile(screen, xscreen, yscreen, x+8, y+8, lx-8, ly-8, data, color);			
}
else {
	data = (u8*)(bit8_tiles[size]+(bittext_maps[size][(u8)letter]<<6));
	PA_16bitDrawTile(screen, xscreen, yscreen, x, y, lx, ly, data, color);
}
}

void TranspLetter16(u8 size, u16* screen, u16 xscreen, u16 yscreen, u16 x, u16 y, char letter, u16 color) {
u8 lx = pa_bittextdefaultsize[size][(u8)letter];
u8 ly = pa_bittextpoliceheight[size];
u8 *data;

if (ly > 8) {
	u16 firstpos = ((letter&31)<<1)+((letter >> 5)<<7);
	data = (u8*)(bit8_tiles[size]+(bittext_maps[size][firstpos]<<6));
	PA_16bitDrawTileTransp(screen, xscreen, yscreen, x, y, 8, 8, data, color);
	data = (u8*)(bit8_tiles[size]+(bittext_maps[size][firstpos+64]<<6));
	PA_16bitDrawTileTransp(screen, xscreen, yscreen, x, y+8, 8, ly-8, data, color);	
	firstpos ++;
	data = (u8*)(bit8_tiles[size]+(bittext_maps[size][firstpos]<<6));
	PA_16bitDrawTileTransp(screen, xscreen, yscreen, x+8, y, lx-8, 8, data, color);
	data = (u8*)(bit8_tiles[size]+(bittext_maps[size][firstpos+64]<<6));
	PA_16bitDrawTileTransp(screen, xscreen, yscreen, x+8, y+8, lx-8, ly-8, data, color);			
}
else {
	data = (u8*)(bit8_tiles[size]+(bittext_maps[size][(u8)letter]<<6));
	PA_16bitDrawTileTransp(screen, xscreen, yscreen, x, y, lx, ly, data, color);
}

}

void PA_16bitTextInit(void){
	//pa_bittextpoliceheight[0] = 6;
	//pa_bittextpoliceheight[1] = 8;
	//pa_bittextpoliceheight[2] = 9;
	//pa_bittextpoliceheight[3] = 12;
	//pa_bittextpoliceheight[4] = 14;
}


s16 PA_SmartText16bBuf(u16 *screen, u16 xscreen, u16 yscreen, s16 basex, s16 basey, s16 maxx, s16 maxy, char* text, u16 color, u8 size, u8 transp, s32 limit){

PA_16bitTextInit();


s16 i, j;
s16 x, y;
s16 lx, ly;
s16 letter; 

ly = pa_bittextpoliceheight[size];

x = basex;
y = basey;

s16 length = 0;
s16 ylimiy = maxy - ly;
s16 wordx = 0; // Longueur de mot
s16 wordletter = 0;

for (i = 0; (text[i] && y <= ylimiy && i < limit); i++) {
	if (text[i] == '\n'){
		if (transp == 0){
			while(x < maxx) { // On efface tout ce qui suit
				for (j = 0; j < ly; j++) PA_Put16bitPixelBuffer(screen, xscreen, yscreen, x, y + j, 0);
				x++;
			}
		}
		if (text[i+1] == ' ') i++; // On vire s'il y a un espace	
		x = basex;
		y += ly;	
	}
	else{
		wordletter = 1;
		wordx = 0;
		
		while(!((text[i+wordletter] <= 32) || (i + wordletter >= limit))) { // >= 32, donc si 0, '\n', on ' ' :)
			letter = text[i+wordletter];
			lx = pa_bittextdefaultsize[size][letter];
			wordx += lx;
			wordletter++;
		}
		
		if (x + wordx >= maxx ) {  // On dépasse en X...
//		if (x + lx >= maxx ) {  // On dépasse en X...
			if (transp == 0){
				while(x < maxx) { // On efface tout ce qui suit
					for (j = 0; j < ly; j++) PA_Put16bitPixelBuffer(screen, xscreen, yscreen, x, y + j, 0);
					x++;
				}
			}
		
			x = basex;
			y += ly;
		
			if(text[i] != ' ') { // On vire s'il y a un espace	
				if(y <= ylimiy) { // Si on n'a pas dépassé...
					for (j = i; j < (i + wordletter); j++) {
						letter = text[j];
						//if (letter > 128) letter -= 96; // pour les accents...
						lx = pa_bittextdefaultsize[size][letter];
						letters16b[transp](size, screen, xscreen, yscreen, x, y, letter, color);				
						x += lx;
					}
					i+=wordletter-1;
				} // Sinon ca va s'arreter
				else i--; // On n'a pas affiché la derniere lettre...
			}
		}		
		else{ // Si pas de passage à la ligne auto
			s32 jmax = (i + wordletter);
			if (text[(i + wordletter-1)] < 32) jmax--; // On ne dessinera pas ce caractère
			
			for (j = i; j < jmax; j++) {
				letter = text[j];
				lx = pa_bittextdefaultsize[size][letter];
//				if (letter >= 0) {
					letters16b[transp](size, screen, xscreen, yscreen, x, y, letter, color);				
					x += lx;
//				}
			}
			i+=wordletter-1;
		}

	}
}

length = i;
if (transp == 0){ // Si pas de transparence, on efface tout le reste
	while(x < maxx) { // On efface tout ce qui suit
		for (i = 0; i < ly+5; i++) PA_Put16bitPixelBuffer(screen, xscreen, yscreen, x, y + i, 0);
		x++;
	}
	
	y += ly;
	basey = y;
	
	if (basex&1) {
		while(y < maxy) {
			PA_Put16bitPixelBuffer(screen, xscreen, yscreen, basex, y, 0);
			y++;
		}
		++basex;
	}
	
//	for (x = basex; x < maxx; x++)
//		for (y = basey; y < maxy; y++) PA_DrawBg[screen][(x >> 1) + (y << 7)] = 0;
}


return length;

}





s16 PA_CenterSmartText16bBuf(u16 *screen, u16 xscreen, u16 yscreen, s16 basex, s16 basey, s16 maxx, s16 maxy, char* text, u16 color, u8 size, u8 transp){
PA_16bitTextInit();
s16 i;
s16 x, y;
s16 lx, ly;
u8 letter; 

ly = pa_bittextpoliceheight[size];

x = basex;
y = basey;

s16 length = 0;


s16 nlines = 0; // Nombre de lignes
s16 xsize[30]; // Taille en largeur de chaque ligne

xsize[0] = 0;

for (i = 0; text[i]; i++){ // Calcul du centrage
	if (text[i] == '\n'){
		nlines++;
		xsize[nlines] = 0; // Rien, par défaut
	}
	else{
		letter = text[i];
		//if (letter > 128) letter -= 96; // pour les accents...
		xsize[nlines] += pa_bittextdefaultsize[size][letter];
	}
}


++nlines; // Si 0, ca fait 1 ligne
y = basey + (((maxy - basey) - (nlines*ly)) >> 1);
nlines = 0;
x = basex + (((maxx-basex) - xsize[nlines]) >> 1);
for (i = 0; text[i]; i++) {
	if (text[i] == '\n'){
		++nlines;
		x = basex + (((maxx-basex) - xsize[nlines]) >> 1);
		y += ly;	
	}
	else{
		letter = text[i];
		lx = pa_bittextdefaultsize[size][letter];
		letters16b[transp](size, screen, xscreen, yscreen, x, y, letter, color);				
		x += lx;
	}

}

length = i;

return length;

}

void pa_16bitTextDecompressBuf(u8 size){
	s16 i;
	if (B16b_actual_size == size)
		return; // If you already did it... don't do it again!!!!
	B16b_actual_size = size;
	if((size < 5)&&(((void*)bittext_tiles_blank[size]) == ((void*)bit8_tiles[size]))){ // Using default font and still compressed
		u32 tilesize = pa_bittextsizes[size]<<3;
		bit8_tiles[size] = (u8*)malloc(tilesize);
		if (bit8_tiles[size] == NULL)
			return;
		for(i = 0; i < tilesize; i++){
		   bit8_tiles[size][i] = (bittext_tiles_blank[size][i>>3]>>(i&7))&1;
		}   				
	}
}

s16 PA_SmartText16bBuf_DS(u16 *screen, s16 basex, s16 basey, s16 maxx, s16 maxy, char* text, u16 color, u8 size, u8 transp, s32 limit){
   pa_16bitTextDecompressBuf(size);
   return PA_SmartText16bBuf(screen,600,240,basex,basey,maxx,maxy,text,color,size,transp,limit);
}   

s16 PA_CenterSmartText16bBuf_DS(u16 *screen, s16 basex, s16 basey, s16 maxx, s16 maxy, char* text, u16 color, u8 size, u8 transp){
   pa_16bitTextDecompressBuf(size);
   return PA_CenterSmartText16bBuf(screen,600,240,basex,basey,maxx,maxy,text,color,size,transp);
}  

s16 PA_CenterSmartText16bBuf_RAW(u16 *screen, s16 tamx, s16 tamy, s16 basex, s16 basey, s16 maxx, s16 maxy, char* text, u16 color, u8 size, u8 transp){
   pa_16bitTextDecompressBuf(size);
   return PA_CenterSmartText16bBuf(screen,tamx,tamy,basex,basey,maxx,maxy,text,color,size,transp);
} 
