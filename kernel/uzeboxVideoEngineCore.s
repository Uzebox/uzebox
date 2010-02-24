/*
 *  Uzebox Kernel
 *  Copyright (C) 2008-2009 Alec Bourque
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

	V3 Changes:
	-Major Refactoring: All video modes in their own files
	-New video modes: 3,4,6,7,8
	-EEPROM functions
	-Assembly functions in their own sections to save flash
	-Added Vsync User callback
	-UART Receive buffer & functions
	-Color burst offset control
*/

#include <avr/io.h>
#include "defines.h"


#define addr 0
#define tileIndex 2

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
.global SetUserPreVsyncCallback
.global SetUserPostVsyncCallback
.global UartInitRxBuffer
.global SetFontTilesIndex

;Public variables
.global curr_frame
.global sync_pulse
.global sync_phase
.global curr_field
.global tile_table_lo
.global tile_table_hi
.global burstOffset
.global vsync_phase
.global joypad1_status_lo
.global joypad2_status_lo
.global joypad1_status_hi
.global joypad2_status_hi


;*** IMPORTANT ***
;Some video modes MUST have some variables aligned on a 8-bit boudary.
;This is done by putting uzeboxVideoEngineCore.o as first in the linking 
;phase and insure a location of 0x100.
#include VMODE_ASM_SOURCE

.section .bss
	.align 1

	sync_phase:  .byte 1 ;0=pre-eq, 1=eq, 2=post-eq, 3=hsync, 4=vsync
	sync_pulse:	 .byte 1
	vsync_flag:  .byte 1	;set 30 hz
	curr_field:	 .byte 1	;0 or 1, changes at 60hz
	curr_frame:  .byte 1	;odd or even frame

	vsync_phase:    .byte 1

	pre_vsync_user_callback:  .word 1 ;pointer to function
	post_vsync_user_callback: .word 1 ;pointer to function

	tile_table_lo:	.byte 1
	tile_table_hi:	.byte 1


	#if VRAM_ADDR_SIZE == 1
		font_tile_index:.byte 1 
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
	rjmp not_vsync

	;invoke vsync stuff
	
	;push C-call registers
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27

	sei ;must enable ints for hsync pulses
	clr r1

	;process user pre callback
	lds ZL,pre_vsync_user_callback+0
	lds ZH,pre_vsync_user_callback+1
	cp  ZL,r1
	cpc ZH,r1
	breq .+2 
	icall

	;invoke stuff the video mode may have to do
	call VideoModeVsync	

	;refresh buttons states
	call ReadControllers
	call ProcessMouseMovement

	;process music (music, envelopes, etc)
	call MixSound
	clr r1

	;process user post callback
	lds ZL,post_vsync_user_callback+0
	lds ZH,post_vsync_user_callback+1
	cp  ZL,r1
	cpc ZH,r1
	breq .+2 
	icall

	pop r27
	pop r26
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	pop r20
	pop r19
	pop r18

not_vsync:
	ret





;**** RENDER ****
render:
	push ZL

	lds ZL,sync_pulse
	cpi ZL,SYNC_HSYNC_PULSES-FIRST_RENDER_LINE
	brsh render_end

	cpi ZL,SYNC_HSYNC_PULSES-FIRST_RENDER_LINE-FRAME_LINES
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
		
	call VMODE_FUNC

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
; WIP
;*************************************************
/*
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

up_pulse:
	;set sync generator counter on TIMER1
	sts _SFR_MEM_ADDR(OCR1BH),ZH
	sts _SFR_MEM_ADDR(OCR1BL),ZL	
	lds ZL,_SFR_MEM_ADDR(TIMSK1) ;generate interrupt on match
	ori ZL,(1<<OCIE1B)
	sts _SFR_MEM_ADDR(TIMSK1),ZL ;generate interrupt on match
	ret
*/

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

	rjmp .

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



#if VRAM_ADDR_SIZE == 1
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

		ldi r22,RAM_TILES_COUNT


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
		
		#if VIDEO_MODE == 3
			subi r20,~(RAM_TILES_COUNT-1)
		#endif

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

#endif


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


;************************************
; This flag is set on each VSYNC by
; the engine. This func is used to
; synchronize the programs on frame
; rate (30hz).
;
; C-callable
;************************************
.section .text.GetVsyncFlag
GetVsyncFlag:
	lds r24,vsync_flag
	ret

;*****************************
; Clear the VSYNC flag.
; 
; C-callable
;*****************************
.section .text.ClearVsyncFlag
ClearVsyncFlag:
	clr r1
	sts vsync_flag,r1
	ret


;***********************************
; Offset the color burst per field
; Optimal value is 4
; C-callable
; r24=burst offset in clock cycles
;************************************
.section .text.SetColorBurstOffset
SetColorBurstOffset:
	sts burstOffset,r24
	ret

;*****************************
; Return joypad 1 or 2 buttons status
; C-callable
; r24=joypad No (0 or 1)
; returns: (int) r25:r24
;*****************************
.section .text.ReadJoypad
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
.section .text.ReadJoypadExt
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
.section .text.WaitUs
WaitUs:	
	ldi r23,8
	dec 23
	brne .-4 ;~1 us
	nop
	sbiw r24,1
	brne WaitUs

	ret
	
;****************************
; Write byte to EEPROM
; extern void WriteEeprom(int addr,u8 value)
; r25:r24 - addr
; r22 - value to write
;****************************

.section .text.WriteEeprom
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
.section .text.ReadEeprom
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



;****************************
; Sets a callback that will be invoked at
; the very beginning of VSYNC, *before* the kernel's
; processing (ie:controllers reading, sound mixing).
; C callable
; r25:r24 - pointer to C function: void ptr*(void)
;****************************
.section .text.SetUserPreVsyncCallback
SetUserPreVsyncCallback:
	sts pre_vsync_user_callback+0,r24
	sts pre_vsync_user_callback+1,r25
	ret


;****************************
; Sets a callback that will be invoked at
; the very beginning of VSYNC, *after* the kernel
; processing.(ie:controllers reading, sound mixing).
; C callable
; r25:r24 - pointer to C function: void ptr*(void)
;****************************
.section .text.SetUserPostVsyncCallback
SetUserPostVsyncCallback:
	sts post_vsync_user_callback+0,r24
	sts post_vsync_user_callback+1,r25
	ret


//for debug
.global internal_spi_byte
.section .text.internal_spi_byte
internal_spi_byte:

	out _SFR_IO_ADDR(SPDR),r24
	ldi r25,5
	dec r25
	brne .-4 ;wait 15 cycles
	in r24,_SFR_IO_ADDR(SPSR) ;clear flag
	in r24,_SFR_IO_ADDR(SPDR) ;read next pixel
	ret



