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



.section .text



sub_video_mode74:

	; Entry happens in cycle 467.

	; Check for display enable

	lds   r19,     m74_config   ; ( 469)
	sbrs  r19,     7       ; ( 470 /  471)
	rjmp  ddis             ; ( 472) Display disabled

	; Initialize SD loading (as needed)

#if (M74_SD_ENABLE != 0)
	sbrs  r19,     6       ; ( 472 /  473)
	rjmp  sdldis           ; ( 474) SD loading disabled
	cbi   _SFR_IO_ADDR(PORTD), 6  ; ( 475) Assert Chip Select for the SD card
	ldi   r22,     0x51           ; ( 476) CMD 17 (decimal 17 OR 0x40)
	out   _SFR_IO_ADDR(SPDR), r22 ; ( 477) Command
	lds   r16,     m74_sdsec_0  ; ( 479)
	lds   r17,     m74_sdsec_1  ; ( 481)
	lds   r18,     m74_sdsec_2  ; ( 483)
	lsl   r16              ; ( 484)
	rol   r17              ; ( 485)
	rol   r18              ; ( 486) Loaded multiplied with 512
	lds   r22,     m74_sdoff_0  ; ( 488)
	lds   r23,     m74_sdoff_1  ; ( 490)
	lds   r24,     m74_sdoff_2  ; ( 492)
	lds   r25,     m74_sdoff_3  ; ( 494)
	add   r23,     r16     ; ( 495)
	adc   r24,     r17     ; ( 496)
	adc   r25,     r18     ; ( 497)
	out   _SFR_IO_ADDR(SPDR), r25 ; ( 498) Address byte 3
	andi  r19,     0xBF    ; ( 499) Disable SD loading for next frame
	sts   m74_config, r19  ; ( 501)
	lds   r15,     m74_sddst_hi ; ( 503)
	lds   r14,     m74_sddst_lo ; ( 505)
	lds   r21,     m74_sdcnt    ; ( 507)
	movw  r16,     r22     ; ( 508)
	lsr   r17              ; ( 509)
	ror   r16              ; ( 510) Skip count in r16
	sts   v_srems, r16     ; ( 512)
	neg   r16              ; ( 513) Byte pairs remaining after the initial skip (0: 256)
	rjmp  .                ; ( 515)
	out   _SFR_IO_ADDR(SPDR), r24 ; ( 516) Address byte 2
	brne  sdl0             ; ( 517 /  518)
	rjmp  .                ; ( 519) Any count of byte pairs may be loaded (full sector available)
	lpm   r0,      Z       ; ( 522) Dummy load (nop)
	rjmp  sdl1             ; ( 524)
sdldis:
	ldi   r24,     0xFF    ; ( 475)
	sts   v_sstat, r24     ; ( 477) Disable loading in HSync by marking it completed
#if (M74_RESET_ENABLE != 0)
	M74WT_R24      92      ; ( 569)
	rjmp  sdldie           ; ( 571)
#else
	M74WT_R24      109     ; ( 586)
	rjmp  sdle             ; ( 588)
#endif
sdl0:
	cp    r16,     r21     ; ( 519)
	brcc  .+2              ; ( 520 /  521)
	mov   r21,     r16     ; ( 521) Limit loading to sector boundary
	cpi   r21,     0       ; ( 522)
	brne  .+2              ; ( 523 /  524)
	mov   r21,     r16     ; ( 524) Special: Full sector load request with no full sector
sdl1:
	sts   v_sremc, r21     ; ( 526) Remaining byte pair to load count
	sub   r16,     r21     ; ( 527) Byte pairs remaining after the skip + load (0: zero)
	inc   r16              ; ( 528) Add the CRC byte pair
	sts   v_sreme, r16     ; ( 530)
	rjmp  .                ; ( 532)
	andi  r23,     0xFE    ; ( 533)
	out   _SFR_IO_ADDR(SPDR), r23 ; ( 534) Address byte 1
	clr   r16              ; ( 535)
	sts   v_sstat, r16     ; ( 537) Nothing loaded yet
	M74WT_R24      14      ; ()
	out   _SFR_IO_ADDR(SPDR), r22 ; ( 552) Address byte 0 (zero)
	M74WT_R24      15      ; ()
	ldi   r22,     0x95    ; () Use the default init CRC (lowest bit set, bit unsure if that's necessary)
	ldi   r21,     0xFF    ; () Send an extra byte, discarding last byte before command completion.
	out   _SFR_IO_ADDR(SPDR), r22 ; ( 570) Empty CRC
#if (M74_RESET_ENABLE != 0)
	ori   r19,     0x40    ; ( 571) Restore SD load enabled flag in m74_config
sdldie:
	lds   r24,     m74_reset_lo   ; ( 573)
	lds   r25,     m74_reset_hi   ; ( 575)
	or    r24,     r25     ; ( 576) If zero, then no reset
	ldi   r24,     lo8(M74_VIDEO_STACK - 1) ; ( 577) Set up frame render stack
	ldi   r25,     hi8(M74_VIDEO_STACK - 1) ; ( 578)
	breq  .+2              ; ( 579)
	out   STACKL,  r24     ; ( 580)
	breq  .+2              ; ( 581)
	out   STACKH,  r25     ; ( 582)
	M74WT_R24      4       ; ( 586)
	sbrc  r19,     6       ; ( 587 /  588) Jump over SD card output if SD was disabled
#else
	M74WT_R24      17      ; ( 587)
#endif
	out   _SFR_IO_ADDR(SPDR), r21 ; ( 588)
sdle:
#else
#if (M74_RESET_ENABLE != 0)
	lds   r24,     m74_reset_lo   ; ()
	lds   r25,     m74_reset_hi   ; ()
	or    r24,     r25     ; () If zero, then no reset
	ldi   r24,     lo8(M74_VIDEO_STACK - 1) ; () Set up frame render stack
	ldi   r25,     hi8(M74_VIDEO_STACK - 1) ; ()
	breq  .+2              ; ()
	out   STACKL,  r24     ; ()
	breq  .+2              ; ()
	out   STACKH,  r25     ; ()
	M74WT_R24      106     ; ( 588)
#else
	M74WT_R24      117     ; ( 588)
#endif
#endif

	; Load palette

	ldi   YH,      M74_PALBUF_H ; ( 589)
#if (M74_PAL_PTRE != 0)
	lds   ZL,      m74_pal_lo   ; ( 591)
	lds   ZH,      m74_pal_hi   ; ( 593)
#else
	ldi   ZL,      lo8(M74_PAL_OFF) ; ( 590)
	ldi   ZH,      hi8(M74_PAL_OFF) ; ( 591)
	rjmp  .                ; ( 593)
#endif
	clr   YL               ; ( 594)
lcloop:
	sbrs  r19,     3       ; ( 1)
	rjmp  .+4              ; ( 3)
	ld    r24,     Z+      ; ( 4)
	rjmp  .+2              ; ( 6)
	lpm   r24,     Z+      ; ( 6)
	rcall m74_setpalcol    ; (45) (3 + 36 cycles)
	inc   YL               ; (46)
#if (M74_SD_ENABLE != 0)
	movw  r16,     ZL      ; ( 1)
	movw  ZL,      r14     ; ( 2)
	rcall m74_spiload_core ; (37) (3 + 32 cycles)
	movw  r14,     ZL      ; (38)
	movw  ZL,      r16     ; (39)
#else
	M74WT_R24      39      ; (39)
#endif
	sbrs  r19,     3       ; ( 1)
	rjmp  .+4              ; ( 3)
	ld    r24,     Z+      ; ( 4)
	rjmp  .+2              ; ( 6)
	lpm   r24,     Z+      ; ( 6)
	rcall m74_setpalcol    ; (45) (3 + 36 cycles)
	inc   YL               ; (46)
	brne  lcloop           ; (47 / 48) (1063 cy total; at 1657 here)

	; Initializing for the scanline loop

	clr   r16              ; ( 1) Scanline counter
#if (M74_ROWS_PTRE != 0)
	lds   ZL,      m74_rows_lo  ; ( 3)
	lds   ZH,      m74_rows_hi  ; ( 5)
#else
	ldi   ZL,      lo8(M74_ROWS_OFF) ; ( 2)
	ldi   ZH,      hi8(M74_ROWS_OFF) ; ( 3)
	rjmp  .                ; ( 5)
#endif
	; Load the first two values to get the initial scanline.
	ld    r17,     Z+      ; ( 7) Load first logical row counter
	adiw  ZL,      1       ; ( 9)
	sts   v_rows_lo, ZL    ; (11)
	sts   v_rows_hi, ZH    ; (13)

#if (M74_M3_ENABLE != 0)
	; Initialize 2bpp Multicolor mode

#if (M74_M3_PTRE != 0)
	lds   r18,     m74_mcadd_lo ; (15)
	lds   r19,     m74_mcadd_hi ; (17)
#else
	ldi   r18,     lo8(M74_M3_OFF)   ; (14)
	ldi   r19,     hi8(M74_M3_OFF)   ; (15)
	rjmp  .                ; (17)
#endif
	subi  r18,     1       ; (18) Stack is pre-incrementing, so correct
	sbci  r19,     0       ; (19)
	sts   v_m3ptr_lo, r18  ; (21)
	sts   v_m3ptr_hi, r19  ; (23)
#else
	M74WT_R24      10      ; (23) (At 1680 here)
#endif

	; Sandwiched between waits so it is simpler to shift it a bit around
	; when tweaking the scanline loop.

	M74WT_R24      19      ; (1699)
	call  m74_scloop       ; (1712)
	M74WT_R24      27      ; (1739)

	; Do some SD loads, then complete it by writing out address, used to
	; finish the function if necessary.

#if (M74_SD_ENABLE != 0)
	movw  ZL,      r14     ; ( 1)
	rcall m74_spiload_core ; (36) (3 + 32 cycles)
	nop                    ; (37) Gap for SPI
	rcall m74_spiload_core ; (72) (3 + 32 cycles)
	sts   m74_sddst_lo, ZL ; (74)
	sts   m74_sddst_hi, ZH ; (76)
#else
	M74WT_R24      76      ; (76)
#endif

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

#if (M74_RESET_ENABLE != 0)
	lds   r24,     m74_reset_lo
	lds   r25,     m74_reset_hi
	mov   r1,      r24
	or    r1,      r25
	brne  .+2
	ret                    ; Reset offset is zero: No reset
	ldi   r22,     lo8(M74_MAIN_STACK - 1) ; () Set up main program stack
	ldi   r23,     hi8(M74_MAIN_STACK - 1) ; ()
	out   STACKL,  r22     ; ()
	out   STACKH,  r23     ; ()
	clr   r1               ; For C language routines, r1 is zero
	push  r24              ; Return address is the reset vector
	push  r25
	reti                   ; All done, return empties stack
#else
	ret                    ; All done
#endif

ddis:
	; Display Disabled frame

#if (M74_SD_ENABLE != 0)
	ldi   r24,     0xFF    ; ( 473)
	sts   v_sstat, r24     ; ( 475) Set it so M74_Finish will operate correctly without display enable
	M74WT_R24      1814 - 475
#else
	M74WT_R24      1814 - 472
#endif
	clr   r16              ; (1815) Scanline counter
ddisl:
	lds   r23,     render_lines_count ; (1817)
	cp    r23,     r16     ; (1818)
#if (M74_RESET_ENABLE == 0)
	breq  laend            ; (1819 / 1820)
#else
	breq  ddise            ; (1819 / 1820) Don't alter stack if display is disabled
#endif
	inc   r16              ; (1820)
	rcall hsync_pulse      ; (21 + AUDIO)
	M74WT_R24      1813 - 21 - AUDIO_OUT_HSYNC_CYCLES
	rjmp  ddisl            ; (1815)
#if (M74_RESET_ENABLE != 0)
ddise:
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
#endif




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
; Waits the given amount of cycles, assuming calling with "rcall".
;
; This routine is used to reduce the size of the video mode, these waits
; taking only two words (ldi + rcall), yet having the same effect like the
; WAIT macro.
;
; r24: Number of cycles to wait - 11 (not including the "ldi r24, ...").
;      Must be at least 4.
;
m74_wait:
	lsr   r24
	brcs  .                ; +1 if bit0 was set
	lsr   r24
	brcs  .                ; +1 if bit1 was set
	brcs  .                ; +1 if bit1 was set
	dec   r24
	nop
	brne  .-6              ; 4 cycle loop
	ret

m74_wait_15:
	nop
m74_wait_14:
	nop
m74_wait_13:
	nop
m74_wait_12:
	nop
m74_wait_11:
	nop
m74_wait_10:
	nop
m74_wait_9:
	nop
m74_wait_8:
	nop
m74_wait_7:
	ret



#if (M74_SD_ENABLE != 0)
;
; SD load function
;
; Loads data from an SD card sector in 2 byte units. With the rcall, takes
; 35 cycles (needs 1 cycle gap between calls for proper SPI operation).
;
; Variables:
; v_sstat: SPI load state
;          --------: State 0: Wait for a 0xFE return.
;          bit4 set: State 1: Skip until the requested offset (v_srems).
;          bit5 set: State 3: Skip until reading past CRC (v_sreme).
;          bit6 set: State 2: Read in the requested byte pairs (v_sremc).
;          bit7 set: State 4: Do nothing.
;          The higher bit has the higher priority.
; v_srems: Remaining byte pairs to skip. Used in State 1.
; v_sremc: Remaining byte pairs to load. Used in State 2. The value of 0 is
;          interpreted as 256.
; v_sreme: Remaining bytes to skip until end. Used in State 3 to complete 514
;          reads (512 data + 2 CRC). The value 0 is interpreted as 256.
; Registers:
;       Z: Target memory area
; Returns:
;       Z: Updated if bytes were loaded
;       C: Set when State 4 was reached
; Clobbers:
; r6, r7, r24
;
m74_spiload_core:
	lds   r6,      v_sstat ; ( 2)
	sbrc  r6,      7       ; ( 3 /  4)
	rjmp  spil_s4          ; ( 5)
	ldi   r24,     0xFF    ; ( 5)
	in    r7,      _SFR_IO_ADDR(SPDR) ; ( 6)
	out   _SFR_IO_ADDR(SPDR), r24     ; ( 7)
	sbrc  r6,      6       ; ( 8 /  9)
	rjmp  spil_s2          ; (10)
	sbrc  r6,      5       ; (10 / 11)
	rjmp  spil_s3          ; (12)
	sbrc  r6,      4       ; (12 / 13)
	rjmp  spil_s1          ; (14)

	; State 0: Wait for 0xFE

	ldi   r24,     0xFE    ; (14)
	cp    r24,     r7      ; (15)
	ldi   r24,     0x10    ; (16)
	brne  .+2              ; (17 / 18)
	mov   r6,      r24     ; (18) State 1: Skip byte pairs
	lds   r24,     v_srems ; (20) Any byte pairs to skip?
	cpi   r24,     0       ; (21)
	brne  spil_s0_0        ; (22 / 23)
	ldi   r24,     0x40    ; (23)
	sbrc  r6,      4       ; (24) Only if state is changing (so it was 0xFE)
	mov   r6,      r24     ; (25) State 2: Read in byte pairs
spil_s0_1:
	sts   v_sstat, r6      ; (27)
	clc                    ; (28)
	ret                    ; (32)
spil_s0_0:
	rjmp  spil_s0_1        ; (25)

	; State 1: Skip until the requested offset is reached

spil_s1:
	lds   r24,     v_srems ; (16)
	dec   r24              ; (17)
	sts   v_srems, r24     ; (19)
	ldi   r24,     0x40    ; (20)
spil_s3_e:
	brne  .+2              ; (21 / 22)
	mov   r6,      r24     ; (22) State 2: Read in byte pairs
	ldi   r24,     0xFF    ; (23)
	in    r7,      _SFR_IO_ADDR(SPDR) ; (24)
	out   _SFR_IO_ADDR(SPDR), r24     ; (25)
	sts   v_sstat, r6      ; (27)
	clc                    ; (28)
	ret                    ; (32)

	; State 2: Read in the requested byte pairs

spil_s2:
	st    Z+,      r7      ; (12)
	lds   r24,     v_sremc ; (14)
	dec   r24              ; (15)
	sts   v_sremc, r24     ; (17)
	ldi   r24,     0x20    ; (18)
	brne  .+2              ; (19 / 20)
	mov   r6,      r24     ; (20) State 3: Skip until end
	sts   v_sstat, r6      ; (22)
	ldi   r24,     0xFF    ; (23)
	in    r7,      _SFR_IO_ADDR(SPDR) ; (24)
	out   _SFR_IO_ADDR(SPDR), r24     ; (25)
	st    Z+,      r7      ; (27)
	clc                    ; (28)
	ret                    ; (32)

	; State 3: Skip until reading past CRC

spil_s3:
	lds   r24,     v_sreme ; (14)
	dec   r24              ; (15)
	sts   v_sreme, r24     ; (17)
	ldi   r24,     0x80    ; (18)
	brne  spil_s3_e        ; (19 / 20) Not at end: Use State 1's return path
	rjmp  .                ; (21)
	mov   r6,      r24     ; (22)
	ldi   r24,     0xFF    ; (23)
	in    r7,      _SFR_IO_ADDR(SPDR) ; (24)
	out   _SFR_IO_ADDR(SPDR), r24     ; (25)
	sts   v_sstat, r6      ; (27)
	sec                    ; (28) Carry set indicating end of data
	ret                    ; (32)

	; State 4: Completed

spil_s4:
	M74WT_R24      22      ; (27)
	sec                    ; (28) Carry set indicating end of data
	ret                    ; (32)
#endif



;
; unsigned char M74_Finish(void);
;
; Finishes the SD load started within the video display. Returns nonzero if
; the load failed.
;
; Returns:
; r25:r24: 0 if succeed
; Clobbers:
; r18, r19, Z
;
M74_Finish:
#if (M74_SD_ENABLE != 0)

	; v_sstat:
	; Bits 4-7 inclusive indicate the state of loading.
	; Bit 3 will be used to indicate whether the card was clocked out. If
	; set, then the clock-out completed, so do nothing.

	clr   r25              ; For return value
	lds   r24,     v_sstat
	sbrc  r24,     3
	rjmp  lficomp          ; SD load completed, clocked out
	sbrc  r24,     7
	rjmp  lfilo            ; SD load completed, needs clocking out
	cpi   r24,     0x10
	brcs  lfifail          ; Still waiting for 0xFE: Assume failed
	movw  r18,     r6      ; Save r7:r6 into r19:r18 (Calling convention!)
	lds   ZL,      m74_sddst_lo
	lds   ZH,      m74_sddst_hi
lfilp:
	rcall m74_spiload_core ; Finish the load (from here it will finish in finite time)
	brcs  lfile            ; 1 cycle gap, OK
	rcall m74_spiload_core
	brcs  lfile
	rcall m74_spiload_core
	brcs  lfile
	rcall m74_spiload_core
	brcc  lfilp
lfile:
	movw  r6,      r18
lfilo:
	clr   r24
lfifae:
	sbi   _SFR_IO_ADDR(PORTD), 6  ; Deassert Chip Select for the SD card
	ldi   r25,     0xFF
	out   _SFR_IO_ADDR(SPDR), r25 ; Clock out the SD card
	sts   v_sstat, r25     ; Just set all bits (0xFF), so bit 3 too becomes set (clocked out)
	clr   r25
	ret
lficomp:
	clr   r24
	ret
lfifail:
	ldi   r24,     1
	rjmp  lfifae
#else
	clr   r25
	clr   r24
	ret
#endif
