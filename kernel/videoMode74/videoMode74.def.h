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
	#error "Invalid value defined in the makefile for TILE_WIDTH. Supported values are 6 or 8."
#endif

#ifndef TILE_HEIGHT
	#define TILE_HEIGHT 8
#elif   TILE_HEIGHT == 8
#else
	#error "Invalid value defined in the makefile for TILE_HEIGHT. Supported value is 8."
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

/* Notes: Don't use 'U' suffixes for the defines since they are used in
** assembler sources where the assembler doesn't understand them. */


/* Notes on the "_PTRE" defines:
** Several offsets can be either used as compile time definition of accessed
** by a pointer in RAM. The latter needs 2 RAM bytes each. If you need
** multiple configurations for one, you can enable it by setting the
** accompanying "_PTRE" definition to '1' for the cost of 2 RAM bytes. They
** will still be initialized with the corresponding compile time definition
** for convenience. */


/* Reset on every frame. If this is enabled, then a video frame will reset
** onto a provided address (can be a C function with void parameters and
** return), saving a lot of memory (240 bytes) from stack, using the palette
** buffer for main program stack. However this also means that the main
** program might not complete (ideally onto a "do-nothing" empty loop) before
** the VBlank is over. You need to set m74_reset to an appropriate address
** before enabling display (if display is disabled, the reset mechanism is
** inactive). After enabling display from the initializing code, an empty
** loop should be provided so the video frame will reset onto the provided
** code. Enabling changes a few defaults to make space for the stack assuming
** default configuration (if you change stack locations, you need to also set
** up those proper).
**
** The stacks: The given addresses point above the top of the stacks (so at
** the first byte unused by stack). For Video, 16 bytes of stack is necessary,
** which space may be used for temporaries (or extra stack) for the main
** program. For the main program, as many bytes as needed. The main program's
** stack in a resetting configuration should be shared with the palette
** buffer to conserve memory! (Providing 256 bytes for the main program).
** Note that the Uzebox logo feature can not be used if you place the palette
** buffer at 0x1000 - 0x10FF since the default stack top + 1 is 0x1100, so
** the palette buffer will destroy the stack in the logo code. */

#ifndef M74_RESET_ENABLE
	#define M74_RESET_ENABLE   0
#endif
#ifndef M74_VIDEO_STACK
	#define M74_VIDEO_STACK    0x1010
#endif
#ifndef M74_MAIN_STACK
	#define M74_MAIN_STACK     0x1010
#endif


/* Disable color zero. Color zero can not be used for sprites (it is the
** transparent color). If you don't need it (using only up to 15 colors), you
** can disable it by this option. This causes omitting initializing bytes 0 -
** 16 of the palette buffer, which may be used for other purposes then (for
** example the video stack). Color zero should not occur in the image data
** then (it will cause pixels of undefined colors for this index). Note that
** it also disables 2bpp mode (which uses color indices 0 - 3). You may use
** the Multicolor mode (Row mode 3) instead with a repeated single tile row
** to conserve RAM memory if you need simple 2bpp. */

#ifndef M74_COL0_DISABLE
	#define M74_COL0_DISABLE   0
#endif


/* Width of 2bpp mode output. At least 2 tiles. Note that multiple 2bpp chunks
** can occur within a row, so you can use more than one effective width by
** appropriately combining smaller blocks. */

#ifndef M74_2BPP_WIDTH
	#define M74_2BPP_WIDTH     16
#endif


/* Location of the palette buffer in RAM, high byte. The default location of
** 0x0F00 places it below the stack (or shared with main stack in reset on
** every frame configuration). */

#ifndef M74_PALBUF_H
	#define M74_PALBUF_H       0x0F
#endif


/* Location of intro logo workspace. Only used during the intro logo's display
** if it is enabled, after that the related memory can be reused. Normally the
** default location set here should be fine. Needs 73 bytes. */

#ifndef M74_LOGO_WORK
	#if (M74_RESET_ENABLE == 0)
		#define M74_LOGO_WORK      0x1010
	#else
		#define M74_LOGO_WORK      (M74_VIDEO_STACK + 0x10)
	#endif
#endif


/* Location of the 16 color palette. By default place it below the stack
** (0x1000 - 0x100F) */

#ifndef M74_PAL_PTRE
	#define M74_PAL_PTRE       0
#endif
#ifndef M74_PAL_OFF
	#if (M74_RESET_ENABLE == 0)
		#define M74_PAL_OFF        0x1000
	#else
		#define M74_PAL_OFF        (M74_VIDEO_STACK + 0x00)
	#endif
#endif


/* The Color 0 reload feature may be disabled to save a tiny bit of flash and
** two bytes of RAM. You may also limit it to a single offset. */

#ifndef M74_COL0_RELOAD
	#define M74_COL0_RELOAD    0
#else
	#if ((M74_COL0_RELOAD != 0) && (M74_COL0_DISABLE != 0))
		#error "Color 0 is disabled (M74_COL0_DISABLE set), relaoding (M74_COL0_RELOAD) can not be used!"
	#endif
#endif
#ifndef M74_COL0_PTRE
	#define M74_COL0_PTRE      0
#endif
#ifndef M74_COL0_OFF
	#if ((M74_COL0_RELOAD != 0) && (M74_COL0_PTRE == 0))
		#error "To use Color 0 reload without pointer, a valid address is needed! (M74_COL0_OFF)"
	#else
		#define M74_COL0_OFF       0
	#endif
#endif


/* This can be used to constrain row selector address to a compile time
** offset, saving 2 bytes of RAM. */

#ifndef M74_ROWS_PTRE
	#define M74_ROWS_PTRE      1
#endif
#ifndef M74_ROWS_OFF
	#if (M74_ROWS_PTRE == 0)
		#error "Row selector pointer disabled, but no address is given! (M74_ROWS_OFF)"
	#else
		#define M74_ROWS_OFF       0
	#endif
#endif


/* This can be used to disable separator line (Row mode 7) to save some flash
** space if this mode is not needed. */

#ifndef M74_M7_ENABLE
	#define M74_M7_ENABLE      1
#endif


/* This can be used to enable 2bpp Multicolor (Row mode 3). This mode takes
** almost 3K of flash and also some bytes of RAM. */

#ifndef M74_M3_ENABLE
	#define M74_M3_ENABLE      0
#endif
#ifndef M74_M3_PTRE
	#define M74_M3_PTRE        0
#endif
#ifndef M74_M3_OFF
	#if ((M74_M3_ENABLE != 0) && (M74_M3_PTRE == 0))
		#error "To use Row Mode 3 without pointer, a valid address is needed! (M74_M3_OFF)"
	#else
		#define M74_M3_OFF         0
	#endif
#endif


/* Setting this subtract value enables double scanning for row mode 3 by
** applying it in odd rows if the corresponding m74_config bit is set. It
** encodes the bytes to subtract from the pointer in such case (one multicolor
** tile takes 2 bytes). The first line of double-scanned content can also be
** set, odd lines are relative to this (so the given line becomes the first
** doubly scanned line by an appropriately placed first pointer subtract). */

#ifndef M74_M3_SUB
	#define M74_M3_SUB         0
#endif
#ifndef M74_M3_SUBSL
	#define M74_M3_SUBSL       0
#endif


/* Enable SD load function. If enabled it becomes possible to load an
** arbitrary 2 byte aligned part of an SD card sector in every frame. It costs
** some flash and RAM. At up to 22 tiles width it works perfectly, at 24 tiles
** width it might not be able to finish the load during display, so
** M74_Finish() will take more time. Using M74_SD_EXT (adds an SD load at the
** cost of audio cycles to every line) along with some non-scrolling sections
** and-or separator lines may allow it reliably finishing. */

#ifndef M74_SD_ENABLE
	#define M74_SD_ENABLE      0
#endif
#if     (M74_SD_ENABLE != 0)
	#ifndef M74_SD_EXT
		#define M74_SD_EXT         0
	#endif
	#if     (M74_SD_EXT == 0)
		#define HSYNC_USABLE_CYCLES 223
	#else
		#define HSYNC_USABLE_CYCLES 196
	#endif
#else
	#define HSYNC_USABLE_CYCLES 223
#endif



/* The VRAM used for the kernel and sprite output. Setting it compile time
** saves 5 bytes of RAM and some flash space. */

#ifndef M74_VRAM_CONST
	#define M74_VRAM_CONST     0
#elif   (M74_VRAM_CONST != 0)
	#ifndef M74_VRAM_OFF
		#error "A base address for VRAM is necessary if M74_VRAM_CONST is used!"
	#endif
	#ifndef M74_VRAM_W
		#define M74_VRAM_W VRAM_TILES_H
	#endif
	#ifndef M74_VRAM_H
		#define M74_VRAM_H VRAM_TILES_V
	#endif
	#ifndef M74_VRAM_P
		#define M74_VRAM_P M74_VRAM_W
	#endif
#endif


/* Resource locations. At least one for each of the three 4bpp ranges
** (0x00 - 0x7F, 0x80 - 0xBF, 0xC0 - 0xFF) must be provided. */

/* 0x00 - 0x7F: Mode dependent tiles for modes 1, 2, 4, 5 and 6. 'OFF' is the
** offset, 'INC' is the increment per row in byte units (or tiles for the 1bpp
** modes). Not required (maybe not using any of these modes). An increment
** value of 0 is interpreted as 256, this is required if you need a 1bpp
** tileset shared between Row mode 3 (Multicolor) and these modes. */

#ifndef M74_TBANK01_0_OFF
	#define M74_TBANK01_0_OFF  0
#endif
#ifndef M74_TBANK01_0_INC
	#define M74_TBANK01_0_INC  128
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

/* 0x00 - 0x7F: Mode dependent tiles for mode 0 (4Kb ROM tile maps). At least
** one must be defined. The low byte should be zero (it is ignored). The MSK
** definitions may be omitted if there are no masks (used for sprites), they
** are ROM offsets for mask indices (1 byte each). */

#ifndef M74_TBANKM0_0_OFF
	#error "At least one tile bank 0-1 offset needed (M74_TBANKM0_0_OFF)"
#endif
#ifndef M74_TBANKM0_0_MSK
	#define M74_TBANKM0_0_MSK  0
#endif
#ifndef M74_TBANKM0_1_OFF
	#define M74_TBANKM0_1_OFF  M74_TBANKM0_0_OFF
#endif
#ifndef M74_TBANKM0_1_MSK
	#define M74_TBANKM0_1_MSK  M74_TBANKM0_0_MSK
#endif
#ifndef M74_TBANKM0_2_OFF
	#define M74_TBANKM0_2_OFF  M74_TBANKM0_0_OFF
#endif
#ifndef M74_TBANKM0_2_MSK
	#define M74_TBANKM0_2_MSK  M74_TBANKM0_0_MSK
#endif
#ifndef M74_TBANKM0_3_OFF
	#define M74_TBANKM0_3_OFF  M74_TBANKM0_0_OFF
#endif
#ifndef M74_TBANKM0_3_MSK
	#define M74_TBANKM0_3_MSK  M74_TBANKM0_0_MSK
#endif

/* 0x80 - 0xBF: 2Kb ROM tile maps (half 4Kb maps). At least one must be
** defined. The low byte should be zero (it is ignored). The MSK definitions
** may be omitted if there are no masks (used for sprites), they are ROM
** offsets for mask indices (1 byte each). */

#ifndef M74_TBANK2_0_OFF
	#error "At least one tile bank 2 offset needed (M74_TBANK2_0_OFF)"
#endif
#ifndef M74_TBANK2_0_MSK
	#define M74_TBANK2_0_MSK  0
#endif
#ifndef M74_TBANK2_1_OFF
	#define M74_TBANK2_1_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_1_MSK
	#define M74_TBANK2_1_MSK  M74_TBANK2_0_MSK
#endif
#ifndef M74_TBANK2_2_OFF
	#define M74_TBANK2_2_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_2_MSK
	#define M74_TBANK2_2_MSK  M74_TBANK2_0_MSK
#endif
#ifndef M74_TBANK2_3_OFF
	#define M74_TBANK2_3_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_3_MSK
	#define M74_TBANK2_3_MSK  M74_TBANK2_0_MSK
#endif
#ifndef M74_TBANK2_4_OFF
	#define M74_TBANK2_4_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_4_MSK
	#define M74_TBANK2_4_MSK  M74_TBANK2_0_MSK
#endif
#ifndef M74_TBANK2_5_OFF
	#define M74_TBANK2_5_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_5_MSK
	#define M74_TBANK2_5_MSK  M74_TBANK2_0_MSK
#endif
#ifndef M74_TBANK2_6_OFF
	#define M74_TBANK2_6_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_6_MSK
	#define M74_TBANK2_6_MSK  M74_TBANK2_0_MSK
#endif
#ifndef M74_TBANK2_7_OFF
	#define M74_TBANK2_7_OFF  M74_TBANK2_0_OFF
#endif
#ifndef M74_TBANK2_7_MSK
	#define M74_TBANK2_7_MSK  M74_TBANK2_0_MSK
#endif

/* 0xC0 - 0xFF: 4bpp RAM tile maps. At least one must be defined. 'OFF' is the
** offset, 'INC' is the increment per row in 4 byte units (64 is the maximum
** sensible value expect probably if combining the two RAM tile sections for
** some reason). The MSK definitions may be omitted if there are no masks
** (used for sprites), they are RAM offsets for mask indices (1 byte each). */

#ifndef M74_TBANK3_0_OFF
	#error "At least one tile bank 3 offset needed (M74_TBANK3_0_OFF)"
#endif
#ifndef M74_TBANK3_0_INC
	#error "At least one tile bank 3 increment needed (M74_TBANK3_0_INC)"
#endif
#ifndef M74_TBANK3_0_MSK
	#define M74_TBANK3_0_MSK  0
#endif
#ifndef M74_TBANK3_1_OFF
	#define M74_TBANK3_1_OFF  M74_TBANK3_0_OFF
#endif
#ifndef M74_TBANK3_1_INC
	#define M74_TBANK3_1_INC  M74_TBANK3_0_INC
#endif
#ifndef M74_TBANK3_1_MSK
	#define M74_TBANK3_1_MSK  M74_TBANK3_0_MSK
#endif

/* ROM mask pool's address. At most 224 x 8 bytes (depends on used masks). */

#ifndef M74_ROMMASK_PTRE
	#define M74_ROMMASK_PTRE  0
#endif
#ifndef M74_ROMMASK_OFF
	#define M74_ROMMASK_OFF   0
#endif

/* RAM mask pool's address. At most 14 x 8 bytes (depends on used masks). */

#ifndef M74_RAMMASK_PTRE
	#define M74_RAMMASK_PTRE  0
#endif
#ifndef M74_RAMMASK_OFF
	#define M74_RAMMASK_OFF   0
#endif

/* RAM tile allocation workspace pointer for sprites. Up to 192 bytes (depends
** on maximal number of RAM tiles used, 3 bytes for a RAM tile). By default it
** is dropped below the stack which may be suitable if not too many RAM tiles
** are used, and the program doesn't use much of stack. */

#ifndef M74_RTLIST_PTRE
	#define M74_RTLIST_PTRE   0
#endif
#ifndef M74_RTLIST_OFF
	#if (M74_RESET_ENABLE == 0)
		#define M74_RTLIST_OFF    0x1010
	#else
		#define M74_RTLIST_OFF    (M74_VIDEO_STACK + 0x10)
	#endif
#endif

/* Sprite recolor table set start offset. Only the high byte is used. If it is
** set zero, then the recoloring support is not compiled in (saving some flash
** space and making sprite blitting even without recoloring a bit faster). One
** recolor table is 256 bytes, containing remap values for input bytes (pixel
** pairs). */

#ifndef M74_RECTB_OFF
	#define M74_RECTB_OFF     0x0000
#endif

/* Enable slow recoloring. It makes sprite blitting about 15 percents slower
** while offering a 256 byte ROM area of 16 byte (or less) recolor tables (one
** byte for one index). This allows for heavier sprite recolor use. Note that
** if enabled, recolor can not be turned off at all. The first 16 bytes should
** be filled up for straight recolor (values 0 - 15), so M74_BlitSprite works
** correctly. The recolor table index is a simple offset within the 256 byte
** (wrapping) table, allowing to pack recolor maps tight. */

#ifndef M74_REC_SLOW
	#define M74_REC_SLOW      0
#else
	#if ((M74_REC_SLOW != 0) && (M74_RECTB_OFF == 0x0000))
		#error "A recolor table (M74_RECTB_OFF) is necessary to use slow recoloring (M74_REC_SLOW)"
	#endif
#endif
