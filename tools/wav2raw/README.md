# wav2raw

High-quality, single-file C99 tool to convert uncompressed WAV to **raw unsigned 8-bit mono** at a fixed game-friendly rate (default **15720 Hz**), with **Audacity-class** resampling and no external libraries.

- Anti-aliasing via **Kaiser-windowed sinc** (~100 dB stopband)  
- **DC offset removal** (10 Hz high-pass)  
- **True-peak-safe normalization** to **−0.3 dBTP**  
- **Noise-shaped TPDF dither** to 8-bit (configurable)  
- **Always overwrites** output files  
- **Quoted CSV** batch mode  
- Optional **frame** and **sector/byte** alignment  
- Supports **WAVEFORMATEXTENSIBLE** (valid-bits < container)  
- Portable (Linux, macOS, MinGW/Windows)

---

## Build

```sh
# Linux / macOS (Clang or GCC)
cc -O3 -std=c99 -lm -o wav2raw wav2raw.c

# Windows (MinGW)
gcc -O3 -std=gnu99 -o wav2raw.exe wav2raw.c -lm
```

No external dependencies beyond the C math library.

---

## Quick start

```sh
# Default: somefile.wav -> somefile.raw (unsigned 8-bit mono @ 15720 Hz)
./wav2raw somefile.wav

# Place outputs in a directory (created if missing; single level)
./wav2raw -o out/ song1.wav song2.wav

# Name templating (tokens: {stem},{rate},{samples},{ext})
./wav2raw --name "{stem}_{rate}_{samples}.{ext}" theme.wav

# Uzebox or exact NTSC-ish rate
./wav2raw -r uzebox   input.wav   # 15720.0 Hz
./wav2raw -r ntsc     input.wav   # 15734.26 Hz
./wav2raw -r 16000    input.wav   # any numeric rate

# Align to video frames (auto = round(rate/frame_rate))
./wav2raw --align-frames auto --frame-rate 60 cut.wav

# Pad output file to 512-byte boundary (0x80 fill)
./wav2raw --align-bytes 512 jingle.wav

# CSV batch (quoted fields OK): input[,output][,gain_db]
./wav2raw --csv jobs.csv -o out/
```

---

## Input / Output

- **Input**: RIFF/WAVE, little-endian, **uncompressed** PCM (8/16/24/32-bit) or **float32**, mono or stereo.  
  WAVEFORMATEXTENSIBLE is supported (uses **valid bits** if smaller than container width).
- **Output**: raw unsigned 8-bit, **mono**, target rate (default **15720 Hz**).  
- **Default output name**: `somefile.wav` → `somefile.raw` (same directory).  
  Use `-o DIR/` to redirect, or `--name` to control the filename.

---

## Options

```
-r <rate>           Target sample rate number (e.g., 15720, 15734.26)
                    or keywords:  'ntsc' (15734.26)  |  'uzebox' (15720)

-o <dir>            Output directory. Created if missing (one level).

--name <template>   Name template. Tokens:
                      {stem}    base name without extension
                      {rate}    integer-rounded target rate
                      {samples} output sample count (after alignment)
                      {ext}     extension without the dot
                    Example:  --name "{stem}_{rate}_{samples}.{ext}"

--suffix <s>        Output suffix if --name is not used (default: .raw)

--align-frames <N|auto>
                    Pad with silence to make the sample count a multiple of N.
                    'auto' uses round(rate / frame_rate).  Padding only; no trim.

--frame-rate <Hz>   Frame rate for --align-frames auto (default: 60)

--align-bytes <N>   Pad file to a multiple of N bytes using 0x80 (U8 silence)

--csv <file>        Batch mode. CSV rows: input[,output][,gain_db]
                    Quoted fields and escaped quotes ("") supported.

--gain <dB>         Extra gain AFTER normalization (e.g., --gain -3.0)
```

### Quantization / Dither controls

```
--shape <0..0.95>           Error-feedback amount (default 0.85)
--shape-quiet-lsb <N>       Disable shaping when |signal| < N LSB
                            Hysteresis at 2×N. Default 4.0 LSB.
--dither <off|tpdf|auto>    Dither mode (default: tpdf)
                              off  : no dither (can produce “crunch” on fades)
                              tpdf : full-scale TPDF
                              auto : level-adaptive TPDF (quieter on fades)
```

**What these mean**

- **Error-feedback shaping** (“shape”): pushes quantization noise upward in frequency, preserving detail.  
  Higher = crisper highs but potentially “buzzy” on ultra-quiet tails. Lower = softer top-end but cleaner fades.
- **Quiet shaping gate** (`--shape-quiet-lsb`): turns shaping off near silence to avoid hiss/buzz buildup.  
  Units are **LSB** in the 8-bit quantizer domain (1 LSB ≈ one U8 step).
- **Dither**:
  - `tpdf` adds a tiny, uniform noise that decorrelates quantization (classic mastering choice).
  - `auto` scales that noise down as the level drops (nice for fade-outs and SFX tails).
  - `off` is only recommended for already-noisy material; otherwise fades can “crunch”.

### Tail silence gate (optional)

```
--tail-silence-lsb <N>      After sustained quiet, force exact digital silence
                            when level ≤ N LSB (0 = disabled). Default 0 (off).
--tail-silence-hold <ms>    How long the smoothed level must stay below the
                            threshold before silencing (default 100 ms).
--tail-silence-release <ms> Delay to release once level rises above 2×threshold
                            (default 20 ms).
```

- Uses the same internal, fast envelope as the dither logic.  
- Great for chopping residual hiss after a fade-out, or between SFX.

---

## Alignment options

### Frame alignment
Use when you want an exact number of samples per video frame for frame-sync playback.

- `--align-frames N` : pad to a multiple of `N` samples.
- `--align-frames auto --frame-rate 60` : pad to a multiple of `round(rate/60)`.

Examples (rounded per option):  
- 15720 Hz @ 60 Hz → **262** samples/frame  
- 15734.26 Hz @ 60 Hz → ≈262.24 → **262**

Padding is silence (0x80). No trimming.

### Sector / byte alignment
`--align-bytes 512` pads the **file** size to a 512-byte multiple using 0x80 (U8 silence). Useful for SD/SPI-RAM sectors.

---

## Quality pipeline

1. **Parse WAV** (PCM 8/16/24/32 or float32; WAVEFORMATEXTENSIBLE honored), stereo → mono (average).  
2. **DC removal**: 10 Hz first-order high-pass.  
3. **Resample**: Kaiser-windowed sinc (~100 dB stopband).  
4. **Normalize**: true-peak-safe to **−0.3 dBTP**.  
5. **Quantize**: to unsigned 8-bit with configurable **dither**/**shaping**.  
6. **Optional padding**: frame and/or byte alignment.

---

## Presets (copy/paste)

**General music (transparent, crisp highs, clean fades)**
```sh
--dither auto --shape 0.85 --shape-quiet-lsb 3
```

**Problem fades (remove “crunch” at tail, still natural)**
```sh
--dither auto --shape 0.80 --shape-quiet-lsb 4 \
--tail-silence-lsb 1.0 --tail-silence-hold 450 --tail-silence-release 80
```

**SFX with heavy → quiet tails (keep attack sparkle, calm endings)**
```sh
--dither auto --shape 0.85 --shape-quiet-lsb 3 \
--tail-silence-lsb 0.8 --tail-silence-hold 300 --tail-silence-release 60
```

**Absolute silence between cues (menu beeps, etc.)**
```sh
--dither auto --shape-quiet-lsb 4 \
--tail-silence-lsb 1.5 --tail-silence-hold 200 --tail-silence-release 60
```

**Very bright content that feels “buzzy”**
```sh
--dither tpdf --shape 0.70 --shape-quiet-lsb 4
```

---

## Troubleshooting & recipes

- **“Hiss follows the amplitude” on quiet parts**  
  Use adaptive dither and a quiet-gate for shaping:  
  `--dither auto --shape-quiet-lsb 3`  
  (Optionally reduce shape a touch: `--shape 0.80`.)

- **“Crunch” on fade-outs** (noisy or zipper-like tail)  
  `--dither auto --shape-quiet-lsb 4`  
  If still present, add a gentle tail gate:  
  `--tail-silence-lsb 0.8 --tail-silence-hold 400 --tail-silence-release 80`

- **Tail gets cut off too early**  
  Increase hold or lower threshold:  
  `--tail-silence-hold 600` or `--tail-silence-lsb 0.6`

- **Too much top-end “zing” / fatiguing**  
  Reduce shaping: `--shape 0.70` (trade a little sparkle for smoother noise).

- **Clipping warning printed**  
  Lower final loudness via `--gain -3` (applied after normalization),  
  or pre-limit the source.

- **I want true “as quiet as possible” fades without hard mute**  
  Skip the tail gate; rely on `--dither auto --shape-quiet-lsb 4`.

---

## CSV format (quoted fields supported)

- Each line: `input[,output][,gain_db]`  
- Lines beginning with `#` are ignored.  
- Fields may be quoted; quotes inside a quoted field are escaped by doubling.

Examples:
```
"in drums/kick.wav","kick_uz.raw",-3.0
"Lead A.wav",,0
pads.wav,"out/pads_uze.raw",
"fx, sweep.wav","fx_sweep.raw",-6.5
```

If the CSV omits an output path, default naming applies (and `-o DIR` will prepend the directory).

---

## Why these rates?

- **15720.0 Hz** (`-r uzebox`) is convenient for 60 FPS sync (**262 samples/frame**).  
- **15734.26 Hz** (`-r ntsc`) matches the nominal NTSC line cadence if your engine prefers that.

Pick the one your playback engine expects.

---

## License

**GPLv3-or-later**
Copyright 2025 Lee Weber (D3thAdd3r)
