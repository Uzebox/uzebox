/*
 *  Uzebox Kernel
 *  Copyright (C) 2008-2009 Alec Bourque
 *  Optimized and trimmed to the bootloader by Sandor Zsuga (Jubatian), 2017
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

;
; Audio mixer that produces a single channel of sawtooth clicks in HSYNC.
;
; A click sound may be requested with GPIOR0 bit 7.
;

#include <avr/io.h>
#include <defines.h>



.global update_sound
.global sound_spos



.section .bss



.equ	sound_spos, RAM_RESERVED + 6 ; At top of RAM above the video bytes



.section .text



;****************************
; Inline sound mixing
; Destroys: Z, r0, r1
; Takes AUDIO_OUT_HSYNC_CYCLES
;****************************

update_sound:

	; Channel 1 (17 cy)

	movw  ZL,      r16
	lds   r16,     sound_spos
	subi  r16,     0x01
	brcc  .+2
	ldi   r16,     0x00    ; End of click sound, silence
	sbic  _SFR_IO_ADDR(GPIOR0), 7
	ldi   r16,     0xFF    ; Requested new sound
	cbi   _SFR_IO_ADDR(GPIOR0), 7
	sts   sound_spos, r16
	ldi   r17,     DEFAULT_MASTER_VOL
	mulsu r16,     r17     ; sample * mixing_vol
	movw  r16,     ZL
	mov   ZH,      r1

	; Final processing (3 cy)

	subi  ZH,      0x80    ; Converts to unsigned
	sts   _SFR_MEM_ADDR(OCR2A), ZH ; Output sound byte

	; Wait and Sync generation

	WAIT  ZL,      38

	;--- Video sync update ( 68 cy LOW pulse) ---
	sbic  _SFR_IO_ADDR(GPIOR0), 0
	sbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN
	;--------------------------------------------

	WAIT  ZL,      66

	;--- Video sync update (136 cy LOW pulse) ---
	sbic  _SFR_IO_ADDR(GPIOR0), 1
	sbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN
	;--------------------------------------------

	; Done

	ret
