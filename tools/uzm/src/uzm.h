/*
 * uzm -- Uzeamp's video files (.UZM): the encoder and its tools, standalone (C and FFmpeg's
 * libraries). See ../README.md; the format is described in format.c.
 */
#ifndef UZM_H
#define UZM_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define UZM_VERSION_STRING "1.0"

/* the picture: 160 x 114 pixels, a byte each (BBGGGRRR), 3 rows a sector */
#define W 160
#define ROWS 114
#define NPIX (W * ROWS)
#define SECTORS 38
#define SAMPLES 262                     /* a frame's samples: one a line */
#define PIC_BYTES (SECTORS * 512)

/* the console: 28,636,360 Hz, 1,820 cycles a line, 262 lines a frame: 60.0546 frames a second */
#define F_CPU 28636360
#define LINE_CYCLES 1820
#define LINES 262
#define FPS ((double)F_CPU / (LINE_CYCLES * LINES))
#define AUDIO_RATE 15734

#define MAXB 0xFD                       /* no byte of a picture's sector is 0xFE or 0xFF */
#define DEFAULT_VERSION 2

/* version 2's trailer: 3 sets of 8 samples (one a showing), then the mark */
#define SET 8
#define SETS 3
#define T_SAMPLES 481
#define T_MARK 505
#define SS_FIRST 2                      /* sectors 2..34 hold samples; 34 starts 2 back */
#define SS_LAST 34
#define LAST_BACK (8 * (SS_LAST - SS_FIRST + 1) - SAMPLES)

/* ---- util.c ---- */
#if defined(__GNUC__) || defined(__clang__)
#define UZM_NORETURN __attribute__((noreturn, format(printf, 1, 2)))
#elif defined(_MSC_VER)
#define UZM_NORETURN __declspec(noreturn)
#else
#define UZM_NORETURN
#endif
UZM_NORETURN void die(const char *fmt, ...);
void *xmalloc(size_t n);
void *xrealloc(void *p, size_t n);
char *xstrdup(const char *s);
FILE *ufopen(const char *path, const char *mode);       /* a UTF-8 path, on every system */
int ufseek(FILE *f, int64_t off, int whence);
int64_t uftell(FILE *f);
int64_t ufsize(const char *path);
const char *base_name(const char *path);
void to_latin1(char *dst, size_t cap, const char *utf8); /* '?' for what latin-1 lacks */
typedef struct {
	uint8_t *p;
	size_t n, cap;
} Buf;
void buf_add(Buf *b, const void *p, size_t n);
void buf_free(Buf *b);

/* ---- format.c ---- */
uint16_t crc16(const uint8_t *d, size_t n, uint16_t crc);   /* CRC-16/XMODEM, an SD card's */
extern const uint16_t CRC_TARGETS[4];
uint16_t fix_crc(uint8_t sec[512]);
void uzm_header(uint8_t h[512], uint32_t pictures, const char *title, const char *artist,
                int version, int n, int m);
void pack_picture(uint8_t out[PIC_BYTES], uint32_t num, const uint8_t *px,
                  const uint8_t *const *sets, int nsets);
void pack_picture_v1(uint8_t out[PIC_BYTES], uint32_t num, const uint8_t *px,
                     const uint8_t *samples);
void mark(uint8_t out[4], uint32_t n, int s);
int sample_slot(int s);
int64_t start_frame(int64_t p, int n, int m);
int showings(int64_t p, int n, int m);
void picture_of(int64_t f, int n, int m, int64_t *p, int *k);
uint32_t frames_for(uint32_t pictures, int version, int n, int m);
void palette(uint8_t pal[256][3]);
int nearest(int r, int g, int b);
void clamp_samples(uint8_t *b, size_t n);

/* the cadences: pictures a second -> (N frames, M pictures) */
int rate_cadence(int rate, int *n, int *m);

typedef struct {
	FILE *f;
	const uint8_t *audio;           /* the frames' sound, 262 samples a frame */
	size_t audio_len;
	int version, n, m;
	uint32_t count;
} Writer;
void frame_sound(const uint8_t *audio, size_t len, int64_t fr, uint8_t out[SAMPLES]);
void writer_add(Writer *w, const uint8_t *px);
void write_picture(FILE *f, const uint8_t *pic);

typedef struct {
	FILE *f;
	int version, w, rows, spp, samples, first, n, m;
	uint32_t pictures, frames;
	char title[65], artist[65];
	int64_t size;
} Uzm;
int uzm_open(Uzm *u, const char *path);
void uzm_close(Uzm *u);
int uzm_sector(Uzm *u, uint32_t p, int s, uint8_t out[512]);   /* 0: past the file's end */
void uzm_pixels(Uzm *u, uint32_t p, uint8_t px[NPIX]);
void uzm_frame_sound(Uzm *u, int64_t fr, uint8_t out[SAMPLES]);

int cmd_info(const char *path);
int cmd_frame(const char *path, long n, const char *out, int is_frame);
int cmd_sound(const char *path, const char *out);
int cmd_test(const char *out, double seconds, int rate, int version);
int pick_version(int v1, int v2, const char *rate, int ivtc);

/* ---- dither.c ---- */
void dither_init(void);
/* k (1-3) showings of a picture: rgb (NPIX x r g b) -> out (k x NPIX bytes) */
void dither_showings(const uint8_t *rgb, int k, uint8_t *out);

/* ---- png.c ---- */
int write_png_rgb(const char *path, int w, int h, const uint8_t *rgb);

/* ---- encode.c / cadence.c ---- */
int cmd_encode(int argc, char **argv);
int cmd_cadence(int argc, char **argv);

#endif
