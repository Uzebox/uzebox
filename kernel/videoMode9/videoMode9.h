/*
 *  Uzebox(tm) Video Mode 3
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
 * Function prototypes for video mode 3
 * ===============================================================================
 */
#pragma once

#include <avr/io.h>

extern unsigned char vram[];  
extern u8 backgroundColor[];
extern u8 GetTile(u8 x,u8 y);

extern void MoveCursor(u8 x,u8 y);
extern void SetCursorVisible(bool visible);
extern void SetCursorParams(u8 tileIndex,u8 blinkSpeed);




