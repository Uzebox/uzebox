/* ***********************************************************************
**
**  Copyright (C) 2006  Jesper Hansen <jesper@redegg.net> 
**
**
**  Simple uart functionality
**
**  File uart.c
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

/** \file uart.c
	Simple uart functionality
*/

#include <avr/io.h>
#include <avr/interrupt.h>
// #include <avr/signal.h>
#include <avr/pgmspace.h>

#include "uart.h"


#if defined(__AVR_ATmega8__)  	|| defined(__AVR_ATmega16__) || 	\
	defined(__AVR_ATmega32__) 	|| defined(__AVR_ATmega8515__) || 	\
	defined(__AVR_ATmega8535__) || defined(__AVR_ATmega323__) 
//
// Single USART devices
//
#define ATMEGA_USART
#define UART0_UBRR_HIGH 	UBRRH
#define UART0_UBRR_LOW 	UBRRL
#define UART0_RECEIVE_INTERRUPT   SIG_UART_RECV
#define UART0_TRANSMIT_INTERRUPT  SIG_UART_DATA
#define UART0_STATUS		UCSRA
#define UART0_CONTROL  	UCSRB
#define UART0_DATA     	UDR
#define UART0_UDRIE    	UDRIE

#elif 	defined(__AVR_ATmega162__) || \
	 	defined(__AVR_ATmega64__)  || \
		defined(__AVR_ATmega128__) || \
		defined(UZEBOX)
//
// Dual USART devices
//
#define ATMEGA_USART0
#define UART0_UBRR_HIGH  	UBRR0H
#define UART0_UBRR_LOW   	UBRR0L
#define UART0_RECEIVE_INTERRUPT   SIG_USART0_RECV
#define UART0_TRANSMIT_INTERRUPT  SIG_USART0_DATA
#define UART0_STATUS    	UCSR0A
#define UART0_CONTROL   	UCSR0B
#define UART0_CONTROL2  	UCSR0C
#define UART0_DATA     	UDR0
#define UART0_UDRIE    	UDRIE0

#define ATMEGA_USART1
#define UART1_UBRR_HIGH  	UBRR1H
#define UART1_UBRR_LOW   	UBRR1L
#define UART1_RECEIVE_INTERRUPT   SIG_USART1_RECV
#define UART1_TRANSMIT_INTERRUPT  SIG_USART1_DATA
#define UART1_STATUS    	UCSR1A
#define UART1_CONTROL   	UCSR1B
#define UART1_CONTROL2  	UCSR1C
#define UART1_DATA     	UDR1
#define UART1_UDRIE    	UDRIE1

#else
//
// unsupported type
//
#error "Processor type not supported in uart.c !"
#endif



//
// size of receive ring buffer
//
#define BUFFER_SIZE		16		//!< Size of receive ringbuffer

uint8_t uart_ring[BUFFER_SIZE];	//!< Receive ringbuffer data space
volatile uint8_t ring_in;		//!< Receive ringbuffer input index
volatile uint8_t ring_out;		//!< Receive ringbuffer output index

/*
uint8_t bytes_in_ring(void)
{
  	if (ring_in > ring_out)
  		return (ring_in - ring_out);
	else if (ring_out > ring_in) 			
  		return (BUFFER_SIZE - (ring_out - ring_in));
	else
		return 0;  		
}
*/

/** UART Receive Complete Interrupt Function.
*/
SIGNAL(UART0_RECEIVE_INTERRUPT)      
{
    uart_ring[ring_in] = UART0_DATA;
	ring_in = (ring_in + 1) % BUFFER_SIZE;
}

/** Init UART.
	Setup I/O ports and interrups for the serial link
*/
void uart_init(void)
{
	ring_in = 0;
	ring_out = 0;

    // set default baud rate 
    UART0_UBRR_HIGH = UART_BAUD_SELECT >> 8;  
    UART0_UBRR_LOW	= UART_BAUD_SELECT;  


#if defined (ATMEGA_USART)
    // enable receive, transmit and ensable receive interrupts 
    UART0_CONTROL = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
    // set default format, asynch, n,8,1
    #ifdef URSEL
    	UCSRC = (1<<URSEL)|(3<<UCSZ0);
    #else
    	UCSRC = (3<<UCSZ0);
    #endif 
    
#elif defined (ATMEGA_USART0 )
    // enable receive, transmit and ensable receive interrupts 
    UART0_CONTROL = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
    // set default format, asynch, n,8,1
 /*   #ifdef URSEL0
	    UCSR0C = (1<<URSEL0)|(3<<UCSZ00);
    #else
    	UCSR0C = (3<<UCSZ00);
    #endif 
*/
#endif

    // enable interrupts 
    sei();
}

#if 0
/** Set Communications Speed.
	\param speed Communications speed to set
*/
void uart_setspeeed(uint16_t speed)
{
	uint16_t temp = (uint16_t) ((uint32_t) F_CPU / (((uint32_t) speed * 16L) - 1));

    // set baud rate 
    UART0_UBRR_HIGH = temp >> 8;
    UART0_UBRR_LOW 	= temp;
}

/** Receive a character.
	Get a character from the communications line.
	Will block if no character is available.
	\return Received character
*/
int uart_getchar(void)
{
	int c;
    while( ring_in == ring_out);				// block waiting

	c = uart_ring[ring_out];    	
	ring_out = (ring_out + 1) % BUFFER_SIZE;
  
    return c;
}

/** Check for available data.
	Check if any character is ready in the receive buffer
	\return Non-zero if character is available
*/
uint8_t uart_haschar(void)
{
	return (ring_in != ring_out);
}
#endif

/** Send a character.
	Transmit a character on the communications line.
	\param data Character to send
	\return Copy of transmitted character.
*/
int uart_putchar(char data)
{   
#if defined (ATMEGA_USART)
	while ( !(UART0_STATUS & (1<<UDRE)) );		// wait for free space in buffer
#else
	while ( !(UART0_STATUS & (1<<UDRE0)) );		// wait for free space in buffer
#endif
    UART0_DATA = data;							// send byte
	return data;
}
