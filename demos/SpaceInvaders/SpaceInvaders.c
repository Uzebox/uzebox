
/*
 *  Space Invaders (for Uzebox)
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

// For the tutorial, see: http://uzebox.org/wiki/index.php?title=Hello_World

/****************************************
 *			Library Dependencies		*
 ****************************************/

#include <uzebox.h>
#include <avr/pgmspace.h>
#include <string.h>


/****************************************
 *			  	 Data					*
 ****************************************/
// Tiles
#include "data/tiles.pic.inc"
#include "data/tiles.map.inc"
// Sprites
#include "data/sprites.pic.inc"
#include "data/sprites.map.inc"
// Music/sfx
#include "data/patches.inc"

/****************************************
 *			  	 Defines				*
 ****************************************/
#define HZ							60
#define SCRN_HGT					(SCREEN_TILES_H * TILE_WIDTH)
#define SCRN_WID					(SCREEN_TILES_V * TILE_HEIGHT)
#define TILE_WID2					(TILE_WIDTH / 2)
#define TILE_HGT2					(TILE_HEIGHT / 2)
#define HIT_QUEUE_SIZE				4
#define PROJECTILE_COUNT			2

// Black area in pixel coordinates (includes score region)
#define BOUNDARY_LEFT				(4 * TILE_WIDTH)
#define BOUNDARY_RIGHT				(26 * TILE_WIDTH)
#define BOUNDARY_TOP				(6 * TILE_HEIGHT)
#define BOUNDARY_BTM				(23 * TILE_HEIGHT)

#define PLAYER_START_LOC_X 			((SCRN_HGT / 2) - (animShip_Width*TILE_WIDTH / 2))
#define PLAYER_START_LOC_Y			(22 * TILE_HEIGHT)
#define PLAYER_START_LIVES			3
#define PLAYER_LIVES_LOC_X			26
#define PLAYER_LIVES_LOC_Y			20
#define PLAYER_SPEED_X				1
#define PLAYER_PROJECTILE_SPD		4

#define ALIENS_PER_ROW				8
#define ALIEN_ROW_COUNT				3
#define ALIEN_COUNT					(ALIENS_PER_ROW * ALIEN_ROW_COUNT)
#define ALIEN_SPACING_X				2	// Distance in tile coordinates
#define ALIEN_SPACING_Y				2	//
#define ALIEN_HIT_DURATION			(HZ / 2)	// Duration to display alien hit image

#define INVADERS_MIN_LOC_X			5
#define INVADERS_MAX_LOC_X			(INVADERS_MIN_LOC_X + 5)
#define INVADERS_MIN_LOC_Y			7
#define INVADERS_MAX_LOC_Y			21
#define INVADERS_HEIGHT				5
#define INVADERS_DISPLAY_STATE_COUNT 4
#define INVADERS_PROJECTILE_SPD		2
#define INVADERS_MOVE_INTERVAL		(2 * HZ / 3)
#define INVADERS_MOVE_INTERVAL_MIN	3
#define INVADERS_PROJ_INTERVAL		(3 * HZ / 2)

#define UFO_START_LOC_X				(25 * TILE_WIDTH)
#define UFO_START_LOC_Y				(6 * TILE_HEIGHT)
#define UFO_SPD						1
#define UFO_MIN_INTERVAL			(2 * HZ)
#define UFO_HIT_TIMER_DURATION		(HZ / 2) // Duration to display ufo hit image
#define UFO_PER_ROUND				3 // Without, the player is rewarded for dragging the round out - not fun

#define SHELTER_LOC_X				6
#define SHELTER_LOC_Y				19
#define SHELTER_COUNT				4
#define SHELTER_LIMIT				3
#define SHELTER_SPACING_X			5

// Black area in tile coordinates (excludes score region)
#define PLAYING_FIELD_LOC_X			4
#define PLAYING_FIELD_LOC_Y			6
#define PLAYING_FIELD_WID			22
#define PLAYING_FIELD_HGT			16

#define SPRITE_PLAYER_SHIP			0
#define SPRITE_PLAYER_PROJ			2 
#define SPRITE_INVADERS_PROJ		3
#define SPRITE_UFO					4

// Indexes into animations data structure in flash
#define ANIM_INDEX_PLAYER_SHIP		0
#define ANIM_INDEX_PLAYER_DEAD		1
#define ANIM_INDEX_PLAYER_SHOOT		2
#define ANIM_INDEX_INVADERS_SHOOT	3

#define PRESS_START_LOC_X			9
#define PRESS_START_LOC_Y			13
#define GAME_OVER_LOC_X				10
#define GAME_OVER_LOC_Y				13

#define ONE_UP_LOC_X				4
#define ONE_UP_LOC_Y				5
#define HISCORE_LOC_X				16
#define HISCORE_LOC_Y				5

#define CLEAR_TILE					0	// Black tile
#define LIVES_CLEAR_TILE			3	// Purple tile to "rub out" extra lives image
#define SCORE_DIGIT_COUNT			7	// Number of digits to display for the score
#define SPACE_INVADERS_ID			12	// Eeprom id
#define DIGIT_OFFSET				67	// Index of first tile of the sequentially-stored numerals in flash

#define DIR_RIGHT					1
#define DIR_LEFT					-1

// Sound effects indexes into patches struct in flash and their volumes
#define SFX_PLAYER_SHOOT			0
#define SFX_VOL_PLAYER_SHOOT		0xf0
#define SFX_PLAYER_HIT				1
#define SFX_VOL_PLAYER_HIT			0xa0
#define SFX_ALIEN_MOVE_A			2
#define SFX_VOL_ALIEN_MOVE_A		0x20
#define SFX_ALIEN_MOVE_B			3
#define SFX_VOL_ALIEN_MOVE_B		0x20
#define SFX_ALIEN_HIT				4
#define SFX_VOL_ALIEN_HIT			0x60
#define SFX_UFO						5
#define SFX_VOL_UFO					0x50

// Utility macros
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define ABS(x) (((x) > 0) ? (x) : -(x))
#define GET_ALIEN_WID() ((ai.dispState == single || ai.dispState == dblVert) ? 1: 2)
#define GET_ALIEN_HGT() ((ai.dispState == single || ai.dispState == dblHoriz) ? 1: 2)
// 8-bit, 255 period LFSR (for generating pseudo-random numbers)
#define PRNG_NEXT() (prng = ((u8)((prng>>1) | ((prng^(prng>>2)^(prng>>3)^(prng>>4))<<7))))

/****************************************
 *			Type declarations			*
 ****************************************/

// Each significant game entity has a FSM to simplify and centralise game logic
typedef enum { 
	title, playing, dead, paused, gameOver
} GameState;

typedef enum {
	playerAlive, playerDead
} PlayerState;

typedef enum {
	alienAlive, alienHit, alienDead
} AlienState;

typedef enum {
	idle, active
} ProjectileState;

typedef enum {
	ufoIdle, ufoActive, ufoHit, ufoDead
} UfoState;


/* Alien/explosion position within a group of four tiles. Aliens require different tile
   rendering configurations depending on their location because they move in half-tile
   increments.

Tile  Alien
 tt    aa
 tt    aa

single  dblHoriz  dblVert  quad
 aatt     taat     tttt    tttt
 aatt     taat     aatt    taat
 tttt     tttt     aatt    taat
 tttt     tttt     tttt    tttt
*/
typedef enum {
	single, dblHoriz, dblVert, quad
} InvaderDisplayState;

typedef enum {
	green, pink, yellow
} AlienType;

typedef struct {
	u8	x;
	u8	y;
} pt;

typedef struct {
	u8 left;
	u8 right;
	u8 top;
	u8 btm;
} rect;

/* Stores aliens/ufos that have been recently hit and are displaying the
   explosion graphic. When their hitTimer expires, the head of the queue
   is removed and painted clear to appear removed. Because the invaders
   have moved on in the meantime, this queue is used to record the region
   where the alien was hit so that the clear is accurate.
*/
typedef struct {
	u8 occupied;
	rect r;
} hitQueue;

typedef struct {
	u8			size;			// wid*hgt
	u8			wid;			// Width of each frame (in tiles)
	u8			hgt;			// Height of each frame (in tiles)
	u8			frameCount;		// # of frames in an animation cycle
	u8 			currFrame;		// The frame that is currently displayed
	u8			disp;			// Displacement counter
	u8			dpf;			// Displacement per frame (scales frame rate to movement speed)
	u8			synch;			// Flag to indicate animation should be synchronized with others of similar type
	const char	*frames;		// Stored in flash
} animation;

typedef struct {
	ProjectileState state;
	pt loc;						// Location in pixels
	char vel;					// Y-Axis velocity in pixels
	u8 sprite;					// Index into sprites data structure
	u8 animIndex;				// Index into animations data structure
	animation anim;				// Current animation (loaded from flash into ram)
} projectile;

typedef struct {
	pt loc;											// Location in tiles
	u8 dmg[mapShelter_Width * mapShelter_Height];	// No. of times each tile can hit before shelter is destroyed
} shelter;

typedef struct {
	PlayerState state;
	pt loc;						// Location in pixels
	u8 spd;						// Amount moved in direction of travel (in pixels)
	u8 lives;					// Remaining lives
	u8 sprite;					// Index into sprites data structure
	u32 score;					// Game score from killing aliens/ufos
	projectile prj;				// Laser cannon
	animation anim;				// Current animation (loaded from flash into ram)
} player;

typedef struct {
	u8 hitTimer;				// Counts down after alien is hit until explosion graphic is removed
	AlienState state;
	AlienType type;				// Different aliens score differently (more flexible to store type than score)
} alien;

typedef struct {
	InvaderDisplayState dispState;
	pt tileLoc;						// Top left tile of group of four display tiles (See enum InvaderDisplayState)
	pt absLoc;						// Tile/2 precision
	char dir;						// Direction of travel (DIR_RIGHT or DIR_LEFT)
	u8 moveTimer;					// Time between moves
	u8 maxMoveTimer;				// Time that moveTimer resets to when it reaches zero (scales with round/level)
	u8 prjTimer;					// Time between shooting at player
	u8 maxPrjTimer;					// Time that prjTimer resets to when it reaches zero
	u8 aliensRemaining;				// No. of aliens left (game ends at zero)
	projectile prj;					// Shot at player each prjTimer period
	u8 attackers[ALIENS_PER_ROW];	// Frontline aliens who shoot at the player
	alien aliens[ALIEN_COUNT];		// Aliens container
} invaders;

typedef struct {
	UfoState state;
	pt loc;						// Location in pixels
	u8 wid;						// Width changes if ufo is half-in/half-out of playing field
	char vel;					// X-Axis velocity in pixels
	u8 hitTimer;				// Counts down after ufo is hit until explosion graphic is removed
	u16 bonus;					// Random bonus between 1k-5k
	u8 roundCount;				// Counts ufo's per round (for purposes of limiting their appearances)
} ufoDetails;

/****************************************
 *			File-level variables		*
 ****************************************/
const animation animations[] PROGMEM = {
	/* Player */
	{2,2,1,1,0,0,0,0,animShip},
	{2,2,1,2,0,0,4,0,animShipDestroyed},
	{1,1,1,1,0,0,0,0,animPlayerShoot},
	/* Enemy */
	{1,1,1,3,0,0,8,0,animInvaderShoot}
};

u8 prng; // Pseudo-random number generator
GameState gameState;
player p1;
invaders ai;
ufoDetails ufo;
shelter shelters[SHELTER_COUNT]; // Red blocks between player and invaders
hitQueue hitQ[4]; // See struct hitQueue
u8 hqIndex;	// Head of hitQ
u32 hiScore;
u32 freeLife; // Free life requirement is adjusted as the game progresses to avoid divisions

// Alien graphics to match their InvaderDisplayState
PGM_P const alienMaps[] PROGMEM = { mapAlienGreen0, mapAlienGreen1, mapAlienGreen2, mapAlienGreen3,
									mapAlienPink0, mapAlienPink1, mapAlienPink2, mapAlienPink3,
									mapAlienYellow0, mapAlienYellow1, mapAlienYellow2, mapAlienYellow3
};

// Alien dead explosion maps to match their InvaderDisplayState
PGM_P const alienDeadMaps[] PROGMEM = { mapAlienDead0, mapAlienDead1, mapAlienDead2, mapAlienDead3 };

// Shelters disintegrate when hit (each section has three hits until dead)
PGM_P const shelterMaps[] PROGMEM = { mapShelterDmg0, mapShelterDmg1, mapShelterDmg2 };

/****************************************
 *			Function declarations		*
 ****************************************/
void LoadHighScore(void);
void SaveHighScore(void);
void FlashPressStart(void);
void InitRound(u8 round);
void PrintPlayerScore(void);
void PrintHiScore(void);
void PrintScores(void);
void PrintNumbers(pt *pts, u8 count, u32 val, u8 digits);
void FillRegion(u8 x, u8 y, u8 width, u8 height, u8 tile);
void ClearAliens(void);
void MoveAliens(void);
void SetInvaderDisplayState(void);
void DrawAliens(void);
void DrawShelters(void);
void DrawLives(void);
void MoveProjectiles(u8 count, projectile **projectiles);
void HideSprite(u8 spriteIndex, u8 wid, u8 hgt);
u8 PointInRect(const pt *p, const rect *r);
void ProcessShelterCollision(projectile *prj);
void ProcessCollisions(void);
void MovePlayer(u16 move);
void PlayerAttack(u16 cmd);
void AliensAttack(void);
pt GetAlienLoc(u8 index);
pt GetAlienLocAbs(u8 index);
void AnimateProjectiles(u8 count, projectile **projectiles);
void DrawProjectiles(u8 count, projectile **projectiles);
void AnimatePlayer(void);
void DrawPlayer(void);
void SetPlayerState(PlayerState state);
void SetAlienState(u8 index, AlienState state);
void SetProjectileState(projectile *p, ProjectileState state, char vel, const pt *loc);
void SetUfoState(UfoState state);
void UpdateUfo(void);
void UfoBonusRoll(void);
void MoveUfo(void);
void DrawUfo(void);
void AdjustScore(int val);
u8 EnqueueHitAlien(const rect *r);
rect* DequeueHitAlien(void);
void UpdateAttackers(void);
void SpaceInvadersMapSprite(u8 index, u8 wid, u8 hgt, const char *map, u8 spriteFlags);
u8 InvadersBreachedShelters(void);

/****************************************
 *			Function definitions		*
 ****************************************/

/**
	Loads hi score from eeprom (requires a formatted and non-full eeprom).
*/
void LoadHighScore(void) {
	struct EepromBlockStruct ebs;

	ebs.id = SPACE_INVADERS_ID;

	if (!isEepromFormatted() || EepromReadBlock(SPACE_INVADERS_ID, &ebs))
		return;
	hiScore = 0;

	for (u8 i = 0; i < 4; i++)
		hiScore |= ((u32)(ebs.data[i]))<<(24-(i<<3));
}

/**
	Saves the current hi score to eeprom (requires a formatted and non-full eeprom).
*/
void SaveHighScore(void) {
	struct EepromBlockStruct ebs;

	ebs.id = SPACE_INVADERS_ID;

	if (!isEepromFormatted())
		return;
	for (u8 i = 0; i < 4; i++)
		ebs.data[i] = hiScore>>(24-i*8);
	EepromWriteBlock(&ebs);
}

/**
	Animates the "Press Start" message.
*/
void FlashPressStart(void) {
	static u8 flashState = 1;

	if (flashState)
		DrawMap2(PRESS_START_LOC_X, PRESS_START_LOC_Y, mapPressStart);
	else
		FillRegion(PRESS_START_LOC_X, PRESS_START_LOC_Y, mapPressStart_Width, mapPressStart_Height, CLEAR_TILE);
	flashState = !flashState;
}

/**
	Initialise game entities' state for new round.

	@param round the round number to initialise (influences state and timers).
*/
void InitRound(u8 round) {
	for (u8 i = 0; i < HIT_QUEUE_SIZE; i++)
		hitQ[i].occupied = 0;
	ai.dispState = single;
	ai.tileLoc = (pt){ INVADERS_MIN_LOC_X, INVADERS_MIN_LOC_Y };
	ai.absLoc = (pt){ ai.tileLoc.x<<3, ai.tileLoc.y<<3 };
	ai.dir = DIR_RIGHT;
	ai.moveTimer = ai.maxMoveTimer = MAX(INVADERS_MOVE_INTERVAL-(round<<2), INVADERS_MOVE_INTERVAL_MIN);
	ai.prjTimer = ai.maxPrjTimer = INVADERS_PROJ_INTERVAL-(round<<2);
	ai.aliensRemaining = ALIEN_COUNT;
	SetProjectileState(&ai.prj, idle, 0, 0);
	ai.prj.sprite = SPRITE_INVADERS_PROJ;
	ai.prj.animIndex = ANIM_INDEX_INVADERS_SHOOT;
	memcpy_P(&ai.prj.anim, animations + ANIM_INDEX_INVADERS_SHOOT, sizeof(ai.prj.anim));

	// Init to front-line aliens
	for (u8 i = 0; i < ALIENS_PER_ROW; i++)
		ai.attackers[i] = i + ALIEN_COUNT - ALIENS_PER_ROW;

	for (u8 i = 0; i < ALIEN_COUNT; i++) {
		SetAlienState(i, alienAlive);
		ai.aliens[i].type = i / ALIENS_PER_ROW;
	}

	for (u8 i = 0; i < SHELTER_COUNT; i++) {
		shelters[i].loc = (pt){SHELTER_LOC_X + i * SHELTER_SPACING_X, SHELTER_LOC_Y};

		for (u8 j = 0; j < (mapShelter_Width * mapShelter_Height); j++) {
			if (j != 4)
				shelters[i].dmg[j] = 0;
			else
				shelters[i].dmg[j] = SHELTER_LIMIT;
		}
	}

	if (round == 0) {
		p1.loc.x = PLAYER_START_LOC_X;
		p1.loc.y = PLAYER_START_LOC_Y;
		p1.score = 0;
		p1.lives = PLAYER_START_LIVES;
		freeLife = 25000;
		HideSprite(0, MAX_SPRITES, 1);
		SetUfoState(ufoIdle);
	} else {
		HideSprite(SPRITE_PLAYER_PROJ, PROJECTILE_COUNT, 1);
	}

	SetPlayerState(playerAlive);
	p1.spd = PLAYER_SPEED_X;
	p1.sprite = SPRITE_PLAYER_SHIP;
	p1.prj.sprite = SPRITE_PLAYER_PROJ;
	p1.prj.animIndex = ANIM_INDEX_PLAYER_SHOOT;
	SetProjectileState(&p1.prj, idle, 0, 0);
	ufo.roundCount = 0;

	// Clear playing field
	DrawMap2(0, 0, mapBackground);

	// Populate playing field
	DrawAliens();
	DrawShelters();
	DrawLives();
	DrawMap2(ONE_UP_LOC_X, ONE_UP_LOC_Y, map1Up);
	DrawMap2(HISCORE_LOC_X, HISCORE_LOC_Y, mapHi);
	PrintPlayerScore();
	PrintHiScore();
	gameState = playing;
}

/**
	Fills a region with the tile specified (tile-based coordinates).

	@param x the x-axis position to begin the fill.
	@param y the y-axis position to begin the fill.
	@param width how far along the x-axis from x to fill.
	@param height how far along the y-axis from y to fill.
	@param the tile to be used for the fill.
*/
void FillRegion(u8 x, u8 y, u8 width, u8 height, u8 tile) {
	for (u8 i = y; i < (y + height); i++) {
		for (u8 j = x; j < (x + width); j++) {
			SetTile(j, i, tile);
		}
	}
}

/**
	Removes the aliens from the playing field.
*/
void ClearAliens(void) {
	for (u8 i = 0; i < ALIEN_ROW_COUNT; i++) {
		for (u8 j = 0; j < ALIENS_PER_ROW; j++) {
			if (ai.aliens[i * ALIENS_PER_ROW + j].state == alienAlive)
				FillRegion(ai.tileLoc.x + j * ALIEN_SPACING_X, ai.tileLoc.y + i * ALIEN_SPACING_Y, 
					GET_ALIEN_WID(), GET_ALIEN_HGT(), CLEAR_TILE);
		}
	}
}

/**
	Edges the aliens across the screen and down towards the player.
*/
void MoveAliens(void) {
	if (--ai.moveTimer)
		return;
	ai.moveTimer = ai.maxMoveTimer;
	ai.absLoc.x += TILE_WID2 * ai.dir;

	if ((ai.dir == DIR_RIGHT && (ai.absLoc.x&7) == 0) || (ai.dir == DIR_LEFT && (ai.absLoc.x&7)))
		ai.tileLoc.x += ai.dir;
	if ((ai.dir == DIR_RIGHT && ai.absLoc.x > (INVADERS_MAX_LOC_X<<3)) || (ai.dir == DIR_LEFT && ai.absLoc.x < (INVADERS_MIN_LOC_X<<3))) {
		ai.dir *= -1;
		ai.absLoc.y += TILE_HGT2;
		
		if ((ai.absLoc.y&7) == 0)
			++ai.tileLoc.y;
	}
	SetInvaderDisplayState();

	switch (ai.dispState) {
		case single:
		case dblVert:
			TriggerNote(0,SFX_ALIEN_MOVE_A, 75, SFX_VOL_ALIEN_MOVE_A);
			//TriggerFx(SFX_ALIEN_MOVE_A, SFX_VOL_ALIEN_MOVE_A, true);
			break;
		case dblHoriz:
		case quad:
			TriggerNote(0,SFX_ALIEN_MOVE_B, 75, SFX_VOL_ALIEN_MOVE_B);
			//TriggerFx(SFX_ALIEN_MOVE_B, SFX_VOL_ALIEN_MOVE_B, true);
			break;
	}
}

/**
	Sets the display state based on the invader's absolute location.
*/
void SetInvaderDisplayState(void) {
	if (ai.absLoc.x&7) {
		if (ai.absLoc.y&7)
			ai.dispState = quad;
		else
			ai.dispState = dblHoriz;
	} else {
		if (ai.absLoc.y&7)
			ai.dispState = dblVert;
		else
			ai.dispState = single;
	}
}

/**
	Prints the aliens to vram based on their state.
*/
void DrawAliens(void) {
	AlienType type;
	AlienState aState;

	for (u8 i = 0; i < ALIEN_ROW_COUNT; i++) {
		for (u8 j = 0; j < ALIENS_PER_ROW; j++) {
			aState = ai.aliens[i * ALIENS_PER_ROW + j].state;
			type = ai.aliens[i * ALIENS_PER_ROW + j].type;

			if (aState == alienAlive) {
				DrawMap2(ai.tileLoc.x + j * ALIEN_SPACING_X, ai.tileLoc.y + i * ALIEN_SPACING_Y, 
					(const char*)pgm_read_word(alienMaps + type * INVADERS_DISPLAY_STATE_COUNT + ai.dispState));
			} else if (aState == alienHit) {
				if (--ai.aliens[i * ALIENS_PER_ROW + j].hitTimer == 0)
					SetAlienState(i * ALIENS_PER_ROW + j, alienDead);
			}
		}
	}
}

/**
	Prints the shelters to vram.
*/
void DrawShelters(void) {
	for (u8 i = 0; i < SHELTER_COUNT; i++)
		DrawMap2(shelters[i].loc.x, shelters[i].loc.y, mapShelter);
}

/**
	Prints and erases the player's extra lives.
*/
void DrawLives(void) {
	if (p1.lives == 0)
		return;
	for (u8 i = 0; i < (p1.lives - 1); i++)
		DrawMap2(PLAYER_LIVES_LOC_X, PLAYER_LIVES_LOC_Y - i, mapLives);
	FillRegion(PLAYER_LIVES_LOC_X, PLAYER_LIVES_LOC_Y - (p1.lives - 1), mapLives_Width, mapLives_Height, LIVES_CLEAR_TILE);
}

/**
	Prints the player's score to vram.
*/
void PrintPlayerScore(void) {
	PrintNumbers(&(pt){ONE_UP_LOC_X + map1Up_Width, ONE_UP_LOC_Y}, 1, p1.score, SCORE_DIGIT_COUNT);
}

/**
	Prints the hi score to vram.
*/
void PrintHiScore(void) {
	PrintNumbers(&(pt){HISCORE_LOC_X + mapHi_Width, HISCORE_LOC_Y}, 1, hiScore, SCORE_DIGIT_COUNT);
}

/**
	Prints both the player's score and the hi score to vram.
*/
void PrintScores(void) {
	pt pts[2] = { {ONE_UP_LOC_X + map1Up_Width, ONE_UP_LOC_Y}, {HISCORE_LOC_X + mapHi_Width, HISCORE_LOC_Y} };
	PrintNumbers(pts, 2, hiScore, SCORE_DIGIT_COUNT);
}

/**
	Prints to vram val as both the player's score and the hi score. This
	function saves cycles for when these scores are equal.

	@param pts the locations to print the scores (tile-based).
	@param count the number of locations pointed to by pts.
	@param val the value to print.
	@param digits the number of digits to print for each score.
*/
void PrintNumbers(pt *pts, u8 count, u32 val, u8 digits) {
	u8 numeral;

	for (u8 i = 0; i < digits; i++, val /= 10) {
		numeral = val % 10;

		for (u8 j = 0; j < count; j++)
			SetTile(pts[j].x + digits - i, pts[j].y, DIGIT_OFFSET + numeral);
	}
}

/**
	Moves projectiles along the y-axis.

	@param count the number of projectiles pointers pointed to by projectiles
	@param projectiles the projectiles to move
*/
void MoveProjectiles(u8 count, projectile **projectiles) {
	for (u8 i = 0; i < count; i++) {
		if (projectiles[i]->state == active)
			projectiles[i]->loc.y += projectiles[i]->vel;
	}
}

/**
	Moves a sprite array's position offscreen so that it is not drawn.
*/
void HideSprite(u8 spriteIndex, u8 wid, u8 hgt) {
	u8 size = wid*hgt;

	for (int i = 0; i < size; i++)
		MoveSprite(spriteIndex+i,SCREEN_TILES_H<<3,0,1,1);
}

/**
	Determines if a point lies within the bounds of a rectangle.

	@param p the point to test.
	@param r the rectangle to test.
	@return the result of the test.
*/
u8 PointInRect(const pt *p, const rect *r) {
	return p->x >= r->left && p->x < r->right && p->y >= r->top && p->y < r->btm;
}

/**
	Adds damage to the shelter section at point p for the shelter at index.

	@param p the point at which the damage should be applied.
	@param index the index of the shelter to which the damage should be applied.
	@return whether or not the shelter was destroyed.
*/
u8 DestroyShelter(pt p, u8 index) {
	pt sLoc = shelters[index].loc;

	p.x >>= 3;
	p.y >>= 3;
	p.x -= sLoc.x;
	p.y -= sLoc.y;

	u8 *dmg = &shelters[index].dmg[p.x + p.y * mapShelter_Width];

	if (*dmg < SHELTER_LIMIT) {
		DrawMap2(sLoc.x + p.x, sLoc.y + p.y, (const char*)pgm_read_word(shelterMaps + *dmg));
		*dmg += 1;
		return 0;
	} else {
		return 1;
	}
}

/**
	Determines if a projectile has collided with a shelter. If there is a collision,
	the shelter has damage applied to it and the projectile is destroyed.

	@param prj the projectile to test for collision.
*/
void ProcessShelterCollision(projectile *prj) {
	if (prj->state != active)
		return;
	rect shelterRect = (rect){ 0, 0, SHELTER_LOC_Y<<3, (SHELTER_LOC_Y<<3) + (mapShelter_Height<<3) };

	for (u8 i = 0; i < SHELTER_COUNT; i++) {
		shelterRect.left = shelters[i].loc.x<<3;
		shelterRect.right = shelterRect.left + (mapShelter_Width<<3);
		
		if (PointInRect(&prj->loc, &shelterRect)) {
			if (!DestroyShelter(prj->loc, i)) {
				SetProjectileState(prj, idle, 0, 0);
				break;
			}
		}
	}	
}

/**
	Determines if the player's projectile has collided with an alien. If there
	is a collision, the projectile is destroyed and the alien is marked as "hit".
*/
void ProcessInvadersCollision(void) {
	if (p1.prj.state != active)
		return;
	u8 hit = 0;
	pt relativeLoc = { p1.prj.loc.x - ai.absLoc.x, p1.prj.loc.y - ai.absLoc.y };

	switch (ai.dispState) {
		case single:
			if (((relativeLoc.x&15) < 8) && ((relativeLoc.y&15) < 8))
				hit = 1;
			break;
		case dblHoriz:
			if ((((relativeLoc.x&15) > 3) || ((relativeLoc.x&15) < 12)) && ((relativeLoc.y&15) < 8))
				hit = 1;
			break;
		case dblVert:
			if (((relativeLoc.x&15) < 8) && (((relativeLoc.y&15) > 3) || ((relativeLoc.y&15) < 12)))
				hit = 1;
			break;
		case quad:
			if ((((relativeLoc.x&15) > 3) || ((relativeLoc.x&15) < 12)) && (((relativeLoc.y&15) > 3) || ((relativeLoc.y&15) < 12)))
				hit = 1;
			break;
	}

	if (hit) {
		if ((relativeLoc.x / 16) < ALIENS_PER_ROW) {	// Prevents high x with low y from false positive
			u8 index = (relativeLoc.x / 16) + (relativeLoc.y / 16) * ALIENS_PER_ROW;

			if (index < ALIEN_COUNT && ai.aliens[index].state == alienAlive) {
				SetAlienState(index, alienHit);
				SetProjectileState(&p1.prj, idle, 0, 0);
			}
		}
	}
}

/**
	Routes all complex collision checks and internally processes simple
	collision checks.
*/
void ProcessCollisions(void) {
	if (p1.state == playerDead)
		return;
	rect r = { p1.loc.x, p1.loc.x + (animShip_Width<<3), p1.loc.y, p1.loc.y + (animShip_Height<<3) };

	// Check for collision between invader projectile and player ship
	if (ai.prj.state == active) {
		if (PointInRect(&ai.prj.loc, &r)) {
			SetPlayerState(playerDead);
			return;
		}
	}

	// Check for collision between invader projectile and shelters
	ProcessShelterCollision(&ai.prj);
	
	// Check for collision between invader projectile and game boundary
	if (ai.prj.state == active) {
		if (ai.prj.loc.y >= BOUNDARY_BTM)
			SetProjectileState(&ai.prj, idle, 0, 0);
	}

	// Check for collision between player projectile and shelters
	ProcessShelterCollision(&p1.prj);

	// Check for collision between player projectile and invader
	ProcessInvadersCollision();

	// Check for collision between player projectile and ufo
	if (p1.prj.state == active && ufo.state == ufoActive) {
		r.left = ufo.loc.x;
		r.right = r.left + (mapUfo_Width<<3);
		r.top = ufo.loc.y;
		r.btm = r.top + (mapUfo_Height<<3);

		if (PointInRect(&p1.prj.loc, &r)) {
			SetProjectileState(&p1.prj, idle, 0, 0);
			SetUfoState(ufoHit);
		}
	}

	// Check for collision between player projectile and game boundary
	if (p1.prj.state == active) {
		if (p1.prj.loc.y <= BOUNDARY_TOP)
			SetProjectileState(&p1.prj, idle, 0, 0);
	}
}

/**
	Moves the player along the x-axis based on user input.

	@param move the user's input.
*/
void MovePlayer(u16 move) {
	if (p1.state != playerAlive)
		return;
	if (move & BTN_LEFT)
		p1.loc.x -= (p1.loc.x > BOUNDARY_LEFT) ? p1.spd : 0;
	else if (move & BTN_RIGHT)
		p1.loc.x += (p1.loc.x < (BOUNDARY_RIGHT - (animShip_Width<<3))) ? p1.spd : 0;
}

/**
	Fires a laser cannon projectile from the player's ship.

	@param cmd the user's input.
*/
void PlayerAttack(u16 cmd) {
	if (p1.state != playerAlive)
		return;
	if ((cmd & (BTN_A | BTN_B)) && p1.prj.state == idle) {
		SetProjectileState(&p1.prj, active, -PLAYER_PROJECTILE_SPD, &(pt){p1.loc.x + TILE_WIDTH - 1, p1.loc.y - TILE_HEIGHT});
		//TriggerFx(SFX_PLAYER_SHOOT, SFX_VOL_PLAYER_SHOOT, true);
		TriggerNote(1,SFX_PLAYER_SHOOT, 70, SFX_VOL_PLAYER_SHOOT);
	}
}

/**
	Fires a lightning strike from the invaders.
*/
void AliensAttack(void) {
	if (ai.prj.state != idle)
		return;
	if (--ai.prjTimer == 0) {
		u8 atkIndex = PRNG_NEXT()&7;

		while (ai.aliensRemaining && ai.aliens[ai.attackers[atkIndex]].state != alienAlive)
			atkIndex = (atkIndex + ((prng&1) ? 1 : -1))&7;
		ai.prj.loc = GetAlienLocAbs(ai.attackers[atkIndex]);
		ai.prj.loc.y += TILE_HEIGHT;
		ai.prjTimer = ai.maxPrjTimer;
		SetProjectileState(&ai.prj, active, INVADERS_PROJECTILE_SPD, &ai.prj.loc);
	}
}

/**
	Builds an alien's location from its invaders index (tile-based).

	@param index the alien's index in the invaders collection.
	@return the location of the alien at index.
*/
pt GetAlienLoc(u8 index) {
	pt loc;

	loc.x = ai.tileLoc.x + (index&3) * ALIEN_SPACING_X;
	loc.y = ai.tileLoc.y + (index>>3) * ALIEN_SPACING_Y;
	return loc;
}

/**
	Builds an alien's location from its invaders index (tile/2 based).

	@param index the alien's index in the invaders collection.
	@return the absolute location of the alien at index.
*/
pt GetAlienLocAbs(u8 index) {
	pt loc;

	loc.x = ai.absLoc.x + (index&7) * (ALIEN_SPACING_X<<3);
	loc.y = ai.absLoc.y + (index>>3) * (ALIEN_SPACING_Y<<3);
	return loc;
}

/**
	Animates are series of projectiles.

	@param count the number of projectiles pointers pointed to by projectiles.
	@param projectiles the projectiles to animate.
*/
void AnimateProjectiles(u8 count, projectile **projectiles) {
	for (u8 i = 0; i < count; i++) {
		if (projectiles[i]->state == active) {
			if (++projectiles[i]->anim.disp >= projectiles[i]->anim.dpf) {
				projectiles[i]->anim.disp = 0;

				if (++projectiles[i]->anim.currFrame >= projectiles[i]->anim.frameCount)
					projectiles[i]->anim.currFrame = 0;
			}
		}
	}
}

/**
	Prints the maps of a series of projectiles to vram.

	@param count the number of projectiles pointers pointed to by projectiles.
	@param projectiles the projectiles to draw.
*/
void DrawProjectiles(u8 count, projectile **projectiles) {
	for (u8 i = 0; i < count; i++) {
		if (projectiles[i]->state == active) {
			SpaceInvadersMapSprite(projectiles[i]->sprite, projectiles[i]->anim.wid, projectiles[i]->anim.hgt, 
				projectiles[i]->anim.frames + projectiles[i]->anim.currFrame, 0);
			MoveSprite(projectiles[i]->sprite, projectiles[i]->loc.x, projectiles[i]->loc.y, 
				projectiles[i]->anim.wid, projectiles[i]->anim.hgt);
		}
	}
}

/**
	Animates the player ship.
*/
void AnimatePlayer(void) {
	if (++p1.anim.disp >= p1.anim.dpf) {
		p1.anim.disp = 0;

		if (++p1.anim.currFrame >= p1.anim.frameCount)
			p1.anim.currFrame = 0;
	}
}

/**
	Prints the player's ship/explosion frame to vram.
*/
void DrawPlayer(void) {
	SpaceInvadersMapSprite(p1.sprite, p1.anim.wid, p1.anim.hgt, p1.anim.frames + p1.anim.currFrame, 0);
	MoveSprite(p1.sprite, p1.loc.x, p1.loc.y, p1.anim.wid, p1.anim.hgt);
}

/**
	Transitions the player to a new state.

	@param state the state to which the player transitions.
*/
void SetPlayerState(PlayerState state) {
	switch (state) {
		case playerAlive:
			memcpy_P(&p1.anim, animations + ANIM_INDEX_PLAYER_SHIP, sizeof(p1.anim));
			break;
		case playerDead:
			SetProjectileState(&p1.prj, idle, 0, 0);
			SetProjectileState(&ai.prj, idle, 0, 0);
			memcpy_P(&p1.anim, animations + ANIM_INDEX_PLAYER_DEAD, sizeof(p1.anim));
			--p1.lives;
			DrawLives();
			//TriggerFx(SFX_PLAYER_HIT, SFX_VOL_PLAYER_HIT, true);
			TriggerNote(1,SFX_PLAYER_HIT, 40, SFX_VOL_PLAYER_HIT);
			break;
	}
	p1.state = state;
}

/**
	Transitions an alien to a new state.

	@param index an index into the invaders collection of aliens.
	@param state the state to which the alien transitions.
*/
void SetAlienState(u8 index, AlienState state) {
	if (index > ALIEN_COUNT)
		return;
	alien *a = &ai.aliens[index];

	switch (state) {
		case alienAlive:
			break;
		case alienHit:
		{
			rect r;

			a->hitTimer = MIN(ALIEN_HIT_DURATION, ai.maxMoveTimer);
			r.left = ai.tileLoc.x + (index&7) * ALIEN_SPACING_X;
			r.right = r.left + GET_ALIEN_WID();
			r.top = ai.tileLoc.y + (index>>3) * ALIEN_SPACING_Y;
			r.btm = r.top + GET_ALIEN_HGT();
			EnqueueHitAlien(&r);
			DrawMap2(r.left, r.top, (const char*)pgm_read_word(alienDeadMaps + ai.dispState));
			--ai.aliensRemaining;
			AdjustScore(100 + (yellow - a->type) * 100);
			//TriggerFx(SFX_ALIEN_HIT, SFX_VOL_ALIEN_HIT, true);
			TriggerNote(1,SFX_ALIEN_HIT, 50, SFX_VOL_ALIEN_HIT);
			break;
		}
		case alienDead:
		{
			rect *r = DequeueHitAlien();

			if (r)
				FillRegion(r->left, r->top, r->right - r->left, r->btm - r->top, CLEAR_TILE);
			break;
		}
	}
	a->state = state;
}

/**
	Transitions a projectile to a new state.

	@param p the projectile whose state should be set.
	@param state the state to which the projectile transitions.
	@param vel the velocity to give the projectile in its new state.
	@param loc the location to give the projectile in its new state.
*/
void SetProjectileState(projectile *p, ProjectileState state, char vel, const pt *loc) {
	switch (state) {
		case idle:
			HideSprite(p->sprite, p->anim.wid, p->anim.hgt);
			break;
		case active:
			if (loc)
				p->loc = *loc;
			p->vel = vel;
			memcpy_P(&p->anim, animations + p->animIndex, sizeof(p->anim));
			break;
	}
	p->state = state;
}

/**
	Transitions the ufo to a new state.

	@param state the state to which the ufo transitions.
*/
void SetUfoState(UfoState state) {
	switch (state) {
		case ufoIdle:
			HideSprite(SPRITE_UFO, mapUfo_Width, mapUfo_Height);
			break;
		case ufoActive:
			sprites[SPRITE_UFO].tileIndex = pgm_read_byte(mapUfo + 2);
			ufo.loc = (pt) { UFO_START_LOC_X, UFO_START_LOC_Y };
			ufo.wid = mapUfo_Width>>1;
			ufo.vel = -UFO_SPD;
			ufo.bonus = (1 + (((u16)PRNG_NEXT() + 1)>>6)) * 1000;	// 1-5k (1 in 256 of 5k)
			//TriggerFx(SFX_UFO, SFX_VOL_UFO, true);
			TriggerNote(2,SFX_UFO, 60, SFX_VOL_UFO);
			break;
		case ufoHit:
		{
			HideSprite(SPRITE_UFO, mapUfo_Width, mapUfo_Height);

			rect r;
			const char *deadMap;
			
			if ((ufo.loc.x&7) < TILE_WID2) {
				deadMap = mapAlienDead1;
				r.left = ufo.loc.x>>3;
				r.right = r.left + mapUfo_Width;
			} else {
				deadMap = mapAlienDead0;
				r.left = (ufo.loc.x>>3) + 1;
				r.right = r.left + (mapUfo_Width>>1);
			}

			r.top = ufo.loc.y>>3;
			r.btm = r.top + mapUfo_Height;
			EnqueueHitAlien(&r);
			DrawMap2(r.left, r.top, deadMap);
			AdjustScore(ufo.bonus);
			ufo.hitTimer = UFO_HIT_TIMER_DURATION;
			//TriggerFx(SFX_ALIEN_HIT, SFX_VOL_ALIEN_HIT, true);
			TriggerNote(2,SFX_ALIEN_HIT, 50, SFX_VOL_ALIEN_HIT);
			break;
		}
		case ufoDead:
		{
			rect *r = DequeueHitAlien();

			if (r)
				FillRegion(r->left, r->top, r->right - r->left, r->btm - r->top, CLEAR_TILE);
			break;
		}
	}
	ufo.state = state;
}

/**
	Manages the ufo based on its state.
*/
void UpdateUfo(void) {
	static u16 spawnTimer = UFO_MIN_INTERVAL;

	switch (ufo.state) {
		case ufoIdle:
			if (--spawnTimer == 0) {
				spawnTimer = UFO_MIN_INTERVAL + (PRNG_NEXT()<<1);	// Spawns a ufo roughly every 6 seconds, on average

				if (ufo.roundCount < UFO_PER_ROUND) {
					++ufo.roundCount;
					SetUfoState(ufoActive);
				}
			}
			break;
		case ufoActive:
			MoveUfo();
			DrawUfo();
			break;
		case ufoHit:
			if (--ufo.hitTimer == 0)
				SetUfoState(ufoDead);
			break;
		case ufoDead:
			SetUfoState(ufoIdle);
			break;
	}
}

/**
	Moves the ufo along the x-axis and cancels the ufo if it goes off-screen.
*/
void MoveUfo(void) {
	if (ufo.state != ufoActive)
		return;
	ufo.loc.x += ufo.vel;

	if (ufo.loc.x <= BOUNDARY_LEFT)
		ufo.vel *= -1;
	else if (ufo.loc.x >= UFO_START_LOC_X && ufo.vel == UFO_SPD)
		SetUfoState(ufoIdle);
}

/**
	Prints the ufo to vram.
*/
void DrawUfo(void) {
	if (ufo.state != ufoActive)
		return;
	if (ufo.loc.x == (UFO_START_LOC_X - TILE_WIDTH)) {
		if (ufo.vel == UFO_SPD) {
			HideSprite(SPRITE_UFO, mapUfo_Width, mapUfo_Height);
			sprites[SPRITE_UFO].tileIndex = pgm_read_byte(mapUfo + 2);
			ufo.wid = mapUfo_Width>>1;
		} else {
			MapSprite(SPRITE_UFO, mapUfo);
			ufo.wid = mapUfo_Width;
		}
	}
	MoveSprite(SPRITE_UFO, ufo.loc.x, ufo.loc.y, ufo.wid, mapUfo_Height);
}

/**
	Adjusts and prints the player's score and the hi score (if required).

	@param val the value to add to the player's score.
*/
void AdjustScore(int val) {
	p1.score += val;
	
	if (p1.score > hiScore) {
		hiScore = p1.score;
		PrintScores();
	} else {
		PrintPlayerScore();
	}

	if (p1.lives < 10) {
		if (p1.score >= freeLife) {
			freeLife += 25000;
			++p1.lives;
			DrawLives();
		}
	}
}

/**
	Enqueues the bounding rectangle of an alien that has been hit
	by the player's laser cannon; this is used later for accurate
	explosion graphic removal.

	@param r the hit region to enqueue.
	@return Success of enqueue operation.
*/
u8 EnqueueHitAlien(const rect *r) {
	if (!hitQ[hqIndex].occupied) {
		hitQ[hqIndex].occupied = 1;
		hitQ[hqIndex].r = *r;
		hqIndex = (hqIndex + 1)&3;
		return 1;
	}
	return 0;
}

/**
	Dequeues that which EnqueueHitAlien enqueued.

	@return the hit region that was previously enqueued.
*/
rect* DequeueHitAlien(void) {
	u8 it = (hqIndex + 1)&3;

	for (u8 i = 0; i < HIT_QUEUE_SIZE; i++) {
		if (hitQ[it].occupied) {
			hitQ[it].occupied = 0;
			return &hitQ[it].r;
		}
		it = (it + 1)&3;
	}
	return 0;
}

/**
	Ensures that only those aliens that are alive and on the front row of their
	respectives columns are designated shooters.
*/
void UpdateAttackers(void) {
	for (u8 i = 0; i < ALIENS_PER_ROW; i++) {
		while (ai.aliensRemaining && ai.aliens[ai.attackers[i]].state != alienAlive && ai.attackers[i] >= ALIENS_PER_ROW)
			ai.attackers[i] -= ALIENS_PER_ROW;
	}
}

/**
	Similar to kernel's MapSprite, but no wid/hgt header to read from flash.

	@param index the index of the sprite in the kernel's sprites data structure.
	@param wid the width of the sprite to map (tile-based).
	@param hgt the height of the sprite to map (tile-based).
	@param map the map to use in the operation.
	@param spriteFlags a flag indicating that the sprite map should be flipped on the x-axis (SPRITE_FLIP_X = true).
*/
void SpaceInvadersMapSprite(u8 index, u8 wid, u8 hgt, const char *map, u8 spriteFlags) {
	u8 x,y,xStart,xEnd,xStep;

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
			sprites[index].tileIndex = pgm_read_byte(&(map[(y*wid)+x]));
			sprites[index].flags = spriteFlags;
		}
	}
}

/**
	Checks whether or not the invaders are close enough to the player's ship
	to have killed it.

	@return the result of the check.
*/
u8 InvadersBreachedShelters(void) {
	u8 index, frontLine = 0;

	for (u8 i = 0; i < ALIENS_PER_ROW; i++) {
		index = ai.attackers[i]>>3;
		
		if (index > frontLine)
			frontLine = index;
	}
	
	if ((ai.tileLoc.y + (frontLine * ALIEN_SPACING_Y + 1)) >= INVADERS_MAX_LOC_Y)
		return 1;
	return 0;
}


int main(void) {
	u16 btnPrev = 0;			// Previous buttons that were held
	u16 btnHeld = 0;    		// Buttons that are held right now
	u16 btnPressed = 0;  		// Buttons that were pressed this frame
	projectile *projectiles[] = { &p1.prj, &ai.prj };
	u8 round = 0, flashStartTimer = 0, playerDeadTimer = 0, gameOverTimer = 0;

	// Init
	gameState = title;
	hiScore = 0;
	LoadHighScore();
	SetTileTable(tileset);
	SetSpritesTileTable(spriteset);
	SetSpriteVisibility(true);
	InitMusicPlayer(patches);
	SetMasterVolume(0xc0);
	ClearVram();
	DrawMap2(0, 0, mapBackground);
	flashStartTimer = 1;
	
	while(1) {
		if (GetVsyncFlag()) { // This places an upper bound of 60 Hz on your game logic (for NTSC)
			ClearVsyncFlag();

			btnHeld = ReadJoypad(0);
			btnPressed = btnHeld&(btnHeld^btnPrev);
			btnPrev = btnHeld;

			if ((btnPressed&BTN_SELECT) && (gameState != title)) {
				if (p1.score >= hiScore)
					SaveHighScore();
				FillRegion(PLAYING_FIELD_LOC_X, PLAYING_FIELD_LOC_Y, PLAYING_FIELD_WID, PLAYING_FIELD_HGT-1, CLEAR_TILE);
				HideSprite(0, MAX_SPRITES, 1);
				gameState = title;
			}

			switch (gameState) {
				case title:						
					if (btnPressed&BTN_START) {
						InitRound(round = 0);
						gameState = playing;
					} else {
						if (--flashStartTimer == 0) {
							FlashPressStart();
							flashStartTimer = HZ>>1;
						}
					}
					++prng;
					prng = MAX(prng,1);
					break;
				case playing:
					if (ai.aliensRemaining == 0)
						InitRound(++round);

					if (btnPressed&BTN_START) {
						gameState = paused;
						break;
					}

					MoveProjectiles(2, projectiles);
					ProcessCollisions();

					if (InvadersBreachedShelters())
						SetPlayerState(playerDead);

					if (p1.state == playerDead) {
						playerDeadTimer = 2*HZ;
						gameState = dead;
						break;
					}
					
					MovePlayer(btnHeld);
					PlayerAttack(btnPressed);
					ClearAliens();
					MoveAliens();
					AliensAttack();
					AnimateProjectiles(2, projectiles);
					DrawProjectiles(2, projectiles);
					AnimatePlayer();
					DrawPlayer();
					DrawAliens();
					UpdateAttackers();
					UpdateUfo();
					break;
				case dead:
					// Ignore pause game while dead
					if (--playerDeadTimer) {
						AnimatePlayer();
						DrawPlayer();
						break;
					}
					if (p1.lives == 0) {
						gameState = gameOver;
						break;
					} else if (InvadersBreachedShelters()) {
						InitRound(++round);
					} else {
						SetPlayerState(playerAlive);
						gameState = playing;
					}
					break;
				case paused:
					if (btnPressed&BTN_START){
						gameState = playing;
					}
					break;
				case gameOver:
					if (gameOverTimer == 0) {
						if (p1.score >= hiScore)
							SaveHighScore();
						FillRegion(PLAYING_FIELD_LOC_X, PLAYING_FIELD_LOC_Y, PLAYING_FIELD_WID, PLAYING_FIELD_HGT-1, CLEAR_TILE);
						DrawMap2(GAME_OVER_LOC_X, GAME_OVER_LOC_Y, mapGameOver);
						HideSprite(0, MAX_SPRITES, 1);
						gameOverTimer = 3*HZ;
					} else if (--gameOverTimer == 0) {
						flashStartTimer = 1;
						gameState = title;
					}
					break;
			}
		}
	}
}


