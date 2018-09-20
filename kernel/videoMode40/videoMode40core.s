/*
 *  Uzebox Kernel - Mode 40
 *  Copyright (C) 2017 Sandor Zsuga (Jubatian)
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
; Video mode 40
;
; Attribute mode 40x25 text
;
; 320 pixels width (4 cycles / pixel)
; 8 pixels wide tiles, 40 tiles
; 8 pixels tall tiles
; Attribute RAM allowing definition of FG & BG color for each tile
; 40 + 80 byte RAM usage / tile row (VRAM, ARAM)
; No scrolling
; No sprites
;
; Alternative: 3bpp low-res Bitmap
;
; 80 pixels width (16 cycles / pixel), double scanning
; Bitplane linear layout, 2 planes in ARAM, 1 plane in VRAM
;
; Alternative: 1bpp mid-res Bitmap
;
; 160 pixels width (8 cycles / pixel), double scanning
; Pixels are in ARAM, color attributes in VRAM
;
; Different VRAM sizes are possible.
;
;=============================================================================

;
; unsigned char vram[];
;
; The Video RAM. Its size depends on the configuration in VideoMode40.def.h.
;
.global vram

;
; unsigned char aram[];
;
; The Attribute RAM. Its size depends on the configuration in
; VideoMode40.def.h.
;
.global aram

;
; unsigned char palette[];
;
; 8 bytes specifying the 8 colors used by the 3bpp low-res Bitmap mode. The
; colors are in BBGGGRRR format (normal Uzebox colors).
;
.global palette

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

;
; void SetTileTable(unsigned char const* data);
;
; Uzebox kernel function: sets the address of the tileset to use. The tileset
; must start on a 256 byte boundary! (Use __attribute__ ((aligned(256))) on
; tileset definitions)
;
.global SetTileTable

;
; void SetTileTableRow(unsigned char const* data, unsigned char row);
;
; Sets tileset to use or 3bpp (0xFExx) / 1bpp (0xFFxx) mode for a given row.
; The tileset must start on a 256 byte boundary!
;
.global SetTileTableRow

;
; void SetBorderColor(unsigned char col);
;
; Sets the border color.
;
.global SetBorderColor

;
; void PutPixel(unsigned char x, unsigned char y, unsigned char color);
;
; Puts a pixel at the given X:Y location. This is only valid for the 1bpp and
; the 3bpp row modes, in other row modes the call is ignored.
;
.global PutPixel

;
; unsigned char GetPixel(unsigned char x, unsigned char y);
;
; Gets a pixel from the given X:Y location. This is only valid for the 1bpp
; and the 3bpp row modes, in other row modes it returns zero.
;
.global GetPixel



#define PIXOUT VIDEO_PORT



.section .bss

	; Globals

.balign 8
	palette:       .space 8
.balign 1
	vram:          .space VRAM_SIZE
	aram:          .space VRAM_SIZE * 2

	; Locals

	v_tbase:       .space VRAM_TILES_V ; Tileset base (address high)
	v_fbase:       .space 1            ; Font base for char output
	v_border:      .space 1            ; Border color

.section .text




;
; Video frame renderer
;

sub_video_mode40:

;
; Entry happens in cycle 467.
;

	; Load initial palette pointer

	ldi   XL,      lo8(palette) ; ( 468)
	ldi   XH,      hi8(palette) ; ( 469)
	movw  r2,      XL           ; ( 470) r3:r2: Palette

	; Prepare scanline variables

	ldi   XL,      lo8(vram)
	ldi   XH,      hi8(vram)
	ldi   YL,      lo8(aram)
	ldi   YH,      hi8(aram)    ; ( 474)
	lds   r14,     v_border     ; ( 476) r14: Border color
	ldi   r20,     0            ; ( 477) r20: Start line in VRAM
	ldi   r21,     (VRAM_TILES_V * TILE_HEIGHT) ; ( 478) r21: Line count to output from VRAM
	lds   r19,     render_lines_count
	mov   r22,     r19
	sub   r19,     r21          ; ( 482)
	brcs  .+8
	mov   r18,     r19
	lsr   r18                   ; ( 485) Top border
	sub   r19,     r18          ; ( 486) Bottom border
	rjmp  .+8                   ; ( 488)
	nop
	mov   r21,     r22          ; ( 486) As many lines as available
	ldi   r18,     0            ; ( 487) No top border
	ldi   r19,     0            ; ( 488) No bottom border

	; At this point the following registers are prepared:
	;
	;  r3: r2: Palette
	;     r18: Top border lines
	;     r19: Bottom border lines
	;     r20: Start line in the VRAM region (0)
	;     r21: VRAM lines
	;       X: vram top
	;       Y: aram top

	; Wait until next line

	WAIT  ZL,      1331    ; (1819)

	; Loop entry point

scl_0:

	; Audio and Alignment (at cycle 1819 here)
	; The hsync_pulse routine clobbers r0, r1, Z and the T flag.

	call  hsync_pulse      ; (22 + AUDIO)

	; Left padding wait

	WAIT  ZL,      (HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES) + ((44 - VRAM_TILES_H) * 16)

	; Prepare for line

	cpi   r18,     0       ; ( 1)
	breq  scl_vram         ; ( 2 /  3)

	; Top border

	dec   r18              ; ( 3)
	WAIT  ZL,      11
	out   PIXOUT,  r14     ; (15) Border begin
	rjmp  co_exit_nr

scl_vram:

	cpi   r21,     0       ; ( 4)
	breq  scl_bbor         ; ( 5 /  6)

	; VRAM area

	dec   r21              ; ( 6)
	ldi   r17,     VRAM_TILES_H
	mov   r25,     r20
	andi  r25,     0x07
	mov   ZL,      r20
	inc   r20
	lsr   ZL
	lsr   ZL
	lsr   ZL
	out   PIXOUT,  r14     ; (15 =>  1) Left border
	clr   ZH
	subi  ZL,      lo8(-(v_tbase))
	sbci  ZH,      hi8(-(v_tbase))
	ld    r24,     Z       ; ( 6) Tile row configuration
	cpi   r24,     0xFE
	breq  b3_prep          ; ( 8 /  9) 0xFE: 3bpp
	brcc  b1_prep          ; ( 9 / 10) 0xFF: 1bpp

	; Attribute mode

	mov   r15,     r24
	add   r15,     r25     ; (11) Row select
	ldi   r16,     5
	ldi   ZL,      lo8(pm(at_b))
	ldi   ZH,      hi8(pm(at_b))
	movw  r12,     ZL
	ld    ZL,      X+
	mov   ZH,      r15     ; (18) r15: Row select
	lpm   r0,      Z
	movw  ZL,      r12     ; (22) r13:r12: at_b
	mul   r0,      r16     ; (24) r16: 5 (Size of AT_HEAD blocks in words)
	add   ZL,      r0
	ld    r10,     Y+      ; (27) BG color
	adc   ZH,      r1
	ld    r11,     Y+      ; (30) FG color
	ijmp                   ; (32) Enter scanline loop

scl_bbor:

	cpi   r19,     0       ; ( 7)
	breq  scl_end          ; ( 8 /  9)

	; Bottom border

	dec   r19              ; ( 9)
	WAIT  ZL,      5
	out   PIXOUT,  r14     ; (15) Border begin
	rjmp  co_exit_nr

b3_prep:

	; 3bpp mode

	movw  ZL,      r2      ; (10) Load palette pointer
	sbrs  r25,     2
	rjmp  b3_row0123
	nop
	sbrs  r25,     1
	rjmp  b3_row45
	nop
	rjmp  b36_entry        ; (18)
b3_row45:
	rjmp  b34_entry        ; (18)
b3_row0123:
	sbrs  r25,     1
	rjmp  b3_row01
	nop
	rjmp  b32_entry        ; (18)
b3_row01:
	rjmp  b30_entry        ; (18)

b1_prep:

	; 1bpp mode

	rjmp  .                ; (12)
	lpm   ZL,      Z       ; (15)
	lds   r10,     palette ; (17) Load BG color
	ld    r11,     X+      ; (19) Preload first FG
	sbrs  r25,     2
	rjmp  b1_row0123
	nop
	sbrs  r25,     1
	rjmp  b1_row45
	nop
	rjmp  b16_loop         ; (27)
b1_row45:
	rjmp  b14_loop         ; (27)
b1_row0123:
	sbrs  r25,     1
	rjmp  b1_row01
	nop
	rjmp  b12_loop         ; (27)
b1_row01:
	rjmp  b10_loop         ; (27)

scl_end:

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

co_exit_nr:

	; Common exit for border lines

	WAIT  ZL,      (VRAM_TILES_H * 32) + (64 - 6)
	rjmp  co_exit_x

co_exit:

	; Common exit at cy 3 after border generation.

	cpi   r25,     7
	breq  .+4
	lpm   ZL,      Z
	rjmp  .+6
	subi  YL,      lo8(-(VRAM_TILES_H * 2))
	sbci  YH,      hi8(-(VRAM_TILES_H * 2))
	adiw  XL,      (VRAM_TILES_H)
	WAIT  ZL,      21
co_exit_x:
	clr   r11
	out   PIXOUT,  r11

	; Right padding wait

	WAIT  ZL,      58 + ((44 - VRAM_TILES_H) * 16)

	; Done

	rjmp  scl_0



;
; Scanline loop core for the 1bpp Bitmap mode (4x4 px char blocks)
;
; Register allocation:
;  r1: r0: Temp for colors
;     r10: BG color
;     r11: Temp for FG color
; r13:r12: Temp for colors
;     r14: Border color
;     r17: Count of tiles to output
;  YH: YL: Attribute RAM pointer
;  XH: XL: Video RAM pointer
;
; Video data:
; ARAM: Row0: b0,0-3; Row1: b0,4-7; Row2: b1,0-3; Row3: b1,4-7
; VRAM: Foreground colors
;
b10_mid:
	mov   r1,      r10
	sbrc  r0,      1
	mov   r1,      r11
	mov   r12,     r10
	out   PIXOUT,  r1
	sbrc  r0,      2
	mov   r12,     r11
	adiw  YL,      2
	mov   r13,     r10
	sbrc  r0,      3
	mov   r13,     r11
	out   PIXOUT,  r12
	ld    r11,     X+
	dec   r17
	ret

b12_mid:
	mov   r1,      r10
	sbrc  r0,      5
	mov   r1,      r11
	mov   r12,     r10
	out   PIXOUT,  r1
	sbrc  r0,      6
	mov   r12,     r11
	adiw  YL,      2
	mov   r13,     r10
	sbrc  r0,      7
	mov   r13,     r11
	out   PIXOUT,  r12
	ld    r11,     X+
	dec   r17
	ret

b10_loop:
	ldd   r0,      Y + 0
	mov   r1,      r10
	sbrc  r0,      0
	mov   r1,      r11
	out   PIXOUT,  r1
	rcall b10_mid
	out   PIXOUT,  r13
	brne  b10_loop
	rjmp  b1_exit

b12_loop:
	ldd   r0,      Y + 0
	mov   r1,      r10
	sbrc  r0,      4
	mov   r1,      r11
	out   PIXOUT,  r1
	rcall b12_mid
	out   PIXOUT,  r13
	brne  b12_loop
	rjmp  b1_exit

b14_loop:
	ldd   r0,      Y + 1
	mov   r1,      r10
	sbrc  r0,      0
	mov   r1,      r11
	out   PIXOUT,  r1
	rcall b10_mid
	out   PIXOUT,  r13
	brne  b14_loop
	rjmp  b1_exit

b16_loop:
	ldd   r0,      Y + 1
	mov   r1,      r10
	sbrc  r0,      4
	mov   r1,      r11
	out   PIXOUT,  r1
	rcall b12_mid
	out   PIXOUT,  r13
	brne  b16_loop
	rjmp  b1_exit

b1_exit:
	sbiw  XL,      (VRAM_TILES_H + 1)
b1_exit_b3:
	subi  YL,      lo8(VRAM_TILES_H * 2)
	sbci  YH,      hi8(VRAM_TILES_H * 2)
	out   PIXOUT,  r14
	rjmp  co_exit



;
; Scanline loop core for the 3bpp Bitmap mode (2x4 px char blocks)
;
; Register allocation:
;  r1: r0: Temp for bitplanes
;     r10: Temp for bitplane
;     r11: Temp for color output
;     r14: Border color
;     r17: Count of tiles to output
;  YH: YL: Attribute RAM pointer
;  XH: XL: Video RAM pointer
;  ZH: ZL: Palette pointer
;
; Bitplane layout:
; Color bit0: ARAM, first byte
; Color bit1: ARAM, second byte
; Color bit2: VRAM
; Each byte encodes a 2x4 pixel block, bit0 for the upper left px, bit 7 for
; the lower right (so essentially this is still a char. mode which can
; integrate fine with the normal character mode).
;
b30_loop:
	nop
	out   PIXOUT,  r11
	nop
b30_entry:
	ld    r10,     X+
	ld    r0,      Y+
	ld    r1,      Y+
	bst   r0,      0
	bld   ZL,      0
	bst   r1,      0
	bld   ZL,      1
	bst   r10,     0
	bld   ZL,      2
	ld    r11,     Z
	out   PIXOUT,  r11
	bst   r0,      1
	bld   ZL,      0
	bst   r1,      1
	bld   ZL,      1
	bst   r10,     1
	bld   ZL,      2
	ld    r11,     Z
	lpm   r0,      Z
	dec   r17
	brne  b30_loop
	rjmp  b3_exit

b32_loop:
	nop
	out   PIXOUT,  r11
	nop
b32_entry:
	ld    r10,     X+
	ld    r0,      Y+
	ld    r1,      Y+
	bst   r0,      2
	bld   ZL,      0
	bst   r1,      2
	bld   ZL,      1
	bst   r10,     2
	bld   ZL,      2
	ld    r11,     Z
	out   PIXOUT,  r11
	bst   r0,      3
	bld   ZL,      0
	bst   r1,      3
	bld   ZL,      1
	bst   r10,     3
	bld   ZL,      2
	ld    r11,     Z
	lpm   r0,      Z
	dec   r17
	brne  b32_loop
	rjmp  b3_exit

b34_loop:
	nop
	out   PIXOUT,  r11
	nop
b34_entry:
	ld    r10,     X+
	ld    r0,      Y+
	ld    r1,      Y+
	bst   r0,      4
	bld   ZL,      0
	bst   r1,      4
	bld   ZL,      1
	bst   r10,     4
	bld   ZL,      2
	ld    r11,     Z
	out   PIXOUT,  r11
	bst   r0,      5
	bld   ZL,      0
	bst   r1,      5
	bld   ZL,      1
	bst   r10,     5
	bld   ZL,      2
	ld    r11,     Z
	lpm   r0,      Z
	dec   r17
	brne  b34_loop
	rjmp  b3_exit

b36_loop:
	nop
	out   PIXOUT,  r11
	nop
b36_entry:
	ld    r10,     X+
	ld    r0,      Y+
	ld    r1,      Y+
	bst   r0,      6
	bld   ZL,      0
	bst   r1,      6
	bld   ZL,      1
	bst   r10,     6
	bld   ZL,      2
	ld    r11,     Z
	out   PIXOUT,  r11
	bst   r0,      7
	bld   ZL,      0
	bst   r1,      7
	bld   ZL,      1
	bst   r10,     7
	bld   ZL,      2
	ld    r11,     Z
	lpm   r0,      Z
	dec   r17
	brne  b36_loop
	rjmp  b3_exit

b3_exit:
	out   PIXOUT,  r11
	lpm   ZL,      Z
	lpm   ZL,      Z
	lpm   ZL,      Z
	sbiw  XL,      (VRAM_TILES_H)
	rjmp  b1_exit_b3



;
; Scanline loop core for the Attribute mode
;
; Register allocation:
;
;  r1: r0: Temp (multiplication)
; r11:r10: FG:BG colors (temp)
; r12:r13: Preload with at_b
;     r14: Border color
;     r15: ROM Row select for character images
;     r16: 5 (Size of AT_HEAD blocks in words)
;     r17: Count of tiles to output
;  YH: YL: Attribute RAM pointer (Preload 1st tile for entry)
;  XH: XL: Video RAM pointer (Preload 1st tile for entry)
;  ZH: ZL: Temp (load progmem, indirect jump)
;
; Entry is by preparing the first tile and performing an ijmp.
;
; ROM tile data is low bits on the left (bit 0 corresponds to left px.)
;
at_exit:
	nop
	out   PIXOUT,  r14
	rjmp  co_exit

.macro AT_HEAD px0, px1, midl
	out   PIXOUT,  \px0    ; bit0: Px0
	ld    ZL,      X+
	mov   ZH,      r15     ; r15: Row select
	out   PIXOUT,  \px1    ; bit1: Px1
	rjmp  \midl
.endm
.macro AT_MIDL px2, px3, tail
	dec   r17              ; r17: Tilecount
	out   PIXOUT,  \px2    ; bit2: Px2
	lpm   r0,      Z
	out   PIXOUT,  \px3    ; bit3: Px3
	rjmp  \tail
.endm
.macro AT_TAIL px4, px5, px6, px7, endc
	movw  ZL,      r12     ; r13:r12: at_b
	out   PIXOUT,  \px4    ; bit4: Px4
	breq  \endc
	mul   r0,      r16     ; r16: 5 (Size of AT_HEAD blocks in words)
	out   PIXOUT,  \px5    ; bit5: Px5
	add   ZL,      r0
	ld    r0,      Y+      ; BG color
	out   PIXOUT,  \px6    ; bit6: Px6
	adc   ZH,      r1
	ld    r1,      Y+      ; FG color
	out   PIXOUT,  \px7    ; bit7: Px7
	movw  r10,     r0      ; Copy colors to r10 (BG) and r11 (FG)
	ijmp
.endm
.macro AT_ENDC px5, px6, px7
	subi  YL,      lo8(VRAM_TILES_H * 2)
	out   PIXOUT,  \px5    ; bit5: Px5
	sbci  YH,      hi8(VRAM_TILES_H * 2)
	sbiw  XL,      (VRAM_TILES_H + 1)
	out   PIXOUT,  \px6    ; bit6: Px6
	lpm   ZL,      Z
	out   PIXOUT,  \px7    ; bit7: Px7
	rjmp  at_exit
.endm

at_t0:	AT_TAIL r10, r10, r10, r10, at_e0
at_t1:	AT_TAIL r11, r10, r10, r10, at_e0
at_e0:	AT_ENDC      r10, r10, r10
at_t2:	AT_TAIL r10, r11, r10, r10, at_e2
at_t3:	AT_TAIL r11, r11, r10, r10, at_e2
at_e2:	AT_ENDC      r11, r10, r10
at_t4:	AT_TAIL r10, r10, r11, r10, at_e4
at_t5:	AT_TAIL r11, r10, r11, r10, at_e4
at_e4:	AT_ENDC      r10, r11, r10
at_t6:	AT_TAIL r10, r11, r11, r10, at_e6
at_t7:	AT_TAIL r11, r11, r11, r10, at_e6
at_e6:	AT_ENDC      r11, r11, r10
at_t8:	AT_TAIL r10, r10, r10, r11, at_e8
at_t9:	AT_TAIL r11, r10, r10, r11, at_e8
at_e8:	AT_ENDC      r10, r10, r11
at_tA:	AT_TAIL r10, r11, r10, r11, at_eA
at_tB:	AT_TAIL r11, r11, r10, r11, at_eA
at_eA:	AT_ENDC      r11, r10, r11
at_tC:	AT_TAIL r10, r10, r11, r11, at_eC
at_tD:	AT_TAIL r11, r10, r11, r11, at_eC
at_eC:	AT_ENDC      r10, r11, r11
at_tE:	AT_TAIL r10, r11, r11, r11, at_eE
at_tF:	AT_TAIL r11, r11, r11, r11, at_eE
at_eE:	AT_ENDC      r11, r11, r11

at_m00:	AT_MIDL r10, r10, at_t0
at_m04:	AT_MIDL r11, r10, at_t0
at_m08:	AT_MIDL r10, r11, at_t0
at_m0C:	AT_MIDL r11, r11, at_t0
at_m10:	AT_MIDL r10, r10, at_t1
at_m14:	AT_MIDL r11, r10, at_t1
at_m18:	AT_MIDL r10, r11, at_t1
at_m1C:	AT_MIDL r11, r11, at_t1
at_m20:	AT_MIDL r10, r10, at_t2
at_m24:	AT_MIDL r11, r10, at_t2
at_m28:	AT_MIDL r10, r11, at_t2
at_m2C:	AT_MIDL r11, r11, at_t2
at_m30:	AT_MIDL r10, r10, at_t3
at_m34:	AT_MIDL r11, r10, at_t3
at_m38:	AT_MIDL r10, r11, at_t3
at_m3C:	AT_MIDL r11, r11, at_t3
at_m40:	AT_MIDL r10, r10, at_t4
at_m44:	AT_MIDL r11, r10, at_t4
at_m48:	AT_MIDL r10, r11, at_t4
at_m4C:	AT_MIDL r11, r11, at_t4
at_m50:	AT_MIDL r10, r10, at_t5
at_m54:	AT_MIDL r11, r10, at_t5
at_m58:	AT_MIDL r10, r11, at_t5
at_m5C:	AT_MIDL r11, r11, at_t5
at_m60:	AT_MIDL r10, r10, at_t6
at_m64:	AT_MIDL r11, r10, at_t6
at_m68:	AT_MIDL r10, r11, at_t6
at_m6C:	AT_MIDL r11, r11, at_t6
at_m70:	AT_MIDL r10, r10, at_t7
at_m74:	AT_MIDL r11, r10, at_t7
at_m78:	AT_MIDL r10, r11, at_t7
at_m7C:	AT_MIDL r11, r11, at_t7
at_m80:	AT_MIDL r10, r10, at_t8
at_m84:	AT_MIDL r11, r10, at_t8
at_m88:	AT_MIDL r10, r11, at_t8
at_m8C:	AT_MIDL r11, r11, at_t8
at_m90:	AT_MIDL r10, r10, at_t9
at_m94:	AT_MIDL r11, r10, at_t9
at_m98:	AT_MIDL r10, r11, at_t9
at_m9C:	AT_MIDL r11, r11, at_t9
at_mA0:	AT_MIDL r10, r10, at_tA
at_mA4:	AT_MIDL r11, r10, at_tA
at_mA8:	AT_MIDL r10, r11, at_tA
at_mAC:	AT_MIDL r11, r11, at_tA
at_mB0:	AT_MIDL r10, r10, at_tB
at_mB4:	AT_MIDL r11, r10, at_tB
at_mB8:	AT_MIDL r10, r11, at_tB
at_mBC:	AT_MIDL r11, r11, at_tB
at_mC0:	AT_MIDL r10, r10, at_tC
at_mC4:	AT_MIDL r11, r10, at_tC
at_mC8:	AT_MIDL r10, r11, at_tC
at_mCC:	AT_MIDL r11, r11, at_tC
at_mD0:	AT_MIDL r10, r10, at_tD
at_mD4:	AT_MIDL r11, r10, at_tD
at_mD8:	AT_MIDL r10, r11, at_tD
at_mDC:	AT_MIDL r11, r11, at_tD
at_mE0:	AT_MIDL r10, r10, at_tE
at_mE4:	AT_MIDL r11, r10, at_tE
at_mE8:	AT_MIDL r10, r11, at_tE
at_mEC:	AT_MIDL r11, r11, at_tE
at_mF0:	AT_MIDL r10, r10, at_tF
at_mF4:	AT_MIDL r11, r10, at_tF
at_mF8:	AT_MIDL r10, r11, at_tF
at_mFC:	AT_MIDL r11, r11, at_tF

at_b:	AT_HEAD r10, r10, at_m00
	AT_HEAD r11, r10, at_m00
	AT_HEAD r10, r11, at_m00
	AT_HEAD r11, r11, at_m00
	AT_HEAD r10, r10, at_m04
	AT_HEAD r11, r10, at_m04
	AT_HEAD r10, r11, at_m04
	AT_HEAD r11, r11, at_m04
	AT_HEAD r10, r10, at_m08
	AT_HEAD r11, r10, at_m08
	AT_HEAD r10, r11, at_m08
	AT_HEAD r11, r11, at_m08
	AT_HEAD r10, r10, at_m0C
	AT_HEAD r11, r10, at_m0C
	AT_HEAD r10, r11, at_m0C
	AT_HEAD r11, r11, at_m0C
	AT_HEAD r10, r10, at_m10
	AT_HEAD r11, r10, at_m10
	AT_HEAD r10, r11, at_m10
	AT_HEAD r11, r11, at_m10
	AT_HEAD r10, r10, at_m14
	AT_HEAD r11, r10, at_m14
	AT_HEAD r10, r11, at_m14
	AT_HEAD r11, r11, at_m14
	AT_HEAD r10, r10, at_m18
	AT_HEAD r11, r10, at_m18
	AT_HEAD r10, r11, at_m18
	AT_HEAD r11, r11, at_m18
	AT_HEAD r10, r10, at_m1C
	AT_HEAD r11, r10, at_m1C
	AT_HEAD r10, r11, at_m1C
	AT_HEAD r11, r11, at_m1C
	AT_HEAD r10, r10, at_m20
	AT_HEAD r11, r10, at_m20
	AT_HEAD r10, r11, at_m20
	AT_HEAD r11, r11, at_m20
	AT_HEAD r10, r10, at_m24
	AT_HEAD r11, r10, at_m24
	AT_HEAD r10, r11, at_m24
	AT_HEAD r11, r11, at_m24
	AT_HEAD r10, r10, at_m28
	AT_HEAD r11, r10, at_m28
	AT_HEAD r10, r11, at_m28
	AT_HEAD r11, r11, at_m28
	AT_HEAD r10, r10, at_m2C
	AT_HEAD r11, r10, at_m2C
	AT_HEAD r10, r11, at_m2C
	AT_HEAD r11, r11, at_m2C
	AT_HEAD r10, r10, at_m30
	AT_HEAD r11, r10, at_m30
	AT_HEAD r10, r11, at_m30
	AT_HEAD r11, r11, at_m30
	AT_HEAD r10, r10, at_m34
	AT_HEAD r11, r10, at_m34
	AT_HEAD r10, r11, at_m34
	AT_HEAD r11, r11, at_m34
	AT_HEAD r10, r10, at_m38
	AT_HEAD r11, r10, at_m38
	AT_HEAD r10, r11, at_m38
	AT_HEAD r11, r11, at_m38
	AT_HEAD r10, r10, at_m3C
	AT_HEAD r11, r10, at_m3C
	AT_HEAD r10, r11, at_m3C
	AT_HEAD r11, r11, at_m3C
	AT_HEAD r10, r10, at_m40
	AT_HEAD r11, r10, at_m40
	AT_HEAD r10, r11, at_m40
	AT_HEAD r11, r11, at_m40
	AT_HEAD r10, r10, at_m44
	AT_HEAD r11, r10, at_m44
	AT_HEAD r10, r11, at_m44
	AT_HEAD r11, r11, at_m44
	AT_HEAD r10, r10, at_m48
	AT_HEAD r11, r10, at_m48
	AT_HEAD r10, r11, at_m48
	AT_HEAD r11, r11, at_m48
	AT_HEAD r10, r10, at_m4C
	AT_HEAD r11, r10, at_m4C
	AT_HEAD r10, r11, at_m4C
	AT_HEAD r11, r11, at_m4C
	AT_HEAD r10, r10, at_m50
	AT_HEAD r11, r10, at_m50
	AT_HEAD r10, r11, at_m50
	AT_HEAD r11, r11, at_m50
	AT_HEAD r10, r10, at_m54
	AT_HEAD r11, r10, at_m54
	AT_HEAD r10, r11, at_m54
	AT_HEAD r11, r11, at_m54
	AT_HEAD r10, r10, at_m58
	AT_HEAD r11, r10, at_m58
	AT_HEAD r10, r11, at_m58
	AT_HEAD r11, r11, at_m58
	AT_HEAD r10, r10, at_m5C
	AT_HEAD r11, r10, at_m5C
	AT_HEAD r10, r11, at_m5C
	AT_HEAD r11, r11, at_m5C
	AT_HEAD r10, r10, at_m60
	AT_HEAD r11, r10, at_m60
	AT_HEAD r10, r11, at_m60
	AT_HEAD r11, r11, at_m60
	AT_HEAD r10, r10, at_m64
	AT_HEAD r11, r10, at_m64
	AT_HEAD r10, r11, at_m64
	AT_HEAD r11, r11, at_m64
	AT_HEAD r10, r10, at_m68
	AT_HEAD r11, r10, at_m68
	AT_HEAD r10, r11, at_m68
	AT_HEAD r11, r11, at_m68
	AT_HEAD r10, r10, at_m6C
	AT_HEAD r11, r10, at_m6C
	AT_HEAD r10, r11, at_m6C
	AT_HEAD r11, r11, at_m6C
	AT_HEAD r10, r10, at_m70
	AT_HEAD r11, r10, at_m70
	AT_HEAD r10, r11, at_m70
	AT_HEAD r11, r11, at_m70
	AT_HEAD r10, r10, at_m74
	AT_HEAD r11, r10, at_m74
	AT_HEAD r10, r11, at_m74
	AT_HEAD r11, r11, at_m74
	AT_HEAD r10, r10, at_m78
	AT_HEAD r11, r10, at_m78
	AT_HEAD r10, r11, at_m78
	AT_HEAD r11, r11, at_m78
	AT_HEAD r10, r10, at_m7C
	AT_HEAD r11, r10, at_m7C
	AT_HEAD r10, r11, at_m7C
	AT_HEAD r11, r11, at_m7C
	AT_HEAD r10, r10, at_m80
	AT_HEAD r11, r10, at_m80
	AT_HEAD r10, r11, at_m80
	AT_HEAD r11, r11, at_m80
	AT_HEAD r10, r10, at_m84
	AT_HEAD r11, r10, at_m84
	AT_HEAD r10, r11, at_m84
	AT_HEAD r11, r11, at_m84
	AT_HEAD r10, r10, at_m88
	AT_HEAD r11, r10, at_m88
	AT_HEAD r10, r11, at_m88
	AT_HEAD r11, r11, at_m88
	AT_HEAD r10, r10, at_m8C
	AT_HEAD r11, r10, at_m8C
	AT_HEAD r10, r11, at_m8C
	AT_HEAD r11, r11, at_m8C
	AT_HEAD r10, r10, at_m90
	AT_HEAD r11, r10, at_m90
	AT_HEAD r10, r11, at_m90
	AT_HEAD r11, r11, at_m90
	AT_HEAD r10, r10, at_m94
	AT_HEAD r11, r10, at_m94
	AT_HEAD r10, r11, at_m94
	AT_HEAD r11, r11, at_m94
	AT_HEAD r10, r10, at_m98
	AT_HEAD r11, r10, at_m98
	AT_HEAD r10, r11, at_m98
	AT_HEAD r11, r11, at_m98
	AT_HEAD r10, r10, at_m9C
	AT_HEAD r11, r10, at_m9C
	AT_HEAD r10, r11, at_m9C
	AT_HEAD r11, r11, at_m9C
	AT_HEAD r10, r10, at_mA0
	AT_HEAD r11, r10, at_mA0
	AT_HEAD r10, r11, at_mA0
	AT_HEAD r11, r11, at_mA0
	AT_HEAD r10, r10, at_mA4
	AT_HEAD r11, r10, at_mA4
	AT_HEAD r10, r11, at_mA4
	AT_HEAD r11, r11, at_mA4
	AT_HEAD r10, r10, at_mA8
	AT_HEAD r11, r10, at_mA8
	AT_HEAD r10, r11, at_mA8
	AT_HEAD r11, r11, at_mA8
	AT_HEAD r10, r10, at_mAC
	AT_HEAD r11, r10, at_mAC
	AT_HEAD r10, r11, at_mAC
	AT_HEAD r11, r11, at_mAC
	AT_HEAD r10, r10, at_mB0
	AT_HEAD r11, r10, at_mB0
	AT_HEAD r10, r11, at_mB0
	AT_HEAD r11, r11, at_mB0
	AT_HEAD r10, r10, at_mB4
	AT_HEAD r11, r10, at_mB4
	AT_HEAD r10, r11, at_mB4
	AT_HEAD r11, r11, at_mB4
	AT_HEAD r10, r10, at_mB8
	AT_HEAD r11, r10, at_mB8
	AT_HEAD r10, r11, at_mB8
	AT_HEAD r11, r11, at_mB8
	AT_HEAD r10, r10, at_mBC
	AT_HEAD r11, r10, at_mBC
	AT_HEAD r10, r11, at_mBC
	AT_HEAD r11, r11, at_mBC
	AT_HEAD r10, r10, at_mC0
	AT_HEAD r11, r10, at_mC0
	AT_HEAD r10, r11, at_mC0
	AT_HEAD r11, r11, at_mC0
	AT_HEAD r10, r10, at_mC4
	AT_HEAD r11, r10, at_mC4
	AT_HEAD r10, r11, at_mC4
	AT_HEAD r11, r11, at_mC4
	AT_HEAD r10, r10, at_mC8
	AT_HEAD r11, r10, at_mC8
	AT_HEAD r10, r11, at_mC8
	AT_HEAD r11, r11, at_mC8
	AT_HEAD r10, r10, at_mCC
	AT_HEAD r11, r10, at_mCC
	AT_HEAD r10, r11, at_mCC
	AT_HEAD r11, r11, at_mCC
	AT_HEAD r10, r10, at_mD0
	AT_HEAD r11, r10, at_mD0
	AT_HEAD r10, r11, at_mD0
	AT_HEAD r11, r11, at_mD0
	AT_HEAD r10, r10, at_mD4
	AT_HEAD r11, r10, at_mD4
	AT_HEAD r10, r11, at_mD4
	AT_HEAD r11, r11, at_mD4
	AT_HEAD r10, r10, at_mD8
	AT_HEAD r11, r10, at_mD8
	AT_HEAD r10, r11, at_mD8
	AT_HEAD r11, r11, at_mD8
	AT_HEAD r10, r10, at_mDC
	AT_HEAD r11, r10, at_mDC
	AT_HEAD r10, r11, at_mDC
	AT_HEAD r11, r11, at_mDC
	AT_HEAD r10, r10, at_mE0
	AT_HEAD r11, r10, at_mE0
	AT_HEAD r10, r11, at_mE0
	AT_HEAD r11, r11, at_mE0
	AT_HEAD r10, r10, at_mE4
	AT_HEAD r11, r10, at_mE4
	AT_HEAD r10, r11, at_mE4
	AT_HEAD r11, r11, at_mE4
	AT_HEAD r10, r10, at_mE8
	AT_HEAD r11, r10, at_mE8
	AT_HEAD r10, r11, at_mE8
	AT_HEAD r11, r11, at_mE8
	AT_HEAD r10, r10, at_mEC
	AT_HEAD r11, r10, at_mEC
	AT_HEAD r10, r11, at_mEC
	AT_HEAD r11, r11, at_mEC
	AT_HEAD r10, r10, at_mF0
	AT_HEAD r11, r10, at_mF0
	AT_HEAD r10, r11, at_mF0
	AT_HEAD r11, r11, at_mF0
	AT_HEAD r10, r10, at_mF4
	AT_HEAD r11, r10, at_mF4
	AT_HEAD r10, r11, at_mF4
	AT_HEAD r11, r11, at_mF4
	AT_HEAD r10, r10, at_mF8
	AT_HEAD r11, r10, at_mF8
	AT_HEAD r10, r11, at_mF8
	AT_HEAD r11, r11, at_mF8
	AT_HEAD r10, r10, at_mFC
	AT_HEAD r11, r10, at_mFC
	AT_HEAD r10, r11, at_mFC
	AT_HEAD r11, r11, at_mFC



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
	sub   r24,     r0
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



;
; void SetTileTable(unsigned char const* data);
;
; Uzebox kernel function: sets the address of the tileset to use. The tileset
; must start on a 256 byte boundary! (Use __attribute__ ((aligned(256))) on
; tileset definitions)
;
.section .text.SetTileTable
SetTileTable:

	ldi   ZL,      lo8(v_tbase)
	ldi   ZH,      hi8(v_tbase)
	ldi   r24,     VRAM_TILES_V
	st    Z+,      r25     ; Save only high byte, low is assumed aligned
	dec   r24
	brne  .-6
	ret



;
; void SetTileTableRow(unsigned char const* data, unsigned char row);
;
; Sets tileset to use or 3bpp (0xFExx) / 1bpp (0xFFxx) mode for a given row.
; The tileset must start on a 256 byte boundary!
;
.section .text.SetTileTableRow
SetTileTableRow:

	cpi   r22,     VRAM_TILES_V
	brcc  .+12
	ldi   ZL,      lo8(v_tbase)
	ldi   ZH,      hi8(v_tbase)
	clr   r23
	add   ZL,      r22
	adc   ZH,      r23
	st    Z,       r25     ; Save only high byte, low is assumed aligned
	ret



;
; void SetBorderColor(unsigned char col);
;
; Sets the border color.
;
.section .text.SetBorderColor
SetBorderColor:

	sts   v_border, r24
	ret



;
; void PutPixel(unsigned char x, unsigned char y, unsigned char color);
;
; Puts a pixel at the given X:Y location. This is only valid for the 1bpp and
; the 3bpp row modes, in other row modes the call is ignored.
;
.section .text.PutPixel
PutPixel:

	mov   r23,     r22
	andi  r22,     0x03    ; Line select within tile row
	lsr   r23
	lsr   r23              ; Tile row select
	cpi   r23,     VRAM_TILES_V
	brcc  pp_exit

	; Precalculate VRAM & ARAM rows

	ldi   r21,     40
	mul   r23,     r21
	movw  XL,      r0      ; VRAM position
	clr   r1
	ldi   ZL,      lo8(v_tbase)
	ldi   ZH,      hi8(v_tbase)
	add   ZL,      r23
	adc   ZH,      r1
	ld    r25,     Z       ; Tile row type
	movw  ZL,      XL
	lsl   ZL
	rol   ZH               ; ARAM position
	subi  ZL,      lo8(-(aram))
	sbci  ZH,      hi8(-(aram))

	; Select row type

	cpi   r25,     0xFE
	breq  pp_3bpp
	brcs  pp_exit          ; < 0xFE: Character row, ignore

	; 1bpp mode: 160 pixels wide

	cpi   r24,     160
	brcc  pp_exit
	ldi   r21,     1       ; Pixel mask
	lsr   r24
	brcc  .+2
	lsl   r21
	lsr   r24              ; 0 - 39, X tile position
	brcc  .+4
	lsl   r21
	lsl   r21
	sbrc  r22,     0
	swap  r21
	sbrc  r22,     1
	adiw  ZL,      1
	lsl   r24
	add   ZL,      r24
	adc   ZH,      r1      ; Position of pixel got
	ld    r22,     Z
	sbrc  r20,     0
	or    r22,     r21
	com   r21
	sbrs  r20,     0
	and   r22,     r21
	st    Z,       r22

pp_exit:

	ret

pp_3bpp:

	; 3bpp mode: 80 pixels wide

	cpi   r24,     80
	brcc  pp_exit
	subi  XL,      lo8(-(vram))
	sbci  XH,      hi8(-(vram))
	ldi   r21,     1       ; Pixel mask
	lsr   r24
	brcc  .+2
	lsl   r21
	lsr   r22
	brcc  .+4
	lsl   r21
	lsl   r21
	sbrc  r22,     0
	swap  r21
	add   XL,      r24
	adc   XH,      r1
	lsl   r24
	add   ZL,      r24
	adc   ZH,      r1      ; Poisition of pixel got
	mov   r25,     r21
	com   r25              ; AND mask
	ld    r22,     Z
	sbrs  r20,     0
	and   r22,     r25
	sbrc  r20,     0
	or    r22,     r21
	st    Z+,      r22     ; Plane 0
	ld    r22,     Z
	sbrs  r20,     1
	and   r22,     r25
	sbrc  r20,     1
	or    r22,     r21
	st    Z,       r22     ; Plane 1
	ld    r22,     X
	sbrs  r20,     2
	and   r22,     r25
	sbrc  r20,     2
	or    r22,     r21
	st    X,       r22     ; Plane 2
	ret



;
; unsigned char GetPixel(unsigned char x, unsigned char y);
;
; Gets a pixel from the given X:Y location. This is only valid for the 1bpp
; and the 3bpp row modes, in other row modes it returns zero.
;
.section .text.GetPixel
GetPixel:

	mov   r23,     r22
	andi  r22,     0x03    ; Line select within tile row
	lsr   r23
	lsr   r23              ; Tile row select
	cpi   r23,     VRAM_TILES_V
	brcc  gp_exit

	; Precalculate VRAM & ARAM rows

	ldi   r21,     40
	mul   r23,     r21
	movw  XL,      r0      ; VRAM position
	clr   r1
	ldi   ZL,      lo8(v_tbase)
	ldi   ZH,      hi8(v_tbase)
	add   ZL,      r23
	adc   ZH,      r1
	ld    r25,     Z       ; Tile row type
	movw  ZL,      XL
	lsl   ZL
	rol   ZH               ; ARAM position
	subi  ZL,      lo8(-(aram))
	sbci  ZH,      hi8(-(aram))

	; Select row type

	cpi   r25,     0xFE
	breq  gp_3bpp
	brcs  gp_exit          ; < 0xFE: Character row, ignore

	; 1bpp mode: 160 pixels wide

	cpi   r24,     160
	brcc  gp_exit
	ldi   r21,     1       ; Pixel mask
	lsr   r24
	brcc  .+2
	lsl   r21
	lsr   r24              ; 0 - 39, X tile position
	brcc  .+4
	lsl   r21
	lsl   r21
	sbrc  r22,     0
	swap  r21
	sbrc  r22,     1
	adiw  ZL,      1
	lsl   r24
	add   ZL,      r24
	adc   ZH,      r1      ; Position of pixel got
	ldi   r24,     0
	ld    r22,     Z
	and   r22,     r21
	breq  .+2
	ori   r24,     1
	ret

gp_exit:

	ldi   r24,     0
	ret

gp_3bpp:

	; 3bpp mode: 80 pixels wide

	cpi   r24,     80
	brcc  gp_exit
	subi  XL,      lo8(-(vram))
	sbci  XH,      hi8(-(vram))
	ldi   r21,     1       ; Pixel mask
	lsr   r24
	brcc  .+2
	lsl   r21
	lsr   r22
	brcc  .+4
	lsl   r21
	lsl   r21
	sbrc  r22,     0
	swap  r21
	add   XL,      r24
	adc   XH,      r1
	lsl   r24
	add   ZL,      r24
	adc   ZH,      r1      ; Poisition of pixel got
	ldi   r24,     0
	ld    r22,     Z+
	and   r22,     r21
	breq  .+2
	ori   r24,     1
	ld    r22,     Z
	and   r22,     r21
	breq  .+2
	ori   r24,     2
	ld    r22,     X
	and   r22,     r21
	breq  .+2
	ori   r24,     4
	ret
