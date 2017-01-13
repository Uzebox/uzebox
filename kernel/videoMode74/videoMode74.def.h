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
#endif
#if     (TILE_WIDTH == 6)
	#ifndef VRAM_TILES_H
		#define VRAM_TILES_H   32
	#endif
	#ifndef SCREEN_TILES_H
		#define SCREEN_TILES_H 32
	#endif
#elif   (TILE_WIDTH == 8)
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


#define HSYNC_USABLE_CYCLES 215



/* Notes: Don't use 'U' suffixes for the defines since they are used in
** assembler sources where the assembler doesn't understand them. */


/* Notes on the "_PTRE" defines:
** Several offsets can be either used as compile time definition of accessed
** by a pointer in RAM. The latter needs 2 RAM bytes each. If you need
** multiple configurations for one, you can enable it by setting the
** accompanying "_PTRE" definition to '1' for the cost of 2 RAM bytes. They
** will still be initialized with the corresponding compile time definition
** for convenience. */



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
** palette buffer and optionally other work areas to conserve memory.
** Note that the Uzebox logo feature can not be used if you place the palette
** buffer at 0x1000 - 0x10FF since the default stack top + 1 is 0x1100, so
** the palette buffer will destroy the stack in the logo code. */

#ifndef M74_RESET_ENABLE
	#define M74_RESET_ENABLE   0
#endif
#ifndef M74_MAIN_STACK
	#define M74_MAIN_STACK     0x1100
#endif



/* Tile descriptor tables. Each uses up to 130 bytes in ROM or RAM
** respectively. To have Mode 74 operational, you have to define at least one
** proper tile descriptor! The Uzebox logo feature expects M74_RAMTD_OFF
** defined, using its first 5 bytes, which it restores after use. */

#ifndef M74_ROMTD_OFF
	#define M74_ROMTD_OFF      0
#endif
#ifndef M74_RAMTD_OFF
	#define M74_RAMTD_OFF      0x1030
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



/* Location of intro logo workspace. Only used during the intro logo's display
** if it is enabled, after that the related memory can be reused. Normally the
** default location set here should be fine. A region for the logo RAM tiles
** also has to be set up (it uses 18 RAM tiles, so 576 bytes). */

#ifndef M74_LOGO_WORK
	#define M74_LOGO_WORK      0x1040
#endif
#ifndef M74_LOGO_RAMTILES
	#define M74_LOGO_RAMTILES  0x0700
#endif



/* Location of the 16 color palette. By default place it below the stack
** (0x1000 - 0x100F) */

#ifndef M74_PAL_PTRE
	#define M74_PAL_PTRE       0
#endif
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
#else
	#if ((M74_COL0_OFF != 0) && (M74_COL0_DISABLE != 0))
		#error "Color 0 is disabled (M74_COL0_DISABLE set), relaoding (M74_COL0_OFF nonzero) can not be used!"
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



/* This can be used to enable separator line (Row mode 2). It uses some dozen
** bytes of Flash. */

#ifndef M74_M2_ENABLE
	#define M74_M2_ENABLE      0
#endif


/* This can be used to enable 2bpp Multicolor (Row mode 3). This mode takes
** almost 3K of flash and also some bytes of RAM. */

#ifndef M74_M3_ENABLE
	#define M74_M3_ENABLE      0
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
** M74_Finish() will take more time. */

#ifndef M74_SD_ENABLE
	#define M74_SD_ENABLE      0
#endif



/* The VRAM used for the kernel and sprite output. This is a rectangular
** region normally mapping to the VRAM set up for Mode 74 output (m74_tidx),
** so sprites and kernel stuff appears proper. */

#ifndef M74_VRAM_OFF
	#error "A base address for kernel and sprite VRAM surface is necessary!"
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

/* RAM mask index base. This is always based at whatever RAM tile offset base
** is set up (there is no practical use for supporting more than 64 RAM tiles
** here since it can not be exploited). So it has up to 64 elements, the first
** belonging to RAM tile 0 (Tile index 0xC0). If RAM tiles are only used for
** rendering sprites or the area is repopulated after every frame, this may be
** located under the palette buffer or video stack. */

#ifndef M74_RAMMASKIDX_OFF
	#if ((M74_SPR_ENABLE != 0) && (M74_MSK_ENABLE != 0))
		#error "A RAM mask index base (M74_RAMMASKIDX_OFF) has to be defined for the sprite system!"
	#endif
#endif



/* ROM mask pool's address. At most 224 x 8 bytes (depends on used masks).
** If no actual masks are used, or only RAM masks are used, this may be left
** zero. */

#ifndef M74_ROMMASK_OFF
	#define M74_ROMMASK_OFF    0
#endif

/* RAM mask pool's address. At most 14 x 8 bytes (depends on used masks).
** If no actual masks are used, or only ROM masks are used, this may be left
** zero. */

#ifndef M74_RAMMASK_OFF
	#define M74_RAMMASK_OFF    0
#endif



/* Location of X row shifts. It can be disabled by leaving it zero. Enabling
** it adds row shifts (to the left) of 0 - 7 pixels which should follow how
** the display is generated. This somewhat eases scrolling, and enables
** parallax scrolling. Up to 32 bytes, depending on how many rows the VRAM
** has. If the content is re-generated before the render of every frame, it
** may be located under the palette buffer or video stack. */

#ifndef M74_XSHIFT_OFF
	#define M74_XSHIFT_OFF     0
#endif



/* RAM tile allocation workspace pointer for sprites. Up to 192 bytes (depends
** on maximal number of RAM tiles used, 3 bytes for a RAM tile). It may be
** located below the palette buffer if the VRAM is rebuilt before new renders
** completely (this case M74_ResReset has to be called before starting the
** render). */

#ifndef M74_RTLIST_PTRE
	#define M74_RTLIST_PTRE    0
#endif
#ifndef M74_RTLIST_OFF
	#if ((M74_SPR_ENABLE != 0) && (M74_RTLIST_PTRE == 0))
		#error "A RAM tile list offset (M74_RTLIST_OFF) has to be defined for the sprite system!"
	#endif
#endif



/* Sprite recolor table set start offset. Only the high byte is used. If it is
** set zero, then the recoloring support is not compiled in (saving some flash
** space and making sprite blitting even without recoloring a bit faster). One
** recolor table is 256 bytes, containing remap values for input bytes (pixel
** pairs). */

#ifndef M74_RECTB_OFF
	#define M74_RECTB_OFF      0
#endif

/* Enable slow recoloring. It makes sprite blitting about 15 percents slower
** while offering a 256 byte ROM area of 16 byte (or less) recolor tables (one
** byte for one index). This allows for heavier sprite recolor use. Note that
** if enabled, recolor can not be turned off at all. The first 16 bytes should
** be filled up for straight recolor (values 0 - 15), so M74_BlitSprite works
** correctly. The recolor table index is a simple offset within the 256 byte
** (wrapping) table, allowing to pack recolor maps tight. */

#ifndef M74_REC_SLOW
	#define M74_REC_SLOW       0
#else
	#if ((M74_REC_SLOW != 0) && (M74_RECTB_OFF == 0x0000))
		#error "A recolor table (M74_RECTB_OFF) is necessary to use slow recoloring (M74_REC_SLOW)"
	#endif
#endif
