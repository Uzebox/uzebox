;
; Uzebox Kernel - Video Mode 748 scanline loop, Mode 4 (SPI RAM 4bpp)
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
; SPI RAM mode 4, entry point
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
m74_mode4:



;
; Prepare pointer for left side pixels
;
; 21 + 4 cycles
;
; At 1760 at the end
;
	; --- SPI ---
	in    r2,      SR_DR
	out   SR_DR,   r2
	; -----------
	M74WT_r24      11      ; (11)
	mov   r24,     r17     ; (12)
	andi  r24,     0x07    ; (13) Row select
	mov   r3,      r24
	lsl   r24
	add   r24,     r3      ; (16)
	; --- SPI ---
	in    r3,      SR_DR
	out   SR_DR,   r3
	; -----------
	lsl   r24
	lsl   r24              ; (18) * 12
	inc   r24
	add   XL,      r24
	adc   XH,      YL      ; (21) YL: Zero; X points at row to use



;
; Process Color 0 reloading if enabled. YL is zero at this point.
;
; 44 + 6 cycles
;
; At 1810 at the end
;
#if (M74_COL0_OFF != 0)
	sbrs  r19,     4       ; ( 1 /  2)
	rjmp  spi4c00          ; ( 3) Reload disabled
	mov   ZL,      r17     ; ( 3) Create Color0 table from logical scanline ctr.
	ldi   ZH,      hi8(M74_COL0_OFF) ; ( 4)
	ld    r8,      Z       ; ( 6) Color0 table
	st    Y+,      r8      ; ( 8)
	st    Y+,      r8      ; (10)
	st    Y+,      r8      ; (12)
	; --- SPI ---
	in    r4,      SR_DR
	out   SR_DR,   r4
	; -----------
	st    Y+,      r8      ; (14)
	st    Y+,      r8      ; (16)
	st    Y+,      r8      ; (18)
	st    Y+,      r8      ; (20)
	st    Y+,      r8      ; (22)
	st    Y+,      r8      ; (24)
	st    Y+,      r8      ; (26)
	st    Y+,      r8      ; (28)
	; --- SPI ---
	in    r5,      SR_DR
	out   SR_DR,   r5
	; -----------
	st    Y+,      r8      ; (30)
	st    Y+,      r8      ; (32)
	st    Y+,      r8      ; (34)
	st    Y+,      r8      ; (36)
	st    Y+,      r8      ; (38)
	M74WT_R24,     4       ; (42)
	rjmp  spi4c01          ; (44)
spi4c00:
	M74WT_R24       9      ; (12)
	; --- SPI ---
	in    r4,      SR_DR
	out   SR_DR,   r4
	; -----------
	M74WT_R24      16      ; (28)
	; --- SPI ---
	in    r5,      SR_DR
	out   SR_DR,   r5
	; -----------
	M74WT_R24      16      ; (44)
spi4c01:
#else
	M74WT_R24      12      ; (12)
	; --- SPI ---
	in    r4,      SR_DR
	out   SR_DR,   r4
	; -----------
	M74WT_R24      16      ; (28)
	; --- SPI ---
	in    r5,      SR_DR
	out   SR_DR,   r5
	; -----------
	M74WT_R24      16      ; (44)
#endif
	; --- SPI ---
	in    r6,      SR_DR
	out   SR_DR,   r6
	; -----------



;
; Filler
;
	M74WT_R24      10



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
	ldi   r20,     24      ; (   1) Count of tiles to render
	inc   r16              ; (   2) Increment the physical line counter
	inc   r17              ; (   3) Increment the logical line counter
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	ldi   ZL,      2       ; (   6)
	in    r7,      SR_DR   ; (   7)
	out   SR_DR,   r7      ; (   8) 6th SPI RAM byte before audio
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES



;
; Init buffer and copy bytes into it. Z: Buffer write pointer; X: VRAM
;
; 60 cycles
;
; At cycle 306 at the end
;
	in    r8,      SR_DR
	out   SR_DR,   r8
	ldi   ZL,      lo8(M74_M4_BUFFER)
	ldi   ZH,      hi8(M74_M4_BUFFER)
	ld    r0,      X+
	st    Z+,      r0
	ld    r0,      X+
	st    Z+,      r0
	ld    r0,      X+
	st    Z+,      r0
	ld    r0,      X+
	in    r9,      SR_DR
	out   SR_DR,   r9
	st    Z+,      r0
	ld    r0,      X+
	st    Z+,      r0
	ld    r0,      X+
	st    Z+,      r0
	ld    r0,      X+
	st    Z+,      r0
	ld    r0,      X+
	in    r10,     SR_DR
	out   SR_DR,   r10
	st    Z+,      r0
	ld    r0,      X+
	st    Z+,      r0
	ld    r0,      X+
	st    Z+,      r0
	ld    r0,      X+
	st    Z+,      r0
	ld    r0,      X+
	in    r11,     SR_DR
	out   SR_DR,   r11
	st    Z+,      r0      ; 12 bytes copied OK
	ldi   XL,      lo8(M74_M4_BUFFER)
	ldi   XH,      hi8(M74_M4_BUFFER)



;
; Continue streaming SPI bytes, filling in 12 additional bytes of the buffer.
;
; 34 cycles
;
; At cycle 340 at the end
;
	st    Z+,      r2
	st    Z+,      r3
	st    Z+,      r4
	st    Z+,      r5
	st    Z+,      r6
	st    Z+,      r7
	in    r2,      SR_DR
	out   SR_DR,   r2
	st    Z+,      r8
	st    Z+,      r9
	st    Z+,      r10
	st    Z+,      r11
	st    Z+,      r2
	lpm   r0,      Z       ; Dummy load (nop)
	lpm   r0,      Z       ; Dummy load (nop)
	in    r3,      SR_DR
	out   SR_DR,   r3
	st    Z+,      r3      ; 12 streamed bytes added OK



;
; SPI padding
;
; 9 cycles
;
; At cycle 349 at the end
;
	M74WT_R24      9



;
; Main scanline loop.
;
; First pixel out (Tile pixel 0) finishes at cycle 354. Enters at cycle 349.
;
spi4tl:
	ld    YL,      X+
	ld    r0,      Y
	out   PIXOUT,  r0      ; Tile pixel 0
	in    r2,      SR_DR
	out   SR_DR,   r2
	nop
	swap  YL
	ld    r1,      Y
	out   PIXOUT,  r1      ; Tile pixel 1
	st    Z+,      r2
	ld    YL,      X+
	ld    r0,      Y
	out   PIXOUT,  r0      ; Tile pixel 2
	swap  YL
	ld    r1,      Y
	nop
	in    r2,      SR_DR
	out   SR_DR,   r2
	out   PIXOUT,  r1      ; Tile pixel 3
	st    Z+,      r2
	ld    YL,      X+
	ld    r0,      Y
	out   PIXOUT,  r0      ; Tile pixel 4
	swap  YL
	ld    r1,      Y
	ld    YL,      X+
	dec   r20              ; Count of "tiles" (8px units)
	out   PIXOUT,  r1      ; Tile pixel 5
	breq  spi4te           ; Last 2 pixels
	nop
	in    r2,      SR_DR
	out   SR_DR,   r2
	ld    r0,      Y
	out   PIXOUT,  r0      ; Tile pixel 6
	st    Z+,      r2
	nop
	swap  YL
	ld    r1,      Y
	out   PIXOUT,  r1      ; Tile pixel 7
	rjmp  spi4tl
spi4te:
	in    YL,      SR_DR
	out   SR_DR,   YL
	ld    r0,      Y
	out   PIXOUT,  r0      ; (1684) Pixel 190
	swap  YL
	ld    r1,      Y
	lpm   r0,      Z       ; Dummy load (nop)
	out   PIXOUT,  r1      ; (1691) Pixel 191
	lpm   r0,      Z       ; Dummy load (nop)
	ldi   r18,     0
	rjmp  m74_scloop_sr    ; (1697)
