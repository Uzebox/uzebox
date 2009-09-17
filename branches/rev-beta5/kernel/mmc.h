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

extern uint8_t mmc_readsector(uint32_t lba);
extern uint8_t mmc_init(uint8_t *buffer);
extern void mmc_send_command(uint8_t command, uint16_t px, uint16_t py);
extern uint8_t mmc_datatoken(void);
extern uint8_t mmc_get(void);
extern uint8_t spi_byte(uint8_t byte);
extern void mmc_clock_and_release(void);



