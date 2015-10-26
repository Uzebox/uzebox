;
; Uzebox Kernel - Video Mode 74 renderer sub
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



.section .bss

	; Locals

	v_rows_lo:     .byte 1 ; Row selector current address, low
	v_rows_hi:     .byte 1 ; Row selector current address, high
	v_shifto:      .byte 1 ; X shift override

.section .text



sub_video_mode74:

	; Entry happens in cycle 467.

	; Check for user video mode, before palette load

	lds   r14,     m74_umod_lo  ; ( 469)
	lds   r15,     m74_umod_hi  ; ( 471)
	sbrc  r14,     0       ; ( 472 /  473)
	rjmp  umod0            ; ( 474) Entry after palette load
	mov   r0,      r14     ; ( 474)
	or    r0,      r15     ; ( 475)
	brne  umod1            ; ( 476 /  477)
	rjmp  ddis             ; ( 478) Display disabled
umod1:
	movw  ZL,      r14     ; ( 478) ZH:ZL, r15:r14
	lsr   ZH               ; ( 479)
	ror   ZL               ; ( 480)
	ijmp                   ; ( 482) Continue with User video mode
umod0:

	; Load palette

	lds   r23,     m74_config   ; ( 476)
	lds   YH,      m74_palbuf   ; ( 478)
	lds   ZL,      m74_pal_lo   ; ( 480)
	lds   ZH,      m74_pal_hi   ; ( 482)
	clr   YL               ; ( 483)
lcloop:
	sbrs  r23,     3       ; ( 1)
	rjmp  lcrom            ; ( 3)
	ld    r24,     Z+      ; ( 4)
	rjmp  lccom            ; ( 6)
lcrom:
	lpm   r24,     Z+      ; ( 6)
lccom:
	rcall m74_setpalcol    ; (45) (3 + 36 cycles)
	inc   YL               ; (46)
	brne  lcloop           ; (47 / 48) (767 cy total; at 1250 here)

	; Check for user video mode, after palette load

	or    r15,     r15     ; (1251)
	brne  umod2            ; (1252 / 1253)
	dec   r14              ; (1253)
	brne  umod3            ; (1254 / 1255)
	rjmp  umod4            ; (1256) No user mode
umod2:
	dec   r15              ; (1254)
	nop                    ; (1255)
umod3:
	inc   r15              ; (1256)
	movw  ZL,      r14     ; (1257) ZH:ZL, r15:r14
	lsr   ZH               ; (1258)
	ror   ZL               ; (1259)
	ijmp                   ; (1261) Continue with User video mode
umod4:

	; Initializing for the scanline loop

	clr   r16              ; (1257) Scanline counter
	lds   ZL,      m74_rows_lo  ; (1259)
	lds   ZH,      m74_rows_hi  ; (1261)
	sbrs  r23,     0       ; (1262 / 1263)
	rjmp  lresp            ; (1264)
	; RAM scanline map: nothing to do here
	lpm   r0,      Z       ; (1266) dummy load (nop)
	rjmp  .                ; (1268)
	rjmp  lrese            ; (1270)
lresp:
	; RAM scanline + restart pairs: load the first two values to get the
	; initial scanline.
	ld    r17,     Z+      ; (1266) Load new physical row counter
	ld    r2,      Z+      ; (1268) Load new shift override
	sts   v_shifto,  r2    ; (1270)
lrese:
	sts   v_rows_lo, ZL    ; (1272)
	sts   v_rows_hi, ZH    ; (1274)

	; Initializing RAM clear or SPI load function

	lds   r14,     m74_totc_lo  ; (1276)
	lds   r15,     m74_totc_hi  ; (1278)
	sts   v_remc_lo, r14        ; (1280) Remaining blocks: Total blocks
	sts   v_remc_hi, r15        ; (1282)
	lds   r14,     m74_fadd_lo  ; (1284) Target address in r15:r14
	lds   r15,     m74_fadd_hi  ; (1286)
	ldi   r18,     0       ; (1287) SPI 2 byte blocks remaining: 512
	ldi   r19,     0xFF    ; (1288) SPI state: Normal load
	sts   v_spibc, r18     ; (1290)
	sts   v_spis,  r19     ; (1292)

	; Sandwiched between waits so it is simpler to shift it a bit around
	; when tweaking the scanline loop.

	WAIT  r18,     406     ; (1698)
	call  m74_scloop       ; (1710)
	WAIT  r18,     101     ; (1811)

	; Complete RAM clear / SPI load by writing out remaining blocks

	sts   v_cadd_lo, r14   ; (1813)
	sts   v_cadd_hi, r15   ; (1815)

	; Update the sync_pulse variable which was neglected during the loop
	; In r16 the scanline counter now equals render_lines_count, ready to
	; be subtracted.

	lds   r0,      sync_pulse ; (1817)
	sub   r0,      r16     ; (1818)
	sts   sync_pulse, r0   ; (1820 = 0)
laend:
	rcall hsync_pulse      ; Last hsync, from now cycle precise part over.

	; Set vsync flag & flip field
	lds   ZL,      sync_flags
	ldi   r20,     SYNC_FLAG_FIELD
	eor   ZL,      r20
	ori   ZL,      SYNC_FLAG_VSYNC
	sts   sync_flags, ZL
	; Clear any pending timer interrupt
	ldi   ZL,      (1<<OCF1A)
	sts   _SFR_MEM_ADDR(TIFR1), ZL
	ret                    ; All done

ddis:
	; Display Disabled frame

	WAIT  r23,     1814 - 478
	clr   r16              ; (1815) Scanline counter
ddisl:
	lds   r23,     render_lines_count ; (1817)
	cp    r23,     r16     ; (1818)
	breq  laend            ; (1819 / 1820)
	inc   r16              ; (1820)
	rcall hsync_pulse      ; (21 + AUDIO)
	WAIT  r23,     1000 - 21 - AUDIO_OUT_HSYNC_CYCLES
	WAIT  r23,     813     ; () Needed to split it up
	rjmp  ddisl            ; (1815)




;
; Sets a color in the palette.
;
; YL:  The color index to set * 16
; r24: The color value to set
; YH:  Points at the palette buffer
;
m74_setpalcol:
	st    Y+,      r24     ; ( 2)
	st    Y+,      r24     ; ( 4)
	st    Y+,      r24     ; ( 6)
	st    Y+,      r24     ; ( 8)
	st    Y+,      r24     ; (10)
	st    Y+,      r24     ; (12)
	st    Y+,      r24     ; (14)
	st    Y+,      r24     ; (16)
	st    Y+,      r24     ; (18)
	st    Y+,      r24     ; (20)
	st    Y+,      r24     ; (22)
	st    Y+,      r24     ; (24)
	st    Y+,      r24     ; (26)
	st    Y+,      r24     ; (28)
	st    Y+,      r24     ; (30)
	st    Y,       r24     ; (32)
	ret                    ; (36)




;
; SPI load function
;
; Processes 2 bytes per call from the SPI bus, advancing as long as the
; remaining byte pair count is nonzero. After return, r7:r6 should be written
; out into v_remc_hi:v_remc_lo. Leave at least 5 cycles between calls
; (including the cycles for the call instruction).
;
; The m74_spiload_core_nc variant doesn't check whether the input is drained,
; saving 3 cycles (so is 28 cycles long, but also demands 8 cycles between
; calls including the call instruction).
;
; r6:  Pre-load with v_remc_lo
; r7:  Pre-load with v_remc_hi
; Z:   Target memory area
; r22: Zero
; Returns:
; r4:  v_spis, SPI current state after operation, undefined if loading was
;      already completed.
; Clobbers:
; r24
;
m74_spiload_core:
	mov   r24,     r6      ; ( 1)
	or    r24,     r7      ; ( 2)
	breq  spil0            ; ( 3 /  4) No more 2 byte blocks to load
m74_spiload_core_nc:
	in    r24,     _SFR_IO_ADDR(SPDR) ; ( 4)
	out   _SFR_IO_ADDR(SPDR), r22     ; ( 5)
	lds   r4,      v_spis  ; ( 7) Load SPI state
	cp    r4,      r22     ; ( 8)
	brmi  spil1            ; ( 9 / 10) 0xFF: Normal loads
	breq  spil2            ; (10 / 11) 0x00: CRC byte pair
	; 0x01: Waiting for next block
	cpi   r24,     0xFE    ; (11)
	ldi   r24,     2       ; (12)
	brne  spil3            ; (13 / 14) Not 0xFE: Still busy
	sub   r4,      r24     ; (15) 0x01 => 0xFF: Normal loads
spil3:
	lpm   r24,     Z       ; (18) Dummy load (nop)
	lpm   r24,     Z       ; (21) Dummy load (nop)
	rjmp  .                ; (23)
	rjmp  spil5            ; (25)
spil1:
	; 0xFF: Normal loads
	st    Z+,      r24     ; (12)
	add   r6,      r4      ; (13) Update remaining bytes
	adc   r7,      r4      ; (14) Subtracts 1 by adding 0xFFFF
	lds   r24,     v_spibc ; (16) SPI 2 byte block downcount
	dec   r24              ; (17)
	sts   v_spibc, r24     ; (19)
	brne  spil4            ; (20 / 21)
	inc   r4               ; (21) Reaching CRC: 0xFF => 0x00
spil4:
	in    r24,     _SFR_IO_ADDR(SPDR) ; (22)
	out   _SFR_IO_ADDR(SPDR), r22     ; (23)
	st    Z+,      r24     ; (25)
spil5:
	sts   v_spis,  r4      ; (27) Save SPI state
	ret                    ; (31)
spil2:
	; 0x00: CRC loads
	inc   r4               ; (12) 0x00 => 0x01, Passing CRC, wait for next block
	lpm   r24,     Z       ; (15) Dummy load (nop)
	lpm   r24,     Z       ; (18) Dummy load (nop)
	lpm   r24,     Z       ; (21) Dummy load (nop)
	in    r24,     _SFR_IO_ADDR(SPDR) ; (22)
	out   _SFR_IO_ADDR(SPDR), r22     ; (23)
	rjmp  spil5            ; (25)
spil0:
	; No SPI loads (everything loaded)
	WAIT  r24,     23      ; (27)
	ret                    ; (31)



;
; Separator line with palette reload and stuff.
;
; Enters in cycle 1790. After doing its work, it returns to m74_scloop, cycle
; 1702 of the next line. For the documentation of what it displays, see the
; comments for m74_tdesc in videoMode74.s
;
; First pixel output in 24 tile wide mode has to be performed at cycle 352 (so
; OUT finishing in 353).
;
; r16: Scanline counter (increments it by one)
; r17: Logical row counter (increments it by one)
; r20: Byte 0 of tile descriptor
; r11: Byte 1 of tile descriptor
; r12: Byte 2 of tile descriptor
; r10: Byte 3 of tile descriptor
; r4:  Byte 4 of tile descriptor
; r13: Byte 5 of tile descriptor
; XL:  Byte 6 of tile descriptor
; XH:  Byte 7 of tile descriptor
; r23: Global configuration (m74_config)
; r14: RAM clear / SPI load address, low
; r15: RAM clear / SPI load address, high
; r22: Zero
; YH:  Palette buffer, high
;
; Everything expect r14, r15, r16, r17, r22, and YH may be clobbered.
;
m74_sl_separator:
	lds   r18,     m74_ldsl    ; (1792)
	lds   r6,      v_remc_lo   ; (1794)
	lds   r7,      v_remc_hi   ; (1796)

	; Prepare palette offset

	mov   r23,     r17     ; (1797)
	andi  r23,     0x7     ; (1798)
	swap  r23              ; (1799)
	add   XL,      r23     ; (1800)
	adc   XH,      r22     ; (1801)

	; Branch off for new / old palette load

	sbrs  r20,     1       ; (1802 / 1803)
	rjmp  sloldp           ; (1804) Color the separator line by old palette

	; Color the separator by new palette

	ldi   r25,     0x0F    ; (1804)
	movw  r8,      r10     ; (1805) r9:r8, r11:r10
	movw  r2,      r12     ; (1806) r3:r2, r13:r12
	mov   r23,     r4      ; (1807)
	mov   r19,     r4      ; (1808)
	swap  r11              ; (1809)
	and   r11,     r25     ; (1810) Color 0 index
	and   r9,      r25     ; (1811) Color 1 index
	swap  r12              ; (1812)
	and   r12,     r25     ; (1813) Color 2 index
	and   r2,      r25     ; (1814) Color 3 index
	swap  r10              ; (1815)
	and   r10,     r25     ; (1816) Color 4 index
	and   r8,      r25     ; (1817) Color 5 index
	swap  r23              ; (1818)
	and   r23,     r25     ; (1819) Color 6 index
	and   r19,     r25     ; (1820) Color 7 index
	swap  r13              ; (   1)
	and   r13,     r25     ; (   2) Color 8 index
	and   r3,      r25     ; (   3) Color 9 index

	;
	; The hsync_pulse part for the new scanline.
	;
	; The "update_sound" function destroys r0, r1, Z and the T flag in
	; SREG.
	;
	; HSYNC_USABLE_CYCLES:
	; 220 (Allowing 4CH audio + UART or 5CH audio)
	;
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	inc   r16              ; (   6) Physical scanline counter increment
	inc   r17              ; (   7) Logical row counter increment
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	WAIT  ZL,      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES
	movw  ZL,      r14     ; ( 233) ZH:ZL, r15:r14

	; Do a RAM clear / SPI load. It would have been even better to start
	; before the hsync in case the SD card is busy and needs little bits
	; of motivation to progress.
	rcall m74_sl_func      ; ( 279) 233 + 3 + 43

	; Fetch colors from new palette

	movw  r14,     ZL      ; ( 280) r15:r14, ZH:ZL
	movw  ZL,      XL      ; ( 281) ZH:ZL, XH:XL
	add   ZL,      r11     ; ( 282)
	adc   ZH,      r22     ; ( 283)
	sbrc  r20,     2       ; ( 284 /  285) RAM / ROM source select
	rjmp  slnram           ; ( 286) RAM source
	lpm   r11,     Z       ; () Color 0 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r9      ; ()
	adc   ZH,      r22     ; ()
	lpm   r9,      Z       ; () Color 1 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r12     ; ()
	adc   ZH,      r22     ; ()
	lpm   r12,     Z       ; () Color 2 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r2      ; ()
	adc   ZH,      r22     ; ()
	lpm   r2,      Z       ; () Color 3 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r10     ; ()
	adc   ZH,      r22     ; ()
	lpm   r10,     Z       ; () Color 4 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r8      ; ()
	adc   ZH,      r22     ; ()
	lpm   r8,      Z       ; () Color 5 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r23     ; ()
	adc   ZH,      r22     ; ()
	lpm   r23,     Z       ; () Color 6 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r19     ; ()
	adc   ZH,      r22     ; ()
	lpm   r19,     Z       ; () Color 7 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r13     ; ()
	adc   ZH,      r22     ; ()
	lpm   r13,     Z       ; () Color 8 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r3      ; ()
	adc   ZH,      r22     ; ()
	lpm   r3,      Z       ; ( 342) Color 9 value
slnrae:
	sbrc  r20,     4       ; ( 343)
	mov   r11,     r22     ; ( 344) 20 or 18 tiles width: No Color 0
	sbrc  r20,     4       ; ( 345)
	mov   r9,      r22     ; ( 346) 20 or 18 tiles width: No Color 1
	sbrc  r20,     3       ; ( 347)
	mov   r11,     r22     ; ( 348) 22 or 18 tiles width: No Color 0
	andi  r20,     0x18    ; ( 349)
	cpi   r20,     0x18    ; ( 350)
	brne  slnwd18          ; ( 351 / 352)
	mov   r12,     r22     ; ( 352) 18 tiles width: No color 2
slnwd18:
	out   PIXOUT,  r11     ; ( 353) Tile 0, Color 0
	movw  ZL,      r14     ; () ZH:ZL, r15:r14
	rcall m74_sl_func      ; () 354 + 3 + 43
	ld    r0,      X+      ; () Pal. 0 Start transferring palette
	clr   YL               ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	nop                    ; ()
	out   PIXOUT,  r9      ; ( 409) Tile 1, Color 1
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r12     ; ( 465) Tile 2, Color 2
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r2      ; ( 521) Tile 3, Color 3
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	ld    r24,     X+      ; () Pal. 1
	rcall m74_setpalcol    ; () 3 + 36
	ld    r0,      X+      ; () Pal. 2
	out   PIXOUT,  r10     ; ( 577) Tile 4, Color 4
	rcall m74_sl_func      ; () 3 + 43
	inc   YL               ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r8      ; ( 633) Tile 5, Color 5
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r23     ; ( 689) Tile 6, Color 6
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r19     ; ( 745) Tile 7, Color 7
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	ld    r24,     X+      ; () Pal. 3
	rcall m74_setpalcol    ; () 3 + 36
	ld    r24,     X+      ; () Pal. 4
	ld    r0,      X+      ; () Pal. 5
	inc   YL               ; ()
	nop                    ; ()
	out   PIXOUT,  r13     ; ( 801) Tile 8, Color 8
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r3      ; ( 857) Tile 9, Color 9
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r3      ; ( 913) Tile 10, Color 9
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r3      ; ( 969) Tile 11, Color 9
	st    Y+,      r0      ; ()
	ld    r24,     X+      ; () Pal. 6
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	nop                    ; ()
	ld    r0,      X+      ; () Pal. 7
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r3      ; (1025) Tile 12, Color 9
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r3      ; (1081) Tile 13, Color 9
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r3      ; (1137) Tile 14, Color 9
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	ld    r24,     X+      ; () Pal. 8
	rcall m74_setpalcol    ; () 3 + 36
	ld    r24,     X+      ; () Pal. 9
	ld    r0,      X+      ; () Pal. 10
	ld    r1,      X+      ; () Pal. 11
	out   PIXOUT,  r13     ; (1193) Tile 15, Color 8
	inc   YL               ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r19     ; (1249) Tile 16, Color 7
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r23     ; (1305) Tile 17, Color 6
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r8      ; (1361) Tile 18, Color 5
	st    Y+,      r0      ; ()
	mov   r24,     r1      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	inc   YL               ; ()
	ld    r0,      X+      ; () Pal. 12
	ld    r1,      X+      ; () Pal. 13
	ld    r3,      X+      ; () Pal. 14
	ld    r8,      X+      ; () Pal. 15
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
	out   PIXOUT,  r10     ; (1417) Tile 19, Color 4
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
	st    Y+,      r0      ; ()
	st    Y+,      r0      ; ()
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
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	nop                    ; ()
	out   PIXOUT,  r2      ; (1473) Tile 20, Color 3
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	st    Y+,      r1      ; ()
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r3      ; ()
	out   PIXOUT,  r12     ; (1529) Tile 21, Color 2
	rcall m74_sl_func      ; () 3 + 43
	nop                    ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	out   PIXOUT,  r9      ; (1585) Tile 22, Color 1
	rcall m74_sl_func      ; () 3 + 43
	mov   r24,     r8      ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	out   PIXOUT,  r11     ; (1641) Tile 23, Color 0
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	st    Y+,      r3      ; ()
	rcall m74_setpalcol    ; () 3 + 36
	sts   v_remc_lo, r6    ; () Restore stuff after RAM clear / SPI load
	out   PIXOUT,  r22     ; (1697) Termination
	sts   v_remc_hi, r7    ; ()
	movw  r14,     ZL      ; () r15:r14, ZH:ZL
	rjmp  m74_scloop       ; (1702)

slnram:
	; New palette from RAM
	ld    r11,     Z       ; () Color 0 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r9      ; ()
	adc   ZH,      r22     ; ()
	ld    r9,      Z       ; () Color 1 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r12     ; ()
	adc   ZH,      r22     ; ()
	ld    r12,     Z       ; () Color 2 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r2      ; ()
	adc   ZH,      r22     ; ()
	ld    r2,      Z       ; () Color 3 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r10     ; ()
	adc   ZH,      r22     ; ()
	ld    r10,     Z       ; () Color 4 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r8      ; ()
	adc   ZH,      r22     ; ()
	ld    r8,      Z       ; () Color 5 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r23     ; ()
	adc   ZH,      r22     ; ()
	ld    r23,     Z       ; () Color 6 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r19     ; ()
	adc   ZH,      r22     ; ()
	ld    r19,     Z       ; () Color 7 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r13     ; ()
	adc   ZH,      r22     ; ()
	ld    r13,     Z       ; () Color 8 value
	movw  ZL,      XL      ; () ZH:ZL, XH:XL
	add   ZL,      r3      ; ()
	adc   ZH,      r22     ; ()
	ld    r3,      Z       ; ( 333) Color 9 value
	lpm   r24,     Z       ; ( 336) Dummy load (nop)
	rjmp  .                ; ( 338)
	rjmp  .                ; ( 340)
	rjmp  slnrae           ; ( 342)

sloldp:
	; Color the separator by old palette
	mov   YL,      r11     ; (1805)
	ld    r11,     Y       ; (1807) Color 0 value
	swap  YL               ; (1808)
	ld    r9,      Y       ; (1810) Color 1 value
	mov   YL,      r12     ; (1811)
	ld    r12,     Y       ; (1813) Color 2 value
	swap  YL               ; (1814)
	ld    r2,      Y       ; (1816) Color 3 value
	mov   YL,      r10     ; (1817)
	ld    r10,     Y       ; (1819) Color 4 value
	swap  YL               ; (1820)
	ld    r8,      Y       ; (   2) Color 5 value
	mov   YL,      r4      ; (   3)

	;
	; The hsync_pulse part for the new scanline.
	;
	; The "update_sound" function destroys r0, r1, Z and the T flag in
	; SREG.
	;
	; HSYNC_USABLE_CYCLES:
	; 220 (Allowing 4CH audio + UART or 5CH audio)
	;
	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; (   5)
	inc   r16              ; (   6) Physical scanline counter increment
	inc   r17              ; (   7) Logical row counter increment
	ldi   ZL,      2       ; (   8)
	call  update_sound     ; (  12) (+ AUDIO)
	WAIT  ZL,      HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES
	movw  ZL,      r14     ; ( 233) ZH:ZL, r15:r14

	; Do a RAM clear / SPI load. It would have been even better to start
	; before the hsync in case the SD card is busy and needs little bits
	; of motivation to progress.
	rcall m74_sl_func      ; ( 279) 233 + 3 + 43

	; Continue with colors

	ld    r23,     Y       ; ( 281) Color 6 value
	swap  YL               ; ( 282)
	ld    r19,     Y       ; ( 284) Color 7 value
	mov   YL,      r13     ; ( 285)
	ld    r13,     Y       ; ( 287) Color 8 value
	swap  YL               ; ( 288)
	ld    r3,      Y       ; ( 290) Color 9 value

	; An extra RAM clear / SPI load fits before branching back

	rcall m74_sl_func      ; ( 336) 290 + 3 + 43
	movw  r14,     ZL      ; ( 337) r15:r14, ZH:ZL
	lpm   r24,     Z       ; ( 340) Dummy load (nop)
	rjmp  slnrae           ; ( 342)



;
; Performs either SPI load or RAM clear function as needed
;
; r16: Scanline counter
; r18: Pre-loaded with m74_ldsl
; r6:  Pre-load with v_remc_lo
; r7:  Pre-load with v_remc_hi
; Z:   Target memory area
; r22: Zero
; Clobbers:
; r4, r5, r24
;
m74_sl_func:
	cp    r18,     r16     ; ( 1) Compare start with current line
	brcc  slf0             ; ( 2 /  3) Function may only run if reached
	sbrs  r7,      7       ; ( 3 /  4) RAM clear or SPI load select
	rjmp  slf1             ; ( 5) SPI load
	cp    r6,      r22     ; ( 5)
	breq  slf2             ; ( 6 /  7) No more 16 byte blocks to process
	st    Z+,      r22     ; ( 8)
	st    Z+,      r22     ; (10)
	st    Z+,      r22     ; (12)
	st    Z+,      r22     ; (14)
	st    Z+,      r22     ; (16)
	st    Z+,      r22     ; (18)
	st    Z+,      r22     ; (20)
	st    Z+,      r22     ; (22)
	st    Z+,      r22     ; (24)
	st    Z+,      r22     ; (26)
	st    Z+,      r22     ; (28)
	st    Z+,      r22     ; (30)
	st    Z+,      r22     ; (32)
	st    Z+,      r22     ; (34)
	st    Z+,      r22     ; (36)
	st    Z+,      r22     ; (38)
	dec   r6               ; (39)
	ret                    ; (43)
slf0:
	WAIT  r24,     36      ; (39)
	ret                    ; (43)
slf1:
	lpm   r24,     Z       ; ( 8) Dummy load (nop)
	lpm   r24,     Z       ; (11) Dummy load (nop)
	rjmp  m74_spiload_core ; (43) 11 + 2 + 31
slf2:
	WAIT  r24,     32      ; (39)
	ret                    ; (43)
