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
; Core tile output loop.
;
; r0, r1: Temp
; r2, r3, r4, r5, r6, r7, r8, r9: Preloaded pixels for the tile
; r10: ROM 4bpp tiles 0x00 - 0x3F, offset high (Or - 0x7F in others)
; r11: ROM 4bpp tiles 0x40 - 0x7F, offset high (Or pixel 1 color in 1bpp)
; r12: ROM 4bpp tiles 0x80 - 0xBF, offset high, adjusted for muls
; r13: Row selection offset (For 4bpp tiles only)
; r18: Remaining pixels - 8. After the line, also for the last partial tile.
; r19: Mode for tiles 0x00 - 0x7F:
;      0: ROM 4bpp tiles
;      1: Flat tiles
;      4: 8px wide 1bpp ROM tiles
;      5: 8px wide 1bpp RAM tiles
;      6: 6px wide 1bpp ROM tiles
;      7: 6px wide 1bpp ROM tiles with Attributes
; r21: RAM 4bpp tiles 0xC0 - 0xFF, offset high, adjusted for muls
; r23: 1bpp tile index offset
; r24: Pixel 0 color in 1bpp / attribute mode
; r25: 32 (for tile offset multiplication) (1bpp uses it for px. 0 color)
; X: Palette (only XH loaded, XL is from color)
; Y: Palette (only YH loaded, YL is from color)
; Z: Tile data loading
; Stack: VRAM offset
; r14, r15, r16, r17, r20, r22: Unused
;
; r18: At end, 0xF8 - 0xFF may be in this reg (if used correctly). This
;      indicates the pixels to render at end of line in the low 3 bits (0xF8:
;      8px, 0xFF: 1px).
;
; Tile offset bases in 4bpp modes:
; 0x00 - 0x3F: 0x0000
; 0x40 - 0x7F: 0x0800
; 0x80 - 0xBF: 0xF000 (due to the muls)
; 0xC0 - 0xFF: 0xF800 (due to the muls)
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
; The Video Stack is empty at this point, so can be restored to its base.
;

c1bp6a:
	; 1bpp 6 pixels wide ROM tile components
	mov   XL,      r11     ; (39) Replicate fg. color into XL
	lpm   YL,      Z       ; (42) Dummy load (nop)
	out   PIXOUT,  r8      ; (43) Pixel 6
	rjmp  c1bp6b           ; (45)
c1bp6:
	; 4x 1bpp 6 pixels wide ROM tiles
	mov   ZH,      r10     ; (18) Complete offset with high part
	lpm   YL,      Z       ; (21) Dummy load (nop)
	out   PIXOUT,  r5      ; (22) Pixel 3
	add   ZL,      r23     ; (23)
	lpm   r0,      Z       ; (26) Tile 0 pixel data
	pop   ZL               ; (28)
	out   PIXOUT,  r6      ; (29) Pixel 4
	add   ZL,      r23     ; (30)
	lpm   r1,      Z       ; (33) Tile 1 pixel data
	cpi   r19,     7       ; (34) Row mode 6 (no attr) or 7 (attributes)
	nop                    ; (35)
	out   PIXOUT,  r7      ; (36) Pixel 5
	brne  c1bp6a           ; (37 / 38) Attribute mode branch
	pop   YL               ; (39)
	ld    r11,     Y       ; (41) First tile px 1 color in r11
	swap  YL               ; (42)
	out   PIXOUT,  r8      ; (43) Pixel 6
	ld    XL,      Y       ; (45) Second tile px 1 color in XL
c1bp6b:
	mov   r25,     r24     ; () Expand bg. color
	movw  r2,      r24     ; () r3:r2, r25:r24
	movw  r4,      r24     ; () r5:r4, r25:r24
	movw  r6,      r24     ; () r7:r6, r25:r24
	out   PIXOUT,  r9      ; (50) Pixel 7
	sbrc  r0,      7       ; ()
	mov   r2,      r11     ; ()
	sbrc  r0,      6       ; ()
	mov   r3,      r11     ; ()
	sbrc  r0,      5       ; ()
	mov   r4,      r11     ; ()
	out   PIXOUT,  r2      ; ( 1) Pixel 0 (Output tile 0 & 1)
	sbrc  r0,      4       ; ()
	mov   r5,      r11     ; ()
	sbrc  r0,      3       ; ()
	mov   r6,      r11     ; ()
	sbrc  r0,      2       ; ()
	mov   r7,      r11     ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	lpm   YL,      Z       ; () Dummy load (nop)
	movw  r8,      r24     ; () r9:r8, r25:r24
	sbrc  r1,      7       ; ()
	mov   r8,      XL      ; ()
	out   PIXOUT,  r4      ; (15) Pixel 2
	sbrc  r1,      6       ; ()
	mov   r9,      XL      ; ()
	pop   ZL               ; ()
	add   ZL,      r23     ; ()
	movw  r2,      r24     ; () r3:r2, r25:r24
	out   PIXOUT,  r5      ; (22) Pixel 3
	lpm   r0,      Z       ; () Tile 2 pixel data
	movw  r4,      r24     ; () r5:r4, r25:r24
	sbrc  r1,      5       ; ()
	mov   r2,      XL      ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	sbrc  r1,      4       ; ()
	mov   r3,      XL      ; ()
	sbrc  r1,      3       ; ()
	mov   r4,      XL      ; ()
	sbrc  r1,      2       ; ()
	mov   r5,      XL      ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	pop   ZL               ; ()
	add   ZL,      r23     ; ()
	lpm   r1,      Z       ; () Tile 3 pixel data
	out   PIXOUT,  r8      ; (43) Pixel 6
	lpm   YL,      Z       ; () Dummy load (nop)
	lpm   YL,      Z       ; () Dummy load (nop)
	out   PIXOUT,  r9      ; (50) Pixel 7
	cpi   r19,     7       ; (51) Row mode 6 (no attr) or 7 (attributes)
	brne  c1bp6c           ; (52 / 53) Attribute mode branch
	pop   YL               ; (54)
	ld    r11,     Y       ; (56 = 0) First tile px 1 color in r11
	out   PIXOUT,  r2      ; ( 1) Pixel 0 (Output tile 1 & 2)
	swap  YL               ; ( 2)
	ld    XL,      Y       ; ( 4) Second tile px 1 color in XL
c1bp6d:
	movw  r6,      r24     ; () r7:r6, r25:r24
	sbrc  r0,      7       ; ()
	mov   r6,      r11     ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	movw  r8,      r24     ; () r9:r8, r25:r24
	movw  r2,      r24     ; () r3:r2, r25:r24
	sbrc  r0,      6       ; ()
	mov   r7,      r11     ; ()
	sbrc  r0,      5       ; ()
	mov   r8,      r11     ; ()
	out   PIXOUT,  r4      ; (15) Pixel 2
	lpm   YL,      Z       ; () Dummy load (nop)
	lpm   YL,      Z       ; () Dummy load (nop)
	out   PIXOUT,  r5      ; (22) Pixel 3
	rjmp  .                ; ()
	subi  r18,     24      ; () Bill the number of pixels output
	movw  r4,      r24     ; () r5:r4, r25:r24
	sbrc  r0,      4       ; ()
	mov   r9,      r11     ; ()
	out   PIXOUT,  r6      ; (29) Pixel 4
	sbrc  r0,      3       ; ()
	mov   r2,      r11     ; ()
	sbrc  r0,      2       ; ()
	mov   r3,      r11     ; ()
	sbrc  r1,      7       ; ()
	mov   r4,      XL      ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	movw  r6,      r24     ; () r7:r6, r25:r24
	movw  ZL,      r24     ; () ZH:ZL, r25:r24
	sbrc  r1,      6       ; ()
	mov   r5,      XL      ; ()
	sbrc  r1,      5       ; ()
	mov   r6,      XL      ; ()
	out   PIXOUT,  r8      ; (43) Pixel 6
	sbrc  r1,      4       ; ()
	mov   r7,      XL      ; ()
	sbrc  r1,      3       ; ()
	mov   ZL,      XL      ; ()
	sbrc  r1,      2       ; ()
	mov   ZH,      XL      ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	movw  r8,      ZL      ; () r9:r8, ZH:ZL
	ldi   r25,     32      ; () Restore 4bpp multiplier
	brcc  .+2              ; (53 / 54)
	rjmp  cend0            ; (55)
	rjmp  cloop            ; (56 = 0)
c1bp6c:
	lpm   YL,      Z       ; (56 = 0) Dummy load (nop)
	out   PIXOUT,  r2      ; ( 1) Pixel 0
	mov   XL,      r11     ; ( 2)
	rjmp  c1bp6d           ; ( 4)

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
c4tai0:
	out   PIXOUT,  r7      ; (36) Pixel 5
	lpm   r0,      Z       ; (39) Dummy load (nop)
	rjmp  c4tail           ; (41)

cflat:
	; Flat (single color) tile by the low 4 bits of ZL
	andi  ZL,      0x0F    ; (13)
	mov   r0,      ZL      ; (14)
	out   PIXOUT,  r4      ; (15) Pixel 2
	swap  ZL               ; (16)
	or    ZL,      r0      ; (17)
	mov   YL,      ZL      ; (18)
	mov   XL,      ZL      ; (19)
	ld    r2,      Y       ; (21)
	out   PIXOUT,  r5      ; (22) Pixel 3
	ld    r3,      Y       ; (24)
	ld    r4,      Y       ; (26)
	ld    r5,      Y       ; (28)
	out   PIXOUT,  r6      ; (29) Pixel 4
	rjmp  .                ; (31)
	rjmp  .                ; (33)
	rjmp  c4tai0           ; (35)

cb80ff:
	; 0x80 - 0xFF 4bpp ROM & RAM tiles
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	movw  ZL,      r0      ; ( 9)
	sbrc  ZH,      3       ; (10 / 11)
	rjmp  cramt            ; (12)
	add   ZH,      r12     ; (12) ROM 4bpp, tiles 0x80 - 0xBF adjusted base
	rjmp  c4com            ; (14)

csp00:
	; Row mode decisions
	breq  cflat            ; (11 / 12) Row mode 1: Flat tiles
	cpi   r19,     6       ; (12)
	brcs  c1bp8            ; (13 / 14) Row mode < 6: 8px wide 1bpp tiles
	nop                    ; (14)
	out   PIXOUT,  r4      ; (15) Pixel 2
	rjmp  c1bp6            ; (17)

c1bp8:
	; 1bpp 8 pixels wide ROM / RAM tile output
	out   PIXOUT,  r4      ; (15) Pixel 2
	nop                    ; (16)
	add   ZL,      r23     ; (17) Add tile index offset
	mov   ZH,      r10     ; (18) Complete offset with high part
	mov   r25,     r24     ; (19) Expand bg. color
	movw  r2,      r24     ; (20) r3:r2, r25:r24
	movw  r0,      r24     ; (21) r1:r0, r25:r24
	out   PIXOUT,  r5      ; (22) Pixel 3
	cpi   r19,     4       ; (23) Row mode 4 (ROM tiles) or 5 (RAM tiles)
	breq  .+4              ; (24 / 25)
	ld    XL,      Z       ; (26) RAM tile
	rjmp  .+2              ; (28)
	lpm   XL,      Z       ; (28) ROM tile
	out   PIXOUT,  r6      ; (29) Pixel 4
	movw  r4,      r24     ; () r5:r4, r25:r24
	subi  r18,     8       ; () Bill the number of pixels output
	sbrc  XL,      7       ; ()
	mov   r2,      r11     ; ()
	sbrc  XL,      6       ; ()
	mov   r3,      r11     ; ()
	out   PIXOUT,  r7      ; (36) Pixel 5
	sbrc  XL,      5       ; ()
	mov   r4,      r11     ; ()
	sbrc  XL,      4       ; ()
	mov   r5,      r11     ; ()
	movw  r6,      r24     ; () r7:r6, r25:r24
	ldi   r25,     32      ; () Restore 4bpp multiplier
	out   PIXOUT,  r8      ; (43) Pixel 6
	sbrc  XL,      3       ; ()
	mov   r6,      r11     ; ()
	sbrc  XL,      2       ; ()
	mov   r7,      r11     ; ()
	sbrc  XL,      1       ; ()
	mov   r0,      r11     ; ()
	out   PIXOUT,  r9      ; (50) Pixel 7
	sbrc  XL,      0       ; ()
	mov   r1,      r11     ; ()
	movw  r8,      r0      ; () r9:r8, r1:r0
	brcs  cend0            ; (54 / 55)
	rjmp  cloop            ; (56 = 0)

cloop:
	; Normal loop entry point
	out   PIXOUT,  r2      ; ( 1) Pixel 0
centry:
	; Scanline render entry point (at cycle 1)
	pop   ZL               ; ( 3)
	muls  ZL,      r25     ; ( 5) cccc sttt ttt0 0000
	brcs  cb80ff           ; ( 6 /  7)
	cpi   r19,     1       ; ( 7) Row mode 0?
	out   PIXOUT,  r3      ; ( 8) Pixel 1
	brcc  csp00            ; ( 9 / 10) Not zero (as larger or eq to 1): Other row modes
	movw  ZL,      r0      ; (10)
	mov   r0,      r10     ; (11) ROM 4bpp, tiles 0x00 - 0x3F base
	sbrc  ZH,      3       ; (12 / 13)
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
	ldi   r24,     lo8(M74_VIDEO_STACK + 15) ; (56 = 0)
	out   PIXOUT,  r2      ; ( 1) Pixel 0
	ldi   r25,     hi8(M74_VIDEO_STACK + 15) ; ( 2)
	out   STACKL,  r24     ; ( 3) Restore the empty Video stack
	out   STACKH,  r25     ; ( 4)
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
; Horizontal size reduction block (this part is normally jumped over)
;
drloop:
#if (M74_SD_ENABLE != 0)
	M74WT_R24      15      ; (16)
	movw  ZL,      r14     ; (17) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (52) 35 cycles
	movw  r14,     ZL      ; (53) r15:r14, ZH:ZL Target pointer
#else
	M74WT_R24      52      ; (53)
#endif
	subi  r22,     8       ; (54)
	brcs  drend            ; (55 / 56 = 0)
	rjmp  drloop           ; ( 1)
;
; Return for exiting the scanline loop (this part is normally jumped over)
;
sclpret:
	rjmp  m74_scloopr      ; (1707)



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
; Horizontal size reduction, trailing part. In r22 the horizontal size is
; preserved from before the scanline.
;
	subi  r22,     8       ; ( 1)
	brcc  drloop           ; ( 2 /  3) (1700)
drend:



;
; Loop test condition: If the scanline counter reached the count of lines to
; render, return. It is possible to request zero lines. Note: NO CALL! It
; resets the Video Stack to its base!
;
; Entry is at cycle 1700. (the rjmp must be issued at 1698)
; Exit is at 1707.
;
m74_scloop:
	lds   r22,     render_lines_count ; (1702)
	cp    r22,     r16     ; (1703)
	breq  sclpret          ; (1704 / 1705)
	clr   YL               ; (1705) YL is used as a zero register until Color0 reload



;
; Row management code
;
; r23: Zero
; r16: Scanline counter (Normally 0 => 223)
; r17: Logical row counter
;
; Cycles:
;  18 (Row select)
;  36 (Tile descriptor load)
;  16 (Tile index load)
; ---
;  70
;
	lds   r9,      m74_config   ; ( 2)
	lds   ZL,      v_rows_lo    ; ( 4)
	lds   ZH,      v_rows_hi    ; ( 6)
	ld    r25,     Z+      ; ( 8)
	cp    r25,     r16     ; ( 9)
	brne  mrespl           ; (10 / 11) Did not reach new split point yet
	ld    r17,     Z+      ; (12) Load new logical row counter
	ld    r20,     Z+      ; (14) Load new X shift
	sts   v_rows_lo, ZL    ; (16)
	sts   v_rows_hi, ZH    ; (18)
mrese:
	;
	; Load tile descriptors
	;
	lds   ZL,      m74_tdesc_lo ; ( 2)
	lds   ZH,      m74_tdesc_hi ; ( 4)
	mov   r24,     r17     ; ( 5)
	lsr   r24              ; ( 6)
	lsr   r24              ; ( 7)
	lsr   r24              ; ( 8) Tile descriptor offset from log. row counter
	add   ZL,      r24     ; ( 9)
	adc   ZH,      YL      ; (10)
	sbrs  r9,      1       ; (11 / 12)
	rjmp  mrtiro           ; (13)
	ld    ZL,      Z       ; (14) Tile descriptor index from RAM
	rjmp  mrtira           ; (16)
mrespl:
	; Row select - no reload path
	sbiw  ZL,      2       ; (13) No new line: just load prev. X shift
	ld    r20,     Z       ; (15)
	nop                    ; (16)
	rjmp  mrese            ; (18)
mrtdra:
	subi  ZL,      lo8(-(M74_RAMTD_OFF - 128)) ; (21)
	sbci  ZH,      hi8(-(M74_RAMTD_OFF - 128)) ; (22)
	rjmp  .                ; (24)
	ld    r22,     Z+      ; (26)
	ld    r10,     Z+      ; (28)
	ld    r11,     Z+      ; (30)
	ld    r12,     Z+      ; (32)
	ld    r21,     Z+      ; (34)
	rjmp  mrtdco           ; (36)
mrtiro:
	lpm   ZL,      Z       ; (16) Tile descriptor index from ROM
mrtira:
	clr   ZH               ; (17)
	sbrc  ZL,      7       ; (18 / 19) Bit 7 zero: ROM
	rjmp  mrtdra           ; (20)
	subi  ZL,      lo8(-(M74_ROMTD_OFF)) ; (20)
	sbci  ZH,      hi8(-(M74_ROMTD_OFF)) ; (21)
	lpm   r22,     Z+      ; (24)
	lpm   r10,     Z+      ; (27)
	lpm   r11,     Z+      ; (30)
	lpm   r12,     Z+      ; (33)
	lpm   r21,     Z+      ; (36)
mrtdco:
	;
	; Load tile indices
	;
	lds   ZL,      m74_tidx_lo  ; ( 2)
	lds   ZH,      m74_tidx_hi  ; ( 4)
	lsl   r24              ; ( 5)
	add   ZL,      r24     ; ( 6)
	adc   ZH,      YL      ; ( 7)
	sbrs  r9,      2       ; ( 8 /  9)
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
; Process Color 0 reloading if enabled, otherwise an SD load slot. YL is zero
; at this point.
;
; 40 cycles
;
; At 1815 at the end
;
#if (M74_COL0_OFF != 0)
	sbrs  r9,      4       ; ( 1 /  2)
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
#if (M74_SD_ENABLE != 0)
	movw  ZL,      r14     ; ( 4) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (39) 35 cycles
	movw  r14,     ZL      ; (40) r15:r14, ZH:ZL Target pointer
#else
	M74WT_R24      37      ; (40)
#endif
c0rle:
#else
#if (M74_SD_ENABLE != 0)
	M74WT_R24      3       ; ( 3)
	movw  ZL,      r14     ; ( 4) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (39) 35 cycles
	movw  r14,     ZL      ; (40) r15:r14, ZH:ZL Target pointer
#else
	M74WT_R24      40      ; (40)
#endif
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
; 215 (Allowing 4CH audio or either 5CH or the UART)
;
; From the prolog some calculations are moved here so to make it possible
; to shift the display a bit around for proper centering.
;
; Cycle counter is at 227 on its end
;
	sbiw  XL,      1       ; (1817) Adjust for stack (pre-incrementing)
	movw  r4,      XL      ; (1818) r5:r4, XH:XL, putting it aside
	mov   r19,     r22     ; (1819) Split the config byte
	mov   r23,     r22     ; (1820 = 0)
	andi  r19,     0x07    ; (   1) Mode selector's final resting place is r19
	andi  r22,     0x18    ; (   2) Row width
	andi  r23,     0xE0    ; (   3) High bits masked (used for 1bpp tile index)
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	ldi   r18,     192     ; (   6) No. of pixels to output
	andi  r20,     0x07    ; (   7) Mask X shift to 0 - 7
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES



;
; Do horizontal size reduction
;
; 2 cycles
;
	cpi   r22,     0       ; ( 1)
	brne  dfred            ; ( 2 / 3) Needs reduction
dfend:



;
; Branch off to special row modes
;
; Modes 0-1: 5 cycles
; Mode    2: 8 cycles
; Mode    3: 7 cycles
; Modes 4-7: 3 cycles
;
	sbrc  r19,     2       ; ( 1 /  2)
	rjmp  mtm4567          ; ( 3) Modes 4-7 using 4bpp and 1bpp tiles
#if   ((M74_M2_ENABLE != 0U) || (M74_M3_ENABLE != 0U))
	sbrs  r19,     1       ; ( 3 /  4)
	rjmp  mtm01            ; ( 5) Modes 0-1 using only 4bpp tiles
#else
	nop                    ; ( 3)
	rjmp  mtm01            ; ( 5) Modes 0-1 using only 4bpp tiles
#endif
#if   ((M74_M2_ENABLE == 0U) && (M74_M3_ENABLE == 0U))
#elif ((M74_M2_ENABLE != 0U) && (M74_M3_ENABLE != 0U))
	sbrc  r19,     0       ; ( 5 /  6)
	rjmp  m74_m3_2bppmc    ; ( 7) Jump off to 2bpp Multicolor
	rjmp  m74_m2_separator ; ( 8) Jump off to separator line
#elif ((M74_M2_ENABLE != 0U))
	rjmp  .                ; ( 6)
	rjmp  m74_m2_separator ; ( 8) Jump off to separator line
#else
	nop                    ; ( 5)
	rjmp  m74_m3_2bppmc    ; ( 7) Jump off to 2bpp Multicolor
#endif



;
; Horizontal size reduction block (this part is normally jumped over)
;
dfred:
	mov   YL,      r22     ; ( 2) Need to preserve r22 for trailing reduction
dfloop:
#if (M74_SD_ENABLE != 0)
	M74WT_R24      13      ; (15)
	movw  ZL,      r14     ; (16) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (51) 35 cycles
	movw  r14,     ZL      ; (52) r15:r14, ZH:ZL Target pointer
#else
	M74WT_R24      50      ; (52)
#endif
	subi  r18,     16      ; (53) Reduce total count of pixels to output
	subi  YL,      8       ; (54)
	breq  dfend            ; (55 / 56 = 0)
	nop                    ; (56 = 0)
	rjmp  dfloop           ; ( 2)



;
; Prepare for rendering modes 0, 1, 2, 4, 5, 6
;
; 13 cycles
;
mtm01:
	mov   XH,      r17     ; ( 3)
	andi  XH,      0x07    ; ( 4)
	rjmp  mtm01e           ; ( 6)
mtm4567:
	mov   XH,      r17     ; ( 1)
	andi  XH,      0x07    ; ( 2)
	add   r10,     XH      ; ( 3) 1bpp row offset simply adds to high
	mov   YL,      r11     ; ( 4) Background & Foreground color for 1bpp
	ld    r11,     Y       ; ( 6) Foreground (1) color
mtm01e:
	swap  YL               ; ( 7) Prepared for bg. color load (into r24)
	lsl   XH               ; ( 8) Prepare 4bpp row select
	lsl   XH               ; ( 9)
	mov   r13,     XH      ; (10) Done
	ldi   r25,     32      ; (11) Load 32 for 4bpp tile offset calculation
	mov   XH,      YH      ; (12) XH is also a palette pointer
	clr   r2               ; (13) Leftmost pixel of partial tile (scroll) is zero



;
; Increment line counters
;
; 2 cycles
;
	inc   r16              ; ( 1) Increment the physical line counter
	inc   r17              ; ( 2) Increment the logical line counter



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
; 51 cycles
;
	cpi   r20,     0       ; ( 1)
	breq  pnscr            ; ( 2 /  3) If not scrolling, then no partial loads
	;
	; Partial tile load code
	;
	; Supports only 8px wide tiles, either 1bpp or 4bpp. The rest (6px)
	; can not be scrolled by X (at least not past the left edge).
	;
	ld    r24,     Y       ; ( 4) Finish attribute mode color picking: Bg. color
	out   STACKL,  r4      ; ( 5)
	out   STACKH,  r5      ; ( 6) VRAM pointer set up
	pop   ZL               ; ( 8)
	muls  ZL,      r25     ; (10) cccc sttt ttt0 0000
	brcs  pb80ff           ; (11 / 12)
	cpi   r19,     1       ; (12) Row mode 0?
	brcc  psp00            ; (13 / 14) Not zero (as larger or eq to 1): Other row modes
	movw  ZL,      r0      ; (14)
	mov   r0,      r10     ; (15) ROM 4bpp, tiles 0x00 - 0x3F base
	sbrc  ZH,      3       ; (16 / 17)
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
p1bcom:
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
#if (M74_SD_ENABLE != 0)
	movw  ZL,      r14     ; ( 4) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (39) 35 cycles
	movw  r14,     ZL      ; (40) r15:r14, ZH:ZL Target pointer
#else
	M74WT_R24      37      ; (40)
#endif
	ld    r24,     Y       ; (42) Finish attribute mode color picking: Bg. color
	out   STACKL,  r4      ; (43)
	out   STACKH,  r5      ; (44) VRAM pointer set up
	clr   r3               ; (45)
	movw  r4,      r2      ; (46) r5:r4, r3:r2
	movw  r6,      r2      ; (47) r7:r6, r3:r2
	movw  r8,      r2      ; (48) r9:r8, r3:r2
	subi  r18,     8       ; (49)
	rjmp  centry           ; (51) Go for the scanline output!
pb80ff:
	; 0x80 - 0xFF 4bpp ROM & RAM tiles
	movw  ZL,      r0      ; (13)
	sbrc  ZH,      3       ; (14 / 15)
	rjmp  pramt            ; (16)
	add   ZH,      r12     ; (16) ROM 4bpp, tiles 0x80 - 0xBF adjusted base
	rjmp  p4com            ; (18)
psp00:
	; Row mode decisions
	nop                    ; (15)
	breq  pflat            ; (16 / 17) Row mode 1: Flat tiles
	rjmp  p1bp8            ; (18) Assume 8px wide 1bpp tiles
pflat:
	; Flat (single color) tile by the low 4 bits of ZL
	andi  ZL,      0x0F    ; (18)
	mov   r0,      ZL      ; (19)
	swap  ZL               ; (20)
	or    ZL,      r0      ; (21)
	mov   YL,      ZL      ; (22)
	mov   XL,      ZL      ; (23)
	ld    r3,      Y       ; (25)
	ld    r4,      Y       ; (27)
	ld    r5,      Y       ; (29)
	lpm   r0,      Z       ; (32) Dummy load (nop)
	rjmp  p4tai0           ; (34)

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
p4tai0:
	lpm   r0,      Z       ; (37) Dummy load (nop)
	rjmp  p4tail           ; (39)
p1bp8:
	; 1bpp 8 pixels wide ROM / RAM tile output
	add   ZL,      r23     ; (19) Add tile index offset
	mov   ZH,      r10     ; (20) Complete offset with high part
	mov   r3,      r24     ; (21) Fill up with bg. color
	mov   r4,      r24     ; (22)
	mov   r5,      r24     ; (23)
	movw  r6,      r4      ; (24) r7:r6, r5:r4
	movw  r8,      r4      ; (25) r9:r8, r5:r4
	cpi   r19,     4       ; (26) Row mode 4 (ROM tiles) or 5 (RAM tiles)
	breq  .+4              ; (27 / 28)
	ld    r0,      Z       ; (29) RAM tile
	rjmp  .+2              ; (31)
	lpm   r0,      Z       ; (31) ROM tile
	sbrc  r0,      6       ; (32)
	mov   r3,      r11     ; (33)
	sbrc  r0,      5       ; (34)
	mov   r4,      r11     ; (35)
	sbrc  r0,      4       ; (36)
	mov   r5,      r11     ; (37)
	sbrc  r0,      3       ; (38)
	mov   r6,      r11     ; (39)
	sbrc  r0,      2       ; (40)
	mov   r7,      r11     ; (41)
	sbrc  r0,      1       ; (42)
	mov   r8,      r11     ; (43)
	sbrc  r0,      0       ; (44)
	mov   r9,      r11     ; (45)
	rjmp  .                ; (47)
	rjmp  p1bcom           ; (49)
