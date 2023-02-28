
/*
 *  Uzebox Donkey Kong
 *  Copyright (C) 2010 Paul McPhee
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

/****************************************
 *			Library Dependencies		*
 ****************************************/

#include <uzebox.h>
#include <avr/pgmspace.h>
#include <string.h>

/****************************************
 *			  	 Data					*
 ****************************************/
#include "data/tiles0.inc"
#include "data/tiles1.inc"
#include "data/sprites.inc"
#include "data/patches.inc"
#include "data/songs.inc"

/****************************************
 *			  	 Defines				*
 ****************************************/
#define DK_EEPROM_ID				11
#define HZ							60
#define SCRN_HGT					(SCREEN_TILES_V * TILE_HEIGHT)
#define SCRN_WID					(SCREEN_TILES_H * TILE_WIDTH)
#define TILE_WID2					(TILE_WIDTH / 2)
#define TILE_HGT2					(TILE_HEIGHT / 2)
#define MUT_BG_COUNT				4 // 4x8 bits for 32 mutable bgs max per level
// Tiles0
#define TILE_LIVES 0x0
#define TILE_PURSE 0x3
#define TILE_BEAM_RED 0x20
#define TILE_LADDER 0x28
#define TILE_1UP 0x9d
#define TILE_LADDER_WHITE 0x6a
#define TILE_BEAM_ORANGE 0x6b
#define TILE_CONVEYOR_BELT 0x6c
#define TILE_MESH 0x6d
#define TILE_LADDER_YELLOW 0x4a
#define TILE_BEAM_BLUE 0xa8
#define TILE_POLE 0x84
#define TILE_BLANK 0x99

// Tiles1
#define TILE_BLANK1 0x0
#define TILE_1UP1 0x4
#define TILE_METERS 0x55

// Settings
#define MAX_ANIMATED_BGS 8
#define MAX_ELEVATORS 6
#define MAX_EXTRA_LIVES 5
#define MAX_ENEMIES 4
#define MAX_ACTORS (MAX_ENEMIES+1)
#define MARIO_HGT 16
#define MARIO_HGT2 (MARIO_HGT>>1)

// Utility macros
#define SWAP_SPRITE_FLAGS(f) ((f) = (f)?0:SPRITE_FLIP_X)
#define ACTOR_MOVE_RDY(m) (gameTicks&m)
#define MIN(x,y) (((x)<(y)) ? (x) : (y))
#define MAX(x,y) (((x)>(y)) ? (x) : (y))
#define ABS(x) (((x) > 0) ? (x) : -(x))
#define NORMALIZE(x) (((x) > 0) ? 1 : ((x) < 0) ? -1 : 0)
// 8-bit, 255 period LFSR (for generating pseudo-random numbers)
#define PRNG_NEXT() (prng = ((u8)((prng>>1) | ((prng^(prng>>2)^(prng>>3)^(prng>>4))<<7))))

// Collision bit-flags
#define L_INTERSECT	1
#define R_INTERSECT	2
#define V_INTERSECT	(L_INTERSECT|R_INTERSECT)
#define T_INTERSECT	4
#define B_INTERSECT	8
#define H_INTERSECT	(T_INTERSECT|B_INTERSECT)

// BG bit-flags
	// Outer
#define BGC	0x01	// Collidable
#define BGT	0x02	// Triggerable
#define BGI	0x04	// Invisible
#define BGQ	0x10	// Queryable
#define BGSTEP 0x20	// Level 1 step

	// Inner
#define BGP	0x01	// Patterned
#define BGA	0x02	// Animated (automatically repeats like a pattern)
#define BGAO 0x04	// Animated (init to off position)
#define BGEOF 0x08	// End-of-file

	// Common
#define BGM	0x80	// Mutable

// Mutatable bg event codes
#define MUT_EV_DRAW	1
#define MUT_EV_ANIM	2
#define MUT_EV_COLLISION 4

// Mutable BG flags
#define MUT_DK 0
#define MUT_SLEDGE 1
#define MUT_FIRE 2
#define MUT_LADDER 3
#define MUT_BROKEN_LADDER 4
//#define MUT_BARREL_DELETE 5
#define MUT_KONG 6
#define MUT_WIN 7
#define MUT_UMBRELLA 8
#define MUT_OUT_OF_BOUNDS 9
#define MUT_PURSE 10
#define MUT_CONVEYOR_BELT 11
#define MUT_RIVET 12
#define MUT_LADDER_RETRACT 13

// Game states
#define STATE_TITLE 0x1
#define STATE_HISCORE 0x2
#define STATE_LADDER 0x3
#define STATE_INTRO 0x4
#define STATE_PLAYING 0x5
#define STATE_PLAYER_DEAD 0x6
#define STATE_GAME_OVER 0x7
#define STATE_PLAYER_WIN 0x8
#define STATE_PAUSED 0x80

// Mario states
#define STATE_NORMAL 0x1
#define STATE_CLIMBING 0x2
#define STATE_CLIMB_FIN 0x4
#define STATE_CLIMB_ALL (STATE_CLIMBING+STATE_CLIMB_FIN)
#define STATE_FACE_WALL 0x8
#define STATE_SLEDGING 0x10
#define STATE_JUMPING 0x20
#define STATE_DYING 0x40

// Kong states
#define DK_STATE_PICKUP 0x1
#define DK_STATE_ROLL 0x2
#define DK_STATE_DECIDE 0x4
#define DK_STATE_TAUNT 0x8
#define DK_STATE_IDLE 0x10

// Barrel states
#define STATE_ROLLING 0x1
#define STATE_FALLING 0x2
#define STATE_DROPPING 0x4
#define STATE_EXPLODING 0x8
#define STATE_DESTROYED 0x10

// Sparky/ghost states
#define STATE_DESPAWN 0x0
#define STATE_SPAWNING 0x1
#define STATE_TRACING 0x2
#define STATE_JUNCTURE 0x4

// Pie states
#define STATE_FRESH 0x1
#define STATE_BURNED 0x2
#define STATE_DROPPED 0x4
//#define STATE_EXPLODING 0x8 	// Re-used from barrels
//#define STATE_DESTROYED 0x10	//

// Spring states
//#define STATE_DESPAWN 0x0 // // Re-used from sparky/ghosts
#define STATE_BOUNCING 0x1
#define STATE_FREEFALLING 0x2
// #define STATE_JUNCTURE 0x4 // Re-used from sparky/ghosts

// Ghost states
#define STATE_RED 0x1
#define STATE_BLUE 0x2
//#define STATE_EXPLODING 0x8 	// Re-used from barrels
//#define STATE_DESTROYED 0x10	//

// Actor Types
#define ACTOR_IDLE 0x1
#define ACTOR_PLAYER 0x2
#define ACTOR_BARREL 0x4
#define ACTOR_SPARKY 0x8
#define ACTOR_IGNITER 0x10
#define ACTOR_SPRING 0x20
#define ACTOR_PIE 0x40
#define ACTOR_GHOST 0x60
#define ACTOR_KONG 0x70

// Misc
#define DIR_RIGHT 1
#define DIR_LEFT -1
#define DIR_UP -1
#define DIR_DOWN 1
#define BONUS_LOC_X 23
#define BONUS_LOC_Y 3
#define SCORE_LOC_X 2
#define SCORE_LOC_Y 1
#define HISCORE_LOC_X 13
#define HISCORE_LOC_Y 1
#define LADDER_BTM 1
#define LADDER_TOP -1

// SFX settings
#define SFX_WALK0 7
#define SFX_WALK1 8
#define SFX_WALK2 9
#define SFX_WALK3 10
#define SFX_WALK_VOL 0x60
#define SFX_JUMP 11
#define SFX_JUMP_VOL 0x80
#define SFX_BONUS 12
#define SFX_BONUS_VOL 0xf0
#define SFX_DESTROY 13
#define SFX_DESTROY_VOL 0xf0
#define SFX_KONG_STOMP 14
#define SFX_KONG_STOMP_VOL 0xff
#define SFX_SPRING_BOUNCE 15
#define SFX_SPRING_BOUNCE_VOL 0xff
#define SFX_SPRING_DROP 16
#define SFX_SPRING_DROP_VOL 0xff
#define SFX_DK_FALL 17
#define SFX_DK_FALL_VOL 0xc0
#define SFX_DK_ESCAPE 18
#define SFX_DK_ESCAPE_VOL 0x80


// Repeated-sfx intervals
#define SFX_WALKING_COUNT 7
#define SFX_CLIMBING_COUNT 9

// Bonuses
#define BONUS_BARREL_JUMP 100
#define BONUS_PIE_JUMP 100
#define BONUS_RIVET 100
#define BONUS_BARREL_DESTROY 500
#define BONUS_PURSE 600
#define BONUS_UMBRELLA 800

// Enemies in flash
#define PGM_BARREL_ENEMY_INDEX 0
#define PGM_SPARKY0_ENEMY_INDEX 1
#define PGM_IGNITER_ENEMY_INDEX 2
#define PGM_SPARKY1_ENEMY_INDEX 3
#define PGM_SPARKY2_ENEMY_INDEX 4
#define PGM_SPRING_ENEMY_INDEX 5
#define PGM_SPARKY3_ENEMY_INDEX 6
#define PGM_PIE_ENEMY_INDEX 7
#define PGM_GHOST_ENEMY_INDEX 8
#define PGM_KONG_ENEMY_INDEX 9

// Enemy animation indexes
#define BARREL_ANIM_INDEX 11
#define SPARKY_ANIM_INDEX 15
#define IGNITER_ANIM_INDEX 13
#define SPRING_ANIM_INDEX 24
#define PIE_ANIM_INDEX 30
#define GHOST_ANIM_INDEX 31
#define KONG_ANIM_INDEX 33

/****************************************
 *			Type declarations			*
 ****************************************/
typedef struct {
	u8 x;
	u8 y;
} pt;

typedef struct {
	u8 w;
	u8 h;
} size;

typedef struct {
	u8 left;
	u8 right;
	u8 top;
	u8 btm;
} rect;

typedef struct {
	size s;
	u8 frameCount;
	u8 frameSequenceIndex; // Position in frameSequences array to begin loading frame indexes.
	u16 frameDurationsIndex; // Position in frameDurations array to begin loading frame timers.
	const char *frames; // First map of animation. Maps MUST be stored contiguously in flash.
} animation;

typedef struct {
	char frame;
	int counter;
	int duration;
	animation anim;
} animationController;

typedef struct bgAnimIndex {
	u8 iOuter;					// Outer bg index of bg animation
	u8 iInner;					// Inner bg index of bg animation
} bgAnimIndex;

typedef struct bgInner {
	u8 type;					// 0|BGA|BGP|BGM
	u8 tile;
	rect r;
} bgInner;

typedef struct bgOuter {
	u8 type;					// 0|BGC|BGT*|BGI|BGM
	u8 count;					// # of inner bgs in this outer bg
	u16 index;					// The position in bgiTbl at which the inner bgs begin
	rect r;
} bgOuter;

typedef struct bgDirectory {
	u8 jumpDivIndex;
	u8 jumpDivCount;
	u8 jumpIndex;
	u8 regionsIndex;
	u8 regionsCount;
	u16 bgoIndex;				// Index into pgmBgs flash array
	u8 bgoCount;				// The # of background elements in the slice
	u8 animCount;				// The # of animated background elements in the slice
	u8 animIndex;				// Index into pgmAnimDir
} bgDirectory;

typedef struct {
	u8 count;
	//bgAnimIndex ids[MAX_ANIMATED_BGS];
	bgInner bgs[MAX_ANIMATED_BGS];
	//bgInner mutBgs[MAX_ANIMATED_BGS];
	animationController acs[MAX_ANIMATED_BGS];
} animatedBgs;

typedef struct {
	u8 state;
	u8 type;
	u8 moveMod;
	u8 spriteFlags;
	size dim;
	pt loc;
	u16 moveCmd;
	u8 spriteIndex;
	animationController ac;
	char vx;
	char vy;
	char dir;
} actor;

typedef struct {
	u8 state;
	u8 counter;
} kong;

typedef struct {
	u8 state;
	pt loc;
} pathNode;

typedef struct {
	u8 key;
	u8 data;
} byteDictionary;

#include "data/levels.inc"

/****************************************
 *			File-level variables		*
 ****************************************/
u8 level;
u8 dkRound;
u8 nextFreeSprite;
animatedBgs	animBgs;
actor allActors[MAX_ENEMIES+1];
u8 vramBuf[2];
u8 mutmap[MUT_BG_COUNT];
actor *mario;
char isNearLadder;
actor *enemies;
kong dk;
u8 prng = 1;
u16 gameTicks;
pathNode aiPath[MAX_ENEMIES];
u16 sledgeTimer;
u8 pauseCounter;
u8 jumpCounter;
u8 extraLives;
u16 bonus;
u8 levelTimer;
u8 gameState;
u8 bonusTimer;
u16 bonusIndex;
pt bonusLoc;
u8 jumpBonusFlag;
char dirConveyorBelt;
//elevator elevators[MAX_ELEVATORS];
rect *elevators; // Re-use idle actor's memory
u8 climbFlag; // Hack to prevent nearby ladders overiding each other's settings
u32 score;
u32 hiScore;
char ladderRetracter;
u8 sfxCounter;

const actor pgmEnemies[] PROGMEM = {
	{ STATE_ROLLING,ACTOR_BARREL,0x3,0,{8,8},{52,60},0,0,{0,0,0,{{0,0},0,0,0,0}},0,0,DIR_RIGHT },
	{ STATE_TRACING|STATE_SPAWNING,ACTOR_SPARKY,0x1,0,{16,16},{32,184},0,0,{0,0,0,{{0,0},0,0,0,0}},0,0,DIR_RIGHT },
	{ STATE_DROPPING,ACTOR_IGNITER,0x3,0,{16,8},{40,60},71,0,{0,0,0,{{0,0},0,0,0,0}},0,0,DIR_RIGHT },
	{ STATE_JUNCTURE,ACTOR_SPARKY,0x1,0,{16,16},{80,96},40,0,{0,0,0,{{0,0},0,0,0,0}},0,0,DIR_RIGHT },
	{ STATE_JUNCTURE,ACTOR_SPARKY,0x1,0,{16,16},{224,96},56,0,{0,0,0,{{0,0},0,0,0,0}},0,0,DIR_RIGHT },
	{ STATE_JUNCTURE,ACTOR_SPRING,0x1,0,{8,16},{24,48},0,0,{0,0,0,{{0,0},0,0,0,0}},0,0,DIR_RIGHT },
	{ STATE_TRACING|STATE_SPAWNING,ACTOR_SPARKY,0x1,0,{16,16},{120,88},76,0,{0,0,0,{{0,0},0,0,0,0}},0,0,DIR_LEFT },
	{ STATE_FRESH,ACTOR_PIE,0xff,0,{8,6},{0,0},0,0,{0,0,0,{{0,0},0,0,0,0}},0,0,DIR_RIGHT },
	{ STATE_RED,ACTOR_GHOST,0x1,0,{16,16},{144,104},0,0,{0,0,0,{{0,0},0,0,0,0}},0,0,DIR_RIGHT },
	{ DK_STATE_IDLE,ACTOR_KONG,0x1,0,{24,24},{0,0},0,0,{0,0,0,{{0,0},0,0,0,0}},0,0,DIR_RIGHT },
};

const int frameDurations[] PROGMEM = {
	0x8000,				// (0) All single-frame animations
	5,5,				// (1)
	32,32,				// (3)
	12,12,				// (5)
	64, 64,				// (7)
	4,4,				// (9)
	4,					// (11)
	8,8,8,8,			// (12)
	8,8,				// (16)
	12,12,12,			// (18)
	12,12,12,12,		// (21)
	11,11,11,11,11,11,11,11,11,60, // (25)
};

const u8 frameSequences[] PROGMEM = {
	0,					// (0) All single-frame animations
	0,1,				// (1)
	1,0,				// (3)
	0,1,2,3,			// (5)
	3,2,1,0,			// (9)
	0,1,2,				// (13)
	0,3,2,1,			// (16)
	0,1,2,0,1,2,0,1,2,3	// (20)
};

const animation animations[] PROGMEM = {
	{ {4, 4}, 2, 1, 3, mapDKTaunt0 },
	{ {2, 2}, 2, 1, 5, mapFireBeam0 },
	{ {2, 4}, 2, 1, 5, mapOilFire0 },
	{ {2, 2}, 2, 1, 7, mapPauline0 },

	{ {2, 1}, 2, 1, 5, mapHalfFlame0 },
	{ {2, 2}, 2, 1, 1, mapMarioWalk0 },
	{ {2, 2}, 1, 0, 11, mapMarioClimb0 },
	{ {2, 2}, 2, 1, 9, mapMarioClimb1 },

	{ {2, 2}, 1, 0, 0, mapMarioClimb3 },
	{ {4, 4}, 1, 0, 0, mapDKBarrelLt0 },
	{ {4, 4}, 1, 0, 0, mapDKBarrelRt0 },
	{ {1, 1}, 4, 5, 12, mapBarrelRoll0 },

	{ {1, 1}, 4, 9, 12, mapBarrelRoll0 },
	{ {2, 1}, 2, 1, 1, mapBarrelDrop0 },
	{ {4, 4}, 1, 0, 0, mapDKBarrelHold },
	{ {2, 2}, 2, 1, 5, mapSparky0 },

	{ {3, 2}, 2, 1, 16, mapMarioMallet0 },
	{ {1, 1}, 3, 13, 18, mapExplode0 },
	{ {2, 2}, 10, 20, 25, mapMarioDie0 },
	{ {2, 2}, 1, 0, 0, mapMarioJump0 },

	{ {4, 1}, 2, 1, 7, mapHelp0 },
	{ {1, 1}, 2, 1, 5, mapPulleyRopeLt0 },
	{ {1, 1}, 2, 1, 5, mapPulleyRopeRt0 },
	{ {4, 4}, 1, 0, 0, mapDKIdle },

	{ {1, 2}, 2, 1, 16, mapSpring0 },
	{ {1, 1}, 4, 5, 21, mapCogLt0 },
	{ {1, 1}, 4, 5, 21, mapCogRt0 },
	{ {2, 4}, 2, 1, 5, mapBonfire0 },

	{ {1, 1}, 4, 16, 21, mapCogLt0 },
	{ {1, 1}, 4, 16, 21, mapCogRt0 },
	{ {2, 1}, 1, 0, 0, mapPie0 },
	{ {2, 2}, 2, 1, 5, mapGhostRed0 },

	{ {2, 2}, 2, 1, 5, mapGhostBlue0 },
	{ {3, 3}, 2, 1, 1, mapDKEscape0 },
	{ {4, 4}, 2, 1, 5, mapDKTaunt0 },
	{ {11, 1}, 2, 1, 3, mapPressStart0 },

	{ {3, 3}, 1, 0, 0, mapDKBouncing },
	{ {4, 4}, 2, 1, 3, mapDKTaunt10 },
};


/****************************************
 *			Function declarations		*
 ****************************************/

void DKFill(const rect *r, u8 tileId);
void DKFillMap(const rect *r, const char *map);
void DKDrawLevel(void);
void DKLoadAnimatedBgs(void);
void DKSetBgAnimationFrames(u8 bgIndex, u8 animIndex);
void DKAnimateBgs(animationController *acs, u8 count);
void DKDrawAnimatedBgs(u8 from, u8 count);
void DKMoveActor(actor *a);
void DKDrawActors(actor *a, u8 count);
u8 DKRectsIntersect(const rect *r1, const rect *r2);
void DKDetectCollisions(actor *a);
u8 DKProcessMutableBgEvent(u8 evType, bgInner *bgiSrc, bgInner *bgiInfo, void *v);
void DKSetState(u8 state, actor *a);
void DKMapSprite(u8 index, const char *map, u8 spriteFlags);
void DKSetVelocityX(actor *a, char vx);
void DKLoadActorAnimation(actor *a);
void DKSetKongState(u8 state);
void DKKongAI(void);
u8 DKFindIdleEnemy(actor **a);
actor* DKSpawnEnemy(u8 x, u8 y, u8 pgmIndex, u8 type);
void DKAnimateEnemies(void);
void DKAnimatePlayer(void);
void DKHideSprite(u8 spriteIndex, u8 wid, u8 hgt);
u8 DKServiceTimers(void);
void DKDetectActorCollisions(void);
void DKPrintNumber(u8 x, u8 y, u32 val, u8 digits);
void DKPrintExtraLives(void);
void DKPrintLevel(void);
void DKPrintBonus(void);
void DKPrintScores(u8 forcePrint);
void DKInitLevel(u8 lvl);
void DKShowWinScene(void);
void DKAddToScore(int val);
void DKDisplayBonus(u8 x, u8 y, u16 val);
void DKClearBonus(void);
void DKMoveElevators(void);
void DKDrawElevators(void);
void DKBindToElevators(void);
void DKConveyorBeltLogic(void);
void DKGhostSpawnLogic(void);
void DKLoadHiScore(void);
void DKSaveHiScore(char *name);
void DKResumeSong(void);
void DKDisplayLadderScene(u8 lvl);
void DKSyncConveyorBeltAnimations(void);
u8 DKGetMovementScale(void);
void DKPaintInners(u8 startIndex);
u8 DKIncBeamIndex(u8 index);
void DKShowIntroScene(void);
void DKSetGameState(u8 state);
void DKClearVram(void);
u8 DKShowHiScoreScreen(u8 editable);
void DKPrintString(u8 x, u8 y, char *s, u8 count);
//void DKApplyEnemyVelocity(actor *a, u8 i, u8 delta1, u8 delta2);
void DKSetActorIdle(actor *a);
u8 DKNextLevel(void);

/****************************************
 *			Function definitions		*
 ****************************************/

void DKClearVram(void) {
	DKFill(&(rect){0,240,0,224}, (gameState <= STATE_LADDER)?TILE_BLANK1:TILE_BLANK);
}

#define DIGIT_TILES_OFFSET 0x9e
#define DIGIT_TILES_OFFSET1 0x5

void DKPrintNumber(u8 x, u8 y, u32 val, u8 digits) {
	u8 numeral;

	for (u8 i = 0; i < digits; i++, val /= 10) {
		numeral = val % 10;
		SetTile(x+digits-i-1, y, ((gameState <= STATE_LADDER)?DIGIT_TILES_OFFSET1:DIGIT_TILES_OFFSET)+numeral);
	}
}

void DKHideSprite(u8 spriteIndex, u8 wid, u8 hgt) {
	u8 size = wid*hgt;

	for (int i = 0; i < size; i++)
		MoveSprite(spriteIndex+i,SCREEN_TILES_H<<3,0,1,1);
}

u8 DKFindIdleEnemy(actor **a) {
	*a = 0;

	for (u8 i = 0; i < MAX_ENEMIES; ++i) {
		if (enemies[i].type == ACTOR_IDLE) {
			*a = &enemies[i];
			return i;
		}
	}
	return 0;
}


u8 DKGetMovementScale(void) {
	u8 moveMod = 0x1;

	if (dkRound > 3 && dkRound <= 5)
		moveMod = 0x3;
	else if (dkRound > 5 && dkRound <= 9)
		moveMod = 0x7;
	else if (dkRound > 9 && dkRound <= 14)
		moveMod = 0xf;
	else if (dkRound > 14)
		moveMod = 0xff;
	return moveMod;
}

actor* DKSpawnEnemy(u8 x, u8 y, u8 pgmIndex, u8 type) {
	actor *a = 0;
	u8 idleIndex = DKFindIdleEnemy(&a),animIndex,spriteIndex;

	if (a) {
		memcpy_P(a,pgmEnemies+pgmIndex,sizeof(*a));

		switch (type) {
			case ACTOR_BARREL:
				animIndex = BARREL_ANIM_INDEX;
				spriteIndex = nextFreeSprite-2; //16-(idleIndex<<1); //6+(idleIndex<<1);
				nextFreeSprite -= 2;
				memcpy_P(&aiPath[idleIndex],barrelPaths+a->moveCmd,sizeof(aiPath[idleIndex]));
				break;
			case ACTOR_IGNITER:
				animIndex = IGNITER_ANIM_INDEX;
				spriteIndex = nextFreeSprite-2; //4;
				nextFreeSprite -= 2;
				memcpy_P(&aiPath[idleIndex],barrelPaths+a->moveCmd,sizeof(aiPath[idleIndex]));
				break;
			case ACTOR_SPARKY:
				animIndex = SPARKY_ANIM_INDEX;
				a->moveMod = DKGetMovementScale();
				spriteIndex = nextFreeSprite-4;
				nextFreeSprite -= 4;
				memcpy_P(&aiPath[idleIndex],sparkyPaths+a->moveCmd,sizeof(aiPath[idleIndex]));
				break;
			case ACTOR_SPRING:
				animIndex = SPRING_ANIM_INDEX;
				a->moveMod = DKGetMovementScale();
				spriteIndex = nextFreeSprite-2; //12;
				nextFreeSprite -= 2;
				memcpy_P(&aiPath[idleIndex],springPaths+a->moveCmd,sizeof(aiPath[idleIndex]));
				DKSetState(aiPath[idleIndex].state, a);
				break;
			case ACTOR_PIE:
				animIndex = PIE_ANIM_INDEX;
				spriteIndex = 4+(idleIndex<<1);
				nextFreeSprite -= 2;
				a->loc.x = x;
				a->loc.y = y;
				break;
			case ACTOR_GHOST:
			{
				if (idleIndex > 2) {
					DKSetActorIdle(a); //a->type = ACTOR_IDLE;
					return 0;
				}
				if (sledgeTimer) {
					animIndex = GHOST_ANIM_INDEX+1;
					a->state = STATE_BLUE;
				} else {
					animIndex = GHOST_ANIM_INDEX;
				}
				a->moveMod = DKGetMovementScale();

				do {
					PRNG_NEXT();
					a->dir = DIR_RIGHT;
					a->spriteFlags = 0;

					if ((prng&3) == 0) {
						a->loc.x = 144;
						a->loc.y = 104;
						a->moveCmd = 0;
						a->dir = DIR_LEFT;
						a->spriteFlags = SPRITE_FLIP_X;
					} else if ((prng&3) == 1) {
						a->loc.x = 192;
						a->loc.y = 168;
						a->moveCmd = 60;
					} else if ((prng&3) == 2) {
						a->loc.x = 40;
						a->loc.y = 200;
						a->moveCmd = 74;
					} else if ((prng&3) == 3) {
						a->loc.x = 96;
						a->loc.y = 64;
						a->moveCmd = 22;
					}
				} while (ABS(a->loc.x-mario->loc.x) < 24);

				spriteIndex = nextFreeSprite-4; //12-(idleIndex<<2);  //4+(idleIndex<<2);
				nextFreeSprite -=4;
				memcpy_P(&aiPath[idleIndex],ghostPaths+a->moveCmd,sizeof(aiPath[idleIndex]));
				break;
			}
			case ACTOR_KONG:
				animIndex = KONG_ANIM_INDEX;
				spriteIndex = 4;
				a->loc.x = x;
				a->loc.y = y;
				break;
			default:
				return 0;
		}
	} else {
		return 0;
	}
	memcpy_P(&a->ac.anim,animations+animIndex,sizeof(a->ac.anim));
	a->ac.duration = pgm_read_word(frameDurations+a->ac.anim.frameDurationsIndex);
	a->spriteIndex = spriteIndex;
	DKMapSprite(a->spriteIndex, a->ac.anim.frames+pgm_read_byte(
			frameSequences+a->ac.anim.frameSequenceIndex+a->ac.frame)*(a->ac.anim.s.w*a->ac.anim.s.h+2), a->spriteFlags);
	return a;
}


void DKAnimateEnemies(void) {
	animationController *ac;

	for (u8 i = 0; i < MAX_ENEMIES; ++i) {
		if ((enemies[i].type&ACTOR_IDLE) == 0) {
			if (pauseCounter && enemies[i].state != STATE_EXPLODING)
				continue;
			ac = &enemies[i].ac;

			if (++ac->counter >= ac->duration) {
				ac->counter = 0;

				if (++ac->frame >= ac->anim.frameCount) {
					if ((enemies[i].type == ACTOR_BARREL || enemies[i].type == ACTOR_PIE || enemies[i].type == ACTOR_GHOST) &&
							enemies[i].state == STATE_EXPLODING) {
						DKSetState(STATE_DESTROYED, &enemies[i]);
						continue;
					} else {
						ac->frame = 0;
					}
				}
				if (enemies[i].type == ACTOR_SPRING) {
					if (ac->frame == 0) {
						ac->anim.s = (size){1,2};
						enemies[i].dim = (size){ 8, 16 };
					} else {
						ac->anim.s = (size){2,1};
						enemies[i].dim = (size){ 16, 8 };
					}
				}
				ac->duration = pgm_read_word(frameDurations + ac->anim.frameDurationsIndex + ac->frame);
				DKMapSprite(enemies[i].spriteIndex, ac->anim.frames+pgm_read_byte(
						frameSequences+ac->anim.frameSequenceIndex+ac->frame)*(ac->anim.s.w*ac->anim.s.h+2), enemies[i].spriteFlags);
			}
		}
	}
}


void DKKongAI(void) {
	if (dk.counter == 0) {
		switch (dk.state) {
			case DK_STATE_PICKUP:
				DKSetKongState(DK_STATE_DECIDE);
				break;
			case DK_STATE_ROLL:
				if (level == 0) {
					actor *a = 0;
					DKFindIdleEnemy(&a);
					if (a) {
						DKSetKongState(DK_STATE_PICKUP);
						break;
					}
				}
				DKSetKongState(DK_STATE_TAUNT);
				break;
			case DK_STATE_DECIDE:
			{
				actor *a = 0;
				u8 index = DKFindIdleEnemy(&a);

				if (a) {
					if (index == 0) {
						DKSetKongState(DK_STATE_PICKUP);
						DKSpawnEnemy(0,0,PGM_IGNITER_ENEMY_INDEX,ACTOR_IGNITER);
					} else {
						DKSetKongState(DK_STATE_ROLL);
						DKSpawnEnemy(0,0,PGM_BARREL_ENEMY_INDEX,ACTOR_BARREL);
					}
				}
				break;
			}
			case DK_STATE_IDLE:
				DKSetKongState(DK_STATE_TAUNT);
				break;
			case DK_STATE_TAUNT:
			{
				if (level == 0) {
					actor *a = 0;
					DKFindIdleEnemy(&a);

					if (a)
						DKSetKongState(DK_STATE_PICKUP);
				} else {
					DKSetKongState(DK_STATE_IDLE);
				}
				break;
			}
		}
	} else {
		--dk.counter;
	}
}


void DKSetKongState(u8 state) {
	u8 bgIndex = (level == 3)?2:0, animIndex = 0;
	dk.state = state;

	switch (state) {
		case DK_STATE_PICKUP:
			animIndex = 9;
			dk.counter = 30;
			break;
		case DK_STATE_ROLL:
			animIndex = 10;
			dk.counter = 30;
			break;
		case DK_STATE_DECIDE:
			animIndex = 14;
			dk.counter = 30;
			break;
		case DK_STATE_IDLE:
			animIndex = 23;
			dk.counter = 150;
			break;
		case DK_STATE_TAUNT:
		default:
			animIndex = 0;
			dk.counter = 90;
			break;
	}
	DKSetBgAnimationFrames(bgIndex, animIndex);
}


void DKDrawActors(actor *a, u8 count) {
	for (u8 i = 0; i < count; ++i) {
		if ((a[i].type&ACTOR_IDLE) == 0) {
			if (a[i].type == ACTOR_PLAYER && a[i].state == STATE_SLEDGING) {
				u8 offsetX = (a[i].dir == DIR_RIGHT)?8:16;

				if (a[i].ac.frame == 0)
					MoveSprite(a[i].spriteIndex, a[i].loc.x-offsetX, a[i].loc.y-8, a[i].ac.anim.s.w, a[i].ac.anim.s.h);
				else
					MoveSprite(a[i].spriteIndex, a[i].loc.x-8, a[i].loc.y-16, a[i].ac.anim.s.w, a[i].ac.anim.s.h);
			} else {
				if (mario->state == STATE_SLEDGING && ((level == 0 && a[i].spriteIndex == 6) || (level == 3 && a[i].spriteIndex == 4))) {
					if ((gameTicks&3) < 2) {
						if ((a[i].ac.anim.s.w*a[i].ac.anim.s.h) > 2)
							DKHideSprite(a[i].spriteIndex+2,2,1);
						DKMapSprite(mario->spriteIndex,mario->ac.anim.frames+pgm_read_byte(
								frameSequences+mario->ac.anim.frameSequenceIndex+mario->ac.frame)*(mario->ac.anim.s.w*mario->ac.anim.s.h+2), mario->spriteFlags);
						continue;
					} else {
						DKMapSprite(a[i].spriteIndex,a[i].ac.anim.frames+pgm_read_byte(
								frameSequences+a[i].ac.anim.frameSequenceIndex+a[i].ac.frame)*(a[i].ac.anim.s.w*a[i].ac.anim.s.h+2), a[i].spriteFlags);
					}
				}
				MoveSprite(a[i].spriteIndex, a[i].loc.x-(a[i].ac.anim.s.w<<2), a[i].loc.y-(a[i].ac.anim.s.h<<2), a[i].ac.anim.s.w, a[i].ac.anim.s.h);
			}
		}
	}
}


void DKAnimatePlayer(void) {
	u8 map = 0;

	switch (mario->state) {
		case STATE_NORMAL:
			mario->spriteFlags = (mario->dir == DIR_RIGHT)?0:SPRITE_FLIP_X;

			if (ACTOR_MOVE_RDY(mario->moveMod) && mario->vx == 0 && (mario->ac.frame || mario->ac.counter)) {
				mario->ac.frame = 0;
				mario->ac.counter = 0;
				map = 1;
			} else {
				mario->ac.counter += ABS(mario->vx);
			}
			break;
		case STATE_CLIMBING:
			mario->ac.counter -= mario->vy;

			if (mario->ac.counter >= mario->ac.duration || mario->ac.counter < 0)
				SWAP_SPRITE_FLAGS(mario->spriteFlags);
			break;
		case STATE_CLIMB_FIN:
			mario->ac.counter -= mario->vy;
			break;
		case STATE_SLEDGING:
			mario->spriteFlags = (mario->dir == DIR_RIGHT)?0:SPRITE_FLIP_X;
			++mario->ac.counter;

			if (mario->ac.counter >= mario->ac.duration) {
				if (mario->ac.frame == 0) {
					mario->ac.anim.s = (size){2,3};
					mario->dim = (size){ 16, 24 };
				} else {
					mario->ac.anim.s = (size){3,2};
					mario->dim = (size){ 24, 16 };
				}
			}
			break;
		case STATE_JUMPING:
			//++mario->ac.counter;
			break;
		case STATE_DYING:
			++mario->ac.counter;

			if (mario->ac.counter >= mario->ac.duration && mario->ac.frame == (mario->ac.anim.frameCount-1)) {
				DKSetState(STATE_NORMAL, mario);
				return;
			}
			break;
	}
	if (mario->ac.counter >= mario->ac.duration) {
		if (++mario->ac.frame >= mario->ac.anim.frameCount)
			mario->ac.frame = 0;
		mario->ac.duration = pgm_read_word(frameDurations + mario->ac.anim.frameDurationsIndex + mario->ac.frame);
		mario->ac.counter = 0;
		map = 1;
	} else if (mario->ac.counter < 0) {
		if (--mario->ac.frame < 0)
			mario->ac.frame = mario->ac.anim.frameCount-1;
		mario->ac.duration = pgm_read_word(frameDurations + mario->ac.anim.frameDurationsIndex + mario->ac.frame);
		mario->ac.counter = mario->ac.duration-1;
		map = 1;
	}

	if (map)
		DKMapSprite(mario->spriteIndex, mario->ac.anim.frames+pgm_read_byte(
				frameSequences+mario->ac.anim.frameSequenceIndex+mario->ac.frame)*(mario->ac.anim.s.w*mario->ac.anim.s.h+2), mario->spriteFlags);
}


void DKLoadActorAnimation(actor *a) {
	u8 index = 0;

	switch (a->type) {
		case ACTOR_PLAYER:
			DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);

			switch (a->state) {
				case STATE_CLIMBING:
					index = 6;
					break;
				case STATE_CLIMB_FIN:
					index = 7;
					break;
				case STATE_FACE_WALL:
					index = 8;
					break;
				case STATE_SLEDGING:
					index = 16;
					a->spriteFlags = (a->dir == DIR_RIGHT)?0:SPRITE_FLIP_X;
					break;
				case STATE_JUMPING:
					index = 19;
					a->spriteFlags = (a->dir == DIR_RIGHT)?0:SPRITE_FLIP_X;
					break;
				case STATE_DYING:
					DKHideSprite(0,MAX_SPRITES,1);
					index = 18;
					break;
				case STATE_NORMAL:
				default:
					index = 5;
					a->spriteFlags = (a->dir == DIR_RIGHT)?0:SPRITE_FLIP_X;
					break;
			}
			break;
		case ACTOR_BARREL:
		case ACTOR_IGNITER:
			switch (a->state) {
				case STATE_FALLING:
					return;
				case STATE_DROPPING:
					index = BARREL_ANIM_INDEX+2;
					break;
				case STATE_EXPLODING:
					DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);
					index = 17;
					break;
				case STATE_ROLLING:
				default:
					DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);

					if (a->dir == DIR_RIGHT)
						index = BARREL_ANIM_INDEX;
					else
						index = BARREL_ANIM_INDEX+1;
					break;
			}
			break;
		case ACTOR_SPARKY:
			index = SPARKY_ANIM_INDEX;
			a->spriteFlags = (a->dir == DIR_RIGHT)?0:SPRITE_FLIP_X;
			break;
		case ACTOR_SPRING:
			index = SPRING_ANIM_INDEX;
			break;
		case ACTOR_PIE:
			if (a->state == STATE_EXPLODING) {
				DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);
				index = 17;
			} else {
				index = PIE_ANIM_INDEX;
			}
			break;
		case ACTOR_GHOST:
			if (a->state == STATE_EXPLODING) {
				DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);
				index = 17;
			} else if (a->state == STATE_BLUE) {
				index = GHOST_ANIM_INDEX+1;
			} else {
				index = GHOST_ANIM_INDEX;
			}
			a->spriteFlags = (a->dir == DIR_RIGHT)?0:SPRITE_FLIP_X;
			break;
		default:
			return;
	}
	memcpy_P(&a->ac.anim,animations+index,sizeof(a->ac.anim));
	a->ac.frame = 0;
	a->ac.counter = 0;
	a->ac.duration = pgm_read_word(frameDurations + a->ac.anim.frameDurationsIndex);
	DKMapSprite(a->spriteIndex, a->ac.anim.frames+pgm_read_byte(
			frameSequences+a->ac.anim.frameSequenceIndex)*(a->ac.anim.s.w*a->ac.anim.s.h+2), a->spriteFlags);
}


void DKSetActorIdle(actor *a) {
	if (a->type == ACTOR_IDLE)
		return;
	a->type = ACTOR_IDLE;

	if (level == 1 || level == 2) {
		nextFreeSprite += 2; // Pies & spring
		return;
	}
	DKHideSprite(4,MAX_SPRITES-4,1);
	nextFreeSprite = MAX_SPRITES;

	for (u8 i = 0; i < MAX_ENEMIES; ++i) {
		if (enemies[i].type != ACTOR_IDLE) {
			nextFreeSprite -= (enemies[i].type == ACTOR_SPARKY || enemies[i].type == ACTOR_GHOST)?4:2;
			enemies[i].spriteIndex = nextFreeSprite;
			DKMapSprite(enemies[i].spriteIndex, enemies[i].ac.anim.frames+pgm_read_byte(
				frameSequences+enemies[i].ac.anim.frameSequenceIndex+enemies[i].ac.frame)*
				(enemies[i].ac.anim.s.w*enemies[i].ac.anim.s.h+2), enemies[i].spriteFlags);
		}
	}
}


void DKSetState(u8 state, actor *a) {
	switch (a->type) {
		case ACTOR_PLAYER:
			if (a->state != state) {
				if (a->state == STATE_SLEDGING) {
					sledgeTimer = 1;
					a->dim = (size){ 10, 16 };
					DKResumeSong();
				} else if (a->state == STATE_DYING) {
					DKSetGameState((extraLives)?STATE_PLAYER_DEAD:STATE_GAME_OVER);
					break;
				}

				if (state&STATE_CLIMB_ALL) {
					a->loc.x += ((a->loc.x&7) > TILE_WID2)?TILE_WIDTH-(a->loc.x&7):-(a->loc.x&7);

					if (state&(STATE_CLIMBING|STATE_CLIMB_FIN))
						sfxCounter = SFX_CLIMBING_COUNT;
				}
				if (state == STATE_SLEDGING) {
					sledgeTimer = 8*HZ;
					a->dim = (size){ 24, 16 };

					if (level == 0 || level == 3)
						DKHideSprite(4,2,2);
					StartSong(HammerSong);
				} else if (state == STATE_JUMPING) {
					jumpCounter = 0;
					jumpBonusFlag = 0;
					TriggerFx(SFX_JUMP,SFX_JUMP_VOL,true);
				} else if (state == STATE_DYING) {
					StartSong(DeathSong);
				} else if (state == STATE_NORMAL) {
					sfxCounter = SFX_WALKING_COUNT;
				}

				if (jumpBonusFlag && a->state == STATE_JUMPING && state != STATE_DYING) {
					DKDisplayBonus(bonusLoc.x,bonusLoc.y,BONUS_BARREL_JUMP);
					DKAddToScore(BONUS_BARREL_JUMP);
					jumpBonusFlag = 0;
				}

				a->state = state;
				DKLoadActorAnimation(a);
			}
			break;
		case ACTOR_BARREL:
			if (a->state != state) {
				if (state == STATE_DROPPING) {
					byteDictionary dict;

					for (u8 i = 0; i < LADDER_COUNT; ++i) {
						memcpy_P(&dict,ladderLinks+i,sizeof(dict));

						if (a->moveCmd == dict.key) {
							a->moveCmd = dict.data;
							break;
						}
					}
				} else if (state == STATE_EXPLODING) {
					++pauseCounter;
				} else if (state == STATE_DESTROYED) {
					DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);
					DKSetActorIdle(a); //a->type = ACTOR_IDLE;
					--pauseCounter;
					break;
				}

				if (a->state == STATE_FALLING && a->loc.y > (mario->loc.y+16)) {
					byteDictionary dict;

					for (u8 i = 0; i < EXIT_COUNT; ++i) {
						memcpy_P(&dict,exitLinks+i,sizeof(dict));

						if (a->moveCmd == dict.key) {
							a->moveCmd = dict.data;
							memcpy_P(&aiPath[a-enemies],barrelPaths+a->moveCmd,sizeof(aiPath[0]));
							a->state = aiPath[a-enemies].state;
							break;
						}
					}
					break;
				} else if (a->state == STATE_FALLING && state == STATE_ROLLING) {
					a->dir *= -1;
				}
				a->state = state;
				DKLoadActorAnimation(a);
			}
			break;
		case ACTOR_IGNITER:
			a->state = state;

			if (state == STATE_DROPPING)
				DKLoadActorAnimation(a);
			break;
		case ACTOR_SPARKY:
			a->state = state;
			DKLoadActorAnimation(a);
			break;
		case ACTOR_SPRING:
			if (state == STATE_DESPAWN) {
				DKSetActorIdle(a);
				DKSpawnEnemy(0,0,PGM_SPRING_ENEMY_INDEX,ACTOR_SPRING);
				break;
			} else if (state&STATE_BOUNCING) {
				TriggerFx(SFX_SPRING_BOUNCE,SFX_SPRING_BOUNCE_VOL,true);
			} else if (state&STATE_FREEFALLING) {
				TriggerFx(SFX_SPRING_DROP,SFX_SPRING_DROP_VOL,true);
			}

			a->state = state&~(STATE_BOUNCING|STATE_FREEFALLING);
			break;
		case ACTOR_PIE:
			if (state&(STATE_BURNED|STATE_DROPPED)) {
				DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);
				DKSetActorIdle(a); //a->type = ACTOR_IDLE;
				break;
			} else if (state == STATE_EXPLODING) {
				++pauseCounter;
			} else if (state == STATE_DESTROYED) {
				DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);
				DKSetActorIdle(a); //a->type = ACTOR_IDLE;
				--pauseCounter;
				break;
			}
			a->state = state;
			DKLoadActorAnimation(a);
			break;
		case ACTOR_GHOST:
			if (state == STATE_EXPLODING) {
				++pauseCounter;
			} else if (state == STATE_DESTROYED) {
				DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);
				DKSetActorIdle(a); //a->type = ACTOR_IDLE;
				--pauseCounter;
				break;
			}
			a->state = state;
			DKLoadActorAnimation(a);
			break;
	}
}

void DKDisplayBonus(u8 x, u8 y, u16 val) {
	//const char* map = 0; // Stack overflow

	DKClearBonus();
	bonusIndex = x+y*VRAM_TILES_H;
	vramBuf[0] = vram[bonusIndex];
	vramBuf[1] = vram[bonusIndex+1];

	if (val == BONUS_BARREL_DESTROY) {
		DrawMap2(x,y,map500);
		TriggerFx(SFX_DESTROY,SFX_DESTROY_VOL,true);
		//map = map500;
	} else if (val == BONUS_PURSE) {
		DrawMap2(x,y,map600);
		TriggerFx(SFX_BONUS,SFX_BONUS_VOL,true);
		//map = map600;
	} else if (val == BONUS_UMBRELLA) {
		DrawMap2(x,y,map800);
		TriggerFx(SFX_BONUS,SFX_BONUS_VOL,true);
		//map = map800;
	} else {
		DrawMap2(x,y,map100);
		TriggerFx(SFX_BONUS,SFX_BONUS_VOL,true);
	}
		//map = map100;
	//DrawMap2(x,y,map);
	bonusTimer = HZ;
}


void DKDetectActorCollisions(void) {
	rect pr,er,sr; // Player, enemy & sledgehammer bounding boxes
	actor *a = enemies;

	if (mario->state == STATE_SLEDGING && !mario->ac.frame) {
		if (mario->dir == DIR_RIGHT) {
			pr.left = mario->loc.x-8;
			pr.right = pr.left+24;
		} else {
			pr.left = mario->loc.x-16;
			pr.right = pr.left+24;
		}
	} else {
		pr.left = mario->loc.x-(mario->dim.w>>1);
		pr.right = pr.left+mario->dim.w;
	}

	if (mario->state == STATE_SLEDGING && mario->ac.frame)
		pr.top = mario->loc.y-16;
	else
		pr.top = mario->loc.y-(mario->dim.h>>1);
	pr.btm = mario->loc.y+(mario->dim.h>>1);

	for (u8 i = 0; i < MAX_ENEMIES; ++i,++a) {
		if (a->type&ACTOR_IDLE)
			continue;
		er.left = a->loc.x-(a->dim.w>>1);
		er.right = er.left+a->dim.w;
		er.top = a->loc.y-(a->dim.h>>1);
		er.btm = er.top+a->dim.h;

		if (DKRectsIntersect(&pr,&er)) {
			if (a->type == ACTOR_BARREL || a->type == ACTOR_PIE || a->type == ACTOR_GHOST) {
				if (mario->state == STATE_SLEDGING) {
					sr = pr;

					if (!mario->ac.frame) {
						if (mario->dir == DIR_RIGHT)
							sr.left = sr.right-TILE_WIDTH;
						else
							sr.right = sr.left+TILE_WIDTH;
					} else {
						sr.btm = sr.top+TILE_HEIGHT;
					}

					// Check for collision with sledgehammer
					if (DKRectsIntersect(&sr,&er)) {
						DKSetState(STATE_EXPLODING, a);
						DKDisplayBonus(a->loc.x>>3,(a->loc.y>>3)-1,BONUS_BARREL_DESTROY);
						DKAddToScore(BONUS_BARREL_DESTROY);
						continue;
					}
				}
				DKSetState(STATE_DYING, mario);
				break;
			} else {
				DKSetState(STATE_DYING, mario);
				break;
			}
		} else if ((a->type == ACTOR_BARREL || a->type == ACTOR_PIE) && mario->state == STATE_JUMPING) {
			if (!jumpBonusFlag && mario->loc.y < a->loc.y && ABS(mario->loc.x-a->loc.x) < 4 && ABS(mario->loc.y-a->loc.y) <= 16) {
				bonusLoc.x = mario->loc.x>>3;
				bonusLoc.y = mario->loc.y>>3;
				jumpBonusFlag = 1;
			}
		}
	}
}


void DKSetBgAnimationFrames(u8 bgIndex, u8 animIndex) {
	memcpy_P(&animBgs.acs[bgIndex].anim,animations+animIndex,sizeof(animation));
	animBgs.acs[bgIndex].frame = 0;
	animBgs.acs[bgIndex].counter = 0;
	animBgs.acs[bgIndex].duration = pgm_read_word(frameDurations + animBgs.acs[bgIndex].anim.frameDurationsIndex);
	DKDrawAnimatedBgs(bgIndex, 1);
}


#define BGO_INDEX_OFFSET 	5	// Byte offset of bgoIndex in bgDirectory
#define AC_INDEX_OFFSET		8	// Byte offset of animCount in bgDirectory
#define AI_INDEX_OFFSET		9	// Byte offset of animIndex in bgDirectory
#define BGI_INDEX_OFFSET	2	// Byte offset of bgInner index in bgOuter

void DKLoadAnimatedBgs(void) {
	u8 animIndex;
	char *byteGrab = (char*)bgDir;
	u16 bgoIndex,bgiIndex;
	bgAnimIndex id;

	animBgs.count = pgm_read_byte(byteGrab+((level*sizeof(bgDirectory))+AC_INDEX_OFFSET));

	if (animBgs.count) {
		animIndex = pgm_read_byte(byteGrab+((level*sizeof(bgDirectory))+AI_INDEX_OFFSET));
		bgoIndex = pgm_read_word(byteGrab+((level*sizeof(bgDirectory))+BGO_INDEX_OFFSET));

		for (u8 i = 0; i < animBgs.count; i++) {
			memcpy_P(&id,bgAnimDir+animIndex+i,sizeof(bgAnimIndex));
			byteGrab = (char*)(bgoTbl+bgoIndex+id.iOuter);
			bgiIndex = pgm_read_word(byteGrab+BGI_INDEX_OFFSET);
			memcpy_P(&animBgs.bgs[i],bgiTbl+bgiIndex+id.iInner,sizeof(bgInner));
			DKSetBgAnimationFrames(i, animBgs.bgs[i].tile);
		}
	}
}

void DKDrawAnimatedBgs(u8 from, u8 count) {
	for (u8 i = from, to = from+count; i < to; ++i) {
		if (animBgs.bgs[i].type&BGAO)
			continue;
		if (animBgs.acs[i].counter == 0)
			DKFillMap(&animBgs.bgs[i].r,animBgs.acs[i].anim.frames+pgm_read_byte(
				frameSequences+animBgs.acs[i].anim.frameSequenceIndex+animBgs.acs[i].frame)*(animBgs.acs[i].anim.s.w*animBgs.acs[i].anim.s.h+2));
	}
}

void DKAnimateBgs(animationController *acs, u8 count) {
	for (u8 i = 0; i < count; ++i,++acs) {
		if (++acs->counter >= acs->duration) {
			acs->counter = 0;

			if (++acs->frame >=acs->anim.frameCount)
				acs->frame = 0;
			acs->duration = pgm_read_word(frameDurations + acs->anim.frameDurationsIndex + acs->frame);
		}
	}
}


void DKFill(const rect *r, u8 tileId) {
	u8 x,y;

	for (y = r->top; y < r->btm; y+=TILE_WIDTH) {
    	for (x = r->left; x < r->right; x+=TILE_HEIGHT)
            SetTile(x>>3,y>>3,tileId);
    }
}


void DKFillMap(const rect *r, const char *map) {
	u8 mapWid = pgm_read_byte(&(map[0]));
	u16 mapSize = mapWid*pgm_read_byte(&(map[1]));

	for (u8 y = r->top>>3,yLim = r->btm>>3, yMap = 0; y < yLim; y++,yMap+=mapWid) {
		if (yMap == mapSize)
			yMap = 0;
    	for (u8 x = r->left>>3,xLim = r->right>>3, xMap = 0; x < xLim; x++,xMap++) {
			if (xMap == mapWid)
				xMap = 0;
            SetTile(x,y,pgm_read_byte(map+xMap+yMap+2));
		}
    }
}


void DKDrawLevel(void) {
	bgOuter bgo;
	bgInner bgi,bgm;
	bgDirectory bgd;

	DKClearVram();
	SetTile(SCORE_LOC_X,SCORE_LOC_Y,(gameState <= STATE_LADDER)?TILE_1UP1:TILE_1UP);
	DrawMap2(HISCORE_LOC_X,HISCORE_LOC_Y,(gameState <= STATE_LADDER)?mapHiScore1:mapHiScore);

	if (gameState >= STATE_PLAYING) {
		DrawMap2(23,2,mapLevelEquals);
		DrawMap2(23,3,mapBonusFrame);
	}

	DKLoadAnimatedBgs();
	DKDrawAnimatedBgs(0, animBgs.count);
	memcpy_P(&bgd,bgDir+level,sizeof(bgDirectory));

	for (u8 i = 0; i < bgd.bgoCount; i++) {
		memcpy_P(&bgo,bgoTbl+bgd.bgoIndex+i,sizeof(bgOuter));

		if ((bgo.type&(BGI|BGT)) == 0) {
			for (u8 j = 0; j < bgo.count; j++) {
				memcpy_P(&bgi,bgiTbl+bgo.index+j,sizeof(bgInner));

				if (bgi.type&BGM) {
					++j;
					memcpy_P(&bgm,bgiTbl+bgo.index+j,sizeof(bgInner));
				}

				if ((bgi.type&(BGA|BGAO)) == 0) {
					if (bgi.type&BGP)
						DKFillMap(&bgi.r,(const char*)pgm_read_word(bgMaps + bgi.tile));
					else
						DKFill(&bgi.r,bgi.tile);
				}
			}
		}
	}
}


void DKMapSprite(u8 index, const char *map, u8 spriteFlags) {
	u8 x,y,xStart,xEnd,xStep;
	u8 wid = pgm_read_byte(map), hgt = pgm_read_byte(map+1);

	if (spriteFlags&SPRITE_FLIP_X) {
		xStart = wid-1;
		xEnd = 255;
		xStep = -1;
	} else {
		xStart = 0;
		xEnd = wid;
		xStep = 1;
	}

	for (y = 0; y < hgt; y++) {
		for (x = xStart; x < xEnd; x += xStep,index++) {
			sprites[index].tileIndex = pgm_read_byte(&(map[(y*wid)+x+2]));
			sprites[index].flags = spriteFlags;
		}
	}
}


void DKSetVelocityX(actor *a, char vx) {
	a->vx = vx;

	if (vx)
		a->dir = (vx < 0)?DIR_LEFT:DIR_RIGHT;
}

#if 0 // Cheaper to repeat code in DKMoveActors
void DKApplyEnemyVelocity(actor *a, u8 i, u8 delta1, u8 delta2) {
	if (a->loc.x != aiPath[i-1].loc.x) {
		a->vx = ((a->loc.x > aiPath[i-1].loc.x)?-1:1)*((gameTicks&a->moveMod)?delta1:delta2);
		a->loc.x += a->vx;

		if ((a->vx > 0 && a->loc.x > aiPath[i-1].loc.x) || (a->vx < 0 && a->loc.x < aiPath[i-1].loc.x))
			a->loc.x = aiPath[i-1].loc.x;
	}

	if (a->loc.y != aiPath[i-1].loc.y) {
		a->vy += ((a->loc.y > aiPath[i-1].loc.y)?-1:1)*((gameTicks&a->moveMod)?1:2);

		if (a->type == ACTOR_BARREL || a->type == ACTOR_IGNITER) {
			if (a->state&STATE_DROPPING)
				a->vy = 1;
			else if (ABS(a->vy > 3))
				a->vy = 3*NORMALIZE(a->vy);
		} else if (a->type == ACTOR_SPARKY) {
			if (a->state&STATE_SPAWNING)
				a->vy <<= 1;
		}

		a->loc.y += a->vy;

		if ((a->vy > 0 && a->loc.y > aiPath[i-1].loc.y) || (a->vy < 0 && a->loc.y < aiPath[i-1].loc.y))
			a->loc.y = aiPath[i-1].loc.y;
	}
}
#endif

void DKMoveActors(actor *actors, u8 count) {
	actor *a = actors;

	for (u8 i = 0; i < count; ++i,++a) {
		if (a->type&ACTOR_IDLE)
			continue;

		switch (a->type) {
			case ACTOR_PLAYER:
			{
				char prevDir = a->dir;

				if (!ACTOR_MOVE_RDY(a->moveMod))
					break;
				if (a->moveCmd&(BTN_LEFT|BTN_RIGHT)) {
					a->dir = (a->moveCmd&BTN_LEFT)?-1:1;
					a->vx = a->dir;
				}
				if (a->dir != prevDir)
					DKLoadActorAnimation(a);

				if (a->state == STATE_JUMPING) {
					if (++jumpCounter < 5) {
						a->vy = -1;
					} else if (jumpCounter < 11) {
						a->vy = 0;
					} else if (jumpCounter < 17) {
						a->vy = 1;
					} else if (jumpCounter < 20) {
						a->vy = 2;
					} else {
						DKSetState(STATE_NORMAL, a);
					}
				} else if ((a->state&(STATE_SLEDGING|STATE_CLIMB_ALL)) == 0 && a->vy == 0 && (a->moveCmd&(BTN_A|BTN_B))) {
					DKSetState(STATE_JUMPING, a);
					a->vy = -7;
				} else {
					DKSetVelocityX(a, 0);

					if ((a->moveCmd&(BTN_LEFT|BTN_RIGHT)) && a->state != STATE_CLIMBING && a->vy == 0)
						a->vx = a->dir;
					if (isNearLadder) {
						if ((a->moveCmd&BTN_UP) && (a->state&STATE_CLIMB_ALL)) {
							a->vy = -1;
						} else if ((a->moveCmd&BTN_DOWN) && (a->state&STATE_CLIMB_ALL)) {
							a->vy = 1;
						} else if ((a->state&STATE_CLIMB_ALL) == 0) {
							if (a->vy > 0) {
								if (gameTicks&1)
									a->vy += ((gameTicks&3)>>1);
								if (a->vy > 5)
									a->vy = 5;
								if (a->vy == 0)
									a->vy = 1;
							} else {
								a->vy = 2;
							}
						} else {
							a->vy = 0;
						}
					} else {
						if (a->vy > 0) {
							if (gameTicks&1)
								a->vy += ((gameTicks&3)>>1);
							if (a->vy > 5)
								a->vy = 5;
							if (a->vy == 0)
								a->vy = 1;
						} else {
							a->vy = 2;
						}
					}
				}

				if ((a->state&STATE_FACE_WALL) && a->vx)
					DKSetState(STATE_NORMAL, a);
				DKDetectCollisions(mario);

				if (a->state&STATE_CLIMB_ALL)
					DKSetVelocityX(a, 0);
				if (a->state == STATE_JUMPING) {
					if (gameTicks&1)
						a->loc.x += a->vx;
				} else {
					a->loc.x += a->vx;
				}

				if (a->loc.x < a->dim.w)
					a->loc.x = a->dim.w;
				else if (a->loc.x > (SCRN_WID-a->dim.w))
					a->loc.x = SCRN_WID-a->dim.w;
				a->loc.y += a->vy;

				if (a->state == STATE_NORMAL && a->vx && --sfxCounter == 0) {
					sfxCounter = SFX_WALKING_COUNT;
					TriggerFx(SFX_WALK0+(PRNG_NEXT()&3),SFX_WALK_VOL,true);
				} else if ((a->state&(STATE_CLIMBING|STATE_CLIMB_FIN)) && a->vy && --sfxCounter == 0) {
					sfxCounter = SFX_CLIMBING_COUNT;
					TriggerFx(SFX_WALK0+(PRNG_NEXT()&3),SFX_WALK_VOL,true);
				}
				break;
			}
			case ACTOR_BARREL:
			case ACTOR_IGNITER:
				if (a->state == STATE_EXPLODING)
					break;
				if (a->loc.x == aiPath[i-1].loc.x && a->loc.y == aiPath[i-1].loc.y) {
					DKSetVelocityX(a, 0);
					a->vy = 0;

					if (a->state == STATE_DROPPING) {
						DKSetState(aiPath[i-1].state, a);
					} else if (aiPath[i-1].state == STATE_DROPPING && a->loc.y < (mario->loc.y+16)) {
						if (PRNG_NEXT()&1) {
							DKSetState(STATE_DROPPING, a);
							memcpy_P(&aiPath[i-1],barrelPaths+a->moveCmd,sizeof(aiPath[i-1]));
						}
					}

					if (a->state != STATE_DROPPING) {
						++a->moveCmd;
						memcpy_P(&aiPath[i-1],barrelPaths+a->moveCmd,sizeof(aiPath[i-1]));

						if (aiPath[i-1].state == 0) {
							DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);

							if (a->type == ACTOR_IGNITER) {
								DKSetActorIdle(a); //a->type = ACTOR_IDLE;
								animBgs.bgs[1].type = BGA;
								animBgs.bgs[2].type = BGA;
								DKSpawnEnemy(0,0,PGM_SPARKY0_ENEMY_INDEX,ACTOR_SPARKY);
							} else {
								DKSetActorIdle(a); //a->type = ACTOR_IDLE;
							}
							break;
						} else if (aiPath[i-1].state != STATE_DROPPING) {
							DKSetState(aiPath[i-1].state, a);
						}
					}
				}

				if (a->loc.x != aiPath[i-1].loc.x) {
					a->vx = ((a->loc.x > aiPath[i-1].loc.x)?-1:1)*((gameTicks&a->moveMod)?1:2);
					a->loc.x += a->vx;

					if ((a->vx > 0 && a->loc.x > aiPath[i-1].loc.x) || (a->vx < 0 && a->loc.x < aiPath[i-1].loc.x))
						a->loc.x = aiPath[i-1].loc.x;
				}

				if (a->loc.y != aiPath[i-1].loc.y) {
					a->vy += ((a->loc.y > aiPath[i-1].loc.y)?-1:1)*((gameTicks&a->moveMod)?1:2);

					if (a->state&STATE_DROPPING)
						a->vy = 1;
					else if (ABS(a->vy > 3))
						a->vy = 3*NORMALIZE(a->vy);
					a->loc.y += a->vy;

					if ((a->vy > 0 && a->loc.y > aiPath[i-1].loc.y) || (a->vy < 0 && a->loc.y < aiPath[i-1].loc.y))
						a->loc.y = aiPath[i-1].loc.y;
				}

				break;
			case ACTOR_SPARKY:
				if (!ACTOR_MOVE_RDY(a->moveMod))
					break;
				if (a->loc.x == aiPath[i-1].loc.x && a->loc.y == aiPath[i-1].loc.y) {
					DKSetVelocityX(a, 0);
					a->vy = 0;
					++a->moveCmd;

					if (a->state&STATE_JUNCTURE) {
						memcpy_P(&aiPath[i-1],sparkyPaths+a->moveCmd,sizeof(aiPath[i-1]));
						PRNG_NEXT();

						if (prng < 85)
							a->moveCmd = aiPath[i-1].state;
						else if (prng < 170)
							a->moveCmd = aiPath[i-1].loc.x;
						else
							a->moveCmd = aiPath[i-1].loc.y;
					}
					memcpy_P(&aiPath[i-1],sparkyPaths+a->moveCmd,sizeof(aiPath[i-1]));

					if (a->loc.x < aiPath[i-1].loc.x)
						a->dir = DIR_RIGHT;
					else if (a->loc.x > aiPath[i-1].loc.x)
						a->dir = DIR_LEFT;
					DKSetState(aiPath[i-1].state, a);
				}
				if (a->loc.x != aiPath[i-1].loc.x) {
					a->vx = (a->loc.x > aiPath[i-1].loc.x)?-1:1;
					a->loc.x += a->vx;

					if ((a->vx > 0 && a->loc.x > aiPath[i-1].loc.x) || (a->vx < 0 && a->loc.x < aiPath[i-1].loc.x))
						a->loc.x = aiPath[i-1].loc.x;
				}

				if (a->loc.y != aiPath[i-1].loc.y) {
					a->vy = (a->loc.y > aiPath[i-1].loc.y)?-1:1;

					if (a->state&STATE_SPAWNING)
						a->vy <<= 1;
					a->loc.y += a->vy;

					if ((a->vy > 0 && a->loc.y > aiPath[i-1].loc.y) || (a->vy < 0 && a->loc.y < aiPath[i-1].loc.y))
						a->loc.y = aiPath[i-1].loc.y;
				}
				break;
			case ACTOR_SPRING:
				if (!ACTOR_MOVE_RDY(a->moveMod))
					break;
				if (a->loc.x == aiPath[i-1].loc.x && a->loc.y == aiPath[i-1].loc.y) {
					++a->moveCmd;

					if (a->state&STATE_JUNCTURE) {
						memcpy_P(&aiPath[i-1],springPaths+a->moveCmd,sizeof(aiPath[i-1]));
						a->moveCmd = aiPath[i-1].state;
					}
					memcpy_P(&aiPath[i-1],springPaths+a->moveCmd,sizeof(aiPath[i-1]));
					DKSetState(aiPath[i-1].state, a);
				}

				if (a->loc.x != aiPath[i-1].loc.x)
					a->loc.x = MIN(a->loc.x+3,aiPath[i-1].loc.x);
				if (a->loc.y < aiPath[i-1].loc.y)
					a->loc.y = MIN(a->loc.y+6,aiPath[i-1].loc.y);
				else if (a->loc.y > aiPath[i-1].loc.y)
					a->loc.y = MAX(a->loc.y-6,aiPath[i-1].loc.y);
				break;
			case ACTOR_PIE:
				if (a->state == STATE_EXPLODING)
					break;
				if (a->loc.x == 120 && a->loc.y < 96) {
					DKSetState(STATE_BURNED, a);
				} else if (a->loc.x < 9 || a->loc.x > 231) {
					DKSetState(STATE_DROPPED, a);
				} else if (a->state == STATE_FRESH && (gameTicks&1)) {
					a->loc.x += dirConveyorBelt;
				}
				break;
			case ACTOR_GHOST:
			{
				if (!ACTOR_MOVE_RDY(a->moveMod) || (a->state == STATE_EXPLODING))
					break;
				if (a->loc.x == aiPath[i-1].loc.x && a->loc.y == aiPath[i-1].loc.y) {
					u8 prevState = aiPath[i-1].state;

					++a->moveCmd;
					memcpy_P(&aiPath[i-1],ghostPaths+a->moveCmd,sizeof(aiPath[i-1]));

					if (prevState == STATE_JUNCTURE) {
						PRNG_NEXT();

						if (prng < 85)
							a->moveCmd = aiPath[i-1].state;
						else if (prng < 170)
							a->moveCmd = aiPath[i-1].loc.x;
						else
							a->moveCmd = aiPath[i-1].loc.y;
					} else {
						u8 mapIndex = prevState>>3,mutIndex = prevState&7,mutVal = mutmap[mapIndex]&(1<<mutIndex);

						PRNG_NEXT();

						if (mutVal) {
							if (prng < 85)
								a->moveCmd = aiPath[i-1].state;
							else if (prng < 170)
								a->moveCmd = aiPath[i-1].loc.x;
							else
								a->moveCmd = aiPath[i-1].loc.y;
						} else {
							if (prng < 128)
								a->moveCmd = aiPath[i-1].loc.x;
							else
								a->moveCmd = aiPath[i-1].loc.y;
						}

					}
					memcpy_P(&aiPath[i-1],ghostPaths+a->moveCmd,sizeof(aiPath[i-1]));

					if (a->loc.x < aiPath[i-1].loc.x)
						a->dir = DIR_RIGHT;
					else if (a->loc.x > aiPath[i-1].loc.x)
						a->dir = DIR_LEFT;
					DKSetState((sledgeTimer)?STATE_BLUE:STATE_RED, a);
				}
				if (a->loc.x != aiPath[i-1].loc.x) {
					a->vx = (a->loc.x > aiPath[i-1].loc.x)?-1:1;
					a->loc.x += a->vx;

					if ((a->vx > 0 && a->loc.x > aiPath[i-1].loc.x) || (a->vx < 0 && a->loc.x < aiPath[i-1].loc.x))
						a->loc.x = aiPath[i-1].loc.x;
				}

				if (a->loc.y != aiPath[i-1].loc.y) {
					a->vy = (a->loc.y > aiPath[i-1].loc.y)?-1:1;
					a->loc.y += a->vy;

					if ((a->vy > 0 && a->loc.y > aiPath[i-1].loc.y) || (a->vy < 0 && a->loc.y < aiPath[i-1].loc.y))
						a->loc.y = aiPath[i-1].loc.y;
				}
				break;
			}
		}
	}
}


inline u8 DKRectsIntersect(const rect *r1, const rect *r2) {
	if (r1->btm <= r2->top || r1->right <= r2->left || r1->left >= r2->right || r1->top >= r2->btm)
		return 0;
	return 1;
}


u8 DKProcessMutableBgEvent(u8 evType, bgInner *bgiSrc, bgInner *bgiInfo, void *v) {
	u8 mapIndex = (bgiInfo->type)>>3,mutIndex = (bgiInfo->type)&7,mutVal = mutmap[mapIndex]&(1<<mutIndex);
	u8 switchOff = 0, retval = 0;

	switch (evType) {
		case MUT_EV_DRAW:

			break;
		case MUT_EV_ANIM:

			break;
		case MUT_EV_COLLISION:
			switch (mario->type) {
				case ACTOR_PLAYER:
				{
					bgOuter *bgo = (bgOuter*)v;

					if (bgo->count == 0)
						bgiInfo->tile = (u8)bgo->index;

					switch (bgiInfo->tile) {
						case MUT_LADDER:
						case MUT_BROKEN_LADDER:
						case MUT_LADDER_RETRACT:
						{
							if (climbFlag)
								break;
							if ((mario->loc.x-bgo->r.left) < 2 || (mario->loc.x-bgo->r.left) > 6) {
								isNearLadder = 0;
								break;
							}

							if (level == 1 && ladderRetracter != 1 && bgiInfo->tile == MUT_LADDER_RETRACT) {
								bgo->r.top += 32;
								bgiInfo->tile = MUT_BROKEN_LADDER;
							}

							char ladderPos = mario->loc.y-bgo->r.top;

							if (ladderPos < MARIO_HGT2)
								isNearLadder = LADDER_TOP;
							else
								isNearLadder = LADDER_BTM;

							if (isNearLadder == LADDER_TOP) {
								if (bgiInfo->tile == MUT_BROKEN_LADDER) {
								 	if (ladderPos == 0 && (mario->moveCmd&BTN_UP))
										mario->loc.y -= mario->vy;
								} else if (mario->state == STATE_CLIMBING) {
									DKSetState(STATE_CLIMB_FIN, mario);
								} else if (ladderPos == 0 && (mario->moveCmd&BTN_UP) && (mario->state&STATE_CLIMB_FIN)) {
									DKSetState(STATE_FACE_WALL, mario);
								} else if (mario->moveCmd&BTN_DOWN && (mario->state&STATE_CLIMB_ALL) == 0) {
									DKSetState(STATE_CLIMB_FIN, mario);
								}
							} else {
								if ((mario->moveCmd&BTN_UP) && (mario->state&STATE_CLIMB_ALL) == 0) {
									DKSetState(STATE_CLIMBING, mario);
								} else if (mario->state&STATE_CLIMB_FIN) {
									DKSetState(STATE_CLIMBING, mario);
								}
							}

							if (mario->state&STATE_CLIMB_ALL)
								climbFlag = 1;
							break;
						}
						case MUT_CONVEYOR_BELT:
							if (((mario->state&STATE_CLIMB_ALL) == 0) && (gameTicks&1))
								mario->loc.x += dirConveyorBelt;
							break;
						case MUT_SLEDGE:
							if (mutVal) {
								if (level == 3) {
									for (u8 i = 0; i < MAX_ENEMIES; ++i) {
										if (enemies[i].type == ACTOR_GHOST)
											DKSetState(STATE_BLUE, &enemies[i]);
									}
								}
								switchOff = 1;
								DKSetState(STATE_SLEDGING, mario);
							}
							break;
						case MUT_KONG:
							// Kong is 3x3 but in a 4x4 shell, so check for 3x3 collision (mario's
							// loc is the center-point of his 16x16 sprite)
							if (mario->loc.x < bgo->r.right && mario->loc.y > bgo->r.top)
								DKSetState(STATE_DYING, mario);
							break;
						case MUT_PURSE:
							if (mutVal) {
								switchOff = 1;
								DKDisplayBonus((bgiInfo->r.left+4)>>3,(bgiInfo->r.top-8)>>3,BONUS_PURSE);
								DKAddToScore(BONUS_PURSE);
							}
							break;
						case MUT_UMBRELLA:
							if (mutVal) {
								switchOff = 1;
								DKDisplayBonus(bgiInfo->r.left>>3,(bgiInfo->r.top-8)>>3,BONUS_UMBRELLA);
								DKAddToScore(800);
							}
							break;
						case MUT_FIRE:
							if (level == 0 && (animBgs.bgs[2].type&BGAO))
								break;
						case MUT_OUT_OF_BOUNDS:
							DKSetState(STATE_DYING, mario);
							break;
						case MUT_RIVET:
							bgo->r.top += 16;

							if (mutVal) {
								if ((mario->dir == DIR_RIGHT && mario->loc.x == (bgiInfo->r.left+16)) ||
										(mario->dir == DIR_LEFT && mario->loc.x == (bgiInfo->r.right-16))) {
									mutmap[mapIndex] &=~ mutVal;
									DKFill(&bgiSrc->r, (gameState <= STATE_LADDER)?TILE_BLANK1:TILE_BLANK);
									DKDisplayBonus((bgiInfo->r.left+12)>>3,bgiInfo->r.btm>>3,BONUS_RIVET);
									DKAddToScore(BONUS_RIVET);

									// [6],[8,9,11],[19,21,23],[25]
									if (!((mutmap[0]&0x40) || (mutmap[1]&0xb) || (mutmap[2]&0xa8) || (mutmap[3]&0x2))) {
										DKClearBonus();
										DKSetState(STATE_NORMAL, mario);
										DKSetGameState(STATE_PLAYER_WIN);
									}
								}
							} else if (mario->loc.x == bgiInfo->r.left+12) {
								// Allow player to fall through gap left by rivet
								retval = 1;
							}
							if (!DKRectsIntersect(&(rect){mario->loc.x+mario->vx-5,mario->loc.x+mario->vx+5,
									mario->loc.y+mario->vy-8,mario->loc.y+mario->vy+8}, &bgo->r))
								retval = 1;
							break;
						case MUT_WIN:
							mario->dir = DIR_LEFT;
							DKSetState(STATE_NORMAL, mario);
							DKSetGameState(STATE_PLAYER_WIN);
							break;
					}
					break;
				}
			}
	}

	if (switchOff) {
		mutmap[mapIndex] &=~ mutVal;
		DKFill(&bgiSrc->r, (gameState <= STATE_LADDER)?TILE_BLANK1:TILE_BLANK);
	}
	return retval;
}


void DKMoveElevators(void) {
	if ((gameTicks&DKGetMovementScale()) == 0)
		return;
	char dir;

	for (u8 i = 0; i < MAX_ELEVATORS; ++i) {
		dir = (i&1)?1:-1;
		elevators[i].top += dir;
		elevators[i].btm += dir;

		if (elevators[i].top > 200) {
			elevators[i].top = 56;
			elevators[i].btm = 64;
			continue;
		} else if (elevators[i].top < 56) {
			elevators[i].top = 199;
			elevators[i].btm = 207;
			continue;
		}
	}
}

void DKDrawElevators(void) {
	if ((gameTicks&DKGetMovementScale()) == 0)
		return;
	u8 x, y;
	char dir;
	const char *map;

	for (u8 i = 0; i < MAX_ELEVATORS; ++i) {
		if (elevators[i].top < 64 || elevators[i].top > 200)
			continue;
		dir = (i&1)?1:-1;
		map = mapRampSquare;
		x = elevators[i].left>>3;
		y = elevators[i].top>>3;

		if (elevators[i].top&7) {
			map += 3+(TILE_HEIGHT-1-(elevators[i].top&7))*4;

			if (y <= 23) {
				SetTile(x,y+1,pgm_read_byte(map+3));
				SetTile(x+1,y+1,pgm_read_byte(map+3));
			}
		} else if ((dir == DIR_UP && y <= 23) || (dir == DIR_DOWN && y >= 10)) {
			DKFill(&(rect){elevators[i].left,elevators[i].right,elevators[i].top-TILE_HEIGHT*dir,elevators[i].btm-TILE_HEIGHT*dir},
					(gameState <= STATE_LADDER)?TILE_BLANK1:TILE_BLANK);
		}

		if (y >= 9 && y <= 24) {
			SetTile(x,y,pgm_read_byte(map+2));
			SetTile(x+1,y,pgm_read_byte(map+2));
		}
	}
}


void DKBindToElevators(void) {
	if (mario->loc.y <= 48)
		return;
	rect mr = (rect){mario->loc.x-8,mario->loc.x+8,mario->loc.y-8,mario->loc.y+8};

	for (u8 i = 0; i < MAX_ELEVATORS; ++i) {
		if ((mr.right >= elevators[i].left && mr.right <= elevators[i].right) ||
				(mr.left >= elevators[i].left && mr.left <= elevators[i].right)) {
			if (((mr.btm-elevators[i].top) == 1) || ((elevators[i].top-mr.btm) == 1)) {
				mario->loc.y = elevators[i].top-8;
				break;
			}
		}
	}
}


void DKDetectCollisions(actor *a) {
	u8 colVal;
	char xDist,yDist;
	bgDirectory bgd;
	bgOuter bgo;
	u8 rStart,rFin,jumpsIt;
	rect region,rPre,rPost;

	if (a->type == ACTOR_PLAYER) {
		rStart = 10;
		rFin = 16;
	} else {
		rStart = a->dim.w;
		rFin = a->dim.h;
	}

	rPre.left = a->loc.x-(rStart>>1);
	rPre.right = rPre.left+rStart;
	rPre.top = a->loc.y-(rFin>>1);
	rPre.btm = rPre.top+rFin;
	rPost.left = rPre.left+a->vx;
	rPost.right = rPre.right+a->vx;
	rPost.top = rPre.top+a->vy;
	rPost.btm = rPre.btm+a->vy;

	memcpy_P(&bgd, bgDir+level, sizeof(bgd));

	for (u8 i = 0; i < bgd.regionsCount; ++i) {
		memcpy_P(&region,bgRegions+bgd.regionsIndex+i,sizeof(region));

		if (!DKRectsIntersect(&rPost, &region))
			continue;
		// Jump to relevant section
		jumpsIt = 0;

		while (1) {
			if (a->loc.y < pgm_read_byte(bgJumpDivisions+bgd.jumpDivIndex+jumpsIt)) {
				rStart = pgm_read_byte(bgJumpIndexes+bgd.jumpIndex+i*bgd.jumpDivCount+jumpsIt);
				break;
			}
			++jumpsIt;
		}

		rStart += bgd.bgoIndex + pgm_read_byte(bgRegionIndexes+bgd.regionsIndex+i);
		rFin = MIN(rStart+5,bgd.bgoIndex+pgm_read_byte(bgRegionIndexes+bgd.regionsIndex+i+1));

		if (level == 2)
			rFin += MAX_ELEVATORS;

		for (u8 j = rStart; j < rFin; ++j) {
			if (level == 2 && ((rFin-j) <= MAX_ELEVATORS)) {
				bgo.r = elevators[MAX_ELEVATORS-(rFin-j)];
				bgo.type = BGC;
			} else {
				memcpy_P(&bgo,bgoTbl+j,sizeof(bgo));
			}

			if ((bgo.type&(BGC|BGM)) == 0)
				continue;
			if (DKRectsIntersect(&rPost, &bgo.r)) {
				if (bgo.type&BGM) {
					bgInner bgi,bgm;

					if (bgo.count) {
						memcpy_P(&bgi,bgiTbl+bgo.index,sizeof(bgInner));
						memcpy_P(&bgm,bgiTbl+bgo.index+1,sizeof(bgInner));
					}

					if (DKProcessMutableBgEvent(MUT_EV_COLLISION,&bgi,&bgm,&bgo) || (bgo.type&BGC) == 0)
						continue;
				}

				if (a->vx == 0) {
					colVal = H_INTERSECT;
				} else if (a->vy == 0) {
					colVal = V_INTERSECT;
				} else {
					xDist = (a->vx > 0)?bgo.r.left-rPre.right:rPre.left-bgo.r.right;
					yDist = (a->vy > 0)?bgo.r.top-rPre.btm:rPre.top-bgo.r.btm;

					if (yDist < 0) {
						colVal = V_INTERSECT;
					} else if (xDist < 0) {
						colVal = H_INTERSECT;
					} else {
						colVal = ((xDist-ABS(a->vx)) < (yDist-ABS(a->vy)))?V_INTERSECT:H_INTERSECT;
					}
				}

				if (colVal&V_INTERSECT) {
					if (level == 0 && (a->type != ACTOR_PLAYER || (a->state&STATE_CLIMB_ALL) == 0)) {
						if ((bgo.type&BGSTEP) && (rPre.btm <= bgo.r.btm))
							a->vy = -1;
					}
				} else if ((colVal&H_INTERSECT) && (rPre.btm <= bgo.r.top)) {
					if (a->type == ACTOR_PLAYER && (a->state&STATE_CLIMBING) && (a->moveCmd&BTN_DOWN))
						DKSetState(STATE_FACE_WALL, a);
					if (a->type == ACTOR_BARREL && a->state == STATE_FALLING) {
						a->vy = -2;
						DKSetState(STATE_ROLLING, a);
					} else if (a->type == ACTOR_PLAYER && a->vy > 4 && (a->state&STATE_CLIMB_ALL) == 0) {
						DKSetState(STATE_DYING, a);
						a->vy -= NORMALIZE(a->vy)*(MIN(rPost.btm,bgo.r.btm)-MAX(rPost.top,bgo.r.top));
						break;
					} else if (a->type != ACTOR_PLAYER || (a->state&STATE_CLIMB_FIN) == 0) {
						if (a->state == STATE_JUMPING)
							DKSetState(STATE_NORMAL, a);
						a->vy -= NORMALIZE(a->vy)*(MIN(rPost.btm,bgo.r.btm)-MAX(rPost.top,bgo.r.top));
						rPost.top = rPre.top+a->vy;
						rPost.btm = rPre.btm+a->vy;
					}
				}
			}
		}
	}
}


void DKClearBonus(void) {
	if (bonusIndex) {
		vram[bonusIndex] = vramBuf[0];
		vram[bonusIndex+1] = vramBuf[1];
		bonusIndex = 0;
	}
}


void DKPrintBonus(void) {
	DKPrintNumber(BONUS_LOC_X+1,BONUS_LOC_Y+1,bonus,4);
}


void DKPrintExtraLives(void) {
	for (u8 i = 1; i <= MAX_EXTRA_LIVES; ++i) {
		if (i <= extraLives)
			SetTile(1+i,2,TILE_LIVES);
		else
			SetTile(1+i,2,TILE_BLANK);
	}
}


void DKPrintLevel(void) {
	DKPrintNumber(BONUS_LOC_X+2,BONUS_LOC_Y-1,dkRound,2);
}


void DKPrintScores(u8 forcePrint) {
	DKPrintNumber(SCORE_LOC_X+MAP1UP_WIDTH,SCORE_LOC_Y,score,7);

	if (forcePrint || score == hiScore)
		DKPrintNumber(HISCORE_LOC_X+MAPHISCORE_WIDTH,HISCORE_LOC_Y,hiScore,7);
}


void DKAddToScore(int val) {
	score += val;

	if (score > hiScore)
		hiScore = score;
	DKPrintScores(0);
}


void DKDisplayLadderScene(u8 lvl) {
	DKSetGameState(STATE_LADDER);
	DKClearVram();
	DKPrintScores(1);
	SetTile(SCORE_LOC_X,SCORE_LOC_Y,TILE_1UP1);
	DrawMap2(HISCORE_LOC_X,HISCORE_LOC_Y,mapHiScore1);
	DrawMap2(4,25,mapHowHigh);

	u8 limit;

	if (dkRound == 1 && lvl == 3)
		limit = 1;
	else if (dkRound == 2 && lvl > 0)
		limit = lvl-1;
	else
		limit = lvl;

	for (u8 i = 0; i <= limit; ++i) {
		DrawMap2(13,21-i*3,mapDKLadder);

		switch (i) { // Allow fall throughs
			case 3:
				DKPrintNumber(6,14,100,3);
				SetTile(10,14,TILE_METERS);
			case 2:
				DKPrintNumber(7,17,75,2);
				SetTile(10,17,TILE_METERS);
			case 1:
				DKPrintNumber(7,20,50,2);
				SetTile(10,20,TILE_METERS);
			case 0:
				DKPrintNumber(7,23,25,2);
				SetTile(10,23,TILE_METERS);
				break;
		}
	}

	u8 timer = 3*HZ;;

	while (1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();

			if (--timer == 0 || (timer < (3*HZ-20) && (ReadJoypad(0)&(BTN_A|BTN_B|BTN_START|BTN_SELECT))))
				break;
		}
	}
}


void DKInitLevel(u8 lvl) {
	DKHideSprite(0,MAX_SPRITES,1);

	if (lvl != level) {
		StartSong(StageStart);
		DKDisplayLadderScene(lvl);
	}
	if (lvl == 0) {
		if (level == 3)
			++dkRound;
		else if (level == 5)
			dkRound = 1;
	}
	FadeOut(2,1);
	DKSetGameState(STATE_PLAYING);
	level = lvl;
	pauseCounter = 0;
	nextFreeSprite = MAX_SPRITES;
	mario->moveMod = 3;
	mario->vx = 0;
	mario->vy = 0;
	mario->type = ACTOR_PLAYER;
	mario->spriteIndex = 0;
	mario->dim = (size){ 10, 16 };
	mario->dir = DIR_RIGHT;
	mario->state = 0;
	DKSetState(STATE_NORMAL, mario);
	memset(mutmap,0xff,MUT_BG_COUNT*sizeof(mutmap[0]));
	DKDrawLevel();

	for (u8 i = 0; i < MAX_ENEMIES; ++i)
		enemies[i].type = ACTOR_IDLE;
	bonus = 5000;
	levelTimer = 1;
	DKSetKongState(DK_STATE_TAUNT);

	DKPrintExtraLives();
	DKPrintLevel();
	DKPrintScores(1);

	switch(level) {
		case 0:
			mario->loc = (pt){48+(mario->dim.w>>1), 192+(mario->dim.h>>1)};
			DKSetKongState(DK_STATE_PICKUP);
			break;
		case 1:
			dirConveyorBelt = (PRNG_NEXT()&1)?DIR_RIGHT:DIR_LEFT;
			DKSyncConveyorBeltAnimations();
			mario->loc = (pt){56+(mario->dim.w>>1), 192+(mario->dim.h>>1)};
			DKSpawnEnemy(0,0,PGM_SPARKY3_ENEMY_INDEX,ACTOR_SPARKY);
			// Used for retractable ladders logic
			ladderRetracter = 1;
			break;
		case 2:
			mario->loc = (pt){8+(mario->dim.w>>1), 176+(mario->dim.h>>1)};
			DKSpawnEnemy(0,0,PGM_SPARKY1_ENEMY_INDEX,ACTOR_SPARKY);
			DKSpawnEnemy(0,0,PGM_SPARKY2_ENEMY_INDEX,ACTOR_SPARKY);
			DKSpawnEnemy(0,0,PGM_SPRING_ENEMY_INDEX,ACTOR_SPRING);
			// Init elevators
			u8 top = 56+(PRNG_NEXT()&31);
			elevators = (rect*)&allActors[MAX_ENEMIES].moveMod;

			for (u8 i = 0; i < MAX_ELEVATORS; ++i, top+=3*TILE_HEIGHT) {
				if (i&1) {
					elevators[i].left = 104;
					elevators[i].right = 120;
				} else {
					elevators[i].left = 40;
					elevators[i].right = 56;
				}
				elevators[i].top = top;
				elevators[i].btm = top+TILE_HEIGHT;
			}
			break;
		case 3:
			mario->loc = (pt){56+(mario->dim.w>>1), 192+(mario->dim.h>>1)};
			break;
	}
	DKResumeSong();
	FadeIn(2,1);
}


void DKShowWinScene(void) {
	u16 timer = 0;

	DKHideSprite(4,MAX_SPRITES-4,1);
	DKDrawActors(mario, 1);

	if (level == 3) {
		u8 x = 13*TILE_WIDTH, y = 9*TILE_HEIGHT;
		DKFill(&(rect){64,176,72,184},(gameState <= STATE_LADDER)?TILE_BLANK1:TILE_BLANK);
		DKFill(&(rect){64,176,184,208},TILE_BEAM_BLUE);
		DKSetBgAnimationFrames(2,23);
		TriggerFx(SFX_DK_FALL,SFX_DK_FALL_VOL,1);

		while (1) {
			if (GetVsyncFlag()) {
				ClearVsyncFlag();

				if (++timer == (20*HZ+16)) {
					break;
				} else if (timer == 40) {
					DKAddToScore(bonus);
					DKSetBgAnimationFrames(2,34);
				} else if (timer < 3*HZ) {
					// Kong flailing arms
					DKAnimateBgs(&animBgs.acs[2], 1);
					DKDrawAnimatedBgs(2,1);
				} else if (timer == 3*HZ) {
					DrawMap2(13,6,mapBlankKong);
					DKMapSprite(6, mapDKFalling, 0);
				} else if (timer < (4*HZ+30)) {
					// Kong falling upside down
					MoveSprite(6,x,y++,3,3);
				} else if (timer == (4*HZ+30)) {
					TriggerFx(SFX_KONG_STOMP,SFX_KONG_STOMP_VOL,true);
				} else if (timer == 6*HZ) {
					// Platform lowered
					DKHideSprite(0,4,1);
					DKFill(&(rect){64,176,16,72},(gameState <= STATE_LADDER)?TILE_BLANK1:TILE_BLANK);
					DKFill(&(rect){64,176,72,80},TILE_BEAM_BLUE);
					DrawMap2(11,7,mapPauline0);
				} else if (timer == 7*HZ) {
					StartSong(EndingSong);
					// Move mario to platform
					mario->dir = DIR_LEFT;
					DKLoadActorAnimation(mario);
					mario->loc.x = 128;
					mario->loc.y = 64;
					DKDrawActors(mario, 1);
				} else if (timer == 8*HZ) {
					// Heart shown
					DrawMap2(13,7,mapHeart);
				} else if (timer > 8*HZ && (ReadJoypad(0)&(BTN_A|BTN_B|BTN_START|BTN_SELECT))) {
					break;
				}
			}
		}
		StopSong();
	} else {
		u8 xKong, yKong;
		actor *a = 0;

		if (level == 0) {
			xKong = 3;
			yKong = 5;
		} else {
			xKong = 4;
			yKong = 4;
		}
		DrawMap2(xKong+9,yKong-2,mapHelp1);

		for (u8 i = 0; i < MAX_ENEMIES; ++i)
			enemies[i].type = ACTOR_IDLE;
		while (1) {
			if (GetVsyncFlag()) {
				ClearVsyncFlag();

				if (++timer == 5*HZ) {
					break;
				} else if (timer == (HZ>>1)) {
					DKAddToScore(bonus);
					DrawMap2(xKong+9,yKong-2,mapHeart);
					StartSong(LevelEnd);
				} else if (timer == 3*HZ) {
					TriggerFx(SFX_DK_ESCAPE,SFX_DK_ESCAPE_VOL,true);
					DrawMap2(xKong+7,yKong-2,mapBlankPauline);
					DrawMap2(xKong,yKong,mapBlankKong);
					DrawMap2(xKong+9,yKong-2,mapBrokenHeart);
					a = DKSpawnEnemy((xKong<<3)+44,(yKong<<3)-4, PGM_KONG_ENEMY_INDEX, ACTOR_KONG);
					DKDrawActors(a, 1);
				} else if (timer == (4*HZ)) {
					DKHideSprite(4,3,3);
				} else if (timer > 4*HZ) {
					continue;
				} else if (timer > (3*HZ+28)) {
					if (timer == (3*HZ+40))
						TriggerFx(SFX_DK_ESCAPE,SFX_DK_ESCAPE_VOL,true);
					DKAnimateEnemies();

					if (timer&1) {
						--a->loc.y;
						DKDrawActors(a, 1);
					}
				}
			}
		}
	}
}


void DKSyncConveyorBeltAnimations(void) {
	for (u8 i = 0, animIndex = 0; i < MAX_ANIMATED_BGS; ++i) {
		switch (i) {
			case 1:
			case 6:
				animIndex = (dirConveyorBelt == DIR_LEFT)?28:25;
				break;
			case 4:
			case 7:
				animIndex = (dirConveyorBelt == DIR_LEFT)?29:26;
				break;
			default:
				continue;
		}
		DKSetBgAnimationFrames(i, animIndex);
	}
}


void DKConveyorBeltLogic(void) {
	if ((gameTicks&511) == 0) {
		char dir = (PRNG_NEXT()&1)?DIR_RIGHT:DIR_LEFT;

		if (dirConveyorBelt != dir) {
			dirConveyorBelt = dir;
			DKSyncConveyorBeltAnimations();
		}
	}

	if ((gameTicks&127) == 0 && (prng&1)) {
		// Retractable ladders
		if (ladderRetracter != 1 || mario->state == STATE_JUMPING || mario->loc.y < 49 || mario->loc.y > 80)
			ladderRetracter = (ladderRetracter < 0)?65:-65;
	}

	if (ABS(ladderRetracter) > 1) {
		ladderRetracter = (ladderRetracter < 0)?ladderRetracter+1:ladderRetracter-1;

		if ((ABS(ladderRetracter)&31) == 2) {
			if (ladderRetracter > 0) {
				if (ladderRetracter == 34) {
					DrawMap2(2,9,mapLadderWhite2);
					DrawMap2(25,9,mapLadderWhite2);
				} else {
					DrawMap2(2,8,mapLadderWhite2);
					DrawMap2(25,8,mapLadderWhite2);
				}
			} else {
				if (ladderRetracter == -34) {
					DrawMap2(2,8,mapBlankLadder);
					DrawMap2(25,8,mapBlankLadder);
				} else {
					DrawMap2(2,9,mapBlankLadder);
					DrawMap2(25,9,mapBlankLadder);
				}
			}
		}
	}

	if ((gameTicks&63) == 0) { // && (PRNG_NEXT()&1)) {
		u8 x = 0,y = 0,z = prng&3;

		if (dirConveyorBelt == DIR_RIGHT)
			z &=~ 1;
		else
			z |= 1;

		if (z == 0) {
			x = 9;
			y = 92;
		} else if (z == 1) {
			x = 231;
			y = 92;
		} else if (z == 2) {
			x = 9;
			y = 172;
		} else if (z == 3) {
			x = 231;
			y = 172;
		}

		for (u8 i = 0; i < MAX_ENEMIES; ++i) {
			if (enemies[i].type == ACTOR_PIE && enemies[i].loc.y == y && ABS(enemies[i].loc.x-x) < 48)
				return;
		}
		DKSpawnEnemy(x,y,PGM_PIE_ENEMY_INDEX,ACTOR_PIE);
	}
}


void DKGhostSpawnLogic(void) {
	if ((gameTicks&255) == 0)
		DKSpawnEnemy(0,0,PGM_GHOST_ENEMY_INDEX,ACTOR_GHOST);
}


u8 DKServiceTimers(void) {
	u8 pause = 0;

	if (mario->state == STATE_DYING) {
		DKAnimatePlayer();
		DKDrawActors(mario, 1);
		pause = 1;
	} else if (pauseCounter) {
		DKAnimateEnemies();
		pause = 1;
	} else if (sledgeTimer && --sledgeTimer == 0) {
		DKSetState(STATE_NORMAL, mario);

		if (level == 3) {
			for (u8 i = 0; i < MAX_ENEMIES; ++i) {
				if (enemies[i].type == ACTOR_GHOST)
					DKSetState(STATE_RED, &enemies[i]);
			}
		}
	}

	if (mario->state != STATE_DYING && --levelTimer == 0) {
		levelTimer = 2*HZ;

		if (bonus)
			bonus -= 100;
		if (bonus == 0)
			DKSetState(STATE_DYING, mario);
		DKPrintBonus();
	}

	if (bonusTimer && --bonusTimer == 0)
		DKClearBonus();
	return pause;
}


void DKResumeSong(void) {
	switch (level) {
		case 0:
			StartSong(Level1Song);
			break;
		case 1:
			StartSong(Level2Song);
			break;
		case 2:
			StopSong();
			break;
		case 3:
			StartSong(Level3Song);
			break;
	}
}


void DKShowTitleScreen(void) {
	StopSong();
	DKHideSprite(0,MAX_SPRITES,1);
	level = 4;
	DKSetGameState(STATE_TITLE);
	DKDrawLevel();
	DKPrintScores(1);
}


void DKPaintInners(u8 startIndex) {
	bgInner bgi;

	do {
		memcpy_P(&bgi,innerDir+startIndex++,sizeof(bgi));

		if (bgi.type&BGP)
			DKFillMap(&bgi.r,(const char*)pgm_read_word(bgMaps+bgi.tile));
		else
			DKFill(&bgi.r,bgi.tile);
	} while ((bgi.type&BGEOF) == 0);
}

u8 DKIncBeamIndex(u8 index) {
	switch (index) {
		case 0: index = 1; break;
		case 1: index = 7; break;
		case 7: index = 11; break;
		case 11: index = 16; break;
		case 16: index = 23; break;
		default: index = 0; break;
	}
	return index;
}


void DKShowIntroScene(void) {
	u8 state = 0, beamIndex = 0;
	u16 timer = 0;
	actor *a = 0;

	for (u8 i = 0; i < MAX_ENEMIES; ++i)
		enemies[i].type = ACTOR_IDLE;
	pauseCounter = 0;
	// Draw initial level
	DKSetGameState(STATE_INTRO);
	level = 5;
	DKDrawLevel();
	DKPrintScores(1);
	StartSong(StartGame);

	while (1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();

			if (state && (ReadJoypad(0)&(BTN_A|BTN_B|BTN_START|BTN_SELECT)))
				break;
			if (state == 0) {
				if (timer > (HZ>>1))
					state = 1;
			} else if (state == 1) {
				a = DKSpawnEnemy(108,188, PGM_KONG_ENEMY_INDEX, ACTOR_KONG);
				DKDrawActors(a, 1);
				state = 2;
			} else if (state == 2) {
				if (timer&1) {
					DKAnimateEnemies();
					--a->loc.y;
					DKDrawActors(a, 1);

					if (a->loc.y == 76)
						state = 3;
				}
			} else if (state == 3) {
				if (timer > (4*HZ+48)) {
					--a->loc.y;
					DKDrawActors(a, 1);

					if (a->loc.y == 36)
						state = 4;
				}
			} else if (state == 4) {
				if (timer&1) {
					++a->loc.y;
					DKDrawActors(a, 1);

					if (a->loc.y == 52) {
						TriggerFx(SFX_KONG_STOMP,SFX_KONG_STOMP_VOL,true);
						DrawMap2(10,3,mapPauline0);
						memcpy_P(&a->ac.anim,animations+36,sizeof(a->ac.anim));
						DKMapSprite(a->spriteIndex, a->ac.anim.frames, a->spriteFlags);
						DKPaintInners(beamIndex);
						beamIndex = DKIncBeamIndex(beamIndex);
						state = 5;
					}
				}
			} else if (state == 5) {
				if (timer&1) {
					--a->loc.y;
					--a->loc.x;
					DKDrawActors(a, 1);

					if (a->loc.y == 44) {
						state = 6;
					}
				}
			} else if (state == 6) {
				if (timer&1) {
					++a->loc.y;
					--a->loc.x;
					DKDrawActors(a, 1);

					if (a->loc.y == 52) {
						TriggerFx(SFX_KONG_STOMP,SFX_KONG_STOMP_VOL,true);
						DKPaintInners(beamIndex);
						beamIndex = DKIncBeamIndex(beamIndex);
						state = 5;
					}

					if (a->loc.x == 28) {
						DrawMap2(2,4,mapDKTaunt0);
						DKHideSprite(a->spriteIndex, a->ac.anim.s.w, a->ac.anim.s.h);
						state = 7;
					}
				}
			} else if (state == 7) {
			 	if (timer == 12*HZ)
					break;
				if ((timer&31) == 0)
					DrawMap2(2,4,(timer&63)?mapDKTaunt1:mapDKTaunt0);
				if (timer < (11*HZ) && (timer&63) == 0)
					TriggerFx(SFX_DK_ESCAPE,SFX_DK_ESCAPE_VOL,true);
			}

			if ((state == 2 || state == 3) && a->loc.y > 48) {
				if ((a->loc.y&7) == 4) {
					u8 x = (a->loc.x-8)>>3, y = (a->loc.y+24)>>3;
					vram[x+y*VRAM_TILES_H] = vram[x+y*VRAM_TILES_H+1];
					vram[x+2+y*VRAM_TILES_H] = vram[x+3+y*VRAM_TILES_H];
				}
			}
			++timer;
		}
	}
	StopSong();
}



void DKLoadHiScore(void) {
	struct EepromBlockStruct *ebs = (struct EepromBlockStruct*)enemies;
	ebs->id = DK_EEPROM_ID;

	if (!EepromReadBlock(ebs->id, ebs)) {
		hiScore = *(u32*)ebs->data;
	} else {
		u32 *s;

		for (u8 i = 0; i < 4; ++i) {
			s = (u32*)&ebs->data[7*i];
			*s = 0;
			#pragma GCC diagnostic ignored "-Wstringop-truncation"
			strncpy((char*)&ebs->data[7*i+4],"UZE",3);
		}
		EepromWriteBlock(ebs);
	}
}


void DKSaveHiScore(char *name) {
	u32 *s;
	struct EepromBlockStruct *ebs = (struct EepromBlockStruct*)enemies;

	ebs->id = DK_EEPROM_ID;

	if (EepromReadBlock(ebs->id, ebs))
		return;

	for (u8 i = 0; i < 4; ++i) {
		s = (u32*)&ebs->data[7*i];

		if (score > *s) {
			for (u8 j = 27; j >= 7*(i+1); --j)
				ebs->data[j] = ebs->data[j-7];
			//memmove(ebs.data+7*(i+1),ebs.data+7*i,7*(3-i));
			*s = score;
			strncpy((char*)&ebs->data[7*i+4],name,3);
			break;
		}
	}
	EepromWriteBlock(ebs);
}

#define ALPHA_TILES_OFFSET 0x3a

void DKPrintString(u8 x, u8 y, char *s, u8 count) {
	for (u8 i = 0; i < count; ++i,++s)
		SetTile(x++,y,ALPHA_TILES_OFFSET+*s-'A');
}


u8 DKShowHiScoreScreen(u8 editable) {
	u8 temp = gameState;

	DKHideSprite(0,MAX_SPRITES,1);
	DKSetGameState(STATE_HISCORE);
	DKClearVram();
	SetTile(SCORE_LOC_X,SCORE_LOC_Y,TILE_1UP1);
	DrawMap2(HISCORE_LOC_X,HISCORE_LOC_Y,mapHiScore1);
	DKPrintScores(1);
	DKPaintInners(26);

	u8 index = 4, x = 10, y = 11;
	u32 *s;
	struct EepromBlockStruct *ebs = (struct EepromBlockStruct*)enemies;
	ebs->id = DK_EEPROM_ID;

	if (EepromReadBlock(ebs->id, ebs))
		return 0;

	for (u8 i = 0, limit = 4; i < limit; ++i,y+=2) {
		s = (u32*)&ebs->data[7*i];

		if (temp == STATE_GAME_OVER && index == 4 && score > *s) {
			index = i;
			s = &score;
			--i;
			--limit;
		} else {
			DKPrintString(x+11,y,(char*)&ebs->data[7*i+4],3);
		}
		DKPrintNumber(x,y,*s,7);
	}

	//DKSpawnEnemy(0,0,PGM_BARREL_ENEMY_INDEX,ACTOR_BARREL);
	mario->type = ACTOR_PLAYER;
	mario->dir = DIR_RIGHT;
	mario->vx = 1;
	mario->loc = (pt){116, 160};
	mario->state = 0;
	DKSetState(STATE_NORMAL, mario);
	DKMapSprite(4,mapBarrelRoll0, 0);
	MoveSprite(4,112,168,1,1);

	char name[3] = { 'A','A','A' };
	u8 cursor = 0, counter = 0;
	u16 btnHeld = 0, btnPressed = 0;

	if (editable && index < 4) {
		x += 11;
		y = 11+index*2;
		DKPrintString(x,y,name,3);
	} else {
		editable = 0;
	}

	index = 0; // Re-use for barrel animation

	while(1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();
			++gameTicks;
			btnPressed = btnHeld;
			btnHeld = ReadJoypad(0);
			btnPressed = btnHeld&(btnHeld^btnPressed);

			if (editable) {
				if (btnHeld == 0 && (++counter&15) == 0)
					SetTile(x+cursor,y,TILE_BLANK1);
				else if (btnHeld || (counter&7) == 0)
					SetTile(x+cursor,y,ALPHA_TILES_OFFSET+name[cursor]-'A');

				if (btnPressed&BTN_START) {
					DKSaveHiScore(name);
					editable = 0;
					gameTicks = 240;
					continue;
				}

				if (btnPressed&BTN_UP) {
					name[cursor]++;
					temp = 0;
				} else if (btnHeld&BTN_UP) {
					if ((++temp&7) == 0)
						name[cursor]++;
				} else if (btnPressed&BTN_DOWN) {
					name[cursor]--;
					temp = 0;
				} else if (btnHeld&BTN_DOWN) {
					if ((++temp&7) == 0)
						name[cursor]--;
				} else if (btnPressed&BTN_LEFT) {
					cursor = (cursor)?cursor-1:2;
				} else if (btnPressed&BTN_RIGHT) {
					cursor = (cursor < 2)?cursor+1:0;
				}

				if (name[cursor] < 'A')
					name[cursor] = 'Z';
				else if (name[cursor] > 'Z')
					name[cursor] = 'A';
			} else if (gameTicks == 480) {
				break;
			} else if (btnPressed&BTN_START) {
				DKHideSprite(0,5,1);
				return 1;
			}
			if (gameTicks&1) {
				DKAnimatePlayer();
				DKDrawActors(mario, 1);
			} else if ((gameTicks&7) == 0) {
				DKMapSprite(4,mapBarrelRoll0+(index++&3)*3, 0);
			}
		}
	}
	return 0;
}


u8 DKNextLevel(void) {
	u8 lvl = level;

	switch (lvl) {
		case 0:
			if (dkRound >= 3)
				lvl = 1;
			else if (dkRound >= 2)
				lvl = 2;
			else
				lvl = 3;
			break;
		case 1:
		case 2:
			++lvl;
			break;
		default:
			lvl = 0;
			break;
	}
	return lvl;
}


void DKSetGameState(u8 state) {
	gameState = state;

	switch (state) {
		case STATE_TITLE:
		case STATE_HISCORE:
			gameTicks = 0;
		case STATE_LADDER:
			SetTileTable(tileset0+TILE_BLANK*64);
			break;
		case STATE_INTRO:
		case STATE_PLAYING:
		case STATE_PLAYER_DEAD:
		case STATE_GAME_OVER:
		case STATE_PLAYER_WIN:
			SetTileTable(tileset0);
			break;
	}
}


int main(void) {
	u16 btnHeld = 0;    		// Buttons that are held right now
	u16 btnPressed = 0;  		// Buttons that were pressed this frame

	prng = 1;
	mario = allActors;
	enemies = allActors+1;

	DKLoadHiScore();
	SetTileTable(tileset0);
	SetSpritesTileTable(spriteset);
	SetSpriteVisibility(true);
	InitMusicPlayer(patches);
	SetMasterVolume(0xc0);
	DKShowTitleScreen();

	while(1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();
			++gameTicks;
			btnPressed = btnHeld;
			btnHeld = ReadJoypad(0);
			btnPressed = btnHeld&(btnHeld^btnPressed);

			if ((btnPressed&BTN_SELECT) && gameState != STATE_TITLE) {
				DKShowTitleScreen();
				continue;
			}

			if ((btnPressed&BTN_START) && gameState != STATE_TITLE && gameState != STATE_HISCORE) {
				if (gameState&STATE_PAUSED)
					ResumeSong();
				else
					StopSong();
				DKSetGameState(gameState^STATE_PAUSED);
			}

			if (gameState&STATE_PAUSED)
				continue;

			if (gameState == STATE_TITLE || gameState == STATE_HISCORE) {
				PRNG_NEXT();

				if (btnPressed&BTN_START) {
					score = 0;
					extraLives = 5;
					StopSong();
					DKShowIntroScene();
					DKInitLevel(0);
				} else if (gameState == STATE_TITLE) {
					DKAnimateBgs(animBgs.acs, animBgs.count);
					DKDrawAnimatedBgs(0, animBgs.count);
				}

				if (gameTicks == 480) {
					if (!DKShowHiScoreScreen(0))
						DKShowTitleScreen();
				}
				continue;
			}

			climbFlag = 0;

			if (gameState == STATE_PLAYER_DEAD) {
				--extraLives;
				DKInitLevel(level);
			} else if (gameState == STATE_GAME_OVER) {
				if (!DKShowHiScoreScreen(1))
					DKShowTitleScreen();
				continue;
			}
			if (DKServiceTimers())
				continue;
			mario->moveCmd = btnHeld;
			DKMoveActors(allActors, MAX_ACTORS);
			DKDetectActorCollisions();

			if (mario->state == STATE_DYING) {
				continue;
			} else if (gameState == STATE_PLAYER_WIN) {
				DKClearBonus();
				StopSong();
				DKShowWinScene();
				DKInitLevel(DKNextLevel());
				continue;
			}

			if (level == 1) {
				DKConveyorBeltLogic();
			} else if (level == 2) {
				DKMoveElevators();
				DKDrawElevators();
				DKBindToElevators();
			} else if (level == 3) {
				DKGhostSpawnLogic();
			}
			DKAnimatePlayer();
			DKAnimateEnemies();
			DKDrawActors(allActors, MAX_ACTORS);
			DKKongAI();
			DKAnimateBgs(animBgs.acs, animBgs.count);
			DKDrawAnimatedBgs(0, animBgs.count);
		}
	}
}

