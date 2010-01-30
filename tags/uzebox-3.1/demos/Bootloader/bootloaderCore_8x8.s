/*
 *  Uzebox Bootloader Kernel
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
*/


#include <avr/io.h>

;Line rate timer delay: 15.73426 kHz*2 = 1820/2 = 910
;2x is to account for vsync equalization & serration pulse that are at 2x line rate


#define MIX_BANK_SIZE (253+9)
#define MIX_BUF_SIZE MIX_BANK_SIZE*2

#define HDRIVE_CL 1819
#define HDRIVE_CL_TWICE 909 
#define SYNC_HSYNC_PULSES 253 

#define SYNC_PRE_EQ_PULSES 6
#define SYNC_EQ_PULSES 6
#define SYNC_POST_EQ_PULSES 6

#define SYNC_PHASE_PRE_EQ	0
#define SYNC_PHASE_EQ		1
#define SYNC_PHASE_POST_EQ	2
#define SYNC_PHASE_HSYNC	3

#define SYNC_PIN PB0
#define SYNC_PORT PORTB
#define DATA_PORT PORTC

#define TILE_HEIGHT 8
#define TILE_WIDTH 8

#define VRAM_TILES_H 30 
#define VRAM_TILES_V 28
#define SCREEN_TILES_H 30
#define SCREEN_TILES_V 28

#define FIRST_RENDER_LINE 20
#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V*2

#define JOYPAD_OUT_PORT PORTA
#define JOYPAD_IN_PORT PINA
#define JOYPAD_CLOCK_PIN PA3
#define JOYPAD_LATCH_PIN PA2
#define JOYPAD_DATA1_PIN PA0
#define JOYPAD_DATA2_PIN PA1

;Public methods
.global TIMER1_COMPA_vect
.global SetFont
.global ClearVram
.global joypad_status
.global vram
.global vsync_flag

.section .bss
	//vram is organized as 16 bit per tile index: 
	//byte0=tile index,byte1=background color
	vram: 	  	.space VRAM_SIZE 
	fontram:	.space 64*8 ;8*8*30


	sync_phase:  .byte 1 ;0=pre-eq, 1=eq, 2=post-eq, 3=hsync, 4=vsync
	sync_pulse:	 .byte 1
	vsync_flag:	 .byte 1	;set 30 hz
	curr_field:	 .byte 1	;0 or 1, changes at 60hz

	;last read results of joypads	
	joypad_status:		.word 1


.section .init8
	call InitVideo

.section .text
	fontshade:  		.byte 91,164,246,255,246,164,91,82

	sync_func_vectors:	.word pm(do_pre_eq)
						.word pm(do_eq)
						.word pm(do_post_eq)
						.word pm(do_hsync)



;***************************************************
; Video Mode 1: Tiles-Only
; Process video frame in tile mode (40*28)
;***************************************************	

sub_video_mode1:

	;waste time to align with next hsync in render function ;1554
	ldi r24,lo8(390-32)
	ldi r25,hi8(390-32)
	sbiw r24,1
	brne .-4
	//rjmp .

	
	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	ldi r23,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)	
	clr r22 ;current tile line

next_tile_line:	
	rcall hsync_pulse 

	ldi r19,30
	dec r19			
	brne .-4

	;***Render scanline***
	call render_tile_line

	ldi r19,27 
	dec r19			
	brne .-4
	nop

	dec r23
	breq text_frame_end
	
	lpm	;3 nop
	inc r22

	cpi r22,8 ;last line of a tile? 1
	breq next_tile_row 
	
	;wait to align with next_tile_row instructions (+1 cycle for the breq)
	ldi r19,3
	dec r19
	brne .-4
	nop

	rjmp next_tile_line	

next_tile_row:
	clr r22		;current char line			;1	

	ldi r19,VRAM_TILES_H*2
	add YL,r19
	adc YH,r22

	lpm ;3 nop
	rjmp .

	rjmp next_tile_line

text_frame_end:

	ldi r19,5
	dec r19
	brne .-4
	rjmp .

	rcall hsync_pulse ;145

	;set vsync flag if beginning of next frame (each two fields)
	ldi r17,1
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
; Must preserve: r22,r23,Y
; 
;*************************************************

render_tile_line:

	;get shade color for current font line
	clr r16
	ldi ZL,lo8(fontshade)
	ldi ZH,hi8(fontshade)
	add ZL,r22
	adc ZH,r16
	lpm r4,Z

	;copy current VRAM position
	movw XL,YL

	ldi r21,TILE_HEIGHT ;TILE_WIDTH*TILE_HEIGHT

	;Pre-calculate tile table base+tile row offset
	ldi r16,1 ;TILE_WIDTH ;tile width in pixels
	mul r22,r16 
	ldi ZL,lo8(fontram)
	ldi ZH,hi8(fontram)
	add ZL,r0	;add tile row offset
	adc ZH,r1   ;add tile row offset
	
	movw r24,ZL	;save for use in loop

	;load the first tile from vram
	ld	r16,X+	;load next tile # from VRAM	
	mul r16,r21  ;calculate offset in tile table
	add ZL,r0	
	adc ZH,r1   

	ld r2,X+	;load next tile bg color
	ld r16,Z	;load first tile pixels

	;draw 30 tiles wide (8x8), 6 clocks/pixel -->1440
	ldi r17,30

	


m1_rtl_loop:
	mov r18,r2
	sbrc r16,7
	mov r18,r4
	out _SFR_IO_ADDR(DATA_PORT),r18	
	ld	r20,X+	;load next tile # from VRAM	
	
	mov r18,r2
	sbrc r16,6
	mov r18,r4
	out _SFR_IO_ADDR(DATA_PORT),r18	
	mul r20,r21 ;calculate offset in tile table
	
	mov r18,r2
	sbrc r16,5
	mov r18,r4
	out _SFR_IO_ADDR(DATA_PORT),r18	
	add r0,r24  ;add tile table base + tile row offset 	
	adc r1,r25  ;add tile table base + tile row offset 	
	
	mov r18,r2
	sbrc r16,4
	mov r18,r4
	out _SFR_IO_ADDR(DATA_PORT),r18	
	movw ZL,r0
	mov r15,r16
	
	mov r18,r2
	sbrc r15,3
	mov r18,r4
	out _SFR_IO_ADDR(DATA_PORT),r18	
	ld r16,Z	;load tile's row of pixels
	
	mov r18,r2
	sbrc r15,2
	mov r18,r4
	out _SFR_IO_ADDR(DATA_PORT),r18	
	ld r3,X+	;load next tile bg color
	
	mov r18,r2
	sbrc r15,1
	mov r18,r4
	out _SFR_IO_ADDR(DATA_PORT),r18		
	mov r18,r2
	sbrc r15,0

	mov r18,r4
	dec r17
	mov r2,r3
	out _SFR_IO_ADDR(DATA_PORT),r18
	brne m1_rtl_loop


	;end set last pix to zero
	lpm ;3 nops
	clr r16	
	out _SFR_IO_ADDR(DATA_PORT),r16

	ret



;************
; HSYNC
;************
do_hsync:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ; HDRIVE sync pulse low

	//call update_sound_buffer ;36 -> 63
	call wait63cycles

	ldi ZL,32-9
	dec ZL
	brne .-4;135

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;136

	//set_normal_rate_HDRIVE
	ldi ZL,hi8(HDRIVE_CL)
	sts _SFR_MEM_ADDR(OCR1AH),ZL	
	ldi ZL,lo8(HDRIVE_CL)
	sts _SFR_MEM_ADDR(OCR1AL),ZL


	ldi ZL,SYNC_PHASE_PRE_EQ
	ldi ZH,SYNC_PRE_EQ_PULSES

	rcall update_sync_phase

	sbrs ZL,0
	rcall render

	sbrs ZL,0
	rjmp not_start_of_frame


	sei ;must enable ints for hsync pulses
	call read_joypads

not_start_of_frame:


	ret


;*****************************************
; READ JOYPADS
; read 60 time/sec before redrawing screen
;*****************************************

read_joypads:
	push r24

	//latch data
	sbi _SFR_IO_ADDR(JOYPAD_OUT_PORT),JOYPAD_LATCH_PIN
	jmp . ; wait ~200ns
	jmp . ;(6 cycles)
	cbi _SFR_IO_ADDR(JOYPAD_OUT_PORT),JOYPAD_LATCH_PIN
	
	//clear button state bits
	clr ZL //P1
	clr ZH

	;wait 12 cycles
	ldi r24,4
	dec r24
	brne .-4

;29
	ldi r24,12 //number of buttons to fetch
read_joypads_loop:	
	;read data pin

	lsl ZL
	rol ZH
	sbic _SFR_IO_ADDR(JOYPAD_IN_PORT),JOYPAD_DATA1_PIN
	ori ZL,1

	;pulse clock pin
	sbi _SFR_IO_ADDR(JOYPAD_OUT_PORT),JOYPAD_CLOCK_PIN
	jmp . ;wait 6 cycles
	jmp .
	cbi _SFR_IO_ADDR(JOYPAD_OUT_PORT),JOYPAD_CLOCK_PIN

	jmp .
	jmp .
	jmp .
	jmp .

	dec r24
	brne read_joypads_loop ;232

	com ZL 
	com ZH

#if JOYSTICK == TYPE_NES
	;Do some bit transposition
	bst ZH,3
	bld ZL,3
	bst ZH,2
	bld ZH,3
	andi ZH,0b00001011
	andi ZL,0b11111000

#elif JOYSTICK == TYPE_SNES
	andi ZH,0b00001111
#endif 

	sts joypad_status,ZL
	sts joypad_status+1,ZH

	pop r24
	ret




;**** RENDER ****
render:
	push ZL

	lds ZL,sync_pulse
	cpi ZL,SYNC_HSYNC_PULSES-FIRST_RENDER_LINE
	brsh render_end
	cpi ZL,SYNC_HSYNC_PULSES-FIRST_RENDER_LINE-(SCREEN_TILES_V*TILE_HEIGHT)
	brlo render_end
	
	;push r1-r29
	ldi ZL,29
	clr ZH
push_loop:
	ld r0,Z	;load value from register file
	push r0
	dec ZL
	brne push_loop	

	call sub_video_mode1

	;pop r1-r29
	ldi ZL,1
	clr ZH
pop_loop:
	pop r0
	st Z+,r0 ;store value to register file
	cpi ZL,30
	brlo pop_loop	


render_end:
	pop ZL
	ret

;***************************************************************************
; Video sync interrupt
;***************************************************************************
TIMER1_COMPA_vect:
	push ZH;2
	push ZL;2

	;save flags & status register
	in ZL,_SFR_IO_ADDR(SREG);1
	push ZL ;2		

	;Read timer offset since rollover to remove cycles 
	;and conpensate for interrupt latency.
	;This is nessesary to eliminate frame jitter.

	lds ZL,_SFR_MEM_ADDR(TCNT1L)
	subi ZL,0x0e ;MIN_INT_LATENCY

	cpi ZL,1
	brlo .		;advance PC to next instruction

	cpi ZL,2
	brlo .		;advance PC to next instruction

	cpi ZL,3
	brlo .		;advance PC to next instruction

 	cpi ZL,4
	brlo .		;advance PC to next instruction

	cpi ZL,5
	brlo .		;advance PC to next instruction

	cpi ZL,6
	brlo .		;advance PC to next instruction

	cpi ZL,7
	brlo .		;advance PC to next instruction
	
	cpi ZL,8
	brlo .		;advance PC to next instruction

	cpi ZL,9
	brlo .		;advance PC to next instruction

	rcall sync


	pop ZL
	out _SFR_IO_ADDR(SREG),ZL
	pop ZL
	pop ZH
	reti


;***************************************************
; Composite SYNC
;***************************************************

sync:
	push r0
	push r1
			
	ldi ZL,lo8(sync_func_vectors)	
	ldi ZH,hi8(sync_func_vectors)

	lds r0,sync_phase
	lsl r0 ;*2
	clr r1
	
	add ZL,r0
	adc ZH,r1
	
	lpm r0,Z+
	lpm r1,Z
	movw ZL,r0

	icall	;call sync functions

	pop r1
	pop r0
	ret


;*************************************************
; Generate a H-Sync pulse - 136 clocks (4.749us)
; Note: TCNT1 should be equal to 
; 0x44 on the cbi 
; 0xcc on the sbi 
;
; Cycles: 144
; Destroys: ZL (r30)
;*************************************************
hsync_pulse:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2
	
	//call update_sound_buffer ;36 -> 63
	call wait63cycles
	
	ldi ZL,30-9
	dec ZL 
	brne .-4;92


	lds ZL,sync_pulse
	dec ZL
	sts sync_pulse,ZL

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2

	rjmp .

	ret


;**************************
; PRE_EQ pulse
; Note: TCNT1 should be equal to 
; 0x44 on the cbi
; 0x88 on the sbi
; pulse duration: 68 clocks
;**************************
do_pre_eq:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ; HDRIVE sync pulse low

	//call update_sound_buffer_2 ;36 -> 63
	call wait63cycles

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;68
	;nop

	ldi ZL,SYNC_PHASE_EQ
	ldi ZH,SYNC_EQ_PULSES
	rcall update_sync_phase

	//set_double_rate_HDRIVE
	ldi ZL,hi8(HDRIVE_CL_TWICE)
	sts _SFR_MEM_ADDR(OCR1AH),ZL	
	ldi ZL,lo8(HDRIVE_CL_TWICE)
	sts _SFR_MEM_ADDR(OCR1AL),ZL

	ret

;************
; Serration EQ
; Note: TCNT1 should be equal to 
; 0x44  on the cbi
; 0x34A on the sbi
; low pulse duration: 774 clocks
;************
do_eq:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ; HDRIVE sync pulse low

	//call update_sound_buffer_2 ;36 -> 63
	call wait63cycles

	ldi ZL,181-9+4
do_eq_delay:
	nop
	dec ZL
	brne do_eq_delay ;135

	rjmp .

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;136

	ldi ZH,SYNC_POST_EQ_PULSES
	ldi ZL,SYNC_PHASE_POST_EQ
	rcall update_sync_phase
		
	ret

;************
; POST_EQ
; Note: TCNT1 should be equal to 
; 0x44 on the cbi
; 0x88 on the sbi
; pulse cycles: 68 clocks
;************
do_post_eq:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ; HDRIVE sync pulse low

	//call update_sound_buffer_2 ;36 -> 63
	call wait63cycles

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;68

	nop

	ldi ZL,SYNC_PHASE_HSYNC
	ldi ZH,SYNC_HSYNC_PULSES
	rcall update_sync_phase

	ret

;*******
;delay loop used in sync subs
;*******
wait63cycles:
	;output mute sound byte to keep the
	;emulator at the good speed
	ldi ZL,0x80
	sts _SFR_MEM_ADDR(OCR2A),ZL 
	
	ldi ZL,18
	dec ZL
	brne .-4
	rjmp .

	ret




;************
; update sync phase
; ZL=next phase #
; ZH=next phases's pulse count
;
; returns: ZL: 0=more pulses in phase
;              1=was the last pulse in phase
;***********
update_sync_phase:

	lds r0,sync_pulse
	dec r0
	lds r1,_SFR_MEM_ADDR(SREG)
	sbrc r1,SREG_Z
	mov r0,ZH
	sts sync_pulse,r0	;set pulses for next sync phase

	lds r0,sync_phase
	sbrc r1,SREG_Z
	mov r0,ZL			;go to next phase 
	sts sync_phase,r0
	
	ldi ZL,0
	sbrc r1,SREG_Z
	ldi ZL,1

	ret


;***********************************
; CLEAR VRAM
; Fill the screen with the specified tile
; C-callable
;************************************
ClearVram:
	//init vram		
	ldi r30,lo8(VRAM_TILES_H*VRAM_TILES_V)
	ldi r31,hi8(VRAM_TILES_H*VRAM_TILES_V)

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)

	clr r1

fill_vram_loop:
	st X+,r1
	sbiw r30,1
	brne fill_vram_loop

	ret


;***********************************
; SET FONT TILE
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; r20=Font tile No (8 bit)
; r18=Font background color
;************************************
SetFont:
	clr r25

	ldi r17,VRAM_TILES_H*2

	mul r22,r17		;calculate Y line addr in vram
	lsl r24
	add r0,r24		;add X offset
	adc r1,r25
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	subi r20,32
	st X+,r20
	st X,r18

	clr r1

	ret

/*
init_table:
	.byte TCCR1B,0
	.byte TCCR0B,0
	.byte DDRC,0xff
	.byte DDRB,0xff
	.byte DDRD,0x80
	.byte DDRA,SYNC_PHASE_PRE_EQ
*/

;****************************************************
; INITIALIZATION
;****************************************************
InitVideo:

	;setup timer 1 to generate interrupts on 
	;twice the line rate.
	
	cli

	;stop timers
	clr r20
	sts _SFR_MEM_ADDR(TCCR1B),r20
	sts _SFR_MEM_ADDR(TCCR0B),r20
	
	//*** Set params *****

	;set port directions
	ldi r20,0xff
	out _SFR_IO_ADDR(DDRC),r20 ;video dac
	out _SFR_IO_ADDR(DDRB),r20 ;h-sync for ad725

	ldi r20,0x80
	out _SFR_IO_ADDR(DDRD),r20 ;audio-out, midi-in

	;setup port A for joypads
	ldi r20,_SFR_IO_ADDR(DDRA)
	andi r20,0b11111100 ;data in 
	ori  r20,0b00001100 ;control lines
	out _SFR_IO_ADDR(DDRA),r20
	cbi _SFR_IO_ADDR(PORTA),PA2
	cbi _SFR_IO_ADDR(PORTA),PA3


	;set sync parameters
	;starts at odd field, in pre-eq pulses, line 1
	
	ldi r16,SYNC_PHASE_PRE_EQ
	sts sync_phase,r16
	ldi r16,SYNC_PRE_EQ_PULSES
	sts sync_pulse,r16


	;clear counters
	sts _SFR_MEM_ADDR(TCNT1H),r20
	sts _SFR_MEM_ADDR(TCNT1L),r20	


	;set sync generator counter on TIMER1
	ldi r20,hi8(HDRIVE_CL_TWICE)
	sts _SFR_MEM_ADDR(OCR1AH),r20
	
	ldi r20,lo8(HDRIVE_CL_TWICE)
	sts _SFR_MEM_ADDR(OCR1AL),r20


	ldi r20,(1<<WGM12)+(1<<CS10) 
	sts _SFR_MEM_ADDR(TCCR1B),r20;CTC mode, use OCR1A for match

	ldi r20,(1<<OCIE1A)
	sts _SFR_MEM_ADDR(TIMSK1),r20 ;generate interrupt on match



	;set clock divider counter for AD725 on TIMER0
	;outputs 14.31818Mhz (4FSC)
	ldi r20,(1<<COM0A0)+(1<<WGM01)  ;toggle on compare match + CTC
	sts _SFR_MEM_ADDR(TCCR0A),r20

	ldi r20,0
	sts _SFR_MEM_ADDR(OCR0A),r20	;divide main clock by 2

	ldi r20,(1<<CS00)  ;enable timer, no pre-scaler
	nop
	sts _SFR_MEM_ADDR(TCCR0B),r20


	;set sound PWM on TIMER2
	ldi r20,(1<<COM2A1)+(1<<WGM21)+(1<<WGM20)   ;Fast PWM	
	sts _SFR_MEM_ADDR(TCCR2A),r20

	ldi r20,0
	sts _SFR_MEM_ADDR(OCR2A),r20	;duty cycle (amplitude)

	ldi r20,(1<<CS20)  ;enable timer, no pre-scaler
	sts _SFR_MEM_ADDR(TCCR2B),r20

	ldi r20,(1<<SYNC_PIN)
	out _SFR_IO_ADDR(SYNC_PORT),r20	;set sync line to hi


	;copy font in RAM
	ldi ZL,lo8(fonts)
	ldi ZH,hi8(fonts)
	ldi XL,lo8(fontram)
	ldi XH,hi8(fontram)

	ldi r24,lo8(64*8)
	ldi r25,hi8(64*8)
il1:
	lpm r0,Z+
	st X+,r0
	sbiw r24,1
	brne il1


	sei

	clr r1
	ret


fonts:
;font size=512 bytes
#include "fonts8x8.pic.inc" 

