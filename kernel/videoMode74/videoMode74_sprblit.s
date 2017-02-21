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
;          bit1: If set, sprite source is RAM
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

	; Save a few registers to have something to work with

	push  r12              ; Will be used for loop counter
	push  r24
	push  r25              ; Preserve source start address

	; Calculate target offset including mask (which belongs / aligns with
	; the target).

	bst   r23,     7       ; T: Set if Y location negative, clear otherwise
#if (M74_MSK_ENABLE != 0)
	brts  .+4              ; Y location positive (move down?)
	add   r14,     r23
	adc   r15,     r1      ; Set up mask source
#endif
	brts  .+8              ; Y location positive (move down?)
	mov   r0,      r23     ; If positive, calculate offset on target
	lsl   r0               ; Destination increment is 4 bytes / row
	lsl   r0
	add   YL,      r0

	; Calculate source offset and increment (A sprite line is 4 bytes)

	sbrs  r16,     2
	rjmp  spbs0            ; No vertical flipping
	ldi   XL,      0xFC
	ldi   XH,      0xFF    ; Source decrements after each line
	subi  r24,     0xE4
	sbci  r25,     0xFF    ; Add 28, to start at the last line
	brtc  spbs2            ; If Y location is positive (moving down), then OK
	mov   r21,     r23
	rjmp  spbs1            ; Subtract lines to skip (Y loc. negative!)
spbs0:
	ldi   XL,      0x04
	ldi   XH,      0x00    ; Source increments after each line
	brtc  spbs2            ; If Y location is positive (moving down), then OK
	mov   r21,     r23
	neg   r21              ; Add lines to skip
spbs1:
	lsl   r21
	lsl   r21
	add   r24,     r21
	adc   r25,     XH      ; Source start address calculated OK
spbs2:

	; Calculate number of lines to output

	ldi   r20,     8
	mov   r12,     r20     ; Normally 8 lines
	brts  .+4
	sub   r12,     r23     ; Positive Y location: subtract
	brtc  .+2
	add   r12,     r23     ; Negative Y location: add

	; Calculate jump target by X alignment into r1:r0

	subi  r22,     0xF9    ; Add 7; 0xF9 - 0x07 becomes 0x00 - 0x0E
	ldi   r20,     lo8(pm(spljta))
	ldi   r21,     hi8(pm(spljta))
	sbrc  r16,     0       ; Flipped?
	subi  r22,     0xF1    ; Cheat: Add 15 to reach flipped jump table (spljtaf)
	add   r20,     r22
	adc   r21,     r1
	movw  r0,      r20     ; From now r1 is not zero

	; Render the sprite part (two separate loops: one with masking and
	; one without)

	bst   r16,     1       ; T: ROM / RAM sprite source
#if (M74_MSK_ENABLE != 0)
	sbrc  r16,     4       ; Has mask?
	rjmp  spbml            ; Enter render loop with mask
#endif
	clr   r17              ; Use zero for mask
	rjmp  spbl             ; No mask, enter maskless render loop
spblret:
	dec   r12
	breq  spbex
	add   r24,     XL
	adc   r25,     XH      ; Source increment / decrement
	subi  YL,      0xFC    ; Destination increment
spbl:
	clr   r20              ; Pre-clear sprite data buffer
	clr   r21
	movw  r22,     r20     ; r23:r22 cleared
	movw  ZL,      r0      ; Load jump target
	ijmp
#if (M74_MSK_ENABLE != 0)
spbmlret:
	dec   r12
	breq  spbex
	add   r24,     XL
	adc   r25,     XH      ; Source increment / decrement
	subi  YL,      0xFC    ; Destination increment
spbml:
	clr   r20              ; Pre-clear sprite data buffer
	clr   r21
	movw  r22,     r20     ; r23:r22 cleared
	movw  ZL,      r14     ; Load mask offset
	sbrc  r16,     5
	rjmp  .+8
	lpm   r17,     Z+      ; ROM mask source
	movw  r14,     ZL      ; Save mask offset
	movw  ZL,      r0      ; Load jump target
	ijmp
	ld    r17,     Z+      ; RAM mask source
	movw  r14,     ZL      ; Save mask offset
	movw  ZL,      r0      ; Load jump target
	ijmp
#endif

	; Done, clean up and return

spbex:
	pop   r25
	pop   r24
	pop   r12
	clr   r1
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
; r24:r25: Source start address. Preserved.
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

#if (M74_REC_SLOW != 0)

	; Slow recolor, replaces the normal pixel fetch logic here
	; (the endif for the else path is after this, with an M74_REC_SLOW
	; comment)

#include "videoMode74/videoMode74_sprsrec.s"

#else

	; S0000000

splr7f:
	sbrc  r17,     7
	rjmp  splexit          ; Masked: just bail out
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+4
	ld    r20,     Z+      ; RAM source
	rjmp  .+2
	lpm   r20,     Z+      ; ROM source
	andi  r20,     0xF0
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrs  r11,     7       ; Recoloring enabled?
	rjmp  splr7rc
#endif
	rjmp  splpxe8
splr7:
	sbrc  r17,     7
	rjmp  splexit          ; Masked: just bail out
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      3       ; Last byte contains sprite pixel
	brtc  .+4
	ld    r20,     Z+      ; RAM source
	rjmp  .+2
	lpm   r20,     Z+      ; ROM source
	swap  r20              ; Align since low nybble contains pixel
	andi  r20,     0xF0
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splpxe8
splr7rc:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r20
	lpm   r20,     Z
#endif
	rjmp  splpxe8

	; SS000000

splr6f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+4
	ld    r20,     Z+      ; RAM source
	rjmp  .+2
	lpm   r20,     Z+      ; ROM source
	swap  r20
splr6m:
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splr6re
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r20
	lpm   r20,     Z
splr6re:
#endif
	sbrc  r17,     7       ; Process mask
	andi  r20,     0x0F
	sbrc  r17,     6
	andi  r20,     0xF0
	rjmp  splpxe8
splr6:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      3       ; Last byte contains sprite pixel
	brts  .+4
	lpm   r20,     Z+      ; ROM source
	rjmp  splr6m
	ld    r20,     Z+      ; RAM source
	rjmp  splr6m

	; SSS00000

splr5f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+6
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  .+4
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
	movw  ZL,      r20
	andi  r21,     0xF0
	andi  r20,     0xF0
	andi  ZH,      0x0F
	or    r20,     ZH
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splr5re
	rjmp  splr5rc
#else
	andi  r17,     0xE0    ; Process mask
	brne  splr5me
	rjmp  splpxe8
#endif
splr5:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      2
	brtc  .+6
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	rjmp  .+4
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lsl   r21
	rol   r20
	lsl   r21
	rol   r20
	lsl   r21
	rol   r20
	lsl   r21
	rol   r20
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splr5re
splr5rc:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r20
	lpm   r20,     Z
	mov   ZL,      r21
	lpm   r21,     Z
splr5re:
#endif
	andi  r17,     0xE0    ; Process mask
	brne  splr5me
	rjmp  splpxe8

	; SSSS0000

splr4f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+6
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  .+4
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
	swap  r20
	swap  r21
splr4m:
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splr4re
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r20
	lpm   r20,     Z
	mov   ZL,      r21
	lpm   r21,     Z
splr4re:
#endif
	andi  r17,     0xF0    ; Process mask
	brne  splr4me
	rjmp  splpxe8
splr4:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      2
	brts  .+6
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	rjmp  splr4m
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	rjmp  splr4m

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
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+8
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  .+6
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
	movw  r18,     r22
	movw  ZL,      r20
	andi  r22,     0xF0
	andi  r21,     0xF0
	andi  r20,     0xF0
	andi  r18,     0x0F
	andi  ZH,      0x0F
	or    r21,     r18
	or    r20,     ZH
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splr3re
splr3rc:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r20
	lpm   r20,     Z
	mov   ZL,      r21
	lpm   r21,     Z
	mov   ZL,      r22
	lpm   r22,     Z
splr3re:
#endif
	andi  r17,     0xF8    ; Process mask
	brne  splr3me
	rjmp  splpxe8
splr3:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      1
	brtc  .+8
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	rjmp  .+6
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lsl   r22
	rol   r21
	rol   r20
	lsl   r22
	rol   r21
	rol   r20
	lsl   r22
	rol   r21
	rol   r20
	lsl   r22
	rol   r21
	rol   r20
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splr3re
	rjmp  splr3rc
#else
	andi  r17,     0xF8    ; Process mask
	brne  splr3me
	rjmp  splpxe8
#endif

	; SSSSSS00

splr2f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+8
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  .+6
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
	swap  r20
	swap  r21
	swap  r22
splr2m:
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splr2re
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r20
	lpm   r20,     Z
	mov   ZL,      r21
	lpm   r21,     Z
	mov   ZL,      r22
	lpm   r22,     Z
splr2re:
#endif
	andi  r17,     0xFC    ; Process mask
	brne  splr2me
	rjmp  splpxe8
splr2:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      1
	brts  .+8
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	rjmp  splr2m
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	rjmp  splr2m

	; Masking block

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

	; SSSSSSS0

splr1f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+10
	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  .+8
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
	movw  r18,     r22
	movw  ZL,      r20
	andi  r23,     0xF0
	andi  r22,     0xF0
	andi  r21,     0xF0
	andi  r20,     0xF0
	andi  r19,     0x0F
	andi  r18,     0x0F
	andi  ZH,      0x0F
	or    r22,     r19
	or    r21,     r18
	or    r20,     ZH
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splr1re
	rjmp  splr1rc
#else
	andi  r17,     0xFE    ; Process mask
	brne  splr1me
	rjmp  splpxe8
#endif
splr1:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+10
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  .+8
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	lsl   r23
	rol   r22
	rol   r21
	rol   r20
	lsl   r23
	rol   r22
	rol   r21
	rol   r20
	lsl   r23
	rol   r22
	rol   r21
	rol   r20
	lsl   r23
	rol   r22
	rol   r21
	rol   r20
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splr1re
splr1rc:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r20
	lpm   r20,     Z
	mov   ZL,      r21
	lpm   r21,     Z
	mov   ZL,      r22
	lpm   r22,     Z
	mov   ZL,      r23
	lpm   r23,     Z
splr1re:
#endif
	andi  r17,     0xFE    ; Process mask
	brne  splr1me
	rjmp  splpxe8

	; Masking block

spla0me:
	sbrc  r17,     0
	andi  r23,     0xF0
splr1me:
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
	rjmp  splpxe8

	; SSSSSSSS

spla0f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+10
	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  .+8
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
	swap  r20
	swap  r21
	swap  r22
	swap  r23
spla0m:
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  spla0re
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r20
	lpm   r20,     Z
	mov   ZL,      r21
	lpm   r21,     Z
	mov   ZL,      r22
	lpm   r22,     Z
	mov   ZL,      r23
	lpm   r23,     Z
spla0re:
#endif
	andi  r17,     0xFF    ; Process mask
	brne  spla0me
	rjmp  splpxe8
spla0:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+10
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	rjmp  spla0m
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  spla0m

	; 0SSSSSSS

spll1f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+10
	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r20,     Z+      ; RAM source
	rjmp  .+8
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r20,     Z+      ; ROM source
	movw  r18,     r22
	movw  ZL,      r20
	andi  r23,     0x0F
	andi  r22,     0x0F
	andi  r21,     0x0F
	andi  r20,     0x0F
	andi  r18,     0xF0
	andi  ZH,      0xF0
	andi  ZL,      0xF0
	or    r23,     r18
	or    r22,     ZH
	or    r21,     ZL
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  spll1re
	rjmp  spll1rc
#else
	andi  r17,     0x7F    ; Process mask
	brne  spll1me
	rjmp  splpxe8
#endif
spll1:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+10
	ld    r20,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  .+8
	lpm   r20,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	lsr   r20
	ror   r21
	ror   r22
	ror   r23
	lsr   r20
	ror   r21
	ror   r22
	ror   r23
	lsr   r20
	ror   r21
	ror   r22
	ror   r23
	lsr   r20
	ror   r21
	ror   r22
	ror   r23
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  spll1re
spll1rc:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r20
	lpm   r20,     Z
	mov   ZL,      r21
	lpm   r21,     Z
	mov   ZL,      r22
	lpm   r22,     Z
	mov   ZL,      r23
	lpm   r23,     Z
spll1re:
#endif
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
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      1
	brtc  .+8
	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	rjmp  .+6
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	swap  r21
	swap  r22
	swap  r23
spll2m:
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  spll2re
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r21
	lpm   r21,     Z
	mov   ZL,      r22
	lpm   r22,     Z
	mov   ZL,      r23
	lpm   r23,     Z
spll2re:
#endif
	andi  r17,     0x3F    ; Process mask
	brne  spll2me
	rjmp  splpxe6
spll2:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+8
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	rjmp  spll2m
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  spll2m

	; 000SSSSS

spll3f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      1
	brtc  .+8
	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r21,     Z+      ; RAM source
	rjmp  .+6
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r21,     Z+      ; ROM source
	movw  r18,     r22
	movw  ZL,      r20
	andi  r23,     0x0F
	andi  r22,     0x0F
	andi  r21,     0x0F
	andi  r18,     0xF0
	andi  ZH,      0xF0
	or    r23,     r18
	or    r22,     ZH
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  spll3re
	rjmp  spll3rc
#else
	andi  r17,     0x1F    ; Process mask
	brne  spll3me
	rjmp  splpxe6
#endif
spll3:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+8
	ld    r21,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  .+6
	lpm   r21,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	lsr   r21
	ror   r22
	ror   r23
	lsr   r21
	ror   r22
	ror   r23
	lsr   r21
	ror   r22
	ror   r23
	lsr   r21
	ror   r22
	ror   r23
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  spll3re
spll3rc:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r21
	lpm   r21,     Z
	mov   ZL,      r22
	lpm   r22,     Z
	mov   ZL,      r23
	lpm   r23,     Z
spll3re:
#endif
	andi  r17,     0x1F    ; Process mask
	brne  spll3me
	rjmp  splpxe6

	; 0000SSSS

spll4f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      2
	brtc  .+6
	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	rjmp  .+4
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	swap  r22
	swap  r23
spll4m:
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  spll4re
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r22
	lpm   r22,     Z
	mov   ZL,      r23
	lpm   r23,     Z
spll4re:
#endif
	andi  r17,     0x0F    ; Process mask
	brne  spll4me
	rjmp  splpxe4
spll4:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+6
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	rjmp  spll4m
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  spll4m

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

spll5f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      2
	brtc  .+6
	ld    r23,     Z+      ; RAM source
	ld    r22,     Z+      ; RAM source
	rjmp  .+4
	lpm   r23,     Z+      ; ROM source
	lpm   r22,     Z+      ; ROM source
	movw  r18,     r22
	andi  r23,     0x0F
	andi  r22,     0x0F
	andi  r18,     0xF0
	or    r23,     r18
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  spll5re
spll5rc:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r22
	lpm   r22,     Z
	mov   ZL,      r23
	lpm   r23,     Z
spll5re:
#endif
	andi  r17,     0x07    ; Process mask
	brne  spll5me
	rjmp  splpxe4
spll5:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+6
	ld    r22,     Z+      ; RAM source
	ld    r23,     Z+      ; RAM source
	rjmp  .+4
	lpm   r22,     Z+      ; ROM source
	lpm   r23,     Z+      ; ROM source
	lsr   r22
	ror   r23
	lsr   r22
	ror   r23
	lsr   r22
	ror   r23
	lsr   r22
	ror   r23
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  spll5re
	rjmp  spll5rc
#else
	andi  r17,     0x07    ; Process mask
	brne  spll5me
	rjmp  splpxe4
#endif

	; 000000SS

spll6f:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      3
	brtc  .+4
	ld    r23,     Z+      ; RAM source
	rjmp  .+2
	lpm   r23,     Z+      ; ROM source
	swap  r23
spll6m:
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  spll6re
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r23
	lpm   r23,     Z
spll6re:
#endif
	sbrc  r17,     1       ; Process mask
	andi  r23,     0x0F
	sbrc  r17,     0
	andi  r23,     0xF0
	rjmp  splpxe2
spll6:
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brts  .+4
	lpm   r23,     Z+      ; ROM source
	rjmp  spll6m
	ld    r23,     Z+      ; RAM source
	rjmp  spll6m

	; 0000000S

spll7f:
	sbrc  r17,     0
	rjmp  splexit          ; Masked: just bail out
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	adiw  ZL,      3       ; Otherwise last byte
	brtc  .+4
	ld    r23,     Z+      ; RAM source
	rjmp  .+2
	lpm   r23,     Z+      ; ROM source
	andi  r23,     0x0F
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrc  r11,     7       ; Recoloring enabled?
	rjmp  splpxe2
spll7rc:
	ldi   ZH,      (M74_RECTB_OFF >> 8)
	add   ZH,      r11
	mov   ZL,      r23
	lpm   r23,     Z
#endif
	rjmp  splpxe2
spll7:
	sbrc  r17,     0
	rjmp  splexit          ; Masked: just bail out
	movw  ZL,      r24     ; ZH:ZL, r25:r24
	brtc  .+4
	ld    r23,     Z+      ; RAM source
	rjmp  .+2
	lpm   r23,     Z+      ; ROM source
	swap  r23
	andi  r23,     0x0F
#if ((M74_RECTB_OFF >> 8) != 0)
	sbrs  r11,     7       ; Recoloring enabled?
	rjmp  spll7rc
#endif
	rjmp  splpxe2

	; endif for M74_REC_SLOW
#endif

	; Pixel blitting

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
