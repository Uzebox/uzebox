/*
 *  SD/MMC Card reader demo
 *  Copyright (C) 2008 David Etherton
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
#include <uzebox.h>
#include <mmc.h>

#include "data/fonts.pic.inc"

int sector;
int offset;
unsigned char buffer[512];

char translate_char(char ch)
{
	if (ch >= ' ' && ch <= 'Z')
		return ch;
	else if (ch >= 'a' && ch <= 'z')
		return ch-32;
	else
		return '.';
}

void update_display()
{
	unsigned char row, col;
	Print(3,1,PSTR("SECTOR"));
	PrintHexInt(10,1,sector);
	for (row=0; row<16; row++)
	{
		PrintHexInt(2,row+3,offset+(row<<3));
		for (col=0; col<8; col++)
			PrintHexByte(col * 3 + 7,row+3,buffer[offset+(row<<3)+col]);
		for (col=0; col<8; col++)
			PrintChar(31+col,row+3,translate_char(buffer[offset+(row<<3)+col]));
	}
}


int main(){
   unsigned char temp;

   SetFontTable(fonts);
   ClearVram();
   Print(8,20,PSTR("TESTING MMC..."));

   do { temp = mmc_init(buffer);
   		Print(8,21,temp? PSTR("INIT FAILED") : PSTR("INIT GOOD   ")); } while (temp);

   do { temp = mmc_readsector(0);
   		Print(8,22,temp? PSTR("FIRST READ FAILED") : PSTR("FIRST READ GOOD   ")); } while (temp);

   Print(8,23,PSTR("L/R SHOULDER = NEW SECTOR"));
   Print(8,24,PSTR("PAD UP/DN = SCROLL"));


	update_display();

  	while (1)
  	{
      	unsigned int btns = ReadJoypad(0);
      	WaitVsync(2);

		if ((btns & BTN_SL) && sector)
			--sector;
		else if (btns & BTN_SR)
			++sector;

		// reread sector if either shoulder button was pressed (and wait for release)
		if (btns & (BTN_SL|BTN_SR)) {
			mmc_readsector(sector);
			// while (ReadJoypad(0)) WaitVsync(1);
		}

		if ((btns & BTN_DOWN) && offset < 512-(16*8))
			offset+=8;
		else if ((btns & BTN_UP) && offset)
			offset-=8;

		// redraw display if any buttons were pressed
		if (btns)
			update_display();
   }

} 
