/*
 *  Uzebox Kernel
 *  Copyright (C) 2008-2009 Alec Bourque
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

/*
 * Audio mixer than mixes the sound channels to a circular buffer during VSYNC.
 *
 */

#include <avr/io.h>
#include <defines.h>

.global update_sound_buffer
.global update_sound_buffer_2
.global update_sound_buffer_fast
.global process_music
.global waves
.global mix_pos
.global mix_buf
.global mix_bank
.global tr4_barrel_lo
.global tr4_barrel_hi
.global tr4_params
.global sound_enabled
.global update_sound

//Public variables
.global mixer



#define vol 			0
#define step_lo			1
#define step_hi			2
#define samplepos_frac	3
#define samplepos_lo	4
#define samplepos_hi	5

.section .bss

/*
 * The inline mixer use the free cycles during hsync
 * to mix music and hence does not need a mix buffer.
 */


mix_buf: 	  .space MIX_BUF_SIZE
mix_pos:	  .space 2
mix_bank: 	  .space 1 ;0=first half,1=second half
mix_block:	  .space 1

sound_enabled:.space 1

//struct MixerStruct -> soundEngine.h
mixer:
mixerStruct:


tr1_vol:		 .space 1
tr1_step_lo:	 .space 1
tr1_step_hi:	 .space 1
tr1_pos_frac:	 .space 1
tr1_pos_lo:		 .space 1
tr1_pos_hi:		 .space 1

tr2_vol:		 .space 1
tr2_step_lo:	 .space 1
tr2_step_hi:	 .space 1
tr2_pos_frac:	 .space 1
tr2_pos_lo:		 .space 1
tr2_pos_hi:		 .space 1

tr3_vol:		 .space 1
tr3_step_lo:	 .space 1
tr3_step_hi:	 .space 1
tr3_pos_frac:	 .space 1
tr3_pos_lo:		 .space 1
tr3_pos_hi:		 .space 1


#if MIXER_CHAN4_TYPE == 0
	tr4_vol:		 .space 1
	tr4_params:		 .space 1 //bit0=>0=7,1=15 bits lfsr, b1:6=divider
	tr4_barrel_lo:	 .space 1
	tr4_barrel_hi:	 .space 1
	tr4_divider:	 .space 1 ;current divider accumulator
	tr4_reserved1:	 .space 1

#else
	tr4_vol:		 .space 1
	tr4_step_lo:	 .space 1
	tr4_step_hi:	 .space 1
	tr4_pos_frac:	 .space 1
	tr4_pos_lo:		 .space 1
	tr4_pos_hi:		 .space 1
	tr4_loop_len_lo: .space 1
	tr4_loop_len_hi: .space 1
	tr4_loop_end_lo: .space 1
	tr4_loop_end_hi: .space 1

#endif


.section .text

;**********************
; Mix sound and process music track
; NOTE: registers r18-r27 are already saved by the caller
;***********************
process_music:

	//call update_sound
	
#if ENABLE_MIXER==1
	lds ZL,sound_enabled
	sbrc ZL,0
 	call ProcessMusic
#endif


	;Flip mix bank & set target bank adress for mixing
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

	ldi r18,1
	eor	r0,r18
	sts mix_bank,r0
	
	ldi r18,2
	sts mix_block,r18	


#if ENABLE_MIXER==1

	lds ZL,sound_enabled
	sbrs ZL,0
	ret

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


	;mix channels

	#if SOUND_CHANNEL_4_ENABLE == 1

		#if MIXER_CHAN4_TYPE == 0	
			lds r21,tr4_vol
			lds r22,tr4_barrel_lo
			lds r23,tr4_barrel_hi
			lds r24,tr4_divider
		#else
			lds r21,tr4_vol
			lds ZL,tr4_pos_lo
			lds ZH,tr4_pos_hi
			lds r24,tr4_pos_frac

			lds r4,tr4_step_lo 
			lds r5,tr4_step_hi 
			clr r6

			;compute loop start
			movw r10,ZL
			lds r0,tr4_loop_len_lo
			lds r1,tr4_loop_len_hi
			sub r10,r0
			sbc r11,r1
			lds r8,tr4_loop_end_lo
			lds r9,tr4_loop_end_hi
			
			movw r2,XL	;push

			ldi r28,lo8(262/2)
		ch4_loop:
			;channel 4 -PCM mode
		.rept 2
			add r24,r4
			adc ZL,r5
			adc ZH,r6

			cp ZL,r8
			cpc ZH,r9
			brlo .+2
			movw ZL,r10

			lpm	r20,Z	;load sample
			mulsu r20,r21;(sample*mixing vol)
			st X+,r1

		.endr	
			dec r28
			brne ch4_loop

			movw XL,r2	;pop

			//save positions
			sts tr4_vol,r21
			sts tr4_pos_lo,ZL
			sts tr4_pos_hi,ZH
			sts tr4_pos_frac,r24


		#endif	//MIXER_CHAN4_TYPE == 0
	
	#endif	//SOUND_CHANNEL_4_ENABLE == 1





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

	#if MIXER_CHAN4_TYPE == 1 && SOUND_CHANNEL_4_ENABLE == 1
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
	;clr r0
	sbc r0,r0	;sign extend

	#if MIXER_CHAN4_TYPE == 0 || SOUND_CHANNEL_4_ENABLE == 0
		mov r28,r1	;add (sample*vol>>8) to mix buffer lsb
		mov r29,r0	;ajust mix buffer msb
	#else
		add r28,r1	;add (sample*vol>>8) to mix buffer lsb
		adc r29,r0	;ajust mix buffer msb		
	#endif

	#if SOUND_CHANNEL_2_ENABLE == 1
		;channel 2
		add	r9,r7	;add step to fractional part of sample pos
		adc r10,r8	;add step to low byte of sample pos 
		movw ZL,r10
		lpm	r20,Z	;load sample
		mulsu r20,r18;(sample*mixing vol)
		;clr r0
		sbc r0,r0	;sign extend
		add r28,r1	;add (sample*vol>>8) to mix buffer lsb
		adc r29,r0	;ajust mix buffer msb
	#endif 

	#if SOUND_CHANNEL_3_ENABLE == 1
		;channel 3
		add	r16,r12	;add step to fractional part of sample pos
		adc r14,r13	;add step to low byte of sample pos 
		movw ZL,r14
		lpm	r20,Z	;load sample
		mulsu r20,r19;(sample*mixing vol)
		;clr r0
		sbc r0,r0	;sign extend
		add r28,r1	;add (sample*vol>>8) to mix buffer lsb
		adc r29,r0	;ajust mix buffer msb
	#endif

	#if MIXER_CHAN4_TYPE == 0 && SOUND_CHANNEL_4_ENABLE == 1	

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
		;clr r0
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

#endif // ENABLE_MIXER==1

	ret


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

	ret ;20+4=24


;************************
; Regular sound output used during hblanks.
; Handles MIDI.
; In: ZL = video phase (1=pre-eq/post-eq, 2=hsync)
; Destroys: ZH
; Cycles: VSYNC = 68
;         HSYNC = 135
;***********************
update_sound:
	push r16
	push r17
	push r18
	push ZL

	lds ZL,mix_pos
	lds ZH,mix_pos+1
			
	ld r16,Z+
	sts _SFR_MEM_ADDR(OCR2A),r16 ;output sound byte

	;compare+wrap=8 cycles fixed
	ldi r16,hi8(MIX_BUF_SIZE+mix_buf)
	cpi ZL,lo8(MIX_BUF_SIZE+mix_buf)
	cpc ZH,r16

	ldi r16,lo8(mix_buf)
	ldi r17,hi8(mix_buf)

	brlo .+2
	movw ZL,r16

	sts mix_pos,ZL
	sts mix_pos+1,ZH	


#if UART_RX_BUFFER == 1
	;read MIDI-in data (27 cycles)
	ldi ZL,lo8(uart_rx_buf)
	ldi ZH,hi8(uart_rx_buf)
	lds r16,uart_rx_buf_end
	
	clr r17
	add ZL,r16
	adc ZH,r17

	lds r17,_SFR_MEM_ADDR(UCSR0A)	
	lds r18,_SFR_MEM_ADDR(UDR0)

	st Z,r18
	
	sbrc r17,RXC0
	inc r16
	andi r16,(UART_RX_BUFFER_SIZE-1) ;wrap
	sts uart_rx_buf_end,r16
	
	rjmp .
	rjmp .
	rjmp .
#else
	//alignment cycles
	ldi ZL,8
	dec ZL
	brne .-4
#endif

	pop ZL
	pop r18
	pop r17
	pop r16

	;*** Video sync update ***
	sbrc ZL,0								;pre-eq/post-eq sync
	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN	;TCNT1=0xAC
	sbrs ZL,0								
	rjmp .+2
	ret

	ldi ZH,20
	dec ZH
	brne .-4
	rjmp .

	;*** Video sync update ***
	sbrc ZL,1								;hsync
	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN	;TCNT1=0xF0
	sbrs ZL,1								
	rjmp .

	ret 

