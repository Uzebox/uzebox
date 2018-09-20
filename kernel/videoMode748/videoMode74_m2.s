;
; Uzebox Kernel - Video Mode 748 Row mode 2 (Separator line)
; Copyright (C) 2018 Sandor Zsuga (Jubatian)
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;
; Uzebox is a reserved trade mark
;

.section .text



;
; Separator line with palette reload.
;
; Enters in cycle 1735.
;
; r14:r15: Row selector offset
;     r16: Scanline counter (Normally 0 => 223)
;     r17: Logical row counter
;     r19: m74_config
; r22:r23: Tile descriptors
;     r25: render_lines_count
;       X: VRAM, pointing at mode
;      YL: Zero
;      YH: Palette buffer
;
m74_mode2:

	sbi   SR_PORT, SR_PIN  ; ( 2) Deselect SPI RAM (Any prev. operation)
	ld    r20,     X+      ; ( 4) Mode & Flags
	cbi   SR_PORT, SR_PIN  ; ( 6) Select SPI RAM
	ldi   r24,     0x03    ; ( 7) Command: Read from SPI RAM
	out   SR_DR,   r24     ; ( 8)
	ld    r2,      X+      ; (10) Palette address low (None / RAM / ROM / SPI RAM)
	ld    r3,      X+      ; (12) Palette address high
	ld    r18,     X+      ; (14) Color of separator line (unless Color0 reload)
	ldi   r24,     0xFF    ; (15)
	mov   r12,     r24     ; (16) For SPI RAM
	ldi   r24,     0x00    ; (17)
	sbrc  r20,     7       ; (18 / 19) SPI RAM bank select
	ldi   r24,     0x01    ; (19)
	lpm   ZL,      Z       ; (22)
	lpm   ZL,      Z       ; (25)
	out   SR_DR,   r24     ; (26)



;
; Process Color 0 reloading if enabled. YL is zero at this point.
;
; Cycles: 49 (1810)
;
	rcall m74_repcol0      ; (49)



;
; The hsync_pulse part.
;
; Cycle counter is at 246 on its end.
;
	ldi   YL,      0       ; (1811)
	ld    r13,     Y       ; (1813) Color 0
	sbrc  r20,     4
	mov   r18,     r13     ; (1815) Color 0 is used for the line
	lpm   ZL,      Z       ; (1818)
	lpm   ZL,      Z       ; (   1)
	nop                    ; (   2)
	inc   r16              ; (   3) Physical line counter
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	inc   r17              ; (   6) Logical line counter
	ldi   ZL,      2       ; (   7)
	out   SR_DR,   r12     ; (   8) SPI RAM dummy for first data fetch
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES



;
; Prepare for color replacements
;
; Cycles:  1 ( 247)
;
	movw  ZL,      r2      ; ( 1) For RAM / ROM loads (SPI RAM is initialized OK)



;
; Process line
;
	rcall m2_colrep        ; ( 303)
	M74WT_R24      50      ; ( 353)
	out   PIXOUT,  r18     ; ( 354)
	rcall m2_colrep        ; ( 410)
	rcall m2_colrep        ; ( 466)
	rcall m2_colrep        ; ( 522)
	rcall m2_colrep        ; ( 578)
	rcall m2_colrep        ; ( 634)
	rcall m2_colrep        ; ( 690)
	rcall m2_colrep        ; ( 746)
	rcall m2_colrep        ; ( 802)
	rcall m2_colrep        ; ( 858)
	rcall m2_colrep        ; ( 914)
	rcall m2_colrep        ; ( 970)
	rcall m2_colrep        ; (1026)
	rcall m2_colrep        ; (1082)
	rcall m2_colrep        ; (1138)
	rcall m2_colrep        ; (1194)
	M74WT_R24      500     ; (1694)
	clr   r18              ; (1695)
	rjmp  m74_scloop_sr    ; (1697)



;
; Routine to replace a palette byte
;
; r12: 0xFF (for SPI RAM)
; r20: Mode 2 config
;  YL: Palette index to process (high nybble), increments by 16.
;   Z: Palette source pointer
;
m2_colrep:
	sbrc  r20,     6
	rjmp  m2_colrep_23
	sbrs  r20,     5
	rjmp  m2_colrep_0      ; ( 5) No color replacing
	nop
	ld    r24,     Z+      ; ( 7) RAM source
	rjmp  m2_colrep_r      ; ( 9)
m2_colrep_23:
	sbrs  r20,     5
	rjmp  m2_colrep_2      ; ( 6) ROM source
	in    r24,     SR_DR   ; ( 6) SPI RAM source
	out   SR_DR,   r12     ; ( 7)
	rjmp  m2_colrep_r      ; ( 9)
m2_colrep_2:
	lpm   r24,     Z+      ; ( 9)
m2_colrep_r:
	rcall m74_setpalcol    ; (48) (3 + 36 cycles)
	inc   YL               ; (49)
	ret                    ; (53)
m2_colrep_0:
	M74WT_R24      44      ; (49)
	ret                    ; (53)
