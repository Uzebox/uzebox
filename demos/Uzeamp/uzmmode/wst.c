/*
 *  The song stream: the C side. See wst.h, ../plan-stream.md, gen_wst.py.
 */
#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uzebox.h"
#include "wst.h"

wst_song_t wsong;

extern volatile unsigned char sync_phase;

#define WST_MARGIN  16              /* silent samples ahead of the first stored one */
#define WST_TOKEN   200000UL        /* bytes the token is waited for (about 0.15 s) */
#define WST_BUSY    2000            /* bytes of the card's busy a restart waits out */

static uint8_t spi_ff(void){
	SPDR = 0xFF;
	while (!(SPSR & (1 << SPIF))) { }
	return SPDR;
}

/* (a - b) mod the ring, a and b in mix_buf */
static uint16_t ring_diff(uint16_t a, uint16_t b){
	int16_t d = (int16_t)(a - b);
	return d < 0 ? (uint16_t)(d + MIX_BUF_SIZE) : (uint16_t)d;
}

static uint16_t ring_add(uint16_t a, uint16_t n){
	a += n;
	if (a >= (uint16_t)(mix_buf + MIX_BUF_SIZE)) a -= MIX_BUF_SIZE;
	return a;
}

/* 0x80 from a for n slots, wrapping */
static void ring_silence(uint16_t a, uint16_t n){
	while (n--) {
		*(uint8_t *)a = 0x80;
		a = ring_add(a, 1);
	}
}

/* The read at the song's sample s (the read open: restart; else begin), up to its byte: the
** token, then the sector's bytes before it. Returns k for the state machine (514 - skipped). */
static uzefs_res open_at(uint32_t s, bool restart, uint16_t *k){
	uint32_t pos = wsong.data + s;
	uint32_t sect = wsong.first + (pos >> 9);
	uint16_t skip = pos & 511;
	uzefs_res r = restart ? uzefs_stream_restart(sect, WST_BUSY) : uzefs_stream_begin(sect);
	if (r != UZEFS_OK) return r;
	for (uint32_t n = 0; ; n++) {
		uint8_t b = spi_ff();
		if (b == 0xFE) break;
		if (b != 0xFF || n > WST_TOKEN) return UZEFS_ERR_DISK;
	}
	for (uint16_t i = 0; i < skip; i++) spi_ff();
	*k = 514 - skip;
	return UZEFS_OK;
}

uzefs_res wst_start(uint8_t fh, uint32_t data, uint32_t len, uint32_t from, uint8_t rate31){
	uzefs_res r;
	uint32_t run;
	uint16_t k;

	memset(&wsong, 0, sizeof(wsong));
	wsong.fh = fh;
	wsong.data = data;
	wsong.len = len;
	wsong.from = from;
	wsong.rate31 = rate31;
	wsong.low = 0xFFFF;
	r = uzefs_extent(fh, 0, &wsong.first, &run);
	if (r != UZEFS_OK) { wsong.state = WST_FAILED; return r; }

	/* the stream's frames, the ring silent, the read not yet clocked; the kernel enables
	** compare B after a vsync */
	memset(mix_buf, 0x80, MIX_BUF_SIZE);
	cli();
	wst.off = 0xFF;
	wst.k = 0;
	sound_stream_w = (uint16_t)mix_pos;     /* (the read holds there: silence) */
	sound_stream = 0x80 | (rate31 ? 1 : 0);
	wst.on = 1;
	sei();
	/* a vsync passes (the kernel enables compare B at its end; WaitVsync can return at once on
	** a flag already set), then a frame */
	while (sync_phase & 1) { }
	while (!(sync_phase & 1)) { }
	WaitVsync(1);

	r = open_at(from, false, &k);
	if (r != UZEFS_OK) {
		uzefs_stream_end();
		wsong.state = WST_FAILED;
		return r;
	}
	cli();
	sound_stream_w = ring_add((uint16_t)mix_pos, WST_MARGIN);
	wst.left = (len - from) > 0xFFFF ? 0xFFFF : (uint16_t)(len - from);
	wst.k = k;
	wsong.s16 = wst.stored;
	wsong.f16 = wst.frames;
	wst.off = 0;
	sei();
	wsong.level = WST_MARGIN;
	wsong.state = WST_PLAYING;
	wst_fill(len - from);
	sound_stream_held = 0;                  /* (the silence before the song) */
	return UZEFS_OK;
}

uint8_t wst_poll(void){
	uint16_t s, w, rp;
	uint8_t off, held;

	if (wsong.state != WST_PLAYING && wsong.state != WST_ENDING) return wsong.state;
	cli();
	s = wst.stored;
	off = wst.off;
	w = sound_stream_w;
	rp = (uint16_t)mix_pos;
	held = sound_stream_held;
	sound_stream_held = 0;
	wsong.stored += (uint16_t)(s - wsong.s16);
	wsong.s16 = s;
	{	/* the bytes still wanted, with the count (no frame between) */
		uint32_t left = wsong.len - wsong.from - wsong.stored;
		wst.left = left > 0xFFFF ? 0xFFFF : (uint16_t)left;
	}
	sei();
	wsong.fields++;
	wsong.level = ring_diff(w, rp);         /* (the read never passes W) */

	if (wsong.state == WST_ENDING) {
		if (wsong.level == 0) wsong.state = WST_DONE;
		return wsong.state;
	}
	if (held) wsong.dry++;
	/* (not in the song's last ringful: the level falls there) */
	if (wsong.fields > 1 && wsong.len - wsong.from - wsong.stored > MIX_BUF_SIZE &&
	    (uint16_t)wsong.level < wsong.low) wsong.low = wsong.level;

	/* the song's end: the stream stopped, W back to after the song's last sample (the read
	** stops there), what came after it silenced */
	if (wsong.from + wsong.stored >= wsong.len) {
		uint16_t extra = wsong.from + wsong.stored - wsong.len;
		cli();
		wst.off = 0xFF;
		w = sound_stream_w;
		rp = (uint16_t)mix_pos;
		if (extra > ring_diff(w, rp)) extra = ring_diff(w, rp);
		w = ring_add(w, MIX_BUF_SIZE - extra);
		sound_stream_w = w;
		sei();
		ring_silence(w, extra);             /* (beyond W: never read) */
		wsong.level -= extra;
		uzefs_stream_end();
		wsong.state = WST_ENDING;
		return wsong.state;
	}

	/* an error token stopped the read: again from the next sample */
	if (off) {
		uint16_t k;
		wsong.errors++;
		if (open_at(wsong.from + wsong.stored, true, &k) != UZEFS_OK) {
			uzefs_stream_end();
			wsong.state = WST_FAILED;
			return wsong.state;
		}
		cli();
		wst.k = k;
		wst.off = 0;
		sei();
	}

	{
		uint32_t left = wsong.len - wsong.from - wsong.stored;
		wst_fill(left > 0xFFFF ? 0xFFFF : (uint16_t)left);
		/* the fill stopped short of the ring's top only for the frame */
		cli();
		w = sound_stream_w;
		rp = (uint16_t)mix_pos;
		off = wst.off;
		sei();
		if (!off && ring_diff(w, rp) < WST_FILL_MAX - 64 && left > WST_FILL_MAX) wsong.fills++;
	}
	return wsong.state;
}

void wst_stop(void){
	cli();
	wst.off = 0xFF;
	sei();
	if (wsong.state == WST_PLAYING) uzefs_stream_end();
	if (wsong.state == WST_PLAYING || wsong.state == WST_ENDING) wsong.state = WST_IDLE;
}

void wst_off(void){
	cli();
	wst.on = 0;
	sound_stream = 0;
	sei();
	memset(mix_buf, 0x80, MIX_BUF_SIZE);   /* (interrupts on: 524 bytes take 2 lines) */
	WaitVsync(1);
}
