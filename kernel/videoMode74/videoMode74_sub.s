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
	ldi   YH,      M74_PALBUF_H ; ( 477)
#if (M74_PAL_PTRE != 0)
	lds   ZL,      m74_pal_lo   ; ( 479)
	lds   ZH,      m74_pal_hi   ; ( 481)
#else
	ldi   ZL,      lo8(M74_PAL_OFF) ; ( 478)
	ldi   ZH,      hi8(M74_PAL_OFF) ; ( 479)
	rjmp  .                ; ( 481)
#endif
	clr   YL               ; ( 482)
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
	brne  lcloop           ; (47 / 48) (767 cy total; at 1249 here)

	; Check for user video mode, after palette load

	nop                    ; (1250)
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
	ld    r2,      Z+      ; (1268) Load new X scroll
	sts   v_shifto,  r2    ; (1270)
lrese:
	sts   v_rows_lo, ZL    ; (1272)
	sts   v_rows_hi, ZH    ; (1274)

	; Initializing RAM clear or SPI load function

	lds   r14,     m74_totc     ; (1276)
	lds   r15,     m74_skip     ; (1278)
	sts   v_remc, r14           ; (1280) Remaining blocks: Total blocks
	sts   v_rems, r15           ; (1282) Remaining skips: Total skips
	lds   r14,     m74_fadd_lo  ; (1284) Target address in r15:r14
	lds   r15,     m74_fadd_hi  ; (1286)

#if (M74_M3_ENABLE != 0)
	; Initialize 2bpp Multicolor mode

#if (M74_M3_PTRE != 0)
	lds   r18,     m74_mcadd_lo ; (1288)
	lds   r19,     m74_mcadd_hi ; (1290)
#else
	ldi   r18,     lo8(M74_M3_OFF)   ; (1287)
	ldi   r19,     hi8(M74_M3_OFF)   ; (1288)
	rjmp  .                ; (1290)
#endif
	subi  r18,     1       ; (1291) Stack is pre-incrementing, so correct
	sbci  r19,     0       ; (1292)
	sts   v_m3ptr_lo, r18  ; (1294)
	sts   v_m3ptr_hi, r19  ; (1296)
#else
	WAIT  r18,     10      ; (1296)
#endif

	; Sandwiched between waits so it is simpler to shift it a bit around
	; when tweaking the scanline loop.

	WAIT  r18,     400     ; (1696)
	call  m74_scloop       ; (1709)
	WAIT  r18,     102     ; (1811)

	; Complete RAM clear / SPI load by writing out current address, used
	; to finish the function.

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
; Processes 2 bytes per call from the SPI bus, either as skip or block load
; depending on the state of the respective counters. It can not cross SD card
; block boundary, but this way, without handling that, it is also compatible
; with an SPI RAM, just loading data. After return, r7:r6 should be written
; out into v_remc and v_rems.
;
; 0xFF on v_rems indicates that the load is completed. 0x00 on v_remc is
; treated as 256. Leave at least 8 cycles between calls (including the cycles
; for the call instruction).
;
; r6:  Pre-load with v_remc
; r7:  Pre-load with v_rems
; Z:   Target memory area
; r23: Zero
; Returns:
; r6, r7, Z updated, Carry set on end of load, clear otherwise.
; Clobbers:
; r24
;
; Note: The m74_spiload_core normal entry point is not used, so it is removed
; from the code, but left here to see how it would look like completed.
;
;m74_spiload_core:
;	inc   r7               ; ( 1)
;	breq  spil0            ; ( 2 /  3) End of SPI load (v_rems is 0xFF)
m74_spiload_core_nc:
	in    r24,     _SFR_IO_ADDR(SPDR) ; ( 3)
	out   _SFR_IO_ADDR(SPDR), r23     ; ( 4)
	dec   r7               ; ( 5) Restore v_rems, also setting zero flag if in loading stage
	brne  spil1            ; ( 6 /  7) If nonzero, then skipping, otherwise loading
	st    Z+,      r24     ; ( 8) Store loaded byte
	dec   r6               ; ( 9) Will have one block less to load
	breq  spil2            ; (10 / 11) Reached zero?
	clc                    ; (11) Not at end of load
	rjmp  spil3            ; (13)
spil2:
	dec   r7               ; (12) If so, then set v_rems 0xFF, so SPI load ends
	sec                    ; (13) Indicate end of load with Carry set
spil3:
	lpm   r24,     Z       ; (16) Dummy load (nop)
	lpm   r24,     Z       ; (19) Dummy load (nop)
	nop                    ; (20)
	in    r24,     _SFR_IO_ADDR(SPDR) ; (21)
	out   _SFR_IO_ADDR(SPDR), r23     ; (22)
	st    Z+,      r24     ; (24)
spil4:
	ret                    ; (28)
;spil0:
;	; No SPI loads (everything loaded)
;	WAIT  r24,     20      ; (23)
;	sec                    ; (24) Indicate end of load with Carry set
;	ret                    ; (28)
spil1:
	dec   r7               ; ( 8) One block less to skip
	clc                    ; ( 9) Not at end of load
	lpm   r24,     Z       ; (12) Dummy load (nop)
	lpm   r24,     Z       ; (15) Dummy load (nop)
	lpm   r24,     Z       ; (18) Dummy load (nop)
	rjmp  .                ; (20)
	in    r24,     _SFR_IO_ADDR(SPDR) ; (21)
	out   _SFR_IO_ADDR(SPDR), r23     ; (22)
	rjmp  spil4            ; (24)



;
; Performs either SPI load or RAM clear function as needed
;
; r16: Scanline counter
; r18: Pre-loaded with m74_ldsl
; r6:  Pre-load with v_remc
; r7:  Pre-load with v_rems
; Z:   Target memory area
; r23: Zero
; Clobbers:
; r24
;
m74_sl_func:
	cp    r18,     r16     ; ( 1) Compare start with current line
	brcc  slf0             ; ( 2 /  3) Function may only run if reached
	inc   r7               ; ( 3)
	brne  slf1             ; ( 4 /  5) RAM clear or SPI load select
	cp    r6,      r23     ; ( 5)
	breq  slf2             ; ( 6 /  7) No more 16 byte blocks to process
	st    Z+,      r23     ; ( 8)
	st    Z+,      r23     ; (10)
	st    Z+,      r23     ; (12)
	st    Z+,      r23     ; (14)
	st    Z+,      r23     ; (16)
	st    Z+,      r23     ; (18)
	st    Z+,      r23     ; (20)
	st    Z+,      r23     ; (22)
	st    Z+,      r23     ; (24)
	st    Z+,      r23     ; (26)
	st    Z+,      r23     ; (28)
	st    Z+,      r23     ; (30)
	st    Z+,      r23     ; (32)
	st    Z+,      r23     ; (34)
	st    Z+,      r23     ; (36)
	st    Z+,      r23     ; (38)
	dec   r6               ; (39)
	dec   r7               ; (40) Restore r7 (v_rems)
	ret                    ; (44)
slf1:
	lpm   r24,     Z       ; ( 8) Dummy load (nop)
	lpm   r24,     Z       ; (11) Dummy load (nop)
	lpm   r24,     Z       ; (14) Dummy load (nop)
	rjmp  .                ; (16)
	rjmp  m74_spiload_core_nc   ; (44) 2 + 26
slf0:
	rjmp  .                ; ( 5)
	rjmp  .                ; ( 7)
slf2:
	WAIT  r24,     33      ; (40)
	ret                    ; (44)
