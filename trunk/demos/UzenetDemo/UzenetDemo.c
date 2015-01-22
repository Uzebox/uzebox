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

#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

#include "data/font6x8-full-mode9.inc"
#include "data/power-mode9.inc"
#include "data/patches.inc"
#include "data/stage4song.inc"

#define WIFI_OK 			0
#define WIFI_TIMEOUT 		1


u16 wifi_timeout=5*60;

void wifi_SendChar( char data )
{
	debug_char(data);

	// Wait for empty transmit buffer
	while ( !( UCSR0A & (1<<UDRE0)) );

	// Put data into buffer, sends the data
	UDR0 = data;

}


void wifi_SendStringP(const char* str){
	char c;
	while(str!=NULL){
		c=pgm_read_byte(str);
		if(c==0)break;		
		wifi_SendChar(c);		
		str++;
	};	
}

u8 wifi_WaitForStringP(const char* str, char* rxbuf){
	u8 c;
	const char* p=str;
	char* buf=rxbuf;
	ClearVsyncCounter();

	while(1){		

		if(UartUnreadCount()>0){
			c=UartReadChar();			
			debug_char(c);	

			if(buf!=NULL){				
				*buf=c;
				buf++;
			}

			if(c==pgm_read_byte(p)){
				p++;
				if(pgm_read_byte(p)==0){
					return WIFI_OK;				
				}
			}else{
				//reset string compare
				p=str;
			}			
		}
			
		if(GetVsyncCounter()>wifi_timeout){
			return WIFI_TIMEOUT;
		}
	}

}


void timeout(){
	debug_str_p(PSTR("Timeout!\r\n"));	
	while(true){
		while(UartUnreadCount()>0){
			debug_char(UartReadChar());
		}
	}
}

void SendAndWait(const char* strToSend, const char* strToWait){
	wifi_SendStringP(strToSend);
	if(wifi_WaitForStringP(strToWait, NULL)==WIFI_TIMEOUT){
		timeout();
	}
}

int main(){
	u8 c;
	char rxbuf[25];

	SetTileTable(font6x8);

	InitMusicPlayer(patches);
	StartSong(Level4Song);


	debug_str_p(PSTR("Uzenet Wi-Fi Tester (ESP8266)\r\n"));

	debug_str_p(PSTR("Initializing UART...\r\n"));
	//initialize UART0 
	UBRR0H=0;	
	/*
	http://wormfood.net/avrbaudcalc.php
	Baud  UBRR0L	Error%
	9600	185		0.2
	14400	123		0.2
	19200	92		0.2
	28800	61		0.2
	38400	46		0.8
	57600	30		0.2
	76800	22		1.3
	115200	15		3.0
	*/

	//UBRR0L=185;	//9600 bauds	960 bytes/s		16 bytes/field
	//UBRR0L=92;	//19200 bauds	1920 bytes/s	32 bytes/field
	UBRR0L=92;	//38400 bauds	3840 bytes/s	64 bytes/field
	//UBRR0L=30;		//57600 bauds	5760 bytes/s	96 bytes/field


	UCSR0A=(1<<U2X0); // double speed mode
	UCSR0C=(1<<UCSZ01)+(1<<UCSZ00)+(0<<USBS0); //8-bit frame, no parity, 1 stop bit
	UCSR0B=(1<<RXEN0)+(1<<TXEN0); //Enable UART TX & RX


	debug_str_p(PSTR("Resetting module...\r\n"));
	//reset module
	DDRD|=(1<<PD3);
	PORTD&=~(1<<PD3);
	WaitVsync(1);
	PORTD|=(1<<PD3);

	if(wifi_WaitForStringP(PSTR("ready\r\n"),NULL)!=WIFI_OK)timeout();
	debug_str_p(PSTR("Module initialized succesfully.\r\n"));
	
	debug_str_p(PSTR("Disable echo...\r\n"));
	SendAndWait(PSTR("ATE0\r\n"),PSTR("OK\r\n"));

	debug_str_p(PSTR("Get firmware version...\r\n"));
	SendAndWait(PSTR("AT+GMR\r\n"),PSTR("OK\r\n"));

	debug_str_p(PSTR("Waiting for connection to router...\r\n"));
	for(u8 i=0;i<10;i++){
		wifi_SendStringP(PSTR("AT+CIFSR\r\n"));
		if(wifi_WaitForStringP(PSTR("OK\r\n"),rxbuf)!=WIFI_OK)timeout();
		if(!strstr_P(rxbuf,PSTR("0.0.0.0"))){
			break;
		}else{
			WaitVsync(60);
		}
	}

	debug_str_p(PSTR("Get access point...\r\n"));
	SendAndWait(PSTR("AT+CWJAP?\r\n"),PSTR("OK\r\n"));

	debug_str_p(PSTR("Set multiple connections mode...\r\n"));
	SendAndWait(PSTR("AT+CIPMUX=1\r\n"),PSTR("OK\r\n"));
	
	debug_str_p(PSTR("Connect to web server...\r\n"));
	SendAndWait(PSTR("AT+CIPSTART=0,\"TCP\",\"belogic.com\",80\r\n"),PSTR("OK\r\nLinked\r\n"));

	debug_str_p(PSTR("Send request...\r\n"));
	SendAndWait(PSTR("AT+CIPSEND=0,49\r\nGET /hello.txt HTTP/1.0\r\nHost: belogic.com:80\r\n\r\n"),PSTR("SEND OK\r\n"));

	while(true){

		while(UartUnreadCount()>0){
			c=UartReadChar();
			debug_char(c);
		}
	}



} 
