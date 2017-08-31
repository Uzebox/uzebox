/*
 *  Uzebox(tm) Video Mode 13
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
 * Global defines for video mode 13
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE "videoMode13/videoMode13core.s"
#define VMODE_C_SOURCE "videoMode13/videoMode13.c"
#define VMODE_C_PROTOTYPES "videoMode13/videoMode13.h"
#define VMODE_FUNC sub_video_mode13

//No scrolling saves RAM and allows a 30x28 screen resolution
//while scrolling reduces to 28x28 and require vram of at least 32 tiles wide.
#ifndef SCROLLING
	#define SCROLLING 0
#endif

#ifndef TILE_HEIGHT
	#define TILE_HEIGHT 8
#endif

#ifndef	TILE_WIDTH
	#define TILE_WIDTH 8
#endif

#ifndef OVERLAY_LINES
	#define OVERLAY_LINES 0
#endif

#define CYCLES_PER_PIXELS 6

#if SCROLLING == 0
	#ifndef VRAM_TILES_H
		#define VRAM_TILES_H 30
	#endif
	#define SCREEN_TILES_H VRAM_TILES_H 
	#define FILL_DELAY ((CYCLES_PER_PIXELS*TILE_WIDTH)*(30-VRAM_TILES_H))/2 //use to generate filler cycles when VRAM_TILES_H<30
#else
	#define VRAM_TILES_H 32
	#ifndef SCREEN_TILES_H
		#define SCREEN_TILES_H 28
	#endif
	#define FILL_DELAY 0
#endif

#ifndef SCREEN_TILES_V
	#define SCREEN_TILES_V 28
#endif

#if SCROLLING == 0
	#ifndef VRAM_TILES_V
		#define VRAM_TILES_V SCREEN_TILES_V		
	#endif
#else
	#ifndef VRAM_TILES_V
		#define VRAM_TILES_V 32	
	#endif

	#if SCROLLING == 1 && (VRAM_TILES_V!=32 && VRAM_TILES_V!=24 && VRAM_TILES_V!=16)
		#error Invalid VRAM_TILES_V value defined in makefile. Supported values are: 16,24,32.
	#endif
#endif


#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 20	
#endif

#ifndef FRAME_LINES
	#define FRAME_LINES SCREEN_TILES_V*TILE_HEIGHT
#endif

#define SPRITE_STRUCT_SIZE 4

#ifndef TRANSLUCENT_COLOR
	#define TRANSLUCENT_COLOR 0xfe	
#endif

#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V	
#define VRAM_ADDR_SIZE 1 //in bytes
#define VRAM_PTR_TYPE char

#define SPRITES_ENABLED 1

#if RAM_TILES_COUNT==0 && MAX_SPRITES>0
	#error Sprites are used (MAX_SPRITES>0) but RAM_TILES_COUNT==0 or is undefined.
#endif

#ifndef EXTENDED_PALETTE
	#define EXTENDED_PALETTE 0
#endif

//Sprite flags
#define SPRITE_FLIP_X 1
#define SPRITE_FLIP_Y 2
#define SPRITE_FLIP_X_BIT 0
#define SPRITE_FLIP_Y_BIT 1
#define SPRITE_BANK0 0<<6
#define SPRITE_BANK1 1<<6
#define SPRITE_BANK2 2<<6
#define SPRITE_BANK3 3<<6

#define OFF_SCREEN SCREEN_TILES_H*TILE_WIDTH

#define HSYNC_USABLE_CYCLES 225 //Maximum free cycles usable by the hysnc and audio

#define PALETTE_SIZE 256

#if EXTENDED_PALETTE
	#define MAX_PALETTE_COLORS 15
	#define TRANSPARENT_COLOR 0xF
#else
	#define MAX_PALETTE_COLORS 8
	#define TRANSPARENT_COLOR 0x1
#endif

//maximum possible ramtiles. The limit is
//controller by the cycles available in the
//rendering phase prolog to restore the
//ramtiles indexes
#define MAX_RAMTILES 90
