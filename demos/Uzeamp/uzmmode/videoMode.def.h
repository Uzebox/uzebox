/*
 *  Uzeamp's video mode (uzmmode): mode 5's text frames, and video frames streamed from the SD
 *  card (uzm.def.h). Built as a custom mode: -DVIDEO_MODE=0 -DVIDEO_MODE_PATH=<this folder>.
 *
 *  Text frames are mode 5's (kernel/videoMode5), with the same settings. Video frames draw
 *  UZM_LINES lines from UZM_FIRST_LINE: SetRenderingParameters(UZM_FIRST_LINE, UZM_LINES)
 *  when uzm.on is set, and back to the text's when it is cleared.
 */
#pragma once

#define VMODE_ASM_SOURCE QUOTE(VIDEO_MODE_PATH/videoModeCore.s)
#define VMODE_C_SOURCE QUOTE(VIDEO_MODE_PATH/videoMode.c)
#define VMODE_C_PROTOTYPES QUOTE(VIDEO_MODE_PATH/videoMode.h)
#define VMODE_FUNC sub_video_uzm

#include QUOTE(VIDEO_MODE_PATH/uzm.def.h)

/* mode 5's settings, as kernel/videoMode5/videoMode5.def.h */
#define TILE_HEIGHT 8

#ifndef TILE_WIDTH
	#define TILE_WIDTH 6
#endif

#ifndef VRAM_TILES_H
	#if TILE_WIDTH == 6
		#define VRAM_TILES_H 40
	#else
		#define VRAM_TILES_H 30
	#endif
#endif

#ifndef VRAM_TILES_V
	#define VRAM_TILES_V 28
#endif
#ifndef SCREEN_TILES_H
	#define SCREEN_TILES_H VRAM_TILES_H
#endif
#ifndef SCREEN_TILES_V
	#define SCREEN_TILES_V VRAM_TILES_V
#endif
#ifndef FIRST_RENDER_LINE
	#define FIRST_RENDER_LINE 20
#endif

#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V
#define VRAM_ADDR_SIZE 1 //in bytes
#define VRAM_PTR_TYPE char
#define SPRITES_ENABLED 0

#ifndef FRAME_LINES
	#define FRAME_LINES SCREEN_TILES_V*TILE_HEIGHT
#endif

/* The video frames' sound goes straight into the vsync mixer's buffer (mix_buf) */
#if defined(SOUND_MIXER) && SOUND_MIXER != 0
	#error "uzmmode needs the vsync mixer (SOUND_MIXER=0, the default): its sound goes in mix_buf"
#endif
