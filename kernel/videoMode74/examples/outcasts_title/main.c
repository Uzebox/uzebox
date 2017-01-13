/*
 *  Mode 74 tests
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
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "tiles.h"



/* Background color index for fading */
#define BGCOL 0x0AU



/* Row selectors */
static unsigned char rowsel[] = {
         0U,   0U, /* First line: Start at scanline 0 */
 255U              /* End of list */
};



/* Tile map indices */
static const unsigned char tidx[] PROGMEM = {
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
  22U, 0x04U, /* Top frame line */

/* Multicolor region (9 tile rows), each tile row is 32 + 2 + 5 bytes to cover
** 22 tiles width (so 6 tiles extra to the 16 tile wide MC area). These are on
** the top of RAM, just below the palette buffer. */

 (0x0F00U - (9U * 39U)) & 0xFFU, (0x0F00U - (9U * 39U)) >> 8,
 (0x0F00U - (8U * 39U)) & 0xFFU, (0x0F00U - (8U * 39U)) >> 8,
 (0x0F00U - (7U * 39U)) & 0xFFU, (0x0F00U - (7U * 39U)) >> 8,
 (0x0F00U - (6U * 39U)) & 0xFFU, (0x0F00U - (6U * 39U)) >> 8,
 (0x0F00U - (5U * 39U)) & 0xFFU, (0x0F00U - (5U * 39U)) >> 8,
 (0x0F00U - (4U * 39U)) & 0xFFU, (0x0F00U - (4U * 39U)) >> 8,
 (0x0F00U - (3U * 39U)) & 0xFFU, (0x0F00U - (3U * 39U)) >> 8,
 (0x0F00U - (2U * 39U)) & 0xFFU, (0x0F00U - (2U * 39U)) >> 8,
 (0x0F00U - (1U * 39U)) & 0xFFU, (0x0F00U - (1U * 39U)) >> 8,

  44U, 0x04U, /* Bottom frame line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */

  66U, 0x04U, /* "New game" line */
   0U, 0x04U, /* Empty line */
  88U, 0x04U, /* "Continue" line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */

   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U, /* Empty line */
   0U, 0x04U  /* Empty line */
};



static const unsigned char line_content[] PROGMEM = {
/* Empty line */
 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
/* Top border line */
 0x80U, 0x80U, 0x81U, 0x87U, 0x87U, 0x87U, 0x87U, 0x87U, 0x87U, 0x87U, 0x87U,
 0x87U, 0x87U, 0x87U, 0x87U, 0x87U, 0x87U, 0x87U, 0x87U, 0x82U, 0x80U, 0x80U,
/* Bottom border line */
 0x80U, 0x80U, 0x83U, 0x88U, 0x88U, 0x88U, 0x88U, 0x88U, 0x88U, 0x88U, 0x88U,
 0x88U, 0x88U, 0x88U, 0x88U, 0x88U, 0x88U, 0x88U, 0x88U, 0x84U, 0x80U, 0x80U,
/* "New game" text line (now with selectors, just a demo screen) */
 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x20U, 0x20U, 0x20U, 0x10U, 'N',   'e',
 'w',   ' ',   'G',   'a',   'm',   'e',   0x11U, 0x20U, 0x20U, 0x20U, 0x80U,
/* "Continue" text line */
 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x20U, 0x20U, 0x20U, 0x20U, 'C',   'o',
 'n',   't',   'i',   'n',   'u',   'e',   0x20U, 0x20U, 0x20U, 0x20U, 0x80U,
/* Padding empty line, text lines (6px tiles) use more tiles... */
 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U
};



int main(){

	/* Ensures that the font tile map is linked */
	volatile unsigned char dummy_sec = res_font[0];

	unsigned char  i;
	unsigned char  j;
	unsigned int   i16;
	unsigned char  mlct;
	unsigned char  r;
	unsigned char  g;
	unsigned char  b;
	unsigned char  rb;
	unsigned char  gb;
	unsigned char  bb;
	unsigned char* pal  = (unsigned char*)(M74_PAL_OFF);
	unsigned char* mcdt = (unsigned char*)(0x0F00U - (9U * 39U) - 2304U); /* Multicolor image data */
	unsigned char* mcvr = (unsigned char*)(0x0F00U - (9U * 39U)); /* Multicolor tiles */
	unsigned char* vram = (unsigned char*)(0x0400U); /* 161 bytes are available here */


	/* Set row selector */

	m74_rows = (unsigned int)(&rowsel[0]);

	/* Set tile row descriptors */

	m74_tdesc = (unsigned int)(&res_screen_00[0]);
	m74_tidx  = (unsigned int)(&tidx[0]);


	/* Load multicolor image's palette */

	for (i = 0U; i < 16U; i++)
	{
		pal[i] = pgm_read_byte(&(res_pal_00[i]));
	}

	/* Load multicolor image's data. */

	for (i16 = 0U; i16 < 2304U; i16++)
	{
		mcdt[i16] = pgm_read_byte(&(imgdata[i16]));
	}

	/* Load multicolor image's attributes */

	for (i = 0U; i < 9U; i++)
	{
		i16  = i;
		i16 *= 39U;
		mcvr[i16 +  0U] = 0x80U; /* Column  0: 8px wide 1bpp tile */
		mcvr[i16 +  1U] = 0x80U; /* Column  1: 8px wide 1bpp tile */
		mcvr[i16 +  2U] = 0xC5U; /* Column  2: 8px wide 1bpp starter tile */
		mcvr[i16 +  3U] = 0x10U; /* 16 MC tiles follow */
		mcvr[i16 + 36U] = 0x86U; /* Column 19: 8px wide 1bpp tile */
		mcvr[i16 + 37U] = 0x80U; /* Column 20: 8px wide 1bpp tile */
		mcvr[i16 + 38U] = 0x80U; /* Column 21: 8px wide 1bpp tile */

		for (j = 0U; j < 32U; j++)
		{
			mcvr[i16 + 4U + j] =
			    pgm_read_byte(&imgattr[(unsigned int)(i) * 32U + j]);
		}

	}


	/* Fill up vram */

	for (i = 0U; i < 22U * 6U; i++)
	{
		vram[i] = pgm_read_byte(&(line_content[i]));
	}



	/* Set main configuration flags & Enable */

	m74_config =
	    M74_CFG_RAM_PALETTE |
	    M74_CFG_ENABLE;



	/* Main loop */

	mlct = 0U;
	while(1)
	{

		/* Load multicolor image's palette */

		for (i = 0U; i < 16U; i++)
		{
			pal[i] = pgm_read_byte(&(res_pal_00[i]));
		}

		/* Do some palette hacks (fade in - out) */

		rb = (pal[BGCOL] >> 0) & 7U;
		gb = (pal[BGCOL] >> 3) & 7U;
		bb = (pal[BGCOL] >> 6) & 3U;

		for (i = 0U; i < 16U; i++)
		{
			r = (pal[i] >> 0) & 7U;
			g = (pal[i] >> 3) & 7U;
			b = (pal[i] >> 6) & 3U;

			if       (mlct  < 16U)
			{
				r = rb;
				g = gb;
				b = bb;
			}
			else if ((mlct == 16U) || (mlct == 0xFFU))
			{
				r = ((r * 1U) + (rb * 7U)) >> 3;
				g = ((g * 1U) + (gb * 7U)) >> 3;
				b = ((b * 1U) + (bb * 7U)) >> 3;
			}
			else if ((mlct == 17U) || (mlct == 0xFEU))
			{
				r = ((r * 2U) + (rb * 6U)) >> 3;
				g = ((g * 2U) + (gb * 6U)) >> 3;
				b = ((b * 2U) + (bb * 6U)) >> 3;
			}
			else if ((mlct == 18U) || (mlct == 0xFDU))
			{
				r = ((r * 3U) + (rb * 5U)) >> 3;
				g = ((g * 3U) + (gb * 5U)) >> 3;
				b = ((b * 3U) + (bb * 5U)) >> 3;
			}
			else if ((mlct == 19U) || (mlct == 0xFCU))
			{
				r = ((r * 4U) + (rb * 4U)) >> 3;
				g = ((g * 4U) + (gb * 4U)) >> 3;
				b = ((b * 4U) + (bb * 4U)) >> 3;
			}
			else if ((mlct == 20U) || (mlct == 0xFBU))
			{
				r = ((r * 5U) + (rb * 3U)) >> 3;
				g = ((g * 5U) + (gb * 3U)) >> 3;
				b = ((b * 5U) + (bb * 3U)) >> 3;
			}
			else if ((mlct == 21U) || (mlct == 0xFAU))
			{
				r = ((r * 6U) + (rb * 2U)) >> 3;
				g = ((g * 6U) + (gb * 2U)) >> 3;
				b = ((b * 6U) + (bb * 2U)) >> 3;
			}
			else if ((mlct == 22U) || (mlct == 0xF9U))
			{
				r = ((r * 7U) + (rb * 1U)) >> 3;
				g = ((g * 7U) + (gb * 1U)) >> 3;
				b = ((b * 7U) + (bb * 1U)) >> 3;
			}
			else
			{
				/* Original colors remain */
			}

			pal[i] = r + (g << 3) + (b << 6);
		}

		mlct ++;
		if (mlct == 0x20U)
		{
			mlct = 0xC0U; /* Make turnaround a bit faster */
		}
		WaitVsync(4);
	}

}
