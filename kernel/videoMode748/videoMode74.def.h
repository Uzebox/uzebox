/*
 *  Uzebox Kernel - Video Mode 748
 *  Copyright (C) 2018 Sandor Zsuga (Jubatian)
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
 * This file contains global defines for video mode 748
 *
 * ===========================================================================
 */

#pragma once

#define VMODE_ASM_SOURCE "videoMode748/videoMode74.s"
#define VMODE_C_SOURCE "videoMode748/videoMode74.c"
#define VMODE_C_PROTOTYPES "videoMode748/videoMode74.h"
#define VMODE_FUNC sub_video_mode74


/* Since the mode is quite configurable, the widths and heights set up here
** only matter for using the Uzebox kernel functions. So they can be pretty
** arbitrary (even larger than the actual display, to be accessed by
** scrolling) as long as Mode 74 can be set up to display them. */

#ifndef TILE_WIDTH
	#define TILE_WIDTH     8
#endif
#if     (TILE_WIDTH == 8)
	#ifndef VRAM_TILES_H
		#define VRAM_TILES_H   24
	#endif
	#ifndef SCREEN_TILES_H
		#define SCREEN_TILES_H 24
	#endif
#else
	#error "Invalid value for TILE_WIDTH. Supported values are 6 or 8."
#endif

#ifndef TILE_HEIGHT
	#define TILE_HEIGHT    8
#endif
#if     (TILE_HEIGHT != 8)
	#error "Invalid value for TILE_HEIGHT. Supported value is 8."
#endif

#ifndef VRAM_TILES_V
	#define VRAM_TILES_V   (224 / TILE_HEIGHT)
#endif

#ifndef SCREEN_TILES_V
	#define SCREEN_TILES_V (224 / TILE_HEIGHT)
#endif

#ifndef FRAME_LINES
	#define FRAME_LINES (SCREEN_TILES_V * TILE_HEIGHT)
#endif

#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 20 + ((224 - FRAME_LINES) / 2)
#endif

#define VRAM_SIZE       (VRAM_TILES_H * VRAM_TILES_V)
#define VRAM_ADDR_SIZE  1
#define VRAM_PTR_TYPE   unsigned char

#ifndef SPRITES_ENABLED
	#define SPRITES_ENABLED 0
#endif


#define HSYNC_USABLE_CYCLES 234



/* Notes: Don't use 'U' suffixes for the defines since they are used in
** assembler sources where the assembler doesn't understand them. */



/* Video temporary workspaces.
** These are only used during the scanline render, so if you are using the
** reset feature, you can locate them at such locations where the main code
** may reuse them. */

/* The render stack. Needs 16 bytes. Defines the bottom of the stack. */

#ifndef M74_VIDEO_STACK
	#define M74_VIDEO_STACK    0x1010
#endif

/* The palette buffer. The low byte of the address is ignored (256 byte
** boundary). Needs 256 bytes. Disabling color 0 makes the first 16 bytes
** available for other purposes. */

#ifndef M74_PALBUF
	#define M74_PALBUF         0x0F00
#endif



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
** The main program stack: The definition gives the top of the stack (so first
** used stack position is the definition - 1). It should be shared with the
** palette buffer and optionally other work areas to converse memory.
** Note that the Uzebox logo feature can not be used if you place the palette
** buffer at 0x1000 - 0x10FF since the default stack top + 1 is 0x1100, so
** the palette buffer will destroy the stack in the logo code. */

#ifndef M74_RESET_ENABLE
	#define M74_RESET_ENABLE   0
#endif
#ifndef M74_MAIN_STACK
	#define M74_MAIN_STACK     0x1100
#endif



/* Location of the 16 color palette. By default place it below the stack
** (0x1000 - 0x100F), and m74_paddr is pointed here. */

#ifndef M74_PAL_OFF
	#define M74_PAL_OFF        0x1000
#endif



/* Color 0 reloading on every scanline from RAM can be enabled by defining a
** 256 byte RAM bank for it (so low bytes of the offset are ignored).
** Reloading is performed by the logical row counter, so potentially all 256
** bytes may be accessed. The actual use of reloading can be controlled by a
** global configuration flag. */

#ifndef M74_COL0_OFF
	#define M74_COL0_OFF       0
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



/* Row mode 4 (SPI RAM 4bpp) needs a 96 byte line buffer, it can be allocated
** here. By default it is placed below the stack. */

#ifndef M74_M4_BUFFER
	#define M74_M4_BUFFER      0x1020
#endif

/* Row mode 4 (SPI RAM 4bpp) streams (it doesn't set up a start address), the
** beginning address for this can be set here. It uses 672 bytes of SPI RAM
** for every tile row. */

#ifndef M74_M4_BASE
	#define M74_M4_BASE        0
#endif



/* Enable Row Mode 6 & Row Mode 7. Enable them only if you are using them,
** they cost about 3K of ROM space. */

#ifndef M74_M67_ENABLE
	#define M74_M67_ENABLE     0
#endif

/* Font table address for Row Mode 6 & 7, for the SetTile or SetFont routines.
** This is an SPI RAM address (17 bits), where the font data has to be loaded.
** The font has the same format like a mask set (8 bytes / character). Note
** that using SetTile you may have more than 256 characters. The default sets
** this area up at the end of the SPI RAM. */

#ifndef M74_M67_FONT_OFF
	#define M74_M67_FONT_OFF   0x1F800
#endif



/* The followings belong to the sprite system. The sprite system should only
** be enabled if it is actually used (takes a considerable amount of flash),
** then the necessary components have to be defined. */

#ifndef M74_SPR_ENABLE
	#define M74_SPR_ENABLE     SPRITES_ENABLED
#endif



/* Enable the masking system. Masks allow sprites to show behind tiles, using
** 1bpp data. Masking costs 1 extra byte for each tile for holding indices
** into a mask table. It also takes some flash and extra CPU time. */

#ifndef M74_MSK_ENABLE
	#define M74_MSK_ENABLE     0
#endif

/* ROM mask index base. The ROM has 2048 tile slots, this list may have at
** most that many bytes. It doesn't need to be aligned to any boundary.
** Typically tile assets are packed together in the ROM taking one
** continuous region: this base offset may be forged so the first used
** location belongs to the first sprite. Example: If tiles begin at 0x8000,
** for 256 tiles, and you want to have the mask indices for them at 0xA000,
** 256 bytes, the base should be set up as (0xA000 - (0x8000 / 32)). */

#ifndef M74_ROMMASKIDX_OFF
	#if ((M74_SPR_ENABLE != 0) && (M74_MSK_ENABLE != 0))
		#error "A ROM mask index base (M74_ROMMASKIDX_OFF) has to be defined for the sprite system!"
	#endif
#endif

/* RAM mask index base. 1 byte is taken for each RAM tile available for the
** sprite engine. This is used to make it unnecessary to always look up mask
** indices by the background VRAM (in SPI RAM). It can be located under the
** palette buffer (as long as renders finish in one VBlank; as it is only
** needed for sprite rendering). */

#ifndef M74_RAMMASKIDX_OFF
	#define M74_RAMMASKIDX_OFF 0x1020
#endif

/* ROM mask pool's address. At most 256 x 8 bytes (depends on used masks).
** If no actual masks are used, this may be left zero. This has to be aligned
** at a 8 byte boundary. */

#ifndef M74_ROMMASK_OFF
	#define M74_ROMMASK_OFF    0
#endif



/* Sprite recolor table set start offset. Only the high byte is used. This is
** a 256 byte ROM area of 16 byte (or less) recolor tables (one byte for one
** index). This allows for heavier sprite recolor use. The first 16 bytes
** should be filled up for straight recolor (values 0 - 15), so M74_BlitSprite
** works correctly. The recolor table index is a simple offset within the 256
** byte (wrapping) table, allowing to pack recolor maps tight. */

#ifndef M74_RECTB_OFF
	#if (M74_SPR_ENABLE != 0)
		#error "A recolor table (M74_RECTB_OFF) has to be defined for the sprite system!"
	#endif
#endif
