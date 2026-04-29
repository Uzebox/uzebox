/*
 * videoMode23.s
 *
 * Video Mode 23: Arduboy-native vertical work buffer + horizontal scanout
 *
 * 128x64, 1bpp
 *
 * Double buffer model:
 *	sBuffer[1024]		(C draw buffer, native Arduboy vertical page layout)
 *	front_buffer[1024]	(displayed buffer, horizontal row-major)
 *
 * Work-buffer layout:
 *	byte index = x + ((y >> 3) * 128)
 *	bit mask   = 1 << (y & 7)
 *
 * Display():
 *	converts sBuffer to the horizontal front buffer once per frame
 *
 * Color semantics (Arduboy2-style):
 *	0 = BLACK  (clear)
 *	1 = WHITE  (set)
 *	2 = INVERT (toggle)
 *
 * Text:
 *	- ArduboySetTextColor(u8) and ArduboySetTextBackground(u8) supported.
 *	- If textColor == textBackground, background is treated as transparent.
 *	- ArduboyClear() resets cursor to (0,0).
 *	- ArduboyWrite(u8 ch) supported (like Print::write).
 *
 * Assets:
 *	- Put font + any other PROGMEM gfx blobs in gfx.inc.
 *	  This file must define: font6x8_h:
 */

.global InitializeVideoMode
.global DisplayLogo
.global VideoModeVsync
.global sub_video_mode_23

.global sBuffer

.global Display
.global ArduboyDisplay

.global ClearVram
.global ArduboyClear

.global ClearDisplayBuffer
.global SetPalette
.global SetInvert

.global ArduboySetCursor
.global ArduboyGetCursorX
.global ArduboyGetCursorY

.global ArduboySetTextColor
.global ArduboySetTextBackground
.global ArduboyWrite

.global DrawPixel
.global ArduboyDrawPixel
.global DrawFastHLine
.global ArduboyDrawFastHLine
.global DrawFastVLine
.global ArduboyDrawFastVLine
.global DrawBitmap
.global ArduboyDrawBitmap
.global DrawOverwriteRaw
.global SpritesDrawOverwrite
.global SpritesDrawPlusMask

.global ArduboyFillRect
.global ArduboyDrawRect
.global ArduboyDrawLine
.global ArduboyDrawCircle
.global ArduboyFillCircle

.global ArduboyPrint
.global ArduboyPrintU8
.global ArduboyPrintU16

.global __do_copy_data
.global __do_clear_bss

#ifndef VM23_STRIDE
	#define VM23_STRIDE 16
#endif

#ifndef VM23_ROWS
	#define VM23_ROWS 64
#endif

#ifndef VM23_LINE_REPEAT
	#define VM23_LINE_REPEAT 2
#endif

#if VM23_LINE_REPEAT == 2
	#define VM23_VISIBLE_LINES 128
#elif VM23_LINE_REPEAT == 3
	#define VM23_VISIBLE_LINES 192
#else
	#error "VM23_LINE_REPEAT must be 2 or 3"
#endif

.section .bss
sBuffer:
	.space 1024

front_buffer:
	.space 1024

/* one-byte lookahead pad for scanout from front_buffer */
vm23_front_pad:
	.space 1

arduboy_cursor_x:
	.space 2
arduboy_cursor_y:
	.space 2

.section .data
fg_color:		.byte 0xff
bg_color:		.byte 0x00
invert_flag:	.byte 0x00

text_color:		.byte 0x01		; default WHITE
text_bg:		.byte 0x00		; default BLACK

.section .text

; ---------------------------------------------------------
; Main video loop (Mode 12-style shell)
; Y = current row pointer in front_buffer
; Each logical row repeated VM23_LINE_REPEAT times
; Total scanlines emitted = VM23_VISIBLE_LINES
; ---------------------------------------------------------
sub_video_mode_23:
	clr r1
	ldi YL,lo8(front_buffer)
	ldi YH,hi8(front_buffer)

	WAIT r16, 1345   ;1350 - 15 - 0

	clr r20				; rendered scanlines counter

	ldi r21,VM23_LINE_REPEAT	; current repeat countdown
	mov r22,r21			; reload value

	ldi r23,VM23_VISIBLE_LINES	; total scanlines to emit
	ldi r24,VM23_STRIDE		; bytes per row (16)

vm23_next_scan_line:
	rcall hsync_pulse

	WAIT r19,282 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT - 3 + 3

	rcall render_row

	WAIT r19,68 - CENTER_ADJUSTMENT + 0

	; repeat each row VM23_LINE_REPEAT times
	clt
	cpi r21,1
	breq .+2
	set
	brts .+2
	add YL,r24
	brts .+2
	adc YH,r1

	dec r21
	brne .+2
	mov r21,r22

	inc r20
	cp r20,r23
	brne vm23_next_scan_line

	nop
	rcall hsync_pulse

	; set vsync flag & flip field
	lds ZL,sync_flags
	ldi r20,SYNC_FLAG_FIELD
	ori ZL,SYNC_FLAG_VSYNC
	eor ZL,r20
	sts sync_flags,ZL

	; clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	clr r1
	ret


; ---------------------------------------------------------
; render_row
; Mode 12 128-wide cadence, but reads horizontal row-major bytes.
;
; Input:
;	Y = row base in front_buffer (must not be modified)
; ---------------------------------------------------------
render_row:
	movw r2,YL				; save row base
	ldi r18,VM23_STRIDE		; 16 bytes

	lds r14,fg_color
	lds r15,bg_color

	; 6-cycle filler so setup matches Mode 12 submode1 timing
	lpm
	lpm

	ld r16,Y+				; preload first byte

vm23_main_loop_128:
	; pixel 0
	mov r0,r14
	sbrs r16,7
	mov r0,r15
	out _SFR_IO_ADDR(DATA_PORT),r0
	lpm
	lpm
	nop

	; pixel 1
	mov r0,r14
	sbrs r16,6
	mov r0,r15
	out _SFR_IO_ADDR(DATA_PORT),r0
	lpm
	lpm
	nop

	; pixel 2
	mov r0,r14
	sbrs r16,5
	mov r0,r15
	out _SFR_IO_ADDR(DATA_PORT),r0
	lpm
	lpm
	nop

	; pixel 3
	mov r0,r14
	sbrs r16,4
	mov r0,r15
	out _SFR_IO_ADDR(DATA_PORT),r0
	lpm
	lpm
	nop

	; pixel 4
	mov r0,r14
	sbrs r16,3
	mov r0,r15
	out _SFR_IO_ADDR(DATA_PORT),r0
	lpm
	lpm
	nop

	; pixel 5
	mov r0,r14
	sbrs r16,2
	mov r0,r15
	out _SFR_IO_ADDR(DATA_PORT),r0
	lpm
	lpm
	nop

	; pixel 6
	mov r0,r14
	sbrs r16,1
	mov r0,r15
	out _SFR_IO_ADDR(DATA_PORT),r0
	lpm
	lpm
	nop

	; pixel 7
	mov r0,r14
	sbrs r16,0
	mov r0,r15
	out _SFR_IO_ADDR(DATA_PORT),r0
	rjmp .					; 2-cycle filler (Mode 12 idiom)

	ld r16,Y+				; preload next byte
	dec r18
	brne vm23_main_loop_128

	; IMPORTANT: must be lpm, not nop
	lpm

	clr r0
	out _SFR_IO_ADDR(DATA_PORT),r0

	movw YL,r2				; restore row base
	ret


; ---------------------------------------------------------
; Display() / ArduboyDisplay()
; Convert vertical Arduboy work buffer -> horizontal front buffer
; ---------------------------------------------------------
.section .text.Display
Display:
ArduboyDisplay:
	ldi r24,lo8(sBuffer)
	ldi r25,hi8(sBuffer)
	ldi ZL,lo8(front_buffer)
	ldi ZH,hi8(front_buffer)

	ldi r18,64				; logical rows
	ldi r19,1				; bit mask within current Arduboy page

vm23_display_row_loop:
	movw XL,r24				; current page base in sBuffer
	ldi r20,16				; 16 output bytes per row

vm23_display_byte_loop:
	clr r21					; output byte
	ldi r22,8				; 8 source columns per output byte

vm23_display_bit_loop:
	ld r16,X+
	mov r17,r16
	and r17,r19
	lsl r21
	tst r17
	breq vm23_display_bit_skip
	ori r21,1
vm23_display_bit_skip:
	dec r22
	brne vm23_display_bit_loop

	st Z+,r21
	dec r20
	brne vm23_display_byte_loop

	lsl r19
	brcc vm23_display_same_page
	ldi r19,1
	subi r24,lo8(-(128))
	sbci r25,hi8(-(128))
vm23_display_same_page:
	dec r18
	brne vm23_display_row_loop
	ret


; ---------------------------------------------------------
; ClearVram() / ArduboyClear()
; Clear draw buffer sBuffer + reset cursor to (0,0)
; ---------------------------------------------------------
.section .text.ClearVram
ClearVram:
ArduboyClear:
	clr r1
	ldi ZL,lo8(sBuffer)
	ldi ZH,hi8(sBuffer)

	ldi r24,128

vm23_clear_vram_loop:
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	dec r24
	brne vm23_clear_vram_loop

	; cursor = 0,0
	sts arduboy_cursor_x,   r1
	sts arduboy_cursor_x+1, r1
	sts arduboy_cursor_y,   r1
	sts arduboy_cursor_y+1, r1
	ret


; ---------------------------------------------------------
; ClearDisplayBuffer()
; Clear internal front buffer only
; ---------------------------------------------------------
.section .text.ClearDisplayBuffer
ClearDisplayBuffer:
	clr r1
	ldi ZL,lo8(front_buffer)
	ldi ZH,hi8(front_buffer)

	ldi r24,128

vm23_clear_front_loop:
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	dec r24
	brne vm23_clear_front_loop
	ret


; ---------------------------------------------------------
; SetPalette(fg, bg)
; r24=fg, r22=bg
; ---------------------------------------------------------
.section .text.SetPalette
SetPalette:
	sts fg_color,r24
	sts bg_color,r22
	ret


; ---------------------------------------------------------
; SetInvert(enabled)
; r24=0 off, nonzero on
; ---------------------------------------------------------
.section .text.SetInvert
SetInvert:
	sts invert_flag,r24
	ret


; ---------------------------------------------------------
; ArduboySetCursor(x,y)
; s16 x=r25:r24, s16 y=r23:r22
; ---------------------------------------------------------
.section .text.ArduboySetCursor
ArduboySetCursor:
	sts arduboy_cursor_x,   r24
	sts arduboy_cursor_x+1, r25
	sts arduboy_cursor_y,   r22
	sts arduboy_cursor_y+1, r23
	ret


; ---------------------------------------------------------
; ArduboyGetCursorX() -> s16 in r25:r24
; ---------------------------------------------------------
.section .text.ArduboyGetCursorX
ArduboyGetCursorX:
	lds r24,arduboy_cursor_x
	lds r25,arduboy_cursor_x+1
	ret


; ---------------------------------------------------------
; ArduboyGetCursorY() -> s16 in r25:r24
; ---------------------------------------------------------
.section .text.ArduboyGetCursorY
ArduboyGetCursorY:
	lds r24,arduboy_cursor_y
	lds r25,arduboy_cursor_y+1
	ret


; ---------------------------------------------------------
; ArduboySetTextColor(color)
; r24=color (0/1/2)
; ---------------------------------------------------------
.section .text.ArduboySetTextColor
ArduboySetTextColor:
	sts text_color,r24
	ret


; ---------------------------------------------------------
; ArduboySetTextBackground(color)
; r24=color (0/1/2)
; ---------------------------------------------------------
.section .text.ArduboySetTextBackground
ArduboySetTextBackground:
	sts text_bg,r24
	ret


; ---------------------------------------------------------
; ArduboyWrite(ch)
; r24=ch
; returns 1 in r24 (like Print::write)
; ---------------------------------------------------------
.section .text.ArduboyWrite
ArduboyWrite:
	mov r20,r24
	call ArduboyPrintChar5x7
	ldi r24,1
	ret


; ---------------------------------------------------------
; DrawPixel(x,y,color) / ArduboyDrawPixel
; Native Arduboy vertical work buffer:
; 	byte index = x + ((y >> 3) * 128)
; 	bit mask   = 1 << (y & 7)
; x=r25:r24, y=r23:r22, color=r20 (0/1/2)
; ---------------------------------------------------------
.section .text.DrawPixel
DrawPixel:
ArduboyDrawPixel:
	clr r1

	; reject negative/out-of-range
	tst r25
	brne vm23_drawpixel_ret
	tst r23
	brne vm23_drawpixel_ret
	cpi r24,128
	brsh vm23_drawpixel_ret
	cpi r22,64
	brsh vm23_drawpixel_ret

	; X = &sBuffer[ x + ((y >> 3) * 128) ]
	ldi XL,lo8(sBuffer)
	ldi XH,hi8(sBuffer)

	mov r19,r22
	lsr r19
	lsr r19
	lsr r19				; page = y >> 3

	clr r18
	bst r19,0
	bld r18,7				; low add = (page & 1) << 7
	mov r0,r19
	lsr r0					; high add = page >> 1

	add XL,r18
	adc XH,r0
	add XL,r24
	adc XH,r1

	; mask = 1 << (y & 7)
	mov r18,r22
	andi r18,7
	ldi r19,1
	tst r18
	breq vm23_drawpixel_mask_ready
vm23_drawpixel_mask_loop:
	lsl r19
	dec r18
	brne vm23_drawpixel_mask_loop
vm23_drawpixel_mask_ready:

	ld r21,X

	cpi r20,2
	breq vm23_drawpixel_invert
	tst r20
	breq vm23_drawpixel_clear

	; set bit
	or r21,r19
	rjmp vm23_drawpixel_store

vm23_drawpixel_invert:
	eor r21,r19
	rjmp vm23_drawpixel_store

vm23_drawpixel_clear:
	mov r0,r19
	com r0
	and r21,r0

vm23_drawpixel_store:
	st X,r21

vm23_drawpixel_ret:
	clr r1
	ret


; ---------------------------------------------------------
; DrawFastHLine / ArduboyDrawFastHLine
; Fast in-bounds path updates contiguous Arduboy page bytes directly.
; Falls back to the old per-pixel path for clipped/negative cases.
; x=r25:r24, y=r23:r22, w=r20, color=r18
; ---------------------------------------------------------
.section .text.DrawFastHLine
DrawFastHLine:
ArduboyDrawFastHLine:
	clr r1
	tst r20
	breq vm23_hline_fast_done
	tst r25
	brne vm23_drawfasthline_slow
	tst r23
	brne vm23_drawfasthline_slow
	cpi r24,128
	brsh vm23_drawfasthline_slow
	cpi r22,64
	brsh vm23_drawfasthline_slow
	mov r19,r24
	add r19,r20
	brcs vm23_drawfasthline_slow
	cpi r19,129
	brsh vm23_drawfasthline_slow

	ldi XL,lo8(sBuffer)
	ldi XH,hi8(sBuffer)

	mov r19,r22
	lsr r19
	lsr r19
	lsr r19
	clr r21
	bst r19,0
	bld r21,7
	mov r0,r19
	lsr r0
	add XL,r21
	adc XH,r0
	add XL,r24
	adc XH,r1

	mov r21,r22
	andi r21,7
	ldi r19,1
	tst r21
	breq vm23_hline_mask_ready
vm23_hline_mask_loop:
	lsl r19
	dec r21
	brne vm23_hline_mask_loop
vm23_hline_mask_ready:
	cpi r18,2
	breq vm23_hline_fast_invert
	tst r18
	breq vm23_hline_fast_clear

vm23_hline_fast_set:
	ld r21,X
	or r21,r19
	st X+,r21
	dec r20
	brne vm23_hline_fast_set
vm23_hline_fast_done:
	clr r1
	ret

vm23_hline_fast_clear:
	mov r0,r19
	com r0
vm23_hline_fast_clear_loop:
	ld r21,X
	and r21,r0
	st X+,r21
	dec r20
	brne vm23_hline_fast_clear_loop
	clr r1
	ret

vm23_hline_fast_invert:
	ld r21,X
	eor r21,r19
	st X+,r21
	dec r20
	brne vm23_hline_fast_invert
	clr r1
	ret

vm23_drawfasthline_slow:
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	movw r12,r24
	movw r10,r22
	mov r14,r20
	mov r15,r18

	tst r14
	breq vm23_hline_slow_done

vm23_hline_slow_loop:
	movw r24,r12
	movw r22,r10
	mov r20,r15
	call DrawPixel

	inc r12
	brne vm23_hline_slow_no_carry
	inc r13
vm23_hline_slow_no_carry:
	dec r14
	brne vm23_hline_slow_loop

vm23_hline_slow_done:
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	ret

; ---------------------------------------------------------
; DrawFastVLine / ArduboyDrawFastVLine
; Fast in-bounds path updates page bytes directly.
; Falls back to the old per-pixel path for clipped/negative cases.
; x=r25:r24, y=r23:r22, h=r20, color=r18
; ---------------------------------------------------------
.section .text.DrawFastVLine
DrawFastVLine:
ArduboyDrawFastVLine:
	clr r1
	tst r20
	breq vm23_vline_fast_done
	tst r25
	brne vm23_drawfastvline_slow
	tst r23
	brne vm23_drawfastvline_slow
	cpi r24,128
	brsh vm23_drawfastvline_slow
	cpi r22,64
	brsh vm23_drawfastvline_slow
	mov r19,r22
	add r19,r20
	brcs vm23_drawfastvline_slow
	cpi r19,65
	brsh vm23_drawfastvline_slow

	ldi XL,lo8(sBuffer)
	ldi XH,hi8(sBuffer)

	mov r19,r22
	lsr r19
	lsr r19
	lsr r19
	clr r21
	bst r19,0
	bld r21,7
	mov r0,r19
	lsr r0
	add XL,r21
	adc XH,r0
	add XL,r24
	adc XH,r1

	mov r17,r20				; remaining height
	mov r19,r22
	andi r19,7				; starting bit within page

vm23_vline_fast_page_loop:
	ldi r22,8
	sub r22,r19				; bits left in current page
	mov r23,r17
	cp r23,r22
	brlo vm23_vline_fast_bits_ready
	mov r23,r22
vm23_vline_fast_bits_ready:
	mov r22,r23
	ldi r21,1
	dec r22
	breq vm23_vline_fast_lowmask_ready
vm23_vline_fast_lowmask_loop:
	lsl r21
	ori r21,1
	dec r22
	brne vm23_vline_fast_lowmask_loop
vm23_vline_fast_lowmask_ready:
	mov r22,r19
	tst r22
	breq vm23_vline_fast_mask_ready
vm23_vline_fast_shift_loop:
	lsl r21
	dec r22
	brne vm23_vline_fast_shift_loop
vm23_vline_fast_mask_ready:

	ld r24,X
	cpi r18,2
	breq vm23_vline_fast_do_invert
	tst r18
	breq vm23_vline_fast_do_clear
	or r24,r21
	rjmp vm23_vline_fast_store
vm23_vline_fast_do_invert:
	eor r24,r21
	rjmp vm23_vline_fast_store
vm23_vline_fast_do_clear:
	mov r0,r21
	com r0
	and r24,r0
vm23_vline_fast_store:
	st X,r24

	sub r17,r23
	breq vm23_vline_fast_done
	subi XL,lo8(-(128))
	sbci XH,hi8(-(128))
	clr r19
	rjmp vm23_vline_fast_page_loop

vm23_vline_fast_done:
	clr r1
	ret

vm23_drawfastvline_slow:
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	movw r12,r24
	movw r10,r22
	mov r14,r20
	mov r15,r18

	tst r14
	breq vm23_vline_slow_done

vm23_vline_slow_loop:
	movw r24,r12
	movw r22,r10
	mov r20,r15
	call DrawPixel

	inc r10
	brne vm23_vline_slow_no_y_carry
	inc r11
vm23_vline_slow_no_y_carry:
	dec r14
	brne vm23_vline_slow_loop

vm23_vline_slow_done:
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	ret

; ---------------------------------------------------------
; DrawBitmap / ArduboyDrawBitmap
; Fast path for fully in-bounds, page-aligned native Arduboy bitmaps.
; Falls back to the old per-pixel path for unaligned/clipped cases.
; Layout: width bytes per 8-pixel page, page-major.
; total bytes = w * ((h+7)>>3)
; x=r25:r24, y=r23:r22, bmp=r21:r20, w=r18, h=r16, color=r14
; bmp is PROGMEM pointer
; ---------------------------------------------------------
.section .text.DrawBitmap
DrawBitmap:
ArduboyDrawBitmap:
	clr r1
	tst r18
	breq vm23_drawbitmap_fast_done
	tst r16
	breq vm23_drawbitmap_fast_done
	tst r25
	brne vm23_drawbitmap_slow
	tst r23
	brne vm23_drawbitmap_slow
	cpi r24,128
	brsh vm23_drawbitmap_slow
	cpi r22,64
	brsh vm23_drawbitmap_slow
	mov r17,r24
	add r17,r18
	brcs vm23_drawbitmap_slow
	cpi r17,129
	brsh vm23_drawbitmap_slow
	mov r17,r22
	add r17,r16
	brcs vm23_drawbitmap_slow
	cpi r17,65
	brsh vm23_drawbitmap_slow
	mov r17,r22
	andi r17,7
	brne vm23_drawbitmap_slow

	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29

	movw r8,r20				; bitmap ptr
	movw r12,r24			; base x
	movw r10,r22			; base y
	mov r7,r18				; width
	mov r6,r16				; height
	mov r5,r14				; color

	mov r17,r16
	subi r17,lo8(-(7))
	lsr r17
	lsr r17
	lsr r17					; page count

	mov r18,r16
	andi r18,7				; last partial bits
	mov r15,r18
	breq vm23_drawbitmap_full_last
	clr r14
	inc r14
	mov r16,r15
	dec r16
	breq vm23_drawbitmap_lastmask_ready
vm23_drawbitmap_lastmask_loop:
	lsl r14
	inc r14
	dec r16
	brne vm23_drawbitmap_lastmask_loop
	rjmp vm23_drawbitmap_lastmask_ready
vm23_drawbitmap_full_last:
	clr r14
	dec r14
vm23_drawbitmap_lastmask_ready:

vm23_drawbitmap_fast_page_loop:
	ldi XL,lo8(sBuffer)
	ldi XH,hi8(sBuffer)
	mov r16,r10
	lsr r16
	lsr r16
	lsr r16
	clr r18
	bst r16,0
	bld r18,7
	mov r0,r16
	lsr r0
	add XL,r18
	adc XH,r0
	add XL,r12
	adc XH,r1

	movw ZL,r8
	mov r4,r7
	cpi r17,1
	breq vm23_drawbitmap_use_lastmask
	clr r3
	dec r3
	rjmp vm23_drawbitmap_mask_ready
vm23_drawbitmap_use_lastmask:
	mov r3,r14
vm23_drawbitmap_mask_ready:

	mov r18,r5
	cpi r18,2
	breq vm23_drawbitmap_fast_invert
	tst r5
	breq vm23_drawbitmap_fast_clear

vm23_drawbitmap_fast_set_loop:
	lpm r2,Z+
	and r2,r3
	ld r16,X
	or r16,r2
	st X+,r16
	dec r4
	brne vm23_drawbitmap_fast_set_loop
	rjmp vm23_drawbitmap_fast_next_page

vm23_drawbitmap_fast_clear:
	lpm r2,Z+
	and r2,r3
	ld r16,X
	mov r0,r2
	com r0
	and r16,r0
	st X+,r16
	dec r4
	brne vm23_drawbitmap_fast_clear
	rjmp vm23_drawbitmap_fast_next_page

vm23_drawbitmap_fast_invert:
	lpm r2,Z+
	and r2,r3
	ld r16,X
	eor r16,r2
	st X+,r16
	dec r4
	brne vm23_drawbitmap_fast_invert

vm23_drawbitmap_fast_next_page:
	add r8,r7
	adc r9,r1
	ldi r18,8
	add r10,r18
	adc r11,r1
	dec r17
	brne vm23_drawbitmap_fast_page_loop

	clr r1
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
vm23_drawbitmap_fast_done:
	ret

vm23_drawbitmap_slow:
	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29

	clr r1

	movw r12,r24			; base x
	movw r10,r22			; current page y base
	movw r8,r20				; current bitmap page ptr (PROGMEM)
	mov r7,r18				; width
	mov r15,r14				; color
	mov r14,r16				; remaining height

	tst r7
	breq vm23_drawbitmap_slow_done
	tst r14
	breq vm23_drawbitmap_slow_done

vm23_drawbitmap_slow_page_loop:
	movw ZL,r8
	movw r2,r12				; current x
	mov r5,r7				; columns remaining
	mov r6,r14				; bits in this page
	mov r16,r6
	cpi r16,8
	brlo vm23_drawbitmap_slow_bits_ready
	ldi r16,8
	mov r6,r16
vm23_drawbitmap_slow_bits_ready:

vm23_drawbitmap_slow_col_loop:
	lpm r4,Z+
	movw YL,r10				; current y
	mov r16,r6

vm23_drawbitmap_slow_bit_loop:
	sbrs r4,0
	rjmp vm23_drawbitmap_slow_skip_px
	movw r24,r2
	movw r22,YL
	mov r20,r15
	call DrawPixel
vm23_drawbitmap_slow_skip_px:
	lsr r4
	adiw YL,1
	dec r16
	brne vm23_drawbitmap_slow_bit_loop

	inc r2
	brne vm23_drawbitmap_slow_no_x_carry
	inc r3
vm23_drawbitmap_slow_no_x_carry:

	dec r5
	brne vm23_drawbitmap_slow_col_loop

	mov r16,r14
	cpi r16,9
	brlo vm23_drawbitmap_slow_done
	ldi r16,8
	sub r14,r16
	add r10,r16
	adc r11,r1
	add r8,r7
	adc r9,r1
	rjmp vm23_drawbitmap_slow_page_loop

vm23_drawbitmap_slow_done:
	clr r1
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
	ret

; ---------------------------------------------------------
; DrawOverwriteRaw
; Fast path for fully in-bounds, page-aligned native Arduboy bitmaps.
; Falls back to the old per-pixel path for unaligned/clipped cases.
; Layout: width bytes per 8-pixel page, page-major.
; total bytes = w * ((h+7)>>3)
; x=r25:r24, y=r23:r22, bmp=r21:r20, w=r18, h=r16
; bmp is PROGMEM pointer
; ---------------------------------------------------------
.section .text.DrawOverwriteRaw
DrawOverwriteRaw:
	clr r1
	tst r18
	breq vm23_overwrite_fast_done
	tst r16
	breq vm23_overwrite_fast_done
	tst r25
	brne vm23_overwrite_slow
	tst r23
	brne vm23_overwrite_slow
	cpi r24,128
	brsh vm23_overwrite_slow
	cpi r22,64
	brsh vm23_overwrite_slow
	mov r17,r24
	add r17,r18
	brcs vm23_overwrite_slow
	cpi r17,129
	brsh vm23_overwrite_slow
	mov r17,r22
	add r17,r16
	brcs vm23_overwrite_slow
	cpi r17,65
	brsh vm23_overwrite_slow
	mov r17,r22
	andi r17,7
	brne vm23_overwrite_slow

	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29

	movw r8,r20				; bitmap ptr
	movw r12,r24			; base x
	movw r10,r22			; base y
	mov r7,r18				; width
	mov r6,r16				; height

	mov r17,r16
	subi r17,lo8(-(7))
	lsr r17
	lsr r17
	lsr r17					; page count

	mov r18,r16
	andi r18,7				; last partial bits
	mov r15,r18
	breq vm23_overwrite_full_last
	clr r14
	inc r14
	mov r16,r15
	dec r16
	breq vm23_overwrite_lastmask_ready
vm23_overwrite_lastmask_loop:
	lsl r14
	inc r14
	dec r16
	brne vm23_overwrite_lastmask_loop
	rjmp vm23_overwrite_lastmask_ready
vm23_overwrite_full_last:
	clr r14
	dec r14
vm23_overwrite_lastmask_ready:

vm23_overwrite_fast_page_loop:
	ldi XL,lo8(sBuffer)
	ldi XH,hi8(sBuffer)
	mov r16,r10
	lsr r16
	lsr r16
	lsr r16
	clr r18
	bst r16,0
	bld r18,7
	mov r0,r16
	lsr r0
	add XL,r18
	adc XH,r0
	add XL,r12
	adc XH,r1

	movw ZL,r8
	mov r4,r7
	cpi r17,1
	breq vm23_overwrite_use_lastmask
	clr r3
	dec r3
	rjmp vm23_overwrite_mask_ready
vm23_overwrite_use_lastmask:
	mov r3,r14
vm23_overwrite_mask_ready:
	mov r18,r3
	inc r18
	breq vm23_overwrite_full_copy

vm23_overwrite_partial_copy:
	lpm r2,Z+
	ld r16,X
	mov r0,r3
	com r0
	and r16,r0
	and r2,r3
	or r16,r2
	st X+,r16
	dec r4
	brne vm23_overwrite_partial_copy
	rjmp vm23_overwrite_fast_next_page

vm23_overwrite_full_copy:
	lpm r2,Z+
	st X+,r2
	dec r4
	brne vm23_overwrite_full_copy

vm23_overwrite_fast_next_page:
	add r8,r7
	adc r9,r1
	ldi r18,8
	add r10,r18
	adc r11,r1
	dec r17
	brne vm23_overwrite_fast_page_loop

	clr r1
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
vm23_overwrite_fast_done:
	ret

vm23_overwrite_slow:
	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r28
	push r29

	clr r1

	movw r12,r24			; base x
	movw r10,r22			; current page y base
	movw r8,r20				; current bitmap page ptr (PROGMEM)
	mov r7,r18				; width
	mov r14,r16				; remaining height

	tst r7
	breq vm23_overwrite_slow_done
	tst r14
	breq vm23_overwrite_slow_done

vm23_overwrite_slow_page_loop:
	movw ZL,r8
	movw r2,r12				; current x
	mov r5,r7				; columns remaining
	mov r6,r14				; bits in this page
	mov r16,r6
	cpi r16,8
	brlo vm23_overwrite_slow_bits_ready
	ldi r16,8
	mov r6,r16
vm23_overwrite_slow_bits_ready:

vm23_overwrite_slow_col_loop:
	lpm r4,Z+
	movw YL,r10				; current y
	mov r16,r6

vm23_overwrite_slow_bit_loop:
	clr r20
	sbrc r4,0
	ldi r20,1
	movw r24,r2
	movw r22,YL
	call DrawPixel
	lsr r4
	adiw YL,1
	dec r16
	brne vm23_overwrite_slow_bit_loop

	inc r2
	brne vm23_overwrite_slow_no_x_carry
	inc r3
vm23_overwrite_slow_no_x_carry:

	dec r5
	brne vm23_overwrite_slow_col_loop

	mov r16,r14
	cpi r16,9
	brlo vm23_overwrite_slow_done
	ldi r16,8
	sub r14,r16
	add r10,r16
	adc r11,r1
	add r8,r7
	adc r9,r1
	rjmp vm23_overwrite_slow_page_loop

vm23_overwrite_slow_done:
	clr r1
	pop r29
	pop r28
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
	ret

; ---------------------------------------------------------
; SpritesDrawOverwrite(x,y,sprite,frame)
; sprite format (PROGMEM): [w][h][frame0...]
; frame data is native Arduboy vertical page format.
; frame_bytes = w * ((h+7)>>3)
; x=r25:r24, y=r23:r22, spr=r21:r20, frame=r18
; ---------------------------------------------------------
.section .text.SpritesDrawOverwrite
SpritesDrawOverwrite:
	push r16
	push r17

	mov r27,r18				; frame index
	movw ZL,r20				; sprite base (PROGMEM)

	lpm r18,Z+				; width
	lpm r16,Z+				; height

	mov r19,r16
	subi r19,lo8(-(7))
	lsr r19
	lsr r19
	lsr r19					; pages = (h+7)>>3

	mul r18,r19				; frame_bytes = w * pages
	mov r20,r0
	mov r21,r1
	clr r1

	tst r27
	breq vm23_sow_ptr_ready
vm23_sow_frame_loop:
	add ZL,r20
	adc ZH,r21
	dec r27
	brne vm23_sow_frame_loop
vm23_sow_ptr_ready:

	mov r20,ZL
	mov r21,ZH
	call DrawOverwriteRaw

	pop r17
	pop r16
	ret


; ---------------------------------------------------------
; SpritesDrawPlusMask(x,y,sprite,frame)
; Fast path for fully in-bounds, page-aligned native Arduboy plus-mask sprites.
; Falls back to the old per-pixel path for unaligned/clipped cases.
; x=r25:r24, y=r23:r22, spr=r21:r20, frame=r18
; ---------------------------------------------------------
.section .text.SpritesDrawPlusMask
SpritesDrawPlusMask:
	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29

	clr r1

	movw r12,r24			; base x
	movw r10,r22			; base y

	movw ZL,r20				; sprite base (PROGMEM)
	mov r17,r18				; frame index

	lpm r15,Z+				; width
	lpm r16,Z+				; height
	mov r14,r16				; full height copy

	tst r15
	breq vm23_spm_exit
	tst r14
	breq vm23_spm_exit

	; fast path requires fully in-bounds and y page aligned
	tst r13
	brne vm23_spm_slow_entry
	tst r11
	brne vm23_spm_slow_entry
	mov r18,r12
	cpi r18,128
	brsh vm23_spm_slow_entry
	mov r18,r10
	cpi r18,64
	brsh vm23_spm_slow_entry
	mov r2,r12
	add r2,r15
	brcs vm23_spm_slow_entry
	mov r18,r2
	cpi r18,129
	brsh vm23_spm_slow_entry
	mov r2,r10
	add r2,r14
	brcs vm23_spm_slow_entry
	mov r18,r2
	cpi r18,65
	brsh vm23_spm_slow_entry
	mov r18,r10
	andi r18,7
	brne vm23_spm_slow_entry

	mov r18,r16
	subi r18,lo8(-(7))
	lsr r18
	lsr r18
	lsr r18					; pages = (h+7)>>3

	mul r15,r18
	mov r20,r0
	mov r21,r1
	clr r1
	lsl r20
	rol r21					; frame_bytes *= 2

	tst r17
	breq vm23_spm_fast_frame_ready
vm23_spm_fast_frame_loop:
	add ZL,r20
	adc ZH,r21
	dec r17
	brne vm23_spm_fast_frame_loop
vm23_spm_fast_frame_ready:
	movw r8,ZL

	mov r17,r14
	andi r17,7
	breq vm23_spm_full_last
	clr r13
	inc r13
	mov r2,r17
	dec r2
	breq vm23_spm_lastmask_ready
vm23_spm_lastmask_loop:
	lsl r13
	inc r13
	dec r2
	brne vm23_spm_lastmask_loop
	rjmp vm23_spm_lastmask_ready
vm23_spm_full_last:
	clr r13
	dec r13
vm23_spm_lastmask_ready:

	mov r17,r14
	subi r17,lo8(-(7))
	lsr r17
	lsr r17
	lsr r17					; page countdown

vm23_spm_fast_page_loop:
	ldi XL,lo8(sBuffer)
	ldi XH,hi8(sBuffer)
	mov r2,r10
	lsr r2
	lsr r2
	lsr r2
	clr r3
	bst r2,0
	bld r3,7
	mov r0,r2
	lsr r0
	add XL,r3
	adc XH,r0
	add XL,r12
	adc XH,r1

	movw ZL,r8
	mov r5,r15
	cpi r17,1
	breq vm23_spm_fast_lastmask
	clr r4
	dec r4
	rjmp vm23_spm_fast_mask_ready
vm23_spm_fast_lastmask:
	mov r4,r13
vm23_spm_fast_mask_ready:

vm23_spm_fast_col_loop:
	lpm r2,Z+				; img
	lpm r3,Z+				; mask
	and r2,r4
	mov r16,r2
	com r16
	and r3,r16				; clear_mask = mask & ~img
	and r3,r4
	ld r16,X
	mov r0,r3
	com r0
	and r16,r0
	or r16,r2
	st X+,r16
	dec r5
	brne vm23_spm_fast_col_loop

	add r8,r15
	adc r9,r1
	add r8,r15
	adc r9,r1
	ldi r18,8
	add r10,r18
	adc r11,r1
	dec r17
	brne vm23_spm_fast_page_loop
	rjmp vm23_spm_exit

vm23_spm_slow_entry:
	mov r14,r16				; remaining height
	mov r18,r16
	subi r18,lo8(-(7))
	lsr r18
	lsr r18
	lsr r18					; pages = (h+7)>>3

	mul r15,r18
	mov r20,r0
	mov r21,r1
	clr r1
	lsl r20
	rol r21					; frame_bytes *= 2

	tst r17
	breq vm23_spm_slow_frame_ready
vm23_spm_slow_frame_loop:
	add ZL,r20
	adc ZH,r21
	dec r17
	brne vm23_spm_slow_frame_loop

vm23_spm_slow_frame_ready:
	movw r8,ZL				; current frame page ptr

vm23_spm_slow_page_loop:
	movw ZL,r8
	movw r2,r12				; current x
	mov r5,r15				; columns remaining
	mov r6,r14				; bits in this page
	mov r16,r6
	cpi r16,8
	brlo vm23_spm_slow_bits_ready
	ldi r16,8
	mov r6,r16
vm23_spm_slow_bits_ready:

vm23_spm_slow_col_loop:
	lpm r4,Z+				; img
	lpm r17,Z+				; mask
	movw YL,r10				; current y
	mov r16,r6

vm23_spm_slow_bit_loop:
	sbrc r4,0
	rjmp vm23_spm_slow_set
	sbrc r17,0
	rjmp vm23_spm_slow_clear
	rjmp vm23_spm_slow_after

vm23_spm_slow_set:
	movw r24,r2
	movw r22,YL
	ldi r20,1
	call DrawPixel
	rjmp vm23_spm_slow_after

vm23_spm_slow_clear:
	movw r24,r2
	movw r22,YL
	clr r20
	call DrawPixel

vm23_spm_slow_after:
	lsr r4
	lsr r17
	adiw YL,1
	dec r16
	brne vm23_spm_slow_bit_loop

	inc r2
	brne vm23_spm_slow_no_x_carry
	inc r3
vm23_spm_slow_no_x_carry:

	dec r5
	brne vm23_spm_slow_col_loop

	mov r16,r14
	cpi r16,9
	brlo vm23_spm_exit
	ldi r16,8
	sub r14,r16
	add r10,r16
	adc r11,r1
	add r8,r15
	adc r9,r1
	add r8,r15
	adc r9,r1
	rjmp vm23_spm_slow_page_loop

vm23_spm_exit:
	clr r1
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
	ret

; ---------------------------------------------------------
; ArduboyFillRect(x,y,w,h,color)
; Fast path for fully in-bounds fills using page masks.
; Falls back to the old row-by-row path for clipped/negative cases.
; x=r25:r24, y=r23:r22, w=r20, h=r18, color=r16
; ---------------------------------------------------------
.section .text.ArduboyFillRect
ArduboyFillRect:
	clr r1
	tst r20
	breq vm23_fillrect_fast_done
	tst r18
	breq vm23_fillrect_fast_done
	tst r25
	brne vm23_fillrect_slow
	tst r23
	brne vm23_fillrect_slow
	cpi r24,128
	brsh vm23_fillrect_slow
	cpi r22,64
	brsh vm23_fillrect_slow
	mov r17,r24
	add r17,r20
	brcs vm23_fillrect_slow
	cpi r17,129
	brsh vm23_fillrect_slow
	mov r17,r22
	add r17,r18
	brcs vm23_fillrect_slow
	cpi r17,65
	brsh vm23_fillrect_slow

	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29

	movw r12,r24			; base x
	movw r10,r22			; base y
	mov r7,r20				; width
	mov r6,r18				; remaining height
	mov r5,r16				; color
	mov r19,r22
	andi r19,7				; starting bit within page

vm23_fillrect_fast_page_loop:
	ldi r22,8
	sub r22,r19				; bits left in current page
	mov r23,r6
	cp r23,r22
	brlo vm23_fillrect_fast_bits_ready
	mov r23,r22
vm23_fillrect_fast_bits_ready:
	mov r22,r23
	ldi r21,1
	dec r22
	breq vm23_fillrect_fast_lowmask_ready
vm23_fillrect_fast_lowmask_loop:
	lsl r21
	ori r21,1
	dec r22
	brne vm23_fillrect_fast_lowmask_loop
vm23_fillrect_fast_lowmask_ready:
	mov r22,r19
	tst r22
	breq vm23_fillrect_fast_mask_ready
vm23_fillrect_fast_shift_loop:
	lsl r21
	dec r22
	brne vm23_fillrect_fast_shift_loop
vm23_fillrect_fast_mask_ready:

	ldi XL,lo8(sBuffer)
	ldi XH,hi8(sBuffer)
	mov r16,r10
	lsr r16
	lsr r16
	lsr r16
	clr r18
	bst r16,0
	bld r18,7
	mov r0,r16
	lsr r0
	add XL,r18
	adc XH,r0
	add XL,r12
	adc XH,r1

	mov r4,r7
	mov r17,r5
	cpi r17,2
	breq vm23_fillrect_fast_invert
	tst r5
	breq vm23_fillrect_fast_clear

vm23_fillrect_fast_set_loop:
	ld r16,X
	or r16,r21
	st X+,r16
	dec r4
	brne vm23_fillrect_fast_set_loop
	rjmp vm23_fillrect_fast_next_page

vm23_fillrect_fast_clear:
	mov r0,r21
	com r0
vm23_fillrect_fast_clear_loop:
	ld r16,X
	and r16,r0
	st X+,r16
	dec r4
	brne vm23_fillrect_fast_clear_loop
	rjmp vm23_fillrect_fast_next_page

vm23_fillrect_fast_invert:
	ld r16,X
	eor r16,r21
	st X+,r16
	dec r4
	brne vm23_fillrect_fast_invert

vm23_fillrect_fast_next_page:
	sub r6,r23
	breq vm23_fillrect_fast_popret
	ldi r17,8
	add r10,r17
	adc r11,r1
	clr r19
	rjmp vm23_fillrect_fast_page_loop

vm23_fillrect_fast_popret:
	clr r1
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
vm23_fillrect_fast_done:
	ret

vm23_fillrect_slow:
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17

	movw r12,r24
	movw r10,r22
	mov r14,r20
	mov r15,r18
	mov r17,r16

	tst r14
	breq vm23_fillrect_slow_done
	tst r15
	breq vm23_fillrect_slow_done

vm23_fillrect_slow_row_loop:
	movw r24,r12
	movw r22,r10
	mov r20,r14
	mov r18,r17
	call DrawFastHLine

	inc r10
	brne vm23_fillrect_slow_no_carry
	inc r11
vm23_fillrect_slow_no_carry:
	dec r15
	brne vm23_fillrect_slow_row_loop

vm23_fillrect_slow_done:
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	ret

; ---------------------------------------------------------
; ArduboyDrawRect(x,y,w,h,color)
; x=r25:r24, y=r23:r22, w=r20, h=r18, color=r16
; ---------------------------------------------------------
.section .text.ArduboyDrawRect
ArduboyDrawRect:
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17

	movw r12,r24
	movw r10,r22
	mov r14,r20
	mov r15,r18
	mov r17,r16

	tst r14
	breq vm23_drawrect_done
	tst r15
	breq vm23_drawrect_done

	; top
	movw r24,r12
	movw r22,r10
	mov r20,r14
	mov r18,r17
	call DrawFastHLine

	; bottom if h>1
	mov r16,r15
	cpi r16,2
	brlo vm23_drawrect_check_sides

	movw r24,r12
	movw r22,r10
	mov r16,r15
	dec r16
	add r22,r16
	adc r23,r1
	mov r20,r14
	mov r18,r17
	call DrawFastHLine

vm23_drawrect_check_sides:
	; no vertical interior if h<=2
	mov r16,r15
	cpi r16,3
	brlo vm23_drawrect_done

	; left interior
	movw r24,r12
	movw r22,r10
	subi r22,lo8(-(1))
	sbci r23,hi8(-(1))
	mov r20,r15
	subi r20,2
	mov r18,r17
	call DrawFastVLine

	; right interior if w>1
	mov r16,r14
	cpi r16,2
	brlo vm23_drawrect_done

	movw r24,r12
	mov r16,r14
	dec r16
	add r24,r16
	adc r25,r1

	movw r22,r10
	subi r22,lo8(-(1))
	sbci r23,hi8(-(1))
	mov r20,r15
	subi r20,2
	mov r18,r17
	call DrawFastVLine

vm23_drawrect_done:
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	ret


; ---------------------------------------------------------
; ArduboyPrintChar5x7 (6x8 cell from font6x8_h)
; Uses the existing row-major font, but plots into the native Arduboy
; vertical work buffer through DrawPixel(). Transparent if fg==bg.
;
; Input:
;	r20 = character code (byte)
; ---------------------------------------------------------
.section .text.ArduboyPrintChar5x7
ArduboyPrintChar5x7:
	push r2
	push r3
	push r4
	push r5
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r18
	push r19
	push r30
	push r31

	clr r1

	; ignore CR
	cpi r20,13
	brne vm23_pc_not_cr
	rjmp vm23_pc_done

vm23_pc_not_cr:
	; handle LF
	cpi r20,10
	brne vm23_pc_draw

	sts arduboy_cursor_x,   r1
	sts arduboy_cursor_x+1, r1

	lds r16,arduboy_cursor_y
	lds r17,arduboy_cursor_y+1
	subi r16,lo8(-(8))
	sbci r17,hi8(-(8))
	sts arduboy_cursor_y,   r16
	sts arduboy_cursor_y+1, r17
	rjmp vm23_pc_done

vm23_pc_draw:
	lds r12,arduboy_cursor_x
	lds r13,arduboy_cursor_x+1
	lds r10,arduboy_cursor_y
	lds r11,arduboy_cursor_y+1

	tst r13
	breq 1f
	rjmp vm23_pc_advance_only
1:
	tst r11
	breq 2f
	rjmp vm23_pc_advance_only
2:
	mov r16,r12
	cpi r16,123
	brlo 3f
	rjmp vm23_pc_advance_only
3:
	mov r16,r10
	cpi r16,57
	brlo 4f
	rjmp vm23_pc_advance_only
4:

	ldi ZL,lo8(font6x8_h)
	ldi ZH,hi8(font6x8_h)

	clr r17
	mov r16,r20
	lsl r16
	rol r17
	lsl r16
	rol r17
	lsl r16
	rol r17
	add ZL,r16
	adc ZH,r17

	lds r18,text_color
	lds r19,text_bg

	mov r17,r19
	cp r17,r18
	breq vm23_pc_bg_off
	ldi r17,1
	rjmp vm23_pc_bg_ready
vm23_pc_bg_off:
	clr r17
vm23_pc_bg_ready:

	ldi r16,8
vm23_pc_row_loop:
	lpm r14,Z+
	movw r2,r12
	clr r15
	inc r15
	inc r15
	inc r15
	inc r15
	inc r15
	inc r15
	clr r4
	inc r4
	lsl r4
	lsl r4
	lsl r4
	lsl r4
	lsl r4
	lsl r4
	lsl r4

vm23_pc_col_loop:
	mov r5,r14
	and r5,r4
	breq vm23_pc_try_bg

	movw r24,r2
	movw r22,r10
	mov r20,r18
	call DrawPixel
	rjmp vm23_pc_after_px

vm23_pc_try_bg:
	tst r17
	breq vm23_pc_after_px
	movw r24,r2
	movw r22,r10
	mov r20,r19
	call DrawPixel

vm23_pc_after_px:
	inc r2
	brne vm23_pc_no_x_carry
	inc r3
vm23_pc_no_x_carry:
	lsr r4
	dec r15
	brne vm23_pc_col_loop

	inc r10
	brne vm23_pc_y_inc_done
	inc r11
vm23_pc_y_inc_done:
	dec r16
	brne vm23_pc_row_loop

vm23_pc_advance_only:
	lds r16,arduboy_cursor_x
	lds r17,arduboy_cursor_x+1
	subi r16,lo8(-(6))
	sbci r17,hi8(-(6))
	sts arduboy_cursor_x,   r16
	sts arduboy_cursor_x+1, r17

vm23_pc_done:
	pop r31
	pop r30
	pop r19
	pop r18
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r5
	pop r4
	pop r3
	pop r2
	ret


; ---------------------------------------------------------
; vm23_apply_color_mask
; Apply color op (r6) to byte (r4) under mask (r5)
; r6: 0 clear, 1 set, 2 invert
; Destroys: r0
; Returns: r4 updated
; ---------------------------------------------------------
vm23_apply_color_mask:
	mov r24,r6
	cpi r24,2
	breq vm23_acm_invert
	tst r24
	breq vm23_acm_clear

	; set
	or r4,r5
	ret

vm23_acm_invert:
	eor r4,r5
	ret

vm23_acm_clear:
	mov r0,r5
	com r0
	and r4,r0
	ret


; ---------------------------------------------------------
; ArduboyPrint(text)
; r25:r24 = char* (RAM)
; ---------------------------------------------------------
.section .text.ArduboyPrint
ArduboyPrint:
	push r26
	push r27

	movw XL,r24
vm23_print_loop:
	ld r20,X+
	tst r20
	breq vm23_print_done
	call ArduboyPrintChar5x7
	rjmp vm23_print_loop
vm23_print_done:
	pop r27
	pop r26
	ret


; ---------------------------------------------------------
; ArduboyPrintU8(value)
; r24=value
; ---------------------------------------------------------
.section .text.ArduboyPrintU8
ArduboyPrintU8:
	push r16
	push r17
	push r18
	push r19

	mov r16,r24
	clr r17				; hundreds
	clr r18				; tens

vm23_printu8_hund_loop:
	cpi r16,100
	brlo vm23_printu8_tens_start
	subi r16,100
	inc r17
	rjmp vm23_printu8_hund_loop

vm23_printu8_tens_start:
	cpi r16,10
	brlo vm23_printu8_emit

vm23_printu8_tens_loop:
	cpi r16,10
	brlo vm23_printu8_emit
	subi r16,10
	inc r18
	rjmp vm23_printu8_tens_loop

vm23_printu8_emit:
	tst r17
	breq vm23_printu8_check_tens
	mov r20,r17
	subi r20,lo8(-'0')
	call ArduboyPrintChar5x7

vm23_printu8_check_tens:
	tst r17
	brne vm23_printu8_emit_tens
	tst r18
	breq vm23_printu8_emit_ones

vm23_printu8_emit_tens:
	mov r20,r18
	subi r20,lo8(-'0')
	call ArduboyPrintChar5x7

vm23_printu8_emit_ones:
	mov r20,r16
	subi r20,lo8(-'0')
	call ArduboyPrintChar5x7

	pop r19
	pop r18
	pop r17
	pop r16
	ret


; ---------------------------------------------------------
; ArduboyPrintU16(value)
; r25:r24 = value
; prints unsigned decimal 0..65535
; ---------------------------------------------------------
.section .text.ArduboyPrintU16
ArduboyPrintU16:
	push r16
	push r17
	push r18
	push r19
	push r20
	push r21

	movw r16,r24		; remaining value
	clr r19				; emitted flag

	; 10000
	ldi r20,lo8(10000)
	ldi r21,hi8(10000)
	rcall vm23_printu16_digit

	; 1000
	ldi r20,lo8(1000)
	ldi r21,hi8(1000)
	rcall vm23_printu16_digit

	; 100
	ldi r20,lo8(100)
	ldi r21,hi8(100)
	rcall vm23_printu16_digit

	; 10
	ldi r20,lo8(10)
	ldi r21,hi8(10)
	rcall vm23_printu16_digit

	; ones
	mov r20,r16
	subi r20,lo8(-'0')
	call ArduboyPrintChar5x7

	pop r21
	pop r20
	pop r19
	pop r18
	pop r17
	pop r16
	ret

; ---------------------------------------------------------
; vm23_printu16_digit
; in:
;	r17:r16 = remaining value
;	r21:r20 = divisor
;	r19 = emitted flag
; out:
;	r17:r16 = remainder
;	r19 updated if digit emitted
; destroys:
;	r18, r20
; ---------------------------------------------------------
vm23_printu16_digit:
	clr r18

vm23_printu16_digit_loop:
	cp  r16,r20
	cpc r17,r21
	brlo vm23_printu16_digit_done

	sub r16,r20
	sbc r17,r21
	inc r18
	rjmp vm23_printu16_digit_loop

vm23_printu16_digit_done:
	tst r19
	brne vm23_printu16_emit
	tst r18
	breq vm23_printu16_digit_ret

vm23_printu16_emit:
	mov r20,r18
	subi r20,lo8(-'0')
	call ArduboyPrintChar5x7
	ldi r19,1

vm23_printu16_digit_ret:
	ret


; ---------------------------------------------------------
; ArduboyDrawLine(x0,y0,x1,y1,color)
; x0=r25:r24, y0=r23:r22, x1=r21:r20, y1=r19:r18, color=r16
; ---------------------------------------------------------
.section .text.ArduboyDrawLine
ArduboyDrawLine:
	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
	push r30

	clr r1

	movw r12,r24		; x0
	movw r10,r22		; y0
	movw r8,r20			; x1
	movw r6,r18			; y1
	mov r30,r16			; color

	; dx = abs(x1 - x0), sx = (x0 < x1) ? +1 : -1
	cp r12,r8
	cpc r13,r9
	brlt vm23_line_x_inc

	movw r14,r12
	sub r14,r8
	sbc r15,r9
	clr r2
	dec r2
	clr r3
	dec r3
	rjmp vm23_line_x_done

vm23_line_x_inc:
	movw r14,r8
	sub r14,r12
	sbc r15,r13
	clr r2
	inc r2
	clr r3

vm23_line_x_done:
	; absdy = abs(y1 - y0), sy = (y0 < y1) ? +1 : -1
	cp r10,r6
	cpc r11,r7
	brlt vm23_line_y_inc

	movw r4,r10
	sub r4,r6
	sbc r5,r7
	ldi r28,0xff
	ldi r29,0xff
	rjmp vm23_line_y_abs_done

vm23_line_y_inc:
	movw r4,r6
	sub r4,r10
	sbc r5,r11
	ldi r28,1
	clr r29

vm23_line_y_abs_done:
	; dy = -absdy
	com r4
	com r5
	inc r4
	brne vm23_line_dy_neg_done
	inc r5
vm23_line_dy_neg_done:

	; err = dx + dy
	movw r16,r14
	add r16,r4
	adc r17,r5

vm23_line_loop:
	movw r24,r12
	movw r22,r10
	mov r20,r30
	call DrawPixel

	cp r12,r8
	cpc r13,r9
	brne vm23_line_not_done
	cp r10,r6
	cpc r11,r7
	breq vm23_line_done

vm23_line_not_done:
	; e2 = 2 * err
	movw r24,r16
	lsl r24
	rol r25

	; if (e2 >= dy) { err += dy; x0 += sx; }
	cp r24,r4
	cpc r25,r5
	brlt vm23_line_skip_x
	add r16,r4
	adc r17,r5
	add r12,r2
	adc r13,r3

vm23_line_skip_x:
	; if (e2 <= dx) { err += dx; y0 += sy; }
	cp r14,r24
	cpc r15,r25
	brlt vm23_line_skip_y
	add r16,r14
	adc r17,r15
	add r10,r28
	adc r11,r29

vm23_line_skip_y:
	rjmp vm23_line_loop

vm23_line_done:
	pop r30
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
	ret

; ---------------------------------------------------------
; ArduboyDrawCircle(x,y,r,color)
; x=r25:r24, y=r23:r22, r=r20, color=r18
; ---------------------------------------------------------
.section .text.ArduboyDrawCircle
ArduboyDrawCircle:
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r18
	clr r1

	movw r12,r24
	movw r10,r22
	clr r14
	mov r15,r20
	mov r17,r18

	mov r16,r15
	lsl r16
	neg r16
	subi r16,lo8(-(3))

vm23_drawcircle_loop:
	rcall vm23_drawcircle_plot8

	tst r16
	brmi vm23_drawcircle_d_neg

	mov r18,r14
	sub r18,r15
	lsl r18
	lsl r18
	subi r18,lo8(-(10))
	add r16,r18
	dec r15
	rjmp vm23_drawcircle_step_x

vm23_drawcircle_d_neg:
	mov r18,r14
	lsl r18
	lsl r18
	subi r18,lo8(-(6))
	add r16,r18

vm23_drawcircle_step_x:
	inc r14
	cp r15,r14
	brsh vm23_drawcircle_loop

	pop r18
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	ret

vm23_drawcircle_plot8:
	; (cx+x, cy+y)
	movw r24,r12
	add r24,r14
	adc r25,r1
	movw r22,r10
	add r22,r15
	adc r23,r1
	mov r20,r17
	call DrawPixel

	; (cx-x, cy+y)
	movw r24,r12
	sub r24,r14
	sbc r25,r1
	movw r22,r10
	add r22,r15
	adc r23,r1
	mov r20,r17
	call DrawPixel

	; (cx+x, cy-y)
	movw r24,r12
	add r24,r14
	adc r25,r1
	movw r22,r10
	sub r22,r15
	sbc r23,r1
	mov r20,r17
	call DrawPixel

	; (cx-x, cy-y)
	movw r24,r12
	sub r24,r14
	sbc r25,r1
	movw r22,r10
	sub r22,r15
	sbc r23,r1
	mov r20,r17
	call DrawPixel

	; (cx+y, cy+x)
	movw r24,r12
	add r24,r15
	adc r25,r1
	movw r22,r10
	add r22,r14
	adc r23,r1
	mov r20,r17
	call DrawPixel

	; (cx-y, cy+x)
	movw r24,r12
	sub r24,r15
	sbc r25,r1
	movw r22,r10
	add r22,r14
	adc r23,r1
	mov r20,r17
	call DrawPixel

	; (cx+y, cy-x)
	movw r24,r12
	add r24,r15
	adc r25,r1
	movw r22,r10
	sub r22,r14
	sbc r23,r1
	mov r20,r17
	call DrawPixel

	; (cx-y, cy-x)
	movw r24,r12
	sub r24,r15
	sbc r25,r1
	movw r22,r10
	sub r22,r14
	sbc r23,r1
	mov r20,r17
	call DrawPixel

	ret

; ---------------------------------------------------------
; ArduboyFillCircle(x,y,r,color)
; x=r25:r24, y=r23:r22, r=r20, color=r18
; ---------------------------------------------------------
.section .text.ArduboyFillCircle
ArduboyFillCircle:
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r18
	push r19

	clr r1

	movw r12,r24
	movw r10,r22
	clr r14
	mov r15,r20
	mov r17,r18

	mov r16,r15
	lsl r16
	neg r16
	subi r16,lo8(-(3))

vm23_fillcircle_loop:
	rcall vm23_fillcircle_spans

	tst r16
	brmi vm23_fillcircle_d_neg

	mov r18,r14
	sub r18,r15
	lsl r18
	lsl r18
	subi r18,lo8(-(10))
	add r16,r18
	dec r15
	rjmp vm23_fillcircle_step_x

vm23_fillcircle_d_neg:
	mov r18,r14
	lsl r18
	lsl r18
	subi r18,lo8(-(6))
	add r16,r18

vm23_fillcircle_step_x:
	inc r14
	cp r15,r14
	brsh vm23_fillcircle_loop

	pop r19
	pop r18
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	ret

vm23_fillcircle_spans:
	; row: cy + xoff, xstart = cx - yoff, width = 2*yoff + 1
	movw r24,r12
	sub r24,r15
	sbc r25,r1
	movw r22,r10
	add r22,r14
	adc r23,r1
	mov r20,r15
	lsl r20
	inc r20
	mov r18,r17
	call DrawFastHLine

	; row: cy - xoff, xstart = cx - yoff, width = 2*yoff + 1
	movw r24,r12
	sub r24,r15
	sbc r25,r1
	movw r22,r10
	sub r22,r14
	sbc r23,r1
	mov r20,r15
	lsl r20
	inc r20
	mov r18,r17
	call DrawFastHLine

	; row: cy + yoff, xstart = cx - xoff, width = 2*xoff + 1
	movw r24,r12
	sub r24,r14
	sbc r25,r1
	movw r22,r10
	add r22,r15
	adc r23,r1
	mov r20,r14
	lsl r20
	inc r20
	mov r18,r17
	call DrawFastHLine

	; row: cy - yoff, xstart = cx - xoff, width = 2*xoff + 1
	movw r24,r12
	sub r24,r14
	sbc r25,r1
	movw r22,r10
	sub r22,r15
	sbc r23,r1
	mov r20,r14
	lsl r20
	inc r20
	mov r18,r17
	call DrawFastHLine

	ret

; ---------------------------------------------------------
; PROGMEM assets (font6x8_h, logos, etc.)
; Put them in gfx.inc so you can add logo data too.
; gfx.inc must define: font6x8_h:
; ---------------------------------------------------------
.section .progmem.data
#include "gfx.inc"


.section .text
; ---------------------------------------------------------
; Standard mode hooks
; ---------------------------------------------------------
DisplayLogo:
VideoModeVsync:
InitializeVideoMode:
	ret