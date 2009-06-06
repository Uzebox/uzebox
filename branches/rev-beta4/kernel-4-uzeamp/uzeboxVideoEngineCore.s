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

#define addr 0
#define tileIndex 2

;Sprites Struct offsets
#define sprPosX  0
#define sprPosY  1
#define sprTileIndex 2
#define sprFlags 3





;Public methods
.global TIMER1_COMPA_vect
.global TIMER1_COMPB_vect
.global SetTile
.global SetFont
.global RestoreTile
.global LoadMap
.global ClearVram
.global CopyTileToRam
.global GetVsyncFlag
.global ClearVsyncFlag
.global SetTileTable
.global SetFontTable
.global SetTileMap
.global ReadJoypad
.global ReadJoypadExt
.global BlitSprite
.global WriteEeprom
.global ReadEeprom
.global WaitUs
.global SetSpritesTileTable
.global SetColorBurstOffset

;Public variables
.global vram
.global curr_frame
.global sync_pulse
.global sync_phase
.global curr_field
.global tile_table_lo
.global sprites_tiletable_lo
.global burstOffset
.global vsync_phase
.global joypad1_status_lo
.global joypad2_status_lo
.global joypad1_status_hi
.global joypad2_status_hi
.global line_buffer


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

	#if VIDEO_MODE == 3

	#endif

/*
	#if VIDEO_MODE == 3
		ram_tiles:				.space RAM_TILES_COUNT*TILE_HEIGHT*TILE_WIDTH
		ram_tiles_restore:  	.space RAM_TILES_COUNT*3 ;vram addr|Tile
		sprites_tiletable_lo: 	.byte 1
		sprites_tiletable_hi: 	.byte 1	
		ScreenScrollX:			.byte 1
		ScreenScrollY:			.byte 1
		lastTileFirstPixel:		.byte 1
		vram_linear_buf:		.space 32*2
	#endif
*/

	#if VIDEO_MODE == 4
		textram:				.space (16 * 36)
		scroll:					.space 1
		scroll_hi:				.space 1
		scroll_v_fine:			.space 1
		scroll_h_fine:			.space 1
		tileheight:				.space 1
		textheight:				.space 1
	#endif



	sync_phase:  .byte 1 ;0=pre-eq, 1=eq, 2=post-eq, 3=hsync, 4=vsync
	sync_pulse:	 .byte 1
	vsync_flag:  .byte 1	;set 30 hz
	curr_field:	 .byte 1	;0 or 1, changes at 60hz
	curr_frame:  .byte 1	;odd or even frame

	vsync_phase:    .byte 1

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
						.byte 1
	joypad1_status_hi:	.byte 1
						.byte 1
	
	joypad2_status_lo:	.byte 1
						.byte 1
	joypad2_status_hi:	.byte 1
						.byte 1


	burstOffset:		.byte 1


.section .text
	
	sync_func_vectors:	.word pm(do_pre_eq)
						.word pm(do_eq)
						.word pm(do_post_eq)
						.word pm(do_hsync)

#if VIDEO_MODE == 1
	#include "videoMode1.s"
#endif

#if VIDEO_MODE == 2
	#include "videoMode2.s"
#endif

#if VIDEO_MODE == 3
	#include "videoMode3.s"
#endif

#if VIDEO_MODE == 4
	#include "videoMode4.s"
#endif

#if VIDEO_MODE == 7
	#include "videoMode7.s"
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





;**** RENDER ****
render:
	push ZL

	lds ZL,sync_pulse
	cpi ZL,SYNC_HSYNC_PULSES-FIRST_RENDER_LINE
	brsh render_end
#if VIDEO_MODE == 7
	cpi ZL,SYNC_HSYNC_PULSES-FIRST_RENDER_LINE-FRAME_LINES
#else
	cpi ZL,SYNC_HSYNC_PULSES-FIRST_RENDER_LINE-(SCREEN_TILES_V*TILE_HEIGHT)
#endif
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
	#elif VIDEO_MODE == 2
		call sub_video_mode2
	#elif VIDEO_MODE == 3
		call sub_video_mode3
	#elif VIDEO_MODE == 4
		call sub_video_mode4
	#elif VIDEO_MODE == 5
		call sub_video_mode5
	#elif VIDEO_MODE == 6
		call sub_video_mode6
	#elif VIDEO_MODE == 7
		call sub_video_mode7
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
; Interrupt that set the sync signal back to .3v
;*************************************************
TIMER1_COMPB_vect:
	push ZL

	;save flags & status register
	in ZL,_SFR_IO_ADDR(SREG);1
	push ZL ;2	

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;68
	lds ZL, _SFR_MEM_ADDR(TIMSK1)
	andi ZL,~(1<<OCIE1B)
	sts _SFR_MEM_ADDR(TIMSK1),ZL ;stop generate interrupt on match
	
	pop ZL
	out _SFR_IO_ADDR(SREG),ZL	
	
	pop ZL
	reti

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


	;set sync generator counter on TIMER1
	;ldi ZH,hi8(0x90+63)
	;ldi ZL,lo8(0x90+63)
	;rjmp up_pulse

	ret

up_pulse:
	;set sync generator counter on TIMER1
	sts _SFR_MEM_ADDR(OCR1BH),ZH
	sts _SFR_MEM_ADDR(OCR1BL),ZL	
	lds ZL,_SFR_MEM_ADDR(TIMSK1) ;generate interrupt on match
	ori ZL,(1<<OCIE1B)
	sts _SFR_MEM_ADDR(TIMSK1),ZL ;generate interrupt on match
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

	;set sync generator counter on TIMER1
	;ldi ZH,hi8(0x90+704)
	;ldi ZL,lo8(0x90+704)
	;rjmp up_pulse
			
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



	lds ZL,sync_pulse
	cpi ZL,(SYNC_POST_EQ_PULSES-1)
	brne noshift
	//cause a shift in the color burst phase
	//on odd frames (NTSC superframe?)
	lds ZL,curr_field
	cpi ZL,1
	nop
	
	lds ZH,burstOffset
	brne peq_odd
	lds ZH,burstOffset
	neg ZH
 peq_odd:

	ldi ZL,hi8(HDRIVE_CL_TWICE) //4
	sts _SFR_MEM_ADDR(OCR1AH),ZL	
	
	ldi ZL,lo8(HDRIVE_CL_TWICE) //4
	add ZL,ZH
	sts _SFR_MEM_ADDR(OCR1AL),ZL
	ret

noshift:
	;restore full line size
	ldi ZL,hi8(HDRIVE_CL_TWICE)
	sts _SFR_MEM_ADDR(OCR1AH),ZL	
	ldi ZL,lo8(HDRIVE_CL_TWICE)
	sts _SFR_MEM_ADDR(OCR1AL),ZL









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
	//.section text.LoadMap
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
	//.section text.RestoreTile
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
	//.section text.SetFont
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
	//.section text.SetTile
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
	//.section text.SetTileMap
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
	//.section text.SetFontTable
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

		ldi r22,RAM_TILES_COUNT


	fill_vram_loop:
		st X+,r22
		sbiw r30,1
		brne fill_vram_loop

		clr r1

		ret

	.rept 50
		fmulsu r20,r20
	.endr
		
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
			subi r20,~(RAM_TILES_COUNT-1)
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


#if VIDEO_MODE == 2 || VIDEO_MODE == 3
;*****************************
; Defines where the sprites tile are defined.
; C-callable
; r25:r24=pointer to sprites pixel data.
;*****************************
SetSpritesTileTable:
	sts sprites_tiletable_lo,r24
	sts sprites_tiletable_hi,r25
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


;***********************************
; Offset the color burst per field
; C-callable
; r24=burst offset in clock cycles
;************************************
SetColorBurstOffset:
	sts burstOffset,r24

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
	lds r25,joypad1_status_lo+1
	ret
rj_p2:
	lds r24,joypad2_status_lo
	lds r25,joypad2_status_lo+1	

	ret

;*****************************
; Return joypad 1 or 2 buttons status
; C-callable
; r24=joypad No (0 or 1)
; returns: (int) r25:r24
;*****************************
ReadJoypadExt:

	tst r24
	brne rj_p2m
		
	lds r24,joypad1_status_hi
	lds r25,joypad1_status_hi+1
	ret
rj_p2m:
	lds r24,joypad2_status_hi
	lds r25,joypad2_status_hi+1	
	ret

	
;****************************
; Wait for n microseconds
; r25:r24 - us to wait
; returns: void
;****************************
WaitUs:
	
wms_loop:	
	ldi r23,8
	dec 23
	brne .-4 ;~1 us
	nop
	sbiw r24,1
	brne wms_loop

	ret
	
;****************************
; Write byte to EEPROM
; extern void WriteEeprom(int addr,u8 value)
; r25:r24 - addr
; r22 - value to write
;****************************


WriteEeprom:
   ; Wait for completion of previous write
   sbic _SFR_IO_ADDR(EECR),EEPE
   rjmp WriteEeprom
   ; Set up address (r25:r24) in address register
   out _SFR_IO_ADDR(EEARH), r25
   out _SFR_IO_ADDR(EEARL), r24
   ; Write data (r22) to Data Register
   out _SFR_IO_ADDR(EEDR),r22
   cli
   ; Write logical one to EEMPE
   sbi _SFR_IO_ADDR(EECR),EEMPE
   ; Start eeprom write by setting EEPE
   sbi _SFR_IO_ADDR(EECR),EEPE
   sei
   ret


;****************************
; Read byte from EEPROM
; extern unsigned char ReadEeprom(int addr)
; r25:r24 - addr
; r24 - value read
;****************************

ReadEeprom:
   ; Wait for completion of previous write
   sbic _SFR_IO_ADDR(EECR),EEPE
   rjmp ReadEeprom
   ; Set up address (r25:r24) in address register
   out _SFR_IO_ADDR(EEARH), r25
   out _SFR_IO_ADDR(EEARL), r24
   ; Start eeprom read by writing EERE
   cli
   sbi _SFR_IO_ADDR(EECR),EERE
   ; Read data from Data Register
   in r24,_SFR_IO_ADDR(EEDR)
   sei
   ret


.global internal_spi_byte
internal_spi_byte:

	out _SFR_IO_ADDR(SPDR),r24
	ldi r25,5
	dec r25
	brne .-4 ;wait 15 cycles
	in r24,_SFR_IO_ADDR(SPSR) ;clear flag
	in r24,_SFR_IO_ADDR(SPDR) ;read next pixel


	ret

