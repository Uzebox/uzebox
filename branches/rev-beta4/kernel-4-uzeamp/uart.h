
/* ***********************************************************************
**
**  Copyright (C) 2006  Jesper Hansen <jesper@redegg.net> 
**
**
**  Simple uart functionality
**
**  File uart.h
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

/** \file uart.h
	Simple uart functionality
*/

#ifndef __UART_H__
#define __UART_H__

#include <inttypes.h>

// requires the constant F_CPU to be set to the frequency of the CPU crystal 
#if !defined F_CPU
#error Constant F_CPU not set !
#endif


//
// UART Default Baud rate calculation 
//
#define UART_BAUD_RATE			115200								 	//!< Default Baudrate
#define UART_BAUD_SELECT		(F_CPU / (UART_BAUD_RATE * 16L) - 1)	//!< Calculation of default baudrate


/** Check for available data.
	Check if any character is ready in the receive buffer
	\return Non-zero if character is available
*/
uint8_t uart_haschar( void );

/** Init UART.
	Setup I/O ports and interrups for the serial link
*/
void 	uart_init( void );

/** Set Communications Speed.
	\param speed Communications speed to set
*/
void 	uart_setspeed( uint16_t speed );

/** Receive a character.
	Get a character from the communications line.
	Will block if no character is available.
	\return Received character
*/
int  	uart_getchar( void );

/** Send a character.
	Transmit a character on the communications line.
	\param data Character to send
	\return Copy of transmitted character.
*/
int  	uart_putchar( char data );


#endif

