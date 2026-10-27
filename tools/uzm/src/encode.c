/*
 * uzm encode -- a video file to a .UZM, as uzm.py's encode did with the ffmpeg program, here
 * through FFmpeg's libraries: the same filters, the same dither, the same file.
 */
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <libavutil/avstring.h>
#include <libavutil/frame.h>
#include <libavutil/mathematics.h>

#include "media.h"
#include "uzm.h"

#define COMPRESS "dynaudnorm=f=250:g=15:p=0.9"
/* mono before anything measures the sound: a mix after the filters loses 3 dB (stereo) */
#define MONO "aformat=channel_layouts=mono,"
/* the output formats ffmpeg makes from -f u8 -ar 15734 -ac 1 / -f null -ac 1 / -pix_fmt */
#define OUT_U8 "sample_fmts=u8:sample_rates=15734:channel_layouts=mono:"
#define OUT_NULL "sample_fmts=s16:channel_layouts=mono:"

typedef struct {
	const char *title, *artist, *start, *duration, *dither, *crop, *pre, *post, *black, *scaler,
	           *cut, *loudness, *rate, *audio;
	int letterbox, gpu, ivtc, compress, temporal;
} Opts;

/* ---- fractions, as uzm.py's (the source's rate, the film's) ---- */
typedef struct { int64_t num, den; } Frac;

static int64_t gcd64(int64_t a, int64_t b)
{
	while (b) { int64_t t = a % b; a = b; b = t; }
	return a < 0 ? -a : a;
}

static Frac frac(int64_t num, int64_t den)
{
	int64_t g = gcd64(num, den);
	if (!g) g = 1;
	return (Frac){ num / g, den / g };
}

static Frac frac_mul(Frac a, int64_t num, int64_t den) { return frac(a.num * num, a.den * den); }
static int frac_ge(Frac a, int64_t num, int64_t den) { return a.num * den >= num * a.den; }
static double frac_d(Frac a) { return (double)a.num / a.den; }

static Frac film_rate(Frac r)
{
	if (frac_ge(r, 45, 1)) return frac_mul(r, 2, 5);
	if (frac_ge(r, 27, 1)) return frac_mul(r, 4, 5);
	return r;
}

static int choose_rate(const Frac *r)
{
	if (!r || frac_ge(*r, 45, 1)) return 60;
	if (frac_ge(*r, 27, 1)) return 30;
	if (frac_ge(*r, 49, 2)) return 25;
	return 24;
}

/* ---- the sound ---- */
typedef struct {
	Buf b;
} Sound;

static int sound_frame(void *op, AVFrame *f)
{
	Sound *s = op;
	buf_add(&s->b, f->data[0], (size_t)f->nb_samples);
	return 0;
}

static int ignore_frame(void *op, AVFrame *f)
{
	(void)op;
	(void)f;
	return 0;
}

/* a field of loudnorm's JSON: "name" : "value" */
static int json_field(const char *js, const char *name, char *out, size_t cap)
{
	char key[64];
	snprintf(key, sizeof key, "\"%s\"", name);
	const char *p = strstr(js, key);
	if (!p) return 0;
	p = strchr(p + strlen(key), ':');
	if (!p) return 0;
	p = strchr(p, '"');
	if (!p) return 0;
	const char *e = strchr(++p, '"');
	if (!e || (size_t)(e - p) >= cap) return 0;
	memcpy(out, p, (size_t)(e - p));
	out[e - p] = 0;
	return 1;
}

/* The sound's filters (before the 8-bit conversion): ffmpeg's two-pass EBU R128 loudness
 * normalisation to target LUFS (the first pass measures the source, mono; the second applies
 * one gain, peaks held under -1 dBTP), or none ("off"). A quiet source would otherwise use only
 * a few of the 256 levels. */
static void loudness_filter(char *af, size_t cap, const Cut *cut, int pick, const char *target,
                            const char *pre)
{
	char desc[1024];
	snprintf(af, cap, MONO "%s", pre);
	if (!strcmp(target, "off")) {
		size_t n = strlen(af);
		if (n && af[n - 1] == ',') af[n - 1] = 0;
		return;
	}
	snprintf(desc, sizeof desc, MONO "%sloudnorm=I=%s:TP=-1:LRA=11:print_format=json", pre, target);
	media_capture(1);
	media_audio(cut, pick, desc, OUT_NULL, ignore_frame, NULL);
	media_capture(0);
	const char *log = media_captured(), *open = NULL;
	for (const char *p = log; (p = strchr(p, '{')); p++) open = p;
	char I[32], tp[32], lra[32], th[32], off[32];
	if (!open || !json_field(open, "input_i", I, sizeof I) ||
	    !json_field(open, "input_tp", tp, sizeof tp) ||
	    !json_field(open, "input_lra", lra, sizeof lra) ||
	    !json_field(open, "input_thresh", th, sizeof th) ||
	    !json_field(open, "target_offset", off, sizeof off))
		die("the loudness measurement failed");
	double level = strtod(I, NULL);
	if (!(level > -70)) {
		/* too short or too quiet to measure: loudnorm refuses -inf */
		printf("  sound: %s LUFS, not measurable: kept as it is\n", I);
		size_t n = strlen(af);
		if (n && af[n - 1] == ',') af[n - 1] = 0;
		return;
	}
	printf("  sound: %s LUFS, peaks %s dBTP -> %s LUFS\n", I, tp, target);
	snprintf(af, cap, MONO "%sloudnorm=I=%s:TP=-1:LRA=11:measured_I=%s:measured_TP=%s:"
	         "measured_LRA=%s:measured_thresh=%s:offset=%s:linear=true",
	         pre, target, I, tp, lra, th, off);
}

static uint8_t *read_sound(const Cut *cut, int pick, const Opts *o, size_t *len)
{
	char af[1024];
	if (pick == PICK_BEST && !media_has_stream(cut->path, 1)) {
		/* (uzm.py stopped there: ffmpeg found no sound to write) */
		printf("  (no sound in %s: silence)\n", base_name(cut->path));
		*len = 0;
		return xmalloc(1);
	}
	loudness_filter(af, sizeof af, cut, pick, o->loudness ? o->loudness : "-15",
	                o->compress ? COMPRESS "," : "");
	Sound s = { { 0 } };
	media_audio(cut, pick, af, OUT_U8, sound_frame, &s);
	clamp_samples(s.b.p, s.b.n);
	*len = s.b.n;
	return s.b.p;
}

/* ---- the pictures ---- */
typedef struct {
	FILE *f;
	Writer w;
	int temporal, n, m;
	const uint8_t *audio;
	size_t audio_len;
	uint32_t pics;
	/* paletteuse's pal8 pictures: its palette's entries to the console's bytes */
	uint32_t lut_pal[256];
	uint8_t lut[256];
	int have_lut;
	uint8_t rgb[NPIX * 3], px[3 * NPIX];
} Pics;

static void progress(uint32_t pics)
{
	if (pics % 600 == 0) {
		printf("  %u pictures\n", pics);
		fflush(stdout);
	}
}

static void check_size(const AVFrame *f)
{
	if (f->width != W || f->height != ROWS)
		die("the filters gave %dx%d pictures, not %dx%d", f->width, f->height, W, ROWS);
}

static int rgb_frame(void *op, AVFrame *f)
{
	Pics *p = op;
	check_size(f);
	for (int y = 0; y < ROWS; y++) memcpy(p->rgb + y * W * 3, f->data[0] + (size_t)y * f->linesize[0], W * 3);
	if (!p->temporal) {
		dither_showings(p->rgb, 1, p->px);
		writer_add(&p->w, p->px);
	} else {
		static uint8_t pic[PIC_BYTES], snd[SAMPLES];
		int64_t f0 = start_frame(p->pics, p->n, p->m);
		int k = showings(p->pics, p->n, p->m);
		dither_showings(p->rgb, k, p->px);
		for (int j = 0; j < k; j++) {
			const uint8_t *sets[1] = { snd };
			frame_sound(p->audio, p->audio_len, f0 + j, snd);
			pack_picture(pic, (uint32_t)(f0 + j), p->px + (size_t)j * NPIX, sets, 1);
			write_picture(p->f, pic);
		}
	}
	p->pics++;
	progress(p->pics);
	return 0;
}

static int pal8_frame(void *op, AVFrame *f)
{
	Pics *p = op;
	check_size(f);
	const uint32_t *pal = (const uint32_t *)f->data[1];
	if (!p->have_lut || memcmp(pal, p->lut_pal, sizeof p->lut_pal)) {
		uint8_t ours[256][3];
		palette(ours);
		memcpy(p->lut_pal, pal, sizeof p->lut_pal);
		for (int k = 0; k < 256; k++) {
			int r = pal[k] >> 16 & 255, g = pal[k] >> 8 & 255, b = pal[k] & 255, found = -1;
			for (int i = 0; i < 0xFE && found < 0; i++)
				if (ours[i][0] == r && ours[i][1] == g && ours[i][2] == b) found = i;
			p->lut[k] = (uint8_t)(found >= 0 ? found : nearest(r, g, b));
		}
		p->have_lut = 1;
	}
	for (int y = 0; y < ROWS; y++)
		for (int x = 0; x < W; x++) p->px[y * W + x] = p->lut[f->data[0][(size_t)y * f->linesize[0] + x]];
	writer_add(&p->w, p->px);
	p->pics++;
	progress(p->pics);
	return 0;
}

/* ---- the command ---- */
static const char *ENCODE_USAGE =
	"usage: uzm encode IN OUT.UZM [options]   (uzm help: the options)";

static void cat(char *s, size_t cap, const char *fmt, ...)
{
	va_list ap;
	size_t n = strlen(s);
	if (n && n + 1 < cap) s[n++] = ',', s[n] = 0;
	va_start(ap, fmt);
	vsnprintf(s + n, cap - n, fmt, ap);
	va_end(ap);
	if (strlen(s) + 1 >= cap) die("the filters are too long");
}

int cmd_encode(int argc, char **argv)
{
	Opts o;
	memset(&o, 0, sizeof o);
	if (argc < 2) die("%s", ENCODE_USAGE);
	const char *src = argv[0], *out = argv[1];
	for (int i = 2; i < argc; i++) {
		const char *k = argv[i];
		struct { const char *name; int *flag; } flags[] = {
			{ "--letterbox", &o.letterbox }, { "--gpu", &o.gpu }, { "--ivtc", &o.ivtc },
			{ "--compress", &o.compress }, { "--temporal", &o.temporal },
		};
		struct { const char *name; const char **val; } vals[] = {
			{ "--title", &o.title }, { "--artist", &o.artist }, { "--start", &o.start },
			{ "--duration", &o.duration }, { "--dither", &o.dither }, { "--crop", &o.crop },
			{ "--pre", &o.pre }, { "--post", &o.post }, { "--black", &o.black },
			{ "--scaler", &o.scaler }, { "--cut", &o.cut }, { "--loudness", &o.loudness },
			{ "--rate", &o.rate }, { "--audio", &o.audio },
		};
		int done = 0;
		for (size_t j = 0; j < sizeof flags / sizeof *flags && !done; j++)
			if (!strcmp(k, flags[j].name)) { *flags[j].flag = 1; done = 1; }
		for (size_t j = 0; j < sizeof vals / sizeof *vals && !done; j++)
			if (!strcmp(k, vals[j].name)) {
				if (i + 1 >= argc) die("%s wants a value", k);
				*vals[j].val = argv[++i];
				done = 1;
			}
		if (!done) die("unknown option %s", k);
	}

	AVRational ar = media_frame_rate(src);
	Frac r = { 0, 1 }, film = { 0, 1 };
	int have_r = ar.num > 0, have_film = 0;
	if (have_r) r = frac(ar.num, ar.den);
	if (o.ivtc) {
		if (!have_r) die("--ivtc: IN's frame rate is unknown");
		film = film_rate(r);
		have_film = 1;
	}
	int rate;
	if (!o.rate || !strcmp(o.rate, "auto")) rate = choose_rate(have_film ? &film : have_r ? &r : NULL);
	else rate = atoi(o.rate);
	int n, m;
	if (!rate_cadence(rate, &n, &m)) die("--rate: 60, 30, 25 or 24");
	char rs[32] = "?", fs[64] = "";
	if (have_r) snprintf(rs, sizeof rs, "%.3f", frac_d(r));
	if (have_film) snprintf(fs, sizeof fs, ", film inside at %.3f", frac_d(film));
	printf("  source %s frames a second%s -> %d pictures a second (%.2f: %d frames for %d)\n",
	       rs, fs, rate, FPS * m / n, n, m);
	if (o.gpu) printf("  (--gpu: decoding on the CPU in this build)\n");

	Cut cut = { src, AV_NOPTS_VALUE, AV_NOPTS_VALUE };
	if (o.start) cut.start_us = parse_time_us(o.start, "--start");
	if (o.duration) cut.duration_us = parse_time_us(o.duration, "--duration");
	int apick = o.audio ? atoi(o.audio) : PICK_BEST;

	/* the pictures' rate, exactly the console's: 60.05 x M / N */
	char chain[4096] = "", fps[96];
	snprintf(fps, sizeof fps, "fps=%lld/%lld:round=near", (long long)F_CPU * m,
	         (long long)LINE_CYCLES * LINES * n);
	if (have_film) {
		/* inverse telecine: the film's frames back, each once, on the film's own grid (see
		 * uzm.py): every other frame of a 60 fps source, decimate drops one in 5, the time
		 * base made fine, then the pictures' rate */
		if (film.num * 2 * r.den < r.num * film.den) cat(chain, sizeof chain, "select='not(mod(n\\,2))'");
		cat(chain, sizeof chain, "decimate");
		cat(chain, sizeof chain, "settb=1/240000");
		cat(chain, sizeof chain, "setpts=N/(%lld/%lld)/TB", (long long)film.num, (long long)film.den);
	}
	cat(chain, sizeof chain, "%s", fps);
	if (o.crop) cat(chain, sizeof chain, "crop=%s", o.crop);
	if (o.pre) cat(chain, sizeof chain, "%s", o.pre);
	if (o.black) {
		/* levels at or below black/255 become 0 */
		double b = atoi(o.black) / 255.0;
		cat(chain, sizeof chain, "colorlevels=rimin=%.3f:gimin=%.3f:bimin=%.3f", b, b, b);
	}
	const char *scaler = o.scaler ? o.scaler : "lanczos";
	if (o.letterbox)
		cat(chain, sizeof chain, "scale=%d:%d:force_original_aspect_ratio=decrease:flags=%s,"
		    "pad=%d:%d:(ow-iw)/2:(oh-ih)/2", W, ROWS, scaler, W, ROWS);
	else
		cat(chain, sizeof chain, "scale=%d:%d:flags=%s", W, ROWS, scaler);
	if (o.post) cat(chain, sizeof chain, "%s", o.post);
	if (o.cut) {
		/* pixels whose brightest channel is at most cut become black (last, at 160x114) */
		int t = atoi(o.cut);
		const char *mx = "max(max(r(X,Y),g(X,Y)),b(X,Y))";
		cat(chain, sizeof chain, "format=gbrp,geq=r='if(lte(%s,%d),0,r(X,Y))':g='if(lte(%s,%d),0,"
		    "g(X,Y))':b='if(lte(%s,%d),0,b(X,Y))'", mx, t, mx, t, mx, t);
	}

	const char *d = o.dither ? o.dither : "ours";
	int ours = o.temporal || !strcmp(d, "ours");
	if (o.temporal && strcmp(d, "ours")) printf("  (--temporal: our dither, not %s)\n", d);
	char dither[64] = "";
	if (!ours) {
		if (!strncmp(d, "bayer", 5))
			snprintf(dither, sizeof dither, "dither=bayer:bayer_scale=%s", d[5] ? d + 5 : "2");
		else if (!strcmp(d, "fs")) snprintf(dither, sizeof dither, "dither=floyd_steinberg");
		else if (!strcmp(d, "sierra2") || !strcmp(d, "sierra2_4a") || !strcmp(d, "sierra3") ||
		         !strcmp(d, "burkes") || !strcmp(d, "atkinson") || !strcmp(d, "none"))
			snprintf(dither, sizeof dither, "dither=%s", d);
		else die("--dither: ours, bayerN, fs, sierra2, sierra2_4a, sierra3, burkes, atkinson or none");
	}

	/* the sound first, all of it (its frames go into the pictures' trailers) */
	size_t alen;
	uint8_t *audio = read_sound(&cut, apick, &o, &alen);

	FILE *f = ufopen(out, "wb");
	if (!f) die("can't write %s", out);
	uint8_t h[512];
	uzm_header(h, 0, "", "", 1, 1);
	fwrite(h, 1, 512, f);
	Pics *p = xmalloc(sizeof *p);
	memset(p, 0, sizeof *p);
	p->f = f;
	p->n = n;
	p->m = m;
	p->audio = audio;
	p->audio_len = alen;
	p->temporal = o.temporal;
	p->w = (Writer){ f, audio, alen, n, m, 0 };
	char *desc = xmalloc(sizeof chain + 256);
	if (ours) {
		snprintf(desc, sizeof chain + 256, "%s,format=rgb24", chain);
		media_video(&cut, PICK_BEST, desc, "pix_fmts=rgb24:", NULL, rgb_frame, p);
	} else {
		/* ffmpeg's paletteuse onto the console's colours: a 16x16 picture of them */
		uint8_t pal[256][3];
		palette(pal);
		AVFrame *pf = av_frame_alloc();
		pf->width = 16;
		pf->height = 16;
		pf->format = AV_PIX_FMT_RGB24;
		if (av_frame_get_buffer(pf, 0) < 0) die("out of memory");
		for (int i = 0; i < 256; i++) memcpy(pf->data[0] + (i / 16) * pf->linesize[0] + (i % 16) * 3, pal[i], 3);
		snprintf(desc, sizeof chain + 256, "[in]%s[v];[v][pal]paletteuse=%s:diff_mode=none",
		         chain, dither);
		media_video(&cut, PICK_BEST, desc, "pix_fmts=pal8:", pf, pal8_frame, p);
		av_frame_free(&pf);
	}
	free(desc);

	uint32_t pics = p->pics, frames;
	const char *title = o.title ? o.title : base_name(src), *artist = o.artist ? o.artist : "";
	if (o.temporal) {
		frames = (uint32_t)start_frame(pics, n, m);
		uzm_header(h, frames, title, artist, 1, 1);
	} else {
		frames = (uint32_t)start_frame(pics, n, m);
		uzm_header(h, pics, title, artist, n, m);
	}
	if (ufseek(f, 0, SEEK_SET) || fwrite(h, 1, 512, f) != 512) die("writing %s failed", out);
	if (fclose(f)) die("writing %s failed", out);
	if (alen && (int64_t)alen < (int64_t)frames * SAMPLES - AUDIO_RATE / 10)
		printf("  (the sound is %.2f s shorter than the pictures: silence after it)\n",
		       ((double)frames * SAMPLES - alen) / AUDIO_RATE);
	if (o.temporal)
		printf("%s: %u source pictures (%.2f a second), temporal: %u pictures at %.2f, %.1f s, %.1f MB\n",
		       out, pics, FPS * m / n, frames, FPS, frames / FPS, ufsize(out) / 1e6);
	else
		printf("%s: %u pictures (%.2f a second), %.1f s, %.1f MB\n", out, pics, FPS * m / n,
		       frames / FPS, ufsize(out) / 1e6);
	free(audio);
	free(p);
	return 0;
}
