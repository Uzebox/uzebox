/*
 *  Uzebox quick and dirty tutorial
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
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "mmc.h"
#include "mmcextra.h"



//Import the appropriate font file
//based on the video mode's tile height
//and width. These values can be overriden
//in the makefile.
#if TILE_WIDTH == 6 && TILE_HEIGHT == 8
	#include "data/font-6x8-full.inc"

#elif TILE_WIDTH == 6 && TILE_HEIGHT == 12
	#include "data/font-6x12-full.inc"

#elif TILE_WIDTH == 8 && TILE_HEIGHT == 8
	#include "data/font-8x8-full.inc"
#else
	#error Unsupported tile size defined in makefile! For this demo only 6x8, 6x12 and 8x8 tile sizes are supported.
#endif


int main(){

int sectorStart;
sectorStart = findFileFirstSector("HELLWRLDTXT");
mmc_cuesector(sectorStart);



	//Set the font and tiles to use.
	//Always invoke before any ClearVram()
	SetFontTable(font);

	//Clear the screen (fills the vram with tile zero)
	ClearVram();


	for(u8 x=6;x<34;x++){
		PrintChar(x,10,mmcGetChar());
	}



	//Prints a string on the screen. Note that PSTR() is a macro 
	//that tells the compiler to store the string in flash.
	//14 is half the lenght of the "Hello world" string
	Print((SCREEN_TILES_H/2)-14, (SCREEN_TILES_V/2)-1, PSTR("Hello World From The Uzebox!"));

	while(1);

} 
