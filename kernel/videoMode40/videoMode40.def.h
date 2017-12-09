/*
 *  Uzebox Kernel - Video Mode 40
 *  Copyright (C) 2017 Sandor Zsuga (Jubatian)
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
 * Global defines for video mode 40
 *
 * ===========================================================================
 */

#pragma once

#define VMODE_ASM_SOURCE "videoMode40/videoMode40core.s"
#define VMODE_C_SOURCE "videoMode40/videoMode40.c"
#define VMODE_C_PROTOTYPES "videoMode40/videoMode40.h"
#define VMODE_FUNC sub_video_mode40


#define TILE_WIDTH  8
#define TILE_HEIGHT 8

#ifndef SCREEN_TILES_H
	#ifndef VRAM_TILES_H
		#define SCREEN_TILES_H 40
	#else
		#define SCREEN_TILES_H VRAM_TILES_H
	#endif
#endif

#ifndef VRAM_TILES_H
	#define VRAM_TILES_H SCREEN_TILES_H
#endif

#ifndef SCREEN_TILES_V
	#ifndef VRAM_TILES_V
		#define SCREEN_TILES_V 25
	#else
		#define SCREEN_TILES_V VRAM_TILES_V
	#endif
#endif

#ifndef VRAM_TILES_V
	#define VRAM_TILES_V SCREEN_TILES_V
#endif

#ifndef SCREEN_BORDER_V
	#define SCREEN_BORDER_V 6
#endif

#ifndef FIRST_RENDER_LINE
	#if ((20 + ((224 - (SCREEN_TILES_V * 8)) / 2) - SCREEN_BORDER_V) >= 20)
		#define FIRST_RENDER_LINE 20 + ((224 - (SCREEN_TILES_V * TILE_HEIGHT)) / 2) - SCREEN_BORDER_V
	#else
		#define FIRST_RENDER_LINE 20
	#endif
#endif

#ifndef FRAME_LINES
	#if (((SCREEN_TILES_V * TILE_HEIGHT) + (2 * SCREEN_BORDER_V)) <= 224)
		#define FRAME_LINES ((SCREEN_TILES_V * TILE_HEIGHT) + (2 * SCREEN_BORDER_V))
	#else
		#define FRAME_LINES 224
	#endif
#endif

/* VRAM characteristics */

#define VRAM_SIZE (VRAM_TILES_H * VRAM_TILES_V)
#define VRAM_ADDR_SIZE 1
#define VRAM_PTR_TYPE unsigned char

/* Maximum free cycles usable by the hysnc and audio (Note: more than 230 has
** no use since the kernel has such limitation elsewhere) */

#define HSYNC_USABLE_CYCLES 251

/* Extras: Various font tileset */

#ifndef M40_C64_GRAPHICS
	#define M40_C64_GRAPHICS 0
#endif
#ifndef M40_C64_ALPHA
	#define M40_C64_ALPHA 0
#endif
#ifndef M40_C64_MIXED
	#define M40_C64_MIXED 0
#endif
#ifndef M40_IBM_ASCII
	#define M40_IBM_ASCII 0
#endif
#ifndef M40_MATTEL
	#define M40_MATTEL 0
#endif
