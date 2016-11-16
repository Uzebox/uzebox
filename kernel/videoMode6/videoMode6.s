/*
 *  Uzebox Kernel - Mode 6
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
; Video Mode 6
; 240x224
; Monochrome
; Use Ram tiles (up to 256)
;***************************************************	

.global DisplayLogo
.global VideoModeVsync
.global InitializeVideoMode
.global vram
.global ramTiles
.global SetPixel2
.global Line2
.global ClearBuffer
.global nextFreeRamTile
.global SetBackgroundColor
.global SetForegroundColor
.global SetHsyncCallback
.global SetUserRamTileIndex
.global SetFontTilesIndex
.global SetTileTable
.global SetFont
.section .bss
.align 3
	shift_tbl_ram:	.space 8
.align 1
	vram: 	  		.space VRAM_SIZE 
	ramTiles:		.space RAM_TILES_COUNT*TILE_HEIGHT ;8 pixels per bytes
	nextFreeRamTile:.space 1
	userRamTileIndex:.space 1
	currentLine:	.space 1
	fg_color:		.space 1
	bg_color:		.space 1
	tile_table_lo:			.space 1
	tile_table_hi:			.space 1
	font_tile_index:		.space 1
	hsync_user_callback:  .space 2 ;pointer to function

.section .text

shift_tbl:
	.byte 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01

sub_video_mode6:

	;waste cycles to align with next hsync in render function
	WAIT r19,1342+3

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	;total scanlines to draw
	lds r10,render_lines_count

	clr r22
	sts currentLine,r22
	;lds r4,cyc
	;inc r4
	;sts cyc,r4
	clr r5

next_text_line:	
	rcall hsync_pulse 

	;process user hsync callback
	lds ZL,hsync_user_callback+0
	lds ZH,hsync_user_callback+1
	icall	;callback must take exactly 32 cycles

	WAIT r19,213 - AUDIO_OUT_HSYNC_CYCLES

	;***draw line***
	call render_tile_line_hires ;render_tile_line
	
	WAIT r19,52-5

	lds r19,currentLine
	inc r19
	sts currentLine,r19

	inc r4
	inc r5

	dec r10
	breq text_frame_end
	
	lpm ;3 nop
	inc r22

	cpi r22,8 ;last char line? 1
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

	WAIT r19,17

	rcall hsync_pulse ;145
	

text_end2:

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
; RENDER TILE LINE
;
; r22     = Y offset in tiles
; Y       = VRAM adress to draw from (must not be modified)
;
; Must preserve: r10,r22,Y
; 
; cycles  = 1495
;*************************************************
render_tile_line:

	movw XL,YL

	ldi r23,TILE_HEIGHT	;bytes per tile

	;compute ramtiles table base adress
	clr r0
	ldi r24,lo8(ramTiles)
	ldi r25,hi8(ramTiles)
	add r24,r22				;add Y offset in tiles
	adc r25,r0

	;load the first 8 pixels
	ld	r20,X+	;load tile index from VRAM
	mul r20,r23
	movw ZL,r24
	add ZL,r0
	adc ZH,r1
	ld r17,Z

	ldi r18,SCREEN_TILES_H
	lds r20,bg_color
	lds r21,fg_color

m6_loop:
	mov r2,r20
	sbrc r17,7
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	ld r16,X+		;load next tile

	mov r2,r20
	sbrc r17,6
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	mul r16,23

	mov r2,r20
	sbrc r17,5
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	add r0,r24
	adc r1,r25

	mov r2,r20
	sbrc r17,4
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	movw ZL,r0
	mov r19,r17

	mov r2,r20
	sbrc r19,3
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	ld r17,Z			;load next 8 pixels

	mov r2,r20
	sbrc r19,2
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	rjmp .	

	mov r2,r20
	sbrc r19,1
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	nop
	dec r18
	
	mov r2,r20
	sbrc r19,0
	mov r2,	r21
	out _SFR_IO_ADDR(DATA_PORT),r2
	brne m6_loop

	//clear last pixel
	lpm 
	clr r0
	out _SFR_IO_ADDR(DATA_PORT),r0


	ret

//high resolution
render_tile_line_hires:

	movw XL,YL

	ldi r23,8	;bytes per tile

	;compute ramtiles table base adress
	clr r0
	ldi r24,lo8(ramTiles)
	ldi r25,hi8(ramTiles)
	add r24,r22				;add Y offset in tiles
	adc r25,r0

	;load the first 8 pixels
	ld	r20,X+	;load tile index from VRAM
	mul r20,r23
	movw ZL,r24
	add ZL,r0
	adc ZH,r1
	ld r17,Z

	ldi r18,SCREEN_TILES_H
	lds r20,bg_color
	lds r21,fg_color

1:
	;mov r2,r20
	;sbrc r17,7
	;mov r2,	r21
	rol r17
	sbc r2,r2
	out _SFR_IO_ADDR(DATA_PORT),r2	;pix 0
	ld r16,X+		;load next tile

;	mov r2,r20
;	sbrc r17,6
;	mov r2,	r21
	rol r17
	sbc r2,r2
	out _SFR_IO_ADDR(DATA_PORT),r2	;pix 1
	mul r16,23

;	mov r2,r20
;	sbrc r17,5
;	mov r2,	r21
	rol r17
	sbc r2,r2
	out _SFR_IO_ADDR(DATA_PORT),r2	;pix 2
	add r0,r24
	adc r1,r25

;	mov r2,r20
;	sbrc r17,4
;	mov r2,	r21
	rol r17
	sbc r2,r2
	out _SFR_IO_ADDR(DATA_PORT),r2	;pix 3
	movw ZL,r0
	mov r19,r17

;	mov r2,r20
;	sbrc r19,3
;	mov r2,	r21
	rol r19
	sbc r2,r2
	out _SFR_IO_ADDR(DATA_PORT),r2	;pix 4
	ld r17,Z			;load next 8 pixels

;	mov r2,r20
;	sbrc r19,2
;	mov r2,	r21
	rol r19
	sbc r2,r2
	out _SFR_IO_ADDR(DATA_PORT),r2	;pix 5
	rjmp .	

;	mov r2,r20
;	sbrc r19,1
;	mov r2,	r21
	rol r19
	sbc r2,r2
	out _SFR_IO_ADDR(DATA_PORT),r2	;pix 6
	nop

;	mov r2,r20
;	sbrc r19,0
;	mov r2,	r21
	rol r19
	sbc r2,r2
	dec r18
	out _SFR_IO_ADDR(DATA_PORT),r2	;pix 7
	brne 1b

	//clear last pixel
	lpm 
	clr r0
	out _SFR_IO_ADDR(DATA_PORT),r0


	ret

/*
	Sets a pixel for the line function

   	r24 = X
   	r22 = Y

	;worst: 63 cycles
	;best: 27 cycles

   	Regs:
   	-----
   	Using r20-r31 internally
   	X=vram
   	r23= current vram tile   
	r18:r19=previous tile positions
*/
SetPixel2:
	mov r25,r22
	movw r20,r24	;save x,y	

	;check if we are in the same 
	;ram tile # as the previous pixel
;	andi r24,0xf8
;	andi r25,0xf8
;	cp   r24,r18
;	cpc  r25,r19
;	breq same_tile		
;	movw r18,r24	;save 

	lsr r25 
	lsr r25 
	lsr r25 
	lsr r24
	lsr r24
	lsr r24
	ldi XL,SCREEN_TILES_H
	mul r25,XL
	movw XL,r0
	clr r1
	add XL,r24				;+(px>>3)
	adc XH,r1
	subi XL,lo8(-(vram))
	sbci XH,hi8(-(vram)) 	;+vram[]	
	ld r23,X				;load c	
	cpi r23,0
	brne allocated

	lds r23,nextFreeRamTile	;load c
	cpi r23,(RAM_TILES_COUNT-1)
	brsh end_noram	//no more ramtiles

	st X,r23 	;	vram[addr]=c
	mov r0,r23
	inc r0
	sts nextFreeRamTile,r0

allocated:
	ldi r22,TILE_WIDTH
	mul r23,r22		;c*8
	movw XL,r0
	clr r1
	subi XL,lo8(-(ramTiles))
	sbci XH,hi8(-(ramTiles))

same_tile:	
	andi r20,7	;x&7
	ldi ZL,lo8(shift_tbl_ram)
	ldi ZH,hi8(shift_tbl_ram)
	add ZL,r20
	ld r20,Z	;(0x80>>(px&7))
	
	movw ZL,XL

	andi r21,7	;y&7
	add ZL,r21
	adc ZH,r1	;0
	ld r0,Z	;load 8 pixels
	or r0,r20
	st Z,r0	;write pixels

end_noram:
	ret



/*
 * Clears the video buffer
 */
ClearBuffer:

	ldi XL,lo8(vram+(36*6))
	ldi XH,hi8(vram+(36*6))

.rept SCREEN_TILES_H*SCREEN_TILES_V
	st X+,r1	
.endr

	ldi ZL,lo8(ramTiles+8)
	ldi ZH,hi8(ramTiles+8)
	lds r24,nextFreeRamTile
	cpi r24,1
	breq no_clear

clr_loop:			
	st Z+,r1	;clear ram tile
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	st Z+,r1
	dec r24
	cp r24,1
	brne clr_loop

no_clear:
	ldi r24,1
	sts nextFreeRamTile,r24
	ret

/*
  Fast pixels macro to use inline
  with the line function. 
  
  
  FAST_PIXEL=Optimized for arbitrary plotting
  FAST_VPIXEL=optimized for vertical lines
  FAST_HPIXEL=optimized for horizontal lines
  
  position.

 	;worst: 58 cycles
	;best: 22 cycles

   	Regs:
   	-----
	r24=x
	r25=y
   	r23=current vram tile   
	r18:r19=previous tile positions
	r8=TILE_WIDTH
 */
.macro FAST_PIXEL
	movw r20,r24	;save x,y	

	;check if we are in the same 
	;ram tile # as the previous pixel
	andi r24,0xf8
	andi r25,0xf8
	cp   r24,r18
	cpc  r25,r19
	breq 3f			;jmp same_tile	
	movw r18,r24	;save 

	lsr r25 
	lsr r25 
	lsr r25 
	lsr r24
	lsr r24
	lsr r24
	ldi XL,SCREEN_TILES_H
	mul r25,XL
	movw XL,r0
	clr r1
	add XL,r24				;+(px>>3)
	adc XH,r1
	subi XL,lo8(-(vram))
	sbci XH,hi8(-(vram)) 	;+vram[]	
	ld r23,X				;load c	
	cpi r23,0
	brne 2f					;jmp allocated

	lds r23,nextFreeRamTile	;load c
	cpi r23,(RAM_TILES_COUNT-1)
	brlo 1f			;jmp more_tiles
	movw r24,r20	;restore x,y
	rjmp 4f			;no more ramtiles, jmp end

1:	;more_tiles
	st X,r23 	;	vram[addr]=c
	mov r0,r23
	inc r0
	sts nextFreeRamTile,r0

2:	;allocated
	mul r23,r8		;c*8
	movw XL,r0
	clr r1
	subi XL,lo8(-(ramTiles))
	sbci XH,hi8(-(ramTiles))

3:	;same_tile
	movw r24,r20	;restore x,y

	andi r20,7	;x&7
	movw ZL,r12	;addr=shift_tbl_ram
	add ZL,r20
	ld r20,Z	;(0x80>>(px&7))

	movw ZL,XL

	andi r21,7	;y&7
	add ZL,r21	;addr=(c*8)+(py&7);
	adc ZH,r1	;0
	ld r0,Z	;load 8 pixels
	or r0,r20
	st Z,r0	;write pixels

4:	;end

.endm

.macro VFAST_PIXEL
	movw r20,r24	;save x,y	

	;check if we are in the same 
	;ram tile # as the previous pixel
	andi r24,0xf8
	andi r25,0xf8
	cp   r24,r18
	cpc  r25,r19
	breq 3f			;jmp same_tile	
	movw r18,r24	;save 

	lsr r25 
	lsr r25 
	lsr r25 
	lsr r24
	lsr r24
	lsr r24
	ldi XL,SCREEN_TILES_H
	mul r25,XL
	movw XL,r0
	clr r1
	add XL,r24				;+(px>>3)
	adc XH,r1
	subi XL,lo8(-(vram))
	sbci XH,hi8(-(vram)) 	;+vram[]	
	ld r23,X				;load c	
	cpi r23,0
	brne 2f					;jmp allocated

	lds r23,nextFreeRamTile	;load c
	cpi r23,(RAM_TILES_COUNT-1)
	brlo 1f			;jmp more_tiles
	movw r24,r20	;restore x,y
	rjmp 4f			;no more ramtiles, jmp end

1:	;more_tiles
	st X,r23 	;	vram[addr]=c
	mov r0,r23
	inc r0
	sts nextFreeRamTile,r0

2:	;allocated
	mul r23,r8		;c*8
	movw XL,r0
	clr r1
	subi XL,lo8(-(ramTiles))
	sbci XH,hi8(-(ramTiles))

3:	;same_tile
	movw r24,r20	;restore x,y

	andi r20,7	;x&7
	movw ZL,r12	;addr=shift_tbl_ram
	add ZL,r20
	adc ZH,r1	;0
	ld r20,Z	;(0x80>>(px&7))

	movw ZL,XL

	andi r21,7	;y&7
	add ZL,r21	;addr=(c*8)+(py&7);
	ld r0,Z	;load 8 pixels
	or r0,r20
	st Z,r0	;write pixels

4:	;end

.endm

/*
 * r24=x1
 * r22=y1
 * r20=x2
 * r18=y2
 */
Line2:
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

	;save for use by h/v line 
	;mov r26,r20
	;mov r27,r18
	
	;dx=x2-x1;     //the horizontal distance of the line
	;sdx=dx>=0?1:-1;
	;dxabs=abs(dx);  
	sub r20,r24 ;dx
	sbc r1,r1	;sign extend
	mov r15,r20 ;dxabs (r25)
	ldi r16,1	;sdx=1
	brpl .+4
	neg r15		;abs(dx)
	neg r16		;sdx=-1
	
	;dy=y2-y1;      //the vertical distance of the line
    ;sdy=dy>=0?1:-1;
    ;dyabs=abs(dy);
	sub r18,r22 ;dy
	sbc r1,r1  	;sign extend
	mov r14,r18 ;dyabs
	ldi r17,1	;sdy=1
	brpl .+4
	neg r14		;abs(dy)
	neg r17		;sdy=-1
	
	;check if line is 1 pixel
	cp r15,r1
	cpc r14,r1
	brne 1f
	rcall SetPixel2
	rjmp line_end	
1:

	mov r10,r14
	lsr r10		;x=dyabs>>1;
	mov r11,r15
	lsr r11		;y=dxabs>>1;

	ldi r18,lo8(shift_tbl_ram)
	ldi r19,hi8(shift_tbl_ram)
	movw r12,r18
	ldi r18,TILE_WIDTH
	mov r8,r18
	
	;x=r24
	;y=r22
	mov r25,r22
	ser r18		;last pixel x
	ser r19		;last pixel y
	
	;tst r15		;test if line is 100% vertical
	;brne .+2 
	;rjmp vert_line
	;tst r14		;test if line is 100% horizontal
	;brne .+2
	;rjmp horz_line

	;if (dxabs>=dyabs) // the line is more horizontal than vertical
	cp r15,r14
	//brlo more_vertical
	brsh .+2
	rjmp more_vertical

	;more horizontal	
	mov r9,r15	;for(i=0;i<dxabs;i++)
horz_loop:	
	add r11,r14	;y+=dyabs;	
	cp r11,r15	;if(y>=dxabs)
	brlo noyupdate
	sub r11,r15	;y-=dxabs;
	add r25,r17	;py+=sdy;	
noyupdate:
	add r24,r16 ;px+=sdx;
	FAST_PIXEL
	dec r9
	brne horz_loop	
	rjmp line_end

more_vertical:		
	mov r9,r14	;for(i=0;i<dyabs;i++)
vert_loop:	
	add r10,r15	;x+=dxabs;	
	cp r10,r14	;if(x>=dyabs)
	brlo noxupdate
	sub r10,r14	;x-=dyabs;
	add r24,r16	;px+=sdx;
noxupdate:
	add r25,r17 ;py+=sdy;
	FAST_PIXEL
	dec r9
	brne vert_loop				


line_end:
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
	ret

vert_line:
	;r17=sdy
	;r14=dyabs
	;r24=x
	;r25=y
	;r26=x2
	;r27=y2


/*
	mov r9,r14
vl_loop:
	add r25,r17 ;py+=sdy;
	FAST_PIXEL
	dec r9
	brne vl_loop	
*/

	push YL
	push YH

	cp r25,r27
	brlo noyswap
	mov r0,r25
	mov r25,r27
	mov r27,r0
noyswap:

	;compute first ramtile offset
	movw r20,r24	
	;get pixel mask
	andi r20,7	;x&7
	movw ZL,r12	;addr=shift_tbl_ram
	add ZL,r20
	ld r20,Z	;(0x80>>(px&7))
	andi r21,7	;y&7

	lsr r25 
	lsr r25 
	lsr r25 
	lsr r24
	lsr r24
	lsr r24
	ldi XL,SCREEN_TILES_H
	mul r25,XL
	movw XL,r0
	clr r1
	add XL,r24				;+(px>>3)
	adc XH,r1
	subi XL,lo8(-(vram))
	sbci XH,hi8(-(vram)) 	;+vram[]	
3:
	ld r23,X				;load c	
	cpi r23,0
	brne 2f					;jmp allocated

	lds r23,nextFreeRamTile	;load c
	cpi r23,(RAM_TILES_COUNT-1)
	brlo 1f			;jmp more_tiles
	movw r24,r20	;restore x,y
	rjmp 4f			;no more ramtiles, jmp end

1:	;more_tiles
	st X,r23 	;	vram[addr]=c
	mov r0,r23
	inc r0
	sts nextFreeRamTile,r0

2:	;allocated
	mul r23,r8		;c*8
	movw YL,r0
	clr r1
	subi YL,lo8(-(ramTiles))
	sbci YH,hi8(-(ramTiles))
	add YL,r21	;addr=(c*8)+(py&7);
	adc YH,r1	;0

	ldi r22,TILE_HEIGHT
	sub r22,r21 ;-(py&7)
	cp r22,r14	;(8-(y&7))<dyabs?
	brlo .+2
	mov r22,r14	

	sub r14,r22	;pixels left to draw
4:
	ld r0,Y		;load 8 pixels
	or r0,r20	;set pixel
	st Y+,r0	;write 8 pixels
	dec r22
	brne 4b

	adiw XL,SCREEN_TILES_H
	clr r21
	tst r14		;more pixels?
	brne 3b

	pop YH
	pop YL

	rjmp line_end

horz_line:
	;r16=sdx
	;r15=dxabs
	mov r9,r15
hl_loop:
	add r24,r16	;px+=sdx;
	FAST_PIXEL
	dec r9
	brne hl_loop	
	rjmp line_end

;Nothing to do in this mode
DisplayLogo:
VideoModeVsync:
	ret

InitializeVideoMode:
	ldi r24,1
	sts nextFreeRamTile,r24
	ldi r24,0xff
	sts fg_color,r24
	sts bg_color,r1

	ldi r24,lo8(pm(DefaultCallback))
	sts hsync_user_callback+0,r24
	ldi r24,hi8(pm(DefaultCallback))
	sts hsync_user_callback+1,r24

	;copy rom table to ram
	ldi ZL,lo8(shift_tbl)
	ldi ZH,hi8(shift_tbl)
	ldi XL,lo8(shift_tbl_ram)
	ldi XH,hi8(shift_tbl_ram)
	ldi r24,8
init_cp_loop:	
	lpm r25,Z+
	st X+,r25
	dec r24
	brne init_cp_loop

	ret

.section .text.SetBackgroundColor
SetBackgroundColor:
	sts bg_color,r24	
	ret

.section .text.SetForegroundColor
SetForegroundColor:
	sts fg_color,r24
	ret

.section .text.SetUserRamTileIndex
SetUserRamTileIndex:
	sts userRamTileIndex,r24
	ret

;****************************
; Sets a callback that will be invoked during HBLANK 
; before rendering each line.
; C callable
; r25:r24 - pointer to C function: void ptr*(void)
;****************************
.section .text.SetHsyncCallback
SetHsyncCallback:
	sts hsync_user_callback+0,r24
	sts hsync_user_callback+1,r25
	ret

;must take exactly 32 cycles including the ret
DefaultCallback:
	WAIT r18,28
	ret


.global mycallback
;C-callable
;must take exactly 32 cycles including the ret
mycallback:
/*
	ldi XL,lo8(bgcolors)
	ldi XH,hi8(bgcolors)
	lds r0,currentLine
	add XL,r0
	adc XH,r1
	ld r24,X	
	sts bg_color,r24	

	ldi XL,lo8(fgcolors)
	ldi XH,hi8(fgcolors)
	lds r0,currentLine
	add XL,r0
	adc XH,r1
	ld r24,X	
	sts fg_color,r24	

	WAIT r18,8
*/
	ldi XL,lo8(bgcolors)
	ldi XH,hi8(bgcolors)
	lds r0,currentLine
	add XL,r0
	adc XH,r1
	ld r24,X	
	out _SFR_IO_ADDR(DDRC),24

	WAIT r24,19

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

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	lds r21,font_tile_index
	add r20,r21

	st X,r20

	clr r1

	ret
