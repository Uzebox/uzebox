/*
 *  Uzebox Kernel - Mode 9
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

;***************************************************
; Video mode 9
; Real-time code generated tile data
; Sub mode 1: 360x240, tiles only, 60x28 tiles, 6x8 pixels tile, 256 colors per pixel (-DRESOLUTION=60)
; Sub mode 2: 480x240, tiles only, 80x28 tiles, 6x8 pixels, 2 colors per pixel (-DRESOLUTION=80)
; no scrolling, no sprites
; Includes cursor functions and windowing for vertical scolling
;***************************************************

.global vram
.global codetiles_table
.global SetTile
.global ClearVram
.global SetFontTilesIndex
.global GetFontTilesIndex
.global SetTileTable
.global SetTile
.global GetTile
.global SetFont
.global MoveCursor
.global SetCursorVisible
.global SetCursorParams
.global backgroundColor
.global foregroundColor
.global SetVerticalScroll
.global GetVerticalScroll
.global VerticalScrollUp
.global SetForegroundColor
.global ClearLine
.global GetRowVramAddress
.global VerticalScrollDown


.section .bss
	vram: 	  		.space VRAM_SIZE	;allocate space for the video memory (VRAM)
	screen_line:	.space 1
	vertical_scroll:.space 1
	tile_table_lo:	.space 1
	tile_table_hi:	.space 1
	font_tile_index:.space 1
	backgroundColor:.space VRAM_TILES_V ;allocate 1 color per row
	foregroundColor:.space 1
	cursor_x:		.space 1
	cursor_y:		.space 1
	cursor_visible:	.space 1
	cursor_tile:	.space 1
	cursor_speed:	.space 1
	cursor_current_delay: .space 1
	cursor_state:	.space 1


.section .text

sub_video_mode9:

	;waste line to align with next hsync in render function
	WAIT r19,1342-20-1-5-2

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)
	lds r15,vertical_scroll	;current Y tile

	//compute adress of first tile row to draw
	ldi r16,VRAM_TILES_H
	mul r16,r15
	add YL,r0
	adc YH,r1

	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16
	clr r22
	sts screen_line,r22

	ldi XL,lo8(backgroundColor)
	ldi XH,hi8(backgroundColor)
	ld  r2,X+	;load background color for current text line
	lds r3,foregroundColor //only for 80 col mode

	//process cursor
	lds r5,cursor_current_delay
	lds r6,cursor_speed
	lds r7,cursor_state
	ldi r16,1
	clr r0

	inc r5
	cp r5,r6
	brne 1f
	clr r5
1:
	sts cursor_current_delay,r5
	cp r5,r0
	brne 1f
	eor r7,r16
1:
	sts cursor_state,r7



next_text_line:
	rcall hsync_pulse

	WAIT r19,HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT - 30 - 2

	;***draw line***
	call render_tile_line

	WAIT r19,48 + ((RESOLUTION-SCREEN_TILES_H)*TILE_WIDTH*CYCLES_PER_PIXEL) - CENTER_ADJUSTMENT - 12

	dec r10
	breq text_frame_end

	lpm ;3 nop
	inc r22

	cpi r22,8 ;last char line? 1
	breq next_text_row

	;wait to align with next_tile_row instructions (+1 cycle for the breq)
	WAIT r19,22

	rjmp next_text_line

next_text_row:
	clr r22		;current char line			;1

	clr r0
	ldi r19,VRAM_TILES_H
	add YL,r19
	adc YH,r0

	ld r2,X+	;get next row color
	inc r15

	;wrap back to start of vram if required
	ldi ZL,lo8(vram)
	ldi ZH,hi8(vram)
	ldi r19,SCREEN_TILES_V
	cp r15,r19	;reached the last line?
	brne 1f
	movw YL,ZL
1:
	brne 1f
	clr r15
1:

	lds r19,screen_line
	inc r19
	sts screen_line,r19

	rjmp next_text_line

text_frame_end:

	WAIT r19,29

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

	ret

;*************************************************
; Renders a line within the current tile row.
; Draws 40 tiles wide @ 6 clocks per pixel
;
; r10	  = total scanlines to draw
; r15	  = Current tile row
; r16,r17 = destroyed by code tiles
; r22     = Y offset in tile row (0-7)
; Y       = VRAM adress to draw from (must not be modified)
;
; cycles  = 1495
;*************************************************
render_tile_line:
	push YL
	push YH

   	ldi r18,lo8(pm(render_tile_line_end))
	ldi r19,hi8(pm(render_tile_line_end))
	ldi r21,FONT_TILE_SIZE ;size of a tile in words
	ldi r23,FONT_TILE_WIDTH ;size of tile row in words


	;////////////////////////////////////////////
	;Compute if we must draw the cursor on this line
	;////////////////////////////////////////////
	lds r11,cursor_tile
	lds r12,cursor_visible
	lds r13,cursor_x
	lds r14,cursor_y
	lds r16,cursor_state
	lds r0,	screen_line
	ldi r17,1

	cpse r14,r0		;is cursor on this row?
	clr r12			;cursor not visible
	cpse r16,r17
	clr r12

	clr r0
	movw r8,YL		;save vram pointer
	add r8,r13
	adc r9,r0
	movw ZL,r8
	ld r7,Z			;load tile at cursor and backup
	mov r6,r7		;if cursor not active at this line, use current tile
	cpse r12,r0
	mov r6,r11
	st Z,r6			;store cursor tile in vram


	;////////////////////////////////////////////
	;Compute the adress of the first tile to draw
	;////////////////////////////////////////////
	lds r24,tile_table_lo
	lds r25,tile_table_hi
   	lsr r25				;divide by 2 because we point to a program adress
	ror r24

	mul r22,r23			;compute Y offset in current tile row
	add r24,r0			;add to title table base adress
	adc r25,r1

	ld	r20,Y+			;load tile index from VRAM
	mul r20,r21
	add r0,r24			;add tile table base address+offset
	adc r1,r25

	movw ZL,r0	 		;copy to Z, the register used by ijmp
#if RESOLUTION==80
	clr r4				;black pixel for end of line
#endif

    ldi r20,SCREEN_TILES_H ;tiles to render
	ijmp      ;jump to first codetile

render_tile_line_end:
#if RESOLUTION==60
   	clr r4
#endif
   	out VIDEO_PORT,r4

	//restore tile under cursor
	movw ZL,r8
	st Z,r7


	pop YH
	pop YL
	ret

/*////////////////////////////////////////////////////////////////////////////////////////////////////
	Generated assembly code for code tiles (80) (pixel color control if r2 (bg) or r3 (fg) is assembled):
  ////////////////////////////////////////////////////////////////////////////////////////////////////
	out 0x08,r2
	ld	r17, Y+

	out 0x08,r3
	mul	r17, r21

	out 0x08,r2
	add	r0, r24
	adc	r1, r25

	out 0x08,r3
	movw r30, r18
	dec	r20

	out 0x08,r2
	breq	.+2
	movw	r30, r0

	out 0x08,r3
	ijmp
	*/

;***********************************
; CLEAR VRAM 8bit
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearVram
ClearVram:
	//init vram
//	ldi r30,lo8(VRAM_SIZE)
//	ldi r31,hi8(VRAM_SIZE)

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)

fill_vram_loop:

.rept VRAM_SIZE
	st X+,r1
.endr
;	sbiw r30,1
;	brne fill_vram_loop

;	clr r1

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
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	st X,r20

	clr r1

	ret



;***********************************
; Get TILE index 8bit mode
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; Returns:
; r24=Tile No (8 bit)
;************************************
.section .text.GetTile
GetTile:

	clr r25
	clr r23

	ldi r18,VRAM_TILES_H

	mul r22,r18		;calculate Y line addr in vram
	add r0,r24		;add X offset
	adc r1,r25
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	ld r24,X

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

	lds r0,vertical_scroll
	add r22,r0
	cpi r22,VRAM_TILES_V
	brlo 1f
	subi r22,VRAM_TILES_V
1:
	mul r22,r18		;calculate Y line addr in vram

	add r0,r24		;add X offset
	adc r1,r25

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	lds r21,font_tile_index
	add r20,r21

	st X,r20

	clr r1

	ret

;***********************************
; Move Cursor
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
;************************************
.section .text.MoveCursor
MoveCursor:
	cpi r24,SCREEN_TILES_H
	brlo 1f
	ldi r24,SCREEN_TILES_H-1
1:
	sts cursor_x,r24

	cpi r22,SCREEN_TILES_V
	brlo 1f
	ldi r22,SCREEN_TILES_V-1
1:
	sts cursor_y,r22
	ret


;***********************************
; Set Cursor visible
; C-callable
; r24=visible. 0=off, 1=on
;************************************
.section .text.SetCursorVisible
SetCursorVisible:
	sts cursor_visible,r24
	ret


;***********************************
; Set Cursor params
; C-callable
; r24=cursor tile
; r22=cursor speed. Frames to wait between blinks
;************************************
.section .text.SetCursorParams
SetCursorParams:
	sts cursor_tile,r24
	sts cursor_speed,r22
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
; Get FONT Index
; C-callable
; Return r24=First font tile index in tile table (8 bit)
;************************************
.section .text.GetFontTilesIndex
	GetFontTilesIndex:
	lds r24,font_tile_index
	clr r25
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

;***********************************
; Set the vertical scroll position in vram
; C-callable
; 24=line
;************************************
.section .text.SetVerticalScroll
SetVerticalScroll:
	sts vertical_scroll,r24
	ret

;**********************************
; Get the vertical scroll position in vram
; C-callable
; ; Returns:
; r24=vertical scroll offset in memory (8 bit)
;************************************
.section .text.GetVerticalScroll
GetVerticalScroll:
	lds r24, vertical_scroll
	ret


;**********************************
; Set the text color (foreground color)
; C-callable
; r24=color index
;************************************
.section .text.SetForegroundColor
SetForegroundColor:
	sts foregroundColor,r24
	ret

;***********************************
; Scrolls the screen up by one row
; C-callable
; 24=boolean: clear the new row
;************************************
.section .text.VerticalScrollUp
VerticalScrollUp:

	lds r25, vertical_scroll
	mov r23,r25
	inc r25
	cpi r25,SCREEN_TILES_V
	brlo 1f
	clr r25
1:
	sts vertical_scroll,r25

	cpi r24,0
	breq 2f

	ldi r24,SCREEN_TILES_V-1
	ldi r22,0
	ldi r20,SCREEN_TILES_H-1
	rcall ClearLine

	clr r1
	ret

;***********************************
; Scrolls the screen by one row
; C-callable
; 24=boolean: clear the new row
;************************************
.section .text.VerticalScrollDown
VerticalScrollDown:

	lds r25, vertical_scroll
	mov r23,r25

	dec r25
	brpl 1f
	ldi r25,SCREEN_TILES_V-1
1:
	sts vertical_scroll,r25

	cpi r24,0
	breq 2f

	ldi r24,0
	ldi r22,0
	ldi r20,SCREEN_TILES_H-1
	rcall ClearLine

	clr r1
	ret

;***********************************
; Patially clears a line in video memory
; C-callable
; r24=row
; r22=start column
; r20=end column
;************************************
.section .text.ClearLine
ClearLine:

	sub r20,r22
	inc r20

	ldi r18,VRAM_TILES_H

	lds r0,vertical_scroll
	add r24,r0
	cpi r24,VRAM_TILES_V
	brlo 1f
	subi r24,VRAM_TILES_V
1:
	mul r24,r18		;calculate Y line addr in vram

	clr r23
	add r0,r22		;add X offset
	adc r1,r23

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

2:
	st X+,r23
	dec r20
	brne 2b

	clr r1
	ret

;***********************************
; Calculate the vram adress at the specified
; row accounting for scrolling.
; C-callable
; r24= row (y)
;
; returns:
; r24:r25 = VRAM address
;************************************
.section .text.GetRowVramAddress
GetRowVramAddress:

	ldi r18,VRAM_TILES_H

	lds r0,vertical_scroll
	add r24,r0
	cpi r24,VRAM_TILES_V
	brlo 1f
	subi r24,VRAM_TILES_V
1:
	mul r24,r18		;calculate Y line addr in vram

	clr r23

	ldi r24,lo8(vram)
	ldi r25,hi8(vram)
	add r24,r0
	adc r25,r1

	clr r1
	ret



