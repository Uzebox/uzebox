
// diagnostic ports for emulator - these are listed as "reserved" on the actual hardware
// PPRINT - print a char to the console
#define PPRINT 0x1A
// HPRINT - print a hex value to the console
#define HPRINT 0x19

// MMC/SD/SPI stuff
#define SPI_PORT	PORTB
#define SPI_DDR		DDRB
#define SPI_PIN		PINB

#define MMC_CS_PORT	PORTD
#define MMC_CS_DIR	DDRD

#define SD_SCK		1	
#define SD_CMD		2
#define SD_DAT0		3
#define SD_DAT3		4
#define SD_DAT1		5
#define SD_DAT2		6
#define SD_CARD		7

#define MMC_SCK    7
#define MMC_MOSI   5
#define MMC_MISO   6
#define MMC_CS     6

// command values to be used with mmccommand
//#define CMD_RESET 0x40
//#define CMD_INIT 0x41
//#define CMD_READBLOCK 0x51
#define CMD_RESET 0
#define CMD_INIT 1
#define CMD_READBLOCK 17


#define FAT_PARTITION_TYPE_FAT16 0x06
#define FAT_ERR_INVALID_PARTITION_TYPE 0xf0
#define FS_OK 0

/*
	typedef struct {
		unsigned char execCode[446];
		PartitionEntry partition1;
		PartitionEntry partition2;
		PartitionEntry partition3;
		PartitionEntry partition4;
		int marker;
	} MBR;

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
*/

// The only part of the MBR we care about is the start sector on partition 1
// NOTE: this is mapped to the mmc I/O buffer
.equ mbr_partition1,				446
.equ mbr_partition1_partitionType,	4
.equ mbr_partition1_startSector, 	8


/*
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
  */
// Boot record structure
// NOTE: this is mapped to the mmc I/O buffer
//.equ boot_record, buffer
.equ boot_jmp, 0
.equ boot_oemName, boot_jmp+3
.equ boot_bytesPerSector, boot_oemName+8
.equ boot_sectorsPerCluster, boot_bytesPerSector+2
.equ boot_reservedSectors, boot_sectorsPerCluster+1
.equ boot_fatCopies, boot_reservedSectors+2
.equ boot_maxRootDirectoryEntries, boot_fatCopies+1
.equ boot_totalSectorsLegacy, boot_maxRootDirectoryEntries+2
.equ boot_mediaDescriptor, boot_totalSectorsLegacy+2
.equ boot_sectorsPerFat, boot_mediaDescriptor+1
// we don't care about the rest...

/*
typedef struct{
	//Header fields (512 bytes)
	u8 marker[6];	//'UZEBOX'
	u8 version;		//header version
	u8 target;		//AVR target (ATmega644=0, ATmega1284=1)
	u32 progSize;	//program memory size in bytes
	u16 year;
	u8 name[32];
	u8 author[32];
	u8 icon[16*16];
         unsigned long crc32;
         unsigned char mouse;
	u8 reserved[178];
}RomHeader;
*/
// Uzebox header
.equ uze_header, 0
.equ uze_marker, uze_header+0
.equ uze_version, uze_marker+6
.equ uze_target, uze_version+1
.equ uze_progSize, uze_target+1
.equ uze_year, uze_progSize+4
.equ uze_name, uze_year+2
.equ uze_author, uze_name+32
.equ uze_icon, uze_author+32
.equ uze_crc32, uze_icon+256
.equ uze_mouse, uze_crc32+4

/*
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
*/

// relative offsets for directory entry
.equ dir_filename, 0
.equ dir_extension, dir_filename+8
.equ dir_fileAttrs, dir_extension+3
.equ dir_reserved, dir_fileAttrs+1
.equ dir_created_ms, dir_reserved+1
.equ dir_created_time, dir_created_ms+1
.equ dir_created_date, dir_created_time+2
.equ dir_accessed, dir_created_date+2
.equ dir_ea_index, dir_accessed+2
.equ dir_modified_time, dir_ea_index+2
.equ dir_modified_date, dir_modified_time+2
.equ dir_first_cluster, dir_modified_date+2
.equ dir_file_size, dir_first_cluster+2

#define DIR_ENTRY_SIZE 32
#define DIR_ENTRIES_PER_SECTOR 16

#define FAT_ATTR_READONLY	0x01
#define FAT_ATTR_HIDDEN		0x02
#define FAT_ATTR_SYSTEM		0x04
#define FAT_ATTR_VOLUME		0x08
#define FAT_ATTR_DIRECTORY	0x10
#define FAT_ATTR_ARCHIVE	0x20
#define FAT_ATTR_DEVICE		0x40

// Video defines
#define VRAM_TILES_H 40 
#define VRAM_TILES_V 28
#define SCREEN_TILES_H 40
#define SCREEN_TILES_V 28
#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V*2

// UI defines
#define MAX_MENU_ENTRIES 18
#define LEFT_MARGIN 2
#define TOP_MARGIN 2
#define INFO_START (VRAM_TILES_H*(MAX_MENU_ENTRIES+TOP_MARGIN+1))*2
#define INFO_NAME
#define INFO_YEAR
#define INFO_AUTHOR
#define INFO_SIZE
#define MENU_TITLE_BG_COLOR 80
#define INFO_AREA_BG_COLOR 80 //0x52


