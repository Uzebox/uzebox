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
 *  Uzebox is a reserved trade mark
*/

/** 
 * ==============================================================================
 *
 * This file contains default compilation setting that sets the video mode and 
 * enables certains options. Note that these options affects the
 * memory consomption (RAM & FLASH).
 *
 * Since this is a defaults file that is part of the kernel and affect 
 * all games, it is preferable you define custom compilation options. 
 * In AvrStudio go in: Project->Congiguration Options->Custom Options
 * and add -D switches. 
 * 
 * I.e.: To define video mode 2 add a -DVIDEO_MODE=2 switch under [All files]
 *
 * ===============================================================================
 */
#ifndef __DEFINES_H_
	#define __DEFINES_H_
 	#include <avr/io.h>
	
	//Generic define
	#define DISABLED 0
	#define ENABLED  1

	//Pin used to enable the AD723
	#define VIDEOCE_PIN PB4

	//sound player master volume
	#define DEFAULT_MASTER_VOL	0x6f

	//Joypad standard buttons mappings.
	//Applies to both NES & SNES gamepads.
	#define TYPE_SNES 0
	#define TYPE_NES 1

	#define BTN_SR	   1
	#define BTN_SL	   2
	#define BTN_X	   4
	#define BTN_A	   8
	#define BTN_RIGHT  16
	#define BTN_LEFT   32
	#define BTN_DOWN   64
	#define BTN_UP     128
	#define BTN_START  256
	#define BTN_SELECT 512
	#define BTN_Y      1024 
	#define BTN_B      2048 

	//Screen sections flags
	#define SCT_PRIORITY_BG  0
	#define SCT_PRIORITY_SPR 1

	//Sprites Options
	#define SPR_OVERFLOW_CLIP   0
	#define SPR_OVERFLOW_ROTATE 1

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

 	/*
	 * Defines the video mode to use. 
	 * For all modes, tiles are 6x8 pixels (horizontal x vertical)
	 *
	 * 0 = Reserved
	 * 1 = 40x28 Tile-only.
	 * 2 = 22x26 Tiles, Sprites and full-screen scrolling.
	 * 
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
	 * 2 = with "uzebox" synth voice
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
	#endif

	/*
	 * Screen center adjustment for mode 1 only.
	 * Useful if your game field absolutely needs a non-even width.
	 * Do not go more than +14/-14.
	 *
	 * Center = 0
	 */
	#ifndef CENTER_ADJUSTMENT
		#define CENTER_ADJUSTMENT 0
	#endif

	
	/*
	 * Number of screen sections to allocate memory for
	 * Min=1, Max=SCREEN_TILES_V*TILE_HEIGHT
	 */
	#ifndef SCREEN_SECTIONS_COUNT
		#define SCREEN_SECTIONS_COUNT 1
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
	 * Include wavetable
	 *
	 * 0=dont' include (saves ~2.3K flash)
	 * 1=include waves
	 */
	#ifndef INCLUDE_DEFAULT_WAVES
		#define INCLUDE_DEFAULT_WAVES 1
	#endif





	/*
	 * Kernel Internal settings, do not modify
	 */

	#if VIDEO_MODE == 1
		#define TILE_HEIGHT 8
		#define TILE_WIDTH 6
	
		#define VRAM_TILES_H 40 

		#ifndef VRAM_TILES_V
			#define VRAM_TILES_V 28
		#endif

		#define SCREEN_TILES_H 40
		#define SCREEN_TILES_V 28
	
		#define FIRST_RENDER_LINE 20
		#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V*2
		#define VRAM_ADDR_SIZE 2 //in bytes

	#elif VIDEO_MODE == 2
		#define TILE_HEIGHT 8
		#define TILE_WIDTH 6

		#define VRAM_TILES_H  32
		
		#ifndef VRAM_TILES_V
			#define VRAM_TILES_V 32
		#endif

		#define SCREEN_TILES_H 22
		#define SCREEN_TILES_V 26
		#define FIRST_RENDER_LINE 24
		#define MAX_SPRITES 32
		#define MAX_SPRITES_PER_LINE 5
		#define SPRITE_STRUCT_SIZE 3

		#define SCREEN_SECTION_STRUCT_SIZE 15

		#define TRANSLUCENT_COLOR 0xfe	
		#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V	
		#define X_SCROLL_WRAP VRAM_TILES_H*TILE_WIDTH

		#define VRAM_ADDR_SIZE 1 //in bytes

	#endif

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
	#define MIDI_RX_BUF_SIZE 128

	#define JOYPAD_OUT_PORT PORTA
	#define JOYPAD_IN_PORT PINA
	#define JOYPAD_CLOCK_PIN PA3
	#define JOYPAD_LATCH_PIN PA2
	#define JOYPAD_DATA1_PIN PA0
	#define JOYPAD_DATA2_PIN PA1

#endif
