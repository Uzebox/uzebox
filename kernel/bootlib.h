/*
 *  SD Card interface library
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



#ifndef BOOTLIB_H
#define BOOTLIB_H


#include <stdint.h>


/* Definitions for accessing the Flags field */

#define  SDC_FLAGS_INIT   0x01U
#define  SDC_FLAGS_SDHC   0x02U
#define  SDC_FLAGS_FAT32  0x04U
#define  SDC_FLAGS_CRCOFF 0x10U


/*
** SD data access structure. Normally with the exception of bufp it shouldn't
** be used directly. See sdlib.s for the definitions of the fields. For bufp
** you have to provide an 512 byte sector buffer area.
*/
typedef struct{
 uint8_t  flags;
 uint8_t* bufp;  /* SD sector buffer */
 uint8_t  csize;
 uint16_t fatp;
 uint32_t datap;
 uint32_t rootp;
 uint32_t fclus;
 uint32_t cclus;
 uint8_t  csec;
}sdc_struct_t;


/*
** Set SPI data rate for SD access (slower speed). Note that this is not
** necessary with FS routines as they do it for themselves.
*/
void     SPI_Set_SD(void);


/*
** Set SPI data rate to maximum speed (for example to interface SPI RAM).
** Note that this is not necessary with FS routines as they do it before
** returing (so you can interleave them with handling an SPI peripheral
** demanding max speed).
*/
void     SPI_Set_Max(void);


/*
** Calculates CRC7 for an SD command. Calculation must start with 0x00 for
** crcval. The result after completing the calculation must be stored after a
** left shift and an OR with 1.
*/
uint8_t  SDC_CRC7_Byte(uint8_t crcval, uint8_t byte);


/*
** Calculates CRC16 for an SD sector. Calculation must start with 0x0000 for
** crcval. The result after completing must be used as-is, in Little Endian
** byte order (the card sends and accepts in this byte order).
*/
uint16_t SDC_CRC16_Byte(uint16_t crcval, uint8_t byte);


/*
** Waits for the end of a sequence of 0xFF returning the byte breaking the
** sequence. Waits for up to 4096 bytes.
*/
uint8_t  SDC_Wait_FF(void);


/*
** Sends SD command and waits for the first response byte (normally R1), which
** it returns. Calculates CRC proper, and supports low SPI speeds. Pulls CS
** low before sending the command, and keeps it that way (low).
*/
uint8_t  SDC_Command(uint8_t cmd, uint32_t data);


/*
** Releases the SD card appropriately with a trailing 0xFF byte.
*/
void     SDC_Release(void);


/*
** Detects and initializes SD card. This takes a few dozen milliseconds. It
** populates the SD data structure according to the results, which means
** setting the Initialized, SDHC and CRC flags. Normally you don't ever need
** to call this as it is called by FS_Init().
**
** Returns zero on success, otherwise:
** 1: CMD0 failed (possibly no card in socket)
** 2: CMD59 failed (couldn't enable CRC checking)
** 3: ACMD41 failed (not possible to initialize, bad card)
** 4: ACMD41 timed out
** 5: CMD58 failed (couldn't query card)
*/
uint8_t  SDC_Init(sdc_struct_t* sds);


/*
** Toggles SD CRC checking. By default CRC checking is normally ON.
*/
void     SDC_CRC_Enable(sdc_struct_t* sds, uint8_t ena);


/*
** Converts sector address for SD card type. This can be used to generate an
** address parameter for SDC_Command.
*/
uint32_t SDC_Command_Address(sdc_struct_t* sds, uint32_t sector);


/*
** Performs a single sector read (attempts a retry on fault).
**
** Returns zero on success, otherwise:
** 1: Card is not initialized
** 2: CMD17 failed
** 3: Timed out during waiting for data token
** 4: CRC error (data is loaded, but possibly corrupt)
*/
uint8_t  SDC_Read_Sector(sdc_struct_t* sds, uint32_t sector);


/*
** Performs a single sector write (attempts a retry on fault).
**
** Returns zero on success, otherwise:
** 1: Card is not initialized
** 2: CMD24 failed
** 3: Timed out during waiting (card should be reinitialized)
** 4: CRC error (data rejected by card)
*/
uint8_t  SDC_Write_Sector(sdc_struct_t* sds, uint32_t sector);


/*
** Detects and initializes SD card and FAT filesystem over it. This takes a
** few dozen milliseconds. It populates the SD data structure according to the
** results, which means setting the Filesystem type flag in addition to SD
** init.
**
** Returns zero if initialization succeeded. Otherwise:
** 1: SD Init: CMD0 failed (possibly no card in socket)
** 2: SD Init: CMD59 failed (couldn't enable CRC checking)
** 3: SD Init: ACMD41 failed (not possible to initialize, bad card)
** 4: SD Init: ACMD41 timed out
** 5: SD Init: CMD58 failed (couldn't query card)
** 6: SD read fault
** 7: No usable FAT filesystem found
*/
uint8_t  FS_Init(sdc_struct_t* sds);


/*
** Returns currently selected sector of file.
*/
uint32_t FS_Get_Sector(sdc_struct_t* sds);


/*
** Loads currently selected sector of file into sector buffer.
**
** Returns zero on success, otherwise SDC_Read_Sector errors.
*/
uint8_t  FS_Read_Sector(sdc_struct_t* sds);


/*
** Saves sector buffer into currently selected sector of file.
**
** Returns zero on success, otherwise SDC_Write_Sector errors.
*/
uint8_t  FS_Write_Sector(sdc_struct_t* sds);


/*
** Moves sector pointer forwards one sector (supports fragmentation).
**
** Returns zero on success, otherwise:
** 1: End of file or other error.
*/
uint8_t  FS_Next_Sector(sdc_struct_t* sds);


/*
** Resets sector pointer to the beginning of the file.
*/
void     FS_Reset_Sector(sdc_struct_t* sds);


/*
** Selects root directory for reading.
*/
void     FS_Select_Root(sdc_struct_t* sds);


/*
** Selects a start cluster for reading (for example one returned by
** FAT_Get_File_Cluster()).
*/
void     FS_Select_Cluster(sdc_struct_t* sds, uint32_t cluster);


/*
** Returns a file's start cluster by 32 byte file descriptor (obtained by
** reading the root directory). Returns zero if the file descriptor is not
** valid.
*/
uint32_t FS_Get_File_Cluster(sdc_struct_t* sds, uint8_t* fdesc);


/*
** Finds a file and returns its start cluster. Returns zero if the file is not
** found.
**
** File names have to be supplied upper case (as they are stored this way in
** the FS), even positions on the high bytes, odd positions on the low bytes.
*/
uint32_t FS_Find(sdc_struct_t* sds,
                 uint16_t ch01, uint16_t ch23, uint16_t ch45, uint16_t ch67,
                 uint16_t ex01, uint16_t ex2x);


/*
** Retrieves file position information. This can be saved for faster seeking
** within a file.
*/
uint32_t FS_Get_Pos(sdc_struct_t* sds);


/*
** Restores file position using a position info. acquired by FS_Get_Pos.
*/
void     FS_Set_Pos(sdc_struct_t* sds, uint32_t pos);


/*
** Sends a bootloader request to load another game. The passed SD structure
** must be positioned at the beginning of the .uze image (which may be within
** another file, it doesn't necessarily have to be a stand-alone file). This
** function does not return. It is only supported if a suitable bootloader is
** available.
*/
void     Bootld_Request(sdc_struct_t* sds);


#endif
