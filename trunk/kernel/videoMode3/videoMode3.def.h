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

#ifndef	TILE_WIDTH
	#define TILE_WIDTH 8
#endif

#ifndef OVERLAY_LINES
	#define OVERLAY_LINES 0
#endif

#if SCROLLING == 0
	#define VRAM_TILES_H 30
	#define SCREEN_TILES_H 30
#else
	#define VRAM_TILES_H 32
	#define SCREEN_TILES_H 28
#endif

#ifndef SCREEN_TILES_V
	#define SCREEN_TILES_V 28
#endif

#ifndef VRAM_TILES_V
	#if SCROLLING == 0
		#define VRAM_TILES_V SCREEN_TILES_V		
	#else
		#define VRAM_TILES_V 32	
	#endif
#endif


#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 20	
#endif

#ifndef FRAME_LINES
	#define FRAME_LINES SCREEN_TILES_V*TILE_HEIGHT
#endif

#define SPRITE_STRUCT_SIZE 5
#define TRANSLUCENT_COLOR 0xfe	
#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V	
#define VRAM_ADDR_SIZE 1 //in bytes

#define SPRITES_ENABLED 1

#define SPRITE_FLIP_X 1

