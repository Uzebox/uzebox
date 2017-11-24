/*
 *  Uzebox(tm) Video Mode 3
 *  Copyright (C) 2008 Alec Bourque
 *                2017 Sandor Zsuga (Jubatian)
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
 * Global defines for video mode 3
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE "videoMode3/videoMode3core.s"
#define VMODE_C_SOURCE "videoMode3/videoMode3.c"
#define VMODE_C_PROTOTYPES "videoMode3/videoMode3.h"
#define VMODE_FUNC sub_video_mode3

//No scrolling saves RAM and allows a 30x28 screen resolution
//while scrolling reduces to 28x28 and require vram of at least 32 tiles wide.
#ifndef SCROLLING
	#define SCROLLING 0
#endif

#ifndef TILE_HEIGHT
	#define TILE_HEIGHT 8
#endif

#ifndef TILE_WIDTH
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
	#define FILL_DELAY ((CYCLES_PER_PIXELS*TILE_WIDTH)*(30-VRAM_TILES_H))/2
#else
	#define VRAM_TILES_H 32
	#ifndef SCREEN_TILES_H
		#define SCREEN_TILES_H 28
	#endif
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


/*
** If clear, the ProcessSprites() function and the associated sprite structure
** array is removed, instead you can directly blit sprite tiles using
** BlitSprite. This is useful for writing own sprite managers. Automatic VSync
** sprite processing is not available when this is clear.
*/
#ifndef SPRITES_AUTO_PROCESS
	#define SPRITES_AUTO_PROCESS 1
#endif
#if (SPRITES_AUTO_PROCESS == 0)
	#define SPRITES_VSYNC_PROCESS 0
#endif


/*
** If clear, the ProcessSprites() function will no longer be called from
** VSync, and RestoreBackground() has to be called manually before working
** with the background or calling ProcessSprites() again. Use WaitVsync(1) to
** synchronize to the end of the video frame, then if you want to alter video
** contents for the next frame, call RestoreBackground(), do your work with
** the VRAM (Scrolling and tile updates), finally call ProcessSprites() to
** render the sprites.
*/
#ifndef SPRITES_VSYNC_PROCESS
	#define SPRITES_VSYNC_PROCESS 1
#endif


/*
** If clear, removes the RAM tile restore list. This case the VRAM has to be
** completely rebuilt for rendering a new video frame as there is no
** information to restore it after blitting sprites. This may be useful for
** games rebuilding the VRAM anyway or not using sprites at all (only user RAM
** tiles).
*/
#ifndef RTLIST_ENABLE
	#define RTLIST_ENABLE 1
#endif


/*
** If set, aligns RAM tiles to 64 byte boundary (can only be enabled for 8 x 8
** pixel tiles). This will allocate VRAM and RAM tiles in the .noinit section,
** you should pass linker options to locate these at the beginning og the RAM
** (so you wouldn't waste memory by padding). When the RAM tiles are aligned,
** sprite blitting is faster.
**
** Example: If you have 32 aligned RAM tiles with scrolling (1K VRAM) Mode 3:
**
** LDFLAGS += -Wl,--section-start,.noinit=0x800100 -Wl,--section-start,.data=0x800D00
*/
#ifndef RT_ALIGNED
	#define RT_ALIGNED 0
#endif
#if (RT_ALIGNED != 0)
	#if ((TILE_HEIGHT != 8) || (TILE_WIDTH != 8))
		#error Aligned RAM tiles (RT_ALIGNED) can only be used with 8x8 tiles!
	#endif
#endif


/*
** If set, the mode's resolution is changed from 6 cycles / pixel to 5.5
** cycles per pixel, which allows for up to 32 tiles (256 pixels) displayed
** (31 when scrolling due to the VRAM layout).
*/
#ifndef RESOLUTION_EXT
	#define RESOLUTION_EXT 0
#endif
#if (RESOLUTION_EXT == 0)
	#if (SCREEN_TILES_H > 30)
		#error SCREEN_TILES_H is too large for the current settings!
	#endif
#else
	#if (SCREEN_TILES_H > 32)
		#error SCREEN_TILES_H is too large for the current settings!
	#endif
#endif


/*
** If set, enables RAM sourced sprites (SPRITE_RAM flag), slightly increasing
** the code size of the blitter. RAM sprites are sourced from User RAM tiles.
*/
#ifndef SPRITE_RAM_ENABLE
	#define SPRITE_RAM_ENABLE 0
#endif


//Sprite flags
#define SPRITE_FLIP_X 1
#define SPRITE_FLIP_Y 2
#define SPRITE_OFF    4
#define SPRITE_RAM    8
#define SPRITE_FLIP_X_BIT 0
#define SPRITE_FLIP_Y_BIT 1
#define SPRITE_OFF_BIT    2
#define SPRITE_RAM_BIT    3
#define SPRITE_BANK0 0<<6
#define SPRITE_BANK1 1<<6
#define SPRITE_BANK2 2<<6
#define SPRITE_BANK3 3<<6


#define MAX_RAMTILES 60
#define HSYNC_USABLE_CYCLES 225 //Maximum free cycles usable by the hysnc and audio

/* Note: This is only provided for compatibility with older games. You should
** use the Y coordinate to move a sprite off-screen (when 32 tiles are visible
** horizontally, there is no off screen location by X). */
#define OFF_SCREEN (SCREEN_TILES_H * TILE_WIDTH)
