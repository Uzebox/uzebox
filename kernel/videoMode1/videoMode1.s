/*
 *  Uzebox Kernel - Video Mode 1
 *  Copyright (C) 2008-2012  Alec Bourque
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
; Video Mode 1 Rasterizer and Functions
;***************************************************************
;
; Spec
; ----
; Type:			Tile-based
; Cycles/Pixel: 6
; Tile Width: 	6 or 8
; Tile Height:  Variable
; Resolution: 	240x224 pixels
; Sprites: 		No
; Scrolling: 	No
;
; Description
; -----------
; This video mode is tile-based and does not support 
; sprites or scrolling. Tile are 6 or 8 pixels wide
; and can have an arbitrary height (i.e: 8, 9, 12 etc). 
; The VRAM is organized by default as
; a 40x28 (30x28) array of 16-bits pointers which points
; to individual tiles in flash. Because it is using 
; adresses instead of indexes (like most other modes),
; more than 256 tiles can be displayed simultaneously.
; After initialization, pointers to a tile set and/or font set
; must be defined by calling SetTileTable() and SetFontTable().
;
; To define tile width, use makefile parameter -DTILE_WIDTH=n where n=[6|8]
;
; Functions specific to mode 1
; ----------------------------
; -SetFontTable(): Defines the font set to use with Print() functions
; -SetFont()     : Sets a font at the specified X/Y location in VRAM
; 
;***************************************************************

.global vram
.global SetTile
.global ClearVram
.global SetFont
.global SetTile
.global SetFontTable
.global SetTileTable
.global tile_table_lo
.global tile_table_hi



.section .bss
	vram: 	  		.space VRAM_SIZE	;allocate space for the video memory (VRAM)
	font_table_lo:	.space 1			;pointer to user font table
	font_table_hi:	.space 1
	tile_table_lo:	.space 1
	tile_table_hi:	.space 1
	
.section .text

sub_video_mode1:

	;waste line to align with next hsync in render function
	WAIT r19,1347

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16
	clr r22
	ldi r23,TILE_WIDTH ;tile width in pixels



next_tile_line:	
	rcall hsync_pulse

	WAIT r19,HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT
	
	call render_tile_line

	WAIT r19,51 - CENTER_ADJUSTMENT

	dec r10
	breq frame_end
	
	lpm ;3 nop
	inc r22

	cpi r22,TILE_HEIGHT ;last char line? 1
	breq next_tile_row 
	
	;wait to align with next_tile_row instructions (+1 cycle for the breq)
	WAIT r19,10

	rjmp next_tile_line	

next_tile_row:
	clr r22		;current char line			;1	

	clr r0
	ldi r19,VRAM_TILES_H*2
	add YL,r19
	adc YH,r0

	lpm
	nop

	rjmp next_tile_line

frame_end:

	WAIT r19,17

	rcall hsync_pulse ;145

	;set vsync flag & flip field
	lds ZL,sync_flags
	ldi r20,SYNC_FLAG_FIELD
	eor ZL,r20
	#if MODE1_FAST_VSYNC == 0
		sbrs ZL,1
	#endif
	ori ZL,SYNC_FLAG_VSYNC
	sts sync_flags,ZL
		
	;clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	ret

;*************************************************
; Renders a line within the current tile row.
; Draws 1) 40 tiles (6 pixels wide) @ 6 clocks per pixel 
;       or
;       2) 30 tiles (8 pixels wide) @ 6 clocks per pixel   
;
; r22     = Y offset in tile row (0-7)
; r23 	  = tile width in bytes
; Y       = VRAM adress to draw from (must not be modified)
; 
; cycles  = 1495
;*************************************************
render_tile_line:

	movw XL,YL			;copy current VRAM pointer to X
	
	;////////////////////////////////////////////
	;Compute the adress of the first tile to draw
	;////////////////////////////////////////////
	ld	r20,X+			;load absolute tile adress from VRAM (LSB)
	ld	r21,X+			;load absolute tile adress from VRAM (MSB)
	mul r22,r23			;compute Y offset in current tile row
	movw r24,r0			;store result in r24:r25 for use in inner loop
	add r20,r24			;add Y offset to tile address
	adc r21,r25 		;add Y offset to tile address
	movw ZL,r20 		;copy to Z, the only register that can read from flash

	ldi r18,SCREEN_TILES_H ;load the number of horizontal tiles to draw

mode1_loop:	
	lpm r16,Z+			;get pixel 0 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC
	
	ld	r20,X+			;load next tile adress from VRAM (LSB)

	lpm r16,Z+			;get pixel 1 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	ld	r21,X+			;load next tile adress from VRAM (MSB)

	lpm r16,Z+			;get pixel 2 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	rjmp .				;2 cycles delay

	lpm r16,Z+			;get pixel 3 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	add r20,r24			;add Y offset to tile address
	adc r21,r25 		;add Y offset to tile address

	lpm r16,Z+			;get pixel 4 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	lpm r16,Z+			;get pixel 5 from flash

#if TILE_WIDTH == 8
	rjmp .				;2 cycles delay

	out VIDEO_PORT,r16	;and output it to the video DAC
	lpm r16,Z+			;get pixel 6 from flash
	rjmp .				;2 cycles delay

	out VIDEO_PORT,r16	;and output it to the video DAC
	lpm r16,Z+			;get pixel 7 from flash
#endif

	movw ZL,r20			;load the next tile's adress in Z
	dec r18				;decrement horizontal tiles to draw

	out VIDEO_PORT,r16	;and output it to the video DAC
	brne mode1_loop		
	
	lpm					;3 cycles delay
	clr r16				;set last pixel to zero (black)
	out VIDEO_PORT,r16

	ret


;***********************************
; CLEAR VRAM
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearVram
ClearVram:
	//init vram		
	ldi r30,lo8(VRAM_TILES_H*VRAM_TILES_V)
	ldi r31,hi8(VRAM_TILES_H*VRAM_TILES_V)

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)


	lds r22,font_table_lo
	lds r23,font_table_hi

fill_vram_loop:

	st X+,r22
	st X+,r23	
	sbiw r30,1
	brne fill_vram_loop

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
	clr r21

	ldi r18,VRAM_TILES_H
	lsl r18	

	mul r22,r18		;calculate Y line addr in vram
	lsl r24
	add r0,r24		;add X offset
	adc r1,r25
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	lds r22,font_table_lo
	lds r23,font_table_hi

	ldi r18,(TILE_WIDTH*TILE_HEIGHT)
	mul r20,r18
	add r22,r0
	adc r23,r1

	st X+,r22
	st X,r23

	clr r1

	ret


;***********************************
; SET TILE
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; r21:r20=Tile No (16 bit)
;************************************
.section .text.SetTile
SetTile:
	clr r25
	clr r23	

	ldi r18,VRAM_TILES_H
	lsl r18	

	mul r22,r18		;calculate Y line addr in vram
	lsl r24
	add r0,r24		;add X offset
	adc r1,r25
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	lds r22,tile_table_lo
	lds r23,tile_table_hi

	ldi r18,(TILE_WIDTH*TILE_HEIGHT)
	mul r20,r18
	add r22,r0
	adc r23,r1

	mul r21,r18
	add r23,r0

	st X+,r22
	st X,r23

	clr r1

	ret

	
;***********************************
; Define the tile data source
; C-callable
; r25:r24=pointer to font tiles data
;************************************
.section .text.SetFontTable
SetFontTable:
	sts font_table_lo,r24
	sts font_table_hi,r25

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
