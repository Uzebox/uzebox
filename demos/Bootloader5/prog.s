/*
 *  Flash programming functions
 *  Copyright (C) 2017 Sandor Zsuga (Jubatian)
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


;
; This is a 112 byte section at 0xFF90 - 0xFFFF. Used to allow for
; reprogramming the bootloader in need.
;
.section .prog



;
; Programs an SD sector if necessary
;
; Inputs:
; r25:r24: SD structure (sector buffer needs to be filled)
;     r22: Target sector in ROM (512b units)
; Outputs:
;     r24: 0: Failure
;          1: Success, Programming was necessary
;          3: Success, Page already contained desired content
; Clobbers:
; r0, r1 (zero), r20, r21, r22, r23, r25, XL, XH, ZL, ZH
;
.global Prog_Sector
Prog_Sector:

	lsl   r22              ; Convert to page address
	movw  ZL,      r24
	ldd   r24,     Z + 1   ; Sector buffer pointer, low
	ldd   r25,     Z + 2   ; Sector buffer pointer, high
	movw  r20,     r24

	; Program pages

	rcall Prog_Page
	mov   r23,     r24
	movw  r24,     r20
	inc   r22
	inc   r25
	rcall Prog_Page

	; Done, return

	and   r24,     r23     ; Combine results of page programmings
	ret



;
; Programs a 256 byte Flash page by the passed source. This is only carried
; out if the source actually doesn't match the page (so this routine can be
; called freely without worrying about Flash wear: it only programs when it
; is actually necessary to program)
;
; Inputs:
; r25:r24: Source 256 byte buffer
;     r22: Target 256 byte page address
; Outputs:
;     r24: 0: Failure
;          1: Success, Programming was necessary
;          3: Success, Page already contained desired content
; Clobbers:
; r0, r1 (zero), r25, XL, XH, ZL, ZH
;
.global Prog_Page
Prog_Page:

	movw  XL,      r24     ; Source in RAM
	mov   ZH,      r22     ; Target in ROM

	; Check whether programming is necessary

	rcall Prog_Page_chk
	ori   r24,     0x02    ; Pre-mark page already matches
	cpi   r24,     0x03
	breq  Prog_Page_ret    ; Page already matches, do nothing

	; Wait for EEPROM to finish writing

	sbic  _SFR_IO_ADDR(EECR), EEPE
	rjmp  .-4

	; Erase the page

	ldi   r24,     (1 << PGERS) | (1 << SPMEN)
	rcall Prog_Page_spm

	; Re-enable RWW section

	ldi   r24,     (1 << RWWSRE) | (1 << SPMEN)
	rcall Prog_Page_spm

	; Transfer data to page buffer (The Z pointer remains the same at end)

Prog_Page_prgl:
	ld    r0,      X+
	ld    r1,      X+
	ldi   r24,     (1 << SPMEN)
	rcall Prog_Page_spm
	subi  ZL,      0xFE
	brne  Prog_Page_prgl
	clr   r1
	dec   XH               ; Restore X pointer for check

	; Page write

	ldi   r24,     (1 << PGWRT) | (1 << SPMEN)
	rcall Prog_Page_spm

	; Re-enable RWW section

	ldi   r24,     (1 << RWWSRE) | (1 << SPMEN)
	rcall Prog_Page_spm

	; Check whether programming was successful. The return value is all
	; right for this.

	; Fall through to Prog_Page_chk



;
; Check whether the page contains correct contents
;
; Inputs:
; XH: XL:  Source 256 byte buffer
; ZH: ZL:  Target 256 byte page address
; Outputs:
;     r24: 1 on success (page contains desired content), 0 on failure.
;     ZL:  Zero (as it should be)
; Clobbers:
; r0, r25
Prog_Page_chk:

	ldi   ZL,      0x00    ; Page in ROM
	ldi   r24,     0x01    ; Assume success normally
Prog_Page_chkl:
	lpm   r0,      Z+
	ld    r25,     X+
	cpse  r0,      r25
	ldi   r24,     0x00    ; Comparison failed
	cpi   ZL,      0x00
	brne  Prog_Page_chkl
	dec   ZH
	dec   XH               ; Restore source & dest pointers
Prog_Page_ret:
	ret



;
; Performs SPM, disabling & enabling interrupts around it. EEPROM access must
; not be ongoing during this. Waits for the completion of any previous SPM.
;
; Inputs:
;     r24: Value for SPMCSR
; Clobbers:
; r0
;
Prog_Page_spm:

	cli
	out    _SFR_IO_ADDR(SPMCSR), r24
	spm
	sei                    ; Fall through to waiting for completion



;
; Wait for SPM completion
;
; Clobbers:
; r0
;
Prog_Page_spm_wait:

	in    r0,      _SFR_IO_ADDR(SPMCSR)
	sbrc  r0,      SPMEN
	rjmp  Prog_Page_spm_wait
	ret
