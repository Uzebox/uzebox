/*
 *  Uzebox Kernel - Mode 3
 *  Copyright (C) 2009 Alec Bourque
 *                2017 Sandor Zsuga (Jubatian)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

;***************************************************
; Video Mode 3: 28x28 (224x224 pixels) using 8x8 tiles
; with overlay & sprites X flipping.
;
; If the SCROLLING build parameter=0, the scrolling 
; code is removed and the screen resolution 
; increases up to 30*28 tiles.
; 
; For compile time switch and information relating
; to this video mode see:
; http://uzebox.org/wiki/index.php?title=Video_Mode_3
;
;***************************************************	

.global TIMER1_OVF_vect
.global vram
.global ram_tiles
#if (RTLIST_ENABLE != 0)
.global ram_tiles_restore
#endif
.global free_tile_index
.global user_ram_tiles_c
.global user_ram_tiles_c_tmp
#if (SPRITES_AUTO_PROCESS != 0)
.global sprites
#endif
.global overlay_vram
.global sprites_tile_banks
.global Screen
.global SetSpritesTileTable
.global CopyFlashTile
.global CopyRamTile
.global SetSpritesTileBank
.global SetTile
.global ClearVram
.global SetFontTilesIndex
.global SetTileTable
.global SetTile
.global RestoreBackground
.global BlitSpritePart
.global SetFont
.global GetTile

;Screen Sections Struct offsets
#define scrollX				0
#define scrollY				1
#define sectionHeight		2
#define vramBaseAdressLo	3
#define vramBaseAdressHi	4
#define tileTableAdressLo	5
#define tileTableAdressHi	6
#define wrapLine			7
#define flags				8
#define scrollXcoarse		9
#define scrollXfine			10		
#define vramRenderAdressLo	11
#define vramRenderAdressHi	12
#define vramWrapAdressLo	13
#define vramWrapAdressHi	14

#define PIXOUT _SFR_IO_ADDR(DATA_PORT)


#if ((SCROLLING != 0) || (RT_ALIGNED != 0))
.section .noinit
#else
.section .bss
#endif
#if (SCROLLING != 0)
.balign 256
#else
.balign 32
#endif

;
; VRAM MUST be aligned to 32 bytes for no scrolling and 256 with scrolling.
; To align vram to a 32 / 256 byte boundary without wasting ram, add the
; following to your makefile's linker section and adjust the .data section
; start to make room for the vram size (including the overlay ram). By
; default the vram is 32x32 so 1k is required.
;
; LDFLAGS += -Wl,--section-start,.noinit=0x800100 -Wl,--section-start,.data=0x800500
;
; If you are using aligned ramtiles (RT_ALIGNED is set nonzero), you also need
; to calculate the data start including their size aligned at the nearest 64
; byte boundary after the top of the VRAM + OverlayVRAM (this only matters for
; the non-scrolling Mode 3 at odd heights).
;
; Example: If you have 32 aligned RAM tiles with scrolling (1K VRAM) Mode 3:
;
; LDFLAGS += -Wl,--section-start,.noinit=0x800100 -Wl,--section-start,.data=0x800D00
;
; Note: A possible linker bug or misunderstood feature exists: the linker for
; some reason reports the size of the section padded to the largest alignment
; used in it. So in the case of the scrolling Mode 3, you will get the .noinit
; section's size padded to the next 256 byte boundary. Keep this in mind when
; using a RAM tile count not being a multiple of 4.
;

vram:
	.space VRAM_SIZE

overlay_vram:
#if ((SCROLLING == 0) && (OVERLAY_LINES != 0))
	.space VRAM_TILES_H * OVERLAY_LINES
#endif

#if (RT_ALIGNED != 0)
.balign 64
ram_tiles:
	.space RAM_TILES_COUNT * TILE_HEIGHT * TILE_WIDTH
#endif

.section .bss
.balign 1

#if (SPRITES_AUTO_PROCESS != 0)
sprites:
	.space SPRITE_STRUCT_SIZE * MAX_SPRITES
#endif
#if (RT_ALIGNED == 0)
ram_tiles:
	.space RAM_TILES_COUNT * TILE_HEIGHT * TILE_WIDTH
#endif
#if (RTLIST_ENABLE != 0)
ram_tiles_restore:
	.space RAM_TILES_COUNT * 3 ; 2 bytes VRAM addr; 1 byte Tile
#endif
free_tile_index:
	.space 1               ; Next free tile index
user_ram_tiles_c:
	.space 1               ; User RAM tiles count
user_ram_tiles_c_tmp:
	.space 1               ; User RAM tiles count, user supplied value

sprites_tile_banks:
	.space 8
tile_table_lo:
	.space 1
tile_table_hi:
	.space 1
font_tile_index:
	.space 1


	;ScreenType struct members
	Screen:
		overlay_height:			.space 1
		overlay_tile_table:		.space 2
	#if SCROLLING == 1
		screen_scrollX:			.space 1
		screen_scrollY:			.space 1
		screen_scrollHeight:	.space 1
	#endif

.section .text





;***************************************************
; Mode 3 Frame driver
;***************************************************

sub_video_mode3:

	; Wait cycles to align with next hsync

	WAIT  r16,     395


#if ((RTLIST_ENABLE != 0) && (SPRITES_VSYNC_PROCESS != 0))

	; Refresh ramtiles indexes in VRAM. This has to be done because the
	; main program may have altered the VRAM after vsync and the rendering
	; interrupt.

	lds   r16,     user_ram_tiles_c

	ldi   ZL,      lo8(ram_tiles_restore)
	ldi   ZH,      hi8(ram_tiles_restore)
	ldi   r18,     3
	mul   r16,     r18
	add   ZL,      r0
	adc   ZH,      r1      ; Skip user RAM tiles

	lds   r18,     free_tile_index
	ldi   r19,     MAX_RAMTILES ; Maximum possible ramtiles
	sub   r19,     r18     ; Subtract free tiles
	add   r19,     r16     ; Add user tiles

	cp    r18,     r16
	breq  no_ramtiles
	nop
	nop
upd_loop:
	ld    XL,      Z+      ; Load vram offset of ramtile
	ld    XH,      Z+

	ld    r17,     X       ; Get latest VRAM tile that may have been modified my
	st    Z+,      r17     ; The main program and store it in the restore buffer
	st    X,       r16     ; Write the ramtile index back to vram

	inc   r16
	cp    r16,     r18
	brlo  upd_loop         ; Loop is 14 cycles

no_ramtiles:
	; Wait for remaining maximum possible ramtiles
1:
	ldi   r17,     3
	dec   r17
	brne  .-4
	rjmp  .
	dec   r19
	brne  1b

#else

	WAIT  r17,     18 + (MAX_RAMTILES * 14) - 2

#endif


	ldi   YL,      lo8(vram)
	ldi   YH,      hi8(vram)

#if (SCROLLING != 0)

	; Prepare scrolling related elements

	; Add X scroll (coarse)

	lds   r18,     screen_scrollX ; ScreenScrollX
	mov   r25,     r18
	andi  r18,     0xf8    ; (x>>3) * 8 interleave
	add   YL,      r18

	; Save Y wrap adress

	movw  r12,     YL

	; Add Y scroll (coarse)

	lds   r16,     screen_scrollY ; ScreenScrollY
	mov   r22,     r16
	lsr   r16
	lsr   r16
	lsr   r16              ; Divide by 8

	lds   r17,     screen_scrollHeight
	sub   r17,     r16
	mov   r15,     r17     ; Y tiles to draw before wrapping

	mov   r17,     r16
	lsr   r16
	lsr   r16
	lsr   r16              ; Divide by 8
	add   YH,      r16     ; (bits 6-7 for 256 byte VRAM bank select)
	andi  r17,     0x7
	add   YL,      r17     ; Interleave (bits 3-5)
	andi  r22,     0x7     ; Fine Y scrolling (bits 0-2)

#else

	clr   r22              ; Fine Y scrolling (line counter within tile row)

	WAIT  r17,     23

#endif


	; Prepare overlay

	lds   r20,     overlay_tile_table
	lds   r21,     overlay_tile_table + 1
	lds   r6,      tile_table_lo
	lds   r7,      tile_table_hi
	movw  XL,      r6      ; Store for later

	; Save main section values

	movw  r10,     YL      ; Main section VRAM begin
	mov   r23,     r22     ; Main section line counter within tile row (fine Y scroll)
#if (SCROLLING != 0)
	mov   r24,     r15     ; Y tiles to draw before wrapping
	mov   r9,      r25     ; Main section X scroll
#else
	rjmp  .
#endif

	; Load values for overlay if it's activated (overlay_height > 0)

#if (SCROLLING != 0)

	; Compute beginning of overlay in vram

	lds   r16,     screen_scrollHeight
	mov   r18,     r16
	lsr   r16
	lsr   r16
	lsr   r16              ; Hi8
	inc   r16              ; Add 0x100 ram offset
	andi  r18,     7       ; Lo8

	lds   r19,     overlay_height
	cpi   r19,     0
	in    r0,      _SFR_IO_ADDR(SREG)

	sbrs  r0,      SREG_Z
	clr   r22              ; Overlay: No Y fine scroll
	sbrs  r0,      SREG_Z
	mov   YL,      r18     ; lo8(overlay_vram)
	sbrs  r0,      SREG_Z
	mov   YH,      r16     ; hi8(overlay_vram)
	sbrs  r0,      SREG_Z
	ldi   r24,     0xFF    ; Overlay doesn't wrap (max out wrap counter)
	sbrs  r0,      SREG_Z
	clr   r9               ; Overlay has no X scroll
	sbrs  r0,      SREG_Z
	movw  XL,      r20     ; Overlay tile table

#else

	lds   r19,     overlay_height
	cpi   r19,     0

	breq  .+2
	ldi   YL,      lo8(overlay_vram)
	breq  .+2
	ldi   YH,      hi8(overlay_vram)
	breq  .+2
	movw  XL,      r20     ; Overlay tile table

	WAIT  r17,     15

#endif


	; Total scanlines to draw

	lds   r8,      render_lines_count

	; Prepare Timer1 to use it for terminating scanlines

	ldi   r16,     (1 << OCF1B) + (1 << OCF1A) + (1 << TOV1)
	sts   _SFR_MEM_ADDR(TIFR1), r16  ; Clear any pending timer int

	ldi   r16,     (0 << WGM12) + (1 << CS10)
	sts   _SFR_MEM_ADDR(TCCR1B), r16 ; Switch to timer1 normal mode (mode 0)

	ldi   r16,     (1 << TOIE1)
	sts   _SFR_MEM_ADDR(TIMSK1), r16 ; Enable Overflow interrupt




;*************************************************************
; Rendering main loop starts here
;*************************************************************
;
; Starts with the Overlay section, and transitions onto the Main section when
; consuming Overlay tile rows (provided in r19).
;
; r6:r7   = Main section tileset
; r8      = Total scanlines to draw
; r9      = Current section scrollX
; r10:r11 = Main section begin VRAM address
; r12:r13 = Main section Y wrap to adress
; r15     = Main section Y tiles to draw before wrapping
; r19     = Overlay tile rows to draw
; r22     = Current section line counter within tile row
; r23     = Main section begin line counter within tile row
; r24     = Current section Y tiles to draw before wrapping
; r25     = Main section scrollX
; YH:YL   = Current section VRAM address
; XH:XL   = Current section ROM tileset start pointer

next_tile_line:

	; Get tile row offset

	ldi   r16,     TILE_WIDTH ; Tile width in pixels
	mul   r22,     r16     ; r1:r0: Row offset within tile

	; Compute base adresses for ROM and RAM tiles

	movw  r16,     XL      ; Tile table
	subi  r16,     lo8(RAM_TILES_COUNT * TILE_HEIGHT * TILE_WIDTH)
	sbci  r17,     hi8(RAM_TILES_COUNT * TILE_HEIGHT * TILE_WIDTH)
	add   r16,     r0
	adc   r17,     r1
	movw  r2,      r16     ; r3:r2: ROM tiles row adress

	ldi   r16,     lo8(ram_tiles)
	ldi   r17,     hi8(ram_tiles)
	add   r16,     r0
	adc   r17,     r1
	movw  r4,      r16     ; r5:r4: RAM tiles row adress

	ldi   r16,     TILE_HEIGHT * TILE_WIDTH
	mov   r14,     r16     ; 14 cycles

	; Prepare Timer1 OVF interrupt location

#if (RESOLUTION_EXT == 0)
	ldi   r16,     lo8(0xFFFF - (48 * SCREEN_TILES_H) - 44)
	ldi   r17,     hi8(0xFFFF - (48 * SCREEN_TILES_H) - 44)
#else
	ldi   r16,     lo8(0xFFFF - (44 * SCREEN_TILES_H) - 44)
	ldi   r17,     hi8(0xFFFF - (44 * SCREEN_TILES_H) - 44)
#endif

	; Save current VRAM location (left column)

	push  YL
	push  YH

	; Fetch first two tiles to prepare for scrolling output

#if (SCROLLING != 0)
	ld    r21,     Y       ; Tile 0 ID from VRAM
	subi  YL,      0xF8
	ld    r20,     Y       ; Tile 1 ID from VRAM
	subi  YL,      0xF8    ; 6 cycles
#else
	ld    r21,     Y+      ; Tile 0 ID from VRAM
	rjmp  .
	rjmp  .
#endif

	; Enter next scanline including left alignment waits

	rcall hsync_pulse

	WAIT  r18,     HSYNC_USABLE_CYCLES - AUDIO_OUT_HSYNC_CYCLES

#if (RESOLUTION_EXT == 0)
	WAIT  r18,     0  + ((30 - SCREEN_TILES_H) * 24)
#else
	WAIT  r18,     16 + ((32 - SCREEN_TILES_H) * 22)
#endif

	; Set up Timer 1

	sts   _SFR_MEM_ADDR(TCNT1H), r17
	sts   _SFR_MEM_ADDR(TCNT1L), r16
	sei                    ; 7 cycles

#if (SCROLLING != 0)

	; Prepare first two tile addresses

	clr   r16

	mul   r21,     r14     ; r1:r0: Tile address
	cpi   r21,     RAM_TILES_COUNT
	movw  ZL,      r2      ; ROM tile address
	brcc  .+2
	movw  ZL,      r4      ; RAM tile address
	rol   r16              ; r16.0: Tile0 RAM if set
	add   ZL,      r0
	adc   ZH,      r1      ; ZH:ZL: Tile 0 address to start with

	mov   r18,     r9
	andi  r18,     0x07    ; Low 7 bits: 0-7 px visible of last tile
	clr   r1
	add   ZL,      r18
	adc   ZH,      r1      ; ZH:ZL: Skipped non-visible left pixels

	mul   r20,     r14     ; r1:r0: Tile address
	cpi   r20,     RAM_TILES_COUNT
	movw  r20,     r2      ; ROM tile address
	brcc  .+2
	movw  r20,     r4      ; RAM tile address
	rol   r16              ; r16.0: Tile1 RAM if set; r16.1: Tile0 RAM if set
	add   r20,     r0      ; r21:r20: Tile 1 address to start with
	adc   r21,     r1      ; 24 cycles

	; Select entry point

	ldi   r17,     26
	mul   r16,     r17     ; Select entry block
	mov   r16,     r0
	ldi   r17,     3
	mul   r18,     r17     ; Select entry point within block
	clr   r17
	subi  r16,     lo8(-(pm(romrom_e)))
	sbci  r17,     hi8(-(pm(romrom_e)))
	add   r0,      r16
	adc   r1,      r17     ; 12 cycles

	; Enter scanline loop

	clr   r17              ; End of scanline zero pixel
	push  r0
	push  r1
	ret                    ; 9 cycles (+2 relative to non-scrolling)

#else

	; Prepare first tile

	clr   r16

	mul   r21,     r14     ; r1:r0: Tile address
	cpi   r21,     RAM_TILES_COUNT
	movw  ZL,      r2      ; ROM tile address
	brcc  .+2
	movw  ZL,      r4      ; RAM tile address
	rol   r16              ; r16.0: Tile0 RAM if set
	add   ZL,      r0
	adc   ZH,      r1      ; ZH:ZL: Tile 0 address to start with

	; Pad to match scrolling Mode 3's cycle budget

	WAIT  r17,     31

	; Enter scanline loop

	clr   r17              ; End of scanline zero pixel
	sbrs  r16,     0
	rjmp  .+4
	nop
	rjmp  ramloop_px0      ; 6 cycles
	rjmp  romloop_px0      ; 6 cycles

#endif

	; End of scanline using Timer1 overflow

TIMER1_OVF_vect:

	out   PIXOUT,  r17     ; Zero pixel terminating the line

	pop   r0               ; pop & discard OVF interrupt return address
	pop   r0               ; pop & discard OVF interrupt return address

	; Restore VRAM address (left column)

	pop   YH
	pop   YL

	; Right alignment wait

#if (RESOLUTION_EXT == 0)
	WAIT  r16,     11 + ((30 - SCREEN_TILES_H) * 24)
#else
	WAIT  r16,     27 + ((32 - SCREEN_TILES_H) * 22)
#endif

	; Next line & row logic

	inc   r22              ; Line counter within tile row
	dec   r8               ; Total remaining scanlines counter
	breq  text_frame_end

	cpi   r22,     TILE_HEIGHT ; At last char line?
	breq  next_tile_row

	; Wait to align with next_tile_row instructions (+1 cycle for the breq)

	WAIT  r16,     23
	rjmp  next_tile_line

next_tile_row:

	clr   r22              ; Clear line counter for next tile row

	; Increment VRAM pointer for next row

#if (SCROLLING != 0)

	mov   r16,     YL
	andi  r16,     0x7
	cpi   r16,     0x7
	breq  .+4
	inc   YL               ; Within a 8 tile tall block
	rjmp  .+4
	andi  YL,      0xF8    ; Crossing a 8 tile tall block boundary
	inc   YH

	dec   r24              ; Tile rows until wraparound
	brne  .+2
	movw  YL,      r12     ; Load wrap to address

#else

	adiw  YL,      VRAM_TILES_H
	WAIT  r16,     8

#endif

	; Check end of overlay section

	dec   r19              ; At end, load main section params
	brne  .+2
	mov   r22,     r23     ; Main section begin line counter
	brne  .+2
	movw  YL,      r10     ; Main section begin VRAM adress
#if (SCROLLING != 0)
	brne  .+2
	mov   r24,     r15     ; Main section remaining tile rows before Y wrapping
	brne  .+2
	mov   r9,      r25     ; Main section scrollX
#else
	rjmp  .
	rjmp  .
#endif
	brne  .+2
	movw  XL,      r6      ; Main section ROM tileset

	rjmp next_tile_line

text_frame_end:

	WAIT  r18,     48

	; Restore Timer1 to the value it should normally have at this point

	ldi   r16,     hi8(101 - TIMER1_DISPLACE)
	sts   _SFR_MEM_ADDR(TCNT1H), r16
	ldi   r16,     lo8(101 - TIMER1_DISPLACE)
	sts   _SFR_MEM_ADDR(TCNT1L), r16

	rcall hsync_pulse      ; 145

#if ((RTLIST_ENABLE != 0) && (SPRITES_VSYNC_PROCESS != 0))
	clr   r1
	call  RestoreBackground
#endif

	; Set VSync flag & flip field

	lds   ZL,      sync_flags
	ldi   r20,     SYNC_FLAG_FIELD
	ori   ZL,      SYNC_FLAG_VSYNC
	eor   ZL,      r20
	sts   sync_flags, ZL

	; Restore Timer 1's operation mode

	ldi   r16,     (1 << OCF1B) + (1 << OCF1A) + (1 << TOV1)
	sts   _SFR_MEM_ADDR(TIFR1), r16  ; Clear any pending timer int

	ldi   r16,     (1 << WGM12) + (1 << CS10)
	sts   _SFR_MEM_ADDR(TCCR1B), r16 ; Switch back to timer1 CTC mode (mode 4)

	ldi   r16,     (1 << OCIE1A)
	sts   _SFR_MEM_ADDR(TIMSK1), r16 ; Restore ints on compare match

	ret




#if (SCROLLING != 0)

	; Left side entry blocks for 1-8 pixels. Each pixel is 3 words, and a
	; complete block is 26 words (8 * 3 + 2 words). Entry is performed by
	; a ret (pushing the appropriate entry address on stack).

romrom_e:
	rjmp  .
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 0
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 1
	rjmp  .
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 2
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 3
	rjmp  .
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 4
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 5
	rjmp  .
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 6
#if (RESOLUTION_EXT == 0)
	nop
#endif
	lpm   r16,     Z+
	movw  ZL,      r20
	out   PIXOUT,  r16     ; Pixel 7
	rjmp  romloop_px0
#if (RESOLUTION_EXT != 0)
	nop
#endif

romram_e:
	rjmp  .
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 0
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 1
	rjmp  .
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 2
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 3
	rjmp  .
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 4
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 5
	rjmp  .
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 6
#if (RESOLUTION_EXT == 0)
	nop
#endif
	lpm   r16,     Z+
	movw  ZL,      r20
	out   PIXOUT,  r16     ; Pixel 7
	rjmp  ramloop_px0
#if (RESOLUTION_EXT != 0)
	nop
#endif

ramrom_e:
	lpm   r16,     Z       ; Dummy load (nop)
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 0
#if (RESOLUTION_EXT == 0)
	lpm   r16,     Z       ; Dummy load (nop)
#else
	rjmp  .
#endif
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 1
	lpm   r16,     Z       ; Dummy load (nop)
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 2
#if (RESOLUTION_EXT == 0)
	lpm   r16,     Z       ; Dummy load (nop)
#else
	rjmp  .
#endif
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 3
	lpm   r16,     Z       ; Dummy load (nop)
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 4
#if (RESOLUTION_EXT == 0)
	lpm   r16,     Z       ; Dummy load (nop)
#else
	rjmp  .
#endif
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 5
	lpm   r16,     Z       ; Dummy load (nop)
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 6
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif
	ld    r16,     Z+
	movw  ZL,      r20
	out   PIXOUT,  r16     ; Pixel 7
	rjmp  romloop_px0

ramram_e:
	lpm   r16,     Z       ; Dummy load (nop)
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 0
#if (RESOLUTION_EXT == 0)
	lpm   r16,     Z       ; Dummy load (nop)
#else
	rjmp  .
#endif
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 1
	lpm   r16,     Z       ; Dummy load (nop)
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 2
#if (RESOLUTION_EXT == 0)
	lpm   r16,     Z       ; Dummy load (nop)
#else
	rjmp  .
#endif
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 3
	lpm   r16,     Z       ; Dummy load (nop)
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 4
#if (RESOLUTION_EXT == 0)
	lpm   r16,     Z       ; Dummy load (nop)
#else
	rjmp  .
#endif
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 5
	lpm   r16,     Z       ; Dummy load (nop)
	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 6
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif
	ld    r16,     Z+
	movw  ZL,      r20
	out   PIXOUT,  r16     ; Pixel 7
	rjmp  ramloop_px0

#endif




	; Timer1 terminated main scanline loop for either 6 cycles / pixel or
	; 5.5 cycles / pixel. When using 5.5 cy/px, the interval between Pixel
	; 0 and Pixel 1 is 5 cycles, so when there is no X scroll, a 6 cycle
	; interval will be turned into 7 for termination (3 cycles IT latency
	; and 3 cycles JMP in the generated interrupt entry table before the
	; zero pixel output).

romloop:
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 6
	add   r0,      r2      ; Add tile table address + row offset lsb
#if (RESOLUTION_EXT == 0)
	nop
#endif

	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 7, Timer1 OVF IT hits after this when no scrolling
	adc   r1,      r3      ; Add tile table address + row offset msb
	movw  ZL,      r0      ; Next tile (ROM)

romloop_px0:
	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 0
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif

	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 1
#if (SCROLLING == 0)
	ld    r20,     Y+      ; Load next tile ID from VRAM
#else
	ld    r20,     Y       ; Load next tile ID from VRAM
#endif

	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 2
#if (SCROLLING == 0)
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif
#else
	subi  YL,      0xF8    ; Add 8 to VRAM address low
#if (RESOLUTION_EXT == 0)
	nop
#endif
#endif

	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 3
	mul   r20,     r14     ; r14 = Width * Height

	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 4
	cpi   r20,     RAM_TILES_COUNT ; Is tile in RAM or ROM? (RAM tiles have indexes < RAM_TILES_COUNT)
#if (RESOLUTION_EXT == 0)
	nop
#endif

	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 5
	brcc  romloop          ; ROM tiles: stay in ROM loop
	nop

	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 6
	add   r0,      r4      ; Add tile table address + row offset lsb
#if (RESOLUTION_EXT == 0)
	nop
#endif

	lpm   r16,     Z+
	out   PIXOUT,  r16     ; Pixel 7, Timer1 OVF IT hits after this when no scrolling
	adc   r1,      r5      ; Add tile table address + row offset msb
	movw  ZL,      r0      ; Next tile (RAM)

ramloop_px0:
	nop

	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 0
#if (SCROLLING == 0)
	ld    r20,     Y+      ; Load next tile ID from VRAM
#else
	ld    r20,     Y       ; Load next tile ID from VRAM
#endif
#if (RESOLUTION_EXT == 0)
	nop
#endif

	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 1
#if (SCROLLING == 0)
	nop
#else
	subi  YL,      0xF8    ; Add 8 to VRAM address low
#endif
	mul   r20,     r14     ; r14 = Width * Height

	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 2
#if (RESOLUTION_EXT == 0)
	lpm   r16,     Z       ; Dummy load (nop)
#else
	rjmp  .
#endif

	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 3
	lpm   r16,     Z       ; Dummy load (nop)

	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 4
	cpi   r20,     RAM_TILES_COUNT ; Is tile in RAM or ROM? (RAM tiles have indexes < RAM_TILES_COUNT)
#if (RESOLUTION_EXT == 0)
	rjmp  .
#else
	nop
#endif

	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 5
	brcc  ramloop_tr       ; ROM tiles: transfer to ROM loop
	nop
	add   r0,      r4      ; Add tile table address + row offset lsb

	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 6
	adc   r1,      r5      ; Add tile table address + row offset msb
#if (RESOLUTION_EXT == 0)
	nop
#endif

	ld    r16,     Z+
	movw  ZL,      r0      ; Next tile (RAM)
	out   PIXOUT,  r16     ; Pixel 7, Timer1 OVF IT hits after this when no scrolling
	rjmp  ramloop_px0

ramloop_tr:
	add   r0,      r2      ; Add tile table address + row offset lsb

	ld    r16,     Z+
	out   PIXOUT,  r16     ; Pixel 6
	adc   r1,      r3      ; Add tile table address + row offset msb
#if (RESOLUTION_EXT == 0)
	nop
#endif

	ld    r16,     Z+
	movw  ZL,      r0      ; Next tile (ROM)
	out   PIXOUT,  r16     ; Pixel 7, Timer1 OVF IT hits after this when no scrolling
	rjmp  romloop_px0






;***********************************
; Copy a flash tile to a ram tile
; C-callable
; r24=Source ROM tile index
; r22=Dest RAM tile index
;************************************
CopyFlashTile:
	ldi r18,TILE_HEIGHT*TILE_WIDTH

	;compute source adress
	lds ZL,tile_table_lo
	lds ZH,tile_table_hi
	mul r24,r18
	add ZL,r0
	adc ZH,r1

	;compute destination adress
	ldi XL,lo8(ram_tiles)
	ldi XH,hi8(ram_tiles)
	mul r22,r18
	add XL,r0
	adc XH,r1

	;copy data (fastest possible)
.rept TILE_HEIGHT*TILE_WIDTH
	lpm r1,Z+
	st X+,r1
.endr
	clr r1
	ret

;***********************************
; Copy a flash tile to a ram tile
; C-callable
; r24=Source RAM tile index
; r22=Dest RAM tile index
;************************************
CopyRamTile:

	ldi r18,TILE_HEIGHT*TILE_WIDTH

	;compute source adress
	ldi ZL,lo8(ram_tiles)
	ldi ZH,hi8(ram_tiles)
	mul r24,r18
	add ZL,r0
	adc ZH,r1

	;compute destination adress
	ldi XL,lo8(ram_tiles)
	ldi XH,hi8(ram_tiles)
	mul r22,r18
	add XL,r0
	adc XH,r1

	;copy data (fastest possible)
.rept TILE_HEIGHT*TILE_WIDTH
	ld r1,Z+
	st X+,r1
.endr
	clr r1
	ret



;***********************************
; Restore background (VRAM)
; C-callable
;************************************
RestoreBackground:

#if (RTLIST_ENABLE != 0)

	; Restore list: Begin at user_ram_tiles_c (above the user RAM tiles),
	; end before free_tile_index (the first unused RAM tile).

	lds   ZL,      user_ram_tiles_c
	mov   r24,     ZL
	add   ZL,      ZL
	add   ZL,      r24     ; Multiply by 3
	clr   ZH
	subi  ZL,      lo8(-(ram_tiles_restore))
	sbci  ZH,      hi8(-(ram_tiles_restore))

	lds   r0,      free_tile_index
	sub   r24,     r0
	brcc  rbg_exit

	; Restore loop

rbg_loop:
	ld    XL,      Z+      ; VRAM address low
	ld    XH,      Z+      ; VRAM address high
	ld    r0,      Z+      ; Tile index to restore
	st    X,       r0
	inc   r24
	brne  rbg_loop

rbg_exit:

#endif

#if (SPRITES_AUTO_PROCESS == 0)
	lds   r0,      user_ram_tiles_c_tmp
	sts   user_ram_tiles_c, r0
	sts   free_tile_index, r0
#endif

	ret



;***********************************
; SET TILE 8bit mode
; C-callable
;     r24: RAM tile index (bt)
; r23:r22: Sprite flags : Sprite tile index
; r21:r20: Y:X (0 or 1, location of 8x8 sprite fragment on 2x2 tile container)
; r19:r18: DY:DX (0 to 7, offset of sprite relative to 0:0 of container)
;************************************
BlitSpritePart:

	; Get tile bank addr or User RAM tiles base depending on sprite source

#if (SPRITE_RAM_ENABLE != 0)
	sbrs  r23,     SPRITE_RAM_BIT
	rjmp  bsp_srom

	set                    ; T flag: RAM sprite if set
	ldi   ZL,      lo8(ram_tiles)
	ldi   ZH,      hi8(ram_tiles)
	rjmp  bsp_send

bsp_srom:

	clt
#endif
	ldi   r25,     4 * 2
	mul   r23,     r25
	mov   XL,      r1
	clr   XH
	subi  XL,      lo8(-(sprites_tile_banks))
	sbci  XH,      hi8(-(sprites_tile_banks))
	ld    ZL,      X+
	ld    ZH,      X+

bsp_send:

	ldi   r25,     TILE_WIDTH * TILE_HEIGHT
	mul   r22,     r25
	add   ZL,      r0      ; Tile data src
	adc   ZH,      r1

	; dest = ram_tiles + (bt * TILE_HEIGHT * TILE_WIDTH)

	mul   r24,     r25
	movw  XL,      r0
	subi  XL,      lo8(-(ram_tiles))
	sbci  XH,      hi8(-(ram_tiles))

	/*
	if ((yx & 1U) == 0U){
		srcXdiff = dx;
		xspan    = TILE_WIDTH - dx;
		if ((flags & SPRITE_FLIP_X) == 0U){
			dest += dx;
		}else{
			dest += (TILE_WIDTH - 1U);
			src  += srcXdiff;
		}
	}else{
		srcXdiff = TILE_WIDTH - dx;
		xspan    = dx;
		if ((flags & SPRITE_FLIP_X) == 0U){
			src  += srcXdiff;
		}else{
			dest += (dx - 1U);
		}
	}
	*/

	ldi   r25,     0       ; srcXdiff high byte & used for zero
	sbrc  r20,     0
	rjmp  x_2nd_tile

	mov   r24,     r18     ; srcXdiff = dx
	ldi   r20,     TILE_WIDTH
	sub   r20,     r18     ; xspan = TILE_WIDTH - dx
	sbrs  r23,     SPRITE_FLIP_X_BIT
	rjmp  x_1st_tile_nxf

	adiw  XL,      TILE_WIDTH - 1 ; dest += (TILE_WIDTH - 1)

x_2nd_tile_nxf:

	add   ZL,      r24
	adc   ZH,      r25     ; src += srcXdiff
	rjmp  x_check_end

x_2nd_tile:

	ldi   r24,     TILE_WIDTH
	sub   r24,     r18     ; srcXdiff = TILE_WIDTH - dx
	mov   r20,     r18     ; xspan = dx;
	sbrs  r23,     SPRITE_FLIP_X_BIT
	rjmp  x_2nd_tile_nxf

	sbiw  XL,      1       ; dest -= 1

x_1st_tile_nxf:

	add   XL,      r18
	adc   XH,      r25     ; dest += dx

x_check_end:

	/*
	if ((yx & 0x0100U) == 0U){
		yspan = (TILE_HEIGHT - dy);
		dest += (dy * TILE_WIDTH);
		if ((flags & SPRITE_FLIP_Y) != 0U){
			src += (TILE_WIDTH * (TILE_HEIGHT - 1U));
		}
	}else{
		yspan = dy;
		if ((flags & SPRITE_FLIP_Y) != 0U){
			src += (TILE_WIDTH * (dy - 1));
		}else{
			src += (TILE_WIDTH * (TILE_HEIGHT - dy));
		}
	}
	*/

	ldi   r22,     TILE_WIDTH
	ldi   r18,     TILE_HEIGHT
	sub   r18,     r19     ; temp = (TILE_HEIGHT - dy)

	sbrc  r21,     0
	rjmp  y_2nd_tile

	mul   r22,     r19
	add   XL,      r0
	adc   XH,      r1      ; dest += (dy * TILE_WIDTH)

	sbrc  r23,     SPRITE_FLIP_Y_BIT
	subi  ZL,      lo8(-(TILE_WIDTH * (TILE_HEIGHT - 1)))
	sbrc  r23,     SPRITE_FLIP_Y_BIT
	sbci  ZH,      hi8(-(TILE_WIDTH * (TILE_HEIGHT - 1)))

	mov   r1,      r18     ; yspan = temp

	rjmp  y_check_end

y_2nd_tile:

	mov   r1,      r19     ; temp = dy - 1
	dec   r1
	sbrs  r23,     SPRITE_FLIP_Y_BIT
	mov   r1,      r18     ; temp = TILE_HEIGHT - dy
	mul   r22,     r1      ; src += (temp * TILE_WIDTH)
	add   ZL,      r0
	adc   ZH,      r1

	mov   r1,      r19     ; yspan = dy

y_check_end:

	/*
	if ((flags & SPRITE_FLIP_Y) != 0U){
		srcXdiff -= (TILE_WIDTH * 2);
	}
	*/

	sbrc  r23,     SPRITE_FLIP_Y_BIT
	sbiw  r24,     (TILE_WIDTH * 2)

	/*
	if ((flags & SPRITE_FLIP_X) == 0U){
		destXdiff = TILE_WIDTH - (xspan - 1);
		step = 1;
	}else{
		destXdiff = TILE_WIDTH + (xspan - 1);
		step = -1;
	}
	; destXdiff is calculated negated for an optimization in the loop
	*/

	sbrc  r23,     SPRITE_FLIP_X_BIT
	rjmp  x_diff_xf

	ldi   r21,     -(TILE_WIDTH + 1) ; destXdiff = -(TILE_WIDTH + 1)
	add   r21,     r20     ; destXdiff += xspan
#if (RT_ALIGNED == 0)
	ldi   r23,     0x00
#endif
	ldi   r22,     0x01    ; step = 1
	rjmp  x_diff_end

x_diff_xf:

	ldi   r21,     -(TILE_WIDTH - 1) ; destXdiff = -(TILE_WIDTH - 1)
	sub   r21,     r20     ; destXdiff -= xspan
#if (RT_ALIGNED == 0)
	ldi   r23,     0xFF
#endif
	ldi   r22,     0xFF    ; step = -1

x_diff_end:

	/*
	for (y2 = 0U; y2 < yspan; y2++){
		for (x2 = 0U; x2 < xspan; x2++){
			px = pgm_read_byte(src);
			src ++;
			if(px != TRANSLUCENT_COLOR){
				*dest = px;
			}
			dest += step;
		}
		src += srcXdiff;
		dest += destXdiff;
	}
	*/
	;     r19 = translucent color
	;  r1:r20 = yspan:xspan
	; r23:r22 = step
	;     r21 = destXdiff (negated)
	;       X = dest
	; r25:r24 = srcXdiff
	;       Z = src
	;
	; dest += step; is omitted for the last X iteration, compensated with
	; destXdiff.
	; destXdiff is negated to allow for using sub instead of add, so subi
	; can be used to subtract high byte.

	ldi   r19,     TRANSLUCENT_COLOR

	mov   r0,      r20     ; xspan
	lsr   r20
#if (SPRITE_RAM_ENABLE != 0)
	brts  bsp_ramloop      ; T flag set: RAM loop, clear: ROM loop
#endif
	brcc  x_loop1          ; ROM sprite loop entry
	breq  x_loopx

x_loop0:
	lpm   r18,     Z+      ; px = pgm_read_byte(src); src ++;
	cpse  r18,     r19     ; if (px != TRANSLUCENT_COLOR)
	st    X,       r18     ; *dest = px
	add   XL,      r22     ; dest += step;
#if (RT_ALIGNED == 0)
	adc   XH,      r23
#endif
x_loop1:
	lpm   r18,     Z+      ; px = pgm_read_byte(src); src ++;
	cpse  r18,     r19     ; if (px != TRANSLUCENT_COLOR)
	st    X,       r18     ; *dest = px
	add   XL,      r22     ; dest += step;
#if (RT_ALIGNED == 0)
	adc   XH,      r23
#endif
	subi  r20,     1
	brne  x_loop0
x_loopx:
	lpm   r18,     Z+      ; px = pgm_read_byte(src); src ++;
	cpse  r18,     r19     ; if (px != TRANSLUCENT_COLOR)
	st    X,       r18     ; *dest = px

	dec   r1
	breq  loop_e

	add   ZL,      r24     ; src += srcXdiff
	adc   ZH,      r25
	sub   XL,      r21     ; dest += destXdiff (negated)
#if (RT_ALIGNED == 0)
	sbci  XH,      0xFF
#endif

	mov   r20,     r0      ; xspan
	lsr   r20
	brcc  x_loop1
	brne  x_loop0
	rjmp  x_loopx

loop_e:

	ret                    ; r1 is zero at this point

#if (SPRITE_RAM_ENABLE != 0)
bsp_ramloop:

	brcc  r_loop1          ; RAM sprite loop entry
	breq  r_loopx

r_loop0:
	ld    r18,     Z+      ; px = *src; src ++;
	cpse  r18,     r19     ; if (px != TRANSLUCENT_COLOR)
	st    X,       r18     ; *dest = px
	add   XL,      r22     ; dest += step;
#if (RT_ALIGNED == 0)
	adc   XH,      r23
#endif
r_loop1:
	ld    r18,     Z+      ; px = *src; src ++;
	cpse  r18,     r19     ; if (px != TRANSLUCENT_COLOR)
	st    X,       r18     ; *dest = px
	add   XL,      r22     ; dest += step;
#if (RT_ALIGNED == 0)
	adc   XH,      r23
#endif
	subi  r20,     1
	brne  r_loop0
r_loopx:
	ld    r18,     Z+      ; px = *src; src ++;
	cpse  r18,     r19     ; if (px != TRANSLUCENT_COLOR)
	st    X,       r18     ; *dest = px

	dec   r1
	breq  loop_e

	add   ZL,      r24     ; src += srcXdiff
	adc   ZH,      r25
	sub   XL,      r21     ; dest += destXdiff (negated)
#if (RT_ALIGNED == 0)
	sbci  XH,      0xFF
#endif

	mov   r20,     r0      ; xspan
	lsr   r20
	brcc  r_loop1
	brne  r_loop0
	rjmp  r_loopx
#endif




;*****************************
; Defines where the sprites tile are defined. (obsolete, use SetSpritesTileTableBank)
; C-callable
; r25:r24=pointer to sprites pixel data.
;*****************************
.section .text.SetSpritesTileTable
SetSpritesTileTable:
	sts sprites_tile_banks,r24
	sts sprites_tile_banks+1,r25
	ret



;*****************************
; Defines where the sprites tile are defined.
; Sprites can use one of four tile banks.
; C-callable
;     r24=bank No (0-3)
; r23:r22=pointer to sprites pixel data.
;*****************************
.section .text.SetSpritesTileBank
SetSpritesTileBank:
	andi r24,3
	lsl r24	
	ldi ZL,lo8(sprites_tile_banks)
	ldi ZH,hi8(sprites_tile_banks)
	add ZL,r24
	adc ZH,r1
	st Z,r22
	std Z+1,r23
	ret

;***********************************
; CLEAR VRAM 8bit
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearVram
ClearVram:
	//init vram		
	ldi r30,lo8(VRAM_SIZE+(VRAM_TILES_H*OVERLAY_LINES))
	ldi r31,hi8(VRAM_SIZE+(VRAM_TILES_H*OVERLAY_LINES))

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)

	ldi r22,RAM_TILES_COUNT

fill_vram_loop:
	st X+,r22
	sbiw r30,1
	brne fill_vram_loop

	clr r1

	ret

;***********************************
; SET FONT TILE
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; r20=Font tile No (8 bit)
;************************************
.section .text.SetFont
SetFont:
	lds r21,font_tile_index
	add r20,21
	rjmp SetTile	

;***********************************
; SET TILE 8bit mode
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; r20=Tile No (8 bit)
;************************************
.section .text.SetTile
SetTile:
#if SCROLLING == 1
	;index formula is vram[((y>>3)*256)+8x+(y&7)]
	
	andi r24,0x1f
	mov r23,r22
	lsr r22
	lsr r22
	lsr r22			;y>>3
	ldi r18,8		
	mul r24,r18		;x*8
	movw XL,r0
	subi XL,lo8(-(vram))
	sbci XH,hi8(-(vram))
	add XH,r22		;vram+((y>>3)*256)
	andi r23,7		;y&7	
	add XL,r23
						
	subi r20,~(RAM_TILES_COUNT-1)	
	st X,r20

	clr r1

	ret

#else

	clr r25
	clr r23	

	ldi r18,VRAM_TILES_H

	mul r22,r18		;calculate Y line addr in vram
	add r0,r24		;add X offset
	adc r1,r25
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1
	
	subi r20,~(RAM_TILES_COUNT-1)	
	st X,r20

	clr r1

	ret

#endif


;***********************************
; SET FONT Index
; C-callable
; r24=First font tile index in tile table (8 bit)
;************************************
.section .text.SetFontTilesIndex
	SetFontTilesIndex:
	sts font_tile_index,r24
	ret




;***********************************
; Define the tile data source
; C-callable
; r25:r24=pointer to tiles data
;************************************
.section .text.SetTileTable
SetTileTable:
	sts tile_table_lo,r24
	sts tile_table_hi,r25	
	ret



;***********************************
; Get the tile index at the specified position 
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; Returns: Tile No (8 bit)
;************************************
.section .text.GetTile
GetTile:
#if SCROLLING == 1
	;index formula is vram[((y>>3)*256)+8x+(y&7)]
	
	andi r24,0x1f
	mov r23,r22
	lsr r22
	lsr r22
	lsr r22			;y>>3
	ldi r18,8		
	mul r24,r18		;x*8
	movw XL,r0
	subi XL,lo8(-(vram))
	sbci XH,hi8(-(vram))
	add XH,r22		;vram+((y>>3)*256)
	andi r23,7		;y&7	
	add XL,r23

	ld r24,X						
	subi r24,RAM_TILES_COUNT
	
	clr r25
	clr r1

	ret

#else

	clr r25
	clr r23	

	ldi r18,VRAM_TILES_H

	mul r22,r18		;calculate Y line addr in vram
	add r0,r24		;add X offset
	adc r1,r25
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1
	
	ld r24,X
	subi r24,RAM_TILES_COUNT

	clr r25
	clr r1

	ret

#endif

