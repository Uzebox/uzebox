#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/boot.h>

/*
 * Joystick constants & functions
 */


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

//FAT stuff
#define FAT_ATTR_READONLY	0x01
#define FAT_ATTR_HIDDEN		0x02
#define FAT_ATTR_SYSTEM		0x04
#define FAT_ATTR_VOLUME		0x08
#define FAT_ATTR_DIRECTORY	0x10
#define FAT_ATTR_ARCHIVE	0x20
#define FAT_ATTR_DEVICE		0x40


//EEPROM Kernel structs
struct EepromHeaderStruct
{
	//special identifier/magic number to determine if the EEPROM 
	//contains kernel recognizable data
	unsigned int signature;
	
	//version of this EEPROM data structure
	unsigned char version;

	//size of allocated blocks in bytes (should be 32)
	unsigned char blockSize;  

	//size of this header in blocks (should be 2)
	unsigned char headerSize;

	//identifies the harware type. Uzebox, Fuzebox,etc. Do we need that?
	unsigned char hardwareVersion;

	//identifies the harware revision. Do we need that?
	unsigned char hardwareRevision;

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
	unsigned int features;

	//Even more features -- for future use
	unsigned int featuresExt;

	//MAC adress for the Ethernet interface
	unsigned char macAdress[6];		

	//Composite Color Correction 
	//0=none
	//1=shorten line 	
	unsigned char colorCorrectionType;	

	//used by the bootloader to know the currently flashed game
	unsigned long currentGameCrc32; //19

	//for future expansion
	unsigned char reserved[10];		
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
	unsigned char buffer[512];
	BootRecord bootRecord;
	MBR mbr;
	DirectoryTableEntry files[16];
	RomHeader header;
} sector;

typedef struct{
	unsigned char filename[13]; //filename+'.'+ext+0
	unsigned long firstSector;
	unsigned long fileSize;	
} File;



/*
 * Functions
 */
void LoadRootDirectory(unsigned char *buffer);
long GetFileSector(DirectoryTableEntry *file);
unsigned char LoadFiles(unsigned char *buffer,File *destFiles);
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


File files[16];

char strDemo[] PROGMEM = ">> Uzebox gameloader Menu Alpha1 <<";
//char strStarting[] PROGMEM="Starting installed game in 5 seconds...";
//char strStart[] PROGMEM="Press START to launch game now.";
//char strMenu[] PROGMEM="Press any other button for menu.";


void showInfo(unsigned char fileNo){

	for(unsigned char i=0;i<(40*4);i++){
		vram[(40*24)+i]=0x5000;
	}

	mmc_readsector(files[fileNo].firstSector);
	Print((25*40*2)+(3*2),sector.header.author,0x50);	
	//PrintInt(6,26,sector.header.year,80);
	//PrintRam(8,y+i,sector.header.author,0);	
}


//unsigned char gameExist;

void topMenu(){
	ClearVram();
	DrawBar(0,40,0x50);
	Print((3*2),strDemo,0x50);
}



int main(){

	unsigned int fileCount=0,x=3,y=5,fileNo=0;
	unsigned int i;
	char c;


	//boot normal game if no key if pressed and 
	//theres a jmp instruction at progmem=0
	//WaitVSync(2);
	//gameExist=pgm_read_byte(0);
	
	//if(joypad_status==0 && pgm_read_byte(0)==0x0c){
	//
	//	cli();
	//	unsigned char temp = MCUCR;
	//	// Enable change of Interrupt Vectors 
	//	MCUCR = temp|(1<<IVCE);
	//	// Move interrupts to Start of Flash
	//	MCUCR = temp&~(1<<IVSEL);	
	//	asm("jmp 0");
	//	
	//}



	topMenu();
	init();
	topMenu();


	

	LoadRootDirectory(sector.buffer);
	

	//find files in the sector
	for(i=0;i<16;i++){
		if((sector.files[i].fileAttributes & (FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME|FAT_ATTR_DIRECTORY|FAT_ATTR_DEVICE))==0){
			c=sector.files[i].filename[0];
			if( c!=0 && c!=0xe5 && c!=0x05 && c!=0x2e &&
				sector.files[i].extension[0]=='U' &&
				sector.files[i].extension[1]=='Z' &&
				sector.files[i].extension[2]=='E' 
				){									


				//files[fileCount].fileSize=sector.files[i].fileSize;				
				files[fileCount].firstSector=GetFileSector(&sector.files[i]);	
				

				fileCount++;
				if(fileCount==15) break; //can't fit more than 13 for now			
			}				
		}
	}

	//fetch names from SD
	for(i=0;i<fileCount;i++){
		mmc_readsector(files[i].firstSector);
		Print(((y+i)*40*2)+(x*2),sector.header.name,0);	
	}

	fileNo=0;
	showInfo(fileNo);

	unsigned char col=1,dir=1;//,x=2,y=5;

	while(joypad_status!=0); //in case the user pressed start to enter the bootloader

	while(1){
		//drawCursor(x-1,y+fileNo,37,col);
		DrawBar((y+fileNo)*40*2,40,col);
		WaitVSync(6);	

		col+=dir;
		if(col==5){
			dir=-1;
		}else if(col==0){
			dir=1;
		}

		if(joypad_status&BTN_UP){
			if(fileNo>0){
				//drawCursor(x-1,y+fileNo,37,0);
				DrawBar((y+fileNo)*40*2,40,0);
				fileNo--;
				wave_vol=90;
				showInfo(fileNo);
			}
		}
	
		if(joypad_status&BTN_DOWN){
			if(fileNo<fileCount-1){
				DrawBar((y+fileNo)*40*2,40,0);
				//drawCursor(x-1,y+fileNo,37,0);
				fileNo++;
				wave_vol=90;
				showInfo(fileNo);
			}
		}


		if(joypad_status&BTN_START){
			while(joypad_status!=0);
			


 			flashGame(fileNo);
			
			
			cli();
			unsigned char temp = MCUCR;
			// Enable change of Interrupt Vectors 
			MCUCR = temp|(1<<IVCE);
			// Move interrupts to Start of Flash
			MCUCR = temp&~(1<<IVSEL);	
			asm("jmp 0");
			


		}

	}


	while(1);
}



void flashGame(unsigned char fileNo){

	unsigned long flashPage=0;//,crc;
	unsigned char fileSector,sectors,pageNo,progress,progressCount=0,progressPos=2;
	
	//read rom header
	mmc_readsector(files[fileNo].firstSector);
	conv32.bytes.byte1=ReadEeprom(19);
	conv32.bytes.byte2=ReadEeprom(20);
	conv32.bytes.byte3=ReadEeprom(21);
	conv32.bytes.byte4=ReadEeprom(22);
	//crc=conv32.value;



	if(conv32.value!=sector.header.crc32 || pgm_read_byte(0)!=0x0c){
	
		//PrintLong(20,3,crc,0);
		//PrintLong(20,4,sector.header.crc32,0);

		//set resolution to 2 tiles
		WaitVSync(1);
		first_render_line_tmp=120;
		screen_tiles_v_tmp=2;	
					
		for (int i=0; i<40; i++){
			SetFont(i,0,' ',0);
			SetFont(i,1,' ',80);
		}
		Print((15*2),PSTR("LOADING..."),0);

	//	WaitVsync(1);

		conv32.value=sector.header.crc32;
		sectors=sector.header.progSize/512;
		if((sector.header.progSize%512)!=0)sectors++;
		progress=sectors/32;
		
		eeprom_busy_wait();

		for(fileSector=0;fileSector<sectors;fileSector++){
			//read first sectgor after header
			mmc_readsector(files[fileNo].firstSector+1+fileSector);
			unsigned char *buf=sector.buffer;
	
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


		WriteEeprom(19,conv32.bytes.byte1);
		WriteEeprom(20,conv32.bytes.byte2);
		WriteEeprom(21,conv32.bytes.byte3);
		WriteEeprom(22,conv32.bytes.byte4);

	}
}



long dirTableSector;
long sectorsPerCluster;
long maxRootDirectoryEntries;
long bytesPerSector;
long bootRecordSector;
int reservedSectors;
int sectorsPerFat;

void fat_init(unsigned char *buffer){

	//read MBR
	mmc_readsector(0);

	//read boot record
	bootRecordSector= ((MBR*)buffer)->partition1.startSector;
	mmc_readsector(bootRecordSector);

	reservedSectors=((BootRecord*)buffer)->reservedSectors;
	sectorsPerFat=((BootRecord*)buffer)->sectorsPerFat;

	maxRootDirectoryEntries=((BootRecord*)buffer)->maxRootDirectoryEntries;
	bytesPerSector=((BootRecord*)buffer)->bytesPerSector;
	sectorsPerCluster=((BootRecord*)buffer)->sectorsPerCluster;
}


void LoadRootDirectory(unsigned char *buffer){
	fat_init(buffer);

	//get directory table
	dirTableSector=bootRecordSector + reservedSectors + (sectorsPerFat * 2); 
	mmc_readsector(dirTableSector);

}


long GetFileSector(DirectoryTableEntry *file){
	return dirTableSector+((maxRootDirectoryEntries * 32)/bytesPerSector)+((file->firstCluster-2)*sectorsPerCluster);
}



bool init(){
	unsigned char temp;

   	do{ 
		temp = mmc_init(sector.buffer);
    	Print((12*40*2)+(7*2),temp? PSTR("SD INIT FAIL: NO CARD?") : PSTR("SD INIT OK"),0);  
						 
	} while (temp);


	

   do { 
		temp = mmc_readsector(0);
        Print((12*40*2)+(19*2),temp? PSTR("INIT READ FAIL") : PSTR("READ OK"),0); 
	} while (temp);


	return 0;
}




/*
int main(){
	fat_init(sector.buffer);
	init();

	while(1);
}
*/
