/*
 * Emulator8080.h
 *
 *  Created on: 17 déc. 2020
 *      Author: admin
 */

#ifndef EMULATOR8080_H_
#define EMULATOR8080_H_

struct RegisterStruct
{
/*
	u16 rPC;
	u16 rHL;
	u16 rBC;
	u16 rDE;
	u16 rSP;
	u16 rFA;
	u8  rParity;
	u8  rIntFlag;
	u8  lastop;
	u16 lastPC;
	u8  flags8080;
	u16 counter;
*/
    unsigned int  pc;
    union {
    	unsigned int rHL;
    	struct{
    	    unsigned char rL;
    		unsigned char rH;
    	};
    };
    //unsigned char rL;
	//unsigned char rH;
    union {
    	unsigned int rBC;
    	struct{
    		unsigned char rC;
    		unsigned char rB;
    	};
    };
	//unsigned char rC;
	//unsigned char rB;

    union {
    	unsigned int rDE;
    	struct{
    		unsigned char rE;
    		unsigned char rD;
    	};
    };

	//unsigned char rE;
	//unsigned char rD;

    union {
    	unsigned int rSP;
    	struct{
    		unsigned char rSp_lo;
    		unsigned char rSp_hi;
    	};
    };

	//unsigned char rSp_lo;
	//unsigned char rSp_hi;

    union {
    	unsigned int rAF;
    	struct{
    	   unsigned char rFlags;			//internal AVR flags
    	   unsigned char rA;
    	};
    };

    //unsigned char rFlags;			//internal AVR flags
	//unsigned char rA;

	unsigned char rParity;
	unsigned char intflag;
	unsigned char flags_8080;
	unsigned int breakpoint;

	//unsigned int  counter;			//Counter incremented each frame. Used for benchmarking and RTC emulation.
	//unsigned char counter_lat;		//latched hi byte is trigger when reading lo byte
	//unsigned char ram_size;			//SPI RAM size detected 0=64K, 1=128K
	//unsigned char lastport;
	//unsigned char lastop;
	//unsigned int  lastpc;
};

struct FdcStruct{
	unsigned char drive;	//FDC drive
	unsigned char track;	//FDC track
	unsigned char sector;	//FDC sector
	unsigned int  dma;		//FDC DMA address
};

extern struct RegisterStruct cpu_registers;
struct FdcStruct fdcports;

#endif /* EMULATOR8080_H_ */
