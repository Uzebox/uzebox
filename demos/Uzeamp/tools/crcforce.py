# Can every 512-byte SD block be made "FE/FF-free" with a CRC16 whose two bytes are FE or FF,
# by choosing 3 fix bytes (each <= 0xFD) in the block? CRC-16/XMODEM (poly 0x1021, init 0), as SD.
import random, time
def crc16(data, crc=0):
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) & 0xFFFF if crc & 0x8000 else (crc << 1) & 0xFFFF
    return crc
TARGETS = {0xFEFE, 0xFEFF, 0xFFFE, 0xFFFF}
FIX = (509, 510, 511)          # fix bytes: the block's last 3
# CRC is linear: crc(block) = crc(block with fix bytes 0) ^ L(fix bytes). Build L's basis.
zero = bytes(512)
basis = {}
for p in FIX:
    for bit in range(8):
        d = bytearray(512); d[p] = 1 << bit
        basis[(p, bit)] = crc16(d)   # crc of zero block is 0, so this is the pure linear part
def lin(vals):
    c = 0
    for p, v in zip(FIX, vals):
        for bit in range(8):
            if v >> bit & 1: c ^= basis[(p, bit)]
    return c
# table: L(b0,b1) for the 2^16 pairs, given b2 -> invert
pair = {}
for b0 in range(256):
    for b1 in range(256):
        pair.setdefault(lin((b0, b1, 0)), (b0, b1))
assert len(pair) == 65536      # (b0,b1) -> CRC contribution is a bijection
random.seed(1)
fails = 0; N = 20000; t = time.time(); tried = []
for i in range(N):
    blk = bytearray(random.randrange(254) for _ in range(512))
    for p in FIX: blk[p] = 0
    base = crc16(blk)
    ok = False
    for b2 in range(254):
        c2 = lin((0, 0, b2))
        for tg in TARGETS:
            b0, b1 = pair[base ^ c2 ^ tg]
            if b0 <= 0xFD and b1 <= 0xFD:
                blk[509], blk[510], blk[511] = b0, b1, b2; ok = True; break
        if ok: tried.append(b2); break
    if not ok: fails += 1; continue
    assert crc16(blk) in TARGETS and max(blk) <= 0xFD
print("blocks", N, "fails", fails, "worst b2 tried", max(tried), "secs", round(time.time()-t, 1))
