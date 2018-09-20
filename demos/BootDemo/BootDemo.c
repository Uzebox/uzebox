/*
 *  Bootloader interface library demo
 *  Copyright (C) 2018 Sandor Zsuga
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


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <bootlib.h>



/* Text elements */

static const char txt_card[] PROGMEM = "Card type ........:";
static const char txt_sdsc[] PROGMEM = "SDSC";
static const char txt_sdhc[] PROGMEM = "SDHC";
static const char txt_sdno[] PROGMEM = "None (Fault: .)";

static const char txt_fs[]   PROGMEM = "Filesystem type ..:";
static const char txt_fs16[] PROGMEM = "FAT16";
static const char txt_fs32[] PROGMEM = "FAT32";

static const char txt_csiz[] PROGMEM = "Cluster size .....:";
static const char txt_fatp[] PROGMEM = "FAT start sec. ...:";
static const char txt_datp[] PROGMEM = "Data start sec. ..:";

static const char txt_filc[] PROGMEM = "BOOTDEMO cluster .:";
static const char txt_filn[] PROGMEM = "None (Not found)";

static const char txt_bt0[]  PROGMEM = "Now a bootloader request will be made";
static const char txt_bt1[]  PROGMEM = "(No actual programming will happen";
static const char txt_bt2[]  PROGMEM = "as the request is to reprogram this)";



int main(){

	u8  pos;
	u8  res;
	sdc_struct_t sd_struct;
	u8  buf[512];
	u32 t32;

	/* Assign a sector buffer */

	sd_struct.bufp = &(buf[0]);


	/* Print columns and rows on screen border */

	for(pos = 0U; pos < SCREEN_TILES_H; pos++){
		PrintChar(pos, 0U, '0' + (pos % 10U));
	}
	for(pos = 0U; pos < SCREEN_TILES_V; pos++){
		PrintChar(0U, pos, '0' + (pos % 10U));
	}

	/* Print information headers */

	Print(3U,  3U, txt_card);
	Print(3U,  4U, txt_fs);

	Print(3U,  6U, txt_csiz);
	Print(3U,  7U, txt_fatp);
	Print(3U,  8U, txt_datp);

	Print(3U, 10U, txt_filc);


	/* Init the card & filesystem */

	res = FS_Init(&sd_struct);
	if (res != 0U){
		Print(23U, 3U, txt_sdno);
		PrintChar(36U, 3U, res + '0');
		while(1);
	}
	if ((sd_struct.flags & SDC_FLAGS_SDHC) != 0U){
		Print(23U, 3U, txt_sdhc);
	}else{
		Print(23U, 3U, txt_sdsc);
	}
	if ((sd_struct.flags & SDC_FLAGS_FAT32) != 0U){
		Print(23U, 4U, txt_fs32);
	}else{
		Print(23U, 4U, txt_fs16);
	}


	/* Print some filesystem info */

	PrintHexInt(23U, 6U, sd_struct.csize);
	PrintHexLong(23U, 7U, sd_struct.fatp);
	PrintHexLong(23U, 8U, sd_struct.datap);


	/* Find the program itself on the SD card */

	t32 = FS_Find(&sd_struct,
	    ((u16)('B') << 8) |
	    ((u16)('O')     ),
	    ((u16)('O') << 8) |
	    ((u16)('T')     ),
	    ((u16)('D') << 8) |
	    ((u16)('E')     ),
	    ((u16)('M') << 8) |
	    ((u16)('O')     ),
	    ((u16)('U') << 8) |
	    ((u16)('Z')     ),
	    ((u16)('E') << 8) |
	    ((u16)(0)       ));
	if (t32 == 0U){
		Print(23U, 10U, txt_filn);
		while(1);
	}
	PrintHexLong(23U, 10U, t32);


	/* Read stuff from file */

	FS_Select_Cluster(&sd_struct, t32);
	FS_Read_Sector(&sd_struct);

	for (pos = 0U; pos < 32U; pos ++){
		PrintChar(3U + pos, 12U, buf[14U + pos]);
	}
	for (pos = 0U; pos < 32U; pos ++){
		PrintChar(3U + pos, 13U, buf[46U + pos]);
	}


	/* Wait some */

	WaitVsync(255);


	/* Display bootloader request info */

	Print(3U, 16U, txt_bt0);
	Print(3U, 17U, txt_bt1);
	Print(3U, 18U, txt_bt2);

	WaitVsync(255);
	WaitVsync(255);


	/* Ask for programming. Note that no actual programming will happen
	** as the demo calls for reprogramming itself, which the V.5.x.x
	** bootloaders which have this API detect properly. */

	Bootld_Request(&sd_struct);
	while(1);

}
