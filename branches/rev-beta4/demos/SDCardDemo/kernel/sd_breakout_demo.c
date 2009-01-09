/* ***********************************************************************
**
**  Copyright (C) 2006  Jesper Hansen <jesper@redegg.net> 
**
**
**  Simple MMC/SD card example
**
**  File sd_breakout_demo.c
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

/** \file sd_breakout_demo.c
	Simple MMC/SD card example
*/


/*! 
 * \mainpage MMC/SD Card Example Code
 *
 * <img src="sd_breakout2_s.jpg">
 *
 * \section intro_sec Introduction
 *
 * This code example show how to intialize and readout sector data from
 * a MMC or SD card.
 * These cards can use SPI to communicate with a host, thereby making it
 * pretty simple to interface with a microprocessor.
 *
 * \section requisites_sec Prerequisites
 *
 * The code is written for an AVR microprocessor, but can relatively easy be modified
 * to run on other microprocessors, if you really really want to.
 * The tools used to write the code is the AVR Studio IDE  * with the AVR-GCC C compiler.
 * These tools are FREE, and you can find them at <a href="http://www.avrfreaks.com">AVR Freaks</a>
 *
 * You obviously also need a MMC or SD card that can be interfaced to your processor.
 * A sugestion is to use the <b>SD Breakout card</b> (shown above) from the <a href="http://www.jelu.se">JELU Web-shop</a>.<br>
 * You can also use the <b>Mega8 Mini-Devboard</b>, also from the <a href="http://www.jelu.se">JELU Web-shop</a>.<br>
 * The code has been written for and tested on both these platforms.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Installing the files
 * 
 * Extract the files to any subdirectory on your machine.
 *
 * \subsection step2 Step 2: Configuring for your hardware
 *
 * Assuming you're using AVR Studio, open the <b>sd_breakout_demo.aps</b> project file.
 * Set the Frequency on the Project Option Tab to suit your target system. 
 * If you're using the Mega8 Mini-Devboard, set the Device to atmega8.
 *
 * \subsection step3 Step 3: Making the project
 *
 * Simply hit F7, or click the Build Active Project button.
 *
 * \subsection step4 Step 4: Downloading to target.
 *  
 * Download the HEX file to your target, using your regular download tool.
 *	
 * \subsection step5 Step 5: Running the code
 *  
 * Connect a terminal to the UART of your dev-board, set the speed to 115200 bps
 * Insert a MMC or SD card and reset your target.
 * You should now be able to browse the sectors of your MMC/SD card.
 *
 *
 * /Jesper
 *
 *
 ****************************************************************************************/
 

#include <avr/io.h>
#include <inttypes.h>
#include <stdio.h>
#include <ctype.h>

#include "uart.h"
#include "mmc_if.h"



/** HEX dump a block of data.
*/
void dump(uint8_t * p, uint16_t len)
{
	int i,j;

	for (i=0;i<len/16;i++)
	{
		printf("%04x  ",i*16);
		for (j=0;j<16;j++)
			printf("%02x ",p[i*16+j]);
		printf("  ");
		for (j=0;j<16;j++)
			printf("%c", isalpha(p[i*16+j]) ? p[i*16+j] : '.');
		printf("\n");
	}
}



/** main function.
*/
int main(void)
{
	uint32_t sector = 0;
	uint8_t sectorbuffer[512];

	//
	// init the uart
	//
	uart_init();

	//
	// setup callbacks for stdio use
	//
 	fdevopen(uart_putchar,uart_getchar,0);

	//
	// say hello
	//
	printf("\n\n**  mmc_demo is alive. **\n");

	//
	// init mmc card and report status
	//
	printf("mmc_init returns %d\n", mmc_init() );

	//
	// main loop
	//	
	while (1)
	{
		printf("\nsector %ld\n",sector);		// show sector number
		mmc_readsector(sector,sectorbuffer);	// read a data sector
		dump(sectorbuffer,512);					// dump sector contents
	
		//
		// get user keypress
		//
		switch (getchar())
		{
			case '-' : 				// if '-' key
				if (sector > 0) 	// and sectornumber > 0
					sector--; 		// decrement sectornumber	
				break;

			case '+' : 				// if '+' key
				sector++;			// increment sector number
				break;
		}
	}

}
