/*
 *  Uzebox quick and dirty tutorial
 *  Copyright (C) 2008  Alec Bourque
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

#include "data/fonts.pic.inc"

int main(){

	//Set the font and tiles to use.
	//Always invoke before any ClearVram()
	SetFontTable(fonts);

	//Clear the screen (fills the vram with tile zero)
	ClearVram();

	//Prints a string on the screen. Note that PSTR() is a macro 
	//that tells the compiler to store the string in flash.
	Print(8,12,PSTR("HELLO WORLD FROM THE UZEBOX!"));


	while(1);

} 
