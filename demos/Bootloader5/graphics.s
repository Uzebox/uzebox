/*
 *  Graphics support functions
 *  Copyright (C) 2017 Sandor Zsuga (Jubatian)
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
#include "kernel/defines.h"



.section .text



;
; Creates a gradient bar.
;
; This bar is 10 pixels tall, uses the following layout:
;
; Top / bottom framing bar: Color 1 (Fg, Bg)
; Background of the 8 pixel body: Color 8 - 15
; Foreground of the 8 pixel body: Color 8 - 15
;
; Inputs:
;     r24: Top location
; Clobbers:
; r1 (zero), r24, ZL, ZH
;
.global Graphics_GradBar
Graphics_GradBar:

	rcall graphics_gettop
	ldi   r24,     0x11
	st    Z+,      r24
	std   Z + 8,   r24
	ldi   r24,     0x88
Graphics_GradBar_l:
	st    Z+,      r24
	subi  r24,     0xEF
	cpi   r24,     0x10
	brne  Graphics_GradBar_l
	ret



;
; Clears a region in the color map
;
; Background color: 0
; Foreground color: 0
;
; Inputs:
;     r24: Top location
;     r22: Count of rows to clear
; Clobbers:
; r1 (zero), r22, r24, ZL, ZH
;
.global Graphics_ClearCol
Graphics_ClearCol:

	ldi   r20,     0x00    ; Fall through to set color



;
; Sets a region in the color map
;
; Inputs:
;     r24: Top location
;     r22: Count of rows to set
;     r20: Color code
; Clobbers:
; r1 (zero), r22, r24, ZL, ZH
;
.global Graphics_SetCol
Graphics_SetCol:

	rcall graphics_gettop
Graphics_ClearCol_l:
	st    Z+,      r20
	dec   r22
	brne  Graphics_ClearCol_l
	ret



;
; Calculates start pointer in Z for the line color map
;
; Inputs:
;     r24: Top location
; Outputs:
; ZH: ZL:  Pointer in line color map
; Clobbers:
; r1 (zero)
;
graphics_gettop:

	clr   r1
	ldi   ZL,      lo8(linecol)
	ldi   ZH,      hi8(linecol)
	add   ZL,      r24
	adc   ZH,      r1
	ret



;
; Copies short stretches of data from ROM to RAM. Useful for VRAM filling.
;
; Inputs:
; r25:r24: Destination RAM location
; r23:r22: Source ROM location
;     r20: Byte count
; Clobbers:
; r0, r20, ZL, ZH
; XH: XL:  =Input(r25:r24) + Byte count
;
.global Graphics_CopyROM
Graphics_CopyROM:

	movw  XL,      r24
	movw  ZL,      r22
Graphics_CopyROM_l:
	lpm   r0,      Z+
	st    X+,      r0
	dec   r20
	brne  Graphics_CopyROM_l
	ret



;
; Clears the VRAM. Use this instead of ClearVRAM as it uses the ClearRAM
; function in the kernel added for the bootloader.
;
.global Graphics_ClearVRAM
Graphics_ClearVRAM:

	ldi   r24,     lo8(vram)
	ldi   r25,     hi8(vram)
	ldi   r22,     (VRAM_SIZE & 0xFF)
	ldi   r23,     (VRAM_SIZE >> 8)
	rjmp  ClearRAM



;
; Initializes palette.
;
; Clobbers:
; r0, r20, XL, XH, ZL, ZH
;
.global Graphics_InitPal
Graphics_InitPal:

	ldi   r24,     lo8(pal_fg)
	ldi   r25,     hi8(pal_fg)
	ldi   r22,     lo8(Graphics_paldata)
	ldi   r23,     hi8(Graphics_paldata)
	ldi   r20,     32
	rjmp  Graphics_CopyROM

Graphics_paldata:
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0xAD, 0xB6, 0xB7, 0xFF, 0xFF, 0xB7, 0xB6, 0xAD
	.byte 0x00, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x01, 0x0A, 0x0B, 0x13, 0x13, 0x0B, 0x0A, 0x01

