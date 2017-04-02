;
; Uzebox Kernel - Video Mode 748 scanline loop
; Copyright (C) 2017 Sandor Zsuga (Jubatian)
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
; For usage notes and related see the main comments. The entry point is at
; m74_scloop.
;



;
; Audio notes and cycles remaining:
;
;             AUDIO_OUT_HSYNC_CYCLES  Rem.Cycles  Rem.Blank
;
; 5CH + UART: .................. 258 ...... 1541 ...... 197
; 5CH ......: .................. 212 ...... 1587 ...... 243
; 4CH + UART: .................. 213 ...... 1586 ...... 242
; 4CH ......: .................. 167 ...... 1632 ...... 288
;
; Rem.Cycles are with the use of hsync_pulse
;



;
; hsync_pulse notes:
;
; It takes 18 + 3 (rcall) cycles without the audio. Its first four
; instructions (and the 3 cycles of rcall) matters. The sync_pulse variable
; may be skipped altogether for the scanline loop if it is fixed afterwards.
;
; In total so 3 + 2 + 9 = 14 cycles can be gained by dropping it, to do its
; job directly.
;



.section .text



;
; Core tile output loop.
;
; r0, r1: Temp
; r2, r3, r4, r5, r6, r7, r8, r9: Preloaded pixels for the tile
; r10: ROM 4bpp tiles 0x00 - 0x3F, offset high
; r11: ROM 4bpp tiles 0x40 - 0x7F, offset high
; r12: ROM 4bpp tiles 0x80 - 0xBF, offset high
; r13: Row selection offset
; r18: Remaining pixels - 8. After the line, also for the last partial tile.
; r21: RAM 4bpp tiles 0xC0 - 0xFF, offset high
; r24: 32 (for tile offset multiplication) (1bpp uses it for px. 0 color)
; X: Palette (only XH loaded, XL is from color)
; Y: Palette (only YH loaded, YL is from color)
; Z: Tile data loading
; Stack: VRAM offset
; r14, r15, r16, r17, r19, r20, r22, r23, r25: Unused
;
; r18: At end, 0xF8 - 0xFF may be in this reg (if used correctly). This
;      indicates the pixels to render at end of line in the low 3 bits (0xF8:
;      8px, 0xFF: 1px).
;
; Partial tile at the begin & end:
; 1 - 8 pixels may display on the end, and 0 - 7 on the begin. 8 pixels would
; mean no scrolling. This allows for having all tiles interleaved when no
; scrolling is present, so only one column of extra code is necessary, only
; for types actually supported for scrolling.
;
; The Video Stack is empty at this point, so can be restored to its base.
;

cramt:
	; 4bpp RAM tiles
	add   ZH,      r21     ; (13) RAM tiles adjusted base
	add   ZL,      r13     ; (14) Row select
	out   PIXOUT,  r4      ; (15) Pixel 2
	ld    YL,      Z+      ; (17)
	ld    XL,      Z+      ; (19)
	ld    r2,      Y       ; (21)
	out   PIXOUT,  r5      ; (22) Pixel 3
	ld    r4,      X       ; (24)
	swap  YL               ; (25)
	swap  XL               ; (26)
	ld    r3,      Y       ; (28)
	out   PIXOUT,  r6      ; (29) Pixel 4
	ld    r5,      X       ; (31)
	ld    YL,      Z+      ; (33)
	ld    XL,      Z+      ; (35)
	out   PIXOUT,  r7      ; (36) Pixel 5
	lpm   r0,      Z       ; (39) Dummy load (nop)
	rjmp  c4tail           ; (41)
cloop:
	; Normal loop entry point
	out   PIXOUT,  r2      ; ( 1) Pixel 0
centry:
	; Scanline render entry point (at cycle 1)
	pop   ZL               ; ( 3)
	muls  ZL,      r24     ; ( 5) cccc sttt ttt0 0000
	movw  ZL,      r0      ; ( 6)
	andi  ZH,      0x07    ; ( 7) Mask to select one of 64 tiles (cy preserved)
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	brcc  cb007f           ; ( 9 / 10)
	sbrc  r1,      3       ; (10 / 11)
	rjmp  cramt            ; (12)
	add   ZH,      r12     ; (12) ROM 4bpp, tiles 0x80 - 0xBF base
	rjmp  c4com            ; (14)
cb007f:
	mov   r0,      r10     ; (11) ROM 4bpp, tiles 0x00 - 0x3F base
	sbrc  r1,      3       ; (12 / 13)
	mov   r0,      r11     ; (13) ROM 4bpp, tiles 0x40 - 0x7F base
	add   ZH,      r0      ; (14)
c4com:
	; 4bpp ROM tiles
	out   PIXOUT,  r4      ; (15) Pixel 2
	add   ZL,      r13     ; (16) Row select
	lpm   YL,      Z+      ; (19)
	ld    r2,      Y       ; (21)
	out   PIXOUT,  r5      ; (22) Pixel 3
	swap  YL               ; (23)
	ld    r3,      Y       ; (25)
	lpm   XL,      Z+      ; (28)
	out   PIXOUT,  r6      ; (29) Pixel 4
	ld    r4,      X       ; (31)
	swap  XL               ; (32)
	lpm   YL,      Z+      ; (35)
	out   PIXOUT,  r7      ; (36) Pixel 5
	ld    r5,      X       ; (38)
	lpm   XL,      Z+      ; (41)
c4tail:
	subi  r18,     8       ; (42) Bill count of pixels output
	out   PIXOUT,  r8      ; (43) Pixel 6
	ld    r6,      Y       ; (45)
	ld    r8,      X       ; (47)
	swap  YL               ; (48)
	swap  XL               ; (49)
	out   PIXOUT,  r9      ; (50) Pixel 7
	ld    r7,      Y       ; (52)
	ld    r9,      X       ; (54)
	brcc  cloop            ; (55 / 56 = 0)
cend0:
	; Trailing pixels part for X scrolling. 1 - 8 pixels are output here
	; (including Pixel 0 before the jump)
	ldi   ZL,      lo8(M74_VIDEO_STACK + 15) ; (56 = 0)
	out   PIXOUT,  r2      ; ( 1) Pixel 0
	ldi   ZH,      hi8(M74_VIDEO_STACK + 15) ; ( 2)
	out   STACKL,  ZL      ; ( 3) Restore the empty Video stack
	out   STACKH,  ZH      ; ( 4)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	lpm   r0,      Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r4      ; (15) Pixel 2
	lpm   r0,      Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r5      ; (22) Pixel 3
	lpm   r0,      Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	lpm   r0,      Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	lpm   r0,      Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	lpm   r0,      Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	lpm   r0,      Z       ; () Dummy read (nop)
	clr   r18              ; () Just make sure it is zero
	rjmp  cend             ; () (when having goofed up 6cy / pixel output)



;
; Return for exiting the scanline loop
;
sclpret:
	rjmp  m74_scloopr      ; (1703)



cend:
	;
	; At this point 25 tiles worth of cycles were consumed, the cycle
	; counter should be at:
	;  227 (HSync)
	;   20 (Preparation code)
	;   51 (Prolog)
	;   -1 (Cycles after entry before Pixel 0)
	; 1400 (25 tiles including the trailing part)
	; ----
	; 1697
	;
m74_scloop_sr:
	out   PIXOUT,  r18     ; (1698) Right border (blanking) starts (r18 zero)



;
; Loop test condition: If the scanline counter reached the count of lines to
; render, return. It is possible to request zero lines. Note: NO CALL! It
; resets the Video Stack to its base!
;
; r14:r15: Row selector offset (m74_rows + 2)
; r16: Scanline counter (Normally 0 => 223)
; r17: Logical row counter (init from m74_rows[0])
; r19: m74_config
; r22:r23: Tile descriptors (m74_tdesc)
; r25: render_lines_count
;
; Entry is at cycle 1698. (the rjmp must be issued at 1696)
; Exit is at 1703.
;
m74_scloop:
	cp    r25,     r16     ; (1699)
	breq  sclpret          ; (1700 / 1701)
	clr   YL               ; (1701) YL is used as a zero register until Color0 reload



;
; Row management code.
;
; r14:r15: Row selector offset
; r16: Scanline counter (Normally 0 => 223)
; r17: Logical row counter
; r19: m74_config
; r22:r23: Tile descriptors
; r25: render_lines_count
; YL:  Zero
;
; Cycles:
;  11 (Row select)
;  40 (Tile descriptor load)
;  16 (Tile index load)
; ---
;  67
;
	;
	; Select row
	;
	movw  ZL,      r14     ; ( 1)
	ld    r24,     Z+      ; ( 3)
	cp    r24,     r16     ; ( 4)
	breq  mresp            ; ( 5 /  6) At new split point if equal
	sbiw  ZL,      2       ; ( 7) No new line: just load prev. X shift
	ld    r20,     Z       ; ( 9)
	rjmp  mrese            ; (11)
mresp:
	ld    r17,     Z+      ; ( 8) Load new logical row counter
	ld    r20,     Z+      ; (10) Load new X shift
	movw  r14,     ZL      ; (11)
mrese:
	;
	; Load tile descriptors
	;
	movw  ZL,      r22     ; ( 1)
	mov   r24,     r17     ; ( 2)
	lsr   r24              ; ( 3)
	lsr   r24              ; ( 4)
	lsr   r24              ; ( 5) Tile descriptor offset from log. row counter
	add   ZL,      r24     ; ( 6)
	adc   ZH,      YL      ; ( 7)
	sbrs  r19,     1       ; ( 8 /  9)
	rjmp  mrtiro           ; (10)
	ld    ZL,      Z       ; (11) Tile descriptor index from RAM
	rjmp  mrtira           ; (13)
mrtdra:
	subi  ZL,      lo8(-(M74_RAMTD_OFF - 128)) ; (18)
	sbci  ZH,      hi8(-(M74_RAMTD_OFF - 128)) ; (19)
	ld    r18,     Z       ; (21)
	andi  r18,     0x07    ; (22)
	cpi   r18,     0x04    ; (23) SPI RAM Mode 4?
	brne  .+6              ; (24 / 25)
	in    r2,      SR_DR   ; (25)
	out   SR_DR,   r2      ; (26) (1738) => 6x SPI loads fit before call update_sound
	rjmp  m74_spi4         ; (28) (1740) SPI RAM mode 4
	ld    r18,     Z+      ; (27)
	lpm   r21,     Z       ; (30) Dummy load (nop)
	ld    r10,     Z+      ; (32)
	ld    r11,     Z+      ; (34)
	ld    r12,     Z+      ; (36)
	ld    r21,     Z+      ; (38)
	rjmp  mrtdco           ; (40)
mrtiro:
	lpm   ZL,      Z       ; (13) Tile descriptor index from ROM
mrtira:
	clr   ZH               ; (14)
	sbrc  ZL,      7       ; (15 / 16) Bit 7 zero: ROM
	rjmp  mrtdra           ; (17)
	subi  ZL,      lo8(-(M74_ROMTD_OFF)) ; (17)
	sbci  ZH,      hi8(-(M74_ROMTD_OFF)) ; (18)
	lpm   r18,     Z       ; (21)
	andi  r18,     0x07    ; (22)
	cpi   r18,     0x04    ; (23) SPI RAM Mode 4?
	brne  .+6              ; (24 / 25)
	in    r2,      SR_DR   ; (25)
	out   SR_DR,   r2      ; (26) (1738) => 6x SPI loads fit before call update_sound
	rjmp  m74_spi4         ; (28) (1740) SPI RAM mode 4
	lpm   r18,     Z+      ; (28)
	lpm   r10,     Z+      ; (31)
	lpm   r11,     Z+      ; (34)
	lpm   r12,     Z+      ; (37)
	lpm   r21,     Z+      ; (40)
mrtdco:
	;
	; Load tile indices
	;
	lds   ZL,      m74_tidx_lo  ; ( 2)
	lds   ZH,      m74_tidx_hi  ; ( 4)
	lsl   r24              ; ( 5)
	add   ZL,      r24     ; ( 6)
	adc   ZH,      YL      ; ( 7)
	sbrs  r19,     2       ; ( 8 /  9)
	rjmp  mtdroi           ; (10)
	; RAM tile index list
	ld    XL,      Z+      ; (11)
	ld    XH,      Z+      ; (13)
	nop                    ; (14)
	rjmp  mtdrie           ; (16)
mtdroi:
	; ROM tile index list
	lpm   XL,      Z+      ; (13)
	lpm   XH,      Z+      ; (16)
mtdrie:



;
; Process Color 0 reloading if enabled. YL is zero at this point.
;
; 40 cycles
;
; At 1808 at the end
;
#if (M74_COL0_OFF != 0)
	sbrs  r19,     4       ; ( 1 /  2)
	rjmp  c0rl0            ; ( 3) Reload disabled
	mov   ZL,      r17     ; ( 3) Create Color0 table from logical scanline ctr.
	ldi   ZH,      hi8(M74_COL0_OFF) ; ( 4)
	ld    r8,      Z       ; ( 6) Color0 table
	st    Y+,      r8      ; ( 8)
	st    Y+,      r8      ; (10)
	st    Y+,      r8      ; (12)
	st    Y+,      r8      ; (14)
	st    Y+,      r8      ; (16)
	st    Y+,      r8      ; (18)
	st    Y+,      r8      ; (20)
	st    Y+,      r8      ; (22)
	st    Y+,      r8      ; (24)
	st    Y+,      r8      ; (26)
	st    Y+,      r8      ; (28)
	st    Y+,      r8      ; (30)
	st    Y+,      r8      ; (32)
	st    Y+,      r8      ; (34)
	st    Y+,      r8      ; (36)
	st    Y+,      r8      ; (38)
	rjmp  c0rle            ; (40)
c0rl0:
	M74WT_R24      37      ; (40)
c0rle:
#else
	M74WT_R24      40      ; (40)
#endif



;
; Branch off to other modes
;
; At 1812 at the end
;
	sbrc  r18,     2       ; ( 1 / 2)
	rjmp  m74_spi567       ; ( 3) (1811) Other SPI modes
#if (M74_M2_ENABLE == 0U)
	rjmp  .                ; ( 4)
#else
	sbrc  r18,     1       ; ( 3 / 4)
	rjmp  m74_m2_separator ; ( 5) (1813) Separator line
#endif



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
; From the prolog some calculations are moved here so to make it possible
; to shift the display a bit around for proper centering.
;
; Cycle counter is at 246 on its end
;
	sbiw  XL,      1       ; (1814) Adjust for stack (pre-incrementing)
	movw  r4,      XL      ; (1815) r5:r4, XH:XL, putting it aside
	mov   XH,      r17     ; (1816)
	andi  XH,      0x07    ; (1817)
	lsl   XH               ; (1818) Prepare 4bpp row select
	lsl   XH               ; (1819)
	mov   r13,     XH      ; (1820 = 0) Done
	mov   XH,      YH      ; (   1) XH is also a palette pointer
	clr   r2               ; (   2) Leftmost pixel of partial tile (scroll) is zero
	nop                    ; (   3)
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	ldi   r18,     192     ; (   6) No. of pixels to output
	andi  r20,     0x07    ; (   7) Mask X shift to 0 - 7
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES



;
; Prepare for rendering mode 0
;
; Enters in cycle 246.
;
	ldi   r24,     32      ; ( 1) Load 32 for 4bpp tile offset calculation



;
; Prolog code components:
;
; An off-scanline renderer to produce the first partial tile column. It only
; runs if a nonzero X scroll is present. It fills up the r2 - r9 regs for the
; first in-scanline tile visible.
;
; A cleanup code which clears r2 - r9 as necessary to clip the left edge of
; the display. When no X scroll is present, all regs start cleared. Note that
; r2 is always cleared, so its generation is trimmed from the code to save a
; few cycles.
;
; A delay code which shifts the beginning of the scanline according to the
; scroll.
;
; r20: X scroll: 0 - 7, shifting to the left
;
; Other regs are (also) set up according to the requirements of the scanline
; loop.
;
; Enters in cycle 247.
;
; 51 cycles
;
	inc   r16              ; ( 1) Increment the physical line counter
	inc   r17              ; ( 2) Increment the logical line counter
	cpi   r20,     0       ; ( 3)
	breq  pnscr            ; ( 4 /  5) If not scrolling, then no partial loads
	;
	; Partial tile load code
	;
	; Supports only 8px wide tiles, either 1bpp or 4bpp. The rest (6px)
	; can not be scrolled by X (at least not past the left edge).
	;
	out   STACKL,  r4      ; ( 5)
	out   STACKH,  r5      ; ( 6) VRAM pointer set up
	pop   ZL               ; ( 8)
	muls  ZL,      r24     ; (10) cccc sttt ttt0 0000
	movw  ZL,      r0      ; (11)
	andi  ZH,      0x07    ; (12) Mask to select one of 64 tiles (cy preserved)
	brcc  pb007f           ; (13 / 14)
	sbrc  r1,      3       ; (14 / 15)
	rjmp  pramt            ; (16)
	add   ZH,      r12     ; (16) ROM 4bpp, tiles 0x80 - 0xBF base
	rjmp  p4com            ; (18)
pb007f:
	mov   r0,      r10     ; (15) ROM 4bpp, tiles 0x00 - 0x3F base
	sbrc  r1,      3       ; (16 / 17)
	mov   r0,      r11     ; (17) ROM 4bpp, tiles 0x40 - 0x7F base
	add   ZH,      r0      ; (18)
p4com:
	; 4bpp ROM tiles
	add   ZL,      r13     ; (19) Row select
	lpm   YL,      Z+      ; (22)
	lpm   XL,      Z+      ; (25)
	ld    r4,      X       ; (27)
	swap  YL               ; (28)
	swap  XL               ; (29)
	ld    r3,      Y       ; (31)
	ld    r5,      X       ; (33)
	lpm   YL,      Z+      ; (36)
	lpm   XL,      Z+      ; (39)
p4tail:
	ld    r6,      Y       ; (41)
	ld    r8,      X       ; (43)
	swap  YL               ; (44)
	swap  XL               ; (45)
	ld    r7,      Y       ; (47)
	ld    r9,      X       ; (49)
	;
	; Left side cleanup code. If r18 is 1, then 7 pixels are visible from
	; the partial tile, if it is 7, then 1 pixel. Then delay the output as
	; necessary and go on to render.
	;
	sub   r18,     r20     ; (50)
	cpi   r20,     7       ; (51)
	brne  pte123456        ; (52 / 53)
	clr   r3               ; (53)
	movw  r4,      r2      ; (54) r5:r4, r3:r2
	movw  r6,      r2      ; (55) r7:r6, r3:r2
	clr   r8               ; (56) Cleared all but r9.
ptej1:
	rjmp  centry           ; (58) Go for the scanline output! (At no scroll + 7)
	;
	; No scroll: Clean up all on the left (so omitting the use of a
	; partial tile)
	;
pnscr:
	M74WT_R24      36      ; (41)
	ldi   r24,     32      ; (42) For multiplications
	out   STACKL,  r4      ; (43)
	out   STACKH,  r5      ; (44) VRAM pointer set up
	clr   r3               ; (45)
	movw  r4,      r2      ; (46) r5:r4, r3:r2
	movw  r6,      r2      ; (47) r7:r6, r3:r2
	movw  r8,      r2      ; (48) r9:r8, r3:r2
	subi  r18,     8       ; (49)
	rjmp  centry           ; (51) Go for the scanline output!

	; Left side cleanup code pixels 2 - 7
pte123456:
	cpi   r20,     6       ; (54)
	brne  pte12345         ; (55 / 56)
	clr   r3               ; (56)
	movw  r4,      r2      ; (57) r5:r4, r3:r2
	movw  r6,      r2      ; (58) r7:r6, r3:r2
ptej2:
	lpm   r0,      Z       ; (61) Dummy load (nop)
	rjmp  ptej1            ; (63)
pte12345:
	cpi   r20,     5       ; (57)
	brne  pte1234          ; (58 / 59)
	clr   r3               ; (59)
	movw  r4,      r2      ; (60) r5:r4, r3:r2
	clr   r6               ; (61)
ptej3:
	rjmp  .                ; (63)
	rjmp  ptej2            ; (65)
pte1234:
	cpi   r20,     4       ; (60)
	brne  pte123           ; (61 / 62)
	clr   r3               ; (62)
	movw  r4,      r2      ; (63) r5:r4, r3:r2
ptej4:
	lpm   r0,      Z       ; (66) Dummy load (nop)
	rjmp  ptej3            ; (68)
pte123:
	cpi   r20,     3       ; (63)
	brne  pte12            ; (64 / 65)
	clr   r3               ; (65)
	clr   r4               ; (66)
ptej5:
	rjmp  .                ; (68)
	rjmp  ptej4            ; (70)
pte12:
	cpi   r20,     2       ; (66)
	brne  pte1             ; (67 / 68)
	clr   r3               ; (68)
ptej6:
	lpm   r0,      Z       ; (71) Dummy load (nop)
	rjmp  ptej5            ; (73)
pte1:
	lpm   r0,      Z       ; (71) Dummy load (nop)
	rjmp  .                ; (73)
	rjmp  ptej6            ; (75)

	; Components accessed with "rjmp", so could be put a bit off
pramt:
	; 4bpp RAM tiles
	add   ZH,      r21     ; (17) RAM tiles adjusted base
	add   ZL,      r13     ; (18) Row select
	ld    YL,      Z+      ; (20)
	ld    XL,      Z+      ; (22)
	ld    r4,      X       ; (24)
	swap  YL               ; (25)
	swap  XL               ; (26)
	ld    r3,      Y       ; (28)
	ld    r5,      X       ; (30)
	ld    YL,      Z+      ; (32)
	ld    XL,      Z+      ; (34)
	lpm   r0,      Z       ; (37) Dummy load (nop)
	rjmp  p4tail           ; (39)
