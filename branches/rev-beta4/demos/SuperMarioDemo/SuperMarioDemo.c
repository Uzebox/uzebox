/*
 *  Uzebox Super Mario Demo
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
 *
 *  Uzebox is a reserved trade mark
*/

/*

About this program:
-------------------

This program demonstrates the latest sprites engine with video mode 3. This mode uses
'ramtiles' to display sprites. Search the forums for more info on theses.

There is no scrolling yet in this mode.


Uze


*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <avr/interrupt.h>

//external data
#include "data/smb.map.inc"
#include "data/smb.pic.inc"
#include "data/fonts_8x8.pic.inc"

#include "data/mario_sprites.map.inc"
#include "data/mario_sprites.pic.inc"


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

unsigned char goombaX[4];
char goombaDir[4];
unsigned char goombaAnim[4];
unsigned char goombaSpr[4];
unsigned char goombaSprIndex[4];


int main(){	
	
	
	SetTileTable(smb_tileset);
	SetSpritesTileTable(mario_sprites_tileset);
	//ClearVram();
	//SetColorBurstOffset(4);

	DrawMap2(0,0,map_main);
	DrawMap2(13,15,map_opt0);



	dx=0;
	sx=100;
	sy=169;
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

	goombaX[2]=129 ;//51;
	goombaDir[2]=1;
	goombaAnim[2]=0;
	goombaSpr[2]=0;
	goombaSprIndex[2]=14;


	MapSprite(0,map_rwalk1);
	MapSprite(6,map_lgoomba1);
	MapSprite(10,map_rgoomba2);
	MapSprite(14,map_rgoomba2);

	g=0;
	MoveSprite(0,161,sy+dy,2,3);

	MoveSprite(goombaSprIndex[0],goombaX[0],176,2,2);
	MoveSprite(goombaSprIndex[1],goombaX[1],176,2,2);
	MoveSprite(goombaSprIndex[2],goombaX[2],176,2,2);


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
		for(g=0;g<3;g++){
		

				if(goombaX[g]<=15 && goombaDir[g]==-1){
					goombaDir[g]=1;
				}
		
				if(goombaX[g] >= 215 && goombaDir[g]==1){
					goombaDir[g]=-1;
			
				}
		
				goombaX[g]+=goombaDir[g];
				goombaAnim[g]++;

				if(goombaAnim[g]==8){
					goombaSpr[g]^=1;
					goombaAnim[g]=0;
				}

				if(goombaDir[g]==1){
					if(goombaSpr[g]==0){
						MapSprite(goombaSprIndex[g],map_rgoomba1);
					}else{
						MapSprite(goombaSprIndex[g],map_rgoomba2);
					}
	
				}else{
					if(goombaSpr[g]==0){
						MapSprite(goombaSprIndex[g],map_lgoomba1);
					}else{
						MapSprite(goombaSprIndex[g],map_lgoomba2);
					}
				}
				MoveSprite(goombaSprIndex[g],goombaX[g],176,2,2);
			

		}
	

	}		
}

void PerformActions(){
	char sdx;

	if(stopping==true && walkFrame<=5){
		if(sprDir==1){
			MapSprite(0,map_rwalk1);
		}else{
			MapSprite(0,map_lwalk1);
		}
		dy=0;
		stopFrame++;
		return;
	}

	sdx=dx;
	if(dx==-1 && sx<=10) sdx=0;
	if(dx==1 && sx>=220) sdx=0;

	switch(action){
		case ACTION_WALK:
			
			if(frame==0){
				if(sprDir==1){
					MapSprite(0,map_rwalk2);
				}else{
					MapSprite(0,map_lwalk2);
				}
				dy=-1;				
			}else if(frame>0 && frame<=5){
				sx+=sdx;
			}else if(frame==6){
				sx+=(sdx*2);
			}else if(frame==7){
				if(sprDir==1){
					MapSprite(0,map_rwalk1);
				}else{
					MapSprite(0,map_lwalk1);
				}
				dy=0;
				sx+=(sdx*2);

			if(stopping){
				stopFrame++;
			}

			}else if(frame>7 && frame<=11){
				sx+=sdx;
			}else if(frame==12){
				if(sprDir==1){
					MapSprite(0,map_rwalk2);
				}else{
					MapSprite(0,map_lwalk2);
				}
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
				if(sprDir==1){
					MapSprite(0,map_lskid);
				}else{
					MapSprite(0,map_rskid);
				}
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
				if(sprDir==1){
					MapSprite(0,map_rjump1);
				}else{
					MapSprite(0,map_ljump1);
				}
				
				sx+=sdx;



			}else if(frame>0 && frame<=20){
			
				sx+=sdx;



			}else if(frame==21){
				if(sprDir==1){
					MapSprite(0,map_rjump2);
				}else{
					MapSprite(0,map_ljump2);
				}
				sx+=sdx;
			


			}else if(frame>21 && frame<=42){
				
				sx+=sdx;
			}else if(frame==43){
			
				sx+=sdx;
				dy=0;
				//if(dx==0){
				
					if(sprDir==1){			
						MapSprite(0,map_rwalk1);
					}else{
						MapSprite(0,map_lwalk1);								
					}
					action=ACTION_IDLE;
				
			}
			
			jmpPos+=3;
			frame++;

			break;
	};
}

unsigned char processControls(void){
	static int lastbuttons=0;
	unsigned int joy=ReadJoypad(0);


	if(joy&BTN_A){
		if(action!=ACTION_JUMP){
			action=ACTION_JUMP;
			frame=0;
			jmpPos=0;
		}
	
	}else if(joy&BTN_UP){
		//sy--;
	
	}else if(joy&BTN_DOWN){
		//sy++;
	
	}else if(joy&BTN_LEFT){
		
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
		
		if(action==ACTION_IDLE){
			action=ACTION_WALK;
			dx=1;
			frame=0;
			stopping=false;
			walkFrame=0;
			lastWalkDir=BTN_RIGHT;
			sprDir=1;
		}		
		
	}else if(joy&BTN_SELECT){
		mode++;
		if(mode==3) mode=0;

		if(mode==0){
			SetColorBurstOffset(0);
			DrawMap2(13,15,map_opt0);
		}else if(mode==1){
			SetColorBurstOffset(3);
			DrawMap2(13,15,map_opt1);
		}else if(mode==2){
			SetColorBurstOffset(4);
			DrawMap2(13,15,map_opt2);
		}


		while(ReadJoypad(0)!=0);

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

