/*
 *  Uzebox Kernel - Mode 42
 *  Copyright (C) 2018 Sandor Zsuga (Jubatian)
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

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "uzebox.h"
#if (M40_C64_GRAPHICS != 0)
#include "../videoMode40/data/c64_graphics.inc"
#endif
#if (M40_C64_ALPHA != 0)
#include "../videoMode40/data/c64_alpha.inc"
#endif
#if (M40_C64_MIXED != 0)
#include "../videoMode40/data/c64_mixed.inc"
#endif
#if (M40_IBM_ASCII != 0)
#include "../videoMode40/data/ibm_ascii.inc"
#endif
#if (M40_MATTEL != 0)
#include "../videoMode40/data/mattel.inc"
#endif


/* Default palette */
static const u8 def_palette[20] PROGMEM = {
 0x00U, 0xFFU, 0x52U, 0xADU,
 0x00U, 0x80U, 0x28U, 0xA8U,
 0x05U, 0x85U, 0x15U, 0xADU,
 0x52U, 0xD2U, 0x7AU, 0xFAU,
 0x57U, 0xD7U, 0x7FU, 0xFFU
};


/* Callback invoked by UzeboxCore.Initialize() */
void InitializeVideoMode(){

	u16 i16;
	u8  i8;

	for (i8 = 0U; i8 < 20U; i8 ++){
		palette[i8] = pgm_read_byte(&def_palette[i8]);
	}

	SetBorderColor(0);

	ClearVram();

	for (i16 = 0U; i16 < VRAM_SIZE; i16 ++){
		aram[i16] = 0xF0U;
	}

	SetFontTilesIndex(0x20U);

#if (M40_MATTEL != 0)
	SetTileTable(m40_mattel);
#endif
#if (M40_C64_GRAPHICS != 0)
	SetTileTable(m40_c64_graphics);
#endif
#if (M40_C64_ALPHA != 0)
	SetTileTable(m40_c64_alpha);
#endif
#if (M40_C64_MIXED != 0)
	SetTileTable(m40_c64_mixed);
#endif
#if (M40_IBM_ASCII != 0)
	SetTileTable(m40_ibm_ascii);
#endif

}

/* Callback invoked during vsync */
void VideoModeVsync(){
}

/* Callback invoked by UzeboxCore.Initialize() */
void DisplayLogo(){
}
