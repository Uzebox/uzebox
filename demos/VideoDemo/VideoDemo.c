/*
 *  Uzebox(tm) Movie Player
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

/*
 * Uses video mode 7.
 */

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>
#include <mmc.h>
#include <fat.h>

#include "data/fonts.pic.inc"

extern unsigned char sync_phase;
extern unsigned char sync_pulse;
char init();
extern unsigned int data_delay;
//unsigned char sector[512]; //sd sector buffer

union SectorData {
	unsigned char buffer[512];
	DirectoryTableEntry files[16];
} sector;

//File files[16];

unsigned long fileSector;



extern unsigned char vram_buf[];
extern unsigned char internal_spi_byte(unsigned char c);
extern void StartPlayback();
extern void StopPlayback();
extern volatile unsigned char curr_field;

int main(){


	init();

//	while(1);

	LoadRootDirectory(sector.buffer);
	//LoadFiles(sector.buffer,files);

	//find the first .uzm file

	for(unsigned char i=0;i<16;i++){
		if((sector.files[i].fileAttributes & (FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME|FAT_ATTR_DIRECTORY|FAT_ATTR_DEVICE))==0){
			if((sector.files[i].filename[0]!=0) && (sector.files[i].filename[0]!=0xe5) && (sector.files[i].filename[0]!=0x05) && (sector.files[i].filename[0]!=0x2e)){									
				
				if(sector.files[i].extension[0]=='U' && sector.files[i].extension[1]=='Z' && sector.files[i].extension[2]=='M'){

					fileSector=GetFileSector(&sector.files[i]);
					break;

				}
						
			}				
		}
	}









	//fileSector=files[0].firstSector;

	// send the multiple block read command and logical sector address
	mmc_send_command(18,(fileSector>>7) & 0xffff, (fileSector<<9) & 0xffff);	
	mmc_datatoken();

	StartPlayback();

	while(1);

}





void flash(){
	PORTD^=(1<<PD4);
}

char init(){	
	unsigned char temp=0;
	int timeout=0;

	cli();

	do { 
		temp = InitFat(sector.buffer);
   		//Print(3,26,temp? PSTR("INIT FAILED") : PSTR("INIT GOOD   ")); 
		//timeout++;
		//if(timeout>10){
		//	sei();
		//	return -1;
		//}
		flash();
   	}while (temp);
	
	timeout=0;

	do {
		temp = mmc_readsector(0);
   	//	Print(20,26,temp? PSTR("FIRST READ FAILED") : PSTR("FIRST READ GOOD   ")); 
		//timeout++;
		//if(timeout>6000){
		//	sei();
		//	while(1){
		//		flash();
		//	}
		//}
		flash();
	}while (temp);
	
	sei();
	PORTD&=~(1<<PD4);	
	return 0;
}




