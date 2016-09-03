/*
 *  Uzebox Kernel - Mode 90
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
; Video mode 90
;
; Real-time code generated tile data
;
; 360 pixels width (4 cycles / pixel)
; 6 pixels wide tiles, 60 tiles
; 16 colors
; Tiles have fixed colors selected from the 16 of the palette
; Tile size: 112 bytes (at 8 pixels tile height)
; No scrolling
; No sprites
;
;=============================================================================
;
; The mode needs a code tileset and palette source provided by the user.
;
; The following global symbols are required:
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
; The tile row generator should work within the following constraints:
;
; Inputs:
; X:   Points at first tile to render in VRAM
; r2:  Color 0 of palette
; ...
; r17: Color 15 of palette
;
; Entry happens at cycle 261 (the first instruction: the entry jump begins at
; this cycle). Exit must happen at cycle 1742 (after the "ret").
; In total (including the "jmp" and "ret") the code must use 1481 cycles.
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
; The Video RAM. Its size depends on the configuration in VideoMode90.def.h.
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
; TILE_HEIGHT number of entry jumps (2 words each) for each tile row renderer
; in ROM. Normally it is loaded from m90_deftilerows, utilizing this, tweaks
; of the video mode are possible.
;
.global m90_trows

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

	; Locals

	v_fbase:       .space 1          ; Font base (for SetFontTilesIndex)
	v_vrrow:                         ; Video RAM Row
	v_vrrow_lo:    .space 1
	v_vrrow_hi:    .space 1
	v_trow:        .space 1          ; Current row within tile
	v_lcnt:        .space 1          ; Count of remaining lines

.section .text




;
; Video frame renderer
;

sub_video_mode90:

;
; Entry happens in cycle 467.
;

	; Load the palette

	ldi   XL,      lo8(palette) ; ( 468)
	ldi   XH,      hi8(palette) ; ( 469)
	ld    r2,      X+      ; ( 471)
	ld    r3,      X+      ; ( 473)
	ld    r4,      X+      ; ( 475)
	ld    r5,      X+      ; ( 477)
	ld    r6,      X+      ; ( 479)
	ld    r7,      X+      ; ( 481)
	ld    r8,      X+      ; ( 483)
	ld    r9,      X+      ; ( 485)
	ld    r10,     X+      ; ( 487)
	ld    r11,     X+      ; ( 489)
	ld    r12,     X+      ; ( 491)
	ld    r13,     X+      ; ( 493)
	ld    r14,     X+      ; ( 495)
	ld    r15,     X+      ; ( 497)
	ld    r16,     X+      ; ( 499)
	ld    r17,     X+      ; ( 501)

	; Prepare scanline variables

	lds   r0,      render_lines_count ; ( 503)
	sts   v_lcnt,  r0          ; ( 505) Count of scanlines to generate
	ldi   XL,      lo8(vram)   ; ( 506)
	sts   v_vrrow_lo, XL       ; ( 508)
	ldi   XH,      hi8(vram)   ; ( 509)
	sts   v_vrrow_hi, XH       ; ( 511)

	; Wait until next line

	WAIT  r18,     1291    ; (1802)

scl_1:

	rjmp  .                ; (1804)
	rjmp  .                ; (1806)
	rjmp  scl_1e           ; (1808)

scl_0:

	; At 1792 here. Prepare for line

	lds   XL,      v_vrrow_lo  ; (1794) Load VRAM row start address
	lds   XH,      v_vrrow_hi  ; (1796)
	lds   r18,     v_trow  ; (1798) Count tile rows
	inc   r18              ; (1799)
	cpi   r18,     TILE_HEIGHT ; (1800)
	brne  scl_1            ; (1801 / 1802)
	subi  XL,      lo8(-(VRAM_TILES_H)) ; (1802) When the tile row wraps,
	sbci  XH,      hi8(-(VRAM_TILES_H)) ; (1803) advance VRAM row
	sts   v_vrrow_lo, XL   ; (1805)
	sts   v_vrrow_hi, XH   ; (1807)
	clr   r18              ; (1808)
scl_1e:
	sts   v_trow,  r18     ; (1810)
	lds   r20,     m90_trows_lo ; (1812) Select tile row's render code
	lds   r21,     m90_trows_hi ; (1814)
	lsr   r21              ; (1815)
	ror   r20              ; (1816)
	lsl   r18              ; (1817)
	clr   r19              ; (1818)
	add   r20,     r18     ; (1819)
	adc   r21,     r19     ; (1820 = 0)

	; Audio and Alignment (at cycle 1820 = 0 here)
	; The hsync_pulse routine clobbers r0, r1, Z and the T flag.

	rcall hsync_pulse      ; (21 + AUDIO)
	lds   r18,     v_lcnt  ; ( 2)
	subi  r18,     1       ; ( 3)
	brcs  scl_0e           ; ( 4 /  5) All lines drawn?
	sts   v_lcnt,  r18     ; ( 6)
	WAIT  r18,     HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT

	; At 257 here (HSYNC_USABLE_CYCLES is 230), neglecting the
	; CENTER_ADJUSTMENT

	movw  ZL,      r20     ; ( 258)
	icall                  ; ( 261)

	; At 1742 here. Complete CENTER_ADJUSTMENT and go on

	WAIT  r18,     48 - CENTER_ADJUSTMENT
	rjmp  scl_0            ; (1792)

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
