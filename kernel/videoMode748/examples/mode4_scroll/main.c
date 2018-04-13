/*
 *  Mode 748, Row mode 4 demo
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



/* Row selectors */
static u8 rowsel[] = {
         0U, /* First line: Start at scanline 0 */
 255U        /* End of list */
};


/* VRAM rows */
static m74_mode4_vram_t vramrow[32U];


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
 (u16)(&vramrow[27]),
 (u16)(&vramrow[28]),
 (u16)(&vramrow[29]),
 (u16)(&vramrow[30]),
 (u16)(&vramrow[31])
};


/* Maximal Y dimension of image */
#define IMAGE_MAX_Y  (0x20000U / 96U)

/* Start location in SPI RAM for image left column data */
#define LEFTCOL_BASE (0x20000U - (IMAGE_MAX_Y * 12U))



/*
** Get next byte from file
**
** No error checking, just a quick & dirty solution.
*/
static u8  file_get_next_byte(sdc_struct_t* sds, u16* secpos)
{
	u8  rval;
	u16 spos = *secpos;

	spos &= 0x1FFU;
	if (spos == 0U){
		FS_Next_Sector(sds);
		FS_Read_Sector(sds);
	}

	rval = sds->bufp[spos];
	spos ++;
	*secpos = spos;

	return rval;
}



/*
** Fill up a row's VRAM with the appropriate left column data. Since this is
** a simple wraparound scroll, providing the Y position is sufficient to
** determine what to fill and where from.
*/
static void vram_fill_left(u16 ypos)
{
	u8* data = &(vramrow[(u8)(ypos & 0xFFU) >> 3].data[0]);
	u32 t32  = (u32)(LEFTCOL_BASE) + ((ypos & 0xFFF8U) * 12U);
	u8  i;

	SpiRamSeqReadStart(t32 >> 16, t32 & 0xFFFFU);
	for (i = 0U; i < 96U; i ++){
		data[i] = SpiRamSeqReadU8();
	}
	SpiRamSeqReadEnd();
}



int main(){

	sdc_struct_t sds;
	u8  i;
	u8  bpos;
	u8  bval;
	u16 a16;
	u8  dir;
	u8  img_x;
	u16 img_y;
	u16 img_p;
	u32 t32;
	u8* pal  = (u8*)(m74_paddr);
	u8* tbuf;


	/* Set main configuration flags, display disabled */

	m74_config = 0U;


	/* Init FS to reset SD card, to ensure that the SPI RAM is accessible
	** (otherwise it may interfere). */

	sds.bufp = &(vramrow[0].config); /* Just using some free RAM */
	FS_Init(&sds);
	SpiRamInit();


	/* Reduce display to 1 line to accelerate loading (more VBLANK time) */

	SetRenderingParameters(20U, 1U);


	/* Find data file */

	t32 = FS_Find(&sds,
	    ((u16)('M') << 8) |
	    ((u16)('7')     ),
	    ((u16)('4') << 8) |
	    ((u16)('8')     ),
	    ((u16)('R') << 8) |
	    ((u16)('M')     ),
	    ((u16)('4') << 8) |
	    ((u16)('S')     ),
	    ((u16)('D') << 8) |
	    ((u16)('A')     ),
	    ((u16)('T') << 8) |
	    ((u16)(0)       ));

	if (t32 != 0U){

		/* File is there, load from it (no error checks, assume OK) */

		FS_Select_Cluster(&sds, t32);
		FS_Read_Sector(&sds);
		img_x  = sds.bufp[0];
		img_y  = sds.bufp[2];
		img_y |= (u16)(sds.bufp[3]) << 8;
		for (i = 0U; i < 16U; i++){
			pal[i] = sds.bufp[4U + i];
		}
		a16    = 20U;
		if (img_y >= IMAGE_MAX_Y){ img_y = IMAGE_MAX_Y; }

	}else{

		/* No file */

		for (i = 0U; i < 16U; i++)
		{
			pal[i] = 0x05U; /* Red */
		}
		img_x = 0U;
		img_y = 0U;
		a16   = 0U;

	}

	/* Load file data. Left 12 bytes go into storage (for providing left
	** column of the image later), right 84 bytes into the actual display
	** SPI RAM area. */

	img_p = 0U;
	tbuf = &(vramrow[0].config) + 512U; /* Workspace for loading */
	for (i = 0U; i < 96U; i ++){
		tbuf[i] = 0U;
	}

	while (img_p < img_y){

		/* Load one image line, centered for display */

		bpos = 48U - (img_x / 4U);
		for (i = 0U; i < (img_x / 2U); i ++){
			bval = file_get_next_byte(&sds, &a16);
			if (bpos < 96U){
				tbuf[bpos] = bval;
			}
			bpos ++;
		}

		/* Left half */

		t32 = (u32)(LEFTCOL_BASE) + (img_p * 12U);
		SpiRamSeqWriteStart(t32 >> 16, t32 & 0xFFFFU);
		for (i = 0U; i < 12U; i++){
			SpiRamSeqWriteU8(tbuf[i]);
		}
		SpiRamSeqWriteEnd();

		/* Right half */

		t32 = ((u32)(img_p) * 84U);
		SpiRamSeqWriteStart(t32 >> 16, t32 & 0xFFFFU);
		for (i = 0U; i < 84U; i++){
			SpiRamSeqWriteU8(tbuf[i + 12U]);
		}
		SpiRamSeqWriteEnd();

		img_p ++;

	}


	/* Set rendering parameters for full height display. */

	SetRenderingParameters(20U, 224U);


	/* Set row selector */

	m74_rows  = (u16)(&rowsel[0]);

	/* Set tile row VRAM addresses */

	m74_vaddr = (u16)(&vramrow_offs[0]);

	/* Set up VRAM and add initial fill */

	for (i = 0U; i < 32U; i ++){
		vramrow[i].config = 4U; /* Row mode 4 */
		vram_fill_left(i * 8U);
	}


	/* Enable display */

	m74_config |= M74_CFG_ENABLE;


	a16 = 0U;
	dir = 0U;

	while(1)
	{

		WaitVsync(2);

		if (dir == 0U){ /* Scroll down */

			if ((a16 + 224U) <  img_y){
				a16 ++;
				vram_fill_left(a16 + 224U);
			}
			if ((a16 + 224U) >= img_y){
				dir = 1U;
			}

		}else{          /* Scroll up */

			if (a16 != 0U){
				a16 --;
				vram_fill_left(a16);
			}
			if (a16 == 0U){
				dir = 0U;
			}

		}

		rowsel[0] = a16 & 0xFFU;
		m74_m4_bank = ((u32)(a16) * 84U) >> 16;
		m74_m4_addr = ((u32)(a16) * 84U) & 0xFFFFU;

	}

}
