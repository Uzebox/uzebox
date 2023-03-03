/*
 *  Uzebox video mode 13 simple demo
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
#include <avr/interrupt.h>

#include "spiram.c"

#include "data/akuma.inc"

extern void SetVram(u16 addr);

int main(){

	SetVram(0);
	
	SetRenderingParameters(FIRST_RENDER_LINE,0);

	sram_start();

	sram_beginSequence(0,SRAM_WRITE);
	u8 d;
	for(u16 i=0;i<akuma_size;i++){
		d=pgm_read_byte(&(akuma[i]));
		sram_write(d);
	}
	
	
	for(u16 i=0;i<akuma_size;i++){
		d=pgm_read_byte(&(akuma[i]));
		sram_write(d);
	}	
	sram_endSequence();

	SetRenderingParameters(FIRST_RENDER_LINE,FRAME_LINES);

	while(1){
		WaitVsync(120);
		
		for(u8 i=0;i<=224;i++){
			SetVram(i*(360/8));
			WaitVsync(1);
		}
	
	}

} 
