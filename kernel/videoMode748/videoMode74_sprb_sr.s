;
; Uzebox Kernel - Video Mode 748 sprite blitter (SPI RAM source)
; Copyright (C) 2015 - 2018 Sandor Zsuga (Jubatian)
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
; Note: No section specification here! (.section .text)
; This is because this component belongs to the sprite blitter
; (M74_BlitSprite), sitting in its section.
;



;
; Blits a sprite onto a 8x8 4bpp RAM tile
;
; Outputs the appropriate fraction of a sprite on a RAM tile. The sprite has
; fixed 8x8 pixel layout, 4 bytes per line, 32 bytes total, high nybble first
; for pixels. Color index 0 is transparent.
;
; Worst case should be around 900 cycles. For rendering a complete 8x8 sprite
; (onto 4 RAM tiles) it should take 1400 cycles (worst case). For masked
; sprites, around 1100 cycles, for complete sprite, around 1800 cycles (if all
; sprite pixels are rendered, that is, the mask has no effect).
;
; r25:r24: Source 8x8 sprite start address
;       Y: Target RAM tile address
;     r23: Y location on tile (2's complement; 0xF9 - 0x07)
;     r22: X location on tile (2's complement; 0xF9 - 0x07)
;     r16: Flags
;          bit0: If set, flip horizontally
;          bit1: If set, sprite source is in high bank of SPI RAM
;          bit2: If set, flip vertically
;          bit4: If set, mask is used
;          bit5: If set, mask source is RAM
;     r11: Recolor table select (used only if there is any)
;     r1:  Zero
; r15:r14: Mask source offset (8 bytes). Only used if r16 bit4 is set
; Clobbered registers:
; r0, r14, r15, r17, r18, r19, r20, r21, r22, r23, XL, XH, YL, YH, ZL, ZH, T
;
m74_blitspritept:

	; Prepare for SPI RAM reading

	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   XL,      0x03    ; Read from SPI RAM
	out   SR_DR,   XL      ; Send command

	; Save a few registers to have something to work with

	push  r12              ; Will be used for loop counter
	push  r6
	push  r7
	push  r8
	push  r9               ; Will be used for loading from SPI RAM

	; SPI RAM address, highest byte: bank selection

	rjmp  .
	rjmp  .
	mov   XL,      r16
	lsr   XL
	andi  XL,      1
	out   SR_DR,   XL

	; Calculate source start offset

	movw  r6,      r24     ; Source offset (in SPI RAM)
	sbrs  r16,     2
	rjmp  spbs0            ; No vertical flipping
	sbrc  r23,     7
	rjmp  spbs2            ; Y loc. negative: Source address OK
	mov   r21,     r23     ; Number of lines to adjust source
	rjmp  spbs1
spbs0:
	sbrs  r23,     7
	rjmp  spbs3            ; Y loc. positive: Source address OK
	mov   r21,     r23
	neg   r21              ; Number of lines to adjust source
spbs1:
	lsl   r21
	lsl   r21
	add   r6,      r21
	adc   r7,      r1
	rjmp  spbs4            ; (14)
spbs2:
	nop
spbs3:
	nop
	rjmp  .
	rjmp  .
	rjmp  .                ; (14) Padding to even paths for SPI RAM cycles
spbs4:

	; Send source start offset to the SPI RAM

	rjmp  .
	nop
	out   SR_DR,   r7
	rcall splw17
	out   SR_DR,   r6

	; Calculate no. of lines to generate

	ldi   r20,     8       ; Normally 8 lines are generated
	mov   r12,     r20
	sbrs  r23,     7
	sub   r12,     r23     ; Positive Y location: subtract from line count
	sbrc  r23,     7
	add   r12,     r23     ; Negative Y location: add to line count

	; Send dummy byte to SPI RAM (to begin fetching first data byte)

	rcall splw11
	out   SR_DR,   r6

	; Calculate destination & mask start offsets

	sbrs  r23,     7
	rjmp  spbd0            ; Y location positive (moving down)
	rjmp  .                ; Y location negative: do nothing
	rjmp  .
	rjmp  spbd1            ; ( 8)
spbd0:
#if (M74_MSK_ENABLE != 0)
	add   r14,     r23     ; Add to mask source
#else
	nop
#endif
	add   YL,      r23     ; Add to destination location
	add   YL,      r23
	add   YL,      r23
	add   YL,      r23     ; ( 8) Destination is 4 bytes / row
spbd1:

	; Fetch first row data byte 0 from SPI RAM

	rcall splw8
	in    r6,      SR_DR
	out   SR_DR,   r6

	; Adjust for vertical flipping

	sbrc  r16,     2
	rjmp  spbd2            ; Vertical flipping present
	ldi   XL,      0x04    ; Add to destination (Offset within RAM tile)
#if (M74_MSK_ENABLE != 0)
	ldi   XH,      0x01    ; Add to mask (if any)
	nop
#else
	rjmp  .
#endif
	rjmp  .
	rjmp  .
	rjmp  spbd3            ; (11)
spbd2:
	mov   r21,     r12     ; Get no. of lines to draw - 1
	dec   r21
	ldi   XL,      0xFC    ; Add to destination (Offset within RAM tile)
#if (M74_MSK_ENABLE != 0)
	ldi   XH,      0xFF    ; Add to mask (if any)
	add   r14,     r21     ; First mask line when flipped
#else
	rjmp  .
#endif
	lsl   r21
	lsl   r21
	add   YL,      r21     ; (11) First dest. line when flipped
spbd3:

	; Fetch first row data byte 1 from SPI RAM

	rjmp  .
	rjmp  .
	nop
	in    r7,      SR_DR
	out   SR_DR,   r7

	; Calculate jump target by X alignment into r1:r0

	subi  r22,     0xF9    ; Add 7; 0xF9 - 0x07 becomes 0x00 - 0x0E
	ldi   r20,     lo8(pm(spljta))
	ldi   r21,     hi8(pm(spljta))
	sbrc  r16,     0       ; Flipped?
	subi  r22,     0xF1    ; Cheat: Add 15 to reach flipped jump table (spljtaf)
	add   r20,     r22
	adc   r21,     r1
	movw  r0,      r20     ; ( 8) From now r1 is not zero

	; Fetch first row data byte 2 from SPI RAM

	rcall splw8
	in    r8,      SR_DR
	out   SR_DR,   r8

	; Render the sprite part (two separate loops: one with masking and
	; one without)

#if (M74_MSK_ENABLE != 0)
	sbrc  r16,     4       ; Has mask?
	rjmp  spbml            ; Enter render loop with mask
	rjmp  .
	rjmp  .
	rjmp  .
#else
	rcall splw8
#endif
	clr   r17              ; Use zero for mask
	rjmp  spbl             ; No mask, enter maskless render loop
spblret:
	dec   r12
	breq  spbex
	add   YL,      XL      ; Destination adjustment
spbl:
	movw  ZL,      r0      ; Load jump target
	ijmp
#if (M74_MSK_ENABLE != 0)
spbmlret:
	dec   r12
	breq  spbex
	add   YL,      XL      ; Destination adjustment
spbml:
	movw  ZL,      r14     ; Load mask offset
	sbrc  r16,     5
	rjmp  .+10
	lpm   r17,     Z       ; ROM mask source
	add   ZL,      XH      ; Mask adjustment
	movw  r14,     ZL      ; Save mask offset
	movw  ZL,      r0      ; Load jump target
	ijmp
	ld    r17,     Z       ; RAM mask source
	add   ZL,      XH      ; Mask adjustment
	movw  r14,     ZL      ; Save mask offset
	movw  ZL,      r0      ; Load jump target
	ijmp
#endif

	; Done, clean up and return (at this point a last SPI transmission is
	; still in progress, but will finish during the pops).

spbex:
	pop   r9
	pop   r8
	pop   r7
	pop   r6
	pop   r12
	clr   r1
	sbi   SR_PORT, SR_PIN  ; Done with SPI RAM
	ret



;
; Blits a single 8px wide sprite line onto a tile
;
; Outputs a single 8 pixels wide sprite line from a source 4bpp buffer onto a
; target 4bpp tile line using color index 0 transparency. Number of pixels
; generated depends on the alignment.
;
; Note that the commented out parts are used directly, the rets are replaced
; to "rjmp  spblret" instructions, thus eliminating call + ret overhead.
; Registers 'r0' and 'r1' are only used for alignment, so they were optimized
; off above to store the proper jump target.
;
; The SPI data (4 bytes) is loaded into r6:r7:r8:r9, last "in" (for r9)
; performed upon entry.
;
; Y:       Destination start address. Preserved.
; r0:      Alignment: 7: Aligned, >7: shift to left, <7: shift to right
; r1:      Zero
; T:       If set, source is in RAM
; r16:     bit0: If set, flip horizontally
; r17:     Mask: set bits inhibit sprite pixel output.
; Clobbered registers:
; r17 (bits only cleared), r18, r19, r20, r21, r22, r23, ZL, ZH
;
m74_spr_blitspriteline:
;
;	; Pre-clear sprite data buffer
;
;	clr   r20
;	clr   r21
;	movw  r22,     r20     ; r23:r22 cleared
;
;	; Branch off by alignment
;
;	sbrs  r16,     0       ; Flipped?
;	rjmp  .+10             ; No flipping
;	ldi   ZL,      lo8(pm(spljtaf))
;	ldi   ZH,      hi8(pm(spljtaf))
;	add   ZL,      r0
;	adc   ZH,      r1
;	ijmp
;	ldi   ZL,      lo8(pm(spljta))
;	ldi   ZH,      hi8(pm(spljta))
;	add   ZL,      r0
;	adc   ZH,      r1
;	ijmp
spljta:
	rjmp  splr7            ; S0000000
	rjmp  splr6            ; SS000000
	rjmp  splr5            ; SSS00000
	rjmp  splr4            ; SSSS0000
	rjmp  splr3            ; SSSSS000
	rjmp  splr2            ; SSSSSS00
	rjmp  splr1            ; SSSSSSS0
	rjmp  spla0            ; SSSSSSSS
	rjmp  spll1            ; 0SSSSSSS
	rjmp  spll2            ; 00SSSSSS
	rjmp  spll3            ; 000SSSSS
	rjmp  spll4            ; 0000SSSS
	rjmp  spll5            ; 00000SSS
	rjmp  spll6            ; 000000SS
	rjmp  spll7            ; 0000000S
spljtaf:
	rjmp  splr7f           ; S0000000
	rjmp  splr6f           ; SS000000
	rjmp  splr5f           ; SSS00000
	rjmp  splr4f           ; SSSS0000
	rjmp  splr3f           ; SSSSS000
	rjmp  splr2f           ; SSSSSS00
	rjmp  splr1f           ; SSSSSSS0
	rjmp  spla0f           ; SSSSSSSS
	rjmp  spll1f           ; 0SSSSSSS
	rjmp  spll2f           ; 00SSSSSS
	rjmp  spll3f           ; 000SSSSS
	rjmp  spll4f           ; 0000SSSS
	rjmp  spll5f           ; 00000SSS
	rjmp  spll6f           ; 000000SS
	rjmp  spll7f           ; 0000000S



	; S0000000

splr7:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   ZL,      r9
	rjmp  splr7c
splr7f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   ZL,      r6
	swap  ZL
	nop
splr7c:
	andi  ZL,      0x0F
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZL,      r11
	lpm   r20,     Z
	swap  r20
	sbrc  r17,     7       ; Process mask
	andi  r20,     0x0F
	rjmp  .
	rjmp  .
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
	rcall splw16
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
	rjmp  .
	rjmp  .
	rjmp  splpxre

	; SS000000

splr6:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r20,     r9
	rjmp  splr6c
splr6f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r20,     r6
	swap  r20
	nop
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
	nop
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
	lpm   r20,     Z
	or    r20,     r19
	sbrc  r17,     7       ; Process mask
	andi  r20,     0x0F
	sbrc  r17,     6
	andi  r20,     0xF0
	rcall splw8
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
	rjmp  .
	rjmp  .
	rjmp  splpxre

	; SSS00000

splr5:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r20,     r8
	mov   r21,     r9
	rjmp  splr5c
splr5f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r21,     r6
	mov   r20,     r7
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
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
	lpm   r19,     Z
	or    r20,     r19
	mov   ZL,      r21
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	swap  r21
	clr   r22
	clr   r23
	nop
	rjmp  .
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
	rjmp  .
	rjmp  .
	nop
	rjmp  splr5me          ; Process mask

	; SSSS0000

splr4:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r20,     r8
	mov   r21,     r9
	rjmp  splr4c
splr4f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r21,     r6
	mov   r20,     r7
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
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
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
	nop
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
	lpm   r21,     Z
	or    r21,     r19
	clr   r22
	clr   r23
	rjmp  splr4me          ; Process mask

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

splr3f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r22,     r6
	mov   r21,     r7
	mov   r20,     r8
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
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
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
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
	lpm   r19,     Z
	or    r21,     r19
	mov   ZL,      r22
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	swap  r22
	clr   r23
	andi  r17,     0xF8    ; Process mask
	brne  splr3me
	rjmp  splpxe8
splr3:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r20,     r7
	mov   r21,     r8
	mov   r22,     r9
	rjmp  splr3c

	; SSSSSS00

splr2:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r20,     r7
	mov   r21,     r8
	mov   r22,     r9
	rjmp  splr2c
splr2f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r22,     r6
	mov   r21,     r7
	mov   r20,     r8
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
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
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
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
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
	clr   r23
	andi  r17,     0xFC    ; Process mask
	brne  splr2me
	rjmp  splpxe8

	; SSSSSSS0

splr1f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r23,     r6
	mov   r22,     r7
	mov   r21,     r8
	mov   r20,     r9
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
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
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
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
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

splr1:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r20,     r6
	mov   r21,     r7
	mov   r22,     r8
	mov   r23,     r9
	rjmp  splr1c

	; SSSSSSSS

spla0:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r20,     r6
	mov   r21,     r7
	mov   r22,     r8
	mov   r23,     r9
	rjmp  spla0c
spla0f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r23,     r6
	mov   r22,     r7
	mov   r21,     r8
	mov   r20,     r9
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
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
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
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
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

spll1:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r20,     r6
	mov   r21,     r7
	mov   r22,     r8
	mov   r23,     r9
	rjmp  spll1c
spll1f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r23,     r6
	mov   r22,     r7
	mov   r21,     r8
	mov   r20,     r9
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
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
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
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
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

spll2f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r23,     r7
	mov   r22,     r8
	mov   r21,     r9
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
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
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
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
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
	clr   r20
	andi  r17,     0x3F    ; Process mask
	brne  spll2me
	rjmp  splpxe8
spll2:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r21,     r6
	mov   r22,     r7
	mov   r23,     r8
	rjmp  spll2c

	; 000SSSSS

spll3:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r21,     r6
	mov   r22,     r7
	mov   r23,     r8
	nop
	rjmp  spll3c
spll3f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r23,     r7
	mov   r22,     r8
	mov   r21,     r9
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
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
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
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
	lpm   r19,     Z
	swap  r19
	or    r22,     r19
	mov   ZL,      r21
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r21,     Z
	clr   r20
	andi  r17,     0x1F    ; Process mask
	brne  spll3me
	rjmp  splpxe8

	; 0000SSSS

spll4:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r22,     r6
	mov   r23,     r7
	rjmp  spll4c
spll4f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r23,     r8
	mov   r22,     r9
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
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
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
	nop
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
	lpm   r23,     Z
	or    r23,     r19
	clr   r20
	clr   r21
	rjmp  spll4me          ; Process mask

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
	rjmp  splpxe8

	; 00000SSS

spll5:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r22,     r6
	mov   r23,     r7
	rjmp  spll5c
spll5f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r23,     r8
	mov   r22,     r9
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
	nop
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
	lpm   r19,     Z
	swap  r19
	or    r23,     r19
	mov   ZL,      r22
	swap  ZL
	andi  ZL,      0x0F
	add   ZL,      r11
	lpm   r22,     Z
	clr   r20
	clr   r21
	rjmp  .
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
	rjmp  .
	rjmp  .
	nop
	rjmp  spll5me          ; Process mask

	; 000000SS

spll6:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r23,     r6
	rjmp  spll6c
spll6f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   r23,     r9
	swap  r23
	nop
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
	nop
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
	lpm   r23,     Z
	or    r23,     r19
	sbrc  r17,     1       ; Process mask
	andi  r23,     0x0F
	sbrc  r17,     0
	andi  r23,     0xF0
	rcall splw8
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
	rjmp  .
	rjmp  .
	rjmp  splpxle

	; 0000000S

spll7:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   ZL,      r6
	swap  ZL
	rjmp  spll7c
spll7f:
	in    r9,      SR_DR   ; SPI, byte 3
	out   SR_DR,   r9      ; SPI, byte 3
	mov   ZL,      r9
	nop
	rjmp  .
spll7c:
	andi  ZL,      0x0F
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZL,      r11
	lpm   r23,     Z
	sbrc  r17,     0       ; Process mask
	andi  r23,     0xF0
	rjmp  .
	rjmp  .
	in    r6,      SR_DR   ; SPI, byte 0
	out   SR_DR,   r6      ; SPI, byte 0
	rcall splw16
	in    r7,      SR_DR   ; SPI, byte 1
	out   SR_DR,   r7      ; SPI, byte 1
	rjmp  .
	rjmp  .
	rjmp  splpxle


	; Waits (for SPI timing), to be called with rcall

splw17:
	nop
splw16:
	nop
splw15:
	nop
splw14:
	nop
splw13:
	nop
splw12:
	nop
splw11:
	nop
splw10:
	nop
splw9:
	nop
splw8:
	nop
splw7:
	ret


	; Pixel blitting. The fastest path through this is 11 cycles (without
	; the SPI access, no sprite content). The main entry point is splpxe8
	; (other entry points are not used since they provide minimal boost in
	; the ordinary blitter, here they are omitted to get the 11 cycle
	; minimum for timing SPI accesses).
	;
	; After SPI, byte 2 there are at least 10 cycles, while there are
	; enough cycles outside (in the main loop) to pad for the next SPI
	; byte access.

splpx6x:
	cpi   r21,     0x01
	brcc  splpx4n
splpx4x:
	cpi   r22,     0x01
	brcc  splpx2n
splpx2x:
	cpi   r23,     0x01
	brcc  splpx0n
splpx0x:
#if (M74_MSK_ENABLE != 0)
	sbrs  r16,     4       ; Has mask?
	rjmp  spblret          ; No mask return
	rjmp  spbmlret         ; Has mask return
#else
	rjmp  spblret          ; No mask return
#endif
splpxe8:
	cpi   r20,     0x01
	in    r8,      SR_DR   ; SPI, byte 2
	out   SR_DR,   r8      ; SPI, byte 2
splpx6n:
	brcs  splpx6x
splpx6h:
	brhs  splpx6l
	cpi   r20,     0x10
	brcc  .+6
	ldd   r18,     Y + 0
	andi  r18,     0xF0
	or    r20,     r18
	std   Y + 0,   r20
splpxe6:
	cpi   r21,     0x01
	brcs  splpx4x
splpx4n:
	brhs  splpx4l
splpx4h:
	cpi   r21,     0x10
	brcc  .+6
	ldd   r18,     Y + 1
	andi  r18,     0xF0
	or    r21,     r18
	std   Y + 1,   r21
splpxe4:
	cpi   r22,     0x01
	brcs  splpx2x
splpx2n:
	brhs  splpx2l
splpx2h:
	cpi   r22,     0x10
	brcc  .+6
	ldd   r18,     Y + 2
	andi  r18,     0xF0
	or    r22,     r18
	std   Y + 2,   r22
splpxe2:
	cpi   r23,     0x01
	brcs  splpx0x
splpx0n:
	brhs  splpx0l
splpx0h:
	cpi   r23,     0x10
	brcc  .+6
	ldd   r18,     Y + 3
	andi  r18,     0xF0
	or    r23,     r18
	std   Y + 3,   r23
splexit:
#if (M74_MSK_ENABLE != 0)
	sbrs  r16,     4       ; Has mask?
	rjmp  spblret          ; No mask return
	rjmp  spbmlret         ; Has mask return
#else
	rjmp  spblret          ; No mask return
#endif
splpx6l:
	ldd   r18,     Y + 0
	andi  r18,     0x0F
	or    r20,     r18
	std   Y + 0,   r20
	cpi   r21,     0x01
	brcs  splpx4x
	brhc  splpx4h
splpx4l:
	ldd   r18,     Y + 1
	andi  r18,     0x0F
	or    r21,     r18
	std   Y + 1,   r21
	cpi   r22,     0x01
	brcs  splpx2x
	brhc  splpx2h
splpx2l:
	ldd   r18,     Y + 2
	andi  r18,     0x0F
	or    r22,     r18
	std   Y + 2,   r22
	cpi   r23,     0x01
	brcs  splpx0x
	brhc  splpx0h
splpx0l:
	ldd   r18,     Y + 3
	andi  r18,     0x0F
	or    r23,     r18
	std   Y + 3,   r23
#if (M74_MSK_ENABLE != 0)
	sbrs  r16,     4       ; Has mask?
	rjmp  spblret          ; No mask return
	rjmp  spbmlret         ; Has mask return
#else
	rjmp  spblret          ; No mask return
#endif



	; Pixel blitting for the S0000000 and SS000000 cases.
	; The blitter paths are equalized to interleave better with SPI loads,
	; optimizing for best usage on the worst case path. Only r20 might
	; contain valid (nonzero) pixels here.
	;
	; Assuming an rjmp is used to enter, 4 cycles are needed after
	; fetching SPI, byte 1 before the "rjmp splpxre".

splpxre:
	cpi   r20,     0x01
	brcs  splpxrx          ; ( 2 /  3)
	brhs  splpxrl          ; ( 3 /  4)
	cpi   r20,     0x10
	brcc  splpxrf          ; ( 5 /  6)
	nop
	ldd   r18,     Y + 0
	andi  r18,     0xF0
	or    r20,     r18     ; (10)
splpxrw:
	std   Y + 0,   r20     ; (12)
	in    r8,      SR_DR   ; SPI, byte 2
	out   SR_DR,   r8      ; SPI, byte 2
#if (M74_MSK_ENABLE != 0)
	sbrc  r16,     4       ; ( 1 / 2) Has mask?
	rjmp  spbmlret         ; ( 3) Has mask return
	rjmp  splpxee          ; ( 4)
splpxee:
	rjmp  .                ; ( 6)
	rjmp  spblret          ; ( 8) No mask return
#else
	rjmp  splpxee          ; ( 2)
splpxee:
	rjmp  .
	rjmp  .
	rjmp  spblret          ; ( 8) No mask return
#endif
splpxrl:
	ldd   r18,     Y + 0
	andi  r18,     0x0F
	or    r20,     r18
	rjmp  splpxrw          ; (10)
splpxrx:
	nop
	ldd   r20,     Y + 0
splpxrf:
	rjmp  .
	rjmp  splpxrw          ; (10)



	; Pixel blitting for the 000000SS and 0000000S cases.
	; The blitter paths are equalized to interleave better with SPI loads,
	; optimizing for best usage on the worst case path. Only r23 might
	; contain valid (nonzero) pixels here.
	;
	; Assuming an rjmp is used to enter, 4 cycles are needed after
	; fetching SPI, byte 1 before the "rjmp splpxle".

splpxle:
	cpi   r23,     0x01
	brcs  splpxlx          ; ( 2 /  3)
	brhs  splpxll          ; ( 3 /  4)
	cpi   r23,     0x10
	brcc  splpxlf          ; ( 5 /  6)
	nop
	ldd   r18,     Y + 3
	andi  r18,     0xF0
	or    r23,     r18     ; (10)
splpxlw:
	std   Y + 3,   r23     ; (12)
	in    r8,      SR_DR   ; SPI, byte 2
	out   SR_DR,   r8      ; SPI, byte 2
#if (M74_MSK_ENABLE != 0)
	sbrc  r16,     4       ; ( 1 / 2) Has mask?
	rjmp  spbmlret         ; ( 3) Has mask return
	rjmp  splpxee          ; ( 4)
#else
	rjmp  splpxee          ; ( 2)
#endif
splpxll:
	ldd   r18,     Y + 3
	andi  r18,     0x0F
	or    r23,     r18
	rjmp  splpxlw          ; (10)
splpxlx:
	nop
	ldd   r23,     Y + 3
splpxlf:
	rjmp  .
	rjmp  splpxlw          ; (10)
