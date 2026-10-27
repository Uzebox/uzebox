/*
 *  Uzeamp video (UZM): definitions shared by the C code and the renderer.
 *  See ../video-study.md for the design, ../tools/uzm.py for the file format.
 *
 *  The renderer draws 114 rows of 160 pixels, each row on 2 lines (228 lines), a byte a
 *  pixel (BBGGGRRR) straight to the DAC, 9 cycles a pixel. While it draws, it reads the card:
 *  a multiple-block read (CMD18) is open all the time, and every 18 cycles one byte is
 *  clocked in and stored in a 2 KB ring at 0x0800-0x0FFF. A byte moves the ring's write
 *  pointer on only if it is below 0xFE: the file holds no 0xFE/0xFF and each sector's CRC is
 *  made of them, so the card's CRCs, waits and tokens never reach the ring.
 *
 *  A picture is 38 sectors: 3 rows (480 bytes) and a 32-byte trailer each. The trailer
 *  carries the sound and the picture's number. The ring holds 4 sectors and starts on one,
 *  so a row never crosses the ring's end.
 *
 *  Version 2 of the format (../plan-rates.md): a picture is shown 1-3 times (the player
 *  restarts the read for a repeat), each showing with its own 8 samples a sector, on sectors
 *  2-34 (sector 34's start 2 before). The full frame draws the 114 rows from line 20; the cut
 *  one, for a card too slow to restart in the full frame's blank, rows 6-107 (sectors 2-35)
 *  from line 32, so the blank is 24 lines longer and the picture stays where it was.
 */
#pragma once

#define UZM_RING        0x0800          /* the ring, 0x0800-0x0FFF: its wrap is andi/ori */
#define UZM_RING_SIZE   2048
#define UZM_WIDTH       160
#define UZM_ROWS        114
#define UZM_LINES       228             /* UZM_ROWS lines, twice each */
#define UZM_FIRST_LINE  20              /* the first line drawn (SetRenderingParameters) */
#define UZM_SECTORS     38              /* sectors a picture */
#define UZM_SAMPLES     262             /* sound samples a showing (a frame): one a line */
#define UZM_TRAILER     32              /* bytes after a sector's 3 rows */
#define UZM_T_SAMPLES   1               /* showing k's 8 samples at 1 + 8k (k = 0..2) */
#define UZM_T_SET       8
#define UZM_T_MARK      25              /* the picture's number (3 x 7 bits), the sector */
#define UZM_SHOWINGS    3               /* a picture is shown at most this many times */

/* The full frame and the cut one: lines, first line, first sector */
#define UZM_CUT_ROWS    6               /* rows hidden at the top and at the bottom */
#define UZM_CUT_LINES   (UZM_LINES - 4 * UZM_CUT_ROWS)          /* 204 */
#define UZM_CUT_FIRST   (UZM_FIRST_LINE + 2 * UZM_CUT_ROWS)     /* 32 */
#define UZM_CUT_SECTOR  (UZM_CUT_ROWS / 3)                      /* 2: the first drawn */
#define UZM_CUT_DROP    (4 * 512)       /* the hidden sectors between two pictures: the
                                        ** ring's size, so the drop is a count */

/* The renderer stops clocking the card when this many unread bytes are in the ring: a line
** can store about 100 more. The vblank fill stops at UZM_FILL_MAX. */
#define UZM_STALL       1920
#define UZM_FILL_MAX    1984

/* struct uzm_t, field offsets (the asm uses them) */
#define UZM_O_X         0               /* u16: the next byte to show */
#define UZM_O_Y         2               /* u16: where the next byte from the card goes */
#define UZM_O_PC        4               /* u16: bytes stored, wrapping */
#define UZM_O_CC        6               /* u16: bytes shown (consumed), wrapping */
#define UZM_O_AW        8               /* u16: where the picture's sound goes (a bank of mix_buf) */
#define UZM_O_ON        10              /* u8: 1: video frames; 0: mode 5's text frames */
#define UZM_O_PROD      11              /* u8: 0xFF: the card is read; 0: it is never clocked */
#define UZM_O_UNDER     12              /* u16: lines that found their row missing */
#define UZM_O_MARK      14              /* 4 bytes: the last trailer's mark */
#define UZM_O_FRAMES    18              /* u16: video frames drawn */
#define UZM_O_TOFF      20              /* u8: the showing's samples, from a row's start
                                        ** (161 + 8k) */
#define UZM_O_KLO       21              /* u8: lines left (r25) on sector 34's last line */
#define UZM_O_KHI       22              /* u8: lines left on sector 2's last line */
#define UZM_O_LINES     23              /* u8: the frame's lines: 228, or 204 cut */
#define UZM_O_FIRST     24              /* u8: its first line: 20, or 32 cut */
#define UZM_O_CUT       25              /* u8: 1: the cut frame */
#define UZM_SIZE        26

/* The song stream (SOUND_STREAM builds: ../plan-stream.md, gen_wst.py, wst.h). Where the next
** data byte goes is the kernel's sound_stream_w (the ring's read stops there). struct wst_t,
** field offsets (the asm uses them) */
#define WST_O_K         0               /* u16: the sector's bytes left: 514..3 data, 2..1 its
                                        ** CRC, 0 waiting for the next sector's token */
#define WST_O_OFF       2               /* u8: 0: the stream is read; 0xFF: nothing is clocked
                                        ** (the program stopped it, or an error token did) */
#define WST_O_ON        3               /* u8: 1: the text frames are the stream's */
#define WST_O_STORED    4               /* u16: data bytes stored, wrapping */
#define WST_O_FRAMES    6               /* u16: the stream's text frames, wrapping */
#define WST_O_W0        8               /* u16: the frame's: sound_stream_w at its start */
#define WST_O_LEFT      10              /* u16: data bytes still wanted: at 0 the bytes are no
                                        ** longer data (nothing past the song is stored) */
#define WST_SIZE        12
#define WST_FILL_MAX    500             /* the fill stops with this many unread (of 524) */

#ifndef __ASSEMBLER__
#include <stdint.h>

typedef struct {
	uint16_t x, y;
	uint16_t pc, cc;
	uint16_t aw;
	uint8_t  on, prod;
	uint16_t under;
	uint8_t  mark[4];
	uint16_t frames;
	uint8_t  toff, klo, khi, lines, first, cut;
} uzm_t;

_Static_assert(sizeof(uzm_t) == UZM_SIZE, "uzm_t: offsets out of step with uzm.def.h");

typedef struct {
	uint16_t k;
	uint8_t  off, on;
	uint16_t stored, frames;
	uint16_t w0, left;
} wst_t;

_Static_assert(sizeof(wst_t) == WST_SIZE, "wst_t: offsets out of step with uzm.def.h");

extern volatile uzm_t uzm;

/* 1: each vsync silences the bank about to be played (the frames still write the picture's
** sound): a still picture, its sound not repeated every field */
extern volatile uint8_t uzm_mute;

/* The ring empty, at its start */
void     uzm_reset(void);
/* Video frames (1) or mode 5's text frames (0), from the next field. Waits for the vsync's
** pulses (up to a field) to switch: see videoMode.c. */
void     uzm_video(uint8_t on);
/* Mode 5's text frames on that many lines from the next field (FRAME_LINES: all), to free
** the CPU for a long card job. Waits for the vsync's pulses, as uzm_video. */
void     uzm_text_lines(uint8_t lines);
/* The full frame (0) or the cut one (1) from the next frame. Called right after a frame (in
** the blank, before the next): to the cut at once, to the full frame after up to 12 lines
** (videoMode.c). The read must then start at the picture's sector 0, or UZM_CUT_SECTOR. */
void     uzm_cut(uint8_t on);
/* The next frame plays showing k's sound (0..2) */
static inline void uzm_showing(uint8_t k) { uzm.toff = 161 + UZM_T_SET * k; }
/* In the blank after a cut frame, when the next frame shows the next picture: the hidden
** sectors between them (2 of each) are counted as shown, and arrive and go */
static inline void uzm_drop(void) { uzm.cc += UZM_CUT_DROP; }
/* In the vertical blank, with the read open (uzefs_stream_begin): stores bytes from the card
** in the ring, dropping 0xFE/0xFF, until UZM_FILL_MAX bytes are unread or the frame is two
** lines away. Returns the bytes stored. Interrupts stay on. */
uint16_t uzm_fill(void);
/* Unread bytes in the ring */
static inline int16_t uzm_level(void) { return (int16_t)(uzm.pc - uzm.cc); }
#endif
