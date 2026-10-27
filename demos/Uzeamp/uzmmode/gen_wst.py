#!/usr/bin/env python3
"""gen_wst.py -- writes wst_frame.inc: the text frame that streams a song from the card (a
plain WAV, read with one multiple-block read), and the blank's fill. Checks every line's timing.

    python -I gen_wst.py            write ../uzmmode/wst_frame.inc and report
    python -I gen_wst.py --check    check only

See ../plan-stream.md. The song plays from mix_buf (the ring, 524 bytes) read at mix_pos, as
the kernel's update_sound plays it in the blank (SOUND_STREAM=1, defines.h). The frame is
mode 5's text (30 tiles of 8 pixels, lines from first_render_line, the same picture), drawn
by a generated line program, one for each rate:

  head   0-303    the sync pulse (cbi at 3, sbi at 139, as the kernel's hsync_pulse); sample A
                  out at 26 (update_sound's cycle); its slot silenced; sample B read (31.5 kHz;
                  at 15.7 kHz B = A); the card's byte or two (each 0xFF out, read 17 or more
                  cycles later, then the framing state machine); the tile row's set-up
  loop   304-1742 30 tiles of 48 cycles, as mode 5's (pixels 6 cycles apart from 307); their 5
                  spare cycles write OCR2A from r19 every tile (dec r2 / sbrc r2,7 + mov r19,r3
                  / sts): sample A for tiles 0-12, B from tile 13 (at 935, half a line after A)
  tail   1743-1819 the black pixel at 1747, mix_pos, the tile row, the next line

The framing state machine, a byte in r17, 50 cycles whatever the byte (k in r9:r8 counts the
sector's bytes left: 514..3 data, 2..1 CRC, 0 waiting for the token):
  - data (k > 2, and wst.left, the song's bytes still wanted, not 0: left - 1): stored at W
    (r5:r4), W moves on (wrapping at mix_buf's end);
  - otherwise 0x80 is written at W (the next data byte replaces it), W stays;
  - k = 0: 0xFE (the token) sets k = 514; 0xFF waits; any other byte is an error token:
    r13 = 0xFF, which stops the clocking (no more 0xFF out) and makes every later byte read
    as 0xFF, so nothing more is stored until the program restarts the read;
  - otherwise k = k - 1.
r13 (wst.off) = 0xFF also when the program has stopped the stream: the frame then plays the
ring, clocks nothing and leaves the SPI alone.

Nothing stores more than a line plays: the frame's lines read their samples before their
bytes, so the ring never overflows in the frame; the fill stops at WST_FILL_MAX unread.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "wst_frame.inc")

LINE = 1820
T_ENTRY = 479                       # mode 5's entry (473) and its dispatch (6)
T_CBI, T_SBI = 3, 139
T_A = 26                            # update_sound's sts OCR2A in the kernel's hsync lines
T_LOOP, TILE, TILES = 304, 48, 30
T_PIX0 = T_LOOP + 3                 # 307
T_TAIL = T_LOOP + TILE * TILES - 1  # 1743: the last brne falls through
T_BLACK = T_PIX0 + 6 * 8 * TILES    # 1747
B_TILE = 13                         # the first tile writing B
T_B = T_LOOP + TILE * B_TILE + 7    # 935
END = "mix_buf+MIX_BUF_SIZE"

ONE = {"out", "in", "cp", "cpc", "cpi", "adc", "add", "sub", "sbc", "subi", "sbci", "andi", "ori",
       "and", "or", "mov", "movw", "ldi", "inc", "dec", "clr", "nop", "eor", "tst", "com", "lsl",
       "cli", "sei"}
TWO = {"ld", "st", "ldd", "std", "lds", "sts", "adiw", "sbiw", "rjmp", "cbi", "sbi", "mul"}
THREE = {"lpm", "rcall"}


def cyc(ins):
    op = ins.split()[0]
    if op.endswith(":"):
        return 0
    if op in ONE:
        return 1
    if op in TWO:
        return 2
    if op in THREE:
        return 3
    raise ValueError("cycles of %r?" % ins)


class G:
    """A group: instructions that stay together (flags, skips, branches). cycles: the total,
    whichever path (given for groups with skips or branches)."""
    def __init__(self, ins, cycles=None, after=None, tag=None):
        self.ins = ins if isinstance(ins, list) else [ins]
        self.cycles = cycles if cycles is not None else sum(cyc(i) for i in self.ins)
        self.after = after
        self.tag = tag


class Timeline:
    def __init__(self, start, end):
        self.start, self.end = start, end
        self.at = {}                    # cycle -> (parts, cycles)
        self.tags = {}                  # tag -> start cycle

    def busy(self):
        b = set()
        for t, (_, n) in self.at.items():
            b.update(range(t, t + n))
        return b

    def free(self, t, n, busy):
        return t + n <= self.end and all((t + k) not in busy for k in range(n))

    def put(self, t, parts, n, tag=None):
        assert self.free(t, n, self.busy()), "cycle %d (%s) is taken" % (t, parts)
        self.at[t] = (parts if isinstance(parts, list) else [parts], n)
        if tag:
            self.tags[tag] = t

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
                t = max(t, g.after(self) if callable(g.after) else g.after)
            busy = self.busy()
            while not self.free(t, g.cycles, busy):
                t += 1
                if t >= self.end:
                    raise RuntimeError("no room for %s" % g.ins)
            self.put(t, g.ins, g.cycles, g.tag)
            t += g.cycles
        return t

    def lines(self):
        out, t = [], self.start
        busy = self.busy()
        while t < self.end:
            if t in self.at:
                parts, n = self.at[t]
                for k, part in enumerate(parts):
                    out.append((t if k == 0 else None, part))
                t += n
            else:
                assert t not in busy
                out.append((t, "nop"))
                t += 1
        return out


def wrap(p, tmp):
    """p (XL or ZL) back to mix_buf at mix_buf's end, r15:r14 = mix_buf: 5 cycles"""
    h = "XH" if p == "XL" else "ZH"
    return [G("ldi   %s, hi8(%s)" % (tmp, END)),
            G(["cpi   %s, lo8(%s)" % (p, END), "cpc   %s, %s" % (h, tmp), "brne  .+2",
               "movw  %s, r14" % p], 4)]


K_UPDATE = [
    "movw  r24, r8", "sbiw  r24, 1", "brcs  1f",
    # counting: k - 1
    "movw  r8, r24", "rjmp  .", "rjmp  .", "rjmp  .", "rjmp  .", "nop", "rjmp  2f",
    # waiting (k was 0): 0xFE -> 514, 0xFF -> 0, other -> 0 and r13 = 0xFF
    "1:", "cpi   r17, 0xFE", "ldi   r16, 0", "brne  .+2", "ldi   r16, 0xFF",
    "cpi   r17, 0xFE", "sbc   r18, r18", "or    r13, r18",
    "ldi   r24, 2", "and   r24, r16", "mov   r25, r24", "movw  r8, r24",   # 514 = 0x0202
    "2:",
]


def sm_body(fill=False):
    """The state machine after its in r17 (and its optional out): 49 cycles. fill: the blank's,
    with interrupts on: a byte's store and W's move (published to sound_stream_w) together,
    interrupts off, so that a kernel read parked at W never plays the byte before W passes it
    (it would play it twice)"""
    return [
        G("or    r17, r13"),                                        # stopped: 0xFF
        G("lds   r24, wst+WST_O_LEFT"), G("lds   r25, wst+WST_O_LEFT+1"),
        G("ldi   r16, 2"),
        G(["cp    r16, r8", "cpc   r11, r9", "sbc   r16, r16"]),    # 0xFF: data (k > 2)
        G(["sbiw  r24, 1", "sbc   r18, r18"]),                      # 0xFF: nothing left
        G("com   r18"),
        G("and   r16, r18"),                                        # data: k > 2, left > 0
        G(["sbrc  r16, 0", "sts   wst+WST_O_LEFT, r24"], 3),        # left - 1 with data
        G(["sbrc  r16, 0", "sts   wst+WST_O_LEFT+1, r25"], 3),
        G("mov   r21, r17"),
        G(["sbrs  r16, 0", "ldi   r21, 0x80"], 2),                  # not data: 0x80 at W
        G("movw  XL, r4"),
    ] + ([G("cli")] if fill else []) + [
        G("st    X+, r21"),
    ] + wrap("XL", "r18") + [
        G(["sbrc  r16, 0", "movw  r4, XL"], 2),                     # W moves on with data
    ] + ([G("sts   sound_stream_w, r4"), G("sts   sound_stream_w+1, r5"), G("sei")]
         if fill else []) + [
        G(K_UPDATE, 16),
    ]


SM_CYCLES = 1 + sum(g.cycles for g in sm_body())                     # 35 with the in
SPI_OUT = ["sbrs  r13, 0", "out   SPDR_IO, r20"]                     # no clock when stopped


PREAMBLE = [                        # mode 5's render_tile_line before its loop: 18 cycles
    "movw  XL, YL", "ldi   r17, TILE_HEIGHT*TILE_WIDTH",
    "lds   r24, tile_table_lo", "lds   r25, tile_table_hi",
    "mul   r22, r23", "add   r24, r0", "adc   r25, r1",
    "ld    r20, X+", "mul   r20, r17", "add   r0, r24", "adc   r1, r25",
    "movw  ZL, r0", "ldi   r18, SCREEN_TILES_H",
]


# After a sample is played from slot Z (= R): the slot left at 0x80 and R moved on, unless R is
# at W (r5:r4, the next slot the stream fills): the ring is empty, the sample was 0x80 and R
# stays (r12 set: a hold). 8 cycles either way.
HOLD = ["cp    ZL, r4", "cpc   ZH, r5", "breq  1f",
        "ldi   r16, 0x80", "st    Z+, r16", "rjmp  2f",
        "1:", "ldi   r16, 1", "or    r12, r16", "rjmp  .", "2:"]


def head(rate31):
    tl = Timeline(0, T_LOOP)
    tl.put(T_CBI, "cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN", 2)
    tl.put(T_SBI, "sbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN", 2)
    tl.put(T_A - 2, "ld    r19, Z", 2)
    tl.put(T_A, "sts   _SFR_MEM_ADDR(OCR2A), r19", 2, tag="A")
    t_pre = T_LOOP - sum(cyc(i) for i in PREAMBLE)
    tl.put_seq(t_pre, PREAMBLE)
    items = [G("movw  ZL, r6")]                                     # Z = R, before A's ld
    items += [G(HOLD, 8, after=T_A + 2)] + wrap("ZL", "r16")
    if rate31:
        items += [G("ld    r3, Z"), G(HOLD, 8)] + wrap("ZL", "r16")
        items += [G("movw  r6, ZL")]
    else:
        items += [G("movw  r6, ZL"), G("mov   r3, r19")]           # B = A
    items += [G("ldi   r16, %d" % (B_TILE - 1)), G("mov   r2, r16"), G("ldi   r20, 0xFF"),
              G(SPI_OUT, 2, tag="out1")]
    out1 = lambda t: t.tags["out1"] + 1 + 17
    items += [G("in    r17, SPDR_IO", after=out1, tag="in1")]
    if rate31:
        items += [G(SPI_OUT, 2, tag="out2")]
    items += sm_body()
    if rate31:
        out2 = lambda t: t.tags["out2"] + 1 + 17
        items += [G("in    r17, SPDR_IO", after=out2, tag="in2")] + sm_body()
    end = tl.fill(items, 0)
    assert end <= t_pre, "the head runs into the tile row's set-up (%d > %d)" % (end, t_pre)
    return tl


def loop_lines():
    """A tile, 48 cycles: (offset, instruction); pixels at 3 + 6j"""
    return [
        (0, "lpm   r16, Z+"), (3, "out   VIDEO_PORT, r16"),
        (4, "lpm   r16, Z+"), (7, "sts   _SFR_MEM_ADDR(OCR2A), r19"), (9, "out   VIDEO_PORT, r16"),
        (10, "ld    r20, X+"), (12, "lpm   r16, Z+"), (15, "out   VIDEO_PORT, r16"),
        (16, "mul   r20, r17"), (18, "lpm   r16, Z+"), (21, "out   VIDEO_PORT, r16"),
        (22, "movw  r20, r24"), (23, "add   r20, r0"), (24, "lpm   r16, Z+"),
        (27, "out   VIDEO_PORT, r16"), (28, "adc   r21, r1"), (29, "dec   r2"),
        (30, "lpm   r16, Z+"), (33, "out   VIDEO_PORT, r16"),
        (34, "sbrc  r2, 7"), (35, "mov   r19, r3"),                 # 2 cycles either way
        (36, "lpm   r16, Z+"), (39, "out   VIDEO_PORT, r16"),
        (40, "lpm   r16, Z+"), (43, "movw  ZL, r20"), (44, "dec   r18"),
        (45, "out   VIDEO_PORT, r16"), (46, "brne  %s"),
    ]


T_END = LINE - 5                    # 1815: the line's end is fixed from here (tail_end)


def tail_end(name):
    """1815-1819: the next line, or (on the last) the kernel's pulse, its cbi at 3 of the next"""
    return [
        "\tdec   r10                              ; %4d" % T_END,
        "\tbrne  2f                               ; %4d: taken, at %d" % (T_END + 1, T_END + 3),
        "\tnop                                    ; %4d" % (T_END + 2),
        "\tnop                                    ; %4d" % (T_END + 3),
        "\tcall  hsync_pulse                      ; %4d: the last line: its cbi at 3, A at 26"
        % (T_END + 4),
        "\tjmp   wst_end",
        "2:\trjmp  %-32s ; %4d" % (name, T_END + 3),
    ]


def tail(name):
    tl = Timeline(T_TAIL, T_END)
    tl.put(T_BLACK, "out   VIDEO_PORT, r11", 1)
    items = [
        G("sts   mix_pos, r6"), G("sts   mix_pos+1, r7"),           # for the kernel's pulse
        G("sts   sound_stream_w, r4"), G("sts   sound_stream_w+1, r5"),
        G("inc   r22"),
        G(["cpi   r22, 8", "ldi   r16, 0", "brne  .+2", "ldi   r16, VRAM_TILES_H"], 4),
        G(["add   YL, r16", "adc   YH, r11"]),
        G("andi  r22, 7"),
    ]
    end = tl.fill(items, T_TAIL)
    assert end <= T_END
    return tl


def prologue(rate31, t):
    """From cycle t of the entry line to its end: the frame's registers, the SPI, and at
    31.5 kHz the entry line's B (the kernel's compare B can't interrupt the frame)."""
    out = []

    def w(ins, n=None):
        nonlocal t
        out.append("\t%-38s ; %4d" % (ins, t))
        t += cyc(ins) if n is None else n

    for ins in ["lds   r4, sound_stream_w", "lds   r5, sound_stream_w+1",
                "sts   wst+WST_O_W0, r4", "sts   wst+WST_O_W0+1, r5",
                "lds   r6, mix_pos", "lds   r7, mix_pos+1",
                "lds   r8, wst+WST_O_K", "lds   r9, wst+WST_O_K+1",
                "clr   r11", "clr   r12", "lds   r13, wst+WST_O_OFF",
                "ldi   r16, lo8(mix_buf)", "mov   r14, r16", "ldi   r16, hi8(mix_buf)", "mov   r15, r16",
                "ldi   YL, lo8(vram)", "ldi   YH, hi8(vram)",
                "lds   r10, render_lines_count", "clr   r22", "ldi   r23, TILE_WIDTH",
                "ldi   r16, (1 << SPE) | (1 << MSTR)", "ldi   r17, (1 << SPI2X)"]:
        w(ins)
    w("sbrs  r13, 0", 1); out.append("\t%-38s ;" % "out   _SFR_IO_ADDR(SPCR), r16"); t += 1
    w("sbrs  r13, 0", 1); out.append("\t%-38s ;" % "out   _SFR_IO_ADDR(SPSR), r17"); t += 1
    # stopped: k = 0, so that the bytes, read as 0xFF, are never data (the program may have
    # stopped the stream in the middle of a sector)
    for ins in ["mov   r16, r13", "com   r16", "and   r8, r16", "and   r9, r16"]:
        w(ins)
    if rate31:
        pre = ["movw  ZL, r6", "ld    r16, Z"]
        t0 = T_B - sum(cyc(i) for i in pre)
        out.append("\tWAIT  r19, %-28d ; %4d" % (t0 - t, t)); t = t0
        for ins in pre:
            w(ins)
        assert t == T_B
        w("sts   _SFR_MEM_ADDR(OCR2A), r16")
        w(HOLD[0], 8)
        out.extend(("\t%-38s ;" % i) if not i.endswith(":") else i for i in HOLD[1:])
        w("ldi   r16, hi8(%s)" % END)
        w("cpi   ZL, lo8(%s)" % END); w("cpc   ZH, r16")
        w("brne  .+2", 2); out.append("\t%-38s ;" % "movw  ZL, r14")
        w("movw  r6, ZL")
    out.append("\tWAIT  r19, %-28d ; %4d" % (LINE - t, t))
    return out


FILL = """\
;
; uint16_t wst_fill(uint16_t max)
;
; The blank's share of the stream: bytes from the open read through the state machine, as the
; frame takes them, until WST_FILL_MAX are unread in the ring, max data bytes are stored (give
; or take 16: the song's end), the read stops (an error token), or the next frame is two lines
; away. Interrupts stay on. Returns the data bytes stored (also added to wst.stored); nothing is
; being clocked when it returns, and the SPI is off. Does nothing when wst.off is set.
;
wst_fill:
	lds   r24, wst+WST_O_OFF
	tst   r24
	breq  1f
	clr   r24
	clr   r25
	ret
1:	push  r2
	push  r3
	push  r4
	push  r5
	push  r8
	push  r9
	push  r11
	push  r13
	push  r14
	push  r15
	push  r16
	push  r17
	push  r28
	push  r29
	movw  r2, r24                      ; max
	lds   r4, sound_stream_w
	lds   r5, sound_stream_w+1
	lds   r8, wst+WST_O_K
	lds   r9, wst+WST_O_K+1
	clr   r11
	clr   r13
	ldi   r16, lo8(mix_buf)
	mov   r14, r16
	ldi   r16, hi8(mix_buf)
	mov   r15, r16
	movw  YL, r4                       ; W at the last count
	clr   r22                          ; r23:r22: the data bytes stored
	clr   r23
	ldi   r20, 0xFF
	lds   r18, first_render_line
	lds   r19, render_lines_count
	ldi   ZL, 255
	sub   ZL, r18                      ; hi: two lines before the frame
	ldi   ZH, 253
	sub   ZH, r18
	sub   ZH, r19                      ; lo: the frame's last line
	ldi   r16, (1 << SPE) | (1 << MSTR)
	out   _SFR_IO_ADDR(SPCR), r16
	ldi   r16, (1 << SPI2X)
	out   _SFR_IO_ADDR(SPSR), r16
	out   SPDR_IO, r20                 ; the first byte

w_check:                               ; (at least 17 cycles from the last out to the next in)
	lds   r16, sync_phase
	sbrs  r16, 0
	rjmp  w_room                       ; the vsync's pulses: no frame near
	lds   r16, sync_pulse
	cp    ZH, r16
	brsh  w_room                       ; after the frame
	cp    ZL, r16
	brlo  w_room
	rjmp  w_stop                       ; the frame is two lines away or less
w_room:
	movw  r24, r4                      ; stored since the last count: (W - Y) mod the ring
	sub   r24, YL
	sbc   r25, YH
	brcc  2f
	subi  r24, lo8(-(MIX_BUF_SIZE))
	sbci  r25, hi8(-(MIX_BUF_SIZE))
2:	add   r22, r24
	adc   r23, r25
	movw  YL, r4
	cli                                ; W for the kernel's reads (they stop there)
	sts   sound_stream_w, r4
	sts   sound_stream_w+1, r5
	sei
	tst   r13
	breq  6f
	rjmp  w_stop                       ; an error token: the read has stopped
6:	cp    r22, r2
	cpc   r23, r3
	brlo  7f
	rjmp  w_stop                       ; max stored
7:	cli                               ; unread: (W - mix_pos) mod the ring
	lds   r24, mix_pos
	lds   r25, mix_pos+1
	sei
	movw  r18, r4
	sub   r18, r24
	sbc   r19, r25
	brcc  3f
	subi  r18, lo8(-(MIX_BUF_SIZE))
	sbci  r19, hi8(-(MIX_BUF_SIZE))
3:	cpi   r18, lo8(WST_FILL_MAX)
	ldi   r16, hi8(WST_FILL_MAX)
	cpc   r19, r16
	brlo  8f
	rjmp  w_stop                       ; the ring is full enough
8:	ldi   r19, 16
w_slot:
%(slot)s
	dec   r19
	breq  .+2
	rjmp  w_slot
	rjmp  w_check

w_stop:                                ; a byte may be being clocked: let it in
	ldi   r16, 6
4:	dec   r16
	brne  4b
%(last)s
	ldi   r16, (1 << MSTR)             ; the SPI off: the clock stops, the card stays selected
	out   _SFR_IO_ADDR(SPCR), r16
	movw  r24, r4
	sub   r24, YL
	sbc   r25, YH
	brcc  5f
	subi  r24, lo8(-(MIX_BUF_SIZE))
	sbci  r25, hi8(-(MIX_BUF_SIZE))
5:	add   r22, r24
	adc   r23, r25
	sts   sound_stream_w, r4
	sts   sound_stream_w+1, r5
	sts   wst+WST_O_K, r8
	sts   wst+WST_O_K+1, r9
	sts   wst+WST_O_OFF, r13
	lds   r24, wst+WST_O_STORED
	lds   r25, wst+WST_O_STORED+1
	add   r24, r22
	adc   r25, r23
	sts   wst+WST_O_STORED, r24
	sts   wst+WST_O_STORED+1, r25
	movw  r24, r22
	pop   r29
	pop   r28
	pop   r17
	pop   r16
	pop   r15
	pop   r14
	pop   r13
	pop   r11
	pop   r9
	pop   r8
	pop   r5
	pop   r4
	pop   r3
	pop   r2
	ret
"""

EPILOGUE = """\
wst_end:                                ; (after the last line's hsync_pulse)

	; a sample held (the ring was empty)
	tst   r12
	breq  2f
	sts   sound_stream_held, r12
2:
	; the data bytes stored: (W - W at the start) mod the ring (at most a frame's lines' worth)
	lds   r24, wst+WST_O_W0
	lds   r25, wst+WST_O_W0+1
	movw  r26, r4
	sub   r26, r24
	sbc   r27, r25
	brcc  1f
	subi  r26, lo8(-(MIX_BUF_SIZE))
	sbci  r27, hi8(-(MIX_BUF_SIZE))
1:	lds   r24, wst+WST_O_STORED
	lds   r25, wst+WST_O_STORED+1
	add   r24, r26
	adc   r25, r27
	sts   wst+WST_O_STORED, r24
	sts   wst+WST_O_STORED+1, r25
	sts   sound_stream_w, r4
	sts   sound_stream_w+1, r5
	sts   wst+WST_O_K, r8
	sts   wst+WST_O_K+1, r9

	; the SPI off if the frame had it (nothing is being clocked: each line read its bytes)
	lds   r16, wst+WST_O_OFF
	ldi   r17, (1 << MSTR)
	sbrs  r16, 0
	out   _SFR_IO_ADDR(SPCR), r17
	sts   wst+WST_O_OFF, r13
	lds   r24, wst+WST_O_FRAMES
	lds   r25, wst+WST_O_FRAMES+1
	adiw  r24, 1
	sts   wst+WST_O_FRAMES, r24
	sts   wst+WST_O_FRAMES+1, r25

	; the inline pulses did not count down sync_pulse; hsync_pulse did, once
	lds   r24, sync_pulse
	lds   r25, render_lines_count
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


def emit_groups(groups):
    out = []
    for g in groups:
        for ins in g.ins:
            out.append("\t" + ins if not ins.endswith(":") else ins)
    return out


def build():
    out = []
    w = out.append
    w("; wst_frame.inc -- the text frame streaming a song, and the blank's fill, GENERATED by")
    w("; gen_wst.py; do not edit. Cycles in the comments: from the entry line's start in the")
    w("; prologues, then from the line's start in the line programs.")
    w("")
    w("; after the kernel's .text, so that mode 5's and the kernel's rcalls stay in reach")
    w(".section .text.wst")
    w("")
    w("sub_video_wst:                               ; %4d" % T_ENTRY)
    w("\tlds   r16, sound_stream                ; %4d" % T_ENTRY)
    w("\tsbrc  r16, 0                           ; %4d" % (T_ENTRY + 2))
    w("\trjmp  wst_p31                          ; %4d: 31.5 kHz at %d" % (T_ENTRY + 3, T_ENTRY + 5))
    w("\trjmp  wst_p15                          ; %4d: 15.7 kHz at %d" % (T_ENTRY + 4, T_ENTRY + 6))
    progs = {}
    for rate31, start in ((True, T_ENTRY + 5), (False, T_ENTRY + 6)):
        n = "31" if rate31 else "15"
        w("")
        w("wst_p%s:                                     ; %4d" % (n, start))
        out.extend(prologue(rate31, start))
        w("")
        w("wst_line%s:                                  ; %4d = 0 of the first line" % (n, LINE))
        hd, tl = head(rate31), tail("wst_line" + n)
        for c, ins in hd.lines():
            w("\t%-38s ;%s" % (ins, " %4d" % c if c is not None else "") if not ins.endswith(":")
              else ins)
        w("wst_loop%s:                                  ; %4d + 48k" % (n, T_LOOP))
        for o, ins in loop_lines():
            if ins.startswith("brne"):
                ins = ins % ("wst_loop" + n)
            w("\t%-38s ; %4d + 48k" % (ins, T_LOOP + o))
        for c, ins in tl.lines():
            w("\t%-38s ;%s" % (ins, " %4d" % c if c is not None else ""))
        out.extend(tail_end("wst_line" + n))
        progs[n] = (hd, tl)
    w("")
    w(EPILOGUE)
    # the fill: the same state machine, its out right after its in
    slot = emit_groups([G("in    r17, SPDR_IO"), G(SPI_OUT, 2)] + sm_body(fill=True))
    last = emit_groups([G("in    r17, SPDR_IO")] + sm_body(fill=True))
    w(FILL % {"slot": "\n".join(slot), "last": "\n".join(last)})
    w(".section .text")
    return "\n".join(out) + "\n", progs


def check(progs):
    """Each program over two lines: the pixels, the pulse, the SPI's writes and reads, OCR2A."""
    errs = []
    for n, (hd, tl) in progs.items():
        ev = []
        for base in (0, LINE):
            for tlx in (hd, tl):
                for t, (parts, cycles) in tlx.at.items():
                    if parts == SPI_OUT:
                        ev.append((base + t + 1, "spi_out"))
                        continue
                    tt = t
                    for p in parts:
                        if p.startswith("out   VIDEO_PORT"):
                            ev.append((base + tt, "pix"))
                        elif p.startswith("in    r17, SPDR_IO"):
                            ev.append((base + tt, "spi_in"))
                        elif p.startswith("cbi "):
                            ev.append((base + tt, "cbi"))
                        elif p.startswith("sbi "):
                            ev.append((base + tt, "sbi"))
                        elif p.startswith("sts   _SFR_MEM_ADDR(OCR2A)"):
                            ev.append((base + tt, "ocr"))
                        if len(parts) == 1:
                            break
                        tt += cyc(p) if not p.split()[0].startswith(("br", "sbr")) else 1
            for k in range(TILES):
                o = base + T_LOOP + TILE * k
                for off, ins in loop_lines():
                    if ins.startswith("out   VIDEO_PORT"):
                        ev.append((o + off, "pix"))
                    elif "OCR2A" in ins:
                        ev.append((o + off, "ocr_tile%d" % k))
        ev.sort()
        pix = [t for t, k in ev if k == "pix" and t < LINE]
        if len(pix) != 8 * TILES + 1:
            errs.append("%s: %d DAC outs a line" % (n, len(pix)))
        if pix[0] != T_PIX0:
            errs.append("%s: first pixel at %d" % (n, pix[0]))
        if pix[-1] != T_BLACK:
            errs.append("%s: black at %d" % (n, pix[-1]))
        for a, b in zip(pix, pix[1:]):
            if b - a != 6:
                errs.append("%s: pixels %d, %d" % (n, a, b))
        last_out = None
        n_in = 0
        for t, k in ev:
            if k == "spi_out":
                if last_out is not None and t - last_out < 18:
                    errs.append("%s: SPI writes %d, %d" % (n, last_out, t))
                last_out = t
            elif k == "spi_in":
                if last_out is None or t - last_out < 17:
                    errs.append("%s: SPI read at %d, %s after its write" %
                                (n, t, None if last_out is None else t - last_out))
                n_in += t < LINE
        want = 2 if n == "31" else 1
        if n_in != want:
            errs.append("%s: %d bytes a line" % (n, n_in))
        cbis = [t for t, k in ev if k == "cbi"]
        sbis = [t for t, k in ev if k == "sbi"]
        if cbis != [T_CBI, LINE + T_CBI] or sbis != [T_SBI, LINE + T_SBI]:
            errs.append("%s: pulse: cbi %s, sbi %s" % (n, cbis, sbis))
        # the samples: A at 26; the tiles write r19, which turns to B after tile B_TILE - 1's
        # dec r2 (r2 = B_TILE - 1 at the line's start)
        a = [t for t, k in ev if k == "ocr" and t < LINE]
        if a != [T_A]:
            errs.append("%s: OCR2A from the head at %s" % (n, a))
        r2 = B_TILE - 1
        first_b = None
        r19 = "A"
        for k in range(TILES):
            if r19 == "B" and first_b is None:
                first_b = k
            r2 = (r2 - 1) & 0xFF
            if r2 & 0x80:
                r19 = "B"
        tb = T_LOOP + TILE * first_b + 7
        if first_b != B_TILE or tb != T_B:
            errs.append("%s: B from tile %s (at %d)" % (n, first_b, tb))
        if not (T_A < 910 + 3 <= T_LOOP + TILE * (B_TILE - 1) + 7 + 910 and
                T_LOOP + TILE * (B_TILE - 1) + 7 < 910 + 3 <= T_B):
            errs.append("%s: A's last write and B's first must sit either side of mid-row" % n)
    return errs


def main():
    text, progs = build()
    errs = check(progs)
    for n, (hd, tl) in progs.items():
        print("%s kHz: %d spare cycles in the head, %d in the tail; the state machine %d a byte"
              % ("31.5" if n == "31" else "15.7", T_LOOP - len(hd.busy()),
                 (LINE - T_TAIL) - len(tl.busy()), SM_CYCLES))
    if errs:
        print("TIMING ERRORS:\n  " + "\n  ".join(errs))
        sys.exit(1)
    print("timing checked: pixels 6 apart from %d, black at %d, pulse %d-%d, SPI reads 17 after"
          " their writes, A at %d, B from %d" % (T_PIX0, T_BLACK, T_CBI, T_SBI, T_A, T_B))
    if "--check" not in sys.argv:
        open(OUT, "w", newline="\n").write(text)
        print("wrote %s" % OUT)


if __name__ == "__main__":
    main()
