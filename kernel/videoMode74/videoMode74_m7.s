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
; Enters in cycle 1769. After doing its work, it returns to m74_scloop, cycle
; 1703 of the next line. For the documentation of what it displays, see the
; comments for m74_tdesc in videoMode74.s
;
; First pixel output in 24 tile wide mode has to be performed at cycle 353 (so
; OUT finishing in 354).
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
m74_m7_separator:

	; Prepare palette offset

	mov   r24,     r17     ; (1770)
	andi  r24,     0x7     ; (1771)
	swap  r24              ; (1772)
	add   XL,      r24     ; (1773)
	adc   XH,      r23     ; (1774)

	; Branch off for new / old palette load

	sbrs  r22,     1       ; (1775 / 1776)
	rjmp  m7oldp           ; (1777) Color the separator line by old palette

	; Color the separator by new palette

	mov   r20,     YL      ; (1777)
	mov   r21,     YL      ; (1778)
	swap  r20              ; (1779)
	andi  r20,     0x0F    ; (1780) Color 0 index
	andi  r21,     0x0F    ; (1781) Color 1 index

	; Fetch colors from new palette

	movw  ZL,      XL      ; (1782) ZH:ZL, XH:XL
	add   ZL,      r20     ; (1783)
	adc   ZH,      r23     ; (1784)
	sbrs  r22,     2       ; (1785 / 1786) RAM / ROM source select
	rjmp  m7nrom           ; (1787) ROM source
	ld    r20,     Z       ; (1788) Color 0 value
	movw  ZL,      XL      ; (1789) ZH:ZL, XH:XL
	add   ZL,      r21     ; (1790)
	adc   ZH,      r23     ; (1791)
	ld    r21,     Z       ; (1793) Color 1 value
	nop                    ; (1794)
	rjmp  m7pend           ; (1796)

m7oldp:
	; Color the separator by old palette

	ld    r20,     Y       ; (1779) Color 0 value
	swap  YL               ; (1780)
	ld    r21,     Y       ; (1782) Color 1 value
	M74WT_R24      12      ; (1794)
	rjmp  m7pend           ; (1796)

m7nrom:
	lpm   r20,     Z       ; (1790) Color 0 value
	movw  ZL,      XL      ; (1791) ZH:ZL, XH:XL
	add   ZL,      r21     ; (1792)
	adc   ZH,      r23     ; (1793)
	lpm   r21,     Z       ; (1796) Color 1 value
m7pend:

	; Load colors for edge tiles. The 24 tiles will use regs as follows:
	; 2 3 4 21 20 21 20 21 20 21 20 21 21 20 21 20 21 20 21 20 21 4 3 2

	movw  r2,      r20     ; (1797) r3:r2, r21:r20
	mov   r4,      r20     ; (1798)

	; Cut edge tiles depending on selected display width

	sbrs  r22,     4       ; (1799 / 1800)
	rjmp  m7wd2224         ; (1801)
	sbrc  r22,     3       ; (1801 / 1802)
	rjmp  m7wd18           ; (1803)
	rjmp  m7wd20           ; (1804)
m7wd2224:
	nop                    ; (1802)
	sbrc  r22,     3       ; (1803 / 1804)
	rjmp  m7wd22           ; (1805)
	rjmp  m7wd24           ; (1806)
m7wd18:
	mov   r4,      r23     ; (1804) 18 tiles width: r4 black
m7wd20:
	mov   r3,      r23     ; (1805) 20 or 18 tiles width: r3 black
m7wd22:
	mov   r2,      r23     ; (1806) 22, 20 or 18 tiles width: r2 black
m7wd24:
	rjmp  .                ; (1808)

	; Row counter increments

	inc   r16              ; (1809) Physical scanline counter increment
	inc   r17              ; (1810) Logical row counter increment

	;
	; Load palette
	; Register usage:
	; r24, r23, r0,  r1,  r5,  r8,  r9,  r10,
	; r11, r12, r13, r19, r22, r25, XL,  XH
	; Temporarily clobbers zero (r23) too, being a bit short on regs. To
	; fix this, the first two colors (to release r24 and r23) are
	; processed before doing anything else.
	;

	movw  ZL,      XL      ; (1811) ZH:ZL, XH:XL
	sbrs  r22,     2       ; (1812 / 1813) RAM / ROM source select
	rjmp  m7lrom           ; (1814) ROM source
	ld    r8,      Z+      ; (1815)
	ld    r9,      Z+      ; (1817)
	ld    r10,     Z+      ; (1819)
	ld    r11,     Z+      ; (   1)
	ld    r5,      Z+      ; (   3)
	;
	; The hsync_pulse part for the new scanline.
	;
	; The "update_sound" function destroys r0, r1, Z and the T flag in
	; SREG.
	;
	; HSYNC_USABLE_CYCLES:
	; 223 (Allowing 4CH audio + UART or 5CH audio)
	; 196 (If extra SD load enabled)
	;
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	movw  r12,     ZL      ; (   6)
	nop                    ; (   7)
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES
	;
	; Extra SD load if it was configured to happen
	;
#if ((M74_SD_ENABLE == 0U) || (M74_SD_EXT == 0U))
	; HSYNC_USABLE_CYCLES: 223
	M74WT_R24      10      ; (10) (245)
#else
	; HSYNC_USABLE_CYCLES: 196
	movw  ZL,      r14     ; ( 1)
	rcall m74_spiload_core ; (36) 35 cycles
	movw  r14,     ZL      ; (37) (245)
#endif
	movw  ZL,      r12     ; ( 246) Restore saved palette pointer
	mov   r24,     r8      ; ( 247) Load color 0 to its proper place
	mov   r23,     r9      ; ( 248) Load color 1 to its proper place
	movw  r0,      r10     ; ( 249) Load color 2 & 3 to their proper places
	ld    r8,      Z+      ; ( 251)
	ld    r9,      Z+      ; ( 253)
	ld    r10,     Z+      ; ( 255)
	ld    r11,     Z+      ; ( 257)
	ld    r12,     Z+      ; ( 259)
	ld    r13,     Z+      ; ( 261)
	ld    r19,     Z+      ; ( 263)
	ld    r22,     Z+      ; ( 265)
	ld    r25,     Z+      ; ( 267)
	ld    XL,      Z+      ; ( 269)
	ld    XH,      Z+      ; ( 271)
	rcall m74_wait_15      ; ( 286) (r24 is not available)
	rjmp  m7lend           ; ( 288)

m7lrom:
	; ROM palette source

	lpm   r8,      Z+      ; (1817)
	lpm   r9,      Z+      ; (1820 => 0)
	lpm   r10,     Z+      ; (   3)
	;
	; The hsync_pulse part for the new scanline.
	;
	; The "update_sound" function destroys r0, r1, Z and the T flag in
	; SREG.
	;
	; HSYNC_USABLE_CYCLES:
	; 223 (Allowing 4CH audio + UART or 5CH audio)
	; 196 (If extra SD load enabled)
	;
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	movw  r12,     ZL      ; (   6)
	nop                    ; (   7)
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	M74WT_R24      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES
	;
	; Extra SD load if it was configured to happen
	;
#if ((M74_SD_ENABLE == 0U) || (M74_SD_EXT == 0U))
	; HSYNC_USABLE_CYCLES: 223
	M74WT_R24      10      ; (10) (245)
#else
	; HSYNC_USABLE_CYCLES: 196
	movw  ZL,      r14     ; ( 1)
	rcall m74_spiload_core ; (36) 35 cycles
	movw  r14,     ZL      ; (37) (245)
#endif
	movw  ZL,      r12     ; ( 246) Restore saved palette pointer
	mov   r24,     r8      ; ( 247) Load color 0 to its proper place
	mov   r23,     r9      ; ( 248) Load color 1 to its proper place
	mov   r0,      r10     ; ( 249) Load color 2 to its proper place
	lpm   r1,      Z+      ; ( 252)
	lpm   r5,      Z+      ; ( 255)
	lpm   r8,      Z+      ; ( 258)
	lpm   r9,      Z+      ; ( 261)
	lpm   r10,     Z+      ; ( 264)
	lpm   r11,     Z+      ; ( 267)
	lpm   r12,     Z+      ; ( 270)
	lpm   r13,     Z+      ; ( 273)
	lpm   r19,     Z+      ; ( 276)
	lpm   r22,     Z+      ; ( 279)
	lpm   r25,     Z+      ; ( 282)
	lpm   XL,      Z+      ; ( 285)
	lpm   XH,      Z+      ; ( 288)
m7lend:

	; Load first two colors

	clr   YL               ; ( 289)
	rcall m74_setpalcol    ; ( 328) 3 + 36
	inc   YL               ; ( 329)
	st    Y+,      r23     ; ( 331)
	st    Y+,      r23     ; ( 333)
	st    Y+,      r23     ; ( 335)
	st    Y+,      r23     ; ( 337)
	st    Y+,      r23     ; ( 339)
	st    Y+,      r23     ; ( 341)
	st    Y+,      r23     ; ( 343)
	st    Y+,      r23     ; ( 345)
	st    Y+,      r23     ; ( 347)
	st    Y+,      r23     ; ( 349)
	st    Y+,      r23     ; ( 351)
	st    Y+,      r23     ; ( 353)
	out   PIXOUT,  r2      ; ( 354) Tile 0, Color r2
	st    Y+,      r23     ; ( 356)
	st    Y+,      r23     ; ( 358)
	st    Y+,      r23     ; ( 360)
	st    Y+,      r23     ; ( 362)
	clr   r23              ; ( 363) Restore zero

	; From here things are right for working with color or SD load.

#if (M74_SD_ENABLE != 0)
	;
	; SD loading, fitting 24 loads in
	;
	movw  ZL,      r14     ; ( 364) ZH:ZL, r15:r14, preparing for SD load
	rcall m74_spiload_core ; () 3 + 32
	M74WT_R24      10      ; ()
	out   PIXOUT,  r3      ; ( 410) Tile 1, Color r3
	rcall m74_spiload_core ; () 3 + 32
	M74WT_R24      12      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r4      ; ( 466) Tile 2, Color r4
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r21     ; ( 522) Tile 3, Color 1
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	out   PIXOUT,  r20     ; ( 578) Tile 4, Color 0
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	out   PIXOUT,  r21     ; ( 634) Tile 5, Color 1
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	out   PIXOUT,  r20     ; ( 690) Tile 6, Color 0
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r5      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	out   PIXOUT,  r21     ; ( 746) Tile 7, Color 1
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	st    Y+,      r8      ; ()
	out   PIXOUT,  r20     ; ( 802) Tile 8, Color 0
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	out   PIXOUT,  r21     ; ( 858) Tile 9, Color 1
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r9      ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	out   PIXOUT,  r20     ; ( 914) Tile 10, Color 0
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	out   PIXOUT,  r21     ; ( 970) Tile 11, Color 1
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r10     ; ()
	st    Y+,      r10     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	out   PIXOUT,  r21     ; (1026) Tile 12, Color 1
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r11     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	out   PIXOUT,  r20     ; (1082) Tile 13, Color 0
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	out   PIXOUT,  r21     ; (1138) Tile 14, Color 1
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r12     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	out   PIXOUT,  r20     ; (1194) Tile 15, Color 0
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	st    Y+,      r13     ; ()
	out   PIXOUT,  r21     ; (1250) Tile 16, Color 1
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	out   PIXOUT,  r20     ; (1306) Tile 17, Color 0
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r19     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	out   PIXOUT,  r21     ; (1362) Tile 18, Color 1
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	out   PIXOUT,  r20     ; (1418) Tile 19, Color 0
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r22     ; ()
	st    Y+,      r22     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	out   PIXOUT,  r21     ; (1474) Tile 20, Color 1
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      r25     ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	out   PIXOUT,  r4      ; (1530) Tile 21, Color r4
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	out   PIXOUT,  r3      ; (1586) Tile 22, Color r3
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XL      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	out   PIXOUT,  r2      ; (1642) Tile 23, Color r2
	rcall m74_spiload_core ; () 3 + 32
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y+,      XH      ; ()
	st    Y,       XH      ; ()
	out   PIXOUT,  r23     ; (1698) Termination
	rjmp  .                ; ()
	movw  r14,     ZL      ; () r15:r14, ZH:ZL
	rjmp  m74_scloop       ; (1703)
#else
	;
	; No SD access, aim for small size
	;
	M74WT_R24      46      ; ()
	out   PIXOUT,  r3      ; ( 410) Tile 1, Color r3
	M74WT_R24      55      ; ()
	out   PIXOUT,  r4      ; ( 466) Tile 2, Color r4
	M74WT_R24      55      ; ()
	out   PIXOUT,  r21     ; ( 522) Tile 3, Color 1
	M74WT_R24      55      ; ()
	out   PIXOUT,  r20     ; ( 578) Tile 4, Color 0
	M74WT_R24      55      ; ()
	out   PIXOUT,  r21     ; ( 634) Tile 5, Color 1
	M74WT_R24      55      ; ()
	out   PIXOUT,  r20     ; ( 690) Tile 6, Color 0
	M74WT_R24      55      ; ()
	out   PIXOUT,  r21     ; ( 746) Tile 7, Color 1
	M74WT_R24      55      ; ()
	out   PIXOUT,  r20     ; ( 802) Tile 8, Color 0
	M74WT_R24      55      ; ()
	out   PIXOUT,  r21     ; ( 858) Tile 9, Color 1
	M74WT_R24      55      ; ()
	out   PIXOUT,  r20     ; ( 914) Tile 10, Color 0
	mov   r24,     r0      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r21     ; ( 970) Tile 11, Color 1
	mov   r24,     r1      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r21     ; (1026) Tile 12, Color 1
	mov   r24,     r5      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r20     ; (1082) Tile 13, Color 0
	mov   r24,     r8      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r21     ; (1138) Tile 14, Color 1
	mov   r24,     r9      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r20     ; (1194) Tile 15, Color 0
	mov   r24,     r10     ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r21     ; (1250) Tile 16, Color 1
	mov   r24,     r11     ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r20     ; (1306) Tile 17, Color 0
	mov   r24,     r12     ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r21     ; (1362) Tile 18, Color 1
	mov   r24,     r13     ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r20     ; (1418) Tile 19, Color 0
	mov   r24,     r19     ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r21     ; (1474) Tile 20, Color 1
	mov   r24,     r22     ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r4      ; (1530) Tile 21, Color r4
	mov   r24,     r25     ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r3      ; (1586) Tile 22, Color r3
	mov   r24,     XL      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	M74WT_R24      14      ; ()
	out   PIXOUT,  r2      ; (1642) Tile 23, Color r2
	mov   r24,     XH      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	M74WT_R24      15      ; ()
	out   PIXOUT,  r23     ; (1698) Termination
	rjmp  .                ; ()
	movw  r14,     ZL      ; () r15:r14, ZH:ZL
	rjmp  m74_scloop       ; (1703)
#endif
