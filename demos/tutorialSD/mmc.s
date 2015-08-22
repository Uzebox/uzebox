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
.global mmc_cue_sector_exact_position
.global mmc_stoptransmission


.section .bss
	byte_count:			.word 1
	vramaddress:		.word 1

.section .text

.global mmcSkipBytes
.global mmcGetLong
.global mmcGetInt
.global mmcGetChar

.global mmcFindFileFirstSector
.global mmcFindFileFirstSectorFlash
.global mmcDirectRead



; void mmcDirectRead(uint8_t *dest, uint16_t count, uint8_t span, uint8_t run);
;
; Inputs
;     Dest  R24:25 = Address in RAM data will be copied too
;     Count R22:23 = How many bytes to transfer from SD to RAM
;     Span  R20    = How many bytes of destination to skip/span after each "run"
;     Run   R18    = How many bytes to copy in a row before doing a skip/span
;
; Returns
;     Void
;
; Modifies
;     byte_count (the 512 byte counter for keeping track of SD reads)
;     RAM pointed to by *dest
;
; Trashed
;     R0, R19, R21, R22, R23, R24, R25, R26, R27, R30, R31
;
;
; Register Usage
;  r0  = thisRun
;  r1  = zero
;  ...
;  r18 = run
;  r19 = local flags  Bit 0 = SPDR already read
;  r20 = span
;  r21 = 0xFF         Constant to send out SPI port
;  r22 = count(lo)
;  r23 = count(hi)
;  r24 = trash        (starting location for *dest(lo) before move to r26)
;  r25 = byteRead     (starting location for *dest(hi) before move to r27)
;  r26 = *dest(lo)
;  r27 = *dest(hi)
;  r28 =
;  r29 =
;  r30 = 512ByteCounter(lo)
;  r31 = 512ByteCounter(hi)
;
;
; For interest C version took 108 words and blocked on in(SPDR) taking 27..34 clocks per byte
/*
void readSDBytes2(uint8_t *dest, uint16_t count, uint8_t span, uint8_t run)
{
	uint8_t thisRun = run;

	while(count != 0) {
		*dest++ = mmc_get_byte();
		thisRun--;
		if (thisRun == 0) {
			thisRun = run;
			dest += span;
		}
		count--;
	}
}
*/

; Things that have to happen apart from the obvious from the C code above
;
;    For every OUT(SPDR) there must be an IN(SPDR)
;    There must be at minimum 18 clocks between the OUT and the IN
;    For each IN you need to store the byte at the address pointed to by Z
;    You should only update the address of Z with run/span after the first write
;    For every OUT/IN there must be a check to see if 512 bytes have been read
;    IF 512 Bytes have been read you must "Get a Data Token"
;        (note: mmcDataToken has been modified so that it leaves R30:31 as ZERO
;               this is the 512ByteCounter and it should be zero afterwards
;               origionally this was done here, but moving it to mmcDataToken
;               means we could
;
;               rjmp  mmcDataToken  <<< rjmp rather than rcall and steal its RET
;
;               instead of
;
;               rcall mmcDataToken  <<< an extra Call and a return over above version
;               clr   R30
;               clr   R31
;               ret
;
;    For maximum speed you should do the (n+1)th OUT directly after the nth IN
;
;    ie:
;        Preamble:
;            <set up>
;            ...
;        Loop:
;            out
;            <do something>
;            in
;            store
;            <test end condition and exit>
;            rjmp Loop
;        Exit:
;            <clean up>
;            ...
;
;    Can never be as fast as
;
;        Preamble:
;            <set up>
;            out      <<<< first out
;            ...
;        Loop:
;            <test end condition and exit>
;            in       <<<< Zero clocks wasted between IN and OUT
;            out
;            store
;            <do something>
;            rjmp Loop
;        Exit:
;            in       <<<< Last in
;            store    <<<< last store
;            <clean up>
;            ...
;
;    Doing the OUT STRAIGHT after the IN like this presents a problem.
;    Every 512 bytes there has to be "mmc_DataToken" BETWEEN the IN and the OUT
;
;          IN     <<< Get Byte 510
;          OUT    <<< Cue Byte 511
;          STORE  <<< Store byte 510
;          ...
;          IN     <<< Get Byte 511
;          OUT    <<< Cue Byte 512
;          STORE  <<< Store byte 511
;          ...
;          IN     <<< Get Byte 512
;          <GET DATA TOKEN>            <<<<< We have to be able to fit the data token in there
;          OUT    <<< Cue Byte 0
;          STORE  <<< Store byte 512
;          ...
;          IN     <<< Get Byte 0
;          OUT    <<< Cue Byte 1
;          STORE  <<< Store byte 0
;          ...
;          ...
;
;    So we chnage it so the IN can be skipped (based on a flag) and move the test for 512bytes in front of the IN
;
;      Loop:
;          Clear Skip Flag
;          Test 512 Bytes
;          If 512Bytes True then
;              in                  <<<< This IN only happens once every 512 bytes
;              mmc_DataToken
;              Set Skip Flag
;          <something>
;          If Skip Flag Clear
;              in                  <<<< This IN happens 511 out of 512 times
;          Out
;          ...
;          rjmp loop
;
; Something else unorthodox
;
;    add    r26, r20
;    brcc   mmcDirectReadLoop
;    adc    r27, r1
;    rjmp   mmcDirectReadLoop
;
;    is used rather than just
;
;    add    r26, r20
;    adc    r27, r1
;    rjmp   mmcDirectReadLoop
;
;    Even though it takes one more cycle if the 8 bit add overflows and costs one extra word.  It is one clock
;    shorter for the most common case of no overflow.  This lets the standard/common case of run=1 be 18
;    clocks rather than 19 clocks.
;

.section .text.mmcDirectRead                  ;                                                             Clocks between out(SPDR) and in(SPDR) for case
mmcDirectRead:                                ;                                                             First     Normal    First512  Normal512
    ldi    r21, 0xFF                          ;                                                                       Run !run            Run !Run
    out    _SFR_IO_ADDR(SPDR), r21            ; Send out an 0x00 on SPI bus                                  .         .    .    .         .    .
    mov    r0, r18                            ; thisRun = run                                                1         .    .    1         .    .
    movw   r26, r24                           ; move *dest to r25 for use of "ST X+"                         2         .    .    2         .    .
    lds    r31, byte_count+0                  ; Get the 512ByteCounter into Z                                4         .    .    4         .    .
    lds    r30, byte_count+1                  ;                                                              6         .    .    7         .    .
    rjmp   .                                  ; waste 2 clock cycles to make 18 clocks to next in(SPDR)      8         .    .    8         .    .
                                              ;                                                              .         .    .    .         .    .
mmcDirectReadLoop:                            ;                                                              .         .    .    .         .    .
    cbr    r19, (1<<0)                        ; Clear the SPDR already read flag                             9         9    9    9         9    9
    adiw   r30, 1                             ; Inc the 512ByteCounter                                      11        11   11   11        11   11
    sbrc   r31, 1                             ; if 512cnt != 512 then don't                                 13        13   13   12        12   12
    rcall  mmcDirectReadHit512                ; (in(SPDR), GetToken, Set SPDR Read flag, clear R30:31)       .         .    .   15        15   15
                                              ;                                                              .         .    .    .         .    .
    subi   r22, 1                             ; count--                                                     14        14   14    .         .    .
    sbci   r23, 0                             ;                                                             15        15   15    .         .    .
    breq   mmcDirectReadClenup                ; if(count==0) exit the loop                                  16        16   16    .         .    .
                                              ;                                                              .         .    .    .         .    .
    sbrs   r19, 0                             ; if we have already read SPDR (because 512cnt) then skip     17        17   17    .         .    .
    in     r25, _SFR_IO_ADDR(SPDR)            ; read the byte waiting in SPDR after 18 clocks elapsed       18 :)     18   18    .         .    .

    out    _SFR_IO_ADDR(SPDR), r21            ; Send the next 0x00 out the SPI bus                           .         .    .    .         .    .
    st     X+, r25                            ; Save the byte read from SPDR to X                            .         2    2    .         2    2
                                              ;                                                              .         .    .    .         .    .
    dec    r0                                 ; thisRun--                                                    .         3    3    .         3    3
    brne   mmcDirectReadRunEnd                ; if(thisRun != 0) then skip the run/span maths                .         4    5    .         4    5
    mov    r0, r18                            ; otherwise thisRun = run                                      .         5    .    .         5    .
    add    r26, r20                           ;           *dest += span                                      .         6    .    .         6    .
    brcc   mmcDirectReadLoop                  ;                                                              .         8    .    .         8    .
    adc    r27, r1                            ;                          *? becomes +2 every 256th byte      .         *?   .    .         *?   .
    rjmp   mmcDirectReadLoop                  ; start loop again         *? which makes total 20 not 18      .         *?   .    .         *?   .
mmcDirectReadRunEnd:                          ;                                                              .         .    .    .         .    .
    nop                                       ; waste 1 clock                                                .         .    6    .         .    6
    rjmp   mmcDirectReadLoop                  ; start loop again                                             .         .    8    .         .    8

mmcDirectReadClenup:
    sbrs   r19, 0                             ; Check to see if SPDR is already read
    in     r25, _SFR_IO_ADDR(SPDR)            ; if not then read SPDR
    st     X+, r25                            ; Save the byte read from SPDR to X
    sts    byte_count+1, r30                  ; Save the 512ByteCounter back to RAM
    sts    byte_count+0, r31
    ret

mmcDirectReadHit512:                          ;                                                              .         .    .   16        16   16
    nop                                       ; waste 1 clock                                                .         .    .   17        17   17
    sbr    r19, (1<<0)                        ; set the SPDR already read into r25 flag                      .         .    .   18        18   18
    in     r25, _SFR_IO_ADDR(SPDR)            ; in(SPDR)

	rcall  spibyte_ff                         ; ??????? GET CRC AND STUFF ?????  Investigate why SD fails but UZEM works without this implicit
	rcall  spibyte_ff

    rjmp   mmc_datatoken                      ; rjmp not rcall to reuse the RET at end of datatoken
                                              ; note: mmc_datatoken resets 512Cnt in R30:31 back to zero

/*
	mmc_cuesector(0x000);	// Get the MBR

	mmcSkipBytes(offsetof(MBR, partition1)+ offsetof(PartitionEntry, startSector));			// Skip the execCode and a few other bytes

	long bootRecordSector = mmcGetLong();   // sector that the boot record starts at

	mmc_stoptransmission();					// stop reading the MBR
	mmc_cuesector(bootRecordSector);		// and start reading the boot record

	mmcSkipBytes(offsetof(BootRecord, bytesPerSector));

	int  bytesPerSector    = mmcGetInt();
	char sectorsPerCluster = mmcGetChar();
	int  reservedSectors   = mmcGetInt();
	mmcSkipBytes(1);
	int  maxRootDirectoryEntries = mmcGetInt();
	mmcSkipBytes(3);
	int sectorsPerFat = mmcGetInt();

	long dirTableSector = bootRecordSector + reservedSectors + (sectorsPerFat * 2);

	mmc_stoptransmission();
	mmc_cuesector(dirTableSector);

	uint8_t fileFound = 1;

	do {
		if(fileFound == 0) {
			mmcSkipBytes(21);
			fileFound = 1;
		}

		for(uint8_t i = 0; i<11; i++){
			if(mmc_get_byte() != fileName[i]) fileFound = 0;
		}

	} while (fileFound == 0);

	mmcSkipBytes(15);

	int firstCluster = mmcGetInt();

	mmc_stoptransmission();

	return(dirTableSector+((maxRootDirectoryEntries * 32)/bytesPerSector)+((firstCluster-2)*sectorsPerCluster));
*/

.section .text.mmcFindFileFirstSector
mmcFindFileFirstSectorFlash:
	bset	6					; set the T flag in SREG to indicate we are looking at string in FLASH
	rjmp	mmcFindFileFirstSectorCommon
mmcFindFileFirstSector:
	bclr	6					; clear the T flag in SREG to indicate we are looking at string in RAM
mmcFindFileFirstSectorCommon:
	push	r2					; Save R2 as it will be overwritten by SPC
	push	r16					; Save R16:17 as it will be overwritten by MaxFilesToSearchFor
	push	r17
	push	r28					; Save R28:29 as it will be overwritten by the String Index Pointer
	push	r29
	push	r24					; Save the address pointing too the String (could be flash or RAM)
	push	r25


// mmc_cuesector(0x000);
	rcall 	mmc_find_clear_r22_23_24_25
	rcall	mmc_cuesector
// mmcSkipBytes(offsetof(MBR, partition1)+ offsetof(PartitionEntry, startSector));
	ldi		r24, 0xC6
	ldi		r25, 0x01
	rcall	mmcSkipBytes
//long bootRecordSector = mmcGetLong();
	rcall	mmcGetLong
	movw	r26, r24			; Save the high word of the boot sector to r26:27 (which is untouched by StopTransmission)
	movw	r18, r22			; Save the low word to r18:19
//mmc_stoptransmission();
	rcall	mmc_stoptransmission
//mmc_cuesector(bootRecordSector);
	movw	r24, r26
	movw	r22, r18
	rcall	mmc_cuesector
//mmcSkipBytes(offsetof(BootRecord, bytesPerSector));
	ldi		r24, 0x0B
	rcall	mmcSkipBytesMax256
//int  bytesPerSector    = mmcGetInt();
	rcall	mmcGetInt								; We divide BytesPerSector by 32 for a re-arrange of the later maths
	ldi		r23, 0x20								; ((maxRootDirectoryEntries * 32)/bytesPerSector)
	rcall	mmc_find_div_r245_by_r23_result_r0		; The result of this is at most 8 bits (BPS is 128..4096)
//char sectorsPerCluster = mmcGetChar();
	rcall	mmcGetChar
	mov		r2, r24
//int  reservedSectors   = mmcGetInt();
	rcall	mmcGetInt								; Get reserved sectors and add it straight to R12:19:26:26
	rcall	mmc_find_add_r245_to_r189_r267			; which now contains (bootRecordSector + reservedSectors)
//mmcSkipBytes(1);
	ldi		r24, 0x01
	rcall	mmcSkipBytesMax256
//	int  maxRootDirectoryEntries = mmcGetInt();
	rcall	mmcGetInt
	movw	r16, r24								; Make a copy of Max Root Dir Entries for NumFilesToCheck

	mov		r23, r0									; Do the second part of the maths for
	rcall	mmc_find_div_r245_by_r23_result_r0		; ((maxRootDirectoryEntries * 32)/bytesPerSector)
													; leaving the result in R0 (If you try to custom format a disk
													; with 4096 RDE and only 256 BPS this will fail.  All sensible
													; "round" binary values will work
//mmcSkipBytes(3);
	ldi		r24, 0x03
	rcall	mmcSkipBytesMax256
//int sectorsPerFat = mmcGetInt();
	rcall	mmcGetInt								; Sectors per fat then divide by two
	lsl		r24										; and then add to (bootRecordSector + reservedSectors)
	rol		r25
	rcall	mmc_find_add_r245_to_r189_r267
//long dirTableSector = bootRecordSector + reservedSectors + (sectorsPerFat * 2);
//mmc_stoptransmission();
	rcall	mmc_stoptransmission


//mmc_cuesector(dirTableSector);
	movw	r24, r26
	movw	r22, r18
	rcall	mmc_cuesector

	movw	r24, r0									; Now that we have finished "cueing" the DirTable we can continue the maths
	rcall	mmc_find_add_r245_to_r189_r267			; dirTableSector + ((maxRootDirectoryEntries * 32)/bytesPerSector)

	pop		r21								; pop the base address of the 8.3 filename string
	pop		r20

mmc_find_file_for_each_dir_entry_loop:
	movw	r28, r20						; Get the base address of the filename into the index_backup
	ldi		r22, 11							; number of chars to check (11 = 8+3)
	cbr		r23, (1<<0)						; clear the search for file flag
mmc_find_file_text_search_loop:
	rcall	mmcGetChar						; Get the first byte from the SD card to compare

	movw	r30, r28
	brts	mmc_find_file_text_search_loop_not_ram
	ld		r25, Z+							; get the first byte of the search string to compare
mmc_find_file_text_search_loop_not_ram:
	brtc	mmc_find_file_text_search_loop_not_flash
	lpm		r25, Z+
mmc_find_file_text_search_loop_not_flash:

	movw	r28, r30

	cp		r24, r25						; compare the two bytes

	breq	mmc_find_file_text_not_equal	; and if not matched then set dirty flag
	sbr		r23, (1<<0)
mmc_find_file_text_not_equal:
	dec		r22
	brne	mmc_find_file_text_search_loop	; continue for all 11 bytes

	sbrs	r23, 0							; if the dirty flag was not set (ie: all 11 chars matched)
	rjmp	mmc_find_file_text_found		; then we have found the file

	subi	r16, 1							; subtract ONE from NumFilesToCheck
	sbc		r17, r1

	brne	mmc_find_file_keep_searching
	rcall	mmc_find_clear_r22_23_24_25
	rjmp	mmc_find_file_file_not_found

mmc_find_file_keep_searching:
	ldi		r24, 21							; other wise skip fwd 21 bytes to the next directory entry (11 + 21 = 32)
	rcall	mmcSkipBytesMax256

	rjmp	mmc_find_file_for_each_dir_entry_loop	; and continue

mmc_find_file_text_found:

//mmcSkipBytes(15);
	ldi		r24, 15							; Skip FWD to read the address of the first cluster of this file
	rcall	mmcSkipBytesMax256

//int firstCluster = mmcGetInt();
	rcall	mmcGetInt

	subi	r24, 2							; (firstCluster-2)
	sbc		r25, r1

mmc_find_file_mul_by_BPS_loop:					; Do a really dumb multiply by successive adds.
	rcall	mmc_find_add_r245_to_r189_r267		; This smaller code because we already have a function for the add
	dec		r2									; so looping back x many SectorsPerCluster (in r2) manages a 8x16->24
	brne	mmc_find_file_mul_by_BPS_loop		; in only 3 words (even though it could take 4000 cycles)
												; Note: This IS a 24 bit result for any file past the 40 megabyte mark

;mmc_stoptransmission();
	rcall	mmc_stoptransmission


;return(dirTableSector+((maxRootDirectoryEntries * 32)/bytesPerSector)+((firstCluster-2)*sectorsPerCluster));
	movw	r22, r18
	movw	r24, r26

mmc_find_file_file_not_found:
	pop		r29								; restore the registers we used
	pop		r28
	pop		r17
	pop		r16
	pop		r2
	ret

mmc_find_clear_r22_23_24_25:
	ldi		r22, 0x00
	ldi		r23, 0x00
	movw	r24, r22
	ret

mmc_find_div_r245_by_r23_result_r0:					; This is a dumb divide only works for values with 1 high bit
	lsr		r25										; AFAIK this is OK for the FAT maths as MaxRDE can only be 512, 1024, 2048 or 4096
	ror		r24
	lsr		r23
	brcc	mmc_find_div_r245_by_r23_result_r0
	mov		r0, r24
	ret

mmc_find_add_r245_to_r189_r267:
	add		r18, r24
	adc		r19, r25
	adc		r26, r1
	adc		r27, r1
	ret



/*
printasc:
	subi	r24, 55
	movw	r30, r16
	st		z+, r24
	movw	r16, r30
	ret

printhex:
	mov		r25,r24
	swap	r25
	andi	r24, 0x0F
	andi	r25, 0x0F
	movw	r30, r16
	st		z+, r25
	st		z+, r24
	movw	r16, r30
	ret
*/

.section .text.mmcSkipBytes
mmcSkipBytesMax256:
	ldi		r25, 0x00
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
	rcall	mmcGetInt			; First two bytes from SD card and move to R22:23
    movw	r22, r24
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
	ldi		r24, 0xFF
	out 	_SFR_IO_ADDR(SPDR),r24

	lds		r31, byte_count+0
	lds		r30, byte_count+1
	adiw	r30, 1
	sts		byte_count+1, r30
	mov		r24, r31
	andi	r24, 0x01
	sts		byte_count+0, r24

	sbrs	r31, 1
	rjmp	spi_byte_wait

hit_512_boundary:
	rcall	spi_byte_wait

	push	r24
	rcall	mmc_datatoken
	pop		r24
	ret

//todo: read Start reading at a non 512 byte boundry
// read straight to memory (with SPAN)


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
	nop
    in		r24,_SFR_IO_ADDR(SPSR)
	sbrs	r24,SPIF
    rjmp	spi_byte_wait
    in		r24,_SFR_IO_ADDR(SPDR)
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
    //rcall spibyte_ff
	clr r31
	clr	r30
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

	ldi  r24, 0x60
	sts  vramaddress+1, r24
	ldi  r24, 0x0C
	sts  vramaddress+0, r24


	;setup I/O ports 
    in r24,_SFR_IO_ADDR(SPI_PORT)
    andi r24,~((1 << MMC_SCK) | (1 << MMC_MOSI)) 
    ori r24,(1 << MMC_MISO)
    out _SFR_IO_ADDR(SPI_PORT),r24
    
    sbi _SFR_IO_ADDR(SPI_DDR), MMC_SCK
    sbi _SFR_IO_ADDR(SPI_DDR), MMC_MOSI

;    in r24,_SFR_IO_ADDR(SPI_DDR)
;    ori r24,(1<<MMC_SCK) | (1<<MMC_MOSI)
;    out _SFR_IO_ADDR(SPI_DDR),r24

	sbi	_SFR_IO_ADDR(MMC_CS_PORT), MMC_CS	;Initial level is high
	sbi	_SFR_IO_ADDR(MMC_CS_DIR), MMC_CS	;Direction is output
;	sbi	_SFR_IO_ADDR(SPI_DDR), 0
    
    ldi r24,(1<<MSTR)|(1<<SPE)|(1<<SPR1)|(1<<SPR0)   			;enable SPI interface clock div by 128 = ~~200Khz
    out _SFR_IO_ADDR(SPCR),r24    
    
	; 80 clocks for power stabilization
    ldi r25,10
mmcinit_power_loop:
    rcall spibyte_ff              
    dec r25
    brne mmcinit_power_loop



    ldi r24,(1<<MSTR)|(1<<SPE)   			;enable SPI interface clock div cleared for fasted speed
    out _SFR_IO_ADDR(SPCR),r24
    ldi r24,1
    out _SFR_IO_ADDR(SPSR),r24              ;set double speed

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
	rjmp mmc_cue_common

mmc_cue_sector_exact_position:
	movw r20, r22					; shift the address from R22:23:24:25 into R20:21:22:23
	movw r22, r24

mmc_cue_common:
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
	sts		byte_count+0, r1 //r24
	sts		byte_count+1, r1 //r20
    clr		r24						; return success
    ret

.section .text.mmc_stoptransmission
mmc_stoptransmission:
    ldi		r24, CMD_STOPTRANSMISSION
    rcall	mmc_send_command_no_address
	rcall   spibyte_ff
	ret
