;**************************************************************
;*
;*           Uzebox Telnet CP/M client stub
;*     Launcher for the Uzebox native telnet client.
;*                  (c) Uze 2020
;*
;* Note: To compile these sources, the cross-assembler
;* [The Macro Assembler AS / TASM]
;* (http://john.ccac.rwth-aachen.de:8000/as/) is required.
;*
;* See: /default/build-telnet.bat for the build script.
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
TELNTID 	EQU 0		;Native Telnet app id

break		MACRO		;pseudo-op used for breakpoints
			db 8
			ENDM

start:
		;check command line
		lxi hl,TBUFF
		mov a,m
		cpi 0
		jz checkfile

		inx hl			;skip first empty space
		inx hl
		mov a,m
		cpi '-'			;check if we want the help page
		jnz connect
		inx hl
		mov a,m
		cpi '?'
		jz exitinfo
		ani 0DFH	;convert to uppercase
		cpi 'H'
		jz exitinfo
		jmp invparam
checkfile:
		;no command line args, do we have a hosts file defined?
		lxi de,fcb
		mvi c,FOPEN
		call BDOS
		cpi 0FFH
		jz exitinfo			;file not found, exit with message

		lxi hl,fdata
loop:
		;point to read buffer
		push hl
		mov b,h
		mov c,l
		call SETDMA

		;read file sectors
		lxi de,fcb
		mvi c,FREADSEQ
		call BDOS

		;increment read buffer by 128
		pop hl
		lxi bc,128
		dad bc

		cpi 0
		jz loop			;more sectors to read

		mvi a,0
		mov m,a			;end of data

		;clear screen and title title
		lxi d,cls
		mvi c,9
		call BDOS

		;display loading from file
		lxi d,datfile
		mvi c,9		;output string
		call BDOS

		;close file
		lxi de,fcb
		mvi c,FCLOSE
		call BDOS

		;find pointers to the hosts
		lxi hl,hosts
		lxi de,fdata
		mvi b,'A'
mloop:
		;store pointer to current host
		mov a,e
		stax hl
		mov a,d
		inx hl
		stax hl
		inx hl

		mov c,b
		call CONOUT
		mvi c,'.'
		call CONOUT
		mvi c,' '
		call CONOUT
hloop:
		ldax de
		inx de
		cpi 0AH ;'\n'
		jz nexthost

		mov c,a
		call CONOUT		;output the host name
		jmp hloop

nexthost:
		inr b
		ldax de
		cpi '$'			;we reached the last item in the file?
		jz endhosts

		mvi c,0AH ;'\n'
		call CONOUT
		jmp mloop
endhosts:

		mvi c,'\r'
		call CONOUT
		mvi c,'\n'
		call CONOUT

invkey:
		call CONST	;key avail?
		cpi 0
		jz invkey

		call CONIN	;get key in A

		cpi CTRLC
		jz quit
		ani 0DFH	;convert to uppercase
		cpi 'A'
		jm  invkey	;key lower than 'A'
		cmp b
		jp  invkey	;key higher than the max num of hosts

		;calculate pointer to host
		sui 'A'
		lxi hl,hosts
		stc
		cmc
		ral 		;*2
		mov e,a
		mvi d,0
		dad de

		mov e,m
		inx hl
		mov d,m

		lxi hl,TBUFF+1
		mvi b,0
cmdloop:
		ldax de
		cpi '\r'
		jz endcmd
		cpi ' '
		jz endcmd
		inr b
		inx de

		mov m,a
		inx hl

		jmp cmdloop
endcmd:
		lxi hl,TBUFF
		mov m,b			;command lenght

connect:
		mvi a,TELNTID
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

fcb:	db 0		;default drive
		db "TELNET  "
		db "DAT"
		db 0,0,0,0,0,0,0,0
		db 0,0,0,0,0,0,0,0
		db 0,0,0,0,0,0,0,0

cls: 	db 27,"[J",27,"[H"
title:	db "Uzebox Telnet Client v1.0","\r\n\r\n",'$'

info:	db "Usage: telnet [-h|-?] host[:port|default=23]\r\n"		;use ANSI inverse video
		db " [-h|-?] : Display this help page.\r\n"
		db " Ctrl-X: Exit the telnet session.\r\n"
		db "Example: telnet telehack.com:23\r\n"
		db "Optionnaly provide a TELNET.DAT file with hosts, one per line\r\n"
		db " and ending the file with a dollar sign.\r\n"
		db '$'

invstr: db "Invalid command line parameter(s).\r\n\r\n", '$'

datfile:db "No host specified, loading TELNET.DAT.\r\nPress Ctrl-X at anytime to exit the Telnet session.\r\n\r\n", '$'

hosts:	ds 20*2		;pointers to hosts (max 20)

fdata:	db 0		;host file loaded here



