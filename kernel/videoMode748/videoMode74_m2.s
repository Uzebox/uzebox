;
; Uzebox Kernel - Video Mode 748 Row mode 2 (Separator line)
; Copyright (C) 2017 Sandor Zsuga (Jubatian)
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
; Enters in cycle 1813. After doing its work, it returns to m74_scloop_sr,
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
; r18: Byte 0 of tile descriptor (mode)
; r10: Byte 1 of tile descriptor
; r11: Byte 2 of tile descriptor
; r12: Byte 3 of tile descriptor
; r21: Byte 4 of tile descriptor
; X:   Offset from tile index list
; r19: Global configuration (m74_config)
; YH:  Palette buffer, high
;
; Everything expect r14, r15, r16, r17, r19, r22, r23, r25, and YH may be
; clobbered. Register r18 has to be set zero before return.
;
m74_m2_separator:

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
	M74WT_R24      10      ; (   3)
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	rjmp  .                ; (   7)
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES


;
; Cycles:  81
; Ends:    327
;

	M74WT_R24      81      ; (81)



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
	sbrs  r18,     6       ; ( 7) No loading at all if set
	rjmp  m2load           ; ( 9) Need loading palette
m2nold:



;
; No palette reload
;
; Cycles:  727
; Ends:   1062
;
	mov   YL,      r10     ; ( 1)
	ld    r2,      Y       ; ( 3) Color of separator line acquired
	M74WT_R24      15      ; (18)
	out   PIXOUT,  r2      ; (19) ( 354)
	M74WT_R24      706     ; (725)
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
	adiw  XL,      16      ; ( 8) Select palette to load
	M74WT_R24      7       ; (15)
	rjmp  m2ldcm           ; (17)
m2load:
	cpi   r24,     0       ; ( 1) Load before or after coloring separator?
	brne  m2ldaf           ; ( 2 /  3) To load after
	rjmp  .                ; ( 4)
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
	sbrs  r18,     5       ; ( 2 /  3) ROM / RAM palette source
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
	rcall m74_setpalcol    ; (656) (41 * 16 cycles)
m2comm:



;
; Common trailing region
;
; Cycles:  630
; Ends:   1692
;
	M74WT_R24      630     ; (630)



;
; Row counter increments & Return
;
; Cycles:    5
; Ends:   1697
;

	inc   r16              ; ( 1) Physical scanline counter increment
	inc   r17              ; ( 2) Logical row counter increment
	clr   r18              ; ( 3)
	rjmp  m74_scloop_sr    ; ( 5) (1697)
