/*
 * uzm -- decoding through FFmpeg's libraries, the way the ffmpeg command line (fftools 8.0) does
 * it for `ffmpeg [-ss S] [-t D] -i IN -vf CHAIN -f rawvideo -` and the like, so that the pictures
 * and the sound are the ones uzm.py got from the ffmpeg program:
 *
 *  - the stream: the video stream with the most pixels, the audio stream with the most channels
 *    (a default one first), or v:0 / a:n when asked (ffmpeg_mux_init.c, map_auto_*);
 *  - the cut (ffmpeg_demux.c): a seek to S with every stream present, so the format seeks by
 *    its default stream (3/23 s earlier when a stream reorders and the format's timestamps are
 *    DTS-like), every packet's timestamps moved by -(S + the file's start time), and a trim
 *    filter (starti 0 when S is given, durationi D) in front of the graph;
 *  - a frame's timestamp (ffmpeg_dec.c): video, the decoder's best effort (else the last one's
 *    plus its estimated duration); audio, its own, through av_rescale_delta into 1/sample rate;
 *  - the graph (ffmpeg_filter.c): made in ffmpeg's order (the chain, then the source, its
 *    rotation from the frame's display matrix and the trim, then the output's format filter
 *    and the sink), frames pushed in the source's time base, the source closed at the end at
 *    the last frame's time plus its duration;
 *  - the output (fg_output_frame): the pictures made constant-rate, as for the rawvideo muxer.
 * Not reproduced: timestamp discontinuities (MPEG-TS wraps) and a stream whose format changes
 * on the way (ffmpeg rebuilds its graph; here a warning).
 */
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/avstring.h>
#include <libavutil/bprint.h>
#include <libavutil/channel_layout.h>
#include <libavutil/display.h>
#include <libavutil/opt.h>
#include <libavutil/parseutils.h>
#include <libavutil/pixdesc.h>

#include "media.h"
#include "uzm.h"

/* the graph segment API (the order ffmpeg builds its graphs in) and AVFrame.duration */
#if LIBAVFILTER_VERSION_INT < AV_VERSION_INT(9, 3, 100)
#error "uzm needs FFmpeg 6.0 or newer"
#endif

/* ---- messages ---- */
static int verbose_level = AV_LOG_ERROR;
static int capturing;
static AVBPrint captured;

static void log_cb(void *avcl, int level, const char *fmt, va_list vl)
{
	if (capturing && level <= AV_LOG_INFO) {
		va_list vc;
		va_copy(vc, vl);
		av_vbprintf(&captured, fmt, vc);
		va_end(vc);
	}
	if (level <= verbose_level) av_log_default_callback(avcl, level, fmt, vl);
}

void media_init(int verbose)
{
	verbose_level = verbose ? AV_LOG_INFO : AV_LOG_ERROR;
	av_log_set_level(AV_LOG_INFO);
	av_log_set_callback(log_cb);
	av_bprint_init(&captured, 0, AV_BPRINT_SIZE_UNLIMITED);
}

void media_capture(int on)
{
	if (on) av_bprint_clear(&captured);
	capturing = on;
}

const char *media_captured(void) { return captured.str; }

int64_t parse_time_us(const char *s, const char *what)
{
	int64_t us;
	if (av_parse_time(&us, s, 1) < 0) die("%s: not a time: %s", what, s);
	return us;
}

static void check(int ret, const char *what)
{
	if (ret < 0) die("%s: %s", what, av_err2str(ret));
}

/* ---- the stream ---- */
static int pick_stream(AVFormatContext *ic, enum AVMediaType type, int pick)
{
	int best = -1, nth = 0;
	int64_t best_score = -1;
	for (unsigned i = 0; i < ic->nb_streams; i++) {
		AVStream *st = ic->streams[i];
		AVCodecParameters *par = st->codecpar;
		if (par->codec_type != type) continue;
		if (pick == PICK_FIRST) return (int)i;
		if (pick >= 0) {
			if (nth++ == pick) return (int)i;
			continue;
		}
		int64_t score = type == AVMEDIA_TYPE_VIDEO ? (int64_t)par->width * par->height
		                                           : par->ch_layout.nb_channels;
		score += 100000000 * !!(st->event_flags & AVSTREAM_EVENT_FLAG_NEW_PACKETS);
		score += 5000000 * !!(st->disposition & AV_DISPOSITION_DEFAULT);
		if (type == AVMEDIA_TYPE_VIDEO && (st->disposition & AV_DISPOSITION_ATTACHED_PIC)) score = 1;
		if (score > best_score) {
			best_score = score;
			best = (int)i;
		}
	}
	return best;
}

int media_has_stream(const char *path, int audio)
{
	AVFormatContext *ic = NULL;
	if (avformat_open_input(&ic, path, NULL, NULL) < 0) die("can't open %s", path);
	if (avformat_find_stream_info(ic, NULL) < 0) die("%s: no stream information", path);
	int si = pick_stream(ic, audio ? AVMEDIA_TYPE_AUDIO : AVMEDIA_TYPE_VIDEO, PICK_BEST);
	avformat_close_input(&ic);
	return si >= 0;
}

AVRational media_frame_rate(const char *path)
{
	AVFormatContext *ic = NULL;
	AVRational r = { 0, 1 };
	if (avformat_open_input(&ic, path, NULL, NULL) < 0) die("can't open %s", path);
	if (avformat_find_stream_info(ic, NULL) < 0) die("%s: no stream information", path);
	int si = pick_stream(ic, AVMEDIA_TYPE_VIDEO, PICK_FIRST);
	if (si >= 0) r = ic->streams[si]->r_frame_rate;
	if (r.num <= 0 || r.den <= 0) r = (AVRational){ 0, 1 };
	avformat_close_input(&ic);
	return r;
}

/* ---- the run: demux, decode, filter ---- */
typedef struct {
	enum AVMediaType type;
	const Cut *cut;
	const char *desc, *outfmt;
	const AVFrame *pal;
	FrameFn fn;
	void *opaque;
	AVFormatContext *ic;
	AVStream *st;
	AVCodecContext *dec;
	int ts_unreliable;
	AVFilterGraph *graph;
	AVFilterContext *src, *pal_src, *sink;
	AVFrame *out;
	AVRational tb;                  /* the source's time base (the first frame's) */
	int w, h, fmt, rate;            /* what the graph was made for */
	int done;                       /* the sink is at its end, or fn said stop */
	int warned;
	/* the decoder's timestamp history (ffmpeg_dec.c) */
	int64_t last_pts, last_dur;
	AVRational last_tb;
	int last_rate;
	int64_t rescale_delta;
	/* the output's frame rate (CFR / VSCFR) */
	int vscfr, got_frame;
	AVRational tb_out;
	int64_t next_pts, frame_number, prev_hist[3];
	AVFrame *last_frame;
} Run;

/* a filter between *last and the graph, as ffmpeg's insert_filter */
static void insert_filter(Run *r, AVFilterContext **last, const char *name, const char *args)
{
	AVFilterContext *f;
	check(avfilter_graph_create_filter(&f, avfilter_get_by_name(name), NULL, args, NULL, r->graph),
	      name);
	check(avfilter_link(*last, 0, f, 0), name);
	*last = f;
}

/* ffmpeg's autorotation (cmdutils.c's get_rotation, ffmpeg_filter.c's filters) */
static void autorotate(Run *r, AVFilterContext **last, const int32_t *m)
{
	double theta = -round(av_display_rotation_get(m));
	theta -= 360 * floor(theta / 360 + 0.9 / 360);
	if (fabs(theta - 90) < 1.0) {
		insert_filter(r, last, "transpose", m[3] > 0 ? "cclock_flip" : "clock");
	} else if (fabs(theta - 180) < 1.0) {
		if (m[0] < 0) insert_filter(r, last, "hflip", NULL);
		if (m[4] < 0) insert_filter(r, last, "vflip", NULL);
	} else if (fabs(theta - 270) < 1.0) {
		insert_filter(r, last, "transpose", m[3] < 0 ? "clock_flip" : "cclock");
	} else if (fabs(theta) > 1.0) {
		char buf[64];
		snprintf(buf, sizeof buf, "%f*PI/180", theta);
		insert_filter(r, last, "rotate", buf);
	} else if (m[4] < 0) {
		insert_filter(r, last, "vflip", NULL);
	}
}

/* The graph, built in ffmpeg_filter.c's order (the order its filters are made in decides where
 * the format negotiation puts the conversions): the chain first (unlabelled, or [in] and [pal]
 * for paletteuse), then each input's source, its rotation and the cut's trim, then the sink and
 * the output's format filter (r->outfmt, as -pix_fmt / -ar / -ac make it). */
static AVFilterContext *video_source(Run *r, AVFrame *f)
{
	AVFilterContext *src = avfilter_graph_alloc_filter(r->graph, avfilter_get_by_name("buffer"), "in");
	if (!src) die("out of memory");
	AVBufferSrcParameters *par = av_buffersrc_parameters_alloc();
	par->format = f->format;
	par->time_base = r->tb;
	par->frame_rate = av_guess_frame_rate(r->ic, r->st, NULL);
	par->width = f->width;
	par->height = f->height;
	par->sample_aspect_ratio = f->sample_aspect_ratio.den > 0 ? f->sample_aspect_ratio
	                                                         : (AVRational){ 0, 1 };
#if LIBAVFILTER_VERSION_INT >= AV_VERSION_INT(10, 4, 100)
	par->color_space = f->colorspace;
	par->color_range = f->color_range;
#endif
	check(av_buffersrc_parameters_set(src, par), "the video source");
	av_free(par);
	check(avfilter_init_dict(src, NULL), "the video source");
	r->w = f->width;
	r->h = f->height;
	r->fmt = f->format;
	return src;
}

static AVFilterContext *audio_source(Run *r, AVFrame *f)
{
	AVFilterContext *src;
	AVBPrint a;
	av_bprint_init(&a, 0, AV_BPRINT_SIZE_AUTOMATIC);
	av_bprintf(&a, "time_base=%d/%d:sample_rate=%d:sample_fmt=%s", r->tb.num, r->tb.den,
	           f->sample_rate, av_get_sample_fmt_name(f->format));
	if (av_channel_layout_check(&f->ch_layout) && f->ch_layout.order != AV_CHANNEL_ORDER_UNSPEC) {
		av_bprintf(&a, ":channel_layout=");
		av_channel_layout_describe_bprint(&f->ch_layout, &a);
	} else {
		av_bprintf(&a, ":channels=%d", f->ch_layout.nb_channels);
	}
	int ret = avfilter_graph_create_filter(&src, avfilter_get_by_name("abuffer"), "in", a.str, NULL,
	                                       r->graph);
	av_bprint_finalize(&a, NULL);
	check(ret, "the audio source");
	r->rate = f->sample_rate;
	r->fmt = f->format;
	return src;
}

static void make_graph(Run *r, AVFrame *f)
{
	char args[512];
	int ret;
	r->graph = avfilter_graph_alloc();
	if (!r->graph) die("out of memory");
	r->tb = f->time_base;

	/* the chain */
	AVFilterGraphSegment *seg = NULL;
	AVFilterInOut *ins = NULL, *outs = NULL;
	ret = avfilter_graph_segment_parse(r->graph, r->desc, 0, &seg);
	if (ret >= 0) ret = avfilter_graph_segment_create_filters(seg, 0);
	if (ret >= 0) ret = avfilter_graph_segment_apply(seg, 0, &ins, &outs);
	avfilter_graph_segment_free(&seg);
	if (ret < 0) die("the filters \"%s\": %s", r->desc, av_err2str(ret));
	if (!ins || !outs || outs->next) die("the filters \"%s\": not one chain", r->desc);

	/* the inputs: the stream (rotated, cut), and the palette's picture */
	for (AVFilterInOut *in = ins; in; in = in->next) {
		AVFilterContext *head;
		if (in->name && !strcmp(in->name, "pal")) {
			if (!r->pal) die("the filters use a palette that isn't there");
			const AVFrame *p = r->pal;
			snprintf(args, sizeof args, "video_size=%dx%d:pix_fmt=%d:time_base=1/25:pixel_aspect=1/1",
			         p->width, p->height, p->format);
			check(avfilter_graph_create_filter(&r->pal_src, avfilter_get_by_name("buffer"), "pal",
			                                   args, NULL, r->graph), "the palette's source");
			head = r->pal_src;
		} else {
			r->src = r->type == AVMEDIA_TYPE_VIDEO ? video_source(r, f) : audio_source(r, f);
			head = r->src;
			if (r->type == AVMEDIA_TYPE_VIDEO) {
				const AVFrameSideData *sd = av_frame_get_side_data(f, AV_FRAME_DATA_DISPLAYMATRIX);
				if (sd) autorotate(r, &head, (const int32_t *)sd->data);
			}
			/* the cut's trim, as ffmpeg's insert_trim: durationi, then starti 0 with a start */
			if (r->cut->start_us != AV_NOPTS_VALUE || r->cut->duration_us != AV_NOPTS_VALUE) {
				AVFilterContext *trim = avfilter_graph_alloc_filter(
					r->graph, avfilter_get_by_name(r->type == AVMEDIA_TYPE_VIDEO ? "trim" : "atrim"),
					"trim_in");
				if (!trim) die("out of memory");
				if (r->cut->duration_us != AV_NOPTS_VALUE)
					check(av_opt_set_int(trim, "durationi", r->cut->duration_us, AV_OPT_SEARCH_CHILDREN),
					      "the cut");
				if (r->cut->start_us != AV_NOPTS_VALUE)
					check(av_opt_set_int(trim, "starti", 0, AV_OPT_SEARCH_CHILDREN), "the cut");
				check(avfilter_init_str(trim, NULL), "the cut");
				check(avfilter_link(head, 0, trim, 0), "the cut");
				head = trim;
			}
		}
		check(avfilter_link(head, 0, in->filter_ctx, (unsigned)in->pad_idx), "the graph's input");
	}
	if (!r->src) die("the filters \"%s\" don't read the stream", r->desc);

	/* the output: the sink, the format filter before it */
	int video = r->type == AVMEDIA_TYPE_VIDEO;
	check(avfilter_graph_create_filter(&r->sink, avfilter_get_by_name(video ? "buffersink" : "abuffersink"),
	                                   "out", NULL, NULL, r->graph), "the sink");
	AVFilterContext *last = outs->filter_ctx;
	unsigned pad = (unsigned)outs->pad_idx;
	if (r->outfmt && *r->outfmt) {
		AVFilterContext *fmt;
		check(avfilter_graph_create_filter(&fmt, avfilter_get_by_name(video ? "format" : "aformat"),
		                                   "format", r->outfmt, NULL, r->graph), "the output's format");
		check(avfilter_link(last, pad, fmt, 0), "the output's format");
		last = fmt;
		pad = 0;
	}
	check(avfilter_link(last, pad, r->sink, 0), "the sink");
	avfilter_inout_free(&ins);
	avfilter_inout_free(&outs);

	ret = avfilter_graph_config(r->graph, NULL);
	if (ret < 0) die("the filters \"%s\": %s", r->desc, av_err2str(ret));
	if (r->pal_src) {
		AVFrame *p = av_frame_clone(r->pal);
		p->pts = 0;
		check(av_buffersrc_add_frame(r->pal_src, p), "the palette");
		av_frame_free(&p);
		check(av_buffersrc_close(r->pal_src, 1, 0), "the palette");
	}
}

/* ---- the output's frame rate (ffmpeg_filter.c's video_sync_process, fg_output_frame) ----
 * The rawvideo muxer has no variable frame rate, so ffmpeg makes the pictures constant-rate
 * (CFR) in 1/the sink's frame rate: a picture late by more than 1.1 is repeated, one early by
 * more than 1.1 dropped; VSCFR (a file of one stream, a simple graph) does not repeat the first
 * picture to fill the time before it. At the end, the last picture again as many times as the
 * median of the last three repeats of the one before. */
static int64_t mid3(int64_t a, int64_t b, int64_t c)
{
	if (a > b) { int64_t t = a; a = b; b = t; }
	return c < a ? a : c > b ? b : c;
}

static void emit(Run *r, AVFrame *f)
{
	if (!r->done && r->fn(r->opaque, f)) r->done = 1;
}

static void sync_frame(Run *r, AVFrame *f)
{
	int64_t nb = f ? 1 : 0, nb_prev = 0;
	if (r->type != AVMEDIA_TYPE_VIDEO) {
		if (f) emit(r, f);
		return;
	}
	if (!f && !r->got_frame) return;
	if (!f) {
		nb = nb_prev = mid3(r->prev_hist[0], r->prev_hist[1], r->prev_hist[2]);
	} else {
		if (!r->tb_out.num) {
			AVRational fr = av_buffersink_get_frame_rate(r->sink);
			if (fr.num <= 0 || fr.den <= 0) fr = (AVRational){ 25, 1 };
			r->tb_out = av_inv_q(fr);
		}
		AVRational ftb = av_buffersink_get_time_base(r->sink);
		double duration = f->duration * av_q2d(ftb) / av_q2d(r->tb_out);
		double sync_ipts = AV_NOPTS_VALUE;
		if (f->pts != AV_NOPTS_VALUE) {
			AVRational tb = r->tb_out;
			int extra = av_clip(29 - av_log2(tb.den), 0, 16);
			tb.den <<= extra;
			sync_ipts = (double)av_rescale_q(f->pts, ftb, tb);
			sync_ipts /= 1 << extra;
			if (sync_ipts != llrint(sync_ipts)) sync_ipts += FFSIGN(sync_ipts) * 1.0 / (1 << 17);
		}
		double delta0 = sync_ipts - r->next_pts, delta = delta0 + duration;
		if (delta0 < 0 && delta > 0) {
			sync_ipts = (double)r->next_pts;
			duration += delta0;
			delta0 = 0;
		}
		if (r->vscfr && r->frame_number == 0 && delta0 >= 0.5) {
			delta = duration;
			delta0 = 0;
			r->next_pts = llrint(sync_ipts);
		}
		if (delta < -1.1) nb = 0;
		else if (delta > 1.1) {
			nb = llrintf((float)delta);
			if (delta0 > 1.1) nb_prev = llrintf((float)(delta0 - 0.6));
		}
	}
	r->prev_hist[2] = r->prev_hist[1];
	r->prev_hist[1] = r->prev_hist[0];
	r->prev_hist[0] = nb_prev;
	if (nb > 3600 * 30) nb = 0;                     /* "frame duplication too large" */
	for (int64_t i = 0; i < nb && !r->done; i++) {
		AVFrame *in = i < nb_prev && r->last_frame->buf[0] ? r->last_frame : f;
		if (!in) break;
		emit(r, in);
		r->frame_number++;
		r->next_pts++;
		r->got_frame = 1;
	}
	if (f) {
		av_frame_unref(r->last_frame);
		av_frame_move_ref(r->last_frame, f);
	}
}

static void drain(Run *r)
{
	while (!r->done) {
		int ret = av_buffersink_get_frame(r->sink, r->out);
		if (ret == AVERROR(EAGAIN)) return;
		if (ret == AVERROR_EOF) {
			sync_frame(r, NULL);
			r->done = 1;
			return;
		}
		check(ret, "filtering");
		sync_frame(r, r->out);
		av_frame_unref(r->out);
	}
}

/* ffmpeg_dec.c's audio_samplerate_update and audio_ts_process */
static void audio_ts(Run *r, AVFrame *f)
{
	AVRational tb_filter = { 1, f->sample_rate };
	if (f->sample_rate != r->last_rate) {
		int prev = r->last_tb.den, sr = f->sample_rate;
		int64_t g = av_gcd(prev, sr);
		AVRational tb_new = prev / g >= INT_MAX / sr ? (AVRational){ 1, 28224000 }
		                                             : (AVRational){ 1, (int)(prev / g * sr) };
		if (f->time_base.num == 1 && f->time_base.den > tb_new.den && !(f->time_base.den % tb_new.den))
			tb_new = f->time_base;
		if (r->last_pts != AV_NOPTS_VALUE) r->last_pts = av_rescale_q(r->last_pts, r->last_tb, tb_new);
		r->last_dur = av_rescale_q(r->last_dur, r->last_tb, tb_new);
		r->last_tb = tb_new;
		r->last_rate = f->sample_rate;
	}
	AVRational tb = r->last_tb;
	int64_t pred = r->last_pts == AV_NOPTS_VALUE ? 0 : r->last_pts + r->last_dur;
	if (f->pts == AV_NOPTS_VALUE) {
		f->pts = pred;
		f->time_base = tb;
	} else if (r->last_pts != AV_NOPTS_VALUE &&
	           f->pts > av_rescale_q_rnd(pred, tb, f->time_base, AV_ROUND_UP)) {
		r->rescale_delta = AV_NOPTS_VALUE;          /* a gap: the conversion starts again */
	}
	f->pts = av_rescale_delta(f->time_base, f->pts, tb, f->nb_samples, &r->rescale_delta, tb);
	r->last_pts = f->pts;
	r->last_dur = av_rescale_q(f->nb_samples, tb_filter, tb);
	f->pts = av_rescale_q(f->pts, tb, tb_filter);
	f->duration = f->nb_samples;
	f->time_base = tb_filter;
}

/* ffmpeg_dec.c's video_duration_estimate */
static int64_t video_duration(Run *r, const AVFrame *f)
{
	int64_t ts_diff = f->pts != AV_NOPTS_VALUE && r->last_pts != AV_NOPTS_VALUE ? f->pts - r->last_pts : -1;
	int unreliable = f->duration == 1 && ts_diff > 2 * f->duration;
	if (f->duration > 0 && !r->ts_unreliable && !unreliable) return f->duration;
	int64_t codec = 0;
	if (r->dec->framerate.den && r->dec->framerate.num) {
		AVRational field_rate = av_mul_q(r->dec->framerate, (AVRational){ 2, 1 });
		codec = av_rescale_q(f->repeat_pict + 2, av_inv_q(field_rate), f->time_base);
	}
	if (codec > 0 && r->ts_unreliable) return codec;
	if (ts_diff > 0) return ts_diff;
	if (f->duration > 0) return f->duration;
	if (codec > 0) return codec;
	AVRational fr = r->st->avg_frame_rate;
	if (fr.num && fr.den) {
		int64_t d = av_rescale_q(1, av_inv_q(fr), f->time_base);
		if (d > 0) return d;
	}
	return r->last_dur > 1 ? r->last_dur : 1;
}

static void feed(Run *r, AVFrame *f)
{
	f->time_base = r->dec->pkt_timebase;
	if (r->type == AVMEDIA_TYPE_AUDIO) {
		audio_ts(r, f);
	} else {
		f->pts = f->best_effort_timestamp;
		if (f->pts == AV_NOPTS_VALUE) f->pts = r->last_pts == AV_NOPTS_VALUE ? 0 : r->last_pts + r->last_dur;
		r->last_dur = video_duration(r, f);
		r->last_pts = f->pts;
		r->last_tb = f->time_base;
	}
	if (!r->graph) make_graph(r, f);
	else if (!r->warned && (r->type == AVMEDIA_TYPE_VIDEO
	                            ? f->width != r->w || f->height != r->h || f->format != r->fmt
	                            : f->sample_rate != r->rate || f->format != r->fmt)) {
		fprintf(stderr, "uzm: warning: the stream's format changes on the way\n");
		r->warned = 1;
	}
	f->pts = av_rescale_q(f->pts, f->time_base, r->tb);
	f->duration = av_rescale_q(f->duration, f->time_base, r->tb);
	f->time_base = r->tb;
	int ret = av_buffersrc_add_frame_flags(r->src, f, AV_BUFFERSRC_FLAG_PUSH);
	if (ret < 0 && ret != AVERROR_EOF) check(ret, "filtering");
	drain(r);
}

static void decode(Run *r, AVPacket *pkt, AVFrame *f)
{
	if (pkt && pkt->size == 0) return;
	if (pkt && r->ts_unreliable) pkt->pts = pkt->dts = AV_NOPTS_VALUE;
	int ret = avcodec_send_packet(r->dec, pkt);
	if (ret < 0 && ret != AVERROR_EOF) {
		if (ret == AVERROR(EAGAIN)) die("the decoder refused a packet");
		fprintf(stderr, "uzm: warning: a bad packet (%s)\n", av_err2str(ret));
		return;
	}
	while (!r->done) {
		ret = avcodec_receive_frame(r->dec, f);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) return;
		if (ret < 0) {
			fprintf(stderr, "uzm: warning: a frame did not decode (%s)\n", av_err2str(ret));
			return;
		}
		feed(r, f);
		av_frame_unref(f);
	}
}

static void run(enum AVMediaType type, const Cut *cut, int pick, const char *desc,
                const char *outfmt, const AVFrame *pal, FrameFn fn, void *opaque)
{
	Run r;
	memset(&r, 0, sizeof r);
	r.type = type;
	r.cut = cut;
	r.desc = desc;
	r.outfmt = outfmt;
	r.pal = pal;
	r.fn = fn;
	r.opaque = opaque;
	r.last_pts = AV_NOPTS_VALUE;
	r.last_tb = (AVRational){ 1, 1 };
	r.rescale_delta = AV_NOPTS_VALUE;
	const char *kind = type == AVMEDIA_TYPE_VIDEO ? "video" : "audio";

	int ret = avformat_open_input(&r.ic, cut->path, NULL, NULL);
	if (ret < 0) die("can't open %s: %s", cut->path, av_err2str(ret));
	AVFormatContext *ic = r.ic;
	ret = avformat_find_stream_info(ic, NULL);
	if (ret < 0) die("%s: %s", cut->path, av_err2str(ret));
	int si = pick_stream(ic, type, pick);
	if (si < 0) {
		if (pick >= 0) die("%s: no %s stream %d", cut->path, kind, pick);
		die("%s: no %s stream", cut->path, kind);
	}
	r.st = ic->streams[si];
	r.ts_unreliable = !!(ic->iformat->flags & AVFMT_NOTIMESTAMPS);
	r.vscfr = !pal && ic->nb_streams == 1;     /* ffmpeg: a simple graph from a file of one stream */
	r.last_frame = av_frame_alloc();

	const AVCodec *codec = avcodec_find_decoder(r.st->codecpar->codec_id);
	if (!codec) die("%s: no decoder for its %s (%s)", cut->path, kind,
	                avcodec_get_name(r.st->codecpar->codec_id));
	r.dec = avcodec_alloc_context3(codec);
	check(avcodec_parameters_to_context(r.dec, r.st->codecpar), "the decoder");
	r.dec->pkt_timebase = r.st->time_base;
	AVDictionary *opts = NULL;
	av_dict_set(&opts, "threads", "auto", 0);
	ret = avcodec_open2(r.dec, codec, &opts);
	av_dict_free(&opts);
	if (ret < 0) die("%s: the %s decoder: %s", cut->path, kind, av_err2str(ret));

	/* ffmpeg's seek (every stream still there) and timestamp offset */
	int64_t timestamp = (cut->start_us == AV_NOPTS_VALUE ? 0 : cut->start_us) +
	                    (ic->start_time != AV_NOPTS_VALUE ? ic->start_time : 0);
	if (cut->start_us != AV_NOPTS_VALUE) {
		int64_t seek = timestamp;
		if (!(ic->iformat->flags & AVFMT_SEEK_TO_PTS))
			for (unsigned i = 0; i < ic->nb_streams; i++)
				if (ic->streams[i]->codecpar->video_delay) {
					seek -= 3 * AV_TIME_BASE / 23;
					break;
				}
		ret = avformat_seek_file(ic, -1, INT64_MIN, seek, seek, 0);
		if (ret < 0)
			fprintf(stderr, "uzm: warning: %s: could not seek to %.3f s\n", cut->path,
			        (double)timestamp / AV_TIME_BASE);
	}
	for (unsigned i = 0; i < ic->nb_streams; i++)
		if ((int)i != si) ic->streams[i]->discard = AVDISCARD_ALL;

	AVPacket *pkt = av_packet_alloc();
	AVFrame *f = av_frame_alloc();
	r.out = av_frame_alloc();
	while (!r.done) {
		ret = av_read_frame(ic, pkt);
		if (ret == AVERROR_EOF) break;
		if (ret < 0) {
			fprintf(stderr, "uzm: warning: reading %s stopped: %s\n", cut->path, av_err2str(ret));
			break;
		}
		if (pkt->stream_index == si) {
			int64_t off = av_rescale_q(-timestamp, AV_TIME_BASE_Q, r.st->time_base);
			if (pkt->dts != AV_NOPTS_VALUE) pkt->dts += off;
			if (pkt->pts != AV_NOPTS_VALUE) pkt->pts += off;
			decode(&r, pkt, f);
		}
		av_packet_unref(pkt);
	}
	if (!r.done) decode(&r, NULL, f);
	if (!r.graph) die("%s: its %s gave no frames", cut->path, kind);
	if (!r.done) {
		int64_t end = r.last_pts == AV_NOPTS_VALUE ? AV_NOPTS_VALUE : r.last_pts + r.last_dur;
		end = av_rescale_q_rnd(end, r.last_tb, r.tb, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
		check(av_buffersrc_close(r.src, end, AV_BUFFERSRC_FLAG_PUSH), "filtering");
		drain(&r);
	}
	av_frame_free(&f);
	av_frame_free(&r.out);
	av_frame_free(&r.last_frame);
	av_packet_free(&pkt);
	avfilter_graph_free(&r.graph);
	avcodec_free_context(&r.dec);
	avformat_close_input(&r.ic);
}

void media_video(const Cut *cut, int pick, const char *desc, const char *outfmt,
                 const AVFrame *pal, FrameFn fn, void *opaque)
{
	run(AVMEDIA_TYPE_VIDEO, cut, pick, desc, outfmt, pal, fn, opaque);
}

void media_audio(const Cut *cut, int pick, const char *desc, const char *outfmt, FrameFn fn,
                 void *opaque)
{
	run(AVMEDIA_TYPE_AUDIO, cut, pick, desc, outfmt, NULL, fn, opaque);
}
