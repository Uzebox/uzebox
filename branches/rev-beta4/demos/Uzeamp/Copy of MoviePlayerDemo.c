/*
 *  SD/MMC Card reader demo
 *  Copyright (C) 2008 David Etherton
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
#include <mmc_if.h>

#include "data/fonts.pic.inc"




extern int mmc_mixerStart(uint32_t lba);
extern uint32_t mmc_mixerGetCurrentSector();
extern void mmc_mixerStop();

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

union SectorData {
	unsigned char buffer[512];
	BootRecord bootRecord;
	MBR mbr;
	DirectoryTableEntry files[16];
} sector;


typedef struct{
	unsigned char filename[13]; //filename+'.'+ext+0
	unsigned long firstSector;
	unsigned long fileSize;	
} File;



long GetFileSector(DirectoryTableEntry *file);
void LoadRootDirectory(unsigned char *buffer);
void PrintSector(unsigned char *buffer);
void init();


int main(){
	unsigned char c;
	unsigned int joy,i,j,k,x,y,cur,fileCount=0,pos,anim=0;
	unsigned long sectorNo=0,loc=0;
	bool playing=false;
	
	File files[16];

	SetFontTable(fonts);	
	init();
	


	LoadRootDirectory(sector.buffer);
	

	x=3;y=5;cur=0;
	Print(x,y-2,PSTR("FILENAME       SIZE     1ST SECTOR"));
	Print(x,y-1,PSTR("----------------------------------"));

	//find files in the sector
	for(i=0;i<16;i++){
		if((sector.files[i].fileAttributes & (FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME|FAT_ATTR_DIRECTORY|FAT_ATTR_DEVICE))==0){
			if((sector.files[i].filename[0]!=0) && (sector.files[i].filename[0]!=0xe5) && (sector.files[i].filename[0]!=0x05) && (sector.files[i].filename[0]!=0x2e)){									
				
				pos=0;
				for(j=0;j<8;j++){
					c=sector.files[i].filename[j];
					if(c==0x20)break;
					files[fileCount].filename[pos++]=c;
				}
				files[fileCount].filename[pos++]='.';
				for(k=0;k<3;k++){
					c=sector.files[i].extension[k];
					if(c==0x20)break;
					files[fileCount].filename[pos++]=c;
				}
				files[fileCount].filename[pos]=0;

				files[fileCount].fileSize=sector.files[i].fileSize;				
				files[fileCount].firstSector=GetFileSector(&sector.files[i]);	
				
				PrintRam(x,y+fileCount,files[fileCount].filename);
				PrintLong(x+21,y+fileCount,files[fileCount].fileSize);
				PrintLong(x+33,y+fileCount,files[fileCount].firstSector);
				//PrintInt(x+33,y+fileCount,sector.files[i].firstCluster,false);
				
				fileCount++;			
			}				
		}
	}

	Print(x,y+fileCount+2,PSTR("----------------------------------"));

	long songSize=0;
	c='>';
	Print(3,1,PSTR("SECTOR PLAYING:"));
  	while (1)
  	{
		
		WaitVsync(1);
		PrintHexLong(19,1,sectorNo);

		joy=ReadJoypad(0);
		if(joy&BTN_B){
			mmc_mixerStop();
			playing=false;
			while(ReadJoypad(0)!=0);
		}
		
		if(joy&BTN_A){
			sectorNo=files[cur].firstSector+1; //skip WAV header
			mmc_mixerStart(sectorNo);
			playing=true;
			songSize=files[cur].fileSize/512;
			Print(x,y+fileCount+2,PSTR("----------------------------------"));
			while(ReadJoypad(0)!=0);
		}

		if(joy&BTN_UP){
			PrintChar(x-1,y+cur,' ');
			if(cur>0)cur--;
			while(ReadJoypad(0)!=0);
		}

		if(joy&BTN_DOWN){
			PrintChar(x-1,y+cur,' ');
			if(cur<fileCount)cur++;
			while(ReadJoypad(0)!=0);
		}


		if(joy&BTN_SL){
			if(playing){
				sectorNo-=100;
				mmc_mixerStart(sectorNo);
			}
		}

		if(joy&BTN_SR){
			if(playing){
				sectorNo+=100;
				mmc_mixerStart(sectorNo);
			}
		}
		
		if(playing){
			if(anim&16){
				c=' ';
			}else{
				c='>';
			}
			anim++;

			sectorNo=mmc_mixerGetCurrentSector();
			loc=(34*(sectorNo-files[cur].firstSector))/songSize;
			if(loc<34){
				PrintChar(x+loc,y+fileCount+2,'I');	
			}else{
				loc=34;
			}
			if(loc>0){
				PrintChar(x+loc-1,y+fileCount+2,'-');	
			}

		}else{
			c='>';
		}

		PrintChar(x-1,y+cur,c);

		


		if(sectorNo>=(songSize+files[cur].firstSector)){
			mmc_mixerStop();
			playing=false;
		}
   }

} 

long dirTableSector;
long sectorsPerCluster;
long maxRootDirectoryEntries;
long bytesPerSector;
void LoadRootDirectory(unsigned char *buffer){

	//read MBR
	mmc_readsector(0, buffer);
	
	//read boot record
	long bootRecordSector=sector.mbr.partition1.startSector;
	mmc_readsector(bootRecordSector, buffer);

	int reservedSectors=sector.bootRecord.reservedSectors;
	int sectorsPerFat=sector.bootRecord.sectorsPerFat;
	maxRootDirectoryEntries=sector.bootRecord.maxRootDirectoryEntries;
	bytesPerSector=sector.bootRecord.bytesPerSector;
	sectorsPerCluster=sector.bootRecord.sectorsPerCluster;

	//get directory table
	dirTableSector=bootRecordSector + reservedSectors + (sectorsPerFat * 2); //+ ((maxRootDirectoryEntries * 32) / bytesPerSector);
	mmc_readsector(dirTableSector, buffer);

}

long GetFileSector(DirectoryTableEntry *file){
	return dirTableSector+((maxRootDirectoryEntries * 32)/bytesPerSector)+((file->firstCluster-2)*sectorsPerCluster);
}

void init(){
	unsigned char temp;
	ClearVram();
	Print(3,3,PSTR("TESTING MMC..."));

   do { temp = mmc_init();
   		Print(3,4,temp? PSTR("INIT FAILED") : PSTR("INIT GOOD   ")); 
		if(ReadJoypad(0)&&BTN_SELECT){
			while(ReadJoypad(0)!=0);
			SoftReset();
		}
   } while (temp);

   do { temp = mmc_readsector(0, sector.buffer);
   		Print(3,5,temp? PSTR("FIRST READ FAILED") : PSTR("FIRST READ GOOD   ")); } while (temp);
	
	ClearVram();
}
