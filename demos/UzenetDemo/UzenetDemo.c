/*
 *  Uzebox quick and dirty tutorial
 *  Copyright (C) 2008  Alec Bourque
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

//Latest Official Espressif AT firmware binaries
//https://www.espressif.com/en/products/hardware/esp8266ex/resources

#include <stdbool.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <time.h>
#include <uzebox.h>
#include "keyboard.h"
#include "console.h"
#include "uzenet.h"
#include <videoMode9/videoMode9.h>

#include "data/font6x8-full.inc"

void vsyncCallback(){
	wifi_Tick();
	ConsoleHandler();
}

void wifiCallback(s8 status){
	printf_P(PSTR("wifi status: "));
	switch(status){
		case WIFI_STAT_READY:
			printf_P(PSTR("ready"));
			break;
		case WIFI_STAT_CONNECTED:
			printf_P(PSTR("connected"));
			break;
		case WIFI_STAT_IP:
			printf_P(PSTR("has ip"));
			break;
		default:
			printf("%i",status);
			break;
	}

	printf_P(PSTR("\r\n"));
}




int main(){

	SetTileTable(font);
	SetUserPreVsyncCallback(&vsyncCallback);
	SetCursorParams(127-32,30);
	SetCursorVisible(true);
	SetForegroundColor(CONS_DEFAULT_COLOR);
	console_Init(CONS_OUTPUT_NON_ALPHA);

	for(int i=0;i<SCREEN_TILES_V;i++)backgroundColor[i]=10;

	printf_P(PSTR("                    *** Uzebox Telnet Console 1.1 ***\r\n\r\n"));

	printf_P(PSTR("Init module and connecting to AP...\r\n"));

	wifi_Echo(true);


	if(wifi_Init(&wifiCallback)!=WIFI_OK){
		printf_P(PSTR("Init Error! Run the Uzenet configuration program\r\nto diagnose and/or configure the wifi module.\r\n"));
		while(1);
	}
	printf_P(PSTR("Init done.\r\n"));


	printf_P(PSTR("Connecting to telnet site...\r\n"));
	//char host[]="adminpc"; u16 port=23;
	//char host[]="newadminpc"; u16 port=23;
	//char host[]="newadminpc"; u16 port=25;
	char host[]="telehack.com"; u16 port=23;
	//char host[]="asciiattic.com"; u16 port=23;
	//char host[]="bbs.wccastle.net"; u16 port=2424;
	//char host[]="absinthebbs.net"; u16 port=1940;
	//char host[]="cavebbs.homeip.net"; u16 port=23;

	if(wifi_TcpConnect(host,port,true)!=WIFI_OK){
		printf_P(PSTR("TCP connection failed!\r\n"));
		while(1);
	}

	printf_P(PSTR("Connected!\r\n"));

	//console_Clear();
	console_Echo(false);
	wifi_Echo(false);

	//for(int i=0;i<120;i++){
	//	WaitVsync(1);
	//	console_Process();
	//}


	//wifi_SendString_P(PSTR("starwars\r\n"));

	while(true){
		console_Process();

	//	int j=ReadJoypad(0);
	//	if(j!=0){
		//	wifi_SendString_P(PSTR("starwars\r\n"));
		//}
	}

}
