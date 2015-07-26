/*
 *  Uzebox Kernel - Mode 14
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published bys
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

;***************************************************
; Video Mode 14 - Cunning Fellow
; 256x224
; Section 1 (used for life / score / highscore / messages)
;    256x16
;    1 Bpp / Monochrome ROM tiles
;    Single 256 entry font table
;    Foreground colour changeable per scanline
;
; Section 2 (playfield)
;    256x208
;    2 Bpp RamTiles
;    PLUS 1 Bpp Background off SD card
;    128 Ram Tiles
;    Background Colour Fixed at BLACK
;    Background 1 Bpp bitmap Fixed at Medium Blue
;    3 foreground colours changeable per scanline
; RAM Usage
;   - RAM_Tiles 2048b = 128x16 byte 8x8 pixel tiles
;   - VRAM 896b = 32x28 cells as
;         32x2  for section 1
;         32x26 for section 2
;***************************************************	


.global nextFreeRamTile
.global ColourMask
.global FrameNo

.global waitNFrames
.global DisplayLogo
.global VideoModeVsync
.global SDCardVideoModeEnable
.global SDCardVideoModeDisable
.global sd_card_inter_block_bytes
.global InitializeVideoMode
.global ClearBufferFinal
.global ClearBuffer
.global ClearBufferTextMode

.global SetHsyncCallback
.global DefaultCallback
.global flashingCallback
.global BlankingCallback
.global titleCallback
.global creditCallback

.global bresh_line_C
.global SetPixelFastC
.global CosMulFastC
.global SinMulFastC
.global CosFastC
.global SinFastC
.global MulSU
.global renderObjects

.global decLives
.global incLives
.global addScore
.global addScoreFull

.global drawclawdeath
.global drawdroid
.global drawflipper
.global drawmutflipper
.global drawbullet
.global drawlaser
.global drawzap
.global drawspiker
.global drawspike
.global drawtanker
.global drawfuseball
.global drawmirror
.global drawpowerup
.global drawpowerupb
.global drawclaw
.global drawpulsar
.global drawdemona
.global drawdemonb
.global drawexplosion
.global drawyesyesyes
.global drawoneup
.global drawtestmarker
.global drawtestline
.global drawsuperzap
.global drawblank



.section .bss
.align 0
	nextFreeRamTile:		.byte 1		; NOTE. .p2align <#bits> or .balign <bytes> is prefered
	ColourMask:				.byte 1
	FrameNo:				.byte 1		; Frame Counter because "wait VSync does not work"

.align 1
	hsync_user_callback:  	.word 1 	; pointer to function
	ClearVRAMPointer:		.word 1		; Counter to point to the next byte of VRAM to clea

.section .text


; Fast constants (During normal pixel rendering there is no time to LDI/OUT so these
;                 constants are all pre-loaded into spare registers at the start of
;                 the frame) (addendum - OR they need to be restored somewhere
;                 at the end of the frame and I have spare cycles and register here
;                 so can trade some to get free clocks for "user_space")
;
; r2     = 0x00   = alternate ZERO
; r4     = 0x01   = value needed for both TCCR1B = 1xprescale and TIMSK = TOIE1 only
; r5     = 0x03   = TIMSK1 = Clear TOV1, OCF1A ints
; r6/7   = 0xFB02 = TCNT1 value to cause overflow interrupt after 256 pixels.
; r8     = 0x02   = TIMSK1 = OCIE1A only
; r9     = 0x09   = TCCR1B = 1xprescale & CTC-A mode
; r10    = 0xFF   = Trigger value to send down SPDR to get next byte
; r11/12 = 0x0159 = TCNT1 value to restore back to normal HSync opperation
; r13    = 0x80   = Value to mulitply something by to be an LSL>1 in R0
; r14    = 0x08   = MUL value in Section1 to convert from ROMTile Number to Flash Address

; r16/17 = Font Table Address plus Line offset for section1

; r18 = SPI Byte Read / 1Bpp ROM read in section1
; r19 = Trash / PORTCC colour out working register in section1
; r22 = Scanline Counter

; r20 Colour 1       NOTE: Colour 0 is "Background" and is either Black/Blue depending
; r21 Colour 2             on the SPDR data shifted into R0.  I am one clock short to
; r23 Colour 3             be able to make this "any colour" unless I trash the bootloader

; r24/25 = IJUMP location to end scanline.  This is so Timer1 can work in a split screen mode

; X   = VRAM Address
; Y   = RamTile address (YH = ((0..7)*256+offset) for which ROW, YL = RamTile# (Must be even))
; Z   = IJMP address

sub_video_mode14:					; At this point R0..R29 have all been saved to the stack
									; So all can be trashed.


	//sbi   _SFR_IO_ADDR(GPIOR0),1 testing clearvramflag




	clr		r2						; MUL trashes R1/ZERO so this is alternate ZERO

	clr 	r22						; Line counter (starts ZERO - ends at 224)
									; This unorthodox method of using a counter from 0..244
									; is so the lower 3 bits can be used as a 0..7 counter for
									; the "line within row" count.
									; At the end of the loop an "INC/CPI/BRNE" costs one more
									; cycle than "DEC/BRNE" but it saves a register and an "INC"
									; over keeping a seperate 0..7 counter.

; FAST CONSTANTS TO LOAD IN TO IO REGS

	ldi		r19, 0x01				; 0x01 in TCCR1B = Prescale:1 nothing else
	mov		r4, r19					; 0x01 in TIMSK  = TOIE1 only

	ldi		r19, 0x03				; TIMSK1 = Clear TOV1, OCF1A ints
	mov		r5, r19

	ldi		r19, 0x02				; TCNT1 value to cause OVF to happen on pixel 256
	mov		r6, r19					;
	ldi		r19, 0xFB				; 0xFFFF - (32bytes * 8Pixels * 5clocks_per_pixel) - overhead
	mov		r7, r19

	ldi		r19, 0x02				; TIMSK1 = OCIE1A only
	mov		r8, r19

	ldi		r19, 0x09				; TCCR1B = Prescale:1 CTC-A mode
	mov		r9, r19

	ldi		r19, 0x5A				; TCNT1 value to restore HSync = 0x68 value
	mov		r11, r19
	ldi		r19, 0x01
	mov		r12, r19

	ldi		r19, 0x80				; Multiply value for LSL>1 to R0
	mov		r13, r19

	ldi		r19, 0xFF				; Byte to send down SPDR to trigger next read
	mov		r10, r19

	ldi		r19, 0x08				; MUL value for RAMTile conversion
	mov		r14, r19

	ldi		r19, 0x40				; Start of user/vector part of VRAM (to be cleared)
	sts 	ClearVRAMPointer+0,r19
	ldi		r19, hi8(vram)
	sts 	ClearVRAMPointer+1,r19

	lds		r19, FrameNo					// Increment the Frame Number so we can wait till
	inc		r19								// a new frame is draw from the C code (waiting VSync)
	sts		FrameNo, r19

    sbis   _SFR_IO_ADDR(GPIOR0),0	; Test to see if Video Mode is allowed to acces SD card.
    rjmp   SubVideoModeNoSD


	ldi		r25, hi8(pm(End_of_scanline_Section_1))  ; Where we are going to jump too after the TCNT1 int
	ldi		r24, lo8(pm(End_of_scanline_Section_1))


	; SEND SD card CMD 12 (Stop Transmission)
	; 0xFF 0x4C, 0x00, 0x00, 0x00, 0x00, 0x95, 0xFF
	;
	; After the CMD12 the SD card is busy for some time.  We then have
	; to send two more 0xFF bytes before the new "read multi" command.

	ldi		r19, 0xFF
	rcall	send_byte_in_17_CLK		; takes 17 clocks including RCALL/RET
	ldi		r19, 0x4C
	rcall	send_byte_in_17_CLK
	ldi		r19, 0x00
	rcall	send_byte_in_17_CLK
	ldi		r19, 0x00
	rcall	send_byte_in_17_CLK
	ldi		r19, 0x00
	rcall	send_byte_in_17_CLK
	ldi		r19, 0x00
	rcall	send_byte_in_17_CLK
	ldi		r19, 0x95
	rcall	send_byte_in_17_CLK
	ldi		r19, 0xFF
	rcall	send_byte_in_17_CLK

	WAIT r19,582					; waste cycles to align with next
									; hsync that is first rendered line
									; This is going in the middle of the STOP_CMD and the READ_MULTI
									; so there is time for the SD card to get ready


; If the "Update Status" flag is set then
; Show the number of lives on the top left hand side of the screen
; and
; Print out the score and high score on the top right hand side of the screen
;
; otherwise if the flag is not set then waste time

	sbis   _SFR_IO_ADDR(GPIOR0),2
	rjmp	showScoreDontUpdate

; Show Score
;
; Takes two 32 bit BCD variables in RAM and prints them to the screen with
; leading zero supression
;
; This is neither optimized for speed or size.  It was made to use as few
; registers as possible to minimize reshuffling of code as it was being
; tacked into the spare time in the first render line.  There was 900
; clock cycles free so plenty of time.
;
; If things get tight in flash in the future then shuffling this to be
; BEFORE the register pre-loading above and then loading each BCD digit
; into its own register rather than re-using R23/24 will save some time
; and space.

showScore:
	lds		R20, score+0		; Compare the 32 bit values score and hiScore
	lds		r23, hiScore+0
	cp		r23, R20

	lds		R20, score+1
	lds		r23, hiScore+1
	cpc		r23, R20

	lds		R20, score+2
	lds		r23, hiScore+2
	cpc		r23, R20

	lds		R20, score+3
	lds		r23, hiScore+3
	cpc		r23, R20

	brcc	showScoreNoUpdate		; if hiScore > score then skip updating the hiScore

	;lds		R20, score+3		; (note: we dont need an LDS R20, score+3 as this has already happened)
	sts		hiScore+3, R20			; hiScore = score
	lds		R20, score+2
	sts		hiScore+2, R20
	lds		R20, score+1
	sts		hiScore+1, R20
	lds		R20, score+0
	sts		hiScore+0, R20
	rjmp	showScorePrint


showScoreNoUpdate:
	wait	R20, 15					; equalize the paths

showScorePrint:

	ldi		ZL, 0x18				; 8th last column of VRAM on row 1
	ldi		ZH, hi8(vram)

	ldi		R21, 0x00				; clear the Leading_Zero_Blanking counter

	lds		R20, score+3			; print out the 4 nibbles (8 BCD) digits of score
	rcall	showScoreTwoNibbles
	lds		R20, score+2
	rcall	showScoreTwoNibbles
	lds		R20, score+1
	rcall	showScoreTwoNibbles
	lds		R20, score+0
	rcall	showScoreTwoNibblesLast ; The last digit is a special case that does not supress the low nibble

	ldi		ZL, 0x38				; 8th last column of VRAM on row 2
									; (no need to load ZH as it already has hi8(vram)

	ldi		R21, 0x00				; re-clear the leading zero blanking counter

	lds		R20, hiScore+3			; print out the 4 nibbles (8 BCD) digits of score
	rcall	showScoreTwoNibbles
	lds		R20, hiScore+2
	rcall	showScoreTwoNibbles
	lds		R20, hiScore+1
	rcall	showScoreTwoNibbles
	lds		R20, hiScore+0
	rcall	showScoreTwoNibblesLast

showLives:
	ldi		ZL, lo8(vram)				; Point to the first char on the 1st line of VRAM
	ldi		ZH, hi8(vram)

	ldi		XL, 0x20					; Point to the first char on the 2nd line of VRAM
	ldi		XH, hi8(vram)

	lds		r23, lives					; Get the RAM variable that holds the BCD value for lives
	cpi		r23, 6						; See if lives > 5
	in		R20,  _SFR_IO_ADDR(SREG)	; get the Status Register to Skip on CARRY SET
	sbrs	R20, 0
	ldi		r23, 1						; if Carry is not set then load R23 with 1 (to show only one claw)

	ldi		R21, 5						; repeat this 5 times       for(i=0;i<5;i++)

showLivesPrintLoop:
	dec		r23							; dec the number of claws to draw
	sbrs	r23, 7						; if there has NOT been an overflow to the MSB
	rcall	showLivesPrintClaw			; then print a claw
	sbrc	r23, 7						; if there has been an overflow (we are less than zero in R23)
	rcall	showLivesPrintBlank			; then print a space (to rub out any possible claw from last time)

	dec		R21							; end of for "loop"
	brne	showLivesPrintLoop

	lds		r23, lives					; re-get the RAM variable
	cpi		r23, 6
	brsh	showLivesPrintDigits

	wait 	R20, 14						; equalize path
	rjmp	showLivesEnd

showLivesPrintDigits:
	ldi		ZL, 0x22
	ldi		R20, 'X'-55
	st		Z+, R20

	mov		R20, r23
	andi	R20, 0xF0
	breq	showLivesSingleDigit

	swap	R20
	st		Z+, R20
	andi	r23, 0x0F
	st		Z+, r23

	rjmp	showLivesEnd

showLivesSingleDigit:
	st		Z+, r23
	rjmp	.
	nop
	rjmp	showLivesEnd

showScoreTwoNibbles:				; 21 clock cycles including rcall/ret
	mov		r23, R20				; Copy the 2 BCD digits to a 2nd register
	andi	R20, 0xF0				; clear the low nibble
	swap	R20						; swap the low and high nibble
	andi	r23, 0x0F				; clear the high nibble in copy

									; supress leading zeros
	sub		R21, R20				; subrtact the first BCD digit from a counter that started as ZERO
	sbrs	R21, 7					; if the BCD digit was non-zero then the MSB of the counter will be set
	ldi		R20, 0x24				; so we load the register with 0x24 (SPACE/BLANK)
	st		Z+, R20					; print the first BCD digit to VRAM pointed to by Z

	sub		R21, r23				; subrtact the second BCD digit from a counter that started as ZERO
	sbrs	R21, 7					; if any BCD digit so far has been non ZERO the MSB will be set
	ldi		r23, 0x24				; so we load the register with 0x24 (SPACE/BLANK)
	st		Z+, r23					; print the first BCD digit to VRAM pointed to by Z
	ret

showScoreTwoNibblesLast:			; 18 clock cycles including rcall/ret
	mov		r23, R20				; Copy the 2 BCD digits to a 2nd register
	andi	R20, 0xF0				; clear the low nibble
	swap	R20						; swap the low and high nibble
	andi	r23, 0x0F				; clear the high nibble in copy

									; supress leading zeros
	sub		R21, R20				; subrtact the first BCD digit from a counter that started as ZERO
	sbrs	R21, 7					; if the BCD digit was non-zero then the MSB of the counter will be set
	ldi		R20, 0x24				; so we load the register with 0x24 (SPACE/BLANK)
	st		Z+, R20					; print the first BCD digit to VRAM pointed to by Z

	st		Z+, r23					; Just print out the very LAST digit regardless if it is ZERO or now
	ret

showLivesPrintClaw:					; Show the 4 chars that make up a claw at the VRAM location pointed to by Z (row1) and X (row2)
	ldi		R20, 0x25
	st		Z+,  R20
	ldi		R20, 0x26
	st		Z+,  R20
	ldi		R20, 0x27
	st		X+,  R20
	ldi		R20, 0x28
	st		X+,  R20
	ret

showLivesPrintBlank:				; clear the 4 chars that at the VRAM location pointed to by Z (row1) and X (row2)
	ldi		R20, 0x24
	st		Z+,  R20
	st		Z+,  R20
	st		X+,  R20
	st		X+,  R20
	rjmp	.						; equalize the clock counts with the claw version
	nop
	ret

showScoreDontUpdate:
	wait	r20, 379

showLivesEnd:
	cbi		_SFR_IO_ADDR(GPIOR0),2


	ldi		XL, lo8(vram)			; Load start of VRAM into X
	ldi		XH, hi8(vram)


	; The sector number in the File system is shifted left 9 bits (*512)
	; to equate to the sector number the CMD18 accepts
	;
	; SectorBMP is a C long (32bits)
	; Because we are shifting left by nine, that is the same as shifting left by
	; one BYTE and then one extra bit.  The +4 byte just discarded and the lowest
	; byte of the result is ZERO

	lds		r20, sectorBMP+2
	lds		r21, sectorBMP+1
	lds		r23, sectorBMP+0

	clc
	rol		r23
	rol		r21
	rol		r20

	; Send 2x dummy bytes to SD card after CMD 12 and a pause in time

	ldi		r19, 0xFF						// dummy
	rcall	send_byte_in_17_CLK
	ldi		r19, 0xFF						// dummy
	rcall	send_byte_in_17_CLK

	; SEND SD card CMD 18 (Read Multi Sector)
	; 0xFF 0x52, 0xaa, 0xbb, 0xcc, 0x00, 0x95, 0xFF
	;
	; Where aabbcc are the high 23 bits of Sector * 512
	; The lowest byte is always 0x00

	ldi		r19, 0xFF						// dummy
	rcall	send_byte_in_17_CLK
	ldi		r19, 0x52						// Read Multi
	rcall	send_byte_in_17_CLK
	mov		r19, r20						// highest byte
	rcall	send_byte_in_17_CLK
	mov		r19, r21						// 2nd byte
	rcall	send_byte_in_17_CLK
	mov		r19, r23						// 3rd byte
	rcall	send_byte_in_17_CLK
	ldi		r19, 0x00						// Lowest byte always ZERO
	rcall	send_byte_in_17_CLK
	ldi		r19, 0x95						// CRC
	rcall	send_byte_in_17_CLK
	ldi		r19, 0xFF						// dummy
	rcall	send_byte_in_17_CLK

	; Finally the colours are last because R20/R21/R23 where used in the
	; MMC Cue Sector command above

                                    ;            bbgggrrr
	ldi		r19, 0b00111000				; Colour 1 0b00111000 Green
	mov		r20, r19
	ldi		r19, 0b00000111				; Colour 2 0b00000111 Red
	mov		r21, r19
	ldi		r19, 0B00111111				; Colour 3 0b00111111 Yellow
	mov		r23, r19

	sts	_SFR_MEM_ADDR(TCCR1B),r4	; turn off CTC mode
	sts _SFR_MEM_ADDR(TIFR1),r5		; clear any pending timer1_ovf / OC1A int
	sts _SFR_MEM_ADDR(TIMSK1),r4	; enable the timer overflow interrupt




	out 	_SFR_IO_ADDR(DDRC),r23	; Make the colour for Section 1 YELLOW
									; DDRC x controls colour when PORTC is 0xFF

Next_pixel_row_section_1:
	rcall hsync_pulse 				; Destroys ZL

	lds ZL,hsync_user_callback+0	; process user hsync callback
	lds ZH,hsync_user_callback+1
	icall							; callback must take exactly 32 cycles

	WAIT r19,213 - AUDIO_OUT_HSYNC_CYCLES
	WAIT r19,82



	mov		r16, r22				; Copy scanline (0..224) into r16
	andi	r16, 0b00000111			; and then AND with 0b00000111 to get row (0..7)
	ldi		r17, lo8(FontTable)		; get base address of FontTable
	add		r16, r17				; Add the lo8() base address of the FontTable to the PixelRow
	ldi		r17, hi8(FontTable)
	adc		r17, r2					; Add the hi8() of the base address of the Font Table

	ld		r0, X+					; Get the first Byte/Char from the VRAM

	mul		r0, r14					; Multiply this by 8 (for 8 bytes per tile)

	add		r0, r16					; add this to the (base address + pixel row from above)
	adc		r1, r17

	movw	ZL, r0					; Move into the Z register ready for the LPM instruction

	lpm		r18, Z					; Get the first 8 bits/pixels to show to the screen

	rol		r18						; move the MSB into the carry flag

	sbc		r19, r19				; subract r19 from it self to expand the carry flat to 0x00/0xFF

	sts _SFR_MEM_ADDR(TCNT1H), r7	; TCNT value to make TOVF1 trigger on the 3rd last
	sts _SFR_MEM_ADDR(TCNT1L), r6	; instruction of the render loop

	sei								; global enable interrupts.

	rjmp	.						; We have to waste two cycles here so section one
									; aligns with section 2 that needed an IJMP to
									; get to pixel 1


; Section 1 of the screen is 16 pixel rows long.
; It is a ROM-Tile section with 1BPP tiles
;
; It exists for 3 purposes
;
; 1, Display the levels, lives, score on the screen with minimum User-time CPU usage
; 2, As above, but with not using any RAM-Tiles (we only have 128 so need them all for action)
; 3, Most importantly does the SD card Get_Token to get it ready to display the BMP
;
; #3 saves 5000 or so clock cycles from the 70K total in User-time


Section1_Pixel_Loop_Get_Token:			; Clocks this pixel / clock this run of pixels

	out 	_SFR_IO_ADDR(PORTC),r19		; 1  1 Output Pixel 0
	ld 		r0, X+						; 2  2 Get next ROMTile/Char from VRAM
										; 3  3
	rol 	r18							; 4  4 Set up the carry flag to calc Pixel_1
	sbc 	r19,r19						; 5  5 Use SBC trick to calculate Pixel_1
	out 	_SFR_IO_ADDR(PORTC),r19		; 1  6 Output Pixel 1
	mul		r0, r14						; 2  7 Multiple the Tile/Char by 8 to get Char Offset
										; 3  8
	rol 	r18							; 4  9 Set up the carry flag to calc Pixel_2
	sbc 	r19,r19						; 5 10 Use SBC trick to calculate Pixel_2
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 11 Output Pixel 2
	add		r0, r16						; 2 12 Add the (Base Address + Line Offset) to Char Offset
	adc		r1, r17						; 3 13
	rol 	r18							; 4 14 Set up the carry flag to calc Pixel_3
	sbc 	r19,r19						; 5 15 Use SBC trick to calculate Pixel_3
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 16 Output Pixel 3
	in      YL,_SFR_IO_ADDR(SPDR)		; 2 17 Get a byte from the SPI Data register (YL is unused for pointers here)
	rol 	r18							; 3 18 Set up the carry flag to calc Pixel_4
	sbc 	r19,r19						; 4 19 Use SBC trick to calculate Pixel_4
	rol 	r18							; 5 20 Set up the carry flag to calc Pixel_5 (We can do this before pixel 4 because carry preserved)
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 21 Output Pixel 4
	sbc 	r19,r19						; 2 22 Use SBC trick to calculate Pixel_5
	cpi		YL, 0xFE					; 3 23 See if we have got the SD Card ready data token yet (0xFE)
	breq	Section1_No_SPI_Out			; 4 24 If we have got the data token then DON'T output the 0xFF SPI byte
	out    _SFR_IO_ADDR(SPDR), r10		; 5 25 Send 0xFF out the SPI port to get next SPI data back
Section1_No_SPI_Out:
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 26 Output Pixel 5
	movw	ZL, r0						; 2 27 Get ready for the LPM instruction
	rol 	r18							; 3 28 Set up the carry flag to calc Pixel_6
	sbc 	r19,r19						; 4 29 Use SBC trick to calculate Pixel_6
	rol 	r18							; 5 30 Set up the carry flag to calc Pixel_7 (We can do this before pixel 4 because carry preserved)
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 31 Output Pixel 6
	sbc 	r19,r19						; 2 32 Use SBC trick to calculate Pixel_7
	lpm		r18, Z						; 3 33 Get the 8 bits/pixels from FLASH
										; 4 34
										; 5 35
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 36 Output Pixel 7
	rol 	r18							; 2 37 Set up the carry flag to calc Pixel_6
	sbc 	r19,r19						; 3 38 Use SBC trick to calculate Pixel_6
	rjmp 	Section1_Pixel_Loop_Get_Token ; 4 39 Got back to the start
										; 5 40


End_of_scanline_Section_1:
	pop		r19							; Take the RETI return address of the
	pop		r19							; stack (we dont want to return to there)
										; (r19 is trash because of "wait")

 	inc 	r22							; Check to see if all lines in this section rendered
 	cpi		r22, 16						; (16 lines in the first section)
	breq 	End_of_Section_1

	mov		r18, r22
	andi 	r18, 0x07					; if 8 rows have been rendered
	breq 	Pixel_row_mod_8_section_1	; then we are on a new char row

	sbiw	XL, 33						; If we are not on a new char - row then
										; we have to step back (32+1) bytes in the VRAM counter

	WAIT 	r19,141						; Extra Cycles becuase 256 pixels instead of 280
	rjmp 	Next_pixel_row_section_1

Pixel_row_mod_8_section_1:
	WAIT 	r19,140

	sbiw	XL, 1						; Subrtact 1 from the VRAM counter because the
										; the last phase_b of "renderlines" read one byte
										; of the next line.
	rjmp 	Next_pixel_row_section_1


; This is the front porch of line 17
; we just need to get ready for the different video mode
; by changing the effective interrupt vector (R24/25)
; and read the first byte of BMP from the SD card

End_of_Section_1:
	out 	_SFR_IO_ADDR(DDRC),r10	; Got back to normal port_C colour mode

	ldi		r25, hi8(pm(End_of_scanline_Section_2))  ; Where we are going to jump too after the TCNT1 int
	ldi		r24, lo8(pm(End_of_scanline_Section_2))

	out    	_SFR_IO_ADDR(SPDR), r10		; send out an FF to cue first byte

	sbiw	XL, 1						; Subrtact 1 from the VRAM counter because the
										; the last phase_b of "renderlines" read one byte
										; of the next line.
	WAIT 	r19,141						; Extra Cycles becuase 256 pixels instead of 280


; From here down is the RAMTile vector mode that the webs/game plays on
; In terms of timer interrupt etc this is identical to the above section
; Section1 was copied from this section that was written first)
;
; This setion starts at (visible) scanline 16 and ends at 224
;
; Every 8 pixels rows, it advances one text row.
; Every 16 pixel rows (2 text rows) it has to read the inter-block-bytes from the SD card.

Next_pixel_row_section_2:
	rcall hsync_pulse 				; Destroys ZL

	lds ZL,hsync_user_callback+0	; process user hsync callback
	lds ZH,hsync_user_callback+1
	icall							; callback must take exactly 32 cycles

	WAIT r19,213 - AUDIO_OUT_HSYNC_CYCLES


.macro video_render_clear_get_counter
	lds 	ZL, ClearVRAMPointer+0		; get the clear_counter from RAM variable
	lds 	ZH, ClearVRAMPointer+1
.endm

.macro video_render_clear_save_counter
	sts		ClearVRAMPointer+0, ZL
	sts		ClearVRAMPointer+1, ZH
.endm


	WAIT r19,4     //88
	sbis   _SFR_IO_ADDR(GPIOR0),1
	rjmp	DontClear1

; X contains the VRAM pointer
; Y and Z are about to be loaded after this so can trash them now

// 88 clocks for the wait without the code BETWEEN these two comments
	nop
	video_render_clear_get_counter				; 4 clocks
	ldi		YL, 0x00							; 1 clock
	ldi		r18, 0x02							; 1 clock
	rcall video_render_clear_search_tile 		; 72 clocks  (60 + 12 * x)
	video_render_clear_save_counter				; 4 clocks

DontClear1Return:
// 88 clocks for the wait without the code BETWEEN these two comments


	mov		YH, r22					; Copy scanline (0..224) into YH
	andi	YH, 0b00000111			; and then AND with 0b00000111 to get row (0..7)
	subi	YH, hi8(-(ramTiles))	; then add base address of ramTiles to YH

	ldi		r18, 0x00				; First Column is BLACK pixel

	; Do the same thing we do in "PhaseB" of renderlines prior
	; to the first IJUMP

	ld		YL, X+					; Load ramtile number from VRAM into YL

	ld		ZH, Y+					; Get the 8bit pixel data into ZH

	mov		ZL, ZH					; Copy ZH to ZL for the MUL*32+1 trick
	andi	ZH, 0x0F
	ori		ZH, 0x20

	sts _SFR_MEM_ADDR(TCNT1H), r7	; TCNT value to make TOVF1 trigger on the 3rd last
	sts _SFR_MEM_ADDR(TCNT1L), r6	; instruction of the render loop

	sei								; global enable interrupts.
	ijmp							; Go to the pixel out code
									; (at the end of the scanline we will IJMP to the
									;  address pointed too by R24/25)

DontClear1:
	WAIT 	r19,80
	rjmp	DontClear1Return

DontClear2:
	WAIT 	r19,127
	rjmp	DontClear2Return

DontClear3:
	WAIT 	r19,127
	rjmp	DontClear3Return

video_render_clear_search_tile:
	cpi		YL, 0x00
	brne	NoReadRamTileToClearLong

	cp		ZL, XL							; compare the clear_counter with the VRAM_counter
	cpc		ZH, XH
	breq	NoReadRamTileToClearShort		; if we have caught up to the VRAM counter then DONT clear anything.
	ld		YL, Z							; otherwise get the RAMTile to clear
	st		Z+, r2							; and clear VRAM location and increment to the next possible
	dec		r18
	brne	video_render_clear_search_tile
	rjmp	video_render_clear_ramTile_in_Y
NoReadRamTileToClearLong:
	rjmp	.
	nop
NoReadRamTileToClearShort:
	nop
	rjmp	.
	dec		r18
	brne	video_render_clear_search_tile
	rjmp	.

video_render_clear_ramTile_in_Y:

	ldi		YH, hi8(ramTiles + 0x0000)		;
	st		Y+, r10							;
	st		Y,  r10							;
	ldi		YH, hi8(ramTiles + 0x0100)		;
	st		Y,  r10							;
	st		-Y, r10							;
	ldi		YH, hi8(ramTiles + 0x0200)		;
	st		Y+, r10							;
	st		Y,  r10							;
	ldi		YH, hi8(ramTiles + 0x0300)		;
	st		Y,  r10							;
	st		-Y, r10							;
	ldi		YH, hi8(ramTiles + 0x0400)		;
	st		Y+, r10							;
	st		Y,  r10							;
	ldi		YH, hi8(ramTiles + 0x0500)		;
	st		Y,  r10							;
	st		-Y, r10							;
	ldi		YH, hi8(ramTiles + 0x0600)		;
	st		Y+, r10							;
	st		Y,  r10							;
	ldi		YH, hi8(ramTiles + 0x0700)		;
	st		Y,  r10							;
	st		-Y, r10							;

	ret


End_of_scanline_Section_2:
	pop		r19							; Take the RETI return address of the
	pop		r19							; stack (we dont want to return to there)
										; (r19 is trash because of "wait")

 	inc 	r22							; Check to see if all lines rendered
 	cpi		r22, 224					; (224 lines in the frame)
	breq 	End_of_video_field

	mov		r18, r22
	andi 	r18, 0x07					; if 8 rows have been rendered
	breq 	Next_8_pixel_rows 			; then we are on a new char row

	sbiw	XL, 33						; If we are not on a new char - row then
										; we have to step back (32+1) bytes in the VRAM
										; counter

	WAIT 	r19,9	//141					; Extra Cycles becuase 256 pixels instead of 280
	sbis   _SFR_IO_ADDR(GPIOR0),1
	rjmp	DontClear2

	WAIT 	r19,12

	video_render_clear_get_counter				; 4 clocks
	ldi		YL, 0x00							; 1 clock
	ldi		r18, 0x05							; 1 clock
	rcall video_render_clear_search_tile 		; 120 clocks  (60 + 12 * x)
	video_render_clear_save_counter				; 4 clocks


DontClear2Return:
	rjmp 	Next_pixel_row_section_2

Next_8_pixel_rows:
	sbrc	r22, 3						; Every 2nd set of 8 we have read 512 bytes of the
	rjmp	Pixel_row_mod_8				; SD card and need to read CRC+stuff
Pixel_row_mod_16:

	rcall	sd_card_inter_block_bytes	; Read the CRC/Stuff bytes between the 512 byte blocks

	sbiw	XL, 1						; Subrtact 1 from the VRAM counter because the
										; the last phase_b of "renderlines" read one byte
										; of the next line.
	WAIT 	r19,55
	rjmp 	Next_pixel_row_section_2

Pixel_row_mod_8:

	sbiw	XL, 1						; Subrtact 1 from the VRAM counter because the
										; the last phase_b of "renderlines" read one byte
										; of the next line.
	WAIT 	r19,5	//137
	sbis   _SFR_IO_ADDR(GPIOR0),1
	rjmp	DontClear3

	WAIT 	r19,12

	video_render_clear_get_counter				; 4 clocks
	ldi		YL, 0x00							; 1 clock
	ldi		r18, 0x05							; 1 clock
	rcall video_render_clear_search_tile 		; 120 clocks  (60 + 12 * x)
	video_render_clear_save_counter				; 4 clocks

DontClear3Return:
	rjmp 	Next_pixel_row_section_2

End_of_video_field:						; This is the LAST HSync pulse.  We must send
										; 4/5 SPI Bytes here so we are ready for a STOP_TRANS

	rcall	sd_card_inter_block_bytes	; We have reached the end of the BMP but we
										; still need to send the inter_block_bytes
										; as some SD cards will not listen to the new
										; CMD12 when they are outputting the CRC/etc

	WAIT 	r19,64						; Extra Cycles becuase 256 pixels instead of 280
	rcall hsync_pulse

End_of_Video_Mode_cleanup:
	;set vsync flag & flip field
	lds ZL,sync_flags
	ldi r20,SYNC_FLAG_FIELD
	ori ZL,SYNC_FLAG_VSYNC
	eor ZL,r20
	sts sync_flags,ZL

	sts	_SFR_MEM_ADDR(TCCR1B),r9		; Set Timer1 back to CTC_A mode

	sts _SFR_MEM_ADDR(TCNT1H), r12		; Restore TCNT to what it would have been
	sts _SFR_MEM_ADDR(TCNT1L), r11		;  if we didnt change it for interrupt
										;  ended scanlines

	sts _SFR_MEM_ADDR(TIFR1),r5			; clear any pending timer1_ovf / OC1A int

	sts _SFR_MEM_ADDR(TIMSK1),r8		; Restore TimerMask to what it was

	cbi	    _SFR_IO_ADDR(GPIOR0), 1		; unset the "ClearVram" flag

	ret									; Return kernel then user code.



send_byte_in_17_CLK:
	in		r18, _SFR_IO_ADDR(SPDR)		; get the previous byte from the data register
	out		_SFR_IO_ADDR(SPDR), r19		; send a new byte out the SPI port

	ldi		r19,2						; wait some clock cycles
	dec		r19
	brne	.-4
	rjmp	.

	ret									; return with the new value in R18

; Read the extra bytes between blocks
;
; Between each block (512 bytes) in a multi-block read the SD card outputs
; at least 4 bytes
;
; CRC Byte 1
; CRC Byte 2
; ONE or more Busy Token (0xFF)
; ONE Ready Token (0xFE)
;
; For high quality cards there will be either 1 or 2 Busy Tokens
;     (Cheap SD cards have >2 busy tokens and can't be used for streaming)
;
; Too account for the variable 4 or 5 byte output we
;     - Read 3 bytes (always CRC, CRC 0xFF)
;     - Read a 4th byte which MAY be either 0xFF or 0xFE
;        - if the lowest bit in the 4th byte read is 0 skip the next READ
;     - Read another byte which is the CUE byte for the next 512 block

sd_card_inter_block_bytes:
	ldi		r19, 3						; 3 SPI bytes to be sent/received

										; Byte        1    2    3
	in    	r18, _SFR_IO_ADDR(SPDR)  	; CLKS        1   19   37 (18 clocks between)
	out    	_SFR_IO_ADDR(SPDR), r10		; Data     0x00 0x00 0xFF
	ldi		r18, 4						; Meaning  CRC1 CRC2 Stuff
	dec		r18
	brne	.-4							; Delay some CLKs to make 18 between them
	nop
	dec		r19							; end of the 3 byte FOR loop
	brne	.-16
	nop
										; Byte        4
										; CLK        55
	in    	r18, _SFR_IO_ADDR(SPDR)		; Data     0xFF  or  0xFE
	sbrc	r18, 0						; Meaning  Stuff or  Data Token
	out    	_SFR_IO_ADDR(SPDR), r10		; If we recieved FE then don't send this byte

	ldi		r18, 5
	dec		r18							; Delay some CLKs to make 18 between them
	brne	.-4
	nop

	in    	r18, _SFR_IO_ADDR(SPDR)  	; 74 (19 clocks because SBRC steals one)
	out    	_SFR_IO_ADDR(SPDR), r10		; This is an OUT so the first IN during
										; "renderline" is the data

	ret










































SubVideoModeNoSD:
	ldi		r25, hi8(pm(End_of_scanline_Section_NoSD))  ; Where we are going to jump too after the TCNT1 int
	ldi		r24, lo8(pm(End_of_scanline_Section_NoSD))

	WAIT r19,1299						; waste cycles to align with next
										; hsync that is first rendered line

                                    	;            bbgggrrr
	ldi		r19, 0b00111000				; Colour 1 0b00111000 Green
	mov		r20, r19
	ldi		r19, 0b00000111				; Colour 2 0b00000111 Red
	mov		r21, r19
	ldi		r19, 0B00111111				; Colour 3 0b00111111 Yellow
	mov		r23, r19

	ldi		XL, lo8(vram)				; Load start of VRAM into X
	ldi		XH, hi8(vram)

	sts	_SFR_MEM_ADDR(TCCR1B),r4	; turn off CTC mode
	sts _SFR_MEM_ADDR(TIFR1),r5		; clear any pending timer1_ovf / OC1A int
	sts _SFR_MEM_ADDR(TIMSK1),r4	; enable the timer overflow interrupt



Next_pixel_row_section_NoSD:
	rcall hsync_pulse 				; Destroys ZL

	lds ZL,hsync_user_callback+0	; process user hsync callback
	lds ZH,hsync_user_callback+1
	icall							; callback must take exactly 32 cycles

	out 	_SFR_IO_ADDR(DDRC),r20	; Make the colour for Section 1 YELLOW
									; DDRC x controls colour when PORTC is 0xFF

	WAIT r19,213 - AUDIO_OUT_HSYNC_CYCLES
	WAIT r19,81

	mov		r16, r22				; Copy scanline (0..224) into r16
	andi	r16, 0b00000111			; and then AND with 0b00000111 to get row (0..7)
	ldi		r17, lo8(FontTable)		; get base address of FontTable
	add		r16, r17				; Add the lo8() base address of the FontTable to the PixelRow
	ldi		r17, hi8(FontTable)
	adc		r17, r2					; Add the hi8() of the base address of the Font Table

	ld		r0, X+					; Get the first Byte/Char from the VRAM

	mul		r0, r14					; Multiply this by 8 (for 8 bytes per tile)

	add		r0, r16					; add this to the (base address + pixel row from above)
	adc		r1, r17

	movw	ZL, r0					; Move into the Z register ready for the LPM instruction

	lpm		r18, Z					; Get the first 8 bits/pixels to show to the screen

	rol		r18						; move the MSB into the carry flag

	sbc		r19, r19				; subract r19 from it self to expand the carry flag to 0x00/0xFF

	sts _SFR_MEM_ADDR(TCNT1H), r7	; TCNT value to make TOVF1 trigger on the 3rd last
	sts _SFR_MEM_ADDR(TCNT1L), r6	; instruction of the render loop

	sei								; global enable interrupts.

	rjmp	.						; We have to waste two cycles here so section NoSD
									; aligns with section 2 that needed an IJMP to
									; get to pixel 1


Section_NoSD_Pixel_Loop:				; Clocks this pixel / clock this run of pixels

	out 	_SFR_IO_ADDR(PORTC),r19		; 1  1 Output Pixel 0
	ld 		r0, X+						; 2  2 Get next ROMTile/Char from VRAM
										; 3  3
	rol 	r18							; 4  4 Set up the carry flag to calc Pixel_1
	sbc 	r19,r19						; 5  5 Use SBC trick to calculate Pixel_1
	out 	_SFR_IO_ADDR(PORTC),r19		; 1  6 Output Pixel 1
	mul		r0, r14						; 2  7 Multiple the Tile/Char by 8 to get Char Offset
										; 3  8
	rol 	r18							; 4  9 Set up the carry flag to calc Pixel_2
	sbc 	r19,r19						; 5 10 Use SBC trick to calculate Pixel_2
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 11 Output Pixel 2
	add		r0, r16						; 2 12 Add the (Base Address + Line Offset) to Char Offset
	adc		r1, r17						; 3 13
	rol 	r18							; 4 14 Set up the carry flag to calc Pixel_3
	sbc 	r19,r19						; 5 15 Use SBC trick to calculate Pixel_3
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 16 Output Pixel 3
	nop									; 2 17
	rol 	r18							; 3 18 Set up the carry flag to calc Pixel_4
	sbc 	r19,r19						; 4 19 Use SBC trick to calculate Pixel_4
	rol 	r18							; 5 20 Set up the carry flag to calc Pixel_5 (We can do this before pixel 4 because carry preserved)
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 21 Output Pixel 4
	sbc 	r19,r19						; 2 22 Use SBC trick to calculate Pixel_5
	nop									; 3 23
	nop									; 4 24
	nop									; 5 25
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 26 Output Pixel 5
	movw	ZL, r0						; 2 27 Get ready for the LPM instruction
	rol 	r18							; 3 28 Set up the carry flag to calc Pixel_6
	sbc 	r19,r19						; 4 29 Use SBC trick to calculate Pixel_6
	rol 	r18							; 5 30 Set up the carry flag to calc Pixel_7
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 31 Output Pixel 6
	sbc 	r19,r19						; 2 32 Use SBC trick to calculate Pixel_7
	lpm		r18, Z						; 3 33 Get the 8 bits/pixels from FLASH
										; 4 34
										; 5 35
	out 	_SFR_IO_ADDR(PORTC),r19		; 1 36 Output Pixel 7
	rol 	r18							; 2 37 Set up the carry flag to calc Pixel_6
	sbc 	r19,r19						; 3 38 Use SBC trick to calculate Pixel_6
	rjmp 	Section_NoSD_Pixel_Loop		; 4 39 Got back to the start
										; 5 40

End_of_scanline_Section_NoSD:
	pop		r19							; Take the RETI return address of the
	pop		r19							; stack (we dont want to return to there)
										; (r19 is trash because of "wait")

 	inc 	r22							; Check to see if all lines in this section rendered
 	cpi		r22, 224					; (224 lines in the frame)
	breq 	End_of_video_field_NoSD

	mov		r18, r22
	andi 	r18, 0x07					; if 8 rows have been rendered
	breq 	Pixel_row_mod_8_section_NoSD; then we are on a new char row

	sbiw	XL, 33						; If we are not on a new char - row then
										; we have to step back (32+1) bytes in the VRAM counter

	WAIT 	r19,141						; Extra Cycles becuase 256 pixels instead of 280
	rjmp 	Next_pixel_row_section_NoSD

Pixel_row_mod_8_section_NoSD:
	WAIT 	r19,140

	sbiw	XL, 1						; Subrtact 1 from the VRAM counter because the
										; the last phase_b of "renderlines" read one byte
										; of the next line.
	rjmp 	Next_pixel_row_section_NoSD



End_of_video_field_NoSD:
	WAIT 	r19,147
	rcall hsync_pulse

End_of_Video_Mode_cleanup_NoSD:
	;set vsync flag & flip field
	lds ZL,sync_flags
	ldi r20,SYNC_FLAG_FIELD
	ori ZL,SYNC_FLAG_VSYNC
	eor ZL,r20
	sts sync_flags,ZL

	sts	_SFR_MEM_ADDR(TCCR1B),r9		; Set Timer1 back to CTC_A mode

	sts _SFR_MEM_ADDR(TCNT1H), r12		; Restore TCNT to what it would have been
	sts _SFR_MEM_ADDR(TCNT1L), r11		;  if we didnt change it for interrupt
										;  ended scanlines

	sts _SFR_MEM_ADDR(TIFR1),r5			; clear any pending timer1_ovf / OC1A int

	sts _SFR_MEM_ADDR(TIMSK1),r8		; Restore TimerMask to what it was

	cbi	    _SFR_IO_ADDR(GPIOR0), 1		; unset the "ClearVram" flag

	ret									; Return kernel then user code.























; void waitNFrames(uint8_t n)
;
; Waits for n number of video frames to be rendered
;
; Inputs
; 		r24 = number of frames to wait
;
; Returns
;		void
;
; Trashed
;		r24
;		r25

waitNFrames:
	lds		r25, FrameNo					; Get the current video frame (for the first time)
	add		r24, r25						; Add the number of frames we wish to wait to it
waitNFramesLoop:
	lds		r25, FrameNo					; get the current video frame over and over again
	cpse	r24, r25						;    until it is equal to the first time we got it + n
	rjmp	waitNFramesLoop
	ret

; void ClearBufferFinal(void)
;
; Clears the last few lines of the video buffer AFTER the video mode pixel renderer
; has already cleared most of them.
;
; This is required because the video mode can not clear the last [few] lines of
; VRAM / ramTiles as it still needs them to render them to screen.
;
; Inputs
;		void
;
; Returns
;		void
;
; Modifies RAM
;		clears ramTiles to 0xFF and VRAM to 0x00
;		nextFreeRamTile = 0
;
; Trashed
;       R23
;		R24:25
;		R26:27
;		R30:31

ClearBufferFinal:
	sts 	nextFreeRamTile,r1				; set the first free ramTile as 0

	lds 	ZL, ClearVRAMPointer+0			; get the clear_counter from RAM variable
	lds 	ZH, ClearVRAMPointer+1

	ldi		r24, lo8(vram+0x0380)			; The end address of VRAM (VRAM base + 896)
	ldi		r25, hi8(vram+0x0380)

	ldi		r23, 0xFF						; The value of <BLANK> data for ramTiles

ClearBufferFinalLoop:
	ld		XL, Z							; Get the ramTile number from VRAM
	cpi		XL, 0x00						; if this cell is already empty then
	breq	ClearBufferFinalAlreadyClear	; skip the clearing

	st		Z+, r1							; store <zero> at the VRAM location

											; XL already = ramtile # (odd)
	ldi		XH, hi8(ramTiles + 0x0000)		; Make XH = address of scanline 0, 1, 2 ... 7
	st		X+, r23							; and then write <blank> to ramTile # (odd/even)
	st		X,  r23							;
	ldi		XH, hi8(ramTiles + 0x0100)		;
	st		X,  r23							;
	st		-X, r23							;
	ldi		XH, hi8(ramTiles + 0x0200)		;
	st		X+, r23							;
	st		X,  r23							;
	ldi		XH, hi8(ramTiles + 0x0300)		;
	st		X,  r23							;
	st		-X, r23							;
	ldi		XH, hi8(ramTiles + 0x0400)		;
	st		X+, r23							;
	st		X,  r23							;
	ldi		XH, hi8(ramTiles + 0x0500)		;
	st		X,  r23							;
	st		-X, r23							;
	ldi		XH, hi8(ramTiles + 0x0600)		;
	st		X+, r23							;
	st		X,  r23							;
	ldi		XH, hi8(ramTiles + 0x0700)		;
	st		X,  r23							;
	st		-X, r23							;

	cp		ZL, r24							; Compare Z to the end address of VRAM
	cpc		ZH, r25

	brne	ClearBufferFinalLoop			; if we are not at the end of VRAM then keep clearing
	ret

ClearBufferFinalAlreadyClear:
	ld		XH, Z+							; INC Z in 2 clock cycles and one WORD of flash (we don't care if we trash XH)

	cp		ZL, r24							; Compare Z to the end address of VRAM
	cpc		ZH, r25

	brne	ClearBufferFinalLoop			; if we are not at the end of VRAM then keep clearing
	ret



; void ClearBuffer(void)
; void ClearBufferTextMode(void)
;
; Clears the video buffer and the ramTiles (textmode version does not clear ramTiles)
; This is a dumb routine that clears all 1920/896 ram locations regardless
; It is only called once at each mode switch so does not need to be smart.
; Normaly clearing of VRAM / ramTiles is done during render to save time
;
; Inputs
;		void
;
; Returns
;		void
;
; Modifies RAM
;		clears entire contents of ramTiles to 0xFF and VRAM to 0x00
;            (Actually the first 2 rows of VRAM are cleared to 0x0A which is BLANK/<Space>
;             in the font set for the text mode section1)
;		nextFreeRamTile = 0
;
; Trashed
;       R23
;		R24
;		R26:27

ClearBuffer:
	sts 	nextFreeRamTile,r1		; set the first free ramTile as 0

	ldi 	XL,lo8(ramTiles)		; Get base address of ramTiles
	ldi 	XH,hi8(ramTiles)

	ldi		r24, 128				; there are 128 * 16 = 2048 bytes of ramTile to clear
	ldi		r23, 0xFF               ; RAMTile cleared is 0xFF rather than the usual 0x00
	rcall	ClearBufferLoop16Byte

	ldi		r24, 4					; After that there are 4 * 16 = 64 bytes of Status Line VRAM to clear
	ldi		r23, 0x24				; 0x24 = SPACE in the tempest reduced font set
	rcall	ClearBufferLoop16Byte

	ldi		r24, 52					; After that there are 52 * 16 = 832 bytes of normal VRAM to clear
	ldi		r23, 0x00				; that need to be cleared to 0x00
	rjmp	ClearBufferLoop16Byte
									; This last one is RJMP rather than RCALL as it saves a RET

ClearBufferTextMode:
	sts 	nextFreeRamTile,r1		; set the first free ramTile as 0

	ldi 	XL,lo8(vram)			; Get base address of VRAM (text mode does not need to clear ramTiles)
	ldi 	XH,hi8(vram)

	ldi		r24, 56					; There are 56 * 16 = 896 bytes of VRAM to clear in text mode
	ldi		r23, 0x24				; 0x24 = SPACE in the tempest reduced font set

									; We fall into ClearBufferLoop16Byte rather than rcall it
									; (saves an RCALL and RET)

ClearBufferLoop16Byte:				; Clears (R24 * 16) bytes pointed to by X
	st		X+, r23					; clear 16 bytes
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23
	st		X+, r23

	dec		r24						; see if x16 many times bytes have been cleared
	brne	ClearBufferLoop16Byte
	ret


SDCardVideoModeEnable:
	sbi	    _SFR_IO_ADDR(GPIOR0), 0             ; Because of the render line code having to share
	                                            ; space with the reset vector, we have to set a GPIO bit
	                                            ; to indicate we are rendering rather than reseting.
	                                            ; GPIO bits are in a "known state" after reset.
	ret

SDCardVideoModeDisable:							; restore video mode to text only mode (no SD card)
	cbi	    _SFR_IO_ADDR(GPIOR0), 0
	rcall	ClearBufferTextMode
	ret

;Nothing to do in this mode
DisplayLogo:
VideoModeVsync:
	ret

InitializeVideoMode:

	ldi 	r24,lo8(pm(DefaultCallback))		; Point HSyncCallBack to something so it does not jump
	sts 	hsync_user_callback+0,r24			; to somewhere undefined before a user callback is set
	ldi 	r24,hi8(pm(DefaultCallback))
	sts 	hsync_user_callback+1,r24

	rcall	SDCardVideoModeDisable				; Set the video mode to text_mode (don't use SD card yet)
	ret

;****************************
; Sets a callback that will be invoked during HBLANK
; before rendering each line.
; C callable
; r25:r24 - pointer to C function: void ptr*(void)
;****************************
.section .text.SetHsyncCallback
SetHsyncCallback:
	sts 	hsync_user_callback+0,r24
	sts 	hsync_user_callback+1,r25
	ret

;must take exactly 32 cycles including the ret
DefaultCallback:
	WAIT r19,28
	ret

flashingCallback:
	lds		r19, actionTimerSubCount
	sbrs	r19, 0
	rjmp	flashingCallbackSkip1

	ldi		r20, 0b00111000				; Colour 1 0b00111000 Green
	ldi		r21, 0b00000111				; Colour 2 0b00000111 Red
	ldi		r23, 0B00111111				; Colour 3 0b00111111 Yellow

	rjmp	flashingCallbackSkip2
flashingCallbackSkip1:
	ldi		r20, 0b11111111				; Colour 1 0b00111000 Green
	ldi		r21, 0b11111111				; Colour 2 0b00000111 Red
	ldi		r23, 0B11111111				; Colour 3 0b00111111 Yellow
	nop

flashingCallbackSkip2:
	WAIT r19,19
	ret


BlankingCallback:
	ldi	r20, 0b00000000
	ldi	r21, 0b00000000
	ldi	r23, 0b00000000
	WAIT r19,25
	ret

titleCallback:							; Call back to change colours on a per scanline basis for the title screen

	cpi		r22, 73						; Section 1 starts at row 73 and is the gradient behind the word TORNADO
	brlo	titleCallbackSection1

	cpi		r22, 100					; Section 2 is part way down the word TORNADO and is the bottom light blue colour
	brlo	titleCallbackSection2

	cpi		r22, 140					; Section 3 is the pink/red colour of the words PRESS START
	brlo	titleCallbackSection3

titleCallbackSection4:					; Section 4 is the 3 yellow colours for the numbers "2000"

                                    ;            bbgggrrr
	ldi		r20, 0b11111111 		; Colour 1 0b00111111 Very Light Blue
	ldi		r21, 0b11111101 		; Colour 2 0b00000111 Very Light Blue
	ldi		r23, 0B11100011			; Colour 3 0b00111111 Darky purply blue
	WAIT r19,19
	ret
titleCallbackSection1:
                                    ;            bbgggrrr
	ldi		r20, 0b11101101 		; Colour 1 0b11101101 Very Light Blue
	mov		r19, r22		 		; Colour 2 0b00000111 Dark Yellow

	subi	r19, 41
	ldi		r21, 0b00000000

	bst		r19, 0
	bld		r21, 	0
	bst		r19, 1
	bld		r21, 	3
	bst		r19, 2
	bld		r21, 	4
	bst		r19, 3
	bld		r21, 	5
	bst		r19, 4
	bld		r21, 	7
	bst		r19, 5
	bld		r21, 	6

	ldi		r23, 0B00010100			; Colour 3 0b00111111 Dark Redish Yellow
	WAIT 	r19,8
	ret
titleCallbackSection2:
	ldi		r20, 0b11101101 		; Colour 1 0b11101101 Very Light Blue
	ldi		r21, 0b11111100 		; Colour 2 0b00000111 Light Blue
	ldi		r23, 0B00010100			; Colour 3 0b00111111 Dark Redish Yellow

	WAIT r19,20
	ret

titleCallbackSection3:
	ldi		r20, 0b00111111 		; Colour 1 0b00111111 Bright Yellow
	ldi		r21, 0b01010111 		; Colour 2 0b00000111 Dark Yellow
	ldi		r23, 0B00010100			; Colour 3 0b00111111 Dark Redish Yellow

	WAIT r19,18
	ret

creditCallback:						; Call back to change colours on a per scanline basis for credit/hi-score screen
	sbrs	r22, 3
	rjmp	creditCallbackOdds

creditCallbackEvens:
                                    ;            bbgggrrr
	ldi		r20, 0b00111111 		; Colour 1 0b00111111 Bright Yellow
	ldi		r21, 0b01010111 		; Colour 2 0b00000111 Dark Yellow
	ldi		r23, 0B00001100			; Colour 3 0b00111111 Dark red

	WAIT r19,23
	ret

creditCallbackOdds:
                                    ;            bbgggrrr
	ldi		r20, 0b00111111 		; Colour 1 0b00111111 Bright Yellow
	ldi		r21, 0b01010111 		; Colour 2 0b00000111 Dark Yellow
	ldi		r23, 0B00011010			; Colour 3 0b00111111 Dark green

	WAIT r19,22
	ret


















decLives:
	lds		r23, lives				; Get the 2 digit BCD variable from RAM
	cpi		r23, 0x00				; if lives already = Zero
	breq	decLivesEnd				; then don't decriment
	mov		r22, r23				; Make a copy of the BCD variable
	andi	r22, 0x0F				; clear out the high nibble
	brne	decLivesNoRollover		; if the lower nibble was ZERO already, then
	subi	r23, 0x07				; take 7 away from the whole BCD value (decriment the 10s and leave 9 in the units)
	sts		lives, r23				; save the variable to RAM
	ret
decLivesNoRollover:
	dec		r23						; if the lowest BCD digit was not ZERO you can just decriment and
	sts		lives, r23				; save the value to RAM
decLivesEnd:
	ret

incLives:
	lds		r23, lives				; Get the 2 digit BCD variable from RAM
	cpi		r23, 0x99				; if lives already = 99
	breq	incLivesEnd				; then don't incriment
	mov		r22, r23				; Make a copy of the BCD variable
	andi	r22, 0x0F				; clear out the high nibble
	cpi		r22, 0x09				; see if the lowest order BCD digit is 9
	brne	incLivesNoRollover		; if the lower nibble was 9, then
	subi	r23, -(0x07)			; Add 7 to the whole BCD value (incriment the 10s and leave 0 in the units)
	sts		lives, r23				; save the variable to RAM
	ret
incLivesNoRollover:
	inc		r23						; if the lowest BCD digit was not NINE you can just incriment and
	sts		lives, r23				; save the value to RAM
incLivesEnd:
	ret


; void AddScore(uint16_t points)
;
; Adds an 16bit (4 digit) BCD number to the score in RAM
;
; Input
;		Packed 16 bit BCD 	in r24:25
; Returns
;		Nothing
; Modifies
;		score[] 4 byte array in RAM (8 digit BDC number)
;
; Trashes
;		r25
; 		r21
;		r20
;		r19


addScore:
	lds		r21, score+0		; load the LS Byte from RAM into the accumulator
	clr		r19					; clear the carry byte
	ldi		r20, 0x66			; load the decimal-adjusting-byte-value
	add		r21, r20			; Add the adjuster to the accumulator
	add		r21, r24			; add the lowest byte from the 16 bit BCD
	brcc	NoCarry1			; if there was no carry (from high nibble)
	inc		r19					; then incriment the carry byte
	andi	r20, 0x0F			; and clear out the hi nibble of the decimal-adjust-byte-value to later subtract
NoCarry1:
	brhc	NoHalfCarry1		; is there was no carry (from the low nibble)
	andi	r20, 0xF0			; clear out the lo nibble of the decimal-adjust-byte-value to later subtract
NoHalfCarry1:
	sub		r21, r20			; subtract the decimal-adjust-byte-value
	sts		score+0, r21		; save the new score lowest byte to RAM

	lds		r21, score+1
	add		r25, r19			; All "As Above" apart from adding the carry to the accumulator before clearing it
	clr		r19
	ldi		r20, 0x66
	add		r21, r20
	add		r21, r25
	brcc	NoCarry2
	inc		r19
	andi	r20, 0x0F
NoCarry2:
	brhc	NoHalfCarry2
	andi	r20, 0xF0
NoHalfCarry2:
	sub		r21, r20
	sts		score+1, r21


	lds		r21, score+2		; as above
	add		r21, r19
	clr		r19
	ldi		r20, 0x66
	add		r21, r20
	brcc	NoCarry3
	inc		r19
	andi	r20, 0x0F
NoCarry3:
	brhc	NoHalfCarry3
	andi	r20, 0xF0
NoHalfCarry3:
	sub		r21, r20
	sts		score+2, r21

	lds		r21, score+3		; as above
	add		r21, r19
	clr		r19
	ldi		r20, 0x66
	add		r21, r20
	brcc	NoCarry4
	ldi		r21, 0x99			; in the unlikey even someone ever clock this
	sts		score+3, r21		; then just make the score 99999999
	sts		score+2, r21
	sts		score+1, r21
	sts		score+0, r21
	ret
NoCarry4:
	brhc	NoHalfCarry4
	andi	r20, 0xF0
NoHalfCarry4:
	sub		r21, r20
	sts		score+3, r21

	ret

; void AddScoreFull(uint32_t points)
;
; Adds an 32bit (8 digit) BCD number to the score in RAM
;
; Input
;		Packed 32 bit BCD 	in r22:r23:r24:25
; Returns
;		Nothing
; Modifies
;		score[] 4 byte array in RAM
;
; Trashes
;		r25
;		r24
;		r23
; 		r21
;		r20
;		r19

addScoreFull:
	lds		r21, score+0		; load the LS Byte from RAM into the accumulator
	clr		r19					; clear the carry byte
	ldi		r20, 0x66			; load the decimal-adjusting-byte-value
	add		r21, r20			; Add the adjuster to the accumulator
	add		r21, r22			; add the lowest byte from the 32 bit BCD
	brcc	NoCarryFull1			; if there was no carry (from high nibble)
	inc		r19					; then incriment the carry byte
	andi	r20, 0x0F			; and clear out the hi nibble of the decimal-adjust-byte-value to later subtract
NoCarryFull1:
	brhc	NoHalfCarryFull1		; is there was no carry (from the low nibble)
	andi	r20, 0xF0			; clear out the lo nibble of the decimal-adjust-byte-value to later subtract
NoHalfCarryFull1:
	sub		r21, r20			; subtract the decimal-adjust-byte-value
	sts		score+0, r21		; save the new score lowest byte to RAM

	lds		r21, score+1
	add		r23, r19			; All "As Above" apart from adding the carry to the accumulator before clearing it
	clr		r19
	ldi		r20, 0x66
	add		r21, r20
	add		r21, r23
	brcc	NoCarryFull2
	inc		r19
	andi	r20, 0x0F
NoCarryFull2:
	brhc	NoHalfCarryFull2
	andi	r20, 0xF0
NoHalfCarryFull2:
	sub		r21, r20
	sts		score+1, r21

	lds		r21, score+2		; as above
	add		r24, r19
	clr		r19
	ldi		r20, 0x66
	add		r21, r20
	add		r21, r24
	brcc	NoCarryFull3
	inc		r19
	andi	r20, 0x0F
NoCarryFull3:
	brhc	NoHalfCarryFull3
	andi	r20, 0xF0
NoHalfCarryFull3:
	sub		r21, r20
	sts		score+2, r21

	lds		r21, score+3		; as above
	add		r25, r19
	clr		r19
	ldi		r20, 0x66
	add		r21, r20
	add		r21, r25
	brcc	NoCarryFull4
	ldi		r21, 0x99			; in the unlikey even someone ever clock this
	sts		score+3, r21		; then just make the score 99999999
	sts		score+2, r21
	sts		score+1, r21
	sts		score+0, r21
	ret

NoCarryFull4:
	brhc	NoHalfCarryFull4
	andi	r20, 0xF0
NoHalfCarryFull4:
	sub		r21, r20
	sts		score+3, r21

	ret


; void SetPixelFastC(uint8_t, x_char, uint8_t y_char)
;
; C-Callable version of SetPixel
;
; NOTE:  This routine will only work with ramTiles and VRAM on 1K boundaries
;
; Inputs
; 		x_pixel in R24
; 		y_pixel in R22
;
; Returns
; 		void
;
; Modified RAM
; 		on success
;				May modify both ramTile array and vram aray.  Read description
;		on fail
;			   	Nill
; Trashed
;		R1
;		R0
;		R19
;		R21
;		R22
;		R23
;		R24
;		R25
;		R26:27


SetPixelFastC:

	cpi		r22, 224			; Make sure we are not trying to plot a pixel out out bounds
	brsh	SPF_Fail_2			; if so fail (no need to restore R0)
	cpi		r22, 16
	brlo	SPF_Fail_2

	ldi		r23, 0x04			;
	mul		r22, r23			; Multiply Y x4 Result in R1/0                 .0.0.0.0.0.y7.y6:y5y4y3y2y1y0.0.0
	movw	XL, r0				; Move result to X register
	andi	XL, 0b11100000		; Clear out Y0..2       R27/26                 .0.0.0.0.0.y7.y6:y5y4y3.0.0.0.0.0
	ldi		r23, 0x20
	mul		r24, r23			; Multiply X x32 Result in R1/0                .0.0.0x7x6x5x4x3:x2x1x0.0.0.0.0.0
	or		XL, r1				; OR X7..3 into low byte of VRAM_Address	   .0.0.0.0.0.0y7y6:y5y4y3x7x6x5x4x3
	subi	XH, hi8(-(vram))	; Add the base address of VRAM				   .0.0.0.0.1.1y7y6:y5y4y3x7x6x5x4x3

    ld      r23, X              ; Get the Tile to use from VRAM address. r23 is now Tile#

    cpi     r23, 0x00           ; See if there is already a tile allocated at this X/Y address
    brne    SPF_Allocated

    lds     r23,nextFreeRamTile         ; If not allocated then we need to get # of the next free tile
    cpi     r23,254					    ; make sure we have not run out of ram tiles
    breq    SPF_Fail

    subi    r23, -(2)                   ; Save the new value of "next free" into
    st      X, r23                      ; After alloacting new tile save the # in the VRAM location X/Y
    sts     nextFreeRamTile, r23

SPF_Allocated:


; We want to end up as  .0.0.0.0.0y2y1y0  t7t6t5t4t3t2t1x2

	mov		XL, r23				; Copy the RAMTile # to XL
	bst		r24, 2				; copy X2 to the LSB of XL (overwriting T0)
	bld		XL, 0				;									XL  => t7t6t5t4t3t2t1x2
	mov		XH, r22				; Copy Y to XH              		XH  => y7y6y5y4y3y2y1y0
	andi	XH, 0x07			; Clear the 5 highest bits  		XH  => .0.0.0.0.0y2y1y0
    subi    XH, hi8(-(ramTiles))

    ld      r23, X              	; Get TileRowByte / Pixels

; This converts X1:X0 into the pixel mask
;
; X value of converts to pixel mask
; 	xxxxxx00 11111100
; 	xxxxxx01 11110011
; 	xxxxxx10 11001111
; 	xxxxxx11 00111111


	ldi		r22, 0b1111110			; Load the pixel mask value 111111000
	sbrc	r24, 0					; if the LSB is set then change the value0
	ldi		r22, 0b1111001			; 111100111
	sbrc	r24, 1					; if the 2nd LSB is set then1
	swap	r2						; swap the nibbles to make the value either 11001111 or 001111112

	lds     r25,ColourMask
	or		r22, r25
    and    	r23, r22            	; OR TileRowByte with the pixel mask
    st      X, r23              	; write TileRowByte back to memory

SPF_Fail:
    clr     r1                 		; clear r1 back to zero after the MUL trashing.
SPF_Fail_2:
    ret




































; fast_line_enty
;
; Saves trashed "call saved" registers to the stack
; and loads three frequently used literals into high registers

.macro fast_line_entry
	ldi		r25, 0x04
	mov		r10, r25
	ldi		r25, 0x20
	mov		r11, r25
	lds     r25, ColourMask
.endm

; The registers in Bresh_Line have been moved around to free hi registers
; This uses more low regsiters that must be saved for the C version of the routine
; but the26 clock  expense here is justified because the C version is never called
; when things need to be fast

.macro fast_line_entry_C
	push	r7
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15						; Save register used by "err"
	push	r16						; Save register used by "dummy"
	push	r17						; Save register used by ramTilePixelByte
	push	r28						; Save register used by 0x04 to be used repeatedly by MUL
	push	r29						; Save register used by 0x20 to be used repeatedly by MUL
.endm

; fast_line_exit
;
; Saves the currently held local copy of the pixels in ramTilePixelByte
; and restores trashed registers and the stack

.macro fast_line_exit
	fast_line_write_ramTilePixelByte	; save ramTilePixelByte into the location pointed to by ramTileByteAddress
	fast_line_exit_common
.endm

.macro fast_line_exit_fail
	fast_line_exit_common
.endm

.macro fast_line_exit_common
.endm

.macro fast_line_exit_C
	pop		r29
	pop		r28
	pop		r17
	pop		r16
	pop		r15
	pop		r14
	pop		r13
	pop		r12
	pop		r11
	pop		r10
	pop		r9
	pop		r8
	pop		r7
	clr		r1
.endm

; fast_line_convert_x0_y0_into_VRAM_address
;
; converts the X0 and Y0 address passed in r24 and r22 into a VRAM memory
; location and leaves this result in R26:27 (VRAM_Address)
;
; Inputs
; 			Y0 address = R22
;			X0 address = R24
; Outputs
;			VRAM_Address = R26:27 (X)
;
; Requires that the constants 4 and 32 are pre-loaded in R10, R11
;
; Trashes R0:1

.macro fast_line_convert_x0_y0_into_VRAM_address
	mul		r22, r10				; Multiply Y0 by 4   y7y6y5y4y3y2y1y0 		=> .0.0.0.0.0.0y7y6:y5y4y3y2y1y0.0.0
	movw	r26, r0					; move the 16 bit result into VRAM_Address
	andi	r26, 0b11100000			; clear out the bits that are used for Xn	   .0.0.0.0.0.0y7y6:y5y4y3.0.0.0.0.0
	mul		r24, r11				; Multiply X0 by 32  x7x6x5x4x3x2x1x0 		=> .0.0.0x7x6x5x4x3:x2x1x0.0.0.0.0.0
	or		r26, r1					; OR X7..3 into low byte of VRAM_Address	   .0.0.0.0.0.0y7y6:y5y4y3x7x6x5x4x3
	subi	r27, hi8(-(vram))		; Add the base address of VRAM				   .0.0.0.0.1.1y7y6:y5y4y3x7x6x5x4x3
.endm

; fast_line_update_Xn_in_VRAM_address
;
; updates the X0 address part of the pointer held in R26:27 (VRAM_Address)
; This is done constantly in Horizontal lines and often in shallow diagonal lines
; Updating only the X part of the address takes 4 clocks cycles rather than
; 8 clk for "fast_line_convert_x0_y0_into_VRAM_address"
;
; Note: there is no equivelant "update Y only" routine as updating Y takes 8 clks
;
; Inputs
;			X0 address = R24
; Outputs
;			VRAM_Address = R26:27 (X)
;
; Requires that the constant 32 be in R11
;
; Trashes R0:1

.macro fast_line_update_Xn_in_VRAM_address
	andi	r26, 0b11100000			; clear out the old Xn bits 			  X => .0.0.0.0.1.1y7y6:y5y4y3.0.0.0.0.0
	mul		r24, r11				; Multiply X0 by 32  x7x6x5x4x3x2x1x0 	 R0 => .0.0.0x7x6x5x4x3:x2x1x0.0.0.0.0.0
	or		r26, r1					; OR X7..3 into low byte of VRAM_Address  X => .0.0.0.0.0.0y7y6:y5y4y3x7x6x5x4x3
.endm

; fast_line_convert_ramTileNo_into_ramTileByteAddress
;
; Takes a 8 bit value in R30 (ramTileNumber) and the Y0 address (R22) and the LSB of X (R24) and converts
; this into the the 16 bit pointer in R30:31 (ramTileByteAddress)
;
; Inputs
;           X0 address    = r24
;			Y0 address    = R22
;			ramTileNumber = R30
; Outputs
;			ramTileByteAddress = R30:31 (Z)
;
; Requires that the constant 4 be in R10
;
; Trashes R0:1

.macro fast_line_convert_ramTileNo_into_ramTileByteAddress
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Tile_X_only
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Y_Base_only
.endm

.macro fast_line_convert_ramTileNo_into_ramTileByteAddress_Tile_X_only
	bst		r24, 2					; copy X2 to the LSB of R30 (overwriting T0)
	bld		r30, 0					;						R30  => t7t6t5t4t3t2t1x2
.endm
.macro fast_line_convert_ramTileNo_into_ramTileByteAddress_Y_Base_only
	mov		R31, r22				; Copy Y to R31              		R31  => y7y6y5y4y3y2y1y0
	andi	R31, 0x07				; Clear the 5 highest bits  		R31  => .0.0.0.0.0y2y1y0
	subi	r31, hi8(-(ramTiles))	; add the base address of RamTles to
.endm


; fast_line_get_ramTilePixelByte
;
; Gets a local copy of the 8bits (representing 8 pixels) pointed to by
; R30:31 (ramTileByteAddress)
;
; Inputs
;			ramTileByteAddress = R30:31 (Z)
; Outputs
;			ramTilePixelByte = R12

.macro fast_line_get_ramTilePixelByte
	ld		r12, Z					; get local copy of 8 pixels at the ramTileByteAddress into ramTilePixelByte
.endm

; fast_line_get_pixel_mask
;
; Turns the lower 2 bits of R24 (X0 Address) into a pixelMask used to OR
; on a pixel within the pixelByte
;
; Inputs
;			X0 address = R24
; Outputs
;			pixelMask = R23
; X value of converts to pixel mask
; 	xxxxxx00 11111100
; 	xxxxxx01 11110011
; 	xxxxxx10 11001111
; 	xxxxxx11 00111111


.macro fast_line_get_pixel_mask
	ldi		r23, 0b11111100			; Load the pixel mask value 11111100
	sbrc	r24, 0					; if the LSB is set then change the value
	ldi		r23, 0b11110011			; 11110011
	sbrc	r24, 1					; if the 2nd LSB is set then
	swap	r23						; swap the nibbles to make the value either 11001111 or 00111111
.endm

; fast_line_OR_pixel_mask
;
; OR R23 (pixelMask) with R12 (the local copy of the pixelByte)
;
; Inputs
;			pixelMask = R23
;			ramTilePixelByte = R12
; Outputs
;			ramTilePixelByte = R12

.macro fast_line_OR_pixel_mask
	mov		r0, r23					; Save the Pixel Mask
	or		r0, r25					; OR it with the Colour Mask
	and		r12, r0					; AND the local copy of ramTilePixelByte with pixelMask
.endm

; fast_line_write_ramTilePixelByte
;
; Saves R12 (Local copy of ramTilePixelByte) into the RAM location pointed to by
; R30:31 (ramTileByteAddress)
;
; Inputs
;			ramTilePixelByte = R12
;			ramTileByteAddress = R30:31 (Z)
; Outputs
;			RAM Location pointed to by Z

.macro fast_line_write_ramTilePixelByte
	st		Z, r12					; save ramTilePixelByte into the location pointed to by ramTileByteAddress
.endm

; fast_line_get_ramTileNo
;
; Get the ramTileNumber pointed to by VRAM_Address
; If the VRAM_Address is currently pointing to ramTileNumber = 0 then
; try allocate a new ramTile and save it in that VRAM_Address Location.
; If no more ramTiles are free then FAIL and exit the line draw routine
;
; NOTE: "FAIL" is a jump to the exit location of the routine.  This
; means you can NOT RCALL/CALL any of these routines. They all must
; be inlined
;
; Inputs
;		VRAM_Address = R26:27 (X)
;		nextFreeRamTile = 8 bit Variable in RAM
; Outputs
; 		ramTileNumber = R30
;		nextFreeRamTile = 8 bit Variable in RAM
;		RAM Location pointed to by Z
; Trashes
;		R30:31 (ramTileByteAddress)

.macro fast_line_get_ramTileNo
	ld		r30, X					; get ramTileNo from VRAM_address
	cpi		r30, 0x00				; see if there is already a ramTile allocated
	brne	.Lfast_line_get_ramTileNo_allocated\@
									; if ramTileNO != 0 then there is already a tile allocated at this address
	lds		r30,nextFreeRamTile 	; otherwise get the next free tile number to allocate
	cpi     r30,254					; compare this with the maximum number of tiles available ((*2)-1)
	brne	.Lfast_line_get_ramTileNo_continue\@
									; if there are more free tiles available then continue

	ldi		r30, 0x00			; OPTIONAL - reuse RAMTiles
	//rjmp	bresh_pixel_fail		; otherwise FAIL

.Lfast_line_get_ramTileNo_continue\@:	; continue (no "out of ramTiles error")
	subi	r30, -(2)				; inc "nextFreeRamTile" (2 bytes per tile in 2 Bpp mode)
	st		X, r30					; save the newly allocated ramTileNo into VRAM at VRAM_address
	sts		nextFreeRamTile, r30	; Save it to variable in RAM
.Lfast_line_get_ramTileNo_allocated\@:	; allocated
.endm

; fast_line_X_plus
; fast_line_X_minus
; fast_line_Y_plus
; fast_line_Y_minus
; fast_line_XP_YP (X plus  Y plus )
; fast_line_XM_YP (X minus Y plus )
; fast_line_XM_YM (X minus Y minus)
; fast_line_XP_YM (X plus  Y minus)
;
; Eight inline routines to quickly put a pixel in the 8 locations
; next to the last pixel that was plotted
;
; +------+------+------+
; |XM_YM |YM    |XP_YM |
; |      |      |      |
; |      |      |      |
; +------+------+------+
; |XM    |      |XP    |
; |      |      |      |
; |      |      |      |
; +------+------+------+
; |XM_YP |YM    |XP_YP |
; |      |      |      |
; |      |      |      |
; +------+------+------+
;
; During Bresenham linedraw routine all the pixels being plotted (except the first)
; are in the one of eight locations on the screen next to the last pixel.
;
; Due to that fact we will already know most of the information needed to plot
; the current pixel and do not need to recalculate it.
;
; Example - for an X_Plus pixel plot we already know the Y address and we have a
; 3 in 4 chance of already knowing the pixelByte.  We also know what the last
; pixelMask was and can calculate it with an logical shift right (LSL)
;
; Also in the 1/8 chance that we need a NEW pixel byte, we can also save some time
; by not calculating the full X/Y address as we already know the Y address
;
; 3/4th of the time fast_line_X_plus will run in 8 CLKs
; 1/4th of the time fast_line_X_plus will run in 31/40 CLKs (existing/new ramTile)
;
; This is opposed to 37/47 CLKs for a routine that blindly calls "PutPixel"

.macro fast_line_X_plus
	lsl		r23								; rotate the pixel mask
	brcs	.Lfast_line_X_plus_same_byte\@	; if there is a carry then we dont need to :


	fast_line_write_ramTilePixelByte		; save the current pixel byte before getting new one
	fast_line_update_Xn_in_VRAM_address		; update the X part of VRAM_address (Y has not changed)
	fast_line_get_ramTileNo					; get the new ramTileNumber
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Tile_X_only
	fast_line_get_ramTilePixelByte
	ldi		r23, 0b11111100
	rjmp	.Lfast_line_X_plus_common\@

.Lfast_line_X_plus_same_byte\@:  			; Still on the same ramTilePixelByte
	lsl		r23
	ori		r23, 0b00000011
.Lfast_line_X_plus_common\@:
	fast_line_OR_pixel_mask
.endm

.macro fast_line_X_minus
	lsr		r23								; rotate the pixel mask
	brcs	.Lfast_line_X_minus_same_byte\@	; if there is no carry then we dont need to :

	fast_line_write_ramTilePixelByte		; save the current pixel byte before getting new one
	fast_line_update_Xn_in_VRAM_address		; update the X part of VRAM_address (Y has not changed)
	fast_line_get_ramTileNo					; get the new ramTileNumber
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Tile_X_only
	fast_line_get_ramTilePixelByte
	ldi		r23, 0b00111111
	rjmp	.Lfast_line_X_minus_common\@

.Lfast_line_X_minus_same_byte\@:  			; Still on the same ramTilePixelByte
	lsr		r23
	ori		r23, 0b11000000
.Lfast_line_X_minus_common\@:
	fast_line_OR_pixel_mask
.endm

.macro fast_line_Y_plus
	fast_line_write_ramTilePixelByte	; save the current pixel byte before getting new one
	mov		r31, r22					; save a copy of the lo8 byte of ramTileByteAddress
	andi	r31, 0b00000111				; test to see if the bottom 3 bits are clear
	brne	.Lfast_line_Y_plus_no_3bit_rollover\@	; if not we are still on the same ramTile

												; otherwise we have to
	fast_line_convert_x0_y0_into_VRAM_address	; get the new vram_address (only Y was changed but update_X/Y is as fast)
	fast_line_get_ramTileNo						; get the new ramTileNumber
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Tile_X_only

.Lfast_line_Y_plus_no_3bit_rollover\@:

	fast_line_convert_ramTileNo_into_ramTileByteAddress_Y_Base_only

	fast_line_get_ramTilePixelByte
	fast_line_OR_pixel_mask

.endm

.macro fast_line_Y_minus
	fast_line_write_ramTilePixelByte	; save the current pixel byte before getting new one
	mov		r31, r22
	andi	r31, 0b00000111
	cpi		r31, 0b00000111
	brne	.Lfast_line_Y_minus_no_3bit_rollover\@	; if not we will still be on the same ramTile after the DEC

												; otherwise we have to
	fast_line_convert_x0_y0_into_VRAM_address	; get the new vram_address (only Y was changed but update_X/Y is as fast)
	fast_line_get_ramTileNo						; get the new ramTileNumber
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Tile_X_only

.Lfast_line_Y_minus_no_3bit_rollover\@:

	fast_line_convert_ramTileNo_into_ramTileByteAddress_Y_Base_only

	fast_line_get_ramTilePixelByte
	fast_line_OR_pixel_mask

.endm

.macro fast_line_X_Wrap
	fast_line_update_Xn_in_VRAM_address
	fast_line_get_ramTileNo
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Tile_X_only
.endm
.macro fast_line_Y_Wrap //23
	fast_line_convert_x0_y0_into_VRAM_address
	fast_line_get_ramTileNo
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Y_Base_only
	fast_line_get_pixel_mask
.endm
.macro fast_line_XY_Wrap //23
	fast_line_convert_x0_y0_into_VRAM_address
	fast_line_get_ramTileNo
	fast_line_convert_ramTileNo_into_ramTileByteAddress
	fast_line_get_pixel_mask
.endm

.macro fast_line_XP_YP
	fast_line_write_ramTilePixelByte		; save the current pixel byte before getting new one

	lsl		r23
	brcc	.Lfast_line_XP_YP_DoAll\@

	mov		r31, r22
	andi	r31, 0b00000111
	breq	.Lfast_line_XP_YP_DoAll\@

.Lfast_line_XP_YP_DoXOnly\@:
	lsl		r23
	ori		r23, 0b00000011
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Y_Base_only

	rjmp	.Lfast_line_XP_YP_DoCommon\@
.Lfast_line_XP_YP_DoAll\@:
	fast_line_XY_Wrap
.Lfast_line_XP_YP_DoCommon\@:

	fast_line_get_ramTilePixelByte
	fast_line_OR_pixel_mask
.endm

.macro fast_line_XM_YP
	fast_line_write_ramTilePixelByte		; save the current pixel byte before getting new one

	lsr		r23
	brcc	.Lfast_line_XM_YP_DoAll\@

	mov		r31, r22
	andi	r31, 0b00000111
	breq	.Lfast_line_XM_YP_DoAll\@

.Lfast_line_XM_YP_DoXOnly\@:
	lsr		r23
	ori		r23, 0b11000000
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Y_Base_only

	rjmp	.Lfast_line_XM_YP_DoCommon\@
.Lfast_line_XM_YP_DoAll\@:
	fast_line_XY_Wrap
.Lfast_line_XM_YP_DoCommon\@:

	fast_line_get_ramTilePixelByte
	fast_line_OR_pixel_mask
.endm


.macro fast_line_XP_YM
	fast_line_write_ramTilePixelByte		; save the current pixel byte before getting new one

	lsl		r23
	brcc	.Lfast_line_XP_YM_DoAll\@

	mov		r31, r22
	andi	r31, 0b00000111
	cpi		r31, 0b00000111
	breq	.Lfast_line_XP_YM_DoAll\@

.Lfast_line_XP_YM_DoXOnly\@:
	lsl		r23
	ori		r23, 0b00000011
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Y_Base_only

	rjmp	.Lfast_line_XP_YM_DoCommon\@
.Lfast_line_XP_YM_DoAll\@:
	fast_line_XY_Wrap
.Lfast_line_XP_YM_DoCommon\@:

	fast_line_get_ramTilePixelByte
	fast_line_OR_pixel_mask
.endm

.macro fast_line_XM_YM
	fast_line_write_ramTilePixelByte		; save the current pixel byte before getting new one

	lsr		r23
	brcc	.Lfast_line_XM_YM_DoAll\@

	mov		r31, r22
	andi	r31, 0b00000111
	cpi		r31, 0b00000111
	breq	.Lfast_line_XM_YM_DoAll\@

.Lfast_line_XM_YM_DoXOnly\@:
	lsr		r23
	ori		r23, 0b11000000
	fast_line_convert_ramTileNo_into_ramTileByteAddress_Y_Base_only

	rjmp	.Lfast_line_XM_YM_DoCommon\@
.Lfast_line_XM_YM_DoAll\@:
	fast_line_XY_Wrap
.Lfast_line_XM_YM_DoCommon\@:

	fast_line_get_ramTilePixelByte
	fast_line_OR_pixel_mask
.endm

; Takes the angle in R(R21) and distance in D(R19) and leaves sin(R)*D in X and cos(R)*D in Y
;
; Trashes 	X   (is always refreshed by DRAWLINE)
; 			r23 (is always refreshed by DRAWLINE)
; trashed

.macro drawob_sin_cos_mul_XY0
	rcall	drawob_sin_cos_mul_XY0_nm
.endm

.macro drawob_sin_cos_mul_XY0z
	mov		r30, r21
    ldi 	r31, hi8(SinCosTable)
	lpm		r23, Z
	mulsu	r23, R19
	mov		r24, r1

	subi	r30, (-(64))
	lpm		r23, Z
	mulsu	r23, R19
	mov		r22, r1
	add		r24, r17
	add		r22, r16
.endm

.macro drawob_sin_cos_mul_XY1
	rcall	drawob_sin_cos_mul_XY1_nm
.endm

.macro drawob_sin_cos_mul_XY1z
	mov		r30, r21
    ldi 	r31, hi8(SinCosTable)
	lpm		r23, Z
	mulsu	r23, R19
	mov		r20, r1

	subi	r30, (-(64))
	lpm		r23, Z
	mulsu	r23, R19
	mov		r18, r1
	add		r20, r17
	add		r18, r16
.endm

.macro drawob_entry

	ldd		r17, y+3	; load X
	ldd		r16, y+4	; load Y
	ldd		r21, y+5	; load R
	ldd		r9,  y+6	; load S
						; y+7 = animation
.endm

.macro drawob_load_colour col
	ldi		r25, \col
.endm

.macro drawob_load_dist dist
	ldi		r19, \dist				; Load the DISTANCE before scale
	mul		r19, r9					; multiply it by scale
	mov		r19, r1
.endm

.macro drawob_add_angle angle
	subi	r21, \angle
.endm

.macro drawob_save_XY0
	mov		r8, r24					; This would be so much nicer with a movw if XY where next to each other,  May move them later
	mov		r7, r22
.endm

.macro drawob_save_XY1
	mov		r8, r20
	mov		r7, r18
.endm

.macro drawob_restore_XY0
	mov		r24, r8
	mov		r22, r7
.endm
.macro drawob_restore_XY1
	mov		r20, r8
	mov		r18, r7
.endm



.set COLOUR_YELLOW,  0b00000000
.set COLOUR_RED,     0b01010101
.set COLOUR_GREEN,   0b10101010
.set COLOUR_CLEAR,   0b11111111
.set COLOUR_RED_YEL, 0b01000100

; bresh_q1_asm
; bresh_q2_asm
; bresh_q3_asm
; bresh_q4_asm
;
; Four routines to cover the four quadrants in Bresenhams line draw algo.
;
; Q1 = X+Y+
; Q2 = X-Y+
; Q3 = X-Y-
; Q4 = X+Y-
;
; Although it is normal to cover the quadrants with "Step_X" and "Step"Y"
; variables with only one set of decisions to be made before the two main
; loops.  In this case (for speed) we also need to run (inline) routines
; that are specific to X+/X-/Y+/Y-.  It would be too expensive to make
; decision/branch each itteration of the inner loop.  So after our first
; quadrant decision, we just JUMP to one of four routines that are hard
; coded for "Step_X", "Step_Y" and the special case pixel plotting
; routines.
;
; Apart from that they are bog-standard Bresenham and should not need
; any indepth explanation

bresh_q1_asm:
	; dx = gx1 - gx0
	mov		r13, r20				; Copy gx1 into dx
	sub		r13, r24				; subtract (without carry)  gx0 from dx (contains gx1)

	; dy = gy1 - gy0
	mov		r14, r18				; copy gy1 into dy
	sub		r14, r22				; subtract (without carry) gy0 from dy (contains gy1)

	; if (dx < dy)
	cp		r13, r14
	brge	bresh_q1_shallow		; (not_true) -> ELSE
	rjmp	bresh_q1_steep

bresh_q1_shallow:
	; err = dx >> 1
	mov		r15, r13				; copy dx into err
	asr		r15						; dx >> 1

bresh_q1_shallow_loop:
	; err = err - dy
	sub		r15, r14
	; gx0++
	inc		r24

	; if (err < 0)
	sbrs	r15, 7						; if bit7 is 1 the result is negative
	rjmp	bresh_q1_shallow_no_minor	; (not_true) -> continue

bresh_q1_shallow_minor:
	; err = err + dx
	add		r15, r13
	; gy0++
	inc		r22
	; SetPixelFastC(gx0, gy0)
	fast_line_XP_YP
	; while (gx0 != gx1)
	cp		r24, r20
	breq	bresh_q1_shallow_exit
	rjmp	bresh_q1_shallow_loop


bresh_q1_shallow_no_minor:
	; SetPixelFastC(gx0, gy0)
	fast_line_X_plus


	; while (gx0 != gx1)
	cp		r24, r20
	breq	bresh_q1_shallow_exit
	rjmp	bresh_q1_shallow_loop
bresh_q1_shallow_exit:
	fast_line_exit
	ret



bresh_q1_steep:
	; err = dy >> 1
	mov		r15, r14				; copy dy into err
	asr		r15						; dy >> 1

bresh_q1_steep_loop:
	; err = err - dx
	sub		r15, r13

	; gy0++
	inc		r22

	; if (err < 0)
	sbrs	r15, 7					; if bit7 is 1 the result is negative
	rjmp	bresh_q1_steep_no_minor	; (not_true) -> continue

bresh_q1_steep_minor:
	; err = err + dy
	add		r15, r14
	; gx0++
	inc		r24
	; SetPixelFastC(gx0, gy0)
	fast_line_XP_YP
	; while (gy0 != gy1)
	cp		r22, r18
	breq	bresh_q1_steep_exit
	rjmp	bresh_q1_steep_loop


bresh_q1_steep_no_minor:
	; SetPixelFastC(gx0, gy0)
	fast_line_Y_plus


	; while (gy0 != gy1)
	cp		r22, r18
	breq	bresh_q1_steep_exit
	rjmp	bresh_q1_steep_loop

bresh_q1_steep_exit:
	fast_line_exit
	ret


bresh_q2_asm:
	; dx = gx0 - gx1
	mov		r13, r24				; Copy gx0 into dx
	sub		r13, r20				; subtract (without carry)  gx1 from dx (contains gx0)

	; dy = gy1 - gy0
	mov		r14, r18				; copy gy1 into dy
	sub		r14, r22				; subtract (without carry) gy0 from dy (contains gy1)

	; if (dx < dy)
	cp		r13, r14
	brge	bresh_q2_shallow		; (not_true) -> ELSE
	rjmp	bresh_q2_steep

bresh_q2_shallow:
	; err = dx >> 1
	mov		r15, r13				; copy dx into err
	asr		r15						; dx >> 1

bresh_q2_shallow_loop:
	; err = err - dy
	sub		r15, r14
	; gx0--
	dec		r24

	; if (err < 0)
	sbrs	r15, 7						; if bit7 is 1 the result is negative
	rjmp	bresh_q2_shallow_no_minor	; (not_true) -> continue

bresh_q2_shallow_minor:
	; err = err + dx
	add		r15, r13
	; gy0++
	inc		r22
	; SetPixelFastC(gx0, gy0)
	fast_line_XM_YP
	; while (gx0 != gx1)
	cp		r24, r20
	breq	bresh_q2_shallow_exit
	rjmp	bresh_q2_shallow_loop


bresh_q2_shallow_no_minor:
	; SetPixelFastC(gx0, gy0)
	fast_line_X_minus
	; while (gx0 != gx1)
	cp		r24, r20
	breq	bresh_q2_shallow_exit
	rjmp	bresh_q2_shallow_loop
bresh_q2_shallow_exit:
	fast_line_exit
	ret


bresh_q2_steep:
	; err = dy >> 1
	mov		r15, r14				; copy dy into err
	asr		r15						; dy >> 1

bresh_q2_steep_loop:
	; err = err - dx
	sub		r15, r13
	; gy0++
	inc		r22

	; if (err < 0)
	sbrs	r15, 7					; if bit7 is 1 the result is negative
	rjmp	bresh_q2_steep_no_minor	; (not_true) -> continue

bresh_q2_steep_minor:
	; err = err + dy
	add		r15, r14
	; gx0--
	dec		r24
	; SetPixelFastC(gx0, gy0)
	fast_line_XM_YP
	; while (gy0 != gy1)
	cp		r22, r18
	breq	bresh_q2_steep_exit
	rjmp	bresh_q2_steep_loop


bresh_q2_steep_no_minor:
	; SetPixelFastC(gx0, gy0)
	fast_line_Y_plus
	; while (gy0 != gy1)
	cp		r22, r18
	breq	bresh_q2_steep_exit
	rjmp	bresh_q2_steep_loop
bresh_q2_steep_exit:
	fast_line_exit
	ret




bresh_q3_asm:
	; dx = gx0 - gx1
	mov		r13, r24				; Copy gx0 into dx
	sub		r13, r20				; subtract (without carry)  gx1 from dx (contains gx0)

	; dy = gy0 - gy1
	mov		r14, r22				; copy gy0 into dy
	sub		r14, r18				; subtract (without carry) gy1 from dy (contains gy0)

	; if (dx < dy)
	cp		r13, r14
	brge	bresh_q3_shallow		; (not_true) -> ELSE
	rjmp	bresh_q3_steep

bresh_q3_shallow:
	; err = dx >> 1
	mov		r15, r13				; copy dx into err
	asr		r15						; dx >> 1

bresh_q3_shallow_loop:
	; err = err - dy
	sub		r15, r14
	; gx0--
	dec		r24

	; if (err < 0)
	sbrs	r15, 7						; if bit7 is 1 the result is negative
	rjmp	bresh_q3_shallow_no_minor	; (not_true) -> continue

bresh_q3_shallow_minor:
	; err = err + dx
	add		r15, r13
	; gy0--
	dec		r22
	; SetPixelFastC(gx0, gy0)
	fast_line_XM_YM
	; while (gx0 != gx1)
	cp		r24, r20
	breq	bresh_q3_shallow_exit
	rjmp	bresh_q3_shallow_loop


bresh_q3_shallow_no_minor:
	; SetPixelFastC(gx0, gy0)
	fast_line_X_minus
	; while (gx0 != gx1)
	cp		r24, r20
	breq	bresh_q3_shallow_exit
	rjmp	bresh_q3_shallow_loop
bresh_q3_shallow_exit:
	fast_line_exit
	ret


bresh_q3_steep:
	; err = dy >> 1
	mov		r15, r14				; copy dy into err
	asr		r15						; dy >> 1

bresh_q3_steep_loop:
	; err = err - dx
	sub		r15, r13
	; gy0--
	dec		r22

	; if (err < 0)
	sbrs	r15, 7					; if bit7 is 1 the result is negative
	rjmp	bresh_q3_steep_no_minor	; (not_true) -> continue

bresh_q3_steep_minor:
	; err = err + dy
	add		r15, r14
	; gx0--
	dec		r24
	; SetPixelFastC(gx0, gy0)
	fast_line_XM_YM
	; while (gy0 != gy1)
	cp		r22, r18
	breq	bresh_q3_steep_exit
	rjmp	bresh_q3_steep_loop


bresh_q3_steep_no_minor:
	; SetPixelFastC(gx0, gy0)
	fast_line_Y_minus
	; while (gy0 != gy1)
	cp		r22, r18
	breq	bresh_q3_steep_exit
	rjmp	bresh_q3_steep_loop
bresh_q3_steep_exit:
	fast_line_exit
	ret


bresh_q4_asm:
	; dx = gx1 - gx0
	mov		r13, r20				; Copy gx1 into dx
	sub		r13, r24				; subtract (without carry)  gx0 from dx (contains gx1)

	; dy = gy0 - gy1
	mov		r14, r22				; copy gy0 into dy
	sub		r14, r18				; subtract (without carry) gy1 from dy (contains gy0)

	; if (dx < dy)
	cp		r13, r14
	brge	bresh_q4_shallow		; (not_true) -> ELSE
	rjmp	bresh_q4_steep

bresh_q4_shallow:
	; err = dx >> 1
	mov		r15, r13				; copy dx into err
	asr		r15						; dx >> 1

bresh_q4_shallow_loop:
	; err = err - dy
	sub		r15, r14
	; gx0++
	inc		r24

	; if (err < 0)
	sbrs	r15, 7						; if bit7 is 1 the result is negative
	rjmp	bresh_q4_shallow_no_minor	; (not_true) -> continue

bresh_q4_shallow_minor:
	; err = err + dx
	add		r15, r13
	; gy0--
	dec		r22
	; SetPixelFastC(gx0, gy0)
	fast_line_XP_YM
	; while (gx0 != gx1)
	cp		r24, r20
	breq	bresh_q4_shallow_exit
	rjmp	bresh_q4_shallow_loop

bresh_q4_shallow_no_minor:
	; SetPixelFastC(gx0, gy0)
	fast_line_X_plus
	; while (gx0 != gx1)
	cp		r24, r20
	breq	bresh_q4_shallow_exit
	rjmp	bresh_q4_shallow_loop
bresh_q4_shallow_exit:
	fast_line_exit
	ret


bresh_q4_steep:
	; err = dy >> 1
	mov		r15, r14				; copy dy into err
	asr		r15						; dy >> 1

bresh_q4_steep_loop:
	; err = err - dx
	sub		r15, r13
	; gy0--
	dec		r22

	; if (err < 0)
	sbrs	r15, 7					; if bit7 is 1 the result is negative
	rjmp	bresh_q4_steep_no_minor	; (not_true) -> continue

bresh_q4_steep_minor:
	; err = err + dy
	add		r15, r14
	; gx0++
	inc		r24
	; SetPixelFastC(gx0, gy0)
	fast_line_XP_YM
	; while (gy0 != gy1)
	cp		r22, r18
	breq	bresh_q4_steep_exit
	rjmp	bresh_q4_steep_loop


bresh_q4_steep_no_minor:
	; SetPixelFastC(gx0, gy0)
	fast_line_Y_minus
	; while (gy0 != gy1)
	cp		r22, r18
	breq	bresh_q4_steep_exit
	rjmp	bresh_q4_steep_loop
bresh_q4_steep_exit:
	fast_line_exit
	ret

; bresh_line_hplus_asm_loop:
; bresh_line_hminus_asm_loop:
; bresh_line_vplus_asm_loop:
; bresh_line_vminus_asm_loop:
;
; Four routines to cover the four special cases of lines with
; M = 0 or infinity

bresh_line_hplus_asm_loop:
	inc		r24
	fast_line_X_plus
bresh_line_hplus_asm:
	cp		r20, r24
	brne	bresh_line_hplus_asm_loop
	fast_line_exit
	ret

bresh_line_hminus_asm_loop:
	dec		r24
	fast_line_X_minus
bresh_line_hminus_asm:
	cp		r20, r24
	brne	bresh_line_hminus_asm_loop
	fast_line_exit
	ret

bresh_line_vplus_asm_loop:
	inc		r22
	fast_line_Y_plus
bresh_line_vplus_asm:
	cp		r18, r22
	brne	bresh_line_vplus_asm_loop
	fast_line_exit
	ret

bresh_line_vminus_asm_loop:
	dec		r22
	fast_line_Y_minus
bresh_line_vminus_asm:
	cp		r18, r22
	brne	bresh_line_vminus_asm_loop
	fast_line_exit
	ret

drawob_sin_cos_mul_XY0_nm:
	drawob_sin_cos_mul_XY0z
	ret
drawob_sin_cos_mul_XY1_nm:
	drawob_sin_cos_mul_XY1z
	ret

; void bresh_line_asm(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
;
; Bresenham Line draw Algo
;
; Does housekeeping and sets up the registers for the "FAST" routines
;
; Tests for NINE seperate cases of the line by comparing X0, X1, Y0 and Y1
;
; Case 1: Horizontal + Line
; Case 2: Horizontal - Line
; Case 3: Vertical  + Line
; Case 4: Vertical  - Line
; Case 5: Quadrant 1 (H+V+)
; Case 6: Quadrant 2 (H-V+)
; Case 7: Quadrant 3 (H-V-)
; Case 8: Quadrant 4 (H+V-)
; Case 9: Single Pixel (Zero Length Line)
;
; Does the closing housekeeping and returns to the C calling program
;

set_pixel_render_entry_point:
	fast_line_convert_x0_y0_into_VRAM_address
	fast_line_get_pixel_mask
	fast_line_get_ramTileNo
	fast_line_convert_ramTileNo_into_ramTileByteAddress
	fast_line_get_ramTilePixelByte
	fast_line_OR_pixel_mask
	fast_line_write_ramTilePixelByte
	ret

bresh_line_C:
	fast_line_entry_C
	rcall	bresh_line_asm
	fast_line_exit_C
	ret

bresh_line_asm:
	fast_line_entry

bresh_line_render_entry_point:

	fast_line_convert_x0_y0_into_VRAM_address
	fast_line_get_pixel_mask
	fast_line_get_ramTileNo
	fast_line_convert_ramTileNo_into_ramTileByteAddress
	fast_line_get_ramTilePixelByte
	fast_line_OR_pixel_mask
	fast_line_write_ramTilePixelByte


	cp		r18, r22				; compare Y1 and Y0
	breq	bresh_h_line			; if Y1 = Y0 are equal we can only be a hoizontal line
	brlo	v_minus_q3_q4			; if Y1 < Y0 we can only be V-, Quadrant 3 or Quadrant 4
									; if Y1 > Y0 we can only be V+, Q1 or Q2
v_plus_q1_q2:
	cp		r20, r24				; comapre X1 and X0
	breq	v_plus					; If X1 = X0 (and Y1 > Y0 from previous) we must be a V+
	brlo	q2						; if X1 < X0 (and Y1 > Y0 from previous) we must be Q2
									; if X1 > X0 (and Y1 > Y0 from previous) we must be Q1
q1:
	rjmp	bresh_q1_asm
q2:
	rjmp	bresh_q2_asm
v_plus:
	rjmp	bresh_line_vplus_asm


v_minus_q3_q4:
	cp		r20, r24				; comapre X1 and X0
	breq	v_minus					; If X1 = X0 (and Y1 < Y0 from previous) we must be a V-
	brlo	q3						; if X1 < X0 (and Y1 < Y0 from previous) we must be Q3
									; if X1 > X0 (and Y1 < Y0 from previous) we must be Q4

q4:
	rjmp	bresh_q4_asm
q3:
	rjmp	bresh_q3_asm
v_minus:
	rjmp	bresh_line_vminus_asm



bresh_h_line:
	cp		r20, r24				; comapre X1 and X0
	breq	bresh_pixel				; If X1 = X0 (and Y1 = Y0 from previous) we must be a single pixel
	brlo	h_minus					; if X1 < X0 (and Y1 = Y0 from previous) we must be H-
									; if X1 > X0 (and Y1 = Y0 from previous) we must be H+

h_plus:
	rjmp	bresh_line_hplus_asm
h_minus:
	rjmp	bresh_line_hminus_asm

bresh_pixel:

bresh_pixel_fail:		; Do not use the normal "fast_line"exit" routine
						; when failing as we dont want to write the local
						; pixel data to a random unknown location.

	fast_line_exit_fail ; Restore the stack and call saved registers

	ret












MulSU:
	mov		r23, r24			; MULSU can not use R24 so move it to R23
	mulsu	r22, r23
	mov		r24, r1
	clr		r1
	ret



; int8_t CosMulFastC(uint8_t angle, uint8_t distance)
;
; Returns the Cosine of the angle multiplied by the distance/2
;
; Inputs
;		Angle (0..255) in r24
;		Distance (0..127) in r22
; Returns
;		cos(angle) * distabce as signed 8 bit value in r24
; Trashes
;		R0:1
;		R23
;		R24
;		R26:27


CosMulFastC:

	subi	r24, (-(64))			; COS is 90 degrees out of phase with SIN

SinMulFastC:

	mov		r30, r24				; Get the offset in the SIN table
    ldi 	r31, hi8(SinCosTable)
	lpm		r23, Z					; Read value from table into r24

	mulsu	r23, r22				; Multiply signed "sin(angle)" by unsigned "Distance*2"
	mov		r24, r1					; move signed 8 bit result into r24

	clr		r1						; restore "R1:<ZERO>" for C
	ret

; int8_t CosFastC(uint8_t angle)
;
; Returns the cosine of the angle
;
; Inputs
;		Angle (0..255) in r24
; Returns
;		cos(angle) as signed 8 bit value in r24
; Trashes
;		R24
;		R26:27

CosFastC:

	subi	r24, (-(64))			; COS is 90 degrees out of phase with SIN

SinFastC:

	mov		r30, r24				; Get the offset in the SIN table
    ldi 	r31, hi8(SinCosTable)
	lpm		r24, Z					; Read value from table into r24
	ret











drawflipper:
; ****************************************
;	ldd		r24, y+3	; load X
;	ldd		r22, y+4	; load Y
;	drawob_load_colour COLOUR_RED	; Load COLOUR_RED into "ColourMask"
;	rcall	set_pixel_render_entry_point
;	drawob_exit
;*****************************************

	drawob_entry
	drawob_load_colour COLOUR_RED	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		50
	drawob_add_angle 	 	10
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		128
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_add_angle 		235
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		128
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawmutflipper:

	drawob_entry
	drawob_load_colour COLOUR_RED	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		51
	drawob_add_angle 	 	118
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		29
	drawob_add_angle 		244
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		128
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		51
	drawob_add_angle 		12
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_add_angle 	 	148
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		29
	drawob_add_angle 		12
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		128
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		51
	drawob_add_angle 		244
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawbullet:
	ldd		r24, y+3	; load X
	ldd		r22, y+4	; load Y
	drawob_load_colour COLOUR_GREEN	; Load COLOUR_RED into "ColourMask"
	rjmp	set_pixel_render_entry_point

drawlaser:
	drawob_entry

	drawob_load_colour COLOUR_RED_YEL	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		30
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		64
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		64
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		64
	drawob_sin_cos_mul_XY0
	rjmp	set_pixel_render_entry_point

drawzap:
	drawob_entry
	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_RED into "ColourMask"

	drawob_add_angle 		24
	drawob_load_dist 		16
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		30
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_add_angle 		85
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		16
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_add_angle 		85
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		30
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawspiker:
	drawob_entry
	drawob_load_colour COLOUR_GREEN	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY0

	drawob_load_dist 		44
	drawob_add_angle 		50
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		38
	drawob_add_angle 		50
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		32
	drawob_add_angle 		50
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		26
	drawob_add_angle 		50
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		20
	drawob_add_angle 		50
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		14
	drawob_add_angle 		50
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		8
	drawob_add_angle 		50
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawspike:
	ldd		r24, y+3	; load X
	ldd		r22, y+4	; load Y
	ldd		r20, y+5	; load X1
	ldd		r18, y+6	; load Y1
	drawob_load_colour COLOUR_GREEN	; Load COLOUR_RED into "ColourMask"
	rjmp	bresh_line_render_entry_point

drawtanker:
	drawob_entry
	drawob_load_colour COLOUR_RED	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY1
	drawob_add_angle		64
	drawob_sin_cos_mul_XY0
	drawob_save_XY0
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		14
	drawob_add_angle		10
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_restore_XY1

	drawob_load_dist 		50
	drawob_add_angle		54
	drawob_sin_cos_mul_XY0
	drawob_save_XY0
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		14
	drawob_add_angle		10
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_restore_XY1

	drawob_load_dist 		50
	drawob_add_angle		54
	drawob_sin_cos_mul_XY0
	drawob_save_XY0
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		14
	drawob_add_angle		10
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_restore_XY1

	drawob_load_dist 		50
	drawob_add_angle		54
	drawob_sin_cos_mul_XY0
	drawob_save_XY0
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		14
	drawob_add_angle		10
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawfuseball:
	drawob_entry
	drawob_load_colour COLOUR_RED	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		20
	drawob_load_dist 		24
	drawob_sin_cos_mul_XY1
	drawob_save_XY1
	rcall	bresh_line_render_entry_point
	drawob_restore_XY0
	drawob_add_angle 		128
	drawob_sin_cos_mul_XY1
	drawob_save_XY1
	rcall	bresh_line_render_entry_point
	drawob_restore_XY0
	drawob_add_angle 		235
	drawob_load_dist 		50
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_colour COLOUR_GREEN	; Load COLOUR_RED into "ColourMask"

	drawob_add_angle 		42
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		20
	drawob_load_dist 		24
	drawob_sin_cos_mul_XY1
	drawob_save_XY1
	rcall	bresh_line_render_entry_point
	drawob_restore_XY0
	drawob_add_angle 		128
	drawob_sin_cos_mul_XY1
	drawob_save_XY1
	rcall	bresh_line_render_entry_point
	drawob_restore_XY0
	drawob_add_angle 		235
	drawob_load_dist 		50
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_RED into "ColourMask"

	drawob_add_angle 		42
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		20
	drawob_load_dist 		24
	drawob_sin_cos_mul_XY1
	drawob_save_XY1
	rcall	bresh_line_render_entry_point
	drawob_restore_XY0
	drawob_add_angle 		128
	drawob_sin_cos_mul_XY1
	drawob_save_XY1
	rcall	bresh_line_render_entry_point
	drawob_restore_XY0
	drawob_add_angle 		235
	drawob_load_dist 		50
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawmirror:
	drawob_entry
	drawob_load_colour COLOUR_GREEN	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		64
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		64
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		64
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		64
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawpowerup:
	drawob_entry

	drawob_load_colour COLOUR_GREEN	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		40
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		60
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_add_angle 		51
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point
	drawob_load_dist 		40
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_add_angle 		51
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point
	drawob_load_dist 		60
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_add_angle 		51
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point
	drawob_load_dist 		40
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_add_angle 		51
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point
	drawob_load_dist 		60
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawpowerupb:
	drawob_entry

	drawob_load_colour COLOUR_GREEN	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		40
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		51
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		51
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		51
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		51
	drawob_sin_cos_mul_XY0
	rjmp	set_pixel_render_entry_point

drawpulsar:
	ldd		r9,  y+7					; get the "animation" byte to work out which frame to draw

	sbrc	r9, 7
	rjmp	drawpulsar34
drawpulsar12:
	sbrc	r9, 6
	rjmp	drawpulsar2
	rjmp	drawpulsar1

drawpulsar34:
	sbrc	r9, 6
	rjmp	drawpulsar4
drawpulsar3:

	drawob_entry
	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		34
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		31
	drawob_add_angle 		23
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		27
	drawob_add_angle 		194
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		28
	drawob_add_angle 		103
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		27
	drawob_add_angle 		103
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		31
	drawob_add_angle 		194
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		34
	drawob_add_angle 		23
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawpulsar4:
	sbrc	r9, 0
	rjmp	drawpulsar4cont
	ret
drawpulsar4cont:

	drawob_entry
	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		45
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		39
	drawob_add_angle 		18
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		30
	drawob_add_angle 		205
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		28
	drawob_add_angle 		97
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		30
	drawob_add_angle 		97
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		39
	drawob_add_angle 		205
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		45
	drawob_add_angle 		18
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawpulsar2:

	drawob_entry
	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		25
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		26
	drawob_add_angle 		29
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		25
	drawob_add_angle 		183
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		28
	drawob_add_angle 		108
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		25
	drawob_add_angle 		108
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		26
	drawob_add_angle 		183
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		25
	drawob_add_angle 		29
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawpulsar1:

	drawob_entry
	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		17
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		22
	drawob_add_angle 		37
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		23
	drawob_add_angle 		169
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		28
	drawob_add_angle 		114
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		23
	drawob_add_angle 		114
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		22
	drawob_add_angle 		169
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		17
	drawob_add_angle 		37
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawdemona:
	drawob_entry
	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		38
	drawob_add_angle 		238
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		36
	drawob_add_angle 		42
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		40
	drawob_add_angle 		21
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		40
	drawob_add_angle 		38
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		36
	drawob_add_angle 		21
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		38
	drawob_add_angle 		42
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		41
	drawob_add_angle 		23
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		41
	drawob_add_angle 		46
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		38
	drawob_add_angle 		23
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_colour COLOUR_GREEN	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		33
	drawob_add_angle 		249
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		11
	drawob_add_angle 		37
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		26
	drawob_add_angle 		194
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		30
	drawob_add_angle 		25
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point


	drawob_load_dist 		30
	drawob_add_angle 		184
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		11
	drawob_add_angle 		219
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		26
	drawob_add_angle 		62
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		30
	drawob_add_angle 		231
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawdemonb:
	drawob_entry
	drawob_load_colour COLOUR_RED	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		23
	drawob_add_angle 		42
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		56
	drawob_add_angle 		13
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		35
	drawob_add_angle 		9
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		56
	drawob_add_angle 		9
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		23
	drawob_add_angle 		13
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		23
	drawob_add_angle 		212
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		41
	drawob_add_angle 		169
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		38
	drawob_add_angle 		23
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		64
	drawob_add_angle 		247
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		86
	drawob_add_angle 		244
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		59
	drawob_add_angle 		4
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		41
	drawob_add_angle 		250
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		41
	drawob_add_angle 		210
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		38
	drawob_add_angle 		233
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		64
	drawob_add_angle 		9
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		86
	drawob_add_angle 		12
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		59
	drawob_add_angle 		252
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		41
	drawob_add_angle 		6
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawexplosion:

	ldi		r25, 0xFF

	lds		r17, actionTimerSubCount
	sbrs	r17, 0
	andi	r25, 0xAA
	sbrs	r17, 1
	andi	r25, 0x55

	drawob_entry

	drawob_load_dist 		127
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		36
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		36
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		36
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		36
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		36
	drawob_sin_cos_mul_XY0
	rcall	set_pixel_render_entry_point

	drawob_add_angle 		36
	drawob_sin_cos_mul_XY0
	rjmp	set_pixel_render_entry_point

drawyesyesyes:
	drawob_entry
	drawob_load_colour COLOUR_YELLOW

	lsl		r9
	lsl		r9
	lsl		r9

	drawob_load_dist 		110
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		75
	drawob_add_angle 		245
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		65
	drawob_add_angle 		22
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		75
	drawob_add_angle 		234
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		85
	drawob_add_angle 		233
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		45
	drawob_add_angle 		105
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		210
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		176
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		206
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		25
	drawob_add_angle 		97
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		10
	drawob_add_angle 		101
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		110
	drawob_add_angle 		3
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		65
	drawob_add_angle 		245
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		50
	drawob_add_angle 		23
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		100
	drawob_add_angle 		2
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		106
	drawob_add_angle 		16
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		60
	drawob_add_angle 		11
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawoneup:
	drawob_entry
	drawob_load_colour COLOUR_YELLOW

	drawob_load_dist 		89
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		90
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		82
	drawob_add_angle 		9
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		148
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		80
	drawob_add_angle 		243
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		134
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		82
	drawob_add_angle 		115
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		148
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		93
	drawob_add_angle 		12
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		50
	drawob_add_angle 		28
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		26
	drawob_add_angle 		242
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawtestmarker:
	ldd		r24, y+3	; load X
	ldd		r22, y+4	; load Y
	drawob_load_colour COLOUR_GREEN	; Load COLOUR_RED into "ColourMask"
	rjmp	set_pixel_render_entry_point

drawtestline:
	ldd		r24, y+3					; load X
	ldd		r22, y+4					; load Y
	ldd		r20, y+5					; load X1
	ldd		r18, y+6					; load Y1
	drawob_load_colour COLOUR_GREEN		; Load COLOUR_GREEN into "ColourMask"
	rjmp	bresh_line_render_entry_point

drawsuperzap:
	drawob_entry
	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_YELLOW into "ColourMask"

	drawob_load_dist 		50
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		128
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawclawdeath:
	drawob_entry
	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		25
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		125
	drawob_add_angle 		5
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		240
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		25
	drawob_add_angle 		8
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		25
	drawob_add_angle 		64
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		125
	drawob_add_angle 		5
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		240
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		25
	drawob_add_angle 		8
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		25
	drawob_add_angle 		64
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		125
	drawob_add_angle 		5
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		240
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		25
	drawob_add_angle 		8
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		25
	drawob_add_angle 		64
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		125
	drawob_add_angle 		5
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_add_angle 		240
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		25
	drawob_add_angle 		8
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawdroid:
	drawob_entry

	push	r21				; save the angle

	drawob_load_colour COLOUR_GREEN	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		44
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		56
	drawob_add_angle 		13
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		44
	drawob_add_angle 		13
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		44
	drawob_add_angle 		38
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		56
	drawob_add_angle 		13
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		44
	drawob_add_angle 		13
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		44
	drawob_add_angle 		38
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		56
	drawob_add_angle 		13
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		44
	drawob_add_angle 		13
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_load_dist 		44
	drawob_add_angle 		38
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		56
	drawob_add_angle 		13
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		44
	drawob_add_angle 		13
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	pop		r21							; restore the pre-molested angle
	com		r21							; compliment the angle so the inside square spins oppostie the outside box

	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		20
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		128
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point

	drawob_add_angle 		64
	drawob_sin_cos_mul_XY0
	drawob_add_angle 		128
	drawob_sin_cos_mul_XY1
	rjmp	bresh_line_render_entry_point

drawblank:
	ret

drawclaw:
	drawob_entry
	drawob_load_colour COLOUR_YELLOW	; Load COLOUR_RED into "ColourMask"

	drawob_load_dist 		40
	drawob_add_angle 		226
	drawob_sin_cos_mul_XY0
	drawob_load_dist 		50
	drawob_add_angle 		24
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		30
	drawob_add_angle 		70
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		50
	drawob_add_angle 		70
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	drawob_load_dist 		40
	drawob_add_angle 		24
	drawob_sin_cos_mul_XY1
	rcall	bresh_line_render_entry_point
	ret

/*
void renderObjects(void){
	uint8_t i;
	ObjectDescStruct *Current;

	drawFunctionPointer_t drawFunction;

	for(i = 0; i < MAX_OBJS; i++) {
		Current = (ObjectDescStruct*)&ObjectStore[i];
		if(Current->obType != OBJ_EMPTY)
		{
			drawFunction = (drawFunctionPointer_t)pgm_read_word(&drawFunctionPointers[Current->obType]);
			drawFunction(Current);
		}
	}
}
*/

renderObjects:
	fast_line_entry_C							; save all the registers C does not want trashed
	fast_line_entry								; set up all the registers for the draw line routines

	ldi		r28, lo8(ObjectStore)				; Get the base address of the ObjectStore[] array
	ldi		r29, hi8(ObjectStore)
renderObjectsLoop:
	ld		r30, Y								; Get the Object Type of the current object (r30 = ObjectStore[i].ObjType)
	cpi		r30, 0x00							; If the object type is <zero>
	breq	renderObjectsSkip					; we don't need to draw anything
	add		r30, r30							; Multiply the object number by 2 (flash addressing)
	clr		r31									; and clear ZH (NB: Object Type can not be > 128 for this to work)
	subi	r30, lo8(-(drawFunctionPointers))	; Add the base address to the function pointer table in PROGMEM
	sbci	r31, hi8(-(drawFunctionPointers))
	lpm		r26, Z+								; Get the address of the code/routine that draws the given
	lpm		r27, Z+								; object type into X
	movw	r30, r26							; move that value into Z ready for the ICALL
	icall										; Call the routine that draws the object

renderObjectsSkip:
	adiw	r28, 0x08							; add 8 to Y to point to the next element of ObjectStore[]
	cpi		r29, 0x04							; if the address of Y hits 0x0400 we are at the end of the object store
	brne	renderObjectsLoop					; else loop back

	fast_line_exit_C							; restore the registers C needs to not be trashed
	ret


; return( (((web << 12) + ((lane & 0b11111100) << 6) + zoom) * 13) + (13 * 480 * 2) + StartSector );

.global getSectorBMP
getSectorBMP:							; R21                     r20                     R19                     R18
	lds   r18, zoom						; X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  Z7 Z6 Z5 Z4 Z3 Z2 Z1 Z0
	lds   r19, lane						; X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  L7 L6 L5 L4 L3 L2 L1 L0 Z7 Z6 Z5 Z4 Z3 Z2 Z1 Z0
	lds   r20, level					; X  X  X  X  X  X  X  X  W7 W6 W5 W4 W3 W2 W1 W0 L7 L6 L5 L4 L3 L2 L1 L0 Z7 Z6 Z5 Z4 Z3 Z2 Z1 Z0

	bst   r19, 2                        ;                                                                            **
	bld   r18, 6						; X  X  X  X  X  X  X  X  W7 W6 W5 W4 W3 W2 W1 W0 L7 L6 L5 L4 L3 L2 L1 L0 Z7 L2 Z5 Z4 Z3 Z2 Z1 Z0
	bst   r19, 3                        ;                                                                         **
	bld   r18, 7						; X  X  X  X  X  X  X  X  W7 W6 W5 W4 W3 W2 W1 W0 L7 L6 L5 L4 L3 L2 L1 L0 L3 L2 Z5 Z4 Z3 Z2 Z1 Z0

	andi  r19, 0xF0						; X  X  X  X  X  X  X  X  W7 W6 W5 W4 W3 W2 W1 W0 L7 L6 L5 L4 0  0  0  0  L3 L2 Z5 Z4 Z3 Z2 Z1 Z0
	andi  r20, 0x0F						; X  X  X  X  X  X  X  X  0  0  0  0  W3 W2 W1 W0 L7 L6 L5 L4 0  0  0  0  L3 L2 Z5 Z4 Z3 Z2 Z1 Z0
	or    r19, r20						; X  X  X  X  X  X  X  X  0  0  0  0  W3 W2 W1 W0 L7 L6 L5 L4 W3 W2 W1 W0 L3 L2 Z5 Z4 Z3 Z2 Z1 Z0
	swap  r19							; X  X  X  X  X  X  X  X  0  0  0  0  W3 W2 W1 W0 W3 W2 W1 W0 L7 L6 L5 L4 L3 L2 Z5 Z4 Z3 Z2 Z1 Z0

	clr   r20							; X  X  X  X  X  X  X  X  0  0  0  0  0  0  0  0  W3 W2 W1 W0 L7 L6 L5 L4 L3 L2 Z5 Z4 Z3 Z2 Z1 Z0
	clr   r21							; 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  W3 W2 W1 W0 L7 L6 L5 L4 L3 L2 Z5 Z4 Z3 Z2 Z1 Z0

	movw   r22, r18						; Copy the 32 bits to R22:23:24:25 (32 bit returned result for GCC)
	movw   r24, r20

	lsl    r18							; Left shift the 32 bit number 2 times = x4 (only 3 bytes are significant)
	rol    r19
	rol    r20

	lsl    r18
	rol    r19
	rol    r20

	add    r22, r18						; add x4 and x1 together to get x5
	adc    r23, r19
	adc    r24, r20

	lsl    r18							; left shift the x4 again to turn it into x8
	rol    r19
	rol    r20

	add    r22, r18						; add x5 and x8 together to get x13
	adc    r23, r19
	adc    r24, r20

	subi   r22,  lo8(-0x30C0)			; Add the length of the intro movies (13 sectors per frame * 480 frames * 2 movies = 0x30C0)
	sbci   r23,  hi8(-0x30C0)
	sbci   r24, hlo8(-0x30C0)

	lds    r18, sectorStart+0			; get the start sector of the movie on the SD card
	lds    r19, sectorStart+1

	add    r22, r18						; add the start sector to the return result
	adc    r23, r19
	adc    r24, r1

	ret

