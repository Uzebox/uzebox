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
	
	extern unsigned char vram[];  
		
	struct SpriteStruct
	{
		unsigned char x;
		unsigned char y;
		unsigned char tileIndex;
	};

	/*
	 *	unsigned char scrollX: x displacement
	 *	unsigned char scrollY: y displacement
	 *	unsigned char height: section height in scanlines
	 *	unsigned char *vramBaseAdress: location in vram where this section starts rendering
	 *	unsigned char *tileTableAdress: tile set to use when rendering this section
	 *	unsigned char wrapLine: Define at what Y scroll within that section Y-wrap will happen (Normally Y-Wrap happens at 0xff). So
	 *                        instead of wrapping each 32 tiles you can wrap each 12 or 12 tiles+ 3 lines, etc.
	 *  					  IMPORTANT: insure scrollY is always < wrapLine or the screen will get trashed (perform Y scroll clipping).
	 *	unsigned char flags: 1=sprites will be drawn on top of this section. 0=sprites will be under.
	 */
	struct ScreenSectionStruct
	{
		//user defined
		unsigned char scrollX;
		unsigned char scrollY;
		unsigned char height;
		unsigned char *vramBaseAdress;
		const char *tileTableAdress;
		unsigned char wrapLine;
		unsigned char flags;

		//calculated (don't write to)
		unsigned char scrollXcoarse;
		unsigned char scrollXfine;		
		unsigned char *vramRenderAdress;
		unsigned char *vramWrapAdress;
	};

	
	extern struct SpriteStruct sprites[];
	extern struct ScreenSectionStruct screenSections[];
