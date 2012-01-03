/*
 *  Uzebox Kernel - Mode 3
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
; Video Mode 3: 28x28 (224x224 pixels) using 8x8 tiles
; with overlay & sprites X flipping.
;
; If the SCROLLING build parameter=0, the scrolling 
; code is removed and the screen resolution 
; increase to 30*28 tiles.
; 
;***************************************************	

.global vram
.global codetiles_table


.section .bss
	vram: 	  	.space VRAM_SIZE	;allocate space for the video memory (VRAM)


.section .text

sub_video_mode9:

	;waste line to align with next hsync in render function
	ldi ZL,222-15-1
mode1_render_delay:
	rjmp .
	rjmp .
	dec ZL
	brne mode1_render_delay 
	nop


	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16
	clr r22




next_text_line:	
	rcall hsync_pulse ;3+144=147

	ldi r19,45 - 4  + CENTER_ADJUSTMENT
	dec r19			
	brne .-4
	

	;***draw line***
	call render_tile_line

	ldi r19,12 + 4 - CENTER_ADJUSTMENT
	dec r19			
	brne .-4
	nop


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


	;set vsync flag if beginning of next frame (each two fields)
	ldi r17,1
	lds r16,curr_field
	eor r16,r17
	sts curr_field,r16

	
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




/*
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
*/


