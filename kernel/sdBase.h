/*
 *  Uzebox Kernel SD/MMC function prototypes
 *  Copyright (C) 2008-2009 Alec Bourque, Eric Anderthon
 *  
 *  Based on work by: Copyright (C) 2006  Jesper Hansen <jesper@redegg.net> 
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
 *
 *  Uzebox is a reserved trade mark
*/
#pragma once

#include <stdint.h>

#ifdef SD_CARD_DEBUG
    extern uint32_t SD_DEBUG_bootRecordSector;
    extern uint16_t SD_DEBUG_bytesPerSector;
    extern uint8_t  SD_DEBUG_sectorsPerCluster;
    extern uint16_t SD_DEBUG_reservedSectors;
    extern uint16_t SD_DEBUG_maxRootDirectoryEntries;
    extern uint16_t SD_DEBUG_sectorsPerFat;
    extern uint32_t SD_DEBUG_dirTableSector;
    extern uint16_t SD_DEBUG_firstCluster;
    extern uint32_t SD_DEBUG_firstSector;
#endif

extern void     sdCardDirectReadSimple(uint8_t *dest, uint16_t count);
extern void     sdCardDirectRead(uint8_t *dest, uint16_t count, uint8_t span, uint8_t run);
extern long     sdCardFindFileFirstSector(const char *fileName);
extern long     sdCardFindFileFirstSectorFlash(const char *fileName);

extern void     sdCardSkipBytes(uint16_t toSkip);
extern uint32_t sdCardGetLong(void);
extern uint16_t sdCardGetInt(void);
extern uint8_t  sdCardGetChar(void);
extern uint8_t  sdCardGetByte(void);
extern uint8_t  sdCardSendByteFF(void);
extern void     sdCardSendByte(uint8_t toSend);
extern void     sdCardSendCommand(uint8_t command, uint16_t px, uint16_t py);
extern uint8_t  sdCardInitNoBuffer(void);
extern uint8_t  sdCardCueSectorAddress(uint32_t lba);
extern uint8_t  sdCardCueByteAddress(uint32_t address);
extern uint8_t  sdCardStopTransmission(void);
