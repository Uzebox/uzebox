/*
 	Created on: Nov 26, 2019
    Author: Uze

	Intel 8080 cpu emulator.
	-All official instructions and flags are implemented (except INT)
	-Passes the 8080EXM "Exerciser test". See: https://github.com/jscrane/emul8/blob/master/test8080.
	-Includes code for the floppy and RAM emulation
	-Uses the AVR flag register as the 8080 flag register for speed.
	-Instructions timing not implemented.
	-Undocumented instructions not implemented.

	Todo: Externalize RAM and floppy code.

	References:

	https://obsolescence.wixsite.com/obsolescence/cpm-internals
	http://www.classiccmp.org/cpmarchives/cpm/mirrors/www.geocities.com/SiliconValley/5711/architec.html
	http://cpuville.com/Code/CPM-on-a-new-computer.html
	https://www.pagetable.com/?p=457
	https://www.tramm.li/i8080/
	http://www.retroarchive.org/
	http://pcsauro.altervista.org/CPM.PHP
	http://www.classiccmp.org/cpmarchives/cpm/mirrors/www.retroarchive.org/cpm/lang/lang.htm --> many interesting sources
	http://www.gaby.de/cpm/manuals/archive/cpm22htm/ch6.htm#Section_6.6 --> CP/M Alteration Guide
	http://www.sydneysmith.com/wordpress/1651/cpm-disk-formats/ --> CP/M Disk Format
	http://www.idealine.info/sharpmz/succpminfo06.htm --> CP/M Disk format
	https://www.seasip.info/Cpm/format22.html --> directory structure
	https://www.autometer.de/unix4fun/z80pack/ --> Lots of tools and disk images for cp/m and 8080 cpu

 */

#include <avr/io.h>
#include <stdio.h>

;Public opeartions
.global cpu_init
.global cpu_exec
.global cpu_compare_states
.global cpu_registers
.global cpu_set_ram_size

/*
 Uncomment to activates single step debugging
*/
;#define DEBUG_EMU

#define DEASSERT_SPI sbi   SR_PORT, SR_PIN

#define  SR_PORT _SFR_IO_ADDR(PORTA)
#define  SR_PIN  PA4
#define  SR_DR   _SFR_IO_ADDR(SPDR)

;holds the value who resulted in changes in the parity bit for later evaluation
#define reg_parity		r2
#define reg_op_count	r3

#define reg_DE			r4
#define reg_E			r4
#define reg_D			r5

#define reg_SP			r6
#define reg_SP_lo		r6
#define reg_P			r6
#define reg_SP_hi		r7
#define reg_S			r7

#define reg_BC			r8
#define reg_C			r8
#define reg_B			r9

#define reg_value_0		r11
#define reg_value_1		r12
#define reg_value_2		r13

#define reg_HL			r14
#define reg_L 			r14
#define reg_H 			r15

#define reg_PSW			r16
#define reg_flags		r16
#define reg_A	 		r17

#define reg_temp		r18
#define reg_temp2		r19

#define reg_M			r24

#define reg_PC			YL
#define reg_PC_lo 		YL
#define reg_PC_hi		YH

#define RAM_SIZE_64K	0
#define RAM_SIZE_128K	1
;
; Global assembly delay macro for 0 to 1535 cycles
; Parameters: reg_temp = Register used in inner loop (will be destroyed)
;             clocks = CPU clocks to wait
;
.macro wait clocks
.if     (\clocks) >= 768
	ldi   reg_temp,    0
	dec   reg_temp
	brne  .-4
.endif
.if     ((\clocks) % 768) >= 9
	ldi   reg_temp,    ((\clocks) % 768) / 3
	dec   reg_temp
	brne  .-4
.if     ((\clocks) % 3) == 2
	rjmp  .
.elseif ((\clocks) % 3) == 1
	nop
.endif
.elseif ((\clocks) % 768) == 8
	lpm   reg_temp,    Z
	lpm   reg_temp,    Z
	rjmp  .
.elseif ((\clocks) % 768) == 7
	lpm   reg_temp,    Z
	rjmp  .
	rjmp  .
.elseif ((\clocks) % 768) == 6
	lpm   reg_temp,    Z
	lpm   reg_temp,    Z
.elseif ((\clocks) % 768) == 5
	lpm   reg_temp,    Z
	rjmp  .
.elseif ((\clocks) % 768) == 4
	rjmp  .
	rjmp  .
.elseif ((\clocks) % 768) == 3
	lpm   reg_temp,    Z
.elseif ((\clocks) % 768) == 2
	rjmp  .
.elseif ((\clocks) % 768) == 1
	nop
.endif
.endm

; Increments the specified register
.macro INR_ reg
	;modifies Z,S,P,AC
	out _SFR_IO_ADDR(SREG),reg_flags	;required to propagate the carry flag since this ins doesn't modify it
	inc \reg
	mov reg_parity,\reg
	in reg_flags,_SFR_IO_ADDR(SREG)
	sbr reg_flags,(1<<SREG_H)
	mov reg_temp,\reg
	andi reg_temp,0xf
	breq 1f
	cbr reg_flags,(1<<SREG_H)
1:
.endm

;Decrement the sepcified register
.macro DCR_ reg
	;modifies Z,S,P,AC
	out _SFR_IO_ADDR(SREG),reg_flags	;required to propagate the carry flag since dec/dcr doesn't modify it
	dec \reg
	mov reg_parity,\reg
	in reg_flags,_SFR_IO_ADDR(SREG)
	sbr reg_flags,(1<<SREG_H)
	mov reg_temp,\reg
	andi reg_temp,0xf
	cpi reg_temp,0xf
	brne 1f
	cbr reg_flags,(1<<SREG_H)
1:
.endm

;Add the specified register to the accumulator
.macro ADD_ reg
	;modifies Z,S,P,AC,C
	add reg_A,\reg
	mov reg_parity,reg_A
	in reg_flags,_SFR_IO_ADDR(SREG)
.endm

;Add with carry the specified register to the accumulator
.macro ADC_ reg
	;modifies Z,S,P,AC,C
	out _SFR_IO_ADDR(SREG),reg_flags
	adc reg_A,\reg
	mov reg_parity,reg_A
	in reg_flags,_SFR_IO_ADDR(SREG)
.endm

;Substract the specified register to the accumulator
.macro SUB_ reg
	;modifies Z,S,P,AC,C
	sub reg_A,\reg
	mov reg_parity,reg_A
	in reg_flags,_SFR_IO_ADDR(SREG)
	ldi reg_temp,(1<<SREG_H)
	eor reg_flags,reg_temp
.endm

;Sucbstract with carry the specified register from the accumulator
.macro SBB_ reg
	;modifies Z,S,P,AC,C
	out _SFR_IO_ADDR(SREG),reg_flags
	sbc reg_A,\reg
	tst reg_A							;zero flag not changed by sbc if result is zero so tst is used to set the zero flag
	mov reg_parity,reg_A
	in reg_flags,_SFR_IO_ADDR(SREG)
	ldi reg_temp,(1<<SREG_H)
	eor reg_flags,reg_temp
.endm

;Compaare the accumulator to the specified register
.macro CMP_ reg
	;modifies Z,S,P,AC,C
	cp reg_A,\reg
	in reg_flags,_SFR_IO_ADDR(SREG)
	ldi reg_temp,(1<<SREG_H)
	eor reg_flags,reg_temp
	mov reg_parity,reg_A
	sub reg_parity,\reg
.endm

;ANDs the specified register with the accumulator
.macro ANA_ reg
	;modifies Z,S,P,AC,C
	mov reg_temp,\reg
	or reg_temp,reg_A
	andi reg_temp,0x08
	breq 1f
	seh
1:
	and reg_A,\reg
	clc
	mov reg_parity,reg_A
	in reg_flags,_SFR_IO_ADDR(SREG)
.endm

;XORs the specified register with the accumulator
.macro XRA_ reg
	;modifies Z,S,P,AC,C
	eor reg_A,\reg
	clc
	clh
	mov reg_parity,reg_A
	in reg_flags,_SFR_IO_ADDR(SREG)
.endm

;ORs the specified register with the accumulator
.macro OR_ reg
	;modifies Z,S,P,AC,C
	or reg_A,\reg
	clc
	clh
	mov reg_parity,reg_A
	in reg_flags,_SFR_IO_ADDR(SREG)
.endm

.macro DAD_ regLSB regMSB
	add reg_L,\regLSB
	adc reg_H,\regMSB
	;Only carry can by modified
	in reg_temp,_SFR_IO_ADDR(SREG)
	bst reg_temp,SREG_C
	bld reg_flags,SREG_C
.endm

.section .bss
	.align 1

	;cpu emulation
	cpu_registers:
	pc:			.space 2
	rL:			.space 1
	rH: 		.space 1
	rC:			.space 1
	rB:			.space 1
	rE:			.space 1
	rD:			.space 1
	rSp_lo:		.space 1
	rSp_hi:		.space 1
	rFlags:		.space 1
	rA:			.space 1
	rParity:	.space 1
	intflag:	.space 1
	flags_8080: .space 1	;updated when exiting ecpu_xec
	breakpoint: .space 2

	;disk drive emulation
	;Note: dmabuf is defined by computer.c buffer
	cpu_fdcports:
	drive:		.space 1	;FDC drive
	track:		.space 1	;FDC track
	sector:		.space 1	;FDC sector
	dma:		.space 2	;FDC DMA address
	opcount:	.space 2	;amount of bytes read or written in the last SD access
	fdc_status: .space 1	;status of last FDC/SD card operation
	fdc_op:		.space 1	;FDC operation to execute, 0=read, 1=write
	fdc_retries:.space 1	;current count of retries if the SD card access fails
	fptr0:		.space 1	;used to store temporarely the file pointer
	fptr1:		.space 1
	fptr2:		.space 1
	sec_off:	.space 1	;offset of the 128 sector in the 512 bytes sector

	ram_size:	.space 1	;SPI RAM size detected 0=64K, 1=128K
	cache:		.space 256

	halt: 		.space 1
	debug:		.space 1
	counter:	.space 2	;Counter incremented each frame. Used for benchmarking and RTC emulation.
	counter_lat:.space 1	;latched hi byte is trigger when reading lo byte
	lastport:	.space 1
	lastop:		.space 1
	lastpc:	    .space 2


.section .text

.align 8

 ;256-byte array with a 1 wherever the number of set bits of the array index is EVEN
 ;must be aligned on an 256 byte boundary
parity_tbl:
       	.byte 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
        .byte 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
        .byte 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
        .byte 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
        .byte 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
        .byte 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
        .byte 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
        .byte 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
        .byte 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
        .byte 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
        .byte 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
        .byte 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
        .byte 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
        .byte 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
        .byte 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
        .byte 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1

jump_table:
	rjmp OPCODE_00 ; NOP       	- No operation
	rjmp OPCODE_01 ; LXI  BC,# 	- Load register pair immediate
	rjmp OPCODE_02 ; STAX BC   	- Store indirect through BC or DE
	rjmp OPCODE_03 ; INX  BC   	- Increment register pair
	rjmp OPCODE_04 ; INR  B    	- Increment register
	rjmp OPCODE_05 ; DCR  B    	- Decrement register
	rjmp OPCODE_06 ; MVI  B,#  	- Move immediate to register
	rjmp OPCODE_07 ; RLC       	- Rotate A left
	rjmp OPCODE_08 ; BREAK      - Invalid opcode (nop used for breakpoints in the emulator)
	rjmp OPCODE_09 ; DAD  BC   	- Add register pair to HL (16 bit add)
	rjmp OPCODE_0a ; LDAX BC   	- Load indirect through BC or DE
	rjmp OPCODE_0b ; DCX  BC   	- Decrement register pair
	rjmp OPCODE_0c ; INR  C    	- Increment register
	rjmp OPCODE_0d ; DCR  C    	- Decrement register
	rjmp OPCODE_0e ; MVI  C,#  	- Move immediate to register
	rjmp OPCODE_0f ; RRC       	- Rotate A right
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_11 ; LXI  DE,# 	- Load register pair immediate
	rjmp OPCODE_12 ; STAX DE   	- Store indirect through BC or DE
	rjmp OPCODE_13 ; INX  DE   	- Increment register pair
	rjmp OPCODE_14 ; INR  D    	- Increment register
	rjmp OPCODE_15 ; DCR  D    	- Decrement register
	rjmp OPCODE_16 ; MVI  D,#  	- Move immediate to register
	rjmp OPCODE_17 ; RAL       	- Rotate A left through carry
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_19 ; DAD  DE   	- Add register pair to HL (16 bit add)
	rjmp OPCODE_1a ; LDAX DE   	- Load indirect through BC or DE
	rjmp OPCODE_1b ; DCX  DE   	- Decrement register pair
	rjmp OPCODE_1c ; INR  E    	- Increment register
	rjmp OPCODE_1d ; DCR  E    	- Decrement register
	rjmp OPCODE_1e ; MVI  E,#  	- Move immediate to register
	rjmp OPCODE_1f ; RAR       	- Rotate A right through carry
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_21 ; LXI  HL,#	- Load register pair immediate
	rjmp OPCODE_22 ; SHLD      	- Store HL direct
	rjmp OPCODE_23 ; INX  HL   	- Increment register pair
	rjmp OPCODE_24 ; INR  H    	- Increment register
	rjmp OPCODE_25 ; DCR  H    	- Decrement register
	rjmp OPCODE_26 ; MVI  H,#  	- Move immediate to register
	rjmp OPCODE_27 ; DAA       	- Decimal Adjust accumulator
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_29 ; DAD  HL   	- Add register pair to HL (16 bit add)
	rjmp OPCODE_2a ; LHLD   	- Load HL Direct
	rjmp OPCODE_2b ; DCX  HL   	- Decrement register pair
	rjmp OPCODE_2c ; INR  L    	- Increment register
	rjmp OPCODE_2d ; DCR  L    	- Decrement register
	rjmp OPCODE_2e ; MVI  L,#  	- Move immediate to register
	rjmp OPCODE_2f ; CMA       	- Complement A
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_31 ; LXI  SP,# 	- Load register pair immediate
	rjmp OPCODE_32 ; STA  M    	- Store A to memory
	rjmp OPCODE_33 ; INX  SP   	- Increment register pair
	rjmp OPCODE_34 ; INR  M    	- Increment register
	rjmp OPCODE_35 ; DCR  M    	- Decrement register
	rjmp OPCODE_36 ; MVI  M,#  	- Move immediate to register
	rjmp OPCODE_37 ; STC       	- Set Carry flag
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_39 ; DAD  SP   	- Add register pair to HL (16 bit add)
	rjmp OPCODE_3a ; LDA  M    	- Load A from memory direct
	rjmp OPCODE_3b ; DCX  SP   	- Decrement register pair
	rjmp OPCODE_3c ; INR  A    	- Increment register
	rjmp OPCODE_3d ; DCR  A  	- Decrement register
	rjmp OPCODE_3e ; MVI  A,#	- Move immediate to register
	rjmp OPCODE_3f ; CMC       	- Complement Carry flag
	rjmp OPCODE_40 ; MOV  B,B 	- Move register to register
	rjmp OPCODE_41 ; MOV  B,C  	- Move register to register
	rjmp OPCODE_42 ; MOV  B,D  	- Move register to register
	rjmp OPCODE_43 ; MOV  B,E  	- Move register to register
	rjmp OPCODE_44 ; MOV  B,H  	- Move register to register
	rjmp OPCODE_45 ; MOV  B,L  	- Move register to register
	rjmp OPCODE_46 ; MOV  B,M  	- Move memory (HL) to register
	rjmp OPCODE_47 ; MOV  B,A  	- Move register to register
	rjmp OPCODE_48 ; MOV  C,B  	- Move register to register
	rjmp OPCODE_40 ; MOV  C,C  	- Move register to register
	rjmp OPCODE_4a ; MOV  C,D  	- Move register to register
	rjmp OPCODE_4b ; MOV  C,E  	- Move register to register
	rjmp OPCODE_4c ; MOV  C,H  	- Move register to register
	rjmp OPCODE_4d ; MOV  C,L  	- Move register to register
	rjmp OPCODE_4e ; MOV  C,M  	- Move memory (HL) to register
	rjmp OPCODE_4f ; MOV  C,A  	- Move register to register
	rjmp OPCODE_50 ; MOV  D,B  	- Move register to register
	rjmp OPCODE_51 ; MOV  D,C  	- Move register to register
	rjmp OPCODE_40 ; MOV  D,D  	- Move register to register
	rjmp OPCODE_53 ; MOV  D,E  	- Move register to register
	rjmp OPCODE_54 ; MOV  D,H  	- Move register to register
	rjmp OPCODE_55 ; MOV  D,L  	- Move register to register
	rjmp OPCODE_56 ; MOV  D,M  	- Move memory (HL) to register
	rjmp OPCODE_57 ; MOV  D,A  	- Move register to register
	rjmp OPCODE_58 ; MOV  E,B  	- Move register to register
	rjmp OPCODE_59 ; MOV  E,C  	- Move register to register
	rjmp OPCODE_5a ; MOV  E,D  	- Move register to register
	rjmp OPCODE_40 ; MOV  E,E  	- Move register to register
	rjmp OPCODE_5c ; MOV  E,H  	- Move register to register
	rjmp OPCODE_5d ; MOV  E,L  	- Move register to register
	rjmp OPCODE_5e ; MOV  E,M  	- Move memory (HL) to register
	rjmp OPCODE_5f ; MOV  E,A  	- Move register to register
	rjmp OPCODE_60 ; MOV  H,B  	- Move register to register
	rjmp OPCODE_61 ; MOV  H,C  	- Move register to register
	rjmp OPCODE_62 ; MOV  H,D  	- Move register to register
	rjmp OPCODE_63 ; MOV  H,E  	- Move register to register
	rjmp OPCODE_40 ; MOV  H,H  	- Move register to register
	rjmp OPCODE_65 ; MOV  H,L  	- Move register to register
	rjmp OPCODE_66 ; MOV  H,M  	- Move memory (HL) to register
	rjmp OPCODE_67 ; MOV  H,A  	- Move register to register
	rjmp OPCODE_68 ; MOV  L,B  	- Move register to register
	rjmp OPCODE_69 ; MOV  L,C  	- Move register to register
	rjmp OPCODE_6a ; MOV  L,D  	- Move register to register
	rjmp OPCODE_6b ; MOV  L,E  	- Move register to register
	rjmp OPCODE_6c ; MOV  L,H  	- Move register to register
	rjmp OPCODE_40 ; MOV  L,L  	- Move register to register
	rjmp OPCODE_6e ; MOV  L,M  	- Move memory (HL) to register
	rjmp OPCODE_6f ; MOV  L,A  	- Move register to register
	rjmp OPCODE_70 ; MOV  M,B  	- Move memory (HL) to register
	rjmp OPCODE_71 ; MOV  M,C  	- Move memory (HL) to register
	rjmp OPCODE_72 ; MOV  M,D  	- Move memory (HL) to register
	rjmp OPCODE_73 ; MOV  M,E  	- Move memory (HL) to register
	rjmp OPCODE_74 ; MOV  M,H  	- Move memory (HL) to register
	rjmp OPCODE_75 ; MOV  M,L  	- Move memory (HL) to register
	rjmp OPCODE_76 ; HLT       	- Halt processor
	rjmp OPCODE_77 ; MOV  M,A  	- Move memory (HL) to register
	rjmp OPCODE_78 ; MOV  A,B  	- Move register to register
	rjmp OPCODE_79 ; MOV  A,C  	- Move register to register
	rjmp OPCODE_7a ; MOV  A,D  	- Move register to register
	rjmp OPCODE_7b ; MOV  A,E  	- Move register to register
	rjmp OPCODE_7c ; MOV  A,H  	- Move register to register
	rjmp OPCODE_7d ; MOV  A,L  	- Move register to register
	rjmp OPCODE_7e ; MOV  A,M  	- Move memory (HL) to register
	rjmp OPCODE_40 ; MOV  A,A  	- Move register to register
	rjmp OPCODE_80 ; ADD  B    	- Add register to A
	rjmp OPCODE_81 ; ADD  C    	- Add register to A
	rjmp OPCODE_82 ; ADD  D    	- Add register to A
	rjmp OPCODE_83 ; ADD  E    	- Add register to A
	rjmp OPCODE_84 ; ADD  H    	- Add register to A
	rjmp OPCODE_85 ; ADD  L    	- Add register to A
	rjmp OPCODE_86 ; ADD  M    	- Add register to A
	rjmp OPCODE_87 ; ADD  A    	- Add register to A
	rjmp OPCODE_88 ; ADC  B    	- Add register to A with carry
	rjmp OPCODE_89 ; ADC  C    	- Add register to A with carry
	rjmp OPCODE_8a ; ADC  D    	- Add register to A with carry
	rjmp OPCODE_8b ; ADC  E    	- Add register to A with carry
	rjmp OPCODE_8c ; ADC  H    	- Add register to A with carry
	rjmp OPCODE_8d ; ADC  L    	- Add register to A with carry
	rjmp OPCODE_8e ; ADC  M    	- Add register to A with carry
	rjmp OPCODE_8f ; ADC  A    	- Add register to A with carry
	rjmp OPCODE_90 ; SUB  B    	- Subtract register from A
	rjmp OPCODE_91 ; SUB  C    	- Subtract register from A
	rjmp OPCODE_92 ; SUB  D    	- Subtract register from A
	rjmp OPCODE_93 ; SUB  E    	- Subtract register from A
	rjmp OPCODE_94 ; SUB  H    	- Subtract register from A
	rjmp OPCODE_95 ; SUB  L    	- Subtract register from A
	rjmp OPCODE_96 ; SUB  M    	- Subtract register from A
	rjmp OPCODE_97 ; SUB  A    	- Subtract register from A
	rjmp OPCODE_98 ; SBB  B    	- Subtract register from A with borrow
	rjmp OPCODE_99 ; SBB  C    	- Subtract register from A with borrow
	rjmp OPCODE_9a ; SBB  D    	- Subtract register from A with borrow
	rjmp OPCODE_9b ; SBB  E    	- Subtract register from A with borrow
	rjmp OPCODE_9c ; SBB  H    	- Subtract register from A with borrow
	rjmp OPCODE_9d ; SBB  L    	- Subtract register from A with borrow
	rjmp OPCODE_9e ; SBB  M    	- Subtract register from A with borrow
	rjmp OPCODE_9f ; SBB  A    	- Subtract register from A with borrow
	rjmp OPCODE_a0 ; ANA  B    	- AND register with A
	rjmp OPCODE_a1 ; ANA  C    	- AND register with A
	rjmp OPCODE_a2 ; ANA  D    	- AND register with A
	rjmp OPCODE_a3 ; ANA  E    	- AND register with A
	rjmp OPCODE_a4 ; ANA  H    	- AND register with A
	rjmp OPCODE_a5 ; ANA  L    	- AND register with A
	rjmp OPCODE_a6 ; ANA  M    	- AND register with A
	rjmp OPCODE_a7 ; ANA  A    	- AND register with A
	rjmp OPCODE_a8 ; XRA  B    	- ExclusiveOR register with A
	rjmp OPCODE_a9 ; XRA  C    	- ExclusiveOR register with A
	rjmp OPCODE_aa ; XRA  D    	- ExclusiveOR register with A
	rjmp OPCODE_ab ; XRA  E    	- ExclusiveOR register with A
	rjmp OPCODE_ac ; XRA  H    	- ExclusiveOR register with A
	rjmp OPCODE_ad ; XRA  L    	- ExclusiveOR register with A
	rjmp OPCODE_ae ; XRA  M    	- ExclusiveOR register with A
	rjmp OPCODE_af ; XRA  A    	- ExclusiveOR register with A
	rjmp OPCODE_b0 ; ORA  B    	- OR  register with A
	rjmp OPCODE_b1 ; ORA  C    	- OR  register with A
	rjmp OPCODE_b2 ; ORA  D    	- OR  register with A
	rjmp OPCODE_b3 ; ORA  E    	- OR  register with A
	rjmp OPCODE_b4 ; ORA  H    	- OR  register with A
	rjmp OPCODE_b5 ; ORA  L    	- OR  register with A
	rjmp OPCODE_b6 ; ORA  M    	- OR  register with A
	rjmp OPCODE_b7 ; ORA  A    	- OR  register with A
	rjmp OPCODE_b8 ; CMP  B    	- Compare register with A
	rjmp OPCODE_b9 ; CMP  C    	- Compare register with A
	rjmp OPCODE_ba ; CMP  D    	- Compare register with A
	rjmp OPCODE_bb ; CMP  E    	- Compare register with A
	rjmp OPCODE_bc ; CMP  H    	- Compare register with A
	rjmp OPCODE_bd ; CMP  L    	- Compare register with A
	rjmp OPCODE_be ; CMP  M    	- Compare register with A
	rjmp OPCODE_bf ; CMP  A    	- Compare register with A
	rjmp OPCODE_c0 ; RNZ      	- Conditional return from subroutine if Zero flag not set
	rjmp OPCODE_c1 ; POP  BC   	- Pop register pair from the stack
	rjmp OPCODE_c2 ; JNZ  a    	- Conditional jump if Zero flag not set
	rjmp OPCODE_c3 ; JMP  a    	- Unconditional jump
	rjmp OPCODE_c4 ; CNZ  a		- Conditional subroutine call if Zero flag not set
	rjmp OPCODE_c5 ; PUSH BC 	- Push register pair on the stack
	rjmp OPCODE_c6 ; ADI  #    	- Add immediate to A
	rjmp OPCODE_RST; RST  n 	- Restart (Call n*8)
	rjmp OPCODE_c8 ; RZ       	- Conditional return from subroutine if Zero flag set
	rjmp OPCODE_c9 ; RET       	- Unconditional return from subroutine
	rjmp OPCODE_ca ; JZ   a  	- Conditional jump if Zero flag set
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_cc ; CZ   a		- Conditional subroutine call if Zero flag set
	rjmp OPCODE_cd ; CALL a 	- Unconditional subroutine call
	rjmp OPCODE_ce ; ACI  #    	- Add immediate to A with carry
	rjmp OPCODE_RST; RST  n    	- Restart (Call n*8)
	rjmp OPCODE_d0 ; RNC      	- Conditional return from subroutine if Carry flag not set
	rjmp OPCODE_d1 ; POP  DE   	- Pop register pair from the stack
	rjmp OPCODE_d2 ; JNC  a 	- Conditional jump if Carry flag not set
	rjmp OPCODE_d3 ; OUT  p 	- Write A to output port
	rjmp OPCODE_d4 ; CNC  a    	- Conditional subroutine call if Carry flag not set
	rjmp OPCODE_d5 ; PUSH DE   	- Push register pair on the stack
	rjmp OPCODE_d6 ; SUI  #    	- Subtract immediate from A
	rjmp OPCODE_RST; RST  2    	- Restart (Call n*8)
	rjmp OPCODE_d8 ; RC       	- Conditional return from subroutine if Carry flag set
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_da ; JC   a    	- Conditional jump if Carry flag set
	rjmp OPCODE_db ; IN   p    	- Read input port into A
	rjmp OPCODE_dc ; CC   a    	- Conditional subroutine call if Carry flag set
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_de ; SBI  #    	- Subtract immediate from A with borrow
	rjmp OPCODE_RST; RST  n    	- Restart (Call n*8)
	rjmp OPCODE_e0 ; RPO      	- Conditional return from subroutine if Parity flag not set (ODD)
	rjmp OPCODE_e1 ; POP  HL   	- Pop register pair from the stack
	rjmp OPCODE_e2 ; JPO  a    	- Conditional jump if Parity flag not set (ODD)
	rjmp OPCODE_e3 ; XTHL      	- Swap H:L with top word on stack
	rjmp OPCODE_e4 ; CPO  a    	- Conditional subroutine call if Parity flag not set (ODD)
	rjmp OPCODE_e5 ; PUSH HL   	- Push register pair on the stack
	rjmp OPCODE_e6 ; ANI  #    	- AND immediate with A
	rjmp OPCODE_RST; RST  n    	- Restart (Call n*8)
	rjmp OPCODE_e8 ; RPE      	- Conditional return from subroutine if Parity flag set (EVEN)
	rjmp OPCODE_e9 ; PCHL      	- Jump to address in H:L
	rjmp OPCODE_ea ; JPE  a    	- Conditional jump if Parity flag set (EVEN)
	rjmp OPCODE_eb ; XCHG      	- Exchange DE and HL content
	rjmp OPCODE_ec ; CPE  a    	- Conditional subroutine call if Parity flag set (EVEN)
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_ee ; XRI  #    	- ExclusiveOR immediate with A
	rjmp OPCODE_RST; RST  n    	- Restart (Call n*8)
	rjmp OPCODE_f0 ; RP       	- Conditional return from subroutine if Sign flag not set (POSITIVE)
	rjmp OPCODE_f1 ; POP  PSW   - Pop register pair from the stack
	rjmp OPCODE_f2 ; JP   a    	- Conditional jump if Sign flag not set (POSITIVE)
	rjmp OPCODE_f3 ; DI        	- Disable interrupts
	rjmp OPCODE_f4 ; CP   a    	- Conditional subroutine call if Sign flag not set (POSITIVE)
	rjmp OPCODE_f5 ; PUSH PSW   - Push register pair on the stack
	rjmp OPCODE_f6 ; ORI  #    	- OR  immediate with A
	rjmp OPCODE_RST; RST  n    	- Restart (Call n*8)
	rjmp OPCODE_f8 ; RM      	- Conditional return from subroutine if Sign flag set (MINUS)
	rjmp OPCODE_f9 ; SPHL      	- Set SP to content of H:L
	rjmp OPCODE_fa ; JM   a    	- Conditional jump if Sign flag set (MINUS)
	rjmp OPCODE_fb ; EI        	- Enable interrupts
	rjmp OPCODE_fc ; CM   a    	- Conditional subroutine call if Sign flag set (MINUS)
	rjmp OPCODE_XX ; ???       	- Invalid opcode
	rjmp OPCODE_fe ; CPI  #    	- Compare immediate with A
	rjmp OPCODE_RST; RST  n    	- Restart (Call n*8)

;unimplemented:
OPCODE_XX:
	mov reg_temp,r24
	ldi r24,'U'
	call putchar
	ldi r24,'!'
	call putchar

;*** NOP       [No operation] ***
OPCODE_00:
	rjmp cpu_return

;*** LXI  BC,# [Load register pair immediate] *** ok
OPCODE_01:
	rcall read_pg_memory_16
	movw reg_BC,r24
	rjmp cpu_return

;*** STAX BC   [Store indirect through BC] *** ok
OPCODE_02:
	mov r20,reg_A
	movw r22,reg_BC
	rcall write_memory
	rjmp cpu_return

;*** INX  BC   [Increment register pair] *** ok
OPCODE_03:
	add reg_C,reg_value_1
	adc reg_B,reg_value_0
	rjmp cpu_return

;*** INR  B    [Increment register] *** ok
OPCODE_04:
	INR_ reg_B
	rjmp cpu_return


;*** DCR  B    [Decrement register] *** ok
OPCODE_05:
	DCR_ reg_B
	rjmp cpu_return

;*** MVI  B,#  [Move immediate to register] *** ok
OPCODE_06:
	rcall read_pg_memory
	mov reg_B,r24
	rjmp cpu_return

;*** RLC       [Rotate A left] ***
OPCODE_07:
	bst reg_A,7
	lsl reg_A
	bld reg_A,0
	;Only carry can by modified
	in reg_temp,_SFR_IO_ADDR(SREG)
	bst reg_temp,SREG_C
	bld reg_flags,SREG_C
	rjmp cpu_return

;*** BREAK [Illegal intruction - used for breakpoints in the emulator]
OPCODE_08:
	nop
	rjmp cpu_return

;*** DAD  BC   [Add register pair to HL (16 bit add)] *** ok
OPCODE_09:
	DAD_ reg_C reg_B
	rjmp cpu_return

;*** LDAX BC   [Load indirect through BC] ***
OPCODE_0a:
	movw r22,reg_BC
	rcall read_memory
	mov reg_A,r24
	rjmp cpu_return

;*** DCX  BC   [Decrement register pair] ***
OPCODE_0b:
	sub reg_C,reg_value_1	;no flags affected
	sbc reg_B,reg_value_0
	rjmp cpu_return

;*** INR  C    [Increment register] ***
OPCODE_0c:
	INR_ reg_C
	rjmp cpu_return


;*** DCR  C    [Decrement register] ***
OPCODE_0d:
	DCR_ reg_C
	rjmp cpu_return

;*** MVI  C,#  [Move immediate to register] ***
OPCODE_0e:
	rcall read_pg_memory
	mov reg_C,r24
	rjmp cpu_return

;*** RRC       [Rotate A right] ***
OPCODE_0f:
	bst reg_A,0
	lsr reg_A
	bld reg_A,7
	;Only carry can by modified
	in reg_temp,_SFR_IO_ADDR(SREG)
	bst reg_temp,SREG_C
	bld reg_flags,SREG_C

	rjmp cpu_return

;*** LXI  DE,# [Load register pair DE immediate] ***
OPCODE_11:
	rcall read_pg_memory_16
	movw reg_DE,r24
	rjmp cpu_return

;*** STAX DE   [Store A indirect through DE] ***
OPCODE_12:
	movw r22,reg_DE
	mov r20,reg_A
	rcall write_memory
	rjmp cpu_return

;*** INX  DE   [Increment register pair] ***
OPCODE_13:
	add reg_E,reg_value_1
	adc reg_D,reg_value_0
	rjmp cpu_return

;*** INR  D    [Increment register] ***
OPCODE_14:
	INR_ reg_D
	rjmp cpu_return

;*** DCR  D    [Decrement register] ***
OPCODE_15:
	DCR_ reg_D
	rjmp cpu_return

;*** MVI  D,#  [Move immediate to register] ***
OPCODE_16:
	rcall read_pg_memory
	mov reg_D,r24
	rjmp cpu_return

;*** RAL       [Rotate A left through carry] ***
OPCODE_17:
	out _SFR_IO_ADDR(SREG),reg_flags
	rol reg_A
	;Only carry can by modified
	in reg_temp,_SFR_IO_ADDR(SREG)
	bst reg_temp,SREG_C
	bld reg_flags,SREG_C
	rjmp cpu_return

;*** DAD  DE   [Add register pair to HL (16 bit add)] ***
OPCODE_19:
	DAD_ reg_E reg_D
	rjmp cpu_return

;*** LDAX DE   [Load register A indirect through DE] ***
OPCODE_1a:
	movw r22,reg_DE
	rcall read_memory
	mov reg_A,r24
	rjmp cpu_return

;*** DCX  DE   [Decrement register pair] ***
OPCODE_1b:
	sub reg_E,reg_value_1
	sbc reg_D,reg_value_0
	rjmp cpu_return

;*** INR  E    [Increment register] ***
OPCODE_1c:
	INR_ reg_E
	rjmp cpu_return

;*** DCR  E    [Decrement register] ***
OPCODE_1d:
	DCR_ reg_E
	rjmp cpu_return

;*** MVI  E,#  [Move immediate to register E] ***
OPCODE_1e:
	rcall read_pg_memory
	mov reg_E,r24
	rjmp cpu_return

;*** RAR       [Rotate A right through carry] ***
OPCODE_1f:
	out _SFR_IO_ADDR(SREG),reg_flags
	ror reg_A
	;Only carry can by modified
	in reg_temp,_SFR_IO_ADDR(SREG)
	bst reg_temp,SREG_C
	bld reg_flags,SREG_C
	rjmp cpu_return

;*** LXI  HL,# [Load register pair HL immediate] ***
OPCODE_21:
	rcall read_pg_memory_16
	movw reg_HL,r24
	rjmp cpu_return

;*** SHLD [Store HL direct]  ***
OPCODE_22:
	rcall read_pg_memory_16		;read dest adress
	movw r22,r24
	movw r20,reg_HL
	rcall write_memory_16
	rjmp cpu_return

;*** INX  HL   [Increment register pair] ***
OPCODE_23:
	add reg_L,reg_value_1
	adc reg_H,reg_value_0
	rjmp cpu_return

;*** INR  H    [Increment register] ***
OPCODE_24:
	INR_ reg_H
	rjmp cpu_return

;*** DCR  H    [Decrement register] ***
OPCODE_25:
	DCR_ reg_H
	rjmp cpu_return

;*** MVI  H,#  [Move immediate to register] ***
OPCODE_26:
	rcall read_pg_memory
	mov reg_H,r24
	rjmp cpu_return

;*** DAA       [Decimal Adjust accumulator] ***
OPCODE_27:
	clr r30					;adder
	clr r31					;carry
	mov reg_temp,reg_A
	andi reg_temp,0xf		;lsb of A
	mov reg_temp2,reg_A
	swap reg_temp2
	andi reg_temp2,0xf		;msb of A

    ;if (c->hf || lsb > 9) correction += 0x06;
	cpi reg_temp,10
	brsh corr_lsb
	sbrs reg_flags,SREG_H
	rjmp check_msb
corr_lsb:
	ldi r30,0x06
check_msb:
	; ((A >> 4) >= 9 && (A & 0x0f) > 9))
	cpi reg_temp2,9
	brlo 1f
	cpi reg_temp,10
	brlo 1f
	rjmp corr_msb
1:
	cpi reg_temp2,10
	brsh corr_msb
	sbrs reg_flags,SREG_C
	rjmp ajust
corr_msb:
	ori r30,0x60
	ldi r31,(1<<SREG_C)
ajust:
	ADD_ r30
	or reg_flags,r31	;ajust carry flag
	rjmp cpu_return

;*** DAD  HL   [Add register pair to HL (16 bit add)] ***
OPCODE_29:
	DAD_ reg_L reg_H
	rjmp cpu_return

;*** LHLD    [Load H And L Direct] ***
OPCODE_2a:
	rcall read_pg_memory_16		;load next two operands bytes
	movw r22,r24
	rcall read_memory_16		;read pair at adress
	movw reg_HL,r24
	rjmp cpu_return

;*** DCX  HL   [Decrement register pair] ***
OPCODE_2b:
	sub reg_L,reg_value_1
	sbc reg_H,reg_value_0
	rjmp cpu_return

;*** INR  L    [Increment register] ***
OPCODE_2c:
	INR_ reg_L
	rjmp cpu_return

;*** DCR  L    [Decrement register] ***
OPCODE_2d:
	DCR_ reg_L
	rjmp cpu_return

;*** MVI  L,#  [Move immediate to register] ***
OPCODE_2e:
	rcall read_pg_memory
	mov reg_L,r24
	rjmp cpu_return

;*** CMA       [Complement A] ***
OPCODE_2f:
	com reg_A
	rjmp cpu_return

;*** LXI  SP,# [Load register pair immediate] ***
OPCODE_31:
	rcall read_pg_memory_16	;read 2 bytes in r24:r25
	movw reg_SP,r24
	rjmp cpu_return

;*** STA M   [Store A to memory] ***
OPCODE_32:
	rcall read_pg_memory_16	;load destination adress (immediate)
	movw r22,r24			;dest address
	mov r20,reg_A			;value to write
	rcall write_memory		;write A to memory
	rjmp cpu_return

;*** INX  SP   [Increment register pair] ***
OPCODE_33:
	add reg_P,reg_value_1
	adc reg_S,reg_value_0
	rjmp cpu_return

;*** INR  M    [Increment Memory] ***
OPCODE_34:
	movw r22,reg_HL
	rcall read_memory_no_restart	;returns value in reg_M (r24)
	INR_ reg_M
	mov r20,reg_M
	movw r22,reg_HL
	rcall write_memory
	rjmp cpu_return

;*** DCR  M    [Decrement Memory] ***
OPCODE_35:
	movw r22,reg_HL
	rcall read_memory_no_restart	;returns value in reg_M (r24)
	DCR_ reg_M
	mov r20,reg_M
	movw r22,reg_HL
	rcall write_memory
	rjmp cpu_return

;*** MVI  M,#  [Move immediate to register] ***
OPCODE_36:
	rcall read_pg_memory
	mov r20,r24
	movw r22,reg_HL
	rcall write_memory
	rjmp cpu_return

;*** STC       [Set Carry flag] ***
OPCODE_37:
	ori reg_flags,(1<<SREG_C)
	rjmp cpu_return

;*** DAD  SP   [Add register pair to HL (16 bit add)] ***
OPCODE_39:
	DAD_ reg_P reg_S
	rjmp cpu_return

;*** LDA M   [Load A from Memory Direct] ***
OPCODE_3a:
	rcall read_pg_memory_16		;load address direct
	movw r22,r24
	rcall read_memory			;load value at adress
	mov reg_A,r24
	rjmp cpu_return

;*** DCX  SP   [Decrement register pair] ***
OPCODE_3b:
	sub reg_P,reg_value_1
	sbc reg_S,reg_value_0
	rjmp cpu_return

;*** INR  A    [Increment register] ***
OPCODE_3c:
	INR_ reg_A
	rjmp cpu_return

;*** DCR  A    [Decrement register] ***
OPCODE_3d:
	DCR_ reg_A
	rjmp cpu_return

;*** MVI  A,#  [Move immediate to register] ***
OPCODE_3e:
	rcall read_pg_memory	;load data byte
	mov reg_A,r24
	rjmp cpu_return

;*** CMC       [Complement Carry flag] ***
OPCODE_3f:
	ldi r25,(1<<SREG_C)
	eor reg_flags,r25
	rjmp cpu_return

;*** MOV   B,B  [Move register to register (NOP)] ***
OPCODE_40:
	rjmp cpu_return

;*** MOV   B,C  [Move register to register] ***
OPCODE_41:
	mov reg_B,reg_C
	rjmp cpu_return

;*** MOV   B,D  [Move register to register] ***
OPCODE_42:
	mov reg_B,reg_D
	rjmp cpu_return

;*** MOV   B,E  [Move register to register] ***
OPCODE_43:
	mov reg_B,reg_E
	rjmp cpu_return

;*** MOV   B,H  [Move register to register] ***
OPCODE_44:
	mov reg_B,reg_H
	rjmp cpu_return

;*** MOV   B,L  [Move register to register] ***
OPCODE_45:
	mov reg_B,reg_L
	rjmp cpu_return

;*** MOV   B,M  [Move memory (HL) to register] ***
OPCODE_46:
	movw r22,reg_HL
	rcall read_memory
	mov reg_B,r24
	rjmp cpu_return

;*** MOV   B,A  [Move register to register] ***
OPCODE_47:
	mov reg_B,reg_A
	rjmp cpu_return

;*** MOV   C,B  [Move register to register] ***
OPCODE_48:
	mov reg_C,reg_B
	rjmp cpu_return

;*** MOV   C,D  [Move register to register] ***
OPCODE_4a:
	mov reg_C,reg_D
	rjmp cpu_return

;*** MOV   C,E  [Move register to register] ***
OPCODE_4b:
	mov reg_C,reg_E
	rjmp cpu_return

;*** MOV   C,H  [Move register to register] ***
OPCODE_4c:
	mov reg_C,reg_H
	rjmp cpu_return

;*** MOV   C,L  [Move register to register] ***
OPCODE_4d:
	mov reg_C,reg_L
	rjmp cpu_return

;*** MOV   C,M  [Move memory (HL) to register] ***
OPCODE_4e:
	movw r22,reg_HL
	rcall read_memory
	mov reg_C,r24
	rjmp cpu_return

;*** MOV   C,A  [Move register to register] ***
OPCODE_4f:
	mov reg_C,reg_A
	rjmp cpu_return

;*** MOV   D,B  [Move register to register] ***
OPCODE_50:
	mov reg_D,reg_B
	rjmp cpu_return

;*** MOV   D,C  [Move register to register] ***
OPCODE_51:
	mov reg_D,reg_C
	rjmp cpu_return

;*** MOV   D,E  [Move register to register] ***
OPCODE_53:
	mov reg_D,reg_E
	rjmp cpu_return

;*** MOV   D,H  [Move register to register] ***
OPCODE_54:
	mov reg_D,reg_H
	rjmp cpu_return

;*** MOV   D,L  [Move register to register] ***
OPCODE_55:
	mov reg_D,reg_L
	rjmp cpu_return

;*** MOV   D,M  [Move memory (HL) to register] ***
OPCODE_56:
	movw r22,reg_HL
	rcall read_memory
	mov reg_D,r24
	rjmp cpu_return

;*** MOV   D,A  [Move register to register] ***
OPCODE_57:
	mov reg_D,reg_A
	rjmp cpu_return

;*** MOV   E,B  [Move register to register] ***
OPCODE_58:
	mov reg_E,reg_B
	rjmp cpu_return

;*** MOV   E,C  [Move register to register] ***
OPCODE_59:
	mov reg_E,reg_C
	rjmp cpu_return

;*** MOV   E,D  [Move register to register] ***
OPCODE_5a:
	mov reg_E,reg_D
	rjmp cpu_return

;*** MOV   E,H  [Move register to register] ***
OPCODE_5c:
	mov reg_E,reg_H
	rjmp cpu_return

;*** MOV   E,L  [Move register to register] ***
OPCODE_5d:
	mov reg_E,reg_L
	rjmp cpu_return

;*** MOV   E,M  [Move memory (HL) to register] ***
OPCODE_5e:
	movw r22,reg_HL
	rcall read_memory
	mov reg_E,r24
	rjmp cpu_return

;*** MOV   E,A  [Move register to register] ***
OPCODE_5f:
	mov reg_E,reg_A
	rjmp cpu_return

;*** MOV   H,B  [Move register to register] ***
OPCODE_60:
	mov reg_H,reg_B
	rjmp cpu_return

;*** MOV   H,C  [Move register to register] ***
OPCODE_61:
	mov reg_H,reg_C
	rjmp cpu_return

;*** MOV   H,D  [Move register to register] ***
OPCODE_62:
	mov reg_H,reg_D
	rjmp cpu_return

;*** MOV   H,E  [Move register to register] ***
OPCODE_63:
	mov reg_H,reg_E
	rjmp cpu_return

;*** MOV   H,L  [Move register to register] ***
OPCODE_65:
	mov reg_H,reg_L
	rjmp cpu_return

;*** MOV   H,M  [Move memory (HL) to register] ***
OPCODE_66:
	movw r22,reg_HL
	rcall read_memory
	mov reg_H,r24
	rjmp cpu_return

;*** MOV   H,A  [Move register to register] ***
OPCODE_67:
	mov reg_H,reg_A
	rjmp cpu_return

;*** MOV   L,B  [Move register to register] ***
OPCODE_68:
	mov reg_L,reg_B
	rjmp cpu_return

;*** MOV   L,C  [Move register to register] ***
OPCODE_69:
	mov reg_L,reg_C
	rjmp cpu_return

;*** MOV   L,D  [Move register to register] ***
OPCODE_6a:
	mov reg_L,reg_D
	rjmp cpu_return

;*** MOV   L,E  [Move register to register] ***
OPCODE_6b:
	mov reg_L,reg_E
	rjmp cpu_return

;*** MOV   L,H  [Move register to register] ***
OPCODE_6c:
	mov reg_L,reg_H
	rjmp cpu_return

;*** MOV   L,M  [Move memory (HL) to register] ***
OPCODE_6e:
	movw r22,reg_HL
	rcall read_memory
	mov reg_L,r24
	rjmp cpu_return

;*** MOV   L,A  [Move register to register] ***
OPCODE_6f:
	mov reg_L,reg_A
	rjmp cpu_return

;*** MOV   M,B  [Move memory (HL) to register] ***
OPCODE_70:
	movw r22,reg_HL
	mov r20,reg_B
	rcall write_memory
	rjmp cpu_return

;*** MOV   M,C  [Move C to memory (HL) ] ***
OPCODE_71:
	movw r22,reg_HL
	mov r20,reg_C
	rcall write_memory
	rjmp cpu_return

;*** MOV   M,D  [Move memory (HL) to register] ***
OPCODE_72:
	movw r22,reg_HL
	mov r20,reg_D
	rcall write_memory
	rjmp cpu_return

;*** MOV   M,E  [Move memory (HL) to register] ***
OPCODE_73:
	movw r22,reg_HL
	mov r20,reg_E
	rcall write_memory
	rjmp cpu_return

;*** MOV   M,H  [Move memory (HL) to register] ***
OPCODE_74:
	movw r22,reg_HL
	mov r20,reg_H
	rcall write_memory
	rjmp cpu_return

;*** MOV   M,L  [Move memory (HL) to register] ***
OPCODE_75:
	movw r22,reg_HL
	mov r20,reg_L
	rcall write_memory
	rjmp cpu_return

;*** HALT  [Halt or break CPU] ***
OPCODE_76:
	rjmp cpu_exit

;*** MOV   M,A  [Move memory (HL) to register] ***
OPCODE_77:
	movw r22,reg_HL
	mov r20,reg_A
	rcall write_memory
	rjmp cpu_return

;*** MOV   A,B  [Move register to register] ***
OPCODE_78:
	mov reg_A,reg_B
	rjmp cpu_return

;*** MOV   A,C  [Move register to register] ***
OPCODE_79:
	mov reg_A,reg_C
	rjmp cpu_return

;*** MOV   A,D  [Move register to register] ***
OPCODE_7a:
	mov reg_A,reg_D
	rjmp cpu_return

;*** MOV   A,E  [Move register to register] ***
OPCODE_7b:
	mov reg_A,reg_E
	rjmp cpu_return

;*** MOV   A,H  [Move register to register] ***
OPCODE_7c:
	mov reg_A,reg_H
	rjmp cpu_return

;*** MOV   A,L  [Move register to register] ***
OPCODE_7d:
	mov reg_A,reg_L
	rjmp cpu_return

;*** MOV   A,M  [Move memory (HL) to register] ***
OPCODE_7e:
	movw r22,reg_HL
	rcall read_memory	;load data
	mov reg_A,r24
	rjmp cpu_return

;*** ADD  B    [Add register to A] ***
OPCODE_80:
	ADD_ reg_B
	rjmp cpu_return

;*** ADD  C    [Add register to A] ***
OPCODE_81:
	ADD_ reg_C
	rjmp cpu_return

;*** ADD  D    [Add register to A] ***
OPCODE_82:
	ADD_ reg_D
	rjmp cpu_return

;*** ADD  E    [Add register to A] ***
OPCODE_83:
	ADD_ reg_E
	rjmp cpu_return

;*** ADD  H    [Add register to A] ***
OPCODE_84:
	ADD_ reg_H
	rjmp cpu_return

;*** ADD  L    [Add register to A] ***
OPCODE_85:
	ADD_ reg_L
	rjmp cpu_return

;*** ADD  M    [Add memory to A] ***
OPCODE_86:
	movw r22,reg_HL
	rcall read_memory
	ADD_ reg_M
	rjmp cpu_return

;*** ADD  A    [Add register to A] ***
OPCODE_87:
	ADD_ reg_A
	rjmp cpu_return

;*** ADC  B    [Add register to A with carry] ***
OPCODE_88:
	ADC_ reg_B
	rjmp cpu_return

;*** ADC  C    [Add register to A with carry] ***
OPCODE_89:
	ADC_ reg_C
	rjmp cpu_return

;*** ADC  D    [Add register to A with carry] ***
OPCODE_8a:
	ADC_ reg_D
	rjmp cpu_return

;*** ADC  E    [Add register to A with carry] ***
OPCODE_8b:
	ADC_ reg_E
	rjmp cpu_return

;*** ADC  H    [Add register to A with carry] ***
OPCODE_8c:
	ADC_ reg_H
	rjmp cpu_return

;*** ADC  L    [Add register to A with carry] ***
OPCODE_8d:
	ADC_ reg_L
	rjmp cpu_return

;*** ADC  M    [Add memory to A with carry] ***
OPCODE_8e:
	movw r22,reg_HL
	rcall read_memory
	ADC_ reg_M
	rjmp cpu_return

;*** ADC  A    [Add register to A with carry] ***
OPCODE_8f:
	ADC_ reg_A
	rjmp cpu_return

;*** SUB  B    [Subtract register from A] ***
OPCODE_90:
	SUB_ reg_B
	rjmp cpu_return

;*** SUB  C    [Subtract register from A] ***
OPCODE_91:
	SUB_ reg_C
	rjmp cpu_return

;*** SUB  D    [Subtract register from A] ***
OPCODE_92:
	SUB_ reg_D
	rjmp cpu_return

;*** SUB  E    [Subtract register from A] ***
OPCODE_93:
	SUB_ reg_E
	rjmp cpu_return

;*** SUB  H    [Subtract register from A] ***
OPCODE_94:
	SUB_ reg_H
	rjmp cpu_return

;*** SUB  L    [Subtract register from A] ***
OPCODE_95:
	SUB_ reg_L
	rjmp cpu_return

;*** SUB  M    [Subtract register from A] ***
OPCODE_96:
	movw r22,reg_HL
	rcall read_memory
	SUB_ reg_M
	rjmp cpu_return

;*** SUB  A    [Subtract register from A] ***
OPCODE_97:
	SUB_ reg_A
	rjmp cpu_return

;*** SBB  B    [Subtract register from A with borrow] ***
OPCODE_98:
	SBB_ reg_B
	rjmp cpu_return

;*** SBB  C    [Subtract register from A with borrow] ***
OPCODE_99:
	SBB_ reg_C
	rjmp cpu_return

;*** SBB  D    [Subtract register from A with borrow] *** !!!
OPCODE_9a:
	SBB_ reg_D
	rjmp cpu_return

;*** SBB  E    [Subtract register from A with borrow] ***
OPCODE_9b:
	SBB_ reg_E
	rjmp cpu_return

;*** SBB  H    [Subtract register from A with borrow] ***
OPCODE_9c:
	SBB_ reg_H
	rjmp cpu_return

;*** SBB  L    [Subtract register from A with borrow] ***
OPCODE_9d:
	SBB_ reg_L
	rjmp cpu_return

;*** SBB  M    [Subtract memory from A with borrow] ***
OPCODE_9e:
	movw r22,reg_HL
	rcall read_memory
	SBB_ reg_M
	rjmp cpu_return

;*** SBB  A    [Subtract register from A with borrow] ***
OPCODE_9f:
	SBB_ reg_A
	rjmp cpu_return

;*** ANA  B    [AND register with A] ***
OPCODE_a0:
	ANA_ reg_B
	rjmp cpu_return

;*** ANA  C    [AND register with A] ***
OPCODE_a1:
	ANA_ reg_C
	rjmp cpu_return

;*** ANA  D    [AND register with A] ***
OPCODE_a2:
	ANA_ reg_D
	rjmp cpu_return

;*** ANA  E    [AND register with A] ***
OPCODE_a3:
	ANA_ reg_E
	rjmp cpu_return

;*** ANA  H    [AND register with A] ***
OPCODE_a4:
	ANA_ reg_H
	rjmp cpu_return

;*** ANA  L    [AND register with A] ***
OPCODE_a5:
	ANA_ reg_L
	rjmp cpu_return

;*** ANA  M    [AND register with A] ***
OPCODE_a6:
	movw r22,reg_HL
	rcall read_memory
	ANA_ reg_M
	rjmp cpu_return

;*** ANA  A    [AND register with A] ***
OPCODE_a7:
	ANA_ reg_A
	rjmp cpu_return

;*** XRA  B    [ExclusiveOR register with A] ***
OPCODE_a8:
	XRA_ reg_B
	rjmp cpu_return

;*** XRA  C    [ExclusiveOR register with A] ***
OPCODE_a9:
	XRA_ reg_C
	rjmp cpu_return

;*** XRA  D    [ExclusiveOR register with A] ***
OPCODE_aa:
	XRA_ reg_D
	rjmp cpu_return

;*** XRA  E    [ExclusiveOR register with A] ***
OPCODE_ab:
	XRA_ reg_E
	rjmp cpu_return

;*** XRA  H    [ExclusiveOR register with A] ***
OPCODE_ac:
	XRA_ reg_H
	rjmp cpu_return

;*** XRA  L    [ExclusiveOR register with A] ***
OPCODE_ad:
	XRA_ reg_L
	rjmp cpu_return

;*** XRA  M    [ExclusiveOR register with A] ***
OPCODE_ae:
	movw r22,reg_HL
	rcall read_memory
	XRA_ reg_M
	rjmp cpu_return

;*** XRA  A    [ExclusiveOR register with A] ***
OPCODE_af:
	XRA_ reg_A
	rjmp cpu_return

;*** ORA  B    [OR  register with A] ***
OPCODE_b0:
	OR_ reg_B
	rjmp cpu_return

;*** ORA  C    [OR  register with A] ***
OPCODE_b1:
	OR_ reg_C
	rjmp cpu_return

;*** ORA  D    [OR  register with A] ***
OPCODE_b2:
	OR_ reg_D
	rjmp cpu_return

;*** ORA  E    [OR  register with A] ***
OPCODE_b3:
	OR_ reg_E
	rjmp cpu_return

;*** ORA  H    [OR  register with A] ***
OPCODE_b4:
	OR_ reg_H
	rjmp cpu_return

;*** ORA  L    [OR  register with A] ***
OPCODE_b5:
	OR_ reg_L
	rjmp cpu_return

;*** ORA  M    [OR  register with A] ***
OPCODE_b6:
	movw r22,reg_HL
	rcall read_memory
	OR_ reg_M
	rjmp cpu_return

;*** ORA  A    [OR  register with A] ***
OPCODE_b7:
	OR_ reg_A
	rjmp cpu_return

;*** CMP  B    [Compare A with register B] ***
OPCODE_b8:
	CMP_ reg_B
	rjmp cpu_return

;*** CMP  C    [Compare register with A] ***
OPCODE_b9:
	CMP_ reg_C
	rjmp cpu_return

;*** CMP  D    [Compare register with A] ***
OPCODE_ba:
	CMP_ reg_D
	rjmp cpu_return

;*** CMP  E    [Compare register with A] ***
OPCODE_bb:
	CMP_ reg_E
	rjmp cpu_return

;*** CMP  H    [Compare register with A] ***
OPCODE_bc:
	CMP_ reg_H
	rjmp cpu_return

;*** CMP  L    [Compare register with A] ***
OPCODE_bd:
	CMP_ reg_L
	rjmp cpu_return

;*** CMP  M    [Compare register with A] ***
OPCODE_be:
	movw r22,reg_HL
	rcall read_memory
	CMP_ reg_M
	rjmp cpu_return

;*** CMP  A    [Compare register with A] ***
OPCODE_bf:
	CMP_ reg_A
	rjmp cpu_return

;*** RNZ      [Conditional return from subroutine if Zero flag not set] ***
OPCODE_c0:
	sbrs reg_flags,SREG_Z
	rjmp ret_routine
	rjmp cpu_return

;*** POP  BC   [Pop register pair from the stack] ***
OPCODE_c1:
	rcall pop_reg_pair
	movw reg_BC,r24
	rjmp cpu_return

;*** JNZ a    [Conditional jump if Zero flag not set] ***
OPCODE_c2:
	sbrs reg_flags,SREG_Z
	rjmp pc_load_pg
	rjmp cpu_return_inc_pc_2

;*** JMP  a    [Unconditional jump] ***
OPCODE_c3:
	rjmp pc_load_pg

;*** CNZ a    [Conditional subroutine call if Zero flag not set] ***
OPCODE_c4:
	sbrc reg_flags,SREG_Z
	rjmp cpu_return_inc_pc_2
	rjmp call_routine

;*** PUSH BC   [Push register pair on the stack] ***
OPCODE_c5:
	movw r20,reg_BC
	rjmp push_reg_pair

;*** ADI  #    [Add immediate to A] ***
OPCODE_c6:
	rcall read_pg_memory
	ADD_ r24
	rjmp cpu_return

;*** RZ       [Conditional return from subroutine if Zero flag set] ***
OPCODE_c8:
	sbrs reg_flags,SREG_Z
	rjmp cpu_return
	rjmp ret_routine

;*** RET       [Unconditional return from subroutine] ***
OPCODE_c9:
	rjmp ret_routine

;*** JZ  a    [Conditional jump if Zero flag set] ***
OPCODE_ca:
	sbrc reg_flags,SREG_Z
	rjmp pc_load_pg
	rjmp cpu_return_inc_pc_2

;*** CZ  a    [Conditional subroutine call if Zero flag set] ***
OPCODE_cc:
	sbrs reg_flags,SREG_Z
	rjmp cpu_return_inc_pc_2
	rjmp call_routine

;*** CALL a    [Unconditional subroutine call] ***
OPCODE_cd:
	rjmp call_routine

;*** ACI  #    [Add immediate to A with carry] ***
OPCODE_ce:
	rcall read_pg_memory
	ADC_ r24
	rjmp cpu_return

;*** RNC      [Conditional return from subroutine if Carry flag not set] ***
OPCODE_d0:
	sbrs reg_flags,SREG_C
	rjmp ret_routine
	rjmp cpu_return

;*** POP  DE   [Pop register pair from the stack] ***
OPCODE_d1:
	rcall pop_reg_pair
	movw reg_DE,r24
	rjmp cpu_return

;*** JNC a    [Conditional jump if Carry flag not set] ***
OPCODE_d2:
	sbrs reg_flags,SREG_C
	rjmp pc_load_pg
	rjmp cpu_return_inc_pc_2

;*** OUT  p    [Write A to output port] ***
OPCODE_d3:
	rcall read_pg_memory		;load port number
	;sts lastport,r24
	mov r22,reg_A
	rcall port_out				;output to port
	rjmp cpu_return

;*** CNC a    [Conditional subroutine call if Carry flag not set] ***
OPCODE_d4:
	sbrc reg_flags,SREG_C
	rjmp cpu_return_inc_pc_2
	rjmp call_routine

;*** PUSH DE   [Push register pair on the stack] ***
OPCODE_d5:
	movw r20,reg_DE
	rjmp push_reg_pair

;*** SUI  #    [Subtract immediate from A] ***
OPCODE_d6:
	rcall read_pg_memory
	SUB_ r24
	rjmp cpu_return

;*** RST 0-7    [Interrupt Call 0-7)] ***
OPCODE_RST:
	clr r25
	andi r24,0b00111000		;Jump interrupt vector
	rjmp call_routine_direct

;*** RC       [Conditional return from subroutine if Carry flag set] ***
OPCODE_d8:
	sbrc reg_flags,SREG_C
	rjmp ret_routine
	rjmp cpu_return

;*** JC  a    [Conditional jump if Carry flag set] ***
OPCODE_da:
	sbrc reg_flags,SREG_C
	rjmp pc_load_pg
	rjmp cpu_return_inc_pc_2

;*** IN   p    [Read input port into A] ***
OPCODE_db:
	rcall read_pg_memory		;load port number
	;sts lastport,r24
	rcall port_in				;input from port
	mov reg_A,r24
	rjmp cpu_return

;*** CC  a    [Conditional subroutine call if Carry flag set] ***
OPCODE_dc:
	sbrs reg_flags,SREG_C
	rjmp cpu_return_inc_pc_2
	rjmp call_routine

;*** SBI  #    [Subtract immediate from A with borrow] ***
OPCODE_de:
	rcall read_pg_memory
	SBB_ r24
	rjmp cpu_return

;*** RPO      [Conditional return from subroutine if Parity flag not set (ODD)] ***
OPCODE_e0:
	rcall compute_parity	;set the parity in flag in T
	sbrc reg_flags,SREG_T
	rjmp cpu_return
	rjmp ret_routine

;*** POP  HL   [Pop register pair from the stack] ***
OPCODE_e1:
	rcall pop_reg_pair
	movw reg_HL,r24
	rjmp cpu_return

;*** JPO a    [Conditional jump if Parity flag not set (ODD)] ***
OPCODE_e2:
	rcall compute_parity	;set the parity in flag in T
	sbrc reg_flags,SREG_T
	rjmp cpu_return_inc_pc_2
	rjmp pc_load_pg

;*** XTHL      [Swap H:L with top word on stack] ***
OPCODE_e3:
	movw r22,reg_SP
	rcall read_memory_16_no_restart
	movw r20,reg_HL
	movw reg_HL,r24
	movw r22,reg_SP
	rcall write_memory_16
	rjmp cpu_return

;*** CPO a    [Conditional subroutine call if Parity flag not set (ODD)] ***
OPCODE_e4:
	rcall compute_parity	;set the parity in flag in T
	sbrc reg_flags,SREG_T
	rjmp cpu_return_inc_pc_2
	rjmp call_routine

;*** PUSH HL   [Push register pair on the stack] ***
OPCODE_e5:
	movw r20,reg_HL
	rjmp push_reg_pair

;*** ANI  #    [AND immediate with A] ***
OPCODE_e6:
	rcall read_pg_memory		;load immediate value
	ANA_ r24
	rjmp cpu_return

;*** RPE      [Conditional return from subroutine if Parity flag set (EVEN)] ***
OPCODE_e8:
	rcall compute_parity	;set the parity in flag in T
	sbrs reg_flags,SREG_T
	rjmp cpu_return
	rjmp ret_routine

;*** PCHL      [Jump to address in H:L] ***
OPCODE_e9:
	movw reg_PC,reg_HL
	DEASSERT_SPI
	rcall spi_restart_seq
	rjmp cpu_return

;*** JPE a    [Conditional jump if Parity flag set (EVEN)] ***
OPCODE_ea:
	rcall compute_parity	;set the parity in flag in T
	sbrs reg_flags,SREG_T
	rjmp cpu_return_inc_pc_2
	rjmp pc_load_pg

;*** XCHG      [Exchange DE and HL content] ***
OPCODE_eb:
	movw reg_temp,reg_DE
	movw reg_DE,reg_HL
	movw reg_HL,reg_temp
	rjmp cpu_return

;*** CPE a    [Conditional subroutine call if Parity flag set (EVEN)] ***
OPCODE_ec:
	rcall compute_parity	;set the parity in flag in T
	sbrs reg_flags,SREG_T
	rjmp cpu_return_inc_pc_2
	rjmp call_routine

;*** XRI  #    [ExclusiveOR immediate with A] ***
OPCODE_ee:
	rcall read_pg_memory		;load immediate value
	XRA_ r24
	rjmp cpu_return

;*** RP       [Conditional return from subroutine if Sign flag not set (POSITIVE)] ***
OPCODE_f0:
	sbrs reg_flags,SREG_N
	rjmp ret_routine
	rjmp cpu_return

;*** POP PSW (A:flags)   [Pop register pair from the stack] ***
OPCODE_f1:
	rcall pop_reg_pair

	;TODO
	;mov ZL,r24
	;lds ZH,hi8(flags_pop_lookup_tbl)
	;lpm reg_flags,Z


	;convert flags to 8080 format
	ldi reg_flags,(1<<SREG_I)	;0b10000000  -->AVR Flags
	sbrc r24,0					;8080 C
	ori reg_flags,(1<<SREG_C)
	sbrc r24,2					;8080 P
	ori reg_flags,(1<<SREG_T)
	sbrc r24,4					;8080 AC
	ori reg_flags,(1<<SREG_H)
	sbrc r24,6					;8080 Z
	ori reg_flags,(1<<SREG_Z)
	sbrc r24,7					;8080 S
	ori reg_flags,(1<<SREG_N)

	mov reg_A,r25

	;restore parity register
	mov reg_parity,reg_flags	;00000001
	ldi reg_temp,(1<<SREG_T)	;00000001
	and reg_parity,reg_temp		;00000001
	eor reg_parity,reg_temp		;00000000

	rjmp cpu_return

;*** JP  a    [Conditional jump if Sign flag not set (POSITIVE)] ***
OPCODE_f2:
	sbrs reg_flags,SREG_N
	rjmp pc_load_pg
	rjmp cpu_return_inc_pc_2

;*** DI        [Disable interrupts] ***
OPCODE_f3:
	sts intflag,reg_value_0
	rjmp cpu_return

;*** CP  a    [Conditional subroutine call if Sign flag not set (POSITIVE)] ***
OPCODE_f4:
	sbrc reg_flags,SREG_N
	rjmp cpu_return_inc_pc_2
	rjmp call_routine

;*** PUSH PSW (A:flags)   [Push register pair on the stack] ***
OPCODE_f5:
	rcall compute_parity	;puts parity flag in T bit of reg_flags

	;TODO
	;mov ZL,r24
	;lds ZH,hi8(flags_push_lookup_tbl)
	;lpm reg_flags,Z

	;convert flags to 8080 format
	ldi reg_temp,0b00000010
	sbrc reg_flags,SREG_C
	ori reg_temp,1			;bit 0
	sbrc reg_flags,SREG_T
	ori reg_temp,4			;bit 2
	sbrc reg_flags,SREG_H
	ori reg_temp,16			;bit 4
	sbrc reg_flags,SREG_Z
	ori reg_temp,64			;bit 6
	sbrc reg_flags,SREG_N
	ori reg_temp,128		;bit 7

	mov r20,reg_temp
	mov r21,reg_A
	rjmp push_reg_pair

;*** ORI  #    [OR  immediate with A] ***
OPCODE_f6:
	rcall read_pg_memory
	OR_ r24
	rjmp cpu_return

;*** RM      [Conditional return from subroutine if Sign flag set (MINUS)] ***
OPCODE_f8:
	sbrs reg_flags,SREG_N
	rjmp cpu_return
	rjmp ret_routine

;*** SPHL      [Set SP to content of H:L] ***
OPCODE_f9:
	movw reg_SP,reg_HL
	rjmp cpu_return

;*** JM a    [Conditional jump if Sign flag set (MINUS)] ***
OPCODE_fa:
	sbrc reg_flags,SREG_N
	rjmp pc_load_pg
	rjmp cpu_return_inc_pc_2

;*** EI        [Enable interrupts] ***
OPCODE_fb:
	sts intflag,reg_value_1
	rjmp cpu_return

;*** CM a    [Conditional subroutine call if Sign flag set (MINUS)] ***
OPCODE_fc:
	sbrs reg_flags,SREG_N
	rjmp cpu_return_inc_pc_2
	rjmp call_routine

;*** CPI  #    [Compare immediate with A] ***
OPCODE_fe:
	rcall read_pg_memory	;read 1 bytes in r24
	CMP_ r24
	rjmp cpu_return


compute_parity:
	mov ZL,reg_parity
	ldi ZH,hi8(parity_tbl)
	lpm reg_temp,Z
	bst reg_temp,0
	bld reg_flags,SREG_T		;Return parity flag in T bit

/*
	ldi ZL,lo8(parity_tbl)
	ldi ZH,hi8(parity_tbl)
	add ZL,reg_parity
	adc ZH,reg_value_0
	lpm reg_temp,Z
	bst reg_temp,0
	bld reg_flags,SREG_T		;put parity flag in T bit
*/
	ret


cpu_exec:
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
	push YL
	push YH
	push ZL
	push ZH

	sts halt,r1			;clear the last HLT flag if any
	mov reg_op_count,r24		;instructions to execute

	;initialize set 0
	lds reg_PC_lo,pc
	lds reg_PC_hi,pc+1
	lds reg_L,rL
	lds reg_H,rH
	lds reg_flags,rFlags
	lds reg_A,rA
	lds reg_B,rB
	lds reg_C,rC
	lds reg_D,rD
	lds reg_E,rE
	lds reg_SP_lo,rSp_lo
	lds reg_SP_hi,rSp_hi
	lds reg_parity,rParity


	ldi reg_temp,0
	mov reg_value_0,reg_temp
	ldi reg_temp,1
	mov reg_value_1,reg_temp
	ldi reg_temp,2
	mov reg_value_2,reg_temp

	sts halt,reg_value_0

	rcall spi_restart_seq
	rjmp cpu_start
;
; *** Main Loop ***
;
cpu_return_inc_pc_2:		;returns from a non-taken branch instruction
	rcall read_pg_memory_16	;faster to read skipped adress than restart the spi ram

cpu_start:
cpu_return:

#ifdef EMULATOR_DEBUG
	lds reg_temp,breakpoint
	lds reg_temp2,breakpoint+1
	cp reg_temp,reg_PC_lo
	cpc reg_temp2,reg_PC_hi
	breq cpu_exit
#endif


#ifdef DEBUG_EMU
	dec reg_op_count
	brmi cpu_exit
#endif

#ifdef DEBUG_EMU
	sts lastpc,reg_PC_lo
	sts lastpc+1,reg_PC_hi
#endif

	rcall read_pg_memory	;read next opcode
	ldi zl,lo8(pm(jump_table))
	ldi zh,hi8(pm(jump_table))
	add zl,r24
	adc zh,reg_value_0


#ifdef DEBUG_EMU
	sts lastop,r24
#endif

	ijmp					;execute instruction

cpu_exit:					; branches from HALT
	DEASSERT_SPI

	sts pc,reg_PC_lo
	sts pc+1,reg_PC_hi
	sts rL,reg_L
	sts rH,reg_H
	sts rFlags,reg_flags
	sts rA,reg_A
	sts rB,reg_B
	sts rC,reg_C
	sts rD,reg_D
	sts rE,reg_E
	sts rSp_lo,reg_SP_lo
	sts rSp_hi,reg_SP_hi
	sts rParity,reg_parity

	;rcall convert_flags
	;sts flags_8080,r24

	rcall compute_parity	;puts parity flag in T bit of reg_flags
	;convert flags to 8080 format
	ldi reg_temp,0b00000010
	sbrc reg_flags,SREG_C
	ori reg_temp,1
	sbrc reg_flags,SREG_T
	ori reg_temp,4
	sbrc reg_flags,SREG_H
	ori reg_temp,16
	sbrc reg_flags,SREG_Z
	ori reg_temp,64
	sbrc reg_flags,SREG_N
	ori reg_temp,128
	sts flags_8080,reg_temp

	lds r24,halt		;return 1 if HALT was executed
	clr r25

	pop ZH
	pop ZL
	pop YH
	pop YL
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

	clr r1

	ret

/*
 * Initializes the CPU
 * r25:r24 = Initial program counter adress
*/
cpu_init:
	sts pc,r24
	sts pc+1,r25

	in r24,_SFR_IO_ADDR(SREG)	;insure Global Interrupt bit is on
	andi r24,(1<<SREG_I)

	sts rL,r1
	sts rH,r1
	sts rA,r1
	sts rB,r1
	sts rC,r1
	sts rD,r1
	sts rE,r1
	sts rSp_lo,r1
	sts rSp_hi,r1
	sts rFlags,r24
	ldi r24,1
	sts rParity,r24	;put a number with odd number of bit to turn off the flag at init

	sts debug,r1
	sts intflag,r1

	ldi r24,0xff
	sts breakpoint,r24
	sts breakpoint,r24

	ret

//////////////////////////////////////////////////////////////////////////////////////////////
///  Physical memory emulation layer
//   Works for 64K and 128K SPI RAM
//////////////////////////////////////////////////////////////////////////////////////////////
/*
* Load program counter from pgm memory
*/
pc_load_pg:
	in    reg_PC_lo, SR_DR
	out   SR_DR, reg_temp
	wait  17
	in    reg_PC_hi, SR_DR
	DEASSERT_SPI
	rcall spi_restart_seq
	rjmp cpu_return

/*
 * Reads the next byte in program memory pointed to by PC and then increment PC.
*/
read_pg_memory:
	wait  10 ;normally 12
	in    r24, SR_DR
	out   SR_DR, reg_temp	;restart next xfer
	adiw reg_PC,1
	ret

/*
 * Reads the 2 next byte in program memory pointed to by PC and then increment PC by 2.
 *
 * Uses PC to index memory.
 * returns teh word in r24:r25
*/
read_pg_memory_16:
	wait  10 ;normally 12
	in    r24, SR_DR
	out   SR_DR, reg_temp
	wait  17
	in    r25,     SR_DR
	out   SR_DR, reg_temp	;restart next xfer
	adiw reg_PC,2
	ret

/*
 * Reads a byte from random memory.
 ** r23:r22: Address
*/
read_memory:
	DEASSERT_SPI
	clr r24
	rcall spi_read_u8
	rcall spi_restart_seq
	ret

read_memory_no_restart:
	DEASSERT_SPI
	clr r24
	rjmp spi_read_u8

/*
 * Reads a word from random memory.
*/
read_memory_16:
	DEASSERT_SPI
	rcall spi_read_u16
	rcall spi_restart_seq
	ret

read_memory_16_no_restart:
	DEASSERT_SPI
	rjmp spi_read_u16

/*
 * Write a byte to memory.

 ** Inputs:
**     r24: Bank (low bit used to select low / high 64K)
** r23:r22: Address
**     r20: Value

*/
write_memory:
	DEASSERT_SPI
	clr r24
	rcall spi_write_u8 ;SpiRamWriteU8 ;103 cycles
	rcall spi_restart_seq
	ret

write_memory_16:
	DEASSERT_SPI
	rcall spi_write_u16 ;SpiRamWriteU16
	rcall spi_restart_seq
	ret


/*
 * Call a subroutine
*/
call_routine:

	;read spi u16
	in    r24, SR_DR
	out   SR_DR, reg_temp
	wait  17
	in    r25,     SR_DR

	adiw reg_PC,2			;increment pc to point on return inst
call_routine_direct:
	movw r20,reg_PC			;store return adress
	movw reg_PC,r24			;Load PC with subroutine adress

push_reg_pair:

	DEASSERT_SPI

	sub reg_P,reg_value_2		;decrement stack pointer by 2
	sbc reg_S,reg_value_0

	movw r22,reg_SP			;write to stack
	rcall spi_write_u16;SpiRamWriteU16		;Inputs: r23:r22: Address, r21:r20: Value

	nop
	nop

	rcall spi_restart_seq

	rjmp cpu_return


/*
 * Returns from subroutine
*/
ret_routine:
	DEASSERT_SPI

	movw r22,reg_SP			;stack adress
	rcall spi_read_u16		;get routine address
	movw reg_PC,r24
	add reg_P,reg_value_2		;increment stack pointer by 2
	adc reg_S,reg_value_0
	rcall spi_restart_seq
	rjmp cpu_return

/*
 * Pops a word from the stack
*/
pop_reg_pair:

	DEASSERT_SPI

	movw r22,reg_SP			;stack adress
	rcall spi_read_u16		;get routine address
	add reg_P,reg_value_2		;increment stack pointer by 2
	adc reg_S,reg_value_0

	rcall spi_restart_seq

	ret


//////////////////////////////////////////////////////////////////////////////////////////////
///  SPI memory access
///  Essentially based on Jubatian SPI code. See kernel/spirams.s
//////////////////////////////////////////////////////////////////////////////////////////////
/*
 * Fastest sequence to restart the spi ram in sequence mode
*/
spi_restart_seq:
	wait 4					; seems to be required to give time to recover from the last de-assert (TCSD in datasheet?)
	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   reg_temp,     0x03    ; Read from SPI RAM
	out   SR_DR,   reg_temp     ; Send read command
	wait 12

	lds reg_temp,ram_size
	cpi reg_temp,RAM_SIZE_64K
	breq 1f

	clr reg_temp
	out   SR_DR,   reg_temp     ; Address bits 16 - 23
	wait 17
1:
	out   SR_DR,   reg_PC_hi     ; Address bits 8 - 15
	wait 17
	out   SR_DR,   reg_PC_lo     ; Address bits 0 - 7
	wait 17
	out   SR_DR,   reg_temp
	ret

spi_read_u8:
	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   reg_temp,     0x03    ; Read from SPI RAM
	out   SR_DR,   reg_temp     ; Send read command
	wait 12

	lds reg_temp,ram_size
	cpi reg_temp,RAM_SIZE_64K
	breq 1f
	clr reg_temp
	out   SR_DR,  reg_temp     ; Address bits 16 - 23
	wait 17
1:
	out   SR_DR,   r23     ; Address bits 8 - 15
	wait 17
	out   SR_DR,   r22     ; Address bits 0 - 7
	wait 17
	out   SR_DR,   reg_temp
	wait 16
	in    r24,     SR_DR   ; (17)
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	ret

spi_read_u16:
	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   reg_temp,     0x03    ; Read from SPI RAM
	out   SR_DR,   reg_temp     ; Send read command
	wait 12

	lds reg_temp,ram_size
	cpi reg_temp,RAM_SIZE_64K
	breq 1f
	clr reg_temp
	out   SR_DR,   reg_temp     ; Address bits 16 - 23
	wait 17
1:
	out   SR_DR,   r23     ; Address bits 8 - 15
	wait 17
	out   SR_DR,   r22     ; Address bits 0 - 7
	wait 17
	out   SR_DR,   reg_temp
	wait 16
	in    r24,     SR_DR   ; (17)
	out   SR_DR,   reg_temp
	wait 16
	in 	  r25, SR_DR
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	ret

spi_write_u8:
	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   reg_temp,     0x02    ; Read from SPI RAM
	out   SR_DR,   reg_temp     ; Send read command
	wait 12

	lds reg_temp,ram_size
	cpi reg_temp,RAM_SIZE_64K
	breq 1f
	clr reg_temp
	out   SR_DR,   reg_temp     ; Address bits 16 - 23
	wait 17
1:
	out   SR_DR,   r23     ; Address bits 8 - 15
	wait 17
	out   SR_DR,   r22     ; Address bits 0 - 7
	wait 17
	out   SR_DR,   r20
	wait 17
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	ret

spi_write_u16:
	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   reg_temp,     0x02    ; Read from SPI RAM
	out   SR_DR,   reg_temp     ; Send read command
	wait 12

	lds reg_temp,ram_size
	cpi reg_temp,RAM_SIZE_64K
	breq 1f
	clr reg_temp
	out   SR_DR,   reg_temp     ; Address bits 16 - 23
	wait 17
1:
	out   SR_DR,   r23     ; Address bits 8 - 15
	wait 17
	out   SR_DR,   r22     ; Address bits 0 - 7
	wait 17
	out   SR_DR,   r20
	wait 16
	in    r24,     SR_DR   ; (17)
	out   SR_DR,   r21
	wait 17
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	ret

;Initialize the SPI RAM and return the actuel RAM size detected
;
;returns:	0=64k
;			1=128K
;			0xff=Error
/*
spi_ram_init:
	ldi r24,(1<<SPE)+(1<<MSTR)
	out _SFR_IO_ADDR(SPCR),24
	ldi r24,(1<<SPI2X)	;enable SPI in 2X mode
	out _SFR_IO_ADDR(SPSR),r24

;	in r24,_SFR_IO_ADDR(DDRB)
;	ori r24,(1<<PB7)+(1<<PB5)
;	out _SFR_IO_ADDR(DDRD),r24 (bug ?)


	sbi   _SFR_IO_ADDR(DDRB), PB7
	sbi   _SFR_IO_ADDR(DDRB), PB5

	sbi _SFR_IO_ADDR(PORTA),PA4			;deassert SPI RAM
	sbi _SFR_IO_ADDR(DDRA),PA4

	;check installed RAM

	ldi r24,RAM_SIZE_64K
	sts ram_size,r24
	ldi r22,0x20
	ldi r23,0x40
	ldi r20,0xca						;write arbitrary value to first 64K bank
	rcall spi_write_u8
	rcall spi_read_u8					;read back
	cpi r24,0xca
	breq 1f								;64k detected, return

	ldi r24,RAM_SIZE_128K
	sts ram_size,r24
	ldi r22,0x20
	ldi r23,0x40
	ldi r20,0xca						;write arbitrary value to second 64K bank
	rcall spi_write_u8
	rcall spi_read_u8					;read back
	cpi r24,0xca
	breq 1f								;128K detected, return

	ldi r24, 0xff						;error, cannot initialize
	sts ram_size,r24
1:
	lds r24,ram_size
	ret
*/

cpu_set_ram_size:
	sts ram_size,r24
	ret

//////////////////////////////////////////////////////////////////////////////////////////////
///  External I/O emulation : Disk drive, etc
//////////////////////////////////////////////////////////////////////////////////////////////

;in: r24=port no
;    r22=data
port_in:
	cpi r24,17+2+2
	brlo 1f
	ret
1:
	ldi ZL,lo8(pm(ports_in))
	ldi ZH,hi8(pm(ports_in))
	add ZL,r24
	adc ZH,reg_value_0
	ijmp

;in:  r24=port no
;out: r24=data
port_out:
	cpi r24,17+3+2
	brlo 1f
	ret
1:
	ldi ZL,lo8(pm(ports_out))
	ldi ZH,hi8(pm(ports_out))
	add ZL,r24
	adc ZH,reg_value_0
	ijmp

; see https://www.tramm.li/i8080/
ports_in:
	rjmp PORT00_IN	;Get console status
	rjmp PORT01_IN	;Get console char
	ret				;printer status
	ret				;printer data
	ret				;auxilary status / command
	ret				;auxilary data
	ret				;Uzenet interface status - 0=Ok, 1=Network not configured, 2=connected, 3=have IP adress, 0xff=Unknown interface error
	ret				;Uzenet interface baud rate - 0=1200, 1=2400, 2=4800, 3=9600....
	ret				;Uzenet data status - 0=no char avail, 0xff=char avail
	ret				;Uzenet data I/O - read next byte from UART. If no new byte was available, returns the last byte.
	ret				;FDC drive no
	ret				;FDC track
	ret				;FDC sector
	rjmp PORT0D_IN  ;Get FDC command status
	rjmp PORT0E_IN	;Get FDC status
	rjmp PORT0F_IN	;FDC DMA address low
	rjmp PORT10_IN	;FDC DMA address hi
	rjmp PORT11_IN	;Get 60hz counter lsb. Note: when read, it latches the MSB in a temporary location to conserve atomicity of the current 16-bit value.
	rjmp PORT12_IN	;Get 60hz counter msb.
	ret


ports_out:
	ret				;console status
	rjmp PORT01_OUT ;Output char to console
	ret			    ;printer status
	ret				;printer data
	ret				;auxilary status / command
	ret				;auxilary data
	ret
	ret
	ret
	ret
	rjmp PORT0A_OUT ;Set FDC drive no
	rjmp PORT0B_OUT ;Set FDC track
	rjmp PORT0C_OUT ;Set FDC sector
	rjmp PORT0D_OUT ;Set FDC command
	ret				;FDC status
	rjmp PORT0F_OUT ;Set FDC DMA address low
	rjmp PORT10_OUT ;Set FDC DMA address hi
	rjmp PORT11_OUT	;Reset the counter
	ret
	rjmp PORT13_OUT	;Uzebox extension: execute native app


PORT00_IN:
	call terminal_HasChar
	cpse r24,reg_value_0
	ser r24			;CP/M needs 0xff is data is available.
	ret

PORT01_IN:
	call terminal_GetChar
	ret

; Read FDC command: check for IO completion,
; 0 = still in progress, 1 = IO done
PORT0D_IN:  ;FDC command status
	ldi r24,1
	ret

PORT0E_IN:	;FDC status
	lds r24,fdc_status	;0=OK, 1=error
	ret

PORT0F_IN: ;FDC DMA address low
	lds r24,dma
	ret

PORT10_IN: ;FDC DMA address hi
	lds r24,dma+1
	ret

PORT11_IN:	;counter lo byte
	lds r24,counter+1
	sts counter_lat,r24
	lds r24,counter
	ret

PORT12_IN:	;counter hi byte
	lds r24,counter_lat
	ret

PORT01_OUT:;console data
	clr r25
	mov r24,r22
	call putchar
	;call ansi_putchar
	ret

PORT0A_OUT: ;FDC drive
	sts drive,r22

	DEASSERT_SPI
	mov r24,r22
	call _open_file
	rcall spi_restart_seq

	ret

PORT0B_OUT: ;FDC track
	sts track,r22
	ret

PORT0C_OUT: ;FDC sector
	sts sector,r22
	ret

;write FDC command: transfer one sector in the wanted direction,
;input r22: 0 = read, 1 = write
PORT0D_OUT: ;FDC command
	DEASSERT_SPI
	sts fdc_op,r22
	sts fdc_retries,r1	;clear retries count

fdc_retry:
	;compute adress of requested sector relative to beginning of file
	;adress=((track*26)+(sector-1))*128
	lds	r24, track		;track
	lds	r22, sector		;sector
	dec r22				;(sector-1)

	ldi r23,26
	mul r24,r23			;(track*26)
	add r0,R22
	adc r1,reg_value_0	;(track*26)+(sector-1)

	clr r22
	mov r23,r0			;*256/2==*128
	mov r24,r1
	lsr r24
	ror r23
	ror r22
	clr r25
	clr r1

	lds r0,fdc_op
	cp  r0,r1		;read or write?
	brne fdc_write

	call pf_lseek	;seek by 128 bytes sectors

	;read sector into buffer
	ldi r20,lo8(opcount)
	ldi r21,hi8(opcount)
	ldi	r22,128
	ldi	r23,0
	ldi r24,lo8(dmabuf)
	ldi r25,hi8(dmabuf)
	call pf_read
	cpi r24,0			;success?
	breq fdc_read_ok

fdc_reinit:
	;error, try again
	lds r24,fdc_retries
	cpi r24,5
	brsh fdc_fail
	inc r24
	sts fdc_retries,r24

	call _init_fdc
	lds r24,drive
	call _open_file

	rjmp fdc_retry

fdc_fail:
	rcall spi_restart_seq
	ldi r24,1 ;unrecoverable disk error!
	sts fdc_status,r24
	ret

fdc_read_ok:
	;copy back to spiram
	clr r24
	lds r22,dma
	lds r23,dma+1
	call SpiRamSeqWriteStart

	ldi ZL,lo8(dmabuf)
	ldi ZH,hi8(dmabuf)
	ldi r22,128/2
3:
	ld r24,Z+
	ld r25,Z+
	call SpiRamSeqWriteU16
	dec r22
	brne 3b
	call SpiRamSeqWriteEnd
	rjmp fdc_exit

fdc_write:
	;To do an SD write we must do a
	;read-modify-write on the full 512
	;bytes sector.

	;Round the seek adress to a 512 bytes boundary.
	;and save bits 8 & 9 (index of the 128bytes "sub sector")
	clr reg_temp
	bst r22,7
	bld reg_temp,0
	bst r23,0
	bld reg_temp,1
	clr r22
	andi r23,0xfe
	sts sec_off,reg_temp	;save for later
	sts fptr0,r22			;save file pointer to rewind later
	sts fptr1,r23
	sts fptr2,r24

	call pf_lseek			;seek by 512 bytes sectors

	;read the whole sector into buffer
	ldi r20,lo8(opcount)
	ldi r21,hi8(opcount)
	ldi	r22,lo8(512)
	ldi	r23,hi8(512)
	ldi r24,lo8(dmabuf)
	ldi r25,hi8(dmabuf)
	call pf_read
	cpi r24,0				;success?
	breq 1f
	rjmp fdc_reinit			;no, go to retry
1:
	;rewind to previous sector since
	;pf_read increments the read pointer
	lds r22,fptr0
	lds r23,fptr1
	lds r24,fptr2
	clr r25
	call pf_lseek			;seek by 512 bytes sectors

	;copy spiram to dma buffer
	clr r24
	lds r22,dma
	lds r23,dma+1
	call SpiRamSeqReadStart

	ldi ZL,lo8(dmabuf)
	ldi ZH,hi8(dmabuf)

	;add sub-sector offset
	ldi reg_temp,128
	lds reg_temp2,sec_off
	mul reg_temp,reg_temp2
	add ZL,r0
	adc ZH,r1

	clr r1
	ldi r22,128/2
3:
	call SpiRamSeqReadU16
	st Z+,r24
	st Z+,r25
	dec r22
	brne 3b
	call SpiRamSeqReadEnd

	;write back buffer to FDC
	ldi r20,lo8(opcount)
	ldi r21,hi8(opcount)
	ldi	r22,lo8(512)
	ldi	r23,hi8(512)
	ldi r24,lo8(dmabuf)
	ldi r25,hi8(dmabuf)
	call pf_write

	cpi r24,0			;success?
	breq fdc_exit
	rjmp fdc_reinit		;if no reinit the sd and retry

fdc_exit:
	rcall spi_restart_seq
	clr r24				;return success
	sts fdc_status,r24
	ret

PORT0F_OUT: ;FDC DMA address low
	sts dma,r22
	ret

PORT10_OUT: ;FDC DMA address hi
	sts dma+1,r22
	ret

PORT11_OUT:
	cli
	sts counter,r1
	sts counter+1,r1
	sei
	ret

PORT13_OUT:
	;Port value=app id
	DEASSERT_SPI
	mov r24,r22
	call computer_RunNativeApp
	rcall spi_restart_seq
	ret


.global pf_seek_good
;r25:r24=track
;r23:r22=sector
pf_seek_good:
	;adress=((track*26)+(sector-1))*512
	;lds	r24, track		;track
	;lds	r22, sector		;sector
	dec r22				;(sector-1)

	ldi r23,26
	mul r24,r23			;(track*26)
	add r0,R22
	adc r1,reg_value_0	;(track*26)+(sector-1)

	clr r22
	clr r25
	mov r23,r0			;*256
	mov r24,r1
	lsl r23				;*256*2==*512
	rol r24

	clr r1
	call pf_lseek
	ret

