/*
 ****************************************************************************
 * Uzem - The Uzebox Emulator
 *
 * This file was derived from the Simulavr project by Filipe Rinaldi (2009)
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002, 2003  Theodore A. Roth, Klaus Rudolph		
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
#include <iostream>
#include <algorithm>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include "gdbserver.h"
#include "avr8.h"

#define avr_new(type, count, flag)	((type *) do_avr_new(((unsigned) sizeof (type) * (count)), flag))
static void *do_avr_new(size_t size, bool blank_it)
{
    if (size)
    {
        void *ptr;
        ptr = malloc( size );
        if (ptr)
	{
	    if (blank_it == true)
	    	memset(ptr, 0, size);
            return ptr;
	}
	else
	{
            fprintf(stderr, "malloc failed");
	    exit(0);
	}
    }
    return NULL;
}

static char *avr_strdup(const char *s)
{
    if (s)
    {
        char *ptr;
        ptr = strdup(s);
        if (ptr)
            return ptr;
	
        fprintf(stderr, "strdup failed" );
	exit(0);
    }
    return NULL;
}

static void avr_free(void *ptr)
{
    if (ptr)
        free(ptr);
}

GdbServer::GdbServer(avr8 *c, int _port, int debug, int _waitForGdbConnection): core(c), port(_port), global_debug_on(debug), waitForGdbConnection(_waitForGdbConnection) {
    last_reply=NULL; //init static var for last_reply()
    //is_running=0;    //init static var for continue()
    block_on=1;      //init static var for pre_parse_packet()
    conn=-1;        //no connection opened 
    runMode= GDB_RET_NOTHING_RECEIVED;

    int i;
	
#if defined(__WIN32__)
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("WSAStartup() failed");
        exit(1);
    }
#endif

    if ( (sock = socket( PF_INET, SOCK_STREAM, 0 )) < 0 )
        printf( "Can't create socket: %s", strerror(errno) );

    /* Let the kernel reuse the socket address. This lets us run
    twice in a row, without waiting for the (ip, port) tuple
    to time out. */
    i = 1;  
    setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i) );
#if defined(__WIN32__)
	u_long iMode = 1;
	ioctlsocket(sock, FIONBIO, &iMode);
#else
    fcntl( sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK); //dont know 
#endif
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    memset( &address->sin_addr, 0, sizeof(address->sin_addr) );

    if ( bind( sock, (struct sockaddr *)address, sizeof(address) ) )
        printf( "Can not bind socket: %s", strerror(errno) );

    if ( listen(sock, 1) < 0)
    {
        cerr << "Can not listen on socket: " <<  strerror(errno) << endl;
    }

    fprintf( stderr, "Waiting on port %d for gdb client to connect...\n", port );

    logFile=fopen("gdb.log","w");
    fprintf(logFile,"Opening GDB Session 2\n");
}

GdbServer::~GdbServer() {
    CLOSE_SOCK(conn);
    CLOSE_SOCK(sock);

    fprintf(logFile,"Closing GDB Session\n");
    fclose(logFile);
#if defined(__WIN32__)
	WSACleanup();
#endif
}

word_t GdbServer::avr_core_flash_read(int addr) {
    return core->progmem[addr];
}

void GdbServer::avr_core_flash_write( unsigned int addr, word_t val) {
    if (addr>= progSize) {
        cerr << "try to write in flash after last valid address!" << endl;
        exit(0);
    }

    core->progmem[addr]=val;
}

void GdbServer::avr_core_flash_write_hi8( unsigned int addr, byte_t val) {
    if ((addr*2)>= progSize) {
        cerr << "try to write in flash after last valid address!" << endl;
        exit(0);
    }
    u16 tmp = (core->progmem[addr] & 0x00FF) | (val << 8);
    core->progmem[addr] = tmp;
}

void GdbServer::avr_core_flash_write_lo8( unsigned int addr, byte_t val) {
    if (addr>=progSize) {
        cerr << "try to write in flash after last valid address!" << endl;
        exit(0);
    }
    u16 tmp = (core->progmem[addr] & 0xFF00) | (val);
    core->progmem[addr] = tmp;
}

void GdbServer::avr_core_remove_breakpoint(dword_t pc) {
    Breakpoints::iterator ii;
    if ((ii= find(BP.begin(), BP.end(), pc)) != BP.end()) 
        BP.erase(ii);
}

void GdbServer::avr_core_insert_breakpoint(dword_t pc) {
    BP.push_back(pc);
}

int GdbServer::signal_has_occurred(int signo) {(void)signo; return 0;}
void GdbServer::signal_watch_start(int signo){(void)signo;}
void GdbServer::signal_watch_stop(int signo){(void)signo;}


static char HEX_DIGIT[] = "0123456789abcdef";
/* Wrap read(2) so we can read a byte without having
to do a shit load of error checking every time. */

int GdbServer::gdb_read_byte( )
{
    char c;
    int res;
    int cnt = MAX_READ_RETRY;

    while (cnt--)
    {
        res = recv( conn, &c, 1, 0 );

#if defined(__WIN32__)
		if (res == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			   /* fd was set to non-blocking and no data was available */
			   return -1;
			printf( "read failed: %s", strerror(errno) );
		}
#else
		if (res<0)
		{
		   if (errno == EAGAIN)
			   /* fd was set to non-blocking and no data was available */
			   return -1;

		   printf( "read failed: %s", strerror(errno) );
		}
#endif

        if (res == 0) {
            printf( "gdb closed connection. Exiting...\n" );
            exit(0);
        }
        return c;
    }
    printf( "Maximum read retries reached\n" );
    exit(0);

    return 0; /* make compiler happy */
}

/* Convert a hexidecimal digit to a 4 bit nibble. */

int GdbServer::hex2nib( char hex )
{
    if ( (hex >= 'A') && (hex <= 'F') )
        return (10 + (hex - 'A'));

    else if ( (hex >= 'a') && (hex <= 'f') )
        return (10 + (hex - 'a'));

    else if ( (hex >= '0') && (hex <= '9') )
        return (hex - '0');

    /* Shouldn't get here unless the developer screwed up ;) */     
    printf( "Invalid hexidecimal digit: 0x%02x", hex );

    return 0; /* make compiler happy */
}

/* Wrapper for write(2) which hides all the repetitive error
checking crap. */

void GdbServer::gdb_write( const void *buf, size_t count )
{
    int res;

    res = send(  conn, (const char*)buf, count, 0 );

    /* FIXME: should we try and catch interrupted system calls here? */

    if (res < 0)
        printf( "write failed: %s", strerror(errno) );

    /* FIXME: if this happens a lot, we could try to resend the
    unsent bytes. */

    if ((unsigned int)res != count)
        printf( "write only wrote %d of %ld bytes", res, count );
}

/* Use a single function for storing/getting the last reply message.
If reply is NULL, return pointer to the last reply saved.
Otherwise, make a copy of the buffer pointed to by reply. */

const char* GdbServer::gdb_last_reply( const char *reply )
{

    if (reply == NULL)
    {
        if (last_reply == NULL)
            return "";
        else
            return last_reply;
    }

    avr_free(last_reply);

    last_reply = avr_strdup( reply );
    if (last_reply == 0)
    {
	fprintf(stderr, "Failed to copy string %s\n",reply);
	exit(0);
    }

    return last_reply;
}

/* Acknowledge a packet from GDB */

void GdbServer::gdb_send_ack( )
{
    if (global_debug_on)
        fprintf( stderr, " Ack -> gdb\n");

    gdb_write( "+", 1 );
}

/* Send a reply to GDB. */

void GdbServer::gdb_send_reply( const char *reply )
{
    int cksum = 0;
    int bytes;

    /* Save the reply to last reply so we can resend if need be. */
    gdb_last_reply( reply );

    if (global_debug_on)
        fprintf( stderr, "Sent: $%s#", reply );

    if (*reply == '\0')
    {
        gdb_write( "$#00", 4 );

        if (global_debug_on)
            fprintf( stderr, "%02x\n", cksum & 0xff );
    }
    else
    {
        memset( buf, '\0', sizeof(buf) );

        buf[0] = '$';
        bytes = 1;

        while (*reply)
        {
            cksum += (unsigned char)*reply;
            buf[bytes] = *reply;
            bytes++;
            reply++;

            /* must account for "#cc" to be added */
            if (bytes == (MAX_BUF-3))
            {
                /* FIXME: TRoth 2002/02/18 - splitting reply would be better */
                printf( "buffer overflow" );
            }
        }

        if (global_debug_on)
            fprintf( stderr, "%02x\n", cksum & 0xff );

        buf[bytes++] = '#';
        buf[bytes++] = HEX_DIGIT[(cksum >> 4) & 0xf];
        buf[bytes++] = HEX_DIGIT[cksum & 0xf];

        gdb_write( buf, bytes );
    }
}

/* GDB needs the 32 8-bit, gpw registers (r00 - r31), the 
8-bit SREG, the 16-bit SP (stack pointer) and the 32-bit PC
(program counter). Thus need to send a reply with
r00, r01, ..., r31, SREG, SPL, SPH, PCL, PCH
Low bytes before High since AVR is little endian. */

void GdbServer::gdb_read_registers( )
{
    int   i;
    dword_t val;                  /* ensure it's 32 bit value */

    /* (32 gpwr, SREG, SP, PC) * 2 hex bytes + terminator */
    size_t  buf_sz = (32 + 1 + 2 + 4)*2 + 1;
    char   *buf;

    buf = avr_new( char, buf_sz, true );

    /* 32 gen purpose working registers */
    for ( i=0; i<32; i++ )
    {
	val = core->r[i];
        buf[i*2]   = HEX_DIGIT[(val >> 4) & 0xf];
        buf[i*2+1] = HEX_DIGIT[val & 0xf];
    }

    /* GDB thinks SREG is register number 32 */
    val = core->SREG;
    buf[i*2]   = HEX_DIGIT[(val >> 4) & 0xf];
    buf[i*2+1] = HEX_DIGIT[val & 0xf];
    i++;

    /* GDB thinks SP is register number 33 */
    //val = avr_core_mem_read(core, SPL_ADDR);
    val=core->SPL;
    buf[i*2]   = HEX_DIGIT[(val >> 4) & 0xf];
    buf[i*2+1] = HEX_DIGIT[val & 0xf];
    i++;

    //val = avr_core_mem_read(core, SPH_ADDR);
    val=core->SPH;
    buf[i*2]   = HEX_DIGIT[(val >> 4) & 0xf];
    buf[i*2+1] = HEX_DIGIT[val & 0xf];
    i++;

    /* GDB thinks PC is register number 34.
    GDB stores PC in a 32 bit value (only uses 23 bits though).
    GDB thinks PC is bytes into flash, not words like in simulavr. */

    val = core->pc * 2;
    buf[i*2]   = HEX_DIGIT[(val >> 4)  & 0xf];
    buf[i*2+1] = HEX_DIGIT[val & 0xf];

    val >>= 8;
    buf[i*2+2] = HEX_DIGIT[(val >> 4) & 0xf];
    buf[i*2+3] = HEX_DIGIT[val & 0xf];

    val >>= 8;
    buf[i*2+4] = HEX_DIGIT[(val >> 4) & 0xf];
    buf[i*2+5] = HEX_DIGIT[val & 0xf];

    val >>= 8;
    buf[i*2+6] = HEX_DIGIT[(val >> 4) & 0xf];
    buf[i*2+7] = HEX_DIGIT[val & 0xf];

    gdb_send_reply(  buf );

    avr_free(buf);
}

/* GDB is sending values to be written to the registers. Registers are the
same and in the same order as described in gdb_read_registers() above. */

void GdbServer::gdb_write_registers( char *pkt )
{
    int   i;
    byte_t  bval;
    dword_t val;                  /* ensure it's a 32 bit value */

    /* 32 gen purpose working registers */
    for ( i=0; i<32; i++ )
    {
        bval  = hex2nib(*pkt++) << 4;
        bval += hex2nib(*pkt++);
        core->r[i]=bval;

    }

    /* GDB thinks SREG is register number 32 */
    bval  = hex2nib(*pkt++) << 4;
    bval += hex2nib(*pkt++);
    core->SREG=bval;

    /* GDB thinks SP is register number 33 */
    bval  = hex2nib(*pkt++) << 4;
    bval += hex2nib(*pkt++);
    core->SPL = bval;

    bval  = hex2nib(*pkt++) << 4;
    bval += hex2nib(*pkt++);
    core->SPH = bval;

    /* GDB thinks PC is register number 34.
    GDB stores PC in a 32 bit value (only uses 23 bits though).
    GDB thinks PC is bytes into flash, not words like in simulavr.

    Must cast to dword_t so as not to get mysterious truncation. */

    val  = ((dword_t)hex2nib(*pkt++)) << 4;
    val += ((dword_t)hex2nib(*pkt++));

    val += ((dword_t)hex2nib(*pkt++)) << 12;
    val += ((dword_t)hex2nib(*pkt++)) << 8;

    val += ((dword_t)hex2nib(*pkt++)) << 20;
    val += ((dword_t)hex2nib(*pkt++)) << 16;

    val += ((dword_t)hex2nib(*pkt++)) << 28;
    val += ((dword_t)hex2nib(*pkt++)) << 24;
    core->pc=val/2;

    gdb_send_reply( "OK" );
}

/* Extract a hexidecimal number from the pkt. Keep scanning pkt until stop char
is reached or size of int is exceeded or a NULL is reached. pkt is modified
to point to stop char when done.

Use this function to extract a num with an arbitrary num of hex
digits. This should _not_ be used to extract n digits from a m len string
of digits (n <= m). */

int GdbServer::gdb_extract_hex_num( char **pkt, char stop )
{
    int i = 0;
    int num = 0;
    char *p = *pkt;
    int max_shifts = sizeof(int)*2-1; /* max number of nibbles to shift through */

    while ( (*p != stop) && (*p != '\0') )
    {
        if (i > max_shifts)
            printf( "number too large" );

        num = (num << 4) | hex2nib(*p);
        i++;
        p++;
    }

    *pkt = p;
    return num;
}

/* Read a single register. Packet form: 'pn' where n is a hex number with no
zero padding. */

void GdbServer::gdb_read_register( char *pkt )
{
    int reg;

    char reply[MAX_BUF];

    memset(reply, '\0', sizeof(reply));

    reg = gdb_extract_hex_num(&pkt, '\0');

    if ( (reg >= 0) && (reg < 32) )
    {                           /* general regs */
        byte_t val = core->r[reg];
        snprintf( reply, sizeof(reply)-1, "%02x", val );
    }
    else if (reg == 32)         /* sreg */
    {
        byte_t val = core->SREG;
        snprintf( reply, sizeof(reply)-1, "%02x", val );
    }
    else if (reg == 33)         /* SP */
    {
        byte_t spl, sph;
        //spl = avr_core_mem_read( core, SPL_ADDR );
        //sph = avr_core_mem_read( core, SPH_ADDR );
        spl=core->SPL;
        sph=core->SPH;
        snprintf( reply, sizeof(reply)-1, "%02x%02x", spl, sph );
    }
    else if (reg == 34)         /* PC */
    {
        dword_t val = core->pc * 2;
        snprintf( reply, sizeof(reply)-1,
                "%02x%02x" "%02x%02x", 
                val & 0xff, (val >> 8) & 0xff,
                (val >> 16) & 0xff, (val >> 24) & 0xff );
    }
    else
    {
        printf( "Bad register value: %d\n", reg );
        gdb_send_reply( "E00" );
        return;
    }
    gdb_send_reply( reply );
}

/* Write a single register. Packet form: 'Pn=r' where n is a hex number with
no zero padding and r is two hex digits for each byte in register (target
byte order). */

void GdbServer::gdb_write_register( char *pkt )
{
    int reg;
    int val, hval;
    dword_t dval;

    reg = gdb_extract_hex_num(&pkt, '=');
    pkt++;                      /* skip over '=' character */

    /* extract the low byte of value from pkt */
    val  = hex2nib(*pkt++) << 4;
    val += hex2nib(*pkt++);

    if ( (reg >= 0) && (reg < 33) )
    {
        /* r0 to r31 and SREG */
        if (reg == 32)          /* gdb thinks SREG is register 32 */
        {
            core->SREG=val&0xff;
        }
        else
        {
            core->r[reg]=val&0xff;
        }
    }
    else if (reg == 33)
    {
        /* SP is 2 bytes long so extract upper byte */
        hval  = hex2nib(*pkt++) << 4;
        hval += hex2nib(*pkt++);

        //avr_core_mem_write( core, SPL_ADDR, val & 0xff );
        //avr_core_mem_write( core, SPH_ADDR, hval & 0xff );
        core->SPL = (val&0xff);
        core->SPH = (hval&0xff);
    }
    else if (reg == 34)
    {
        /* GDB thinks PC is register number 34.
        GDB stores PC in a 32 bit value (only uses 23 bits though).
        GDB thinks PC is bytes into flash, not words like in simulavr.

        Must cast to dword_t so as not to get mysterious truncation. */

        dval  = (dword_t)val; /* we already read the first two nibbles */

        dval += ((dword_t)hex2nib(*pkt++)) << 12;
        dval += ((dword_t)hex2nib(*pkt++)) << 8;

        dval += ((dword_t)hex2nib(*pkt++)) << 20;
        dval += ((dword_t)hex2nib(*pkt++)) << 16;

        dval += ((dword_t)hex2nib(*pkt++)) << 28;
        dval += ((dword_t)hex2nib(*pkt++)) << 24;
        core->pc=dval / 2;
    }
    else
    {
        printf( "Bad register value: %d\n", reg );
        gdb_send_reply(  "E00" );
        return;
    }

    gdb_send_reply( "OK" );
}

/* Parse the pkt string for the addr and length.
a_end is first char after addr.
l_end is first char after len.
Returns number of characters to advance pkt. */

int GdbServer::gdb_get_addr_len( char *pkt, char a_end, char l_end, unsigned int *addr, int *len )
{
    char *orig_pkt = pkt;

    *addr = 0;
    *len  = 0;

    /* Get the addr from the packet */
    while (*pkt != a_end)
        *addr = (*addr << 4) + hex2nib(*pkt++);
    pkt++;                      /* skip over a_end */

    /* Get the length from the packet */
    while (*pkt != l_end)
        *len = (*len << 4) + hex2nib(*pkt++);
    pkt++;                      /* skip over l_end */

    /*      fprintf( stderr, "+++++++++++++ addr = 0x%08x\n", *addr ); */
    /*      fprintf( stderr, "+++++++++++++ len  = %d\n", *len ); */

    return (pkt - orig_pkt);
}

void GdbServer::gdb_read_memory( char *pkt )
{
    unsigned int   addr = 0;
    int   len  = 0;
    byte_t *buf;
    byte_t  bval;
    word_t  wval;
    int   i;
    int   is_odd_addr;


    pkt += gdb_get_addr_len( pkt, ',', '\0', &addr, &len );

    buf = avr_new( byte_t, (len*2)+1, true );

    //if(addr>=0x804000) addr&=0xffff;
    fprintf(logFile,"%x:",addr);

    if ( (addr & MEM_SPACE_MASK) == EEPROM_OFFSET )
    {
        /* addressing eeprom */

        addr = addr & ~MEM_SPACE_MASK; /* remove the offset bits */

        //printf( "reading of eeprom not yet implemented: 0x%x.\n", addr );
        //snprintf( (char*)buf, len*2, "E%02x", EIO );
        for ( i=0; i<len; i++ )
        {
            bval = core->eeprom[addr+i];
            buf[i*2]   = HEX_DIGIT[bval >> 4];
            buf[i*2+1] = HEX_DIGIT[bval & 0xf];
        }
    }
    else if ( (addr & MEM_SPACE_MASK) == SRAM_OFFSET )
    {
        /* addressing sram */

        addr = addr & ~MEM_SPACE_MASK; /* remove the offset bits */

	if (addr >= (SRAMBASE+sramSize))
	{
	    fprintf(logFile,"invalid ram");

		if (global_debug_on)
	        printf("Sram address:%x invalid\n",addr);
            snprintf( (char*)buf, len*2, "E%02x", EIO );
	}
	else
	{
		// Trying to access one of the registers (below IOBASE)
        	if (addr < IOBASE)
		{
                	bval = core->r[addr];
                	buf[0]   = HEX_DIGIT[bval >> 4];
                	buf[1] = HEX_DIGIT[bval & 0xf];
		}
		// Trying to access one of the IOs (below SRAMBASE)
        	else if (addr < SRAMBASE )
		{
			bval = core->io[addr - IOBASE];
                	buf[0]   = HEX_DIGIT[bval >> 4];
                	buf[1] = HEX_DIGIT[bval & 0xf];
		}
		else
		// Trying to access the generic SRAM memory
        	{
			// Uzem has an array only for the generic SRAM, so, we need calculate the correct address
			addr -= SRAMBASE;
            		for ( i=0; i<len; i++ )
            		{
				if (global_debug_on)
					printf("Reading sram address:%x\n",addr+i);
                		bval = core->sram[addr+i];
                		buf[i*2]   = HEX_DIGIT[bval >> 4];
                		buf[i*2+1] = HEX_DIGIT[bval & 0xf];
           	 	}
        	}
	}
    }
    else if ( (addr & MEM_SPACE_MASK) == FLASH_OFFSET )
    {
        /* addressing flash */

        addr = addr & ~MEM_SPACE_MASK; /* remove the offset bits */

        is_odd_addr = addr % 2;
        i = 0;

        if (is_odd_addr)
        {
            bval = avr_core_flash_read( addr/2 ) >> 8;
            buf[i++] = HEX_DIGIT[bval >> 4];
            buf[i++] = HEX_DIGIT[bval & 0xf];
            addr++;
            len--;
        }

        while (len > 1)
        {
            wval = avr_core_flash_read( addr/2 );

            bval = wval & 0xff;
            buf[i++] = HEX_DIGIT[bval >> 4];
            buf[i++] = HEX_DIGIT[bval & 0xf];

            bval = wval >> 8;
            buf[i++] = HEX_DIGIT[bval >> 4];
            buf[i++] = HEX_DIGIT[bval & 0xf];

            len -= 2;
            addr += 2;
        }

        if (len == 1)
        {
            bval = avr_core_flash_read( addr/2 ) & 0xff;
            buf[i++] = HEX_DIGIT[bval >> 4];
            buf[i++] = HEX_DIGIT[bval & 0xf];
        }
    }
    else
    {
        /* gdb asked for memory space which doesn't exist */
        printf( "Invalid memory address: 0x%x.\n", addr );

        fprintf(logFile,"Invalid memory address: 0x%x.\n", addr);

        snprintf( (char*)buf, len*2, "E%02x", EIO );
    }


    gdb_send_reply( (char*)buf );

    avr_free(buf);
}

void GdbServer::gdb_write_memory( char *pkt )
{
    unsigned int  addr = 0;
    int  len  = 0;
    byte_t bval;
    word_t wval;
    int  is_odd_addr;
    unsigned int  i;
    char reply[10];

    /* Set the default reply. */
    strncpy( reply, "OK", sizeof(reply) );

    pkt += gdb_get_addr_len( pkt, ',', ':', &addr, &len );


    if ( (addr & MEM_SPACE_MASK) == EEPROM_OFFSET )
    {
        /* addressing eeprom */

        addr = addr & ~MEM_SPACE_MASK; /* remove the offset bits */

        //	printf( "writing of eeprom not yet implemented: 0x%x.\n", addr );
        //	snprintf( reply, sizeof(reply), "E%02x", EIO );

        while (len>0) {
            bval  = hex2nib(*pkt++) << 4;
            bval += hex2nib(*pkt++);
            len--;
            core->eeprom[addr] = bval;
            addr++;
        }
    }
    else if ( (addr & MEM_SPACE_MASK) == SRAM_OFFSET )
    {
        /* addressing sram */

        addr = addr & ~MEM_SPACE_MASK; /* remove the offset bits */

	if (addr >= (SRAMBASE+sramSize))
	{
	    if (global_debug_on)
	        printf("Sram address:%x invalid\n",addr);
            snprintf( (char*)buf, len*2, "E%02x", EIO );
	}
	// Trying to access one of the registers (below IOBASE)
        else if (addr < IOBASE)
	{
                bval  = hex2nib(*pkt++) << 4;
                bval += hex2nib(*pkt++);
                core->r[addr]=bval;
	}
	// Trying to access one of the IOs (below SRAMBASE)
        else if (addr < SRAMBASE )
	{
		addr -= IOBASE;
                bval  = hex2nib(*pkt++) << 4;
                bval += hex2nib(*pkt++);
                core->io[addr]=bval;
	}
	else
	// Trying to access the generic SRAM memory
        {
            // Uzem has an array only for the generic SRAM, so, we need calculate the correct address
            addr -= SRAMBASE;

            for ( i=addr; i < addr+len; i++ )
            {
                bval  = hex2nib(*pkt++) << 4;
                bval += hex2nib(*pkt++);
                core->sram[i]=bval;
            }
        }
    }
    else if ( (addr & MEM_SPACE_MASK) == FLASH_OFFSET )
    {
        /* addressing flash */

        addr = addr & ~MEM_SPACE_MASK; /* remove the offset bits */

        is_odd_addr = addr % 2;

        if (is_odd_addr)
        {
            bval  = hex2nib(*pkt++) << 4;
            bval += hex2nib(*pkt++);
            avr_core_flash_write_hi8(addr/2, bval);
            len--;
            addr++;
        }

        while (len > 1)
        {
            wval  = hex2nib(*pkt++) << 4; /* low byte first */
            wval += hex2nib(*pkt++);
            wval += hex2nib(*pkt++) << 12; /* high byte last */
            wval += hex2nib(*pkt++) << 8;
            avr_core_flash_write( addr/2, wval);
            len  -= 2;
            addr += 2;
        }

        if ( len == 1 )
        {
            /* one more byte to write */
            bval  = hex2nib(*pkt++) << 4;
            bval += hex2nib(*pkt++);
            avr_core_flash_write_lo8( addr/2, bval );
        }
    }
    else
    {
        /* gdb asked for memory space which doesn't exist */
        printf( "Invalid memory address: 0x%x.\n", addr );
        snprintf( reply, sizeof(reply), "E%02x", EIO );
    }

    gdb_send_reply( reply );
}

/* Format of breakpoint commands (both insert and remove):

"z<t>,<addr>,<length>"  -  remove break/watch point
"Z<t>,<add>r,<length>"  -  insert break/watch point

In both cases t can be the following:
t = '0'  -  software breakpoint
t = '1'  -  hardware breakpoint
t = '2'  -  write watch point
t = '3'  -  read watch point
t = '4'  -  access watch point

addr is address.
length is in bytes

For a software breakpoint, length specifies the size of the instruction to
be patched. For hardware breakpoints and watchpoints, length specifies the
memory region to be monitored. To avoid potential problems, the operations
should be implemented in an idempotent way. -- GDB 5.0 manual. */

void GdbServer::gdb_break_point( char *pkt )
{
    unsigned int addr = 0;
    int len  = 0;

    char z = *(pkt-1);          /* get char parser already looked at */
    char t = *pkt++;
    pkt++;                      /* skip over first ',' */

    //cout << "###############################################" << endl;

    gdb_get_addr_len( pkt, ',', '\0', &addr, &len );

    switch (t) {
        case '0':               /* software breakpoint */
            /* addr/2 since addr refers to PC */
            if ( addr >= progSize )
            {
                printf( "Attempt to set break at invalid addr\n" );
                gdb_send_reply( "E01" );
                return;
            }




            if (z == 'z') 
            {
                //cout << "Try to UNSET a software breakpoint" << endl;
                //cout << "at address :" << addr << " with len " << len << endl;
                avr_core_remove_breakpoint( addr/2 );
            }
            else
            {
                //cout << "Try to SET a software breakpoint" << endl;
                //cout << "at address :" << addr << " with len " << len << endl;
                avr_core_insert_breakpoint( addr/2 );
            }
            break;

        case '1':               /* hardware breakpoint */
            //cout << "Try to set a hardware breakpoint" << endl;
            //cout << "at address :" << addr << " with len " << len << endl;

            gdb_send_reply( "" );
            return;
            break;

        case '2':               /* write watchpoint */
            //cout << "Try to set a watchpoint" << endl;
            //cout << "at address :" << addr << " with len " << len << endl;
            gdb_send_reply( "" );
            return;
            break;

        case '3':               /* read watchpoint */
            //cout << "Try to set a read watchpoint" << endl;
            //cout << "at address :" << addr << " with len " << len << endl;
            gdb_send_reply( "" );
            return;
            break;

        case '4':               /* access watchpoint */
            //cout << "try to set a access watchpoint" << endl;
            //cout << "at address :" << addr << " with len " << len << endl;
            gdb_send_reply( "" );
            return;             /* unsupported yet */
    }

    gdb_send_reply( "OK" );
}

/* Continue command format: "c<addr>" or "s<addr>"

If addr is given, resume at that address, otherwise, resume at current
address. */


/* Continue with signal command format: "C<sig>;<addr>" or "S<sig>;<addr>"
"<sig>" should always be 2 hex digits, possibly zero padded.
";<addr>" part is optional.

If addr is given, resume at that address, otherwise, resume at current
address. */

int GdbServer::gdb_get_signal( char *pkt )
{
    int signo;
    //char step = *(pkt-1);

    /* strip out the signal part of the packet */

    signo  = (hex2nib( *pkt++ ) << 4);
    signo += (hex2nib( *pkt++ ) & 0xf);

    if (global_debug_on)
        fprintf( stderr, "GDB sent signal: %d\n", signo );

    /* Process signals send via remote protocol from gdb. Signals really don't
    make sense to the program running in the simulator, so we are using
    them sort of as an 'out of band' data. */

    switch (signo)
    {
        case SIGHUP:
            /* Gdb user issuing the 'signal SIGHUP' command tells sim to reset
            itself. We reply with a SIGTRAP the same as we do when gdb
            makes first connection with simulator. */
            //TODO: Create a reset function for avr8
	    //core->reset( );
	    printf("Reset not yet supported, aborting...");
            //gdb_send_reply( "S05" );
	    core->shutdown(0);
    }

    return signo;

}

/* Parse the packet. Assumes that packet is null terminated.
Return GDB_RET_KILL_REQUEST if packet is 'kill' command,
GDB_RET_OK otherwise. */

int GdbServer::gdb_parse_packet( char *pkt )
{

    fprintf(logFile,"\n%s:",pkt);

	switch (*pkt++) {
        case '?':               /* last signal */
            gdb_send_reply( "S05" ); /* signal # 5 is SIGTRAP */
            break;

        case 'g':               /* read registers */
            gdb_read_registers(  );
            break;

        case 'G':               /* write registers */
            gdb_write_registers(  pkt );
            break;

        case 'p':               /* read a single register */
            gdb_read_register(  pkt );
            break;

        case 'P':               /* write single register */
            gdb_write_register(  pkt );
            break;

        case 'm':               /* read memory */
            gdb_read_memory(  pkt );
            break;

        case 'M':               /* write memory */
            gdb_write_memory(  pkt );
            break;

        case 'D':               /* detach the debugger */
        case 'k':               /* kill request */
            /* Reset the simulator since there may be another connection
            before the simulator stops running. */
            gdb_send_reply(  "OK" );
            return GDB_RET_KILL_REQUEST;

        case 'c':               /* continue */
            return GDB_RET_CONTINUE;
            break;

        case 'C':               /* continue with signal */
            if(SIGHUP==gdb_get_signal(pkt)) { //very special solution only for regression testing woth
                                              //old scripts from old simulavr! Continue means not continue
                                              //if signal is SIGHUP :-(, so we do nothing then!
                return GDB_RET_OK;
            }
            
            return GDB_RET_CONTINUE;
            break;

        case 's':               /* step */
            return GDB_RET_SINGLE_STEP;
            break;

        case 'S':               /* step with signal */
            gdb_get_signal(pkt);
            return GDB_RET_SINGLE_STEP;
            break;

        case 'z':               /* remove break/watch point */
        case 'Z':               /* insert break/watch point */
            gdb_break_point(  pkt );
            break;

        case 'q':               /* query requests */
            gdb_send_reply(  "" );
            break;

        default:
            gdb_send_reply(  "" );
    }

    return GDB_RET_OK;
}

void GdbServer::gdb_set_blocking_mode( int mode )
{
#if defined(__WIN32__)
	u_long imode = mode;
	if (ioctlsocket(sock, FIONBIO, &imode))
		printf( "fcntl failed: %s\n", strerror(errno) );
#else
    if (mode)
    {
        /* turn non-blocking mode off */
        if (fcntl( conn, F_SETFL, fcntl(conn, F_GETFL, 0) & ~O_NONBLOCK) < 0)
            printf( "fcntl failed: %s\n", strerror(errno) );
    }
    else
    {
        /* turn non-blocking mode on */
        if (fcntl( conn, F_SETFL, fcntl(conn, F_GETFL, 0) | O_NONBLOCK) < 0)
            printf( "fcntl failed: %s\n", strerror(errno) );
    }
#endif
}

/* Perform pre-packet parsing. This will handle messages from gdb which are
outside the realm of packets or prepare a packet for parsing.

Use the static block_on flag to reduce the over head of turning blocking on
and off every time this function is called. */

int GdbServer::gdb_pre_parse_packet( int blocking )
{
    int  i, res;
    int  c;
    char pkt_buf[MAX_BUF+1];
    int  cksum, pkt_cksum;

    gdb_set_blocking_mode( blocking);

    c = gdb_read_byte( );

    switch (c) {
        case '$':           /* read a packet */
            /* insure that packet is null terminated. */
            memset( pkt_buf, 0, sizeof(pkt_buf) );

            /* make sure we block on fd */
            gdb_set_blocking_mode( GDB_BLOCKING_ON );

            pkt_cksum = i = 0;
            c = gdb_read_byte();
            while ( (c != '#') && (i < MAX_BUF) )
            {
                pkt_buf[i++] = c;
                pkt_cksum += (unsigned char)c;
                c = gdb_read_byte();
            }

            cksum  = hex2nib( gdb_read_byte() ) << 4;
            cksum |= hex2nib( gdb_read_byte() );

            /* FIXME: Should send "-" (Nak) instead of aborting when we get
            checksum errors. Leave this as an error until it is actually
            seen (I've yet to see a bad checksum - TRoth). It's not a simple
            matter of sending (Nak) since you don't want to get into an
            infinite loop of (bad cksum, nak, resend, repeat).*/

            if ( (pkt_cksum & 0xff) != cksum )
                fprintf( stderr, "Bad checksum: sent 0x%x <--> computed 0x%x",
                        cksum, pkt_cksum );

            if (global_debug_on)
                fprintf( stderr, "Recv: \"$%s#%02x\"\n", pkt_buf, cksum );

            /* always acknowledge a well formed packet immediately */
            gdb_send_ack( );

            res = gdb_parse_packet( pkt_buf );
            if (res < 0 )
                return res;

            break;

        case '-':
            if (global_debug_on)
                fprintf( stderr, " gdb -> Nak\n" );
            gdb_send_reply(  gdb_last_reply(NULL) );
            break;

        case '+':
            if (global_debug_on)
                fprintf( stderr, " gdb -> Ack\n" );
            break;

        case 0x03:
            /* Gdb sends this when the user hits C-c. This is gdb's way of
            telling the simulator to interrupt what it is doing and return
            control back to gdb. */
            return GDB_RET_CTRL_C;

        case -1:
            /* fd is non-blocking and no data to read */
            return GDB_RET_NOTHING_RECEIVED;
            break;

        default:
            printf( "Unknown request from gdb: %c (0x%02x)\n", c, c );
    }

    return GDB_RET_OK;
}

/* try to open a new connection to gdb */
bool GdbServer::TryConnectGdb() {
    int i;

    /* accept() needs this set, or it fails (sometimes) */
    addrLength[0] = sizeof(struct sockaddr_in);

    /* We only want to accept a single connection, thus don't need a loop. */
    /* Wait until we have a connection */
    conn = accept( sock, (struct sockaddr *)address, addrLength );

#if defined(__WIN32__)
    if (conn != INVALID_SOCKET) {
#else
    if (conn>0) {
#endif
        /* Tell TCP not to delay small packets.  This greatly speeds up
        interactive response. WARNING: If TCP_NODELAY is set on, then gdb
        may timeout in mid-packet if the (gdb)packet is not sent within a
        single (tcp)packet, thus all outgoing (gdb)packets _must_ be sent
        with a single call to write. (see Stevens "Unix Network
        Programming", Vol 1, 2nd Ed, page 202 for more info) */

        i = 1;
        setsockopt (conn, IPPROTO_TCP, TCP_NODELAY, (char*)&i, sizeof (i));
	core->state = CPU_SINGLE_STEP;
        printf("Connection opened!\n");
        return true;
    }

    // Check SDL events, may be the user wants close the application
    core->idle();
	
    return false;
}

void GdbServer::exec(void) {
    static int wait = 0;
    char reply[MAX_BUF+1];
    bool leave = false;

    if ((conn<0) && (TryConnectGdb() == false))
	   return;

    // If we check for gdb packets after eachinstruction, it takes much time.
    // So, if the user sends a 'continue', try to execute a bunch of instructions before check gdb again.
    if (wait)
    {
	wait--;
	return;
    }

    if (core->gdbBreakpointFound == true) 
    {
    	if (global_debug_on)
        	fprintf( stderr, "Run on breakpoint\n");
	core->gdbBreakpointFound = false;
        runMode=GDB_RET_OK; //we will stop next call from GdbServer::Step
        SendPosition(SIGTRAP);
    }

    if (core->gdbInvalidOpcode == true)
    {
        snprintf( reply, MAX_BUF, "S%02x", SIGILL );
        gdb_send_reply( reply );
        runMode=GDB_RET_OK;
        SendPosition(SIGILL);
    }

    if (runMode==GDB_RET_SINGLE_STEP) {
        runMode=GDB_RET_OK;
        SendPosition(SIGTRAP);
    }


    do {
        int gdbRet=gdb_pre_parse_packet(GDB_BLOCKING_OFF);

        switch (gdbRet) {
            case GDB_RET_NOTHING_RECEIVED:  //nothing changes here
                break;

            case GDB_RET_OK:    //dont change any modes
                runMode=GDB_RET_OK;
                break;

           case GDB_RET_CONTINUE:
                runMode=GDB_RET_CONTINUE;       //lets continue until we receive something from gdb (normal CTRL-C)
                break;                          //or we run into a break point or illegal instruction

           case GDB_RET_SINGLE_STEP:
                runMode=GDB_RET_SINGLE_STEP;
                break;

           case GDB_RET_CTRL_C:
		gdb_debug("Ctr+C issued. PC:0x%04x\n",core->pc*2);
                runMode=GDB_RET_CTRL_C;
                SendPosition(SIGINT); //Give gdb an idea where the core is now 
                break;

            case GDB_RET_KILL_REQUEST:              
		printf("Killed from the client...bye!\n");
                core->shutdown(0);
		break;
            }

            if ((runMode == GDB_RET_SINGLE_STEP ) || ( runMode == GDB_RET_CONTINUE))
                leave=true;
            else{
                // Idle, how about check the SDL events?
                core->idle();
            }

        } while (leave==false);

	// Reload wait counter
	if (runMode == GDB_RET_CONTINUE)
		wait = 100000000;
}

void GdbServer::SendPosition(int signo) {
    /* Send gdb PC, FP, SP */
    int bytes = 0;
    char reply[MAX_BUF+1];

    int pc = core->pc * 2;

    gdb_debug("Sending position [signo:%i]\n",signo);
    bytes = snprintf( reply, MAX_BUF, "T%02x", signo );

    /* SREG, SP & PC */
    snprintf( reply+bytes, MAX_BUF-bytes,
            "20:%02x;" "21:%02x%02x;" "22:%02x%02x%02x%02x;",
            ((int)(core->SREG)),
            core->SPL, core->SPH,
            pc & 0xff, (pc >> 8) & 0xff, (pc >> 16) & 0xff, (pc >> 24) & 0xff );

    gdb_send_reply(reply);
}
