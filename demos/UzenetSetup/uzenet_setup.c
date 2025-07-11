/*
 *  Native application to setup the esp8266 module.
 *  A native application is one compiled for the AVR
 *  and started from the CP/M shell via a simple laucher app.
 *
 *  Laucher source: /data/cpmsrc/uzecon.asm
 *
 *  Created on: Oct 16, 2015
 *  Author: Uze
 *
 */
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <uzenet.h>
#include <time.h>
#include "terminal.h"


#define SSID 0
#define PWD 1
#define APPLY 2
#define EXIT 3
#define FIELDS_COUNT 4

typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t lenght;
	uint8_t type;
	uint8_t pos;
	char* text;
} field;

void uzenetsetup_wifiCallback(s8 status){
}


void configure(char* ssid,char* password){
	wifi_Echo(true);
	int resp;
	printf_P(PSTR("Configuring Uzenet module...\r\n"));

	resp=wifi_Init(&uzenetsetup_wifiCallback,false);
	switch(resp){
		case WIFI_ERR_IP:
			printf_P(PSTR("Cannot obtain IP. Configuring access point..."));
			break;
		case WIFI_OK:
			break;
		default:
			printf_P(PSTR("Error initializing the ESP8266: %d"),resp);
			break;
	}

	wifi_SendString_P(PSTR("AT+CWMODE_DEF=1\r\n"));
	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK){
		printf_P(PSTR("Error communicating with the ESP8266"));
	}

	wifi_SendString_P(PSTR("AT+CWJAP_DEF=\""));
	wifi_SendString(ssid);
	wifi_SendString_P(PSTR("\",\""));
	wifi_SendString(password);
	wifi_SendString_P(PSTR("\"\r\n"));

	if(wifi_WaitForString_P(PSTR("WIFI GOT IP\r\n"),NULL)==WIFI_ERR_TIMEOUT){
		printf_P(PSTR("ESP8266 error: Cannot connect to access point."));
	}

	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK){
		printf_P(PSTR("ESP8266 error"));
	}else{
		printf_P(PSTR("Uzenet configured succesfully. Press any key to exit."));
		//while(!KeyboardHasKey());
		while(!terminal_HasChar());

		printf_P(PSTR("\r\nDisabling module...\r\n"));
		wifi_Disable();
	}

}

/*
const char row1[] PROGMEM={"~!@#$%^&*()_+"};
const char row2[] PROGMEM={"`1234567890-="};
const char row3[] PROGMEM={"          {}|"};
const char row4[] PROGMEM={"QWERTYUIOP[]\\"};
const char row5[] PROGMEM={"         :\""};
const char row6[] PROGMEM={"ASDFGHJKL;'"};
const char row7[] PROGMEM={"       <>?"};
const char row8[] PROGMEM={"ZXCVBNM,./"};
PGM_P const rows[] PROGMEM ={row1,row2,row3,row4,row5,row6,row7,row8};
*/
const char row1[] PROGMEM={"~`!1@2#3$4%5^6&7*8(9)0_-+="};
const char row2[] PROGMEM={" Q W E R T Y U I O P{[}]|\\"};
const char row3[] PROGMEM={" A S D F G H J K L:;\"'"};
const char row4[] PROGMEM={" Z X C V B N M<,>.?/"};
PGM_P const rows[] PROGMEM ={row1,row2,row3,row4};

void uzenet_Setup(){



	terminal_Clear();
	printf_P(PSTR(" \x1b[7m Uzenet Setup v1.1 \x1b[0m\r\n\r\n WiFi network credentials\r\n\r\n SSID:\x1b[7m                                \x1b[0m\r\n\r\n Pass:\x1b[7m                                                                \x1b[0m\r\n\r\n [ Apply ] [ Exit ]"));

	PGM_P rowPtr;
	u8 c1,c2,kx=5,ky=10;
	for(u8 y=0;y<8;y++){
		rowPtr=(PGM_P)pgm_read_word(&(rows[y]));
		while(1){
			c1=pgm_read_byte(rowPtr++);
			if(c1==0)break;
			c2=pgm_read_byte(rowPtr++);

			terminal_MoveCursor(kx,ky);
			printf_P(PSTR(TB_CDR TB_HOR TB_HOR TB_CDL ));
		}
	}


	//draw the virtual keyboard

	printf_P(PSTR("\r\n\r\n\r\n"  "|Q|W|E|R|T|Y|U|I|O|P|[|]|"));



	printf_P(PSTR("\x1b[7m")); //starts inverse video

	u16 key;
	u8 id=0,len;
	char ssid[32]="\0";
	char password[64]="\0";

	field fields[4]={
			{8,4,32,0,0,ssid},
			{8,6,64,0,0,password},
			{4,8,0,1,0,NULL},
			{14,8,0,1,0,NULL}
	};

	for(uint8_t i=0;i<FIELDS_COUNT;i++){
		if(fields[i].type==0){
			terminal_MoveCursor(fields[i].x,fields[i].y);
			printf(fields[i].text);
		}
	}





	terminal_MoveCursor(fields[id].x+strlen(fields[id].text),fields[id].y);

	while(1){
		u16 c=ReadJoypad(0);




		if(terminal_HasChar()){
			key=terminal_GetChar(true);
			len=strlen(fields[id].text);
			switch(key){
				case 8:	//backspace
					if(fields[id].type==0 && len>0){
						fields[id].pos--;
						fields[id].text[len-1]=0;
						terminal_MoveCursor(fields[id].x+len-1,fields[id].y);
						putchar(' ');

					}
					break;
				case 9:	//tab
					id++;
					if(id==FIELDS_COUNT) id=0;
					break;

				case 32 ... 127:
					if(fields[id].type==0 && len<fields[id].lenght){
						fields[id].text[len]=(key&0xff);
						putchar(key);
					}
					break;

				case 13:	//enter
					if(fields[id].type==1){
						printf_P(PSTR("\x1b[0m\r\n\r\n")); //stops inverse video

						if(id==APPLY){
							configure(fields[SSID].text,fields[PWD].text);
						}

						terminal_Clear();
						return;
					}

					break;
			}

			terminal_MoveCursor(fields[id].x+strlen(fields[id].text),fields[id].y);
		}
	}

}
