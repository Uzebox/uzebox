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

#ifndef __LODERUNNER_H_
	#define __LODERUNNER_H_

#define EEPROM_ID			8

#define FIELD_WIDTH			28
#define FIELD_HEIGHT		16

#define ACTION_NONE			0
#define ACTION_WALK			1
#define ACTION_FALL			2
#define ACTION_CLIMB		3
#define ACTION_CLING		4
#define ACTION_FIRE			5
#define ACTION_DIE			6
#define ACTION_INHOLE		7
#define ACTION_RESPAWN		8

#define TILE_OFFSET_X		0
#define TILE_OFFSET_Y		0

#define SPR_OFF				SCREEN_TILES_H*TILE_WIDTH

#define SPR_INDEX_ENEMY		0
#define SPR_INDEX_PLAYER	MAX_PLAYERS-1


#define SPR_WALK1			0
#define SPR_WALK2			1
#define SPR_WALK3			2
#define SPR_WALK4			3
#define SPR_FALL			4
#define SPR_CLIMB1			5
#define SPR_CLIMB2			6
#define SPR_CLING1			7
#define SPR_CLING2			8
#define SPR_CLING3			9
#define SPR_FIRE			10
#define SPR_EXIT1			10

#define SPR_BEAM1			11
#define SPR_BEAM2			12
#define SPR_BEAM3			13
#define SPR_BEAM4			14
#define SPR_BEAM5			15
#define SPR_BEAM6			16
#define SPR_ENEMY_OFFSET	17


#define DIR_LEFT			-1
#define DIR_RIGHT			1

#define GOLD_STATE_VISIBLE   0
#define GOLD_STATE_CAPTURED  1
#define GOLD_STATE_COLLECTED 2

#define SFX_VOLUME 			128
#define FX_PAUSE			12

#define ANIMATION_SLOTS_COUNT 32
#define ANIM_CMD_END		0
#define ANIM_CMD_SETTILE	1
#define ANIM_CMD_SETSPRITE  2
#define ANIM_CMD_TURNOFFSPRITE 3
#define ANIM_CMD_SETSPRITEATTR 4
#define ANIM_CMD_FLIP_SPRITE_ATTR 5
#define ANIM_CMD_DELAY		0x80

#define MAX_GOLD 32

extern void Walk(u8 id,s8 dir);
extern void Climb(u8 id,s8 dir);
extern void Fall(u8 id);
extern void Fire(u8 id);
extern void Cling(u8 id,s8 dir);
extern void Die(u8 id);
extern void InHole(u8 id);
extern void Respawn(u8 id);

extern s8 TriggerAnimation(const u8* animation,u8 x,u8 y,u8 param1);
extern void ProcessAnimations();
extern void CopyTileToRam(u8 romIndex,u8 ramIndex);
extern char ram_tiles[];
extern void UnpackGameMap(u8 mapNo);
extern void PrintByte2(int x,int y, unsigned char val);
extern void RoundYpos(u8 id);

//x and y are PIXELS values, not tiles
extern u8 GetTileAtHead(u8 x,u8 y);
extern u8 GetTileUnder(u8 x,u8 y);
extern u8 GetTileAtFeet(u8 x,u8 y);
extern u8 GetTileOnSide(u8 x,u8 y,s8 dir);

extern bool IsTileSolid(u8 tileId,u8 id);
extern bool IsTileBlocking(u8 tileId);
extern bool IsTileHole(u8 tileId);
extern bool IsTileBG(u8 tileId);

extern void UpdateInfo();
extern u16 ProcessEnemy(u8 id);
extern void PrintLong2(int x,int y, unsigned long val);

//static vars
typedef struct Player{
	s32 x;				//24:8 fixed point
	s32 y;				//24:8 fixed point
	s32 playerSpeed;	//24:8 fixed point
	u8  frame; 			//4:4 fixed point
	u8  frameSpeed; 	//4:4 fixed point
	s8  dir;			//facing direction (-1=left, 1=right)
	u8  action;			//current action (i.e: walk, fall,etc)
	u8  lastAction;
	u8  lives;			//remaining lives
	u8  spriteIndex;	//sprite slot used
	bool active;		//visible\active
	bool died;

	u8 tileAtFeet;
	u8 tileAtHead;
	u8 tileUnder;

	s8 capturedGoldId;
	s8 lastCapturedGoldId;
	u16 capturedGoldDelay;
	u8 lastAiAction;
	s16 aiTarget;
	u8 respawnX;
	u8 stuckDelay;		//when enemy is stuck, wait some random # of frames
} Player;

typedef struct Gold{
	u8 x;
	u8 y;
	u8 state;
}Gold;

typedef struct Animation{
	const u8* commandStream;
	u8 commandCount;
	u8 x;
	u8 y;
	u8 delay;
	u8 param1;
}Animation;


typedef struct Game{
	u8 goldCount;		//the number of gold to collect
	u8 goldCollected;	//remaining to collect
	u8 goldAnimFrame;
	u8 goldAnimSpeed;
	Gold gold[MAX_GOLD];
	Animation animations[ANIMATION_SLOTS_COUNT]; //data for animations
	u8 level;
	u8 totalLevels;
	bool exitLadders;
	bool levelComplete;
	bool levelRestart;
	bool levelQuit;
	bool demoMode;
	u8 demoSaveLevel;
	bool displayCredits;
}Game;



#endif
