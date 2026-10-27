/*
 * uzm -- the .UZM format: writing, reading, checking. Byte for byte what uzm.py writes.
 *
 * The file (version 2, the one written; version 1, below, is still read)
 *   Sector 0, the header (read with uzefs_read, so any bytes):
 *      0  "UZM1"           4  u16 version (2)     6  u16 width (160)    8  u16 rows (114)
 *     10  u16 sectors a picture (38)             12  u32 pictures      16  u16 samples a frame (262)
 *     18  u16 first picture's sector (1)         20  u8 N, 21 u8 M: the cadence
 *     32  title (64 bytes)                       96  artist (64 bytes)
 *   The console shows 60.05 frames a second (262 lines of 1820 cycles). Every N frames show M
 *   pictures: picture p is shown from frame floor(p N / M) to the next picture's first, 1-3
 *   times (60: 1/1, 30: 2/1, 25: 12/5, 24: 5/2), so the pictures come at 60.05 M / N a second.
 *   Then the pictures, 38 sectors each. Sector s of a picture holds rows 3s..3s+2 (160 bytes
 *   each, a byte a pixel, BBGGGRRR) and a 32-byte trailer:
 *      0  reserved (0)
 *      1  showing 1's samples: 8 (unsigned, 15,734 Hz)
 *      9  showing 2's: 8
 *     17  showing 3's: 8
 *     25  the picture's number in 3 bytes of 7 bits (low first), then s
 *     29  3 bytes chosen so that the sector's CRC16 is FEFE, FEFF, FFFE or FFFF
 *   A showing is a frame: 262 samples, one a line. Only sectors 2-34 carry samples: sector s
 *   holds a showing's samples 8(s-2)..8(s-2)+7, sector 34 holds 254..261, the 2 before again.
 *   No byte of a picture's sector is 0xFE or 0xFF, and with that CRC the player keeps every
 *   byte below 0xFE that the card sends in a multiple-block read.
 *
 * Version 1 (60 Hz only): version 1, no cadence. The trailer: 0 back (0, or 4 in sector 37),
 *   1 7 samples (sectors 0-36 hold samples 7s..7s+6, sector 37 255..261), 8 the picture's
 *   number and s, 12 zeros, 29 the CRC fix bytes.
 */
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "uzm.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ---- the CRC ---- */
static uint16_t crc_table[256];
static uint16_t *inv;                   /* crc16(x y, 0) -> x << 8 | y */

static void crc_init(void)
{
	if (crc_table[1]) return;
	for (int i = 0; i < 256; i++) {
		uint16_t c = (uint16_t)(i << 8);
		for (int k = 0; k < 8; k++) c = (uint16_t)(c & 0x8000 ? c << 1 ^ 0x1021 : c << 1);
		crc_table[i] = c;
	}
}

uint16_t crc16(const uint8_t *d, size_t n, uint16_t crc)
{
	crc_init();
	while (n--) crc = (uint16_t)(crc << 8 ^ crc_table[(crc >> 8 ^ *d++) & 0xFF]);
	return crc;
}

const uint16_t CRC_TARGETS[4] = { 0xFEFE, 0xFEFF, 0xFFFE, 0xFFFF };

/* The sector's last 3 bytes (a, b, c), all below 0xFE, take its CRC to a target. c0 is the CRC
 * of the first 509; crc(x y, s) = crc(x y, 0) ^ crc(00 00, s), and x y -> crc(x y, 0) is a
 * bijection, so for each a the pair (b, c) is looked up. */
uint16_t fix_crc(uint8_t sec[512])
{
	if (!inv) {
		inv = xmalloc(65536 * sizeof *inv);
		for (int x = 0; x < 256; x++)
			for (int y = 0; y < 256; y++) {
				uint8_t p[2] = { (uint8_t)x, (uint8_t)y };
				inv[crc16(p, 2, 0)] = (uint16_t)(x << 8 | y);
			}
	}
	static const uint8_t zero[2];
	uint16_t c0 = crc16(sec, 509, 0);
	for (int a = 0; a <= MAXB; a++) {
		uint8_t ab = (uint8_t)a;
		uint16_t c1 = crc16(&ab, 1, c0);
		uint16_t z = crc16(zero, 2, c1);
		for (int t = 0; t < 4; t++) {
			uint16_t bc = inv[CRC_TARGETS[t] ^ z];
			int b = bc >> 8, c = bc & 0xFF;
			if (b <= MAXB && c <= MAXB) {
				sec[509] = (uint8_t)a;
				sec[510] = (uint8_t)b;
				sec[511] = (uint8_t)c;
				return CRC_TARGETS[t];
			}
		}
	}
	die("no CRC fix for a sector");
}

/* ---- packing ---- */
static void put16(uint8_t *p, unsigned v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void put32(uint8_t *p, uint32_t v) { put16(p, v & 0xFFFF); put16(p + 2, v >> 16); }
static unsigned get16(const uint8_t *p) { return p[0] | p[1] << 8; }
static uint32_t get32(const uint8_t *p) { return get16(p) | (uint32_t)get16(p + 2) << 16; }

void uzm_header(uint8_t h[512], uint32_t pictures, const char *title, const char *artist,
                int n, int m)
{
	memset(h, 0, 512);
	memcpy(h, "UZM1", 4);
	put16(h + 4, UZM_FORMAT);
	put16(h + 6, W);
	put16(h + 8, ROWS);
	put16(h + 10, SECTORS);
	put32(h + 12, pictures);
	put16(h + 16, SAMPLES);
	put16(h + 18, 1);
	h[20] = (uint8_t)n;
	h[21] = (uint8_t)m;
	to_latin1((char *)h + 32, 64, title ? title : "");
	to_latin1((char *)h + 96, 64, artist ? artist : "");
}

void mark(uint8_t out[4], uint32_t n, int s)
{
	out[0] = n & 0x7F;
	out[1] = (n >> 7) & 0x7F;
	out[2] = (n >> 14) & 0x7F;
	out[3] = (uint8_t)s;
}

int sample_slot(int s)
{
	if (s < SS_FIRST || s > SS_LAST) return -1;
	if (s == SS_LAST) return SAMPLES - SET;
	return SET * (s - SS_FIRST);
}

static void check_sector(const uint8_t *sec, uint32_t n, int s)
{
	for (int i = 0; i < 509; i++)
		if (sec[i] > MAXB) die("picture %u sector %d holds 0xFE/0xFF", n, s);
}

/* Version 2: the 38 sectors of picture num; sets: the sound of each showing (1-3 x 262) */
void pack_picture(uint8_t out[PIC_BYTES], uint32_t num, const uint8_t *px,
                  const uint8_t *const *sets, int nsets)
{
	if (nsets < 1 || nsets > SETS) die("a picture is shown 1-3 times, not %d", nsets);
	for (int s = 0; s < SECTORS; s++) {
		uint8_t *sec = out + 512 * s;
		memset(sec, 0, 512);
		memcpy(sec, px + 480 * s, 480);
		int slot = sample_slot(s);
		if (slot >= 0)
			for (int k = 0; k < nsets; k++) memcpy(sec + T_SAMPLES + SET * k, sets[k] + slot, SET);
		mark(sec + T_MARK, num, s);
		check_sector(sec, num, s);
		fix_crc(sec);
	}
}

/* ---- the cadence ---- */
int64_t start_frame(int64_t p, int n, int m) { return p * n / m; }

int showings(int64_t p, int n, int m) { return (int)(start_frame(p + 1, n, m) - start_frame(p, n, m)); }

void picture_of(int64_t f, int n, int m, int64_t *pp, int *kk)
{
	int64_t p = f * m / n;
	while (start_frame(p + 1, n, m) <= f) p++;
	while (start_frame(p, n, m) > f) p--;
	*pp = p;
	*kk = (int)(f - start_frame(p, n, m));
}

uint32_t frames_for(uint32_t pictures, int version, int n, int m)
{
	return version == 1 ? pictures : (uint32_t)start_frame(pictures, n, m);
}

int rate_cadence(int rate, int *n, int *m)
{
	switch (rate) {
	case 60: *n = 1; *m = 1; return 1;
	case 30: *n = 2; *m = 1; return 1;
	case 25: *n = 12; *m = 5; return 1;
	case 24: *n = 5; *m = 2; return 1;
	}
	return 0;
}

/* ---- colours ---- */
void palette(uint8_t pal[256][3])
{
	for (int i = 0; i < 256; i++) {
		pal[i][0] = (uint8_t)((i & 7) * 255 / 7);
		pal[i][1] = (uint8_t)(((i >> 3) & 7) * 255 / 7);
		pal[i][2] = (uint8_t)((i >> 6) * 255 / 3);
	}
	memset(pal[0xFE], 0, 3);            /* never picked for a colour of their own */
	memset(pal[0xFF], 0, 3);
}

int nearest(int r, int g, int b)
{
	uint8_t pal[256][3];
	palette(pal);
	int best = 0;
	long bd = -1;
	for (int i = 0; i < 0xFE; i++) {
		long dr = pal[i][0] - r, dg = pal[i][1] - g, db = pal[i][2] - b;
		long d = dr * dr + dg * dg + db * db;
		if (bd < 0 || d < bd) { bd = d; best = i; }
	}
	return best;
}

void clamp_samples(uint8_t *b, size_t n)
{
	for (size_t i = 0; i < n; i++)
		if (b[i] > MAXB) b[i] = MAXB;
}

/* ---- writing ---- */
void frame_sound(const uint8_t *audio, size_t len, int64_t fr, uint8_t out[SAMPLES])
{
	size_t o = (size_t)fr * SAMPLES;
	size_t have = o >= len ? 0 : len - o < SAMPLES ? len - o : SAMPLES;
	if (have) memcpy(out, audio + o, have);
	memset(out + have, 0x80, SAMPLES - have);
}

void write_picture(FILE *f, const uint8_t *pic)
{
	if (fwrite(pic, 1, PIC_BYTES, f) != PIC_BYTES) die("writing the file failed (disk full?)");
}

/* the next picture, with its showings' sound taken from the frames' stream */
void writer_add(Writer *w, const uint8_t *px)
{
	static uint8_t pic[PIC_BYTES], snd[SETS][SAMPLES];
	uint32_t p = w->count;
	int64_t f0 = start_frame(p, w->n, w->m);
	int k = showings(p, w->n, w->m);
	const uint8_t *sets[SETS];
	for (int j = 0; j < k; j++) {
		frame_sound(w->audio, w->audio_len, f0 + j, snd[j]);
		sets[j] = snd[j];
	}
	pack_picture(pic, p, px, sets, k);
	write_picture(w->f, pic);
	w->count++;
}

/* ---- reading ---- */
int uzm_open(Uzm *u, const char *path)
{
	uint8_t h[512];
	memset(u, 0, sizeof *u);
	u->f = ufopen(path, "rb");
	if (!u->f) die("can't open %s", path);
	if (fread(h, 1, 512, u->f) < 160 || memcmp(h, "UZM1", 4)) die("%s: not a .UZM file", path);
	u->version = (int)get16(h + 4);
	if (u->version != 1 && u->version != 2) die("%s: not a .UZM file of version 1 or 2", path);
	u->w = (int)get16(h + 6);
	u->rows = (int)get16(h + 8);
	u->spp = (int)get16(h + 10);
	u->pictures = get32(h + 12);
	u->samples = (int)get16(h + 16);
	u->first = (int)get16(h + 18);
	u->n = u->version >= 2 ? h[20] : 1;
	u->m = u->version >= 2 ? h[21] : 1;
	if (!u->n || !u->m) die("%s: no cadence in the header", path);
	memcpy(u->title, h + 32, 64);
	memcpy(u->artist, h + 96, 64);
	u->title[64] = u->artist[64] = 0;
	u->frames = frames_for(u->pictures, u->version, u->n, u->m);
	u->size = ufsize(path);
	return 0;
}

void uzm_close(Uzm *u)
{
	if (u->f) fclose(u->f);
	u->f = NULL;
}

int uzm_sector(Uzm *u, uint32_t p, int s, uint8_t out[512])
{
	int64_t o = 512 * ((int64_t)u->first + (int64_t)p * u->spp + s);
	memset(out, 0, 512);
	if (ufseek(u->f, o, SEEK_SET)) return 0;
	return fread(out, 1, 512, u->f) == 512;
}

void uzm_pixels(Uzm *u, uint32_t p, uint8_t px[NPIX])
{
	uint8_t sec[512];
	for (int s = 0; s < SECTORS; s++) {
		uzm_sector(u, p, s, sec);
		memcpy(px + 480 * s, sec, 480);
	}
}

/* frame fr's 262 samples, as the player writes them into its bank, trailer by trailer */
void uzm_frame_sound(Uzm *u, int64_t fr, uint8_t bank[SAMPLES])
{
	uint8_t sec[512];
	memset(bank, 0, SAMPLES);
	if (u->version == 1) {
		int pos = 0;
		for (int s = 0; s < u->spp; s++) {
			uzm_sector(u, (uint32_t)fr, s, sec);
			if (s == u->spp - 1) pos -= sec[480];
			if (pos >= 0 && pos + 7 <= SAMPLES) memcpy(bank + pos, sec + 481, 7);
			pos += 7;
		}
		return;
	}
	int64_t p;
	int k;
	picture_of(fr, u->n, u->m, &p, &k);
	int pos = 0;
	for (int s = SS_FIRST; s <= SS_LAST; s++) {
		uzm_sector(u, (uint32_t)p, s, sec);
		if (s == SS_LAST) pos -= LAST_BACK;
		memcpy(bank + pos, sec + T_SAMPLES + SET * k, SET);
		pos += SET;
	}
}

/* ---- the commands that only read or make files ---- */
int cmd_info(const char *path)
{
	Uzm u;
	uzm_open(&u, path);
	printf("UZM1 v%d %dx%d, %d sectors a picture, %u pictures, %d samples a frame, first sector %d\n",
	       u.version, u.w, u.rows, u.spp, u.pictures, u.samples, u.first);
	if (u.version >= 2) {
		int most = 0;
		for (uint32_t p = 0; p < u.pictures && p < (uint32_t)u.n + 1; p++) {
			int k = showings(p, u.n, u.m);
			if (k > most) most = k;
		}
		printf("cadence %d frames for %d pictures: %.2f pictures a second, each shown ",
		       u.n, u.m, FPS * u.m / u.n);
		if (most == 1) printf("once\n");
		else printf("up to %d times\n", most);
	}
	printf("%u frames, %.1f s\n", u.frames, u.frames / FPS);
	printf("title \"%s\", artist \"%s\"\n", u.title, u.artist);
	int64_t want = 512 * ((int64_t)u.first + (int64_t)u.pictures * u.spp);
	if (u.size < want)
		printf("the file is %lld bytes, its pictures need %lld\n", (long long)u.size,
		       (long long)want);
	uint64_t bad = 0;
	int mo = u.version == 1 ? 488 : T_MARK;
	uint8_t sec[512], mk[4];
	for (uint32_t p = 0; p < u.pictures; p++)
		for (int s = 0; s < u.spp; s++) {
			int ok = uzm_sector(&u, p, s, sec);
			int high = 0;
			for (int i = 0; i < 512; i++) high |= sec[i] > MAXB;
			uint16_t c = crc16(sec, 512, 0);
			mark(mk, p, s);
			if (!ok || high || (c != 0xFEFE && c != 0xFEFF && c != 0xFFFE && c != 0xFFFF) ||
			    memcmp(sec + mo, mk, 4))
				bad++;
		}
	printf("sectors checked: %llu, bad: %llu\n", (unsigned long long)u.pictures * u.spp,
	       (unsigned long long)bad);
	uzm_close(&u);
	return bad ? 1 : 0;
}

/* picture n (or the picture frame n shows) as a PNG, 4x wide and 4x tall */
int cmd_frame(const char *path, long n, const char *out, int is_frame)
{
	Uzm u;
	uzm_open(&u, path);
	if (n < 0) die("a picture's number is 0 or more");
	uint32_t p = (uint32_t)n;
	if (is_frame) {
		int64_t pp = n;
		int k = 0;
		if (u.version >= 2) picture_of(n, u.n, u.m, &pp, &k);
		printf("frame %ld: picture %lld, showing %d\n", n, (long long)pp, k + 1);
		p = (uint32_t)pp;
	}
	if (p >= u.pictures) die("%s has %u pictures", path, u.pictures);
	uint8_t px[NPIX], pal[256][3];
	palette(pal);
	uzm_pixels(&u, p, px);
	uzm_close(&u);
	uint8_t *rgb = xmalloc((size_t)W * 4 * ROWS * 4 * 3);
	for (int y = 0; y < ROWS * 4; y++)
		for (int x = 0; x < W * 4; x++)
			memcpy(rgb + 3 * ((size_t)y * W * 4 + x), pal[px[(y / 4) * W + x / 4]], 3);
	if (write_png_rgb(out, W * 4, ROWS * 4, rgb)) die("can't write %s", out);
	free(rgb);
	printf("%s\n", out);
	return 0;
}

/* the sound as the player plays it, a frame at a time, as an 8-bit WAV */
int cmd_sound(const char *path, const char *out)
{
	Uzm u;
	uzm_open(&u, path);
	FILE *f = ufopen(out, "wb");
	if (!f) die("can't write %s", out);
	uint32_t data = u.frames * SAMPLES;
	uint8_t h[44];
	memcpy(h, "RIFF", 4);
	put32(h + 4, 36 + data);
	memcpy(h + 8, "WAVEfmt ", 8);
	put32(h + 16, 16);
	put16(h + 20, 1);
	put16(h + 22, 1);
	put32(h + 24, AUDIO_RATE);
	put32(h + 28, AUDIO_RATE);
	put16(h + 32, 1);
	put16(h + 34, 8);
	memcpy(h + 36, "data", 4);
	put32(h + 40, data);
	fwrite(h, 1, 44, f);
	uint8_t bank[SAMPLES];
	for (uint32_t fr = 0; fr < u.frames; fr++) {
		uzm_frame_sound(&u, fr, bank);
		fwrite(bank, 1, SAMPLES, f);
	}
	if (data & 1) fputc(0, f);
	if (fclose(f)) die("writing %s failed", out);
	printf("%s: %u frames, %.1f s\n", out, u.frames, u.frames / FPS);
	uzm_close(&u);
	return 0;
}

/* ---- the test pattern ---- */
static const char *DIGITS[10] = {
	"111101101101111", "010110010010111", "111001111100111", "111001111001111",
	"101101111001001", "111100111001111", "111100111101111", "111001010010010",
	"111101111101111", "111101111001111"
};

static void test_picture(uint32_t n, uint8_t px[NPIX])
{
	static const uint8_t bars[8] = { 0x07, 0x38, 0xC0, 0x3F, 0xC7, 0xF8, 0xFD, 0x00 };
	for (int y = 0; y < ROWS; y++)
		for (int x = 0; x < W; x++) {
			int c, v;
			if (y < 24) c = bars[x * 8 / W];
			else if (y < 40) c = x * 8 / W;                         /* red ramp */
			else if (y < 56) c = (x * 8 / W) << 3;                  /* green ramp */
			else if (y < 72) c = (x * 4 / W) << 6;                  /* blue ramp */
			else if (y < 88) { v = x * 8 / W; c = v | v << 3 | (v >> 1) << 6; }   /* grey */
			else c = ((x / 8 + y / 8) & 1) ? 0x49 : 0x00;          /* checker */
			px[y * W + x] = (uint8_t)(c < MAXB ? c : MAXB);
		}
	int bx = (int)(n % (W - 12));                                     /* a box moving */
	for (int y = 92; y < 104; y++)
		for (int x = bx; x < bx + 12; x++) px[y * W + x] = 0xFD;
	char s[16];
	snprintf(s, sizeof s, "%05u", n % 100000);                        /* the picture number */
	for (int i = 0; i < 5; i++) {
		const char *g = DIGITS[s[i] - '0'];
		for (int gy = 0; gy < 5; gy++)
			for (int gx = 0; gx < 3; gx++)
				if (g[gy * 3 + gx] == '1')
					for (int yy = 0; yy < 2; yy++)
						for (int xx = 0; xx < 3; xx++)
							px[(90 + gy * 2 + yy) * W + 110 + i * 10 + gx * 3 + xx] = 0x3F;
	}
}

int cmd_test(const char *out, double seconds, int rate)
{
	int n, m;
	if (!rate_cadence(rate, &n, &m)) die("--rate: 60, 30, 25 or 24");
	uint32_t pics = (uint32_t)llrint(seconds * FPS * m / n);
	uint32_t frames = (uint32_t)start_frame(pics, n, m);
	size_t len = (size_t)frames * SAMPLES;
	uint8_t *audio = xmalloc(len);
	for (size_t t = 0; t < len; t++)                                  /* 440 Hz, continuous */
		audio[t] = (uint8_t)(0x80 + (int)(60 * sin(2 * M_PI * 440 * (double)t / AUDIO_RATE)));
	FILE *f = ufopen(out, "wb");
	if (!f) die("can't write %s", out);
	uint8_t h[512], px[NPIX];
	char title[32];
	snprintf(title, sizeof title, "TEST PATTERN %d", rate);
	uzm_header(h, pics, title, "uzm", n, m);
	fwrite(h, 1, 512, f);
	Writer w = { f, audio, len, n, m, 0 };
	for (uint32_t i = 0; i < pics; i++) {
		test_picture(i, px);
		writer_add(&w, px);
	}
	if (fclose(f)) die("writing %s failed", out);
	free(audio);
	printf("%s: version %d, %u pictures at %.2f a second, %.1f s, %lld bytes\n", out, UZM_FORMAT,
	       pics, FPS * m / n, frames / FPS, (long long)ufsize(out));
	return 0;
}
