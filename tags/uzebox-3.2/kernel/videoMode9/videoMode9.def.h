/*
 *  Uzebox(tm) Video Mode 9
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
 * Global defines for video mode 9
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE "videoMode9/videoMode9core.s"
#define VMODE_C_SOURCE "videoMode9/videoMode9.c"
#define VMODE_C_PROTOTYPES "videoMode9/videoMode9.h"
#define VMODE_FUNC sub_video_mode9


#ifndef TILE_HEIGHT
	#define TILE_HEIGHT 8
#endif

#define TILE_WIDTH 6
#define VRAM_TILES_H 60
#define SCREEN_TILES_H 60


#ifndef SCREEN_TILES_V
	#define SCREEN_TILES_V 28
#endif

#ifndef VRAM_TILES_V
	#define VRAM_TILES_V SCREEN_TILES_V		
#endif


#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 20	
#endif

#ifndef FRAME_LINES
	#define FRAME_LINES SCREEN_TILES_V*TILE_HEIGHT
#endif

#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V	
#define VRAM_ADDR_SIZE 1 //in bytes

#define FONT_TILE_WIDTH 21 //in words
#define FONT_TILE_SIZE FONT_TILE_WIDTH*TILE_HEIGHT //Size in words: 27 instructions * 8 rows
