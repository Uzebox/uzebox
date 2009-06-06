 ;***************************************************
; TEXT MODE VIDEO PROCESSING
; Process video frame in tile mode (30*28)
;***************************************************	

.global ScreenScrollX
.global ScreenScrollY
.global ram_tiles
.global ram_tiles_restore
.global screenSections
.global sprites

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

.section .bss
	.align 1

	sprites:				.space SPRITE_STRUCT_SIZE*MAX_SPRITES
	ram_tiles:				.space RAM_TILES_COUNT*TILE_HEIGHT*TILE_WIDTH
	ram_tiles_restore:  	.space RAM_TILES_COUNT*3 ;vram addr|Tile
	sprites_tiletable_lo: 	.byte 1
	sprites_tiletable_hi: 	.byte 1	
	vram_linear_buf:		.space 30
	screenSections:			.space SCREEN_SECTIONS_COUNT*SCREEN_SECTION_STRUCT_SIZE


.section .text

sub_video_mode3:
	;de-activate sync timer interrupts
	;we will need to use the I flag to branch in a critical loop
	ldi ZL,(0<<OCIE1A)
	sts _SFR_MEM_ADDR(TIMSK1),ZL


	
	;**********************
	; This block updates the ram_tiles_restore buffer
	; with the actual VRAM. This is required because since the time
	; the process_sprite is executed at VSYNC, the main program may 
	; have altered the vram (TODO: better explain)
	;***********************

	;Set ramtiles indexes in VRAM 
	ldi ZL,lo8(ram_tiles_restore);
	ldi ZH,hi8(ram_tiles_restore);

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	lds r18,free_tile_index


	clr r16
upd_loop:	
	ldd XL,Z+0
	ldd XH,Z+1
	
	add XL,YL
	adc XH,YH

	ld r17,X	;currbgtile
	std Z+2,r17

	cp r16,r18
	brsh noov
	mov r17,r16
noov:
	st X,r17
	
	adiw ZL,3 ;sizeof(ram_tiles_restore)

	inc r16
	cpi r16,RAM_TILES_COUNT
	brlo upd_loop ;23


	;align in time with maximum numbers of ramtile possible
	ldi r16,32
	subi r16,RAM_TILES_COUNT
wait_loop:
	
	ldi r17,6		;wait same as previous upd_loop
	dec r17
	brne .-4

	dec r16
	brne wait_loop



	;**********************
	; setup scroll stuff
	;**********************
	
	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	//add X scroll (coarse)
	lds r18,screenSections+scrollX
	mov r25,r18
	lsr r18
	lsr r18
	lsr r18 ;/8
	clr r17
	add YL,r18
	adc YH,r17


	//add Y scroll (corse)
	lds r22,screenSections+scrollY
	

	mov r16,r22	
	lsr r16
	lsr r16
	lsr r16 ;/8
	ldi r17,VRAM_TILES_H
	mul r16,r17
	add YL,r0
	adc YH,r1	
	andi r22,0x7	;fine Y scrolling



	;wait 873 cycles
	ldi r26,lo8(204)
	ldi r27,hi8(204)
	sbiw r26,1
	brne .-4	
	nop

	
	lds r20,screenSections+tileTableAdressLo
	lds r21,screenSections+tileTableAdressHi
	out _SFR_IO_ADDR(GPIOR1),r20 ;store for later
	out _SFR_IO_ADDR(GPIOR2),r21

	lds r19,screenSections+wrapLine
	lds r15,screenSections+sectionHeight
	lds r11,screenSections+scrollY

	clr r8 ;current section no
	
	lds r12,screenSections+vramBaseAdressLo
	lds r13,screenSections+vramBaseAdressHi
	add r12,r18	;add X scroll coarse
	adc r13,r8
	
	;compute vram render start address
	mov r16,r11
	lsr r16
	lsr r16
	lsr r16
	mul r16,r17
	movw YL,r12
	add YL,r0
	adc YH,r1


	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16


;*************************************************************
; Rendering main loop starts here
;*************************************************************
;r8  = Current section No
;r10 = total scanlines to draw
;r11 = Section current scroll line
;r12:r13 = section Y wrap adress
;r15 = Section height
;r19 = Section wrap line 
;r22 = section current tile row
;r25 = scrollX

next_text_line:	
	;***draw scanline***
	call render_tile_line

	nop

	inc r11
	inc r22

	;Process split screen sections (33 cycles)
	;----------------------------------
	dec r15	;end of screen section?
	brne no_split
	
	inc r8	;increment section No

	ldi r16,SCREEN_SECTION_STRUCT_SIZE
	mul r8,r16
	movw ZL,r0
	subi ZL,lo8(-(screenSections))	
	sbci ZH,hi8(-(screenSections))

	ldd r0,Z+tileTableAdressLo
	ldd r1,Z+tileTableAdressHi
	out _SFR_IO_ADDR(GPIOR1),r0 ;store for later use
	out _SFR_IO_ADDR(GPIOR2),r1


	ldd r19,Z+wrapLine
	ldd r15,Z+sectionHeight
	ldd r11,Z+scrollY
	ldd r25,Z+scrollX

	clr r0
	ldi r17,VRAM_TILES_H

	;vramWrapAdress=vramBaseAdress+(scrollX/8)
	ldd r12,Z+vramBaseAdressLo
	ldd r13,Z+vramBaseAdressHi
	mov r16,r25
	lsr r16
	lsr r16
	lsr r16
	add r12,r16	;add X scroll coarse
	adc r13,r0

	;vramRenderAdress=vramWrapAdress+((scrollY/8)*VRAM_TILES_H);	
	movw YL,r12
	mov r16,r11
	lsr r16
	lsr r16
	lsr r16
	mul r16,r17
	add YL,r0
	adc YH,r1


	mov r22,r11
	andi r22,0x7	;tile offset (fine Y scrolling)
	rjmp end_split
no_split:

	ldi r16,15
	dec r16
	brne .-4 ;27
	
end_split:	

	cp r11,r19 	;wrap Y?
	brne .+2 	
	movw YL,r12	;load wrap adress
	brne .+2
	clr r11		;reset Y scroll line
	brne .+2	
	clr r22		;reset tile row



	dec r10
	breq text_frame_end

	cpi r22,8 ;last char line? 1
	breq next_text_row 
	
	;wait to align with next_tile_row instructions (+1 cycle for the breq)
	rjmp .
	rjmp .
	rjmp next_text_line	

next_text_row:

	clr r22		;current char line			;1	
	adiw YL,VRAM_TILES_H 	;process next line in VRAM ;2
	rjmp next_text_line





text_frame_end:
	;13
	lpm
	lpm
	lpm ;3 nop
	lpm ;3 nop
	lpm ;3 nop
	lpm ;3 nop
	lpm ;3 nop
	nop
	nop

	rcall hsync_pulse ;145
	
	call RestoreBackground

	;set vsync flag if beginning of next frame (each two fields)
	ldi r17,1
	lds r16,curr_field
	eor r16,r17
	sts curr_field,r16

	lds r18,curr_frame
	tst r16
	breq .+2
	eor r18,r17
	sts curr_frame,r18

	;set vsync flag if beginning of next frame
	ldi ZL,1
	sts vsync_flag,ZL
	
	cli 
	
	;clear any pending timer int
	ldi ZL,(1<<OCF1A)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	;re-activate sync timer interrupts
	ldi ZL,(1<<OCIE1A)
	sts _SFR_MEM_ADDR(TIMSK1),ZL


	clr r1


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
	push r22
	push r19

	;fill the linear line buffer with 30 tiles
	mov r19,YL
	andi r19,0xe0 ;restore bits mask
	ldi XL,lo8(vram_linear_buf)
	ldi XH,hi8(vram_linear_buf)

	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2	
	rcall update_sound_buffer_fast ;27 (destroys Z, r16,r17)
	
.rept 15
	ld r18,Y
	st X+,r18
	inc YL
	andi YL,31
	or YL,r19 ;restore hi bits
.endr

	;rjmp .
	ld r18,Y

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2

.rept 14
	;ld r18,Y
	st X+,r18
	inc YL
	andi YL,31
	or YL,r19 ;restore hi bits
	ld r18,Y
.endr

	;ld r18,Y
	st X+,r18




	;--------------------------
	; Rendering 
	;---------------------------
	ldi YL,lo8(vram_linear_buf)
	ldi YH,hi8(vram_linear_buf)

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
	mov r22,r25
	andi r22,0x7		
	mov r14,r22	;pixels to draw on last tile	
	cli			;no trailing pixel to draw (hack, see end: )
	breq .+2
	sei			;some trailing pixel to draw (hack, see end: )

	;get first pixel of last tile in ROM (for ROM tiles fine scroll)
	ldd r18,Y+SCREEN_TILES_H
	mul r18,r19 	;tile*width*height
    add r0,r2    ;add ROM title table address +row offset
    adc r1,r3
	movw ZL,r0
	lpm r9,Z	;hold until end 


	;compute first tile adress
    ld r18,Y+     	;load next tile # from VRAM
	cpi r18,RAM_TILES_COUNT
	in r16,_SFR_IO_ADDR(SREG)	;save the carry flag	
	mul r18,r19 	;tile*width*height
	movw r20,r2		;rom tiles	
	sbrc r16,SREG_C
	movw r20,r4		;ram tiles
    add r0,r20    ;add title table address +row offset
    adc r1,r21
	movw XL,r0


	;compute second tile adress
    ld r18,Y+     	;load next tile # from VRAM
	cpi r18,RAM_TILES_COUNT
	in r7,_SFR_IO_ADDR(SREG)	;save the carry flag
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
	ret 
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
    ld r18,Y+     ;load next tile # from VRAM


    lpm r16,Z+
    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 2
	mul r18,r19 ;tile*width*height


    lpm r16,Z+
    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 3
	cpi r18,RAM_TILES_COUNT		;is tile in RAM or ROM? (RAM tiles have indexes<RAM_TILES_COUNT)
	in r6,_SFR_IO_ADDR(SREG)	;save the carry flag


    lpm r16,Z+
    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 4
	brsh .+2		;skip in next tile is in ROM	
	movw r20,r4 	;load RAM title table address +row offset	
   
    lpm r16,Z+
    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 5
    bst r6,SREG_C	;store carry state in T flag for later branch
	add r0,r20		;add title table address +row offset lsb
    
    lpm r16,Z+
    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 6
	adc r1,r21		;add title table address +row offset msb
	dec r17			;decrement tiles to draw on line

   
    lpm r16,Z+
    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 7   
    lpm r16,Z+

	breq end	
    movw ZL,r0   	;copy next tile adress

    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 8   
    brtc romloop
	
	rjmp .

ramloop:

    ld r16,Z+
    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 1
    ld r18,Y+     ;load next tile # from VRAM

    ld r16,Z+ 
	nop   
	out _SFR_IO_ADDR(DATA_PORT),r16 		;pixel 2
	mul r18,r19 ;tile*width*height


    ld r16,Z+
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16         ;pixel 3
	cpi r18,RAM_TILES_COUNT
	in r6,_SFR_IO_ADDR(SREG)	;save the carry flag
	bst r6,SREG_C
   

    ld r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 4
	brts .+2 
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
    rjmp .
	
    dec r17
    breq end
	
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 8   
	
    brtc romloop
	rjmp ramloop
	
end:
	out _SFR_IO_ADDR(DATA_PORT),r16  	;pixel 8
	brid end_fine_scroll				;hack: interrupt flag=0 => no fine offset pixel to draw
	brts end_ram_fine_scroll_loop

/***END ROM LOOP***/
end_rom_fine_scroll_loop:
	movw ZL,r0
	adiw ZL,1
	out _SFR_IO_ADDR(DATA_PORT),r9        ;output saved 1st pixel
	dec r14
	breq end_fine_scroll_rom
	
.rept 6
	lpm r16,Z+		
	out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 
	dec r14
	breq end_fine_scroll_rom
.endr
	
/***END RAM LOOP***/
end_ram_fine_scroll_loop:
	ld r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 
	dec r14
	brne end_ram_fine_scroll_loop

end_fine_scroll:	
	nop
end_fine_scroll_rom:
	clr r16	
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16   


	lds ZL,sync_pulse
	dec ZL
	sts sync_pulse,ZL

	pop r19
	pop r22
	pop YH
	pop YL

	ret




#if VIDEO_MODE == 3
	;***********************************
	; SET TILE 8bit mode
	; C-callable
	; r24=ROM tile index
	; r22=RAM tile index
	;************************************
	CopyTileToRam:
	/*
		src=tile_table_lo+((bt&0x7f)*64);
		dest=ram_tiles+(free_tile_index*TILE_HEIGHT*TILE_WIDTH);

		ram_tiles_restore[free_tile_index].addr=ramPtr;//(by*VRAM_TILES_H)+bx+x;
		ram_tiles_restore[free_tile_index].tileIndex=bt;

		for(j=0;j<64;j++){
			px=pgm_read_byte(src++);
			*dest++=px;
		}
	*/

		ldi r18,TILE_HEIGHT*TILE_WIDTH

		;compute source adress
		lds ZL,tile_table_lo
		lds ZH,tile_table_hi
		;andi r24,0x7f
		subi r24,RAM_TILES_COUNT
		mul r24,r18
		add ZL,r0
		adc ZH,r1

		;compute destination adress
		ldi XL,lo8(ram_tiles)
		ldi XH,hi8(ram_tiles)
		mul r22,r18
		add XL,r0
		adc XH,r1

		clr r0
		;copy data (fastest possible)
	.rept TILE_HEIGHT*TILE_WIDTH
		lpm r0,Z+	
		st X+,r0
	.endr


		clr r1
		ret




	;***********************************
	; SET TILE 8bit mode
	; C-callable
	; r24=SpriteNo
	; r22=RAM tile index (bt)
	; r21:r20=Y:X
	; r19:r18=DY:DX
	;************************************
	BlitSprite:
	
		;src=sprites_tiletable_lo+(sprites[i].tileIndex*TILE_HEIGHT*TILE_WIDTH)
		ldi r25,SPRITE_STRUCT_SIZE
		mul r24,r25
	
		ldi ZL,lo8(sprites)	
		ldi ZH,hi8(sprites)	
		add ZL,r0
		adc ZH,r1
		ldd r24,Z+sprTileIndex

		lds ZL,sprites_tiletable_lo
		lds ZH,sprites_tiletable_hi
		ldi r25,TILE_WIDTH*TILE_HEIGHT
		mul r24,r25
		add ZL,r0	;src
		adc ZH,r1

		;dest=ram_tiles+(bt*TILE_HEIGHT*TILE_WIDTH)
		ldi XL,lo8(ram_tiles)	
		ldi XH,hi8(ram_tiles)
		mul r22,r25
		add XL,r0
		adc XH,r1

		;if(x==0){
		;	dest+=dx;
		;	xdiff=dx;
		;}else{
		;	src+=(8-dx);
		;	xdiff=(8-dx);
		;}	
		clr r1
		cpi r20,0
		brne x_2nd_tile
		add XL,r18
		adc XH,r1
		mov r24,r18	;xdiff
		rjmp x_check_end
	x_2nd_tile:
		ldi r24,8
		sub r24,r18	;xdiff
		add ZL,r24
		adc ZH,r1	
	x_check_end:

		;if(y==0){
		;	dest+=(dy*TILE_WIDTH);
		;	ydiff=dy;
		;}else{
		;	src+=((8-dy)*TILE_WIDTH);
		;	ydiff=(8-dy);
		;}
		cpi r21,0
		brne y_2nd_tile
		ldi r25,TILE_WIDTH
		mul r25,r19
		add XL,r0
		adc XH,r1
		mov r25,r19	;ydiff
		rjmp y_check_end
	y_2nd_tile:
		ldi r25,TILE_HEIGHT
		sub r25,r19	;ydiff
		ldi r21,TILE_WIDTH
		mul r21,r25
		add ZL,r0
		adc ZH,r1	
	y_check_end:	
	
	/*
		for(y2=ydiff;y2<TILE_HEIGHT;y2++){
			for(x2=xdiff;x2<TILE_WIDTH;x2++){
								
				px=pgm_read_byte(src++);
				if(px!=TRANSLUCENT_COLOR){
					*dest=px;
				}
				dest++;

			}		
			src+=xdiff;
			dest+=xdiff;

		}
	*/

		clr r1
		ldi r19,TRANSLUCENT_COLOR

		ldi r21,8
		sub r21,r25 ;y2

	y2_loop:
		ldi r20,8
		sub r20,r24 ;x2
	x2_loop:
		lpm r18,Z+
		cpse r18,r19
		st X,r18
		adiw XL,1
		dec r20
		brne x2_loop

		add ZL,r24
		adc ZH,r1
		add XL,r24
		adc XH,r1

		dec r21
		brne y2_loop

		clr r1

		ret
#endif
