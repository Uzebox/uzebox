/*
 *  Uzebox Kernel - Video Mode 74
 *  Copyright (C) 2015 Sandor Zsuga (Jubatian)
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
	#include "intro.h"



	/* Callback invoked by UzeboxCore.Initialize() */
	void DisplayLogo(){

		#if INTRO_LOGO !=0

			/* For now there is no logo image */

		#endif
	}


	/* Callback invoked by UzeboxCore.Initialize() */
	void InitializeVideoMode()
	{
		m74_palbuf = 0x0FU; /* Palette buffer just below stack, should be OK for most users */
		m74_ldsl   = 0xFFU; /* Turn off RAM clear or SPI load (never reached line) */
		m74_totc   = 0U;    /* Same, nothing to clear / load */
		m74_umod   = 0U;    /* No user video mode, display disabled */
	}

	/* Callback invoked during hsync */
	void VideoModeVsync(){
		ProcessFading();
	}
