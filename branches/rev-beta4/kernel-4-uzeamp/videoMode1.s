
;***************************************************
; TEXT MODE VIDEO PROCESSING
; Process video frame in tile mode (40*28)
;***************************************************	

sub_video_mode1:

	;waste line to align with next hsync in render function
	ldi ZL,222 
mode0_render_delay:
	lpm
	nop
	dec ZL
	brne mode0_render_delay 

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
	

text_end2:

	;set vsync flag if beginning of next frame (each two fields)
	ldi r17,1
	lds r16,curr_field
	eor r16,r17
	sts curr_field,r16
	sbrs r16,0
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

m0_loop:	
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
	brne m0_loop

	;end set last pix to zero
	rjmp .
	clr r16
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16

	ret