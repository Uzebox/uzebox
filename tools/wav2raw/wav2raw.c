/*
 *  Uzebox(tm Alec Bourque)
 *  wav2raw utility
 *  2025 Lee Weber
 *
 *  GPLv3-or-later
 *
 *  Summary of automated conversion:
 * - DC high-pass (10 Hz), ~100 dB Kaiser resample, -0.3 dBTP normalize
 * - 1st-order noise-shaped TPDF dither to U8
 * - Always overwrite outputs
 * - Name templating: --name "{stem}_{rate}_{samples}.{ext}"
 * - Quoted CSV: input[,output][,gain_db]
 * - Frame alignment: --align-frames auto|N [--frame-rate Hz]  (pads with silence)
 * - Sector/byte alignment: --align-bytes N (pads with 0x80)
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ===== mkdir one-level ===== */
static int mkoutdir(const char* path){
	if(!path || !*path)
		return 0;
#ifdef _WIN32
	int r = _mkdir(path);
#else
	int r = mkdir(path, 0755);
#endif
	if(r == 0 || errno == EEXIST)
		return 0;
	return -1;
}

/* ===== RNG / TPDF ===== */
static uint32_t rng_state = 0x12345678u;
static inline uint32_t xrnd(void){
	uint32_t x = rng_state;
	x ^= x<<13; x ^= x>>17;
	x ^= x<<5;
	return rng_state = x;
}


static inline double tpdf(void){
	double a = ((xrnd() & 0xFFFFFFu)/16777216.0) - 0.5;
	double b = ((xrnd() & 0xFFFFFFu)/16777216.0) - 0.5;
	return a + b;
}

/* ===== Bessel I0 (Kaiser) ===== */
static double bessel_i0(double x){
	double ax = fabs(x);
	if(ax < 3.75){
		double y = x/3.75; y*=y;
		return 1.0 + y*(3.5156229 + y*(3.0899424 + y*(1.2067492 + y*(0.2659732 + y*(0.0360768 + y*0.0045813)))));
	}else{
		double y = 3.75/ax;
		return (exp(ax)/sqrt(ax))*	(0.39894228 + y*(0.01328592 + y*(0.00225319 + y*(-0.00157565 + y*(0.00916281 +
						y*(-0.02057706 + y*(0.02635537 + y*(-0.01647633 + y*0.00392377))))))));
	}
}

/* ===== WAV parsing ===== */
typedef struct{
	uint32_t sr;
	uint16_t ch;
	uint16_t fmt;	//1=PCM, 3=float32
	uint16_t bits;
	size_t frames;
	double *mono;	//-1..+1
}wav_t;


static int rd32le(FILE* f, uint32_t* out){
	uint8_t b[4];
	if(fread(b,1,4,f) != 4)
		return 0;
	*out = (uint32_t)b[0] | ((uint32_t)b[1]<<8) | ((uint32_t)b[2]<<16) | ((uint32_t)b[3]<<24);
	return 1;
}


static int rd16le(FILE* f, uint16_t* out){
	uint8_t b[2];
	if(fread(b,1,2,f) != 2)
		return 0;
	*out = (uint16_t)b[0] | ((uint16_t)b[1]<<8);
	return 1;
}


static inline int32_t sign_extend_u32(uint32_t u, unsigned bits){//sign-extend lower 'bits' of u into int32_t
	if(bits == 0)
		return 0;
	if(bits >= 32)
		return (int32_t)u;	//already full width
	//mask to 'bits' and apply "flip-and-subtract" trick
	uint32_t sign = 1u << (bits - 1);
	uint32_t mask = (1u << bits) - 1u;
	u &= mask;
	return (int32_t)((u ^ sign) - sign);
}


static int load_wav(const char* path, wav_t* out){
	memset(out,0,sizeof(*out));

	FILE* f = fopen(path,"rb");
	if(!f){
		fprintf(stderr,"ERROR: open %s: %s\n", path, strerror(errno));
		return -1;
	}

	//RIFF/WAVE header
	uint8_t hdr[12];
	if(fread(hdr,1,12,f) != 12 || memcmp(hdr,"RIFF",4) || memcmp(hdr+8,"WAVE",4)){
		fprintf(stderr,"ERROR: not RIFF/WAVE: %s\n", path);
		fclose(f);
		return -1;
	}

	//chunk scan
	uint16_t audio_format = 0;	//1=PCM, 3=IEEE float, 0xFFFE=EXTENSIBLE
	uint16_t num_channels = 0;
	uint32_t sample_rate = 0;
	uint16_t bits_per_sample = 0;
	uint16_t block_align = 0;

	uint32_t data_off = 0;
	uint32_t data_sz = 0;

	uint16_t valid_bits = 0;	//from WAVEFORMATEXTENSIBLE, 0 if absent
	uint8_t sub_guid[16];
	int is_extensible = 0;		//wFormatTag==0xFFFE
	int eff_format = 0;		//1=PCM, 3=IEEE float (resolved)
	size_t bytes_per_container = 0;	//container bytes per channel (e.g. 1,2,3,4)
	double* mono = NULL;

	while(!feof(f)){
		uint8_t id[4];
		if(fread(id,1,4,f) != 4)
			break;

		uint32_t chunk_size = 0;
		if(!rd32le(f, &chunk_size))
			goto wav_fail;

		long here = ftell(f);

		if(!memcmp(id,"fmt ",4)){	//fmt chunk
			uint32_t byte_rate_ignored = 0;

			if(!rd16le(f, &audio_format))
				goto wav_fail;
			if(!rd16le(f, &num_channels))
				goto wav_fail;
			if(!rd32le(f, &sample_rate))
				goto wav_fail;

			if(!rd32le(f, &byte_rate_ignored))
				goto wav_fail;	//ignore value, but must read it
			if(!rd16le(f, &block_align))
				goto wav_fail;
			if(!rd16le(f, &bits_per_sample))
				goto wav_fail;

			if(chunk_size >= 18){	//has cbSize
				uint16_t cbSize = 0;
				if(!rd16le(f, &cbSize))
					goto wav_fail;

				if(audio_format == 0xFFFE && cbSize >= 22){	//WAVEFORMATEXTENSIBLE
					uint32_t chmask_ignore = 0;

					if(!rd16le(f, &valid_bits))
						goto wav_fail;
					if(!rd32le(f, &chmask_ignore))
						goto wav_fail;
					(void)chmask_ignore;

					for(int gi=0; gi<16; gi++){
						if(fread(&sub_guid[gi],1,1,f) != 1)
							goto wav_fail;
					}
					is_extensible = 1;

					// resolve effective format via GUID.Data1
					uint32_t d1 = (uint32_t)sub_guid[0] | ((uint32_t)sub_guid[1]<<8) | ((uint32_t)sub_guid[2]<<16)| ((uint32_t)sub_guid[3]<<24);
					if(d1 == 0x00000001u){
						eff_format = 1;	//PCM
					}else if(d1 == 0x00000003u){
						eff_format = 3;	//IEEE float
					}else{
						fprintf(stderr,"ERROR: unsupported WAVEFORMATEXTENSIBLE SubFormat in %s\n", path);
						goto wav_fail;
					}
				}
			}

			if(!is_extensible){
				//not extensible: effective format is the base tag
				eff_format = audio_format;
			}
		}else if(!memcmp(id,"data",4)){
			data_off = ftell(f);
			data_sz = chunk_size;
		}

		//advance to next chunk (word-aligned)
		if(fseek(f, here + ((chunk_size+1u) & ~1u), SEEK_SET) != 0)
			goto wav_fail;
	}

	// --- validate using eff_format (handles EXTENSIBLE) ---
	if(!data_off || !sample_rate || !num_channels){
		fprintf(stderr,"ERROR: missing critical WAV fields in %s\n", path);
		goto wav_fail;
	}
	if(eff_format != 1 && eff_format != 3){
		fprintf(stderr,"ERROR: unsupported WAV format tag (eff=%d) in %s\n", eff_format, path);
		goto wav_fail;
	}
	//for PCM we need bits_per_sample, for float we expect 32-bit container
	if(eff_format == 1 && bits_per_sample == 0){
		fprintf(stderr,"ERROR: PCM with 0 bits_per_sample in %s\n", path);
		goto wav_fail;
	}

	// --- derive container bytes and frame count ---
	if(block_align >= num_channels && (block_align % num_channels) == 0){
		bytes_per_container = (size_t)block_align / (size_t)num_channels;
	}else{//fallback (should be rare)
		bytes_per_container = (eff_format == 3) ? 4u : (size_t)((bits_per_sample + 7) / 8);
	}

	if(bytes_per_container == 0){
		fprintf(stderr,"ERROR: invalid container bytes in %s\n", path);
		goto wav_fail;
	}

	size_t frames = (size_t)data_sz / ( (size_t)num_channels * bytes_per_container );
	if(frames == 0){
		fprintf(stderr,"ERROR: no audio frames in %s\n", path);
		goto wav_fail;
	}

	mono = (double*)malloc(sizeof(double) * frames);
	if(!mono){
		fclose(f);
		return -1;
	}

	if(fseek(f, (long)data_off, SEEK_SET) != 0){
		fclose(f);
		free(mono);
		return -1;
	}

	for(size_t i=0; i<frames; i++){//sample loop
		double acc = 0.0;

		for(uint16_t c=0; c<num_channels; c++){
			double v = 0.0;

			if(eff_format == 1){//PCM (possibly EXTENSIBLE)
				uint16_t vbits = valid_bits ? valid_bits : bits_per_sample;

				switch(bytes_per_container){
					case 1:{//8-bit unsigned
						uint8_t s8;
						if(fread(&s8,1,1,f) != 1)
							goto wav_fail_read;
						v = ((int)s8 - 128) / 128.0;
					}break;

					case 2:{//16-bit signed container (or <16 valid)
						uint16_t le16;
						if(!rd16le(f, &le16))
							goto wav_fail_read;

						if(vbits > 0 && vbits < 16){
							int32_t ss = sign_extend_u32((uint32_t)le16, vbits);
							v = (double)ss / (double)(1u << (vbits - 1));
						}else{
							int16_t s = (int16_t)le16;
							v = s / 32768.0;
						}
					}break;

					case 3:{//24-bit packed container (or <24 valid)
						uint8_t b[3];
						if(fread(b,1,3,f) != 3)
							goto wav_fail_read;
						uint32_t u = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16);
						uint16_t vb = vbits ? vbits : 24;
						int32_t ss = sign_extend_u32(u, vb);
						double den = (vb >= 2 && vb <= 30) ? (double)(1u << (vb - 1)) : 8388608.0;
						v = (double)ss / den;
					}break;

					case 4:{//32-bit signed container (or <32 valid)
						uint32_t le32;
						if(!rd32le(f, &le32))
							goto wav_fail_read;

						if(vbits > 0 && vbits < 32){
							int32_t ss = sign_extend_u32(le32, vbits);
							double den = (vbits >= 2 && vbits <= 30) ? (double)(1u << (vbits - 1)) : 2147483648.0;
							v = (double)ss / den;
						}else{
							int32_t s = (int32_t)le32;
							v = s / 2147483648.0;
						}
					}break;

					default:{//odd container sizes: read-and-discard safely
						for(size_t jj=0; jj<bytes_per_container; jj++){
							uint8_t tmp;
							if(fread(&tmp,1,1,f) != 1)
								goto wav_fail_read;
						}
						v = 0.0;
					}break;
				}
			}else{//eff_format == 3 (IEEE float)
				if(bytes_per_container != 4){
					fprintf(stderr,"ERROR: float with non-32b container in %s\n", path);
					goto wav_fail_read;
				}
				uint32_t le32;
				if(!rd32le(f, &le32))
					goto wav_fail_read;
				union{
					uint32_t u;
					float f;
				}x;
				x.u = le32;
				v = (double)x.f;
			}

			acc += v;
		}

		mono[i] = acc / (double)num_channels;
	}

	fclose(f);

	out->sr = sample_rate;
	out->ch = 1;
	out->fmt = 1;
	out->bits = 16;
	out->frames = frames;
	out->mono = mono;
	return 0;

wav_fail_read:
	fprintf(stderr,"ERROR: truncated/corrupt WAV while reading %s\n", path);
wav_fail:
	if(f)
		fclose(f);
	free(mono);
	return -1;
}

/* ===== DC high-pass (10 Hz) ===== */
static void hpf_dc_inplace(double* x, size_t n, double sr){
	double fc = 10.0, a = exp(-2.0*M_PI*fc / sr);
	double y = 0.0, x1 = 0.0;
	for(size_t i=0; i<n; i++){
		double xi = x[i];
		y = (xi-x1)+a*y;
		x1 = xi;
		x[i] = y;
	}
}

/* ===== Kaiser resampler (auto HQ ~100 dB) ===== */
typedef struct{
	double* out;
	size_t n;
}vecd_t;


static double kaiser_beta_for_As(double As){
	if(As>50.0)
		return 0.1102*(As-8.7);
	if(As>=21.0)
		return 0.5842*pow(As-21.0,0.4) + 0.07886*(As-21.0);
	return 0.0;
}


static vecd_t resample_sinc_auto(const double* restrict in, size_t inN, double inSR, double outSR){
	vecd_t v = {0};
	if(!inN || inSR<=0 || outSR<=0)
		return v;

	double ratio = outSR / inSR;
	double tw = 0.015;
	double nyq_in = 0.5;
	double nyq_needed = nyq_in * ((ratio<1.0)? ratio : 1.0);
	double fpass = nyq_needed - tw;
	if(fpass < 0.05*nyq_needed)
		fpass = 0.05*nyq_needed;
	if(fpass <= 0.0)
		fpass = nyq_needed * 0.5;

	double As = 100.0;
	double beta = kaiser_beta_for_As(As);
	double N_est = ceil((As - 8.0) / (2.285 * (2.0*M_PI*tw)));
	int taps = (int)N_est;
	if(taps & 1)
		taps++;
	if(taps < 256)
		taps = 256;
	if(taps > 1024)
		taps = 1024;
	int half = taps/2;

	//precompute Kaiser window (constant over all samples)
	double i0b = bessel_i0(beta);
	double *win = (double*)malloc((size_t)taps * sizeof(double));
	if(!win) return v;

	for(int k=0; k<taps; k++){
		double nrm = (double)k / (double)(taps-1);
		double t2 = 2.0*nrm - 1.0;
		if(t2 < -1.0)
			t2 = -1.0;
		if(t2 > 1.0)
			t2 = 1.0;
		win[k] = bessel_i0(beta * sqrt(1.0 - t2*t2)) / i0b;
	}

	size_t outN = (size_t)floor((double)inN * ratio + 0.5);
	if(!outN)
		outN = 1;
	double *out = (double*)calloc(outN, sizeof(double));
	if(!out){
		free(win);
		return v;
	}

	const double two_fpass = 2.0 * fpass;

	for(size_t n=0; n<outN; n++){
		double t = (double)n / ratio;
		long i0 = (long)floor(t);
		double frac = t - (double)i0;

		//tighten tap range near boundaries to avoid per-tap bounds checks
		int kmin = -half;
		int kmax = half - 1;
		long left = -i0;
		long right = (long)inN - 1 - i0;
		if(left > kmin)
			kmin = (int)left;
		if(right < kmax)
			kmax = (int)right;

		double sum = 0.0;
		for(int k=kmin; k<=kmax; k++){
			double x = (double)(k - frac);
			double arg = two_fpass * x;
			double sinc = (fabs(arg) < 1e-12) ? 1.0 : sin(M_PI*arg)/(M_PI*arg);
			double h = two_fpass * sinc * win[k + half];
			sum += in[i0 + k] * h;
		}
		out[n] = sum;
	}

	free(win);
	v.out = out;
	v.n = outN;
	return v;
}

/* ===== Normalize to -0.3 dBTP ===== */
static void normalize_truepeak_safe(double* x, size_t n){
	if(!n)
		return;
	double peak=0.0;
	for(size_t i=0; i<n; i++){
		double a=fabs(x[i]);
		if(a>peak)
			peak=a;
	}
	double target = pow(10.0, -0.3/20.0);	//~0.967
	if(peak > 0.0){
		double g=target/peak;
		for(size_t i=0;i<n;i++)
			x[i]*=g;
	}
}

/* ===== pad samples to multiple ===== */
static void pad_samples_to_multiple(double** px, size_t* pn, size_t mult){
	if(!mult)
		return;
	size_t n = *pn;
	size_t r = n % mult;
	if(!r)
		return;
	size_t need = mult - r;
	double* y = (double*)realloc(*px, (n+need)*sizeof(double));
	if(!y)
		return;
	memset(y+n, 0, need*sizeof(double));
	*px = y;
	*pn = n+need;
}

/* ===== quantize & write (adaptive dither + quiet-gated shaping + optional tail silence) ===== */
static int write_u8_raw_shaped(	const char* path,
				const double* x,
				size_t n,
				double post_gain_db,
				size_t* out_clipped,
				size_t align_bytes,
				double shape,			/* 0..~0.95 */
				double quiet_lsb,		/* shaping off when |signal| < quiet_lsb (hysteresis at 2×) */
				int dither_mode,		/* 0=off, 1=tpdf, 2=auto */
				double out_sr,			/* output sample rate (Hz) */
				double tail_lsb,		/* 0 => disabled; threshold in LSB */
				double tail_hold_ms,		/* ms below thresh before silencing */
				double tail_release_ms){	/* ms above 2×thresh before releasing */
	FILE* f = fopen(path,"wb");
	if(!f){
		fprintf(stderr,"ERROR: write %s: %s\n", path, strerror(errno));
		return -1;
	}

	//gain to apply before quantization
	const double g = pow(10.0, post_gain_db/20.0);

	//error feedback state
	double e1 = 0.0;
	const double e1_clip = 64.0;	//limit error memory (≈LSBs)

	//shaping hysteresis thresholds (in quantizer units)
	const double off_thr = (quiet_lsb > 0.0) ? quiet_lsb : 0.0;
	const double on_thr = (quiet_lsb > 0.0) ? quiet_lsb * 2.0 : 0.0;
	int shaping_on = (off_thr <= 0.0);	//if no threshold, keep shaping on

	//amplitude smoother (~8 ms @ ~16 kHz) used by auto-dither AND tail-gate
	double amag_lp = 0.0;
	const double alpha = 1.0/128.0;

	const int tail_enabled = (tail_lsb > 0.0 && out_sr > 0.0) ? 1 : 0;
	int tail_silent = 0;
	const long hold_need = (long)llround((tail_hold_ms	> 0.0 ? tail_hold_ms	: 0.0) * out_sr / 1000.0);
	const long rel_need = (long)llround((tail_release_ms > 0.0 ? tail_release_ms : 0.0) * out_sr / 1000.0);
	long hold_cnt = 0;
	long rel_cnt = 0;

	//decoupling for ultra-quiet passages when global dither is off
	const double decouple_min_lsb = 0.90;	//strong enough to break idle tones without obvious hiss
	double decouple_on_lsb = (quiet_lsb > 0.0) ? quiet_lsb : 6.0;

	size_t clipped = 0;

	//clamp shape range defensively
	if(shape < 0.0)
		shape = 0.0;
	if(shape > 0.95)
		shape = 0.95;

	for(size_t i=0; i<n; i++){
		//apply gain and hard-clip to [-1,1]
		double y = x[i] * g;
		if(y > 1.0){
			y = 1.0;
			clipped++;
		}else if(y < -1.0){
			y = -1.0;
			clipped++;
		}

		//quantizer domain (−128..+127) and instantaneous magnitude
		const double s = y * 127.0;
		const double amag = fabs(s);

		//shaping on/off with hysteresis
		if(on_thr > 0.0){//only if thresholding requested
			if(amag < off_thr)
				shaping_on = 0;
			else if(amag > on_thr)
				shaping_on = 1;
		}

		//force shaping off a bit earlier when global dither is fully off (prevents idle tones)
		int shaping_gate_off = 0;
		if(dither_mode == 0){
			if(amag_lp < decouple_on_lsb * 1.25)
				shaping_gate_off = 1;
		}

		const double a_pre = shaping_on ? shape : 0.0;
		double a_gate = a_pre;
		if(shaping_gate_off)
			a_gate = 0.0;

		if(a_gate == 0.0)
			e1 *= 0.5;

		const double shaped = s - a_gate * e1;	//error feedback

		amag_lp = (1.0 - alpha)*amag_lp + alpha*amag;	//unconditional amplitude smoothing (used by tail gate & auto dither)

		double d_amp = 0.0;//dither amount
		if(dither_mode == 1){
			d_amp = 1.0;	//full-scale TPDF
		}else if(dither_mode == 2){
			//scale with level around the shaping-on reference
			const double ref = (on_thr > 0.0) ? on_thr : 1.0;
			double r = (ref > 0.0) ? (amag_lp / ref) : 1.0;
			if(r < 0.0)
				r = 0.0;
			if(r > 1.0)
				r = 1.0;
			d_amp = r;
		}//else 0.0 when off

		//tail-silence detector (optional)
		if(tail_enabled){
			const double tail_on = tail_lsb;
			const double tail_off = tail_lsb * 2.0;	//hysteresis

			if(!tail_silent){
				if(amag_lp <= tail_on){
					if(hold_cnt < hold_need)
						hold_cnt++;
					if(hold_need > 0 && hold_cnt >= hold_need){
						tail_silent = 1;
						e1 = 0.0;	//clear error memory on entering silence
						hold_cnt = 0;
						rel_cnt = 0;
					}
				}else{
					hold_cnt = 0;
				}
			}else{
				if(amag_lp >= tail_off){
					if(rel_cnt < rel_need)
						rel_cnt++;
					if(rel_need == 0 || rel_cnt >= rel_need){
						tail_silent = 0;
						rel_cnt = 0;
					}
				}else{
					rel_cnt = 0;
				}
			}
		}

		//apply dither unless tail-forced silence; if global dither is off, add minimal decoupling on ultra-quiet
		double d = 0.0;
		if(!tail_silent && d_amp > 0.0){
			d = tpdf() * d_amp;
		}else if(!tail_silent && d_amp == 0.0){
			if(amag_lp < decouple_on_lsb * 1.50){
				d = tpdf() * decouple_min_lsb;
			}
		}

		//quantize (or force midcode while tail-silent)
		long q;
		if(tail_silent){
			q = 0;	//midcode => unsigned 0x80
		}else{
			double qf = shaped + d;
			if(qf > 127.0)
				qf = 127.0;
			if(qf < -128.0)
				qf = -128.0;
			q = lround(qf);
			if(q < -128)
				q = -128;
			if(q > 127)
				q = 127;
		}

		//dead-band freeze near zero when global dither is off (prevents last-second crunch)
		if(!tail_silent){
			if(dither_mode == 0){
				if(amag_lp < 0.75){
					if(fabs(e1) < 0.75)
						q = 0;
				}
			}
		}

		if(!tail_silent){//update error memory...
			e1 = (double)q - s;
			if(dither_mode == 0){
				if(amag_lp < 0.75){
					if(fabs(e1) < 0.75)
						e1 = 0.0;
				}
			}
			if(e1 > e1_clip)
				e1 = e1_clip;
			if(e1 < -e1_clip)
				e1 = -e1_clip;
		}else{//...or clear while silent
			e1 = 0.0;
		}

		const uint8_t u = (uint8_t)(q + 128);//bias to unsigned and write
		fwrite(&u,1,1,f);
	}

	if(align_bytes){//sector/byte alignment: pad with 0x80
		long pos = ftell(f);
		if(pos >= 0){
			long r = pos % (long)align_bytes;
			if(r){
				long need = (long)align_bytes - r;
				const uint8_t pad = 0x80;
				for(long i=0; i<need; i++)
					fwrite(&pad,1,1,f);
			}
		}
	}

	fclose(f);
	if(out_clipped)
		*out_clipped = clipped;
	return 0;
}

/* ===== path helpers ===== */
static const char* base_name(const char* p){
	const char *s = strrchr(p,'/'), *t = strrchr(p,'\\');
	const char *m = (s&&t)?((s>t)?s:t):(s? s:(t? t:p-1));
	return m? m+1 : p;
}


static void stem_from_basename(const char* b, char* dst, size_t dstsz){
	const char* dot = strrchr(b,'.');
	size_t n = dot? (size_t)(dot - b) : strlen(b);
	if(n >= dstsz)
		n = dstsz-1;
	memcpy(dst,b,n);
	dst[n] = '\0';
}


/* ===== name templating ===== */
static int has_dirsep(const char* s){
	for(; *s; s++){
		if(*s == '/' || *s == '\\')
			return 1;
	}
	return 0;
}


static void format_name(const char* templ, const char* stem, long rate_i, size_t samples, const char* ext_no_dot, char* out, size_t outsz){
	//tokens: {stem},{rate},{samples},{ext}
	size_t oi=0;
	for(size_t i=0; templ[i] && oi+1<outsz;){
		if(templ[i] == '{' ){
			const char *start = templ+i+1;
			const char *end = strchr(start,'}');
			if(end){
				size_t len = (size_t)(end-start);
				char tok[32];
				if(len >= sizeof(tok))
					len = sizeof(tok)-1;
				memcpy(tok,start,len);
				tok[len] = '\0';
				const char *rep = "";
				char numbuf[32];

				if(!strcmp(tok,"stem"))
					rep = stem;
				else if(!strcmp(tok,"rate")){
					snprintf(numbuf,sizeof(numbuf),"%ld", rate_i);
					rep = numbuf;
				}else if(!strcmp(tok,"samples")){
					snprintf(numbuf,sizeof(numbuf),"%zu", samples);
					rep = numbuf;
				}else if(!strcmp(tok,"ext"))
					rep = ext_no_dot;
				else{
					rep = "";
				}

				size_t rl = strlen(rep);
				if(oi+rl >= outsz)
					rl = outsz - 1 - oi;
				memcpy(out+oi, rep, rl);
				oi += rl;
				i = (size_t)(end - templ) + 1;
				continue;
			}
		}
		out[oi++] = templ[i++];
	}
	out[oi]='\0';
}

/* ===== CSV (quoted) ===== */
typedef struct{
	char in[2048];
	char out[2048];
	double gain_db;
	int has_out;
	int has_gain;
}job_t;


static int parse_csv_line(const char* line, char* f1, size_t n1, char* f2, size_t n2, char* f3, size_t n3){
	int field = 0, inq = 0;
	const char *p = line;
	size_t wi = 0;
	char* outs[3] = { f1, f2, f3 }; size_t lim[3] = { n1, n2, n3 };
	if(f1&&n1)
		f1[0] ='\0';
	if(f2&&n2)
		f2[0] ='\0';
	if(f3&&n3)
		f3[0] ='\0';
	while(*p && *p != '\n' && *p != '\r'){
		char c = *p++;
		if(inq){
			if(c == '"'){
				if(*p == '"'){//escaped quote
					if(field < 3 && wi+1 < lim[field])
						outs[field][wi++] = '"';
					p++;
				}else
					inq = 0;
			}else{
				if(field < 3 && wi+1 < lim[field])
					outs[field][wi++] = c;
			}
		}else{
			if(c == ','){
				if(field < 3 && outs[field])
					outs[field][wi] = '\0';
				field++;
				wi=0;
			}else if(c == '"'){
				inq = 1;
			}else{
				if(field < 3 && wi+1 < lim[field])
					outs[field][wi++] = c;
			}
		}
	}
	if(field<3 && outs[field])
		outs[field][wi] = '\0';
	return field+1;	//number of fields seen
}


static int load_csv(const char* path, job_t** jobs, size_t* njobs){
	FILE* f=fopen(path,"r");
	if(!f){
		fprintf(stderr,"ERROR: csv %s: %s\n",path,strerror(errno));
		return -1;
	}
	char line[4096];
	size_t cap = 0,n = 0;
	*jobs = NULL;
	*njobs = 0;
	while(fgets(line,sizeof(line),f)){
		//skip blank / comment lines
		const char* p = line;
		while(*p == ' '||*p == '\t')
			p++;
		if(*p == '\0'||*p == '\n'||*p == '#')
			continue;

		char a[2048]={0}, b[2048]={0}, c[256]={0};
		int nf = parse_csv_line(p, a,sizeof(a), b,sizeof(b), c,sizeof(c));
		if(nf <= 0 || !*a)
			continue;

		if(n == cap){
			cap = cap ? cap*2:16;
			*jobs = (job_t*)realloc(*jobs,cap*sizeof(job_t));
		}
		job_t *j = &(*jobs)[n];
		memset(j,0,sizeof(*j));
		snprintf(j->in,sizeof(j->in),"%s",a);
		if(*b){
			snprintf(j->out,sizeof(j->out),"%s",b);
			j->has_out = 1;
		}
		if(*c){
			j->gain_db=atof(c);
			j->has_gain = 1;
		}
		n++;
	}
	fclose(f);
	*njobs = n;
	return 0;
}

/* ===== CLI ===== */
static void usage(void){
	fprintf(stderr,
"wave2raw — HQ resample to raw unsigned 8-bit mono @ 15720 Hz (default)\n"
"Usage:\n"
"\twave2raw [options] input.wav [more.wav ...]\n"
"\twave2raw --csv list.csv [options]\n\n"
"Options:\n"
"-r <rate>			Target sample rate number (e.g. 15720, 15734.26) or 'ntsc' or 'uzebox'\n"
"-o <dir>			Output directory (created if needed)\n"
"--name <tmpl>			Name template (tokens: {stem},{rate},{samples},{ext}); else uses stem+suffix\n"
"--suffix <s>			Output suffix (default .raw) (ignored if --name is used)\n"
"--align-frames <N|auto>		Pad samples so count is multiple of N; 'auto' uses round(rate/frame-rate)\n"
"--frame-rate <Hz>		Frame rate for 'auto' (default 60)\n"
"--align-bytes <N>		Pad file to multiple of N bytes with 0x80 (e.g., 512)\n"
"--csv <file>			CSV list: input[,output][,gain_db]  (quoted fields supported)\n"
"--gain <dB>			Extra post-normalize gain (default 0)\n"
"--shape <0..0.95>		Error-feedback amount (default 0.85)\n"
"--shape-quiet-lsb <N>		Disable shaping when |signal| < N LSB (hysteresis at 2×; default 4)\n"
"--dither <off|tpdf|auto>	Dither mode (default tpdf; try 'auto' to quiet fades)\n"
"--tail-silence-lsb <N>		After long quiet, force digital silence below N LSB (off if 0)\n"
"--tail-silence-hold <ms>	How long below threshold before silencing (default 100)\n"
"--tail-silence-release <ms>	Delay to release once level rises (default 20)\n"
);
}


static int ieq(const char* a, const char* b){
	for(; *a||*b; a++,b++){
		if(tolower((unsigned char)*a) != tolower((unsigned char)*b))
			return 0;
	}
	return 1;
}


int main(int argc, char** argv){
	int argi = 1;
	double target_sr = 15720.0;
	const char *outdir = "";
	const char *name_tmpl = NULL;
	const char *suffix = ".raw";
	const char *csv = NULL;
	double post_gain_db = 0.0;
	size_t align_frames = 0;	//0 = off, else multiple
	int align_frames_auto = 0;
	double frame_rate = 60.0;
	size_t align_bytes = 0;
	double shape = 0.85;
	double shape_quiet_lsb = 4.0;
	int dither_mode = 2;		//0=off, 1=tpdf (full-scale), 2=auto (level-adaptive)
	double tail_lsb = 0.0;		//0 => disabled
	double tail_hold_ms = 100.0;
	double tail_release_ms = 20.0;

	if(argc<2){
		usage();
		return 1;
	}

	while(argi < argc && argv[argi][0] == '-'){
		const char *a = argv[argi++];
		if(!strcmp(a,"-r") && argi < argc){
			const char* r=argv[argi++];
			if(ieq(r,"ntsc"))
				target_sr = 15734.26;
			else if(ieq(r,"uzebox"))
				target_sr = 15720.0;
			else
				target_sr = atof(r);
		}else if(!strcmp(a,"-o") && argi < argc){
			outdir = argv[argi++];
		}else if(!strcmp(a,"--name") && argi < argc){
			name_tmpl = argv[argi++];
		}else if(!strcmp(a,"--suffix") && argi < argc){
			suffix = argv[argi++];
		}else if(!strcmp(a,"--align-frames") && argi < argc){
			const char* v = argv[argi++];
			if(ieq(v,"auto")){
				align_frames_auto = 1;
			}else{
				align_frames = (size_t)strtoul(v,NULL,10);
				align_frames_auto = 0;
			}
		}else if(!strcmp(a,"--frame-rate") && argi < argc){
			frame_rate = atof(argv[argi++]);
		}else if(!strcmp(a,"--align-bytes") && argi < argc){
			align_bytes = (size_t)strtoul(argv[argi++],NULL,10);
		}else if(!strcmp(a,"--csv") && argi < argc){
			csv = argv[argi++];
		}else if(!strcmp(a,"--gain") && argi < argc){
			post_gain_db = atof(argv[argi++]);
		}else if(!strcmp(a,"--shape") && argi < argc){
			shape = atof(argv[argi++]);
			if(shape < 0.0)
				shape = 0.0;
			if(shape > 0.95)
				shape = 0.95;
		}else if(!strcmp(a,"--shape-quiet-lsb") && argi < argc){
			shape_quiet_lsb = atof(argv[argi++]);
			if(shape_quiet_lsb < 0.0)
				shape_quiet_lsb = 0.0;
		}else if(!strcmp(a,"--dither") && argi < argc){
			const char* dm = argv[argi++];
			if(ieq(dm,"off")){
				dither_mode = 0;
			}else if(ieq(dm,"auto")){
				dither_mode = 2;
			}else{
				dither_mode = 1;
			}
		}else if(!strcmp(a,"--tail-silence-lsb") && argi < argc){
			tail_lsb = atof(argv[argi++]);
			if(tail_lsb < 0.0)
				tail_lsb = 0.0;
		}else if(!strcmp(a,"--tail-silence-hold") && argi < argc){
			tail_hold_ms = atof(argv[argi++]);
			if(tail_hold_ms < 0.0)
				tail_hold_ms = 0.0;
		}else if(!strcmp(a,"--tail-silence-release") && argi < argc){
			tail_release_ms = atof(argv[argi++]);
			if(tail_release_ms < 0.0)
				tail_release_ms = 0.0;
		}else{
			usage();
			return 1;
		}

		if(outdir && *outdir){
			if(mkoutdir(outdir) != 0){
				fprintf(stderr,"ERROR: cannot create %s\n",outdir);
				return 1;
			}
		}
	}

	job_t* jobs = NULL;
	size_t nj = 0;
	if(csv){
		if(load_csv(csv,&jobs,&nj) != 0)
			return 1;
	}else{
		if(argi >= argc){
			usage();
			return 1;
		}
		nj = (size_t)(argc-argi);
		jobs = (job_t*)calloc(nj,sizeof(job_t));
		for(size_t i=0; i<nj; i++)
			snprintf(jobs[i].in,sizeof(jobs[i].in),"%s",argv[argi+(int)i]);
	}

	int exit_code = 0;
	for(size_t ji=0; ji<nj; ji++){
		const char* inpath = jobs[ji].in;
		char outpath[4096];
		if(jobs[ji].has_out){
			//CSV output wins; prepend outdir if it's a bare filename
			if(outdir && *outdir && !has_dirsep(jobs[ji].out))
				snprintf(outpath,sizeof(outpath),"%s/%s",outdir,jobs[ji].out);
			else
				snprintf(outpath,sizeof(outpath),"%s",jobs[ji].out);
		}else{
			const char* b = base_name(inpath);
			char stem[2048];
			stem_from_basename(b, stem, sizeof(stem));
			long rate_i = (long)llround(target_sr);
			const char* ext = suffix;
			const char* e = (*ext=='.')? (ext+1) : ext;
			char namebuf[3072];
			if(name_tmpl && *name_tmpl){
				//samples unknown yet; temp 0 -> we’ll refill after resample.
				format_name(name_tmpl, stem, rate_i, 0, e, namebuf,sizeof(namebuf));
				if(outdir && *outdir && !has_dirsep(namebuf))
					snprintf(outpath,sizeof(outpath), "%s/%s", outdir, namebuf);
				else
					snprintf(outpath,sizeof(outpath), "%s", namebuf);
			}else{//default stem + suffix
				if(outdir && *outdir)
					snprintf(outpath,sizeof(outpath), "%s/%s%s", outdir, stem, suffix);
				else
					snprintf(outpath,sizeof(outpath), "%s%s", stem, suffix);
			}
		}

		wav_t w;
		if(load_wav(inpath,&w) != 0){
			exit_code=1;
			continue;
		}

		hpf_dc_inplace(w.mono, w.frames, (double)w.sr);//DC removal

		vecd_t y = resample_sinc_auto(w.mono, w.frames, (double)w.sr, target_sr);
		free(w.mono);
		if(!y.out || !y.n){
			fprintf(stderr,"ERROR: resample failed: %s\n",inpath);
			exit_code=1;
			continue;
		}

		//normalize
		normalize_truepeak_safe(y.out, y.n);

		//frame alignment(pad)
		if(align_frames_auto){
			long frames_per = (long)llround(target_sr / ((frame_rate>0)? frame_rate : 60.0));
			if(frames_per < 1)
				frames_per = 1;
			align_frames = (size_t)frames_per;
		}
		if(align_frames)
			pad_samples_to_multiple(&y.out, &y.n, align_frames);

		//if using name template, rebuild name now that {samples} is known
		if(!jobs[ji].has_out && name_tmpl && *name_tmpl){
			const char* b = base_name(inpath);
			char stem[2048];
			stem_from_basename(b, stem, sizeof(stem));
			long rate_i = (long)llround(target_sr);
			const char* ext = suffix;
			const char* e = (*ext=='.')? (ext+1) : ext;
			char namebuf[3072];
			format_name(name_tmpl, stem, rate_i, y.n, e, namebuf,sizeof(namebuf));
			if(outdir && *outdir && !has_dirsep(namebuf))
				snprintf(outpath,sizeof(outpath), "%s/%s", outdir, namebuf);
			else
				snprintf(outpath,sizeof(outpath), "%s", namebuf);
		}

		//write (quantize + sector/byte pad), count clipping
		size_t clipped=0;
		if(write_u8_raw_shaped(	outpath, y.out, y.n, post_gain_db + (jobs[ji].has_gain ? jobs[ji].gain_db : 0.0),
					&clipped, align_bytes, shape, shape_quiet_lsb, dither_mode, target_sr, tail_lsb, tail_hold_ms, tail_release_ms) != 0){
			exit_code=1;
		}else{
			fprintf(stdout,"OK: %s -> %s  (%.2f Hz -> %.2f Hz, %zu -> %zu samples)\n", inpath, outpath, (double)w.sr, target_sr, w.frames, y.n);
			if(clipped){
				fprintf(stderr,"WARNING: %zu samples clipped in %s\n", clipped, inpath);
			}
		}
		free(y.out);
	}
	free(jobs);
	return exit_code;
}