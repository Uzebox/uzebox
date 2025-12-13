/*
 *  Native application to display wifi a list esp8266 module parameters.
 *  A native application is one compiled for the AVR
 *  and started from the CP/M shell via a simple laucher app.
 *
 *  Laucher source: /data/cpmsrc/uzecon.asm
 *
 *  Created on: dec 11, 2025
 *  Author: Uze
 *
 * Revs:
 * v1.0 - Original
 *
 */
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "uzenet.h"
#include <keyboard.h>
#include <time.h>
#include "terminal.h"


#define STATUS_CONNECTED 2
#define STATUS_TRANSMITTING 3
#define STATUS_DISCONNECTED 4
#define STATUS_FAIL 5

// Your data structure
typedef struct {
	char ssid[33];
	char ip[16];
	char mac[18];
	char firmware_ver[16];
	int channel;
	int rssi;
	int status;
} WifiStats;


// 1. Define the strings in Flash
const char s_excellent[]  PROGMEM = "Excellent";
const char s_good[] PROGMEM = "Good";
const char s_fair[] PROGMEM = "Fair";
const char s_weak[] PROGMEM = "Weak";
const char s_bad[]  PROGMEM = "Bad";
const char s_unusable[] PROGMEM = "Unusable";

// 2. Define the Lookup Table in Flash
// Index corresponds to: abs(rssi / 10)
const char* const rssi_table[] PROGMEM = {
	s_excellent,  // Index 0: 0 to -9
	s_excellent,  // Index 1: -10 to -19
	s_excellent,  // Index 2: -20 to -29
	s_excellent,  // Index 3: -30 to -39
	s_excellent,  // Index 4: -40 to -49 (Your cut-off for Excellent)
    s_good, // Index 5: -50 to -59
    s_fair, // Index 6: -60 to -69
    s_weak, // Index 7: -70 to -79
    s_bad,  // Index 8: -80 to -89
    s_unusable  // Index 9: -90 and lower
};


const char s_connected []  PROGMEM = "Connected";
const char s_transmitting []  PROGMEM = "Transmitting";
const char s_disconnected []  PROGMEM = "Disconnected";
const char s_fail []  PROGMEM = "Connection Fail";
const char* const status_table[] PROGMEM = {
	s_connected,
	s_transmitting,
	s_disconnected,
	s_fail
};

#define ERR_OK 0
#define ERR_INIT 1
#define ERR_AP_CONN 2


const char s_err_ok[]  PROGMEM = "Ok";
const char s_err_init[]  PROGMEM = "Unable to intialize interface";
const char s_err_ap_conn[]  PROGMEM = "No access point connection";

const char* const error_table[] PROGMEM = {
		s_err_ok,
		s_err_init,
		s_err_ap_conn
};


void parse_wifi_response(const char* buffer, WifiStats* stats);

void netstat_wifiCallback(s8 status){
}

static bool progress_display=false ;
void netstat_VsyncCallback(){

	static int progress_counter=0;
	if(progress_display == true){
		if(progress_counter++==15){
			printf_P(PSTR("\xe2\x80\xa2")); //print dot
			progress_counter=0;
		}
	}
}

static void quit(u8 code){
	progress_display=false;
	PGM_P err_msg = (PGM_P)pgm_read_word(&error_table[code]);
	printf_P(PSTR("\r\nError: %S. \r\nRun UZECONF.COM first to setup the Uzenet interface.\r\n"),err_msg);
}

static void ReadDummyLine(char* buf, u8 count){
	for(u8 i=0;i<count;i++){
		wifi_ReadLine(buf);
	}
}

void  uzenet_netstat(){

	WifiStats stats;
	char lineBuffer[128];
	u8 retry_cnt=0;
	int status;

	//terminal_Clear();
	wifi_Echo(false);
	wifi_SetTimeout(60*3);

	printf_P(PSTR("Displays network interface information (v1.0)\r\nInitializing "));
	progress_display=true;

	/**
	 * Initialize the wifi module
	 */
	while(1){
		wifi_Disable();
		WaitVsync(10);
		int status= wifi_Init(UART_115200_BAUD,&netstat_wifiCallback,true); //initialize to default speed for the esp8266
		if(status==WIFI_OK){
			break;
		}else if(retry_cnt<1){
			retry_cnt++;
		}else{
			if( wifi_Status()==WIFI_READY){
				printf_P(PSTR("\r\n\r\n   Status: Initialized\r\n"));
				quit(ERR_AP_CONN);
			}else{
				quit(ERR_INIT);
			}
			return;
		}
	}

	progress_display=false;
	printf_P(PSTR(" Ok\r\n\r\n"));

	/**
	 * Get the interface connection status
	 */
	wifi_SendString_P(PSTR("AT+CIPSTATUS\r\n"));
	wifi_ReadLine(lineBuffer); //response
	parse_wifi_response(lineBuffer,&stats);
	PGM_P status_msg = (PGM_P)pgm_read_word(&status_table[stats.status-2]);
	ReadDummyLine(lineBuffer,2); //skip crlf, OK

	/**
	 * Get the SSID, channel and RSSI
	 */
	wifi_SendString_P(PSTR("AT+CWJAP_CUR?\r\n"));
	wifi_ReadLine(lineBuffer); //response
	parse_wifi_response(lineBuffer,&stats);
	ReadDummyLine(lineBuffer,2); //skip crlf, OK
	//produce a human redeable form for the rssi
	int idx = -stats.rssi / 10;
	if (idx > 9) idx = 9; // Cap anything worse than -90 as "Unusable"
	PGM_P signal_strenght_msg = (PGM_P)pgm_read_word(&rssi_table[idx]);

	/**
	 * Get the station IP
	 */
	wifi_SendString_P(PSTR("AT+CIPSTA?\r\n"));
	wifi_ReadLine(lineBuffer); //response
	parse_wifi_response(lineBuffer,&stats);
	ReadDummyLine(lineBuffer,4); //skip gateway, netmask, crlf, OK

	/**
	 * Get the station MAC
	 */
	wifi_SendString_P(PSTR("AT+CIPSTAMAC?\r\n"));
	wifi_ReadLine(lineBuffer); //response
	parse_wifi_response(lineBuffer,&stats);
	ReadDummyLine(lineBuffer,2); //skip crlf, OK

	/**
	 * Get the AT firware version
	 */
	wifi_SendString_P(PSTR("AT+GMR\r\n"));
	wifi_ReadLine(lineBuffer); //response
	parse_wifi_response(lineBuffer,&stats);
	ReadDummyLine(lineBuffer,4); //skip crlf, OK


	printf_P(PSTR("   Status: %S\r\n     SSID: %s\r\n  Channel: %i\r\n     RSSI: %i dBm (%S)\r\n       IP: %s\r\n      MAC: %s\r\n Firmware: %s\r\n"),
		status_msg,stats.ssid,stats.channel,stats.rssi ,signal_strenght_msg, stats.ip,stats.mac,stats.firmware_ver);


}

void parse_wifi_response(const char* buffer, WifiStats* stats) {

	   // 1. Connection Status (STATUS:2)
	    if (sscanf(buffer, "STATUS:%d", &stats->status) == 1) {
	        return;
	    }

	    // 2. SSID (AT+CWJAP?)
	    if (strstr(buffer, "+CWJAP")) {
	        sscanf(buffer, "%*[^:]:\"%32[^\"]\",\"%*[^\"]\",%d,%d",
	               stats->ssid, &stats->channel, &stats->rssi);
	        return;
	    }

	    // 3. MUST be checked BEFORE +CIPSTA because CIPSTAMAC contains CIPSTA
	    if (strstr(buffer, "+CIPSTAMAC")) {
	        // Parse: +CIPSTAMAC:"18:fe:..."
	        sscanf(buffer, "%*[^:]:\"%17[^\"]\"", stats->mac);
	        return;
	    }

	    // 4. IP Address (AT+CIPSTA?)
	    if (strstr(buffer, "+CIPSTA")) {
	        if (strstr(buffer, "ip:")) {
	            // Parse: +CIPSTA:ip:"192.168..."
	            sscanf(buffer, "%*[^:]:ip:\"%15[^\"]\"", stats->ip);
	        }
	        return;
	    }

	    // 5. Firmware
	    if (strstr(buffer, "AT version:")) {
	        sscanf(buffer, "AT version:%15[^(\r\n]", stats->firmware_ver);
	        return;
	    }
}

