/*
NOTE THIS IS NOT THE OFFICIAL BRANCH OF THE UZEBOX EMULATOR
PLEASE SEE THE FORUM FOR MORE DETAILS:  http://uzebox.org/forums/

(The MIT License)

Copyright (c) 2008,2009, David Etherton, Eric Anderton

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
#ifndef AVR8_H
#define AVR8_H

#include <vector>
#include <stdint.h>

#include "gdbserver.h"

#if defined(__WIN32__)
    #include <windows.h> // Win32 memory mapped I/O
    #include <winioctl.h>
#elif defined(LINUX)
    #include <sys/mman.h> // Linux memory mapped I/O
#else
    #include <sys/mmap.h> // Unix memory mapped I/O
#endif

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

// Joysticks
#define MAX_JOYSTICKS 2
#define NUM_JOYSTICK_BUTTONS 8
#define MAX_JOYSTICK_AXES 8
#define MAX_JOYSTICK_HATS 8

#define JOY_SNES_X 0
#define JOY_SNES_A 1
#define JOY_SNES_B 2
#define JOY_SNES_Y 3
#define JOY_SNES_LSH 6
#define JOY_SNES_RSH 7
#define JOY_SNES_SELECT 8
#define JOY_SNES_START 9

#define JOY_DIR_UP 1
#define JOY_DIR_RIGHT 2
#define JOY_DIR_DOWN 4
#define JOY_DIR_LEFT 8
#define JOY_DIR_COUNT 4
#define JOY_AXIS_UNUSED -1

#define JOY_MASK_UP 0x11111111
#define JOY_MASK_RIGHT 0x22222222
#define JOY_MASK_DOWN 0x44444444
#define JOY_MASK_LEFT 0x88888888

#ifndef JOY_ANALOG_DEADZONE
	#define JOY_ANALOG_DEADZONE 4096
#endif

#endif

#if defined (_MSC_VER) && _MSC_VER >= 1400
// don't whine about sprintf and fopen.
// could switch to sprintf_s but that's not standard.
#pragma warning(disable:4996)
#endif

// 644 Overview: http://www.atmel.com/dyn/resources/prod_documents/doc2593.pdf
// AVR8 insn set: http://www.atmel.com/dyn/resources/prod_documents/doc0856.pdf

enum { NES_A, NES_B, PAD_SELECT, PAD_START, PAD_UP, PAD_DOWN, PAD_LEFT, PAD_RIGHT };
enum { SNES_B, SNES_Y, SNES_A=8, SNES_X, SNES_LSH, SNES_RSH };

#if 1	// 644P
const unsigned eepromSize = 2048;
const unsigned sramSize = 4096;
const unsigned progSize = 65536;
#else	// 1284P
const unsigned eepromSize = 4096;
const unsigned sramSize = 16384;
const unsigned progSize = 131072;
#endif

#define IOBASE		32
#define SRAMBASE	256

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
		UCSR0A, UCSR0B, UCSR0C, resc3, UBRR0L, UBRR0H, UDR0, resc7,
        resc8, resc9, resca, rescb, rescc, rescd, resce, rescf,
        resd0, resd1, resd2, resd3, resd4, resd5, resd6, resd7,
        resd8, resd9, resda, resdb, resdc, resdd, resde, resdf,
        rese0, rese1, rese2, rese3, rese4, rese5, rese6, rese7,
        rese8, rese9, resea, reseb, resec, resed, resee, resef,
        resf0, resf1, resf2, resf3, resf4, resf5, resf6, resf7,
        resf8, resf9, resfa, resfb, resfc, resfd, resfe, resff
	};
}

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;

using namespace std;

struct joyButton { u8 button; u8 bit; };
struct joyAxis { int axis; u8 bits; };
struct joystickState {
	SDL_Joystick *device;
	struct joyButton *buttons;
	struct joyAxis axes[MAX_JOYSTICK_AXES];
	u32 hats; // 4 bits per hat (1 for each direction)
};

enum { JMAP_IDLE, JMAP_INIT, JMAP_BUTTONS, JMAP_AXES, JMAP_MORE_AXES, JMAP_DONE };

struct joyMapSettings {
	int jstate, jiter, jindex, jaxis;
};

enum cpu_state {
	CPU_RUNNING,
	CPU_STOPPED,
	CPU_SINGLE_STEP
};

class GdbServer;

//SPI state machine states
enum{
    SPI_IDLE_STATE,
    SPI_ARG_X_LO,
    SPI_ARG_X_HI,
    SPI_ARG_Y_LO,
    SPI_ARG_Y_HI,
    SPI_ARG_CRC,
    SPI_RESPOND_SINGLE,
    SPI_RESPOND_MULTI,
    SPI_READ_SINGLE_BLOCK,
    SPI_READ_MULTIPLE_BLOCK,
    SPI_WRITE_SINGLE,
    SPI_WRITE_SINGLE_BLOCK,
};

struct SDPartitionEntry{
    u8 state;
    u8 startHead;
    u16 startCylinder;
    u8 type;
    u8 endHead;
    u16 endCylinder;
    u32 sectorOffset;
    u32 sectorCount;
};

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
		prevWDR(0), frameCounter(0), new_input_mode(false),gdb(0),enableGdb(false), gdbBreakpointFound(false),gdbInvalidOpcode(false),gdbPort(1284),state(CPU_STOPPED),
        spiByte(0), spiClock(0), spiTransfer(0), spiState(SPI_IDLE_STATE), spiResponsePtr(0), spiResponseEnd(0),eepromFile("eeprom.bin"),joystickFile(0),


    #if defined(__WIN32__)
        hDisk(INVALID_HANDLE_VALUE),
    #endif
        sdImage(0),emulatedMBR(0)        
	{
		memset(r, 0, sizeof(r));
		memset(io, 0, sizeof(io));
		memset(sram, 0, sizeof(sram));
		memset(eeprom, 0, sizeof(eeprom));
		memset(progmem,0,progSize);

		PIND = 8;
		SPL = (SRAMBASE+sramSize-1) & 0x00ff;
		SPH = (SRAMBASE+sramSize-1) >> 8;
        	spiTransfer = 0;
#if GUI
		pad_mode = SNES_PAD;
#endif
	}

	u16 progmem[progSize/2];
	u16 pc;
	u16 breakpoint;

	GdbServer *gdb;
	bool enableGdb;
	bool gdbBreakpointFound;
	bool gdbInvalidOpcode;
	int gdbPort;
	cpu_state state;

	u8 TEMP;				// for 16-bit timers
	u32 cycleCounter, prevPortB, prevWDR;
	bool singleStep, nextSingleStep, enableSound, fullscreen, framelock, interlaced,
		new_input_mode;
	int interruptLevel;
	u32 lastFlip;
	u32 inset;
#if GUI
	SDL_Surface *screen;
	joystickState joysticks[MAX_JOYSTICKS];
	joyMapSettings jmap;
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
	ringBuffer audioRing;
#endif
    
    // SPI Emulation
    u8 spiByte;
    u8 spiTransfer;
    u16 spiClock;
    u16 spiCycleWait;
    u8 spiState;
    u8 spiCommand;
    union{
        u32 spiArg;
        union{
            struct{
                u16 spiArgY;
                u16 spiArgX;
            };
            struct{
                u8 spiArgYlo;
                u8 spiArgYhi;
                u8 spiArgXlo;
                u8 spiArgXhi;
            };
        };
    };
    
    u32 spiByteCount;
    u8 spiResponseBuffer[12];
    u8* spiResponsePtr;
    u8* spiResponseEnd;
    
     
    // SD Emulation
#if defined(__WIN32__)
    HANDLE hDisk;
    LPBYTE lpSector;
    u32 lpSectorIndex;
#endif
    FILE* sdImage;
    u8* emulatedMBR;
    u32 emulatedReadPos;
    size_t emulatedMBRLength;
    u32 sectorSize;
    char* eepromFile;
	char* joystickFile;
    u8 eeprom[eepromSize];
    u8 eeClock;

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
			u8 io[256];		// Direct-mapped I/O space
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
                u8 resc8, resc9, resca, rescb, rescc, rescd, resce, rescf;
                u8 resd0, resd1, resd2, resd3, resd4, resd5, resd6, resd7;
                u8 resd8, resd9, resda, resdb, resdc, resdd, resde, resdf;
                u8 rese0, rese1, rese2, rese3, rese4, rese5, rese6, rese7;
                u8 rese8, rese9, resea, reseb, resec, resed, resee, resef;
                u8 resf0, resf1, resf2, resf3, resf4, resf5, resf6, resf7;
                u8 resf8, resf9, resfa, resfb, resfc, resfd, resfe, resff;
			};
		};
		u8 sram[sramSize];
	};

	void write_io(u8 addr,u8 value);
	u8 read_io(u8 addr);

	inline u8 read_progmem(u16 addr)
	{
		u16 word = progmem[addr>>1];
		return (addr&1)? word>>8 : word;
	}

	inline void write_sram(u16 addr,u8 value)
	{
		if (addr < IOBASE)
			r[addr] = value;		// Write a register
		else if (addr >= IOBASE && addr < SRAMBASE)
			write_io(addr - IOBASE, value);
		else {
			//if (addr >= SRAMBASE + sramSize)
			//	printf("illegal write of %x to addr %x, pc = %x\n",value,addr,pc-1);
			sram[(addr - SRAMBASE) & (sramSize-1)] = value;
		}
	}

	inline u8 read_sram(u16 addr)
	{
		if (addr < IOBASE)
			return r[addr];		// Read a register
		else if (addr >= IOBASE && addr < SRAMBASE)
			return read_io(addr - IOBASE);
		else {
			//if (addr >= SRAMBASE + sramSize)
			//	printf("illegal read from addr %x, pc = %x\n",addr,pc-1);
			return sram[(addr - SRAMBASE) & (sramSize-1)];
		}
	}

	inline static int get_insn_size(u16 insn)
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
	void init_joysticks();
	void handle_key_down(SDL_Event &ev);
	void handle_key_up(SDL_Event &ev);
	void update_buttons(int key,bool down);
	void update_joysticks(SDL_Event &ev);
	void set_jmap_state(int state);
	void map_joysticks(SDL_Event &ev);
	void load_joystick_file(const char* filename);
#endif
	void trigger_interrupt(int location);
	u8 exec(bool disasmOnly,bool verbose);
    void spi_calculateClock();    
	void update_hardware(int cycles);    
    void update_spi();
    void SDLoadImage(char *filename);    
    void SDBuildMBR(SDPartitionEntry* entry);    
#if defined(__WIN32__)
    void SDMapDrive(const char* driveLetter);
#endif
    void SDSeekToOffset(u32 offset);    
    u8 SDReadByte();    
    void SDWriteByte(u8 value);    
    void SDCommit();
    void LoadEEPROMFile(char* filename);
    void shutdown(int errcode);
    void idle(void);
};
#endif

// undefine the following to disable SPI debug messages
#ifdef USE_SPI_DEBUG 
    #define SPI_DEBUG(...) printf(__VA_ARGS__)
#else
    #define SPI_DEBUG(...)
#endif

#ifdef USE_EEPROM_DEBUG 
    #define EEPROM_DEBUG(...) printf(__VA_ARGS__)
#else
    #define EEPROM_DEBUG(...)
#endif

#ifdef USE_PORT_PRINT
#else
#endif
