 ;***************************************************
; TEXT MODE VIDEO PROCESSING
; Process video frame in tile mode (30*28)
;***************************************************	
.global PutPixel
.global vram
.global palette

.section .bss
	.align 2		
	vram: 	  				.space VRAM_SIZE
	palette:				.space 4	;palette must be aligned to 4 bytes

.section .text

sub_video_mode8:

	;wait 873 cycles
	ldi r26,lo8(390)
	ldi r27,hi8(390)
	sbiw r26,1
	brne .-4		
	nop
	nop


	ldi YL,lo8(vram)
	ldi YH,hi8(vram)
	//ldi r20,SCREEN_HEIGHT*2
	clr r20

;*************************************************************
; Rendering main loop starts here
;*************************************************************
next_scan_line:	
	rcall hsync_pulse 

	ldi r19,68 + CENTER_ADJUSTMENT
	dec r19			
	brne .-4
	nop
	nop

	;***draw line***
	rcall render_tile_line

	ldi r19,39 - CENTER_ADJUSTMENT
	dec r19			
	brne .-4
	nop


	;duplicate each line
	sbrc r20,0
	subi YL,lo8(-(SCREEN_WIDTH/4))
	sbrc r20,0
	sbci YH,hi8(-(SCREEN_WIDTH/4))

	inc r20
	cpi r20,(SCREEN_HEIGHT*2)
	brne next_scan_line


	rcall hsync_pulse ;145
	
	;set vsync flag if beginning of next frame
	ldi ZL,1
	sts vsync_flag,ZL
			
	;clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	clr r1

	ret


;*************************************************
; RENDER TILE LINE
;
; r20     = render line counter (incrementing)
; Y       = VRAM adress to draw from (must not be modified)
;
; Must preserve r20,Y
; 
; cycles  = 1495
;*************************************************
render_tile_line:
	movw r22,YL ;push

	ldi r19,lo8(palette)
	ldi XH,hi8(palette)

	;11 cycles per pixel
	ldi r18,SCREEN_WIDTH/4
	ld r16,Y+ ;load next 4 pixels
main_loop:		
	mov XL,r16
	swap XL
	lsr XL
	lsr XL
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 0
	rjmp .
	nop
	mov XL,r16
	swap XL
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	nop

	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 1	
	nop
	rjmp .
	mov XL,r16
	lsr XL
	lsr XL
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 2
	rjmp .
	mov XL,r16
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	ld r16,Y+	;load next 4 pixels
	dec r18

	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 3
	brne main_loop

	lpm
	lpm
	clr r0
	rjmp .
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 3


	movw YL,r22 ;pop
	ret


;********************
;r25:r24=X
;r23:r22=Y
;r21:r20=Color
;********************
//PutPixel:
	
	;get pixel's byte adress
	ldi ZL,lo8(vram)
	ldi ZH,hi8(vram)
	ldi r21,0x3 ;mask
	ldi r18,(SCREEN_WIDTH/4)
	mul r18,r22
	add ZL,r0
	adc ZH,r1
	clr r1
	mov r25,r24
	andi r25,0x3 ;offset in byte
	lsr r24
	lsr r24
	add ZL,r24
	adc ZH,r1
	ld r18,Z	;load current byte (4 pixels)
	
	mul r21,r25 ;shift mask
	com r0
	and r18,r0	;mask previous pixel
	mul r20,r25 ;shift pixel
	or r18,r0
	st Z,r18

	clr r1
	ret

/*

main_loop:		
	ld r16,Y+
	mov XL,r16
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 0
	rjmp .
	nop

	mov XL,r16
	lsr XL
	lsr XL
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 1
	rjmp .
	rjmp .

	mov XL,r16
	swap XL
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 2
	rjmp .

	mov XL,r16
	swap XL
	lsr XL
	lsr XL
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 3

	dec r18
	brne main_loop
	*/






