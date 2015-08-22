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

#define CMD_RESET 0
#define CMD_INIT 1
#define CMD_SEND_CSD 9
#define CMD_SEND_CID 10
#define CMD_READBLOCK 17
#define CMD_READMULTIBLOCK 18
#define CMD_STOPTRANSMISSION 12


extern uint8_t mmc_cue_sector_exact_position(uint32_t address);
extern uint8_t mmc_cuesector(uint32_t lba);
extern uint8_t mmc_stoptransmission(void);
extern uint8_t mmc_readsector(uint32_t lba);
extern uint8_t mmc_init(uint8_t *buffer);
extern uint8_t mmc_init_no_buffer(void);
extern void mmc_send_command(uint8_t command, uint16_t px, uint16_t py);
extern uint8_t mmc_datatoken(void);
extern uint8_t mmc_get(void);
extern uint8_t spi_byte(uint8_t byte);
extern uint8_t spibyte_ff(void);
extern uint8_t mmc_get_byte(void);
extern void mmc_clock_and_release(void);

extern void mmcSkipBytes(uint16_t toSkip);
extern char mmcGetChar(void);
extern int  mmcGetInt(void);
extern long mmcGetLong(void);

extern long mmcFindFileFirstSector(const char *fileName);
extern long mmcFindFileFirstSectorFlash(const char *fileName);

extern void mmcDirectRead(uint8_t *dest, uint16_t count, uint8_t span, uint8_t run);
