/*
 ****************************************************************************
 * Uzem - The Uzebox Emulator
 *
 * This file was derived from the Simulavr project by Filipe Rinaldi (2009)
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002  Theodore A. Roth
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************
 */

#ifndef SIM_GDB_H
#define SIM_GDB_H

#if defined(__WIN32__)
	#include <winsock2.h>
	#define SIGHUP 1
	#define SIGINT 2
	#define SIGQUIT 3
	#define SIGILL 4
	#define SIGTRAP 5
	#define CLOSE_SOCK(s) closesocket(s)
	typedef int socklen_t;
#else
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#define CLOSE_SOCK(s) close(s)
#endif

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <cstdint>


struct avr8;

typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint32_t dword_t;

class Breakpoints: public std::vector<dword_t> {
};

#define MAX_BUF 400 /* Maximum size of read/write buffers. */

#define GET_LITTLE_ENDIAN16(byte1,byte2)	((byte1 << 8) | byte2)
#define GET_BIG_ENDIAN16(byte1,byte2)		((byte2 << 8) | byte1)

#ifdef USE_GDBSERVER_DEBUG
	#define gdb_debug(fmt,...)	fprintf(stderr,"[GDB] "/**/fmt, ##__VA_ARGS__)
#else
	#define gdb_debug(fmt,...)
#endif

enum {
    //    MAX_BUF        = 400,         /* Maximum size of read/write buffers. */
    MAX_READ_RETRY = 5,          /* Maximum number of retries if a read is incomplete. */

    MEM_SPACE_MASK = 0x00ff0000,  /* mask to get bits which determine memory space */
    FLASH_OFFSET   = 0x00000000,  /* Data in flash has this offset from gdb */
    SRAM_OFFSET    = 0x00800000,  /* Data in sram has this offset from gdb */
    EEPROM_OFFSET  = 0x00810000,  /* Data in eeprom has this offset from gdb */

    GDB_BLOCKING_OFF = 0,         /* Signify that a read is non-blocking. */
    GDB_BLOCKING_ON  = 1,         /* Signify that a read will block. */

    GDB_RET_NOTHING_RECEIVED = -5, /* if the read in non blocking receives nothing, we have nothing todo */
    GDB_RET_SINGLE_STEP = -4,     /* do one single step in gdb loop */
    GDB_RET_CONTINUE    = -3,     /* step until another command from gdb is received */
    GDB_RET_CTRL_C       = -2,    /* gdb has sent Ctrl-C to interrupt what is doing */
    GDB_RET_KILL_REQUEST = -1,    /* gdb has requested that sim be killed */
    GDB_RET_OK           =  0     /* continue normal processing of gdb requests */
        /* means that we should NOT execute any step!!! */
};

class GdbServer {
    protected: 
        avr8 *core;
        int port;       //internet port number
        int sock;       //the opened os-net-socket
        int conn;       //the real established conection
        struct sockaddr_in address[1];
        socklen_t          addrLength[1]; 

        int global_debug_on;    //debugging the debugger
        int waitForGdbConnection;
        int runMode;

        //method local static vars.
        char *last_reply;  //used in last_reply();
        char buf[MAX_BUF]; //used in send_reply();
        int block_on;      //used in pre_parse_packet();

    	FILE* logFile;

        word_t avr_core_flash_read(int addr) ;
        void avr_core_flash_write(unsigned int addr, word_t val) ;
        void avr_core_flash_write_hi8(unsigned int addr, byte_t val) ;
        void avr_core_flash_write_lo8(unsigned int addr, byte_t val) ;
        void avr_core_remove_breakpoint(dword_t pc) ;
        void avr_core_insert_breakpoint(dword_t pc) ;
        int signal_has_occurred(int signo); 
        void signal_watch_start(int signo);
        void signal_watch_stop(int signo);
        int avr_core_step() ;
        int gdb_read_byte( );
        int hex2nib( char hex );
        void gdb_write( const void *buf, size_t count );
        const char* gdb_last_reply( const char *reply );
        void gdb_send_ack( );
        void gdb_send_reply( const char *reply );
        void gdb_read_registers( );
        void gdb_write_registers(  char *pkt );
        int gdb_extract_hex_num( char **pkt, char stop );
        void gdb_read_register( char *pkt );
        void gdb_write_register( char *pkt );
        int gdb_get_addr_len( char *pkt, char a_end, char l_end, unsigned int *addr, int *len);
        void gdb_read_memory( char *pkt );
        void gdb_write_memory( char *pkt );
        void gdb_break_point( char *pkt );
        void gdb_continue( char *pkt );
        int gdb_get_signal(char *pkt);
        int gdb_parse_packet( char *pkt );
        void gdb_set_blocking_mode( int mode );
        int gdb_pre_parse_packet( int blocking );
        void gdb_main_loop(); 
        void gdb_interact( int port, int debug_on );
        void SendPosition(int signal); //send gdb the actual position where the simulation is stopped

    public:
        GdbServer( avr8*, int port, int debugOn, int WaitForGdbConnection=true);
        virtual ~GdbServer();
	Breakpoints BP;
	void exec(void);
        bool TryConnectGdb();
};

#endif /* SIM_GDB_H */
