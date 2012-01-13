/*
 *  Uzebox(tm) Uzeamp
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
#include <avr/interrupt.h>
#include <mmc_player.h>

#include "data/main.pic.inc"
#include "data/font8x8.pic.inc"
#include "data/font8x8blue.pic.inc"
#include "data/sprites.pic.inc"

#include "data/main.map.inc"
#include "data/sprites.map.inc"

void DrawMap3(unsigned char x,unsigned char y,const char *map);


extern void processMouseMovement(void);



long GetFileSector(DirectoryTableEntry *file);
void LoadRootDirectory(unsigned char *buffer);
void PrintSector(unsigned char *buffer);
char init();
void printDigits(unsigned long currentSectorNo,unsigned long songSize );
void animateTextLine(bool reset);
void printFileTime(unsigned char x,unsigned char y,unsigned long songSize);
void loadInfoBlock(mmc_File *file);
int findTag(unsigned long absAdress,unsigned char *tag,unsigned char *dest,unsigned int destLenght,unsigned char dataOffset);
unsigned char getChar(unsigned long absAdress);
int loadWaveInfoBlock(mmc_File *file);
void buttonHandler(unsigned char buttonIndex,unsigned char eventType);
void startSong();
void stopSong();
void pauseSong();
void rewindSong();
void forwardSong();
bool isPlaying();



#define SPR_SONG_CUR 0
#define SPR_MOUSE 2

#define BTN_REWIND 0
#define BTN_PLAY 1
#define BTN_PAUSE 2
#define BTN_STOP 3
#define BTN_FORWARD 4
#define BTN_FILES 5


unsigned char infoTemp[48]; 
unsigned char infoSong[104];
unsigned char infoSongLen=0;
unsigned int soundDataStart;

extern mmc_SectorData sector;
mmc_File files[16];

void noCard(){
	SetFontTilesIndex(MAIN_TILESET_SIZE);
	Print(5,19,PSTR("NO SD CARD DETECTED!"));
	while(1);
}


/*
0xFF = Metal
0xFC = ATLANTIS
0xE7 = PURPLE HAZE
0xBE = GOLD
0xBD = FOrest
0x77 = Freak
0x3E = ST3
0x27 = Red Alert
*/

unsigned char skins[]={0xff,0xfc,0xe7,0xbe,0xbd,0x77,0x3e,0x27};
const char skinNames[8][11]={"METAL","ATLANTIS","PURPLE HAZE","GOLDFINGER","FOREST","FREAK","ST3","RED ALERT"};



unsigned char currSkin=0;
void flipSkin(){
	DDRC=skins[currSkin];
	
	//PrintHexByte(2,1,skins[currSkin]);
	//PrintBinaryByte(5,1,skins[currSkin]);
	currSkin++;
	if(currSkin==sizeof(skins))currSkin=0;
}

void VsyncCallBack(void){
	mmc_playerProcess();
}

bool cueing=false,mouseCueing=false;
unsigned long sectorNo=0;
unsigned int cur,playingFile=0,fileCount=0,x,y;
unsigned long songSize=0;
bool cardDetected=false;
bool useMouse=false;
unsigned char debugNo=0;


#define LEFT 1
int main(){


	unsigned char c;
	unsigned int joy,i;

	SetUserPreVsyncCallback(&VsyncCallBack);

	infoSong[0]=0;
	
	SetTileTable(main_tileset);
	SetFontTilesIndex(MAIN_TILESET_SIZE);


	if(init()==0){ //init the mmc
		cardDetected=true;
	}


	DrawMap3(0+LEFT,0,map_main);

	for(i=0;i<11;i++){
		DrawMap2(2+(i*2)+LEFT,4,map_digitBlank);
	}


	printDigits(0,0);
	SetFontTilesIndex(MAIN_TILESET_SIZE+FONT_TILESET_SIZE);
	
	if(!cardDetected) noCard();

	Print(19+LEFT,4,PSTR("SECTOR"));

	x=2+LEFT;y=14;cur=0;
	SetFontTilesIndex(MAIN_TILESET_SIZE);
	
	//load wave file references
	u8 count=mmc_listDir(files,16,"WAV");
	for(i=0;i<count;i++){
		PrintRam(x,y+i,files[i].filename);
		printFileTime(x+16,y+i,(files[i].fileSize/512));	
	}


	c='^';

  	while (1)
  	{
		WaitVsync(1);

		animateTextLine(false);
		PrintHexLong(18+LEFT,5,sectorNo);
		SetFontTilesIndex(MAIN_TILESET_SIZE);

		if(!cueing && isPlaying()) sectorNo=mmc_playerGetCurrentSector();

		joy=ReadJoypad(0);
	
		if(joy&BTN_START){
			if(isPlaying()){
				mmc_playerStop();
			}else{
				if(cur!=playingFile || sectorNo==0){				
					playingFile=cur;
				}
				startSong();
			
			}
			while(ReadJoypad(0)!=0);
		}


		if(joy&BTN_DOWN){
			PrintChar(x-1,y+cur,' ');
			if(cur<(fileCount-1))cur++;
			while(ReadJoypad(0)!=0);
		}

		if(joy&BTN_UP){
			PrintChar(x-1,y+cur,' ');
			if(cur>0)cur--;
			while(ReadJoypad(0)!=0);
		}

		if(joy&BTN_X){
			flipSkin();
			while(ReadJoypad(0)!=0);
		}

		if(joy&BTN_B){
			if(mmc_playerGetStatus()==MMC_PLAYER_STARTED){
				mmc_playerPause();
			}else if(mmc_playerGetStatus()==MMC_PLAYER_PAUSED){
				mmc_playerResume();
			}		

			while(ReadJoypad(0)!=0);
		}


		if(joy&BTN_SL){
			if(isPlaying()){
				sectorNo-=100; //fast-forward
				if(joy&BTN_A)sectorNo-=400;//fast-forward even faster
				if(sectorNo<files[playingFile].firstSector){
					sectorNo=files[playingFile].firstSector;
				}
				cueing=true;
			}
		}

		if(joy&BTN_SR){
			if(isPlaying()){				
				sectorNo+=100;//fast-forward
				if(joy&BTN_A)sectorNo+=400;//fast-forward even faster
				if(sectorNo>(files[playingFile].firstSector+songSize)){
					sectorNo=files[playingFile].firstSector+songSize;
				}
				cueing=true;
			}
		}
	
		if( (joy&BTN_SL)==0 && (joy&BTN_SR)==0 && cueing==true){
			cueing=false;
			//mmc_mixerStart(sectorNo);
		}


		if(isPlaying()){
			printDigits(sectorNo-files[playingFile].firstSector,songSize );
		}

		PrintChar(x-1,y+cur,'^');

   }

} 

bool isPlaying(){
	return  (mmc_playerGetStatus()==MMC_PLAYER_STARTED);
}

void startSong(){
	if(isPlaying()) stopSong();

	loadWaveInfoBlock(&files[playingFile]);
	animateTextLine(true);	
	mmc_playerStart(files[playingFile]);
		
}

void stopSong(){
	mmc_playerStop();
}


void rewindSong(){
	if(isPlaying()){
		sectorNo-=100; //fast-forward
		//if(joy&BTN_A)sectorNo-=400;//fast-forward even faster
		if(sectorNo<files[playingFile].firstSector){
			sectorNo=files[playingFile].firstSector;
		}
		cueing=true;
	}
}
void forwardSong(){
	if(isPlaying()){				
		sectorNo+=100;//fast-forward
		//if(joy&BTN_A)sectorNo+=400;//fast-forward even faster
		if(sectorNo>(files[playingFile].firstSector+songSize)){
			sectorNo=files[playingFile].firstSector+songSize;
		}
		cueing=true;
	}
}




void animateTextLine(bool reset){

	static unsigned char pos=0,wait=0;
	unsigned char curPos=pos,c;
	SetFontTilesIndex(MAIN_TILESET_SIZE+FONT_TILESET_SIZE);
	
	if(reset){
		pos=0;
		curPos=0;
		if(infoSongLen<=24){
			for(char i=0;i<24;i++)PrintChar(2+i,7,' ');
			PrintRam(2+LEFT,7,infoSong);
		}
		wait=20;
	}
	
	if(wait>=20){

		if(infoSongLen>24){
			for(int i=0;i<24;i++){
				if(curPos>=infoSongLen){
					c=32;
				}else{
					c=infoSong[curPos];
				}
				PrintChar(i+2+LEFT,7,c);
				curPos++;
				if(curPos>=(infoSongLen+4)) curPos=0;
			}

			wait=0;
			pos++;
			if(pos>=(infoSongLen+4)) pos=0;
		}
	}
	
	wait++;

}

void printDigits(unsigned long currentSectorNo,unsigned long songSize ){
	unsigned long hours,minutes,seconds,temp;
	unsigned char digit1, digit2,x=2+LEFT;
	//15734 bytes/sec @ 512bytes/sector
	//31 sectors/sec
	//1844 sectors/min
	//110630 sectors/hour

	hours=currentSectorNo/110630;
	temp=(currentSectorNo-(hours*110630));
	minutes=temp/1844;
	seconds=(temp-(minutes*1844))/31;
	
	//print hours
	digit1=(hours/10)%10;
	digit2=hours%10;
	DrawMap2(x+0,4,map_digit0+(digit1*8));
	DrawMap2(x+2,4,map_digit0+(digit2*8));
	DrawMap2(x+4,4,map_digitSep);
	digit1=(minutes/10)%10;
	digit2=minutes%10;
	DrawMap2(x+5,4,map_digit0+(digit1*8));
	DrawMap2(x+7,4,map_digit0+(digit2*8));
	DrawMap2(x+9,4,map_digitSep);
	digit1=(seconds/10)%10;
	digit2=seconds%10;
	DrawMap2(x+10,4,map_digit0+(digit1*8));
	DrawMap2(x+12,4,map_digit0+(digit2*8));

}


void printFileTime(unsigned char x,unsigned char y,unsigned long songSize){
	unsigned long hours,minutes,seconds,temp;
	unsigned char digit1, digit2;
	//15734 bytes/sec @ 512bytes/sector
	//31 sectors/sec
	//1844 sectors/min
	//110630 sectors/hour

	hours=songSize/110630;
	temp=(songSize-(hours*110630));
	minutes=temp/1844;
	seconds=(temp-(minutes*1844))/31;
	
	//print hours
	digit1=(hours/10)%10;
	digit2=hours%10;
	if(hours>0 && digit1>0) PrintChar(x,y,digit1+'0');
	if(hours>0) PrintChar(x+1,y,digit2+'0');
	if(hours>0) PrintChar(x+2,y,':');

	digit1=(minutes/10)%10;
	digit2=minutes%10;
	if(hours>0 || digit1>0)PrintChar(x+3,y,digit1+'0');
	if(hours>0 || minutes>0)PrintChar(x+4,y,digit2+'0');
	PrintChar(x+5,y,':');

	digit1=(seconds/10)%10;
	digit2=seconds%10;
	PrintChar(x+6,y,digit1+'0');
	PrintChar(x+7,y,digit2+'0');


}



char init(){	
	unsigned char temp;
	int timeout=0;

	SetFontTilesIndex(MAIN_TILESET_SIZE);

	Print(3,17,PSTR("INITIALIZING SD..."));



	do { temp = mmc_masterInit();
		timeout++;
		if(timeout>10){
			Print(21,17,PSTR("FAILED"));
			return -1;
		}
   	}while (temp);
	
	Print(21,17,PSTR("OK"));
	timeout=0;

	do {
		temp = mmc_readsector(0);
		timeout++;
		if(timeout>6000){
   			Print(8,18,PSTR("FIRST READ FAILED")); 
			while(1);
		}
	}while (temp);

	ClearVram();
	return 0;
}


//Draws a map of tile at the specified position using an X offset in the map (cheap hack) 
//because the map was initially created with mode3 at 30 tile wide. The new one is only 28 wide.

void DrawMap3(unsigned char x,unsigned char y,const char *map){
	unsigned char i;
	unsigned char mapWidth=pgm_read_byte(&(map[0]));
	unsigned char mapHeight=pgm_read_byte(&(map[1]));

	for(unsigned char dy=0;dy<mapHeight;dy++){
		for(unsigned char dx=0;dx<28;dx++){
			
			i=pgm_read_byte(&(map[(dy*mapWidth)+dx+1+2]));
			
			vram[((y+dy)*VRAM_TILES_H)+x+dx]=(i + RAM_TILES_COUNT) ;
			
		
		}
	}

}




unsigned char getChar(unsigned long absAdress){
	static unsigned long lastSector=0;
	unsigned char c;
	
	unsigned long sectorNo=(absAdress/512);
	
	if(sectorNo!=lastSector){
		mmc_readsector(sectorNo);	
		lastSector=sectorNo;
	}
	
	c=sector.buffer[absAdress-(sectorNo*512)];
	if(c>=97) c-=32;
	return c;
}

//search max 2 sectors
int findTag(unsigned long absAdress,unsigned char *tag,unsigned char *dest,unsigned int destLenght,unsigned char dataOffset){
	unsigned long i=absAdress,size;
	int j;

	
	while(1){
		if( getChar(i)==tag[0] && getChar(i+1)==tag[1] && getChar(i+2)==tag[2] && getChar(i+3)==tag[3]){
			if(dest!=NULL){
				i+=4;
				//get info size
				size=getChar(i)+(getChar(i+1)<<8)+(getChar(i+2)<<8)+(getChar(i+3)<<8);
				if(size>=(destLenght-1))size=(destLenght-1);
				i+=dataOffset;

				for(j=0;j<size;j++){
					dest[j]=getChar(i+j);
				}
				dest[j]=0;
			}
			return 0; //found!
		}
		
		i++;
		if(i>(absAdress+1024)){
			dest[0]=0;
			return -1; //not found!
		}
	}

	

}




int loadWaveInfoBlock(mmc_File *file){
	unsigned long infoBlockStartAddr;
	unsigned char tag[4],i,pos,c;
	
	mmc_readsector(file->firstSector);
	
	//check if its a valid WAV file
	if(sector.riffHeader.format[0]=='W' && sector.riffHeader.format[1]=='A' && sector.riffHeader.format[2]=='V' && sector.riffHeader.format[3]=='E'){
		infoBlockStartAddr=sector.riffHeader.subchunk2Size+45;
	
		soundDataStart=44;

		tag[0]='I';
		tag[1]='N';
		tag[2]='F';
		tag[3]='O';
		if(findTag((file->firstSector*512)+infoBlockStartAddr,tag,NULL,0,0)==0){
			tag[0]='I';
			tag[1]='A';
			tag[2]='R';
			tag[3]='T';
			findTag((file->firstSector*512)+infoBlockStartAddr,tag,infoTemp,sizeof(infoTemp),4);
			//append to the concatenated song name
			pos=0;
			if(infoTemp[0]!=0){
				i=0;
				while(i<sizeof(infoTemp)){
					c=infoTemp[i++];
					if(c==0) break;
					infoSong[pos++]=c;
				}
				infoSong[pos++]=' ';
				infoSong[pos++]='-';
				infoSong[pos++]=' ';
			}

			//PrintRam(5,1,infoTemp);

			tag[0]='D';
			tag[1]='I';
			tag[2]='S';
			tag[3]='P';
			findTag((file->firstSector*512)+infoBlockStartAddr,tag,infoTemp,sizeof(infoTemp),8);

			if(infoTemp[0]!=0){
				i=0;
				while(i<sizeof(infoTemp)){
					c=infoTemp[i++];
					if(c==0) break;
					infoSong[pos++]=c;
				}
			}
			infoSong[pos]=0;
			infoSongLen=pos;
			
			//PrintRam(2,1,infoSong);

			return 0;
		}else{
			infoSongLen=0;
			infoSong[0]=0;
			soundDataStart=0;
			return 1;
		}	
	}

	return -1;
}

