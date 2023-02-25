/*
 * Keyboard handler. Responsible to poll for keys
 * on a Uzebox keyboard and translate them to ascii.
 *
 * Created on: Avril 11, 2022
 * Author: Uze
 */

#include <stdbool.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "keyboard.h"

#define KB_SEND_KEY 0x00
#define KB_SEND_END 0x01
#define KB_SEND_DEVICE_ID 0x02
#define KB_SEND_FIRMWARE_REV 0x03
#define KB_RESET 0x7f

#define KB_MAX_SCANCODE 0x83

#define Wait200ns() asm volatile("lpm\n\tlpm\n\t");
#define Wait100ns() asm volatile("lpm\n\t");

u8	 _ReadKey(u8 command);				//private function to read a key from the  keyboard
void _Decode(u8 sc);					//private function to decode scan codes to ascii caracters

const char *debugString;				//Holds the simulated keystrokes
u8 bufferedKey=0,bufferedKeyShifted=0; 	//Last key read by KeyboardPoll()
u8 modifiers=0; 						//Current state of modifier keys
u8 pause_byte_count=0;					//Pause key make lenght counter. Pause has a special 7 bytes scan code.
/*
** Keyboard scan code to ascii mapping for scancodes set 2.
**
** Array index corresponds to keyboard key codes.
** First value is unshifted key, second value is shifted key.
**
** See: http://www.quadibloc.com/comp/scan.htm
**      https://webdocs.cs.ualberta.ca/~amaral/courses/329/labs/scancodes.html
*/
const unsigned char keymap[][2] PROGMEM = { //80 keys mapped

	{0,0},				//0x00
	{KB_F9,KB_F9}, 		//0x01 - F9
	{0,0},				//0x02
	{KB_F5,KB_F5}, 		//0x03 - F5
	{KB_F3,KB_F3},		//0x04 - F3
	{KB_F1,KB_F1},		//0x05 - F1
	{KB_F2,KB_F2},		//0x06 - F2
	{KB_F12,KB_F12},	//0x07 - F12
	{0,0},				//0x08
	{KB_F10,KB_F10},	//0x09 - F10
	{KB_F8,KB_F8}, 		//0x0a - F8
	{KB_F6,KB_F6}, 		//0x0b - F6
	{KB_F4,KB_F4}, 		//0x0c - F4
	{KB_TAB,KB_TAB},	//0x0d - TAB
	{'`','~'}, 			//0x0e - Back Quote
	{0,0},				//0x0f
	{0,0},				//0x10
	{0,0},				//0x11
	{0,0},				//0x12
	{0,0},				//0x13
	{0,0},				//0x14
	{'q','Q'}, 			//0x15 - q
	{'1','!'},			//0x16 - 1
	{0,0},				//0x17
	{0,0},				//0x18
	{0.0},				//0x19
	{'z','Z'},			//0x1a - z
	{'s','S'}, 			//0x1b - s
	{'a','A'},			//0x1c - a
	{'w','W'},			//0x1d - w
	{'2','@'},			//0x1e - 2
	{0,0},				//0x1f
	{0,0},				//0x20
	{'c','C'},			//0x21 - c
	{'x','X'},			//0x22 - x
	{'d','D'},			//0x23 - d
	{'e','E'},			//0x24 - e
	{'4','$'},			//0x25 - 4
	{'3','#'},			//0x26 - 3
	{0,0},				//0x27
	{0,0},				//0x28
	{KB_SPACE,KB_SPACE},//0x29 - Spacebar
	{'v','V'},			//0x2a - v
	{'f','F'},			//0x2b - f
	{'t','T'},			//0x2c - t
	{'r','R'},			//0x2d - r
	{'5','%'},			//0x2e - 5
	{0,0},				//0x2f
	{0,0},				//0x30
	{'n','N'},			//0x31 - n
	{'b','B'},			//0x32 - b
	{'h','H'},			//0x33 - h
	{'g','G'},			//0x34 - g
	{'y','Y'},			//0x35 - y
	{'6','^'},			//0x36 - 6
	{0,0},				//0x37
	{0,0},				//0x38
	{0,0},				//0x39
	{'m','M'},			//0x3a - m
	{'j','J'},			//0x3b - j
	{'u','U'},			//0x3c - u
	{'7','&'},			//0x3d - 7
	{'8','*'},			//0x3e - 8
	{0,0},				//0x3f
	{0,0},				//0x40
	{',','<'},			//0x41 - ,
	{'k','K'},			//0x42 - k
	{'i','I'},			//0x43 - i
	{'o','O'},			//0x44 - o
	{'0',')'},			//0x45 - 0
	{'9','('},			//0x46 - 9
	{0,0},				//0x47
	{0,0},				//0x48
	{'.','>'},			//0x49 - .
	{'/','?'},			//0x4a - /
	{'l','L'},			//0x4b - l
	{';',':'},			//0x4c - ;
	{'p','P'},			//0x4d - p
	{'-','_'},			//0x4e - -
	{0,0},				//0x4f
	{0,0},				//0x50
	{0,0},				//0x51
	{'\'','\"'},		//0x52 - '
	{0,0},				//0x53
	{'[','{'},			//0x54 - [
	{'=','+'},			//0x55 - =
	{0,0},				//0x56
	{0,0},				//0x57
	{0,0},				//0x58
	{0,0},				//0x59
	{KB_ENTER,KB_ENTER},//0x5a - Enter
	{']','}'},			//0x5b - ]
	{0,0},				//0x5c
	{'\\','|'},			//0x5d - Baskslash
	{0,0},				//0x5e
	{0,0},				//0x5f
	{0,0},				//0x60
	{0,0},				//0x61
	{0,0},				//0x62
	{0,0},				//0x63
	{0,0},				//0x64
	{0,0},				//0x65
	{KB_BCKSP,KB_BCKSP},//0x66 - Backspace
	{0,0},				//0x67
	{0,0},				//0x68
	{'1','1'}, 			//0x69 - keypad 1
	{0,0},				//0x6a
	{'4','4'},			//0x6b - keypad 4
	{'7','7'},			//0x6c - keypad 7
	{0,0},				//0x6d
	{0,0},				//0x6e
	{0,0},				//0x6f
	{'0','0'},			//0x70 - keypad 0
	{'.','>'},			//0x71 - .
	{'2','2'},			//0x72 - keypad 2
	{'5','5'},			//0x73 - keypad 5
	{'6','6'},			//0x74 - keypad 6
	{'8','8'},			//0x75 - keypad 8
	{KB_ESC,KB_ESC},	//0x76 - ESC
	{0,0},				//0x77
	{KB_F11,KB_F11},	//0x78 - F11
	{'+','+'},			//0x79 - keypad +
	{'3','3'},			//0x7a - keypad 3
	{'-','-'},			//0x7b - keypad -
	{'*','*'},			//0x7c - keypad *
	{'9','9'},			//0x7d - keypad 9
	{KB_SCRLK,KB_SCRLK},//0x7e - Scroll Lock
	{0,0},				//0x7f
	{0,0},				//0x80
	{0,0},				//0x81
	{0,0},				//0x82
	{KB_F7,KB_F7}		//0x83 - F7
};

/*
*  Poll the keyboard for an available keystroke and buffers it.
*  The function will not poll the keyboard until the buffered key is read.
*/
void KeyboardPoll(void){
	if(bufferedKey==0){
		u8 key=_ReadKey(KB_SEND_END);
		if(key!=0){
			_Decode(key);
		}
	}

	if(bufferedKey==0 && debugString!=NULL){
		u8 key=pgm_read_byte(debugString);
		if(key!=0){
			bufferedKey=key;
			bufferedKeyShifted=key;
			debugString++;
		}else{
			debugString=NULL;
		}
	}

}

/*
** Returns true if a normal key was fetched by the last keyboard poll.
** Normal keys are all ones except the modifier keys SHIFT, ALT, CTRL and WINKEY.
*/
bool KeyboardHasKey(void){
	return (bufferedKey!=0);
}

/*
** Return the last key fetched from the last keyboard polling. If no new key is available, it returns 0.
**
** If the parameter == true, the function will decode considereing the state of the shift key.
 * Ex.: If Shift is pressed and key is 'a' then it will return 'A'. Otherwise 'a' is always
 * returned regardless of the state if the Shift key.
*/
u8 KeyboardGetKey(bool shifted){
	if(bufferedKey!=0){
		u8 temp;
		if(shifted && (modifiers & KB_FLAG_SHIFT)){
			temp=bufferedKeyShifted;
		}else{
			temp=bufferedKey;
		}
		bufferedKey=0;
		return temp;
	}else{
		return 0;
	}
}

/**
 * Return the state of key modifiers SHIFT, ALT, CTRL and WINKEY.
 *
 * Use the KB_FLAG_* constants to check for the desired modifiers.
 * Ex: if(KeyboardGetModifiers() & KB_FLAG_ALT) DoSomething();
 */

u8 KeyboardGetModifiers(){
	return modifiers;
}

/**
 * Reads the keyboard using the Uzebus protocol
 * command: KB_SEND_END == reads a single key
 * For garanteed stability this function should not
 * be interrupted, i.e: it should be called through
 * a vsync handler by the KeyboardPoll() funciton.
 */
u8 _ReadKey(u8 command){
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

	//read bits serially
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



u8 is_up=0, mode = 0;
bool is_extended=false;
/*
 * Decodes a keyboard scancode to an ASCII character.
 *
 * Key down ("make" codes) are mostly single bytes.
 * Key up ("break" codes) is two bytes: 0xF0 then the make keycode.
 *
 * return value is 16 bit:
 * LSB: Decoded key with printable caracters in ASCII
 * MSB: bit 0: Shift key is pressed
 * 		bit 1: Control key is pressed
 * 		bit 2: Alt key is pressed
 */
void _Decode(u8 sc)
{
	if(pause_byte_count){
		pause_byte_count--;
		if(pause_byte_count==0){
			bufferedKey=KB_PAUSE;
			bufferedKeyShifted=KB_PAUSE;
		}else{
			bufferedKey=0;
		}
		return;
	}

	if(sc==0xe0){
		is_extended=true;
		bufferedKey=0;
		return;
	}else if(sc==0xe1){
		pause_byte_count=7;// 7 more remaining bytes to ignore. Pause key has no break code.
		bufferedKey=0;
		return;
	}else{
		unsigned char c=0;
		if (!is_up)// Last data received was the up-key identifier
		{
			switch (sc)
			{
				case 0xF0 :// The up-key identifier
					is_up = 1;
					break;

				case 0x12 :// Left SHIFT
				case 0x59 :// Right SHIFT
					modifiers |= KB_FLAG_SHIFT;
					break;

				case 0x14 :// Left Control (right ctrl is extended 0x0e+0x14)
					modifiers |= KB_FLAG_CTRL;
					break;

				case 0x11 :// Left Alt (right Alt is extended 0x0e+0x11)
					modifiers |= KB_FLAG_ALT;
					break;

				default:

					if(is_extended){
						//is_extended=false;

						//map/process extended keys
						switch(sc){
							case 0x7c:	//Print screen
								c=KB_PRTSC;
								break;
							case 0x1f:
							case 0x27:		//Windows modifier key (L+R)
								modifiers |= KB_FLAG_WIN;
								bufferedKey=0;
								return;
								break;
							case 0x2f:			 //Applications modifier key
								modifiers |= KB_FLAG_APPS;
								bufferedKey=0;
								return;
								break;
							case 0x69:	//end
								c=KB_END;
								break;
							case 0x6b:	//left arrow
								c=KB_LEFT;
								break;
							case 0x6c:	//home
								c=KB_HOME;
								break;
							case 0x70:	//insert
								c=KB_INS;
								break;
							case 0x71:	//delete
								c=KB_DEL;
								break;
							case 0x72:	//down arrow
								c=KB_DOWN;
								break;
							case 0x74:	//right arrow
								c=KB_RIGHT;
								break;
							case 0x75:	//up arrow
								c=KB_UP;
								break;
							case 0x7A:	//page down
								c=KB_DOWN;
								break;
							case 0x7d:	//page up
								c=KB_PGUP;
								break;
							case 0x7e:	//ctrl+break
								c=KB_BREAK;
								break;
						}
						bufferedKey=c;
						bufferedKeyShifted=c;
					}else{
						if(sc<=KB_MAX_SCANCODE){
							bufferedKey=pgm_read_byte(&(keymap[sc][0]));
							bufferedKeyShifted=pgm_read_byte(&(keymap[sc][1]));
						}
					}
					return;
					break;
			}

		} else {
			is_up = 0;// Two 0xF0 in a row not allowed
			switch (sc)
			{
				case 0x12 :// Left SHIFT
				case 0x59 :// Right SHIFT
					modifiers &=(~KB_FLAG_SHIFT);
					break;

				case 0x14 :// Left control (right ctrl is extended 0x0e+0x14)
					modifiers &=(~KB_FLAG_CTRL);
					break;

				case 0x11 :// Left Alt (right Alt is extended 0x0e+0x11)
					modifiers &=(~KB_FLAG_ALT);
					break;

				default:

					if(is_extended){
						//map/process extended keys
						switch(sc){
							case 0x1f:
							case 0x27: //Windows key
								modifiers &=(~KB_FLAG_WIN);
								break;
							case 0x2f:
								modifiers &=(~KB_FLAG_APPS); //Applications modifier key
								break;
						}
					}
					break;
			}
			is_extended=false;
		}
	}

	bufferedKey=0;
}


/**
 * Simulates a series of keystrokes from the keyboard.
 */
void KeyboardDebugSend(const char *string){
	debugString=string;
}

