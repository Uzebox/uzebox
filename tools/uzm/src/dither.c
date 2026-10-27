/*
 * uzm -- the dither: Sierra 2-4A error diffusion (2/4 to the next pixel, 1/4 below and 1/4
 * below-behind), each channel on its own, its error kept (no -ffast-math: doubles as written,
 * the same results as uzm.py and uzmdither.c, byte for byte).
 *
 * The palette is each channel's levels combined (red and green 8, blue 4), so each channel is
 * quantised on its own, but for 0xFE and 0xFF (blue 3, green 7, red 6-7): those pixels take the
 * nearest allowed colour among the levels around them.
 *
 * Temporal (k showings of one picture): showing j aims at (j + 1) x the source minus the levels
 * already shown (clamped to 0-383), odd showings right to left, so the showings' average comes
 * closer to the source than one picture can. A plain picture is k = 1.
 */
#include <stdlib.h>
#include <string.h>

#include "uzm.h"

static int L8[8], L4[4];
static unsigned char Q8[256], Q4[256];

void dither_init(void)
{
	for (int i = 0; i < 8; i++) L8[i] = i * 255 / 7;
	for (int i = 0; i < 4; i++) L4[i] = i * 255 / 3;
	for (int v = 0; v < 256; v++) {
		int b = 0;
		for (int i = 1; i < 8; i++) if (abs(L8[i] - v) < abs(L8[b] - v)) b = i;
		Q8[v] = (unsigned char)b;
		b = 0;
		for (int i = 1; i < 4; i++) if (abs(L4[i] - v) < abs(L4[b] - v)) b = i;
		Q4[v] = (unsigned char)b;
	}
}

/* 0xFE/0xFF (blue 3, green 7, red 6-7): the nearest allowed colour around */
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
	*qr = br;
	*qg = bg;
	*qb = bb;
}

static double clamp255(double v) { return v < 0 ? 0.0 : v > 255 ? 255.0 : v; }

/* one picture: targets t[3][NPIX] -> px, the levels shown added to shown[3][NPIX] */
static void dither(int t[3][NPIX], int rev, unsigned char *px, int shown[3][NPIX])
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

void dither_showings(const uint8_t *rgb, int k, uint8_t *out)
{
	static int t[3][NPIX], shown[3][NPIX];
	memset(shown, 0, sizeof shown);
	for (int j = 0; j < k; j++) {
		for (int ch = 0; ch < 3; ch++)
			for (int i = 0; i < NPIX; i++) {
				int v = (j + 1) * rgb[3 * i + ch] - shown[ch][i];
				t[ch][i] = v < 0 ? 0 : v > 383 ? 383 : v;
			}
		dither(t, j & 1, out + (size_t)j * NPIX, shown);
	}
}
