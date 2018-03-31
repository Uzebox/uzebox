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

	; Audio generation

	lds   ZH,      sound_spos
	subi  ZH,      0x01
	brcc  .+2
	ldi   ZH,      0x00    ; End of click sound, silence
	sbic  _SFR_IO_ADDR(GPIOR0), 7
	ldi   ZH,      0xFF    ; Requested new sound
	cbi   _SFR_IO_ADDR(GPIOR0), 7
	sts   sound_spos, ZH

	lsl   ZH               ; Make 2 periods
	lsl   ZH               ; Make 4 periods
	subi  ZH,      64      ; 192 -> 256=0 -> 192
	brpl  .+2
	com   ZH               ; 63 -> 0 -> 127 -> 64
	subi  ZH,      192     ; 127 -> 64 -> 191 -> 128

	sts   _SFR_MEM_ADDR(OCR2A), ZH ; Output sound byte

	; Wait and Sync generation

	WAIT  ZL,      39

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
