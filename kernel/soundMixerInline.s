/*
 *  Uzebox Kernel
 *  Copyright (C) 2008 - 2009 Alec Bourque
 *                2017 Sandor Zsuga (Jubatian)
 *                     CunningFellow
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

; Public variables
.global mixer



; For uzeboxSoundEngineCore.s

#define vol            0
#define step_lo        1
#define step_hi        2
#define samplepos_frac 3
#define samplepos_lo   4
#define samplepos_hi   5



.section .bss

	sound_enabled:.space 1

	; struct MixerStruct -> soundEngine.h

mixer:
mixerStruct:

	tr1_vol:       .space 1
	tr1_step_lo:   .space 1
	tr1_step_hi:   .space 1
	tr1_pos_frac:  .space 1
	tr1_pos_lo:    .space 1
	tr1_pos_hi:    .space 1

	tr2_vol:       .space 1
	tr2_step_lo:   .space 1
	tr2_step_hi:   .space 1
	tr2_pos_frac:  .space 1
	tr2_pos_lo:    .space 1
	tr2_pos_hi:    .space 1

	tr3_vol:       .space 1
	tr3_step_lo:   .space 1
	tr3_step_hi:   .space 1
	tr3_pos_frac:  .space 1
	tr3_pos_lo:    .space 1
	tr3_pos_hi:    .space 1

	tr4_vol:       .space 1
	tr4_params:    .space 1 ; bit0=>0=7,1=15 bits lfsr, b1:6=divider
	tr4_barrel_lo: .space 1
	tr4_barrel_hi: .space 1
	tr4_divider:   .space 1 ; current divider accumulator
	tr4_reserved1: .space 1

#if SOUND_CHANNEL_5_ENABLE==1
	tr5_vol:       .space 1
	tr5_step_lo:   .space 1
	tr5_step_hi:   .space 1
	tr5_pos_frac:  .space 1
	tr5_pos_lo:    .space 1
	tr5_pos_hi:    .space 1
	tr5_loop_len_lo: .space 1
	tr5_loop_len_hi: .space 1
	tr5_loop_end_lo: .space 1
	tr5_loop_end_hi: .space 1
#endif

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
; In: ZL = video phase (1 = Pre-eq / Post-eq, 2 = Hsync, 0 = No sync)
;
; Total cycle count must be AUDIO_OUT_HSYNC_CYCLES in Hsync only, otherwise
; it can finish earlier or later. Sync pulse timings have to be maintained.
;
; Destroys: Z, r0, r1
;****************************

update_sound:

	push  r18
	push  r17
	push  r16

	mov   r18,     ZL

	; Mix result is collected in r0:r1 (r0 is the high byte!)

	; Channel 1 (27 cy - 3 for initializing mix. buffer)

	lds   r17,     tr1_pos_frac
	lds   ZL,      tr1_pos_lo
	lds   ZH,      tr1_pos_hi
	lds   r16,     tr1_step_lo
	add   r17,     r16     ; Add step to fractional part of sample pos
	lds   r16,     tr1_step_hi
	adc   ZL,      r16     ; Add step to low byte of sample pos
	lpm   r16,     Z       ; Load sample
	sts   tr1_pos_lo, ZL
	sts   tr1_pos_frac, r17
	lds   r17,     tr1_vol
	mulsu r16,     r17     ; (sample * mixing_vol)
	sbc   r0,      r0      ; Sign extend

	; Channel 2 (27 cy + 2/3 sync generator + 2 preload)

	lds   r17,     tr2_pos_frac
	lds   ZL,      tr2_pos_lo
	lds   ZH,      tr2_pos_hi
	lds   r16,     tr2_step_lo
	add   r17,     r16     ; Add step to fractional part of sample pos
	lds   r16,     tr2_step_hi
	adc   ZL,      r16     ; Add step to low byte of sample pos
	lpm   r16,     Z       ; Load sample
	sts   tr2_pos_lo, ZL
	sts   tr2_pos_frac, r17
	movw  ZL,      r0
	lds   r17,     tr2_vol
	mulsu r16,     r17     ; (sample * mixing_vol)
	sbc   r0,      r0      ; Sign extend
	add   r1,      ZH      ; Add ((sample * vol) >> 8) to mix buffer lsb
	lds   r17,     tr3_pos_frac
	;--- Video sync update ( 68 cy LOW pulse) ---
	sbrc  r18,     0
	sbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN
	;--------------------------------------------
	adc   r0,      ZL      ; Ajust mix buffer msb

	; Channel 3 (27 cy - 2 preload)

	lds   ZL,      tr3_pos_lo
	lds   ZH,      tr3_pos_hi
	lds   r16,     tr3_step_lo
	add   r17,     r16     ; Add step to fractional part of sample pos
	lds   r16,     tr3_step_hi
	adc   ZL,      r16     ; Add step to low byte of sample pos
	lpm   r16,     Z       ; Load sample
	sts   tr3_pos_lo, ZL
	sts   tr3_pos_frac, r17
	movw  ZL,      r0
	lds   r17,     tr3_vol
	mulsu r16,     r17     ; (sample * mixing_vol)
	sbc   r0,      r0      ; Sign extend
	add   r1,      ZH      ; Add ((sample * vol) >> 8) to mix buffer lsb
	adc   r0,      ZL      ; Ajust mix buffer msb

	; Channel 4 - 7/15 bit LFSR (34 cy)

	lds   r16,     tr4_barrel_lo ; Get the LFSR (16 bits barrel shifter)
	lds   r17,     tr4_barrel_hi

	lds   ZH,      tr4_vol ; get the Volume
	lsr   ZH               ; Divide it by 2 to get sample for '1'.
	clc
	sbrc  r16,     0       ; If the LSB of the LFSR is zero
	neg   ZH               ; then produce sample (negative) for '0' (C set unless zero)
	sbc   ZL,      ZL      ; Sign extend
	add   r1,      ZH      ; Add sample to mix buffer lsb
	adc   r0,      ZL      ; Adjust mix buffer msb

	lds   ZL,      tr4_divider ; load the divider
	subi  ZL,      2       ; Decrement bits 1..7 leaving bit 0 untouched by subtracting 2
	brcs  ch4_shift        ; if not enough ticks have elapsed then don't shift the LFSR
	lpm   ZL,      Z
	lpm   ZL,      Z
	lpm   ZL,      Z
	rjmp  ch4_end

ch4_shift:
	mov   ZL,      r16     ; Perform the actual LFSR shifting by copying low byte of LFSR to a temp for XOR opperation
	lsr   r17              ; shift the 16 bits of the barrel shifter
	ror   r16              ; leaving the old bit 0 into Carry (Same bit used to decide +ve or -ve "sample" above)
	eor   ZL,      r16     ; perform the XOR of bit 0 and bit 1
	bst   ZL,      0       ; Save that XOR'd bit to T
	bld   r17,     6       ; Write T to the 15th bit of the LFSR (regardless of mode as 7 bit will overwrite it)
	lds   ZL,      tr4_params ; Reload the divider / Parameters which consists of 7 bits of divider + 1 bit of mode
	sbrs  ZL,      0       ; If the 7/14 mode bit indicates 7 bit mode then
	bld   r16,     6       ; Store T to the 7th bit of the LFSR

ch4_end:
	sts   tr4_barrel_lo, r16 ; save the LFSR
	sts   tr4_barrel_hi, r17
	sts   tr4_divider, ZL  ; Save the divider (plus 7/15 mode bit in LSB)

#if (SOUND_CHANNEL_5_ENABLE != 0)

	; Channel 5 - PCM (45 cy + 2/3 sync generator)

	ldi   r17,     0
	lds   ZL,      tr5_pos_frac
	lds   ZH,      tr5_step_lo
	;--- Video sync update (136 cy LOW pulse) ---
	sbrc  r18,     1
	sbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN
	;--------------------------------------------
	add   ZL,      ZH      ; Add fractional part
	sts   tr5_pos_frac, ZL

	lds   ZL,      tr5_pos_lo
	lds   ZH,      tr5_pos_hi
	lds   r16,     tr5_step_hi
	adc   ZL,      r16     ; Add low part
	adc   ZH,      r17     ; Add high part

	lds   r16,     tr5_loop_end_lo
	lds   r17,     tr5_loop_end_hi
	cp    ZL,      r16
	cpc   ZH,      r17
	brcc  ch5_reset        ; Gone past end of sample
	lpm   r16,     Z
	rjmp  .
	rjmp  ch5_nores

ch5_reset:
	lds   r16,     tr5_loop_len_lo
	lds   r17,     tr5_loop_len_hi
	sub   ZL,      r16
	sbc   ZH,      r17     ; Reset to beginning

ch5_nores:
	sts   tr5_pos_lo, ZL
	sts   tr5_pos_hi, ZH

	lpm   r16,     Z       ; Load sample
	movw  ZL,      r0
	lds   r17,     tr5_vol
	mulsu r16,     r17     ; (sample * mixing_vol)
	sbc   r0,      r0      ; Sign extend
	add   r1,      ZH      ; Add ((sample * vol) >> 8) to mix buffer lsb
	adc   r0,      ZL      ; Ajust mix buffer msb

#endif

	; Restore no longer used registers (5 cy + 2/3 sync generator)

	movw  ZL,      r0      ; Move mix buffer for Final processing
	pop   r16
	pop   r17
#if (SOUND_CHANNEL_5_ENABLE == 0)
	;--- Video sync update (136 cy LOW pulse) ---
	sbrc  r18,     1
	sbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN
	;--------------------------------------------
#endif

	; Final processing (9 cy)

	subi  ZH,      0x80
	sbci  ZL,      0xFF    ; Converts to unsigned
	brpl  .+6
	ldi   ZH,      0x00    ; Saturate from bottom to 0x00
	nop
	rjmp  .+6
	cpi   ZL,      0x00
	breq  .+2
	ldi   ZH,      0xFF    ; Saturate from top to 0xFF
	sts   _SFR_MEM_ADDR(OCR2A), ZH ; Output sound byte

#if (UART != 0)

	; Read UART data (20 cycles)

	ldi   ZL,      lo8(uart_rx_buf)
	ldi   ZH,      hi8(uart_rx_buf)
	lds   r18,     uart_rx_head

	clr   r1
	add   ZL,      r18
	adc   ZH,      r1
	inc   r18
	andi  r18,     (UART_RX_BUFFER_SIZE - 1) ; Wrap

	lds   r0,      _SFR_MEM_ADDR(UCSR0A)

	sbrc  r0,      RXC0    ; Data in?
	rjmp  uart_rx_in
	lpm   ZL,      Z
	rjmp  .
	rjmp  uart_rx_end

uart_rx_in:
	lds   r0,      _SFR_MEM_ADDR(UDR0)
	st    Z,       r0
	sts   uart_rx_head, r18

uart_rx_end:

	; Send UART data (23 cycles)

	ldi   ZL,      lo8(uart_tx_buf)
	ldi   ZH,      hi8(uart_tx_buf)
	lds   r18,     uart_tx_tail

	add   ZL,      r18
	adc   ZH,      r1      ; r1 = 0

	lds   r0,      _SFR_MEM_ADDR(UCSR0A)

	lds   r1,      uart_tx_head
	cp    r1,      r18     ; Is there any data in the buffer to send?
	sbrs  r0,      UDRE0   ; Data can be sent?
	rjmp  uart_tx_wt
	brne  uart_tx_out
uart_tx_wt:
	lpm   ZL,      Z
	rjmp  .
	rjmp  .
	rjmp  uart_tx_end

uart_tx_out:
	ld    r0,      Z
	sts   _SFR_MEM_ADDR(UDR0), r0
	inc   r18
	andi  r18,     (UART_TX_BUFFER_SIZE - 1) ; Wrap
	sts   uart_tx_tail, r18

uart_tx_end:

#endif

	pop   r18

	ret
