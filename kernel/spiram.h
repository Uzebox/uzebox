/*
 *  SPI RAM basic interface functions
 *  Copyright (C) 2017 Sandor Zsuga (Jubatian)
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



/*
**  Important: Before using the functions of this library, you have to
**  initialize the SD card (otherwise it will interfere with the SPI RAM, so
**  doing an SPI init is not sufficient!). The routines assume that the SPI
**  is set up for a data rate of 14MHz (maximal speed on the UzeBox).
*/



#ifndef SPIRAM_H
#define SPIRAM_H


#include <uzebox.h>



/*
** Initializes SPI RAM. It is necessary to call it even after an SD Card init
** to set up the SPI RAM's Chip Select. Returns 1 on success (128K SPI RAM is
** available), 0 on failure.
*/
u8 SpiRamInit(void);



/*
** Reads unsigned or signed 8 bit value from SPI RAM.
*/
u8 SpiRamReadU8(u8 bank, u16 addr);
s8 SpiRamReadS8(u8 bank, u16 addr);

/*
** Writes unsigned or signed 8 bit value to SPI RAM.
*/
void SpiRamWriteU8(u8 bank, u16 addr, u8 val);
void SpiRamWriteS8(u8 bank, u16 addr, s8 val);

/*
** Reads unsigned or signed 16 bit value from SPI RAM.
*/
u16 SpiRamReadU16(u8 bank, u16 addr);
s16 SpiRamReadS16(u8 bank, u16 addr);

/*
** Writes unsigned or signed 16 bit value to SPI RAM.
*/
void SpiRamWriteU16(u8 bank, u16 addr, u16 val);
void SpiRamWriteS16(u8 bank, u16 addr, s16 val);

/*
** Reads unsigned or signed 32 bit value from SPI RAM.
*/
u32 SpiRamReadU32(u8 bank, u16 addr);
s32 SpiRamReadS32(u8 bank, u16 addr);

/*
** Writes unsigned or signed 32 bit value to SPI RAM.
*/
void SpiRamWriteU32(u8 bank, u16 addr, u32 val);
void SpiRamWriteS32(u8 bank, u16 addr, s32 val);

/*
** Reads into RAM from SPI RAM.
*/
void SpiRamReadInto(u8 bank, u16 addr, void* dst, u16 len);

/*
** Writes from RAM to SPI RAM.
*/
void SpiRamWriteFrom(u8 bank, u16 addr, void* src, u16 len);



/*
** Starts a sequential read from SPI RAM.
*/
void SpiRamSeqReadStart(u8 bank, u16 addr);

/*
** Sequentially reads unsigned or signed 8 bit value from SPI RAM.
*/
u8 SpiRamSeqReadU8(void);
s8 SpiRamSeqReadS8(void);

/*
** Sequentially reads unsigned or signed 16 bit value from SPI RAM.
*/
u16 SpiRamSeqReadU16(void);
s16 SpiRamSeqReadS16(void);

/*
** Sequentially reads unsigned or signed 32 bit value from SPI RAM.
*/
u32 SpiRamSeqReadU32(void);
s32 SpiRamSeqReadS32(void);

/*
** Sequentally reads into RAM from SPI RAM.
*/
void SpiRamSeqReadInto(void* dst, u16 len);

/*
** Terminates sequential SPI RAM read.
*/
void SpiRamSeqReadEnd(void);



/*
** Starts a sequential write to SPI RAM.
*/
void SpiRamSeqWriteStart(u8 bank, u16 addr);

/*
** Sequentially writes unsigned or signed 8 bit value to SPI RAM.
*/
void SpiRamSeqWriteU8(u8 val);
void SpiRamSeqWriteS8(s8 val);

/*
** Sequentially writes unsigned or signed 16 bit value to SPI RAM.
*/
void SpiRamSeqWriteU16(u16 val);
void SpiRamSeqWriteS16(s16 val);

/*
** Sequentially writes unsigned or signed 32 bit value to SPI RAM.
*/
void SpiRamSeqWriteU32(u32 val);
void SpiRamSeqWriteS32(s32 val);

/*
** Sequentally writes from RAM to SPI RAM.
*/
void SpiRamSeqWriteFrom(void* src, u16 len);

/*
** Terminates sequential SPI RAM write.
*/
void SpiRamSeqWriteEnd(void);



#endif
