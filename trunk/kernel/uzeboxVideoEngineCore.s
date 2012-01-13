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
	Changes
	---------------------------------------------
	V2.0:
	-Sprite engine
	-Reset console with joypad
	-ReadJoypad() now return int instead of char
	-NTSC timing more accurate
	-Use of conditionals (see defines.h)
	-Many small improvements

	V3.0
	-Major Refactoring: All video modes in their own files
	-New video modes: 3,4,6,7,8
	-EEPROM functions
	-Assembly functions in their own sections to save flash
	-Added Vsync User callback
	-UART Receive buffer & functions
	-Color burst offset control

	V3.2
	-Rewrote sync code
	-Use interrupt to pull back sync line for serration pulses
	-Added inline mixer selectable with a compile switch
	-Added channel 5 PCM (avail with inline mixer only)
	-Fixed the "click" sound upon game resets
	-Removed color burst offset code
	-Removed RAM patch code in music engine

*/

#include <avr/io.h>
#include "defines.h"

;Global delay macro for 0 to 1275 (old:767) cycles
;Parameters: reg=Registerto use in inner loop (will be destroyed)
;            clocks=CPU clocks to wait
.macro WAIT reg,clocks	
	.if (\clocks) > 767
	 	ldi	\reg, (\clocks)/6    
	 	dec	\reg
		jmp . 
	 	brne .-8
		.if ((\clocks) % 6) == 1
			nop
		.elseif ((\clocks) % 6) == 2
			rjmp .
		.elseif ((\clocks) % 6) == 3
			jmp .
		.elseif ((\clocks) % 6) == 4
			rjmp .
			rjmp .
		.elseif ((\clocks) % 6) == 5
			rjmp .
			jmp .
		.endif
	.else
		.if (\clocks) > 2
		 	ldi	\reg, (\clocks)/3    
		 	dec	\reg                    
		 	brne   .-4
		.endif
		.rept (\clocks) % 3
		 	nop
		.endr
	.endif
.endm 

;Public methods
.global TIMER1_COMPA_vect
.global TIMER1_COMPB_vect
.global GetVsyncFlag
.global ClearVsyncFlag
.global ReadJoypad
.global ReadJoypadExt
.global WriteEeprom
.global ReadEeprom
.global WaitUs
.global SetUserPreVsyncCallback
.global SetUserPostVsyncCallback
.global UartInitRxBuffer
.global IsRunningInEmulator

;Public variables
.global sync_pulse
.global sync_phase
.global vsync_phase
.global joypad1_status_lo
.global joypad2_status_lo
.global joypad1_status_hi
.global joypad2_status_hi
.global first_render_line_tmp
.global render_lines_count_tmp
.global first_render_line
.global render_lines_count


;*** IMPORTANT ***
;Some video modes MUST have some variables aligned on a 8-bit boundary.
;This is done by putting uzeboxVideoEngineCore.o as first in the linking 
;phase and insure a location of 0x100.
#include VMODE_ASM_SOURCE

.section .bss
	.align 1

	sync_phase:  .byte 1	;0=vsync, 1=hsync
	sync_pulse:	 .byte 1	;scanline counter
	vsync_flag:  .byte 1	;set  @ 60Hz np

	pre_vsync_user_callback:  .word 1 ;pointer to function
	post_vsync_user_callback: .word 1 ;pointer to function

	first_render_line:		.byte 1
	render_lines_count: 	.byte 1

	first_render_line_tmp:	.byte 1
	render_lines_count_tmp: .byte 1

	
	;last read results of joypads
	joypad1_status_lo:	.byte 1
						.byte 1
	joypad1_status_hi:	.byte 1
						.byte 1

	joypad2_status_lo:	.byte 1
						.byte 1
	joypad2_status_hi:	.byte 1
						.byte 1


.section .text

;***************************************************************************
; Main Video sync interrupt
;***************************************************************************
TIMER1_COMPA_vect:
	push r0
	push r1
	push ZL;2
	push ZH;2
	
	;save flags & status register
	in ZL,_SFR_IO_ADDR(SREG);1
	push ZL ;2		

	;Read timer offset since rollover to remove cycles 
	;and conpensate for interrupt latency.
	;This is nessesary to eliminate frame jitter.
	lds ZL,_SFR_MEM_ADDR(TCNT1L)
	subi ZL,0x12 ;MIN_INT_LATENCY

	ldi ZH,1
latency_loop:
	cp ZL,ZH
	brlo .		;advance PC to next instruction	
	inc ZH
	cpi ZH,10
	brlo latency_loop
	jmp .
	
	;increment sync pulse counter
	lds ZL,sync_pulse
	dec ZL
	sts sync_pulse,ZL

	;process sync phases
	lds ZH,sync_phase
	sbrc ZH,0
	rjmp sync_hsync
		

;***************************************************
; VSYNC PRE-EQ pulse generation
; Note: TCNT1 should be equal to 
; 0x68 on the cbi
; 0xAC on the sbi
; pulse duration: 68 clocks
;***************************************************		
	cpi ZL,SYNC_EQ_PULSES+SYNC_POST_EQ_PULSES
	brlo sync_eq

	;Set HDRIVE to double rate during VSYNC
	ldi ZH,hi8(HDRIVE_CL_TWICE)
	sts _SFR_MEM_ADDR(OCR1AH),ZH	
	ldi ZH,lo8(HDRIVE_CL_TWICE)
	sts _SFR_MEM_ADDR(OCR1AL),ZH

	bst ZL,0
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN	;TCNT1=0x68
	brtc sync_pre_eq_no_sound_update
	ldi ZL,1	;indicate update_sound to generate the SBI for pre-eq
	call update_sound
	rjmp sync_end

sync_pre_eq_no_sound_update:
	WAIT ZL,64
	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN	;TCNT1=0xAC
		
	rjmp sync_end
	
	
;***************************************************
; SYNC EQ pulse generation
; Note: TCNT1 should be equal to 
; 0x68  on the cbi
; 0x36E on the sbi
; low pulse duration: 774 clocks
;***************************************************	
sync_eq:
	cpi ZL,SYNC_POST_EQ_PULSES
	brlo sync_post_eq

	rjmp .
	rjmp .

	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2

	bst ZL,0
	ldi ZL,4
	brtc sync_eq_skip
	
	call update_sound

sync_eq_skip:
	;enable interupt to bring back sync 
	;level instead of wasting cycles
	;in a big wait loop

 	;clear interrupt flag
	ldi ZL,(1<<OCF1B)
	sts _SFR_MEM_ADDR(TIFR1),ZL 
	
	;generate interrupt on match
	;for timer1 compare unit b
	ldi ZL,(1<<OCIE1A)+(1<<OCIE1B)
	sts _SFR_MEM_ADDR(TIMSK1),ZL

	rjmp sync_end

;**********************************************************
; Interrupt that set the sync signal back to .3v
; during VSYNC EQ pulses to recover ~5000 cycles per field
; with interrupt latency conpensation
; 37 cycles
;**********************************************************	
TIMER1_COMPB_vect:
	push ZL
	;save flags & status register
	in ZL,_SFR_IO_ADDR(SREG);1
	push ZL ;2		

	lds ZL,_SFR_MEM_ADDR(TCNT1L)
	subi ZL,62+31 ;0x5D ;MIN_INT_LATENCY

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

 	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;68
	ldi ZL,(1<<OCIE1A) ; disable OCIE1B 
	sts _SFR_MEM_ADDR(TIMSK1),ZL ;stop generate interrupt on match
	
	;restore flags
	pop ZL
	out _SFR_IO_ADDR(SREG),ZL	
	pop ZL
	reti
	
	
;***************************************************
; SYNC POST EQ pulse generation
; Note: TCNT1 should be equal to 
; 0x68 on the cbi
; 0xAC on the sbi
; pulse cycles: 68 clocks
;***************************************************	
sync_post_eq:	
	rjmp .

	bst ZL,0
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2
	brtc sync_post_eq_no_sound_update
	ldi ZL,1	
	call update_sound
	rjmp sync_pre_eq_cont

sync_post_eq_no_sound_update:
	WAIT ZL,64

	sbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN 

sync_pre_eq_cont:	
	;check if it's the last vsync pulse
	lds ZL,sync_pulse
	cpi ZL,0
	breq .+2 ;skip rjmp
	rjmp sync_end
	
	;update sync flags
	ldi ZL,SYNC_HSYNC_PULSES
	sts sync_pulse,ZL
	ldi ZL,1
	sts sync_phase,ZL
	
	rjmp sync_end
	
	
sync_hsync:
;***************************************************
; HSYNC pulse generation
; Note: TCNT1 should be equal to 
; 0x68 on the cbi
; 0xF0 on the sbi
; pulse duration: 136 clocks
;***************************************************	

	; Set HDRIVE to normal rate
	ldi ZL,hi8(HDRIVE_CL)
	sts _SFR_MEM_ADDR(OCR1AH),ZL	
	ldi ZL,lo8(HDRIVE_CL)
	sts _SFR_MEM_ADDR(OCR1AL),ZL
	rjmp .

	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2
	
	ldi ZL,2	;indicate update_sound to generate the SBI for pre-eq
	rjmp .
	call update_sound

	;check if we have reached the first line to render
	ldi ZH,SYNC_HSYNC_PULSES
	lds r0,first_render_line
	sub ZH,r0				
	lds ZL,sync_pulse
	cp ZL,ZH
	brsh no_render

	ldi ZH,SYNC_HSYNC_PULSES
	lds r0,first_render_line
	sub ZH,r0				
	lds r0,render_lines_count
	sub ZH,r0			
	cp ZL,ZH
	brlo no_render

	;push r1-r29
	ldi ZL,29
	clr ZH
push_loop:
	ld r0,Z	;load value from register file
	push r0
	dec ZL
	brne push_loop	

	;timing compensation
	;to insure we always call the video mode 
	;routine at the same cycle	
	WAIT r16,18+212-AUDIO_OUT_HSYNC_CYCLES

	call VMODE_FUNC		;TCNT1=0x234

	;pop r1-r29
	ldi ZL,1
	clr ZH
pop_loop:
	pop r0
	st Z+,r0 ;store value to register file
	cpi ZL,30
	brlo pop_loop	

no_render:

	;check if it's the last hsync pulse and we are 
	;ready for VSYNC
	lds ZL,sync_pulse
	cpi ZL,0
	breq .+2
	rjmp sync_end

;***************************************************
; Process VSYNC stuff
;***************************************************
	
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

	sei ;must enable ints for re-entrant sync pulses
	clr r1

	;set vsync flags
	clr ZL
	sts sync_phase,ZL
	ldi ZL,SYNC_PRE_EQ_PULSES+SYNC_EQ_PULSES+SYNC_POST_EQ_PULSES
	sts sync_pulse,ZL

	;fetch render height registers if they changed	
	lds ZH,first_render_line_tmp
	sts first_render_line,ZH
	
	lds ZH,render_lines_count_tmp
	sts render_lines_count,ZH

	;process user pre callback
	lds ZL,pre_vsync_user_callback+0
	lds ZH,pre_vsync_user_callback+1
	cp  ZL,r1
	cpc ZH,r1
	breq .+2 
	icall

	;refresh buttons states
	#if CONTROLLERS_VSYNC_READ == 1
		call ReadControllers
	#endif 
	
	;invoke stuff the video mode may have to do
	call VideoModeVsync	

	;process music (music, envelopes, etc)
	call process_music
	clr r1

	;process user post callback
	lds ZL,post_vsync_user_callback+0
	lds ZH,post_vsync_user_callback+1
	cp  ZL,r1
	cpc ZH,r1
	breq .+2 
	icall

	#if SNES_MOUSE == 1
		call ReadMouseExtendedData
		call ProcessMouseMovement
	#endif

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
	
sync_end:	
	;restore flags
	pop ZL
	out _SFR_IO_ADDR(SREG),ZL
	
	pop ZH
	pop ZL
	pop r1
	pop r0
	reti


;*************************************************
; Generate a H-Sync pulse 
; Called by video modes when rendering a frame.
; Note: TCNT1 should be equal to 
; 0x68 on the cbi 
; 0xf0 on the sbi 
;	
; Destroys: ZL (r30)
;*************************************************
hsync_pulse:
	cbi _SFR_IO_ADDR(SYNC_PORT),SYNC_PIN ;2
	ldi ZL,2
	rjmp .
	call update_sound

	lds ZL,sync_pulse
	dec ZL
	sts sync_pulse,ZL

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
#if SNES_MOUSE == 1
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
#endif
	
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

;****************************
; Return true (1) if the program run on uzem
; C callable
;****************************
.section .text.IsRunningInEmulator
IsRunningInEmulator:
	ldi r26,0xaa
	sts 0xff,r26
	lds r25,0xff
	ldi r24,1
	cpse r25,r26	
	clr r24
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



