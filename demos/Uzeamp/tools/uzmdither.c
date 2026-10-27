/*
 * uzmdither -- uzm.py's temporal dither (encode --temporal) in C: the same arithmetic as
 * uzm.py's dither_picture and temporal_job, byte for byte, about 100 times faster.
 *
 *     uzmdither < jobs > pictures
 *
 * A job: 1 byte k (the showings, 1-3), then the source picture, 160 x 114 pixels of r g b.
 * Out: k pictures of 160 x 114 bytes (the console's colours, BBGGGRRR, never 0xFE/0xFF).
 * Showing j aims at (j + 1) x the source minus the levels already shown (clamped to 0-383),
 * dithered by Sierra 2-4A (2/4 to the next pixel, 1/4 below, 1/4 below-behind), odd showings
 * right to left. Each picture's output is flushed before the next job is read.
 *
 * Built by uzm.py with gcc -O2 (doubles as Python's, no -ffast-math: the same results).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#define W 160
#define ROWS 114
#define N (W * ROWS)

static int L8[8], L4[4];
static unsigned char Q8[256], Q4[256];

static void tables(void)
{
	for (int i = 0; i < 8; i++) L8[i] = i * 255 / 7;
	for (int i = 0; i < 4; i++) L4[i] = i * 255 / 3;
	for (int v = 0; v < 256; v++) {
		int b = 0;
		for (int i = 1; i < 8; i++) if (abs(L8[i] - v) < abs(L8[b] - v)) b = i;
		Q8[v] = b;
		b = 0;
		for (int i = 1; i < 4; i++) if (abs(L4[i] - v) < abs(L4[b] - v)) b = i;
		Q4[v] = b;
	}
}

/* 0xFE/0xFF (blue 3, green 7, red 6-7): the nearest allowed colour around (as uzm.py) */
static void near_white(double vr, double vg, double vb, int *qr, int *qg, int *qb)
{
	int R[2] = { *qr, *qr - 1 }, G[2] = { *qg, *qg - 1 }, B[2] = { *qb, *qb - 1 };
	double best = -1;
	int br = 0, bg = 0, bb = 0;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++) {
				int r = R[i], g = G[j], b = B[k];
				if (r < 0 || g < 0 || b < 0 || (b << 6 | g << 3 | r) >= 0xFE) continue;
				double dr = vr - L8[r], dg = vg - L8[g], db = vb - L4[b];
				double d = dr * dr + dg * dg + db * db;
				if (best < 0 || d < best) { best = d; br = r; bg = g; bb = b; }
			}
	*qr = br; *qg = bg; *qb = bb;
}

static double clamp255(double v) { return v < 0 ? 0.0 : v > 255 ? 255.0 : v; }

/* one picture: targets t[3][N] -> px[N], the levels shown added to shown[3][N] */
static void dither(const int t[3][N], int rev, unsigned char *px, int shown[3][N])
{
	static double rows[2][3][W + 2];
	double (*c)[W + 2] = rows[0], (*n)[W + 2] = rows[1];
	int dx = rev ? -1 : 1;
	memset(rows, 0, sizeof rows);
	for (int y = 0; y < ROWS; y++) {
		double (*tmp)[W + 2] = c;
		c = n;
		n = tmp;
		memset(n, 0, sizeof rows[0]);
		double ar = 0, ag = 0, ab = 0;
		for (int s = 0; s < W; s++) {
			int x = rev ? W - 1 - s : s, i = y * W + x, e = x + 1;
			double ur = t[0][i] + c[0][e] + ar;
			double ug = t[1][i] + c[1][e] + ag;
			double ub = t[2][i] + c[2][e] + ab;
			double vr = clamp255(ur), vg = clamp255(ug), vb = clamp255(ub);
			int qr = Q8[(int)(vr + 0.5)], qg = Q8[(int)(vg + 0.5)], qb = Q4[(int)(vb + 0.5)];
			if (qb == 3 && qg == 7 && qr >= 6) near_white(ur, ug, ub, &qr, &qg, &qb);
			px[i] = (unsigned char)(qb << 6 | qg << 3 | qr);
			int sr = L8[qr], sg = L8[qg], sb = L4[qb];
			shown[0][i] += sr;
			shown[1][i] += sg;
			shown[2][i] += sb;
			double er = (vr - sr) * 0.25, eg = (vg - sg) * 0.25, eb = (vb - sb) * 0.25;
			ar = er + er;
			ag = eg + eg;
			ab = eb + eb;
			n[0][e] += er;
			n[1][e] += eg;
			n[2][e] += eb;
			n[0][e - dx] += er;
			n[1][e - dx] += eg;
			n[2][e - dx] += eb;
		}
	}
}

int main(void)
{
	static unsigned char rgb[3 * N], px[N];
	static int t[3][N], shown[3][N];
	int k;
#ifdef _WIN32
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
#endif
	tables();
	while ((k = getchar()) != EOF) {
		if (k < 1 || k > 3 || fread(rgb, 1, sizeof rgb, stdin) != sizeof rgb) {
			fprintf(stderr, "uzmdither: a bad job\n");
			return 1;
		}
		memset(shown, 0, sizeof shown);
		for (int j = 0; j < k; j++) {
			for (int ch = 0; ch < 3; ch++)
				for (int i = 0; i < N; i++) {
					int v = (j + 1) * rgb[3 * i + ch] - shown[ch][i];
					t[ch][i] = v < 0 ? 0 : v > 383 ? 383 : v;
				}
			dither((const int (*)[N])t, j & 1, px, shown);
			fwrite(px, 1, N, stdout);
		}
		fflush(stdout);
	}
	return 0;
}
