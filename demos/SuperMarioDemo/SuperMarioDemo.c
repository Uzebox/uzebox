/*
 *  Uzebox(tm) Super Mario Demo
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

/*

About this program:
-------------------

This program demonstrates the latest sprites engine with video mode 3. This mode uses
'ramtiles' to display sprites. Search the forums for more info on theses.

Full screen scrolling, overlay and sprites horizontal flipping are supported. use the Screen object to set scrolling and control 
the visibility & height of the overlay.

*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>


//external data
#include "data/smb.map.inc"
#include "data/smb.pic.inc"
#include "data/fonts_8x8.pic.inc"

#include "data/mario_sprites.map.inc"
#include "data/mario_sprites.pic.inc"

#include "data/nsmb.inc"
#include "data/patches.inc"

unsigned char sx=0,sy=0, anim=0,frame=0,action=0,stopFrame,sprDir,jmpPos,g,i, active=1,mode=0;
char dx=1,dy=0;
bool stopping=false;
unsigned int walkFrame,lastWalkDir;
double jumpVal;

#define ACTION_IDLE 0
#define ACTION_WALK 1
#define ACTION_STOPPING 2
#define ACTION_JUMP 3

unsigned char processControls(void);

unsigned char mario[2][6];
void PerformActions();

extern const char waves[];
extern void SetColorBurstOffset(unsigned char value);

unsigned char goombaX[2];
char goombaDir[2];
unsigned char goombaAnim[4];
unsigned char goombaSpr[2];
unsigned char goombaSprIndex[2];
unsigned char delay=0;



int main(){	



	ClearVram();
	InitMusicPlayer(patches);
	SetMasterVolume(0x40);
	StartSong(song_nsmb);

	SetSpritesTileTable(mario_sprites_tileset);
	SetFontTilesIndex(SMB_TILESET_SIZE);
	//SetColorBurstOffset(4);
	SetTileTable(smb_tileset);
	

	//DrawMap2(0,0,map_main);
	DrawMap2(0,VRAM_TILES_V,map_hud);
	
	unsigned char c;
	for(int y=0;y<23;y++){
		for(int x=0;x<30;x++){
			c=pgm_read_byte(&(map_main[(y*MAP_MAIN_WIDTH)+x+2]));
			SetTile(x,y+1,c);
		}	
	}


	dx=0;
	sx=50;
	sy=169-32+8;
	sprDir=1;

	goombaX[0]=17; //159;
	goombaDir[0]=-1;
	goombaAnim[0]=0;
	goombaSpr[0]=0;
	goombaSprIndex[0]=6;

	goombaX[1]=65 ;//201;
	goombaDir[1]=1;
	goombaAnim[1]=0;
	goombaSpr[1]=0;
	goombaSprIndex[1]=10;


	MapSprite2(0,map_rwalk1,0);
	MapSprite2(6,map_rgoomba1,SPRITE_FLIP_X);
	MapSprite2(10,map_rgoomba2,0);



	g=0;
	MoveSprite(0,sx,sy,2,3);
	Scroll(0,-1);

	MoveSprite(goombaSprIndex[0],goombaX[0],176,2,2);
	MoveSprite(goombaSprIndex[1],goombaX[1],176,2,2);

	Screen.scrollY=0;
	Screen.overlayHeight=OVERLAY_LINES;

	
	while(1){
		WaitVsync(1);
	

		processControls();

		if((active&1)!=0){
			PerformActions();
			MoveSprite(0,sx,sy+dy,2,3);
		}else{
			MoveSprite(0,sx,230,2,3);
		}



		//animate goombas
		for(g=0;g<2;g++){
		

				if(goombaX[g]<=0 && goombaDir[g]==-1){
					goombaDir[g]=1;
				}
		
				if(goombaX[g] >= (215+15) && goombaDir[g]==1){
					goombaDir[g]=-1;
			
				}
		
				goombaX[g]+=goombaDir[g];
				goombaAnim[g]++;

				if(goombaAnim[g]==8){
					goombaSpr[g]^=1;
					goombaAnim[g]=0;
				}

				if(goombaSpr[g]==0){
					MapSprite2(goombaSprIndex[g],map_rgoomba1,goombaDir[g]!=1?SPRITE_FLIP_X:0);
				}else{
					MapSprite2(goombaSprIndex[g],map_rgoomba2,goombaDir[g]!=1?SPRITE_FLIP_X:0);
				}

				MoveSprite(goombaSprIndex[g],goombaX[g],176-32+8,2,2);
			

		}
	

	}		
	
}

void loadNextStripe(){
	static unsigned int srcX=30;
	static unsigned char destX=30;

	for(int y=0;y<23;y++){
		SetTile(destX,y+1,pgm_read_byte(&(map_main[(y*MAP_MAIN_WIDTH)+srcX+2])));		
	}

	srcX++;
	if(srcX>=MAP_MAIN_WIDTH)srcX=0;

	destX++;
	if(destX>=32)destX=0;
}

void PerformActions(){
	char sdx,sprFlags=(sprDir!=1?SPRITE_FLIP_X:0);

	if(stopping==true && walkFrame<=5){
		MapSprite2(0,map_rwalk1,sprFlags);
		dy=0;
		stopFrame++;
		return;
	}

	sdx=dx;
	if(dx==1 && sx>=110) sdx=0;
	//if(dx==-1 && sx<=10) sdx=0;
	//if(dx==1 && sx>=220) sdx=0;

	switch(action){
		case ACTION_WALK:
			
			if(frame==0){
				MapSprite2(0,map_rwalk2,sprFlags);
				dy=-1;				
			}else if(frame>0 && frame<=5){
				sx+=sdx;
			}else if(frame==6){
				sx+=(sdx*2);
			}else if(frame==7){
				MapSprite2(0,map_rwalk1,sprFlags);
				dy=0;
				sx+=(sdx*2);

			if(stopping){
				stopFrame++;
			}

			}else if(frame>7 && frame<=11){
				sx+=sdx;
			}else if(frame==12){
				MapSprite2(0,map_rwalk2,sprFlags);
				dy=-1;	
				sx+=(sdx*2);
			}else if(frame>12 && frame<=15){
				sx+=sdx;
			}else if(frame==16){
				sx+=sdx;
				frame=7;
				break;	
			}else if(frame==17){
				dy=0;
				MapSprite2(0,map_lskid,sprFlags);
				sx+=sdx;
			}else if(frame==18){
				sx+=sdx;
			}else if(frame==19){
				frame=7;
				sx+=sdx;
			}



			frame++;
			walkFrame++;

			break;

		case ACTION_JUMP:
			
		
			dy=-( pgm_read_byte(&(waves[jmpPos])) /2  );


			if(frame==0){
				MapSprite2(0,map_rjump1,sprFlags);

				sx+=sdx;



			}else if(frame>0 && frame<=20){
			
				sx+=sdx;



			}else if(frame==21){
				MapSprite2(0,map_rjump2,sprFlags);
				sx+=sdx;

			}else if(frame>21 && frame<=42){
				
				sx+=sdx;
			}else if(frame==43){
			
				sx+=sdx;
				dy=0;
				MapSprite2(0,map_rwalk1,sprFlags);
				action=ACTION_IDLE;
				
			}
			
			jmpPos+=3;
			frame++;

			break;
	};
}

unsigned char processControls(void){
	static unsigned char ov=4,scroll=0;
	static int lastbuttons=0;
	unsigned int joy=ReadJoypad(0);


	if(joy&BTN_A){
		if(action!=ACTION_JUMP){
			action=ACTION_JUMP;
			frame=0;
			jmpPos=0;
		}
	
	}else if(joy&BTN_X){
		Scroll(0,1);	
	//	while(ReadJoypad(0)!=0);
	}else if(joy&BTN_Y){
		Scroll(0,-1);
	//	while(ReadJoypad(0)!=0);
	}else if(joy&BTN_SR){
	//	Scroll(1,0);
	//	while(ReadJoypad(0)!=0);
	}else if(joy&BTN_SL){
	//	Scroll(-1,0);
	//	while(ReadJoypad(0)!=0);
	}else if(joy&BTN_UP){
		//if(sy==0)sy=(VRAM_TILES_V*8);
		//sy--;
		//while(ReadJoypad(0)!=0);

	}else if(joy&BTN_DOWN){
		//sy++;
		//if(sy>=(VRAM_TILES_V*8))sy=0;
	//	while(ReadJoypad(0)!=0);
	
	}
	
	if(joy&BTN_LEFT){
		
		//while(ReadJoypad(0)!=0);
		
		if(action==ACTION_IDLE){
			action=ACTION_WALK;
			dx=-1;
			frame=0;
			stopping=false;
			walkFrame=0;
			lastWalkDir=BTN_LEFT;
			sprDir=-1;
		}
		
		
	}else if(joy&BTN_RIGHT){

		//while(ReadJoypad(0)!=0);

		
		if(action==ACTION_IDLE){
			action=ACTION_WALK;
			dx=1;
			frame=0;
			stopping=false;
			walkFrame=0;
			lastWalkDir=BTN_RIGHT;
			sprDir=1;
		}
		
		if(sx>=110){

			if(joy&BTN_B){
				Scroll(2,0);
				scroll+=2;
			}else{
				Scroll(1,0);
				scroll++;
			}
			
			if(scroll>=8){
				loadNextStripe();
				scroll=0;
			}
		}

	
	}

	if(joy&BTN_START){
		ov++;
		if(ov>OVERLAY_LINES)ov=0;

		Screen.overlayHeight=ov;
		while(ReadJoypad(0)!=0);
	}

	if(joy&BTN_SELECT){
		StopSong();
		while((ReadJoypad(0)&BTN_SELECT)!=0);
	}


	


	if(stopping==true && stopFrame==1){
		action=ACTION_IDLE;
		stopping=false;
		dx=0;
	}else if(lastWalkDir!= (joy & (BTN_LEFT+BTN_RIGHT)) && action==ACTION_WALK){
		
		if((joy & (BTN_LEFT+BTN_RIGHT))!=0 ){
			frame=17;
		}else{
			stopping=true;
			stopFrame=0;
		}
	}
	

	lastbuttons=joy;
	lastWalkDir= joy & (BTN_LEFT+BTN_RIGHT);


	return 0;
}

