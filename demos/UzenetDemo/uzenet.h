/*
 * uzenet.h
 *
 *  Created on: Oct 16, 2015
 *      Author: admin
 */

#ifndef UZENET_H_
#define UZENET_H_

#define WIFI_OK					0

#define WIFI_STAT_UNINIT		0
#define WIFI_STAT_READY			1
#define WIFI_STAT_CONNECTED		2
#define WIFI_STAT_IP			3

#define WIFI_ERR				-1
#define WIFI_ERR_TIMEOUT		-2
#define WIFI_ERR_RECEIVE		-3
#define WIFI_ERR_INIT			-4
#define WIFI_ERR_IP				-5
#define WIFI_ERR_CMD			-6

#define HTTP_GET	0
#define HTTP_POST	1


typedef void (*wifi_CallBackFunc)(s8 status);

extern void wifi_Tick();
extern void wifi_Reset();
extern void wifi_Echo(bool echoOn);
extern int 	wifi_Init(wifi_CallBackFunc func);
extern int 	wifi_SendString_P(const char* str);
extern int 	wifi_SendString(char* str);
extern int 	wifi_WaitForString_P(const char* str, char* rxbuf);
extern int 	wifi_TcpConnect(char* host, u16 port, bool passthrough);
extern u8   wifi_UnreadCount();
extern s16  wifi_ReadChar();
extern int  wifi_SendChar(char c);
extern void wifi_RestoreDefaultSettings();


//TODO: put in some other lib

typedef struct {
	unsigned char verb;
	char* host;
	unsigned int port;
	char* url;
	char* content;
} HttpRequest;

typedef struct {
	unsigned int responseCode;
	char* content;
} HttpResponse;

extern int HttpGet(char* host,u16 port,char* url, HttpResponse* response);


#endif /* UZENET_H_ */
