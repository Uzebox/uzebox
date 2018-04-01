/*
 *  Mode 748, Sprite demo
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
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <spiram.h>
#include <bootlib.h>
#include "tiles.h"



/* Row selectors */
static unsigned char rowsel[] = {
         9U, /* First line: Start at scanline 9 */
 255U        /* End of list */
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



/* VRAM rows (Mode 2 rows are nice for filling up unused rows) */
static m74_mode0_vram_t vramrow[27U];
static m74_mode2_vram_t vramdummy = { 2U | M74_CFG_PAL_SRC_NONE, 0U, 0x1FU };


/* Tile row VRAM address table */
static u16 const vramrow_offs[32U] PROGMEM = {
 (u16)(&vramrow[ 0]),
 (u16)(&vramrow[ 1]),
 (u16)(&vramrow[ 2]),
 (u16)(&vramrow[ 3]),
 (u16)(&vramrow[ 4]),
 (u16)(&vramrow[ 5]),
 (u16)(&vramrow[ 6]),
 (u16)(&vramrow[ 7]),
 (u16)(&vramrow[ 8]),
 (u16)(&vramrow[ 9]),
 (u16)(&vramrow[10]),
 (u16)(&vramrow[11]),
 (u16)(&vramrow[12]),
 (u16)(&vramrow[13]),
 (u16)(&vramrow[14]),
 (u16)(&vramrow[15]),
 (u16)(&vramrow[16]),
 (u16)(&vramrow[17]),
 (u16)(&vramrow[18]),
 (u16)(&vramrow[19]),
 (u16)(&vramrow[20]),
 (u16)(&vramrow[21]),
 (u16)(&vramrow[22]),
 (u16)(&vramrow[23]),
 (u16)(&vramrow[24]),
 (u16)(&vramrow[25]),
 (u16)(&vramrow[26]),
 (u16)(&vramdummy),
 (u16)(&vramdummy),
 (u16)(&vramdummy),
 (u16)(&vramdummy),
 (u16)(&vramdummy)
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



int main(){

	sdc_struct_t tsds;
	u8  i;
	u8  j;
	u8  t0;
	u8  t1;
	u8  t2;
	u16 a16;
	u16 c16 = 0U;
	u8* pal  = (u8*)(m74_paddr);


	/* Set main configuration flags, display disabled */

	m74_config = 0U;


	/* Init FS to reset SD card, to ensure that the SPI RAM is accessible
	** (otherwise it may interfere). */

	tsds.bufp = &(vramrow[0].config); /* Just using some free RAM */
	FS_Init(&tsds);
	SpiRamInit();


	/* Set rendering parameteras: Reduce height to 22 fake tiles, the many
	** sprites need it (the dragon disc image originally had 24 fake
	** tiles, but looks fine this way, too). */

	SetRenderingParameters(32U, 198U);


	/* Transfer entire ROM to SPI RAM */

	SpiRamSeqWriteStart(0U, 0U);
	a16 = 0U;
	do{
		SpiRamSeqWriteU8(pgm_read_byte(a16));
		a16 ++;
	}while(a16 != 0U);
	SpiRamSeqWriteEnd();


	/* Set row selector */

	m74_rows  = (u16)(&rowsel[0]);

	/* Set tile row VRAM addresses */

	m74_vaddr = (u16)(&vramrow_offs[0]);

	/* Set base and maximal RAM tile count allocated for sprites */

	m74_rtbase = 34U;
	m74_rtmax = 78U;


	/* Load palette */

	for (i = 0U; i < 16U; i++)
	{
		pal[i] = pgm_read_byte(&(res_pal_00[i]));
	}

	/* Set up VRAM (including background in SPI RAM) */

	SpiRamSeqWriteStart(1U, 0U);
	for (j = 0U; j < 27U; j ++){
		vramrow[j].config    = 0U | M74_CFG_SPIRAM_A16; /* Row mode: 0 */
		vramrow[j].bg_addr   = (u16)(j) * 24U;
		vramrow[j].t0_addr_h = (u16)(&res_tiles_00[32U *   0U]) >> 8;
		vramrow[j].t1_addr_h = (u16)(&res_tiles_00[32U * 128U]) >> 8;
		for (i = 0U; i < 24U; i++){
			SpiRamSeqWriteU8(pgm_read_byte(&(imgvram[((u16)(j) * 24U) + i])));
		}
	}
	SpiRamSeqWriteEnd();

	/* Clear foreground VRAM to start with a clean state */

	M74_VramRestore();


	/* Enable display */

	m74_config |= M74_CFG_ENABLE;


	/* Wait for a clean frame start */

	WaitVsync(2U);

	while(1)
	{

		if (GetVsyncFlag() != 0)
		{
			/* Problem: passed vsync already. Skip next render */
			ClearVsyncFlag();
			c16 ++;
			continue;
		}
		else
		{
			while (GetVsyncFlag() == 0);
			ClearVsyncFlag();
		}

		M74_VramRestore();

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
			                ((( sintb(t0) * (u16)(t1)) >> 8)       >> 1) + 101U + ((0U - t1) >> 2),
			                ((((costb(t0) * (u16)(t1)) >> 8) * 3U) >> 2) + 113U + (((0U - t1) * 3U) >> 3),
			                M74_SPR_I2 | M74_SPR_MASK);
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
				                   (((c16 >> 3) + i) << 4) & 0x10U );
			}
		}

		/* Sine lines */

		for (i = 0U; i < 32U; i++)
		{
			M74_PutPixel(i, (i + c16) & 0xFFU, (sintb(i + c16) >> 1) + 45U, M74_SPR_I3 | M74_SPR_MASK);
			M74_PutPixel(i, (i - c16) & 0xFFU, (sintb(i - c16) >> 1) + 50U, M74_SPR_I3 | M74_SPR_MASK);
		}

		c16 ++;

	}

}
