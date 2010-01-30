#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include "fat.h"

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


#define SECTOR_SIZE 512
#define FAT_DIRTABLE_ENTRIES_PER_SECTOR 16
#define FAT_DIRTABLE_ENTRY_SIZE 32

typedef enum {
	FR_OK = 0,			/* 0 */
	FR_DISK_ERR,		/* 1 */
	FR_INT_ERR,			/* 2 */
	FR_NOT_READY,		/* 3 */
	FR_NO_FILE,			/* 4 */
	FR_NO_PATH,			/* 5 */
	FR_INVALID_NAME,	/* 6 */
	FR_DENIED,			/* 7 */
	FR_EXIST,			/* 8 */
	FR_INVALID_OBJECT,	/* 9 */
	FR_WRITE_PROTECTED,	/* 10 */
	FR_INVALID_DRIVE,	/* 11 */
	FR_NOT_ENABLED,		/* 12 */
	FR_NO_FILESYSTEM,	/* 13 */
	FR_MKFS_ABORTED,	/* 14 */
	FR_TIMEOUT		/* 15 */
} FRESULT;

typedef struct{
		unsigned char filename[8]; //zero padded
		unsigned char extension[3];//
		unsigned char fileAttributes;
		unsigned int currentSector;
		unsigned int currentCluster;
		unsigned long currentFileSize;
		unsigned int firstClusterExt; //for fat32
		unsigned int lastModifiedTime;
		unsigned int lastModifiedDate;
		unsigned int firstCluster;
		unsigned long fileSize;

} FS_File;

union SectorData {
	unsigned char buffer[512];
} sector;



bool init();
extern void WaitVSync(unsigned char count);
extern void SetFont(char x,char y, unsigned char tileId,unsigned char bgColor);
extern void ClearVram(void);
extern uint8_t mmc_readsector(uint32_t lba);
extern void Print(unsigned int adress ,char *string, unsigned char bgColor);
extern void DrawBar(unsigned int adress,unsigned char len,unsigned char color);
void topMenu();
void PrintHexByte(char x,char y,unsigned char byte);
void PrintHexInt(char x,char y,uint16_t value);
FRESULT FS_ListDir(FS_File* file);
bool filter(FS_File* dirEntry);
FRESULT FS_ReadBlock(FS_File* file,int* bytesRead);
void PrintHexLong(char x,char y,uint32_t value);

extern volatile unsigned int joypad_status;
extern volatile unsigned char vsync_flag;
extern volatile unsigned char wave_vol;
extern volatile unsigned char first_render_line_tmp;
extern volatile unsigned char screen_tiles_v_tmp;
extern unsigned int vram[];  

extern uint16_t max_root_dir_entries;
extern uint16_t current_dir_sector;
extern uint8_t sectors_per_cluster;
extern uint32_t cur_dir_lba;
extern uint8_t current_dir_entry;
extern uint32_t data_start_lba;
extern uint32_t fat_table_lba;


char strDemo[] PROGMEM = ">> Uzebox gameloader Menu Alpha1 <<";


int main(){

	uint8_t y,dx,x,fileNo=0,c;
	uint16_t j=0;

//	uint32_t test=0xaabbccdd;
//	mmc_readsector(test);



	topMenu();
	init();
	topMenu();



	FS_File file;
	FRESULT res;
	x=2;y=15;
	
	while(1){
		res=FS_ListDir(&file);
		if(file.filename[0]==0) break;
		if(res==FR_OK){
			if(filter(&file)){
				
				j=0;
				for(dx=0;dx<8;dx++){
					c=file.filename[j++];
					if(c=='~') c='-';
					vram[(y*40)+x+dx]=c-32;
				}
				y++;
				if(y==25){
					y=5;
					x+=9;
				}

				break;
			}
		}else{
				Print((26*40*2)+(7*2),PSTR("LIST FILE FAIL"),0);
		}
	}

	int bytesRead;


	while(1){


			x=2;y=2;
			res=FS_ReadBlock(&file,&bytesRead);
			if(res==0){

				for(j=0;j<512;j++){
					c=sector.buffer[j];
					if(c<32 || c>'z')c=32;
					if(c>='a' && c<='z')c&=~0x20;
					vram[(y*40)+x]=c-32;
					
					x++;
					if(x>=38){
						x=2;
						y++;
					}
				}

				PrintHexInt(30,22,bytesRead);

			}else{
				Print((26*40*2)+(7*2),PSTR("BLOCK READ FAIL"),0);
			}
		
		
			while(1){
				if(joypad_status&BTN_RIGHT){
					while(joypad_status!=0){};
					break;
				}

				if(joypad_status&BTN_SR){
					break;
				}
			};

		
	}

	unsigned char col=1,dir=1;
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

	}

	while(1);
}








//Print a byte in hexadecimal
void PrintHexByte(char x,char y,unsigned char byte){
	unsigned char nibble;

	//hi nibble	
	nibble=(byte>>4);
	if(nibble<=9){
		vram[(y*40)+x]=(nibble+16);
	}else{
		vram[(y*40)+x]=(nibble+16+7);
	}

	//lo nibble	
	nibble=(byte&0xf);
	if(nibble<=9){		
		vram[(y*40)+(x+1)]=(nibble+16);
	}else{
		vram[(y*40)+(x+1)]=(nibble+16+7);
	}

}

//Print a hexdecimal integer
void PrintHexInt(char x,char y,uint16_t value){
	PrintHexByte(x,y, (unsigned int)value>>8);
	PrintHexByte(x+2,y,value&0xff);
}

//Print a hexdecimal long
void PrintHexLong(char x,char y,uint32_t value){
	PrintHexInt(x,y, (uint32_t)value>>16);
	PrintHexInt(x+4,y,value&0xffff);
}

void topMenu(){
	ClearVram();
	DrawBar(0,40,0x50);
	Print((3*2),strDemo,0x50);
}


bool filter(FS_File* dirEntry){

	if((dirEntry->fileAttributes & (FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME|FAT_ATTR_DIRECTORY|FAT_ATTR_DEVICE))==0){
		
		if(	dirEntry->filename[0]!=0xe5 && 
			dirEntry->filename[0]!=0x05 && 
			dirEntry->filename[0]!=0x2e &&
			dirEntry->extension[0]=='T' &&
			dirEntry->extension[1]=='X' &&
			dirEntry->extension[2]=='T' 

			&& dirEntry->filename[0]=='2'
			&& dirEntry->filename[1]=='A'
		)return true;					

	}

	return false;
}


bool init(){
	unsigned char temp;

   	do{ 
		//temp = mmc_init(sector.buffer);
		temp = fat_init(sector.buffer);
    	Print((12*40*2)+(7*2),temp? PSTR("SD INIT FAIL: NO CARD?") : PSTR("SD INIT OK"),0);  
						 
	} while (temp);

	return 0;
}



FRESULT FS_ListDir(FS_File* file){

	while(current_dir_sector<sectors_per_cluster){
		
		mmc_readsector(cur_dir_lba+current_dir_sector);	
				
		while(current_dir_entry<FAT_DIRTABLE_ENTRIES_PER_SECTOR){

			for(uint8_t i=0;i<FAT_DIRTABLE_ENTRY_SIZE;i++){
				((uint8_t*)file)[i]=sector.buffer[(current_dir_entry*FAT_DIRTABLE_ENTRY_SIZE)+i];
			}
			
			if(file->filename[0]==0){
				return FR_OK; //no more directory entries
			}else{
				current_dir_entry++;
				file->currentFileSize=0;
				file->currentSector=0;
				file->currentCluster=file->firstCluster-2;
				return FR_OK;
			}
		}
		
	

		current_dir_entry=0;		
		current_dir_sector++;	
		
	}
	
	//todo add cluster support?
	return FR_INT_ERR;
}

uint16_t cnt=0;
FRESULT FS_ReadBlock(FS_File* file,int* bytesRead){
//max_root_dir_entries

	uint32_t sec=data_start_lba+((uint32_t)file->currentCluster*sectors_per_cluster)+file->currentSector;
	
	PrintHexLong(2,20,sec);
	PrintHexLong(2,21,file->currentFileSize);
	PrintHexLong(2,22,file->fileSize);
	PrintHexInt(2,23,sectors_per_cluster);
	PrintHexInt(2,24,file->currentSector);
	PrintHexInt(2,25,file->currentCluster);
	PrintHexInt(2,26,cnt++);
	
	PrintHexLong(30,20,fat_table_lba);		
	PrintHexInt(30,21,file->firstCluster);

	if((file->currentFileSize+512) >= file->fileSize){
		*bytesRead=(file->fileSize - file->currentFileSize);
		file->currentFileSize=file->fileSize;
	}else{
		*bytesRead=512;
		file->currentFileSize+=512;
		file->currentSector++;

		if(file->currentSector==sectors_per_cluster){
			//get next cluster in FAT16
			mmc_readsector(fat_table_lba+(file->currentCluster>>8));
			file->currentCluster=((int16_t*)sector.buffer)[file->currentCluster&0xff];
			file->currentSector=0;


		}		
	}

	//read last otherwise the FAT read will overwrite
	mmc_readsector(sec);

	return FR_OK;
}
