/*
 * keyboard.c
 *
 *  Created on: Oct 13, 2015
 *      Author: admin
 */

#include <stdbool.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "keyboard.h"

#define Wait200ns() asm volatile("lpm\n\tlpm\n\t");
#define Wait100ns() asm volatile("lpm\n\t");


// Unshifted characters
const unsigned char unshifted[][2] PROGMEM = {
	{0x0d,9},
	{0x0e,'`'},
	{0x15,'q'},
	{0x16,'1'},
	{0x1a,'z'},
	{0x1b,'s'},
	{0x1c,'a'},
	{0x1d,'w'},
	{0x1e,'2'},
	{0x21,'c'},
	{0x22,'x'},
	{0x23,'d'},
	{0x24,'e'},
	{0x25,'4'},
	{0x26,'3'},
	{0x29,' '},
	{0x2a,'v'},
	{0x2b,'f'},
	{0x2c,'t'},
	{0x2d,'r'},
	{0x2e,'5'},
	{0x31,'n'},
	{0x32,'b'},
	{0x33,'h'},
	{0x34,'g'},
	{0x35,'y'},
	{0x36,'6'},
	{0x3a,'m'},
	{0x3b,'j'},
	{0x3c,'u'},
	{0x3d,'7'},
	{0x3e,'8'},
	{0x41,','},
	{0x42,'k'},
	{0x43,'i'},
	{0x44,'o'},
	{0x45,'0'},
	{0x46,'9'},
	{0x49,'.'},
	{0x4a,'/'},
	{0x4b,'l'},
	{0x4c,';'},
	{0x4d,'p'},
	{0x4e,'-'},
	{0x52,'\''},
	{0x54,'['},
	{0x55,'='},
	{0x5a,13},
	{0x5b,']'},
	{0x5d,'\\'},
	{0x66,8},
	{0x69,'1'},
	{0x6b,'4'},
	{0x6c,'7'},
	{0x70,'0'},
	{0x71,'.'},
	{0x72,'2'},
	{0x73,'5'},
	{0x74,'6'},
	{0x75,'8'},
	{0x79,'+'},
	{0x7a,'3'},
	{0x7b,'-'},
	{0x7c,'*'},
	{0x7d,'9'},
	{0x76,27},  //ESC

	{0x05,128}, //F1
	{0x06,129}, //F2
	{0x04,130}, //F3
	{0x0c,131}, //F4
	{0x03,132}, //F5
	{0x0b,133}, //F6
	{0x83,134}, //F7
	{0x0a,135}, //F8
	{0x01,136}, //F9
	{0x09,137}, //F10
	{0,0}
};

// Shifted characters
const unsigned char shifted[][2] PROGMEM= {
	{0x0d,9},
	{0x0e,'~'},
	{0x15,'Q'},
	{0x16,'!'},
	{0x1a,'Z'},
	{0x1b,'S'},
	{0x1c,'A'},
	{0x1d,'W'},
	{0x1e,'@'},
	{0x21,'C'},
	{0x22,'X'},
	{0x23,'D'},
	{0x24,'E'},
	{0x25,'$'},
	{0x26,'#'},
	{0x29,' '},
	{0x2a,'V'},
	{0x2b,'F'},
	{0x2c,'T'},
	{0x2d,'R'},
	{0x2e,'%'},
	{0x31,'N'},
	{0x32,'B'},
	{0x33,'H'},
	{0x34,'G'},
	{0x35,'Y'},
	{0x36,'^'},
	{0x3a,'M'},
	{0x3b,'J'},
	{0x3c,'U'},
	{0x3d,'&'},
	{0x3e,'*'},
	{0x41,'<'},
	{0x42,'K'},
	{0x43,'I'},
	{0x44,'O'},
	{0x45,')'},
	{0x46,'('},
	{0x49,'>'},
	{0x4a,'?'},
	{0x4b,'L'},
	{0x4c,':'},
	{0x4d,'P'},
	{0x4e,'_'},
	{0x52,'\"'},
	{0x54,'{'},
	{0x55,'+'},
	{0x5a,13},
	{0x5b,'}'},
	{0x5d,'|'},
	{0x66,8},
	{0x69,'1'},
	{0x6b,'4'},
	{0x6c,'7'},
	{0x70,'0'},
	{0x71,'>'},
	{0x72,'2'},
	{0x73,'5'},
	{0x74,'6'},
	{0x75,'8'},
	{0x79,'+'},
	{0x7a,'3'},
	{0x7b,'-'},
	{0x7c,'*'},
	{0x7d,'9'},
	{0x76,27},  //ESC
	{0x05,128}, //F1
	{0x06,129}, //F2
	{0x04,130}, //F3
	{0x0c,131}, //F4
	{0x03,132}, //F5
	{0x0b,133}, //F6
	{0x83,134}, //F7
	{0x0a,135}, //F8
	{0x01,136}, //F9
	{0x09,137}, //F10
	{0,0}
};

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


u8 is_up=0, shift = 0, mode = 0;
u16 ctrl,alt;
bool is_extended=false;
/*
 * Decodes a keyboard scancode to an ASCII character
 * return value is 16 bit:
 * LSB: Standard ASCII character
 * MSB: bit 0: Is a non-ascii extended character (ex num pad keys)
 * 		bit 1: Control key was pressed
 * 		bit 2: Alt key was pressed
 */
u16 decode(u8 sc)
{
	if(sc==0xe0){
		is_extended=true;
		return 0;
	}else{
		unsigned char i,c;
		if (!is_up)// Last data received was the up-key identifier
		{
			switch (sc)
			{
				case 0xF0 :// The up-key identifier
					is_up = 1;
					break;

				case 0x12 :// Left SHIFT
				case 0x59 :// Right SHIFT
					shift = 1;
					break;

				case 0x14 :// Left Control (right ctrl is extended 0x0e+0x14)
					ctrl = KB_CTRL_FLAG; //ctlr flag bit
					break;

				case 0x11 :// Left Alt (right Alt is extended 0x0e+0x11)
					alt = KB_ALT_FLAG; //alt flag bit
					break;

				default:
					if(is_extended){
						is_extended=false;
						return alt|ctrl|0x0100|sc;
					}else{

						if(!shift)// If shift not pressed,
						{ // do a table look-up
							for(i = 0; pgm_read_byte(&(unshifted[i][0]))!=sc && pgm_read_byte(&(unshifted[i][0])); i++);
							if (pgm_read_byte(&(unshifted[i][0])) == sc) {
								c=pgm_read_byte(&(unshifted[i][1]));

								return alt|ctrl|c;
							}
						} else {// If shift pressed
							for(i = 0; pgm_read_byte(&(shifted[i][0]))!=sc && pgm_read_byte(&(shifted[i][0])); i++);
							if (pgm_read_byte(&(shifted[i][0])) == sc) {
								c=pgm_read_byte(&(shifted[i][1]));

								return alt|ctrl|c;
							}
						}
					}
					break;
			}

		} else {
			is_up = 0;// Two 0xF0 in a row not allowed
			switch (sc)
			{
				case 0x12 :// Left SHIFT
				case 0x59 :// Right SHIFT
					shift = 0;
					break;

				case 0x14 :// Left control (right ctrl is extended 0x0e+0x14)
					ctrl = 0;
					break;

				case 0x11 :// Left Alt (right Alt is extended 0x0e+0x11)
					alt = 0; //alt flag bit
					break;
			}
		}

	}
	is_extended=false;
	return 0; //invalid char
}


