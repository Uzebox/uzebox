#!/usr/bin/env python3
"""uzm.py -- makes Uzeamp video files (.UZM). See ../uzmmode/uzm.def.h.

    python -I uzm.py test OUT.UZM [seconds]       a test pattern: colour bars, gradients, a box
                                                  moving a pixel a picture, the picture number,
                                                  and a 440 Hz tone (default 10 s)
    python -I uzm.py encode IN OUT.UZM [options]  a video file, through ffmpeg
        --title T --artist A                      shown by the player
        --start S --duration D                    a part of IN (seconds)
        --dither bayerN|fs|sierra2|sierra2_4a|sierra3|burkes|atkinson|none
                                                  ffmpeg's dither (default sierra2_4a: error
                                                  diffusion keeps dark greys as a fine grain;
                                                  bayer's pattern shows on dark backgrounds,
                                                  atkinson drops them to black)
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
        --loudness N|off                          the sound's loudness in LUFS (default -12,
                                                  near songs mastered at 0 dB; two-pass
                                                  EBU R128); off: the source's level
        --gpu                                     decode on the GPU (NVDEC, -hwaccel cuda;
                                                  falls back to the CPU). Same output; not
                                                  faster here: the packing in Python dominates
        --crop W:H:X:Y                            a part of IN's picture first (ffmpeg's crop)
    python -I uzm.py info FILE.UZM                the header, and a check of every sector
    python -I uzm.py frame FILE.UZM N OUT.png     picture N as an image, 4x

The file
  Sector 0, the header (read with uzefs_read, so any bytes):
     0  "UZM1"           4  u16 version (1)     6  u16 width (160)    8  u16 rows (114)
    10  u16 sectors a picture (38)             12  u32 pictures      16  u16 samples a picture (262)
    18  u16 first picture's sector (1)         32  title (64 bytes)  96  artist (64 bytes)
  Then the pictures, 38 sectors each, at 60.05 a second (the console's: 262 lines of 1820
  cycles). Sector s of a picture holds rows 3s..3s+2 (160 bytes each, a byte a pixel,
  BBGGGRRR) and a 32-byte trailer:
     0  back: 0, or 4 in sector 37 (its samples start 4 before: see below)
     1  7 sound samples (unsigned, 15,734 Hz): sectors 0-36 hold samples 7s..7s+6; sector 37
        holds 255..261, the first 4 again, so every sector writes 7 and the picture's 262 end
        exactly at the sound bank's end
     8  the picture's number in 3 bytes of 7 bits (low first), then s
    12  zeros
    29  3 bytes chosen so that the sector's CRC16 is FEFE, FEFF, FFFE or FFFF
  No byte of a picture's sector is 0xFE or 0xFF, and with that CRC the player keeps every byte
  below 0xFE that the card sends in a multiple-block read: CRCs, waits and tokens all drop out.
"""
import binascii
import json
import os
import re
import struct
import subprocess
import sys
import tempfile

W, ROWS, ROWS_SEC, SECTORS, SAMPLES = 160, 114, 3, 38, 262
F_CPU, LINE, LINES = 28636360, 1820, 262
FPS = F_CPU / (LINE * LINES)                      # 60.0546
AUDIO_RATE = 15734
CRC_TARGETS = (0xFEFE, 0xFEFF, 0xFFFE, 0xFFFF)
MAXB = 0xFD

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


# --- packing ----------------------------------------------------------------------------------
def header(pictures, title="", artist=""):
    h = bytearray(512)
    h[0:4] = b"UZM1"
    struct.pack_into("<HHHHIHH", h, 4, 1, W, ROWS, SECTORS, pictures, SAMPLES, 1)
    h[32:96] = title.encode("latin-1", "replace")[:63].ljust(64, b"\0")
    h[96:160] = artist.encode("latin-1", "replace")[:63].ljust(64, b"\0")
    return bytes(h)


def picture(n, pixels, samples):
    """n: the picture's number; pixels: 160 x 114 bytes (<= 0xFD); samples: 262 bytes.
    Returns the 38 sectors."""
    assert len(pixels) == W * ROWS and len(samples) == SAMPLES
    out = bytearray()
    for s in range(SECTORS):
        sec = bytearray(512)
        sec[0:480] = pixels[s * 480:(s + 1) * 480]
        first = 7 * s if s < SECTORS - 1 else SAMPLES - 7
        sec[480] = 0 if s < SECTORS - 1 else 4
        sec[481:488] = samples[first:first + 7]
        sec[488:492] = bytes((n & 0x7F, (n >> 7) & 0x7F, (n >> 14) & 0x7F, s))
        if max(sec[:509]) > MAXB:
            raise ValueError("picture %d sector %d holds 0xFE/0xFF" % (n, s))
        fix_crc(sec)
        out += sec
    return bytes(out)


def clamp_samples(b):
    return bytes(min(x, MAXB) for x in b)


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


def cmd_test(out, seconds):
    import math
    n = int(round(seconds * FPS))
    with open(out, "wb") as f:
        f.write(header(n, "TEST PATTERN", "uzm.py"))
        t = 0
        for i in range(n):
            smp = bytes(0x80 + int(60 * math.sin(2 * math.pi * 440 * (t + k) / AUDIO_RATE))
                        for k in range(SAMPLES))
            t += SAMPLES
            f.write(picture(i, test_picture(i), smp))
    print("%s: %d pictures, %.1f s, %d bytes" % (out, n, n / FPS, os.path.getsize(out)))


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


def loudness_filter(src, cut, target):
    """ffmpeg's two-pass EBU R128 loudness normalisation, as -af arguments: the first pass
    measures the source (mono), the second applies one gain to reach target LUFS, peaks held
    under -1 dBTP (limited where the gain would push them over). Applied before the 8-bit
    conversion: a quiet source (Dragon's Lair: -32 LUFS, peaks -17 dB) would otherwise use only
    a few of the 256 levels, faint and grainy beside songs mastered near 0 dB. "off": none."""
    if target == "off":
        return []
    p = subprocess.run(["ffmpeg", "-hide_banner"] + cut + ["-i", src, "-vn", "-ac", "1", "-af",
                        "loudnorm=I=%s:TP=-1:LRA=11:print_format=json" % target, "-f", "null",
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
        return []
    print("  sound: %s LUFS, peaks %s dBTP -> %s LUFS" % (j["input_i"], j["input_tp"], target))
    return ["-af", "loudnorm=I=%s:TP=-1:LRA=11:measured_I=%s:measured_TP=%s:measured_LRA=%s:"
            "measured_thresh=%s:offset=%s:linear=true" % (target, j["input_i"], j["input_tp"],
                                                          j["input_lra"], j["input_thresh"],
                                                          j["target_offset"])]


def cmd_encode(src, out, opts):
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
    fps = "%d/%d" % (F_CPU, LINE * LINES)
    chain = ["fps=%s" % fps]
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
        m = "max(max(r(X,Y),g(X,Y)),b(X,Y))"
        chain.append("format=gbrp,geq=r='if(lte(%s,%d),0,r(X,Y))':g='if(lte(%s,%d),0,g(X,Y))'"
                     ":b='if(lte(%s,%d),0,b(X,Y))'" % (m, t, m, t, m, t))
    d = opts.get("dither", "sierra2_4a")
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
    audio = run(["ffmpeg", "-v", "error"] + cut + ["-i", src, "-vn", "-ac", "1"] +
                loudness_filter(src, cut, opts.get("loudness", "-12")) +
                ["-ar", str(AUDIO_RATE), "-f", "u8", "-acodec", "pcm_u8", "-"])
    frame = W * ROWS
    # ffmpeg's rawvideo pal8 output is each frame's indices, then the palette they index (256
    # entries of 4 bytes, B G R A): the indices are ffmpeg's order of the palette image, not
    # ours, so each frame's are mapped back to the console's bytes through that palette
    step = frame + 1024
    if len(video) % step:
        sys.exit("ffmpeg's pal8 output: %d bytes, not frames of %d + 1024" % (len(video), frame))
    n = len(video) // step
    ours = {}
    for i, rgb in enumerate(palette()[:0xFE]):
        ours.setdefault(rgb, i)
    audio = clamp_samples(audio) + bytes([0x80]) * (SAMPLES * n)
    with open(out, "wb") as f:
        f.write(header(n, opts.get("title", os.path.basename(src)), opts.get("artist", "")))
        lut, lut_pal = None, None
        for i in range(n):
            pal = video[i * step + frame:(i + 1) * step]
            if pal != lut_pal:
                lut_pal = pal
                lut = bytes(ours.get((pal[4 * k + 2], pal[4 * k + 1], pal[4 * k]),
                                     nearest((pal[4 * k + 2], pal[4 * k + 1], pal[4 * k])))
                            for k in range(256))
            px = video[i * step:i * step + frame].translate(lut)
            f.write(picture(i, px, audio[i * SAMPLES:(i + 1) * SAMPLES]))
            if i % 600 == 0:
                print("  %d / %d" % (i, n), flush=True)
    print("%s: %d pictures, %.1f s, %.1f MB" % (out, n, n / FPS, os.path.getsize(out) / 1e6))


def cmd_info(path):
    d = open(path, "rb").read()
    magic, ver, w, rows, spp, n, smp, first = struct.unpack_from("<4sHHHHIHH", d, 0)
    title = d[32:96].split(b"\0")[0].decode("latin-1")
    artist = d[96:160].split(b"\0")[0].decode("latin-1")
    print("%s v%d %dx%d, %d sectors a picture, %d pictures (%.1f s), %d samples, first sector %d"
          % (magic.decode(), ver, w, rows, spp, n, n / FPS, smp, first))
    print("title %r, artist %r" % (title, artist))
    bad = 0
    for i in range(n):
        for s in range(spp):
            o = 512 * (first + i * spp + s)
            sec = d[o:o + 512]
            if (len(sec) < 512 or max(sec) > MAXB or binascii.crc_hqx(sec, 0) not in CRC_TARGETS
                    or sec[488:492] != bytes((i & 0x7F, (i >> 7) & 0x7F, (i >> 14) & 0x7F, s))):
                bad += 1
    print("sectors checked: %d, bad: %d" % (n * spp, bad))


def cmd_frame(path, n, out):
    """Picture n of a .UZM as a PNG, 4x wide and 4x tall (a pixel is 9 cycles by 2 lines)."""
    d = open(path, "rb").read()
    spp, first = struct.unpack_from("<H", d, 10)[0], struct.unpack_from("<H", d, 18)[0]
    px = bytearray()
    for s in range(spp):
        o = 512 * (first + n * spp + s)
        px += d[o:o + 480]
    pal = palette()
    rgb = bytes(c for v in px for c in pal[v])
    tmp = os.path.join(tempfile.mkdtemp(prefix="uzm-"), "f.rgb")
    open(tmp, "wb").write(rgb)
    run(["ffmpeg", "-v", "error", "-y", "-f", "rawvideo", "-pix_fmt", "rgb24", "-s", "%dx%d" % (W, ROWS),
         "-i", tmp, "-vf", "scale=%d:%d:flags=neighbor" % (W * 4, ROWS * 4), out])
    print(out)


def main():
    a = sys.argv[1:]
    if len(a) >= 2 and a[0] == "test":
        cmd_test(a[1], float(a[2]) if len(a) > 2 else 10)
    elif len(a) >= 3 and a[0] == "encode":
        opts, rest = {}, a[3:]
        while rest:
            k = rest.pop(0)
            if k == "--letterbox":
                opts["letterbox"] = True
            elif k == "--gpu":
                opts["gpu"] = True
            elif k in ("--title", "--artist", "--start", "--duration", "--dither", "--crop",
                       "--pre", "--post", "--black", "--scaler", "--cut", "--loudness"):
                opts[k[2:]] = rest.pop(0)
            else:
                sys.exit("unknown option %s" % k)
        cmd_encode(a[1], a[2], opts)
    elif len(a) == 2 and a[0] == "info":
        cmd_info(a[1])
    elif len(a) == 4 and a[0] == "frame":
        cmd_frame(a[1], int(a[2]), a[3])
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
