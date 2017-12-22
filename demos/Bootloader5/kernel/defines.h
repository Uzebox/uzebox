/*
 *  Uzebox(tm) Global defines and build options
 *  Copyright (C) 2008-2009 Alec Bourque
 *  Optimized and trimmed to the bootloader by Sandor Zsuga (Jubatian), 2017
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


/*
** For some reason the Atmega1284P io.h does not include the old "PA0" defines
*/
#ifndef PA0
#define PA0 PORTA0
#define PA1 PORTA1
#define PA2 PORTA2
#define PA3 PORTA3
#define PA4 PORTA4
#define PA5 PORTA5
#define PA6 PORTA6
#define PA7 PORTA7
#endif
#ifndef PB0
#define PB0 PORTB0
#define PB1 PORTB1
#define PB2 PORTB2
#define PB3 PORTB3
#define PB4 PORTB4
#define PB5 PORTB5
#define PB6 PORTB6
#define PB7 PORTB7
#endif
#ifndef PC0
#define PC0 PORTC0
#define PC1 PORTC1
#define PC2 PORTC2
#define PC3 PORTC3
#define PC4 PORTC4
#define PC5 PORTC5
#define PC6 PORTC6
#define PC7 PORTC7
#endif
#ifndef PD0
#define PD0 PORTD0
#define PD1 PORTD1
#define PD2 PORTD2
#define PD3 PORTD3
#define PD4 PORTD4
#define PD5 PORTD5
#define PD6 PORTD6
#define PD7 PORTD7
#endif


/*
** Generic defines
*/
#define DISABLED    0
#define ENABLED     1


/*
** Defines the video mode to use. Only bootloader (0) is available
*/
#ifndef VIDEO_MODE
#define VIDEO_MODE  0
#endif


/*
** Joystick type used on the board.
** Note: Will be read from EEPROM in a future release.
*/
#define TYPE_SNES   0
#define TYPE_NES    1
#ifndef JOYSTICK
#define JOYSTICK    TYPE_SNES
#endif


/*
** Disable second player's controller.
** This saves some ROM and RAM for single player games
*/
#ifndef P2_DISABLE
#define P2_DISABLE  0
#endif


/*
** Pin used to enable the AD723
*/
#define VIDEOCE_PIN PB4


/*
** Sound player master volume
*/
#define DEFAULT_MASTER_VOL 0x6F


/*
** Joypad standard buttons mappings.
** Applies to both NES & SNES gamepads.
*/
#if   (JOYSTICK == TYPE_SNES)
#define BTN_SR     2048
#define BTN_SL     1024
#define BTN_X      512
#define BTN_A      256
#define BTN_RIGHT  128
#define BTN_LEFT   64
#define BTN_DOWN   32
#define BTN_UP     16
#define BTN_START  8
#define BTN_SELECT 4
#define BTN_Y      2
#define BTN_B      1
#elif (JOYSTICK == TYPE_NES)
#define BTN_SR     2048 /* Unused */
#define BTN_SL     1024 /* Unused */
#define BTN_X      512  /* Unused */
#define BTN_Y      256  /* Unused */
#define BTN_RIGHT  128
#define BTN_LEFT   64
#define BTN_DOWN   32
#define BTN_UP     16
#define BTN_START  8
#define BTN_SELECT 4
#define BTN_B      2
#define BTN_A      1
#endif


/*
** Inline mixer cycles
*/
#define AUDIO_OUT_HSYNC_CYCLES (133)


/*
** Line rate timer delay: 15.73426 kHz*2 = 1820/2 = 910
** 2x is to account for vsync equalization & serration pulse that are at 2x line rate
*/
#define HDRIVE_CL           1819
#define HDRIVE_CL_TWICE     909
#define SYNC_HSYNC_PULSES   253

#define SYNC_PRE_EQ_PULSES  6
#define SYNC_EQ_PULSES      6
#define SYNC_POST_EQ_PULSES 6

#define SYNC_FLAG_VSYNC     1
#define SYNC_FLAG_FIELD     2

#define SYNC_PIN   PB0
#define SYNC_PORT  PORTB
#define DATA_PORT  PORTC

#define VIDEO_PORT _SFR_IO_ADDR(DATA_PORT)


/*
** Reserved region in RAM containing variables which are set up before
** clearing sweep
*/
#define RAM_RESERVED        0x10E0


#define JOYPAD_OUT_PORT     PORTA
#define JOYPAD_IN_PORT      PINA
#define JOYPAD_CLOCK_PIN    PA3
#define JOYPAD_LATCH_PIN    PA2
#define JOYPAD_DATA1_PIN    PA0
#define JOYPAD_DATA2_PIN    PA1

#define EEPROM_HEADER_VER   1
#define EEPROM_BLOCK_SIZE   32
#define EEPROM_HEADER_SIZE  1
#define EEPROM_SIGNATURE    0x555A
#define EEPROM_SIGNATURE2   0x555B
#define EEPROM_FREE_BLOCK   0xffff

#define EEPROM_ERROR_INVALID_BLOCK   0x1
#define EEPROM_ERROR_FULL            0x2
#define EEPROM_ERROR_BLOCK_NOT_FOUND 0x3
#define EEPROM_ERROR_NOT_FORMATTED   0x4

#if   VIDEO_MODE == 0
#include "videoMode0/videoMode0.def.h"
#else
#error Invalid video mode defined with VIDEO_MODE
#endif

#ifdef HSYNC_USABLE_CYCLES
#if ((HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES) < 0)
#error There is not enough CPU cycles to support the build options. Disable the UART (-DUART=0), audio channel 5 (-DSOUND_CHANNEL_5_ENABLE=0) or the inline mixer (-DSOUND_MIXER=0).
#endif
#endif

#endif
