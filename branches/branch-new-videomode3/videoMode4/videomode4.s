/*
 *  Uzebox Kernel - Mode 4
 *  Copyright (C) 2008 David Etherton
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
;***************************************************
; Video Mode 4
; -18x12 tiles (16x16 pixels)
; -No sprites
; -Full screen X/Y scrolling
; -Fixed text area
;***************************************************	

.global DisplayLogo
.global VideoModeVsync
.global InitializeVideoMode
.global vram
.global SetAsciiTiles

.section .bss
	vram: 	  				.space VRAM_SIZE 
	textram:				.space (16 * 36)
	scroll:					.space 1
	scroll_hi:				.space 1
	scroll_v_fine:			.space 1
	scroll_h_fine:			.space 1
	tileheight:				.space 1
	textheight:				.space 1
	asciitiles:				.space 2 ;pointer to text tiles

.section .text

.macro pixel reg
	out DATA_PORT-32,\reg	; 1
	.endm

render_bitmap_line:
; r24/r25 - pointer to interleaved text patterns (biased by +0,+64, etc depending on scanline)
; Y  = pointer to text memory (autoincremented)
; destroys r0, r1, r2, r16 ZL, ZH
; (r0 and r1 are actually the background and foreground colors)
; code relies on the fact that the last column is always empty since it's text
; to have enough cycles to load the next eight pixels
; 12+1440+43 = 1495 cycles

	; 12 cycles
	clr r0		; 1 - background color
	clr r1		; 1 - foreground color
	dec r1		; 1 - convert to 255

	ld ZL,Y+	; 2 - get character to draw
	mov ZH,r0	; 1 - high word
	add ZL,r24	; 1 - lo word of ascii bitmap
	adc ZH,r25	; 1 - hi word of ascii bitmap
	lpm r2,Z	; 3 - get bitmap pixels
	mov r16,r0	; 1 - get foreground color

	; 1440 cycles
.rept 36
	ld ZL,Y+	; 2 - get (next) character to draw
	sbrc r2,7	; 1 - test pixel 7
	mov r16,r1	; 1 - get background color
	out _SFR_IO_ADDR(DATA_PORT),r16	; 1

	nop
	mov r16,r0	; 1 - get foreground color
	sbrc r2,6	; 1 - test pixel 6
	mov r16,r1	; 1 - get background color
	out _SFR_IO_ADDR(DATA_PORT),r16	; 1

	nop
	mov r16,r0	; 1 - get foreground color
	sbrc r2,5	; 1 - test pixel 5
	mov r16,r1	; 1 - get background color
	out _SFR_IO_ADDR(DATA_PORT),r16	; 1

	nop
	mov r16,r0	; 1 - get foreground color
	sbrc r2,4	; 1 - test pixel 4
	mov r16,r1	; 1 - get background color
	out _SFR_IO_ADDR(DATA_PORT),r16	; 1

	mov ZH,r0	; 1 - high word
	mov r16,r0	; 1 - get foreground color
	sbrc r2,3	; 1 - test pixel 3
	mov r16,r1	; 1 - get background color
	out _SFR_IO_ADDR(DATA_PORT),r16	; 1

	add ZL,r24	; 1 - lo word of ascii bitmap
	mov r16,r0	; 1 - get foreground color
	sbrc r2,2	; 1 - test pixel 2
	mov r16,r1	; 1 - get background color
	out _SFR_IO_ADDR(DATA_PORT),r16	; 1

	adc ZH,r25	; 1- hi word of ascii bitmap
	mov r16,r0	; 1 - get foreground color
	sbrc r2,1	; 1 - test pixel 1
	mov r16,r1	; 1 - get background color
	out _SFR_IO_ADDR(DATA_PORT),r16	; 1

	lpm r2,Z	; 3 - get bitmap pixels
	mov r16,r0	; 1 - get foreground color
	out _SFR_IO_ADDR(DATA_PORT),r0	; 1 (gap color)
.endr

	; need 43 cycles delay here
	sbiw YL,37		; 2
	ldi r16,12		; 1
	; loop below is 3 cycles per iteration
	dec r16			;   (1)
	brne .-4		;   (2)
	nop				;   (1) for when branch not taken

	ret				; 4


;***************************************************
; TEXT MODE VIDEO PROCESSING
; Process video frame in tile mode (18x12, 16,x16 tiles)
;***************************************************	

sub_video_mode4:

	;waste line to align with next hsync in render function
	ldi ZL,222 //200-20+22+19+1
mode0_render_delay:
	lpm
	nop
	dec ZL
	brne mode0_render_delay 

	nop
	nop

	lds YL,scroll
	lds YH,scroll_hi

	lds r16,tileheight; total scanlines to draw (28*8)
	mov r20,r16
	lds XL,scroll_v_fine
	ldi r23,TILE_WIDTH ;tile width in pixels



next_text_line:
	rcall hsync_pulse ;3+144=147

#define CENTER 10
	ldi r19,21 + CENTER		; 63 cycles
	dec r19			; 1
	brne .-4		; 1

	;***draw line***
	call render_tile_line	; 1591+4 = 1595

	ldi r19,26 - CENTER		; 78 cycles
	dec r19			; 1
	brne .-4		; 1
	nop

	; 1806 up to here
	dec r20					; 1
	breq text_frame_end		; 1 (12 cycles before next hsync_pulse)
	
	ldi r19,16				; 1
	add XL,r19				; 1

	cpi XL,0 ;last char line? 1
	breq next_text_row 		; 1
	lpm						; 3 nop
	lpm						; 3 nop
	
	rjmp next_text_line		; 2 (1820 total)

next_text_row:
	clr r22		;current char line		; 2	(1+1 for branch taken)
	clr r0								; 1
	ldi r19,VRAM_TILES_H				; 1
	add YL,r19							; 1
	adc YH,r0							; 1

	rjmp next_text_line					; 2 (1820 total)

#define CENTER_ADJUSTMENT2 12
	; now do the four status lines
text_frame_end:
	nop

	ldi YL,lo8(textram)				; 1
	ldi YH,hi8(textram)				; 1
	lds r24,asciitiles+0			; 2
	lds r25,asciitiles+1			; 2
	ldi r26,64						; 1
	lds r20,textheight				; 1
status_outer_loop:
	ldi r21,8						; 1
status_inner_loop:
	rcall hsync_pulse ;147
		ldi r19,41-5 + CENTER_ADJUSTMENT2
		dec r19									
		brne .-4
	rcall render_bitmap_line
		ldi r19,2+10+5 - CENTER_ADJUSTMENT2
		dec r19			
		brne .-4

	add r24,r26		; 1
	adc r25,r0		; 1
	dec r21			; 1
	breq status_next_row
	lpm
	lpm
	lpm
	nop
	rjmp status_inner_loop

status_next_row:
	subi r25,2	; 1
	adiw YL,36	; 2
	dec r20		; 1
	rjmp .
	rjmp .
	brne status_outer_loop ; 2
	nop

	rcall hsync_pulse ;145
	

text_end2:

	;set vsync flag if beginning of next frame (each two fields)
	ldi r17,1
	lds r16,curr_field
	eor r16,r17
	sts curr_field,r16
	sbrs r16,0
	sts vsync_flag,r17

	;clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	ret

;*************************************************
; RENDER TILE LINE
;
; XL	  = internal tile offset
; Y       = VRAM adress to draw from (must not be modified)
;
; Can destroy: r0-r17, XH,ZL,ZH
; 
; cycles  = 37+26+1515+13 = 1591
;*************************************************
render_tile_line:
	; get all 18 tiles now before starting render kernel
	; XL is scanline within tile (0, 16, 32, etc)
	; 37 cycles in block below
	mov ZL,XL	; 1 cycle
	ldd ZH,y+0	; tile 0
	ldd XH,y+1	; tile 1
	ldd r0,y+2	; tile 2
	ldd r1,y+3	; tile 3
	ldd r2,y+4	; tile 4
	ldd r3,y+5	; tile 5
	ldd r4,y+6	; tile 6
	ldd r5,y+7	; tile 7
	ldd r6,y+8	; tile 8
	ldd r7,y+9	; tile 9
	ldd r8,y+10	; tile 10
	ldd r9,y+11	; tile 11
	ldd r10,y+12	; tile 12
	ldd r11,y+13	; tile 13
	ldd r12,y+14	; tile 14
	ldd r13,y+15	; tile 15
	ldd r14,y+16	; tile 16
	ldd r15,y+17	; tile 17

	; this does a computed GOTO without using Z register
	; scroll_h_fine must be between 0 and 15
	; 26 cycles in block below
	ldi r24,lo8(tile_0)			; 1
	ldi r25,hi8(tile_0)			; 1
	lsr r25						; 1
	ror r24						; 1
	lds r16,scroll_h_fine		; 2
	clr r17						; 1
	add ZL,r16					; 1
	adc ZH,r17					; 1
	add r24,r16					; 1
	adc r25,r17					; 1			
	add r16,r16					; 1
	add r24,r16					; 1
	adc r25,r17					; 1
	ldd r17,y+18				; 2 - tile 18 (fine scroll)
	push r24					; 2
	push r25					; 2
	lds r24,scroll_h_fine		; 2
	ret							; 4

	; tile 0 (empty insn here for padding)
tile_0:
	nop

.rept 15
	lpm r16,Z+							; 3
	pixel r16		; 1
	nop									; 1
.endr
	lpm r16,Z							; 3
	pixel r16		; 1
	movw ZL,XL							; 1

.macro tileN nextReg
.rept 14
	lpm r16,Z+
	pixel r16		; 1
	nop
.endr
	lpm r16,Z+							; 3
	pixel r16		; 1
	mov XH,\nextReg						; 1
	lpm r16,Z							; 3
	pixel r16		; 1
	movw ZL,XL							; 1
.endm

	tileN r0		; tile 1
	tileN r1		; tile 2
	tileN r2		; tile 3
	tileN r3		; tile 4
	tileN r4		; tile 5
	tileN r5		; tile 6
	tileN r6		; tile 7
	tileN r7		; tile 8
	tileN r8		; tile 9
	tileN r9		; tile 10
	tileN r10		; tile 11
	tileN r11		; tile 12
	tileN r12		; tile 13
	tileN r13		; tile 14
	tileN r14		; tile 15
	tileN r15		; tile 16
	tileN r17		; tile 17

	; tile 18 (up to 15 pixels)
.macro tile18 fineScroll
	lpm r16,Z+							; 3
	pixel r16		; 1
	cpi r24,\fineScroll					; 1
	lpm r16,Z+							; 3
	pixel r16		; 1
	breq doneFineScroll					; 1 (+1 if taken)
.endm

	tile18 2
	tile18 4
	tile18 6
	tile18 8
	tile18 10
	tile18 12
	tile18 14

	lpm r16,Z+							; 3
	out _SFR_IO_ADDR(DATA_PORT),r16		; 1
	nop
	lpm r16,Z+							; 3
	out _SFR_IO_ADDR(DATA_PORT),r16		; 1
	nop

	nop		; extra cycle because we never took the branch

doneFineScroll:
	;end set last pix to zero
	clr r16
	clr r16
	clr r16
	clr r16
	out _SFR_IO_ADDR(DATA_PORT),r16
	; +5 cycles

	ret		; 4 cycles

	
	
;Nothing to do in this mode
DisplayLogo:
VideoModeVsync:
	ret
	
;C callable
InitializeVideoMode:
	
	ldi r24,SCREEN_TILES_V*(SCREEN_TILES_H-2)
	sts tileheight,r24

	ldi r24,4
	sts textheight,r24
	
	ret

;C callable
SetAsciiTiles:
	sts asciitiles+0,r24
	sts asciitiles+1,r25
	ret
