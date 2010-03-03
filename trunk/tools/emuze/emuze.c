
/*
 *  Emuze - An Eeprom Management Tool for the Uzebox
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
#include "data/tiles.inc"
#include "data/fonts.inc"

/****************************************
 *			  	 Defines				*
 ****************************************/
#define GAMES_COUNT 18
#define EEPROM_BLOCK_COUNT 64
#define STATE_COUNT 5 // Count of navStates
#define MENU_OPTIONS_COUNT 6
#define MAX_MENU_WID 10
#define MAX_MENU_HGT (MENU_OPTIONS_COUNT+2)

#define OPTION_EDIT 0
#define OPTION_COPY 1
#define OPTION_PASTE 2
#define OPTION_RELOAD 3
#define OPTION_SAVE 4
#define OPTION_FORMAT 5

#define CLEAR_TILE 0x0
#define MENU_TOP_LEFT 0x2
#define MENU_TOP_RIGHT 0x3
#define MENU_HORIZ_TOP 0x5
#define MENU_HORIZ_BTM 0x6
#define MENU_BTM_LEFT 0x32
#define MENU_BTM_RIGHT 0x33
#define MENU_VERT_LEFT 0x4
#define MENU_VERT_RIGHT 0x31

#define CURSOR_HGREEN 0x35
#define CURSOR_HRED 0x36
#define CURSOR_VGREEN 0x7
#define CURSOR_VRED 0x34

#define HEX_DUMP_X 5
#define HEX_DUMP_Y 23

#define GAMES_PER_PAGE 7
#define PAGE_COUNT (EEPROM_BLOCK_COUNT / GAMES_PER_PAGE + EEPROM_BLOCK_COUNT % GAMES_PER_PAGE)
#define BLOCK_MENU_LOC_X 6
#define BLOCK_MENU_LOC_Y 2
#define BLOCK_MENU_CELL_WID (SCREEN_TILES_H - (BLOCK_MENU_LOC_X + 1))
#define BLOCK_MENU_CELL_HGT 2
#define GAME_ICON_HGT 2
#define GAME_ICON_WID 3
#define FRAME_SIZE 8 // Game icons' map element count

#define PAGE_NO_LOC_X 15
#define PAGE_NO_LOC_Y 24
#define PAGE_NO_WID 10
#define PAGE_NO_HGT 1

#define KERNEL_BLOCK_X 6
#define KERNEL_BLOCK_Y 2
#define KERNEL_BLOCK_SPACING_X 18
#define KERNEL_BLOCK_WID 30
#define KERNEL_BLOCK_HGT 25


// Utility macros
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define ABS(x) (((x) > 0) ? (x) : -(x))


/****************************************
 *			Type declarations			*
 ****************************************/

// Navigation states (determines how we interpret user input).
// Sync with STATE_COUNT def above if states added/removed.
typedef enum { 
	navBlock, navMenu, navHexDump, navHexCell, navKernel
} navStates;

typedef struct {
	u8 x;
	u8 y;
} pt;

typedef struct {
	u8 w;
	u8 h;
} size;

typedef struct {
	u8 min;
	u8 mid;
	u8 max;
} range;

typedef struct {
	navStates curr;
	navStates *prev;
	navStates history[STATE_COUNT+1];
	int indexes[STATE_COUNT]; // Menu positions for each state.
} emuzeState;

typedef struct {
	u8 tile;
	range r; // Index range
	pt loc;
} menuCursor;

typedef struct tileMenu tileMenu;

struct tileMenu {
	u8 isVisible;
	pt loc;
	size s;
	menuCursor cursor;
	u8 *buffer; // Currently only used by navMenu to re-paint underlying vram on close.
};

typedef struct {
	u8 frameCount;
	u8 frameSequenceIndex; // Position in frameSequences array to begin loading frame indexes.
	u16 frameDurationsIndex; // Position in frameDurations array to begin loading frame timers.
	const int *frames; // First map of animation. Maps MUST be stored contiguously in flash.
} animation;

typedef struct {
	u8 frame;
	u16 counter;
	u16 duration;
	animation anim;
} menuAnimation;

typedef struct {
	u16 id;
	const char *title;
	u16 animIndex;
} gameDetails;

/****************************************
 *			File-level variables		*
 ****************************************/
emuzeState state;
tileMenu blockMenu; // Current when browsing eeprom blocks.
tileMenu menu; // Current when choosing operation to perform on selected block.
tileMenu hexMenu; // Current when editing block's hex data.
struct EepromBlockStruct ebs;
struct EepromBlockStruct clipboard;
int page; // Current page number
u16 pageIds[GAMES_PER_PAGE]; // Game ID's for current page.
u16 gameIds[GAMES_COUNT]; // All game ID's (leave these in flash if ram gets tight).
menuAnimation menuAnim; // Selected block's animation.
u8 menuScrollTimer; // Regulates auto-scroll speed.
u8 menuBuffer[MAX_MENU_WID*MAX_MENU_HGT*VRAM_ADDR_SIZE]; // So we can repaint w/e the open menu overwrote.

const char freeBlockStr[] PROGMEM = "FREE BLOCK";
const char pageStr[] PROGMEM = "PAGE";
const char fwdSlashString[] PROGMEM = "/";

const char editStr[] PROGMEM = "EDIT";
const char copyStr[] PROGMEM = "COPY";
const char pasteStr[] PROGMEM = "PASTE";
const char reloadStr[] PROGMEM = "RE-LOAD";
const char saveStr[] PROGMEM = "SAVE";
const char formatStr[] PROGMEM = "FORMAT";

const char *menuStrings[] PROGMEM = {
	editStr,
	copyStr,
	pasteStr,
	reloadStr,
	saveStr,
	formatStr
};

// Kernel block strings
const char kbTitle[] PROGMEM = "--- KERNEL SETTINGS ---";
const char kbFeatures[] PROGMEM = "--- FEATURES ---";
const char kbNes[] PROGMEM = "NES";
const char kbSnes[] PROGMEM = "SNES";
const char kbJoyStickOther[] PROGMEM = "OTHER";
const char bitOff[] PROGMEM = "0";
const char bitOn[] PROGMEM = "1";

const char kbFeatures0[] PROGMEM = "JOYSTICK:";
const char kbFeatures1[] PROGMEM = "SOFT POWER SW:";
const char kbFeatures2[] PROGMEM = "STATUS LED:";
const char kbFeatures3[] PROGMEM = "SD CARD:";
const char kbFeatures4[] PROGMEM = "MIDI IN:";
const char kbFeatures5[] PROGMEM = "MIDI OUT:";
const char kbFeatures6[] PROGMEM = "ETHERNET:";
const char kbFeatures7[] PROGMEM = "PS2 K/B:";
const char kbFeatures8[] PROGMEM = "PS2 MOUSE:";
const char kbFeatures9[] PROGMEM = "AD725 P/C:";

const char kbSpecs[] PROGMEM = "--- SPECS ---";
const char kbSpecs0[] PROGMEM = "SIGNATURE: $";
const char kbSpecs1[] PROGMEM = "VERSION: $";
const char kbSpecs2[] PROGMEM = "BLOCK SIZE: $";
const char kbSpecs3[] PROGMEM = "HEADER SIZE: $";
const char kbSpecs4[] PROGMEM = "H/W VER.: $";
const char kbSpecs5[] PROGMEM = "H/W REV.: $";
const char kbSpecs6[] PROGMEM = "EXT FEATURES: $";
const char kbSpecs7[] PROGMEM = "MAC ADDR: $";
const char kbSpecs8[] PROGMEM = "COLOR CORRECTION: $";
const char kbSpecs9[] PROGMEM = "GAME CRC: $";
const char kbSpecs10[] PROGMEM = "RESERVED: 10 BYTES";

// Game title strings
const char kernelGame[] PROGMEM = "RESERVED";
const char megatrisGame[] PROGMEM = "MEGATRIS";
const char whackAMoleGame[] PROGMEM = "WHACK-A-MOLE";
const char voidFighterGame[] PROGMEM = "VOID FIGHTER";
const char pongGame[] PROGMEM = "PONG";
const char arkanoidGame[] PROGMEM = "ARKANOID";
const char drMarioGame[] PROGMEM = "DR. MARIO";
const char lodeRunnerGame[] PROGMEM = "LODE RUNNER";
const char unkownGame[] PROGMEM = "???";
const char spaceInvadersGame[] PROGMEM = "SPACE INVADERS";
const char pacmanGame[] PROGMEM = "PAC-MAN";
const char bcdashGame[] PROGMEM = "B.C. DASH";
const char sokobanWorldGame[] PROGMEM = "SOKOBAN WORLD";
const char advOfLoloGame[] PROGMEM = "ADVENTURES OF LOLO";
const char zombienatorGame[] PROGMEM = "ZOMBIENATOR";
const char corridaNebososaGame[] PROGMEM = "CORRIDA NEBOSOSA";
const char castlevaniaGame[] PROGMEM = "CASTLEVANIA: VENGEANCE";


// (#) denotes index to be referenced in animations array below. Note: trailing game title
// comments may not be complete or up-to-date.
u16 frameDurations[] PROGMEM = {
	65535,				// (0) All single-frame animations.
	60, 5,				// (1) Adventures of Lolo
	5, 5, 5, 5,			// (3) Unknown, Pac-Man
	5, 5, 5, 5,	5,		// (7) Pong
	10, 10, 10,			// (12) Megatris, Whack-A-Mole, Arkanoid, Space Invaders, Lode Runner, Kernel
	5, 5,				// (15) Zombienator
	3, 3, 3, 3,			// (17) B.C. Dash
	20, 20, 20,			// (21) Dr Mario
	7, 7				// (24) Sokoban World
};

u8 frameSequences[] PROGMEM = {
	0,					// (0) All single-frame animations.
	0,1,				// (1) Lolo, Zombienator, Sokoban World
	0,1,2,				// (3) Megatris, Whack-A-Mole, Arkanoid, Dr Mario, Space Invaders
	0,1,2,3,			// (6)
	0,1,2,3,4,			// (10) Pong
	0,1,2,3,4,5,		// (15)
	0,1,0,2,			// (21) Pac-Man
	0,1,2,1				// (25) Unknown, B.C. Dash, Lode Runner, Kernel
};

const animation animations[] PROGMEM = {
	{ 4, 25, 3, mapUnknown0 },
	{ 3, 3, 12, mapMegatris0 },
	{ 3, 3, 12, mapWhackAMole0 },
	{ 1, 0, 0, mapVoidFighter0 },

	{ 5, 10, 7, mapPong0 },
	{ 3, 3, 12, mapArkanoid0 },
	{ 3, 3, 12, mapSpaceInvaders0 },
	{ 4, 21, 3, mapPacman0 },

	{ 4, 25, 17, mapBcDash0 },
	{ 2, 1, 1, mapAdvOfLolo0 },
	{ 2, 1, 15, mapZombienator0 },
	{ 4, 25, 12, mapLodeRunner0 },

	{ 3, 3, 21, mapDrMario0 },
	{ 2, 1, 24, sokobanWorld0 },
	{ 4, 25, 12, mapKernel0 }
};

// findGameIndex() expects these to be sorted by id if in binary search mode.
gameDetails games[] PROGMEM = {
	{ 2, megatrisGame, 1 },
	{ 3, whackAMoleGame, 2 },
	{ 4, voidFighterGame, 3 },
	{ 5, pongGame, 4 },

	{ 6, arkanoidGame, 5 },
	{ 7, drMarioGame, 12 },
	{ 8, lodeRunnerGame, 11 },
	{ 12, spaceInvadersGame, 6 },

	{ 13, pacmanGame, 7 },
	{ 14, bcdashGame, 8 },
	{ 15, bcdashGame, 8 },
	{ 61, sokobanWorldGame, 13 },

	{ 62, sokobanWorldGame, 13 },
	{ 63, advOfLoloGame, 9 },
	{ 89, zombienatorGame, 10 },
	{ 569, corridaNebososaGame, 0 },

	{ 666, castlevaniaGame, 0 },
	{ EEPROM_SIGNATURE, kernelGame, 14 } // 0x555A
};

/****************************************
 *			Function declarations		*
 ****************************************/

void fillRegion(u8 x, u8 y, u8 width, u8 height, u16 tile);
void drawLogo();
void drawMenu(tileMenu *m);
void closeMenu(tileMenu *m);
int moveCursor(menuCursor *cursor, char dist);
void drawCursor(menuCursor *cursor);
void hideCursor(menuCursor *cursor);
void printPgmStrings(u8 x, u8 y, u8 count, PGM_P *strs);
void setState(int newState);
void activateItem(int index);
void initMenus(void);
void readBlock(struct EepromBlockStruct *block, int index);
void writeBlock(struct EepromBlockStruct *block, int index);
void formatBlock(int index);
void flipPage(char dir);
void dumpHex(struct EepromBlockStruct *ebs);
void loadGameIds(void);
void printPageNumber(void);
void hidePageNumber(void);
int findGameIndex(u16 id);
void drawAnimationFrame(u8 x, u8 y, const int *frames, u8 frameIndex);
void loadMenuAnimation(u16 gameId);
void animateMenuSelection(void);
void printKernelBlock(void);
void hideKernelBlock(void);
void cycleHexCell(u16 btnPressed, u16 btnHeld);
u8 autoScrollMenuH(u16 btnPressed, u16 btnHeld, u8 inertia, u8 interval);
u8 autoScrollMenuV(u16 btnPressed, u16 btnHeld, u8 inertia, u8 interval);

/****************************************
 *			Function definitions		*
 ****************************************/

void printKernelBlock(void) {
	int x = KERNEL_BLOCK_X, y = KERNEL_BLOCK_Y;
	readBlock(&ebs, 0);

	struct EepromHeaderStruct *ehs = (struct EepromHeaderStruct*)&ebs;

	Print(x+2, y, kbTitle);
	y += 2;
	Print(x + 5, y++, kbFeatures);
	Print(x, y, kbFeatures0);
	Print(x + strlen_P(kbFeatures0) + 1, y, ((ehs->features&7) == 0) ? kbSnes : ((ehs->features&7) == 1) ? kbNes : kbJoyStickOther );
	Print(x + KERNEL_BLOCK_SPACING_X, y, kbFeatures5);
	Print(x + KERNEL_BLOCK_SPACING_X + strlen_P(kbFeatures5) + 1, y++, (ehs->features&0x80) ? bitOn : bitOff);
	Print(x, y, kbFeatures1);
	Print(x + strlen_P(kbFeatures1) + 1, y, (ehs->features&8) ? bitOn : bitOff);
	Print(x + KERNEL_BLOCK_SPACING_X, y, kbFeatures6);
	Print(x + KERNEL_BLOCK_SPACING_X + strlen_P(kbFeatures6) + 1, y++, (ehs->features&0x100) ? bitOn : bitOff);
	Print(x, y, kbFeatures2);
	Print(x + strlen_P(kbFeatures2) + 1, y, (ehs->features&0x10) ? bitOn : bitOff);
	Print(x + KERNEL_BLOCK_SPACING_X, y, kbFeatures7);
	Print(x + KERNEL_BLOCK_SPACING_X + strlen_P(kbFeatures7) + 1, y++, (ehs->features&0x200) ? bitOn : bitOff);
	Print(x, y, kbFeatures3);
	Print(x + strlen_P(kbFeatures3) + 1, y, (ehs->features&0x20) ? bitOn : bitOff);
	Print(x + KERNEL_BLOCK_SPACING_X, y, kbFeatures8);
	Print(x + KERNEL_BLOCK_SPACING_X + strlen_P(kbFeatures8) + 1, y++, (ehs->features&0x400) ? bitOn : bitOff);
	Print(x, y, kbFeatures4);
	Print(x + strlen_P(kbFeatures4) + 1, y, (ehs->features&0x40) ? bitOn : bitOff);
	Print(x + KERNEL_BLOCK_SPACING_X, y, kbFeatures9);
	Print(x + KERNEL_BLOCK_SPACING_X + strlen_P(kbFeatures9) + 1, y++, (ehs->features&0x800) ? bitOn : bitOff);
	++y;
	Print(x + 6, y++, kbSpecs);
	Print(x, y, kbSpecs0);
	PrintHexInt(x + strlen_P(kbSpecs0), y++, ehs->signature);
	Print(x, y, kbSpecs1);
	PrintHexByte(x + strlen_P(kbSpecs1), y++, ehs->version);
	Print(x, y, kbSpecs2);
	PrintHexByte(x + strlen_P(kbSpecs2), y++, ehs->blockSize);
	Print(x, y, kbSpecs3);
	PrintHexByte(x + strlen_P(kbSpecs3), y++, ehs->headerSize);
	Print(x, y, kbSpecs4);
	PrintHexByte(x + strlen_P(kbSpecs4), y++, ehs->hardwareVersion);
	Print(x, y, kbSpecs5);
	PrintHexByte(x + strlen_P(kbSpecs5), y++, ehs->hardwareRevision);
	Print(x, y, kbSpecs6);
	PrintHexInt(x + strlen_P(kbSpecs6), y++, ehs->featuresExt);
	Print(x, y, kbSpecs7);

	for (u8 i = 0; i < 6; i++)
		PrintHexByte(x + strlen_P(kbSpecs7) + (i<<1), y, ehs->macAdress[i]);
	++y;
	Print(x, y, kbSpecs8);
	PrintHexByte(x + strlen_P(kbSpecs8), y++, ehs->colorCorrectionType);
	Print(x, y, kbSpecs9);
	PrintHexLong(x + strlen_P(kbSpecs9), y++, ehs->currentGameCrc32);
	Print(x, y, kbSpecs10);
}


void hideKernelBlock(void) {
	fillRegion(KERNEL_BLOCK_X, KERNEL_BLOCK_Y, KERNEL_BLOCK_WID, KERNEL_BLOCK_HGT, CLEAR_TILE);
}


void fillRegion(u8 x, u8 y, u8 width, u8 height, u16 tile) {
	for (u8 i = y; i < (y + height); i++) {
		for (u8 j = x; j < (x + width); j++)
			SetTile(j, i, tile);
	}
}


void drawLogo(void) {
	u8 logoTimer = 4;
	int i = SCREEN_TILES_V - MAPEMUZELOGO_HEIGHT;
	
	while (i > 0) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();

			if (--logoTimer == 0) {
				DrawMap((i&1)*2, i, mapEmuzeLogo);
				logoTimer = 4;
				i -= MAPEMUZELOGO_HEIGHT;
			}
		}
	}
}


int findGameIndex(u16 id) {
#if 1 // Binary search (sorted list required)
	int lower = 0, upper = GAMES_COUNT+1, probe;
	
	while ((probe = (upper-lower)>>1)) {
		probe += lower;

		if (gameIds[probe] < id) {
			lower = probe;
		} else if (gameIds[probe] > id) {
			upper = probe;
		} else {
			return probe;
		}
	}
	if (gameIds[probe] == id)
		return probe;
	else
		return -1;
#else // Linear search (sorted list optional)
	for (int i = 0; i < GAMES_COUNT; i++) {
		if (gameIds[i] == id)
			return i;
	}
	return -1;
#endif
}


void drawAnimationFrame(u8 x, u8 y, const int *frames, u8 frameIndex) {
	DrawMap(x, y, frames + (frameIndex * FRAME_SIZE));
}


void loadMenuAnimation(u16 gameId) {
	int index = findGameIndex(pageIds[state.indexes[state.curr] % GAMES_PER_PAGE]);

	if (index == -1) {
		memcpy_P(&menuAnim.anim, animations, sizeof(menuAnim.anim));
		menuAnim.duration = pgm_read_word(frameDurations + menuAnim.anim.frameDurationsIndex);
	} else {
		gameDetails details;
		memcpy_P(&details, games + index, sizeof(details));
		memcpy_P(&menuAnim.anim, animations + details.animIndex, sizeof(menuAnim.anim));
		menuAnim.duration = pgm_read_word(frameDurations + menuAnim.anim.frameDurationsIndex);
	}
	menuAnim.frame = 0;
	menuAnim.counter = 0;
	drawAnimationFrame(blockMenu.loc.x, blockMenu.loc.y + (state.indexes[navBlock] % GAMES_PER_PAGE) * (GAME_ICON_HGT+1), 
			menuAnim.anim.frames, menuAnim.frame);
}


void animateMenuSelection(void) {
	if (++menuAnim.counter >= menuAnim.duration) {
		menuAnim.counter = 0;
		
		if (++menuAnim.frame >= menuAnim.anim.frameCount)
			menuAnim.frame = 0;
		menuAnim.duration = pgm_read_word(frameDurations + menuAnim.anim.frameDurationsIndex + menuAnim.frame);
		drawAnimationFrame(blockMenu.loc.x, blockMenu.loc.y + (state.indexes[navBlock] % GAMES_PER_PAGE) * (GAME_ICON_HGT+1), 
				menuAnim.anim.frames, pgm_read_byte(frameSequences + menuAnim.anim.frameSequenceIndex + menuAnim.frame));
	}
}


void flipPage(char dir) {
	if (dir)
		dir = dir / ABS(dir); // Only single unit movement
	page = (page + dir) % PAGE_COUNT;

	if (page < 0)
		page = PAGE_COUNT - 1;
	// Load page ids
	u16 offset;

	for (u8 i = 0; i < GAMES_PER_PAGE; i++) {
		offset = (i + page * GAMES_PER_PAGE) * EEPROM_BLOCK_SIZE;

		if (offset < 64*EEPROM_BLOCK_SIZE)
			pageIds[i] = (u16)ReadEeprom(offset) + ((u16)ReadEeprom(offset+1)<<8);
		else
			break;
	}
	state.indexes[navBlock] = page * GAMES_PER_PAGE + state.indexes[navBlock] % GAMES_PER_PAGE;
	// Clamp to our index bounds
	moveCursor(&blockMenu.cursor, 0);
}


void drawMenu(tileMenu *m) {
	if (m->isVisible)
		return;
	m->isVisible = 1;

	if (m == &blockMenu) {
		u8 x = m->loc.x, y = m->loc.y;
		int index = 0;
		gameDetails details;

		for (u8 i = 0; i < GAMES_PER_PAGE; i++, y += GAME_ICON_HGT+1) {
			if ((page * GAMES_PER_PAGE + i) >= EEPROM_BLOCK_COUNT)
				break;
			// Find game id that matches idList for this page
			index = findGameIndex(pageIds[i]);

			if (index != -1) {
				memcpy_P(&details, games+index, sizeof(details));
			} else {
				details.title = unkownGame;
				details.animIndex = 0;
			}
			// Draw map and print text for game that matches id
			if ((page * GAMES_PER_PAGE + i) == state.indexes[navBlock]) {
				// Don't force the animated selection to its initial frame
				drawAnimationFrame(blockMenu.loc.x, blockMenu.loc.y + (state.indexes[navBlock] % GAMES_PER_PAGE) * (GAME_ICON_HGT+1), 
						menuAnim.anim.frames, menuAnim.frame);
			} else {
				// Only want the frames from the animation list
				const int *frames = (const int*)pgm_read_word(
						(const char*)(animations + details.animIndex)+(sizeof(animation)-sizeof(int*)));
				drawAnimationFrame(x, y, frames, 0);
			}
			PrintInt(x + GAME_ICON_WID + 6, y+1, pageIds[i], 1);

			if (pageIds[i] == EEPROM_FREE_BLOCK)
				Print(x + GAME_ICON_WID + 8, y+1, freeBlockStr);
			else
				Print(x + GAME_ICON_WID + 8, y+1, details.title);
		}
	} else if (m == &menu) {
		u8 x = m->loc.x, y = m->loc.y, w = m->s.w, h = m->s.h;

		// Save underlying tiles
		for (u8 i = y; i < (y+h); i++)
			memcpy(m->buffer + (i - y)*w*2, vram + (x + i*VRAM_TILES_H)*2, w*2);

		// Draw menu
		SetTile(x, y, MENU_TOP_LEFT);
		fillRegion(x+1, y, w-2, 1, MENU_HORIZ_TOP);
		SetTile(x+w-1, y, MENU_TOP_RIGHT);
		++y;
	
		for (u8 i = 1; i < (h-1); i++) {
			SetTile(x, y, MENU_VERT_LEFT);
			fillRegion(x+1, y, w-2, 1, CLEAR_TILE);
			SetTile(x+w-1, y, MENU_VERT_RIGHT);
			++y;
		}
	
		SetTile(x, y, MENU_BTM_LEFT);
		fillRegion(x+1, y, w-2, 1, MENU_HORIZ_BTM);
		SetTile(x+w-1, y, MENU_BTM_RIGHT);
	} else if (m == &hexMenu) {
		readBlock(&ebs, state.indexes[navBlock]);
		dumpHex(&ebs);
	}
}


void closeMenu(tileMenu *m) {
	if (!m->isVisible)
		return;
	m->isVisible = 0;

	u8 y = m->loc.y;

	if (m == &blockMenu) {
		for (u8 i = 0; i < GAMES_PER_PAGE; i++, y += GAME_ICON_HGT+1)
			fillRegion(m->loc.x, y, BLOCK_MENU_CELL_WID, BLOCK_MENU_CELL_HGT, CLEAR_TILE);
		hideCursor(&m->cursor);
	} else if (m == &menu) {
		// Restore underlying tiles
		for (u8 i =m->loc.y; i < (m->loc.y + m->s.h); i++)
			memcpy(vram + (m->loc.x + i*VRAM_TILES_H)*2, m->buffer + (i - m->loc.y)*m->s.w*2, m->s.w*2);
	} else if (m == &hexMenu) {
		fillRegion(hexMenu.loc.x, hexMenu.loc.y, 32, 3, CLEAR_TILE);
		hideCursor(&m->cursor);
	}
}


void hideCursor(menuCursor *cursor) {
	SetTile(cursor->loc.x, cursor->loc.y, CLEAR_TILE);
}


int moveCursor(menuCursor *cursor, char dist) {
	int index = state.indexes[state.curr], prevIndex = index;

	hideCursor(cursor);

	if (cursor == &blockMenu.cursor) {
		index += dist;

		if (index < 0)
			index = GAMES_PER_PAGE + index;
		state.indexes[state.curr] = (page * GAMES_PER_PAGE) + (index % GAMES_PER_PAGE);
		cursor->loc.y = (BLOCK_MENU_LOC_Y+1) + (GAME_ICON_HGT+1) * (state.indexes[state.curr] % GAMES_PER_PAGE);

		if (state.indexes[navBlock] >= EEPROM_BLOCK_COUNT) {
			if (dist < 0)
				moveCursor(cursor, -1);
			else
				moveCursor(cursor, page * GAMES_PER_PAGE - state.indexes[navBlock]);
		}
	} else if (cursor == &menu.cursor) {
		if (index == cursor->r.min && dist < 0) {
			cursor->loc.y += cursor->r.max;
			state.indexes[state.curr] = cursor->r.max;
		} else if (index == cursor->r.max && dist == 1) {
			cursor->loc.y -= cursor->r.max;
			state.indexes[state.curr] = cursor->r.min;
		} else {
			cursor->loc.y += dist;
			state.indexes[state.curr] += dist;
		}
	} else if (cursor == &hexMenu.cursor) {
		index = (index + dist) % (cursor->r.max);

		if (index < 0)
			index = cursor->r.max + index;
		state.indexes[navHexDump] = index;
		state.indexes[navHexCell] = index;
		cursor->loc.x = HEX_DUMP_X + index % cursor->r.mid;
		cursor->loc.y = (HEX_DUMP_Y - 1) + 2 * (index / cursor->r.mid);
	}
	drawCursor(cursor);
	return prevIndex;
}


void drawCursor(menuCursor *cursor) {
	SetTile(cursor->loc.x, cursor->loc.y, cursor->tile);
}


void printPgmStrings(u8 x, u8 y, u8 count, PGM_P *strs) {
	u8 i = 0;

	while (count--)
		Print(x, y++, (PGM_P)pgm_read_word(strs+i++));
}


void printPageNumber(void) {
	u8 x = (page > 8) ? 1 : 0;

	hidePageNumber();
	PrintInt(PAGE_NO_LOC_X + 8 + x, PAGE_NO_LOC_Y, PAGE_COUNT, 0);
	Print(PAGE_NO_LOC_X + 6 + x, PAGE_NO_LOC_Y, fwdSlashString);
	PrintInt(PAGE_NO_LOC_X + 5 + x, PAGE_NO_LOC_Y, page+1, 0);
	Print(PAGE_NO_LOC_X, PAGE_NO_LOC_Y, pageStr);
}


void hidePageNumber(void) {
	fillRegion(PAGE_NO_LOC_X, PAGE_NO_LOC_Y, PAGE_NO_WID, PAGE_NO_HGT, CLEAR_TILE);
}

void setState(int newState) {
	int oldState = state.curr;

	// The state stack (history) is not currently referenced (although it is maintained). It
	// may be advisable to pop the stack when closing menus so that any menu hierarchy changes
	// filter throughout the code.
	if (newState < navBlock || newState > navKernel)
		return;
	if (newState > state.curr) {
		*state.prev = state.curr;
		++state.prev;
		state.curr = newState;
	} else if (newState < state.curr) {
		while (state.curr > newState) {
			--state.prev;
			state.curr = *state.prev;
		}
	}

	switch (state.curr) {
		case navBlock:
			closeMenu(&hexMenu);
			closeMenu(&menu);
			printPageNumber();
			blockMenu.cursor.tile = CURSOR_VGREEN;
			drawCursor(&blockMenu.cursor);
			break;
		case navMenu:
			hidePageNumber();

			if (oldState == navBlock) {
				drawMenu(&menu);
				printPgmStrings(17, 9, MENU_OPTIONS_COUNT, menuStrings);
				drawMenu(&hexMenu);
			} else if (oldState == navHexDump) {
				hideCursor(&hexMenu.cursor);
			}
			menu.cursor.tile = CURSOR_VGREEN;
			drawCursor(&menu.cursor);
			break;
		case navHexDump:
			hexMenu.cursor.tile = CURSOR_HGREEN;
			drawCursor(&hexMenu.cursor);
			break;
		case navHexCell:
			break;
		case navKernel:
			hidePageNumber();
			printKernelBlock();
			break;
	}
}


void dumpHex(struct EepromBlockStruct *ebs) {
	u8 x = HEX_DUMP_X, y = HEX_DUMP_Y;
	u8 *dptr = (u8*)ebs;

	for (u8 i = 0; i < EEPROM_BLOCK_SIZE; i++, x+=2) {
		if (i && ((i&15) == 0)) {
			x = HEX_DUMP_X;
			y += 2;
		}
		PrintHexByte(x, y, *dptr++);
	}
}


void activateItem(int index) {
	switch (state.curr) {
		case navBlock:
			if (pageIds[state.indexes[state.curr] % GAMES_PER_PAGE] == EEPROM_SIGNATURE) {
				closeMenu(&blockMenu);
				setState(navKernel);
			} else {
				blockMenu.cursor.tile = CURSOR_VRED;
				drawCursor(&blockMenu.cursor);
				setState(navMenu);
			}
			break;
		case navMenu:
			switch (index) {
				case OPTION_EDIT:
					menu.cursor.tile = CURSOR_VRED;
					drawCursor(&menu.cursor);
					setState(navHexDump);
					break;
				case OPTION_COPY:
					memcpy(&clipboard, &ebs, sizeof(clipboard));
					break;
				case OPTION_RELOAD:
					closeMenu(&hexMenu);
					drawMenu(&hexMenu);
					break;
				case OPTION_PASTE:
					if (clipboard.id == EEPROM_FREE_BLOCK)
						break;
					memcpy(&ebs, &clipboard, sizeof(ebs));
					// Allow fall through
				case OPTION_SAVE:
					writeBlock(&ebs, state.indexes[navBlock]);
					setState(navBlock);
					flipPage(0);
					closeMenu(&blockMenu);
					drawMenu(&blockMenu);
					loadMenuAnimation(state.indexes[state.curr]);
					activateItem(state.indexes[state.curr]);
					break;
				case OPTION_FORMAT:
					formatBlock(state.indexes[navBlock]);
					setState(navBlock);
					flipPage(0);
					closeMenu(&blockMenu);
					drawMenu(&blockMenu);
					loadMenuAnimation(state.indexes[state.curr]);
					activateItem(state.indexes[state.curr]);
					break;
			}
			break;
		case navHexDump:
			hexMenu.cursor.tile = CURSOR_HRED;
			drawCursor(&hexMenu.cursor);
			setState(navHexCell);
			break;
		case navHexCell:
			break;
		case navKernel:
			hideKernelBlock();
			setState(navBlock);
			drawMenu(&blockMenu);
			break;
	}
}


void readBlock(struct EepromBlockStruct *block, int index) {
	u8 *destPtr;
	u16 destAddr;

	destPtr = (u8*)block;
	destAddr = index * EEPROM_BLOCK_SIZE;

	for(u8 j = 0; j < EEPROM_BLOCK_SIZE; j++)
		*destPtr++ = ReadEeprom(destAddr++);
}


void writeBlock(struct EepromBlockStruct *block, int index) {
	u8 *dptr = (u8*)block;

	for (u8 i = 0; i < EEPROM_BLOCK_SIZE; i++)
		WriteEeprom(index*EEPROM_BLOCK_SIZE+i, *dptr++);
}


void formatBlock(int index) {
	for (u8 i = 0; i < sizeof(ebs.id); i++)
		WriteEeprom(index*EEPROM_BLOCK_SIZE+i,(u8)EEPROM_FREE_BLOCK);
}


void PrintHexNibble(char x, char y, u8 byte) {
	if (byte <= 9){
		SetFont(x,y,byte+('0'-' '));
	} else {
		SetFont(x,y,byte+('A'-' '-10));
	}
}


void cycleHexCell(u16 btnPressed, u16 btnHeld) {
	static u8 cycleTimer = 6;
	
	if ((btnPressed|btnHeld) & (BTN_UP|BTN_DOWN))  {
		if ((btnPressed) & (BTN_UP|BTN_DOWN)) {
			cycleTimer = 6;
		} else if (btnHeld & (BTN_UP|BTN_DOWN)) {
			if (cycleTimer) {
				--cycleTimer;
				return;
			} else {
				cycleTimer = 3;
			}
		}
		char dir = (btnHeld & BTN_UP) ? 1 : -1;

		int index = state.indexes[navHexDump]>>1;
		u8 *dptr = (u8*)&ebs + index;

		if (state.indexes[navHexDump]&1) {
			*dptr = ((*dptr)&0xf0) + (((*dptr)+dir)&0x0f);
			PrintHexNibble(hexMenu.cursor.loc.x, hexMenu.cursor.loc.y+1, (*dptr)&0x0f);
		} else {
			*dptr = (((*dptr)+dir*0x10)&0xf0) + ((*dptr)&0x0f);
			PrintHexNibble(hexMenu.cursor.loc.x, hexMenu.cursor.loc.y+1, (*dptr)>>4);
		}
	}
}


void initMenus(void) {
	blockMenu.loc = (pt){BLOCK_MENU_LOC_X, BLOCK_MENU_LOC_Y};
	blockMenu.s = (size){0, 0};
	blockMenu.cursor.tile = CURSOR_VGREEN;
	blockMenu.cursor.r = (range){0, GAMES_PER_PAGE-1, GAMES_PER_PAGE-1};
	blockMenu.cursor.loc = (pt){blockMenu.loc.x-1, blockMenu.loc.y+1};
	blockMenu.buffer = 0;

	menu.loc = (pt){15, 8};
	menu.s = (size){10, MENU_OPTIONS_COUNT+2};
	menu.cursor.tile = CURSOR_VGREEN;
	menu.cursor.r = (range){0, MENU_OPTIONS_COUNT-1, MENU_OPTIONS_COUNT-1};
	menu.cursor.loc = (pt){menu.loc.x+1, menu.loc.y+1};
	menu.buffer = menuBuffer;

	hexMenu.loc = (pt){HEX_DUMP_X, HEX_DUMP_Y};
	hexMenu.s = (size){EEPROM_BLOCK_SIZE, 3};
	hexMenu.cursor.tile = CURSOR_HGREEN;
	hexMenu.cursor.r = (range){0, EEPROM_BLOCK_SIZE, EEPROM_BLOCK_SIZE<<1};
	hexMenu.cursor.loc = (pt){hexMenu.loc.x, hexMenu.loc.y-1};
	hexMenu.buffer = 0;
}


void loadGameIds(void) {
	for (u16 i = 0; i < GAMES_COUNT; i++)
		gameIds[i] = pgm_read_word(games+i);
}


u8 autoScrollMenuH(u16 btnPressed, u16 btnHeld, u8 inertia, u8 interval) {
	if ((btnPressed | btnHeld) & (BTN_LEFT|BTN_RIGHT)) {
		if (btnPressed & (BTN_LEFT|BTN_RIGHT))
			menuScrollTimer = inertia;
		else if (--menuScrollTimer)
			return 0;
		else
			menuScrollTimer = interval;
		return 1;
	}
	return 0;
}


u8 autoScrollMenuV(u16 btnPressed, u16 btnHeld, u8 inertia, u8 interval) {
	if ((btnPressed | btnHeld) & (BTN_UP|BTN_DOWN)) {
		if (btnPressed & (BTN_UP|BTN_DOWN))
			menuScrollTimer = inertia;
		else if (--menuScrollTimer)
			return 0;
		else
			menuScrollTimer = interval;
		return 1;
	}
	return 0;
}


int main(void) {
	u8 menuActivationTimer = 0;
	u16 btnPrev = 0;			// Previous buttons that were held
	u16 btnHeld = 0;    		// Buttons that are held right now
	u16 btnPressed = 0;  		// Buttons that were pressed this frame
	u16 btnReleased = 0;		// Buttons that were released this frame

	SetFontTable(fontset);
	SetTileTable(tileset);
	ClearVram();
	drawLogo();
	state.prev = state.history;
	clipboard.id = EEPROM_FREE_BLOCK;
	loadGameIds();
	initMenus();
	setState(navBlock);
	page = -1;
	flipPage(1);
	printPageNumber();
	state.indexes[navBlock] = 0;
	loadMenuAnimation(state.indexes[state.curr]);
	drawMenu(&blockMenu);
	
	while(1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();

			btnHeld = ReadJoypad(0);
			btnPressed = btnHeld&(btnHeld^btnPrev);
        	btnReleased = btnPrev&(btnHeld^btnPrev);
			btnPrev = btnHeld;

			if (state.curr != navKernel)
				animateMenuSelection();

			switch (state.curr) {
				case navBlock:
					if (btnPressed & BTN_A) {
						activateItem(state.indexes[state.curr]);
					} else if (autoScrollMenuV(btnPressed, btnHeld, 8, 4)) {
						int index = moveCursor(&blockMenu.cursor, (btnHeld&BTN_UP) ? -1 : 1);
						drawCursor(&blockMenu.cursor);

						// Don't re-load animations for pages with only one icon
						if (index != state.indexes[state.curr]) {
							drawAnimationFrame(blockMenu.loc.x, blockMenu.loc.y + (index % GAMES_PER_PAGE) * (GAME_ICON_HGT+1), 
										menuAnim.anim.frames, 0);
							loadMenuAnimation(state.indexes[state.curr]);
						}
					} else if (autoScrollMenuH(btnPressed, btnHeld, 12, 8)) {
						hideCursor(&blockMenu.cursor);
						flipPage((btnHeld&BTN_LEFT) ? -1 : 1);
						printPageNumber();
						closeMenu(&blockMenu);
						drawMenu(&blockMenu);
						drawCursor(&blockMenu.cursor);
						loadMenuAnimation(state.indexes[state.curr]);
					}
					break;
				case navMenu:
					if (!menu.isVisible) {
						setState(navBlock);
						continue;
					}
					if (menuActivationTimer) {
						if (--menuActivationTimer == 0) {
							menu.cursor.tile = CURSOR_VGREEN;
							drawCursor(&menu.cursor);
						} else {
							continue;
						}
					}
					if (autoScrollMenuV(btnPressed, btnHeld, 8, 4)) {
						moveCursor(&menu.cursor, (btnHeld&BTN_UP) ? -1 : 1);
						drawCursor(&menu.cursor);
					} else if (btnPressed & BTN_A) {
						activateItem(state.indexes[state.curr]);

						// Visualize button depression for all but editing option
						if (state.indexes[state.curr]) {
							menu.cursor.tile = CURSOR_VRED;
							drawCursor(&menu.cursor);
							menuActivationTimer = 8;
						}
					} else if (btnPressed & BTN_B) {
						setState(navBlock);
					}
					break;
				case navHexDump:
					if (btnPressed & BTN_A) {
						activateItem(state.indexes[state.curr]);
					} else if (btnPressed & BTN_B) {
						setState(navMenu);
					} else if (autoScrollMenuH(btnPressed, btnHeld, 4, 2)) {
						moveCursor(&hexMenu.cursor, (btnHeld&BTN_LEFT) ? -1 : 1);
						drawCursor(&hexMenu.cursor);
					} else if (btnPressed & (BTN_UP|BTN_DOWN)) {
						moveCursor(&hexMenu.cursor, (btnPressed&BTN_UP) ? -hexMenu.cursor.r.mid : hexMenu.cursor.r.mid);
						drawCursor(&hexMenu.cursor);
					}
					break;
				case navHexCell:
					if (btnPressed & (BTN_A | BTN_B)) {
						setState(navHexDump);
					} else if ((btnPressed | btnHeld) & (BTN_UP|BTN_DOWN)) {
						cycleHexCell(btnPressed, btnHeld);
					} else if (autoScrollMenuH(btnPressed, btnHeld, 4, 2)) {
						moveCursor(&hexMenu.cursor, (btnHeld&BTN_LEFT) ? -1 : 1);
						drawCursor(&hexMenu.cursor);
					}
					break;
				case navKernel:
					if (btnPressed & BTN_B)
						activateItem(state.indexes[state.curr]);
					break;
			}
		}		
	}
}





