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
; Enters in cycle 1770. After doing its work, it returns to m74_scloop_sr,
; cycle 1697 of the next line.
;
; First pixel output in 24 tile wide mode has to be performed at cycle 353 (so
; OUT finishing in 354).
;
; VRAM layout:
;
; 0x00 - 0x7F: 6px wide 1bpp tiles (same as row mode 5)
; 0x80 - 0xBF: 8px wide 1bpp tiles
; 0xC0 - 0xFF: 8px wide 1bpp entry / filler tiles
;
; Tile row descriptor layout:
;
; byte 0: bit 0 - 2: Unused
;         bit 3 - 4: Row width
;         bit 5 - 7: 3 (Selects 2bpp Multicolor)
; byte 1: bit 0 - 1: 1bpp tile configuration selector
;         bit 2 - 3: Unused
;         bit 4 - 7: Foreground color index for 1bpp tiles
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
; Tile row descriptor byte 1, bits 0 - 1 use the same configuration like other
; modes, however the low byte of the offset and the increment is ignored, the
; latter being fixed at 64 (256 bytes).
;
; r16: Scanline counter (increments it by one)
; r17: Logical row counter (increments it by one)
; r22: Byte 0 of tile descriptor
; YL:  Byte 1 of tile descriptor
; X:   Offset from tile index list
; r9:  Global configuration (m74_config)
; r14: SD load address, low
; r15: SD load address, high
; r23: Zero
; YH:  Palette buffer, high
;
; Everything expect r14, r15, r16, r17, r23, and YH may be clobbered.
;
; Register usage in the output logic:
;
; r0,  r1:            Temporary for multiplication etc.
; r2,  r3,  r4,  r5:  Temporary color holding regs for pixel output
; r18, r19, r20, r21: Colors 0, 1, 2 and 3 fetched by attributes in MC output
; r8,  r9:            Temporary holding regs for colors 0 and 1 in MC output
; r10:                0x05, used for calculating jump address in MC output
; r11:                0xFF, used for an add in MC output
; r12:                Stores base offset low for code tile blk. for MC output
; r13:                Stores base offset high for code tile blk. for MC output
; r22:                MC output remaining tile counter
; X:                  VRAM pointer (offset from tile index list used)
; r24, r25:           Background color for 1bpp tiles
; SP:                 Multicolor framebuffer pointer
; r6,  r7:            Saved stack pointer
; r23:                Tile counter (reusing zero)
; r16:                Foreground color for 1bpp tiles (reusing scanline ctr)
; r17:                1bpp tiles offset high (reusing logical row ctr)
;
; r23, r16 and r17 is restored before return, they are stored on the stack
; (expect r23 which is just zero; this is not a problem: it doesn't increase
; stack use since inline audio mixer also needs stack space).
;

m74_m3_2bppmc:

	;
	; Do Color 0 reload here since it fits, and makes life easier further
	; down. It also enables doing Color 0 reload even at 24 tiles width or
	; without taking away SD loads at reduced widths.
	;

#if (M74_COL0_RELOAD != 0)
	sbrs  r9,      4       ; (1771 / 1772)
	rjmp  m3dfrl0          ; (1773)
	mov   r0,      YL      ; (1773) Put aside byte 1 of tile descriptor
#if (M74_COL0_PTRE != 0)
	lds   ZL,      m74_col0_lo  ; (1775)
	lds   ZH,      m74_col0_hi  ; (1777)
#else
	ldi   ZL,      lo8(M74_COL0_OFF) ; (1774)
	ldi   ZH,      hi8(M74_COL0_OFF) ; (1775)
	rjmp  .                ; (1777)
#endif
	add   ZL,      r17     ; (1778)
	adc   ZH,      r23     ; (1779) (Just carry, r23 is zero)
	sbrs  r9,      5       ; (1780 / 1781)
	rjmp  .+4              ; (1782)
	ld    r8,      Z       ; (1783) Color0 table in RAM
	rjmp  .+2              ; (1785)
	lpm   r8,      Z       ; (1785) Color0 table in ROM
	clr   YL               ; (1786)
	st    Y+,      r8      ; (1788)
	st    Y+,      r8      ; (1790)
	st    Y+,      r8      ; (1792)
	st    Y+,      r8      ; (1794)
	st    Y+,      r8      ; (1796)
	st    Y+,      r8      ; (1798)
	st    Y+,      r8      ; (1800)
	st    Y+,      r8      ; (1802)
	st    Y+,      r8      ; (1804)
	st    Y+,      r8      ; (1806)
	st    Y+,      r8      ; (1808)
	st    Y+,      r8      ; (1810)
	st    Y+,      r8      ; (1812)
	st    Y+,      r8      ; (1814)
	st    Y+,      r8      ; (1816)
	st    Y+,      r8      ; (1818)
	mov   YL,      r0      ; (1819) Restore YL (Fg. palette index)
	rjmp  .                ; (   1)
	rjmp  m3dfrl1          ; (   3)
m3dfrl0:
#if (M74_SD_ENABLE != 0)
	M74WT_R24,     13      ; (1786)
	movw  ZL,      r14     ; (1787)
	rcall m74_spiload_core ; (   2) 35 cycles
	movw  r14,     ZL      ; (   3)
#else
	M74WT_R24      50      ; (   3)
#endif
m3dfrl1:
#else
#if (M74_SD_ENABLE != 0)
	M74WT_R24      16      ; (1786)
	movw  ZL,      r14     ; (1787)
	rcall m74_spiload_core ; (   2) 35 cycles
	movw  r14,     ZL      ; (   3)
#else
	M74WT_R24      53      ; (   3)
#endif
#endif

	;
	; The hsync_pulse part for the new scanline.
	;
	; The "update_sound" function destroys r0, r1, Z and the T flag in
	; SREG.
	;
	; HSYNC_USABLE_CYCLES:
	; 223 (Allowing 4CH audio + UART or 5CH audio)
	;
	; Cycle counter is at 228 on termination
	;
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	mov   r21,     r17     ; (   6)
	andi  r21,     0x7     ; (   7) Prepare tile row count in r21
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES



	;
	; Extra SD load if it was configured to happen
	;
#if ((M74_SD_ENABLE == 0U) || (M74_SD_EXT == 0U))
	; HSYNC_USABLE_CYCLES: 223
	M74WT_R24      9       ; ( 9)
	movw  ZL,      r14     ; (10) (245)
#else
	; HSYNC_USABLE_CYCLES: 196
	movw  ZL,      r14     ; ( 1)
	rcall m74_spiload_core ; (36) 35 cycles
	movw  r14,     ZL      ; (37) (245) Also serves as SPI gap for next load
#endif
	;
	; A further extra SD load just fits in this mode
	;
#if (M74_SD_ENABLE != 0)
	rcall m74_spiload_core ; ( 280) 35 cycles
	movw  r14,     ZL      ; ( 281)
#else
	M74WT_R24      36      ; ( 281)
#endif



	;
	; Do horizontal size reduction along with SD loads.
	; Also calculate number of tiles to generate into r20 for now.
	;

	andi  r22,     0x18    ; ( 282)
	sts   v_hsize, r22     ; ( 284)
	ldi   r20,      96     ; ( 285) 4 * 24
	sub   r20,     r22     ; ( 286) 4 * 18; 4 * 20; 4 * 22; 4 * 24 depending on tile count
	lsr   r20              ; ( 287)
	lsr   r20              ; ( 288)
m3dfloop:
	subi  r22,     8       ; ( 1) (289)
	brcs  m3dfend          ; ( 2 /  3) (291)
#if (M74_SD_ENABLE != 0)
	M74WT_R24      15      ; (17)
	movw  ZL,      r14     ; (18) ZH:ZL, r15:r14 Target pointer
	rcall m74_spiload_core ; (53) 35 cycles
	movw  r14,     ZL      ; (54) r15:r14, ZH:ZL Target pointer
#else
	M74WT_R24      52      ; (54)
#endif
	rjmp  m3dfloop         ; (56 = 0)
m3dfend:

	;
	; Preparations for the main tile loop
	;
	; 41 cycles
	;

	inc   r16              ; ( 1) Physical scanline counter increment
	inc   r17              ; ( 2) Logical row counter increment
	push  r17              ; ( 4)
	push  r16              ; ( 6) Save regs which will be clobbered

	; Load 1bpp configuration

	sbrs  YL,      1       ; ( 7 /  8)
	rjmp  m3cfg0x          ; ( 9)
	ldi   r17,     hi8(M74_TBANK01_3_OFF) ; ( 9)
	sbrs  YL,      0       ; (10)
	ldi   r17,     hi8(M74_TBANK01_2_OFF) ; (11)
	rjmp  m3cfge           ; (13)
m3cfgd0:
	nop                    ; (24) No double-scan subtract path
	rjmp  m3cfgd1          ; (26)
m3cfg0x:
	ldi   r17,     hi8(M74_TBANK01_1_OFF) ; (10)
	sbrs  YL,      0       ; (11)
	ldi   r17,     hi8(M74_TBANK01_0_OFF) ; (12)
	nop                    ; (13)
m3cfge:

	; Apply row increment (256) on 1bpp offset

	add   r17,     r21     ; (14)

	; Hack the stack making it the multicolor framebuffer pointer. Also
	; apply double-scan subtract here if the conditions for it are met.

	in    r6,      STACKL  ; (15)
	in    r7,      STACKH  ; (16) Save stack pointer
	lds   r24,     v_m3ptr_lo   ; (18)
	lds   r25,     v_m3ptr_hi   ; (20)
	subi  r16,     (M74_M3_SUBSL + 1) ; (21)
	brcs  m3cfgd0          ; (22 / 23)
	and   r16,     r9      ; (23) Physical row counter bit 0 aligned with m74_config bit
	sbrc  r16,     0       ; (24 / 25)
	subi  r24,     M74_M3_SUB   ; (25) If this does not execute, carry is clear
	sbci  r25,     0       ; (26)
m3cfgd1:
	out   STACKL,  r24     ; (27)
	out   STACKH,  r25     ; (28) From now stack is used as fb. pointer

	; Pick colors from palette for 1bpp output

	ld    r16,     Y       ; (30) Foreground (1) color
	lds   YL,      m74_bgcol    ; (32)
	ld    r24,     Y       ; (34)
	mov   r25,     r24     ; (35) Background (0) color

	; Pre-load registers for the tile loop

	ldi   r18,     0x05    ; (36)
	ldi   r19,     0xFF    ; (37)
	movw  r10,     r18     ; (38) r11:r10, r19:r18 Load constants
	ldi   r18,     lo8(pm(m3pxblk)) ; (39)
	ldi   r19,     hi8(pm(m3pxblk)) ; (40)
	movw  r12,     r18     ; (41) r13:r12, r19:r18 Load base offset for code tiling

	;
	; Tile output lead-in. The first pixel must be produced at 353 (so the
	; out ending at 354). The number of tiles to generate is still in r19,
	; otherwise everything is in the green at this point.
	;

	mov   r4,      r23     ; ( 333)
	mov   r5,      r23     ; ( 334) Zero partial -1th tile
	mov   r23,     r20     ; ( 335)
	rjmp  m3tlcom          ; ( 337) (12 of half-tile)



;
; Exit from line render, going back onto the main scanline loop
;
m3tlend:
	nop                    ; (12)
	in    r2,      STACKL  ; (13)
	in    r3,      STACKH  ; (14)
	out   PIXOUT,  r4      ; (15) Pixel 6 of prev. tile
	sts   v_m3ptr_lo, r2   ; (17)
	sts   v_m3ptr_hi, r3   ; (19) Save framebuffer pointer
	out   STACKL,  r6      ; (20)
	out   STACKH,  r7      ; (21) Restore stack
	out   PIXOUT,  r5      ; (22) Pixel 7 of prev. tile
	pop   r16              ; (24) Restore physical line counter (used for FG color)
	pop   r17              ; (26) Restore logical line counter (used for 1bpp offset)
	rjmp  m74_scloop_sr    ; (28 / 1697)



;
; 8px wide 1bpp tiles
;
m3tl8:
	movw  r2,      r24     ; () r3:r2, r25:r24; BG color loads
	sbrc  r0,      1       ; ()
	mov   r2,      r16     ; ()
	dec   r23              ; () One tile less to go (zero test below)
	out   PIXOUT,  r5      ; (22) Pixel 3 of current tile
	sbrc  r0,      0       ; ()
	mov   r3,      r16     ; ()
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
	mov   r2,      r16     ; ()
	out   PIXOUT,  r2      ; ( 1) Pixel 0 of current tile
	movw  r4,      r24     ; () r5:r4, r25:r24; BG color loads
	movw  r8,      r24     ; () r9:r8, r25:r24; BG color loads
	sbrc  r0,      6       ; ()
	mov   r3,      r16     ; ()
	sbrc  r0,      5       ; ()
	mov   r4,      r16     ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1 of current tile
	sbrc  r0,      4       ; ()
	mov   r5,      r16     ; ()
	sbrc  r0,      3       ; ()
	mov   r8,      r16     ; ()
	sbrc  r0,      2       ; ()
	mov   r9,      r16     ; ()
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
	mov   r4,      r16     ; ()
	sbrc  r0,      0       ; ()
	mov   r5,      r16     ; ()
	out   PIXOUT,  r8      ; ( 1) Pixel 4 of current tile (bg)
	ld    r19,     Y       ; ()
	ld    YL,      X+      ; ()
	ld    r20,     Y       ; ()
	out   PIXOUT,  r9      ; ( 8) Pixel 5 of current tile
	cpi   r22,     0       ; ( 9)
	brne  m3pxentry        ; (10 / 11) Go for rendering 2bpp Multicolor if any
m3pxend:
	subi  XL,      2       ; (11) Attributes weren't needed, so restore
	sbci  XH,      0       ; (12) Or if falling out of 2bpp MC, then restore for that
m3tlcom:
	ld    ZL,      X+      ; (14)
	out   PIXOUT,  r4      ; (15) Pixel 6 of prev. tile
	mov   ZH,      r17     ; (16) Load ROM tile offset base
	lpm   r0,      Z       ; (19) Load pixel data for tile
	movw  r2,      r24     ; (20) r3:r2, r25:r24; BG color loads
	cpi   ZL,      0xC0    ; (21) Check for entry / filler tiles (Carry clear if so)
	out   PIXOUT,  r5      ; (22) Pixel 7 of prev. tile
	movw  r4,      r24     ; () r5:r4, r25:r24; BG color loads
	movw  r8,      r24     ; () r9:r8, r25:r24; BG color loads
	sbrc  r0,      7       ; ()
	mov   r2,      r16     ; ()
	sbrc  r0,      6       ; ()
	mov   r3,      r16     ; ()
	out   PIXOUT,  r2      ; ( 1) Pixel 0 of current tile
	sbrc  r0,      5       ; ()
	mov   r4,      r16     ; ()
	sbrc  r0,      4       ; ()
	mov   r5,      r16     ; ()
	sbrc  r0,      3       ; ()
	mov   r8,      r16     ; ()
	out   PIXOUT,  r3      ; ( 8) Pixel 1 of current tile
	sbrc  r0,      2       ; ()
	mov   r9,      r16     ; ()
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
	mul   r0,      r10     ; (19) r10 contains 0x05 for the assignment block size
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
	mul   r0,      r10     ; (14) r10 contains 5 for the assignment block size
	out   PIXOUT,  r4      ; (15)
	swap  YL               ; (16)
	ld    r9,      Y       ; (18)
	movw  ZL,      r12     ; (19) ZH:ZL, r13:r12, latter contains base for px. assign block
	add   ZL,      r0      ; (20)
	adc   ZH,      r1      ; (21)
	out   PIXOUT,  r5      ; (22) Prev. block color
	add   r22,     r11     ; (23) r11 contains 0xFF, always producing carry
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
