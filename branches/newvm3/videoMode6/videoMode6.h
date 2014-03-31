/*
 *  Uzebox(tm) Video Mode 2
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
 * This file contains function prototypes and exports for mode 2
 *
 * ===============================================================================
 */

#pragma once
	
extern u8 vram[];  
extern u8 ramTiles[];
extern void SetBackgroundColor(u8 color);
extern void SetForegroundColor(u8 color);
extern void ClearBuffer();
extern void Line2(u8 x1,u8 y1,u8 x2,u8 y2);
extern void SetPixel2(u8 x,u8 y);
extern void SetRamTileBaseDrawingIndex(u8 tileIndex);
		
extern void SetHsyncCallback(HsyncCallBackFunc);
