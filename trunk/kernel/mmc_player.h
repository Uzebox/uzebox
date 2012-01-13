/*
 *  Uzebox MMC WAV player
 *  Copyright (C) 2012 Alec Bourque
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

#ifndef __MMCPLAYER_H_
#define __MMCPLAYER_H_


	#define MMC_PLAYER_STOPPED 0x00
	#define MMC_PLAYER_STARTED 0x01
	#define MMC_PLAYER_PAUSED  0x02
	#define MMC_PLAYER_ERROR   0xff

	#define FAT_ATTR_READONLY	0x01
	#define FAT_ATTR_HIDDEN		0x02
	#define FAT_ATTR_SYSTEM		0x04
	#define FAT_ATTR_VOLUME		0x08
	#define FAT_ATTR_DIRECTORY	0x10
	#define FAT_ATTR_ARCHIVE	0x20
	#define FAT_ATTR_DEVICE		0x40

	typedef struct{
		//header
		unsigned char chunkID[4];
		unsigned long chunkSize;
		unsigned char format[4];
	
		//fmt chunk
		unsigned char subchunk1ID[4];
		unsigned long subchunk1Size;
		unsigned int audioFormat;
		unsigned int numChannels;
		unsigned long sampleRate;
		unsigned long byteRate;
		unsigned int blockAlign;
		unsigned int bitsPerSample;

		//data chunk
		unsigned char subchunk2ID[4];
		unsigned long subchunk2Size;

	} RIFFheader;

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

	typedef union {
		unsigned char buffer[512];
		BootRecord bootRecord;
		MBR mbr;
		DirectoryTableEntry files[16];
		RIFFheader riffHeader;
	} mmc_SectorData;

	typedef struct {
		unsigned char filename[9]; //8+ 0 terminator 
		unsigned char extension[3]; 
		unsigned long firstSector;
		unsigned long fileSize;	
	} mmc_File;


	void mmc_playerStart(mmc_File file);
	void mmc_playerStop();
	void mmc_playerPause();
	void mmc_playerResume();
	u8   mmc_playerGetStatus();
	u32  mmc_playerGetCurrentSector();
	void mmc_playerProcess();
	u8   mmc_masterInit();

	u8 mmc_listDir(mmc_File* files, u8 count, const char* extfilter);

#endif
