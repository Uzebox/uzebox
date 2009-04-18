;***************************************************
; Video Mode 7
; 170x120
; Movie player
;***************************************************

#define LINE_BUFFER_SIZE 64
#define LINES_PER_SECTOR 16

.section .bss

	line_buffer:	.space LINE_BUFFER_SIZE*2
	

.section .text

//*** Main routine ****
sub_video_mode7:

	;waste line to align with next hsync in render function
	ldi ZL,222-64-1
mode7_render_delay:
	lpm
	nop
	dec ZL
	brne mode7_render_delay

	lpm
	lpm

	;clear the line buffer
	ldi r16,LINE_BUFFER_SIZE
	clr r17
	ldi XL,lo8(line_buffer)
	ldi XH,hi8(line_buffer)
clr_loop:
	st X+,r17
	st X+,r17
	dec r16
	brne clr_loop


	ldi r16,LINE_BUFFER_SIZE/2
	mov r25,r16
	mov r13,r16

	ldi r16,lo8(LINE_BUFFER_SIZE)
	ldi r17,hi8(LINE_BUFFER_SIZE)
	movw r14,r16

	clr r21		;first frame sector
	ldi r22,224-8 	;decremental line counter
	clr r23		;incremental line counter
	ldi r24,LINES_PER_SECTOR 	;lines per SD sector


next_line:
	;***First Line***
	rcall hsync_pulse ;3+144=147

	ldi r18,30+57-14+4
	dec r18
	brne .-4
	nop

	;draw line
	eor r25,r13	;add half line offset
	call render_tile_line

	inc r23
	dec r22
	brne next_line

/*
next_line:
	;***First Line***

	rcall hsync_pulse ;3+144=147

	ldi r18,30+57-14+5
	dec r18
	brne .-4
	nop

	;draw line
	ldi r25,0
	call render_tile_line
	nop

	


	;*** Second Line ***

	rcall hsync_pulse ;3+144=147
	
	ldi r18,30+57-14+4
	dec r18
	brne .-4
	nop

	;draw line
	ldi r25,LINE_BUFFER_SIZE/2
	call render_tile_line




	inc r23
	dec r22
	brne next_line
*/


	nop
	rcall hsync_pulse ;145

	;set vsync flag if beginning of next frame (each two fields)
	ldi r17,1
	lds r16,curr_field
	eor r16,r17
	sts curr_field,r16

	lds r18,curr_frame
	tst r16
	breq .+2
	eor r18,r17
	sts curr_frame,r18

	;set vsync flag if beginning of next frame
	ldi ZL,1
	sts vsync_flag,ZL

	;clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL



	ret




;*************************************************
; RENDER TILE LINE
;
; r22     = Lines remaining
; r23 	  = current line
; r24     = sector lines left
; r25     = next line buffer displacement
;
; cycles  = 1476
;*************************************************

render_tile_line:

	;current buffer
	ldi XL,lo8(line_buffer)
	ldi XH,hi8(line_buffer)
	sbrc r23,1
	add XL,r14
	sbrc r23,1
	adc XH,r15


	;next line buffer
	ldi YL,lo8(line_buffer)
	ldi YH,hi8(line_buffer)
	sbrs r23,1
	add YL,r14
	sbrs r23,1
	adc YH,r15
	
	rjmp .
	
		

	;add half line offset each other line
	clr r0
	add YL,r25
	adc YH,r0
	
		

	ser r16
	;16

	ldi r18,32
loop:
	out _SFR_IO_ADDR(SPDR),r16
	ld r19,X+
	out _SFR_IO_ADDR(DATA_PORT),r19
	
	rjmp .
	rjmp .
	rjmp .
	rjmp .
	rjmp .
	rjmp .
	rjmp .
		
	ld r19,X+
	out _SFR_IO_ADDR(DATA_PORT),r19
	dec r18	
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read next pixel
	st Y+,r17
	rjmp .
	rjmp .
	rjmp .
	brne loop ;34*32=1088

	clr r20
	out _SFR_IO_ADDR(DATA_PORT),r20

	ldi r19,106-30
	dec r19
	brne .-4
	

/*
	ldi r18,64
loop:
	out _SFR_IO_ADDR(SPDR),r16
	ld r19,X+
	out _SFR_IO_ADDR(DATA_PORT),r19
	
	rjmp .
	rjmp .
	rjmp .
	rjmp .
	
	
	ld r19,X+
	out _SFR_IO_ADDR(DATA_PORT),r19
	dec r18	
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read next pixel
	st Z+,r17
	brne loop ;1408+16

	clr r20
	out _SFR_IO_ADDR(DATA_PORT),r20
*/





	dec r24
	breq next_sector

	
	ldi r19,16+14
	dec r19
	brne .-4 ;48

	;48+1408+16
	
	ret ;+4=1476
	
next_sector:


	;read first  CRC byte
	out _SFR_IO_ADDR(SPDR),r16
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	nop
	rjmp .
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read crc

	;read second CRC byte
	out _SFR_IO_ADDR(SPDR),r16	
	nop
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	nop
	rjmp .
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read crc
	;44


	;wait 8 clocks
	out _SFR_IO_ADDR(SPDR),r16
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	nop
	rjmp .
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read crc

	;read next data token (0xfe)
	out _SFR_IO_ADDR(SPDR),r16	
	nop
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	nop
	rjmp .
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read crc



	ldi r24,LINES_PER_SECTOR
	inc r21;ldi r21,1
	nop
	

	ret
	

	
	
	


/*	
;*************************************************
; RENDER TILE LINE
;
; r22     = Lines remaining
; r23 	  = current line
; r24     = sector lines left
; r25       = next line buffer displacement
;
; cycles  = 1629
;*************************************************


render_tile_line:

	;current buffer
	ldi XL,lo8(line_buffer)
	ldi XH,hi8(line_buffer)
	sbrc r23,1
	subi XL,~LINE_BUFFER_SIZE
	sbrc r23,1
	sbci XH,~0
	
	;next line buffer
	ldi ZL,lo8(line_buffer)
	ldi ZH,hi8(line_buffer)
	sbrs r23,1
	subi ZL,~LINE_BUFFER_SIZE
	sbrs r23,1
	sbci ZH,~0
	;add half line offset
	clr r0
	add ZL,r25
	adc ZH,0
	
	ser r16
	;16
	
	;read first byte
	out _SFR_IO_ADDR(SPDR),r16
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	lpm

	ldi r18,170/2 ;85
loop:
	out _SFR_IO_ADDR(SPDR),r16
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read next pixel
	ld r19,X+
	out _SFR_IO_ADDR(DATA_PORT),r19
	st Z+,r17
	rjmp .
	rjmp .
	ld r19,X+
	out _SFR_IO_ADDR(DATA_PORT),r19
	dec r18
	brne loop ;18*85=1530
	;1530+16=1546

	clr r20
	out _SFR_IO_ADDR(DATA_PORT),r20

	dec r24
	breq next_sector
	;2
	
	ldi r19,27
	dec r19
	brne .-4

	;77+1546+2=1625
	
	ret ;+4=1629
	
next_sector:
	;read byte 511
	out _SFR_IO_ADDR(SPDR),r16
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	nop
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read & discard next pixel

	;read byte 512
	out _SFR_IO_ADDR(SPDR),r16
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	nop
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read & discard next pixel	
	
	;read first  CRC byte
	out _SFR_IO_ADDR(SPDR),r16
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	nop
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read next pixel

	;read second CRC byte
	out _SFR_IO_ADDR(SPDR),r16
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	nop
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read next pixel	
	;76

	ldi r24,3
	lpm 

	ret
	
	
*/
	
	
	/*

;*************************************************
; RENDER TILE LINE 64x224 - direct read
;
; r22     = Lines remaining
; r23 	  = current line
; r24     = sector lines left
; r25       = next line buffer displacement
;
; cycles  = 1476
;*************************************************

render_tile_line:


	ldi r19,5
	dec r19
	brne .-4

	ser r16
	;16
	
	ldi r18,64
loop:
	out _SFR_IO_ADDR(SPDR),r16

	lpm
	lpm
	lpm
	lpm
	lpm
	
	dec r18	
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read next pixel
	out _SFR_IO_ADDR(DATA_PORT),r17
	brne loop ;1408+16

	clr r20
	out _SFR_IO_ADDR(DATA_PORT),r20

	dec r24
	breq next_sector
	;2
	
	ldi r19,16
	dec r19
	brne .-4 ;48

	;48+1408+16
	
	ret ;+4=1476
	
next_sector:

	;read first  CRC byte
	out _SFR_IO_ADDR(SPDR),r16
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	nop
	rjmp .
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read next pixel

	;read second CRC byte
	out _SFR_IO_ADDR(SPDR),r16
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	nop
	rjmp .
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read next pixel	
	;44

	ldi r24,8
	lpm 
	ldi r21,1

	ret
*/
