/*
 * Uzenet interface library used to communicate with the ESP8266.
 *
 * Notes:
 *  - This version only support a single TCP connection (AT+CIPMUX=0).
 *  - In order to simplify code, the library assumes that some paramters on
 *    the esp8266 were prepogrammed. See wifi_Init() for detail.
 *
 *  Created on: Oct 16, 2015-2023
 *      Author: Uze
 */
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "uzenet.h"
#include <time.h>

//uart clock dividers
#define UART_9600_CLK_DIV   	372
#define UART_19200_CLK_DIV  	185
#define UART_38400_CLK_DIV  	92
#define UART_57600_CLK_DIV		61
#define UART_115200_CLK_DIV		30

#define WIFI_DEFAULT_TIMEOUT 60*10	//10 sec
#define ESP_RESET PD3
#define ESP_ENABLE PA6

//enable debugging printouts
#define WIFI_DEBUG 0

int wifi_WaitForString_P(const char* str, char* rxbuf);
static int _wifi_SendCommandAndWait(const char* strToSend, const char* strToWait);
int WaitforIPD();

int _wifi_cons_putchar(char var, FILE *stream);
FILE UZENET_STREAM = FDEV_SETUP_STREAM(_wifi_cons_putchar, NULL, _FDEV_SETUP_WRITE);
wifi_CallBackFunc userCallBackFunc=NULL;
static bool echo=false, passtroughMode=false;
static u8 status=WIFI_UNINIT, tcpStatus=WIFI_TCP_DISCONNECTED;
static u16 wifi_timeout=WIFI_DEFAULT_TIMEOUT, vsyncCounter=0;


void _userCallBack(u16 status){
	if(userCallBackFunc!=NULL){
		userCallBackFunc(status);
	}
}

/**
 * Output characters to the current stream.
 */
void _wifi_putchar(u8 c){
	if(c=='\r' || c=='\n' || (c>=32 && c<127)){
		putchar(c);
	}else{
		putchar('.');
	}
}

/*
 * Internal function called by printf() functions as a stream handler.
 * If the main program include the following line:
 *
 *   stdout = &UZENET_STREAM;
 *
 * then printf() functions will output formatted text directly to the UART / ESP8266 RX pin.
 */
int _wifi_cons_putchar(char c, FILE *stream) {
	_wifi_putchar(c);
	return 0;
}

/**
 * Enable the wifi interface. Actually only supported by the Uzebox
 * Portable in order to wake up from sleep mode and save battery.
 */
void wifi_Enable(){
	DDRA|=(1<<ESP_ENABLE);
}


/**
 * Disable the wifi interface and UART.
 *
 * The actual disabling of the ESP8266 is only supported by the Uzebox
 * Portable in order to have the device enter sleep mode and save battery.
 */
void wifi_Disable(){
	DDRA&=~(1<<ESP_ENABLE);
	UCSR0B&=((1<<RXEN0)+(1<<TXEN0)); //Diable UART TX & RX
}

/**
 * Internal function to set the speed of the UART.
 *
 * speed : One the UART speed constant defined in uzenet.h
 */
void wifi_SetUartSpeed(u8 speed){
	//UART baud rate dividers for double speed
	const u16 bauds[]={UART_9600_CLK_DIV,UART_19200_CLK_DIV,UART_38400_CLK_DIV,UART_57600_CLK_DIV,UART_115200_CLK_DIV};
	u16 baud=bauds[speed];

	//initialize UART0
	UBRR0H=(baud>>8);
	UBRR0L=(baud&0xff);
	UCSR0A=(1<<U2X0); // double speed mode
	UCSR0C=(1<<UCSZ01)+(1<<UCSZ00)+(0<<USBS0); //8-bit frame, no parity, 1 stop bit
	UCSR0B=(1<<RXEN0)+(1<<TXEN0); //Enable UART TX & RX
}


/**
 * Return the current status of the wifi interface.
 */
u8 wifi_Status(){
	return status;
}


/**
 * Try to set the current speed of the ESP8266 and upon success, set the UART to the same speed.
 * Note that the ESP8266 will respond OK at the previous speed then switch at the requested speed.
 * Some delay is required before sending other commands.
 */
int wifi_SetSpeed(u8 speed){

	switch(speed){
		case UART_9600_BAUD:
			if(_wifi_SendCommandAndWait(PSTR("AT+UART_CUR=9600,8,1,0,0\r\n"),PSTR("OK\r\n"))!=WIFI_OK){
				return WIFI_ERR_TIMEOUT;
			}
			break;
		case UART_19200_BAUD:
			if(_wifi_SendCommandAndWait(PSTR("AT+UART_CUR=19200,8,1,0,0\r\n"),PSTR("OK\r\n"))!=WIFI_OK){
				return WIFI_ERR_TIMEOUT;
			}
			break;
		case UART_38400_BAUD:
			if(_wifi_SendCommandAndWait(PSTR("AT+UART_CUR=38400,8,1,0,0\r\n"),PSTR("OK\r\n"))!=WIFI_OK){
				return WIFI_ERR_TIMEOUT;
			}
			break;
		case UART_57600_BAUD:
			if(_wifi_SendCommandAndWait(PSTR("AT+UART_CUR=57600,8,1,0,0\r\n"),PSTR("OK\r\n"))!=WIFI_OK){
				return WIFI_ERR_TIMEOUT;
			}
			break;
		case UART_115200_BAUD:
			if(_wifi_SendCommandAndWait(PSTR("AT+UART_CUR=115200,8,1,0,0\r\n"),PSTR("OK\r\n"))!=WIFI_OK){
				return WIFI_ERR_TIMEOUT;
			}
			break;
		default:
			return WIFI_ERR_INV_SPEED;
			break;

	}

	wifi_SetUartSpeed(speed);

	WaitVsync(2); //IMPORTANT: Don't remove, this delay is required for the 8266 to internally switch speed.

	return WIFI_OK;
}


/**
 * Resets the ESP8266 wifi module
 */
void wifi_Reset(){
	DDRD |=(1<<ESP_RESET);	//set reset line to output
	PORTD|=(1<<ESP_RESET);	//deassert reset

	//force a double reset, for some reasons only 1 doesn't work on warmboots
	WaitUs(500);	//assert reset for at least 500us
	PORTD&=~(1<<ESP_RESET); // assert RESET
	WaitUs(500);	//assert reset for at least 500us
	PORTD|=(1<<ESP_RESET);	//deassert reset
	WaitUs(500);	//assert reset for at least 500us

	//enable module (required for Uzebox Portable)
	DDRA|=(1<<ESP_ENABLE);
	WaitUs(100);	//assert reset for at least 100us per spec
	PORTD|=(1<<ESP_RESET);	// deassert RESET
}


/* Initialize the wifi module.
 *
 * This function expects the ESP8266 UART to be set/defined (all other parameters must be factory set):
 *
 *  - UART parameters: 9800,N,8,1			(AT+UART_DEF 9600,8,1,0,0)
 *  - Client mode 							(AT+CWMODE_DEF=1)
 *  - Access point's SSID and password set	(AT+CWJAP_DEF ssid,pwd)
 *
 * Params:
 *  speed			 : One the UART speed constant
 *  callCackFunc	 : user function to be called on status change
 *  waitForConnection: Only initialize the module and don't wait for access point connection (ex: if module not yet configured)
 */
int wifi_Init(u8 speed, wifi_CallBackFunc callBackFunc, bool waitForConnection){

	if(callBackFunc!=NULL) userCallBackFunc=callBackFunc;

	u8 currentSpeed=UART_9600_BAUD;
	status=WIFI_UNINIT;

	wifi_SetUartSpeed(UART_9600_BAUD);	//Programmed at 9600 by uzeconf
	_userCallBack(status);

	wifi_Reset();

	if(wifi_WaitForString_P(PSTR("ready\r\n"),NULL)!=WIFI_OK){

		//We need to ensure the UART RX buffer size is bug enough to handlle 115200 BAUD
		//otherwise it may crash the program.
		if(UART_RX_BUFFER_SIZE<256){
			return WIFI_ERR_INIT;
		}

		//try at 115200
		wifi_Disable();
		wifi_SetUartSpeed(UART_115200_BAUD);
		wifi_Reset();

		if(wifi_WaitForString_P(PSTR("ready"),NULL)!=WIFI_OK){
			return WIFI_ERR_INIT;
		}
		currentSpeed=UART_115200_BAUD;

	}else{
		status=WIFI_READY;
	}


	if(currentSpeed != speed){
		#if WIFI_DEBUG == 1
			printf_P(PSTR("Setting new speed: %d, current=%d \r\n"),speed, currentSpeed);
		#endif

		wifi_SetSpeed(speed);

		#if WIFI_DEBUG == 1
			if(_wifi_SendCommandAndWait(PSTR("AT+UART_CUR?\r\n"),PSTR("OK\r\n"))!=WIFI_OK){
				return WIFI_ERR_INIT;
			}
		#endif

		if(_wifi_SendCommandAndWait(PSTR("ATE0\r\n"),PSTR("OK\r\n"))!=WIFI_OK){
			return WIFI_ERR_INIT;
		}
	}

	if(!waitForConnection){
		_userCallBack(status);
		return WIFI_OK;
	}

	#if WIFI_DEBUG == 1
		printf_P(PSTR("Waiting for IP...\r\n"));
	#endif

	if(wifi_WaitForString_P(PSTR("WIFI GOT IP\r\n"),NULL)==WIFI_ERR_TIMEOUT){
		#if WIFI_DEBUG == 1
			printf_P(PSTR("Timeout during reset sequence! \r\n"));
		#endif
		return WIFI_ERR_INIT;
	}

	#if WIFI_DEBUG == 1
		if(_wifi_SendCommandAndWait(PSTR("AT+UART_CUR?\r\n"),PSTR("OK\r\n"))!=WIFI_OK){
			return WIFI_ERR_INIT;
		}
	#endif

		/*
	if(currentSpeed != speed){
		#if WIFI_DEBUG == 1
			printf_P(PSTR("Setting new speed: %d, current=%d \r\n"),speed, currentSpeed);
		#endif

		wifi_SetSpeed(speed);

		#if WIFI_DEBUG == 1
			if(_wifi_SendCommandAndWait(PSTR("AT+UART_CUR?\r\n"),PSTR("OK\r\n"))!=WIFI_OK){
				return WIFI_ERR_INIT;
			}
		#endif

		if(_wifi_SendCommandAndWait(PSTR("ATE0\r\n"),PSTR("OK\r\n"))!=WIFI_OK){
			return WIFI_ERR_INIT;
		}
	}*/

	status=WIFI_CONNECTED;
	_userCallBack(status);

	return WIFI_OK;

}



/*
 * VSync callback handled that must be called by the main program once per field (60hz).
 */
void wifi_VsyncCallback(){
	vsyncCounter++;
}

/**
 * Enable echoing incomming and outgoing characters to the current stream.
 */
void wifi_Echo(bool echoOn){
	echo=echoOn;
}

/**
 * Send a string in PROGRAM memory to the wifi interface. If the UART buffer is full
 * this call will block until free space is available.
 */
int wifi_SendString_P(const char* str){

	while(1){
		char c=pgm_read_byte(str++);
		if(c==0)break;
		while(UartSendChar(c)==-1); //block if buffer full
		if(echo) _wifi_putchar(c);
	};

	return WIFI_OK;
}

/**
 * Send a string in RAM memory to the wifi interface. If the UART buffer is full
 * this call will block until free space is available.
 */
int wifi_SendString(char* str){

	while(1){
		char c=*str++;
		if(c==0)break;
		while(UartSendChar(c)==-1); //block if buffer full
		if(echo) _wifi_putchar(c);
	};

	return WIFI_OK;
}

/**
 * Wait for a specified string to be received from the wifi interface. This
 * call is blocking.
 *
 * Params
 *   str  : String to wait for
 *   rxbuf: Optional buffer to put the received content
 */
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

/**
 * Internal helper function to send a string to the wifi interface and wait for
 * a specified string to be received. This call is blocking. Both string must
 * be in program memory.
 *
 * Params
 *   strToSend  : String to send
 *   strToWait  : String to wait for
 */
static int _wifi_SendCommandAndWait(const char* strToSend, const char* strToWait){
	wifi_SendString_P(strToSend);
	return wifi_WaitForString_P(strToWait, NULL);
}

/**
 * Open a TCP socket to the specified host.
 *
 * Params
 *   host		: Host name or IP adress to connect to
 *   port		: TCP Port
 *   passthrough: true - Tell the module to send incoming characters as son as avaible (only value currently supported)
 *   			  false- Interafce need to use other AT commands to poll for available chars and read them (unsupported in this version)
 */
int wifi_TcpConnect(char* host, u16 port, bool passthrough){
	char port_str[6];
	itoa(port,port_str,10);

	if(passthrough){
		//Enable passthrough mode
		if(_wifi_SendCommandAndWait(PSTR("AT+CIPMODE=1\r\n"),PSTR("OK\r\n"))!=WIFI_OK) return WIFI_ERR;
		passtroughMode=true;
	}else if(passthrough==false && passtroughMode==true){
		//Disable passthough mode
		if(_wifi_SendCommandAndWait(PSTR("AT+CIPMODE=0\r\n"),PSTR("OK\r\n"))!=WIFI_OK) return WIFI_ERR;
		passtroughMode=true;
	}

	wifi_SendString_P(PSTR("AT+CIPSTART=\"TCP\",\""));
	wifi_SendString(host);
	wifi_SendString("\",");
	wifi_SendString(port_str);
	wifi_SendString(",7200\r\n");

	if(wifi_WaitForString_P(PSTR("OK\r\n"),NULL)!=WIFI_OK) return WIFI_ERR;

	if(passthrough){
		//enter passthrough mode
		//if(_wifi_SendCommandAndWait(PSTR("AT+CIPMODE=1\r\n"),PSTR("OK\r\n"))!=WIFI_OK) return WIFI_ERR;
		if(_wifi_SendCommandAndWait(PSTR("AT+CIPSEND\r\n"),PSTR(">"))) return WIFI_ERR;
		passtroughMode=true;
	}

	tcpStatus=WIFI_TCP_CONNECTED;

	return WIFI_OK;
}


/**
 * Disconnect from the open TCP connection (if any)
 */
int wifi_TcpDisconnect(){
	if(tcpStatus!=WIFI_TCP_DISCONNECTED){

		if(passtroughMode){
			//exit passtrough mode. Must wait 1 sec after before sending another AT command.
			wifi_SendString_P(PSTR("+++"));
			WaitVsync(60);
			if(_wifi_SendCommandAndWait(PSTR("AT+CIPCLOSE\r\n"),PSTR("OK\r\n"))!=WIFI_OK) return WIFI_ERR;
		}

		passtroughMode=false;
		tcpStatus=WIFI_TCP_DISCONNECTED;
	}

	return WIFI_OK;
}

/**
 * Return true if a TCP connection is currently open.
 */
bool wifi_TcpConnected(){
	return (tcpStatus==WIFI_TCP_CONNECTED);
}

/**
 * Return the number of unread characters pending.
 */
u8 wifi_UnreadCount(){
	return UartUnreadCount();
}

/**
 * Return the next available character. If the buffer is
 * empty, the function returns -1.
 */
s16 wifi_ReadChar(){
	int c=UartReadChar();
	if(echo && c!=-1) _wifi_putchar(c);
	return c;
}

/**
 * Send a character to the wifi interface. Blocks if the TX buffer is full.
 */
s16 wifi_SendChar(char c){
	while(UartSendChar(c)==-1); //block if buffer full
	if(echo) _wifi_putchar(c);
	return WIFI_OK; //todo: timeout?
}

/**
 * Specifies the default time in frames (1/60 of a seconds) to wait
 * for ESP8266 commands to respond. The default is 10 seconds.
 */
void wifi_SetTimeout(u16 timeout){
	wifi_timeout=timeout;
}

/**
 * Internal function to get content chunks from the 8266 when in non-passthrough mode.
 * Specifically, wait for for +IPD marker and return size of data
 * For future use.
 */
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
