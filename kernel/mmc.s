#include <avr/io.h>
//#include "defines.h"

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
#define CMD_RESET		0
#define CMD_INIT		1
#define CMD_SEND_CSD		9
#define CMD_STOPTRANSMISSION 	12
#define CMD_READBLOCK		17
#define CMD_READMULTIBLOCK 	18


.global spi_byte
.global mmc_get
.global mmc_datatoken
.global mmc_clock_and_release
.global mmc_send_command
.global mmc_init
.global mmc_readsector
.global sector_buffer_ptr


.section .bss
	sector_buffer_ptr:	.space 2  ;pointer to sector buffer (at least 512 bytes)
	last_sector:		.space 4 ;used for caching

.section .text

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
    out _SFR_IO_ADDR(SPDR),r24
spi_byte_wait:
	nop
    in r24,_SFR_IO_ADDR(SPSR)    
	bst r24,SPIF
    brtc spi_byte_wait
    in r24,_SFR_IO_ADDR(SPDR)
	nop
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
	nop
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

; void mmc_send_command(uint8_t command, uint16_t px, uint16_t py)
; mmccommand(cmd:20,highx:21,lowx:22,highy:23,lowy:24)
; ----------------------
; C callable 
; r24 = command
; r23:r22 = px
; r21:r20 = py
.section .text.mmc_send_command
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
.section .text.mmc_init
mmc_init:
	sts sector_buffer_ptr+0,r25
	sts sector_buffer_ptr+1,r24

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
    ldi r23,0x00
    ldi r22,0x00
    ldi r21,0x00
    ldi r20,0x00
    rcall mmc_send_command


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
    ldi r23,0x00
    ldi r22,0x00
    ldi r21,0x00
    ldi r20,0x00
    rcall mmc_send_command
	rjmp mmcinit_cmd1_loop

mmcinit_cmd1_done:
	rcall mmc_clock_and_release

	ser r24
	sts last_sector+0,r24
	sts last_sector+1,r24
	sts last_sector+2,r24
	sts last_sector+3,r24

	clr r24   
    ret
  
;
; mmc_readsector
;------------------------
; C callable
; r25:r24:r23:r22 = LBA sector (32 bit)
; return: byte status    
.section .text.mmc_readsector
mmc_readsector:

	;requested sector is already loaded?

	lds r18,last_sector+0
	lds r19,last_sector+1
	lds r20,last_sector+2
	lds r21,last_sector+3
	cp 	r18,r22
	cpc r19,r23
	cpc r20,r24
	cpc r21,r25
	breq mmc_readsector_end
	sts last_sector+0,r22
	sts last_sector+1,r23
	sts last_sector+2,r24
	sts last_sector+3,r25

	;Regular SD needs bytes adress
	;shift sector value by 9 bits (*512)
	
	;shift <<8
	clr r20
	mov r21,r22
	mov r22,r23
	mov r23,r24
	
	;shift one more bit to finish up the 512 multiplier
	clc
	rol r21
	rol r22
	rol r23

	;call read block command with lba argument in-place
    ldi r24,CMD_READBLOCK    
    rcall mmc_send_command 

    rcall mmc_datatoken
	cpi r24,0xfe
    breq mmc_readsector_gotdata
	
	;error!
	rcall mmc_clock_and_release
	ldi r24,0xff
	ret

mmc_readsector_gotdata:    
	
	lds XH,sector_buffer_ptr+0
	lds XL,sector_buffer_ptr+1    

    ; read sector data
	ldi r30,lo8(512)
	ldi r31,hi8(512)
mmc_readsector_loop:
    rcall spibyte_ff
    st X+,r24
	sbiw r30,1
	nop
    brne mmc_readsector_loop    
    nop
    ; ignore dummy checksums and clock out the device
    rcall spibyte_ff
    rcall spibyte_ff
	nop
    rcall mmc_clock_and_release

mmc_readsector_end:
    clr r24
    ret

