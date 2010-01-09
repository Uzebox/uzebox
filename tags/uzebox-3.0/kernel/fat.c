/*
 *  SD/MMC FAT routines
 *  Copyright (C) 2009 Alec Bourque
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

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <mmc.h>
#include <fat.h>
#include <avr/interrupt.h>

long dirTableSector;
long sectorsPerCluster;
long maxRootDirectoryEntries;
long bytesPerSector;

uint8_t *fatBuffer;

uint8_t InitFat(uint8_t *buffer){
	fatBuffer=buffer;
	return mmc_init(buffer);
}

void LoadRootDirectory(){

	//read MBR
	mmc_readsector(0);

	//read boot record
	long bootRecordSector= ((MBR*)fatBuffer)->partition1.startSector;
	mmc_readsector(bootRecordSector);

	int reservedSectors=((BootRecord*)fatBuffer)->reservedSectors;
	int sectorsPerFat=((BootRecord*)fatBuffer)->sectorsPerFat;
	maxRootDirectoryEntries=((BootRecord*)fatBuffer)->maxRootDirectoryEntries;
	bytesPerSector=((BootRecord*)fatBuffer)->bytesPerSector;
	sectorsPerCluster=((BootRecord*)fatBuffer)->sectorsPerCluster;

	//get directory table
	dirTableSector=bootRecordSector + reservedSectors + (sectorsPerFat * 2); 
	mmc_readsector(dirTableSector);

}

long GetFileSector(DirectoryTableEntry *file){
	return dirTableSector+((maxRootDirectoryEntries * 32)/bytesPerSector)+((file->firstCluster-2)*sectorsPerCluster);
}

unsigned char LoadFiles(File *files){
	//find files in the sector
	unsigned char i,j,k,pos,fileCount=0,c;
	DirectoryTableEntry *dirEntry;

	for(i=0;i<16;i++){
		dirEntry=&(((DirectoryTableEntry*)fatBuffer)[i]);

		if((dirEntry->fileAttributes & (FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME|FAT_ATTR_DIRECTORY|FAT_ATTR_DEVICE))==0){
			if((dirEntry->filename[0]!=0) && (dirEntry->filename[0]!=0xe5) && (dirEntry->filename[0]!=0x05) && (dirEntry->filename[0]!=0x2e)){									
				
				pos=0;
				for(j=0;j<8;j++){
					c=dirEntry->filename[j];
					if(c==0x20)break;
					if(c=='~')c='_';
					files[fileCount].filename[pos++]=c;
				}
				files[fileCount].filename[pos++]='.';
				for(k=0;k<3;k++){
					c=dirEntry->extension[k];
					if(c==0x20)break;
					files[fileCount].filename[pos++]=c;
				}
				files[fileCount].filename[pos]=0;

				files[fileCount].fileSize=dirEntry->fileSize;				
				files[fileCount].firstSector=GetFileSector(dirEntry);	
				
				fileCount++;
				if(fileCount==15) break; //can't fit more than 13 for now			
			}				
		}
	
	}
	return fileCount;
}
