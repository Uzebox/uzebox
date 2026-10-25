#!/usr/bin/env python3
"""Uzeamp in CUzeBox, driven through the GDB stub: a scripted session, its screenshots, and how
deep the stack went.

    python -I uzeamp_check.py run HEX CARD OUT [SCRIPT]   the session; screenshots OUT_<name>.png
    python -I uzeamp_check.py compare A_PREFIX B_PREFIX   the screenshots of two runs, the
                                                          emulator's overlay text cropped off

HEX: the ROM (.hex, its .elf beside it); CARD: a folder whose files make the card (copied with
the ROM into a scratch folder, which CUzeBox serves as the card).
UZEAMP_EMU_ARGS: more CUzeBox options, e.g. --no-sdhost (no card in the slot).

SCRIPT: steps separated by ";" (default: the M1 session below):
    f N             run N frames
    p BUTTONS N     press BUTTONS for N frames, then run them. The stub's names: start, select,
                    a, b, x, y, up, down, left, right, l, r (the shoulders: Uzeamp's SL/SR);
                    several joined with + (r+a)
    h BUTTONS N     hold BUTTONS for the next N frames, running none (f steps run them)
    s NAME          screenshot
    d NAME          the movie's state: its start picture, the frames since, the last trailer's
                    picture (start + frames - 1 while it plays from the start; a still keeps
                    one), the rows missing, the sound buffer's lowest and highest sample
The stack: at main, RAM from the end of .bss to 16 below SP is filled with 0xA5; at the end the
lowest byte changed is the stack's deepest point.
"""
import binascii
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time

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
        self.proc = subprocess.Popen([EMULATOR, hexname, "--esp=0", "--gdb=%d" % port,
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
    for f in os.listdir(card):
        shutil.copy(os.path.join(card, f), work)
    e = Emu(work, os.path.basename(hexpath))
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
                import struct
                mv = struct.unpack("<III", e.read(syms["movie"], 12))
                u = e.read(syms["uzm"], 20)
                frames, under = struct.unpack_from("<H", u, 18)[0], struct.unpack_from("<H", u, 12)[0]
                mark = u[14] | u[15] << 7 | u[16] << 14
                ps = e.read(syms["playerStatus"], 1)[0]
                ok = "ok" if frames == 0 or mark == mv[2] + frames - 1 else "not start + frames - 1"
                snd = e.read(syms["mix_buf"], 524)
                print("%-9s status %d, movie from picture %d of %d, %d frames since, last mark "
                      "%d (sector %d): %s, rows missing %d, sound %d..%d" %
                      (step[1], ps, mv[2], mv[1], frames, mark, u[17], ok, under, min(snd), max(snd)))
            elif step[0] == "s":
                path = os.path.abspath("%s_%s.png" % (out, step[1])).replace("\\", "/")
                e.monitor("screenshot " + path)
                print("shot %s" % path)
        if "cardTest" in syms:
            import struct
            low, still, late = struct.unpack("<HHH", e.read(syms["cardTest"], 6))
            print("the card test: lowest %d unread after a frame, a still's blank stored %d, rows "
                  "late %d; warning %d, accepted %d" % (low, still, late,
                                                       e.read(syms["movieSlow"], 1)[0],
                                                       e.read(syms["cardAccepted"], 1)[0]))
        if "uzm" in syms:
            import struct
            u = e.read(syms["uzm"], 20)
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
