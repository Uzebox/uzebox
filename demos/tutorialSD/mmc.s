#include <avr/io.h>
#include "defines.h"

// MMC/SD/SPI stuff
#define SPI_PORT	PORTB
#define SPI_DDR		DDRB
#define SPI_PIN		PINB

#define MMC_CS_PORT	PORTD
#define MMC_CS_DIR	DDRD

#define SD_SCK		1	
#define SD_CMD		2
#define SD_DAT0		3
#define SD_DAT3		4
#define SD_DAT1		5
#define SD_DAT2		6
#define SD_CARD		7

#define MMC_SCK    7
#define MMC_MOSI   5
#define MMC_MISO   6
#define MMC_CS     6

// command values to be used with mmccommand
#define CMD_RESET 0
#define CMD_INIT 1
#define CMD_SEND_CSD 9
#define CMD_READBLOCK 17
#define CMD_READMULTIBLOCK 18
#define CMD_STOPTRANSMISSION 12

.global mmcSkipBytes
.global mmcGetLong
.global mmcGetInt
.global mmcGetChar

.global mmc_get_byte
.global spi_byte
.global spibyte_ff
.global mmc_get
.global mmc_datatoken
.global mmc_clock_and_release
.global mmc_send_command_no_address
.global mmc_send_command
.global mmc_init_no_buffer
.global mmc_cuesector
.global mmc_stoptransmission


.section .bss
	byte_count:			.word 1

.section .text

.global mmcSkipBytes
.global mmcGetLong
.global mmcGetInt
.global mmcGetChar

.section .text.mmcSkipBytes
mmcSkipBytes:
	movw	r22, r24

mmcSkipBytesLoop:
	rcall	mmc_get_byte
	subi	r22, 1
	sbci	r23, 0
	brne	mmcSkipBytesLoop
	ret

.section .text.mmcGetLong
mmcGetLong:
	rcall	mmc_get_byte		; First byte from SD card straight to R22
    mov		r22, r24
	rcall	mmc_get_byte		; Second byte from SD card straight to R23
    mov		r23, r24
								; Fall through to GetInt to receive 3rd and 4th bytes
.section .text.mmcGetInt
mmcGetInt:
	rcall	mmc_get_byte		; First byte from SD card to temp location in R20 (3rd byte of GetLong)
    mov		r20, r24
    rcall	mmc_get_byte		; Second byte from SD card to temp location in R21 (4th byte of GetLong)
    mov		r21, r24
    movw	r24, r20			; Move R20:21 to the R24:25 location C is expecting it
    ret

.section .text.mmcGetChar		; Simple fall through to get_byte
mmcGetChar:

.section .text.mmc_get_byte
mmc_get_byte:
	rcall	spibyte_ff

	lds		r31, byte_count+0
	lds		r30, byte_count+1
	adiw	r30, 1
	sts		byte_count+1, r30

	cpi		r31, 0x02
	breq	hit_512_boundary

	sts		byte_count+0, r31

	ret

hit_512_boundary:
	sts		byte_count+0, r30

	push	r24
	rcall	mmc_datatoken
	pop		r24
	ret


; u8 spibyte(u8)
; -------------------
; waits for SPIF bit of SPSR to be clear before returning
; C callable
; in r24: byte to send
; returns: byte read
.section .text.spibyte_ff
spibyte_ff:
	ldi r24,0xff

.section .text.spi_byte
spi_byte:
    out 	_SFR_IO_ADDR(SPDR),r24
spi_byte_wait:
    in		r24,_SFR_IO_ADDR(SPSR)
	sbrs	r24,SPIF
    rjmp	spi_byte_wait
    in		r24,_SFR_IO_ADDR(SPDR)
    ret


; u8 mmc_get(void)
;-------------------
; C callable
; in: void
; returns: byte read
.section .text.mmc_get
mmc_get:
	ser r30
	ser r31
mmc_get_wait:
	sbiw r30,1
	breq mmc_get_end
    rcall spibyte_ff
    cpi r24,0xff
    breq mmc_get_wait
mmc_get_end:    
	ret


; byte mmc_datatoken(void)
;-----------------------
; C callable
; in: void 
; returns: token
.section .text.mmc_datatoken
mmc_datatoken:   
	ser r30
	ser r31
mmc_datatoken_loop: 
	sbiw r30,1
	breq mmc_datatoken_end
    rcall spibyte_ff
    cpi r24,0xfe
	brne mmc_datatoken_loop
mmc_datatoken_end:
    ret
    

; void mmc_clock_and_release(void)
;---------------------------------
; generates 80 clocks on the SPI device
; C callable
;
.section .text.mmc_clock_and_release
mmc_clock_and_release:

    ldi r25,10
clock_loop:
    rcall spibyte_ff              ; 80 clocks for power stabilization
    dec r25
    brne clock_loop
	
	;release SPI
    in r24,_SFR_IO_ADDR(MMC_CS_PORT)
    ori r24,(1 << MMC_CS)
    out _SFR_IO_ADDR(MMC_CS_PORT),r24
    ret

.section .text.mmc_send_command
mmc_send_command_no_address:

	ldi		r20, 0
	ldi		r21, 0
	movw	r22, r20

; void mmc_send_command(uint8_t command, uint16_t px, uint16_t py)
; mmccommand(cmd:20,highx:21,lowx:22,highy:23,lowy:24)
; ----------------------
; C callable 
; r24 = command
; r23:r22 = px
; r21:r20 = py
mmc_send_command:
    in r25,_SFR_IO_ADDR(MMC_CS_PORT)
    andi r25,~(1 << MMC_CS)
    out _SFR_IO_ADDR(MMC_CS_PORT),r25  ; enable CS

    mov r25,r24 	; save command
    rcall spibyte_ff ; send dummy byte    

    mov r24,r25  ; restore command
	ori r24,0x40
    rcall spi_byte ; send command 

    mov r24,r23
    rcall spi_byte ; high x    

    mov r24,r22
    rcall spi_byte ; low x
    
	mov r24,r21
    rcall spi_byte ; high y    
    
	mov r24,r20
    rcall spi_byte ; low y
 
    
    ldi r24,0x95 ; correct CRC for first command in SPI 
    rcall spi_byte ; after that CRC is ignored, so no problem with always sending 0x95
       
    rcall spibyte_ff ; ignore return byte
    ret


    
;
; void mmc_init(u8* sector_buffer)
;----------------
;C callable
;r25:r24=pointer to sector buffer (512 bytes)
;

.section .text.mmc_init_no_buffer
mmc_init_no_buffer:
	;setup I/O ports 
    in r24,_SFR_IO_ADDR(SPI_PORT)
    andi r24,~((1 << MMC_SCK) | (1 << MMC_MOSI)) 
    ori r24,(1 << MMC_MISO)
    out _SFR_IO_ADDR(SPI_PORT),r24
    
    in r24,_SFR_IO_ADDR(SPI_DDR)
    ori r24,(1<<MMC_SCK) | (1<<MMC_MOSI)
    out _SFR_IO_ADDR(SPI_DDR),r24
    
	sbi	_SFR_IO_ADDR(MMC_CS_PORT), MMC_CS	;Initial level is high
	sbi	_SFR_IO_ADDR(MMC_CS_DIR), MMC_CS	;Direction is output
	sbi	_SFR_IO_ADDR(SPI_DDR), 0
    
    ldi r24,(1<<MSTR)|(1<<SPE)   			;enable SPI interface
    out _SFR_IO_ADDR(SPCR),r24    
    ldi r24,1
    out _SFR_IO_ADDR(SPSR),r24              ;set double speed	

    
	; 80 clocks for power stabilization
    ldi r25,10
mmcinit_power_loop:
    rcall spibyte_ff              
    dec r25
    brne mmcinit_power_loop

 	;issue card reset
    ldi r24,CMD_RESET
    rcall mmc_send_command_no_address


    rcall mmc_get
    cpi r24,0x01
    breq mmcinit_card_detected
    
	;invalid response code returned
    ;return error code since the card cannot be detected
	rcall mmc_clock_and_release
	ldi r24,1
    ret

mmcinit_card_detected:
    ;send CMD1 until we get a 0 back, indicating card is done initializing 

mmcinit_cmd1_loop:
    rcall spibyte_ff    
    cpi r24,0
    breq mmcinit_cmd1_done
	ldi r24,CMD_INIT
    rcall mmc_send_command_no_address
	rjmp mmcinit_cmd1_loop

mmcinit_cmd1_done:
	rcall mmc_clock_and_release
    ret
  

;
; mmc_cuesector
;------------------------
; C callable
; r25:r24:r23:r22 = LBA sector (32 bit)
; return: byte status
.section .text.mmc_cuesector
mmc_cuesector:
	;Regular SD needs bytes adress
	;shift sector value by 9 bits (*512)

	;shift <<8
	clr r20
	mov r21,r22
	mov r22,r23
	mov r23,r24

	;shift one more bit to finish up the 512 multiplier
	lsl r21
	rol r22
	rol r23

	;call read block command with lba argument in-place
    ldi r24,CMD_READMULTIBLOCK
    rcall mmc_send_command

    rcall mmc_datatoken				; wait for data token
	cpi r24,0xfe
    breq mmc_cuesector_end			; if data token received cue_sector succeded

									; other wise there was an error and we need to
	rcall mmc_clock_and_release		; release the SD card bus
	ldi r24,0xff					; return fail
	ret

mmc_cuesector_end:
    clr		r24						; return success
	sts		byte_count+0, r24		; Reset byte-in-sector counter to zero
	sts		byte_count+1, r24
    ret

.section .text.mmc_stoptransmission
mmc_stoptransmission:
    ldi		r24, CMD_STOPTRANSMISSION
    rcall	mmc_send_command_no_address
	rcall spibyte_ff
	ret
