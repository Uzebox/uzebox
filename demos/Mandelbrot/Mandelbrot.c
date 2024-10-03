/*
 *  Uzebox Bitmap Demo
 *  Copyright (C) 2009 Alec Bourque
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

/*
 * This program render a Mandelbrot set to compete with various 8-bit micros.
 * Uses fixed point math.
 *
 * See: https://github.com/SlithyMatt/multi-mandlebrot
 */
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uzebox.h"

#include "data/fat_pixels.inc"
#include "data/fonts.pic.inc"

//fixed point math macros
#define FixedPtBits 12
#define toFixed(int_number) ((int32_t)int_number<<FixedPtBits)
#define fixedMul(a,b) ((a*b)>>FixedPtBits)

void mandelbrot_float();
void mandelbrot_fixed();

int putchar_printf(char c, FILE *stream) {
 	static u8 x=0,y=0;
 	if(c=='\n'){
 		x=1;y++;
 	}else if(c=='\r'){
 		x=1;
 	}else if(c=='\f'){
 		x=1;y=1;
 	}else if(c>=' ' && c <= '~'){
 		PrintChar(x++,y,c);
 	}
 	return 0;
}
static FILE stream = FDEV_SETUP_STREAM(putchar_printf, NULL, _FDEV_SETUP_WRITE);

int main(){
	u16 elapse1, elapse2,elapse3,elapse4;
	stdout=&stream;

	ClearVram();
	SetTileTable(pixels);
	SetFontTilesIndex(16);
	SetRenderingParameters(20, FRAME_LINES);

	ClearVsyncCounter();
	mandelbrot_float();;
	elapse1=GetVsyncCounter();

	SetRenderingParameters(255,0);
	ClearVsyncCounter();
	mandelbrot_float();
	elapse2=GetVsyncCounter();
	SetRenderingParameters(20, FRAME_LINES);

	ClearVsyncCounter();
	mandelbrot_fixed();;
	elapse3=GetVsyncCounter();

	SetRenderingParameters(255,0);
	ClearVsyncCounter();
	mandelbrot_fixed();
	elapse4=GetVsyncCounter();
	SetRenderingParameters(20, FRAME_LINES);

	printf_P(PSTR("\f\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"));
	printf_P(PSTR("FLOAT(VIDEO ON) = %f SECONDS\n"),(float)elapse1/60.0f);
	printf_P(PSTR("FLOAT(VIDEO OFF)= %f SECONDS\n"),(float)elapse2/60.0f);
	printf_P(PSTR("FIXED(VIDEO ON) = %f SECONDS\n"),(float)elapse3/60.0f);
	printf_P(PSTR("FIXED(VIDEO OFF)= %f SECONDS\n"),(float)elapse4/60.0f);

	while(1);
}


#define px_const 0x1c0 //fixedDivWithInt(0x3800,32) // = 3.5/32
#define py_const 0x174 //fixedDivWithInt(0x2000,22) // = 2/22
void mandelbrot_fixed(){
	uint8_t px, py,i;
	int32_t xz,yz,x,y,xt,x2,y2;

	for(py = 0; py < 22; py++) {
	  for(px = 0; px < 32; px++) {

		  xz = fixedMul(toFixed(px), px_const) - 0x2800; //XZ=PX*3.5/32-2.5
  		  yz = fixedMul(toFixed(py), py_const) - 0x1000; //YZ=PY*2/22-1
		  x=0; y=0;

		 for(i = 0; i < 15; i++) {
			x2 = fixedMul(x,x);
			y2 = fixedMul(y,y);
			if(x2 + y2 > 0x4000) break;
			xt = x2 - y2 + xz;
			y = (fixedMul(x,y)<<1) + yz;
			x= xt;
		 }

		SetTile(px,py,i-1);

	  }	// Move to the next pixel
	}	// Move to the next line of pixels

}

void mandelbrot_float(){
	uint8_t i;
	float xz=0,yz=0,x=0,y=0,xt=0,py=0,px=0;

	for(py = 0; py < 22; py++) {
		for(px = 0; px < 32; px++) {

		  xz=(px*3.5/32)-2.5;
  		  yz=(py*2/22)-1;
		  x=0; y=0;

		 for(i = 0; i < 15; i++) {
			if((x*x)+(y*y) > 4) break;
			xt = (x*x) - (y*y) + xz;
			y = 2*x*y + yz;
			x=xt;
		 }

		 SetTile(px,py,i-1);

	  }	// Move to the next pixel
	}	// Move to the next line of pixels

}


