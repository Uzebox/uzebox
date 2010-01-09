/*
 *  Uzebox Kernel
 *  Copyright (C) 2008  Alec Bourque
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

/*
	V2 Changes:
	-Sprite engine
	-Reset console with joypad
	-ReadJoypad() now return int instead of char
	-NTSC timing more accurate
	-Use of conditionals (see defines.h)
	-Many small improvements

*/

#include <avr/io.h>
#include "defines.h"



;Sprites Struct offsets
#define sprPosX  0
#define sprPosY  1
#define sprIndex 2

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


;Public methods
.global TIMER1_COMPA_vect
.global SetTile
.global SetFont
.global RestoreTile
.global LoadMap
.global ClearVram


.global GetVsyncFlag
.global ClearVsyncFlag
.global SetTileTable
.global SetFontTable
.global SetTileMap
.global ReadJoypad
.global read_joypads

;Public variables
.global vram
.global sync_pulse
.global sync_phase
.global curr_field


#if VIDEO_MODE == 2
.global rotate_spr_no
.global scanline_sprite_buf
.global	sprites_per_lines
.global	sprites
.global SetSpritesTileTable
.global screenSections
.global SetSpritesOptions
.global ProcessSprites
#endif

#if VRAM_ADDR_SIZE == 1
.global SetFontTilesIndex
#endif

.section .data
	#if VIDEO_MODE == 2

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
	#endif

.section .bss
	.align 5
	vram: 	  				.space VRAM_SIZE ;MUST be aligned to 32 bytes
	

.section .bss
	.align 1

	#if VIDEO_MODE == 2
		sprites:			.space 32*SPRITE_STRUCT_SIZE ;|X|Y|TILE INDEX|	
		screenSections:		.space SCREEN_SECTION_STRUCT_SIZE*SCREEN_SECTIONS_COUNT
		spritesOptions:		.byte 1		;b0 overflow: 0=clip, 1=flick
		
		sprites_tiletable_lo: .byte 1
		sprites_tiletable_hi: .byte 1

		sprites_per_lines:	.space (SCREEN_TILES_V)*TILE_HEIGHT*MAX_SPRITES_PER_LINE ;|Y-offset(3bits)|Sprite No(5bits)|
		sprite_buf_erase:	.space MAX_SPRITES_PER_LINE; ;4x8 bit pointers
		rotate_spr_no:		.byte 1	
	#endif


	sync_phase:  .byte 1 ;0=pre-eq, 1=eq, 2=post-eq, 3=hsync, 4=vsync
	sync_pulse:	 .byte 1
	vsync_flag: .byte 1	;set 30 hz
	curr_field:	 .byte 1	;0 or 1, changes at 60hz


	tile_table_lo:	.byte 1
	tile_table_hi:	.byte 1


	#if VRAM_ADDR_SIZE == 1
		font_tile_index:.byte 1 

	#else
		tile_map_lo:	.byte 1
		tile_map_hi:	.byte 1

		font_table_lo:	.byte 1
		font_table_hi:	.byte 1		
	#endif 

	;last read results of joypads

	joypad1_status_lo:	.byte 1
	joypad1_status_hi:	.byte 1
	

	joypad2_status_lo:	.byte 1
	joypad2_status_hi:	.byte 1


.section .text
	
	sync_func_vectors:	.word pm(do_pre_eq)
						.word pm(do_eq)
						.word pm(do_post_eq)
						.word pm(do_hsync)


#if VIDEO_MODE == 2

;***************************************************
; Mode 2: Tile & Sprites Video Mode
; Process video frame in tile mode (24*28) 
; -with sprites
;***************************************************	

sub_video_mode2:

	;waste line to align with next hsync in render function
	ldi ZL,216+3
m2_render_delay:
	lpm
	nop
	dec ZL
	brne m2_render_delay 

	rjmp .
	rjmp .

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

	;set vsync flag if beginning of next frame (each two fields)
	ldi r17,1
	lds r16,curr_field
	eor r16,r17
	sts curr_field,r16

	;set vsync flag if beginning of next frame
	ldi ZL,1
	sts vsync_flag,ZL

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
	;0x44 on the cbi 
	;0xcd on the sbi 
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

#endif



	

#if VIDEO_MODE == 1

;***************************************************
; TEXT MODE VIDEO PROCESSING
; Process video frame in tile mode (40*28)
;***************************************************	

sub_video_mode1:

	;waste line to align with next hsync in render function
	ldi ZL,222 //200-20+22+19+1
mode0_render_delay:
	lpm
	nop
	dec ZL
	brne mode0_render_delay 

	nop
	nop
	nop
	nop
	nop
	nop

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16
	clr r22
	ldi r23,TILE_WIDTH ;tile width in pixels



next_text_line:	
	rcall hsync_pulse ;3+144=147

	ldi r19,41-5 + CENTER_ADJUSTMENT
text_wait1:
	dec r19			
	brne text_wait1;206

	;***draw line***
	call render_tile_line

	ldi r19,10+5 - CENTER_ADJUSTMENT
text_wait2:
	dec r19			
	brne text_wait2;233

	
	nop


	dec r10
	breq text_frame_end
	

	nop
	inc r22

	nop
	nop

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
	ldi r19,VRAM_TILES_H*2
	add YL,r19
	adc YH,r0


	;nop
	;nop
	nop
	nop
	nop

	nop
	rjmp next_text_line

text_frame_end:
	;13
	lpm ;3 nop
	lpm ;3 nop
	lpm ;3 nop
	lpm ;3 nop
	lpm ;3 nop
	nop
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
; r22     = Y offset in tiles
; r23 	  = tile width in bytes
; Y       = VRAM adress to draw from (must not be modified)
;
; Can destroy: r0,r1,r2,r3,r4,r5,r13,r16,r17,r18,r19,r20,r21,Z
; 
; cycles  = 1495
;*************************************************
render_tile_line:
	push r24
	push r25

	movw XL,YL

	;add tile Y offset
	mul r22,r23
	movw r24,r0
	nop

	;load the first tile from vram
	ld	r20,X+	;load tile adress LSB from VRAM
	ld	r21,X+	;load tile adress MSB from VRAM
	add r20,r24	;add tile address
	adc r21,r25  ;add tile address	


	movw ZL,r20
	;draw 40 tiles wide, 6 clocks/pixel
.rept 40
	
	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16
	
	ld	r20,X+	;load tile adress # LSB from VRAM

	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16

	ld	r21,X+	;load tile adress # MSB from VRAM

	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16

	rjmp .

	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16

	rjmp .

	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16

	add r20,r24	;add tile table row offset 
	adc r21,r25 ;add tile table row offset 


	lpm r16,Z+
	out _SFR_IO_ADDR(DATA_PORT),r16

	nop
	movw ZL,r20

	

.endr

	;end set last pix to zero
	nop
	clr r16
	nop
	out _SFR_IO_ADDR(DATA_PORT),r16




	ldi r16,4+2+2
rtl2:
	dec r16
	brne rtl2

	nop
	nop
	nop

	pop r25
	pop r24
	ret
#endif


;************
; HSYNC
;************
do_hsync:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ; HDRIVE sync pulse low

	call update_sound_buffer ;36 -> 63

	ldi ZL,32-9
do_hsync_delay:
	dec ZL
	brne do_hsync_delay ;135

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;136

	rcall set_normal_rate_HDRIVE


	ldi ZL,SYNC_PHASE_PRE_EQ
	ldi ZH,SYNC_PRE_EQ_PULSES

	rcall update_sync_phase

	sbrs ZL,0
	rcall render

	sbrs ZL,0
	rjmp not_start_of_frame


	;call sound mixing if first vsync pulse
	call MixSound

not_start_of_frame:


	ret


;*****************************************
; READ JOYPADS
; read 60 time/sec before redrawing screen
;*****************************************

read_joypads:

	//latch data
	sbi _SFR_IO_ADDR(JOYPAD_OUT_PORT),JOYPAD_LATCH_PIN
	jmp . ; wait ~200ns
	jmp . ;(6 cycles)
	cbi _SFR_IO_ADDR(JOYPAD_OUT_PORT),JOYPAD_LATCH_PIN
	
	//clear button state bits
	clr r20 //P1
	clr r21

	clr r22 //P2
	clr r23

	ldi r24,12 //number of buttons to fetch
	
	jmp .
	jmp .
	jmp .
	jmp .
	jmp .
	jmp .
;29
	
read_joypads_loop:	
	;read data pin

	lsl r20
	rol r21
	sbic _SFR_IO_ADDR(JOYPAD_IN_PORT),JOYPAD_DATA1_PIN
	ori r20,1
	
	lsl r22
	rol r23
	sbic _SFR_IO_ADDR(JOYPAD_IN_PORT),JOYPAD_DATA2_PIN
	ori r22,1
	

	;pulse clock pin
	sbi _SFR_IO_ADDR(JOYPAD_OUT_PORT),JOYPAD_CLOCK_PIN
	jmp . ;wait 6 cycles
	jmp .
	cbi _SFR_IO_ADDR(JOYPAD_OUT_PORT),JOYPAD_CLOCK_PIN

	jmp .
	jmp .
	jmp .
	jmp .

	dec r24
	brne read_joypads_loop ;232

	com r20 
	com r21
	com r22
	com r23

#if JOYSTICK == TYPE_NES
	;Do some bit transposition
	bst r21,3
	bld r20,3
	bst r21,2
	bld r21,3
	andi r21,0b00001011
	andi r20,0b11111000

	bst r23,3
	bld r22,3
	bst r23,2
	bld r23,3
	andi r23,0b00001011
	andi r22,0b11111000

#elif JOYSTICK == TYPE_SNES
	andi r21,0b00001111
	andi r23,0b00001111
#endif 

	sts joypad1_status_lo,r20
	sts joypad1_status_hi,r21

	sts joypad2_status_lo,r22
	sts joypad2_status_hi,r23

	ret




;**** RENDER ****
render:
	push ZL

	lds ZL,sync_pulse
	cpi ZL,SYNC_HSYNC_PULSES-FIRST_RENDER_LINE
	brsh render_end
	cpi ZL,SYNC_HSYNC_PULSES-FIRST_RENDER_LINE-(SCREEN_TILES_V*TILE_HEIGHT)
	brlo render_end
	
	push r2
	push r3
	push r4
	push r5

	push r6
	push r7
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

	push r18
	push r19
	push r20
	push r21

	push r22
	push r23
	push r24
	push r25

	push XL
	push XH
	push YL
	push YH 
		
	
	#if VIDEO_MODE == 1
		call sub_video_mode1
	#else
		call sub_video_mode2
	#endif


	pop YH
	pop YL
	pop XH
	pop XL

	pop r25
	pop r24
	pop r23
	pop r22

	pop r21
	pop r20
	pop r19
	pop r18

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
	pop r7
	pop r6

	pop r5
	pop r4
	pop r3
	pop r2

render_end:
	pop ZL
	ret

;***************************************************************************
; Video sync interrupt
; 4 cycles to invoke 
;
;***************************************************************************
TIMER1_COMPA_vect:
	push ZH;2
	push ZL;2

	;save flags & status register
	in ZL,_SFR_IO_ADDR(SREG);1
	push ZL ;2		

	;Read timer offset since rollover to remove cycles 
	;and conpensate for interrupt latency.
	;This is nessesary to eliminate frame jitter.

	lds ZL,_SFR_MEM_ADDR(TCNT1L)
	subi ZL,0x0e ;MIN_INT_LATENCY

	cpi ZL,1
	brlo .		;advance PC to next instruction

	cpi ZL,2
	brlo .		;advance PC to next instruction

	cpi ZL,3
	brlo .		;advance PC to next instruction

 	cpi ZL,4
	brlo .		;advance PC to next instruction

	cpi ZL,5
	brlo .		;advance PC to next instruction

	cpi ZL,6
	brlo .		;advance PC to next instruction

	cpi ZL,7
	brlo .		;advance PC to next instruction
	
	cpi ZL,8
	brlo .		;advance PC to next instruction

	cpi ZL,9
	brlo .		;advance PC to next instruction

	rcall sync
	;call update_sound_buffer

	pop ZL
	out _SFR_IO_ADDR(SREG),ZL
	pop ZL
	pop ZH
	reti


;***************************************************
; Composite SYNC
;***************************************************

sync:
	push r0
	push r1
			
	ldi ZL,lo8(sync_func_vectors)	
	ldi ZH,hi8(sync_func_vectors)

	lds r0,sync_phase
	lsl r0 ;*2
	clr r1
	
	add ZL,r0
	adc ZH,r1
	
	lpm r0,Z+
	lpm r1,Z
	movw ZL,r0

	icall	;call sync functions

	pop r1
	pop r0
	ret


;*************************************************
; Generate a H-Sync pulse - 136 clocks (4.749us)
; Note: TCNT1 should be equal to 
; 0x44 on the cbi 
; 0xcc on the sbi 
;
; Cycles: 144
; Destroys: ZL (r30)
;*************************************************
hsync_pulse:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2
	
	call update_sound_buffer ;36 -> 63
	
	ldi ZL,21
	dec ZL 
	brne .-4



	lds ZL,sync_pulse
	dec ZL
	sts sync_pulse,ZL

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2

	nop
	nop

	ret


;**************************
; PRE_EQ pulse
; Note: TCNT1 should be equal to 
; 0x44 on the cbi
; 0x88 on the sbi
; pulse duration: 68 clocks
;**************************
do_pre_eq:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ; HDRIVE sync pulse low

	call update_sound_buffer_2 ;36 -> 63

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;68
	nop

	ldi ZL,SYNC_PHASE_EQ
	ldi ZH,SYNC_EQ_PULSES
	rcall update_sync_phase

	rcall set_double_rate_HDRIVE

	ret

;************
; Serration EQ
; Note: TCNT1 should be equal to 
; 0x44  on the cbi
; 0x34A on the sbi
; low pulse duration: 774 clocks
;************
do_eq:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ; HDRIVE sync pulse low

	call update_sound_buffer_2 ;36 -> 63

	ldi ZL,181-9+4
do_eq_delay:
	nop
	dec ZL
	brne do_eq_delay ;135

	nop
	nop

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;136

	ldi ZH,SYNC_POST_EQ_PULSES
	ldi ZL,SYNC_PHASE_POST_EQ
	rcall update_sync_phase
		
	ret

;************
; POST_EQ
; Note: TCNT1 should be equal to 
; 0x44 on the cbi
; 0x88 on the sbi
; pulse cycles: 68 clocks
;************
do_post_eq:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ; HDRIVE sync pulse low

	call update_sound_buffer_2 ;36 -> 63

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;68

	nop

	ldi ZL,SYNC_PHASE_HSYNC
	ldi ZH,SYNC_HSYNC_PULSES
	rcall update_sync_phase

	ret






;************
; update sync phase
; ZL=next phase #
; ZH=next phases's pulse count
;
; returns: ZL: 0=more pulses in phase
;              1=was the last pulse in phase
;***********
update_sync_phase:

	lds r0,sync_pulse
	dec r0
	lds r1,_SFR_MEM_ADDR(SREG)
	sbrc r1,SREG_Z
	mov r0,ZH
	sts sync_pulse,r0	;set pulses for next sync phase

	lds r0,sync_phase
	sbrc r1,SREG_Z
	mov r0,ZL			;go to next phase 
	sts sync_phase,r0
	
	ldi ZL,0
	sbrc r1,SREG_Z
	ldi ZL,1

	ret

;**************************************
; Set HDRIVE to double rate during VSYNC
;**************************************
set_double_rate_HDRIVE:

	ldi ZL,hi8(HDRIVE_CL_TWICE)
	sts _SFR_MEM_ADDR(OCR1AH),ZL
	
	ldi ZL,lo8(HDRIVE_CL_TWICE)
	sts _SFR_MEM_ADDR(OCR1AL),ZL

	ret

;**************************************
; Set HDRIVE to normal rate
;**************************************
set_normal_rate_HDRIVE:

	ldi ZL,hi8(HDRIVE_CL)
	sts _SFR_MEM_ADDR(OCR1AH),ZL
	
	ldi ZL,lo8(HDRIVE_CL)
	sts _SFR_MEM_ADDR(OCR1AL),ZL

	ret


/*******************************************************************
 *******************************************************************
 * Memory size dependent functions
 *******************************************************************
 *******************************************************************/

#if VRAM_ADDR_SIZE == 2
	;***********************************
	; CLEAR VRAM
	; Fill the screen with the specified tile
	; C-callable
	;************************************
	ClearVram:
		//init vram		
		ldi r30,lo8(VRAM_TILES_H*VRAM_TILES_V)
		ldi r31,hi8(VRAM_TILES_H*VRAM_TILES_V)

		ldi XL,lo8(vram)
		ldi XH,hi8(vram)


		lds r22,font_table_lo
		lds r23,font_table_hi

	fill_vram_loop:

		st X+,r22
		st X+,r23	
		sbiw r30,1
		brne fill_vram_loop

		clr r1

		ret

	;***********************************
	; LOAD Main map
	;************************************
	LoadMap:
		push r16
		push r17
		//init vram
	
		ldi r24,lo8(VRAM_TILES_H *VRAM_TILES_V)
		ldi r25,hi8(VRAM_TILES_H *VRAM_TILES_V)
		ldi XL,lo8(vram)
		ldi XH,hi8(vram)

		lds ZL,tile_map_lo
		lds ZH,tile_map_hi

		ldi r20,(TILE_WIDTH*TILE_HEIGHT) ;48

		lds r16,tile_table_lo
		lds r17,tile_table_hi

	load_map_loop:
		lpm r22,Z+ ;16
		lpm r23,Z+ ;17

		mul r22,r20
		movw r18,r0
		mul r23,r20
		add r19,r0

		add r18,r16
		adc r19,r17

		st X+,r18	;store tile adress
		st X+,r19

		sbiw r24,1
		brne load_map_loop

		clr r1

		pop r17
		pop r16
		ret

	;***********************************
	; RESTORE TILE
	; Copy a map tile # to the same position VRAM
	; C-callable
	; r24=X pos (8 bit)
	; r22=Y pos (8 bit)
	;************************************
	RestoreTile:
		clr r25
		clr r23
		clr r19
		ldi r18,VRAM_TILES_H*2
		mul r22,r18		;calculate Y line addr
		lsl r24
		add r0,r24		;add X offset
		adc r1,r19

		//load map tile #
		lds ZL,tile_map_lo
		lds ZH,tile_map_hi

		add ZL,r0
		adc ZH,r1
		lpm r20,Z+ 
		lpm r21,Z+ 

		ldi XL,lo8(vram)
		ldi XH,hi8(vram)
		add XL,r0
		adc XH,r1

		ldi r18,(TILE_WIDTH*TILE_HEIGHT)
		mul r20,r18
		movw r22,r0
		mul r21,r18
		add r23,r0

		lds r20,tile_table_lo
		lds r21,tile_table_hi

		add r22,r20
		adc r23,r21

		st X+,r22
		st X,r23

		clr r1

		ret

	;***********************************
	; SET FONT TILE
	; C-callable
	; r24=X pos (8 bit)
	; r22=Y pos (8 bit)
	; r20=Font tile No (8 bit)
	;************************************
	SetFont:
		clr r25
		clr r21

		ldi r18,VRAM_TILES_H
		lsl r18	

		mul r22,r18		;calculate Y line addr in vram
		lsl r24
		add r0,r24		;add X offset
		adc r1,r25
		ldi XL,lo8(vram)
		ldi XH,hi8(vram)
		add XL,r0
		adc XH,r1

		lds r22,font_table_lo
		lds r23,font_table_hi

		ldi r18,(TILE_WIDTH*TILE_HEIGHT)
		mul r20,r18
		add r22,r0
		adc r23,r1

		st X+,r22
		st X,r23

		clr r1
	
		ret


	;***********************************
	; SET TILE
	; C-callable
	; r24=X pos (8 bit)
	; r22=Y pos (8 bit)
	; r21:r20=Tile No (16 bit)
	;************************************
	SetTile:
		clr r25
		clr r23	

		;ldi r18,40*2
		ldi r18,VRAM_TILES_H
		lsl r18	

		mul r22,r18		;calculate Y line addr in vram
		lsl r24
		add r0,r24		;add X offset
		adc r1,r25
		ldi XL,lo8(vram)
		ldi XH,hi8(vram)
		add XL,r0
		adc XH,r1

		lds r22,tile_table_lo
		lds r23,tile_table_hi

		ldi r18,(TILE_WIDTH*TILE_HEIGHT)
		mul r20,r18
		add r22,r0
		adc r23,r1

		mul r21,r18
		add r23,r0

		st X+,r22
		st X,r23

		clr r1
	
		ret

	;*****************************
	; Defines a tile map
	; C-callable
	; r25:r24=pointer to tiles map
	;*****************************
	SetTileMap:
		//adiw r24,2
		sts tile_map_lo,r24
		sts tile_map_hi,r25

		ret

	;***********************************
	; Define the tile data source
	; C-callable
	; r25:r24=pointer to font tiles data
	;************************************
	SetFontTable:
		sts font_table_lo,r24
		sts font_table_hi,r25
	
		ret

#endif

#if VRAM_ADDR_SIZE == 1
	;***********************************
	; CLEAR VRAM 8bit
	; Fill the screen with the specified tile
	; C-callable
	;************************************
	ClearVram:
		//init vram		
		ldi r30,lo8(VRAM_TILES_H*VRAM_TILES_V)
		ldi r31,hi8(VRAM_TILES_H*VRAM_TILES_V)

		ldi XL,lo8(vram)
		ldi XH,hi8(vram)

		clr r22

	fill_vram_loop:
		st X+,r22
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

		#if VIDEO_MODE == 3
			add r20,RAM_TILES_COUNT
		#endif
		
		st X,r20

		clr r1
	
		ret

	;***********************************
	; Load the "Main Map" 8bit : unsupported
	;************************************
	LoadMap:
		ret

	;***********************************
	; SET FONT TILE
	; C-callable
	; r24=X pos (8 bit)
	; r22=Y pos (8 bit)
	; r20=Font tile No (8 bit)
	;************************************
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
 	SetFontTilesIndex:
		sts font_tile_index,r24
		ret

#endif



#if VIDEO_MODE == 2


;*****************************
; Configure global sprites options
; C-callable
; r24=bit field of options
;*****************************
SetSpritesOptions:
	sts spritesOptions,r24
	ret

;*****************************
; Defines where the sprites tile are defined.
; C-callable
; r25:r24=pointer to sprites pixel data.
;*****************************
SetSpritesTileTable:
	sts sprites_tiletable_lo,r24
	sts sprites_tiletable_hi,r25
	ret

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


#endif 

;***********************************
; Define the tile data source
; C-callable
; r25:r24=pointer to tiles data
;************************************
SetTileTable:
	sts tile_table_lo,r24
	sts tile_table_hi,r25
	
	ret


;************************************
; This flag is set on each VSYNC by
; the engine. This func is used to
; synchronize the programs on frame
; rate (30hz).
;
; C-callable
;************************************
GetVsyncFlag:
	lds r24,vsync_flag
	ret

;*****************************
; Clear the VSYNC flag.
; 
; C-callable
;*****************************
ClearVsyncFlag:
	clr r1
	sts vsync_flag,r1
	ret

;*****************************
; Return joypad 1 or 2 buttons status
; C-callable
; r24=joypad No (0 or 1)
; returns: (int) r25:r24
;*****************************
ReadJoypad:	
	tst r24
	brne rj_p2
		
	lds r24,joypad1_status_lo
	lds r25,joypad1_status_hi
	ret
rj_p2:
	lds r24,joypad2_status_lo
	lds r25,joypad2_status_hi	

	ret

;*****************************
; Sets CPU speed to 14.3Mhz
;*****************************
SetLowSpeed:
	ldi r24,0x80
	ldi r25,1
	sts _SFR_MEM_ADDR(CLKPR),r24
	sts _SFR_MEM_ADDR(CLKPR),r25
	ret

;*****************************
; Sets CPU speed to normal 
;*****************************
SetFullSpeed:
	ldi r24,0x80
	ldi r25,0
	sts _SFR_MEM_ADDR(CLKPR),r24
	sts _SFR_MEM_ADDR(CLKPR),r25
	ret

