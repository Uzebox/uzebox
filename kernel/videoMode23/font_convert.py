#!/usr/bin/env python3
import re
import sys

def parse_hex_bytes(text: str):
	# grabs 0xNN tokens
	vals = [int(x, 16) for x in re.findall(r'0x[0-9A-Fa-f]{2}', text)]
	return vals

def convert_font5x7_to_font6x8_h(font5x7):
	if len(font5x7) % 5 != 0:
		raise SystemExit(f"font length {len(font5x7)} not multiple of 5")
	glyphs = len(font5x7) // 5
	if glyphs != 256:
		print(f"warning: glyph count = {glyphs} (expected 256)", file=sys.stderr)

	out = [0] * (glyphs * 8)

	for gi in range(glyphs):
		cols = font5x7[gi*5:gi*5+5]  # 5 column bytes, bit0=top row
		for y in range(8):
			row = 0
			for x in range(5):
				if (cols[x] >> y) & 1:
					row |= (0x80 >> x)  # bit7 is x=0
			# x=5 spacing column => bit2 left 0
			out[gi*8 + y] = row

	return out, glyphs

def emit_asm_bytes(label, data, glyphs, out_path):
	with open(out_path, "w") as f:
		f.write("/* Auto-generated from Arduboy2 font5x7[] */\n")
		f.write(".section .progmem.data\n")
		f.write(f"{label}:\n")

		# 8 bytes per line = 1 glyph per line
		for i in range(0, len(data), 8):
			chunk = data[i:i+8]
			f.write("\t.byte " + ", ".join(f"0x{b:02X}" for b in chunk) + "\n")

	print(f"Wrote {out_path} ({len(data)} bytes, {glyphs} glyphs)")

def main():
	if len(sys.argv) != 3:
		print(f"usage: {sys.argv[0]} <arduboy_font5x7.h> <out_font6x8_h.c>", file=sys.stderr)
		raise SystemExit(1)

	src_path = sys.argv[1]
	out_path = sys.argv[2]

	text = open(src_path, "r", encoding="utf-8").read()
	font5x7 = parse_hex_bytes(text)

	data, glyphs = convert_font5x7_to_font6x8_h(font5x7)
	emit_asm_bytes("font6x8_h", data, glyphs, out_path)

if __name__ == "__main__":
	main()