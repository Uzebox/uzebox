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
; Video Mode 
; Bitmappep mode: 96x74 cells with a grid and two buffers
;****************************************************	


.global SetFontTilesIndex
.global SetTile
.global SetFont
.global SetTileTable
.global PutPixel
.global GetPixel
.global vram
.global palette
.global InitializeVideoMode
.global DisplayLogo
.global VideoModeVsync
.global ClearVram
.global ClearTextVram
.global vmode_page
.global vmode_grid_col
.global cursor_x
.global cursor_y
.global vmode
.global vmode_text_lines
.global __do_copy_data ;These must be present or .data/.bss may not be initialized correctly
.global __do_clear_bss ;see: https://savannah.nongnu.org/bugs/index.php?36124

.section .bss
	
	vram: 	  		.space VRAM_SIZE
	text_vram:		.space SCREEN_TILES_H*SCREEN_TILES_V
	tile_table_lo:	.space 1
	tile_table_hi:	.space 1
	font_tile_index:.space 1

.section .data

	vmode_text_lines: .byte SCREEN_TILES_V
	vmode:			.byte 0
	vmode_page:		.byte 0
	vmode_grid_col:	.byte 0
	cursor_x:		.byte 0
	cursor_y:		.byte 0
	cursor_delay:	.byte 0
	cursor_phase:	.byte 0
	curr_row:		.byte 0

.section .text

	write_masks:
			.byte 0b01111111
			.byte 0b10111111
			.byte 0b11011111
			.byte 0b11101111
			.byte 0b11110111
			.byte 0b11111011
			.byte 0b11111101
			.byte 0b11111110
	read_masks:
			.byte 0b11000000,4
			.byte 0b00110000,16
			.byte 0b00001100,64
			.byte 0b00000011,1


sub_video_mode:

	lds r16,vmode
	ldi r17,1
	cpse r16,r17
	rjmp sub_video_mode5

	WAIT r16,1350-6

	ldi r16,(SCREEN_HEIGHT*4)+2

sub_video_mode_split:
	mov r13,r16

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	lds r16,vmode_page
	sbrc r16,0
	subi YL,lo8(-(PAGE_SIZE))
	sbrc r16,0
	sbci YH,hi8(-(PAGE_SIZE))

	;flash cursor
	lds r25,cursor_x

	lds r16,cursor_delay
	inc r16
	andi r16,15
	sts cursor_delay,r16

	ldi r18,1
	lds r17,cursor_phase
	cpi r16,1
	brne .+2
	eor r17,r18
	sts cursor_phase,r17

	cpi r17,1
	brne .+2
	ser r25

	
	clr r20		;line counter
	clr r14		;cell row counter

;*************************************************************
; Rendering main loop starts here
;*************************************************************
m8_next_scan_line:
	rcall hsync_pulse 

	WAIT r19,262 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT -18

	;***draw line***
	rcall m8_render_tile_line

	WAIT r19,59 + 32 - 2 - CENTER_ADJUSTMENT

	;advance VRAM pointer by one pixel line
	;every 3 scanlines
	mov r21,r20
	andi r21,3
	cpi r21,3
	brne skip
	adiw YL,SCREEN_WIDTH/8
	inc r14
	rjmp updated
skip:
	lpm
	nop
updated:

	inc r20
	cp r20,r13 //(SCREEN_HEIGHT*4)+2
	brlo m8_next_scan_line

	nop
	rcall hsync_pulse ;145
	
	;set vsync flag & flip field
	lds ZL,sync_flags
	ldi r20,SYNC_FLAG_FIELD
	eor ZL,r20
	ori ZL,SYNC_FLAG_VSYNC
	sts sync_flags,ZL
			
	;clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	ret


;*************************************************
; RENDER TILE LINE
; r14     = current cell row (must not be modified)
; r20     = render line counter (incrementing,must not be modified)
; r25     = cursor x
; Y       = VRAM adress to draw from (must not be modified)
;
; Must preserve r13,r14,r20,r25,Y
; 
; cycles  = 1495
;*************************************************
m8_render_tile_line:
	movw XL,YL 	
	
	mov r16,r20
	ldi r18,3
	and r16,r18
	
	lds r21,vmode_grid_col //grid color
	
	mov r17,r21
	cpse r16,r18
	ldi r17,0    //dead cell color

	mov r19,r21
	cpse r16,r18
	ldi r19,0xff //living cell color

	mov r23,r21
	cpse r16,r18
	ldi r23,0b00000111 //0b11100000	//cursor color

	clr r24				//line pixel counter
	//lds r25,cursor_x
	mov r30,r25
	lds r15,cursor_y
	cp r15,r14			//cursor y is on current line?
	breq .+2
	ldi r30,0xff		//if not, put cursor x offscreen


	ldi r22,0	 //blank screen
	clr r2

	;15 cycles per pixel (96x80) @1440 cycles
	ldi r18,SCREEN_WIDTH/8
	ld r16,X+ ;load next 8 pixels
	mov r1,r17 //dead color
	sbrc r16,7
	mov r1,r19 //living color
	cp r24,r30
	brne .+2
	mov r1,r23 //cursor?

	out _SFR_IO_ADDR(DATA_PORT),r21 ;grid
	rjmp .
	inc r24
m8_loop:
	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 0
	mov r1,r17 //next pixel dead color
	sbrc r16,6
	mov r1,r19 //next pixel living color
	cp r24,r30
	brne .+2
	mov r1,r23
	lpm
	rjmp .
	out _SFR_IO_ADDR(DATA_PORT),r21 ;grid
	inc r24
	rjmp .
	
	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 1
	mov r1,r17 //next pixel dead color
	sbrc r16,5
	mov r1,r19 //next pixel living color
	cp r24,r30
	brne .+2
	mov r1,r23
	lpm
	rjmp .
	out _SFR_IO_ADDR(DATA_PORT),r21 ;grid
	inc r24
	rjmp .

	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 2
	mov r1,r17 //next pixel dead color
	sbrc r16,4
	mov r1,r19 //next pixel living color
	cp r24,r30
	brne .+2
	mov r1,r23
	lpm
	rjmp .
	out _SFR_IO_ADDR(DATA_PORT),r21 ;grid
	inc r24
	rjmp .

	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 3
	mov r1,r17 //next pixel dead color
	sbrc r16,3
	mov r1,r19 //next pixel living color
	cp r24,r30
	brne .+2
	mov r1,r23
	lpm
	rjmp .
	out _SFR_IO_ADDR(DATA_PORT),r21 ;grid
	inc r24
	rjmp .

	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 4
	mov r1,r17 //next pixel dead color
	sbrc r16,2
	mov r1,r19 //next pixel living color
	cp r24,r30
	brne .+2
	mov r1,r23
	lpm
	rjmp .
	out _SFR_IO_ADDR(DATA_PORT),r21 ;grid
	inc r24
	rjmp .

	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 5
	mov r1,r17 //next pixel dead color
	sbrc r16,1
	mov r1,r19 //next pixel living color
	cp r24,r30
	brne .+2
	mov r1,r23
	lpm
	rjmp .
	out _SFR_IO_ADDR(DATA_PORT),r21 ;grid
	inc r24
	rjmp .

	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 6
	mov r1,r17 //next pixel dead color
	sbrc r16,0
	mov r1,r19 //next pixel living color
	cp r24,r30
	brne .+2
	mov r1,r23
	lpm
	rjmp .
	out _SFR_IO_ADDR(DATA_PORT),r21 ;grid
	inc r24
	rjmp .

	out _SFR_IO_ADDR(DATA_PORT),r1 ;pixel 7
	ld r16,X+ ;load next 8 pixels
	mov r1,r17 //next pixel dead color
	sbrc r16,7
	mov r1,r19 //next pixel living color
	cp r24,r30
	brne .+2
	mov r1,r23
	nop
	inc r24
	dec r18
	out _SFR_IO_ADDR(DATA_PORT),r21 ;grid

	cpse r18,r2
	rjmp m8_loop
	
	nop
	out _SFR_IO_ADDR(DATA_PORT),r22 //blank screen




	ret




;**************************************************
;Plots a pixel at the specified location with the
;specified color (0-1).
;---------------------
;C-callable
;r24=X
;r22=Y
;r20=Color
;r18=Page
;***************************************************
//.section .text.PutPixel
PutPixel:
/*
	cpi r24,SCREEN_WIDTH
	brsh PutPixel_out_of_screen
	cpi r22,SCREEN_HEIGHT
	brsh PutPixel_out_of_screen

	;Compute vram base adress
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	sbrc r18,0
	subi XL,lo8(-(PAGE_SIZE))
	sbrc r18,0
	sbci XH,hi8(-(PAGE_SIZE))

	;compute vram pointer byte that contains the pixel
	mov r25,r24
	lsr r25
	lsr r25
	lsr r25
	mul r22,r25	; (X>>3)*Y
	
	;load 8 pixels
	ld r21,X

	andi r20,1
	andi r24,7

	cpi r20,1
	breq setpixel

clearpixel:
	ldi r25,0xfe	;mask


setpixel:		
	


	clr r1

PutPixel_out_of_screen:
	ret
*/


	cpi r24,SCREEN_WIDTH
	brsh PutPixel_out_of_screen
	cpi r22,SCREEN_HEIGHT
	brsh PutPixel_out_of_screen
	
	andi r20,1	;clip color

	;mov ZL,r24
	;clr ZH
	;andi ZL,7
	;subi ZL,lo8(-(write_masks))
	;sbci ZH,hi8(-(write_masks))
	ldi ZL,lo8(write_masks)
	ldi ZH,hi8(write_masks)
	mov r25,r24
	andi r25,7
	add ZL,r25
	adc ZH,r1

	;Compute vram base adress
	ldi r23,(SCREEN_WIDTH/8)
	mul r22,r23	;(SCREEN_WIDTH/8)*Y
	lsr r24
	lsr r24
	lsr r24		;X>>3
	clr r25
	add r24,r0
	adc r25,r1
	subi r24,lo8(-(vram))
	sbci r25,hi8(-(vram))
	movw XL,r24
	sbrc r18,0
	subi XL,lo8(-(PAGE_SIZE))
	sbrc r18,0
	sbci XH,hi8(-(PAGE_SIZE))

	ld r22,X	;load byte (8 pixels)
	lpm r24,Z	;load byte mask

	and r22,r24	;mask byte
	com r24
	mul r20,r24	;*shift multiplier (*1,*4,*16,*64)
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
//.section .text.GetPixel
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
	
;***********************************
; CLEAR VRAM 8bit
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearTextVram
ClearTextVram:
	//init vram
	ldi r30,lo8(TEXT_VRAM_SIZE)
	ldi r31,hi8(TEXT_VRAM_SIZE)

	ldi XL,lo8(text_vram)
	ldi XH,hi8(text_vram)

fill_vram_loop2:
	st X+,r1
	sbiw r30,1
	brne fill_vram_loop2

	clr r1

	ret

;Nothing to do in this mode
DisplayLogo:
VideoModeVsync:
InitializeVideoMode:
	ret

sub_video_mode5:

	;waste line to align with next hsync in render function
	WAIT ZL,1338

	ldi YL,lo8(text_vram)
	ldi YH,hi8(text_vram)

	lds r10,vmode_text_lines
	lsl r10
	lsl r10
	lsl r10 ;*8
	;total scanlines to draw
	;lds r10,render_lines_count

	clr r22
	ldi r23,TILE_WIDTH ;tile width in pixels

next_text_line:
	rcall hsync_pulse

	WAIT r19,261 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT

	;***draw line***
	call m5_render_tile_line

	ldi r19,16 - CENTER_ADJUSTMENT
	dec r19
	brne .-4


	dec r10
	breq text_frame_end

	lpm ;3 nop
	inc r22

	cpi r22,TILE_HEIGHT ;last char line? 1
	breq next_text_row

	;wait to align with next_tile_row instructions (+1 cycle for the breq)
	lpm ;3 nop
	lpm ;3 nop
	lpm ;3 nop
	nop

	rjmp next_text_line

next_text_row:
	clr r22		;current char line			;1

	clr r0
	ldi r19,VRAM_TILES_H
	add YL,r19
	adc YH,r0

	lpm
	nop

	rjmp next_text_line

text_frame_end:

	ldi r19,5
	dec r19
	brne .-4
	rjmp .

	rcall hsync_pulse ;145

	lds r17,vmode_text_lines
	cpi r17,SCREEN_TILES_V
	brsh no_split_screen

	ldi r16,(SCREEN_HEIGHT*4)+2
	lsl r17
	lsl r17
	lsl r17
	sub r16,r17

	WAIT ZL,1820-AUDIO_OUT_HSYNC_CYCLES-164
	WAIT ZL,101

	jmp sub_video_mode_split


no_split_screen:
	;set vsync flag & flip field
	lds ZL,sync_flags
	ldi r20,SYNC_FLAG_FIELD
	ori ZL,SYNC_FLAG_VSYNC
	eor ZL,r20
	sts sync_flags,ZL

	;clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	ret

;*************************************************
; Renders a line within the current tile row.
; Draws 40 tiles wide @ 6 clocks per pixel
;
; r22     = Y offset in tile row (0-7)
; r23 	  = tile width in bytes
; Y       = VRAM adress to draw from (must not be modified)
;
; cycles  = 1495
;*************************************************
m5_render_tile_line:

	movw XL,YL			;copy current VRAM pointer to X
	ldi r17,TILE_HEIGHT*TILE_WIDTH

	;////////////////////////////////////////////
	;Compute the adress of the first tile to draw
	;////////////////////////////////////////////
	lds r24,tile_table_lo
	lds r25,tile_table_hi
	mul r22,r23			;compute Y offset in current tile row
	add r24,r0			;add to base tileset adress
	adc r25,r1

	ld r20,X+			;load first tile index from VRAM
	mul r20,r17			;multiply tile index by tile size
	add r0,r24			;add tileset adress
	adc r1,r25
	movw ZL,r0	 		;copy to Z, the only register that can read from flash

	ldi r18,SCREEN_TILES_H ;load the number of horizontal tiles to draw

m5_loop:
	lpm r16,Z+			;get pixel 0 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	ld	r20,X+			;load next tile index from VRAM

	lpm r16,Z+			;get pixel 1 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	mul r20,r17			;multiply tile index by tile size

	lpm r16,Z+			;get pixel 2 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	movw r20,r24		;load tile table base adress+line offset
	add r20,r0

	lpm r16,Z+			;get pixel 3 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	adc r21,r1
	nop

	lpm r16,Z+			;get pixel 4 from flash
	out VIDEO_PORT,r16	;and output it to the video DAC

	lpm r16,Z+			;get pixel 5 from flash
	movw ZL,r20			;load the next tile's adress in Z
	dec r18				;decrement horizontal tiles to draw

	out VIDEO_PORT,r16	;and output it to the video DAC
	brne m5_loop


	rjmp .				;2 cycles delay
	nop					;1 cycle delay
	clr r16				;set last pixel to zero (black)
	out VIDEO_PORT,r16

	ret



;***********************************
; SET TILE 8bit mode
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; r20=Tile No (8 bit)
;************************************
.section .text.SetTile
SetTile:

	clr r25
	clr r23

	ldi r18,VRAM_TILES_H

	mul r22,r18		;calculate Y line addr in vram
	add r0,r24		;add X offset
	adc r1,r25
	ldi XL,lo8(text_vram)
	ldi XH,hi8(text_vram)
	add XL,r0
	adc XH,r1

	st X,r20

	clr r1

	ret

;***********************************
; SET FONT TILE
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; r20=Font tile No (8 bit)
;************************************
.section .text.SetFont
SetFont:
	clr r25

	ldi r18,VRAM_TILES_H

	mul r22,r18		;calculate Y line addr in vram

	add r0,r24		;add X offset
	adc r1,r25

	ldi XL,lo8(text_vram)
	ldi XH,hi8(text_vram)
	add XL,r0
	adc XH,r1

	lds r21,font_tile_index
	add r20,r21

	st X,r20

	clr r1

	ret


;***********************************
; SET FONT Index
; C-callable
; r24=First font tile index in tile table (8 bit)
;************************************
.section .text.SetFontTilesIndex
SetFontTilesIndex:
	sts font_tile_index,r24
	ret


;***********************************
; Define the tile data source
; C-callable
; r25:r24=pointer to tiles data
;************************************
.section .text.SetTileTable
SetTileTable:
	sts tile_table_lo,r24
	sts tile_table_hi,r25
	ret

