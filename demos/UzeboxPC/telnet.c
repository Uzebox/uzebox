/*
 * Implements a simple telnet client.
 * This is a native application (one compiled for the AVR)
 * and started from the CP/M shell via a simple laucher app.
 *
 * Launcher source: /data/cpmsrc/telnet.asm
 *
 * Protocol information:
 *		http://pcmicro.com/netfoss/telnet.html
 *
 *	Tools to troubleshoot the protocol:
 *    Windump: A command line utility for Windows to capture TCP/IP packets.
 *             Ex.: Execute "windump -w out.pcap -s 0 host telehack.com" at the
					command shell and then open a telnet client on you machine and
					connect to telehack.com:23. This will start the packets capture
					and log then to file out.pcap.
 *	  Wireshark : A windows protocol analysis tool of the captured packets (.pcap files)
 *				  where you can isolate the Telnet packets form the rest.
 *
 *  Created on: 30 janv. 2020
 */
#include <stdbool.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <time.h>
#include <uzebox.h>
#include "uzenet.h"
#include "terminal.h"
#include "telnet.h"
#include "keyboard.h"

//The debug feature allows to simulate incoming Telnet data
#define DEBUG 0
//#include "data/debugdata.inc"

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

#define UPPERCASE_MASK 0b11011111

//#define _TELNET_STATUS_DISCONNECTED 0
//#define _TELNET_STATUS_CONNECTING	1
//#define _TELNET_STATUS_CONNECTED	2

bool debugEnabled=false;
u8 ch;
u8 telnet_cmd_byte=0;					//The number of Telnet commands bytes received so far
u8 telnet_option=0;						//The telnet command's option
u8 telnet_sb_cnt=0;						//The Telnet SB bytes to wait for
u8 telnet_cmd=0;						//The Telnet command number
bool connected=false;					//Indicate if we already initialized the wifi
bool progress_display=false;
u8 progress_counter=0;					//Used to display the progress bar when connecting

void telnet_VsyncCallback(){
	if(progress_display == true){
		if(progress_counter++==15){
			printf_P(PSTR("\xe2\x80\xa2")); //print dot
			progress_counter=0;
		}
	}
}

//void wifiCallback(s8 status){
//}

void telnet_CommandLine(char* cmdline){

	//extract host
	char* host = strtok(cmdline, ":");

	//extract port
	u16 port=23;
	if(host != NULL ) {
	      char* p = strtok(NULL, ":");
	      if(p!=NULL) port=atoi(p);
	}

	telnet(host,port);
}


void telnet(char* host, u16 port){

	progress_display=true;

	printf_P(PSTR("\r\nConnecting to Telnet site: %s, port %d\r\n"),host,port);

	//connect to wifi access point
	if(!connected){
		if(wifi_Init(UART_115200_BAUD,NULL,true) != WIFI_OK){
			printf_P(PSTR("\r\n\r\nNetwork initialization Error!\r\n\r\nRun UZECONF.COM to configure the Uzenet interface.\r\n"));
			progress_display=false;
			return;
		}
		connected=true;
	}

	//Open a TCP connection to telnet host
	if(wifi_TcpConnect(host,port,true)!=WIFI_OK){
		printf_P(PSTR("\r\nConnection failed! Host or port invalid?\r\n"));
		progress_display=false;
		return;
	}
	progress_display=false;

	terminal_Clear();
	terminal_Echo(false);
	terminal_SetCursorVisible(true);


	while(true){

		//handle telnet specific traffic
		if(wifi_UnreadCount()>0){
			ch=wifi_ReadChar();

			if(telnet_cmd_byte){				//handle telnet command bytes
				if(telnet_cmd==0){				//first byte is command
					if(ch>=251 && ch<=254){		//WILL, WONT, DO, WONT commands?
						telnet_cmd=ch;
					}else if(ch==_TELNET_SB){	//SB command
						telnet_cmd=ch;
						telnet_sb_cnt=4;		//bytes in this command
					}else{						//not a supported command so ignore. All commands before 250 have no options byte.
						telnet_cmd_byte=0;		//end of command
					}
					telnet_option=0;

				}else if(telnet_cmd==_TELNET_SB){
					if(telnet_sb_cnt==4){
						telnet_option=ch;
					}
					telnet_sb_cnt--;		//discard bytes 1,IAC,SE
					if(telnet_sb_cnt==0){

						switch(telnet_option){
							case _TELNET_NAWS:
								wifi_SendString_P(PSTR("\xff\xfa\x1f"));
								wifi_SendChar(0); //SB response flag
								wifi_SendChar(0);
								wifi_SendChar(SCREEN_TILES_H);
								wifi_SendChar(0);
								wifi_SendChar(SCREEN_TILES_V);
								wifi_SendString_P(PSTR("\xff\xf0"));
								break;

							case _TELNET_DTT:
								wifi_SendString_P(PSTR("\x0ff\x0fa\x018"));
								wifi_SendChar(0); //SB response flag
								wifi_SendString_P(PSTR("VT100\x0ff\x0f0"));
								//wifi_SendString_P(PSTR("ANSI\x0ff\x0f0"));
								break;

							default:
								//Generic handler for unsupported options
								break;
						}
						telnet_cmd_byte=0;		//end of command
					}

				}else{
					//Process telnet commands
					telnet_option=ch;

					switch(telnet_option){
						case _TELNET_STATUS:
							if(telnet_cmd==_TELNET_WILL){
								wifi_SendString_P(PSTR("\xff\xfe\x05")); //DON't
							}
							break;

						case _TELNET_NAWS:
							if(telnet_cmd==_TELNET_DO){
								wifi_SendString_P(PSTR("\xff\xfb\x1f")); //WILL Negotiate About Window Size 0xfb=251, 0x1f=31
								wifi_SendString_P(PSTR("\xff\xfa\x1f"));
								wifi_SendChar(0);
								wifi_SendChar(SCREEN_TILES_H);
								wifi_SendChar(0);
								wifi_SendChar(SCREEN_TILES_V);
								wifi_SendString_P(PSTR("\xff\xf0"));
							}
							break;

						case _TELNET_DTT:
							if(telnet_cmd==_TELNET_DO){
								wifi_SendString_P(PSTR("\xff\xfb\x18")); //WILL
							}
							break;

						default:
							//Generic handler for unsupported options
							wifi_SendChar(0xff);
							if(telnet_cmd==_TELNET_DO){
								wifi_SendChar(_TELNET_WONT);
							}else if(telnet_cmd==_TELNET_WILL){
								wifi_SendChar(_TELNET_DO);
							}
							wifi_SendChar(telnet_option);
							break;
					}
					telnet_cmd_byte=0;		//end of command
				}

				if(telnet_cmd_byte!=0) telnet_cmd_byte++;

			}else if (ch == 0xff) { 			//We received a Telnet IAC - Interpret As Command byte
				telnet_cmd_byte=1;				//first byte reveived
				telnet_cmd=0;					//clear command
				telnet_sb_cnt=0;				//clear sub negotiation byte count
			}else{
				putchar(ch);					//send char to terminal
			}
		}

		if(terminal_HasChar()){
			u8 c=terminal_GetChar();
			if(c==CTRL_X){
				printf_P(PSTR("\r\nDisconnecting from host...\r\n"));
				if(wifi_TcpDisconnect()!=WIFI_OK){
					printf_P(PSTR("TCP deconnection failed!\r\n"));
				}

				printf_P(ESC_CMD_TEXT_ATTR,0); //insure we end any inverse video
				terminal_SetCursorVisible(true); //insure teh cursor is visble
				terminal_Clear(); //clear the screen
				printf_P(PSTR("\r\nDisconnected from Telnet host.\r\n"));
				break;
			}
			wifi_SendChar(c);
		}


	}

}

