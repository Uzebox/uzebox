/*
 *  Uzebox Kernel - Mode 42
 *  Copyright (C) 2018 Sandor Zsuga (Jubatian)
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
; Video mode 42
;
; Attribute mode 40x25 text
;
; 320 pixels width (4 cycles / pixel)
; 8 pixels wide tiles, 40 tiles
; 8 pixels tall tiles
; 4 + 16 color palette for 2bpp modes and attributes
; Attribute RAM allowing definition of FG & BG color for each tile
; 40 + 40 byte RAM usage / tile row (VRAM, ARAM)
; No scrolling
; No sprites
;
; Alternative: 2bpp low-res Bitmap
;
; 80 pixels width (16 cycles / pixel), double scanning
; Bitplane linear layout, 1 plane in ARAM, 1 plane in VRAM
;
; Alternative: 1bpp mid-res Bitmap
;
; 160 pixels width (8 cycles / pixel), double scanning
; Pixels are taken from VRAM and ARAM.
;
; Different VRAM sizes are possible.
;
;=============================================================================

;
; unsigned char vram[];
;
; The Video RAM. Its size depends on the configuration in VideoMode42.def.h.
;
.global vram

;
; unsigned char aram[];
;
; The Attribute RAM. Its size depends on the configuration in
; VideoMode42.def.h.
;
.global aram

;
; unsigned char palette[];
;
; 20 bytes specifying the 4 colors used by the 2bpp low-res Bitmap mode and
; the 16 colors used by the attribute mode. The colors are in BBGGGRRR format
; (normal Uzebox colors).
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
; Sets tileset to use or 2bpp (0xFExx) / 1bpp (0xFFxx) mode for a given row.
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
; and the 2bpp row modes, in other row modes it returns zero.
;
.global GetPixel



#define PIXOUT VIDEO_PORT
#define STACKH 0x3E
#define STACKL 0x3D



.section .bss

	; Globals

.balign 4
	palette:       .space 20
.balign 1
	vram:          .space VRAM_SIZE
	aram:          .space VRAM_SIZE

	; Locals

	v_tbase:       .space VRAM_TILES_V ; Tileset base (address high)
	v_fbase:       .space 1            ; Font base for char output
	v_border:      .space 1            ; Border color

.section .text




;
; Video frame renderer
;

sub_video_mode42:

;
; Entry happens in cycle 467.
;

	; Load initial palette pointer for 1bpp / 2bpp

	ldi   XL,      lo8(palette) ; ( 468)
	ldi   XH,      hi8(palette) ; ( 469)
	movw  r2,      XL           ; ( 470) r3:r2: Palette

	; Prepare palette buffer

	ldi   XL,      lo8(palette + 4)
	ldi   XH,      hi8(palette + 4)
	ldi   ZL,      0
	ldi   ZH,      hi8(M42_PALBUF)
	mov   r0,      ZH      ; ( 475)
pbloop:
	ld    r1,      X+
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	st    Z+,      r1
	cp    ZH,      r0
	breq  pbloop           ; (1066) (37 * 16 - 1 = 591 for the loop)

	; Prepare scanline variables

	ldi   XL,      lo8(vram)
	ldi   XH,      hi8(vram)
	ldi   YL,      lo8(aram)
	ldi   YH,      hi8(aram)    ; (1070)
	lds   r14,     v_border     ; (1072) r14: Border color
	ldi   r20,     0            ; (1073) r20: Start line in VRAM
	ldi   r21,     (VRAM_TILES_V * TILE_HEIGHT) ; ( 478) r21: Line count to output from VRAM
	lds   r19,     render_lines_count
	mov   r22,     r19
	sub   r19,     r21     ; (1078)
	brcs  .+8
	mov   r18,     r19
	lsr   r18              ; (1081) Top border
	sub   r19,     r18     ; (1082) Bottom border
	rjmp  .+8              ; (1084)
	nop
	mov   r21,     r22     ; (1082) As many lines as available
	ldi   r18,     0       ; (1083) No top border
	ldi   r19,     0       ; (1084) No bottom border

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

	WAIT  ZL,      735     ; (1819)

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
	WAIT  ZL,      16
	out   PIXOUT,  r14     ; (20) Border begin
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
	clr   ZH
	subi  ZL,      lo8(-(v_tbase))
	sbci  ZH,      hi8(-(v_tbase))
	ld    r24,     Z       ; (25) Tile row configuration
	out   PIXOUT,  r14     ; (20 =>  1) Left border
	cpi   r24,     0xFE
	breq  b2_prep          ; ( 3 /  4) 0xFE: 2bpp
	brcc  b1_prep          ; ( 4 /  5) 0xFF: 1bpp

	; Attribute mode

	mov   r15,     r24
	add   r15,     r25     ; ( 6) Row select
	ldi   r16,     5
	in    r12,     STACKL
	in    r13,     STACKH  ; ( 9) Stack restoration value
	movw  r8,      YL      ; (10) Attribute RAM restoration value
	out   STACKL,  YL
	out   STACKH,  YH      ; (12) Stack pointer is used to read Attribute RAM
	ld    YL,      Y       ; (14) First attribute (pop pre-increments)
	ldi   YH,      hi8(M42_PALBUF)
	ld    ZL,      X+
	mov   ZH,      r15     ; (18) r15: Row select
	lpm   r0,      Z
	mul   r0,      r16     ; (23) r16: 5 (Size of AT_HEAD blocks in words)
	movw  ZL,      r0
	subi  ZH,      hi8(-(pm(at_b)))
	ld    r10,     Y       ; (27) BG color
	swap  YL
	ld    r11,     Y       ; (30) FG color
	ijmp                   ; (32) Enter scanline loop

scl_bbor:

	cpi   r19,     0       ; ( 7)
	breq  scl_end          ; ( 8 /  9)

	; Bottom border

	dec   r19              ; ( 9)
	WAIT  ZL,      10
	out   PIXOUT,  r14     ; (20) Border begin
	rjmp  co_exit_nr

b2_prep:

	; 2bpp mode

	lpm   ZL,      Z       ; ( 7)
	lpm   ZL,      Z       ; (10)
	lpm   ZL,      Z       ; (13)
	movw  ZL,      r2      ; (14) Load palette pointer
	sbrs  r25,     2
	rjmp  b2_row0123
	nop
	sbrs  r25,     1
	rjmp  b2_row45
	nop
	rjmp  b26_entry        ; (22)
b2_row45:
	rjmp  b24_entry        ; (22)
b2_row0123:
	sbrs  r25,     1
	rjmp  b2_row01
	nop
	rjmp  b22_entry        ; (22)
b2_row01:
	rjmp  b20_entry        ; (22)

b1_prep:

	; 1bpp mode

	lpm   ZL,      Z       ; ( 8)
	lpm   ZL,      Z       ; (11)
	lpm   ZL,      Z       ; (14)
	movw  ZL,      r2      ; (15) Load palette pointer
	ld    r10,     Z+      ; (17) Load BG color
	ld    r11,     Z+      ; (19) Load FG color
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
	rjmp  .+4
	adiw  YL,      (VRAM_TILES_H)
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
;     r11: FG color
; r13:r12: Temp for colors
;     r14: Border color
;     r17: Count of tiles to output
;  YH: YL: Attribute RAM pointer
;  XH: XL: Video RAM pointer
;
; Video data:
; ARAM: Row0: b0,0-3; Row1: b0,4-7
; VRAM: Row2: b1,0-3; Row3: b1,4-7
;
b10_mid:
	mov   r1,      r10
	sbrc  r0,      1
	mov   r1,      r11
	mov   r12,     r10
	out   PIXOUT,  r1
	sbrc  r0,      2
	mov   r12,     r11
	mov   r13,     r10
	sbrc  r0,      3
	mov   r13,     r11
	rjmp  .
	out   PIXOUT,  r12
	ret

b12_mid:
	mov   r1,      r10
	sbrc  r0,      5
	mov   r1,      r11
	mov   r12,     r10
	out   PIXOUT,  r1
	sbrc  r0,      6
	mov   r12,     r11
	mov   r13,     r10
	sbrc  r0,      7
	mov   r13,     r11
	rjmp  .
	out   PIXOUT,  r12
	ret

b10_loop:
	ld    r0,      Y+
	mov   r1,      r10
	sbrc  r0,      0
	mov   r1,      r11
	out   PIXOUT,  r1
	rcall b10_mid
	adiw  XL,      1
	dec   r17
	out   PIXOUT,  r13
	brne  b10_loop
	rjmp  b1_exit

b12_loop:
	ld    r0,      Y+
	mov   r1,      r10
	sbrc  r0,      4
	mov   r1,      r11
	out   PIXOUT,  r1
	rcall b12_mid
	adiw  XL,      1
	dec   r17
	out   PIXOUT,  r13
	brne  b12_loop
	rjmp  b1_exit

b14_loop:
	ld    r0,      X+
	mov   r1,      r10
	sbrc  r0,      0
	mov   r1,      r11
	out   PIXOUT,  r1
	rcall b10_mid
	adiw  YL,      1
	dec   r17
	out   PIXOUT,  r13
	brne  b14_loop
	rjmp  b1_exit

b16_loop:
	ld    r0,      X+
	mov   r1,      r10
	sbrc  r0,      4
	mov   r1,      r11
	out   PIXOUT,  r1
	rcall b12_mid
	adiw  YL,      1
	dec   r17
	out   PIXOUT,  r13
	brne  b16_loop
	rjmp  b1_exit

b1_exit:
	sbiw  XL,      VRAM_TILES_H
	sbiw  YL,      VRAM_TILES_H
	out   PIXOUT,  r14
	rjmp  co_exit



;
; Scanline loop core for the 2bpp Bitmap mode (2x4 px char blocks)
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
; Color bit0: ARAM
; Color bit1: VRAM
; Each byte encodes a 2x4 pixel block, bit0 for the upper left px, bit 7 for
; the lower right (so essentially this is still a char. mode which can
; integrate fine with the normal character mode).
;
b20_loop:
	nop
	out   PIXOUT,  r11
	rjmp  .
	lpm   r0,      Z
b20_entry:
	ld    r0,      Y+
	ld    r1,      X+
	bst   r0,      0
	bld   ZL,      0
	bst   r1,      0
	bld   ZL,      1
	ld    r11,     Z
	out   PIXOUT,  r11
	bst   r0,      1
	bld   ZL,      0
	bst   r1,      1
	bld   ZL,      1
	ld    r11,     Z
	rjmp  .
	lpm   r0,      Z
	dec   r17
	brne  b20_loop
	rjmp  b2_exit

b22_loop:
	nop
	out   PIXOUT,  r11
	rjmp  .
	lpm   r0,      Z
b22_entry:
	ld    r0,      Y+
	ld    r1,      X+
	bst   r0,      2
	bld   ZL,      0
	bst   r1,      2
	bld   ZL,      1
	ld    r11,     Z
	out   PIXOUT,  r11
	bst   r0,      3
	bld   ZL,      0
	bst   r1,      3
	bld   ZL,      1
	ld    r11,     Z
	rjmp  .
	lpm   r0,      Z
	dec   r17
	brne  b22_loop
	rjmp  b2_exit

b24_loop:
	nop
	out   PIXOUT,  r11
	rjmp  .
	lpm   r0,      Z
b24_entry:
	ld    r0,      Y+
	ld    r1,      X+
	bst   r0,      4
	bld   ZL,      0
	bst   r1,      4
	bld   ZL,      1
	ld    r11,     Z
	out   PIXOUT,  r11
	bst   r0,      5
	bld   ZL,      0
	bst   r1,      5
	bld   ZL,      1
	ld    r11,     Z
	rjmp  .
	lpm   r0,      Z
	dec   r17
	brne  b24_loop
	rjmp  b2_exit

b26_loop:
	nop
	out   PIXOUT,  r11
	rjmp  .
	lpm   r0,      Z
b26_entry:
	ld    r0,      Y+
	ld    r1,      X+
	bst   r0,      6
	bld   ZL,      0
	bst   r1,      6
	bld   ZL,      1
	ld    r11,     Z
	out   PIXOUT,  r11
	bst   r0,      7
	bld   ZL,      0
	bst   r1,      7
	bld   ZL,      1
	ld    r11,     Z
	rjmp  .
	lpm   r0,      Z
	dec   r17
	brne  b26_loop
	rjmp  b2_exit

b2_exit:
	out   PIXOUT,  r11
	lpm   ZL,      Z
	lpm   ZL,      Z
	lpm   ZL,      Z
	rjmp  b1_exit



;
; Scanline loop core for the Attribute mode
;
; Register allocation:
;
;  r1: r0: Temp (multiplication)
;  r9: r8: Attribute RAM restoration value (into YH:YL)
;     r10: BG color
;     r11: FG color (temp)
; r13:r12: Stack restoration address
;     r14: Border color
;     r15: ROM Row select for character images
;     r16: 5 (Size of AT_HEAD blocks in words)
;     r17: Count of tiles to output
;      YH: Palette buffer bank
;  XH: XL: Video RAM pointer (Preload 1st tile for entry)
;  ZH: ZL: Temp (load progmem, indirect jump)
;   STACK: Attribute RAM pointer (Preload 1st tile for entry)
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
.macro AT_MIDL px2, px3, px4, px5, px6, px7, endc
	dec   r17              ; r17: Tilecount
	out   PIXOUT,  \px2    ; bit2: Px2
	lpm   r0,      Z
	out   PIXOUT,  \px3    ; bit3: Px3
	breq  \endc
	mul   r0,      r16     ; r16: 5 (Size of AT_HEAD blocks in words)
	out   PIXOUT,  \px4    ; bit4: Px4
	movw  ZL,      r0
	pop   YL               ; Color pair
	out   PIXOUT,  \px5    ; bit5: Px5
	subi  ZH,      hi8(-(pm(at_b)))
	ld    r0,      Y       ; BG color
	out   PIXOUT,  \px6    ; bit6: Px6
	swap  YL
	ld    r1,      Y       ; FG color
	out   PIXOUT,  \px7    ; bit7: Px7
	movw  r10,     r0      ; Copy FG & BG colors to r11
	ijmp
.endm
.macro AT_ENDC px4, px5, px6, px7
	out   STACKL,  r12     ; r13:r12: saved stack pointer
	out   PIXOUT,  \px4    ; bit4: Px4
	out   STACKH,  r13
	rjmp  .
	out   PIXOUT,  \px5    ; bit5: Px5
	movw  YL,      r8      ; r9:r8: saved attribute RAM pointer
	sbiw  XL,      (VRAM_TILES_H + 1)
	out   PIXOUT,  \px6    ; bit6: Px6
	lpm   ZL,      Z
	out   PIXOUT,  \px7    ; bit7: Px7
	rjmp  at_exit
.endm

at_m00:	AT_MIDL r10, r10, r10, r10, r10, r10, at_e0
at_m04:	AT_MIDL r11, r10, r10, r10, r10, r10, at_e0
at_e0:	AT_ENDC           r10, r10, r10, r10
at_m08:	AT_MIDL r10, r11, r10, r10, r10, r10, at_e0
at_m0C:	AT_MIDL r11, r11, r10, r10, r10, r10, at_e0
at_m10:	AT_MIDL r10, r10, r11, r10, r10, r10, at_e1
at_m14:	AT_MIDL r11, r10, r11, r10, r10, r10, at_e1
at_e1:	AT_ENDC           r11, r10, r10, r10
at_m18:	AT_MIDL r10, r11, r11, r10, r10, r10, at_e1
at_m1C:	AT_MIDL r11, r11, r11, r10, r10, r10, at_e1
at_m20:	AT_MIDL r10, r10, r10, r11, r10, r10, at_e2
at_m24:	AT_MIDL r11, r10, r10, r11, r10, r10, at_e2
at_e2:	AT_ENDC           r10, r11, r10, r10
at_m28:	AT_MIDL r10, r11, r10, r11, r10, r10, at_e2
at_m2C:	AT_MIDL r11, r11, r10, r11, r10, r10, at_e2
at_m30:	AT_MIDL r10, r10, r11, r11, r10, r10, at_e3
at_m34:	AT_MIDL r11, r10, r11, r11, r10, r10, at_e3
at_e3:	AT_ENDC           r11, r11, r10, r10
at_m38:	AT_MIDL r10, r11, r11, r11, r10, r10, at_e3
at_m3C:	AT_MIDL r11, r11, r11, r11, r10, r10, at_e3
at_m40:	AT_MIDL r10, r10, r10, r10, r11, r10, at_e4
at_m44:	AT_MIDL r11, r10, r10, r10, r11, r10, at_e4
at_e4:	AT_ENDC           r10, r10, r11, r10
at_m48:	AT_MIDL r10, r11, r10, r10, r11, r10, at_e4
at_m4C:	AT_MIDL r11, r11, r10, r10, r11, r10, at_e4
at_m50:	AT_MIDL r10, r10, r11, r10, r11, r10, at_e5
at_m54:	AT_MIDL r11, r10, r11, r10, r11, r10, at_e5
at_e5:	AT_ENDC           r11, r10, r11, r10
at_m58:	AT_MIDL r10, r11, r11, r10, r11, r10, at_e5
at_m5C:	AT_MIDL r11, r11, r11, r10, r11, r10, at_e5
at_m60:	AT_MIDL r10, r10, r10, r11, r11, r10, at_e6
at_m64:	AT_MIDL r11, r10, r10, r11, r11, r10, at_e6
at_e6:	AT_ENDC           r10, r11, r11, r10
at_m68:	AT_MIDL r10, r11, r10, r11, r11, r10, at_e6
at_m6C:	AT_MIDL r11, r11, r10, r11, r11, r10, at_e6
at_m70:	AT_MIDL r10, r10, r11, r11, r11, r10, at_e7
at_m74:	AT_MIDL r11, r10, r11, r11, r11, r10, at_e7
at_e7:	AT_ENDC           r11, r11, r11, r10
at_m78:	AT_MIDL r10, r11, r11, r11, r11, r10, at_e7
at_m7C:	AT_MIDL r11, r11, r11, r11, r11, r10, at_e7
at_m80:	AT_MIDL r10, r10, r10, r10, r10, r11, at_e8
at_m84:	AT_MIDL r11, r10, r10, r10, r10, r11, at_e8
at_e8:	AT_ENDC           r10, r10, r10, r11
at_m88:	AT_MIDL r10, r11, r10, r10, r10, r11, at_e8
at_m8C:	AT_MIDL r11, r11, r10, r10, r10, r11, at_e8
at_m90:	AT_MIDL r10, r10, r11, r10, r10, r11, at_e9
at_m94:	AT_MIDL r11, r10, r11, r10, r10, r11, at_e9
at_e9:	AT_ENDC           r11, r10, r10, r11
at_m98:	AT_MIDL r10, r11, r11, r10, r10, r11, at_e9
at_m9C:	AT_MIDL r11, r11, r11, r10, r10, r11, at_e9
at_mA0:	AT_MIDL r10, r10, r10, r11, r10, r11, at_eA
at_mA4:	AT_MIDL r11, r10, r10, r11, r10, r11, at_eA
at_eA:	AT_ENDC           r10, r11, r10, r11
at_mA8:	AT_MIDL r10, r11, r10, r11, r10, r11, at_eA
at_mAC:	AT_MIDL r11, r11, r10, r11, r10, r11, at_eA
at_mB0:	AT_MIDL r10, r10, r11, r11, r10, r11, at_eB
at_mB4:	AT_MIDL r11, r10, r11, r11, r10, r11, at_eB
at_eB:	AT_ENDC           r11, r11, r10, r11
at_mB8:	AT_MIDL r10, r11, r11, r11, r10, r11, at_eB
at_mBC:	AT_MIDL r11, r11, r11, r11, r10, r11, at_eB
at_mC0:	AT_MIDL r10, r10, r10, r10, r11, r11, at_eC
at_mC4:	AT_MIDL r11, r10, r10, r10, r11, r11, at_eC
at_eC:	AT_ENDC           r10, r10, r11, r11
at_mC8:	AT_MIDL r10, r11, r10, r10, r11, r11, at_eC
at_mCC:	AT_MIDL r11, r11, r10, r10, r11, r11, at_eC
at_mD0:	AT_MIDL r10, r10, r11, r10, r11, r11, at_eD
at_mD4:	AT_MIDL r11, r10, r11, r10, r11, r11, at_eD
at_eD:	AT_ENDC           r11, r10, r11, r11
at_mD8:	AT_MIDL r10, r11, r11, r10, r11, r11, at_eD
at_mDC:	AT_MIDL r11, r11, r11, r10, r11, r11, at_eD
at_mE0:	AT_MIDL r10, r10, r10, r11, r11, r11, at_eE
at_mE4:	AT_MIDL r11, r10, r10, r11, r11, r11, at_eE
at_eE:	AT_ENDC           r10, r11, r11, r11
at_mE8:	AT_MIDL r10, r11, r10, r11, r11, r11, at_eE
at_mEC:	AT_MIDL r11, r11, r10, r11, r11, r11, at_eE
at_mF0:	AT_MIDL r10, r10, r11, r11, r11, r11, at_eF
at_mF4:	AT_MIDL r11, r10, r11, r11, r11, r11, at_eF
at_eF:	AT_ENDC           r11, r11, r11, r11
at_mF8:	AT_MIDL r10, r11, r11, r11, r11, r11, at_eF
at_mFC:	AT_MIDL r11, r11, r11, r11, r11, r11, at_eF

.balign 512

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

.balign 2



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
	movw  ZL,      XL      ; ARAM position
	subi  ZL,      lo8(-(aram))
	sbci  ZH,      hi8(-(aram))
	subi  XL,      lo8(-(vram))
	sbci  XH,      hi8(-(vram))

	; Select row type

	cpi   r25,     0xFE
	breq  pp_2bpp
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
	movw  ZL,      XL
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

pp_2bpp:

	; 2bpp mode: 80 pixels wide

	cpi   r24,     80
	brcc  pp_exit
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
	add   ZL,      r24
	adc   ZH,      r1      ; Poisition of pixel got
	mov   r25,     r21
	com   r25              ; AND mask
	ld    r22,     Z
	sbrs  r20,     0
	and   r22,     r25
	sbrc  r20,     0
	or    r22,     r21
	st    Z,       r22     ; Plane 0
	ld    r22,     X
	sbrs  r20,     1
	and   r22,     r25
	sbrc  r20,     1
	or    r22,     r21
	st    X,       r22     ; Plane 1
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
	movw  ZL,      XL      ; ARAM position
	subi  ZL,      lo8(-(aram))
	sbci  ZH,      hi8(-(aram))
	subi  XL,      lo8(-(vram))
	sbci  XH,      hi8(-(vram))

	; Select row type

	cpi   r25,     0xFE
	breq  gp_2bpp
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
	movw  ZL,      XL
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

gp_2bpp:

	; 2bpp mode: 80 pixels wide

	cpi   r24,     80
	brcc  gp_exit
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
	add   ZL,      r24
	adc   ZH,      r1      ; Poisition of pixel got
	ldi   r24,     0
	ld    r22,     Z
	and   r22,     r21
	breq  .+2
	ori   r24,     1
	ld    r22,     X
	and   r22,     r21
	breq  .+2
	ori   r24,     2
	ret
