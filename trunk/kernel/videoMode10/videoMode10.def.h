/*
 *  Uzebox(tm) Video Mode 10 defines
 *  Copyright (C) 2011  Alec Bourque, 2012 Martin Sustek
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
 * This file contains global defines for video mode 10
 * It's mode 10 hacked to have 16x12 tiles (each tile is 12x16 pixel)
 *
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE "videoMode10/videoMode10.s"
#define VMODE_C_SOURCE "videoMode10/videoMode10.c"
#define VMODE_C_PROTOTYPES "videoMode10/videoMode10.h"
#define VMODE_FUNC sub_video_mode5

#define TILE_HEIGHT 16
#define TILE_WIDTH 12
#define VRAM_TILES_H 16 

#ifndef VRAM_TILES_V
	#define VRAM_TILES_V 12
#endif

#define SCREEN_TILES_H VRAM_TILES_H
#define SCREEN_TILES_V VRAM_TILES_V

#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 35
#endif

#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V
#define VRAM_ADDR_SIZE 1 //in bytes
#define VRAM_PTR_TYPE char
#define SPRITES_ENABLED 0

#ifndef FRAME_LINES
	#define FRAME_LINES SCREEN_TILES_V*TILE_HEIGHT
#endif

