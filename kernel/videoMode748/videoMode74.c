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

	}



	/* Callback invoked by UzeboxCore.Initialize() */
	void InitializeVideoMode()
	{
		m74_config = 0U;    /* Display disabled */
		m74_discol = 0U;    /* Color when disabled: Black */
		m74_paddr  = M74_PAL_OFF;     /* 16 color palette address */
#if (M74_ROWS_PTRE != 0)
		m74_rows   = M74_ROWS_OFF;    /* Row selector address */
#endif
#if (M74_RESET_ENABLE != 0)
		m74_reset  = 0U;              /* Reset starts off disabled (so logo may display) */
#endif
		m74_m4_bank = M74_M4_BASE / 65536U;
		m74_m4_addr = M74_M4_BASE % 65536U;
#if (M74_SPR_ENABLE != 0)
		m74_rtmax  = 32U;   /* Sprites: Allow 32 RAM tiles by default */
		m74_rtbase = 72U;   /* Sprites: In the range 0x0A00 - 0x0DFF */
#endif
	}



	/* Callback invoked during hsync */
	void VideoModeVsync(){
		ProcessFading();
	}
