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
#include "uzenet.h"
#include <keyboard.h>
#include <time.h>
#include <petitfatfs/pff.h>
#include <petitfatfs/diskio.h>
#include "terminal.h"

#define SSID 0
#define PWD 1
#define APPLY 2
#define EXIT 3
#define FIELDS_COUNT 4

#define UZENET_OK 	0
#define UZENET_NOCHANGE	1
#define UZENET_ERR_FILENOTFOUND  -1
#define UZENET_ERR_BADFILEFORMAT -2
#define UZENET_ERR -3

typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t lenght;
	uint8_t type;
	uint8_t pos;
	char* text;
} field;

char ssid[33]="\0";
char password[65]="\0";

extern FATFS fs;          // Work area (file system object) for the volume
extern BYTE dmabuf[512];  // Buffer for file sector read
extern WORD br;           // File read count
extern FRESULT res;       // Petit FatFs function common result code

void uzenetsetup_wifiCallback(s8 status){
}

/**
 * Reprograms the setting in the ESP8266 flash by performing the following steps:
 *
 * 1.Restore factory setting
 * 2.Define the UART speed to 9600,N,8,1
 * 3.Define to station mode (i.e. as wifi client)
 * 4.Define wifi credential and auto-reconnected to access point
 *
 */
int configure(char* ssid,char* password){
	terminal_Clear();
	wifi_SetTimeout(60*15);
	wifi_Echo(true);

	printf_P(PSTR("\x1b[0mConfiguring Uzenet module...\r\n"));

	printf_P(PSTR("Restoring the factory default settings: "));
	wifi_SendString_P(PSTR("AT+RESTORE\r\n"));
	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK){
		printf_P(PSTR("Error restoring ESP8266.\r\n"));
		return UZENET_ERR;
	}

	if(wifi_WaitForString_P(PSTR("ready\r\n"),NULL)!=WIFI_OK){
		printf_P(PSTR("Error restoring ESP8266.\r\n"));
		return UZENET_ERR;
	}
	printf_P(PSTR("Restore complete.\r\n"));


	printf_P(PSTR("Setting BAUD speed: "));
	wifi_Echo(false);
	wifi_SendString_P(PSTR("AT+UART_DEF=9600,8,1,0,0\r\n"));
	wifi_Echo(true);
	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK){
		printf_P(PSTR("Error programming the baud frequency.\r\n"));
		return UZENET_ERR;
	}
	WaitVsync(2); //IMPORTANT: Don't remove, this delay is required for the 8266 to internally switch speed.

	wifi_SetUartSpeed(UART_9600_BAUD);
	printf_P(PSTR("BAUD speed set.\r\n"));


	printf_P(PSTR("Setting station mode: "));
	wifi_Echo(false);
	wifi_SendString_P(PSTR("AT+CWMODE_DEF=1\r\n"));
	wifi_Echo(true);
	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK){
		printf_P(PSTR("Error communicating with the ESP8266.\r\n"));
		return UZENET_ERR;
	}
	printf_P(PSTR("Station mode set.\r\n"));

	printf_P(PSTR("Setting network credential: "));

	wifi_Echo(false);
	wifi_SendString_P(PSTR("AT+CWJAP_DEF=\""));
	wifi_SendString(ssid);
	wifi_SendString_P(PSTR("\",\""));
	wifi_SendString(password);
	wifi_SendString_P(PSTR("\"\r\n"));
	wifi_Echo(true);
	if(wifi_WaitForString_P(PSTR("WIFI GOT IP\r\n"),NULL)==WIFI_ERR_TIMEOUT){
		printf_P(PSTR("Error connecting to access point.\r\n"));
		return UZENET_ERR;
	}

	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK){
		printf_P(PSTR("ESP8266 error.\r\n"));
		return UZENET_ERR;
	}

	wifi_Echo(false);
	wifi_SendString_P(PSTR("AT+CWJAP_CUR?\r\n"));
	wifi_Echo(true);
	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK){
		printf_P(PSTR("Error obtaining access point details.\r\n"));
		return UZENET_ERR;
	}

	printf_P(PSTR("Connected to access point.\r\n"));


	return WIFI_OK;
}

/**
 * Loads the wifi credentials from the uzenet.cfg file (if present)
 */
int load_Config_File(){
	//initialize the SD card
    res=pf_mount(&fs);
    if(res){
    	WaitVsync(30);
    	res=pf_mount(&fs);
    	if(res){
    	   	printf_P(PSTR("\r\nSD card mount failed. "));
    	   	return -1;
    	}
    }

	//check if a drive config file exists and load the image files to use
	res=pf_open("UZENET.CFG");
	if(res==FR_OK){
		res=pf_read(dmabuf, 512, &br);
		if(res==FR_OK){
			//clear fields
			for(int i=0;i<sizeof(ssid);i++) ssid[i]=0;
			for(int i=0;i<sizeof(password);i++) password[i]=0;

			//load 2 first lines as ssid and password
			char* line = strtok((char*)dmabuf, "\r\n");
			if(line==NULL)return UZENET_ERR_BADFILEFORMAT;
			strcpy(ssid,line);
			line = strtok(NULL, "\r\n");
			if(line==NULL)return UZENET_ERR_BADFILEFORMAT;
			strcpy(password,line);
		}
	}else{
		printf_P(PSTR("\r\nCan't find UZENET.CFG file.\r\n"));
		return UZENET_ERR_FILENOTFOUND;
	}
	return UZENET_OK;
}

/**
 *
 */
void quit(int status){
	printf_P(PSTR("\x1b[0m")); //insure inverse video is off

	if(status!=UZENET_NOCHANGE){
		if(status==WIFI_OK){
			printf_P(PSTR("\r\nUzenet configured succesfully. "));
		}else{
			printf_P(PSTR("\r\nUzenet configuration failed. "));
		}

		printf_P(PSTR("Press any key or joypad button to exit."));
		while(!terminal_HasChar() && ReadJoypad(0)==0);
		if(terminal_HasChar()) terminal_GetChar(); //purge keyboard buffer
	}

	wifi_Disable();
	terminal_Clear();

}

void uzenet_Setup(){
	terminal_Clear();

	printf_P(PSTR("\x1b[7m Uzenet Setup v1.1 \x1b[0m\r\n\r\n"));

	//initialize module
	wifi_Echo(false);
	wifi_SetTimeout(60*5);
	printf_P(PSTR("Initializing UART and ESP8266...\r\n\r\n"));
	int status= wifi_Init(UART_115200_BAUD,&uzenetsetup_wifiCallback,false); //initialize to default speed for the esp8266
	if(status!=WIFI_OK){
		quit(status);
		return;
	}



	printf_P(PSTR("Current UART speed: "));
	wifi_SendString_P(PSTR("AT+UART_CUR?\r\n"));
	wifi_Echo(true);
	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK){
		printf_P(PSTR("Error obtaining UART speed.\r\n"));
		quit(WIFI_ERR);
		return;
	}

	printf_P(PSTR("\r\nAT Firmware version: "));
	wifi_SendString_P(PSTR("AT+GMR\r\n"));
	wifi_Echo(true);
	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK){
		printf_P(PSTR("Error obtaining firware version.\r\n"));
		quit(WIFI_ERR);
		return;
	}

	printf_P(PSTR("\r\nESP8266 Module Initialized successfully.\r\n\r\nPress any key or Joypad button...\r\n\r\n"));
	while(!terminal_HasChar() && ReadJoypad(0)==0);
	while(terminal_HasChar()) terminal_GetChar();

	u16 key;
	u8 id=0,len;
	bool loadedFromFile=false;

	//try first loading from file on sd card
	if(load_Config_File()==UZENET_OK) loadedFromFile=true;

	field fields[4]={
			{8,5,32,0,0,ssid},
			{8,7,64,0,0,password},
			{4,9,0,1,0,NULL},
			{14,9,0,1,0,NULL}
	};

	terminal_Clear();
	printf_P(PSTR("\x1b[7m Uzenet Setup v1.1 - ESP8266 Initialized \x1b[0m\r\n\r\n"));
	if(loadedFromFile){
		printf_P(PSTR(" Wifi credentials loaded from UZENET.CFG file. Press Enter on the\r\n keyboard or the Start button on the P1 joypad."));
	}else{
		printf_P(PSTR(" Enter crentials. To auto load, create UZENET.CFG on SD Card and put the\r\n ssid and password on 2 two separate lines ended with a CrLf (\\r\\n)."));
	}
	printf_P(PSTR("\r\n\r\n SSID:\x1b[7m                                \x1b[0m\r\n\r\n Pass:\x1b[7m                                                                \x1b[0m\r\n\r\n [ Apply ] [ Exit ]"));
	printf_P(PSTR("\x1b[7m")); //starts inverse video

	//Write ssid/password in fields
	for(uint8_t i=0;i<FIELDS_COUNT;i++){
		if(fields[i].type==0){
			terminal_MoveCursor(fields[i].x,fields[i].y);
			printf(fields[i].text);
		}
	}

	if (loadedFromFile) id=2; //move cursor to Apply button
	terminal_MoveCursor(fields[id].x+strlen(fields[id].text),fields[id].y);

	while(1){
		if(terminal_HasChar()){
			key=terminal_GetChar(true);
			len=strlen(fields[id].text);

			//escape sequences
			if(key==KB_ESC){
				terminal_GetChar(); // ignore second character of escape sequence O or [
				key=terminal_GetChar(); //actual key
				switch(key){
					case 'B':	//down
					case 'C':	//right
						id++;
						if(id==FIELDS_COUNT) id=0;
						break;
					case 'A':	//up
					case 'D':	//left
						if(id==0) id=FIELDS_COUNT;
						id--;
						break;
				}
			}else{
				//normal keys
				switch(key){
					case KB_BCKSP:	//backspace
						if(fields[id].type==0 && len>0){
							fields[id].pos--;
							fields[id].text[len-1]=0;
							terminal_MoveCursor(fields[id].x+len-1,fields[id].y);
							putchar(' ');

						}
						break;
					case KB_TAB:
						id++;
						if(id==FIELDS_COUNT) id=0;
						break;

					case 32 ... 127:
						if(fields[id].type==0 && len<fields[id].lenght){
							fields[id].text[len]=(key&0xff);
							putchar(key);
						}
						break;

					case KB_ENTER:
						if(fields[id].type==1){
							if(id==APPLY){
								int status=configure(fields[SSID].text,fields[PWD].text);
								quit(status);
							}else{
								quit(UZENET_NOCHANGE);
							}
							return;
						}

						break;
				}
			}

			terminal_MoveCursor(fields[id].x+strlen(fields[id].text),fields[id].y);
		}
		if(ReadJoypad(0)!=0){
			while(ReadJoypad(0)!=0){
				int status=configure(fields[SSID].text,fields[PWD].text);
				quit(status);
				return;
			}
		}
	}

}

