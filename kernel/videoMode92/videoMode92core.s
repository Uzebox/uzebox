/*
 *  Uzebox Kernel - Mode 92
 *  Copyright (C) 2016 Alec Bourque,
 *                     Sandor Zsuga (Jubatian)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

;=============================================================================
;
; Video mode 92
;
; Real-time code generated tile data
;
; 360 pixels width (4 cycles / pixel)
; 6 x 8 pixel tiles, 60 x 28 tiles (at full 224 pixels height)
; 16 colors, option for using up to 2 vertical palette reloads
; Tiles have fixed colors selected from the 16 of the palette
; Tile size: 112 bytes
; No scrolling
; No sprites
; Optional 2bpp framebuffer rows using colors 12 - 15
;
;=============================================================================
;
; The mode needs a code tileset and palette source provided by the user.
;
; The following global symbols are required (Note the "m90" prefix as this is
; a compatible Mode 90 variant!):
;
; m90_defpalette:
;     16 bytes at arbitrary location in ROM, defining the default palette.
;     This palette is loaded upon initialization into palette.
;
; m90_deftilerows:
;     An array of entry jumps (2 words each) for each tile row in ROM. Its
;     size depends on the tile height (TILE_HEIGHT). This address is loaded
;     upon initialization for rendering tile rows. Note: Jumps are used
;     instead of entry points since it is impossible to make the assembler
;     emit addresses in ".byte" directives.
;
; The 2bpp framebuffer works as follows:
;
; It generates 120 pixels horizontally using 30 bytes of data. It can take an
; extra VRAM region to increase vertical resolution, producing tile rows as
; follows:
;
; 0: Normal VRAM row, bytes 0 - 29
; 1: Normal VRAM row, bytes 0 - 29
; 2: Extra VRAM row, bytes 0 - 29
; 3: Extra VRAM row, bytes 0 - 29
; 4: Normal VRAM row, bytes 30 - 59
; 5: Normal VRAM row, bytes 30 - 59
; 6: Extra VRAM row, bytes 30 - 59
; 7: Extra VRAM row, bytes 30 - 59
;
; If the Extra VRAM is pointed at the normal VRAM, an up to 120 x 56 pixels
; quad-scanned framebuffer is produced.
;
; The 2bpp framebuffer bytes are output high bits first.
;
; The tile row generator should work within the following constraints:
;
; Inputs:
; X:   Points at first tile to render in VRAM
; r2:  Color 0 of palette
; ...
; r17: Color 15 of palette
;
; Entry happens at cycle 277 (the first instruction: the entry jump begins at
; this cycle). Exit must happen at cycle 1758 (after the "ret").
; In total (including the "jmp" and "ret") the code must use 1481 cycles.
; The blanking "out" must occur 9 cycles before the "ret".
;
; The palette registers have to be preserved.
;
; It should produce 60 tiles, then blank the output.
;
; Technically anything is doable there, however a normal Mode 90 output should
; work as follows:
;
; r18: Loaded with 60, counts the tiles to render.
; r19: Loaded with 7, used as a multiplier to get the tile data.
; r20: Loaded with the low byte of the base of the tile data blocks.
; r21: Loaded with the high byte of the base of the tile data blocks.
;
; The scanline loop:
;
; common_row_x:
;	dec   r18
;	out   PIXOUT,  r0
;	breq  common_row_x_e
;	ld    ZL,      X+
;	out   PIXOUT,  r1
;	andi  ZL,      0xFF    ; 0x7F if 128 tiles, 0x3F if 64
;	mul   ZL,      r19
;	out   PIXOUT,  r22
;	movw  ZL,      r0
;	add   ZL,      r20
;	adc   ZH,      r21
;	out   PIXOUT,  r23
;	ijmp
; common_row_x_e:
;	nop
;	out   PIXOUT,  r1
;	jmp   common_e
;
; The tile row data blocks:
;
;	mov   r0,      (col)   ; Pixel 2 of tile row
;	out   PIXOUT,  (col)   ; Pixel 0 of tile row
;	mov   r1,      (col)   ; Pixel 3 of tile row
;	mov   r22,     (col)   ; Pixel 4 of tile row
;	mov   r23,     (col)   ; Pixel 5 of tile row
;	out   PIXOUT,  (col)   ; Pixel 1 of tile row
;	rjmp  common_row_x
;
;=============================================================================


;
; unsigned char vram[];
;
; The Video RAM. Its size depends on the configuration in VideoMode92.def.h.
;
.global vram

;
; unsigned char palette[];
;
; 16 bytes specifying the 16 colors potentially used by the tiles. The colors
; are in BBGGGRRR format (normal Uzebox colors).
;
.global palette

;
; const unsigned int* m90_trows;
;
; Eight entry jumps (2 words each) for each tile row renderer in ROM. Normally
; it is loaded from m90_deftilerows, utilizing this, tweaks of the video mode
; are possible.
;
.global m90_trows

;
; unsigned char* m90_pal1;
;
; The palette loaded when the row counter matches m90_palrel1. By default it
; points to palette.
;
.global m90_pal1

;
; unsigned char m90_palrel1;
;
; The row where the palette pointed by m90_pal1 is loaded. By default it is
; unused (0xFF). It has priority over m90_palrel2 (if the two happen on the
; same row).
;
.global m90_palrel1

;
; unsigned char* m90_pal2;
;
; The palette loaded when the row counter matches m90_palrel2. By default it
; points to palette.
;
.global m90_pal2

;
; unsigned char m90_palrel2;
;
; The row where the palette pointed by m90_pal2 is loaded. By default it is
; unused (0xFF).
;
.global m90_palrel2

;
; const unsigned char* m90_exvram;
;
; Extra VRAM's location, used for 2bpp framebuffer. By default it is pointed
; at the normal VRAM.
;
.global m90_exvram

;
; unsigned char m90_split;
;
; Split location between Mode 90 rows and 2bpp framebuffer. The 2bpp
; framebuffer occupies the top tile rows until the line specified in this
; variable. By default this is zero, making all tile rows Mode 90.
;
.global m90_split

;
; void ClearVram(void);
;
; Uzebox kernel function: clears the VRAM.
;
.global ClearVram

;
; void SetTile(char x, char y, unsigned int tileId);
;
; Uzebox kernel function: sets a tile at a given X:Y location on VRAM.
;
.global SetTile

;
; unsigned int GetTile(char x, char y);
;
; Retrieves a tile from a given X:Y location on VRAM. This is a supplementary
; function set up to match with the Uzebox kernel's SetTile function.
;
.global GetTile

;
; void SetFont(char x, char y, unsigned char tileId);
;
; Uzebox kernel function: sets a (character) tile at a given X:Y location on
; VRAM.
;
.global SetFont

;
; unsigned char GetFont(char x, char y);
;
; Retrieves a (character) tile from a given X:Y location on VRAM. This is a
; supplementary function set up to match with the Uzebox kernel's SetFont
; function.
;
.global GetFont

;
; void SetFontTilesIndex(unsigned char index);
;
; Uzebox kernel function: sets the address of the space (0x20) character in
; the tileset, which is by default at tile 0x00.
;
.global SetFontTilesIndex



.section .bss

	; Globals

	vram:          .space VRAM_SIZE
	palette:       .space 16
	m90_trows:
	m90_trows_lo:  .space 1
	m90_trows_hi:  .space 1
	m90_exvram:
	m90_exvram_lo: .space 1
	m90_exvram_hi: .space 1
	m90_split:     .space 1
	m90_pal1:
	m90_pal1_lo:   .space 1
	m90_pal1_hi:   .space 1
	m90_palrel1:   .space 1
	m90_pal2:
	m90_pal2_lo:   .space 1
	m90_pal2_hi:   .space 1
	m90_palrel2:   .space 1

	; Locals

	v_fbase:       .space 1          ; Font base (for SetFontTilesIndex)
	v_lcnt:        .space 1          ; Line counter
	v_pal_lo:      .space 1          ; Current palette pointer, low
	v_pal_hi:      .space 1          ; Current palette pointer, high

.section .text



#define  PIXOUT _SFR_IO_ADDR(PORTC)



;
; Video frame renderer
;

sub_video_mode90:

;
; Entry happens in cycle 467.
;

	; Load initial palette pointer

	ldi   XL,      lo8(palette) ; ( 468)
	ldi   XH,      hi8(palette) ; ( 469)
	sts   v_pal_lo, XL     ; ( 471)
	sts   v_pal_hi, XH     ; ( 473)

	; Prepare scanline variables

	clr   r0               ; ( 474)
	sts   v_lcnt,  r0      ; ( 476)

	; Wait until next line

	WAIT  r18,     1282    ; (1758)
	rjmp  scl_0            ; (1760)

	; Spilled out parts of the render loop: palette split code

scl_px:
	lds   XL,      v_pal_lo     ; (1773)
	lds   XH,      v_pal_hi     ; (1775)
	nop                    ; (1776)
	rjmp  scl_pe           ; (1778)
scl_p1:
	lds   XL,      m90_pal1_lo  ; (1769)
	lds   XH,      m90_pal1_hi  ; (1771)
	nop                    ; (1772)
	rjmp  scl_ps           ; (1774)

	; Loop entry point

scl_0:

	; At 1760 here. Prepare for line

	lds   r18,     v_lcnt  ; (1762) Current line to render

	; Check palette splits

	lds   r19,     m90_palrel1  ; (1764)
	cp    r19,     r18          ; (1765)
	breq  scl_p1                ; (1766 / 1767)
	lds   r19,     m90_palrel2  ; (1768)
	cp    r19,     r18          ; (1769)
	brne  scl_px                ; (1770 / 1771)
	lds   XL,      m90_pal2_lo  ; (1772)
	lds   XH,      m90_pal2_hi  ; (1774)
scl_ps:
	sts   v_pal_lo, XL     ; (1776)
	sts   v_pal_hi, XH     ; (1778)
scl_pe:

	; Palette pointer prepared, load the palette

	ld    r2,      X+      ; (1780)
	ld    r3,      X+      ; (1782)
	ld    r4,      X+      ; (1784)
	ld    r5,      X+      ; (1786)
	ld    r6,      X+      ; (1788)
	ld    r7,      X+      ; (1790)
	ld    r8,      X+      ; (1792)
	ld    r9,      X+      ; (1794)
	ld    r10,     X+      ; (1796)
	ld    r11,     X+      ; (1798)
	ld    r12,     X+      ; (1800)
	ld    r13,     X+      ; (1802)
	ld    r14,     X+      ; (1804)
	ld    r15,     X+      ; (1806)
	ld    r16,     X+      ; (1808)
	ld    r17,     X+      ; (1810)

	; Calculate VRAM base

	mov   r19,     r18     ; (1811) Line counter will remain in r19
	mov   r22,     r18     ; (1812)
	andi  r18,     0x07    ; (1813) Row within tile
	lsr   r22              ; (1814)
	andi  r22,     0xFC    ; (1815)
	ldi   r23,     15      ; (1816) r22 * r23 will give the base

	; Decide row type

	lds   XH,      render_lines_count ; (1818)
	lds   XL,      m90_split    ; (1820 = 0)

	; Audio and Alignment (at cycle 1820 = 0 here)
	; The hsync_pulse routine clobbers r0, r1, Z and the T flag.

	rcall hsync_pulse      ; (21 + AUDIO)
	cp    r19,     XL      ; (  22)
	inc   r19              ; (  23)
	sts   v_lcnt,  r19     ; (  25)
	brcs  s2bpp            ; (  26 /   27) Above the split, 2bpp framebuffer mode
	cp    XH,      r19     ; (  27)
	brcs  scl_0e           ; (  28 /   29) All lines drawn?
	WAIT  XH,      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES

	; Mode 90 rows

	mul   r22,     r23     ; ( 260) r1:r0: Byte offset in VRAM (60 bytes / row)
	movw  XL,      r0      ; ( 261)
	subi  XL,      lo8(-(vram)) ; ( 262)
	sbci  XH,      hi8(-(vram)) ; ( 263)
	lds   ZL,      m90_trows_lo ; ( 265) Select tile row's render code
	lds   ZH,      m90_trows_hi ; ( 267)
	lsr   ZH               ; ( 268)
	ror   ZL               ; ( 269)
	lsl   r18              ; ( 270)
	clr   r23              ; ( 271)
	add   ZL,      r18     ; ( 272)
	adc   ZH,      r23     ; ( 273)
	nop                    ; ( 274)

	; Enter code tile row

	icall                  ; ( 277)

	; At 1758 here.

	rjmp  scl_0            ; (1760)

scl_0e:

	; All lines completed. End of cycle sync area

	; Set vsync flag & flip field

	lds   ZL,      sync_flags
	ldi   r20,     SYNC_FLAG_FIELD
	ori   ZL,      SYNC_FLAG_VSYNC
	eor   ZL,      r20
	sts   sync_flags, ZL

	; Clear any pending timer interrupt

	ldi   ZL,      (1<<OCF1A)
	sts   _SFR_MEM_ADDR(TIFR1), ZL

	ret

s2bppv:
	ldi   XL,      lo8(vram)  ; ( 281)
	ldi   XH,      hi8(vram)  ; ( 282)
	rjmp  s2bppn              ; ( 284)

s2bpp:

	; 2bpp framebuffer mode

	cp    XH,      r19     ; (  28)
	brcs  scl_0e           ; (  29 /   30) All lines drawn?
	WAIT  XH,      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES + 16

	mul   r22,     r23     ; ( 277) r1:r0: Byte offset in VRAM (60 bytes / row)
	sbrs  r18,     1       ; ( 278 /  279)
	rjmp  s2bppv           ; ( 280)
	lds   XL,      m90_exvram_lo ; ( 281)
	lds   XH,      m90_exvram_hi ; ( 283)
	nop                    ; ( 284)
s2bppn:
	add   XL,      r0      ; ( 285)
	adc   XH,      r1      ; ( 286)
	sbrc  r18,     2       ; ( 287 /  288)
	subi  XL,      0xE2    ; ( 288) Adds 30 for lower half of row
	sbrc  r18,     2       ; ( 289 /  290)
	sbci  XH,      0xFF    ; ( 290)
	ldi   r18,     30      ; ( 291) Count of bytes to process into pixels
	clr   r23              ; ( 292) Initial pixel

s2bppl:
	out   PIXOUT,  r23     ; ( 293) In first iteration
	ld    r19,     X+
	clr   ZH
	clr   ZL
	bst   r19,     7
	bld   ZL,      1
	bst   r19,     6
	bld   ZL,      0
	subi  ZL,      0xF2
	ld    r23,     Z
	out   PIXOUT,  r23     ; ( 305) Mode 90, 1440 pixels output begins here
	lpm   ZL,      Z       ; Dummy load (nop)
	clr   ZL
	bst   r19,     5
	bld   ZL,      1
	bst   r19,     4
	bld   ZL,      0
	subi  ZL,      0xF2
	ld    r23,     Z
	out   PIXOUT,  r23
	lpm   ZL,      Z       ; Dummy load (nop)
	clr   ZL
	bst   r19,     3
	bld   ZL,      1
	bst   r19,     2
	bld   ZL,      0
	subi  ZL,      0xF2
	ld    r23,     Z
	out   PIXOUT,  r23
	lpm   ZL,      Z       ; Dummy load (nop)
	andi  r19,     0x03
	mov   ZL,      r19
	subi  ZL,      0xF2
	ld    r23,     Z
	dec   r18
	brne  s2bppl
	nop
	out   PIXOUT,  r23
	lpm   r0,      Z       ; Dummy load (nop)
	lpm   r0,      Z       ; Dummy load (nop)
	rjmp  .
	rjmp  .
	clr   r23
	out   PIXOUT,  r23     ; (1745)

	lpm   r0,      Z       ; Dummy load (nop)
	lpm   r0,      Z       ; Dummy load (nop)
	lpm   r0,      Z       ; Dummy load (nop)
	rjmp  .                ;
	rjmp  .                ; Align with Mode 90 rows

	; At 1758 here.

	rjmp  scl_0            ; (1760)



;
; void ClearVram(void);
;
; Uzebox kernel function: clears the VRAM.
;
.section .text.ClearVram
ClearVram:

	ldi   ZL,      lo8(VRAM_SIZE)
	ldi   ZH,      hi8(VRAM_SIZE)
	ldi   XL,      lo8(vram)
	ldi   XH,      hi8(vram)
	clr   r1
clvr0:
	st    X+,      r1
	sbiw  ZL,      1
	brne  clvr0
	ret



;
; void SetTile(char x, char y, unsigned int tileId);
;
; Uzebox kernel function: sets a tile at a given X:Y location on VRAM.
;
;     r24: x
;     r22: y
; r21:r20: tileId (r21 not used)
;
.section .text.SetTile
SetTile:

	ldi   r18,     VRAM_TILES_H
	mul   r22,     r18     ; Calculate Y line addr in vram
	movw  XL,      r0
	clr   r1
	add   XL,      r24     ; Add X offset
	adc   XH,      r1
	subi  XL,      lo8(-(vram))
	sbci  XH,      hi8(-(vram))
	st    X,       r20
	ret



;
; unsigned int GetTile(char x, char y);
;
; Retrieves a tile from a given X:Y location on VRAM. This is a supplementary
; function set up to match with the Uzebox kernel's SetTile function.
;
;     r24: x
;     r22: y
;
; Returns:
;
; r25:r24: Tile ID (r25 always zero)
;
.section .text.GetTile
GetTile:

	ldi   r18,     VRAM_TILES_H
	mul   r22,     r18     ; Calculate Y line addr in vram
	movw  XL,      r0
	clr   r1
	add   XL,      r24     ; Add X offset
	adc   XH,      r1
	subi  XL,      lo8(-(vram))
	sbci  XH,      hi8(-(vram))
	ld    r24,     X
	clr   r25
	ret



;
; void SetFont(char x, char y, unsigned char tileId);
;
; Uzebox kernel function: sets a (character) tile at a given X:Y location on
; VRAM.
;
;     r24: x
;     r22: y
;     r20: tileId
;
.section .text.SetFont
SetFont:

	ldi   r18,     VRAM_TILES_H
	mul   r22,     r18     ; Calculate Y line addr in vram
	movw  XL,      r0
	clr   r1
	add   XL,      r24     ; Add X offset
	adc   XH,      r1
	subi  XL,      lo8(-(vram))
	sbci  XH,      hi8(-(vram))
	lds   r0,      v_fbase
	add   r20,     r0
	st    X,       r20
	ret



;
; unsigned char GetFont(char x, char y);
;
; Retrieves a (character) tile from a given X:Y location on VRAM. This is a
; supplementary function set up to match with the Uzebox kernel's SetFont
; function.
;
;     r24: x
;     r22: y
;
; Returns:
;
; r25:r24: Tile ID (r25 always zero)
;
.section .text.GetFont
GetFont:

	ldi   r18,     VRAM_TILES_H
	mul   r22,     r18     ; Calculate Y line addr in vram
	movw  XL,      r0
	clr   r1
	add   XL,      r24     ; Add X offset
	adc   XH,      r1
	subi  XL,      lo8(-(vram))
	sbci  XH,      hi8(-(vram))
	lds   r0,      v_fbase
	ld    r24,     X
	add   r24,     r0
	clr   r25
	ret



;
; void SetFontTilesIndex(unsigned char index);
;
; Uzebox kernel function: sets the address of the space (0x20) character in
; the tileset, which is by default at tile 0x00.
;
;     r24: index
;
.section .text.SetFontTilesIndex
SetFontTilesIndex:

	sts   v_fbase, r24
	ret
