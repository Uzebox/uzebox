;
; Uzebox Kernel - Video Mode 748
; Copyright (C) 2017 Sandor Zsuga (Jubatian)
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;
; Uzebox is a reserved trade mark
;
;
;
; ****************************************************************************
; Video Mode 748 Rasterizer and Functions
; ****************************************************************************
;
; Spec
; ----
; Type:         Tile-based (mostly)
; Cycles/Pixel: 7 / 3.5
; Tile Width:   8
; Tile Height:  8
; Resolution:   192x224 pixels or 384x224 pixels in 1bpp SPI RAM mode
; Sprites:      Possible by RAM tiles (32 bytes / tile, up to 64)
; Scrolling:    X and Y scrolling possible
;
; Description
; -----------
;
; See exports
;
; ****************************************************************************
;

;
; Exports
;

;
; volatile unsigned char m74_config;
;
; Global configuration flags
;
; bit 0: Unused
; bit 1: If set, Tile row descriptors are located in RAM, otherwise ROM (32b)
; bit 2: If set, Tile index source list is in RAM, otherwise ROM (64b)
; bit 3: If set, RAM palette source, otherwise ROM (16b)
; bit 4: Color 0 reload enabled if set
; bit 5: Unused
; bit 6: Unused
; bit 7: Display enabled if set. Otherwise screen is black
;
; Color 0 reload:
; If enabled, on every scanline, Color 0 of the palette will be reloaded. This
; overrides the color 0 of palette reloads in a separator. The reloads are
; performed by logical scanline position (as set by Row select).
;
.global m74_config

#if (M74_ROWS_PTRE != 0)
;
; volatile unsigned int m74_rows;
;
; Row selector address. Records are of 3 bytes in the following layout except
; for the first record which misses the first byte:
;
; - byte 0: Scanline to act on (0 - 223)
; - byte 1: New logical scanline position
; - byte 2: New X shift (high 5 bits ignored)
;
; The list ends when the scanline can not match any more (either already
; passed or can never be reached).
;
.global m74_rows
#endif

;
; volatile unsigned int m74_tdesc;
;
; Tile row descriptor address. This points at a 32 byte list of indices which
; address into the tile descriptor tables.
;
; There are two such tables selected by bit 7 of the index: if the bit is 0,
; it selects the ROM table, otherwise the RAM table. The low 7 bits of the
; index are an offset into this table.
;
; Each entry takes 5 bytes using the following layout:
;
; byte 0: bit 0 - 2: Row mode:
;                    0: ROM 4bpp tiles (3 x 64 ROM tiles + 64 RAM tiles)
;                    1: (Same as 0)
;                    2: Special: Separator line
;                    3: (Same as 2)
;                    4: SPI RAM 4bpp bitmap
;                    5: SPI RAM 3bpp bitmap
;                    6: SPI RAM 1bpp bitmap with Attributes (384 px wide)
;                    7: SPI RAM 1bpp bitmap (384 px wide)
;         bit 3 - 7: Flags (used by some modes)
;
; Mode 0:
; byte 1: Offset high for ROM tiles 0x00 - 0x3F
; byte 2: Offset high for ROM tiles 0x40 - 0x7F
; byte 3: Offset high for ROM tiles 0x80 - 0xBF
; byte 4: Offset high for RAM tiles 0xC0 - 0xFF
;
; Mode 2:
; byte 1 is used, see below.
;
; Mode 4:
; No extra byte usage.
;
; Mode 5:
; No extra byte usage.
;
; Mode 6:
; byte 1 is used to specify foreground (high nybble) and background (low
; nybble) colors, selected from the palette.
;
; Mode 7:
; No extra byte usage.
;
; Mode 2 (Separator with Palette reload feature):
;
; This mode displays a simple separator while allowing a reload of the palette
; (useful for split-screen effects).
;
; It uses the high nybble of byte 1 to color the separator line.
;
; The tile index source specifies the offset of the palette, if bit 5 of
; byte 0 is set, it is in RAM, otherwise in ROM. Palette reload can take place
; for line 0, from the given offset, which also affects the color of the
; separator line, or line 7, from given offset + 16, which doesn't affect the
; color of the separator line (taken from the previous palette).
;
; Palette reload can be turned off by setting bit 6 of byte 0. This case the
; seperator is simply a single colored line of the given width.
;
.global m74_tdesc

;
; volatile unsigned int m74_tidx;
;
; Address of ROM / RAM tile index source address list. These specify the RAM
; addresses of VRAM for each tile row (32 x 2 bytes, low byte first).
;
.global m74_tidx

#if (M74_PAL_PTRE != 0)
;
; volatile unsigned int m74_pal;
;
; Address of palette, either in RAM or ROM depending on bit 3 of m74_config.
; The frame's render starts with this palette.
;
.global m74_pal
#endif

#if (M74_ROMMASK_PTRE != 0)
;
; volatile unsigned int m74_romma;
;
; Address of ROM mask pool containing at most 224 masks, 8 bytes each. These
; are used for sprite blitting.
;
.global m74_romma
#endif

#if (M74_RAMMASK_PTRE != 0)
;
; volatile unsigned int m74_ramma;
;
; Address of RAM mask pool containing at most 14 masks, 8 bytes each. These
; are used for sprite blitting.
;
.global m74_ramma
#endif

#if (M74_RESET_ENABLE != 0)
;
; volatile unsigned int m74_reset;
;
; Reset vector where the video frame render will reset upon return with an
; empty stack. It should be a function with void parameters and void return.
; This only happens when the video display is enabled. On init, before
; enabling display, a proper reset vector should be set up, then after enable,
; an empty loop should follow (waiting for the frame to terminate it).
;
.global m74_reset
#endif

;
; volatile unsigned int m74_saddr;
;
; SPI RAM base address. This address is used combined with M74_CFG_SPIRAM_HIGH
; in m74_config for SPI RAM sourced modes.
;
.global m74_saddr

;
; unsigned char M74_Finish(void);
;
; Always returns zero.
;
.global M74_Finish

;
; void ClearVram(void);
;
; Uzebox kernel function: clears the VRAM. Operates on the region set up
; by M74_SetVram().
;
.global ClearVram

;
; void SetTile(char x, char y, unsigned int tileId);
;
; Uzebox kernel function: sets a tile at a given X:Y location on VRAM.
; Operates on the region set up by M74_SetVram().
;
.global SetTile

;
; void SetFont(char x, char y, unsigned char tileId);
;
; Uzebox kernel function: sets a (character) tile at a given X:Y location on
; VRAM. Operates on the region set up by M74_SetVram().
;
.global SetFont

;
; void M74_RamTileFillRom(unsigned int src, unsigned char dst);
;
; Fills a RAM tile from a ROM tile. Source is a normal address, destination is
; a tile offset (byte offset divided by 32).
;
.global M74_RamTileFillRom

;
; void M74_RamTileFillRam(unsigned char src, unsigned char dst);
;
; Fills a RAM tile from a RAM tile. Both source and destination are tile
; offsets (byte offset divided by 32).
;
.global M74_RamTileFillRam

;
; void M74_RamTileClear(unsigned char dst);
;
; Clears a RAM tile to color index zero. Destination is a tile offset (byte
; offset divided by 32).
;
.global M74_RamTileClear

;
; void M74_Halt(void);
;
; Halts program execution. Use with reset (M74_RESET_ENABLE set) to terminate
; components which are supposed to be terminated by a new frame. This is not
; required, but by the C language a function call is necessary to enforce a
; sequence point (so every side effect completes before the call including
; writes to any globals).
;
.global M74_Halt

;
; void M74_Seq(void);
;
; Sequence point. Use with reset (M74_RESET_ENABLE set) to enforce a sequence
; point, so everything is carried out which is before. This is not required,
; but by the C language a function call is necessary to enforce a sequence
; point (so every side effect completes before the call including writes to
; any globals).
;
.global M74_Seq


;
; Video output port, where the pixels go, Stack and SPI RAM stuff
;
#define PIXOUT  VIDEO_PORT
#define STACKH  0x3E
#define STACKL  0x3D
#define SR_PORT _SFR_IO_ADDR(PORTA)
#define SR_PIN  PA4
#define SR_DR   _SFR_IO_ADDR(SPDR)



;
; Replacement WAIT macro for the mode. The routines it calls are in
; videoMode74_sub.s, within rcall range for all components. This macro makes
; overall code size smaller.
;
.macro M74WT_SMR24 clocks
	.if     (\clocks) >= 15
		rcall m74_wait_15
	.elseif (\clocks) == 14
		rcall m74_wait_14
	.elseif (\clocks) == 13
		rcall m74_wait_13
	.elseif (\clocks) == 12
		rcall m74_wait_12
	.elseif (\clocks) == 11
		rcall m74_wait_11
	.elseif (\clocks) == 10
		rcall m74_wait_10
	.elseif (\clocks) == 9
		rcall m74_wait_9
	.elseif (\clocks) == 8
		rcall m74_wait_8
	.elseif (\clocks) == 7
		rcall m74_wait_7
	.elseif (\clocks) == 6
		lpm   r24,     Z
		lpm   r24,     Z
	.elseif (\clocks) == 5
		lpm   r24,     Z
		rjmp  .
	.elseif (\clocks) == 4
		rjmp  .
		rjmp  .
	.elseif (\clocks) == 3
		lpm   r24,     Z
	.elseif (\clocks) == 2
		rjmp  .
	.elseif (\clocks) == 1
		nop
	.else
	.endif
.endm
.macro M74WT_R24   clocks
	.if     (\clocks) > 267
		ldi   r24,     ((\clocks) / 16)
		rcall m74_wait_13
		dec   r24
		brne  .-6
		M74WT_SMR24    ((\clocks) % 16)
	.elseif (\clocks) > 15
		ldi   r24,     ((\clocks) - 12)
		rcall m74_wait
	.else
		M74WT_SMR24    (\clocks)
	.endif
.endm



.section .bss

	; Globals

	m74_config:    .space 1 ; Global configuration
#if (M74_ROWS_PTRE != 0)
	m74_rows:
	m74_rows_lo:   .space 1 ; Row selector address, low
	m74_rows_hi:   .space 1 ; Row selector address, high
#endif
	m74_tdesc:
	m74_tdesc_lo:  .space 1 ; Tile row descriptor address, low
	m74_tdesc_hi:  .space 1 ; Tile row descriptor address, high
	m74_tidx:
	m74_tidx_lo:   .space 1 ; Tile index source address list, low
	m74_tidx_hi:   .space 1 ; Tile index source address list, high
#if (M74_PAL_PTRE != 0)
	m74_pal:
	m74_pal_lo:    .space 1 ; Palette source, low
	m74_pal_hi:    .space 1 ; Palette source, high
#endif
#if (M74_RESET_ENABLE != 0)
	m74_reset:
	m74_reset_lo:  .space 1 ; Reset vector, low
	m74_reset_hi:  .space 1 ; Reset vector, high
#endif
	m74_saddr:
	m74_saddr_lo:  .space 1 ; SPI RAM base address, low
	m74_saddr_hi:  .space 1 ; SPI RAM base address, high

	; Locals

	v_reset_lo:    .space 1 ; Stack restore address, low
	v_reset_hi:    .space 1 ; Stack restore address, high

.section .text



#if (M74_SPR_ENABLE != 0)
;
; Sprite library. Included here to avoid it interfering with relative jumps &
; calls within the Mode 74 core.
;
#include "videoMode748/videoMode74_sprite.s"
#include "videoMode748/videoMode74_vram.s"
#endif



;
; void ClearVram(void);
;
; Uzebox kernel function: clears the VRAM. Operates on the region set up
; by M74_SetVram().
;
.section .text.ClearVram
ClearVram:
	ldi   r18,     lo8(M74_VRAM_P * M74_VRAM_H)
	ldi   r19,     hi8(M74_VRAM_P * M74_VRAM_H)
	movw  r0,      r18     ; Length of VRAM in r1:r0
	ldi   ZL,      lo8(M74_VRAM_OFF)
	ldi   ZH,      hi8(M74_VRAM_OFF)
	clr   r20
	; Clear excess bytes compared to lower multiple of 4
	sbrs  r0,      0
	rjmp  clvr0
	st    Z+,      r20
	dec   r0
clvr0:
	sbrs  r0,      1
	rjmp  clvr1
	st    Z+,      r20
	dec   r0
	st    Z+,      r20
	dec   r0
clvr1:
	; Test for zero
	mov   r18,     r1
	or    r18,     r0
	breq  clvr2
	movw  r24,     r0      ; r25:r24, r1:r0
	; If nonzero, clear remaining area in blocks of four (3cy / byte)
clvr3:
	st    Z+,      r20
	st    Z+,      r20
	st    Z+,      r20
	st    Z+,      r20
	sbiw  r24,     4
	brne  clvr3
clvr2:
	clr   r1
	ret



;
; void SetTile(char x, char y, unsigned int tileId);
; void SetFont(char x, char y, unsigned char tileId);
;
; Uzebox kernel function: sets a tile at a given X:Y location on VRAM.
; Operates on the region set up by M74_SetVram().
;
; The high byte of tileId is not used, so the two functions can be served by
; the same routine.
;
;     r24: x
;     r22: y
; r21:r20: tileId (r21 not used)
;
.section .text
SetTile:
SetFont:
	ldi   r25,     M74_VRAM_P
	mul   r25,     r22
	movw  ZL,      r0
	clr   r1
	add   ZL,      r24
	adc   ZH,      r1
	subi  ZL,      lo8(-(M74_VRAM_OFF))
	sbci  ZH,      hi8(-(M74_VRAM_OFF))
	st    Z,       r20
	ret



;
; void M74_RamTileFillRom(unsigned int src, unsigned char dst);
;
; Fills a RAM tile from a ROM tile. Source is a normal address, destination is
; a tile offset (byte offset divided by 32).
;
; r25:r24: src
;     r22: dst
;
.section .text.M74_RamTileFillRom
M74_RamTileFillRom:
	movw  ZL,      r24     ; Source offset in Z
	ldi   XL,      32
	mul   r22,     XL
	movw  XL,      r0      ; Destination offset generated in X
	clr   r1               ; r1 must be zero for C
	ldi   r22,     8
frtrol:
	lpm   r0,      Z+
	st    X+,      r0
	lpm   r0,      Z+
	st    X+,      r0
	lpm   r0,      Z+
	st    X+,      r0
	lpm   r0,      Z+
	st    X+,      r0
	dec   r22
	brne  frtrol
	ret



;
; void M74_RamTileFillRam(unsigned char src, unsigned char dst);
;
; Fills a RAM tile from a RAM tile. Both source and destination are tile
; offsets (byte offset divided by 32).
;
;     r24: src
;     r22: dst
;
.section .text.M74_RamTileFillRam
M74_RamTileFillRam:
	ldi   XL,      32
	mul   r24,     XL
	movw  ZL,      r0      ; Source offset generated in Z
	mul   r22,     XL
	movw  XL,      r0      ; Destination offset generated in X
	clr   r1               ; r1 must be zero for C
	ldi   r22,     8
frtral:
	ld    r0,      Z+
	st    X+,      r0
	ld    r0,      Z+
	st    X+,      r0
	ld    r0,      Z+
	st    X+,      r0
	ld    r0,      Z+
	st    X+,      r0
	dec   r22
	brne  frtral
	ret



;
; void M74_RamTileClear(unsigned char dst);
;
; Clears a RAM tile to color index zero. Destination is a tile offset (byte
; offset divided by 32).
;
;     r24: dst
;
.section .text.M74_RamTileClear
M74_RamTileClear:
	ldi   XL,      32
	mul   r24,     XL
	movw  XL,      r0      ; Destination offset generated in X
	clr   r1               ; r1 must be zero for C
	ldi   r22,     8
frtcll:
	st    X+,      r1
	st    X+,      r1
	st    X+,      r1
	st    X+,      r1
	dec   r22
	brne  frtcll
	ret



;
; void M74_Halt(void);
;
; Halts program execution. Use with reset (M74_RESET_ENABLE set) to terminate
; components which are supposed to be terminated by a new frame. This is not
; required, but by the C language a function call is necessary to enforce a
; sequence point (so every side effect completes before the call including
; writes to any globals).
;
.section .text.M74_Halt
M74_Halt:
	rjmp  M74_Halt



;
; void M74_Seq(void);
;
; Sequence point. Use with reset (M74_RESET_ENABLE set) to enforce a sequence
; point, so everything is carried out which is before. This is not required,
; but by the C language a function call is necessary to enforce a sequence
; point (so every side effect completes before the call including writes to
; any globals).
;
.section .text.M74_Seq
M74_Seq:
	ret



.section .text


;
; Other components of the mode.
; Note that the order of _scloop, _m2, _spi* and _sub must be preserved and
; they must be last (so add further includes before them, this is required due
; to some hsync_pulse rcalls which the kernel adds after the video mode), to
; ensure that all relative jumps are within range.
;
#include "videoMode748/videoMode74_spi67.s"
#include "videoMode748/videoMode74_spi5.s"
#include "videoMode748/videoMode74_spi4.s"
#include "videoMode748/videoMode74_sub.s"
#include "videoMode748/videoMode74_scloop.s"
#if (M74_M2_ENABLE != 0)
#include "videoMode748/videoMode74_m2.s"
#endif
