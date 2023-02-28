/*
 *  Uzebox Pac-Man
 *  Copyright (C) 2009 Paul McPhee
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
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>
#include <stdint.h>


/****************************************
 *				Constants				*
 ****************************************/
 #ifndef INTERMISSION_LOOP
	#define INTERMISSION_LOOP 0		// When enabled, the intermissions repeat successively
#endif

// Pacman
#define PACMAN_ANIMS_COUNT			6
#define PACMAN						2	// Sprite offset
#define PAC_START_LOC_X				72
#define PAC_START_LOC_Y				128
#define ANIM_PACMAN_DIE				4

	// States
#define PSTATE_MOVING				1
#define PSTATE_DYING				2
#define PSTATE_DEAD					4
#define PSTATE_REVIVING				8
#define PSTATE_GAME_OVER			16
#define PSTATE_AI					32

// Ghosts
#define GHOST_COUNT					4
#define BLINKY						0	// For array positioning/sprite indexing
#define PINKY						1	//
#define INKY						2	//
#define CLYDE						3	//
#define GSPROFF						3	// Ghost sprites offset
#define BLINKY_ANIMS_COUNT			12	// 2 more for intermissions
#define PINKY_ANIMS_COUNT			10
#define INKY_ANIMS_COUNT			10
#define CLYDE_ANIMS_COUNT			10
#define ANIM_GHOST_FLEE				4
#define ANIM_GHOST_FLASH			5
#define ANIM_GHOST_DEAD_UP			6
#define ANIM_GHOST_DEAD_DN			7
#define ANIM_GHOST_DEAD_LT			8
#define ANIM_GHOST_DEAD_RT			9
#define GHOST_ANIM_DEAD_OFFSET		6	// Offset from alive to dead animations
#define BLINKY_SCATTER_LOC_X		16
#define BLINKY_SCATTER_LOC_Y		0
#define BLINKY_HOUSE_LOC_X			9
#define BLINKY_HOUSE_LOC_Y			10
#define PINKY_SCATTER_LOC_X			2
#define PINKY_SCATTER_LOC_Y			0
#define PINKY_HOUSE_LOC_X			9
#define PINKY_HOUSE_LOC_Y			10
#define INKY_SCATTER_LOC_X			16
#define INKY_SCATTER_LOC_Y			21
#define INKY_HOUSE_LOC_X			8
#define INKY_HOUSE_LOC_Y			10
#define CLYDE_SCATTER_LOC_X			2
#define CLYDE_SCATTER_LOC_Y			21
#define CLYDE_HOUSE_LOC_X			10
#define CLYDE_HOUSE_LOC_Y			10

	// States
#define NSTATE_MOVING				1
#define NSTATE_FRIGHTENED			2
#define NSTATE_DEAD					4
#define NSTATE_FLASH				8
#define NSTATE_EATEN				16
#define NSTATE_REVIVED				32
#define NSTATE_EMPTY_NEST			64
#define NSTATE_HOUSE_ARREST			128

	// elroy
#define NSTATE_ELROY				1
#define NSTATE_ELROY1				2
#define NSTATE_ELROY2				4
#define NSTATE_ELROY1_HOLD			8
#define NSTATE_ELROY2_HOLD			16

// Map
#define GAME_MAP_WID 				5	// Actually 19, but we represent as 4/byte with a trailing don't care bit
#define GAME_MAP_HGT 				22
#define SCREEN_LT					2
#define SCREEN_TOP					0
#define MAP_OFFSET_X				(SCREEN_LT * TILE_WID + 32)
#define MAP_OFFSET_Y				24
#define TILE_WID					8
#define TILE_HGT					8
#define TILE_WID2					(TILE_WID / 2)
#define TILE_HGT2					(TILE_HGT / 2)
#define TUNNEL_LEFT					(MAP_OFFSET_X + 5)		// One pixel before Square() to prevent getting lost in the nether on direction swap
#define TUNNEL_RIGHT				(MAP_OFFSET_X + 147)	//
#define GHOST_HOUSE_DOOR_X			9
#define GHOST_HOUSE_DOOR_Y			8

#define GHOST_NOTURN_AX				8	// Ghosts can't turn upwards here
#define GHOST_NOTURN_AY				8	//
#define GHOST_NOTURN_BX				10  //
#define GHOST_NOTURN_BY				8	//
#define GHOST_NOTURN_CX				8	//
#define GHOST_NOTURN_CY				16	//
#define GHOST_NOTURN_DX				10	//
#define GHOST_NOTURN_DY				16	//

#define GHOST_SLOW_AX				3	// Ghosts go slow in tunnels
#define GHOST_SLOW_AY				10	//
#define GHOST_SLOW_BX				15	//
#define GHOST_SLOW_BY				10	//

// Movement
#define SPEED_MODE_MOVE				0	// Instead of fractional speeds we stop for 1/X frames
#define SPEED_MODE_PAUSE			1	//
#define DIR_UP 						-1
#define DIR_LEFT					-1
#define DIR_DOWN					1
#define DIR_RIGHT					1
#define DIR_INVALID					2	// Indicates no queued movement

// General
#define PACMAN_EEPROM_ID			13
#define PLAYER_COUNT				1	// Was considering 2 players
#define MAX_PELLETS					157
#define ANIM_UP						0
#define ANIM_LT						1
#define ANIM_DN						2
#define ANIM_RT						3
#define ANIM_NONE					0xFF
#define FRUIT_BONUS_SPRITE			0	// 0-1	Paint first
#define KILL_BONUS_SPRITE			7	// 7-8	Paint last to overlay
#define CURTAIN_SPRITE_SML			KILL_BONUS_SPRITE	// Re-use to save sprite count as they never appear concurrently
#define CURTAIN_SPRITE_LGE			5	// 5-8			//
#define POINTS_PELLET				10
#define POINTS_ENERGIZER			50
#define FRUIT_TILE_CHERRY			1
#define FRUIT_TILE_STRAWBERRY		2
#define FRUIT_TILE_PEACH			3
#define FRUIT_TILE_APPLE			4
#define FRUIT_TILE_MELON			5
#define FRUIT_TILE_GALAXIAN			6
#define FRUIT_TILE_BELL				7
#define FRUIT_TILE_KEY				8
#define SCORE_X						(SCREEN_LT + 8)		// Game score position
#define SCORE_Y						2	//
#define HSCORE_X					(SCREEN_LT + 19)	// High score position
#define HSCORE_Y					2	//
#define HZ							60	// Frame rate
#define FRUIT_LOG_X 				(MAP_OFFSET_X / TILE_WID + 16)
#define FRUIT_LOG_Y 				25	//
#define MAX_LIVES					5
#define LIFE_SPRITE					47
#define LIFE_POS_X					(MAP_OFFSET_X / TILE_WID + 2)
#define LIFE_POS_Y					25
#define INTRO_PAUSE					(5*HZ)
#define LVL_COMPLETE_PAUSE			(5*HZ)
#define LVL_COMPLETE_FLASH			(4*HZ)

// Game Text
#define GT_P1_X 					6
#define GT_P1_Y						8
#define GT_P1_LEN					7
#define GT_RDY_X					7
#define GT_RDY_Y					12
#define GT_RDY_LEN					5
#define GT_GAMEOVER_X				6
#define GT_GAMEOVER_Y				12
#define GT_GAMEOVER_LEN				7

// SFX
#define SFX_PELLET1					0
#define SFX_PELLET2					1
#define SFX_VOL_PELLET				0x30
#define SFX_FRUIT					2
#define SFX_VOL_FRUIT				0xD0
#define SFX_PACMAN_DIE				3
#define SFX_VOL_PACMAN_DIE			0xA6
#define SFX_SIREN_SLOW				4
#define SFX_SIREN_MED				5
#define SFX_SIREN_FAST				6
#define SFX_VOL_SIREN				0xA6
#define SFX_DUR_SIREN				32
#define SFX_FRIGHTENED				7
#define SFX_VOL_FRIGHTENED			0xA6
#define SFX_DUR_FRIGHTENED			8
#define SFX_KILL_GHOST				8
#define SFX_VOL_KILL_GHOST			0x7C
#define SFX_DUR_KILL_GHOST			22
#define SFX_DEAD_GHOST				9
#define SFX_VOL_DEAD_GHOST			0xA6
#define SFX_DUR_DEAD_GHOST			16
#define SFX_EXTRA_LIFE				10
#define SFX_VOL_EXTRA_LIFE			0xA6
	// States
#define SFXSTATE_SIREN				1
#define SFXSTATE_FRIGHTENED			2
#define SFXSTATE_DEAD_BLINKY		4
#define SFXSTATE_DEAD_PINKY			8
#define SFXSTATE_DEAD_INKY			16
#define SFXSTATE_DEAD_CLYDE			32
#define SFXSTATE_DEAD_GHOST			(SFXSTATE_DEAD_BLINKY + SFXSTATE_DEAD_PINKY + SFXSTATE_DEAD_INKY + SFXSTATE_DEAD_CLYDE)
#define SFXSTATE_KILL_GHOST			64
#define SFXSTATE_EXTRA_LIFE			128
#define SFXSTATE_PACMAN_DIE			256


// Font offsets (there's some junk at the start of the font tiles)
#define FONT_WHITE 					18
#define FONT_RED 					44
#define FONT_PINK 					58
#define FONT_AQUA 					81
#define FONT_ORANGE					99
#define FONT_BROWN					117


// Intermissions
#define IM_PACMAN_START_X			(SCREEN_TILES_H * TILE_WID - TILE_WID)
#define IM_PACMAN_START_Y			(((SCREEN_TILES_V * TILE_HGT) / 2) - TILE_WID2)
#define IM_BLINKY_START_X			(SCREEN_TILES_H * TILE_WID - TILE_WID)
#define IM_BLINKY_START_Y			IM_PACMAN_START_Y
#define IM_PACMAN_GIANT				5
#define IM2_PACMAN_EXIT				1
#define IM2_BLINKY_SNARED			2
#define IM2_BLINKY_EXPOSED			4
#define IM2_ANIM_SNARED				ANIM_LT
#define IM2_SNARED_TILE1			(PACMAN_TILESET_SIZE + 12)
#define IM2_SNARED_TILE2			(PACMAN_TILESET_SIZE + 11)
#define IM2_SNARED_TILE3			(PACMAN_TILESET_SIZE + 13)
#define IM2_EXPOSED_TILE1			(PACMAN_TILESET_SIZE + 9)
#define IM2_EXPOSED_TILE2			(PACMAN_TILESET_SIZE + 10)
#define IM3_ANIM_STITCHED			10
#define IM3_ANIM_EXPOSED			11

// Title/Intro
#define TI_PACMAN_START_X			(20 * TILE_WID)
#define TI_PACMAN_START_Y			(16 * TILE_HGT)

// States
	// Intro
#define GSTATE_INIT					1
#define GSTATE_TITLE_SCREEN			2
#define GSTATE_GHOST_INTRO			4
#define GSTATE_INTRO_END			8

	// General
#define GSTATE_NEW_GAME				1
#define GSTATE_NEW_LEVEL			2
#define GSTATE_GHOST_EATEN			4
#define GSTATE_BONUS_PAUSE			8
#define GSTATE_LVL_COMPLETE			16
#define GSTATE_LVL_COMMENCED		32
#define GSTATE_NORMAL				64
#define GSTATE_INTERMISSION			128

	// Fright
#define GSTATE_FRIGHTENED_INIT		1
#define GSTATE_FRIGHTENED			2
#define GSTATE_FLASH				4
	// Fruit
#define GSTATE_FRUIT_INIT			1
#define GSTATE_FRUIT				2
#define GSTATE_FRUIT_BONUS_INIT		4
#define GSTATE_FRUIT_BONUS			8
	// Pursuit
#define GSTATE_SCATTER_INIT			1
#define GSTATE_SCATTER				2
#define GSTATE_CHASE_INIT			4
#define GSTATE_CHASE				8

// Screen States
#define SSTATE_INTRO				1
#define SSTATE_TITLE				2
#define SSTATE_DEMO					4
#define SSTATE_GAME					8
	// Reset flags
#define RESET_NONE					0
#define RESET_ALL					1
#define RESET_TMRS					2


/****************************************
 *				Utils					*
 ****************************************/
// General
#define MIN(_x,_y)  ((_x)<(_y) ? (_x) : (_y))
#define MAX(_x,_y)  ((_x)>(_y) ? (_x) : (_y))
#define MOD2N(_a,_n) ((_a)&((_n)-1))	// 2^n mod
#define ABS(_x)		(((_x) > 0) ? (_x) : -(_x))
// Bit manipulations
#define SB(_reg, _mask)		(_reg) |= (_mask);	// Set bits
#define CB(_reg, _mask)		(_reg) &= ~(_mask);	// Clear bits
#define QB(_reg, _mask)		((_reg) & (_mask))	// Query bits
#define QBC(_reg, _mask)	(((_reg) & (_mask)) && (((_reg) &=~ (_mask)) || 1))	// Query bits w/ clear
// Game-specific
#define SetLoc(_x, _y, _loc)	(_loc).x = (_x);	\
								(_loc).y = (_y);

#define SetDir(_x, _y, _dir)	(_dir).x = (_x);	\
								(_dir).y = (_y);

#define DirDiff(_a, _b)	((_a).x != (_b).x || (_a).y != (_b).y)
#define Square(_x, _y)	((MOD2N((_x)-TILE_WID2, TILE_WID) == 0) && (MOD2N((_y)-TILE_HGT2, TILE_HGT) == 0))
// PRNG must be repeatable to have any chance of letting players develop patterns
#define PrngNext()		((char)(ghosts[MOD2N(prng++, GHOST_COUNT)].loc.x))

#define OUTSIDE_GHOST_HOUSE_DOOR(_g)	((ghosts[(_g)].loc.x == GHOST_HOUSE_DOOR_X) && (ghosts[(_g)].loc.y == GHOST_HOUSE_DOOR_Y))
#define INSIDE_GHOST_HOUSE_DOOR(_g)		((ghosts[(_g)].loc.x == GHOST_HOUSE_DOOR_X) && (ghosts[(_g)].loc.y == GHOST_HOUSE_DOOR_Y + 2))
#define IN_GHOST_HOUSE(_g)				((ghosts[(_g)].loc.x == ghosts[(_g)].hloc.x) && (ghosts[(_g)].loc.y == ghosts[(_g)].hloc.y))

/****************************************
 *			Type declarations			*
 ****************************************/
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct pt {
	u8 x;
	u8 y;
} pt;

typedef struct vec {
	char x;
	char y;
} vec;

typedef struct speed {
	u8 modeInit;						// Do we start moving or paused
	u8 mode;							// Current mode
	u8 moveMax;							// When moveCount reaches this, switch to pause mode
	u8 moveCount;						// Counts how many frames we have been moving since last pause
	u8 pauseMax;						// When pauseCount reaches this, switch to move mode
	u8 pauseCount;						// Counts how many frames we have been paused since last movement
} speed;

typedef struct collQuery {
	u8 		mapIndex;					// Index into game_map (narrows down to 4x1 block of squares)
	u8 		gridIndex;					// Index into square of 4x1 block pointed to by mapIndex
	bool 	collided;					// Map collision indicator
} collQuery;

typedef struct animation {
	u8			size;					// wid*hgt
	u8			wid;					// Width of each frame (in tiles)
	u8			hgt;					// Height of each frame (in tiles)
	u8			frameCount;				// Number of frames*size in an animation cycle
	u8 			currFrame;				// The frame that is currently displayed
	u8			disp;					// Displacement counter
	u8			dpf;					// Displacement per frame (scales frame rate to movement speed)
	const char	*frames;				// Stored in flash
} animation;


typedef struct npc {
	vec			loc;					// Location on board - TILE_WID/HGT resolution
	pt			sprLoc;					// Sprite location - location on screen (single pixel resolution)
	vec			dir;					// Normalized
	vec			prevDir;				// For animation switches
	u8			state;					// Bitmap
	u8			sprite;					// Current sprite array index
	speed		*spd;					// Current speed
	animation	*anim;					// Current animation
	vec			sloc;					// Scatter location
	vec			hloc;					// House location
	u8			pelCount;				// Pellet counter - controls ghost house release
	u8			flashCount;				// Number of times to flash when exiting frightened mode
	bool		inHouse;				// True if trapped in the ghost house
	speed		nspd;					// Normal speed
	speed		fspd;					// Fright speed
	speed		tspd;					// Tunnel speed
} npc;

typedef struct player {
	vec			loc;					// Location on board - TILE_WID/HGT resolution
	pt			sprLoc;					// Sprite location - location on screen (pixel resolution)
	vec			dir;					// Normalized
	vec 		prevDir;				// For animation switches
	u8			state;					// Bitmap
	u8			sprite;					// Current sprite array index
	speed		*spd;					// Current speed
	animation	*anim;					// Current animation
	vec			qdir;					// Queued direction. Pacman can be told to turn at next intersection
	speed		nspd;					// Normal speed
	speed		fspd;					// Fright speed
} player;


typedef struct bonusFruit {
	pt			loc;
	u16			activeTmr;				// Fruit disappears when this gets to 0
	u16			bonus;					// Bonus score for eating fruit
	u8			tile;					// Fruit icon map
	const char	*bonusMap;				// Sprite to display when fruit is eaten
	u8			fruitCounter[6];		// Last six fruits (including this round) left->right reverse-chronological order
	u8			bonusTmr;				// Bonus sprite disappears when this gets to 0
} bonusFruit;

typedef struct gamestate {
	u8 intro;							// Specific to the title screens
	u8 general;							// Miscellaneous game state
	u8 fright;							// Special case when ghosts are vulnerable
	u8 pursuit;							// Game-level ghost AI state
	u8 fruit;							// Bonus fruit details
} gamestate;

/****************************************
 *			Data Dependencies			*
 ****************************************/
// Tiles
#include "data/tiles.pic.inc"
#include "data/pacfont.pic.inc"
#include "data/tiles.map.inc"
// Sprites
#include "data/sprites.pic.inc"
#include "data/sprites.map.inc"
// Animations
#include "data/animations.pic.inc"
#include "data/animations.map.inc"
// Music/sfx
#include "data/pacpatches.inc"
#include "data/newgame.inc"
#include "data/intermission.inc"
// Utils
#include "data/LUT.inc"

/****************************************
 *			File-level variables		*
 ****************************************/
player pacman;				// Good guy
npc ghosts[GHOST_COUNT];	// Bad guys. Ordered: Blinky, Pinky, Inky, Clyde
u8 gameMap[GAME_MAP_WID * GAME_MAP_HGT];	// Map meta data for collisions/pellets
u8 level;
u8 pellets;					// Those dots for which pacman has a penchant
u8 frameCount;				// Resets every 21 frames to assist with speed calcs (21 due to 105% elroy2 speed)
u8 scrnState;				// Which screen is displaying
u16 playerInput;			// snes controller input storage
gamestate gs;				// Game-level state
u8 freshCorpse;				// Most recently devoured ghost
u8 elroy;					// Blinky's elroy flag
u32 score;
u32 highscore;
u32 scoreMod;				// Counts in lots of 10k for new lives and saves us mod'ing the 32-bit score regularly
u8 credit;					// Simulate coin insertion for nostalgia's sake
u16 coinTmr;				// Handles coin insertion prompt display timing
u16 frightTmr;				// Time ghosts spend frightened after energizer pickup
u16 bonusTmr;				// Time spent paused while displaying bonus for eating ghost
u16 gameOverTmr;			// Time spent paused after game over
u32 lvlTmr;					// Approx. level time
u8 lives;					// Number of lives remaining
// Ghost house logic is overly complicated because that is how the original pacman was designed
u8 pelIndex;				// Active pellet counter (controls ghost house release and indexes ghosts array)
u8 pelCount;				// Global Pellet Counter (enabled upon life lost) (PINKY: 7, INKY: 17, CLYDE: 32)
u8 pelTmr;					// Global Pellet Timer (ensures pacman is eating pellets so game will progress. Level < 5: 4secs, Level >= 5: 3secs)
u16 killBonus;				// 200, 400, 800, 1600
u8 pacmanAiOffset;			// Reduces chances of seeing repeated demo AI routines
u16 sfxState;				// Don't want some sounds stealing focus from others
u8 extraLifeCount;			// Counts the frames to play the extra life sfx
int prng;					// PRNG seed
u8 sfxSiren;				// Siren pitch alters as the level progresses
u8 sirenToggle;				// Allow players to turn off the siren
const char *killBonusMap;	// Current kill bonus sprite map
animation playerAnims[PLAYER_COUNT];
animation ghostAnims[GHOST_COUNT];
bonusFruit fruit;
const char p1Score[] PROGMEM = "1UP";
const char hs[] PROGMEM = "HIGH SCORE";
const char pushStartButton[] PROGMEM = "PUSH START BUTTON";
const char onePlayerOnly[] PROGMEM = "1 PLAYER ONLY";
const char bonusPacman[] PROGMEM = "BONUS PAC-MAN FOR 10000 PTS";
const char copyright[] PROGMEM = "\\ 1980 MIDWAY MFG. CO.";
const char creditText[] PROGMEM = "CREDIT     ";
const char insertCoin[] PROGMEM = "INSERT COIN";
const char charNick[] PROGMEM = "CHARACTER / NICKNAME";
const char shadow[] PROGMEM = "-SHADOW";
const char speedy[] PROGMEM = "-SPEEDY";
const char bashful[] PROGMEM = "-BASHFUL";
const char pokey[] PROGMEM = "-POKEY";
const char pts[] PROGMEM = "PTS";
const char blinkyName[] PROGMEM = "\"BLINKY\"";
const char pinkyName[] PROGMEM = "\"PINKY\"";
const char inkyName[] PROGMEM = "\"INKY\"";
const char clydeName[] PROGMEM = "\"CLYDE\"";
u8 fontOffset;				// The font offset changes as we switch between game/title screen tilesets. Some fonts are common to both
// We've got plenty of RAM to spare. Ordered: White, Red, Pink, Aqua, Orange, Brown
char fontIndexes[] = "0123456789CEGHIOPRSUADKMNT\"-ABDHIKLNOSWY\"-.\\0189ACDEFGIKMNOPSWY\"-1ABEFHIKLNOPRSUY\"-ABCDEHKLNOPRSTUY-01ABCFMNOPRSTU";

/****************************************
 *			Function prototypes			*
 ****************************************/

inline u16 NewtonSqrt(u16 b, u8 iter);
void PacmanPrintNumeral(int x, int y, u32 val, u8 digits);
void PacmanPrintByte(int x, int y, u8 val);
void PacmanPrintLong(int x, int y, u32 val);
void PacmanPrint(int x, int y, const char *s, u8 color);
bool CheckGhostCollision(void);
void CheckPlayerActivity(void);
void DrawFruitLog(void);
void AddScore(u16 s);
u16 DisplayTitleScreen(u8 reset);
u8 DisplayIntroScreen(u8 reset);
void PacmanFormatEeprom(void);
void LoadHighScore(void);
void SaveHighScore(void);
void PassPelletCounter(void);
void PlayIntermission(void);
void PlayIntermission13(u8 mode);
void PlayIntermission2(void);
void Flash1Up(void);
void FlashEnergizers(void);
void ApplyGhostState(u8 g);
void ApplyGameState(void);
void UpdateKillBonus(void);
u8 GetFlashCount(void);
void UpdateElroyCutoff(void);
void SetFrightenedTimer(void);
void UpdatePursuitMode(void);
inline vec GetDistance(vec origin, vec dest);
void NavigateIntersection(vec dest, vec loc, vec *dir);
bool AtIntersection(vec loc, pt sprLoc, vec dir);
void DoGhostAI(u8 ghost);
inline void HideSprite(u8 spriteIndex, u8 wid, u8 hgt);
void SetFruit(void);
void EatFruit(void);
void DrawScore(u8 x, u8 y, u32 score);
inline bool IsEnergizer(vec loc);
void EatPellet(vec loc, collQuery *cq);
void ProcessInput(void);
void QueryCollisions(vec loc, collQuery *cq);
u8 GetAnimDir(vec dir);
void AnimateGhost(u8 ghost);
void SetGhostSpeed(u8 ghost);
void AnimatePacman(void);
void SetPacmanSpeed(void);
void UpdateSpeed(speed *s);
void MoveGhost(u8 ghost);
void MovePacman(void);
inline void DrawPacman(void);
inline void DrawGhost(u8 ghost);
void DrawLives(void);
void LoadPacmanAnimations(u8 index);
void LoadGhostAnimations(u8 index, u8 ghost);
void InitLevel(bool reset);
inline u16 GetInput(void);

/****************************************
 *			Function definitions		*
 ****************************************/

// 2 iterations is acceptably accurate for b <= 1000
u16 NewtonSqrt(u16 b, u8 iter) {
	int x, xshift;

	if (b == 0) {
		return 0;
	} else {
		if (b <= 4) {
			xshift = 1;
		} else if (b <= 16) {
			xshift = 2;
		} else if (b <= 64) {
			xshift = 3;
		} else if (b <= 256) {
			xshift = 4;
		} else {
			xshift = 5;
		}
		x = MAX((b>>xshift),1);

		for (u8 i = 0; i < iter; i++) {
			x = (x + b/x)>>1;
		}
		return x;
	}
}


// We re-write print functions to accommodate our wonky font set
void PacmanPrintNumeral(int x, int y, u32 val, u8 digits) {
	u8 c, i;

	for (i = 0; (i < digits) && ((i == 0) || val); i++, val /= 10) {
		c = val % 10;

		if (c || (i == 0) || (val > 0)) {
			SetTile(x--, y, c + FONT_WHITE + fontOffset);
		} else {
			SetTile(x--, y, 0 + fontOffset);
		}
	}
}


void PacmanPrintByte(int x, int y, u8 val) {
	PacmanPrintNumeral(x, y, val, 3);
}


void PacmanPrintLong(int x, int y, u32 val) {
	PacmanPrintNumeral(x, y, val, 10);
}


void PacmanPrint(int x, int y, const char *s, u8 color) {
	u8 c, d;
	int i = 0;

	while (1) {
		c = pgm_read_byte(&(s[i++]));

		if (c) {
			d = color - FONT_WHITE;

			while (1) {
				if ((c == ' ') || (fontIndexes[d] == 0)) {
					SetTile(x++, y, 0 + fontOffset);	// space
					break;
				} else if (c == fontIndexes[d]) {
					SetTile(x++, y, d + FONT_WHITE + fontOffset);
					break;
				}

				++d;
			}
		} else {
			break;
		}
	}
}


void PacmanFormatEeprom(void) {
	// Set sig. so we don't format next time
	WriteEeprom(0, (u8)EEPROM_SIGNATURE);
	WriteEeprom(1, (u8)(EEPROM_SIGNATURE>>8));
	// Reserve first valid space for pacman
	WriteEeprom(2*EEPROM_BLOCK_SIZE, (u8)EEPROM_FREE_BLOCK);
	WriteEeprom(2*EEPROM_BLOCK_SIZE + 1, (u8)(EEPROM_FREE_BLOCK>>8));
}


void LoadHighScore(void) {
	struct EepromBlockStruct ebs;

	ebs.id = PACMAN_EEPROM_ID;

	if (!isEepromFormatted()) {
		PacmanFormatEeprom();
	} else if (EepromReadBlock(PACMAN_EEPROM_ID, &ebs) == 0) {
		highscore = 0;

		for (u8 i = 0; i < 4; i++) {
			highscore |= ((u32)(ebs.data[i]))<<(24-(i<<3));
		}
	}
}


void SaveHighScore(void) {
	struct EepromBlockStruct ebs;

	ebs.id = PACMAN_EEPROM_ID;

	if (!isEepromFormatted()) {
		PacmanFormatEeprom();
	}

	for (u8 i = 0; i < 4; i++) {
		ebs.data[i] = highscore>>(24-i*8);
	}

	EepromWriteBlock(&ebs);
}


// Intermissions are rewarded upon certain level completions
void PlayIntermission(void) {
#if INTERMISSION_LOOP
	switch (level % 3) {
		case 1:
			PlayIntermission13(1);
			break;
		case 2:
			PlayIntermission2();
			break;
		default:
			PlayIntermission13(3);
			break;
	}
#else
	if (level == 2) {
		PlayIntermission13(1);
	} else if (level == 5) {
		PlayIntermission2();
	} else if ((level == 9) || (level == 13) || (level == 17)) {
		PlayIntermission13(3);
	}
#endif
}


// Plays both intermissions 1 & 3 (mode)
void PlayIntermission13(u8 mode) {
	bool songPlaying = true;

	// Init scene
	ClearVram();
	SetLoc(IM_PACMAN_START_X, IM_PACMAN_START_Y, pacman.sprLoc);
	SetDir(DIR_LEFT, 0, pacman.dir);
	MoveSprite(pacman.sprite, pacman.sprLoc.x, pacman.sprLoc.y - TILE_HGT2, pacman.anim->wid, pacman.anim->hgt);
	pacman.state = PSTATE_MOVING;
	SetPacmanSpeed();
	LoadPacmanAnimations(ANIM_LT);
	SetLoc(IM_BLINKY_START_X, IM_BLINKY_START_Y, ghosts[BLINKY].sprLoc);
	SetDir(DIR_LEFT, 0, ghosts[BLINKY].dir);
	SetDir(DIR_LEFT, 0, ghosts[BLINKY].prevDir);	// Prevent animation unloading special intermission sprite
	MoveSprite(ghosts[BLINKY].sprite, ghosts[BLINKY].sprLoc.x, ghosts[BLINKY].sprLoc.y - TILE_HGT2,
		ghosts[BLINKY].anim->wid, ghosts[BLINKY].anim->hgt);
	ghosts[BLINKY].state = 0;
	SetGhostSpeed(BLINKY);
	MapSprite(CURTAIN_SPRITE_LGE, mapCurtainLge);	// So that actors ease their way onto the stage
	MoveSprite(CURTAIN_SPRITE_LGE, IM_PACMAN_START_X - TILE_WID, IM_PACMAN_START_Y - TILE_HGT2, 2, 2);
	frameCount = 0;
	StartSong(song_intermission);

	if (mode == 1) {
		LoadGhostAnimations(ANIM_LT, BLINKY);
	} else {
		LoadGhostAnimations(IM3_ANIM_STITCHED, BLINKY);
	}

	DrawPacman();
	DrawGhost(BLINKY);

	while (1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();

			// Give kernel one frame to end previous song
			if (!songPlaying) {
				songPlaying = true;
				StartSong(song_intermission);
			}

			// Let pacman get a head-start
			if ((pacman.sprLoc.x == (IM_PACMAN_START_X - 4*TILE_WID)) && (pacman.dir.x == DIR_LEFT)) {
				SB(ghosts[BLINKY].state, NSTATE_MOVING);
			}

			// Move curtain for actors' exit left
			if ((ghosts[BLINKY].sprLoc.x == (IM_BLINKY_START_X - 2*TILE_WID)) && (ghosts[BLINKY].dir.x == DIR_LEFT)) {
				MoveSprite(CURTAIN_SPRITE_LGE, 0, IM_PACMAN_START_Y - TILE_HGT - 1, 2, 2);
			}

			// Pacman exit stage left
			if ((pacman.sprLoc.x == TILE_WID2) && (pacman.dir.x == DIR_LEFT)) {
				HideSprite(pacman.sprite, pacman.anim->wid, pacman.anim->hgt);
				CB(pacman.state, PSTATE_MOVING);
			}

			// Begin phase 2 of scene
			if ((ghosts[BLINKY].sprLoc.x == TILE_WID2) && (ghosts[BLINKY].dir.x == DIR_LEFT)) {
				StopSong();
				songPlaying = false;

				if (mode == 1) {
					CB(pacman.state, PSTATE_MOVING);
					SetDir(DIR_RIGHT, 0, ghosts[BLINKY].dir);
					SetDir(DIR_RIGHT, 0, ghosts[BLINKY].prevDir);
					ghosts[BLINKY].sprite = 4;	// Give giant pacman room
					LoadGhostAnimations(ANIM_RT, BLINKY);
					DrawGhost(BLINKY);
					MoveSprite(ghosts[BLINKY].sprite, TILE_WID2, ghosts[BLINKY].sprLoc.y - TILE_HGT2,
						ghosts[BLINKY].anim->wid, ghosts[BLINKY].anim->hgt);
				} else {
					SetDir(DIR_RIGHT, 0, ghosts[BLINKY].dir);
					SetDir(DIR_RIGHT, 0, ghosts[BLINKY].prevDir);
					LoadGhostAnimations(IM3_ANIM_EXPOSED, BLINKY);
					DrawGhost(BLINKY);
				}
			}

			// Let blinky get a head-start
			if ((mode == 1) && (ghosts[BLINKY].sprLoc.x == 4*TILE_WID) && (ghosts[BLINKY].dir.x == DIR_RIGHT)) {
				SetDir(DIR_RIGHT, 0, pacman.dir);
				SetDir(DIR_RIGHT, 0, pacman.prevDir);
				SB(pacman.state, PSTATE_MOVING);
				SetLoc(pacman.sprLoc.x, pacman.sprLoc.y - TILE_HGT2 / 2, pacman.sprLoc);
				LoadPacmanAnimations(IM_PACMAN_GIANT);
				pacman.sprite = 0;
			} else if ((mode == 1) && (ghosts[BLINKY].sprLoc.x == (4*TILE_WID + 1)) && (ghosts[BLINKY].dir.x == DIR_RIGHT)) {
				DrawPacman();
				MoveSprite(pacman.sprite, pacman.sprLoc.x, pacman.sprLoc.y, pacman.anim->wid, pacman.anim->hgt);
				// Turn the tables
				pacman.spd = &pacman.fspd;
				ghosts[BLINKY].spd = &ghosts[BLINKY].fspd;
			}

			// Move curtain for actors' exit right
			if (((mode == 3) && (ghosts[BLINKY].sprLoc.x == 2*TILE_WID) && (ghosts[BLINKY].dir.x == DIR_RIGHT)) ||
					((mode == 1) && (pacman.sprLoc.x == 2*TILE_WID) && (pacman.dir.x == DIR_RIGHT))) {
				MoveSprite(CURTAIN_SPRITE_LGE, IM_PACMAN_START_X - TILE_WID, IM_PACMAN_START_Y - TILE_HGT, 2, 2);
			}

			AnimatePacman();
			UpdateSpeed(pacman.spd);
			MovePacman();

			AnimateGhost(BLINKY);
			UpdateSpeed(ghosts[BLINKY].spd);
			MoveGhost(BLINKY);


			if ((ghosts[BLINKY].sprLoc.x == (IM_BLINKY_START_X - TILE_WID2)) && (ghosts[BLINKY].dir.x == DIR_RIGHT)) {
				if (mode == 1) {
					HideSprite(ghosts[BLINKY].sprite, ghosts[BLINKY].anim->wid, ghosts[BLINKY].anim->hgt);
					CB(ghosts[BLINKY].state, NSTATE_MOVING);
				} else {
					return;
				}
			}

			if ((pacman.sprLoc.x == IM_PACMAN_START_X) && (pacman.dir.x == DIR_RIGHT)) {
				// Reset sprite indexes
				HideSprite(pacman.sprite, pacman.anim->wid, pacman.anim->hgt);
				pacman.sprite = PACMAN;
				ghosts[BLINKY].sprite = BLINKY;
				return;
			}

			++frameCount;
		}
	}
}


void PlayIntermission2(void) {
	u8 state = 0;
	u16 exposeTmr = 0;

	ClearVram();
	SetLoc(IM_PACMAN_START_X, IM_PACMAN_START_Y, pacman.sprLoc);
	SetDir(DIR_LEFT, 0, pacman.dir);
	MoveSprite(pacman.sprite, pacman.sprLoc.x, pacman.sprLoc.y - TILE_HGT2, pacman.anim->wid, pacman.anim->hgt);
	pacman.state = PSTATE_MOVING;
	SetPacmanSpeed();
	LoadPacmanAnimations(ANIM_LT);
	SetLoc(IM_BLINKY_START_X, IM_BLINKY_START_Y, ghosts[BLINKY].sprLoc);
	SetDir(0, 0, ghosts[BLINKY].prevDir);
	SetDir(DIR_LEFT, 0, ghosts[BLINKY].dir);
	MoveSprite(ghosts[BLINKY].sprite, ghosts[BLINKY].sprLoc.x, ghosts[BLINKY].sprLoc.y - TILE_HGT2,
		ghosts[BLINKY].anim->wid, ghosts[BLINKY].anim->hgt);
	ghosts[BLINKY].state = NSTATE_MOVING;
	SetGhostSpeed(BLINKY);
	CB(ghosts[BLINKY].state, NSTATE_MOVING);
	MapSprite(CURTAIN_SPRITE_LGE, mapCurtainLge);	// So that actors ease their way onto the stage
	MoveSprite(CURTAIN_SPRITE_LGE, IM_PACMAN_START_X - TILE_WID, IM_PACMAN_START_Y - TILE_HGT2, 2, 2);
	StartSong(song_intermission);

	while (1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();

			// Let pacman get a head-start
			if ((pacman.sprLoc.x == (IM_PACMAN_START_X - 4*TILE_WID)) && (pacman.dir.x == DIR_LEFT)) {
				SB(ghosts[BLINKY].state, NSTATE_MOVING);
			}

			// Move curtain for actors' exit left
			if ((ghosts[BLINKY].sprLoc.x == (IM_BLINKY_START_X - 2*TILE_WID)) && (ghosts[BLINKY].dir.x == DIR_LEFT)) {
				MoveSprite(CURTAIN_SPRITE_LGE, 0, IM_PACMAN_START_Y - TILE_HGT - 1, 2, 2);
			}

			// Pacman exit stage left
			if (pacman.sprLoc.x == TILE_WID2) {
				HideSprite(pacman.sprite, pacman.anim->wid, pacman.anim->hgt);
				CB(pacman.state, PSTATE_MOVING);
			}

			if ((!QB(state, IM2_BLINKY_SNARED)) && (ghosts[BLINKY].sprLoc.x == (SCREEN_TILES_H * TILE_WID2))) {
				SB(state , IM2_BLINKY_SNARED);
				ghosts[BLINKY].spd->modeInit = SPEED_MODE_MOVE;
				ghosts[BLINKY].spd->mode = SPEED_MODE_MOVE;
				ghosts[BLINKY].spd->moveMax = 1;
				ghosts[BLINKY].spd->moveCount = 0;
				ghosts[BLINKY].spd->pauseMax = 7;
				ghosts[BLINKY].spd->pauseCount = 0;
				LoadGhostAnimations(IM2_ANIM_SNARED, BLINKY);
				// Stretch ghost constume
				SetTile((SCREEN_TILES_H * TILE_WID2)>>3, IM_BLINKY_START_Y>>3, IM2_SNARED_TILE1);
			}

			if (QB(state, IM2_BLINKY_SNARED) && (ghosts[BLINKY].sprLoc.x == (SCREEN_TILES_H * TILE_WID2 - TILE_WID2))) {
				// Stretch ghost constume further
				SetTile((SCREEN_TILES_H * TILE_WID2 - TILE_WID)>>3, IM_BLINKY_START_Y>>3, IM2_SNARED_TILE2);
			}

			if (!QB(state, IM2_BLINKY_EXPOSED) && (ghosts[BLINKY].sprLoc.x == (SCREEN_TILES_H * TILE_WID2 - TILE_WID - TILE_WID2))) {
				SB(state, IM2_BLINKY_EXPOSED);
				HideSprite(ghosts[BLINKY].sprite, ghosts[BLINKY].anim->wid, ghosts[BLINKY].anim->hgt);
				// Hide stretched ghost costume
				SetTile((SCREEN_TILES_H * TILE_WID2)>>3, IM_BLINKY_START_Y>>3, 0);
				SetTile((SCREEN_TILES_H * TILE_WID2 - TILE_WID)>>3, IM_BLINKY_START_Y>>3, 0);
				// Show ripped ghost costume
				SetTile((SCREEN_TILES_H * TILE_WID2)>>3, IM_BLINKY_START_Y>>3, IM2_SNARED_TILE3);
				// Show blinky looking at ripped ghost costume
				SetTile((SCREEN_TILES_H * TILE_WID2 - 2 * TILE_WID)>>3, IM_BLINKY_START_Y>>3, IM2_EXPOSED_TILE1);
				exposeTmr = 3 * HZ;
			}

			if (!QB(state, IM2_PACMAN_EXIT)) {
				UpdateSpeed(pacman.spd);
				MovePacman();
				AnimatePacman();
			}

			if (!QB(state, IM2_BLINKY_EXPOSED)) {
				UpdateSpeed(ghosts[BLINKY].spd);
				MoveGhost(BLINKY);
				AnimateGhost(BLINKY);
			} else if (exposeTmr && (--exposeTmr == HZ)) {
				// Show blinky looking self-consciously at screen
				SetTile((SCREEN_TILES_H * TILE_WID2 - 2 * TILE_WID)>>3, IM_BLINKY_START_Y>>3, IM2_EXPOSED_TILE2);
			} else if (exposeTmr == 0) {
				return;
			}
			++frameCount;
		}
	}
}


void Flash1Up(void) {
	static bool hide1Up = false;

	if (hide1Up) {
		SetTile(SCORE_X - 3, SCORE_Y - 1, 0);
		SetTile(SCORE_X - 2, SCORE_Y - 1, 0);
		SetTile(SCORE_X - 1, SCORE_Y - 1, 0);
	} else {
		PacmanPrint(SCORE_X - 3, SCORE_Y - 1, p1Score, FONT_WHITE);
	}

	hide1Up = !hide1Up;
}


void FlashEnergizers(void) {
	static u8 tileIndex = 12, tileIndexIntro = 133;

	if (QB(gs.intro, GSTATE_GHOST_INTRO)) {
		SetTile(MAP_OFFSET_X>>3, (MAP_OFFSET_Y + TI_PACMAN_START_Y)>>3, QB(*(gameMap + 80), 1<<6) ? tileIndexIntro : 0);
		SetTile(SCORE_X + 2, SCORE_Y + 20, tileIndexIntro);
		tileIndexIntro = (tileIndexIntro) ? 0 : 133;
	} else if (!gs.intro) {
		SetTile((MAP_OFFSET_X>>3) + 1, (MAP_OFFSET_Y>>3) + 3, QB(*(gameMap + 15), 1<<4) ? tileIndex : 0);
		SetTile((MAP_OFFSET_X>>3) + 17, (MAP_OFFSET_Y>>3) + 3, QB(*(gameMap + 19), 1<<4) ? tileIndex : 0);
		SetTile((MAP_OFFSET_X>>3) + 1, (MAP_OFFSET_Y>>3) + 16, QB(*(gameMap + 80), 1<<4) ? tileIndex : 0);
		SetTile((MAP_OFFSET_X>>3) + 17, (MAP_OFFSET_Y>>3) + 16, QB(*(gameMap + 84), 1<<4) ? tileIndex : 0);
		tileIndex = (tileIndex) ? 0 : 12;
	}
}


// We know it is a valid pellet position, just need to determine if it is an energizer
bool IsEnergizer(vec loc) {
	// top left, top right, btm left, btm right
	if (QB(gs.intro, GSTATE_GHOST_INTRO)) {
		return (loc.x == 0 && loc.y == 16);
	} else {
		return (loc.x == 1 && loc.y == 3) || (loc.x == 17 && loc.y == 3) ||
			(loc.x == 1 && loc.y == 16) || (loc.x == 17 && loc.y == 16);
	}
}


// Cutoffs scaled from the arcade version to match our reduced pellet count
void UpdateElroyCutoff(void) {
	u8 limit = 0;

	switch (level) {
		case 1:
			limit = 12;
			break;
		case 2:
			limit = 19;
			break;
		case 3:
		case 4:
		case 5:
			limit = 25;
			break;
		case 6:
		case 7:
		case 8:
			limit = 32;
			break;
		case 9:
		case 10:
		case 11:
			limit = 38;
			break;
		case 12:
		case 13:
		case 14:
			limit = 51;
			break;
		case 15:
		case 16:
		case 17:
		case 18:
			limit = 64;
			break;
		default:	// 19+
			limit = 77;
			break;
	}

	if ((pellets == limit) || (pellets == (limit>>1))) {
		SB(elroy, NSTATE_ELROY);
	}
}


// Values mimic arcade version
void SetFrightenedTimer(void) {
	switch (level) {
		case 1:
			frightTmr = 6;
			break;
		case 2:
		case 6:
		case 10:
			frightTmr = 5;
			break;
		case 3:
			frightTmr = 4;
			break;
		case 4:
		case 14:
			frightTmr = 3;
			break;
		case 5:
		case 7:
		case 8:
		case 11:
			frightTmr = 2;
			break;
		case 9:
		case 12:
		case 13:
		case 15:
		case 16:
		case 18:
			frightTmr = 1;
			break;
		default:
			frightTmr = 0;
			break;
	}
	frightTmr *= HZ;
}


// Values mimic arcade version
u8 GetFlashCount(void) {
	u8 fcount;

	switch (level) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 10:
		case 11:
		case 14:
			fcount = 5;
			break;
		case 9:
		case 12:
		case 13:
		case 15:
		case 16:
		case 18:
			fcount = 3;
			break;
		default:
			fcount = 0;
			break;
	}
	return fcount;
}


// Bonus score is reset upon energizer consumption or all ghosts leaving fright mode
void UpdateKillBonus(void) {
	switch (killBonus) {
		case 200:
			killBonusMap = map400;
			break;
		case 400:
			killBonusMap = map800;
			break;
		case 800:
			killBonusMap = map1600;
			break;
		default:
			killBonusMap = map200;
			break;
	}

	killBonus = (killBonus == 1600) ? 200 : killBonus<<1;
}


// Ghost house logic pointer. Controls who is next in line to leave.
void PassPelletCounter(void) {
	for (u8 i = PINKY; i <= CLYDE; i++) {
		// Pass counter to next most preferred ghost
		if (i != pelIndex) {
			if (ghosts[i].inHouse) {
				pelIndex = i;
				break;
			}
		}
	}
}


void EatPellet(vec loc, collQuery *cq) {
	static u8 sfxPelletMod = 0;

	// Make sure there is an active pellet on the square
	if (QB(gameMap[cq->mapIndex], 1<<(cq->gridIndex - 1))) {
		// We play the pellet sfx in 2 parts so that it ends right as we stop eating
		if (!gs.intro && !QB(sfxState, SFXSTATE_EXTRA_LIFE)) {
			if (++sfxPelletMod & 1) {
				TriggerFx(SFX_PELLET1, SFX_VOL_PELLET, true);
			} else {
				TriggerFx(SFX_PELLET2, SFX_VOL_PELLET, true);
			}
		}

		pellets = (gs.intro) ? pellets : pellets - 1;
		pelTmr = (level < 5) ? 4*HZ : 3*HZ;	// Reset global pellet timer

		// Check for release of ghosts trapped in house
		if (pelCount) {
			// Global pellet counter in use
			if (++pelCount == 5) {	// Offset by 1 because we use first value as on/off flag
				ghosts[PINKY].inHouse = false;
				PassPelletCounter();
			} else if (pelCount == 12) {
				ghosts[INKY].inHouse = false;
				PassPelletCounter();
			} else if ((pelCount == 20) && (QB(ghosts[CLYDE].state, NSTATE_HOUSE_ARREST))) {
				// This is where the ghosts can get trapped in the house indefinitely (mimics arcade version)
				ghosts[CLYDE].inHouse = false;
				PassPelletCounter();
				pelCount = 0;
			}
		} else if ((level < 3) && (ghosts[pelIndex].pelCount)) {
			// Individual counters in use
			if (ghosts[pelIndex].inHouse) {
				if (ghosts[pelIndex].pelCount && (--ghosts[pelIndex].pelCount == 0)) {
					ghosts[pelIndex].inHouse = false;
					PassPelletCounter();
				}
			}
		}

		CB(gameMap[cq->mapIndex], 1<<(cq->gridIndex - 1));	// Turn off pellet in gameMap
		SetTile((MAP_OFFSET_X>>3) + loc.x, (MAP_OFFSET_Y>>3) + loc.y, 0);	// Paint blank tile so it appears eaten
		UpdateElroyCutoff();

		// Pellets and energizers score and react differently
		if (IsEnergizer(loc)) {
			gs.fright = GSTATE_FRIGHTENED_INIT;
		} else {
			AddScore(POINTS_PELLET);
		}

		// Optimal to check for fruit appearance here
		if (pellets == 45 || pellets == 109) {	// Same percentage of level completed as arcade
			gs.fruit = GSTATE_FRUIT_INIT;
		}

		// Check for level complete
		if (pellets == 0) {
			lvlTmr = 0;
			SB(gs.general, GSTATE_LVL_COMPLETE);
		}
	}

}


// Yum
void EatFruit(void) {
	if (gs.fruit == GSTATE_FRUIT) {
		if (pacman.loc.x == (fruit.loc.x - (MAP_OFFSET_X>>3)) && pacman.loc.y == (fruit.loc.y - (MAP_OFFSET_Y>>3))) {
			gs.fruit = GSTATE_FRUIT_BONUS_INIT;
		}
	}
}

// Determines if pacman and a ghost/s collided. We only run this check after pacman moves (unless ghosts are frightened). This
// results in a 1 in 8 chance of missing the collision in order to mimic the arcade (which is 1 in 16 due to sprite size).
bool CheckGhostCollision(void) {
	if (!QB(gs.general, GSTATE_BONUS_PAUSE) && !QB(pacman.state, PSTATE_DYING | PSTATE_DEAD | PSTATE_REVIVING)) {
		for (u8 i = 0; i < GHOST_COUNT; i++) {
			if (!QB(ghosts[i].state, NSTATE_DEAD | NSTATE_EATEN)) {
				if ((ghosts[i].loc.x == pacman.loc.x) && (ghosts[i].loc.y == pacman.loc.y)) {
					if (QB(ghosts[i].state, NSTATE_FRIGHTENED | NSTATE_FLASH) || QB(gs.fright, GSTATE_FRIGHTENED_INIT)) {
						// Allow one frame to progress to fright state if pellet space is a dead-heat
						if (!QB(gs.fright, GSTATE_FRIGHTENED_INIT)) {
							freshCorpse = i;
							SB(gs.general, GSTATE_GHOST_EATEN);
						}

						return true;
					} else {
						SB(pacman.state, PSTATE_DYING);
						return true;
					}
				}
			}
		}
	}

	return false;
}


// Checks for map collisions and moves the player, if appropriate
void ProcessInput(void) {
	bool preemptive;
	collQuery cq;
	vec dir, loc, target;
	pt sprLoc;

	if (QB(pacman.state, PSTATE_DYING | PSTATE_DEAD | PSTATE_REVIVING)) {
		return;
	}

	if (QB(pacman.state, PSTATE_AI)) {
		if (Square(pacman.sprLoc.x, pacman.sprLoc.y)) {
			QueryCollisions(pacman.loc, &cq);
			EatPellet(pacman.loc, &cq);
			EatFruit();

			if (AtIntersection(pacman.loc, pacman.sprLoc, pacman.dir)) {
				int temp = MOD2N(frameCount, GHOST_COUNT);

				if (temp == BLINKY) {
					target.x = ghosts[temp].loc.x - ghosts[MAX(0, temp-1)].loc.y;
					target.y = ghosts[temp].loc.y - ghosts[MAX(0, temp-1)].loc.x;
				} else if (temp == PINKY) {
					target.x = ghosts[temp].loc.x + ghosts[MAX(0, temp-1)].loc.y;
					target.y = ghosts[temp].loc.y - ghosts[MAX(0, temp-1)].loc.x;
				} else if (temp == INKY) {
					target.x = ghosts[temp].loc.x - ghosts[MAX(0, temp-1)].loc.y;
					target.y = ghosts[temp].loc.y + ghosts[MAX(0, temp-1)].loc.x;
				} else {
					target.x = ghosts[temp].loc.x + ghosts[MAX(0, temp-1)].loc.y;
					target.y = ghosts[temp].loc.y + ghosts[MAX(0, temp-1)].loc.x;
				}
				NavigateIntersection(target, pacman.loc, &pacman.dir);
				CheckGhostCollision();
				return;
			}
		}
	}

	sprLoc = pacman.sprLoc;
	loc = pacman.loc;
	dir = (pacman.qdir.x != DIR_INVALID) ? pacman.qdir : pacman.dir;

	if (Square(sprLoc.x, sprLoc.y)) {
		preemptive = false;
	} else if (Square(sprLoc.x + pacman.dir.x, sprLoc.y + pacman.dir.y) && (ABS(pacman.dir.x) != ABS(pacman.qdir.x)) &&
			 (ABS(pacman.dir.y) != ABS(pacman.qdir.y))) {	// Turn fast if preemptive input received (pacman gains ground)
		preemptive = true;
	} else {
		CheckGhostCollision();
		return;
	}

	if (preemptive) {
		loc.x += pacman.dir.x;
		loc.y += pacman.dir.y;
	}


	QueryCollisions(loc, &cq);
	EatPellet(loc, &cq);
	EatFruit();
	loc.x += dir.x;
	loc.y += dir.y;
	QueryCollisions(loc, &cq);

	if (QB(pacman.state, PSTATE_MOVING)) {
		// Check for map collision
		if (cq.collided) {
			if (!preemptive) {
				if (pacman.qdir.x != DIR_INVALID) {
					// New input map collision - check with current speed
					loc.x = pacman.loc.x + pacman.dir.x;
					loc.y = pacman.loc.y + pacman.dir.y;
					QueryCollisions(loc, &cq);
				}

				if (cq.collided) {
					// Map collision - stop player
					CB(pacman.state, PSTATE_MOVING);
					pacman.qdir.x = DIR_INVALID;
				}
			}
		} else {
			// No map collision based on new input - permit location update
			if (preemptive) {
				pacman.sprLoc.x += pacman.dir.x;
				pacman.sprLoc.y += pacman.dir.y;
				pacman.loc.x += pacman.dir.x;
				pacman.loc.y += pacman.dir.y;
			}

			pacman.dir = dir;
			pacman.qdir.x = DIR_INVALID;
		}
	}
	CheckGhostCollision();
}


// Index into our map's meta data to determine what's legal
void QueryCollisions(vec loc, collQuery *cq) {
	// Zero-out result
	cq->collided = false;
	cq->mapIndex = ((loc.x)>>2) + loc.y * GAME_MAP_WID;
	cq->gridIndex = 7 - (MOD2N(loc.x, 4)<<1);

	if (!QB(gameMap[cq->mapIndex], (1<<cq->gridIndex))) {
		cq->collided = true;
	}
}


u8 GetAnimDir(vec dir) {
	if (dir.x == 0) {
		// up or down
		if (dir.y == 1) {
			return ANIM_DN;
		} else {
			return ANIM_UP;
		}
	} else {
		// left or right
		if (dir.x == 1) {
			return ANIM_RT;
		} else if (dir.x == -1) {
			return ANIM_LT;
		} else {
			return ANIM_NONE;
		}
	}
}


// Load speed from flash LUT
void SetPacmanSpeed(void) {
	u8 i, nspd, fspd;

	if (QB(gs.general, GSTATE_INTERMISSION)) {
		i = 8;
	} else if (gs.intro) {
		i = 10;
	} else {
		if (level == 1) {
			i = 0;
		} else if (level < 5) {
			i = 2;
		} else if (level < 21) {
			i = 4;
		} else {
			i = 6;
		}
	}

	nspd = pgm_read_byte(pacmanSpdLUT+i++);
	fspd = pgm_read_byte(pacmanSpdLUT+i);
	memcpy_P(&pacman.nspd, spdLut+nspd, sizeof(speed));
	memcpy_P(&pacman.fspd, spdLut+fspd, sizeof(speed));
	pacman.spd = &pacman.nspd;
}


// Load speed from flash LUT
void SetGhostSpeed(u8 g) {
	u8 i, nspd, fspd, tspd;

	if ((g == BLINKY) && QB(elroy, NSTATE_ELROY1 | NSTATE_ELROY2)) {
		i = (QB(elroy, NSTATE_ELROY1)) ? 0 : 1;

		if (level == 1) {
			i += 0;
		} else if (level < 5) {
			i += 2;
		} else {
			i += 4;
		}
		// Maintain original fright and tunnel speed
		nspd = pgm_read_byte(elroySpdLUT+i);
		memcpy_P(&ghosts[g].nspd, spdLut+nspd, sizeof(speed));
		return;
	} else {
		if QB(gs.general, GSTATE_INTERMISSION) {
			i = 12;
		} else if (gs.intro) {
			i = 15;
		} else {
			if (level == 1) {
				i = 0;
			} else if (level < 5) {
				i = 3;
			} else if (level < 21) {
				i = 6;
			} else {
				i = 9;
			}
		}

		nspd = pgm_read_byte(ghostSpdLUT+i++);
		fspd = pgm_read_byte(ghostSpdLUT+i++);
		tspd = pgm_read_byte(ghostSpdLUT+i);
		memcpy_P(&ghosts[g].nspd, spdLut+nspd, sizeof(speed));
		memcpy_P(&ghosts[g].tspd, spdLut+tspd, sizeof(speed));
		memcpy_P(&ghosts[g].fspd, spdLut+fspd, sizeof(speed));
		ghosts[g].spd = &ghosts[g].nspd;
	}
}


void UpdateSpeed(speed *s) {
	if (frameCount == 0) {
		s->moveCount = 0;
		s->pauseCount = 0;
		s->mode = s->modeInit;
	}

	if (s->mode == SPEED_MODE_MOVE) {
		if (s->moveCount++ == s->moveMax) {
			s->moveCount = 0;
			s->mode = SPEED_MODE_PAUSE;
		}
	} else if (s->pauseCount++ == s->pauseMax) {
		s->pauseCount = 0;
		s->mode = SPEED_MODE_MOVE;
	}
}


void HideSprite(u8 spriteIndex, u8 wid, u8 hgt) {
	MoveSprite(spriteIndex, SCREEN_TILES_H * TILE_WID, SCREEN_TILES_V * TILE_HGT, wid, hgt);
}


void DrawScore(u8 x, u8 y, u32 score) {
	PacmanPrintLong(x, y, score);

	if (score == 0) {
		PacmanPrintByte(x - 1, y, 0);
	}
}


void DrawLives(void) {
	for (u8 i = 0; i < MAX_LIVES; i++) {
		// Blank out canvas (Fill references a different tile, so just use SetTile)
		SetTile(LIFE_POS_X + i, LIFE_POS_Y, 0);
		// Draw lives icons
		if (i < lives) {
			SetTile(LIFE_POS_X + i, LIFE_POS_Y, LIFE_SPRITE);
		}
	}
}


// Overly complicated to mimic arcade version
void UpdatePursuitMode(void) {
	bool scatter;

	if (level == 1) {
		if (lvlTmr <= 84*HZ) {
			if ((lvlTmr == 27*HZ) || (lvlTmr == 54*HZ) || (lvlTmr == 79*HZ)) {
				scatter = true;
			} else if ((lvlTmr == 7*HZ) || (lvlTmr == 34*HZ) || (lvlTmr == 59*HZ) || (lvlTmr == 84*HZ)) {
				scatter = false;
			} else {
				return;
			}
		} else {
			return;
		}
	} else if (level < 5) {
		if (lvlTmr <= 59*HZ) {
			if ((lvlTmr == 27*HZ) || (lvlTmr == 54*HZ)) {
				scatter = true;
			} else if ((lvlTmr == 7*HZ) || (lvlTmr == 34*HZ) || (lvlTmr == 59*HZ)) {
				scatter = false;
			} else {
				return;
			}
		} else {
			return;
		}
	} else {
		if (lvlTmr <= 55*HZ) {
			if ((lvlTmr == 25*HZ) || (lvlTmr == 50*HZ)) {
				scatter = true;
			} else if ((lvlTmr == 5*HZ) || (lvlTmr == 30*HZ) || (lvlTmr == 55*HZ)) {
				scatter = false;
			} else {
				return;
			}
		} else {
			return;
		}
	}

	gs.pursuit = (scatter) ? GSTATE_SCATTER_INIT : GSTATE_CHASE_INIT;
}


vec GetDistance(vec origin, vec dest) {
	vec dist;

	dist.x = dest.x - origin.x;
	dist.y = dest.y - origin.y;
	return dist;
}


// Determines which way to turn at an intersection for both ghosts and demo AI. Rules
// based on arcade version and shortest distance to target
void NavigateIntersection(vec dest, vec loc, vec *dir) {
	vec origin = loc;
	vec delta = GetDistance(origin, dest), dirList[4] = { { 0, 0 } };

	u8 absX, absY, dirX, dirY;

	absX = ABS(delta.x);
	absY = ABS(delta.y);

	// Normalize distances into directions
	dirX = (delta.x > 0) ? 1 : -1;	// Favour left
	dirY = (delta.y > 0) ? 1 : -1;	// Favour up

	u8 i = 0;

	if (absX > absY) {
		// Attempt to shorten longest axis first (lengthen it as last resort)
		dirList[i++].x = dirX;
		dirList[i++].y = dirY;
		dirList[i++].y = -dirY;
		dirList[i].x = -dirX;
	} else if (absX < absY) {
		// Attempt to shorten longest axis first (lengthen it as last resort)
		dirList[i++].y = dirY;
		dirList[i++].x = dirX;
		dirList[i++].x = -dirX;
		dirList[i].y = -dirY;
	} else {
		// Tie-breaker priority: up,left,down,right (CCW from 12)
		if (dirY == DIR_UP) {
			dirList[i++].y = dirY;	// Up`
			dirList[i++].x = dirX;	// Left/Right
		} else if (dirX == DIR_LEFT) {
			dirList[i++].x = dirX;	// Left
			dirList[i++].y = dirY;	// Down
		} else {
			dirList[i++].y = dirY;	// Down
			dirList[i++].x = dirX;	// Right
		}

		if (-dirY == DIR_UP) {
			dirList[i++].y = -dirY;	// Up
			dirList[i].x = -dirX;	// Left/Right
		} else if (-dirX == DIR_LEFT) {
			dirList[i++].x = -dirX;	// Left
			dirList[i].y = -dirY;	// Down
		} else {
			dirList[i++].y = -dirY;	// Down
			dirList[i].x = -dirX;	// Right
		}
	}

	collQuery cq;

	for (i = 0; i < 4; i++) {
		// Don't reverse direction
		if (dirList[i].x != -dir->x || dirList[i].y != -dir->y) {
			origin.x += dirList[i].x;
			origin.y += dirList[i].y;
			QueryCollisions(origin, &cq);

			if (!cq.collided) {
				*dir = dirList[i];
				return;
			} else {
				// Undo last check
				origin = loc;
			}
		}
	}
}


// We are at an intersection if there is an unblocked perpendicular direction along which to travel.
// NOTE: May use LUT if this consumes too many cycles and we have spare ram.
bool AtIntersection(vec loc, pt sprLoc, vec dir) {
	collQuery cq;

	// Removed Square check as it is done in AI section and this is only called from there, currently
	if (dir.x == 0) {
		// Travelling up/down
		loc.x += 1;	// Check right
		QueryCollisions(loc, &cq);

		if (cq.collided) {
			loc.x -= 2;	// Check left
			QueryCollisions(loc, &cq);

			if (cq.collided) {
				// Both left and right blocked
				return false;
			}
		}
	} else {
		// Travelling left/right
		loc.y += 1;	// Check down
		QueryCollisions(loc, &cq);

		if (cq.collided) {
			loc.y -= 2;	// Check up
			QueryCollisions(loc, &cq);

			if (cq.collided) {
				// Both up and down blocked
				return false;
			}
		}
	}
	return true;
}

// We attempt to initially stagger AI calculations by offsetting ghosts within their 8-pixel squares in InitLevel().
// Ghosts only think when on a pixel whose coordinates are evenly divisible by 8 (as this is the only time
// that they can turn). Once deaths and tunnel speed changes start occurring, AI calcs naturally jumble
void DoGhostAI(u8 g) {
	vec target = { 0, 0 };

	// Ensure we can act on our result
	if (ghosts[g].spd->mode != SPEED_MODE_MOVE) {
		return;
	}

	// House arrest
	if (QB(ghosts[g].state, NSTATE_HOUSE_ARREST)) {
		if (ghosts[g].inHouse || !Square(ghosts[g].sprLoc.x, ghosts[g].sprLoc.y)) {
			if (ghosts[g].sprLoc.y == (MAP_OFFSET_Y + (ghosts[g].hloc.y * TILE_HGT))) {
				ghosts[g].dir.y = 1;
			} else if (ghosts[g].sprLoc.y == (MAP_OFFSET_Y + TILE_HGT + (ghosts[g].hloc.y * TILE_HGT))) {
				ghosts[g].dir.y = -1;
			}
			return;
		} else {
			CB(ghosts[g].state, NSTATE_HOUSE_ARREST);
			SB(ghosts[g].state, NSTATE_REVIVED);

			if ((g == BLINKY) || (g == PINKY)) {
				ghosts[g].dir.x = 0;
				ghosts[g].dir.y = -1;
			} else if (g == INKY) {
				ghosts[g].dir.x = 1;
				ghosts[g].dir.y = 0;
			} else {
				ghosts[g].dir.x = -1;
				ghosts[g].dir.y = 0;
			}
		}
	}

	// Ghosts only need to think when they can turn (outside of house arrest)
	if (!Square(ghosts[g].sprLoc.x, ghosts[g].sprLoc.y)) {
		return;
	}

	// Ghost house quirks (because we have no meta data to guide us)
	if (QB(ghosts[g].state, NSTATE_DEAD)) {
		if (OUTSIDE_GHOST_HOUSE_DOOR(g)) {
			ghosts[g].dir.x = 0;
			ghosts[g].dir.y = 1;
			return;
		} else if (IN_GHOST_HOUSE(g)) {
			CB(ghosts[g].state, NSTATE_DEAD);
			CB(sfxState, (SFXSTATE_DEAD_BLINKY<<g));
			LoadGhostAnimations(GetAnimDir(ghosts[g].dir), g);

			// Trap in house if under global/personal pellet counter
			if ((g == BLINKY) || (!pelCount && (ghosts[g].pelCount == 0))) {
				SB(ghosts[g].state, NSTATE_REVIVED);
				ghosts[g].dir.x *= -1;
				ghosts[g].dir.y *= -1;
			} else {
				SB(ghosts[g].state, NSTATE_HOUSE_ARREST);
				ghosts[g].inHouse = true;

				// Check to see if new arrival has preference to hold counter
				if (!QB(ghosts[pelIndex].state, NSTATE_HOUSE_ARREST) || (g < pelIndex)) {
					pelIndex = g;
				}

				ghosts[g].dir.x = 0;
				ghosts[g].dir.y = -1;
			}
			return;
		}
	} else if (QB(ghosts[g].state, NSTATE_REVIVED)) {
		if (OUTSIDE_GHOST_HOUSE_DOOR(g)) {
			CB(ghosts[g].state, NSTATE_REVIVED);
			SB(ghosts[g].state, NSTATE_EMPTY_NEST);
		} else if (INSIDE_GHOST_HOUSE_DOOR(g)) {
			ghosts[g].dir.x = 0;
			ghosts[g].dir.y = -1;
			return;
		}
	}

	// Ensure calculation will return a valid answer
	if (!AtIntersection(ghosts[g].loc, ghosts[g].sprLoc, ghosts[g].dir)) {
		return;
	}

	// Can't turn upwards on restricted tiles (helps pacman escape)
	if (!QB(ghosts[g].state, NSTATE_DEAD)) {
		if (!QB(ghosts[g].state, NSTATE_FRIGHTENED | NSTATE_FLASH)) {
			if (ghosts[g].dir.y != DIR_DOWN) {
				if ((ghosts[g].loc.x == GHOST_NOTURN_AX) && (ghosts[g].loc.y == GHOST_NOTURN_AY)) {
					return;
				} else if ((ghosts[g].loc.x == GHOST_NOTURN_BX) && (ghosts[g].loc.y == GHOST_NOTURN_BY)) {
					return;
				} else if ((ghosts[g].loc.x == GHOST_NOTURN_CX) && (ghosts[g].loc.y == GHOST_NOTURN_CY)) {
					return;
				} else if ((ghosts[g].loc.x == GHOST_NOTURN_DX) && (ghosts[g].loc.y == GHOST_NOTURN_DY)) {
					return;
				}
			}
		}
	}

	if (QB(ghosts[g].state, NSTATE_DEAD)) {
		target = ghosts[g].hloc;
	} else if (QB(ghosts[g].state, NSTATE_FRIGHTENED | NSTATE_FLASH)) {
		target.x = PrngNext();
		target.y = PrngNext();
	} else if (gs.pursuit == GSTATE_SCATTER) {
		if ((g == BLINKY) && QB(elroy, NSTATE_ELROY1 | NSTATE_ELROY2)) {
			// Elroy treats scatter as chase
			target = pacman.loc;
		} else {
			// Scatter mode
			target = ghosts[g].sloc;
		}
	} else {	// Chase mode
		switch (g) {
			case BLINKY:
				target = pacman.loc;
				break;
			case PINKY:
				target.x = pacman.loc.x + pacman.dir.x * 4;
				target.y = pacman.loc.y + pacman.dir.y * 4;
				// Maintain logic bug from arcade version
				if (pacman.dir.y == -1) {
					target.x = pacman.loc.x - 4;
				}
				break;
			case INKY:
				// 2 squares ahead of pacman
				target.x = pacman.loc.x + pacman.dir.x * 2;
				target.y = pacman.loc.y + pacman.dir.x * 2;
				// Maintain logic bug from arcade version
				if (pacman.dir.y == -1) {
					target.x = pacman.loc.x - 2;
				}
				// Add blinky's delta from this location
				target.x += target.x - ghosts[BLINKY].loc.x;
				target.y += target.y - ghosts[BLINKY].loc.y;
				break;
			case CLYDE:
			{
				int dx, dy;
				dx = ghosts[g].loc.x - pacman.loc.x;
				dy = ghosts[g].loc.y - pacman.loc.y;

				if (NewtonSqrt(dx*dx + dy*dy, 2) >= 8) {
					target = pacman.loc;
				} else {
					target = ghosts[g].sloc;
				}
			}
				break;
		}
	}

	NavigateIntersection(target, ghosts[g].loc, &ghosts[g].dir);
}



void MovePacman(void) {
	if (QB(pacman.state, PSTATE_DEAD)) {
		pacman.anim->disp++;
	} else if (pacman.spd->mode == SPEED_MODE_MOVE) {
		if (QB(pacman.state, PSTATE_MOVING)) {
			pacman.sprLoc.x += pacman.dir.x;
			pacman.sprLoc.y += pacman.dir.y;

			// Tunnel teleport
			if (!QB(gs.general, GSTATE_INTERMISSION) && !gs.intro) {
				if ((pacman.dir.x == DIR_LEFT) && (pacman.sprLoc.x <= TUNNEL_LEFT)) {
					pacman.sprLoc.x = TUNNEL_RIGHT;
				} else if ((pacman.dir.x == DIR_RIGHT) && (pacman.sprLoc.x >= TUNNEL_RIGHT)) {
					pacman.sprLoc.x = TUNNEL_LEFT;
				}
			}

			pacman.anim->disp++;
			MoveSprite(pacman.sprite, pacman.sprLoc.x - TILE_WID2, pacman.sprLoc.y - TILE_HGT2, pacman.anim->wid, pacman.anim->hgt);

			// Update grid position when our center-point is entering a new 8x8 grid position
			if (MOD2N(pacman.sprLoc.x - TILE_WID2, TILE_WID) == 0 && MOD2N(pacman.sprLoc.y - TILE_HGT2, TILE_HGT) == 0) {
				pacman.loc.x = (pacman.sprLoc.x - MAP_OFFSET_X)>>3;
				pacman.loc.y = (pacman.sprLoc.y - MAP_OFFSET_Y)>>3;
			}
		}
	}
}


void MoveGhost(u8 g) {
	if (ghosts[g].spd->mode == SPEED_MODE_MOVE) {
		if (QB(ghosts[g].state, NSTATE_MOVING)) {
			ghosts[g].sprLoc.x += ghosts[g].dir.x;
			ghosts[g].sprLoc.y += ghosts[g].dir.y;

			// Tunnel teleport
			if (!QB(gs.general, GSTATE_INTERMISSION)) {
				if ((ghosts[g].dir.x == DIR_LEFT) && (ghosts[g].sprLoc.x <= TUNNEL_LEFT)) {
					ghosts[g].sprLoc.x = TUNNEL_RIGHT;
				} else if ((ghosts[g].dir.x == DIR_RIGHT) && (ghosts[g].sprLoc.x >= TUNNEL_RIGHT)) {
					ghosts[g].sprLoc.x = TUNNEL_LEFT;
				}
			}

			ghosts[g].anim->disp++;
			MoveSprite(ghosts[g].sprite, ghosts[g].sprLoc.x - TILE_WID2, ghosts[g].sprLoc.y - TILE_HGT2, ghosts[g].anim->wid, ghosts[g].anim->hgt);

			// Update grid position when our center-point is entering a new 8x8 grid position
			if (MOD2N(ghosts[g].sprLoc.x - TILE_WID2, TILE_WID) == 0 && MOD2N(ghosts[g].sprLoc.y - TILE_HGT2, TILE_HGT) == 0) {
				ghosts[g].loc.x = (ghosts[g].sprLoc.x - MAP_OFFSET_X)>>3;
				ghosts[g].loc.y = (ghosts[g].sprLoc.y - MAP_OFFSET_Y)>>3;
			}
		}
	}
}


void DrawPacman(void) {
	u8 limit = pacman.anim->currFrame + pacman.anim->size;

	for (u8 i = pacman.anim->currFrame, j = 0; i < limit; i++, j++) {
		sprites[pacman.sprite + j].tileIndex = pgm_read_byte(&(pacman.anim->frames[i]));
	}
}


void DrawGhost(u8 g) {
	u8 limit = ghosts[g].anim->currFrame + ghosts[g].anim->size;

	for (u8 i = ghosts[g].anim->currFrame, j = 0; i < limit; i++, j++) {
		sprites[ghosts[g].sprite + j].tileIndex = pgm_read_byte(&(ghosts[g].anim->frames[i]));
	}
}


void AnimatePacman(void) {
	if (DirDiff(pacman.dir, pacman.prevDir)) {
		u8 animdir = GetAnimDir(pacman.dir);

		if (animdir != ANIM_NONE) {
			LoadPacmanAnimations(animdir);
			DrawPacman();
		}
	}

	if ((!QB(pacman.state, PSTATE_MOVING | PSTATE_DEAD)) && (pacman.anim->currFrame != 1)) {
		// Stationary pacman always has his mouth open
		pacman.anim->disp = 0;
		pacman.anim-> currFrame = 1;
		DrawPacman();
	} else if (pacman.anim->disp == pacman.anim->dpf) {
		pacman.anim->disp = 0;
		pacman.anim->currFrame += pacman.anim->size;

		if (pacman.anim->currFrame == pacman.anim->frameCount) {
			pacman.anim->currFrame = 0;

			if (QBC(pacman.state, PSTATE_DEAD)) {
				SB(pacman.state, PSTATE_REVIVING);
				HideSprite(pacman.sprite, pacman.anim->wid, pacman.anim->hgt);
			}
		}

		DrawPacman();
	}
	pacman.prevDir = pacman.dir;
}


void AnimateGhost(u8 g) {
	// Frightened ghosts do not have directional animations
	if (DirDiff(ghosts[g].dir, ghosts[g].prevDir) && (!(QB(ghosts[g].state, NSTATE_FRIGHTENED) || QB(ghosts[g].state, NSTATE_FLASH)))) {
		u8 animdir = GetAnimDir(ghosts[g].dir);

		if (animdir != ANIM_NONE) {
			LoadGhostAnimations(animdir, g);
			DrawGhost(g);
		}
	}

	if (ghosts[g].anim->disp == ghosts[g].anim->dpf) {
		ghosts[g].anim->disp = 0;
		ghosts[g].anim->currFrame += ghosts[g].anim->size;

		if (ghosts[g].anim->currFrame == ghosts[g].anim->frameCount) {
			ghosts[g].anim->currFrame = 0;

			if (ghosts[g].flashCount) {
				ghosts[g].flashCount -= 1;
			}
		}
		DrawGhost(g);
	}
	ghosts[g].prevDir = ghosts[g].dir;
}


u16 GetInput(void) {
	static bool release = true;
	static u16 ctl;

	ctl = ReadJoypad(0);

	if (!QB(pacman.state, PSTATE_AI)) {
		if (ctl&BTN_LEFT) {
			pacman.qdir.x = -1;
			pacman.qdir.y = 0;
		} else if (ctl&BTN_RIGHT) {
			pacman.qdir.x = 1;
			pacman.qdir.y = 0;
		} else if (ctl&BTN_UP) {
			pacman.qdir.x = 0;
			pacman.qdir.y = -1;
		} else if (ctl&BTN_DOWN) {
			pacman.qdir.x = 0;
			pacman.qdir.y = 1;
		}

		if (ctl&BTN_B) {
			if (release) {
				release = false;
				sirenToggle = !sirenToggle;
			}
		} else {
			release = true;
		}
	}

	if (ctl&(BTN_LEFT | BTN_RIGHT | BTN_UP | BTN_DOWN)) {
		SB(pacman.state, PSTATE_MOVING);
	}

	return ctl;	// New input acquired
}


// Updates fruit struct map pointers and score bonuses
void SetFruit(void) {
	switch (level) {
		case 1:
			fruit.tile = FRUIT_TILE_CHERRY;
			fruit.bonusMap = map100;
			fruit.bonus = 100;
			break;
		case 2:
			fruit.tile = FRUIT_TILE_STRAWBERRY;
			fruit.bonusMap = map300;
			fruit.bonus = 300;
			break;
		case 3:
		case 4:
			fruit.tile = FRUIT_TILE_PEACH;
			fruit.bonusMap = map500;
			fruit.bonus = 500;
			break;
		case 5:
		case 6:
			fruit.tile = FRUIT_TILE_APPLE;
			fruit.bonusMap = map700;
			fruit.bonus = 700;
			break;
		case 7:
		case 8:
			fruit.tile = FRUIT_TILE_MELON;
			fruit.bonusMap = map1000;
			fruit.bonus = 1000;
			break;
		case 9:
		case 10:
			fruit.tile = FRUIT_TILE_GALAXIAN;
			fruit.bonusMap = map2000;
			fruit.bonus = 2000;
			break;
		case 11:
		case 12:
			fruit.tile = FRUIT_TILE_BELL;
			fruit.bonusMap = map3000;
			fruit.bonus = 3000;
			break;
		default:
			fruit.tile = FRUIT_TILE_KEY;
			fruit.bonusMap = map5000;
			fruit.bonus = 5000;
			break;
	}

	fruit.activeTmr = 0;
	fruit.bonusTmr = 0;
	fruit.loc.x = (MAP_OFFSET_X>>3) + 9;
	fruit.loc.y = (MAP_OFFSET_Y>>3) + 12;
	fruit.fruitCounter[(level - 1) % 6] = fruit.tile;
}


// We load animations when they are required or they would consume too much RAM
void LoadPacmanAnimations(u8 index) {
	memcpy_P(&playerAnims, gameAnimations + index, sizeof(animation));
	pacman.anim = playerAnims;
}


// We load animations when they are required or they would consume too much RAM
void LoadGhostAnimations(u8 index, u8 g) {
	u8 offset = 0;

	switch (g) {	// Do in reverse and allow fall-through
		case CLYDE:
			offset += INKY_ANIMS_COUNT;
		case INKY:
			offset += PINKY_ANIMS_COUNT;
		case PINKY:
			offset += BLINKY_ANIMS_COUNT;
		case BLINKY:
			offset += PACMAN_ANIMS_COUNT;
		default:
			offset += index;
	}
	if (QB(ghosts[g].state, NSTATE_DEAD)) {
		offset += GHOST_ANIM_DEAD_OFFSET;
	}
	memcpy_P(&ghostAnims[g], gameAnimations + offset, sizeof(animation));
	ghosts[g].anim = &ghostAnims[g];
}


// Intro screen introduces the ghosts and demonstrates their vulnerability
u8 DisplayIntroScreen(u8 reset) {
	static u16 introTmr = 0;
	u8 scenePlaying = 4, entrance = GHOST_COUNT + 1;

	scrnState = SSTATE_INTRO;

	if (QB(reset, RESET_ALL)) {
		ClearVram();
		memcpy_P(gameMap, gameMapPgm, GAME_MAP_WID * GAME_MAP_HGT);
		memcpy_P(gameMap + (GAME_MAP_WID*16), introMapPatchPgm, GAME_MAP_WID);
		PacmanPrint(SCORE_X - 3, SCORE_Y - 1, p1Score, FONT_WHITE);
		PacmanPrint(HSCORE_X - 7, HSCORE_Y - 1, hs, FONT_WHITE);
		DrawScore(HSCORE_X, HSCORE_Y, highscore);
		DrawScore(SCORE_X, SCORE_Y, score);
		PacmanPrint(SCORE_X - 4, SCORE_Y + 3, charNick, FONT_WHITE);
		PacmanPrint(SCORE_X - 8, SCORE_Y + 24, creditText, FONT_WHITE);
		PacmanPrintByte(SCORE_X + 1, SCORE_Y + 24, credit);
		SetFruit();
		DrawFruitLog();
		introTmr = 11*HZ + 1;
	} else if (QB(reset, RESET_TMRS)) {
		introTmr = 11*HZ + 1;
	}

	if (introTmr && (--introTmr != 0)) {
		if (introTmr == 10*HZ) {
			SetTile(SCORE_X - 5, SCORE_Y + 6, 14);
			PacmanPrint(SCORE_X - 2, SCORE_Y + 6, shadow, FONT_RED);
		} else if (introTmr == 9*HZ) {
			PacmanPrint(SCORE_X + 8, SCORE_Y + 6, blinkyName, FONT_RED);
		} else if (introTmr == 8*HZ) {
			SetTile(SCORE_X - 5, SCORE_Y + 9, 15);
			PacmanPrint(SCORE_X - 2, SCORE_Y + 9, speedy, FONT_PINK);
		} else if (introTmr == 7*HZ) {
			PacmanPrint(SCORE_X + 8, SCORE_Y + 9, pinkyName, FONT_PINK);
		} else if (introTmr == 6*HZ) {
			SetTile(SCORE_X - 5, SCORE_Y + 12, 16);
			PacmanPrint(SCORE_X - 2, SCORE_Y + 12, bashful, FONT_AQUA);
		} else if (introTmr == 5*HZ) {
			PacmanPrint(SCORE_X + 8, SCORE_Y + 12, inkyName, FONT_AQUA);
		} else if (introTmr == 4*HZ) {
			SetTile(SCORE_X - 5, SCORE_Y + 15, 17);
			PacmanPrint(SCORE_X - 2, SCORE_Y + 15, pokey, FONT_ORANGE);
		} else if (introTmr == 3*HZ) {
			PacmanPrint(SCORE_X + 8, SCORE_Y + 15, clydeName, FONT_ORANGE);
		} else if (introTmr == 2*HZ) {
			SetTile(SCORE_X + 2, SCORE_Y + 19, 132);
			PacmanPrintByte(SCORE_X + 5, SCORE_Y + 19, 10);
			PacmanPrint(SCORE_X + 7, SCORE_Y + 19, pts, FONT_WHITE);
			SetTile(SCORE_X + 2, SCORE_Y + 20, 133);
			PacmanPrintByte(SCORE_X + 5, SCORE_Y + 20, 50);
			PacmanPrint(SCORE_X + 7, SCORE_Y + 20, pts, FONT_WHITE);
		} else if (introTmr == HZ) {
			PacmanPrint(SCORE_X - 4, SCORE_Y + 22, copyright, FONT_PINK);
		} else if (introTmr == 1) {
			// Pacman
			SetLoc(TI_PACMAN_START_X>>3, TI_PACMAN_START_Y>>3, pacman.loc);
			SetLoc(MAP_OFFSET_X + TI_PACMAN_START_X + TILE_WID2, MAP_OFFSET_Y + TI_PACMAN_START_Y + TILE_HGT2, pacman.sprLoc);
			SetDir(DIR_LEFT, 0, pacman.dir);
			SetDir(DIR_INVALID, 0, pacman.qdir);
			SetDir(0, 0, pacman.prevDir);
			MoveSprite(pacman.sprite, pacman.sprLoc.x, pacman.sprLoc.y - TILE_HGT2, pacman.anim->wid, pacman.anim->hgt);
			pacman.state = PSTATE_MOVING;
			SetPacmanSpeed();
			MapSprite(CURTAIN_SPRITE_SML, mapCurtainSml);	// So that actors ease their way onto the stage
			MoveSprite(CURTAIN_SPRITE_SML, pacman.sprLoc.x, pacman.sprLoc.y - TILE_HGT2, 1, 1);
		}
	} else {
		SetDir(DIR_INVALID, 0, pacman.qdir);
		ProcessInput();
		UpdateSpeed(pacman.spd);
		MovePacman();
		AnimatePacman();

		if (MOD2N(frameCount, 16) == 0) {
			FlashEnergizers();
		}

		switch (pacman.loc.x) {
			case ((TI_PACMAN_START_X>>3) - 1):
				entrance = BLINKY;
				break;
			case ((TI_PACMAN_START_X>>3) - 2):
				entrance = PINKY;
				break;
			case ((TI_PACMAN_START_X>>3) - 3):
				entrance = INKY;
				break;
			case ((TI_PACMAN_START_X>>3) - 4):
				entrance = CLYDE;
				break;
		}

		if ((entrance >= BLINKY) && (entrance <= CLYDE)) {
			SetLoc(TI_PACMAN_START_X>>3, TI_PACMAN_START_Y>>3, ghosts[entrance].loc);
			SetLoc(MAP_OFFSET_X + TI_PACMAN_START_X + TILE_WID2, MAP_OFFSET_Y + TI_PACMAN_START_Y + TILE_HGT2, ghosts[entrance].sprLoc);
			SetDir(DIR_LEFT, 0, ghosts[entrance].dir);
			SetDir(0, 0, ghosts[entrance].prevDir);
			MoveSprite(ghosts[entrance].sprite, ghosts[entrance].sprLoc.x, ghosts[entrance].sprLoc.y - TILE_HGT2, ghosts[entrance].anim->wid,
				ghosts[entrance].anim->hgt);
			ghosts[entrance].state = NSTATE_MOVING;
			SetGhostSpeed(entrance);
			entrance = GHOST_COUNT + 1;
		}

		for (u8 i = 0; i < GHOST_COUNT; i++) {
			if (!QB(ghosts[i].state, NSTATE_DEAD)) {
				UpdateSpeed(ghosts[i].spd);
				MoveGhost(i);
				AnimateGhost(i);
			}

			scenePlaying = (!QB(ghosts[i].state, NSTATE_DEAD)) ? scenePlaying : scenePlaying - 1;
		}

		if ((pacman.loc.x == 0) && (pacman.dir.x == DIR_LEFT)) {
			pacman.dir.x = DIR_RIGHT;
		}
		++frameCount;
	}

	return scenePlaying;
}


// Displays the simple, authentic title screen
u16 DisplayTitleScreen(u8 reset) {
	static u16 titleTmr = 0;

	scrnState = SSTATE_TITLE;

	if (QB(reset, RESET_ALL)) {
		CB(pacman.state, PSTATE_MOVING);
		HideSprite(pacman.sprite, pacman.anim->wid, pacman.anim->hgt);

		for (u8 i = 0; i < GHOST_COUNT; i++) {
			CB(ghosts[i].state, NSTATE_MOVING | NSTATE_DEAD);
			HideSprite(ghosts[i].sprite, ghosts[i].anim->wid, ghosts[i].anim->hgt);
		}

		ClearVram();
		PacmanPrint(SCORE_X - 3, SCORE_Y - 1, p1Score, FONT_WHITE);
		PacmanPrint(HSCORE_X - 7, HSCORE_Y - 1, hs, FONT_WHITE);
		DrawScore(HSCORE_X, HSCORE_Y, highscore);
		DrawScore(SCORE_X, SCORE_Y, score);
		PacmanPrint(SCORE_X - 3, SCORE_Y + 8, pushStartButton, FONT_ORANGE);
		PacmanPrint(SCORE_X - 1, SCORE_Y + 12, onePlayerOnly, FONT_AQUA);
		PacmanPrint(SCORE_X - 8, SCORE_Y + 16, bonusPacman, FONT_BROWN);
		PacmanPrint(SCORE_X - 5, SCORE_Y + 20, copyright, FONT_PINK);
		PacmanPrint(SCORE_X - 8, SCORE_Y + 24, creditText, FONT_WHITE);
		PacmanPrintByte(SCORE_X + 1, SCORE_Y + 24, credit);
		titleTmr = 5*HZ;
	}

	if (QB(reset, RESET_TMRS)) {
		titleTmr = 5*HZ;
	} else {
		titleTmr = (!titleTmr) ? titleTmr : titleTmr - 1;
	}
	return titleTmr;
}

// The fruit log keeps track of the most recently eaten fruit
void DrawFruitLog(void) {
	u8 offset = (gs.intro) ? 1 : 0;

	for (u8 i = 0; i < MIN(level, 6); i++) {
		if (level < 6) {
			SetTile(FRUIT_LOG_X - i, FRUIT_LOG_Y + offset, fruit.fruitCounter[i] + fontOffset);
		} else {
			SetTile(FRUIT_LOG_X - i, FRUIT_LOG_Y + offset, fruit.fruitCounter[(i + (level % 6)) % 6] + fontOffset);
		}
	}
}


void AddScore(u16 s) {
	score += s;
	DrawScore(SCORE_X, SCORE_Y, score);

	if (score > highscore) {
		highscore = score;
		DrawScore(HSCORE_X, HSCORE_Y, highscore);
	}

	if ((score - scoreMod) >= 10000) {
		lives = MIN(lives + 1, MAX_LIVES);
		scoreMod += 10000;
		DrawLives();
		SB(sfxState, SFXSTATE_EXTRA_LIFE);
		extraLifeCount = 129;	// 8 extra life dings
	}
}


// We re-use this function upon death but avoid parts of it by setting reset to false
void InitLevel(bool reset) {
	if (reset) {
		ClearVram();
	}

	// Fruit inits
	if (QBC(gs.fruit, GSTATE_FRUIT)) {
		fruit.activeTmr = 0;
		SetTile(fruit.loc.x, fruit.loc.y, 0);
	} else if (QBC(gs.fruit, GSTATE_FRUIT_BONUS)) {
		fruit.bonusTmr = 0;
		HideSprite(FRUIT_BONUS_SPRITE, pgm_read_byte(&fruit.bonusMap[0]), pgm_read_byte(&fruit.bonusMap[1]));
	}

	// Reset timers, counters, pointers
	if (reset) {
		pellets = MAX_PELLETS;
		elroy = 0;
		prng = 0;	// Reset to accommodate patterns
		pelIndex = (level == 1) ? INKY : ((level == 2) ? CLYDE : PINKY);
		pelCount = 0;
		pelTmr = (level < 5) ? 4*HZ : 3*HZ;
	}

	// Demo AI varies based on framecount, so don't reset
	if (!QB(pacman.state, PSTATE_AI)) {
		frameCount = 0;
		scrnState = SSTATE_GAME;
	} else {
		scrnState = SSTATE_DEMO;
	}

	// Fright timer/bonus/map reset
	frightTmr = 0;
	killBonus = 200;
	killBonusMap = map200;
	HideSprite(CURTAIN_SPRITE_LGE, 2, 2);	// Uses same sprites as kill bonus

	// Reset common ghost properties
	for (u8 i = 0; i < GHOST_COUNT; i++) {
		ghosts[i].state = 0;
		ghosts[i].prevDir.x = 0;
		ghosts[i].prevDir.y = 0;
		HideSprite(ghosts[i].sprite, ghosts[i].anim->wid, ghosts[i].anim->hgt);

		if (i != BLINKY) {
			SB(ghosts[i].state, NSTATE_HOUSE_ARREST);
		}
	}

	// Init SFX
	sfxSiren = SFX_SIREN_SLOW;
	sfxState = SFXSTATE_SIREN;
	extraLifeCount = 0;

	if (reset) {
		// Draw board and re-load meta data
		DrawMap2(SCREEN_LT, MAP_OFFSET_Y>>3, mapBoard);
		memcpy_P(gameMap, gameMapPgm, GAME_MAP_WID * GAME_MAP_HGT);
	}

	// Draw board accessories
	PacmanPrint(SCORE_X - 3, SCORE_Y - 1, p1Score, FONT_WHITE);
	PacmanPrint(HSCORE_X - 7, HSCORE_Y - 1, hs, FONT_WHITE);
	DrawScore(HSCORE_X, HSCORE_Y, highscore);
	DrawScore(SCORE_X, SCORE_Y, score);
	DrawLives();
	SetFruit();
	DrawFruitLog();

	// Draw map labels
	if (QB(pacman.state, PSTATE_AI)) {
		DrawMap2((MAP_OFFSET_X>>3) + GT_GAMEOVER_X, (MAP_OFFSET_Y>>3) + GT_GAMEOVER_Y, mapGameOver);
	} else {
		// Player One
		if (reset && (level == 1)) {
			DrawMap2((MAP_OFFSET_X>>3) + GT_P1_X, (MAP_OFFSET_Y>>3) + GT_P1_Y, mapPlayerOne);
		}
		// Ready!
		DrawMap2((MAP_OFFSET_X>>3) + GT_RDY_X, (MAP_OFFSET_Y>>3) + GT_RDY_Y, mapReady);
	}

	// Init pacman
	SetLoc(PAC_START_LOC_X>>3, PAC_START_LOC_Y>>3, pacman.loc);
	SetLoc(MAP_OFFSET_X + PAC_START_LOC_X + TILE_WID2, MAP_OFFSET_Y + PAC_START_LOC_Y + TILE_HGT2, pacman.sprLoc);
	SetDir(DIR_LEFT, 0, pacman.dir);
	SetDir(DIR_INVALID, 0, pacman.qdir);
	SetDir(0, 0, pacman.prevDir);
	pacman.sprite = PACMAN;
	SetPacmanSpeed();
	SB(pacman.state, PSTATE_MOVING);
	LoadPacmanAnimations(ANIM_LT);
	pacman.anim->currFrame = 0;
	DrawPacman();
	MoveSprite(pacman.sprite, pacman.sprLoc.x - TILE_WID2, pacman.sprLoc.y - TILE_HGT2, pacman.anim->wid, pacman.anim->hgt);

	// Init ghosts - blinky
	SetLoc(BLINKY_HOUSE_LOC_X, BLINKY_HOUSE_LOC_Y - 2, ghosts[BLINKY].loc);
	SetLoc(BLINKY_SCATTER_LOC_X, BLINKY_SCATTER_LOC_Y, ghosts[BLINKY].sloc);
	SetLoc(MAP_OFFSET_X + BLINKY_HOUSE_LOC_X * TILE_WID + TILE_WID2 - 2, MAP_OFFSET_Y + (BLINKY_HOUSE_LOC_Y - 2)*TILE_HGT + TILE_HGT2, ghosts[BLINKY].sprLoc);
	SetLoc(BLINKY_HOUSE_LOC_X, BLINKY_HOUSE_LOC_Y, ghosts[BLINKY].hloc);
	SetDir(DIR_LEFT, 0, ghosts[BLINKY].dir);
	ghosts[BLINKY].sprite = BLINKY + GSPROFF;
	LoadGhostAnimations(ANIM_LT, BLINKY);
	SB(ghosts[BLINKY].state, NSTATE_MOVING);
	SetGhostSpeed(BLINKY);
	ghosts[BLINKY].inHouse = false;

	// pinky
	SetLoc(PINKY_HOUSE_LOC_X, PINKY_HOUSE_LOC_Y, ghosts[PINKY].loc);
	SetLoc(PINKY_SCATTER_LOC_X, PINKY_SCATTER_LOC_Y, ghosts[PINKY].sloc);
	SetLoc(MAP_OFFSET_X + PINKY_HOUSE_LOC_X*TILE_WID + TILE_WID2, MAP_OFFSET_Y + PINKY_HOUSE_LOC_Y*TILE_HGT + TILE_HGT2, ghosts[PINKY].sprLoc);
	SetLoc(PINKY_HOUSE_LOC_X, PINKY_HOUSE_LOC_Y, ghosts[PINKY].hloc);
	SetDir(0, DIR_DOWN, ghosts[PINKY].dir);
	ghosts[PINKY].sprite = PINKY + GSPROFF;
	LoadGhostAnimations(ANIM_DN, PINKY);
	SB(ghosts[PINKY].state, NSTATE_MOVING);
	SetGhostSpeed(PINKY);

	if (reset) {
		ghosts[PINKY].pelCount = 0;
		ghosts[PINKY].inHouse = false;
	}

	// inky
	SetLoc(INKY_HOUSE_LOC_X, INKY_HOUSE_LOC_Y, ghosts[INKY].loc);
	SetLoc(INKY_SCATTER_LOC_X, INKY_SCATTER_LOC_Y, ghosts[INKY].sloc);
	SetLoc(MAP_OFFSET_X + INKY_HOUSE_LOC_X*TILE_WID + TILE_WID2, MAP_OFFSET_Y + INKY_HOUSE_LOC_Y*TILE_HGT + TILE_HGT2 + 1, ghosts[INKY].sprLoc);
	SetLoc(INKY_HOUSE_LOC_X, INKY_HOUSE_LOC_Y, ghosts[INKY].hloc);
	SetDir(0, DIR_UP, ghosts[INKY].dir);
	ghosts[INKY].sprite = INKY + GSPROFF;
	LoadGhostAnimations(ANIM_UP, INKY);
	SB(ghosts[INKY].state, NSTATE_MOVING);
	SetGhostSpeed(INKY);

	if (reset) {
		ghosts[INKY].pelCount = (level == 1) ? 19 : 0;
		ghosts[INKY].inHouse = (level == 1) ? true : false;
	}

	// clyde
	SetLoc(CLYDE_HOUSE_LOC_X, CLYDE_HOUSE_LOC_Y, ghosts[CLYDE].loc);
	SetLoc(CLYDE_SCATTER_LOC_X, CLYDE_SCATTER_LOC_Y, ghosts[CLYDE].sloc);
	SetLoc(MAP_OFFSET_X + CLYDE_HOUSE_LOC_X*TILE_WID + TILE_WID2, MAP_OFFSET_Y + CLYDE_HOUSE_LOC_X*TILE_HGT + TILE_HGT2 - 1, ghosts[CLYDE].sprLoc);
	SetLoc(CLYDE_HOUSE_LOC_X, CLYDE_HOUSE_LOC_Y, ghosts[CLYDE].hloc);
	SetDir(0, DIR_UP, ghosts[CLYDE].dir);
	ghosts[CLYDE].sprite = CLYDE + GSPROFF;
	LoadGhostAnimations(ANIM_UP, CLYDE);
	SB(ghosts[CLYDE].state, NSTATE_MOVING);
	SetGhostSpeed(CLYDE);

	if (reset) {
		ghosts[CLYDE].pelCount = (level == 1) ? 39 : ((level == 2) ? 32 : 0);
		ghosts[CLYDE].inHouse = (level < 3) ? true : false;
	}
}


// Acts as a central region to act on ghost logic determined elsewhere
void ApplyGhostState(u8 g) {
	// Enforce tunnel speed restrictions
	if (!QB(ghosts[g].state, NSTATE_DEAD)) {
		if (Square(ghosts[g].sprLoc.x, ghosts[g].sprLoc.y)) {
			if ((ghosts[g].loc.x == GHOST_SLOW_AX) && (ghosts[g].loc.y == GHOST_SLOW_AY) && (ghosts[g].dir.x == -1)) {
				// Going into west tunnel
				ghosts[g].spd = &ghosts[g].tspd;
			} else if ((ghosts[g].loc.x == (GHOST_SLOW_AX + 1)) && (ghosts[g].loc.y == GHOST_SLOW_AY) && (ghosts[g].dir.x == 1)) {
				// Coming out of west tunnel
				if (QB(ghosts[g].state, NSTATE_FRIGHTENED | NSTATE_FLASH)) {
					ghosts[g].spd = &ghosts[g].fspd;
				} else {
					ghosts[g].spd = &ghosts[g].nspd;
				}
			} else if ((ghosts[g].loc.x == GHOST_SLOW_BX) && (ghosts[g].loc.y == GHOST_SLOW_BY) && (ghosts[g].dir.x == 1)) {
				// Going into east tunnel
				ghosts[g].spd = &ghosts[g].tspd;
			} else if ((ghosts[g].loc.x == (GHOST_SLOW_BX - 1)) && (ghosts[g].loc.y == GHOST_SLOW_BY) && (ghosts[g].dir.x == -1)) {
				// Coming out of east tunnel
				if (QB(ghosts[g].state, NSTATE_FRIGHTENED | NSTATE_FLASH)) {
					ghosts[g].spd = &ghosts[g].fspd;
				} else {
					ghosts[g].spd = &ghosts[g].nspd;
				}
			}
		}
	}

	// Prevent chance to miss collision when pacman has advantage (otherwise it is frustrating)
	if (QB(ghosts[g].state, NSTATE_FRIGHTENED | NSTATE_FLASH)) {
		CheckGhostCollision();
	}

	if (QB(ghosts[g].state, NSTATE_EMPTY_NEST)) {
		CB(ghosts[g].state, NSTATE_EMPTY_NEST);

		if (g == CLYDE) {
			// If Blinky was in Elroy state, ensure he retains it after clyde exits house
			if (QBC(elroy, NSTATE_ELROY1_HOLD)) {
				SB(elroy, NSTATE_ELROY);
			} else if (QBC(elroy, NSTATE_ELROY2_HOLD)) {
				SB(elroy, NSTATE_ELROY1);
				SB(elroy, NSTATE_ELROY);
			}
		}
	}

	if (QB(ghosts[g].state, NSTATE_FLASH)) {
		if (ghosts[g].flashCount == 0) {
			CB(ghosts[g].state, NSTATE_FLASH);
			LoadGhostAnimations(GetAnimDir(ghosts[g].dir), g);

			// Don't override tunnel speed
			if (ghosts[g].spd != &ghosts[g].tspd) {
				ghosts[g].spd = &ghosts[g].nspd;
			}

			DrawGhost(g);
		}
	}

	// Blinky can go into Cruise Elroy mode
	if ((g == BLINKY) && QBC(elroy, NSTATE_ELROY)) {
		if (QB(elroy, NSTATE_ELROY1)) {
			CB(elroy, NSTATE_ELROY1);
			SB(elroy, NSTATE_ELROY2);
			SetGhostSpeed(BLINKY);
			sfxSiren = SFX_SIREN_FAST;
		} else {
			SB(elroy, NSTATE_ELROY1);
			SetGhostSpeed(BLINKY);
			sfxSiren = SFX_SIREN_MED;
		}
	}
}


// Allows player to break out of intro/title/demo screens
void CheckPlayerActivity(void) {
	static bool coinInserted = false, startPressed = false;

	if (gs.intro || QB(pacman.state, PSTATE_AI)) {
		if (playerInput & BTN_A) {
			if (!coinInserted) {
				coinInserted = true;
				credit = (credit == 255) ? credit : credit + 1;
				PacmanPrint(SCORE_X - 8, SCORE_Y + 24, creditText, FONT_WHITE);
				PacmanPrintByte(SCORE_X + 1, SCORE_Y + 24, credit);

				if (QB(gs.general, GSTATE_BONUS_PAUSE)) {
					bonusTmr = 1;
				}

				CB(gs.general, GSTATE_NORMAL | GSTATE_LVL_COMPLETE | GSTATE_NEW_LEVEL | GSTATE_LVL_COMMENCED);
				pacman.state = 0;
				gs.intro = GSTATE_TITLE_SCREEN;

				if (!QB(scrnState, SSTATE_INTRO | SSTATE_TITLE)) {
					SetTileTable(pacfontTileset);
					fontOffset = 0;
				}

				if (!QB(scrnState, SSTATE_TITLE)) {
					DisplayTitleScreen(RESET_ALL);
				}

				DisplayTitleScreen(RESET_TMRS);
			}
		} else {
			coinInserted = false;
		}

		if (playerInput & BTN_START) {
			if (!startPressed) {
				startPressed = true;

				if (credit && !QB(pacman.state, PSTATE_AI)) {
					--credit;
					gs.intro = GSTATE_INTRO_END;
					CB(pacman.state, PSTATE_AI);

					if (QB(gs.general, GSTATE_BONUS_PAUSE)) {
						bonusTmr = 1;
					}
				} else {
					if (QB(pacman.state, PSTATE_AI)) {
						if (QB(gs.general, GSTATE_BONUS_PAUSE)) {
							bonusTmr = 1;
						}

						CB(gs.general, GSTATE_NORMAL | GSTATE_LVL_COMPLETE | GSTATE_NEW_LEVEL | GSTATE_LVL_COMMENCED);
						pacman.state = 0;
						SetTileTable(pacfontTileset);
						fontOffset = 0;
						gs.intro = GSTATE_TITLE_SCREEN;
						DisplayTitleScreen(RESET_ALL);
					} else {
						// Flash insert coin
						coinTmr = 2*HZ;
					}
				}
			}
		} else {
			startPressed = false;
		}

		if (coinTmr) {
			switch (coinTmr) {
				case 1:
					PacmanPrint(SCORE_X - 8, SCORE_Y + 24, creditText, FONT_WHITE);
					PacmanPrintByte(SCORE_X + 1, SCORE_Y + 24, credit);
					break;
				case (HZ+HZ/2):
				case (HZ/2):
					for (u8 i = 0; i < 11; i++) {
						SetTile(SCORE_X - 8 + i, SCORE_Y + 24, 0);
					}
					break;
				case (HZ):
				case (2*HZ):
					PacmanPrint(SCORE_X - 8, SCORE_Y + 24, insertCoin, FONT_WHITE);
					break;
			}

			--coinTmr;
		}
	}
}


// Acts as a central region to act on game logic determined elsewhere
void ApplyGameState(void) {
	static u8 gFrightCount;	// Counter for frightened ghosts. Fright state can exit early if all ghosts are eaten

	// Bonus pause - pause upon kill to display sprite
	if (QBC(gs.general, GSTATE_GHOST_EATEN)) {
		if (!gs.intro) {
			SB(sfxState, (SFXSTATE_DEAD_BLINKY<<freshCorpse));
			TriggerFx(SFX_KILL_GHOST, SFX_VOL_KILL_GHOST, true);
		}

		gFrightCount = (gFrightCount) ? gFrightCount - 1: gFrightCount;
		SB(gs.general, GSTATE_BONUS_PAUSE);
		CB(gs.general, GSTATE_NORMAL);
		CB(ghosts[freshCorpse].state, NSTATE_FRIGHTENED);
		SB(ghosts[freshCorpse].state, NSTATE_EATEN);

		if (!gs.intro) {
			ghosts[freshCorpse].spd = &ghosts[freshCorpse].nspd;
			ghosts[freshCorpse].flashCount = 0;
			AddScore(killBonus);
		} else {
			CB(ghosts[freshCorpse].state, NSTATE_MOVING);
		}

		// Set time to display bonus
		bonusTmr = 1*HZ;
		// Hide pacman and his victim
		HideSprite(ghosts[freshCorpse].sprite, ghosts[freshCorpse].anim->wid, ghosts[freshCorpse].anim->hgt);
		HideSprite(pacman.sprite, pacman.anim->wid, pacman.anim->hgt);
		// Display bonus
		MapSprite(KILL_BONUS_SPRITE, killBonusMap);
		MoveSprite(KILL_BONUS_SPRITE, pacman.sprLoc.x - TILE_WID, pacman.sprLoc.y - TILE_HGT2, pgm_read_byte(&killBonusMap[0]),
				pgm_read_byte(&killBonusMap[1]));
	}

	if (QB(gs.general, GSTATE_BONUS_PAUSE)) {
		if (bonusTmr && (--bonusTmr == 0)) {
			CB(gs.general, GSTATE_BONUS_PAUSE);
			CB(ghosts[freshCorpse].state, NSTATE_EATEN);
			SB(ghosts[freshCorpse].state, NSTATE_DEAD);
			HideSprite(KILL_BONUS_SPRITE, pgm_read_byte(&killBonusMap[0]), pgm_read_byte(&killBonusMap[1]));
			UpdateKillBonus();

			if (!gs.intro) {
				ghosts[freshCorpse].spd = &ghosts[freshCorpse].nspd;
				LoadGhostAnimations(GetAnimDir(ghosts[freshCorpse].dir), freshCorpse);
				DrawGhost(freshCorpse);

				if (!CheckGhostCollision()) {
					SB(gs.general, GSTATE_NORMAL);
					MoveSprite(pacman.sprite, pacman.sprLoc.x - TILE_WID2, pacman.sprLoc.y - TILE_HGT2, pacman.anim->wid, pacman.anim->hgt);
				} else {
					return;
				}
			}
		} else {
			if (MOD2N(bonusTmr, 16) == 0) {
				FlashEnergizers();

				if (!gs.intro && !QB(pacman.state, PSTATE_AI)) {
					if (MOD2N(bonusTmr, 32) == 0) {
						Flash1Up();
					}
				}
			}

			CheckPlayerActivity();
			return;
		}
	}

	// Stay responsive
	if (gs.intro || QB(pacman.state, PSTATE_AI)) {
		CheckPlayerActivity();
	}

	// Intro screens
	if (gs.intro) {
		SetTileTable(pacfontTileset);
		fontOffset = 0;

		if (QBC(gs.intro, GSTATE_INIT)) {
			SB(gs.intro, GSTATE_GHOST_INTRO);
			elroy = 0;
			pellets = MAX_PELLETS;
			DisplayIntroScreen(RESET_ALL);
		} else if (QB(gs.intro, GSTATE_GHOST_INTRO)) {
			if (!DisplayIntroScreen(RESET_NONE)) {
				CB(gs.intro, GSTATE_GHOST_INTRO);
				CB(pacman.state, PSTATE_MOVING);
				SB(gs.intro, GSTATE_TITLE_SCREEN);
				DisplayTitleScreen(RESET_ALL);
				return;
			}
		} else if (QB(gs.intro, GSTATE_TITLE_SCREEN)) {
			if (!DisplayTitleScreen(RESET_NONE)) {
				CB(gs.intro, GSTATE_TITLE_SCREEN);
				coinTmr = 0;
				elroy = 0;
				SB(gs.intro, GSTATE_INTRO_END);
				SB(pacman.state, PSTATE_AI);
			}
		} else if (QBC(gs.intro, GSTATE_INTRO_END)) {
			SetTileTable(pacmanTileset);
			fontOffset = PACMAN_TILESET_SIZE;
			SB(gs.general, GSTATE_NEW_LEVEL);
			frameCount += pacmanAiOffset++;	// Vary demo AI
			gs.intro = 0;
			level = 1;
			score = 0;
			scoreMod = 0;
			lives = (QB(pacman.state, PSTATE_AI)) ? 1 : 3;
		}
	}

	// Level init/complete
	if (QB(gs.general, GSTATE_LVL_COMPLETE)) {
		if (!lvlTmr) {
			CB(gs.general, GSTATE_NORMAL);
			gs.fright = 0;
			gFrightCount = 0;
			sfxState = 0;
			elroy = 0;
		 	lvlTmr = LVL_COMPLETE_PAUSE;
			// Pacman completes level with mouth closed
			pacman.anim-> currFrame = 0;
			DrawPacman();
		} else if (--lvlTmr == (LVL_COMPLETE_FLASH + 1)) {
			for (u8 i = 0; i < GHOST_COUNT; i++) {
				ghosts[i].state = 0;
				HideSprite(ghosts[i].sprite, ghosts[i].anim->wid, ghosts[i].anim->hgt);
			}
		} else if (lvlTmr == 0) {
			gs.general = GSTATE_INTERMISSION;
			HideSprite(0, MAX_SPRITES, 1);
			PlayIntermission();
			gs.general = GSTATE_NEW_LEVEL;
			++lives;
			++level;
		} else if (lvlTmr <= LVL_COMPLETE_FLASH) {
			if (lvlTmr % (LVL_COMPLETE_FLASH / 4) == 0) {
				DrawMap2(MAP_OFFSET_X>>3, MAP_OFFSET_Y>>3, mapLvlCompleteG);
			} else if (lvlTmr % (LVL_COMPLETE_FLASH / 8) == 0) {
				DrawMap2(MAP_OFFSET_X>>3, MAP_OFFSET_Y>>3, mapLvlCompleteB);
			}
		}

		if (lvlTmr && (MOD2N(lvlTmr, 32) == 0)) {
			Flash1Up();
		}
		return;
	} else if (QBC(gs.general, GSTATE_NEW_LEVEL)) {
		SB(gs.general, GSTATE_LVL_COMMENCED);

		for (u8 i = 0; i < GHOST_COUNT; i++) {
			SB(ghosts[i].state, NSTATE_MOVING);
		}
		lvlTmr = (level == 1) ? INTRO_PAUSE : (INTRO_PAUSE>>1) + 1;
		InitLevel(true);

		if (!QB(pacman.state, PSTATE_AI) && (level == 1)) {
			StartSong(song_newgame);
		}
		return;
	} else if (QB(gs.general, GSTATE_LVL_COMMENCED)) {
#if INTERMISSION_LOOP
		SB(gs.general, GSTATE_LVL_COMPLETE);
#endif
		if (--lvlTmr == (INTRO_PAUSE>>1)) {

			if (!QB(pacman.state, PSTATE_AI)) {
				// Remove "Player One" text
				if (level == 1) {
					for (u8 i = ((MAP_OFFSET_X>>3) + GT_P1_X); i < ((MAP_OFFSET_X>>3) + (GT_P1_X + GT_P1_LEN)); i++) {
						SetTile(i, (MAP_OFFSET_Y>>3) + GT_P1_Y, 0);
					}
				}
			}

			// Draw Ghosts
			DrawGhost(BLINKY);
			MoveSprite(ghosts[BLINKY].sprite, ghosts[BLINKY].sprLoc.x - TILE_WID2, ghosts[BLINKY].sprLoc.y - TILE_HGT2,
				ghosts[BLINKY].anim->wid, ghosts[BLINKY].anim->hgt);
			DrawGhost(PINKY);
			MoveSprite(ghosts[PINKY].sprite, ghosts[PINKY].sprLoc.x - TILE_WID2, ghosts[PINKY].sprLoc.y - TILE_HGT2,
				ghosts[PINKY].anim->wid, ghosts[PINKY].anim->hgt);
			DrawGhost(INKY);
			MoveSprite(ghosts[INKY].sprite, ghosts[INKY].sprLoc.x - TILE_WID2, ghosts[INKY].sprLoc.y - TILE_HGT2,
				ghosts[INKY].anim->wid, ghosts[INKY].anim->hgt);
			DrawGhost(CLYDE);
			MoveSprite(ghosts[CLYDE].sprite, ghosts[CLYDE].sprLoc.x - TILE_WID2, ghosts[CLYDE].sprLoc.y - TILE_HGT2,
				ghosts[CLYDE].anim->wid, ghosts[CLYDE].anim->hgt);
		} else if (lvlTmr == 0) {
			CB(gs.general, GSTATE_LVL_COMMENCED);
			SB(gs.general, GSTATE_NORMAL);
			gs.fright = 0;
			gs.pursuit = GSTATE_SCATTER;	// Skip direction swap by going straight to scatter
			gs.fruit = 0;
			--lives;
			DrawLives();

			if (!QB(pacman.state, PSTATE_AI)) {
				// Remove "Ready" text
				for (u8 i = ((MAP_OFFSET_X>>3) + GT_RDY_X); i < ((MAP_OFFSET_X>>3) + (GT_RDY_X + GT_RDY_LEN)); i++) {
					SetTile(i, (MAP_OFFSET_Y>>3) + GT_RDY_Y, 0);
				}
			}
		}

		if (MOD2N(lvlTmr, 32) == 0) {
			if (!QB(pacman.state, PSTATE_AI)) {
				Flash1Up();
			}
		}

		return;
	}

	// Pacman
	if (QBC(pacman.state, PSTATE_DYING)) {	// Intermediate state PSTATE_DYING ensures we process death before any other code does
		// Ensure frames are progressing as pacman_die animation signals when to continue game
		SB(gs.general, GSTATE_NORMAL);
		// Pacman could be hidden from eating a ghost on the same square
		MoveSprite(pacman.sprite, pacman.sprLoc.x - TILE_WID2, pacman.sprLoc.y - TILE_HGT2, pacman.anim->wid, pacman.anim->hgt);
		SB(pacman.state, PSTATE_DEAD);
		CB(pacman.state, PSTATE_MOVING);

		for (u8 i = 0; i < GHOST_COUNT; i++) {
			HideSprite(ghosts[i].sprite, ghosts[i].anim->wid, ghosts[i].anim->hgt);
			CB(ghosts[i].state, NSTATE_MOVING);
			ghosts[i].inHouse = true;
		}

		LoadPacmanAnimations(ANIM_PACMAN_DIE);
		pacman.anim->currFrame = 0;
		DrawPacman();
		SB(sfxState, SFXSTATE_PACMAN_DIE);
		TriggerFx(SFX_PACMAN_DIE, SFX_VOL_PACMAN_DIE, true);
		return;
	} else if (QB(pacman.state, PSTATE_DEAD)) {
		return;
	} else if (QBC(pacman.state, PSTATE_REVIVING)) {
		// Ensure fruit is not displaying
		SetTile(fruit.loc.x, fruit.loc.y, 0);
		HideSprite(FRUIT_BONUS_SPRITE, pgm_read_byte(&fruit.bonusMap[0]), pgm_read_byte(&fruit.bonusMap[1]));
		gs.fruit = 0;
		gs.fright = 0;

		if (lives) {
			CB(gs.general, GSTATE_NORMAL);
			SB(gs.general, GSTATE_LVL_COMMENCED);
			CB(sfxState, SFXSTATE_FRIGHTENED | SFXSTATE_DEAD_GHOST | SFXSTATE_EXTRA_LIFE | SFXSTATE_PACMAN_DIE);
			sfxSiren = SFX_SIREN_SLOW;

			// If Blinky was in Elroy state, ensure he retains it after clyde exits house
			if (QBC(elroy, NSTATE_ELROY1)) {
				SB(elroy, NSTATE_ELROY1_HOLD);
			} else if (QBC(elroy, NSTATE_ELROY2)) {
				SB(elroy, NSTATE_ELROY2_HOLD);
			}

			pelCount = 1;		// Enable global pellet counter
			pelIndex = PINKY;	// Pinky is first in line for release after pacman's death
			InitLevel(false);
			lvlTmr = (INTRO_PAUSE>>1) + 1;
		} else {
			SB(pacman.state, PSTATE_GAME_OVER);
			CB(gs.general, GSTATE_NORMAL);
			SB(pacman.state, PSTATE_GAME_OVER);

			if (score == highscore) {
				SaveHighScore();
			}

			gameOverTmr = (QB(pacman.state, PSTATE_AI)) ? 1 : 3*HZ;
			DrawMap2((MAP_OFFSET_X>>3) + 6, (MAP_OFFSET_Y>>3) + 12, mapGameOver);
		}
	} else if (QB(pacman.state, PSTATE_GAME_OVER)) {
		if (gameOverTmr && (--gameOverTmr == 0)) {
			CB(pacman.state, PSTATE_AI);
			SB(gs.intro, GSTATE_INIT);
		}
	}

	// Frightened
	if (QBC(gs.fright, GSTATE_FRIGHTENED_INIT)) {
		if (!gs.intro) {
			AddScore(POINTS_ENERGIZER);
			SetFrightenedTimer();

			if (frightTmr) {
				gs.fright = GSTATE_FRIGHTENED;
				SB(sfxState, SFXSTATE_FRIGHTENED);

				for (u8 i = 0; i < GHOST_COUNT; i++) {
					gFrightCount = (!QB(ghosts[i].state, NSTATE_DEAD)) ? gFrightCount + 1 : gFrightCount;
				}
			}
		}

		pacman.spd = &pacman.fspd;
		killBonus = 200;
		killBonusMap = map200;


		for (u8 i = 0; i < GHOST_COUNT; i++) {
			if (!QB(ghosts[i].state, NSTATE_DEAD | NSTATE_HOUSE_ARREST | NSTATE_REVIVED)) {
				// Reverse direction
				ghosts[i].dir.x *= -1;
				ghosts[i].dir.y *= -1;
			}

			if (gs.intro || frightTmr) {
				if (!QB(ghosts[i].state, NSTATE_DEAD)) {
					CB(ghosts[i].state, NSTATE_FLASH);
					SB(ghosts[i].state, NSTATE_FRIGHTENED);
					LoadGhostAnimations(ANIM_GHOST_FLEE, i);
					DrawGhost(i);

					// Don't override tunnel speed
					if (ghosts[i].spd != &ghosts[i].tspd) {
						ghosts[i].spd = &ghosts[i].fspd;
					}
				}
			}
		}
	} else if ((gs.fright == GSTATE_FRIGHTENED) || (gs.fright == GSTATE_FLASH)) {
		if (gFrightCount && (MOD2N(frameCount, SFX_DUR_FRIGHTENED) == 0)) {
			if (sfxState < SFXSTATE_DEAD_BLINKY) {
				TriggerFx(SFX_FRIGHTENED, SFX_VOL_FRIGHTENED, true);
			}
		}

		if (gs.fright == GSTATE_FRIGHTENED) {
			if (frightTmr && (--frightTmr == 0)) {
				gs.fright = GSTATE_FLASH;

				u8 temp = GetFlashCount();

				for (u8 i = 0; i < GHOST_COUNT; i++) {
					if (QB(ghosts[i].state, NSTATE_FRIGHTENED)) {
						CB(ghosts[i].state, NSTATE_FRIGHTENED);
						SB(ghosts[i].state, NSTATE_FLASH);
						ghosts[i].flashCount = temp;
						LoadGhostAnimations(ANIM_GHOST_FLASH, i);
						DrawGhost(i);
					}
				}
			}
		} else if (gs.fright == GSTATE_FLASH) {
			if (!(ghosts[BLINKY].flashCount || ghosts[PINKY].flashCount || ghosts[INKY].flashCount || ghosts[CLYDE].flashCount)) {
				CB(sfxState, SFXSTATE_FRIGHTENED);
				gFrightCount = 0;
				gs.fright = 0;
				pacman.spd = &pacman.nspd;

				for (u8 i = 0; i < GHOST_COUNT; i++) {
					if (QB(ghosts[i].state, NSTATE_FLASH)) {
						CB(ghosts[i].state, NSTATE_FLASH);
						ghosts[i].spd = &ghosts[i].nspd;
						LoadGhostAnimations(GetAnimDir(ghosts[i].dir), i);
					}
				}
			}
		}
	} else {
		++lvlTmr;	// We don't count frightened time as part of normal level time
	}

	// Pursuit - game-level ghost AI state
	if ((gs.pursuit == GSTATE_SCATTER_INIT) || (gs.pursuit == GSTATE_CHASE_INIT)) {
		if (gs.pursuit == GSTATE_SCATTER_INIT) {
			gs.pursuit = GSTATE_SCATTER;
		} else {
			gs.pursuit = GSTATE_CHASE;
		}

		for (u8 i = 0; i < GHOST_COUNT; i++) {
			if (!QB(ghosts[i].state, NSTATE_DEAD | NSTATE_REVIVED | NSTATE_HOUSE_ARREST)) {
				// Reverse direction
				ghosts[i].dir.x *= -1;
				ghosts[i].dir.y *= -1;
			}
		}
	}

	// Fruit
	if (gs.fruit == GSTATE_FRUIT_INIT) {
		gs.fruit = GSTATE_FRUIT;
		SetTile(fruit.loc.x, fruit.loc.y, fruit.tile + fontOffset);
		fruit.activeTmr = 10 * HZ;
	} else if (gs.fruit == GSTATE_FRUIT) {
		if (fruit.activeTmr && (--fruit.activeTmr == 0)) {
			gs.fruit = 0;

			if (QB(pacman.state, PSTATE_AI)) {
				DrawMap2((MAP_OFFSET_X>>3) + 6, (MAP_OFFSET_Y>>3) + 12, mapGameOver);
			} else {
				SetTile(fruit.loc.x, fruit.loc.y, 0);
			}
		}
	} else if (gs.fruit == GSTATE_FRUIT_BONUS_INIT) {
		TriggerFx(SFX_FRUIT, SFX_VOL_FRUIT, true);
		gs.fruit = GSTATE_FRUIT_BONUS;
		AddScore(fruit.bonus);

		if (QB(pacman.state, PSTATE_AI)) {
			DrawMap2((MAP_OFFSET_X>>3) + 6, (MAP_OFFSET_Y>>3) + 12, mapGameOver);
		} else {
			SetTile(fruit.loc.x, fruit.loc.y, 0);
		}

		MapSprite(FRUIT_BONUS_SPRITE, fruit.bonusMap);
		MoveSprite(FRUIT_BONUS_SPRITE, fruit.loc.x * TILE_WID - (TILE_WID>>1),
				(fruit.loc.y) * TILE_HGT, pgm_read_byte(&fruit.bonusMap[0]), pgm_read_byte(&fruit.bonusMap[1]));
		fruit.bonusTmr = 4 * HZ;
	} else if (gs.fruit == GSTATE_FRUIT_BONUS) {
		if (fruit.bonusTmr && (--fruit.bonusTmr == 0)) {
			gs.fruit = 0;
			HideSprite(FRUIT_BONUS_SPRITE, pgm_read_byte(&fruit.bonusMap[0]), pgm_read_byte(&fruit.bonusMap[1]));
		}
	}
}


int main() {
	u8 i;

	// Initialise sound resources
	InitMusicPlayer(patches);
	SetMasterVolume(0xb0);

	// Initialise gfx resources
	SetTileTable(pacmanTileset);
	SetSpritesTileTable(pacmanSpriteset);
	SetSpriteVisibility(true);
	fontOffset = PACMAN_TILESET_SIZE;

	// Seed game state
	SB(gs.intro, GSTATE_INIT);
	LoadHighScore();
	scrnState = SSTATE_INTRO;

	// Init sprites
	pacman.sprite = PACMAN;
	ghosts[BLINKY].sprite = BLINKY + GSPROFF;
	ghosts[PINKY].sprite = PINKY + GSPROFF;
	ghosts[INKY].sprite = INKY + GSPROFF;
	ghosts[CLYDE].sprite = CLYDE + GSPROFF;

	// Some players might find the constant siren annoying
	sirenToggle = true;

	while(1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();
			ApplyGameState();
			playerInput = GetInput();

			// Keep dead ghosts moving through pauses
			if (!QB(gs.general, GSTATE_NORMAL)) {
				if (!gs.intro && QB(gs.general, GSTATE_BONUS_PAUSE)) {
					for (i = 0; i < GHOST_COUNT; i++) {
						if (QB(ghosts[i].state, NSTATE_DEAD)) {
							UpdateSpeed(ghosts[i].spd);
							DoGhostAI(i);
							MoveGhost(i);
							AnimateGhost(i);
							ApplyGhostState(i);
						}
					}
				}

				continue;
			}

			// Do regular jobs
			if (QB(sfxState, SFXSTATE_DEAD_GHOST) && (sfxState < SFXSTATE_EXTRA_LIFE)) {
				if (MOD2N(frameCount, SFX_DUR_DEAD_GHOST) == 0) {
					TriggerFx(SFX_DEAD_GHOST, SFX_VOL_DEAD_GHOST, true);
				}
			}

			if (MOD2N(frameCount, 16) == 0) {
				FlashEnergizers();

				if (MOD2N(frameCount, 32) == 0) {
					if (!QB(pacman.state, PSTATE_AI)) {
						Flash1Up();

						if (sirenToggle && (sfxState == SFXSTATE_SIREN)) {
							TriggerFx(sfxSiren, SFX_VOL_SIREN, true);
						}
					}
				}
			}

			// Ensure ghosts leave house if pacman stops eating
			if (pelTmr && (--pelTmr == 0)) {
				pelTmr = (level < 5) ? 4*HZ : 3*HZ;
				ghosts[pelIndex].inHouse = false;
				PassPelletCounter();
			}

			ProcessInput();

			// Update pacman
			if (QB(pacman.state, PSTATE_MOVING)) {
				UpdateSpeed(pacman.spd);
			}

			MovePacman();
			AnimatePacman();

			// Update ghosts
			for (i = 0; i < GHOST_COUNT; i++) {
				UpdateSpeed(ghosts[i].spd);
				DoGhostAI(i);
				MoveGhost(i);
				ApplyGhostState(i);
				AnimateGhost(i);
			}

			// Update ghost pursuit AI
			UpdatePursuitMode();

			// Play extra life ding
			if (extraLifeCount) {
				if (!QB(sfxState, SFXSTATE_PACMAN_DIE) && (MOD2N(--extraLifeCount, 16) == 0)) {
					TriggerFx(SFX_EXTRA_LIFE, SFX_VOL_EXTRA_LIFE, true);

					if (extraLifeCount == 0) {
						CB(sfxState, SFXSTATE_EXTRA_LIFE);
					}
				}
			}

			// Keep things moving
			++frameCount;
		}
	}
}




/****************************************
 *			Code Mortuary				*
 ****************************************/
#if 0

typedef struct actor {
	vec			loc;
	pt			sprLoc;
	vec			dir;
	vec			prevDir;
	u8			state;
	u8			sprite;
	u8			type;
	speed		*spd;
	animation	*anim;
} actor;

#define IS_PLAYER(_a)	((_a)->type == PLAYER)
#define IS_GHOST(_a)	((_a)->type == GHOST)

void MoveActor(void *v) {
	actor *a = (actor*)v;

	if (IS_PLAYER(a)) {
		if (QB(a.state, PSTATE_DEAD)) {
			a.anim->disp++;
			return;
		}
	}

	if (a->spd->mode == SPEED_MODE_MOVE) {
		if (QB(a->state, ASTATE_MOVING)) {
			a->sprLoc.x += a->dir.x;
			a->sprLoc.y += a->dir.y;

			// Tunnel teleport
			if (!QB(gs.general, GSTATE_INTERMISSION) && !gs.intro) {
				if ((a->dir.x == DIR_LEFT) && (a->sprLoc.x <= TUNNEL_LEFT)) {
					a->sprLoc.x = TUNNEL_RIGHT;
				} else if ((a->dir.x == DIR_RIGHT) && (a->sprLoc.x >= TUNNEL_RIGHT)) {
					a->sprLoc.x = TUNNEL_LEFT;
				}
			}

			a->anim->disp++;
			MoveSprite(a->sprite, a->sprLoc.x - TILE_WID2, a->sprLoc.y - TILE_HGT2, a->anim->wid, a->anim->hgt);

			// Update grid position when our center-point is entering a new 8x8 grid position
			if ((MOD2N(a->sprLoc.x - TILE_WID2, TILE_WID) == 0) && (MOD2N(a->sprLoc.y - TILE_HGT2, TILE_HGT) == 0)) {
				a->loc.x = (a->sprLoc.x - MAP_OFFSET_X)>>3;
				a->loc.y = (a->sprLoc.y - MAP_OFFSET_Y)>>3;
			}
		}
	}
}


void AnimateActor(void *v) {
	actor *a = (actor*)v;

	if (DirDiff(a->dir, a->prevDir)) {
		if (IS_PLAYER(a)) {
			if (animdir != ANIM_NONE) {
				LoadPacmanAnimations(animdir);
				DrawActor(a);
			}
		} else if (!(QB(ghosts[g].state, NSTATE_FRIGHTENED) || QB(ghosts[g].state, NSTATE_FLASH))) {
			u8 animdir = GetAnimDir(a->dir);

			if (animdir != ANIM_NONE) {
				LoadGhostAnimations(animdir, a->sprite);
				DrawActor(a);
			}
		}
	}

	if ((IS_PLAYER(a)) && (!QB(a->state, ASTATE_MOVING | ASTATE_DEAD)) && (a->anim->currFrame != 1)) {
		// Stationary pacman always has his mouth open
		a->anim->disp = 0;
		a->anim-> currFrame = 1;
		DrawActor(a);
	}

	if (a->anim->disp == a->anim->dpf) {
		a->anim->disp = 0;
		a->anim->currFrame += a->anim->size;

		if (a->anim->currFrame == a->anim->frameCount) {
			a->anim->currFrame = 0;

			if (IS_PLAYER(a)) {
				if (QBC(pacman.state, PSTATE_DEAD)) {
					SB(pacman.state, PSTATE_REVIVING);
					HideSprite(PACMAN, pacman.anim->wid, pacman.anim->hgt);
				}
			}

			if (IS_GHOST(a)) {
				if (ghosts[a->sprite].flashCount) {
					ghosts[a->sprite].flashCount -= 1;
				}
			}
		}
		DrawActor(a);
	}
	a->prevDir = a->dir;
}


void DrawActor(void *v) {
	actor *a = (actor*)v;

	u8 limit = a->anim->currFrame + a->anim->size, u8 j = (IS_PLAYER(a)) ? 0 : GSPROFF;

	for (u8 i = a->anim->currFrame; i < limit; i++, j++) {
		sprites[a->sprite + j].tileIndex = pgm_read_byte(&(a->anim->frames[i]));
	}
}

#endif
