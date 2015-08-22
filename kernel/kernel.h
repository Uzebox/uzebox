/*
 *  Uzebox(tm) kernel build options
 *  Copyright (C) 2008-2009 Alec Bourque
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

	typedef uint8_t  u8;
	typedef int8_t   s8;
	typedef uint16_t u16;
	typedef int16_t  s16;
	typedef uint32_t u32;
	typedef int32_t  s32;


	/*
	* Kernel data structures
	*/

	struct MixerWaveChannelStruct
	{
		unsigned char volume;			//(0-255)
		unsigned int  step;				//8:8 fixed point
		unsigned char positionFrac; 	//8bit fractional part 
		const char *position;			//16bit sample pointer	
	};


	struct MixerNoiseChannelStruct
	{
		unsigned char volume;				//(0-255)
		unsigned char params;				//bit0=7/15 bits lfsr, b1:6=divider (samples between shifts)
		unsigned int  barrel;				//16bit LFSR barrel shifter
		unsigned char divider;				//divider accumulator
		unsigned char reserved;
	};

	//Common type for both channels (except noise chan)
	struct MixerChannelStruct
	{
		unsigned char volume;				//(0-255)
		unsigned int  step;					//8:8 fixed point
		unsigned const char structAlignment[3];	//dont access!
	};


	struct SubChannelsStruct
	{
		struct MixerWaveChannelStruct wave[WAVE_CHANNELS];
		
		#if MIXER_CHAN4_TYPE == 0		
			struct MixerNoiseChannelStruct noise;		
		#endif 

		struct MixerWaveChannelStruct pcm;	
	};

	struct MixerStruct
	{
		union Channels{
			struct MixerChannelStruct all[CHANNELS];
			struct SubChannelsStruct type;	
		}channels;

		int pcmLoopLenght;
		const char *pcmLoopEnd;			//PCM channel's absolute adress of PCM loop end.
	};


	extern void SetMixerNoiseParams(unsigned char params);
	extern void SetMixerWave(unsigned char channel,unsigned char patch);
	extern void SetMixerNote(unsigned char channel,unsigned char note);//,int volume);
	extern void SetMixerVolume(unsigned char channel,unsigned char volume);

	typedef void (*VsyncCallBackFunc)(void);
	typedef void (*HsyncCallBackFunc)(void);

	#define TRACK_FLAGS_SLIDING		8
	#define TRACK_FLAGS_ALLOCATED	16
	#define TRACK_FLAGS_PLAYING		32
	#define TRACK_FLAGS_HOLD_ENV	64
	#define TRACK_FLAGS_PRIORITY	128

	struct TrackStruct
	{
		unsigned char flags;		//b0-b2: reserved
									//b3: pitch slide		: 1=sliding to note, 0=slide off
									//b4: allocated 		: 1=used by music player, 0=voice can be controlled by main program
									//b5: patch playing 	: 1=playing, 0=stopped
									//b6: hold envelope		: 1=hold volume envelope, i.e: don't increae/decrease, 0=don't hold
									//b7: priority			: 1=Hi/Fx, 0=low/regular note

		unsigned char note;
		unsigned char channel;

		#if MUSIC_ENGINE == MOD
			const char *patternPos;
		#else
			unsigned char expressionVol;
		#endif

		u8 loopCount; 

		s16 slideStep;		//used to slide to note
		u8  slideNote;		//target note
		u8	slideSpeed;		//fixed point 4:4, 1:0= 1/16 half note per frame

		unsigned char tremoloPos;
		unsigned char tremoloLevel;
		unsigned char tremoloRate;

		unsigned char trackVol;
		unsigned char noteVol;
		unsigned char envelopeVol;		//(0-255)
		char envelopeStep;				//signed, amount of envelope change each frame +127/-128
		
		unsigned char patchNo;
		unsigned char fxPatchNo;
		unsigned char patchNextDeltaTime;
		unsigned char patchCurrDeltaTime;
		unsigned char patchPlayingTime;	//used by fx to steal oldest voice
		const char *patchCommandStreamPos;
		
	};
	typedef struct TrackStruct Track;


	typedef void (*PatchCommand)(Track* track, char value);

	extern unsigned char mix_buf[];
	extern volatile unsigned char *mix_pos;
	extern volatile unsigned char mix_bank;
	extern unsigned char tr4_barrel_lo;
	extern unsigned char tr4_barrel_hi;
	extern unsigned char tr4_params;
	
	extern struct MixerStruct mixer;		//low level sound mixer
	extern Track tracks[CHANNELS];			//music player tracks
	extern void ProcessMusic(void);

	extern volatile u8 uart_rx_buf_start;
	extern volatile u8 uart_rx_buf_end;
	extern volatile u8 uart_rx_buf[];

	struct  PatchStruct{   
   		unsigned char type;
		const char *pcmData;
		const char *cmdStream;  
		unsigned int loopStart;
		unsigned int loopEnd;   		       
	}; 
	typedef struct PatchStruct Patch;

	extern void SetColorBurstOffset(unsigned char offset);
	void ProcessMouseMovement(void);
	void ProcessFading();

	//EEPROM Kernel structs
	#define EEPROM_HEADER_VER 1
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

		//used by the bootloader to know the currently flashed game
		unsigned long currentGameCrc32;

		//Bootloader flags:
		//b0: Boot method: 0=Bootloader starts on reset (default) , 1=Current game starts on reset (hold any key to enter bootloader)
		//b1-b7: reserved		
		unsigned char bootloaderFlags; 

		//for future expansion
		unsigned char reserved[9];			
	};

	struct EepromBlockStruct{
		//some unique block ID assigned by ?. If 0xffff, block is free.
		unsigned int id;
		
		//application specific data
		//cast to your own types
		unsigned char data[30];		
	};

#endif
