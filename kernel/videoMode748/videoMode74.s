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
; Resolution:   192x224 pixels or 384x224 pixels
; Sprites:      Possible by RAM tiles (32 bytes / tile, up to 128)
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
; volatile u8 m74_config;
;
; Global configuration flags
;
; bit 0: Display enabled if set. Otherwise screen is colored by m74_discol.
; bit 1: If set, VRAM row addresses are in RAM, otherwise ROM (64b)
; bit 4: Color 0 reload enabled if set
; bit 5-6: Palette source: 0: RAM, 1: RAM, 2: ROM, 3: SPI RAM
; bit 7: 16th bit of Palette address if it is in SPI RAM
;
; Color 0 reload:
; If enabled, on every scanline, Color 0 of the palette will be reloaded. This
; overrides the color 0 of palette reloads in a separator. The reloads are
; performed by logical scanline position (as set by Row select).
;
.global m74_config

;
; volatile u8 m74_discol;
;
; Color of display when it is disabled. Black by default.
;
.global m74_discol

#if (M74_ROWS_PTRE != 0)
;
; volatile u16 m74_rows;
;
; Row selector address. Records are of 2 bytes in the following layout except
; for the first record which misses the first byte:
;
; - byte 0: Scanline to act on (0 - 223)
; - byte 1: New logical scanline position
;
; The list ends when the scanline can not match any more (either already
; passed or can never be reached).
;
.global m74_rows
#endif

;
; volatile u16 m74_vaddr;
;
; Video RAM addresses (either in ROM or RAM). This points at a 32 byte list of
; RAM addresses pointing to the available Video RAM rows (each row is 8 pixels
; tall).
;
; See "Tile row modes overwiev" in M748_Manual.rst for descriptions on how the
; Video RAM can be filled up to select and configure rows.
;
.global m74_vaddr

;
; volatile u16 m74_paddr;
;
; Address of palette, either in RAM, ROM or SPI RAM depending on bit 5-6 of
; m74_config. The frame's render starts with this palette.
;
.global m74_paddr

#if (M74_RESET_ENABLE != 0)
;
; volatile u16 m74_reset;
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
; volatile u8  m74_m4_bank;
; volatile u16 m74_m4_addr;
;
; SPI RAM base address for Row mode 4. By default it is set up to M74_M4_BASE.
;
.global m74_m4_bank
.global m74_m4_addr

;
; u8 M74_Finish(void);
;
; Always returns zero.
;
.global M74_Finish

;
; void ClearVram(void);
;
; Uzebox kernel function: clears the VRAM. This means clearing Row mode 0's
; VRAM, all other modes are unaffected. If the sprite engine is enabled, use
; M74_VramRestore() instead.
;
.global ClearVram

;
; void SetTile(char x, char y, u16 tileId);
;
; Uzebox kernel function: sets a tile at a given X:Y location. This draws on
; Mode 6 / 7 SPI RAM canvas, setting fake tiles on the 1bpp bitmap.
;
.global SetTile

;
; void SetFont(char x, char y, u8 tileId);
;
; Uzebox kernel function: sets a character at a given X:Y location. This draws
; on Mode 6 / 7 SPI RAM canvas, setting fake tiles on the 1bpp bitmap.
;
.global SetFont

;
; void M74_PrepareM4Row(u8 row, u8 bank, u16 addr);
;
; Prepares Row Mode 4 tile row from SPI RAM source. The SPI RAM source has to
; have 8 * 96 bytes (768 bytes) of data (a 192 pixels wide 4bpp bitmap slice).
; It is assumed that every row displays at its natural position (so display
; beginning at line 0) when determining target in SPI RAM.
;
; Can be used with display enabled, but note that it takes about 20 scanlines!
; (So ideally only call once in a VBlank to prevent it being interrupted)
;
.global M74_PrepareM4Row

;
; void M74_RamTileFillRom(u16 src, u8 dst);
;
; Fills a RAM tile from a ROM tile. Source is a normal address, destination is
; a tile offset (byte offset divided by 32).
;
.global M74_RamTileFillRom

;
; void M74_RamTileFillRam(u8 src, u8 dst);
;
; Fills a RAM tile from a RAM tile. Both source and destination are tile
; offsets (byte offset divided by 32).
;
.global M74_RamTileFillRam

;
; void M74_RamTileClear(u8 dst);
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
	m74_discol:    .space 1 ; Color of display when it is disabled
#if (M74_ROWS_PTRE != 0)
	m74_rows:
	m74_rows_lo:   .space 1 ; Row selector address, low
	m74_rows_hi:   .space 1 ; Row selector address, high
#endif
	m74_vaddr:
	m74_vaddr_lo:  .space 1 ; Video RAM addresses, low
	m74_vaddr_hi:  .space 1 ; Video RAM addresses, high
	m74_paddr:
	m74_paddr_lo:  .space 1 ; Palette source, low
	m74_paddr_hi:  .space 1 ; Palette source, high
#if (M74_RESET_ENABLE != 0)
	m74_reset:
	m74_reset_lo:  .space 1 ; Reset vector, low
	m74_reset_hi:  .space 1 ; Reset vector, high
#endif
	m74_m4_bank:   .space 1 ; Row mode 4 base SPI RAM bank
	m74_m4_addr:            ; Row mode 4 base SPI RAM address
	m74_m4_addr_lo: .space 1
	m74_m4_addr_hi: .space 1

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
#endif



;
; void ClearVram(void);
;
; Uzebox kernel function: clears the VRAM. This means clearing Row mode 0's
; VRAM, all other modes are unaffected. If the sprite engine is enabled, use
; M74_VramRestore() instead.
;
; Clobbered registers:
; r22, r23, r24, r25, XL, XH, ZL, ZH
;
.section .text.ClearVram
ClearVram:

	lds   r23,     m74_config
	lds   ZL,      m74_vaddr + 0
	lds   ZH,      m74_vaddr + 1
	ldi   r24,     32
clvr_l0:
	sbrs  r23,     1       ; m74_config bit 1: M74_RAM_VADDR
	rjmp  .+6
	ld    XL,      Z+
	ld    XH,      Z+      ; VRAM address
	rjmp  .+4
	lpm   XL,      Z+
	lpm   XH,      Z+      ; VRAM address
	ld    r25,     X
	andi  r25,     7
	brne  clvr_l1          ; Not a Mode 0 row, skip it
	adiw  XL,      5       ; VRAM
	ldi   r25,     0
	ldi   r22,     25
clvr_l2:
	st    X+,      r25     ; Clear VRAM row
	dec   r22
	brne  clvr_l2
clvr_l1:
	dec   r24
	brne  clvr_l0
	ret



;
; void SetTile(char x, char y, u16 tileId);
; void SetFont(char x, char y, u8 tileId);
;
; Uzebox kernel function: sets a tile at a given X:Y location. This draws on
; Mode 6 / 7 SPI RAM canvas, setting fake tiles on the 1bpp bitmap.
;
;     r24: x
;     r22: y
; r21:r20: tileId (r21 not set for SetFont)
;
.section .text.SetTileFont
SetFont:
	ldi   r21,     0       ; SetFont takes only 8 bits for the ID.
	subi  r20,     0xE0    ; Also it assumes index 0 corresponding to ASCII 0x20.
SetTile:
	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   r25,     0x03    ; Read
	out   SR_DR,   r25
	ldi   r23,     8
	mul   r20,     r23
	mov   r20,     r0
	mov   r25,     r1      ; ( 5)
	mul   r21,     r23
	mov   r21,     r0
	add   r21,     r25
	ldi   r25,     0
	adc   r25,     r1
	clr   r1               ; (12) r25:r21:r20: Tile address
	subi  r20,     (((0x1000000 - (M74_M67_FONT_OFF))      ) & 0xFF)
	sbci  r21,     (((0x1000000 - (M74_M67_FONT_OFF)) >>  8) & 0xFF)
	sbci  r25,     (((0x1000000 - (M74_M67_FONT_OFF)) >> 16) & 0xFF)
	andi  r25,     0x01    ; (16)
	nop                    ; (17)
	out   SR_DR,   r25     ; SPI RAM: Address high
	cpi   r24,     48      ; ( 1) X position OK? (0 - 47 is valid)
	brcs  .+2
	rjmp  SetTile_notile
	lds   r23,     m74_config
	lds   ZL,      m74_vaddr + 0
	lds   ZH,      m74_vaddr + 1
	clr   r1               ; (10)
	andi  r22,     0x1F    ; (11) 32 rows, wrapping
	add   ZL,      r22
	adc   ZH,      r1
	add   ZL,      r22
	adc   ZH,      r1      ; (15)
	rjmp  .                ; (17)
	out   SR_DR,   r21     ; SPI RAM: Address mid
	sbrs  r23,     1       ; ( 1) m74_config bit 1: M74_RAM_VADDR
	rjmp  .+8
	nop
	ld    XL,      Z+
	ld    XH,      Z+      ; ( 7) VRAM address
	rjmp  .+4              ; ( 9)
	lpm   XL,      Z+
	lpm   XH,      Z+      ; ( 9) VRAM address
	ld    r25,     X
	andi  r25,     6       ; (12) Must be 6 for Row mode 6 or 7
	cpi   r25,     6
	breq  .+2
	rjmp  SetTile_notile
	ld    r25,     X+      ; (17)
	out   SR_DR,   r20     ; SPI RAM: Address low
	ldi   r23,     0x00
	sbrc  r25,     7       ; ( 2) Row mode: SPI RAM A16
	ldi   r23,     0x01
	ld    r20,     X+      ; ( 5) Address low
	ld    r21,     X+      ; ( 7) Address mid
	ldi   r25,     lo8(7 * 48)
	add   r20,     r25
	ldi   r25,     hi8(7 * 48)
	adc   r21,     r25
	adc   r23,     r1      ; (12) Start output on bottom line (due to push-pop load)
	add   r20,     r24
	adc   r21,     r1
	adc   r23,     r1      ; (15)
	rjmp  .                ; (17)
	out   SR_DR,   r25     ; Dummy
	rcall SetTile_w16
	in    r25,     SR_DR   ; Byte 0
	out   SR_DR,   r25
	push  r25
	rcall SetTile_w14
	in    r25,     SR_DR   ; Byte 1
	out   SR_DR,   r25
	push  r25
	rcall SetTile_w14
	in    r25,     SR_DR   ; Byte 2
	out   SR_DR,   r25
	push  r25
	rcall SetTile_w14
	in    r25,     SR_DR   ; Byte 3
	out   SR_DR,   r25
	push  r25
	rcall SetTile_w14
	in    r25,     SR_DR   ; Byte 4
	out   SR_DR,   r25
	push  r25
	rcall SetTile_w14
	in    r25,     SR_DR   ; Byte 5
	out   SR_DR,   r25
	push  r25
	rcall SetTile_w14
	in    r25,     SR_DR   ; Byte 6
	out   SR_DR,   r25
	push  r25
	rcall SetTile_w14
	in    r25,     SR_DR   ; Byte 7
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	push  r25
	ldi   XL,      8
SetTile_olp:
	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   r25,     0x02    ; Write
	out   SR_DR,   r25
	rcall SetTile_w17
	out   SR_DR,   r23     ; SPI RAM: Address high
	rcall SetTile_w17
	out   SR_DR,   r21     ; SPI RAM: Address mid
	rcall SetTile_w17
	out   SR_DR,   r20     ; SPI RAM: Address low
	rcall SetTile_w14
	pop   r25
	subi  r20,     48
	out   SR_DR,   r25     ; SPI RAM: Data
	sbci  r21,     0
	sbci  r23,     0
	dec   XL
	rcall SetTile_w14
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	brne  SetTile_olp
	ret

SetTile_notile:
	rcall SetTile_w14
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	ret

SetTile_w17:
	nop
SetTile_w16:
	rjmp  .
SetTile_w14:
	rjmp  .
	rjmp  .
	rjmp  .
	nop
	ret



;
; void M74_PrepareM4Row(u8 row, u8 bank, u16 addr);
;
; Prepares Row Mode 4 tile row from SPI RAM source. The SPI RAM source has to
; have 8 * 96 bytes (768 bytes) of data (a 192 pixels wide 4bpp bitmap slice).
; It is assumed that every row displays at its natural position (so display
; beginning at line 0) when determining target in SPI RAM.
;
; Can be used with display enabled, but note that it takes about 20 scanlines!
; (So ideally only call once in a VBlank to prevent it being interrupted)
;
;     r24: row
;     r22: bank
; r21:r20: addr
;
.section .text.M74_PrepareM4Row
M74_PrepareM4Row:

	lds   r23,     m74_config
	lds   ZL,      m74_vaddr + 0
	lds   ZH,      m74_vaddr + 1
	clr   r1
	andi  r24,     0x1F    ; 32 rows, wrapping
	add   ZL,      r24
	adc   ZH,      r1
	add   ZL,      r24
	adc   ZH,      r1
	sbrs  r23,     1       ; m74_config bit 1: M74_RAM_VADDR
	rjmp  .+6
	ld    XL,      Z+
	ld    XH,      Z+      ; VRAM address
	rjmp  .+4
	lpm   XL,      Z+
	lpm   XH,      Z+      ; VRAM address
	ld    r25,     X+
	andi  r25,     7
	cpi   r25,     4       ; Row mode 4?
	breq  .+2
	ret                    ; Do nothing if not

	; Copy SPI RAM part first using the row mode's VRAM as buffer

	subi  r20,     0xF4    ; Add 12 to ignore left part for now
	sbci  r21,     0xFF
	sbci  r22,     0xFF
	lds   ZL,      m74_m4_addr_lo
	lds   ZH,      m74_m4_addr_hi
	lds   r19,     m74_m4_bank
	ldi   r23,     84
	lsl   r24
	lsl   r24
	lsl   r24
	mul   r23,     r24     ; Target address
	add   ZL,      r0
	adc   ZH,      r1
	eor   r1,      r1      ; r1 zero without affecting carry
	adc   r19,     r1
	ldi   r23,     8
M74_PrepareM4Row_l0:
	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   r25,     0x03    ; Read
	out   SR_DR,   r25
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   r22     ; SPI RAM: Address high
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   r21     ; SPI RAM: Address mid
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   r20     ; SPI RAM: Address low
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   r20     ; SPI RAM: Dummy
	subi  r20,     (((0x1000000 - 96)      ) & 0xFF)
	sbci  r21,     (((0x1000000 - 96) >>  8) & 0xFF)
	sbci  r22,     (((0x1000000 - 96) >> 16) & 0xFF)
	nop
	ldi   r25,     83
M74_PrepareM4Row_l1:
	rcall M74_PrepareM4Row_w11
	in    r0,      SR_DR
	out   SR_DR,   r0
	st    X+,      r0
	dec   r25
	brne  M74_PrepareM4Row_l1
	nop
	rcall M74_PrepareM4Row_w11
	in    r0,      SR_DR
	st    X+,      r0
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	subi  XL,      84
	sbci  XH,      0
	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   r25,     0x02    ; Write
	out   SR_DR,   r25
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   r19     ; SPI RAM: Address high
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   ZH      ; SPI RAM: Address mid
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   ZL      ; SPI RAM: Address low
	rcall M74_PrepareM4Row_w11
	subi  ZL,      (((0x1000000 - 84)      ) & 0xFF)
	sbci  ZH,      (((0x1000000 - 84) >>  8) & 0xFF)
	sbci  r19,     (((0x1000000 - 84) >> 16) & 0xFF)
	ldi   r25,     84
M74_PrepareM4Row_l2:
	ld    r0,      X+
	out   SR_DR,   r0
	rcall M74_PrepareM4Row_w11
	nop
	dec   r25
	brne  M74_PrepareM4Row_l2
	subi  XL,      84
	sbci  XH,      0
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	dec   r23
	brne  M74_PrepareM4Row_l0

	; Copy VRAM part, populating the left column rendered from it

	subi  r20,     (((96 * 8 + 12)      ) & 0xFF) ; Rewind to beginning
	sbci  r21,     (((96 * 8 + 12) >>  8) & 0xFF)
	sbci  r22,     (((96 * 8 + 12) >> 16) & 0xFF)
	ldi   r23,     8
M74_PrepareM4Row_l3:
	cbi   SR_PORT, SR_PIN  ; Select SPI RAM
	ldi   r25,     0x03    ; Read
	out   SR_DR,   r25
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   r22     ; SPI RAM: Address high
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   r21     ; SPI RAM: Address mid
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   r20     ; SPI RAM: Address low
	rcall M74_PrepareM4Row_w17
	out   SR_DR,   r20     ; SPI RAM: Dummy
	subi  r20,     (((0x1000000 - 96)      ) & 0xFF)
	sbci  r21,     (((0x1000000 - 96) >>  8) & 0xFF)
	sbci  r22,     (((0x1000000 - 96) >> 16) & 0xFF)
	nop
	ldi   r25,     11
M74_PrepareM4Row_l4:
	rcall M74_PrepareM4Row_w11
	in    r0,      SR_DR
	out   SR_DR,   r0
	st    X+,      r0
	dec   r25
	brne  M74_PrepareM4Row_l4
	nop
	rcall M74_PrepareM4Row_w11
	in    r0,      SR_DR
	st    X+,      r0
	sbi   SR_PORT, SR_PIN  ; Deselect SPI RAM
	dec   r23
	brne  M74_PrepareM4Row_l3

	; Done, target filled

	ret

M74_PrepareM4Row_w17:
	rjmp  .
	rjmp  .
	rjmp  .
M74_PrepareM4Row_w11:
	rjmp  .
	rjmp  .
	ret



;
; void M74_RamTileFillRom(u16 src, u8 dst);
;
; Fills a RAM tile from a ROM tile. Source is a normal address, destination is
; a RAM tile index.
;
; r25:r24: src
;     r22: dst
;
.section .text.M74_RamTileFillRom
M74_RamTileFillRom:
	movw  ZL,      r24     ; Source offset in Z
	ldi   XL,      32
	mul   r22,     XL
	movw  XL,      r0
	inc   XH               ; Destination offset generated in X
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
; void M74_RamTileFillRam(u8 src, u8 dst);
;
; Fills a RAM tile from a RAM tile. Both source and destination are RAM tile
; indices.
;
;     r24: src
;     r22: dst
;
.section .text.M74_RamTileFillRam
M74_RamTileFillRam:
	ldi   XL,      32
	mul   r24,     XL
	movw  ZL,      r0
	inc   ZH               ; Source offset generated in Z
	mul   r22,     XL
	movw  XL,      r0
	inc   XH               ; Destination offset generated in X
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
; void M74_RamTileClear(u8 dst);
;
; Clears a RAM tile to color index zero. Destination is a RAM tile index.
;
;     r24: dst
;
.section .text.M74_RamTileClear
M74_RamTileClear:
	ldi   XL,      32
	mul   r24,     XL
	movw  XL,      r0
	inc   XH               ; Destination offset generated in X
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
#if (M74_M67_ENABLE != 0)
#include "videoMode748/videoMode74_m67.s"
#endif
#include "videoMode748/videoMode74_m2.s"
#include "videoMode748/videoMode74_m4.s"
#include "videoMode748/videoMode74_m5.s"
#include "videoMode748/videoMode74_sub.s"
#include "videoMode748/videoMode74_scloop.s"
