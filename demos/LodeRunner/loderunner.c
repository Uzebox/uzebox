/*
 *  Uzebox(tm) Lode Runner
 *  Copyright (C) 2010  Alec Bourque
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
#include "loderunner.h"

#include "data/levelMaps.inc"
#include "data/tiles.inc"
#include "data/font_8x12.pic.inc"
#include "data/sprites.inc"
#include "data/sprites-title.inc"
#include "data/patches.inc"

#include "misc.c"
#include "ai.c"

const u8 playerWalkFrames[]={SPR_WALK1, SPR_WALK3, SPR_WALK2, SPR_WALK4, SPR_WALK3, SPR_WALK2};
const u8 playerClimbFrames[]={SPR_CLIMB1, SPR_CLIMB2,SPR_CLIMB2, SPR_CLIMB1};
const u8 playerClingFrames[]={SPR_CLING1, SPR_CLING2, SPR_CLING3};

//animations
const u8 anim_destroyBrick[] PROGMEM ={
		ANIM_CMD_DELAY|15,
		ANIM_CMD_SETTILE, TILE_DESTROY1, ANIM_CMD_DELAY|5,
		ANIM_CMD_SETTILE, TILE_DESTROY2, ANIM_CMD_DELAY|5,
		ANIM_CMD_SETTILE, TILE_DESTROY3, ANIM_CMD_DELAY|5,
		ANIM_CMD_SETTILE, TILE_DESTROY4, ANIM_CMD_DELAY|5,
		ANIM_CMD_SETTILE, TILE_DESTROY5, ANIM_CMD_DELAY|5,
		ANIM_CMD_SETTILE, TILE_BG_HOLE,  ANIM_CMD_DELAY|127, ANIM_CMD_DELAY|110,
		ANIM_CMD_SETTILE, TILE_DESTROY5, ANIM_CMD_DELAY|5,
		ANIM_CMD_SETTILE, TILE_DESTROY4, ANIM_CMD_DELAY|5,
		ANIM_CMD_SETTILE, TILE_DESTROY3, ANIM_CMD_DELAY|5,
		ANIM_CMD_SETTILE, TILE_DESTROY2, ANIM_CMD_DELAY|5,
		ANIM_CMD_SETTILE, TILE_DESTROY1, ANIM_CMD_DELAY|5,
		ANIM_CMD_SETTILE, TILE_BREAKABLE,
		ANIM_CMD_END
};
const u8 anim_fire[] PROGMEM ={
		ANIM_CMD_SETSPRITE,SPR_BEAM1,ANIM_CMD_DELAY|2,
		ANIM_CMD_SETSPRITE,SPR_BEAM2,ANIM_CMD_DELAY|2,
		ANIM_CMD_SETSPRITE,SPR_BEAM3,ANIM_CMD_DELAY|2,
		ANIM_CMD_SETSPRITE,SPR_BEAM4,ANIM_CMD_DELAY|2,
		ANIM_CMD_SETSPRITE,SPR_BEAM5,ANIM_CMD_DELAY|2,
		ANIM_CMD_SETSPRITE,SPR_BEAM6,ANIM_CMD_DELAY|2,
		ANIM_CMD_TURNOFFSPRITE,
		ANIM_CMD_END
};

const u8 anim_getout_of_hole[] PROGMEM={
		ANIM_CMD_SETSPRITE,SPR_FALL+SPR_ENEMY_OFFSET,ANIM_CMD_DELAY|0,
		ANIM_CMD_FLIP_SPRITE_ATTR,1,ANIM_CMD_DELAY|3,
		ANIM_CMD_FLIP_SPRITE_ATTR,1,ANIM_CMD_DELAY|3,
		ANIM_CMD_FLIP_SPRITE_ATTR,1,ANIM_CMD_DELAY|3,
		ANIM_CMD_FLIP_SPRITE_ATTR,1,ANIM_CMD_DELAY|3,
		ANIM_CMD_FLIP_SPRITE_ATTR,1,ANIM_CMD_DELAY|3,
		ANIM_CMD_FLIP_SPRITE_ATTR,1,ANIM_CMD_DELAY|3,
		ANIM_CMD_SETSPRITE,SPR_WALK4+SPR_ENEMY_OFFSET,ANIM_CMD_DELAY|10,
		ANIM_CMD_SETSPRITE,SPR_EXIT1+SPR_ENEMY_OFFSET,ANIM_CMD_DELAY|20,

		ANIM_CMD_END
};

#define SCORE_LEVEL_COMPLETE 1500
#define SCORE_GOLD_COLLECTED 250
#define SCORE_ENEMY_DEAD     150
#define SCORE_SEND_IN_HOLE   75


//local function prototypes
void ProcessPlayer(u8 id);
void ProcessGold();
void hideAllSprites();

//variable defines
const u16 autoplayCommands[] PROGMEM={BTN_RIGHT,138, BTN_UP,30,BTN_LEFT,30,BTN_A,20, 0,115, BTN_LEFT,40, BTN_UP,45, BTN_LEFT,120, BTN_DOWN,48, BTN_LEFT,20,
								BTN_RIGHT,20, BTN_UP,48, BTN_LEFT,72, BTN_UP,48, BTN_RIGHT,54, BTN_UP,80, BTN_LEFT,30,BTN_RIGHT,230,
								BTN_LEFT,10, BTN_A,10, 0,60,  BTN_LEFT, 46, BTN_UP,50, 0xff};
u16 autoplayPos;
u16 autoplayDelay;
u16 autoplayButtons;
bool autoplayActive;

Game game;
Player player[MAX_PLAYERS]; //0=active player, 1-4=enemies





int main()
{
	SetTileTable(lode_tileset);
	SetFontTilesIndex(LODE_TILESET_SIZE);
    SetSpriteVisibility(false);
    InitMusicPlayer(patches);
    ClearVram();
	FadeOut(0,true);

    loadEeprom();

	Logo();

    game.level=0;
	while(1){
		game.demoMode=false;
		game.displayCredits=false;

		player[SPR_INDEX_PLAYER].lives=6;

		hideAllSprites();
		ClearVram();
		GameTitle();
		hideAllSprites();

	    SetSpritesTileTable(lode_sprites);


		SetSpriteVisibility(true);
		setUserRamTilesCount(1);
		do{

			for(u8 i=0;i<ANIMATION_SLOTS_COUNT;i++){
				game.animations[i].commandStream=NULL;
			}

			if(game.demoMode){
				game.demoSaveLevel=game.level;
				game.level=0;
			}

			UnpackGameMap(game.level);
			FadeIn(3,false);


			if(!game.demoMode){

				//wait for player to press a key
				u16 frame=0;
				while(1){
					WaitVsync(1);
					if(frame&16){
						sprites[player[SPR_INDEX_PLAYER].spriteIndex].x=SPR_OFF;

					}else{
						sprites[player[SPR_INDEX_PLAYER].spriteIndex].x=player[SPR_INDEX_PLAYER].x>>8;
					}
					frame++;
					if(ReadJoypad(0)!=0)break;
				}
				srand(frame);

				if((saveGame.playedLevels[game.level/8] & (1<<(game.level%8)))==0){

					saveGame.playedLevels[game.level/8] |= (1<<(game.level%8));
					saveEeprom();
				}

			}else{
				//insure enemies always use the same path in demo mode
				srand(45357);
				autoplayPos=0;
				autoplayDelay=0;
				autoplayActive=true;
				game.displayCredits=true;
			}

			sprites[player[SPR_INDEX_PLAYER].spriteIndex].x=player[SPR_INDEX_PLAYER].x>>8;

			//main game loop
			do{
				WaitVsync(1);

				//update player & enemies
				for(u8 id=0;id<MAX_PLAYERS;id++){
					ProcessPlayer(id);
				}

				ProcessGold();
				ProcessAnimations();

			}while(!player[SPR_INDEX_PLAYER].died && !game.levelComplete && !game.levelQuit && !game.levelRestart);

			TriggerNote(0,2,80,0); //stop falling sound
			FadeOut(4,true);
			hideAllSprites();
			ClearVram();

			if(game.levelComplete){
				if(player[SPR_INDEX_PLAYER].died==true){
					player[SPR_INDEX_PLAYER].lives--;
				}

				//mark level as completed in savegame
				saveGame.completedLevels[game.level/8]|=1<<(game.level%8);

				game.level++;
				saveEeprom();
			}

		}while((game.level<LEVELS_COUNT && !game.demoMode && !game.levelQuit) || game.levelRestart);

		SetSpriteVisibility(false);
		hideAllSprites();
		ClearVram();

		if(!game.demoMode && !game.levelQuit){

			if(player[SPR_INDEX_PLAYER].lives>0){
				Print(7,5,PSTR("CONGRATULATIONS!"));
			}

			Print(10,9,PSTR("GAME OVER"));
			FadeIn(4,true);

			while(ReadJoypad(0)==0);
			while(ReadJoypad(0)!=0);

		}else if(game.demoMode){
			game.level=game.demoSaveLevel;
			SetMasterVolume(DEFAULT_MASTER_VOL);

			if(game.displayCredits) Credits();
		}
	}
} 



u16 demoMode(){
	if(ReadJoypad(0)!=0){
		game.levelComplete=true;
		game.displayCredits=false;
		return 0;
	}
	if(autoplayActive){
		if(autoplayDelay==0){
			autoplayButtons=pgm_read_word(&autoplayCommands[autoplayPos]);
			if(autoplayButtons==0xff){
				autoplayActive=false;
				return 0;
			}
			autoplayPos++;
			autoplayDelay=pgm_read_word(&autoplayCommands[autoplayPos]);
			autoplayPos++;
		}else{
			autoplayDelay--;
		}
		return autoplayButtons;
	}else{
		return 0;
	}
}

void RollMenu(){
	u8 c;
	for(u8 j=0;j<30;j++){
		c=vram[(VRAM_TILES_H*16)];
		for(u8 i=0;i<60;i++){
			vram[(VRAM_TILES_H*16)+i]=vram[(VRAM_TILES_H*16)+i+1];
		}
		vram[(VRAM_TILES_H*17)+29]=c;
		WaitVsync(1);
	}
}

void PauseMenu(){
	u16 joy;
	u8 option=0,pos=3;

	Print(4,17,PSTR("CONTINUE  RESTART  QUIT"));
	TriggerFx(FX_PAUSE,SFX_VOLUME,false);
	RollMenu();
	SetTile(pos,16,TILE_CURSOR);
	while(ReadJoypad(0)!=0);

	while(1){
		joy=ReadJoypad(0);
		if(joy!=0){
			if(joy==BTN_RIGHT || joy==BTN_SELECT){
				if(option==2){
					option=0;
				}else{
					option++;
				}
			}else if(joy==BTN_LEFT){
				if(option==0){
					option=2;
				}else{
					option--;
				}
			}else if(joy==BTN_START || joy==BTN_A){
				if(option==1)game.levelRestart=true;
				if(option==2)game.levelQuit=true;
				break;
			}
			TriggerFx(13,SFX_VOLUME,true);
			SetTile(pos,16,TILE_BLACK);
			if(option==0)pos=3;
			if(option==1)pos=13;
			if(option==2)pos=22;
			SetTile(pos,16,TILE_CURSOR);
			while(ReadJoypad(0)!=0);
		}
	}

	while(ReadJoypad(0)!=0);
	if(option==0){
		SetTile(pos,16,TILE_BLACK);
		RollMenu();
	}
}

void ProcessPlayer(u8 id){
	unsigned int joy=0;

	if(!player[id].active) return;

	u8 x=player[id].x>>8;
	u8 y=player[id].y>>8;

	player[id].tileAtFeet=GetTileAtFeet(x,y);
	player[id].tileAtHead=GetTileAtHead(x,y);
	player[id].tileUnder=GetTileUnder(x,y);

	if(id>=SPR_INDEX_PLAYER){

		if(game.demoMode){
			joy=demoMode();
		}else{
			joy=ReadJoypad(0);
		}

		//pause game
		if(joy&BTN_START){
			PauseMenu();
			return;
		}

		/*
		 //cheats!

		if(joy&BTN_SELECT){
			game.goldCollected=game.goldCount;
			return;
		}
		if(joy&BTN_SR){
			game.goldCollected=game.goldCount;
			game.levelComplete=true;
			return;
		}
		*/

		u8 gx,gy;
		//check if player have captured some gold
		for(u8 i=0;i<game.goldCount;i++){
			if(game.gold[i].state==GOLD_STATE_VISIBLE){
				gx=game.gold[i].x*TILE_WIDTH;
				gy=game.gold[i].y*TILE_HEIGHT;

				if((x+4)>=gx && (x+2)<=(gx+TILE_WIDTH) && y>=gy && y<=(gy+TILE_HEIGHT)){
					game.gold[i].state=GOLD_STATE_COLLECTED;
					game.goldCollected++;
					SetTile(game.gold[i].x,game.gold[i].y,TILE_BG);
					TriggerFx(0,SFX_VOLUME,false);
				}
			}
		}

		//check if player died crushed in a brick
		if(player[id].tileAtFeet==TILE_BREAKABLE){
			player[id].action=ACTION_DIE;
		}else{

			//check if player collided with enemies
			for(u8 i=0;i<SPR_INDEX_PLAYER;i++){
				if(player[i].active){
					gx=player[i].x>>8;
					gy=player[i].y>>8;

					if((x+4)>=gx && (x+2)<=(gx+TILE_WIDTH) && y>=gy && y<=(gy+TILE_HEIGHT-1)){
						player[id].action=ACTION_DIE;
					}
				}
			}
		}

	}else{
		if(player[id].action!=ACTION_FALL || player[id].action!=ACTION_DIE || player[id].action!=ACTION_RESPAWN || player[id].action!=ACTION_INHOLE){
			joy=ProcessEnemy(id);
		}

	}
	


	switch(player[id].action){
		case ACTION_WALK:

			if(joy&BTN_A){
				//fire!
				Fire(id);

			}else if(joy&BTN_RIGHT){
				Walk(id,1);

			}else if(joy&BTN_LEFT){
				Walk(id,-1);

			}else if(joy&BTN_UP){
				if(player[id].tileAtFeet==TILE_LADDER){
					Climb(id,-1);
				}

			}else if(joy&BTN_DOWN){
				if(player[id].tileUnder==TILE_LADDER || player[id].tileAtFeet==TILE_LADDER){
					Climb(id,1);
				}
			}

			break;

		case ACTION_FALL:
			Fall(id);
			break;

		case ACTION_CLIMB:
			if(joy&BTN_RIGHT){
				Walk(id,1);

			}else if(joy&BTN_LEFT){
				Walk(id,-1);

			}else if(joy&BTN_UP){
				Climb(id,-1);

			}else if(joy&BTN_DOWN){
				Climb(id,1);
			}

			break;
				
		case ACTION_CLING:
			if(joy&BTN_RIGHT){
				Cling(id,1);

			}else if(joy&BTN_LEFT){
				Cling(id,-1);

			}else if(joy&BTN_DOWN){
				Fall(id);
			}

			break;

		case ACTION_FIRE:
			Fire(id);
			break;

		case ACTION_DIE:
			Die(id);
			break;

		case ACTION_INHOLE:
			InHole(id);
			break;

		case ACTION_RESPAWN:
			Respawn(id);
			break;
	}




}


u16 ProcessEnemy(u8 id){

	if(player[id].action==ACTION_RESPAWN) return ACTION_NONE;

	u8 x=player[id].x>>8;
	u8 y=player[id].y>>8;

	//check if dead
	if(player[id].tileAtFeet==TILE_BREAKABLE){
		player[id].action=ACTION_RESPAWN;
		return ACTION_NONE;
	}

	//Grab gold
	if(player[id].capturedGoldId==-1 && IsTileGold(player[id].tileAtFeet)){

		//Find gold at location (x,y);
		s8 goldId=-1;
		for(u8 i=0;i<game.goldCount;i++){
			if(game.gold[i].x==((x+4)/TILE_WIDTH) && game.gold[i].y==(y/TILE_HEIGHT)){
				goldId=i;
				break;
			}
		}

		if(goldId!=player[id].lastCapturedGoldId){
			game.gold[goldId].state=GOLD_STATE_CAPTURED;
			player[id].capturedGoldId=goldId;
			player[id].capturedGoldDelay=(rand()%500)+120;
			SetTile(((x+4)>>3),(y/TILE_HEIGHT),TILE_BG);
		}

	//release gold
	}else if(player[id].capturedGoldId!=-1 && player[id].capturedGoldDelay==0 &&
			 player[id].action==ACTION_WALK && player[id].tileAtFeet==TILE_BG &&
			 IsTileSolid(player[id].tileUnder,id)){

		game.gold[player[id].capturedGoldId].state=GOLD_STATE_VISIBLE;
		game.gold[player[id].capturedGoldId].x=((x+4)/TILE_WIDTH);
		game.gold[player[id].capturedGoldId].y=(y/TILE_HEIGHT);
		player[id].lastCapturedGoldId=player[id].capturedGoldId;
		player[id].capturedGoldId=-1;

	//decrease gold release delay
	}else if(player[id].capturedGoldId!=-1 && player[id].capturedGoldDelay>0){

		player[id].capturedGoldDelay--;
	}


	return Ai(id);

}


void InHole(u8 id){
	if(player[id].lastAction!=ACTION_INHOLE){
		player[id].action=ACTION_INHOLE;
		player[id].frame=0;

		if(player[id].capturedGoldId!=-1){
			game.gold[player[id].capturedGoldId].state=GOLD_STATE_VISIBLE;
			game.gold[player[id].capturedGoldId].x=(player[id].x>>(8+3));
			game.gold[player[id].capturedGoldId].y=((player[id].y>>8)/TILE_HEIGHT)-1;
			player[id].capturedGoldId=-1;
			player[id].lastCapturedGoldId=-1;
			player[id].capturedGoldDelay=0;
		}
		TriggerFx(5,0x50,true);
	}

	u8 x=(player[id].x>>8);
	u8 y=(player[id].y>>8);
	u8 frame=player[id].frame;

	if(frame==150){
		TriggerAnimation(anim_getout_of_hole,x,y,player[id].spriteIndex);

	}else if(frame==170){
		player[id].x+=0;
		player[id].y-=0x200;

	}else if(frame==175){
		player[id].x+=0x100*player[id].dir;
		player[id].y-=0x300;

	}else if(frame==180){
		player[id].x+=0x200*player[id].dir;
		player[id].y-=0x300;

	}else if(frame>=185){
		player[id].x+=0x300*player[id].dir;
		player[id].y-=0x400;

		player[id].action=ACTION_WALK;
		sprites[player[id].spriteIndex].flags=(player[id].dir==1?0:SPR_FLIP_X);
	}


	sprites[player[id].spriteIndex].x=(player[id].x)>>8;
	sprites[player[id].spriteIndex].y=(player[id].y)>>8;

	player[id].frame++;
	player[id].lastAction=ACTION_INHOLE;

}


void Die(u8 id){
	if(player[id].lastAction!=ACTION_DIE){
		player[id].frame=0;
		player[id].lastAction=ACTION_DIE;
		TriggerNote(0,2,80,0); //stop falling sound
		TriggerFx(4,0xff,true);
	}

	player[id].frame++;
	if(player[id].frame&8){
		sprites[player[id].spriteIndex].x=SPR_OFF;
		//player[id].
	}else{
		sprites[player[id].spriteIndex].x=player[id].x>>8;
	}
	if(player[id].frame==90){
		player[id].died=true;
	}
}

void Respawn(u8 id){
	if(player[id].lastAction!=ACTION_RESPAWN){
		player[id].frame=0;
		player[id].lastAction=ACTION_RESPAWN;
	}

	player[id].frame++;

	if(player[id].frame==20){
		u16 respawnX;

		//do{
			respawnX=((rand()%28)+1)*TILE_WIDTH;
		//}while(IsTileBlocking(GetTileAtFeet(respawnX,player[id].y>>3)));

		player[id].y=5;
		player[id].x=respawnX<<8;

		//PrintHexByte(1,6,respawnX);

		sprites[player[id].spriteIndex].x=respawnX;
		sprites[player[id].spriteIndex].y=0;
		sprites[player[id].spriteIndex].tileIndex=SPR_EXIT1+(id<SPR_INDEX_PLAYER?SPR_ENEMY_OFFSET:0);

	}else if(player[id].frame==40){

		player[id].action=ACTION_FALL;
	}
}

void Fire(u8 id){
	if(player[id].lastAction!=ACTION_FIRE){
		player[id].frame=0;
		player[id].action=ACTION_FIRE;
		player[id].lastAction=ACTION_FIRE;

		u8 playerX=player[id].x>>8;
		u8 playerY=player[id].y>>8;
		s8 checkDisp=player[id].dir==1?8:-8;

		if(IsTileBlocking(GetTileAtFeet(playerX+checkDisp,playerY))){
			player[id].action=ACTION_WALK;
			return;
		}

		if(player[id].dir==1 && GetTileUnder(playerX+checkDisp,playerY)==TILE_BREAKABLE && !IsTileGold(GetTileAtFeet(playerX+checkDisp,playerY))){
			TriggerAnimation(anim_destroyBrick,(playerX+12)>>3,(playerY/TILE_HEIGHT)+1,0);

		}else if(player[id].dir==-1 && GetTileUnder(playerX+checkDisp,playerY)==TILE_BREAKABLE && !IsTileGold(GetTileAtFeet(playerX+checkDisp,playerY))){
			TriggerAnimation(anim_destroyBrick,(playerX-4)>>3,(playerY/TILE_HEIGHT)+1,0);

		}else if( GetTileUnder(playerX+checkDisp,playerY)!=TILE_UNBREAKABLE ){
			player[id].action=ACTION_WALK;
			return;
		}

		sprites[player[id].spriteIndex].tileIndex=SPR_FIRE;
		sprites[player[id].spriteIndex+1].flags=(player[id].dir==1?0:SPR_FLIP_X);

		TriggerFx(2,0xff,true);
		TriggerAnimation(anim_fire,playerX+(player[id].dir==1?8:-8),playerY,player[id].spriteIndex+1);
	}

	player[id].frame++;
	if(player[id].frame==25){
		player[id].action=ACTION_WALK;
		player[id].lastAction=ACTION_NONE;
		sprites[player[id].spriteIndex].tileIndex=playerWalkFrames[0];
	}

}

void Cling(u8 id,s8 dir){

	if(player[id].lastAction!=ACTION_CLING){
		player[id].frame=0;
	}

	player[id].dir=dir;
	player[id].action=ACTION_CLING;

	s32 newX=(player[id].x+(player[id].playerSpeed*dir))>>8;
	u8  newY=player[id].y>>8;

	//check if player is not blocked by screen limit or a wall
	if((newX>=0 && newX<(SCREEN_TILES_H*TILE_WIDTH)) && !IsTileBlocking(GetTileOnSide(newX,newY,dir)) ){

		player[id].x+=(player[id].playerSpeed * dir);
		player[id].frame+=player[id].frameSpeed;
		if((player[id].frame)>>4 >= (sizeof playerClingFrames)) player[id].frame=0;

		if(GetTileAtHead(newX,newY)!=TILE_ROPE){
			player[id].action=ACTION_WALK;
		}

	}else{
		//blocked!
	}

	sprites[player[id].spriteIndex].tileIndex=playerClingFrames[(player[id].frame)>>4]+(id<SPR_INDEX_PLAYER?SPR_ENEMY_OFFSET:0);
	sprites[player[id].spriteIndex].flags=(player[id].dir==1?0:SPR_FLIP_X);
	sprites[player[id].spriteIndex].x=(player[id].x)>>8;
	sprites[player[id].spriteIndex].y=newY;



	player[id].lastAction=ACTION_CLING;
}

void EndFall(u8 id,u8 action){
	player[id].action=action;
	RoundYpos(id);
	if(id>=SPR_INDEX_PLAYER){
		TriggerNote(0,2,80,0); //stop falling sound
	}
}

void Fall(u8 id){
	player[id].action=ACTION_FALL;

	if(id>=SPR_INDEX_PLAYER){
		if(player[id].lastAction!=ACTION_FALL){
			TriggerNote(0,3,80,SFX_VOLUME);
		}else{
			mixer.channels.type.wave[0].step-=16;
			mixer.channels.type.wave[0].volume-=5;
		}
	}

	u8 newX=player[id].x>>8;
	u8 newY=player[id].y>>8;

	u8 truncY=(newY/12)*12;

	//check if an enemy and fell into a hole dug by the player
	if(id<SPR_INDEX_PLAYER && IsTileHole(GetTileAtFeet(newX,truncY))){
		player[id].action=ACTION_INHOLE;
		RoundYpos(id);
		sprites[player[id].spriteIndex].y=truncY;
		SetTile(newX/TILE_WIDTH,truncY/TILE_HEIGHT,TILE_BG_STEP_ON);
		return;
	}


	//check if player has touched down on something
	u8 tile=GetTileUnder(newX,newY);
	if(IsTileBlocking(tile)){
		u8 dir=player[id].dir;
		EndFall(id,ACTION_WALK);
		Walk(id,0);
		player[id].dir=dir;
		return;
	}

	//ugly hack so enemy does not get sticked on teh rope
	if(player[id].lastAction==ACTION_CLING && player[id].playerSpeed<0x100){
		player[id].y+=0x100;
	}

	player[id].y+=(player[id].playerSpeed);
	newY=player[id].y>>8;

	sprites[player[id].spriteIndex].tileIndex=SPR_FALL+(id<SPR_INDEX_PLAYER?SPR_ENEMY_OFFSET:0);
	sprites[player[id].spriteIndex].flags=(player[id].dir==1?0:SPR_FLIP_X);
	sprites[player[id].spriteIndex].x=newX;
	sprites[player[id].spriteIndex].y=newY;

	if(GetTileAtHead(newX,newY)==TILE_ROPE){
		if(newY%12==0){
			EndFall(id,ACTION_CLING);
			player[id].lastAction=ACTION_CLING;
			return;
		}
	}else if(IsTileSolid(tile,id)){
		EndFall(id,ACTION_WALK);
	}

	player[id].lastAction=ACTION_FALL;
}



void Climb(u8 id,s8 dir){
	u8 x,y;//tileUnder,tileAtHead,tileAtFeet;

	if(player[id].lastAction!=ACTION_CLIMB){
		player[id].frame=0;
		if((player[id].x>>(8+3))<((player[id].x+0x400)>>(8+3))){
			//round X position to align to tile
			player[id].x+=0x800;
			player[id].x&=0xf8ff;
		}
	}

	player[id].action=ACTION_CLIMB;
	player[id].dir=dir;

	x=player[id].x>>8;
	y=player[id].y>>8;



	bool isLadder;
	if(dir==1){
		//if climbing down
		isLadder=(player[id].tileAtHead==TILE_LADDER || player[id].tileUnder==TILE_LADDER);
	}else{
		//if climbing up
		isLadder=(player[id].tileAtFeet==TILE_LADDER || player[id].tileAtHead==TILE_LADDER );
	}


		if(isLadder){

			if((dir==-1 && !IsTileBlocking(player[id].tileAtHead)) || (dir==1 && !IsTileBlocking(player[id].tileUnder))){

				player[id].x=player[id].x&0xf800;

				if((dir==-1 && player[id].y>3) || dir==1)
					player[id].y+=(player[id].playerSpeed * dir);

				//insure enemies doesn't reach the top of the screen
				if(id!=SPR_INDEX_PLAYER && dir==-1 && y<=4){
					player[id].dir=1;
					player[id].lastAiAction=AI_ACTION_CLIMB_DOWN;

				//we have reached the exit!
				}else if(id==SPR_INDEX_PLAYER && player[id].y <= 4 && game.goldCollected==game.goldCount){
					game.levelComplete=true;
					return;
				}

				player[id].frame+=player[id].frameSpeed;
				if((player[id].frame)>>4 >= sizeof playerClimbFrames) player[id].frame=0;

				sprites[player[id].spriteIndex].tileIndex=playerClimbFrames[player[id].frame>>4]+(id<SPR_INDEX_PLAYER?SPR_ENEMY_OFFSET:0);
				sprites[player[id].spriteIndex].flags=((player[id].frame>>5)&1)==0?0:SPR_FLIP_X;
				sprites[player[id].spriteIndex].x=player[id].x>>8;
				sprites[player[id].spriteIndex].y=player[id].y>>8;

			}else{
				player[id].action=ACTION_WALK;
			}

		}else{
			//finished ladder
			if(IsTileBG(player[id].tileUnder)){
				player[id].action=ACTION_FALL;
			}else{
				player[id].action=ACTION_WALK;
			}
		}


	player[id].lastAction=ACTION_CLIMB;

}

void Walk(u8 id,s8 dir){

	//"round corner" when exiting ladders
	if(player[id].lastAction==ACTION_CLIMB){
		u16 tmp=((player[id].y>>8)/TILE_HEIGHT)*TILE_HEIGHT;
		if(abs(tmp-(player[id].y>>8))<=4){
			player[id].y=(tmp&0xff)<<8;
		}
	}

	if(player[id].lastAction!=ACTION_WALK){
		player[id].frame=0;
	}

	player[id].dir=dir;
	player[id].action=ACTION_WALK;

	s32 newX=(player[id].x+(player[id].playerSpeed*dir))>>8;
	u8  newY=player[id].y>>8;
	u8 tileAtHead=GetTileAtHead(newX,newY);
	u8 tileAtFeet=GetTileAtFeet(newX,newY);

	//check if player is not blocked by screen limit or a wall
	if((newX>=0 && newX<(SCREEN_TILES_H*TILE_WIDTH)) && !IsTileBlocking(GetTileOnSide(newX,newY,dir)) ){

		player[id].x+=(player[id].playerSpeed * dir);
		player[id].frame+=player[id].frameSpeed;
		if((player[id].frame)>>4 >= (sizeof playerWalkFrames)) player[id].frame=0;

		if(tileAtHead==TILE_ROPE){
			player[id].action=ACTION_CLING;
			RoundYpos(id);

		}else if( !IsTileSolid(GetTileUnder(newX,newY),id) && tileAtFeet!=TILE_LADDER){
			player[id].action=ACTION_FALL;
			if((player[id].x>>(8+3))<((player[id].x+0x400)>>(8+3))){
				player[id].x+=0x800;
			}
			player[id].x&=0xf8ff;


		}

	}else{
		//blocked!
		if(id<SPR_INDEX_PLAYER){
			player[id].dir=-player[id].dir;
		}
	}

	sprites[player[id].spriteIndex].tileIndex=playerWalkFrames[(player[id].frame)>>4]+(id<SPR_INDEX_PLAYER?SPR_ENEMY_OFFSET:0);
	sprites[player[id].spriteIndex].flags=(player[id].dir==1?0:SPR_FLIP_X);
	sprites[player[id].spriteIndex].x=(player[id].x)>>8;
	sprites[player[id].spriteIndex].y=newY;

	player[id].lastAction=ACTION_WALK;
}


void hideAllSprites(){

	for(u8 j=0;j<MAX_SPRITES;j++){
		sprites[j].x=SCREEN_TILES_H*TILE_WIDTH;
	}
}
