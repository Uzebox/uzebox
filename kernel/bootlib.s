/*
 *  Bootloader interface library
 *  Copyright (C) 2018 Sandor Zsuga (Jubatian)
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
*/



#include <avr/io.h>


.section .text



/*
** This library allows for interfacing with Version 5.0.06 and later
** bootloaders (the "new" loader with the brown interface). It also supports
** backwards compatibility for SD Card and FAT File I/O to allow for running
** the game using it with the old bootloader or with no bootloader.
**
** (Backwards compatibility doesn't exist for the bootloader request which can
** be used to request programming a different game from the game itself).
*/



/*
** Ports and pins
*/
#define SPI_P   _SFR_IO_ADDR(PORTB)
#define SPI_DDR _SFR_IO_ADDR(DDRB)
#define CS_P    _SFR_IO_ADDR(PORTD)
#define CS_DDR  _SFR_IO_ADDR(DDRD)
#define SD_SCK  7
#define SD_MOSI 5
#define SD_MISO 6
#define SD_CS   6

/*
** SPI registers
*/
#define SPI_CR  _SFR_IO_ADDR(SPCR)
#define SPI_DR  _SFR_IO_ADDR(SPDR)
#define SPI_SR  _SFR_IO_ADDR(SPSR)



/*
** SD card access data structure:
**
** uint8  flags;       SD status flags
**                     bit 0: 1 if SD card is detected and initialized
**                     bit 1: 0 (Comp. mode: Indicates SDSC card)
**                     bit 2: 0 (Comp. mode: Indicates FAT-16 filesystem)
**                     bit 3: Selected file is the FAT16 root directory if set
**                     bit 4: 1 if CRC checking is disabled
** uint8* bufp;        Pointer to 512 byte sector buffer
** uint8  csize;       Cluster size in 512 byte sector units
** uint16 fatp;        Address of FAT in 512 byte sector units
** uint32 datap;       Address of Data in 512 byte sector units
** uint32 rootp;       FAT16: Address of Root directory in 512 byte sectors
**                     FAT32: First cluster of Root directory
** uint32 fclus;       First cluster of currently selected file
** uint32 cclus;       Current cluster of currently selected file
** uint8  csec;        Currect 512 byte sector within current cluster
**
** In compatibility mode (no bootloader) SDHC, FAT32 and fragmentation is not
** supported. The members "fclus" and "cclus" store sector addresses, "csec"
** is not used. All the sector addresses are 24 bits, the high byte is unused
** and ignored.
**
** To support the Uzem emulator, CRC checking can be disabled. This is done by
** checking the result of CMD59 (twice), if it seems like it is not supported,
** CRC won't be enabled.
*/



/*
** Bootloader entry points
*/
.equ	BL_Initialize,           0xF000
.equ	BL_Signature,            0xF002
.equ	BL_Version,              0xF006
.equ	BL_SD_CRC7_Byte,         0xF008
.equ	BL_SD_CRC16_Byte,        0xF00A
.equ	BL_SD_Wait_FF,           0xF00C
.equ	BL_SD_Command,           0xF00E
.equ	BL_SD_Release,           0xF010
.equ	BL_SD_Init,              0xF012
.equ	BL_SD_Read_Sector,       0xF014
.equ	BL_SD_Read_Sector_Rt,    0xF016
.equ	BL_FAT_Init,             0xF01C
.equ	BL_FAT_Get_Sector,       0xF01E
.equ	BL_FAT_Read_Sector,      0xF020
.equ	BL_FAT_Next_Sector,      0xF024
.equ	BL_FAT_Reset_Sector,     0xF026
.equ	BL_FAT_Select_Root,      0xF028
.equ	BL_FAT_Select_Cluster,   0xF02A
.equ	BL_FAT_Get_File_Cluster, 0xF02C
.equ	BL_Bootld_Request,       0xF030



/*
** Internal function to check whether the bootloader is available.
**
** Outputs:
** C: Set if new bootloader is available
** Clobbers:
*/
bootlib_hasloader:

	push  r24
	push  ZL
	push  ZH
	ldi   ZL,      lo8(BL_Signature)
	ldi   ZH,      hi8(BL_Signature)
	lpm   r24,     Z+
	subi  r24,     0xB0
	lpm   r24,     Z+
	sbci  r24,     0x07
	lpm   r24,     Z+
	sbci  r24,     0x10
	lpm   r24,     Z+
	sbci  r24,     0xAD
	brne  bootlib_hasloader_nl
	lpm   r24,     Z+      ; Version high
	cpi   r24,     0x50
	brcs  bootlib_hasloader_nl
	brne  bootlib_hasloader_ok
	lpm   r24,     Z+      ; Version low
	cpi   r24,     0x06
	brcs  bootlib_hasloader_nl
bootlib_hasloader_ok:
	sec
	rjmp  .+2
bootlib_hasloader_nl:
	clc
	pop   ZH
	pop   ZL
	pop   r24
	ret



/*
** Set SPI rate suitable for SD access (slower)
**
** Clobbers:
** ZL
*/
.global SPI_Set_SD
SPI_Set_SD:

	ldi   ZL,      (1 << MSTR) | (1 << SPE) | (1 << SPR0)
	out   SPI_CR,  ZL
	rjmp  SPI_Set_2x



/*
** Set SPI rate to maximum (suitable for SPI RAM if any)
**
** Clobbers:
** ZL
*/
.global SPI_Set_Max
SPI_Set_Max:

	ldi   ZL,      (1 << MSTR) | (1 << SPE)
	out   SPI_CR,  ZL
SPI_Set_2x:
	ldi   ZL,      (1 << SPI2X)
	out   SPI_SR,  ZL
	ret




/*
** Slow (but small: no table) CRC7 calculation for SD Card commands.
**
** Calculation must start with 0x00 CRC value.
**
** The final CRC value for the CRC field must be generated by left shifting
** the result and OR-ing one.
**
** Inputs:
**     r24: CRC value
**     r22: Byte to add to the calculation
** Outputs:
**     r24: Resulting CRC value
** Clobbers (only for no bootloader):
** r22
*/
.global SDC_CRC7_Byte
SDC_CRC7_Byte:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_SD_CRC7_Byte

	lsl   r24
	eor   r24,     r22     ; crcval  ^= byte
	ldi   r22,     0x89    ; CRC7 polynomial
	sbrc  r24,     7
	eor   r24,     r22
	rcall SD_CRC7_Byte_blk
	rcall SD_CRC7_Byte_blk
	rcall SD_CRC7_Byte_blk
	rcall SD_CRC7_Byte_blk
	rcall SD_CRC7_Byte_blk
	rcall SD_CRC7_Byte_blk

SD_CRC7_Byte_blk:
	lsl   r24
	sbrc  r24,     7
	eor   r24,     r22
	ret



/*
** Slow (but small: no table) CRC16 calculation for SD Card commands.
**
** Calculation must start with 0x0000 CRC value.
**
** The final CRC value is to be used as-is, the card has it in Little Endian
** byte order.
**
** Inputs:
** r25:r24: CRC value
**     r22: Byte to add to the calculation
** Outputs:
** r25:r24: Resulting CRC value
** Clobbers (only for no bootloader):
** r22, r23
*/
.global SDC_CRC16_Byte
SDC_CRC16_Byte:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_SD_CRC16_Byte

	eor   r25,     r22     ; crcval ^= (byte << 8)
	ldi   r22,     0x21
	ldi   r23,     0x10    ; CRC16 polynomial (0x1021)
	rcall SD_CRC16_Byte_blk
	rcall SD_CRC16_Byte_blk
	rcall SD_CRC16_Byte_blk
	rcall SD_CRC16_Byte_blk
	rcall SD_CRC16_Byte_blk
	rcall SD_CRC16_Byte_blk
	rcall SD_CRC16_Byte_blk

SD_CRC16_Byte_blk:
	lsl   r24
	rol   r25
	brcc  .+4
	eor   r24,     r22
	eor   r25,     r23
	ret



/*
** Releases the SD card appropriately with a trailing 0xFF byte.
**
** Inputs:
** Outputs:
** Clobbers (only for no bootloader):
** r0
*/
.global SDC_Release
SDC_Release:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_SD_Release

	sbi   CS_P,    SD_CS   ; Chip Select: High
	                       ; Fall through to sdlib_wait_spi_with_FF



/*
** Internal function to send 0xFF byte and wait for completion. Useful for
** reading data.
**
** Clobbers:
** r0
*/
sdlib_wait_spi_with_FF:

	sec
	in    r0,      SPI_SR  ; Make sure SPIF is ready for polling
	sbc   r0,      r0      ; r0 = 0xFF
	out   SPI_DR,  r0      ; Fall through to sdlib_wait_spi



/*
** Internal function to wait for SPI transaction completion.
**
** Clobbers:
** r0
*/
sdlib_wait_spi:

	in    r0,      SPI_SR
	sbrc  r0,      SPIF
	ret
	rjmp  .-8



/*
** Internal function to zero r23:r22:r21:r20.
*/
sdlib_cl_r23_r20:

	ldi   r23,     0
	ldi   r22,     0
	movw  r20,     r22
	ret



/*
** Internal function for SDC_Command: Calculate CRC and wait SPI completion.
*/
sdlib_crc7_byte_wait_spi:

	rcall SDC_CRC7_Byte
	rjmp  sdlib_wait_spi



/*
** Sends SD command and waits for the first response byte (normally R1), which
** it returns. Calculates CRC proper, and supports low SPI speeds. Pulls CS
** low before sending the command, and keeps it that way (low).
**
** Inputs:
**     r24: Command byte
** r23:r22: Argument, high
** r21:r20: Argument, low (together they are a proper C uint32)
** Outputs:
**     r24: First response byte (normally R1)
** Clobbers (only for no bootloader):
** r0, r24, r25
*/
.global SDC_Command
SDC_Command:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_SD_Command

	cbi   CS_P,    SD_CS   ; Chip Select: Low
	andi  r24,     0x3F
	ori   r24,     0x40    ; Form command byte
	rcall sdlib_wait_spi_with_FF
	out   SPI_DR,  r24     ; Send command byte
	push  r22              ; Put aside argument byte 1 to begin CRC calculation
	mov   r22,     r24     ; First byte for CRC7
	ldi   r24,     0       ; Initial CRC7 value
	rcall sdlib_crc7_byte_wait_spi
	out   SPI_DR,  r23     ; Send arg. byte 0
	mov   r22,     r23     ; Argument byte 0 for CRC
	rcall sdlib_crc7_byte_wait_spi
	pop   r22              ; Restore arg. byte 1
	out   SPI_DR,  r22     ; Send arg. byte 1
	rcall sdlib_crc7_byte_wait_spi
	out   SPI_DR,  r21     ; Send arg. byte 2
	mov   r22,     r21     ; Argument byte 2 for CRC
	rcall sdlib_crc7_byte_wait_spi
	out   SPI_DR,  r20     ; Send arg. byte 3
	mov   r22,     r20     ; Argument byte 3 for CRC
	rcall sdlib_crc7_byte_wait_spi
	lsl   r24
	ori   r24,     1       ; Final CRC7 value
	out   SPI_DR,  r24     ; Send CRC
	rcall sdlib_wait_spi
	ldi   r25,     16      ; Wait up to 16 bytes for response
SD_Command_wl:
	rcall sdlib_wait_spi_with_FF
	in    r24,     SPI_DR
	cpi   r24,     0xFF
	brne  SD_Command_ret
	dec   r25
	brne  SD_Command_wl
SD_Command_ret:
	ret



/*
** Waits for the end of a sequence of 0xFF returning the byte breaking the
** sequence. Waits for up to 4096 bytes.
**
** Inputs:
** Outputs:
**     r24: Byte received (0xFF is timed out)
** Clobbers (only for no bootloader):
** r0, r25
*/
.global SDC_Wait_FF
SDC_Wait_FF:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_SD_Wait_FF

	ldi   r24,     0x00
	ldi   r25,     0x10
SD_Wait_FF_l:
	rcall sdlib_wait_spi_with_FF
	in    r0,      SPI_DR
	inc   r0
	brne  SD_Wait_FF_ret
	sbiw  r24,     1
	brne  SD_Wait_FF_l
SD_Wait_FF_ret:
	dec   r0
	mov   r24,     r0
	ret



/*
** Common return blocks
*/
sdlib_ret_okr:
	rcall SDC_Release
sdlib_ret_ok:
	ldi   r24,     0x00
sdlib_ret_fl_c:
	ldi   r25,     0x00
sdlib_ret:
	ret
sdlib_ret_fl_01:
	ldi   r24,     0x01
	rjmp  sdlib_ret_fl_c
sdlib_ret_fl_02r:
	rcall SDC_Release
sdlib_ret_fl_02:
	ldi   r24,     0x02
	rjmp  sdlib_ret_fl_c
sdlib_ret_fl_03r:
	rcall SDC_Release
sdlib_ret_fl_03:
	ldi   r24,     0x03
	rjmp  sdlib_ret_fl_c
sdlib_ret_fl_04r:
	rcall SDC_Release
sdlib_ret_fl_04:
	ldi   r24,     0x04
	rjmp  sdlib_ret_fl_c
sdlib_ret_fl_05:
	ldi   r24,     0x05
	rjmp  sdlib_ret_fl_c



/*
** Detects and initializes SD card. This takes a few dozen milliseconds. It
** populates the SD data structure according to the results, which means
** setting the Initialized, SDHC and CRC flags. Normally you don't ever need
** to call this as it is called by FS_Init().
**
** Inputs:
** r25:r24: Pointer to SD data structure
** Outputs:
**     r24: Zero if initialization succeeded. Otherwise one of the followings:
**          1: CMD0 failed (possibly no card in socket)
**          2: CMD59 failed (couldn't enable CRC checking)
**          3: ACMD41 failed (not possible to initialize, bad card)
**          4: ACMD41 timed out
**          5: CMD58 failed (couldn't query card)
** Clobbers (only for no bootloader):
** r0, r18, r19, r20, r21, r22, r23, r24, r25, X, Z
*/
.global SDC_Init
SDC_Init:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_SD_Init

	; Put aside SD data structure in a pointer and set uninitialized

	movw  XL,      r24
	ldi   ZH,      0x00    ; Collects flags to store
	st    X,       ZH

	; Setup pins

	sbi   SPI_P,   SD_MISO ; MISO is weak pull-up

	sbi   SPI_DDR, SD_SCK  ; SD_SCK is an output
	sbi   SPI_DDR, SD_MOSI ; SD_MOSI is an output

	cbi   SPI_P,   SD_SCK  ; SCK is low
	cbi   SPI_P,   SD_MOSI ; MOSI is low

	sbi   CS_P,    SD_CS   ; Initial Chip Select level is high
	sbi   CS_DDR,  SD_CS   ; Direction is output

	; Enable SPI at 1/128 divider (~250KHz at 32MHz)

	ldi   r25,     (1 << MSTR) | (1 << SPE) | (1 << SPR1) | (1 << SPR0)
	out   SPI_CR,  r25
	ldi   r25,     0
	out   SPI_SR,  r25

	; Try transitioning into Idle state 3 times

	ldi   ZL,      3
SD_Init_CMD0l:

	; Send 80 clocks to get card into native mode.

	ldi   r23,     10      ; 80 clocks as 10 * 8 bits
	rcall sdlib_wait_spi_with_FF
	dec   r23
	brne  .-6              ; 10 bytes

	; Send CMD0 trying to get into Idle state

	ldi   r24,     0       ; CMD0
	rcall sdlib_cl_r23_r20 ; Argument: 0x00000000
	rcall SD_Init_CommRel
	cpi   r24,     0x01    ; Return value (R1 response) should be 0x01 (Idle state)
	breq  SD_Init_CMD0l_e
	dec   ZL
	brne  SD_Init_CMD0l
	rjmp  sdlib_ret_fl_01
SD_Init_CMD0l_e:

	; Enable CRC checking. Try it twice while checking the result (this
	; allows for using emulators which can't calculate CRC or don't even
	; support this SD command, while still safely turning on CRC where it
	; is available).

	ldi   ZL,      0x01    ; Expected R1 response value (Idle)
	rcall SD_Init_CRCEna
	cpse  r24,     ZL
	rcall SD_Init_CRCEna
	cpse  r24,     ZL
	ori   ZH,      0x10    ; Disable CRC checking (both attempts failed)

	; Try transitioning into Ready state 256 times at most (should be
	; between 0.5 - 1 sec at most)

	ldi   ZL,      0
SD_Init_CMD1l:

	; Send CMD1 to leave Idle state

	ldi   r24,     1       ; CMD1
	rcall sdlib_cl_r23_r20 ; Argument: 0x00000000
	rcall SD_Init_CommRel
	cpi   r24,     0x01    ; R1 is Idle?
	breq  SD_Init_CMD1l_m  ; Wait some more
	brcs  SD_Init_CMD1l_e  ; R1 is Ready? (0x00)
	rjmp  sdlib_ret_fl_03
SD_Init_CMD1l_m:
	dec   ZL
	brne  SD_Init_CMD1l
	rjmp  sdlib_ret_fl_04
SD_Init_CMD1l_e:

	; Card initialized, now bump up SPI speed to ~3.5MHz (not max as due
	; to the CRC calculations, it can not be exploited anyway).

	rcall SPI_Set_SD

	; Set flags in SD structure

	ori   ZH,      0x01    ; Card initialized (bit 0) set, SDSC
	st    X,       ZH

	; Done

	rjmp  sdlib_ret_ok     ; Success

SD_Init_CommRel:

	rcall SDC_Command
	rjmp  SDC_Release

SD_Init_CRCEna:

	ldi   r24,     59      ; CMD59
	rcall sdlib_cl_r23_r20
	ldi   r20,     0x01    ; CRC checking ON
	rjmp  SD_Init_CommRel



/*
** Toggles SD CRC checking. By default CRC checking is normally ON.
**
** Inputs:
** r25:r24: Pointer to SD data structure
**     r22: Enable (1) or Disable (0).
** Clobbers (only for no bootloader):
** r0, r19, r20, r21, r22, r23, r24, r25, X
*/
.global SDC_CRC_Enable
SDC_CRC_Enable:

	movw  XL,      r24
	mov   r20,     r22
	cpi   r20,     0
	rcall sdlib_cl_r23_r20 ; Doesn't affect flags
	breq  .+2
	ldi   r20,     0x01    ; CRC checking ON (Enabled)
	rcall SDC_CRC_Enable_Cmd
	breq  SD_CRC_Enable_ok
	rcall SDC_CRC_Enable_Cmd
	breq  SD_CRC_Enable_ok
	ldi   r20,     0x00    ; Assume CRC checking becoming disabled
SD_CRC_Enable_ok:
	ld    r21,     X
	ori   r21,     0x10    ; CRC disabled
	cpse  r20,     r22     ; r22: 0, if equal, CRC is really disabled
	andi  r21,     0xEF    ; CRC enabled
	st    X,       r21
	ret

SDC_CRC_Enable_Cmd:
	push  XL
	push  XH
	push  r20
	ldi   r24,     59      ; CMD59
	rcall SDC_Command
	push  r24
	rcall SDC_Release
	pop   r24
	rcall sdlib_cl_r23_r20
	pop   r20
	pop   XH
	pop   XL
	cpi   r24,     0x00    ; Response is Idle?
	ret



/*
** Convert sector address in r23:r22:r21:r20 for SDSC card
*/
sdlib_convsec_sc:

	mov   r23,     r22
	mov   r22,     r21
	mov   r21,     r20
	ldi   r20,     0x00
	lsl   r21
	rol   r22
	rol   r23              ; SDSC cards use byte address
	ret



/*
** Converts sector address for SD card type. This can be used to generate an
** address parameter for SDC_Command.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** r23:r22: 512b sector address, high
** r21:r20: 512b sector address, low (together they are a proper C uint32)
** Outputs:
** r25:r24: Command address high
** r23:r22: Command address low
** Clobbers (only for no bootloader):
** r20, r21, X
*/
.global SDC_Command_Address
SDC_Command_Address:

	movw  XL,      r24
	ld    r25,     X
	sbrs  r25,     1       ; SDHC card: Sector address as-is.
	rcall sdlib_convsec_sc ; SDSC cards use byte address
	movw  r24,     r22
	movw  r22,     r20
	ret



/*
** Performs a single sector read with a retry when the read fails.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** r23:r22: 512b sector address, high
** r21:r20: 512b sector address, low (together they are a proper C uint32)
** Outputs:
**     r24: Zero if operation succeeded. Otherwise one of the followings:
**          1: Card is not initialized
**          2: CMD17 failed
**          3: Timed out during waiting for data token
**          4: CRC error (data is loaded, but possibly corrupt)
** Clobbers (only for no bootloader):
** r0, r18, r19, r20, r21, r22, r23, r24, r25, X, Z, T(SREG)
*/
.global SDC_Read_Sector
SDC_Read_Sector:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_SD_Read_Sector_Rt

	movw  r18,     r20
	movw  XL,      r22
	push  r24
	push  r25
	rcall SD_Read_Sector_Nr
	pop   r25
	pop   r0
	movw  r22,     XL
	movw  r20,     r18
	cpi   r24,     0x00
	brne  .+2
	ret                    ; Read successful
	mov   r24,     r0      ; Fall through to SD_Read_Sector_Nr



/*
** Performs a single sector read (this is internal in this library even
** though the bootloader exposed it as API, no retry).
**
** Inputs:
** r25:r24: Pointer to SD data structure
** r23:r22: 512b sector address, high
** r21:r20: 512b sector address, low (together they are a proper C uint32)
** Outputs:
**     r24: Zero if operation succeeded. Otherwise one of the followings:
**          1: Card is not initialized
**          2: CMD17 failed
**          3: Timed out during waiting for data token
**          4: CRC error (data is loaded, but possibly corrupt)
** Clobbers (only for no bootloader):
** r0, r20, r21, r22, r23, r24, r25, Z, T(SREG)
*/
SD_Read_Sector_Nr:

	; Load parameters

	movw  ZL,      r24
	ld    r25,     Z       ; Flags
	bst   r25,     4       ; CRC checking state into T
	rcall sdlib_get_secbuf_Z

	; Transform sector address to byte address (SDSC)

	rcall sdlib_convsec_sc ; SDSC cards use byte address

	; Prepare SD Read block command (CMD17)

	ldi   r24,     17      ; CMD17, parameter is OK in r23:r22:r21:r20
	rcall SDC_Command
	cpi   r24,     0x00    ; R1 is Ready?
	breq  .+2
	rjmp  sdlib_ret_fl_02r

	; Wait for Data Token & Check

	rcall SDC_Wait_FF
	cpi   r24,     0xFE    ; Data token OK
	breq  .+2
	rjmp  sdlib_ret_fl_03r

	; Data is ready to be read. Do it along with CRC calculation

	ldi   r24,     0x00
	ldi   r25,     0x00    ; CRC value begin
	rcall sdlib_wait_spi_with_FF
	movw  r20,     ZL
	subi  r21,     0xFE    ; End of target (+ 512 bytes); carry set
SD_Read_Sector_l:
	in    r22,     SPI_DR
	sbc   r0,      r0
	out   SPI_DR,  r0
	st    Z+,      r22     ; Store byte
	rcall SDC_CRC16_Byte
	rcall sdlib_wait_spi
	cp    ZL,      r20
	cpc   ZH,      r21
	brcs  SD_Read_Sector_l

	; 512 bytes in and calculated CRC upon, read the CRC

	in    r21,     SPI_DR
	rcall sdlib_wait_spi_with_FF
	in    r20,     SPI_DR
	sub   r24,     r20
	sbc   r25,     r21
	breq  .+4              ; CRC OK
	brts  .+2              ; CRC Disabled
	rjmp  sdlib_ret_fl_04r

	; Done, correct read

	rjmp  sdlib_ret_okr    ; Success



/*
** Performs a single sector write with a retry when the write fails. Note
** that this may take a long time!
**
** Inputs:
** r25:r24: Pointer to SD data structure
** r23:r22: 512b sector address, high
** r21:r20: 512b sector address, low (together they are a proper C uint32)
** Outputs:
**     r24: Zero if operation succeeded. Otherwise one of the followings:
**          1: Card is not initialized
**          2: CMD24 failed
**          3: Timed out during waiting (card should be reinitialized)
**          4: CRC error (data is rejected by card)
** Clobbers (only for no bootloader):
** r0, r18, r19, r20, r21, r22, r23, r24, r25, X, Z, T(SREG)
*/
.global SDC_Write_Sector
SDC_Write_Sector:

	movw  r18,     r20
	movw  XL,      r22
	push  r24
	push  r25
	rcall SD_Write_Sector_Nr
	pop   r25
	pop   r0
	movw  r22,     XL
	movw  r20,     r18
	cpi   r24,     0x00
	brne  .+2
	ret                    ; Write successful
	mov   r24,     r0      ; Fall through to SD_Write_Sector_Nr



/*
** Performs a single sector write (no retry). Note that this may take a long
** time!
**
** Inputs:
** r25:r24: Pointer to SD data structure
** r23:r22: 512b sector address, high
** r21:r20: 512b sector address, low (together they are a proper C uint32)
** Outputs:
**     r24: Zero if operation succeeded. Otherwise one of the followings:
**          1: Card is not initialized
**          2: CMD24 failed
**          3: Timed out during waiting (card should be reinitialized)
**          4: CRC error (data is rejected by card)
** Clobbers (only for no bootloader):
** r0, r20, r21, r22, r23, r24, r25, Z, T(SREG)
*/
SD_Write_Sector_Nr:

	; Load parameters

	movw  ZL,      r24
	ld    r25,     Z       ; Flags
	bst   r25,     4       ; CRC checking state into T
	rcall sdlib_get_secbuf_Z

	; Transform sector address to byte address (SDSC)

	sbrs  r25,     1       ; SDHC card: Sector address as-is.
	rcall sdlib_convsec_sc ; SDSC cards use byte address

	; Prepare SD Write block command (CMD24)

	ldi   r24,     24      ; CMD24, parameter is OK in r23:r22:r21:r20
	rcall SDC_Command
	cpi   r24,     0x00    ; R1 is Ready?
	breq  .+2
	rjmp  sdlib_ret_fl_02r

	; Prepare data packet

	rcall sdlib_wait_spi_with_FF
	ldi   r24,     0xFE    ; Data token
	out   SPI_DR,  r24

	; Data is ready to be written. Do it along with CRC calculation

	ldi   r24,     0x00
	ldi   r25,     0x00    ; CRC value begin
	movw  r20,     ZL
	subi  r21,     0xFE    ; End of target (+ 512 bytes); carry set
SD_Write_Sector_l:
	ld    r22,     Z+
	rcall sdlib_wait_spi
	out   SPI_DR,  r22
	rcall SDC_CRC16_Byte
	cp    ZL,      r20
	cpc   ZH,      r21
	brcs  SD_Write_Sector_l

	; 512 bytes out and calculated CRC upon, send the CRC

	rcall sdlib_wait_spi
	out   SPI_DR,  r25
	rcall sdlib_wait_spi
	out   SPI_DR,  r24

	; Check response

	rcall sdlib_wait_spi
	rcall sdlib_wait_spi_with_FF
	in    r22,     SPI_DR
	andi  r22,     0x1F
	cpi   r22,     0x05
	breq  .+2              ; Data accepted
	rjmp  sdlib_ret_fl_04r

	; Wait as long as card is busy

	ldi   r23,     0x00
	ldi   r24,     0x00
	ldi   r25,     0x10
SD_Write_Sector_w:
	rcall sdlib_wait_spi_with_FF
	in    r22,     SPI_DR
	subi  r23,     1
	sbci  r24,     0
	sbci  r25,     0       ; Timed out?
	brne  .+2
	rjmp  sdlib_ret_fl_03r
	cpi   r22,     0xFF
	brne  SD_Write_Sector_w

	; Done, correct write

	rjmp  sdlib_ret_okr    ; Success



/*
** Internal function to fetch sector buffer from SD data structure
**
** Inputs:
** r11:r10: SD data structure
** Outputs:
** ZH: ZL:  Sector buffer address
** Clobbers:
** r0
*/
sdlib_get_secbuf:
	movw  ZL,      r10
sdlib_get_secbuf_Z:
	ldd   r0,      Z + 1
	ldd   ZH,      Z + 2
	mov   ZL,      r0
	ret



/*
** Internal function to check whether a value is a power of 2
**
** Inputs:
**     r24: Value to check
** Outputs:
**     C:   Set if it is not a power of 2, clear otherwise
** Clobbers:
** r0, r24
*/
sdlib_isnotpow2:
	mov   r0,      r24
	subi  r24,     1       ; C set if zero (indicating not a power of 2)
	and   r24,     r0
	breq  .+2              ; val & (val - 1) must be zero to be a power of 2
sdlib_isfatboot_ok:
	sec
	ret



/*
** Internal function to check whether a sector is a valid FAT boot sector
**
** Inputs:
** r11:r10: Pointer to SD data structure
** Outputs:
**     C:   Set if it could be a valid FAT boot sector
**     r21: Bytes / sector, high (low is zero)
**     r20: Sectors / cluster
** r23:r22: Reserved sectors
**  ZH: ZL: Sector buffer
** Clobbers:
** r0, r24
*/
sdlib_isfatboot:
	rcall sdlib_get_secbuf ; ZH:ZL: SD data structure -> sector buffer
	ldd   r20,     Z + 0x0B
	cpi   r20,     0x00    ; Sector size, low: must be 0
	brne  sdlib_isfatboot_bad
	ldd   r20,     Z + 0x10
	cpi   r20,     0x02    ; Number of FATs: must be 2
	brne  sdlib_isfatboot_bad
	ldd   r21,     Z + 0x0C
	ldd   r20,     Z + 0x0D
	ldd   r22,     Z + 0x0E
	ldd   r23,     Z + 0x0F
	mov   r24,     r21
	rcall sdlib_isnotpow2  ; Sector size, high: must be a power of 2
	brcs  sdlib_isfatboot_bad
	mov   r24,     r20
	rcall sdlib_isnotpow2  ; Sectors / cluster: must be a power of 2
	brcs  sdlib_isfatboot_bad
	cpi   r23,     0x00
	brne  sdlib_isfatboot_ok
	cpi   r22,     0x00    ; Reserved sectors: must not be 0 (16 bits)
	brne  sdlib_isfatboot_ok
sdlib_isfatboot_bad:
	clc
	ret



/*
** Detects and initializes SD card and FAT filesystem over it. This takes a
** few dozen milliseconds. It populates the SD data structure according to the
** results, which means setting the Filesystem type flag in addition to SD
** init.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** Outputs:
**     r24: Zero if initialization succeeded. Otherwise one of the followings:
**          1: SD Init: CMD0 failed (possibly no card in socket)
**          2: SD Init: CMD59 failed (couldn't enable CRC checking)
**          3: SD Init: ACMD41 failed (not possible to initialize, bad card)
**          4: SD Init: ACMD41 timed out
**          5: SD Init: CMD58 failed (couldn't query card)
**          6: SD read fault
**          7: No usable FAT filesystem found
** Clobbers (only for no bootloader):
** r0, r1 (zero), r18, r19, r20, r21, r22, r23, r24, r25, X, Z
*/
.global FS_Init
FS_Init:

	rcall bootlib_hasloader
	brcc  .+6
	call  BL_FAT_Init
	rjmp  SPI_Set_Max

	push  r10
	push  r11
	movw  r10,     r24

	; Init SD card (try twice)

	rcall SDC_Init
	cpi   r24,     0x00
	breq  FAT_Init_cont
	rcall SDC_Init
	cpi   r24,     0x00
	breq  FAT_Init_cont
	rjmp  FAT_Init_ret     ; SD init failed, return failure code

	; Return blocks

FAT_Init_ret_succ:
	rcall SPI_Set_Max
	clr   r24
	pop   YH
	pop   YL
FAT_Init_ret:
	pop   r11
	pop   r10
	clr   r1
	clr   r25
	ret

FAT_Init_ret_6:
	ldi   r24,     0x06
	rjmp  FAT_Init_ret

FAT_Init_ret_7:
	ldi   r24,     0x07
	rjmp  FAT_Init_ret

FAT_Init_cont:

	; Read sector 0

	movw  r24,     r10
	ldi   r20,     0
	ldi   r21,     0
	movw  r22,     r20
	rcall SDC_Read_Sector
	ldi   XL,      0
	ldi   XH,      0
	cpi   r24,     0x00
	brne  FAT_Init_ret_6   ; Read failed

	; Check whether it is a FAT boot sector

	rcall sdlib_isfatboot
	brcs  FAT_Init_bootok

	; It was likely an MBR. Load partition 0 start

	movw  r24,     r10
	subi  ZL,      0x42
	sbci  ZH,      0xFE    ; Add 0x01B2 (address of Partition 0)
	ldd   r20,     Z + 8
	ldd   r21,     Z + 9   ; Get Partition 0 LBA (assume it is on the beginning somewhere)
	ldi   r22,     0
	ldi   r23,     0
	push  r20
	push  r21
	rcall SDC_Read_Sector
	pop   XH
	pop   XL
	cpi   r24,     0x00
	brne  FAT_Init_ret_6   ; Read failed

	; Check whether it is a FAT boot sector

	rcall sdlib_isfatboot
	brcc  FAT_Init_ret_7   ; No usable FAT filesystem

FAT_Init_bootok:

	; Assume a valid FAT boot sector is found. Get filesystem parameters.
	; The followings are already loaded:
	;     r21: Bytes / sector, high (low is zero)
	;     r20: Sectors / cluster
	; r23:r22: Reserved sectors
	; XH: XL:  Base 512 byte sector

	push  YL
	push  YH
	movw  YL,      r10

	mul   r20,     r21
	lsr   r1
	ror   r0
	std   Y + 3,   r0      ; Cluster size in 512 byte sector units
	mul   r22,     r21
	movw  r24,     r0
	mul   r23,     r21
	add   r25,     r0
	lsr   r25
	ror   r24
	add   XL,      r24
	adc   XH,      r25
	std   Y + 4,   XL
	std   Y + 5,   XH      ; FAT0 address in 512 byte sector units

	; Get FAT size, also switch to FAT32 if it is 0 (indicating this
	; filesystem). FAT0 address in XH:XL is preserved for further relative
	; offsets & FAT32 root directory address (in FAT0).

	ldd   r18,     Z + 0x16
	ldd   r19,     Z + 0x17
	cpi   r18,     0x00
	brne  FAT_Init_no32
	cpi   r19,     0x00
	brne  FAT_Init_no32

	rjmp  FAT_Init_ret_7   ; FAT32 not supported

FAT_Init_no32:

	mul   r18,     r21     ; Calculate FAT size in 512 byte sectors
	movw  r24,     r0
	mul   r19,     r21
	add   r25,     r0
	ldi   r18,     0x00
	adc   r18,     r1      ; r18:25:r24: 2x FAT size in 512 byte sectors
	clr   r1
	add   r24,     XL
	adc   r25,     XH
	adc   r18,     r1      ; Root directory (FAT16) address

	; FAT16 filesystem

	std   Y + 10,  r24
	std   Y + 11,  r25
	std   Y + 12,  r18     ; Root dir. address in 512 byte sectors
	std   Y + 13,  r1

	; Get root directory size

	ldd   r20,     Z + 0x11
	ldd   r21,     Z + 0x12

	; Round up to sector boundary and get data start

	subi  r20,     0xF1
	sbci  r21,     0xFF    ; Add 0x000F
	lsr   r21
	ror   r20
	lsr   r21
	ror   r20
	lsr   r21
	ror   r20
	lsr   r21
	ror   r20              ; Divide by 16 (32b units => 512b units)
	add   r24,     r20
	adc   r25,     r21
	adc   r18,     r1      ; (r1 is zero)
	std   Y + 6,   r24
	std   Y + 7,   r25
	std   Y + 8,   r18     ; Data address in 512 byte sectors
	std   Y + 9,   r1

	; Done, necessary FAT data is all loaded.

	rjmp  FAT_Init_ret_succ



/*
** Returns currently selected sector of file.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** Outputs:
** r25:r24: Current sector, high
** r23:r22: Current sector, low
** Clobbers (only for no bootloader):
** r0, r1 (zero), r18, r19, r20, r21, Z
*/
.global FS_Get_Sector
FS_Get_Sector:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_FAT_Get_Sector

	movw  ZL,      r24
	ldd   r22,     Z + 18  ; Current sector of file / root dir.
	ldd   r23,     Z + 19
	ldd   r24,     Z + 20
	ldi   r25,     0
	ret



/*
** Internal function to set SPI speed to SD and query bootloader
*/
sdlib_set_sd_hasloader:

	rcall SPI_Set_SD
	rjmp  bootlib_hasloader



/*
** Loads currently selected sector of file into sector buffer
**
** Inputs:
** r25:r24: Pointer to SD data structure
** Outputs:
**     r24: SD read errors (SDC_Read_Sector)
** Clobbers (only for no bootloader):
** r0, r1 (zero), r18, r19, r20, r21, r22, r23, r24, r25, X, Z
*/
.global FS_Read_Sector
FS_Read_Sector:

	rcall sdlib_set_sd_hasloader
	brcc  .+6
	call  BL_FAT_Read_Sector
	rjmp  SPI_Set_Max

	rcall FS_Get_Sector
	movw  r20,     r22
	movw  r22,     r24
	movw  r24,     ZL
	rcall SDC_Read_Sector
	rjmp  SPI_Set_Max



/*
** Saves sector buffer into currently selected sector of file
**
** Inputs:
** r25:r24: Pointer to SD data structure
** Outputs:
**     r24: SD write errors (SDC_Write_Sector)
** Clobbers (only for no bootloader):
** r0, r1 (zero), r18, r19, r20, r21, r22, r23, r24, r25, X, Z
*/
.global FS_Write_Sector
FS_Write_Sector:

	rcall SPI_Set_SD
	rcall FS_Get_Sector
	movw  r20,     r22
	movw  r22,     r24
	movw  r24,     ZL
	rcall SDC_Write_Sector
	rjmp  SPI_Set_Max



/*
** Internal function to read Data start in r23:r22:r21:r20
*/
sdlib_get_daddr:

	ldd   r20,     Z + 6   ; Data start 512 byte sector (it is after Root dir)
	ldd   r21,     Z + 7
	ldd   r22,     Z + 8
	ldi   r23,     0
	ret

/*
** Internal function to store Current sector from (r25):r24:r19:r18
*/
sdlib_set_csec:

	std   Z + 18,  r18
	std   Z + 19,  r19
	std   Z + 20,  r24     ; Current sector / cluster stored
	ret



/*
** Moves sector pointer forward (bootloader supports fragmentation).
**
** Inputs:
** r25:r24: Pointer to SD data structure
** Outputs:
**     r24: Zero on success. Otherwise:
**          1: End of file or other error.
** Clobbers (only for no bootloader):
** r0, r1 (zero), r18, r19, r20, r21, r22, r23, r24, r25, X, Z
*/
.global FS_Next_Sector
FS_Next_Sector:

	rcall sdlib_set_sd_hasloader
	brcc  .+6
	call  BL_FAT_Next_Sector
	rjmp  SPI_Set_Max

	rcall FS_Get_Sector
	movw  r18,     r22
	subi  r18,     0xFF    ; Increment
	sbci  r19,     0xFF
	sbci  r24,     0xFF
	rcall sdlib_get_daddr  ; Data start 512 byte sector (it is after Root dir)
	cp    r18,     r20
	cpc   r19,     r21
	cpc   r24,     r22
	ldd   r0,      Z + 0   ; If FAT16 root dir. check end.
	sbrc  r0,      3
	brcc  FAT_Next_Sector_eof
	rcall sdlib_set_csec   ; Next sector / cluster stored

FAT_Next_Sector_ok:

	ldi   r24,     0x00

FAT_Next_Sector_ret:

	ldi   r25,     0x00
	rjmp  SPI_Set_Max

FAT_Next_Sector_eof:

	ldi   r24,     0x01
	rjmp  FAT_Next_Sector_ret



/*
** Reset sector pointer.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** Clobbers (only for no bootloader):
** r0, r18, r19, r24, r25, Z
*/
.global FS_Reset_Sector
FS_Reset_Sector:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_FAT_Reset_Sector

	movw  ZL,      r24

	ldd   r18,     Z + 14
	ldd   r19,     Z + 15
	ldd   r24,     Z + 16
	rjmp  sdlib_set_csec   ; Copy first sector of file into current



/*
** Selects root directory for reading.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** Clobbers (only for no bootloader):
** r0, r20, r21, r22, r23, Z
*/
.global FS_Select_Root
FS_Select_Root:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_FAT_Select_Root

	movw  ZL,      r24

	ldd   r23,     Z + 0
	ori   r23,     0x08    ; FAT16 root directory will be read
	std   Z + 0,   r23
	ldd   r20,     Z + 10  ; Root directory sector
	ldd   r21,     Z + 11
	ldd   r22,     Z + 12
	rjmp  FAT_Select_Root_tail



/*
** Selects a start cluster for reading.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** r23:r22: Start cluster, high
** r21:r20: Start cluster, low
** Clobbers (only for no bootloader):
** r0, r1 (zero), r18, r19, r20, r21, r22, r23, r24, r25, XH, Z
*/
.global FS_Select_Cluster
FS_Select_Cluster:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_FAT_Select_Cluster

	; Convert to sector address. FAT16 has 16 bit clusters, so shortcut
	; by not doing 32 bit calculations here, assuming r23:r22 being zero.

	movw  ZL,      r24

	subi  r20,     2       ; Cluster base is 2 (maps to address 0 in data)
	sbci  r21,     0
	ldd   XH,      Z + 3   ; Cluster size in 512 byte sectors

	mul   r20,     XH
	movw  r18,     r0
	ldi   r24,     0
	mul   r21,     XH
	add   r19,     r0
	adc   r24,     r1      ; Sector address within Data in r24:r19:r18
	clr   r1

	rcall sdlib_get_daddr
	add   r20,     r18
	adc   r21,     r19
	adc   r22,     r24     ; Absolute sector address by adding Data address

	ldd   r18,     Z + 0
	andi  r18,     0xF7    ; Normal read (not a FAT16 root dir.)
	std   Z + 0,   r18

	movw  r24,     ZL

FAT_Select_Root_tail:

	std   Z + 14,  r20
	std   Z + 15,  r21
	std   Z + 16,  r22
	rjmp  FS_Reset_Sector



/*
** Returns a file's start cluster by file descriptor. Returns zero if the
** input region is not a valid file descriptor.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** r23:r22: Pointer to 32 byte FAT file descriptor
** Outputs:
** r25:r24: Start cluster, high
** r23:r22: Start cluster, low
** Clobbers (only for no bootloader):
** r0, Z
*/
.global FS_Get_File_Cluster
FS_Get_File_Cluster:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_FAT_Get_File_Cluster

	movw  ZL,      r22

	ldd   r22,     Z + 0x1A
	ldd   r23,     Z + 0x1B
	ldi   r24,     0x00    ; FAT16 (0): Only 16 bit start cluster
	ldi   r25,     0x00
	ret



/*
** Finds a file and returns its start cluster. Returns zero if the file is not
** found.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** r23:r22: File name, chars 0-1
** r21:r20: File name, chars 2-3
** r19:r18: File name, chars 4-5
** r17:r16: File name, chars 6-7
** r15:r14: Extension, chars 0-1
** r13:r12: Extension, char 2; r12 is unused.
** Outputs:
** r25:r24: Start cluster, high
** r23:r22: Start cluster, low
** Clobbers:
** r0, r1 (zero), r18, r19, r20, r21, r22, r23, r24, r25, X, Z, T(SREG)
*/
.global FS_Find
FS_Find:

	push  r2
	push  r3
	push  r4
	push  r5
	push  r6
	push  r7
	push  r8
	push  r9
	push  r10
	push  r11
	push  r12
	movw  r8,      r22     ; r9:r8: chars 0-1
	movw  r6,      r20     ; r7:r6: chars 2-3
	movw  r4,      r18     ; r5:r4: chars 4-5
	movw  r2,      r16     ; r3:r2: chars 6-7
	movw  r10,     r24     ; r11:r10: SD data structure pointer

	rcall FS_Select_Root

	ldi   r16,     0xFF
	ldi   r17,     0xFF    ; Current directory entry
	clr   r12              ; First sector will be loaded

FAT_Find_loop:

	subi  r16,     0xFF
	sbci  r17,     0xFF

	; If dir. entry low 4 bits are zero, that's the beginning of a 512
	; byte sector which has to be loaded.

	movw  XL,      r16
	andi  XL,      0x0F
	brne  FAT_Find_secloaded

	sbrs  r12,     7
	rjmp  FAT_Find_nextsec
	movw  r24,     r10
	rcall FS_Next_Sector
	cpi   r24,     0x00
	brne  FAT_Find_ret_zero
FAT_Find_nextsec:
	movw  r24,     r10
	rcall FS_Read_Sector
	cpi   r24,     0x00
	brne  FAT_Find_ret_zero
	dec   r12              ; Further sectors will be loaded (sets bit7)

FAT_Find_secloaded:

	; The appropriate sector of the root directory is loaded in the
	; sector buffer. Try to match the file.

	rcall sdlib_get_secbuf
	mov   XL,      r16
	andi  XL,      0x0F
	ldi   XH,      32
	mul   XL,      XH
	add   ZL,      r0
	adc   ZH,      r1      ; Address of directory entry
	clr   r1

	movw  r24,     r10
	movw  r22,     ZL
	push  ZL
	push  ZH
	rcall FS_Get_File_Cluster
	pop   ZH
	pop   ZL

	cp    r22,     r1      ; (r1 is zero)
	cpc   r23,     r1
	cpc   r24,     r1
	cpc   r25,     r1
	breq  FAT_Find_loop    ; Start cluster zero: Invalid entry, continue

	ldd   r0,      Z + 0   ; Char 0 match?
	cp    r0,      r9
	ldd   r0,      Z + 1   ; Char 1 match?
	cpc   r0,      r8
	ldd   r0,      Z + 2   ; Char 2 match?
	cpc   r0,      r7
	ldd   r0,      Z + 3   ; Char 3 match?
	cpc   r0,      r6
	ldd   r0,      Z + 4   ; Char 4 match?
	cpc   r0,      r5
	ldd   r0,      Z + 5   ; Char 5 match?
	cpc   r0,      r4
	ldd   r0,      Z + 6   ; Char 6 match?
	cpc   r0,      r3
	ldd   r0,      Z + 7   ; Char 7 match?
	cpc   r0,      r2
	ldd   r0,      Z + 8   ; Ext 0 match?
	cpc   r0,      r15
	ldd   r0,      Z + 9   ; Ext 1 match?
	cpc   r0,      r14
	ldd   r0,      Z + 10  ; Ext 2 match?
	cpc   r0,      r13
	brne  FAT_Find_loop

	; File matches. Return.

FAT_Find_ret:

	movw  r16,     r2
	pop   r12
	pop   r11
	pop   r10
	pop   r9
	pop   r8
	pop   r7
	pop   r6
	pop   r5
	pop   r4
	pop   r3
	pop   r2
	ret

FAT_Find_ret_zero:

	clr   r22
	clr   r23
	movw  r24,     r22
	rjmp  FAT_Find_ret



/*
** Internal function to retrieve left shift count for sector info when adding
** onto the cluster position.
** r19: Count of shifts to left
** r18: AND mask for the high bits where the sector position is
*/
sdlib_pos_shift:

	ldd   r24,     Z + 3   ; csize: Cluster size in 512 byte sector units
	ldi   r19,     0       ; Left shift count
	ldi   r18,     0xFF    ; AND mask
	rjmp  .+4
	inc   r19
	lsl   r18
	lsl   r24
	brcc  .-8
	ret




/*
** Retrieves file position information. This can be saved for faster seeking
** within a file.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** Outputs:
** r25:r24: Position info, high
** r23:r22: Position info, low
** Clobbers:
** r18, r19, Z
*/
.global FS_Get_Pos
FS_Get_Pos:

	movw  ZL,      r24
	rcall sdlib_pos_shift
	ldd   r25,     Z + 21  ; cclus: Cluster position, high
	com   r18
	and   r25,     r18     ; Mask off any high bit
	ldd   r24,     Z + 22  ; csec: Sector position within cluster
	rjmp  .+2
	lsl   r24              ; Shift up sector position
	subi  r19,     1
	brcc  .-6
	or    r25,     r24     ; Add it to the cluster pos.
	ldd   r24,     Z + 20  ; cclus: Cluster position
	ldd   r23,     Z + 19  ; cclus: Cluster position
	ldd   r22,     Z + 18  ; cclus: Cluster position, low
	ret



/*
** Restores file position using a position info. acquired by FS_Get_Pos.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** r23:r22: Position info, high
** r21:r20: Position info, low
** Clobbers:
** r18, r19, Z
*/
.global FS_Set_Pos
FS_Set_Pos:

	movw  ZL,      r24
	rcall sdlib_pos_shift
	mov   r24,     r23
	and   r24,     r18     ; Mask off sector position within cluster
	com   r18
	and   r23,     r18     ; Clean cluster position
	rjmp  .+2
	lsr   r24              ; Shift down sector position
	subi  r19,     1
	brcc  .-6
	std   Z + 18,  r20     ; cclus: Cluster position, low
	std   Z + 19,  r21     ; cclus: Cluster position
	std   Z + 20,  r22     ; cclus: Cluster position
	std   Z + 21,  r23     ; cclus: Cluster position, high
	std   Z + 22,  r24     ; csec: Sector position within cluster
	ret



/*
** Sends a bootloader request to load another game. The passed SD structure
** must be positioned at the beginning of the .uze image (which may be within
** another file, it doesn't necessarily have to be a stand-alone file). This
** function does not return. It is only supported if a suitable bootloader is
** available.
**
** Inputs:
** r25:r24: Pointer to SD data structure
*/
.global Bootld_Request
Bootld_Request:

	rcall bootlib_hasloader
	brcc  .+4
	jmp   BL_Bootld_Request

	rjmp  .-2              ; It is not possible to do anything if there is
	                       ; no compatible bootloader available.
