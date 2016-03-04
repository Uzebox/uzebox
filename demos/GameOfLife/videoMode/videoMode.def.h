/*
 *  Uzebox(tm) Video Mode for Game of Life
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
 * This file contains global defines for this custom video mode (for asm & C)
 *
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE QUOTE(VIDEO_MODE_PATH/videoMode.s)
//#define VMODE_C_SOURCE QUOTE(VIDEO_MODE_PATH/videoMode.c)
#define VMODE_C_PROTOTYPES QUOTE(VIDEO_MODE_PATH/videoMode.h)
#define VMODE_FUNC sub_video_mode

#define TILE_HEIGHT 8
#define TILE_WIDTH 6
#define VRAM_TILES_H 40
#define VRAM_TILES_V 28
#define SCREEN_TILES_H 40
#define SCREEN_TILES_V 28


#define SCREEN_WIDTH 88
#define SCREEN_HEIGHT 56 //74

#define PAGE_SIZE (SCREEN_WIDTH/8)*SCREEN_HEIGHT
#define VRAM_SIZE PAGE_SIZE*2
#define VRAM_ADDR_SIZE 1
#define VRAM_PTR_TYPE u8
#define TEXT_VRAM_SIZE SCREEN_TILES_H*SCREEN_TILES_V

#define FIRST_RENDER_LINE 20
#define FRAME_LINES 224

