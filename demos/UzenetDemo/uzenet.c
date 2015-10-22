/*
 * uzenet.c
 *
 *  Created on: Oct 16, 2015
 *      Author: admin
 */
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "uzenet.h"
#include <time.h>

bool connOpen=false;
bool echo=false;
u16 wifi_timeout=10*60;
int WaitforIPD();


void wifi_HWReset(){
	//reset module
	DDRD|=(1<<PD3);
	PORTD&=~(1<<PD3);
	WaitVsync(1);
	PORTD|=(1<<PD3);
}

void wifi_Echo(bool echoOn){
	echo=echoOn;
}

int wifi_SendString_P(const char* str){

	char c;
	while(str!=NULL){
		c=pgm_read_byte(str);
		if(c==0)break;
		if(c=='\f'){
			 //send '\'+'0' escape sequence to esp8266 (for AT+CIPSENDEX command)
			while(UartSendChar(0x5c)==-1); //block if buffer full
			while(UartSendChar(0x30)==-1); //block if buffer full
		}else{
			while(UartSendChar(c)==-1); //block if buffer full
			if(echo) putchar(c);
		}
		str++;
	};

	return WIFI_OK;
}

int wifi_SendString(char* str){

	char c;
	while(str!=NULL){
		c=*str;
		if(c==0)break;
		if(c=='\f'){
			 //send '\'+'0' escape sequence to esp8266 (for AT+CIPSENDEX command)
			while(UartSendChar(0x5c)==-1); //block if buffer full
			while(UartSendChar(0x30)==-1); //block if buffer full
		}else{
			while(UartSendChar(c)==-1); //block if buffer full
			if(echo) putchar(c);
		}
		str++;
	};

	return WIFI_OK;
}

int wifi_WaitForStringP(const char* str, char* rxbuf){
	u8 c;
	s16 r;
	const char* p=str;
	char* buf=rxbuf;
	ClearVsyncCounter();

	while(1){

		r=UartReadChar();
		if(r!=-1){

			c=r&(0xff);

			if(echo) putchar(c);

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

const char bodyMarker[] PROGMEM = "\r\n\r\n";
int ReceiveHtmlBody(char* rxbuf,int len){
	u8 c;
	s16 r,headerSize=0,bodySize=0;
	bool inBody=false;
	char* buf=rxbuf;
	const char* p=bodyMarker;

	ClearVsyncCounter();

	while(1){

		r=UartReadChar();
		if(r!=-1){

			c=r&(0xff);
			if(echo) putchar(c);

			if(inBody){
				*buf=c;
				buf++;
				bodySize++;
			}else{
				if(c==pgm_read_byte(p)){
					p++;
					if(pgm_read_byte(p)==0){
						inBody=true;
					}
				}else{
					//reset string compare
					p=bodyMarker;
				}
				headerSize++;
			}

		}

		if(GetVsyncCounter()>wifi_timeout){
			return WIFI_TIMEOUT;
		}

		if(headerSize+bodySize == len){
			break;
		}
	}

	*buf=0;
	return bodySize;
}

//wait for +IPD marker and return size of data
const char ipd[] PROGMEM = "+IPD,0,";
int WaitforIPD(){
	u8 c;
	s16 data;

	const char* p=ipd;
	char rxbuf[16];
	char* buf=rxbuf;

	ClearVsyncCounter();

	while(1){
		data=UartReadChar();
		if(data!=-1){

			c=data&(0xff);
			if(echo) putchar(c);

			if(c==pgm_read_byte(p)){
				p++;
				if(pgm_read_byte(p)==0){

					//now wait for the ":"character
					while(1){
						data=UartReadChar();
						if(data!=-1){
							c=data&(0xff);
							*buf=c;
							if(echo) putchar(c);
							if(c==':'){
								*buf=0;
								break;
							}
							buf++;
						}
						if(GetVsyncCounter()>wifi_timeout) return WIFI_TIMEOUT;
					}
					//extract size value from buffer
					return atoi(rxbuf);
				}
			}else{
				//reset string compare
				p=ipd;
			}
		}

		if(GetVsyncCounter()>wifi_timeout) return WIFI_TIMEOUT;
	}

}




int SendCommandAndWait(const char* strToSend, const char* strToWait){
	wifi_SendString_P(strToSend);
	if(wifi_WaitForStringP(strToWait, NULL)==WIFI_TIMEOUT){

		if(connOpen){
			wifi_SendString_P(PSTR("AT+CIPCLOSE=0\r\n"));
			wifi_WaitForStringP(PSTR("OK\r\n"), NULL);
			connOpen=false;
		}

		return WIFI_TIMEOUT;
	}

	return WIFI_OK;
}


int WaitForString_P(const char* strToWait){

	if(wifi_WaitForStringP(strToWait, NULL)==WIFI_TIMEOUT){

		if(connOpen){
			wifi_SendString_P(PSTR("AT+CIPCLOSE=0\r\n"));
			wifi_WaitForStringP(PSTR("OK\r\n"), NULL);
			connOpen=false;
		}

		return WIFI_TIMEOUT;
	}

	return WIFI_OK;
}

int SendDataAndWait(const char* strToSend, const char* strToWait){

	wifi_SendString_P(strToSend);

	if(wifi_WaitForStringP(strToWait,NULL)==WIFI_TIMEOUT){

		if(connOpen){
			wifi_SendString_P(PSTR("AT+CIPCLOSE=0\r\n"));
			wifi_WaitForStringP(PSTR("OK\r\n"), NULL);
			connOpen=false;
		}

		return WIFI_TIMEOUT;
	}

	return WIFI_OK;
}

int HttpGet(char* host,u16 port,char* url, HttpResponse* response){
	wifi_SendString_P(PSTR("AT+CIPSTART=0,\"TCP\",\""));
	wifi_SendString(host);
	wifi_SendString_P(PSTR("\",80\r\n"));
	WaitForString_P(PSTR("OK\r\n"));
	SendCommandAndWait(PSTR("AT+CIPSENDEX=0,256\r\n\f"),PSTR(">"));
	wifi_SendString_P(PSTR("GET "));
	wifi_SendString(url);
	wifi_SendString_P(PSTR(" HTTP/1.0\r\nHost: "));
	wifi_SendString(host);
	wifi_SendString_P(PSTR(":80\r\n\r\n\f"));
	WaitForString_P(PSTR("SEND OK\r\n"));

	int len=WaitforIPD();
	if(len<0)return len; //error!
	int res=ReceiveHtmlBody(response->content,len);
	if(res<0)return res; //error!

	WaitForString_P(PSTR("0,CLOSED\r\n"));

	return WIFI_OK;
}





