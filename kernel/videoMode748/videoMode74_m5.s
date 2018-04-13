;
; Uzebox Kernel - Video Mode 748 scanline loop, Mode 5 (SPI RAM 3bpp)
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
; SPI RAM mode 5, entry point
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
m74_mode5:



;
; Prepare for Mode 5 scanline
;
; Cycles: 26 (1761)
;
	sbi   SR_PORT, SR_PIN  ; ( 2) Deselect SPI RAM (Any prev. operation)
	ld    r20,     X+      ; ( 4) Mode & Flags
	cbi   SR_PORT, SR_PIN  ; ( 6) Select SPI RAM
	ldi   r24,     0x03    ; ( 7) Command: Read from SPI RAM
	out   SR_DR,   r24     ; ( 8)
	ld    r2,      X+      ; (10) SPI RAM address low
	ld    r3,      X+      ; (12) SPI RAM address high
	ldi   r24,     0xFF    ; (13)
	mov   r12,     r24     ; (14) For SPI RAM
	mov   r24,     r17     ; (15)
	andi  r24,     7       ; (16)
	ldi   r21,     72      ; (17)
	mul   r24,     r21     ; (19) Start offset adjustment
	add   r2,      r0      ; (20)
	adc   r3,      r1      ; (21)
	sbc   r21,     r21     ; (22)
	sbrc  r20,     7       ; (23 / 24) SPI RAM bank select
	subi  r21,     0x01    ; (24)
	andi  r21,     0x01    ; (25)
	out   SR_DR,   r21     ; (26)



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
	M74WT_R24      11      ; (   1)
	inc   r16              ; (   2) Increment the physical line counter
	inc   r17              ; (   3) Increment the logical line counter
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	ldi   r20,     24      ; (   6) Count of tiles to render
	ldi   ZL,      2       ; (   7)
	out   SR_DR,   r12     ; (   8) SPI RAM dummy for first data fetch
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES



;
; Extra padding.
;
	M74WT_R24      88



;
; Main scanline loop.
;
; Enters in cycle 334.
;
	in    ZL,      SR_DR
	out   SR_DR,   ZL      ; Load first byte
	mov   ZH,      YH
	mov   XH,      YH
	clr   r1
	rjmp  .
	rjmp  spi5te           ; ( 343)
spi5tl:
	out   PIXOUT,  r1      ; Tile pixel 5
	in    ZL,      SR_DR
	out   SR_DR,   ZL
	andi  XL,      0x77
	ld    r0,      X
	swap  XL
	out   PIXOUT,  r0      ; Tile pixel 6
	nop
	ld    r1,      X
spi5te:
	bst   ZL,      7
	bld   XL,      6
	bst   ZL,      3
	out   PIXOUT,  r1      ; Tile pixel 7
	bld   XL,      5
	andi  ZL,      0x77
	ld    r0,      Z
	swap  ZL
	in    YL,      SR_DR
	out   PIXOUT,  r0      ; Tile pixel 0
	out   SR_DR,   YL
	ld    r1,      Z
	rjmp  .
	bst   YL,      7
	out   PIXOUT,  r1      ; Tile pixel 1
	bld   XL,      4
	bst   YL,      3
	bld   XL,      2
	andi  YL,      0x77
	ld    r0,      Y
	out   PIXOUT,  r0      ; Tile pixel 2
	swap  YL
	ld    r1,      Y
	in    YL,      SR_DR
	out   SR_DR,   YL
	bst   YL,      7
	out   PIXOUT,  r1      ; Tile pixel 3
	bld   XL,      1
	bst   YL,      3
	bld   XL,      0
	andi  YL,      0x77
	ld    r0,      Y
	out   PIXOUT,  r0      ; Tile pixel 4
	swap  YL
	ld    r1,      Y
	dec   r20              ; Count of "tiles" (8px units)
	brne  spi5tl
	andi  XL,      0x77
	out   PIXOUT,  r1      ; (1677) Pixel 189
	ld    r0,      X
	swap  XL
	ld    r1,      X
	nop
	out   PIXOUT,  r0      ; (1684) Pixel 190
	lpm   r0,      Z       ; Dummy load (nop)
	lpm   r0,      Z       ; Dummy load (nop)
	out   PIXOUT,  r1      ; (1691) Pixel 191
	lpm   r0,      Z       ; Dummy load (nop)
	ldi   r18,     0
	rjmp  m74_scloop_sr    ; (1697)
