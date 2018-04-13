;
; Uzebox Kernel - Video Mode 748 scanline loop, Mode 6 and 7 (SPI RAM 1bpp)
; Copyright (C) 2018 Sandor Zsuga (Jubatian)
;
; -----
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
; -----
;



;
; Code block macro
;
; Register usage:
;
; r5: Foreground color
; r4: Background color
;
; A code block looks like as follows
;
;	out   PIXOUT,  r5/r4   ; Pixel 0 of tile
;	mov   r10,     r5/r4   ; Pixel 6 color
;	mov   r11,     r5/r4   ; Pixel 7 color
;	out   PIXOUT,  r5/r4   ; Pixel 1 of tile
;	rjmp  spi6_bx          ; 'x' depends on color of pixels 2-5
;
.macro CBLK c0, c1, c2, c3, c4, c5, c6, c7
.if     ((\c0) == 0)
	out   PIXOUT,  r4
.else
	out   PIXOUT,  r5
.endif
.if     ((\c6) == 0)
	mov   r10,     r4
.else
	mov   r10,     r5
.endif
.if     ((\c7) == 0)
	mov   r11,     r4
.else
	mov   r11,     r5
.endif
.if     ((\c1) == 0)
	out   PIXOUT,  r4
.else
	out   PIXOUT,  r5
.endif
.if     (((\c2) == 0) && ((\c3) == 0) && ((\c4) == 0) && ((\c5) == 0))
	rjmp  spi6_b0
.elseif (((\c2) == 1) && ((\c3) == 0) && ((\c4) == 0) && ((\c5) == 0))
	rjmp  spi6_b1
.elseif (((\c2) == 0) && ((\c3) == 1) && ((\c4) == 0) && ((\c5) == 0))
	rjmp  spi6_b2
.elseif (((\c2) == 1) && ((\c3) == 1) && ((\c4) == 0) && ((\c5) == 0))
	rjmp  spi6_b3
.elseif (((\c2) == 0) && ((\c3) == 0) && ((\c4) == 1) && ((\c5) == 0))
	rjmp  spi6_b4
.elseif (((\c2) == 1) && ((\c3) == 0) && ((\c4) == 1) && ((\c5) == 0))
	rjmp  spi6_b5
.elseif (((\c2) == 0) && ((\c3) == 1) && ((\c4) == 1) && ((\c5) == 0))
	rjmp  spi6_b6
.elseif (((\c2) == 1) && ((\c3) == 1) && ((\c4) == 1) && ((\c5) == 0))
	rjmp  spi6_b7
.elseif (((\c2) == 0) && ((\c3) == 0) && ((\c4) == 0) && ((\c5) == 1))
	rjmp  spi6_b8
.elseif (((\c2) == 1) && ((\c3) == 0) && ((\c4) == 0) && ((\c5) == 1))
	rjmp  spi6_b9
.elseif (((\c2) == 0) && ((\c3) == 1) && ((\c4) == 0) && ((\c5) == 1))
	rjmp  spi6_ba
.elseif (((\c2) == 1) && ((\c3) == 1) && ((\c4) == 0) && ((\c5) == 1))
	rjmp  spi6_bb
.elseif (((\c2) == 0) && ((\c3) == 0) && ((\c4) == 1) && ((\c5) == 1))
	rjmp  spi6_bc
.elseif (((\c2) == 1) && ((\c3) == 0) && ((\c4) == 1) && ((\c5) == 1))
	rjmp  spi6_bd
.elseif (((\c2) == 0) && ((\c3) == 1) && ((\c4) == 1) && ((\c5) == 1))
	rjmp  spi6_be
.else
	rjmp  spi6_bf
.endif
.endm



;
; 2.5Kbytes Code block table (unaligned)
;
spi6_blks:
	CBLK 0,0,0,0,0,0,0,0
	CBLK 0,0,0,0,0,0,0,1
	CBLK 0,0,0,0,0,0,1,0
	CBLK 0,0,0,0,0,0,1,1
	CBLK 0,0,0,0,0,1,0,0
	CBLK 0,0,0,0,0,1,0,1
	CBLK 0,0,0,0,0,1,1,0
	CBLK 0,0,0,0,0,1,1,1
	CBLK 0,0,0,0,1,0,0,0
	CBLK 0,0,0,0,1,0,0,1
	CBLK 0,0,0,0,1,0,1,0
	CBLK 0,0,0,0,1,0,1,1
	CBLK 0,0,0,0,1,1,0,0
	CBLK 0,0,0,0,1,1,0,1
	CBLK 0,0,0,0,1,1,1,0
	CBLK 0,0,0,0,1,1,1,1
	CBLK 0,0,0,1,0,0,0,0
	CBLK 0,0,0,1,0,0,0,1
	CBLK 0,0,0,1,0,0,1,0
	CBLK 0,0,0,1,0,0,1,1
	CBLK 0,0,0,1,0,1,0,0
	CBLK 0,0,0,1,0,1,0,1
	CBLK 0,0,0,1,0,1,1,0
	CBLK 0,0,0,1,0,1,1,1
	CBLK 0,0,0,1,1,0,0,0
	CBLK 0,0,0,1,1,0,0,1
	CBLK 0,0,0,1,1,0,1,0
	CBLK 0,0,0,1,1,0,1,1
	CBLK 0,0,0,1,1,1,0,0
	CBLK 0,0,0,1,1,1,0,1
	CBLK 0,0,0,1,1,1,1,0
	CBLK 0,0,0,1,1,1,1,1
	CBLK 0,0,1,0,0,0,0,0
	CBLK 0,0,1,0,0,0,0,1
	CBLK 0,0,1,0,0,0,1,0
	CBLK 0,0,1,0,0,0,1,1
	CBLK 0,0,1,0,0,1,0,0
	CBLK 0,0,1,0,0,1,0,1
	CBLK 0,0,1,0,0,1,1,0
	CBLK 0,0,1,0,0,1,1,1
	CBLK 0,0,1,0,1,0,0,0
	CBLK 0,0,1,0,1,0,0,1
	CBLK 0,0,1,0,1,0,1,0
	CBLK 0,0,1,0,1,0,1,1
	CBLK 0,0,1,0,1,1,0,0
	CBLK 0,0,1,0,1,1,0,1
	CBLK 0,0,1,0,1,1,1,0
	CBLK 0,0,1,0,1,1,1,1
	CBLK 0,0,1,1,0,0,0,0
	CBLK 0,0,1,1,0,0,0,1
	CBLK 0,0,1,1,0,0,1,0
	CBLK 0,0,1,1,0,0,1,1
	CBLK 0,0,1,1,0,1,0,0
	CBLK 0,0,1,1,0,1,0,1
	CBLK 0,0,1,1,0,1,1,0
	CBLK 0,0,1,1,0,1,1,1
	CBLK 0,0,1,1,1,0,0,0
	CBLK 0,0,1,1,1,0,0,1
	CBLK 0,0,1,1,1,0,1,0
	CBLK 0,0,1,1,1,0,1,1
	CBLK 0,0,1,1,1,1,0,0
	CBLK 0,0,1,1,1,1,0,1
	CBLK 0,0,1,1,1,1,1,0
	CBLK 0,0,1,1,1,1,1,1
	CBLK 0,1,0,0,0,0,0,0
	CBLK 0,1,0,0,0,0,0,1
	CBLK 0,1,0,0,0,0,1,0
	CBLK 0,1,0,0,0,0,1,1
	CBLK 0,1,0,0,0,1,0,0
	CBLK 0,1,0,0,0,1,0,1
	CBLK 0,1,0,0,0,1,1,0
	CBLK 0,1,0,0,0,1,1,1
	CBLK 0,1,0,0,1,0,0,0
	CBLK 0,1,0,0,1,0,0,1
	CBLK 0,1,0,0,1,0,1,0
	CBLK 0,1,0,0,1,0,1,1
	CBLK 0,1,0,0,1,1,0,0
	CBLK 0,1,0,0,1,1,0,1
	CBLK 0,1,0,0,1,1,1,0
	CBLK 0,1,0,0,1,1,1,1
	CBLK 0,1,0,1,0,0,0,0
	CBLK 0,1,0,1,0,0,0,1
	CBLK 0,1,0,1,0,0,1,0
	CBLK 0,1,0,1,0,0,1,1
	CBLK 0,1,0,1,0,1,0,0
	CBLK 0,1,0,1,0,1,0,1
	CBLK 0,1,0,1,0,1,1,0
	CBLK 0,1,0,1,0,1,1,1
	CBLK 0,1,0,1,1,0,0,0
	CBLK 0,1,0,1,1,0,0,1
	CBLK 0,1,0,1,1,0,1,0
	CBLK 0,1,0,1,1,0,1,1
	CBLK 0,1,0,1,1,1,0,0
	CBLK 0,1,0,1,1,1,0,1
	CBLK 0,1,0,1,1,1,1,0
	CBLK 0,1,0,1,1,1,1,1
	CBLK 0,1,1,0,0,0,0,0
	CBLK 0,1,1,0,0,0,0,1
	CBLK 0,1,1,0,0,0,1,0
	CBLK 0,1,1,0,0,0,1,1
	CBLK 0,1,1,0,0,1,0,0
	CBLK 0,1,1,0,0,1,0,1
	CBLK 0,1,1,0,0,1,1,0
	CBLK 0,1,1,0,0,1,1,1
	CBLK 0,1,1,0,1,0,0,0
	CBLK 0,1,1,0,1,0,0,1
	CBLK 0,1,1,0,1,0,1,0
	CBLK 0,1,1,0,1,0,1,1
	CBLK 0,1,1,0,1,1,0,0
	CBLK 0,1,1,0,1,1,0,1
	CBLK 0,1,1,0,1,1,1,0
	CBLK 0,1,1,0,1,1,1,1
	CBLK 0,1,1,1,0,0,0,0
	CBLK 0,1,1,1,0,0,0,1
	CBLK 0,1,1,1,0,0,1,0
	CBLK 0,1,1,1,0,0,1,1
	CBLK 0,1,1,1,0,1,0,0
	CBLK 0,1,1,1,0,1,0,1
	CBLK 0,1,1,1,0,1,1,0
	CBLK 0,1,1,1,0,1,1,1
	CBLK 0,1,1,1,1,0,0,0
	CBLK 0,1,1,1,1,0,0,1
	CBLK 0,1,1,1,1,0,1,0
	CBLK 0,1,1,1,1,0,1,1
	CBLK 0,1,1,1,1,1,0,0
	CBLK 0,1,1,1,1,1,0,1
	CBLK 0,1,1,1,1,1,1,0
	CBLK 0,1,1,1,1,1,1,1
	CBLK 1,0,0,0,0,0,0,0
	CBLK 1,0,0,0,0,0,0,1
	CBLK 1,0,0,0,0,0,1,0
	CBLK 1,0,0,0,0,0,1,1
	CBLK 1,0,0,0,0,1,0,0
	CBLK 1,0,0,0,0,1,0,1
	CBLK 1,0,0,0,0,1,1,0
	CBLK 1,0,0,0,0,1,1,1
	CBLK 1,0,0,0,1,0,0,0
	CBLK 1,0,0,0,1,0,0,1
	CBLK 1,0,0,0,1,0,1,0
	CBLK 1,0,0,0,1,0,1,1
	CBLK 1,0,0,0,1,1,0,0
	CBLK 1,0,0,0,1,1,0,1
	CBLK 1,0,0,0,1,1,1,0
	CBLK 1,0,0,0,1,1,1,1
	CBLK 1,0,0,1,0,0,0,0
	CBLK 1,0,0,1,0,0,0,1
	CBLK 1,0,0,1,0,0,1,0
	CBLK 1,0,0,1,0,0,1,1
	CBLK 1,0,0,1,0,1,0,0
	CBLK 1,0,0,1,0,1,0,1
	CBLK 1,0,0,1,0,1,1,0
	CBLK 1,0,0,1,0,1,1,1
	CBLK 1,0,0,1,1,0,0,0
	CBLK 1,0,0,1,1,0,0,1
	CBLK 1,0,0,1,1,0,1,0
	CBLK 1,0,0,1,1,0,1,1
	CBLK 1,0,0,1,1,1,0,0
	CBLK 1,0,0,1,1,1,0,1
	CBLK 1,0,0,1,1,1,1,0
	CBLK 1,0,0,1,1,1,1,1
	CBLK 1,0,1,0,0,0,0,0
	CBLK 1,0,1,0,0,0,0,1
	CBLK 1,0,1,0,0,0,1,0
	CBLK 1,0,1,0,0,0,1,1
	CBLK 1,0,1,0,0,1,0,0
	CBLK 1,0,1,0,0,1,0,1
	CBLK 1,0,1,0,0,1,1,0
	CBLK 1,0,1,0,0,1,1,1
	CBLK 1,0,1,0,1,0,0,0
	CBLK 1,0,1,0,1,0,0,1
	CBLK 1,0,1,0,1,0,1,0
	CBLK 1,0,1,0,1,0,1,1
	CBLK 1,0,1,0,1,1,0,0
	CBLK 1,0,1,0,1,1,0,1
	CBLK 1,0,1,0,1,1,1,0
	CBLK 1,0,1,0,1,1,1,1
	CBLK 1,0,1,1,0,0,0,0
	CBLK 1,0,1,1,0,0,0,1
	CBLK 1,0,1,1,0,0,1,0
	CBLK 1,0,1,1,0,0,1,1
	CBLK 1,0,1,1,0,1,0,0
	CBLK 1,0,1,1,0,1,0,1
	CBLK 1,0,1,1,0,1,1,0
	CBLK 1,0,1,1,0,1,1,1
	CBLK 1,0,1,1,1,0,0,0
	CBLK 1,0,1,1,1,0,0,1
	CBLK 1,0,1,1,1,0,1,0
	CBLK 1,0,1,1,1,0,1,1
	CBLK 1,0,1,1,1,1,0,0
	CBLK 1,0,1,1,1,1,0,1
	CBLK 1,0,1,1,1,1,1,0
	CBLK 1,0,1,1,1,1,1,1
	CBLK 1,1,0,0,0,0,0,0
	CBLK 1,1,0,0,0,0,0,1
	CBLK 1,1,0,0,0,0,1,0
	CBLK 1,1,0,0,0,0,1,1
	CBLK 1,1,0,0,0,1,0,0
	CBLK 1,1,0,0,0,1,0,1
	CBLK 1,1,0,0,0,1,1,0
	CBLK 1,1,0,0,0,1,1,1
	CBLK 1,1,0,0,1,0,0,0
	CBLK 1,1,0,0,1,0,0,1
	CBLK 1,1,0,0,1,0,1,0
	CBLK 1,1,0,0,1,0,1,1
	CBLK 1,1,0,0,1,1,0,0
	CBLK 1,1,0,0,1,1,0,1
	CBLK 1,1,0,0,1,1,1,0
	CBLK 1,1,0,0,1,1,1,1
	CBLK 1,1,0,1,0,0,0,0
	CBLK 1,1,0,1,0,0,0,1
	CBLK 1,1,0,1,0,0,1,0
	CBLK 1,1,0,1,0,0,1,1
	CBLK 1,1,0,1,0,1,0,0
	CBLK 1,1,0,1,0,1,0,1
	CBLK 1,1,0,1,0,1,1,0
	CBLK 1,1,0,1,0,1,1,1
	CBLK 1,1,0,1,1,0,0,0
	CBLK 1,1,0,1,1,0,0,1
	CBLK 1,1,0,1,1,0,1,0
	CBLK 1,1,0,1,1,0,1,1
	CBLK 1,1,0,1,1,1,0,0
	CBLK 1,1,0,1,1,1,0,1
	CBLK 1,1,0,1,1,1,1,0
	CBLK 1,1,0,1,1,1,1,1
	CBLK 1,1,1,0,0,0,0,0
	CBLK 1,1,1,0,0,0,0,1
	CBLK 1,1,1,0,0,0,1,0
	CBLK 1,1,1,0,0,0,1,1
	CBLK 1,1,1,0,0,1,0,0
	CBLK 1,1,1,0,0,1,0,1
	CBLK 1,1,1,0,0,1,1,0
	CBLK 1,1,1,0,0,1,1,1
	CBLK 1,1,1,0,1,0,0,0
	CBLK 1,1,1,0,1,0,0,1
	CBLK 1,1,1,0,1,0,1,0
	CBLK 1,1,1,0,1,0,1,1
	CBLK 1,1,1,0,1,1,0,0
	CBLK 1,1,1,0,1,1,0,1
	CBLK 1,1,1,0,1,1,1,0
	CBLK 1,1,1,0,1,1,1,1
	CBLK 1,1,1,1,0,0,0,0
	CBLK 1,1,1,1,0,0,0,1
	CBLK 1,1,1,1,0,0,1,0
	CBLK 1,1,1,1,0,0,1,1
	CBLK 1,1,1,1,0,1,0,0
	CBLK 1,1,1,1,0,1,0,1
	CBLK 1,1,1,1,0,1,1,0
	CBLK 1,1,1,1,0,1,1,1
	CBLK 1,1,1,1,1,0,0,0
	CBLK 1,1,1,1,1,0,0,1
	CBLK 1,1,1,1,1,0,1,0
	CBLK 1,1,1,1,1,0,1,1
	CBLK 1,1,1,1,1,1,0,0
	CBLK 1,1,1,1,1,1,0,1
	CBLK 1,1,1,1,1,1,1,0
	CBLK 1,1,1,1,1,1,1,1



;
; SPI RAM mode 6 & 7, entry point
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
m74_mode6:
m74_mode7:



;
; Prepare for Mode 6 & 7 scanline
;
; Cycles: 26 (1761)
;
	sbi   SR_PORT, SR_PIN  ; ( 2) Deselect SPI RAM (Any prev. operation)
	ld    r21,     X+      ; ( 4) Mode & Flags
	cbi   SR_PORT, SR_PIN  ; ( 6) Select SPI RAM
	ldi   r24,     0x03    ; ( 7) Command: Read from SPI RAM
	out   SR_DR,   r24     ; ( 8)
	ld    r2,      X+      ; (10) SPI RAM address low
	ld    r3,      X+      ; (12) SPI RAM address high
	ldi   r24,     0xFF    ; (13)
	mov   r12,     r24     ; (14) For SPI RAM
	mov   r24,     r17     ; (15)
	andi  r24,     7       ; (16)
	ldi   r20,     48      ; (17)
	mul   r24,     r20     ; (19) Start offset adjustment
	add   r2,      r0      ; (20)
	adc   r3,      r1      ; (21)
	sbc   r20,     r20     ; (22)
	sbrc  r21,     7       ; (23 / 24) SPI RAM bank select
	subi  r20,     0x01    ; (24)
	andi  r20,     0x01    ; (25)
	out   SR_DR,   r20     ; (26)



;
; Process Color 0 reloading if enabled. YL is zero at this point.
;
; Cycles: 49 (1810)
;
	rcall m74_repcol0      ; (49)



;
; The hsync_pulse part for the new scanline.
;
; Normally in "conventional" graphics modes this is an "rcall hsync_pulse"
; into the kernel, however here even those cycles were needed. I count the
; cycles of the line beginning with that rcall, so the hsync cbi ends on
; cycle 5.
;
; Note that the "sync_pulse" variable is not updated, which is normally a
; decrementing counter for managing the mode. It is not used within the
; display portion, so I only update it proper after the display ends (by
; subtracting r16, the amount of lines which skipped updating it).
;
; The "update_sound" function destroys r0, r1, Z and the T flag in SREG.
;
; HSYNC_USABLE_CYCLES:
; 234 (Allowing 4CH audio or either 5CH or the UART)
;
; Cycle counter is at 246 on its end
;
	ld    r5,      X+      ; (1812) Foreground color
	ld    r4,      X+      ; (1814) Background color
	ldi   YL,      0       ; (1815)
	ld    r6,      Y       ; (1817) Color 0
	sbrc  r21,     4       ; (1818 / 1819) Use Color 0 for Background?
	mov   r4,      r6      ; (1819)
	rjmp  .                ; (   1)
	inc   r16              ; (   2) Increment the physical line counter
	inc   r17              ; (   3) Increment the logical line counter
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	ldi   r20,     48      ; (   6) Count of tiles to render
	ldi   ZL,      2       ; (   7)
	out   SR_DR,   r12     ; (   8) SPI RAM dummy for first data fetch
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES



;
; Extra padding.
;
	M74WT_R24      81



;
; Prepare for code block entry
;
; Enter in cycle 337.
;
	in    r0,      SR_DR   ; ( 1)
	out   SR_DR,   r0      ; ( 2)
	bst   r21,     0       ; ( 3) Into T to select between attr (0) and no-attr (1)
	brts  .+2              ; ( 4 /  5)
	sbiw  XL,      2       ; ( 6) If attribute mode, then rewind VRAM to attributes
	brts  .                ; ( 7)
	ldi   r24,     5       ; ( 8) Code block size
	clr   r10              ; ( 9)
	clr   r11              ; (10)
	mul   r0,      r24     ; (12) r24 = 5, size of code blocks (words)
	movw  ZL,      r0      ; (13)
	subi  ZL,      lo8(-(pm(spi6_blks))) ; (14)
	rjmp  spi6_sl_entry    ; (16)



;
; Processing code blocks (16 blocks)
;
; Entry can be made in cycle 351 by ijmp after preparing the first tile.
; Alternatively in spi6_sl_entry, cycle 343, r10, r11 zero.
;
spi6_exit_0:
	out   PIXOUT,  r4      ; Pixel 3 of tile
	lpm   r0,      Z
spi6_exit_01:
	out   PIXOUT,  r4      ; Pixel 4 of tile
	rjmp  .
spi6_exit_02:
	out   PIXOUT,  r4      ; Pixel 5 of tile
	lpm   r0,      Z
spi6_exit_03:
	out   PIXOUT,  r10     ; Pixel 6 of tile
	rjmp  .
	out   PIXOUT,  r11     ; Pixel 7 of tile
	ldi   r18,     0
	rjmp  m74_scloop_sr    ; (1697)

spi6_b0:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r4      ; Pixel 2 of tile
	breq  spi6_exit_0
	in    r0,      SR_DR
	out   PIXOUT,  r4      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r4      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r4      ; Pixel 5 of tile
spi6_sl_entry:
	brts  spi6_noattr_0    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_b1:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r5      ; Pixel 2 of tile
	breq  spi6_exit_0
	in    r0,      SR_DR
	out   PIXOUT,  r4      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r4      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r4      ; Pixel 5 of tile
	brts  spi6_noattr_0    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_exit_1:
	out   PIXOUT,  r5      ; Pixel 3 of tile
	nop
	rjmp  spi6_exit_01

spi6_b2:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r4      ; Pixel 2 of tile
	breq  spi6_exit_1
	in    r0,      SR_DR
	out   PIXOUT,  r5      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r4      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r4      ; Pixel 5 of tile
	brts  spi6_noattr_0    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
spi6_noar_0:
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_noattr_0:
	nop
	out   PIXOUT,  r10     ; Pixel 6 of tile
	rjmp  spi6_noar_0

spi6_b3:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r5      ; Pixel 2 of tile
	breq  spi6_exit_1
	in    r0,      SR_DR
	out   PIXOUT,  r5      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r4      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r4      ; Pixel 5 of tile
	brts  spi6_noattr_0    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_exit_2:
	out   PIXOUT,  r4      ; Pixel 3 of tile
	lpm   r0,      Z
spi6_exit_21:
	out   PIXOUT,  r5      ; Pixel 4 of tile
	rjmp  spi6_exit_02

spi6_b4:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r4      ; Pixel 2 of tile
	breq  spi6_exit_2
	in    r0,      SR_DR
	out   PIXOUT,  r4      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r5      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r4      ; Pixel 5 of tile
	brts  spi6_noattr_0    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_b5:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r5      ; Pixel 2 of tile
	breq  spi6_exit_2
	in    r0,      SR_DR
	out   PIXOUT,  r4      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r5      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r4      ; Pixel 5 of tile
	brts  spi6_noattr_1    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_exit_3:
	out   PIXOUT,  r5      ; Pixel 3 of tile
	nop
	rjmp  spi6_exit_21

spi6_b6:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r4      ; Pixel 2 of tile
	breq  spi6_exit_3
	in    r0,      SR_DR
	out   PIXOUT,  r5      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r5      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r4      ; Pixel 5 of tile
	brts  spi6_noattr_1    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_b7:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r5      ; Pixel 2 of tile
	breq  spi6_exit_3
	in    r0,      SR_DR
	out   PIXOUT,  r5      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r5      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r4      ; Pixel 5 of tile
	brts  spi6_noattr_1    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
spi6_noar_1:
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_noattr_1:
	nop
	out   PIXOUT,  r10     ; Pixel 6 of tile
	rjmp  spi6_noar_1

spi6_exit_4:
	out   PIXOUT,  r4      ; Pixel 3 of tile
	lpm   r0,      Z
spi6_exit_41:
	out   PIXOUT,  r4      ; Pixel 4 of tile
	rjmp  .
spi6_exit_42:
	out   PIXOUT,  r5      ; Pixel 5 of tile
	nop
	rjmp  spi6_exit_03

spi6_b8:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r4      ; Pixel 2 of tile
	breq  spi6_exit_4
	in    r0,      SR_DR
	out   PIXOUT,  r4      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r4      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r5      ; Pixel 5 of tile
	brts  spi6_noattr_1    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_b9:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r5      ; Pixel 2 of tile
	breq  spi6_exit_4
	in    r0,      SR_DR
	out   PIXOUT,  r4      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r4      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r5      ; Pixel 5 of tile
	brts  spi6_noattr_1    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_exit_5:
	out   PIXOUT,  r5      ; Pixel 3 of tile
	nop
	rjmp  spi6_exit_41

spi6_ba:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r4      ; Pixel 2 of tile
	breq  spi6_exit_5
	in    r0,      SR_DR
	out   PIXOUT,  r5      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r4      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r5      ; Pixel 5 of tile
	brts  spi6_noattr_2    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_bb:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r5      ; Pixel 2 of tile
	breq  spi6_exit_5
	in    r0,      SR_DR
	out   PIXOUT,  r5      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r4      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r5      ; Pixel 5 of tile
	brts  spi6_noattr_2    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_exit_6:
	out   PIXOUT,  r4      ; Pixel 3 of tile
	lpm   r0,      Z
spi6_exit_61:
	out   PIXOUT,  r5      ; Pixel 4 of tile
	rjmp  spi6_exit_42

spi6_bc:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r4      ; Pixel 2 of tile
	breq  spi6_exit_6
	in    r0,      SR_DR
	out   PIXOUT,  r4      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r5      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r5      ; Pixel 5 of tile
	brts  spi6_noattr_2    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
spi6_noar_2:
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_noattr_2:
	nop
	out   PIXOUT,  r10     ; Pixel 6 of tile
	rjmp  spi6_noar_2

spi6_bd:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r5      ; Pixel 2 of tile
	breq  spi6_exit_6
	in    r0,      SR_DR
	out   PIXOUT,  r4      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r5      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r5      ; Pixel 5 of tile
	brts  spi6_noattr_2    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_exit_7:
	out   PIXOUT,  r5      ; Pixel 3 of tile
	nop
	rjmp  spi6_exit_61

spi6_be:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r4      ; Pixel 2 of tile
	breq  spi6_exit_7
	in    r0,      SR_DR
	out   PIXOUT,  r5      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r5      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r5      ; Pixel 5 of tile
	brts  spi6_noattr_2    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

spi6_bf:
	dec   r20              ; Remaining tiles
	out   PIXOUT,  r5      ; Pixel 2 of tile
	breq  spi6_exit_7
	in    r0,      SR_DR
	out   PIXOUT,  r5      ; Pixel 3 of tile
	out   SR_DR,   r0
	mul   r0,      r24     ; r24 = 5, size of code blocks (words)
	out   PIXOUT,  r5      ; Pixel 4 of tile
	movw  ZL,      r0
	subi  ZL,      lo8(-(pm(spi6_blks)))
	out   PIXOUT,  r5      ; Pixel 5 of tile
	brts  spi6_noattr_2    ; T set: No attributes
	ld    r4,      X+
	out   PIXOUT,  r10     ; Pixel 6 of tile
	ld    r5,      X+
	out   PIXOUT,  r11     ; Pixel 7 of tile
	sbci  ZH,      hi8(-(pm(spi6_blks)))
	ijmp

