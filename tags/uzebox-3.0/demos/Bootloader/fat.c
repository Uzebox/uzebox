#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include "fat.h"

extern uint8_t mmc_readsector(uint32_t lba);
extern uint8_t mmc_init(uint8_t *buffer);

typedef bool (*fat_filter)(DirectoryTableEntry* dirEntry);

typedef union {
	unsigned char buffer*;
	BootRecord bootRecord;
	MBR mbr;
	DirectoryTableEntry files[16];
} SectorData;

long dirTableSector;
long sectorsPerCluster;
long maxRootDirectoryEntries;
long bytesPerSector;
long bootRecordSector;
int reservedSectors;
int sectorsPerFat;
char currentDirEntry;
SectorData *buffer;





uint8_t fat_init(unsigned char *buf){
	
	uint8_t result=mmc_init(buf);
	if(result!=0)return result;

	buffer=(SectorData)buf;

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

	//load root directory table
	dirTableSector=bootRecordSector + reservedSectors + (sectorsPerFat * 2); 
	mmc_readsector(dirTableSector);
	currentDirEntry=0;

	return 0;
}

/*
//void fat_load_root_directory(unsigned char *buffer){
void fat_load_root_directory(){
	//get directory table
	dirTableSector=bootRecordSector + reservedSectors + (sectorsPerFat * 2); 
	mmc_readsector(dirTableSector);
	currentDirEntry=0;
}
*/

long GetFileSector(DirectoryTableEntry *file){
	return dirTableSector+((maxRootDirectoryEntries * 32)/bytesPerSector)+((file->firstCluster-2)*sectorsPerCluster);
}

bool uze_filter(DirectoryTableEntry* dirEntry){
	return false;
}

DirectoryTableEntry* fat_list_next_entry(fat_filter filter){


	return NULL;
}


