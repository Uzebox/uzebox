
/* ***********************************************************************
**
**  Copyright (C) 2006  Jesper Hansen <jesper@redegg.net> 
**
**
**  Interface functions for MMC/SD cards
**
**  File mmc_if.h
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

/** \file mmc_if.c
	Simple MMC/SD-card functionality
*/


#include <avr/io.h>
#include <inttypes.h>
#include <stdio.h>
#include "mmc_if.h"
#include <avr/pgmspace.h>
#include "uzebox.h"

uint32_t volatile mmc_mixer_sector;
bool volatile mmc_mixer_active=false;

/** Hardware SPI I/O. 
	\param byte Data to send over SPI bus
	\return Received data from SPI bus
*/
static uint8_t spi_byte(uint8_t byte) __attribute__((noinline));

extern unsigned char internal_spi_byte(unsigned char c);

static uint8_t spi_byte(uint8_t byte)
{
	
	//SPDR = byte;
	//while(!(SPSR & (1<<SPIF)))
	//{}
	//return SPDR;
	
	return internal_spi_byte(byte);
}



/** Send a command to the MMC/SD card.
	\param command	Command to send
	\param px	Command parameter 1
	\param py	Command parameter 2
*/
void mmc_send_command(uint8_t command, uint16_t px, uint16_t py)
{
	register union u16convert r;

	MMC_CS_PORT &= ~(1 << MMC_CS);	// enable CS

	spi_byte(0xff);			// dummy byte

	spi_byte(command | 0x40);

	r.value = px;
	spi_byte(r.bytes.high);	// high byte of param x
	spi_byte(r.bytes.low);	// low byte of param x

	r.value = py;
	spi_byte(r.bytes.high);	// high byte of param y
	spi_byte(r.bytes.low);	// low byte of param y

	spi_byte(0x95);			// correct CRC for first command in SPI          
							// after that CRC is ignored, so no problem with 
							// always sending 0x95                           
	spi_byte(0xff);			// ignore return byte
}


/** Get Token.
	Wait for and return a non-ff token from the MMC/SD card
	\return The received token or 0xFF if timeout
*/
uint8_t mmc_get(void)
{
	uint16_t i = 0xffff;
	uint8_t b = 0xff;

	while ((b == 0xff) && (--i)) 
	{
		b = spi_byte(0xff);
	}
	return b;

}

/** Get Datatoken.
	Wait for and return a data token from the MMC/SD card
	\return The received token or 0xFF if timeout
*/
unsigned int data_delay;
uint8_t mmc_datatoken(void)
{
	uint16_t i = 0xffff;
	uint8_t b = 0xff;

	while ((b != 0xfe) && (--i)) 
	{
		b = spi_byte(0xff);
	}
	data_delay=0xffff-i;
	return b;
}


/** Finish Clocking and Release card.
	Send 10 clocks to the MMC/SD card
 	and release the CS line 
*/
void mmc_clock_and_release(void)
{
	uint8_t i;

	// SD cards require at least 8 final clocks
	for(i=0;i<10;i++)
		spi_byte(0xff);	

    MMC_CS_PORT |= (1 << MMC_CS);	// release CS
}



/** Read MMC/SD sector.
 	Read a single 512 byte sector from the MMC/SD card
	\param lba	Logical sectornumber to read
	\param buffer	Pointer to buffer for received data
	\return 0 on success, -1 on error
*/
int mmc_readsector(uint32_t lba, uint8_t *buffer)
{
	uint16_t i;

	if(mmc_mixer_active==true){
		Print(2,1,PSTR("ERROR:1"));
	}

	// send read command and logical sector address
	mmc_send_command(17,(lba>>7) & 0xffff, (lba<<9) & 0xffff);

	if (mmc_datatoken() != 0xfe)	// if no valid token
	{
	    mmc_clock_and_release();	// cleanup and	
   		return -1;					// return error code
	}

	for (i=0;i<512;i++)				// read sector data
    	*buffer++ = spi_byte(0xff);

	spi_byte(0xff);					// ignore dummy checksum
	spi_byte(0xff);					// ignore dummy checksum

    mmc_clock_and_release();		// cleanup

	return 0;						// return success		
}


//used in multi sector read
int mmc_readNextSector(uint8_t *buffer)
{
	uint16_t i;

	if (mmc_datatoken() != 0xfe)	// if no valid token
	{
	    mmc_clock_and_release();	// cleanup and	
   		return -1;					// return error code
	}

	for (i=0;i<512;i++)				// read sector data
    	*buffer++ = spi_byte(0xff);

	spi_byte(0xff);					// ignore dummy checksum
	spi_byte(0xff);					// ignore dummy checksum


	return 0;						// return success		
}



/** Init MMC/SD card.
	Initialize I/O ports for the MMC/SD interface and 
	send init commands to the MMC/SD card
	\return 0 on success, other values on error 
*/
uint8_t mmc_init(void)
{
	int i;


	// setup I/O ports 

	SPI_PORT &= ~((1 << MMC_SCK) | (1 << MMC_MOSI));	// low bits
	SPI_PORT |= (1 << MMC_MISO);						// high bits
	SPI_DDR  |= (1<<MMC_SCK) | (1<<MMC_MOSI);			// direction


	MMC_CS_PORT |= (1 << MMC_CS);	// Initial level is high	
	MMC_CS_DIR  |= (1 << MMC_CS);	// Direction is output


	// also need to set SS as output
#if defined(__AVR_ATmega8__)
	// is already set as CS, but we set it again to accomodate for other boards 
	SPI_DDR |= (1<<2);
#else
	SPI_DDR |= (1<<0);			// assume it's bit0 (mega128, portB and others)
#endif

	SPCR = (1<<MSTR)|(1<<SPE);	// enable SPI interface
	SPSR = 1;					// set double speed	

	for(i=0;i<10;i++)			// send 80 clocks while card power stabilizes
		spi_byte(0xff);

	mmc_send_command(0,0,0);	// send CMD0 - reset card

	if (mmc_get() != 1)			// if no valid response code
	{
	   mmc_clock_and_release();
	   return 1;  				// card cannot be detected
	}

	//
	// send CMD1 until we get a 0 back, indicating card is done initializing 
	//
	i = 0xffff;						// max timeout
	while ((spi_byte(0xff) != 0) && (--i))	// wait for it
	{
	     mmc_send_command(1,0,0);	// send CMD1 - activate card init
	}

    mmc_clock_and_release();		// clean up

	if (i == 0)						// if we timed out above
	   return 2;					// return failure code

	return 0;
}




void mmc_mixerStart(uint32_t lba){
	if(mmc_mixer_active==true){
		mmc_send_command(12,0,0); //stop current transfer
		mmc_clock_and_release();
	}
		
	// send the multiple block read command and logical sector address
	mmc_mixer_sector=lba;
	mmc_send_command(18,(mmc_mixer_sector>>7) & 0xffff, (mmc_mixer_sector<<9) & 0xffff);		
	mmc_mixer_active=true;
}

void mmc_mixerStop(){
	if(mmc_mixer_active==true){
		mmc_send_command(12,0,0); //stop transfers
		mmc_clock_and_release();

		//Initialize the mixer buffer
		for(int i=0;i<MIX_BANK_SIZE*2;i++){
			mix_buf[i]=0x80;
		}		
	}	
	mmc_mixer_active=false;
}

uint32_t mmc_mixerGetCurrentSector(){
	return mmc_mixer_sector;
}



//call once on each vsync
void mmc_processMixer()
{
	
	static int mixerRead=0,sectorRead=0;
	int retVal=0;
	uint8_t *buf=mix_buf;	


	if(mmc_mixer_active){

		if(mix_bank==1){
			buf+=MIX_BANK_SIZE;
		}


		do{
		
			if(sectorRead==0){				
				if (mmc_datatoken() != 0xfe)	// if no valid token
				{
				    mmc_clock_and_release();	// cleanup and	
					Print(10,10,PSTR("TOKEN!"));
					return;
				}
			}

			do{			// read sector data

		    	*buf++=spi_byte(0xff);
			 
				mixerRead++;
				sectorRead++;

			}while(mixerRead<MIX_BANK_SIZE && sectorRead<512);
	

			if(sectorRead==512){
				spi_byte(0xff);					// ignore dummy checksum
				spi_byte(0xff);					// ignore dummy checksum
			    
			
				mmc_mixer_sector++;
				sectorRead=0;
			}

			if(mixerRead==MIX_BANK_SIZE){
				mixerRead=0;
				retVal=1;		
			}


		}while(retVal==0);
	}
	

}


