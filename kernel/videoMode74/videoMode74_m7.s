;
; Uzebox Kernel - Video Mode 74 Row mode 7 (Separator line)
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
; Separator line with palette reload and stuff.
;
; Enters in cycle 1770. After doing its work, it returns to m74_scloop, cycle
; 1700 of the next line. For the documentation of what it displays, see the
; comments for m74_tdesc in videoMode74.s
;
; First pixel output in 24 tile wide mode has to be performed at cycle 350 (so
; OUT finishing in 351).
;
; r16: Scanline counter (increments it by one)
; r17: Logical row counter (increments it by one)
; r22: Byte 0 of tile descriptor
; YL:  Byte 1 of tile descriptor
; X:   Offset from tile index list
; r9:  Global configuration (m74_config)
; r14: RAM clear / SPI load address, low
; r15: RAM clear / SPI load address, high
; r23: Zero
; YH:  Palette buffer, high
;
; Everything expect r14, r15, r16, r17, r23, and YH may be clobbered.
;
m74_m7_separator:

	lds   r18,     m74_ldsl    ; (1772)
	lds   r6,      v_remc      ; (1774)
	lds   r7,      v_rems      ; (1776)

	; Prepare palette offset

	mov   r24,     r17     ; (1777)
	andi  r24,     0x7     ; (1778)
	swap  r24              ; (1779)
	add   XL,      r24     ; (1780)
	adc   XH,      r23     ; (1781)

	; Branch off for new / old palette load

	sbrs  r22,     1       ; (1782 / 1783)
	rjmp  m7oldp           ; (1784) Color the separator line by old palette

	; Color the separator by new palette

	mov   r20,     YL      ; (1784)
	mov   r21,     YL      ; (1785)
	swap  r20              ; (1786)
	andi  r20,     0x0F    ; (1787) Color 0 index
	andi  r21,     0x0F    ; (1788) Color 1 index

	; Fetch colors from new palette

	movw  ZL,      XL      ; (1789) ZH:ZL, XH:XL
	add   ZL,      r20     ; (1790)
	adc   ZH,      r23     ; (1791)
	sbrs  r22,     2       ; (1792 / 1793) RAM / ROM source select
	rjmp  m7nrom           ; (1794) ROM source
	ld    r20,     Z       ; (1795) Color 0 value
	movw  ZL,      XL      ; (1796) ZH:ZL, XH:XL
	add   ZL,      r21     ; (1797)
	adc   ZH,      r23     ; (1798)
	ld    r21,     Z       ; (1800) Color 1 value
	nop                    ; (1801)
	rjmp  m7pend           ; (1803)

m7oldp:
	; Color the separator by old palette

	ld    r2,      Y       ; (1786) Color 0 value
	swap  YL               ; (1787)
	ld    r3,      Y       ; (1788) Color 1 value
	lpm   r24,     Z       ; (1791) Dummy load (nop)
	lpm   r24,     Z       ; (1794) Dummy load (nop)
	lpm   r24,     Z       ; (1797) Dummy load (nop)
	rjmp  .                ; (1799)
	rjmp  .                ; (1801)
	rjmp  m7pend           ; (1803)

m7nrom:
	lpm   r20,     Z       ; (1797) Color 0 value
	movw  ZL,      XL      ; (1798) ZH:ZL, XH:XL
	add   ZL,      r21     ; (1799)
	adc   ZH,      r23     ; (1800)
	lpm   r21,     Z       ; (1803) Color 1 value
m7pend:

	; Load colors for edge tiles. The 24 tiles will use regs as follows:
	; 2 3 4 21 20 21 20 21 20 21 20 21 21 20 21 20 21 20 21 20 21 4 3 2

	movw  r2,      r20     ; (1804) r3:r2, r21:r20
	mov   r4,      r20     ; (1805)

	; Cut edge tiles depending on selected display width

	sbrc  r22,     4       ; (1806)
	mov   r2,      r23     ; (1807) 20 or 18 tiles width: r2 black
	sbrc  r22,     4       ; (1808)
	mov   r3,      r23     ; (1809) 20 or 18 tiles width: r3 black
	sbrc  r22,     3       ; (1810)
	mov   r2,      r23     ; (1811) 22 or 18 tiles width: r2 black
	andi  r22,     0x18    ; (1812)
	cpi   r22,     0x18    ; (1813)
	brne  m7nwd18          ; (1814 / 1815)
	mov   r4,      r23     ; (1815) 18 tiles width: r4 black
m7nwd18:

	; Wait the remaining few cycles until hsync

	lpm   r24,     Z       ; (1818) Dummy load (nop)
	lpm   r24,     Z       ; (   1) Dummy load (nop)
	rjmp  .                ; (   3)

	;
	; The hsync_pulse part for the new scanline.
	;
	; The "update_sound" function destroys r0, r1, Z and the T flag in
	; SREG.
	;
	; HSYNC_USABLE_CYCLES:
	; 217 (Allowing 4CH audio + UART or 5CH audio)
	;
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	inc   r16              ; (   6) Physical scanline counter increment
	inc   r17              ; (   7) Logical row counter increment
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	WAIT  ZL,      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES

	;
	; Load palette (cycle ctr. is at 229)
	; Register usage:
	; r24, r23, r0,  r1,  r5,  r8,  r9,  r10,
	; r11, r12, r13, r19, r22, r25, XL,  XH
	; Temporarily clobbers zero (r23) too, being a bit short on regs. To
	; fix this, the first two colors (to release r24 and r23) are
	; processed before doing anything else.
	;

	movw  ZL,      XL      ; ( 230) ZH:ZL, XH:XL
	sbrs  r22,     2       ; ( 231 /  232) RAM / ROM source select
	rjmp  m7lrom           ; ( 233) ROM source

	; RAM palette source

	ld    r24,     Z+      ; ( 234)
	ld    r23,     Z+      ; ( 236)
	ld    r0,      Z+      ; ( 238)
	ld    r1,      Z+      ; ( 240)
	ld    r5,      Z+      ; ( 242)
	ld    r8,      Z+      ; ( 244)
	ld    r9,      Z+      ; ( 246)
	ld    r10,     Z+      ; ( 248)
	ld    r11,     Z+      ; ( 250)
	ld    r12,     Z+      ; ( 252)
	ld    r13,     Z+      ; ( 254)
	ld    r19,     Z+      ; ( 256)
	ld    r22,     Z+      ; ( 258)
	ld    r25,     Z+      ; ( 260)
	ld    XL,      Z+      ; ( 262)
	ld    XH,      Z+      ; ( 264)
	lpm   YL,      Z       ; ( 267) Dummy load (nop)
	lpm   YL,      Z       ; ( 270) Dummy load (nop)
	lpm   YL,      Z       ; ( 273) Dummy load (nop)
	lpm   YL,      Z       ; ( 276) Dummy load (nop)
	lpm   YL,      Z       ; ( 279) Dummy load (nop)
	rjmp  m7lend           ; ( 281)

m7lrom:
	; ROM palette source

	lpm   r24,     Z+      ; ( 236)
	lpm   r23,     Z+      ; ( 239)
	lpm   r0,      Z+      ; ( 242)
	lpm   r1,      Z+      ; ( 245)
	lpm   r5,      Z+      ; ( 248)
	lpm   r8,      Z+      ; ( 251)
	lpm   r9,      Z+      ; ( 254)
	lpm   r10,     Z+      ; ( 257)
	lpm   r11,     Z+      ; ( 260)
	lpm   r12,     Z+      ; ( 263)
	lpm   r13,     Z+      ; ( 266)
	lpm   r19,     Z+      ; ( 269)
	lpm   r22,     Z+      ; ( 272)
	lpm   r25,     Z+      ; ( 275)
	lpm   XL,      Z+      ; ( 278)
	lpm   XH,      Z+      ; ( 281)
m7lend:

	; Load first two colors

	clr   YL               ; ( 282)
	rcall m74_setpalcol    ; ( 321) 3 + 36
	inc   YL               ; ( 322)
	st    Y+,      r23     ; ( 324)
	st    Y+,      r23     ; ( 326)
	st    Y+,      r23     ; ( 328)
	st    Y+,      r23     ; ( 330)
	st    Y+,      r23     ; ( 332)
	st    Y+,      r23     ; ( 334)
	st    Y+,      r23     ; ( 336)
	st    Y+,      r23     ; ( 338)
	st    Y+,      r23     ; ( 340)
	st    Y+,      r23     ; ( 342)
	st    Y+,      r23     ; ( 344)
	st    Y+,      r23     ; ( 346)
	st    Y+,      r23     ; ( 348)
	st    Y+,      r23     ; ( 350)
	out   PIXOUT,  r2      ; ( 351) Tile 0, Color r2
	st    Y+,      r23     ; ( 353)
	st    Y+,      r23     ; ( 355)
	clr   r23              ; ( 356) Restore zero
	movw  ZL,      r14     ; ( 357) ZH:ZL, r15:r14, preparing for RAM clear / SPI load

	; From here things are right for working with color or RAM clear / SPI load.

	rcall m74_sl_func      ; () 3 + 44
	rjmp  .                ; ()
	out   PIXOUT,  r3      ; ( 407) Tile 1, Color r3
	mov   r24,     r0      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	out   PIXOUT,  r4      ; ( 463) Tile 2, Color r4
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	out   PIXOUT,  r21     ; ( 519) Tile 3, Color 1
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	out   PIXOUT,  r20     ; ( 575) Tile 4, Color 0
	st    Y+,      r1      ; ()
	mov   r24,     r5      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	out   PIXOUT,  r21     ; ( 631) Tile 5, Color 1
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	out   PIXOUT,  r20     ; ( 687) Tile 6, Color 0
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	out   PIXOUT,  r21     ; ( 743) Tile 7, Color 1
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	mov   r24,     r9      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	out   PIXOUT,  r20     ; ( 799) Tile 8, Color 0
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	out   PIXOUT,  r21     ; ( 855) Tile 9, Color 1
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	out   PIXOUT,  r20     ; ( 911) Tile 10, Color 0
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	mov   r24,     r11     ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	out   PIXOUT,  r21     ; ( 967) Tile 11, Color 1
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	out   PIXOUT,  r21     ; (1023) Tile 12, Color 1
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	out   PIXOUT,  r20     ; (1079) Tile 13, Color 0
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	out   PIXOUT,  r21     ; (1135) Tile 14, Color 1
	mov   r24,     r13     ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	out   PIXOUT,  r20     ; (1191) Tile 15, Color 0
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	out   PIXOUT,  r21     ; (1247) Tile 16, Color 1
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	out   PIXOUT,  r20     ; (1303) Tile 17, Color 0
	st    Y+,      r19     ; ()
	mov   r24,     r22     ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	out   PIXOUT,  r21     ; (1359) Tile 18, Color 1
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	out   PIXOUT,  r20     ; (1415) Tile 19, Color 0
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	out   PIXOUT,  r21     ; (1471) Tile 20, Color 1
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	mov   r24,     XL      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	out   PIXOUT,  r4      ; (1527) Tile 21, Color r4
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	out   PIXOUT,  r3      ; (1583) Tile 22, Color r3
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	out   PIXOUT,  r2      ; (1639) Tile 23, Color r2
	rcall m74_sl_func      ; () 3 + 44
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y,       XH      ; ()
	sts   v_remc,  r6      ; () Restore stuff after RAM clear / SPI load
	out   PIXOUT,  r23     ; (1695) Termination
	sts   v_rems,  r7      ; ()
	movw  r14,     ZL      ; () r15:r14, ZH:ZL
	rjmp  m74_scloop       ; (1700)
