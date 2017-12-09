/*
 *  Uzebox Kernel - Mode 40
 *  Copyright (C) 2017 Sandor Zsuga (Jubatian)
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
 * ===========================================================================
 * Function prototypes for video mode 40
 * ===========================================================================
 */

#pragma once

#include <avr/io.h>

/* Provided by VideoMode40.s */

extern unsigned char  vram[];
extern unsigned char  aram[];
extern unsigned char  palette[];

/* Supplementary functions to complement the Uzebox kernel's set */

unsigned int  GetTile(char x, char y);
unsigned char GetFont(char x, char y);

/* Custom functions */

void SetTileTableRow(char const* data, unsigned char row);
void SetBorderColor(unsigned char col);
void PutPixel(unsigned char x, unsigned char y, unsigned char col);
unsigned char GetPixel(unsigned char x, unsigned char y);

/* Special tile rows */

#define M40_TILEROW_3BPP ((void*)(0xFE00U))
#define M40_TILEROW_1BPP ((void*)(0xFF00U))

/* Optional charsets */

#if (M40_C64_GRAPHICS != 0)
extern const char m40_c64_graphics[] PROGMEM;
#endif
#if (M40_C64_ALPHA != 0)
extern const char m40_c64_alpha[] PROGMEM;
#endif
#if (M40_C64_MIXED != 0)
extern const char m40_c64_mixed[] PROGMEM;
#endif
#if (M40_IBM_ASCII != 0)
extern const char m40_ibm_ascii[] PROGMEM;
#endif
#if (M40_MATTEL != 0)
extern const char m40_mattel[] PROGMEM;
#endif
