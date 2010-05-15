/*
 *  Uzebox video mode 9 simple demo
 *  Copyright (C) 2010  Alec Bourque
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


#include "data/fonts6x8.inc"


int main(){

	//Set the font and tiles to use.
	//Always invoke before any ClearVram()
	SetTileTable(font);


	//Clear the screen (fills the vram with tile zero)
	ClearVram();

	//Prints a string on the screen. Note that PSTR() is a macro 
	//that tells the compiler to store the string in flash.
	Print(0,10,PSTR("012345678901234567890123456789012345678901234567890123456789"));
	Print(9,12,PSTR("HIGH RESOLUTION MODE DEMO 60X28 - 6X8 TILES"));
	Print(0,14,PSTR("012345678901234567890123456789012345678901234567890123456789"));
	

	while(1);

} 
