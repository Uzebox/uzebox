/*
(The MIT License)

Copyright (c) 2008-2013 David Etherton, Eric Anderton, Alec Bourque et al.

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

#include "avr8.h"
#include "gdbserver.h"
#include "SDEmulator.h"
#include "logo.h"

#define FPS 30

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


static const char *port_name(int);

#if GUI
static const char* joySettingsFilename = "joystick-settings";
#endif

#define SD_ENABLED() SDpath

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

#define ILL if (!disasmOnly) { fprintf(stderr,"invalid insn %x\n",insn); shutdown(1); }

#if defined(_DEBUG)
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

#if GUI
int avr8::guithread(void *ptr) {
	static unsigned int lastFrame;
	static unsigned int previousTime;
	unsigned int timeDelta;
	avr8 *self = (avr8 *)ptr;
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = self->screen->w;
	rect.h = self->screen->h;

	while (!self->exitThreads) {
		int blitFrameBuffer;

		SDL_LockMutex(self->mtxVSync);

		/* Wait for "VSync" */
		SDL_CondWait(self->cVSync, self->mtxVSync);

		SDL_UnlockMutex(self->mtxVSync);

		blitFrameBuffer = self->currentFrameBuffer ^ 1;

		if (self->exitThreads)
			return 0;

		/* Tell CPU it's safe to continue drawing */
		//cVSDone.notify_one();

		/* Unlock surface for blitting */
		if (SDL_MUSTLOCK(self->frameBuffer[blitFrameBuffer]))
			SDL_UnlockSurface(self->frameBuffer[blitFrameBuffer]);

		/* Actually draw the buffer to the screen */
		SDL_BlitSurface( self->frameBuffer[blitFrameBuffer], &rect, self->screen, &rect );

		/* Lock surface to gain pixel access */
		if (SDL_MUSTLOCK(self->frameBuffer[blitFrameBuffer]))
			SDL_LockSurface(self->frameBuffer[blitFrameBuffer]);

		/* Flip screen buffer */
		SDL_Flip(self->screen);

		/* Sync to FPS fps */
		timeDelta = SDL_GetTicks() - previousTime;
		if (timeDelta < 1000 / FPS) {
			SDL_Delay((1000 / FPS) - (timeDelta-1));
		}
		previousTime = SDL_GetTicks();

	}
	return 0;
}
#endif

void avr8::write_io(u8 addr,u8 value)
{
	if (addr == ports::PORTC)
	{
	     pixel = palette[value & DDRC];
	}
	// p106 in 644 manual; 16-bit values are latched
	else if (addr == ports::TCNT1H || addr == ports::ICR1H)
		TEMP = value;
	else if (addr == ports::TCNT1L || addr == ports::ICR1L)
	{
		io[addr] = value;
		io[addr+1] = TEMP;
	}
	else if (addr == ports::PORTD)
	{        
        // write value with respect to DDRD register
        io[addr] = value & DDRD;

        //detect if are enabling the SD CE
     //   if(!SD_ENABLED() && ((value & DDRD)&0x40)){
       // 	SDpath

        //}
    }
#if GUI    
	else if (addr == ports::PORTB)
	{
        u32 elapsed = cycleCounter - prevPortB;
        prevPortB = cycleCounter;

        //if (scanline_count == -999 && value && elapsed >= 774 - 4 && elapsed <= 774 + 4)
          if (scanline_count == -999 && value && elapsed >= 774 -7 && elapsed <= 774 + 7)
            scanline_count = scanline_top;
        else if (scanline_count != -999 && value) {
            ++scanline_count;
            current_cycle = left_edge;

            current_scanline = (u32*)((u8*)frameBuffer[currentFrameBuffer]->pixels + scanline_count * 2 * screen->pitch + inset);
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

                // framelock no longer necessary, audio buffer full takes care of it for us.
                if (frameCounter & 1) {
                    currentFrame++;

                    if (visualize)
                      draw_memorymap();

                    /* Tell GUI thread to update screen */
                    SDL_LockMutex(mtxVSync);
										currentFrameBuffer ^= 1;
                    SDL_CondSignal(cVSync);
                    SDL_UnlockMutex(mtxVSync);
                }

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
				//printf("New input routines detected, switching emulation method.\n");
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
	//else if (addr == ports::PORTC)
	//{
    //    pixel = palette[value & DDRC];
	//}
#endif	// GUI

    else if(addr == ports::SPDR)
    {
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
    }
    else if(addr == ports::SPCR)
    {
        SPI_DEBUG("SPCR: %02X\n",value);
        io[addr] = value;
        if(SD_ENABLED()) spi_calculateClock();
    }
    else if(addr == ports::SPSR){
        SPI_DEBUG("SPSR: %02X\n",value);
        io[addr] = value;
        if(SD_ENABLED()) spi_calculateClock();
    }

    else if(addr == ports::EECR){
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
            eeClock = 4;
        }
        else{
            io[addr] = value;
        }
    }
    else if(addr == ports::EEARH || addr == ports::EEARL || addr == ports::EEDR){
        //printf("writing to port %s (%x) pc = %x\n",port_name(addr),value,pc-1);
		io[addr] = value;
    }
    else if(addr == ports::TIFR1){
		//clear flags by writing logical one
		io[addr] &= ~(value);
    }

	#ifdef USE_PORT_PRINT
    else if(addr == ports::res3A){
        // emulator-only whisper support
        printf("%c",value);
    }    
    else if(addr == ports::res39){
        // emulator-only whisper support
        printf("%02x",value);
    }
	#endif

	else
		io[addr] = value;
}


u8 avr8::read_io(u8 addr)
{
	// p106 in 644 manual; 16-bit values are latched
	if (addr == ports::TCNT1L || addr == ports::ICR1L)
	{
		TEMP = io[addr+1];
		return io[addr];
	}
	else if (addr == ports::TCNT1H || addr == ports::ICR1H){
		return TEMP;
    }
    //else if(addr == ports::EECR || addr == ports::EEARH || addr == ports::EEARL || addr == ports::EEDR){
       // printf("reading port %s (%x) pc = %x\n",port_name(addr),io[addr],pc-1);
	//	return io[addr];
    //}
	else
		return io[addr];
}

/*
	Someone awesome should optimize this
*/
inline void avr8::draw_memorymap()
{
	int sramaddr = 0;
	int startx = inset + 630 + 5;
	int starty = 20;
	SDL_Surface *surface = frameBuffer[currentFrameBuffer];

	/* SRAM */
	for (int y = 0; y < 64; ++y) {
		for (int x = 0; x < 32; ++x) {
			u32 *pixel = (u32*)((u8*)surface->pixels + ((starty + y) * surface->pitch)) + startx + x;
			*pixel = (sramviz[sramaddr++] << 8);
			if ((sramviz[sramaddr-1] & 0xff) > 8) sramviz[sramaddr-1]-=8;
			if (((sramviz[sramaddr-1]>>8) & 0xff) > 8) sramviz[sramaddr-1] = (sramviz[sramaddr-1] & 0xff) | (((sramviz[sramaddr-1]>>8) - 8)<<8);
		}
	}
	int white = SDL_MapRGB(screen->format,255,255,255);
	for (int x = 0; x < 32; ++x) {
		u32 *pixel = (u32*)((u8*)surface->pixels + ((starty - 1) * surface->pitch)) + startx + x;
		*pixel = white;
		pixel = (u32*)((u8*)surface->pixels + ((starty + 64) * surface->pitch)) + startx + x;
		*pixel = white;
	}
	for (int y = 0; y < 64; ++y) {
		u32 *pixel = (u32*)((u8*)surface->pixels + ((starty + y) * surface->pitch)) + startx - 1;
		*pixel = white;
		pixel = (u32*)((u8*)surface->pixels + ((starty + y) * surface->pitch)) + startx + 32;
		*pixel = white;
	}

	/* Progmem */
	starty = 20 + 64 + 20;
	sramaddr = 0;
	startx = inset + 630 + 5;
	for (int y = 0; y < 200; ++y) {
		for (int x = 0; x < 160; ++x) {
			u32 *pixel = (u32*)((u8*)surface->pixels + ((starty + y) * surface->pitch)) + startx + x;
			*pixel = (progmemviz[sramaddr++] << 8);
			if ((progmemviz[sramaddr-1] & 0xff) > 8) progmemviz[sramaddr-1]-=8;
			if (((progmemviz[sramaddr-1]>>8) & 0xff) > 8) progmemviz[sramaddr-1] = (progmemviz[sramaddr-1] & 0xff) | (((progmemviz[sramaddr-1]>>8) - 8)<<8);
		}
	}
	for (int x = 0; x < 160; ++x) {
		u32 *pixel = (u32*)((u8*)surface->pixels + ((starty - 1) * surface->pitch)) + startx + x;
		*pixel = white;
		pixel = (u32*)((u8*)surface->pixels + ((starty + 200) * surface->pitch)) + startx + x;
		*pixel = white;
	}
	for (int y = 0; y < 200; ++y) {
		u32 *pixel = (u32*)((u8*)surface->pixels + ((starty + y) * surface->pitch)) + startx - 1;
		*pixel = white;
		pixel = (u32*)((u8*)surface->pixels + ((starty + y) * surface->pitch)) + startx + 160;
		*pixel = white;
	}
}

u8 avr8::exec(bool disasmOnly,bool verbose)
{

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

#if DISASM
	// Built-in breakpoints
	if (pc == breakpoint)
	{
		singleStep = nextSingleStep = true;
		return 0;
	}	
	// set a breakpoint on the printf to halt at a given address.
	// if (pc == 0x33d3)
	//	printf("hit\n");
#endif

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

	progmemviz[lastpc] |= VIZ_WRITE;

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
				break;
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
				break;
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
			    cycles = 2;
			    break;
			case 0x940C: // JMP; relies on fact that upper k bits are always zero!
			    DIS("JMP    $%04x",progmem[pc]);
			    pc = progmem[pc];
			    cycles = 3;
			    break;
			case 0x940E: // CALL; relies on fact that upper k bits are always zero!
			    DIS("CALL   $%04x",progmem[pc]);
			    write_sram(SP,(pc+1));
			    DEC_SP;
			    write_sram(SP,(pc+1)>>8);
			    DEC_SP;
			    pc = progmem[pc];
			    cycles = 4;
			    break;
			case 0x9419:
			    DIS("EIJMP");
			    pc = Z;
			    cycles = 2;
			    break;
			case 0x9508:
			    DIS("RET");
			    INC_SP;
			    pc = read_sram(SP) << 8;
			    INC_SP;
			    pc |= read_sram(SP);
			    cycles = 4;
			    break;
			case 0x9509:
			    DIS("ICALL");
			    write_sram(SP,u8(pc));
			    DEC_SP;
			    write_sram(SP,(pc)>>8);
			    DEC_SP;
			    pc = Z;
			    cycles = 3;
			    break;
			case 0x9518:
			    DIS("RETI");
			    INC_SP;
			    pc = read_sram(SP) << 8;
			    INC_SP;
			    pc |= read_sram(SP);
			    cycles = 4;
			    SREG |= (1<<SREG_I);
			    //--interruptLevel;
			    break;
			case 0x9519:
			    DIS("EICALL");
			    write_sram(SP,u8(pc));
			    DEC_SP;
			    write_sram(SP,(pc)>>8);
			    DEC_SP;
			    pc = Z;
			    cycles = 3;
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
			    //SDemulator.debug(true);
			    // Implement this if/when we need watchdog timer functionality.
			    if(prevWDR){
			       // printf("WDR measured %u cycles\n", cycleCounter - prevWDR);
			        prevWDR = 0;
			}else
			    prevWDR = cycleCounter + 1;

			break;
			case 0x95C8:
			    DIS("LPM    r0,Z");
			    r0 = read_progmem(Z);
			    cycles = 3;
			    break;
			case 0x95D8:
			    DIS("ELPM   r0,Z");
			    r0 = read_progmem(Z);
			    cycles = 3;
			    break;
			case 0x95E8:
			    DIS("SPM");
			    if (Z >= progSize/2)
				{
					fprintf(stderr,"illegal write to progmem addr %x\n",Z);
                    shutdown(1);
				}
				else
					progmem[Z] = r0 | (r1<<8);
			    cycles = 4; // undocumented?!?!?
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
					break;
				}
			    break;
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
			//if(inDebug==1){
			//	printf("Break!");
			//}
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

		//printf("$%04x [%04x]  %-23.23s",lastpc,progmem[lastpc],insnBuf);
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
			//return;
		}
	}
#endif
	update_hardware(cycles);

	return cycles;
}


void avr8::trigger_interrupt(int location)
{
#if DISASM
		printf("INTERRUPT %d triggered at cycleCounter = %u\n", (location/2), cycleCounter);
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

//		++interruptLevel;
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

#if GUI
bool avr8::init_gui()
{
	if ( SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 )
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return false;
	}
	atexit(SDL_Quit);

	init_joysticks();

	if (fullscreen)
		screen = SDL_SetVideoMode(800,600,32,sdl_flags | SDL_FULLSCREEN);
	else
		screen = SDL_SetVideoMode(visualize ? 800 : 630,448,32,sdl_flags);

	frameBuffer[0] = SDL_CreateRGBSurface( sdl_flags, screen->w, screen->h, 32, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask );
	frameBuffer[1] = SDL_CreateRGBSurface( sdl_flags, screen->w, screen->h, 32, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask );

	if (!screen || !frameBuffer[0] || !frameBuffer[1]) 
	{
		fprintf(stderr, "Unable to set 630x448x32 video mode.\n");
		return false;
	}
	else if (fullscreen)	// Center in fullscreen
		inset = ((600-448)/2) * screen->pitch + 4 * ((800-630)/2);

	if (fullscreen)
	{
		SDL_ShowCursor(0);
	}

	if (SDL_MUSTLOCK(frameBuffer[1]))
		SDL_UnlockSurface(frameBuffer[1]);

	if (SDL_MUSTLOCK(frameBuffer[0]))
		SDL_UnlockSurface(frameBuffer[0]);

	currentFrameBuffer = 0;

	SDL_Surface *slogo;
	slogo = SDL_CreateRGBSurfaceFrom((void *)&logo,32,32,32,32*4,0xFF,0xff00,0xff0000,0xff000000);
	SDL_WM_SetIcon(slogo, NULL);
	SDL_FreeSurface(slogo);

	/* Start SDL blitter thread */
	mtxVSync = SDL_CreateMutex();
	if (!mtxVSync) {
		fprintf(stderr, "Error creating mutex for SDL blitting\n");
		shutdown(1);
	}
	cVSync = SDL_CreateCond();
	if (!cVSync) {
		fprintf(stderr, "Error creating condition variable for SDL blitting\n");
		shutdown(1);
	}
	tgui = SDL_CreateThread(&avr8::guithread, this);
	if (!tgui) {
		fprintf(stderr, "Error spawning SDL blitting thread\n");
		shutdown(1);
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
	//Syncronized with the kernel, this value now results in the image 
	//being perfectly centered in both the emulator and a real TV
	left_edge = -166;

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
	if (jmap.jstate == JMAP_IDLE)
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
				puts(" 7  - Re-map joystick");
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
	// printf("buttons = %x\n",buttons);
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
				printf("P%i joystick: %s.\n", i+1,SDL_JoystickName(i));
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
			joystickFile = (char*)joySettingsFilename;	// Save on exit
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
			for (int i = 0, j = 0; i < MAX_JOYSTICK_AXES; i+=2) {
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
		//printf("Mapped to axis: %i\n",ev.jaxis.axis);
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
	/*
	switch (ev.type) {
		case SDL_JOYAXISMOTION:
			printf("Axis motion on P%i. Axis: %i Value: %i\n", ev.jaxis.which+1, ev.jaxis.axis, ev.jaxis.value);
			break;
		case SDL_JOYBALLMOTION:
			printf("Ball motion on P%i. Ball: %i Xrel: %i Yrel: %i\n", ev.jball.which+1, ev.jball.ball, ev.jball.xrel, ev.jball.yrel);
			break;
		case SDL_JOYHATMOTION:
			printf("Hat motion on P%i. Hat: %i Value: %i\n", ev.jhat.which+1, ev.jhat.hat, ev.jhat.value);
			break;
		case SDL_JOYBUTTONDOWN:
			printf("Button press on P%i. Button: %i\n", ev.jbutton.which+1, ev.jbutton.button);
			break;
		case SDL_JOYBUTTONUP:
			printf("Button release on P%i. Button: %i\n", ev.jbutton.which+1, ev.jbutton.button);
			break;
		default:
			printf("Unknown event. Type: %i\n", ev.user.type);
			break;
	}
	*/
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
#endif

void avr8::update_hardware(int cycles)
{
	cycleCounter += cycles;

	if (TCCR1B & 7)	//if timer 1 is started
	{
		u16 TCNT1 = TCNT1L | (TCNT1H<<8);
		u16 OCR1A = OCR1AL | (OCR1AH<<8);
		u16 OCR1B = OCR1BL | (OCR1BH<<8);

		if(TCCR1B & WGM12){ //timer in CTC mode: count up to OCRnA then resets to zero

			if (TCNT1 < OCR1B && (TCNT1 + cycles) >= OCR1B){
				if (TIMSK1 & OCIE1B){
					TIFR1|=OCF1B; //CTC match flag interrupt
				}
			}

			if (TCNT1 < OCR1A && TCNT1 + cycles >= OCR1A){
				TCNT1 -= (OCR1A - cycles);
				if (TIMSK1 & OCIE1A){
					TIFR1|=OCF1A; //CTC match flag interrupt
				}
			}else{
				TCNT1 += cycles;
			}

		}else{	//timer in normal mode: counts up to 0xffff then rolls over

			if (TCNT1 > (0xFFFF - cycles)){
				if (TIMSK1 & TOIE1){
					 TIFR1|=TOV1; //overflow interrupt
				}
			}
			TCNT1 += cycles;
		}

		TCNT1L = (u8) TCNT1;
		TCNT1H = (u8) (TCNT1>>8);
	}

/*
#if GUI && VIDEO_METHOD == 1
	if (scanline_count >= 0 && current_cycle < 1440)
	{
		while (cycles)
		{
			if (current_cycle >= 0 && current_cycle < 1440)
				current_scanline[(current_cycle*7)>>4] = 
				next_scanline[(int)(current_cycle*7)>>4] = pixel;
			++current_cycle;
			--cycles;
		}
	}
#endif
*/
    // clock the SPI hardware. 
    if((SPCR & 0x40) && SD_ENABLED()){ // only if SPI is enabled
        //TODO: test for master/slave modes (assume master for now)
        // TODO: factor in clock divider 
        
        if(spiTransfer && (cycles > 0)){
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
        if((SPCR & 0x80) && (SPSR & 0x80)){
            //TODO: verify that SPI is dependent upon the global interrupt flag
            if (BIT(SREG,SREG_I)){
                SPSR ^= 0x80; // clear the interrupt
                trigger_interrupt(SPI_STC); // execute the vector
            }
        }
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
        cycleCounter += 1; // ireads take one additonal cycle
        int addr = (EEARH << 8) | EEARL;
        if(addr < eepromSize) EEDR = eeprom[addr];
        EECR ^= EERE; // clear read  bit
        
        // interrupt?
        //if((EECR & EERIE) && BIT(SREG,SREG_I)){
        //    SPSR ^= 0x80; // clear the interrupt
        //    trigger_interrupt(SPI_STC); // execute the vector
        //}
    }

	//process interrupts in order of priority
    if(SREG & (1<<SREG_I)){

		if (TIFR1 & OCF1A){

			TIFR1&= ~OCF1A; //clear CTC match flag
			trigger_interrupt(TIMER1_COMPA);

		}else if (TIFR1 & OCF1B){

			TIFR1&= ~OCF1B; //clear CTC match flag
			trigger_interrupt(TIMER1_COMPB);

		}else if (TIFR1 & TOV1){

			TIFR1&= ~TOV1; //clear TOV1 flag
			trigger_interrupt(TIMER1_OVF);
		}
	}

#if GUI && VIDEO_METHOD == 1
	if (scanline_count >= 0 && current_cycle < 1440)
	{
		while (cycles)
		{
			if (current_cycle >= 0 && current_cycle < 1440)
				current_scanline[(current_cycle*7)>>4] =
				next_scanline[(int)(current_cycle*7)>>4] = pixel;
			++current_cycle;
			--cycles;
		}
	}
#endif

}

static u8 dummy_sector[512];

#ifdef SPI_DEBUG
char ascii(unsigned char ch){
    if(ch >= 32 && ch <= 127){
        return ch;
    }
    return '.';
}

#endif

/*
void avr8::update_spi(){

	 switch(spiState){
	    case SPI_IDLE_STATE:
	        if(spiByte == 0xff){
	            SPDR = 0x01; // echo back that we're ready
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
					 spiState = SPI_RESPOND_R1;
					 spiResponseBuffer[0] = 0xFF;	//Simulate 1 token delay
					 spiResponseBuffer[1] = 0x01;	//Return Idle state flag
					 spiByteCount = 2;
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
				 case 0x51: //CMD17 =  READ_BLOCK
					 SPDR = 0x00;
					 spiState = SPI_RESPOND_SINGLE;
					 spiResponseBuffer[0] = 0x00; // 8-clock wait
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
					 spiResponseBuffer[0] = 0x00; // 8-clock wait
					 spiResponseBuffer[1] = 0x00; // no error
					 spiResponseBuffer[2] = 0xFE; // start block
					 spiResponsePtr = spiResponseBuffer;
					 spiResponseEnd = spiResponsePtr+3;
					 SDSeekToOffset(spiArg);
					 spiByteCount = 512;
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

			 case SPI_RESPOND_R1:


				 break;

	 }
}
*/

void avr8::update_spi(){
    // SPI state machine
    //SPI_DEBUG("byte: %02x\n",spiByte);
    //SPI_DEBUG("state: %d\n",spiState);
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
            spiResponseBuffer[0] = 0x4; // card is in "trans" state
            spiResponseBuffer[1] = 0xff; //ready
            spiResponsePtr = spiResponseBuffer;
            spiResponseEnd = spiResponsePtr+2;
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
            //SPDR = 127;
            //memset(spiResponseBuffer,0xFF,9); // Q&D - return garbage in response to the whole command
            //spiResponsePtr = spiResponseBuffer;
            //spiResponseEnd = spiResponsePtr+9;

        //    spiResponseBuffer[0] = 127; //junk byte varies from card to card
//            spiResponseBuffer[0] = 0x0; //card is busy
  //          spiResponseBuffer[1] = 0xff; //ready
    //        spiResponsePtr = spiResponseBuffer;
      //      spiResponseEnd = spiResponsePtr+2;
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

void avr8::LoadEEPROMFile(char* filename){
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
        	printf("Warning: fread in %s returned an unexpected value:%i,\n", __FUNCTION__,result);
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
#if GUI
  /* Stop SDL thread */
  exitThreads = 1;

  /* wake up SDL thread, to let it finish */
  SDL_CondSignal(cVSync);
  /* Ensure that mutex is unlocked */
  //SDL_UnlockMutex(mtxVSync);

  /* Wait for threads to finish */
  SDL_WaitThread(tgui, NULL);

  if (mtxVSync)
    SDL_DestroyMutex(mtxVSync);
  if (cVSync)
    SDL_DestroyCond(cVSync);

	if (joystickFile) {
		FILE* f = fopen(joystickFile,"wb");

        if(f) {
			for (int i = 0; i < MAX_JOYSTICKS; ++i) {
				fwrite(joyButtons[i],sizeof(struct joyButton),NUM_JOYSTICK_BUTTONS,f);
				fwrite(joysticks[i].axes,sizeof(struct joyAxis),MAX_JOYSTICK_AXES,f);
				
				if (joysticks[i].device && SDL_JoystickOpened(SDL_JoystickIndex(joysticks[i].device)))
					SDL_JoystickClose(joysticks[i].device);
			}
            fclose(f);
        }
	}
#endif
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

