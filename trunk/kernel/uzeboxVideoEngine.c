/*
 *  Uzebox Kernel
 *  Copyright (C) 2008-2009 Alec Bourque
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

//inlude C functions required by the current video mode.
#ifdef VMODE_C_SOURCE
	#include VMODE_C_SOURCE
#endif 

#ifndef CHAR_ZERO
	#define CHAR_ZERO 16 
#endif



//Draws a map of tile at the specified position
#if VRAM_ADDR_SIZE == 1
	void DrawMap(unsigned char x,unsigned char y,const VRAM_PTR_TYPE *map) {
		//unsigned char i;
		u8 mapWidth=pgm_read_byte(&(map[0]));
		u8 mapHeight=pgm_read_byte(&(map[1]));
		
		for(u8 dy=0;dy<mapHeight;dy++){
			for(u8 dx=0;dx<mapWidth;dx++){			
				SetTile(x+dx,y+dy,pgm_read_byte(&(map[(dy*mapWidth)+dx+2])));					
			}
		}

	}

	void DrawMap2(unsigned char x,unsigned char y,const char *map) __attribute__((alias("DrawMap"))) __attribute__ ((deprecated));
#else

	//Draws a map of tile at the specified position

	void DrawMap(unsigned char x,unsigned char y,const VRAM_PTR_TYPE *map){
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
#endif


//Print an unsigned long in decimal
void PrintLong(int x,int y, unsigned long val){
	unsigned char c,i;

	for(i=0;i<10;i++){
		c=val%10;
		if(val>0 || i==0){
			SetFont(x--,y,c+CHAR_ZERO);
		}else{
			SetFont(x--,y,0);
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
			SetFont(x--,y,c+CHAR_ZERO);
		}else{
			if(zeropad){
				SetFont(x--,y,CHAR_ZERO);
			}else{
				SetFont(x--,y,0);
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
			SetFont(x--,y,c+CHAR_ZERO);
		}else{
			if(zeropad){
				SetFont(x--,y,CHAR_ZERO);
			}else{
				SetFont(x--,y,0);
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
			SetFont(x+i,y,CHAR_ZERO+1);
		}else{
			SetFont(x+i,y,CHAR_ZERO);
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
		SetFont(x,y,nibble+CHAR_ZERO);
	}else{
		SetFont(x,y,nibble+CHAR_ZERO+7);
	}

	//lo nibble	
	nibble=(byte&0xf);
	if(nibble<=9){		
		SetFont(x+1,y,nibble+CHAR_ZERO);
	}else{
		SetFont(x+1,y,nibble+CHAR_ZERO+7);
	}

}

//Print a hexdecimal integer
void PrintHexInt(char x,char y,int value){
	PrintHexByte(x,y, (unsigned int)value>>8);
	PrintHexByte(x+2,y,value&0xff);
}

//Print a hexdecimal integer
void PrintHexLong(char x,char y,uint32_t value){
	PrintHexByte(x,y, value>>24);	
	PrintHexByte(x+2,y, value>>16);
	PrintHexByte(x+4,y, value>>8);
	PrintHexByte(x+6,y,value&0xff);
}

//Print a string from flash
void Print(int x,int y,const char *string){


	int i=0;
	char c;

	while(1){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			c=((c&127)-32);			
			SetFont(x++,y,c);
		}else{
			break;
		}
	}
	
}

//Print a string from RAM
void PrintRam(int x,int y,unsigned char *string){

	int i=0;
	char c;

	while(1){
		c=string[i++];		
		if(c!=0){
			c=((c&127)-32);			
			SetFont(x++,y,c);
		}else{
			break;
		}
	}
	
}

//Print a single character
void PrintChar(int x,int y,char c){

	SetFont(x,y,(c-32));
	
}

//fill a region with the specified tile from the tile table
void Fill(int x,int y,int width,int height,int tile){
	int cx,cy;
	
	for(cy=0;cy<height;cy++){
		for(cx=0;cx<width;cx++){		
			SetTile(x+cx,y+cy,tile);
		}
	}
}

//fill a region with the specified tile from the font table
void FontFill(int x,int y,int width,int height,int tile){
	int cx,cy;
	
	for(cy=0;cy<height;cy++){
		for(cx=0;cx<width;cx++){		
			SetFont(x+cx,y+cy,(tile-32));
		}
	}
}

//Wait for the beginning of next frame (60hz)
void WaitVsync(int count){
	int i;
	//ClearVsyncFlag();
	for(i=0;i<count;i++){
		while(!GetVsyncFlag());
		ClearVsyncFlag();		
	}
}


//Fade table created by tim1724 
#define FADER_STEPS 12
const unsigned char fader[FADER_STEPS] PROGMEM={
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
			DDRC = pgm_read_byte(&(fader[fadeStep-1]));
			fadeStep+=fadeDir;
			if(fadeStep==0 || fadeStep==(FADER_STEPS+1)){
				fadeActive=false;
			}
		}else{
			currFadeFrame--;
		}			
	}
}


