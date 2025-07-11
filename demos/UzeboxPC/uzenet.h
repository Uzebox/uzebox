/*
 * uzenet.h
 *
 *  Created on: Oct 16, 2015
 *      Author: admin
 */

#ifndef UZENET_H_
#define UZENET_H_

#include <uzebox.h>

#define WIFI_OK					0

/**
 * wifi Interface states
 */
#define WIFI_UNINIT				0	//module not initialized
#define WIFI_READY				1	//module ready to receive commands via UART
#define WIFI_CONNECTED			2	//module connected to access point

/**
 * WIFI TCO connection error codes
 */
#define WIFI_TCP_DISCONNECTED	0	//module is disconnected from host
#define WIFI_TCP_CONNECTED		1	//module is connected to host via TCP

/**
 * WIFI error coded
 */
#define WIFI_ERR				-1
#define WIFI_ERR_TIMEOUT		-2
#define WIFI_ERR_RECEIVE		-3
#define WIFI_ERR_INIT			-4
#define WIFI_ERR_IP				-5
#define WIFI_ERR_CMD			-6
#define WIFI_ERR_INV_SPEED		-7

/**
 * UART speed constants
 *
 * Minimum UART buffer size by BAUD speed:
 * 9600: 32 bytes
 * 19200: 64 bytes
 * 38400: 128 bytes
 * 57600: 128 bytes
 * 115200: 256 bytes
 */
#define UART_9600_BAUD   		0 //372
#define UART_19200_BAUD  		1 //185
#define UART_38400_BAUD  		2 //92
#define UART_57600_BAUD			3 //61
#define UART_115200_BAUD		4 //30
#define UART_DEF_BAUD			0

/**
 * Callback function prototype that allow the main program to
 * receive the evolving status of the interface
 */
typedef void (*wifi_CallBackFunc)(s8 status);

/*
 * Stream handler. If the main program include the following line:
 *
 *   stdout = &UZENET_STREAM;
 *
 * then printf() functions will output formatted text directly to the UART / ESP8266 RX pin.
 */
extern FILE UZENET_STREAM;


/**
 * Enable the wifi interface. Actually only supported by the Uzebox
 * Portable in order to wake up from sleep mode and save battery.
 */
extern void	wifi_Enable();

/**
 * Disable the wifi interface. Actually only supported by the Uzebox
 * Portable in order to have the device enter sleep mode and save battery.
 */
extern void wifi_Disable();

/**
 * Resets the ESP8266 wifi module
 */
extern void wifi_Reset();

/**
 * Sets the speed of the UART.
 *
 * Params
 *   speed : One the UART speed constant defined in uzenet.h
 */
extern void wifi_SetUartSpeed(u8 speed);


/* Initialize the wifi module by resetting it and setting the requested UART speed for the MCU and ESP8266.
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
 *  waitForConnection: Not used
 */
extern int  wifi_Init(u8 speed, wifi_CallBackFunc callBackFunc, bool waitForConnection);

/*
 * VSync callback handled that must be called by the main program once per field (60hz).
 */
extern void wifi_VsyncCallback();

/**
 * Return the current status of the wifi interface.
 */
extern u8   wifi_Status();

/**
 * Open a TCP socket to the specified host.
 *
 * Params
 *   host		: Host name or IP adress to connect to
 *   port		: TCP Port
 *   passthrough: true - Tell the module to send incoming characters as son as avaible (only value currently supported)
 *   			  false- Interafce need to use other AT commands to poll for available chars and read them (unsupported in this version)
 */
extern int 	wifi_TcpConnect(char* host, u16 port, bool passthrough);

/**
 * Return true if a TCP connection is currently open.
 */
extern bool wifi_TcpConnected();

/**
 * Disconnect from the open TCP connection (if any)
 */
extern int  wifi_TcpDisconnect();

/**
 * Return the number of unread characters pending.
 */
extern u8   wifi_UnreadCount();

/**
 * Return the next available character. If the buffer is
 * empty, the function returns -1.
 */
extern int  wifi_ReadChar();

/**
 * Send a character to the wifi interface. Blocks if the TX buffer is full.
 */
extern int  wifi_SendChar(char c);

/**
 * Enable echoing incomming and outgoing characters to the current stream.
 */
extern void wifi_Echo(bool echoOn);

/**
 * Send a string in PROGRAM memory to the wifi interface. If the UART buffer is full
 * this call will block until free space is available.
 */
extern int 	wifi_SendString_P(const char* str);

/**
 * Send a string in RAM memory to the wifi interface. If the UART buffer is full
 * this call will block until free space is available.
 */
extern int 	wifi_SendString(char* str);

/**
 * Wait for a specified string to be received from the wifi interface. This
 * call is blocking.
 *
 * Params
 *   str  : String to wait for
 *   rxbuf: Optional buffer to put the received content
 */
extern int 	wifi_WaitForString_P(const char* str, char* rxbuf);


/**
 * Specifies the default time in frames (1/60 of a seconds) to wait
 * for ESP8266 commands to respond. The default is 10 seconds.
 */
extern void wifi_SetTimeout(u16 timeout);

#endif /* UZENET_H_ */
