/*
 *  Uzebox Kernel - Mode 92
 *  Copyright (C) 2016 Alec Bourque,
 *                     Sandor Zsuga (Jubatian)
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
 * Function prototypes for video mode 92
 * ===========================================================================
 */

#pragma once

#include <avr/io.h>

/* Provided by VideoMode92.s */

extern unsigned char  vram[];
extern unsigned char  palette[];
extern const unsigned int*  m90_trows;
extern const unsigned char* m90_exvram;
extern unsigned char  m90_split;
extern unsigned char  m90_palrel1;
extern unsigned char* m90_pal1;
extern unsigned char  m90_palrel2;
extern unsigned char* m90_pal2;

/* Supplementary functions to complement the Uzebox kernel's set */

unsigned int  GetTile(char x, char y);
unsigned char GetFont(char x, char y);

/* Provided by the user tileset */

extern const unsigned char m90_defpalette[];
extern const unsigned int m90_deftilerows[];
