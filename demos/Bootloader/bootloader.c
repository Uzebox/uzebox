/*
 *  Uzebox(tm) Gameloader
 *  Copyright (C) Alec Bourque
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

/*
About this program:
-------------------
This program is an AVR bootloader that allows to read and flash games from a SD/MMC card. 
The bootloader requires 4K of flash.

The Atmega644 needs to have some fuses set in order to support teh bootloader. For setup details see: http://uzebox.org/forums/viewtopic.php?p=3847#p3847

*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/wdt.h>

/*
 * Game loader version string as display in the menu
 */
const char strDemo[] PROGMEM = ">> Uzebox game loader 0.4.2 <<";


/*
 * Joystick constants & functions
 */

#define MAX_GAMES 128

#define BTN_SR	   1
#define BTN_SL	   2
#define BTN_X	   4
#define BTN_A	   8
#define BTN_RIGHT  16
#define BTN_LEFT   32
#define BTN_DOWN   64
#define BTN_UP     128
#define BTN_START  256
#define BTN_SELECT 512
#define BTN_Y      1024 
#define BTN_B      2048 

//SD Card defines
#define BYTES_PER_SECTOR 512 	//fixed for regular SD

//FAT Defines
#define FAT_ATTR_READONLY	0x01
#define FAT_ATTR_HIDDEN		0x02
#define FAT_ATTR_SYSTEM		0x04
#define FAT_ATTR_VOLUME		0x08
#define FAT_ATTR_DIRECTORY	0x10
#define FAT_ATTR_ARCHIVE	0x20
#define FAT_ATTR_DEVICE		0x40

#define EEP_BOOT_METHOD 1
#define EEP_BOOT_METHOD_MENU 0
#define EEP_BOOT_METHOD_GAME 1

#define EEP_FIELD_CRC32 18
#define EEP_FIELD_FLAGS 22


//EEPROM Kernel structs
struct EepromHeaderStruct
{
	//special identifier/magic number to determine if the EEPROM 
	//contains kernel recognizable data
	unsigned int signature;//0
	
	//version of this EEPROM data structure
	unsigned char version;//2

	//size of allocated blocks in bytes (should be 32)
	unsigned char blockSize;//3  

	//size of this header in blocks (should be 2)
	unsigned char headerSize;//4

	//identifies the harware type. Uzebox, Fuzebox,etc. Do we need that?
	unsigned char hardwareVersion;//5

	//identifies the harware revision. Do we need that?
	unsigned char hardwareRevision;//6

	/*
	Hardware features on board
	b15:b12 Reserved  
	b11     AD725 power control
	b10     PS2 Mouse
	b9      PS2 Keyboard
	b8      Ethernet
	b7      MIDI OUT
	b6      MIDI IN
	b5      SD Card Interface
	b4      Status LED
	b3      Soft Power switch 
	b2:b0   Joystick type: 0=SNES, 1=NES, 2-7=Reserved
	*/
	unsigned int features;//7

	//Even more features -- for future use
	unsigned int featuresExt;//9

	//MAC adress for the Ethernet interface
	unsigned char macAdress[6];	//11	

	//Composite Color Correction 
	//0=none
	//1=shorten line 	
	unsigned char colorCorrectionType;//17	

	//used by the bootloader to know the currently flashed game
	unsigned long currentGameCrc32; //18

	//Bootloader flags:
	//b0: Boot method: 0=Bootloader starts on reset (default) , 1=Current game starts on reset (hold any key to enter bootloader)
	//b1-b7: reserved	
	unsigned char bootloaderFlags; //22

	//for future expansion
	unsigned char reserved[9]; //23		
};

struct EepromBlockStruct{
	//some unique block ID assigned by ?. If 0xffff, block is free.
	unsigned int id;
	
	//application specific data
	//cast to your own types
	unsigned char data[30];		
};


typedef struct{
	unsigned char filename[8]; //zero padded
	unsigned char extension[3];//
	unsigned char fileAttributes;
	unsigned char reserved;
	unsigned char creationTimeMillis;
	unsigned int creationTime;
	unsigned int creationDate;
	unsigned int lastAccessDate;
	unsigned int eaIndex;
	unsigned int lastModifiedTime;
	unsigned int lastModifiedDate;
	unsigned int firstCluster;
	unsigned long fileSize;

} DirectoryTableEntry;


typedef struct{
	unsigned char state;
	unsigned char startHead;
	unsigned int startCylinder;
	unsigned char type;
	unsigned char endHead;
	unsigned int endCylinder;
	unsigned long startSector; //boot record starts at this sector
	unsigned long size; //in sectors

} PartitionEntry;


typedef struct {
	unsigned char execCode[446];
	PartitionEntry partition1;
	PartitionEntry partition2;
	PartitionEntry partition3;
	PartitionEntry partition4;
	int marker;
} MBR;

typedef struct {
	unsigned char jmp[3];
	unsigned char oemName[8];
	unsigned int bytesPerSector;
	unsigned char sectorsPerCluster;
	unsigned int reservedSectors;
	unsigned char fatCopies;
	unsigned int maxRootDirectoryEntries;
	unsigned int totalSectorsLegacy;
	unsigned char mediaDescriptor;
	unsigned int sectorsPerFat;
	unsigned int sectorPerTrack;
	unsigned int numbersOfHeads;
	unsigned long hiddenSectors;
	unsigned long totalSectors;
	unsigned char physicalDriveNumber;
	unsigned char reserved;
	unsigned char extendedBootSignature;
	unsigned long serialNumber;
	unsigned char volumeLabel[11];	
	unsigned char bootCode[448];
	unsigned int signature;

} BootRecord;

typedef struct{
	/*Header fields*/
	char marker[6];	//'UZEBOX'
	unsigned char version;		//header version
	unsigned char target;		//AVR target (ATmega644=0, ATmega1284=1)
	unsigned long progSize;	//program memory size in bytes
	unsigned int year;
	char name[32];
	char author[32];
	char icon[16*16];
	unsigned long crc32;
	unsigned char mouse;
} RomHeader;

union SectorData {
	unsigned char buffer[BYTES_PER_SECTOR];
	BootRecord bootRecord;
	MBR mbr;
	DirectoryTableEntry files[16];
	RomHeader header;
} sector;

//typedef struct{
//	unsigned char filename[13]; //filename+'.'+ext+0
//	unsigned long firstSector;
//	unsigned long fileSize;	
//} File;



/*
 * Functions
 */
void LoadRootDirectory(unsigned char *buffer);
long GetFileSector(DirectoryTableEntry *file);
//unsigned char LoadFiles(unsigned char *buffer,File *destFiles);
extern void WriteEeprom(unsigned int addr,unsigned char value);
extern unsigned char ReadEeprom(unsigned int addr);




/** Helper structure.
	This simplify conversion between bytes and words.
*/
struct u16bytes
{
	uint8_t low;	//!< byte member
	uint8_t high;	//!< byte member
};

/** Helper union.
	This simplify conversion between bytes and words.
*/
union u16convert
{
	uint16_t value;			//!< for word access
	struct u16bytes bytes;	//!< for byte access
};

/** Helper structure.
	This simplify conversion between bytes and longs.
*/
struct u32bytes
{
	uint8_t byte1;	//!< byte member
	uint8_t byte2;	//!< byte member
	uint8_t byte3;	//!< byte member
	uint8_t byte4;	//!< byte member
};

/** Helper structure.
	This simplify conversion between words and longs.
*/
struct u32words
{
	uint16_t low;		//!< word member
	uint16_t high;		//!< word member
};

/** Helper union.
	This simplify conversion between bytes, words and longs.
*/
union u32convert 
{
	uint32_t value;			//!< for long access
	struct u32words words;	//!< for word access
	struct u32bytes bytes;	//!< for byte access
} conv32;


extern uint8_t mmc_readsector(uint32_t lba);
extern uint8_t mmc_init(uint8_t *buffer);


bool init();
void flashGame(unsigned char fileNo);


extern void WaitVSync(unsigned char count);
extern void SetFont(char x,char y, unsigned char tileId,unsigned char bgColor);

extern void ClearVram(void);
extern void Print(unsigned int adress ,char *string, unsigned char bgColor);
extern void DrawBar(unsigned int adress,unsigned char len,unsigned char color);

extern volatile unsigned int joypad_status;
extern volatile unsigned char vsync_flag;
extern volatile unsigned char wave_vol;
extern volatile unsigned char first_render_line_tmp;
extern volatile unsigned char screen_tiles_v_tmp;
extern unsigned int vram[];  


#define TEST_SIZE 0
#define FONT_TYPE 0


long dirTableSector;
long sectorsPerCluster;
long maxRootDirectoryEntries;
long bootRecordSector;
int reservedSectors;
int sectorsPerFat;
unsigned char maxRootDirectorySectors;
unsigned char eeBootloaderFlags;

/*
 * Initializes the watchdog timer used for software resets
 */
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3"))); 
void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();
}

/*
 * Performs a software reset
 */
void SoftReset(void){        
	wdt_enable(WDTO_15MS);  
	while(1);
}

void fat_init(unsigned char *buffer){

	//read MBR
	mmc_readsector(0);

	//read boot record
	bootRecordSector= ((MBR*)buffer)->partition1.startSector;
	mmc_readsector(bootRecordSector);

	reservedSectors=((BootRecord*)buffer)->reservedSectors;
	sectorsPerFat=((BootRecord*)buffer)->sectorsPerFat;
	sectorsPerCluster=((BootRecord*)buffer)->sectorsPerCluster;

	maxRootDirectoryEntries=((BootRecord*)buffer)->maxRootDirectoryEntries;
	maxRootDirectorySectors=((maxRootDirectoryEntries * 32)/BYTES_PER_SECTOR);
		

}


long GetFileSector(DirectoryTableEntry *file){
	return dirTableSector+maxRootDirectorySectors+((file->firstCluster-2)*sectorsPerCluster);
}



bool init(){
	unsigned char temp;

   	do{ 
		temp = mmc_init(sector.buffer);
		if(temp){
    		Print((12*40*2)+(9*2), PSTR("SD INIT FAIL: NO CARD?"),0);  
		}
						 
	} while (temp);

   do { 
		temp = mmc_readsector(0);
		if(temp){
        	Print((13*40*2)+(13*2),PSTR("INIT READ FAIL"),0); 
		}
	} while (temp);


	return 0;
}


unsigned long filesFirstSector[128];



/*
 * Read the game creator from the specified UZE file
 * and displays it in the info box
 * 
 * Input: fileNo = pointer to a file handle
 */
void showInfo(unsigned char fileNo){

	//erase box
	for(unsigned char i=0;i<(40*3);i++){
		vram[(40*24)+i]=0x5000;
	}

	//load rom header & display it in the infox box area
	mmc_readsector(filesFirstSector[fileNo]);			
	Print((25*40*2)+(3*2),sector.header.author,0x50);	

	//display bootloader startup type
	char *type=PSTR("BOOT: MENU");	
	if(eeBootloaderFlags&EEP_BOOT_METHOD){
		type=PSTR("BOOT: GAME");
	}
	Print((25*40*2)+(28*2),type,0x50);	
}


/*
 * Clears the screens and intializes the selextion bar, 
 * top menu bar and info box.
 * 
 * Input:  page  => current navigation page
 *         total => total number of navigation pages
 */
void topMenu(unsigned char page,unsigned char total){
	ClearVram();
	DrawBar((40*2),40,0x50);
	Print((40*2)+(5*2),strDemo,0x50);
	Print((3*40*2)+(14*2),PSTR("< PAGE 1/1 >"),0);
	SetFont(21,3,(page/16)+49,0);
	
	if(total&0xf)total+=16;
	SetFont(23,3,(total/16)+32+16,0);

}


#define X 3
#define Y 5

int main(){

	unsigned char fileCount,fileNo,i,c=0;
	unsigned char col,dir;
	unsigned char page;//,totalPages;



	eeBootloaderFlags=ReadEeprom(EEP_FIELD_FLAGS);

	//wait for SD to stabilize
	WaitVSync(4); //2

	//boot normal game if EEPROM flag is set, no key is pressed and 
	//theres a jmp instruction at progmem=0
	if((eeBootloaderFlags&EEP_BOOT_METHOD) == EEP_BOOT_METHOD_GAME && 
		joypad_status == 0 && 
		pgm_read_byte(0) == 0x0c && 
		BOOT_METHOD_GAME==1){
boot_game:							
		cli();
		unsigned char temp = MCUCR;
		// Enable change of Interrupt Vectors 
		MCUCR = temp|(1<<IVCE);
		// Move interrupts to Start of Flash
		MCUCR = temp&~(1<<IVSEL);	
		asm("jmp 0");
	}


	topMenu(0,0);
	init();
	topMenu(0,0);

	fat_init(sector.buffer);

	//get directory table
	dirTableSector=bootRecordSector + reservedSectors + (sectorsPerFat * 2); 

	fileCount=0;
	col=1,dir=1;
	page=0;

	
	//find files in the directory and store their first sector
	do{

		mmc_readsector(dirTableSector+page);

		//read all entries in the sector
		for(i=0;i<16;i++){
			if((sector.files[i].fileAttributes & (FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME|FAT_ATTR_DIRECTORY|FAT_ATTR_DEVICE))==0){
				c=sector.files[i].filename[0];
				if(c == 0){

					break;
				}else if(c!=0xe5 && c!=0x05 && c!=0x2e &&
					sector.files[i].extension[0]=='U' &&
					sector.files[i].extension[1]=='Z' &&
					sector.files[i].extension[2]=='E' 
					){									

					filesFirstSector[fileCount]=GetFileSector(&sector.files[i]);	
					fileCount++;
					if(fileCount==MAX_GAMES) break;
				}				
			}
			
		}
		
		page++;

	}while(c!=0 && fileCount<MAX_GAMES && page<maxRootDirectorySectors);

	page=0;

browse_files:


	fileNo=0;
	topMenu(page,fileCount);


	//fetch names from SD
	for(i=0;i<16;i++){
		mmc_readsector(filesFirstSector[page+i]);
		Print(((i+Y)*40*2)+(X*2),sector.header.name,0);	
	}
	
	showInfo(fileNo+page);


	while(joypad_status!=0); //in case the user pressed start to enter the bootloader insure the key in relealsed

	while(1){
		//drawCursor(x-1,y+fileNo,37,col);
		DrawBar((Y+fileNo)*40*2,40,col);
		WaitVSync(6);	

		col+=dir;
		if(col==5){
			dir=-1;
		}else if(col==0){
			dir=1;
		}


		if(joypad_status&BTN_UP){
			if(fileNo>0){
				DrawBar((Y+fileNo)*40*2,40,0); //erase bar at current location
				fileNo--;
				wave_vol=90;
				showInfo(fileNo+page);
			}
		
		}else if(joypad_status&BTN_DOWN){
			if(fileNo<15 && (page+fileNo)<fileCount-1){
				DrawBar((Y+fileNo)*40*2,40,0);  //erase bar at current location
				fileNo++;				
				wave_vol=90;
				showInfo(fileNo+page);
			}

		}else if(joypad_status&BTN_LEFT){
			if(page>0){
				page-=16;
				goto browse_files;
			}
		
		}else if(joypad_status&BTN_RIGHT){
			if((page+16)<fileCount){
				page+=16;
				goto browse_files;
			}

		}else if(joypad_status&BTN_START){
			while(joypad_status!=0);
		
			flashGame(fileNo+page);
			goto boot_game;
		
		}
		else if(joypad_status&BTN_SELECT){
			eeBootloaderFlags^=1;
			WriteEeprom(EEP_FIELD_FLAGS,eeBootloaderFlags);
			goto browse_files;			
		}

	}


	while(1);
}



void flashGame(unsigned char fileNo){

	unsigned long flashPage=0;//,crc;
	unsigned char fileSector,sectors,pageNo,progress,progressCount=0,progressPos=2;
	
	//read rom header's CRC value
	mmc_readsector(filesFirstSector[fileNo]);
	conv32.bytes.byte1=ReadEeprom(EEP_FIELD_CRC32+0);
	conv32.bytes.byte2=ReadEeprom(EEP_FIELD_CRC32+1);
	conv32.bytes.byte3=ReadEeprom(EEP_FIELD_CRC32+2);
	conv32.bytes.byte4=ReadEeprom(EEP_FIELD_CRC32+3);

	//Flash only if it's a different game or if there's no game already 
	if(conv32.value!=sector.header.crc32 || pgm_read_byte(0)!=0x0c){
	
		//set resolution to 2 tiles
		WaitVSync(1);
		first_render_line_tmp=120;
		screen_tiles_v_tmp=2;	
					
		for (int i=0; i<40; i++){
			SetFont(i,0,' ',0);
			SetFont(i,1,' ',80);
		}
		Print((15*2),PSTR("LOADING..."),0);


		conv32.value=sector.header.crc32;
		sectors=sector.header.progSize/BYTES_PER_SECTOR;
	
		//clip prog size to prevent overwriting the bootloader
		//(65536-4096)/BYTES_PER_SECTOR=120 sectors max
		if(sectors>=120){
			sectors=120;
		} else if((sector.header.progSize%BYTES_PER_SECTOR)!=0){
			sectors++;
		}
		progress=sectors/32;
		
		eeprom_busy_wait();

		for(fileSector=0;fileSector<sectors;fileSector++){
	
			//read first sectgor after header
			mmc_readsector(filesFirstSector[fileNo]+1+fileSector);
			unsigned char *buf=sector.buffer;

			/*			
			//patch soft power interrupt vectors into bootloader
			if(fileSector==0){
				sector.buffer[6]=0x04;
				sector.buffer[7]=0xf0;
				sector.buffer[10]=0x08;
				sector.buffer[11]=0xf0;
			}
			*/

			//program the game
			//two flash pages per SD sector
			for(pageNo=0;pageNo<2;pageNo++){
			
			    
			    cli();
				boot_page_erase (flashPage);
				sei();
			    boot_spm_busy_wait ();      // Wait until the memory is erased.
				
				
			    for (int i=0; i<SPM_PAGESIZE; i+=2)
			    {
			        // Set up little-endian word.

			        uint16_t w = *buf++;
			        w += (*buf++) << 8;
					cli();	
			        boot_page_fill (flashPage + i, w);
					sei();
			    }
				cli();
			    boot_page_write (flashPage);     // Store buffer in flash page.
				sei();

			    boot_spm_busy_wait();       // Wait until the memory is written.

			    // Reenable RWW-section again. We need this if we want to jump back
			    // to the application after bootloading.
			    cli();
				boot_rww_enable();
				sei();

				flashPage+=256;
			}

			progressCount++;
			if(progressCount>=progress){
				progressCount=0;
				progressPos++;
			}
			SetFont(progressPos,1,'>',80);

		}

		//Write the CRC of the game just flashed to eeprom
		WriteEeprom(EEP_FIELD_CRC32+0,conv32.bytes.byte1);
		WriteEeprom(EEP_FIELD_CRC32+1,conv32.bytes.byte2);
		WriteEeprom(EEP_FIELD_CRC32+2,conv32.bytes.byte3);
		WriteEeprom(EEP_FIELD_CRC32+3,conv32.bytes.byte4);

	}
}

