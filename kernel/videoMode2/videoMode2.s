.global vram
.global SetSpritesTileTable
.global vram
.global rotate_spr_no
.global scanline_sprite_buf
.global	sprites_per_lines
.global	sprites
.global SetSpritesTileTable
.global screenSections
.global SetSpritesOptions
.global ProcessSprites
.global SetTile
.global ClearVram
.global SetFontTilesIndex
.global SetTileTable
.global SetTile
.global SetFont


;Sprites Struct offsets
#define sprPosX  0
#define sprPosY  1
#define sprTileIndex 2
#define sprFlags 3

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
		
.section .data
	;*** IMPORTANT ***
	;scanline_sprite_buf array MUST be aligned on a 8-bit boudary 
	;Currently this is done by putting this object as first in the linking phase and results in location 0x100
	;For some reasons, adding a .align 8 before actually cause following variables in .data to
	;be aligned on 0x200 hence wasting precious bytes.
	;Do align with using a custom makefile, remove all source file from project then add *first* uzeboxVideoEngineCore.s
	;then the other files.
	scanline_sprite_buf:.space (SCREEN_TILES_H+2)*TILE_WIDTH ;+2 to account for left+right clipping

	mode2_render_offsets:
		.word pm(mode2_render_line_offset_0)
		#if MODE2_HORIZONTAL_SCROLLING == 1
		.word pm(mode2_render_line_offset_1)
		.word pm(mode2_render_line_offset_2)
		.word pm(mode2_render_line_offset_3)
		.word pm(mode2_render_line_offset_4)
		.word pm(mode2_render_line_offset_5)
		#endif

.section .bss
	.align 5
	vram: 	  				.space VRAM_SIZE ;MUST be aligned to 32 bytes
	
.section .bss
	.align 1


	sprites:			.space 32*SPRITE_STRUCT_SIZE ;|X|Y|TILE INDEX|	
	screenSections:		.space SCREEN_SECTION_STRUCT_SIZE*SCREEN_SECTIONS_COUNT
	spritesOptions:		.space 1		;b0 overflow: 0=clip, 1=flick
	
	sprites_tiletable_lo: .space 1
	sprites_tiletable_hi: .space 1

	sprites_per_lines:	.space (SCREEN_TILES_V)*TILE_HEIGHT*MAX_SPRITES_PER_LINE ;|Y-offset(3bits)|Sprite No(5bits)|
	sprite_buf_erase:	.space MAX_SPRITES_PER_LINE; ;4x8 bit pointers
	rotate_spr_no:		.space 1
	//tile_table_lo:	.space 1
	//tile_table_hi:	.space 1
	font_tile_index:.space 1
.section .text

;***************************************************
; Mode 2: Tile & Sprites Video Mode
; Process video frame in tile mode (24*28) 
; -with sprites
;***************************************************	

sub_video_mode2:

	;waste line to align with next hsync in render function
	;ldi ZL,202-13
;m2_render_delay:
	;rjmp .
	;rjmp .
	;dec ZL
	;brne m2_render_delay 	
	;nop
	WAIT ZL,1324


	lds r20,screenSections+tileTableAdressLo
	lds r21,screenSections+tileTableAdressHi
	out _SFR_IO_ADDR(GPIOR1),r20 ;store for later
	out _SFR_IO_ADDR(GPIOR2),r21

	lds YL,screenSections+vramRenderAdressLo
	lds YH,screenSections+vramRenderAdressHi

	;get Y scroll wrap adress
	lds r4,screenSections+vramWrapAdressLo	
	lds r5,screenSections+vramWrapAdressHi

	lds r9,screenSections+wrapLine
	lds r15,screenSections+sectionHeight

	lds r14,screenSections+scrollXfine
	lds r23,screenSections+scrollY
	mov r22,r23
	andi r22,0x7	;tile offset

	clr r8 ;current section no

	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16

	ldi XL,lo8(sprites_per_lines)
	ldi XH,hi8(sprites_per_lines)

	;Outer loop used registers
	;-----------------------------
	;r4:r5 = section Y wrap adress
	;r8 = Current section No
	;r9 = section wrap line
	;r10 = total scanlines to draw
	;r14 = section X scroll fine offset
	;r15 = section height
	;r22 = section current tile row
	;r23 = section current scroll line
	;X = Sprites per line buffer current line
	;Y = section VRAM rendering position

m2_next_text_line:	
	;***draw line***
	rcall mode2_do_hsync_and_sprites 
	clr r16
	movw YL,r2 ;restore
	movw XL,r6 ;restore 
	out _SFR_IO_ADDR(DATA_PORT),r16 ;turn off last pixel
	;r17=>TRANSLUCENT_COLOR


	//Clear sprite buffer for next render line (58)
	ldi ZH,hi8(scanline_sprite_buf)

	lds ZL,sprite_buf_erase
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17

	lds ZL,sprite_buf_erase+1
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17

	lds ZL,sprite_buf_erase+2
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17

	lds ZL,sprite_buf_erase+3
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17

	lds ZL,sprite_buf_erase+4
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17
	st Z+,r17


	inc r22
	inc r23


	;Process split screen sections (22 cycles)
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
		
	;nop
	;nop
	;nop
	;nop
	;nop
	;nop

	ldd YL,Z+vramRenderAdressLo
	ldd YH,Z+vramRenderAdressHi

	ldd r4,Z+vramWrapAdressLo	
	ldd r5,Z+vramWrapAdressHi

	ldd r9,Z+wrapLine
	ldd r15,Z+sectionHeight
	ldd r14,Z+scrollXfine
	ldd r23,Z+scrollY

	mov r22,r23
	andi r22,0x7	;tile offset
	rjmp end_split
no_split:

	nop
	nop
	nop
	nop
	nop
		
	ldi r16,9
	dec r16
	brne .-4 ;27
	
end_split:	


	cp r23,r9 	;wrap Y?
	brne .+2 	
	movw YL,r4	;load wrap adress
	brne .+2
	clr r23		;reset Y scroll line
	brne .+2	
	clr r22		;reset tile row


	dec r10
	breq m2_text_frame_end

	cpi r22,8 ;last tile row? 1
	breq m2_next_text_row 
	
	;wait to align with next_tile_row instructions (+1 cycle for the breq)
	rjmp .
	rjmp .

	rjmp m2_next_text_line	

m2_next_text_row:
	clr r22		;current tile row			;1	
	adiw YL,VRAM_TILES_H ;32	;process next line in VRAM ;2
	rjmp m2_next_text_line

m2_text_frame_end:

	lpm
	lpm

	call hsync_pulse ;145


	lds r20,sync_pulse
	subi r20,SCREEN_TILES_V*TILE_HEIGHT
	sts sync_pulse,r20

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


;**************************
; Render sprite+H-Sync pulse+sound mix
; Cycles: 144
; Can destroy: r0,r1, r2,r3, r4,r5, r6,r7, r8, r11,r12 ,r13, r16,r17, r19,  r24,r25,Z
;**************************
mode2_do_hsync_and_sprites:
	;Important: TCNT1 should be equal to:
	;0x68 on the cbi 
	;0xf0 on the sbi 
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2


	//Update_sound_buffer_fast (20 cycles)
	;--*** Start sound update ***--	
	lds ZL,mix_pos
	lds ZH,mix_pos+1
				
	ld r16,Z+		;load next sample

	;compare+wrap=8 cycles fixed
	ldi r18,hi8(MIX_BUF_SIZE+mix_buf)
	cpi ZL,lo8(MIX_BUF_SIZE+mix_buf)
	cpc ZH,r18
	ldi r18,lo8(mix_buf)
	ldi r19,hi8(mix_buf)

	sts _SFR_MEM_ADDR(OCR2A),r16 ;output sound byte

	brlo .+2
	movw ZL,r18

	sts mix_pos,ZL
	sts mix_pos+1,ZH	
	;--*** End sound update ***-- ;20


	movw r2,YL //save Y register
	
	;prepare some constants usedfor all aprites
	lds r12,sprites_tiletable_lo
	lds r13,sprites_tiletable_hi
	ldi r24,TILE_WIDTH*TILE_HEIGHT
	ldi r18,SPRITE_STRUCT_SIZE 	
	ldi r19,TILE_WIDTH
	ldi YH,hi8(scanline_sprite_buf)
	ldi r20,lo8(sprites) 
	ldi r21,hi8(sprites)	
	ldi r25,8 //for left shift by 3

	;*** Process sprite #1 ***
	;address good sprite info line
	ld r16,X+		;get sprite # + y offset in sprite
	mul r16,r25		;shift 3 msbits in r1
	mov r17,r1		;sprite y offset
	andi r16,0x1f	;keep sprite#

	movw ZL,r20		;get sprite table base
	mul r16,r18		;sprite no*sprite struct size
	add ZL,r0		;index sprite in struct
	adc ZH,r1

	ldd r16,Z+2 ;SpriteTileNo
	ld   YL,Z 	;get x pos

	movw ZL,r12 ;get sprites tile table base
	mul r16,r24 ;tileheight*tilewidth
	add ZL,r0	;point to first sprite tile pixel in flash
	adc ZH,r1
	mul r17,r19	;get sprite tile line
	add ZL,r0
	adc ZH,r1

	sts sprite_buf_erase,YL		;save addr for buffer clear	
	;26

	lpm r0,Z+	;move sprite pixels from ROM to buffer
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0 
	;30



	;*** Process sprite #2 ***
	;address good sprite info line
	ld r16,X+	;get sprite # + y offset in sprite
	mul r16,r25		;shift 3 msbits in r1
	mov r17,r1		;sprite y offset
	andi r16,0x1f	;keep sprite#

	movw ZL,r20
	mul r16,r18 ;sprite no*sprite struct size
	add ZL,r0
	adc ZH,r1

	ldd r16,Z+2 ;SpriteTileNo
	ld   YL,Z 	;get x pos

	movw ZL,r12 ;get sprites tile table base
	mul r16,r24 ;tileheight*tilewidth
	add ZL,r0	;point to first sprite tile pixel in flash
	adc ZH,r1
	mul r17,r19	;get sprite tile line
	add ZL,r0
	adc ZH,r1

	sts sprite_buf_erase+1,YL		;save addr for buffer clear

	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0

	ld r16,X+	;get sprite #3 + y offset in sprite (to offset SBI)
	
	;*** toggle sync pin  - EXACT TIMING REQD***
	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2

	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0 
	;30




	;*** Process sprite #3 ***
	;address good sprite info line
	
	mul r16,r25		;shift 3 msbits in r1
	mov r17,r1		;sprite y offset
	andi r16,0x1f	;keep sprite#

	movw ZL,r20
	mul r16,r18 ;sprite no*sprite struct size
	add ZL,r0
	adc ZH,r1

	ldd r16,Z+2 ;SpriteTileNo
	ld   YL,Z 	;get x pos

	movw ZL,r12 ;get sprites tile table base	
	mul r16,r24 ;tileheight*tilewidth
	add ZL,r0	;point to first sprite tile pixel in flash
	adc ZH,r1
	mul r17,r19	;get sprite tile line
	add ZL,r0
	adc ZH,r1
		
	sts sprite_buf_erase+2,YL		;save addr for buffer clear

	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0 
	;30




	;*** Process sprite #4 ***
	;address good sprite info line
	ld r16,X+	;get sprite # + y offset in sprite
	mul r16,r25		;shift 3 msbits in r1
	mov r17,r1		;sprite y offset
	andi r16,0x1f	;keep sprite#

	movw ZL,r20
	mul r16,r18 ;sprite no*sprite struct size
	add ZL,r0
	adc ZH,r1

	ldd r16,Z+2 ;SpriteTileNo
	ld   YL,Z 	;get x pos	

	movw ZL,r12 ;get sprites tile table base	
	mul r16,r24 ;tileheight*tilewidth
	add ZL,r0	;point to first sprite tile pixel in flash
	adc ZH,r1
	mul r17,r19	;get sprite tile line
	add ZL,r0
	adc ZH,r1

	sts sprite_buf_erase+3,YL		;save addr for buffer clear
	;30

	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0 
	;30

	;*** Process sprite #5 ***
	;address good sprite info line
	ld r16,X+		;get sprite # + y offset in sprite
	mul r16,r25		;shift 3 msbits in r1
	mov r17,r1		;sprite y offset
	andi r16,0x1f	;keep sprite#

	movw ZL,r20		;get sprite table base
	mul r16,r18		;sprite no*sprite struct size
	add ZL,r0		;index sprite in struct
	adc ZH,r1

	ldd r16,Z+2 ;SpriteTileNo
	ld   YL,Z 	;get x pos

	movw ZL,r12 ;get sprites tile table base
	mul r16,r24 ;tileheight*tilewidth
	add ZL,r0	;point to first sprite tile pixel in flash
	adc ZH,r1
	mul r17,r19	;get sprite tile line
	add ZL,r0
	adc ZH,r1

	sts sprite_buf_erase+4,YL		;save addr for buffer clear	
	;26

	lpm r0,Z+	;move sprite pixels from ROM to buffer
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0
	lpm r0,Z+
	st Y+,r0 
	;30


	;nop
	;nop
	;nop
	;nop
	;nop

	;*************************************************
	; RENDER TILE LINE MODE 2
	; r22 = Current tile row (0-7)
	;*************************************************

	movw YL,r2	;restore Y
	movw r6,XL	;save X
	movw XL,YL

	ldi r17,TRANSLUCENT_COLOR ;transparent color

	in r20,_SFR_IO_ADDR(GPIOR1) ;tile_table_lo
	in r21,_SFR_IO_ADDR(GPIOR2) ;tile_table_hi

	;add tile Y offset	
	mul r22,r19 ;TILE_WIDTH
	movw r24,r0

	mov r19,XL
	andi r19,0xe0 ;mask for 32-bytes line wrap

	;Get fine scolling branch adress
	mov YL,r14
	lsl YL
	clr YH
	subi YL,lo8(-(mode2_render_offsets))
	sbci YH,hi8(-(mode2_render_offsets))
	ld ZL,Y+	
	ld ZH,Y

	
	ldi YL,lo8(scanline_sprite_buf+6)
	ldi YH,hi8(scanline_sprite_buf+6)

	;load the first tile from vram
	ld	r12,X	;load tile no from VRAM
	ldi r18,TILE_HEIGHT*TILE_WIDTH
	mul r12,r18 ;tile*width*height

	inc XL
	andi XL,0x1f	;wrap at 32th tiles
	or XL,r19		;restore higher bits


	add r0,r20	;add title table address
	adc r1,r21

	add r0,r24	;add tile Y offset
	adc r1,r25  ;add tile Y offset	

	ijmp	;call render line functions


	

;******* OFFSET 0 ********************
mode2_render_line_offset_0:
	;r1:r0 = Tile row index
	movw ZL,r0

	lpm
	lpm
	lpm
	lpm

	;first out must be at cycle 14
.rept 22
	
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16
	
	ld	r12,X //+	;load tile # from VRAM

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16

	mul r12,r18 ;tile*width*height
	
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16

	add r0,r20	;add title table address
	adc r1,r21
	
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16

	add r0,24	;add tile table row offset 
	adc r1,25 ;add tile table row offset 

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16

	inc XL		
	andi XL,0x1f	;wrap to 32 tiles horizontally

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16

	or XL,r19	;restore higher bits
	movw ZL,r0	;load next tile adress
.endr

	ret


#if MODE2_HORIZONTAL_SCROLLING == 1

;******* OFFSET 1 ********************
mode2_render_line_offset_1:
	;r1:r0 = Tile row index
	movw ZL,r0
	
	adiw ZL,1 ;add tile x-scroll offset

	ld r12,X 	;load next tile # from VRAM

	rjmp .
	rjmp .
	rjmp .
	rjmp .

	;first out must be at cycle 14
.rept 22
	
	lpm r16,Z+	 ;load bg pixel
	ld  r13,Y+   ;load sprite pixel
	cpse r13,r17 ;check transparency
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 2

	mul r12,r18 ;tile*width*height
	
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 3

	add r0,r20	;add title table address
	adc r1,r21
	
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 4

	add r0,24	;add tile table row offset 
	adc r1,25 ;add tile table row offset 

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 5

	inc XL		
	andi XL,0x1f	;wrap to 32 tiles horizontally

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 6

	or XL,r19	;restore higher bits
	movw ZL,r0	;load next tile adress

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 1
	
	ld	r12,X 	;load next tile # from VRAM
.endr
	ret




;******* OFFSET 2 ********************
mode2_render_line_offset_2:
	;r1:r0 = Tile row index
	movw ZL,r0
	
	adiw ZL,2 ;add tile x-scroll offset
	
	ld r12,X 	;load next tile # from VRAM
	mul r12,r18 ;tile*width*height
	
	rjmp .
	rjmp .
	rjmp .

	;first out must be at cycle 14
.rept 22
		
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 3

	add r0,r20	;add title table address
	adc r1,r21
	
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 4

	add r0,24	;add tile table row offset 
	adc r1,25 ;add tile table row offset 

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 5

	inc XL		
	andi XL,0x1f	;wrap to 32 tiles horizontally

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 6

	or XL,r19	;restore higher bits
	movw ZL,r0	;load next tile adress

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 1
	
	ld	r12,X 	;load next tile # from VRAM

	lpm r16,Z+	 ;load bg pixel
	ld  r13,Y+   ;load sprite pixel
	cpse r13,r17 ;check transparency
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 2

	mul r12,r18 ;tile*width*height
.endr
	ret



;******* OFFSET 3 ********************
mode2_render_line_offset_3:
	;r1:r0 = Tile row index
	movw ZL,r0
	
	adiw ZL,3 ;add tile x-scroll offset
	
	ld r12,X 	;load next tile # from VRAM
	mul r12,r18 ;tile*width*height
	add r0,r20	;add title table address
	adc r1,r21
	
	rjmp .
	rjmp .

	;first out must be at cycle 14
.rept 22
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 4

	add r0,24	;add tile table row offset 
	adc r1,25 ;add tile table row offset 

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 5

	inc XL		
	andi XL,0x1f	;wrap to 32 tiles horizontally

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 6

	or XL,r19	;restore higher bits
	movw ZL,r0	;load next tile adress

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 1
	
	ld	r12,X 	;load next tile # from VRAM

	lpm r16,Z+	 ;load bg pixel
	ld  r13,Y+   ;load sprite pixel
	cpse r13,r17 ;check transparency
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 2

	mul r12,r18 ;tile*width*height

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 3

	add r0,r20	;add title table address
	adc r1,r21
.endr
	ret




;******* OFFSET 4 ********************
mode2_render_line_offset_4:
	;r1:r0 = Tile row index
	movw ZL,r0
	
	adiw ZL,4 ;add tile x-scroll offset
	
	ld r12,X 	;load next tile # from VRAM
	mul r12,r18 ;tile*width*height
	add r0,r20	;add title table address
	adc r1,r21
	add r0,24	;add tile table row offset 
	adc r1,25 ;add tile table row offset 
	
	rjmp .

	;first out must be at cycle 14
.rept 22
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 5

	inc XL		
	andi XL,0x1f	;wrap to 32 tiles horizontally

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 6

	or XL,r19	;restore higher bits
	movw ZL,r0	;load next tile adress

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 1
	
	ld	r12,X 	;load next tile # from VRAM

	lpm r16,Z+	 ;load bg pixel
	ld  r13,Y+   ;load sprite pixel
	cpse r13,r17 ;check transparency
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 2

	mul r12,r18 ;tile*width*height

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 3

	add r0,r20	;add title table address
	adc r1,r21
	
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 4

	add r0,24	;add tile table row offset 
	adc r1,25 ;add tile table row offset 
.endr
	ret 




;******* OFFSET 5 ********************
mode2_render_line_offset_5:
	;r1:r0 = Tile row index
	movw ZL,r0
	
	adiw ZL,5 ;add tile x-scroll offset
	
	ld r12,X 	;load next tile # from VRAM
	mul r12,r18 ;tile*width*height
	add r0,r20	;add title table address
	adc r1,r21
	add r0,24	;add tile table row offset 
	adc r1,25 ;add tile table row offset 
	inc XL		
	andi XL,0x1f	;wrap to 32 tiles horizontally	
	

	;first out must be at cycle 14
.rept 22
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 6

	or XL,r19	;restore higher bits
	movw ZL,r0	;load next tile adress

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 1
	
	ld	r12,X 	;load next tile # from VRAM

	lpm r16,Z+	 ;load bg pixel
	ld  r13,Y+   ;load sprite pixel
	cpse r13,r17 ;check transparency
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 2

	mul r12,r18 ;tile*width*height

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 3

	add r0,r20	;add title table address
	adc r1,r21
	
	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 4

	add r0,24	;add tile table row offset 
	adc r1,25 ;add tile table row offset 

	lpm r16,Z+
	ld  r13,Y+
	cpse r13,r17
	mov r16,r13
	out _SFR_IO_ADDR(DATA_PORT),r16 ;Tile Pixel 5

	inc XL		
	andi XL,0x1f	;wrap to 32 tiles horizontally
.endr
	ret 


#endif

SetSpritesTileTable:
	sts sprites_tiletable_lo,r24
	sts sprites_tiletable_hi,r25
	ret


;*****************************
; Configure global sprites options
; C-callable
; r24=bit field of options
;*****************************
SetSpritesOptions:
	sts spritesOptions,r24
	ret

;*****************************
; Preprocess sprites visibility & positions
;*****************************
ProcessSprites:
	push r14
	push r15
	push r16
	push r17
	push YL
	push YH


	ldi ZL,lo8(screenSections)
	ldi ZH,hi8(screenSections)

	;*****************************
	;prepare scroll sections
	;******************************
	ldi r18,0
pss:
	mov r24,r18
	ldd r22,Z+scrollX
	ldd r20,Z+scrollY
	
	push ZL
	push ZH
	push r18
	call SetScrolling
	pop r18
	pop ZH
	pop ZL	

	adiw ZL,SCREEN_SECTION_STRUCT_SIZE	
	inc r18
	cpi r18,SCREEN_SECTIONS_COUNT
	brlo pss




	;*****************************
	;Clear sprites per line buffer
	;******************************
	clr r21 ;current section scanline
	ldi r18,SCREEN_SECTIONS_COUNT
	clr r22  ;zero register
	ldi r23,MAX_SPRITES_PER_LINE
	ldi r24,lo8(sprites_per_lines)
	ldi r25,hi8(sprites_per_lines)	
	ldi ZL,lo8(screenSections)
	ldi ZH,hi8(screenSections)

ps_clr1:	
	ldd r19,Z+flags	
	ldd r20,Z+sectionHeight

	movw YL,r24
	mul r21,23
	add YL,r0
	adc YH,r1

	add r21,r20

	tst r20	;skip if section height is zero
	breq ps_no_erase1

	andi r19,SCT_PRIORITY_SPR
	breq ps_no_erase1

ps_erase1:
	st Y+,r22
	st Y+,r22
	st Y+,r22
	st Y+,r22
	st Y+,r22
	dec r20
	brne ps_erase1

ps_no_erase1:
	adiw ZL,SCREEN_SECTION_STRUCT_SIZE	
	dec r18
	brne ps_clr1


	;*****************************
	;check if we need to do sprite clip or flick
	;******************************
	ldi r19,1	;sprNo
	lds r18,spritesOptions
	cpi r18,SPR_OVERFLOW_ROTATE
	brne no_flick
	lds r19,rotate_spr_no
no_flick:
	


	;*****************************
	;process sprites
	;******************************
	
	ldi r16,lo8(sprites)
	ldi r17,hi8(sprites)
	movw r14,r16

	ldi r18,SPRITE_STRUCT_SIZE
	ldi r26,MAX_SPRITES_PER_LINE
	ldi r27,MAX_SPRITES
	 
	ldi r20,1
ps_main_loop:
	movw ZL,r14 ;get sprites array adress
	mul r19,r18 ;spriteNo*sprite struct size
	add ZL,r0
	adc ZH,r1	

	ldd r16,Z+sprPosX ;sx
	ldd r17,Z+sprPosY ;sy
	
	cpi r17,0
	breq ps_main_end
	cpi r17,(SCREEN_TILES_V+1)*TILE_HEIGHT
	brsh ps_main_end
	cpi r16,0
	breq ps_main_end
	cpi r16,(SCREEN_TILES_H+1)*TILE_WIDTH
	brsh ps_main_end

	ldi r22,0	;cy
ps_inner_loop:
	mov r23,r17 ;compute sy+cy	
	add r23,r22
		
	cpi r23,TILE_HEIGHT ;(8) screen's top clipping	
	brlo ps_inner_end
	cpi r23,((SCREEN_TILES_V+1)*TILE_HEIGHT)
	brsh ps_inner_end

	subi r23,TILE_HEIGHT ; compute disp=sy+cy-8
	movw YL,r24
	mul r23,r26	;disp*max sprites per lines
	add YL,r0
	adc YH,r1

slot0:
	ldd r0,Y+0
	tst r0
	brne slot1
	mul r22,r27 ;(cy<<5)+sprNo
	add r0,r19
	std Y+0,r0
	rjmp ps_inner_end

slot1:
	ldd r0,Y+1
	tst r0
	brne slot2
	mul r22,r27 ;(cy<<5)+sprNo
	add r0,r19
	std Y+1,r0
	rjmp ps_inner_end

slot2:
	ldd r0,Y+2
	tst r0
	brne slot3
	mul r22,r27 ;(cy<<5)+sprNo
	add r0,r19
	std Y+2,r0
	rjmp ps_inner_end

slot3:
	ldd r0,Y+3
	tst r0
	brne slot4
	mul r22,r27 ;(cy<<5)+sprNo
	add r0,r19
	std Y+3,r0
	rjmp ps_inner_end

slot4:
	ldd r0,Y+4
	tst r0
	brne ps_inner_end
	mul r22,r27 ;(cy<<5)+sprNo
	add r0,r19
	std Y+4,r0
	
	mov r0,r19	;	rotateSprNo=sprNo-1;
	dec r0
	sts rotate_spr_no,r0


ps_inner_end:
	adiw YL,5
	inc r23
	inc r22
	cpi r22,TILE_HEIGHT
	brlo ps_inner_loop

ps_main_end:

	inc r19			;sprNo
	cpi r19,MAX_SPRITES
	brlo .+2
	ldi r19,1		;wrap back to sprite 1

	inc r20
	cpi r20,MAX_SPRITES
	brsh ps_end 
	rjmp ps_main_loop 
ps_end:

	lds r23,rotate_spr_no
	cpi r23,MAX_SPRITES
	brlo .+2
	ldi r23,1
	sts rotate_spr_no,r23



	;*****************************
	;Clear sprites per line buffer
	;******************************
	clr r21 ;current section scanline
	ldi r18,SCREEN_SECTIONS_COUNT
	clr r22  ;zero register
	ldi r23,MAX_SPRITES_PER_LINE
	ldi ZL,lo8(screenSections)
	ldi ZH,hi8(screenSections)

ps_clr2:	
	ldd r19,Z+flags	
	ldd r20,Z+sectionHeight

	movw YL,r24
	mul r21,23
	add YL,r0
	adc YH,r1

	add r21,r20

	tst r20	;skip if section height is zero
	breq ps_no_erase2

	andi r19,SCT_PRIORITY_SPR
	brne ps_no_erase2

ps_erase2:
	st Y+,r22
	st Y+,r22
	st Y+,r22
	st Y+,r22
	st Y+,r22
	dec r20
	brne ps_erase2

ps_no_erase2:
	adiw ZL,SCREEN_SECTION_STRUCT_SIZE	
	dec r18
	brne ps_clr2

	clr r1

	pop YH
	pop YL
	pop r17
	pop r16
	pop r15
	pop r14

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
	//sts tile_table_lo,r24
	//sts tile_table_hi,r25
	sts screenSections+tileTableAdressLo,r24
	sts screenSections+tileTableAdressHi,r25	
	ret

