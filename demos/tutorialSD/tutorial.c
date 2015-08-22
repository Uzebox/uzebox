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



#include "data/tiles.inc"
#include "data/fonts6x8.inc"


#define COORD(X, Y) (X + (Y*40))

const char  fileName[] PROGMEM = "HELLWRLDTXT";

int main(){


SetTileTable(tiles);			//Set the tileset to use (set this first)
SetFontTilesIndex(TILES_SIZE);	//Set the tile number in the tilset that contains the first font
ClearVram();					//Clear the screen (fills the vram with tile zero)

int sectorStart;

mmc_init_no_buffer();
sectorStart = mmcFindFileFirstSectorFlash(fileName);
mmc_cuesector(sectorStart);

/*
for(u8 x=6;x<34;x++){
	PrintChar(x,10,mmcGetChar());
}
*/



mmcDirectRead(&vram[COORD(0,0)], 28, 0, 1);    // HELLO WORLD

mmcDirectRead(&vram[COORD(0,2)], 13, 39, 1);   // VERTICAL TEXT

mmcDirectRead(&vram[COORD(3,2)], 13, 1, 1);    // DOUBLE SPACES

mmcDirectRead(&vram[COORD(0,17)], 80, 39, 40); // DOUBLE LINE SPACING

mmcDirectRead(&vram[COORD(20,4)], 22, 38, 2);  // 16 BIT MODES


while(1);

} 
