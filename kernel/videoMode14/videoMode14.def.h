/*
 *  Uzebox(tm) Video Mode 14
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
 * This file contains global defines for video mode 14
 *
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE "videoMode14/videoMode14.s"
#define VMODE_C_PROTOTYPES "videoMode14/videoMode14.h"
#define VMODE_FUNC sub_video_mode14


#define TILE_HEIGHT 8
#define TILE_WIDTH 8
#define TILE_BPP 2
#define TILE_MAX_NO 128
#define RAMTILE_SIZE ((TILE_HEIGHT * TILE_WIDTH * TILE_BPP * TILE_MAX_NO) / 8)

#define VRAM_TILES_H 32
#define VRAM_TILES_V 28		
#define SCREEN_TILES_H 32
#define SCREEN_TILES_V 28

#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 20	
#endif
#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V
#define VRAM_ADDR_SIZE 1 //in bytes


#ifndef FRAME_LINES
	#define FRAME_LINES SCREEN_TILES_V*TILE_HEIGHT
#endif

