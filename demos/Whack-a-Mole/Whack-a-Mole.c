/*
 *  Uzebox(tm) Whack-a-Mole
 *  Copyright (C) 2009  Alec Bourque
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
#include <gui.h>

#include "data/Whacksong.inc"

#include "data/patches.inc"
#include "data/cursor.pic.inc"
#include "data/cursor.map.inc"

#include "data/mole.map.inc"
#include "data/mole.pic.inc"
#include "data/fonts_8x8.pic.inc"

#include "data/title.pic.inc"
#include "data/fonts_8x8_2.pic.inc"
#include "data/title.map.inc"


#define FIELD_TOP 5
#define BUTTONS_COUNT 1
#define BUTTON_UNPRESSED 0
#define BUTTON_PRESSED 1
#define MOUSE 2


char createMyButton(unsigned char x,unsigned char y,const char *normalMapPtr,const char *pushedMapPtr);
void processMouseMovement(void);
void processHammer(void);
void doStart();
void processMyButtons();
void processMoles();
void WaitKey();
void InitControllers();
void menu();
void PrintSpeed();

const char strNotDetected[] PROGMEM ="MOUSE NOT DETECTED!";
const char strHighScore[] PROGMEM="HI-SCORE:";
const char strWhacked[] PROGMEM="MOLES WHACKED:";
const char strTime[] PROGMEM="TIME:";

const char strCopy[] PROGMEM="\\2009 UZE";
const char strStart[] PROGMEM="PRESS A BUTTON";
const char strWeb[] PROGMEM="HTTP://BELOGIC.COM/UZEBOX";

const char strSpeed[] PROGMEM="MOUSE SPD:";
const char strLo[] PROGMEM= "LOW ";
const char strMed[] PROGMEM="MED ";
const char strHi[] PROGMEM= "HIGH";

const char strError[] PROGMEM= "ERROR:";

struct ButtonStruct
{
	unsigned char id;
	unsigned char x;
	unsigned char y;
	unsigned char width;
	unsigned char height;
	const char *normalMapPtr;
	const char *pushedMapPtr;
	unsigned char state;
	bool clicked;
};
struct ButtonStruct buttons[BUTTONS_COUNT];
unsigned char nextFreeButtonIndex=0;


struct MoleStruct
{
	bool active;
	unsigned char animFrame;
	unsigned char x;
	unsigned char y;
	bool whacked;
};
struct MoleStruct moles[4][4];


struct EepromBlockSaveGameStruct{
	//some unique block ID
	unsigned int id;
		
	unsigned char top1name[6];
	unsigned int top1Score;
	unsigned char top2name[6];	
	unsigned int top2Score;
	unsigned char top3name[6];
	unsigned int top3Score;

	unsigned char data[6];		
};
struct EepromBlockSaveGameStruct saveGameBlock;

extern unsigned char playDevice,playPort;
extern unsigned int actionButton;
int mx,my;
char dx=0,dy=0;
unsigned char highScore=0,molesWhacked=0,activeMoles=0,level,mouseSpeed=MOUSE_SENSITIVITY_MEDIUM;
unsigned int time=0,lastTime=0;
bool gameOver;

int main(){
	unsigned char x,y,tmp;
    SetTileTable(title_tileset);
    ClearVram();

	SetSpritesTileTable(cursor_tileset);

	EnableSnesMouse(0,map_cursor);
	InitControllers();
	InitMusicPlayer(patches);
	SetMouseSensitivity(mouseSpeed);

	
	menu();


	srand(TCNT1);

    SetTileTable(mole_tileset);
	SetFontTilesIndex(MOLE_TILESET_SIZE);

    ClearVram();

    Print(2,1,strHighScore);
	PrintByte(13,1,highScore,true);

	PrintSpeed();


	mx=120;
	my=100;

	Print(12-5,3,strWhacked);
	PrintByte(28-5,3,molesWhacked,true);

	Print(2+10,4,strTime);
	PrintByte(9+10,4,(time/60),true);

	DrawMap2(2,FIELD_TOP,map_main);

	//create holes
	for(y=0;y<4;y++){
		for(x=0;x<4;x++){
			DrawMap2((x*6)+4,(y*4)+FIELD_TOP+3,map_hole);
			moles[y][x].whacked=false;
			moles[y][x].animFrame=0;
			moles[y][x].active=false;
			moles[y][x].x=(x*6)+4;
			moles[y][x].y=(y*4)+FIELD_TOP+3;
		}
	
	}

	while(1){

		MapSprite(0,map_cursor);


		activeMoles=0;
		level=2;	
		molesWhacked=0;
		time=60*60;
		gameOver=false;


		doStart();

		

		Print(12-5,3,strWhacked);
		PrintByte(28-5,3,molesWhacked,true);

		Print(2+10,4,strTime);
		PrintByte(9+10,4,(time/60),true);


		do{
	   		WaitVsync(1);
			processMouseMovement();
			processHammer();
			processMoles();
	
			time--;
			tmp=(time/60);
			PrintByte(28-5,3,molesWhacked,true);
			PrintByte(9+10,4,tmp,true);
			level=((60-tmp)/6)+3;
			
			if(tmp<=5 && tmp!=lastTime && tmp!=0){
				TriggerFx(8,0xa0,true);	
			}

			lastTime=tmp;
		}while(tmp>0);

		gameOver=true;
		
		TriggerFx(9,0xa0,true);	
	

		while(activeMoles>0){
			WaitVsync(1);
			processMouseMovement();
			processMoles();
		
		}

		DrawMap2(11,FIELD_TOP+7+4,map_gameOver);

		if(molesWhacked>highScore){
			highScore=molesWhacked;
			PrintByte(13,1,highScore,true);
		}
	}

} 

void PrintSpeed(){
	Print(15,1,strSpeed);
	if(mouseSpeed==MOUSE_SENSITIVITY_LOW){
		Print(25,1,strLo);
	}else if(mouseSpeed==MOUSE_SENSITIVITY_HIGH){
		Print(25,1,strHi);
	}else{
		Print(25,1,strMed);
	}
}

void menu(){
	unsigned int joy;
    SetTileTable(title_tileset);
	SetFontTilesIndex(TITLE_TILESET_SIZE);
    ClearVram();

	DrawMap2(4,8,map_title);
	
	Print(8,12,strStart);
	Print(10,23,strCopy);
	Print(3,25,strWeb);


	if(playDevice==0)Print(6,17,strNotDetected);

	StartSong(song_Whacksong);

	while(1){
		joy=ReadJoypad(playPort);
		if(joy&actionButton){
			while(ReadJoypad(playPort)&actionButton);
			StopSong();
			return;
		}
	}
}

void InitControllers() {
    // Wait a few frames for the mouse hardware to settle/handshake
    WaitVsync(10);

    unsigned char controllers = DetectControllers();

    // Check Port 1 (bits 0 & 1)
    if ((controllers & 3) == MOUSE) {
        playDevice = 1; // Mouse mode
        playPort = 0;
        actionButton = BTN_MOUSE_LEFT;
    }
    // Check Port 2 (bits 2 & 3)
    else if (((controllers >> 2) & 3) == MOUSE) {
        playDevice = 1;
        playPort = 1;
        actionButton = BTN_MOUSE_LEFT;
    }
    // Default to Gamepad Port 1
    else {
        playDevice = 0; // Gamepad mode
        playPort = 0;
        actionButton = BTN_A;
    }
}

unsigned char cnt=0;
void processMoles(){
	unsigned char x,y,rdx,rdy;
	const char *ptr=NULL;

	//randomize moles
	if(activeMoles<level && !gameOver){

		rdx=random()%256;
		if(rdx<(16+(level*2))){

			rdx=rand()%4;		
			rdy=rand()%4;
			cnt++;
			if(moles[rdy][rdx].active==false){

				moles[rdy][rdx].active=true;
				moles[rdy][rdx].whacked=false;
				moles[rdy][rdx].animFrame=0;
				activeMoles++;

			}
		}

	}

	//animate moles
	for(y=0;y<4;y++){
		for(x=0;x<4;x++){
			if(moles[y][x].active){
				ptr=NULL;

				if(moles[y][x].whacked){

					switch(moles[y][x].animFrame){
						case 0:
						case 8:
						case 16:
							ptr=map_mole6;
							break;
				
						case 4:
						case 12:
						case 20:
							ptr=map_mole7;
							break;				
																										
						case 24:
							ptr=map_hole;
							break;

						case 28:
							moles[y][x].active=false;
							activeMoles--;
							break;
					}

				}else{
					switch(moles[y][x].animFrame){
						case 0:
						case 52+10:
							ptr=map_mole1;
							break;
				
						case 6:
						case 46+10:
							ptr=map_mole2;
							break;				
				
				
						case 12:
						case 40+10:
							ptr=map_mole3;
							break;
						
						case 18:
						case 36+10:
							ptr=map_mole4;
							break;
						
						case 24:
							ptr=map_mole5;
							break;
																						
						case 58+10:
							ptr=map_hole;
							break;

						case 64+10:
							moles[y][x].active=false;
							activeMoles--;
							break;
					}
				}

				if(ptr!=NULL){				
					DrawMap2((x*6)+4,(y*4)+FIELD_TOP+3,ptr);
				}

				moles[y][x].animFrame++;
				
			}

	
			
		}		
	}

	
}

void WaitKey(){
	while((ReadJoypad(playPort)&BTN_MOUSE_LEFT)==0);
	while((ReadJoypad(playPort)&BTN_MOUSE_LEFT)!=0);
}


void doStart(){
	unsigned char btnId;
	unsigned int frame=0;


	nextFreeButtonIndex=0;
	btnId=createMyButton(13,FIELD_TOP+3,map_startBtnUp,map_startBtnDown);

	while(1){
   		WaitVsync(1);
		processMouseMovement();
		processMyButtons();

		//check if we have pressed the start button
		if(buttons[btnId].clicked){
			DrawMap2(10,FIELD_TOP+3,map_hole);
			DrawMap2(16,FIELD_TOP+3,map_hole);
			MapSprite(0,map_hammerUp);

			DrawMap2(12,FIELD_TOP+7+4,map_ready);



			while(frame<120){
				WaitVsync(1);
				processMouseMovement();
				processHammer();

				if(frame==40 || frame==110){
					DrawMap2(10,FIELD_TOP+3+4+4,map_hole);
					DrawMap2(16,FIELD_TOP+3+4+4,map_hole);
				}else if(frame==50){
					DrawMap2(12,FIELD_TOP+7+4,map_whack);
				}

				frame++;
			}
			return;
		}


	}
}

void processMyButtons(){
	unsigned char i,tx,ty;
	unsigned int joy=ReadJoypad(playPort);
	static unsigned int lastButtons;

	tx=(mx+6)>>3;
	ty=my>>3;





	for(i=0;i<nextFreeButtonIndex;i++){
		if(tx>=buttons[i].x && tx<(buttons[i].x+buttons[i].width) && ty>=buttons[i].y && ty<(buttons[i].y+buttons[i].height)){
			if(joy&actionButton && buttons[i].state==BUTTON_UNPRESSED){
				DrawMap2(buttons[i].x,buttons[i].y,buttons[i].pushedMapPtr);
				buttons[i].state=BUTTON_PRESSED;
			}
			
			if((joy&actionButton)==0 && buttons[i].state==BUTTON_PRESSED){
				//button clicked!
				buttons[i].state=BUTTON_UNPRESSED;
				buttons[i].clicked=true;
				DrawMap2(buttons[i].x,buttons[i].y,buttons[i].normalMapPtr);
			}			
		}else{
		
			if((joy&actionButton)==0 && buttons[i].state==BUTTON_PRESSED){
				//button clicked!
				buttons[i].state=BUTTON_UNPRESSED;
				buttons[i].clicked=false;
				DrawMap2(buttons[i].x,buttons[i].y,buttons[i].normalMapPtr);
			}

		}


	}

	if((joy&BTN_MOUSE_RIGHT) && ((lastButtons&BTN_MOUSE_RIGHT)==0)){
		TriggerFx(8,0xff,true);
		mouseSpeed++;
		if(mouseSpeed==3)mouseSpeed=0;
		SetMouseSensitivity(mouseSpeed);
		PrintSpeed();
	}

	lastButtons=joy;
}

char createMyButton(unsigned char x,unsigned char y,const char *normalMapPtr,const char *pushedMapPtr){
	unsigned char id;

	if(nextFreeButtonIndex<BUTTONS_COUNT){
		buttons[nextFreeButtonIndex].x=x;
		buttons[nextFreeButtonIndex].y=y;
		buttons[nextFreeButtonIndex].normalMapPtr=normalMapPtr;
		buttons[nextFreeButtonIndex].pushedMapPtr=pushedMapPtr;
		buttons[nextFreeButtonIndex].state=0;
		buttons[nextFreeButtonIndex].width=pgm_read_byte(&(normalMapPtr[0]));
		buttons[nextFreeButtonIndex].height=pgm_read_byte(&(normalMapPtr[1]));
		buttons[nextFreeButtonIndex].clicked=false;
		DrawMap2(x,y,normalMapPtr);
		id=nextFreeButtonIndex;
		nextFreeButtonIndex++;
	}else{
		id=-1;
	}

	return id;
}


void processMouseMovement(void){
	unsigned int joy;
	


	//check in case its a SNES pad

	if(playDevice==0){
		joy=ReadJoypad(playPort);

		if(joy&BTN_LEFT){
			mx-=2;
			if(mx<5) mx=5; 
		}
		if(joy&BTN_RIGHT){
			mx+=2;
			if(mx>227) mx=227;
		}
		if(joy&BTN_UP){
			my-=2;
			if(my<0)my=0;
		}
		if(joy&BTN_DOWN){
			my+=2;
			if(my>218)my=218;
		}

	}else{
	
		joy=ReadJoypadExt(playPort);

		if(joy&0x80){
			mx-=(joy&0x7f);
			if(mx<5) mx=5; 
		}else{
			mx+=(joy&0x7f);
			if(mx>227) mx=227;
		}
	
		if(joy&0x8000){
			my-=((joy>>8)&0x7f);
			if(my<0)my=0;
		}else{
			my+=((joy>>8)&0x7f);
			if(my>218)my=218;
		}
	
	}

	MoveSprite(0,mx,my,3,3);
}

void processHammer(void){
	static unsigned char hammerState=0,hammerFrame=0;
	static bool mouseBtnReleased=true;
	unsigned int joy;
	unsigned char x,y,dx,dy;

	
	joy=ReadJoypad(playPort);

	if((joy&actionButton) && hammerState==0 && mouseBtnReleased){

		MapSprite(0,map_hammerDown);
		hammerState=1;
		hammerFrame=0;
		dy=5;
		dx=-5;
		mouseBtnReleased=false;
		srand(TCNT1);
		TriggerFx(6,0xa0,true);

		if(!gameOver){
			//check if a mole was whacked!

			dx=((mx+6)>>3);
			dy=((my+8)>>3);


			for(y=0;y<4;y++){
				for(x=0;x<4;x++){
					if(moles[y][x].active && !moles[y][x].whacked && moles[y][x].animFrame>=12 && moles[y][x].animFrame<=60){
						if(dx>=moles[y][x].x && dx<(moles[y][x].x+5) &&
						   dy>=moles[y][x].y && dy<(moles[y][x].y+4)){
						
							//PrintHexByte(10,4,moles[y][x].active);
							//PrintHexByte(13,4,moles[y][x].animFrame);
						
							moles[y][x].whacked=true;
							moles[y][x].animFrame=0;
							molesWhacked++;
							TriggerFx(7,0xff,true);

						}
					}
				}	
			}
				
		}

	}

	if(hammerState==1 && mouseBtnReleased){
		MapSprite(0,map_hammerUp);
		dy=0;
		dx=0;
		hammerState=0;
	}
	
	if((joy&actionButton)==0)mouseBtnReleased=true;
	


	hammerFrame++;
}

