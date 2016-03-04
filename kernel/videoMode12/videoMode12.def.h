/*
 *  Uzebox(tm) Video Mode 12
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
 * This file contains global defines for video mode 12 (for asm & C)
 *
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE "videoMode12/videoMode12.s"
#define VMODE_C_PROTOTYPES "videoMode12/videoMode12.h"
#define VMODE_FUNC sub_video_mode12

#define VRAM_ADDR_SIZE 1 //in bytes
#define VRAM_PTR_TYPE char

#define FIRST_RENDER_LINE 36
#define FRAME_LINES 192

