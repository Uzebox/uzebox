/*
 *  Uzebox Kernel - Mode 9
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
; Video mode 9
; 360x240, tiles only, 60x28 tiles, 6x8 pixels tile
; Real-time code generated tile data
; 256 colors per pixel
; no scrolling, no sprites  
;***************************************************	

.global vram
.global codetiles_table
.global SetTile
.global ClearVram
.global SetFontTilesIndex
.global SetTileTable
.global SetTile
.global SetFont

.section .bss
	vram: 	  	.space VRAM_SIZE	;allocate space for the video memory (VRAM)
	tile_table_lo:	.byte 1
	tile_table_hi:	.byte 1
	font_tile_index:.byte 1 

.section .text

sub_video_mode9:

	;waste line to align with next hsync in render function
	WAIT r19,1348

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16
	clr r22

next_text_line:	
	rcall hsync_pulse ;3+144=147

	WAIT r19,249 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT

	;***draw line***
	call render_tile_line

	WAIT r19,48 - CENTER_ADJUSTMENT

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
; Y       = VRAM adress to draw from (must not be modified)
; 
; cycles  = 1495
;*************************************************
render_tile_line:
	push YL
	push YH	

   	ldi r18,lo8(pm(render_tile_line_end))
	ldi r19,hi8(pm(render_tile_line_end))
	ldi r21,FONT_TILE_SIZE ;size of a tile in words  
	ldi r23,FONT_TILE_WIDTH ;size of tile row in words

	;////////////////////////////////////////////
	;Compute the adress of the first tile to draw
	;////////////////////////////////////////////
	lds r24,tile_table_lo
	lds r25,tile_table_hi
   	lsr r25				;divide by 2 because we point to a program adress
	ror r24
	//ldi r24,lo8(pm(codetiles_table))
	//ldi r25,hi8(pm(codetiles_table))
	//nop
	//nop
	mul r22,r23			;compute Y offset in current tile row
	add r24,r0			;add to title table base adress	
	adc r25,r1

	ld	r20,Y+			;load tile index from VRAM
	mul r20,r21
	add r0,r24			;add tile table base address+offset
	adc r1,r25

	movw ZL,r0	 		;copy to Z, the register used by ijmp
   
    ldi r20,SCREEN_TILES_H ;tiles to render
	ijmp      ;jump to first codetile



render_tile_line_end:   
	;set black pixel
   	clr r16
   	out VIDEO_PORT,r16

	pop YH
	pop YL
	ret


  ;21 bytes/row*2*8=432 bytes per code tile

codetiles_table:   

;line 1
   ldi r16,1		;load pixel #1
   out VIDEO_PORT,r16
   ld r17,Y+      	;next tile in vram
   
   ldi r16,2		;load pixel #2
   out VIDEO_PORT,r16
   mul r17,r21		;multiply by tile size in words
   
   ldi r16,3		;load pixel #3
   out VIDEO_PORT,r16
   add r0,r24      	;add codetiles table addr+y offset
   adc r1,r25
     
   ldi r16,4		;load pixel #4
   out VIDEO_PORT,r16   
   movw ZL,r18      ;load return adress
   dec r20			;decrement tiles to draw on line
   
   ldi r16,5		;load pixel #5
   out VIDEO_PORT,r16
   breq .+2
   movw ZL,r0		;load next codetile adress
   
   ldi r16,6		;load pixel #6
   out VIDEO_PORT,r16
   ijmp

;line 2
line2:
   ldi r16,1		;load pixel #1
   out VIDEO_PORT,r16
   ld r17,Y+      	;next tile in vram
   
   ldi r16,2		;load pixel #2
   out VIDEO_PORT,r16
   mul r17,r21		;multiply by tile size in words
   
   ldi r16,3		;load pixel #3
   out VIDEO_PORT,r16
   add r0,r24      	;add codetiles table addr+y offset
   adc r1,r25
     
   ldi r16,4		;load pixel #4
   out VIDEO_PORT,r16   
   movw ZL,r18      ;load return adress
   dec r20			;decrement tiles to draw on line
   
   ldi r16,5		;load pixel #5
   out VIDEO_PORT,r16
   breq .+4
   movw ZL,r0		;load next codetile adress
   
   ldi r16,6		;load pixel #6
   out VIDEO_PORT,r16
   ijmp

;line 3
   ldi r16,1		;load pixel #1
   out VIDEO_PORT,r16
   ld r17,Y+      	;next tile in vram
   
   ldi r16,2		;load pixel #2
   out VIDEO_PORT,r16
   mul r17,r21		;multiply by tile size in words
   
   ldi r16,3		;load pixel #3
   out VIDEO_PORT,r16
   add r0,r24      	;add codetiles table addr+y offset
   adc r1,r25
     
   ldi r16,4		;load pixel #4
   out VIDEO_PORT,r16   
   movw ZL,r18      ;load return adress
   dec r20			;decrement tiles to draw on line
   
   ldi r16,5		;load pixel #5
   out VIDEO_PORT,r16
   breq .+4
   movw ZL,r0		;load next codetile adress
   
   ldi r16,6		;load pixel #6
   out VIDEO_PORT,r16
   ijmp

;line 4
   ldi r16,1		;load pixel #1
   out VIDEO_PORT,r16
   ld r17,Y+      	;next tile in vram
   
   ldi r16,2		;load pixel #2
   out VIDEO_PORT,r16
   mul r17,r21		;multiply by tile size in words
   
   ldi r16,3		;load pixel #3
   out VIDEO_PORT,r16
   add r0,r24      	;add codetiles table addr+y offset
   adc r1,r25
     
   ldi r16,4		;load pixel #4
   out VIDEO_PORT,r16   
   movw ZL,r18      ;load return adress
   dec r20			;decrement tiles to draw on line
   
   ldi r16,5		;load pixel #5
   out VIDEO_PORT,r16
   breq .+4
   movw ZL,r0		;load next codetile adress
   
   ldi r16,6		;load pixel #6
   out VIDEO_PORT,r16
   ijmp

;line 5
   ldi r16,1		;load pixel #1
   out VIDEO_PORT,r16
   ld r17,Y+      	;next tile in vram
   
   ldi r16,2		;load pixel #2
   out VIDEO_PORT,r16
   mul r17,r21		;multiply by tile size in words
   
   ldi r16,3		;load pixel #3
   out VIDEO_PORT,r16
   add r0,r24      	;add codetiles table addr+y offset
   adc r1,r25
     
   ldi r16,4		;load pixel #4
   out VIDEO_PORT,r16   
   movw ZL,r18      ;load return adress
   dec r20			;decrement tiles to draw on line
   
   ldi r16,5		;load pixel #5
   out VIDEO_PORT,r16
   breq .+4
   movw ZL,r0		;load next codetile adress
   
   ldi r16,6		;load pixel #6
   out VIDEO_PORT,r16
   ijmp

;line 6
   ldi r16,1		;load pixel #1
   out VIDEO_PORT,r16
   ld r17,Y+      	;next tile in vram
   
   ldi r16,2		;load pixel #2
   out VIDEO_PORT,r16
   mul r17,r21		;multiply by tile size in words
   
   ldi r16,3		;load pixel #3
   out VIDEO_PORT,r16
   add r0,r24      	;add codetiles table addr+y offset
   adc r1,r25
     
   ldi r16,4		;load pixel #4
   out VIDEO_PORT,r16   
   movw ZL,r18      ;load return adress
   dec r20			;decrement tiles to draw on line
   
   ldi r16,5		;load pixel #5
   out VIDEO_PORT,r16
   breq .+4
   movw ZL,r0		;load next codetile adress
   
   ldi r16,6		;load pixel #6
   out VIDEO_PORT,r16
   ijmp

;line 7
   ldi r16,1		;load pixel #1
   out VIDEO_PORT,r16
   ld r17,Y+      	;next tile in vram
   
   ldi r16,2		;load pixel #2
   out VIDEO_PORT,r16
   mul r17,r21		;multiply by tile size in words
   
   ldi r16,3		;load pixel #3
   out VIDEO_PORT,r16
   add r0,r24      	;add codetiles table addr+y offset
   adc r1,r25
     
   ldi r16,4		;load pixel #4
   out VIDEO_PORT,r16   
   movw ZL,r18      ;load return adress
   dec r20			;decrement tiles to draw on line
   
   ldi r16,5		;load pixel #5
   out VIDEO_PORT,r16
   breq .+4
   movw ZL,r0		;load next codetile adress
   
   ldi r16,6		;load pixel #6
   out VIDEO_PORT,r16
   ijmp

;line 8
   ldi r16,1		;load pixel #1
   out VIDEO_PORT,r16
   ld r17,Y+      	;next tile in vram
   
   ldi r16,2		;load pixel #2
   out VIDEO_PORT,r16
   mul r17,r21		;multiply by tile size in words
   
   ldi r16,3		;load pixel #3
   out VIDEO_PORT,r16
   add r0,r24      	;add codetiles table addr+y offset
   adc r1,r25
     
   ldi r16,4		;load pixel #4
   out VIDEO_PORT,r16   
   movw ZL,r18      ;load return adress
   dec r20			;decrement tiles to draw on line
   
   ldi r16,5		;load pixel #5
   out VIDEO_PORT,r16
   breq .+4
   movw ZL,r0		;load next codetile adress
   
   ldi r16,6		;load pixel #6
   out VIDEO_PORT,r16
   ijmp


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
