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
	#if (INTRO_LOGO != 0)
		#include "videoMode74/uzeboxlogo.h"
	#endif



	#if (INTRO_LOGO != 0)

		/* For logo: Fill in normal image & palette */

		static void uzeboxlogo_1_load(){

			unsigned char  i;
			unsigned char* pal = (unsigned char*)(M74_PAL_OFF);

			for (i = 0U; i < 13U; i++)
			{
				M74_RamTileFillRom((unsigned int)(&uzeboxlogo_tiles_1[0]) + ((unsigned int)(i) * 32U), 0xC1U + i, 0U);
			}
			for (i = 0U; i < 16U; i++)
			{
				pal[i] = pgm_read_byte(&(uzeboxlogo_pal_1[i]));
			}

		}

		/* For logo: Fill in flash image & palette */

		static void uzeboxlogo_2_load(){

			unsigned char  i;
			unsigned char* pal = (unsigned char*)(M74_PAL_OFF);

			for (i = 0U; i < 13U; i++)
			{
				M74_RamTileFillRom((unsigned int)(&uzeboxlogo_tiles_2[0]) + ((unsigned int)(i) * 32U), 0xC1U + i, 0U);
			}
			for (i = 0U; i < 16U; i++)
			{
				pal[i] = pgm_read_byte(&(uzeboxlogo_pal_2[i]));
			}

		}

	#endif



	/* Callback invoked by UzeboxCore.Initialize() */
	void DisplayLogo(){

		#if (INTRO_LOGO != 0)

			unsigned char* wrk = (unsigned char*)(M74_LOGO_WORK);
			unsigned char* rsl;
			unsigned char  i;

			/* Preparations (doesn't enable display yet) */

			#if (M74_ROWS_OFF != 0)
				rsl = (unsigned char*)(m74_rows);
				wrk[70] = rsl[0]; /* Save initializer of row selector */
				wrk[71] = rsl[1];
				wrk[72] = rsl[2];
			#else
				m74_rows = (unsigned int)(&(wrk[70]));
				rsl = (unsigned char*)(m74_rows);
			#endif
			rsl[0] = 0U;
			rsl[1] = 0U;
			rsl[2] = 255U; /* Row selector setup to simply use the logo rows */
			m74_tdesc = (unsigned int)(&(wrk[0])); /* Tile descriptors (4 rows only) */
			m74_tidx  = (unsigned int)(&(wrk[8])); /* Tile indices (4 rows only) */
			for (i = 0U; i < 70U; i++)
			{
				wrk[i] = pgm_read_byte(&(uzeboxlogo_vram[i]));
			}
			for (i = 0U; i < 5U; i++)
			{
				M74_RamTileFillRom((unsigned int)(&uzeboxlogo_text[0]) + (i * 32U), 0xCEU + i, 0U);
			}
			M74_RamTileClear(0xC0U, 0U);
			SetRenderingParameters(110U, 32U);

			/* Logo display sequence starts */

			InitMusicPlayer(logoInitPatches);
			WaitVsync(15U);

			#if (INTRO_LOGO == 1)
				TriggerFx(0U, 0xFFU, true);
			#endif

			/* Fill in normal logo image & palette */

			uzeboxlogo_1_load();
			m74_config =
			    M74_CFG_RAM_TDESC |
			    M74_CFG_RAM_TIDX |
			    M74_CFG_RAM_PALETTE |
			    M74_CFG_ENABLE;
			WaitVsync(3U);

			/* Fill in flash logo image & palette */

			uzeboxlogo_2_load();
			WaitVsync(2U);

			/* Fill in normal logo image & palette (again) */

			uzeboxlogo_1_load();

			#if (INTRO_LOGO == 2)
				SetMasterVolume(0xC0U);
				TriggerNote(3U, 0U, 16U, 0xFFU);
			#endif

			WaitVsync(65U);

			/* Disable display, so logo is OFF */

			m74_config = 0U;
			WaitVsync(20U);

			/* Restore row selector contents if necessary, and
			** also the default rendering parameters */

			#if (M74_ROWS_OFF != 0)
				rsl[0] = wrk[70];
				rsl[1] = wrk[71];
				rsl[2] = wrk[72];
			#endif

			SetRenderingParameters(FIRST_RENDER_LINE, FRAME_LINES);

		#endif
	}



	/* Callback invoked by UzeboxCore.Initialize() */
	void InitializeVideoMode()
	{
		m74_config = 0U;    /* Display disabled */
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
#if (M74_RESET_ENABLE != 0)
		m74_reset  = 0U;              /* Reset starts off disabled (so logo may display) */
#endif
#if (M74_SD_ENABLE != 0)
		m74_sdsec  = 0U;              /* Sector part of SD offset initially zero */
#endif
	}



	/* Callback invoked during hsync */
	void VideoModeVsync(){
		ProcessFading();
	}
