/*
 * uzenet.h
 *
 *  Created on: Oct 16, 2015
 *      Author: admin
 */

#ifndef UZENET_H_
#define UZENET_H_

#define WIFI_OK 			 0
#define WIFI_TIMEOUT 		-1
#define WIFI_RECEIVE_ERROR	-2

#define HTTP_GET	0
#define HTTP_POST	1

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


extern void wifi_HWReset();
extern int wifi_SendString_P(const char* str);
extern int wifi_SendString(char* str);
extern int wifi_WaitForStringP(const char* str, char* rxbuf);
extern int SendCommandAndWait(const char* strToSend, const char* strToWait);
extern int SendDataAndWait(const char* strToSend, const char* strToWait);
extern int WaitForString_P(const char* strToWait);
extern int HttpGet(char* host,u16 port,char* url, HttpResponse* response);

extern void wifi_Echo(bool echoOn);


#endif /* UZENET_H_ */
