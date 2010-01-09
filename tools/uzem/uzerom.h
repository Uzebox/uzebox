/*
	Copyright (c) 2009 Eric Anderton, Alec Bourque
        
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef UZEROM_H

#define HEADER_VERSION 1
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define MAX_PROG_SIZE 61440 //65536-4096
#define HEADER_SIZE 512

#if defined(__GNUC__)
    #define ALIGN1 __attribute__ ((aligned(1)))
#else
    #define ALIGN1
#endif

struct RomHeader{
    //Header fields (512 bytes)
    unsigned char marker[6];	//'UZEBOX'
    unsigned char version;		//header version
    unsigned char target;		//AVR target (ATmega644=0, ATmega1284=1)
    unsigned long progSize;	    //program memory size in bytes
    unsigned short year;
    unsigned char name[32];
    unsigned char author[32];
    unsigned char icon[16*16];
    unsigned long crc32;
    unsigned char mouse;
    unsigned char reserved[178];
} ALIGN1;

/*
    isUzeromFile - returns true if the file is indeed an .uze file
*/
bool isUzeromFile(char* in_filename);

/*
    readUzeImage - reads an .uze file into the header and buffer structures provided.
*/
bool loadUzeImage(char* in_filename,RomHeader *header,unsigned char *buffer);

/*
    load_hex - loads a hex image into the buffer provided, and provides the number of bytes read.
*/
bool loadHex(const char *in_filename,unsigned char *buffer,unsigned int *bytesRead = 0);

#endif
