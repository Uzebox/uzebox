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
 * Audio mixer than mixes 5 channels into one sample during each HSYNC.
 *
 */

#include <avr/io.h>
#include <defines.h>

.global InitSound
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
.global counter


//Public variables
.global mixer



#define vol 			0
#define step_lo			1
#define step_hi			2
#define samplepos_frac	3
#define samplepos_lo	4
#define samplepos_hi	5

.section .bss

	sound_enabled:.byte 1

	//struct MixerStruct -> soundEngine.h
	mixer:	
	mixerStruct:

	tr1_vol:		 .byte 1
	tr1_step_lo:	 .byte 1
	tr1_step_hi:	 .byte 1
	tr1_pos_frac:	 .byte 1
	tr1_pos_lo:		 .byte 1
	tr1_pos_hi:		 .byte 1

	tr2_vol:		 .byte 1
	tr2_step_lo:	 .byte 1
	tr2_step_hi:	 .byte 1
	tr2_pos_frac:	 .byte 1
	tr2_pos_lo:		 .byte 1
	tr2_pos_hi:		 .byte 1

	tr3_vol:		 .byte 1
	tr3_step_lo:	 .byte 1
	tr3_step_hi:	 .byte 1
	tr3_pos_frac:	 .byte 1
	tr3_pos_lo:		 .byte 1
	tr3_pos_hi:		 .byte 1

	tr4_vol:		 .byte 1
	tr4_params:		 .byte 1 //bit0=>0=7,1=15 bits lfsr, b1:6=divider 
	tr4_barrel_lo:	 .byte 1
	tr4_barrel_hi:	 .byte 1
	tr4_divider:	 .byte 1 ;current divider accumulator
	tr4_reserved1:	 .byte 1

	tr5_vol:		 .byte 1
	tr5_step_lo:	 .byte 1
	tr5_step_hi:	 .byte 1
	tr5_pos_frac:	 .byte 1
	tr5_pos_lo:		 .byte 1
	tr5_pos_hi:		 .byte 1
	tr5_loop_start_lo: .byte 1
	tr5_loop_start_hi: .byte 1
	tr5_loop_end_lo: .byte 1
	tr5_loop_end_hi: .byte 1


.section .text

;**********************
; Mix sound and process music track
; NOTE: registers r18-r27 are already saved by the caller
;***********************
process_music:
	
#if ENABLE_MIXER==1
	lds ZL,sound_enabled
	sbrc ZL,0
 	call ProcessMusic
#endif

	ret

;****************************
; Inline sound mixing
; In: ZL = video phase (1=pre-eq/post-eq, 2=hsync)

; Destroys: ZL (r30)
; cycles: 212
;****************************
update_sound:
	push r16
	push r17
	push r18
	push r28
	push r29

	mov r18,ZL

	;channel 1 
	lds r16,tr1_step_lo
	lds r17,tr1_pos_frac
	add	r17,r16	;add step to fractional part of sample pos
	lds r16,tr1_step_hi	
	lds ZL,tr1_pos_lo
	lds ZH,tr1_pos_hi 
	adc ZL,r16	;add step to low byte of sample pos
	lpm	r16,Z	;load sample
	sts tr1_pos_lo,ZL
	sts tr1_pos_frac,r17
	lds r17,tr1_vol
	mulsu r16,r17;(sample*mixing vol)
	clr r0
	sbc r0,r0	;sign extend	
	mov r28,r1	;set (sample*vol>>8) to mix buffer lsb
	mov r29,r0	;set mix buffer msb	

;38
	
	;channel 2
	lds r16,tr2_step_lo
	lds r17,tr2_pos_frac
	add	r17,r16	;add step to fractional part of sample pos
	lds r16,tr2_step_hi	
	lds ZL,tr2_pos_lo
	lds ZH,tr2_pos_hi 
	adc ZL,r16	;add step to low byte of sample pos
	lpm	r16,Z	;load sample
	sts tr2_pos_lo,ZL
	sts tr2_pos_frac,r17
	lds r17,tr2_vol
	;clr r17
	;nop

	;*** Video sync update ***
	sbrc r18,0								;pre-eq/post-eq sync
	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN	;TCNT1=0xAC
	sbrs r18,0								
	rjmp .
	;*************************

	mulsu r16,r17;(sample*mixing vol)
	clr r0
	sbc r0,r0	;sign extend
	add r28,r1	;add (sample*vol>>8) to mix buffer lsb
	adc r29,r0	;ajust mix buffer msb		
;70
	
	;channel 3
	lds r16,tr3_step_lo
	lds r17,tr3_pos_frac
	add	r17,r16				;add step to fractional part of sample pos
	lds r16,tr3_step_hi	
	lds ZL,tr3_pos_lo
	lds ZH,tr3_pos_hi 
	adc ZL,r16				;add step to low byte of sample pos
	lpm	r16,Z				;load sample
	sts tr3_pos_lo,ZL
	sts tr3_pos_frac,r17
	lds r17,tr3_vol
	mulsu r16,r17			;(sample*mixing vol)
	clr r0
	sbc r0,r0				;sign extend
	add r28,r1				;add (sample*vol>>8) to mix buffer lsb
	adc r29,r0				;ajust mix buffer msb
;97	

	;channel 4 - 7/15 bit LFSR 
	lds r16,tr4_barrel_lo
	lds r17,tr4_barrel_hi
	lds ZL,tr4_divider
	dec ZL	
	brpl ch4_no_shift	

	lds ZH,tr4_params
	mov ZL,ZH
	lsr ZL 			;keep bits7:1

	mov r0,r16  ;copy barrel shifter
	lsr r0
	eor r0,r16  ;xor bit0 and bit1
	bst r0,0
	lsr r17
	ror r16
	bld r17,6	;15 bits mode
	sbrs ZH,0
	bld r16,6	;7 bits mode

	sts tr4_barrel_lo,r16
	sts tr4_barrel_hi,r17

	rjmp ch4_end
ch4_no_shift:
	;wait loop 21 cycles
	ldi r17,6
	dec r17
	brne .-4
	;rjmp .
ch4_end:

	sts tr4_divider,ZL
;126
	
	;*** Video sync update ***
	sbrc r18,1								;hsync
	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN	;TCNT1=0xF0
	sbrs r18,1								
	rjmp .
	;*************************
	
	ldi r17,0x80 ;-128
	sbrc r16,0
	ldi r17,0x7f ;+127
	
	lds r16,tr4_vol

	mulsu r17,r16;(sample*mixing vol)
	clr r0
	sbc r0,r0	;sign extend
	add r28,r1	;add (sample*vol>>8) to mix buffer lsb
	adc r29,r0	;ajust mix buffer msb

;142

	;channel 5 PCM 

	;add fractional part
	lds r16,tr5_pos_frac
	lds r17,tr5_step_lo 
	add r16,r17
	sts tr5_pos_frac,r16

	;add lo
	lds ZL,tr5_pos_lo
	lds r17,tr5_step_hi 
	adc ZL,r17

	;add hi
	lds ZH,tr5_pos_hi
	ldi r16,0
	adc ZH,r16

	lds r0,tr5_loop_end_lo
	lds r1,tr5_loop_end_hi

	lds r16,tr5_loop_start_lo
	lds r17,tr5_loop_start_hi	

	cp ZL,r0
	cpc ZH,r1
	brlo .+2
	movw ZL,r16

	sts tr5_pos_lo,ZL
	sts tr5_pos_hi,ZH

	lpm	r16,Z	;load sample
	lds r17,tr5_vol

	mulsu r16,r17;(sample*mixing vol)
	clr r0
	sbc r0,r0	;sign extend
	add r28,r1	;add (sample*vol>>8) to mix buffer lsb
	adc r29,r0	;adjust mix buffer msb	
;186	
	
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
	sts _SFR_MEM_ADDR(OCR2A),r28 ;output sound byte
	
	pop r29
	pop r28
	pop r18
	pop r17
	pop r16
	
	ret

;212




