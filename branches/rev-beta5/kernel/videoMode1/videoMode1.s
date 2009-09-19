/*
 *  Uzebox Kernel - Mode 1
 *  Copyright (C) 2009  Alec Bourque
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

;***************************************************
; TEXT MODE VIDEO PROCESSING
; Process video frame in tile mode (30*28)
;***************************************************	

.global vram

.section .bss
	vram: 	  	.space VRAM_SIZE
	tile_map_lo:	.byte 1
	tile_map_hi:	.byte 1
	font_table_lo:	.byte 1
	font_table_hi:	.byte 1	

.section .text
;***************************************************
; TEXT MODE VIDEO PROCESSING
; Process video frame in tile mode (40*28)
;***************************************************	

sub_video_mode1:

	;waste line to align with next hsync in render function
	ldi ZL,222 
mode1_render_delay:
	lpm
	nop
	dec ZL
	brne mode1_render_delay 

	lpm
	lpm

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16
	clr r22
	ldi r23,TILE_WIDTH ;tile width in pixels



next_text_line:	
	rcall hsync_pulse ;3+144=147

	ldi r19,37 + CENTER_ADJUSTMENT
	dec r19			
	brne .-4

	;***draw line***
	call render_tile_line

	ldi r19,26 - CENTER_ADJUSTMENT
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
; RENDER TILE LINE
;
; r22     = Y offset in tiles
; r23 	  = tile width in bytes
; Y       = VRAM adress to draw from (must not be modified)
;
; Can destroy: r0,r1,r2,r3,r4,r5,r13,r16,r17,r18,r19,r20,r21,Z
; 
; cycles  = 1495
;*************************************************
render_tile_line:

	movw XL,YL

	;add tile Y offset
	mul r22,r23
	movw r24,r0

	;load the first tile from vram
	ld	r20,X+	;load tile adress LSB from VRAM
	ld	r21,X+	;load tile adress MSB from VRAM
	add r20,r24	;add tile address
	adc r21,r25  ;add tile address	


	movw ZL,r20
	;draw 40 tiles wide, 6 clocks/pixel
	ldi r18,SCREEN_TILES_H

m1_loop:	
	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16
	
	ld	r20,X+	;load tile adress # LSB from VRAM

	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16

	ld	r21,X+	;load tile adress # MSB from VRAM

	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16

	rjmp .

	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16

	add r20,r24	;add tile table row offset 
	adc r21,r25 ;add tile table row offset 

	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16

	lpm r16,Z+
	movw ZL,r20
	dec r18

	out _SFR_IO_ADDR(DATA_PORT),r16
	brne m1_loop

	;end set last pix to zero
	rjmp .
	clr r16
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16

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

;*****************************
; Defines a tile map
; C-callable
; r25:r24=pointer to tiles map
;*****************************
.section .text.SetTileMap
SetTileMap:
	//adiw r24,2
	sts tile_map_lo,r24
	sts tile_map_hi,r25

	ret

;***********************************
; LOAD Main map
;************************************
.section .text.LoadMap
LoadMap:
	push r16
	push r17
	//init vram

	ldi r24,lo8(VRAM_TILES_H *VRAM_TILES_V)
	ldi r25,hi8(VRAM_TILES_H *VRAM_TILES_V)
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)

	lds ZL,tile_map_lo
	lds ZH,tile_map_hi

	ldi r20,(TILE_WIDTH*TILE_HEIGHT) ;48

	lds r16,tile_table_lo
	lds r17,tile_table_hi

load_map_loop:
	lpm r22,Z+ ;16
	lpm r23,Z+ ;17

	mul r22,r20
	movw r18,r0
	mul r23,r20
	add r19,r0

	add r18,r16
	adc r19,r17

	st X+,r18	;store tile adress
	st X+,r19

	sbiw r24,1
	brne load_map_loop

	clr r1

	pop r17
	pop r16
	ret

;***********************************
; RESTORE TILE
; Copy a map tile # to the same position VRAM
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
;************************************
//.section text.RestoreTile
.section .text.RestoreTile
RestoreTile:
	clr r25
	clr r23
	clr r19
	ldi r18,VRAM_TILES_H*2
	mul r22,r18		;calculate Y line addr
	lsl r24
	add r0,r24		;add X offset
	adc r1,r19

	//load map tile #
	lds ZL,tile_map_lo
	lds ZH,tile_map_hi

	add ZL,r0
	adc ZH,r1
	lpm r20,Z+ 
	lpm r21,Z+ 

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	ldi r18,(TILE_WIDTH*TILE_HEIGHT)
	mul r20,r18
	movw r22,r0
	mul r21,r18
	add r23,r0

	lds r20,tile_table_lo
	lds r21,tile_table_hi

	add r22,r20
	adc r23,r21

	st X+,r22
	st X,r23

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
