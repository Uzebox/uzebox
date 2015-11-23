;
; Uzebox Kernel - Video Mode 74
; Copyright (C) 2015 Sandor Zsuga (Jubatian)
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
; Video Mode 74 Rasterizer and Functions
; ****************************************************************************
;
; Spec
; ----
; Type:         Tile-based (mostly)
; Cycles/Pixel: 7
; Tile Width:   8 (for most modes)
; Tile Height:  8
; Resolution:   192x224 pixels (variable)
; Sprites:      Possible by RAM tiles
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
; bit 0: Row select mode
;        0: RAM line + restart pairs with X scroll
;        1: RAM / ROM scanline map (up to 224b)
; bit 1: If set, Tile row descriptors are located in RAM, otherwise ROM (64b)
; bit 2: If set, Tile index source list is in RAM, otherwise ROM (64b)
; bit 3: If set, RAM palette source, otherwise ROM (16b)
; bit 4: Color 0 reload enabled if set
; bit 5: If set, RAM color 0 reload source, otherwise ROM (256b)
; bit 6: If set, RAM X scroll map is used after the scanline map (+ 256b)
; bit 7: If set, RAM scanline map is used, otherwise ROM
;
; Row select mode 0:
; In the RAM triplets of the following format exist:
; - byte 0: Scanline to act on (0 - 223)
; - byte 1: New logical scanline position
; - byte 2: New X shift (high 5 bits add to tile index source address)
; The first record misses its byte 0 (starting with byte 1). The list ends
; when the scanline can not match any more (either already passed or can never
; be reached).
;
; Color 0 reload:
; If enabled, on every scanline, Color 0 of the palette will be reloaded. This
; overrides the color 0 of palette reloads in a separator (except within the
; separator if it request itself being colored by the new palette). The
; reloads are performed by logical scanline position (as set by Row select).
;
.global m74_config

;
; volatile unsigned int m74_rows;
;
; Row selector address. The area is used according to bit 0 of m74_config.
;
.global m74_rows

;
; volatile unsigned int m74_tdesc;
;
; Tile row descriptor address. There are 32 x 8 bytes of tile descriptors
; either in ROM or RAM as requested by bit 1 of m74_config. They are formatted
; as follows:
;
; byte 0: bit 0 - 2: ROM 4bpp tiles at 0x80 - 0xBF configuration selector
;         bit 3 - 4: Row width:
;                    0: 24 tiles (192 px) (Color 0 reload not supported)
;                    1: 22 tiles (176 px)
;                    2: 20 tiles (160 px)
;                    3: 18 tiles (144 px)
;         bit 5 - 7: Mode for tiles 0x00 - 0x7F:
;                    0: ROM 4bpp tiles
;                    1: RAM 8px wide 1bpp tiles
;                    2: ROM 8px wide 1bpp tiles
;                    3: Special: 2bpp Multicolor (see videoMode74_m3.s)
;                    4: RAM 2bpp region (2+ tiles wide, M74_2BPP_WIDTH)
;                    5: ROM 6px wide 1bpp tiles
;                    6: ROM 6px wide 1bpp tiles with Fg color attributes
;                    7: Special: Separator with Palette reload feature
; byte 1: bit 0 - 1: Tiles at 0x00 - 0x7F configuration selector
;         bit     2: RAM 4bpp tiles at 0xC0 - 0xFF configuration selector
;         bit     3: If set, full logical row is used to generate base offset
;         bit 4 - 7: Foreground color index for 1bpp modes
;
; ROM 4bpp tiles don't use the low byte of their base offset (it is always
; zero). Their row increment should always be set up to 512 bytes (128 for the
; M74_TBANK01_x_INC defines) so their layout is compatible with that of the
; ROM 4bpp tiles at 0x80 - 0xBF.
;
; In all modes, the high bits of bytes refer to pixels on the left.
;
; Modes 5 and 6 (6px wide 1bpp tiles):
;
; These tiles come in blocks of 4, substituting 3 "normal" tiles. Within the
; block tile indices 0x80 - 0xFF will also produce 6px wide 1bpp tiles.
;
; Mode 7 (Separator with Palette reload feature):
;
; This mode displays a simple decorative separator while allowing a reload of
; the palette (useful for split-screen effects).
;
; It uses byte 1 to color the individual tiles of the separator, also
; respecting bits 3 - 4 of byte 0 for its width. It can not be X scrolled.
;
; The tile index source specifies the offset of the palette, if bit 2 of
; byte 0 is set, it is in RAM, otherwise in ROM. The line within the tile row
; selects the palette to use (16 byte increments), allowing to access 8
; palettes.
;
; If bit 1 of the mode byte is set, the separator is colored by the new
; palette, otherwise by the old palette.
;
; The render of the selector is as follows:
; 0: Color index from the high nybble of byte 1
; 1: Color index from the low nybble of byte 1
; -: Black
; Width = 24 tiles:
; |0|1|0|1|0|1|0|1|0|1|0|1|1|0|1|0|1|0|1|0|1|0|1|0|
; Width = 22 tiles:
; |-|1|0|1|0|1|0|1|0|1|0|1|1|0|1|0|1|0|1|0|1|0|1|-|
; Width = 20 tiles:
; |-|-|0|1|0|1|0|1|0|1|0|1|1|0|1|0|1|0|1|0|1|0|-|-|
; Width = 18 tiles:
; |-|-|-|1|0|1|0|1|0|1|0|1|1|0|1|0|1|0|1|0|1|-|-|-|
;
.global m74_tdesc

;
; volatile unsigned int m74_tidx;
;
; Address of ROM / RAM tile index source address list. These specify the RAM
; addresses of VRAM for each tile row (32 x 2 bytes, low byte first).
;
.global m74_tidx

;
; volatile unsigned char m74_bgcol;
;
; The background color used for 1bpp modes. It is a color index into the
; palette on the high nybble (the low nybble is unused).
;
.global m74_bgcol

;
; volatile unsigned int m74_pal;
;
; Address of palette, either in RAM or ROM depending on bit 3 of m74_config.
; The frame's render starts with this palette.
;
.global m74_pal

#if (M74_COL0_RELOAD != 0)
;
; volatile unsigned int m74_col0;
;
; Address of Color 0 reload table, either in RAM or ROM depending on bit 5 of
; m74_config. Only used if it is enabled by bit 4 of m74_config, and the
; displayed row is 22 tiles wide or narrower.
;
.global m74_col0
#endif

;
; volatile unsigned char m74_ldsl;
;
; Scanline to start RAM clear or SPI load at. This is the last scanline which
; is unaffected by the function. These functions work in the spare cycles
; within graphics output, when the display is narrower than 24 tiles, a
; separator is displayed, and some other cases. RAM clear is performed in 16
; byte blocks, SPI load is performed in 2 byte blocks. For a racing with the
; beam style setup, one tile may be utilized for one block (for narrower than
; 24 tile row configurations).
;
.global m74_ldsl

;
; volatile unsigned char m74_totc;
;
; Total blocks of data to cover by the RAM clear or SPI load function. A
; value of zero requests 256 blocks for SPI load, but turns off clearing for
; RAM clear. One block is 2 bytes for SPI load, 16 bytes for RAM clear.
;
.global m74_totc

;
; volatile unsigned char m74_skip;
;
; Blocks (2 bytes) to skip in SPI load before writing to the target area. This
; is useful for loading from an SD card sector to retrieve an arbitrary small
; region. If this is 0xFF, then RAM clear is performed instead. To turn off
; both functions, set this to 0xFF, and m74_totc to 0x00.
;
.global m74_skip

;
; volatile unsigned int m74_fadd;
;
; Target start address in RAM for SPI load or RAM clear function.
;
.global m74_fadd

;
; volatile unsigned int m74_umod;
;
; User video mode entry point as a byte address in the flash. The supplied
; mode replaces Mode 74 entirely if enabled. The design of the user mode
; should follow the general principles of Uzebox video mode design.
;
; 0: All output disabled (blank screen).
; 1: Disabled, Mode 74 is active as normal.
; Lowest bit clear: Entry before palette load. Entry happens in cycle 482.
; Lowest bit set: Entry after palette load. Entry happens in cycle 1261.
;
; Only the high 15 bits are used as actual address.
;
.global m74_umod

#if (M74_M3_ENABLE != 0)
;
; volatile unsigned int m74_mcadd;
;
; 2bpp Multicolor mode framebuffer start address. Used if such mode is
; displayed.
;
.global m74_mcadd
#endif

;
; void M74_SetVram(unsigned int addr, unsigned char wdt, unsigned char hgt);
;
; Sets up a region to use with the Uzebox kernel functions and rectangular
; VRAM based support functions of Mode 74. It doesn't alter the video mode
; itself, it should be set up so it uses this region as tile index sources to
; actually display the area.
;
.global M74_SetVram

;
; void M74_SetVramEx(unsigned int addr, unsigned char wdt, unsigned char hgt, unsigned char pt);
;
; Sets up a region to use with the Uzebox kernel functions and rectangular
; VRAM based support functions of Mode 74. It doesn't alter the video mode
; itself, it should be set up so it uses this region as tile index sources to
; actually display the area. The 'pt' parameter can specify a pitch larger
; than the used width.
;
.global M74_SetVramEx

;
; void M74_Finish(void);
;
; Finishes the RAM clear or SPI load started within the video display.
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



; Video output port, where the pixels go
#define PIXOUT VIDEO_PORT



.section .bss

	; Globals

	m74_config:    .byte 1 ; Global configuration
	m74_bgcol:     .byte 1 ; Background color for 1bpp modes (high nybble)
	m74_ldsl:      .byte 1 ; Load start scanline (RAM clear / SPI load)
	m74_rows:
	m74_rows_lo:   .byte 1 ; Row selector address, low
	m74_rows_hi:   .byte 1 ; Row selector address, high
	m74_tdesc:
	m74_tdesc_lo:  .byte 1 ; Tile row descriptor address, low
	m74_tdesc_hi:  .byte 1 ; Tile row descriptor address, high
	m74_tidx:
	m74_tidx_lo:   .byte 1 ; Tile index source address list, low
	m74_tidx_hi:   .byte 1 ; Tile index source address list, high
	m74_pal:
	m74_pal_lo:    .byte 1 ; Palette source, low
	m74_pal_hi:    .byte 1 ; Palette source, high
#if (M74_COL0_RELOAD != 0)
	m74_col0:
	m74_col0_lo:   .byte 1 ; Color 0 reload address, low
	m74_col0_hi:   .byte 1 ; Color 0 reload address, high
#endif
	m74_totc:      .byte 1 ; Total block count for RAM clear / SPI load
	m74_skip:      .byte 1 ; Blocks to skip count for SPI load
	m74_fadd:
	m74_fadd_lo:   .byte 1 ; RAM clear / SPI load target address, low
	m74_fadd_hi:   .byte 1 ; RAM clear / SPI load target address, high
	m74_umod:
	m74_umod_lo:   .byte 1 ; User video mode entry, low
	m74_umod_hi:   .byte 1 ; User video mode entry, high
#if (M74_M3_ENABLE != 0)
	m74_mcadd:
	m74_mcadd_lo:  .byte 1 ; 2bpp Multicolor framebuffer start, low
	m74_mcadd_hi:  .byte 1 ; 2bpp Multicolor framebuffer start, high
#endif

	; Locals. Note that m3ptr and cadd share RAM locations: This can be
	; done since the latter is only set up after exiting the scanline
	; loop.

	v_vram_lo:     .byte 1 ; VRAM location for rectangular VRAM functions, low
	v_vram_hi:     .byte 1 ; VRAM location for rectangular VRAM functions, high
	v_vram_w:      .byte 1 ; Width of VRAM for rectangular VRAM functions
	v_vram_h:      .byte 1 ; Height of VRAM for rectangular VRAM functions
	v_vram_p:      .byte 1 ; Pitch of VRAM for rectangular VRAM functions
	v_remc:        .byte 1 ; Remaining block count for RAM clear / SPI load
	v_rems:        .byte 1 ; Remaining skip count for SPI load
	v_m3ptr_lo:            ; Current location in multicolor framebuffer, low
	v_cadd_lo:     .byte 1 ; RAM clear / SPI target addr. after scanline loop, low
	v_m3ptr_hi:            ; Current location in multicolor framebuffer, high
	v_cadd_hi:     .byte 1 ; RAM clear / SPI target addr. after scanline loop, high
	v_hsize:       .byte 1 ; Horizontal size on bits 3 and 4

.section .text



;
; void M74_SetVram(unsigned int addr, unsigned char wdt, unsigned char hgt);
;
; Sets up a region to use with the Uzebox kernel functions. It doesn't alter
; the video mode itself, it should be set up so it uses this region as tile
; index sources to actually display the area.
;
; r25:r24: addr
;     r22: wdt
;     r20: hgt
;
.section .text.M74_SetVram
M74_SetVram:
	sts   v_vram_hi, r25
	sts   v_vram_lo, r24
	sts   v_vram_w, r22
	sts   v_vram_h, r20
	sts   v_vram_p, r22
	ret



;
; void M74_SetVramEx(unsigned int addr, unsigned char wdt, unsigned char hgt, unsigned char pt);
;
; Sets up a region to use with the Uzebox kernel functions and rectangular
; VRAM based support functions of Mode 74. It doesn't alter the video mode
; itself, it should be set up so it uses this region as tile index sources to
; actually display the area. The 'pt' parameter can specify a pitch larger
; than the used width.
;
; r25:r24: addr
;     r22: wdt
;     r20: hgt
;     r18: pt
;
.section .text.M74_SetVramEx
M74_SetVramEx:
	sts   v_vram_hi, r25
	sts   v_vram_lo, r24
	sts   v_vram_w, r22
	sts   v_vram_h, r20
	sts   v_vram_p, r18
	ret



;
; unsigned char M74_Finish(void);
;
; Finishes the RAM clear or SPI load started within the video display.
;
.section .text.M74_Finish
M74_Finish:
	movw  r18,     r6      ; Save r7:r6 into r19:r18 (Calling convention!)
	clr   r22
	lds   ZL,      v_cadd_lo
	lds   ZH,      v_cadd_hi
	lds   r6,      v_remc
	lds   r7,      v_rems
	inc   r7
	brne  lfispi           ; SPI load is v_rems is not 0xFF
	cp    r6,      r22
	breq  lfise            ; No more blocks to clear
lfiral:
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	st    Z+,      r22
	dec   r6
	brne  lfiral
	rjmp  lfise6
lfispi:
	rcall m74_spiload_core_nc
	brcs  lfise7           ; Carry set: load ended
	rjmp  .                ; Pad to 36 cycles for proper SPI operation
	rjmp  .
	rjmp  lfispi
lfise7:
	sts   v_rems,  r7      ; Just store so a new call will do nothing
lfise6:
	sts   v_remc,  r6      ; Just store so a new call will do nothing
lfise:
	movw  r6,      r18
	ret



;
; void ClearVram(void);
;
; Uzebox kernel function: clears the VRAM. Operates on the region set up
; by M74_SetVram().
;
.section .text.ClearVram
ClearVram:
	lds   r18,     v_vram_w
	lds   r19,     v_vram_h
	mul   r18,     r19     ; Length of VRAM in r1:r0
	lds   ZL,      v_vram_lo
	lds   ZH,      v_vram_hi
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
	lds   r25,     v_vram_w
	mul   r25,     r22
	add   r0,      r24
	brcc  sttl0
	inc   r1               ; Carry over
sttl0:
	lds   ZL,      v_vram_lo
	lds   ZH,      v_vram_hi
	add   ZL,      r0
	adc   ZH,      r1
	st    Z,       r20
	clr   r1
	ret



.section .text


;
; Other components of the mode.
; Note that the order of _scloop, _m7, _m3 and _sub must be preserved and they
; must be last (so add further includes before them, this is required due to
; some hsync_pulse rcalls which the kernel adds after the video mode), to
; ensure that all relative jumps are within range.
;
#include "videoMode74/videoMode74_scloop.s"
#if (M74_M7_ENABLE != 0)
#include "videoMode74/videoMode74_m7.s"
#endif
#include "videoMode74/videoMode74_sub.s"
#if (M74_M3_ENABLE != 0)
#include "videoMode74/videoMode74_m3.s"
#endif
