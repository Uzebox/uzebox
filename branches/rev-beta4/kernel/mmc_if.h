
/* ***********************************************************************
**
**  Copyright (C) 2006  Jesper Hansen <jesper@redegg.net> 
**
**
**  Interface functions for MMC/SD cards
**
**  File mmc_if.h
**
**  Modified to run on uzebox (changed MMC_CS_... definitions)
**
*************************************************************************
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software Foundation, 
**  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
*************************************************************************/

/** \file mmc_if.h
	Simple MMC/SD-card functionality
*/

#ifndef __MMC_IF_H__
#define __MMC_IF_H__


/** @name MMC/SD Card I/O ports
*/
//@{
#define SPI_PORT	PORTB
#define SPI_DDR		DDRB
#define SPI_PIN		PINB

#if defined(UZEBOX)
	#define MMC_CS_PORT	PORTD
	#define MMC_CS_DIR	DDRD
#else
	#define MMC_CS_PORT	PORTB
	#define MMC_CS_DIR	DDRB
#endif

//@}

/** @name MMC/SD Card I/O lines in MMC mode
*/
//@{
#define SD_SCK		1	//!< Clock
#define SD_CMD		2
#define SD_DAT0		3
#define SD_DAT3		4
#define SD_DAT1		5
#define SD_DAT2		6
#define SD_CARD		7
//@}

/** @name MMC/SD Card I/O lines in SPI mode
*/
//@{
#if defined(UZEBOX)

	#define MMC_SCK    7
	#define MMC_MOSI   5
	#define MMC_MISO   6
	#define MMC_CS     6

#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || \
    defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || defined(__AVR_ATmega162__) 

	#define MMC_SCK		5
	#define MMC_MOSI	3
	#define MMC_MISO	4
	#define MMC_CS		2

#elif defined(__AVR_ATmega64__)  || defined(__AVR_ATmega128__) 

	#define MMC_SCK		1
	#define MMC_MOSI	2
	#define MMC_MISO	3
	#define MMC_CS		0

#else
	//
	// unsupported type
	//
	#error "Processor type not supported in mmc_if.h !"

#endif
//@}



/** Helper structure.
	This simplify conversion between bytes and words.
*/
struct u16bytes
{
	uint8_t low;	//!< byte member
	uint8_t high;	//!< byte member
};

/** Helper union.
	This simplify conversion between bytes and words.
*/
union u16convert
{
	uint16_t value;			//!< for word access
	struct u16bytes bytes;	//!< for byte access
};

/** Helper structure.
	This simplify conversion between bytes and longs.
*/
struct u32bytes
{
	uint8_t byte1;	//!< byte member
	uint8_t byte2;	//!< byte member
	uint8_t byte3;	//!< byte member
	uint8_t byte4;	//!< byte member
};

/** Helper structure.
	This simplify conversion between words and longs.
*/
struct u32words
{
	uint16_t low;		//!< word member
	uint16_t high;		//!< word member
};

/** Helper union.
	This simplify conversion between bytes, words and longs.
*/
union u32convert 
{
	uint32_t value;			//!< for long access
	struct u32words words;	//!< for word access
	struct u32bytes bytes;	//!< for byte access
};






/** Read MMC/SD sector.
 	Read a single 512 byte sector from the MMC/SD card
	\param lba	Logical sectornumber to read
	\param buffer	Pointer to buffer for received data
	\return 0 on success, -1 on error
*/
int mmc_readsector(uint32_t lba, uint8_t *buffer);


/** Init MMC/SD card.
	Initialize I/O ports for the MMC/SD interface and 
	send init commands to the MMC/SD card
	\return 0 on success, other values on error 
*/
uint8_t mmc_init(void);

#endif
