/*
 *  Uzebox Kernel - Video Mode 74
 *  Copyright (C) 2015 Sandor Zsuga (Jubatian)
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
 * ===========================================================================
 *
 * This file contains global defines for video mode 74
 *
 * ===========================================================================
 */

#pragma once

#define VMODE_ASM_SOURCE "videoMode74/videoMode74.s"
#define VMODE_C_SOURCE "videoMode74/videoMode74.c"
#define VMODE_C_PROTOTYPES "videoMode74/videoMode74.h"
#define VMODE_FUNC sub_video_mode74


/* Since the mode is quite configurable, the widths and heights set up here
** only matter for using the Uzebox kernel functions. So they can be pretty
** arbitrary (even larger than the actual display, to be accessed by
** scrolling) as long as Mode 74 can be set up to display them. */

#ifndef TILE_WIDTH
	#define TILE_WIDTH     8
	#define VRAM_TILES_H   24
	#define SCREEN_TILES_H 24
#elif   TILE_WIDTH == 6
	#define VRAM_TILES_H   32
	#define SCREEN_TILES_H 32
#elif   TILE_WIDTH == 8
	#define VRAM_TILES_H   24
	#define SCREEN_TILES_H 24
#else
	#error Invalid value defined in the makefile for TILE_WIDTH. Supported values are 6 or 8.
#endif

#ifndef TILE_HEIGHT
	#define TILE_HEIGHT 8
#elif   TILE_HEIGHT == 8
#else
	#error Invalid value defined in the makefile for TILE_HEIGHT. Supported value is 8.
#endif

#ifndef VRAM_TILES_V
	#define VRAM_TILES_V   (224 / TILE_HEIGHT)
#endif

#ifndef SCREEN_TILES_V
	#define SCREEN_TILES_V (224 / TILE_HEIGHT)
#endif

#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 20
#endif

#define VRAM_SIZE       (VRAM_TILES_H * VRAM_TILES_V)
#define VRAM_ADDR_SIZE  1
#define VRAM_PTR_TYPE   unsigned char

#define SPRITES_ENABLED 0

#ifndef FRAME_LINES
	#define FRAME_LINES (SCREEN_TILES_V * TILE_HEIGHT)
#endif

/* Maximum free cycles usable by the hysnc and audio */
#define HSYNC_USABLE_CYCLES 217


/* Notes: Don't use 'U' suffixes for the defines since they are used in
** assembler sources where the assembler doesn't understand them. */


/* Width of 2bpp mode output. At least 2 tiles. Note that multiple 2bpp chunks
** can occur within a row, so you can use more than one effective width by
** appropriately combining smaller blocks. */

#ifndef M74_2BPP_WIDTH
	#define M74_2BPP_WIDTH     16
#endif


/* Location of the palette buffer in RAM, high byte. The default location of
** 0x0F00 places it under the stack. */

#ifndef M74_PALBUF_H
	#define M74_PALBUF_H       0x0F
#endif


/* The Color 0 reload feature may be disabled to save a tiny bit of flash and
** two bytes of RAM. */

#ifndef M74_COL0_RELOAD
	#define M74_COL0_RELOAD    1
#endif


/* This can be used to disable separator line (Row mode 7) to save some flash
** space if this mode is not needed. */

#ifndef M74_M7_ENABLE
	#define M74_M7_ENABLE      1
#endif


/* This can be used to enable 2bpp Multicolor (Rom mode 3). This mode takes
** almost 3K of flash and also some bytes of RAM. */

#ifndef M74_M3_ENABLE
	#define M74_M3_ENABLE      0
#endif


/* Resource locations. At least one for each of the three resources must
** be provided. */

/* 0x00 - 0x7F: Mode dependent tiles for modes 1 - 6. 'OFF' is the offset,
** 'INC' is the increment per row in 4 byte units. */

#ifndef M74_TBANK01_0_OFF
	#define M74_TBANK01_0_OFF  0
#endif
#ifndef M74_TBANK01_0_INC
	#define M74_TBANK01_0_INC  32
#endif
#ifndef M74_TBANK01_1_OFF
	#define M74_TBANK01_1_OFF  M74_TBANK01_0_OFF
#endif
#ifndef M74_TBANK01_1_INC
	#define M74_TBANK01_1_INC  M74_TBANK01_0_INC
#endif
#ifndef M74_TBANK01_2_OFF
	#define M74_TBANK01_2_OFF  M74_TBANK01_0_OFF
#endif
#ifndef M74_TBANK01_2_INC
	#define M74_TBANK01_2_INC  M74_TBANK01_0_INC
#endif
#ifndef M74_TBANK01_3_OFF
	#define M74_TBANK01_3_OFF  M74_TBANK01_0_OFF
#endif
#ifndef M74_TBANK01_3_INC
	#define M74_TBANK01_3_INC  M74_TBANK01_0_INC
#endif
#define M74_TBANK01_0_H ((M74_TBANK01_0_OFF >> 8) & 0xFF)
#define M74_TBANK01_0_L ((M74_TBANK01_0_OFF     ) & 0xFF)
#define M74_TBANK01_1_H ((M74_TBANK01_1_OFF >> 8) & 0xFF)
#define M74_TBANK01_1_L ((M74_TBANK01_1_OFF     ) & 0xFF)
#define M74_TBANK01_2_H ((M74_TBANK01_2_OFF >> 8) & 0xFF)
#define M74_TBANK01_2_L ((M74_TBANK01_2_OFF     ) & 0xFF)
#define M74_TBANK01_3_H ((M74_TBANK01_3_OFF >> 8) & 0xFF)
#define M74_TBANK01_3_L ((M74_TBANK01_3_OFF     ) & 0xFF)

/* 0x00 - 0x7F: Mode dependent tiles for mode 0 (4Kb ROM tile maps). At least
** one must be defined. The low byte should be zero (it is ignored). */

#ifndef M74_TBANKM0_0_OFF
	#error At least one tile bank 0-1 offset needed (M74_TBANKM0_0_OFF)
#endif
#ifndef M74_TBANKM0_1_OFF
	#define M74_TBANKM0_1_OFF  M74_TBANKM0_0_OFF
#endif
#ifndef M74_TBANKM0_2_OFF
	#define M74_TBANKM0_2_OFF  M74_TBANKM0_0_OFF
#endif
#ifndef M74_TBANKM0_3_OFF
	#define M74_TBANKM0_3_OFF  M74_TBANKM0_0_OFF
#endif
#define M74_TBANKM0_0_H ((M74_TBANKM0_0_OFF >> 8) & 0xFF)
#define M74_TBANKM0_1_H ((M74_TBANKM0_1_OFF >> 8) & 0xFF)
#define M74_TBANKM0_2_H ((M74_TBANKM0_2_OFF >> 8) & 0xFF)
#define M74_TBANKM0_3_H ((M74_TBANKM0_3_OFF >> 8) & 0xFF)

/* 0x80 - 0xBF: 2Kb ROM tile maps (half 4Kb maps). At least one must be
** defined. The low byte should be zero (it is ignored). */

#ifndef M74_TBANK2_0_OFF
	#error At least one tile bank 2 offset needed (M74_TBANK2_0_OFF)
#endif
#ifndef M74_TBANK2_1_OFF
	#define M74_TBANK2_1_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_2_OFF
	#define M74_TBANK2_2_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_3_OFF
	#define M74_TBANK2_3_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_4_OFF
	#define M74_TBANK2_4_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_5_OFF
	#define M74_TBANK2_5_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_6_OFF
	#define M74_TBANK2_6_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_7_OFF
	#define M74_TBANK2_7_OFF  M74_TBANK2_0_OFF
#endif
#define M74_TBANK2_0_H ((M74_TBANK2_0_OFF >> 8) & 0xFF)
#define M74_TBANK2_1_H ((M74_TBANK2_1_OFF >> 8) & 0xFF)
#define M74_TBANK2_2_H ((M74_TBANK2_2_OFF >> 8) & 0xFF)
#define M74_TBANK2_3_H ((M74_TBANK2_3_OFF >> 8) & 0xFF)
#define M74_TBANK2_4_H ((M74_TBANK2_4_OFF >> 8) & 0xFF)
#define M74_TBANK2_5_H ((M74_TBANK2_5_OFF >> 8) & 0xFF)
#define M74_TBANK2_6_H ((M74_TBANK2_6_OFF >> 8) & 0xFF)
#define M74_TBANK2_7_H ((M74_TBANK2_7_OFF >> 8) & 0xFF)

/* 0xC0 - 0xFF: 4bpp RAM tile maps. At least one must be defined. 'OFF' is the
** offset, 'INC' is the increment per row in 4 byte units (64 is the maximum
** sensible value expect probably if combining the two RAM tile sections for
** some reason). */

#ifndef M74_TBANK3_0_OFF
	#error At least one tile bank 3 offset needed (M74_TBANK3_0_OFF)
#endif
#ifndef M74_TBANK3_0_INC
	#error At least one tile bank 3 increment needed (M74_TBANK3_0_INC)
#endif
#ifndef M74_TBANK3_1_OFF
	#define M74_TBANK3_1_OFF  M74_TBANK3_0_OFF
#endif
#ifndef M74_TBANK3_1_INC
	#define M74_TBANK3_1_INC  M74_TBANK3_0_INC
#endif
#define M74_TBANK3_0_H ((M74_TBANK3_0_OFF >> 8) & 0xFF)
#define M74_TBANK3_0_L ((M74_TBANK3_0_OFF     ) & 0xFF)
#define M74_TBANK3_1_H ((M74_TBANK3_1_OFF >> 8) & 0xFF)
#define M74_TBANK3_1_L ((M74_TBANK3_1_OFF     ) & 0xFF)

