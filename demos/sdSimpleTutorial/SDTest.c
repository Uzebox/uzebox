/*
 *  sd simple tutorial
 *  cunning fellow
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

#include <ctype.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "data/tiles.inc"
#include "data/fonts6x8.inc"
#include <sdSimple.h>


#define COORD(X, Y) (X + (Y*40))

const char  fileName[] PROGMEM = "HELLWRLDTXT";

int main(){


SetTileTable(tiles);			//Set the tileset to use (set this first)
SetFontTilesIndex(TILES_SIZE);	//Set the tile number in the tilset that contains the first font
ClearVram();					//Clear the screen (fills the vram with tile zero)

long sectorStart;

sdCardInitNoBuffer();
sectorStart = sdFindFileFirstSectorFlash(fileName);

if(sectorStart == 0){
	Print(0,0,PSTR("FILE HELLWRLD.TXT NOT FOUND ON SD CARD"));
} else {

    sdCardCueSectorAddress(sectorStart);


    /*

    HELLO WORLD FROM THE SD CARD
    VERTICAL TEXT
    DOUBLE SPACES
    DOUBLE LINE SPACING DOUBLE LINE SPACING  DOUBLE LINE SPACING DOUBLE LINE SPACING
    RUN2SPANXFOR16BITINDEX

	*/

    // sdDirectReadSimple(uint8_t *dest, uint16_t count);
    // sdDirectRead(uint8_t *dest, uint16_t count, uint8_t span, uint8_t run);
    //
    //
    // HELLO WORLD FROM THE SD CARD
    // Read 28 bytes of test straight to VRAM
    // sdDirectReadSimple(address, length) == sdDriectReadSimple(address, length, 0, 0)
    // That is SPAN = 0 RUN = 0

    sdDirectReadSimple(&vram[COORD(0,0)], 28);    // HELLO WORLD
    sdCardSkipBytes(2);                           // Skip the CR/LF

    // VERTICAL TEXT
    // SPAN = 39
    // RUN = 1
    //
    // Every 1 (RUN) byte skip 39 (SPAN) bytes
    //
    // Screen width = 40 so 39 is screen width - 1

    sdDirectRead(&vram[COORD(0,2)], 13, 39, 1);   // VERTICAL TEXT
    sdCardSkipBytes(2);                           // Skip the CR/LF

    // DOUBLE SPACES
    // SPAN = 1
    // RUN = 1
    //
    // Every 1 (RUN) byte skip 1 (SPAN) bytes

    sdDirectRead(&vram[COORD(3,2)], 13, 1, 1);    // DOUBLE SPACES
    sdCardSkipBytes(2);                           // Skip the CR/LF

    // DOUBLE SPACES
    // SPAN = 39
    // RUN = 80
    //
    // Every 40 (RUN) bytes skip 39 (SPAN) bytes
    // 40 is one whole line of text
    // 39 is (40-1)

    sdDirectRead(&vram[COORD(0,17)], 80, 39, 40); // DOUBLE LINE SPACING
    sdCardSkipBytes(2);                           // Skip the CR/LF

    // 16 bit modes
    // SPAN = 38
    // RUN = 2
    //
    // Every 2 (RUN) bytes skip 38 (SPAN) bytes
    // 2 bytes = 16 bits.  So modes with a 16 bit tile index could use this to fill in a column
    // 38 is (40-2)

    sdDirectRead(&vram[COORD(20,4)], 22, 38, 2);  // 16 BIT MODES
}

while(1);

}
