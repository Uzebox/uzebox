/*
 *  Uzebox Kernel
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
 *
 *  Uzebox is a reserved trade mark
*/

#include <avr/io.h>
#include "defines.h"

.global InitSound
.global update_sound_buffer
.global update_sound_buffer_2
.global update_sound_buffer_fast
.global MixSound
.global SetMixerNote
.global SetMixerVolume
.global SetMixerWave
.global SetMixerNoiseParams
.global waves
.global mix_pos
.global mix_buf
.global mix_bank
.global tr4_barrel_lo
.global tr4_barrel_hi
.global tr4_params

#if MIDI_IN == ENABLED
.global midi_rx_buf_start
.global midi_rx_buf_end
.global midi_rx_buf
#endif


//Public variables
.global mixer



#define vol 			0
#define step_lo			1
#define step_hi			2
#define samplepos_frac	3
#define samplepos_lo	4
#define samplepos_hi	5

.section .bss
	mix_buf: 	  .space MIX_BUF_SIZE
	mix_pos:	  .word 1
	mix_bank: 	  .byte 1 ;0=first half,1=second half
	mix_block:	  .byte 1
	stat_lines:	  .byte 1


#if MIDI_IN == ENABLED
	midi_rx_buf:  	.space MIDI_RX_BUF_SIZE 
	midi_rx_buf_start:.byte 1
	midi_rx_buf_end:.byte 1
#endif


//struct MixerStruct -> soundEngine.h
mixer:	
mixerStruct:

tr1_vol:		 .byte 1
tr1_step_lo:	 .byte 1
tr1_step_hi:	 .byte 1
tr1_pos_frac:	 .byte 1
tr1_pos_lo:		 .byte 1
tr1_pos_hi:		 .byte 1
tr1_loop_start_lo: .byte 1
tr1_loop_start_hi: .byte 1
tr1_loop_end_lo: .byte 1
tr1_loop_end_hi: .byte 1

tr2_vol:		 .byte 1
tr2_step_lo:	 .byte 1
tr2_step_hi:	 .byte 1
tr2_pos_frac:	 .byte 1
tr2_pos_lo:		 .byte 1
tr2_pos_hi:		 .byte 1
tr2_loop_start_lo: .byte 1
tr2_loop_start_hi: .byte 1
tr2_loop_end_lo: .byte 1
tr2_loop_end_hi: .byte 1

tr3_vol:		 .byte 1
tr3_step_lo:	 .byte 1
tr3_step_hi:	 .byte 1
tr3_pos_frac:	 .byte 1
tr3_pos_lo:		 .byte 1
tr3_pos_hi:		 .byte 1
tr3_loop_start_lo: .byte 1
tr3_loop_start_hi: .byte 1
tr3_loop_end_lo: .byte 1
tr3_loop_end_hi: .byte 1

#if MIXER_CHAN4_TYPE == 0
	tr4_vol:		 .byte 1
	tr4_params:		 .byte 1 //bit0=>0=7,1=15 bits lfsr, b1:6=divider 
	tr4_barrel_lo:	 .byte 1
	tr4_barrel_hi:	 .byte 1
	tr4_divider:	 .byte 1 ;current divider accumulator
	tr4_reserved1:	 .byte 1
	tr4_reserved2:	 .byte 1
	tr4_reserved3:	 .byte 1
	tr4_reserved4:	 .byte 1
	tr4_reserved5:	 .byte 1
#else
	tr4_vol:		 .byte 1
	tr4_step_lo:	 .byte 1
	tr4_step_hi:	 .byte 1
	tr4_pos_frac:	 .byte 1
	tr4_pos_lo:		 .byte 1
	tr4_pos_hi:		 .byte 1
	tr4_loop_start_lo: .byte 1
	tr4_loop_start_hi: .byte 1
	tr4_loop_end_lo: .byte 1
	tr4_loop_end_hi: .byte 1

	//no part of the structure but used during muxing to hold a computed value

#endif

.section .text
	



;**********************
; SET NOTE
;(C-call compatible)
; r24: Channel (0,1,2,3)
; r22: -MIDI note, 69=A5(440) for waves channels (0,1,2)
;      -Noise params for channel 3
;***********************
SetMixerNote:
	clr r25
	clr r23

#if MIXER_CHAN4_TYPE == 0
	cpi r24,3
	brlo set_note_waves
	ret
#endif

set_note_waves:
	;get step for note
	ldi ZL,lo8(steptable)
	ldi ZH,hi8(steptable)
	lsl r22
	rol r23
	add ZL,r22
	adc ZH,r23	

	lpm r26,Z+
	lpm r27,Z

	ldi ZL,lo8(mixerStruct)
	ldi ZH,hi8(mixerStruct)
	ldi r18,CHANNEL_STRUCT_SIZE
	mul r18,r24
	add ZL,r0
	adc ZH,r1

	std Z+step_lo,r26
	std Z+step_hi,r27

	clr r1
	

	ret

#if MIXER_CHAN4_TYPE == 0
;**********************
; SET NOISE CHANNEL PARAMS
;(C-call compatible)
; r24: noise divider
;*****************
SetMixerNoiseParams:
	;preserve wave type (7/15 bit)
	lds r25,tr4_params
	andi r25,1
	lsl r24
	or r24,r25

	sts tr4_params,r24	
	ret

#endif

;**********************
; SET sound patch for channel 
; (C-call compatible)
; r24 channel (0,1,2)
; r23:r22 Waves channels: patch (0x00-0xfd) 
;         Noise channel: 0xfe=7 bit lfsr, 0xff=15 bit lfsr
;                 
;***********************
SetMixerWave:
	clr r25
	clr r23

	ldi ZL,lo8(mixerStruct)
	ldi ZH,hi8(mixerStruct)
	ldi r18,CHANNEL_STRUCT_SIZE
	mul r18,r24	
	add ZL,r0
	adc ZH,r1

#if MIXER_CHAN4_TYPE == 0
	cpi r22,0xfe	;7bit lfsr
	brne smw1
	lds r22,tr4_params
	andi r22,0xfe;
	sts tr4_params,r22
	rjmp esmw	
smw1:
	cpi r22,0xff	;15bit lfsr
	brne smw2
	lds r22,tr4_params
	ori r22,0xfe;
	sts tr4_params,r22	
	rjmp esmw
smw2:
#endif

	ldi r23,hi8(waves)
	add r23,r22
	std Z+samplepos_hi,r23 ;store path No

esmw:
	clr r1	
	ret




;**********************
; SET Volume
; (C-call compatible)
; r24 channel (0,1,2,3)
; r22 volume (0x00-0xff)
;***********************
SetMixerVolume:
	clr r25
	clr r23

	ldi ZL,lo8(mixerStruct)
	ldi ZH,hi8(mixerStruct)
	ldi r18,CHANNEL_STRUCT_SIZE
	mul r18,r24	
	add ZL,r0
	adc ZH,r1
	std Z+vol,r22 ;store vol

	clr r1	
	ret

MixSound:

	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27 ;optimize push/pop memory allocation for C call


	;process music (music, envelopes, etc)
	;call in soundEngine.c
	sei ;must enable ints for hsync pulses
	clr r1
	
#if VIDEO_MODE == 2
	//this call should not be here. Temp fix.
 

 	call ProcessSprites


#endif




	call read_joypads
	call ProcessMusic

	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29

	;set target bank adress
	lds r0,mix_bank
	tst r0
	brne set_hi_bank
	ldi XL,lo8(mix_buf)
	ldi XH,hi8(mix_buf)
	rjmp end_set_bank
set_hi_bank:
	ldi XL,lo8(mix_buf+MIX_BANK_SIZE)
	ldi XH,hi8(mix_buf+MIX_BANK_SIZE)
end_set_bank:

	ldi r16,1
	eor	r0,r16
	sts mix_bank,r0
	
	ldi r16,2
	sts mix_block,r16	
	

	;mix

#if MIXER_CHAN4_TYPE == 0	
	lds r21,tr4_vol
	lds r22,tr4_barrel_lo
	lds r23,tr4_barrel_hi
	lds r24,tr4_divider
#else
	lds r21,tr4_vol
	lds r22,tr4_pos_lo
	lds r23,tr4_pos_hi
	lds r24,tr4_pos_frac

	lds r4,tr4_step_lo 
	lds r5,tr4_step_hi 
	clr r6
	lds r8,tr4_loop_end_lo
	lds r9,tr4_loop_end_hi

	lds r10,tr4_loop_start_lo
	lds r11,tr4_loop_start_hi

	movw r2,XL	;push

	ldi r28,lo8(262/2)
ch4_loop:
	;channel 4 -PCM mode
.rept 2
	add r24,r4
	adc r22,r5
	adc r23,r6

	cp r22,r8
	cpc r23,r9
	brlo .+2
	movw r22,r10

	movw ZL,r22
	lpm	r20,Z	;load sample
	mulsu r20,r21;(sample*mixing vol)
	st X+,r1

.endr	
	dec r28
	brne ch4_loop

	movw XL,r2	;push

#endif








	lds r2,tr1_step_lo
	lds r3,tr1_step_hi
	lds r4,tr1_pos_lo
	lds r5,tr1_pos_hi 
	lds r6,tr1_pos_frac
	lds r17,tr1_vol
	
	lds r7,tr2_step_lo
	lds r8,tr2_step_hi
	lds r9,tr2_pos_frac
	lds r10,tr2_pos_lo
	lds r11,tr2_pos_hi
	lds r18,tr2_vol	

	lds r12,tr3_step_lo
	lds r13,tr3_step_hi
	lds r14,tr3_pos_lo
	lds r15,tr3_pos_hi
	lds r16,tr3_pos_frac
	lds r19,tr3_vol	



	

	ldi r25,0xff 
mix_loop:

	#if MIXER_CHAN4_TYPE == 1
		ld 28,X
		clr r29	;sign extend
		sbrc r28,7
		ser r29
	#endif

	;channel 1 - 12 cycles/sample
	add	r6,r2	;add step to fractional part of sample pos
	adc r4,r3	;add step to low byte of sample pos
	movw ZL,r4
	lpm	r20,Z	;load sample
	mulsu r20,r17;(sample*mixing vol)
	clr r0
	sbc r0,r0	;sign extend

	#if MIXER_CHAN4_TYPE == 0
		mov r28,r1	;add (sample*vol>>8) to mix buffer lsb
		mov r29,r0	;ajust mix buffer msb
	#else
		add r28,r1	;add (sample*vol>>8) to mix buffer lsb
		adc r29,r0	;ajust mix buffer msb		
	#endif

	;channel 2
	add	r9,r7	;add step to fractional part of sample pos
	adc r10,r8	;add step to low byte of sample pos 
	movw ZL,r10
	lpm	r20,Z	;load sample
	mulsu r20,r18;(sample*mixing vol)
	clr r0
	sbc r0,r0	;sign extend
	add r28,r1	;add (sample*vol>>8) to mix buffer lsb
	adc r29,r0	;ajust mix buffer msb


	;channel 3
	add	r16,r12	;add step to fractional part of sample pos
	adc r14,r13	;add step to low byte of sample pos 
	movw ZL,r14
	lpm	r20,Z	;load sample
	mulsu r20,r19;(sample*mixing vol)
	clr r0
	sbc r0,r0	;sign extend
	add r28,r1	;add (sample*vol>>8) to mix buffer lsb
	adc r29,r0	;ajust mix buffer msb


#if MIXER_CHAN4_TYPE == 0	
	;channel 4 - 7/15 bit LFSR (12 cycles/24 cycles)
	dec r24
	brpl no_shift

	lds r20,tr4_params
	mov r24,r20
	lsr r24 ;keep bits7:1

	mov r0,r22  ;copy barrel shifter
	lsr r0
	eor r0,r22  ;xor bit0 and bit1
	bst r0,0
	lsr r23
	ror r22	
	bld r23,6	;15 bits mode
	sbrs r20,0
	bld r22,6	;7 bits mode

no_shift:
	ldi r20,0x80 ;-128
	sbrc r22,0
	ldi r20,0x7f ;+127

	mulsu r20,r21;(sample*mixing vol)
	clr r0
	sbc r0,r0	;sign extend
	add r28,r1	;add (sample*vol>>8) to mix buffer lsb
	adc r29,r0	;ajust mix buffer msb

#endif

	;final processing

	;clip
	clr r0
	cpi r28,128	;> 127?
	cpc r29,r0 ;0	
	brlt .+2
	ldi r28,127
	
	dec r0
	cpi r28,-128; <-128?
	cpc r29,r0 ;0xff
	brge .+2
	ldi r28,-128


	subi r28,128	;convert to unsigned		
	st X+,r28


	dec r25
	breq .+2
	rjmp mix_loop

	lds r20,mix_block
	dec r20
	sts mix_block,r20
	ldi r25,(MIX_BANK_SIZE-0xff)
	breq .+2
	rjmp mix_loop


	//save current positions
	sts tr1_pos_frac,r6
	sts tr1_pos_lo,r4	

	sts tr2_pos_frac,r9
	sts tr2_pos_lo,r10

	sts tr3_pos_frac,r16
	sts tr3_pos_lo,r14

#if MIXER_CHAN4_TYPE == 0
	sts tr4_barrel_lo,r22
	sts tr4_barrel_hi,r23
	sts tr4_divider,r24
#else
	sts tr4_vol,r21	
	sts tr4_pos_lo,r22
	sts tr4_pos_hi,r23
	sts tr4_pos_frac,r24

#endif
	
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2

	clr r25
	lds r24,sync_phase
	clr r23
	lds r22,sync_pulse
	clr r1
	call DisplayMixStats
		
	pop r27
	pop r26
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	pop r20
	pop r19
	pop r18


	ret

/*
;**********************************
; Optimized sound output (cannot be called during double rate sync)
; NO MIDI
; Destroys: Z,r16,r17
; Cycles: 24
;**********************************
update_sound_buffer_fast:
	lds ZL,mix_pos
	lds ZH,mix_pos+1
			
	ld r16,Z+		;load next sample
	;subi r16,128	;convert to unsigned
	nop
	sts _SFR_MEM_ADDR(OCR2A),r16 ;output sound byte

	;compare+wrap=8 cycles fixed
	ldi r16,hi8(MIX_BUF_SIZE+mix_buf)
	cpi ZL,lo8(MIX_BUF_SIZE+mix_buf)
	cpc ZH,r16
	;12

	ldi r16,lo8(mix_buf)
	ldi r17,hi8(mix_buf)
	brlo .+2
	movw ZL,r16

	sts mix_pos,ZL
	sts mix_pos+1,ZH		

	ret ;20+4=25
*/

;************************
; Regular sound output used during hblanks.
; Handles MIDI.
;
; Cycles: 36 + 27 = 63
;***********************
update_sound_buffer:	
	jmp .
	jmp .
update_sound_buffer_2b:
	push r16
	push r17
	out _SFR_IO_ADDR(GPIOR1),r18 ;push

	lds ZL,mix_pos
	lds ZH,mix_pos+1
			
	ld r16,Z+
	;subi r16,128  ;convert to unsigned
	nop
	sts _SFR_MEM_ADDR(OCR2A),r16 ;output sound byte

	;compare+wrap=8 cycles fixed
	ldi r16,hi8(MIX_BUF_SIZE+mix_buf)
	cpi ZL,lo8(MIX_BUF_SIZE+mix_buf)
	cpc ZH,r16
	
	brsh s1
	nop
	nop
s1:
	brlo s2
	ldi ZL,lo8(mix_buf)
	ldi ZH,hi8(mix_buf)
s2:

	sts mix_pos,ZL
	sts mix_pos+1,ZH	



#if MIDI_IN == ENABLED
	;read MIDI-in data (27 cycles)
	ldi ZL,lo8(midi_rx_buf)
	ldi ZH,hi8(midi_rx_buf)
	lds r16,midi_rx_buf_end
	
	;inc r16 ;used to write ahead in undefined part of buffer if no rx data
	;andi r16,(MIDI_RX_BUF_SIZE-1) ;wrap
	nop
	nop

	clr r17
	add ZL,r16
	adc ZH,r17

	lds r17,_SFR_MEM_ADDR(UCSR0A)	
	lds r18,_SFR_MEM_ADDR(UDR0)

	st Z,r18
	
	sbrc r17,RXC0
	;dec r16
	inc r16
	andi r16,(MIDI_RX_BUF_SIZE-1) ;wrap
	sts midi_rx_buf_end,r16
#else
	//alignment cycles
	lpm
	lpm
	lpm
	lpm
	lpm
	lpm
	nop
	nop
#endif


	nop
	in r18,_SFR_IO_ADDR(GPIOR1) ;pop
	pop r17
	pop r16

	ret 


;no sound update for 1/2 dual-rate sync pulses
;cycles: 35
update_sound_buffer_2:
	nop
	lds ZL,sync_pulse
	sbrs ZL,0
	rjmp update_sound_buffer_2b	

	ldi ZL,8+14-5
usb2:		
	dec ZL
	brne usb2	

	nop
	nop

	ret 


.align 8
waves:
#if INCLUDE_DEFAULT_WAVES == 1
	#include "data/sounds.inc"
#endif

steptable:
#include "data/steptable.inc"

