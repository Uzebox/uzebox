/*
 *  Uzebox(tm) Packrom utility
 *  Copyright (C) 2008 Alec Bourque
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

/* This tool converts standard .HEX files into .UZE file. UZE files are
 * binary version of HEX files and includes a 512 bytes header. They
 * are intended to be simpler to load by the Uzebox bootloader.
 *
 * Revisions:
 * ----------
 * 1.3: 4/9/2012 - Fixed parse_hex_nibble bug (s >= 'a' && s <= 'f')
 * 1.4: 4/7/2023 - Added peripheral bit fields(devices supported, and emulator defaults)
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fstream>

#define HEADER_VERSION 1
#define VERSION_MAJOR 1
#define VERSION_MINOR 4
#define MAX_PROG_SIZE 61440 //65536-4096
#define HEADER_SIZE 512
#define MARKER_SIZE 6

#define PERIPHERAL_MOUSE 1
#define PERIPHERAL_KEYBOARD 2
#define PERIPHERAL_MULTITAP 4
#define PERIPHERAL_ESP8266 8

#define JAMMA_ROTATE_90 1
#define JAMMA_ROTATE_180 2
#define JAMMA_ROTATE_270 4
#define JAMMA_FLIP_H 8
#define JAMMA_FLIP_V 16
#define JAMMA_B0 32 //future use...
#define JAMMA_B1 64
#define JAMMA_B2 128

#if defined (_MSC_VER) && _MSC_VER >= 1400
// don't whine about sprintf and fopen.
// could switch to sprintf_s but that's not standard.
#pragma warning(disable:4996)
#endif

// 644 Overview: http://www.atmel.com/dyn/resources/prod_documents/doc2593.pdf
// AVR8 insn set: http://www.atmel.com/dyn/resources/prod_documents/doc0856.pdf
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;

#pragma pack( 1 )
typedef struct{
	/*Header fields*/
	u8 marker[MARKER_SIZE];	// 0x00-0x05: Marker 'UZEBOX'
	u8 version;		// 0x06: Header version
	u8 target;		// 0x07: Target UC; ATMega644 = 0, ATMega1284 = 1
	u32 progSize;		// 0x08-0x11: Occupied program memory size
	u16 year;		// 0x12-0x13: Release year
	u8 name[32];		// 0x14-0x2D: Name of program (Zero terminated)
	u8 author[32];		// 0x2E-0x4D: Name of author (Zero terminated)
	u8 icon[16*16];		// 0x4E-0x14D: 16 x 16 icon using the Uzebox palette
	u32 crc32;		// 0x14E-0x151: CRC of the program
	u8 psupport;		// 0x152: Peripherals supported(emulator info)
	u8 description[64];	// 0x153-0x192: Description (Zero terminated)
	u8 pdefault;		// 0x193: Peripherals default(emulator behavior on start)
	u8 jamma;		// 0x194: JAMMA Options(Rotation/Mirroring)
	u8 reserved[112];	// 0x195-0x1FF: Future use
}RomHeader;

union ROM{
	u8 progmem[MAX_PROG_SIZE+HEADER_SIZE];
	RomHeader header;
};
#pragma pack()

ROM rom;

/* crc_tab[] -- this crcTable is being build by chksum_crc32GenTab().
 *		so make sure, you call it before using the other
 *		functions!
 */
u32 crc_tab[256];

/* chksum_crc() -- to a given block, this one calculates the
 *				crc32-checksum until the length is
 *				reached. the crc32-checksum will be
 *				the result.
 */
u32 chksum_crc32 (unsigned char *block, unsigned int length)
{
   unsigned long crc;
   unsigned long i;

   crc = 0xFFFFFFFF;
   for (i = 0; i < length; i++)
   {
      crc = ((crc >> 8) & 0x00FFFFFF) ^ crc_tab[(crc ^ *block++) & 0xFF];
   }
   return (crc ^ 0xFFFFFFFF);
}

/* chksum_crc32gentab() --      to a global crc_tab[256], this one will
 *				calculate the crcTable for crc32-checksums.
 *				it is generated to the polynom [..]
 */

void chksum_crc32gentab ()
{
   unsigned long crc, poly;
   int i, j;

   poly = 0xEDB88320L;
   for (i = 0; i < 256; i++)
   {
      crc = i;
      for (j = 8; j > 0; j--)
      {
	 if (crc & 1)
	 {
	    crc = (crc >> 1) ^ poly;
	 }
	 else
	 {
	    crc >>= 1;
	 }
      }
      crc_tab[i] = crc;
   }
}

//copy strings without end of lines special characters
static void strcpy2(char* dest, char* src, int maxsize)
{
	u8 c;
	int i=0;
	while(i<maxsize){
	//strcpy((char*)rom.header.name,line+5);
		c=*src++;
		if(c<32) break;
		*dest++=c;
		i++;
	}
}

static inline int parse_hex_nibble(char s)
{
	if (s >= '0' && s <= '9')
		return s - '0';
	else if (s >= 'A' && s <= 'F')
		return s - 'A' + 10;
	else if (s >= 'a' && s <= 'f')
		return s - 'a' + 10;
	else
		return -1;
}

static int parse_hex_byte(const char *s)
{
	return (parse_hex_nibble(s[0])<<4) | parse_hex_nibble(s[1]);
}

static int parse_hex_word(const char *s)
{
	return (parse_hex_nibble(s[0])<<12) | (parse_hex_nibble(s[1]) << 8) |
		(parse_hex_nibble(s[2])<<4) | parse_hex_nibble(s[3]);
}

bool load_hex(const char *in_filename)
{
	
	//http://en.wikipedia.org/wiki/.hex

	//(I've added the spaces for clarity, they don't exist in the real files)
	//:10 65B0 00 661F771F881F991F1A9469F760957095 59
	//:10 65C0 00 809590959B01AC01BD01CF010895F894 91
	//:02 65D0 00 FFCF FB
	//:02 65D2 00 0100 C6
	//:00 0000 01 FF [EOF marker]

	//First field is the byte count. Second field is the 16-bit address. Third field is the record type; 
	//00 is data, 01 is EOF.	For record type zero, next "wide" field is the actual data, followed by a 
	//checksum.
	u16 progmemLast=0;
	char line[128];
	int lineNumber = 1;
	bool warned = false;

	FILE *in_file = fopen(in_filename,"r");


	if (!in_file) return false;

	// Set entire memory out first in case new image is shorter than last one (0xff == NOP)
	memset(rom.progmem+HEADER_SIZE, 0xff , MAX_PROG_SIZE);

	while (fgets(line, sizeof(line), in_file) && line[0]==':')
	{
		int bytes = parse_hex_byte(line+1);
		int addr = parse_hex_word(line+3);
		int recordType = parse_hex_byte(line+7);
		if (recordType == 0)
		{
			char *lp = line + 9;
			while (bytes > 0)
			{
                // Check if it went past the 60KB
                if(addr < MAX_PROG_SIZE)
                {
                    rom.progmem[addr+HEADER_SIZE] = parse_hex_byte(lp);
                    addr ++;
                    if (addr > progmemLast){
                        progmemLast = addr;
                    }
                }
                else if(!warned)
                {
                    fprintf(stderr, "\n\t***Warning***: The hex file has instructions after\n "
                        "\tthe 60KB mark, which are being ignored and is, probably, incompatible with the\n"
                        "\tbootloader. Note: This might not be a problem if your hex is a dump from the\n "
                        "\tchip's flash.\n\n");

                    warned = true;
                }

				lp += 2;
				bytes -= 1;
			}
		}
		else if (recordType == 1)
		{
			break;
		}
		else
			fprintf(stderr,"ignoring unknown record type %d in line %d of %s\n",recordType,lineNumber,in_filename);

		++lineNumber;
	}
    
	rom.header.progSize=progmemLast;

	fclose(in_file);

	return true;
}

bool load_uze(const char *in_filename){//implies patching an existing .uze file
	FILE *in_file = fopen(in_filename, "r");
	if (!in_file) return false;

	memset(rom.progmem+HEADER_SIZE, 0xff , MAX_PROG_SIZE);
	int addr = 0;
	while(!feof(in_file)){
		if(addr < HEADER_SIZE){//eat old header data
			fgetc(in_file);
			addr++;
			continue;
		}

		if(addr < MAX_PROG_SIZE+HEADER_SIZE)
			rom.progmem[addr++] = fgetc(in_file);
		else{
			fprintf(stderr, "\n\t***Warning***: The hex file has instructions after\n"
			"\tthe 60KB mark, which are being ignored and is, probably, incompatible with the\n"
			"\tbootloader. Note: This might not be a problem if your hex is a dump from the\n "
			"\tchip's flash.\n\n");
                        break;
		}
	}
	fclose(in_file);
	rom.header.progSize = addr-HEADER_SIZE-1;
	return true;
}

int main(int argc,char **argv)
{

	if (argc!=4)
	{
		fprintf(stderr,"%s ver %i.%i -- Packs a HEX file to binary and adds a header.\n",argv[0],VERSION_MAJOR,VERSION_MINOR);
		fprintf(stderr,"usage(create): %s <input.hex> <ouput.uze> <gameinfo.properties>\n",argv[0]);
		fprintf(stderr,"creation example: %s halloween.hex halloween.uze gameinfo.properties\n",argv[0]);
		fprintf(stderr,"usage(patch): %s <input.uze> <output.uze> <gameinfo.properties>\n",argv[0]);
		fprintf(stderr,"patching example: %s halloween.uze hwpatched.uze gameinfo.properties\n",argv[0]);
		return 1;
	}

	if(argv[1][0] == '\0'){
		fprintf(stderr,"Error: no input file specified\n");
		return -1;
	}
	if(argv[2][0] == '\0'){
		fprintf(stderr,"Error: no output file specified\n");
		return -1;
	}
	if(argv[3][0] == '\0'){
		fprintf(stderr,"Error: no properties file specified\n");
		return -1;
	}

	int mode = 0;//default create mode
	if(strstr(argv[1], ".hex") == NULL){//if the user inputs a .uze file, assumed the intent is to patch with the given properties file(skip hex load)
		mode = 1;//patch mode
		fprintf(stderr, "\tPatching file: [%s]->[%s]\n", argv[1], argv[2]);
	}else//otherwise create new .uze file
		fprintf(stderr,"\tPacking file: [%s]->[%s]\n", argv[1], argv[2]);

	chksum_crc32gentab();
	memset((void *)&rom.header, 0x00 , HEADER_SIZE);

	//parse the games properties file
	FILE *file = fopen ( argv[3], "r" );
	if ( file )
	{
		char line [128];
		while ( fgets ( line, sizeof(line), file ) != NULL ) /* read a line */
		{
			if(!strncmp(line,"#",1)){
				continue;//ignore comment line

			}else if(!strncmp(line,"name=",5)){
				strcpy2((char*)rom.header.name,line+5,sizeof(rom.header.name));
				fprintf(stderr,"\tGame Name: %s\n", rom.header.name);

			}else if(!strncmp(line,"desc=",5)){//this is allowed to contain '\n'?
				strcpy2((char*)rom.header.description,line+5,sizeof(rom.header.description));
				fprintf(stderr,"\tDescription: %s\n", rom.header.description);

			}else if(!strncmp(line,"author=",7)){
				strcpy2((char*)rom.header.author,line+7,sizeof(rom.header.author));
				fprintf(stderr,"\tAuthor: %s\n", rom.header.author);

			}else if(!strncmp(line,"year=",5)){
				rom.header.year=(u16) strtoul(line+5,NULL,10);
				fprintf(stderr,"\tYear: %i\n", rom.header.year);

			}else if(!strncmp(line,"mouse=support",13)){
				rom.header.psupport |= PERIPHERAL_MOUSE;
				fprintf(stderr,"\tMouse: Supported\n");

			}else if(!strncmp(line,"keyboard=support",16)){
				rom.header.psupport |= PERIPHERAL_KEYBOARD;
				fprintf(stderr,"\tKeyboard: Supported\n");

			}else if(!strncmp(line,"multitap=support",16)){
				rom.header.psupport |= PERIPHERAL_MULTITAP;
				fprintf(stderr,"\tMultitap: Supported\n");

			}else if(!strncmp(line,"esp8266=support",15)){
				rom.header.psupport |= PERIPHERAL_KEYBOARD;
				fprintf(stderr,"\tESP8266: Supported\n");

			}else if(!strncmp(line,"mouse=default",13)){
				rom.header.psupport |= PERIPHERAL_MOUSE;
				rom.header.pdefault |= PERIPHERAL_MOUSE;
				fprintf(stderr,"\tMouse: Default\n");

			}else if(!strncmp(line,"keyboard=default",16)){
				rom.header.psupport |= PERIPHERAL_KEYBOARD;
				rom.header.pdefault |= PERIPHERAL_KEYBOARD;
				fprintf(stderr,"\tKeyboard: Default\n");

			}else if(!strncmp(line,"multitap=default",16)){
				rom.header.psupport |= PERIPHERAL_MULTITAP;
				rom.header.pdefault |= PERIPHERAL_MULTITAP;
				fprintf(stderr,"\tMultitap: Default\n");

			}else if(!strncmp(line,"esp8266=default",15)){
				rom.header.psupport |= PERIPHERAL_ESP8266;
				rom.header.pdefault |= PERIPHERAL_ESP8266;
				fprintf(stderr,"\tESP8266: Default\n");

			}else if(!strncmp(line,"JAMMA=rotate90",14)){
				rom.header.jamma |= JAMMA_ROTATE_90;
				fprintf(stderr,"\tJAMMA: Rotate 90\n");

			}else if(!strncmp(line,"JAMMA=rotate180",15)){
				rom.header.jamma |= JAMMA_ROTATE_180;
				fprintf(stderr,"\tJAMMA: Rotate 180\n");

			}else if(!strncmp(line,"JAMMA=rotate270",15)){
				rom.header.jamma |= JAMMA_ROTATE_270;
				fprintf(stderr,"\tJAMMA: Rotate 270\n");

			}else if(!strncmp(line,"JAMMA=flipH",11)){
				rom.header.jamma |= JAMMA_FLIP_H;
				fprintf(stderr,"\tJAMMA: Flip Horizontal\n");

			}else if(!strncmp(line,"JAMMA=flipV",11)){
				rom.header.jamma |= JAMMA_FLIP_H;
				fprintf(stderr,"\tJAMMA: Flip Vertical\n");

			}else{//terminate string at '\r' or '\n', and compare to remaining possible arguments
			
				for(char *p=line+5; *p;++p){
					if(*p < ' '){
						*p=0;
						break;
					}
				}

				if(!strncmp(line,"icon=",5)){
					// read 24bits/32bits raw RGB 16x16 icon (easy to export from Gimp as .raw)
					// ordered row-major, left to right, top to bottom
					FILE *iconfile = fopen(line+5, "rb");
					fprintf(stderr,"\tIcon: %s\n", line+5);
					if(iconfile){
						fseek(iconfile, 0, SEEK_END);
						int size = ftell(iconfile);
						fseek(iconfile, 0, SEEK_SET);

						for(int i=0,y=16; y--;){
							fprintf(stderr, "\t\t");
							for(int x=16; x--; ++i){
								u8 rgb[3];
								if(fread(&rgb, 3, 1, iconfile) < 1){
									fprintf(stderr,"\tIcon file read error\033[0m\n");
									y=0;
									break;
								}
								if(size == 1024){ // 768=RGB, 1024=RGBA, discard alpha
									fseek(iconfile, 1, SEEK_CUR);
								}
								rgb[0] &= 0xE0;
								rgb[1] &= 0xE0;
								rgb[2] &= 0xC0;
								rom.header.icon[i] = (((rgb[0]) >> 5) & 7) | (((rgb[1]) >> 2) & 0x38) | ((rgb[2]) & 0xC0);
								fprintf(stderr, "\033[38;2;%d;%d;%d;48;2;%d;%d;%dm##", rgb[0] & 255, rgb[1] & 255, rgb[2] & 255, rgb[0] & 255, rgb[1] & 255, rgb[2] & 255);
							}
							fprintf(stderr, "\033[0m\n");
						}

						fclose(iconfile);
					}else{
						fprintf(stderr,"\tIcon file error\n");
						return 1;
					}
				}else{
					fprintf(stderr,"\tUnknown Option [%s]\n", line);
				}
			}
		}
		fclose (file);
	}
	else
	{
		fprintf(stderr,"Can't read properties file: %s \n",argv[3]);
		return 1;
	}

	if(mode == 0){//create new .uze
		if(!load_hex(argv[1])){
			fprintf(stderr,"Could not process HEX file.\n");
			return 1;
		}
	}else{//patch existing .uze file
		if(!load_uze(argv[1])){
			fprintf(stderr,"Could not process UZE file.\n");
			return 1;
		}
	}

	memcpy(rom.header.marker,"UZEBOX",MARKER_SIZE);
	rom.header.version=HEADER_VERSION;
	rom.header.target=0;
	rom.header.crc32=chksum_crc32(rom.progmem+HEADER_SIZE, rom.header.progSize);

	fprintf(stderr,"\tCRC32: 0x%lx\n", (long unsigned int) rom.header.crc32);
	fprintf(stderr,"\tProgram size: %li \n", (long unsigned int) rom.header.progSize);

	//write the output file
	FILE *out_file = fopen(argv[2],"wb");
	if(!fwrite(&rom.progmem,rom.header.progSize+HEADER_SIZE,1,out_file)){

		fprintf(stderr,"Could not process output file.\n");
		return 1;
	}



	fclose(out_file);

	return 0;
}
