/*
 *  SD Card - SPI RAM interface demo
 *  Copyright (C) 2017 Sandor Zsuga
 *
 *  Based on Alec Bourque's Quick and Dirty Uzebox tutorial
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


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <sdBase.h>
#include <spiram.h>


static char const filename[] PROGMEM = "SDSPIRAMUZE";


/* Import the appropriate font file based on the video mode's tile height
** and width. These values can be overridden in the makefile. */

#if   (TILE_WIDTH == 6) && (TILE_HEIGHT == 8)
	#include "data/font-6x8-full.inc"
#elif (TILE_WIDTH == 6) && (TILE_HEIGHT == 12)
	#include "data/font-6x12-full.inc"
#elif (TILE_WIDTH == 8) && (TILE_HEIGHT == 8)
	#include "data/font-8x8-full.inc"
#else
	#error Unsupported tile size defined in makefile! For this demo only 6x8, 6x12 and 8x8 tile sizes are supported.
#endif


int main(){

	u32 sec;
	u8  buf[8];
	u8  rby;
	u8  pos;

	/* Init the SD Card and the SPI RAM. Always do the inits in this order
	** since the SD Card on a normal Uzebox (using the bootloader) would
	** have the card in an undefined state, usually interfering with the
	** SPI RAM without reinit. Note that if an SD Card is not plugged in
	** (you are programming this directly on the ATMega644), the
	** sdCardInitNoBuffer() routine will have a hefty 15 sec timeout. */

	sdCardInitNoBuffer();
	SpiRamInit();

	/* Prepare screen */

	SetFontTable(font);
	ClearVram();

	/* Get .uze file's start sector */

	sec = sdCardFindFileFirstSectorFlash(&filename[0]);

	/* Load stuff from the SD Card (the game name from the .uze file) and
	** display it on tile row 4. */

	sdCardCueSectorAddress(sec);
	for(pos = 0U; pos < 14U; pos ++){ sdCardGetChar(); } /* Skip to game name */
	pos = 2U;
	do{
		rby = sdCardGetChar();
		if (rby == 0U){ break; } /* End of string */
		PrintChar(pos, 4U, rby);
		pos ++;
	}while (pos < SCREEN_TILES_H);
	sdCardStopTransmission();

	/* Mess around a little with the SPI RAM generating 18 bytes of
	** content on its beginning */

	SpiRamWriteS8(0U, 0x0000U, 'T');
	SpiRamWriteS8(0U, 0x0001U, 'e');
	SpiRamWriteS8(0U, 0x0002U, 's');
	SpiRamWriteS8(0U, 0x0003U, 't');

	SpiRamWriteU16(0U, 0x0004U, SpiRamReadU16(0U, 0x0001U));
	SpiRamWriteU32(0U, 0x0006U, SpiRamReadU32(0U, 0x0002U));

	SpiRamReadInto(0U, 0x0002U, &buf[0], 8U);
	SpiRamWriteFrom(0U, 0x000AU, &buf[0], 8U);

	/* Print data from SPI RAM by sequential reading */

	SpiRamSeqReadStart(0U, 0x0000U);
	for(pos = 2U; pos < 20U; pos ++){
		PrintChar(pos, 2U, SpiRamSeqReadS8());
	}
	SpiRamSeqReadEnd();

	/* Access the SD Card again, printing the same stuff (game name) on
	** tile row 6, just to demonstrate it is possible to access it even
	** after working with the SPI RAM. */

	sdCardCueSectorAddress(sec);
	for(pos = 0U; pos < 14U; pos ++){ sdCardGetChar(); } /* Skip to game name */
	pos = 2U;
	do{
		rby = sdCardGetChar();
		if (rby == 0U){ break; } /* End of string */
		PrintChar(pos, 6U, rby);
		pos ++;
	}while (pos < SCREEN_TILES_H);
	sdCardStopTransmission();

	/* Print columns and rows on screen border */

	for(pos = 0U; pos < SCREEN_TILES_H; pos++){
		PrintChar(pos, 0U, '0' + (pos % 10U));
	}
	for(pos = 0U; pos < SCREEN_TILES_V; pos++){
		PrintChar(0U, pos, '0' + (pos % 10U));
	}

	/* Prints a string on the screen. Note that PSTR() is a macro that
	** tells the compiler to store the string in flash. 14 is half the
	** lenght of the "Hello world" string. */

	Print( (SCREEN_TILES_H / 2U) - 14U,
	       (SCREEN_TILES_V / 2U) - 1U,
	       PSTR("Hello World From The Uzebox!") );

	while(1);

}
