;***************************************************
; Video Mode 7
; 170x120
; Movie player
;***************************************************

#define LINE_BUFFER_SIZE 170
#define LINES_PER_SECTOR 6

.global load_sound_buffer
.global render_start
.global playback_start
.global StartPlayback
.global StopPlayback
.global InitializeVideoMode
.global DisplayLogo
.global VideoModeVsync
.global vram

.section .bss
	
	vram: 	  		.space 1
	line_buffer:	.space LINE_BUFFER_SIZE*2
	render_start:	.space 1
	playback_start:	.space 1


.section .text

//*** Main routine ****
sub_video_mode7:

	;waste line to align with next hsync in render function
	ldi ZL,222-170-2-25+9
mode7_render_delay:
	lpm
	nop
	dec ZL
	brne mode7_render_delay




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

	clr r21			;first frame sector
	ldi r22,224+4	;decremental line counter
	clr r23			;incremental line counter
	ldi r24,LINES_PER_SECTOR 	;lines per SD sector

	ser r20
	clr r6

	lds r18,render_start
	cpi r18,1
	brne next_line_wait

next_line:
	;trigger 1st line pixel read
	out _SFR_IO_ADDR(SPDR),r20

	;***First Line***
	rcall hsync_pulse ;3+144=147

	;draw line
	eor r25,r13	;add half line offset
	rcall render_tile_line

	inc r23
	dec r22
	brne next_line
	rjmp end_frame

next_line_wait:
	rcall hsync_pulse ;3+144=147
	ldi r24,lo8(1670/4)
	ldi r25,hi8(1670/4)
wait:
	sbiw r24,1
	brne wait
	rjmp .

	dec r22
	brne next_line_wait
	
end_frame:
	;frame rendering finished
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
; RENDER SCANLINE
;
; r22     = Lines remaining
; r23 	  = current line
; r24     = sector lines left
; r25     = next line buffer displacement
;
; cycles  = 1679
;*************************************************

render_tile_line:


	;current buffer
	ldi XL,lo8(line_buffer)
	ldi XH,hi8(line_buffer)
	movw YL,XL

	sbrc r23,1
	add XL,r14
	sbrc r23,1
	adc XH,r15

	;next line buffer
	sbrs r23,1
	add YL,r14
	sbrs r23,1
	adc YH,r15
	
	;add half line offset each other line
	add YL,r25
	adc YH,r6
		
	ldi r18,84
loop:
	nop
	;in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r17,_SFR_IO_ADDR(SPDR) ;read next pixel
	out _SFR_IO_ADDR(SPDR),r20
	ld r19,X+
	out _SFR_IO_ADDR(DATA_PORT),r19
	st Y+,r17
	rjmp .
	rjmp .
	ld r19,X+
	out _SFR_IO_ADDR(DATA_PORT),r19
	dec r18
	brne loop ;18*84=1512


	;write last line pixel
	;in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	nop
	in r17,_SFR_IO_ADDR(SPDR) ;read next pixel
	st Y+,r17

	out _SFR_IO_ADDR(DATA_PORT),r6

	dec r24
	breq next_sector
	
	
	
	ldi r19,42
	dec r19
	brne .-4 
	rjmp .
	
	ret 
	;1660

next_sector:

	;-read the two last sector's bytes
	;-2 CRC bytes
	;-wait 8 clocks
	;-read next data token (0xfe)
	ldi r19,6

read_blocks:
	;read first  CRC byte
	out _SFR_IO_ADDR(SPDR),r20
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	dec r19
	;in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	nop
	in r17,_SFR_IO_ADDR(SPDR) ;read crc
	brne read_blocks ;21

	ldi r24,LINES_PER_SECTOR

	ret
	;1660

;********************************
; Can destroy r18-r27 and r30-r31
;********************************
load_sound_buffer:

	;set target bank adress
	lds r18,mix_bank
	tst r18
	breq lset_hi_bank
	ldi ZL,lo8(mix_buf)
	ldi ZH,hi8(mix_buf)
	rjmp lend_set_bank
lset_hi_bank:
	ldi ZL,lo8(mix_buf+MIX_BANK_SIZE)
	ldi ZH,hi8(mix_buf+MIX_BANK_SIZE)
lend_set_bank:


	ldi r24,lo8(262)
	ldi r25,hi8(262)
	ser r20
lsb_loop:
	out _SFR_IO_ADDR(SPDR),r20
	ldi r18,4
	dec r18
	brne .-4 ;wait 12 cycles
	sbiw r24,1
	nop
	in r19,_SFR_IO_ADDR(SPSR) ;clear flag
	in r19,_SFR_IO_ADDR(SPDR) ;read crc
	st Z+,r19
	brne lsb_loop ;22



	;-2 CRC bytes
	;-wait 8 clocks
	;-read next data token (0xfe)
	ldi r19,254

read_filler:
	;read first  CRC byte
	out _SFR_IO_ADDR(SPDR),r20
	ldi r18,5
	dec r18
	brne .-4 ;wait 15 cycles
	dec r19
	in r17,_SFR_IO_ADDR(SPSR) ;clear flag
	in r18,_SFR_IO_ADDR(SPDR) ;read crc
	brne read_filler ;21


	ldi r18,1
	sts render_start,r18

	ret


StartPlayback:
	ldi r24,1
	sts playback_start,r24
	ret

StopPlayback:
	ldi r24,0
	sts render_start,r24
	sts playback_start,r24
	ret


;No logo for this mode
DisplayLogo:
	ret


VideoModeVsync:

	lds r24,playback_start
	cpi r24,0
	breq skip
	rcall load_sound_buffer
skip:
	ret


InitializeVideoMode:

	ldi XL,lo8(line_buffer)
	ldi XH,hi8(line_buffer)
	ldi r24,64*2
	ldi r25,0xff
m7_init_loop:
	st X+,r25
	dec r24
	brne m7_init_loop

	sts render_start,r1
	sts playback_start,r1

	ret
