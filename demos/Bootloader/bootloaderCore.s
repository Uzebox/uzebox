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

#define VIDEOCE_PIN PB4 //Pin used to enable the AD723 on the Fuzebox & AVCore
#define AUDIO_OUT_PIN PD7
#define LED_PIN PD4
#define RESET_SWITCH_PIN PD2

#define TILE_HEIGHT 8
#define TILE_WIDTH 6

#define VRAM_TILES_H 40 
#define VRAM_TILES_V 28
#define SCREEN_TILES_H 40
#define SCREEN_TILES_V 28

#define FIRST_RENDER_LINE 20
#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V*2

#define JOYPAD_OUT_PORT PORTA
#define JOYPAD_IN_PORT PINA
#define JOYPAD_CLOCK_PIN PA3
#define JOYPAD_LATCH_PIN PA2
#define JOYPAD_DATA1_PIN PA0
#define JOYPAD_DATA2_PIN PA1
#define TYPE_SNES 0
#define TYPE_NES 1
#define JOYSTICK TYPE_SNES

#define FONT_SIZE 63 //91

#if BOOTLOADER_ADDR==0
	#warning "Bootloader was compiled with address=0x0000"
#endif

;Public methods
.global TIMER1_COMPA_vect
.global SetFont
.global ClearVram
.global joypad_status
.global vram
.global vsync_flag
.global InitVideo
.global wave_vol
.global wave_pos
.global WriteEeprom
.global ReadEeprom
.global first_render_line_tmp
.global screen_tiles_v_tmp
.global WaitVSync
.global DrawBar
.global Print

.section .bss
	//vram is organized as 16 bit per tile index: 
	//byte0=tile index,byte1=background color
	vram: 	  	.space VRAM_SIZE 
	fontram:	.space FONT_SIZE*8 ;unpacked size


	sync_phase:  .byte 1 ;0=pre-eq, 1=eq, 2=post-eq, 3=hsync, 4=vsync
	sync_pulse:	 .byte 1
	vsync_flag:	 .byte 1	;set 30 hz
	curr_field:	 .byte 1	;0 or 1, changes at 60hz

	first_render_line:	.byte 1
	screen_tiles_v: 	.byte 1

	first_render_line_tmp:	.byte 1
	screen_tiles_v_tmp: 	.byte 1

	;last read results of joypads	
	joypad_status:		.word 1

	wave_pos:		.byte 1
	wave_vol:		.byte 1
	wave_vol_frac:	.byte 1

.section .init8
	call InitVideo


.section .text
	fontshade:  		.byte 91,164,246,246,246,164,91,82

	sync_func_vectors:	.word pm(do_pre_eq)
						.word pm(do_eq)
						.word pm(do_post_eq)
						.word pm(do_hsync)



;***************************************************
; Video Mode 1: Tiles-Only
; Process video frame in tile mode 6x7 pixel tiles (40*28) 
;***************************************************	

sub_video_mode1:

	;waste time to align with next hsync in render function ;1554
	ldi r24,lo8(390-32-4-1-3)
	ldi r25,hi8(390-32-4-1-3)
	sbiw r24,1
	brne .-4	
	rjmp .
	

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	//SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	lds r22,screen_tiles_v
	ldi r23,TILE_HEIGHT
	mul r22,r23
	mov r23,r0
	
	clr r22 ;current tile line

next_tile_line:	
	rcall hsync_pulse 

	ldi r19,30+7+1
	dec r19			
	brne .-4

	;***Render scanline***
	call render_tile_line

	ldi r19,27-7+1+1
	dec r19			
	brne .-4
	rjmp .

	dec r23
	breq text_frame_end
	
	inc r22
	cpi r22,8 ;last line of a tile? 1
	breq next_tile_row 
	
	;wait to align with next_tile_row instructions (+1 cycle for the breq)
	lpm
	rjmp .
	
	rjmp next_tile_line	

next_tile_row:
	clr r22		;current char line			;1	
	ldi r19,VRAM_TILES_H*2
	add YL,r19
	adc YH,r22
	rjmp next_tile_line

text_frame_end:

	ldi r19,4
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
	ldi ZL,lo8(fontram)
	ldi ZH,hi8(fontram)
	add ZL,r22	;add tile row offset
	adc ZH,r16  ;add tile row offset
	
	movw r24,ZL	;save for use in loop

	;load the first tile from vram
	ld	r16,X+	;load next tile # from VRAM	
	mul r16,r21  ;calculate offset in tile table
	add ZL,r0	
	adc ZH,r1   

	ld r2,X+	;load first tile bg color
	ld r16,Z	;load first tile row pixels

	;draw 40 tiles wide (6x8), 6 clocks/pixel -->1440
	ldi r17,40

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
	dec r17
	
	mov r18,r2
	sbrc r16,3
	mov r18,r4
	out _SFR_IO_ADDR(DATA_PORT),r18	
	ld r16,Z	;load tile's row of pixels
	
	mov r18,r2
	ld r2,X+	;load next tile bg color
	out _SFR_IO_ADDR(DATA_PORT),r18	;6th pixel is always background color
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
	
	;SYNC_HSYNC_PULSES-FIRST_RENDER_LINE
	ldi ZH,SYNC_HSYNC_PULSES
	lds r0,first_render_line
	sub ZH,r0				
	lds ZL,sync_pulse
	cp ZL,ZH
	brsh render_end

	;SYNC_HSYNC_PULSES-FIRST_RENDER_LINE-(SCREEN_TILES_V*TILE_HEIGHT)
	lds r0,screen_tiles_v
	ldi ZL,TILE_HEIGHT
	mul r0,ZL
	sub ZH,r0				
	lds ZL,sync_pulse
	cp ZL,ZH
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

	ldi ZH,1
latency_loop:
	cp ZL,ZH
	brlo .		;advance PC to next instruction	
	inc ZH
	cpi ZH,20
	brlo latency_loop


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


	;fetch render height registers if they changed	
	lds ZH,first_render_line_tmp
	cpi ZH,0
	breq no_reload

	sts first_render_line,ZH
	lds ZH,screen_tiles_v_tmp
	sts screen_tiles_v,ZH
	clr ZH
	sts first_render_line_tmp,ZH
	sts screen_tiles_v_tmp,ZH

no_reload:

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

	ldi ZL,176
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

	ldi ZL,SYNC_PHASE_HSYNC
	ldi ZH,SYNC_HSYNC_PULSES
	rcall update_sync_phase

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
	ldi r30,lo8(VRAM_SIZE)
	ldi r31,hi8(VRAM_SIZE)

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

	ldi r19,VRAM_TILES_H*2

	mul r22,r19		;calculate Y line addr in vram
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

;****************************************************
; INITIALIZATION
;****************************************************
InitVideo:

	;setup timer 1 to generate interrupts on 
	;twice the line rate.
	
	cli

	clr r20
	sts wave_vol,r20
	sts first_render_line_tmp,r20
	sts screen_tiles_v_tmp,r20
	;stop timers	
	sts _SFR_MEM_ADDR(TCCR1B),r20
	sts _SFR_MEM_ADDR(TCCR0B),r20
	;clear counters
	sts _SFR_MEM_ADDR(TCNT1H),r20
	sts _SFR_MEM_ADDR(TCNT1L),r20	
		

	//*** Set params *****

	;set port directions
	ldi r20,0xff
	out _SFR_IO_ADDR(DDRC),r20 ;video dac
	out _SFR_IO_ADDR(DDRB),r20 ;h-sync for ad725

	ldi r20,(1<<AUDIO_OUT_PIN)+(1<<LED_PIN)
	out _SFR_IO_ADDR(DDRD),r20 ;audio-out + led


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
	sts _SFR_MEM_ADDR(TCCR0B),r20


	;set sound PWM on TIMER2
	ldi r20,(1<<COM2A1)+(1<<WGM21)+(1<<WGM20)   ;Fast PWM	
	sts _SFR_MEM_ADDR(TCCR2A),r20

	ldi r20,0
	sts _SFR_MEM_ADDR(OCR2A),r20	;duty cycle (amplitude)

	ldi r20,(1<<CS20)  ;enable timer, no pre-scaler
	sts _SFR_MEM_ADDR(TCCR2B),r20

	ldi r20,(1<<SYNC_PIN)+(1<<VIDEOCE_PIN)
	out _SFR_IO_ADDR(SYNC_PORT),r20	;set sync line to hi & enable AD723

	ldi r20,FIRST_RENDER_LINE
	sts first_render_line,r20
	ldi r20,SCREEN_TILES_V
	sts screen_tiles_v,r20
	


	;Unpack font in RAM
	ldi ZL,lo8(fonts)
	ldi ZH,hi8(fonts)
	ldi XL,lo8(fontram)
	ldi XH,hi8(fontram)

	ldi r24,FONT_SIZE ;62
loop1:
	lpm r16,Z+ ;read the packed letter(8 rows * 5 bits)
	lpm r17,Z+
	lpm r18,Z+
	lpm r19,Z+
	lpm r20,Z+
	
	ldi r23,8
loop2:
	st X+,r16
	ldi r22,5
loop3:	
	;shift left 5 bits
	rol r20
	rol r19
	rol r18
	rol r17
	rol r16
	dec r22
	brne loop3

	dec r23
	brne loop2

	dec r24
	brne loop1

	#if BOOTLOADER_ADDR != 0 
		in r16, _SFR_IO_ADDR(MCUCR)
		mov r17, r16
		; Enable change of Interrupt Vectors
		ori r16, (1<<IVCE)
		out _SFR_IO_ADDR(MCUCR), r16
		; Move interrupts to Boot Flash section
		ldi r17, (1<<IVSEL)
		out _SFR_IO_ADDR(MCUCR), r17
	#endif

	sei

	clr r1
	ret


;*******************************************
; delay loop used in sync subs & basic sound
; sound is a simple ramp/sawtooth
; and does not account for sync double rate
; so it's a bit distorted
;*******************************************
wait63cycles:
	push r16
	push r17

	ldi ZL,9
	dec ZL
	brne .-4
	nop

	lds r16,wave_pos
	lds r17,wave_vol
	lds r30,wave_vol_frac

	mulsu r16,r17
	
	clr r0
	subi r16,-90

	subi r30,0x50
	cpse r17,r0
	sbci r17,0	
		
	sts wave_pos,r16
	sts wave_vol,r17
	sts wave_vol_frac,r30
	
	ldi r16,128
	sub r1,r16	;convert to signed	

	sts _SFR_MEM_ADDR(OCR2A),r1

	pop r17
	pop r16
	ret


;****************************
; Write byte to EEPROM
; extern void WriteEeprom(int addr,u8 value)
; r25:r24 - addr
; r22 - value to write
;****************************


WriteEeprom:
   ; Wait for completion of previous write
   sbic _SFR_IO_ADDR(EECR),EEPE
   rjmp WriteEeprom
   ; Set up address (r25:r24) in address register
   out _SFR_IO_ADDR(EEARH), r25
   out _SFR_IO_ADDR(EEARL), r24
   ; Write data (r22) to Data Register
   out _SFR_IO_ADDR(EEDR),r22
   cli
   ; Write logical one to EEMPE
   sbi _SFR_IO_ADDR(EECR),EEMPE
   ; Start eeprom write by setting EEPE
   sbi _SFR_IO_ADDR(EECR),EEPE
   sei
   ret


;****************************
; Read byte from EEPROM
; extern unsigned char ReadEeprom(int addr)
; r25:r24 - addr
; r24 - value read
;****************************

ReadEeprom:
   ; Wait for completion of previous write
   sbic _SFR_IO_ADDR(EECR),EEPE
   rjmp ReadEeprom
   ; Set up address (r25:r24) in address register
   out _SFR_IO_ADDR(EEARH), r25
   out _SFR_IO_ADDR(EEARL), r24
   ; Start eeprom read by writing EERE
   cli
   sbi _SFR_IO_ADDR(EECR),EERE
   ; Read data from Data Register
   in r24,_SFR_IO_ADDR(EEDR)
   sei
   ret

;*******************************************
; Wait for the specified amount of VSYNC cycles
; C-Callable
; r24 - Cycles to wait
;********************************************
WaitVSync:

	lds r25,vsync_flag
	cpi r25,0
	breq WaitVSync
	
	clr r25 
	sts vsync_flag,r25

	dec r24
	brne WaitVSync

    ret

;*******************************************
; Prints a bar of teh specified lenght in VRAM
; Only sets the color bg attribute
; 
; C-Callable
; r25:r24 - Print adress in RAM (Y*40)+X
; r22     - Lenght
; r20     - Background color
;********************************************
DrawBar:
	movw ZL,r24
	subi ZL,lo8(-(vram))
	sbci ZH,hi8(-(vram))	

DrawBar_loop:
	std Z+1,r20
	adiw ZL,2
	dec r22
	brne DrawBar_loop

	ret


;*******************************************
; Prints a ASCIIZ String from ROM or RAM. The function
; will get data from ROM if the string addr >= 0x8000.
; 
; C-Callable
; r25:r24 - Print adress in RAM (Y*40)+X
; r23:r22 - String pointer in Memory
; r20     - Background color
;********************************************
Print:
	movw XL,r24
	subi XL,lo8(-(vram))
	sbci XH,hi8(-(vram))
	movw ZL,r22

Print_loop:
	ld r24,Z	
	lpm r25,Z+	
	sbrc ZH,7
	mov r24,r25

	tst r24			;end of string?
	breq Print_end
	
	cpi r24,0x40
	brlo .+2
	cbr r24,0x20	;mask out lower case characters

	subi r24,32		;first character in font
	st X+,r24
	st X+,r20	
	rjmp Print_loop

Print_end:
	ret


fonts:
;font size=512 bytes
#include "data/fonts.inc" 
