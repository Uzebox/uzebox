/*
 *  Uzebox Kernel - Mode 41
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

/* Callback invoked by UzeboxCore.Initialize() */
void InitializeVideoMode(){

	u16 i16;

	palette[0] = 0x00U;
	palette[1] = 0xFFU;
	palette[2] = 0x00U;
	palette[3] = 0x00U;

	SetBorderColor(0);
	SetBackgroundPerLine(0);

	ClearVram();

	for (i16 = 0U; i16 < VRAM_SIZE; i16 ++){
		aram[i16] = 0xFFU;
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
