/*
 * uzm -- decoding a file through FFmpeg's libraries: a stream, a cut (-ss / -t), a filter graph,
 * the frames that come out. Behaves as the ffmpeg command line does with the same options, so
 * the pictures and the sound are the ones uzm.py got from it.
 */
#ifndef UZM_MEDIA_H
#define UZM_MEDIA_H

#include <libavutil/frame.h>
#include <libavutil/rational.h>

typedef struct {
	const char *path;
	int64_t start_us;       /* -ss (microseconds), or AV_NOPTS_VALUE */
	int64_t duration_us;    /* -t, or AV_NOPTS_VALUE */
} Cut;

enum { PICK_BEST = -1, PICK_FIRST = -2 };       /* the stream: as ffmpeg picks it / v:0 */

/* each frame out of the graph; a non-zero return stops */
typedef int (*FrameFn)(void *opaque, AVFrame *f);

/* desc: the filters, one chain from the stream (cut) to the output (ffmpeg's -vf / -af), or
 * with paletteuse [in] and [pal] (the picture pal); outfmt: the output's format filter's
 * options, as ffmpeg makes them from -pix_fmt / -ar / -ac ("pix_fmts=rgb24:"), or NULL.
 * pick: PICK_BEST, PICK_FIRST, or the n-th stream of the kind (ffmpeg's -map 0:a:n). */
void media_video(const Cut *cut, int pick, const char *desc, const char *outfmt,
                 const AVFrame *pal, FrameFn fn, void *opaque);
void media_audio(const Cut *cut, int pick, const char *desc, const char *outfmt, FrameFn fn,
                 void *opaque);

/* whether path has an audio (1) or video (0) stream ffmpeg would pick */
int media_has_stream(const char *path, int audio);

/* the first video stream's r_frame_rate (ffprobe's), {0, 1} when unknown */
AVRational media_frame_rate(const char *path);

/* the libraries' messages (info and above) kept in a buffer, e.g. loudnorm's measurement */
void media_capture(int on);
const char *media_captured(void);

void media_init(int verbose);
int64_t parse_time_us(const char *s, const char *what);

#endif
