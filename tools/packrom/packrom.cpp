#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#define HEADER_VERSION 1
#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define MAX_PROG_SIZE 61440 //65536-4096
#define HEADER_SIZE 512
#define MARKER_SIZE 6

#if defined (_MSC_VER) && _MSC_VER >= 1400
// don't whine about sprintf and fopen.
// could switch to sprintf_s but that's not standard.
#pragma warning(disable:4996)
#endif

// 644 Overview: http://www.atmel.com/dyn/resources/prod_documents/doc2593.pdf
// AVR8 insn set: http://www.atmel.com/dyn/resources/prod_documents/doc0856.pdf
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;

#pragma pack( 1 )
typedef struct{
	/*Header fields*/
	u8 marker[MARKER_SIZE];	//'UZEBOX'
	u8 version;		//header version
	u8 target;		//AVR target (ATmega644=0, ATmega1284=1)
	u32 progSize;	//program memory size in bytes
	u16 year;
	u8 name[32];
	u8 author[32];
	u8 icon[16*16];
	u32 crc32;
	u8 mouse;
	u8 description[64];
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
   register unsigned long crc;
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
	else if (s >= 'a' && s <= 'a')
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

	FILE *in_file = fopen(in_filename,"r");


	if (!in_file) return false;

	// Set entire memory out first in case new image is shorter than last one (0xff == NOP)
	memset(rom.progmem+HEADER_SIZE, 0xff , sizeof(rom.progmem-HEADER_SIZE));

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
				rom.progmem[addr+HEADER_SIZE] = parse_hex_byte(lp);
				addr ++;
				if (addr > progmemLast){
					progmemLast = addr;
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

int main(int argc,char **argv)
{

	if (argc!=4)
	{
		fprintf(stderr,"%s ver %i.%i -- Packs a HEX file to binary and adds a header.\n",argv[0],VERSION_MAJOR,VERSION_MINOR);
		fprintf(stderr,"usage: %s <input.hex> <ouput.uze> <gameinfo.properties>\n",argv[0]);
		fprintf(stderr,"example: %s halloween.hex halloween.uze gameinfo.properties\n",argv[0]);
		return 1;
	}

	fprintf(stderr,"\tPacking file: %s\n",argv[1]);
	
	chksum_crc32gentab();

	//parse the games properties file
	FILE *file = fopen ( argv[3], "r" );
	if ( file )
	{
		char line [128];
		while ( fgets ( line, sizeof(line), file ) != NULL ) /* read a line */
		{
			if(!strncmp(line,"#",1)){
				//ignore comment line

			}else if(!strncmp(line,"name=",5)){
				strcpy2((char*)rom.header.name,line+5,sizeof(rom.header.name));
				fprintf(stderr,"\tGame Name: %s\n", rom.header.name);

			}else if(!strncmp(line,"desc=",5)){
				strcpy2((char*)rom.header.description,line+5,sizeof(rom.header.description));
				fprintf(stderr,"\tDescription: %s\n", rom.header.description);

			}else if(!strncmp(line,"author=",7)){
				strcpy2((char*)rom.header.author,line+7,sizeof(rom.header.author));
				fprintf(stderr,"\tAuthor: %s\n", rom.header.author);

			}else if(!strncmp(line,"year=",5)){
				rom.header.year=(u16) strtoul(line+5,NULL,10);
				fprintf(stderr,"\tYear: %i\n", rom.header.year);
			}
		}
		fclose (file);
	}
	else
	{
		fprintf(stderr,"Can't read properties file: %s \n",argv[3]);
		return 1;
	}

	if(!load_hex(argv[1])){
		fprintf(stderr,"Could not process HEX file.\n");
		return 1;
	}


	memcpy(rom.header.marker,"UZEBOX",MARKER_SIZE);
	rom.header.version=HEADER_VERSION;
	rom.header.target=0;
	rom.header.crc32=chksum_crc32(rom.progmem+HEADER_SIZE, rom.header.progSize);

	fprintf(stderr,"\tCRC32: 0x%lx\n", rom.header.crc32);
	fprintf(stderr,"\tProgram size: %li \n",rom.header.progSize);

	//write the output file
	FILE *out_file = fopen(argv[2],"wb");
	if(!load_hex(argv[1])){
		fprintf(stderr,"Could not process output file.\n");
		return 1;
	}



	fwrite(&rom.progmem,rom.header.progSize+HEADER_SIZE,1,out_file);
	fclose(out_file);

	//while (1)
	//{
	//	if ('n' == getchar())
	//	   break;
	//}

	return 0;
}
