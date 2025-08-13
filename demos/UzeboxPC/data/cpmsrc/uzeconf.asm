;**************************************************************
;*
;*             Uzenet configuration laucher for CP/M 2.2
;*                  (c) Uze 2020
;*
;*  This program is executable from CP/M and simply delegates
;*  to a native AVR application to perform the wifi module setup.
;*
;**************************************************************
	cpu 8080
	ORG 100h

IOBYTE		EQU	3		;i/o definition byte.
CDISK		EQU	4		;current drive name and user number.
BDOS		EQU	5		;entry point for the cp/m bdos.
TFCB		EQU	05CH	;default file control block.
TBUFF		EQU	80H		;i/o buffer and command line storage.
TBASE		EQU	100H	;transiant program storage area.
CONST		EQU 0FA06H  ;BIOS call to check if a char is available
CONOUT		EQU 0FA0CH	;BIOS call to output a char to console
CONIN		EQU 0FA09H	;BIOS call to wait for a char from the console
SETDMA		EQU	0FA24H	;BIOS call to set DMA adress
EXECIO		EQU 13H		;I/O port used to transfer control to native app on the avr MCU
FOPEN		EQU 15		;BDOS open file
FCLOSE		EQU 16		;BDOS close file
FREADSEQ	EQU 20		;Read file sequentially
CTRLC		EQU 3		;ascii code for CTRL-C

NATIVEID 	EQU 1		;ID for Uzenet native configuration app

break		MACRO		;pseudo-op used for breakpoints
			db 8
			ENDM

start:
		;check command line
		lxi hl,TBUFF
		mov a,m
		cpi 0
		jz noargs		;no arguments on command line
		inx hl			;skip first empty space
		inx hl
		mov a,m
		cpi '-'			;check if we want the help page
		jnz noargs
		inx hl
		mov a,m
		cpi '?'
		jz exitinfo
		ani 0DFH	;convert to uppercase
		cpi 'H'
		jz exitinfo
		jmp invparam
noargs:
		mvi a,NATIVEID
		out EXECIO		;invoke native app
		ret
invparam:
		lxi d,invstr
		mvi c,9		;output string
		call BDOS
exitinfo:
		lxi d,title
		mvi c,9		;output string
		call BDOS

		lxi d,info
		mvi c,9		;output string
		call BDOS
quit:
		ret


cls: 	db 27,"[J",27,"[H"
title:	db 27,"[7m","Uzenet Configuration v1.0",27,"[m\r\n",'$' ;use ANSI inverse video

info:	db "Usage: Uzeconf\r\n"
		db "Configure the wifi parameters of the Uzenet interface.\r\n"
		db '$'

invstr: db "Invalid command line parameter(s).\r\n\r\n", '$'




