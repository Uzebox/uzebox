
#include <avr/io.h>

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

.global mmcinit
.global mmcreadsector

.section .bss

.section .text

;
; byte spibyte(byte)
;
; uses: r2
; r20 = byte to write
; r0 = return value
; waits for SPIF bit of SPSR to be clear before returning
;
; spibyte_ff shortcut writes 0xff out instead of expecting a parameter
;
spibyte_ff:
    ldi r20,0xff
spibyte:
    out _SFR_IO_ADDR(SPDR),r20
spibyte_wait:
    in r2,_SFR_IO_ADDR(SPSR)
    bst r2,SPIF
    brtc spibyte_wait
    in r0,_SFR_IO_ADDR(SPDR)
    ret
    
;
; byte mmcget(r20:inbyte)
;
; used: r17,r2
; r0 = return value
; note: loops forever
; TODO: keep forever looping behavior?
mmcget:
    ldi r17,0xff
mmcget_loop:
    call spibyte_ff
    mov r2,r0
    cp r2,r17
    brne mmcget_loop
    ret
    
;
; void clock()
; generates 80 clocks on the SPI device
;
; used r16
clock:    
    ldi r16,10
clock_loop:
    call spibyte_ff              ; 80 clocks for power stabilization
    dec r16
    brne clock_loop
    ret
    
;
; clockrelease()
;
clockrelease:
    call clock
    in r16,_SFR_IO_ADDR(MMC_CS_PORT)
    ori r16,(1 << MMC_CS)
    out _SFR_IO_ADDR(MMC_CS_PORT),r16
    ret
;
; byte mmcdatatoken(r20:inbyte)
;
; used r17s
; returns - flags set for comparison to 0xfe
; note: loops forever
; TODO: keep forever looping behavior?
mmcdatatoken:
    ldi r17,0xfe
    call mmcget_loop
    cp r0,r17
    ret

mmccommand:
    in r16,_SFR_IO_ADDR(MMC_CS_PORT)
    andi r16,~(1 << MMC_CS)
    out _SFR_IO_ADDR(MMC_CS_PORT),r16  ; enable CS

    mov r16,r20 ; save command
    call spibyte_ff  ; dummy byte

    mov r20,r16
    ori r20,0x40 
    call spibyte ; command

    mov r20,23
    call spibyte ; high x
    
    mov r20,r22
    call spibyte ; low x

    mov r20,25
    call spibyte ; high y
    
    mov r20,r24
    call spibyte ; low y
    
    ldi r20,0x95 ; correct CRC for first command in SPI 
    call spibyte ; after that CRC is ignored, so no problem with always sending 0x95
       
    call spibyte_ff ; ignore return byte
    ret
    
;
; void mmcinit()
;
mmcinit:
    in r16,_SFR_IO_ADDR(SPI_PORT)
    andi r16,~((1 << MMC_SCK) | (1 << MMC_MOSI)) 
    ori r16,(1 << MMC_MISO)
    out _SFR_IO_ADDR(SPI_PORT),r16
    
    in r16,_SFR_IO_ADDR(SPI_DDR)
    ori r16,(1<<MMC_SCK) | (1<<MMC_MOSI)
    out _SFR_IO_ADDR(SPI_DDR),r16 
    
    in r16,_SFR_IO_ADDR(MMC_CS_PORT)      ;TODO: is this needed?
    ori r16,(1 << MMC_CS) ; initial level is high
    out _SFR_IO_ADDR(MMC_CS_PORT),r16
    
    in r16,_SFR_IO_ADDR(MMC_CS_DIR)      ;TODO: is this needed?
    ori r16,(1 << MMC_CS) ; direction is output
    out _SFR_IO_ADDR(MMC_CS_DIR),r16
    
    in r16,_SFR_IO_ADDR(SPI_DDR)
    ori r16,(1<<2)        ; accomodate other boards
    out _SFR_IO_ADDR(SPI_DDR),r16
    
    ldi r16,(1<<MSTR)|(1<<SPE)   ;enable SPI interface
    out _SFR_IO_ADDR(SPCR),r16
    
    ldi r16,1
    out _SFR_IO_ADDR(SPSR),r16                ;set double speed	
    
    ldi r16,10
    ldi r20,0xff
    
mmcinit_power_loop:
    mov r0,r17
    call spibyte              ; 80 clocks for power stabilization
    dec r16
    brne mmcinit_power_loop

    ldi r20,0x00
    ldi r22,0x00
    ldi r23,0x00
    ldi r24,0x00
    ldi r25,0x00
    call mmccommand          ; issue card reset
    
    call spibyte_ff
    ldi r16,0x01
    cp r0,r16
    brne mmcinit_card_detected
    ; invalid response code
    ; return error since the card cannot be detected
    rjmp mmcfail
mmcinit_card_detected:
    ;send CMD1 until we get a 0 back, indicating card is done initializing 
    ; NOTE: loops forever
mmcinit_cmd1:
    call mmcget
    ldi r16,0x00
    cp r0,r16
    breq mmcinit_cmd1_done
    ldi r20,0x01
    ldi r22,0x00
    ldi r23,0x00
    ldi r24,0x00
    ldi r25,0x00
    call mmccommand
    rjmp mmcinit_cmd1
    
mmcinit_cmd1_done:
    call clockrelease
    ldi r16,0x00
    cp r0,r16
    brne mmcinit_done
    rjmp mmcfail
mmcinit_done:
    rjmp mmcpass
    
;
; int mmc_readsector(uint32_t lba, uint8_t *buffer)
;
    
mmcreadsector:
    movw XL,r24  

    // todo: fix LBA shifting crazyness
    ldi r20,17
    ldi r22,0xff
    ldi r23,0xff
    nop
    nop
    nop
    
    call mmcdatatoken
    brne mmcreadsector_gotdata
    rjmp mmcfail

mmcreadsector_gotdata:    
    ; read sector data
    ldi r19,0x02 ; set for 2 loops
mmcreadsector_outer_loop:
    ldi r18,0x00 ; set for 256 reads
mmcreadsector_inner_loop:
    call spibyte_ff
    st X+,r0
    dec r18
    brne mmcreadsector_inner_loop
    dec r19
    brne mmcreadsector_outer_loop    
    
    ; ignore dummy checksums and clock out the device
    call spibyte_ff
    call spibyte_ff
    
    ; --fallthrough--
mmcpass:
    call clockrelease
    clr r1
    ret
    
mmcfail:
    call clockrelease
    ldi r16,0xff
    mov r1,r16
    ret