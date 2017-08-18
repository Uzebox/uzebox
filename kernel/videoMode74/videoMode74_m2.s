;
; Uzebox Kernel - Video Mode 74 Row mode 2 (Separator line)
; Copyright (C) 2015 Sandor Zsuga (Jubatian)
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
; Separator line with palette reload and stuff.
;
; Enters in cycle 237. After doing its work, it returns to m74_scloop_sr,
; cycle 1697 of the next line. For the documentation of what it displays, see
; the comments for m74_tdesc in videoMode74.s
;
; First pixel output in 24 tile wide mode has to be performed at cycle 353 (so
; OUT finishing in 354).
;
; Depending on the width (r18: number of pixels to produce) there are either
; 1344, 1232, 1120 or 1008 display cycles. That is up to 336 cycles might not
; be present.
;
; r16: Scanline counter (increments it by one)
; r17: Logical row counter (increments it by one)
; r18: Number of pixels to generate
; r20: X shift (0 - 7)
; r19: 2 (Row mode)
; r22: Row width bits (0, 8, 16, 24 for tile counts 24, 22, 20, 18)
; r23: High 3 bits of byte 0 of descriptor
; r10: Byte 1 of tile descriptor
; r11: Byte 2 of tile descriptor
; r12: Byte 3 of tile descriptor
; r21: Byte 4 of tile descriptor
; X:   Offset from tile index list reduced by 1
; r9:  Global configuration (m74_config)
; r14: SD load address, low
; r15: SD load address, high
; YH:  Palette buffer, high
;
; Everything expect r14, r15, r16, r17, r22, and YH may be clobbered.
; Register r18 has to be set zero before return.
;
m74_m2_separator:

;
; Do SD loads until nearing the first pixel
;
; Cycles:  90
; Ends:    327
;

#if (M74_SD_ENABLE != 0)
	movw  ZL,      r14     ; ( 1) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (36) 35 cycles
	nop                    ; (37) Delay for SPI
	rcall m74_spiload_core ; (72) 35 cycles
	movw  r14,     ZL      ; (73) r15:r14, ZH:ZL Target pointer
	M74WT_R24      17      ; (90)
#else
	M74WT_R24      90      ; (90)
#endif



;
; Branch off to palette loading or doing nothing during the line
;
; Loading:    9 cycles (ends at 336)
; No loading: 8 cycles (ends at 335)
;

	mov   r24,     r17     ; ( 1)
	andi  r24,     0x7     ; ( 2)
	breq  .+2              ; ( 3 / 4)
	cpi   r24,     0x7     ; ( 4)
	breq  m2ldts           ; ( 5 / 6) 0 or 7 (first or last row): Maybe needs loading
	nop                    ; ( 6)
	rjmp  m2nold           ; ( 8)
m2ldts:
	sbrs  r23,     6       ; ( 7) No loading at all if set
	rjmp  m2load           ; ( 9) Need loading palette
m2nold:



;
; No palette reload: Stream from SD instead if enabled
;
; Cycles:  727
; Ends:   1062
;
	mov   YL,      r10     ; ( 1)
	ld    r2,      Y       ; ( 3) Color of separator line acquired
	M74WT_R24      15      ; (18)
	out   PIXOUT,  r2      ; (19) ( 354)
#if (M74_SD_ENABLE != 0)
	movw  ZL,      r14     ; ( 1) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (36) 35 cycles
	nop                    ; (37) Delay for SPI
	rcall m74_spiload_core ; (72) 35 cycles
	nop                    ; (73) Delay for SPI
	rcall m74_spiload_core ; (108) 35 cycles
	nop                    ; (109) Delay for SPI
	rcall m74_spiload_core ; (144) 35 cycles
	nop                    ; (145) Delay for SPI
	rcall m74_spiload_core ; (180) 35 cycles
	nop                    ; (181) Delay for SPI
	rcall m74_spiload_core ; (216) 35 cycles
	nop                    ; (217) Delay for SPI
	rcall m74_spiload_core ; (252) 35 cycles
	nop                    ; (253) Delay for SPI
	rcall m74_spiload_core ; (288) 35 cycles
	nop                    ; (289) Delay for SPI
	rcall m74_spiload_core ; (324) 35 cycles
	nop                    ; (325) Delay for SPI
	rcall m74_spiload_core ; (360) 35 cycles
	nop                    ; (361) Delay for SPI
	rcall m74_spiload_core ; (396) 35 cycles
	nop                    ; (397) Delay for SPI
	rcall m74_spiload_core ; (432) 35 cycles
	nop                    ; (433) Delay for SPI
	rcall m74_spiload_core ; (468) 35 cycles
	nop                    ; (469) Delay for SPI
	rcall m74_spiload_core ; (504) 35 cycles
	nop                    ; (505) Delay for SPI
	rcall m74_spiload_core ; (540) 35 cycles
	nop                    ; (541) Delay for SPI
	rcall m74_spiload_core ; (576) 35 cycles
	nop                    ; (577) Delay for SPI
	rcall m74_spiload_core ; (612) 35 cycles
	nop                    ; (613) Delay for SPI
	rcall m74_spiload_core ; (648) 35 cycles
	nop                    ; (649) Delay for SPI
	rcall m74_spiload_core ; (684) 35 cycles
	movw  r14,     ZL      ; (685) (704) r15:r14, ZH:ZL Target pointer
	M74WT_R24      21      ; (725)
#else
	M74WT_R24      706     ; (725)
#endif
	rjmp  m2comm           ; (727)



;
; Palette loading (either before or after separator color), entry: color the
; separator line.
;
; Cycles:   18
; Ends:    354 (Outputs line color)
;

m2ldaf:
	mov   YL,      r10     ; ( 4)
	ld    r2,      Y       ; ( 6) Color of separator line acquired
	adiw  XL,      17      ; ( 8) Select palette to load
	M74WT_R24      7       ; (15)
	rjmp  m2ldcm           ; (17)
m2load:
	cpi   r24,     0       ; ( 1) Load before or after coloring separator?
	brne  m2ldaf           ; ( 2 /  3) To load after
	adiw  XL,      1       ; ( 4) Restore adjusted tile index list
	mov   YL,      r10     ; ( 5)
	andi  YL,      0xF0    ; ( 6)
	swap  YL               ; ( 7) Index to use
	clr   r2               ; ( 8)
	movw  ZL,      XL      ; ( 9)
	add   ZL,      YL      ; (10)
	adc   ZH,      r2      ; (11) Color's offset
	sbrs  r23,     5       ; (12 / 13) ROM / RAM palette source
	rjmp  .+4              ; (14)
	ld    r2,      Z       ; (15) Color of separator line acquired
	rjmp  .+2              ; (17)
	lpm   r2,      Z       ; (17) Color of separator line acquired
m2ldcm:
	out   PIXOUT,  r2      ; (18) ( 354)



;
; Load the palette in registers. The following registers are taken for this
; (retaining the capability to call m74_setpalcol):
;
;  r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
;  r8,  r9, r10, r11, r12, r13, r20, r21
;
; Cycles:   52
; Ends:    406
;
	movw  ZL,      XL      ; ( 1)
	sbrs  r23,     5       ; ( 2 /  3) ROM / RAM palette source
	rjmp  m2prom           ; ( 4)
	ld    r0,      Z+      ; ( 5)
	ld    r1,      Z+      ; ( 7)
	ld    r2,      Z+      ; ( 9)
	ld    r3,      Z+      ; (11)
	ld    r4,      Z+      ; (13)
	ld    r5,      Z+      ; (15)
	ld    r6,      Z+      ; (17)
	ld    r7,      Z+      ; (19)
	ld    r8,      Z+      ; (21)
	ld    r9,      Z+      ; (23)
	ld    r10,     Z+      ; (25)
	ld    r11,     Z+      ; (27)
	ld    r12,     Z+      ; (29)
	ld    r13,     Z+      ; (31)
	ld    r20,     Z+      ; (33)
	ld    r21,     Z+      ; (35)
	M74WT_R24      15      ; (50)
	rjmp  m2pcom           ; (52)
m2prom:
	lpm   r0,      Z+      ; ( 7)
	lpm   r1,      Z+      ; (10)
	lpm   r2,      Z+      ; (13)
	lpm   r3,      Z+      ; (16)
	lpm   r4,      Z+      ; (19)
	lpm   r5,      Z+      ; (22)
	lpm   r6,      Z+      ; (25)
	lpm   r7,      Z+      ; (28)
	lpm   r8,      Z+      ; (31)
	lpm   r9,      Z+      ; (34)
	lpm   r10,     Z+      ; (37)
	lpm   r11,     Z+      ; (40)
	lpm   r12,     Z+      ; (43)
	lpm   r13,     Z+      ; (46)
	lpm   r20,     Z+      ; (49)
	lpm   r21,     Z+      ; (52)
m2pcom:



;
; Refill the palette buffer
;
; Cycles:  656
; Ends:   1062
;
#if (M74_COL0_DISABLE == 0)
	clr   YL               ; ( 1)
	mov   r24,     r0      ; ( 2)
	rcall m74_setpalcol    ; (41) (3 + 36 cycles)
	inc   YL               ; ()
#else
	M74WT_R24      41      ; ()
	ldi   YL,      16      ; ()
#endif
	mov   r24,     r1      ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r2      ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r3      ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r4      ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r5      ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r6      ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r7      ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r8      ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r9      ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r10     ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r11     ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r12     ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r13     ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r20     ; ()
	rcall m74_setpalcol    ; ()
	inc   YL               ; ()
	mov   r24,     r21     ; ()
	rcall m74_setpalcol    ; (656) (41 * 6 cycles)
m2comm:



;
; Common trailing region: SD loads if enabled
;
; Cycles:  292
; Ends:   1354
;
#if (M74_SD_ENABLE != 0)
	movw  ZL,      r14     ; ( 1) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (36) 35 cycles
	nop                    ; (37) Delay for SPI
	rcall m74_spiload_core ; (72) 35 cycles
	nop                    ; (73) Delay for SPI
	rcall m74_spiload_core ; (108) 35 cycles
	nop                    ; (109) Delay for SPI
	rcall m74_spiload_core ; (144) 35 cycles
	nop                    ; (145) Delay for SPI
	rcall m74_spiload_core ; (180) 35 cycles
	nop                    ; (181) Delay for SPI
	rcall m74_spiload_core ; (216) 35 cycles
	nop                    ; (217) Delay for SPI
	rcall m74_spiload_core ; (252) 35 cycles
	nop                    ; (253) Delay for SPI
	rcall m74_spiload_core ; (288) 35 cycles
	movw  r14,     ZL      ; (289) r15:r14, ZH:ZL Target pointer
	M74WT_R24      3       ; (292)
#else
	M74WT_R24      292     ; (292)
#endif



;
; Row counter increments
;
; Cycles:    2
; Ends:   1356
;

	inc   r16              ; ( 1) Physical scanline counter increment
	inc   r17              ; ( 2) Logical row counter increment



;
; Variable count of available cycles (depending on row width)
; Here 336 (56 * 6), 224 (56 * 4), 112 (56 * 2) or 0 cycles may be present
; depending on width, still in r18 (192 / 176 / 160 / 144 pixels). This also
; sets r18 to zero on its end.
;
; Cycles:  339
; Ends:   1695
;
	subi  r18,     144     ; ( 1)
	breq  m2wdce           ; ( 2 /  3)
m2wdlp:
#if (M74_SD_ENABLE != 0)
	movw  ZL,      r14     ; ( 1) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (36) 35 cycles
	nop                    ; (37) Delay for SPI
	rcall m74_spiload_core ; (72) 35 cycles
	nop                    ; (73) Delay for SPI
	rcall m74_spiload_core ; (108) 35 cycles
	movw  r14,     ZL      ; (109) r15:r14, ZH:ZL Target pointer
#else
	M74WT_R24      109     ; (109)
#endif
	subi  r18,     16      ; (110)
	brne  m2wdlp           ; (111 / 112)
	rjmp  .                ; (113)
m2wdce:



;
; Done, return
;
	rjmp  m74_scloop_sr    ; (1697)
