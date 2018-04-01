/*
 *  Mode 748, Row mode 5 demo
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
         0U, /* First line: Start at scanline 0 */
 255U        /* End of list */
};


/* VRAM rows for 216 lines */
static m74_mode5_vram_t vramrow[27U];


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
 (u16)(&vramrow[26]),
 (u16)(&vramrow[26]),
 (u16)(&vramrow[26]),
 (u16)(&vramrow[26]),
 (u16)(&vramrow[26])
};



int main(){

	sdc_struct_t tsds;
	u8  i;
	u8  j;
	u8* pal  = (u8*)(m74_paddr);


	/* Set main configuration flags, display disabled */

	m74_config = 0U;


	/* Init FS to reset SD card, to ensure that the SPI RAM is accessible
	** (otherwise it may interfere). */

	tsds.bufp = (u8*)(0x0800U); /* Just using some free RAM */
	FS_Init(&tsds);
	SpiRamInit();


	/* Set rendering parameters for 4:3 display. */

	SetRenderingParameters(24U, 216U);


	/* Set row selector */

	m74_rows  = (u16)(&rowsel[0]);

	/* Set tile row VRAM addresses */

	m74_vaddr = (u16)(&vramrow_offs[0]);


	/* Load palette */

	for (i = 0U; i < 16U; i++)
	{
		pal[i] = pgm_read_byte(&(res_pal[i]));
	}


	/* Set up VRAM */

	for (j = 0U; j < (216U / 8U); j ++){
		vramrow[j].config = 5U; /* Row mode: 5 */
		vramrow[j].img_addr = ((u16)(j) * 72U * 8U); /* Address in SPI RAM */
	}


	/* Set up SPI RAM */

	SpiRamSeqWriteStart(0U, 0x0000U);
	for (j = 0U; j < 216U; j++){
		for (i = 0U; i < 72U; i++){
			SpiRamSeqWriteU8(
			    pgm_read_byte(&res_img[((u16)(j) * 72U) + i]));
		}
	}
	SpiRamSeqWriteEnd();


	/* Enable display */

	m74_config |= M74_CFG_ENABLE;

	while(1);

}
