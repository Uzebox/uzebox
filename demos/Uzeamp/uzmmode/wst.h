/*
 *  The song stream (SOUND_STREAM=1 builds of uzmmode): a plain WAV played straight from the
 *  card, at 15,734 or 31,468 Hz, while the text frames are drawn. ../plan-stream.md.
 *
 *  The song plays from mix_buf, the kernel's 524-byte sound buffer, used as a ring: read at
 *  mix_pos by the kernel in the blank (update_sound; compare B and the vsync's half lines for
 *  the second sample a line at 31.5 kHz) and by the stream's text frame (gen_wst.py), each
 *  played slot left at 0x80, so a ring not refilled plays silence. The card's multiple-block
 *  read stays open: the text frame's lines clock one byte (two at 31.5 kHz) through a framing
 *  state machine into the ring, and wst_poll's fill tops it up in the blank.
 *
 *  The file must be in one piece (uzefs_fragments). While the stream is open no other uzefs
 *  call may run (the card is the stream's): wst_stop first.
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "uzefs/uzefs.h"

/* The frame's and the fill's state (uzm.def.h): wst.k, wst.off, wst.on, wst.stored,
** wst.frames */
extern volatile wst_t wst;

/* The kernel's (SOUND_STREAM): bit 0, two samples a line; bit 7, a stream (played slots
** left at 0x80, the read stopped at sound_stream_w); sound_stream_held set by a stop there */
extern volatile uint8_t sound_stream;
extern volatile uint16_t sound_stream_w;
extern volatile uint8_t sound_stream_held;

/* The blank's fill (wst_frame.inc): stores up to max data bytes (give or take 16) */
uint16_t wst_fill(uint16_t max);

enum { WST_IDLE, WST_PLAYING, WST_ENDING, WST_DONE, WST_FAILED };

typedef struct {
	uint8_t  state;         /* WST_* */
	uint8_t  rate31;        /* 1: 31,468 Hz */
	uint8_t  fh;
	uint32_t first;         /* the file's first card sector */
	uint32_t data;          /* the samples' first byte in the file */
	uint32_t len;           /* the samples' bytes */
	uint32_t stored;        /* the samples stored since the start (from data + from) */
	uint32_t from;          /* where the stream started, in the samples */
	int16_t  level;         /* unread in the ring after the last frame */
	uint16_t low;           /* the lowest level seen (from the second field on) */
	uint16_t dry;           /* fields the ring ran empty in (it held: a gap of silence) */
	uint16_t errors;        /* error tokens (each read restarted at the next sample) */
	uint16_t fields;        /* fields polled */
	uint16_t fills;         /* fills cut short by the frame (the ring not topped up) */
	uint16_t s16, f16;      /* wst.stored and wst.frames when last polled */
} wst_song_t;

extern wst_song_t wsong;

/* Starts a song: fh open, the samples at data in the file, len bytes, from the sample from;
** rate31 1 for 31,468 Hz. The ring is silenced and the text frames become the stream's for
** two fields first. UZEFS_ERR_* (the card refused, or no token came: the stream is stopped). */
uzefs_res wst_start(uint8_t fh, uint32_t data, uint32_t len, uint32_t from, uint8_t rate31);

/* Once a field, right after the text frame (after WaitVsync): the bookkeeping, a restart after
** an error token, the song's end, then the fill. Returns wsong.state. */
uint8_t wst_poll(void);

/* Stops the stream (CMD12): the ring plays its unread samples out, then silence. The text
** frames stay the stream's until wst_off. */
void wst_stop(void);

/* After wst_stop: the ring silenced, the kernel's single-rate output and mode 5's text frames
** back (the WAV player's buffer). */
void wst_off(void);
