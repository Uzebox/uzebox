/*
 *  Uzebox Kernel
 *  Copyright (C) 2008  Alec Bourque
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "uzebox.h"
#include <avr/interrupt.h>

#define CHAR_ZERO 16 

#if VIDEO_MODE == 3
	extern unsigned char ram_tiles[];
	struct SpriteStruct sprites[MAX_SPRITES];
	extern unsigned char *sprites_tiletable_lo;
	extern unsigned char *tile_table_lo;
	extern struct BgRestoreStruct ram_tiles_restore[];

	extern void CopyTileToRam(unsigned char romTile,unsigned char ramTile);
	extern void BlitSprite(unsigned char spriteNo,unsigned char ramTileNo,unsigned int xy,unsigned int dxdy);

	unsigned char free_tile_index;
	bool spritesOn=true;

	void RestoreBackground(){
		unsigned char i;
		for(i=0;i<free_tile_index;i++){			
			vram[ram_tiles_restore[i].addr]=ram_tiles_restore[i].tileIndex;
		}	
	}


	void SetSpriteVisibility(bool visible){
		spritesOn=visible;
	}


	void ProcessSprites(){
		unsigned char i,bx,by,dx,dy,bt,x,y,tx=1,ty=1;
		unsigned int ramPtr;


		free_tile_index=0;


		if(!spritesOn) return;
		
		for(i=0;i<MAX_SPRITES;i++){
			by=sprites[i].y;
			if(by<(SCREEN_TILES_V*TILE_HEIGHT)){
				tx=1;
				ty=1;
				//get the BG tiles that are overlapped by the sprite
				bx=sprites[i].x>>3;
				dx=sprites[i].x&0x7;
				if(dx>0) tx++;

				by=sprites[i].y>>3;			
				dy=sprites[i].y&0x7;		
				if(dy>0) ty++;			

				for(y=0;y<ty;y++){

					for(x=0;x<tx;x++){

						ramPtr=((by+y)*VRAM_TILES_H)+bx+x;
						bt=vram[ramPtr];

						if( (bt>=RAM_TILES_COUNT)  && (free_tile_index < RAM_TILES_COUNT) ){

							//tile is mapped to flash. Copy it to next free RAM tile.
							//if no ram free ignore tile
							ram_tiles_restore[free_tile_index].addr=ramPtr;
							ram_tiles_restore[free_tile_index].tileIndex=bt;
														
							CopyTileToRam(bt,free_tile_index);

							vram[ramPtr]=free_tile_index;
							bt=free_tile_index;
							free_tile_index++;										
						}
					
						if(bt<RAM_TILES_COUNT){

							BlitSprite(i,bt,(y<<8)+x,(dy<<8)+dx);

							/*	THIS COMMENTED CODE HAS BEEN MOVED TO ASM				
							//blit sprite to RAM tile
							src=sprites_tiletable_lo+(sprites[i].tileIndex*64);
							dest=ram_tiles+(bt*TILE_HEIGHT*TILE_WIDTH);

							if(x==0){
								dest+=dx;
								xdiff=dx;
							}else{
								src+=(8-dx);
								xdiff=(8-dx);
							}

							if(y==0){
								dest+=(dy*TILE_WIDTH);
								ydiff=dy;
							}else{
								src+=((8-dy)*TILE_WIDTH);
								ydiff=(8-dy);
							}

							for(y2=ydiff;y2<TILE_HEIGHT;y2++){
								for(x2=xdiff;x2<TILE_WIDTH;x2++){
													
									px=pgm_read_byte(src++);
									if(px!=TRANSLUCENT_COLOR){
										*dest=px;
									}
									dest++;

								}		
								src+=xdiff;
								dest+=xdiff;

							}
							*/					
						}


					}//end for X
				}//end for Y
		
			}//	if(by<(SCREEN_TILES_V*TILE_HEIGHT))		
		}


		//restore BG tiles
		RestoreBackground();

	}

#endif


#if VIDEO_MODE == 2
	#include "data/scrolltable.inc"
	extern char scanline_sprite_buf[];
	extern char sprites_per_lines[SCREEN_TILES_V*TILE_HEIGHT][MAX_SPRITES_PER_LINE];
	struct SpriteStruct sprites[MAX_SPRITES];

	/**
	 *  =====USED INTERNALLY ONLY - USE SCREEN SECTIONS XSCROLL & YSCROLL===
	 *
	 * Scroll the window to the specified position within the 
	 * 32x32 tiles VRAM. Note that due to the non-binary aligment of the 
	 * tiles width(6), the caller needs to perform X wrapping
	 * before calling this function. Wrapping is at X_SCROLL_WRAP 
	 * (32 horizontal VRAM tiles * 6 horizontal pixel per tiles = 192)
	 * Y Scrolling doesnt need this because it wraps automatically at 0xff.
	 * Try to execute this call a close as possible to WaitVsync() call
	 * to minimize chances that a rendering interrupt occurs between the setting
	 * of scroll_x and scroll_y.
	 *
	 * I.e:
	 *
	 * sx++;
	 * if(sx>=X_SCROLL_WRAP) sx=0;
	 *
	 * or
	 *
	 * sx--;
	 * if(sx==0xff) sx=(X_SCROLL_WRAP-1);
	 */
	 
	 void SetScrolling(unsigned char section,unsigned char sx,unsigned char sy){
		unsigned char Xcoarse,Xfine;

		//prevent srolling overflow
		if(sx>=0 && sx<X_SCROLL_WRAP){
			Xcoarse=pgm_read_byte(&(scrolltable[sx][0]));
			Xfine=pgm_read_byte(&(scrolltable[sx][1]));
		
			screenSections[section].scrollXcoarse=Xcoarse;
			screenSections[section].scrollXfine=Xfine;
		}
		
		screenSections[section].vramWrapAdress=screenSections[section].vramBaseAdress+screenSections[section].scrollXcoarse;
		screenSections[section].vramRenderAdress=screenSections[section].vramWrapAdress+((sy/8)*VRAM_TILES_H);

	}


#endif





//Draws a map of tile at the specified position

void DrawMap2(unsigned char x,unsigned char y,const char *map){
	unsigned char i;
	unsigned char mapWidth=pgm_read_byte(&(map[0]));
	unsigned char mapHeight=pgm_read_byte(&(map[1]));

	for(unsigned char dy=0;dy<mapHeight;dy++){
		for(unsigned char dx=0;dx<mapWidth;dx++){
			
			i=pgm_read_byte(&(map[(dy*mapWidth)+dx+2]));
			
			vram[((y+dy)*VRAM_TILES_H)+x+dx]=(i + RAM_TILES_COUNT) ;
			
		
		}
	}

}



//Draws a map of tile at the specified position

void DrawMap(unsigned char x,unsigned char y,const int *map){
	int i;
	int mapWidth=pgm_read_word(&(map[0]));
	int mapHeight=pgm_read_word(&(map[1]));

	for(unsigned char dy=0;dy<mapHeight;dy++){
		for(unsigned char dx=0;dx<mapWidth;dx++){
			
			i=pgm_read_word(&(map[(dy*mapWidth)+dx+2]));
			
			SetTile(x+dx,y+dy,i);	
		}
	}

}

//Print an unsigned long in decimal
void PrintLong(int x,int y, unsigned long val){
	unsigned char c,i;

	for(i=0;i<6;i++){
		c=val%10;
		if(val>0 || i==0){
			SetFont(x--,y,c+CHAR_ZERO+RAM_TILES_COUNT);
		}else{
			SetFont(x--,y,0+RAM_TILES_COUNT);
		}
		val=val/10;
	}
		
}

//Print an unsigned byte in decimal
void PrintByte(int x,int y, unsigned char val,bool zeropad){
	unsigned char c,i;

	for(i=0;i<3;i++){
		c=val%10;
		if(val>0 || i==0){
			SetFont(x--,y,c+CHAR_ZERO+RAM_TILES_COUNT);
		}else{
			if(zeropad){
				SetFont(x--,y,CHAR_ZERO+RAM_TILES_COUNT);
			}else{
				SetFont(x--,y,0+RAM_TILES_COUNT);
			}
		}
		val=val/10;
	}
		
}

//Print an unsigned byte in decimal
void PrintInt(int x,int y, unsigned int val,bool zeropad){
	unsigned char c,i;

	for(i=0;i<5;i++){
		c=val%10;
		if(val>0 || i==0){
			SetFont(x--,y,c+CHAR_ZERO+RAM_TILES_COUNT);
		}else{
			if(zeropad){
				SetFont(x--,y,CHAR_ZERO+RAM_TILES_COUNT);
			}else{
				SetFont(x--,y,0+RAM_TILES_COUNT);
			}
		}
		val=val/10;
	}
		
}

//Print a byte in binary format
void PrintBinaryByte(char x,char y,unsigned char byte){
	int i;
	
	for(i=0;i<8;i++){
		
		if(byte&0x80){
			SetFont(x+i,y,CHAR_ZERO+1+RAM_TILES_COUNT);
		}else{
			SetFont(x+i,y,CHAR_ZERO+RAM_TILES_COUNT);
		}
		byte<<=1;
	}
}

//Print a byte in hexadecimal
void PrintHexByte(char x,char y,unsigned char byte){
	unsigned char nibble;

	//hi nibble	
	nibble=(byte>>4);
	if(nibble<=9){
		SetFont(x,y,nibble+CHAR_ZERO+ RAM_TILES_COUNT);
	}else{
		SetFont(x,y,nibble+CHAR_ZERO+7+ RAM_TILES_COUNT);
	}

	//lo nibble	
	nibble=(byte&0xf);
	if(nibble<=9){		
		SetFont(x+1,y,nibble+CHAR_ZERO+ RAM_TILES_COUNT);
	}else{
		SetFont(x+1,y,nibble+CHAR_ZERO+7+ RAM_TILES_COUNT);
	}

}

//Print a hexdecimal integer
void PrintHexInt(char x,char y,int value){
	PrintHexByte(x,y, (unsigned int)value>>8);
	PrintHexByte(x+2,y,value&0xff);
}

//Print a string
void Print(int x,int y,const char *string){


	int i=0;
	char c;

	while(1){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			c=((c&127)-32) + RAM_TILES_COUNT;			
			SetFont(x++,y,c);
		}else{
			break;
		}
	}
	
}

//Print a single character
void PrintChar(int x,int y,char c){

	SetFont(x,y,(c-32)+RAM_TILES_COUNT);
	
}

//fill a region with the specified tile from the tile table
void Fill(int x,int y,int width,int height,int tile){
	int cx,cy;
	
	for(cy=0;cy<height;cy++){
		for(cx=0;cx<width;cx++){		
			SetTile(x+cx,y+cy,tile+RAM_TILES_COUNT);
		}
	}
}

//fill a region with the specified tile from the font table
void FontFill(int x,int y,int width,int height,int tile){
	int cx,cy;
	
	for(cy=0;cy<height;cy++){
		for(cx=0;cx<width;cx++){		
			SetFont(x+cx,y+cy,(tile-32)+RAM_TILES_COUNT);
		}
	}
}

//Wait for the beginning of next frame (30hz)
void WaitVsync(int count){
	int i;
	//ClearVsyncFlag();
	for(i=0;i<count;i++){
		while(!GetVsyncFlag());
		ClearVsyncFlag();		
	}
}

#if SPRITES_ENABLED == 1

	void MapSprite(unsigned char startSprite,const char *map){
		unsigned char tile;
		unsigned char mapWidth=pgm_read_byte(&(map[0]));
		unsigned char mapHeight=pgm_read_byte(&(map[1]));

		for(unsigned char dy=0;dy<mapHeight;dy++){
			for(unsigned char dx=0;dx<mapWidth;dx++){
			
			 	tile=pgm_read_byte(&(map[(dy*mapWidth)+dx+2]));		
				sprites[startSprite++].tileIndex=tile ;
			}
		}

	}

	void MoveSprite(unsigned char startSprite,unsigned char x,unsigned char y,unsigned char width,unsigned char height){
	
		for(unsigned char dy=0;dy<height;dy++){
			for(unsigned char dx=0;dx<width;dx++){
				
				sprites[startSprite].x=x+(8*dx);
				sprites[startSprite].y=y+(8*dy);
				startSprite++;
			}
		}	

	}
#endif

//Fade table created by tim1724 
#define FADER_STEPS 12
unsigned char fader[FADER_STEPS]={
           // BB GGG RRR
    0x00,  // 00 000 000
    0x40,  // 01 000 000
    0x88,  // 10 001 000
    0x91,  // 10 010 001
    0xD2,  // 11 010 010
    0xE4,  // 11 100 100
    0xAD,  // 10 101 101
    0xB5,  // 10 110 101
    0xB6,  // 10 110 110
    0xBE,  // 10 111 110
    0xBF,  // 10 111 111
    0xFF,  // 11 111 111
};


unsigned char fadeStep,fadeSpeed,currFadeFrame;
char fadeDir;
bool volatile fadeActive;


void doFade(unsigned char speed,bool blocking){
	fadeSpeed=speed;
	currFadeFrame=0;
	fadeActive=true;
		
	if(blocking){
		while(fadeActive==true);
	}
	
	
}

void FadeIn(unsigned char speed,bool blocking){
	if(speed==0){
		DDRC=0xff;
		return;
	}
	fadeStep=1;
	fadeDir=1;
	doFade(speed,blocking);
}

void FadeOut(unsigned char speed,bool blocking){
	if(speed==0){
		DDRC=0;
		return;
	}
	
	fadeStep=FADER_STEPS;
	fadeDir=-1;
	doFade(speed,blocking);
}


//called by the kernel at each field end
void ProcessFading(){
	if(fadeActive==true){
		if(currFadeFrame==0){
			currFadeFrame=fadeSpeed;
			DDRC = fader[fadeStep-1];
			fadeStep+=fadeDir;
			if(fadeStep==0 || fadeStep==(FADER_STEPS+1)){
				fadeActive=false;
			}
		}else{
			currFadeFrame--;
		}			
	}
}

