/*
 *  Uzebox Kernel - Video Mode 74
 *  Copyright (C) 2015 Sandor Zsuga (Jubatian)
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
 * ===========================================================================
 *
 * This file contains global defines for video mode 74
 *
 * ===========================================================================
 */

#pragma once

#define VMODE_ASM_SOURCE "videoMode74/videoMode74.s"
#define VMODE_C_SOURCE "videoMode74/videoMode74.c"
#define VMODE_C_PROTOTYPES "videoMode74/videoMode74.h"
#define VMODE_FUNC sub_video_mode74


/* Since the mode is quite configurable, the widths and heights set up here
** only matter for using the Uzebox kernel functions. So they can be pretty
** arbitrary (even larger than the actual display, to be accessed by
** scrolling) as long as Mode 74 can be set up to display them. */

#ifndef TILE_WIDTH
	#define TILE_WIDTH     8
	#define VRAM_TILES_H   24
	#define SCREEN_TILES_H 24
#elif   TILE_WIDTH == 6
	#define VRAM_TILES_H   32
	#define SCREEN_TILES_H 32
#elif   TILE_WIDTH == 8
	#define VRAM_TILES_H   24
	#define SCREEN_TILES_H 24
#else
	#error Invalid value defined in the makefile for TILE_WIDTH. Supported values are 6 or 8.
#endif

#ifndef TILE_HEIGHT
	#define TILE_HEIGHT 8
#elif   TILE_HEIGHT == 8
#else
	#error Invalid value defined in the makefile for TILE_HEIGHT. Supported value is 8.
#endif

#ifndef VRAM_TILES_V
	#define VRAM_TILES_V   (224 / TILE_HEIGHT)
#endif

#ifndef SCREEN_TILES_V
	#define SCREEN_TILES_V (224 / TILE_HEIGHT)
#endif

#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 20
#endif

#define VRAM_SIZE       (VRAM_TILES_H * VRAM_TILES_V)
#define VRAM_ADDR_SIZE  1
#define VRAM_PTR_TYPE   unsigned char

#define SPRITES_ENABLED 0

#ifndef FRAME_LINES
	#define FRAME_LINES (SCREEN_TILES_V * TILE_HEIGHT)
#endif

/* Maximum free cycles usable by the hysnc and audio */
#define HSYNC_USABLE_CYCLES 220
