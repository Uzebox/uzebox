/*
 *  Uzebox Kernel - Mode 0 (Bootloader)
 *  Copyright (C) 2017 Sandor Zsuga (Jubatian)
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
 * This file contains global defines for video mode 0
 *
 * ===========================================================================
 */

#pragma once


#define VMODE_ASM_SOURCE    "videoMode0/videoMode0.s"
#define VMODE_C_SOURCE      "videoMode0/videoMode0.c"
#define VMODE_C_PROTOTYPES  "videoMode0/videoMode0.h"
#define VMODE_FUNC          sub_video_mode0


#define TILE_WIDTH          6
#define TILE_HEIGHT         8

#ifndef VRAM_TILES_H
#define VRAM_TILES_H        40
#endif
#ifndef VRAM_TILES_V
#define VRAM_TILES_V        (224 / TILE_HEIGHT)
#endif

#define SCREEN_TILES_H      VRAM_TILES_H
#define SCREEN_TILES_V      VRAM_TILES_V

#define VRAM_SIZE           (VRAM_TILES_H * VRAM_TILES_V)
#define VRAM_ADDR_SIZE      1
#define VRAM_PTR_TYPE       u8

#define SPRITES_ENABLED     0

#define FRAME_LINES         (SCREEN_TILES_V * TILE_HEIGHT)

#define FIRST_RENDER_LINE   (20 + ((224 - FRAME_LINES) / 2))

#define HSYNC_USABLE_CYCLES 240


/*
** Count of tiles used. One tile takes 8 bytes of RAM.
*/
#ifndef TILESET_SIZE
#define TILESET_SIZE        256
#endif
