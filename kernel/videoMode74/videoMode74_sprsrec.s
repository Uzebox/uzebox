;
; Uzebox Kernel - Video Mode 74 sprite blitter
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
; This is part of the sprite blitter. It contains the code of the slow
; recoloring blitter which allows for saving considerable amounts of ROM
; depending on sprite usage style (including overlaid 2bpp or 1bpp sprites).
;


	; Straight (Non-flipped) sprites. Do these here so mask blocks remain
	; within branch range.

splr6:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      3       ; Last byte contains sprite pixel
	brts  .+4
	lpm   r20,     Z+      ; ROM source
	rjmp  splr6c
	ld    r20,     Z+      ; RAM source
	rjmp  splr6c

splr5:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      2
	brts  .+6
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	rjmp  splr5c
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	rjmp  splr5c

splr4:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      2
	brts  .+6
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	rjmp  splr4c
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	rjmp  splr4c

splr3:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      1
	brts  .+8
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	rjmp  splr3c
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	rjmp  splr3c

splr2:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      1
	brts  .+8
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	rjmp  splr2c
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	rjmp  splr2c

splr1:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+10
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	rjmp  splr1c
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  splr1c

spla0:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+10
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	rjmp  spla0c
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  spla0c

spll1:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+10
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	rjmp  spll1c
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  spll1c

spll2:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+8
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	rjmp  spll2c
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  spll2c

spll3:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+8
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	rjmp  spll3c
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  spll3c

spll4:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+6
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	rjmp  spll4c
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  spll4c

spll5:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+6
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	rjmp  spll5c
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  spll5c

spll6:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+4
	lpm   r23,     Z+      ; ROM source
	rjmp  spll6c
	ld    r23,     Z+      ; RAM source
	rjmp  spll6c



	; S0000000

splr7f:
	sbrc  r17,     7
	rjmp  splexit          ; Masked: just bail out
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+4
	ld    ZL,      Z       ; RAM source
	rjmp  .+2
	lpm   ZL,      Z       ; ROM source
	swap  ZL
	andi  ZL,      0x0F
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZL,      r11
	lpm   r20,     Z
	swap  r20
	rjmp  splpxe8

splr7:
	sbrc  r17,     7
	rjmp  splexit          ; Masked: just bail out
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      3       ; Last byte contains sprite pixel
	brtc  .+4
	ld    ZL,      Z       ; RAM source
	rjmp  .+2
	lpm   ZL,      Z       ; ROM source
	andi  ZL,      0x0F
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZL,      r11
	lpm   r20,     Z
	swap  r20
	rjmp  splpxe8

	; SS000000

	ld    r20,     Z+      ; RAM source
	rjmp  splr6s
splr6f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .-6
	lpm   r20,     Z+      ; ROM source
splr6s:
	swap  r20
splr6c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r20
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r20
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r20,     Z
	or    r20,     r19
	sbrc  r17,     7       ; Process mask
	andi  r20,     0x0F
	sbrc  r17,     6
	andi  r20,     0xF0
	rjmp  splpxe8

	; SSS00000

	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  splr5s
splr5f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .-8
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
splr5s:
	swap  r20
	swap  r21
splr5c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r20
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r20,     Z
	swap  r20
	mov   ZL,      r21
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	or    r20,     r19
	mov   ZL,      r21
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	swap  r21
	andi  r17,     0xE0    ; Process mask
	brne  splr5me
	rjmp  splpxe8

	; SSSS0000

	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  splr4s
splr4f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .-8
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
splr4s:
	swap  r20
	swap  r21
splr4c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r20
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r20
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r20,     Z
	or    r20,     r19
	mov   ZL,      r21
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r21
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	or    r21,     r19
	andi  r17,     0xF0    ; Process mask
	brne  splr4me
	rjmp  splpxe8

	; Masking block

splr3me:
	sbrc  r17,     3
	andi  r22,     0x0F
splr4me:
	sbrc  r17,     4
	andi  r21,     0xF0
splr5me:
	sbrc  r17,     7
	andi  r20,     0x0F
	sbrc  r17,     6
	andi  r20,     0xF0
	sbrc  r17,     5
	andi  r21,     0x0F
	rjmp  splpxe8

	; SSSSS000

	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  splr3s
splr3f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .-10
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
splr3s:
	swap  r20
	swap  r21
	swap  r22
splr3c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r20
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r20,     Z
	swap  r20
	mov   ZL,      r21
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	or    r20,     r19
	mov   ZL,      r21
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	swap  r21
	mov   ZL,      r22
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	or    r21,     r19
	mov   ZL,      r22
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	swap  r22
	andi  r17,     0xF8    ; Process mask
	brne  splr3me
	rjmp  splpxe8

	; SSSSSS00

	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  splr2s
splr2f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .-10
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
splr2s:
	swap  r20
	swap  r21
	swap  r22
splr2c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r20
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r20
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r20,     Z
	or    r20,     r19
	mov   ZL,      r21
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r21
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	or    r21,     r19
	mov   ZL,      r22
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r22
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	or    r22,     r19
	andi  r17,     0xFC    ; Process mask
	brne  splr2me
	rjmp  splpxe8

	; SSSSSSS0

	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  splr1s
splr1f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .-12
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
splr1s:
	swap  r20
	swap  r21
	swap  r22
	swap  r23
splr1c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r20
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r20,     Z
	swap  r20
	mov   ZL,      r21
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	or    r20,     r19
	mov   ZL,      r21
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	swap  r21
	mov   ZL,      r22
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	or    r21,     r19
	mov   ZL,      r22
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	swap  r22
	mov   ZL,      r23
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	or    r22,     r19
	mov   ZL,      r23
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r23,     Z
	swap  r23
	andi  r17,     0xFE    ; Process mask
	brne  splr1me
	rjmp  splpxe8

	; Masking block

splr1me:
	sbrc  r17,     1
	andi  r23,     0x0F
splr2me:
	sbrc  r17,     7
	andi  r20,     0x0F
	sbrc  r17,     6
	andi  r20,     0xF0
	sbrc  r17,     5
	andi  r21,     0x0F
	sbrc  r17,     4
	andi  r21,     0xF0
	sbrc  r17,     3
	andi  r22,     0x0F
	sbrc  r17,     2
	andi  r22,     0xF0
	rjmp  splpxe8

	; SSSSSSSS

	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  spla0s
spla0f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .-12
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
spla0s:
	swap  r20
	swap  r21
	swap  r22
	swap  r23
spla0c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r20
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r20
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r20,     Z
	or    r20,     r19
	mov   ZL,      r21
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r21
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	or    r21,     r19
	mov   ZL,      r22
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r22
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	or    r22,     r19
	mov   ZL,      r23
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r23
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r23,     Z
	or    r23,     r19
	andi  r17,     0xFF    ; Process mask
	brne  spla0me
	rjmp  splpxe8

	; Masking block

spla0me:
	sbrc  r17,     7
	andi  r20,     0x0F
	sbrc  r17,     6
	andi  r20,     0xF0
	sbrc  r17,     5
	andi  r21,     0x0F
	sbrc  r17,     4
	andi  r21,     0xF0
	sbrc  r17,     3
	andi  r22,     0x0F
	sbrc  r17,     2
	andi  r22,     0xF0
	sbrc  r17,     1
	andi  r23,     0x0F
	sbrc  r17,     0
	andi  r23,     0xF0
	rjmp  splpxe8

	; 0SSSSSSS

	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  spll1s
spll1f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .-12
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
spll1s:
	swap  r20
	swap  r21
	swap  r22
	swap  r23
spll1c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r23
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r23,     Z
	mov   ZL,      r22
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	or    r23,     r19
	mov   ZL,      r22
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	mov   ZL,      r21
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	or    r22,     r19
	mov   ZL,      r21
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	mov   ZL,      r20
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	or    r21,     r19
	mov   ZL,      r20
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r20,     Z
	andi  r17,     0x7F    ; Process mask
	brne  spll1me
	rjmp  splpxe8

	; Masking block

spll1me:
	sbrc  r17,     6
	andi  r20,     0xF0
spll2me:
	sbrc  r17,     5
	andi  r21,     0x0F
	sbrc  r17,     4
	andi  r21,     0xF0
	sbrc  r17,     3
	andi  r22,     0x0F
	sbrc  r17,     2
	andi  r22,     0xF0
	sbrc  r17,     1
	andi  r23,     0x0F
	sbrc  r17,     0
	andi  r23,     0xF0
	rjmp  splpxe8

	; 00SSSSSS

	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	rjmp  spll2s
spll2f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      1
	brts  .-12
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
spll2s:
	swap  r21
	swap  r22
	swap  r23
spll2c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r21
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r21
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	or    r21,     r19
	mov   ZL,      r22
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r22
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	or    r22,     r19
	mov   ZL,      r23
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r23
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r23,     Z
	or    r23,     r19
	andi  r17,     0x3F    ; Process mask
	brne  spll2me
	rjmp  splpxe6

	; 000SSSSS

	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	rjmp  spll3s
spll3f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      1
	brts  .-12
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
spll3s:
	swap  r21
	swap  r22
	swap  r23
spll3c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r23
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r23,     Z
	mov   ZL,      r22
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	or    r23,     r19
	mov   ZL,      r22
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	mov   ZL,      r21
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	or    r22,     r19
	mov   ZL,      r21
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	andi  r17,     0x1F    ; Process mask
	brne  spll3me
	rjmp  splpxe6

	; 0000SSSS

	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	rjmp  spll2s
spll4f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      2
	brts  .-10
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
spll4s:
	swap  r22
	swap  r23
spll4c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r22
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r22
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	or    r22,     r19
	mov   ZL,      r23
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r23
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r23,     Z
	or    r23,     r19
	andi  r17,     0x0F    ; Process mask
	brne  spll4me
	rjmp  splpxe4

	; Masking block

spll3me:
	sbrc  r17,     4
	andi  r21,     0xF0
spll4me:
	sbrc  r17,     3
	andi  r22,     0x0F
spll5me:
	sbrc  r17,     2
	andi  r22,     0xF0
	sbrc  r17,     1
	andi  r23,     0x0F
	sbrc  r17,     0
	andi  r23,     0xF0
	rjmp  splpxe6

	; 00000SSS

	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	rjmp  spll5s
spll5f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      2
	brts  .-10
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
spll5s:
	swap  r22
	swap  r23
spll5c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r23
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r23,     Z
	mov   ZL,      r22
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	or    r23,     r19
	mov   ZL,      r22
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	andi  r17,     0x07    ; Process mask
	brne  spll5me
	rjmp  splpxe4

	; 000000SS

	ld    r23,     Z+      ; RAM source
	rjmp  spll6s
spll6f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      3
	brts  .-8
	lpm   r23,     Z+      ; ROM source
spll6s:
	swap  r23
spll6c:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	mov   ZL,      r23
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r19,     Z
	swap  r19
	mov   ZL,      r23
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r23,     Z
	or    r23,     r19
	sbrc  r17,     1       ; Process mask
	andi  r23,     0x0F
	sbrc  r17,     0
	andi  r23,     0xF0
	rjmp  splpxe2

	; 0000000S

spll7f:
	sbrc  r17,     0
	rjmp  splexit          ; Masked: just bail out
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      3       ; Otherwise last byte
	brtc  .+4
	ld    ZL,      Z       ; RAM source
	rjmp  .+2
	lpm   ZL,      Z       ; ROM source
	andi  ZL,      0x0F
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZL,      r11
	lpm   r23,     Z
	rjmp  splpxe2

spll7:
	sbrc  r17,     0
	rjmp  splexit          ; Masked: just bail out
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+4
	ld    ZL,      Z       ; RAM source
	rjmp  .+2
	lpm   ZL,      Z       ; ROM source
	swap  ZL
	andi  ZL,      0x0F
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZL,      r11
	lpm   r23,     Z
	rjmp  splpxe2

