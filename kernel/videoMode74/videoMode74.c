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
		m74_enable = 0U;    /* Display disabled, SD load if compiled in, disabled */
		m74_bgcol  = 0U;    /* Background color: index zero */
		m74_rtmax  = 32U;   /* Allow 32 RAM tiles by default (should leave enough stack with default config) */
		m74_rtno   = 0U;    /* Currently allocated RAM tiles */
#if (M74_ROWS_PTRE != 0)
		m74_rows   = M74_ROWS_OFF;    /* Row selector address */
#endif
#if (M74_PAL_PTRE != 0)
		m74_pal    = M74_PAL_OFF;     /* 16 color palette address */
#endif
#if ((M74_COL0_PTRE != 0) && (M74_COL0_RELOAD != 0))
		m74_col0   = M74_COL0_OFF;    /* Color 0 reload address */
#endif
#if ((M74_M3_PTRE != 0) && (M74_M3_ENABLE != 0))
		m74_mcadd  = M74_M3_OFF;      /* Multicolor framebuffer address */
#endif
#if (M74_ROMMASK_PTRE != 0)
		m74_romma  = M74_ROMMASK_OFF; /* ROM mask pool address */
#endif
#if (M74_RAMMASK_PTRE != 0)
		m74_ramma  = M74_RAMMASK_OFF; /* RAM mask pool address */
#endif
#if (M74_RTLIST_PTRE != 0)
		m74_rtlist = M74_RTLIST_OFF;  /* RAM tile allocation workspace address */
#endif
	}

	/* Callback invoked during hsync */
	void VideoModeVsync(){
		ProcessFading();
	}
