#!/usr/bin/env python3
"""Uzeamp in CUzeBox, driven through the GDB stub: a scripted session, its screenshots, and how
deep the stack went. See ../plan-video.md (each milestone's acceptance).

    python -I uzeamp_check.py run HEX CARD OUT [SCRIPT]   the session; screenshots OUT_<name>.png
    python -I uzeamp_check.py compare A_PREFIX B_PREFIX   the screenshots of two runs, the
                                                          emulator's overlay text cropped off

HEX: the ROM (.hex, its .elf beside it); CARD: a folder whose files make the card (copied into
card/ of a scratch folder, which CUzeBox serves with its host-directory card: --sdhost=card).
UZEAMP_EMU_ARGS: more CUzeBox options, e.g. --no-sdhost (no card in the slot).

SCRIPT: steps separated by ";" (default: the M1 session below):
    f N             run N frames
    p BUTTONS N     press BUTTONS for N frames, then run them. The stub's names: start, select,
                    a, b, x, y, up, down, left, right, l, r (the shoulders: Uzeamp's SL/SR);
                    several joined with + (r+a)
    h BUTTONS N     hold BUTTONS for the next N frames, running none (f steps run them)
    v N [late]      run N frames, checking each movie frame of play (Uzeamp's movieFrameHook):
                    the picture its trailer named and its sound, read from the bank it went to,
                    against the card's .UZM (the first one in CARD; version 2). late: the first
                    repeat frame gets a late row (uzm.under + 1), as a slow card's would
    w SYMBOL N      write N (16 bits) to a variable: w testStill 2100 makes the card test find
                    the full frame's restarts too slow
    s NAME          screenshot
    d NAME          the movie's state: its start frame, the frames since (the card test's
                    black frames included), the last trailer's picture, the rows missing, the
                    sound buffer's lowest and highest sample (v checks every frame)
The stack: at main, RAM from the end of .bss to 16 below SP is filled with 0xA5; at the end the
lowest byte changed is the stack's deepest point.
"""
import binascii
import json
import struct
import os
import shutil
import subprocess
import sys
import tempfile
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import uzm  # noqa: E402

EMULATOR = os.environ.get("UZEBOX_EMULATOR",
                          r"C:\msys64\home\admin\cuzebox-esp8266-gdb\cuzebox.exe")
M1_SCRIPT = ("f 90; s list; p start 2; f 60; s play; f 300; s advanced; p r 40; f 60; s cued; "
             "p down 2; f 20; p start 2; f 90; s other; p down 2; f 20; s cursor")


class Emu:
    def __init__(self, work, hexname, port=12870):
        sys.path.insert(0, os.path.join(os.path.dirname(EMULATOR), "test", "gdb"))
        from rsp_client import RSPClient
        flags = getattr(subprocess, "IDLE_PRIORITY_CLASS", 0)
        extra = os.environ.get("UZEAMP_EMU_ARGS", "").split()    # e.g. --no-sdhost: no card
        self.proc = subprocess.Popen([EMULATOR, hexname, "--esp=0", "--sdhost=card",
                                      "--gdb=%d" % port,
                                      "--gdb-wait", "--gdb-nolimit"] + extra, cwd=work,
                                     stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT,
                                     creationflags=flags)
        for _ in range(100):
            try:
                self.rsp = RSPClient("127.0.0.1", port, timeout=5.0)
                if self.rsp.xchg("qSupported", timeout=1.0):
                    return
            except OSError:
                time.sleep(0.1)
        raise RuntimeError("no GDB stub")

    def close(self):
        try:
            self.rsp.close()
        finally:
            self.proc.kill()
            self.proc.wait(5)

    def read(self, addr, n):
        out = b""
        while n:
            k = min(n, 256)
            out += bytes.fromhex(self.rsp.xchg("m%x,%x" % (addr, k)))
            addr, n = addr + k, n - k
        return out

    def write(self, addr, data):
        for i in range(0, len(data), 128):
            part = data[i:i + 128]
            self.rsp.expect_ok("M%x,%x:%s" % (addr + i, len(part), part.hex()))

    def cont(self, timeout=120):
        self.rsp.send("c")
        if self.rsp.read_packet(timeout) is None:
            raise RuntimeError("no stop within %d s" % timeout)

    def monitor(self, cmd):
        self.rsp.send("qRcmd," + binascii.hexlify((cmd + " --json").encode()).decode())
        out = ""
        while True:
            p = self.rsp.read_packet(15)
            if p and p.startswith("O") and p != "OK":
                out += bytes.fromhex(p[1:]).decode("latin-1")
                continue
            break
        reply = json.loads(out) if out.strip() else {}
        if "error" in reply:
            raise RuntimeError("monitor %s: %s" % (cmd, reply["error"]))
        return reply

    def pc(self):
        regs = self.rsp.xchg("g")
        # AVR: 32 registers, SREG, SP (2), PC (4): the PC is a byte address, little-endian
        return int.from_bytes(bytes.fromhex(regs[2 * 35:2 * 39]), "little")

    def sp(self):
        g = bytes.fromhex(self.rsp.xchg("g"))
        return g[33] | g[34] << 8


def symbols(elf):
    out = {}
    for line in subprocess.run(["avr-nm", elf], capture_output=True, text=True,
                               check=True).stdout.splitlines():
        p = line.split()
        out[p[-1]] = int(p[0], 16)
    return out


def cmd_run(hexpath, card, out, script):
    elf = os.path.splitext(hexpath)[0] + ".elf"
    syms = symbols(elf)
    work = tempfile.mkdtemp(prefix="uzeamp-")
    shutil.copy(hexpath, work)
    os.makedirs(os.path.join(work, "card"))
    for f in os.listdir(card):
        if os.path.isfile(os.path.join(card, f)):
            shutil.copy(os.path.join(card, f), os.path.join(work, "card"))
    e = Emu(work, os.path.basename(hexpath))
    movies = sorted(f for f in os.listdir(card) if f.upper().endswith(".UZM"))
    movie = uzm.Uzm(os.path.join(card, movies[0])) if movies else None
    checked = [0, 0, 0]                     # frames checked, picture wrong, sound wrong
    try:
        e.rsp.expect_ok("Z0,%x,2" % syms["main"])
        e.cont()
        e.rsp.expect_ok("z0,%x,2" % syms["main"])
        lo = (syms["__bss_end"] & 0xFFFF)
        hi = e.sp() - 16
        e.write(0x800000 + lo, b"\xA5" * (hi - lo))
        for step in [s.split() for s in script.split(";") if s.strip()]:
            if step[0] == "f":
                e.monitor("frames %s" % step[1])
                e.cont()
            elif step[0] == "p":
                e.monitor("input 0 press %s %s" % (step[1].replace("+", " "), step[2]))
                e.monitor("frames %s" % step[2])
                e.cont()
            elif step[0] == "h":
                e.monitor("input 0 press %s %s" % (step[1].replace("+", " "), step[2]))
            elif step[0] == "d":
                # Movie: sect0, pictures, frames, frame, n, m, cut
                sect0, pics, total, start, n, m, cut = struct.unpack(
                    "<IIIIBBB", e.read(syms["movie"], 19))
                u = e.read(syms["uzm"], 26)
                frames, under = struct.unpack_from("<H", u, 18)[0], struct.unpack_from("<H", u, 12)[0]
                mark = u[14] | u[15] << 7 | u[16] << 14
                ps = e.read(syms["playerStatus"], 1)[0]
                snd = e.read(syms["mix_buf"], 524)
                print("%-9s status %d, movie from frame %d of %d (%d pictures, %d every %d "
                      "frames%s), %d frames since (the card test's included), last mark %d (sector %d), "
                      "rows missing %d, "
                      "sound %d..%d" % (step[1], ps, start, total, pics, m, n, ", cut" if cut else "",
                                        frames, mark, u[17], under, min(snd), max(snd)))
            elif step[0] == "v":
                hook = syms["movieFrameHook"]
                e.rsp.expect_ok("Z0,%x,2" % hook)
                e.monitor("frames %s" % step[1])
                bad = []
                late = len(step) > 2 and step[2] == "late"
                drawn = None
                while True:
                    e.cont()
                    if e.pc() != hook:
                        break
                    p = struct.unpack("<I", e.read(syms["movieChkP"], 4))[0]
                    k = e.read(syms["movieChkK"], 1)[0]
                    u = e.read(syms["uzm"], 26)
                    # a blank with no movie frame since the last (the first, when a movie starts
                    # without the card test's frames): nothing shown, no sound written
                    f = struct.unpack_from("<H", u, 18)[0]
                    if f == 0 or f == drawn:
                        continue
                    drawn = f
                    mark = u[14] | u[15] << 7 | u[16] << 14
                    aw = struct.unpack_from("<H", u, 8)[0] | 0x800000
                    fr = uzm.start_frame(p, movie.n, movie.m) + k
                    if late and k:
                        under = struct.unpack_from("<H", u, 12)[0] + 1
                        e.write(syms["uzm"] + 12, struct.pack("<H", under))
                        print("          a late row on frame %d (picture %d, showing %d)" % (fr, p, k + 1))
                        late = False
                    checked[0] += 1
                    if mark != p:
                        checked[1] += 1
                        bad.append("frame %d: picture %d's, the trailer said %d" % (fr, p, mark))
                    elif e.read(aw - uzm.SAMPLES, uzm.SAMPLES) != movie.frame_sound(fr):
                        checked[2] += 1
                        bad.append("frame %d (picture %d, showing %d): the sound differs" % (fr, p, k + 1))
                e.rsp.expect_ok("z0,%x,2" % hook)
                print("%-9s checked to now: %d frames, %d pictures wrong, %d sounds wrong%s" % (
                    "v %s" % step[1], checked[0], checked[1], checked[2],
                    ("; " + "; ".join(bad[:3])) if bad else ""))
            elif step[0] == "w":
                e.write(syms[step[1]], struct.pack("<H", int(step[2], 0)))
            elif step[0] == "s":
                path = os.path.abspath("%s_%s.png" % (out, step[1])).replace("\\", "/")
                e.monitor("screenshot " + path)
                print("shot %s" % path)
        if "cardTest" in syms:
            low, still, late = struct.unpack("<HHH", e.read(syms["cardTest"], 6))
            ct = e.read(syms["cardTest"], 10)
            still_cut, switches = struct.unpack_from("<HH", ct, 6)
            mv = e.read(syms["movie"], 19)
            print("the card test: lowest %d unread after a frame, a still's blank stored %d (cut %s), "
                  "rows late %d; warning %d, accepted %d; cardCut %d, the movie cut %d, switched to "
                  "the cut in play %d" % (low, still, still_cut if still_cut != 2048 else "-", late,
                                          e.read(syms["movieSlow"], 1)[0],
                                          e.read(syms["cardAccepted"], 1)[0],
                                          e.read(syms["cardCut"], 1)[0], mv[18], switches))
        if "uzm" in syms:
            u = e.read(syms["uzm"], 26)
            print("the renderer: %d video frames, rows missing on %d lines, last mark %s" %
                  (struct.unpack_from("<H", u, 18)[0], struct.unpack_from("<H", u, 12)[0],
                   u[14:18].hex()))
        # with uzmmode, 0x0800-0x0FFF is the video ring (the text, the file list): the stack's
        # own area starts at 0x1000
        floor = 0x1000 if lo < 0x0800 else lo
        ram = e.read(0x800000 + floor, hi - floor)
        deepest = next((floor + i for i, b in enumerate(ram) if b != 0xA5), hi)
        print(".bss ends at 0x%04x; the stack's area from 0x%04x: deepest write 0x%04x, %d bytes "
              "deep from RAMEND, %d bytes above its floor" % (lo, floor, deepest, 0x10FF - deepest,
                                                             deepest - floor))
    finally:
        e.close()
        shutil.rmtree(work, ignore_errors=True)


def raw(png):
    # the emulator's overlay text: its top 2 and bottom 4 character rows
    return subprocess.run(["ffmpeg", "-v", "error", "-i", png, "-vf", "crop=iw:ih-56:0:16",
                           "-f", "rawvideo", "-pix_fmt", "rgb24", "-"],
                          capture_output=True, check=True).stdout


def cmd_compare(a, b):
    import glob
    same = True
    for pa in sorted(glob.glob(a + "_*.png")):
        name = pa[len(a) + 1:-4]
        pb = "%s_%s.png" % (b, name)
        if not os.path.exists(pb):
            print("%-10s missing in %s" % (name, b)); same = False; continue
        ra, rb = raw(pa), raw(pb)
        diff = sum(x != y for x, y in zip(ra, rb)) // 3
        print("%-10s %s" % (name, "identical" if ra == rb else "%d pixels differ" % diff))
        same &= ra == rb
    return same


def main():
    a = sys.argv[1:]
    if len(a) >= 4 and a[0] == "run":
        cmd_run(a[1], a[2], a[3], a[4] if len(a) > 4 else M1_SCRIPT)
    elif len(a) == 3 and a[0] == "compare":
        sys.exit(0 if cmd_compare(a[1], a[2]) else 1)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
