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



.section .bss

	; Locals

	v_hsize:       .byte 1 ; Horizontal size on bit 3 and 4

.section .text



;
; Core tile output loop.
;
; r22: Zero
; r23, r20, r21, r0, r1: Temp
; r2, r3, r4, r5, r6, r7, r8, r9: Preloaded pixels for the tile
; r10: ROM 4bpp tiles 0x00 - 0x3F / specials, offset high
; r11: ROM 4bpp tiles 0x80 - 0xFF, offset high (Row incr: 512; 4K)
; r12: RAM 4bpp tiles / specials, offset low
; r13: Pixel 1 color in 1bpp / attribute mode or
;      Tiles in a 2bb region
;      RAM 4bpp tiles, offset high
; r24: Pixel 0 color in 1bpp / attribute mode or Temp
; r25: Pixel 0 color in 1bpp / attribute mode or Temp
; r18: Remaining pixels - 8. After the line, also for the last partial tile.
; r19: Mode for tiles 0x00 - 0x7F:
;      0b00000000: ROM + RAM 4bpp tiles
;      0b10000000: 2 bit RAM region
;      0b0xxxxxxx: 8px wide 1bpp tile
;      0b1xxxxxxx: 6px wide 1bpp tile (ROM source only)
;      0bx0xxxxxx: RAM 1bpp source / attr disabled in 6px wide mode
;      0bx1xxxxxx: ROM 1bpp source / attr enabled in 6px wide mode
; X: Tile index ROM / RAM address
; Y: Palette (only YH loaded, YL is from color)
; r14, r15, r16, r17: Unused
;
; r13: 2bpp region is at least 2 tiles wide.
; r18: At end, 0xF8 - 0xFF may be in this reg (if used correctly). This
;      indicates the pixels to render at end of line in the low 3 bits (0xF8:
;      8px, 0xFF: 1px).
;
; 6px wide attribute mode output:
; Color can be set for pixel 1. After the 4 tile indices, 2 attribute bytes
; follow, high nybble of first specifying fg. color for the first tile.
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
cb1x:
	mov   ZH,      r11     ; ( 4)
	lsl   ZL               ; ( 5)
	rjmp  ccom             ; ( 7)
clram:
	out   PIXOUT,  r9      ; (50) Pixel 7
	ld    r8,      Y       ; (52)
	swap  YL               ; (53)
	ld    r9,      Y       ; (55)
centry:
	; Pixel output loop entry point
	lsl   ZL               ; (56 = 0)
	out   PIXOUT,  r2      ; ( 1) Pixel 0
	brcs  cb1x             ; ( 2)
	mov   ZH,      r10     ; ( 3)
	cpse  r19,     r22     ; ( 4) Mode nonzero: Special modes for 0x00 - 0x7F
	rjmp  csp00            ; ( 6)
	brmi  cramt            ; ( 6) 0x40 - 0x7F are 4 bit RAM tiles
	lsl   ZL               ; ( 7)
ccom:
	; 4bpp ROM tile output
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	adc   ZH,      r22     ; ( 9) Use bit 6 of tile index for 0xC0 - 0xFF tiles
	lpm   YL,      Z+      ; (12)
	ld    r2,      Y       ; (14)
	out   PIXOUT,  r4      ; (15) Pixel 2
	swap  YL               ; (16)
	ld    r3,      Y       ; (18)
	lpm   YL,      Z+      ; (21)
	out   PIXOUT,  r5      ; (22) Pixel 3
	subi  r18,     8       ; (23) The resulting carry is used down in a brcc
	lpm   r23,     Z+      ; (26)
	ld    r4,      Y       ; (28)
	out   PIXOUT,  r6      ; (29) Pixel 4
	swap  YL               ; (30)
	ld    r5,      Y       ; (32)
	mov   YL,      r23     ; (33)
	ld    r6,      Y       ; (35)
	out   PIXOUT,  r7      ; (36) Pixel 5
	swap  YL               ; (37)
	ld    r7,      Y       ; (39)
	lpm   YL,      Z+      ; (42)
	out   PIXOUT,  r8      ; (43) Pixel 6
	rjmp  .                ; (45)
cvram:
	ld    ZL,      X+      ; (47) Load next tile index from RAM
	brcc  clram            ; (48 / 49)
cvrec:
	nop                    ; (49)
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
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	lsl   ZL               ; ( 9)
	add   ZL,      r12     ; (10)
	mov   ZH,      r13     ; (11)
	adc   ZH,      r22     ; (12) (Just carry, r22 is zero)
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
	add   ZL,      r12     ; (12)
	adc   ZH,      r22     ; (13) (Just carry, r22 is zero)
	ldi   r23,     16      ; (14) Base width is 16 pixels (2 tiles)
	out   PIXOUT,  r4      ; (15) Pixel 2
	mov   r24,     r13     ; (16) Load tile count to output
	dec   r24              ; (17) At least 2 tiles
c2bpple:
	ld    r20,     Z+      ; () Input tile, left (r2,r3,r4,r5)
	mov   YL,      r20     ; ()
	lsr   YL               ; ()
	out   PIXOUT,  r5      ; (22) Pixel 3
	lsr   YL               ; ()
	andi  r20,     0x33    ; ()
	andi  YL,      0x33    ; ()
	ld    r2,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	ld    r4,      Y       ; ()
	mov   YL,      r20     ; ()
	ld    r3,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	ld    r5,      Y       ; ()
	ld    r20,     Z+      ; () Input tile, right (r6,r7,r8,r9)
	mov   YL,      r20     ; ()
	lsr   YL               ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	lsr   YL               ; ()
	andi  r20,     0x33    ; ()
	andi  YL,      0x33    ; ()
	ld    r6,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	ld    r8,      Y       ; ()
	mov   YL,      r20     ; ()
	ld    r7,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r2      ; ( 1) Pixel 0
	ld    r9,      Y       ; ()
	dec   r24              ; ()
	brne  c2bppl           ; ( 5 / 6)
	ld    r20,     Z+      ; () Input tile, left (r2,r3,r4,r5)
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	mov   YL,      r20     ; ()
	lsr   YL               ; ()
	lsr   YL               ; ()
	andi  r20,     0x33    ; ()
	ld    r21,     Z+      ; () Input tile, right (r6,r7,r8,r9)
	out   PIXOUT,  r4      ; (15) Pixel 2
	andi  YL,      0x33    ; ()
	ld    r2,      Y       ; ()
	swap  YL               ; ()
	ld    r4,      Y       ; ()
	out   PIXOUT,  r5      ; (22) Pixel 3
	mov   YL,      r20     ; ()
	ld    r3,      Y       ; ()
	swap  YL               ; ()
	ld    r5,      Y       ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	nop                    ; ()
	mov   YL,      r21     ; ()
	lsr   YL               ; ()
	lsr   YL               ; ()
	andi  r21,     0x33    ; ()
	andi  YL,      0x33    ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	sub   r18,     r23     ; () Bill the number of pixels output
	ld    ZL,      X+      ; ()
	ld    r6,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	ld    r8,      Y       ; ()
	mov   YL,      r21     ; ()
	ld    r7,      Y       ; ()
	swap  YL               ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	ld    r9,      Y       ; (52)
	brcs  c2bppe           ; (53 / 54)
	rjmp  centry           ; (55)
c2bppe:
	rjmp  cend0            ; (56)
c2bppl:
	subi  r23,     0xF8    ; () +8 pixels output
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	lpm   r25,     Z       ; () Dummy load (nop)
	lpm   r25,     Z       ; () Dummy load (nop)
	out   PIXOUT,  r4      ; (15) Pixel 2
	rjmp  c2bpple          ; ()
c1bp8:
	; 1bpp 8 pixels wide ROM / RAM tile output
	lsr   ZL               ; (12) Back to 0x00 - 0x7F range
	add   ZL,      r12     ; (13)
	adc   ZH,      r22     ; (14) (Just carry, r22 is zero)
	out   PIXOUT,  r4      ; (15) Pixel 2
	cpi   r19,     0x40    ; (16)
	brpl  c1bp8r           ; (17 / 18)
	ld    r23,     Z+      ; (19) RAM tile
	rjmp  c1bp8c           ; (21)
c1bp8r:
	lpm   r23,     Z+      ; (21) ROM tile
c1bp8c:
	out   PIXOUT,  r5      ; (22) Pixel 3
	movw  r2,      r24     ; () r3:r2, r25:r24
	movw  r4,      r24     ; () r5:r4, r25:r24
	movw  r20,     r24     ; () r21:r20, r25:r24
	movw  r0,      r24     ; () r1:r0, r25:r24
	sbrc  r23,     7       ; ()
	mov   r2,      r13     ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	sbrc  r23,     6       ; ()
	mov   r3,      r13     ; ()
	sbrc  r23,     5       ; ()
	mov   r4,      r13     ; ()
	sbrc  r23,     4       ; ()
	mov   r5,      r13     ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	sbrc  r23,     3       ; ()
	mov   r20,     r13     ; ()
	sbrc  r23,     2       ; ()
	mov   r21,     r13     ; ()
	sbrc  r23,     1       ; ()
	mov   r0,      r13     ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	sbrc  r23,     0       ; ()
	mov   r1,      r13     ; ()
	nop                    ; ()
	subi  r18,     8       ; () Bill the number of pixels output (carry used below)
	ld    ZL,      X+      ; (49)
	out   PIXOUT,  r9      ; (50) Pixel 7
	movw  r6,      r20     ; () r7:r6, r21:r20
	movw  r8,      r0      ; () r9:r8, r1:r0
	brcs  c2bppe           ; (53 / 54)
	rjmp  centry           ; (55)
c1bp6a:
	out   PIXOUT,  r9      ; (50) Pixel 7
	mov   ZH,      r10     ; (51)
	nop                    ; (52)
	rjmp  c1bp6b           ; (54)
c1bp6c:
	out   PIXOUT,  r5      ; (22) Pixel 3
	lpm   r23,     Z       ; (25) Dummy read (nop)
	lpm   r23,     Z       ; (28) Dummy read (nop)
	out   PIXOUT,  r6      ; (29) Pixel 4
	rjmp  .                ; (31)
	rjmp  c1bp6d           ; (33)
c1bp6:
	; 4x 1bpp 6 pixels wide ROM tiles
	out   PIXOUT,  r4      ; (15) Pixel 2
	lsr   ZL               ; (16) Back to 0x00 - 0x7F range
	add   ZL,      r12     ; (17) Complete offset low
	adc   ZH,      r22     ; (18) (Just carry, r22 is zero)
	lpm   r20,     Z+      ; (21) Tile 0 pixel data
	out   PIXOUT,  r5      ; (22) Pixel 3
	mov   ZH,      r10     ; (23)
	ld    ZL,      X+      ; (25)
	add   ZL,      r12     ; (26) Complete offset low (Tile 1)
	adc   ZH,      r22     ; (27) (Just carry, r22 is zero)
	movw  r2,      r24     ; (28) r3:r2, r25:r24
	out   PIXOUT,  r6      ; (29) Pixel 4
	movw  r4,      r24     ; (30) r5:r4, r25:r24
	lpm   r21,     Z+      ; (33) Tile 1 pixel data
	ld    ZL,      X+      ; (35)
	out   PIXOUT,  r7      ; (36) Pixel 5
	add   ZL,      r12     ; (37) Complete offset low (Tile 2)
	mov   ZH,      r10     ; (38)
	adc   ZH,      r22     ; (39) (Just carry, r22 is zero)
	lpm   r0,      Z+      ; (42) Tile 2 pixel data
	out   PIXOUT,  r8      ; (43) Pixel 6
	movw  r6,      r24     ; (44) r7:r6, r25:r24
	ld    ZL,      X+      ; (46)
	cpi   r19,     0xC0    ; (47)
	brmi  c1bp6a           ; (48 / 49)
	mov   ZH,      r10     ; (49)
	out   PIXOUT,  r9      ; (50) Pixel 7
	ld    YL,      X+      ; (52)
	ld    r13,     Y       ; (54) First tile px 1 color in r13
c1bp6b:
	sbrc  r20,     7       ; (55)
	mov   r2,      r13     ; (56)
	out   PIXOUT,  r2      ; ( 1) Pixel 0 (Output tile 0 & 1)
	sbrc  r20,     6       ; ( 2)
	mov   r3,      r13     ; ( 3)
	sbrc  r20,     5       ; ( 4)
	mov   r4,      r13     ; ( 5)
	sbrc  r20,     4       ; ( 6)
	mov   r5,      r13     ; ( 7)
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	sbrc  r20,     3       ; ( 9)
	mov   r6,      r13     ; (10)
	sbrc  r20,     2       ; (11)
	mov   r7,      r13     ; (12)
	add   ZL,      r12     ; (13) Complete offset low (Tile 3)
	adc   ZH,      r22     ; (14) (Just carry, r22 is zero)
	out   PIXOUT,  r4      ; (15) Pixel 2
	lpm   r1,      Z+      ; (18) Tile 3 pixel data
	cpi   r19,     0xC0    ; (19)
	brmi  c1bp6c           ; (20 / 21)
	swap  YL               ; (21)
	out   PIXOUT,  r5      ; (22) Pixel 3
	ld    r13,     Y       ; (24) Second tile px 1 color in r13
	ld    YL,      X+      ; (26)
	ld    ZH,      Y       ; (28) Third tile px 1 color in ZH
	out   PIXOUT,  r6      ; (29) Pixel 4
	swap  YL               ; (30)
	ld    YL,      Y       ; (32) Fourth tile px 1 color in YL
	nop                    ; (33)
c1bp6d:
	ld    ZL,      X+      ; (35)
	out   PIXOUT,  r7      ; (36) Pixel 5
	rjmp  .                ; ()
	movw  r8,      r24     ; () r9:r8, r25:r24
	movw  r2,      r24     ; () r3:r2, r25:r24
	sbrc  r21,     7       ; ()
	mov   r8,      r13     ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	sbrc  r21,     6       ; ()
	mov   r9,      r13     ; ()
	sbrc  r21,     5       ; ()
	mov   r2,      r13     ; ()
	sbrc  r21,     4       ; ()
	mov   r3,      r13     ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	lpm   r23,     Z       ; () Dummy read (nop)
	lpm   r23,     Z       ; () Dummy read (nop)
	out   PIXOUT,  r2      ; ( 1) Pixel 0 (Output tile 1 & 2)
	movw  r4,      r24     ; () r5:r4, r25:r24
	movw  r6,      r24     ; () r7:r6, r25:r24
	sbrc  r21,     3       ; ()
	mov   r4,      r13     ; ()
	sbrc  r21,     2       ; ()
	mov   r5,      r13     ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	sbrc  r19,     6       ; ()
	mov   r13,     ZH      ; () Load attribute for Tile 2
	sbrc  r0,      7       ; ()
	mov   r6,      r13     ; ()
	sbrc  r0,      6       ; ()
	mov   r7,      r13     ; ()
	out   PIXOUT,  r4      ; (15) Pixel 2
	movw  r8,      r24     ; () r9:r8, r25:r24
	movw  r2,      r24     ; () r3:r2, r25:r24
	sbrc  r0,      5       ; ()
	mov   r8,      r13     ; ()
	sbrc  r0,      4       ; ()
	mov   r9,      r13     ; ()
	out   PIXOUT,  r5      ; (22) Pixel 3
	sbrc  r0,      3       ; ()
	mov   r2,      r13     ; ()
	sbrc  r0,      2       ; ()
	mov   r3,      r13     ; ()
	sbrc  r19,     6       ; ()
	mov   r13,     YL      ; () Load attribute for Tile 3
	out   PIXOUT,  r6      ; (29) Pixel 4
	movw  r4,      r24     ; () r5:r4, r25:r24
	nop                    ; ()
	sbrc  r1,      7       ; ()
	mov   r4,      r13     ; ()
	sbrc  r1,      6       ; ()
	mov   r5,      r13     ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	movw  r6,      r24     ; () r7:r6, r25:r24
	movw  r20,     r24     ; () r21:r20, r25:r24
	sbrc  r1,      5       ; ()
	mov   r6,      r13     ; ()
	sbrc  r1,      4       ; ()
	mov   r7,      r13     ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	sbrc  r1,      3       ; ()
	mov   r20,     r13     ; ()
	sbrc  r1,      2       ; ()
	mov   r21,     r13     ; ()
	mov   r8,      r20     ; ()
	subi  r18,     24      ; () Bill the number of pixels output
	out   PIXOUT,  r9      ; (50) Pixel 7
	mov   r9,      r21     ; ()
	nop                    ; ()
	brcs  c1bp6x           ; (53 / 54)
	rjmp  centry           ; (55)
c1bp6x:
	rjmp  cend0            ; (56)
cend1:
	; Trailing pixels part for X scrolling. 1 - 8 pixels are output here
	; (including Pixel 0 before the jump)
	nop                    ; ( 4)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	lpm   r23,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r4      ; (15) Pixel 2
	lpm   r23,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r5      ; (22) Pixel 3
	lpm   r23,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	lpm   r23,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	lpm   r23,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	lpm   r23,     Z       ; () Dummy read (nop)
	inc   r18              ; ()
	breq  cend             ; ()
	nop                    ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	lpm   r23,     Z       ; () Dummy read (nop)
	lpm   r23,     Z       ; () Dummy read (nop)
cend:
	;
	; At this point 25 tiles worth of cycles were consumed, the cycle
	; counter should be at:
	;  234 (HSync)
	;    3 (Horiz. size reduction)
	;    2 (Line counter increments)
	;    9 (Attributes)
	;   47 (Prolog)
	;    1 (Cycles after entry before Pixel 0)
	; 1400 (25 tiles including the trailing part)
	; ----
	; 1696
	;
	out   PIXOUT,  r22     ; (1697) Right border (blanking) starts



;
; Horizontal size reduction, trailing part.
;
	lds   r20,     v_hsize ; (1699)
drloop:
	cpi   r20,     0       ; ( 1)
	breq  drend            ; ( 2 /  3) (1702)
	subi  r20,     8       ; ( 3)
	lds   r8,      m74_ldsl     ; ( 5) Load start scanline for RAM clear
	lds   r6,      v_remc_lo    ; ( 7) Remaining bytes / blocks
	lds   r7,      v_remc_hi    ; ( 9)
	cp    r8,      r16     ; (10) Compare start with current line
	brcc  drnfunc          ; (11 / 12) Function may run only if reached
	movw  ZL,      r14     ; (12) ZH:ZL, r15:r14 Target pointer
	sbrc  r7,      7       ; (13 / 14)
	rjmp  drramc           ; (15) Bit 7 of high set: RAM clear function
	rjmp  drspil           ; (16) Bit 7 of high clear: SPI stream function
drnfunc:
	WAIT  r24,     42      ; (54)
	rjmp  drloop           ; (56 = 0)
drend:



;
; Loop test condition: If the scanline counter reached the count of lines to
; render, return. It is possible to request zero lines.
;
; Entry is at cycle 1702. (the call must be issued at 1698)
; Exit is at 1710.
;
m74_scloop:
	lds   r23,     render_lines_count ; (1704)
	cp    r23,     r16     ; (1705)
	brne  sclpco           ; (1706 / 1707)
	ret                    ; (1710)
sclpco:
	clr   r22              ; (1708) r22 is used as a permanent zero register.



;
; Row management code
;
; r22: Zero
; r16: Scanline counter (Normally 0 => 223)
; r17: Logical row counter
;
; Cycles:
;  33 (Row select)
;  43 (Tile descriptor load)
;  36 (Precalc for row)
; ---
; 112
;
	lds   r23,     m74_config   ; ( 2)
	lds   ZL,      v_rows_lo    ; ( 4)
	lds   ZH,      v_rows_hi    ; ( 6)
	sbrs  r23,     0       ; ( 7 /  8)
	rjmp  mresp            ; ( 9)
	; RAM / ROM scanline map
	add   ZL,      r16     ; ( 9)
	adc   ZH,      r22     ; (10)
	sbrs  r23,     7       ; (11 / 12)
	rjmp  mresro           ; (13)
	ld    r17,     Z       ; (14) RAM scanline map
	rjmp  mresre           ; (16)
mresro:
	lpm   r17,     Z       ; (16) ROM scanline map
mresre:
	rjmp  mrespb           ; (18)
mrespl:
	lds   r2,      v_shifto     ; (16) No new line: just load shift
	rjmp  .                ; (18)
mrespb:
	lpm   r3,      Z       ; (21) Dummy load (nop)
	rjmp  mrese            ; (23)
mresns:
	lpm   r3,      Z       ; (29) Dummy load (nop)
	rjmp  .                ; (31)
	rjmp  mresse           ; (33)
mresp:
	; RAM scanline + restart pairs
	ld    r2,      Z+      ; (11)
	cp    r2,      r16     ; (12)
	brne  mrespl           ; (13 / 14)
	ld    r17,     Z+      ; (15) Load new logical row counter
	ld    r2,      Z+      ; (17) Load new shift override
	sts   v_shifto,  r2    ; (19)
	sts   v_rows_lo, ZL    ; (21)
	sts   v_rows_hi, ZH    ; (23)
mrese:
	; Test for X shift map, and load if necessary
	sbrs  r23,     6       ; (24 / 25)
	rjmp  mresns           ; (26) No X shift map
	lds   ZL,      m74_xsh_lo   ; (27)
	lds   ZH,      m74_xsh_hi   ; (29)
	add   ZL,      r17     ; (30) Offset by logical row counter
	adc   ZH,      r22     ; (31)
	ld    r2,      Z       ; (33)
mresse:
	;
	; Load tile descriptors
	;
	lds   ZL,      m74_tdesc_lo ; ( 2)
	lds   ZH,      m74_tdesc_hi ; ( 4)
	mov   r24,     r17     ; ( 5)
	andi  r24,     0xF8    ; ( 6) Tile descriptor offset from log. row counter
	add   ZL,      r24     ; ( 7)
	adc   ZH,      r22     ; ( 8)
	sbrs  r23,     1       ; ( 9 / 10)
	rjmp  mtdro            ; (11)
	; RAM tile descriptors. Mostly everything goes in the regs where the
	; scanline renderer will expect them, r4 receives the offset low
	; increment.
	ld    r19,     Z+      ; (12)
	ld    r11,     Z+      ; (14)
	ld    r12,     Z+      ; (16)
	ld    r10,     Z+      ; (18)
	ld    r4,      Z+      ; (20)
	ld    r13,     Z+      ; (22)
	lpm   r5,      Z       ; (25) Dummy load (nop)
	lpm   r5,      Z       ; (28) Dummy load (nop)
	sbrc  r23,     2       ; (29 / 30)
	rjmp  mtdovr           ; (31)
	lpm   r5,      Z       ; (33) Dummy load (nop)
	rjmp  .                ; (35)
	rjmp  .                ; (37)
	rjmp  mtdov0           ; (39)
mtdron:
	lpm   XL,      Z+      ; (35)
	lpm   XH,      Z+      ; (38)
	lpm   r5,      Z       ; (41) Dummy load (nop)
	rjmp  mtde             ; (43)
mnshf:
	; No shift override present (belongs to preparing for scanline render)
	mov   r18,     r19     ; () Shift comes from tile descriptor
	rjmp  .                ; ()
	rjmp  mnshfe           ; ()
mtdro:
	; ROM tile descriptors Mostly everything goes in the regs where the
	; scanline renderer will expect them, r4 receives the offset low
	; increment.
	lpm   r19,     Z+      ; (14)
	lpm   r11,     Z+      ; (17)
	lpm   r12,     Z+      ; (20)
	lpm   r10,     Z+      ; (23)
	lpm   r4,      Z+      ; (26)
	lpm   r13,     Z+      ; (29)
	sbrs  r23,     2       ; (30 / 31)
	rjmp  mtdron           ; (32)
mtdovr:
	; Tile index overrides
	lds   ZL,      m74_tidx_lo  ; (33)
	lds   ZH,      m74_tidx_hi  ; (35)
	lsr   r24              ; (36)
	lsr   r24              ; (37)
	add   ZL,      r24     ; (38)
	adc   ZH,      r22     ; (39)
mtdov0:
	ld    XL,      Z+      ; (41)
	ld    XH,      Z+      ; (43)
mtde:
	;
	; Prepare for scanline render
	;
	; Process tile row configuration
	mov   r20,     r19     ; ( 1) r20: Save for horizontal size
	andi  r19,     0xE0    ; ( 2) Mode selector remains in r19
	cpi   r19,     0xE0    ; ( 3)
	brne  mtnmod           ; ( 4 /  5)
	rjmp  m74_sl_separator ; ( 6) Jump off to separator line
mtnmod:
	andi  r20,     0x18    ; ( 6) Mask off size bits
	sts   v_hsize, r20     ; ( 8) Save them
	; Data loads done. Now calculate stuff for the actual scanline render.
	; Calculate left shift into r18 and apply shift override.
	sbrc  r23,     0       ; ( 9 / 10) If clear, shift override is present
	rjmp  mnshf            ; (11)
	mov   r18,     r2      ; (11) Shift override
	lsr   r2               ; (12)
	lsr   r2               ; (13)
	lsr   r2               ; (14)
	add   XL,      r2      ; (15) Apply high 5 bits as offset
	adc   XH,      r22     ; (16)
mnshfe:
	andi  r18,     0x7     ; (17)
	; Apply row counter low bits where it is due.
	mov   r24,     r17     ; (18)
	andi  r24,     0x7     ; (19) Physical row counter low
	mov   r5,      r24     ; (20)
	lsl   r5               ; (21) 512 increment for ROM 4bpp tiles
	add   r11,     r5      ; (22)
	mov   r25,     r19     ; (23)
	swap  r25              ; (24) Mode on bits 1 - 3, bit 7 clear.
	dec   r25              ; (25) If mode was zero, bit 7 is set.
	sbrc  r25,     7       ; (26)
	add   r10,     r5      ; (27) Mode 0: 512 increment for ROM 4bpp tiles
	mul   r4,      r24     ; (29)
	brne  mrca1            ; (30 / 31)
	mov   r1,      r24     ; (31) If r4 was zero, multiply with 256
mrca1:
	add   r12,     r0      ; (32)
	sbrs  r25,     7       ; (33)
	adc   r10,     r1      ; (34) Modes 1 - 7 offset high
	sbrc  r25,     7       ; (35)
	adc   r13,     r1      ; (36) Mode 0 RAM tiles offset high
	; In r25, the swapped mode - 1 is still present, to be used with
	; attribute mode color picking.



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
; 220 (Allowing 4CH audio + UART or 5CH audio)
;
; From the prolog some calculations are moved here so to make it possible
; to shift the display a bit around for proper centering.
;
	inc   r25              ; (   1)
	andi  r25,     0x6     ; (   2) Mode low 2 bits on 1, 2
	dec   r25              ; (   3) bit7 set indicates Mode 0 or Mode 4.
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	clr   r2               ; (   6) First output pixel is always zero (scroll)
	ldi   r21,     192     ; (   7) No. of pixels to output
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	WAIT  ZL,      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES
	ld    ZL,      X+      ; ( 234) Load first tile index from RAM



;
; Horizontal size reduction as requested, in r20. 0, 1, 2 or 3 tiles worth of
; cycles are spent here without graphics output, and accordingly r21 is set
; up to reflect the number of pixels to generate (initialized above the hsync)
; r23 still contains the global config at this point
;
dfloop:
	cpi   r20,     0       ; ( 1)
	breq  dfend            ; ( 2 /  3)
	subi  r20,     8       ; ( 3)
	subi  r21,     16      ; ( 4)
	; Perform Color 0 reload under leftmost omitted tile if it was
	; enabled.
	mov   r9,      ZL      ; ( 5) Put aside already loaded tile index
	sbrc  r23,     4       ; ( 6 /  7)
	rjmp  dfrl0            ; ( 8) Reload enabled
	WAIT  r24,     45      ; (52)
	rjmp  dfrl02           ; (54)
dfrl0:
	lds   ZL,      m74_col0_lo  ; (10)
	lds   ZH,      m74_col0_hi  ; (12)
	add   ZL,      r17     ; (13)
	adc   ZH,      r22     ; (14) (Just carry, r22 is zero)
	sbrs  r23,     5       ; (15 / 16)
	rjmp  dfrl00           ; (17)
	ld    r8,      Z       ; (18) Color0 table in ROM
	rjmp  dfrl01           ; (20)
dfrl00:
	lpm   r8,      Z       ; (20)
dfrl01:
	clr   YL               ; (21)
	st    Y+,      r8      ; (23)
	st    Y+,      r8      ; (25)
	st    Y+,      r8      ; (27)
	st    Y+,      r8      ; (29)
	st    Y+,      r8      ; (31)
	st    Y+,      r8      ; (33)
	st    Y+,      r8      ; (35)
	st    Y+,      r8      ; (37)
	st    Y+,      r8      ; (39)
	st    Y+,      r8      ; (41)
	st    Y+,      r8      ; (43)
	st    Y+,      r8      ; (45)
	st    Y+,      r8      ; (47)
	st    Y+,      r8      ; (49)
	st    Y+,      r8      ; (51)
	st    Y,       r8      ; (53)
	mov   ZL,      r9      ; (54) Restore Z (First tile index)
dfrl02:
	lds   r8,      m74_ldsl     ; (56 = 0) Load start scanline for RAM clear
dfloop0:
	cpi   r20,     0       ; ( 1)
	breq  dfend            ; ( 2 /  3)
	subi  r20,     8       ; ( 3)
	subi  r21,     16      ; ( 4)
	lds   r6,      v_remc_lo    ; ( 6) Remaining bytes / blocks
	lds   r7,      v_remc_hi    ; ( 8)
	cp    r8,      r16     ; ( 9) Compare start with current line
	brcc  dfnfunc          ; (10 / 11) Function may run only if reached
	movw  ZL,      r14     ; (11) ZH:ZL, r15:r14 Target pointer
	sbrc  r7,      7       ; (12 / 13)
	rjmp  dframc           ; (14) Bit 7 of high set: RAM clear function
	rjmp  dfspil           ; (15) Bit 7 of high clear: SPI stream function
dfnfunc:
	WAIT  r24,     43      ; (54)
	rjmp  dfloop0          ; (56 = 0)
dfend:



;
; Increment line counters
;
	inc   r16              ; (   1) Increment the physical line counter
	inc   r17              ; (   2) Increment the logical line counter



;
; Attribute mode color picking. Note that if a color 0 swapping is added, then
; this must come after that so the proper color is picked from palette index
; zero.
;
; r25: If bit 7 is set, then Mode 0 or Mode 4, no attributes (preserve r13)
;
	mov   YL,      r13     ; ( 1)
	ld    r23,     Y       ; ( 3) Foreground (1) color
	swap  YL               ; ( 4)
	ld    r24,     Y       ; ( 6) Background (0) color
	sbrs  r25,     7       ; ( 7) Mode 0 and 4: No attributes, keep r13 intact
	mov   r13,     r23     ; ( 8)
	mov   r25,     r24     ; ( 9)



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
; r18: X scroll: 0 - 7, shifting to the left
;
; Other regs are (also) set up according to the requirements of the scanline
; loop.
;
; Timing notes:
;
; The first six cycles of this was removed to shift the mode a bit to the
; left. So now it totals 47 cycles instead of 54.
;
	cp    r18,     r22     ; ( 8)
	breq  pnscrl0          ; ( 9 /10) No scrolling: No first partial tile load
	;
	; Partial tile load code
	;
	; Supports only 8px wide tiles, either 1bpp or 4bpp. The rest can not
	; be scrolled by X (at least not past the left edge).
	;
	lsl   ZL               ; (10)
	brcs  pb1x             ; (11 / 12)
	mov   ZH,      r10     ; (12)
	cpse  r19,     r22     ; (13) Mode nonzero: Special modes for 0x00 - 0x7F
	rjmp  psp00            ; (15)
	brmi  pramt            ; (15 / 16) 0x40 - 0x7F are 4 bit RAM tiles
	lsl   ZL               ; (16)
pcom:
	; 4bpp ROM tile output
	adc   ZH,      r22     ; (17) Use bit 6 of tile index for 0xC0 - 0xFF tiles
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
pb1x:
	mov   ZH,      r11     ; (13)
	lsl   ZL               ; (14)
	rjmp  pcom             ; (16)
pnscrl0:
	rjmp  pnscrl           ; (12)
pramt:
	; 4bpp RAM tile output
	lsl   ZL               ; (17)
	add   ZL,      r12     ; (18)
	mov   ZH,      r13     ; (19)
	adc   ZH,      r22     ; (20) (Just carry, r22 is zero)
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
	add   ZL,      r12     ; (17)
	adc   ZH,      r22     ; (18) (Just carry, r22 is zero)
	cpi   r19,     0x40    ; (19)
	brpl  p1bp8r           ; (20 / 21)
	ld    r23,     Z+      ; (22) RAM tile
	rjmp  p1bp8c           ; (24)
p1bp8r:
	lpm   r23,     Z+      ; (24) ROM tile
p1bp8c:
	movw  r2,      r24     ; (25) r3:r2, r25:r24
	movw  r4,      r24     ; (26) r5:r4, r25:r24
	movw  r6,      r24     ; (27) r7:r6, r25:r24
	movw  r8,      r24     ; (28) r9:r8, r25:r24
	sbrc  r23,     6       ; (29)
	mov   r3,      r13     ; (30)
	sbrc  r23,     5       ; (31)
	mov   r4,      r13     ; (32)
	sbrc  r23,     4       ; (33)
	mov   r5,      r13     ; (34)
	sbrc  r23,     3       ; (35)
	mov   r6,      r13     ; (36)
	sbrc  r23,     2       ; (37)
	mov   r7,      r13     ; (38)
	sbrc  r23,     1       ; (39)
	mov   r8,      r13     ; (40)
	sbrc  r23,     0       ; (41)
	mov   r9,      r13     ; (42)
	lpm   r23,     Z       ; (45) Dummy load (nop)
	rjmp  p1bcom           ; (47)
pnscrl:
	;
	; No scroll: Clean up all on the left (so omitting the use of a
	; partial tile
	;
	clr   r3               ; (13)
	movw  r4,      r2      ; (14) r5:r4, r3:r2
	movw  r6,      r2      ; (15) r7:r6, r3:r2
	movw  r8,      r2      ; (16) r9:r8, r3:r2
	WAIT  r18,     34      ; (50)
	subi  r21,     8       ; (51)
	mov   r18,     r21     ; (52)
	rjmp  centry           ; (54) Go for the scanline output!
ptend:
	;
	; Left side cleanup code. If r18 is 1, then 7 pixels are visible from
	; the partial tile, if it is 7, then 1 pixel. Then delay the output as
	; necessary and go on to render.
	;
	sub   r21,     r18     ; (52)
	cpi   r18,     7       ; (53)
	brne  pte123456        ; (54 / 55)
	clr   r3               ; (55)
	movw  r4,      r2      ; (56) r5:r4, r3:r2
	movw  r6,      r2      ; (57) r7:r6, r3:r2
	clr   r8               ; (58) Cleared all but r9.
ptej1:
	mov   r18,     r21     ; (59)
	rjmp  centry           ; (61) Go for the scanline output! (At no scroll + 7)
pte123456:
	cpi   r18,     6       ; (56)
	brne  pte12345         ; (57 / 58)
	clr   r3               ; (58)
	movw  r4,      r2      ; (59) r5:r4, r3:r2
	movw  r6,      r2      ; (60) r7:r6, r3:r2
ptej2:
	lpm   r23,     Z       ; (63) Dummy load (nop)
	rjmp  ptej1            ; (65)
pte12345:
	cpi   r18,     5       ; (59)
	brne  pte1234          ; (60 / 61)
	clr   r3               ; (61)
	movw  r4,      r2      ; (62) r5:r4, r3:r2
	clr   r6               ; (63)
ptej3:
	rjmp  .                ; (65)
	rjmp  ptej2            ; (67)
pte1234:
	cpi   r18,     4       ; (62)
	brne  pte123           ; (63 / 64)
	clr   r3               ; (64)
	movw  r4,      r2      ; (65) r5:r4, r3:r2
ptej4:
	lpm   r23,     Z       ; (68) Dummy load (nop)
	rjmp  ptej3            ; (70)
pte123:
	cpi   r18,     3       ; (65)
	brne  pte12            ; (66 / 67)
	clr   r3               ; (67)
	clr   r4               ; (68)
ptej5:
	rjmp  .                ; (70)
	rjmp  ptej4            ; (72)
pte12:
	cpi   r18,     2       ; (68)
	brne  pte1             ; (69 / 70)
	clr   r3               ; (70)
ptej6:
	lpm   r23,     Z       ; (73) Dummy load (nop)
	rjmp  ptej5            ; (75)
pte1:
	lpm   r23,     Z       ; (73) Dummy load (nop)
	rjmp  .                ; (75)
	rjmp  ptej6            ; (77)



;
; Ram clear function before pixel output generation. Enters in cycle 14
;
dframc:
	cp    r6,      r22     ; (15)
	breq  dframc0          ; (16 / 17) No more 16 byte blocks to process
	st    Z+,      r22     ; (18)
	st    Z+,      r22     ; (20)
	st    Z+,      r22     ; (22)
	st    Z+,      r22     ; (24)
	st    Z+,      r22     ; (26)
	st    Z+,      r22     ; (28)
	st    Z+,      r22     ; (30)
	st    Z+,      r22     ; (32)
	st    Z+,      r22     ; (34)
	st    Z+,      r22     ; (36)
	st    Z+,      r22     ; (38)
	st    Z+,      r22     ; (40)
	st    Z+,      r22     ; (42)
	st    Z+,      r22     ; (44)
	st    Z+,      r22     ; (46)
	st    Z+,      r22     ; (48)
	dec   r6               ; (49)
	sts   v_remc_lo, r6    ; (51)
	rjmp  dframc1          ; (53)
dframc0:
	WAIT  r24,     36      ; (53)
dframc1:
	mov   ZL,      r9      ; (54) Restore Z (First tile index)
	rjmp  dfloop0          ; (56)



;
; SPI load function before pixel generation. Enters in cycle 15
;
dfspil:
	rcall m74_spiload_core ; (49) 15 + 3 + 31
	sts   v_remc_lo, r6    ; (51)
	sts   v_remc_hi, r7    ; (53)
	mov   ZL,      r9      ; (54) Restore Z (First tile index)
	rjmp  dfloop0          ; (56)



;
; Ram clear function after pixel output generation. Enters in cycle 15
;
drramc:
	cp    r6,      r22     ; (16)
	breq  drramc0          ; (17 / 18) No more 16 byte blocks to process
	st    Z+,      r22     ; (19)
	st    Z+,      r22     ; (21)
	st    Z+,      r22     ; (23)
	st    Z+,      r22     ; (25)
	st    Z+,      r22     ; (27)
	st    Z+,      r22     ; (29)
	st    Z+,      r22     ; (31)
	st    Z+,      r22     ; (33)
	st    Z+,      r22     ; (35)
	st    Z+,      r22     ; (37)
	st    Z+,      r22     ; (39)
	st    Z+,      r22     ; (41)
	st    Z+,      r22     ; (43)
	st    Z+,      r22     ; (45)
	st    Z+,      r22     ; (47)
	st    Z+,      r22     ; (49)
	dec   r6               ; (50)
	sts   v_remc_lo, r6    ; (52)
	rjmp  dframc1          ; (54)
drramc0:
	WAIT  r24,     36      ; (53)
drramc1:
	rjmp  drloop           ; (56)



;
; SPI load function after pixel generation. Enters in cycle 16
;
drspil:
	rcall m74_spiload_core ; (50) 16 + 3 + 31
	sts   v_remc_lo, r6    ; (52)
	sts   v_remc_hi, r7    ; (54)
	rjmp  drloop           ; (56)
