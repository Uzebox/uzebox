/*
 *  Uzeamp video (UZM): definitions shared by the C code and the renderer.
 *  See ../tools/uzm.py for the file format.
 *
 *  The renderer draws 114 rows of 160 pixels, each row on 2 lines (228 lines), a byte a
 *  pixel (BBGGGRRR) straight to the DAC, 9 cycles a pixel. While it draws, it reads the card:
 *  a multiple-block read (CMD18) is open all the time, and every 18 cycles one byte is
 *  clocked in and stored in a 2 KB ring at 0x0800-0x0FFF. A byte moves the ring's write
 *  pointer on only if it is below 0xFE: the file holds no 0xFE/0xFF and each sector's CRC is
 *  made of them, so the card's CRCs, waits and tokens never reach the ring.
 *
 *  A picture is 38 sectors: 3 rows (480 bytes) and a 32-byte trailer each. The trailer
 *  carries the sound (7 samples) and the picture's number. The ring holds 4 sectors and
 *  starts on one, so a row never crosses the ring's end.
 */
#pragma once

#define UZM_RING        0x0800          /* the ring, 0x0800-0x0FFF: its wrap is andi/ori */
#define UZM_RING_SIZE   2048
#define UZM_WIDTH       160
#define UZM_ROWS        114
#define UZM_LINES       228             /* UZM_ROWS lines, twice each */
#define UZM_FIRST_LINE  20              /* the first line drawn (SetRenderingParameters) */
#define UZM_SECTORS     38              /* sectors a picture */
#define UZM_SAMPLES     262             /* sound samples a picture: one a line */
#define UZM_TRAILER     32              /* bytes after a sector's 3 rows */
#define UZM_T_BACK      0               /* trailer: how far before the bank's pointer to write */
#define UZM_T_SAMPLES   1               /* 7 samples */
#define UZM_T_MARK      8               /* the picture's number (3 x 7 bits), the sector */

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
#define UZM_SIZE        20

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
} uzm_t;

_Static_assert(sizeof(uzm_t) == UZM_SIZE, "uzm_t: offsets out of step with uzm.def.h");

extern volatile uzm_t uzm;

/* 1: each vsync silences the bank about to be played (the frames still write the picture's
** sound): a still picture, its sound not repeated every field */
extern volatile uint8_t uzm_mute;

/* The ring empty, at its start */
void     uzm_reset(void);
/* Video frames (1) or mode 5's text frames (0), from the next field. Waits for the vsync's
** pulses (up to a field) to switch: see videoMode.c. */
void     uzm_video(uint8_t on);
/* In the vertical blank, with the read open (uzefs_stream_begin): stores bytes from the card
** in the ring, dropping 0xFE/0xFF, until UZM_FILL_MAX bytes are unread or the frame is two
** lines away. Returns the bytes stored. Interrupts stay on. */
uint16_t uzm_fill(void);
/* Unread bytes in the ring */
static inline int16_t uzm_level(void) { return (int16_t)(uzm.pc - uzm.cc); }
#endif
