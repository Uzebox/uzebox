#!/usr/bin/env python3
"""gen_uzm.py -- writes uzm_frame.inc, the video frame of Uzeamp's mode, and checks its timing.

    python -I gen_uzm.py            write ../uzmmode/uzm_frame.inc and report
    python -I gen_uzm.py --check    check only

The frame (see uzm.def.h): entered at cycle 467 of a line like every mode, it draws UZM_LINES
lines from the next one with one line program, run once a line:

  head   0-302    the SPI slots of the blank (P at 0, then one every 18 cycles), the sync pulse
                  (cbi at 3, sbi at 139, as the kernel's hsync_pulse), the row's start (line A
                  keeps X, line B goes back to it), the trailer (line B of a sector's last row:
                  the showing's 8 samples into the sound bank, sectors 2-34 only, its mark),
                  the sound sample out
  loop   303-1741 80 periods of 18 cycles: 2 pixels (9 cycles apart, the first at 307) and one
                  SPI slot each; the loop's counting fits the period
  tail   1742-1819 the black pixel at 1747, R (the last byte in: nothing is clocked from
                  there to the next line's P), the counts, the stall decision (SPI on or off for
                  the next line), the next line's index

An SPI slot (9 cycles): in r17 (the byte the slot before started) / out SPDR, r20 (the next) /
st Y, r17 / cp r17, r21 / adc YL / adc YH / andi YH / ori YH: Y moves on when the byte is below
r21 (0xFE; 0 when stalled). A byte is read 17 cycles after the write that started it.

Version 2 of the format (../plan-rates.md): a picture is shown 1-3 times, and each showing has
its own 8 samples in the trailer (at 1, 9 or 17: uzm.toff = 161, 169 or 177 from the row's
start), kept on sectors 2-34 only, sector 34's starting 2 before. Which sector a line is in
comes from r25 (the lines left, uzm.lines at the frame's first line): sector 2's last line has
r25 = uzm.khi, sector 34's uzm.klo, so the same code draws the full frame (228 lines from
sector 0) and the cut one (204 from sector 2). Nothing the card sends steers a write.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "uzm_frame.inc")

LINE = 1820
T_ENTRY = 467
T_CBI, T_SBI = 3, 139           # the kernel's pulse: cbi, then sbi 136 cycles later
T_LOOP, T_PIX0, PERIOD, PERIODS = 303, 307, 18, 80
T_TAIL = T_LOOP + PERIOD * PERIODS - 1   # the loop's last brne falls through: 1742
T_BLACK = T_PIX0 + 9 * 2 * PERIODS       # 1747
T_R = 1748
HEAD_SLOTS = 16
LINES = 228

ONE = {"out", "in", "cp", "cpc", "cpi", "adc", "add", "sub", "sbc", "subi", "sbci", "andi", "ori",
       "and", "mov", "movw", "ldi", "inc", "dec", "clr", "nop", "eor", "tst", "com", "or"}
TWO = {"ld", "st", "ldd", "std", "lds", "sts", "adiw", "sbiw", "rjmp", "cbi", "sbi"}


def cyc(ins):
    op = ins.split()[0]
    if op in ONE:
        return 1
    if op in TWO:
        return 2
    raise ValueError("cycles of %r?" % ins)


class G:
    """A group: instructions that stay together (flags, skips). cycles = total, whichever path:
    the constant-time skips and branches are given with their total."""
    def __init__(self, ins, cycles=None, after=None):
        self.ins = ins if isinstance(ins, list) else [ins]
        self.cycles = cycles if cycles is not None else sum(cyc(i) for i in self.ins)
        self.after = after


SLOT = ["in    r17, SPDR_IO", "out   SPDR_IO, r20", "st    Y, r17", "cp    r17, r21",
        "adc   YL, r1", "adc   YH, r1", "andi  YH, 0x0F", "ori   YH, 0x08"]   # 9 cycles
R_SLOT = [SLOT[0]] + SLOT[2:]                                                # 8: no out


class Timeline:
    def __init__(self, start, end):
        self.start, self.end = start, end
        self.at = {}                    # cycle -> (text, cycles)

    def free(self, t, n):
        return all(t + k < self.end and (t + k) not in self.busy for k in range(n))

    @property
    def busy(self):
        b = set()
        for t, (_, n) in self.at.items():
            b.update(range(t, t + n))
        return b

    def put(self, t, text, n):
        assert self.free(t, n), "cycle %d (%s) is taken" % (t, text)
        self.at[t] = (text, n)

    def put_seq(self, t, lines):
        for ins in lines:
            n = cyc(ins)
            self.put(t, ins, n)
            t += n
        return t

    def fill(self, items, t0):
        """Places the groups in order, each in the first free span after the one before."""
        t = t0
        for g in items:
            if g.after is not None:
                t = max(t, g.after)
            while not self.free(t, g.cycles):
                t += 1
                if t >= self.end:
                    raise RuntimeError("no room for %s" % g.ins)
            if len(g.ins) == 1:
                self.put(t, g.ins[0], g.cycles)
            else:
                self.put(t, " ; ".join(g.ins), g.cycles)
            t += g.cycles
        return t

    def lines(self):
        out, t = [], self.start
        busy = self.busy
        while t < self.end:
            if t in self.at:
                text, n = self.at[t]
                for k, part in enumerate(text.split(" ; ")):
                    out.append((t if k == 0 else None, part))
                t += n
            else:
                assert t not in busy
                out.append((t, "nop"))
                t += 1
        return out


def head():
    tl = Timeline(0, T_LOOP)
    tl.put(0, "out   SPDR_IO, r20", 1)                         # P
    for k in range(1, HEAD_SLOTS + 1):
        tl.put_seq(PERIOD * k - 1, SLOT)
    tl.put(T_CBI, "cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN", 2)
    tl.put(T_SBI, "sbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN", 2)
    items = [
        # line A keeps the row's start, line B goes back to it (r14 = 2j: bit 1 = line B)
        G(["sbrs  r14, 1", "movw  r2, XL"], 2),
        G(["sbrc  r14, 1", "movw  XL, r2"], 2),
        # r15: the trailer's 32 bytes, skipped after line B of a sector's last row (j = 5)
        G(["mov   r19, r14", "cpi   r19, 10", "ldi   r19, 0", "brne  .+2", "ldi   r19, 32"], 5),
        G("mov   r15, r19"),
        # the trailer, 160 bytes on from the row; read on every line, kept on j = 5 of sectors
        # 2-34 only. The showing's samples: uzm.toff on from the row's start
        G("lds   r19, uzm+UZM_O_TOFF"),
        G(["add   XL, r19", "adc   XH, r1"]),
        # r18: 0xFF to keep (j = 5, khi >= r25 >= klo), else 0
        G("lds   r16, uzm+UZM_O_KLO"),
        G("lds   r22, uzm+UZM_O_KHI"),
        G(["cp    r1, r15", "sbc   r18, r18"]),                 # 0xFF on j = 5
        G(["cp    r25, r16", "sbc   r23, r23"]),                # 0xFF below sector 34
        G("com   r23"),
        G("and   r18, r23"),
        G(["cp    r22, r25", "sbc   r23, r23"]),                # 0xFF above sector 2
        G("com   r23"),
        G("and   r18, r23"),
        # sector 34's samples start 2 before (r25 = klo), whatever the card sent, so that a
        # showing's 262 end at its bank's end
        G(["cp    r25, r16", "ldi   r16, 0", "brne  .+2", "ldi   r16, 2"], 4),
        G("and   r16, r18"),
        G("ldi   ZL, lo8(uzm_dummy)"),
        G("ldi   ZH, hi8(uzm_dummy)"),
        G(["sbrc  r18, 0", "movw  ZL, r6"], 2),                 # the bank, or 8 bytes of nowhere
        G(["sub   ZL, r16", "sbc   ZH, r1"]),
    ]
    for _ in range(8):
        items += [G("ld    r19, X+"), G("st    Z+, r19")]
    items += [G(["sbrc  r18, 0", "movw  r6, ZL"], 2)]
    # the mark, at the trailer's 25
    items += [G("movw  XL, r2"), G(["subi  XL, lo8(-185)", "sbci  XH, hi8(-185)"])]
    for i in range(4):
        items += [G("ld    r19, X+"), G("sts   uzm+UZM_O_MARK+%d, r19" % i)]
    items += [
        G("movw  XL, r2"),
        # the sound: one sample a line, mix_pos in r5:r4, wrapping at mix_buf's end
        G("movw  ZL, r4"),
        G("ld    r19, Z+"),
        G("sts   _SFR_MEM_ADDR(OCR2A), r19"),
        G("ldi   r22, hi8(mix_buf+MIX_BUF_SIZE)"),
        G(["cpi   ZL, lo8(mix_buf+MIX_BUF_SIZE)", "cpc   ZH, r22", "ldi   r22, lo8(mix_buf)",
           "ldi   r23, hi8(mix_buf)", "brlo  .+2", "movw  ZL, r22"], 6),
        G("movw  r4, ZL"),
        G("ldi   r24, %d" % PERIODS),
    ]
    tl.fill(items, 1)
    return tl


def loop_lines():
    return [
        "ld    r16, X+", "ld    r18, X+", "out   VIDEO_PORT, r16", "in    r17, SPDR_IO",
        "out   SPDR_IO, r20", "st    Y, r17", "cp    r17, r21", "adc   YL, r1", "adc   YH, r1",
        "andi  YH, 0x0F", "out   VIDEO_PORT, r18", "ori   YH, 0x08", "dec   r24",
    ]   # + brne uzm_loop: 18 cycles taken


STALL_DECISION = [
    G("movw  r22, r8"),
    G(["sub   r22, r10", "sbc   r23, r11"]),                    # r23:r22 = unread bytes
    G(["cpi   r22, 160", "cpc   r23, r1", "brge  .+2", "inc   r0"], 4),   # a row missing
    G("ldi   r21, 0xFE"),
    G("ldi   r19, hi8(UZM_STALL)"),
    G(["cpi   r22, lo8(UZM_STALL)", "cpc   r23, r19", "brlt  .+2", "ldi   r21, 0"], 4),
    G("lds   r19, uzm+UZM_O_PROD"),
    G("and   r21, r19"),                                       # producer off: always stalled
    G("mov   r19, r21"),
    G("andi  r19, 0x40"),
    G("ori   r19, (1 << MSTR)"),
    G("out   _SFR_IO_ADDR(SPCR), r19"),                        # SPE with r21: on, or off
]


def tail():
    tl = Timeline(T_TAIL, LINE)
    tl.put(T_BLACK, "out   VIDEO_PORT, r1", 1)
    tl.put_seq(T_R, R_SLOT)
    tl.put(LINE - 4, "dec   r25", 1)
    tl.put(LINE - 3, "breq  uzm_end", 1)                       # 1817: not taken, but on the
    tl.put(LINE - 2, "rjmp  uzm_line", 2)                      # last line: then 1819 in uzm_end
    items = [
        G(["add   XL, r15", "adc   XH, r1"]),                  # the trailer skipped (j = 5)
        G(["sbrc  XH, 4", "ldi   XH, 0x08"], 2),               # the ring's end
        # bytes stored this line, after R
        G("movw  r22, YL", after=T_R + 8),
        G(["sub   r22, r12", "sbc   r23, r13"]),
        G("andi  r23, 0x07"),
        G(["add   r8, r22", "adc   r9, r23"]),
        G("movw  r12, YL"),
        # bytes shown: the row on line B, the trailer after j = 5
        G(["add   r10, r15", "adc   r11, r1"]),
        G("ldi   r22, 0"),
        G(["sbrc  r14, 1", "ldi   r22, 160"], 2),
        G(["add   r10, r22", "adc   r11, r1"]),
    ] + STALL_DECISION + [
        G("inc   r14"),
        G("inc   r14"),
        G(["mov   r19, r14", "cpi   r19, 12", "brne  .+2", "clr   r14"], 4),
        G("sts   mix_pos, r4"),
        G("sts   mix_pos+1, r5"),
    ]
    tl.fill(items, T_TAIL)
    return tl


PROLOGUE = [
    "lds   XL, uzm+UZM_O_X", "lds   XH, uzm+UZM_O_X+1",
    "lds   YL, uzm+UZM_O_Y", "lds   YH, uzm+UZM_O_Y+1",
    "movw  r12, YL",
    "lds   r8, uzm+UZM_O_PC", "lds   r9, uzm+UZM_O_PC+1",
    "lds   r10, uzm+UZM_O_CC", "lds   r11, uzm+UZM_O_CC+1",
    "lds   r6, uzm+UZM_O_AW", "lds   r7, uzm+UZM_O_AW+1",
    "lds   r4, mix_pos", "lds   r5, mix_pos+1",
    "ldi   r20, 0xFF", "clr   r0", "clr   r14", "clr   r15",
    "lds   r25, uzm+UZM_O_LINES",
    "ldi   r19, (1 << SPI2X)", "out   _SFR_IO_ADDR(SPSR), r19",
]

EPILOGUE = """\
uzm_end:
	nop                                 ; 1819 (the breq taken at 1817)
	rcall hsync_pulse                   ; 1820: its cbi at 3, as every line's

	sts   uzm+UZM_O_X, XL
	sts   uzm+UZM_O_X+1, XH
	sts   uzm+UZM_O_Y, YL
	sts   uzm+UZM_O_Y+1, YH
	sts   uzm+UZM_O_PC, r8
	sts   uzm+UZM_O_PC+1, r9
	sts   uzm+UZM_O_CC, r10
	sts   uzm+UZM_O_CC+1, r11
	sts   uzm+UZM_O_AW, r6
	sts   uzm+UZM_O_AW+1, r7
	lds   r24, uzm+UZM_O_UNDER
	lds   r25, uzm+UZM_O_UNDER+1
	add   r24, r0
	adc   r25, r1
	sts   uzm+UZM_O_UNDER, r24
	sts   uzm+UZM_O_UNDER+1, r25
	lds   r24, uzm+UZM_O_FRAMES
	lds   r25, uzm+UZM_O_FRAMES+1
	adiw  r24, 1
	sts   uzm+UZM_O_FRAMES, r24
	sts   uzm+UZM_O_FRAMES+1, r25

	; the bus is the program's until the next frame: the SPI off, nothing being clocked (the
	; last line's R took the last byte in)
	ldi   r19, (1 << MSTR)
	out   _SFR_IO_ADDR(SPCR), r19

	; the inline pulses did not count down sync_pulse; hsync_pulse did, once
	lds   r24, sync_pulse
	lds   r25, uzm+UZM_O_LINES
	sub   r24, r25
	sts   sync_pulse, r24

	; vsync flag, field flip
	lds   ZL, sync_flags
	ldi   r20, SYNC_FLAG_FIELD
	ori   ZL, SYNC_FLAG_VSYNC
	eor   ZL, r20
	sts   sync_flags, ZL

	; clear any pending timer interrupt
	ldi   ZL, (1 << OCF1A)
	sts   _SFR_MEM_ADDR(TIFR1), ZL
	ret
"""


def build():
    out = []
    w = out.append
    w("; uzm_frame.inc -- the video frame of Uzeamp's mode, GENERATED by gen_uzm.py; do not edit.")
    w("; Cycles in the comments: from the entry line's start in the prologue, then from the line's")
    w("; start in the line program.")
    w("")
    w("sub_video_uzm:")
    t = T_ENTRY
    w("\tlds   r16, uzm+UZM_O_ON                ; %4d" % t); t += 2
    w("\tsbrs  r16, 0                           ; %4d  video: skips the jmp (3 cycles)" % t); t += 1
    w("\tjmp   sub_video_mode5                  ; %4d  text: mode 5 entered at %d" % (t, t + 3))
    t += 2                                     # the video path: sbrs skipping a 2-word jmp = 3
    text_entry = T_ENTRY + 2 + 1 + 3
    for ins in PROLOGUE:
        w("\t%-38s ; %4d" % (ins, t)); t += cyc(ins)
    for g in STALL_DECISION:
        if g.ins[0].startswith("cpi   r22, 160"):
            continue                           # no underrun count before the first line
        for k, ins in enumerate(g.ins):
            w("\t%-38s ; %4d" % (ins, t) if k == 0 else "\t%-38s ;" % ins)
        t += g.cycles
    w("\tWAIT  r19, %d                          ; %4d" % (LINE - t, t))
    t = LINE
    w("")
    w("uzm_line:                                    ; %4d = 0 of the first line" % t)
    hd, tl = head(), tail()
    for c, ins in hd.lines():
        w("\t%-38s ;%s" % (ins, " %4d" % c if c is not None else ""))
    w("uzm_loop:                                    ; %4d" % T_LOOP)
    c = T_LOOP
    for ins in loop_lines():
        w("\t%-38s ; %4d + 18k" % (ins, c)); c += cyc(ins)
    w("\t%-38s ; %4d + 18k (falls through at %d on the last)" % ("brne  uzm_loop", c, T_TAIL - 1))
    for c, ins in tl.lines():
        w("\t%-38s ;%s" % (ins, " %4d" % c if c is not None else ""))
    w("")
    w(EPILOGUE)
    return "\n".join(out) + "\n", hd, tl, text_entry


def check(hd, tl):
    """Timing over two whole lines: outs to the DAC, the SPI's writes and reads, the pulse."""
    ev = []                                     # (cycle, kind)
    for base in (0, LINE):
        for tlx in (hd, tl):
            for t, (text, n) in tlx.at.items():
                parts = text.split(" ; ")
                tt = t
                for p in parts:
                    if p.startswith("out   VIDEO_PORT"):
                        ev.append((base + tt, "pix"))
                    elif p.startswith("out   SPDR_IO"):
                        ev.append((base + tt, "spi_out"))
                    elif p.startswith("in    r17, SPDR_IO"):
                        ev.append((base + tt, "spi_in"))
                    elif p.startswith("cbi"):
                        ev.append((base + tt, "cbi"))
                    elif p.startswith("sbi"):
                        ev.append((base + tt, "sbi"))
                    tt += cyc(p) if len(parts) > 1 and p.split()[0] in ONE | TWO else 0
        c = T_LOOP
        for k in range(PERIODS):
            o = base + T_LOOP + PERIOD * k
            ev += [(o + 4, "pix"), (o + 5, "spi_in"), (o + 6, "spi_out"), (o + 13, "pix")]
    ev.sort()
    errs = []
    pix = [t for t, k in ev if k == "pix" and t < LINE]
    if len(pix) != 2 * PERIODS + 1:
        errs.append("%d DAC outs in a line" % len(pix))
    if pix[0] != T_PIX0:
        errs.append("first pixel at %d" % pix[0])
    for a, b in zip(pix, pix[1:]):
        if b - a != 9:
            errs.append("pixels %d, %d" % (a, b))
    last_out = None
    for t, k in ev:
        if k == "spi_out":
            if last_out is not None and t - last_out < 18:
                errs.append("SPI writes %d, %d" % (last_out, t))
            last_out = t
        elif k == "spi_in" and last_out is not None and t - last_out < 17:
            errs.append("SPI read %d, %d after its write" % (t, t - last_out))
    cbis = [t for t, k in ev if k == "cbi"]
    sbis = [t for t, k in ev if k == "sbi"]
    if cbis != [T_CBI, LINE + T_CBI] or sbis != [T_SBI, LINE + T_SBI]:
        errs.append("pulse: cbi %s, sbi %s" % (cbis, sbis))
    n_in = len([1 for t, k in ev if k == "spi_in" and t < LINE])
    return errs, n_in


def main():
    text, hd, tl, text_entry = build()
    errs, n_in = check(hd, tl)
    free_head = T_LOOP - len(hd.busy)
    free_tail = (LINE - T_TAIL) - len(tl.busy)
    print("a line: %d bytes read (%d in the blank), %d spare cycles in the head, %d in the tail"
          % (n_in, n_in - PERIODS, free_head, free_tail))
    print("mode 5 is entered at cycle %d: its first WAIT is %d cycles shorter" %
          (text_entry, text_entry - T_ENTRY))
    if errs:
        print("TIMING ERRORS:\n  " + "\n  ".join(errs))
        sys.exit(1)
    print("timing checked: pixels 9 apart from %d, SPI writes 18 apart, reads 17 after, pulse %d-%d"
          % (T_PIX0, T_CBI, T_SBI))
    if "--check" not in sys.argv:
        open(OUT, "w", newline="\n").write(text)
        print("wrote %s" % OUT)


if __name__ == "__main__":
    main()
