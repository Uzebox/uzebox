/*
 *  Uzebox Kernel - Mode 80
 *  Copyright (C) 2019 Sandor Zsuga (Jubatian)
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
; Video mode 80
;
; Real-time code generated tile data
;
; 80 tiles width (17 cycles / tile)
; Up to 16 colors, first 2 colors have option for in-line swaps
; Tiles have fixed colors selected from the 16 of the palette
; No scrolling
; No sprites
;
;=============================================================================
;
; For description on how the scanline core works, look into the generator
; (generators/tilegen.c). It describes how it works and how it would be
; assembled normally.
;
;=============================================================================


;
; unsigned char vram[];
;
; The Video RAM. Its size depends on the configuration in VideoMode80.def.h.
;
.global vram

;
; unsigned char* m80_bgclist;
;
; Background color list. If non-NULL, on every line, the background color is
; reloaded from this list.
;
.global m80_bgclist

;
; unsigned char* m80_fgclist;
;
; Foreground color list. If non-NULL, on every line, the foreground color is
; reloaded from this list (foreground color is color 1).
;
.global m80_fgclist

;
; unsigned char const* m80_rompal;
;
; ROM palette. If non-NULL, 16 colors are loaded from this at frame start.
;
.global m80_rompal

;
; unsigned char* m80_rampal;
;
; RAM palette. If non-NULL, 16 colors are loaded from this at frame start.
; Higher priority than m80_rompal (so could be used for temporary override)
;
.global m80_rampal

;
; m80_dlist_tdef* m80_dlist;
;
; Display list providing screen split and vertical scrolling options.
;
.global m80_dlist

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
; Character set to use. Later will receive some better solution for adding.
;
#include "m80_cp437.s"
;#include "m80_mode9.s"



.section .bss

	; Globals

	vram:          .space VRAM_SIZE
	m80_bgclist:   .space 2
	m80_fgclist:   .space 2
	m80_dlist:     .space 2
	m80_rompal:    .space 2
	m80_rampal:    .space 2

	; Locals

	v_fbase:       .space 1 ; Font base address (space char. in font)

.section .text



;
; Default palette
;
m80_defpal:
	.byte 0x00, 0xFF, 0xC0, 0x38, 0xF8, 0x07, 0xC7, 0x3F
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00



;
; Video frame renderer
;
; Register usage:
;
;  r1: r0: Temporaries, used within scanline code for multiplication
;  r2-r17: Palette colors, r2: Background
;     r18: Temporary, used to load tile in scanline loop
;     r19: Contains current row code block high word
;     r20: Set to 4, size of code blocks in scanline loop
;     r21: Zero pixel for terminating the line
;     r22: Line counter
;     r23: Display list next scanline match
;     r24: Tile row counter
;     r25: Line within tile row counter
;       X: Used to read VRAM
;       Y: Display list address
;       Z: Used for jumps within scanline code
;

sub_video_mode80:

;
; Entry happens in cycle 467.
;

	; Prepare palette. Start off with defaults, override if palette
	; sources exist.

	lds   ZL,      m80_rompal + 0
	lds   ZH,      m80_rompal + 1
	ldi   YL,      lo8(m80_defpal)
	ldi   YH,      hi8(m80_defpal)
	cpi   ZH,      0
	brne  .+2
	movw  ZL,      YL
	lpm   r2,      Z+
	lpm   r3,      Z+
	lpm   r4,      Z+
	lpm   r5,      Z+
	lpm   r6,      Z+
	lpm   r7,      Z+
	lpm   r8,      Z+
	lpm   r9,      Z+
	lpm   r10,     Z+
	lpm   r11,     Z+
	lpm   r12,     Z+
	lpm   r13,     Z+
	lpm   r14,     Z+
	lpm   r15,     Z+
	lpm   r16,     Z+
	lpm   r17,     Z+
	lds   ZL,      m80_rampal + 0
	lds   ZH,      m80_rampal + 1
	cpi   ZH,      0
	brne  .+10
	ldi   ZL,      10
	dec   ZL
	brne  .-4
	nop
	rjmp  .+32
	ld    r2,      Z+
	ld    r3,      Z+
	ld    r4,      Z+
	ld    r5,      Z+
	ld    r6,      Z+
	ld    r7,      Z+
	ld    r8,      Z+
	ld    r9,      Z+
	ld    r10,     Z+
	ld    r11,     Z+
	ld    r12,     Z+
	ld    r13,     Z+
	ld    r14,     Z+
	ld    r15,     Z+
	ld    r16,     Z+
	ld    r17,     Z+

	; Prepare display list if any

	lds   YL,      m80_dlist + 0
	lds   YH,      m80_dlist + 1
	ldi   r23,     0       ; Display list next: Start
	cpi   YH,      0
	brne  .+2
	ldi   r23,     0xFF    ; Display list next: Never

	; Prepare counters

	ldi   r22,     0       ; Line Counter
	ldi   r24,     0       ; Tile Row counter (if no Display List)
	ldi   r25,     0       ; Line within Tile Row counter (if no Display List)

	; Prepare for Timer 1 use in the scanline loop for line termination

	ldi   ZL,      (1 << OCF1B) + (1 << OCF1A) + (1 << TOV1)
	sts   _SFR_MEM_ADDR(TIFR1), ZL  ; Clear any pending timer int

	ldi   ZL,      (0 << WGM12) + (1 << CS10)
	sts   _SFR_MEM_ADDR(TCCR1B), ZL ; Switch to timer1 normal mode (mode 0)

	ldi   ZL,      (1 << TOIE1)
	sts   _SFR_MEM_ADDR(TIMSK1), ZL ; Enable Overflow interrupt

	; Prepare constants

	ldi   r21,     0       ; Line terminating zero pixel
	ldi   r20,     M80_CODEBLOCK_SIZE ; Size of code blocks (heads)

	; Wait until next line

	WAIT  r18,     1184
	rjmp  scl_0

scl_nd:

	; No Display List processing filler

	lpm   ZL,      Z
	lpm   ZL,      Z
	lpm   ZL,      Z
	rjmp  .
	rjmp  .
	rjmp  scl_de

	; Scanline loop entry by Timer1 termination

.global TIMER1_OVF_vect
TIMER1_OVF_vect:

	out   PIXOUT,  r21     ; Zero pixel terminating the line

	pop   r0               ; pop & discard OVF interrupt return address
	pop   r0               ; pop & discard OVF interrupt return address

	; Tail wait

	WAIT  ZL,      (((1462 - (M80_TILE_CYCLES * SCREEN_TILES_H)) + 0) / 2) + 5

	; Entry point from lead-in

scl_0:

	; Display list processing when needed

	cp    r23,     r22
	brne  scl_nd
	ld    r24,     Y+      ; VRAM row to begin with
	cpi   r24,     VRAM_TILES_V
	brcs  .+2
	ldi   r24,     0
	ld    r25,     Y+      ; Tile row (within VRAM row) to begin at
	cpi   r25,     TILE_HEIGHT
	brcs  .+2
	ldi   r25,     0
	ld    r2,      Y+      ; Background color
	ld    r3,      Y+      ; Foreground color
	ld    r23,     Y+      ; (18 cycles) Next scanline to match
scl_de:

	; Background color from list if any

	lds   ZL,      m80_bgclist + 0
	lds   ZH,      m80_bgclist + 1
	add   ZL,      r22
	adc   ZH,      r21     ; r21: zero. If ptr. was NULL, ZH remains zero
	breq  .
	breq  .+2
	ld    r2,      Z       ; (10 cycles)

	; Foreground color from list if any

	lds   ZL,      m80_fgclist + 0
	lds   ZH,      m80_fgclist + 1
	add   ZL,      r22
	adc   ZH,      r21     ; r21: zero. If ptr. was NULL, ZH remains zero
	breq  .
	breq  .+2
	ld    r3,      Z       ; (10 cycles)

	; Check end of line

	lds   ZL,      render_lines_count
	cp    r22,     ZL
	breq  scl_0e
	rjmp  .
	rjmp  .
	lpm   ZL,      Z

	; Audio and Alignment (at cycle 1820 = 0 here)
	; The hsync_pulse routine clobbers r0, r1, Z and the T flag.

	rcall hsync_pulse      ; (21 + AUDIO)
	WAIT  ZL,      (((1462 - (M80_TILE_CYCLES * SCREEN_TILES_H)) + 1) / 2) + (HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES)

	; Enter code tile row

	; Prepare row variables

	ldi   XL,      lo8(vram)
	ldi   XH,      hi8(vram)
	ldi   ZL,      VRAM_TILES_H
	mul   ZL,      r24
	add   XL,      r0
	adc   XH,      r1      ; VRAM begin address to read
	mov   ZL,      r25
	ldi   ZH,      0
	subi  ZL,      lo8(-(m80_tilerows))
	sbci  ZH,      hi8(-(m80_tilerows))
	lpm   r19,     Z       ; Current row renderer select
	subi  r19,     hi8(-(pm(m80_tilerow_0)))
	inc   r25              ; Increment line within tile row
	cpi   r25,     TILE_HEIGHT
	brne  .+2
	ldi   r25,     0
	brne  .+2
	inc   r24              ; Increment tile row when passed previous
	cpi   r24,     VRAM_TILES_V
	brcs  .+2
	ldi   r24,     0
	inc   r22              ; Increment line counter

	; Prepare timer

	ldi   ZL,      lo8(0x10000 - ((M80_TILE_CYCLES * SCREEN_TILES_H) + 10))
	ldi   ZH,      hi8(0x10000 - ((M80_TILE_CYCLES * SCREEN_TILES_H) + 10))
	sts   _SFR_MEM_ADDR(TCNT1H), ZH
	sts   _SFR_MEM_ADDR(TCNT1L), ZL
	sei

	; Prepare for the tile row

	ld    r18,     X+
	mul   r18,     r20
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20

	; Enter tile row, will be terminated by timer

	ijmp

scl_0e:

	; Restore Timer 1 to the value it should normally have at this point

	ldi   r24,     hi8(101 - TIMER1_DISPLACE)
	sts   _SFR_MEM_ADDR(TCNT1H), r24
	ldi   r24,     lo8(101 - TIMER1_DISPLACE)
	sts   _SFR_MEM_ADDR(TCNT1L), r24

	rcall hsync_pulse      ; Last hsync, from now cycle precise part over.

	; Set vsync flag & flip field

	lds   ZL,      sync_flags
	ldi   r20,     SYNC_FLAG_FIELD
	ori   ZL,      SYNC_FLAG_VSYNC
	eor   ZL,      r20
	sts   sync_flags, ZL

	; Restore Timer 1's operation mode

	ldi   r24,     (1 << OCF1B) + (1 << OCF1A) + (1 << TOV1)
	sts   _SFR_MEM_ADDR(TIFR1), r24  ; Clear any pending timer int

	ldi   r24,     (1 << WGM12) + (1 << CS10)
	sts   _SFR_MEM_ADDR(TCCR1B), r24 ; Switch back to timer1 CTC mode (mode 4)

	ldi   r24,     (1 << OCIE1A)
	sts   _SFR_MEM_ADDR(TIMSK1), r24 ; Restore ints on compare match

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
