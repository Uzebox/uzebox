/*
 *  Burns a .uze file in the ROM with progress bar
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
#include "kernel/defines.h"


.section .text



;
; Programs a complete .uze file into the ROM with a progress bar display
;
; The progress bar:
;
; --------------------------------------
;   >>##############################<<
; --------------------------------------
;
; One bar represents 2048 bytes (4 sectors).
;
; Both palettes color 0 is assumed to be black.
; Colors 8 - 15 are assumed to be gradients.
; Color 1 is used for framing.
;
; 17 pixels height is enabled of which the tile row 8 - 15 contains the bar.
;
; If programming is successful, the game is started.
;
; Note that the file position is not reset before programming to support
; uploading images contained within other files.
;
; Inputs:
; r25:r24: SD structure (with file selected for use)
;     r22: Delay before programming (frames)
; Does not return (reboots).
;
.global Prog_Uze
.global Prog_Uze_Boot_Game
Prog_Uze:

	; Save SD structure address & delay

	movw  r10,     r24
	mov   r9,      r22

	; Prepare display for loading bar

	ldi   r24,     (FIRST_RENDER_LINE + ((FRAME_LINES) / 2) - 13)
	ldi   r22,     17
	rcall SetRenderingParameters

	; Prepare line coloring

	ldi   r24,     0
	ldi   r22,     17
	rcall Graphics_ClearCol
	ldi   r24,     7
	rcall Graphics_GradBar

	; Prepare VRAM

	rcall Graphics_ClearVRAM
	ldi   YL,      lo8(vram + 40)
	ldi   YH,      hi8(vram + 40)
	ldi   r22,     ('>' - 0x20)
	st    Y+,      r22
	st    Y+,      r22     ; Positioned at loading bar beginning
	ldi   r22,     ('<' - 0x20)
	std   Y + 30,  r22
	std   Y + 31,  r22

	; Delay

	mov   r24,     r9
	rcall WaitVsync

	; Load file info

	movw  r24,     r10
	rcall FAT_Read_Sector
	cpse  r24,     r1
	rjmp  Prog_Uze_fail
	movw  ZL,      r10
	ldd   r20,     Z + 1
	ldd   r21,     Z + 2
	movw  ZL,      r20
	ldd   r20,     Z + 8
	ldd   r21,     Z + 9
	ldd   r22,     Z + 10
	ldd   r23,     Z + 11  ; Program size in r23:r22:r21:r20
	ldi   r19,     0xF0
	cpi   r20,     0x01
	cpc   r21,     r19     ; Must be < 61441 (0xF001)
	cpc   r22,     r1
	cpc   r23,     r1
	brcc  Prog_Uze_fail
	subi  r20,     0x01
	sbci  r21,     0xFE    ; Add 511 (0x01FF which is -0xFE01)
	lsr   r21
	breq  Prog_Uze_fail    ; Zero sectors is also invalid
	mov   r12,     r21     ; r12: Count of sectors to program

	; Program sectors

	ldi   r16,     0x00    ; Target sector in Flash
Prog_Uze_ploop:
	movw  r24,     r10     ; SD structure
	rcall FAT_Next_Sector
	cpse  r24,     r1
	rjmp  Prog_Uze_fail
	movw  r24,     r10
	rcall FAT_Read_Sector
	cpse  r24,     r1
	rjmp  Prog_Uze_fail
	movw  r24,     r10
	mov   r22,     r16
	rcall Prog_Sector
	cpi   r24,     0x01
	brcs  Prog_Uze_fail    ; 0: Failed
	ldi   r22,     95      ; Filled box (programming performed)
	breq  .+6              ; 1: Programming was necessary
	ld    r23,     Y       ; If it is already a "prog. was necessary" box
	cpse  r22,     r23     ; don't override with a hollow box
	ldi   r22,     96      ; 3: No programming: hollow box
	st    Y+,      r22
	sbrc  r16,     0
	sbrs  r16,     1
	sbiw  YL,      1       ; Target sector & 0x03 != 0x03: Revert increment
	inc   r16
	cpse  r16,     r12
	rjmp  Prog_Uze_ploop

	; Successfully reaching this point means the game is programmed

	ldi   r24,     45
	rcall WaitVsync

	; Boot game

Prog_Uze_Boot_Game:

	cli
	ldi   r24,     (1 << IVCE)
	ldi   r25,     0       ; Clear IVSEL: Move vectors to 0x0000
	out   _SFR_IO_ADDR(MCUCR), r24
	out   _SFR_IO_ADDR(MCUCR), r25
	jmp   0x0000

Prog_Uze_fail:

	; Couldn't program the game. Display error

	rcall Graphics_ClearVRAM
	ldi   r24,     lo8(vram + 49)
	ldi   r25,     hi8(vram + 49)
	ldi   r22,     res_text_prguzeerr
	rcall Res_Graphics_CopyROM

	; Wait 3 secs and return. Not the most elegant solution (a literal
	; "goto" back into the Game Selector), at least 'r9' was preserved
	; for it, so it will work normally after restoring the display
	; config.

	ldi   r24,     180
	rcall WaitVsync
	rcall SetRenderingParameters_Default
	rjmp  Game_Selector
