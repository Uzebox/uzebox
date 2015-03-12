/*
 *  Uzebox(tm) Video Mode 2
 *  Copyright (C) 20098  Alec Bourque
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
 *
 * This file contains global defines for video mode 2
 *
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE "videoMode2/videoMode2.s"
#define VMODE_C_SOURCE "videoMode2/videoMode2.c"
#define VMODE_C_PROTOTYPES "videoMode2/videoMode2.h"
#define VMODE_FUNC sub_video_mode2

#define TILE_HEIGHT 8
#define TILE_WIDTH 6

#define VRAM_TILES_H  32
#ifndef VRAM_TILES_V
	#define VRAM_TILES_V 32
#endif

#define SCREEN_TILES_H 22
#define SCREEN_TILES_V 26

#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V	
#define MAX_SPRITES_PER_LINE 5
#define SPRITE_STRUCT_SIZE 3
#define TRANSLUCENT_COLOR 0xfe	
#define X_SCROLL_WRAP VRAM_TILES_H*TILE_WIDTH
#define VRAM_ADDR_SIZE 1 //in bytes	
#define VRAM_PTR_TYPE char	
#define SPRITES_ENABLED 1
#define SCREEN_SECTION_STRUCT_SIZE 15

#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 24
#endif

#ifndef FRAME_LINES
	#define FRAME_LINES SCREEN_TILES_V*TILE_HEIGHT
#endif

//Define the type of sound mixer compatible 
#if SOUND_MIXER == MIXER_TYPE_INLINE
	#error Invalid compilation option (-DSOUND_MIXER=1): Inline audio mixer not supported for video mode 2 
#endif

