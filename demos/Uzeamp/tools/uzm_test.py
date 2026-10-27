#!/usr/bin/env python3
"""uzm_test.py -- checks uzm.py's packing (plan-rates.md, M1).

    python -I uzm_test.py

For clips made by ffmpeg (a test picture with a frame counter, and a tone) at 60, 30, 25 and
24 frames a second, encoded at their rates:
  - every sector is valid (uzm.py info: no 0xFE/0xFF, the CRC, the marks);
  - the sound the player would play, unpacked frame by frame from the trailers (Uzm.frame_sound),
    is ffmpeg's sound, byte for byte, frame after frame;
  - each picture's showings are what the cadence says, and the frames' count and duration.
Also the test patterns at each rate (their tone is known exactly).
"""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import uzm  # noqa: E402

UZM = os.path.join(HERE, "uzm.py")
SECS = 3


def sh(cmd):
    p = subprocess.run(cmd, capture_output=True, text=True)
    if p.returncode:
        sys.exit("failed: %s\n%s%s" % (" ".join(cmd), p.stdout, p.stderr))
    return p.stdout


def clip(tmp, fps):
    """A clip at fps: ffmpeg's testsrc (its counter changes every frame) and a 300 Hz tone."""
    out = os.path.join(tmp, "clip%s.mp4" % fps.replace("/", "_"))
    sh(["ffmpeg", "-v", "error", "-y", "-f", "lavfi", "-i", "testsrc=size=320x240:rate=%s" % fps,
        "-f", "lavfi", "-i", "sine=frequency=300:sample_rate=48000", "-t", str(SECS),
        "-pix_fmt", "yuv420p", "-c:v", "libx264", "-c:a", "aac", "-shortest", out])
    return out


def check_sound(path, expect):
    """Every frame's unpacked sound against the stream it came from (padded with silence)."""
    u = uzm.Uzm(path)
    bad = 0
    for fr in range(u.frames):
        want = expect[fr * uzm.SAMPLES:(fr + 1) * uzm.SAMPLES]
        want += bytes([0x80]) * (uzm.SAMPLES - len(want))
        if u.frame_sound(fr) != want:
            bad += 1
    return u, bad


def main():
    tmp = tempfile.mkdtemp(prefix="uzmtest-")
    fails = 0

    def ok(cond, what):
        nonlocal fails
        print("  %s %s" % ("ok  " if cond else "FAIL", what))
        fails += not cond

    # the cadences themselves
    for rate, (n, m) in sorted(uzm.RATES.items()):
        shows = [uzm.showings(p, n, m) for p in range(2 * m)]
        frames = [uzm.picture_of(f, n, m) for f in range(uzm.start_frame(m, n, m))]
        ok(sum(shows[:m]) == n and max(shows) <= uzm.SETS and
           all(uzm.start_frame(p, n, m) + k == f for f, (p, k) in enumerate(frames)),
           "cadence %d: showings %s" % (rate, shows[:m]))
    slots = {s: uzm.sample_slot(s) for s in range(uzm.SECTORS)}
    cover = bytearray(uzm.SAMPLES)
    for s, o in slots.items():
        if o is not None:
            cover[o:o + uzm.SET] = b"\1" * uzm.SET
    ok(all(cover) and slots[0] is None and slots[1] is None and slots[35] is None and
       slots[37] is None and slots[34] == uzm.SAMPLES - uzm.SET,
       "samples: sectors 2-34 cover the 262, none in 0, 1, 35-37")

    # test patterns at each rate: the tone is known exactly
    for rate in sorted(uzm.RATES):
        out = os.path.join(tmp, "test%d.UZM" % rate)
        sh([sys.executable, "-I", UZM, "test", out, "2", "--rate", str(rate)])
        u, bad = check_sound(out, uzm.test_tone(uzm.frames_for(
            uzm.Uzm(out).pictures, 2, *uzm.RATES[rate])))
        info = subprocess.run([sys.executable, "-I", UZM, "info", out], capture_output=True)
        ok(info.returncode == 0 and bad == 0,
           "pattern %d: %d pictures, %d frames, sectors valid, sound of every frame right (%d wrong)"
           % (rate, u.pictures, u.frames, bad))

    # clips at each rate, encoded at the rate auto chooses
    for fps, rate in (("60", 60), ("30", 30), ("25", 25), ("24", 24), ("30000/1001", 30),
                      ("24000/1001", 24)):
        src = clip(tmp, fps)
        out = os.path.join(tmp, "clip%s.UZM" % fps.replace("/", "_"))
        log = sh([sys.executable, "-I", UZM, "encode", src, out, "--loudness", "off"])
        u = uzm.Uzm(out)
        ok((u.n, u.m) == uzm.RATES[rate], "clip %s fps -> rate %d (%s)" % (
            fps, rate, log.splitlines()[0].strip()))
        expect = uzm.clamp_samples(sh_bytes(["ffmpeg", "-v", "error", "-i", src, "-vn", "-ac", "1",
                                             "-ar", str(uzm.AUDIO_RATE), "-f", "u8",
                                             "-acodec", "pcm_u8", "-"]))
        u, bad = check_sound(out, expect)
        info = subprocess.run([sys.executable, "-I", UZM, "info", out], capture_output=True)
        ok(info.returncode == 0 and bad == 0 and abs(u.frames / uzm.FPS - SECS) < 0.2,
           "clip %s: %d pictures, %d frames (%.2f s), sound of every frame right (%d wrong)" % (
               fps, u.pictures, u.frames, u.frames / uzm.FPS, bad))

    # --ivtc: 23.976 film carried at 59.94, each copy with its own noise; drawings on twos,
    # and one held for 2 seconds
    for src_rate, tele in (("24000/1001", "60000/1001"), ("24", "60")):
        bad, rep, skip, n = ivtc_case(tmp, src_rate, tele)
        ok(bad == 0 and skip == 0 and rep <= 1,
           "--ivtc, film %s at %s: %d pictures, %d not the film's drawing of their time, "
           "%d repeated (the rates' drift), %d skipped" % (src_rate, tele, n, bad, rep, skip))

    # --temporal: flat colours between the levels; white (0xFE/0xFF never shown: no mix of the
    # near-whites is closer to it than one of them, only no worse)
    for rgb in ((54, 127, 42), (20, 200, 128), (240, 240, 240), (255, 255, 255)):
        for k in (2, 3):
            one, avg = temporal_case(rgb, k)
            white = rgb == (255, 255, 255)
            ok(avg[0] <= one[0] + 0.5 and avg[1] <= one[1] if white else
               avg[0] <= 2.0 + one[0] and avg[1] < 0.7 * one[1],
               "--temporal, %s in %d showings: the showings' average off by %.1f over the picture "
               "(one picture %.1f), %.1f a pixel (rms; one picture %.1f)" % (
                   rgb, k, avg[0], one[0], avg[1], one[1]))
    # uzmdither.c against the Python dither, byte for byte: random pictures, gradients, flat
    # colours and white, 1-3 showings
    exe = uzm.dither_helper()
    ok(exe is not None, "uzmdither builds (%s)" % exe)
    if exe:
        import random
        rnd = random.Random(7)
        n = uzm.W * uzm.ROWS
        pics = [bytes(rnd.randrange(256) for _ in range(3 * n)),
                bytes(v for i in range(n) for v in (i % 256, (i // 160) * 2 % 256, 255 - i % 256)),
                bytes((54, 127, 42)) * n, bytes((255, 255, 255)) * n, bytes((250, 252, 249)) * n]
        d = uzm.Ditherer(exe)
        same = sum(d.showings(p, k) == uzm.temporal_python(p, k) for p in pics for k in (1, 2, 3))
        d.close()
        ok(same == 3 * len(pics), "uzmdither's pictures are the Python dither's: %d of %d jobs"
           % (same, 3 * len(pics)))
    for fps in ("30", "24000/1001"):
        src = clip(tmp, fps)
        a, b = os.path.join(tmp, "td_plain.UZM"), os.path.join(tmp, "td.UZM")
        sh([sys.executable, "-I", UZM, "encode", src, a, "--loudness", "off"])
        sh([sys.executable, "-I", UZM, "encode", src, b, "--loudness", "off", "--temporal"])
        expect = uzm.clamp_samples(sh_bytes(["ffmpeg", "-v", "error", "-i", src, "-vn", "-ac", "1",
                                             "-ar", str(uzm.AUDIO_RATE), "-f", "u8",
                                             "-acodec", "pcm_u8", "-"]))
        u, bad = check_sound(b, expect)
        info = subprocess.run([sys.executable, "-I", UZM, "info", b], capture_output=True)
        ok(info.returncode == 0 and bad == 0 and (u.n, u.m) == (1, 1) and
           u.frames == uzm.Uzm(a).frames,
           "--temporal clip %s: %d pictures at 60, as many frames as without, sectors valid, "
           "sound of every frame right (%d wrong)" % (fps, u.pictures, bad))

    print("all passed" if not fails else "%d FAILED" % fails)
    sys.exit(1 if fails else 0)


def temporal_case(rgb, k):
    """A flat picture of rgb: (mean error over the picture, rms error a pixel) of one dithered
    picture and of the average of k temporal showings, the error the larger channel's."""
    n = uzm.W * uzm.ROWS
    pal = uzm.palette()
    blob = uzm.temporal_job((0, bytes(rgb) * n, [bytes([0x80]) * uzm.SAMPLES] * k))
    size = uzm.SECTORS * 512
    shows = [b"".join(blob[q * size + s * 512:q * size + s * 512 + 480] for s in range(uzm.SECTORS))
             for q in range(k)]
    first = uzm.dither_picture([rgb[0]] * n, [rgb[1]] * n, [rgb[2]] * n, 0)[0]

    def err(pics):
        worst = (0.0, 0.0)
        for c in range(3):
            vals = [sum(pal[p[i]][c] for p in pics) / len(pics) for i in range(n)]
            mean = abs(sum(vals) / n - rgb[c])
            rms = (sum((v - rgb[c]) ** 2 for v in vals) / n) ** 0.5
            worst = (max(worst[0], mean), max(worst[1], rms))
        return worst
    return err([first]), err(shows)


IVTC_SECS = 20


def drawing(j):
    """The film's frame j shows drawing d(j): one a frame to 60, then on twos to 150, held from
    150 to 198 (2 s), then one a frame again. Each drawing is a flat colour."""
    if j < 60:
        return j
    if j < 150:
        return 60 + (j - 60) // 2
    if j < 198:
        return 105
    return 106 + (j - 198)


def colour(d):
    return (d * 53 % 200 + 20, d * 97 % 200 + 20, d * 31 % 200 + 20)


def ivtc_case(tmp, src_rate, tele):
    """Encodes a film at src_rate, carried at tele with noise, with --ivtc. Returns the pictures
    whose drawing is not the film's at their time (within a frame), the drawings repeated and
    skipped where the film had none, and the pictures."""
    from fractions import Fraction
    name = os.path.join(tmp, "ivtc%s" % src_rate.replace("/", "_"))
    d = "if(lt(N,60),N,if(lt(N,150),60+floor((N-60)/2),if(lt(N,198),105,106+N-198)))"
    geq = "r='mod((%s)*53\\,200)+20':g='mod((%s)*97\\,200)+20':b='mod((%s)*31\\,200)+20'" % (d, d, d)
    sh(["ffmpeg", "-v", "error", "-y", "-f", "lavfi", "-i",
        "color=black:size=320x240:rate=%s,format=rgb24,geq=%s" % (src_rate, geq),
        "-f", "lavfi", "-i", "sine=frequency=300:sample_rate=48000", "-t", str(IVTC_SECS),
        "-pix_fmt", "yuv444p", "-c:v", "libx264", "-crf", "10", "-c:a", "aac", "-shortest",
        name + "_film.mp4"])
    sh(["ffmpeg", "-v", "error", "-y", "-i", name + "_film.mp4", "-vf",
        "fps=%s,noise=alls=6:allf=t" % tele, "-pix_fmt", "yuv444p", "-c:v", "libx264",
        "-crf", "18", "-c:a", "copy", name + "_tele.mp4"])
    sh([sys.executable, "-I", UZM, "encode", name + "_tele.mp4", name + ".UZM", "--ivtc",
        "--loudness", "off"])
    u = uzm.Uzm(name + ".UZM")
    pal = uzm.palette()
    film = Fraction(src_rate)
    pics = Fraction(uzm.F_CPU * u.m, uzm.LINE * uzm.LINES * u.n)
    seen, bad = [], 0
    for p in range(u.pictures):
        px = u.pixels(p)[::3]
        rgb = [sum(pal[v][c] for v in px) / len(px) for c in range(3)]
        j = round(p * film / pics)                   # the film's frame at this picture's time
        near = [drawing(k) for k in range(max(0, j - 2), j + 3)]
        got = min(set(near), key=lambda x: sum((a - b) ** 2 for a, b in zip(rgb, colour(x))))
        seen.append(got)
        if got not in [drawing(k) for k in range(max(0, j - 1), j + 2)]:
            bad += 1
    # repeats, but not the film's own holds (on twos: which of two identical frames goes is
    # either), nor the last pictures (ffmpeg's rate filter pads the end)
    rep = sum(1 for a, b in zip(seen[:-4], seen[1:-4]) if a == b and not 60 <= a <= 105)
    skip = sum(b - a - 1 for a, b in zip(seen, seen[1:]) if b > a + 1)
    return bad, max(rep, 0), skip, u.pictures


def sh_bytes(cmd):
    p = subprocess.run(cmd, capture_output=True)
    if p.returncode:
        sys.exit("failed: %s" % " ".join(cmd))
    return p.stdout


if __name__ == "__main__":
    main()
