/*
 *  Uzebox Keyboard Firmware
 *  Copyright (C) 2015-2024 Alec Bourque
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
 *
 *  -----------------------------------------------------------------
 *
 *  Firmware to interface a PS/2 AT keyboard to the Uzebox
 *  via a SNES port. The interface requires a single 8-pin AtTiny85
 *  with no external components. The PLL clock source fuse should be
 *  set and the CKDIV8 unset in order to run full speed (16Mhz).
 *
 *  The firmware returns keyboard scan codes as-is. To start a
 *  new "transaction" to transfer one or more bytes, the host must send
 *  the begin signal by lowering the SNES clock line while raising the
 *  latch line and holding for at least 600ns. Then lower the latch and
 *  raise the clock lines and wait another 1uS before clocking the data.
 *  The host *must* send 0x01 (CMD_SEND_END) to receive its last byte
 *  and close the transaction.
 *
 *  Timer1 is used as a mechanism to recover lost sync with the keyboard.
 *  If after 4ms the 11 bits are not received, the pending character is discarded
 *  and state is restored to be ready to receive the next character.
 *
 *  Version 1.1: Support sending host commands to keyboard. Actually only
 *  used to support Perixx keyboard that requires the host to send any command right after
 *  a reset/BAT. Otherwise the keyboard renters BAT (self-test) endlessly sending 0xAA on the bus.
 *
 *
 *  Schematic: http://belogic.com/uzebox/downloads.htm
 *
 *  Targets: AtTiny45/85 
 *
 *  Fuses required: EXTENDED: 0xFF
 *                      HIGH: 0xDF
 *                       LOW: 0xE1
 *
 *  See:
 *  - http://web.archive.org/web/20170930054025/http://www.computer-engineering.org/ps2protocol/
 *  - http://web.archive.org/web/20171012093645if_/http://www.computer-engineering.org/ps2keyboard
 *  - http://www.philipstorr.id.au/pcbook/book3/keyboard.htm
 */

.INCLUDE "tn85def.inc"

.equ DEVICE_ID=0xCC
.equ FIRWARE_REV=0x11

.equ BUFFER_SIZE=32

.equ CMD_SEND_KEY=0x00
.equ CMD_SEND_END=0x01
.equ CMD_SEND_DEVICE_ID=0x02
.equ CMD_SEND_FIRMWARE_REV=0x03
.equ CMD_RESET=0x7f
.equ PIN_KB_CLK		=PB4
.equ PIN_KB_DATA	=PB3

.def rKbCmd			=r12	;Command byte to send to kb
.def rKbCmdParam	=r13	;Command's parameter byte (if applicable)
.def rKbIntLoopLo	=r14	;Kb interrupt loop counter lsb
.def rKbIntLoopHi	=r15	;Kb interrupt loop counter msb
.def rTemp			=r16	;temp register for main, do not use in interrupts.
.def rTxData		=r17	;data for next transfer to host
.def rHostCmd		=r18	;command byte received from the host
.def rIntTemp		=r19	;temp register for interrupts
.def rIntSregSave	=r20	;holds SREG in interrupts
.def rHostState		=r21	;Host state (0=waiting for host RTS, 1=tranfer mode)
.def rEdge 			=r22	;current keyboard clock edge (0=falling,1=rising)
.def rBitCount		=r23	;bits left to transfer from the keyboard
.def rData			=r24	;incoming data from the keyboard
.def rDeviceState	=r25	;Device (Keyboard) state (0=uninitialised,1=initialized)

;********************************************************************
; S R A M   D E F I N I T I O N S
;********************************************************************
.DSEG
.ORG  0X0060

buffer: .byte BUFFER_SIZE;*must* be at ORG 0x60


;********************************************************************
;   R E S E T   A N D   I N T   V E C T O R S
;********************************************************************

.CSEG
.ORG $0000
	rjmp Main			;0x0000 RESET 			Reset vector
	reti 				;0x0001 INT0			External Interrupt 0
	rjmp PCINT0_vect	;0x0002 PCINT0 			Change Interrupt Request 0
	rjmp TIMER1_COMPA_vect;0x0003 TIMER1_COMPA 	Compare Match A
	reti 				;0x0004 TIMER1_OVF 		Timer/Counter1 Overflow
	reti 				;0x0005 TIMER0_OVF 		Timer/Counter0 Overflow
	reti 				;0x0006 EE_RDY 			EEPROM Ready
	reti 				;0x0007 ANA_COMP 		Analog Comparator
	reti 				;0x0008 ADC ADC 		Conversion Complete
	reti 				;0x0009 TIMER1_COMPB 	Timer/Counter1 Compare Match B
	reti 				;0x000A TIMER0_COMPA 	Timer/Counter0 Compare Match A
	reti 				;0x000B TIMER0_COMPB 	Timer/Counter0 Compare Match B
	reti 				;0x000C WDT 			Watchdog Time-out
	reti 				;0x000D USI_START 		USI START
	rjmp USI_OVF_vect	;0x000E USI_OVF 		USI Overflow


;********************************************************************
;  M A I N    P R O G R A M
;********************************************************************
Main:
	;***Initialize***

    ;set stack
.ifdef SPH
	ldi rTemp, HIGH(RAMEND) 	;for AtTiny45/85
    out SPH,rTemp
.endif
    ldi rTemp, LOW(RAMEND)
    out SPL,rTemp

	ldi rTemp,(1<<PCIE)		;enable PIN change interrupts
	out GIMSK,rTemp

	ldi rTemp,(1<<PCINT4)		;turn on pin change int on PB4 (keyboard clock)
	out PCMSK,rTemp

	ldi rTemp,(1<<PCIF)		;clear any pending interrupts
	out GIFR,rTemp

	ldi rTemp,(1<<PB1)		;Set MISO as output, all other to input
	out DDRB,rTemp

	//ldi rTemp,(1<<PB3)+(1<<PB4)	;activate pull-ups on the keyboard clock and data lines
	ldi rTemp,(0<<PB3)+(0<<PB4)	; Set data bit to zero to set to hi-impedence the keyboard clock and data lines (pcb 1.1+)
	out PORTB,rTemp

	ldi rTemp,(1<<OCIE1A)		;activate int on compare match A
	out TIMSK,rTemp

	ldi rTemp,4
	out OCR1A,rTemp			;delay about 2ms

	out TCNT1,r0			;clear timer

	ldi rBitCount,11
	clr rEdge
	clr rData
	clr rHostState
	clr rDeviceState
	clr rTxData

	clr r0				;always zero in all code
	mov r1,r0
	inc r1				;always one in all code
	mov r2,r1
	inc r2				;always two in all code
	mov r3,r2
	inc r3				;always three in all code

	ldi XL,low(buffer)	;set tail pointer
	ldi XH,high(buffer)
	movw YL,XL			;set head pointer

	in rTemp,SREG
	ori rTemp,0x80
	out SREG,rTemp		;enable global interrupts


	;*************************
	; Main loop
	;*************************
loop:
	cp rHostState,r1
	breq loop

ready:
	;detect start condition
	in rTemp,PINB
	andi rTemp,0b00000101 //clk-MISO-MOSI
	cpi rTemp, 0b00000001
	brne loop

	;wait for end of start condition
loop2:
	in rTemp,PINB
	andi rTemp,0b00000101
	cpi rTemp, 0b00000100
	brne loop2

	mov rHostState,r1

	ldi rTemp, (1<<USIWM0)+(1<<USICS1)+(1<<USICS0)+(1<<USIOIE)
	out USICR,rTemp		;enable SPI 3-wire slave mode, negative/falling clock, tx complete interrupt

	ldi rTemp,(1<<USIOIF)
	out USISR,rTemp

	out USIDR,r0

	cp XL,YL		;data in the ring buffer?
	breq loop

	ld rTemp,X+		;read from tail
	andi XL,0x7f	;wrap pointer to 32
	ori XL,0x60
	out USIDR,rTemp
	rjmp loop


;********************************************************************
; Keyboard clock interrupt
; -------------------------------------------------------------------
; This interrupt is executed when the keyboard pulses its clock
; line to sends data.
;********************************************************************
PCINT0_vect:
	in rIntSregSave,SREG

	cpi rEdge,0
	brne rising_edge

	;falling edge
	ldi rIntTemp,(1<<CS13)+(1<<CS12)+(1<<CS11)+(1<<CS10) ;set timer 1 to clk/16384 (1ms)
	out TCCR1,rIntTemp

	cpi rBitCount,11
	brsh skip_bits		;skip start bit
	cpi rBitCount,3
	brlo skip_bits		;skip parity and stop bit

	lsr rData
	sbic PINB,3			;data bit set?
	ori rData,0x80
skip_bits:
	ldi rEdge,1			;set interrupt on rising edge
	rjmp pcint0_end

rising_edge:
	ldi rEdge,0			;set interrupt on falling edge
	dec rBitCount
	brne pcint0_end

	st Y+,rData			;all bits received, store in ring buffer
	andi YL,0x7f		;wrap pointer to 32
	ori YL,0x60
	ldi rBitCount,11

	out TCCR1,r0		;stop timer1
	out TCNT1,r0		;clear timer1
	out GTCCR,r2		;clear prescaler

	cpi rData, 0xaa		;is it a BAT (self-test) completion code?
	brne pcint0_end

	;wait for frame to complete
	ldi rIntTemp,255
pcint0_wait:
	nop
	nop
	nop
	dec rIntTemp
	brne pcint0_wait

	;send a command to avoid BAT again for certain keyboards like the Perixx
	ldi rIntTemp,0xee	;echo command
	mov rKbCmd,rIntTemp
	rcall send_command_to_keyboard

	mov rDeviceState,r1	;kb initialized

pcint0_end:
	out SREG,rIntSregSave
	reti

;********************************************************************
; Send  the specified command to the keyboard.
; Required a PCB with rev 1.1 or greater.
; -------------------------------------------------------------------
; rKbCmd		= command
; rKbCmdParam	= command parameter, if applicable
;
; See for protocol detail: http://web.archive.org/web/20170930054025/http://www.computer-engineering.org/ps2protocol/
;********************************************************************
send_command_to_keyboard:

	;Inhibit communication by pulling Clock low for at least 100 microseconds.
	sbi DDRB,PIN_KB_CLK			;pulls KB CLK line low

	;wait 100us
	ldi rIntTemp,LOW(1600/4)
	mov rKbIntLoopLo,rIntTemp
	ldi rIntTemp,HIGH(1600/4)
	mov rKbIntLoopHi,rIntTemp
send_cmd_loop:
	sub rKbIntLoopLo,r1			;decrement loop counter (r1 alway equals 1 in all code)
	sbc rKbIntLoopHi,r0			;(r0 alway equals 0 in all code)
	brne send_cmd_loop

	;Apply "Request-to-send" by pulling Data low, then release Clock.
	sbi DDRB,PIN_KB_DATA		;pulls KB DATA line low
	cbi DDRB,PIN_KB_CLK			;release clock line

	;account for slow slew rate of io pins
	ldi rIntTemp, 10 ; =32cy/3
send_cmd_slew_wait:
	dec rIntTemp
	brne send_cmd_slew_wait


	; Wait for the device to bring the Clock line low.
send_cmd_wait_clk_1:
	sbic PINB,PIN_KB_CLK
	rjmp send_cmd_wait_clk_1

	;send 8 bits of data
	ldi rIntTemp,8
	mov rKbIntLoopLo,rIntTemp

	;send all data bits
send_cmd_data_loop:

	;Set/reset the Data line to send the first data bit
	cbi DDRB,PIN_KB_DATA		;send 1 (release data line)
	sbrs rKbCmd,0
	sbi DDRB,PIN_KB_DATA		;send 0 (pull data line)

	; Wait for the device to bring the Clock line high.
send_cmd_wait_clk_2:
	sbis PINB,PIN_KB_CLK
	rjmp send_cmd_wait_clk_2

	; Wait for the device to bring the Clock line low.
send_cmd_wait_clk_3:
	sbic PINB,PIN_KB_CLK
	rjmp send_cmd_wait_clk_3

	lsr rKbCmd			;get next bit
	dec rKbIntLoopLo
	brne send_cmd_data_loop



	; Send parity bit *** fake even parity to test ***
	cbi DDRB,PIN_KB_DATA		;send 1 (clear data line)

	; Wait for the device to bring the Clock line high.
send_cmd_wait_clk_4:
	sbis PINB,PIN_KB_CLK
	rjmp send_cmd_wait_clk_4

	; Wait for the device to bring the Clock line low.
send_cmd_wait_clk_5:
	sbic PINB,PIN_KB_CLK
	rjmp send_cmd_wait_clk_5



	cbi DDRB,PIN_KB_DATA		;release data line

	; Wait for the device to bring the Data line low.
send_cmd_wait_clk_6:
	sbic PINB,PIN_KB_DATA
	rjmp send_cmd_wait_clk_6

	; Wait for the device to bring the Clock line low.
send_cmd_wait_clk_7:
	sbic PINB,PIN_KB_CLK
	rjmp send_cmd_wait_clk_7

	; Wait for the device to release the Data line.
send_cmd_wait_clk_8:
	sbis PINB,PIN_KB_DATA
	rjmp send_cmd_wait_clk_8

	; Wait for the device to release the Clock line.
send_cmd_wait_clk_9:
	sbis PINB,PIN_KB_CLK
	rjmp send_cmd_wait_clk_9

	ret


;********************************************************************
; SPI transfer complete interrupt
; -------------------------------------------------------------------
; This interrupt is executed when the host has completed a SPI
; transfer. Commands are read from the host and available scan codes
; data will be put on the SPI data register for the next transfer.
; SPI bus is turned off to avoid having the SNES polling
; pulsing the clock line sending out/loosing the data.
;********************************************************************
USI_OVF_vect:
	in rIntSregSave,SREG

	in rHostCmd,USIDR
	sbi USISR,USIOIF ;Clear the overflow flag

	clr rTxData

	cpi rHostCmd,CMD_SEND_KEY
	brne next_cmd

	cp XL,YL		;data in the ring buffer?
	breq usi_end

	ld rTxData,X+	;read from tail
	andi XL,0x7f	;wrap pointer to 32
	ori XL,0x60
	rjmp usi_end

next_cmd:
	cpi rHostCmd,CMD_SEND_END
	brne usi_end

	;end transmisison
	clr rTxData
	out USICR,r0	;turn off spi
	clr rHostState

usi_end:
	out USIDR,rTxData
	out SREG,rIntSregSave
	reti


;********************************************************************
; Keyboard read transfer reset
; -------------------------------------------------------------------
; This interrupt is executed if a full transfer (11 bits) of
; the keyboard is not received within 3ms.
;********************************************************************
TIMER1_COMPA_vect:

	mov rEdge,r0
	ldi rBitCount,11
	out TCCR1,r0		;stop timer1
	out TCNT1,r0		;clear timer1
	out GTCCR,r2		;clear prescaler

	reti



