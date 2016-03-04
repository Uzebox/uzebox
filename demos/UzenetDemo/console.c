/*
 * console.c
 *
 *  Created on: Oct 13, 2015
 *      Author: admin
 */
#include <stdbool.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <time.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "keyboard.h"
#include "console.h"
#include "uzenet.h"

#define _CONS_X_ORIGIN 2
#define _CONS_Y_ORIGIN 1
#define _CONS_CUR_DEF_SPEED 30
#define _CONS_CUR_TILE 124

u8 cx=_CONS_X_ORIGIN,cy=_CONS_Y_ORIGIN;
u8 tickCounter=0;
u16 _cons_flags=0, _clock_correction=0;
bool readKeys=true;
char linebuf[SCREEN_TILES_H+1];

int _cons_putchar_printf(char var, FILE *stream);
void _cons_scroll();
void _cons_char(char c);

static FILE mystdout = FDEV_SETUP_STREAM(_cons_putchar_printf, NULL, _FDEV_SETUP_WRITE);

char GetFont(int x,int y){
	return GetTile(x,y)+32;
}

void console_init(u16 flags){
	 // setup our stdio stream
	stdout = &mystdout;
	_cons_flags=flags;
}

// this function is called by printf as a stream handler
int _cons_putchar_printf(char var, FILE *stream) {
    // translate \n to \r for br@y++ terminal
	_cons_char(var);
    return 0;
}

//fills the line buffer with the selected line, removing leading and trailing spaces
char* GetLine(u8 line){
	u8 dest=0,c=0,xstart=0,xend=SCREEN_TILES_H;
	//skip leading spaces
	while(1){
		if(GetFont(xstart,cy)!=32)break;

		//check if empty line
		if(xstart==SCREEN_TILES_H){
			linebuf[0]=0;
			return linebuf;
		}

		xstart++;
	}

	//skip trailing spaces
	while(1){
		if(GetFont(xend,cy)!=32)break;
		xend--;
	}


	for(u8 x=xstart;x<=xend;x++){
		c=GetFont(x,cy);
		linebuf[dest++]=c;
	}
	linebuf[dest]=0;
	return linebuf;
}


//executes each vsync
void ConsoleHandler(){

	//clock tick
	if(tickCounter < 60){
		tickCounter++;
	}else{
		system_tick();
		tickCounter=0;
	}
	//NTSC field rate is 59.95hz
	//So we need to add 1 tick after each 1000 fields
	//to compensate
	_clock_correction++;
	//3000 too fast
	//3500 too fast
	///4000 too fast

	//4250 too slow
	//5000 too slow
	if(_clock_correction==4125){
		_clock_correction=0;
		system_tick();
	}


	if(readKeys){

		u8 key=GetKey(KB_SEND_END);
		if(key!=0){

			u16 c=decode(key);
			if(c!=0){

				switch(c){
					case 0x175: //up
						if(cy>_CONS_Y_ORIGIN)cy--;
						break;
					case 0x172: //down
						cy++;
						_cons_scroll();
						break;
					case 0x16b: //left
						if(cx>_CONS_X_ORIGIN)cx--;
						break;
					case 0x174: //Right
						if(cx<(SCREEN_TILES_H-1))cx++;
						break;

					case 13: //ENTER
						//send line to UART

						//check if command
						if(strcmp(GetLine(cy),"cls")==0){
							 console_clear();
						}else if(strcmp(GetLine(cy),"echo on")==0){
							wifi_Echo(true);
							printf_P(PSTR("\r\nOk\r\n"));
						}else if(strcmp(GetLine(cy),"echo off")==0){
							wifi_Echo(false);
							printf_P(PSTR("\r\nOk\r\n"));
						}else if(strcmp(GetLine(cy),"help")==0){
							printf_P(PSTR("\ncls : clear screen.\nset uart=<57600,115200> : Set Atmega644 UART speed.\necho <on,off>: turns off UART echo on or off.\n"));

						}else if(strcmp(GetLine(cy),"set uart=57600")==0){
							UBRR0L=60;		//57600 @ 2x
							printf_P(PSTR("\r\nOk\r\n"));
						}else if(strcmp(GetLine(cy),"set uart=115200")==0){
							UBRR0L=30;		//115200 @ 2x
							printf_P(PSTR("\r\nOk\r\n"));
						}else{
							for(u8 i=_CONS_X_ORIGIN;i<cx;i++){
								u8 tx=GetFont(i,cy);
								while(UartSendChar(tx)==-1); //block if buffer full
							}

							UartSendChar('\r');
							UartSendChar('\n');
						}
						cx=_CONS_X_ORIGIN;
						cy++;
						break;

					case 8: //back space
						if(cx>_CONS_X_ORIGIN){
							cx--;
							PrintChar(cx,cy,32);
						}
						break;

					default:
						_cons_char(c);
						break;
				}


			}

		}
	}

	MoveCursor(cx,cy);
}

void console_setCursor(u8 x,u8 y){
	cx=x;
	cy=y;
}

void console_clear(){
	ClearVram();
	cx=_CONS_X_ORIGIN;
	cy=_CONS_Y_ORIGIN;
}

void _cons_char(char c){

	if(c==0x0d){ // \r
		cx=_CONS_X_ORIGIN;
	}else if(c==0x0a){ // \n
		cx=_CONS_X_ORIGIN;
		cy++;
	}else if( c<32 || c>'z'){
		if(_cons_flags & CONS_OUTPUT_NON_ALPHA){
			PrintChar(cx,cy,'<');
			PrintHexByte(cx+1,cy,c);
			PrintChar(cx+3,cy,'>');
			cx+=4;
		}
	}else{
		PrintChar(cx++,cy,c);
	}

	if(cx>=(SCREEN_TILES_H-1)){
		cx=_CONS_X_ORIGIN;
		cy++;
	}

	_cons_scroll();

}
void _cons_scroll(){
	if(cy>(SCREEN_TILES_V-2)){
		//scroll all lines up

		for(u16 i=0;i<(VRAM_TILES_H*(SCREEN_TILES_V-2));i++){
			vram[i]=vram[(i+VRAM_TILES_H)];
		}

		//clear last line
		for(u8 i=0;i<VRAM_TILES_H;i++){
			SetFont(i,SCREEN_TILES_V-2,0);
			//vram[(VRAM_TILES_H*(SCREEN_TILES_V-2))+i]=32;
		}

		cy=SCREEN_TILES_V-2;
		cx=_CONS_X_ORIGIN;
	}
}
