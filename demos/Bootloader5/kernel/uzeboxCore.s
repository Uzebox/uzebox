/*
 *  Uzebox Kernel
 *  Copyright (C) 2008-2009 Alec Bourque
 *  Optimized and trimmed to the bootloader by Sandor Zsuga (Jubatian), 2017
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

#include <avr/io.h>
#include "defines.h"



.section .bss


;
; Bootloader request may be used to populate res_sd_struct, so it is placed
; in the protected RAM region above the stack (no init clearing). It is also
; placed at the very end of the RAM so when doing a bootloader request, always
; an upwards copy has to be performed. This is particularly important when the
; caller passes the structure from its stack which might overlap with this
; region.
;
.global res_sd_struct
.equ	res_sd_struct, 0x1100 - 23
;	.space 1               ; SD status flags
;	.space 2               ; Sector buffer pointer
;	.space 1               ; Cluster size
;	.space 2               ; Address of FAT
;	.space 4               ; Address of data
;	.space 4               ; Address of root directory
;	.space 4               ; First cluster of current file
;	.space 4               ; Current cluster of current file
;	.space 1               ; Current sector of current file



.section .text


;
; EEPROM Block 0 contents
;
eeprom_format_table:
	.byte (EEPROM_SIGNATURE & 0xFF)
	.byte (EEPROM_SIGNATURE >> 8)
	.byte EEPROM_HEADER_VER
	.byte EEPROM_BLOCK_SIZE
	.byte EEPROM_HEADER_SIZE
	.byte 1                ; 0x05: HardwareVersion
	.byte 0                ; 0x06: HardwareRevision
	.byte 0x38, 0x08       ; 0x07: Standard uzebox & fuzebox features
	.byte 0, 0             ; 0x09: Extended features
	.byte 0, 0, 0, 0, 0, 0 ; 0x0B: MAC
	.byte 0                ; 0x11: ColorCorrectionType
	.byte 0, 0, 0, 0       ; 0x12: Game CRC
	.byte 0                ; 0x16: Bootloader flags
	.byte 0                ; 0x17: Unused
	.byte 0                ; 0x18: Graphics mode flags
	.byte 0                ; 0x19: Last selected game in the bootloader
	.byte 0, 0, 0, 0, 0, 0 ; reserved
;
; New bootloader usage:
;
; Game CRC: No longer used, left empty
; Bootloader flags (same as old bootloader):
; 0x00: Start game selector, 0x01: Start game (if any)
; Unused:
; (Had plans with it which I later rather discarded)
; Graphics mode flags:
; bit 4: 262p+ enabled, if possible, 4 cycles should be inserted in VSync.
; bit 5: 525i enabled, if possible, true interlaced render should be used.
; Last selected game in the bootloader:
; This is used to remember this position after programming, making the game
; selector more user-friendly.
;



;
; I/O initialization table. Values are in pairs to support setting up ports
; guarded by 4 cycle timeouts. List end has to be at the start of a pair.
;
io_table:
	.byte _SFR_MEM_ADDR(MCUSR),  0x00  ; Clear reset cause to allow turning WD off
	.byte _SFR_MEM_ADDR(GPIOR0), 0x00  ; Clear GPIOR0 (for video)

	.byte _SFR_MEM_ADDR(WDTCSR), (1 << WDCE) | (1 << WDE)
	.byte _SFR_MEM_ADDR(WDTCSR), 0x00  ; Turn off Watchdog (prescaler at 16ms)

	.byte _SFR_MEM_ADDR(CLKPR),  (1 << CLKPCE)
	.byte _SFR_MEM_ADDR(CLKPR),  0x00  ; Make sure running with no prescaler

	.byte _SFR_MEM_ADDR(MCUCR),  (1 << IVCE)
	.byte _SFR_MEM_ADDR(MCUCR),  (1 << IVSEL) ; Select Bootloader for IT vectors

	.byte _SFR_MEM_ADDR(SPL),    ((RAM_RESERVED - 1) & 0xFF)
	.byte _SFR_MEM_ADDR(SPH),    ((RAM_RESERVED - 1) >> 8) ; Stack location

	.byte _SFR_MEM_ADDR(TCCR1B), 0x00  ; Stop timers
	.byte _SFR_MEM_ADDR(TCCR0B), 0x00

	.byte _SFR_MEM_ADDR(PRR),    (1 << PRTWI) | (1 << PRADC) ; Turn off TWI and ADC (not used in a Uzebox)
	.byte _SFR_MEM_ADDR(DDRC),   0xFF  ; Video DAC

	.byte _SFR_MEM_ADDR(DDRD),   (1 << PD7) | (1 << PD6) | (1 << PD4) | (1 << PD1) ; Audio-out, Chip Select, LED, UART TX
	.byte _SFR_MEM_ADDR(PORTD),  (1 << PD6) | (1 << PD4) | (1 << PD5) | (1 << PD3) | (1 << PD2) | (1 << PD0) ; Set CS high, LED on, pull-up for all inputs (PD3, PD2 are buttons)

	; Setup port A for joypads
	.byte _SFR_MEM_ADDR(DDRA),   0x0C  ; Set only control lines (CLK, Latch) as outputs
	.byte _SFR_MEM_ADDR(PORTA),  0xFB  ; Activate pullups on the data lines and unused pins

	; Set up video timing according to display lines (136 cycles LOW pulses)
	.byte _SFR_MEM_ADDR(TCNT1H), 0
	.byte _SFR_MEM_ADDR(TCNT1L), 0

	.byte _SFR_MEM_ADDR(OCR1AH), (1819 >> 8)
	.byte _SFR_MEM_ADDR(OCR1AL), (1819 & 0xFF)

	.byte _SFR_MEM_ADDR(TCCR1B), (1 << WGM12) + (1 << CS10) ; CTC mode, use OCR1A for match
	.byte _SFR_MEM_ADDR(TIMSK1), (1 << OCIE1A)              ; Generate interrupt on match

	; Set clock divider counter for AD725 on TIMER0
	; Outputs 14.31818Mhz (4FSC)
	.byte _SFR_MEM_ADDR(TCCR0A), (1 << COM0A0) + (1 << WGM01) ; toggle on compare match + CTC
	.byte _SFR_MEM_ADDR(OCR0A),  0     ; Divide main clock by 2

	.byte _SFR_MEM_ADDR(TCCR0B), (1 << CS00) ; Enable timer, no pre-scaler

	; Set sound PWM on TIMER2
	.byte _SFR_MEM_ADDR(TCCR2A), (1 << COM2A1) + (1 << WGM21) + (1 << WGM20) ; Fast PWM

	.byte _SFR_MEM_ADDR(OCR2A),  0     ; Duty cycle (amplitude)
	.byte _SFR_MEM_ADDR(TCCR2B), (1 << CS20) ; Enable timer, no pre-scaler

	.byte _SFR_MEM_ADDR(DDRB),   (1 << SYNC_PIN) | (1 << VIDEOCE_PIN) | (1 << PB3) | (1 << PB7) | (1 << PB5) ; 4FSC, SCK, MOSI
	.byte _SFR_MEM_ADDR(PORTB),  (1 << SYNC_PIN) | (1 << VIDEOCE_PIN) | (1 << PB6) | (1 << PB2) | (1 << PB1) ; Set sync & chip enable line to hi, MISO and unused pins pull-up

	; End of list
	.byte 0xFF, 0xFF



;
; Interrupt vector table & API call table
; (Things are a bit mixed up here, that the API of the bootloader has to be
; added within the kernel, but I have no other idea how else to make this
; interleaved layout).
;
.section .vectors

	rjmp  Initialize           ; Vector0: Boot
	.byte 0xB0, 0x07           ; API: 0xB007 signature
	.byte 0x10, 0xAD           ; API: 0x10AD signature (Vector1)
	.byte (VERSION >> 8), (VERSION & 0xFF) ; API: Version
	rjmp  SD_CRC7_Byte         ; API (Vector2)
	rjmp  SD_CRC16_Byte        ; API
	rjmp  SD_Wait_FF           ; API (Vector3)
	rjmp  SD_Command           ; API
	rjmp  SD_Release           ; API (Vector4)
	rjmp  SD_Init              ; API
	rjmp  SD_Read_Sector       ; API (Vector5)
	rjmp  SD_Read_Sector_Rt    ; API
	ret                        ; API (Vector6) (Reserved for SD_Write_Sector)
	ret                        ; API (Reserved for SD_Write_Sector_Rt)
	rjmp  FAT_Init             ; API (Vector7)
	rjmp  FAT_Get_Sector       ; API
	rjmp  FAT_Read_Sector      ; API (Vector8)
	ret                        ; API (Reserved for FAT_Write_Sector)
	rjmp  FAT_Next_Sector      ; API (Vector9)
	rjmp  FAT_Reset_Sector     ; API
	rjmp  FAT_Select_Root      ; API (Vector10)
	rjmp  FAT_Select_Cluster   ; API
	rjmp  FAT_Get_File_Cluster ; API (Vector11)
	ret                        ; API (Reserved for a directory listing function)
	rjmp  Bootld_Request       ; API (Vector12)
	rjmp  .-2                  ;
	rjmp  TIMER1_COMPA_vect    ; Vector13: Video



.section .text

;
; Kernel initialization
;
Initialize:

	; Clear r1 and SREG (interrupts disabled)

	clr   r1
	out   _SFR_IO_ADDR(SREG), r1

Initialize_entry:

	; Initialize I/O registers

	wdr
	ldi   ZL,      lo8(io_table)
	ldi   ZH,      hi8(io_table)
	ldi   XH,      0x00
	ldi   YH,      0x00
Initialize_ioloop:
	lpm   XL,      Z+
	lpm   r2,      Z+
	lpm   YL,      Z+
	lpm   r3,      Z+
	cpi   XL,      0xFF
	breq  Initialize_ioloop_end
	st    X,       r2
	st    Y,       r3
	rjmp  Initialize_ioloop
Initialize_ioloop_end:

	; Set video parameters. Start with no display (0 lines of the video
	; mode) to let EEPROM formatting finish faster when formatting is
	; necessary.

	ldi   ZL,      lo8(sync_pulse)
	ldi   ZH,      hi8(sync_pulse)
	ldi   XL,      6
	st    Z+,      XL      ; sync_pulse = 6 (Starts near a VSync)
	st    Z+,      r1
	dec   XL
	brne  .-6
	; sync_flags = 0
	; first_render_line      = 0
	; render_lines_count     = 0
	; first_render_line_tmp  = 0
	; render_lines_count_tmp = 0
	; sound_spos = 0

	; Let interrupts running so display is generated

	sei

	; Clear the RAM below stack

	ldi   r24,     0x00
	ldi   r25,     0x01    ; Begin: 0x0100
	ldi   r22,     0x00
	ldi   r23,     0x0F    ; Length: 0x0F00 (omit top 256 bytes)
	rcall ClearRAM

	; Format EEPROM if necessary

	rcall FormatEeprom

	; Enlarge actual display to correct size

	rcall SetRenderingParameters_Default

	; Jump to main ('T' flag still indicates bootloader request)

	rjmp  main



;
; Kernel init with parameters
;
; This can be used to request loading a new game by passing a properly
; populated SD structure pointing at the beginning of the game image (.uze
; file). It doesn't necessary have to be an actual file, it could be part of
; another file.
;
; Inputs:
; r25:r24: SD structure to use, positioned at the beginning of a .uze file
;
Bootld_Request:

	; Clear r1 and SREG (interrupts disabled)

	clr   r1
	out   _SFR_IO_ADDR(SREG), r1

	; Set T indicating bootloader request

	set

	; Copy parameters. This may be a move upwards if the two regions
	; overlap, so a decrementing copy is necessary.

	movw  XL,      r24
	adiw  XL,      23
	ldi   ZL,      lo8(res_sd_struct + 23)
	ldi   ZH,      hi8(res_sd_struct + 23)
	ld    r25,     -X
	st    -Z,      r25
	cpse  r24,     XL
	rjmp  .-8

	; Done, continue normal kernel init

	rjmp  Initialize_entry



;
; Soft reset
;
.global SoftReset
.section .text.SoftReset
SoftReset:

	; First check whether the watchdog is already running, if so, return.
	; This may happen if the soft reset is called from interrupt, which
	; happens if CONTROLLERS_VSYNC_READ is set nonzero.
	; Note that no "wdr" is used, it is unnecessary. If the watchdog
	; resets right when it was enabled, that's all right.

	ldi  ZL,       lo8(_SFR_MEM_ADDR(WDTCSR))
	ldi  ZH,       hi8(_SFR_MEM_ADDR(WDTCSR))
	ld   r24,      Z
	sbrc r24,      WDE     ; Watchdog already enabled?
	ret                    ; If so, return doing nothing (let it time out)
	ldi  r24,      (1 << WDCE) | (1 << WDE)
	ldi  r25,      (1 << WDE) ; Enable Watchdog, 16ms timeout
	cli
	st   Z,        r24
	st   Z,        r25
	sei
	rjmp .-2               ; Halt user program



;
; Format EEPROM including setting up head block; only do this if necessary
; (EEPROM is not already formatted).
;
.global FormatEeprom
.section .text.FormatEeprom
FormatEeprom:

	push  YL
	push  YH

	; Check whether EEPROM is formatted (2 byte signature is there)

	ldi   r24,     0x00     ; Pos. 0x0000 signature byte
	rcall ReadEeprom_Head
	cpi   r24,     (EEPROM_SIGNATURE & 0xFF)
	brne  FormatEeprom_doformat
	ldi   r24,     0x01     ; Pos. 0x0001 signature byte
	rcall ReadEeprom_Head
	cpi   r24,     (EEPROM_SIGNATURE >> 8)
	breq  FormatEeprom_end

FormatEeprom_doformat:

	; Write EEPROM header

	ldi   YL,      0
	ldi   YH,      0
FormatEeprom_headl:
	movw  ZL,      YL
	subi  ZL,      lo8(-(eeprom_format_table))
	sbci  ZH,      hi8(-(eeprom_format_table))
	movw  r24,     YL      ; r25:r24: Address
	lpm   r22,     Z       ; r22: Data
	rcall WriteEeprom
	inc   YL
	cpi   YL,      EEPROM_BLOCK_SIZE * EEPROM_HEADER_SIZE
	brne  FormatEeprom_headl

	; Write block occupation info (all are free blocks)

FormatEeprom_blockl:
	movw  r24,     YL
	ldi   r22,     (EEPROM_FREE_BLOCK & 0xFF)
	rcall WriteEeprom
	adiw  YL,      1
	movw  r24,     YL
	ldi   r22,     (EEPROM_FREE_BLOCK >> 8)
	rcall WriteEeprom
	adiw  YL,      (EEPROM_BLOCK_SIZE - 1)
	cpi   YH,      (2048 >> 8)
	brne  FormatEeprom_blockl

	; Done

FormatEeprom_end:

	pop   YH
	pop   YL
	ret
