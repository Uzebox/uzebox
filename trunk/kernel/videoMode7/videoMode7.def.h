/*
 *  Uzebox(tm) Video Mode 7
 *  Copyright (C) 2009 Alec Bourque
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
 * This file contains global defines for video mode 7 (for asm & C)
 *
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE "videoMode7/videoMode7.s"
#define VMODE_C_PROTOTYPES "videoMode7/videoMode7.h"
#define VMODE_FUNC sub_video_mode7

#define TILE_HEIGHT 8
#define TILE_WIDTH 8	
#define VRAM_TILES_H 40 
#define VRAM_TILES_V 28
#define SCREEN_TILES_H 40
#define SCREEN_TILES_V 28 
#define FIRST_RENDER_LINE 20
#define VRAM_SIZE 1 
#define VRAM_PTR_TYPE char
#define VRAM_ADDR_SIZE 1
#define FRAME_LINES 228

//Define the type of sound mixer compatible 
#if SOUND_MIXER == MIXER_TYPE_INLINE
	#error Invalid compilation option (-DSOUND_MIXER=1): Inline audio mixer not supported for video mode 7
#endif
