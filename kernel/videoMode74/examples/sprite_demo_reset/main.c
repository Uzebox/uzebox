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



/* Row selectors */
static unsigned char rowsel[] = {
         9U,   0U, /* First line: Start at scanline 9 */
 255U              /* End of list */
};



/* Sine table */
static const unsigned char sine[] PROGMEM = {
 0x81U, 0x84U, 0x87U, 0x8AU, 0x8EU, 0x91U, 0x94U, 0x97U,
 0x9AU, 0x9DU, 0xA0U, 0xA3U, 0xA6U, 0xA9U, 0xACU, 0xAFU,
 0xB2U, 0xB5U, 0xB7U, 0xBAU, 0xBDU, 0xC0U, 0xC2U, 0xC5U,
 0xC8U, 0xCAU, 0xCDU, 0xCFU, 0xD2U, 0xD4U, 0xD6U, 0xD9U,
 0xDBU, 0xDDU, 0xDFU, 0xE1U, 0xE3U, 0xE5U, 0xE7U, 0xE9U,
 0xEAU, 0xECU, 0xEEU, 0xEFU, 0xF1U, 0xF2U, 0xF3U, 0xF5U,
 0xF6U, 0xF7U, 0xF8U, 0xF9U, 0xFAU, 0xFBU, 0xFCU, 0xFCU,
 0xFDU, 0xFDU, 0xFEU, 0xFEU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFEU, 0xFEU, 0xFDU, 0xFDU,
 0xFCU, 0xFCU, 0xFBU, 0xFAU, 0xF9U, 0xF8U, 0xF7U, 0xF6U,
 0xF5U, 0xF3U, 0xF2U, 0xF1U, 0xEFU, 0xEEU, 0xECU, 0xEAU,
 0xE9U, 0xE7U, 0xE5U, 0xE3U, 0xE1U, 0xDFU, 0xDDU, 0xDBU,
 0xD9U, 0xD6U, 0xD4U, 0xD2U, 0xCFU, 0xCDU, 0xCAU, 0xC8U,
 0xC5U, 0xC2U, 0xC0U, 0xBDU, 0xBAU, 0xB7U, 0xB5U, 0xB2U,
 0xAFU, 0xACU, 0xA9U, 0xA6U, 0xA3U, 0xA0U, 0x9DU, 0x9AU,
 0x97U, 0x94U, 0x91U, 0x8EU, 0x8AU, 0x87U, 0x84U, 0x81U,
 0x7FU, 0x7CU, 0x79U, 0x76U, 0x72U, 0x6FU, 0x6CU, 0x69U,
 0x66U, 0x63U, 0x60U, 0x5DU, 0x5AU, 0x57U, 0x54U, 0x51U,
 0x4EU, 0x4BU, 0x49U, 0x46U, 0x43U, 0x40U, 0x3EU, 0x3BU,
 0x38U, 0x36U, 0x33U, 0x31U, 0x2EU, 0x2CU, 0x2AU, 0x27U,
 0x25U, 0x23U, 0x21U, 0x1FU, 0x1DU, 0x1BU, 0x19U, 0x17U,
 0x16U, 0x14U, 0x12U, 0x11U, 0x0FU, 0x0EU, 0x0DU, 0x0BU,
 0x0AU, 0x09U, 0x08U, 0x07U, 0x06U, 0x05U, 0x04U, 0x04U,
 0x03U, 0x03U, 0x02U, 0x02U, 0x01U, 0x01U, 0x01U, 0x01U,
 0x01U, 0x01U, 0x01U, 0x01U, 0x02U, 0x02U, 0x03U, 0x03U,
 0x04U, 0x04U, 0x05U, 0x06U, 0x07U, 0x08U, 0x09U, 0x0AU,
 0x0BU, 0x0DU, 0x0EU, 0x0FU, 0x11U, 0x12U, 0x14U, 0x16U,
 0x17U, 0x19U, 0x1BU, 0x1DU, 0x1FU, 0x21U, 0x23U, 0x25U,
 0x27U, 0x2AU, 0x2CU, 0x2EU, 0x31U, 0x33U, 0x36U, 0x38U,
 0x3BU, 0x3EU, 0x40U, 0x43U, 0x46U, 0x49U, 0x4BU, 0x4EU,
 0x51U, 0x54U, 0x57U, 0x5AU, 0x5DU, 0x60U, 0x63U, 0x66U,
 0x69U, 0x6CU, 0x6FU, 0x72U, 0x76U, 0x79U, 0x7CU, 0x7FU
};
#define sintb(x) pgm_read_byte(&(sine[((x)      ) & 0xFFU]))
#define costb(x) pgm_read_byte(&(sine[((x) + 64U) & 0xFFU]))


/* Tile map indices */
static const unsigned char tidx[] PROGMEM = {
 0x00U, 0x04U,
 0x18U, 0x04U,
 0x30U, 0x04U,
 0x48U, 0x04U,
 0x60U, 0x04U,
 0x78U, 0x04U,
 0x90U, 0x04U,
 0xA8U, 0x04U,

 0xC0U, 0x04U,
 0xD8U, 0x04U,
 0xF0U, 0x04U,
 0x08U, 0x05U,
 0x20U, 0x05U,
 0x38U, 0x05U,
 0x50U, 0x05U,
 0x68U, 0x05U,

 0x80U, 0x05U,
 0x98U, 0x05U,
 0xB0U, 0x05U,
 0xC8U, 0x05U,
 0xE0U, 0x05U,
 0xF8U, 0x05U,
 0x10U, 0x06U,
 0x28U, 0x06U,

 0x40U, 0x06U,
 0x58U, 0x06U,
 0x70U, 0x06U,
 0x88U, 0x06U,
 0xA0U, 0x06U,
 0xB8U, 0x06U,
 0xD0U, 0x06U,
 0xE8U, 0x06U
};



/* Scolltext */
static const unsigned char scrolltxt[] PROGMEM =
 "                                                                "
 "SPRITES...               SPRITES!...     AND YET MORE SPRITES!!!"
 " THE ATMEGA CAN DO IT! MODE 74 CAN DO IT! 4 BITS PER PIXEL, 64 R"
 "AM TILES, AND A LITTLE MORE SPRITES OCCASIONALLY THAN WHICH FIT "
 "INTO THAT! WHY STOP AT JUST DOING PROPER MASKING IF MORE IS POSS"
 "IBLE? OF COURSE IT ALSO SUPPORTS FLIPPING ON X AND Y, AND EVEN U"
 "SER RAM TILES, JUST NOT DEMONSTRATED HERE. NOW LETS START MAKING"
 " SOME SPIFFIER GAMES FOR UZEBOX! JUBATIAN SIGNS OFF...   WRAP...";



static unsigned int main_c16 = 0U;

static void reset(void){

	unsigned char  i;
	unsigned char  t0;
	unsigned char  t1;
	unsigned char  t2;
	unsigned int   c16  = main_c16;

	M74_VramRestore();

	main_c16 ++;

	/* Render begins. Add an enforced sequence point here, so all
	** non-volatile globals are written out proper before starting the
	** render which may be terminated early. */

	M74_Seq();

	/* Balls */

	if ((c16 & 0x1FFU) < 0x040U)
	{
		t1 = c16 & 0xFFU;
	}
	else if ((c16 & 0x1FFU) < 0x140U)
	{
		t1 = 0x40U;
	}
	else
	{
		t1 = c16 & 0xFFU;
	}
	t1 = sintb(t1);
	for (i = 0U; i < 8U; i++)
	{
		t0 = (c16 + (i * 32U)) & 0xFFU;
		M74_BlitSprite( RES_SPRITES_00_OFF + (32U * 32U),
		                ((( sintb(t0) * (unsigned int)(t1)) >> 8)       >> 1) + 101U + ((0U - t1) >> 2),
		                ((((costb(t0) * (unsigned int)(t1)) >> 8) * 3U) >> 2) + 113U + (((0U - t1) * 3U) >> 3),
		                M74_SPR_I2 | M74_SPR_MASK );
	}

	/* Scrolltext */

	for (i = 0U; i < 25U; i++)
	{
		t0 = pgm_read_byte(&(scrolltxt[((c16 >> 3) + i) & 0x1FFU]));
		if (t0 >= 32U)
		{
			t0 -= 32U;
		}
		if (i < 13)
		{
			t2 = M74_SPR_MASK;
		}
		else
		{
			t2 = 0U;
		}
		t2 |= M74_SPR_I3;
		if (t0 != 0U)
		{
			t1 = ((i + 1U) << 3) - (c16 & 7U);
			M74_BlitSpriteCol( RES_SPRITES_00_OFF + (32U * t0),
			                   t1,
			                   (sintb(t1 + 16U) >> 1) + 32U,
			                   t2,
			                   (((c16 >> 3) + i) << 7) & 0x80U );
		}
	}

	/* Sine lines */

	for (i = 0U; i < 32U; i++)
	{
		M74_PutPixel(i, (i + c16) & 0xFFU, (sintb(i + c16) >> 1) + 45U, M74_SPR_I3 | M74_SPR_MASK);
		M74_PutPixel(i, (i - c16) & 0xFFU, (sintb(i - c16) >> 1) + 50U, M74_SPR_I3 | M74_SPR_MASK);
	}

	/* All done */

	M74_Halt();
}



int main(){

	/* Ensures that the tile map is linked */
	volatile unsigned char dummy_sec = res_sprites_00[0];

	unsigned char  i;
	unsigned int   a16;
	unsigned char* vram = (unsigned char*)(M74_VRAM_OFF);
	unsigned char* pal  = (unsigned char*)(M74_PAL_OFF);

	/* Set rendering parameteras: Reduce height to 22 fake tiles, the many
	** sprites need it (the dragon disc image originally had 24 fake
	** tiles, but looks fine this way, too). */

	SetRenderingParameters(32U, 198U);


	/* Set row selector */

	m74_rows  = (unsigned int)(&rowsel[0]);

	/* Set tile row descriptors */

	m74_tdesc = (unsigned int)(&res_screen_00[0]);
	m74_tidx  = (unsigned int)(&tidx[0]);

	/* Set maximal RAM tile count allocated for sprites */

	m74_rtmax = 64U;

	/* Set reset vector */

	m74_reset = (unsigned int)(&reset);

	/* Load palette */

	for (i = 0U; i < 16U; i++)
	{
		pal[i] = pgm_read_byte(&(res_pal_00[i]));
	}

	/* Set up VRAM */

	for (a16 = 0U; a16 < 24U * 27U; a16++)
	{
		vram[a16] = pgm_read_byte(&(imgvram[a16]));
	}


	/* Over. Add an enforced sequence point here, so all non-volatile
	** globals are written out proper before passing it. Otherwise this
	** wouldn't happen, and non-volatile globals may not be updated
	** ever. */

	M74_Seq();

	/* Set main configuration flags & Enable */

	m74_config =
	    M74_CFG_RAM_PALETTE |
	    M74_CFG_ENABLE;

	M74_Halt();

}
