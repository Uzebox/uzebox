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
; bit 0: Row mode 3 double scanning enabled if set.
; bit 1: If set, Tile row descriptors are located in RAM, otherwise ROM (64b)
; bit 2: If set, Tile index source list is in RAM, otherwise ROM (64b)
; bit 3: If set, RAM palette source, otherwise ROM (16b)
; bit 4: Color 0 reload enabled if set
; bit 5: If set, RAM color 0 reload source, otherwise ROM (256b)
; bit 6: SD load enabled if set. This is cleared in every frame.
; bit 7: Display enabled if set. Otherwise screen is black.
;
; Color 0 reload:
; If enabled, on every scanline, Color 0 of the palette will be reloaded. This
; overrides the color 0 of palette reloads in a separator (except within the
; separator if it request itself being colored by the new palette). The
; reloads are performed by logical scanline position (as set by Row select).
; Color 0 reload only works at 22 tiles or less width.
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
; In all modes, the high bits of bytes refer to pixels on the left.
;
; Modes 5 and 6 (6px wide 1bpp tiles):
;
; These tiles come in blocks of 4, substituting 3 "normal" tiles. Within the
; block tile indices 0x80 - 0xFF will also produce 6px wide 1bpp tiles.
;
; Mode 7 (Separator with Palette reload feature):
;
; This mode displays a simple separator while allowing a reload of the palette
; (useful for split-screen effects).
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

#if (M74_PAL_PTRE != 0)
;
; volatile unsigned int m74_pal;
;
; Address of palette, either in RAM or ROM depending on bit 3 of m74_config.
; The frame's render starts with this palette.
;
.global m74_pal
#endif

#if ((M74_COL0_PTRE != 0) && (M74_COL0_RELOAD != 0))
;
; volatile unsigned int m74_col0;
;
; Address of Color 0 reload table, either in RAM or ROM depending on bit 5 of
; m74_config. Only used if it is enabled by bit 4 of m74_config, and the
; displayed row is 22 tiles wide or narrower.
;
.global m74_col0
#endif

#if ((M74_M3_PTRE != 0) && (M74_M3_ENABLE != 0))
;
; volatile unsigned int m74_mcadd;
;
; 2bpp Multicolor mode framebuffer start address. Used if such mode is
; displayed.
;
.global m74_mcadd
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

#if (M74_SD_ENABLE != 0)
;
; volatile unsigned long m74_sdsec;
;
; SD card loading: Base sector address. The byte offset specified is relative
; to this. Initially it is zero, it should be set up to the start of the
; game's data to make it useful.
;
.global m74_sdsec

;
; volatile unsigned long m74_sdoff;
;
; SD card loading: Offset (32 bits; in bytes) to load data from. Lowest bit is
; ignored (only even offsets). Loading can not pass 512 byte sector boundary.
; It is overwritten during the load.
;
.global m74_sdoff

;
; volatile unsigned char m74_sdcnt;
;
; SD card loading: Count of 2 byte blocks to load from the SD card. A value of
; zero requests 512 bytes (a full sector). Loading is truncated to next sector
; boundary.
;
.global m74_sdcnt

;
; volatile unsigned int m74_sddst;
;
; SD card loading: Destination offset in RAM for the loaded data.
;
.global m74_sddst
#endif

#if (M74_VRAM_CONST == 0)
;
; void M74_SetVram(unsigned int addr, unsigned char wdt, unsigned char hgt);
;
; Sets up a region to use with the Uzebox kernel functions and rectangular
; VRAM based support functions of Mode 74. It doesn't alter the video mode
; itself, it should be set up so it uses this region as tile index sources to
; actually display the area.
;
.global M74_SetVram
#endif

#if (M74_VRAM_CONST == 0)
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
#endif

;
; unsigned char M74_Finish(void);
;
; Finishes the SD load started within the video display. Returns nonzero if
; the load failed.
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
; void M74_RamTileFillRom(unsigned int src, unsigned char dst, unsigned char map);
;
; Fills a RAM tile from an arbitrarily located ROM 32 byte source. The 'map'
; parameter specifies the RAM tile map configuration to use (0 or 1).
;
.global M74_RamTileFillRom

;
; void M74_RamTileFillRam(unsigned int src, unsigned char dst, unsigned char map);
;
; Fills a RAM tile from an arbitrarily located RAM 32 byte source. The 'map'
; parameter specifies the RAM tile map configuration to use (0 or 1).
;
.global M74_RamTileFillRam

;
; void M74_RamTileClear(unsigned char dst, unsigned char map);
;
; Clears a RAM tile to color index zero. The 'map' parameter specifies the RAM
; tile map configuration to use (0 or 1).
;
.global M74_RamTileClear



;
; Video output port, where the pixels go, and Stack
;
#define PIXOUT VIDEO_PORT
#define STACKH 0x3E
#define STACKL 0x3D



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

	m74_config:    .byte 1 ; Global configuration
	m74_bgcol:     .byte 1 ; Background color for 1bpp modes (high nybble)
#if (M74_ROWS_PTRE != 0)
	m74_rows:
	m74_rows_lo:   .byte 1 ; Row selector address, low
	m74_rows_hi:   .byte 1 ; Row selector address, high
#endif
	m74_tdesc:
	m74_tdesc_lo:  .byte 1 ; Tile row descriptor address, low
	m74_tdesc_hi:  .byte 1 ; Tile row descriptor address, high
	m74_tidx:
	m74_tidx_lo:   .byte 1 ; Tile index source address list, low
	m74_tidx_hi:   .byte 1 ; Tile index source address list, high
#if (M74_PAL_PTRE != 0)
	m74_pal:
	m74_pal_lo:    .byte 1 ; Palette source, low
	m74_pal_hi:    .byte 1 ; Palette source, high
#endif
#if ((M74_COL0_PTRE != 0) && (M74_COL0_RELOAD != 0))
	m74_col0:
	m74_col0_lo:   .byte 1 ; Color 0 reload address, low
	m74_col0_hi:   .byte 1 ; Color 0 reload address, high
#endif
#if (M74_ROMMASK_PTRE != 0)
	m74_romma:
	m74_romma_lo:  .byte 1 ; ROM mask pool address, low
	m74_romma_hi:  .byte 1 ; ROM mask pool address, high
#endif
#if (M74_RAMMASK_PTRE != 0)
	m74_ramma:
	m74_ramma_lo:  .byte 1 ; RAM mask pool address, low
	m74_ramma_hi:  .byte 1 ; RAM mask pool address, high
#endif
#if ((M74_M3_PTRE != 0) && (M74_M3_ENABLE != 0))
	m74_mcadd:
	m74_mcadd_lo:  .byte 1 ; 2bpp Multicolor framebuffer start, low
	m74_mcadd_hi:  .byte 1 ; 2bpp Multicolor framebuffer start, high
#endif
#if (M74_RESET_ENABLE != 0)
	m74_reset:
	m74_reset_lo:  .byte 1 ; Reset vector, low
	m74_reset_hi:  .byte 1 ; Reset vector, high
#endif
#if (M74_SD_ENABLE != 0)
	m74_sdsec:             ; SD load base sector address (4 bytes)
	m74_sdsec_0:   .byte 1
	m74_sdsec_1:   .byte 1
	m74_sdsec_2:   .byte 1
	m74_sdsec_3:   .byte 1
	m74_sdoff:             ; SD load byte offset to load from (4 bytes, even)
	m74_sdoff_0:
	v_sstat:       .byte 1 ; SD load status
	m74_sdoff_1:
	v_sreme:       .byte 1 ; SD load remaining 2 byte blocks until sector end
	m74_sdoff_2:
	v_srems:       .byte 1 ; SD load remaining 2 byte blocks to skip
	m74_sdoff_3:   .byte 1
	m74_sdcnt:             ; SD load count of 2 byte blocks to load (0: 256)
	v_sremc:       .byte 1 ; SD load remaining 2 byte blocks to load
	m74_sddst:             ; SD load destination address (2 bytes)
	m74_sddst_lo:  .byte 1
	m74_sddst_hi:  .byte 1
#endif

	; Locals

#if (M74_VRAM_CONST == 0)
	v_vram_lo:     .byte 1 ; VRAM location for rectangular VRAM functions, low
	v_vram_hi:     .byte 1 ; VRAM location for rectangular VRAM functions, high
	v_vram_w:      .byte 1 ; Width of VRAM for rectangular VRAM functions
	v_vram_h:      .byte 1 ; Height of VRAM for rectangular VRAM functions
	v_vram_p:      .byte 1 ; Pitch of VRAM for rectangular VRAM functions
#endif
	v_hsize:       .byte 1 ; Horizontal size on bits 3 and 4
	v_rows_lo:     .byte 1 ; Row selector current address, low
	v_rows_hi:     .byte 1 ; Row selector current address, high
#if (M74_M3_ENABLE != 0)
	v_m3ptr_lo:    .byte 1 ; Current location in multicolor framebuffer, low
	v_m3ptr_hi:    .byte 1 ; Current location in multicolor framebuffer, high
#endif

.section .text



;
; Sprite library. Included here to avoid it interfering with relative jumps &
; calls within the Mode 74 core.
;
#include "videoMode74/videoMode74_sprite.s"



#if (M74_VRAM_CONST == 0)
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
#endif



#if (M74_VRAM_CONST == 0)
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
#endif



;
; void ClearVram(void);
;
; Uzebox kernel function: clears the VRAM. Operates on the region set up
; by M74_SetVram().
;
.section .text.ClearVram
ClearVram:
#if (M74_VRAM_CONST == 0)
	lds   r18,     v_vram_p
	lds   r19,     v_vram_h
#else
	ldi   r18,     M74_VRAM_P
	ldi   r19,     M74_VRAM_H
#endif
	mul   r18,     r19     ; Length of VRAM in r1:r0
#if (M74_VRAM_CONST == 0)
	lds   ZL,      v_vram_lo
	lds   ZH,      v_vram_hi
#else
	ldi   ZL,      lo8(M74_VRAM_OFF)
	ldi   ZH,      hi8(M74_VRAM_OFF)
#endif
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
#if (M74_VRAM_CONST == 0)
	lds   r25,     v_vram_p
#else
	ldi   r25,     M74_VRAM_P
#endif
	mul   r25,     r22
	add   r0,      r24
	brcc  sttl0
	inc   r1               ; Carry over
sttl0:
#if (M74_VRAM_CONST == 0)
	lds   ZL,      v_vram_lo
	lds   ZH,      v_vram_hi
#else
	ldi   ZL,      lo8(M74_VRAM_OFF)
	ldi   ZH,      hi8(M74_VRAM_OFF)
#endif
	add   ZL,      r0
	adc   ZH,      r1
	st    Z,       r20
	clr   r1
	ret



;
; void M74_RamTileFillRom(unsigned int src, unsigned char dst, unsigned char map);
;
; Fills a RAM tile from an arbitrarily located ROM 32 byte source. The 'map'
; parameter specifies the RAM tile map configuration to use (0 or 1).
;
; r1 must be zero (stands if called from C)
;
; r25:r24: src
;     r22: dst
;     r20: map
;
.section .text.M74_RamTileFillRom
M74_RamTileFillRom:
	ldi   XL,      lo8(M74_TBANK3_0_OFF)
	sbrc  r20,     0
	ldi   XL,      lo8(M74_TBANK3_1_OFF)
	ldi   XH,      hi8(M74_TBANK3_0_OFF)
	sbrc  r20,     0
	ldi   XH,      hi8(M74_TBANK3_1_OFF)
	ldi   r21,     ((M74_TBANK3_0_INC << 2) - 4)
	sbrc  r20,     0
	ldi   r21,     ((M74_TBANK3_1_INC << 2) - 4)
	lsl   r22
	lsl   r22
	add   XL,      r22
	adc   XH,      r1      ; r1 is zero
	movw  ZL,      r24
	ldi   r20,     8
frtrol:
	lpm   r0,      Z+
	st    X+,      r0
	lpm   r0,      Z+
	st    X+,      r0
	lpm   r0,      Z+
	st    X+,      r0
	lpm   r0,      Z+
	st    X+,      r0
	add   XL,      r21
	adc   XH,      r1      ; r1 is zero
	dec   r20
	brne  frtrol
	ret



;
; void M74_RamTileFillRam(unsigned int src, unsigned char dst, unsigned char map);
;
; Fills a RAM tile from an arbitrarily located RAM 32 byte source. The 'map'
; parameter specifies the RAM tile map configuration to use (0 or 1).
;
; r1 must be zero (stands if called from C)
;
; r25:r24: src
;     r22: dst
;     r20: map
;
.section .text.M74_RamTileFillRam
M74_RamTileFillRam:
	ldi   XL,      lo8(M74_TBANK3_0_OFF)
	sbrc  r20,     0
	ldi   XL,      lo8(M74_TBANK3_1_OFF)
	ldi   XH,      hi8(M74_TBANK3_0_OFF)
	sbrc  r20,     0
	ldi   XH,      hi8(M74_TBANK3_1_OFF)
	ldi   r21,     ((M74_TBANK3_0_INC << 2) - 4)
	sbrc  r20,     0
	ldi   r21,     ((M74_TBANK3_1_INC << 2) - 4)
	lsl   r22
	lsl   r22
	add   XL,      r22
	adc   XH,      r1      ; r1 is zero
	movw  ZL,      r24
	ldi   r20,     8
frtral:
	ld    r0,      Z+
	st    X+,      r0
	ld    r0,      Z+
	st    X+,      r0
	ld    r0,      Z+
	st    X+,      r0
	ld    r0,      Z+
	st    X+,      r0
	add   XL,      r21
	adc   XH,      r1      ; r1 is zero
	dec   r20
	brne  frtral
	ret



;
; void M74_RamTileClear(unsigned char dst, unsigned char map);
;
; Clears a RAM tile to color index zero. The 'map' parameter specifies the RAM
; tile map configuration to use (0 or 1).
;
; r1 must be zero (stands if called from C)
;
;     r24: dst
;     r22: map
;
.section .text.M74_RamTileClear
M74_RamTileClear:
	ldi   XL,      lo8(M74_TBANK3_0_OFF)
	sbrc  r22,     0
	ldi   XL,      lo8(M74_TBANK3_1_OFF)
	ldi   XH,      hi8(M74_TBANK3_0_OFF)
	sbrc  r22,     0
	ldi   XH,      hi8(M74_TBANK3_1_OFF)
	ldi   r21,     ((M74_TBANK3_0_INC << 2) - 4)
	sbrc  r22,     0
	ldi   r21,     ((M74_TBANK3_1_INC << 2) - 4)
	lsl   r24
	lsl   r24
	add   XL,      r24
	adc   XH,      r1      ; r1 is zero
	ldi   r20,     8
frtcll:
	st    X+,      r1
	st    X+,      r1
	st    X+,      r1
	st    X+,      r1
	add   XL,      r21
	adc   XH,      r1      ; r1 is zero
	dec   r20
	brne  frtcll
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
