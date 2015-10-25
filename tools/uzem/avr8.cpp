/*
(The MIT License)

Copyright (c) 2008-2015 by
David Etherton, Eric Anderton, Alec Bourque (Uze), Filipe Rinaldi,
Sandor Zsuga (Jubatian), Matt Pandina (Artcfox)

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

/*
Revision Log
------------
7/8/2013 V1.16 Added emulation for Timer1 Overflow interrupt

More info at uzebox.org

*/


#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <queue>

#include "avr8.h"
#include "gdbserver.h"
#include "SDEmulator.h"
#include "Keyboard.h"
#include "logo.h"

using namespace std;

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

#define EEPM1 0x20
#define EEPM0 0x10
#define EERIE 0x08
#define EEMPE 0x04
#define EEPE  0x02
#define EERE  0x01

//Interrupts vector adresses
#define INT_RESET		0x00
#define WDT				0x10
#define TIMER1_COMPA	0x1A
#define TIMER1_COMPB	0x1C
#define TIMER1_OVF		0x1E
#define SPI_STC     	0x26

#define REG_TCNT1L		0x84

//TIFR1 flags
#define TOV1			1
#define OCF1A			2
#define OCF1B			4

//TIMSK1 flags
#define TOIE1			1
#define OCIE1A			2
#define OCIE1B			4
#define ICIE1			32

//TCCR1B flags
#define CS10			1
#define WGM12			8

//Watchdog flags
#define WDE				8
#define WDIE			64
#define WDIF			128

#define DELAY16MS			457142	//in cpu cycles
#define HSYNC_HALF_PERIOD 	910		//in cpu cycles
#define HSYNC_PERIOD 		1820	//in cpu cycles

static const char* joySettingsFilename = "joystick-settings";

#define SD_ENABLED() SDpath

/*
#define D3	((insn >> 4) & 7)
#define R3	(insn & 7)
#define D4	((insn >> 4) & 15)
#define R4	(insn & 15)
#define R5	((insn & 15) | ((insn >> 5) & 0x10))
#define D5	((insn >> 4) & 31)
#define K8	(((insn >> 4) & 0xF0) | (insn & 0xF))
#define k7	((s16)(insn<<6)>>9)
#define k12	((s16)(insn<<4)>>4)
*/

#define BIT(x,b)	(((x)>>(b))&1)
#define C			BIT(SREG,SREG_C)


// Delayed output flags. If either is set, there is a delayed out waiting to
// be written on the end of update_hardware, so the next cycle it can have
// effect. The "dly_out" variable contains these flags, and the "dly_xxx"
// variables the values to be written out.
#define DLY_TCCR1B     0x0001U
#define DLY_TCNT1      0x0002U


// Masks for SREG bits, use to combine them
#define SREG_IM (1U << SREG_I)
#define SREG_TM (1U << SREG_T)
#define SREG_HM (1U << SREG_H)
#define SREG_SM (1U << SREG_S)
#define SREG_VM (1U << SREG_V)
#define SREG_NM (1U << SREG_N)
#define SREG_ZM (1U << SREG_Z)
#define SREG_CM (1U << SREG_C)

// Clears bits. Use this on the bits which should change processing
// the given instruction.
inline static void clr_bits(u8 &dest, unsigned int bits)
{
	dest = dest & (~bits);
}
// Inverse set bit: Sets if value is zero. Mostly for Z flag
inline static void set_bit_inv(u8 &dest, unsigned int bit, unsigned int value)
{
	// Assume at most 16 bits input on value, makes it 0 or 1, the latter
	// if the input was nonzero. The "& 1U" part is usually thrown away by
	// the compiler (32 bits). The "& 0xFFFFU" part might also be thrown
	// away depending on the input.
	value = ((value & 0xFFFFU) - 1U) >> 31;
	dest  = dest | ((value & 1U) << bit);
}
// Set bit using only the lowest bit of 'value': if 1, sets the bit.
inline static void set_bit_1(u8 &dest, unsigned int bit, unsigned int value)
{
	// The "& 1" on 'value' might be thrown away for suitable input.
	dest  = dest | ((value & 1U) << bit);
}
// Store bit (either set or clear) using only the lowest bit of 'value'.
inline static void store_bit_1(u8 &dest, unsigned int bit, unsigned int value)
{
	// The "& 1" on 'value' might be thrown away for suitable input
	// If 'bit' is constant (inlining), it folds up well on optimizing.
	dest  = dest & (~(1U << bit));
	dest  = dest | ((0U - (value & 1U)) & (1U << bit));
}


// This computes both the half-carry (bit3) and full carry (bit7)
#define BORROWS		(~Rd&Rr)|(Rr&R)|(R&~Rd)
#define CARRIES		((Rd&Rr)|(Rr&~R)|(~R&Rd))

#define UPDATE_HC_SUB \
	CH = BORROWS; \
	set_bit_1(SREG, SREG_H, (CH & 0x08U) >> 3); \
	set_bit_1(SREG, SREG_C, (CH & 0x80U) >> 7);
#define UPDATE_HC_ADD \
	CH = CARRIES; \
	set_bit_1(SREG, SREG_H, (CH & 0x08U) >> 3); \
	set_bit_1(SREG, SREG_C, (CH & 0x80U) >> 7);

#define UPDATE_H		set_bit_1(SREG, SREG_H, (CARRIES & 0x8) >> 3)
#define UPDATE_Z		set_bit_inv(SREG, SREG_Z, R)
#define UPDATE_V_ADD	set_bit_1(SREG, SREG_V, (((Rd&Rr&~R)|(~Rd&~Rr&R)) & 0x80) >> 7)
#define UPDATE_V_SUB	set_bit_1(SREG, SREG_V, (((Rd&~Rr&~R)|(~Rd&Rr&R)) & 0x80) >> 7)
#define UPDATE_N		set_bit_1(SREG, SREG_N, (R & 0x80) >> 7)
#define UPDATE_S		set_bit_1(SREG, SREG_S, BIT(SREG,SREG_N) ^ BIT(SREG,SREG_V))

#define UPDATE_SVN_SUB	UPDATE_V_SUB; UPDATE_N; UPDATE_S
#define UPDATE_SVN_ADD	UPDATE_V_ADD; UPDATE_N; UPDATE_S

// Simplified version for logical insns.
// sreg_clr on S, V, and N should be called before this.
// If 7th bit of R is set:
//     Sets N, sets S, clears V.
// If 7th bit of R is clear:
//     Clears N, clears S, clears V.
#define UPDATE_SVN_LOGICAL \
	SREG |= ((0x7FU - (unsigned int)(R)) >> 8) & (SREG_SM | SREG_VM);

#define UPDATE_CZ_MUL(x)		set_bit_1(SREG,SREG_C,(x & 0x8000) >> 15); set_bit_inv(SREG,SREG_Z,x)

// UPDATE_CLEAR_Z: Updates Z flag by clearing if result is nonzero. This
// should be used if the previous Z flag state is meant to be preserved (such
// as in CPC), so don't include Z in a clr_bits then.
#define UPDATE_CLEAR_Z		(SREG &= ~(((0U - (unsigned int)(R)) >> 8) & SREG_ZM))

#define SET_C		(SREG |= (1<<SREG_C))

#define ILLEGAL_OP fprintf(stderr,"invalid insn at address %x\n",currentPc); shutdown(1);

#if defined(_DEBUG)
#define DISASM 1
#define DIS(fmt,...)	sprintf(insnBuf,fmt,##__VA_ARGS__); if (disasmOnly) break
#else
#define DISASM 0
#define DIS(fmt,...)
#endif

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

u32 hsync_more_col;
u32 hsync_less_col;

FILE* avconv_video = NULL;
FILE* avconv_audio = NULL;

void avr8::spi_calculateClock(){
    // calculate the number of cycles before the write completes
    u16 spiClockDivider;
    switch(SPCR & 0x03){
		case 0: spiClockDivider = 4; break;
		case 1: spiClockDivider = 16; break;
		case 2: spiClockDivider = 64; break;
		case 3: spiClockDivider = 128; break;
    }
    if(SPSR & 0x01){
        spiClockDivider = spiClockDivider >> 1; // double the speed
    }
    spiCycleWait = spiClockDivider*8;
    SPI_DEBUG("SPI divider set to : %d (%d cycles per byte)\n",spiClockDivider,spiCycleWait);
}

// Renders a line into a 32 bit output buffer.
// Performs a shrink by 2
static inline void render_line(u32* dest, u8 const* src, u32 const* pal)
{
	for (unsigned int i = 0; i < VIDEO_DISP_WIDTH; i++)
		dest[i] = pal[src[i<<1]];
}

inline void avr8::write_io(u8 addr,u8 value)
{
	// Pixel output ideally should inline, it is performed about 2 - 3
	// million times per second in a Uzebox game.
	if (addr == ports::PORTC)
	{
		pixel_raw = value & DDRC;
	}
	else
	{
		write_io_x(addr, value);
	}
}

// Should not be called directly, use write_io instead (pixel output!)
void avr8::write_io_x(u8 addr,u8 value)
{
	u8 changed;
	u8 went_low;

	switch (addr)
	{
	case (ports::OCR2A):
		if (enableSound && TCCR2B)
		{
			// raw pcm sample at 15.7khz
#ifndef __EMSCRIPTEN__
			while (audioRing.isFull())SDL_Delay(1);
#endif
			SDL_LockAudio();
			audioRing.push(value);
			SDL_UnlockAudio();

			//Send audio byte to ffmpeg
			if(recordMovie && avconv_audio) {
				fwrite(&value, 1, 1, avconv_audio);

				// Keep audio in sync, since the sample rate we encode at is not a factor of the clock speed
				const double needs_extra_sample = 4.0 * 1.0 / 15734.0 / (1.0 / 15734.0 - 1820.0 / 28636360.0);
				static double accumulated_error = 0.0;
				accumulated_error += (28636360 % 15734);
				if (accumulated_error > needs_extra_sample) {
					accumulated_error -= needs_extra_sample;
					fwrite(&value, 1, 1, avconv_audio);
				}
			}
		}
		break;

	case (ports::PORTD):
		// write value with respect to DDRD register
		io[addr] = value & DDRD;
		break;

	case (ports::PORTB):
		if(value&1){
			elapsedCycles=cycleCounter-prevCyclesCounter;

			if (scanline_count == -999 && elapsedCycles >= HSYNC_HALF_PERIOD -10 && elapsedCycles <= HSYNC_HALF_PERIOD + 10)
			{
			   scanline_count = scanline_top;
			}
			else if (scanline_count != -999)
			{

				if (scanline_count >= 0){
					render_line(
						(u32*)((u8*)surface->pixels + scanline_count * surface->pitch),
						&scanline_buf[left_edge],
						palette);
				}

				scanline_count ++;
				current_cycle = 0U;

				if (scanline_count == 224)
				{

					SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
					SDL_RenderClear(renderer);
					SDL_RenderCopy(renderer, texture, NULL, NULL);
					SDL_RenderPresent(renderer);

					//Send video frame to ffmpeg
					if (recordMovie && avconv_video) fwrite(surface->pixels, VIDEO_DISP_WIDTH*224*4, 1, avconv_video);

					SDL_Event event;
					while (singleStep? SDL_WaitEvent(&event) : SDL_PollEvent(&event))
					{
						switch (event.type) {
							case SDL_KEYDOWN:
								handle_key_down(event);
								break;
							case SDL_KEYUP:
								handle_key_up(event);
								break;
							case SDL_JOYBUTTONDOWN:
							case SDL_JOYBUTTONUP:
							case SDL_JOYAXISMOTION:
							case SDL_JOYHATMOTION:
							case SDL_JOYBALLMOTION:
								if (jmap.jstate != JMAP_IDLE)
									map_joysticks(event);
								else
									update_joysticks(event);
								break;
							case SDL_QUIT:
								printf("User abort (closed window).\n");
								shutdown(0);
								break;
						}
					}

					//capture or replay controlelr capture data
					if(captureMode==CAPTURE_WRITE){
						fputc((u8)(buttons[0]&0xff),captureFile);
						fputc((u8)((buttons[0]>>8)&0xff),captureFile);
					}else if(captureMode==CAPTURE_READ && captureSize>0){
						buttons[0]=captureData[capturePtr]+(captureData[capturePtr+1]<<8);
						capturePtr+=2;
						captureSize-=2;
					}else if(captureMode==CAPTURE_READ && captureSize==0){
						printf("Playback reached end of capture file.\n");
						shutdown(0);
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
							buttons[0] &= ~(1<<8);
						// keep mouse centered so it doesn't get stuck on edge of screen.
						// ...and immediately consume the bogus motion event it generated.
						if (fullscreen)
						{
							SDL_WarpMouseInWindow(window,400,300);
							SDL_GetRelativeMouseState(&mouse_dx,&mouse_dy);
						}
					}
					else
						buttons[0] |= 0xFFFF8000;

					singleStep = nextSingleStep;
					scanline_count = -999;
				}
			}

		   prevCyclesCounter=cycleCounter;
		}
		break;

	case (ports::PORTA):
		changed = value ^ io[addr];
		went_low = changed & io[addr];

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
			}
		}
		else if (went_low == (1<<3))	// CLOCK
		{
			if (new_input_mode)	PINA = u8((latched_buttons[0] & 1) | ((latched_buttons[1] & 1) << 1));
			latched_buttons[0] >>= 1;
			latched_buttons[1] >>= 1;

			if ((latched_buttons[1] < 0xFFFFF) && !new_input_mode)
			{
				//New input routines detected, switching emulation method
				new_input_mode = true;
			}
		}
		if (!new_input_mode) PINA = u8((latched_buttons[0] & 1) | ((latched_buttons[1] & 1) << 1));


		//Uzebox keyboard (always on P2 port)
		switch(uzeKbState){
			case KB_STOP:
				//check uzekeyboard start condition: clock=low & latch=high simultaneously
				if((value&0x0c)==0x04){
					uzeKbState=KB_TX_START;
					uzeKbEnabled=true;	//enable keyboard capture for Uzebox Keyboard
				}
				break;

			case KB_TX_START:
				//check start condition pulse completed: clock=high & latch=low (normal state)
				if((value&0x0c)==0x08){
					uzeKbState=KB_TX_READY;
					uzeKbClock=8;
				}
				break;

			case KB_TX_READY:
				if (went_low == (1<<3))	// CLOCK
				{
					if(uzeKbClock==8){
						uzeKbDataOut=0;
						//returns only keys (no commands response yet)
						if(uzeKbScanCodeQueue.empty()){
							uzeKbDataIn=0;
						}else{
							uzeKbDataIn=uzeKbScanCodeQueue.front();
							uzeKbScanCodeQueue.pop();
						}
					}


					//shift data out to keyboard
					//latch pin is used as "Data Out"
					uzeKbDataOut<<=1;
					if(value&0x04){ //latch pin=1?
						uzeKbDataOut|=1;
					}

					//shift data in from keyboard
					if(uzeKbDataIn&0x80){
						PINA|=(0x02); //set P2 data bit
					}else{
						PINA&=~(0x02); //clear P2 data bit
					}
					uzeKbDataIn<<=1;

					uzeKbClock--;
					if(uzeKbClock==0){
						if(uzeKbDataOut==KB_SEND_END){
							uzeKbState=KB_STOP;
						}else{
							uzeKbClock=8;
						}
					}
				}
				break;
		}


		io[addr] = value;
		break;

	case (ports::TCNT1H):
		// p106 in 644 manual; 16-bit values are latched
		T16_latch = value;
		break;

	case (ports::TCNT1L):
		dly_TCNT1L = value;
		dly_TCNT1H = T16_latch;
		dly_out |= DLY_TCNT1;
		break;

	case (ports::SPDR):
		if((SPCR & 0x40) && SD_ENABLED()){ // only if SPI is enabled and card is present
			spiByte = value;
			//TODO: flag collision if x-fer in progress
			spiClock = spiCycleWait;
			spiTransfer = 1;
			SPSR ^= 0x80; // clear interrupt
			//SPI_DEBUG("spiClock: %0.2X\n",spiClock);
		}
		// SPI_DEBUG("SPDR: %0.2X\n",value);
		io[addr] = value;
		break;

	case (ports::SPCR):
		SPI_DEBUG("SPCR: %02X\n",value);
		io[addr] = value;
		if(SD_ENABLED()) spi_calculateClock();
		break;

	case (ports::SPSR):
		SPI_DEBUG("SPSR: %02X\n",value);
		io[addr] = value;
		if(SD_ENABLED()) spi_calculateClock();
		break;

	case (ports::EECR):
		//printf("writing to port %s (%x) pc = %x\n",port_name(addr),value,pc-1);
		//EEPROM can only be put into either read or write mode, and the master bit must be set

		if(value & EERE){
			if(io[addr] & EEPE){
				io[addr] = value ^ EERE; // programming in progress, don't allow this to be set
			}
			else{
				io[addr] = value;
			}
		}
		else if(value & EEPE){
			if( (io[addr] & EERE) || !(io[addr] & EEMPE)){  // need master program enabled first
				io[addr] = value ^ EEPE; // read in progress, don't allow this to be set
			}
			else{
				io[addr] = value;
			}
		}
		if(value & EEMPE){
			io[addr] = value;
			// eeClock = 4; TODO: This was only set here, never used. Maybe a never completed EEPROM timing code.
		}
		else{
			io[addr] = value;
		}
		break;

	// Note: This was commented out in the original code. If needed,
	// integrate in the switch.
	//else if(addr == ports::EEARH || addr == ports::EEARL || addr == ports::EEDR){
	//	io[addr] = value;

	case (ports::TIFR1):
		//clear flags by writing logical one
		io[addr] &= ~(value);
		break;

	case (ports::TCCR1B):
		dly_TCCR1B = value;
		dly_out |= DLY_TCCR1B;
		break;

	case (ports::OCR1AH):
	case (ports::OCR1AL):
	case (ports::OCR1BH):
	case (ports::OCR1BL):
		// TODO: These should also be latched by the Atmel docs, maybe
		// implement it later.
		io[addr] = value;
		timer1_next = 0U; // Force timer state recalculation (update_hardware)
		break;

	case (ports::res3A):
		// emulator-only whisper support
		printf("%c",value);
		break;

	case (ports::res39):
		// emulator-only whisper support
		printf("%02x",value);
		break;

	default:
		io[addr] = value;
		break;
	}
}


u8 avr8::read_io(u8 addr)
{
	// p106 in 644 manual; 16-bit values are latched
	if      (addr == ports::TCNT1L)
	{
		T16_latch = (TCNT1 >> 8) & 0xFFU;
		return TCNT1 & 0xFFU;
	}
	else if (addr == ports::TCNT1H)
	{
		return T16_latch;
	}
	else
	{
		return io[addr];
	}
}



// Performs hardware updates which have to be calculated at cycle precision
void avr8::update_hardware()
{
	cycleCounter ++;

	// timer1_next stores the cycles remaining until the next event on the
	// Timer1 16 bit timer. It can be cleared to zero whenever the timer's
	// state is changed (port writes). Locking it to zero should have no
	// effect, causing the timer to re-calculate its state proper on every
	// update_hardware call. It should only improve performance.

	if (timer1_next == 0U)
	{

		// Apply delayed timer interrupt flags

		TIFR1 |= itd_TIFR1;
		itd_TIFR1 = 0U;

		// Process timer

		if ((TCCR1B & 7U) != 0U) // If timer 1 is started
		{

			unsigned int OCR1A = OCR1AL | ((unsigned int)(OCR1AH) << 8);
			unsigned int OCR1B = OCR1BL | ((unsigned int)(OCR1BH) << 8);

			if(TCCR1B & WGM12) // Timer in CTC mode: count up to OCRnA then resets to zero
			{

				if (TCNT1 == 0xFFFFU)
				{
					itd_TIFR1 |= TOV1;
				}

				if (TCNT1 == OCR1B)
				{
					itd_TIFR1 |= OCF1B;
				}

				if (TCNT1 == OCR1A)
				{
					TCNT1 = 0U;
					itd_TIFR1 |= OCF1A;
				}
				else
				{
					TCNT1 = (TCNT1 + 1U) & 0xFFFFU;
				}

				// Calculate next timer event

				if (itd_TIFR1 == 0U)
				{
					timer1_next = 0xFFFFU - TCNT1;
					if ( (TCNT1 <= OCR1B) &&
					     (timer1_next > (OCR1B - TCNT1)) )
					{
						timer1_next = (OCR1B - TCNT1);
					}
					if ( (TCNT1 <= OCR1A) &&
					     (timer1_next > (OCR1A - TCNT1)) )
					{
						timer1_next = (OCR1A - TCNT1);
					}
				}

			}else{	//timer in normal mode: counts up to 0xffff then rolls over

				if (TCNT1 == 0xFFFFU)
				{
					itd_TIFR1 |= TOV1;
				}
				TCNT1 = (TCNT1 + 1U) & 0xFFFFU;

				// Calculate next timer event

				if (itd_TIFR1 == 0U)
				{
					timer1_next = 0xFFFFU - TCNT1;
				}

			}

		}

	}
	else
	{
		timer1_next --;
		TCNT1 ++;
	}

	// Apply delayed outputs

	if (dly_out != 0U)
	{
		if ((dly_out & DLY_TCCR1B) != 0U)
		{
			TCCR1B = dly_TCCR1B;
			timer1_next = 0U; // Timer state changes
		}
		if ((dly_out & DLY_TCNT1) != 0U)
		{
			TCNT1 = (dly_TCNT1H << 8) | dly_TCNT1L;
			timer1_next = 0U; // Timer state changes
		}
		dly_out = 0U;
	}

	// Draw pixel on scanline

	scanline_buf[current_cycle & 0x7FFU] = pixel_raw;
	current_cycle ++;
}



// Performs hardware updates which can be done at instruction precision
// Also process interrupt requests
inline void avr8::update_hardware_ins()
{
	// Get cycle count to emulate

	unsigned int cycles = cycleCounter - cycle_ctr_ins;
	cycle_ctr_ins = cycleCounter;

	// Notes:
	//
	// From this point if further cycles are required to be consumed,
	// those should be consumed using update_hardware(). This won't
	// increase this run's cycle count (cycles), but will show in the
	// next run proper.

	// Watchdog notes:
	//
	// This is a bare minimum implementation to make the Uzebox kernel's
	// seed generator operational (used for seeding a PRNG).

	if(WDTCSR & WDE){ //if watchdog enabled
		watchdogTimer += cycles;
		if(watchdogTimer>=DELAY16MS && (WDTCSR&WDIE)){
			WDTCSR|=WDIF;	//watchdog interrupt
			//reset watchdog
			//watchdog is based on a RC oscillator
			//so add some random variation to simulate entropy
			watchdogTimer=rand()%1024;
		}
	}

    // clock the SPI hardware.
    if((SPCR & 0x40) && SD_ENABLED()){ // only if SPI is enabled
        //TODO: test for master/slave modes (assume master for now)
        // TODO: factor in clock divider

        if(spiTransfer){
            if(spiClock <= cycles){
                //SPI_DEBUG("SPI transfer complete\n");
                update_spi();
                spiClock = 0;
                spiTransfer = 0;
                SPSR |= 0x80; // set interrupt
            }
            else{
                spiClock -= cycles;
            }
        }
            /*
        //HACK: instantaneous SPI access
        if(spiTransfer && spiClock > 0){
            update_spi();
            SPSR |= 0x80; // set interrupt
            spiTransfer = 0;
            spiClock = 0;
        }*/

        // test for interrupt (enable and interrupt flag for SPI)
        // TODO (Jubatian): Verify that the move is OK, if not, try to fix
        // it there (where the other interrupts are)
    }

    //clock the EEPROM hardware
    /*
    1. Wait until EEPE becomes zero.
    2. Wait until SELFPRGEN in SPMCSR becomes zero.
    3. Write new EEPROM address to EEAR (optional).
    4. Write new EEPROM data to EEDR (optional).
    5. Write a logical one to the EEMPE bit while writing a zero to EEPE in EECR.
    6. Within four clock cycles after setting EEMPE, write a logical one to EEPE.
    The EEPROM can not be programmed during a CPU write to the Flash memory.
    */
    // are we attempting to program?

	// TODO (Jubatian):
	//
	// cycleCounter is incremented here by 4, but this has no effect on at
	// least Timer 1 and the video output, maybe even more. Not like
	// writing to EEPROM would be a common task when drawing the video
	// frame, though.

    if(EECR & (EEPE|EERE))
    {
		if(EECR & EEPE){
			//printf("attempting write of EEPROM\n");
			cycleCounter += 4; // writes take four cycles
			int addr = (EEARH << 8) | EEARL;
			if(addr < eepromSize) eeprom[addr] = EEDR;
			EECR ^= (EEMPE | EEPE); // clear program bits

			// interrupt?
			//if((EECR & EERIE) && BIT(SREG,SREG_I)){
			//    SPSR ^= 0x80; // clear the interrupt
			//    trigger_interrupt(SPI_STC); // execute the vector
			//}
		}
		// are we attempting to read?
		else if(EECR & EERE){
		   // printf("attempting read of EEPROM\n");
			cycleCounter += 4; // eeprom reads take 4 additonal cycles
			int addr = (EEARH << 8) | EEARL;
			if(addr < eepromSize) EEDR = eeprom[addr];
			EECR ^= EERE; // clear read  bit

			// interrupt?
			//if((EECR & EERIE) && BIT(SREG,SREG_I)){
			//    SPSR ^= 0x80; // clear the interrupt
			//    trigger_interrupt(SPI_STC); // execute the vector
			//}
		}
    }

	// Process interrupts in order of priority

	if(SREG & (1<<SREG_I))
	{
		// Note (Jubatian):
		// The SD card's SPI interrupt trigger was within the SPI
		// handling part in update_hardware, however it belongs to
		// interrupt triggers. Priority order might be broken (but
		// essentially the emulator behaved according to this order
		// prior to this move).
		if((SPCR & 0x80) && (SPSR & 0x80))
		{
			// TODO: verify that SPI is dependent upon the global interrupt flag
			SPSR ^= 0x80; // Clear the interrupt
			trigger_interrupt(SPI_STC);
		}
		else if ((WDTCSR&(WDIF|WDIE))==(WDIF|WDIE))
		{
			WDTCSR&= ~WDIF; // Clear watchdog flag
			trigger_interrupt(WDT);
		}
		else if((TIFR1 & (OCF1A|OCF1B|TOV1)) && (TIMSK1&(OCIE1A|OCIE1B|TOIE1)))
		{
			if ((TIFR1 & OCF1A) && (TIMSK1 & OCIE1A) )
			{
				TIFR1&= ~OCF1A; // Clear CTC match flag
				trigger_interrupt(TIMER1_COMPA);
			}
			else if ((TIFR1 & OCF1B) && (TIMSK1 & OCIE1B))
			{
				TIFR1&= ~OCF1B; // Clear CTC match flag
				trigger_interrupt(TIMER1_COMPB);
			}
			else if ((TIFR1 & TOV1) && (TIMSK1 & TOIE1))
			{
				TIFR1&= ~TOV1; // Clear TOV1 flag
				trigger_interrupt(TIMER1_OVF);
			}
		}
	}
}


instructionList_t instructionList[] = {

{   1,"ADC    r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   1, 0b0001110000000000, 0b0000000111110000, 0b0000001000001111},
{   2,"ADD    r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   1, 0b0000110000000000, 0b0000000111110000, 0b0000001000001111},
{   3,"ADIW   r%d, %d "                ,   1,   2,  24,   0,   3,   1,   0,   0,   1,   2, 0b1001011000000000, 0b0000000000110000, 0b0000000011001111},
{   4,"AND    r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   1, 0b0010000000000000, 0b0000000111110000, 0b0000001000001111},
{   5,"ANDI   r%d, %d "                ,   1,   1,  16,   0,   3,   1,   0,   0,   1,   1, 0b0111000000000000, 0b0000000011110000, 0b0000111100001111},
{   6,"ASR    r%d "                    ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010000000101, 0b0000000111110000, 0b0000000000000000},
{   7,"BCLR   %d "                     ,   7,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010010001000, 0b0000000001110000, 0b0000000000000000},
{   8,"BLD    r%d, %d "                ,   1,   1,   0,   0,   6,   1,   0,   0,   1,   1, 0b1111100000000000, 0b0000000111110000, 0b0000000000000111},
{   9,"BRBC   %d, %d "                 ,   7,   1,   0,   0,   3,   1,   0,   1,   1,   2, 0b1111010000000000, 0b0000000000000111, 0b0000001111111000},
{  10,"BRBS   %d, %d "                 ,   7,   1,   0,   0,   3,   1,   0,   1,   1,   2, 0b1111000000000000, 0b0000000000000111, 0b0000001111111000},
{  11,"BREAK "                         ,   0,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010110011000, 0b0000000000000000, 0b0000000000000000},
{  12,"BSET   %d "                     ,   7,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010000001000, 0b0000000001110000, 0b0000000000000000},
{  13,"BST    r%d, %d "                ,   1,   1,   0,   0,   6,   1,   0,   0,   1,   1, 0b1111101000000000, 0b0000000111110000, 0b0000000000000111},
{  14,"CALL   %d (+ next word) "       ,   0,   1,   0,   0,   3,   1,   0,   0,   2,   4, 0b1001010000001110, 0b0000000000000000, 0b0000000111110001},
{  15,"CBI    io%d, %d "               ,   8,   1,   0,   0,   6,   1,   0,   0,   1,   2, 0b1001100000000000, 0b0000000011111000, 0b0000000000000111},
{  16,"COM    r%d "                    ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010000000000, 0b0000000111110000, 0b0000000000000000},
{  17,"CP     r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   1, 0b0001010000000000, 0b0000000111110000, 0b0000001000001111},
{  18,"CPC    r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   1, 0b0000010000000000, 0b0000000111110000, 0b0000001000001111},
{  19,"CPI    r%d, %d "                ,   1,   1,  16,   0,   3,   1,   0,   0,   1,   1, 0b0011000000000000, 0b0000000011110000, 0b0000111100001111},
{  20,"CPSE   r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   3, 0b0001000000000000, 0b0000000111110000, 0b0000001000001111},
{  21,"DEC    r%d "                    ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010000001010, 0b0000000111110000, 0b0000000000000000},
{  22,"EOR    r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   1, 0b0010010000000000, 0b0000000111110000, 0b0000001000001111},
{  23,"FMUL   r%d, r%d "               ,   1,   1,  16,   0,   2,   1,  16,   0,   1,   2, 0b0000001100001000, 0b0000000001110000, 0b0000000000000111},
{  24,"FMULS  r%d, r%d "               ,   1,   1,  16,   0,   2,   1,  16,   0,   1,   2, 0b0000001110000000, 0b0000000001110000, 0b0000000000000111},
{  25,"FMULSU r%d, r%d "               ,   1,   1,  16,   0,   2,   1,  16,   0,   1,   2, 0b0000001110001000, 0b0000000001110000, 0b0000000000000111},
{  26,"ICALL "                         ,   0,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001010100001001, 0b0000000000000000, 0b0000000000000000},
{  27,"IJMP "                          ,   0,   1,   0,   0,   0,   1,   0,   0,   1,   2, 0b1001010000001001, 0b0000000000000000, 0b0000000000000000},
{  28,"IN     r%d, io%d "              ,   1,   1,   0,   0,   8,   1,   0,   0,   1,   1, 0b1011000000000000, 0b0000000111110000, 0b0000011000001111},
{  29,"INC    r%d "                    ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010000000011, 0b0000000111110000, 0b0000000000000000},
{  30,"JMP    %d (+ next word) "       ,   0,   1,   0,   0,   3,   1,   0,   0,   2,   3, 0b1001010000001100, 0b0000000000000000, 0b0000000111110001},
{  31,"LD     r%d, -X "                ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001000000001110, 0b0000000111110000, 0b0000000000000000},
{  32,"LD     r%d, -Y "                ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001000000001010, 0b0000000111110000, 0b0000000000000000},
{  33,"LD     r%d, -Z "                ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001000000000010, 0b0000000111110000, 0b0000000000000000},
{  34,"LD     r%d, X "                 ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001000000001100, 0b0000000111110000, 0b0000000000000000},
{  35,"LD     r%d, X+ "                ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001000000001101, 0b0000000111110000, 0b0000000000000000},
{  36,"LD     r%d, Y+ "                ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001000000001001, 0b0000000111110000, 0b0000000000000000},
{  37,"LD     r%d, Y+%d "              ,   1,   1,   0,   0,   5,   1,   0,   0,   1,   3, 0b1000000000001000, 0b0000000111110000, 0b0010110000000111},
{  38,"LD     r%d, Z+ "                ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001000000000001, 0b0000000111110000, 0b0000000000000000},
{  39,"LD     r%d, Z+%d "              ,   1,   1,   0,   0,   5,   1,   0,   0,   1,   3, 0b1000000000000000, 0b0000000111110000, 0b0010110000000111},
{  40,"LDI    r%d, %d "                ,   1,   1,  16,   0,   3,   1,   0,   0,   1,   1, 0b1110000000000000, 0b0000000011110000, 0b0000111100001111},
{  41,"LDS    r%d, %d (+next word) "   ,   1,   1,   0,   0,   0,   1,   0,   0,   2,   2, 0b1001000000000000, 0b0000000111110000, 0b0000000000000000},
{  42,"LPM "                           ,   0,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001010111001000, 0b0000000000000000, 0b0000000000000000},
{  43,"LPM    r%d, Z "                 ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001000000000100, 0b0000000111110000, 0b0000000000000000},
{  44,"LPM    r%d, Z+ "                ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   3, 0b1001000000000101, 0b0000000111110000, 0b0000000000000000},
{  45,"LSR    r%d "                    ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010000000110, 0b0000000111110000, 0b0000000000000000},
{  46,"MOV    r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   1, 0b0010110000000000, 0b0000000111110000, 0b0000001000001111},
{  47,"MOVW   r%d, r%d "               ,   1,   2,   0,   0,   2,   2,   0,   0,   1,   1, 0b0000000100000000, 0b0000000011110000, 0b0000000000001111},
{  48,"MUL    r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   2, 0b1001110000000000, 0b0000000111110000, 0b0000001000001111},
{  49,"MULS   r%d, r%d "               ,   1,   1,  16,   0,   2,   1,  16,   0,   1,   2, 0b0000001000000000, 0b0000000011110000, 0b0000000000001111},
{  50,"MULSU  r%d, r%d "               ,   1,   1,  16,   0,   2,   1,  16,   0,   1,   2, 0b0000001100000000, 0b0000000001110000, 0b0000000000000111},
{  51,"NEG    r%d "                    ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010000000001, 0b0000000111110000, 0b0000000000000000},
{  52,"NOP "                           ,   0,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000},
{  53,"OR     r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   1, 0b0010100000000000, 0b0000000111110000, 0b0000001000001111},
{  54,"ORI    r%d, %d "                ,   1,   1,  16,   0,   3,   1,   0,   0,   1,   1, 0b0110000000000000, 0b0000000011110000, 0b0000111100001111},
{  55,"OUT    io%d, r%d "              ,   8,   1,   0,   0,   1,   1,   0,   0,   1,   1, 0b1011100000000000, 0b0000011000001111, 0b0000000111110000},
{  56,"POP    r%d "                    ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   2, 0b1001000000001111, 0b0000000111110000, 0b0000000000000000},
{  57,"PUSH   r%d "                    ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   2, 0b1001001000001111, 0b0000000111110000, 0b0000000000000000},
{  58,"RCALL  %d "                     ,   0,   1,   0,   0,   3,   1,   0,   1,   1,   3, 0b1101000000000000, 0b0000000000000000, 0b0000111111111111},
{  59,"RET "                           ,   0,   1,   0,   0,   0,   1,   0,   0,   1,   4, 0b1001010100001000, 0b0000000000000000, 0b0000000000000000},
{  60,"RETI "                          ,   0,   1,   0,   0,   0,   1,   0,   0,   1,   4, 0b1001010100011000, 0b0000000000000000, 0b0000000000000000},
{  61,"RJMP   %d "                     ,   0,   1,   0,   0,   3,   1,   0,   1,   1,   2, 0b1100000000000000, 0b0000000000000000, 0b0000111111111111},
{  62,"ROR    r%d "                    ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010000000111, 0b0000000111110000, 0b0000000000000000},
{  63,"SBC    r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   1, 0b0000100000000000, 0b0000000111110000, 0b0000001000001111},
{  64,"SBCI   r%d, %d "                ,   1,   1,  16,   0,   3,   1,   0,   0,   1,   1, 0b0100000000000000, 0b0000000011110000, 0b0000111100001111},
{  65,"SBI    io%d, %d "               ,   8,   1,   0,   0,   6,   1,   0,   0,   1,   2, 0b1001101000000000, 0b0000000011111000, 0b0000000000000111},
{  66,"SBIC   io%d, %d "               ,   8,   1,   0,   0,   6,   1,   0,   0,   1,   3, 0b1001100100000000, 0b0000000011111000, 0b0000000000000111},
{  67,"SBIS   io%d, %d "               ,   8,   1,   0,   0,   6,   1,   0,   0,   1,   3, 0b1001101100000000, 0b0000000011111000, 0b0000000000000111},
{  68,"SBIW   r%d, %d "                ,   1,   2,  24,   0,   3,   1,   0,   0,   1,   2, 0b1001011100000000, 0b0000000000110000, 0b0000000011001111},
{  69,"SBRC   r%d, %d "                ,   2,   1,   0,   0,   6,   1,   0,   0,   1,   3, 0b1111110000000000, 0b0000000111110000, 0b0000000000000111},
{  70,"SBRS   r%d, %d "                ,   2,   1,   0,   0,   6,   1,   0,   0,   1,   3, 0b1111111000000000, 0b0000000111110000, 0b0000000000000111},
{  71,"SLEEP "                         ,   0,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010110001000, 0b0000000000000000, 0b0000000000000000},
{  72,"SPM    z+ "                     ,   0,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010111101000, 0b0000000000000000, 0b0000000000000000},
{  73,"ST     -x, r%d "                ,   2,   1,   0,   0,   0,   1,   0,   0,   1,   2, 0b1001001000001110, 0b0000000111110000, 0b0000000000000000},
{  74,"ST     -y, r%d "                ,   2,   1,   0,   0,   0,   1,   0,   0,   1,   2, 0b1001001000001010, 0b0000000111110000, 0b0000000000000000},
{  75,"ST     -z, r%d "                ,   2,   1,   0,   0,   0,   1,   0,   0,   1,   2, 0b1001001000000010, 0b0000000111110000, 0b0000000000000000},
{  76,"ST     x, r%d "                 ,   2,   1,   0,   0,   0,   1,   0,   0,   1,   2, 0b1001001000001100, 0b0000000111110000, 0b0000000000000000},
{  77,"ST     x+, r%d "                ,   2,   1,   0,   0,   0,   1,   0,   0,   1,   2, 0b1001001000001101, 0b0000000111110000, 0b0000000000000000},
{  78,"ST     y+, r%d "                ,   2,   1,   0,   0,   0,   1,   0,   0,   1,   2, 0b1001001000001001, 0b0000000111110000, 0b0000000000000000},
{  79,"ST     y+q, r%d (q=%d) "        ,   1,   1,   0,   0,   5,   1,   0,   0,   1,   2, 0b1000001000001000, 0b0000000111110000, 0b0010110000000111},
{  80,"ST     z+, r%d "                ,   2,   1,   0,   0,   0,   1,   0,   0,   1,   2, 0b1001001000000001, 0b0000000111110000, 0b0000000000000000},
{  81,"ST     z+q, r%d (q=%d) "        ,   1,   1,   0,   0,   5,   1,   0,   0,   1,   2, 0b1000001000000000, 0b0000000111110000, 0b0010110000000111},
{  82,"STS    k, r%d "                 ,   1,   1,   0,   0,   0,   1,   0,   0,   2,   2, 0b1001001000000000, 0b0000000111110000, 0b0000000000000000},
{  83,"SUB    r%d, r%d "               ,   1,   1,   0,   0,   2,   1,   0,   0,   1,   1, 0b0001100000000000, 0b0000000111110000, 0b0000001000001111},
{  84,"SUBI   r%d, %d "                ,   1,   1,  16,   0,   3,   1,   0,   0,   1,   1, 0b0101000000000000, 0b0000000011110000, 0b0000111100001111},
{  85,"SWAP   r%d "                    ,   1,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010000000010, 0b0000000111110000, 0b0000000000000000},
{  86,"WDR "                           ,   0,   1,   0,   0,   0,   1,   0,   0,   1,   1, 0b1001010110101000, 0b0000000000000000, 0b0000000000000000},


{   0,"END"                            ,   0,   0,   0,   0,   0,   0,   0,   0,  0,   0, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000}

};

unsigned int avr8::exec()
{

	currentPc=pc;
	const instructionDecode_t insnDecoded = progmemDecoded[pc];
	const u8  opNum  = insnDecoded.opNum;
	const u8  arg1_8 = insnDecoded.arg1;
	const s16 arg2_8 = insnDecoded.arg2;

	const unsigned int startcy = cycleCounter;
	u8 Rd, Rr, R, CH;
	u16 uTmp, Rd16, R16;
	s16 sTmp;

	//GDB must be first
	if (enableGdb == true)
	{
		gdb->exec();
	
		// Check if the next instruction match a GDB breakpoint
		Breakpoints::iterator ii;
  		if ((ii= find(gdb->BP.begin(), gdb->BP.end(), pc)) != gdb->BP.end())
		{
			gdbBreakpointFound = true;
			return 0;
		}
	}

	if (state == CPU_STOPPED)
		return 0;

	//Program counter must be incremented *after* GDB
	pc++;



	// Instruction decoder notes:
	//
	// The instruction's timing is determined by how many update_hardware
	// calls are executed during its decoding. One update_hardware call is
	// placed after the instruction decoder since all instructions take at
	// least one cycle (there is at least one cycle after the last read /
	// write effect), so only multi-cycle instructions need to perform
	// extra calls to update_hardware.
	//
	// The read_sram and write_sram calls only access the sram.
	// TODO: I am not sure whether the instructions calling these only do
	// so on the real thing, but doing otherwise is unlikely, and may even
	// be buggy then (the behavior of things like having the stack over IO
	// area...). This solution is at least fast for these instructions.

	switch (opNum){

		case  1: // 0001 11rd dddd rrrr		(1) ADC Rd,Rr (ROL is ADC Rd,Rd)
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			R = Rd + Rr + C;
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM | SREG_HM);
			UPDATE_HC_ADD; UPDATE_SVN_ADD; UPDATE_Z;
			r[arg1_8] = R;
			break;

		case  2: // 0000 11rd dddd rrrr		(1) ADD Rd,Rr (LSL is ADD Rd,Rd)
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			R = Rd + Rr;
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM | SREG_HM);
			UPDATE_HC_ADD; UPDATE_SVN_ADD; UPDATE_Z;
			r[arg1_8] = R;
			break;

		case  3: // 1001 0110 KKdd KKKK		(2) ADIW Rd+1:Rd,K   (16-bit add to upper four register pairs)
			Rd = arg1_8;
			Rr = arg2_8;
			Rd16 = r[Rd] | (r[Rd+1]<<8);
			R16 = Rd16 + Rr;
			r[Rd] = (u8)R16;
			r[Rd+1] = (u8)(R16>>8);
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			set_bit_1(SREG,SREG_V,((~Rd16&R16)&0x8000) >> 15);
			set_bit_1(SREG,SREG_N,(R16&0x8000) >> 15);
			UPDATE_S;
			set_bit_inv(SREG,SREG_Z,R16);
			set_bit_1(SREG,SREG_C,((~R16&Rd16)&0x8000) >> 15);
			update_hardware();
			break;

		case  4: // 0010 00rd dddd rrrr		(1) AND Rd,Rr (TST is AND Rd,Rd)
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			R = Rd & Rr;
			clr_bits(SREG, SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			UPDATE_SVN_LOGICAL; UPDATE_Z;
			r[arg1_8] = R;
			break;

		case  5: // 0111 KKKK dddd KKKK		(1) ANDI Rd,K (CBR is ANDI with K complemented)
			Rd = r[arg1_8];
			Rr = arg2_8;
			R = Rd & Rr;
			clr_bits(SREG, SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			UPDATE_SVN_LOGICAL; UPDATE_Z;
			r[arg1_8] = R;
			break;

		case  6: // 1001 010d dddd 0101		(1) ASR Rd
			Rd = r[arg1_8];
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			set_bit_1(SREG,SREG_C,Rd&1);
			r[arg1_8] = R = (Rd >> 1) | (Rd & 0x80);
			UPDATE_N;
			set_bit_1(SREG,SREG_V,(R>>7)^(Rd&1));
			UPDATE_S;
			UPDATE_Z;
			break;

		case  8: // 1111 100d dddd 0bbb		(1) BLD Rd,b
			Rd = arg1_8;
			store_bit_1(r[Rd],arg2_8,(SREG >> SREG_T) & 1U);
			break;

		case  7: // 1001 0100 1sss 1000		(1) BCLR s (CLC, etc are aliases with sss implicit)
			Rd = arg1_8;
			SREG &= ~(1U << Rd);
			break;

		case  9: // 1111 01kk kkkk ksss		(1/2) BRBC s,k (BRCC, etc are aliases for this with sss implicit)
			if (!(SREG & (1<<(arg1_8))))
			{
				update_hardware();
				pc += arg2_8;
			}
			break;

		case  10: // 1111 00kk kkkk ksss		(1/2) BRBS s,k (same here)
			if (SREG & (1<<(arg1_8)))
			{
				update_hardware();
				pc += arg2_8;
			}
			break;

		case  11: // 1001 0101 1001 1000		(?) BREAK
			// no operation
			break;

		case  12: // 1001 0100 0sss 1000		(1) BSET s (SEC, etc are aliases with sss implicit)
			Rd = arg1_8;
			SREG |= (1U << Rd);
			break;

		case  13: // 1111 101d dddd 0bbb		(1) BST Rd,b
			Rd = r[arg1_8];
			store_bit_1(SREG,SREG_T,(Rd >> (arg2_8)) & 1U);
			break;

		case  14: // 1001 010k kkkk 111k		(4) CALL k (next word is rest of address)
			// Note: 64K progmem, so 'k' in first insn word is unused
			update_hardware();
			update_hardware();
			update_hardware();
			write_sram(SP,(pc+1));
			DEC_SP;
			write_sram(SP,(pc+1)>>8);
			DEC_SP;
			pc = arg2_8;
			break;

		case  15: // 1001 1000 AAAA Abbb		(2) CBI A,b
			update_hardware();
			Rd = arg1_8;
			write_io(Rd, read_io(Rd) & ~(1<<(arg2_8)));
			break;

		case  16: // 1001 010d dddd 0000		(1) COM Rd
			r[arg1_8] = R = ~r[arg1_8];
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			UPDATE_SVN_LOGICAL; UPDATE_Z; SET_C;
			break;

		case  17: // 0001 01rd dddd rrrr		(1) CP Rd,Rr
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			R = Rd - Rr;
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM | SREG_HM);
			UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_Z;
			break;

		case  18: // 0000 01rd dddd rrrr		(1) CPC Rd,Rr
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			R = Rd - Rr - C;
			clr_bits(SREG, SREG_CM | SREG_NM | SREG_VM | SREG_SM | SREG_HM);
			UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_CLEAR_Z;
			break;

		case  19: // 0011 KKKK dddd KKKK		(1) CPI Rd,K
			Rd = r[arg1_8];
			Rr = arg2_8;
			R = Rd - Rr;
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM | SREG_HM);
			UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_Z;
			break;

		case  20: // 0001 00rd dddd rrrr		(1/2/3) CPSE Rd,Rr
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			if (Rd == Rr)
			{
				unsigned int icc = get_insn_size(progmemDecoded[pc].opNum);
				pc += icc;
				while (icc != 0U)
				{
					update_hardware();
					icc --;
				}
			}
			break;

		case  21: // 1001 010d dddd 1010		(1) DEC Rd
			R = --r[arg1_8];
			clr_bits(SREG, SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			UPDATE_N;
			set_bit_inv(SREG,SREG_V,(unsigned int)(R) - 0x7FU);
			UPDATE_S;
			UPDATE_Z;
			break;

		case  22: // 0010 01rd dddd rrrr		(1) EOR Rd,Rr (CLR is EOR Rd,Rd)
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			R = Rd ^ Rr;
			clr_bits(SREG, SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			UPDATE_SVN_LOGICAL; UPDATE_Z;
			r[arg1_8] = R;
			break;
		
		case  23: // 0000 0011 0ddd 1rrr		(2) FMUL Rd,Rr (registers are in 16-23 range)
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			uTmp = (u8)Rd * (u8)Rr;
			r0 = (u8)(uTmp << 1);
			r1 = (u8)(uTmp >> 7);
			clr_bits(SREG, SREG_CM | SREG_ZM);
			UPDATE_CZ_MUL(uTmp);
			update_hardware();
			break;

		case  24: // 0000 0011 1ddd 0rrr		(2) FMULS Rd,Rr
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			sTmp = (s8)Rd * (s8)Rr;
			r0 = (u8)(sTmp << 1);
			r1 = (u8)(sTmp >> 7);
			clr_bits(SREG, SREG_CM | SREG_ZM);
			UPDATE_CZ_MUL(sTmp);
			update_hardware();
			break;

		case  25: // 0000 0011 1ddd 1rrr		(2) FMULSU Rd,Rr
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			sTmp = (s8)Rd * (u8)Rr;
			r0 = (u8)(sTmp << 1);
			r1 = (u8)(sTmp >> 7);
			clr_bits(SREG, SREG_CM | SREG_ZM);
			UPDATE_CZ_MUL(sTmp);
			update_hardware();
			break;

		case  26: // 1001 0101 0000 1001		(3) ICALL (call thru Z register)
			update_hardware();
			update_hardware();
			write_sram(SP,u8(pc));
			DEC_SP;
			write_sram(SP,(pc)>>8);
			DEC_SP;
			pc = Z;
			break;

		case  27: // 1001 0100 0000 1001		(2) IJMP (jump thru Z register)
			update_hardware();
			pc = Z;
			break;

		case  28: // 1011 0AAd dddd AAAA		(1) IN Rd,A
			Rd = arg1_8;
			Rr = arg2_8;
			r[Rd] = read_io(Rr);
			break;

		case  29: // 1001 010d dddd 0011		(1) INC Rd
			R = ++r[arg1_8];
			clr_bits(SREG, SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			UPDATE_N;
			set_bit_inv(SREG,SREG_V,(unsigned int)(R) - 0x80U);
			UPDATE_S;
			UPDATE_Z;
			break;

		case  30: // 1001 010k kkkk 110k		(3) JMP k (next word is rest of address)
			// Note: 64K progmem, so 'k' in first insn word is unused
			update_hardware();
			update_hardware();
			pc = arg2_8;
			break;

		case  31: // 1001 000d dddd 1110		(2) LD rd,-X
			update_hardware();
			DEC_X;
			r[arg1_8] = read_sram_io(X);
			break;

		case  32: // 1001 000d dddd 1010		(2) LD Rd,-Y
			update_hardware();
			DEC_Y;
			r[arg1_8] = read_sram_io(Y);
			break;

		case  33: // 1001 000d dddd 0010		(2) LD Rd,-Z
			update_hardware();
			DEC_Z;
			r[arg1_8] = read_sram_io(Z);
			break;

		case  34: // 1001 000d dddd 1100		(2) LD rd,X
			update_hardware();
			r[arg1_8] = read_sram_io(X);
			break;

		case  35: // 1001 000d dddd 1101		(2) LD rd,X+
			update_hardware();
			r[arg1_8] = read_sram_io(X);
			INC_X;
			break;

		case  36: // 1001 000d dddd 1001		(2) LD Rd,Y+
			update_hardware();
			r[arg1_8] = read_sram_io(Y);
			INC_Y;
			break;

		case  37: // 10q0 qq0d dddd 1qqq		(2) LDD Rd,Y+q
			update_hardware();
			Rd = arg1_8;
			Rr = arg2_8;
			r[Rd] = read_sram_io(Y + Rr);
			break;

		case  38: // 1001 000d dddd 0001		(2) LD Rd,Z+
			update_hardware();
			r[arg1_8] = read_sram_io(Z);
			INC_Z;
			break;

		case  39: // 10q0 qq0d dddd 0qqq		(2) LDD Rd,Z+q
			update_hardware();
			Rd = arg1_8;
			Rr = arg2_8;
			r[Rd] = read_sram_io(Z + Rr);
			break;

		case  40: // 1110 KKKK dddd KKKK		(1) LDI Rd,K (SER is just LDI Rd,255)
			r[arg1_8] = arg2_8;
			break;

		case  41: // 1001 000d dddd 0000		(2) LDS Rd,k (next word is rest of address)
			update_hardware();
			r[arg1_8] = read_sram_io(arg2_8);
			pc++;
			break;

		case  42: // 1001 0101 1100 1000		(3) LPM (r0 implied, why is this special?)
			update_hardware();
			update_hardware();
			r0 = read_progmem(Z);
			break;

		case  43: // 1001 000d dddd 0100		(3) LPM Rd,Z
			update_hardware();
			update_hardware();
			r[arg1_8] = read_progmem(Z);
			break;

		case  44: // 1001 000d dddd 0101		(3) LPM Rd,Z+
			update_hardware();
			update_hardware();
			r[arg1_8] = read_progmem(Z);
			INC_Z;
			break;

		case  45: // 1001 010d dddd 0110		(1) LSR Rd
			Rd = r[arg1_8];
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			set_bit_1(SREG,SREG_C,Rd&1);
			r[arg1_8] = R = (Rd >> 1);
			UPDATE_N;
			set_bit_1(SREG,SREG_V,Rd&1);
			UPDATE_S;
			UPDATE_Z;
			break;

		case  46: // 0010 11rd dddd rrrr		(1) MOV Rd,Rr
			r[arg1_8]  = r[arg2_8];
			break;

		case  47: // 0000 0001 dddd rrrr		(1) MOVW Rd+1:Rd,Rr+1:R
			Rd = arg1_8;
			Rr = arg2_8;
			r[Rd] = r[Rr];
			r[Rd+1] = r[Rr+1];
			break;

		case  48: // 1001 11rd dddd rrrr		(2) MUL Rd,Rr
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			uTmp = Rd * Rr;
			r0 = (u8)uTmp;
			r1 = (u8)(uTmp >> 8);
			clr_bits(SREG, SREG_CM | SREG_ZM);
			UPDATE_CZ_MUL(uTmp);
			update_hardware();
			break;

		case  49: // 0000 0010 dddd rrrr		(2) MULS Rd,Rr
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			sTmp = (s8)Rd * (s8)Rr;
			r0 = (u8)sTmp;
			r1 = (u8)(sTmp >> 8);
			clr_bits(SREG, SREG_CM | SREG_ZM);
			UPDATE_CZ_MUL(sTmp);
			update_hardware();
			break;

		case  50: // 0000 0011 0ddd 0rrr		(2) MULSU Rd,Rr (registers are in 16-23 range)
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			sTmp = (s8)Rd * (u8)Rr;
			r0 = (u8)sTmp;
			r1 = (u8)(sTmp >> 8);
			clr_bits(SREG, SREG_CM | SREG_ZM);
			UPDATE_CZ_MUL(sTmp);
			update_hardware();
			break;

		case  51: // 1001 010d dddd 0001		(1) NEG Rd
			Rr = r[arg1_8];
			Rd = 0;
			r[arg1_8] = R = Rd - Rr;
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM | SREG_HM);
			UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_Z;
			break;

		case  52: // 0000 0000 0000 0000		(1) NOP
			break;

		case  53: // 0010 10rd dddd rrrr		(1) OR Rd,Rr
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			R = Rd | Rr;
			clr_bits(SREG, SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			UPDATE_SVN_LOGICAL; UPDATE_Z;
			r[arg1_8] = R;
			break;

		case  54: // 0110 KKKK dddd KKKK		(1) ORI Rd,K (same as SBR insn)
			Rd = r[arg1_8];
			Rr = arg2_8;
			R = Rd | Rr;
			clr_bits(SREG, SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			UPDATE_SVN_LOGICAL; UPDATE_Z;
			r[arg1_8] = R;
			break;

		case  55: // 1011 1AAd dddd AAAA		(1) OUT A,Rd
			Rd = arg2_8;
			Rr = arg1_8;
			write_io(Rr,r[Rd]);
			break;

		case  56: // 1001 000d dddd 1111		(2) POP Rd
			update_hardware();
			INC_SP;
			r[arg1_8] = read_sram(SP);
			break;

		case  57: // 1001 001d dddd 1111		(2) PUSH Rd
			update_hardware();
			write_sram(SP,r[arg1_8]);
			DEC_SP;
			break;

		case  58: // 1101 kkkk kkkk kkkk		(3) RCALL k
			update_hardware();
			update_hardware();
			write_sram(SP,(u8)pc);
			DEC_SP;
			write_sram(SP,pc>>8);
			DEC_SP;
			pc += arg2_8;
			break;

		case  59: // 1001 0101 0000 1000		(4) RET
			update_hardware();
			update_hardware();
			update_hardware();
			INC_SP;
			pc = read_sram(SP) << 8;
			INC_SP;
			pc |= read_sram(SP);
			break;

		case  60: // 1001 0101 0001 1000		(4) RETI
			update_hardware();
			update_hardware();
			update_hardware();
			INC_SP;
			pc = read_sram(SP) << 8;
			INC_SP;
			pc |= read_sram(SP);
			SREG |= (1<<SREG_I);
			//--interruptLevel;
			break;

		case  61: // 1100 kkkk kkkk kkkk		(2) RJMP k
			update_hardware();
			pc += arg2_8;
			break;

		case  62: // 1001 010d dddd 0111		(1) ROR Rd
			Rd = r[arg1_8];
			r[arg1_8] = R = (Rd >> 1) | ((SREG&1)<<7);
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			set_bit_1(SREG,SREG_C,Rd&1);
			UPDATE_N;
			set_bit_1(SREG,SREG_V,(R>>7)^(Rd&1));
			UPDATE_S;
			UPDATE_Z;
			break;

		case  63: // 0000 10rd dddd rrrr		(1) SBC Rd,Rr
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			R = Rd - Rr - C;
			clr_bits(SREG, SREG_CM | SREG_NM | SREG_VM | SREG_SM | SREG_HM);
			UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_CLEAR_Z;
			r[arg1_8] = R;
			break;

		case  64: // 0100 KKKK dddd KKKK		(1) SBCI Rd,K
			Rd = r[arg1_8];
			Rr = arg2_8;
			R = Rd - Rr - C;
			clr_bits(SREG, SREG_CM | SREG_NM | SREG_VM | SREG_SM | SREG_HM);
			UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_CLEAR_Z;
			r[arg1_8] = R;
			break;

		case  65: // 1001 1010 AAAA Abbb		(2) SBI A,b
			update_hardware();
			Rd = arg1_8;
			write_io(Rd, read_io(Rd) | (1<<(arg2_8)));
			break;

		case  66: // 1001 1001 AAAA Abbb		(1/2/3) SBIC A,b
			Rd = arg1_8;
			if (!(read_io(Rd) & (1<<(arg2_8))))
			{
				unsigned int icc = get_insn_size(progmemDecoded[pc].opNum);
				pc += icc;
				while (icc != 0U)
				{
					update_hardware();
					icc --;
				}
			}
			break;

		case  67: // 1001 1011 AAAA Abbb		(1/2/3) SBIS A,b
			Rd = arg1_8;
			if (read_io(Rd) & (1<<(arg2_8)))
			{
				unsigned int icc = get_insn_size(progmemDecoded[pc].opNum);
				pc += icc;
				while (icc != 0U)
				{
					update_hardware();
					icc --;
				}
			}
			break;

		case  68: // 1001 0111 KKdd KKKK		(2) SBIW Rd+1:Rd,K
			Rd = arg1_8;
			Rr = arg2_8;
			Rd16 = r[Rd] | (r[Rd+1]<<8);
			R16 = Rd16 - Rr;
			r[Rd] = (u8)R16;
			r[Rd+1] = (u8)(R16>>8);
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM);
			set_bit_1(SREG,SREG_V,((Rd16&~R16)&0x8000) >> 15);
			set_bit_1(SREG,SREG_N,(R16&0x8000) >> 15);
			UPDATE_S;
			set_bit_inv(SREG,SREG_Z,R16);
			set_bit_1(SREG,SREG_C,((R16&~Rd16)&0x8000) >> 15);
			update_hardware();
			break;

		case  69: // 1111 110r rrrr 0bbb		(1/2/3) SBRC Rr,b
			Rd = r[arg1_8];
			if (((Rd >> (arg2_8)) & 1U) == 0)
			{
				unsigned int icc = get_insn_size(progmemDecoded[pc].opNum);
				pc += icc;
				while (icc != 0U)
				{
					update_hardware();
					icc --;
				}
			}
			break;

		case  70: // 1111 111r rrrr 0bbb		(1/2/3) SBRS Rr,b
			Rd = r[arg1_8];
			if (((Rd >> (arg2_8)) & 1U) == 1)
			{
				unsigned int icc = get_insn_size(progmemDecoded[pc].opNum);
				pc += icc;
				while (icc != 0U)
				{
					update_hardware();
					icc --;
				}
			}
			break;

		case  71: // 1001 0101 1000 1000		(?) SLEEP
			elapsedCyclesSleep=cycleCounter-lastCyclesSleep;
			lastCyclesSleep=cycleCounter;
			break;

		case  72: // 1001 0101 1110 1000		(?) SPM Z (writes R1:R0)
			update_hardware();
			update_hardware(); // Cycle count undocumented?!?!?
			update_hardware(); // (4 cycles emulated)
			if (Z >= progSize/2)
			{
				fprintf(stderr,"illegal write to progmem addr %x\n",Z);
				shutdown(1);
			}else{
				progmem[Z] = r0 | (r1<<8);
				decodeFlash(Z-1);
				decodeFlash(Z);
			}
			break;

		case  73: // 1001 001r rrrr 1110		(2) ST -X,Rr
			update_hardware();
			DEC_X;
			write_sram_io(X,r[arg1_8]);
			break;

		case  74: // 1001 001r rrrr 1010		(2) ST -Y,Rr
			update_hardware();
			DEC_Y;
			write_sram_io(Y,r[arg1_8]);
			break;

		case  75: // 1001 001r rrrr 0010		(2) ST -Z,Rr
			update_hardware();
			DEC_Z;
			write_sram_io(Z,r[arg1_8]);
			break;

		case  76: // 1001 001r rrrr 1100		(2) ST X,Rr
			update_hardware();
			write_sram_io(X,r[arg1_8]);
			break;

		case  77: // 1001 001r rrrr 1101		(2) ST X+,Rr
			update_hardware();
			write_sram_io(X,r[arg1_8]);
			INC_X;
			break;

		case  78: // 1001 001r rrrr 1001		(2) ST Y+,Rr
			update_hardware();
			write_sram_io(Y,r[arg1_8]);
			INC_Y;
			break;

		case  79: // 10q0 qq1d dddd 1qqq		(2) STD Y+q,Rd
			Rd = arg1_8;
			Rr = arg2_8;
			update_hardware();
			write_sram_io(Y + Rr, r[Rd]);
			break;

		case  80: // 1001 001r rrrr 0001		(2) ST Z+,Rr
			update_hardware();
			write_sram_io(Z,r[arg1_8]);
			INC_Z;
			break;

		case  81: // 10q0 qq1d dddd 0qqq		(2) STD Z+q,Rd
			Rd = arg1_8;
			Rr = arg2_8;
			update_hardware();
			write_sram_io(Z + Rr, r[Rd]);
			break;

		case  82: // 1001 001d dddd 0000		(2) STS k,Rr (next word is rest of address)
			update_hardware();
			write_sram_io(arg2_8,r[arg1_8]);
			pc++;
			break;

		case  83: // 0001 10rd dddd rrrr		(1) SUB Rd,Rr
			Rd = r[arg1_8];
			Rr = r[arg2_8];
			R = Rd - Rr;
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM | SREG_HM);
			UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_Z;
			r[arg1_8] = R;
			break;

		case  84: // 0101 KKKK dddd KKKK		(1) SUBI Rd,K
			Rd = r[arg1_8];
			Rr = arg2_8;
			R = Rd - Rr;
			clr_bits(SREG, SREG_CM | SREG_ZM | SREG_NM | SREG_VM | SREG_SM | SREG_HM);
			UPDATE_HC_SUB; UPDATE_SVN_SUB; UPDATE_Z;
			r[arg1_8] = R;
			break;

		case  85: // 1001 010d dddd 0010		(1) SWAP Rd
			Rd = r[arg1_8];
			r[arg1_8] = (Rd >> 4) | (Rd << 4);
			break;

		case  86: // 1001 0101 1010 1000		(1) WDR
			//watchdog is based on a RC oscillator
			//so add some random variation to simulate entropy
			watchdogTimer=rand()%1024;
			if(prevWDR){
				printf("WDR measured %u cycles\n", cycleCounter - prevWDR);
				prevWDR = 0;
			}else{
				prevWDR = cycleCounter + 1;
			}
			break;

		default: // Illegal op.
			ILLEGAL_OP;
			break;
	}

	// Process hardware for the last instruction cycle

	update_hardware();

	// Run instruction precise emulation tasks

	update_hardware_ins();

	// Done, return cycles consumed during the processing of this instruction.

	return cycleCounter - startcy;
}

u16 avr8::decodeArg(u16 flash, u16 argMask, u8 argNeg){

	u16 argMaskShift = 0x0001;
	u16 decodeShift = 0x0001;
	u16 arg = 0x0000;

	while (argMaskShift != 0x4000){  //0x4000 is highest bit in argMask that can be set
		if((argMaskShift & argMask) != 0){
			if((argMaskShift & flash) != 0){
				arg = arg | decodeShift;
			}
			decodeShift = decodeShift << 1 ;
		}
		argMaskShift = argMaskShift<<1;
	}
	decodeShift = decodeShift >>1;
	
	if((argNeg == 1) && ((decodeShift & arg) != 0)) {
		while(decodeShift != 0x8000){
			decodeShift = decodeShift << 1 ;
			arg = arg | decodeShift;
		}
	}
	
	return(arg);
}

void avr8::instructionDecode(u16 address){

	int i = 0;
	u16 rawFlash;
	u16 thisMask;
	u16 arg1;
	u16 arg2;

	rawFlash = progmem[address];

	instructionDecode_t thisInst;

	thisInst.opNum = 0;
	thisInst.arg1  = 0;
	thisInst.arg2  = 0;

	while(instructionList[i].opNum != 0){
		thisMask = ~(instructionList[i].arg1Mask | instructionList[i].arg2Mask);

		if((rawFlash & thisMask) == instructionList[i].mask){

			arg1 = (decodeArg(rawFlash, instructionList[i].arg1Mask, instructionList[i].arg1Neg) * instructionList[i].arg1Mul) + instructionList[i].arg1Offset;
			arg2 = (decodeArg(rawFlash, instructionList[i].arg2Mask, instructionList[i].arg2Neg) * instructionList[i].arg2Mul) + instructionList[i].arg2Offset;

			if (instructionList[i].words == 2) { // the 2 word instructions have k16 as the 2nd word of total 32bit instruction
				arg2 = progmem[address+1];
			}

			//fprintf(stdout, instructionList[i].opName, arg1, arg2);
			//fprintf(stdout, "\n");

			thisInst.opNum = instructionList[i].opNum;
			thisInst.arg1  = arg1;
			thisInst.arg2  = arg2;
					
			progmemDecoded[address] = thisInst;	
			return;
		}
		i++;
	}
	return;
}

void avr8::decodeFlash(void){
	for(u16 i=0; i<(progSize/2); i++){
		decodeFlash(i);
	}
}
void avr8::decodeFlash(u16 address){
	
	if (address < (progSize/2)) {
		instructionDecode(address);
	}
}

void avr8::trigger_interrupt(unsigned int location)
{

		// clear interrupt flag
		store_bit_1(SREG,SREG_I,0);

		// push current PC
		write_sram(SP,(u8)pc);
		DEC_SP;
		write_sram(SP,pc>>8);
		DEC_SP;

		// jump to new location (which jumps to the real handler)
		pc = location;

		// bill the cycles consumed (3 cycles).
		// Note  that there is an error in the Atmega644 datasheet where
		// it specifies the IRQ cycles as 5.
		// see: http://www.avrfreaks.net/forum/interrupt-timing-conundrum
		update_hardware();
		update_hardware();
		update_hardware();

}

bool avr8::init_sd()
{
	if (SDemulator.init_with_directory(SDpath) < 0) {
		return false;
	}
	SDPartitionEntry entry;
	sectorSize = 512;

	entry.state = 0x00;
	entry.startCylinder = 0;//TODO
	entry.startHead = 0x00; //TODO
	entry.startCylinder = 0x0002; //TODO
	entry.type = 0x06; // FAT16 >32MB
	entry.endHead = 0; //TODO
	entry.endCylinder = 0; //TODO
	entry.sectorOffset = 1;
	entry.sectorCount = 4294967296/512; // TODO, update with more realistic info

	SDBuildMBR(&entry);

	return true;
}


bool avr8::init_gui()
{
	if ( SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0 )
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return false;
	}

	atexit(SDL_Quit);
	init_joysticks();

	window = SDL_CreateWindow(caption,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,630,448,fullscreen?SDL_WINDOW_FULLSCREEN:SDL_WINDOW_RESIZABLE);
	if (!window){
		fprintf(stderr, "CreateWindow failed: %s\n", SDL_GetError());
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, sdl_flags);
	if (!renderer){
		SDL_DestroyWindow(window);
		fprintf(stderr, "CreateRenderer failed: %s\n", SDL_GetError());
		return false;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_RenderSetLogicalSize(renderer, 630, 448);

	surface = SDL_CreateRGBSurface(0, VIDEO_DISP_WIDTH, 224, 32, 0, 0, 0, 0);
	if(!surface){
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		return false;
	}

	texture = SDL_CreateTexture(renderer,surface->format->format,SDL_TEXTUREACCESS_STATIC,surface->w,surface->h);
	if (!texture){
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		fprintf(stderr, "CreateTexture failed: %s\n", SDL_GetError());
		return false;
	}

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);


	if (fullscreen)
	{
		SDL_ShowCursor(0);
	}

	// Open audio driver
	SDL_AudioSpec desired;
	memset(&desired, 0, sizeof(desired));
	desired.freq = 15734;
	desired.format = AUDIO_U8;
	desired.callback = audio_callback_stub;
	desired.userdata = this;
	desired.channels = 1;
	desired.samples = 512;
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

	current_cycle = 0U;
	scanline_top = -33-5;
	scanline_count = -999;
	//Syncronized with the kernel, this value now results in the image 
	//being perfectly centered in both the emulator and a real TV
	left_edge = VIDEO_LEFT_EDGE;

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
		palette[i] = SDL_MapRGB(surface->format, red, green, blue);
	}
	
	hsync_more_col=SDL_MapRGB(surface->format, 255,0, 0); //red
	hsync_less_col=SDL_MapRGB(surface->format, 255,255, 0); //yellow

	if (recordMovie){

		if (avconv_video == NULL){
			// Detect the pixel format that the GPU picked for optimal speed
			char pix_fmt[] = "aaaa";
			switch (surface->format->Rmask) {
			case 0xff000000: pix_fmt[3] = 'r'; break;
			case 0x00ff0000: pix_fmt[2] = 'r'; break;
			case 0x0000ff00: pix_fmt[1] = 'r'; break;
			case 0x000000ff: pix_fmt[0] = 'r'; break;
			}
			switch (surface->format->Gmask) {
			case 0xff000000: pix_fmt[3] = 'g'; break;
			case 0x00ff0000: pix_fmt[2] = 'g'; break;
			case 0x0000ff00: pix_fmt[1] = 'g'; break;
			case 0x000000ff: pix_fmt[0] = 'g'; break;
			}
			switch (surface->format->Bmask) {
			case 0xff000000: pix_fmt[3] = 'b'; break;
			case 0x00ff0000: pix_fmt[2] = 'b'; break;
			case 0x0000ff00: pix_fmt[1] = 'b'; break;
			case 0x000000ff: pix_fmt[0] = 'b'; break;
			}
			printf("Pixel Format = %s\n", pix_fmt);
			char avconv_video_cmd[1024] = {0};
			snprintf(avconv_video_cmd, sizeof(avconv_video_cmd) - 1, "ffmpeg -y -f rawvideo -s %ux224 -pix_fmt %s -r 59.94 -i - -vf scale=960:720 -sws_flags neighbor -an -preset ultrafast -qp 0 -tune animation uzemtemp.mp4", VIDEO_DISP_WIDTH, pix_fmt);
			avconv_video = popen(avconv_video_cmd, "w");
		}
		if (avconv_video == NULL){
			fprintf(stderr, "Unable to init ffmpeg.\n");
			return false;
		}

		avconv_audio = popen("ffmpeg -y -f u8 -ar 15734 -ac 1 -i - -acodec libmp3lame -ar 44.1k uzemtemp.mp3", "w");
		if(avconv_audio == NULL){
			fprintf(stderr, "Unable to init ffmpeg.\n");
			return false;
		}
	}

	//Set window icon
	SDL_Surface *slogo;
	slogo = SDL_CreateRGBSurfaceFrom((void *)&logo,32,32,32,32*4,0xFF,0xff00,0xff0000,0xff000000);
	SDL_SetWindowIcon(window,slogo);
	SDL_FreeSurface(slogo);

	return true;
}


void avr8::uzekb_handle_key(SDL_Event &ev)
{
	if(ev.type==SDL_KEYUP)uzeKbScanCodeQueue.push(0xf0);

	u16 i;
	for(i = 0; uzeKbScancodes[i][1]!=ev.key.keysym.sym && uzeKbScancodes[i][1]; i++);
	if (uzeKbScancodes[i][1] == ev.key.keysym.sym)
	{
		uzeKbScanCodeQueue.push(uzeKbScancodes[i][0]);
	}
}

void avr8::handle_key_down(SDL_Event &ev)
{
	static int ssnum = 0;
	char ssbuf[32];
	static const char *pad_mode_strings[4] = {"NES pad.","SNES pad.","SNES 2p pad.","SNES mouse."};

	if(uzeKbEnabled)
	{
		uzekb_handle_key(ev);
		return;
	}

	if (jmap.jstate == JMAP_IDLE)
		update_buttons(ev.key.keysym.sym,true);

	switch (ev.key.keysym.sym)
	{
			// SDLK_LEFT/RIGHT/UP/DOWN
			// SDLK_abcd...
			// SDLK_RETURN...
			case SDLK_1: if (left_edge > 0U) { left_edge--; } printf("left=%u\n",left_edge); break;
			case SDLK_2: if (left_edge < 2047U - ((VIDEO_DISP_WIDTH * 7U) / 3U)) { left_edge++; } printf("left=%u\n",left_edge); break;
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
			case SDLK_7:
				if (jmap.jstate == JMAP_IDLE)
					set_jmap_state(JMAP_INIT);
				break;
			case SDLK_n:
				if (jmap.jstate == JMAP_MORE_AXES)
					set_jmap_state(JMAP_DONE);
				break;
			case SDLK_y:
				if (jmap.jstate == JMAP_MORE_AXES)
					set_jmap_state(JMAP_AXES);
				break;
			case SDLK_ESCAPE:
				printf("user abort (pressed ESC).\n");
                shutdown(0);
                /* no break */
			case SDLK_PRINTSCREEN:
				sprintf(ssbuf,"uzem_%03d.bmp",ssnum++);
				printf("saving screenshot to '%s'...\n",ssbuf);
                                {
                                  SDL_Surface* surfBMP;
                                  const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
                                  if (kbstate[SDL_SCANCODE_LSHIFT] || kbstate[SDL_SCANCODE_RSHIFT]) {
                                    surfBMP = SDL_CreateRGBSurface(0, 240, 224, 32, 0, 0, 0, 0);
                                  } else {
                                    surfBMP = SDL_CreateRGBSurface(0, 630, 448, 32, 0, 0, 0, 0);
                                  }
                                  
                                  if (!surfBMP){
                                    fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
                                  } else {
                                    if (SDL_BlitScaled(surface, NULL, surfBMP, NULL) < 0) {
                                      fprintf(stderr, "BlitScaled failed: %s\n", SDL_GetError());
                                      SDL_FreeSurface(surfBMP);
                                    } else {
                                      SDL_SaveBMP(surfBMP,ssbuf);
                                      SDL_FreeSurface(surfBMP);
                                      break;
                                    }
                                  }
                                }
                                fprintf(stderr, "There was a problem rescaling the screenshot, saving the unscaled version.\n");
                                SDL_SaveBMP(surface,ssbuf); // at least save the weirdly scaled one				
				break;
			case SDLK_0:
				PIND = PIND & ~0b00001100;
				break;
			case SDLK_F1:
				puts("1/2 - Adjust left edge lock");
				puts("3/4 - Adjust top edge lock");
				puts(" 5  - Toggle NES/SNES 1p/SNES 2p/SNES mouse mode (default is SNES pad)");
				puts(" 6  - Mouse sensitivity scale factor");
				puts(" 7  - Re-map joystick");
				puts(" F1 - This help text");
				puts("Esc - Quit emulator");
				puts(" 0  - Soft Power switch");
				puts("");
				puts("            Up Down Left Right A B X Y Start Sel LShldr RShldr");
				puts("NES:        ----arrow keys---- a s     Enter Tab              ");
				puts("SNES 1p:    ----arrow keys---- a s x z Enter Tab LShift RShift");
				puts("  2p P1:     w   s    a    d   f g r t   z    x     q      e  ");
				puts("  2p P2:     i   k    j    l   ; ' p [   n    m     u      o  ");
				break;
			default:
				// Makes the compiler happy
				break;
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
	if(uzeKbEnabled){
		uzekb_handle_key(ev);
		return;
	}

	update_buttons(ev.key.keysym.sym,false);
	if (ev.key.keysym.sym == SDLK_0) PIND |= 0b00001100;		//return soft power switch to normal (pullup)
}

struct keymap { u32 key; u8 player, bit; };
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
   { SDLK_f, 0, SNES_B }, { SDLK_g, 0, SNES_A }, { SDLK_z, 0, PAD_START }, { SDLK_x, 0, PAD_SELECT },
   // P2
   { SDLK_j, 1, PAD_LEFT }, { SDLK_k, 1, PAD_DOWN }, { SDLK_l, 1, PAD_RIGHT }, { SDLK_i, 1, PAD_UP },
   { SDLK_u, 1, SNES_LSH }, { SDLK_o, 1, SNES_RSH }, { SDLK_p, 1, SNES_Y }, { SDLK_LEFTBRACKET, 1, SNES_X },
   { SDLK_SEMICOLON, 1, SNES_B }, { SDLK_QUOTE, 1, SNES_A }, { SDLK_n, 1, PAD_START }, { SDLK_m, 1, PAD_SELECT },
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
}

// Joysticks
struct joyButton joy_btns_p1[] =
{
	{ JOY_SNES_START, PAD_START }, { JOY_SNES_SELECT, PAD_SELECT },
	{ JOY_SNES_A, SNES_A }, { JOY_SNES_B, SNES_B },
	{ JOY_SNES_X, SNES_X }, { JOY_SNES_Y, SNES_Y },
	{ JOY_SNES_LSH, SNES_LSH }, { JOY_SNES_RSH, SNES_RSH }
};

struct joyButton joy_btns_p2[] =
{
	{ JOY_SNES_START, PAD_START }, { JOY_SNES_SELECT, PAD_SELECT },
	{ JOY_SNES_A, SNES_A }, { JOY_SNES_B, SNES_B },
	{ JOY_SNES_X, SNES_X }, { JOY_SNES_Y, SNES_Y },
	{ JOY_SNES_LSH, SNES_LSH }, { JOY_SNES_RSH, SNES_RSH }
};

struct joyButton *joyButtons[] =  { joy_btns_p1, joy_btns_p2 };

void avr8::init_joysticks() {
	if (SDL_JoystickEventState(SDL_QUERY) != SDL_ENABLE && SDL_JoystickEventState(SDL_ENABLE) != SDL_ENABLE)
	{
		printf("No supported joysticks found.\n");
	}
	else
	{
		jmap.jstate = JMAP_IDLE;

		for (int i = 0; i < MAX_JOYSTICKS; ++i) {
			for (int j= 0; j < MAX_JOYSTICK_AXES; ++j)
				joysticks[i].axes[j].axis = JOY_AXIS_UNUSED;
		}
			
		load_joystick_file(joySettingsFilename);
		
		for (int i = 0; i < MAX_JOYSTICKS; ++i) {
			if ((joysticks[i].device = SDL_JoystickOpen(i)) == NULL)
				printf("P%i joystick not found.\n", i+1);
			else {
				joysticks[i].buttons = joyButtons[i];
				joysticks[i].hats = 0;
				printf("P%i joystick: %s.\n", i+1,SDL_JoystickName(joysticks[i].device));
			}
			
			for (int j= 0; j < MAX_JOYSTICK_AXES; ++j)
				joysticks[i].axes[j].bits = 0;
		}
	}
}

void avr8::set_jmap_state(int state)
{
	switch (state) {
		case JMAP_INIT:
			printf("Press START on the joystick you wish to re-map...");
			fflush(stdout);
			break;
		case JMAP_BUTTONS:
			jmap.jiter = 0;
			jmap.jaxis = JOY_AXIS_UNUSED;
			
			for (int i= 0; i < MAX_JOYSTICK_AXES; ++i)
				joysticks[jmap.jindex].axes[i].axis = JOY_AXIS_UNUSED;
			break;
		case JMAP_AXES:
			jmap.jiter = NUM_JOYSTICK_BUTTONS;
			jmap.jaxis = JOY_AXIS_UNUSED;
			printf("\nPress Left on axial input (hats are mapped automatically and won't register)...");
			fflush(stdout);
			break;
		case JMAP_MORE_AXES:
			jmap.jiter = NUM_JOYSTICK_BUTTONS+2;
			printf("\nMap an axial input (y/n)? ");
			fflush(stdout);
			break;
		case JMAP_DONE:
			jmap.jstate = JMAP_IDLE;
			joystickFile = joySettingsFilename;	// Save on exit
			printf("\nJoystick mappings complete.\n");
			fflush(stdout);
			state = JMAP_IDLE;
			break;
		default:
			break;
	}
	jmap.jstate = state;
}

void avr8::map_joysticks(SDL_Event &ev)
{	
	if (jmap.jstate == JMAP_MORE_AXES) {
		return;
	} else if (jmap.jstate == JMAP_INIT) { // Initialize mapping settings
		jmap.jindex = ev.jbutton.which;
		set_jmap_state(JMAP_BUTTONS);
	} else if (ev.jbutton.which != jmap.jindex) {
		return; // Ignore input from all joysticks but the one being mapped
	}

	if (jmap.jstate == JMAP_BUTTONS) {
		if (ev.jbutton.type == SDL_JOYBUTTONDOWN)
				joyButtons[ev.jbutton.which][jmap.jiter].button = ev.jbutton.button;
		else
			return;
	} else if (jmap.jstate == JMAP_AXES && ev.type == SDL_JOYAXISMOTION) {
		// Find index to place new axes
		if (jmap.jaxis == JOY_AXIS_UNUSED) {
			for (int i = 0; i < MAX_JOYSTICK_AXES; i+=2) {
				if (joysticks[jmap.jindex].axes[i].axis == JOY_AXIS_UNUSED || joysticks[jmap.jindex].axes[i].axis == ev.jaxis.axis) {
					joysticks[jmap.jindex].axes[i].axis = JOY_AXIS_UNUSED;
					joysticks[jmap.jindex].axes[i+1].axis = JOY_AXIS_UNUSED;
					jmap.jaxis = i;
					break;
				}
			}
		}
		
		if (joysticks[jmap.jindex].axes[jmap.jaxis].axis == JOY_AXIS_UNUSED) {
			if (ev.jaxis.value < -(2*JOY_ANALOG_DEADZONE))
				joysticks[jmap.jindex].axes[jmap.jaxis].axis = ev.jaxis.axis;
			else
				return;
		} else if (joysticks[jmap.jindex].axes[jmap.jaxis].axis != ev.jaxis.axis && ev.jaxis.value < -(2*JOY_ANALOG_DEADZONE)) {
			joysticks[jmap.jindex].axes[jmap.jaxis+1].axis = ev.jaxis.axis;
		} else {
			return;
		}
	} else {
		return;
	}
	
	if (++jmap.jiter == NUM_JOYSTICK_BUTTONS) {
		if (SDL_JoystickNumAxes(joysticks[jmap.jindex].device) == 0) {
			set_jmap_state(JMAP_DONE);
			return;
		} else {
			set_jmap_state(JMAP_MORE_AXES);
		}
	} else if (jmap.jiter == (NUM_JOYSTICK_BUTTONS+2)) {
		set_jmap_state(JMAP_MORE_AXES);
	}
	
	switch (jmap.jiter) {
		case 1: printf("\nPress SELECT..."); break;
		case 2: printf("\nPress A..."); break;
		case 3: printf("\nPress B..."); break;
		case 4: printf("\nPress X..."); break;
		case 5: printf("\nPress Y..."); break;
		case 6: printf("\nPress LShldr..."); break;
		case 7: printf("\nPress RShldr..."); break;
		case 9: printf("\nPress Up on axial input..."); break;
		default: break;
	}
	fflush(stdout);
}

void avr8::update_joysticks(SDL_Event &ev)
{
	u8 axisBits = 0;
	
	if (ev.type == SDL_JOYAXISMOTION) {
		for (int i = 0; i < MAX_JOYSTICK_AXES; ++i) {
			if (joysticks[ev.jaxis.which].axes[i].axis == -1)
				break;
			if (joysticks[ev.jaxis.which].axes[i].axis != ev.jaxis.axis) {
				axisBits |= joysticks[ev.jaxis.which].axes[i].bits;
				continue;
			}
			if (i&1) {
				if (ev.jaxis.value < -JOY_ANALOG_DEADZONE) { // UP
					joysticks[ev.jaxis.which].axes[i].bits |= JOY_DIR_UP;
					joysticks[ev.jaxis.which].axes[i].bits &= ~JOY_DIR_DOWN;
				} else if (ev.jaxis.value > JOY_ANALOG_DEADZONE) { // DOWN
					joysticks[ev.jaxis.which].axes[i].bits |= JOY_DIR_DOWN;
					joysticks[ev.jaxis.which].axes[i].bits &= ~JOY_DIR_UP;
				} else {
					joysticks[ev.jaxis.which].axes[i].bits &= ~(JOY_DIR_UP|JOY_DIR_DOWN);
				}
			} else {
				if (ev.jaxis.value < -JOY_ANALOG_DEADZONE) { // LEFT
					joysticks[ev.jaxis.which].axes[i].bits |= JOY_DIR_LEFT;
					joysticks[ev.jaxis.which].axes[i].bits &= ~JOY_DIR_RIGHT;
				} else if (ev.jaxis.value > JOY_ANALOG_DEADZONE) { // RIGHT
					joysticks[ev.jaxis.which].axes[i].bits |= JOY_DIR_RIGHT;
					joysticks[ev.jaxis.which].axes[i].bits &= ~JOY_DIR_LEFT;
				} else {
					joysticks[ev.jaxis.which].axes[i].bits &= ~(JOY_DIR_LEFT|JOY_DIR_RIGHT);
				}
			}
			axisBits |= joysticks[ev.jaxis.which].axes[i].bits;
		}
	} else if (ev.type == SDL_JOYHATMOTION) {
		joysticks[ev.jhat.which].hats &= ~(0xf<<ev.jhat.hat);
		joysticks[ev.jhat.which].hats |= (ev.jhat.value<<ev.jhat.hat);
	} else if (ev.type == SDL_JOYBUTTONDOWN || ev.type == SDL_JOYBUTTONUP) {
		struct joyButton *j = joysticks[ev.jbutton.which].buttons;
	
		for (int i = 0; i < NUM_JOYSTICK_BUTTONS; ++i, ++j) {
			if (ev.jbutton.button == j->button) {
				if (ev.jbutton.type == SDL_JOYBUTTONUP)
					buttons[ev.jaxis.which] |= (1<<j->bit);
				else
					buttons[ev.jaxis.which] &= ~(1<<j->bit);
				break;
			}
		}
	}
		
	if (ev.type == SDL_JOYAXISMOTION || ev.type == SDL_JOYHATMOTION) {
		for (u32 i = JOY_MASK_UP, bit = PAD_UP; bit; i<<=1) {
			if ((axisBits&i) || (joysticks[ev.jaxis.which].hats&i))
				buttons[ev.jaxis.which] &= ~(1<<bit);
			else
				buttons[ev.jaxis.which] |= (1<<bit);
			if (bit == PAD_UP)
				bit = PAD_RIGHT;
			else if (bit == PAD_RIGHT)
				bit = PAD_DOWN;
			else if (bit == PAD_DOWN)
				bit = PAD_LEFT;
			else
				bit = 0;
		}
	}
}

void avr8::load_joystick_file(const char* filename)
{
	bool validFile = true;
	FILE* f = fopen(filename,"rb");
	
	if (f) {
		size_t btnsSize = MAX_JOYSTICKS*NUM_JOYSTICK_BUTTONS*sizeof(struct joyButton);
		size_t axesSize = MAX_JOYSTICKS*MAX_JOYSTICK_AXES*sizeof(struct joyAxis);
		fseek(f,0,SEEK_END);
		
		size_t size = ftell(f);
		
		if (size < (btnsSize+axesSize)) {
			validFile = false;
		} else {
			fseek(f,0,SEEK_SET);
			size_t result;
			
			for (int i = 0; i < MAX_JOYSTICKS; ++i) {
				if (!(result = fread(joyButtons[i],NUM_JOYSTICK_BUTTONS*sizeof(struct joyButton),1,f)) || 
						!(result = fread(joysticks[i].axes,MAX_JOYSTICK_AXES*sizeof(struct joyAxis),1,f)))
					validFile = false;
			}
		}
		fclose(f);

		if (!validFile)
			printf("Warning: Invalid Joystick settings file.\n");
	}
}



#ifdef SPI_DEBUG
char ascii(unsigned char ch){
    if(ch >= 32 && ch <= 127){
        return ch;
    }
    return '.';
}

#endif

void avr8::update_spi(){
    // SPI state machine
    switch(spiState){
    case SPI_IDLE_STATE:
        if(spiByte == 0xff){
        	//SPI_DEBUG("Idle->0xff\n");
            SPDR = 0xff; // echo back that we're ready
            break;
        }
        spiCommand = spiByte;
        SPDR = 0x00;
        spiState = SPI_ARG_X_HI;
        break;
    case SPI_ARG_X_HI:
        SPI_DEBUG("x hi: %02X\n",spiByte);
        spiArgXhi = spiByte;
        SPDR = 0x00;
        spiState = SPI_ARG_X_LO;
        break;
    case SPI_ARG_X_LO:
        SPI_DEBUG("x lo: %02X\n",spiByte);
        spiArgXlo = spiByte;
        SPDR = 0x00;
        spiState = SPI_ARG_Y_HI;
        break;
    case SPI_ARG_Y_HI:
        SPI_DEBUG("y hi: %02X\n",spiByte);
        spiArgYhi = spiByte;
        SPDR = 0x00;
        spiState = SPI_ARG_Y_LO;
        break;
    case SPI_ARG_Y_LO:
        SPI_DEBUG("y lo: %02X\n",spiByte);
        spiArgYlo = spiByte;
        SPDR = 0x00;
        spiState = SPI_ARG_CRC;
        break;
    case SPI_ARG_CRC:
        SPI_DEBUG("SPI - CMD%d (%02X) X:%04X Y:%04X CRC: %02X\n",spiCommand^0x40,spiCommand,spiArgX,spiArgY,spiByte);
        // ignore CRC and process commands
        switch(spiCommand){
        case 0x40: //CMD0 =  RESET / GO_IDLE_STATE
            SPDR = 0x00;
            spiState = SPI_RESPOND_SINGLE;
            spiResponseBuffer[0] = 0xff; // 8 clock wait
            spiResponseBuffer[1] = 0x01; // send command response R1->idle flag
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+2;
            spiByteCount = 0;
            break;
        case 0x41: //CMD1 =  INIT / SEND_OP_COND
            SPDR = 0x00;
            spiState = SPI_RESPOND_SINGLE;
            spiResponseBuffer[0] = 0x00; // 8-clock wait
            spiResponseBuffer[1] = 0x00; // no error
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+2;
            spiByteCount = 0;
            break;

        case 0x48: //CMD8 =  INIT / SEND_IF_COND
            SPDR = 0x00;
            spiState = SPI_RESPOND_SINGLE;
            spiResponseBuffer[0] = 0xff; // 8-clock wait
            spiResponseBuffer[1] = 0x01; // return command response R7
            spiResponseBuffer[2] = 0x00; // return command response R7
            spiResponseBuffer[3] = 0x00; // return command response R7
            spiResponseBuffer[4] = 0x01; // return command response R7 voltage accepted
            spiResponseBuffer[5] = spiArgYlo; // return command response R7 check pattern
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+6;
            spiByteCount = 0;
            break;

        case 0x4c: //CMD12 =  STOP_TRANSMISSION
            SPDR = 0x00;
            spiState = SPI_RESPOND_SINGLE;
            spiResponseBuffer[0] = 0xff; //stuff byte
            spiResponseBuffer[1] = 0xff; //stuff byte
            spiResponseBuffer[2] = 0x00; // card is ready //in "trans" state
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+3;
            spiByteCount = 0;
            break;


        case 0x51: //CMD17 =  READ_BLOCK
            SPDR = 0x00;
            spiState = SPI_RESPOND_SINGLE;
            spiResponseBuffer[0] = 0xFF; // 8-clock wait
            spiResponseBuffer[1] = 0x00; // no error
            spiResponseBuffer[2] = 0xFE; // start block
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+3;
            SDSeekToOffset(spiArg);
            spiByteCount = 512;
            break;
        case 0x52: //CMD18 =  MULTI_READ_BLOCK
            SPDR = 0x00;
            spiState = SPI_RESPOND_MULTI;
            spiResponseBuffer[0] = 0xFF; // 8-clock wait
            spiResponseBuffer[1] = 0x00; // no error
            spiResponseBuffer[2] = 0xFE; // start block
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+3;
            spiCommandDelay=0;
            SDSeekToOffset(spiArg);
            spiByteCount = 0;
            break;   
        case 0x58: //CMD24 =  WRITE_BLOCK
            SPDR = 0x00;
            spiState = SPI_WRITE_SINGLE;
            spiResponseBuffer[0] = 0x00; // 8-clock wait
            spiResponseBuffer[1] = 0x00; // no error
            spiResponseBuffer[2] = 0xFE; // start block
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+3;
            SDSeekToOffset(spiArg);
            spiByteCount = 512;
            break;

        case 0x69: //ACMD41 =  SD_SEND_OP_COND  (ACMD<n> is the command sequence of CMD55-CMD<n>)
            SPDR = 0x00;
            spiState = SPI_RESPOND_SINGLE;
            spiResponseBuffer[0] = 0xff; // 8 clock wait
           	spiResponseBuffer[1] = 0x00; // send command response R1->OK
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+2;
            spiByteCount = 0;
            break;

        case 0x77: //CMD55 =  APP_CMD  (ACMD<n> is the command sequence of CMD55-CMD<n>)
            SPDR = 0x00;
            spiState = SPI_RESPOND_SINGLE;
            spiResponseBuffer[0] = 0xff; // 8 clock wait
            spiResponseBuffer[1] = 0x01; // send command response R1->idle flag
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+2;
            spiByteCount = 0;
            break;


        case 0x7A: //CMD58 =  READ_OCR
            SPDR = 0x00;
            spiState = SPI_RESPOND_SINGLE;
            spiResponseBuffer[0] = 0xff; // 8 clock wait
            spiResponseBuffer[1] = 0x00; // send command response R1->ok
            spiResponseBuffer[2] = 0x80; // return command response R3
            spiResponseBuffer[3] = 0xff; // return command response R3
            spiResponseBuffer[4] = 0x80; // return command response R3
            spiResponseBuffer[5] = 0x00; // return command response R3
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+6;
            spiByteCount = 0;
            break;

        default:
			printf("Unknown SPI command: %d\n", spiCommand);
            SPDR = 0x00;
            spiState = SPI_RESPOND_SINGLE;
            spiResponseBuffer[0] = 0x02; // data accepted
            spiResponseBuffer[1] = 0x05;  //i illegal command
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+2;
            break;
        }
        break;

   case SPI_RESPOND_SINGLE:
        SPDR = *spiResponsePtr;
        SPI_DEBUG("SPI - Respond: %02X\n",SPDR);
        spiResponsePtr++;
        if(spiResponsePtr == spiResponseEnd){
            if(spiByteCount != 0){
                spiState = SPI_READ_SINGLE_BLOCK;
            }
            else{
                spiState = SPI_IDLE_STATE;
            }
        }
        break;

    case SPI_RESPOND_MULTI:
    	if(spiCommandDelay!=0){
    		spiCommandDelay--;
    		SPDR=0xff;
    		break;
    	}

        SPDR = *spiResponsePtr;
        SPI_DEBUG("SPI - Respond: %02X\n",SPDR);
        spiResponsePtr++;

        if(SPDR==0 && spiByteCount==0){
        	spiCommandDelay=250; //average delay based on a sample of cards
        }

        if(spiResponsePtr == spiResponseEnd){
            spiState = SPI_READ_MULTIPLE_BLOCK;
            spiByteCount = 512;
        }
        break;

    case SPI_READ_SINGLE_BLOCK:
        SPDR = SDReadByte();
        #ifdef USE_SPI_DEBUG
	{
            // output a nice display to see sector data
            int i = 512-spiByteCount;
            int ofs = i&0x000F;
            static unsigned char buf[16];
            if(i > 0 && (ofs == 0)){
                printf("%04X: ",i-16);
                for(int j=0; j<16; j++) printf("%02X ",buf[j]);
                printf("| ");
                for(int j=0; j<16; j++) printf("%c",ascii(buf[j]));
                SPI_DEBUG("\n");
            }
            buf[ofs] = SPDR;
	}
        #endif
        spiByteCount--;
        if(spiByteCount == 0){
            spiResponseBuffer[0] = 0x00; //CRC
            spiResponseBuffer[1] = 0x00; //CRC
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+2;
            spiState = SPI_RESPOND_SINGLE;
        }
        break;

    case SPI_READ_MULTIPLE_BLOCK:
        if(SPDR == 0x4C){ //CMD12 - stop multiple read transmission
            spiState = SPI_RESPOND_SINGLE;

        	spiCommand = 0x4C;
        	SPDR = 0x00;
        	spiState = SPI_ARG_X_HI;
            spiByteCount = 0;
            break;
        }
        else{
            SPDR = SDReadByte();
        }
        SPI_DEBUG("SPI - Data[%d]: %02X\n",512-spiByteCount,SPDR);
        spiByteCount--;
        //inter-sector
        //NOTE: Current MoviePlayer.hex does not work with two delay bytes after the CRC. It has
        //been coded to work with a MicroSD card. These cards usually have only 1 delay byte after the CRC.
        //Uzem uses two delay bytes after the CRC since it is what regular SD cards does
        //and we want to emulate the "worst" case.
        if(spiByteCount == 0){
            spiResponseBuffer[0] = 0x00; //CRC
            spiResponseBuffer[1] = 0x00; //CRC
            spiResponseBuffer[2] = 0xff; //delay
            spiResponseBuffer[3] = 0xff; //delay
            spiResponseBuffer[4] = 0xFE; // start block
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+5;
            spiArg+=512; // automatically move to next block
            SDSeekToOffset(spiArg);
            spiByteCount = 512;
            spiState = SPI_RESPOND_MULTI;
        }
        break;

    case SPI_WRITE_SINGLE:
        SPDR = *spiResponsePtr;
        SPI_DEBUG("SPI - Respond: %02X\n",SPDR);
        spiResponsePtr++;
        if(spiResponsePtr == spiResponseEnd){
            if(spiByteCount != 0){
                spiState = SPI_WRITE_SINGLE_BLOCK;
            }
            else{
                spiState = SPI_IDLE_STATE;
            }
        }
        break;    
    case SPI_WRITE_SINGLE_BLOCK:
        SDWriteByte(SPDR);
        SPI_DEBUG("SPI - Data[%d]: %02X\n",spiByteCount,SPDR);
        SPDR = 0xFF;
        spiByteCount--;
        if(spiByteCount == 0){
            spiResponseBuffer[0] = 0x00; //CRC
            spiResponseBuffer[1] = 0x00; //CRC
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+2;
            spiState = SPI_RESPOND_SINGLE;
            //SDCommit();
        }
        break;    
    }    
}


void avr8::SDLoadImage(char* filename){
    if(sdImage){
        printf("SD Image file already specified.");
        shutdown(1);
    }
    sdImage = fopen(filename,"rb");
    if(!sdImage){
        printf("Cannot find SD image %s\n",filename);
        shutdown(1);
    }
}


void avr8::SDBuildMBR(SDPartitionEntry* entry){
    // create the needed buffer for the entire MBR
    emulatedMBRLength = entry->sectorOffset * sectorSize;
    emulatedMBR = (u8*)malloc(emulatedMBRLength);
    memset(emulatedMBR,0,emulatedMBRLength);
    
    // build a replica of the MBR for a single-partition image (common for SD media)
    memcpy(emulatedMBR + 0x1BE,entry,sizeof(SDPartitionEntry));                

    // Executable Marker
    emulatedMBR[0x1FE] = 0x55;
    emulatedMBR[0x1FF] = 0xAA;
}
    
u8 avr8::SDReadByte(){
    u8 result;
    
    if(emulatedMBR && emulatedReadPos != 0xFFFFFFFF){
        //printf("reading with MBR emulation %d.\n", emulatedReadPos);
        result = emulatedMBR[emulatedReadPos];
        emulatedReadPos++;
    }
    else{
        //printf("Performing normal read.\n");
        SDemulator.read(&result);
    }
    return result;
}

void avr8::SDWriteByte(u8 value){    
    (void)value;
    fprintf(stderr, "No write support in SD emulation\n");
}

void avr8::SDSeekToOffset(u32 pos){
    if(emulatedMBR){
        //printf("seeking with MBR emulation.\n");
        if(pos < emulatedMBRLength){
            // seek to somewhere within the MBR
            emulatedReadPos = pos;
        }
        else{
            SDemulator.seek(pos);
            emulatedReadPos = 0xFFFFFFFF;
        }
    }
    else{
        SDemulator.seek(pos);
    }
}

void avr8::LoadEEPROMFile(const char* filename){
    eepromFile = filename;
    memset(eeprom,0xff,eepromSize);
    FILE* f = fopen(filename,"rb");
    if(f){

        fseek(f,0,SEEK_END);
        size_t size = ftell(f);
        if(size < eepromSize) printf("Warning: EEPROM file is smaller than 2k.\n");
        if(size > eepromSize){
            printf("Warning: EEPROM file is larger than 2k.\n");
            size = eepromSize;
        }
        fseek(f, 0, SEEK_SET);

		size_t result=fread(eeprom,1,size,f);
        if (result != size){
        	printf("Warning: fread in %s returned an unexpected value:%lu,\n", __FUNCTION__,result);
        }
        fclose(f);
    }
    else{
        printf("EEPROM file not found, continuing with emulation.\n");
    }
}

void avr8::shutdown(int errcode){
#if defined(__WIN32__)
    if(hDisk != INVALID_HANDLE_VALUE){
        CloseHandle (hDisk);        
        VirtualFree (lpSector, 0, MEM_RELEASE);
    }        
#endif
    if(sdImage){
        fclose(sdImage);
    }
    if(emulatedMBR){
        free(emulatedMBR);
    }
    if(eepromFile){
        FILE* f = fopen(eepromFile,"wb+");
        if(f){
            fwrite(eeprom,eepromSize,1,f);
            fclose(f);
        }
    }

    if(captureFile!=NULL){
    	fclose(captureFile);
    }

    //movie recording
    if(recordMovie){
		if (avconv_video) pclose(avconv_video);
		if (avconv_audio) pclose(avconv_audio);

		char mux[1024];
		strcpy(mux,"ffmpeg -y -i uzemtemp.mp4 -i uzemtemp.mp3 -vcodec copy -acodec copy -f mp4 ");
		strcat(mux,romName);
		strcat(mux,".mp4");

		FILE* avconv_mux = popen(mux,"r");
		if (avconv_mux) {
			pclose(avconv_mux);
			unlink("uzemtemp.mp4");
			unlink("uzemtemp.mp3");
		}else{
			printf("Error with ffmpeg multiplexer.");
		}
    }

	if (joystickFile) {
		FILE* f = fopen(joystickFile,"wb");

        if(f) {
			for (int i = 0; i < MAX_JOYSTICKS; ++i) {
				fwrite(joyButtons[i],sizeof(struct joyButton),NUM_JOYSTICK_BUTTONS,f);
				fwrite(joysticks[i].axes,sizeof(struct joyAxis),MAX_JOYSTICK_AXES,f);
				
				if (joysticks[i].device)
					SDL_JoystickClose(joysticks[i].device);
			}
            fclose(f);
        }
	}

    exit(errcode);
}

/* This function is called from GDB while the cpu is stopped */
void avr8::idle(void){
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
	if ((event.type == SDL_QUIT) || (event.key.keysym.sym == SDLK_ESCAPE)) {
            printf("User abort.\n");
            shutdown(0);
        }
    }

    SDL_Delay(5);
}

