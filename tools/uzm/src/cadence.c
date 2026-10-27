/*
 * uzm cadence -- which rate a source's pictures really have. At 40x28 in grey, a frame is new
 * when it differs from the one before by more than noise (1 level a pixel on average). Film
 * carried at 60 (or 59.94) fps by repeating its frames 3, 2 has its new frames at 2 fixed places
 * in each group of 5; at 30 (29.97), at 4. A source whose new frames fall anywhere has no film
 * rate to recover: keep its own.
 */
#include <stdlib.h>
#include <string.h>

#include <libavutil/frame.h>

#include "media.h"
#include "uzm.h"

#define CW 40
#define CH 28

typedef struct {
	uint8_t prev[CW * CH];
	long frames;
	long *new_at;
	size_t n_new, cap;
} Cad;

static int gray_frame(void *op, AVFrame *f)
{
	Cad *c = op;
	uint8_t cur[CW * CH];
	if (f->width != CW || f->height != CH) die("cadence: %dx%d pictures", f->width, f->height);
	for (int y = 0; y < CH; y++) memcpy(cur + y * CW, f->data[0] + (size_t)y * f->linesize[0], CW);
	if (c->frames) {
		long sum = 0;
		for (int i = 0; i < CW * CH; i++) sum += abs(cur[i] - c->prev[i]);
		if ((double)sum / (CW * CH) > 1.0) {
			if (c->n_new == c->cap) {
				c->cap = c->cap * 2 + 1024;
				c->new_at = xrealloc(c->new_at, c->cap * sizeof *c->new_at);
			}
			c->new_at[c->n_new++] = c->frames;
		}
	}
	memcpy(c->prev, cur, sizeof cur);
	c->frames++;
	return 0;
}

static int choose(double r)
{
	if (r >= 45) return 60;
	if (r >= 27) return 30;
	if (r >= 24.5) return 25;
	return 24;
}

int cmd_cadence(int argc, char **argv)
{
	if (argc < 1) die("usage: uzm cadence IN [--start S] [--duration D]");
	Cut cut = { argv[0], AV_NOPTS_VALUE, AV_NOPTS_VALUE };
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--start") && i + 1 < argc) cut.start_us = parse_time_us(argv[++i], "--start");
		else if (!strcmp(argv[i], "--duration") && i + 1 < argc)
			cut.duration_us = parse_time_us(argv[++i], "--duration");
		else die("unknown option %s", argv[i]);
	}
	AVRational ar = media_frame_rate(argv[0]);
	double r = ar.num > 0 ? av_q2d(ar) : 0;
	Cad *c = xmalloc(sizeof *c);
	memset(c, 0, sizeof *c);
	media_video(&cut, PICK_FIRST, "scale=40:28:flags=area,format=gray", NULL, NULL, gray_frame, c);
	if (c->frames < 50) die("too few frames (%ld)", c->frames);
	char rs[32] = "?";
	if (r > 0) snprintf(rs, sizeof rs, "%.3f", r);
	printf("%s: %ld frames at %s a second; %zu new pictures (%.1f a second)\n", base_name(argv[0]),
	       c->frames, rs, c->n_new, r > 0 ? c->n_new / (c->frames / r) : 0.0);
	if (!c->n_new) return 0;
	long places[5] = { 0 }, lasts[11] = { 0 };
	for (size_t i = 0; i < c->n_new; i++) places[c->new_at[i] % 5]++;
	for (size_t i = 1; i < c->n_new; i++) {
		long d = c->new_at[i] - c->new_at[i - 1];
		lasts[d < 10 ? d : 10]++;
	}
	printf("  where new pictures fall in each group of 5 frames: {");
	for (int i = 0, first = 1; i < 5; i++)
		if (places[i]) { printf("%s%d: %ld", first ? "" : ", ", i, places[i]); first = 0; }
	printf("}\n  frames each picture lasts (10: 10 or more): {");
	for (int i = 0, first = 1; i <= 10; i++)
		if (lasts[i]) { printf("%s%d: %ld", first ? "" : ", ", i, lasts[i]); first = 0; }
	printf("}\n");
	long order[5];
	memcpy(order, places, sizeof order);
	for (int i = 0; i < 5; i++)                     /* descending */
		for (int j = i + 1; j < 5; j++)
			if (order[j] > order[i]) { long t = order[i]; order[i] = order[j]; order[j] = t; }
	int nplaces = 0;
	long least = -1;
	for (int i = 0; i < 5; i++)
		if (places[i]) {
			nplaces++;
			if (least < 0 || places[i] < least) least = places[i];
		}
	double nn = (double)c->n_new;
	if (r >= 45) {
		double film = (order[0] + order[1]) / nn;
		printf("  in the 2 likeliest places: %.0f%%\n", 100 * film);
		if (film > 0.9) printf("  -> film repeated 3, 2: encode with --ivtc (%.3f a second)\n", r * 2 / 5);
		else printf("  -> no fixed pattern: keep %d (a picture lasting 1 or 2 frames could be missed"
		            " at a lower rate: %ld of %zu)\n", choose(r), lasts[1] + lasts[2], c->n_new - 1);
	} else if (r >= 27) {
		double film = (order[0] + order[1] + order[2] + order[3]) / nn;
		printf("  in the 4 likeliest places: %.0f%%\n", 100 * film);
		if (film > 0.97 && nplaces == 5 && least < 0.03 * nn)
			printf("  -> film with 1 frame in 5 repeated: encode with --ivtc (%.3f a second)\n", r * 4 / 5);
		else printf("  -> its own rate: 30\n");
	} else {
		printf("  -> its own rate: %d\n", r > 0 ? choose(r) : 60);
	}
	free(c->new_at);
	free(c);
	return 0;
}
