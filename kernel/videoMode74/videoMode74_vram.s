;
; Uzebox Kernel - Video Mode 74 VRAM assistance functions
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

;
; The routines here assist in creating scrolling algorithms based on simple
; VRAM copies when passing tile boundaries. In these types of scrolling
; algorithms the VRAM is a rectangular region with no X or Y wrapping within,
; which state is achieved by copying its contents whenever a tile boundary is
; crossed.
;


;
; void M74_VramMove(signed char x, signed char y);
;
; Moves the contents of the VRAM with the given amount of tiles. Uses a bit
; more than 4 cycles per copied tile for usual use cases. Up to 25 columns may
; be moved, so to move 1 tile left / right, at most 25 tiles of VRAM width may
; be set up (the VRAM pitch may be larger).
;
.global M74_VramMove

;
; void M74_VramFillCol(unsigned char y, unsigned int src, unsigned char incr);
;
; Fills a column of VRAM from the given source. Takes as many bytes as tall
; the VRAM is. The source is incremented by incr after each copy. If the
; source is larger or equal to 0x1000, it is treated as a ROM pointer,
; otherwise a RAM pointer.
;
.global M74_VramFillCol

;
; void M74_VramFillRow(unsigned char x, unsigned int src, unsigned char incr);
;
; Fills a row of VRAM from the given source. Takes as many bytes as wide the
; VRAM is (using the width, not the pitch for this). The source is incremented
; by incr after each copy. If the source is larger or equal to 0x1000, it is
; treated as a ROM pointer, otherwise a RAM pointer.
;
.global M74_VramFillRow

;
; void M74_VramFill(unsigned int src, unsigned char pitch);
;
; Fills the VRAM from the given source. Behaves like calling M74_VramFillRow
; for every row with an increment of one, between rows incrementing the source
; pointer from its value on the beginning of the row by pitch.
;
.global M74_VramFill


.section .text



;
; void M74_VramMove(signed char x, signed char y);
;
; Moves the contents of the VRAM with the given amount of tiles. Uses a bit
; more than 4 cycles per copied tile for usual use cases. Up to 25 columns may
; be moved, so to move 1 tile left / right, at most 25 tiles of VRAM width may
; be set up (the VRAM pitch may be larger).
;
;     r24: x
;     r22: y
; Clobbered registers:
; r0, r1 (set zero), r18, r19, r20, r21, r22, r23, r24, r25, X, Z, T
;
.section .text.M74_VramMove
M74_VramMove:

	; Load VRAM layout

	ldi   XL,      lo8(M74_VRAM_OFF)
	ldi   XH,      hi8(M74_VRAM_OFF)
	ldi   r25,     M74_VRAM_W
	ldi   r23,     M74_VRAM_H
	ldi   r21,     M74_VRAM_P
	clr   r20              ; Will be used as a zero reg.

	; Determine width and height of the move into r23 and r25 also
	; acquiring absolute values in r19 and r18.

	mov   r18,     r24
	sbrc  r18,     7
	neg   r18              ; Absolute value of X movement
	sub   r25,     r18
	breq  vrmc0
	brcc  vrmc1
vrmc0:
	ret                    ; Would move more than the width
vrmc1:
	mov   r19,     r22
	sbrc  r19,     7
	neg   r19              ; Absolute value of Y movement
	sub   r23,     r19
	breq  vrmc2
	brcc  vrmc3
vrmc2:
	ret                    ; Would move more than the height
vrmc3:

	; Safety constraint: At most 25 tiles (there is no reason to copy
	; more, and only this many is provided in the unrolled loops)

	cpi   r25,     26
	brcc  vrmc2

	; Determine direction of movement to decide whether the copy has to
	; be performed backwards or forward.

	bst   r24,     7       ; T: X direction (sign)
	cpi   r22,     0
	breq  vrmxq            ; No Y scroll, X decides
	brpl  vrmcdec          ; Positive: Decrementing copy is necessary
	rjmp  vrmcinc          ; Negative: Incrementing copy is necessary
vrmxq:
	cpi   r24,     0
	breq  vrmc2            ; Both zero: Nothing to do, be fast
vrmyq:
	brpl  vrmcdec          ; Positive: Decrementing copy is necessary
	rjmp  vrmcinc          ; Negative: Incrementing copy is necessary

vrmcdec:

	; Decrementing copy: bottom to top and (normally) right to left order.

	movw  ZL,      XL      ; ZH:ZL, XH:XL
	mov   r0,      r23
	add   r0,      r19
	dec   r0               ; (Original) Height - 1
	mul   r0,      r21
	add   r0,      r25
	adc   r1,      r20
	brts  .+4              ; X direction: copy to right if positive
	add   r0,      r18
	adc   r1,      r20     ; Target bottom right corner offset acquired
	add   XL,      r0
	adc   XH,      r1      ; Target address calculated
	mov   r0,      r23
	dec   r0               ; Remaining (to be moved) height - 1
	mul   r0,      r21
	add   r0,      r25
	adc   r1,      r20
	brtc  .+4              ; X direction: copy from right if negative
	add   r0,      r18
	adc   r1,      r20     ; Source bottom right corner offset acquired
	add   ZL,      r0
	adc   ZH,      r1      ; Source address calculated
	sub   r21,     r25     ; Pitch: to decrement between rows

	; Copy loop (up to 25 tiles)

	mov   r24,     r23
	movw  r22,     YL
	movw  YL,      ZL
	lsl   r25
	ldi   ZL,      lo8(pm(vrmcdt))
	ldi   ZH,      hi8(pm(vrmcdt))
	sub   ZL,      r25
	sbc   ZH,      r20
	rjmp  vrmcdl
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
	ld    r1,      -Y
	st    -X,      r1
vrmcdt:
	sub   XL,      r21    ; Subtract pitch to start next VRAM line
	sbc   XH,      r20
	sub   YL,      r21
	sbc   YH,      r20
	dec   r24
	breq  vrmcdle
vrmcdl:
	ijmp
vrmcdle:

	; Return (but restore r1 to zero for proper C interfacing)

	movw  YL,      r22
	clr   r1
	ret

vrmcinc:
	; Incrementing copy: top to bottom and (normally) left to right order.

	movw  ZL,      XL      ; ZH:ZL, XH:XL
	brts  .+4              ; X direction: copy to right if positive
	add   XL,      r18
	adc   XH,      r20     ; Target address calculated
	mul   r19,     r21
	brtc  .+4              ; X direction: copy from right if negative
	add   r0,      r18
	adc   r1,      r20     ; Source top left corner offset acquired
	add   ZL,      r0
	adc   ZH,      r1      ; Source address calculated
	sub   r21,     r25     ; Pitch: to increment between rows

	; Copy loop (up to 25 tiles)

	mov   r24,     r23
	movw  r22,     YL
	movw  YL,      ZL
	lsl   r25
	ldi   ZL,      lo8(pm(vrmcit))
	ldi   ZH,      hi8(pm(vrmcit))
	sub   ZL,      r25
	sbc   ZH,      r20
	rjmp  vrmcil
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
	ld    r1,      Y+
	st    X+,      r1
vrmcit:
	add   XL,      r21    ; Add pitch to start next VRAM line
	adc   XH,      r20
	add   YL,      r21
	adc   YH,      r20
	dec   r24
	breq  vrmcile
vrmcil:
	ijmp
vrmcile:

	; Return (but restore r1 to zero for proper C interfacing)

	movw  YL,      r22
	clr   r1
	ret



;
; void M74_VramFillCol(unsigned char y, unsigned int src, unsigned char incr);
;
; Fills a column of VRAM from the given source. Takes as many bytes as tall
; the VRAM is. The source is incremented by incr after each copy. If the
; source is larger or equal to 0x1000, it is treated as a ROM pointer,
; otherwise a RAM pointer.
;
;     r24: y
; r23:r22: src
;     r20: incr
; Clobbered registers:
; r1 (set zero), r22, r23, r24, X, Z
;
.section .text.M74_VramFillCol
M74_VramFillCol:

	; Source into a pointer reg (Z)

	movw  ZL,      r22     ; ZH:ZL, r23:r22

	; Load VRAM layout

	ldi   XL,      lo8(M74_VRAM_OFF)
	ldi   XH,      hi8(M74_VRAM_OFF)
	ldi   r23,     M74_VRAM_H
	ldi   r22,     M74_VRAM_P
	clr   r1               ; Zero

	; Calculate start offset

	add   XL,      r24
	adc   XH,      r1

	; Determine ROM / RAM

	cpi   ZH,      0x10
	brcs  vrfcram          ; < 0x1000: RAM source

	; ROM source copy loop

vrfcrom:
	lpm   r24,     Z
	st    X,       r24
	add   ZL,      r20
	adc   ZH,      r1
	add   XL,      r22
	adc   XH,      r1
	dec   r23
	brne  vrfcrom
	ret

	; RAM source copy loop

vrfcram:
	ld    r24,     Z
	st    X,       r24
	add   ZL,      r20
	adc   ZH,      r1
	add   XL,      r22
	adc   XH,      r1
	dec   r23
	brne  vrfcram
	ret



;
; void M74_VramFillRow(unsigned char x, unsigned int src, unsigned char incr);
;
; Fills a row of VRAM from the given source. Takes as many bytes as wide the
; VRAM is (using the width, not the pitch for this). The source is incremented
; by incr after each copy. If the source is larger or equal to 0x1000, it is
; treated as a ROM pointer, otherwise a RAM pointer.
;
;     r24: x
; r23:r22: src
;     r20: incr
; Clobbered registers:
; r0, r1 (set zero), r23, r24, X, Z
;
.section .text.M74_VramFillRow
M74_VramFillRow:

	; Source into a pointer reg (Z)

	movw  ZL,      r22     ; ZH:ZL, r23:r22

	; Load VRAM layout

	ldi   XL,      M74_VRAM_P
	mov   r0,      XL
	ldi   XL,      lo8(M74_VRAM_OFF)
	ldi   XH,      hi8(M74_VRAM_OFF)
	ldi   r23,     M74_VRAM_H

	; Calculate start offset

	mul   r0,      r24
	add   XL,      r0
	adc   XH,      r1
	clr   r1               ; Zero

	; Determine ROM / RAM

	cpi   ZH,      0x10
	brcs  vrfcram          ; < 0x1000: RAM source

	; ROM source copy loop

vrfrrom:
	lpm   r24,     Z
	st    X+,      r24
	add   ZL,      r20
	adc   ZH,      r1
	dec   r23
	brne  vrfrrom
	ret

	; RAM source copy loop

vrfrram:
	ld    r24,     Z
	st    X+,      r24
	add   ZL,      r20
	adc   ZH,      r1
	dec   r23
	brne  vrfrram
	ret



;
; void M74_VramFill(unsigned int src, unsigned char pitch);
;
; Fills the VRAM from the given source. Behaves like calling M74_VramFillRow
; for every row with an increment of one, between rows incrementing the source
; pointer from its value on the beginning of the row by pitch.
;
; r25:r24: src
;     r22: incr
; Clobbered registers:
; r0, r1 (set zero), r22, r23, r24, r25, X, Z
;
.section .text.M74_VramFill
M74_VramFill:

	; Source into a pointer reg (Z)

	movw  ZL,      r24     ; ZH:ZL, r25:r24

	; Load VRAM layout

	ldi   XL,      lo8(M74_VRAM_OFF)
	ldi   XH,      hi8(M74_VRAM_OFF)
	ldi   r25,     M74_VRAM_W
	ldi   r23,     M74_VRAM_H
	ldi   r24,     M74_VRAM_P

	; Calculate remaining increments after row outputs

	sub   r24,     r25
	sub   r22,     r25

	; Determine ROM / RAM

	cpi   ZH,      0x10
	brcs  vrfram           ; < 0x1000: RAM source

	; ROM source copy loop

vrfrom1:
	mov   r1,      r25
vrfrom0:
	lpm   r0,      Z+
	st    X+,      r0
	dec   r1
	brne  vrfrom0
	add   XL,      r24
	adc   XH,      r1      ; r1 became zero, so OK for adc
	add   ZL,      r22
	adc   ZH,      r1
	dec   r23
	brne  vrfrom1
	ret

	; RAM source copy loop

vrfram1:
	mov   r1,      r25
vrfram0:
	ld    r0,      Z+
	st    X+,      r0
	dec   r1
	brne  vrfram0
	add   XL,      r24
	adc   XH,      r1      ; r1 became zero, so OK for adc
	add   ZL,      r22
	adc   ZH,      r1
	dec   r23
	brne  vrfram1
	ret
