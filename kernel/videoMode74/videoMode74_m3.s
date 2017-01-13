;
; Uzebox Kernel - Video Mode 74 Row mode 3 (2bpp Multicolor)
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

.section .text



;
; 2bpp Multicolor mode
;
; Enters in cycle 236. After doing its work, it returns to m74_scloop_sr,
; cycle 1697 of the next line.
;
; First pixel output in 24 tile wide mode has to be performed at cycle 353 (so
; OUT finishing in 354). The horizontal size reduction is already performed.
;
; VRAM layout:
;
; 0x00 - 0x7F: 6px wide 1bpp tiles (same as row mode 5)
; 0x80 - 0xBF: 8px wide 1bpp tiles
; 0xC0 - 0xFF: 8px wide 1bpp entry / filler tiles
;
; Tile row descriptor elements:
;
; From byte 0, bits 0 - 2 aren't used as the whole 2Kbytes 1bpp tileset is
; accessible.
; Bytes 1 and 2 are used as usual for 1bpp modes.
; Bytes 3 and 4 are not used.
;
; The 0xC0 - 0xFF range uses 2 VRAM bytes, the second byte indicating the
; number of 2bpp Multicolor tiles to generate after this tile. If it is zero,
; then it is a "filler" tile, taking 2 VRAM bytes like a 2bpp Multicolor tile.
;
; The 2bpp Multicolor tiles use a linear framebuffer with state (v_m3ptr)
; across rows (so they can't be scrolled vertically normally, although some
; tricks are possible). This pointer increments as tile rows are processed,
; so data is consumed wherever a multicolor tile is encountered. By this it is
; possible to create non-rectangular layouts.
;
; Attributes for it are taken from VRAM: 2 bytes for each tile, high nybble of
; first providing color 0 index.
;
; Horizontal scrolling is not supported at all, the related input is ignored.
;
; The first tile of a line can not be a 2bpp Multicolor tile. The last tile of
; a line must be one in the 0x00 - 0xBF range, and must not be a 2bpp
; Multicolor tile.
;
; r16: Scanline counter (increments it by one)
; r17: Logical row counter (increments it by one)
; r18: Number of pixels to generate
; r20: X shift (0 - 7)
; r19: 3 (Row mode)
; r22: Row width bits (0, 8, 16, 24 for tile counts 24, 22, 20, 18)
; r23: High 3 bits of byte 0 of descriptor
; r10: Byte 1 of tile descriptor
; r11: Byte 2 of tile descriptor
; r12: Byte 3 of tile descriptor
; r21: Byte 4 of tile descriptor
; X:   Offset from tile index list reduced by 1
; r9:  Global configuration (m74_config)
; r14: SD load address, low
; r15: SD load address, high
; YH:  Palette buffer, high
;
; Everything expect r14, r15, r16, r17, r22 and YH may be clobbered.
; Register r18 has to be set zero before return.
;
; Register usage in the output logic:
;
; r0,  r1:            Temporary for multiplication etc.
; r2,  r3,  r4,  r5:  Temporary color holding regs for pixel output
; r18, r19, r20, r21: Colors 0, 1, 2 and 3 fetched by attributes in MC output
; r8,  r9:            Temporary holding regs for colors 0 and 1 in MC output
; r6:                 0x05, used for calculating jump address in MC output
; r7:                 0xFF, used for an add in MC output
; r12:                Stores base offset low for code tile blk. for MC output
; r13:                Stores base offset high for code tile blk. for MC output
; r22:                MC output remaining tile counter
; X:                  Multicolor framebuffer pointer
; r24, r25:           Background color for 1bpp tiles
; SP:                 VRAM pointer (offset from tile index list used)
; r23:                Tile counter
; r11:                Foreground color for 1bpp tiles
; r10:                1bpp tiles offset high
;
; r22 is restored before return, it is stored on the stack.
;

m74_m3_2bppmc:
#if (M74_SD_ENABLE != 0)
	movw  ZL,      r14     ; ( 1) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (36) 35 cycles
	movw  r14,     ZL      ; (37) r15:r14, ZH:ZL Target pointer
	M74WT_R24      22      ; (59) (295)
#else
	M74WT_R24      59      ; (59) (295)
#endif



;
; Preparations for the main tile loop
;
; 36 cycles
;
	; Save row width (1 byte on stack)

	push  r22              ; ( 2)

	; Set up tile counter from count of pixels

	mov   r23,     r18     ; ( 3) Pixels: 192, 176, 160 or 144
	swap  r23              ; ( 4)          12,  11,  10,     9
	lsl   r23              ; ( 5) Tiles:   24,  22,  20 or  18

	; Apply row increment (256) on 1bpp offset

	mov   r24,     r17     ; ( 6)
	andi  r24,     0x07    ; ( 7)
	add   r10,     r24     ; ( 8)

	; Restore VRAM pointer (reverting the subtract applied due to stack)

	adiw  XL,      1       ; (10)

	; Load Multicolor framebuffer pointer, and apply double scan

	lds   ZL,      v_m3ptr_lo   ; (12)
	lds   ZH,      v_m3ptr_hi   ; (14)
#if (M74_M3_SUB == 0)
	M74WT_R24      7       ; (21)
#else
	mov   r24,     r16     ; (15) Use physical scanline counter for double scan
	subi  r24,     (M74_M3_SUBSL + 1) ; (16)
	brcs  m3cfgd0          ; (17 / 18)
	and   r24,     r9      ; (18) Physical row counter bit 0 aligned with m74_config bit
	sbrc  r24,     0       ; (19 / 20)
	subi  ZL,      M74_M3_SUB   ; (20) If this does not execute, carry is clear
	sbci  ZH,      0       ; (21)
m3cfgd1:
#endif
	out   STACKL,  ZL      ; (22)
	out   STACKH,  ZH      ; (23)

	; Pick colors from palette for 1bpp output

	mov   YL,      r11     ; (24)
	ld    r11,     Y       ; (26) Foreground (1) color
	swap  YL               ; (27)
	ld    r24,     Y       ; (29)
	mov   r25,     r24     ; (30) Background (0) color

	; Pre-load registers for the tile loop

	ldi   r18,     0x05    ; (31)
	ldi   r19,     0xFF    ; (32)
	movw  r6,      r18     ; (33) r7:r6, r19:r18 Load constants
	ldi   r18,     lo8(pm(m3pxblk)) ; (34)
	ldi   r19,     hi8(pm(m3pxblk)) ; (35)
	movw  r12,     r18     ; (36) r13:r12, r19:r18 Load base offset for code tiling



;
; Increment line counters
;
; 2 cycles
;
	inc   r16              ; ( 1) Increment the physical line counter
	inc   r17              ; ( 2) Increment the logical line counter



;
; Tile output lead-in. The first pixel must be produced at 353 (so the
; out ending at 354).
;

	clr   r4               ; ( 334)
	clr   r5               ; ( 335) Zero partial -1th tile
	rjmp  m3tlcom          ; ( 337) (12 of half-tile)



#if (M74_M3_SUB != 0)
;
; No double scan path for the double scan calculation
;
m3cfgd0:
	nop                    ; (20) No double-scan subtract path
	rjmp  m3cfgd1          ; (22)
#endif



;
; Exit from line render, going back onto the main scanline loop
;
m3tlend:
	clr   r18              ; (12) For the terminating pixel
	in    XL,      STACKL  ; (13)
	in    XH,      STACKH  ; (14) For saving framebuffer ptr.
	out   PIXOUT,  r4      ; (15) Pixel 6 of prev. tile
	ldi   r24,     lo8(M74_VIDEO_STACK + 14) ; (16) There is 1 byte on the stack
	ldi   r25,     hi8(M74_VIDEO_STACK + 14) ; (17)
	out   STACKL,  r24     ; (18)
	out   STACKH,  r25     ; (19) Restore stack
	pop   r22              ; (21) Pop off row width from it
	out   PIXOUT,  r5      ; (22) Pixel 7 of prev. tile
	sts   v_m3ptr_lo, XL   ; (24)
	sts   v_m3ptr_hi, XH   ; (26) Save framebuffer pointer
	rjmp  m74_scloop_sr    ; (28 / 1697)



;
; 8px wide 1bpp tiles
;
m3tl8:
	movw  r2,      r24     ; () r3:r2, r25:r24; BG color loads
	sbrc  r0,      1       ; ()
	mov   r2,      r11     ; ()
	dec   r23              ; () One tile less to go (zero test below)
	out   PIXOUT,  r5      ; (22) Pixel 3 of current tile
	sbrc  r0,      0       ; ()
	mov   r3,      r11     ; ()
	movw  r4,      r2      ; () r5:r4, r3:r2; Transfer to proper last two px regs
	lpm   r22,     Z       ; () Dummy load (nop)
	out   PIXOUT,  r8      ; ( 1) Pixel 4 of current tile
	lpm   r22,     Z       ; () Dummy load (nop)
	lpm   r22,     Z       ; () Dummy load (nop)
	out   PIXOUT,  r9      ; ( 8) Pixel 5 of current tile
	nop                    ; ( 9)
	breq  m3tlend          ; (10 / 11) Note that r23 is zero on exit, so restored
	rjmp  m3tlcom          ; (12)



;
; Tail of 6px wide tiles
;
m3tl6e:
	dec   r23              ; () One tile less to go (zero test below)
	lpm   r0,      Z       ; () Dummy load (nop)
	out   PIXOUT,  r5      ; (22) Pixel 3 of current tile
	movw  r4,      r8      ; () r5:r4, r9:r8, fix trailing pixels
	breq  m3tlend          ; (10 / 11) Note that r23 is zero on exit, so restored
	rjmp  m3tlcom          ; (12)



;
; Normal 6px or 8px wide 1bpp tiles, processing 6px wide tiles on tail.
;
m3tlnor:
	cpi   ZL,      0x80    ; (14) 8px / tile or 6px / tile comparison
	out   PIXOUT,  r4      ; (15) Pixel 2 of current tile
	brcc  m3tl8            ; () >= 0x80: 8px / tile
	ldi   r22,     2       ; ()
	subi  r23,     2       ; () 3 8px wide tiles will be consumed
m3tl6l:
	ld    ZL,      X+      ; ()
	subi  r22,     1       ; () Sets carry when all tiles done
	out   PIXOUT,  r5      ; (22) Pixel 3 of current tile
	lpm   r0,      Z       ; () Dummy load (nop)
	lpm   r0,      Z       ; () Dummy load (nop)
	out   PIXOUT,  r8      ; ( 1) Pixel 4 of current tile
	lpm   r0,      Z       ; () Dummy load (nop)
	lpm   r0,      Z       ; () Dummy load (nop)
	out   PIXOUT,  r9      ; ( 8) Pixel 5 of current tile
	lpm   r0,      Z       ; () This is the "real" pixel data load
	movw  r2,      r24     ; () r3:r2, r25:r24; BG color loads
	sbrc  r0,      7       ; ()
	mov   r2,      r11     ; ()
	out   PIXOUT,  r2      ; ( 1) Pixel 0 of current tile
	movw  r4,      r24     ; () r5:r4, r25:r24; BG color loads
	movw  r8,      r24     ; () r9:r8, r25:r24; BG color loads
	sbrc  r0,      6       ; ()
	mov   r3,      r11     ; ()
	sbrc  r0,      5       ; ()
	mov   r4,      r11     ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1 of current tile
	sbrc  r0,      4       ; ()
	mov   r5,      r11     ; ()
	sbrc  r0,      3       ; ()
	mov   r8,      r11     ; ()
	sbrc  r0,      2       ; ()
	mov   r9,      r11     ; ()
	out   PIXOUT,  r4      ; (15) Pixel 2 of current tile
	brcs  m3tl6e           ; ()
	rjmp  m3tl6l           ; ()



;
; Entry / filler tile processing, accompanying the 2bpp Multicolor main loop.
; Register r23 is used as tile counter. Note that line termination is only
; checked in the 0x00 - 0xBF tile range, so only those can exit. The "m3tlcom"
; is the main entry point for the tile loop.
;
m3tlef:
	ld    r22,     X+      ; () Count of multicolor tiles
	out   PIXOUT,  r4      ; (15) Pixel 2 of current tile (bg)
	ld    YL,      X+      ; ()
	ld    r18,     Y       ; ()
	swap  YL               ; ()
	dec   r23              ; () Subtract the current tile from total tiles
	out   PIXOUT,  r5      ; (22) Pixel 3 of current tile (bg)
	sub   r23,     r22     ; () Subtract multicolor tile count from total tiles
	movw  r4,      r24     ; () r5:r4, r25:r24; BG color loads
	sbrc  r0,      1       ; ()
	mov   r4,      r11     ; ()
	sbrc  r0,      0       ; ()
	mov   r5,      r11     ; ()
	out   PIXOUT,  r8      ; ( 1) Pixel 4 of current tile (bg)
	ld    r19,     Y       ; ()
	ld    YL,      X+      ; ()
	ld    r20,     Y       ; ()
	out   PIXOUT,  r9      ; ( 8) Pixel 5 of current tile
	cpi   r22,     0       ; ( 9)
	brne  m3pxentry        ; (10 / 11) Go for rendering 2bpp Multicolor if any
m3pxend:
	sbiw  XL,      2       ; (12) Attributes weren't needed or if falling out of 2bpp MC, then restore
m3tlcom:
	ld    ZL,      X+      ; (14)
	out   PIXOUT,  r4      ; (15) Pixel 6 of prev. tile
	mov   ZH,      r10     ; (16) Load ROM tile offset base
	lpm   r0,      Z       ; (19) Load pixel data for tile
	movw  r2,      r24     ; (20) r3:r2, r25:r24; BG color loads
	cpi   ZL,      0xC0    ; (21) Check for entry / filler tiles (Carry clear if so)
	out   PIXOUT,  r5      ; (22) Pixel 7 of prev. tile
	movw  r4,      r24     ; () r5:r4, r25:r24; BG color loads
	movw  r8,      r24     ; () r9:r8, r25:r24; BG color loads
	sbrc  r0,      7       ; ()
	mov   r2,      r11     ; ()
	sbrc  r0,      6       ; ()
	mov   r3,      r11     ; ()
	out   PIXOUT,  r2      ; ( 1) Pixel 0 of current tile
	sbrc  r0,      5       ; ()
	mov   r4,      r11     ; ()
	sbrc  r0,      4       ; ()
	mov   r5,      r11     ; ()
	sbrc  r0,      3       ; ()
	mov   r8,      r11     ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1 of current tile
	sbrc  r0,      2       ; ()
	mov   r9,      r11     ; ()
	brcc  m3tlef           ; (11 / 12)
	rjmp  m3tlnor          ; (13)



;
; 2bpp Multicolor main loop (2 halves)
;
m3pxcom:
	brcc  m3pxshalf        ; ( 4 /  5)
	movw  r18,     r8      ; ( 5) r19:r18, r9:r8; Move temp color 0 and 1
	ld    YL,      X+      ; ( 7) Continue loading attrs for coming tile (might be extra past end)
	out   PIXOUT,  r3      ; ( 8)
	breq  m3pxend          ; ( 9 / 10) r22 "decremented" to zero
	ld    r20,     Y       ; (11)
m3pxentry:
	swap  YL               ; (12)
	ld    r21,     Y       ; (14)
	out   PIXOUT,  r4      ; (15)
	pop   r0               ; (17)
	mul   r0,      r6      ; (19) r6 contains 0x05 for the assignment block size
	movw  ZL,      r12     ; (20) ZH:ZL, r13:r12, latter contains base for px. assign blocks
	add   ZL,      r0      ; (21)
	out   PIXOUT,  r5      ; (22) Prev. block color
	adc   ZH,      r1      ; (23) Here carry always becomes clear
	ijmp                   ; (25)

m3pxshalf:
	ld    YL,      X+      ; ( 7) Start loading attributes for next tile (might be extra past end)
	out   PIXOUT,  r3      ; ( 8)
	ld    r8,      Y       ; (10)
	pop   r0               ; (12)
	mul   r0,      r6      ; (14) r6 contains 5 for the assignment block size
	out   PIXOUT,  r4      ; (15)
	swap  YL               ; (16)
	ld    r9,      Y       ; (18)
	movw  ZL,      r12     ; (19) ZH:ZL, r13:r12, latter contains base for px. assign block
	add   ZL,      r0      ; (20)
	adc   ZH,      r1      ; (21)
	out   PIXOUT,  r5      ; (22) Prev. block color
	add   r22,     r7      ; (23) r7 contains 0xFF, always producing carry
	ijmp                   ; (25)



;
; 2,5K pixel assignment block (partial code tile). These sort out pixel
; assignments for a half-tile for the 256 possible input values (4 pixels,
; 2 bits for each pixel in high bits first format). One entry is 5
; instructions long (10 bytes).
;
.macro M3PXB c0, c1, c2, c3
	mov   r3,      \c1
	mov   r4,      \c2
	mov   r5,      \c3
	out   PIXOUT,  \c0
	rjmp  m3pxcom
.endm
m3pxblk:
	M3PXB r18, r18, r18, r18
	M3PXB r18, r18, r18, r19
	M3PXB r18, r18, r18, r20
	M3PXB r18, r18, r18, r21
	M3PXB r18, r18, r19, r18
	M3PXB r18, r18, r19, r19
	M3PXB r18, r18, r19, r20
	M3PXB r18, r18, r19, r21
	M3PXB r18, r18, r20, r18
	M3PXB r18, r18, r20, r19
	M3PXB r18, r18, r20, r20
	M3PXB r18, r18, r20, r21
	M3PXB r18, r18, r21, r18
	M3PXB r18, r18, r21, r19
	M3PXB r18, r18, r21, r20
	M3PXB r18, r18, r21, r21
	M3PXB r18, r19, r18, r18
	M3PXB r18, r19, r18, r19
	M3PXB r18, r19, r18, r20
	M3PXB r18, r19, r18, r21
	M3PXB r18, r19, r19, r18
	M3PXB r18, r19, r19, r19
	M3PXB r18, r19, r19, r20
	M3PXB r18, r19, r19, r21
	M3PXB r18, r19, r20, r18
	M3PXB r18, r19, r20, r19
	M3PXB r18, r19, r20, r20
	M3PXB r18, r19, r20, r21
	M3PXB r18, r19, r21, r18
	M3PXB r18, r19, r21, r19
	M3PXB r18, r19, r21, r20
	M3PXB r18, r19, r21, r21
	M3PXB r18, r20, r18, r18
	M3PXB r18, r20, r18, r19
	M3PXB r18, r20, r18, r20
	M3PXB r18, r20, r18, r21
	M3PXB r18, r20, r19, r18
	M3PXB r18, r20, r19, r19
	M3PXB r18, r20, r19, r20
	M3PXB r18, r20, r19, r21
	M3PXB r18, r20, r20, r18
	M3PXB r18, r20, r20, r19
	M3PXB r18, r20, r20, r20
	M3PXB r18, r20, r20, r21
	M3PXB r18, r20, r21, r18
	M3PXB r18, r20, r21, r19
	M3PXB r18, r20, r21, r20
	M3PXB r18, r20, r21, r21
	M3PXB r18, r21, r18, r18
	M3PXB r18, r21, r18, r19
	M3PXB r18, r21, r18, r20
	M3PXB r18, r21, r18, r21
	M3PXB r18, r21, r19, r18
	M3PXB r18, r21, r19, r19
	M3PXB r18, r21, r19, r20
	M3PXB r18, r21, r19, r21
	M3PXB r18, r21, r20, r18
	M3PXB r18, r21, r20, r19
	M3PXB r18, r21, r20, r20
	M3PXB r18, r21, r20, r21
	M3PXB r18, r21, r21, r18
	M3PXB r18, r21, r21, r19
	M3PXB r18, r21, r21, r20
	M3PXB r18, r21, r21, r21
	M3PXB r19, r18, r18, r18
	M3PXB r19, r18, r18, r19
	M3PXB r19, r18, r18, r20
	M3PXB r19, r18, r18, r21
	M3PXB r19, r18, r19, r18
	M3PXB r19, r18, r19, r19
	M3PXB r19, r18, r19, r20
	M3PXB r19, r18, r19, r21
	M3PXB r19, r18, r20, r18
	M3PXB r19, r18, r20, r19
	M3PXB r19, r18, r20, r20
	M3PXB r19, r18, r20, r21
	M3PXB r19, r18, r21, r18
	M3PXB r19, r18, r21, r19
	M3PXB r19, r18, r21, r20
	M3PXB r19, r18, r21, r21
	M3PXB r19, r19, r18, r18
	M3PXB r19, r19, r18, r19
	M3PXB r19, r19, r18, r20
	M3PXB r19, r19, r18, r21
	M3PXB r19, r19, r19, r18
	M3PXB r19, r19, r19, r19
	M3PXB r19, r19, r19, r20
	M3PXB r19, r19, r19, r21
	M3PXB r19, r19, r20, r18
	M3PXB r19, r19, r20, r19
	M3PXB r19, r19, r20, r20
	M3PXB r19, r19, r20, r21
	M3PXB r19, r19, r21, r18
	M3PXB r19, r19, r21, r19
	M3PXB r19, r19, r21, r20
	M3PXB r19, r19, r21, r21
	M3PXB r19, r20, r18, r18
	M3PXB r19, r20, r18, r19
	M3PXB r19, r20, r18, r20
	M3PXB r19, r20, r18, r21
	M3PXB r19, r20, r19, r18
	M3PXB r19, r20, r19, r19
	M3PXB r19, r20, r19, r20
	M3PXB r19, r20, r19, r21
	M3PXB r19, r20, r20, r18
	M3PXB r19, r20, r20, r19
	M3PXB r19, r20, r20, r20
	M3PXB r19, r20, r20, r21
	M3PXB r19, r20, r21, r18
	M3PXB r19, r20, r21, r19
	M3PXB r19, r20, r21, r20
	M3PXB r19, r20, r21, r21
	M3PXB r19, r21, r18, r18
	M3PXB r19, r21, r18, r19
	M3PXB r19, r21, r18, r20
	M3PXB r19, r21, r18, r21
	M3PXB r19, r21, r19, r18
	M3PXB r19, r21, r19, r19
	M3PXB r19, r21, r19, r20
	M3PXB r19, r21, r19, r21
	M3PXB r19, r21, r20, r18
	M3PXB r19, r21, r20, r19
	M3PXB r19, r21, r20, r20
	M3PXB r19, r21, r20, r21
	M3PXB r19, r21, r21, r18
	M3PXB r19, r21, r21, r19
	M3PXB r19, r21, r21, r20
	M3PXB r19, r21, r21, r21
	M3PXB r20, r18, r18, r18
	M3PXB r20, r18, r18, r19
	M3PXB r20, r18, r18, r20
	M3PXB r20, r18, r18, r21
	M3PXB r20, r18, r19, r18
	M3PXB r20, r18, r19, r19
	M3PXB r20, r18, r19, r20
	M3PXB r20, r18, r19, r21
	M3PXB r20, r18, r20, r18
	M3PXB r20, r18, r20, r19
	M3PXB r20, r18, r20, r20
	M3PXB r20, r18, r20, r21
	M3PXB r20, r18, r21, r18
	M3PXB r20, r18, r21, r19
	M3PXB r20, r18, r21, r20
	M3PXB r20, r18, r21, r21
	M3PXB r20, r19, r18, r18
	M3PXB r20, r19, r18, r19
	M3PXB r20, r19, r18, r20
	M3PXB r20, r19, r18, r21
	M3PXB r20, r19, r19, r18
	M3PXB r20, r19, r19, r19
	M3PXB r20, r19, r19, r20
	M3PXB r20, r19, r19, r21
	M3PXB r20, r19, r20, r18
	M3PXB r20, r19, r20, r19
	M3PXB r20, r19, r20, r20
	M3PXB r20, r19, r20, r21
	M3PXB r20, r19, r21, r18
	M3PXB r20, r19, r21, r19
	M3PXB r20, r19, r21, r20
	M3PXB r20, r19, r21, r21
	M3PXB r20, r20, r18, r18
	M3PXB r20, r20, r18, r19
	M3PXB r20, r20, r18, r20
	M3PXB r20, r20, r18, r21
	M3PXB r20, r20, r19, r18
	M3PXB r20, r20, r19, r19
	M3PXB r20, r20, r19, r20
	M3PXB r20, r20, r19, r21
	M3PXB r20, r20, r20, r18
	M3PXB r20, r20, r20, r19
	M3PXB r20, r20, r20, r20
	M3PXB r20, r20, r20, r21
	M3PXB r20, r20, r21, r18
	M3PXB r20, r20, r21, r19
	M3PXB r20, r20, r21, r20
	M3PXB r20, r20, r21, r21
	M3PXB r20, r21, r18, r18
	M3PXB r20, r21, r18, r19
	M3PXB r20, r21, r18, r20
	M3PXB r20, r21, r18, r21
	M3PXB r20, r21, r19, r18
	M3PXB r20, r21, r19, r19
	M3PXB r20, r21, r19, r20
	M3PXB r20, r21, r19, r21
	M3PXB r20, r21, r20, r18
	M3PXB r20, r21, r20, r19
	M3PXB r20, r21, r20, r20
	M3PXB r20, r21, r20, r21
	M3PXB r20, r21, r21, r18
	M3PXB r20, r21, r21, r19
	M3PXB r20, r21, r21, r20
	M3PXB r20, r21, r21, r21
	M3PXB r21, r18, r18, r18
	M3PXB r21, r18, r18, r19
	M3PXB r21, r18, r18, r20
	M3PXB r21, r18, r18, r21
	M3PXB r21, r18, r19, r18
	M3PXB r21, r18, r19, r19
	M3PXB r21, r18, r19, r20
	M3PXB r21, r18, r19, r21
	M3PXB r21, r18, r20, r18
	M3PXB r21, r18, r20, r19
	M3PXB r21, r18, r20, r20
	M3PXB r21, r18, r20, r21
	M3PXB r21, r18, r21, r18
	M3PXB r21, r18, r21, r19
	M3PXB r21, r18, r21, r20
	M3PXB r21, r18, r21, r21
	M3PXB r21, r19, r18, r18
	M3PXB r21, r19, r18, r19
	M3PXB r21, r19, r18, r20
	M3PXB r21, r19, r18, r21
	M3PXB r21, r19, r19, r18
	M3PXB r21, r19, r19, r19
	M3PXB r21, r19, r19, r20
	M3PXB r21, r19, r19, r21
	M3PXB r21, r19, r20, r18
	M3PXB r21, r19, r20, r19
	M3PXB r21, r19, r20, r20
	M3PXB r21, r19, r20, r21
	M3PXB r21, r19, r21, r18
	M3PXB r21, r19, r21, r19
	M3PXB r21, r19, r21, r20
	M3PXB r21, r19, r21, r21
	M3PXB r21, r20, r18, r18
	M3PXB r21, r20, r18, r19
	M3PXB r21, r20, r18, r20
	M3PXB r21, r20, r18, r21
	M3PXB r21, r20, r19, r18
	M3PXB r21, r20, r19, r19
	M3PXB r21, r20, r19, r20
	M3PXB r21, r20, r19, r21
	M3PXB r21, r20, r20, r18
	M3PXB r21, r20, r20, r19
	M3PXB r21, r20, r20, r20
	M3PXB r21, r20, r20, r21
	M3PXB r21, r20, r21, r18
	M3PXB r21, r20, r21, r19
	M3PXB r21, r20, r21, r20
	M3PXB r21, r20, r21, r21
	M3PXB r21, r21, r18, r18
	M3PXB r21, r21, r18, r19
	M3PXB r21, r21, r18, r20
	M3PXB r21, r21, r18, r21
	M3PXB r21, r21, r19, r18
	M3PXB r21, r21, r19, r19
	M3PXB r21, r21, r19, r20
	M3PXB r21, r21, r19, r21
	M3PXB r21, r21, r20, r18
	M3PXB r21, r21, r20, r19
	M3PXB r21, r21, r20, r20
	M3PXB r21, r21, r20, r21
	M3PXB r21, r21, r21, r18
	M3PXB r21, r21, r21, r19
	M3PXB r21, r21, r21, r20
	M3PXB r21, r21, r21, r21
