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
	See http://code.google.com/p/uzebox/source/browse/trunk/README.txt
*/

#include <avr/io.h>
#include "defines.h"

;
; Global assembly delay macro for 0 to 1535 cycles
; Parameters: reg = Registerto use in inner loop (will be destroyed)
;             clocks = CPU clocks to wait
;
.macro WAIT reg, clocks
.if     (\clocks) >= 768
	ldi   \reg,    0
	dec   \reg
	brne  .-4
.endif
.if     ((\clocks) % 768) >= 9
	ldi   \reg,    ((\clocks) % 768) / 3
	dec   \reg
	brne  .-4
.if     ((\clocks) % 3) == 2
	rjmp  .
.elseif ((\clocks) % 3) == 1
	nop
.endif
.elseif ((\clocks) % 768) == 8
	lpm   \reg,    Z
	lpm   \reg,    Z
	rjmp  .
.elseif ((\clocks) % 768) == 7
	lpm   \reg,    Z
	rjmp  .
	rjmp  .
.elseif ((\clocks) % 768) == 6
	lpm   \reg,    Z
	lpm   \reg,    Z
.elseif ((\clocks) % 768) == 5
	lpm   \reg,    Z
	rjmp  .
.elseif ((\clocks) % 768) == 4
	rjmp  .
	rjmp  .
.elseif ((\clocks) % 768) == 3
	lpm   \reg,    Z
.elseif ((\clocks) % 768) == 2
	rjmp  .
.elseif ((\clocks) % 768) == 1
	nop
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
.global GetVsyncCounter
.global ClearVsyncCounter

;Public variables
.global sync_pulse
.global sync_phase
.global sync_flags
.global joypad1_status_lo
.global joypad2_status_lo
.global joypad1_status_hi
.global joypad2_status_hi
.global first_render_line
.global render_lines_count


;Includes the video mode selected by
;the -DVIDEO_MODE compile switch
#include VMODE_ASM_SOURCE

.section .bss
	.align 1

	sync_phase:  .space 1	;0=vsync, 1=hsync
	sync_pulse:	 .space 1	;scanline counter
	sync_flags:  .space 1	;b0: vsync flag, set at 60Hz when video frame rendered
							;b1: current field (0=odd, 1=even)

	pre_vsync_user_callback:  .space 2 ;pointer to function
	post_vsync_user_callback: .space 2 ;pointer to function

	first_render_line:		.space 1
	render_lines_count: 	.space 1

	
	;last read results of joypads
	joypad1_status_lo:	.space 1
						.space 1
	joypad1_status_hi:	.space 1
						.space 1

	joypad2_status_lo:	.space 1
						.space 1
	joypad2_status_hi:	.space 1
						.space 1

	vsync_counter:		.space 2
	
#if TRUE_RANDOM_GEN == 1
	random_value:			.space 2
#endif

.section .text



;***************************************************************************
; Main Video sync interrupt
;***************************************************************************
TIMER1_COMPA_vect:

	; (3 cy IT entry latency)
	; (3 cy JMP)

	push  r0
	push  r1
	push  ZL
	push  ZH
	in    ZH,      _SFR_IO_ADDR(SREG)
	lds   ZL,      _SFR_MEM_ADDR(TCNT1L) ; 0x10 - 0x15 (5 cy jitter)
	push  ZH

	sbrc  ZL,      2
	rjmp  .+8              ; 0x15 ( 5) or 0x14 ( 6)
	sbrc  ZL,      1
	rjmp  .+4              ; 0x13 ( 7) or 0x12 ( 8)
	nop
	rjmp  .                ; 0x11 ( 9) or 0x10 (10)
	sbrs  ZL,      0
	rjmp  .

	; An lds of TCNT1L here would result 0x1E

	;decrement sync pulse counter
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
;**********************************************************
TIMER1_COMPB_vect:
	push  ZL

	lds   ZL,      _SFR_MEM_ADDR(TCNT1L) ; 0x28 - 0x2D (5 cy jitter)

	sbrc  ZL,      2
	rjmp  .+8              ; 0x2D ( 5) or 0x2C ( 6)
	sbrc  ZL,      1
	rjmp  .+4              ; 0x2B ( 7) or 0x2A ( 8)
	nop
	rjmp  .                ; 0x29 ( 9) or 0x28 (10)
	sbrs  ZL,      0
	rjmp  .

	ldi   ZL,      (1 << OCIE1A) ; Disable OCIE1B
	nop
	sbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; 68
	sts   _SFR_MEM_ADDR(TIMSK1), ZL ; Stop generate interrupt on match

	pop   ZL
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
	WAIT r16,230-(AUDIO_OUT_HSYNC_CYCLES)

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



	;increment the vsync counter
	lds r24,vsync_counter
	lds r25,vsync_counter+1
	adiw r24,1
	sts vsync_counter,r24
	sts vsync_counter+1,r25


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
; rate (60hz).
;
; C-callable
;************************************
.section .text.GetVsyncFlag
GetVsyncFlag:
	lds r24,sync_flags
	andi r24,SYNC_FLAG_VSYNC
	ret

;*****************************
; Clear the VSYNC flag.
; 
; C-callable
;*****************************
.section .text.ClearVsyncFlag
ClearVsyncFlag:
	lds r18,sync_flags
	andi r18,~SYNC_FLAG_VSYNC
	sts sync_flags,r18
	ret


;************************************
; Read the current vsync counter.
; This value is incremented by the kernel
; on each vertical sync (60hz). Can be used
; for timeout functions.
;
; C-callable
; returns: (unsigned int) r25:r24
;************************************
.section .text.GetVsyncCounter
GetVsyncCounter:
	lds r24,vsync_counter
	lds r25,vsync_counter+1
	ret

;************************************
; Clear the vsync counter.
;
; C-callable
;************************************
.section .text.ClearVsyncCounter
ClearVsyncCounter:
	sts vsync_counter,r1
	sts vsync_counter+1,r1
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


;****************************
; Wait for the specified amount of clocks * 4
; Note this is approximative. 
; C callable
; r25:r24 - clocks to wait
;****************************
.global WaitClocks
.section .text.WaitClocks
WaitClocks:

1:	
	sbiw r24,1
	brne 1b
		
	ret

;****************************
; Turns on the onboard LED on PD4
; C callable
;****************************
.global SetLedOn
.section .text.SetLedOn
SetLedOn:
	sbi _SFR_IO_ADDR(PORTD),PD4
	ret

;****************************
; Turns off the onboard LED on PD4
; C callable
;****************************
.global SetLedOff
.section .text.SetLedOff
SetLedOff:
	cbi _SFR_IO_ADDR(PORTD),PD4
	ret

;****************************
; Toggles the onboard LED on PD4
; C callable
;****************************
.global ToggleLed
.section .text.ToggleLed
ToggleLed:
	sbi _SFR_IO_ADDR(PIND),PD4
	ret

//for internal debug use
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



#if TRUE_RANDOM_GEN == 1
	;****************************
	; Generates a true 16-bit random number
	; based upon the watchdog timer RC oscillator
	; entropy. Should not be called by user code because it
	; messes timer1. This function is invoke upon 
	; initialization by wdt_init(void)
	;****************************

	.global wdt_randomize
	.section .text.wdt_randomize
	wdt_randomize:

		;set timer 1 full speed count to 0xffff
		ldi r24,0
		sts sync_pulse,r24


		sts _SFR_MEM_ADDR(TIMSK1),r24
		sts _SFR_MEM_ADDR(OCR1AL),r24
		sts _SFR_MEM_ADDR(OCR1AH),r24
		sts _SFR_MEM_ADDR(OCR1BL),r24
		sts _SFR_MEM_ADDR(OCR1BH),r24
		sts _SFR_MEM_ADDR(TCNT1H),r24
		sts _SFR_MEM_ADDR(TCNT1L),r24
		
		sts _SFR_MEM_ADDR(TCCR1A),r24	
		ldi 24,(1<<CS10)
		sts _SFR_MEM_ADDR(TCCR1B),r24

		cli

		;enable watchdog at fastest speed and generate interrupts
		ldi r24,0
		sts _SFR_MEM_ADDR(MCUSR),r24	
		ldi r25,(1<<WDIE)+(1<<WDE)+(0<<WDP3)+(0<<WDP2)+(0<<WDP1)+(0<<WDP0)
		lds r24,_SFR_MEM_ADDR(WDTCSR)
		ori r24,(1<<WDCE)+(1<<WDE)
		sts _SFR_MEM_ADDR(WDTCSR),r24
		sts _SFR_MEM_ADDR(WDTCSR),r25
	
		sei

		;generate 8 random cycles
	wait:
		lds r24,sync_pulse ;using the yet unalocated "sync_pulse" as a temp variable
		cpi r24,8
		brlo wait

		ret

	;********************************
	; Returns the random seed generated
	; at startup
	; C-Callable
	; Returns: r24:r25(u16)
	;********************************

	.global GetTrueRandomSeed
	.section .text.GetTrueRandomSeed
	GetTrueRandomSeed:
		lds r24,random_value
		lds r25,random_value+1
		ret

	.global WDT_vect
	.section .text.WDT_vect
	;*************************************
	; Watchdog timer interrupt
	;*************************************
	WDT_vect:
		;save flags & status register
		push r16
		push r17

		in r16,_SFR_IO_ADDR(SREG)
		push r16

		lds r16,sync_pulse
		inc r16
		sts sync_pulse,r16

		;XOR succesive timer1 LSB into a int
		sbrc r16,0
		rjmp 1f
		lds r17,random_value
		lds r16,_SFR_MEM_ADDR(TCNT1L)
		eor r17,r16
		sts random_value,r17
		rjmp 2f
	1:
		lds r17,random_value+1
		lds r16,_SFR_MEM_ADDR(TCNT1L)
		eor r17,r16
		sts random_value+1,r17
	2:

		ldi r16,(1<<WDIE)+(1<<WDE)
		sts _SFR_MEM_ADDR(WDTCSR),r16

		;restore flags
		pop r16
		out _SFR_IO_ADDR(SREG),r16
	
		pop r17
		pop r16
		reti
#endif
