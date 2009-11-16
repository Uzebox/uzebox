/*
 *  Uzebox(tm) Global defines and build options
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
 *  Uzebox is a reserved trade mark
*/

/** 
 * ==============================================================================
 *
 * This file contains default compilation setting that sets the video mode and 
 * enables certains options. Note that these options affects the
 * memory consomption (RAM & FLASH).
 *
 * ===============================================================================
 */
#ifndef __DEFINES_H_
	#define __DEFINES_H_
 	#include <avr/io.h>

		
	//Generic define
	#define DISABLED 0
	#define ENABLED  1


 	/*
	 * Defines the video mode to use. 
	 *
	 * 0 = Reserved
	 * 1 = 40x28 Tile-only. 'Unlimited' tiles per frame (16 bit VRAM). 6x8 tiles.
	 * 2 = 22x26 Tiles+Sprites, full-screen scrolling with split screens. Max 256 background tiles. 6x8 tiles & sprites.
	 * 3 = 30x28 Tiles+Sprites (currently no scrolling). Max 256 background tiles. 8x8 tiles. 
	 */	
	#ifndef VIDEO_MODE 
		#define VIDEO_MODE 1
	#endif

	/*
	 * Enable horizontal scrolling for video mode 2.
	 * 
	 * Note: This option needs 9K of flash due to unrolled loops.
	 * 
	 * 0 = no
	 * 1 = yes
	 */	
	#ifndef MODE2_HORIZONTAL_SCROLLING
		#define MODE2_HORIZONTAL_SCROLLING 1
	#endif


	/*
	 * Display the Uzebox logo when the console is reset
	 * 0 = none
	 * 1 = with "bling" sound
	 * 2 = with "uzebox" synth voice (PCM required, MIXER_CHAN4_TYPE must == 1)
	 */
	#ifndef INTRO_LOGO
		#define INTRO_LOGO 1
	#endif

	/*
	 * Joystick type used on the board.
	 * Note: Will be read from EEPROM in a future release. 
	 *
	 * 0 = SNES
	 * 1 = NES
	 */
	#ifndef JOYSTICK
		#define JOYSTICK 0
	#endif

	/*
	 * Activates the MIDI-IN support. 
	 * Not supported with video mode 2.
	 *
	 * 0 = no
	 * 1 = yes
	 */
	#ifndef MIDI_IN
		#define MIDI_IN 0
	#elif MIDI_IN == 1
		#define UART_RX_BUFFER 1
	#endif

	/*
	 * Activates the UART receive buffer 
	 * Not supported with video mode 2.
	 *
	 * 0 = no
	 * 1 = yes
	 */
	#ifndef UART_RX_BUFFER
		#define UART_RX_BUFFER 0
	#endif

	/*
	 * Activates the UART receive buffer 
	 * Not supported with video mode 2.
	 *
	 * 0 = no
	 * 1 = yes
	 */
	#ifndef UART_RX_BUFFER_SIZE
		#define UART_RX_BUFFER_SIZE 128
	#else
		#if UART_RX_BUFFER_SIZE !=2 || UART_RX_BUFFER_SIZE !=4 || UART_RX_BUFFER_SIZE !=8 || UART_RX_BUFFER_SIZE !=16 || UART_RX_BUFFER_SIZE !=32 || UART_RX_BUFFER_SIZE !=64 || UART_RX_BUFFER_SIZE !=128 || UART_RX_BUFFER_SIZE !=256
			#error Invalid size for UART_RX_BUFFER_SIZE: must be a power of 2.
		#endif
	#endif

	/*
	 * Screen center adjustment for mode 1 only.
	 * Useful if your game field absolutely needs a non-even width.
	 * Do not go more than +14/-14 (video mode dependent).
	 *
	 * Center = 0
	 */
	#ifndef CENTER_ADJUSTMENT
		#define CENTER_ADJUSTMENT 0
	#endif

	/*
	 * Channel 4 type
	 *
	 * 0=LFSR noise
	 * 1=PCM
	 */
	#ifndef MIXER_CHAN4_TYPE
		#define MIXER_CHAN4_TYPE 0
	#endif


	/*
	 * Define wavetable
	 *
	 * Specify the location of the wave table
	 */
	#ifndef MIXER_WAVES
		#define MIXER_WAVES "data/sounds.inc"
	#endif


	/*
	 * Include default mixer waves (takes ~2.5K of flash)
	 */
	#ifndef INCLUDE_DEFAULT_WAVES
		#define INCLUDE_DEFAULT_WAVES 1
	#endif

	/*
	 * These are are temp fix when using video mode 3.
	 * Since that mode takes a lot of cycles to
	 * blit sprites, not enough CPU is left
	 * and the program can crash. Disable 1
	 * or more sound channels mixing to 
	 * regain enough CPU. 
	 *
	 * Sound channel 1 is always enabled.
	 *
	 * 0=disable
	 * 1=enable
	 */
	#ifndef SOUND_CHANNEL_2_ENABLE
		#define SOUND_CHANNEL_2_ENABLE 1
	#endif

	#ifndef SOUND_CHANNEL_3_ENABLE
		#define SOUND_CHANNEL_3_ENABLE 1
	#endif
	
	#ifndef SOUND_CHANNEL_4_ENABLE
		#define SOUND_CHANNEL_4_ENABLE 1
	#endif

	/*
	 * Completely remove all the sound mixer & repalyer code.
	 * The ring buffer & basic sound functionality is preserved for 
	 * the emulator and application that fills themselves the buffer.
	 */
	#ifndef ENABLE_MIXER
		#define ENABLE_MIXER 1
	#endif
		
	/*
	 * Define the ammount of memory to allocate
	 * for ramtiles 
	 * (used in video modes that supports ramtiles)
	 */
	 
	#ifndef RAM_TILES_COUNT
		#define RAM_TILES_COUNT 0
	#endif

	/*
	 * Defines the numbers of sprites to alloacate ressources for.
	 * (used in video modes that supports sprites)
	 */
	#ifndef MAX_SPRITES
		#define MAX_SPRITES 32
	#endif

	/*
	 * Number of screen sections to allocate memory for
	 * Min=1, Max=SCREEN_TILES_V*TILE_HEIGHT
	 * (used in video modes that supports screen sections)
	 */
	#ifndef SCREEN_SECTIONS_COUNT
		#define SCREEN_SECTIONS_COUNT 1
	#endif


	/*
	 * Kernel Internal settings, do not modify
	 */

	//Pin used to enable the AD723
	#define VIDEOCE_PIN PB4

	//sound player master volume
	#define DEFAULT_MASTER_VOL	0x6f

	//Joypad standard buttons mappings.
	//Applies to both NES & SNES gamepads.
	#define TYPE_SNES 0
	#define TYPE_NES 1

	#if JOYSTICK == TYPE_SNES
		#define BTN_SR	   2048
		#define BTN_SL	   1024
		#define BTN_X	   512
		#define BTN_A	   256
		#define BTN_RIGHT  128
		#define BTN_LEFT   64
		#define BTN_DOWN   32
		#define BTN_UP     16
		#define BTN_START  8
		#define BTN_SELECT 4
		#define BTN_Y      2
		#define BTN_B      1
	#elif JOYSTICK == TYPE_NES
		#define BTN_SR	   2048 //unused
		#define BTN_SL	   1024 //unused		
		#define BTN_X	   512 //unused
		#define BTN_Y      256 //unused

		#define BTN_RIGHT  128
		#define BTN_LEFT   64
		#define BTN_DOWN   32
		#define BTN_UP     16
		#define BTN_START  8
		#define BTN_SELECT 4
		#define BTN_B      2
		#define BTN_A      1
	#endif 

	#define BTN_MOUSE_LEFT 512
	#define BTN_MOUSE_RIGHT 256

	#define MOUSE_SENSITIVITY_LOW    0b00
	#define MOUSE_SENSITIVITY_MEDIUM 0b10
	#define MOUSE_SENSITIVITY_HIGH   0b01

	
	//Screen sections flags
	#define SCT_PRIORITY_BG  0
	#define SCT_PRIORITY_SPR 1

	//Sprites Options
	#define SPR_OVERFLOW_CLIP   0
	#define SPR_OVERFLOW_ROTATE 1
	#define SPR_FLIP_X 1
	#define SPR_FLIP_Y 2


	//Patch commands
	#define PC_ENV_SPEED	0
	#define PC_NOISE_PARAMS	1
	#define PC_WAVE			2
	#define PC_NOTE_UP		3
	#define PC_NOTE_DOWN	4
	#define PC_NOTE_CUT		5
	#define PC_NOTE_HOLD 	6
	#define PC_ENV_VOL		7
	#define PC_PITCH		8
	#define PC_TREMOLO_LEVEL	9
	#define PC_TREMOLO_RATE	10
	#define PATCH_END		0xff


	#if MIXER_CHAN4_TYPE == 0
		#define WAVE_CHANNELS 3
		#define NOISE_CHANNELS 1
	#else
		#define WAVE_CHANNELS 4
		#define NOISE_CHANNELS 0
	#endif

	#define CHANNELS WAVE_CHANNELS+NOISE_CHANNELS
	#define SWEEP_UP   0x80
	#define SWEEP_DOWN 0x00


	//Line rate timer delay: 15.73426 kHz*2 = 1820/2 = 910
	//2x is to account for vsync equalization & serration pulse that are at 2x line rate
	#define HDRIVE_CL 1819
	#define HDRIVE_CL_TWICE 909
	#define SYNC_HSYNC_PULSES 253

	#define SYNC_PRE_EQ_PULSES 6
	#define SYNC_EQ_PULSES 6
	#define SYNC_POST_EQ_PULSES 6

	#define SYNC_PHASE_PRE_EQ	0
	#define SYNC_PHASE_EQ		1
	#define SYNC_PHASE_POST_EQ	2
	#define SYNC_PHASE_HSYNC	3

	#define SYNC_PIN PB0
	#define SYNC_PORT PORTB
	#define DATA_PORT PORTC


	#define CHANNEL_STRUCT_SIZE 10
	#define MIX_BANK_SIZE (SYNC_HSYNC_PULSES + ((SYNC_PRE_EQ_PULSES+SYNC_EQ_PULSES+SYNC_POST_EQ_PULSES)/2))
	#define MIX_BUF_SIZE MIX_BANK_SIZE*2
	//#define MIDI_RX_BUF_SIZE 128

	#define JOYPAD_OUT_PORT PORTA
	#define JOYPAD_IN_PORT PINA
	#define JOYPAD_CLOCK_PIN PA3
	#define JOYPAD_LATCH_PIN PA2
	#define JOYPAD_DATA1_PIN PA0
	#define JOYPAD_DATA2_PIN PA1

	#define EEPROM_BLOCK_SIZE 32
	#define EEPROM_HEADER_SIZE 2
	#define EEPROM_SIGNATURE 0x555A
	#define EEPROM_SIGNATURE2 0x555B
	#define EEPROM_FREE_BLOCK 0xffff
	#define EEPROM_ERROR_INVALID_BLOCK 0x1
	#define EEPROM_ERROR_FULL 0x2
	#define EEPROM_ERROR_BLOCK_NOT_FOUND 0x3
	#define EEPROM_ERROR_NOT_FORMATTED 0x4


	#if VIDEO_MODE == 1 
		#include "videoMode1/videoMode1.def.h"
	#elif VIDEO_MODE == 2
		#include "videoMode2/videoMode2.def.h"
	#elif VIDEO_MODE == 3
		#include "videoMode3/videoMode3.def.h"
	#elif VIDEO_MODE == 4
		#include "videoMode4/videoMode4.def.h"
	#elif VIDEO_MODE == 6
		#include "videoMode6/videoMode6.def.h"
	#elif VIDEO_MODE == 7
		#include "videoMode7/videoMode7.def.h"
	#elif VIDEO_MODE == 8
		#include "videoMode8/videoMode8.def.h"
	#else
		#error Invalid video mode defined with VIDEO_MODE
	#endif

#endif
