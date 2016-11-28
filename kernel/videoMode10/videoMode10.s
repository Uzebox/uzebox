/*
 *  Uzebox Kernel - Video Mode 10
 *  Copyright (C) 2011  Alec Bourque, 2012 Martin Sustek
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

;***************************************************************
; Video Mode 10 Rasterizer and Functions
;***************************************************************
;
; Spec
; ----
; Type:			Tile-based
; Cycles/Pixel: 6
; Tile Size: 	12x16
; Resolution: 	192x192 pixels (16x12 tiles, hacked to 12x16)
; Sprites: 		No
; Scrolling: 	No
;
; Description
; -----------
; This video mode is tile-based and does not support 
; sprites or scrolling. Tile are 12x16 pixels
; (12 horizontally by 16 vertically). The VRAM is organized as
; a 16x12 array of 8-bit indexes, hence no more than 256 tiles 
; can be displayed simultaneously.
; After initialization, the tile set to use and font index
; must be defined by calling SetTileTable() and SetFontTilesIndex().
;
; Note: This mode is very similar to mode 1, but uses 8-bit indexes
; hence halving the VRAM requirement. Use this mode when more
; RAM is required by the game logic.
;
; Note: This is hacked mode 5, scanline timing is estimated
; (by TV and osciloscope), so no guarantee, that it will work.
; 
;***************************************************************

.global vram
.global SetTile
.global ClearVram
.global SetFontTilesIndex
.global SetTileTable
.global SetTile
.global SetFont

.section .bss
	vram: 	  	.space VRAM_SIZE	;allocate space for the video memory (VRAM)
	tile_table_lo:	.space 1
	tile_table_hi:	.space 1
	font_tile_index:.space 1

.section .text

sub_video_mode5:

	;waste line to align with next hsync in render function
	WAIT ZL,1347

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	;total scanlines to draw	
	lds r10,render_lines_count	

	clr r22
	ldi r23,TILE_WIDTH ;tile width in pixels

next_text_line:	
	rcall hsync_pulse ;3+144=147

	WAIT r19,358 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT

	;***draw line***
	call render_tile_line

	WAIT r19,143 - CENTER_ADJUSTMENT

	dec r10
	breq text_frame_end
	
	lpm ;3 nop
	inc r22

	cpi r22,16 ;last char line? 1
	breq next_text_row 
	
	;wait to align with next_tile_row instructions (+1 cycle for the breq)
	lpm ;3 nop
	lpm ;3 nop
	lpm ;3 nop
	nop

	rjmp next_text_line	

next_text_row:
	clr r22		;current char line			;1	

	clr r0
	ldi r19,VRAM_TILES_H
	add YL,r19
	adc YH,r0

	lpm
	nop

	rjmp next_text_line

text_frame_end:

	ldi r19,5
	dec r19			
	brne .-4
	rjmp .

	rcall hsync_pulse ;145

	;set vsync flag & flip field
	lds ZL,sync_flags
	ldi r20,SYNC_FLAG_FIELD
	ori ZL,SYNC_FLAG_VSYNC
	eor ZL,r20
	sts sync_flags,ZL

	;clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	ret

;*************************************************
; Renders a line within the current tile row.
; Draws 40 tiles wide @ 6 clocks per pixel
;
; r22     = Y offset in tile row (0-7)
; r23 	  = tile width in bytes
; Y       = VRAM adress to draw from (must not be modified)
; 
; cycles  = 1495 (not sure)
;*************************************************
render_tile_line:

	movw XL,YL			;copy current VRAM pointer to X
	ldi r17,TILE_HEIGHT*TILE_WIDTH	

	;////////////////////////////////////////////
	;Compute the adress of the first tile to draw
	;////////////////////////////////////////////
	lds r24,tile_table_lo
	lds r25,tile_table_hi
	mul r22,r23			;compute Y offset in current tile row
	add r24,r0			;add to base tileset adress
	adc r25,r1

	ld r20,X+			;load first tile index from VRAM
	mul r20,r17			;multiply tile index by tile size	
	add r0,r24			;add tileset adress
	adc r1,r25
	movw ZL,r0	 		;copy to Z, the only register that can read from flash

	ldi r18,SCREEN_TILES_H ;load the number of horizontal tiles to draw

mode5_loop:	
	lpm r16,Z+			;get pixel 0 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC
	
	ld	r20,X+			;load next tile index from VRAM

	lpm r16,Z+			;get pixel 1 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	mul r20,r17			;multiply tile index by tile size

	lpm r16,Z+			;get pixel 2 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	movw r20,r24		;load tile table base adress+line offset
	add r20,r0

	lpm r16,Z+			;get pixel 3 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	adc r21,r1
	nop

	lpm r16,Z+			;get pixel 4 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC
	lpm				;3 cycles delay

	lpm r16,Z+			;get pixel 5 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC
	lpm				;3 cycles delay

	lpm r16,Z+			;get pixel 6 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC
	lpm				;3 cycles delay

	lpm r16,Z+			;get pixel 7 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC
	lpm				;3 cycles delay

	lpm r16,Z+			;get pixel 8 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC
	lpm				;2 cycles delay

	lpm r16,Z+			;get pixel 9 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC
	lpm				;2 cycles delay

	lpm r16,Z+			;get pixel 10 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	lpm r16,Z+			;get pixel 11 from flash
	movw ZL,r20			;load the next tile's adress in Z
	dec r18				;decrement horizontal tiles to draw
	
	out VIDEO_PORT,r16	;and output it to the video DAC
	brne mode5_loop		


	rjmp .				;2 cycles delay
	nop					;1 cycle delay
	clr r16				;set last pixel to zero (black)
	out VIDEO_PORT,r16

	ret

;***********************************
; CLEAR VRAM 8bit
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearVram
ClearVram:
	//init vram		
	ldi r30,lo8(VRAM_SIZE)
	ldi r31,hi8(VRAM_SIZE)

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)

fill_vram_loop:
	st X+,r1
	sbiw r30,1
	brne fill_vram_loop

	clr r1

	ret

	
;***********************************
; SET TILE 8bit mode
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; r20=Tile No (8 bit)
;************************************
.section .text.SetTile
SetTile:

	clr r25
	clr r23	

	ldi r18,VRAM_TILES_H

	mul r22,r18		;calculate Y line addr in vram
	add r0,r24		;add X offset
	adc r1,r25
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	st X,r20

	clr r1

	ret

;***********************************
; SET FONT TILE
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; r20=Font tile No (8 bit)
;************************************
.section .text.SetFont
SetFont:
	clr r25

	ldi r18,VRAM_TILES_H

	mul r22,r18		;calculate Y line addr in vram
	
	add r0,r24		;add X offset
	adc r1,r25

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	lds r21,font_tile_index
	add r20,r21

	st X,r20

	clr r1

	ret


;***********************************
; SET FONT Index
; C-callable
; r24=First font tile index in tile table (8 bit)
;************************************
.section .text.SetFontTilesIndex
	SetFontTilesIndex:
	sts font_tile_index,r24
	ret


;***********************************
; Define the tile data source
; C-callable
; r25:r24=pointer to tiles data
;************************************
.section .text.SetTileTable
SetTileTable:
	sts tile_table_lo,r24
	sts tile_table_hi,r25	
	ret
