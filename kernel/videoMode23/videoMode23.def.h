/*
 *  Uzebox(tm) Video Mode 23
 *  Copyright (C) 2009 Alec Bourque
 *  Lee Weber (D3thAdd3r) 2026
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
 * This file contains global defines for video mode 23 (for asm & C)
 *
 * ==============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE	"videoMode23/videoMode23.s"
#define VMODE_C_PROTOTYPES	"videoMode23/videoMode23.h"
#define VMODE_FUNC			sub_video_mode_23

#define VRAM_ADDR_SIZE		1
#define VRAM_PTR_TYPE		char

/*
 * For 2x vertical repeat:
 *	64 logical rows * 2 = 128 rendered scanlines
 *
 * This mode intentionally returns after 128 lines to give more CPU time back
 * to user code.
 */
#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 52
#endif

#define FRAME_LINES			128

#define VRAM_TILES_H 128
#define VRAM_TILES_V 64

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


/* Native mode geometry */
#define VM23_WIDTH			128
#define VM23_HEIGHT			64
#define VM23_STRIDE			16
#define VM23_BUFFER_SIZE	1024

#define VM23_LINE_REPEAT	2
