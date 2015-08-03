
/*
 *  Uzebox(tm) kernel build options
 *  Copyright (C) 2008  Alec Bourque
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
 * Uzebox is a reserved trade mark
*/

#ifndef __FAT_H_
#define __FAT_H_

	#define FAT_ATTR_READONLY	0x01
	#define FAT_ATTR_HIDDEN		0x02
	#define FAT_ATTR_SYSTEM		0x04
	#define FAT_ATTR_VOLUME		0x08
	#define FAT_ATTR_DIRECTORY	0x10
	#define FAT_ATTR_ARCHIVE	0x20
	#define FAT_ATTR_DEVICE		0x40


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

/*
	union SectorData {
		unsigned char buffer[512];
		BootRecord bootRecord;
		MBR mbr;
		DirectoryTableEntry files[16];
		RIFFheader riffHeader;
	} sector;
*/
	typedef struct{
		unsigned char filename[8]; //filename+'.'+ext+0
		unsigned char extension[3];
		unsigned long firstSector;
		unsigned long fileSize;	
	} File;

	/*
	 * Functions
	 */
	void LoadRootDirectory();
	long GetFileSector(DirectoryTableEntry *file);
	unsigned char LoadFiles(File *destFiles);
	uint8_t InitFat(unsigned char *buffer);

#endif
