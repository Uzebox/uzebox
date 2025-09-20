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
 * 1.4: 4/8/2023 - Added patching abillity
 * 1.5: 12/15/2024 - Added extract ability, and dependency file support(SD file append)
 * 1.6: 9/19/2025 - Added SPI RAM banks count, removed spiram=default, use spiram=<banks>
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fstream>

#define HEADER_VERSION 1
#define VERSION_MAJOR 1
#define VERSION_MINOR 5
#define MAX_PROG_SIZE 61440 //65536-4096
#define HEADER_SIZE 512
#define MARKER_SIZE 6

#define PERIPHERAL_MOUSE		1
#define PERIPHERAL_KEYBOARD		2
#define PERIPHERAL_LIGHTGUN		4
#define PERIPHERAL_MULTITAP		8
#define PERIPHERAL_SPIRAM		16
#define PERIPHERAL_ESP8266		32
#define PERIPHERAL_FUTURE0		64
#define PERIPHERAL_FUTURE1		128

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
	u8 version;				// 0x06: Header version
	u8 target;				// 0x07: Target UC; ATMega644 = 0, ATMega1284 = 1
	u32 progSize;			// 0x08-0x0B: Occupied program memory size(ignoring any attached dependency data)
	u16 year;				// 0x1C-0x1D: Release year
	u8 name[32];			// 0x1E-0x2D: Name of program (Zero terminated)
	u8 author[32];			// 0x2E-0x4D: Name of author (Zero terminated)
	u8 icon[16*16];			// 0x4E-0x14D: 16 x 16 icon using the Uzebox palette
	u32 crc32;				// 0x14E-0x151: CRC of the program
	u8 psupport;			// 0x152: Peripherals supported(emulator info)
	u8 description[64];		// 0x153-0x192: Description (Zero terminated)
	u8 pdefault;			// 0x193: Peripherals default(emulator behavior on start)
	u8 jamma;				// 0x194: JAMMA Options(Rotation/Mirroring)
	u32 dependencyLen;		// 0x195-0x198: Length of appended dependency/data file
	u8 spiRamBanks;			// 0x199: Ideal SPI RAM size(in 64K banks, emulation behavior on start)
	u8 reserved[107];		// 0x19A-0x1FF: Future use
}RomHeader;

union ROM{
	u8 progmem[MAX_PROG_SIZE+HEADER_SIZE];
	RomHeader header;
};
#pragma pack()

ROM rom;

char dependencyFile[256];

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
u32 chksum_crc32 (unsigned char *block, unsigned int length){
	unsigned long crc;
	unsigned long i;

	crc = 0xFFFFFFFF;
	for(i = 0; i < length; i++){
		crc = ((crc >> 8) & 0x00FFFFFF) ^ crc_tab[(crc ^ *block++) & 0xFF];
	}
	return (crc ^ 0xFFFFFFFF);
}

/* chksum_crc32gentab() --      to a global crc_tab[256], this one will
 *				calculate the crcTable for crc32-checksums.
 *				it is generated to the polynom [..]
 */

void chksum_crc32gentab (){
	unsigned long crc, poly;
	int i, j;

	poly = 0xEDB88320L;
	for(i = 0; i < 256; i++){
		crc = i;
		for (j = 8; j > 0; j--){
			if (crc & 1){
				crc = (crc >> 1) ^ poly;
			}else{
				crc >>= 1;
			}
		}
		crc_tab[i] = crc;
	}
}

//copy strings without end of lines special characters
static void strcpy2(char* dest, char* src, int maxsize){
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

static inline int parse_hex_nibble(char s){
	if (s >= '0' && s <= '9')
		return s - '0';
	else if (s >= 'A' && s <= 'F')
		return s - 'A' + 10;
	else if (s >= 'a' && s <= 'f')
		return s - 'a' + 10;
	else
		return -1;
}

static int parse_hex_byte(const char *s){
	return (parse_hex_nibble(s[0])<<4) | parse_hex_nibble(s[1]);
}

static int parse_hex_word(const char *s){
	return (parse_hex_nibble(s[0])<<12) | (parse_hex_nibble(s[1]) << 8) |
		(parse_hex_nibble(s[2])<<4) | parse_hex_nibble(s[3]);
}

bool load_hex(const char *in_filename){
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
	if(!in_file)
		return false;

	// Set entire memory out first in case new image is shorter than last one (0xff == NOP)
	memset(rom.progmem+HEADER_SIZE, 0xff , MAX_PROG_SIZE);

	while(fgets(line, sizeof(line), in_file) && line[0]==':'){
		int bytes = parse_hex_byte(line+1);
		int addr = parse_hex_word(line+3);
		int recordType = parse_hex_byte(line+7);
		if(recordType == 0){
			char *lp = line + 9;
			while(bytes > 0){
				if(addr < MAX_PROG_SIZE){// Check if it went past the 60KB
					rom.progmem[addr+HEADER_SIZE] = parse_hex_byte(lp);
					addr ++;
					if(addr > progmemLast){
						progmemLast = addr;
					}
				}else if(!warned){
					fprintf(stderr, "\n\t***Warning***: The hex file has instructions after\n "
					"\tthe 60KB mark, which are being ignored and is, probably, incompatible with the\n"
					"\tbootloader. Note: This might not be a problem if your hex is a dump from the\n "
					"\tchip's flash.\n\n");

					warned = true;
				}

				lp += 2;
				bytes -= 1;
			}
		}else if (recordType == 1){
			break;
		}else
			fprintf(stderr,"ignoring unknown record type %d in line %d of %s\n",recordType,lineNumber,in_filename);

		++lineNumber;
	}
    
	rom.header.progSize = progmemLast;
	fclose(in_file);
	return true;
}

bool load_uze(const char *in_filename){//implies patching an existing .uze file
	FILE *in_file = fopen(in_filename, "r");
	if(!in_file)
		return false;

	memset(rom.progmem+HEADER_SIZE, 0xff, MAX_PROG_SIZE);
	u32 oldProgSize = 0;
	int addr = 0;
	while(!feof(in_file)){
		if(addr < HEADER_SIZE){//eat old header data
			unsigned char t = fgetc(in_file);
			if(addr == 0x8)
				oldProgSize = t;
			else if(addr == 0x9)
				oldProgSize |= (t<<8);
			else if(addr == 0xA)
				oldProgSize |= (t<<16);
			else if(addr == 0xB)
				oldProgSize |= (t<<24);

			addr++;
			continue;
		}
		if(oldProgSize < 64 || oldProgSize > (MAX_PROG_SIZE+HEADER_SIZE)){
			fprintf(stderr,"ERROR: got bad progSize %d\n", oldProgSize);
			return false;
		}

		if((u32)addr >= (u32)oldProgSize+HEADER_SIZE){
			fprintf(stderr,"\tOmmitting after %d original program bytes(assumed as old padding).\n\n", oldProgSize);
			break;
		}else if(addr < MAX_PROG_SIZE+HEADER_SIZE){
			rom.progmem[addr++] = fgetc(in_file);
		}else
			break;
	}
	fclose(in_file);
	rom.header.progSize = addr-HEADER_SIZE;
	return true;
}

bool extract_hex(
const char *uze_filename,
const char *out_filename1, const char *out_filename2, const char *out_filename3, const char *out_filename4){

	fprintf(stderr, "\n\tExtracting ROM: [%s]->[%s][%s][%s][%s]\n", uze_filename, out_filename1, out_filename2, out_filename3, out_filename4);
	FILE *uze_file = fopen(uze_filename, "rb");//load all data from UZE file
	if(uze_file == NULL){
		fprintf(stderr, "\tError: Failed to open uze input file [%s]\n", uze_filename);
		return false;
	}
	FILE *out_file1 = fopen(out_filename1, "w");//hex
	if(out_file1 == NULL){
		fprintf(stderr, "\tError: Failed to open hex output file [%s]\n", out_filename1);
		return false;
	}
	FILE *out_file2 = fopen(out_filename2, "w");//info
	if(out_file2 == NULL){
		fprintf(stderr, "\tError: Failed to open info output file [%s]\n", out_filename2);
		return false;
	}

	if(!fread(&rom.header, HEADER_SIZE, 1, uze_file)){
		fprintf(stderr, "\tError: Failed to read uze header\n");
		return false;
	}

	long int progSize = rom.header.progSize;
	if(progSize < 1 || progSize > MAX_PROG_SIZE)
		fprintf(stderr, "\tWarning: Bad ROM size %d, possible file corruption\n", rom.header.progSize);
	
	FILE *out_file3 = NULL;
	if(rom.header.dependencyLen){//check for Dependency
		out_file3 = fopen(out_filename3, "wb");
		if(out_file3 == NULL){
			fprintf(stderr, "\tError: Failed to open dependency output file [%s]\n", out_filename3);
			return false;
		}
	}
	FILE *out_file4 = NULL;
	for(u32 i=0; i<16*16; i++){//check for Icon
		if(rom.header.icon[i] != 0){
			out_file4 = fopen(out_filename4, "wb");
			if(out_file4 == NULL){
				fprintf(stderr, "\tError: Failed to open icon output file [%s]\n", out_filename4);
				return false;
			}
			break;
		}
	}
	
	if(!fread(rom.progmem+HEADER_SIZE, progSize, 1, uze_file)){
		fprintf(stderr, "\tError: Failed to read uze progmem\n");
		return false;
	}
	
	if(rom.header.dependencyLen){//extract and write Dependency(if present)
		fseek(uze_file, MAX_PROG_SIZE+HEADER_SIZE, SEEK_SET);//move to end of padded ROM data
		for(u32 i=0; i<rom.header.dependencyLen; i++){
			if(feof(uze_file))
				break;
			fputc(fgetc(uze_file), out_file3);
		}
		if(ferror(uze_file) || ferror(out_file3)){
			fprintf(stderr, "\tError: Failed while writing dependency output [%s]\n", out_filename3);
			return false;
		}
		fclose(out_file3);

	}	
	fclose(uze_file);

	for(u32 i=0; i<rom.header.progSize; i+=16){//write HEX
		u16 checksum = 0;
		u32 write_bytes = rom.header.progSize-i;
		if(write_bytes > 16)
			write_bytes = 16;
		fprintf(out_file1, ":%02X", write_bytes);//marker, length
		checksum += write_bytes;
		fprintf(out_file1, "%04X", i);//offset
		for(int j=0; j<4; j++)
			checksum += (i>>(j*8))&0xFF;

		fprintf(out_file1, "00");//record type 0
		for(u32 j=0; j<write_bytes; j++){//write out data bytes
			fprintf(out_file1, "%02X", rom.progmem[HEADER_SIZE+i+j]);
			checksum += rom.progmem[HEADER_SIZE+i+j];
		}
		checksum = ~checksum;//convert to 1s complement
		checksum++;//convert to 2s complement
		checksum &= 0xFF;
		fprintf(out_file1,"%02X\r\n", checksum);
	}
	fprintf(out_file1, ":00000001FF\r\n");//marker,length 0, offset 0, record type EOF
	fclose(out_file1);
	//extract and write Header data
	if(rom.header.name[0])
		fprintf(out_file2, "name=%s\n", (char *)(rom.header.name));
	if(rom.header.year)
		fprintf(out_file2, "year=%d\n", (u32)(rom.header.year));
	if(rom.header.author[0])
		fprintf(out_file2, "author=%s\n", (char *)(rom.header.author));
	if(out_file4 != NULL)//icon present?
		fprintf(out_file2, "icon=%s\n", out_filename4);
	if(out_file3 != NULL)//dependency present?
		fprintf(out_file2, "dependency=%s\n", out_filename3);

	fprintf(out_file2, "spiram=%d\n", (u32)(rom.header.spiRamBanks));
	if(rom.header.spiRamBanks)
		rom.header.psupport |= PERIPHERAL_SPIRAM;//mark as supported if banks were specified(no default for SPI RAM, only bank count)

	u8 psupport = rom.header.psupport;//supported peripherals
	if(psupport & PERIPHERAL_MOUSE)
		fprintf(out_file2, "mouse=supported\n");
	if(psupport & PERIPHERAL_KEYBOARD)
		fprintf(out_file2, "keybord=supported\n");
	if(psupport & PERIPHERAL_LIGHTGUN)
		fprintf(out_file2, "lightgun=supported\n");
	if(psupport & PERIPHERAL_MULTITAP)
		fprintf(out_file2, "multitap=supported\n");
	if(psupport & PERIPHERAL_ESP8266)
		fprintf(out_file2, "esp8266=supported\n");
	u8 pdefault = rom.header.pdefault;//default peripherals
	if(pdefault & PERIPHERAL_MOUSE)
		fprintf(out_file2, "mouse=default\n");
	if(pdefault & PERIPHERAL_KEYBOARD)
		fprintf(out_file2, "keybord=default\n");
	if(pdefault & PERIPHERAL_LIGHTGUN)
		fprintf(out_file2, "lightgun=default\n");
	if(pdefault & PERIPHERAL_MULTITAP)
		fprintf(out_file2, "multitap=default\n");
	if(pdefault & PERIPHERAL_ESP8266)
		fprintf(out_file2, "esp8266=default\n");
	u8 jamma = rom.header.jamma;//JAMMA options
	if(jamma & JAMMA_ROTATE_90)
		fprintf(out_file2, "jamma=rotate90\n");
	if(jamma & JAMMA_ROTATE_180)
		fprintf(out_file2, "jamma=rotate180\n");
	if(jamma & JAMMA_ROTATE_270)
		fprintf(out_file2, "jamma=rotate270\n");
	if(jamma & JAMMA_FLIP_H)
		fprintf(out_file2, "jamma=flipH\n");
	if(jamma & JAMMA_FLIP_V)
		fprintf(out_file2, "jamma=flipV\n");
	if(rom.header.description[0]){
		fprintf(out_file2, "description=%s", (char *)(rom.header.description));
	}
	if(ferror(out_file2)){
		fprintf(stderr, "\tError: Failed while writing info [%s]\n", out_filename2);
		return false;
	}
	fclose(out_file2);

	//extract and write Icon data(if present)
	if(out_file4 != NULL){
		for(u32 i=0; i<16*16; i++){
				fputc(((rom.header.icon[i]&0b00000111)<<5), out_file4);//R
				fputc(((rom.header.icon[i]&0b00111000)<<2), out_file4);//G
				fputc(((rom.header.icon[i]&0b11000000)<<0), out_file4);//B
		}
		fclose(out_file4);
	}
	return true;
}

void DisplayUsage(){
	fprintf(stderr,"packrom ver %i.%i\n\n",VERSION_MAJOR,VERSION_MINOR);
	fprintf(stderr,"Packs a HEX file to UZE binary and adds INFO header(ICON and DATA if in INFO).\n");
	fprintf(stderr,"Or Patches an existing UZE binary with new INFO header(ICON and DATA if in INFO).\n");
	fprintf(stderr,"Or Extract a HEX and INFO(and ICON and DATA if present) from existing UZE file.\n\n");
	fprintf(stderr,"Create UZE: packrom <input.hex> <ouput.uze> <gameinfo.properties>\n");
	fprintf(stderr,"\tcreation example: \x1b[1;31mpackrom game.hex game.uze gameinfo.properties\x1b[0;0m\n");
	fprintf(stderr,"\tcreation notes: to include ICON and/or DATA dependency, encode path in INFO file\n\n");
	fprintf(stderr,"Patch UZE: packrom <input.uze> <output.uze> <newinfo.properties>\n");
	fprintf(stderr,"\tpatching example: \x1b[1;31mpackrom game.uze patched.uze newinfo.properties\x1b[0;0m\n");
	fprintf(stderr,"\tpatching notes: any specified ICON or DATA dependency in the INFO file will be packed\n\n");
	fprintf(stderr,"Extract HEX: packrom <input.uze> <output.hex> <opt-output.properties> <opt-output.dat> <opt-icon-out.raw>\n");
	fprintf(stderr,"\textract example: \x1b[1;31mpackrom game.uze output.hex info.properties dependency.dat icon.raw\x1b[0;0m\n");
	fprintf(stderr,"\textract example: \x1b[1;31mpackrom game.uze output.hex\x1b[0;0m\n");
	fprintf(stderr,"\textract notes: any unspecified name will output as extracted.properties, extraced.dat, or extracted.raw\n\n");
}

int main(int argc,char **argv){

	if(argc < 3){
		DisplayUsage();
		return -1;
	}

	if(strstr(argv[1], ".uze") != NULL && strstr(argv[2], ".hex") != NULL){//extract the ROM image to an output .uze and .info file(and data/icon files if present)
		if(!extract_hex(argv[1], argv[2], ((argc>3)?argv[3]:"extracted.properties"), ((argc>4)?argv[4]:"extracted.dat"),((argc>5)?argv[5]:"extracted.raw")))
			return -1;
		return 0;
	}
	int mode = 0;//default create mode	
	if(strstr(argv[1], ".uze") != NULL){//if the user inputs a .uze file, assumed the intent is to patch with the given properties file(skip hex load)
		if(argc < 4){
			fprintf(stderr,"\n\tPatch mode requires: [input.uze] [output.uze] [patch.properties]\n\n");
			DisplayUsage();
			return -1;
		}
		mode = 1;//patch mode
		fprintf(stderr, "\n\tPatching file: [%s]->[%s] using [%s]\n", argv[1], argv[2], argv[3]);
	}else{//otherwise create new .uze file
		if(argc < 4){
			fprintf(stderr,"\n\tPack mode requires: [input.hex] [output.uze] [info.properties]\n\n");
			DisplayUsage();
			return 01;
		}	
		fprintf(stderr,"\n\tPacking file: [%s]->[%s]\n", argv[1], argv[2]);
	}
	chksum_crc32gentab();
	memset((void *)&rom.header, 0x00 , HEADER_SIZE);

	//parse the games properties file
	FILE *file = fopen (argv[3], "r");
	if(file){
		char line [128];
		while(fgets( line, sizeof(line), file ) != NULL){ /* read a line */
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

			}else if(!strncmp(line,"lightgun=support",16)){
				rom.header.psupport |= PERIPHERAL_LIGHTGUN;
				fprintf(stderr,"\tLightgun: Supported\n");

			}else if(!strncmp(line,"multitap=support",16)){
				rom.header.psupport |= PERIPHERAL_MULTITAP;
				fprintf(stderr,"\tMultitap: Supported\n");

			}else if(!strncmp(line,"esp8266=support",15)){
				rom.header.psupport |= PERIPHERAL_ESP8266;
				fprintf(stderr,"\tESP8266: Supported\n");

			}else if(!strncmp(line,"mouse=default",13)){
				rom.header.psupport |= PERIPHERAL_MOUSE;
				rom.header.pdefault |= PERIPHERAL_MOUSE;
				fprintf(stderr,"\tMouse: Default\n");

			}else if(!strncmp(line,"keyboard=default",16)){
				rom.header.psupport |= PERIPHERAL_KEYBOARD;
				rom.header.pdefault |= PERIPHERAL_KEYBOARD;
				fprintf(stderr,"\tKeyboard: Default\n");

			}else if(!strncmp(line,"lightgun=default",16)){
				rom.header.psupport |= PERIPHERAL_LIGHTGUN;
				rom.header.pdefault |= PERIPHERAL_LIGHTGUN;
				fprintf(stderr,"\tLightgun: Default\n");

			}else if(!strncmp(line,"multitap=default",16)){
				rom.header.psupport |= PERIPHERAL_MULTITAP;
				rom.header.pdefault |= PERIPHERAL_MULTITAP;
				fprintf(stderr,"\tMultitap: Default\n");

			}else if(!strncmp(line,"esp8266=default",15)){
				rom.header.psupport |= PERIPHERAL_ESP8266;
				rom.header.pdefault |= PERIPHERAL_ESP8266;
				fprintf(stderr,"\tESP8266: Default\n");

			}else if(!strncmp(line,"spiram=",7)){//bank count
				rom.header.spiRamBanks=(u8) strtoul(line+7,NULL,10);
				if(!rom.header.spiRamBanks && line[7] != 0){//got something not equivalent to a number?
					fprintf(stderr, "Warning: got [%s], unrecognized argument[%s], ignoring.\n", line, line+7);
				}else{
					u32 t = rom.header.spiRamBanks;
					t /= 2;
					t *= 2;
					if(rom.header.spiRamBanks != 1 && t != rom.header.spiRamBanks){
						fprintf(stderr, "Warning: got [%s], SPI RAM banks must be 1, or a power of 2; ignoring.\n", line);
					}else{
						fprintf(stderr,"\tSPI RAM banks: %i\n", rom.header.spiRamBanks);
					}
				}

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
					// read 24bits/32bits raw RGB 16x16 icon(Uzebox only colors, export as raw: "Standard (R, G, B)","B, G, R , X (BMP stye)"
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
									fprintf(stderr,"\tError: Failed to read Icon file\033[0m\n");
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
						fprintf(stderr,"\tError: Failed to open Icon file\n");
						return 1;
					}
				}else if(!strncmp(line,"dependency=",11)){
						// append a data file, first verify it exists and record the length
						FILE *datafile = fopen(line+11, "rb");
						if(datafile){
							fseek(datafile, 0L, SEEK_END);
							int size = ftell(datafile);
							rom.header.dependencyLen = (u32)size;
							fclose(datafile);
							strncpy(dependencyFile,line+11,sizeof(dependencyFile)-2);
						}else{
							fprintf(stderr,"\tError: Failed to determine dependency length\n");
							return 1;
						}
				}else{
					fprintf(stderr,"\tUnknown Option [%s]\n", line);
				}
			}
		}
		fclose (file);
	}else{
		fprintf(stderr,"\tError Can't read properties file: %s \n",argv[3]);
		return 1;
	}

	if(mode == 0){//create new .uze
		if(!load_hex(argv[1])){
			fprintf(stderr,"\tError: Could not process HEX file.\n");
			return 1;
		}
	}else{//patch existing .uze file
		if(!load_uze(argv[1])){
			fprintf(stderr,"\tError: Could not process UZE file.\n");
			return 1;
		}
	}

	memcpy(rom.header.marker,"UZEBOX",MARKER_SIZE);
	rom.header.version = HEADER_VERSION;
	rom.header.target = 0;
	rom.header.crc32 = chksum_crc32(rom.progmem+HEADER_SIZE, rom.header.progSize);

	fprintf(stderr,"\tCRC32: 0x%lx\n", (long unsigned int) rom.header.crc32);
	fprintf(stderr,"\tProgram size: %li \n", (long unsigned int) rom.header.progSize);

	//write the output file
	FILE *out_file = fopen(argv[2],"wb");
	if(out_file == NULL){
		fprintf(stderr,"\tError: Failed to open output file [%s]\n", argv[2]);
		return 1;
	}
	long unsigned int out_size = rom.header.progSize+HEADER_SIZE;
	if(!fwrite(&rom.progmem,out_size,1,out_file)){
		fprintf(stderr,"\tError: Could not process output file: %s\n", argv[2]);
		return 1;
	}

	if(rom.header.dependencyLen){//append dependency/data file
		FILE *datafile = fopen(dependencyFile, "rb");
		if(datafile){
			fprintf(stderr,"\tPadding ROM to 60K\n");
			for(int i=rom.header.progSize+HEADER_SIZE; i<MAX_PROG_SIZE+HEADER_SIZE; i++){
				fputc(0xFF,out_file);
			}
			fprintf(stderr,"\tDependency file: %s\n", dependencyFile);
			fprintf(stderr,"\tDependency size: %d\n", rom.header.dependencyLen);
			fprintf(stderr,"\tTotal Size(ROM+Header+Dependency): %d\n", MAX_PROG_SIZE+HEADER_SIZE+rom.header.dependencyLen);
			while(!feof(datafile)){
				fputc(fgetc(datafile), out_file); 
			}
			if(ferror(datafile)){
				fprintf(stderr,"\tError: Failed while appending dependency data\n");
				return 1;
			}
		}else{
			fprintf(stderr,"\tError: Could not process dependency file: %s\n", dependencyFile);
			return 1;
		}
	}

	fclose(out_file);

	return 0;
}
