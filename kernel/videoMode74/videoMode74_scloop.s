;
; Uzebox Kernel - Video Mode 74 scanline loop
; Copyright (C) 2015 Sandor Zsuga (Jubatian)
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
; Tile bank location data
;
d_tbank2:
	.byte (M74_TBANK2_0_OFF >> 8) & 0xFF
	.byte (M74_TBANK2_1_OFF >> 8) & 0xFF
	.byte (M74_TBANK2_2_OFF >> 8) & 0xFF
	.byte (M74_TBANK2_3_OFF >> 8) & 0xFF
	.byte (M74_TBANK2_4_OFF >> 8) & 0xFF
	.byte (M74_TBANK2_5_OFF >> 8) & 0xFF
	.byte (M74_TBANK2_6_OFF >> 8) & 0xFF
	.byte (M74_TBANK2_7_OFF >> 8) & 0xFF



;
; Core tile output loop.
;
; r23: Zero
; r22, r0, r1: Temp
; r2, r3, r4, r5, r6, r7, r8, r9: Preloaded pixels for the tile
; r10: Tiles 0x00 - 0x7F, offset low (Unused in Mode 0)
; r11: Tiles 0x00 - 0x7F, offset high
; r12: ROM 4bpp tiles 0x80 - 0xBF, offset high
; r13: Pixel 1 color in 1bpp mode
; r18: Remaining pixels - 8. After the line, also for the last partial tile.
; r19: Mode for tiles 0x00 - 0x7F:
;      0b00000000: ROM 4bpp tiles
;      0b10000000: 2 bit RAM region
;      0b0xxxxxxx: 8px wide 1bpp tile
;      0b1xxxxxxx: 6px wide 1bpp tile (ROM source only)
;      0bx0xxxxxx: RAM 1bpp source / attr disabled in 6px wide mode
;      0bx1xxxxxx: ROM 1bpp source / attr enabled in 6px wide mode
; r20: RAM 4bpp tiles 0xC0 - 0xFF, offset low
; r21: RAM 4bpp tiles 0xC0 - 0xFF, offset high
; r24: Pixel 0 color in 1bpp / attribute mode or Temp
; r25: Pixel 0 color in 1bpp / attribute mode or Temp
; X: Tile index ROM / RAM address
; Y: Palette (only YH loaded, YL is from color)
; Z: Tile data loading
; r14, r15, r16, r17: Unused
;
; r18: At end, 0xF8 - 0xFF may be in this reg (if used correctly). This
;      indicates the pixels to render at end of line in the low 3 bits (0xF8:
;      8px, 0xFF: 1px).
;
; 6px wide attribute mode output:
; Color can be set for pixel 1. After 2 tile indices, 1 attribute byte
; follows, high nybble of first specifying fg. color for the first tile.
;
; Partial tile at the begin & end:
; 1 - 8 pixels may display on the end, and 0 - 7 on the begin. 8 pixels would
; mean no scrolling. This allows for having all tiles interleaved when no
; scrolling is present, so only one column of extra code is necessary, only
; for types actually supported for scrolling.
;
; The 6px wide 1bpp tile mode and the 2bpp region may wreck the output (if
; partially scrolled off on the right). This can not be worked around, and it
; is not really critical either (who uses these modes should use them
; properly).
;
cb80ff:
	mov   ZH,      r12     ; ( 4)
	lsl   ZL               ; ( 5)
	brcc  ccom             ; ( 6 /  7) 0x80 - 0xBF: ROM, 0xC0 - 0xFF: RAM
	mov   ZH,      r21     ; ( 7)
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	rjmp  cramt            ; (10)
clram:
	ld    ZL,      X+      ; (49) Load next tile index from RAM
	out   PIXOUT,  r9      ; (50) Pixel 7
	ld    r8,      Y       ; (52)
	swap  YL               ; (53)
	ld    r9,      Y       ; (55)
centry:
	; Pixel output loop entry point
	lsl   ZL               ; (56 = 0)
	out   PIXOUT,  r2      ; ( 1) Pixel 0
	brcs  cb80ff           ; ( 2)
	mov   ZH,      r11     ; ( 3)
	cpse  r19,     r23     ; ( 4 /  5) Mode nonzero: Special modes for 0x00 - 0x7F
	rjmp  csp00            ; ( 6)
	nop                    ; ( 6)
	lsl   ZL               ; ( 7)
ccom:
	; 4bpp ROM tile output
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	adc   ZH,      r23     ; ( 9) Use bit 6 of tile index for 0xC0 - 0xFF tiles
	lpm   YL,      Z+      ; (12)
	ld    r2,      Y       ; (14)
	out   PIXOUT,  r4      ; (15) Pixel 2
	swap  YL               ; (16)
	ld    r3,      Y       ; (18)
	lpm   YL,      Z+      ; (21)
	out   PIXOUT,  r5      ; (22) Pixel 3
	subi  r18,     8       ; (23) The resulting carry is used down in a brcc
	lpm   r0,      Z+      ; (26)
	ld    r4,      Y       ; (28)
	out   PIXOUT,  r6      ; (29) Pixel 4
	swap  YL               ; (30)
	ld    r5,      Y       ; (32)
	mov   YL,      r0      ; (33)
	ld    r6,      Y       ; (35)
	out   PIXOUT,  r7      ; (36) Pixel 5
	swap  YL               ; (37)
	ld    r7,      Y       ; (39)
	lpm   YL,      Z+      ; (42)
	out   PIXOUT,  r8      ; (43) Pixel 6
	rjmp  .                ; (45)
cvram:
	brcc  clram            ; (46 / 47)
cvrec:
	lpm   r0,      Z       ; (49) Dummy load (nop)
	out   PIXOUT,  r9      ; (50) Pixel 7
	ld    r8,      Y       ; (52)
	swap  YL               ; (53)
	ld    r9,      Y       ; (55)
	nop                    ; (56)
cend0:
	out   PIXOUT,  r2      ; ( 1) Pixel 0
	rjmp  cend1            ; ( 3)
cramt:
	; 4bpp RAM tile output
	add   ZL,      r20     ; (11)
	adc   ZH,      r23     ; (12) (Just carry, r23 is zero)
	ld    YL,      Z+      ; (14)
	out   PIXOUT,  r4      ; (15) Pixel 2
	ld    r2,      Y       ; (17)
	swap  YL               ; (18)
	ld    r3,      Y       ; (20)
	nop                    ; (21)
	out   PIXOUT,  r5      ; (22) Pixel 3
	ld    YL,      Z+      ; (24)
	ld    r4,      Y       ; (26)
	swap  YL               ; (27)
	nop                    ; (28)
	out   PIXOUT,  r6      ; (29) Pixel 4
	ld    r5,      Y       ; (31)
	ld    YL,      Z+      ; (33)
	ld    r6,      Y       ; (35)
	out   PIXOUT,  r7      ; (36) Pixel 5
	swap  YL               ; (37)
	ld    r7,      Y       ; (39)
	ld    YL,      Z+      ; (41)
	subi  r18,     8       ; (42)
	out   PIXOUT,  r8      ; (43) Pixel 6
	rjmp  cvram            ; (44 / 45) T clear: RAM tile index source
csp00b:
	rjmp  c1bp6            ; (14)
csp00:
	cpi   r19,     0x80    ; ( 7)
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	brpl  csp00a           ; ( 9 / 10)
	rjmp  c1bp8            ; (11)
csp00a:
	brne  csp00b           ; (11 / 12)
c2bpp:
	; 2bpp 2+ tile (16+ px / 4+ bytes) wide surface
	; ZL at this point is 0x00 - 0xFE, even, OK. ZH loaded proper.
	add   ZL,      r10     ; (12)
	adc   ZH,      r23     ; (13) (Just carry, r23 is zero)
	ldi   r22,     16      ; (14) Base width is 16 pixels (2 tiles)
	out   PIXOUT,  r4      ; (15) Pixel 2
	ldi   r24,     (M74_2BPP_WIDTH - 1) ; (16)
	mov   r0,      r24     ; (17)
c2bpple:
	ld    r24,     Z+      ; () Input tile, left (r2,r3,r4,r5)
	mov   YL,      r24     ; ()
	lsr   YL               ; ()
	out   PIXOUT,  r5      ; (22) Pixel 3
	lsr   YL               ; ()
	andi  r24,     0x33    ; ()
	andi  YL,      0x33    ; ()
	ld    r2,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	ld    r4,      Y       ; ()
	mov   YL,      r24     ; ()
	ld    r3,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	ld    r5,      Y       ; ()
	ld    r24,     Z+      ; () Input tile, right (r6,r7,r8,r9)
	mov   YL,      r24     ; ()
	lsr   YL               ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	lsr   YL               ; ()
	andi  r24,     0x33    ; ()
	andi  YL,      0x33    ; ()
	ld    r6,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	ld    r8,      Y       ; ()
	mov   YL,      r24     ; ()
	ld    r7,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r2      ; ( 1) Pixel 0
	ld    r9,      Y       ; ()
	dec   r0               ; ()
	brne  c2bppl           ; ( 5 / 6)
	ld    r24,     Z+      ; () Input tile, left (r2,r3,r4,r5)
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	mov   YL,      r24     ; ()
	lsr   YL               ; ()
	lsr   YL               ; ()
	andi  r24,     0x33    ; ()
	ld    r25,     Z+      ; () Input tile, right (r6,r7,r8,r9)
	out   PIXOUT,  r4      ; (15) Pixel 2
	andi  YL,      0x33    ; ()
	ld    r2,      Y       ; ()
	swap  YL               ; ()
	ld    r4,      Y       ; ()
	out   PIXOUT,  r5      ; (22) Pixel 3
	mov   YL,      r24     ; ()
	ld    r3,      Y       ; ()
	swap  YL               ; ()
	ld    r5,      Y       ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	nop                    ; ()
	mov   YL,      r25     ; ()
	lsr   YL               ; ()
	lsr   YL               ; ()
	andi  r25,     0x33    ; ()
	andi  YL,      0x33    ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	sub   r18,     r22     ; () Bill the number of pixels output
	ld    ZL,      X+      ; ()
	ld    r6,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	ld    r8,      Y       ; ()
	mov   YL,      r25     ; ()
	ld    r7,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	ld    r9,      Y       ; (52)
	brcs  c2bppe           ; (53 / 54)
	rjmp  centry           ; (55)
c2bppe:
	rjmp  cend0            ; (56)
c2bppl:
	subi  r22,     0xF8    ; () +8 pixels output
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	lpm   r24,     Z       ; () Dummy load (nop)
	lpm   r24,     Z       ; () Dummy load (nop)
	out   PIXOUT,  r4      ; (15) Pixel 2
	rjmp  c2bpple          ; ()
c1bp8:
	; 1bpp 8 pixels wide ROM / RAM tile output
	lsr   ZL               ; (12) Back to 0x00 - 0x7F range
	add   ZL,      r10     ; (13)
	adc   ZH,      r23     ; (14) (Just carry, r23 is zero)
	out   PIXOUT,  r4      ; (15) Pixel 2
	cpi   r19,     0x40    ; (16)
	brpl  .+4              ; (17 / 18)
	ld    r22,     Z+      ; (19) RAM tile
	rjmp  .+2              ; (21)
	lpm   r22,     Z+      ; (21) ROM tile
	out   PIXOUT,  r5      ; (22) Pixel 3
	movw  r2,      r24     ; () r3:r2, r25:r24
	movw  r4,      r24     ; () r5:r4, r25:r24
	sbrc  r22,     7       ; ()
	mov   r2,      r13     ; ()
	sbrc  r22,     6       ; ()
	mov   r3,      r13     ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	sbrc  r22,     5       ; ()
	mov   r4,      r13     ; ()
	sbrc  r22,     4       ; ()
	mov   r5,      r13     ; ()
	rjmp  .                ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	movw  r6,      r24     ; () r7:r6, r25:r24
	movw  r0,      r24     ; () r1:r0, r25:r24
	sbrc  r22,     3       ; ()
	mov   r6,      r13     ; ()
	sbrc  r22,     2       ; ()
	mov   r7,      r13     ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	sbrc  r22,     1       ; ()
	mov   r0,      r13     ; ()
	sbrc  r22,     0       ; ()
	mov   r1,      r13     ; ()
	ld    ZL,      X+      ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	movw  r8,      r0      ; () r9:r8, r1:r0
	subi  r18,     8       ; () Bill the number of pixels output
	brcs  c2bppe           ; (53 / 54)
	rjmp  centry           ; (55)
c1bp6a:
	; 1bpp 6 pixels wide ROM tile components
	rjmp  .                ; (41)
	mov   r22,     r13     ; (42) Replicate fg. color into r22
	out   PIXOUT,  r8      ; (43) Pixel 6
	rjmp  .                ; (45)
	rjmp  c1bp6b           ; (47)
c1bp6:
	; 4x 1bpp 6 pixels wide ROM tiles
	out   PIXOUT,  r4      ; (15) Pixel 2
	lsr   ZL               ; (16) Back to 0x00 - 0x7F range
	add   ZL,      r10     ; (17) Complete offset low (Tile 0)
	adc   ZH,      r23     ; (18) (Just carry, r23 is zero)
	lpm   r0,      Z+      ; (21) Tile 0 pixel data
	out   PIXOUT,  r5      ; (22) Pixel 3
	mov   ZH,      r11     ; (23)
	ld    ZL,      X+      ; (25)
	add   ZL,      r10     ; (26) Complete offset low (Tile 1)
	adc   ZH,      r23     ; (27) (Just carry, r23 is zero)
	movw  r2,      r24     ; (28) r3:r2, r25:r24
	out   PIXOUT,  r6      ; (29) Pixel 4
	movw  r4,      r24     ; (30) r5:r4, r25:r24
	lpm   r1,      Z+      ; (33) Tile 1 pixel data
	ld    ZL,      X+      ; (35)
	out   PIXOUT,  r7      ; (36) Pixel 5
	cpi   r19,     0xC0    ; (37)
	brmi  c1bp6a           ; (38 / 39) Attribute mode branch
	mov   YL,      ZL      ; (39)
	ld    r13,     Y       ; (41) First tile px 1 color in r13
	swap  YL               ; (42)
	out   PIXOUT,  r8      ; (43) Pixel 6
	ld    r22,     Y       ; (45) Second tile px 1 color in r22
	ld    ZL,      X+      ; (47)
c1bp6b:
	sbrc  r0,      7       ; ()
	mov   r2,      r13     ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	sbrc  r0,      6       ; ()
	mov   r3,      r13     ; ()
	sbrc  r0,      5       ; ()
	mov   r4,      r13     ; ()
	sbrc  r0,      4       ; ()
	mov   r5,      r13     ; ()
	out   PIXOUT,  r2      ; ( 1) Pixel 0 (Output tile 0 & 1)
	movw  r6,      r24     ; () r7:r6, r25:r24
	movw  r8,      r24     ; () r9:r8, r25:r24
	sbrc  r0,      3       ; ()
	mov   r6,      r13     ; ()
	sbrc  r0,      2       ; ()
	mov   r7,      r13     ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	sbrc  r1,      7       ; ()
	mov   r8,      r22     ; ()
	sbrc  r1,      6       ; ()
	mov   r9,      r22     ; ()
	movw  r2,      r24     ; () r3:r2, r25:r24
	add   ZL,      r10     ; () Complete offset low (Tile 2)
	out   PIXOUT,  r4      ; (15) Pixel 2
	adc   ZH,      r23     ; () (Just carry, r23 is zero)
	lpm   r0,      Z+      ; () Tile 2 pixel data
	sbrc  r1,      5       ; ()
	mov   r2,      r22     ; ()
	out   PIXOUT,  r5      ; (22) Pixel 3
	sbrc  r1,      4       ; ()
	mov   r3,      r22     ; ()
	movw  r4,      r24     ; () r5:r4, r25:r24
	nop                    ; ()
	sbrc  r1,      3       ; ()
	mov   r4,      r22     ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	sbrc  r1,      2       ; ()
	mov   r5,      r22     ; ()
	ld    ZL,      X+      ; ()
	add   ZL,      r10     ; () Complete offset low (Tile 3)
	adc   ZH,      r23     ; () (Just carry, r23 is zero)
	out   PIXOUT,  r7      ; (36) Pixel 5
	lpm   r1,      Z+      ; () Tile 3 pixel data
	ld    ZL,      X+      ; ()
	nop                    ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	lpm   r22,     Z       ; () Dummy read (nop)
	lpm   r22,     Z       ; () Dummy read (nop)
	out   PIXOUT,  r9      ; (50) Pixel 7
	lpm   r22,     Z       ; () Dummy read (nop)
	lpm   r22,     Z       ; () Dummy read (nop)
	out   PIXOUT,  r2      ; ( 1) Pixel 0 (Output tile 1 & 2)
	cpi   r19,     0xC0    ; ( 2)
	brmi  c1bp6c           ; ( 3 /  4) Attribute mode branch
	mov   YL,      ZL      ; ( 4)
	ld    r13,     Y       ; ( 6) First tile px 1 color in r13
	swap  YL               ; ( 7)
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	ld    r22,     Y       ; (10) Second tile px 1 color in r22
	ld    ZL,      X+      ; (12)
c1bp6d:
	movw  r6,      r24     ; () r7:r6, r25:r24
	movw  r8,      r24     ; () r9:r8, r25:r24
	out   PIXOUT,  r4      ; (15) Pixel 2
	sbrc  r0,      7       ; ()
	mov   r6,      r13     ; ()
	sbrc  r0,      6       ; ()
	mov   r7,      r13     ; ()
	sbrc  r0,      5       ; ()
	mov   r8,      r13     ; ()
	out   PIXOUT,  r5      ; (22) Pixel 3
	sbrc  r0,      4       ; ()
	mov   r9,      r13     ; ()
	movw  r2,      r24     ; () r3:r2, r25:r24
	movw  r4,      r24     ; () r5:r4, r25:r24
	sbrc  r0,      3       ; ()
	mov   r2,      r13     ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	sbrc  r0,      2       ; ()
	mov   r3,      r13     ; ()
	sbrc  r1,      7       ; ()
	mov   r4,      r22     ; ()
	sbrc  r1,      6       ; ()
	mov   r5,      r22     ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	mov   r6,      r24     ; ()
	sbrc  r1,      5       ; ()
	mov   r6,      r22     ; ()
	mov   r7,      r24     ; ()
	sbrc  r1,      4       ; ()
	mov   r7,      r22     ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	mov   r8,      r24     ; ()
	sbrc  r1,      3       ; ()
	mov   r8,      r22     ; ()
	mov   r0,      r24     ; ()
	sbrc  r1,      2       ; ()
	mov   r0,      r22     ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	mov   r9,      r0      ; ()
	subi  r18,     24      ; () Bill the number of pixels output
	brcs  c1bp6x           ; (53 / 54)
	rjmp  centry           ; (55)
c1bp6x:
	rjmp  cend0            ; (56)
c1bp6c:
	rjmp  .                ; ( 6)
	mov   r22,     r13     ; ( 7) Replicate fg. color into r22
	out   PIXOUT,  r2      ; ( 8) Pixel 1
	rjmp  .                ; (10)
	rjmp  c1bp6d           ; (12)
cend1:
	; Trailing pixels part for X scrolling. 1 - 8 pixels are output here
	; (including Pixel 0 before the jump)
	nop                    ; ( 4)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	lpm   r22,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r4      ; (15) Pixel 2
	lpm   r22,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r5      ; (22) Pixel 3
	lpm   r22,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	lpm   r22,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	lpm   r22,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	lpm   r22,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	lpm   r22,     Z       ; () Dummy read (nop)
	lpm   r22,     Z       ; () Dummy read (nop)
cend:
	;
	; At this point 25 tiles worth of cycles were consumed, the cycle
	; counter should be at:
	;  235 (HSync)
	;    3 (Horiz. size reduction)
	;    2 (Line counter increments)
	;   56 (Prolog)
	;    1 (Cycles after entry before Pixel 0)
	; 1400 (25 tiles including the trailing part)
	; ----
	; 1697
	;
m74_scloop_sr:
	out   PIXOUT,  r23     ; (1698) Right border (blanking) starts



;
; Horizontal size reduction, trailing part.
;
	lds   r20,     v_hsize ; (1700)
drloop:
	subi  r20,     8       ; ( 1)
	brcs  drend            ; ( 2 /  3) (1703)
#if (M74_SD_ENABLE != 0)
	M74WT_R24      15      ; (17)
	movw  ZL,      r14     ; (18) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (53) 35 cycles
	movw  r14,     ZL      ; (54) r15:r14, ZH:ZL Target pointer
#else
	M74WT_R24      52      ; (54)
#endif
	rjmp  drloop           ; (56 = 0)
	; Return for exiting the scanline loop
sclpret:
	ret                    ; (1712)
drend:



;
; Loop test condition: If the scanline counter reached the count of lines to
; render, return. It is possible to request zero lines.
;
; Entry is at cycle 1703. (the call must be issued at 1699)
; Exit is at 1712.
;
m74_scloop:
	lds   r22,     render_lines_count ; (1705)
	cp    r22,     r16     ; (1706)
	breq  sclpret          ; (1707 / 1708)
	clr   r23              ; (1708) r23 is used as a permanent zero register.



;
; Row management code
;
; r23: Zero
; r16: Scanline counter (Normally 0 => 223)
; r17: Logical row counter
;
; Cycles:
;  18 (Row select)
;  34 (Tile descriptor load)
;  27 (Precalc for row, first part)
; ---
;  79
;
	lds   r9,      m74_config   ; ( 2)
	lds   ZL,      v_rows_lo    ; ( 4)
	lds   ZH,      v_rows_hi    ; ( 6)
	ld    r25,     Z+      ; ( 8)
	cp    r25,     r16     ; ( 9)
	brne  mrespl           ; (10 / 11) Did not reach new split point yet
	ld    r17,     Z+      ; (12) Load new logical row counter
	ld    r25,     Z+      ; (14) Load new X shift
	sts   v_rows_lo, ZL    ; (16)
	sts   v_rows_hi, ZH    ; (18)
mrese:
	;
	; Load tile descriptors
	;
	lds   ZL,      m74_tdesc_lo ; ( 2)
	lds   ZH,      m74_tdesc_hi ; ( 4)
	mov   r24,     r17     ; ( 5)
	andi  r24,     0xF8    ; ( 6)
	lsr   r24              ; ( 7)
	lsr   r24              ; ( 8) Tile descriptor offset from log. row counter
	add   ZL,      r24     ; ( 9)
	adc   ZH,      r23     ; (10)
	sbrs  r9,      1       ; (11 / 12)
	rjmp  mtdro            ; (13)
	; RAM tile descriptors
	ld    r22,     Z+      ; (14)
	ld    YL,      Z+      ; (16)
	nop                    ; (17)
	rjmp  mtdre            ; (19)
mrespl:
	; Row select - no reload path
	sbiw  ZL,      2       ; (13) No new line: just load prev. X shift
	ld    r25,     Z       ; (15)
	nop                    ; (16)
	rjmp  mrese            ; (18)
mtdro:
	; ROM tile descriptors
	lpm   r22,     Z+      ; (16)
	lpm   YL,      Z+      ; (19)
mtdre:
	; Tile index list
	lds   ZL,      m74_tidx_lo  ; (21)
	lds   ZH,      m74_tidx_hi  ; (23)
	add   ZL,      r24     ; (24)
	adc   ZH,      r23     ; (25)
	sbrs  r9,      2       ; (26 / 27)
	rjmp  mtdroi           ; (28)
	; RAM tile index list
	ld    XL,      Z+      ; (29)
	ld    XH,      Z+      ; (31)
	nop                    ; (32)
	rjmp  mtdrie           ; (34)
mtdroi:
	; ROM tile index list
	lpm   XL,      Z+      ; (31)
	lpm   XH,      Z+      ; (34)
mtdrie:
	;
	; Prepare for scanline render
	;
	; Process tile row configuration
	mov   r19,     r22     ; ( 1)
	andi  r19,     0xE0    ; ( 2) Mode selector's final resting place is r19
	breq  mtnm0            ; ( 3 /  4) Is it mode 0 (ROM 4bpp)?
	sbrc  r19,     6       ; ( 4 /  5)
	sbrs  r19,     5       ; ( 5 /  6)
	rjmp  mtm12456         ; ( 7) Modes 1, 2, 4, 5, 6
#if   ((M74_M7_ENABLE == 0U) && (M74_M3_ENABLE == 0U))
	nop                    ; ( 7)
#elif ((M74_M7_ENABLE != 0U) && (M74_M3_ENABLE != 0U))
	sbrc  r19,     7       ; ( 7 /  8)
	rjmp  m74_m7_separator ; ( 9) Jump off to separator line
	rjmp  m74_m3_2bppmc    ; (10) Jump off to 2bpp Multicolor
#elif ((M74_M7_ENABLE != 0U))
	nop                    ; ( 7)
	rjmp  m74_m7_separator ; ( 9) Jump off to separator line
#else
	rjmp  .                ; ( 8)
	rjmp  m74_m3_2bppmc    ; (10) Jump off to 2bpp Multicolor
#endif
mtm12456:
	; Load 0x00 - 0x7F configuration for modes 1, 2, 4, 5, 6 & apply increment
	sbrs  YL,      1       ; ( 8 /  9)
	rjmp  mtst0x           ; (10)
	sbrs  YL,      0       ; (10 / 11)
	rjmp  mtst10           ; (12)
	ldi   ZL,      lo8(M74_TBANK01_3_OFF) ; (12)
	ldi   ZH,      hi8(M74_TBANK01_3_OFF) ; (13)
	movw  r10,     ZL                     ; (14)
#if (M74_TBANK01_3_INC != 0)
	ldi   ZL,      M74_TBANK01_3_INC      ; (15)
	nop                    ; (16)
	rjmp  mtste            ; (18)
#else
	rjmp  .                ; (16)
	rjmp  mtst256          ; (18)
#endif
mtnm0:
	; Load 0x00 - 0x7F configuration for mode 0 & apply increment
	ldi   ZL,      0       ; ( 5) Offset low is not used
	sbrs  YL,      1       ; ( 6 /  7)
	rjmp  mtm0t0x          ; ( 8)
	ldi   ZH,      hi8(M74_TBANKM0_3_OFF) ; ( 8)
	sbrs  YL,      0       ; ( 9 / 10)
	ldi   ZH,      hi8(M74_TBANKM0_2_OFF) ; (10)
	rjmp  mtm0te           ; (12)
mtm0t0x:
	ldi   ZH,      hi8(M74_TBANKM0_1_OFF) ; ( 9)
	sbrs  YL,      0       ; (10 / 11)
	ldi   ZH,      hi8(M74_TBANKM0_0_OFF) ; (11)
	nop                    ; (12)
mtm0te:
	movw  r10,     ZL      ; (13)
	mov   r18,     r17     ; (14) Logical row counter
	sbrs  YL,      3       ; (15 / 16)
	andi  r18,     0x7     ; (16) Only within tile row
	lsl   r18              ; (17)
	add   r11,     r18     ; (18) Apply row increment (512 bytes / row)
	andi  r18,     0xE     ; (19) Only within tile row (now always)
	lpm   ZL,      Z       ; (22) Dummy load (nop)
	lpm   ZL,      Z       ; (25) Dummy load (nop)
	rjmp  mtste0           ; (27)
mtst256:
	; The tileset's row increment is zero, to be interpreted as 256
	mov   r18,     r17     ; (19) Logical row counter
	sbrs  YL,      3       ; (20 / 21)
	andi  r18,     0x7     ; (21) Only within tile row
	add   r11,     r18     ; (22) Apply row increment (256)
	nop                    ; (23)
	rjmp  mtst256e         ; (25)
mtst10:
	ldi   ZL,      lo8(M74_TBANK01_2_OFF) ; (13)
	ldi   ZH,      hi8(M74_TBANK01_2_OFF) ; (14)
	movw  r10,     ZL                     ; (15)
#if (M74_TBANK01_2_INC != 0)
	ldi   ZL,      M74_TBANK01_2_INC      ; (16)
	rjmp  mtste            ; (18)
#else
	nop                    ; (16)
	rjmp  mtst256          ; (18)
#endif
mtst0x:
	; Continue row mode 1, 2, 4, 5, 6 configuration selects
	sbrs  YL,      0       ; (11 / 12)
	rjmp  mtst00           ; (13)
	ldi   ZL,      lo8(M74_TBANK01_1_OFF) ; (13)
	ldi   ZH,      hi8(M74_TBANK01_1_OFF) ; (14)
	movw  r10,     ZL                     ; (15)
#if (M74_TBANK01_1_INC != 0)
	ldi   ZL,      M74_TBANK01_1_INC      ; (16)
	rjmp  mtste            ; (18)
#else
	nop                    ; (16)
	rjmp  mtst256          ; (18)
#endif
mtst00:
	ldi   ZL,      lo8(M74_TBANK01_0_OFF) ; (14)
	ldi   ZH,      hi8(M74_TBANK01_0_OFF) ; (15)
	movw  r10,     ZL                     ; (16)
#if (M74_TBANK01_0_INC != 0)
	ldi   ZL,      M74_TBANK01_0_INC      ; (17)
	nop                    ; (18)
#else
	rjmp  mtst256          ; (18)
#endif
mtste:
	mov   r18,     r17     ; (19) Logical row counter
	sbrs  YL,      3       ; (20 / 21)
	andi  r18,     0x7     ; (21) Only within tile row
	mul   ZL,      r18     ; (23)
	add   r10,     r0      ; (24)
	adc   r11,     r1      ; (25)
mtst256e:
	andi  r18,     0x7     ; (26) Only within tile row (now always)
	lsl   r18              ; (27) Prepare for tileset offsetting
mtste0:




#if ((M74_SD_ENABLE != 0U) && (M74_SD_EXT != 0U))
;
; Do an SD load, then the HSync. If M74_SD_EXT is set, HSYNC_USABLE_CYCLES is
; 196, fitting the second part of the row address calculation after the Hsync.
; At the end of that, the cycle count is 235.
;
	movw  ZL,      r14     ; ( 1)
	rcall m74_spiload_core ; (36) 35 cycles

	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	movw  r14,     ZL      ; (   6)
	clr   r2               ; (   7) First output pixel is always zero (scroll)
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES
#endif



;
; Second part of row address calculations
;
	; Load 0xC0 - 0xFF configuration
	sbrs  YL,      2       ; ( 1 / 2)
	rjmp  mtsr0            ; ( 3)
	ldi   r20,     lo8(M74_TBANK3_1_OFF)  ; ( 3)
	ldi   r21,     hi8(M74_TBANK3_1_OFF)  ; ( 4)
	ldi   r24,     M74_TBANK3_1_INC       ; ( 5)
	rjmp  mtsre            ; ( 7)
mtsr0:
	ldi   r20,     lo8(M74_TBANK3_0_OFF)  ; ( 4)
	ldi   r21,     hi8(M74_TBANK3_0_OFF)  ; ( 5)
	ldi   r24,     M74_TBANK3_0_INC       ; ( 6)
	nop                    ; ( 7)
mtsre:
	; Load 0x80 - 0xBF configuration
	movw  ZL,      r22     ; ( 8) ZH:ZL, r23:r22 (r23 is zero)
	andi  ZL,      0x7     ; ( 9) 2Kb ROM half tile map selector
	subi  ZL,      lo8(-(d_tbank2)) ; (10)
	sbci  ZH,      hi8(-(d_tbank2)) ; (11)
	lpm   r12,     Z       ; (14)
	; Prepare horizontal size
	andi  r22,     0x18    ; (15) Mask off size bits
	sts   v_hsize, r22     ; (17) Save them
	; Mask X shift
	andi  r25,     0x7     ; (18)
	; Apply row increment on tiles 0x80 - 0xBF (r18 already contains row << 1)
	add   r12,     r18     ; (19)
	; Apply row increment on tiles 0xC0 - 0xFF
	lsl   r18              ; (20) Row counter was in 4 byte units
	mul   r18,     r24     ; (22)
	add   r20,     r0      ; (23)
	adc   r21,     r1      ; (24)
	; Calculate output width
	ldi   r18,     192     ; (25) No. of pixels to output
	sub   r18,     r22     ; (26) Reduced by output tile count
	sub   r18,     r22     ; (27)



#if ((M74_SD_ENABLE == 0U) || (M74_SD_EXT == 0U))
;
; Padding wait. This can not be added to audio time since it will push the
; display to the right. Nothing can be shuffled back to offset that.
;
	M74WT_R24      9       ; ( 9)
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
; 223 (Allowing 4CH audio + UART or 5CH audio)
;
; From the prolog some calculations are moved here so to make it possible
; to shift the display a bit around for proper centering.
;
; Cycle counter is at 235 on its end
;
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	nop                    ; (   6)
	clr   r2               ; (   7) First output pixel is always zero (scroll)
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES
#endif



;
; Horizontal size reduction as requested, in r22. 0, 1, 2 or 3 tiles worth of
; cycles are spent here without graphics output, r9 still contains the global
; config at this point.
;
#if (M74_COL0_RELOAD != 0)
dfloop0:
	subi  r22,     8       ; ( 1)
	brcs  dfend            ; ( 2 /  3)
	; Perform Color 0 reload under leftmost omitted tile if it was
	; enabled.
	sbrs  r9,      4       ; ( 3 /  4)
	rjmp  dfrl0            ; ( 5) Reload disabled
	mov   r0,      YL      ; ( 5) Put aside already loaded fg. palette index
#if (M74_COL0_PTRE != 0)
	lds   ZL,      m74_col0_lo  ; ( 7)
	lds   ZH,      m74_col0_hi  ; ( 9)
#else
	ldi   ZL,      lo8(M74_COL0_OFF) ; ( 6)
	ldi   ZH,      hi8(M74_COL0_OFF) ; ( 7)
	rjmp  .                ; ( 9)
#endif
	add   ZL,      r17     ; (10)
	adc   ZH,      r23     ; (11) (Just carry, r23 is zero)
	sbrs  r9,      5       ; (12 / 13)
	rjmp  .+4              ; (14)
	ld    r8,      Z       ; (15) Color0 table in RAM
	rjmp  .+2              ; (17)
	lpm   r8,      Z       ; (17) Color0 table in ROM
	clr   YL               ; (18)
dfrl02:
	st    Y+,      r8      ; (20 / 39)
	st    Y+,      r8      ; (22 / 41)
	st    Y+,      r8      ; (24 / 43)
	st    Y+,      r8      ; (26 / 45)
	st    Y+,      r8      ; (28 / 47)
	st    Y+,      r8      ; (30 / 49)
	st    Y+,      r8      ; (32 / 51)
	st    Y+,      r8      ; (34 / 53)
	cpi   YL,      16      ; (35 / 54)
	brne  dfrl02           ; (37 / 55)
	mov   YL,      r0      ; (56 = 0) Restore YL (Fg. palette index)
#endif
dfloop:
	subi  r22,     8       ; ( 1)
	brcs  dfend            ; ( 2 /  3)
	lpm   r24,     Z       ; ( 5) Dummy load (nop)
dfrl0:
#if (M74_SD_ENABLE != 0)
	M74WT_R24      12      ; (17)
	movw  ZL,      r14     ; (18) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (53) 35 cycles
	movw  r14,     ZL      ; (54) r15:r14, ZH:ZL Target pointer
#else
	M74WT_R24      49      ; (54)
#endif
	rjmp  dfloop           ; (56)
dfend:



;
; Increment line counters
;
	inc   r16              ; (   1) Increment the physical line counter
	inc   r17              ; (   2) Increment the logical line counter



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
; r25: X scroll: 0 - 7, shifting to the left
;
; Other regs are (also) set up according to the requirements of the scanline
; loop.
;
; Timing notes:
;
; Things were shifted around several times to center the display and to meet
; various other requirements. Now it takes 2 extra cycles to the original
; calculation (so 56 instead of 54).
;
	ld    r13,     Y       ; ( 0) Foreground (1) color (YL already set up)
	lds   YL,      m74_bgcol    ; ( 2)
	cpi   r25,     0       ; ( 3)
	brne  pscrl            ; ( 4 /  5) If scrolling, then first partial tile load
	;
	; No scroll: Clean up all on the left (so omitting the use of a
	; partial tile
	;
#if (M74_SD_ENABLE != 0)
	movw  ZL,      r14     ; ( 5) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (40) 35 cycles
	movw  r14,     ZL      ; (41) r15:r14, ZH:ZL Target pointer
	nop                    ; (42)
#else
	M74WT_R24      38      ; (42)
#endif
	ld    r24,     Y       ; (44) Finish attribute mode color picking: Bg. color
	ld    ZL,      X+      ; (46) Load first tile index from RAM
	clr   r3               ; (47)
	movw  r4,      r2      ; (48) r5:r4, r3:r2
	movw  r6,      r2      ; (49) r7:r6, r3:r2
	movw  r8,      r2      ; (50) r9:r8, r3:r2
	subi  r18,     8       ; (51)
	mov   r25,     r24     ; (52)
	rjmp  centry           ; (54) Go for the scanline output!
pscrl:
	;
	; Partial tile load code
	;
	; Supports only 8px wide tiles, either 1bpp or 4bpp. The rest can not
	; be scrolled by X (at least not past the left edge).
	;
	ld    r24,     Y       ; ( 7) Finish attribute mode color picking: Bg. color
	ld    ZL,      X+      ; ( 9) Load first tile index from RAM
	lsl   ZL               ; (10)
	brcs  pb80ff           ; (11 / 12)
	mov   ZH,      r11     ; (12)
	cpse  r19,     r23     ; (13 / 14) Mode nonzero: Special modes for 0x00 - 0x7F
	rjmp  psp00            ; (15)
	nop                    ; (15)
	lsl   ZL               ; (16)
pcom:
	; 4bpp ROM tile output
	adc   ZH,      r23     ; (17) Use bit 6 of tile index for 0xC0 - 0xFF tiles
	lpm   YL,      Z+      ; (20)
	swap  YL               ; (21)
	ld    r3,      Y       ; (23)
	lpm   YL,      Z+      ; (26)
	ld    r4,      Y       ; (28)
	swap  YL               ; (29)
	ld    r5,      Y       ; (31)
	lpm   YL,      Z+      ; (34)
	ld    r6,      Y       ; (36)
	swap  YL               ; (37)
	ld    r7,      Y       ; (39)
	lpm   YL,      Z+      ; (42)
	ld    r8,      Y       ; (44)
	swap  YL               ; (45)
	ld    r9,      Y       ; (47)
p1bcom:
	ld    ZL,      X+      ; (49) Load next tile index from RAM
	rjmp  ptend            ; (51)
pb80ff:
	mov   ZH,      r12     ; (13)
	lsl   ZL               ; (14)
	brcc  pcom             ; (15 / 16)
	mov   ZH,      r21     ; (16)
	rjmp  pramt            ; (18)
pramt:
	; 4bpp RAM tile output
	add   ZL,      r20     ; (19)
	adc   ZH,      r23     ; (20) (Just carry, r23 is zero)
	ld    YL,      Z+      ; (22)
	swap  YL               ; (23)
	ld    r3,      Y       ; (25)
	ld    YL,      Z+      ; (27)
	ld    r4,      Y       ; (29)
	swap  YL               ; (30)
	ld    r5,      Y       ; (32)
	ld    YL,      Z+      ; (34)
	ld    r6,      Y       ; (36)
	swap  YL               ; (37)
	ld    r7,      Y       ; (39)
	ld    YL,      Z+      ; (41)
	ld    r8,      Y       ; (43)
	swap  YL               ; (44)
	ld    r9,      Y       ; (46)
	nop                    ; (47)
	ld    ZL,      X+      ; (49) Load next tile index from RAM
	rjmp  ptend            ; (51)
psp00:
	; 1bpp 8 pixels wide ROM / RAM tile output
	; (just branch here for everything special, don't care)
	lsr   ZL               ; (16) Back to 0x00 - 0x7F range
	add   ZL,      r10     ; (17)
	adc   ZH,      r23     ; (18) (Just carry, r23 is zero)
	cpi   r19,     0x40    ; (19)
	brpl  .+4              ; (20 / 21)
	ld    r22,     Z+      ; (22) RAM tile
	rjmp  .+2              ; (24)
	lpm   r22,     Z+      ; (24) ROM tile
	mov   r3,      r24     ; (25)
	mov   r4,      r24     ; (26)
	mov   r5,      r24     ; (27)
	movw  r6,      r4      ; (28) r7:r6, r5:r4
	movw  r8,      r4      ; (29) r9:r8, r5:r4
	sbrc  r22,     6       ; (30)
	mov   r3,      r13     ; (31)
	sbrc  r22,     5       ; (32)
	mov   r4,      r13     ; (33)
	sbrc  r22,     4       ; (34)
	mov   r5,      r13     ; (35)
	sbrc  r22,     3       ; (36)
	mov   r6,      r13     ; (37)
	sbrc  r22,     2       ; (38)
	mov   r7,      r13     ; (39)
	sbrc  r22,     1       ; (40)
	mov   r8,      r13     ; (41)
	sbrc  r22,     0       ; (42)
	mov   r9,      r13     ; (43)
	rjmp  .                ; (45)
	rjmp  p1bcom           ; (47)
ptend:
	;
	; Left side cleanup code. If r18 is 1, then 7 pixels are visible from
	; the partial tile, if it is 7, then 1 pixel. Then delay the output as
	; necessary and go on to render.
	;
	sub   r18,     r25     ; (52)
	cpi   r25,     7       ; (53)
	brne  pte123456        ; (54 / 55)
	clr   r3               ; (55)
	movw  r4,      r2      ; (56) r5:r4, r3:r2
	movw  r6,      r2      ; (57) r7:r6, r3:r2
	clr   r8               ; (58) Cleared all but r9.
ptej1:
	mov   r25,     r24     ; (59)
	rjmp  centry           ; (61) Go for the scanline output! (At no scroll + 7)
pte123456:
	cpi   r25,     6       ; (56)
	brne  pte12345         ; (57 / 58)
	clr   r3               ; (58)
	movw  r4,      r2      ; (59) r5:r4, r3:r2
	movw  r6,      r2      ; (60) r7:r6, r3:r2
ptej2:
	lpm   r22,     Z       ; (63) Dummy load (nop)
	rjmp  ptej1            ; (65)
pte12345:
	cpi   r25,     5       ; (59)
	brne  pte1234          ; (60 / 61)
	clr   r3               ; (61)
	movw  r4,      r2      ; (62) r5:r4, r3:r2
	clr   r6               ; (63)
ptej3:
	rjmp  .                ; (65)
	rjmp  ptej2            ; (67)
pte1234:
	cpi   r25,     4       ; (62)
	brne  pte123           ; (63 / 64)
	clr   r3               ; (64)
	movw  r4,      r2      ; (65) r5:r4, r3:r2
ptej4:
	lpm   r22,     Z       ; (68) Dummy load (nop)
	rjmp  ptej3            ; (70)
pte123:
	cpi   r25,     3       ; (65)
	brne  pte12            ; (66 / 67)
	clr   r3               ; (67)
	clr   r4               ; (68)
ptej5:
	rjmp  .                ; (70)
	rjmp  ptej4            ; (72)
pte12:
	cpi   r25,     2       ; (68)
	brne  pte1             ; (69 / 70)
	clr   r3               ; (70)
ptej6:
	lpm   r22,     Z       ; (73) Dummy load (nop)
	rjmp  ptej5            ; (75)
pte1:
	lpm   r22,     Z       ; (73) Dummy load (nop)
	rjmp  .                ; (75)
	rjmp  ptej6            ; (77)