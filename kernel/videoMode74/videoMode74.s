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
; unsigned char m74_config;
;
; Global configuration flags
;
; bit 0: Row select mode
;        0: RAM line + restart pairs with X shift overrides
;        1: RAM / ROM scanline map, no X shift overrides (up to 224b)
; bit 1: If set, Tile row descriptors are located in RAM, otherwise ROM (256b)
; bit 2: If set, RAM tile index source address overrides are enabled (64b)
; bit 3: If set, RAM palette source, otherwise ROM (16b)
; bit 4: Color 0 reload enabled if set
; bit 5: If set, RAM color 0 reload source, otherwise ROM (256b)
; bit 6: If set, RAM X shift map is used (256b)
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
; unsigned int m74_rows;
;
; Row selector address. The area is used according to bit 0 of m74_config.
;
.global m74_rows

;
; unsigned int m74_tdesc;
;
; Tile row descriptor address. There are 32 x 8 bytes of tile descriptors
; either in ROM or RAM as requested by bit 1 of m74_config. They are formatted
; as follows:
;
; byte 0: Mode & X shift to left:
;         bit 0 - 2: X left shift count. 0: No left shift.
;         bit 3 - 4: Row width:
;                    0: 24 tiles (192 px) (Color 0 reload not supported)
;                    1: 22 tiles (176 px)
;                    2: 20 tiles (160 px)
;                    3: 18 tiles (144 px)
;         bit 5 - 7: Mode for tiles 0x00 - 0x7F:
;                    0: ROM (0x00 - 0x3F) + RAM (0x40 - 0x7F) 4bpp tiles
;                    1: RAM 8px wide 1bpp tiles
;                    2: ROM 8px wide 1bpp tiles
;                    3: ROM 8px wide 1bpp tiles
;                    4: RAM 2bpp region (2+ tiles wide)
;                    5: ROM 6px wide 1bpp tiles
;                    6: ROM 6px wide 1bpp tiles with Fg color attributes
;                    7: Separator with Palette reload feature
; byte 1: Tiles 0x80 - 0xFF (ROM 4bpp) offset high
; byte 2: Tiles 0x00 - 0x7F offset low
; byte 3: Tiles 0x00 - 0x7F offset high
; byte 4: Tiles 0x00 - 0x7F offset increment per row (0: 256).
; byte 5: Mode specific byte
;         Mode 0: RAM (0x40 - 0x7F) tiles offset high
;         Mode 1,2,3,5,6,7: Fg (High nybble) & Bg (Low nybble) colors
;         Mode 4: Width of output in tiles (must be at least 2).
; byte 6: Tile index source address, low
; byte 7: Tile index source address, high
;
; ROM 4bpp tiles have no offset low setting (in Mode 0, byte 5 only applies to
; RAM tiles). ROM 4bpp tile offset always increments by 512 for rows.
;
; In all modes, the high bits of bytes refer to pixels on the left.
;
; The X left shift count and Tile index source may be overridden as requested
; by the appropriate flags of m74_config.
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
; It uses bytes 1 - 5 to color the individual tiles of the separator, also
; respecting bits 3 - 4 of the mode byte for its width. X left shift is not
; used.
;
; Bytes 6 - 7 specify the offset of the palette, if bit 2 of the mode byte is
; set, it is in RAM, otherwise in ROM. The line within the tile row selects
; the palette to use (16 byte increments), allowing to access 8 palettes.
;
; If bit 1 of the mode byte is set, the separator is colored by the new
; palette, otherwise by the old palette.
;
; The render of the selector is as follows:
; Colors from the bytes (high to low nybble order): 0123456789
; Width = 24 tiles:
; |0|1|2|3|4|5|6|7|8|9|9|9|9|9|9|8|7|6|5|4|3|2|1|0|
; Width = 22 tiles:
; |-|1|2|3|4|5|6|7|8|9|9|9|9|9|9|8|7|6|5|4|3|2|1|-|
; Width = 20 tiles:
; |-|-|2|3|4|5|6|7|8|9|9|9|9|9|9|8|7|6|5|4|3|2|-|-|
; Width = 18 tiles:
; |-|-|-|3|4|5|6|7|8|9|9|9|9|9|9|8|7|6|5|4|3|-|-|-|
;
.global m74_tdesc

;
; unsigned int m74_tidx;
;
; Address of RAM tile index source address overrides. Only used if bit 2 of
; m74_config is set, enabling these overrides (replacing byte 6 and byte 7 of
; the tile row descriptor).
;
.global m74_tidx

;
; unsigned char m74_palbuf;
;
; Address of palette buffer, 256 bytes. This is the high 8 bits of the
; address, the low 8 bits are always zero.
;
.global m74_palbuf

;
; unsigned int m74_pal;
;
; Address of palette, either in RAM or ROM depending on bit 3 of m74_config.
; The frame's render starts with this palette.
;
.global m74_pal

;
; unsigned int m74_col0;
;
; Address of Color 0 reload table, either in RAM or ROM depending on bit 5 of
; m74_config. Only used if it is enabled by bit 4 of m74_config, and the
; displayed row is 22 tiles wide or narrower.
;
.global m74_col0

;
; unsigned int m74_xsh;
;
; Address of the X shift map in RAM. Only used if it is enabled by bit 6 of
; m74_config. It uses logical scanline positions. Low 3 bits specify X shift
; while high 5 bits add to the tile index source address.
;
.global m74_xsh

;
; unsigned char m74_ldsl;
;
; Scanline to start RAM clear or SPI load at. This is the last scanline which
; is not affected by the function. These functions work in the spare cycles
; within graphics output, when the display is narrower than 24 tiles or a
; separator is displayed. RAM clear is performed in 16 byte blocks, SPI load
; is performed in 2 byte blocks.
;
.global m74_ldsl

;
; unsigned int m74_totc;
;
; Total blocks of data to cover by the RAM clear or SPI load function. If
; bit 16 is set, then RAM clear is performed, the low 8 bits specifying the
; number of 16 byte blocks to clear. If bit 16 is clear, then SPI load is
; performed, the value specifying the count of 2 byte blocks to load.
;
.global m74_totc

;
; unsigned int m74_fadd;
;
; Target start address in RAM for SPI load or RAM clear function.
;
.global m74_fadd

;
; unsigned int m74_umod;
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

;
; void M74_SetVram(unsigned int addr, unsigned char wdt, unsigned char hgt);
;
; Sets up a region to use with the Uzebox kernel functions. It doesn't alter
; the video mode itself, it should be set up so it uses this region as tile
; index sources to actually display the area.
;
.global M74_SetVram

;
; unsigned char M74_Finish(void);
;
; Attempts to finish the RAM clear or SPI load started within the video
; display. Returns 0 on success, 1 if it should be called back later to
; continue. The latter can only happen with an SPI load, it allows for the
; caller to do other things while the SD card is busy between 512 byte
; blocks.
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
	m74_palbuf:    .byte 1 ; Palette buffer address, high (low is zero)
	m74_ldsl:      .byte 1 ; Load start scanline (RAM clear / SPI load)
	m74_rows:
	m74_rows_lo:   .byte 1 ; Row selector address, low
	m74_rows_hi:   .byte 1 ; Row selector address, high
	m74_tdesc:
	m74_tdesc_lo:  .byte 1 ; Tile row descriptor address, low
	m74_tdesc_hi:  .byte 1 ; Tile row descriptor address, high
	m74_tidx:
	m74_tidx_lo:   .byte 1 ; Tile index source address overrides, low
	m74_tidx_hi:   .byte 1 ; Tile index source address overrides, high
	m74_pal:
	m74_pal_lo:    .byte 1 ; Palette source, low
	m74_pal_hi:    .byte 1 ; Palette source, high
	m74_col0:
	m74_col0_lo:   .byte 1 ; Color 0 reload address, low
	m74_col0_hi:   .byte 1 ; Color 0 reload address, high
	m74_xsh:
	m74_xsh_lo:    .byte 1 ; X shift map address, low
	m74_xsh_hi:    .byte 1 ; X shift map address, high
	m74_totc:
	m74_totc_lo:   .byte 1 ; Total block count for RAM clear / SPI load
	m74_totc_hi:   .byte 1 ; Total block count for RAM clear / SPI load
	m74_fadd:
	m74_fadd_lo:   .byte 1 ; RAM clear / SPI load target address, low
	m74_fadd_hi:   .byte 1 ; RAM clear / SPI load target address, high
	m74_umod:
	m74_umod_lo:   .byte 1 ; User video mode entry, low
	m74_umod_hi:   .byte 1 ; User video mode entry, high

	; Locals

	v_vram_lo:     .byte 1 ; VRAM location for kernel functions, low
	v_vram_hi:     .byte 1 ; VRAM location for kernel functions, high
	v_vram_w:      .byte 1 ; Width of VRAM for kernel functions
	v_vram_h:      .byte 1 ; Height of VRAM for kernel functions
	v_spis:        .byte 1 ; SPI load state
	v_spibc:       .byte 1 ; SPI remaining 2 byte blocks from an 512b unit
	v_remc_lo:     .byte 1 ; Remaining block count for RAM clear / SPI load
	v_remc_hi:     .byte 1 ; Remaining block count for RAM clear / SPI load
	v_cadd_lo:     .byte 1 ; RAM clear / SPI load current address, low
	v_cadd_hi:     .byte 1 ; RAM clear / SPI load current address, high

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
	ret



;
; unsigned char M74_Finish(void);
;
; Attempts to finish the RAM clear or SPI load started within the video
; display. Returns 0 on success, 1 if it should be called back later to
; continue. The latter can only happen with an SPI load, it allows for the
; caller to do other things while the SD card is busy between 512 byte
; blocks.
;
; Return value is in r25:r24 (r25 set zero).
;
.section .text.M74_Finish
M74_Finish:
	movw  r18,     r6      ; Save r7:r6 into r19:r18 (Calling convention!)
	movw  r20,     r4      ; Save r5:r4 into r21:r20 (Calling convention!)
	clr   r22
	lds   ZL,      v_cadd_lo
	lds   ZH,      v_cadd_hi
	lds   r6,      v_remc_lo
	lds   r7,      v_remc_hi
	clr   r23              ; Succesful (zero) return by default (uses r23 since r24 is clobbered)
	sbrc  r7,      7
	rjmp  lfir             ; Highest bit set: RAM clear
lfislh:
	; SPI load function. Note that the loops are exactly 8 cycles long
	; without the body of m74_spiload_core_nc, achieving the fastest
	; possible handling of the SPI transfer (18 cycles / byte).
	cp    r7,      r22
	breq  lfisll           ; Less than 512 bytes remaining
	rcall m74_spiload_core_nc
	dec   r4               ; r4: 0x01 if needs to wait for next block
	brne  lfislh
	rjmp  lfisb
lfisll:
	cp    r6,      r22
	breq  lfisew           ; All bytes loaded
	rcall m74_spiload_core_nc
	dec   r4               ; r4: 0x01 if needs to wait for next block
	brne  lfisll
	rjmp  .                ; Needed to get 8 cycles between m74_spiload_core_nc bodies
lfisb:
	mov   r25,     5       ; Count of tries until giving up
lfislt:
	rcall m74_spiload_core_nc
	dec   r4
	brne  lfislh           ; If no longer needs waiting, then can go on normally
	dec   r25
	brne  lfislt
	inc   r23              ; Retries exhausted: Unsuccesful return (one)
lfisew:
	sts   v_remc_hi, r7    ; Save current remaining count
	rjmp  lfises           ; (low is saved below)
lfir:
	; RAM clear function
	cp    r6,      r22
	breq  lfise            ; No more RAM blocks to clear
lfirl:
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
	brne  lfirl
lfises:
	sts   v_remc_lo, r6  ; Just clear it so a next call won't do anything.
lfise:
	mov   r24,     r23
	clr   r25
	movw  r4,      r20
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
; The scanline render loop and the frame sub
;
#include "videoMode74/videoMode74_sub.s"
#include "videoMode74/videoMode74_scloop.s"
