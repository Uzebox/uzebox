/*
 *  Uzebox(tm) Arkanoid
 *  Copyright (C) 2008-2009  Alec Bourque
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

This program demonstrates the mode 3 sprites engine.


Hope you have fun with it!


Uze


See Also:
---------
kernel/uzebox.h: API functions and variables
kernel/defines.h: Global defines, constants & compilation options


*/
//#define TITLE

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>


//external data
#include "data/map.map.inc"
#include "data/map.pic.inc"

#ifdef TITLE
	#include "data/title.map.inc"
	#include "data/title.pic.inc"
#endif

#include "data/font.pic.inc" //must be in the middle

#include "data/intro.pic.inc"
#include "data/intro.map.inc"

#include "data/sprites.pic.inc"
#include "data/sprites.map.inc"


#include "data/levels.inc"


#include "data/arkanoid1_converted.inc"
#include "data/arkanoid2_converted.inc"
#include "data/arka_patches.inc"



//#include "data/animations.inc"

#define POWERUP_S 0
#define POWERUP_E 1
#define POWERUP_D 2
#define POWERUP_L 3
#define POWERUP_C 4
#define POWERUP_B 5
#define POWERUP_P 6

#define MAX_BALLS 2

#define SPR_BALL 0
#define SPR_VAUS 2
#define SPR_POWERUP 16
#define SPR_LASER 18

#define LEFT_WALL 16
#define RIGHT_WALL 218
#define CEILING 7
#define FIELD_TOP 3
#define FIELD_LEFT 2
#define HARD_BRICKS_ANIM_COUNT 8
#define FX_VOL 0x90


#define VAUS_BODY_TILE 16
#define VAUS_SHADOW_TILE 21

#define DEFAULT_X_SPEED 0x13
#define DEFAULT_Y_SPEED 0x15

void AnimateVaus();
unsigned char processControls(void);
void AnimateNextBall(void);
void processBalls(void);
void DrawLevel(unsigned char levelNo);
void checkCollision(unsigned char ball, int xOffset,int yOffset);
void triggerBrickAnimation(unsigned char x,unsigned char y,unsigned char type);
void processHardBricks();
void clearBrickAnimation(unsigned char x,unsigned char y);
void processPowerUps();
void moveGrowingVaus();
void processLasers();
bool checkBricks(unsigned char projectileType);
void processWarp();
void doWarp();
void checkSwapBalls();
void SetFontinternal(unsigned char x,unsigned char y,unsigned char c);
void animThrusters(unsigned char count);
void PrintHexByte2(char x,char y,unsigned char byte);
void title();
void intro();
void TurnOffSprites(unsigned char startSprite,unsigned char endSprite);
void MapSprite(unsigned char startSprite,const char *map);
void MoveSprite(unsigned char startSprite,unsigned char x,unsigned char y,unsigned char width,unsigned char height);
void Print2(unsigned char x,unsigned char y,const char *string);
void doEnd();

extern const char waves[];
extern void SetColorBurstOffset(unsigned char value);
//extern void RestoreBackground();

const char *brickAnim[] PROGMEM ={map_brick0Anim1,map_brick0Anim2,map_brick0Anim3,map_brick0Anim4,map_brick0Anim5,map_brick0,
								  map_brick1Anim1,map_brick1Anim2,map_brick1Anim3,map_brick1Anim4,map_brick1Anim5,map_brick1};



struct BallStruct
{
	unsigned int x; //fixed point 8:8
	unsigned int y; //fixed point 8:8
	int dx;		 //fixed point 8:8
	int dy;     //fixed point 8:8
	char sdx;	//catch x
	char sdy;	//catch y
};
struct BallStruct balls[MAX_BALLS];


//used to animate grey & gold bricks
struct HardBricksStruct
{
	unsigned char x;
	unsigned char y;
	unsigned char type;
	unsigned char frame;  
};
struct HardBricksStruct hardBricks[HARD_BRICKS_ANIM_COUNT];



unsigned char powerUpX;
unsigned char powerUpY;
unsigned char powerUpType;
unsigned char powerUpFrame;
bool powerUpActive;


//**Vaus variables**
unsigned char vx,vy=200,vausAnimFrame=0,vausAnimType=0,vausWidth=32, vausPowerUp;
bool dead=false,growing=false,shrinking=false;


//playfield variables
unsigned char currentRound,lives=3,activeBalls,catchedBalls,bricksLeft=0,warpFrame=0;
bool playing=false,startWarp=false,warpActive=false;
int XSpeed,YSpeed; //fixed 8:8
const char *bgPtr; //current background tile map

//laser stuff
struct LaserStruct
{
	unsigned int x; //fixed point 8:8
	unsigned int y; //fixed point 8:8
	bool active;
};
struct LaserStruct lasers[2];



unsigned char tx=0,ty=0,brick=0;


unsigned char field[16][13];


const char strCopyright[] PROGMEM ="[ 1986 TAITO CORP JAPAN"; 
const char strRights[] PROGMEM =   "ALL RIGHTS RESERVED";
const char strUze[] PROGMEM =      "UZEBOX CONVERSION BY UZE";
const char strStart[] PROGMEM =      "PRESS START";
const char strEmpty[] PROGMEM =      "           ";

const char strIntro[] PROGMEM = "THE ERA AND TIME OF\nTHIS STORY IS UNKNOWN\rAFTER THE MOTHERSHIP\nQARKANOIDQ WAS DESTROYED\\\nA SPACECRAFT QVAUSQ\nSCRAMBLED AWAY FROM IT]\rBUT ONLY TO BE\nTRAPPED IN SPACE WARPED\nBY SOMEONE]]]]]]]]";

const char strEnd1[] PROGMEM = "CONGRATULATIONS";
const char strEnd2[] PROGMEM = "THAT WAS THE LAST LEVEL";

unsigned char vs=1;


int main(){	
	unsigned char i;
	SetColorBurstOffset(4);
	InitMusicPlayer(patches);
	//SetMasterVolume(0x40);

	SetSpritesTileTable(arkanoidSprites_tileset);	

	while(1){
		
		#ifdef TITLE		
			title();
		#endif

		intro();
		

		SetTileTable(arkanoid_tileset);
		ClearVram();
			
		SetSpriteVisibility(true);
		SetFontTilesIndex(ARKANOID_TILESET_SIZE);


		currentRound=0;

		do{

			DrawLevel(currentRound);
			FadeIn(2,false);

			do{
				WaitVsync(20);

				vx=100;
				dead=false;
				playing=false;
				powerUpActive=false;
				vausPowerUp=-1;
				powerUpType=-1;
				vausWidth=32;			
				warpActive=false;
				startWarp=false;				
				catchedBalls=0;
				activeBalls=1;
				XSpeed=DEFAULT_X_SPEED;
				YSpeed=DEFAULT_Y_SPEED;

				lasers[0].active=false;
				lasers[1].active=false;
			
				//restore the 'warp wall'
				DrawMap2(28,1,map_wall);

				balls[0].y=(vy-5)<<8;
				balls[1].y=0;
				balls[0].sdx=14;
				balls[0].sdy=-5;

				sprites[SPR_BALL].tileIndex=0;
				sprites[SPR_BALL+1].tileIndex=0;

				//clear hard bricks anum struct
				for(i=0;i<HARD_BRICKS_ANIM_COUNT;i++){
					hardBricks[i].x=0;
					hardBricks[i].y=0;
					hardBricks[i].frame=0;
				}
	
				

				AnimateNextBall();




				vausAnimFrame=0;
				vausAnimType=1;
				
				

				do{
					WaitVsync(vs);
					
					processBalls();
					processWarp();				
					processLasers();					
					processControls();
					AnimateVaus();
					processPowerUps();
					processHardBricks();
					if(startWarp)doWarp(); //go to next level
					
				}while(dead==false && bricksLeft>0);


				

				if(dead==true){
					TurnOffSprites(0,MAX_SPRITES-1);
					
					MapSprite(SPR_VAUS,map_vaus1);
					MoveSprite(SPR_VAUS,vx,vy,5,2);

					//explode Vaus
					TriggerFx(9,0xff,false);
					TriggerFx(10,0xff,false);

					vausAnimFrame=0;
					vausAnimType=2;
					while(vausAnimFrame<42){
						WaitVsync(1);		
						AnimateVaus();	
						processHardBricks();	
					}
					TurnOffSprites(1,15);

					//player--;
					WaitVsync(20);
				}

				
				
				TurnOffSprites(0,MAX_SPRITES-1);

			}while(lives>0 && bricksLeft>0);
			
			FadeOut(2,true);
			WaitVsync(20);
			currentRound++;
			if(currentRound==7) doEnd();
		
		}while(lives>0);

		//do game over stuff

	}

}


void doEnd(){
//	SetTileTable(arkanoidTile_tileset);
	ClearVram();
//	SetFontTilesIndex(ARKANOIDTILE_TILESET_SIZE);

	Print2(8,10,strEnd1);
	Print2(4,11,strEnd2);
	FadeIn(2,true);

	while(ReadJoypad(0)==0);
	while(ReadJoypad(0)!=0);
	//SoftReset();
	cli();
	asm("jmp 0");
}
/*
void DebugField(){
	int x,y;
		for(y=0;y<16;y++){
			for(x=0;x<13;x++){
				PrintHexByte2(12+x,5+y,(field[y][x])<<4);
			}
		}
		
}
*/

//Print a string using custom font set
void Print2(unsigned char x,unsigned char y,const char *string){
	
	unsigned char c,i=0;

	while(1){
		c=pgm_read_byte(&(string[i++]));
		if(c==0) break;
		SetFontinternal(x++,y,c);	
	}
	
}

void SetFontinternal(unsigned char x,unsigned char y,unsigned char c){
	if(c!=0){

		if(c==32){
			c=0+RAM_TILES_COUNT;
		}else if(c>=48 && c<=58){
			c=((c&127)-32-16+1)+RAM_TILES_COUNT;			
		}else{
			c=((c&127)-60+6)+RAM_TILES_COUNT;	
		}

		SetFont(x++,y,c);
	}
}

/*
//Print a byte in hexadecimal
void PrintHexByte2(char x,char y,unsigned char byte){
	unsigned char nibble;

	//hi nibble	
	nibble=(byte>>4);
	if(nibble<=9){
		SetFont(x,y,nibble+1+RAM_TILES_COUNT);
	}else{
		SetFont(x,y,nibble+1+ RAM_TILES_COUNT);
	}

	//lo nibble	
	nibble=(byte&0xf);
	if(nibble<=9){		
		SetFont(x+1,y,nibble+1+ RAM_TILES_COUNT);
	}else{
		SetFont(x+1,y,nibble+1+ RAM_TILES_COUNT);
	}

}
*/

/*
void MapSprite(unsigned char startSprite,const char *map){
	unsigned char tile;
	unsigned char mapWidth=pgm_read_byte(&(map[0]));
	unsigned char mapHeight=pgm_read_byte(&(map[1]));

	for(unsigned char dy=0;dy<mapHeight;dy++){
		for(unsigned char dx=0;dx<mapWidth;dx++){
			
		 	tile=pgm_read_byte(&(map[(dy*mapWidth)+dx+2]));		
			sprites[startSprite++].tileIndex=tile ;
		}
	}

}

void MoveSprite(unsigned char startSprite,unsigned char x,unsigned char y,unsigned char width,unsigned char height){
	
	for(unsigned char dy=0;dy<height;dy++){
		for(unsigned char dx=0;dx<width;dx++){
				
			sprites[startSprite].x=x+(8*dx);
			sprites[startSprite].y=y+(8*dy);
			startSprite++;
		}
	}	

}
*/
void TurnOffSprites(unsigned char startSprite,unsigned char endSprite){
	
	for(unsigned char i=startSprite;(i<=endSprite && i<MAX_SPRITES) ;i++){
		sprites[i].y=(SCREEN_TILES_V*TILE_HEIGHT);
	}	

}

void DrawMap3(unsigned char x,unsigned char y,const char *map){
	unsigned char i;
	unsigned char mapWidth=pgm_read_byte(&(map[0]));
	unsigned char mapHeight=pgm_read_byte(&(map[1]));

	for(unsigned char dy=0;dy<mapHeight;dy++){
		for(unsigned char dx=0;dx<mapWidth;dx++){
			
			i=pgm_read_byte(&(map[(dy*mapWidth)+dx+2]));
			
			vram[((y+dy)*VRAM_TILES_H)+x+dx]=(i + RAM_TILES_COUNT + FONT_TILESET_SIZE) ;
			
		}
	}

}

#ifdef TITLE

void title(){
	int frame=0;
	SetTileTable(arkanoidTile_tileset);
	ClearVram();
	SetFontTilesIndex(ARKANOIDTILE_TILESET_SIZE);

	DrawMap2(2,4,map_title);	

	Print2(9,14,strStart);
	Print2(3,21,strCopyright);
	Print2(5,23,strRights);
	Print2(3,25,strUze);

	while(ReadJoypad(0)!=BTN_START);

	TriggerFx(9,0xff,false);
	TriggerFx(10,0xff,false);


	while(frame<80){
		WaitVsync(1);
		
		if((frame>>3) & 1){
			Print2(9,14,strStart);
		}else{
			Print2(9,14,strEmpty);
		}

		frame++;
	}
	
	FadeOut(1,true);
}

#endif

void animThrusters(unsigned char count){
	static unsigned char flip=0;
	unsigned  char j;

	for(j=0;j<count;j++){

		WaitVsync(1);
	
		if(flip==0){
			SetSpriteVisibility(true);
		}else if(flip==2){
			SetSpriteVisibility(false);
		}
		flip++;
		if(flip==4)flip=0;
	}
}

void intro(){
	int frame=0,i,j,x,y;
	unsigned char c;
	
	FadeIn(2,false);

	SetTileTable(font_tileset);
	ClearVram();
	SetFontTilesIndex(0);

	srand(91);

	//put random stars
	for(i=0;i<100;i++){
		x=rand()%29;
		y=rand()%27;
		vram[(y*VRAM_TILES_H)+x]=(1 + RAM_TILES_COUNT + FONT_TILESET_SIZE ) ;
	}


	DrawMap3(7,15,map_introMap);	
	StartSong(song_arkanoid1);

	i=0,j;
	x=2;y=4;

	sprites[0].tileIndex=3;
	sprites[0].x=57;
	sprites[0].y=166;

	sprites[1].tileIndex=3;
	sprites[1].x=57;
	sprites[1].y=178;

	sprites[2].tileIndex=3;
	sprites[2].x=111;
	sprites[2].y=180;

	sprites[3].tileIndex=3;
	sprites[3].x=111;
	sprites[3].y=192;

	sprites[4].tileIndex=4;
	sprites[4].x=85;
	sprites[4].y=152;

	sprites[5].tileIndex=4;
	sprites[5].x=106;
	sprites[5].y=157;


	do{
		



		c=pgm_read_byte(&(strIntro[i++]));
		if(c=='\n'){
			x=2;
			y+=2;
		}else if(c=='\r'){						
			animThrusters(20);			

			x=2;y=4;
			for(j=4*30;j<11*30;j++){	
				vram[j]=0+RAM_TILES_COUNT+ FONT_TILESET_SIZE;
			}

		}else{
			SetFontinternal(x++,y,c);
			animThrusters(2);
		}



		if(ReadJoypad(0)&BTN_START){
			StopSong();
			break;
		}



		frame++;

	}while(c!=0);

	animThrusters(40);
	TurnOffSprites(0,MAX_SPRITES-1);
	FadeOut(2,true);
}

void processWarp(){
	unsigned char index=-1;

	if(warpActive){
		switch(warpFrame){
			case 0:
				index=0;
				break;
			case 10:
				index=1;
				break;
			case 20:
				index=2;
				break;
			case 23:
				index=3;
				break;
			case 26:
				index=4;
				break;				
			case 29:
				warpFrame=20;
				return;

		}
		if(index!=-1){
			//RestoreBackground();
		 	DrawMap2(28,23,map_warpAnim1+(7*index));
		}

		warpFrame++;
	}
}

void doWarp(){
	//warp to next level

	balls[0].y=0;	
	balls[1].y=0;
	TurnOffSprites(SPR_BALL,SPR_BALL+1);
	TurnOffSprites(SPR_POWERUP,SPR_POWERUP+1);

	if(vausPowerUp == POWERUP_L){						
		vausAnimFrame=0;
		vausAnimType=6;
	}else if(vausPowerUp == POWERUP_E){						
		vausAnimFrame=0;
		vausAnimType=4;						
	}
	
	while(vausAnimType!=1){	
		WaitVsync(1);
		AnimateVaus();
		processWarp();
	}

	//cheap hack because there's no sprite clipping in the engine yet!
	//MapSprite(SPR_POWERUP,map_cloak);
	sprites[SPR_POWERUP].tileIndex=2;
	sprites[SPR_POWERUP].x=232;
	sprites[SPR_POWERUP].y=vy;

	vausAnimFrame=0;
	while(vx<=232){
		WaitVsync(1);
		processWarp();
		vausAnimFrame++;

		if((vausAnimFrame&1)==0) vx++;
		
		MoveSprite(SPR_VAUS,vx,vy,5,2);
		if(vx+32>=232){
			sprites[SPR_VAUS+4].y=(SCREEN_TILES_V*TILE_HEIGHT);
			sprites[SPR_VAUS+9].y=(SCREEN_TILES_V*TILE_HEIGHT);
		}

		if(vx+24>=232){
			sprites[SPR_VAUS+3].y=(SCREEN_TILES_V*TILE_HEIGHT);
			sprites[SPR_VAUS+8].y=(SCREEN_TILES_V*TILE_HEIGHT);
		}
				
		if(vx+16>=232){
			sprites[SPR_VAUS+2].y=(SCREEN_TILES_V*TILE_HEIGHT);
			sprites[SPR_VAUS+7].y=(SCREEN_TILES_V*TILE_HEIGHT);
		}
		
		if(vx+8>=232){
			sprites[SPR_VAUS+1].y=(SCREEN_TILES_V*TILE_HEIGHT);
			sprites[SPR_VAUS+6].y=(SCREEN_TILES_V*TILE_HEIGHT);
		}
		
		if(vx+0>=232){
			sprites[SPR_VAUS].y=(SCREEN_TILES_V*TILE_HEIGHT);
			sprites[SPR_VAUS+5].y=(SCREEN_TILES_V*TILE_HEIGHT);
		}
	}
	TurnOffSprites(0,MAX_SPRITES-1);

	bricksLeft=0;
								
}

void processLasers(){
	unsigned char i,lx,ly,brickDestroyed,brickTouched;	

	for(i=0;i<2;i++){
		
		if(lasers[i].active){
			lx=lasers[i].x;
			ly=lasers[i].y;
			brickDestroyed=0;
			brickTouched=0;
			
			if(ly<=12){
				lasers[i].active=false;
				sprites[SPR_LASER+i].y=(SCREEN_TILES_V*TILE_HEIGHT);
			}else{
				sprites[SPR_LASER+i].x=lx;
				sprites[SPR_LASER+i].y=ly;				

				if(ly >=24 && ly <152){
					
					//check if the left laser hit something
					ty=(ly/8)-3;
					tx=(lx/16)-1;										
					brick=field[ty][tx];
					if(brick!=0)brickTouched++;
					if(checkBricks(1))brickDestroyed++;

					//check if the right laser hit something
					tx=((lx+8)/16)-1;										
					brick=field[ty][tx];
					if(brick!=0)brickTouched++;
					if(checkBricks(1))brickDestroyed++;


					if(brickDestroyed>0 || brickTouched>0){
						lasers[i].active=false;
						sprites[SPR_LASER+i].y=(SCREEN_TILES_V*TILE_HEIGHT);

					}

				}
			
				lasers[i].y-=6;
			}
		
		}

	}

}

void processHardBricks(){
	unsigned char i,frame;
	
	for(i=0;i<HARD_BRICKS_ANIM_COUNT;i++){
	
		if(hardBricks[i].frame!=0){
			frame=hardBricks[i].frame>>2;
			if(hardBricks[i].type==2)frame+=6;		
			DrawMap2((hardBricks[i].x*2)+FIELD_LEFT,hardBricks[i].y+FIELD_TOP,(const char*)pgm_read_word(&(brickAnim[frame])));
			hardBricks[i].frame++;
			if(hardBricks[i].frame>=24)hardBricks[i].frame=0;
		}
	
	}

}

void processPowerUps(){
	unsigned char i;

	if(powerUpActive){

		//check if Vaus catched the power up
		if((powerUpY+6)>=vy && powerUpY< (vy+8) ){ 
			
			if( (powerUpX+12)>=vx && powerUpX<(vx+vausWidth-4) ){
				
				TurnOffSprites(SPR_POWERUP,SPR_POWERUP+1);

				//cancel previous power-up effect
				XSpeed=DEFAULT_X_SPEED;
				YSpeed=DEFAULT_Y_SPEED;


				//apply new effect
				switch(powerUpType){

					case POWERUP_S: //slow down

						XSpeed=0x0c;//0x013;
						YSpeed=0x0f;//0x015;

						vausPowerUp=powerUpType;
						break;

					case POWERUP_E: //Enlarge

						if(vausPowerUp == POWERUP_L){						
							vausAnimFrame=0;
							vausAnimType=6;
						}else{
							vausAnimFrame=0;
							vausAnimType=3;
						}

						vausPowerUp=powerUpType;
						break;
					
					case POWERUP_L: //Laser
		
						if(vausPowerUp == POWERUP_E){						
							vausAnimFrame=0;
							vausAnimType=4;
						}else{
							vausAnimFrame=0;
							vausAnimType=5;
						}

						vausPowerUp=powerUpType;
						break;

					case POWERUP_C: //catch			
					case POWERUP_D: //multi-balls

						if(vausPowerUp == POWERUP_L){						
							vausAnimFrame=0;
							vausAnimType=6;
						}else if(vausPowerUp == POWERUP_E){						
							vausAnimFrame=0;
							vausAnimType=4;						
						}
						
						if(powerUpType==POWERUP_D){	
												
							if(balls[0].y==0){
								balls[0].x=balls[1].x;
								balls[0].y=balls[1].y;

								balls[0].dy=balls[1].dy;
								if(balls[1].dx<0){
									balls[0].dx=balls[1].dx-(1<<8);
								}else{
									balls[0].dx=balls[1].dx+(1<<8);
								}
							}else{
								balls[1].x=balls[0].x;
								balls[1].y=balls[0].y;

								balls[1].dy=balls[0].dy;
								if(balls[0].dx<0){
									balls[1].dx=balls[0].dx-(1<<8);
								}else{
									balls[1].dx=balls[0].dx+(1<<8);
								}
							}


							activeBalls=2;
							sprites[SPR_BALL+0].tileIndex=0;
							sprites[SPR_BALL+1].tileIndex=0;

						}						

						vausPowerUp=powerUpType;
						break;


						case POWERUP_B:
							warpActive=true;
							warpFrame=0;
							break;


				}
				
				
				powerUpActive=false;
				return;	
			}
		}
		


		i=powerUpFrame >>3;

		MapSprite(SPR_POWERUP,map_s_powerup1+(powerUpType*16)+(i*4));
		MoveSprite(SPR_POWERUP,powerUpX,powerUpY,2,1);		

		powerUpFrame++;
		powerUpFrame&=0x1f;

		powerUpY++;
		if(powerUpY>=(SCREEN_TILES_V*TILE_HEIGHT)){
			TurnOffSprites(SPR_POWERUP,SPR_POWERUP+2);
			powerUpActive=false;
		}
		
		

	}

}




void DrawBgTile(unsigned char x,unsigned char y, bool shadowed){
	unsigned char i,dx,dy;

	if(x<26){

		dx=x&3;
		dy=y&3;

		if(shadowed==true) dx+=4;	
		
		i=pgm_read_byte(&(bgPtr[(dy*8)+dx+2]));		

		vram[((y+1)*VRAM_TILES_H)+x+2]=(i + RAM_TILES_COUNT) ;			
	}
}


void DrawLevel(unsigned char currentRound){
	unsigned char x,y,brick,i;

	//set level background ptr
	bgPtr=map_bg1+(34*(currentRound&3));
	
	//RestoreBackground();

	DrawMap2(1,1,map_wall);
	DrawMap2(28,1,map_wall);
	DrawMap2(1,0,map_ceiling);

	//draw field background
	for(y=0;y<27;y++){
		for(x=0;x<26;x++){			
			DrawBgTile(x,y,(y==0 || x==0) );
		}
	}


	bricksLeft=0;

	//draw brick shadows
	for(y=0;y<16;y++){
		for(x=0;x<13;x++){
			brick=(unsigned char)pgm_read_byte(&(levels[currentRound][(y*13)+x]));
			if(brick!=0){				
				DrawBgTile((x*2)+1,y+1+2,true);
				DrawBgTile((x*2)+2,y+1+2,true);
			}
		}
	}

	//draw bricks
	for(y=0;y<16;y++){
		for(x=0;x<13;x++){

			brick=(unsigned char)pgm_read_byte(&(levels[currentRound][(y*13)+x]));
			if(brick!=0){				
				DrawMap2((x*2)+FIELD_LEFT,y+FIELD_TOP,map_brick0+(4*(brick-1)));	
				
				//skip unbrekable bricks
				if(brick!=1) bricksLeft++;			
			}
			
			//grey bricks has the hit count in the upper nibble
			if(brick==2){
				brick |= ((2+(currentRound/8))<<4);
			}

			field[y][x]=brick;

		}
	}
	

	//clear all hard bricks animations
	for(i=0;i<HARD_BRICKS_ANIM_COUNT;i++){
		hardBricks[i].frame=0;
	}
	


		

}


void AnimateNextBall(){

	//next ball
	StartSong(song_arkanoid2);

	MapSprite(1,map_round);
	MoveSprite(1,92,152-8,5,1);
	
	sprites[7].tileIndex=5+currentRound+1;
	sprites[7].x=92+(6*8);
	sprites[7].y=152-8;

	WaitVsync(30);

	MapSprite(8,map_ready);
	MoveSprite(8,100,168-8,5,1);
	
	WaitVsync(50);

	TurnOffSprites(0,12);

	vausAnimFrame=0;
	vausAnimType=0;
	while(vausAnimFrame<38){
		WaitVsync(1);		
		AnimateVaus();	
		MoveSprite(SPR_VAUS,vx,vy,5,2);	
	}


}

void AnimateVaus(){

	const char *ptr=NULL;
	char index=-1;

	switch(vausAnimType){
	  case 0:
		//warp-in
		switch(vausAnimFrame){
			case 0:
				index=0;
				break;
			case 10:
				index=1;
				break;
			case 16:	
				index=2;
				break;
			case 22:
				index=3;
				break;				
			case 27:
				index=4;
				break;			
			case 32:
				index=5;
				break;
		}	
		if(index!=-1)MapSprite(SPR_VAUS,map_vauswarp1+(index*12));
		vausAnimFrame++;
		break;

	case 1:

		//animate the "tail lights"
		switch(vausAnimFrame){
			case 0:
				index=0;
				break;
			case 12:
			case 60:
				index=1;				
				break;
			case 24:
			case 48:
				index=2;				
				break;
			case 36:
				index=3;				
				break;	
		}	

		if(index!=-1){
			if(vausWidth>32){
				ptr=map_vausLong1+(index*16);
			}else if(vausPowerUp==POWERUP_L){
				ptr=map_vausLaser1+(index*12);
			}else{
				ptr=map_vaus1+(index*12);
			}

			MapSprite(SPR_VAUS,ptr);
		}
		
		MoveSprite(SPR_VAUS,vx,vy,(vausWidth/8)+1,2);
		vausAnimFrame++;
		if(vausAnimFrame==70)vausAnimFrame=0;
		break;	
 	  
	  case 2:
		//die!
		switch(vausAnimFrame){
			case 0:
				MapSprite(SPR_VAUS,map_vausdie1);
				break;

			case 10:

				MapSprite(SPR_VAUS,map_vausdie2);
				break;

			case 20:
				MoveSprite(SPR_VAUS,vx-2,vy-3,5,2);
				MapSprite(SPR_VAUS,map_vausdie3);
				break;

			case 25:
				MapSprite(SPR_VAUS,map_vausdie4);
				MoveSprite(SPR_VAUS,vx-2,vy-4,5,2);
				break;	
			
			case 32:
				MapSprite(SPR_VAUS,map_vausdie5);
				MoveSprite(SPR_VAUS,vx-4,vy-4,5,2);
				break;
			
			case 38:
				MapSprite(SPR_VAUS,map_vausdie6);
				MoveSprite(SPR_VAUS,vx-5,vy-4,5,2);
				break;
		}	
		
		vausAnimFrame++;
		break;
		
	  case 3:
		//stretch
		if(vausAnimFrame==0){
			MapSprite(SPR_VAUS,map_vausMiddle);
			MapSprite(SPR_VAUS+2,map_vausMiddle);
			MapSprite(SPR_VAUS+4,map_vausLeftHalf);												
			MapSprite(SPR_VAUS+8,map_vausRightHalf);
			TriggerFx(12,0x90,false);
			TriggerFx(13,0x90,false);
		}else{
		
			if(vausWidth<48 ){

				vx--;
				vausWidth+=2;	

			}else{
				MapSprite(SPR_VAUS,map_vausLong1);
				MoveSprite(SPR_VAUS,vx,vy,(vausWidth/8)+1,2);
				vausAnimType=1;
				vausAnimFrame=0;
				return;
			}	

		}

		MoveSprite(SPR_VAUS,vx+16,vy,1,2);
		MoveSprite(SPR_VAUS+2,vx+vausWidth-(3*8),vy,1,2);			
		MoveSprite(SPR_VAUS+4,vx,vy,2,2);
		MoveSprite(SPR_VAUS+8,vx+vausWidth-(2*8),vy,3,2);
		vausAnimFrame++;
	  	break;
		

	  case 4:
	  	//shrink
		if(vausAnimFrame==0){
			MapSprite(SPR_VAUS,map_vausMiddle);
			MapSprite(SPR_VAUS+2,map_vausMiddle);
			MapSprite(SPR_VAUS+4,map_vausLeftHalf);												
			MapSprite(SPR_VAUS+8,map_vausRightHalf);
			TriggerFx(14,0xa0,false);
		}else{
		
			if(vausWidth>32 ){

				vx++;
				vausWidth-=2;	

			}else{
				TurnOffSprites(SPR_VAUS+10,SPR_VAUS+14);
								
				if(vausPowerUp == POWERUP_L ){
					vausAnimType=5;
					vausAnimFrame=0;				
				}else{				
					MapSprite(SPR_VAUS,map_vaus1);					
					MoveSprite(SPR_VAUS,vx,vy,(vausWidth/8)+1,2);
					vausAnimType=1;
					vausAnimFrame=0;
				}

				return;
			}	

		}

		MoveSprite(SPR_VAUS,vx+16,vy,1,2);
		MoveSprite(SPR_VAUS+2,vx+vausWidth-(3*8),vy,1,2);			
		MoveSprite(SPR_VAUS+4,vx,vy,2,2);
		MoveSprite(SPR_VAUS+8,vx+vausWidth-(2*8),vy,3,2);
		vausAnimFrame++;
	  	break;
	
	
		case 5:
		
			//transform to laser
			switch(vausAnimFrame){
				case 0:
					index=0;
					break;
				case 5:
					index=1;
					break;
				case 10:
					index=2;
					break;
				case 15:
					index=3;				
					break;				
				case 20:			
					index=4;				
					break;	
			}

			if(index!=-1) MapSprite(SPR_VAUS,map_vausTransformLaser1+(index*12));
			MoveSprite(SPR_VAUS,vx,vy,5,2);		
			
			vausAnimFrame++;

			if(vausAnimFrame>=25){
				MapSprite(SPR_VAUS,map_vausLaser1);
				vausAnimFrame=0;
				vausAnimType=1;
			}

			break;
	
		case 6:
		
			//transform to normal from laser
			switch(vausAnimFrame){
				case 0:
					index=4;
					break;
				case 5:
					index=3;
					break;
				case 10:
					index=2;
					break;
				case 15:
					index=1;				
					break;				
				case 20:			
					index=0;				
					break;	
			}

			if(index!=-1) MapSprite(SPR_VAUS,map_vausTransformLaser1+(index*12));
			MoveSprite(SPR_VAUS,vx,vy,5,2);		
			
			vausAnimFrame++;

			if(vausAnimFrame>=25){
	
				if(vausPowerUp == POWERUP_E ){
					vausAnimType=3;
					vausAnimFrame=0;	
				}else{
					MapSprite(SPR_VAUS,map_vaus1);
					vausAnimFrame=0;
					vausAnimType=1;
				}
			}

			break;
				
	}
	

}




void checkCollision(unsigned char ball, int xOffset,int yOffset){

	int dx,dy;
	
	dx=(balls[ball].x+xOffset)>>8;
	dy=(balls[ball].y+yOffset)>>8;

	if(dy>=24 && dy<152 && dx>=16 && dx<219){

		ty=(dy/8)-3;
		tx=(dx/16)-1;										
		brick=field[ty][tx];
	}else{
		brick=0;

	}

}

void waitKey(){
	while(ReadJoypad(0)==0);
	while(ReadJoypad(0)!=0);
}


void processBalls(){
	unsigned char i,bx,by;
	int newDir;
	int bdx,bdy;
	unsigned int joy;
	bool brickDestroyed=false;




	joy=ReadJoypad(0);

	for(i=0;i<MAX_BALLS;i++){


		if(balls[i].y!=0){
	
			bdx=balls[i].dx;
			bdy=balls[i].dy;
			bx=balls[i].x>>8;
			by=balls[i].y>>8;

			if(playing){
				brickDestroyed=false;


				if( (bdx>0 && bx>RIGHT_WALL) || (bdx<0 && bx<LEFT_WALL) ){
					//check walls	
					bdx=-(bdx);
				
				}else if( (bdy<0 && by<CEILING) ){
					//check ceiling
					bdy=-(bdy);
					
				}else if(by>=16 && by<152){
					//check bricks
					


					if(bdy<0 && bdx<0){
						//check direction up-left																			
						
						//top check
						checkCollision(i,0,bdy);

						if(brick!=0){
							bdy=-(bdy);
						}else{
							//left check							
							checkCollision(i,bdx,0);
							if(brick!=0){
								bdx=-(bdx);
							}else{
								//corner check
								checkCollision(i,bdx,bdy);
								if(brick!=0){
									bdx=-(bdx);
									bdy=-(bdy);
								}
							}
						}

					}else if(bdy<0 && bdx>0){
						//check direction up-right

						//top check
						checkCollision(i,0,bdy);


						if(brick!=0){
							bdy=-(bdy);
						}else{
							//right check							
							checkCollision(i,bdx+(5<<8),0);
							if(brick!=0){
								bdx=-(bdx);
							}else{
								//corner check
								checkCollision(i,bdx+(5<<8),bdy);
								if(brick!=0){
									bdx=-(bdx);
									bdy=-(bdy);
								}
							}
						}


					}else if(bdy>0 && bdx<0){
						//check direction down-left

						//bottom check
						checkCollision(i,0,bdy+(5<<8));

						if(brick!=0){
							bdy=-(bdy);
						}else{
							//left check							
							checkCollision(i,bdx,0);
							if(brick!=0){
								bdx=-(bdx);
							}else{
								//corner check
								checkCollision(i,bdx,bdy+(5<<8));
								if(brick!=0){
									bdx=-(bdx);
									bdy=-(bdy);
								}
							}
						}

					}else{ //bdy>0 && bdx>0
						//check direction down-right

						//bottom check
						checkCollision(i,0,bdy+(5<<8));

						if(brick!=0){
							bdy=-(bdy);
						}else{
							//right check							
							checkCollision(i,bdx+(5<<8),0);
							if(brick!=0){
								bdx=-(bdx);
							}else{
								//corner check
								checkCollision(i,bdx+(5<<8),bdy+(5<<8));
								if(brick!=0){
									bdx=-(bdx);
									bdy=-(bdy);
								}
							}
						}

					}

					//check for destroyed bricks
					checkBricks(0);

				
				}else if(bdy>0 && (by+5)>vy && (by+5)<(vy+10) && (bx+5)>=vx && bx<=(vx+vausWidth)){
					//rebound on Vaus?


					if(vausPowerUp==POWERUP_C){
						//catch the ball
						bdy=0;
						bdx=0;
						balls[i].sdy=by-vy;
						balls[i].sdx=bx-vx;
						//catchedBalls++;

						//if(catchedBalls==activeBalls)
							playing=false;

					}else{					
						

				
						//process DX reversal
						if(bdx>0 && (bx+3)<=(vx+(vausWidth/2)) ){
							newDir=-1;
						}else if(bdx<0 && (bx+3)>(vx+(vausWidth/2)) ){
							newDir=1;						
						}else{
							if(bdx>0){
								newDir=1;
							}else{
								newDir=-1;
							}
						}


						//process angle
						if(bdx>0){
							//sides -- shallow angle
							if((bx+5)>=vx && (bx+5)<=(vx+3) ){
								bdx=0x1a0;
								bdy=0x120;
								
							//red pads -- 45deg
							}else if((bx+5)<=(vx+9)){ 								
								bdx=0x180;
								bdy=0x180;
							
							//grey area -- sharp angle
							}else if((bx+5)<(vx+(vausWidth/2))){
								bdx=0x120;
								bdy=0x1a0;		
							}
							
							
						}else{
						
							//sides -- shallow angle
							if(bx>=(vx+vausWidth-3) && bx<=(vx+vausWidth) ){
								bdx=0x1a0;
								bdy=0x120;

								//red pads -- 45deg
							}else if(bx>=(vx+vausWidth-9)){ 								
								bdx=0x180;
								bdy=0x180;
							
							//grey area -- sharp angle
							}else if(by>=(vx+(vausWidth/2))){
								bdx=0x120;
								bdy=0x1a0;
							}

						}

						bdx*=newDir;						
						bdy=-(bdy);

						TriggerFx(6,0x80,true);
					}

				}

								


				
				balls[i].dx=bdx;
				balls[i].dy=bdy;

				bdx=(bdx*XSpeed)>>4;
				bdy=(bdy*YSpeed)>>4;
				balls[i].x+=bdx;
				balls[i].y+=bdy;
				

			}else{
				
				//balls[i].x=vx+14;
				//balls[i].y=vy-6;

				balls[i].x=(vx+balls[i].sdx)<<8;
				balls[i].y=(vy+balls[i].sdy)<<8;
			}

	
			//check if we've lost a ball
			if((balls[i].y>>8)>(SCREEN_TILES_V*TILE_HEIGHT)){
				activeBalls--;
				balls[i].y=0;
				sprites[i].y=(SCREEN_TILES_V*TILE_HEIGHT); //turn off

				if(activeBalls==0){
					//we've lost the last ball
					dead=true;

				}
				
			}else{
				sprites[i].x=(balls[i].x>>8);			
				sprites[i].y=(balls[i].y>>8);
			}

		}

	}






}

//return true if a brick was destroyed
bool checkBricks(unsigned char  projectileType){
	bool brickDestroyed=false;
	unsigned char hits;

	if(brick!=0 && ty<16 && tx<13){

		hits=brick>>4;
		brick&=0x0f;						

		if(brick==2){
			hits--;
			if(hits==0){
				if(projectileType==0) TriggerFx(7,0x80,true);
				brickDestroyed=true;
				clearBrickAnimation(tx,ty);
			}else{
				if(projectileType==0) TriggerFx(8,0x80,true);
				field[ty][tx]=(hits<<4) | brick;								
				triggerBrickAnimation(tx,ty,2);
			}
		}else if(brick==1){
			//gold bricks can't be destroyed
			if(projectileType==0) TriggerFx(8,0x80,true);
			triggerBrickAnimation(tx,ty,1);
		}else{
			brickDestroyed=true;
			if(projectileType==0) TriggerFx(7,0x80,true);
		}


		


		if(brickDestroyed){

			//RestoreBackground();


			if(tx>0 && (ty==0 || field[ty-1][tx-1]==0) ){
				DrawBgTile((tx*2),ty+2,false);
			}else{
				DrawBgTile((tx*2),ty+2,true);
			}

			if(ty==0 || field[ty-1][tx]==0){
				DrawBgTile((tx*2)+1,ty+2,false);
			}else{
				DrawBgTile((tx*2)+1,ty+2,true);
			}


			if(field[ty+1][tx]==0){
				DrawBgTile((tx*2)+1,ty+3,false);					
			}
			if(field[ty+1][tx+1]==0 ){
				DrawBgTile((tx*2)+2,ty+3,false);
			}

			bricksLeft--;

			field[ty][tx]=0;


			//power up?
			if(!powerUpActive){
				int rnd=(unsigned int)(rand());
				if(rnd<16384){
			


					if(rnd>=0 && rnd<3000){
						rnd=POWERUP_E;
					}else if(rnd>=3000 && rnd<6000){
						rnd=POWERUP_S;
					}else if(rnd>=6000 && rnd<8500){
						rnd=POWERUP_C;
					}else if(rnd>=8500 && rnd<11500){
						rnd=POWERUP_D;
					}else if(rnd>=11500 && rnd<15800){							
						rnd=POWERUP_L;
					}else if(rnd>=15800 && rnd<16100){
						rnd=POWERUP_B;
					}else{
						rnd=POWERUP_P;									
					}

					//	rnd=POWERUP_S;
					

					if(rnd!=vausPowerUp && !(rnd==POWERUP_B  && warpActive)){
						powerUpFrame=0;																		
						powerUpType=rnd&0xf;
						powerUpActive=true;
						powerUpX=((tx*2)+2)*TILE_WIDTH;
						powerUpY=(ty+3)*TILE_HEIGHT;
					}

				}
			
			}
	

		}				

		
	}
	
	return brickDestroyed;
}

void triggerBrickAnimation(unsigned char x,unsigned char y,unsigned char type){
	//find next free slot
	unsigned char i;
	for(i=0;i<HARD_BRICKS_ANIM_COUNT;i++){
	
		if(hardBricks[i].frame==0){		
			hardBricks[i].x=x;
			hardBricks[i].y=y;
			hardBricks[i].type=type;
			hardBricks[i].frame=1;
			return;
		}
	
	}

}


void clearBrickAnimation(unsigned char x,unsigned char y){
	unsigned char i;
	for(i=0;i<HARD_BRICKS_ANIM_COUNT;i++){

		if(hardBricks[i].x==x && hardBricks[i].y==y){
			hardBricks[i].frame=0;
		}
	
	}

}


unsigned char processControls(void){
	static unsigned int lastbuttons=0;
	unsigned int joy=ReadJoypad(0),i;
	unsigned char highest=vy,activeLasers=0;

	if(joy==(BTN_Y+BTN_X+BTN_SL) && !warpActive){
		warpActive=true;
		warpFrame=0;
	}


	if(joy&BTN_LEFT){
		if(vx>15){
			if(joy&BTN_B){
				vx-=5;
			}else{
				vx-=3;
			}		
		}
	}

	if(joy&BTN_RIGHT){
		if(vx< (VRAM_TILES_H*TILE_WIDTH)-vausWidth-16){			
			if(joy&BTN_B){
				vx+=5;
			}else{
				vx+=3;
			}		
		}else if(warpActive && warpFrame>=20){
			startWarp=true;
		}

	}

	if(joy&BTN_SL){		
		vx--;
		while(ReadJoypad(0)!=0);
	}
	if(joy&BTN_SR){
		vx++;
		while(ReadJoypad(0)!=0);
	}

	if(joy&BTN_X){
		vs=3;
	}else{
		vs=1;
	}

	if(joy&BTN_A){
		if(playing==false){
			srand(TCNT1);
									
			if(balls[0].y==0){
				balls[0].x=balls[1].x;
				balls[0].y=balls[1].y;

				balls[0].dy=balls[1].dy;
				balls[0].dx=balls[1].dx;

				balls[1].y=0;
				sprites[SPR_BALL+1].y=(SCREEN_TILES_V*TILE_HEIGHT);
			}

			playing=true;	
			
			for(i=0;i<MAX_BALLS;i++){
				if(balls[i].y>0){
					balls[i].x=(vx+balls[i].sdx)<<8;
					balls[i].dx=0x180;
					if(i>0)	balls[i].dx+=0x100;			
					balls[i].dy=-(0x180);
					
					//give a left "swing" to the ball
					if(joy&BTN_LEFT)balls[i].dx=-balls[i].dx;
				}
			}

			if(vausPowerUp==POWERUP_C)TriggerFx(6,0x80,true);

		}else if(vausPowerUp==POWERUP_L){
			//process laser shots

			
			for(i=0;i<2;i++){
				if(lasers[i].active){
					activeLasers++;
					if(lasers[i].y<highest){
						highest=lasers[i].y;
					}
				}
			}
			
			for(i=0;i<2;i++){
				if(activeLasers==0 || (!lasers[i].active && highest<100) ){
					lasers[i].active=true;
					TriggerFx(11,0x90,true);
					lasers[i].x=vx+12;
					lasers[i].y=vy-8;
					sprites[SPR_LASER+i].tileIndex=1;
					break;
				}
			}
			

		}

	}

	lastbuttons=joy;

	return 0;
}



