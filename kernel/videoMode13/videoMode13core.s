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

;***************************************************
; Video Mode 13: 28x28 (224x224 pixels) using 8x8 tiles
; with overlay & sprites X flipping.
;
; If the SCROLLING build parameter=0, the scrolling 
; code is removed and the screen resolution 
; increases up to 30*28 tiles.
; 
; For compile time switch and information relating
; to this video mode see:
; http://uzebox.org/wiki/index.php?title=Video_Mode_13
;
;***************************************************	

.global TIMER1_OVF_vect
.global vram
.global ram_tiles
.global ram_tiles_restore
.global sprites
.global overlay_vram
.global sprites_tile_banks
.global Screen
.global SetSpritesTileTable
.global CopyTileToRam
.global SetSpritesTileBank
.global SetTile
.global ClearVram
.global SetFontTilesIndex
.global SetTileTable
.global SetTile
.global BlitSprite
.global SetFont
.global GetTile
.global palette
.global SetPaletteColorAsm

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

#define VIDEO _SFR_IO_ADDR(DATA_PORT)

.section .noinit
	.align 8
	;Ramtiles must be located at 0x100 and RAM_TILES_COUNT must be a multiple of 8
	;vram must be aligne to 256 bytes
	;palette must be aligned to 256 bytes
	;
	;LDFLAGS += -Wl,--section-start,.noinit=0x800100 -Wl,--section-start,.data=0x80xxxx
	;
	#if RAM_TILES_COUNT & 0x07
		#error RAM_TILES_COUNT must be a multiple of 8
	#endif

	ram_tiles:				.space RAM_TILES_COUNT*(TILE_HEIGHT*TILE_WIDTH/2)	;1024+256+840=2120=0x848+0x100=0x948
	palette:				.space 256											
	vram: 	  				.space VRAM_SIZE 									


.section .bss
	.align 1
	sprites:				.space SPRITE_STRUCT_SIZE*MAX_SPRITES
	ram_tiles_restore:  	.space RAM_TILES_COUNT*3 ;vram addr|Tile

	sprites_tile_banks: 	.space 8
	tile_table_lo:			.byte 1
	tile_table_hi:			.byte 1
	font_tile_index:		.byte 1 

	overlay_vram:
	#if SCROLLING == 0 && OVERLAY_LINES >0
							.space 0 ;VRAM_TILES_H*OVERLAY_LINES
	#endif	

	;ScreenType struct members
	Screen:
		overlay_height:			.byte 1
		overlay_tile_table:		.word 1
	#if SCROLLING == 1
		screen_scrollX:			.byte 1
		screen_scrollY:			.byte 1
		screen_scrollHeight:	.byte 1
	#endif

.section .text


;***************************************************
; Mode 13 with NO scrolling
;***************************************************	
sub_video_mode13:

	;wait cycles to align with next hsync
	WAIT r16,36

	;Set ramtiles indexes in VRAM 
	ldi ZL,lo8(ram_tiles_restore);
	ldi ZH,hi8(ram_tiles_restore);

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	lds r18,free_tile_index


	ldi 16,REG_IO_OFFSET
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
	cpi r16,RAM_TILES_COUNT+REG_IO_OFFSET
	brlo upd_loop ;23


#if RAM_TILES_COUNT == 0 
	ldi r16,60-RAM_TILES_COUNT 
#else
	ldi r16,61-RAM_TILES_COUNT 
#endif

wait_loop:
	ldi r17,6
	dec r17
	brne .-4
	dec r16
	brne wait_loop


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


	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16
	clr r22

	;clear any pending timer int (to avoid a crashing bug in uzem 1.20 and previous)
	ldi ZL,(1<<OCF1B)+(1<<OCF1A)+(1<<TOV1)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	;set timer1 OVF interrupt
	;this trick allows to exist the main loops 
	;when the 30 tiles are rendered 
	ldi r16,(1<<TOIE1)
	sts _SFR_MEM_ADDR(TIMSK1),r16
	
	ldi r16,(0<<WGM12)+(1<<CS10)	;switch to timer1 normal mode (mode 0)
	sts _SFR_MEM_ADDR(TCCR1B),r16

	WAIT r19,3

;****************************************
; Rendering main loop starts here
;****************************************
;r10    = total lines to draw
;r22	=Y tile row
;Y      = vram or overlay_ram if overlay_height>0
;
next_tile_line:	
	rcall hsync_pulse ;64171,65991
	WAIT r19,242 + FILL_DELAY - AUDIO_OUT_HSYNC_CYCLES

	;***draw line***
	call render_tile_line

	WAIT r19,35 + FILL_DELAY

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

	adiw YL,VRAM_TILES_H
	rjmp .

	;clr r0
	;ldi r19,VRAM_TILES_H
	;add YL,r19
	;adc YH,r0

	;dec r24		;overlay done?
	;brne .+2
	;movw YL,r8	;main vram
	;brne .+2
	;movw r12,r6	;main tile table
	rjmp .
	rjmp .
	nop

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
	ldi ZL,(1<<OCF1B)+(1<<OCF1A)+(1<<TOV1)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	ldi r16,(1<<WGM12)+(1<<CS10)	;switch back to timer1 CTC mode (mode 4)
	sts _SFR_MEM_ADDR(TCCR1B),r16

	ldi r16,(1<<OCIE1A)				;restore ints on compare match
	sts _SFR_MEM_ADDR(TIMSK1),r16

	ret



;*************************************************
; RENDER TILE LINE
;
; r22     = Y offset in tiles
; r23 	  = tile width in bytes
; Y       = VRAM adress to draw from (must not be modified)
;*************************************************
render_tile_line:

	push YL
	push YH
	
	;Set timer so that it generates an overflow interrupt when
	;all tiles are rendered
	ldi r16,lo8(0xffff-(6*8*VRAM_TILES_H)+9-30)
	ldi r17,hi8(0xffff-(6*8*VRAM_TILES_H)+9-30)
	sts _SFR_MEM_ADDR(TCNT1H),r17
	sts _SFR_MEM_ADDR(TCNT1L),r16
	sei

	mov r24,r22	;Y offset in tiles*tile width in bytes (4)
	lsl r24
	lsl r24
	
	ldi XH,hi8(palette)
	nop
	ldi r16,(TILE_HEIGHT*TILE_WIDTH)/2  ;(bytes per tile)
	mov r15,r16
	clr r2

    ld r17,Y+     	;load next tile # from VRAM
	bst r17,7		;set T flag with msbit of tile index. 1=rom, 0=ram tile   
	andi r17,0x7f   ;clear tile index msbit to have both ram/rom tile bases adress at zero	
	mul r17,r15 	;tile*32	
    add r0,r24    	;add row offset to tile table addr
	movw ZL,r0
	
	lpm XL,Z+       ;load rom pixels 0,1
	nop	
	nop
	brtc ramloop
	rjmp .

romloop:
	ld   r16,X+		;LUT pixel 0
	nop

	out VIDEO,r16	;output pixel 0
	ld 	r17,Y+		;load next tile index 
	bst r17,7		;set T flag with msbit of tile index. 1=rom, 0=ram tile   
	ld 	r16,X		;LUT pixel 1

	out VIDEO,r16   ;output pixel 1
	lpm XL,Z+		;load rom pixels 2,3
	ld  r16,X+      ;LUT pixel 2

	out VIDEO,r16   ;output pixel 2
	andi r17,0x7f   ;clear tile index msbit to have both ram/rom tile bases adress at zero
	mul r17,r15     ;tile index * 32
	ld   r16,X      ;LUT pixel 3

	out VIDEO,r16   ;output pixel 3
	lpm XL,Z+		;load rom pixels 4,5
	ld   r16,X+     ;LUT pixel 4

	out VIDEO,r16   ;output pixel 4
	ld   r16,X      ;LUT pixel 5
	lpm XL,Z		;load rom pixels 6,7   

	out VIDEO,r16   ;output pixel 5
	ld   r16,X+     ;LUT pixel 6
	ld   r17,X      ;LUT pixel 7
	movw ZL,r0      ;copy next tile address

	out VIDEO,r16   ;output pixel 6
	add ZL,r24		;add Y tile offset. Cannot overflow in ZH because tile table is aligned to zero.
	lpm XL,Z+       ;load rom pixels 0,1
  	nop
	 
mainloop:
   out VIDEO,r17	;output pixel 7 (ram & rom)
   brts romloop

ramloop: 
   ld XL,-Z			;load ram pixels 0,1
   ld r16,X+		;LUT pixel 0
   
   out VIDEO,r16   	;output pixel 0
   ld r16,X      	;LUT pixel 1
   ld r17,Y+      	;next tile
   bst r17,7      	;set T flag with msbit of tile index. 1=rom, 0=ram tile     
   
   out VIDEO,r16   	;output pixel 1
   ldd XL,Z+1      	;load ram pixels 2,3
   ld r16,X+      	;LUT pixel 2
   andi r17,0x7f   	;clear tile index msbit to have both ram/rom tile bases adress at zero
      
   out VIDEO,r16   	;output pixel 2
   ld r16,X      	;LUT pixel 3
   mul r17,r15      ;tile index * 32
      
   out VIDEO,r16   	;output pixel 3
   ldd XL,Z+2      	;load ram pixels 4,5
   ld r16,X+      	;LUT pixel 4
   nop
   
   out VIDEO,r16   	;output pixel 4
   ld r16,X      	;LUT pixel 5
   ldd XL,Z+3      	;load ram pixels 6,7
   movw ZL,r0      	;copy tile pointer
   
   out VIDEO,r16   	;output pixel 5
   ld r16,X+      	;LUT pixel 6
   ld r17,X      	;LUT pixel 7
   add ZL,r24		;add Y tile offset. Cannot overflow in ZH because tile table is aligned to zero.
   
   out VIDEO,r16   	;output pixel 6
   lpm XL,Z+      	;load rom pixels 0,1
   rjmp mainloop	


;end of render line   
TIMER1_OVF_vect:
   out VIDEO,r2


	pop r0	;pop OVF interrupt return address
	pop r0	;pop OVF interrupt return address
	
	pop YH
	pop YL

	;restore timer1 to the value it should normally have at this point
	ldi r16,lo8(0x0027)
	sts _SFR_MEM_ADDR(TCNT1H),r2
	sts _SFR_MEM_ADDR(TCNT1L),r16

	ret	;TCNT1 must be equal to 0x0027



;***********************************
; Copy a flash tile to a ram tile
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
	
	ldi r18,TILE_HEIGHT*TILE_WIDTH/2	;tile size in bytes

	;compute source adress
	ldi ZL,0;tile_table_lo
	ldi ZH,0;tile_table_hi
	
	mul r24,r18
	add ZL,r0
	adc ZH,r1

	;compute destination adress
	ldi XL,0;lo8(ram_tiles)
	ldi XH,0;hi8(ram_tiles)
	mul r22,r18
	add XL,r0
	adc XH,r1

	clr r0
	;copy data (fastest possible)
.rept TILE_HEIGHT*TILE_WIDTH/2
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
	push r16
	push r17
	push YL
	push YH

	;src=sprites_tiletable_lo+(sprites[i].tileIndex*TILE_HEIGHT*TILE_WIDTH)
	ldi r25,SPRITE_STRUCT_SIZE
	mul r24,r25

	ldi ZL,lo8(sprites)	
	ldi ZH,hi8(sprites)	
	add ZL,r0
	adc ZH,r1

	ldd r16,Z+sprFlags

	;8x16 multiply
	ldd r24,Z+sprTileIndex
	ldi r30,TILE_WIDTH*TILE_HEIGHT
	mul r24,r30
	movw r26,r0
	
	;get tile bank addr
	ldi r25,4*2
	mul r16,r25
	ldi YL,lo8(sprites_tile_banks)	
	ldi YH,hi8(sprites_tile_banks)	
	clr r0
	add YL,r1
	adc YH,r0		
	ldd ZL,Y+0
	ldd ZH,Y+1
	add ZL,r26	;tile data src
	adc ZH,r27
	
	;dest=ram_tiles+(bt*TILE_HEIGHT*TILE_WIDTH)
	ldi XL,lo8(ram_tiles)	
	ldi XH,hi8(ram_tiles)
	ldi r25,TILE_WIDTH*TILE_HEIGHT
	mul r22,r25
	add XL,r0
	adc XH,r1

	/*
	if((yx&1)==0){
		dest+=dx;	
		destXdiff=dx;
		srcXdiff=dx;
				
		if(flags&SPRITE_FLIP_X){
			src+=(TILE_WIDTH-1);
			srcXdiff=((TILE_WIDTH*2)-dx);
		}
	}else{
		destXdiff=(TILE_WIDTH-dx);

		if(flags&SPRITE_FLIP_X){
			srcXdiff=TILE_WIDTH+dx;
			src+=dx;
			src--;
		}else{
			srcXdiff=destXdiff;
			src+=destXdiff;
		}
	}
	*/
	clr r1
	clr YH		;hi8(srcXdiff)

	cpi r20,0	
	brne x_2nd_tile
	
	add XL,r18	;dest+=dx
	adc XH,r1
	mov r24,r18	;destXdiff=dx
	mov YL,r18	;srcXdiff=dx

	sbrs r16,SPRITE_FLIP_X_BIT
	rjmp x_check_end

	adiw ZL,(TILE_WIDTH-1)	;src+=7
	ldi YL,TILE_WIDTH*2		;srcXdiff=((TILE_WIDTH*2)-dx);
	sub YL,r18	
	rjmp x_check_end

x_2nd_tile:
	ldi r24,TILE_WIDTH
	sub r24,r18		;8-DX = xdiff for dest

	sbrc r16,SPRITE_FLIP_X_BIT
	rjmp x2_flip_x

	mov YL,r24		;srcXdiff=destXdiff;
	add ZL,r24		;src+=destXdiff;
	adc ZH,r1	
	rjmp x_check_end

x2_flip_x:
	ldi YL,TILE_WIDTH
	add YL,r18		;srcXdiff=TILE_WIDTH+dx;	
	add ZL,r18		;src+=dx;
	adc ZH,r1
	sbiw ZL,1		;src--;

x_check_end:



	/*
	if((yx&0x0100)==0){
		dest+=(dy*TILE_WIDTH);
		ydiff=dy;
		if(flags&SPRITE_FLIP_Y){
			src+=(TILE_WIDTH*(TILE_HEIGHT-1));
		}
	}else{			
		ydiff=(TILE_HEIGHT-dy);
		if(flags&SPRITE_FLIP_Y){
			src+=((dy-1)*TILE_WIDTH); 
		}else{
			src+=(ydiff*TILE_WIDTH);
		}
	}
	*/
	cpi r21,0
	brne y_2nd_tile

	ldi r25,TILE_WIDTH	;dest+=(dy*TILE_WIDTH)
	mul r25,r19			
	add XL,r0
	adc XH,r1

	mov r25,r19			;ydiff=dy

	sbrc r16,SPRITE_FLIP_Y_BIT
	adiw ZL,(TILE_WIDTH*(TILE_HEIGHT-1))	;src+=(TILE_WIDTH*(TILE_HEIGHT-1));		

	rjmp y_check_end

y_2nd_tile:
	ldi r25,TILE_HEIGHT	;ydiff=(TILE_HEIGHT-dy)
	sub r25,r19	
	
	mov r22,r19			;temp=dy-1
	dec r22
	sbrs r16,SPRITE_FLIP_Y_BIT
	mov r22,r25			;temp=ydiff

	ldi r21,TILE_WIDTH	;src+=(temp*TILE_WIDTH);
	mul r21,r22
	add ZL,r0
	adc ZH,r1	
y_check_end:	
	
	//if(flags&SPRITE_FLIP_X){
	//	step=-1;
	//}
	ser r22		;step=-1
	ser r23
	sbrs r16,SPRITE_FLIP_X_BIT
	ldi r22,1	;step=1
	sbrs r16,SPRITE_FLIP_X_BIT
	clr r23

	//if(flags&SPRITE_FLIP_Y){
	//	srcXdiff-=(TILE_WIDTH*2);
	//}
	sbrc r16,SPRITE_FLIP_Y_BIT
	sbiw YL,(TILE_WIDTH*2)

	/*
	for(y2=0;y2<(TILE_HEIGHT-ydiff);y2++){
		for(x2=0;x2<(TILE_WIDTH-destXdiff);x2++){
						
			px=pgm_read_byte(src);
			if(px!=TRANSLUCENT_COLOR){
				*dest=px;
			}
			dest++;
			src+=step;
		}		
		src+=srcXdiff;
		dest+=destXdiff;
	}
	*/
	;r19 	= translucent color
	;r20:r21= xspan:yspan
	;r22:r23= step
	;r24	= destXdiff
	;r25	= ydiff
	;X		= dest
	;Y		= srcXdiff
	;Z		= src
	clr r1
	ldi r19,TRANSLUCENT_COLOR

	ldi r21,TILE_HEIGHT
	sub r21,r25 	;yspan=(TILE_HEIGHT-ydiff)

y_loop:
	ldi r20,TILE_WIDTH
	sub r20,r24 	;xspan=(TILE_WIDTH-destXdiff)

x_loop:
	lpm r18,Z		;px=pgm_read_byte(src);
	cpse r18,r19	;if(px!=TRANSLUCENT_COLOR)
	st X,r18		;*dest=px;
	adiw XL,1
	add ZL,r22		;src+=step;
	adc ZH,r23
	dec r20
	brne x_loop

	add ZL,YL		;src+=srcXdiff
	adc ZH,YH
	add XL,r24		;dest+=destXdiff
	adc XH,r1
	dec r21
	brne y_loop


	pop YH
	pop YL
	pop r17
	pop r16
	ret





/*
;***********************************
; SET TILE 8bit mode
; C-callable
; r24=SpriteNo
; r22=RAM tile index (bt)
; r21:r20=Y:X
; r19:r18=DY:DX
;************************************
BlitSprite:
	push r16
	push r17

	;src=sprites_tiletable_lo+(sprites[i].tileIndex*TILE_HEIGHT*TILE_WIDTH)
	ldi r25,SPRITE_STRUCT_SIZE
	mul r24,r25

	ldi ZL,lo8(sprites)	
	ldi ZH,hi8(sprites)	
	add ZL,r0
	adc ZH,r1

	ldd r16,Z+sprFlags

	;8x16 multiply
	ldd r24,Z+sprTileIndex
	;clr r25
	ldi r30,TILE_WIDTH*TILE_HEIGHT
	mul r24,r30
	movw r26,r0
	;mul r25,r30
	;add r27,r0
	
	;get tile bank addr
	ldi r25,4*2
	mul r16,r25
	ldi ZL,lo8(sprites_tile_banks)	
	ldi ZH,hi8(sprites_tile_banks)	
	clr r0
	add ZL,r1
	adc ZH,r0		
	ldd r0,Z+0
	ldd r1,Z+1
	movw ZL,r0

	//lds ZL,sprites_tile_banks
	//lds ZH,sprites_tile_banks+1
	add ZL,r26	;tile data src
	adc ZH,r27
	
	;dest=ram_tiles+(bt*TILE_HEIGHT*TILE_WIDTH)
	ldi XL,lo8(ram_tiles)	
	ldi XH,hi8(ram_tiles)
	ldi r25,TILE_WIDTH*TILE_HEIGHT
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
	mov r24,r18	;xdiff for dest
	mov r17,r18	;xdiff for src

	sbrs r16,SPRITE_FLIP_X_BIT
	rjmp x_check_end

	adiw ZL,(TILE_WIDTH-1);7
	ldi r17,16
	sub r17,r18	;xdiff for src
	rjmp x_check_end


x_2nd_tile:
	ldi r24,TILE_WIDTH
	sub r24,r18	;8-DX = xdiff for dest

	sbrc r16,SPRITE_FLIP_X_BIT
	rjmp x2_flip_x

	mov r17,r24	;xdiff for src
	add ZL,r24
	adc ZH,r1	
	rjmp x_check_end

x2_flip_x:
	ldi r17,TILE_WIDTH
	add r17,r18	;xdiff for src
	
	add ZL,r18
	adc ZH,r1
	sbiw ZL,1

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


//	for(y2=ydiff;y2<TILE_HEIGHT;y2++){
//		for(x2=xdiff;x2<TILE_WIDTH;x2++){
//							
//			px=pgm_read_byte(src++);
//			if(px!=TRANSLUCENT_COLOR){
//				*dest=px;
//			}
//			dest++;
//
//		}		
//		src+=xdiff;
//		dest+=xdiff;
//
//	}

	clr r1
	ldi r19,TRANSLUCENT_COLOR

	clt
	sbrc r16,SPRITE_FLIP_X_BIT
	set

	ldi r21,TILE_HEIGHT ;8
	sub r21,r25 ;y2

y2_loop:
	ldi r20,TILE_WIDTH ;8
	sub r20,r24 ;x2

	brts x2_loop_flip

	;normal X loop (11 cycles)
x2_loop:
	lpm r18,Z+
	cpse r18,r19
	st X,r18
	adiw XL,1
	dec r20
	brne x2_loop
	rjmp x2_loop_end

	;flipped X loop (13 cycles)
x2_loop_flip:
	lpm r18,Z
	sbiw ZL,1
	cpse r18,r19
	st X,r18
	adiw XL,1
	dec r20
	brne x2_loop_flip

x2_loop_end:
	add ZL,r17
	adc ZH,r1
	add XL,r24
	adc XH,r1
	dec r21
	brne y2_loop




	clr r1

	pop r17
	pop r16
	ret
*/




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

;***********************************
; Set the color for a specified palette index
; C-callable
; r24=index
; r22=color
; Returns: void
;************************************
.section .text.SetPaletteColorAsm
SetPaletteColorAsm:

//lsb pixel
//for(i = 0; i < 256; i+=16)
//{
//	palette[i+(index<<1)] = color;
//}

//msb pixel
//for(i = 1; i < 32; i+=2)
//{
//	palette[(index*32)+i] = color;
//}

	;set low pixel
	ldi ZL,lo8(palette)
	ldi ZH,hi8(palette)
	mov r25,r24
	lsl r25
	add ZL,r25
	adc ZH,r1

	st Z,r22
	std Z+16,r22
	std Z+32,r22
	std Z+48,r22
	subi ZL,lo8(-(64))
	sbci ZH,hi8(-(64))
	st Z,r22
	std Z+16,r22
	std Z+32,r22
	std Z+48,r22
	subi ZL,lo8(-(64))
	sbci ZH,hi8(-(64))
	st Z,r22
	std Z+16,r22
	std Z+32,r22
	std Z+48,r22
	subi ZL,lo8(-(64))
	sbci ZH,hi8(-(64))
	st Z,r22
	std Z+16,r22
	std Z+32,r22
	std Z+48,r22

	;set hi pizel	
	ldi ZL,lo8(palette)
	ldi ZH,hi8(palette)
	ldi r25,32
	mul r24,r25
	add ZL,r0
	adc ZH,r1
	
	std Z+1,r22
	std Z+3,r22
	std Z+5,r22
	std Z+7,r22
	std Z+9,r22
	std Z+11,r22
	std Z+13,r22
	std Z+15,r22
	std Z+17,r22
	std Z+19,r22
	std Z+21,r22
	std Z+23,r22
	std Z+25,r22
	std Z+27,r22
	std Z+29,r22
	std Z+31,r22


	ret


