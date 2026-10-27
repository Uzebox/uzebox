#!/usr/bin/env python3
"""uzm.py -- makes Uzeamp video files (.UZM). See ../video-study.md, ../plan-rates.md and
../uzmmode/uzm.def.h.

    python -I uzm.py test OUT.UZM [seconds] [--rate R] [--v1|--v2]
                                                  a test pattern: colour bars, gradients, a box
                                                  moving a pixel a picture, the picture number,
                                                  and a 440 Hz tone, continuous across frames
                                                  (default 10 s, 60 pictures a second)
    Version 2 is written with --v2, --rate other than 60, or --ivtc; version 1 otherwise, until
    Uzeamp reads version 2 (plan-rates.md, M3: then version 2 becomes the default)
    python -I uzm.py encode IN OUT.UZM [options]  a video file, through ffmpeg
        --rate auto|60|30|25|24                   pictures a second (default auto: from IN's
                                                  frame rate). Each picture is stored once and
                                                  shown 1-3 times; 60 shows each once
        --ivtc                                    IN carries film with its frames repeated (24 fps
                                                  animation in a 60 fps file, say): each film
                                                  frame once (every other frame, then ffmpeg's
                                                  decimate drops 1 in 5), on the film's grid
                                                  (60/59.94 x 2/5, 30/29.97 x 4/5); the rate
                                                  then defaults to the film's
        --v1 / --v2                               the format (version 1: 60 Hz only, what Uzeamp
                                                  plays until plan-rates M3)
        --title T --artist A                      shown by the player
        --start S --duration D                    a part of IN (seconds)
        --dither ours|bayerN|fs|sierra2|sierra2_4a|sierra3|burkes|atkinson|none
                                                  ours (the default since 2026-10-26): Sierra
                                                  2-4A in uzmdither.c, each channel on its own,
                                                  its error kept. The others are ffmpeg's
                                                  (paletteuse; sierra2_4a was the default):
                                                  in near-black areas it reddens, as a tie
                                                  between black and the darkest colours goes
                                                  to dark red (0x01) and the red error below
                                                  0 is dropped (Jedi's space (1,1,1) came out
                                                  red at 32 on average, ours 14 as the source
                                                  around it). Bayer's pattern shows on dark
                                                  backgrounds, atkinson drops them to black
        --temporal                                temporal dithering: each showing of a picture
                                                  (2 at 30, 2-3 at 24/25) dithered on its own,
                                                  each aiming at what the ones before missed,
                                                  so the eye's average comes closer to the
                                                  source than one picture can (a flicker of
                                                  about one level at 20-30 Hz). Stored at 60:
                                                  every showing a picture (70 MB a minute;
                                                  FAT32's 4 GB: 58 minutes at most).
                                                  Our own Sierra 2-4A, not ffmpeg's, in
                                                  uzmdither.c (built with gcc on first use:
                                                  3 minutes of film in 30 s); in Python
                                                  without it, 100 times slower
        --black N                                 levels 0..N become black (dark scenes
                                                  can depend on them: compare first)
        --cut N                                   pixels whose brightest channel is <= N become
                                                  black, nothing else changes (a near-black
                                                  background made black; try 24)
        --pre F / --post F                        ffmpeg filters before / after the scale; for
                                                  upscaled pixel art, back to its own size
                                                  first: --pre scale=384:224:flags=area
                                                  --post unsharp=3:3:0.6
        --scaler S                                the scale's method (default lanczos)
        --letterbox                               keep IN's aspect: black bars, top and bottom
        --loudness N|off                          the sound's loudness in LUFS (default -15, the
                                                  level the movies made before 2026-10-25 had
                                                  and that was heard beside songs mastered at
                                                  0 dB; two-pass EBU R128, on the mono mix);
                                                  off: the source's level
        --compress                                a film's range made smaller before the loudness
                                                  (ffmpeg's dynaudnorm, 250 ms windows): its quiet
                                                  scenes lifted, its loud ones kept under the peak.
                                                  A film mixed for a cinema (The Matrix: 24 dB
                                                  between its peaks and its mean) is otherwise
                                                  too quiet in its quiet parts on a TV, 8-bit
        --gpu                                     decode on the GPU (NVDEC, -hwaccel cuda;
                                                  falls back to the CPU). Same output; not
                                                  faster here: the packing in Python dominates
        --crop W:H:X:Y                            a part of IN's picture first (ffmpeg's crop)
        --audio N                                 IN's audio track N (0 the first: ffmpeg's
                                                  -map 0:a:N; a film's other languages)
    python -I uzm.py info FILE.UZM                the header, and a check of every sector
    python -I uzm.py frame FILE.UZM N OUT.png [--frame]
                                                  picture N as an image, 4x; with --frame, N is
                                                  a console frame (60.05 a second)
    python -I uzm.py sound FILE.UZM OUT.wav       the sound as the player plays it, frame by
                                                  frame: each frame's 262 samples from its
                                                  picture's trailers
    python -I uzm.py cadence IN [--start S] [--duration D]
                                                  which rate IN's pictures really have: where
                                                  its new pictures fall in each group of 5
                                                  frames (film carried at 60 or 30 fps repeats
                                                  its frames in a fixed pattern: --ivtc) and
                                                  how long each lasts

The file (version 2; version 1 below)
  Sector 0, the header (read with uzefs_read, so any bytes):
     0  "UZM1"           4  u16 version (2)     6  u16 width (160)    8  u16 rows (114)
    10  u16 sectors a picture (38)             12  u32 pictures      16  u16 samples a frame (262)
    18  u16 first picture's sector (1)         20  u8 N, 21 u8 M: the cadence
    32  title (64 bytes)                       96  artist (64 bytes)
  The console shows 60.05 frames a second (262 lines of 1820 cycles). Every N frames show M
  pictures: picture p is shown from frame floor(p N / M) to the next picture's first, 1-3
  times (60: 1/1, 30: 2/1, 25: 12/5, 24: 5/2), so the pictures come at 60.05 M / N a second.
  The player shows a picture again by restarting the card's read at it.
  Then the pictures, 38 sectors each. Sector s of a picture holds rows 3s..3s+2 (160 bytes
  each, a byte a pixel, BBGGGRRR) and a 32-byte trailer:
     0  reserved (0)
     1  showing 1's samples: 8 (unsigned, 15,734 Hz)
     9  showing 2's: 8
    17  showing 3's: 8
    25  the picture's number in 3 bytes of 7 bits (low first), then s
    29  3 bytes chosen so that the sector's CRC16 is FEFE, FEFF, FFFE or FFFF
  A showing is a frame: 262 samples, one a line. Only sectors 2-34 carry samples: sector s
  holds a showing's samples 8(s-2)..8(s-2)+7, sector 34 holds 254..261, the 2 before again, so
  every sector writes 8 and the 262 end exactly at the sound bank's end. The 2 sectors at each
  end hold none: the player can hide those rows (6 at the top, 6 at the bottom) on a slow card
  and keep the sound. Unused sets are zeros.
  No byte of a picture's sector is 0xFE or 0xFF, and with that CRC the player keeps every byte
  below 0xFE that the card sends in a multiple-block read: CRCs, waits and tokens all drop out.

Version 1 (60 Hz only): the header's version 1, no cadence, 16 the samples a picture (262). The
  trailer: 0 back (0, or 4 in sector 37: its samples start 4 before), 1 7 samples (sectors 0-36
  hold samples 7s..7s+6, sector 37 255..261), 8 the picture's number and s, 12 zeros, 29 the CRC
  fix bytes.
"""
import binascii
import json
import os
import re
import struct
import subprocess
import sys
import tempfile
import wave

W, ROWS, ROWS_SEC, SECTORS, SAMPLES = 160, 114, 3, 38, 262
F_CPU, LINE, LINES = 28636360, 1820, 262
FPS = F_CPU / (LINE * LINES)                      # 60.0546
AUDIO_RATE = 15734
CRC_TARGETS = (0xFEFE, 0xFEFF, 0xFFFE, 0xFFFF)
# the format written when nothing asks for version 2: 1 until Uzeamp reads version 2
# (plan-rates.md, M3), then 2
DEFAULT_VERSION = 2
MAXB = 0xFD

# version 2's trailer: 3 sets of 8 samples (one a showing), then the mark
SET, SETS = 8, 3
T_SAMPLES, T_MARK = 481, 505                      # in the sector (480 + the trailer's offset)
SAMPLE_SECTORS = range(2, 35)                     # 2..34; 34 starts 2 back
LAST_BACK = 8 * len(SAMPLE_SECTORS) - SAMPLES     # 2

# the cadences: pictures a second (nominal) -> (N frames, M pictures)
RATES = {60: (1, 1), 30: (2, 1), 25: (12, 5), 24: (5, 2)}


def start_frame(p, n, m):
    """The first frame that shows picture p."""
    return p * n // m


def showings(p, n, m):
    return start_frame(p + 1, n, m) - start_frame(p, n, m)


def picture_of(f, n, m):
    """The picture frame f shows, and which showing it is (0-based)."""
    p = f * m // n
    while start_frame(p + 1, n, m) <= f:
        p += 1
    while start_frame(p, n, m) > f:
        p -= 1
    return p, f - start_frame(p, n, m)


def sample_slot(s):
    """Where sector s's 8 samples go in a showing's 262 (None: s holds none)."""
    if s not in SAMPLE_SECTORS:
        return None
    if s == SAMPLE_SECTORS[-1]:
        return SAMPLES - SET
    return SET * (s - SAMPLE_SECTORS[0])


# --- the CRC fix bytes ------------------------------------------------------------------------
# crc_hqx is CRC-16/XMODEM (poly 0x1021, init as given), the CRC an SD card sends. The fix bytes
# are the sector's last 3: c0 is the CRC over the first 509; the last 3 bytes (a, b, c) must
# take it to a target. crc(x y, s) = crc(x y, 0) ^ crc(00 00, s), and x y -> crc(x y, 0) is a
# bijection, so for each a the pair (b, c) is looked up.
_INV = {}


def _inverse():
    if not _INV:
        for x in range(256):
            for y in range(256):
                _INV[binascii.crc_hqx(bytes((x, y)), 0)] = (x, y)
    return _INV


def fix_crc(sector):
    """sector: bytearray(512) whose last 3 bytes are free. Sets them; returns the CRC."""
    inv = _inverse()
    c0 = binascii.crc_hqx(bytes(sector[:509]), 0)
    for a in range(MAXB + 1):
        c1 = binascii.crc_hqx(bytes((a,)), c0)
        z = binascii.crc_hqx(b"\0\0", c1)
        for t in CRC_TARGETS:
            b, c = inv[t ^ z]
            if b <= MAXB and c <= MAXB:
                sector[509:512] = bytes((a, b, c))
                crc = binascii.crc_hqx(bytes(sector), 0)
                assert crc == t
                return crc
    raise RuntimeError("no CRC fix for this sector")


# --- colours ----------------------------------------------------------------------------------
def palette():
    """The console's 256 colours, index = the byte (BBGGGRRR), as (r, g, b). 0xFE and 0xFF are
    black here, so a palette mapper never picks them for a colour of their own."""
    out = []
    for i in range(256):
        r, g, b = i & 7, (i >> 3) & 7, i >> 6
        out.append((r * 255 // 7, g * 255 // 7, b * 255 // 3))
    out[0xFE] = out[0xFF] = (0, 0, 0)
    return out


def nearest(rgb, _cache={}):
    if rgb in _cache:
        return _cache[rgb]
    pal = palette()
    best = min(range(0xFE), key=lambda i: sum((a - b) ** 2 for a, b in zip(pal[i], rgb)))
    _cache[rgb] = best
    return best


# --- temporal dithering (--temporal) ----------------------------------------------------------
# The palette is each channel's levels combined (red and green 8, blue 4), so each channel is
# quantised on its own, but for 0xFE and 0xFF (blue 3, green 7, red 6-7): those pixels take
# the nearest allowed colour among the levels around them.
L8 = tuple(i * 255 // 7 for i in range(8))
L4 = tuple(i * 255 // 3 for i in range(4))
Q8 = tuple(min(range(8), key=lambda i: abs(L8[i] - v)) for v in range(256))
Q4 = tuple(min(range(4), key=lambda i: abs(L4[i] - v)) for v in range(256))


def _near_white(vr, vg, vb, qr, qg, qb):
    best = None
    for r in (qr, qr - 1):
        for g in (qg, qg - 1):
            for b in (qb, qb - 1):
                if r < 0 or g < 0 or b < 0 or (b << 6 | g << 3 | r) >= 0xFE:
                    continue
                dr, dg, db = vr - L8[r], vg - L8[g], vb - L4[b]
                d = dr * dr + dg * dg + db * db         # (as uzmdither.c, to the bit)
                if best is None or d < best[0]:
                    best = (d, r, g, b)
    return best[1:]


def dither_picture(tr, tg, tb, rev):
    """Sierra 2-4A error diffusion (2/4 to the next pixel, 1/4 below and 1/4 below-behind) of
    one picture: tr, tg, tb the targets (W x ROWS values, 0-255). rev: each row right to left
    (the showings alternate, so their patterns differ). Returns the bytes and the levels shown
    (three lists)."""
    px = bytearray(W * ROWS)
    lr, lg, lb = [0] * (W * ROWS), [0] * (W * ROWS), [0] * (W * ROWS)
    nr, ng, nb = [0.0] * (W + 2), [0.0] * (W + 2), [0.0] * (W + 2)
    xs, dx = (range(W - 1, -1, -1), -1) if rev else (range(W), 1)
    for y in range(ROWS):
        cr, cg, cb = nr, ng, nb
        nr, ng, nb = [0.0] * (W + 2), [0.0] * (W + 2), [0.0] * (W + 2)
        ar = ag = ab = 0.0
        row = y * W
        for x in xs:
            i = row + x
            e = x + 1
            ur = tr[i] + cr[e] + ar
            ug = tg[i] + cg[e] + ag
            ub = tb[i] + cb[e] + ab
            vr = 0.0 if ur < 0 else 255.0 if ur > 255 else ur
            vg = 0.0 if ug < 0 else 255.0 if ug > 255 else ug
            vb = 0.0 if ub < 0 else 255.0 if ub > 255 else ub
            qr, qg, qb = Q8[int(vr + 0.5)], Q8[int(vg + 0.5)], Q4[int(vb + 0.5)]
            if qb == 3 and qg == 7 and qr >= 6:
                # unclamped: a showing's target above 255 (what the ones before missed of a
                # white) picks the near-white with that channel full
                qr, qg, qb = _near_white(ur, ug, ub, qr, qg, qb)
            px[i] = qb << 6 | qg << 3 | qr
            sr, sg, sb = L8[qr], L8[qg], L4[qb]
            lr[i], lg[i], lb[i] = sr, sg, sb
            er, eg, eb = (vr - sr) * 0.25, (vg - sg) * 0.25, (vb - sb) * 0.25
            ar, ag, ab = er + er, eg + eg, eb + eb
            nr[e] += er
            ng[e] += eg
            nb[e] += eb
            nr[e - dx] += er
            ng[e - dx] += eg
            nb[e - dx] += eb
    return bytes(px), lr, lg, lb


def temporal_job(job):
    """One source picture's showings: (q0, rgb, sounds) -> the packed pictures q0, q0+1, ...
    Showing j aims at (j + 1) x the source minus the levels already shown, so the showings'
    average is the source within what the last one misses (a 1/k of one picture's error)."""
    q0, rgb, sounds = job
    return b"".join(picture(q0 + j, px, [sounds[j]])
                    for j, px in enumerate(temporal_python(rgb, len(sounds))))


# --- packing ----------------------------------------------------------------------------------
def header(pictures, title="", artist="", version=2, n=1, m=1):
    h = bytearray(512)
    h[0:4] = b"UZM1"
    struct.pack_into("<HHHHIHH", h, 4, version, W, ROWS, SECTORS, pictures, SAMPLES, 1)
    if version >= 2:
        h[20], h[21] = n, m
    h[32:96] = title.encode("latin-1", "replace")[:63].ljust(64, b"\0")
    h[96:160] = artist.encode("latin-1", "replace")[:63].ljust(64, b"\0")
    return bytes(h)


def mark(n, s):
    return bytes((n & 0x7F, (n >> 7) & 0x7F, (n >> 14) & 0x7F, s))


def picture_v1(n, pixels, samples):
    """Version 1. n: the picture's number; pixels: 160 x 114 bytes (<= 0xFD); samples: 262
    bytes. Returns the 38 sectors."""
    assert len(pixels) == W * ROWS and len(samples) == SAMPLES
    out = bytearray()
    for s in range(SECTORS):
        sec = bytearray(512)
        sec[0:480] = pixels[s * 480:(s + 1) * 480]
        first = 7 * s if s < SECTORS - 1 else SAMPLES - 7
        sec[480] = 0 if s < SECTORS - 1 else 4
        sec[481:488] = samples[first:first + 7]
        sec[488:492] = mark(n, s)
        if max(sec[:509]) > MAXB:
            raise ValueError("picture %d sector %d holds 0xFE/0xFF" % (n, s))
        fix_crc(sec)
        out += sec
    return bytes(out)


def picture(n, pixels, sets):
    """Version 2. n: the picture's number; pixels: 160 x 114 bytes (<= 0xFD); sets: the sound
    of each showing, 1-3 blocks of 262 bytes. Returns the 38 sectors."""
    assert len(pixels) == W * ROWS and 1 <= len(sets) <= SETS
    assert all(len(x) == SAMPLES for x in sets)
    out = bytearray()
    for s in range(SECTORS):
        sec = bytearray(512)
        sec[0:480] = pixels[s * 480:(s + 1) * 480]
        slot = sample_slot(s)
        if slot is not None:
            for k, smp in enumerate(sets):
                o = T_SAMPLES + SET * k
                sec[o:o + SET] = smp[slot:slot + SET]
        sec[T_MARK:T_MARK + 4] = mark(n, s)
        if max(sec[:509]) > MAXB:
            raise ValueError("picture %d sector %d holds 0xFE/0xFF" % (n, s))
        fix_crc(sec)
        out += sec
    return bytes(out)


def clamp_samples(b):
    return bytes(min(x, MAXB) for x in b)


class Writer:
    """Writes a file's pictures in turn, each with its showings' sound taken from the frames'
    stream (262 samples a frame): version 2 by the cadence N/M, version 1 a frame a picture."""

    def __init__(self, f, audio, version, n, m):
        self.f, self.audio, self.version, self.n, self.m = f, audio, version, n, m
        self.count = 0

    def frame_sound(self, fr):
        x = self.audio[fr * SAMPLES:(fr + 1) * SAMPLES]
        return x + bytes([0x80]) * (SAMPLES - len(x))

    def add(self, px):
        p = self.count
        if self.version == 1:
            self.f.write(picture_v1(p, px, self.frame_sound(p)))
        else:
            f0 = start_frame(p, self.n, self.m)
            self.f.write(picture(p, px, [self.frame_sound(f0 + k)
                                         for k in range(showings(p, self.n, self.m))]))
        self.count += 1


def frames_for(pictures, version, n, m):
    return pictures if version == 1 else start_frame(pictures, n, m)


# --- reading ----------------------------------------------------------------------------------
class Uzm:
    """A .UZM file's header and pictures, either version."""

    def __init__(self, path):
        self.d = open(path, "rb").read()
        d = self.d
        (self.magic, self.version, self.w, self.rows, self.spp, self.pictures, self.samples,
         self.first) = struct.unpack_from("<4sHHHHIHH", d, 0)
        if self.magic != b"UZM1" or self.version not in (1, 2):
            sys.exit("not a .UZM file of version 1 or 2")
        self.n, self.m = (d[20], d[21]) if self.version >= 2 else (1, 1)
        self.title = d[32:96].split(b"\0")[0].decode("latin-1")
        self.artist = d[96:160].split(b"\0")[0].decode("latin-1")
        self.frames = frames_for(self.pictures, self.version, self.n, self.m)

    def sector(self, p, s):
        o = 512 * (self.first + p * self.spp + s)
        return self.d[o:o + 512]

    def pixels(self, p):
        return b"".join(self.sector(p, s)[:480] for s in range(self.spp))

    def frame_sound(self, fr):
        """Frame fr's 262 samples, as the player writes them into its bank, trailer by trailer."""
        bank = bytearray(SAMPLES)
        if self.version == 1:
            pos = 0
            for s in range(self.spp):
                sec = self.sector(fr, s)
                pos -= sec[480] if s == self.spp - 1 else 0
                bank[pos:pos + 7] = sec[481:488]
                pos += 7
            return bytes(bank)
        p, k = picture_of(fr, self.n, self.m)
        pos = 0
        for s in SAMPLE_SECTORS:
            sec = self.sector(p, s)
            if s == SAMPLE_SECTORS[-1]:
                pos -= LAST_BACK
            o = T_SAMPLES + SET * k
            bank[pos:pos + SET] = sec[o:o + SET]
            pos += SET
        assert pos == SAMPLES
        return bytes(bank)


# --- the test pattern -------------------------------------------------------------------------
DIGITS = ["111101101101111", "010110010010111", "111001111100111", "111001111001111",
          "101101111001001", "111100111001111", "111100111101111", "111001010010010",
          "111101111101111", "111101111001111"]


def test_picture(n):
    px = bytearray(W * ROWS)
    bars = [0x07, 0x38, 0xC0, 0x3F, 0xC7, 0xF8, 0xFD, 0x00]   # R G B, yellow, magenta, cyan, white, black
    for y in range(ROWS):
        for x in range(W):
            if y < 24:
                c = bars[x * 8 // W]
            elif y < 40:
                c = (x * 8 // W)                               # red ramp
            elif y < 56:
                c = (x * 8 // W) << 3                          # green ramp
            elif y < 72:
                c = (x * 4 // W) << 6                          # blue ramp
            elif y < 88:
                v = x * 8 // W
                c = v | (v << 3) | ((v >> 1) << 6)            # grey ramp
            else:
                c = 0x49 if (x // 8 + y // 8) & 1 else 0x00   # checker
            px[y * W + x] = min(c, MAXB)
    # a white box moving a pixel a picture
    bx = n % (W - 12)
    for y in range(92, 104):
        for x in range(bx, bx + 12):
            px[y * W + x] = 0xFD
    # the picture number, 5 digits of 3x5 cells of 3x2 pixels, top left of the checker
    s = "%05d" % (n % 100000)
    for i, ch in enumerate(s):
        g = DIGITS[int(ch)]
        for gy in range(5):
            for gx in range(3):
                if g[gy * 3 + gx] == "1":
                    for yy in range(2):
                        for xx in range(3):
                            px[(90 + gy * 2 + yy) * W + 110 + i * 10 + gx * 3 + xx] = 0x3F
    return bytes(px)


def test_tone(frames):
    """440 Hz, continuous across the frames: a frame's set in the wrong place clicks."""
    import math
    return bytes(0x80 + int(60 * math.sin(2 * math.pi * 440 * t / AUDIO_RATE))
                 for t in range(frames * SAMPLES))


def cmd_test(out, seconds, rate, version):
    n, m = RATES[rate]
    pics = int(round(seconds * FPS * m / n))
    audio = test_tone(frames_for(pics, version, n, m))
    with open(out, "wb") as f:
        f.write(header(pics, "TEST PATTERN %d" % rate, "uzm.py", version, n, m))
        w = Writer(f, audio, version, n, m)
        for i in range(pics):
            w.add(test_picture(i))
    print("%s: version %d, %d pictures at %.2f a second, %.1f s, %d bytes" % (
        out, version, pics, FPS * m / n, frames_for(pics, version, n, m) / FPS,
        os.path.getsize(out)))


# --- ffmpeg -----------------------------------------------------------------------------------
def run(cmd, **kw):
    p = subprocess.run(cmd, capture_output=True, **kw)
    if p.returncode:
        sys.exit("%s failed:\n%s" % (cmd[0], p.stderr.decode(errors="replace")[-2000:]))
    return p.stdout


def run_video(cmd_before_input, src, cmd_after, gpu):
    """ffmpeg on the video, decoding on the GPU (NVDEC, -hwaccel cuda) when gpu: the frames
    come back to system memory for the filters. Falls back to the CPU if ffmpeg refuses."""
    if gpu:
        p = subprocess.run(cmd_before_input + ["-hwaccel", "cuda", "-i", src] + cmd_after,
                           capture_output=True)
        if not p.returncode:
            return p.stdout
        print("  (GPU decoding refused, on the CPU: %s)" %
              p.stderr.decode(errors="replace").strip().splitlines()[-1:])
    return run(cmd_before_input + ["-i", src] + cmd_after)


def source_rate(src):
    """IN's frame rate (its video stream's r_frame_rate) as a Fraction, or None."""
    from fractions import Fraction
    out = run(["ffprobe", "-v", "error", "-select_streams", "v:0", "-show_entries",
               "stream=r_frame_rate", "-of", "default=nw=1:nk=1", src]).decode().strip()
    try:
        r = Fraction(out)
        return r if r > 0 else None
    except (ValueError, ZeroDivisionError):
        return None


def film_rate(r):
    """The film's own rate inside a source of r frames a second that repeats its frames
    3, 2, 3, 2 (60 or 59.94: 2/5 of it) or 4 in 5 (30 or 29.97: 4/5); a slower source is the
    film itself."""
    from fractions import Fraction
    if r >= 45:
        return r * Fraction(2, 5)
    if r >= 27:
        return r * Fraction(4, 5)
    return r


def choose_rate(r):
    """The pictures a second for a source of r frames a second: 60 for 45 and up, 30 for 27 to
    45, 25 for 24.5 to 27, 24 below (a rate between two goes to the higher)."""
    if r is None or r >= 45:
        return 60
    if r >= 27:
        return 30
    if r >= 24.5:
        return 25
    return 24


COMPRESS = "dynaudnorm=f=250:g=15:p=0.9"
# mono before anything measures the sound: -ac 1 alone mixes down after the filters, so the
# loudness was set on the source's channels and the mix lost 3 dB of it (stereo) or 12 (5.1)
MONO = "aformat=channel_layouts=mono,"


def loudness_filter(src, cut, target, amap=(), pre=""):
    """ffmpeg's two-pass EBU R128 loudness normalisation, as -af arguments: the first pass
    measures the source (mono), the second applies one gain to reach target LUFS, peaks held
    under -1 dBTP (limited where the gain would push them over). Applied before the 8-bit
    conversion: a quiet source (Dragon's Lair: -32 LUFS, peaks -17 dB) would otherwise use only
    a few of the 256 levels, faint and grainy beside songs mastered near 0 dB. "off": none."""
    if target == "off":
        return ["-af", (MONO + pre).rstrip(",")]
    p = subprocess.run(["ffmpeg", "-hide_banner"] + cut + ["-i", src] + list(amap) + ["-vn", "-ac", "1", "-af",
                        MONO + pre + "loudnorm=I=%s:TP=-1:LRA=11:print_format=json" % target, "-f", "null",
                        "-"], capture_output=True)
    text = p.stderr.decode(errors="replace")
    m = re.findall(r"\{[^{}]*\}", text)
    if p.returncode or not m:
        sys.exit("loudness measurement failed:\n" + text[-1500:])
    j = json.loads(m[-1])
    try:
        level = float(j["input_i"])
    except ValueError:
        level = float("-inf")
    if not level > -70:
        # a clip too short (or too quiet) to measure, as the single pictures of a comparison:
        # loudnorm refuses -inf
        print("  sound: %s LUFS, not measurable: kept as it is" % j["input_i"])
        return ["-af", (MONO + pre).rstrip(",")]
    print("  sound: %s LUFS, peaks %s dBTP -> %s LUFS" % (j["input_i"], j["input_tp"], target))
    return ["-af", MONO + pre + "loudnorm=I=%s:TP=-1:LRA=11:measured_I=%s:measured_TP=%s:measured_LRA=%s:"
            "measured_thresh=%s:offset=%s:linear=true" % (target, j["input_i"], j["input_tp"],
                                                          j["input_lra"], j["input_thresh"],
                                                          j["target_offset"])]


def pick_version(v1, v2, rate, ivtc):
    if v1 and (v2 or ivtc or rate not in (None, "auto", 60, "60")):
        sys.exit("--v1 is 60 pictures a second only, without --ivtc")
    if v1:
        return 1
    if v2 or ivtc or rate not in (None, "auto", 60, "60"):
        return 2
    return DEFAULT_VERSION


def cmd_encode(src, out, opts):
    version = pick_version(opts.get("v1"), opts.get("v2"), opts.get("rate"), opts.get("ivtc"))
    r = source_rate(src)
    film = None
    if opts.get("ivtc"):
        if r is None:
            sys.exit("--ivtc: IN's frame rate is unknown")
        film = film_rate(r)
    rate = opts.get("rate", "auto")
    if rate == "auto":
        rate = choose_rate(film if film else r)
    else:
        rate = int(rate)
    if rate not in RATES:
        sys.exit("--rate: 60, 30, 25 or 24")
    if version == 1 and rate != 60:
        print("  (version 1 is 60 pictures a second only: %d asks for --v2)" % rate)
        rate = 60
    n, m = RATES[rate]
    print("  source %s frames a second%s -> %d pictures a second (%.2f: %d frames for %d)%s" % (
        "%.3f" % r if r else "?", ", film inside at %.3f" % film if film else "", rate,
        FPS * m / n, n, m, ", version 1" if version == 1 else ""))
    tmp = tempfile.mkdtemp(prefix="uzm-")
    pal_rgb = os.path.join(tmp, "pal.rgb")
    pal_png = os.path.join(tmp, "pal.png")
    with open(pal_rgb, "wb") as f:
        f.write(bytes(c for rgb in palette() for c in rgb))
    run(["ffmpeg", "-v", "error", "-y", "-f", "rawvideo", "-pix_fmt", "rgb24", "-s", "16x16",
         "-i", pal_rgb, pal_png])
    cut = []
    if opts.get("start"):
        cut += ["-ss", opts["start"]]
    if opts.get("duration"):
        cut += ["-t", opts["duration"]]
    # the pictures' rate, exactly the console's: 60.05 x M / N
    fps = "fps=%d/%d:round=near" % (F_CPU * m, LINE * LINES * n)
    if film:
        # inverse telecine: the film's frames back, each once, on the film's own grid, then
        # the pictures' rate. A 60 fps source repeats them 3, 2, 3, 2: every other frame of it
        # is 30 fps with one repeat in 5, as a 30 fps source has; decimate drops exactly one
        # frame in each 5, the most like its neighbour. No threshold decides what a new frame
        # is, so a small change (a mouth) is never taken for a repeat (mpdecimate did that),
        # and a drawing the film holds keeps its length: 4 of every 5 stay, numbered on the
        # film's grid, and the sound keeps in step. Resampled by time instead, the 1/120 s
        # offsets of the 3, 2 copies against the slow drift between the two rates lose a
        # frame and repeat one, every other picture for seconds at a time. The time base is
        # made fine first (1/240000 s holds 1/24 and 1001/24000 exactly): in decimate's own,
        # the grid's times were rounded, with the same effect
        f = "(%d/%d)" % (film.numerator, film.denominator)
        chain = ([r"select='not(mod(n\,2))'"] if film * 2 < r else []) + \
            ["decimate", "settb=1/240000", "setpts=N/%s/TB" % f, fps]
    else:
        chain = [fps]
    if opts.get("crop"):
        chain.append("crop=%s" % opts["crop"])
    if opts.get("pre"):
        chain.append(opts["pre"])
    if opts.get("black"):
        # levels at or below black/255 become 0: a source's near-black background stays black
        # instead of dithering into the console's darkest colours (dark red, dark green)
        b = int(opts["black"]) / 255
        chain.append("colorlevels=rimin=%.3f:gimin=%.3f:bimin=%.3f" % (b, b, b))
    scaler = opts.get("scaler", "lanczos")
    if opts.get("letterbox"):
        chain.append("scale=%d:%d:force_original_aspect_ratio=decrease:flags=%s,"
                     "pad=%d:%d:(ow-iw)/2:(oh-ih)/2" % (W, ROWS, scaler, W, ROWS))
    else:
        chain.append("scale=%d:%d:flags=%s" % (W, ROWS, scaler))
    if opts.get("post"):
        chain.append(opts["post"])
    if opts.get("cut"):
        # pixels whose brightest channel is at most cut become black: a near-black background
        # (a flat dark grey) goes, coloured parts stay as they are, however dark. Last before
        # the dither, at 160x114: per pixel (geq) it would be slow at the source's size, and
        # it also takes the near-blacks the sharpening makes
        t = int(opts["cut"])
        mx = "max(max(r(X,Y),g(X,Y)),b(X,Y))"
        chain.append("format=gbrp,geq=r='if(lte(%s,%d),0,r(X,Y))':g='if(lte(%s,%d),0,g(X,Y))'"
                     ":b='if(lte(%s,%d),0,b(X,Y))'" % (mx, t, mx, t, mx, t))
    d = opts.get("dither", "ours")
    if opts.get("temporal") or d == "ours":
        if opts.get("temporal") and d != "ours":
            print("  (--temporal: our dither, not %s)" % d)
        if encode_ours(src, out, opts, cut, chain, n, m, version) is not False:
            return
        print("  (no uzmdither: ffmpeg's sierra2_4a, which reddens near-black areas)")
        d = "sierra2_4a"
    if d.startswith("bayer"):
        dither = "dither=bayer:bayer_scale=%s" % (d[5:] or "2")
    elif d == "fs":
        dither = "dither=floyd_steinberg"
    else:
        dither = "dither=" + d          # sierra2, sierra2_4a, sierra3, burkes, atkinson, none
    graph = "[0:v]%s[v];[v][1:v]paletteuse=%s:diff_mode=none" % (",".join(chain), dither)
    video = run_video(["ffmpeg", "-v", "error"] + cut, src,
                      ["-i", pal_png, "-lavfi", graph, "-an", "-f", "rawvideo", "-pix_fmt", "pal8",
                       "-"], opts.get("gpu"))
    amap = ["-map", "0:a:%d" % int(opts["audio"])] if opts.get("audio") else []
    audio = run(["ffmpeg", "-v", "error"] + cut + ["-i", src] + amap + ["-vn", "-ac", "1"] +
                loudness_filter(src, cut, opts.get("loudness", "-15"), amap,
                                COMPRESS + "," if opts.get("compress") else "") +
                ["-ar", str(AUDIO_RATE), "-f", "u8", "-acodec", "pcm_u8", "-"])
    frame = W * ROWS
    # ffmpeg's rawvideo pal8 output is each frame's indices, then the palette they index (256
    # entries of 4 bytes, B G R A): the indices are ffmpeg's order of the palette image, not
    # ours, so each frame's are mapped back to the console's bytes through that palette
    step = frame + 1024
    if len(video) % step:
        sys.exit("ffmpeg's pal8 output: %d bytes, not frames of %d + 1024" % (len(video), frame))
    pics = len(video) // step
    ours = {}
    for i, rgb in enumerate(palette()[:0xFE]):
        ours.setdefault(rgb, i)
    audio = clamp_samples(audio)
    frames = frames_for(pics, version, n, m)
    if len(audio) < frames * SAMPLES - AUDIO_RATE // 10:
        print("  (the sound is %.2f s shorter than the pictures: silence after it)" %
              ((frames * SAMPLES - len(audio)) / AUDIO_RATE))
    with open(out, "wb") as f:
        f.write(header(pics, opts.get("title", os.path.basename(src)), opts.get("artist", ""),
                       version, n, m))
        w = Writer(f, audio, version, n, m)
        lut, lut_pal = None, None
        for i in range(pics):
            pal = video[i * step + frame:(i + 1) * step]
            if pal != lut_pal:
                lut_pal = pal
                lut = bytes(ours.get((pal[4 * k + 2], pal[4 * k + 1], pal[4 * k]),
                                     nearest((pal[4 * k + 2], pal[4 * k + 1], pal[4 * k])))
                            for k in range(256))
            w.add(video[i * step:i * step + frame].translate(lut))
            if i % 600 == 0:
                print("  %d / %d" % (i, pics), flush=True)
    print("%s: %d pictures (%.2f a second), %.1f s, %.1f MB" % (
        out, pics, FPS * m / n, frames / FPS, os.path.getsize(out) / 1e6))


def temporal_python(rgb, k):
    """The showings' pixels in Python (what uzmdither.c does): k pictures of W x ROWS bytes."""
    t = (rgb[0::3], rgb[1::3], rgb[2::3])
    shown = [[0] * (W * ROWS) for _ in range(3)]
    out = []
    for j in range(k):
        # (above 255 only for the near-whites' choice: the dither clamps)
        tgt = [[min(383, max(0, (j + 1) * v - s)) for v, s in zip(t[c], shown[c])] for c in range(3)]
        px, *lv = dither_picture(*tgt, j % 2)
        shown = [[s + v for s, v in zip(shown[c], lv[c])] for c in range(3)]
        out.append(px)
    return out


def dither_helper():
    """uzmdither (uzmdither.c beside this file), built with gcc -O2 when missing or older than
    its source; None when it can't be (the encoder then dithers in Python, 100 times slower)."""
    here = os.path.dirname(os.path.abspath(__file__))
    c = os.path.join(here, "uzmdither.c")
    exe = os.path.join(here, "uzmdither.exe" if os.name == "nt" else "uzmdither")
    if os.path.exists(exe) and os.path.getmtime(exe) >= os.path.getmtime(c):
        return exe
    try:
        p = subprocess.run(["gcc", "-O2", "-Wall", "-o", exe, c], capture_output=True, text=True)
    except OSError:
        print("  (no gcc: the dither in Python, slow)")
        return None
    if p.returncode or not os.path.exists(exe):
        print("  (uzmdither.c did not build: the dither in Python, slow)\n" + p.stderr[-1000:])
        return None
    return exe


class Ditherer:
    """The temporal dither through uzmdither: a job (k, the source picture) in, k pictures out."""

    def __init__(self, exe):
        self.p = subprocess.Popen([exe], stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    def showings(self, rgb, k):
        self.p.stdin.write(bytes((k,)) + rgb)
        self.p.stdin.flush()
        data = self.p.stdout.read(k * W * ROWS)
        if len(data) != k * W * ROWS:
            sys.exit("uzmdither failed")
        return [data[j * W * ROWS:(j + 1) * W * ROWS] for j in range(k)]

    def close(self):
        self.p.stdin.close()
        self.p.wait()


def encode_ours(src, out, opts, cut, chain, n, m, version):
    """Our dither (the default, and --temporal): the source's pictures (at 60.05 x m / n) as
    RGB from ffmpeg, dithered by uzmdither. Plain: a picture each, at the cadence n/m.
    --temporal: each picture expanded into its showings, written at 60 (cadence 1/1); without
    uzmdither, temporal_job in a pool of processes. Returns False (nothing done) when plain and
    uzmdither can't be built: the caller uses ffmpeg's dither."""
    exe = dither_helper()
    temporal = opts.get("temporal")
    if not exe and not temporal:
        return False
    amap = ["-map", "0:a:%d" % int(opts["audio"])] if opts.get("audio") else []
    audio = clamp_samples(run(["ffmpeg", "-v", "error"] + cut + ["-i", src] + amap +
                              ["-vn", "-ac", "1"] +
                              loudness_filter(src, cut, opts.get("loudness", "-15"), amap,
                                              COMPRESS + "," if opts.get("compress") else "") +
                              ["-ar", str(AUDIO_RATE), "-f", "u8", "-acodec", "pcm_u8", "-"]))
    cmd = (["ffmpeg", "-v", "error"] + cut + (["-hwaccel", "cuda"] if opts.get("gpu") else []) +
           ["-i", src, "-vf", ",".join(chain + ["format=rgb24"]), "-an", "-f", "rawvideo",
            "-pix_fmt", "rgb24", "-"])
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    size = W * ROWS * 3

    def sound(fr):
        x = audio[fr * SAMPLES:(fr + 1) * SAMPLES]
        return x + bytes([0x80]) * (SAMPLES - len(x))

    pics = 0
    title, artist = opts.get("title", os.path.basename(src)), opts.get("artist", "")
    if not temporal:
        # one picture each, shown by the cadence (Writer: its showings' sound)
        d = Ditherer(exe)
        with open(out, "wb") as f:
            f.write(header(0))
            w = Writer(f, audio, version, n, m)
            while True:
                rgb = proc.stdout.read(size)
                if len(rgb) < size:
                    break
                w.add(d.showings(rgb, 1)[0])
                pics += 1
                if pics % 600 == 0:
                    print("  %d pictures" % pics, flush=True)
            f.seek(0)
            f.write(header(pics, title, artist, version, n, m))
        d.close()
        if proc.wait():
            sys.exit("ffmpeg failed")
        frames = frames_for(pics, version, n, m)
        if len(audio) < frames * SAMPLES - AUDIO_RATE // 10:
            print("  (the sound is %.2f s shorter than the pictures: silence after it)" %
                  ((frames * SAMPLES - len(audio)) / AUDIO_RATE))
        print("%s: %d pictures (%.2f a second), %.1f s, %.1f MB" % (
            out, pics, FPS * m / n, frames / FPS, os.path.getsize(out) / 1e6))
        return True
    if exe:
        d = Ditherer(exe)
        with open(out, "wb") as f:
            f.write(header(0))
            while True:
                rgb = proc.stdout.read(size)
                if len(rgb) < size:
                    break
                f0, k = start_frame(pics, n, m), showings(pics, n, m)
                for j, px in enumerate(d.showings(rgb, k)):
                    f.write(picture(f0 + j, px, [sound(f0 + j)]))
                pics += 1
                if pics % 600 == 0:
                    print("  %d pictures" % pics, flush=True)
            frames = start_frame(pics, n, m)
            f.seek(0)
            f.write(header(frames, title, artist, 2, 1, 1))
        d.close()
    else:
        pics, frames = encode_temporal_pool(proc, out, opts, src, n, m, size, sound)
    if proc.wait():
        sys.exit("ffmpeg failed")
    if len(audio) < frames * SAMPLES - AUDIO_RATE // 10:
        print("  (the sound is %.2f s shorter than the pictures: silence after it)" %
              ((frames * SAMPLES - len(audio)) / AUDIO_RATE))
    print("%s: %d source pictures (%.2f a second), temporal: %d pictures at %.2f, %.1f s, %.1f MB" % (
        out, pics, FPS * m / n, frames, FPS, frames / FPS, os.path.getsize(out) / 1e6))
    return True


def encode_temporal_pool(proc, out, opts, src, n, m, size, sound):
    """Without uzmdither: temporal_job in a pool of processes. Returns the frames written."""
    import multiprocessing
    pics = 0
    with open(out, "wb") as f, multiprocessing.Pool() as pool:
        f.write(header(0))
        done = False
        while not done:
            jobs = []
            while len(jobs) < 240:
                rgb = proc.stdout.read(size)
                if len(rgb) < size:
                    done = True
                    break
                f0, k = start_frame(pics, n, m), showings(pics, n, m)
                jobs.append((f0, rgb, [sound(f0 + j) for j in range(k)]))
                pics += 1
            for blob in pool.imap(temporal_job, jobs, chunksize=4):
                f.write(blob)
            print("  %d pictures" % pics, flush=True)
        frames = start_frame(pics, n, m)
        f.seek(0)
        f.write(header(frames, opts.get("title", os.path.basename(src)), opts.get("artist", ""),
                       2, 1, 1))
    return pics, frames


def cmd_info(path):
    u = Uzm(path)
    print("%s v%d %dx%d, %d sectors a picture, %d pictures, %d samples a frame, first sector %d"
          % (u.magic.decode(), u.version, u.w, u.rows, u.spp, u.pictures, u.samples, u.first))
    if u.version >= 2:
        most = max(showings(p, u.n, u.m) for p in range(min(u.pictures, u.n + 1)))
        print("cadence %d frames for %d pictures: %.2f pictures a second, each shown %s" % (
            u.n, u.m, FPS * u.m / u.n, "once" if most == 1 else "up to %d times" % most))
    print("%d frames, %.1f s" % (u.frames, u.frames / FPS))
    print("title %r, artist %r" % (u.title, u.artist))
    bad = 0
    mo = 488 if u.version == 1 else T_MARK
    for p in range(u.pictures):
        for s in range(u.spp):
            sec = u.sector(p, s)
            if (len(sec) < 512 or max(sec) > MAXB or binascii.crc_hqx(sec, 0) not in CRC_TARGETS
                    or sec[mo:mo + 4] != mark(p, s)):
                bad += 1
    print("sectors checked: %d, bad: %d" % (u.pictures * u.spp, bad))
    return bad


def cmd_frame(path, n, out, is_frame):
    """Picture n (or the picture frame n shows) of a .UZM as a PNG, 4x wide and 4x tall (a
    pixel is 9 cycles by 2 lines)."""
    u = Uzm(path)
    if is_frame:
        p, k = picture_of(n, u.n, u.m) if u.version >= 2 else (n, 0)
        print("frame %d: picture %d, showing %d" % (n, p, k + 1))
        n = p
    pal = palette()
    rgb = bytes(c for v in u.pixels(n) for c in pal[v])
    tmp = os.path.join(tempfile.mkdtemp(prefix="uzm-"), "f.rgb")
    open(tmp, "wb").write(rgb)
    run(["ffmpeg", "-v", "error", "-y", "-f", "rawvideo", "-pix_fmt", "rgb24", "-s", "%dx%d" % (W, ROWS),
         "-i", tmp, "-vf", "scale=%d:%d:flags=neighbor" % (W * 4, ROWS * 4), out])
    print(out)


def cmd_sound(path, out):
    """The sound as the player plays it, a frame at a time, as an 8-bit WAV."""
    u = Uzm(path)
    w = wave.open(out, "wb")
    w.setnchannels(1)
    w.setsampwidth(1)
    w.setframerate(AUDIO_RATE)
    for fr in range(u.frames):
        w.writeframes(u.frame_sound(fr))
    w.close()
    print("%s: %d frames, %.1f s" % (out, u.frames, u.frames / FPS))


def cmd_cadence(src, start=None, duration=None):
    """Where a source's new pictures fall: at 40x28 in grey, a frame is new when it differs
    from the one before by more than noise (1 level a pixel on average). Film carried at 60
    (or 59.94) fps by repeating its frames 3, 2 has its new frames at 2 fixed places in each
    group of 5; at 30 (29.97), at 4. A source whose new frames fall anywhere has no film rate
    to recover: keep its own (Dragon's Lair's capture: 11% of its drawings last one frame)."""
    import collections
    r = source_rate(src)
    cut = (["-ss", str(start)] if start else []) + (["-t", str(duration)] if duration else [])
    raw = run(["ffmpeg", "-v", "error"] + cut + ["-i", src, "-map", "0:v:0", "-vf",
               "scale=40:28:flags=area,format=gray", "-f", "rawvideo", "-"])
    fs = 40 * 28
    frames = [raw[i:i + fs] for i in range(0, len(raw) - fs + 1, fs)]
    if len(frames) < 50:
        sys.exit("too few frames (%d)" % len(frames))
    new = [i for i in range(1, len(frames))
           if sum(abs(a - b) for a, b in zip(frames[i], frames[i - 1])) / fs > 1.0]
    print("%s: %d frames at %s a second; %d new pictures (%.1f a second)" % (
        os.path.basename(src), len(frames), "%.3f" % r if r else "?", len(new),
        len(new) / (len(frames) / float(r)) if r else 0))
    if not new:
        return
    places = collections.Counter(i % 5 for i in new)
    order = sorted(places.values(), reverse=True)
    print("  where new pictures fall in each group of 5 frames: %s" % dict(sorted(places.items())))
    lasts = collections.Counter(min(b - a, 10) for a, b in zip(new, new[1:]))
    print("  frames each picture lasts (10: 10 or more): %s" % dict(sorted(lasts.items())))
    if r and r >= 45:
        film = sum(order[:2]) / float(len(new))
        print("  in the 2 likeliest places: %.0f%%" % (100 * film))
        if film > 0.9:
            print("  -> film repeated 3, 2: encode with --ivtc (%.3f a second)" % film_rate(r))
        else:
            print("  -> no fixed pattern: keep %d (a picture lasting 1 or 2 frames could be "
                  "missed at a lower rate: %d of %d)" % (choose_rate(r), lasts[1] + lasts[2],
                                                      len(new) - 1))
    elif r and r >= 27:
        film = sum(order[:4]) / float(len(new))
        print("  in the 4 likeliest places: %.0f%%" % (100 * film))
        if film > 0.97 and len(places) == 5 and min(places.values()) < 0.03 * len(new):
            print("  -> film with 1 frame in 5 repeated: encode with --ivtc (%.3f a second)" %
                  film_rate(r))
        else:
            print("  -> its own rate: 30")
    else:
        print("  -> its own rate: %d" % choose_rate(r))


def main():
    a = sys.argv[1:]
    if len(a) >= 2 and a[0] == "test":
        rest, rate, v1, v2, secs = a[2:], 60, False, False, 10.0
        while rest:
            k = rest.pop(0)
            if k == "--rate":
                rate = int(rest.pop(0))
            elif k == "--v1":
                v1 = True
            elif k == "--v2":
                v2 = True
            else:
                secs = float(k)
        if rate not in RATES:
            sys.exit("--rate: 60, 30, 25 or 24")
        version = pick_version(v1, v2, rate, False)
        cmd_test(a[1], secs, rate, version)
    elif len(a) >= 3 and a[0] == "encode":
        opts, rest = {}, a[3:]
        while rest:
            k = rest.pop(0)
            if k in ("--letterbox", "--gpu", "--ivtc", "--v1", "--v2", "--compress", "--temporal"):
                opts[k[2:]] = True
            elif k in ("--title", "--artist", "--start", "--duration", "--dither", "--crop",
                       "--pre", "--post", "--black", "--scaler", "--cut", "--loudness", "--rate",
                       "--audio"):
                opts[k[2:]] = rest.pop(0)
            else:
                sys.exit("unknown option %s" % k)
        cmd_encode(a[1], a[2], opts)
    elif len(a) == 2 and a[0] == "info":
        sys.exit(1 if cmd_info(a[1]) else 0)
    elif len(a) in (4, 5) and a[0] == "frame":
        cmd_frame(a[1], int(a[2]), a[3], a[4:] == ["--frame"])
    elif len(a) >= 2 and a[0] == "cadence":
        rest, st, du = a[2:], None, None
        while rest:
            k = rest.pop(0)
            if k == "--start":
                st = rest.pop(0)
            elif k == "--duration":
                du = rest.pop(0)
        cmd_cadence(a[1], st, du)
    elif len(a) == 3 and a[0] == "sound":
        cmd_sound(a[1], a[2])
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
