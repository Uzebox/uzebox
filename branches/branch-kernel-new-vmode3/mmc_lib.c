/*
 *  Uzebox(tm) MMC WAV player
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

/*
 * Library to polay wav files in the background.
 */

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <uzebox.h>
#include <mmc.h>
#include <mmc_player.h>
#include <avr/interrupt.h>

u32 mmc_player_sector;
u8  mmc_player_status = MMC_PLAYER_STOPPED;
u16 playerRead,sectorRead;

union{
	u32 value;
	u8 bytes[4];
}samplesToPlay;

long dirTableSector;
long sectorsPerCluster;
long maxRootDirectoryEntries;
long bytesPerSector;

mmc_SectorData sector;

void LoadRootDirectory(unsigned char *buffer){

	//read MBR
	mmc_readsector(0);
	
	//read boot record

	//long bootRecordSector=sector_buffer_ptr->mbr.partition1.startSector;

	long bootRecordSector=sector.mbr.partition1.startSector;
	mmc_readsector(bootRecordSector);

	int reservedSectors=sector.bootRecord.reservedSectors;
	int sectorsPerFat=sector.bootRecord.sectorsPerFat;
	maxRootDirectoryEntries=sector.bootRecord.maxRootDirectoryEntries;
	bytesPerSector=sector.bootRecord.bytesPerSector;
	sectorsPerCluster=sector.bootRecord.sectorsPerCluster;

	//get directory table
	dirTableSector=bootRecordSector + reservedSectors + (sectorsPerFat * 2); //+ ((maxRootDirectoryEntries * 32) / bytesPerSector);
	mmc_readsector(dirTableSector);

}

long GetFileSector(DirectoryTableEntry *file){
	return dirTableSector+((maxRootDirectoryEntries * 32)/bytesPerSector)+((file->firstCluster-2)*sectorsPerCluster);
}


u8 mmc_listDir(mmc_File* files, u8 count, const char* extFilter){
	u8 c,i,j,k,pos,fileCount=0;

	LoadRootDirectory(sector.buffer);
	
	//find files in the sector
	for(i=0;i<count;i++){
		//get only files
		if((sector.files[i].fileAttributes & (FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME|FAT_ATTR_DIRECTORY|FAT_ATTR_DEVICE))==0){
			if((sector.files[i].filename[0]!=0) && (sector.files[i].filename[0]!=0xe5) && (sector.files[i].filename[0]!=0x05) && (sector.files[i].filename[0]!=0x2e)){									
				
				//apply extension filter
				if(extFilter==NULL || (sector.files[i].extension[0]==extFilter[0] && sector.files[i].extension[1]==extFilter[1] && sector.files[i].extension[2]==extFilter[2]) ){

					pos=0;
					for(j=0;j<8;j++){
						c=sector.files[i].filename[j];
						if(c==0x20)break;
						if(c=='~')c='_';
						files[fileCount].filename[pos++]=c;
					}
					files[fileCount].filename[pos]=0;

					//files[fileCount].filename[pos++]='.';
					for(k=0;k<3;k++){
						c=sector.files[i].extension[k];
						//if(c==0x20)break;
						files[fileCount].extension[k]=c;
					}
					

					files[fileCount].fileSize=sector.files[i].fileSize;				
					files[fileCount].firstSector=GetFileSector(&sector.files[i]);	
				
//					PrintRam(x,y+fileCount,files[fileCount].filename);
					//PrintLong(x+21,y+fileCount,files[fileCount].fileSize);
//					printFileTime(x+16,y+fileCount,(files[fileCount].fileSize/512));

				
					fileCount++;
					if(fileCount >=count) break;

				}
						
			}				
		}
	}

	return fileCount;
}


//starts playing wav file pointed at by lba
void mmc_playerStart(mmc_File file){

	if(mmc_player_status==MMC_PLAYER_STARTED)
	{
		mmc_player_status = MMC_PLAYER_STOPPED;
		mmc_send_command(12,0,0); //stop current transfer
		mmc_clock_and_release();
	}

	// send the multiple block read command and logical sector address
	mmc_player_sector=file.firstSector;
	mmc_send_command(18,(mmc_player_sector>>7) & 0xffff, (mmc_player_sector<<9) & 0xffff);	
	playerRead=0;
	sectorRead=0;
	samplesToPlay.value=-1;
	mmc_player_status = MMC_PLAYER_STARTED;
}

void mmc_playerStop(){
	if(mmc_player_status==MMC_PLAYER_STARTED){
		mmc_send_command(12,0,0); //stop transfers
		mmc_clock_and_release();
	}	
	mmc_player_status = MMC_PLAYER_STOPPED;
}

u32 mmc_playerGetCurrentSector(){
	return mmc_player_sector;
}


u8 mmc_playerGetStatus(){
	return mmc_player_status;
}

void mmc_playerPause(){
	if(mmc_player_status == MMC_PLAYER_STARTED)
	{		
		mmc_send_command(12,0,0); //stop transfers
		mmc_clock_and_release();		
		mmc_player_status = MMC_PLAYER_PAUSED;
	}
}

void mmc_playerResume(){
	if(mmc_player_status == MMC_PLAYER_PAUSED)
	{
		mmc_send_command(18,(mmc_player_sector>>7) & 0xffff, (mmc_player_sector<<9) & 0xffff);	
		playerRead=0;
		sectorRead=0;
		mmc_player_status = MMC_PLAYER_STARTED;
	}
}

u8 mmc_masterInit(){
	return mmc_init(sector.buffer);
}

//call once on each vsync
void mmc_playerProcess()
{

	u8 *buf=mix_buf;

	if(mix_bank==1)
		buf+=MIX_BANK_SIZE;
	
	if(mmc_player_status == MMC_PLAYER_STARTED){

		while(1){
		
			if(sectorRead==0){				
				if (mmc_datatoken() != 0xfe)	// if no valid token
				{
				    mmc_clock_and_release();	// cleanup
					//TODO: return error condition
					mmc_player_status = MMC_PLAYER_ERROR;
					return;
				}
			}
			
		
			if(samplesToPlay.value==-1){
				//skip the WAV header bytes
				for(u8 i=0;i<40;i++){ 
					spi_byte(0xff);		
				}				

				//get wave lenght				
				samplesToPlay.bytes[0]=spi_byte(0xff);
				samplesToPlay.bytes[1]=spi_byte(0xff);				
				samplesToPlay.bytes[2]=spi_byte(0xff);				
				samplesToPlay.bytes[3]=spi_byte(0xff);	
				
				//advance sector read counter								
				sectorRead=44;
			}

		
			do{			
				// read sector data
				*buf++=spi_byte(0xff);			 
				samplesToPlay.value--;				
				playerRead++;
				sectorRead++;

			}while(playerRead<MIX_BANK_SIZE && sectorRead<512 && samplesToPlay.value>0);
	
			if(samplesToPlay.value==0){
				
				//sample completed
				//clear remaining half-buffer
				do{
					*buf++=	0x80;
					playerRead++;
				}while(playerRead<MIX_BANK_SIZE);

				mmc_playerStop();
				return;
			}


			if(sectorRead==512){
				//sector completed
				spi_byte(0xff);					// ignore dummy checksum
				spi_byte(0xff);					// ignore dummy checksum			    
			
				mmc_player_sector++;
				sectorRead=0;
			}

			if(playerRead==MIX_BANK_SIZE){
				playerRead=0;
				return;		
			}


		};

	}else{
		//clear half-buffer
		for(playerRead=0;playerRead<MIX_BANK_SIZE;playerRead++){
			*buf++ = 0x80;
		}
	}
	
	
}
