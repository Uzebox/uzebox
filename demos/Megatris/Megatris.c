/*
 *  Uzebox(tm) Megatris
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
*/

/*
About this program:
-------------------

The game's main complication is the "multi-tasking" aspect. 2 players (game fields)
must play simultaneously, at there own speed, plus all the
animations. To avoid using a RTOS or develop some complex task
scheduler, a simpler approach was taken. A global "active field" variable
"f" is used by all functions implicated in parallel execution. That
way, both field code can be executed sequentially. A master loop call in turn
both fields. A state machine enable to have "non-blocking" animations. That
is, if player 1 "Tetris" animation is playing (and the next block is frozen), 
player 2 continues playing, unnafected. 

Concerning graphics, the main screen uses a special map, called the main map.
Its a map the size of the VRAM (40x28) and is used with a special function
called RestoreTile. When drawing a map data is copied from ROM to RAM, During
gameplay a tetramino is drawn in RAM and overwrites the previous map data. To
avoid needing more RAM to save previous image data, the RestoreTile function 
restores a tile from its map at a specific (x,y) location.

Side notes
----------
Your should keep free RAM under 85-86% to leave space for the heap and the stack.
IF you use more than that, your risk random collisions that will crash the program.

Also, the game is not optimized at all, its even quite terrible. So if your find something
weird or seems inneficient, well, your are probably right! In fact that was 
quite the idea. Program, have fun and don't care about CPU power, theres plenty left!

*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

struct tetraminoStruct {
				  char size;
				  const char blocks[4][16];

				  };

const struct tetraminoStruct tetraminos[] PROGMEM={

										//Tetrimino: S
										{3,
										{{0,1,1,
										 1,1,0,
										 0,0,0},

										 {0,1,0,
										  0,1,1,
										  0,0,1},

										 {0,0,0,
										  0,1,1,
										  1,1,0},

										 {1,0,0,
										  1,1,0,
										  0,1,0}}
										},

										//J
										{3,

										{{2,0,0,
										 2,2,2,
										 0,0,0},

										 {0,2,2,
										  0,2,0,
										  0,2,0},

										 {0,0,0,
										  2,2,2,
										  0,0,2},

										 {0,2,0,
										  0,2,0,
										  2,2,0}}
										},

										//Tetrimino: L
 										{3,
										{{0,0,3,
										 3,3,3,
										 0,0,0},

										 {0,3,0,
										  0,3,0,
										  0,3,3},

										 {0,0,0,
										  3,3,3,
										  3,0,0},

										 {3,3,0,
										  0,3,0,
										  0,3,0}}
										},

										//Tetrimino: O
										{2,
										{{4,4,
										 4,4},

										{4,4,
										 4,4},

										{4,4,
										 4,4},

										{4,4,
										 4,4}}
										},

										//Tetrimino: T										
										{3,
										{{0,5,0,
										 5,5,5,
										 0,0,0},

										 {0,5,0,
										  0,5,5,
										  0,5,0},

										 {0,0,0,
										  5,5,5,
										  0,5,0},

										 {0,5,0,
										  5,5,0,
										  0,5,0}}
										},

										//I
										{4,
									   {{0,0,0,0,
										 6,6,6,6,
										 0,0,0,0,
										 0,0,0,0},

										{0,0,6,0,
										 0,0,6,0,
										 0,0,6,0,
										 0,0,6,0},
										
										{0,0,0,0,
										 0,0,0,0,
										 6,6,6,6,
										 0,0,0,0},

										{0,6,0,0,
										 0,6,0,0,
										 0,6,0,0,
										 0,6,0,0}}

										},


										//Tetrimino: Z
										{3,
										{{7,7,0,
										 0,7,7,
										 0,0,0},

										 {0,0,7,
										  0,7,7,
										  0,7,0},

										 {0,0,0,
										  7,7,0,
										  0,7,7},

										 {0,7,0,
										  7,7,0,
										  7,0,0}}
										}
										
			


									  };

#define O_TETRAMINO 3
#define T_TETRAMINO 4
#define I_TRETRAMINO 5
#define BG_TILE 21
#define CURSOR_TILE 11

#define LOCK_DELAY 15 
#define AUTO_REPEAT_DELAY 3 
#define INTER_REPEAT_DELAY 1
#define SOFT_DROP_DELAY 1 
#define ANIMATE_LOCK 1
#define ANIMATE_NONE 0

#define FIELD_HEIGHT 22
#define FIELD_WIDTH 10
#define NEW_BLOCK -1
#define NO_BLOCK -1
#define UNDEFINED -1
#define INACTIVE -1
#define GHOST_TILE 19 //9
#define LINES_PER_LEVEL 10

#define ANIM_BACK_TO_BACK 0
#define ANIM_T_SPIN 1
#define ANIM_TETRIS 2

//declare custom assembly functions
extern void RestoreTile(char x,char y);
extern void LoadMap(void);
extern void SetTileMap(const int *data);

void menu(void);
void drawTetramino(int x,int y,int tetramino,int rotation,int forceTile,bool restore,bool clipToField);
void drawBigLetter(int cx,int cy,char letter,int tile);
void game(void);
bool processGravity(void);
unsigned char processControls(void);
bool updateFields(void);
void initFields(void);
bool moveBlock(int dispX,int dispY);
void issueNewBlock(int block);
bool lockBlock(void);
void doGameOver(void);
bool rotateBlock(int newRotation);
bool fitCheck(int x,int y,int block,int rotation);
void hardDrop(void);
void fillFieldLine(int y,int tile);
void copyFieldLines(int rowSource,int rowDest,int len);
unsigned char waitKey();
void fill(int x,int y,int width,int height,int tile);
void updateStats(int lines,int softDropTiles,int hardDropTiles,int tSpin);
void hold(void);
void waitForStartGame(void);
void updateGhostPiece(bool restore);
void restore(int x,int y,int width,int height);
int  randomize(void);
bool processGarbage();
void optionsMenu();
void runStateMachine();
bool animFields(void);
void WaitVsyncAndProcessAnimations(int count);
bool doGameContinue(void);
void DrawLogo(void);
void DrawMainMenu(void);
void OptionsMenu(void);
void restoreFields(void);

bool updateFieldsEvent(void);
void processAnimations(unsigned char f);

struct fieldStruct {
					unsigned char currentState;
					unsigned char subState;	
					unsigned char lastClearCount;
					bool hardDroppped;
					char nextBlock;
					unsigned char currBlock;
					char holdBlock;
					char currBlockSize;
					char currBlockX;
					char currBlockY;
					unsigned char currBlockRotation;
					char ghostBlockY;
					char left;
					char right;
					char bottom;
					char top;
					bool locking;
					char currLockDelay;
					char gravity;			//the delay in frames between blocks moving down
					char currGravity;				
					unsigned long score;
					unsigned int lines;
					unsigned int nextLevel;
					unsigned char height;
					unsigned char level;					
					bool gameOver;
					bool canHold;
					unsigned int lastButtons;
					unsigned char autoRepeatDelay;
					unsigned char interRepeatDelay;
					unsigned char softDropDelay;
					bool kickUp;
					bool useGhostBlock;
					unsigned char backToBack;
					unsigned char garbageQueue;
					unsigned char surface[22][10];
					unsigned char nextBlockPosX;
					unsigned char nextBlockPosY;
					unsigned char holdBlockPosX;
					unsigned char holdBlockPosY;
					unsigned char bag[7];			//the random bag of pieces
					unsigned char bagPos;
					bool tSpin;		   //t-spin detected on last lock 1,2,3 (or zero=no t-spin)
					bool lastOpIsRotation;		//true if last move was a rotation (gravity, or move -> false)
					unsigned char tSpinAnimFrame;
					unsigned char backToBackAnimFrame;
					unsigned char tetrisAnimFrame;
					};

struct fieldStruct fields[2];

const char strCopyright[] PROGMEM ="2008 ALEC BOURQUE";
const char strWebsite[] PROGMEM ="HTTP://WWW.UZEBOX.ORG"; //"HTTP://WWW.BELOGIC.COM/UZEBOX";
const char strLicence[] PROGMEM ="LICENCED UNDER GNU GPL V3";

const char strMenu1PGame[] PROGMEM ="1 PLAYER";
const char strMenu2PGame[] PROGMEM ="2 PLAYER";
const char strMenuVsCpuGame[] PROGMEM ="VS CPU";
const char strMenuOptions[] PROGMEM ="OPTIONS";
//const char strLines[] PROGMEM = "LINES";
//const char strLevel[] PROGMEM = "LEVEL";
//const char strScore[] PROGMEM = "SCORE";

const char strGameOver[] PROGMEM ="GAME OVER!";
const char strYouWin[] PROGMEM ="YOU WIN!";
const char strYouLose[] PROGMEM ="YOU LOSE!!";

const char strPress[] PROGMEM ="PRESS";
const char strStart[] PROGMEM ="START";

const char strGameOptions[] PROGMEM ="GAME OPTIONS";
const char strSong[] PROGMEM = "SONG:";
const char strBackToMenu[] PROGMEM = "BACK TO MENU";

const char strMenu[] PROGMEM = "MENU";
const char strPlay[] PROGMEM = "PLAY";
const char strUseGhostBlock[] PROGMEM="GHOST BLOCK:";
const char strYes[] PROGMEM = "YES";
const char strNo[] PROGMEM = "NO ";

const char strContinue[] PROGMEM ="RESUME";
const char strRestart[] PROGMEM ="RESTART";

//import tunes
#include "data/Korobeiniki-3tracks.inc"
#include "data/testrisnt.inc"
#include "data/ending.inc"

//import tiles & maps
#include "data/fonts.pic.inc"
#include "data/graphics.inc"

//import patches
#include "data/patches.inc"

/*
* Global config
*/
int randomSeed=0;
int f=0; 					//active field
bool vsMode=false; 			//two player mode
bool restart=false;			//user choose to restart level
bool goMenu=false;			//user choose to go back to menu
unsigned char P1Level=20;	//Player 1 level
unsigned char P2Level=20;	//Player 2 level
unsigned char songNo=0;		//default song
unsigned char maxSongNo=2;	//maximum number of songs



/*
* The game's entry point and main menu screen
*/
int main(){
	int rnd=0;
	unsigned char option=0;
	unsigned int c;


	InitMusicPlayer(patches);
	
	fields[0].useGhostBlock=true;
	fields[1].useGhostBlock=true;

	SetFontTable(fonts);
	SetTileTable(tetrisTiles);
	SetTileMap(map_main+2);


	DrawMainMenu();
	SetTile(14,15+option,CURSOR_TILE); //draw cursor


	while(1){
		rnd+=13;
		c=ReadJoypad(0);
		
		if(c&BTN_START || c&BTN_A){
			while(ReadJoypad(0)!=0); //wait for key release
			srand(rnd);			
			
			if(option==0){
				vsMode=false;
				game();
			}else if(option==1 || option==2){
				vsMode=true;
				game();
			}else{
				OptionsMenu();
			}

			DrawMainMenu();
			SetTile(14,15+option,CURSOR_TILE); //draw cursor
			
			
		}
		
		if(c&BTN_LEFT){
			//TriggerFx(10,0x90,true);

			while(ReadJoypad(0)!=0); //wait for key release	
		}
		if(c&BTN_RIGHT){
			//TriggerNote(3,0,(0x1<<1)+1,0xff);
			//TriggerFx(4,0x90,true);

			while(ReadJoypad(0)!=0); //wait for key release	
		}

		if(c&BTN_UP || c&BTN_DOWN || c&BTN_SELECT){
			SetTile(14,15+option,0); //erase old cursor
			TriggerFx(1,0x90,true);

			if(c&BTN_UP){
				if(option==0){
					option=3;
				}else{
					option--;
				}

			}else if(c&BTN_DOWN || c&BTN_SELECT){
				if(option==3){
					option=0;
				}else{
					option++;
				}
			}
			
			SetTile(14,15+option,CURSOR_TILE); //draw new cursor
			while(ReadJoypad(0)!=0); //wait for key release
		}

		
	}


}

void StartSongNo(unsigned char songNo){

	switch(songNo){
		case 0:
			StartSong(song_testrisnt);
			break;				
		case 1:
			StartSong(song_korobeiniki);
			break;				
			
	}
}

void OptionsMenu(){
	unsigned char c,option=0;
	bool playing=false;

	ClearVram();
	
	Print(14,8,strGameOptions);
	
	Print(11,13,strSong);
	PrintHexByte(17,13,songNo);
	
	Print(11,14,strUseGhostBlock);
	if(fields[0].useGhostBlock==true){
		Print(11+13,14,strYes);
	}else{
		Print(11+13,14,strNo);
	}	
	
	Print(11,15,PSTR("P1 LEVEL:"));
	PrintByte(11+11,15,P1Level,true);
	Print(11,16,PSTR("P2 LEVEL:"));
	PrintByte(11+11,16,P2Level,true);

	Print(11,17,strBackToMenu);

	SetTile(10,13+option,CURSOR_TILE); //draw cursor

	while(1){
		c=ReadJoypad(0);
		
		if(c&(BTN_START|BTN_A|BTN_B)){
			
			while(ReadJoypad(0)!=0); //wait for key release
			
			if(option==0){
				if(playing==true){
					StopSong();
					playing=false;
				}else{
					StartSongNo(songNo);
					playing=true;
				}
			}else if(option==4){
				StopSong();
				return;
			}

			
		}
		
		if(c&BTN_LEFT){
			if(option==0 && songNo>0){
				songNo--;
				PrintHexByte(17,13,songNo);
				TriggerFx(1,0x90,true);
			}else if(option==1){
				TriggerFx(1,0x90,true);
				if(fields[0].useGhostBlock==true){
					Print(11+13,14,strNo);
					fields[0].useGhostBlock=false;
					fields[1].useGhostBlock=false;
				}else{
					Print(11+13,14,strYes);
					fields[0].useGhostBlock=true;
					fields[1].useGhostBlock=true;
				}				
			}else if(option==2 && P1Level>0){
				P1Level--;
				PrintByte(11+11,15,P1Level,true);
				TriggerFx(1,0x90,true);
			}else if(option==3 && P2Level>0){
				P2Level--;
				PrintByte(11+11,16,P2Level,true);
				TriggerFx(1,0x90,true);
			}

			while(ReadJoypad(0)!=0); //wait for key release	
		}
		if(c&BTN_RIGHT){
			if(option==0 && songNo<3){
				songNo++;
				PrintHexByte(17,13,songNo);
				TriggerFx(1,0x90,true);
			}else if(option==1){
				TriggerFx(1,0x90,true);
				if(fields[0].useGhostBlock==true){
					Print(11+13,14,strNo);
					fields[0].useGhostBlock=false;
					fields[1].useGhostBlock=false;
				}else{
					Print(11+13,14,strYes);
					fields[0].useGhostBlock=true;
					fields[1].useGhostBlock=true;
				}				
			}else if(option==2 && P1Level<30){
				P1Level++;
				PrintByte(11+11,15,P1Level,true);
				TriggerFx(1,0x90,true);
	
			}else if(option==3 && P2Level<30){
				P2Level++;
				PrintByte(11+11,16,P2Level,true);
				TriggerFx(1,0x90,true);
			}

			while(ReadJoypad(0)!=0); //wait for key release	
		}

		if(c&BTN_UP || c&BTN_DOWN || c&BTN_SELECT){
			SetTile(10,13+option,0); //erase old cursor
			TriggerFx(1,0x90,true);

			if(c&BTN_UP){
				if(option==0){
					option=4;
				}else{
					option--;
				}

			}else if(c&BTN_DOWN || c&BTN_SELECT){
				if(option==4){
					option=0;
				}else{
					option++;
				}
			}
			
			SetTile(10,13+option,CURSOR_TILE); //draw new cursor
			while(ReadJoypad(0)!=0); //wait for key release
		}

		
	}
}


void DrawMainMenu(){

	ClearVram();


	Print(15,15,strMenu1PGame);
	Print(15,16,strMenu2PGame);
	Print(15,17,strMenuVsCpuGame);
	Print(15,18,strMenuOptions);
	
	PrintChar(10,22,92);
	Print(12,22,strCopyright);
	Print(7,23,strLicence);
	Print(9,25,strWebsite);
	


	//draw tetris TITLE
	DrawMap(3,3,map_title);



}


void game(void){


	do{

		do{

			LoadMap();
			initFields();	

			SetTile(14,25,8);
			SetTile(15,25,20);
			PrintHexByte(16,25,0);

			SetTile(22,25,8);
			SetTile(23,25,20);
			PrintHexByte(24,25,0);

			StartSongNo(songNo);

			fields[0].currentState=0;
		 	fields[1].currentState=0;
			restart=false;
			goMenu=false;

			while(fields[0].gameOver==false && fields[1].gameOver==false){
				//syncronize gameplay on vsync (30 hz)	
				WaitVsync(1);
				f=0;
				runStateMachine();
				processAnimations(0);

				if(vsMode){
					f=1;
					runStateMachine();
					processAnimations(1);
				}

			}

		}while(restart);

	}while(!goMenu && doGameContinue());



}


bool doGameContinue(void){
	unsigned char option=0,cx=fields[0].left+2,cy=15;
	unsigned int c;


	restoreFields();
	//restore(fields[0].left,fields[0].top+2,FIELD_WIDTH,FIELD_HEIGHT-2);
	//restore(fields[1].left,fields[1].top+2,FIELD_WIDTH,FIELD_HEIGHT-2);

	Print(cx+1,cy,strPlay);
	Print(cx+1,cy+1,strMenu);
	SetTile(cx,cy+option,CURSOR_TILE); //draw new cursor

	while(1){
		c=ReadJoypad(0);
	
		if(c&(BTN_START|BTN_A|BTN_B)){
		
			while(ReadJoypad(0)!=0); //wait for key release
		
			if(option==0){
				return true;
			}else{
				return false;
			}		
		}
	

		if(c&BTN_UP || c&BTN_DOWN || c&BTN_SELECT){
			RestoreTile(cx,cy+option); //erase old cursor
			TriggerFx(1,0x90,true);

			if(c&BTN_UP){
				if(option==0){
					option=1;
				}else{
					option--;
				}

			}else if(c&BTN_DOWN || c&BTN_SELECT){
				if(option==1){
					option=0;
				}else{
					option++;
				}
			}
		
			SetTile(cx,cy+option,CURSOR_TILE); //draw new cursor
			while(ReadJoypad(0)!=0); //wait for key release
		}

	
	}


}

void startAnimation(unsigned char type){
	if(type==ANIM_T_SPIN){
		fields[f].tSpinAnimFrame=45; //90;
	}
	if(type==ANIM_BACK_TO_BACK){
		fields[f].backToBackAnimFrame=45; //90;
	}
	if(type==ANIM_TETRIS){
		
		fields[f].tetrisAnimFrame=45; //90;
	}
}

//animate non-locking stuff
void processAnimations(unsigned char f){
	unsigned char dx,dy,frame;

	//BACK-TO-BACK
	if(fields[f].backToBackAnimFrame>0){
		if(f==0){
			dx=14;		
		}else{
			dx=21;
		}
		dy=16;

		switch(fields[f].backToBackAnimFrame){			
			case 1:
				//Fill(dx,dy,5,3,BG_TILE);
				restore(dx,dy,5,3);
				break;
			case 2: //3:			
			case 40: //80:
				DrawMap(dx,dy,map_anim_backtoback1);
				break;
			case 3: //6:
			case 38: //77:				
				DrawMap(dx,dy,map_anim_backtoback2);
				break;
			case 4: //9:
			case 36: //74:				
				DrawMap(dx,dy,map_anim_backtoback3);
				break;


		}	
		fields[f].backToBackAnimFrame--;
	}
		
	//T-SPIN
	if(fields[f].tSpinAnimFrame>0){
		if(f==0){
			dx=14;		
		}else{
			dx=21;
		}
		dy=19;

		switch(fields[f].tSpinAnimFrame){			
			case 1:
				//Fill(dx,dy,7,4,BG_TILE);
				restore(dx,dy,7,4);
				break;
			case 2: //3:			
			case 45: //90:
				DrawMap(dx,dy,map_anim_spark1);
				break;
			case 3: //6:
			case 44://87:				
				DrawMap(dx,dy,map_anim_spark2);
				break;
			case 4: //9:
			case 43: //84:				
				DrawMap(dx,dy,map_anim_spark3);
				break;
			case 5: //2:
			case 42: //81:
				DrawMap(dx,dy,map_anim_spark4);
				break;			
			case 41: //78:
				DrawMap(dx+1,dy+1,map_anim_tspin);
				if(fields[f].lastClearCount==1){			
					DrawMap(dx+1,dy+2,map_anim_single);
				}else if(fields[f].lastClearCount==2){
					DrawMap(dx+1,dy+2,map_anim_double);
				}else if(fields[f].lastClearCount==3){
					DrawMap(dx+1,dy+2,map_anim_triple);
				}
				
		}	
		fields[f].tSpinAnimFrame--;
	}

	//TETRIS
	frame=fields[f].tetrisAnimFrame;
	if(frame>0){
		if(f==0){
			dx=14;		
		}else{
			dx=21;
		}
		dy=19;

		switch(frame){	
			case 1:
				//Fill(dx,dy,7,4,BG_TILE);
				restore(dx,dy,7,4);
				break;
			case 2: //2 ... 3:			
			case 45: //88 ... 90:
				DrawMap(dx,dy,map_anim_spark1);
				break;
			case 3: //4 ... 6:
			case 44: //85 ... 87:				
				DrawMap(dx,dy,map_anim_spark2);
				break;
			case 4: //... 9:
			case 43: //82 ... 84:				
				DrawMap(dx,dy,map_anim_spark3);
				break;
			case 5: //10 ... 12:
			case 42: //79 ... 81:
				DrawMap(dx,dy,map_anim_spark4);
				break;			
			
			default:
											
				switch((frame&7)>>1){
					case 0:
						DrawMap(dx+1,dy+1,map_anim_tetris1);				
						break;
					case 1:
						DrawMap(dx+1,dy+1,map_anim_tetris2);				
						break;
					case 2:
						DrawMap(dx+1,dy+1,map_anim_tetris3);				
						break;
					case 3:
						DrawMap(dx+1,dy+1,map_anim_tetris2);				
						break;
				}				


		}	
		fields[f].tetrisAnimFrame--;
	}

}


void runStateMachine(){
	unsigned char next,op;

	next=fields[f].currentState;
	
	switch(next){
		case 0:
			if(processGravity()==true){
				TriggerFx(4,0x90,true);
				fields[f].subState=0;
				next=1;
				break;				
			}
			
			op=processControls();
			if(op==1){
				next=7;
			}
			break;

		case 1:
			if(lockBlock()==true){
				fields[f].subState=0;
				next=3;
			}
			break;
		case 3:
			if(animFields()==true){
				fields[f].subState=0;
				next=4;
			}
			break;			
		case 4:
			if(updateFields()==true){
				fields[f].subState=0;
				next=5;
			}
			break;	
		case 5:
			if(processGarbage()==true){
				fields[f].subState=0;
				next=6;			
			}
			break;	
		case 6:

			issueNewBlock(NEW_BLOCK);
			updateGhostPiece(false);
			next=0;
			break;			
		case 7:
			hardDrop();
			TriggerFx(4,0x90,true);
			fields[f].subState=0;
			next=1;
	}
	
	fields[f].currentState=next;
}



void initFields(void){
	int x,y;

	//set common config to both fields
	for(x=0;x<2;x++){
		
		if(x==0){
			fields[x].level=P1Level;
			fields[x].gravity=30-(P1Level);
		}else{
			fields[x].level=P2Level;
			fields[x].gravity=30-(P2Level);		
		}
		fields[x].currGravity=0;
		fields[x].currLockDelay=0;
		fields[x].locking=false;
		fields[x].bottom=25;
		fields[x].top=4;
		fields[x].gameOver=false;
		fields[x].lastButtons=0;
		fields[x].autoRepeatDelay=0;
		fields[x].interRepeatDelay=0;
		fields[x].kickUp=false;		
		fields[x].lines=0;
		fields[x].nextLevel=10;
		fields[x].score=0;
		fields[x].height=1;
		fields[x].currBlockX=3;
		fields[x].currBlockY=0;
		fields[x].currBlockRotation=0;
		fields[x].holdBlock=NO_BLOCK;
		fields[x].nextBlock=0;
		fields[x].canHold=true;
		fields[x].ghostBlockY=UNDEFINED;
		fields[x].bagPos=7;
		fields[x].nextBlockPosY=13;
		fields[x].holdBlockPosY=2;
		fields[x].garbageQueue=0;
		fields[x].backToBack=0;
		fields[x].tSpin=false;
		fields[x].tSpinAnimFrame=0;
		fields[x].tetrisAnimFrame=0;

		//fields[x].lastClearedLines=0;
		//fields[x].animClearLinesPhase=INACTIVE;
	}

	//set field specifics
	fields[0].left=2;
	fields[0].right=13;
	fields[0].nextBlockPosX=14;
	fields[0].holdBlockPosX=2;

	fields[1].left=28;
	fields[1].right=39;
	fields[1].nextBlockPosX=22;
	fields[1].holdBlockPosX=34;

	for(y=0;y<FIELD_HEIGHT;y++){
		for(x=0;x<FIELD_WIDTH;x++){
			fields[0].surface[y][x]=0;
			fields[1].surface[y][x]=0;
		}
	}

	

	//issue twice to have a "next" block randomly defined
	f=0;
	issueNewBlock(NEW_BLOCK);
	fields[f].currBlock=fields[f].nextBlock;
	issueNewBlock(NEW_BLOCK);
	updateFields();//updateStats(0,0,0,0);
	updateGhostPiece(false);

	if(vsMode==true){
		f=1;
		issueNewBlock(NEW_BLOCK);
		fields[f].currBlock=fields[f].nextBlock;
		issueNewBlock(NEW_BLOCK);
		updateFields();//updateStats(0,0,0,0);
		updateGhostPiece(false);
	}


}

bool processGravity(void){


	//check lock delay
	if(fields[f].locking){
		fields[f].currLockDelay++;
		if(fields[f].currLockDelay>LOCK_DELAY){
			fields[f].locking=false;
			fields[f].currLockDelay=0;
			fields[f].currGravity=0;

			//lockBlock();
			//processGarbage();
			//issueNewBlock(NEW_BLOCK);
			return true;
		}
	}else{

		fields[f].currGravity++;
		if(fields[f].currGravity>=fields[f].gravity){
			fields[f].currGravity=0;
			//attemp to move block down
			if(moveBlock(fields[f].currBlockX,fields[f].currBlockY+1)){

			}else{
				fields[f].locking=true;
			}
		}
	}
	return false;
}

bool lockBlock(void){
	

	//if(fields[f].hardDroppped==true)fields[f].subState=8;



	switch(fields[f].subState){

		case 0:
			drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,10,false,true);
			break;
		case 1:
			drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,12+fields[f].currBlock,false,true);
			break;

		default:
			drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,false,true);
			//draw block on surface
			char s=pgm_read_byte(&(tetraminos[fields[f].currBlock].size));

			char cy,cx,tile;
			for(cy=0;cy<s;cy++){
				for(cx=0;cx<s;cx++){
					tile=pgm_read_byte(&(tetraminos[fields[f].currBlock].blocks[fields[f].currBlockRotation][(cy*s)+cx]));
					if(tile!=0){
						fields[f].surface[fields[f].currBlockY+cy][fields[f].currBlockX+cx]=fields[f].currBlock+1;
					}
				}
			}

			fields[f].currLockDelay=0;
			fields[f].locking=false;
			fields[f].hardDroppped=false;

			//detect potential t-spin
			if(fields[f].currBlock==T_TETRAMINO && fields[f].lastOpIsRotation==true){
				//check if at least 3 corners are occupied
				unsigned char occupied=0;

				if(fields[f].surface[(unsigned char)fields[f].currBlockY][(unsigned char)fields[f].currBlockX]!=0)occupied++;
				if(fields[f].surface[(unsigned char)fields[f].currBlockY][(unsigned char)fields[f].currBlockX+2]!=0)occupied++;
				if(fields[f].surface[(unsigned char)fields[f].currBlockY+2][(unsigned char)fields[f].currBlockX]!=0)occupied++;
				if(fields[f].surface[(unsigned char)fields[f].currBlockY+2][(unsigned char)fields[f].currBlockX+2]!=0)occupied++;
				if(occupied>=3){
					fields[f].tSpin=true;
				}else{
					fields[f].tSpin=false;
				}
			}


			return true;
	}

	fields[f].subState++;
	return false;
}





bool moveBlock(int x,int y){
	bool updateGhost=(fields[f].currBlockX!=x);

	//check if block can fit in the location

	if(!fitCheck(x,y,fields[f].currBlock,fields[f].currBlockRotation)) return false;

	//erase previous block
	if(updateGhost) updateGhostPiece(true);
	drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,true,true);	

	fields[f].currBlockX=x;
	fields[f].currBlockY=y;

	//draw new one
	if(updateGhost) updateGhostPiece(false);
	drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,false,true);

	fields[f].lastOpIsRotation=false;

	return true;
}

int randomize(void){
	
	
	//srand(randomSeed);
	return (int)(rand() % 7);

}

void issueNewBlock(int block){

	char b1,b2,b3,b4,b5,b6,b7;
	int next=0;
	
	if(block==NEW_BLOCK){

		if(fields[f].bagPos==7){
			//time to fill a new bag 
						
			b1=randomize();
			b2=randomize();while (b2==b1){b2=randomize();}
			b3=randomize();while (b3==b2||b3==b1){b3=randomize();}
			b4=randomize();while (b4==b3||b4==b2||b4==b1){b4=randomize();}
			b5=randomize();while (b5==b4||b5==b3||b5==b2||b5==b1){b5=randomize();}
			b6=randomize();while (b6==b5||b6==b4||b6==b3||b6==b2||b6==b1){b6=randomize();}
			b7=randomize();while (b7==b6||b7==b5||b7==b4||b7==b3||b7==b2||b7==b1){b7=randomize();}

			fields[f].bag[0]=b1;
			fields[f].bag[1]=b2;
			fields[f].bag[2]=b3;
			fields[f].bag[3]=b4;
			fields[f].bag[4]=b5;
			fields[f].bag[5]=b6;
			fields[f].bag[6]=b7;

			next=b1;

			fields[f].bagPos=1;
		}else{
			next=fields[f].bag[fields[f].bagPos];
			fields[f].bagPos++;
		}
		
	}else{
		next=block;
	}
	
	//update the next block
	drawTetramino(fields[f].nextBlockPosX,fields[f].nextBlockPosY,fields[f].nextBlock,0,0,true,false);
	drawTetramino(fields[f].nextBlockPosX,fields[f].nextBlockPosY,next,0,0,false,false);


	fields[f].currBlock=fields[f].nextBlock;
	fields[f].nextBlock=next;

	fields[f].currBlockX=3;
	fields[f].currBlockY=0;
	fields[f].currBlockRotation=0;
	fields[f].kickUp=false;
	fields[f].canHold=true;
	

	//check if game over
	if(!moveBlock(fields[f].currBlockX,fields[f].currBlockY))
		doGameOver();

}

void updateGhostPiece(bool restore){
	int y;

	
	if(fields[f].useGhostBlock ){
	
		
		y=fields[f].currBlockY;		
		while(1){		
			if(!fitCheck(fields[f].currBlockX,y,fields[f].currBlock,fields[f].currBlockRotation)) break;
			y++;
		}

		drawTetramino(fields[f].currBlockX+fields[f].left,y+fields[f].top-1,fields[f].currBlock,fields[f].currBlockRotation,GHOST_TILE,restore,true);
		
	}
}

void hardDrop(void){
	//TriggerFx(6,0xa0,true);

	int y=fields[f].currBlockY;		
	while(1){		
		if(!fitCheck(fields[f].currBlockX,y,fields[f].currBlock,fields[f].currBlockRotation)) break;
		y++;
	}
	
	moveBlock(fields[f].currBlockX,y-1);
	
	fields[f].locking=false;
	fields[f].currLockDelay=0;
	fields[f].currGravity=0;
	fields[f].hardDroppped=true;

}

void hold(void){


	if(fields[f].canHold && !fields[f].locking){
		//erase current block from field	
		updateGhostPiece(true);	
		drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,true,true);

		if(fields[f].holdBlock==NO_BLOCK){

			fields[f].holdBlock=fields[f].currBlock;

			//update the hold block
			drawTetramino(fields[f].holdBlockPosX,fields[f].holdBlockPosY,fields[f].holdBlock,0,0,false,false);

			issueNewBlock(NEW_BLOCK);
			updateGhostPiece(false);	
		}else{
			//swap the hold block and current block
			drawTetramino(fields[f].holdBlockPosX,fields[f].holdBlockPosY,fields[f].holdBlock,0,0,true,false);
			drawTetramino(fields[f].holdBlockPosX,fields[f].holdBlockPosY,fields[f].currBlock,0,0,false,false);
		

			int temp=fields[f].currBlock;		
			fields[f].currBlock=fields[f].holdBlock;
			fields[f].holdBlock=temp;

			fields[f].currBlockX=3;
			fields[f].currBlockY=0;
			fields[f].currBlockRotation=0;
			fields[f].kickUp=false;

			fields[f].currLockDelay=0;
			fields[f].locking=false;

			//check if game over
			if(!fitCheck(fields[f].currBlockX,fields[f].currBlockY,fields[f].currBlock,fields[f].currBlockRotation))
				doGameOver();
			
			updateGhostPiece(false);						
		}
	
		fields[f].canHold=false;
	}
}

void restoreFields(){
	restore(fields[0].left,fields[0].top+2,FIELD_WIDTH,FIELD_HEIGHT-2);
	restore(fields[1].left,fields[1].top+2,FIELD_WIDTH,FIELD_HEIGHT-2);
}


void pause(){
	unsigned char option=0,cx=fields[0].left+1,cy=15;
	unsigned int c;


	StopSong();

	restoreFields();

	Print(cx+1,cy,strContinue);
	Print(cx+1,cy+1,strRestart);
	Print(cx+1,cy+2,strMenu);

	while((ReadJoypad(0)&BTN_START));

	SetTile(cx,cy+option,CURSOR_TILE); //draw new cursor

	while(1){
		c=ReadJoypad(0);
	
		if(c&(BTN_START|BTN_A|BTN_B)){
		
			while(ReadJoypad(0)!=0); //wait for key release
		
			if(option==0){
				restoreFields();
	
				for(cy=2;cy<FIELD_HEIGHT;cy++){
					for(cx=0;cx<FIELD_WIDTH;cx++){
						if(fields[0].surface[cy][cx]!=0){
							SetTile(fields[0].left+cx,fields[0].top+cy,fields[0].surface[cy][cx]);
						}
						if(fields[1].surface[cy][cx]!=0){
							SetTile(fields[1].left+cx,fields[1].top+cy,fields[1].surface[cy][cx]);
						}
					}				
				}
				
				f=1;
				updateGhostPiece(false);
				f=0;
				updateGhostPiece(false);

				ResumeSong();
				return;
			}else if(option==1){
				fields[0].gameOver=true;
				restart=true;
				return;
			}else{
				fields[0].gameOver=true;
				goMenu=true;
				return;
			}		
		}
	

		if(c&BTN_UP || c&BTN_DOWN || c&BTN_SELECT){
			RestoreTile(cx,cy+option); //erase old cursor
			TriggerFx(1,0x90,true);

			if(c&BTN_UP){
				if(option==0){
					option=2;
				}else{
					option--;
				}

			}else if(c&BTN_DOWN || c&BTN_SELECT){
				if(option==2){
					option=0;
				}else{
					option++;
				}
			}
		
			SetTile(cx,cy+option,CURSOR_TILE); //draw new cursor
			while(ReadJoypad(0)!=0); //wait for key release
		}	
	}





	/*
	//wait for key release
	while((ReadJoypad(0)&BTN_START));

	unsigned int c;
	while(1){
		c=ReadJoypad(0);

		if(c&BTN_START){
			//wait for key release
			while((ReadJoypad(0)&BTN_START));	
			break;	
		}
		
	}


	ResumeSong();
	*/
}

unsigned char processControls(void){
		
	int dispX=0,dispY=0;
	unsigned int joy=ReadJoypad(f);


	//process reset (check if joystick is unplugged first)
	//if((joy!=0xff) && (joy&BTN_START) && (joy&BTN_SELECT) && (joy&BTN_A) && (joy&BTN_B)){
	//	while(ReadJoypad(f)!=0);
	//	SoftReset();
	//}
	

	//process hard drop
	if((joy&BTN_UP) && !(fields[f].lastButtons&BTN_UP) && !(joy&BTN_RIGHT) && !(joy&BTN_LEFT)){
		//hardDrop();	
		fields[f].lastButtons=joy;
		//randomSeed+=17;		
		return 1;
	}

	//process hold
	if(((joy&BTN_SELECT) && !(fields[f].lastButtons&BTN_SELECT)) || ((joy&BTN_SR) && !(fields[f].lastButtons&BTN_SR)) ){
		hold();
		fields[f].lastButtons=joy;
		//randomSeed+=67;
		return 0;
	}

	//process pause
	if((joy&BTN_START) && !(fields[f].lastButtons&BTN_START)){
		pause();
		return 0;
	}

	//process left-right-down
	if(joy&BTN_RIGHT){
		if(!(fields[f].lastButtons&BTN_RIGHT) || (fields[f].autoRepeatDelay>AUTO_REPEAT_DELAY && fields[f].interRepeatDelay>INTER_REPEAT_DELAY)){		
			dispX=1;

		}
		//randomSeed+=83;
	}
	
	if(joy&BTN_LEFT){
		if(!(fields[f].lastButtons&BTN_LEFT) || (fields[f].autoRepeatDelay>AUTO_REPEAT_DELAY && fields[f].interRepeatDelay>INTER_REPEAT_DELAY)){		
			dispX=-1;
			
		}
		//randomSeed+=113;
	}

	if(joy&BTN_DOWN){
		if(fields[f].softDropDelay>SOFT_DROP_DELAY){		
			dispY=1;
			fields[f].softDropDelay=0;
		}
		//randomSeed+=41;
	}

	if((dispX!=0 || dispY!=0) && moveBlock(fields[f].currBlockX+dispX,fields[f].currBlockY+dispY)){
		fields[f].currLockDelay=0;
		fields[f].locking=false;
		fields[f].interRepeatDelay=0;
		TriggerFx(5,0x40,true);
	}

	if(joy&(BTN_LEFT|BTN_RIGHT)){
		fields[f].autoRepeatDelay++;
		fields[f].interRepeatDelay++;
	}else{
		fields[f].autoRepeatDelay=0;
		fields[f].interRepeatDelay=0;		
	}
	
	if(joy&BTN_DOWN){
		fields[f].softDropDelay++;
	}else{
		fields[f].softDropDelay=0;
	}

	//process rotation
	int rot=0;
	if(joy&BTN_A && !(fields[f].lastButtons&BTN_A)){
		//rotate right
		rot=fields[f].currBlockRotation+1;
		if(rot==4) rot=0;
		TriggerFx(5,0x40,true);
		rotateBlock(rot);
	}
	if(joy&BTN_B && !(fields[f].lastButtons&BTN_B)){
		//rotate right
		if(fields[f].currBlockRotation==0){
			rot=3;
		}else{
			rot=fields[f].currBlockRotation-1;
		}		
		TriggerFx(5,0x40,true);
		rotateBlock(rot);
	}

	
	fields[f].lastButtons=joy;
	
	return 0;
}

bool fitCheck(int x,int y,int block,int rotation){
	//check if block overlaps existing blocks

	int s=pgm_read_byte(&(tetraminos[block].size));

	int cy,cx,tile;
	for(cy=0;cy<s;cy++){
		for(cx=0;cx<s;cx++){
			tile=pgm_read_byte(&(tetraminos[block].blocks[rotation][(cy*s)+cx]));
			if(tile!=0){
				//check field boundaries
				if((y+cy)>FIELD_HEIGHT-1 || (x+cx)<0 || (x+cx)>FIELD_WIDTH-1) return false;
				
				//check for collisions with surface blocks
				if(fields[f].surface[y+cy][x+cx]!=0) return false;

			}
		}
	}

	return true;

}




bool rotateBlock(int newRotation){
	int x,y;
	bool rotateRight=false;

	if(fields[f].currBlockRotation==3 && newRotation==0){
		rotateRight=true;
	}else if(fields[f].currBlockRotation==0 && newRotation==3){
		rotateRight=false;
	}else if(newRotation > fields[f].currBlockRotation){
		rotateRight=true;
	}


	x=fields[f].currBlockX;
	y=fields[f].currBlockY;

	if(fields[f].currBlock==O_TETRAMINO) return true; //can't rotate the square tetramino


	if(!fitCheck(x,y,fields[f].currBlock,newRotation)){
		//try simple wall kicks
		if(fitCheck(x+1,y,fields[f].currBlock,newRotation)){
			x++;
		}else{
			if(fitCheck(x-1,y,fields[f].currBlock,newRotation)){
				x--;
			}else{
				if(fitCheck(x,y-1,fields[f].currBlock,fields[f].currBlockRotation) && fitCheck(x,y-1,fields[f].currBlock,newRotation)){
					y--;
					fields[f].kickUp=true;
				}else{
					
					//special srs moves
					if(rotateRight==true && fitCheck(x-1,y+1,fields[f].currBlock,newRotation)){
						x--;
						y++;
					}else if(rotateRight==true && fitCheck(x-1,y+2,fields[f].currBlock,newRotation)){
						x--;
						y+=2;
					}else if(rotateRight==false && fitCheck(x+1,y+1,fields[f].currBlock,newRotation)){
						x++;
						y++;
					}else if(rotateRight==false && fitCheck(x+1,y+2,fields[f].currBlock,newRotation)){
						x++;
						y+=2;
					}else{
						
						//test 2 blocks for I tetramino
						if(fitCheck(x+2,y,fields[f].currBlock,newRotation)){
							x+=2;
						}else{
							if(fitCheck(x-2,y,fields[f].currBlock,newRotation)){
								x-=2;
							}else{
								return false;
							}	
						}

					}

				
					
				}
			}
		}
	}


	//if past rotations on same block resulted in a "kick-up" insure
	//this rotation will not result in a space under it
	if(fields[f].kickUp){
		if(fitCheck(x,y+1,fields[f].currBlock,newRotation)){
			y++;
		}
	}

	//erase previous block
	updateGhostPiece(true);
	drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,true,true);

	fields[f].currBlockRotation=newRotation;
	fields[f].currBlockX=x;
	fields[f].currBlockY=y;

	//draw new one
	updateGhostPiece(false);
	drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,false,true);

	fields[f].lastOpIsRotation=true;
	fields[f].currLockDelay=0;
	fields[f].locking=false;

	return true;
		
}

bool lineCleared(int y){
	int x;


	bool cleared=true;
	for(x=0;x<10;x++){
		//printHexByte((x*3)+3,3,fields[f].surface[y][x]);
		if(fields[f].surface[y][x]!=0){
			cleared=false;
			break;
		}		
	}
	return cleared;
}

bool lineFull(int y){
	int x;


	bool full=true;
	for(x=0;x<10;x++){
		if(fields[f].surface[y][x]==0){
			full=false;
			break;
		}		
	}
	return full;
}

//animate the cleared lines
bool animFields(void){
	unsigned char y,size,tile,temp;

	size=pgm_read_byte(&(tetraminos[fields[f].currBlock].size));

	if(size+fields[f].currBlockY>FIELD_HEIGHT){
		size=FIELD_HEIGHT-fields[f].currBlockY;
	}
	
	switch(fields[f].subState){
		
		case 0: //0 ... 1:
			//animate
			fields[f].lastClearCount=0;
			for(y=0;y<size;y++){
				if(lineFull(fields[f].currBlockY+y)){				
					fill(fields[f].left,y+fields[f].currBlockY+fields[f].top,FIELD_WIDTH,1,10);			
					fields[f].lastClearCount++;
				}		
			}

			if(fields[f].tSpin==true){
				startAnimation(ANIM_T_SPIN);
				TriggerFx(19,0xff,true);
			}else if(fields[f].lastClearCount==4){
				startAnimation(ANIM_TETRIS);
				TriggerFx(20,0xff,true);
			}else if(fields[f].lastClearCount==0){
				return true;				
			}else{
				//at least one line completed
				TriggerFx(18,0xc0,true);
			}
			
			
			
			

			break;

		case 1 ... 12: //2 ... 23:
			//animate
			
			if(fields[f].lastClearCount<4){
				switch(fields[f].subState){
					case 1 ... 3:
						tile=22;
						break;
					case 4 ... 6:
						tile=24;
						break;
					case 7 ... 8:
						tile=26;
						break;
					default:
						tile=28;
						break;
				}
				fields[f].subState++;
			}else{
				tile=22+(fields[f].subState/2);
				
			}
			

			for(y=0;y<size;y++){
				if(lineFull(fields[f].currBlockY+y)){		

					if((y+fields[f].currBlockY)<8){
						temp=tile+8;				
					}else{
						temp=tile;
					}

					if(temp>=37) temp=0;

					fill(fields[f].left,y+fields[f].currBlockY+fields[f].top,FIELD_WIDTH,1,temp);						
				}		
			}

			break;

		default:
			for(y=0;y<size;y++){		
				if(lineFull(fields[f].currBlockY+y)){
					//we have a completed row!
					fillFieldLine(y+fields[f].currBlockY,0);
				}		
			}

			return true;  //last animation frame
	}

	fields[f].subState++;

	return false;

}

bool updateFields(void){


	//scan the surface for completed rows
	int x,y,size,tail,clearCount,top, garbageLines=0,fx;	
	bool difficult=false;;

	size=pgm_read_byte(&(tetraminos[fields[f].currBlock].size));

	if(size+fields[f].currBlockY>FIELD_HEIGHT){
		size=FIELD_HEIGHT-fields[f].currBlockY;
	}



	clearCount=fields[f].lastClearCount;
	if(clearCount>0){

		if(fields[f].subState==0){

				tail=size-1;

				for(top=0;top<size;top++){
	
					if(lineCleared(fields[f].currBlockY+tail)){			
						copyFieldLines(fields[f].currBlockY,fields[f].currBlockY+1,tail);
					}else{
						tail--;
					}

				}


		}else if(fields[f].subState<=clearCount){
				//move rest of field down
				//copyFieldLines(0,clearCount,fields[f].currBlockY);
				copyFieldLines(fields[f].subState-1,fields[f].subState,fields[f].currBlockY);

		}else{	
		

				//clear top lines
				for(y=0;y<clearCount;y++){
					fillFieldLine(y,0);
				}
				
				TriggerFx(10,0xa0,true);

	

				//Process garbage lines 
				if(clearCount>0){

					if(fields[f].tSpin==true){
						garbageLines=2*clearCount;
					}else{
						if(clearCount==2){
							garbageLines=1;
						}else if(clearCount==3){
							garbageLines=2;
						}else if(clearCount==4){
							garbageLines=4;						
						}
					}

					if(garbageLines==4 || fields[f].tSpin==true){
						difficult=true;
					}


					if(difficult && fields[f].backToBack!=0){
						garbageLines++;

						fx=22+fields[f].backToBack-1;
						if(fx>25)fx=25;

						TriggerFx(fx,0xff,true);

						startAnimation(ANIM_BACK_TO_BACK);
					}

	
					//send garbage lines to the other field(in 2 players mode)
					if(vsMode==true){
						if(f==0){
							fields[1].garbageQueue+=garbageLines;
						}else{
							fields[0].garbageQueue+=garbageLines;
						}
					}
					PrintHexByte(16,25,fields[0].garbageQueue);
					PrintHexByte(24,25,fields[1].garbageQueue);
				}

				//update score, lines, etc
				//commented out to save FLASH
				if(clearCount!=0){
					//fields[f].lines+=clearCount;

					//if(clearCount==1){
					//	bonus=100;
					//}else if(clearCount==2){
					//	bonus=300;
					//}else if(clearCount==3){
					//	bonus=500;
					//}else{
					//	bonus=800;
					//}
	
					//if(fields[f].backToBack!=0) bonus=(bonus*3)/2;

					if(difficult){				
						fields[f].backToBack++;
					}else{
						fields[f].backToBack=0;						
					}
			
					//score=fields[f].score+=(fields[f].level*fields[f].height)*bonus;
			
					//if(score>999999) fields[f].score=999999;
				}
				

				if(fields[f].lines > fields[f].nextLevel){
					//increase speed
					if(fields[f].gravity>0){
						fields[f].gravity--;
					}
		
					fields[f].level+=1;
					fields[f].nextLevel+=LINES_PER_LEVEL;
				}

				fields[f].tSpin=false;
				return true;
		
		}
	}else{
		fields[f].tSpin=false;
		return true;
	}

	//print scores, lines,etc
	if(f==0){
		x=19;
	}else{
		x=27;
	}



	//PrintLong(x,18,fields[f].lines);
	//PrintLong(x,21,fields[f].level);
	//PrintLong(x,24,fields[f].score);	


	fields[f].subState++;
	return false;
	
}



void copyFieldLines(int rowSource,int rowDest,int len){


	if(len>0){
		int x,y,rs,rd;

		rs=rowSource+len-1;
		rd=rowDest+len-1;

		for(y=0;y<len;y++){
			for(x=0;x<10;x++){
				fields[f].surface[rd][x]=fields[f].surface[rs][x];
				if(rd+fields[f].top>=(fields[f].top+2)){
					if(fields[f].surface[rs][x]==0){
						RestoreTile(x+fields[f].left,rd+fields[f].top);
					}else{	
						SetTile(x+fields[f].left,rd+fields[f].top,fields[f].surface[rs][x]);
					}
				}
			}

			rs--;
			rd--;
		}		

	}
}

void fill(int x,int y,int width,int height,int tile){
	int cx,cy;
	
	for(cy=0;cy<height;cy++){
		for(cx=0;cx<width;cx++){		
			SetTile(x+cx,y+cy,tile);
		}
	}
}

void restore(int x,int y,int width,int height){
	int cx,cy;
	
	for(cy=0;cy<height;cy++){
		for(cx=0;cx<width;cx++){		
			RestoreTile(x+cx,y+cy);
		}
	}
}

void fillFieldLine(int y,int tile){
	int h;

	
	for(h=0;h<10;h++){	
		fields[f].surface[y][h]=tile;
	}
}

unsigned char waitKey(){
	char c;
	while(1){
		c=ReadJoypad(0);
		if(c!=0){
			//wait for key release
			while(ReadJoypad(0)!=0);
			return c;
		}	
	}	
}



void doGameOver(void){
	unsigned char y,t,f2,tile,a,temp;
//	unsigned int joy;
	
	f2=f^1;

	fields[f].gameOver=true;
	
	StopSong();
	TriggerFx(21,0xff,true);
	
	WaitVsyncAndProcessAnimations(25);

	StartSong(song_ending);

	//clear field with stars animation
	for(t=0;t<FIELD_HEIGHT+5;t++){
		//for(a=0;a<1;a++){
			for(y=0;y<=t;y++){
				tile=22+y; //oscillate between adgencent sprites
				if(tile>=(22+7)) tile=29;

				if(t-y+2 < FIELD_HEIGHT){				
					if(tile!=29){

						if((t-y+2)<8){
							temp=tile+8;				
						}else{
							temp=tile;
						}

						Fill(fields[f].left,fields[f].top+t-y+2,FIELD_WIDTH,1,temp);
						if(vsMode==true){
							Fill(fields[f2].left,fields[f2].top+t-y+2,FIELD_WIDTH,1,temp);
						}	
					}else{
						restore(fields[f].left,fields[f].top+t-y+2,FIELD_WIDTH,1);
						if(vsMode==true){
							restore(fields[f2].left,fields[f2].top+t-y+2,FIELD_WIDTH,1);
						}						
					}
					
							
				}
			}
			
			WaitVsyncAndProcessAnimations(1);


	}

	for(y=0;y<10;y++){
		
		if(vsMode==false){
			if(y>0)restore( fields[f].left,fields[f].top+y-1+2,FIELD_WIDTH,1);
			Print(fields[f].left,fields[f].top+y+2,strGameOver);
		}else{
			if(y>0)restore( fields[f].left,fields[f].top+y-1+2,FIELD_WIDTH,1);
			Print(fields[f].left,fields[f].top+y+2,strYouLose);
			
			if(y>0)restore( fields[f2].left,fields[f2].top+y-1+2,FIELD_WIDTH,1);
			Print(fields[f2].left+1,fields[f2].top+y+2,strYouWin);

		}
		

		WaitVsyncAndProcessAnimations(1);
	}


	if(vsMode==false){
		Print(fields[f].left,fields[f].top+11,strGameOver);
	}else{
		Print(fields[f].left,fields[f].top+11,strYouLose);
		Fill(fields[f2].left,fields[f2].top+10,FIELD_WIDTH,3,26);
		Print(fields[f2].left+1,fields[f2].top+11,strYouWin);

	}

	a=1,y=1;
	while(1){
		
		WaitVsyncAndProcessAnimations(2);
		//joy=ReadJoypad(0);
		

		if(ReadJoypad(0)&(BTN_A|BTN_B|BTN_START)){
			
			while(ReadJoypad(0)!=0); //wait for key release			
			return;			
		}	
		
		if(vsMode==true){
			Fill(fields[f2].left,fields[f2].top+10,FIELD_WIDTH,3,22+a);			
			Print(fields[f2].left+1,fields[f2].top+11,strYouWin);
			a+=y;
			if(a==6){				
				y=-1;
			}else if(a==0){
				y=1;
			}
		}

		
	}
	
	
}

void waitForStartGame(void){
	
	Print(4,16,strPress);
	Print(4,17,strStart);	

	
	//wait for key
	unsigned int c;
	int rnd=0;
	while(1){
		rnd+=13;
		c=ReadJoypad(0);

		if(c&BTN_START){
			//wait for key release
			while((ReadJoypad(0)&BTN_START));
			
			restore(4,16,5,2);
			srand(rnd);
			
			return;
		}
		
	}
}


void WaitVsyncAndProcessAnimations(int count){
	int i;
	for(i=0;i<count;i++){
		WaitVsync(1);

		processAnimations(0);
		processAnimations(1);
	}
}






void drawTetramino(int x,int y,int tetramino,int rotation,int forceTile,bool restore,bool clipToField){

	int s=pgm_read_byte(&(tetraminos[tetramino].size));
	int cy,cx,tile;
	for(cy=0;cy<s;cy++){
		for(cx=0;cx<s;cx++){
			tile=pgm_read_byte(&(tetraminos[tetramino].blocks[rotation][(cy*s)+cx]));
			if(tile!=0){
								
				if(!clipToField || (clipToField && (cy+y)>=(fields[f].top+2)) ){

					if(restore){			
						RestoreTile(cx+x,cy+y);
					}else{
						if(forceTile!=0)tile=forceTile;
						SetTile(cx+x,cy+y,tile);
					}

				}
			}

		}
	}
}

bool processGarbage(){

	if(fields[f].garbageQueue>0){
		int x,y,rs,rd,hole;

		rs=1;
		rd=0;

		//move field up
		for(y=0;y<(FIELD_HEIGHT-1);y++){
			for(x=0;x<10;x++){

				//update the field array
				fields[f].surface[rd][x]=fields[f].surface[rs][x];
				
				//draw only the visible rows
				if(rd+fields[f].top>=(fields[f].top+2)){
					if(fields[f].surface[rs][x]==0){
						RestoreTile(x+fields[f].left,rd+fields[f].top);
					}else{	
						SetTile(x+fields[f].left,rd+fields[f].top,fields[f].surface[rs][x]);
					}
				}
			}
			rs++;
			rd++;

		}		
		
		rd=FIELD_HEIGHT-1;
		hole=(int)(rand() % 9);
		//draw garbage lines at bottom of field
		for(y=0;y<1;y++){
			for(x=0;x<10;x++){
				if(x==hole){
					RestoreTile(x+fields[f].left,fields[f].bottom-y);
					fields[f].surface[rd][x]=0;
				}else{
					SetTile(x+fields[f].left,fields[f].bottom-y,8);
					fields[f].surface[rd][x]=8;
				}
			}
			rd--;
		}

		fields[f].garbageQueue--;

		if(fields[f].garbageQueue==0){
			return true;
		}

	}else{
	
		PrintHexByte(16,25,fields[0].garbageQueue);
		PrintHexByte(24,25,fields[1].garbageQueue);
		return true;
	}
	
	fields[f].subState++;
	return false;
	
}

