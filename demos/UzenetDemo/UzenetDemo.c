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
#include <avr/pgmspace.h>
#include <uzebox.h>

#include "data/font6x8-full-mode9.inc"
#include "data/power-mode9.inc"
#include "data/patches.inc"
#include "data/stage4song.inc"

#define WIFI_OK 			0
#define WIFI_TIMEOUT 		1

extern 	u8 uart_tx_tail;
extern 	u8 uart_tx_head;
extern	u8 uart_tx_size;
extern  u8 uart_tx_count;



bool connOpen=false;

u16 wifi_timeout=5*60;



void wifi_SendStringP(const char* str){

	char c;
	while(str!=NULL){
		c=pgm_read_byte(str);
		if(c==0)break;				
		while(UartSendChar(c)==-1); //block if buffer full	
		debug_char(c);			
		str++;
	};	
}

u8 wifi_WaitForStringP(const char* str, char* rxbuf){
	u8 c;
	s16 r;
	const char* p=str;
	char* buf=rxbuf;
	ClearVsyncCounter();

	while(1){		

		r=UartReadChar();
		if(r!=-1){

			c=r&(0xff);			

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
	//while(true){
	//	while(UartUnreadCount()>0){
	//		debug_char(UartReadChar());
	//	}
	//}
	
	u16 joy;
	while(true){

		while(UartUnreadCount()>0){
			debug_char(UartReadChar());
		}

		joy=ReadJoypad(0);
		if(joy!=0){
			while(ReadJoypad(0)!=0);
			switch(joy){
				case BTN_Y:
				debug_char('*');
				debug_hex(uart_tx_tail);
				debug_hex(uart_tx_head);
				debug_char(0xa);
				break;
				
			}
		}
	
		
	}
	
}

void SendCommandAndWait(const char* strToSend, const char* strToWait){
	wifi_SendStringP(strToSend);
	if(wifi_WaitForStringP(strToWait, NULL)==WIFI_TIMEOUT){
		
		if(connOpen){
			wifi_SendStringP(PSTR("AT+CIPCLOSE=0\r\n"));
			wifi_WaitForStringP(PSTR("OK\r\n"), NULL);
			connOpen=false;
		}
	
		timeout();
	}
}

void SendDataAndWait(const char* strToSend, const char* strToWait){
	
	wifi_SendStringP(strToSend);
/*
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				debug_char('#');
				debug_hex(uart_tx_tail);
				debug_hex(uart_tx_head);
				debug_hex(uart_tx_size);
				debug_hex(uart_tx_count);
				debug_char(0xa);
	}
*/
	if(wifi_WaitForStringP(strToWait,NULL)==WIFI_TIMEOUT){
		
		if(connOpen){
			wifi_SendStringP(PSTR("AT+CIPCLOSE=0\r\n"));
			wifi_WaitForStringP(PSTR("OK\r\n"), NULL);
			connOpen=false;
		}
	
		timeout();
	}
}


int main(){
	u8 c;
	char rxbuf[25];

	SetTileTable(font6x8);

	InitMusicPlayer(patches);
	//StartSong(Level4Song);


	debug_str_p(PSTR("Uzenet Wi-Fi Tester (ESP8266)\r\n"));

	debug_str_p(PSTR("Initializing UART...\r\n"));

	//initialize UART0 
	UBRR0H=0;	
	/*
	http://wormfood.net/avrbaudcalc.php
	This is for single speed mode. Double the 
	values for UART double speed mode.
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
	//UBRR0L=46;	//38400 bauds	3840 bytes/s	64 bytes/field
	UBRR0L=60;		//57600 bauds	5760 bytes/s	96 bytes/field


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
	SendCommandAndWait(PSTR("ATE0\r\n"),PSTR("OK\r\n"));

	debug_str_p(PSTR("Get firmware version...\r\n"));
	SendCommandAndWait(PSTR("AT+GMR\r\n"),PSTR("OK\r\n"));

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
	SendCommandAndWait(PSTR("AT+CWJAP?\r\n"),PSTR("OK\r\n"));

	debug_str_p(PSTR("Set multiple connections mode...\r\n"));
	SendCommandAndWait(PSTR("AT+CIPMUX=1\r\n"),PSTR("OK\r\n"));
	
	debug_str_p(PSTR("Connect to web server...\r\n"));
	//SendAndWait(PSTR("AT+CIPSTART=0,\"TCP\",\"belogic.com\",80\r\n"),PSTR("OK\r\nLinked\r\n"));
	//SendAndWait(PSTR("AT+CIPSTART=0,\"TCP\",\"216.189.148.140\",50697\r\n"),PSTR("OK\r\nLinked\r\n"));
	SendCommandAndWait(PSTR("AT+CIPSTART=0,\"TCP\",\"uzebox.net\",50697\r\n"),PSTR("OK\r\nLinked\r\n"));
	connOpen=true;

	debug_str_p(PSTR("Send chat login request...\r\n"));
	//SendAndWait(PSTR("AT+CIPSEND=0,49\r\nGET /hello.txt HTTP/1.0\r\nHost: belogic.com:80\r\n\r\n"),PSTR("SEND OK\r\n"));
	
	SendDataAndWait(PSTR("AT+CIPSEND=0,7\r\nUZECHAT"),PSTR("ENTER USER NAME:\r\nOK\r\n"));

	debug_str_p(PSTR("Send username...\r\n"));
	
	SendDataAndWait(PSTR("AT+CIPSEND=0,10\r\nMASTER_UZE"),PSTR("OK\0\r\nOK\r\n"));
	
	//wifi_WaitForStringP(PSTR, NULL);
	
	SendDataAndWait(PSTR("AT+CIPSEND=0,16\r\nOk, no problems!"),PSTR("OK\r\n"));
	
	//wifi_SendStringP(PSTR("MASTER_UZE\r\n"));

	//SendAndWait(PSTR("AT+CIPCLOSE=0\r\n"),PSTR("OK\r\n"));

	u16 joy;
	while(true){

		while(UartUnreadCount()>0){
			c=UartReadChar();
			debug_char(c);
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

	debug_char('*');
	debug_hex(uart_tx_tail);
	debug_hex(uart_tx_head);
	debug_char(0xa);
				break;
				
			}
		}
	
		
	}



} 
