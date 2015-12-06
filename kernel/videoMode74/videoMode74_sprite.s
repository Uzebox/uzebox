;
; Uzebox Kernel - Video Mode 74 sprite output
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
; This part manages the output of sprites including the necessary RAM tile
; allocations and tile copies.
;
; It fetches tile sources from the first tile descriptor row (to know where to
; look for tile data to be copied onto the allocated RAM tiles before blitting
; the sprite onto them).
;
; The maximal count of RAM tiles used for sprites may be specified. Sprites
; will take RAM tiles incrementally from 0xC0 until hitting the limit when
; already allocated sprite parts might be dropped to favor more important
; oncoming elements. RAM tiles including and above the limit for sprite
; allocation may be used normally for tile data, the sprite output routine
; will properly handle their contents like any ROM tile.
;
; The description of mask support:
;
; For every 4bpp tile type (4Kb ROM tile maps, half tile maps, 4bpp RAM tiles)
; there is a corresponding mask index list, 128 bytes for the 0x00 - 0x7F
; range, 64 bytes for the other two ranges. The index list selects the mask to
; take from the mask pool. Its values are to be interpreted as follows:
;
; 0x00 - 0xDF: Indexes masks in the ROM mask pool.
; 0xE0 - 0xFD: Indexes masks in the RAM mask pool.
; 0xFE: Zero mask (all sprite pixels are shown).
; 0xFF: Full mask (no sprite pixels visible; no RAM tile allocation happens).
;
; Of course these only apply if the sprite was requested to be blit with mask.
;
; Sprite generation:
;
; The region of RAM tiles used for sprites (0xC0 - limit) also have a
; corresponding region of mask indices in the mask index list for the RAM
; tiles. When allocating a RAM tile, not only the original tile data is
; copied, but also the corresponding mask index. This way overlapping sprites
; would work proper (further sprites also mask against the original tile
; mask).
;
; If the RAM tile bank has no masks, a limited sprite masking may still be
; used, particularly if all sprites are asked to be masked and only mask types
; 0xFE and 0xFF are used, things will work normally. It will also work right
; if all masked sprites are blit first, and only then any non-masked sprite.
;

;
; The layout of the sprite allocation workspace:
;
; Uses byte triplets with the following contents:
; byte0: bit 0-3: RAM pointer high of VRAM where tile index was replaced
;        bit 4-7: Importance of sprite content
; byte1: RAM pointer low of VRAM where tile index was replaced
; byte2: Original tile index at the VRAM location
;




;
; void M74_VramRestore(void);
;
; Restores VRAM and clears sprite engine state.
;
; Should be called after frame generation, before starting working on the
; VRAM. General suggested workflow:
; Restore VRAM => Operate on VRAM (ex.: scrolling) => Sprite output.
;
.global M74_VramRestore

;
; void M74_BlitSprite(unsigned int spo, unsigned char xl, unsigned char yl,
;                     unsigned char flg);
;
; Blits a 8x8 sprite.
;
; Uses the rectangular VRAM as target area, xl and yl specifying locations on
; it by the sprite's lower right corner (so location 0:0 produces no sprite,
; 1:1 would make the lower right corner pixel visible).
;
; The sprite has fixed 8x8 pixel layout, 4 bytes per line, 32 bytes total,
; high nybble first for pixels. Color index 0 is transparent.
;
.global M74_BlitSprite

#if (M74_SPLIST_PTRE != 0)
;
; volatile unsigned int m74_rtlist;
;
; RAM tile allocation workspace pointer. Uses 3 bytes for an entry, so assign
; a location accordingly depending on the desired number of RAM tiles to use
; for sprites. Should only be written after a VRAM restore before any sprites
; were produced.
;
.global m74_rtlist
#endif

;
; volatile unsigned char m74_rtmax;
;
; Maximal number of RAM tiles to use for sprites. RAM tiles are taken
; beginning with tile 0xC0, any RAM tile above the limit is free for any use
; (if they occur on the VRAM, they can be composed with sprites normally).
; Should only be written after a VRAM restore before any sprites were
; produced.
;
.global m74_rtmax

;
; volatile unsigned char m74_rtno;
;
; Pseudo-global, needed for initialization (to zero) only
;
.global m74_rtno



.section .bss

	; Globals

#if (M74_RTLIST_PTRE != 0)
	m74_rtlist:
	m74_rtlist_lo: .byte 1 ; RAM tile allocation workspace pointer, low
	m74_rtlist_hi: .byte 1 ; RAM tile allocation workspace pointer, high
#endif
	m74_rtmax:     .byte 1 ; Maximal number of RAM tiles allowed
	m74_rtno:      .byte 1 ; Number of RAM tiles currently allocated

.section .text



;
; void M74_VramRestore(void);
;
; Restores VRAM and clears sprite engine state.
;
; Should be called after frame generation, before starting working on the
; VRAM. General suggested workflow:
; Restore VRAM => Operate on VRAM (ex.: scrolling) => Sprite output.
;
; Clobbered registers:
; r0, r1 (set zero), XL, XH, ZL, ZH
;
.section .text.M74_VramRestore
M74_VramRestore:
#if (M74_RTLIST_PTRE != 0)
	lds   ZL,      m74_rtlist_lo
	lds   ZH,      m74_rtlist_hi
#else
	ldi   ZL,      lo8(M74_RTLIST_OFF)
	ldi   ZH,      hi8(M74_RTLIST_OFF)
#endif
	lds   XH,      m74_rtno
	cpi   XH,      0
	breq  rlsrr1           ; List is empty
	mov   r1,      XH
rlsrr0:
	ld    XH,      Z+
	andi  XH,      0x0F
	ld    XL,      Z+
	ld    r0,      Z+
	st    X,       r0      ; Memory restored
	dec   r1
	brne  rlsrr0           ; List emptied
	sts   m74_rtno, r1
rlsrr1:
	ret



;
; void M74_BlitSprite(unsigned int spo, unsigned char xl, unsigned char yl,
;                     unsigned char flg);
;
; Blits a 8x8 sprite.
;
; Uses the rectangular VRAM as target area, xl and yl specifying locations on
; it by the sprite's lower right corner (so location 0:0 produces no sprite,
; 1:1 would make the lower right corner pixel visible).
;
; The sprite has fixed 8x8 pixel layout, 4 bytes per line, 32 bytes total,
; high nybble first for pixels. Color index 0 is transparent.
;
; r25:r24: Source 8x8 sprite start address
;     r22: X location (right side)
;     r20: Y location (bottom)
;     r18: Flags
;          bit0: If set, flip horizontally
;          bit1: If set, sprite source is RAM
;          bit2: If set, flip vertically
;          bit4: If set, mask is used
;          bit6-7: Sprite importance (smaller: higher)
; Clobbered registers:
; r0, r1 (set zero), r18, r19, r20, r21, r22, r23, r24, r25, XL, XH, ZL, ZH, T
;
.section .text.M74_BlitSprite
M74_BlitSprite:

	push  r4
	push  r5
	push  r6
	push  r7
	push  r8
	push  r9
	push  r10
	push  r11
	push  r12
	push  r13
	push  r14
	push  r15
	push  r16
	push  r17
	push  YL
	push  YH

	; Load tile descriptor first row which will determine what to use as
	; tile sources.

	mov   r16,     r18     ; Flags will stay in r16
	lds   ZL,      m74_tdesc_lo
	lds   ZH,      m74_tdesc_hi
	lds   r19,     m74_config
	sbrs  r19,     1       ; RAM or ROM descriptors?
	rjmp  .+6
	ld    r17,     Z+
	ld    r18,     Z+
	rjmp  .+4
	lpm   r17,     Z+
	lpm   r18,     Z+
	andi  r16,     0xD7    ; Mask off flag bits not loaded from input
	sbrc  r18,     2
	ori   r16,     0x08    ; RAM tile config to use going into flags
	swap  r18
	andi  r18,     0x30    ; Mask out 0x00 - 0x7F config selector
	andi  r17,     0x07    ; Mask out 0x80 - 0xBF config selector
	or    r17,     r18     ; Stores combined config selectors

	; Load RAM tile maximum and current RAM tile count

	lds   r13,     m74_rtmax
	lds   r12,     m74_rtno

	; Load VRAM properties

#if (M74_VRAM_CONST == 0)
	lds   YL,      v_vram_lo
	lds   YH,      v_vram_hi
	lds   r8,      v_vram_w
	lds   r9,      v_vram_h
	lds   r10,     v_vram_p
#else
	ldi   YL,      lo8(M74_VRAM_OFF)
	ldi   YH,      hi8(M74_VRAM_OFF)
	ldi   ZL,      M74_VRAM_W
	ldi   ZH,      M74_VRAM_H
	ldi   r21,     M74_VRAM_P
	movw  r8,      ZL
	mov   r10,     r21
#endif

	; Save away X:Y locations (to retain them across calls)

	mov   r4,      r22     ; X
	mov   r5,      r20     ; Y
	andi  r22,     0x07
	andi  r20,     0x07
	mov   r6,      r22     ; Location within tile
	mov   r7,      r20
	lsr   r4
	lsr   r4
	lsr   r4               ; Tile location
	lsr   r5
	lsr   r5
	lsr   r5

	; Generate lower right sprite part

	mov   r22,     r6
	mov   r20,     r7
	subi  r22,     8
	subi  r20,     8
	cpi   r22,     0xF8
	breq  bsplre           ; No sprite (off tile)
	cpi   r20,     0xF8
	breq  bsplre           ; No sprite (off tile)
	cp    r4,      r8
	brcc  bsplre           ; No sprite (off to the right or left)
	cp    r5,      r9
	brcc  bsplre           ; No sprite (off to the bottom or top)
	movw  ZL,      YL
	mul   r5,      r10
	add   ZL,      r0
	adc   ZH,      r1
	clr   r1
	add   ZL,      r4
	adc   ZH,      r1
	rcall m74_blitspriteptprep
bsplre:

	; Generate lower left sprite part

	dec   r4
	mov   r22,     r6
	mov   r20,     r7
	subi  r20,     8
	cpi   r20,     0xF8
	breq  bsplle           ; No sprite (off tile)
	cp    r4,      r8
	brcc  bsplle           ; No sprite (off to the right or left)
	cp    r5,      r9
	brcc  bsplle           ; No sprite (off to the bottom or top)
	movw  ZL,      YL
	mul   r5,      r10
	add   ZL,      r0
	adc   ZH,      r1
	clr   r1
	add   ZL,      r4
	adc   ZH,      r1
	rcall m74_blitspriteptprep
bsplle:

	; Generate upper left sprite part

	dec   r5
	mov   r22,     r6
	mov   r20,     r7
	cp    r4,      r8
	brcc  bspule           ; No sprite (off to the right or left)
	cp    r5,      r9
	brcc  bspule           ; No sprite (off to the bottom or top)
	movw  ZL,      YL
	mul   r5,      r10
	add   ZL,      r0
	adc   ZH,      r1
	clr   r1
	add   ZL,      r4
	adc   ZH,      r1
	rcall m74_blitspriteptprep
bspule:

	; Generate upper right sprite part

	inc   r4
	mov   r22,     r6
	mov   r20,     r7
	subi  r22,     8
	cpi   r22,     0xF8
	breq  bspure           ; No sprite (off tile)
	cp    r4,      r8
	brcc  bspure           ; No sprite (off to the right or left)
	cp    r5,      r9
	brcc  bspure           ; No sprite (off to the bottom or top)
	movw  ZL,      YL
	mul   r5,      r10
	add   ZL,      r0
	adc   ZH,      r1
	clr   r1
	add   ZL,      r4
	adc   ZH,      r1
	rcall m74_blitspriteptprep
bspure:

	; Save current RAM tile count

	sts   m74_rtno,  r12

	; Done

	pop   YH
	pop   YL
	pop   r17
	pop   r16
	pop   r15
	pop   r14
	pop   r13
	pop   r12
	pop   r11
	pop   r10
	pop   r9
	pop   r8
	pop   r7
	pop   r6
	pop   r5
	pop   r4
	ret



;
; Blits a sprite part including the allocation and management of RAM tiles
; for this.
;
; r25:r24: Source 8x8 sprite start address
;     r22: X location on tile (2's complement; 0xF9 - 0x07 inclusive)
;     r20: Y location on tile (2's complement; 0xF9 - 0x07 inclusive)
;     r17: Configuration selectors for ROM tiles
;          bit0 - 2: 0x80 - 0xBF config selector
;          bit4 - 5: 0x00 - 0x7F config selector
;     r16: Flags
;          bit0: If set, flip horizontally
;          bit1: If set, sprite source is RAM
;          bit2: If set, flip vertically
;          bit3: RAM tile map selector, if set, uses config 1
;          bit4: If set, mask is used
;          bit6-7: Sprite importance (smaller: higher)
;       Z: Tile offset to blit sprite onto
;     r13: Maximal number of RAM tiles allocated to sprites
;     r12: Current first free RAM tile index
;      r1: Zero
; Clobbered registers:
; r0, r11, r14, r15, r18, r19, r20, r21, r22, r23, XL, XH, ZL, ZH, T
;
m74_blitspriteptprep:

	; Calculate sprite part importance. This depends on how large portion
	; of the sprite is within the tile, its set importance, and a little
	; also on the mask (a masked sprite is considered slightly less
	; important). Produces a number between 0 and 15, the higher the more
	; important.

	ldi   r23,     15      ; Importance into r23
	sbrs  r22,     7       ; X alignment (0xF9 - 0x07)
	sub   r23,     r22
	sbrc  r22,     7
	add   r23,     r22
	sbrs  r20,     7       ; Y alignment (0xF9 - 0x07)
	sub   r23,     r20
	sbrc  r20,     7
	add   r23,     r20
	sbrc  r16,     4       ; Has mask?
	subi  r23,     1
	mov   r21,     r16     ; Importance bits at 6 - 7
	swap  r21              ; Importance bits at 3 - 4
	lsr   r21              ; Importance bits at 2 - 3 (0, 2, 4 or 6)
	andi  r21,     0x06
	sub   r23,     r21     ; Importance subtracted
	sbrc  r23,     7
	clr   r23              ; Gone below 0: constrain to 0.
	swap  r23              ; Make it suitable for the sprite alloc. workspace

	; Use Y for allocation workspace, so gaining access to LDD / STD

	push  YL
	push  YH
#if (M74_RTLIST_PTRE != 0)
	lds   YL,      m74_rtlist_lo
	lds   YH,      m74_rtlist_hi
#else
	ldi   YL,      lo8(M74_RTLIST_OFF)
	ldi   YH,      hi8(M74_RTLIST_OFF)
#endif

	; Check whether RAM tile allocation is necessary. The mask is also
	; loaded here since a full mask (0xFF) should inhibit sprite output.
	; The RAM tile allocation has no effect on the mask selection, it just
	; needs to store it when a RAM tile is allocated, so subsequent loads
	; here will fetch the proper mask (if a RAM tile is found here, then
	; the underlaying tile's mask). Note that the mask has to be loaded
	; even if masking is off (flags bit 4 clear) since a subsequent sprite
	; placed on the RAM tile might need it (if placed with mask).

	ld    r18,     Z       ; Load tile index
	movw  XL,      ZL      ; Save VRAM address for later use in the RAM tile allocator
	mov   r21,     r18     ; r21 will be used to index in the mask index list
	mov   ZL,      r17
	lsl   ZL
	clr   ZH               ; Prepare Z for ROM tiles (r17: Configuration selectors)
	sbrs  r18,     7
	rjmp  bsppmt00         ; ROM tiles 0x00 - 0x7F
	sbrs  r18,     6
	rjmp  bsppmt80         ; ROM tiles 0x80 - 0xBF
	ldi   ZH,      hi8(M74_TBANK3_0_MSK)
	sbrc  r16,     3
	ldi   ZH,      hi8(M74_TBANK3_1_MSK)
	cpi   ZH,      0       ; If zero, then RAM tiles have no mask
	brne  .+4
	ldi   r21,     0xFE    ; Zero mask
	rjmp  bsppmex          ; Mask acquired (that is, zero, so no mask)
	ldi   ZL,      lo8(M74_TBANK3_0_MSK)
	sbrc  r16,     3
	ldi   ZL,      lo8(M74_TBANK3_1_MSK)
	andi  r21,     0x3F
	add   ZL,      r21
	adc   ZH,      r1      ; Position to the RAM tile's mask index
	ld    r21,     Z       ; Mask loaded from RAM
	rjmp  bsppme           ; Mask acquired
bsppmt00d:
	.byte ( M74_TBANKM0_0_MSK       & 0xFF)
	.byte ((M74_TBANKM0_0_MSK >> 8) & 0xFF)
	.byte ( M74_TBANKM0_1_MSK       & 0xFF)
	.byte ((M74_TBANKM0_1_MSK >> 8) & 0xFF)
	.byte ( M74_TBANKM0_2_MSK       & 0xFF)
	.byte ((M74_TBANKM0_2_MSK >> 8) & 0xFF)
	.byte ( M74_TBANKM0_3_MSK       & 0xFF)
	.byte ((M74_TBANKM0_3_MSK >> 8) & 0xFF)
bsppmt00:
	swap  ZL
	andi  ZL,      0x6
	subi  ZL,      lo8(-(bsppmt00d))
	sbci  ZH,      hi8(-(bsppmt00d))
	rjmp  bsppmtcom
bsppmt80d:
	.byte ( M74_TBANK2_0_MSK       & 0xFF)
	.byte ((M74_TBANK2_0_MSK >> 8) & 0xFF)
	.byte ( M74_TBANK2_1_MSK       & 0xFF)
	.byte ((M74_TBANK2_1_MSK >> 8) & 0xFF)
	.byte ( M74_TBANK2_2_MSK       & 0xFF)
	.byte ((M74_TBANK2_2_MSK >> 8) & 0xFF)
	.byte ( M74_TBANK2_3_MSK       & 0xFF)
	.byte ((M74_TBANK2_3_MSK >> 8) & 0xFF)
	.byte ( M74_TBANK2_4_MSK       & 0xFF)
	.byte ((M74_TBANK2_4_MSK >> 8) & 0xFF)
	.byte ( M74_TBANK2_5_MSK       & 0xFF)
	.byte ((M74_TBANK2_5_MSK >> 8) & 0xFF)
	.byte ( M74_TBANK2_6_MSK       & 0xFF)
	.byte ((M74_TBANK2_6_MSK >> 8) & 0xFF)
	.byte ( M74_TBANK2_7_MSK       & 0xFF)
	.byte ((M74_TBANK2_7_MSK >> 8) & 0xFF)
bsppmt80:
	andi  r21,     0x3F
	andi  ZL,      0xE
	subi  ZL,      lo8(-(bsppmt80d))
	sbci  ZH,      hi8(-(bsppmt80d))
bsppmtcom:
	lpm   r0,      Z+
	lpm   ZH,      Z
	mov   ZL,      r0
	cpi   ZH,      0       ; If zero, then ROM tiles have no mask
	brne  .+4
	ldi   r21,     0xFE    ; Zero mask
	rjmp  bsppmex          ; Mask acquired (that is, zero, so no mask)
	add   ZL,      r21
	adc   ZH,      r1
	lpm   r21,     Z       ; Tiles 0x00 - 0xBF: Mask index list in ROM
bsppme:
	cpi   r21,     0xFF
	brne  bsppmex          ; Not full mask, so something will render
	sbrc  r16,     4       ; Full mask, but masking is turned off: it will render
	rjmp  bsppexit         ; Masks used and tile has full mask: No sprite rendered
bsppmex:
	mov   r11,     r21     ; Mask goes in r11
	movw  ZL,      XL      ; Restore VRAM address
	cpi   r18,     0xC0    ; Check whether it is a sprite workspace RAM tile
	andi  r18,     0x3F    ; (if not, then a RAM tile has to be allocated at this location)
	brcc  .+2
	rjmp  bsppallc         ; ROM tile: Needs allocation
	cp    r18,     r13
	brcs  .+2
	rjmp  bsppallc         ; RAM tile at or above sprite limit: Needs allocation

	; Already allocated RAM tile. Add importance of new sprite part to it,
	; so it becomes less likely to be removed when too many sprites are to
	; be rendered.

	mov   r0,      r18
	lsl   r0
	add   r0,      r18
	add   YL,      r0
	adc   YH,      r1
	ld    r21,     Y
	add   r21,     r23     ; Add importance
	brcc  .+2
	ori   r21,     0xF0    ; Saturate at 15
	st    Y,       r21

bsppallcok:

	; At this point target tile number is already OK in r18 (0x00 - 0x3F).
	; Only the mask source has to be set up (r16 bit 5 and r15:r14).

	movw  YL,      r24     ; Store away sprite address to be restored after call
	bst   r16,     4       ; T: Mask is used setting
	bld   r17,     7       ; Save it away on a spare bit of config. selectors
	mov   r21,     r11     ; Mask index
	cpi   r21,     0xFE    ; Note: 0xFF full mask already branched to exit
	brne  .+2
	andi  r16,     0xEF    ; Zero mask: No mask to be used
	sbrs  r16,     4
	rjmp  bspppe           ; Preparations done if no mask
	ldi   r23,     8       ; Multiplier for mask data
	cpi   r21,     0xE0
	brcc  bsppramma
#if (M74_ROMMASK_PTRE != 0)
	lds   r14,     m74_romma_lo
	lds   r15,     m74_romma_hi
#else
	ldi   ZL,      lo8(M74_ROMMASK_OFF)
	ldi   ZH,      hi8(M74_ROMMASK_OFF)
	movw  r14,     ZL
#endif
	mul   r21,     r23
	add   r14,     r0
	adc   r15,     r1
	clr   r1
	andi  r16,     0xDF    ; Clear bit 5 (Mask source: ROM)
	rjmp  bspppe
bsppramma:
#if (M74_RAMMASK_PTRE != 0)
	lds   r14,     m74_ramma_lo
	lds   r15,     m74_ramma_hi
#else
	ldi   ZL,      lo8(M74_RAMMASK_OFF)
	ldi   ZH,      hi8(M74_RAMMASK_OFF)
	movw  r14,     ZL
#endif
	subi  r21,     0xE0
	mul   r21,     r23
	add   r14,     r0
	adc   r15,     r1
	clr   r1
	ori   r16,     0x20    ; Set bit 5 (Mask source: RAM)
bspppe:
	rcall m74_blitspritept
	sbrc  r17,     7
	ori   r16,     0x10    ; Restore mask usage flag (was only cleared, so OK)
	movw  r24,     YL      ; Restore sprite address

bsppexit:

	pop   YH
	pop   YL
	ret

bsppallc:

	; Try to allocate a new RAM tile for the sprite, then prepare it for
	; sprite blitting.

	cpse  r12,     r13     ; Already hit sprite limit?
	rjmp  bsppallcnw       ; No, can allocate new RAM tile

	; Ran out of RAM tiles. An already allocated RAM tile has to be
	; dropped (if possible) to make space for the new sprite part. This is
	; done by looking for the lowest importance, if it is less or equal
	; the importance of the coming sprite part, it is discarded.

	mov   r21,     r13     ; Will be used for loop counter
	ldi   r18,     0xFF    ; Will seek the lowest importance
	                       ; r19: Will store index of lowest importance
	                       ; (Note: It will be written at least once in loop)
	mov   r19,     r21
	lsl   r19
	add   r19,     r21
	subi  r19,     24      ; Remove one loop block (8 x 3 bytes)
	add   YL,      r19
	adc   YH,      r1      ; Start on top end minus one loop block
	mov   r19,     r21
	subi  r21,     0xF9    ; Add 0x07 to produce a round-up to next multiple of 8
	andi  r21,     0xF8
	andi  r19,     0x07
	breq  bsppallcil8
	cpi   r19,     0x02
	brcs  bsppallcil1
	breq  bsppallcil2
	cpi   r19,     0x04
	brcs  bsppallcil3
	breq  bsppallcil4
	cpi   r19,     0x06
	brcs  bsppallcil5
	breq  bsppallcil6
	rjmp  bsppallcil7
bsppallcil:
	sbiw  YL,      24
	subi  r19,     0xF8    ; Add 8, correcting tile indices found in prev. iteration
bsppallcil8:
	ldd   r0,      Y + 21
	cp    r0,      r18
	brcc  .+4
	mov   r18,     r0
	ldi   r19,     0x07
bsppallcil7:
	ldd   r0,      Y + 18
	cp    r0,      r18
	brcc  .+4
	mov   r18,     r0
	ldi   r19,     0x06
bsppallcil6:
	ldd   r0,      Y + 15
	cp    r0,      r18
	brcc  .+4
	mov   r18,     r0
	ldi   r19,     0x05
bsppallcil5:
	ldd   r0,      Y + 12
	cp    r0,      r18
	brcc  .+4
	mov   r18,     r0
	ldi   r19,     0x04
bsppallcil4:
	ldd   r0,      Y +  9
	cp    r0,      r18
	brcc  .+4
	mov   r18,     r0
	ldi   r19,     0x03
bsppallcil3:
	ldd   r0,      Y +  6
	cp    r0,      r18
	brcc  .+4
	mov   r18,     r0
	ldi   r19,     0x02
bsppallcil2:
	ldd   r0,      Y +  3
	cp    r0,      r18
	brcc  .+4
	mov   r18,     r0
	ldi   r19,     0x01
bsppallcil1:
	ldd   r0,      Y +  0
	cp    r0,      r18
	brcc  .+4
	mov   r18,     r0
	ldi   r19,     0x00
	subi  r21,     8
	brne  bsppallcil
	andi  r18,     0xF0
	cp    r23,     r18     ; Compare with new part's importance
	brcc  .+2
	rjmp  bsppexit         ; Exit without render since it was less important

	; Restore old tile index

	mov   r18,     r19     ; Tile index will be in r18 to fit the other paths
	lsl   r19
	add   r19,     r18
	add   YL,      r19
	adc   YH,      r1      ; Note: During the loop, Y wound back to start
	ldd   XH,      Y + 0
	ldd   XL,      Y + 1
	ldd   r0,      Y + 2
	andi  XH,      0xF
	st    X,       r0      ; Restored old tile, so all sprite work is gone here

	; Jump to prepare new tile

	rjmp  bsppallcbl

bsppallcnw:

	; New RAM tile allocation

	mov   r18,     r12     ; New tile index to use (low 6 bits)
	mov   r0,      r12
	lsl   r0
	add   r0,      r12
	add   YL,      r0
	adc   YH,      r1      ; Position at new ramtile's location in workspace
	inc   r12

bsppallcbl:

	; RAM tile preparation: either newly allocated or over a discarded
	; previously allocated RAM tile, both are the same from here. The tile
	; index to use (low 6 bits only, high 2 bits zero) is in r18 and the
	; workspace (Y) points at the tile's location proper.

	; Prepare workspace. This is the last point where the offset of the
	; tile is needed (to save it into the restore list, and replace it).

	ld    r19,     Z       ; Load original tile index
	ori   r18,     0xC0    ; Temporarily set high 2 bits (indicates RAM tile)
	st    Z,       r18     ; Replace it to new tile index
	andi  r18,     0x3F
	andi  ZH,      0x0F
	or    ZH,      r23     ; Combined with importance of new sprite part
	std   Y + 0,   ZH
	std   Y + 1,   ZL
	std   Y + 2,   r19     ; Original tile saved

	; Copy mask index if possible (there is a mask index list provided for
	; the RAM tiles)

	ldi   XH,      hi8(M74_TBANK3_0_MSK)
	sbrc  r16,     3
	ldi   XH,      hi8(M74_TBANK3_1_MSK)
	cpi   XH,      0
	breq  bsppallcnm       ; No masks used
	ldi   XL,      lo8(M74_TBANK3_0_MSK)
	sbrc  r16,     3
	ldi   XL,      lo8(M74_TBANK3_1_MSK)
	add   XL,      r18
	adc   XH,      r1      ; Position to the RAM tile's mask index
	st    X,       r11
bsppallcnm:

	; Copy the source tile data itself onto the RAM tile.

	; Prepare destination. Note the loading of pitch (increment), 4 bytes
	; less than the actual pitch, to fit the copy blocks below (and this
	; way an increment of 256 bytes also work right).

	sbrs  r16,     3
	rjmp  .+8
	ldi   XL,      lo8(M74_TBANK3_1_OFF)
	ldi   XH,      hi8(M74_TBANK3_1_OFF)
	ldi   r21,     ((M74_TBANK3_1_INC << 2) - 4)
	rjmp  .+6
	ldi   XL,      lo8(M74_TBANK3_0_OFF)
	ldi   XH,      hi8(M74_TBANK3_0_OFF)
	ldi   r21,     ((M74_TBANK3_0_INC << 2) - 4)
	mov   r0,      r18
	lsl   r0
	lsl   r0
	add   XL,      r0
	adc   XH,      r1      ; Position at target beginning

	; Select the source, also branching off to RAM / ROM tiles (different
	; copy routine required), then copy stuff.

	mov   ZL,      r17
	clr   ZH               ; Prepare Z for ROM tiles (r17: Configuration selectors)
	sbrs  r19,     7
	rjmp  bsppallcc00
	sbrs  r19,     6
	rjmp  bsppallcc80

	; RAM tiles

	movw  ZL,      XL
	sub   ZL,      r18
	sbc   ZH,      r1      ; Revert to RAM tile block beginning
	andi  r19,     0x3F
	lsl   r19
	lsl   r19
	add   ZL,      r19
	adc   ZH,      r1      ; Position of source got
	ldi   r23,     8
	rjmp  bsppallccc0e
bsppallccc0l:
	add   XL,      r21
	adc   XH,      r1
	add   ZL,      r21
	adc   ZH,      r1
bsppallccc0e:
	ld    r0,      Z+
	st    X+,      r0
	ld    r0,      Z+
	st    X+,      r0
	ld    r0,      Z+
	st    X+,      r0
	ld    r0,      Z+
	st    X+,      r0
	dec   r23
	brne  bsppallccc0l
	rjmp  bsppallcok       ; Allocation complete (r18 retains tile index low 6 bits)

	; ROM tiles 0x00 - 0x7F

bsppallcc00d:
	.byte ((M74_TBANKM0_0_OFF >> 8) & 0xFF)
	.byte ((M74_TBANKM0_1_OFF >> 8) & 0xFF)
	.byte ((M74_TBANKM0_2_OFF >> 8) & 0xFF)
	.byte ((M74_TBANKM0_3_OFF >> 8) & 0xFF)
bsppallcc00:
	swap  ZL
	andi  ZL,      0x3
	subi  ZL,      lo8(-(bsppallcc00d))
	sbci  ZH,      hi8(-(bsppallcc00d))
	rjmp  bsppallcccoml

	; ROM tiles 0x80 - 0xBF

bsppallcc80d:
	.byte ((M74_TBANK2_0_OFF >> 8) & 0xFF)
	.byte ((M74_TBANK2_1_OFF >> 8) & 0xFF)
	.byte ((M74_TBANK2_2_OFF >> 8) & 0xFF)
	.byte ((M74_TBANK2_3_OFF >> 8) & 0xFF)
	.byte ((M74_TBANK2_4_OFF >> 8) & 0xFF)
	.byte ((M74_TBANK2_5_OFF >> 8) & 0xFF)
	.byte ((M74_TBANK2_6_OFF >> 8) & 0xFF)
	.byte ((M74_TBANK2_7_OFF >> 8) & 0xFF)
bsppallcc80:
	andi  r19,     0x3F
	andi  ZL,      0x7
	subi  ZL,      lo8(-(bsppallcc80d))
	sbci  ZH,      hi8(-(bsppallcc80d))

	; ROM tiles common copy

bsppallcccoml:
	lpm   ZH,      Z
	lsl   r19
	lsl   r19
	mov   ZL,      r19
	adc   ZH,      r1      ; Position of source got
	ldi   r23,     8
	rjmp  bsppallcc00e
bsppallcc00l:
	add   XL,      r21
	adc   XH,      r1
	subi  ZL,      0x04
	sbci  ZH,      0xFE    ; Adds 508 to Z (512 byte row increment for all ROM tiles)
bsppallcc00e:
	lpm   r0,      Z+
	st    X+,      r0
	lpm   r0,      Z+
	st    X+,      r0
	lpm   r0,      Z+
	st    X+,      r0
	lpm   r0,      Z+
	st    X+,      r0
	dec   r23
	brne  bsppallcc00l
	rjmp  bsppallcok       ; Allocation complete (r18 retains tile index low 6 bits)



;
; Add the blitter, to the same section (M74_BlitSprite)
;
#include "videoMode74/videoMode74_sprblit.s"
