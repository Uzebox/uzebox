/*
 *  Terminal emulator for the Uzebox. Partially emulates the vt100/ansi commands set.
 *
 *	Protocol information for ANSI/VT100:
 *		https://web.archive.org/web/20210226122732/http://ascii-table.com/ansi-escape-sequences.php
 *		https://espterm.github.io/docs/VT100%20escape%20codes.html
 *		https://www.xfree86.org/current/ctlseqs.html
 *		https://www.real-world-systems.com/docs/ANSIcode.html
 *		http://wiki.bash-hackers.org/scripting/terminalcodes
 *
 */

#include <stdbool.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "uzenet.h"
#include "keyboard.h"
#include "terminal.h"


#define X_ORIGIN			0
#define Y_ORIGIN 			0
#define FIRST_TILE_OFFSET	32
#define CUR_DEF_SPEED		30
#define CUR_TILE 			123
#define XMIT_BUF_SIZE 		32	//must be a power of 2

const char ESC_KEY_UP_DEC[]		PROGMEM = "\x1bOA";
const char ESC_KEY_UP[]			PROGMEM = "\x1b[A";
const char ESC_KEY_DOWN_DEC[]  	PROGMEM = "\x1bOB";
const char ESC_KEY_DOWN[] 		PROGMEM = "\x1b[B";
const char ESC_KEY_RIGHT_DEC[]	PROGMEM = "\x1bOC";
const char ESC_KEY_RIGHT[]		PROGMEM = "\x1b[C";
const char ESC_KEY_LEFT_DEC[]  	PROGMEM = "\x1bOD";
const char ESC_KEY_LEFT[] 		PROGMEM = "\x1b[D";
const char ESC_KEY_HOME_DEC[] 	PROGMEM = "\x1bOH";
const char ESC_KEY_HOME[] 		PROGMEM = "\x1b[H";
const char ESC_KEY_END_DEC[]  	PROGMEM = "\x1bOF";
const char ESC_KEY_END[] 		PROGMEM = "\x1b[F";
const char ESC_KEY_INS[] 		PROGMEM = "\x1b[3~";
const char ESC_KEY_DEL[] 		PROGMEM = "\x1b[2~";
const char ESC_KEY_F1[] 		PROGMEM = "\x1b[11~";

const char ESC_CMD_TEXT_ATTR[] PROGMEM = "\x1b[%1dm";

int  cons_putchar_printf(char var, FILE *stream);
void cons_char(unsigned char c);
void terminal_AppendString_P(const char* str);
void terminal_ProcessKey(u8 c);
bool ansi_filter(u8 c);
bool utf8_filter(u8 c);
void process_joypad_entry();
void copyLine(u8 src, u8 dest);

bool DECCKM=false;				//Enable DEC escape sequences for extended keyboard keys
bool DECAWM=false;				//Enables Auto-wrap mode

//display list definition for video mode 80
m80_dlist_tdef dlist[1];
m80_cursor_tdef cursor;

//Terminal control variables
u8 cx=X_ORIGIN,cy=Y_ORIGIN;		//set cursor default position
u8 foregroudColor=TERM_COLOR_DEFAULT;
u8 backgroundColor=10;
bool echoEnable=false;
bool cursorEnable=true;
bool debugEcho=false;
bool inverseVideo;
u8 scroll_top_margin;
u8 scroll_bottom_margin;


//ANSI filter variables
bool escaping=false, is_ansi=false, csi_extension=false;
char csi_buffer[32];
u8 command_line;
u8 csi_ptr;
u8 csi_params[10];
u8 csi_param_ptr;
u8 csi_param_count;
u16 esc_pos=0;
char escape_buf[20];
char param_buf[7];
char parsed_param[7];

//circular buffer and pointers to holds ansi data for the output stream
u8 xmit_buf[XMIT_BUF_SIZE];
volatile u8 xmit_headPtr=0;
volatile u8 xmit_tailPtr=0;

//Input stream definition macro. Allows the use of stdio functions.
FILE TERMINAL_STREAM = FDEV_SETUP_STREAM(cons_putchar_printf, NULL, _FDEV_SETUP_WRITE);

/*
 * Initialize the terminal.
 */
void terminal_Init(){

#ifdef JTAG_DEBUG
	//must use different colors because JTAG needs some
	//pins used by the video DAC
	dlist[0].bgc = 0x00; //dark
	dlist[0].fgc = 0xff; //light
#else
	dlist[0].bgc = 10; //dark amber
	dlist[0].fgc = 38; //light amber
#endif
	dlist[0].vramrow = 0U;
	dlist[0].tilerow = 0U;
	dlist[0].next = 0U;   // End of Display List
	m80_dlist = &dlist[0];

	cursor.x=0;
	cursor.y=0;
	cursor.blinkrate=30;
	cursor.currdelay=30;
	cursor.active=false;
	cursor.state=0;
	m80_cursor = &cursor;

	//Terminal control variables
	cx=X_ORIGIN;
	cy=Y_ORIGIN;		//set cursor default position
	foregroudColor=TERM_COLOR_DEFAULT;
	backgroundColor=10;
	echoEnable=false;
	cursorEnable=true;
	debugEcho=false;
	inverseVideo=false;
	escaping=false;
	is_ansi=false;
	csi_extension=false;
	esc_pos=0;
	xmit_headPtr=0;
	xmit_tailPtr=0;
	DECCKM=false;
	DECAWM=false;

	scroll_top_margin=0;
	scroll_bottom_margin=24;
}

/*
 * This function is called by stdio vprintf functions via a stream handler
 */
int cons_putchar_printf(char c, FILE *stream) {
	//filter the stream for ANSI escape sequences and UTF-8 characters
	//filters return false to stop the filter chain and true
	//to continue or print the received char
	if(!ansi_filter(c))return 0;
	if(!utf8_filter(c))return 0;
	cons_char(c);
    return 0;
}

/**
 * Send a char to the video terminal. Delegates to the stream handler.
 */
void terminal_SendChar(u8 c){
	cons_putchar_printf(c, NULL);
}

/**
 * Get the next available key from the terminal output buffer.
 * Call is blocking.
 */
u8 terminal_GetChar(){
    while(xmit_headPtr==xmit_tailPtr){};				// block waiting for a char to be appended

    u8 c = xmit_buf[xmit_tailPtr];
	xmit_tailPtr = (xmit_tailPtr + 1) % XMIT_BUF_SIZE;
    return c;
}

/**
 * Return if one or more keys are in the terminal output buffer.
 */
bool terminal_HasChar(){
	return xmit_headPtr!=xmit_tailPtr;
}

/**
 * Callback invoked 60 times per second.
 */
void terminal_VsyncCallback(){

	KeyboardPoll();			//checks for new keys

	//process the keyboard
	if(KeyboardHasKey()){
		u8 key=KeyboardGetKey(true);
		terminal_ProcessKey(key);
	}
}


/**
 * Append a char to the transmit buffer.
 */
void terminal_TransmitChar(u8 c){
    xmit_buf[xmit_headPtr] = c;
    xmit_headPtr = (xmit_headPtr + 1) % XMIT_BUF_SIZE;
}

/**
 * Add the specified string in program memory
 * to the transmit buffer.
 */
void terminal_TransmitString_P(const char* str){

	while(1){
		char c=pgm_read_byte(str++);
		if(c==0)break;
		terminal_TransmitChar(c);
	};
}

/**
 * Used to handle the keyboard input. Resolves
 * extended key (ex arrows keys) to ansi escapes sequences
 * and transmits them to the host via a buffer.
 *
 * https://vt100.net/docs/vt100-ug/chapter3.html
 */
void terminal_ProcessKey(u8 c){

	u8 mods=KeyboardGetModifiers();
	const char* command=NULL;
	if((mods & KB_FLAG_CTRL) && ((c&0xff)>='a' && (c&0xff)<='z')){
		terminal_TransmitChar((c&0xff)-'a'+1);	//process Control-A to Control-Z keys
	}else{
		switch(c){
			case KB_UP: //Up arrow
				if(DECCKM){
					command=ESC_KEY_UP_DEC;
				}else{
					command=ESC_KEY_UP;
				}
				break;

			case KB_DOWN: //Down arrow
				if(DECCKM){
					command=ESC_KEY_DOWN_DEC;
				}else{
					command=ESC_KEY_DOWN;
				}
				break;

			case KB_RIGHT: //Right arrow
				if(DECCKM){
					command=ESC_KEY_RIGHT_DEC;
				}else{
					command=ESC_KEY_RIGHT;
				}
				break;

			case KB_LEFT: //Left arrow
				if(DECCKM){
					command=ESC_KEY_LEFT_DEC;
				}else{
					command=ESC_KEY_LEFT;
				}
				break;

			case KB_HOME: //home
				if(DECCKM){
					command=ESC_KEY_HOME_DEC;
				}else{
					command=ESC_KEY_HOME;
				}
				break;

			case KB_END: //end
				if(DECCKM){
					command=ESC_KEY_END_DEC;
				}else{
					command=ESC_KEY_END;
				}
				break;

			case KB_INS:	//Insert
				command=ESC_KEY_INS;
				break;

			case KB_DEL:	//Delete
				command=ESC_KEY_DEL;
				break;

			case KB_F1:  //F1
				command=ESC_KEY_F1;
				break;

			case KB_F9:  	//F9
				if(foregroudColor==40	){
					foregroudColor=0xff;		//white
				}else if(foregroudColor==0xff){
					foregroudColor=38;			//amber
				}else if(foregroudColor==38){
					foregroudColor=40;	//green
				}
				//SetForegroundColor(console_foregroudColor);
				terminal_SetColors(foregroudColor,backgroundColor);
				break;

			case KB_F10:  	//F10
				if(backgroundColor==0){
					backgroundColor=10;
				}else if(backgroundColor==10){
					backgroundColor=2;
				}else{
					backgroundColor=0;
				}
				terminal_SetColors(foregroudColor,backgroundColor);
				break;

			case KB_F11:	//F11
				terminal_SetCursorVisible(true);
				break;

			case KB_F12:	//F12
				terminal_SetCursorVisible(false);
				break;

			case 0:
				break;

			default:
				if(echoEnable){
					cons_char(c);
				}
				terminal_TransmitChar(c);
				break;
		}

		if(command!=NULL) terminal_TransmitString_P(command);
	}

	terminal_MoveCursor(cx,cy);
}



void terminal_SetAutoWrap(bool enable){
	DECAWM=enable;
}

void terminal_Echo(bool enable){
	echoEnable=enable;
}

void terminal_ClearScreen(){
	ClearVram();
}

void terminal_MoveCursor(u8 x,u8 y){
	//update terminal cursor variables
	cx=x;
	cy=y;

	//update mode80 cursor
	cursor.x=x;
	cursor.y=y;
}

void terminal_SetCursorVisible(bool visible){
	cursor.active=visible;
}

void terminal_SetCursorBlinkRate(u8 rate){
	cursor.blinkrate=rate;
}

/*
void terminal_SetVerticalScroll(u8 row){
	dlist[0].vramrow=row;
}

u8	terminal_GetVerticalScroll(){
	return dlist[0].vramrow;
}
*/

void terminal_SetScrollMargins(u8 top, u8 bottom){
	scroll_top_margin=top;
	scroll_bottom_margin=bottom;
}

void terminal_VerticalScrollUp(bool clearNewLine){

	//copy margin lines down 1 vram row
	//TODO: top margin not supported, most be 0
	if(scroll_bottom_margin != (SCREEN_TILES_V-1)){
			for(u8 i=(SCREEN_TILES_V-1);i>scroll_bottom_margin;i--){
			copyLine(i,i+1);

		}
	}

	if(dlist[0].vramrow == SCREEN_TILES_V-1){
		dlist[0].vramrow = 0;
	}else{
		dlist[0].vramrow++;
	}

	if(scroll_bottom_margin == (SCREEN_TILES_V-1)){
		terminal_ClearLine(SCREEN_TILES_V-1,0,SCREEN_TILES_H-1);
	}else{
		terminal_ClearLine(scroll_bottom_margin,0,SCREEN_TILES_H-1);
	}

}

void terminal_VerticalScrollDown(bool clearNewLine){

	//copy margin lines down 1 vram row
	//TODO: top margin not supported, most be 0
	if(scroll_bottom_margin != (SCREEN_TILES_V-1)){
		for(u8 i=scroll_bottom_margin;i<SCREEN_TILES_V;i++){
			copyLine(i+1,i);
		}
	}

	if(dlist[0].vramrow == 0){
		dlist[0].vramrow = SCREEN_TILES_V-1;
	}else{
		dlist[0].vramrow--;
	}

	if(clearNewLine){
		terminal_ClearLine(0,0,SCREEN_TILES_H-1);
	}
}

void terminal_SetColors(u8 foreground,u8 background){
	dlist[0].fgc = foreground;
	dlist[0].bgc = background;
}

void terminal_PutCharAtLoc(u8 x,u8 y, u8 character,u8 attributes){
	//if inverse attribute is on, use 2nd bank of font which color is inverted

	y+=dlist[0].vramrow;	//Add scrolling offset
	if(y>=VRAM_TILES_V) y-=VRAM_TILES_V;

	SetTile(x,y,character-32+(attributes!=0?128:0));
}

u8 	terminal_GetCharAtLoc(u8 x,u8 y){
	return GetTile(x,y);
}

void terminal_ClearLine(u8 row, u8 startColumn, u8 endColumn){
	row+=dlist[0].vramrow;	//Add scrolling offset
	if(row>=VRAM_TILES_V) row-=VRAM_TILES_V;
	u16 pos=(row*VRAM_TILES_H)+startColumn;
	for(u8 i=startColumn;i<=endColumn;i++){
		vram[pos++]=0;
	}
}

/*
 * Copies the whole row of tiles from a source row to a destination row.
 * Accounts for scrolling offset.
 */
void copyLine(u8 src, u8 dest){
	src+=dlist[0].vramrow;	//Add scrolling offset
	if(src>=VRAM_TILES_V) src-=VRAM_TILES_V;
	u16 srcpos=(src*VRAM_TILES_H);

	dest+=dlist[0].vramrow;	//Add scrolling offset
	if(dest>=VRAM_TILES_V) dest-=VRAM_TILES_V;
	u16 destpos=(dest*VRAM_TILES_H);

	for(u8 i=0;i<=(VRAM_TILES_H-1);i++){
		vram[destpos++]=vram[srcpos++];
	}
}


void terminal_Clear(){
	ClearVram();
	cx=X_ORIGIN;
	cy=Y_ORIGIN;
}

/*
 * Outputs the characters to video ram.
 * Handles special control characters like tab, backspace and enter.
 */
void cons_char(unsigned char c){
	switch(c){
		case 8:	//backspace
			if(cx >X_ORIGIN) cx--;
			break;

		case 9: //tab
			cx=(((cx-X_ORIGIN)/8)*8)+8+X_ORIGIN;
			break;

		case 10: //line feed (\n)
			//if(cy==(SCREEN_TILES_V-1)){
			if(cy==(scroll_bottom_margin)){
				terminal_VerticalScrollUp(true);
			}else{
				cy++;
			}

			break;

		case 12:	//form feed (clear screen)
			terminal_Clear();
			break;

		case 13:	//carriage return (\r)
			cx=X_ORIGIN;
			break;

		case 32 ... 139:
			if(cx<SCREEN_TILES_H){
				terminal_PutCharAtLoc(cx++,cy,c, inverseVideo?TERM_ATTR_INVERSE_VIDEO:0);
			}else if(DECAWM){
				if(cy==(SCREEN_TILES_V-1)){
					terminal_VerticalScrollUp(true);
				}else{
					cy++;
				}
				cx=X_ORIGIN;
				terminal_PutCharAtLoc(cx++,cy,c,0);
			}
			break;

		default:
			if(debugEcho){
				if(cx>=(SCREEN_TILES_H-6)){
					if(cy==(SCREEN_TILES_V-1)){
						terminal_VerticalScrollUp(true);
					}else{
						cy++;
					}
					cx=X_ORIGIN;
				}

				PrintChar(cx++,cy,'<');
				PrintHexByte(cx,cy,c);
				cx+=2;
				PrintChar(cx++,cy,'>');
			}

			break;
	}

	terminal_MoveCursor(cx,cy);
}

/**
 * Filter to detect supported ANSI Escape sequence. Used to display
 * bullets and box drawing characters.
 *
 * ANSI escape sequences are in the form: ESC[5;20f
 *
 * Input: 	c = character to filter
 * Output:  0 = caller should end filter chain
 * 			1 = caller should continue with next filter or print the char
 */
bool ansi_filter(u8 c){
	if(escaping){
		if(is_ansi){
			csi_buffer[csi_ptr++]=c;
			if(c >= 0x40 && c <= 0x7f){ //received end command marker
				//parse the escape sequence
				command_line=c;
				csi_ptr=0;
				csi_param_count=0;
				csi_param_ptr=0;
				while(1) {
					c=csi_buffer[csi_ptr];

					if(c == command_line){ //end marker
						if(csi_ptr>0){
							csi_buffer[csi_ptr]=0; //add string terminator for atoi
							csi_params[csi_param_count++]=atoi(csi_buffer+csi_param_ptr);
						}else if(csi_param_count>0){
							csi_params[csi_param_count++]=0xff;	//empty param marker
						}
						break;
					}else if(c==';'){		//param separator
						if(csi_ptr>0){
							csi_buffer[csi_ptr]=0; //add string terminator for atoi
							csi_params[csi_param_count++]=atoi(csi_buffer+csi_param_ptr);
						}else{
							csi_params[csi_param_count++]=0xff;	//empty param marker
						}
						csi_param_ptr=csi_ptr+1;
					}else if(c=='?'){
						csi_param_ptr+=1;
						csi_extension=true;
					}

					csi_ptr++;
				};

				switch(c){
					case 'A':			//move up one line, stop at top of screen
						if(csi_param_count==0){
							if(cy>Y_ORIGIN) cy--;
							}else{
							cy-=csi_params[0];
						}
						terminal_MoveCursor(cx,cy);
						break;

					case 'B':			//move down one line, stop at bottom of screen
						if(csi_param_count==0){
							if(cy<(SCREEN_TILES_V-1)) cy++;
							}else{
							cy+=csi_params[0];
						}
						terminal_MoveCursor(cx,cy);
						break;

					case 'C':			//move forward one position, stop at right edge of screen
						if(csi_param_count==0){
							cx++;
							}else{
							cx+=csi_params[0];
						}
						if(cx>=SCREEN_TILES_H)cx=(SCREEN_TILES_H-1);
						terminal_MoveCursor(cx,cy);
						break;

					case 'D':			//move backwards one position, Same as BackSpace, stop at left edge of screen

						if(csi_param_count==0){
							if(cx>X_ORIGIN) cx--;
							}else{
							cx-=csi_params[0];
						}
						terminal_MoveCursor(cx,cy);
						break;

					case 'd': //vertical Position Absolute
						break;

					case 'g': //tabulation Clear
						//needed for lynx - ESC[3g on Scramworks
						break;

					case 'G': //horizontal absolute tab. Moves the cursor to column n (default 1).
						//needed for lynx on Scramworks
						break;

					case 'h':
						if(csi_extension){
							if(csi_params[0]==25){	//Show cursor
								terminal_SetCursorVisible(true);
								}else if(csi_params[0]==7){					//DECAWM - AutoWrap Mode, start newline after column 80
								DECAWM=true;
								}else if(csi_params[0]==1){					//DECCKM—Cursor Keys Mode
								DECCKM=true;
							}
						}
						break;

					case 'H':					//cursor position -- All cases covered
					case 'f':
						if(csi_param_count==0){
							//no coordinates,move to origin
							 //console_SetCursor(CONS_X_ORIGIN,CONS_Y_ORIGIN);
							 cx=X_ORIGIN;
							 cy=Y_ORIGIN;
						}else{

							if(csi_params[0]==0xff){
								cy=Y_ORIGIN;
							}else{
								cy=csi_params[0];
								if(cy==0) cy=1;
								cy= cy - 1 + Y_ORIGIN;
							}
							if(csi_params[1]==0xff){
								cx=X_ORIGIN;
							}else{
								cx=csi_params[1];
								if(cx==0) cx=1;
								cx= cx - 1 + X_ORIGIN;
							}

							//clip
							if(cy>=SCREEN_TILES_V)cy=SCREEN_TILES_V-1; //row
							if(cx>=SCREEN_TILES_H)cx=SCREEN_TILES_H-1; //column

						}
						terminal_MoveCursor(cx,cy);
						break;

					case 'J':	// erase display
						//[0J     erase from current position to bottom of screen inclusive
						//[1J     erase from top of screen to current position inclusive
						//[2J     erase entire screen (without moving the cursor)
						ClearVram(); //All cases erases the whole screen
						break;

					case 'K':	//erase a line
						if(csi_param_count==0 || (csi_param_count>0 && csi_params[0]==0)) {
							//clear line from cursor to end of line (same as [K )
							terminal_ClearLine(cy,cx,SCREEN_TILES_H-1);

							}else if(csi_params[0]==1){
							//clear line beginning to cursor
							terminal_ClearLine(cy,0,cx);

							}else if(csi_params[0]==2){
							//clear entire line
							terminal_ClearLine(cy,0,SCREEN_TILES_H-1);
						}
						break;

					case 'l':	//reset mode
						if(csi_extension){
							if(csi_params[0]==25){ 	//### hide cursor
								terminal_SetCursorVisible(false);
								}else if(csi_params[0]==7){					//DECAWM - AutoWrap Mode, start newline after column 80
								DECAWM=false;
								}else if(csi_params[0]==1){					//DECCKM—Cursor Keys Mode
								DECCKM=false;
							}
						}
						break;
/*ansi mode only
					case 'L':	//Insert lines

						if(cy==Y_ORIGIN){ //currently only supports inserting from top of screen
							u8 lines=1;
							if(csi_param_count==1) lines=csi_params[0];
							for(u8 i=0;i<lines;i++){
								terminal_VerticalScrollDown(true);
							}
						}
						break;
*/
					case 'm':	//graphics mode related
						if(csi_param_count==0){
							inverseVideo=false;
						}else{
							//iterate params to apply attributes
							for(u8 i=0;i<csi_param_count;i++){
								if(csi_params[i]==0){
									inverseVideo=false;
								}else if(csi_params[i]==7){
									inverseVideo=true;
								}
							}
						}

						break;
/* ansi mode only
					case 'M':	//Delete lines

						if(cy==Y_ORIGIN){ //currently only supports deleting from top of screen
							u8 lines=1;
							if(csi_param_count==1) lines=csi_params[0];
							for(u8 i=0;i<lines;i++){
								terminal_VerticalScrollUp(true);
							}
						}
						break;
*/
					case 'P':	//delete n Characters, from current position to end of field

						if(csi_param_count==0 || (csi_param_count>0 && csi_params[0]==1)){
							if(cx>X_ORIGIN){
								for(u8 x=cx;x<SCREEN_TILES_H;x++){
									c=terminal_GetCharAtLoc(x+1,cy);
									terminal_PutCharAtLoc(x,cy,c,0);
								}
								terminal_PutCharAtLoc(SCREEN_TILES_H-1,cy,' ',0);
							}
						}else{
							printf(" ESC[");
							for(int i=0;i<esc_pos;i++){
								putchar(escape_buf[i]);
							}
						}
						break;

					case 'r':	//top and bottom margins (scroll region on VT100).
						scroll_top_margin=csi_params[0]-1;
						scroll_bottom_margin=csi_params[1]-1;
						break;

					case 'S':	//scroll up
						terminal_VerticalScrollUp(true);
						break;

					case 'T':	//scroll down
						terminal_VerticalScrollDown(true);
						break;

					default:
						break;
				}

				//done!
				escaping=false;

			}
		}else if(c=='['){
			is_ansi=true;
			csi_ptr=0;
		}else{
			//unsupported non-ansi stuff

			if(c=='D'){ //(VT100) index IND - Move/scroll window up one line
				terminal_VerticalScrollUp(true);
			}else if(c=='M'){	//(VT100) revindex RI - Move/scroll window down one line
				terminal_VerticalScrollDown(true);
			}

			escaping=false;
		}
	}else{
		if(c==27){
			escaping=true;
			is_ansi=false;
			return false;
		}else{
			return true;
		}

	}

	return false;
}


/**
 * Filter to detect UTF8 sequences. Principally used to detect extended characters like bullets and box drawing symbols.
 * Input: c = character to filter
 * Output:  0 = caller should end filter chain
 * 			1 = caller should continue with next filter or print the char
 */
bool utf8_seq=false;	//indicates we are in utf8 sequence
u8 bytes[4]; 			//Incoming data bytes, b1 comes first
u8 utf8_lenght;			//length of sequence
u8 utf8_curr;			//current byte
bool utf8_filter(u8 c){

	if(utf8_seq==false && (c&0x80)){ //check if beginning of utf-8
		if((c&0b11100000)==0b11000000){ //2-bytes UTF-8 character detected
			utf8_lenght=2;
		}else if((c&0b11110000)==0b11100000){ //3-bytes UTF-8 character detected
			utf8_lenght=3;
		}else if((c&0b11111000)==0b11110000){ //4-bytes UTF-8 character detected
			utf8_lenght=4;
		}else{
			cons_char(128); 	//square symbol - unsupported characters
			return false;	//no sequence
		}
		utf8_seq=true;
		utf8_curr=0;
		bytes[utf8_curr++]=c;

	}else if(utf8_seq){
		u8 utf_char=128;//'?';

		bytes[utf8_curr++]=c;		//store next byte in sequence
		if(utf8_curr<utf8_lenght) return false;	//wait for more chars

		if(utf8_lenght==2){		//2 bytes char
			if(bytes[0]==0xc2 && bytes[1]==0xa0){
				utf_char=' '; //non-breakable space (&nbsp);
			}
		}else if(utf8_lenght==3){	//3 bytes char
			if(bytes[0]==0xe2){
				if(bytes[1]==0x80 && bytes[2]==0xa2){	//Bullet
					utf_char=127;
				}else if (bytes[1]==0x80 && bytes[2]==0x93){
					utf_char='-'; 			//long dash
				}else if (bytes[1]==0x94){		//Box drawing tiles

					switch(bytes[2]){
						case 0x82:
							utf_char=0x86;	//134:  |   Vertical line
							break;
						case 0x8c:
							utf_char=0x81; 	//129:  __  Corner facing down+right
							break;			//     |
						case 0x80:
							utf_char=0x85;	//133: ---  Horizontal line
							break;
						case 0x90:
							utf_char=0x82; 	//130: __   Corner facing down+left
							break;			//       |
						case 0x94:
							utf_char=0x83; 	//131: |    Corner facing up+right corner
							break;			//      --
						case 0x98:
							utf_char=0x84;	//132:    | Corner facing up+left corner
							break;			//      --
						case 0x9c:
							utf_char=0x87;	//135:  |-
							break;
						case 0xa4:
							utf_char=0x88;	//136: -|
							break;
						case 0xb4:
							utf_char=0x89;	//137:  |
							break;			//     ---
						case 0xac:
							utf_char=0x8A;	//138: ---
							break;			//      |
						case 0xbc:
							utf_char=0x8B;	//139: -|-
							break;
						default:
							utf_char=0x80; //undefined
							break;
					}
				}
			}
		}	//if it was 4 bytes utf8 sequence, we do nothing

		cons_char(utf_char);
		utf8_seq=false;

	}else{
		return true; //possibly a normal printable character
	}

	return false;
}
