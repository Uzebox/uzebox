/*
 *  Uzebox Kernel - Mode 8
 *  Copyright (C) 2009  Alec Bourque
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

;****************************************************
; Video Mode 8
; Bitmappep mode: 120x96 @ 2bpp with 4 colors palette
;****************************************************	
	
.global PutPixel
.global GetPixel
.global vram
.global palette
.global InitializeVideoMode
.global DisplayLogo
.global VideoModeVsync
.global ClearVram

.section .bss
	.align 2		
	vram: 	  				.space VRAM_SIZE
	palette:				.space 4	;palette must be aligned to 4 bytes

.section .text	
	write_masks:		
			.byte 0b00111111,64;~(3<<6)
			.byte 0b11001111,16	;~(3<<4)
			.byte 0b11110011,4	;~(3<<2)
			.byte 0b11111100,1  ;~(3<<0)
	read_masks:
			.byte 0b11000000,4
			.byte 0b00110000,16
			.byte 0b00001100,64
			.byte 0b00000011,1

.section .text

sub_video_mode8:

	WAIT r16,1350

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)
	
	clr r20

;*************************************************************
; Rendering main loop starts here
;*************************************************************
next_scan_line:	
	rcall hsync_pulse 

	WAIT r19,330 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT

	;***draw line***
	rcall render_tile_line

	WAIT r19,118 - CENTER_ADJUSTMENT


	;duplicate each line
	sbrc r20,0
	subi YL,lo8(-(SCREEN_WIDTH/4))
	sbrc r20,0
	sbci YH,hi8(-(SCREEN_WIDTH/4))

	inc r20
	cpi r20,(SCREEN_HEIGHT*2)
	brne next_scan_line

	nop
	rcall hsync_pulse ;145
	
	;set vsync flag & flip field
	lds ZL,sync_flags
	ldi r20,SYNC_FLAG_FIELD
	ori ZL,SYNC_FLAG_VSYNC
	eor ZL,r20
	sts sync_flags,ZL
			
	;clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	clr r1

	ret


;*************************************************
; RENDER TILE LINE
;
; r20     = render line counter (incrementing)
; Y       = VRAM adress to draw from (must not be modified)
;
; Must preserve r20,Y
; 
; cycles  = 1495
;*************************************************
render_tile_line:
	movw r22,YL ;push

	ldi r19,lo8(palette)
	ldi XH,hi8(palette)

	;11 cycles per pixel
	ldi r18,SCREEN_WIDTH/4
	ld r16,Y+ ;load next 4 pixels
main_loop:		
	mov XL,r16
	swap XL
	lsr XL
	lsr XL
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 0
	rjmp .
	nop
	mov XL,r16
	swap XL
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	nop

	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 1	
	nop
	rjmp .
	mov XL,r16
	lsr XL
	lsr XL
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 2
	rjmp .
	mov XL,r16
	andi XL,0x3
	add XL,r19	;lo8 of palette
	ld r0,X		;load 'palettized' value
	ld r16,Y+	;load next 4 pixels
	dec r18

	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 3
	brne main_loop

	lpm
	lpm
	clr r0
	rjmp .
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 3


	movw YL,r22 ;pop
	ret




;**************************************************
;Plots a pixel at the specified location with the
;specified color (0-3).
;---------------------
;C-callable
;r24=X
;r22=Y
;r20=Color
;39 cycles
;***************************************************
.section .text.PutPixel
PutPixel:
	cpi r24,SCREEN_WIDTH
	brsh PutPixel_out_of_screen
	cpi r22,SCREEN_HEIGHT
	brsh PutPixel_out_of_screen
	andi r20,3	;mask color

	mov ZL,r24
	clr ZH
	andi ZL,3
	lsl ZL
	subi ZL,lo8(-(write_masks))
	sbci ZH,hi8(-(write_masks))
	
	ldi r23,(SCREEN_WIDTH/4)
	lsr r24
	lsr r24
	clr r25
	mul r22,r23
	add r24,r0
	adc r25,r1
	subi r24,lo8(-(vram))
	sbci r25,hi8(-(vram))
	movw XL,r24

	ld r22,X	;load byte (4 pixels)

	lpm r24,Z+	;load byte mask
	lpm r25,Z	;load shift multiplier

	and r22,r24	;mask byte
	mul r20,r25	;*shift multiplier (*1,*4,*16,*64)
	or r22,r0	;combine pixels
	st X,r22

	clr r1

PutPixel_out_of_screen:
	ret

;**************************************************
;Return the color of a pixel at the specified location (0-3). 
;Return 0 if pixel is out of screen.
;---------------------
;C-callable
;r24=X
;r22=Y
;returns: r24=Color
;38 cycles
;***************************************************
.section .text.GetPixel
GetPixel:
	cpi r24,SCREEN_WIDTH
	brsh GetPixel_out_of_screen
	cpi r22,SCREEN_HEIGHT
	brsh GetPixel_out_of_screen

	mov ZL,r24
	clr ZH
	andi ZL,3
	lsl ZL
	subi ZL,lo8(-(read_masks))
	sbci ZH,hi8(-(read_masks))
	
	ldi r23,(SCREEN_WIDTH/4)
	lsr r24
	lsr r24
	clr r25
	mul r22,r23
	add r24,r0
	adc r25,r1
	subi r24,lo8(-(vram))
	sbci r25,hi8(-(vram))
	movw XL,r24

	ld r24,X	;load byte (4 pixels)

	lpm r22,Z+	;load byte mask
	lpm r23,Z	;load shift multiplier
		
	and r24,r22	;mask byte
	cpi r23,1
	breq GetPixel_end
	mul r24,r23	;*shift multiplier (*4,*16,*64)
	mov r24,r1

GetPixel_end:
	clr r1
	ret

GetPixel_out_of_screen:
	clr r24
	ret

;***********************************
; CLEAR VRAM 8bit
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearVram
ClearVram:
	//init vram		
	ldi r30,lo8(VRAM_SIZE)
	ldi r31,hi8(VRAM_SIZE)

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)

fill_vram_loop:
	st X+,r1
	sbiw r30,1
	brne fill_vram_loop

	clr r1

	ret
	

;Nothing to do in this mode
DisplayLogo:
VideoModeVsync:
InitializeVideoMode:
	ret

