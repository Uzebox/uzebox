;
; Uzebox Kernel - Video Mode 748 sprite output
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
; This part manages the output of sprites including the necessary RAM tile
; allocations and tile copies.
;
; It uses whatever VRAM layout is defined, blitting onto Row mode 0 tile rows.
; Y coordinates are by logical scanline, Y = 8 corresponds to line 0. Wraps.
;
; The maximal count of RAM tiles used for sprites may be specified. Sprites
; will take RAM tiles incrementally from m74_rtbase until hitting the limit
; when further allocations will be ignored (this lacks the importance system
; which Mode 74 has). RAM tiles outside of the bounds of the sprite allocator
; may be used normally for tile data, the sprite output routine will properly
; handle their contents like any ROM tile.
;
; The description of mask support:
;
; For every 4bpp ROM tile there is a corresponding mask index location
; determined by M74_ROMMASKIDX_OFF. The mask index at that location selects
; the mask to use from the mask pool. Its values are to be interpreted as
; follows:
;
; 0x00 - 0xFD: Indexes masks in the RAM mask pool.
; 0xFE: Zero mask (all sprite pixels are shown).
; 0xFF: Full mask (no sprite pixels visible; no RAM tile allocation happens).
;
; Of course these only apply if the sprite was requested to be blit with mask,
; and the mask index lists are present.
;
; Sprite generation:
;
; The region of RAM tiles used for sprites (m74_rtbase - limit) also have a
; corresponding region of mask indices in the mask index list for the RAM
; tiles. When allocating a RAM tile, not only the original tile data is
; copied, but also the corresponding mask index. This way overlapping sprites
; would work proper (further sprites also mask against the original tile
; mask).
;



;
; void M74_VramRestore(void);
;
; Restores VRAM and clears sprite engine state.
;
; Should be called after frame generation, before starting working on the
; VRAM. General suggested workflow:
; Restore VRAM => Operate on VRAM (ex.: add user RAM tiles) => Sprite output.
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
; The flags:
; bit0: If set, flip horizontally (M74_SPR_FLIPX)
; bit1: Address line 16 for SPI RAM (M74_SPR_SPIRAM_A16)
; bit2: If set, flip vertically (M74_SPR_FLIPY)
; bit4: If set, mask is used (M74_SPR_MASK)
; bit6-7: Sprite importance (M74_SPR_I0 - M74_SPR_I3), ignored
;
.global M74_BlitSprite

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
; The col parameter selects the recolor table. If it is 0, recoloring is off.
; (Uses the default recolor set which should implement straight output)
;
.global M74_BlitSpriteCol

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
; bit6-7: Sprite importance (M74_SPR_I0 - M74_SPR_I3), ignored
;
.global M74_PutPixel

;
; volatile u8 m74_rtmax;
;
; Maximal number of RAM tiles to use for sprites. RAM tiles are taken
; beginning with RAM tile m74_rtbase, any RAM tile outside these bounds is
; free for any use (if they occur on the VRAM, they can be composed with
; sprites normally).
;
.global m74_rtmax

;
; volatile u8 m74_rtbase;
;
; The base RAM tile for sprite output. RAM address is m74_rtbase * 32 + 0x100.
;
.global m74_rtbase



.section .bss

	; Globals

	m74_rtmax:     .byte 1 ; Maximal number of RAM tiles allowed
	m74_rtbase:    .byte 1 ; Base address for allocating RAM tiles

	; Locals

	v_rtno:        .byte 1 ; Number of RAM tiles currently allocated

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
; r23, r24, r25, XL, XH, ZL, ZH
;
M74_VramRestore:

	; Restore only Mode 0 rows, so parts using different modes are not
	; affected.

	lds   r23,     m74_config
	lds   ZL,      m74_vaddr + 0
	lds   ZH,      m74_vaddr + 1
	ldi   r24,     32
rls_l0:
	sbrs  r23,     1       ; m74_config bit 1: M74_RAM_VADDR
	rjmp  .+6
	ld    XL,      Z+
	ld    XH,      Z+      ; VRAM address
	rjmp  .+4
	lpm   XL,      Z+
	lpm   XH,      Z+      ; VRAM address
	ld    r25,     X
	andi  r25,     7
	brne  rls_l1           ; Not a Mode 0 row, skip it
	adiw  XL,      5       ; VRAM
	ldi   r25,     0

	st    X+,      r25     ; Clear VRAM row
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25

	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25

	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25
	st    X+,      r25

	st    X+,      r25

rls_l1:
	dec   r24
	brne  rls_l0
	sts   v_rtno,  r24     ; No RAM tiles allocated for sprites
	ret



;
; void M74_PutPixel(unsigned char col, unsigned char xl, unsigned char yl,
;                   unsigned char flg);
;
; Plots a single pixel.
;
; Uses the Mode 0 VRAM as target area, xl and yl specifying locations on it
; with a 8:8 offset to align proper with sprites (so 8:8 is the upper left
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
;          bit6-7: Pixel importance (ignored)
; Clobbered registers:
; r0, r1 (set zero), r18, r19, r20, r21, r22, r23, r24, r25, XL, XH, ZL, ZH, T
;
M74_PutPixel:

	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM (if anything was going on)

	push  r14
	push  r15
	push  r16
	push  r17
	push  YL
	push  YH
	clr   r1               ; Make sure it is zero
	mov   r16,     r18     ; Flags into r16 for the RAM tile allocator

	; Prepare X:Y locations

	subi  r20,     8
	subi  r22,     8
	mov   r23,     r20     ; Y into r23 to keep it clear from the RAM tile allocator
	mov   r19,     r23
	lsr   r19
	lsr   r19
	lsr   r19              ; Tile Y location on VRAM
	lds   ZL,      m74_vaddr + 0
	lds   ZH,      m74_vaddr + 1
	add   ZL,      r19
	adc   ZH,      r1
	add   ZL,      r19
	adc   ZH,      r1
	lds   r17,     m74_config
	sbrs  r17,     1       ; m74_config bit 1: M74_RAM_VADDR
	rjmp  .+6
	ld    XL,      Z+
	ld    XH,      Z+      ; VRAM address
	rjmp  .+4
	lpm   XL,      Z+
	lpm   XH,      Z+      ; VRAM address
	ld    r17,     X
	mov   r18,     r17
	andi  r18,     0x07    ; Row mode
	brne  bpixe            ; Not row mode 0: Can not blit here
	swap  r17
	andi  r17,     0x07    ; X shift
	add   r22,     r17
	mov   r18,     r22
	lsr   r18
	lsr   r18
	lsr   r18              ; Tile X location on VRAM
	cpi   r18,     25
	brcc  bpixe            ; Out of VRAM on X
	andi  r23,     0x07
	andi  r22,     0x07

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
;          bit6-7: Sprite importance (ignored)
; Clobbered registers:
; r0, r1 (set zero), r18, r19, r20, r21, r22, r23, r24, r25, XL, XH, ZL, ZH, T
;
M74_BlitSprite:
	push  r16
	clr   r16              ; Default recolor
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

	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM (if anything was going on)

	push  r11
	push  r4
	push  r5
	push  r6
	push  r7
	push  r8
	push  r9
	push  r14
	push  r15
	push  r17
	push  YL
	push  YH

	mov   r11,     r16     ; Recolor index will stay in r11
	mov   r16,     r18     ; Flags will stay in r16
	clr   r1               ; Make sure it is zero

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
	cpi   r23,     0xF8
	breq  bsplle           ; No sprite (off tile)
	lds   ZL,      m74_vaddr + 0
	lds   ZH,      m74_vaddr + 1
	add   ZL,      r5
	adc   ZH,      r1
	add   ZL,      r5
	adc   ZH,      r1
	lds   r17,     m74_config
	sbrs  r17,     1       ; m74_config bit 1: M74_RAM_VADDR
	rjmp  .+6
	ld    XL,      Z+
	ld    XH,      Z+      ; VRAM address
	rjmp  .+4
	lpm   XL,      Z+
	lpm   XH,      Z+      ; VRAM address
	ld    r17,     X
	mov   r18,     r17
	andi  r18,     0x07    ; Row mode
	brne  bsplle           ; Not row mode 0: Can not blit here
	swap  r17
	andi  r17,     0x07    ; X shift
	add   r22,     r17
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
	movw  r8,      XL      ; Save VRAM row
	rcall m74_blitspriteptprep
	movw  XL,      r8      ; Restore VRAM row
bsplre:

	; Generate lower left sprite part

	dec   r4
	movw  r22,     r6      ; r23:r22, r7:r6
	rcall m74_blitspriteptprep
bsplle:

	; Prepare for upper row

	ldi   r22,     0x1F
	dec   r5
	and   r5,      r22     ; Proper Y wrap
	pop   r23
	pop   r22
	lds   ZL,      m74_vaddr + 0
	lds   ZH,      m74_vaddr + 1
	add   ZL,      r5
	adc   ZH,      r1
	add   ZL,      r5
	adc   ZH,      r1
	lds   r17,     m74_config
	sbrs  r17,     1       ; m74_config bit 1: M74_RAM_VADDR
	rjmp  .+6
	ld    XL,      Z+
	ld    XH,      Z+      ; VRAM address
	rjmp  .+4
	lpm   XL,      Z+
	lpm   XH,      Z+      ; VRAM address
	ld    r17,     X
	mov   r18,     r17
	andi  r18,     0x07    ; Row mode
	brne  bspule           ; Not row mode 0: Can not blit here
	swap  r17
	andi  r17,     0x07    ; X shift
	add   r22,     r17
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
	movw  r8,      XL      ; Save VRAM row
	rcall m74_blitspriteptprep
	movw  XL,      r8      ; Restore VRAM row
bspure:

	; Generate upper left sprite part

	dec   r4
	movw  r22,     r6      ; r23:r22, r7:r6
	rcall m74_blitspriteptprep
bspule:

	; Done

	pop   YH
	pop   YL
	pop   r17
	pop   r15
	pop   r14
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
;       X: VRAM row begin
; Return:
; Clobbered registers:
; r0, r14, r15, r17, r18, r19, r20, r21, r22, r23, XL, XH, YL, YH, ZL, ZH, T
;
m74_blitspriteptprep:

	; Allocate the RAM tile and calculate necessary address data

	movw  r18,     r4      ; Column (X) & Row (Y) offsets
	cpi   r18,     25
	brcc  bsppexit1        ; Out of VRAM on X
	rcall m74_ramtilealloc
	brtc  bsppexit0        ; No RAM tile

	; Call the sprite part blitter

	rcall m74_blitspritept

bsppexit0:
	bst   r16,     3
	bld   r16,     4       ; Restore mask usage flag (was only cleared, so OK)
bsppexit1:
	ret



;
; RAM tile allocator. This is responsible for managing the allocation of RAM
; tiles and filling them up with the proper contents from the source ROM or
; RAM tile. It also returns the necessary parameters for blitting.
;
;     r16: Flags
;          bit3: Free to accept original "mask is used" flag
;          bit4: If set, mask is used
;     r18: Column (X) on VRAM
;     r19: Row (Y) on VRAM
;      r1: Zero
;       X: VRAM row begin (at row configuration)
; Return:
;       T: Set if sprite can render, clear if it can't
; r14:r15: Mask offset (only set up if masking remined enabled)
;     r16: Flags updated:
;          bit3 contains original r16 bit4
;          bit4 cleared if backround's mask is zero (no masking)
;       Y: Allocated RAM tile's data address
; Clobbered registers:
; r0, r17, r18, r19, r21, XL, XH, ZL, ZH
;
m74_ramtilealloc:

	; Load tile's data, first pass.
	; This is for determining whether the tile was already allocated (if
	; it is within the sprite allocated RAM tile range).

	movw  ZL,      XL      ; Save VRAM row start for allocation
	adiw  XL,      5       ; VRAM data begin
	add   XL,      r18     ; Column (X) offset
	adc   XH,      r1
	ld    r0,      X       ; Load tile index

	; If the tile is within the sprite allocated range, then go for fast
	; path, new sprite component should simply blit over it.

	sbrs  r0,      7       ; Bit 7 set: RAM tile
	rjmp  rtanewrom

#if (M74_MSK_ENABLE != 0)
	bst   r16,     4       ; Load the mask used setting into the 'T' flag,
	bld   r16,     3       ; also setting bit 3 of flags as required.
#endif

	mov   r18,     r0
	andi  r18,     0x7F
	lds   r17,     m74_rtbase
	cp    r18,     r17
	brcs  rtanewram
	lds   r0,      m74_rtmax
	add   r0,      r17
	cp    r18,     r0
	brcc  rtanewram

	; Fast path: Already allocated RAM tile. VRAM address in X is no
	; longer needed. Load its mask if necessary.

#if (M74_MSK_ENABLE != 0)
	brtc  rtafnm           ; No mask: Sprite blits
	ldi   ZL,      lo8(M74_RAMMASKIDX_OFF)
	ldi   ZH,      hi8(M74_RAMMASKIDX_OFF)
	add   ZL,      r18
	adc   ZH,      r1
	sub   ZL,      r17
	sbc   ZH,      r1
	ld    r17,     Z       ; Load mask index of RAM tile
	cpi   r17,     0xFE
	brcs  rtafro           ; No mask (0xFE) or Full mask (0xFF)
	brne  rtadroptile      ; 0xFF: Full mask: Tile dropped
	andi  r16,     0xEF    ; Clear bit 4 of flags (no mask)
	rjmp  rtafnm           ; 0xFE: No mask: Sprite blits
rtafro:
	ldi   r21,     8       ; Multiplier for mask data
	ldi   ZL,      lo8(M74_ROMMASK_OFF)
	ldi   ZH,      hi8(M74_ROMMASK_OFF)
	mul   r17,     r21
	add   ZL,      r0
	adc   ZH,      r1      ; Start offset of mask
	movw  r14,     ZL
rtafnm:
#endif

	; Finish, loading the RAM tile data start offset in Y

	ldi   r21,     32
	mul   r18,     r21
	movw  YL,      r0
	inc   YH
	clr   r1
	set                    ; T flag set: Render the sprite
	ret



rtadroptile:

	; Sprite tile can not be rendered exit point

	clt
	ret


rtanewram:

	; The sprite is drawn on top of a non-sprite RAM tile. r18 contains
	; the source RAM tile index, these tiles are always non-masked. X
	; points at the foreground VRAM index, Z at the beginning of the row.

	; Can allocate new RAM tile?

	lds   r14,     v_rtno
	lds   r15,     m74_rtmax
	cp    r14,     r15
	brcc  rtadroptile      ; No free RAM tiles: Drop
	mov   r15,     r14
	inc   r15              ; For new allocated tile count
	sts   v_rtno,  r15     ; RAM tile allocated

	; Always no mask

#if (M74_MSK_ENABLE != 0)
	andi  r16,     0xEF    ; Clear bit 4 of flags (no mask)
	clt                    ; Also clear the T flag (no mask) for further use
#endif

	; Calculate source RAM tile address

	ldi   r21,     32
	mul   r18,     r21
	movw  ZL,      r0
	inc   ZH               ; Source RAM tile address

	; Calculate target RAM tile address

	lds   r15,     m74_rtbase
	add   r15,     r14
	mul   r15,     r21
	movw  YL,      r0
	inc   YH               ; Target RAM tile address
	clr   r1               ; Restore r1 to zero

	; RAM tile filler

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

	; Mask: None

#if (M74_MSK_ENABLE != 0)
	ldi   r21,     0xFE    ; No mask
#endif

	rjmp  rtaalloccpe      ; Copy OK, now set VRAM offset to new tile



rtadroptile_sp:

	ldi   ZL,      5       ; Wait for SPI transaction to terminate
	dec   ZL
	brne  .-4
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	rjmp  rtadroptile



rtanewrom:

	; The sprite is drawn on top of a ROM tile. The index is in the bg.
	; VRAM which has to be retrieved from SPI RAM. Start that. X points at
	; the foreground VRAM index, Z at the beginning of the row.

	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   r17,     0x03    ; Read
	out   SR_DR,   r17

#if (M74_MSK_ENABLE != 0)
	bst   r16,     4       ; Load the mask used setting into the 'T' flag,
	bld   r16,     3       ; also setting bit 3 of flags as required.
#else
	rjmp  .
#endif

	ld    r17,     Z+      ; VRAM row: configuration
	ldi   r19,     0x00
	sbrc  r17,     7       ; Address bit 16
	ldi   r19,     0x01
	mov   r0,      r18
	ld    r17,     Z+      ; VRAM row: Bg. address low
	ld    r18,     Z+      ; VRAM row: Bg. address high
	add   r17,     r0
	adc   r18,     r1
	adc   r19,     r1      ; (15) Address of tile calculated

	; Can allocate new RAM tile?

	lds   r14,     v_rtno  ; (17)
	out   SR_DR,   r19     ; SPI RAM: Address high
	lds   r15,     m74_rtmax
	cp    r14,     r15
	brcc  rtadroptile_sp   ; No free RAM tiles: Drop with SPI cancel
	mov   r15,     r14     ; ( 5)
	inc   r15              ; ( 6) For new allocated tile count

	; Calculate target RAM tile address

	ldi   r21,     32
	lds   r0,      m74_rtbase
	add   r0,      r14
	mul   r0,      r21
	movw  YL,      r0
	clr   r1
	inc   YH               ; (15) Target RAM tile address

	; Prepare for ROM tile address calculation

	ld    r21,     Z+      ; (17) VRAM row: ROM tiles 0x00 - 0x7F base high
	out   SR_DR,   r18     ; SPI RAM: Address mid
	ld    r0,      Z+      ; ( 2) VRAM row: ROM tiles 0x80 - 0xFF base high
	mov   ZH,      r0
	ldi   ZL,      0
	lsr   ZH
	ror   ZL
	lsr   ZH
	ror   ZL
	lsr   ZH
	ror   ZL
	lsr   ZH
	ror   ZL
	lsr   ZH
	ror   ZL               ; (14) ROM mask base for 0x80 - 0xFF
	mov   r19,     r21
	ldi   r18,     0
	lsr   r19              ; (17)
	out   SR_DR,   r17     ; SPI RAM: Address low
	ror   r18
	lsr   r19
	ror   r18
	lsr   r19
	ror   r18
	lsr   r19
	ror   r18
	lsr   r19
	ror   r18              ; ( 9) ROM mask base address for 0x00 - 0x7F
#if (M74_MSK_ENABLE != 0)
	subi  ZL,      lo8(-(M74_ROMMASKIDX_OFF))
	sbci  ZH,      hi8(-(M74_ROMMASKIDX_OFF))
	subi  r18,     lo8(-(M74_ROMMASKIDX_OFF))
	sbci  r19,     hi8(-(M74_ROMMASKIDX_OFF))
#else
	rjmp  .
	rjmp  .                ; (13)
#endif

	; Wait for SPI

	rjmp  .
	rjmp  .                ; (17)
	out   SR_DR,   r17     ; SPI RAM: Dummy
	rjmp  .
	rjmp  .
	rjmp  .
	rjmp  .
	rjmp  .
	rjmp  .
	rjmp  .
	rjmp  .                ; (16)

	; Finally the tile index arrives, masking can be checked

	in    r17,     SR_DR
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	sbrc  r17,     7
	mov   r21,     r0      ; Select ROM tiles base

#if (M74_MSK_ENABLE != 0)
	brtc  rtamd1           ; No masking at all
	sbrs  r17,     7
	movw  ZL,      r18     ; Select Mask base
	andi  r17,     0x7F    ; Tile select within 128 tile bank
	add   ZL,      r17
	adc   ZH,      r1
	lpm   ZL,      Z       ; Load mask index
	cpi   ZL,      0xFE
	brcs  rtamd0           ; Masking with masks
	breq  .+2
	rjmp  rtadroptile      ; 0xFF: Full mask, no sprite blitting (RAM tile remains unused)
	andi  r16,     0xEF    ; Clear bit 4 of flags (no mask)
	clt                    ; Also clear the T flag (no mask) for further use
rtamd1:
	mov   ZH,      r21     ; ROM tiles base
	ldi   r21,     0xFE    ; Use 0xFE mask (no mask)
	rjmp  rtamd2
rtamd0:
	mov   ZH,      r21     ; ROM tiles base
	ldi   r19,     8
	mul   ZL,      r19     ; Mask data offset
	movw  r18,     r0
	subi  r18,     lo8(-(M74_ROMMASK_OFF))
	sbci  r19,     hi8(-(M74_ROMMASK_OFF))
	mov   r21,     ZL      ; Mask index into r21
rtamd2:
#else
	mov   ZH,      r21     ; ROM tiles base
#endif

	; Didn't bail out on masks: the tile will be allocated.

	andi  r17,     0x7F    ; Tile select within 128 tile bank
	sts   v_rtno,  r15     ; New allocated tile count

	; Calculate source ROM tile address

	ldi   ZL,      32
	mul   r17,     ZL
	mov   ZL,      r0
	add   ZH,      r1
	clr   r1

	; ROM tile filler. Unrolled to make it faster (this is the most common
	; path)

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

	; Write out mask index for the RAM tile

#if (M74_MSK_ENABLE != 0)
	ldi   ZL,      lo8(M74_RAMMASKIDX_OFF)
	ldi   ZH,      hi8(M74_RAMMASKIDX_OFF)
	add   ZL,      r14
	adc   ZH,      r1
	st    Z,       r21
#endif

	; Fill completed. Rewind destination to the beginning of the tile,
	; and save the RAM tile's index on VRAM so it becomes visible.

	sbiw  YL,      32
	lds   r21,     m74_rtbase
	add   r21,     r14
	ori   r21,     0x80    ; Tile allocated
	st    X,       r21     ; X still holds VRAM offset

#if (M74_MSK_ENABLE != 0)
	movw  r14,     r18     ; Mask address was calculated into r19:r18
#endif

	; All done, return

	set
	ret



;
; Add the blitter, to the same section
;
#include "videoMode74/videoMode74_sprb_sr.s"
