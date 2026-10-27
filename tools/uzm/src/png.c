/*
 * uzm -- a PNG writer with no library: RGB, 8 bits a channel, the image data in stored
 * (uncompressed) deflate blocks. Plenty for a 640 x 456 picture.
 */
#include <stdlib.h>
#include <string.h>

#include "uzm.h"

static uint32_t crc_table[256];

static uint32_t crc32_update(uint32_t c, const uint8_t *p, size_t n)
{
	if (!crc_table[1])
		for (uint32_t i = 0; i < 256; i++) {
			uint32_t v = i;
			for (int k = 0; k < 8; k++) v = v & 1 ? 0xEDB88320u ^ v >> 1 : v >> 1;
			crc_table[i] = v;
		}
	c = ~c;
	while (n--) c = crc_table[(c ^ *p++) & 0xFF] ^ c >> 8;
	return ~c;
}

static void be32(uint8_t *p, uint32_t v)
{
	p[0] = (uint8_t)(v >> 24);
	p[1] = (uint8_t)(v >> 16);
	p[2] = (uint8_t)(v >> 8);
	p[3] = (uint8_t)v;
}

static int chunk(FILE *f, const char *type, const uint8_t *data, uint32_t n)
{
	uint8_t h[8];
	be32(h, n);
	memcpy(h + 4, type, 4);
	uint32_t c = crc32_update(0, h + 4, 4);
	c = crc32_update(c, data, n);
	uint8_t t[4];
	be32(t, c);
	return fwrite(h, 1, 8, f) != 8 || (n && fwrite(data, 1, n, f) != n) || fwrite(t, 1, 4, f) != 4;
}

int write_png_rgb(const char *path, int w, int h, const uint8_t *rgb)
{
	size_t row = (size_t)w * 3 + 1, raw_n = row * (size_t)h;
	uint8_t *raw = xmalloc(raw_n);
	for (int y = 0; y < h; y++) {
		raw[y * row] = 0;                               /* filter: none */
		memcpy(raw + y * row + 1, rgb + (size_t)y * w * 3, (size_t)w * 3);
	}
	size_t blocks = (raw_n + 65534) / 65535;
	size_t z_n = 2 + raw_n + blocks * 5 + 4;
	uint8_t *z = xmalloc(z_n), *p = z;
	*p++ = 0x78;                                        /* zlib: deflate, 32 KB window */
	*p++ = 0x01;
	uint32_t a = 1, b = 0;
	for (size_t o = 0; o < raw_n; o += 65535) {
		size_t n = raw_n - o < 65535 ? raw_n - o : 65535;
		*p++ = o + n == raw_n;                          /* BFINAL, BTYPE 00 (stored) */
		*p++ = (uint8_t)n;
		*p++ = (uint8_t)(n >> 8);
		*p++ = (uint8_t)~n;
		*p++ = (uint8_t)(~n >> 8);
		memcpy(p, raw + o, n);
		p += n;
	}
	for (size_t i = 0; i < raw_n; i++) {                /* Adler-32 */
		a = (a + raw[i]) % 65521;
		b = (b + a) % 65521;
	}
	be32(p, b << 16 | a);
	p += 4;
	FILE *f = ufopen(path, "wb");
	if (!f) {
		free(raw);
		free(z);
		return 1;
	}
	static const uint8_t sig[8] = { 0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n' };
	uint8_t ihdr[13];
	be32(ihdr, (uint32_t)w);
	be32(ihdr + 4, (uint32_t)h);
	ihdr[8] = 8;                                        /* bit depth */
	ihdr[9] = 2;                                        /* RGB */
	ihdr[10] = ihdr[11] = ihdr[12] = 0;
	int err = fwrite(sig, 1, 8, f) != 8 || chunk(f, "IHDR", ihdr, 13) ||
	          chunk(f, "IDAT", z, (uint32_t)(p - z)) || chunk(f, "IEND", NULL, 0);
	err |= fclose(f) != 0;
	free(raw);
	free(z);
	return err;
}
