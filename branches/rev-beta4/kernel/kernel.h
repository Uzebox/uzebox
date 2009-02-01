/*
 *  Uzebox(tm) kernel build options
 *  Copyright (C) 2008  Alec Bourque
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Uzebox is a reserved trade mark
*/

#ifndef __KERNEL_H_
#define __KERNEL_H_

	#include "defines.h"

	#if VRAM_ADDR_SIZE == 1
		extern unsigned char vram[];  
	#else
		extern unsigned int vram[]; 
	#endif



	#if  VIDEO_MODE == 2
		struct SpriteStruct
		{
			unsigned char x;
			unsigned char y;
			unsigned char tileIndex;
		};
	#elif VIDEO_MODE == 3
		struct SpriteStruct
		{
			unsigned char x;
			unsigned char y;
			unsigned char tileIndex;
			unsigned char flags;
		};			
		
		struct BgRestoreStruct{
			unsigned int addr;
			unsigned char tileIndex;
		};

	#endif

	//	unsigned char scrollX: x displacement
	//	unsigned char scrollY: y displacement
	//	unsigned char height: section height in scanlines
	//	unsigned char *vramBaseAdress: location in vram where this section starts rendering
	//	unsigned char *tileTableAdress: tile set to use when rendering this section
	//	unsigned char wrapLine: Define at what Y scroll within that section Y-wrap will happen (Normally Y-Wrap happens at 0xff). So
	//                          instead of wrapping each 32 tiles you can wrap each 12 or 12 tiles+ 3 lines, etc.
	//	unsigned char flags: 1=sprites will be drawn on top of this section. 0=sprites will be under.
	struct ScreenSectionStruct
	{
		//user defined
		unsigned char scrollX;
		unsigned char scrollY;
		unsigned char height;
		unsigned char *vramBaseAdress;
		const char *tileTableAdress;
		unsigned char wrapLine;
		unsigned char flags;

		//calculated (don't write to)
		unsigned char scrollXcoarse;
		unsigned char scrollXfine;		
		unsigned char *vramRenderAdress;
		unsigned char *vramWrapAdress;
	};

	#if VIDEO_MODE == 2 ||  VIDEO_MODE == 3
		extern struct SpriteStruct sprites[];
		extern struct ScreenSectionStruct screenSections[];
	#endif


	/*
	* Kernel data structures
	*/

	struct MixerWaveChannelStruct
	{
		unsigned char volume;			//(0-255)
		unsigned int  step;				//8:8 fixed point
		unsigned char positionFrac; 	//8bit fractional part 
		const char *position;			//16bit sample pointer	
		const char *loopStart;			//absolute adress of loop start. unused for chan 1,2,3. Used only if chan4==PBM
		const char *loopEnd;			//absolute adress of loop end. unused for chan 1,2,3. Used only if chan4==PBM
	};


	struct MixerNoiseChannelStruct
	{
		unsigned char volume;				//(0-255)
		unsigned char params;				//bit0=7/15 bits lfsr, b1:6=divider (samples between shifts)
		unsigned int  barrel;				//16bit LFSR barrel shifter
		unsigned char divider;				//divider accumulator
		unsigned char reserved[5];
	};

	//Common type for both channels
	struct MixerChannelStruct
	{
		unsigned char volume;				//(0-255)
		unsigned const char structAlignment[9];	//dont access!
	};

	struct SubChannelsStruct
	{
		struct MixerWaveChannelStruct wave[WAVE_CHANNELS];
		struct MixerNoiseChannelStruct noise;
	};

	struct MixerStruct
	{
		union Channels{
			struct MixerChannelStruct all[CHANNELS];
			struct SubChannelsStruct type;	
		}channels;

	};

	extern void SetMixerNoiseParams(unsigned char params);
	extern void SetMixerWave(unsigned char channel,unsigned char patch);
	extern void SetMixerNote(unsigned char channel,unsigned char note);//,int volume);
	extern void SetMixerVolume(unsigned char channel,unsigned char volume);

	typedef void (*PatchCommand)(unsigned char channel, char value);

	struct TrackStruct
	{
		bool allocated;				//true=used by music player, false=voice can be controlled by main program
		unsigned char priority;			//0=lowest
		unsigned char note;

		unsigned char tremoloPos;
		unsigned char tremoloLevel;
		unsigned char tremoloRate;

		unsigned char expressionVol;
		unsigned char trackVol;
		unsigned char noteVol;
		unsigned char envelopeVol;		//(0-255)
		char envelopeStep;				//signed, amount of envelope change each frame +127/-128
		bool patchPlaying;
		unsigned char patchNo;
		unsigned char fxPatchNo;
		unsigned char patchLastStatus;
		unsigned char patchNextDeltaTime;
		unsigned char patchCurrDeltaTime;
		unsigned char patchPlayingTime;	//used by fx to steal oldest voice
		unsigned char patchWave;		//0-7
		bool patchEnvelopeHold;
		const char *patchCommandStreamPos;

	};


	extern unsigned char mix_buf[];
	extern unsigned char *mix_pos;
	extern unsigned char mix_bank;
	extern unsigned char midi_rx_buf_start;
	extern unsigned char midi_rx_buf_end;
	extern unsigned char tr4_barrel_lo;
	extern unsigned char tr4_barrel_hi;
	extern unsigned char tr4_params;
	
	extern struct MixerStruct mixer;					//low level sound mixer
	extern struct TrackStruct tracks[CHANNELS];			//music player tracks
	extern void ProcessMusic(void);

	struct  PatchStruct{   
   		unsigned char type;
		const char *pcmData;
		const char *cmdStream;  
		unsigned int loopStart;
		unsigned int loopEnd;   		       
	}; 

	extern void SetColorBurstOffset(unsigned char offset);

	//EEPROM Kernel structs
	struct EepromHeaderStruct
	{
		//special identifier/magic number to determine if the EEPROM 
		//contains kernel recognizable data
		unsigned int signature;
		
		//version of this EEPROM data structure
		unsigned char version;

		//size of allocated blocks in bytes (should be 32)
		unsigned char blockSize;  

		//size of this header in blocks (should be 2)
		unsigned char headerSize;

		//identifies the harware type. Uzebox, Fuzebox,etc. Do we need that?
		unsigned char hardwareVersion;

		//identifies the harware revision. Do we need that?
		unsigned char hardwareRevision;

		/*
		Hardware features on board
		b15:b12 Reserved  
		b11     AD725 power control
		b10     PS2 Mouse
		b9      PS2 Keyboard
		b8      Ethernet
		b7      MIDI OUT
		b6      MIDI IN
		b5      SD Card Interface
		b4      Status LED
		b3      Soft Power switch 
		b2:b0   Joystick type: 0=SNES, 1=NES, 2-7=Reserved
		*/
		unsigned int features;

		//Even more features -- for future use
		unsigned int featuresExt;

		//MAC adress for the Ethernet interface
		unsigned char macAdress[6];		

		//Composite Color Correction 
		//0=none
		//1=shorten line 	
		unsigned char colorCorrectionType;	

		//for future expansion
		unsigned char reserved[14];		
	};

	struct EepromBlockStruct{
		//some unique block ID assigned by ?. If 0xffff, block is free.
		unsigned int id;
		
		//application specific data
		//cast to your own types
		unsigned char data[30];		
	};

#endif
