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
#include <avr/interrupt.h>

#include "data/main.pic.inc"
#include "data/font8x8.pic.inc"
#include "data/font8x8blue.pic.inc"
#include "data/sprites.pic.inc"

#include "data/main.map.inc"
#include "data/sprites.map.inc"


extern void mmc_mixerStart(uint32_t lba);
extern void mmc_mixerStop();
extern uint32_t mmc_mixerGetCurrentSector();
extern void processMouseMovement(void);

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

union SectorData {
	unsigned char buffer[512];
	BootRecord bootRecord;
	MBR mbr;
	DirectoryTableEntry files[16];
	RIFFheader riffHeader;
} sector;


typedef struct{
	unsigned char filename[13]; //filename+'.'+ext+0
	unsigned long firstSector;
	unsigned long fileSize;	
} File;



long GetFileSector(DirectoryTableEntry *file);
void LoadRootDirectory(unsigned char *buffer);
void PrintSector(unsigned char *buffer);
char init();
void printDigits(unsigned long currentSectorNo,unsigned long songSize );
void animateTextLine(bool reset);
void printFileTime(unsigned char x,unsigned char y,unsigned long songSize);
void loadInfoBlock(File *file);
int findTag(unsigned long absAdress,unsigned char *tag,unsigned char *dest,unsigned int destLenght,unsigned char dataOffset);
unsigned char getChar(unsigned long absAdress);
int loadWaveInfoBlock(File *file);
void buttonHandler(unsigned char buttonIndex,unsigned char eventType);
void startSong();
void stopSong();
void pauseSong();
void rewindSong();
void forwardSong();



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

File files[16];
Button buttons[6];


bool playing=false;
bool cueing=false,mouseCueing=false;
unsigned long sectorNo=0;
unsigned int cur,playingFile=0,fileCount=0,x,y;
unsigned long songSize=0;
bool cardDetected=false;
bool useMouse=false;
unsigned char debugNo=0;

void debug(char no){

	PrintHexByte(2+(no*3),2,debugNo);

}

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

int main(){
	unsigned char c,sliderX=16,sliderY=76;
	unsigned int joy,i,j,k,pos,actionButton;
	long loc=0,newLoc=0;


	infoSong[0]=0;
	

	SetTileTable(main_tileset);
	SetSpritesTileTable(sprites_tileset);
	SetFontTilesIndex(MAIN_TILESET_SIZE);
	if(init()==0){ //init the mmc
		cardDetected=true;
	}

	//SetSpriteVisibility(true);

	if(EnableSnesMouse(SPR_MOUSE,map_mouse)==0){
		useMouse=true;
	}

//	SetMouseSensitivity(MOUSE_SENSITIVITY_HIGH);
	actionButton=GetActionButton();

	DrawMap2(0,0,map_main);
	
	createButton(&buttons[BTN_REWIND],2,11,map_btnPrevNormal,map_btnPrevPushed);
	createButton(&buttons[BTN_PLAY],5,11,map_btnPlayNormal,map_btnPlayPushed);
	createButton(&buttons[BTN_PAUSE],8,11,map_btnPauseNormal,map_btnPausePushed);
	createButton(&buttons[BTN_STOP],11,11,map_btnStopNormal,map_btnStopPushed);
	createButton(&buttons[BTN_FORWARD],14,11,map_btnNextNormal,map_btnNextPushed);
	createAreaButton(&buttons[BTN_FILES], 2,13,26,14);
	registerButtonHandler(buttonHandler,buttons);



	for(i=0;i<11;i++){
		DrawMap2(3+(i*2),4,map_digitBlank);
	}

	

	printDigits(0,0);
	SetFontTilesIndex(MAIN_TILESET_SIZE+FONT_TILESET_SIZE);
	
	
	if(!cardDetected) noCard();

	Print(20,4,PSTR("SECTOR"));
	LoadRootDirectory(sector.buffer);
	

	x=3;y=14;cur=0;
	MapSprite(SPR_SONG_CUR,map_hCursor);
	SetFontTilesIndex(MAIN_TILESET_SIZE);

	//find files in the sector
	for(i=0;i<16;i++){
		if((sector.files[i].fileAttributes & (FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME|FAT_ATTR_DIRECTORY|FAT_ATTR_DEVICE))==0){
			if((sector.files[i].filename[0]!=0) && (sector.files[i].filename[0]!=0xe5) && (sector.files[i].filename[0]!=0x05) && (sector.files[i].filename[0]!=0x2e)){									
				
				pos=0;
				for(j=0;j<8;j++){
					c=sector.files[i].filename[j];
					if(c==0x20)break;
					if(c=='~')c='_';
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
				//PrintLong(x+21,y+fileCount,files[fileCount].fileSize);
				printFileTime(x+16,y+fileCount,(files[fileCount].fileSize/512));

				
				fileCount++;
				if(fileCount==15) break; //can't fit more than 13 for now			
			}				
		}
	}


	c='^';
	MoveSprite(SPR_SONG_CUR,sliderX,sliderY,2,1);

	
	unsigned char mouseX,mouseY;
  	while (1)
  	{
		WaitVsync(1);

		processButtons();		


		mouseX=GetMouseX();
		mouseY=GetMouseY();




		animateTextLine(false);
		PrintHexLong(19,5,sectorNo);
		SetFontTilesIndex(MAIN_TILESET_SIZE);


		if(!cueing && playing) sectorNo=mmc_mixerGetCurrentSector();



		joy=ReadJoypad(0);


		//PrintInt(6,1,joy&actionButton,true);
		//PrintHexByte(6,2,(char)cueing);

		//handle slider
		if(joy&actionButton){
			//PrintByte(20,2,mouseX,true);		
		//	PrintByte(25,2,mouseY,true);
		//	PrintByte(20,3,loc,true);		
		//	PrintByte(25,3,newLoc,true);

			if((mouseX>=sprites[SPR_SONG_CUR].x && mouseX<(sprites[SPR_SONG_CUR].x+16) && mouseY>=76 && mouseY<=82) || cueing){
			
					cueing=true;
					newLoc=mouseX-sliderX;
					if(newLoc<0) newLoc=0;
					if(newLoc>=(sliderX+196)) newLoc=(sliderX+196)-1;
					sectorNo=((newLoc*songSize)/196)+files[playingFile].firstSector;
					//MoveSprite(SPR_SONG_CUR,sliderX+loc,sliderY,2,1);
				
			}
		}



		if(useMouse){
		
	
					
			if( (joy&actionButton)==0 && cueing==true){
				cueing=false;
				mmc_mixerStart(sectorNo);
			}



		}else{
		
			if(joy&BTN_START){
				if(playing){
					mmc_mixerStop();
					playing=false;
				}else{
					if(cur!=playingFile || sectorNo==0){				
						playingFile=cur;
						sectorNo=files[playingFile].firstSector+1; //skip WAV header
					}
					songSize=files[playingFile].fileSize/512;

					loadWaveInfoBlock(&files[playingFile]);
					animateTextLine(true);
					mmc_mixerStart(sectorNo);
					playing=true;
				
				}
				while(ReadJoypad(0)!=0);
			}


			if(joy&BTN_DOWN){
				PrintChar(x-1,y+cur,' ');
				if(cur<fileCount)cur++;
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



			if(joy&BTN_SL){
				if(playing){
					sectorNo-=100; //fast-forward
					if(joy&BTN_A)sectorNo-=400;//fast-forward even faster
					if(sectorNo<files[playingFile].firstSector){
						sectorNo=files[playingFile].firstSector;
					}
					cueing=true;
				}
			}

			if(joy&BTN_SR){
				if(playing){				
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
				mmc_mixerStart(sectorNo);
			}

		
		}


		if(playing){

		//	if(anim&16){
		//		c=' ';
		//	}else{
		//		c='^';
		//	}
		//	anim++;



			loc=(196*(sectorNo-files[playingFile].firstSector))/songSize;
			if(loc<196){
				MoveSprite(SPR_SONG_CUR,sliderX+loc,sliderY,2,1);
			}else{
				loc=196;
			}

			if(sectorNo>=(songSize+files[playingFile].firstSector) && !cueing){
				mmc_mixerStop();
				playing=false;
			}
			
			printDigits(sectorNo-files[playingFile].firstSector,songSize );



		}

		PrintChar(x-1,y+cur,'^');

	



   }

} 

void startSong(){
		if(playing) stopSong();

		playingFile=cur;
		sectorNo=files[playingFile].firstSector+1; //skip WAV header
		songSize=files[playingFile].fileSize/512;

		loadWaveInfoBlock(&files[playingFile]);
		animateTextLine(true);
		
		playing=true;
		mmc_mixerStart(sectorNo);
		
}

void stopSong(){
	mmc_mixerStop();
	playing=false;
}
void pauseSong(){
	if(playing){
		mmc_mixerStop();
		playing=false;
	}else{
		mmc_mixerStart(sectorNo);
		playing=true;		
	}
}
void rewindSong(){
	if(playing){
		sectorNo-=100; //fast-forward
		//if(joy&BTN_A)sectorNo-=400;//fast-forward even faster
		if(sectorNo<files[playingFile].firstSector){
			sectorNo=files[playingFile].firstSector;
		}
		cueing=true;
	}
}
void forwardSong(){
	if(playing){				
		sectorNo+=100;//fast-forward
		//if(joy&BTN_A)sectorNo+=400;//fast-forward even faster
		if(sectorNo>(files[playingFile].firstSector+songSize)){
			sectorNo=files[playingFile].firstSector+songSize;
		}
		cueing=true;
	}
}


void buttonHandler(unsigned char btn,unsigned char event){
	//PrintHexByte(20,10,btn);
	//PrintHexByte(25,10,event);
	
	switch(event){
		case BUTTON_DBLCLICK:			
			if(btn==BTN_FILES)startSong();
			break;

		case BUTTON_CLICK:
			switch(btn){
				case BTN_PLAY:
					startSong();
					break;
				case BTN_PAUSE:
					pauseSong();
					break;
				case BTN_STOP:
					stopSong();
					break;	
			}
			break;
					
		case BUTTON_DOWN:
			switch(btn){
				case BTN_REWIND:

					flipSkin();
					break;
				case BTN_FORWARD:
					flipSkin();
					break;
				case BTN_FILES:
					//position the cursor on the clicked file name				
					if(GetMouseY()>=112 && GetMouseY()<(112+(fileCount*8))){
						PrintChar(x-1,y+cur,' ');
						cur=(GetMouseY()-112)/8;	
					}
			}
			break;

	}
	
}

unsigned char getChar(unsigned long absAdress){
	static unsigned long lastSector=0;
	unsigned char c;
	
	unsigned long sectorNo=(absAdress/512);
	
	if(sectorNo!=lastSector){
		mmc_readsector(sectorNo, sector.buffer);	
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

/*
Returns:
0=success. Wave file detected and tags loaded
1=Wave file detected, but no tags were present.
-1=not a wave file.
*/
int loadWaveInfoBlock(File *file){
	unsigned long infoBlockStartAddr;
	unsigned char tag[4],i,pos,c;
	
	mmc_readsector(file->firstSector, sector.buffer);
	
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


void animateTextLine(bool reset){

	static unsigned char pos=0,wait=0;
	unsigned char curPos=pos,c;
	SetFontTilesIndex(MAIN_TILESET_SIZE+FONT_TILESET_SIZE);
	
	if(reset){
		pos=0;
		curPos=0;
		if(infoSongLen<=24){
			for(char i=0;i<24;i++)PrintChar(3+i,7,' ');
			PrintRam(3,7,infoSong);
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
				PrintChar(i+3,7,c);
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
	unsigned char digit1, digit2,x=3;
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

char init(){	
	unsigned char temp;
	int timeout=0;

	SetFontTilesIndex(MAIN_TILESET_SIZE);
	ClearVram();
//	cli();

	do { temp = mmc_init();
   		Print(3,4,temp? PSTR("INIT FAILED") : PSTR("INIT GOOD   ")); 
		timeout++;
		if(timeout>10){
			sei();
			return -1;
		}
   	}while (temp);
	
	timeout=0;

	do {
		temp = mmc_readsector(0, sector.buffer);
   		Print(3,5,temp? PSTR("FIRST READ FAILED") : PSTR("FIRST READ GOOD   ")); 
		timeout++;
		if(timeout>6000){
			sei();
			while(1);
		}
	}while (temp);

//	sei();
	return 0;
}

