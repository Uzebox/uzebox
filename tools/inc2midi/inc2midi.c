/*
 *  Uzebox(tm Alec Bourque)
 *  inc2midi utility
 *  2025 Lee Weber
 *
 *  GPLv3-or-later
 *
 *  Summary:
 *	- Converts Uzebox midiconv *.inc arrays back to a standard MIDI file.
 *	- Optional: reads the mconvert "compressed stream" with --compressed.
 *	- Preserves original 60 Hz frame timing exactly by default:
 *		Default = --quant 60, --bpm 225.0  -> PPQ chosen so frames fall on musical grid.
 *		(Result: all events align to 1/16 notes; 1/16 = 4 frames)
 *	- Optional: --grid N forces PPQ to be a multiple of N (no timing change).
 *	- Optional: --snap N snaps events to nearest 1/N notes (changes timing).
 *	- Optional: --type 0|1 chooses MIDI Type 0 (default) or Type 1 (multitrack by channel).
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>


typedef struct{
	uint8_t *d;
	size_t n, cap;
}Buf;


static void binit(Buf *b){
	b->d = NULL;
	b->n = 0;
	b->cap = 0;
}


static void bfree(Buf *b){
	free(b->d);
	b->d = NULL;
	b->n = b->cap = 0;
}


static int bres(Buf *b, size_t add){
	if(b->n + add <= b->cap)
		return 1;
	size_t nc = b->cap ? b->cap : 2048;
	while(nc < b->n + add)
		nc <<= 1;
	uint8_t *p = (uint8_t *) realloc(b->d, nc);
	if(!p)
		return 0;
	b->d = p;
	b->cap = nc;
	return 1;
}


static int bput(Buf *b, uint8_t v){
	if(!bres(b, 1))
		return 0;
	b->d[b->n++] = v;
	return 1;
}


static int bwr(Buf *b, const void *s, size_t n){
	if(!bres(b, n))
		return 0;
	memcpy(b->d + b->n, s, n);
	b->n += n;
	return 1;
}


static int read_vlq(const uint8_t *p, const uint8_t *e, uint32_t *out, const uint8_t **adv){
	uint32_t v = 0;
	const uint8_t *q = p;
	if(q >= e)
		return 0;
	uint8_t c = *q++;
	if(!(c & 0x80)){
		*out = c;
		*adv = q;
		return 1;
	}
	v = c & 0x7F;
	while(1){
		if(q >= e)
			return 0;
		c = *q++;
		v = (v << 7) | (c & 0x7F);
		if(!(c & 0x80))
			break;
	}
	*out = v;
	*adv = q;
	return 1;
}


static int write_vlq(Buf *b, uint32_t v){
	uint8_t tmp[5];
	int i = 0;
	tmp[i++] = (uint8_t)(v & 0x7F);
	v >>= 7;
	while(v){
		tmp[i++] = (uint8_t)(0x80 | (v & 0x7F));
		v >>= 7;
	}
	for(int j = i - 1; j >= 0; --j){
		if(!bput(b, tmp[j]))
			return 0;
	}
	return 1;
}


typedef struct{
	int index; /* which {...} initializer to read */
	int ppq; /* pulses per quarter (explicit, optional) */
	int quant; /* ticks per frame (time-preserving grid); if >0, PPQ derived from BPM */
	double bpm; /* beats per minute (can be fractional) */
	int grid; /* force PPQ multiple-of grid (no timing change) */
	int snap; /* quantize to 1/snap notes (changes timing) */
	int type; /* MIDI file type: 0 or 1 */
	bool no_mono; /* disable per-channel monophonic cleanup */
	bool summary; /* print per-channel note counts */
	bool compressed; /* input bytes are mconvert compressed stream */
	bool verbose;
}Opt;


static void usage(const char *exe){
	fprintf(stderr,
		"Usage: %s input.inc output.mid [options]\n"
		"  -i N, --index N	Select N-th array initializer (default 0)\n"
		"\n"
		"  Timing (defaults map 60 Hz frames to musical grid):\n"
		"	--quant Q	Ticks per 60 Hz frame (time-preserving). Default 60\n"
		"	--bpm B		BPM for grid math (fractional ok). Default 225.0\n"
		"	--ppq P		Explicit PPQ (only if --quant not provided)\n"
		"	--grid N	Make PPQ a multiple of N (no timing change)\n"
		"	--snap N	Snap events to nearest 1/N note (changes timing)\n"
		"\n"
		"  MIDI file:\n"
		"	--type 0|1	Output MIDI Type 0 (default) or Type 1 (per-channel tracks)\n"
		"\n"
		"  Input format:\n"
		"	--compressed	Treat bytes as mconvert compressed stream (default is raw midiconv)\n"
		"\n"
		"  Behavior:\n"
		"	--no-mono	Disable per-channel monophonic cleanup (default: enabled)\n"
		"	--summary	Print per-channel note counts\n"
		"	--verbose	Print mapping and diagnostic info\n", exe);
}


static char *slurp(const char *path, size_t *len){
	FILE *f = fopen(path, "rb");
	if(!f)
		return NULL;
	fseek(f, 0, SEEK_END);
	long L = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buf = (char *) malloc((size_t) L + 1);
	if(!buf){
		fclose(f);
		return NULL;
	}
	if(L && fread(buf, 1, (size_t) L, f) != (size_t) L){
		free(buf);
		fclose(f);
		return NULL;
	}
	buf[L] = 0;
	if(len)
		*len = (size_t) L;
	fclose(f);
	return buf;
}


static int parse_array_bytes(const char *text, int want_idx, uint8_t **out, size_t *outlen, bool verbose){
	int idx = -1;
	const char *p = text;
	int depth = 0;
	bool in_line = false, in_block = false;
	Buf b;
	binit(&b);
	while(*p){
		if(!in_line && !in_block && *p == '/' && p[1] == '/'){
			in_line = true;
			p += 2;
			continue;
		}
		if(!in_line && !in_block && *p == '/' && p[1] == '*'){
			in_block = true;
			p += 2;
			continue;
		}
		if(in_line && ( *p == '\n' || *p == '\r')){
			in_line = false;
			p++;
			continue;
		}
		if(in_block && *p == '*' && p[1] == '/'){
			in_block = false;
			p += 2;
			continue;
		}
		if(in_line || in_block){
			p++;
			continue;
		}

		if( *p == '{'){
			depth++;
			if(depth == 1){
				idx++;
				b.n = 0;
			}
			p++;
			continue;
		}
		if( *p == '}' && depth > 0){
			depth--;
			if(depth == 0 && idx == want_idx){
				*out = b.d;
				*outlen = b.n;
				return 1;
			}
			p++;
			continue;
		}
		if(depth == 1){
			while(isspace((unsigned char) *p) || *p == ',')
				p++;
			if( *p == '}')
				continue;
			if(p[0] == '0' && (p[1] == 'x' || p[1] == 'X')){
				char *end = NULL;
				long v = strtol(p, & end, 16);
				if(p == end){
					p++;
					continue;
				}
				if(v < 0)
					v = 0;
				if(v > 255)
					v &= 0xFF;
				if(!bput(&b, (uint8_t) v)){
					bfree(&b);
					return 0;
				}
				p = end;
				continue;
			}else if(p[0] == '0' && (p[1] == 'b' || p[1] == 'B')){
				p += 2;
				int bits = 0, val = 0;
				while(bits < 8 && ( *p == '0' || *p == '1')){
					val = (val << 1) | ( *p == '1');
					bits++;
					p++;
				}
				if(bits > 0){
					if(!bput(&b, (uint8_t) val)){
						bfree(&b);
						return 0;
					}
				}
				while( *p && *p != ',' && *p != '}' && !isspace((unsigned char) *p))
					p++;
				continue;
			}else if(isdigit((unsigned char) *p)){
				char *end = NULL;
				long v = strtol(p, &end, 10);
				if(p == end){
					p++;
					continue;
				}
				if(v < 0)
					v = 0;
				if(v > 255)
					v &= 0xFF;
				if(!bput(&b, (uint8_t) v)){
					bfree(&b);
					return 0;
				}
				p = end;
				continue;
			}else{
				p++;
			}
			continue;
		}
		p++;
	}
	bfree(&b);
	if(verbose)
		fprintf(stderr, "Could not find array index %d\n", want_idx);
	return 0;
}

/* ===== Event model (absolute time in frames) ===== */
typedef enum {
	EV_NOTEON,
	EV_CTRL,
	EV_PROG,
	EV_META,
	EV_EOT
}EvKind;


typedef struct{
	uint32_t t_frames; /* absolute frame time (60 Hz units) */
	EvKind kind;
	uint8_t ch, a, b; /* NOTEON: a=note b=vel; CTRL: a=cc b=val; PROG: a=patch; META: a=type b=char */
}Event;


typedef struct{
	Event *d;
	size_t n, cap;
}EVec;


static int e_res(EVec *v, size_t add){
	size_t need = v->n + add;
	if(need <= v->cap)
		return 1;
	size_t nc = v->cap ? v->cap : 256;
	while(nc < need)
		nc <<= 1;
	void *p = realloc(v->d, nc *sizeof(Event));
	if(!p)
		return 0;
	v->d = (Event *) p;
	v->cap = nc;
	return 1;
}


static int e_push(EVec *v, Event e){
	if(!e_res(v, 1))
		return 0;
	v->d[v->n++] = e;
	return 1;
}


static int parse_raw_to_events(const uint8_t *buf, size_t len, EVec *out, bool monophonic, bool summary, bool verbose){
	const uint8_t *p = buf;
	const uint8_t *e = buf + len;
	uint8_t running = 0;
	uint32_t absf = 0;
	int active_note[16];
	for(int i = 0; i < 16; i++)
		active_note[i] = -1;
	int note_count[16] = {
		0
	};
	out->d = NULL;
	out->n = out->cap = 0;

	while(p < e){
		uint32_t delta = 0;
		const uint8_t *adv = NULL;
		if(!read_vlq(p, e, & delta, & adv)){
			if(verbose)
				fprintf(stderr, "Bad VLQ near offset %ld\n", (long)(p - buf));
			return 0;
		}
		p = adv;
		absf += delta;
		if(p >= e){
			if(verbose)
				fprintf(stderr, "Unexpected end after delta\n");
			return 0;
		}
		uint8_t s = *p++;
		if(s == 0xFF){
			if(p >= e){
				if(verbose)
					fprintf(stderr, "Truncated meta\n");
				return 0;
			}
			uint8_t type = *p++;
			uint32_t mlen = 0;
			const uint8_t *adv2 = NULL;
			if(!read_vlq(p, e, & mlen, & adv2)){
				if(verbose)
					fprintf(stderr, "Bad meta len\n");
				return 0;
			}
			p = adv2;
			if(type == 0x2F){
				if(mlen != 0){
					if(p + mlen > e){
						if(verbose)
							fprintf(stderr, "EOT overrun\n");
						return 0;
					}
					p += mlen;
				}
				Event ev = {
					absf,
					EV_EOT,
					0,
					0,
					0
				};
				if(!e_push(out, ev))
					return 0;
				break;
			}else if(type == 0x06 && mlen == 1 && p < e){
				uint8_t c = *p++;
				Event ev = {
					absf,
					EV_META,
					0,
					type,
					c
				};
				if(!e_push(out, ev))
					return 0;
			}else{
				if(p + mlen > e){
					if(verbose)
						fprintf(stderr, "Truncated meta data\n");
					return 0;
				}
				p += mlen;
			}
			continue;
		}
		uint8_t status = s;
		uint8_t data1 = 0;
		if(s < 0x80){
			if(!running){
				if(verbose)
					fprintf(stderr, "Running status but none set\n");
				return 0;
			}
			data1 = s;
			status = running;
		}else{
			running = status;
			if(p >= e){
				if(verbose)
					fprintf(stderr, "Missing data1\n");
				return 0;
			}
			data1 = *p++;
		}
		uint8_t ch = status & 0x0F;
		switch(status & 0xF0){
		case 0x90: {
			uint8_t note = data1;
			uint8_t vel = (p < e) ? *p++ : 0;
			if(monophonic && vel > 0 && active_note[ch] >= 0){
				Event off = {
					absf,
					EV_NOTEON,
					ch,
					(uint8_t) active_note[ch],
					0
				};
				if(!e_push(out, off))
					return 0;
				active_note[ch] = -1;
			}
			Event on = {
				absf,
				EV_NOTEON,
				ch,
				note,
				vel
			};
			if(!e_push(out, on))
				return 0;
			if(vel > 0){
				active_note[ch] = note;
				note_count[ch]++;
			}else{
				if(active_note[ch] == note)
					active_note[ch] = -1;
			}
		}
		break;
		case 0xB0: {
			uint8_t cc = data1;
			uint8_t val = (p < e) ? *p++ : 0;
			Event ev = {
				absf,
				EV_CTRL,
				ch,
				cc,
				val
			};
			if(!e_push(out, ev))
				return 0;
		}
		break;
		case 0xC0: {
			Event ev = {
				absf,
				EV_PROG,
				ch,
				data1,
				0
			};
			if(!e_push(out, ev))
				return 0;
		}
		break;
		case 0x80: {
			uint8_t note = data1;
			if(p < e) p++; /* vel */
			Event off = {
				absf,
				EV_NOTEON,
				ch,
				note,
				0
			};
			if(!e_push(out, off))
				return 0;
			if(active_note[ch] == note)
				active_note[ch] = -1;
		}
		break;
		default:
			if(verbose)
				fprintf(stderr, "Unsupported status 0x%02X\n", status);
			return 0;
		}
	}

	if(summary){
		fprintf(stderr, "Per-channel NoteOn counts:\n");
		for(int i = 0; i < 16; i++)
			if(note_count[i])
				fprintf(stderr, "  ch%d: %d\n", i, note_count[i]);
	}
	return 1;
}

/* ===== mconvert compressed -> RAW midiconv ===== */
typedef struct{
	uint8_t *d;
	size_t n, cap;
}BV;


static void bv_init(BV *v){
	v->d = NULL;
	v->n = 0;
	v->cap = 0;
}


static void bv_free(BV *v){
	free(v->d);
	v->d = NULL;
	v->n = v->cap = 0;
}


static int bv_res(BV *v, size_t add){
	if(v->n + add <= v->cap)
		return 1;
	size_t nc = v->cap ? v->cap : 1024;
	while(nc < v->n + add)
		nc <<= 1;
	uint8_t *p = (uint8_t *) realloc(v->d, nc);
	if(!p)
		return 0;
	v->d = p;
	v->cap = nc;
	return 1;
}


static int bv_put(BV *v, uint8_t b){
	if(!bv_res(v, 1))
		return 0;
	v->d[v->n++] = b;
	return 1;
}


static int bv_w(BV *v, const void *s, size_t n){
	if(!bv_res(v, n))
		return 0;
	memcpy(v->d + v->n, s, n);
	v->n += n;
	return 1;
}


static int bv_vlq(BV *v, uint32_t val){
	uint8_t tmp[5];
	int i = 0;
	tmp[i++] = (uint8_t)(val & 0x7F);
	val >>= 7;
	while(val){
		tmp[i++] = (uint8_t)(0x80 | (val & 0x7F));
		val >>= 7;
	}
	for(int j = i - 1; j >= 0; --j){
		if(!bv_put(v, tmp[j]))
			return 0;
	}
	return 1;
}


static int cc_from_sub(uint8_t sub){
	switch (sub){
	case 0x00:
		return 7;
	case 0x08:
		return 11;
	case 0x10:
		return 92;
	case 0x18:
		return 100;
	default:
		return -1;
	}
}

#define SETERR(msg) do { if (err && errlen) { strncpy(err, (msg), errlen-1); err[errlen-1]=0; } } while (0)

static bool mconvert_decompress(const uint8_t *in, size_t inlen, uint8_t **out, size_t *outlen, char *err, size_t errlen){
	*out = NULL;
	*outlen = 0;
	BV v;
	bv_init( & v);
	uint32_t pending = 0;
	size_t i = 0;
	int ended = 0;
	while(i < inlen){
		uint8_t b = in [i++];
		if(b == 0xFF){
			SETERR("0xFF escape not supported");
			bv_free( & v);
			return false;
		}
		uint8_t group = b & 0xE0;
		if(group < 0xA0){
			/* NOTE ON */
			if(i >= inlen){
				SETERR("note-on truncated");
				bv_free( & v);
				return false;
			}
			uint8_t ch = b >> 5;
			uint8_t vol5 = b & 0x1F;
			uint8_t b2 = in [i++];
			uint8_t vol6 = (uint8_t)(((b2 >> 7) ? 0x20 : 0x00) | vol5);
			uint8_t note = (uint8_t)(b2 & 0x7F);
			uint8_t vel7 = (uint8_t)(vol6 << 1);
			if(!bv_vlq( & v, pending)){
				SETERR("oom");
				bv_free( & v);
				return false;
			}
			pending = 0;
			uint8_t st = (uint8_t)(0x90 | (ch & 0x0F));
			if(!bv_put( & v, st) || !bv_put( & v, note) || !bv_put( & v, vel7)){
				SETERR("oom");
				bv_free( & v);
				return false;
			}
			continue;
		}
		if(group == 0xA0){
			/* Program Change */
			if(i >= inlen){
				SETERR("prog truncated");
				bv_free( & v);
				return false;
			}
			uint8_t ch = b & 0x07;
			uint8_t patch = in [i++];
			if(!bv_vlq( & v, pending)){
				SETERR("oom");
				bv_free( & v);
				return false;
			}
			pending = 0;
			uint8_t st = (uint8_t)(0xC0 | (ch & 0x0F));
			if(!bv_put( & v, st) || !bv_put( & v, patch)){
				SETERR("oom");
				bv_free( & v);
				return false;
			}
			continue;
		}
		if(group == 0xC0){
			/* markers & timing */
			uint8_t sub2 = (uint8_t)(b & 0x03);
			if(sub2 == 0x03){
				/* Tick End */
				uint8_t ddd = (uint8_t)((b >> 2) & 0x07);
				if(ddd == 0x07){
					if(i >= inlen){
						SETERR("long delay truncated");
						bv_free( & v);
						return false;
					}
					uint8_t n = in [i++];
					if(n == 0xFF)
						pending += 254u;
					else
						pending += (uint32_t) n;
				}else{
					pending += (uint32_t)(ddd + 1u);
				}
				continue;
			}else if(sub2 == 0x01){
				/* Loop Start -> marker 'S' */
				if(!bv_vlq( & v, pending)){
					SETERR("oom");
					bv_free( & v);
					return false;
				}
				pending = 0;
				uint8_t m[] = {
					0xFF,
					0x06,
					0x01,
					'S'
				};
				if(!bv_w( & v, m, sizeof m)){
					SETERR("oom");
					bv_free( & v);
					return false;
				}
				continue;
			}else if(sub2 == 0x00){
				/* Loop End -> marker 'E' + EOT */
				if(!bv_vlq( & v, pending)){
					SETERR("oom");
					bv_free( & v);
					return false;
				}
				pending = 0;
				uint8_t e[] = {
					0xFF,
					0x06,
					0x01,
					'E'
				};
				if(!bv_w( & v, e, sizeof e)){
					SETERR("oom");
					bv_free( & v);
					return false;
				}
				if(!bv_vlq( & v, 0) || !bv_w( & v, (uint8_t[]){
						0xFF,
						0x2F,
						0x00
					}, 3)){
					SETERR("oom");
					bv_free( & v);
					return false;
				}
				ended = 1;
				break;
			}else if(sub2 == 0x02){
				/* Song End -> EOT */
				if(!bv_vlq( & v, pending)){
					SETERR("oom");
					bv_free( & v);
					return false;
				}
				pending = 0;
				if(!bv_w( & v, (uint8_t[]){
						0xFF,
						0x2F,
						0x00
					}, 3)){
					SETERR("oom");
					bv_free( & v);
					return false;
				}
				ended = 1;
				break;
			}else{
				SETERR("unknown 0xC0 subtype");
				bv_free( & v);
				return false;
			}
		}
		if(group == 0xE0){
			/* Controller */
			if(i >= inlen){
				SETERR("controller truncated");
				bv_free(&v);
				return false;
			}
			uint8_t ch = (uint8_t)(b & 0x07);
			uint8_t sub = (uint8_t)(b & 0x18);
			int cc = cc_from_sub(sub);
			if(cc < 0){
				SETERR("unknown controller");
				bv_free(&v);
				return false;
			}
			uint8_t val = in [i++];
			if(!bv_vlq( & v, pending)){
				SETERR("oom");
				bv_free(&v);
				return false;
			}
			pending = 0;
			uint8_t st = (uint8_t)(0xB0 | (ch & 0x0F));
			if(!bv_put(&v, st) || !bv_put(&v, (uint8_t)cc) || !bv_put(&v, val)){
				SETERR("oom");
				bv_free(&v);
				return false;
			}
			continue;
		}
		SETERR("unknown opcode");
		bv_free(&v);
		return false;
	}
	if(!ended){
		SETERR("no terminator");
		bv_free(&v);
		return false;
	}
	*out = v.d;
	*outlen = v.n;
	return true;
}

/* ===== Math helpers ===== */
static uint32_t ugcd(uint32_t a, uint32_t b){
	while(b){
		uint32_t t = a % b;
		a = b;
		b = t;
	}
	return a;
}


static uint32_t ulcm(uint32_t a, uint32_t b){
	if(!a || !b)
		return 0;
	return (a / ugcd(a, b)) *b;
}


static void be16(uint8_t *p, uint16_t v){
	p[0] = (uint8_t)((v >> 8) & 0xFF);
	p[1] = (uint8_t)(v & 0xFF);
}


static void be32(uint8_t *p, uint32_t v){
	p[0] = (uint8_t)((v >> 24) & 0xFF);
	p[1] = (uint8_t)((v >> 16) & 0xFF);
	p[2] = (uint8_t)((v >> 8) & 0xFF);
	p[3] = (uint8_t)(v & 0xFF);
}


static int write_type0(const char *path, const EVec *evs, int ppq, uint32_t ticks_per_frame, double bpm){
	FILE *f = fopen(path, "wb");
	if(!f){
		perror("open output");
		return 0;
	}
	Buf tr;
	binit( & tr);

	/* Tempo at delta 0 */
	uint32_t us_per_q = (uint32_t) llround(60000000.0 / (bpm > 0.0 ? bpm : 225.0));
	write_vlq( & tr, 0);
	uint8_t st[] = {
		0xFF,
		0x51,
		0x03,
		0,
		0,
		0
	};
	st[3] = (us_per_q >> 16) & 0xFF;
	st[4] = (us_per_q >> 8) & 0xFF;
	st[5] = us_per_q & 0xFF;
	bwr( &tr, st, sizeof st);

	uint32_t lastf = 0;
	for(size_t i = 0; i < evs->n; i++){
		const Event *e = & evs->d[i];
		uint32_t df = e->t_frames - lastf;
		lastf = e->t_frames;
		uint32_t dt = df *ticks_per_frame;
		if(!write_vlq( & tr, dt)){
			fclose(f);
			bfree( & tr);
			return 0;
		}

		switch (e->kind){
		case EV_NOTEON: {
			uint8_t st2 = (uint8_t)(0x90 | (e->ch & 0x0F));
			bput( & tr, st2);
			bput( & tr, e->a);
			bput( & tr, e->b);
		}
		break;
		case EV_CTRL: {
			uint8_t st2 = (uint8_t)(0xB0 | (e->ch & 0x0F));
			bput( & tr, st2);
			bput( & tr, e->a);
			bput( & tr, e->b);
		}
		break;
		case EV_PROG: {
			uint8_t st2 = (uint8_t)(0xC0 | (e->ch & 0x0F));
			bput( & tr, st2);
			bput( & tr, e->a);
		}
		break;
		case EV_META: {
			uint8_t m[] = {
				0xFF,
				e->a,
				0x01,
				e->b
			};
			bwr( & tr, m, sizeof m);
		}
		break;
		case EV_EOT:
			/* let final EOT below close */ break;
		}
	}
	write_vlq( & tr, 0);
	uint8_t eot[] = {
		0xFF,
		0x2F,
		0x00
	};
	bwr( & tr, eot, sizeof eot);

	uint8_t hdr[14];
	memcpy(hdr, "MThd", 4);
	be32(hdr + 4, 6);
	be16(hdr + 8, 0);
	be16(hdr + 10, 1);
	be16(hdr + 12, (uint16_t) ppq);
	fwrite(hdr, 1, 14, f);
	uint8_t th[8];
	memcpy(th, "MTrk", 4);
	be32(th + 4, (uint32_t) tr.n);
	fwrite(th, 1, 8, f);
	fwrite(tr.d, 1, tr.n, f);
	bfree( & tr);
	fclose(f);
	return 1;
}

static int write_type1(const char *path, const EVec *evs, int ppq, uint32_t ticks_per_frame, double bpm){
	/* Build conductor track 0 and per-channel tracks (0..15) */
	Buf t0;
	binit( & t0);
	uint32_t us_per_q = (uint32_t) llround(60000000.0 / (bpm > 0.0 ? bpm : 225.0));
	write_vlq( & t0, 0);
	uint8_t tempo[] = {
		0xFF,
		0x51,
		0x03,
		0,
		0,
		0
	};
	tempo[3] = (us_per_q >> 16) & 0xFF;
	tempo[4] = (us_per_q >> 8) & 0xFF;
	tempo[5] = us_per_q & 0xFF;
	bwr( & t0, tempo, sizeof tempo);

	Buf trk[16];
	for(int i = 0; i < 16; i++)
		binit( & trk[i]);
	uint32_t lastf0 = 0;
	uint32_t lastfch[16];
	for(int i = 0; i < 16; i++)
		lastfch[i] = 0;
	bool used[16] = {
		0
	};

	/* We put all META markers on track 0, and channel events on their channel track */
	for(size_t i = 0; i < evs->n; i++){
		const Event *e = & evs->d[i];
		if(e->kind == EV_META || e->kind == EV_EOT){
			uint32_t df = e->t_frames - lastf0;
			lastf0 = e->t_frames;
			if(!write_vlq( & t0, df * ticks_per_frame)){
				for(int k = 0; k < 16; k++)
					bfree( & trk[k]);
				bfree(&t0);
				return 0;
			}
			if(e->kind == EV_META){
				uint8_t m[] = {
					0xFF,
					e->a,
					0x01,
					e->b
				};
				bwr( & t0, m, sizeof m);
			}
			continue;
		}
		int ch = e->ch & 0x0F;
		used[ch] = true;
		uint32_t df = e->t_frames - lastfch[ch];
		lastfch[ch] = e->t_frames;
		if(!write_vlq( & trk[ch], df * ticks_per_frame)){
			for(int k = 0; k < 16; k++)
				bfree( & trk[k]);
			bfree( & t0);
			return 0;
		}
		switch (e->kind){
		case EV_NOTEON: {
			uint8_t st2 = (uint8_t)(0x90 | ch);
			bput( & trk[ch], st2);
			bput( & trk[ch], e->a);
			bput( & trk[ch], e->b);
		}
		break;
		case EV_CTRL: {
			uint8_t st2 = (uint8_t)(0xB0 | ch);
			bput( & trk[ch], st2);
			bput( & trk[ch], e->a);
			bput( & trk[ch], e->b);
		}
		break;
		case EV_PROG: {
			uint8_t st2 = (uint8_t)(0xC0 | ch);
			bput( & trk[ch], st2);
			bput( & trk[ch], e->a);
		}
		break;
		default:
			break;
		}
	}
	write_vlq( & t0, 0);
	uint8_t eot[] = {
		0xFF,
		0x2F,
		0x00
	};
	bwr( & t0, eot, sizeof eot);
	for(int ch = 0; ch < 16; ch++){
		if(used[ch]){
			write_vlq( & trk[ch], 0);
			bwr( & trk[ch], eot, sizeof eot);
		}
	}

	/* Count tracks = conductor + used channels */
	int track_count = 1;
	for(int ch = 0; ch < 16; ch++){
		if(used[ch])
			track_count++;
	}
	FILE *f = fopen(path, "wb");
	if(!f){
		perror("open output");
		for(int k = 0; k < 16; k++)
			bfree( & trk[k]);
		bfree(&t0);
		return 0;
	}
	uint8_t hdr[14];
	memcpy(hdr, "MThd", 4);
	be32(hdr + 4, 6);
	be16(hdr + 8, 1);
	be16(hdr + 10, (uint16_t) track_count);
	be16(hdr + 12, (uint16_t) ppq);
	fwrite(hdr, 1, 14, f);

	/* write track 0 */
	uint8_t th[8];
	memcpy(th, "MTrk", 4);
	be32(th + 4, (uint32_t) t0.n);
	fwrite(th, 1, 8, f);
	fwrite(t0.d, 1, t0.n, f);
	/* write used channel tracks in order */
	for(int ch = 0; ch < 16; ch++){
		if(used[ch]){
			memcpy(th, "MTrk", 4);
			be32(th + 4, (uint32_t) trk[ch].n);
			fwrite(th, 1, 8, f);
			fwrite(trk[ch].d, 1, trk[ch].n, f);
		}
	}

	for(int k = 0; k < 16; k++)
		bfree(&trk[k]);
	bfree( & t0);
	fclose(f);
	return 1;
}


/* ===== Utility ===== */
static int parse_int(const char *s, int *out){
	char *end = NULL;
	long v = strtol(s, & end, 10);
	if(!s[0] || (end && *end))
		return 0;
	*out = (int) v;
	return 1;
}


static int parse_double(const char *s, double *out){
	char *end = NULL;
	double v = strtod(s, & end);
	if(!s[0] || (end && *end))
		return 0;
	*out = v;
	return 1;
}

/* ===== SNAP: adjust absolute frame times to nearest 1/N notes ===== */
static void apply_snap(EVec *evs, int snapN, uint32_t ticks_per_frame, int ppq){
	if(snapN <= 0)
		return;
	/* size in ticks of 1/N note */
	double grid = (double) ppq / (double) snapN;
	for(size_t i = 0; i < evs->n; i++){
		double t_ticks = (double) evs->d[i].t_frames * (double) ticks_per_frame;
		double snapped = floor((t_ticks / grid) + 0.5) * grid;
		uint32_t new_frames = (uint32_t) llround(snapped / (double) ticks_per_frame);
		evs->d[i].t_frames = new_frames;
	}
	/* stable sort by (t_frames, kind) to keep ordering after snaps */
	for(size_t i = 1; i < evs->n; i++){
		Event key = evs->d[i];
		size_t j = i;
		while(j > 0){
			bool less = false;
			if(key.t_frames < evs->d[j - 1].t_frames){
				less = true;
			}else if(key.t_frames == evs->d[j - 1].t_frames){
				int ka = (key.kind == EV_META) ? 0 : 1, kb = (evs->d[j - 1].kind == EV_META) ? 0 : 1;
				if(ka < kb)
					less = true;
			}
			if(!less)
				break;
			evs->d[j] = evs->d[j - 1];
			j--;
		}
		evs->d[j] = key;
	}
}


int main(int argc, char **argv){
	if(argc < 3){
		usage(argv[0]);
		return 1;
	}
	Opt opt;
	memset( & opt, 0, sizeof opt);
	/* Defaults: align 60 Hz frames to 1/16 notes exactly */
	opt.index = 0;
	opt.quant = 60;
	opt.bpm = 225.0;
	opt.ppq = 0;
	opt.grid = 0;
	opt.snap = 0;
	opt.type = 0;
	for(int i = 3; i < argc; i++){
		if(!strcmp(argv[i], "-i") || !strcmp(argv[i], "--index")){
			if(i + 1 >= argc || !parse_int(argv[++i], & opt.index)){
				fprintf(stderr, "Bad -i\n");
				return 1;
			}
		}else if(!strcmp(argv[i], "--ppq")){
			if(i + 1 >= argc || !parse_int(argv[++i], & opt.ppq)){
				fprintf(stderr, "Bad --ppq\n");
				return 1;
			}
		}else if(!strcmp(argv[i], "--quant")){
			if(i + 1 >= argc || !parse_int(argv[++i], & opt.quant)){
				fprintf(stderr, "Bad --quant\n");
				return 1;
			}
		}else if(!strcmp(argv[i], "--bpm")){
			if(i + 1 >= argc || !parse_double(argv[++i], & opt.bpm)){
				fprintf(stderr, "Bad --bpm\n");
				return 1;
			}
		}else if(!strcmp(argv[i], "--grid")){
			if(i + 1 >= argc || !parse_int(argv[++i], & opt.grid)){
				fprintf(stderr, "Bad --grid\n");
				return 1;
			}
		}else if(!strcmp(argv[i], "--snap")){
			if(i + 1 >= argc || !parse_int(argv[++i], & opt.snap)){
				fprintf(stderr, "Bad --snap\n");
				return 1;
			}
		}else if(!strcmp(argv[i], "--type")){
			if(i + 1 >= argc || !parse_int(argv[++i], & opt.type) || (opt.type != 0 && opt.type != 1)){
				fprintf(stderr, "Bad --type (0 or 1)\n");
				return 1;
			}
		}else if(!strcmp(argv[i], "--no-mono")){
			opt.no_mono = true;
		}else if(!strcmp(argv[i], "--summary")){
			opt.summary = true;
		}else if(!strcmp(argv[i], "--verbose")){
			opt.verbose = true;
		}else if(!strcmp(argv[i], "--compressed")){
			opt.compressed = true;
		}else{
			fprintf(stderr, "Unknown option: %s\n", argv[i]);
			usage(argv[0]);
			return 1;
		}
	}

	size_t srclen = 0;
	char *src = slurp(argv[1], & srclen);
	if(!src){
		perror("input");
		return 1;
	}

	uint8_t *arr = NULL;
	size_t arrlen = 0;
	if(!parse_array_bytes(src, opt.index, & arr, & arrlen, opt.verbose)){
		fprintf(stderr, "Failed to parse array index %d\n", opt.index);
		free(src);
		return 1;
	}
	free(src);
	if(opt.verbose)
		fprintf(stderr, "Parsed %zu bytes from array %d\n", arrlen, opt.index);

	/* Get RAW midiconv bytes (VLQ deltas + MIDI channel/meta) */
	uint8_t *raw = NULL;
	size_t rawlen = 0;
	if(opt.compressed){
		char err[128] = {
			0
		};
		if(!mconvert_decompress(arr, arrlen, & raw, & rawlen, err, sizeof err)){
			fprintf(stderr, "Compressed decode failed: %s\n", err[0] ? err : "unknown");
			free(arr);
			return 1;
		}
		if(opt.verbose)
			fprintf(stderr, "Decoded compressed stream to %zu bytes RAW\n", rawlen);
		free(arr);
	}else{
		raw = arr;
		rawlen = arrlen;
	}

	EVec evs = {
		0
	};
	if(!parse_raw_to_events(raw, rawlen, & evs, !opt.no_mono, opt.summary, opt.verbose)){
		fprintf(stderr, "Failed parsing RAW stream\n");
		free(raw);
		return 1;
	}
	free(raw);

	/* ===== Mapping: compute ticks_per_frame and PPQ =====
	   If --quant is given (default 60), we derive PPQ from BPM so that frames land exactly on the musical grid.
	   Let frames/beat = 3600 / BPM. Reduce to a/b (integers) by gcd.
	   Choose L = lcm(quant, b) ticks per frame. Then PPQ = (L/b) * a. */
	uint32_t L = 0;
	int ppq = opt.ppq;
	double bpm = (opt.bpm > 0.0 ? opt.bpm : 225.0);

	if(opt.quant > 0){
		/* rationalize frames/beat for fractional BPM */
		double frames_per_beat = 3600.0 / bpm;
		const int DEN = 1000;
		uint32_t a = (uint32_t) llround(frames_per_beat * DEN);
		uint32_t b = DEN;
		uint32_t g = ugcd(a, b);
		a /= g;
		b /= g;
		L = ulcm((uint32_t) opt.quant, b);
		ppq = (int)((L / b) * a);
		if(ppq <= 0)
			ppq = 960; /* fallback */
		if(opt.verbose)
			fprintf(stderr, "Mapping: quant=%d, bpm=%.6f => frames/beat=%u/%u, L=%u ticks/frame, PPQ=%d\n", opt.quant, bpm,
			(unsigned) a, (unsigned) b, (unsigned) L, ppq);
	}else{
		if(ppq <= 0)
			ppq = 480;
		L = 1; /* ticks per frame */
		if(opt.verbose)
			fprintf(stderr, "Mapping: explicit PPQ=%d, ticks/frame=%u (no quant)\n", ppq, (unsigned) L);
	}

	/* --grid: scale PPQ and L together so PPQ is a multiple of grid (no timing change) */
	if(opt.grid > 1){
		int ppq2 = ppq;
		int g = opt.grid;
		int mul = (ppq2 % g) == 0 ? 1 : (g / (int) ugcd((uint32_t) ppq2, (uint32_t) g));
		if(mul > 1){
			ppq *= mul;
			L *= mul;
			if(opt.verbose)
				fprintf(stderr, "Grid: scaled PPQ by x%d => PPQ=%d, ticks/frame=%u\n", mul, ppq, (unsigned) L);
		}
	}

	/* --snap: quantize absolute frame times to nearest 1/snap notes (does change timing) */
	if(opt.snap > 1){
		apply_snap( & evs, opt.snap, L, ppq);
		if(opt.verbose)
			fprintf(stderr, "Applied snap to 1/%d notes\n", opt.snap);
	}

	/* ===== Write MIDI ===== */
	int ok = (opt.type == 0) ? write_type0(argv[2], & evs, ppq, L, bpm) : write_type1(argv[2], & evs, ppq, L, bpm);

	free(evs.d);
	if(!ok){
		fprintf(stderr, "Failed writing MIDI\n");
		return 1;
	}
	if(opt.verbose)
		fprintf(stderr, "Wrote %s\n", argv[2]);
	return 0;
}