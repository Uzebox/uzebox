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
; For every 4bpp tile there is a corresponding mask index location determined
; by M74_ROMMASKIDX_OFF for ROM tiles and M74_RAMMASKIDX_OFF for RAM tiles.
; The mask index at that location selects the mask to use from the mask pool.
; Its values are to be interpreted as follows:
;
; 0x00 - 0xDF: Indexes masks in the ROM mask pool.
; 0xE0 - 0xFD: Indexes masks in the RAM mask pool.
; 0xFE: Zero mask (all sprite pixels are shown).
; 0xFF: Full mask (no sprite pixels visible; no RAM tile allocation happens).
;
; Of course these only apply if the sprite was requested to be blit with mask,
; and the mask index lists are present.
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
; void M74_ResReset(void);
;
; Resets the VRAM restore list.
;
; This should be called if you want to drop the VRAM restore list for
; completely refilling the VRAM. This strategy may be used in such
; configurations where the restore list is placed under the palette buffer
; or the video stack to conserve memory (its role being reduced to support
; sprite importances).
;
.global M74_ResReset

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
; The flags:
; bit0: If set, flip horizontally (M74_SPR_FLIPX)
; bit1: If set, sprite source is RAM (M74_SPR_RAM)
; bit2: If set, flip vertically (M74_SPR_FLIPY)
; bit4: If set, mask is used (M74_SPR_MASK)
; bit6-7: Sprite importance (M74_SPR_I0 - M74_SPR_I3)
;
.global M74_BlitSprite

#if ((M74_RECTB_OFF >> 8) != 0)
;
; void M74_BlitSpriteCol(unsigned int spo, unsigned char xl, unsigned char yl,
;                        unsigned char flg, unsigned char col);
;
; Blits a 8x8 sprite with recoloring.
;
; Uses the rectangular VRAM as target area, xl and yl specifying locations on
; it by the sprite's lower right corner (so location 0:0 produces no sprite,
; 1:1 would make the lower right corner pixel visible).
;
; The sprite has fixed 8x8 pixel layout, 4 bytes per line, 32 bytes total,
; high nybble first for pixels. Color index 0 is transparent.
;
; The col parameter selects the recolor table. If it is larger than 127,
; recoloring is turned off. If slow recoloring is used, 0 turns recoloring
; off.
;
.global M74_BlitSpriteCol
#endif

;
; void M74_PutPixel(unsigned char col, unsigned char xl, unsigned char yl,
;                   unsigned char flg);
;
; Plots a single pixel.
;
; Uses the rectangular VRAM as target area, xl and yl specifying locations on
; it with a 8:8 offset to align proper with sprites (so 8:8 is the upper left
; corner).
;
; A pixel importance value of 3 (M74_SPR_I3) gives the highest possible
; importance score to the allocated RAM tile. A pixel importance of 1
; (M74_SPR_I1) is the lowest importance which adds up when plotting multiple
; pixels on the same tile. A pixel importance of 0 (M74_SPR_I0) gives the
; lowest possible importance score which doesn't add up with multiple pixels.
;
; The flags:
; bit4: If set, mask is used (M74_SPR_MASK)
; bit6-7: Sprite importance (M74_SPR_I0 - M74_SPR_I3)
;
.global M74_PutPixel

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



.section .bss

	; Globals

#if (M74_RTLIST_PTRE != 0)
	m74_rtlist:
	m74_rtlist_lo: .space 1 ; RAM tile allocation workspace pointer, low
	m74_rtlist_hi: .space 1 ; RAM tile allocation workspace pointer, high
#endif
	m74_rtmax:     .space 1 ; Maximal number of RAM tiles allowed

	; Locals

	v_rtno:        .space 1 ; Number of RAM tiles currently allocated
	v_ramtoff_hi:  .space 1 ; RAM tiles offset high (low is zero)

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
M74_VramRestore:
#if (M74_RTLIST_PTRE != 0)
	lds   ZL,      m74_rtlist_lo
	lds   ZH,      m74_rtlist_hi
#else
	ldi   ZL,      lo8(M74_RTLIST_OFF)
	ldi   ZH,      hi8(M74_RTLIST_OFF)
#endif
	lds   XH,      v_rtno
	cpi   XH,      0
	breq  rlsrr1           ; List is empty
	mov   r1,      XH
rlsrr0:
	ld    XH,      Z+
	andi  XH,      0x0F    ; Zero in XH indicates mangled entry, so no restore
	ld    XL,      Z+      ; (This happens when a RAM tile was discarded,
	ld    r0,      Z+      ; by restoring the VRAM, but a new proper tile
	breq  .+2              ; allocation didn't finish yet)
	st    X,       r0      ; Memory restored
	dec   r1
	brne  rlsrr0           ; List emptied
	sts   v_rtno,  r1
rlsrr1:
	ret



;
; void M74_ResReset(void);
;
; Resets the VRAM restore list.
;
; This should be called if you want to drop the VRAM restore list for
; completely refilling the VRAM. This strategy may be used in such
; configurations where the restore list is placed under the palette buffer
; or the video stack to conserve memory (its role being reduced to support
; sprite importances).
;
; Clobbered registers:
; r1 (set zero)
;
M74_ResReset:
	clr   r1
	sts   v_rtno,  r1
	ret



;
; void M74_PutPixel(unsigned char col, unsigned char xl, unsigned char yl,
;                   unsigned char flg);
;
; Plots a single pixel.
;
; Uses the rectangular VRAM as target area, xl and yl specifying locations on
; it with a 8:8 offset to align proper with sprites (so 8:8 is the upper left
; corner).
;
; A pixel importance value of 3 (M74_SPR_I3) gives the highest possible
; importance score to the allocated RAM tile. A pixel importance of 1
; (M74_SPR_I1) is the lowest importance which adds up when plotting multiple
; pixels on the same tile. A pixel importance of 0 (M74_SPR_I0) gives the
; lowest possible importance score which doesn't add up with multiple pixels.
;
;     r24: Pixel color (only low 4 bits used)
;     r22: X location
;     r20: Y location
;     r18: Flags
;          bit4: If set, mask is used
;          bit6-7: Pixel importance (larger: higher)
; Clobbered registers:
; r0, r1 (set zero), r18, r19, r20, r21, r22, r23, r24, r25, XL, XH, ZL, ZH, T
;
M74_PutPixel:

	mov   r25,     r12
	push  r13
	push  r14
	push  r15
	push  r16
	push  r17
	push  YL
	push  YH
	clr   r1               ; Make sure it is zero
	mov   r16,     r18     ; Flags into r16 for the RAM tile allocator

	; Load RAM tile maximum and current RAM tile count

	lds   r13,     m74_rtmax
	lds   r12,     v_rtno

	; Save away X:Y locations (to retain them across calls)

	mov   r23,     r20     ; Y into r23 to keep it clear from the RAM tile allocator
	mov   r19,     r23
	lsr   r19
	lsr   r19
	lsr   r19
	dec   r19              ; Tile Y location on VRAM
#if (M74_XSHIFT_OFF != 0)
	ldi   ZL,      lo8(M74_XSHIFT_OFF)
	ldi   ZH,      hi8(M74_XSHIFT_OFF)
	add   ZL,      r19
	adc   ZH,      r1
	ld    r0,      Z
	add   r22,     r0
#endif
	mov   r18,     r22
	lsr   r18
	lsr   r18
	lsr   r18
	dec   r18              ; Tile X location on VRAM
	andi  r23,     0x07
	andi  r22,     0x07

	; Check whether it is on VRAM

	cpi   r18,     M74_VRAM_W
	brcc  bpixe            ; No pixel (off to the right or left)
	cpi   r19,     M74_VRAM_H
	brcc  bpixe            ; No pixel (off to the bottom or top)

	; Prepare pixel's importance into r20

	mov   r20,     r16
	andi  r20,     0xC0
	breq  .+8              ; 0x00: Don't change any more
	sbrc  r20,     6
	subi  r20,     0xD0    ; (0x00); 0x70; 0x80; 0xF0 importances
	sbrs  r20,     7
	subi  r20,     0x60    ; (0x00); 0x10; 0x80; 0xF0 importances

	; Allocate RAM tile

	rcall m74_ramtilealloc
	brtc  bpixe            ; No RAM tile

	; Plot pixel
	; From RAM tile allocation:
	; r14:r15: Mask offset (only set up if masking remined enabled)
	;     r16: Flags updated:
	;          bit4 cleared if backround's mask is zero (no masking)
	;          bit5 indicates whether mask is in ROM (0) or RAM (1)
	;       Y: Allocated RAM tile's data address

	sbrs  r16,     4
	rjmp  bpixnm           ; No mask: pixel will be produced
	add   r14,     r23
	adc   r15,     r1      ; Set up mask source adding Y
	movw  ZL,      r14
	sbrs  r16,     5
	rjmp  .+4
	ld    r17,     Z       ; RAM mask source
	rjmp  .+2
	lpm   r17,     Z       ; ROM mask source
	sbrc  r22,     2
	swap  r17
	bst   r22,     1
	brtc  .+4
	lsl   r17
	lsl   r17
	sbrc  r22,     0
	lsl   r17
	sbrc  r17,     7
	rjmp  bpixe            ; Pixel masked off
bpixnm:
	lsl   r23
	lsl   r23
	ldi   r19,     0xF0    ; Preserve mask on target
	andi  r24,     0x0F    ; Pixel color
	lsr   r22              ; X offset
	brcs  .+4
	swap  r19
	swap  r24              ; Align pixel in byte to high for even offsets
	add   r23,     r22
	add   YL,      r23     ; Target pixel pair offset in RAM tile
	ld    r0,      Y
	and   r0,      r19
	or    r0,      r24
	st    Y,       r0      ; Pixel completed

bpixe:

	; Done

	pop   YH
	pop   YL
	pop   r17
	pop   r16
	pop   r15
	pop   r14
	pop   r13
	mov   r12,     r25
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
;          bit6-7: Sprite importance (larger: higher)
; Clobbered registers:
; r0, r1 (set zero), r18, r19, r20, r21, r22, r23, r24, r25, XL, XH, ZL, ZH, T
;
M74_BlitSprite:
	push  r16
#if ((M74_RECTB_OFF >> 8) != 0)
#if (M74_REC_SLOW == 0)
	ldi   r16,     0xFF    ; Recoloring is off
#else
	clr   r16              ; Default recolor
#endif
	rjmp  m74_blitsprite_entry



;
; void M74_BlitSpriteCol(unsigned int spo, unsigned char xl, unsigned char yl,
;                        unsigned char flg, unsigned char col);
;
; Blits a 8x8 sprite with recoloring.
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
;          bit6-7: Sprite importance (larger: higher)
;     r16: Recolor table index, bit 7 set: Recoloring is off.
; Clobbered registers:
; r0, r1 (set zero), r18, r19, r20, r21, r22, r23, r24, r25, XL, XH, ZL, ZH, T
;
M74_BlitSpriteCol:

	push  r16
m74_blitsprite_entry:
	push  r11
#endif
	push  r4
	push  r5
	push  r6
	push  r7
	push  r8
	push  r9
	push  r12
	push  r13
	push  r14
	push  r15
	push  r17
	push  YL
	push  YH

	; Load tile descriptor first row which will determine what to use as
	; tile sources.

#if ((M74_RECTB_OFF >> 8) != 0)
	mov   r11,     r16     ; Recolor index will stay in r11
#endif
	mov   r16,     r18     ; Flags will stay in r16
	clr   r1               ; Make sure it is zero

	; Load RAM tile maximum and current RAM tile count

	lds   r13,     m74_rtmax
	lds   r12,     v_rtno

	; Load VRAM dimensions

	ldi   ZL,      M74_VRAM_W
	ldi   ZH,      M74_VRAM_H
	movw  r8,      ZL

	; Prepare Y location

	mov   r23,     r20     ; Y into r23
	mov   r5,      r23
	lsr   r5
	lsr   r5
	lsr   r5               ; Tile Y location on VRAM
	andi  r23,     0x07    ; Location within tile on Y

	; Prepare for lower row

	push  r22
	push  r23
	subi  r23,     8
	cp    r5,      r9
	brcc  bsplle           ; No sprite (off to the bottom or top)
	cpi   r23,     0xF8
	breq  bsplle           ; No sprite (off tile)
#if (M74_XSHIFT_OFF != 0)
	ldi   ZL,      lo8(M74_XSHIFT_OFF)
	ldi   ZH,      hi8(M74_XSHIFT_OFF)
	add   ZL,      r5
	adc   ZH,      r1
	ld    r0,      Z
	add   r22,     r0
#endif
	mov   r4,      r22
	lsr   r4
	lsr   r4
	lsr   r4               ; Tile X location on VRAM
	andi  r22,     0x07
	movw  r6,      r22     ; r7:r6, r23:r22, Location within tile

	; Generate lower right sprite part

	subi  r22,     8
	cpi   r22,     0xF8
	breq  bsplre           ; No sprite (off tile)
	cp    r4,      r8
	brcc  bsplre           ; No sprite (off to the right or left)
	rcall m74_blitspriteptprep
bsplre:

	; Generate lower left sprite part

	dec   r4
	movw  r22,     r6      ; r23:r22, r7:r6
	cp    r4,      r8
	brcc  bsplle           ; No sprite (off to the right or left)
	rcall m74_blitspriteptprep
bsplle:

	; Prepare for upper row

	pop   r23
	pop   r22
	dec   r5
	cp    r5,      r9
	brcc  bspule           ; No sprite (off to the bottom or top)
#if (M74_XSHIFT_OFF != 0)
	ldi   ZL,      lo8(M74_XSHIFT_OFF)
	ldi   ZH,      hi8(M74_XSHIFT_OFF)
	add   ZL,      r5
	adc   ZH,      r1
	ld    r0,      Z
	add   r22,     r0
#endif
	mov   r4,      r22
	lsr   r4
	lsr   r4
	lsr   r4               ; Tile X location on VRAM
	andi  r22,     0x07
	movw  r6,      r22     ; r7:r6, r23:r22, Location within tile

	; Generate upper right sprite part

	subi  r22,     8
	cpi   r22,     0xF8
	breq  bspure           ; No sprite (off tile)
	cp    r4,      r8
	brcc  bspure           ; No sprite (off to the right or left)
	rcall m74_blitspriteptprep
bspure:

	; Generate upper left sprite part

	dec   r4
	movw  r22,     r6      ; r23:r22, r7:r6
	cp    r4,      r8
	brcc  bspule           ; No sprite (off to the right or left)
	rcall m74_blitspriteptprep
bspule:

	; Done

	pop   YH
	pop   YL
	pop   r17
	pop   r15
	pop   r14
	pop   r13
	pop   r12
	pop   r9
	pop   r8
	pop   r7
	pop   r6
	pop   r5
	pop   r4
#if ((M74_RECTB_OFF >> 8) != 0)
	pop   r11
#endif
	pop   r16
	ret



;
; Blits a sprite part including the allocation and management of RAM tiles
; for this.
;
; r25:r24: Source 8x8 sprite start address
;     r23: Y location on tile (2's complement; 0xF9 - 0x07 inclusive)
;     r22: X location on tile (2's complement; 0xF9 - 0x07 inclusive)
;     r16: Flags
;          bit0: If set, flip horizontally
;          bit1: If set, sprite source is RAM
;          bit2: If set, flip vertically
;          bit3: Free to accept original "mask is used" flag
;          bit4: If set, mask is used
;          bit5: Free to accept ROM (0) / RAM (1) mask source select
;          bit6-7: Sprite importance (larger: higher)
;      r4: Column (X) on VRAM
;      r5: Row (Y) on VRAM
;     r13: Maximal number of RAM tiles allocated to sprites
;     r12: Current first free RAM tile index
;      r1: Zero
;  r3: r2: VRAM start offset
;      r8: VRAM width (not used)
;      r9: VRAM height (not used)
;     r10: VRAM pitch
; Return:
;     r12: Updated according to remaining RAM tiles
; Clobbered registers:
; r0, r14, r15, r17, r18, r19, r20, r21, r22, r23, XL, XH, YL, YH, ZL, ZH, T
;
m74_blitspriteptprep:

	; Calculate sprite part importance. This depends on how large portion
	; of the sprite is within the tile, its set importance, and a little
	; also on the mask (a masked sprite is considered slightly less
	; important). Produces a number between 0 and 15, the higher the more
	; important.

	ldi   r20,     15      ; Importance into r20
	sbrs  r22,     7       ; X alignment (0xF9 - 0x07)
	sub   r20,     r22
	sbrc  r22,     7
	add   r20,     r22
	sbrs  r23,     7       ; Y alignment (0xF9 - 0x07)
	sub   r20,     r23
	sbrc  r23,     7
	add   r20,     r23
	sbrc  r16,     4       ; Has mask?
	subi  r20,     1
	mov   r21,     r16     ; Importance bits at 6 - 7
	swap  r21              ; Importance bits at 3 - 4
	lsr   r21              ; Importance bits at 2 - 3 (0, 2, 4 or 6)
	andi  r21,     0x06
	add   r20,     r21     ; Importance added
	subi  r20,     0x06    ; Compensate it (so lower importance makes result less)
	sbrc  r20,     7
	clr   r20              ; Gone below 0: constrain to 0.
	swap  r20              ; Make it suitable for the RAM tile allocator

	; Allocate the RAM tile and calculate necessary address data

	movw  r18,     r4      ; Column (X) & Row (Y) offsets
	rcall m74_ramtilealloc
	brtc  bsppexit         ; No RAM tile

	; Call the sprite part blitter

	rcall m74_blitspritept

bsppexit:

	bst   r16,     3
	bld   r16,     4       ; Restore mask usage flag (was only cleared, so OK)
	ret



;
; RAM tile allocator. This is responsible for managing the allocation of RAM
; tiles and filling them up with the proper contents from the source ROM or
; RAM tile. It also returns the necessary parameters for blitting.
;
;     r20: Importance, low 4 bits must be zero. Higher is the more important.
;     r16: Flags
;          bit3: Free to accept original "mask is used" flag
;          bit4: If set, mask is used
;          bit5: Free to accept ROM (0) / RAM (1) mask source select
;     r18: Column (X) on VRAM
;     r19: Row (Y) on VRAM
;     r13: Maximal number of RAM tiles allocated to sprites
;     r12: Current first free RAM tile index
;      r1: Zero
; Return:
;       T: Set if sprite can render, clear if it can't
;     r12: Updated according to remaining RAM tiles
; r14:r15: Mask offset (only set up if masking remined enabled)
;     r16: Flags updated:
;          bit3 contains original r16 bit4
;          bit4 cleared if backround's mask is zero (no masking)
;          bit5 indicates whether mask is in ROM (0) or RAM (1)
;       Y: Allocated RAM tile's data address
; Clobbered registers:
; r0, r17, r18, r19, r20, r21, XL, XH, ZL, ZH
;
m74_ramtilealloc:

	; Use Y for allocation workspace, so gaining access to LDD / STD

#if (M74_RTLIST_PTRE != 0)
	lds   YL,      m74_rtlist_lo
	lds   YH,      m74_rtlist_hi
#else
	ldi   YL,      lo8(M74_RTLIST_OFF)
	ldi   YH,      hi8(M74_RTLIST_OFF)
#endif

#if (M74_MSK_ENABLE != 0)
	; Load the mask used setting into the 'T' flag, also setting bit 3 of
	; flags as required.

	bst   r16,     4
	bld   r16,     3
#endif

	; Prepare RAM tile offset before first allocation. The only sensible
	; configuration for using sprites or blitting is to have a single RAM
	; tile setup across the entire sprite covered VRAM, so this works, and
	; it enhances performance for repeated blits (especially into the same
	; RAM tile). Uses tile row 0 for the fetch since for sprite blitting
	; the first tile descriptor rows necessarily belong to the respective
	; VRAM rows.

	cpse  r12,     r1
	rjmp  rtatle           ; At least one RAM tile allocated: not the first!
	lds   ZL,      m74_tdesc_lo
	lds   ZH,      m74_tdesc_hi
	lds   r17,     m74_config
	sbrs  r17,     1       ; ROM / RAM tile descriptor index select
	rjmp  .+4
	ld    r17,     Z       ; Tile descriptor index from RAM
	rjmp  .+2
	lpm   r17,     Z       ; Tile descriptor index from ROM
	clr   ZH
	mov   ZL,      r17
	sbrs  r17,     7
	rjmp  rtatl0
	subi  ZL,      lo8(-(M74_RAMTD_OFF - 128 + 4))
	sbci  ZH,      hi8(-(M74_RAMTD_OFF - 128 + 4))
	ld    r17,     Z
	rjmp  rtatl1
rtatl0:
	subi  ZL,      lo8(-(M74_ROMTD_OFF + 4))
	sbci  ZH,      hi8(-(M74_ROMTD_OFF + 4))
	lpm   r17,     Z
rtatl1:
	subi  r17,     0x08    ; Correct offset (RAM tiles have 0x08 added to real base)
	sts   v_ramtoff_hi, r17
rtatle:

	; Load tile's data, first pass.
	; X: VRAM offset
	; r18: Tile index
	; This is for determining whether the tile was already allocated (if
	; it is within the sprite allocated RAM tile range).

	ldi   r21,     M74_VRAM_P
	mul   r19,     r21     ; Row (Y) offset
	movw  XL,      r0
	clr   r1
	subi  XL,      lo8(-(M74_VRAM_OFF))
	sbci  XH,      hi8(-(M74_VRAM_OFF))
	add   XL,      r18     ; Column (X) offset
	adc   XH,      r1
	ld    r18,     X       ; Load tile index

	; If the tile is within the sprite allocated range, then go for fast
	; path, new sprite component should simply blit over it.

	cpi   r18,     0xC0
	brcs  rtanew1
	andi  r18,     0x3F
	cp    r18,     r12
	brcc  rtanew0



	; Fast path: Already allocated RAM tile. VRAM address in X is no
	; longer needed. Load its mask if necessary.

#if (M74_MSK_ENABLE != 0)
	brtc  rtafnm           ; No mask: Sprite blits
	ldi   ZL,      lo8(M74_RAMMASKIDX_OFF)
	ldi   ZH,      hi8(M74_RAMMASKIDX_OFF)
	add   ZL,      r18
	adc   ZH,      r1
	ld    r17,     Z       ; Load mask index of RAM tile
	cpi   r17,     0xFE
	brcc  rtafn0           ; No mask (0xFE) or Full mask (0xFF)
	ldi   r21,     8       ; Multiplier for mask data
	cpi   r17,     0xE0
	brcs  rtafro           ; 0x00 - 0xDF: ROM masks
	ldi   ZL,      lo8(M74_RAMMASK_OFF - (0xE0 * 8))
	ldi   ZH,      hi8(M74_RAMMASK_OFF - (0xE0 * 8))
	ori   r16,     0x20    ; Mask is in RAM (bit 5 set)
	rjmp  rtafrc
rtafn0:
	brne  rtadroptile      ; 0xFF: Full mask: Tile dropped
	andi  r16,     0xEF    ; Clear bit 4 of flags (no mask)
	rjmp  rtafnm           ; 0xFE: No mask: Sprite blits
rtafro:
	ldi   ZL,      lo8(M74_ROMMASK_OFF)
	ldi   ZH,      hi8(M74_ROMMASK_OFF)
	andi  r16,     0xDF    ; Mask is in ROM (bit 5 clear)
rtafrc:
	mul   r17,     r21
	add   ZL,      r0
	adc   ZH,      r1      ; Start offset of mask
	clr   r1
	movw  r14,     ZL
rtafnm:
#endif

	; Add importance of new sprite part to the RAM tile, so it becomes
	; less likely to be removed when too many sprites are to be rendered.

	mov   r0,      r18
	lsl   r0
	add   r0,      r18
	add   YL,      r0
	adc   YH,      r1
	ld    r19,     Y
	add   r19,     r20     ; Add importance
	brcc  .+2
	ori   r19,     0xF0    ; Saturate at 15
	st    Y,       r19

	; Finish, loading the RAM tile data start offset in Y

	ldi   r21,     32
	mul   r18,     r21
	movw  YL,      r0
	clr   r1
	lds   r0,      v_ramtoff_hi
	add   YH,      r0
	set                    ; T flag set: Render the sprite
	ret



	; Slow path: A new RAM tile has to be allocated or reused since the
	; targeted tile is a ROM or non-sprite allocated RAM tile

rtanew0:
	ori   r18,     0xC0
rtanew1:

	; First retrieve the mask index, if it is 0xFF and the sprite is asked
	; to be output with mask, then the sprite part's blit must be
	; cancelled. Otherwise the mask index has to be copied off onto the
	; RAM tile masks so further sprite blits respect it. Along with the
	; mask also retrieve the tile's absolute address to be used for
	; copying its contents off. Low bit will store the tile's type:
	; 0: ROM 4bpp tile
	; 1: RAM 4bpp tile
	; Here it is assumed that VRAM rows correspond to tile descriptor
	; rows, no a necessity for Mode 74, but for sprites it is. It is also
	; assumed that Row mode 0 is used all the way.
	;
	; Results:
	;
	; r15:r14: Tile absolute address (bit 0: Set for RAM tiles)
	;     r17: Mask index

	lds   ZL,      m74_tdesc_lo
	lds   ZH,      m74_tdesc_hi
	lds   r17,     m74_config
	add   ZL,      r19     ; r19: Still row (Y) offset
	adc   ZH,      r1      ; Tile descriptor index location
	sbrs  r17,     1       ; ROM / RAM tile descriptor index select
	rjmp  rtairo
	ld    r17,     Z       ; Tile descriptor index from RAM
	rjmp  rtaira
rtaidra:
	subi  ZL,      lo8(-(M74_RAMTD_OFF - 128 + 1))
	sbci  ZH,      hi8(-(M74_RAMTD_OFF - 128 + 1))
	ld    r19,     Z
	rjmp  rtaidco
rtadroptile:
	clt                    ; Tile can not be rendered exit point
	ret                    ; (Placed here to be accessible by conditional branches)
rtairo:
	lpm   r17,     Z       ; Tile descriptor index from ROM
rtaira:
	clr   ZH
	mov   ZL,      r18
	swap  ZL
	lsr   ZL
	lsr   ZL
	andi  ZL,      0x03    ; Selects tile bank to load
	add   ZL,      r17
	adc   ZH,      r1      ; Might be necessary if RAM area passes 128b
	sbrc  r17,     7       ; Bit 7 zero: ROM tile descriptor
	rjmp  rtaidra
	subi  ZL,      lo8(-(M74_ROMTD_OFF + 1))
	sbci  ZH,      hi8(-(M74_ROMTD_OFF + 1))
	lpm   r19,     Z
rtaidco:
	ldi   r21,     32
	muls  r18,     r21
	add   r19,     r1      ; Tile offset in r19:r0 (same way like in scanline loop)
	cpi   r18,     0xC0
	brcs  rtaitro          ; To ROM tiles (0x00 - 0xBF)
	inc   r0               ; RAM tiles
	mov   r14,     r0
	mov   r15,     r19
#if (M74_MSK_ENABLE != 0)
	mov   ZL,      r18
	andi  ZL,      0x3F
	clr   ZH
	subi  ZL,      lo8(-(M74_RAMMASKIDX_OFF))
	sbci  ZH,      hi8(-(M74_RAMMASKIDX_OFF))
	ld    r17,     Z       ; Mask index in r17
#endif
	rjmp  rtaitlc
rtaitro:
	mov   r14,     r0
	mov   r15,     r19
#if (M74_MSK_ENABLE != 0)
	ldi   r21,     8
	mul   r14,     r21
	mov   r17,     r1
	mul   r15,     r21
	movw  ZL,      r0
	clr   r1
	add   ZL,      r17
	adc   ZH,      r1      ; ROM mask offset (0 - 2047) by tile offset (0 - 65535)
	subi  ZL,      lo8(-(M74_ROMMASKIDX_OFF))
	sbci  ZH,      hi8(-(M74_ROMMASKIDX_OFF))
	lpm   r17,     Z       ; Mask index in r17
#endif
rtaitlc:
	clr   r1

#if (M74_MSK_ENABLE != 0)
	; Masking decisions, processing 0xFE and 0xFF masks. Upon return the T
	; flag (masking enabled) is appropriately modified along with bit 4 of
	; r16. With masking set, r17 is guaranteed to be below 0xFE (so
	; selecting a valid ROM / RAM mask).

	brtc  rtamd0           ; No masking at all
	cpi   r17,     0xFE
	brcs  rtamd0           ; Masking with masks
	brne  rtadroptile      ; 0xFF: Full mask, no sprite blitting
	andi  r16,     0xEF    ; Clear bit 4 of flags (no mask)
	clt                    ; Also clear the T flag (no mask) for further use
rtamd0:
#endif



	; Try to allocate a new RAM tile for the sprite, then prepare it for
	; sprite blitting.

	cpse  r12,     r13     ; Already hit sprite limit?
	rjmp  rtaallocnew      ; No, can allocate new RAM tile

	; Ran out of RAM tiles. An already allocated RAM tile has to be
	; dropped (if possible) to make space for the new sprite part. This is
	; done by looking for the lowest importance, if it is less or equal
	; the importance of the coming sprite part, it is discarded.

	mov   ZH,      r13     ; Will be used for loop counter
	ldi   r19,     0xFF    ; Will seek the lowest importance
	                       ; ZL: Will store index of lowest importance
	mov   ZL,      ZH      ; (Note: It will be written at least once in loop)
	subi  ZH,      1
	andi  ZH,      0xF8
	add   YL,      ZH
	adc   YH,      r1
	add   YL,      ZH
	adc   YH,      r1
	add   YL,      ZH
	adc   YH,      r1      ; Y starts so first iteration performs 1 - 8 comparisons
	andi  ZL,      0x07
	breq  rtaallocil8
	cpi   ZL,      0x02
	brcs  rtaallocil1
	breq  rtaallocil2
	cpi   ZL,      0x04
	brcs  rtaallocil3
	breq  rtaallocil4
	cpi   ZL,      0x06
	brcs  rtaallocil5
	breq  rtaallocil6
	rjmp  rtaallocil7
rtaallocil:
	sbiw  YL,      24
	subi  ZL,      0xF8    ; Add 8, correcting tile indices found in prev. iteration
rtaallocil8:
	ldd   r0,      Y + 21
	cp    r0,      r19
	brcc  .+4
	mov   r19,     r0
	ldi   ZL,      0x07
rtaallocil7:
	ldd   r0,      Y + 18
	cp    r0,      r19
	brcc  .+4
	mov   r19,     r0
	ldi   ZL,      0x06
rtaallocil6:
	ldd   r0,      Y + 15
	cp    r0,      r19
	brcc  .+4
	mov   r19,     r0
	ldi   ZL,      0x05
rtaallocil5:
	ldd   r0,      Y + 12
	cp    r0,      r19
	brcc  .+4
	mov   r19,     r0
	ldi   ZL,      0x04
rtaallocil4:
	ldd   r0,      Y +  9
	cp    r0,      r19
	brcc  .+4
	mov   r19,     r0
	ldi   ZL,      0x03
rtaallocil3:
	ldd   r0,      Y +  6
	cp    r0,      r19
	brcc  .+4
	mov   r19,     r0
	ldi   ZL,      0x02
rtaallocil2:
	ldd   r0,      Y +  3
	cp    r0,      r19
	brcc  .+4
	mov   r19,     r0
	ldi   ZL,      0x01
rtaallocil1:
	ldd   r0,      Y +  0
	cp    r0,      r19
	brcc  .+4
	mov   r19,     r0
	ldi   ZL,      0x00
	subi  ZH,      8
	brcc  rtaallocil
	andi  r19,     0xF0
	cp    r20,     r19     ; Compare with new part's importance
	brcc  .+2
	rjmp  rtadroptile      ; Exit without render since it was less important

	; Restore old tile index

	mov   r19,     ZL      ; New (RAM) tile index will be in r19
	lsl   ZL
	add   ZL,      r19
	add   YL,      ZL
	adc   YH,      r1      ; Note: During the loop, Y wound back to start
	ldd   ZH,      Y + 0
	ldd   ZL,      Y + 1
	ldd   r0,      Y + 2
	andi  ZH,      0xF
	st    Z,       r0      ; Restored old tile, so all sprite work is gone here
	clr   ZH               ; 0x0 as high 4 bits of RAM address indicates no
	std   Y + 0,   ZH      ; tile here in restore list

	; Jump to prepare new tile

	rjmp  rtaallocbl

rtaallocnew:

	; New RAM tile allocation

	mov   r19,     r12     ; New (RAM) tile index to use (low 6 bits)
	mov   r0,      r12
	lsl   r0
	add   r0,      r12
	add   YL,      r0
	adc   YH,      r1      ; Position at new ramtile's location in workspace
	inc   r12

rtaallocbl:

	; RAM tile preparation: either newly allocated or over a discarded
	; previously allocated RAM tile, both are the same from here. The tile
	; index to use (low 6 bits only, high 2 bits zero) is in r19 and the
	; workspace (Y) points at the tile's location proper. In r18 the
	; source tile's index is still present.

	; Prepare workspace. This is the last point where the restore list is
	; accessed. The tile's address has to be kept around for writing the
	; RAM tile index in it after the fill is done (reset tolerance).
	; Note the order of writing the restore list and saving its size:
	; these are for ensuring reset tolerance, that is, no matter where the
	; render is terminated, the restore list will correctly restore every
	; modified tile in the VRAM.

	mov   r21,     XH
	andi  r21,     0x0F
	or    r21,     r20     ; Combined with importance of new sprite part
	std   Y + 2,   r18     ; Original tile saved
	std   Y + 1,   XL
	std   Y + 0,   r21     ; Write high 4 bits of address last
	sts   v_rtno,  r12     ; Tile set up, so new size can also be saved

	; Prepare destination offset (on RAM tile)

	ldi   r21,     32
	mul   r19,     r21
	movw  YL,      r0
	clr   r1
	lds   r0,      v_ramtoff_hi
	add   YH,      r0

	; Load source offset and branch off to ROM / RAM tile fill

	movw  ZL,      r14
	sbrs  ZL,      0
	rjmp  rtaallocromt
	andi  ZL,      0xFE    ; RAM tiles, clear low bit



	; RAM tile filler.

	ldi   r21,     8
rtaallocramtl:
	ld    r0,      Z+
	st    Y+,      r0
	ld    r0,      Z+
	st    Y+,      r0
	ld    r0,      Z+
	st    Y+,      r0
	ld    r0,      Z+
	st    Y+,      r0
	dec   r21
	brne  rtaallocramtl
	rjmp  rtaalloccpe      ; Copy OK, now set VRAM offset to new tile



	; ROM tile filler. Unrolled to make it faster (this is the most common
	; path)

rtaallocromt:
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
	lpm   r0,      Z+
	st    Y+,      r0
rtaalloccpe:



#if (M74_MSK_ENABLE != 0)
	; Write out mask index for the RAM tile

	ldi   ZL,      lo8(M74_RAMMASKIDX_OFF)
	ldi   ZH,      hi8(M74_RAMMASKIDX_OFF)
	add   ZL,      r19
	adc   ZH,      r1
	st    Z,       r17
#endif



	; Fill completed. Rewind destination to the beginning of the tile,
	; and save the RAM tile's index on VRAM so it becomes visible.

	sbiw  YL,      32
	ori   r19,     0xC0    ; Make true tile index (0xC0 - 0xFF) from RAM tile index
	st    X,       r19     ; X still holds VRAM offset



#if (M74_MSK_ENABLE != 0)
	; Complete the mask by loading the offset into r15:r14.

	brtc  rtaanm           ; No masking if clear
	ldi   r21,     8       ; Multiplier for mask data
	cpi   r17,     0xE0
	brcs  rtaaro           ; 0x00 - 0xDF: ROM masks
	ldi   ZL,      lo8(M74_RAMMASK_OFF - (0xE0 * 8))
	ldi   ZH,      hi8(M74_RAMMASK_OFF - (0xE0 * 8))
	ori   r16,     0x20    ; Mask is in RAM (bit 5 set)
	rjmp  rtaarc
rtaaro:
	ldi   ZL,      lo8(M74_ROMMASK_OFF)
	ldi   ZH,      hi8(M74_ROMMASK_OFF)
	andi  r16,     0xDF    ; Mask is in ROM (bit 5 clear)
rtaarc:
	mul   r17,     r21
	add   ZL,      r0
	adc   ZH,      r1      ; Start offset of mask
	clr   r1
	movw  r14,     ZL
rtaanm:
#endif



	; All done, return

	set
	ret



;
; Add the blitter, to the same section
;
#include "videoMode74/videoMode74_sprblit.s"
