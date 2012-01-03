/*
 *  Uzebox Kernel - Video Mode 1
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
; Tile Size: 	6x8
; Resolution: 	240x224 pixels (40x28 tiles)
; Sprites: 		No
; Scrolling: 	No
;
; Description
; -----------
; This video mode is tile-based and does not support 
; sprites or scrolling. Tile are 6x8 pixels
; (6 horizontally by 8 vertically). The VRAM is organized as
; a 40x28 array of 16-bits pointers which points
; to individual tiles in flash. Because it is using 
; adresses instead of indexes (like most other modes),
; more than 256 tiles can be displayed simultaneously.
; After initialization, pointers to a tile set and/or font set
; must be defined by calling SetTileTable() and SetFontTable().
;
; Functions specific to mode 1
; ----------------------------
; -SetFontTable(): Defines the font set to use with Print() functions
; -SetFont()     : Sets a font at the specified X/Y location in VRAM
; 
;***************************************************************

.global vram

.section .bss
	vram: 	  	.space VRAM_SIZE	;allocate space for the video memory (VRAM)
	font_table_lo:	.byte 1			;pointer to user font table
	font_table_hi:	.byte 1	

.section .text

sub_video_mode1:

	;waste line to align with next hsync in render function
	ldi ZL,222-15-1
mode1_render_delay:
	rjmp .
	rjmp .
	dec ZL
	brne mode1_render_delay 
	

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16
	clr r22
	ldi r23,TILE_WIDTH ;tile width in pixels



next_text_line:	
	rcall hsync_pulse ;3+144=147

	ldi r19,50 - 4  + CENTER_ADJUSTMENT
	dec r19			
	brne .-4

	;***draw line***
	call render_tile_line

	ldi r19,13 + 4 - CENTER_ADJUSTMENT
	dec r19			
	brne .-4


	dec r10
	breq text_frame_end
	
	lpm ;3 nop
	inc r22

	cpi r22,8 ;last char line? 1
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
	ldi r19,VRAM_TILES_H*2
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


	;set vsync flag if beginning of next frame (each two fields)
	ldi r17,1
	lds r16,curr_field
	eor r16,r17
	sts curr_field,r16

	#if MODE1_FAST_VSYNC == 0
		sbrs r16,0
	#endif
	
	sts vsync_flag,r17

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
	movw ZL,r20			;load the next tile's adress in Z
	dec r18				;decrement horizontal tiles to draw

	out VIDEO_PORT,r16	;and output it to the video DAC
	brne mode1_loop		
	
	rjmp .				;2 cycles delay
	nop					;1 cycle delay
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

	;ldi r18,40*2
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
