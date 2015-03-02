/*
 *  Uzebox keyboard demo
 *  Copyright (C) 2015  Alec Bourque
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
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

#include "data/font-8x8-full.inc"

#include "data/scancodes.inc"

extern volatile unsigned int joypad1_status_lo,joypad2_status_lo;
extern volatile unsigned int joypad1_status_hi,joypad2_status_hi;
#define Wait200ns() asm volatile("lpm\n\tlpm\n\t");
#define Wait100ns() asm volatile("lpm\n\t");

#define KB_SEND_KEY 0x00
#define KB_SEND_END 0x01
#define KB_SEND_DEVICE_ID 0x02
#define KB_SEND_FIRMWARE_REV 0x03
#define KB_RESET 0x7f

u8 GetKey(u8 code);
void ReadButtons2();
void BeginTrans();
void decode(unsigned char sc);
void debug_scroll();
void debug_char2(unsigned char c);
extern u8 _x,_y;


u8 cnt=0;
bool readKeys=true;

void VsyncHandler(){

	if(readKeys){
		ReadButtons2();		//read the snes controllers

		u8 key=GetKey(KB_SEND_END);
		if(key!=0){			
			decode(key);
		}
	}

}

int main(){

	SetUserPreVsyncCallback(&VsyncHandler);

	//Set the font and tiles to use.
	//Always invoke before any ClearVram()
	SetTileTable(font);

	//Clear the screen (fills the vram with tile zero)
	ClearVram();

	debug_str_p(PSTR("   ***Keyboard Demo***\r\n\r\n"));

	u8 curDelay=0,curState=0;
	while(1){
		WaitVsync(1);
		//animate cusrsor
		curDelay++;
		if(curDelay==20){
			curState^=1;
			if(curState){
				PrintChar(_x,_y,'_');
			}else{
				PrintChar(_x,_y,' ');
			}

			curDelay=0;
		}
	}

} 

u8 GetKey(u8 command){
	static u8 state=0;
	u8 data=0;

	unsigned char i;

	if(state==0){

		//ready to transmit condition 
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
		JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);
		Wait200ns();
		Wait200ns();
		Wait200ns();
		Wait200ns();
		Wait200ns();
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));
		JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);
		Wait200ns();
		Wait200ns();
		Wait200ns();
		Wait200ns();
		Wait200ns();

		if(command==KB_SEND_END){
			state=0;
		}else{
			state=1;
		}		
	}

	//read button states
	for(i=0;i<8;i++){

		data<<=1;
	
		//data out		
		if(command&0x80){
			JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);		
		}else{
			JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));
		}

		//pulse clock pin		
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
		
		command<<=1;
		Wait100ns();
				
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))!=0) data|=1;

		JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);		
	
	}

	JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));	
	
	return data;

}


void ReadButtons2(){
	unsigned int p1ButtonsLo=0,p2ButtonsLo=0;
	unsigned char i;

	//latch controllers
	JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);
	Wait200ns();
	Wait200ns();
	JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));


	//read button states
	for(i=0;i<16;i++){

		p1ButtonsLo>>=1;
		p2ButtonsLo>>=1;

		Wait200ns();
		Wait200ns();
			
		//pulse clock pin		
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
		
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN))==0) p1ButtonsLo|=(1<<15);
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))==0) p2ButtonsLo|=(1<<15);
		
		JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);
		
		Wait200ns();
		Wait200ns();
	}

	joypad1_status_lo=p1ButtonsLo;
	joypad2_status_lo=p2ButtonsLo;

}


unsigned char is_up=0, shift = 0, mode = 0;
void decode(unsigned char sc)
{
	//enter=0x5a

	unsigned char i,c;
	if (!is_up)// Last data received was the up-key identifier
	{
		switch (sc)
		{
			case 0xF0 :// The up-key identifier
				is_up = 1;
				break;
			case 0x12 :// Left SHIFT
				shift = 1;
				break;

			case 0x59 :// Right SHIFT
				shift = 1;
				break;
			case 0x05 :// F1
				if(mode == 0)
				mode = 1;// Enter scan code mode
				if(mode == 2)
				mode = 3;// Leave scan code mode
				break;
			default:
				if(mode == 0 || mode == 3)// If ASCII mode
				{
					if(!shift)// If shift not pressed,
					{ // do a table look-up
						for(i = 0; pgm_read_byte(&(unshifted[i][0]))!=sc && pgm_read_byte(&(unshifted[i][0])); i++);
						if (pgm_read_byte(&(unshifted[i][0])) == sc) {
							c=pgm_read_byte(&(unshifted[i][1]));
							debug_char2(c);
							//if(sc==0x5a)debug_hex(c);
						}
					} else {// If shift pressed
						for(i = 0; pgm_read_byte(&(shifted[i][0]))!=sc && pgm_read_byte(&(shifted[i][0])); i++);
						if (pgm_read_byte(&(shifted[i][0])) == sc) {
							c=pgm_read_byte(&(shifted[i][1]));
							debug_char2(c);
							//if(sc==0x5a)debug_hex(c);
						}
					}
				} else{ // Scan code mode
					debug_hex(sc);// Print scan code
				}
				break;
		}
	} else {
		is_up = 0;// Two 0xF0 in a row not allowed
		switch (sc)
		{
			case 0x12 :// Left SHIFT
				shift = 0;
				break;
			case 0x59 :// Right SHIFT
				shift = 0;
				break;
			case 0x05 :// F1
				if(mode == 1)
				mode = 2;
				if(mode == 3)
				mode = 0;
				break;
			case 0x06 :// F2
//				clr();
				break;
		}
	}
}



void debug_char2(unsigned char c){


	if(c==0x0d){ //enter
		PrintChar(_x,_y,' ');//clear cursor
		_x=2;
		_y++;	
	}else if(c==0x08){ //backspace
		PrintChar(_x,_y,' ');//clear cursor

		if(_x==0){
			_x=SCREEN_TILES_H-1;
			_y--;
		}else{
			_x--;
		}		
		PrintChar(_x,_y,' ');

	}else if(c<32 || c>'z'){
		PrintChar(_x,_y,'<');
		PrintHexByte(_x+1,_y,c);
		PrintChar(_x+3,_y,'>');
		_x+=4;
	}else{
		PrintChar(_x++,_y,c);		
	}
	if(_x>=(SCREEN_TILES_H-1)){
		_x=2;
		_y++;
	}

	debug_scroll();

}
