/*
 *  Uzebox Kernel - Mode 0 (Bootloader)
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

;*****************************************************************************
; Video Mode 0 Rasterizer and Functions
;*****************************************************************************
;
; Spec
; ----
; Type:         Tile-based
; Cycles/Pixel: 6
; Tile Width:   6
; Tile Height:  8
; Resolution:   240x224 pixels
; Sprites:      No
; Scrolling:    No
;
; Description
; -----------
; This is a 1bpp RAM tile only video mode, designed for the bootloader.
;
;*****************************************************************************


.global vram
.global tileset
.global pal_fg
.global pal_bg
.global linecol
.global ClearVram
.global SetFont
.global SetTile


#define CENTER_ADJUSTMENT 9
#define PIXOUT VIDEO_PORT


.section .bss

vram:
	.space VRAM_SIZE
tileset:
	.space TILESET_SIZE * 8
pal_fg:
	.space 16
pal_bg:
	.space 16
linecol:
	.space FRAME_LINES



.section .text


sub_video_mode0:

	; Waste line to align with next hsync in render function

	WAIT  r19,     1317

	; Prepare first scanline

	ldi   XL,      lo8(vram)
	ldi   XH,      hi8(vram)
	ldi   YL,      lo8(linecol)
	ldi   YH,      hi8(linecol)
	lds   r10,     render_lines_count
	ldi   r22,     TILE_HEIGHT ; Size of a tile
	ldi   r23,     0       ; Line counter within tile



next_tile_line:

	; Fetch foreground & background colors

	ld    ZL,      Y+
	mov   r18,     ZL
	andi  ZL,      0x0F    ; Bg. color
	clr   ZH
	subi  ZL,      lo8(-(pal_bg))
	sbci  ZH,      hi8(-(pal_bg))
	ld    r16,     Z
	mov   r17,     r16     ; Bg. color in r17 and r16
	mov   ZL,      r18
	swap  ZL
	andi  ZL,      0x0F    ; Fg. color
	clr   ZH
	subi  ZL,      lo8(-(pal_fg))
	sbci  ZH,      hi8(-(pal_fg))
	ld    r18,     Z       ; Fg. color in r18

	; Prepare tileset

	ldi   r20,     lo8(tileset)
	ldi   r21,     hi8(tileset)
	clr   r1
	add   r20,     r23     ; Add line counter within tile
	adc   r21,     r1

	; Prepare for scanline

	ldi   r19,     SCREEN_TILES_H + 1
	clr   r2
	clr   r3
	movw  r4,      r2
	movw  r6,      r2

	; Next Hsync

	rcall hsync_pulse

	WAIT  ZL,      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES + ((40 - SCREEN_TILES_H) * 18) + CENTER_ADJUSTMENT

	; Enter scanline loop (beginning with a "dummy" tile)

sc_loop:
	out   PIXOUT,  r2
	ld    r0,      X+
	mul   r0,      r22     ; 8: Size of a single tile
	add   r0,      r20
	out   PIXOUT,  r3
	adc   r1,      r21     ; r21:r20: Tileset base + Row select
	movw  ZL,      r0
	ld    r0,      Z
	dec   r19              ; Count of tiles left in r19 (+1)
	out   PIXOUT,  r4
	rjmp  .
	movw  r2,      r16     ; Bg. color in r17 and r16
	sbrc  r0,      7
	mov   r2,      r18     ; Fg. color in r18
	out   PIXOUT,  r5
	sbrc  r0,      6
	mov   r3,      r18     ; Fg. color in r18
	movw  r4,      r16     ; Bg. color in r17 and r16
	sbrc  r0,      5
	mov   r4,      r18     ; Fg. color in r18
	out   PIXOUT,  r6
	sbrc  r0,      4
	mov   r5,      r18     ; Fg. color in r18
	movw  r8,      r16     ; Bg. color in r17 and r16
	sbrc  r0,      3
	mov   r8,      r18     ; Fg. color in r18
	out   PIXOUT,  r7
	sbrc  r0,      2
	mov   r9,      r18     ; Fg. color in r18
	movw  r6,      r8
	brne  sc_loop
	clr   r2               ; End of line
	out   PIXOUT,  r2
	sbiw  XL,      SCREEN_TILES_H + 1

	; Scanline done, check end of frame

	WAIT  r19,     42 + ((40 - SCREEN_TILES_H) * 18) - CENTER_ADJUSTMENT

	dec   r10
	breq  frame_end

	inc   r23              ; Increment line counter within tile
	cpi   r23,     TILE_HEIGHT ; Last line of tile?
	brne  no_next_row
	clr   r23
	adiw  XL,      SCREEN_TILES_H
	rjmp  next_tile_line
no_next_row:
	rjmp  .-4

frame_end:

	WAIT  r19,     35

	rcall hsync_pulse

	; Set vsync flag & flip field

	lds   ZL,      sync_flags
	ldi   r20,     SYNC_FLAG_FIELD
	ori   ZL,      SYNC_FLAG_VSYNC
	eor   ZL,      r20
	sts   sync_flags, ZL

	; Clear any pending timer int

	ldi   ZL,      (1 << OCF1A)
	sts   _SFR_MEM_ADDR(TIFR1), ZL

	ret



;***********************************
; CLEAR VRAM
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearVram
ClearVram:

	ldi   ZL,      lo8(VRAM_SIZE)
	ldi   ZH,      hi8(VRAM_SIZE)
	ldi   XL,      lo8(vram)
	ldi   XH,      hi8(vram)
	clr   r1

ClearVram_loop:

	st    X+,      r1
	sbiw  ZL,      1
	brne  ClearVram_loop

	ret



;***********************************
; SET FONT TILE
; C-callable
; Inputs:
;     r24: X pos (8 bit)
;     r22: Y pos (8 bit)
;     r20: Font tile No (8 bit)
;************************************
.section .text.SetFont
SetFont:

	clr   r25
	ldi   r21,     VRAM_TILES_H
	mul   r22,     r21     ; Calculate Y line addr in vram
	add   r0,      r24     ; Add X offset
	adc   r1,      r25
	ldi   XL,      lo8(vram)
	ldi   XH,      hi8(vram)
	add   XL,      r0
	adc   XH,      r1
	st    X,       r20
	clr   r1
	ret



;***********************************
; SET TILE
; C-callable
; Inputs:
;     r24: X pos (8 bit)
;     r22: Y pos (8 bit)
; r21:r20: Tile No (16 bit)
;************************************
.section .text.SetTile
SetTile:

	jmp   SetFont
