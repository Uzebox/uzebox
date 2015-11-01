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

#include "data/font6x8-full-mode9.inc"
#include "data/power-mode9.inc"
#include "data/patches.inc"
#include "data/stage4song.inc"


HttpResponse resp;


void timeout(){
	printf_P(PSTR("Timeout!\r\n"));

	u16 joy;
	while(true){

		while(UartUnreadCount()>0){
			putchar(UartReadChar());
		}

		joy=ReadJoypad(0);
		if(joy!=0){
			while(ReadJoypad(0)!=0);
			switch(joy){
				case BTN_Y:
				putchar('*');
				//debug_hex(uart_tx_tail);
				//debug_hex(uart_tx_head);
				putchar(0xa);
				break;

			}
		}
	}
}
int main(){
	u8 c=0;
	char rxbuf[256];
	resp.content=rxbuf;


	SetCursorParams('z'-32+2,30);
	MoveCursor(0,0);
	SetCursorVisible(true);

	SetUserPreVsyncCallback(&ConsoleHandler);

	SetTileTable(font6x8);
	console_init(CONS_OUTPUT_NON_ALPHA);

	wifi_Echo(false);

	printf_P(PSTR("             *** Uzenet Console 0.1 ***\n\n"));
	printf_P(PSTR("For console commands type: help\n\n"));


	printf_P(PSTR("Initializing UART...\r\n"));

	//initialize UART0 
	UBRR0H=0;	

	//http://wormfood.net/avrbaudcalc.php
	//This is for single speed mode. Double the
	//values for UART double speed mode.
	//Baud  UBRR0L	Error%
	//9600	185		0.2
	//14400	123		0.2
	//19200	92		0.2
	//28800	61		0.2
	//38400	46		0.8
	//57600	30		0.2
	//76800	22		1.3
	//115200	15		3.0


	//UBRR0L=185;	//9600 bauds	960 bytes/s		16 bytes/field
	//UBRR0L=92;	//19200 bauds	1920 bytes/s	32 bytes/field
	//UBRR0L=46;	//38400 bauds	3840 bytes/s	64 bytes/field
	UBRR0L=60;		//57600 bauds	5760 bytes/s	96 bytes/field
	//UBRR0L=30;		//115200 bauds	


	UCSR0A=(1<<U2X0); // double speed mode
	UCSR0C=(1<<UCSZ01)+(1<<UCSZ00)+(0<<USBS0); //8-bit frame, no parity, 1 stop bit
	UCSR0B=(1<<RXEN0)+(1<<TXEN0); //Enable UART TX & RX


	printf_P(PSTR("Resetting module...\r\n"));
	wifi_HWReset();

	if(wifi_WaitForStringP(PSTR("ready\r\n"),NULL)!=WIFI_OK)timeout();

	SendCommandAndWait(PSTR("ATE0\r\n"),PSTR("OK\r\n"));
	SendCommandAndWait(PSTR("AT+CWMODE_CUR=1\r\n"),PSTR("OK\r\n"));

	printf_P(PSTR("Uzenet initialized succesfully.\n"));
	SendCommandAndWait(PSTR("AT+GMR\r\n"),PSTR("OK\r\n"));

	printf_P(PSTR("Waiting for connection to router...\r\n"));

	if(WaitForString_P(PSTR("WIFI GOT IP\r\n"))!=WIFI_OK) timeout();
	printf_P(PSTR("Connected to router.\n"),resp.content);

	//wifi_SendString_P(PSTR("AT+CIFSR\r\n"));

	SendCommandAndWait(PSTR("AT+CIPMUX=1\r\n"),PSTR("OK\r\n"));
	printf_P(PSTR("\nConnecting to uzebox.org...\n"));

	time_t timestamp;
	time_t now;
	struct tm *uzetime;
	char* ds;
	u16 cnt=0;

	HttpGet("uzebox.org",80,"/uzenet/time.php",&resp);
	timestamp=atol(resp.content)-UNIX_OFFSET;
	set_system_time(timestamp);
	set_zone(-4 * ONE_HOUR);

//	uzetime=localtime(&timestamp);
//	ds=isotime(uzetime);
//	printf_P(PSTR("Initial Uzenet server time: %s\n"),ds);

	console_clear();
	wifi_Echo(true);

	while(1){
		HttpGet("uzebox.org",80,"/uzenet/time.php",&resp);
		timestamp=atol(resp.content)-UNIX_OFFSET;
		uzetime=localtime(&timestamp);
		ds=isotime(uzetime);

		int localDiff=time(NULL)-timestamp;

		printf_P(PSTR("Uzenet: %s\n"),ds);

		while(1){

			now=time(NULL);
			uzetime=localtime(&now);
			ds=isotime(uzetime);

			if(localDiff<0){
				printf_P(PSTR("Local : %s, slower by: %i\r"),ds,localDiff);
			}else{
				printf_P(PSTR("Local : %s, faster by: %i\r"),ds,localDiff);
			}

			WaitVsync(60);

			cnt++;
			if(cnt>=(30)){
				cnt=0;
				printf_P(PSTR("\n"),ds);
				break;
			}


		}
	}



//	SendCommandAndWait(PSTR("AT+CIPSTART=0,\"TCP\",\"uzebox.org\",80\r\n"),PSTR("OK\r\n"));
//	SendCommandAndWait(PSTR("AT+CIPSENDEX=0,256\r\n\f"),PSTR(">"));
//	SendCommandAndWait(PSTR("GET /uzenet/time.php HTTP/1.0\r\nHost: uzebox.org:80\r\n\r\n\f"),PSTR("SEND OK\r\n"));


//	printf_P(PSTR("Waiting for connection to router...\r\n"));
//	SendCommandAndWait(PSTR("AT+CWJAP=\"Belogic2G\",\"sie6bhUjae7nnnywikH72M9ejhsgemslle84Dcs\"\r\n"),PSTR("OK\r\n"));

/*
	//SendCommandAndWait(PSTR("AT+UART_DEF=57600,8,1,0,0\r\n"),PSTR("OK\r\n"));
	

	
	printf_P(PSTR("Waiting for connection to router...\r\n"));
	for(u8 i=0;i<10;i++){
		wifi_SendStringP(PSTR("AT+CIFSR\r\n"));
		if(wifi_WaitForStringP(PSTR("OK\r\n"),rxbuf)!=WIFI_OK)timeout();
		if(!strstr_P(rxbuf,PSTR("0.0.0.0"))){
			break;
		}else{
			WaitVsync(60);
		}
	}

	printf_P(PSTR("Get access point...\r\n"));
	SendCommandAndWait(PSTR("AT+CWJAP?\r\n"),PSTR("OK\r\n"));

	printf_P(PSTR("Set multiple connections mode...\r\n"));
	SendCommandAndWait(PSTR("AT+CIPMUX=1\r\n"),PSTR("OK\r\n"));
	
	printf_P(PSTR("Connect to web server...\r\n"));
	//SendAndWait(PSTR("AT+CIPSTART=0,\"TCP\",\"belogic.com\",80\r\n"),PSTR("OK\r\nLinked\r\n"));
	//SendAndWait(PSTR("AT+CIPSTART=0,\"TCP\",\"216.189.148.140\",50697\r\n"),PSTR("OK\r\nLinked\r\n"));
	SendCommandAndWait(PSTR("AT+CIPSTART=0,\"TCP\",\"uzebox.net\",50697\r\n"),PSTR("OK\r\n"));
	connOpen=true;

	printf_P(PSTR("Send chat login request...\r\n"));
	//SendAndWait(PSTR("AT+CIPSEND=0,49\r\nGET /hello.txt HTTP/1.0\r\nHost: belogic.com:80\r\n\r\n"),PSTR("SEND OK\r\n"));
	
	//SendDataAndWait(PSTR("AT+CIPSEND=0,7\r\nUZECHAT"),PSTR("ENTER USER NAME:\r\nOK\r\n"));
	SendDataAndWait(PSTR("AT+CIPSEND=0,7\r\nUZECHAT"),PSTR("ENTER USER NAME:"));

	printf_P(PSTR("Send username...\r\n"));
	
	SendDataAndWait(PSTR("AT+CIPSEND=0,10\r\nMASTER_UZE"),PSTR("OK\0\r\nOK\r\n"));
	
	//wifi_WaitForStringP(PSTR, NULL);
	
	SendDataAndWait(PSTR("AT+CIPSEND=0,16\r\nOk, no problems!"),PSTR("OK\r\n"));
	
	//wifi_SendStringP(PSTR("MASTER_UZE\r\n"));

	//SendAndWait(PSTR("AT+CIPCLOSE=0\r\n"),PSTR("OK\r\n"));
*/
	u16 joy;
	while(true){

		while(UartUnreadCount()>0){
			c=UartReadChar();
			putchar(c);
		}
		
		joy=ReadJoypad(0);
		if(joy!=0){
			while(ReadJoypad(0)!=0);
			switch(joy){
				case BTN_A:
					//SendDataAndWait(PSTR("AT+CIPSEND=0,16\r\nOk, no problems!"),PSTR("OK\r\n"));
				break;
				
				case BTN_B:
					SendDataAndWait(PSTR("AT+CIPSEND=0,13\r\nAnybody here?"),PSTR("OK\r\n"));
				break;
				
				case BTN_X:
					//SendDataAndWait(PSTR("AT+CIPSEND=0,13\r\nUzebox rules!"),PSTR("OK\r\n"));
				break;
				
				case BTN_Y:
					//SendDataAndWait(PSTR("AT+CIPSEND=0,3\r\nYo!"),PSTR("OK\r\n"));

	//debug_char('*');
	//debug_hex(uart_tx_tail);
	//debug_hex(uart_tx_head);
	//debug_char(0xa);
				break;
				
			}
		}
	
		
	}



} 
