/*
 *  Telnet terminal emulator for the Uzebox. Emulates vt100 basic commands.
 *
 *	Protocol information:
 *		https://www.real-world-systems.com/docs/ANSIcode.html
 *		http://pcmicro.com/netfoss/telnet.html
 *		http://wiki.bash-hackers.org/scripting/terminalcodes
 *		http://ascii-table.com/ansi-escape-sequences.php
 *		https://www.xfree86.org/4.8.0/ctlseqs.html
 *
 *	Tools to troubleshoot the protocol:
 *	  windump -w out.pcap -s 0 port 23 : capture packets
 *	  wireshark : Protocol analysis of captured packets
 */


#include <stdbool.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "keyboard.h"
#include "console.h"
#include "uzenet.h"

#define _TELNET_SB	 	250 //FA

#define _TELNET_WILL 	251 //FB
#define _TELNET_WONT 	252 //FC
#define _TELNET_DO 		253 //FD
#define _TELNET_DONT 	254 //FE

#define _TELNET_ECHO	1	//01
#define _TELNET_SGA		3	//03
#define _TELNET_STATUS	5	//05
#define _TELNET_DTT		24	//18
#define _TELNET_NEW_ENVIRON 39 //27
#define _TELNET_NAWS	31	//1F

#define _FIRST_TILE_OFFSET	32

#define _CONS_X_ORIGIN 2
#define _CONS_Y_ORIGIN 0
#define _CONS_CUR_DEF_SPEED 30
#define _CONS_CUR_TILE 123

bool DECCKM=false;

u8 console_foregroudColor=CONS_DEFAULT_COLOR;
u8 cx=_CONS_X_ORIGIN,cy=_CONS_Y_ORIGIN;
u16 _cons_flags=0;
bool readKeys=true;
bool console_echo_en=true;
bool console_cursor_enable=true;
bool DECAWM=false;
bool console_debug_echo=false;
u16 esc_pos=0;
char escape_buf[20];
char param_buf[7];
char parsed_param[7];



int _cons_putchar_printf(char var, FILE *stream);
void _cons_char(unsigned char c);

static FILE mystdout = FDEV_SETUP_STREAM(_cons_putchar_printf, NULL, _FDEV_SETUP_WRITE);

char GetFont(int x,int y){
	return GetTile(x,y)+_FIRST_TILE_OFFSET;
}

// this function is called by printf as a stream handler
int _cons_putchar_printf(char var, FILE *stream) {
    // translate \n to \r for br@y++ terminal
	_cons_char(var);
    return 0;
}

void setBackGroudColor(u8 color){
	for(int i=0;i<SCREEN_TILES_V;i++)backgroundColor[i]=color;
}

//executes each vsync
void ConsoleHandler(){

	if(readKeys){

		u8 key=GetKey(KB_SEND_END);
		if(key!=0){

			u16 c=decode(key);	//decode the pressed key
			if(c!=0){

				switch(c){
					case 0x175: //Up arrow
						if(DECCKM){
							wifi_SendString_P(PSTR("\x1bOA"));
						}else{
							wifi_SendString_P(PSTR("\x1b[A"));
						}
						break;
					case 0x172: //Down arrow
						if(DECCKM){
							wifi_SendString_P(PSTR("\x1bOB"));
						}else{
							wifi_SendString_P(PSTR("\x1b[B"));
						}
						break;
					case 0x174: //Right arrow
						if(DECCKM){
							wifi_SendString_P(PSTR("\x1bOC"));
						}else{
							wifi_SendString_P(PSTR("\x1b[C"));
						}
						break;
					case 0x16b: //Left arrow
						if(DECCKM){
							wifi_SendString_P(PSTR("\x1bOD"));
						}else{
							wifi_SendString_P(PSTR("\x1b[D"));
						}
						break;
					case 0x16c: //home
						if(DECCKM){
							wifi_SendString_P(PSTR("\x1bOH"));
						}else{
							wifi_SendString_P(PSTR("\x1b[H"));
						}
						break;
					case 0x169: //end
						if(DECCKM){
							wifi_SendString_P(PSTR("\x1bOF"));
						}else{
							wifi_SendString_P(PSTR("\x1b[F"));
						}
						break;
					case 128:  //F1
						wifi_SendString_P(PSTR("\x1b[11~"));
						break;
					case 129:  	//F2
						if(console_foregroudColor==0b00111000){
							console_foregroudColor=0xff;		//white
						}else if(console_foregroudColor==0xff){
							console_foregroudColor=38;			//amber
						}else if(console_foregroudColor==38){
							console_foregroudColor=0b00111000;	//green
						}
						SetForegroundColor(console_foregroudColor);
						break;
					case 130:  	//F3
						if(backgroundColor[0]==0){
							setBackGroudColor(10);
						}else{
							setBackGroudColor(0);
						}

						break;
					case 131:  	//F4
						wifi_SendString_P(PSTR("+++")); //Get out of esp8266 passthrough-mode
						break;
					/*
					case (KB_CTRL_FLAG|'c'):
						UartSendChar('\x03');
						break;
					case (KB_CTRL_FLAG|'f'):
						UartSendChar('\x06');
						break;
					*/

					case (KB_CTRL_FLAG|'a'):
					case (KB_CTRL_FLAG|'b'):
					case (KB_CTRL_FLAG|'c'):
					case (KB_CTRL_FLAG|'d'):
					case (KB_CTRL_FLAG|'e'):
					case (KB_CTRL_FLAG|'f'):
					case (KB_CTRL_FLAG|'g'):
					case (KB_CTRL_FLAG|'h'):
					case (KB_CTRL_FLAG|'i'):
					case (KB_CTRL_FLAG|'j'):
					case (KB_CTRL_FLAG|'k'):
					case (KB_CTRL_FLAG|'l'):
					case (KB_CTRL_FLAG|'m'):
					case (KB_CTRL_FLAG|'n'):
					case (KB_CTRL_FLAG|'o'):
					case (KB_CTRL_FLAG|'p'):
					case (KB_CTRL_FLAG|'q'):
					case (KB_CTRL_FLAG|'r'):
					case (KB_CTRL_FLAG|'s'):
					case (KB_CTRL_FLAG|'t'):
					case (KB_CTRL_FLAG|'u'):
					case (KB_CTRL_FLAG|'v'):
					case (KB_CTRL_FLAG|'w'):
					case (KB_CTRL_FLAG|'x'):
					case (KB_CTRL_FLAG|'y'):
					case (KB_CTRL_FLAG|'z'):
						UartSendChar((c&0xff)-'a'+1);
						break;

					case 8:		//backspace
						UartSendChar('\x7f');
						break;
					case 13: 	//ENTER
						if(console_echo_en){
							_cons_char('\r');
							_cons_char('\n');
						}
						UartSendChar('\r');
						UartSendChar('\n');
						break;
					default:
						if(console_echo_en){
							_cons_char(c);
						}
						UartSendChar(c);
						break;
				}


			}

		}
	}

	MoveCursor(cx,cy);
}

void console_Init(u16 flags){
	 // setup our stdio stream
	stdout = &mystdout;
	_cons_flags=flags;
}

void console_Echo(bool enable){
	console_echo_en=enable;
}

void console_SetCursor(u8 x,u8 y){
	cx=x;
	cy=y;
}

void console_Clear(){
	ClearVram();
	cx=_CONS_X_ORIGIN;
	cy=_CONS_Y_ORIGIN;
	//SetVerticalScroll(0);
}


void _cons_char(unsigned char c){

	//u8 nx;

	switch(c){
		case 8:	//backspace
			if(cx >_CONS_X_ORIGIN) cx--;
			break;
		/*
		case 9:
			nx=(((cx-_CONS_X_ORIGIN)/8)*8)+8+_CONS_X_ORIGIN;
			if(nx>=SCREEN_TILES_H){
				nx=SCREEN_TILES_H-1;
				PrintHexByte(0,0,'<');
				PrintHexByte(0,0,nx);
				PrintHexByte(0,0,'>');
				while(1);
			}
			cx=nx;
			break;
			*/
		case 10:	//line feed (\n)
			if(cy==(SCREEN_TILES_V-1)){
				VerticalScrollUp(true);
			}else{
				cy++;
			}

			break;
		case 12:	//form feed (clear screen)
			console_Clear();
			break;
		case 13:	//carriage return (\r)
			cx=_CONS_X_ORIGIN;

			break;
		case 32 ... 129:
			if(cx<SCREEN_TILES_H){
				PrintChar(cx++,cy,c);
			}else if(DECAWM){
				if(cy==(SCREEN_TILES_V-1)){
					VerticalScrollUp(true);
				}else{
					cy++;
				}
				cx=_CONS_X_ORIGIN;
				PrintChar(cx++,cy,c);
			}
			break;

		default:
			if(console_debug_echo){
				if(cx>=(SCREEN_TILES_H-6)){
					if(cy==(SCREEN_TILES_V-1)){
						VerticalScrollUp(true);
					}else{
						cy++;
					}
					cx=_CONS_X_ORIGIN;
				}

				PrintChar(cx++,cy,'<');
				PrintHexByte(cx,cy,c);
				cx+=2;
				PrintChar(cx++,cy,'>');
			}

			break;
	}

}



//wait for and return next arriving char
char getChar(){
	while(wifi_UnreadCount()==0);
	return wifi_ReadChar();
}

//wait for the next n char, but discard them
void waitChar(u8 count){
	for(u8 i=0;i<count;i++){
		while(wifi_UnreadCount()==0);
		u8 c=wifi_ReadChar();
		if(console_debug_echo) _cons_char(c);
	}
}





void negotiate(){
	//wait for 2 more bytes for negotiation
	u8 operation=getChar();
	u8 option=getChar();
	if(operation==_TELNET_SB) waitChar(3);	//1,IAC,SE

	if(console_debug_echo)printf("IAC<FF><%X><%X>",operation,option);

	switch(option){
		case _TELNET_STATUS:
			if(operation==_TELNET_WILL){
				wifi_SendString_P(PSTR("\xff\xfe\x05")); //DON't
				return;
			}
			break;
/*
		case _TELNET_NEW_ENVIRON:
			if(operation==_TELNET_DO){
				wifi_SendString_P(PSTR("\xff\xfb\x27")); //WILL
				return;
			}else if(operation==_TELNET_SB){
				wifi_SendString_P(PSTR("\xff\xfa\x27"));
				wifi_SendChar(0); //SB response flag
				wifi_SendString_P(PSTR("\xff\xf0"));
				return;
			}
			break;
*/
		case _TELNET_NAWS:
			if(operation==_TELNET_DO){
				wifi_SendString_P(PSTR("\xff\xfb\x1f")); //WILL Negotiate About Window Size 0xfb=251, 0x1f=31
				wifi_SendString_P(PSTR("\xff\xfa\x1f"));
				wifi_SendChar(0);
				wifi_SendChar(SCREEN_TILES_H);
				wifi_SendChar(0);
				wifi_SendChar(SCREEN_TILES_V);
				wifi_SendString_P(PSTR("\xff\xf0"));
				return;
			}else if(operation==_TELNET_SB){
				wifi_SendString_P(PSTR("\xff\xfa\x1f"));
				wifi_SendChar(0); //SB response flag
				wifi_SendChar(0);
				wifi_SendChar(SCREEN_TILES_H);
				wifi_SendChar(0);
				wifi_SendChar(SCREEN_TILES_V);
				wifi_SendString_P(PSTR("\xff\xf0"));
				return;
			}
			break;

		case _TELNET_DTT:
			if(operation==_TELNET_DO){
				wifi_SendString_P(PSTR("\xff\xfb\x18")); //WILL
				return;
			}else if(operation==_TELNET_SB){
				wifi_SendString_P(PSTR("\x0ff\x0fa\x018"));
				wifi_SendChar(0); //SB response flag
				wifi_SendString_P(PSTR("VT100\x0ff\x0f0"));
				return;
			}
			break;
	}

	//unsupported options
	wifi_SendChar(0xff);
	if(operation==_TELNET_DO){
		wifi_SendChar(_TELNET_WONT);
	}else if(operation==_TELNET_WILL){
		wifi_SendChar(_TELNET_DO);
	}
	wifi_SendChar(option);

}

/*
void debug(){
	int params[2];
	escape_buf[0]=';';
	escape_buf[1]='8';
	escape_buf[2]='H';
	parseParams(params,'H');
	int x=params[0];
	int y=params[1];

	printf("%d,%d",x,y);
}
*/
/**
 * Parse ansi escape sequence parameters. Params are decimal numbers delimited by ;
 * and terminated by endMArker.Can have forms:
 * ESC[10;20f
 * ESC[;20f -> first param is zero
  */
void parseParams(int params[],char endMarker){
	char pbuf[7];
	char* ip=escape_buf;
	u8 param=0;
	pbuf[0]='0';

	while(true){
		char* op=pbuf+1;
		while(*ip!=';' && *ip!=endMarker)*op++=*ip++;
		*op=0; //terminate asciiz
		params[param++]=atoi(pbuf);
		if(*ip==endMarker) break;
		ip++;
	}

}

int parseSingleParam(){
	char pbuf[7];
	int i;
	pbuf[0]='0';
	for(i=0;i<esc_pos;i++)pbuf[i+1]=escape_buf[i];
	pbuf[i]=0;
	return atoi(pbuf);
}

void printEscape(){
	printf(" ESC[");
	for(int i=0;i<esc_pos;i++){
		putchar(escape_buf[i]);
	}
}

void console_Process(){
	u8* addr;
	while(wifi_UnreadCount()>0){
		u8 c=wifi_ReadChar();


		if (c == 27) {			//ESC
			esc_pos = 0;
			c = getChar();
			if (c == '[') {		//ANSI escape sequence
				do {
					c = getChar();

					//fill the buffer with the command data
					//does not include "ESC[" and the end command marker
					escape_buf[esc_pos++] = c;

				} while (!(c > 'A' && c < 'z'));  //receive until end marker

				escape_buf[esc_pos]=0; //end

				switch(c){
					case 'A':			//move up one line, stop at top of screen
						if(cy>_CONS_Y_ORIGIN) cy--;
						break;
					case 'B':			//move down one line, stop at bottom of screen
						if(cy<(SCREEN_TILES_V-1)) cy++;
						break;
					case 'C':			//move forward one position, stop at right edge of screen
						if(esc_pos==1){
							cx++;
						}else{
							//int v=parseSingleParam();
							//if(v!=16){
							//	cx=0,cy=0;
							//	printf("ESC[C=%d,%d,%d",escape_buf[0],escape_buf[1],parseSingleParam());
							//	while(1);
							//}
							cx+=parseSingleParam();
						}
						if(cx>=SCREEN_TILES_H)cx=(SCREEN_TILES_H-1);

						break;
					case 'D':			//move backwards one position, Same as BackSpace, stop at left edge of screen
						if(cx>_CONS_X_ORIGIN) cx--;
						break;

					case 'd': //Vertical Position Absolute
						break;
					case 'h':
						if(escape_buf[0]=='?'){
							if( escape_buf[1]=='2' && escape_buf[2]=='5'){	//Show cursor
								SetCursorVisible(true);
							}else if(escape_buf[1]=='7'){					//DECAWM - AutoWrap Mode, start newline after column 80
								DECAWM=true;
							}else if(escape_buf[1]=='1'){					//DECCKM—Cursor Keys Mode
								DECCKM=true;
							}
						}
						break;

					case 'H':					//### cursor position -- All cases covered ###
					case 'f':
						if(esc_pos==1){
							//no coordinates,move to origin
							cx=_CONS_X_ORIGIN;
							cy=_CONS_Y_ORIGIN;
						}else{

							int coord[2];
							parseParams(coord,escape_buf[esc_pos-1]);
							if(coord[0]==0){
								cy=_CONS_Y_ORIGIN;
							}else{
								cy=coord[0] -1 + _CONS_Y_ORIGIN;
							}
							if(coord[1]==0){
								cx=_CONS_X_ORIGIN;
							}else{
								cx=coord[1] -1 + _CONS_X_ORIGIN;
							}

							if(cy>=SCREEN_TILES_V)cy=SCREEN_TILES_V-1; //row
							if(cx>=SCREEN_TILES_H)cx=SCREEN_TILES_H-1; //column
						}
						break;

					case 'J':					//### Erase display ###
						//[0J     erase from current position to bottom of screen inclusive
						//[1J     erase from top of screen to current position inclusive
						//[2J     erase entire screen (without moving the cursor)
						ClearVram();
						break;

					case 'K':					//### Erase line
						addr=GetRowVramAddress(cy);

						if(esc_pos==1 || escape_buf[0]=='0') {
							//clear line from cursor to end of line (same as [K )
							for(u8 x=cx;x<SCREEN_TILES_H;x++) addr[x]=0;

						}else if(escape_buf[0]=='1'){
							//clear line beginning to cursor
							for(u8 x=0;x<cx;x++) addr[x]=0;

						}else if(escape_buf[0]=='2'){
							//clear entire line
							for(u8 x=0;x<SCREEN_TILES_H;x++) addr[x]=0;
						}
						break;

					case 'l':												//Reset mode
						if(escape_buf[0]=='?'){
							if(escape_buf[1]=='2' && escape_buf[2]=='5'){ 	//### hide cursor
								SetCursorVisible(false);
							}else if(escape_buf[1]=='7'){					//DECAWM - AutoWrap Mode, start newline after column 80
								DECAWM=false;
							}else if(escape_buf[1]=='1'){					//DECCKM—Cursor Keys Mode
								DECCKM=false;
							}
						}
						break;


					case 'm':					//Set graphics mode
						//[7m=reverse mode
						break;

					case 'P':					//Delete n Characters, from current position to end of field

						if(escape_buf[0]=='1' || esc_pos==1){
							if(cx>_CONS_X_ORIGIN){
								u8* addr=GetRowVramAddress(cy);\
								for(u8 x=cx;x<SCREEN_TILES_H;x++){
									addr[x]=addr[x+1];
								}
								addr[SCREEN_TILES_H-1]=' '-_FIRST_TILE_OFFSET;
							}
						}else{
							printEscape();
						}
						break;

					case 'r':	// top and bottom margins (scroll region on VT100)
						break;

					case 'S': //scroll up
						VerticalScrollUp(true);
						break;

					case 'T': //scroll down
						VerticalScrollDown(true);
						break;

					default:
						//dump for debugging
						printEscape();

						break;
				}

			//unsupported stuff
			}else if(c==']'){
				wifi_WaitForString_P(PSTR("\x07"),NULL);
			}else if(c=='(' || c==')'){
				getChar();
			}


		} else if (c >= 240) {
			if (c == 0xff) { //IAC - Interpret As Command
				negotiate();
			}else{
				printf("Unsupported command:<%d>\r\n", c);
				//249,251 -cavebbs.homeip.net
				//while(1);
			}


		}else if(c==0xe2){ //UTF-8 character detected
			u8 b1=getChar();
			u8 b2=getChar();

			if(b1==0x80 && b2==0xa2){	//utf8 e2 80 a2 - Bullet
				putchar(128);
			}else{
				putchar(129);			//unsupported utf-8 character 'square'
			}

		} else if(c!=0){
			putchar(c); //normal character
		}
	}


}
