/*
 *  Uzebox Kernel - Mode 13
 *  Copyright (C) 2015  Alec Bourque
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
; SPI Video Mode
; Bitmapped mode using SPI RAM
;****************************************************	
	
.global PutPixel
.global GetPixel
.global vram
.global palette
.global InitializeVideoMode
.global DisplayLogo
.global VideoModeVsync
.global ClearVram
.global SetVram

.section .bss
	.align 2		
	vram: 	  				.space 2
	palette:				.space 4	;palette must be aligned to 4 bytes

.section .data	
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

spi_video_mode:

	lds YL,vram
	lds YH,vram+1
	
	lds r20,render_lines_count	


	cbi _SFR_IO_ADDR(PORTA),PA4

	ldi r16,3
	out _SFR_IO_ADDR(SPDR),r16
	WAIT r16,18

	ldi r16,0
	out _SFR_IO_ADDR(SPDR),r16
	WAIT r16,18

	mov r16,YH
	out _SFR_IO_ADDR(SPDR),r16
	WAIT r16,18

	mov r16,YL
	out _SFR_IO_ADDR(SPDR),r16
	WAIT r16,18

	ldi r16,0xff
	out _SFR_IO_ADDR(SPDR),r16
	WAIT r16,18

/*
	//send read command
	ldi r16,3
	out _SFR_IO_ADDR(SPDR),r16
	in r16,_SFR_IO_ADDR(SPSR)
	sbrs r16,SPIF
	rjmp .-6

	//send 24-bit adress MSB
	ldi r16,0
	out _SFR_IO_ADDR(SPDR),r16
	in r16,_SFR_IO_ADDR(SPSR)
	sbrs r16,SPIF
	rjmp .-6

	//send 24-bit adress Middle byte 
	ldi r16,0
	out _SFR_IO_ADDR(SPDR),r16
	in r16,_SFR_IO_ADDR(SPSR)
	sbrs r16,SPIF
	rjmp .-6

	//send 24-bit adress LSB
	ldi r16,2
	out _SFR_IO_ADDR(SPDR),r16
	in r16,_SFR_IO_ADDR(SPSR)
	sbrs r16,SPIF
	rjmp .-6

	//read first pixel
	ldi r16,0xff //dummy
	out _SFR_IO_ADDR(SPDR),r16
	in r16,_SFR_IO_ADDR(SPSR)
	sbrs r16,SPIF
	rjmp .-6
	in r17,_SFR_IO_ADDR(SPDR)
*/

	WAIT r16,1347-102

;*************************************************************
; Rendering main loop starts here
;*************************************************************
next_scan_line:	
	rcall hsync_pulse 

	WAIT r19,275 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT

	;***draw line***
	rcall render_tile_line

	WAIT r19,13 +44 - CENTER_ADJUSTMENT

	subi YL,lo8(-(SCREEN_WIDTH/8))
	sbci YH,hi8(-(SCREEN_WIDTH/8))

	dec r20
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

	//deassert SPI
	sbi _SFR_IO_ADDR(PORTA),PA4
	
	clr r1

	ret


;*************************************************
; RENDER TILE LINE
;
; r20     = render line counter (decrementing)
; r17     = next pixel
;
; Must preserve r20,Y
; 
; cycles  = 1457
;*************************************************
render_tile_line:
	push YL
	push YH

	ldi r18,SCREEN_WIDTH/8

	nop
	nop
	ser r19

//	movw ZL,YL
//	lpm r16,Z+
//	ldi r16,0
	mov r16,r17
	lsl r16

main_loop:		
	sbc r0,r0	
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 0
	
	out _SFR_IO_ADDR(SPDR),r19		;start next pixel xfer	
	lsl r16
	sbc r1,r1	
	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 1	

	nop
	lsl r16
	sbc r2,r2		
	out _SFR_IO_ADDR(DATA_PORT),r2 ;pixel 2

	nop
	lsl r16
	sbc r3,r3	
	out _SFR_IO_ADDR(DATA_PORT),r3 ;pixel 3

	lsl r16
	sbc r0,r0	
	lsl r16
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 4

	sbc r1,r1
	lsl r16
	sbc r2,r2
	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 5	

	lsl r16
	sbc r3,r3
	in r17,_SFR_IO_ADDR(SPDR)
	out _SFR_IO_ADDR(DATA_PORT),r2 ;pixel 6
	
	mov r16,r17
	lsl r16	
	dec r18
	out _SFR_IO_ADDR(DATA_PORT),r3 ;pixel 7	
	brne main_loop

	clr r0
	out _SFR_IO_ADDR(DATA_PORT),r0
	
	pop YH
	pop YL
	ret


/*
render_tile_line_flash:
	push YL
	push YH

	ldi r18,SCREEN_WIDTH/8

	movw ZL,YL
	lpm r16,Z+
	lsl r16


main_loop_f:		
	sbc r0,r0	
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 0
	
	lsl r16
	sbc r1,r1
	lsl r16
	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 1	

	sbc r2,r2	
	lsl r16
	sbc r3,r3
	out _SFR_IO_ADDR(DATA_PORT),r2 ;pixel 2

	lpm r17,Z+	
	out _SFR_IO_ADDR(DATA_PORT),r3 ;pixel 3

	lsl r16
	sbc r0,r0	
	lsl r16
	out _SFR_IO_ADDR(DATA_PORT),r0 ;pixel 4

	sbc r1,r1
	lsl r16
	sbc r2,r2
	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 5	

	lsl r16
	sbc r3,r3
	mov r16,r17
	out _SFR_IO_ADDR(DATA_PORT),r2 ;pixel 6
	
	lsl r16
	nop
	dec r18
	out _SFR_IO_ADDR(DATA_PORT),r3 ;pixel 7	
	brne main_loop_f

	clr r0
	out _SFR_IO_ADDR(DATA_PORT),r0
	
	pop YH
	pop YL
	ret

*/


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
/*
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

	ld r24,Z+	;load byte mask
	ld r25,Z	;load shift multiplier

	and r22,r24	;mask byte
	mul r20,r25	;*shift multiplier (*1,*4,*16,*64)
	or r22,r0	;combine pixels
	st X,r22

	clr r1

PutPixel_out_of_screen:
	ret
*/
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
/*
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

	ld r22,Z+	;load byte mask
	ld r23,Z	;load shift multiplier
		
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
*/
;***********************************
; CLEAR VRAM 8bit
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearVram
ClearVram:
/*
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
*/
	ret
	
.section .text.SetVram
SetVram:
	sts vram,r24
	sts vram+1,r25
	ret

;Nothing to do in this mode
DisplayLogo:
VideoModeVsync:
InitializeVideoMode:
	ret

