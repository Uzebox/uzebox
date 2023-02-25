/*
 *  Uzebox Kernel - Mode 80
 *  Copyright (C) 2019 Sandor Zsuga (Jubatian)
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
 * Function prototypes for video mode 80
 * ===========================================================================
 */

#pragma once

#include <avr/io.h>

/* Display list entry structure */

typedef struct{
 unsigned char  vramrow; /* VRAM row to begin at */
 unsigned char  tilerow; /* Tile row (within VRAM row) to begin at */
 unsigned char  bgc;     /* Background color */
 unsigned char  fgc;     /* Foreground color */
 unsigned char  next;    /* Next scanline to match, Unreachable: End */
}m80_dlist_tdef;

/* Provided by VideoMode80.s */

extern unsigned char    vram[];
extern unsigned char*   m80_bgclist;
extern unsigned char*   m80_fgclist;
extern m80_dlist_tdef*  m80_dlist;
extern unsigned const char* m80_rompal;
extern unsigned char*   m80_rampal;

/* Supplementary functions to complement the Uzebox kernel's set */

unsigned int  GetTile(char x, char y);
unsigned char GetFont(char x, char y);
