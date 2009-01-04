/*
(The MIT License)

Copyright (c) 2008, David Etherton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef GUI
#define GUI		1
#endif

// 0 = no video (for benchmarking), 1 = have video
#ifndef VIDEO_METHOD
#define VIDEO_METHOD	1
#endif

#if GUI
// If you're building from the command line or on a non-MS compiler you'll need
// -lSDL or somesuch.
#include "SDL.h"
#if defined (_MSC_VER)
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif
#endif

#if defined (_MSC_VER) && _MSC_VER >= 1400
// don't whine about sprintf and fopen.
// could switch to sprintf_s but that's not standard.
#pragma warning(disable:4996)
#endif

// 644 Overview: http://www.atmel.com/dyn/resources/prod_documents/doc2593.pdf
// AVR8 insn set: http://www.atmel.com/dyn/resources/prod_documents/doc0856.pdf
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;

enum { NES_A, NES_B, PAD_SELECT, PAD_START, PAD_UP, PAD_DOWN, PAD_LEFT, PAD_RIGHT };
enum { SNES_B, SNES_Y, SNES_A=8, SNES_X, SNES_LSH, SNES_RSH };

#if 1	// 644P
const unsigned sramSize = 4096;
const unsigned progSize = 65536;
const unsigned eepromSize = 2048;
#else	// 1284P
const unsigned sramSize = 16384;
const unsigned progSize = 131072;
const unsigned eepromSize = 4096;	// unconfirmed
#endif

#define X		((XL)|(XH<<8))
#define DEC_X	(XL-- || XH--)
#define INC_X	(++XL || ++XH)

#define Y		((YL)|(YH<<8))
#define DEC_Y	(YL-- || YH--)
#define INC_Y	(++YL || ++YH)

#define Z		((ZL)|(ZH<<8))
#define DEC_Z	(ZL-- || ZH--)
#define INC_Z	(++ZL || ++ZH)

#define SP		(SPL | (SPH<<8))
#define DEC_SP	(SPL-- || SPH--)
#define INC_SP	(++SPL || ++SPH)

#define SREG_I 7
#define SREG_T 6
#define SREG_H 5
#define SREG_S 4		// == N ^ V
#define SREG_V 3
#define SREG_N 2
#define SREG_Z 1
#define SREG_C 0

#define IOBASE		32
#define SRAMBASE	256

/*
 	 1 $0000(1) RESET External Pin, Power-on Reset, Brown-out Reset
	 2 $0002 INT0 External Interrupt Request 0
	 3 $0004 INT1 External Interrupt Request 1
	 4 $0006 INT2 External Interrupt Request 2
	 5 $0008 PCINT0 Pin Change Interrupt Request 0
	 6 $000A PCINT1 Pin Change Interrupt Request 1
	 7 $000C PCINT2 Pin Change Interrupt Request 2
	 8 $000E PCINT3 Pin Change Interrupt Request 3
	 9 $0010 WDT Watchdog Time-out Interrupt
	10 $0012 TIMER2_COMPA Timer/Counter2 Compare Match A
	11 $0014 TIMER2_COMPB Timer/Counter2 Compare Match B
	12 $0016 TIMER2_OVF Timer/Counter2 Overflow
	13 $0018 TIMER1_CAPT Timer/Counter1 Capture Event
	14 $001A TIMER1_COMPA Timer/Counter1 Compare Match A
	15 $001C TIMER1_COMPB Timer/Counter1 Compare Match B
	16 $001E TIMER1_OVF Timer/Counter1 Overflow
	17 $0020 TIMER0_COMPA Timer/Counter0 Compare Match A
	18 $0022 TIMER0_COMPB Timer/Counter0 Compare match B
	19 $0024 TIMER0_OVF Timer/Counter0 Overflow
	20 $0026 SPI_STC SPI Serial Transfer Complete
	21 $0028 USART0_RX USART0 Rx Complete
	22 $002A USART0_UDRE USART0 Data Register Empty
	23 $002C USART0_TX USART0 Tx Complete
	24 $002E ANALOG_COMP Analog Comparator
	25 $0030 ADC ADC Conversion Complete
	26 $0032 EE_READY EEPROM Ready
	27 $0034 TWI 2-wire Serial Interface
	28 $0036 SPM_READY Store Program Memory Ready
*/
#define INT_RESET		0x00
#define TIMER1_COMPA	0x1A

#define REG_TCNT1L		0x84

static const char *port_name(int);

namespace ports 
{
	enum
	{
		PINA,  DDRA,  PORTA, PINB,  DDRB,  PORTB, PINC,  DDRC,
		PORTC, PIND,  DDRD,  PORTD, res2C, res2D, res2E, res2F,
		res30, res31, res32, res33, res34, TIFR0, TIFR1, TIFR2,
		res38, res39, res3A, PCIFR, EIFR,  EIMSK, GPIOR0,EECR,
		EEDR,  EEARL, EEARH, GTCCR, TCCR0A,TCCR0B,TCNT0, OCR0A,
		OCR0B, res49, GPIOR1,GPIOR2,SPCR,  SPSR,  SPDR,  res4f,
		ACSR,  OCDR,  res52, SMCR,  MCUSR, MCUCR, res56, SPMCSR,
		res58, res59, res5A, res5B, res5C, SPL,   SPH,  SREG,
		WDTCSR, CLKPR, res62, res63, PRR, res65, OSCCAL, res67,
		PCICR, EICRA, res6a, PCMSK0, PCMSK1, PCMSK2, TIMSK0, TIMSK1,
		TIMSK2, res71, res72, PCMSK3, res74, res75, res76, res77,
		ADCL, ADCH, ADCSRA, ADCSRB, ADMUX, res7d, DIDR0, DIDR1,
		TCCR1A, TCCR1B, TCCR1C, res83, TCNT1L, TCNT1H, ICR1L, ICR1H,
		OCR1AL, OCR1AH, OCR1BL, OCR1BH, res8c, res8d, res8e, res8f,
		res90, res91, res92, res93, res94, res95, res96, res97,
		res98, res99, res9a, res9b, res9c, res9d, res9e, res9f,
		resa0, resa1, resa2, resa3, resa4, resa5, resa6, resa7,
		resa8, resa9, resaa, resab, resac, resad, resae, resaf,
		TCCR2A, TCCR2B, TCNT2, OCR2A, OCR2B, resb5, ASSR, resb7,
		TWBR, TWSR, TWAR, TWDR, TWCR, TWAMR, resbe, resbf,
		UCSR0A, UCSR0B, UCSR0C, resc3, UBRR0L, UBRR0H, UDR0, resc7
	};
}

class ringBuffer
{
public:
	ringBuffer(int s) : head(0), tail(0), avail(s), size(s)
	{
		buffer = new u8[size];
	}
	~ringBuffer()
	{
		delete[] buffer;
	}
	bool isFull() const
	{
		return avail == 0;
	}
	void push(u8 data)
	{
		if (avail)
		{
			buffer[head++] = data;
			if (head == size) 
				head = 0;
			--avail;
		}
	}
	bool isEmpty() const
	{
		return avail == size;
	}
	int getUsed() const
	{
		return size - avail;
	}
	u8 pop()
	{
		if (avail != size)
		{
			++avail;
			u8 result = buffer[tail++];
			if (tail == size)
				tail = 0;
			return result;
		}
		else
			return 128;
	}
private:
	int head, tail, size, avail;
	u8 *buffer;
};

struct avr8
{
	avr8() : pc(0), cycleCounter(0), singleStep(0), nextSingleStep(0), interruptLevel(0), breakpoint(0xFFFF), audioRing(2048), 
		enableSound(true), fullscreen(false), interlaced(false), lastFlip(0), inset(0), prevPortB(0), 
		prevWDR(0), frameCounter(0), new_input_mode(false), eepromDirty(false), eepromName("eeprom.hex")
	{
		memset(r, 0, sizeof(r));
		memset(io, 0, sizeof(io));
		memset(sram, 0, sizeof(sram));
		progmem = new u16[progSize/2];
		eeprom = new u8[eepromSize];
		memset(progmem,0,progSize);

		PIND = 8;
#if GUI
		pad_mode = SNES_PAD;
#endif
	}
	~avr8()
	{
		delete[] progmem;
		delete[] eeprom;
	}

	void shutdown()
	{
		if (eepromDirty)
			save_hex_eeprom();
	}

	u8 *eeprom;
	u16 *progmem;
	u16 progmemLast;		// size of the last file
	u16 pc;
	u16 breakpoint;
	u8 TEMP;				// for 16-bit timers
	u32 cycleCounter, prevPortB, prevWDR;
	bool singleStep, nextSingleStep, enableSound, fullscreen, framelock, interlaced,
		new_input_mode, eepromDirty;
	int interruptLevel;
	u32 lastFlip;
	const char *eepromName;
	u32 inset;
#if GUI
	SDL_Surface *screen;
	int sdl_flags;
	int frameCounter;
	int scanline_count;
	int current_cycle;
	int scanline_top;
	int left_edge;
	u32 *current_scanline, *next_scanline;
	u32 pixel;
	u32 palette[256];
	// SNES bit order:  B, Y, Select, Start, Up, Down, Left, Right, A, X, L, R
	// NES bit order:  A, B, Select, Start, Up, Down, Left, Right
	u32 buttons[2], latched_buttons[2];
	int mouse_scale;
	enum { NES_PAD, SNES_PAD, SNES_PAD2, SNES_MOUSE } pad_mode;

	void audio_callback(Uint8 *stream,int len);
	static void audio_callback_stub(void *userdata, Uint8 *stream, int len)
	{
		((avr8*)userdata)->audio_callback(stream,len);
	}
#endif
	ringBuffer audioRing;

	struct
	{
		union 
		{
			u8 r[32];		// Register file
			struct
			{
				u8 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15;
				u8 r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, XL, XH, YL, YH, ZL, ZH;
			};
		};
		union
		{
			u8 io[256-32];		// Direct-mapped I/O space
			struct 
			{
				u8 PINA,  DDRA,  PORTA, PINB,  DDRB,  PORTB, PINC,  DDRC;
				u8 PORTC, PIND,  DDRD,  PORTD, res2C, res2D, res2E, res2F;
				u8 res30, res31, res32, res33, res34, TIFR0, TIFR1, TIFR2;
				u8 res38, res39, res3A, PCIFR, EIFR,  EIMSK, GPIOR0,EECR;
				u8 EEDR,  EEARL, EEARH, GTCCR, TCCR0A,TCCR0B,TCNT0, OCR0A;
				u8 OCR0B, res49, GPIOR1,GPIOR2,SPCR,  SPSR,  SPDR,  res4f;
				u8 ACSR,  OCDR,  res52, SMCR,  MCUSR, MCUCR, res56, SPMCSR;
				u8 res58, res59, res5A, res5B, res5C, SPL,   SPH,   SREG;
				u8 WDTCSR, CLKPR, res62, res63, PRR, res65, OSCCAL, res67;
				u8 PCICR, EICRA, res6a, PCMSK0, PCMSK1, PCMSK2, TIMSK0, TIMSK1;
				u8 TIMSK2, res71, res72, PCMSK3, res74, res75, res76, res77;
				u8 ADCL, ADCH, ADCSRA, ADCSRB, ADMUX, res7d, DIDR0, DIDR1;
				u8 TCCR1A, TCCR1B, TCCR1C, res83, TCNT1L, TCNT1H, ICR1L, ICR1H;
				u8 OCR1AL, OCR1AH, OCR1BL, OCR1BH, res8c, res8d, res8e, res8f;
				u8 res90, res91, res92, res93, res94, res95, res96, res97;
				u8 res98, res99, res9a, res9b, res9c, res9d, res9e, res9f;
				u8 resa0, resa1, resa2, resa3, resa4, resa5, resa6, resa7;
				u8 resa8, resa9, resaa, resab, resac, resad, resae, resaf;
				u8 TCCR2A, TCCR2B, TCNT2, OCR2A, OCR2B, resb5, ASSR, resb7;
				u8 TWBR, TWSR, TWAR, TWDR, TWCR, TWAMR, resbe, resbf;
				u8 UCSR0A, UCSR0B, UCSR0C, resc3, UBRR0L, UBRR0H, UDR0, resc7;
			};
		};
		u8 sram[sramSize];
	};

	void write_io(u8 addr,u8 value);
	u8 read_io(u8 addr);

	u8 read_progmem(u16 addr)
	{
		if ((addr>>1) >= progmemLast)
		{
			fprintf(stderr,"illegal read from progmem addr %x (ignored)\n",addr);
			return 0;
		}
		u16 word = progmem[addr>>1];
		return (addr&1)? word>>8 : word;
	}

	void write_sram(u16 addr,u8 value)
	{
		if (addr < IOBASE)
			r[addr] = value;		// Write a register
		else if (addr >= IOBASE && addr < SRAMBASE)
			write_io(addr - IOBASE, value);
		else {
			if (addr >= SRAMBASE + sramSize)
				printf("illegal write of %x to addr %x, pc = %x\n",value,addr,pc-1);
			sram[(addr - SRAMBASE) & (sramSize-1)] = value;
		}
	}

	u8 read_sram(u16 addr)
	{
		if (addr < IOBASE)
			return r[addr];		// Read a register
		else if (addr >= IOBASE && addr < SRAMBASE)
			return read_io(addr - IOBASE);
		else {
			if (addr >= SRAMBASE + sramSize)
				printf("illegal read from addr %x, pc = %x\n",addr,pc-1);
			return sram[(addr - SRAMBASE) & (sramSize-1)];
		}
	}

	static int get_insn_size(u16 insn)
	{
		/*	1001 000d dddd 0000		LDS Rd,k (next word is rest of address)
		1001 001d dddd 0000		STS k,Rr (next word is rest of address)
		1001 010k kkkk 110k		JMP k (next word is rest of address)
		1001 010k kkkk 111k		CALL k (next word is rest of address) */
		// This code is simplified by assuming upper k bits are zero on 644
		insn &= 0xFE0F;
		if (insn == 0x9000 || insn == 0x9200 || insn == 0x940C || insn == 0x940E)
			return 2;
		else
			return 1;
	}

#if GUI
	bool init_gui();
	void handle_key_down(SDL_Event &ev);
	void handle_key_up(SDL_Event &ev);
	void update_buttons(int key,bool down);
#endif

	void trigger_interrupt(int location);

	u8 exec(bool disasmOnly,bool verbose);

	void update_hardware(int cycles);

	bool load_hex_common(const char *filename,void *dest,int destSize,bool isWord);
	bool save_hex_common(const char *filename,const u8 *src,int srcSize);

	bool load_hex_progmem(const char *filename);
	bool load_hex_eeprom();
	bool save_hex_eeprom();
};


/*
K and q are unsigned; k is signed when less than 16 bits

0000 0000 0000 0000		NOP
0000 0001 dddd rrrr		MOVW Rd+1:Rd,Rr+1:R
0000 0010 dddd rrrr		MULS Rd,Rr
0000 0011 0ddd 0rrr		MULSU Rd,Rr (registers are in 16-23 range)
0000 0011 0ddd 1rrr		FMUL Rd,Rr (registers are in 16-23 range)
0000 0011 1ddd 0rrr		FMULS Rd,Rr
0000 0011 1ddd 1rrr		FMULSU Rd,Rr
0000 01rd dddd rrrr		CPC Rd,Rr
0000 10rd dddd rrrr		SBC Rd,Rr
0000 11rd dddd rrrr		ADD Rd,Rr (LSL is ADD Rd,Rd)
0001 00rd dddd rrrr		CPSE Rd,Rr
0001 01rd dddd rrrr		CP Rd,Rr
0001 10rd dddd rrrr		SUB Rd,Rr
0001 11rd dddd rrrr		ADC Rd,Rr (ROL is ADC Rd,Rd)
0010 00rd dddd rrrr		AND Rd,Rr (TST is AND Rd,Rd)
0010 01rd dddd rrrr		EOR Rd,Rr (CLR is EOR Rd,Rd)
0010 10rd dddd rrrr		OR Rd,Rr
0010 11rd dddd rrrr		MOV Rd,Rr
0011 KKKK dddd KKKK		CPI Rd,K
0100 KKKK dddd KKKK		SBCI Rd,K
0101 KKKK dddd KKKK		SUBI Rd,K
0110 KKKK dddd KKKK		ORI Rd,K (same as SBR insn)
0111 KKKK dddd KKKK		ANDI Rd,K (CBR is ANDI with K complemented)
1001 000d dddd 0000		LDS Rd,k (next word is rest of address)
1001 000d dddd 0001		LD Rd,Z+
1001 000d dddd 0010		LD Rd,-Z
1001 000d dddd 0100		LPM Rd,Z
1001 000d dddd 0101		LPM Rd,Z+
1001 000d dddd 0110		ELPM Rd,Z
1001 000d dddd 0111		ELPM Rd,Z+
1001 000d dddd 1001		LD Rd,Y+
1001 000d dddd 1010		LD Rd,-Y
1001 000d dddd 1100		LD rd,X
1001 000d dddd 1101		LD rd,X+
1001 000d dddd 1110		LD rd,-X
1001 000d dddd 1111		POP Rd
1001 001d dddd 0000		STS k,Rr (next word is rest of address)
1001 001r rrrr 0001		ST Z+,Rr
1001 001r rrrr 0010		ST -Z,Rr
1001 001r rrrr 1001		ST Y+,Rr
1001 001r rrrr 1010		ST -Y,Rr
1001 001r rrrr 1100		ST X,Rr
1001 001r rrrr 1101		ST X+,Rr
1001 001r rrrr 1110		ST -X,Rr
1001 001d dddd 1111		PUSH Rd
1001 0100 0000 1001		IJMP (jump thru Z register)
1001 0100 0001 1001		EIJMP (probably not on 644)
1001 0100 0sss 1000		BSET s (SEC, etc are aliases with sss implicit)
1001 0100 1sss 1000		BCLR s (CLC, etc are aliases with sss implicit)
1001 0100 KKKK 1011		DES (probably not on 644)
1001 0101 0000 1000		RET
1001 0101 0000 1001		ICALL (call thru Z register)
1001 0101 0001 1000		RETI
1001 0101 0001 1001		EICALL (probably not on 644)
1001 0101 1000 1000		SLEEP
1001 0101 1001 1000		BREAK
1001 0101 1010 1000		WDR
1001 0101 1100 1000		LPM (r0 implied, why is this special?)
1001 0101 1101 1000		ELPM (r0 implied)
1001 0101 1110 1000		SPM Z+ (writes R1:R0; in some cases it won't postinc?)
1001 0101 1111 1000		SPM Z+
1001 010d dddd 0000		COM Rd
1001 010d dddd 0001		NEG Rd
1001 010d dddd 0010		SWAP Rd
1001 010d dddd 0011		INC Rd
1001 010d dddd 0101		ASR Rd
1001 010d dddd 0110		LSR Rd
1001 010d dddd 0111		ROR Rd
1001 010d dddd 1010		DEC Rd
1001 010k kkkk 110k		JMP k (next word is rest of address)
1001 010k kkkk 111k		CALL k (next word is rest of address)
1001 0110 KKdd KKKK		ADIW Rd+1:Rd,K   (16-bit add to upper four register pairs)
1001 0111 KKdd KKKK		SBIW Rd+1:Rd,K
1001 1000 AAAA Abbb		CBI A,b
1001 1001 AAAA Abbb		SBIC A,b
1001 1010 AAAA Abbb		SBI A,b
1001 1011 AAAA Abbb		SBIS A,b
1001 11rd dddd rrrr		MUL Rd,Rr
1011 0AAd dddd AAAA		IN Rd,A
1011 1AAr rrrr AAAA		OUT A,Rr
10q0 qq0d dddd 0qqq		LD Rd,Z+q
10q0 qq0d dddd 1qqq		LD Rd,Y+q
10q0 qq1r rrrr 0qqq		ST Z+q,Rr
10q0 qq1r rrrr 1qqq		ST Y+q,Rr
1100 kkkk kkkk kkkk		RJMP k
1101 kkkk kkkk kkkk		RCALL k
1110 KKKK dddd KKKK		LDI Rd,K (SER is just LDI Rd,255)
1111 00kk kkkk ksss		BRBS s,k (same here)
1111 01kk kkkk ksss		BRBC s,k (BRCC, etc are aliases for this with sss implicit)
1111 100d dddd 0bbb		BLD Rd,b
1111 101d dddd 0bbb		BST Rd,b
1111 110r rrrr 0bbb		SBRC Rr,b
1111 111r rrrr 0bbb		SBRS Rr,b
*/

#define D3	((insn >> 4) & 7)
#define R3	(insn & 7)
#define D4	((insn >> 4) & 15)
#define R4	(insn & 15)
#define R5	((insn & 15) | ((insn >> 5) & 0x10))
#define D5	((insn >> 4) & 31)
#define K8	(((insn >> 4) & 0xF0) | (insn & 0xF))
#define k7	((s16)(insn<<6)>>9)
#define k12	((s16)(insn<<4)>>4)

#define BIT(x,b)	(((x)>>(b))&1)
#define C			BIT(SREG,SREG_C)

inline void set_bit(u8 &dest,int bit,int value)
{
	if (value)
		dest |= (1<<bit);
	else
		dest &= ~(1<<bit);

}

// This computes both the half-carry (bit3) and full carry (bit7)
#define BORROWS		(~Rd&Rr)|(Rr&R)|(R&~Rd)
#define CARRIES		((Rd&Rr)|(Rr&~R)|(~R&Rd))

#define UPDATE_HC_SUB \
	CH = BORROWS; \
	set_bit(SREG, SREG_H, CH & 0x8); \
	set_bit(SREG, SREG_C, CH & 0x80)
#define UPDATE_HC_ADD \
	CH = CARRIES; \
	set_bit(SREG, SREG_H, CH & 0x8); \
	set_bit(SREG, SREG_C, CH & 0x80)

#define UPDATE_H		set_bit(SREG, SREG_H, (CARRIES & 0x8))
#define UPDATE_Z		set_bit(SREG, SREG_Z, !R)
#define UPDATE_V_ADD	set_bit(SREG, SREG_V, ((Rd&Rr&~R)|(~Rd&~Rr&R)) & 0x80)
#define UPDATE_V_SUB	set_bit(SREG, SREG_V, ((Rd&~Rr&~R)|(~Rd&Rr&R)) & 0x80)
#define UPDATE_N		set_bit(SREG, SREG_N, R & 0x80)
#define UPDATE_S		set_bit(SREG, SREG_S, BIT(SREG,SREG_N) ^ BIT(SREG,SREG_V))

#define UPDATE_SVN_SUB	UPDATE_V_SUB; UPDATE_N; UPDATE_S
#define UPDATE_SVN_ADD	UPDATE_V_ADD; UPDATE_N; UPDATE_S

// Simplified version for logical insns.
#define UPDATE_SVN_LOGICAL \
	if (R & 0x80) { /* Set N and S, clear V */ \
		SREG = (SREG | (1<<SREG_N) | (1<<SREG_S)) & ~(1<<SREG_V); \
	} else { /* Clear N and S and V */ \
		SREG = (SREG & ~((1<<SREG_S)|(1<<SREG_V)|(1<<SREG_N))); \
	}

#define UPDATE_CZ_MUL(x)		set_bit(SREG,SREG_C,x & 0x8000); set_bit(SREG,SREG_Z,!x)

#define CLEAR_Z		(SREG &= ~(1<<SREG_Z))
#define SET_C		(SREG |= (1<<SREG_C))

#define ILL if (!disasmOnly) { fprintf(stderr,"invalid insn %x\n",insn); shutdown(); exit(1); }

#if defined(_DEBUG) && 0
#define DISASM 1
#define DIS(fmt,...)	sprintf(insnBuf,fmt,##__VA_ARGS__); if (disasmOnly) break
#else
#define DISASM 0
#define DIS(fmt,...)
#endif

static const char brbc[8][5] = {"BRCC", "BRNE", "BRPL", "BRVC", "BRGE", "BRHC", "BRTC", "BRID"};
static const char brbs[8][5] = {"BRCS", "BREQ", "BRMI", "BRVS", "BRLT", "BRHS", "BRTS", "BRIE"};

static const char *reg_pair(int reg)
{
	static const char names[16][8] = {
		"r1:r0", "r3:r2", "r5:r4", "r7:r6", "r9:r8", "r11:r10", "r13:r12", "r15:r14",
		"r17:r16", "r19:r18", "r21:r20", "r23:r22", "r25:r24", "X", "Y", "Z" };
	return names[reg>>1];
}

static const char *reg_name(int reg)
{
	static const char names[32][4]  = {
		"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
		"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
		"r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
		"r24", "r25", "XL", "XH", "YL", "YH", "ZL", "ZH" };
	return names[reg];
}

static const char *port_name(int port)
{
	static const char names[][8] = {
		"PINA",  "DDRA",  "PORTA", "PINB",  "DDRB",  "PORTB", "PINC",  "DDRC",
		"PORTC", "PIND",  "DDRD",  "PORTD", "res2C", "res2D", "res2E", "res2F",
		"res30", "res31", "res32", "res33", "res34", "TIFR0", "TIFR1", "TIFR2",
		"res38", "res39", "res3A", "PCIFR", "EIFR",  "EIMSK", "GPIOR0","EECR",
		"EEDR",  "EEARL", "EEARH", "GTCCR", "TCCR0A","TCCR0B","TCNT0", "OCR0A",
		"OCR0B", "res49", "GPIOR1","GPIOR2","SPCR",  "SPSR",  "SPDR",  "res4f",
		"ACSR",  "OCDR",  "res52", "SMCR",  "MCUSR", "MCUCR", "res56", "SPMCSR",
		"res58", "res59", "res5A", "res5B", "res5C", "SPL",   "SPH",  "SREG",
		"WDTCSR", "CLKPR", "res62", "res63", "PRR", "res65", "OSCCAL", "res67",
		"PCICR", "EICRA", "res6a", "PCMSK0", "PCMSK1", "PCMSK2", "TIMSK0", "TIMSK1",
		"TIMSK2", "res71", "res72", "PCMSK3", "res74", "res75", "res76", "res77",
		"ADCL", "ADCH", "ADCSRA", "ADCSRB", "ADMUX", "res7d", "DIDR0", "DIDR1",
		"TCCR1A", "TCCR1B", "TCCR1C", "res83", "TCNT1L", "TCNT1H", "ICR1L", "ICR1H",
		"OCR1AL", "OCR1AH", "OCR1BL", "OCR1BH", "res8c", "res8d", "res8e", "res8f",
		"res90", "res91", "res92", "res93", "res94", "res95", "res96", "res97",
		"res98", "res99", "res9a", "res9b", "res9c", "res9d", "res9e", "res9f",
		"resa0", "resa1", "resa2", "resa3", "resa4", "resa5", "resa6", "resa7",
		"resa8", "resa9", "resaa", "resab", "resac", "resad", "resae", "resaf",
		"TCCR2A", "TCCR2B", "TCNT2", "OCR2A", "OCR2B", "resb5", "ASSR", "resb7",
		"TWBR", "TWSR", "TWAR", "TWDR", "TWCR", "TWAMR", "resbe", "resbf",
		"UCSR0A", "UCSR0B", "UCSR0C", "resc3", "UBRR0L", "UBRR0H", "UDR0", "resc7",
	};
	
	return names[port];
}

static const char *addr_or_port_name(int addr)
{
	static char temp[16];
	if (addr >= IOBASE && addr < SRAMBASE)
		sprintf(temp,"@%s",port_name(addr-IOBASE));
	else
		sprintf(temp,"$%04x",addr);
	return temp;
}

#if GUI
static u8 encode_delta(int d)
{
	u8 result;
	if (d < 0)
	{
		result = 0;
		d = -d;
	}
	else
		result = 1;
	if (d > 127)
		d = 127;
	if (!(d & 64))
		result |= 2;
	if (!(d & 32))
		result |= 4;
	if (!(d & 16))
		result |= 8;
	if (!(d & 8))
		result |= 16;
	if (!(d & 4))
		result |= 32;
	if (!(d & 2))
		result |= 64;
	if (!(d & 1))
		result |= 128;
	return result;
}
#endif

void avr8::write_io(u8 addr,u8 value)
{
	// p106 in 644 manual; 16-bit values are latched
	if (addr == ports::TCNT1H || addr == ports::ICR1H)
		TEMP = value;
	else if (addr == ports::TCNT1L || addr == ports::ICR1L)
	{
		io[addr] = value;
		io[addr+1] = TEMP;
	}
	else if (addr == ports::EECR)
	{
		// EEPM1   5
		// EEPM0   4
		// EERIE   3
		// EEMPE   2
		// EEPE    1
		// EERE    0
		u8 changed = io[addr] ^ value;
		u8 went_hi = changed & value;

		// If EEPE went hi, and EEMPE is already high...
		if (went_hi == (1<<1) && (io[addr] & (1<<2)))
		{
			u16 addr = (EEARL | (EEARH << 8)) & (eepromSize-1);
			if (eeprom[addr] != EEDR)
			{
				eeprom[addr] = EEDR;
				eepromDirty = true;
				printf("EEPROM: wrote %02x to %04x\n",EEDR,addr);
			}
			// Turn off EEMPE (and never turn on EEPE)
			io[addr] = io[addr] & ~(1<<2);
		}
		// If EERE went high...
		else if (went_hi == (1<<0))
		{
			u16 addr = (EEARL | (EEARH << 8)) & (eepromSize-1);
			EEDR = eeprom[addr];
			printf("EEPROM: read %02x from %04x\n",EEDR,addr);
			// (ignore the write)
		}
		else
			io[addr] = value;
	}
#if GUI
	else if (addr == ports::PORTB)
	{
		// printf("PORTB becomes %d at cycle %u (%u since last write)\n",value,cycleCounter,cycleCounter-prev);
		// prev = cycleCounter;
		u32 elapsed = cycleCounter - prevPortB;
		prevPortB = cycleCounter;
		if (scanline_count == -999 && value && elapsed >= 774 - 4 && elapsed <= 774 + 4)
			scanline_count = scanline_top;
		else if (scanline_count != -999 && value) {
			++scanline_count;
			current_cycle = left_edge;

			current_scanline = (u32*)((u8*)screen->pixels + scanline_count * 2 * screen->pitch + inset);
			if (interlaced)
			{
				if (frameCounter & 1)
					current_scanline += (screen->pitch>>2);
				next_scanline = current_scanline;
			}
			else
				next_scanline = current_scanline + (screen->pitch>>2);

			if (scanline_count == 224) 
			{
				if (SDL_MUSTLOCK(screen))
					SDL_UnlockSurface(screen);
				if (frameCounter & 1)
					SDL_Flip(screen);
				// framelock no longer necessary, audio buffer full takes care of it for us.

				SDL_Event event;
				while (singleStep? SDL_WaitEvent(&event) : SDL_PollEvent(&event))
				{
					if (event.type == SDL_KEYDOWN)
						handle_key_down(event);
					else if (event.type == SDL_KEYUP)
						handle_key_up(event);
					else if (event.type == SDL_QUIT)
					{
						printf("user abort (closed window).\n");
						shutdown();
						exit(0);
					}
				}
				if (pad_mode == SNES_MOUSE) 
				{
					// http://www.repairfaq.org/REPAIR/F_SNES.html
					// we always report "low sensitivity"
					int mouse_dx, mouse_dy;
					u8 mouse_buttons = SDL_GetRelativeMouseState(&mouse_dx,&mouse_dy);
					mouse_dx >>= mouse_scale;
					mouse_dy >>= mouse_scale;
					// clear high bit so we know it's the mouse
					buttons[0] = (encode_delta(mouse_dx) << 24)
						| (encode_delta(mouse_dy) << 16) | 0x7FFF;
					if (mouse_buttons & SDL_BUTTON_LMASK)
						buttons[0] &= ~(1<<9);
					if (mouse_buttons & SDL_BUTTON_RMASK)
						buttons[0] &= ~(1<<10);
					// keep mouse centered so it doesn't get stuck on edge of screen.
					// ...and immediately consume the bogus motion event it generated.
					if (fullscreen)
					{
						SDL_WarpMouse(400,300);
						SDL_GetRelativeMouseState(&mouse_dx,&mouse_dy);
					}
				}
				else
					buttons[0] |= 0xFFFF8000;
				singleStep = nextSingleStep;

				if (SDL_MUSTLOCK(screen))
					SDL_LockSurface(screen);
				scanline_count = -999;
				++frameCounter;
			}

		}
	}
	else if (addr == ports::PORTA)
	{
		u8 changed = value ^ io[addr];
		u8 went_low = changed & io[addr];
		// u8 went_hi = changed & value;
		if (went_low == (1<<2))		// LATCH
		{
			for (int i=0; i<2; i++)
			{
				latched_buttons[i] = buttons[i];
				// don't let UP+DOWN register at same time
				if ((latched_buttons[i] & ((1<<PAD_LEFT)|(1<<PAD_RIGHT))) == 0)
					latched_buttons[i] |= (1<<PAD_RIGHT);
				// same for LEFT+RIGHT
				if ((latched_buttons[i] & ((1<<PAD_UP)|(1<<PAD_DOWN))) == 0)
					latched_buttons[i] |= (1<<PAD_DOWN);
				// printf("LATCH latched_buttons = %x\n",latched_buttons);
			}
		}
		else if (went_low == (1<<3))	// CLOCK
		{
			if (new_input_mode)
				PINA = u8((latched_buttons[0] & 1) | ((latched_buttons[1] & 1) << 1));
			latched_buttons[0] >>= 1;
			latched_buttons[1] >>= 1;
			if ((latched_buttons[1] < 0xFFFFF) && !new_input_mode)
			{
				printf("new input routines detected, switching emulation method.\n");
				new_input_mode = true;
			}
		}
		if (!new_input_mode)
			PINA = u8((latched_buttons[0] & 1) | ((latched_buttons[1] & 1) << 1));

		// printf("(writing %x to PORTA pc = %x) setting PINA to %x\n",value,pc-1,PINA);
		io[addr] = value;
	}
	else if (addr == ports::OCR2A)
	{
		if (enableSound && TCCR2B)
		{
			// raw pcm sample at 15.7khz
			while (audioRing.isFull())
				SDL_Delay(1);
			SDL_LockAudio();
			audioRing.push(value);
			SDL_UnlockAudio();
		}
	}
	else if (addr == ports::PORTC)
	{
		pixel = palette[value];
	}
#endif	// GUI
	else
		io[addr] = value;
}


u8 avr8::read_io(u8 addr)
{
	// p106 in 644 manual; 16-bit values are latched
	// if (addr == ports::PORTA || addr == ports::PINA)
	//	printf("reading port %s (%x) pc = %x\n",port_name(addr),io[addr],pc-1);
	if (addr == ports::TCNT1L || addr == ports::ICR1L)
	{
		TEMP = io[addr+1];
		return io[addr];
	}
	else if (addr == ports::TCNT1H || addr == ports::ICR1H)
		return TEMP;
	else
		return io[addr];
}



u8 avr8::exec(bool disasmOnly,bool verbose)
{
	if (pc >= progmemLast) {
		fprintf(stderr,"illegal pc %x caught\n",pc);
		shutdown();
		exit(1);
	}

	if (pc == breakpoint)
		singleStep = nextSingleStep = true;

	// set a breakpoint on the printf to halt at a given address.
	// if (pc == 0x33d3)
	//	printf("hit\n");

	u16 lastpc = pc;
	u16 insn = progmem[pc++];	
	u8 cycles = 1;				// Most insns run in one cycle, so assume that
	u8 Rd, Rr, R, d, CH;
	u16 uTmp, Rd16, R16;
	s16 sTmp;
#if DISASM
	char insnBuf[32];
	sprintf(insnBuf,"?$%04x",insn);
#endif

	// The "DIS" macro must be first, or at least before any side effects on machine state occur.  
	// This is because depending on the configuration, we can build one of three ways; no 
	// disassembly; integrated disassembly and emulation, and disassembly only.  (ie the DIS 
	// macro may just check a flag, do the sprintf, and exit)
	switch (insn >> 12) 
	{
	case 0:
	  /*0000 0000 0000 0000		NOP
		0000 0001 dddd rrrr		MOVW Rd+1:Rd,Rr+1:R
		0000 0010 dddd rrrr		MULS Rd,Rr
		0000 0011 0ddd 0rrr		MULSU Rd,Rr (registers are in 16-23 range)
		0000 0011 0ddd 1rrr		FMUL Rd,Rr (registers are in 16-23 range)
		0000 0011 1ddd 0rrr		FMULS Rd,Rr
		0000 0011 1ddd 1rrr		FMULSU Rd,Rr
		0000 01rd dddd rrrr		CPC Rd,Rr
		0000 10rd dddd rrrr		SBC Rd,Rr
		0000 11rd dddd rrrr		ADD Rd,Rr (LSL is ADD Rd,Rd)*/
		switch(insn >> 8)
		{
		case 0: /*NOP*/; 
			DIS("NOP");
			break;
		case 1: /*MOVW*/ 
			// Don't use tab because the operand is really wide
			DIS("MOVW   %s,%s",reg_pair(D4*2),reg_pair(R4*2));
			Rd = D4 << 1; 
			Rr = R4 << 1; 
			r[Rd] = r[Rr]; 
			r[Rd+1] = r[Rr+1]; 
			break;
		case 2: /*MULS*/ 
			DIS("MULS   %s,%s",reg_name(D4+16),reg_name(R4+16));
			Rd = r[D4 + 16]; 
			Rr = r[R4 + 16];
			sTmp = (s8)Rd * (s8)Rr; 
			r0 = (u8)sTmp; 
			r1 = (u8)(sTmp >> 8);
			UPDATE_CZ_MUL(sTmp);
			cycles=2;
			break;
		case 3: /*MULSU/FMULS/FMULSU*/ 
			switch (insn & 0x88)
			{
			case 0x00:
				DIS("MULSU  %s,%s",reg_name(D3+16),reg_name(R3+16));
				Rd = r[D3 + 16];
				Rr = r[R3 + 16]; 
				sTmp = (s8)Rd * (u8)Rr; 
				r0 = (u8)sTmp; 
				r1 = (u8)(sTmp >> 8);
				UPDATE_CZ_MUL(sTmp);
				cycles=2;
				break;
			case 0x08:
				DIS("FMUL %s,%s",reg_name(D3+16),reg_name(R3+16));
				Rd = r[D3 + 16];
				Rr = r[R3 + 16]; 
				uTmp = (u8)Rd * (u8)Rr; 
				r0 = (u8)(uTmp << 1); 
				r1 = (u8)(uTmp >> 7);
				UPDATE_CZ_MUL(uTmp);
				cycles=2;
				break;
			case 0x80:
				DIS("FMULS  %s,%s",reg_name(D3+16),reg_name(R3+16));
				Rd = r[D3 + 16];
				Rr = r[R3 + 16]; 
				sTmp = (s8)Rd * (s8)Rr; 
				r0 = (u8)(sTmp << 1); 
				r1 = (u8)(sTmp >> 7);
				UPDATE_CZ_MUL(sTmp);
				cycles=2;
				break;
			case 0x88:
				DIS("FMULSU  %s,%s",reg_name(D3+16),reg_name(R3+16));
				Rd = r[D3 + 16];
				Rr = r[R3 + 16]; 
				sTmp = (s8)Rd * (u8)Rr; 
				r0 = (u8)(sTmp << 1); 
				r1 = (u8)(sTmp >> 7);
				UPDATE_CZ_MUL(sTmp);
				cycles=2;
				break;
			}
			break;
		case 4: case 5: case 6: case 7: /*CPC*/
			DIS("CPC    %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[D5];
			Rr = r[R5];
			R = Rd - Rr - C;
			UPDATE_HC_SUB; UPDATE_SVN_SUB; if (R) CLEAR_Z;
			break;
		case 8: case 9: case 10: case 11: /*SBC*/
			DIS("SBC    %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[d = D5];
			Rr = r[R5];
			R = Rd - Rr - C;
			UPDATE_HC_SUB; UPDATE_SVN_SUB; if (R) CLEAR_Z;
			r[d] = R;
			break;
		case 12: case 13: case 14: case 15: /*ADD*/
			DIS(D5==R5?"LSL    %s":"ADD    %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[d = D5];
			Rr = r[R5];
			R = Rd + Rr;
			UPDATE_HC_ADD; UPDATE_SVN_ADD; UPDATE_Z; 
			r[d] = R;
			break;
		}
		break;
	case 1:
	  /*0001 00rd dddd rrrr		CPSE Rd,Rr
		0001 01rd dddd rrrr		CP Rd,Rr
		0001 10rd dddd rrrr		SUB Rd,Rr
		0001 11rd dddd rrrr		ADC Rd,Rr (ROL is ADC Rd,Rd)*/
		switch ((insn >> 10) & 3)
		{
		case 0: /*CPSE*/
			DIS("CPSE   %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[D5];
			Rr = r[R5];
			if (Rd == Rr)
			{
				uTmp = get_insn_size(progmem[pc]);
				cycles += uTmp;
				pc += uTmp;
			}
			break;
		case 1: /*CP*/
			DIS("CP     %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[D5];
			Rr = r[R5];
			R = Rd - Rr;
			UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_Z;
			break;
		case 2: /*SUB*/
			DIS("SUB    %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[d = D5];
			Rr = r[R5];
			R = Rd - Rr;
			UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_Z;
			r[d] = R;
			break;
		case 3: /*ADC*/
			DIS(D5==R5?"ROL    %s":"ADC    %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[d = D5];
			Rr = r[R5];
			R = Rd + Rr + C;
			UPDATE_HC_ADD; UPDATE_SVN_ADD; UPDATE_Z; 
			r[d] = R;
			break;
		}
		break;
	case 2:
	  /*0010 00rd dddd rrrr		AND Rd,Rr (TST is AND Rd,Rd)
		0010 01rd dddd rrrr		EOR Rd,Rr (CLR is EOR Rd,Rd)
		0010 10rd dddd rrrr		OR Rd,Rr
		0010 11rd dddd rrrr		MOV Rd,Rr*/
		switch ((insn >> 10) & 3)
		{
		case 0: /*AND*/
			DIS(D5==R5?"TST    %s":"AND    %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[d = D5];
			Rr = r[R5];
			R = Rd & Rr;
			UPDATE_SVN_LOGICAL; UPDATE_Z;
			r[d] = R;
			break;
		case 1: /*EOR*/
			DIS(D5==R5?"CLR    %s":"EOR    %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[d = D5];
			Rr = r[R5];
			R = Rd ^ Rr;
			UPDATE_SVN_LOGICAL; UPDATE_Z;
			r[d] = R;
			break;
		case 2: /*OR*/
			DIS("OR     %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[d = D5];
			Rr = r[R5];
			R = Rd | Rr;
			UPDATE_SVN_LOGICAL; UPDATE_Z;
			r[d] = R;
			break;
		case 3: /*MOV*/
			DIS("MOV    %s,%s",reg_name(D5),reg_name(R5));
			r[D5]  = r[R5];
			break;
		}
		break;
	case 3: /*0011 KKKK dddd KKKK		CPI Rd,K */
		DIS("CPI    %s,$%x",reg_name(D4+16),K8);
		Rd = r[D4 + 16];
		Rr = K8;
		R = Rd - Rr;
		UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_Z;
		break;
	case 4: /*0100 KKKK dddd KKKK		SBCI Rd,K */
		DIS("SBCI   %s,%d",reg_name(D4+16),K8);
		Rd = r[d = D4 + 16];
		Rr = K8;
		R = Rd - Rr - C;
		UPDATE_HC_SUB; UPDATE_SVN_SUB; if (R) CLEAR_Z;
		r[d] = R;
		break;
	case 5: /*0101 KKKK dddd KKKK		SUBI Rd,K */
		DIS("SUBI   %s,%d",reg_name(D4+16),K8);
		Rd = r[d = D4 + 16];
		Rr = K8;
		R = Rd - Rr;
		UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_Z;
		r[d] = R;
		break;
	case 6: /*0110 KKKK dddd KKKK		ORI Rd,K (same as SBR insn) */
		DIS("ORI    %s,%d",reg_name(D4+16),K8);
		Rd = r[d = D4 + 16];
		Rr = K8;
		R = Rd | Rr;
		UPDATE_SVN_LOGICAL; UPDATE_Z;
		r[d] = R;
		break;
	case 7: /*0111 KKKK dddd KKKK		ANDI Rd,K (CBR is ANDI with K complemented) */
		DIS("ANDI   %s,%d",reg_name(D4+16),K8);
		Rd = r[d = D4 + 16];
		Rr = K8;
		R = Rd & Rr;
		UPDATE_SVN_LOGICAL; UPDATE_Z;
		r[d] = R;
		break;
	case 8: case 10:
	  /*10q0 qq0d dddd 0qqq		LD Rd,Z+q
		10q0 qq0d dddd 1qqq		LD Rd,Y+q
		10q0 qq1d dddd 0qqq		ST Z+q,Rd
		10q0 qq1d dddd 1qqq		ST Y+q,Rd */
		Rd = D5;
		Rr = (insn & 7) | ((insn >> 7) & 0x18) | ((insn >> 8) & 0x20);
		uTmp = ((insn & 0x8)? Y : Z) + Rr;
		if (insn & 0x200)	// ST
		{
			DIS("ST     %c+%d,%s",insn&0x8?'Y':'Z',Rr,reg_name(Rd));
			write_sram(uTmp, r[Rd]);
		}
		else
		{
			DIS("LD     %s,%c+%d",reg_name(Rd),insn&0x8?'Y':'Z',Rr);
			r[Rd] = read_sram(uTmp);
		}
		cycles=2;
		break;
	case 9:
		switch ((insn>>8) & 15)
		{
		case 0: case 1:
		  /*1001 000d dddd 0000		LDS Rd,k (next word is rest of address)
			1001 000d dddd 0001		LD Rd,Z+
			1001 000d dddd 0010		LD Rd,-Z
			1001 000d dddd 0100		LPM Rd,Z
			1001 000d dddd 0101		LPM Rd,Z+
			1001 000d dddd 0110		ELPM Rd,Z
			1001 000d dddd 0111		ELPM Rd,Z+
			1001 000d dddd 1001		LD Rd,Y+
			1001 000d dddd 1010		LD Rd,-Y
			1001 000d dddd 1100		LD rd,X
			1001 000d dddd 1101		LD rd,X+
			1001 000d dddd 1110		LD rd,-X
			1001 000d dddd 1111		POP Rd */
			switch (insn & 15)
			{
			case 0: // LDS Rd,k
				DIS("LDS    %s,%s",reg_name(D5),addr_or_port_name(progmem[pc]));
				r[D5] = read_sram(progmem[pc++]);
				cycles=2;
				break;
			case 1:	// LD Rd,Z+
				DIS("LD     %s,Z+",reg_name(D5));
				r[D5] = read_sram(Z);
				INC_Z;
				cycles=2;
				break;
			case 2: // LD Rd,-Z
				DIS("LD     %s,-Z",reg_name(D5));
				DEC_Z;
				r[D5] = read_sram(Z);
				cycles=2;
				break;
			case 4:
				DIS("LPM    %s,Z",reg_name(D5));
				r[D5] = read_progmem(Z);
				cycles=3;
				break;
			case 5:
				DIS("LPM    %s,Z+",reg_name(D5));
				r[D5] = read_progmem(Z);
				INC_Z;
				cycles=3;
				break;
			case 6:
				DIS("ELPM   %s,Z",reg_name(D5));
				r[D5] = read_progmem(Z);
				cycles=3;
				break;
			case 7:
				DIS("ELPM   %s,Z+",reg_name(D5));
				r[D5] = read_progmem(Z);
				INC_Z;
				cycles=3;
				break;
			case 9:
				DIS("LD     %s,Y+",reg_name(D5));
				r[D5] = read_sram(Y);
				INC_Y;
				cycles=2;
				break;
			case 10:
				DIS("LD     %s,-Y",reg_name(D5));
				DEC_Y;
				r[D5] = read_sram(Y);
				cycles=2;
				break;
			case 12:
				DIS("LD     %s,X",reg_name(D5));
				r[D5] = read_sram(X);
				cycles=2;
				break;
			case 13:
				DIS("LD     %s,X+",reg_name(D5));
				r[D5] = read_sram(X);
				INC_X;
				cycles=2;
				break;
			case 14:
				DIS("LD     %s,-X",reg_name(D5));
				DEC_X;
				r[D5] = read_sram(X);
				cycles=2;
				break;
			case 15:
				DIS("POP    %s",reg_name(D5));
				INC_SP;
				r[D5] = read_sram(SP);
				cycles=2;
				break;
			default:
				ILL;
			}
			break;
		case 2: case 3:
		  /*1001 001d dddd 0000		STS k,Rr (next word is rest of address)
			1001 001r rrrr 0001		ST Z+,Rr
			1001 001r rrrr 0010		ST -Z,Rr
			1001 001r rrrr 1001		ST Y+,Rr
			1001 001r rrrr 1010		ST -Y,Rr
			1001 001r rrrr 1100		ST X,Rr
			1001 001r rrrr 1101		ST X+,Rr
			1001 001r rrrr 1110		ST -X,Rr
			1001 001d dddd 1111		PUSH Rd */
			cycles=2;
			switch (insn & 15)
			{
			case 0:
				DIS("STS    %s,%s",addr_or_port_name(progmem[pc]),reg_name(D5));
				write_sram(progmem[pc++],r[D5]);
				break;
			case 1:
				DIS("ST     Z+,%s",reg_name(D5));
				write_sram(Z,r[D5]);
				INC_Z;
				break;
			case 2:
				DIS("ST     Z+,%s",reg_name(D5));
				DEC_Z;
				write_sram(Z,r[D5]);
				break;
			case 9:
				DIS("ST     Y+,%s",reg_name(D5));
				write_sram(Y,r[D5]);
				INC_Y;
				break;
			case 10:
				DIS("ST     -Y,%s",reg_name(D5));
				DEC_Y;
				write_sram(Y,r[D5]);
				break;
			case 12:
				DIS("ST     X,%s",reg_name(D5));
				write_sram(X,r[D5]);
				break;
			case 13:
				DIS("ST     X+,%s",reg_name(D5));
				write_sram(X,r[D5]);
				INC_X;
				break;
			case 14:
				DIS("ST     -X,%s",reg_name(D5));
				DEC_X;
				write_sram(X,r[D5]);
				break;
			case 15:
				DIS("PUSH   %s",reg_name(D5));
				write_sram(SP,r[D5]);
				DEC_SP;
				break;
			default:
				ILL;
			}
			break;
		case 4: case 5:
		  /*1001 0100 0000 1001		IJMP (jump thru Z register)
			1001 0100 0001 1001		EIJMP (probably not on 644)
			1001 0100 0sss 1000		BSET s (SEC, etc are aliases with sss implicit)
			1001 0100 1sss 1000		BCLR s (CLC, etc are aliases with sss implicit)
			1001 0100 KKKK 1011		DES (probably not on 644)
			1001 0101 0000 1000		RET
			1001 0101 0000 1001		ICALL (call thru Z register)
			1001 0101 0001 1000		RETI
			1001 0101 0001 1001		EICALL (probably not on 644)
			1001 0101 1000 1000		SLEEP
			1001 0101 1001 1000		BREAK
			1001 0101 1010 1000		WDR
			1001 0101 1100 1000		LPM (r0 implied, why is this special?)
			1001 0101 1101 1000		ELPM (r0 implied)
			1001 0101 1110 1000		SPM Z (writes R1:R0)
		    1001 010d dddd 0000		COM Rd
			1001 010d dddd 0001		NEG Rd
			1001 010d dddd 0010		SWAP Rd
			1001 010d dddd 0011		INC Rd
			1001 010d dddd 0101		ASR Rd
			1001 010d dddd 0110		LSR Rd
			1001 010d dddd 0111		ROR Rd
			1001 010d dddd 1010		DEC Rd
			1001 010k kkkk 110k		JMP k (next word is rest of address)
			1001 010k kkkk 111k		CALL k (next word is rest of address) */
			// Bunch of weird cases here, check for them first and then re-decode.
			switch (insn)
			{
			case 0x9409:
				DIS("IJMP");
				pc = Z;
				cycles=2;
				break;
			case 0x940C:	// JMP; relies on fact that upper k bits are always zero!
				DIS("JMP    $%04x",progmem[pc]);
				pc = progmem[pc];
				cycles=3;
				break;
			case 0x940E:	// CALL; relies on fact that upper k bits are always zero!
				DIS("CALL   $%04x",progmem[pc]);
				write_sram(SP,(pc+1));
				DEC_SP;
				write_sram(SP,(pc+1)>>8);
				DEC_SP;
				pc = progmem[pc];
				cycles=4;
				break;
			case 0x9419:
				DIS("EIJMP");
				pc = Z;
				cycles=2;
				break;
			case 0x9508:
				DIS("RET");
				INC_SP;
				pc = read_sram(SP) << 8;
				INC_SP;
				pc |= read_sram(SP);
				cycles=4;
				break;
			case 0x9509:
				DIS("ICALL");
				write_sram(SP,u8(pc));
				DEC_SP;
				write_sram(SP,(pc)>>8);
				DEC_SP;
				pc = Z;
				cycles=3;
				break;
			case 0x9518:
				DIS("RETI");
				INC_SP;
				pc = read_sram(SP) << 8;
				INC_SP;
				pc |= read_sram(SP);
				cycles=4;
				SREG |= (1<<SREG_I);
				--interruptLevel;
				break;
			case 0x9519:
				DIS("EICALL");
				write_sram(SP,u8(pc));
				DEC_SP;
				write_sram(SP,(pc)>>8);
				DEC_SP;
				pc = Z;
				cycles=3;
				break;
			case 0x9588:
				DIS("SLEEP");
				// no operation
				break;
			case 0x9598:
				DIS("BREAK");
				// no operation
				break;
			case 0x95A8:
				DIS("WDR");
				// Implement this if/when we need watchdog timer functionality.
				if (prevWDR)
				{
					printf("WDR measured %u cycles\n",cycleCounter - prevWDR);
					prevWDR = 0;
				}
				else
					prevWDR = cycleCounter + 1;
				break;
			case 0x95C8:
				DIS("LPM    r0,Z");
				r0 = read_progmem(Z);
				cycles=3;
				break;
			case 0x95D8:
				DIS("ELPM   r0,Z");
				r0 = read_progmem(Z);
				cycles=3;
				break;
			case 0x95E8:
				DIS("SPM");
				if (Z >= progSize/2)
				{
					fprintf(stderr,"illegal write to progmem addr %x\n",Z);
					shutdown();
					exit(1);
				}
				else
					progmem[Z] = r0 | (r1<<8);
				cycles=4;	// undocumented?!?!?
				break;
			default:
			  /*1001 010d dddd 0000		COM Rd
				1001 010d dddd 0001		NEG Rd
				1001 010d dddd 0010		SWAP Rd
				1001 010d dddd 0011		INC Rd
				1001 010d dddd 0101		ASR Rd
				1001 010d dddd 0110		LSR Rd
				1001 010d dddd 0111		ROR Rd
				1001 0100 0sss 1000		BSET s (SEC, etc are aliases with sss implicit)
				1001 0100 1sss 1000		BCLR s (CLC, etc are aliases with sss implicit)
				1001 010d dddd 1010		DEC Rd */
				switch (insn & 15)
				{
				case 0:
					DIS("COM    %s",reg_name(D5));
					r[D5] = R = ~r[D5];
					UPDATE_SVN_LOGICAL; UPDATE_Z; SET_C;
					break;
				case 1:
					DIS("NEG    %s",reg_name(D5));
					Rr = r[D5];
					Rd = 0;
					r[D5] = R = Rd - Rr;
					UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_Z;
					break;
				case 2:
					DIS("SWAP    %s",reg_name(D5));
					Rd = r[D5];
					r[D5] = (Rd >> 4) | (Rd << 4);
					break;
				case 3:
					DIS("INC    %s",reg_name(D5));
					R = ++r[D5];
					UPDATE_N;
					set_bit(SREG,SREG_V,R==0x80);
					UPDATE_S;
					UPDATE_Z;
					break;
				case 5:
					DIS("ASR    %s",reg_name(D5));
					Rd = r[D5];
					set_bit(SREG,SREG_C,Rd&1);
					r[D5] = R = (Rd >> 1) | (Rd & 0x80);
					UPDATE_N;
					set_bit(SREG,SREG_V,(R>>7)^(Rd&1));
					UPDATE_S;
					UPDATE_Z;
					break;
				case 6:
					DIS("LSR    %s",reg_name(D5));
					Rd = r[D5];
					set_bit(SREG,SREG_C,Rd&1);
					r[D5] = R = (Rd >> 1);
					UPDATE_N;
					set_bit(SREG,SREG_V,Rd&1);
					UPDATE_S;
					UPDATE_Z;
					break;
				case 7:
					DIS("ROR    %s",reg_name(D5));
					Rd = r[D5];
					r[D5] = R = (Rd >> 1) | ((SREG&1)<<7);
					set_bit(SREG,SREG_C,Rd&1);
					UPDATE_N;
					set_bit(SREG,SREG_V,(R>>7)^(Rd&1));
					UPDATE_S;
					UPDATE_Z;
					break;
				case 8:
					Rd = (insn>>4)&7;
					if (insn & 0x80)
					{
						DIS("CL%c","CZNVSHTI"[Rd]);
						SREG &= ~(1<<Rd);
					}
					else
					{
						DIS("SE%c","CZNVSHTI"[Rd]);
						SREG |= (1<<Rd);
					}
					break;
				case 10:
					DIS("DEC    %s",reg_name(D5));
					R = --r[D5];
					UPDATE_N;
					set_bit(SREG,SREG_V,R==0x7F);
					UPDATE_S;
					UPDATE_Z;
					break;
				default:
					ILL;
				}
			}
			break;
		case 6:
		  /*1001 0110 KKdd KKKK		ADIW Rd+1:Rd,K   (16-bit add to upper four register pairs) */
			Rd = ((insn >> 3) & 0x6) + 24;
			Rr = ((insn >> 2) & 0x30) | (insn & 0xF);
			DIS("ADIW   %s,%d",reg_pair(Rd),Rr);
			Rd16 = r[Rd] | (r[Rd+1]<<8);
			R16 = Rd16 + Rr;
			r[Rd] = (u8)R16;
			r[Rd+1] = (u8)(R16>>8);
			set_bit(SREG,SREG_V,(~Rd16&R16)&0x8000);
			set_bit(SREG,SREG_N,R16&0x8000);
			UPDATE_S;
			set_bit(SREG,SREG_Z,!R16);
			set_bit(SREG,SREG_C,(~R16&Rd16)&0x8000);
			cycles=2;
			break;
		case 7:
		  /*1001 0111 KKdd KKKK		SBIW Rd+1:Rd,K */
			Rd = ((insn >> 3) & 0x6) + 24;
			Rr = ((insn >> 2) & 0x30) | (insn & 0xF);
			DIS("SBIW   %s,%d",reg_pair(Rd),Rr);
			Rd16 = r[Rd] | (r[Rd+1]<<8);
			R16 = Rd16 - Rr;
			r[Rd] = (u8)R16;
			r[Rd+1] = (u8)(R16>>8);
			set_bit(SREG,SREG_V,(Rd16&~R16)&0x8000);
			set_bit(SREG,SREG_N,R16&0x8000);
			UPDATE_S;
			set_bit(SREG,SREG_Z,!R16);
			set_bit(SREG,SREG_C,(R16&~Rd16)&0x8000);
			cycles=2;
			break;
		case 8:
		  /*1001 1000 AAAA Abbb		CBI A,b */
			Rd = (insn >> 3) & 31;
			DIS("CBI    @%s,%d",port_name(Rd),insn&7);
			write_io(Rd, read_io(Rd) & ~(1<<(insn&7)));
			cycles=2;
			break;
		case 9:
		  /*1001 1001 AAAA Abbb		SBIC A,b */
			Rd = (insn >> 3) & 31;
			DIS("SBIC   @%s,%d",port_name(Rd),insn&7);
			if (!(read_io(Rd) & (1<<(insn&7))))
			{
				uTmp = get_insn_size(progmem[pc]);
				cycles += uTmp;
				pc += uTmp;
			}
			break;
		case 10:
		  /*1001 1010 AAAA Abbb		SBI A,b */
			Rd = (insn >> 3) & 31;
			DIS("SBI    @%s,%d",port_name(Rd),insn&7);
			write_io(Rd, read_io(Rd) | (1<<(insn&7)));
			cycles=2;
			break;
		case 11:
		  /*1001 1011 AAAA Abbb		SBIS A,b */
			Rd = (insn >> 3) & 31;
			DIS("SBIS   @%s,%d",port_name(Rd),insn&7);
			if (read_io(Rd) & (1<<(insn&7)))
			{
				uTmp = get_insn_size(progmem[pc]);
				cycles += uTmp;
				pc += uTmp;
			}
			break;
		case 12: case 13: case 14: case 15:
		  /*1001 11rd dddd rrrr		MUL Rd,Rr */
			DIS("MUL    %s,%s",reg_name(D5),reg_name(R5));
			Rd = r[D5];
			Rr = r[R5];
			uTmp = Rd * Rr; 
			r0 = (u8)uTmp; 
			r1 = (u8)(uTmp >> 8);
			UPDATE_CZ_MUL(uTmp);
			cycles=2;
			break;
		}
		break;
	case 11:
	  /*1011 0AAd dddd AAAA		IN Rd,A
		1011 1AAd dddd AAAA		OUT A,Rd */ 
		Rd = D5;
		Rr = ((insn >> 5) & 0x30) | (insn & 0xF);
		if (insn & 0x0800) // OUT
		{
			DIS("OUT    @%s,%s",port_name(Rr),reg_name(Rd));
			write_io(Rr,r[Rd]);
		}
		else	// IN
		{
			DIS("IN     %s,@%s",reg_name(Rd),port_name(Rr));
			r[Rd] = read_io(Rr);
		}
		break;
	case 12: /*1100 kkkk kkkk kkkk		RJMP k */
		DIS("RJMP   $%04x",pc+k12);
		pc += k12;
		cycles=2;
		break;
	case 13: /*1101 kkkk kkkk kkkk		RCALL k */
		DIS("RCALL  $%04x",pc+k12);
		write_sram(SP,(u8)pc);
		DEC_SP;
		write_sram(SP,pc>>8);
		DEC_SP;
		pc += k12;
		cycles=3;
		break;
	case 14: /*1110 KKKK dddd KKKK		LDI Rd,K (SER is just LDI Rd,255) */
		DIS(K8==255?"SER    %s":"LDI    %s,$%02x",reg_name(D4+16),K8);
		r[D4 + 16] = K8;
		break;
	case 15:
	  /*1111 00kk kkkk ksss		BRBS s,k (same here)
		1111 01kk kkkk ksss		BRBC s,k (BRCC, etc are aliases for this with sss implicit)
		1111 100d dddd 0bbb		BLD Rd,b
		1111 101d dddd 0bbb		BST Rd,b
		1111 110r rrrr 0bbb		SBRC Rr,b
		1111 111r rrrr 0bbb		SBRS Rr,b */
		switch ((insn >> 9) & 7)
		{
		case 0: case 1: /*BRBS*/
			sTmp = k7;
			DIS("%s   $%04x",brbs[insn&7],pc + sTmp);
			if (SREG & (1<<(insn&7)))
			{
				pc += sTmp;
				cycles=2;
			}
			break;
		case 2: case 3: /*BRBC*/
			sTmp = k7;
			DIS("%s   $%04x",brbc[insn&7],pc + sTmp);
			if (!(SREG & (1<<(insn&7))))
			{
				pc += sTmp;
				cycles=2;
			}
			break;
		case 4: /*BLD*/
			DIS("BLD    %s,%d",reg_name(D5),insn&7);
			Rd = D5;
			set_bit(r[Rd],insn&7,SREG & (1<<SREG_T));
			break;
		case 5: /*BST*/
			DIS("BST    %s,%d",reg_name(D5),insn&7);
			Rd = r[D5];
			set_bit(SREG,SREG_T,Rd & (1<<(insn&7)));
			break;
		case 6: /*SBRC*/
			DIS("SBRC   %s,%d",reg_name(D5),insn&7);
			Rd = r[D5];
			if (!(Rd & (1<<(insn&7))))
			{
				uTmp = get_insn_size(progmem[pc]);
				cycles += uTmp;
				pc += uTmp;
			}
			break;
		case 7: /*SBRS*/
			DIS("SBRS   %s,%d",reg_name(D5),insn&7);
			Rd = r[D5];
			if (Rd & (1<<(insn&7)))
			{
				uTmp = get_insn_size(progmem[pc]);
				cycles += uTmp;
				pc += uTmp;
			}
			break;
		}
		break;
	}

#if DISASM
	// Don't spew disassembly during interrupts
	if (singleStep && !interruptLevel)
	{
		printf("$%04x [%04x]  %-23.23s",lastpc,progmem[lastpc],insnBuf);
		if (!disasmOnly)
		{
			printf("[%c%c%c%c%c%c%c%c]",
				BIT(SREG,SREG_I)?'I':' ',
				BIT(SREG,SREG_T)?'T':' ',
				BIT(SREG,SREG_H)?'H':' ',
				BIT(SREG,SREG_S)?'S':' ',
				BIT(SREG,SREG_V)?'V':' ',
				BIT(SREG,SREG_N)?'N':' ',
				BIT(SREG,SREG_Z)?'Z':' ',
				BIT(SREG,SREG_C)?'C':' ');
			if (verbose)
			{
				putchar('\n');
				for (int i=0; i<32; i++)
				{
					printf("r%02d:%02x ",i,r[i]);
					if (i == 7)
						putchar('\n');
					if (i == 15)
						printf("  NXTPC:%04x  CYC:%d\n",pc,cycles);
					else if (i == 23)
						printf("  SP:%04x\n",SP);
					else if (i == 31)
						printf("  X:%04x Y:%04x Z:%04x\n",X,Y,Z);
				}
				putchar('\n');
			}
			else
			{
				char *rs = insnBuf;
				while ((rs = strchr(rs,'r')) != NULL)
				{
					int rn = atoi(++rs);
					printf(" r%02d:%02x",rn,r[rn]);
				}
				// These dumb tests are sufficient because it just so happens that
				// no opcodes (well, after a cursory inspection anyway) contain X, Y, or Z.
				if (strchr(insnBuf,'X'))
					printf(" X:%04x",X);
				if (strchr(insnBuf,'Y'))
					printf(" Y:%04x",Y);
				if (strchr(insnBuf,'Z'))
					printf(" Z:%04x",Z);
				// Could just set a flag above instead of this.
				if (strstr(insnBuf,"MUL") || strstr(insnBuf,"SPM"))
					printf(" r1:r0:$%02x%02x",r1,r0);
				if (strstr(insnBuf,"CALL")||strstr(insnBuf,"RET")||strstr(insnBuf,"PUSH")||strstr(insnBuf,"POP"))
					printf(" SP:%04x",SP);
				putchar('\n');
			}
		}
		else	// If we're disassembling, compute the next pc regardless of branches.
		{
			pc = lastpc + get_insn_size(insn);
			putchar('\n');
			return;
		}
	}
#endif

	update_hardware(cycles);

	return cycles;
}


void avr8::trigger_interrupt(int location)
{
	// are interrupts enabled?
	if (BIT(SREG,SREG_I))
	{
#if DISASM
		printf("INTERRUPT triggered at cycleCounter = %u\n",cycleCounter);
#endif

		// clear interrupt flag
		set_bit(SREG,SREG_I,0);

		// push current PC
		write_sram(SP,(u8)pc);
		DEC_SP;
		write_sram(SP,pc>>8);
		DEC_SP;

		// jump to new location (which jumps to the real handler)
		pc = location;

		// bill the cycles consumed.
		// (this in theory can recurse back into here but we've
		// already cleared the interrupt enable flag)
		update_hardware(5);

		++interruptLevel;
	}
	// else interrupt gets ignored!
}

#if GUI
bool avr8::init_gui()
{
	if ( SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return false;
	}
	atexit(SDL_Quit);

	if (fullscreen)
		screen = SDL_SetVideoMode(800,600,32,sdl_flags | SDL_FULLSCREEN);
	else
		screen = SDL_SetVideoMode(720,448,32,sdl_flags);
	if (!screen)
	{
		fprintf(stderr, "Unable to set 720x448x32 video mode.\n");
		return false;
	}
	else if (fullscreen)	// Center in fullscreen
		inset = ((600-448)/2) * screen->pitch + 4 * ((800-720)/2);

	if (SDL_MUSTLOCK(screen) && SDL_LockSurface(screen) < 0)
		return false;

	if (fullscreen)
	{
		SDL_ShowCursor(0);
	}

	// Open audio driver
	SDL_AudioSpec desired;
	memset(&desired, 0, sizeof(desired));
	desired.freq = 15700;
	desired.format = AUDIO_U8;
	desired.callback = audio_callback_stub;
	desired.userdata = this;
	desired.channels = 1;
	desired.samples = 1024;
	if (enableSound)
	{
		if (SDL_OpenAudio(&desired, NULL) < 0)
		{
			fprintf(stderr, "Unable to open audio device, no sound will play.\n");
			enableSound = false;
		}
		else
			SDL_PauseAudio(0);
	}

	current_cycle = -999999;
	scanline_top = -33;
	scanline_count = -999;
	left_edge = -152;

	latched_buttons[0] = buttons[0] = ~0;
	latched_buttons[1] = buttons[1] = ~0;
	mouse_scale = 1;

	// Precompute final palette for speed.
	// Should build some NTSC compensation magic in here too.
	for (int i=0; i<256; i++)
	{
		int red = (((i >> 0) & 7) * 255) / 7;
		int green = (((i >> 3) & 7) * 255) / 7;
		int blue = (((i >> 6) & 3) * 255) / 3;
		palette[i] = SDL_MapRGB(screen->format, red, green, blue);
	}
	
	return true;
}

void avr8::handle_key_down(SDL_Event &ev)
{
	update_buttons(ev.key.keysym.sym,true);
	static int ssnum = 0;
	char ssbuf[32];
	static const char *pad_mode_strings[4] = {"NES pad.","SNES pad.","SNES 2p pad.","SNES mouse."};

	switch (ev.key.keysym.sym)
	{
			// SDLK_LEFT/RIGHT/UP/DOWN
			// SDLK_abcd...
			// SDLK_RETURN...
			case SDLK_1: left_edge--; printf("left=%d\n",left_edge); break;
			case SDLK_2: left_edge++; printf("left=%d\n",left_edge); break;
			case SDLK_3: scanline_top--; printf("top=%d\n",scanline_top); break;
			case SDLK_4: scanline_top++; printf("top=%d\n",scanline_top); break;
			case SDLK_5: 
				if (pad_mode == NES_PAD)
					pad_mode = SNES_PAD;
				else if(pad_mode == SNES_PAD)
					pad_mode = SNES_PAD2;
				else if(pad_mode == SNES_PAD2)
					pad_mode = SNES_MOUSE;
				else
					pad_mode = NES_PAD;
				puts(pad_mode_strings[pad_mode]); 
				break;
			case SDLK_6:
				if (++mouse_scale == 6)
					mouse_scale = 0;
				printf("new mouse sensitivity is %d\n",mouse_scale);
				break;
			case SDLK_ESCAPE:
				printf("user abort (pressed ESC).\n");
				shutdown();
				exit(0);
			case SDLK_PRINT:
				sprintf(ssbuf,"uzem_%03d.bmp",ssnum++);
				printf("saving screenshot to '%s'...\n",ssbuf);
				SDL_SaveBMP(screen,ssbuf);
				break;
			case SDLK_0:
				PIND = PIND & ~8;
				break;
			case SDLK_F1:
				puts("1/2 - Adjust left edge lock");
				puts("3/4 - Adjust top edge lock");
				puts(" 5  - Toggle NES/SNES 1p/SNES 2p/SNES mouse mode (default is SNES pad)");
				puts(" 6  - Mouse sensitivity scale factor");
				puts(" F1 - This help text");
#if DISASM
				puts(" F5 - Debugger resume execution");
				puts(" F9 - Debugger halt execution");
				puts("F10 - Debugger single step");
#endif
				puts("Esc - Quit emulator");
				puts(" 0  - AVCORE Baseboard power switch");
				puts("");
				puts("            Up Down Left Right A B X Y Start Sel LShldr RShldr");
				puts("NES:        ----arrow keys---- a s     Enter Tab              ");
				puts("SNES 1p:    ----arrow keys---- a s x z Enter Tab LShift RShift");
				puts("  2p P1:     w   s    a    d   f g r t   z    x     q      e  ");
				puts("  2p P2:     i   k    j    l   ; ' p [   n    m     u      o  ");
				break;
#if DISASM
			case SDLK_F9:
				singleStep = nextSingleStep = true;
				puts("single step activated, F5 to resume, F10 to step");
				break;
			case SDLK_F5:
				singleStep = nextSingleStep = false;
				puts("resume full speed");
				break;
			case SDLK_F10:
				singleStep = false;
				nextSingleStep = true;
				puts("step.");
				break;
#endif
	}
}

void avr8::audio_callback(Uint8 *stream,int len)
{
	// printf("want %d bytes (have %d)\n",len,audioRing.getUsed());
	while (len--)
		*stream++ = audioRing.pop();
}

void avr8::handle_key_up(SDL_Event &ev)
{
	update_buttons(ev.key.keysym.sym,false);
	if (ev.key.keysym.sym == SDLK_p)
		PIND |= 8;
}

struct keymap { u16 key; u8 player, bit; };
#define END_OF_MAP { 0,0,0 }
keymap nes_one_player[] = 
{	
	{ SDLK_a, 0, NES_A }, { SDLK_s, 0, NES_B }, { SDLK_TAB, 0, PAD_SELECT }, { SDLK_RETURN, 0, PAD_START },
	{ SDLK_UP, 0, PAD_UP }, { SDLK_DOWN, 0, PAD_DOWN }, { SDLK_LEFT, 0, PAD_LEFT }, { SDLK_RIGHT, 0, PAD_RIGHT },
	END_OF_MAP
};
keymap snes_one_player[] =
{
	{ SDLK_s, 0, SNES_B }, { SDLK_z, 0, SNES_Y }, { SDLK_TAB, 0, PAD_SELECT }, { SDLK_RETURN, 0, PAD_START },
	{ SDLK_UP, 0, PAD_UP }, { SDLK_DOWN, 0, PAD_DOWN }, { SDLK_LEFT, 0, PAD_LEFT }, { SDLK_RIGHT, 0, PAD_RIGHT },
	{ SDLK_a, 0, SNES_A }, { SDLK_x, 0, SNES_X }, { SDLK_LSHIFT, 0, SNES_LSH }, { SDLK_RSHIFT, 0, SNES_RSH },
	END_OF_MAP
};
keymap snes_two_players[] =
{
	// P1
	{ SDLK_a, 0, PAD_LEFT }, { SDLK_s, 0, PAD_DOWN }, { SDLK_d, 0, PAD_RIGHT }, { SDLK_w, 0, PAD_UP },
	{ SDLK_q, 0, SNES_LSH }, { SDLK_e, 0, SNES_RSH }, { SDLK_r, 0, SNES_Y }, { SDLK_t, 0, SNES_X },
	{ SDLK_f, 0, SNES_B }, { SDLK_g, 0, SNES_A }, { SDLK_z, 0, PAD_SELECT }, { SDLK_x, 0, PAD_START },
	// P2
	{ SDLK_j, 1, PAD_LEFT }, { SDLK_k, 1, PAD_DOWN }, { SDLK_l, 1, PAD_RIGHT }, { SDLK_i, 1, PAD_UP },
	{ SDLK_u, 1, SNES_LSH }, { SDLK_o, 1, SNES_RSH }, { SDLK_p, 1, SNES_Y }, { SDLK_LEFTBRACKET, 1, SNES_X },
	{ SDLK_SEMICOLON, 1, SNES_B }, { SDLK_QUOTE, 1, SNES_A }, { SDLK_n, 1, PAD_SELECT }, { SDLK_m, 1, PAD_SELECT },
	END_OF_MAP
};
keymap snes_mouse[] =
{
	END_OF_MAP
};
keymap *keymaps[] = { nes_one_player, snes_one_player, snes_two_players, snes_mouse };

void avr8::update_buttons(int key,bool down)
{
	keymap *k = keymaps[pad_mode];
	while (k->key)
	{
		if (key == k->key)
		{
			if (down)
				buttons[k->player] &= ~(1<<k->bit);
			else
				buttons[k->player] |= (1<<k->bit);
			break;
		}
		++k;
	}
	// printf("buttons = %x\n",buttons);
}
#endif

void avr8::update_hardware(int cycles)
{
	cycleCounter += cycles;

	if (TCCR1B)	// not really correct, but close enough?
	{
		u16 TCNT1 = TCNT1L | (TCNT1H<<8);
		u16 OCR1A = OCR1AL | (OCR1AH<<8);
		
		if (TCNT1 < OCR1A && TCNT1 + cycles >= OCR1A)
		{
			TCNT1 -= (OCR1A - cycles);
			if (TIMSK1 & 2)
				trigger_interrupt(TIMER1_COMPA);
		}
		else
			TCNT1 += cycles;

		TCNT1L = (u8) TCNT1;
		TCNT1H = (u8) (TCNT1>>8);
	}

#if GUI && VIDEO_METHOD == 1
	if (scanline_count >= 0 && current_cycle < 1440)
	{
		while (cycles)
		{
			if (current_cycle >= 0 && current_cycle < 1440 && (current_cycle&1))
				current_scanline[current_cycle>>1] = 
				next_scanline[current_cycle>>1] = pixel;
			++current_cycle;
			--cycles;
		}
	}
#endif
}


static inline int parse_hex_nibble(char s)
{
	if (s >= '0' && s <= '9')
		return s - '0';
	else if (s >= 'A' && s <= 'F')
		return s - 'A' + 10;
	else if (s >= 'a' && s <= 'a')
		return s - 'a' + 10;
	else
		return -1;
}

static int parse_hex_byte(const char *s)
{
	return (parse_hex_nibble(s[0])<<4) | parse_hex_nibble(s[1]);
}

static int parse_hex_word(const char *s)
{
	return (parse_hex_nibble(s[0])<<12) | (parse_hex_nibble(s[1]) << 8) |
		(parse_hex_nibble(s[2])<<4) | parse_hex_nibble(s[3]);
}

bool avr8::load_hex_progmem(const char *filename)
{
	return load_hex_common(filename,progmem,progSize,true);
}

bool avr8::load_hex_eeprom()
{
	return load_hex_common(eepromName,eeprom,eepromSize,false);
}

bool avr8::load_hex_common(const char *filename,void *dest,int destSize,bool isWord)
{
	/*
	http://en.wikipedia.org/wiki/.hex

	(I've added the spaces for clarity, they don't exist in the real files)
	:10 65B0 00 661F771F881F991F1A9469F760957095 59
	:10 65C0 00 809590959B01AC01BD01CF010895F894 91
	:02 65D0 00 FFCF FB
	:02 65D2 00 0100 C6
	:00 0000 01 FF [EOF marker]

	First field is the byte count. Second field is the 16-bit address. Third field is the record type; 
	00 is data, 01 is EOF.	For record type zero, next "wide" field is the actual data, followed by a 
	checksum.
	*/
	FILE *f = fopen(filename,"rt");

	if (!f)
		return false;

	// Zero entire memory out first in case new image is shorter than last one
	memset(dest, 0, destSize);

	char line[128];
	int lineNumber = 1;
	if (isWord)
		progmemLast = 0;
	while (fgets(line, sizeof(line), f) && line[0]==':')
	{
		int bytes = parse_hex_byte(line+1);
		int addr = parse_hex_word(line+3);
		int recordType = parse_hex_byte(line+7);
		u8 checksum = bytes + parse_hex_byte(line+3) + parse_hex_byte(line+5) + recordType;
		if (recordType == 0)
		{
			char *lp = line + 9;
			while (bytes > 0)
			{
				if (isWord)
				{
					checksum += parse_hex_byte(lp) + parse_hex_byte(lp+2);
					u16 *p = (u16*) dest;
					p[(addr & (progSize-1)) >> 1] = parse_hex_byte(lp) | (parse_hex_byte(lp+2)<<8);
					addr += 2;
					if ((addr>>1) > progmemLast)
						progmemLast = (addr>>1);
					bytes -= 2;
					lp += 4;
				}
				else
				{
					checksum += parse_hex_byte(lp);
					u8 *b = (u8*) dest;
					b[addr & (destSize-1)] = parse_hex_byte(lp);
					addr += 1;
					bytes -= 1;
					lp += 2;
				}
			}
			checksum = ~checksum + 1;
			if (checksum != parse_hex_byte(lp))
			{
				fprintf(stderr,"line %d: checksum error, expected %x, got %x\n",lineNumber,checksum,parse_hex_byte(lp));
			}
		}
		else if (recordType == 1)
		{
			break;
		}
		else
			fprintf(stderr,"ignoring unknown record type %d in line %d of %s\n",recordType,lineNumber,filename);

		++lineNumber;
	}

	fclose(f);
	return true;
}


bool avr8::save_hex_common(const char *filename,const u8 *src,int srcSize)
{
	FILE *f = fopen(filename,"wt");
	if (!f)
		return false;

	for (int i=0; i<srcSize; i+=16)
	{
		// :10 65B0 00 661F771F881F991F1A9469F760957095 59
		fprintf(f,":10%04X00",i);
		u8 checksum = 0x10 + (i >> 8) + (i & 255);
		for (int j=0; j<16; j++)
		{
			fprintf(f,"%02X",src[i+j]);
			checksum += src[i+j];
		}
		fprintf(f,"%02X\n",(~checksum + 1) & 255);
	}
	fprintf(f,":00000001FF\n");
	fclose(f);

	return true;
}


bool avr8::save_hex_eeprom()
{
	return save_hex_common(eepromName,eeprom,eepromSize);
}


int main(int argc,char **argv)
{
	avr8 uzebox;
	bool disasmOnly = true;

#if GUI
	uzebox.sdl_flags = SDL_DOUBLEBUF | SDL_SWSURFACE;
#endif

	if (argc<2 || !uzebox.load_hex_progmem(argv[1]))
	{
		fprintf(stderr,"usage: %s filename.hex flags...\n",argv[0]);
		fprintf(stderr,"\t-bp addr : set breakpoint\n");
		fprintf(stderr,"\t-nosound : disable sound playback\n");
		fprintf(stderr,"\t-hwsurface : use SDL hardware surface (probably slower)\n");
		fprintf(stderr,"\t-nodoublebuf : no double buffering\n");
		fprintf(stderr,"\t-interlaced : turn on interlaced rendering\n");
		fprintf(stderr,"\t-mouse : start with emulated mouse enabled\n");
		fprintf(stderr,"\t-2p: start with snes 2p mode enabled\n");
		fprintf(stderr,"\t-eeprom name : set eeprom filename (default eeprom.hex)\n");
		return 1;
	}
	else
		puts("Run emulator with no parameters to see command line help.");

	for (int i=0; i<argc; i++)
	{
		if (!strcmp(argv[i],"-bp"))
			uzebox.breakpoint = (u16) strtoul(argv[++i],NULL,16);
		else if (!strcmp(argv[i],"-nosound"))
			uzebox.enableSound = false;
		else if (!strcmp(argv[i],"-fullscreen"))
			uzebox.fullscreen = true;
		else if (!strcmp(argv[i],"-hwsurface"))
			uzebox.sdl_flags = (uzebox.sdl_flags & ~SDL_SWSURFACE) | SDL_HWSURFACE;
		else if (!strcmp(argv[i],"-nodoublebuf"))
			uzebox.sdl_flags &= ~SDL_DOUBLEBUF;
		else if (!strcmp(argv[i],"-interlaced"))
			uzebox.interlaced = true;
		else if (!strcmp(argv[i],"-mouse"))
			uzebox.pad_mode = avr8::SNES_MOUSE;
		else if (!strcmp(argv[i],"-2p"))
			uzebox.pad_mode = avr8::SNES_PAD2;
		else if (!strcmp(argv[i],"-eeprom"))
			uzebox.eepromName = argv[++i];
	}

	uzebox.load_hex_eeprom();

#if GUI
	if (!uzebox.init_gui())
		return 1;
#endif

	while (true)
	{
#if GUI
		const int cycles = 100000000;
		int left = cycles;
		int now = SDL_GetTicks();
		while (left > 0)
			left -= uzebox.exec(disasmOnly,false);
		now = SDL_GetTicks() - now;

		char caption[128];
		sprintf(caption,"uzebox emulator v1.08 (ESC=quit, F1=help)  %02d.%03d Mhz",cycles/now/1000,(cycles/now)%1000);
		if (uzebox.fullscreen)
			puts(caption);
		else
			SDL_WM_SetCaption(caption, NULL);
#else
		uzebox.exec(disasmOnly,false);
#endif
	}

	return 0;
}
