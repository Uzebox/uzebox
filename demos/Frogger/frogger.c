
/*
 *  Frogger for Uzebox
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
#include <avr/pgmspace.h>
#include <string.h>
#include <math.h>
#include <uzebox.h>

/****************************************
 *				Constants				*
 ****************************************/

#define MOVING_REGIONS_COUNT 9
#define HOME_REGION 0
#define PAVEMENT_REGION 6
#define GRASS_REGION (SCREEN_SECTIONS_COUNT-1)

// Sprite indexes
#define SPRITE_P1 1
#define SPRITE_P2 5
#define SPRITE_SNAKE 9
#define SPRITE_BONUS 13
#define SPRITES_OFFSET_P2 20

// Tiles
#define TILE_ROAD 0
#define TILE_BRICK 1
#define TILE_GRASS 2
#define TILE_WATER 3
#define TILE_MUD 4
#define TILE_CLOCK 5
#define TILE_LIVES_P1 6
#define TILE_LIVES_P2 7
#define TILE_TIME 8
#define DIGIT_INDEX 0x4d
#define ALPHA_INDEX 0x57

// Actor types
#define ACTOR_PLAYER 0x1
#define ACTOR_SNAKE 0x2

// Game states
#define STATE_TITLE 0x1
#define STATE_PLAYING 0x2
#define STATE_PAUSED 0x3
#define STATE_LVL_CLEAR 0x4
#define STATE_GAME_OVER 0x5

// Frogger states
#define STATE_INACTIVE 0x1
#define STATE_IDLE 0x2
#define STATE_JUMPING 0x4
#define STATE_DROWNING 0x8
#define STATE_SQUASHED 0x10
#define STATE_STRANDED 0x20
#define STATE_HOME 0x40
#define STATE_WON 0x80

// Snake states
//#define STATE_INACTIVE 0x1
#define STATE_ACTIVE 0x2

// Turtle states
#define STATE_SWIMMING 0x1
#define STATE_SUBMERGING 0x2
#define STATE_EMERGING 0x4
#define STATE_SUBMERGED 0x8

// Home states
#define STATE_EMPTY 0x1
#define STATE_MATE 0x2
#define STATE_FLY 0x4
#define STATE_CROC 0x8
#define STATE_OCCUPIED_P1 0x10
#define STATE_OCCUPIED_P2 0x20
#define STATE_OCCUPIED (STATE_OCCUPIED_P1|STATE_OCCUPIED_P2)

// Sfx
#define SFX_HOP 12
#define SFX_HOP_VOL 0xff
#define SFX_BEEP 16
#define SFX_BEEP_VOL 0xff
#define SFX_DEAD 17
#define SFX_DEAD_VOL 0xff
#define SFX_BONUS 18
#define SFX_BONUS_VOL 0xff
#define SFX_TIME_BONUS 19
#define SFX_TIME_BONUS_VOL 0xd0
#define SFX_LIFE_LOSS_2P 20
#define SFX_LIFE_LOSS_2P_VOL 0xff

// Misc
#define HZ 60
#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define FROGGER_ANIM_SIZE 8
#define MAX_SCORE 99999
#define MAX_TIME 250
#define MAX_LIVES 10
#define HIDE_X 160
#define FRG_EEPROM_ID 16

/****************************************
 *				Utils					*
 ****************************************/
// General
#define MIN(x,y) (((x)<(y)) ? (x) : (y))
#define MAX(x,y) (((x)>(y)) ? (x) : (y))
#define ABS(x) (((x) > 0) ? (x) : -(x))
#define NORMALIZE(x) (((x) > 0) ? 1 : ((x) < 0) ? -1 : 0)
// 8-bit, 255 period LFSR (for generating pseudo-random numbers)
#define PRNG_NEXT() (prng = ((u8)((prng>>1) | ((prng^(prng>>2)^(prng>>3)^(prng>>4))<<7))))

/****************************************
 *			Type declarations			*
 ****************************************/
typedef struct {
	int left;
	int right;
	int top;
	int btm;
} rect;

typedef struct {
	char vel;
	u8 counter;
	u8 counterMax;
} velocity;

typedef struct {
	velocity vx;
	u8 obstacleCount;
	const rect *obstacles;
	struct ScreenSectionStruct *ss;
} regionSettings;

typedef struct {
	u8 frameSize;
	u8 frameCount;
	u8 frameSequenceIndex;
	u8 frameDurationsIndex;
	const char *frames;
} animation;

typedef struct {
	u8 frame;
	u8 counter;
	u8 duration;
} animationCursor;

typedef struct {
	u8 state;
	animation anim;
	animationCursor cur;
} bgEntity;

typedef struct {
	u8 type;
	u8 sprite;
	u8 state;
	u8 x;
	u8 y;
	u8 wid;
	u8 hgt;
	u8 dir;
	u8 temp;
	int lives;
	animation anim;
	animationCursor cur;
} actor;

/****************************************
 *			Data Dependencies			*
 ****************************************/
#include "data/tiles.inc"
#include "data/sprites.inc"
#include "data/patches.inc"
#include "data/song.inc"
#include "data/frogger_end.inc"
#include "data/frogger_end2.inc"

/****************************************
 *			File-level variables		*
 ****************************************/
u16 gameTicks;
u8 prng;
regionSettings regions[MOVING_REGIONS_COUNT];
actor frogger[2];
bgEntity turtles[6];
bgEntity gator;
actor snake;
int scoreBonus;
long score;
long hiScore;
int time;
u8 homeStates[5];
u8 timerBonus0;
u8 players;
u8 gameState;
u8 level = 0;
u16 tempTimer;
u8 beepTimer;
bool gatorIsActive = false;

const char strPlayer[] PROGMEM = "PLAYER";
const char strPlayers[] PROGMEM = "PLAYERS";
const char strGame[] PROGMEM = "GAME";
const char strOver[] PROGMEM = "OVER";
const char strWinner[] PROGMEM = "WINNER";
const char strDraw[] PROGMEM = "DRAW";

// Top to bottom of screen
const rect obstacles[] PROGMEM = {
	{ 0,50,24,40 }, { 90,140,24,40 }, // Logs
	{ 13,36,40,56 }, { 73,96,40,56 }, { 133,156,40,56 }, // Turtles
	{ 0,68,56,72 }, { 102,170,56,72 }, // Logs
	{ 36,80,72,88 }, { 114,158,72,88 }, // Logs
	{ 1,36,88,104 }, { 61,96,88,104 }, { 121,156,88,104 }, // Turtles
	{ 42,70,120,136 }, { 126,154,120,136 }, // Lorries
	{ 46,60,136,152 }, { 166,180,136,152 }, // Sports cars
	{ 12,22,152,168 }, { 66,76,152,168 }, { 156,166,152,168 }, // Tractors
	{ 2,12,168,184 }, { 50,60,168,184 }, { 98,108,168,184 } // Sedans
};

const velocity regionVels[] PROGMEM = {
	{-1,0,4},{1,0,3},{-1,0,2},{-1,0,6},{1,0,3},{-1,0,2},{2,0,2},{-1,0,4},{1,0,4}
};

const u8 obstacleCounts[] PROGMEM = {
	2,3,2,2,3,2,2,3,3
};

// Animations
const u8 frameDurations[] PROGMEM = {
	255,					// (0)
	3,3,3,3,				// (1)
	10,10,10,40,			// (5)
	32,32,					// (9)
	18,18,18,18,			// (11)
	4,4,4,4,				// (15)
	40,						// (19)
	10,10,					// (20)
	180,90,					// (22)
};

const u8 frameSequences[] PROGMEM = {
	0,						// (0)
	0,1,2,1,				// (1)
	0,1,2,3,				// (5)
	0,1,					// (9)
	0,1,2,3,				// (11)
	3,2,1,0,				// (15)
};

const animation animations[] PROGMEM = {
	{ 6,1,0,0,mapP1N0 },
	{ 6,1,0,0,mapP1E0 },
	{ 6,1,0,0,mapP1S0 },
	{ 6,1,0,0,mapP1W0 },
	{ 6,4,1,15,mapP1N0 },
	{ 6,4,1,1,mapP1E0 },
	{ 6,4,1,15,mapP1S0 },
	{ 6,4,1,1,mapP1W0 },
	{ 6,4,5,5,mapFroggerDrown0 },
	{ 10,2,9,9,mapTurtlesSml0 },
	{ 14,2,9,9,mapTurtlesLge0 },
	{ 10,4,11,11,mapSubmergingSml0 },
	{ 14,4,11,11,mapSubmergingLge0 },
	{ 10,4,15,11,mapSubmergingSml0 },
	{ 14,4,15,11,mapSubmergingLge0 },
	{ 10,1,0,0,mapSubmergedSml0 },
	{ 14,1,0,0,mapSubmergedLge0 },
	{ 6,1,0,19,mapFroggerDrown3 },
	{ 6,2,9,20,mapSnakeE0 },
	{ 6,2,9,20,mapSnakeW0 },
	{ 6,1,0,0,mapP2N0 },
	{ 6,1,0,0,mapP2E0 },
	{ 6,1,0,0,mapP2S0 },
	{ 6,1,0,0,mapP2W0 },
	{ 6,4,1,15,mapP2N0 },
	{ 6,4,1,1,mapP2E0 },
	{ 6,4,1,15,mapP2S0 },
	{ 6,4,1,1,mapP2W0 },
	{ 20,2,9,22,mapGator0 },
};

/****************************************
 *			Function prototypes			*
 ****************************************/
void FRGFill(u8 x, u8 y, u8 wid, u8 hgt, u8 tileId);
void FRGFillMap(u8 x, u8 y, u8 wid, u8 hgt, const char *map);
void FRGDrawLevel(void);
void FRGMoveRegions(void);
void FRGAnimateTurtles(void);
void FRGDrawTurtles(void);
void FRGMoveActor(actor *a);
void FRGAnimateActor(actor *a);
void FRGLoadActorAnimations(actor *a);
void FRGSetActorState(u8 state, actor *a);
void FRGCalcFroggerBounds(rect *r, const actor *a);
u8   FRGRectsIntersect(const rect *r1, const rect *r2);
u8   FRGPointInRect(int x, int y, const rect *r);
int  FRGClampToXAxis(int x);
void FRGResetFrogger(u8 index);
void FRGSetTurtleState(u8 index, u8 state);
void FRGTurtlesThink(void);
void FRGAddScore(long score);
void FRGDrawScore(void);
void FRGDrawHiScore(void);
void FRGDrawLevelNumber(void);
void FRGDrawLives(const actor *a);
void FRGAddLives(actor *a, int val);
void FRGAddTime(int val);
void FRGDrawTime(void);
void FRGPrintNumber(u8 x, u8 y, u32 val, u8 digits);
void FRGLoadTurtleAnimations(u8 index);
void FRGTestBounds(u8 index);
void FRGHomesThink(void);
void FRGDrawHome(int index);
void FRGSetHomeState(int index, u8 state);
void FRGFlashBonus(u8 x, u8 y);
void FRGSnakeThink(void);
void FRGTestActorCollisions(actor *a);
void FRGDrawTitleScreen(void);
void FRGPrint(u8 x, u8 y, const char *msg);
void FRGPrintCursor(u8 pos);
void FRGSetGameState(u8 state);
void FRGCheckGameState(void);
void FRGLoadHiScore(void);
void FRGSaveHiScore(void);
void FRGPlayAmbientSfx(void);
void FRGDrawGator(void);
void FRGAnimateGator(void);
void FRGGatorThink(void);
void FRGTriggerSfx(u8 index);

/****************************************
 *			Function definitions		*
 ****************************************/
void FRGFill(u8 x, u8 y, u8 wid, u8 hgt, u8 tileId) {
	u8 xLim = x+wid, yLim = y+hgt;

    for (; x < xLim; ++x) {
        for (u8 yit = y; yit < yLim; ++yit)
            SetTile(x,yit,tileId);
    }
}


void FRGFillMap(u8 x, u8 y, u8 wid, u8 hgt, const char *map) {
	u8 mapWid = pgm_read_byte(&map[0]);
	u16 mapSize = mapWid*pgm_read_byte(&map[1]);

	for (u8 yLim = y+hgt, yMap = 0; y < yLim; ++y,yMap+=mapWid) {
		if (yMap == mapSize)
			yMap = 0;
    	for (u8 xit = x, xLim = x+wid, xMap = 0; xit < xLim; ++xit,++xMap) {
			if (xMap == mapWid)
				xMap = 0;
            SetTile(xit,y,pgm_read_byte(map+xMap+yMap+2));
		}
    }
}


void FRGLoadHiScore(void) {
	struct EepromBlockStruct ebs;
	ebs.id = FRG_EEPROM_ID;

	if (!EepromReadBlock(ebs.id, &ebs)) {
		hiScore = *(long*)ebs.data;
	} else {
		long *s = (long*)ebs.data;
		*s = 0;
		EepromWriteBlock(&ebs);
	}
}


void FRGSaveHiScore(void) {
	struct EepromBlockStruct ebs;

	ebs.id = FRG_EEPROM_ID;

	if (EepromReadBlock(ebs.id, &ebs))
		return;

	long *s;

	s = (long*)ebs.data;

	if (score > *s) {
		*s = score;
		EepromWriteBlock(&ebs);
	}
}


void FRGAddScore(long val) {
	if (players != 1)
		return;

	// Bonus life
	scoreBonus += val;

	if (scoreBonus < 0) {
		scoreBonus = 0;
	} else if (scoreBonus >= 1000) {
		scoreBonus -= 1000;
		FRGAddLives(frogger,1);
		FRGDrawLives(frogger);
	}

	// Score and hi score
	score += val;

	if (score > MAX_SCORE)
		score = MAX_SCORE;
	FRGDrawScore();

	if (score > hiScore) {
		hiScore = score;
		FRGDrawHiScore();
	}
}


void FRGDrawScore(void) {
	FRGPrintNumber(5,0,score,6);
}


void FRGDrawHiScore(void) {
	FRGPrintNumber(15,0,hiScore,6);
}


void FRGDrawLevelNumber(void) {
	FRGPrintNumber(19,25,level,2);
}


void FRGDrawLives(const actor *a) {
	if (players == 1)
		FRGPrintNumber(15,25,(a->lives)?a->lives-1:0,1);
	else
		FRGPrintNumber((a == frogger)?6:16,0,(a->lives)?a->lives-1:0,1);
}


void FRGAddLives(actor *a, int val) {
	a->lives += val;

	if (a->lives > MAX_LIVES)
		a->lives = MAX_LIVES;
	else if (a->lives < 0)
		a->lives = 0;
}


void FRGAddTime(int val) {
	time += val;

	if (time > MAX_TIME)
		time = MAX_TIME;
	else if (time < 0)
		time = 0;
}


void FRGDrawTime(void) {
	if (players == 2)
		return;
	u8 x = 2, w = time/25;

	FRGFill(x,25,10,1,TILE_ROAD);

	if (time == 0)
		return;

	FRGFill(x,25,w,1,TILE_TIME);

	w = time%25;

	if (w >= 21)
		w = 0;
	else if (w >= 17)
		w = 1;
	else if (w >= 13)
		w = 2;
	else if (w >= 9)
		w = 3;
	else if (w >= 5)
		w = 4;
	else if (w >= 1)
		w = 5;
	else
		return;
	SetTile(x+time/25,25,TILE_TIME+w);
}


void FRGPrintNumber(u8 x, u8 y, u32 val, u8 digits) {
	u8 numeral;

	for (u8 i = 0; i < digits; i++, val /= 10) {
		numeral = val % 10;
		SetTile(x+digits-i-1,y,DIGIT_INDEX+numeral);
	}
}


void FRGDrawLevel(void) {
	if (++level > 99)
		level = 99;
	ClearVram();

	if (players == 1) {
		// 1UP
		SetTile(1,0,DIGIT_INDEX+1);
		SetTile(2,0,ALPHA_INDEX+'U'-'A');
		SetTile(3,0,ALPHA_INDEX+'P'-'A');
		// Score
		FRGDrawScore();
		// HI
		SetTile(12,0,ALPHA_INDEX+'H'-'A');
		SetTile(13,0,ALPHA_INDEX+'I'-'A');
		// Hi Score
		FRGDrawHiScore();
		// Time
		SetTile(1,25,TILE_CLOCK);
		FRGDrawTime();
		// Lives
		SetTile(14,25,TILE_LIVES_P1);
		FRGDrawLives(frogger);
		// Level
		SetTile(18,25,ALPHA_INDEX+'L'-'A');
		FRGDrawLevelNumber();
	} else {
		// Lives P1
		SetTile(5,0,TILE_LIVES_P1);
		FRGDrawLives(frogger);
		// Lives P2
		SetTile(15,0,TILE_LIVES_P2);
		FRGDrawLives(frogger+1);
	}

	// Scene
	FRGFill(0,1,32,2,TILE_MUD);
	FRGFill(0,3,32,10,TILE_WATER);
	FRGFill(0,13,32,2,TILE_BRICK);
	FRGFill(0,15,32,8,TILE_ROAD);
	FRGFill(0,23,32,2,TILE_GRASS);

	for (u8 i = 2; i < SCREEN_TILES_H; i+=4)
		DrawMap2(i,1,mapEmptyHome);
	screenSections[PAVEMENT_REGION].scrollX = 4;
	// Medium logs
	DrawMap2(0,3,mapLogMed);
	DrawMap2(15,3,mapLogMed);
	// Large logs
	DrawMap2(0,7,mapLogLge);
	DrawMap2(17,7,mapLogLge);
	// Small logs
	DrawMap2(6,9,mapLogSml);
	DrawMap2(19,9,mapLogSml);
	// Turtles
	FRGDrawTurtles();
	// Lorries
	DrawMap2(7,15,mapLorry);
	DrawMap2(21,15,mapLorry);

	// Sports cars
	DrawMap2(7,17,mapSportsCar);
	DrawMap2(27,17,mapSportsCar);

	// Tractors
	DrawMap2(2,19,mapTractor);
	DrawMap2(11,19,mapTractor);
	DrawMap2(26,19,mapTractor);

	// Sedans
	DrawMap2(0,21,mapSedan);
	DrawMap2(8,21,mapSedan);
	DrawMap2(16,21,mapSedan);

	for (u8 i = 0, obIndex = 0; i < MOVING_REGIONS_COUNT; ++i) {
		memcpy_P(&regions[i].vx,regionVels+i,sizeof(regions[i].vx));
		memcpy_P(&regions[i].obstacleCount,obstacleCounts+i,sizeof(u8));
		regions[i].obstacles = obstacles+obIndex;
		obIndex += regions[i].obstacleCount;
		regions[i].ss->scrollX = PRNG_NEXT();

		if (regions[i].ss->scrollX > X_SCROLL_WRAP)
			regions[i].ss->scrollX -= X_SCROLL_WRAP;

		regions[i].vx.counterMax -= level/3;

		if (regions[i].vx.counterMax == 0 || regions[i].vx.counterMax > 6) // Clamp underflow (6 is slowest possible)
			regions[i].vx.counterMax = 1;
		// Prevent sports car speeding up too soon or it gets too difficult
		if (i == 6 && level < 6 && regions[i].vx.counterMax == 1) {
			regions[i].vx.counterMax = 2;
			regions[i].vx.vel = 3;
		}
	}

	for (u8 i = 0; i < 5; ++i)
		FRGSetHomeState(i,STATE_EMPTY);
}


void FRGMoveRegions(void) {
	for (u8 i = 0; i < MOVING_REGIONS_COUNT; ++i) {
		if (++regions[i].vx.counter == regions[i].vx.counterMax) {
			regions[i].vx.counter = 0;
			regions[i].ss->scrollX += regions[i].vx.vel;

			if (regions[i].ss->scrollX > X_SCROLL_WRAP) {
				if (regions[i].vx.vel > 0)
					regions[i].ss->scrollX -= X_SCROLL_WRAP;
				else
					regions[i].ss->scrollX -= 256-X_SCROLL_WRAP;
			}
		}
	}
}


void FRGLoadTurtleAnimations(u8 index) {
	int offset = 0;
	bgEntity *t = &turtles[index];

	switch (t->state) {
		case STATE_SWIMMING:
			offset = (index < 3)?9:10;
			break;
		case STATE_SUBMERGING:
			offset = (index < 3)?11:12;
			break;
		case STATE_EMERGING:
			offset = (index < 3)?13:14;
			break;
		case STATE_SUBMERGED:
			offset = (index < 3)?15:16;
			break;
	}

	t->cur.frame = 0;
	t->cur.counter = 0;
	memcpy_P(&t->anim, animations+offset, sizeof(animation));
	t->cur.duration = pgm_read_byte(frameDurations+t->anim.frameDurationsIndex+t->cur.frame);
}


void FRGTurtlesThink(void) {
	for (u8 i = 0; i < 6; ++i) {
		switch (turtles[i].state) {
			case STATE_SWIMMING:
				if ((PRNG_NEXT()&31) == 0)
					FRGSetTurtleState(i, STATE_SUBMERGING);
				break;
			case STATE_SUBMERGED:
				if ((PRNG_NEXT()&7) == 0)
					FRGSetTurtleState(i, STATE_EMERGING);
				break;
			case STATE_SUBMERGING:
			case STATE_EMERGING:
			default:
				break;
		}
	}
}

void FRGSetTurtleState(u8 index, u8 state) {
	turtles[index].state = state;
	FRGLoadTurtleAnimations(index);
}


void FRGDrawTurtles(void) {
	for (u8 i = 0, x = 2, y = 5; i < 6; ++i, x+=10) {
		if (i == 3) {
			x = 0;
			y = 11;
		}
		if (turtles[i].cur.counter) // Skip draw if not a new frame
			continue;
		DrawMap2(x,y,turtles[i].anim.frames+turtles[i].anim.frameSize*pgm_read_byte(frameSequences+turtles[i].anim.frameSequenceIndex+turtles[i].cur.frame));
	}
}


void FRGAnimateTurtles(void) {
	for (u8 i = 0; i < 6; ++i) {
		if (++turtles[i].cur.counter >= turtles[i].cur.duration) {
			turtles[i].cur.counter = 0;

			if (++turtles[i].cur.frame >= turtles[i].anim.frameCount) {
				turtles[i].cur.frame = 0;

				if (turtles[i].state == STATE_SUBMERGING) {
					FRGSetTurtleState(i, STATE_SUBMERGED);
					continue;
				} else if (turtles[i].state == STATE_EMERGING) {
					FRGSetTurtleState(i, STATE_SWIMMING);
					continue;
				}
			}
			turtles[i].cur.duration = pgm_read_byte(frameDurations+turtles[i].anim.frameDurationsIndex+turtles[i].cur.frame);
		}
	}
}


void FRGDrawGator(void) {
	if (!gator.cur.counter) // Skip draw if not a new frame
		DrawMap2(0,3,gator.anim.frames+gator.anim.frameSize*pgm_read_byte(frameSequences+gator.anim.frameSequenceIndex+gator.cur.frame));
}


void FRGAnimateGator(void) {
	if (++gator.cur.counter >= gator.cur.duration) {
		gator.cur.counter = 0;

		if (++gator.cur.frame >= gator.anim.frameCount)
			gator.cur.frame = 0;
		gator.cur.duration = pgm_read_byte(frameDurations+gator.anim.frameDurationsIndex+gator.cur.frame);
	}
}


void FRGGatorThink(void) {
	if (regions[0].ss->scrollX == 60 && regions[0].vx.counter == 0) {
		if (prng&1) {
			if (!gatorIsActive) {
				gator.cur.counter = 0;
				FRGDrawGator();
				gatorIsActive = true;
			} else {
				gatorIsActive = false;
				DrawMap2(0,3,mapLogMed);
			}
		}
	}

	if (gatorIsActive) {
		FRGAnimateGator();
		FRGDrawGator();
	}
}


u8 FRGRectsIntersect(const rect *r1, const rect *r2) {
	return !((r1->btm < r2->top) || (r1->right < r2->left) || (r1->left > r2->right) || (r1->top > r2->btm));
}


u8 FRGPointInRect(int x, int y, const rect *r) {
	return (x > r->left && x < r->right && y > r->top && y < r->btm);
}


void FRGCalcFroggerBounds(rect *r, const actor *a) {
	if (a->dir&1) { // Right/Left
		r->top = a->y-4;
		r->btm = a->y+4;
		r->left = a->x-5;
		r->right = a->x+5;
	} else { // Up/Down
		r->top = a->y-5;
		r->btm = a->y+5;
		r->left = a->x-4;
		r->right = a->x+4;
	}

	if (r->left > r->right)
		r->left = 0;
	if (r->top > r->btm)
		r->top = 0;
}


int FRGClampToXAxis(int x) {
	if (x > X_SCROLL_WRAP)
		x -= X_SCROLL_WRAP;
	else if (x < 0)
		x = X_SCROLL_WRAP+x;
	return x;
}


void FRGFlashBonus(u8 x, u8 y) {
	MapSprite(SPRITE_BONUS,mapBonus);
	MoveSprite(SPRITE_BONUS,x,y,2,1);
	timerBonus0 = HZ;
}


void FRGTestBounds(u8 index) {
	if (frogger[index].state&(STATE_SQUASHED|STATE_DROWNING|STATE_STRANDED|STATE_HOME))
		return;

	if (frogger[index].y < 24) {	// Home region
		u8 i = 16;

		for (;i < 144; i+=24)
			if (frogger[index].x >= i && frogger[index].x <= (i+4))
				break;
		if (i < 144) {
			i = (i-16)/24;

			if (homeStates[i]&(STATE_CROC|STATE_OCCUPIED)) {
				FRGSetActorState(STATE_STRANDED,&frogger[index]);
			} else {
				if (homeStates[i]&(STATE_MATE|STATE_FLY)) {
					FRGAddScore(200);
					FRGDrawScore();
					FRGTriggerSfx(SFX_BONUS);
					FRGFlashBonus(frogger[index].x,frogger[index].y);
				}
				FRGSetHomeState(i,(index)?STATE_OCCUPIED_P2:STATE_OCCUPIED_P1);
				FRGSetActorState(STATE_HOME,&frogger[index]);
			}
		} else {
			FRGSetActorState(STATE_STRANDED,&frogger[index]);
		}
	} else if (frogger[index].y < 104) {	// River region
		if (frogger[index].x == 0 || frogger[index].x >= 132) {
			FRGSetActorState(STATE_DROWNING,&frogger[index]);
			return;
		} else if (frogger[index].state == STATE_JUMPING) {
			return;
		}

		bool drowning = true;
		int x;
		rect rOb;

		for (u8 i = 0; drowning && i < 5; ++i) {
			x = FRGClampToXAxis(frogger[index].x+regions[i].ss->scrollX);

			for (u8 j = 0; drowning && j < regions[i].obstacleCount; ++j) {
				memcpy_P(&rOb,regions[i].obstacles+j,sizeof(rOb));

				if (FRGPointInRect(x,frogger[index].y,&rOb)) {
					if (gatorIsActive && gator.cur.frame == 1 && i == 0 && j == 0) {
						rOb.left += 6*TILE_WIDTH;

						if (FRGPointInRect(x,frogger[index].y,&rOb)) {
							FRGSetActorState(STATE_SQUASHED,&frogger[index]);
							return;
						}
					}
					if ((i == 1 || i == 4) && turtles[j+((i==1)?0:3)].state == STATE_SUBMERGED)	// Check if turtles are submerged
						continue;
					if (!regions[i].vx.counter)
						frogger[index].x -= regions[i].vx.vel;
					drowning = false;
				}
			}
		}

		if (drowning)
			FRGSetActorState(STATE_DROWNING,&frogger[index]);
	} else if (frogger[index].y > 120 && frogger[index].y < 184) {	// Road region
		bool squashed = false;
		rect rFrogger, rOb;

		FRGCalcFroggerBounds(&rFrogger,&frogger[index]);

		for (u8 i = 5; !squashed && i < 9; ++i) {
			for (u8 j = 0; !squashed && j < regions[i].obstacleCount; ++j) {
				memcpy_P(&rOb,regions[i].obstacles+j,sizeof(rOb));
				rOb.left = FRGClampToXAxis(rOb.left-regions[i].ss->scrollX);
				rOb.right = FRGClampToXAxis(rOb.right-regions[i].ss->scrollX);

				if (rOb.left > rOb.right)
					rOb.left = 0;

				if (FRGRectsIntersect(&rFrogger,&rOb))
					squashed = true;
			}
		}

		if (squashed)
			FRGSetActorState(STATE_SQUASHED,&frogger[index]);
	}
}


void FRGTestActorCollisions(actor *a) {
	rect rSnake,rFrogger;

	rSnake.left = snake.x-6;
	rSnake.right = snake.x+6;
	rSnake.top = snake.y-6;
	rSnake.btm = snake.y+6;
	FRGCalcFroggerBounds(&rFrogger,a);

	if (rSnake.left > rSnake.right)
		rSnake.left = 0;
	if (FRGRectsIntersect(&rFrogger,&rSnake))
		FRGSetActorState(STATE_SQUASHED,a);
}


void FRGDrawHome(int index) {
	const char *map = 0;

	switch (homeStates[index]) {
		case STATE_EMPTY: map = mapEmptyHome; break;
		case STATE_MATE: map = mapMateHome; break;
		case STATE_FLY: map = mapFlyHome; break;
		case STATE_CROC: map = mapCrocHome2; break;
		case STATE_OCCUPIED_P1: map = mapFrogHomeP1; break;
		case STATE_OCCUPIED_P2: map = mapFrogHomeP2; break;
		default:
			return;
	}
	DrawMap2(2+4*index,1,map);
}


void FRGSetHomeState(int index, u8 state) {
	homeStates[index] = state;
	FRGDrawHome(index);
}


void FRGHomesThink(void) {
	static u16 homeTimer = 0;

	if (++homeTimer == 480)
		homeTimer = PRNG_NEXT()&127;
	else
		return;

	for (u8 i = 0, temp = prng%5; i < 5; ++i) {
		if (homeStates[i]&STATE_OCCUPIED) {
			continue;
		} else if ((homeStates[i]&STATE_EMPTY) == 0) {
			FRGSetHomeState(i,STATE_EMPTY);
		} else if (i == temp) {
			if (players == 2 || prng < 128)
				FRGSetHomeState(i,STATE_CROC);
			else if (prng < 196)
				FRGSetHomeState(i,STATE_FLY);
			else
				FRGSetHomeState(i,STATE_MATE);
		}
	}
}


void FRGSnakeThink(void) {
	if ((gameTicks&7) == 0 && (PRNG_NEXT()&7) == 0) {
		snake.dir = (snake.dir == DIR_LEFT)?DIR_RIGHT:DIR_LEFT;
		FRGLoadActorAnimations(&snake);
	}
}


void FRGMoveActor(actor *a) {
	switch (a->type) {
		case ACTOR_PLAYER:
			if (a->state == STATE_JUMPING) {
				if (a->dir == DIR_UP)
					--a->y;
				else if (a->dir == DIR_DOWN)
					++a->y;
				else if (a->dir == DIR_RIGHT)
					++a->x;
				else if (a->dir == DIR_LEFT)
					--a->x;
			}

			if (a->y > 192)
				a->y = 192;
			if (a->x > 200)
				a->x = 0;
			else if (a->x > 132)
				a->x = 132;
			MoveSprite(a->sprite,a->x,a->y,a->wid,a->hgt);
			break;
		case ACTOR_SNAKE:
			if (a->state == STATE_INACTIVE)
				return;
			a->x += (a->dir == DIR_LEFT)?-1:1;
			MoveSprite(a->sprite,a->x,a->y,a->wid,a->hgt);
			break;
	}
}


void FRGAnimateActor(actor *a) {
	if (++a->cur.counter >= a->cur.duration) {
		a->cur.counter = 0;

		if (++a->cur.frame >= a->anim.frameCount)
			a->cur.frame = 0;
		a->cur.duration = pgm_read_byte(frameDurations+a->anim.frameDurationsIndex+a->cur.frame);
		MapSprite(a->sprite,a->anim.frames+a->anim.frameSize*pgm_read_byte(frameSequences+a->anim.frameSequenceIndex+a->cur.frame));
	}

	switch (a->type) {
		case ACTOR_PLAYER:
			switch (a->state) {
				case STATE_JUMPING:
					if (a->cur.frame == 0 && a->cur.counter == 0)
						FRGSetActorState(STATE_IDLE,a);
					break;
				case STATE_DROWNING:
				case STATE_SQUASHED:
				case STATE_STRANDED:
					if (a->cur.frame == 0 && a->cur.counter == 0) {
						FRGAddTime(MAX_TIME);
						FRGDrawTime();
						FRGResetFrogger((a==frogger)?0:1);
					}
					break;
			}
			break;
		case ACTOR_SNAKE:
		default:
			break;
	}
}


void FRGLoadActorAnimations(actor *a) {
	int index = 0;

	switch (a->type) {
		case ACTOR_PLAYER:
			switch (a->state) {
				case STATE_IDLE:
					index = a->dir;
					index += (a==frogger)?0:SPRITES_OFFSET_P2;
					break;
				case STATE_JUMPING:
					index = 4+a->dir;
					index += (a==frogger)?0:SPRITES_OFFSET_P2;
					break;
				case STATE_DROWNING:
					index = 8;
					break;
				case STATE_SQUASHED:
				case STATE_STRANDED:
					index = 17;
					break;
			}
			break;
		case ACTOR_SNAKE:
			if (a->state == STATE_INACTIVE)
				return;
			index = (a->dir == DIR_RIGHT)?18:19;
			break;
	}

	a->cur.frame = 0;
	a->cur.counter = 0;
	memcpy_P(&a->anim, animations+index, sizeof(animation));
	a->cur.duration = pgm_read_byte(frameDurations+a->anim.frameDurationsIndex+a->cur.frame);
	MapSprite(a->sprite,a->anim.frames+a->anim.frameSize*pgm_read_byte(frameSequences+a->anim.frameSequenceIndex+a->cur.frame));
}


void FRGSetActorState(u8 state, actor *a) {
	if (a->state == state)
		return;
	a->state = state;
	FRGLoadActorAnimations(a);

	switch (a->type) {
		case ACTOR_PLAYER:
			if (state == STATE_JUMPING) {
				FRGTriggerSfx(SFX_HOP);

				if (a->dir == DIR_UP)
					FRGAddScore(10);
				else if (a->dir == DIR_DOWN && ((a->y>>3) < 24))
					FRGAddScore(-10);
			} else if (state&(STATE_DROWNING|STATE_SQUASHED|STATE_STRANDED)) {
				FRGAddLives(a,-1);
				FRGDrawLives(a);

				if(a->lives == 0){
					StopSong();
					TriggerNote(0,2,80,1);
					TriggerNote(1,2,80,1);
					TriggerNote(2,2,80,1);
				}
				FRGTriggerSfx(SFX_DEAD);
			} else if (state&STATE_INACTIVE) {
				MoveSprite(a->sprite,HIDE_X,a->y,a->wid,a->hgt);
			} else if (state&STATE_HOME) {
				++a->temp;
				FRGResetFrogger((a==frogger)?0:1);
			}
			break;
		case ACTOR_SNAKE:
			if (state == STATE_INACTIVE)
				MoveSprite(a->sprite,HIDE_X,a->y,a->wid,a->hgt);
			else
				MoveSprite(a->sprite,a->x,a->y,a->wid,a->hgt);
			break;
		default:
			break;
	}
}


void FRGResetFrogger(u8 index) {
	frogger[index].dir = DIR_UP;
	frogger[index].y = 192;

	if (players == 1)
		frogger[index].x = 66;
	else if (index == 0)
		frogger[index].x = 33;
	else
		frogger[index].x = 99;

	frogger[index].state = 0; // Force state init
	FRGSetActorState(STATE_IDLE,&frogger[index]);
	FRGMoveActor(&frogger[index]);
}


void FRGPrint(u8 x, u8 y, const char *msg) {
	char c = pgm_read_byte(msg++);

	while (c) {
		SetTile(x++,y,ALPHA_INDEX+c-'A');
		c = pgm_read_byte(msg++);
	}
}


void FRGPrintCursor(u8 pos) {
	if (pos == 0) {
		SetTile(6,20,TILE_ROAD);
		SetTile(6,18,TILE_LIVES_P1);
	} else {
		SetTile(6,18,TILE_ROAD);
		SetTile(6,20,TILE_LIVES_P2);
	}
}


void FRGDrawTitleScreen(void) {
	ClearVram();
	DrawMap2(4,5,mapTitleFrogger);
	DrawMap2(5,9,mapTitleFrog);
	FRGPrintNumber(7,18,1,1);
	FRGPrint(9,18,strPlayer);
	FRGPrintNumber(7,20,2,1);
	FRGPrint(9,20,strPlayers);
}


void FRGSetGameState(u8 state) {
	switch (state) {
		case STATE_TITLE:
			StopSong();
			level = 0;
			FRGSaveHiScore();
			FRGSetActorState(STATE_INACTIVE,frogger);
			FRGSetActorState(STATE_INACTIVE,frogger+1);
			FRGSetActorState(STATE_INACTIVE,&snake);
			FRGDrawTitleScreen();
			FRGPrintCursor(0);
			// 1UP
			SetTile(1,0,DIGIT_INDEX+1);
			SetTile(2,0,ALPHA_INDEX+'U'-'A');
			SetTile(3,0,ALPHA_INDEX+'P'-'A');
			// Score
			FRGDrawScore();
			// HI
			SetTile(12,0,ALPHA_INDEX+'H'-'A');
			SetTile(13,0,ALPHA_INDEX+'I'-'A');
			// Hi Score
			FRGDrawHiScore();

			for (u8 i = 0; i < SCREEN_SECTIONS_COUNT; ++i)
				screenSections[i].scrollX = 0;
			break;
		case STATE_PLAYING:
			if (gameState == STATE_LVL_CLEAR || gameState == STATE_TITLE) {
				frogger[0].temp = 0;
				frogger[1].temp = 0;
				FRGAddTime(MAX_TIME);
				FRGSetActorState(STATE_ACTIVE,&snake);
				FRGResetFrogger(0);

				if (players == 2)
					FRGResetFrogger(1);
				if (gameState == STATE_TITLE) {
					FRGAddLives(frogger,(players == 1)?6:MAX_LIVES);
					FRGAddScore(-score);
					scoreBonus = 0;

					if (players == 2)
						FRGAddLives(frogger+1,MAX_LIVES);
				}
				FRGDrawLevel();
			}
			break;
		case STATE_LVL_CLEAR:
			if (players == 2) {
				WaitVsync(60);

				if (frogger[0].temp > frogger[1].temp) {
					FRGAddLives(frogger+1,(int)frogger[1].temp-frogger[0].temp);
					FRGDrawLives(frogger+1);
				} else {
					FRGAddLives(frogger,(int)frogger[0].temp-frogger[1].temp);
					FRGDrawLives(frogger);
				}
				tempTimer = 3*HZ;
				FRGTriggerSfx(SFX_LIFE_LOSS_2P);
			}
			break;
		case STATE_GAME_OVER:
			if (players == 1) {
				FRGSetActorState(STATE_INACTIVE,&snake);
				MoveSprite(SPRITE_P1,HIDE_X,0,frogger[0].wid,frogger[0].hgt);
				FRGPrint(7,14,strGame);
				FRGPrint(12,14,strOver);
				tempTimer = 4*HZ;
				FRGAddLives(frogger,-frogger[0].lives);
				StartSong(song_end2);
			} else {
				FRGSetActorState(STATE_INACTIVE,&snake);
				MoveSprite(SPRITE_P1,HIDE_X,0,frogger[0].wid,frogger[0].hgt);
				MoveSprite(SPRITE_P2,HIDE_X,0,frogger[0].wid,frogger[0].hgt);

				if (frogger[0].lives != frogger[1].lives) {
					SetTile(7,14,ALPHA_INDEX+'P'-'A');
					SetTile(8,14,DIGIT_INDEX+((frogger[0].lives > frogger[1].lives)?1:2));
					FRGPrint(10,14,strWinner);
				} else {
					FRGPrint(10,14,strDraw);
				}
				FRGAddLives(frogger,-frogger[0].lives);
				FRGAddLives(frogger+1,-frogger[1].lives);
				tempTimer = 4*HZ;
				StartSong(song_end);
			}
			beepTimer = 0;
			break;
		case STATE_PAUSED:
		default:
			break;
	}
	gameState = state;
}


void FRGCheckGameState(void) {
	if (players == 1 && gameState == STATE_PLAYING && time == 0) {
		FRGSetActorState(STATE_SQUASHED,&frogger[0]);
	} else if (players == 1 && frogger[0].lives == 0 && (frogger[0].state&(STATE_DROWNING|STATE_SQUASHED|STATE_STRANDED)) == 0) {
		FRGSetGameState(STATE_GAME_OVER);
	} else if (players == 2 && (frogger[0].lives == 0 || frogger[1].lives == 0)) {
		FRGSetGameState(STATE_GAME_OVER);
	} else if (gameState == STATE_PLAYING) {
		u8 i;

		for (i = 0; i < 5; ++i)
			if ((homeStates[i]&STATE_OCCUPIED) == 0)
				break;
		if (i == 5)
			FRGSetGameState(STATE_LVL_CLEAR);
	}
}


void FRGPlayAmbientSfx(void) {
	static u8 ambientTimer = 240;

	if (--ambientTimer == 0)
		ambientTimer = 240;

	if ((ambientTimer == 16 || ambientTimer == 32) && ((frogger[0].y > 120 && frogger[0].y < 184) ||
			(players == 2 && frogger[1].y > 120 && frogger[1].y < 184))) {
		beepTimer = 10;
		FRGTriggerSfx(SFX_BEEP);
	}
}


void FRGTriggerSfx(u8 index) {
	switch (index) {
		case SFX_HOP: if (!beepTimer) TriggerNote(2,12+(prng&3),80,SFX_HOP_VOL); break;
		case SFX_BEEP: TriggerNote(2,SFX_BEEP,80,SFX_BEEP_VOL); break;
		case SFX_DEAD: TriggerNote(2,SFX_DEAD,80,SFX_DEAD_VOL); break;
		case SFX_BONUS: TriggerNote(2,SFX_BONUS,80,SFX_BONUS_VOL); break;
		case SFX_TIME_BONUS: TriggerNote(2,SFX_TIME_BONUS,80,SFX_TIME_BONUS_VOL); break;
		case SFX_LIFE_LOSS_2P: TriggerNote(2,SFX_LIFE_LOSS_2P,80,SFX_LIFE_LOSS_2P_VOL); break;
	}
}


int main(void) {
	u8 menuPos = 0;
	u16 btnHeld[2] = {0,0};    		// Buttons that are held right now
	u16 btnPressed[2] = {0,0};  	// Buttons that were pressed this frame

	// General
	FRGLoadHiScore();
	InitMusicPlayer(patches);
	SetMasterVolume(127);
	tracks[0].trackVol=104;
	tracks[1].trackVol=104;
	tracks[2].trackVol=191;

	SetTileTable(tileset);
	SetSpritesTileTable(spriteset);
	SetSpritesOptions(SPR_OVERFLOW_ROTATE);
	prng = 1;
	// P1
	frogger[0].type = ACTOR_PLAYER;
	frogger[0].sprite = SPRITE_P1;
	frogger[0].wid = 2;
	frogger[0].hgt = 2;
	// P2
	frogger[1].type = ACTOR_PLAYER;
	frogger[1].sprite = SPRITE_P2;
	frogger[1].wid = 2;
	frogger[1].hgt = 2;
	// Snake
	snake.type = ACTOR_SNAKE;
	snake.sprite = SPRITE_SNAKE;
	snake.x = 160;
	snake.y = 112;
	snake.wid = 2;
	snake.hgt = 2;
	snake.dir = DIR_RIGHT;
	// Gator
	memcpy_P(&gator.anim, animations+28, sizeof(animation));
	gator.cur.duration = pgm_read_byte(frameDurations+gator.anim.frameDurationsIndex+gator.cur.frame);

	for (u8 i = 0; i < SCREEN_SECTIONS_COUNT; ++i) {
		screenSections[i].tileTableAdress = tileset;
		screenSections[i].flags=SCT_PRIORITY_SPR;
	}
	screenSections[HOME_REGION].height = 24;
	screenSections[HOME_REGION].vramBaseAdress=vram;
	screenSections[PAVEMENT_REGION].height = 16;
	screenSections[PAVEMENT_REGION].vramBaseAdress=vram+13*VRAM_TILES_H;
	screenSections[GRASS_REGION].height = 24;
	screenSections[GRASS_REGION].vramBaseAdress=vram+23*VRAM_TILES_H;

	for (u8 i = 0; i < MOVING_REGIONS_COUNT; ++i) {
		if (i < 5) {
			regions[i].ss = &screenSections[i+1];
			regions[i].ss->vramBaseAdress=vram+(2*i+3)*VRAM_TILES_H;
		} else {
			regions[i].ss = &screenSections[i+2];
			regions[i].ss->vramBaseAdress=vram+(2*i+5)*VRAM_TILES_H;
		}
		regions[i].ss->height = 16;
	}

	for (u8 i = 0; i < 6; ++i)
		FRGSetTurtleState(i, STATE_SWIMMING);

	FRGSetGameState(STATE_TITLE);

	while(1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();
			++gameTicks;
			btnPressed[0] = btnHeld[0];
			btnPressed[1] = btnHeld[1];
			btnHeld[0] = ReadJoypad(0);
			btnHeld[1] = ReadJoypad(1);
			btnPressed[0] = btnHeld[0]&(btnHeld[0]^btnPressed[0]);
			btnPressed[1] = btnHeld[1]&(btnHeld[1]^btnPressed[1]);

			switch (gameState) {
				case STATE_TITLE:
					if ((btnPressed[0]|btnPressed[1])&(BTN_START|BTN_A|BTN_B)) {
						players = (menuPos)?2:1;
						FRGSetGameState(STATE_PLAYING);
						StartSong(song);
					} else if ((btnPressed[0]|btnPressed[1])&(BTN_UP|BTN_DOWN)) {
						menuPos = (menuPos)?0:1;
						FRGTriggerSfx(SFX_HOP);
						FRGPrintCursor(menuPos);
					}
					PRNG_NEXT();
					continue;
				case STATE_PAUSED:
					if ((btnPressed[0]|btnPressed[1])&BTN_START)
						FRGSetGameState(STATE_PLAYING);
					continue;
				case STATE_PLAYING:
					if ((btnPressed[0]|btnPressed[1])&BTN_START) {
						FRGSetGameState(STATE_PAUSED);
						continue;
					} else if ((btnPressed[0]|btnPressed[1])&BTN_SELECT) {
						FRGSetGameState(STATE_GAME_OVER);
						continue;
					}
					break;
				case STATE_LVL_CLEAR:
					if (players == 1) {
						if (time) {
							FRGAddTime(-1);
							FRGDrawTime();
							FRGAddScore(1);
							FRGTriggerSfx(SFX_TIME_BONUS);
							tempTimer = HZ;
						} else if (--tempTimer == 0) {
							FRGCheckGameState();

							if (gameState == STATE_LVL_CLEAR)
								FRGSetGameState(STATE_PLAYING);
						}
					} else if (--tempTimer == 0) {
						SetTile(5,0,TILE_LIVES_P1);
						SetTile(15,0,TILE_LIVES_P2);
						FRGCheckGameState();

						if (gameState == STATE_LVL_CLEAR)
							FRGSetGameState(STATE_PLAYING);
					} else {
						if ((tempTimer&15) == 0) {
							if (frogger[1].temp > frogger[0].temp)
								SetTile(5,0,(tempTimer&31)?TILE_LIVES_P1:TILE_ROAD);
							else
								SetTile(15,0,(tempTimer&31)?TILE_LIVES_P2:TILE_ROAD);
						}
					}
					continue;
				case STATE_GAME_OVER:
					if (--tempTimer == 0) {
						FRGSetGameState(STATE_TITLE);
						menuPos = 0;
					}
					continue;
			}

			FRGMoveRegions();

			if ((gameTicks&31) == 0) {
				FRGTurtlesThink();
				FRGAddTime(-1);
				FRGDrawTime();
			}
			FRGAnimateTurtles();
			FRGDrawTurtles();
			FRGGatorThink();

			for (u8 i = 0; i < players; ++i) {
				if (frogger[i].state == STATE_IDLE && (btnHeld[i]&(BTN_UP|BTN_DOWN|BTN_LEFT|BTN_RIGHT))) {
					if (btnHeld[i]&BTN_UP)
						frogger[i].dir = DIR_UP;
					else if (btnHeld[i]&BTN_DOWN)
						frogger[i].dir = DIR_DOWN;
					else if (btnHeld[i]&BTN_LEFT)
						frogger[i].dir = DIR_LEFT;
					else if (btnHeld[i]&BTN_RIGHT)
						frogger[i].dir = DIR_RIGHT;
					FRGSetActorState(STATE_JUMPING,&frogger[i]);
				}
				FRGMoveActor(&frogger[i]);
				FRGAnimateActor(&frogger[i]);
			}

			if (gameTicks&1) {
				FRGMoveActor(&snake);
				FRGAnimateActor(&snake);
			}

			for (u8 i = 0; i < players; ++i) {
				FRGTestBounds(i);
				FRGTestActorCollisions(&frogger[i]);
			}

			FRGHomesThink();
			FRGSnakeThink();

			if (timerBonus0 && (--timerBonus0 == 0))
				MoveSprite(SPRITE_BONUS,HIDE_X,0,2,1);

			FRGCheckGameState();

			if (beepTimer)
				--beepTimer;
			FRGPlayAmbientSfx();
		}
	}
}





