/*
 *  Uzebox quick and dirty tutorial
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
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

//the next two includes must be kept in the same order
#include "data/tiles.inc"
#include "data/font-6x8-full.inc"

#define DEAD 0
#define AUTO_REPEAT_DELAY 10
#define INTER_REPEAT_DELAY 2

bool inMainLoop=false;
u16 gen=0;

u8 Title();
void DisplayHowToPlay();
void ComputeNextStep();
void AnimateCursor(u8 x,u8 y);
void UpdateStatusBar();

void lwss(u8 x,u8 y){
	PutPixel(x+1,y,1,0);
	PutPixel(x+4,y,1,0);
	PutPixel(x,y+1,1,0);
	PutPixel(x,y+2,1,0);
	PutPixel(x+4,y+2,1,0);
	PutPixel(x,y+3,1,0);
	PutPixel(x+1,y+3,1,0);
	PutPixel(x+2,y+3,1,0);
	PutPixel(x+3,y+3,1,0);
}

void rPentamino(u8 x,u8 y){

	PutPixel(x+1,y,1,0);
	PutPixel(x+2,y,1,0);
	PutPixel(x  ,y+1,1,0);
	PutPixel(x+1,y+1,1,0);
	PutPixel(x+1,y+2,1,0);
}

void glider(u8 x,u8 y){
	PutPixel(x+1,y,1,0);
	PutPixel(x+2,y+1,1,0);
	PutPixel(x  ,y+2,1,0);
	PutPixel(x+1,y+2,1,0);
	PutPixel(x+2,y+2,1,0);
}

void gliderGun(u8 x,u8 y){
	//Gosper glider gun
	PutPixel(x,y,1,0);
	PutPixel(x+1,y,1,0);
	PutPixel(x,y+1,1,0);
	PutPixel(x+1,y+1,1,0);

	PutPixel(x+10,y,1,0);
	PutPixel(x+11,y-1,1,0);
	PutPixel(x+12,y-2,1,0);
	PutPixel(x+13,y-2,1,0);
	PutPixel(x+10,y+1,1,0);
	PutPixel(x+10,y+2,1,0);
	PutPixel(x+11,y+3,1,0);
	PutPixel(x+12,y+4,1,0);
	PutPixel(x+13,y+4,1,0);

	PutPixel(x+14,y+1,1,0);
	PutPixel(x+15,y-1,1,0);
	PutPixel(x+15,y+3,1,0);
	PutPixel(x+16,y,1,0);
	PutPixel(x+16,y+1,1,0);
	PutPixel(x+16,y+2,1,0);
	PutPixel(x+17,y+1,1,0);

	PutPixel(x+20,y,1,0);
	PutPixel(x+20,y-1,1,0);
	PutPixel(x+20,y-2,1,0);
	PutPixel(x+21,y,1,0);
	PutPixel(x+21,y-1,1,0);
	PutPixel(x+21,y-2,1,0);
	PutPixel(x+22,y-3,1,0);
	PutPixel(x+22,y+1,1,0);
	PutPixel(x+24,y-3,1,0);
	PutPixel(x+24,y-4,1,0);
	PutPixel(x+24,y+1,1,0);
	PutPixel(x+24,y+2,1,0);

	PutPixel(x+34,y-2,1,0);
	PutPixel(x+34,y-1,1,0);
	PutPixel(x+35,y-2,1,0);
	PutPixel(x+35,y-1,1,0);

}


#define ACTION_NONE 0
#define ACTION_STARTSTOP 1
#define ACTION_OPENMENU 2

u8 keyAction=0;

bool paused=false;

void CreateStatusBar(){
	SetTile(14,0,95);
	SetTile(15,0,96);
	SetTile(16,0,97);
	SetTile(17,0,98);
	SetTile(18,0,62);
	Print(19,0,PSTR("Start/Stop"));

	SetTile(30,0,95);
	SetTile(31,0,99);
	SetTile(32,0,100);
	SetTile(33,0,98);
	SetTile(34,0,62);
	Print(35,0,PSTR("Menu"));

	Print(4,0,PSTR("Gen:"));

	UpdateStatusBar();
}

void UpdateStatusBar(){
	if(paused){
		SetTile(1,0,93);
		SetTile(2,0,94);
	}else{
		SetTile(1,0,91);
		SetTile(2,0,92);
	}

	PrintInt(12,0,gen,true);
}

u8 OpenMenu(){

	ClearTextVram();
	
	SetTileTable(tiles);
	SetFontTilesIndex(TILES_SIZE);

	//Show object types
	Print(6,2,PSTR("Select life object to insert"));
	Print(5,3,PSTR("at the current cursor position"));

	Print(7,7,PSTR("Glider"));
	Print(7,8,PSTR("Lightweight spaceship (LWSS)"))	;
	Print(7,9,PSTR("R-Pentamino"));
	Print(7,10,PSTR("Gosper glider gun"));

	Print(7,11,PSTR("Clear field"));
	Print(7,12,PSTR("Exit game"));


	//slide in menu
	WaitVsync(1);
	Fill(0,0,40,1,0);
	for(u8 i=1;i<29;i++){
		WaitVsync(1);
		vmode_text_lines=i;
	}


	s8 option=0,x=5,y=7;
	u16 joy,prevJoy=0;
	while(1){
		WaitVsync(1);
		joy=ReadJoypad(0);
		if(prevJoy==0){
			if (joy==BTN_UP){
				if(option>0) option--;

			}else if (joy==BTN_DOWN){
				if(option<5)option++;

			}else if(joy==BTN_SELECT){
				break;
			}else if(joy==BTN_START){
				switch(option){
					case 0:
						glider(cursor_x,cursor_y);
						break;

					case 1:
						lwss(cursor_x,cursor_y);
						break;
	
					case 2:
						rPentamino(cursor_x,cursor_y);
						break;					

					case 3:
						gliderGun(cursor_x,cursor_y);
						break;	

					case 4:
						ClearVram();
						break;

					default:						
						while(ReadJoypad(0)!=0);
						return 1;  //signal game end, back to main menu
				}
	
				break;
			}
		}
		prevJoy=joy;
		AnimateCursor(x,y+option);
	}

	while(ReadJoypad(0)!=0);

	//close menu
	for(u8 i=29;i>0;i--){
		WaitVsync(1);
		vmode_text_lines=i;
	}
	ClearTextVram();
	SetTileTable(font);
	SetFontTilesIndex(0);
	CreateStatusBar();
	
	return 0;
}

void VsyncCallBack(void){
	static u16 lastButtons;
	static u16 autoRepeatDelay=0,interRepeatDelay=0;
	u16 joy;
    //process cursor asynchonously of cells processing
	if(inMainLoop && keyAction==ACTION_NONE){
		joy=ReadJoypad(0);
		bool autoKey=(autoRepeatDelay>AUTO_REPEAT_DELAY && interRepeatDelay>INTER_REPEAT_DELAY);

		if((joy&BTN_START) && !(lastButtons&BTN_START)){
			//paused=!paused;
			keyAction=ACTION_STARTSTOP;
		}

		if((joy&BTN_SELECT) && !(lastButtons&BTN_SELECT)){
			keyAction=ACTION_OPENMENU;
		}

		if(joy&BTN_RIGHT){
			if(!(lastButtons&BTN_RIGHT) || autoKey ){
				cursor_x++;
				interRepeatDelay=0;
			}
		}else if(joy&BTN_LEFT){
			if(!(lastButtons&BTN_LEFT) || autoKey){
				cursor_x--;
				interRepeatDelay=0;
			}
		}

		if(joy&BTN_UP){
			if(!(lastButtons&BTN_UP) || autoKey){
				cursor_y--;
				interRepeatDelay=0;
			}
		}else if(joy&BTN_DOWN){
			if(!(lastButtons&BTN_DOWN) || autoKey){
				cursor_y++;
				interRepeatDelay=0;
			}
		}

		if(joy&(BTN_LEFT|BTN_RIGHT|BTN_UP|BTN_DOWN)){
			autoRepeatDelay++;
			interRepeatDelay++;
		}else{
			autoRepeatDelay=0;
			interRepeatDelay=0;
		}

		if((joy&BTN_A) && !(lastButtons&BTN_A)){
			PutPixel(cursor_x,cursor_y,1,vmode_page); 	//set pixel
		}
		if((joy&BTN_B) && !(lastButtons&BTN_B)){
			PutPixel(cursor_x,cursor_y,0,vmode_page); 	//clear pixel
		}


		lastButtons=joy;
	}
}



int main(){


	SetUserPostVsyncCallback(&VsyncCallBack);



	while(1){
		vmode=0;
		vmode_text_lines=SCREEN_TILES_V;
		paused=true;
		u8 option=0;


		do{
			option=Title();
			if(option==1){
				DisplayHowToPlay();
			}
		}while(option!=0);

		ClearVram();

		SetTileTable(font);
		SetFontTilesIndex(0);
		CreateStatusBar();


		//SetTile(0,0,91);
		//SetTile(1,0,92);
		//Print(1,0,PSTR("Started  Generation:       SELECT=Menu"));


		vmode_text_lines=1;
		//vmode=1;
		vmode_grid_col=9;//82;
		vmode_page=0;

		cursor_x=44;
		cursor_y=25;

		//R-pentamino
		//PutPixel(40,30,1,0);
		//PutPixel(41,30,1,0);
		//PutPixel(39,31,1,0);
		//PutPixel(40,31,1,0);
		//PutPixel(40,32,1,0);



		//u16 joy;

		inMainLoop=true;
		bool gameEnd=false;
		while(1){

			WaitVsync(1);

			if(!paused){
				ComputeNextStep();
				vmode_page^=1;
				gen++;

			}
		
			if(keyAction!=ACTION_NONE){
				switch(keyAction){
					case ACTION_STARTSTOP:
						paused=!paused;
						keyAction=ACTION_NONE;
						break;
					case ACTION_OPENMENU:
						if(OpenMenu()==1){
							gameEnd=true;
							inMainLoop=false;
						}
						keyAction=ACTION_NONE;
						break;
				}

			}

			UpdateStatusBar();
			if(gameEnd)break;
		}
	}

} 

//Rules:
//1.Any live cell with fewer than two live neighbours dies, as if caused by under-population.
//2.Any live cell with two or three live neighbours lives on to the next generation.
//3.Any live cell with more than three live neighbours dies, as if by overcrowding.
//4.Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
void ComputeNextStep(){
	u8 x,y,cell,neighbours,nextGenByte,mask,pmask,nmask;
	u8 prevLinePrevColByte=0,currLinePrevColByte=0,nextLinePrevColByte=0;
	u8 prevLineCurrColByte,currLineCurrColByte,nextLineCurrColByte;

	u8* prevLinePtr=&vram[vmode_page][0*(SCREEN_WIDTH/8)];
	u8* currLinePtr=&vram[vmode_page][1*(SCREEN_WIDTH/8)];
	u8* nextLinePtr=&vram[vmode_page][2*(SCREEN_WIDTH/8)];

	u8* nextPageLinePtr=&vram[vmode_page^1][1*(SCREEN_WIDTH/8)];

	prevLineCurrColByte=*prevLinePtr++;
	currLineCurrColByte=*currLinePtr++;
	nextLineCurrColByte=*nextLinePtr++;

	for(y=0;y<(SCREEN_HEIGHT-2);y++){
		for(x=0;x<(SCREEN_WIDTH/8);x++){
			nextGenByte=0;
			mask=0x80;
			for(u8 i=0;i<8;i++){
				neighbours=0;
				cell=currLineCurrColByte&mask;

				//count live neighbours 
				if(i==0){
					if((prevLinePrevColByte&1)!=0)neighbours++;
					if((currLinePrevColByte&1)!=0)neighbours++;
					if((nextLinePrevColByte&1)!=0)neighbours++;
					if((prevLineCurrColByte&128)!=0)neighbours++;
					if((nextLineCurrColByte&128)!=0)neighbours++;
					if((prevLineCurrColByte&64)!=0)neighbours++;
					if((currLineCurrColByte&64)!=0)neighbours++;
					if((nextLineCurrColByte&64)!=0)neighbours++;
				}else if(i==7){

					if((prevLineCurrColByte&2)!=0)neighbours++;
					if((currLineCurrColByte&2)!=0)neighbours++;
					if((nextLineCurrColByte&2)!=0)neighbours++;
					if((prevLineCurrColByte&1)!=0)neighbours++;
					if((nextLineCurrColByte&1)!=0)neighbours++;

					prevLinePrevColByte=prevLineCurrColByte;
					currLinePrevColByte=currLineCurrColByte;
					nextLinePrevColByte=nextLineCurrColByte;
					prevLineCurrColByte=*prevLinePtr++;
					currLineCurrColByte=*currLinePtr++;
					nextLineCurrColByte=*nextLinePtr++;

					if((prevLineCurrColByte&0x80)!=0)neighbours++;
					if((currLineCurrColByte&0x80)!=0)neighbours++;
					if((nextLineCurrColByte&0x80)!=0)neighbours++;

				}else{
					pmask=(u8)(mask<<1);
					nmask=(u8)(mask>>1);
					if((prevLineCurrColByte&pmask)!=0)neighbours++;
					if((currLineCurrColByte&pmask)!=0)neighbours++;
					if((nextLineCurrColByte&pmask)!=0)neighbours++;
					if((prevLineCurrColByte&mask)!=0)neighbours++;
					if((nextLineCurrColByte&mask)!=0)neighbours++;
					if((prevLineCurrColByte&nmask)!=0)neighbours++;
					if((currLineCurrColByte&nmask)!=0)neighbours++;
					if((nextLineCurrColByte&nmask)!=0)neighbours++;
				}		

				if(cell==DEAD){
					if(neighbours==3){
						//rule 4
						nextGenByte|=mask; //cell born
					}
				}else{
					
					if(neighbours==2 || neighbours==3){
						//rule 2
						nextGenByte|=mask; //stays alive
					}

					//if(neighbours<2 || neighbours>3)
					//rule 1 & 3
					//copy nothing, cell dies

				}
				mask>>=1;

			}
			*nextPageLinePtr++=nextGenByte;

		}
	}

}


void AnimateCursor(u8 x,u8 y){

	static u8 animFrame=0,animDelay=0,prevY=0;

	if(animDelay==15 || y!=prevY){
		//clear othercursor  location
		SetTile(x,prevY,0);
		SetTile(x+1,prevY,0);

		animDelay=0;
		animFrame++;
		if(animFrame==4)animFrame=0;
		switch(animFrame){
			case 0:
				SetTile(x,y,1);
				SetTile(x+1,y,2);
				break;
			case 1:
				SetTile(x,y,3);
				SetTile(x+1,y,4);
				break;
			case 2:
				SetTile(x,y,5);
				SetTile(x+1,y,6);
				break;
			default:
				SetTile(x,y,7);
				SetTile(x+1,y,8);
				break;
		}
		prevY=y;
	}
	animDelay++;


}


u8 Title(){
	ClearTextVram();

	SetTileTable(tiles);
	SetFontTilesIndex(TILES_SIZE);
	DrawMap2(8,3,map_title1);
	DrawMap2(4,6,map_title2);
	DrawMap2(24,13,map_title3);

	Print(8,15,PSTR("PLAY"));
	Print(8,16,PSTR("HOW TO PLAY"));
//	Print(8,17,PSTR("OPTIONS"));

	Print(13,26,PSTR("V1.1 \\ 2013 Uze"));

	u8 option=0;
	u8 animFrame=0,animDelay=0;
	u8 x=6,y=15;
	u16 joy,prevJoy=0;
	while(1){
		WaitVsync(1);
		joy=ReadJoypad(0);
		AnimateCursor(x,y+option);
		/*
		if(animDelay==15){
			//clear othercursor  location
			SetTile(x,y+(option^1),0);
			SetTile(x+1,y+(option^1),0);

			animDelay=0;
			animFrame++;
			if(animFrame==4)animFrame=0;
			switch(animFrame){
				case 0:
					SetTile(x,y+option,1);
					SetTile(x+1,y+option,2);
					break;
				case 1:
					SetTile(x,y+option,3);
					SetTile(x+1,y+option,4);
					break;
				case 2:
					SetTile(x,y+option,5);
					SetTile(x+1,y+option,6);
					break;
				default:
					SetTile(x,y+option,7);
					SetTile(x+1,y+option,8);
					break;
			}
		}
		animDelay++;
		*/

		if(prevJoy==0){
			if (joy==BTN_SELECT || joy==BTN_UP || joy==BTN_DOWN){
				option^=1;
				animDelay=15;
			}else if(joy==BTN_START){
				while(ReadJoypad(0)!=0);
				return option;
			}
		}
		prevJoy=joy;
	}

}

void DisplayHowToPlay(){
	SetTileTable(tiles);
	SetFontTilesIndex(TILES_SIZE);

	ClearTextVram();
	DrawMap2(4,1,map_title2);

	Print(1,5,PSTR("The Game of Life is a cellular        "));
	Print(1,6,PSTR("automaton devised by the mathematician"));
	Print(1,7,PSTR("John Conway in 1970. One interacts    "));
	Print(1,8,PSTR("with the game by creating an initial  "));
	Print(1,9,PSTR("configuration and observing how it    "));
	Print(1,10,PSTR("evolves."));

	Print(1,12,PSTR("The game is made of a grid of cells  "));
	Print(1,13,PSTR("each of which has two possible states"));
	Print(1,14,PSTR("alive or dead. Each turn, cells"));
	Print(1,15,PSTR("evolves according to four rules:"));

	Print(1,17,PSTR("1.Any live cell with fewer than two "));
	Print(1,18,PSTR("  live neighbours dies"));

	Print(1,19,PSTR("2.Any live cell with two or three live"));
	Print(1,20,PSTR("  neighbours lives on to the next turn"));

	Print(1,21,PSTR("3.Any live cell with more than three"));
	Print(1,22,PSTR("  live neighbours dies"));

	Print(1,23,PSTR("4.Any dead cell with exactly three"));
	Print(1,24,PSTR("  live neighbours becomes a live cell"));

	Print(15,26,PSTR("PRESS START"));

	while(1){
		if(ReadJoypad(0)==BTN_START){
			while(ReadJoypad(0)!=0);
			return;
		}
	}
}

