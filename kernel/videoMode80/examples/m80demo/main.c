/*
 *  Uzebox video mode 80 simple demo
 *  Copyright (C) 2019 Sandor Zsuga (Jubatian)
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
#include <stdint.h>



int main(){

	uint16_t i;

	/* Clear the screen (fills the vram with tile zero) */

	ClearVram();

	for (i = 0U; i < (VRAM_TILES_H * VRAM_TILES_V); i ++){
		vram[i] = i & 0xFFU;
	}

	/* Prints some stuff on the screen */

	Print( 0,  1, PSTR("   <--------><--------><--------><--------><--------><--------><--------><-------->   "));
	Print( 0,  2, PSTR("  00        10        20        30        40        50        60        70        80  "));
	Print( 0, 12, PSTR("                                                                                      "));
	Print( 0, 13, PSTR("                              80 x 28 code tile display                               "));
	Print( 0, 14, PSTR("                                  (??? x 224 pixels)                                  "));
	Print( 0, 15, PSTR("                                                                                      "));
	Print( 0, 25, PSTR("  00        10        20        30        40        50        60        70        80  "));
	Print( 0, 26, PSTR("   <--------><--------><--------><--------><--------><--------><--------><-------->   "));

	while(1);

}
