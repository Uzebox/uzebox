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

	#include <stdbool.h>
	#include <avr/io.h>
	#include <stdlib.h>
	#include <avr/pgmspace.h>
	#include "uzebox.h"

	/* Callback invoked by UzeboxCore.Initialize() */
	void InitializeVideoMode(){

		unsigned char i;

		for (i = 0U; i < 16U; i++)
		{
			palette[i] = pgm_read_byte(&(m90_defpalette[i]));
		}

		m90_trows   = m90_deftilerows;
		m90_exvram  = &vram[0];
		m90_split   = 0U;
		m90_palrel1 = 0xFFU;
		m90_palrel2 = 0xFFU;
		m90_pal1    = &palette[0];
		m90_pal2    = &palette[0];

		SetFontTilesIndex(0);

	}

	/* Callback invoked during vsync */
	void VideoModeVsync(){
	}

	/* Callback invoked by UzeboxCore.Initialize() */
	void DisplayLogo(){
	}
