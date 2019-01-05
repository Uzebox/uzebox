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

#define WIFI_DEFAULT_TIMEOUT 60*15	//15 sec

#define ESP_RESET PD3
#define ESP_ENABLE PA6

//Common setting for double speed mode
//UBRR0L=185;	//9600 bauds	960 bytes/s		16 bytes/field
//UBRR0L=92;	//19200 bauds	1920 bytes/s	32 bytes/field
//UBRR0L=46;	//38400 bauds	3840 bytes/s	64 bytes/field
//UBRR0L=60;	//57600 bauds	5760 bytes/s	96 bytes/field
//UBRR0L=30;	//115200 bauds
#define UART_9600_BAUD 185
#define UART_19200_BAUD 92
#define UART_38400_BAUD 46
#define UART_57600_BAUD 60
#define UART_115200_BAUD 30


static int _wifi_SendCommandAndWait(const char* strToSend, const char* strToWait);
//int	SendDataAndWait(const char* strToSend, const char* strToWait);
wifi_CallBackFunc userCallBackFunc=NULL;
int WaitforIPD();

static bool echo=false;
static u8 status=WIFI_STAT_UNINIT;
static u16 wifi_timeout=WIFI_DEFAULT_TIMEOUT,vsyncCounter=0;

void _userCallBack(u16 status){
	if(userCallBackFunc!=NULL){
		userCallBackFunc(status);
	}
}

void _wifi_putchar(u8 c){
	//putchar(c);

	if(c=='\r' || c=='\n' || (c>=32 && c<127)){
		putchar(c);
	}else{
		putchar('.');
	}

}

static void _wifi_SetUartSpeed(u8 speed){
	//initialize UART0
	UBRR0H=0;
	UBRR0L=speed;
	UCSR0A=(1<<U2X0); // double speed mode
	UCSR0C=(1<<UCSZ01)+(1<<UCSZ00)+(0<<USBS0); //8-bit frame, no parity, 1 stop bit
	UCSR0B=(1<<RXEN0)+(1<<TXEN0); //Enable UART TX & RX
}

/* Initialize the wifi module
 *
 * callCackFunc: user function to be called on status change
 *
 * Assumes the following setting are programmed in the esp flash.
 * If not, user must use the wifi setup rom.
 *  AT+UART_DEF 115200,8,1,0,0
 *  AT+CWMODE_DEF=1
 *  AT+CWJAP_DEF ssid,pwd
 *  AT+CIPMUX=0
 *  AT+SYSMSG_DEF=0
 */
int wifi_Init(wifi_CallBackFunc callBackFunc){
	userCallBackFunc=callBackFunc;

	u8 speed[] = {UART_9600_BAUD,UART_115200_BAUD,UART_57600_BAUD,UART_38400_BAUD,UART_19200_BAUD,0}; //try speeds in order of most common
	int i=0;

	PORTD|=(1<<ESP_RESET);

	wifi_timeout=60;



	//try different uart speeds
	while(true){

		_wifi_SetUartSpeed(speed[i]);

		//enable module
		DDRA|=(1<<ESP_ENABLE);
		wifi_Reset();

		#ifdef WIFI_DEBUG
			printf_P(PSTR("wifi_debug - UART trying speed: %i\r\n"),speed[i]);
		#endif


		if(wifi_WaitForString_P(PSTR("ready\r\n"),NULL)==WIFI_OK) {
			break;
		}

		#ifdef WIFI_DEBUG
			printf_P(PSTR("wifi_debug - UART speed fail!\r\n"));
		#endif

		if(speed[++i]==0) return WIFI_ERR_INIT;

		//disable the module & retry
		DDRA&=~(1<<ESP_ENABLE);
		WaitVsync(1);
	}

	#ifdef WIFI_DEBUG
		printf_P(PSTR("wifi_debug -DEF Speed is %i\r\n"),speed[i]);
	#endif

	//Set the speed at 115200 bauds if not already set
	if(speed[i]!=30){
		wifi_SendString_P(PSTR("AT+UART_CUR=115200,8,1,0,0\r\n"));
		wifi_WaitForString_P(PSTR("OK\r\n"),NULL);
		UBRR0L=30;

		#ifdef WIFI_DEBUG
			printf_P(PSTR("wifi_debug - Setting speed to 115200\r\n"));
		#endif
	}


	status=WIFI_STAT_READY;
	_userCallBack(status);

	wifi_timeout=WIFI_DEFAULT_TIMEOUT;
	wifi_SendString_P(PSTR("ATE0\r\n"));
	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)==WIFI_ERR_TIMEOUT){
		return WIFI_ERR_CMD;
	}
	status=WIFI_STAT_CONNECTED;
	_userCallBack(status);


	if(wifi_WaitForString_P(PSTR("WIFI GOT IP\r\n"),NULL)==WIFI_ERR_TIMEOUT){
		return WIFI_ERR_IP;
	}
	status=WIFI_STAT_IP;
	_userCallBack(status);

	wifi_SendString_P(PSTR("AT+GMR\r\n"));
	wifi_WaitForString_P(PSTR("OK\r\n"),NULL);


	return WIFI_OK;
}


void wifi_RestoreDefaultSettings(){

	//todo: find uart speed

	wifi_SendString_P(PSTR("AT+RESTORE\r\n"));
	wifi_WaitForString_P(PSTR("OK\r\n"),NULL);

	wifi_SendString_P(PSTR("AT+UART_DEF=9600,8,1,0,0\r\n"));
	wifi_WaitForString_P(PSTR("OK\r\n"),NULL);

	_wifi_SetUartSpeed(UART_9600_BAUD);

	wifi_SendString_P(PSTR("AT+CWMODE_DEF=1\r\n"));
	wifi_WaitForString_P(PSTR("OK\r\n"),NULL);

	wifi_SendString_P(PSTR("AT+CWJAP_DEF=\"uzenet\",\"h6dkj90xghrwx89hncx59ktre61hb2k77de67v1\"\r\n"));
	wifi_WaitForString_P(PSTR("OK\r\n"),NULL);

	//AT+SAVETRANSLINK=0: wifi passthough-mode: not enabled by default.
	//AT+CWDHCP_DEF=1,1: Enable DHCP for station mode
}

void wifi_Reset(){
	//reset module
	DDRD|=(1<<ESP_RESET);
	PORTD&=~(1<<ESP_RESET);
	WaitVsync(1);
	PORTD|=(1<<ESP_RESET);
	WaitVsync(1);
}

/*
 * Must be called by the main program once per field (60hz).
 */
void wifi_Tick(){
	vsyncCounter++;
}

void wifi_Echo(bool echoOn){
	echo=echoOn;
}

int wifi_SendString_P(const char* str){

	while(1){
		char c=pgm_read_byte(str++);
		if(c==0)break;
		while(UartSendChar(c)==-1); //block if buffer full
		if(echo) _wifi_putchar(c);
	};

	return WIFI_OK;
}

int wifi_SendString(char* str){

	while(1){
		char c=*str++;
		if(c==0)break;
		while(UartSendChar(c)==-1); //block if buffer full
		if(echo) _wifi_putchar(c);
	};

	return WIFI_OK;
}

int wifi_WaitForString_P(const char* str, char* rxbuf){
	u8 c;
	s16 result;
	const char* p=str;
	vsyncCounter=0;

	while(1){
		result=UartReadChar();
		if(result!=-1){

			c=result&(0xff);
			if(rxbuf!=NULL)*rxbuf++=c;
			if(echo)	_wifi_putchar(c);

			if(c==pgm_read_byte(p++)){
				if(pgm_read_byte(p)==0)	return WIFI_OK;
			}else{
				p=str; //reset string compare
			}
		}

		if(vsyncCounter>wifi_timeout){
			return WIFI_ERR_TIMEOUT;
		}
	}
}

int wifi_TcpConnect(char* host, u16 port, bool passthrough){
	char port_str[6];
	itoa(port,port_str,10);

	wifi_SendString_P(PSTR("AT+CIPSTART=\"TCP\",\""));
	wifi_SendString(host);
	wifi_SendString("\",");
	wifi_SendString(port_str);
	wifi_SendString(",7200\r\n");
	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK) return WIFI_ERR;

	if(passthrough){
		if(_wifi_SendCommandAndWait(PSTR("AT+CIPMODE=1\r\n"),PSTR("OK\r\n"))!=WIFI_OK) return WIFI_ERR;
		if(_wifi_SendCommandAndWait(PSTR("AT+CIPSEND\r\n"),PSTR(">"))) return WIFI_ERR;
	}

	return WIFI_OK;
}


u8 wifi_UnreadCount(){
	return UartUnreadCount();
}

s16 wifi_ReadChar(){
	return UartReadChar();
}

s16 wifi_SendChar(char c){
	while(UartSendChar(c)==-1); //block if buffer full
	if(echo) _wifi_putchar(c);
	return WIFI_OK; //todo: timeout?
}



const char bodyMarker[] PROGMEM = "\r\n\r\n";
int ReceiveHtmlBody(char* rxbuf,int len){
	u8 c;
	s16 r,headerSize=0,bodySize=0;
	bool inBody=false;
	char* buf=rxbuf;
	const char* p=bodyMarker;
	vsyncCounter=0;

	while(1){

		r=UartReadChar();
		if(r!=-1){

			c=r&(0xff);
			if(echo) _wifi_putchar(c);

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

		if(vsyncCounter>wifi_timeout){
			return WIFI_ERR_TIMEOUT;
		}

		if(headerSize+bodySize == len){
			break;
		}
	}

	*buf=0;
	return bodySize;
}

//wait for +IPD marker and return size of data
const char ipd[] PROGMEM = "+IPD,";
int WaitforIPD(){
	u8 c;
	s16 data;

	const char* p=ipd;
	char rxbuf[16];
	char* buf=rxbuf;
	vsyncCounter=0;

	while(1){
		data=UartReadChar();
		if(data!=-1){

			c=data&(0xff);
			if(echo) _wifi_putchar(c);

			if(c==pgm_read_byte(p)){
				p++;
				if(pgm_read_byte(p)==0){

					//now wait for the ":"character
					while(1){
						data=UartReadChar();
						if(data!=-1){
							c=data&(0xff);
							*buf=c;
							if(echo) _wifi_putchar(c);
							if(c==':'){
								*buf=0;
								break;
							}
							buf++;
						}
						if(vsyncCounter>wifi_timeout) return WIFI_ERR_TIMEOUT;
					}
					//extract size value from buffer
					return atoi(rxbuf);
				}
			}else{
				//reset string compare
				p=ipd;
			}
		}

		if(vsyncCounter>wifi_timeout) return WIFI_ERR_TIMEOUT;
	}

}




static int _wifi_SendCommandAndWait(const char* strToSend, const char* strToWait){
	wifi_SendString_P(strToSend);
	return wifi_WaitForString_P(strToWait, NULL);
}

//int SendDataAndWait(const char* strToSend, const char* strToWait){
//	wifi_SendString_P(strToSend);
//	return wifi_WaitForString_P(strToWait,NULL);
//}

int HttpGet(char* host,u16 port,char* url, HttpResponse* response){
	wifi_SendString_P(PSTR("AT+CIPSTART=\"TCP\",\""));
	wifi_SendString(host);
	wifi_SendString_P(PSTR("\",80\r\n"));
	wifi_WaitForString_P(PSTR("OK\r\n"),NULL);
	_wifi_SendCommandAndWait(PSTR("AT+CIPSENDEX=256\r\n\f"),PSTR(">"));
	wifi_SendString_P(PSTR("GET "));
	wifi_SendString(url);
	wifi_SendString_P(PSTR(" HTTP/1.0\r\nHost: "));
	wifi_SendString(host);
	wifi_SendString_P(PSTR(":80\r\n\r\n\f"));
	wifi_WaitForString_P(PSTR("SEND OK\r\n"),NULL);

	int len=WaitforIPD();
	if(len<0)return len; //error!
	int res=ReceiveHtmlBody(response->content,len);
	if(res<0)return res; //error!

	wifi_WaitForString_P(PSTR("CLOSED\r\n"),NULL);

	return WIFI_OK;
}





