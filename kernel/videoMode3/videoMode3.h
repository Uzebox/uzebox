/*
 *  Uzebox(tm) Video Mode 3
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

/** 
 * ==============================================================================
 * Function prototypes for video mode 3
 * ===============================================================================
 */
#pragma once
#include <avr/io.h>

extern u8 vram[];  
extern u8 overlay_vram[];  

struct SpriteStruct
{
	u8 x;
	u8 y;
	u8 tileIndex;
	u8 flags;		
};			

struct BgRestoreStruct{
	u8* addr;
	u8 tileIndex;
};

extern struct SpriteStruct sprites[];

typedef struct {
	u8 overlayHeight;
	const char *overlayTileTable;
	#if SCROLLING == 1		  
		u8 scrollX;
		u8 scrollY;
		u8 scrollHeight;
	#endif
} ScreenType;

extern ScreenType Screen;

/*Define the tile table for the specified sprite bank.*/
extern void SetSpritesTileBank(u8 bank,const char* tileData);

extern u8 GetTile(u8 x,u8 y);

/*Write a ramtile index to VRAM*/
#define SetRamTile(x,y,i) SetTile(x,y,i-RAM_TILES_COUNT)

/*Set the number of ramtiles to allocate for the user program. User ramtiles are
 * not use by the kernel and the sprite blitter. User ramtiles are allocated from
 * the beginning of the ramtiles table.*/
extern void SetUserRamTilesCount(u8 count);

/*Get a pointer to the specified tamtile index. User ramtiles are allocated from
 * the beginning of the ramtiles table.*/
extern u8* GetUserRamTile(u8 index);

/*Copy srcTile from the active tileset in flash to destTile ramtile*/
extern void CopyFlashTile(u8 srcTile,u8 destTile);

/*Copy srcTile ramtile to destTile ramtile*/
extern void CopyRamTile(u8 srcTile,u8 destTile);

