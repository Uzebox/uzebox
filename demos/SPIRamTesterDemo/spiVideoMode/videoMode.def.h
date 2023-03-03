/*
 *  Uzebox(tm) Video Mode 8
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
 * This file contains global defines for video mode 8 (for asm & C)
 *
 * ===============================================================================
 */
#pragma once
#define VMODE_ASM_SOURCE QUOTE(VIDEO_MODE_PATH/videoModeCore.s)
#define VMODE_C_PROTOTYPES QUOTE(VIDEO_MODE_PATH/videoMode.h)
#define VMODE_FUNC spi_video_mode

#define SCREEN_WIDTH 360
#define SCREEN_HEIGHT 224

#define VRAM_SIZE (SCREEN_WIDTH*SCREEN_HEIGHT)/8

#define VRAM_ADDR_SIZE 1
#define VRAM_PTR_TYPE char
#define VRAM_TILES_H 28	

#define FIRST_RENDER_LINE 20
#define FRAME_LINES SCREEN_HEIGHT

