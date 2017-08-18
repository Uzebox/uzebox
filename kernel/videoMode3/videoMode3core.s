/*
 *  Uzebox Kernel - Mode 3
 *  Copyright (C) 2009 Alec Bourque
 *                2017 Sandor Zsuga (Jubatian)
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
; Video Mode 3: 28x28 (224x224 pixels) using 8x8 tiles
; with overlay & sprites X flipping.
;
; If the SCROLLING build parameter=0, the scrolling 
; code is removed and the screen resolution 
; increases up to 30*28 tiles.
; 
; For compile time switch and information relating
; to this video mode see:
; http://uzebox.org/wiki/index.php?title=Video_Mode_3
;
;***************************************************	

.global vram
.global ram_tiles
.global ram_tiles_restore
.global free_tile_index
.global user_ram_tiles_c
.global user_ram_tiles_c_tmp
.global sprites
.global overlay_vram
.global sprites_tile_banks
.global Screen
.global SetSpritesTileTable
.global CopyFlashTile
.global CopyRamTile
.global SetSpritesTileBank
.global SetTile
.global ClearVram
.global SetFontTilesIndex
.global SetTileTable
.global SetTile
.global RestoreBackground
.global BlitSprite
.global SetFont
.global GetTile

;Screen Sections Struct offsets
#define scrollX				0
#define scrollY				1
#define sectionHeight		2
#define vramBaseAdressLo	3
#define vramBaseAdressHi	4
#define tileTableAdressLo	5
#define tileTableAdressHi	6
#define wrapLine			7
#define flags				8
#define scrollXcoarse		9
#define scrollXfine			10		
#define vramRenderAdressLo	11
#define vramRenderAdressHi	12
#define vramWrapAdressLo	13
#define vramWrapAdressHi	14

;Sprites Struct offsets
#define sprPosX  0
#define sprPosY  1
#define sprTileIndex 2
#define sprFlags 3



#if SCROLLING == 1
	.section .noinit
#else
	.section .bss
#endif 

	.align 5
	;VRAM MUST be aligned to 32 bytes for no scrolling and 256 with scrolling.
	;To align vram to a 32/256 byte boundary without wasting ram, 
	;add the following to your makefile's linker section and adjust 
	;the .data section start to make room for the vram size (including the overlay ram).
	;By default the vram is 32x32 so 1k is required.
	;
	;LDFLAGS += -Wl,--section-start,.noinit=0x800100 -Wl,--section-start,.data=0x800500
	;
	vram: 	  				.space VRAM_SIZE 
	
	overlay_vram:
	#if SCROLLING == 0 && OVERLAY_LINES >0
							.space VRAM_TILES_H*OVERLAY_LINES
	#endif

.section .bss

.align 1

sprites:
	.space SPRITE_STRUCT_SIZE * MAX_SPRITES
ram_tiles:
	.space RAM_TILES_COUNT * TILE_HEIGHT * TILE_WIDTH
ram_tiles_restore:
	.space RAM_TILES_COUNT * 3 ; 2 bytes VRAM addr; 1 byte Tile
free_tile_index:
	.space 1               ; Next free tile index
user_ram_tiles_c:
	.space 1               ; User RAM tiles count
user_ram_tiles_c_tmp:
	.space 1               ; User RAM tiles count, user supplied value

sprites_tile_banks:
	.space 8
tile_table_lo:
	.space 1
tile_table_hi:
	.space 1
font_tile_index:
	.space 1


	;ScreenType struct members
	Screen:
		overlay_height:			.space 1
		overlay_tile_table:		.space 2
	#if SCROLLING == 1
		screen_scrollX:			.space 1
		screen_scrollY:			.space 1
		screen_scrollHeight:	.space 1
	#endif

.section .text

#if SCROLLING == 1

	;***************************************************
	; Mode 3 WITH scrolling
	;***************************************************

	sub_video_mode3:
		;de-activate sync timer interrupts
		;we will need to use the I flag to branch in a critical loop
		ldi ZL,(0<<OCIE1A)
		sts _SFR_MEM_ADDR(TIMSK1),ZL

		;wait cycles to align with next hsync
		WAIT r26,183+241

		;Refresh ramtiles indexes in VRAM
		;This has to be done because the main
		;program may have altered the VRAM
		;after vsync and the rendering interrupt.
		lds r16,user_ram_tiles_c

		ldi ZL,lo8(ram_tiles_restore);
		ldi ZH,hi8(ram_tiles_restore);
		ldi r18,3
		mul r16,r18
		add ZL,r0
		adc ZH,r1

		ldi YL,lo8(vram)
		ldi YH,hi8(vram)

		lds r18,free_tile_index
		ldi r19,MAX_RAMTILES		;maximum possible ramtiles
		sub r19,r18					;sub free tile
		add r19,r16					;add user tiles

		cp r18,r16
		breq no_ramtiles
		nop
		nop
upd_loop:
		ld XL,Z+	;load vram offset of ramtile
		ld XH,Z+

		ld r17,X	;get latest VRAM tile that may have been modified my
		st Z+,r17	;the main program and store it in the restore buffer
		st X,r16	;write the ramtile index back to vram

		inc r16
		cp r16,r18
		brlo upd_loop ;loop is 14 cycles

no_ramtiles:
		;wait for remaining maximum possible ramtiles
1:
		ldi r17,3
		dec r17
		brne .-4
		rjmp .
		dec r19
		brne 1b



		;**********************
		; setup scroll stuff
		;**********************
	
		ldi YL,lo8(vram)
		ldi YH,hi8(vram)

		//add X scroll (coarse)
		lds r18,screen_scrollX ;ScreenScrollX
		mov r25,r18
		andi r18,0xf8	;(x>>3) * 8 interleave
		add YL,r18

		;save Y wrap adress 
		movw r12,YL
	

		//add Y scroll (coarse)
		lds r16,screen_scrollY ;ScreenScrollY
		mov r22,r16
		lsr r16
		lsr r16
		lsr r16 ;/8

        lds r17,screen_scrollHeight
        sub r17,r16
        mov r15,r17 ;Y tiles to draw before wrapping

        mov r17,r16
        lsr r16
        lsr r16
        lsr r16 ;/8
		add YH,r16      ; (bits 6-7)
		andi r17,0x7
        add YL,r17      ;interleave (bits 3-5)
        andi r22,0x7    ;fine Y scrolling (bits 0-2)

		;lds r20,tile_table_lo
		;lds r21,tile_table_hi
		;out _SFR_IO_ADDR(GPIOR1),r20 ;store for later
		;out _SFR_IO_ADDR(GPIOR2),r21

		lds r20,overlay_tile_table
		lds r21,overlay_tile_table+1
		lds r6,tile_table_lo
		lds r7,tile_table_hi
		out _SFR_IO_ADDR(GPIOR1),r6 ;store for later
		out _SFR_IO_ADDR(GPIOR2),r7


		;save main section value	
		movw r10,YL
		mov r23,r22
		mov r24,r15
		mov r9,r25

		;load values for overlay if it's activated (overlay_height>0)
		
		;compute beginning of overlay in vram 
		lds r16,screen_scrollHeight
		mov r18,r16
		lsr r16
		lsr r16
		lsr r16			;hi8
		inc r16			;add 0x100 ram offset
		andi r18,7		;lo8
		
		lds r19,overlay_height	
		cpi r19,0
		in r0, _SFR_IO_ADDR(SREG)

		sbrs r0,SREG_Z
		clr r22
		sbrs r0,SREG_Z
		mov YL,r18		;lo8(overlay_vram)
		sbrs r0,SREG_Z
		mov YH,r16		;hi8(overlay_vram)
		sbrs r0,SREG_Z
		ser r24
		sbrs r0,SREG_Z
		clr r9

		sbrs r0,SREG_Z
		out _SFR_IO_ADDR(GPIOR1),r20
		sbrs r0,SREG_Z
		out _SFR_IO_ADDR(GPIOR2),r21


		//ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw
		//mov r8,r16
		lds r8,render_lines_count ;total scanlines to draw



	;*************************************************************
	; Rendering main loop starts here
	;*************************************************************
	;r6:r7   = main area tileset
	;r8      = Total scanlines to draw
	;r9      = Current section scrollX
	;r10:r11 = Main area begin address
	;r12:r13 = Main area Y wrap adress
	;r15 = Main Y tiles to draw before wrapping
	;r19 = Overlay tiles to draw
	;r22 = Current section tile row
	;r23 = Main section tile row
	;r24 = Current Y tiles to draw before wrapping
	;r25 = Main section scrollX

	next_tile_line:
		rcall hsync_pulse

		WAIT r18,HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES
				
		call render_tile_line

		WAIT r18,58

		inc r22
		dec r8
		breq text_frame_end

		cpi r22,TILE_HEIGHT ;last char line? 1
		breq next_tile_row

		;wait to align with next_tile_row instructions (+1 cycle for the breq)
		WAIT r16,25
		rjmp next_tile_line

	next_tile_row:

		clr r22		;clear current char line

		;increment vram pointer next row
		mov r16,YL
		andi r16,0x7
		cpi r16,7
		breq 1f
		inc YL
		rjmp 2f
	1:
		andi YL,0xf8
		inc YH
	2:

		dec r24		;wrap section?
		brne .+2
		movw YL,r12

		dec r19
		brne .+2
		mov r22,r23	;section tile row
		brne .+2
		movw YL,r10 ;vram adress
		brne .+2
		mov r24,r15 ;Y wrapping
		brne .+2
		mov r9,r25  ;scrollX

		brne .+2
		out _SFR_IO_ADDR(GPIOR1),r6  ;tileset
		brne .+2
		out _SFR_IO_ADDR(GPIOR2),r7  ;tilset

		rjmp next_tile_line

	text_frame_end:

		WAIT r18,28

		rcall hsync_pulse ;145
	
		clr r1
		call RestoreBackground

		;set vsync flag & flip field
		lds ZL,sync_flags
		ldi r20,SYNC_FLAG_FIELD
		ori ZL,SYNC_FLAG_VSYNC
		eor ZL,r20
		sts sync_flags,ZL
	
		cli 

		;re-activate sync timer interrupts
		ldi ZL,(1<<OCIE1A)
		sts _SFR_MEM_ADDR(TIMSK1),ZL
			
		;clear any pending timer int
		ldi ZL,(1<<OCF1A)
		sts _SFR_MEM_ADDR(TIFR1),ZL

		ret


	;*************************************************
	; RENDER TILE LINE
	;
	; r10     = render line counter (decrementing)
	; r22     = Y offset in tiles
	; Y       = VRAM adress to draw from (must not be modified)
	;
	; Can destroy: r0,r1,r2,r3,r4,r5,r6,r7,r13,r16,r17,r18,r19,r20,r21,Z
	; 
	; cycles  = 1495
	;*************************************************
	render_tile_line:
		push YL
		push YH
		push r23
		push r22
		push r19
		push r13
		push r12
		push r9
		push r7
		push r6

		;--------------------------
		; Rendering 
		;---------------------------

		;get tile row offset
		ldi r23,TILE_WIDTH ;tile width in pixels
		mul r22,r23

		;compute base adresses for ROM and RAM tiles
		in r16,_SFR_IO_ADDR(GPIOR1) ;tile_table_lo
		in r17,_SFR_IO_ADDR(GPIOR2) ;tile_table_hi
		subi r16,lo8(RAM_TILES_COUNT*TILE_HEIGHT*TILE_WIDTH)
		sbci r17,hi8(RAM_TILES_COUNT*TILE_HEIGHT*TILE_WIDTH)

		add r16,r0
		adc r17,r1
		movw r2,r16			;rom tiles adress

		ldi r16,lo8(ram_tiles)
		ldi r17,hi8(ram_tiles)
		add r16,r0
		adc r17,r1
		movw r4,r16			;ram tiles adress

		ldi r19,TILE_HEIGHT*TILE_WIDTH
		ldi r17,SCREEN_TILES_H-1	;main loop counter


		;handle fine scroll offset
		;lds r22,screenSections+scrollX
		mov r22,r9
		andi r22,0x7		
		mov r14,r22	;pixels to draw on last tile	
		cli			;no trailing pixel to draw (hack, see end: )
		breq .+2
		sei			;some trailing pixel to draw (hack, see end: )

		;get first pixel of last tile in ROM (for ROM tiles fine scroll)
		;and adress of next pixel
		movw ZL,YL
		subi ZL,-(SCREEN_TILES_H*8)
		ld r18,Z
		mul r18,r19 	;tile*width*height
	    add r0,r2    ;add ROM title table address +row offset
	    adc r1,r3
		movw ZL,r0
		lpm r9,Z+	;hold first pixel until end 
		movw r12,ZL ;hold second pixel adress until end


		;compute first tile adress
	    ld r18,Y     	;load next tile # from VRAM
		subi YL,-8
		cpi r18,RAM_TILES_COUNT
		in r16,_SFR_IO_ADDR(SREG)	;save the carry flag for later	
		mul r18,r19 	;tile*width*height
		movw r20,r2		;rom tiles	
		sbrc r16,SREG_C
		movw r20,r4		;ram tiles
	    add r0,r20    ;add title table address +row offset
	    adc r1,r21
		movw XL,r0


		;compute second tile adress
	    ld r18,Y     	;load next tile # from VRAM
		subi YL,-8
		cpi r18,RAM_TILES_COUNT
		in r7,_SFR_IO_ADDR(SREG)	;save the carry flag for later
		bst r7,SREG_C
		mul r18,r19 	;tile*width*height
		movw r20,r2		;rom tiles
		brtc .+2
		movw r20,r4		;ram tiles
	    add r0,r20      ;add title table address +row offset
	    adc r1,r21
		movw ZL,r0
		movw r6,ZL		;push Z


	do_fine_scroll:
		;output 1st tile with fine scroll offset 
		clr r0
		add XL,r22	;add fine offset
		adc XH,r0

		;compute jump offset
		ldi r23,3
		mul r22,r23 ;3 instructions
	
		sbrs r16,SREG_C
		rjmp rom_fine_scroll

	/***FINE SCROLL RAM LOOP***/
	ram_fine_scroll:
		rjmp .
		ldi r22,lo8(pm(ram_fine_scroll_loop))
		ldi r23,hi8(pm(ram_fine_scroll_loop))
		add r22,r0
		adc r23,r1
		push r22
		push r23	
		ret ;jump into ram_fine_scroll_loop
	ram_fine_scroll_loop:
		.rept 8
			ld r16,X+
			lpm
			out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 
		.endr

		;branch to tile #2
		brtc romloop
		rjmp ramloop

	/***FINE SCROLL ROM LOOP***/
	rom_fine_scroll:
		movw ZL,XL
		ldi r22,lo8(pm(rom_fine_scroll_loop))	
		ldi r23,hi8(pm(rom_fine_scroll_loop))
		add r22,r0
		adc r23,r1
		push r22
		push r23	
		ret
	rom_fine_scroll_loop:
		.rept 8
			lpm r16,Z+
			rjmp .
			out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 
		.endr 
	
		movw ZL,r6		;restore Z for tile #2

		;branch to tile #2
		brts ramloop

	
	romloop:
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 1
	    ld r18,Y     ;load next tile # from VRAM

	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 2
		mul r18,r19 ;tile*width*height

	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 3
		subi YL,-8
		cpi r18,RAM_TILES_COUNT		;is tile in RAM or ROM? (RAM tiles have indexes<RAM_TILES_COUNT)
		
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 4
		brsh .+2		;skip if next tile is in ROM	
		movw r20,r4 	;load RAM title table address +row offset	
   
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 5
		add r0,r20		;add tile table address +row offset lsb
	    adc r1,r21		;add title table address +row offset msb

	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 6
		cpi r18,RAM_TILES_COUNT	
		dec r17			;decrement tiles to draw on line (does not affect carry)
   
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 7   
	    lpm r16,Z+

		breq end	
	    movw ZL,r0   	;copy next tile adress

	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 8   
	    brcc romloop
	
		rjmp .

	ramloop:

	    ld r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 1
	    ld r18,Y     ;load next tile # from VRAM

	    ld r16,Z+ 
		subi YL,-8   		
		out _SFR_IO_ADDR(DATA_PORT),r16 		;pixel 2
		mul r18,r19 ;tile*width*height

	    ld r16,Z+
		nop
		out _SFR_IO_ADDR(DATA_PORT),r16         ;pixel 3
		cpi r18,RAM_TILES_COUNT
		rjmp .
   
	    ld r16,Z+
		out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 4
		brcs .+2 
		movw r20,r2 	;ROM title table address +row offset	
   
   
	    ld r16,Z+
	    add r0,r20    ;add title table address +row offset
		out _SFR_IO_ADDR(DATA_PORT),r16       ;pixel 5
	    adc r1,r21
		rjmp .
    
		ld r16,Z+		
		out _SFR_IO_ADDR(DATA_PORT),r16       ;pixel 6
		ld r7,Z+
	    ld r16,Z+	
	
		movw ZL,r0
		out _SFR_IO_ADDR(DATA_PORT),r7      ;pixel 7   
		nop
		cpi r18,RAM_TILES_COUNT	
	    dec r17
	    breq end
	
		nop
		out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 8   
	
	    brcc romloop
		rjmp ramloop
	
	end:
		out _SFR_IO_ADDR(DATA_PORT),r16  	;pixel 8
		brid end_fine_scroll				;hack: interrupt flag=0 => no fine offset pixel to draw
		brcc end_rom_fine_scroll_loop

	/***END RAM LOOP***/
		movw ZL,r0
	end_ram_fine_scroll_loop:
		ld r16,Z+
		out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 
		dec r14
		brne end_ram_fine_scroll_loop
		rjmp end_fine_scroll_ram

	/***END ROM LOOP***/
	end_rom_fine_scroll_loop:
		movw ZL,r12
		nop
		out _SFR_IO_ADDR(DATA_PORT),r9        ;output saved 1st pixel
		dec r14
		breq end_fine_scroll_rom
	
	.rept 6
		lpm r16,Z+		
		out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 
		dec r14
		breq end_fine_scroll_rom
	.endr

	end_fine_scroll:	
		nop
	end_fine_scroll_rom:
		nop
	end_fine_scroll_ram:
		clr r16	
		out _SFR_IO_ADDR(DATA_PORT),r16   

		pop r6
		pop r7
		pop r9
		pop r12
		pop r13
		pop r19
		pop r22
		pop r23
		pop YH
		pop YL

		ret

#else


	;***************************************************
	; Mode 3 with NO scrolling
	;***************************************************	
	sub_video_mode3:

		;wait cycles to align with next hsync
		WAIT r16,465 //30-3+340+98

		;Refresh ramtiles indexes in VRAM
		;This has to be done because the main
		;program may have altered the VRAM
		;after vsync and the rendering interrupt.
		lds r16,user_ram_tiles_c

		ldi ZL,lo8(ram_tiles_restore);
		ldi ZH,hi8(ram_tiles_restore);
		ldi r18,3
		mul r16,r18
		add ZL,r0
		adc ZH,r1

		ldi YL,lo8(vram)
		ldi YH,hi8(vram)

		lds r18,free_tile_index
		ldi r19,MAX_RAMTILES		;maximum possible ramtiles
		sub r19,r18					;sub free tile
		add r19,r16					;add user tiles

		cp r18,r16
		breq no_ramtiles
		nop
		nop
upd_loop:
		ld XL,Z+	;load vram offset of ramtile
		ld XH,Z+

		ld r17,X	;get latest VRAM tile that may have been modified my
		st Z+,r17	;the main program and store it in the restore buffer
		st X,r16	;write the ramtile index back to vram

		inc r16
		cp r16,r18
		brlo upd_loop ;loop is 14 cycles

no_ramtiles:
		;wait for remaining maximum possible ramtiles
1:
		ldi r17,3
		dec r17
		brne .-4
		rjmp .
		dec r19
		brne 1b


		lds r2,overlay_tile_table
		lds r3,overlay_tile_table+1
		lds r16,tile_table_lo 
		lds r17,tile_table_hi
		movw r12,r16
		movw r6,r16

		ldi r24,SCREEN_TILES_V
		ldi YL,lo8(vram)
		ldi YH,hi8(vram)
		movw r8,YL	
		clr r0

		;load values for overlay if it's activated (overlay_height>0)
		lds r19,overlay_height	
		cpi r19,0
		
		breq .+2
		ldi YL,lo8(overlay_vram)
		
		breq .+2
		ldi YH,hi8(overlay_vram)
		
		breq .+2
		mov r24,r19

		breq .+2
		movw r12,r2


		ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
		mov r10,r16
		clr r22
		ldi r23,TILE_WIDTH ;tile width in pixels




	;****************************************
	; Rendering main loop starts here
	;****************************************
	;r6:r7  = Main area tile table
	;r8:r9  = Main area address
	;r10    = total lines to draw
	;r12:r13= Main tile table or overlay tile table if overlay_height>0
	;r24	= vertical tiles to draw before reloading vram adress (for overlay)
	;Y      = vram or overlay_ram if overlay_height>0
	;
	next_tile_line:	
		rcall hsync_pulse

		WAIT r19,250 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT + FILL_DELAY

		;***draw line***
		call render_tile_line

		WAIT r19,47 + FILL_DELAY - CENTER_ADJUSTMENT	

		dec r10
		breq frame_end
	
		inc r22
		lpm ;3 nop

		cpi r22,TILE_HEIGHT ;last char line? 1
		breq next_tile_row 
	
		;wait to align with next_tile_row instructions (+1 cycle for the breq)
		WAIT r19,11
		
		rjmp next_tile_line	

	next_tile_row:
		clr r22		;current char line			;1	

		clr r0
		ldi r19,VRAM_TILES_H
		add YL,r19
		adc YH,r0

		dec r24		;overlay done?
		brne .+2
		movw YL,r8	;main vram
		brne .+2
		movw r12,r6	;main tile table

	
		rjmp next_tile_line

	frame_end:

		WAIT r19,18

		rcall hsync_pulse ;145
	
		clr r1
		call RestoreBackground

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
	; r22     = Y offset in tiles
	; r23 	  = tile width in bytes
	; Y       = VRAM adress to draw from (must not be modified)
	;*************************************************
	render_tile_line:

		;load first tile and determine if its a ROM or RAM tile

		movw XL,YL

		mul r22,r23

		movw r16,r12 ;current tile table (main or overlay)
		subi r16,lo8(RAM_TILES_COUNT*TILE_HEIGHT*TILE_WIDTH)
		sbci r17,hi8(RAM_TILES_COUNT*TILE_HEIGHT*TILE_WIDTH)

		add r16,r0
		adc r17,r1
		movw r2,r16			;rom tiles

		ldi r16,lo8(ram_tiles)
		ldi r17,hi8(ram_tiles)
		add r16,r0
		adc r17,r1
		movw r4,r16			;ram tiles

		ldi r19,TILE_HEIGHT*TILE_WIDTH
		ldi r17,SCREEN_TILES_H

	    ld r18,X+     	;load next tile # from VRAM

		mul r18,r19 	;tile*width*height
		movw r20,r2		;rom tiles
		
		cpi r18,RAM_TILES_COUNT
		brcc .+2
		movw r20,r4		;ram tiles

	    add r0,r20    ;add title table address +row offset
	    adc r1,r21

		movw ZL,r0
		
		cpi r18,RAM_TILES_COUNT
		brcs ramloop
	

	romloop:
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 1
	    ld r18,X+     ;load next tile # from VRAM


	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 2
		mul r18,r19 ;tile*width*height


	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 3
		cpi r18,RAM_TILES_COUNT		;is tile in RAM or ROM? (RAM tiles have indexes<RAM_TILES_COUNT)
		nop

	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 4
		brsh .+2		;skip in next tile is in ROM	
		movw r20,r4 	;load RAM title table address +row offset	
   
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 5
		add r0,r20		;add title table address +row offset lsb
	    adc r1,r21		;add title table address +row offset msb

	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 6
		
		cpi r18,RAM_TILES_COUNT	
		dec r17			;decrement tiles to draw on line
   
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 7   
	    lpm r16,Z+

		breq end	
	    movw ZL,r0   	;copy next tile adress

	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 8   
	    brcc romloop
	
		rjmp .

	ramloop:

	    ld r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 1
	    ld r18,X+     ;load next tile # from VRAM

	    ld r16,Z+ 
		nop   
		out _SFR_IO_ADDR(DATA_PORT),r16 		;pixel 2
		mul r18,r19 ;tile*width*height


	    ld r16,Z+
		nop
		out _SFR_IO_ADDR(DATA_PORT),r16         ;pixel 3
		cpi r18,RAM_TILES_COUNT
   		rjmp .

	    ld r16,Z+
		out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 4
		brcs .+2 
		movw r20,r2 	;ROM title table address +row offset	
   
   
	    ld r16,Z+
	    add r0,r20    ;add title table address +row offset
		out _SFR_IO_ADDR(DATA_PORT),r16       ;pixel 5
	    adc r1,r21
		rjmp .
    
		ld r16,Z+		
		out _SFR_IO_ADDR(DATA_PORT),r16       ;pixel 6
		cpi r18,RAM_TILES_COUNT
		rjmp .  

	    ld r16,Z+	
		out _SFR_IO_ADDR(DATA_PORT),r16      ;pixel 7   
	    ld r16,Z+

	    dec r17
	    breq end
	
		movw ZL,r0
		out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 8   
	
	    brcc romloop
		rjmp ramloop
	
	end:
		out _SFR_IO_ADDR(DATA_PORT),r16  	;pixel 8
		clr r16	
		lpm	
		nop
		out _SFR_IO_ADDR(DATA_PORT),r16        

		ret

#endif

;***********************************
; Copy a flash tile to a ram tile
; C-callable
; r24=Source ROM tile index
; r22=Dest RAM tile index
;************************************
CopyFlashTile:
	ldi r18,TILE_HEIGHT*TILE_WIDTH

	;compute source adress
	lds ZL,tile_table_lo
	lds ZH,tile_table_hi
	mul r24,r18
	add ZL,r0
	adc ZH,r1

	;compute destination adress
	ldi XL,lo8(ram_tiles)
	ldi XH,hi8(ram_tiles)
	mul r22,r18
	add XL,r0
	adc XH,r1

	;copy data (fastest possible)
.rept TILE_HEIGHT*TILE_WIDTH
	lpm r1,Z+
	st X+,r1
.endr
	clr r1
	ret

;***********************************
; Copy a flash tile to a ram tile
; C-callable
; r24=Source RAM tile index
; r22=Dest RAM tile index
;************************************
CopyRamTile:

	ldi r18,TILE_HEIGHT*TILE_WIDTH

	;compute source adress
	ldi ZL,lo8(ram_tiles)
	ldi ZH,hi8(ram_tiles)
	mul r24,r18
	add ZL,r0
	adc ZH,r1

	;compute destination adress
	ldi XL,lo8(ram_tiles)
	ldi XH,hi8(ram_tiles)
	mul r22,r18
	add XL,r0
	adc XH,r1

	;copy data (fastest possible)
.rept TILE_HEIGHT*TILE_WIDTH
	ld r1,Z+
	st X+,r1
.endr
	clr r1
	ret



;***********************************
; Restore background (VRAM)
; C-callable
;************************************
RestoreBackground:

	; Restore list: Begin at user_ram_tiles_c (above the user RAM tiles),
	; end before free_tile_index (the first unused RAM tile).

	lds   ZL,      user_ram_tiles_c
	mov   r24,     ZL
	add   ZL,      ZL
	add   ZL,      r24     ; Multiply by 3
	clr   ZH
	subi  ZL,      lo8(-(ram_tiles_restore))
	sbci  ZH,      hi8(-(ram_tiles_restore))

	lds   r0,      free_tile_index
	sub   r24,     r0
	brcc  rbg_exit

	; Restore loop

rbg_loop:
	ld    XL,      Z+      ; VRAM address low
	ld    XH,      Z+      ; VRAM address high
	ld    r0,      Z+      ; Tile index to restore
	st    X,       r0
	inc   r24
	brne  rbg_loop

rbg_exit:

	ret



;***********************************
; SET TILE 8bit mode
; C-callable
;     r24: SpriteNo
;     r22: RAM tile index (bt)
; r21:r20: Y:X (0 or 1, location of 8x8 sprite fragment on 2x2 tile container)
; r19:r18: DY:DX (0 to 7, offset of sprite relative to 0:0 of container)
;************************************
BlitSprite:

	; src = sprites_tiletable_lo + (sprites[i].tileIndex * TILE_HEIGHT * TILE_WIDTH)

	ldi   r25,     SPRITE_STRUCT_SIZE
	mul   r24,     r25

	movw  ZL,      r0
	subi  ZL,      lo8(-(sprites))
	sbci  ZH,      hi8(-(sprites))

	ldd   r23,     Z + sprFlags
	ldd   r24,     Z + sprTileIndex

	; Get tile bank addr

	ldi   r25,     4 * 2
	mul   r23,     r25
	mov   XL,      r1
	clr   XH
	subi  XL,      lo8(-(sprites_tile_banks))
	sbci  XH,      hi8(-(sprites_tile_banks))
	ld    ZL,      X+
	ld    ZH,      X+

	ldi   r25,     TILE_WIDTH * TILE_HEIGHT
	mul   r24,     r25
	add   ZL,      r0      ; Tile data src
	adc   ZH,      r1

	; dest = ram_tiles + (bt * TILE_HEIGHT * TILE_WIDTH)

	mul   r22,     r25
	movw  XL,      r0
	subi  XL,      lo8(-(ram_tiles))
	sbci  XH,      hi8(-(ram_tiles))

	/*
	if ((yx & 1U) == 0U){
		srcXdiff = dx;
		xspan    = TILE_WIDTH - dx;
		if ((flags & SPRITE_FLIP_X) == 0U){
			dest += dx;
		}else{
			dest += (TILE_WIDTH - 1U);
			src  += srcXdiff;
		}
	}else{
		srcXdiff = TILE_WIDTH - dx;
		xspan    = dx;
		if ((flags & SPRITE_FLIP_X) == 0U){
			src  += srcXdiff;
		}else{
			dest += (dx - 1U);
		}
	}
	*/

	ldi   r25,     0       ; srcXdiff high byte & used for zero
	sbrc  r20,     0
	rjmp  x_2nd_tile

	mov   r24,     r18     ; srcXdiff = dx
	ldi   r20,     TILE_WIDTH
	sub   r20,     r18     ; xspan = TILE_WIDTH - dx
	sbrs  r23,     SPRITE_FLIP_X_BIT
	rjmp  x_1st_tile_nxf

	adiw  XL,      TILE_WIDTH - 1 ; dest += (TILE_WIDTH - 1)

x_2nd_tile_nxf:

	add   ZL,      r24
	adc   ZH,      r25     ; src += srcXdiff
	rjmp  x_check_end

x_2nd_tile:

	ldi   r24,     TILE_WIDTH
	sub   r24,     r18     ; srcXdiff = TILE_WIDTH - dx
	mov   r20,     r18     ; xspan = dx;
	sbrs  r23,     SPRITE_FLIP_X_BIT
	rjmp  x_2nd_tile_nxf

	sbiw  XL,      1       ; dest -= 1

x_1st_tile_nxf:

	add   XL,      r18
	adc   XH,      r25     ; dest += dx

x_check_end:

	/*
	if ((yx & 0x0100U) == 0U){
		yspan = (TILE_HEIGHT - dy);
		dest += (dy * TILE_WIDTH);
		if ((flags & SPRITE_FLIP_Y) != 0U){
			src += (TILE_WIDTH * (TILE_HEIGHT - 1U));
		}
	}else{
		yspan = dy;
		if ((flags & SPRITE_FLIP_Y) != 0U){
			src += (TILE_WIDTH * (dy - 1));
		}else{
			src += (TILE_WIDTH * (TILE_HEIGHT - dy));
		}
	}
	*/

	ldi   r22,     TILE_WIDTH
	ldi   r18,     TILE_HEIGHT
	sub   r18,     r19     ; temp = (TILE_HEIGHT - dy)

	sbrc  r21,     0
	rjmp  y_2nd_tile

	mul   r22,     r19
	add   XL,      r0
	adc   XH,      r1      ; dest += (dy * TILE_WIDTH)

	sbrc  r23,     SPRITE_FLIP_Y_BIT
	subi  ZL,      lo8(-(TILE_WIDTH * (TILE_HEIGHT - 1)))
	sbrc  r23,     SPRITE_FLIP_Y_BIT
	sbci  ZH,      hi8(-(TILE_WIDTH * (TILE_HEIGHT - 1)))

	mov   r1,      r18     ; yspan = temp

	rjmp  y_check_end

y_2nd_tile:

	mov   r1,      r19     ; temp = dy - 1
	dec   r1
	sbrs  r23,     SPRITE_FLIP_Y_BIT
	mov   r1,      r18     ; temp = TILE_HEIGHT - dy
	mul   r22,     r1      ; src += (temp * TILE_WIDTH)
	add   ZL,      r0
	adc   ZH,      r1

	mov   r1,      r19     ; yspan = dy

y_check_end:

	/*
	if ((flags & SPRITE_FLIP_Y) != 0U){
		srcXdiff -= (TILE_WIDTH * 2);
	}
	*/

	sbrc  r23,     SPRITE_FLIP_Y_BIT
	sbiw  r24,     (TILE_WIDTH * 2)

	/*
	if ((flags & SPRITE_FLIP_X) == 0U){
		destXdiff = TILE_WIDTH - (xspan - 1);
		step = 1;
	}else{
		destXdiff = TILE_WIDTH + (xspan - 1);
		step = -1;
	}
	; destXdiff is calculated negated for an optimization in the loop
	*/

	sbrc  r23,     SPRITE_FLIP_X_BIT
	rjmp  x_diff_xf

	ldi   r21,     -(TILE_WIDTH + 1) ; destXdiff = -(TILE_WIDTH + 1)
	add   r21,     r20     ; destXdiff += xspan
	ldi   r23,     0x00
	ldi   r22,     0x01    ; step = 1
	rjmp  x_diff_end

x_diff_xf:

	ldi   r21,     -(TILE_WIDTH - 1) ; destXdiff = -(TILE_WIDTH - 1)
	sub   r21,     r20     ; destXdiff -= xspan
	ldi   r23,     0xFF
	ldi   r22,     0xFF    ; step = -1

x_diff_end:

	/*
	for (y2 = 0U; y2 < yspan; y2++){
		for (x2 = 0U; x2 < xspan; x2++){
			px = pgm_read_byte(src);
			src ++;
			if(px != TRANSLUCENT_COLOR){
				*dest = px;
			}
			dest += step;
		}
		src += srcXdiff;
		dest += destXdiff;
	}
	*/
	;     r19 = translucent color
	;  r1:r20 = yspan:xspan
	; r23:r22 = step
	;     r21 = destXdiff (negated)
	;       X = dest
	; r25:r24 = srcXdiff
	;       Z = src
	;
	; dest += step; is omitted for the last X iteration, compensated with
	; destXdiff.
	; destXdiff is negated to allow for using sub instead of add, so subi
	; can be used to subtract high byte.

	ldi   r19,     TRANSLUCENT_COLOR

	mov   r0,      r20     ; xspan
	lsr   r20
	brcc  x_loop1
	breq  x_loopx

x_loop0:
	lpm   r18,     Z+      ; px = pgm_read_byte(src); src ++;
	cpse  r18,     r19     ; if (px != TRANSLUCENT_COLOR)
	st    X,       r18     ; *dest = px
	add   XL,      r22     ; dest += step;
	adc   XH,      r23
x_loop1:
	lpm   r18,     Z+      ; px = pgm_read_byte(src); src ++;
	cpse  r18,     r19     ; if (px != TRANSLUCENT_COLOR)
	st    X,       r18     ; *dest = px
	add   XL,      r22     ; dest += step;
	adc   XH,      r23
	subi  r20,     1
	brne  x_loop0
x_loopx:
	lpm   r18,     Z+      ; px = pgm_read_byte(src); src ++;
	cpse  r18,     r19     ; if (px != TRANSLUCENT_COLOR)
	st    X,       r18     ; *dest = px

	dec   r1
	breq  loop_e

	add   ZL,      r24     ; src += srcXdiff
	adc   ZH,      r25
	sub   XL,      r21     ; dest += destXdiff (negated)
	sbci  XH,      0xFF

	mov   r20,     r0      ; xspan
	lsr   r20
	brcc  x_loop1
	brne  x_loop0
	rjmp  x_loopx

loop_e:

	ret                    ; r1 is zero at this point





;*****************************
; Defines where the sprites tile are defined. (obsolete, use SetSpritesTileTableBank)
; C-callable
; r25:r24=pointer to sprites pixel data.
;*****************************
.section .text.SetSpritesTileTable
SetSpritesTileTable:
	sts sprites_tile_banks,r24
	sts sprites_tile_banks+1,r25
	ret



;*****************************
; Defines where the sprites tile are defined.
; Sprites can use one of four tile banks.
; C-callable
;     r24=bank No (0-3)
; r23:r22=pointer to sprites pixel data.
;*****************************
.section .text.SetSpritesTileBank
SetSpritesTileBank:
	andi r24,3
	lsl r24	
	ldi ZL,lo8(sprites_tile_banks)
	ldi ZH,hi8(sprites_tile_banks)
	add ZL,r24
	adc ZH,r1
	st Z,r22
	std Z+1,r23
	ret

;***********************************
; CLEAR VRAM 8bit
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearVram
ClearVram:
	//init vram		
	ldi r30,lo8(VRAM_SIZE+(VRAM_TILES_H*OVERLAY_LINES))
	ldi r31,hi8(VRAM_SIZE+(VRAM_TILES_H*OVERLAY_LINES))

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)

	ldi r22,RAM_TILES_COUNT

fill_vram_loop:
	st X+,r22
	sbiw r30,1
	brne fill_vram_loop

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
	lds r21,font_tile_index
	add r20,21
	rjmp SetTile	

;***********************************
; SET TILE 8bit mode
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; r20=Tile No (8 bit)
;************************************
.section .text.SetTile
SetTile:
#if SCROLLING == 1
	;index formula is vram[((y>>3)*256)+8x+(y&7)]
	
	andi r24,0x1f
	mov r23,r22
	lsr r22
	lsr r22
	lsr r22			;y>>3
	ldi r18,8		
	mul r24,r18		;x*8
	movw XL,r0
	subi XL,lo8(-(vram))
	sbci XH,hi8(-(vram))
	add XH,r22		;vram+((y>>3)*256)
	andi r23,7		;y&7	
	add XL,r23
						
	subi r20,~(RAM_TILES_COUNT-1)	
	st X,r20

	clr r1

	ret

#else

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
	
	subi r20,~(RAM_TILES_COUNT-1)	
	st X,r20

	clr r1

	ret

#endif


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
; Get the tile index at the specified position 
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; Returns: Tile No (8 bit)
;************************************
.section .text.GetTile
GetTile:
#if SCROLLING == 1
	;index formula is vram[((y>>3)*256)+8x+(y&7)]
	
	andi r24,0x1f
	mov r23,r22
	lsr r22
	lsr r22
	lsr r22			;y>>3
	ldi r18,8		
	mul r24,r18		;x*8
	movw XL,r0
	subi XL,lo8(-(vram))
	sbci XH,hi8(-(vram))
	add XH,r22		;vram+((y>>3)*256)
	andi r23,7		;y&7	
	add XL,r23

	ld r24,X						
	subi r24,RAM_TILES_COUNT
	
	clr r25
	clr r1

	ret

#else

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
	subi r24,RAM_TILES_COUNT

	clr r25
	clr r1

	ret

#endif

