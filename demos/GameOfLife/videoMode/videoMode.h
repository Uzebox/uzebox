/*
 *  Uzebox(tm) Video Mode 8
 *  Copyright (C) 2009  Alec Bourque
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
 * This file contains function prototypes & exports for video mode 8
 *
 * ===============================================================================
 */
#pragma once

extern unsigned char vram[2][PAGE_SIZE];  
extern u8 vmode_page;
extern u8 vmode_grid_col;
extern u8 vmode_text_lines;
extern u8 vmode;
extern u8 cursor_x;
extern u8 cursor_y;
extern void PutPixel(unsigned char x, unsigned char y,unsigned char color,u8 page);
extern unsigned char GetPixel(unsigned char x, unsigned char y,u8 page);
extern unsigned char palette[];

extern void ClearTextVram();
