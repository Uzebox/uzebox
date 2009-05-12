
;***************************************************
; Video Mode 6
; 360x224
; Monochrome, uses blue channel Msbit
; Use Ram tiles 
;***************************************************	

.section .bss
	cyc: .byte 1

.section .text
sub_video_mode6:

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
	lds r4,cyc
	inc r4
	sts cyc,r4
	clr r5

next_text_line:	
	rcall hsync_pulse ;3+144=147

	ldi r19,37 + CENTER_ADJUSTMENT
	dec r19			
	brne .-4

	;***draw line***
	call render_tile_line

	ldi r19,26 - CENTER_ADJUSTMENT -1 -2
	dec r19			
	brne .-4
	

	inc r4
	inc r5

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
	

text_end2:

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
; RENDER TILE LINE
;
; r22     = Y offset in tiles
; Y       = VRAM adress to draw from (must not be modified)
;
; Must preserve: r10,r22,Y
; 
; cycles  = 1495
;*************************************************
render_tile_line:

	movw XL,YL

	ldi r23,8	;bytes per tile

	;compute ramtiles table base adress
	clr r0
	ldi r24,lo8(ram_tiles)
	ldi r25,hi8(ram_tiles)
	add r24,r22				;add Y offset in tiles
	adc r25,r0

	;load the first 8 pixels
	ld	r20,X+	;load tile index from VRAM
	mul r20,r23
	movw ZL,r24
	add ZL,r0
	adc ZH,r1
	ld r17,Z


	;draw 45 tiles wide @ 4 clocks/pixel
	ldi r18,SCREEN_TILES_H
	;ldi r20,0 ;background color
	mov r20,r4
	ldi r21,0xff ;color


m6_loop:
	mov r2,r20
	sbrc r17,7
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	ld r16,X+		;load next tile

	mov r2,r20
	sbrc r17,6
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	mul r16,23

	mov r2,r20
	sbrc r17,5
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	add r0,r24
	adc r1,r25

	mov r2,r20
	sbrc r17,4
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	movw ZL,r0
	mov r19,r17

	mov r2,r20
	sbrc r19,3
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	ld r17,Z			;load next 8 pixels

	mov r2,r20
	sbrc r19,2
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	rjmp .	

	mov r2,r20
	sbrc r19,1
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	nop
	dec r18
	
	mov r2,r20
	sbrc r19,0
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	brne m6_loop

	//clear last pixel
	lpm 
	clr r0
	out _SFR_IO_ADDR(DATA_PORT),r0


	ret

/*
render_tile_line:

	movw XL,YL

	ldi r23,8	;bytes per tile

	;compute ramtiles table base adress
	clr r0
	ldi r24,lo8(ram_tiles)
	ldi r25,hi8(ram_tiles)
	add r24,r22				;add Y offset in tiles
	adc r25,r0

	;load the first 8 pixels
	ld	r20,X+	;load tile index from VRAM
	mul r20,r23
	movw ZL,r24
	add ZL,r0
	adc ZH,r1
	ld r17,Z


	;draw 45 tiles wide @ 4 clocks/pixel
	ldi r18,SCREEN_TILES_H

m6_loop:	
	out _SFR_IO_ADDR(DATA_PORT),r17
	nop
	rol r17	
	ld r16,X+		;load next tile

	out _SFR_IO_ADDR(DATA_PORT),r17
	nop
	rol r17	
	mul r16,23

	out _SFR_IO_ADDR(DATA_PORT),r17
	nop
	rol r17	
	add r0,r24
	adc r1,r25

	out _SFR_IO_ADDR(DATA_PORT),r17
	nop
	rol r17	
	movw ZL,r0
	nop

	out _SFR_IO_ADDR(DATA_PORT),r17
	nop
	rol r17
	ld r0,Z			;load next 8 pixels

	out _SFR_IO_ADDR(DATA_PORT),r17
	nop
	rol r17
	rjmp .	

	out _SFR_IO_ADDR(DATA_PORT),r17
	nop
	rol r17	
	nop
	dec r18
	
	out _SFR_IO_ADDR(DATA_PORT),r17
	ldi r19,0
	mov r17,r0
	brne m6_loop

	out _SFR_IO_ADDR(DATA_PORT),r19


	ret


*/
