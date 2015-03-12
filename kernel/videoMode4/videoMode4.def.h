/*
 *  Uzebox(tm) Video Mode 4
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
 * This file contains global defines for video mode 4
 *
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE "videoMode4/videoMode4.s"
#define VMODE_C_PROTOTYPES "videoMode4/videoMode4.h"
#define VMODE_FUNC sub_video_mode4

#define TILE_HEIGHT	16
#define TILE_WIDTH	16
#define VRAM_TILES_H 32
#ifndef VRAM_TILES_V
	#define VRAM_TILES_V 32
#endif
#define SCREEN_TILES_H 18
#define SCREEN_TILES_V 12
#define FIRST_RENDER_LINE	24
#define VRAM_SIZE	(VRAM_TILES_H*VRAM_TILES_V)
#define VRAM_ADDR_SIZE	1
#define VRAM_PTR_TYPE char
#define VRAM_H				(SCREEN_TILES_V*TILE_HEIGHT)

#ifndef FRAME_LINES
	#define FRAME_LINES SCREEN_TILES_V*TILE_HEIGHT
#endif
