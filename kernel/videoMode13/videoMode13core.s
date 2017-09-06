/*
 *  Uzebox Kernel - Mode 13
 *  Copyright (C) 2015  Alec Bourque
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
; Video Mode 13: 28x28 (224x224 pixels) using 8x8 tiles
; with overlay & sprites X flipping.
;
; If the SCROLLING build parameter=0, the scrolling
; code is removed and the screen resolution
; increases up to 30*28 tiles.
;
; For compile time switch and information relating
; to this video mode see:
; http://uzebox.org/wiki/index.php?title=Video_Mode_13
;
;***************************************************

.global TIMER1_OVF_vect
.global vram
.global ram_tiles
.global ram_tiles_restore
.global sprites
.global overlay_vram
.global sprites_tile_banks
.global Screen
.global SetSpritesTileTable
.global CopyTileToRam
.global SetSpritesTileBank
.global SetTile
.global ClearVram
.global SetFontTilesIndex
.global SetTileTable
.global SetTile
.global BlitSprite
.global SetFont
.global GetTile
.global palette
.global SetPaletteColorAsm
.global tile_bank

;Screen Sections Struct offsets
#define scrollX              0
#define scrollY              1
#define sectionHeight        2
#define vramBaseAdressLo     3
#define vramBaseAdressHi     4
#define tileTableAdressLo    5
#define tileTableAdressHi    6
#define wrapLine             7
#define flags                8
#define scrollXcoarse        9
#define scrollXfine         10
#define vramRenderAdressLo  11
#define vramRenderAdressHi  12
#define vramWrapAdressLo    13
#define vramWrapAdressHi    14

;Sprites Struct offsets
#define sprPosX              0
#define sprPosY              1
#define sprTileIndex         2
#define sprFlags             3

#define VIDEO _SFR_IO_ADDR(DATA_PORT)



.section .uze_data_origin

	.align 8

	; Ramtiles must be located at 0x100 and RAM_TILES_COUNT must be a multiple of 8
	; vram must be aligne to 256 bytes
	; palette must be aligned to 256 bytes
	;
	; LDFLAGS += -Wl,--section-start,.noinit=0x800100 -Wl,--section-start,.data=0x80xxxx
	;
	#if RAM_TILES_COUNT & 0x07
		#error RAM_TILES_COUNT must be a multiple of 8
	#endif

	ram_tiles:             .space RAM_TILES_COUNT * (TILE_HEIGHT * TILE_WIDTH / 2) ; 1024
	palette:               .space 256       ; + 256
	vram:                  .space VRAM_SIZE ; + 1024
	                                        ; = 2304 = 0x900 + 0x100 = 0xa00 (Scrolling)


.section .bss

	.align 1

	sprites:               .space SPRITE_STRUCT_SIZE * MAX_SPRITES
	ram_tiles_restore:     .space RAM_TILES_COUNT * 3 ; vram addr | Tile

	sprites_tile_banks:    .space 8
	tile_table_lo:         .space 1
	tile_table_hi:         .space 1
	font_tile_index:       .space 1

	; ScreenType struct members

	Screen:
	overlay_height:        .space 1
	overlay_tile_bank:     .space 1
#if SCROLLING == 1
	screen_scrollX:        .space 1
	screen_scrollY:        .space 1
	screen_scrollHeight:   .space 1
	screen_tile_bank:      .space 1
#endif



.section .text


#if SCROLLING == 1

;***************************************************
; Mode 13 with scrolling
;***************************************************

sub_video_mode13:

	; wait cycles to align with next hsync
	WAIT  r16,     61 - 47

	; Refresh ramtiles indexes in VRAM
	; This has to be done because the main
	; program may have altered the VRAM
	; after vsync and the rendering interrupt.

	ldi   ZL,      lo8(ram_tiles_restore)
	ldi   ZH,      hi8(ram_tiles_restore)

	ldi   YL,      lo8(vram)
	ldi   YH,      hi8(vram)

	lds   r18,     free_tile_index
	ldi   r19,     MAX_RAMTILES ; maximum possible ramtiles
	sub   r19,     r18

	cpi   r18,     0
	breq  no_ramtiles
	nop
	clr   16
upd_loop:
	ld    XL,      Z+      ; load vram offset of ramtile
	ld    XH,      Z+

	ld    r17,     X       ; get latest VRAM tile that may have been modified my
	st    Z+,      r17     ; the main program and store it in the restore buffer
	st    X,       r16     ; write the ramtile index back to vram

	inc   r16
	cp    r16,     r18
	brlo  upd_loop         ; loop is 14 cycles

no_ramtiles:
	; wait for remaining maximum possible ramtiles
1:	
	ldi   r17,     3
	dec   r17
	brne  .-4
	rjmp  .
	dec   r19
	brne  1b

	; clear any pending timer int (to avoid a crashing bug in uzem 1.20 and previous)
	ldi   ZL,      (1 << OCF1B) + (1 << OCF1A) + (1 << TOV1)
	sts   _SFR_MEM_ADDR(TIFR1), ZL

	; set timer1 OVF interrupt
	; this trick allows to exist the main loops
	; when the 30 tiles are rendered
	ldi   r16,     (1 << TOIE1)
	sts   _SFR_MEM_ADDR(TIMSK1), r16
	
	ldi   r16,     (0 << WGM12) + (1 << CS10) ; switch to timer1 normal mode (mode 0)
	sts   _SFR_MEM_ADDR(TCCR1B), r16


	lds   r3,      render_lines_count ; total scanlines to draw
	clr   r2


	; compute main section
	ldi   YL,      lo8(vram)
	ldi   YH,      hi8(vram)
	lds   r18,     screen_scrollX ; add X scroll (coarse) ScreenScrollX
	mov   r12,     r18     ; main section X scroll
	mov   r13,     r18
	andi  r18,     0xf8    ; (x>>3) * 8 interleave
	add   YL,      r18
	movw  r10,     YL      ; main section save for after overlay

	; compute overlay start
	lds   r16,     screen_scrollHeight
	ldi   r17,     VRAM_TILES_H
	mul   r16,     r17
	ldi   r16,     lo8(vram)
	ldi   r17,     hi8(vram)
	add   r16,     r0
	adc   r17,     r1
	lds   r1,      overlay_height
	clr   r0               ; set overlay scroll x=0

	ldi   r20,     SCREEN_TILES_V
	mov   r21,     r20
	sub   r21,     r1      ; V tiles in main section
	
	cpse  r1,      r0      ; if overlay height != 0
	movw  YL,      r16     ; load overlay vram addr
	cpse  r1,      r0
	mov   r13,     r0      ; load overlay xscroll
	cpse  r1,      r0
	mov   r20,     r1      ; load overlay height


;****************************************
; Rendering main loop starts here
;****************************************
; r2      = Y tile row
; r3      = total lines to draw
; r10:r11 = main section vram ptr save
; r12     = main section X scroll save
; r13     = current section X scroll
; r14
; r15
; r20     = current section height
; r21     = next/main section height
; r22
; r23
; r25
; Y       = vram or overlay_ram if overlay_height>0
;
next_tile_line:

	; ***draw line***
	call  render_tile_line

	WAIT  r16,     17


	dec   r3
	breq  frame_end

	inc   r2

	mov   r16,     r2
	cpi   r16,     TILE_HEIGHT ; last char line? 1
	breq  next_tile_row

	; wait to align with next_tile_row instructions (+1 cycle for the breq)
	WAIT  r16,     13
	
	rjmp  next_tile_line

next_tile_row:

	clr   r2               ; current char line 1

	dec   r20              ; current section height in tiles
	brne  same_section

	mov   r20,     r21     ; main section height
	movw  YL,      r10     ; main vram ptr
	mov   r13,     r12     ; main x-scroll
	rjmp .
	rjmp .

	nop
	nop

	rjmp next_tile_line

same_section:

	; increment vram pointer next row
	mov   r16,     YL
	andi  r16,     0x7
	cpi   r16,     7
	breq  1f
	inc   YL
	rjmp  2f
1:
	andi  YL,      0xf8
	inc   YH
2:
	nop
	rjmp  next_tile_line

frame_end:

	WAIT  r16,     39

	; restore timer1 to the value it should normally have at this point
	ldi   r16,     hi8(101 - TIMER1_DISPLACE)
	sts   _SFR_MEM_ADDR(TCNT1H), r16
	ldi   r16,     lo8(101 - TIMER1_DISPLACE)
	sts   _SFR_MEM_ADDR(TCNT1L), r16

	rcall hsync_pulse      ; 145

	clr   r1
	call  RestoreBackground

	; set vsync flag & flip field
	lds   ZL,      sync_flags
	ldi   r20,     SYNC_FLAG_FIELD
	ori   ZL,      SYNC_FLAG_VSYNC
	eor   ZL,      r20
	sts   sync_flags, ZL
	
	; clear any pending timer int
	ldi   ZL,      (1 << OCF1B) + (1 << OCF1A) + (1 << TOV1)
	sts   _SFR_MEM_ADDR(TIFR1), ZL

	ldi   r16,     (1 << WGM12) + (1 << CS10) ; switch back to timer1 CTC mode (mode 4)
	sts   _SFR_MEM_ADDR(TCCR1B), r16

	ldi   r16,     (1 << OCIE1A) ; restore ints on compare match
	sts   _SFR_MEM_ADDR(TIMSK1), r16

	ret



;*************************************************
; RENDER TILE LINE
;
; Input registers:
; ----------------
; r2      = Y offset in tile
; r3      = total lines to draw
; r13     = X scroll (used for fine scroll)
; Y       = VRAM adress to draw from (must not be modified)
;
; Uses
; ----
; r0,r1:   temp/muls
; r4:      =32 (tile size in bytes)
; r5:      always zero
; r6,r7:   second tile save Z pointer for main loop
; r8:      save LUT table ptr for main loop
; r9:      rom or ram tile check
; r16,r17: temp & first pixel in loops
; r18:     screen_tile_bank
; r19:     screen_scrollX fine scroll
; r24:     byte offset in current tile accounting for y offset
; X,Y,Z:   pointers
;*************************************************
render_tile_line:

	; Prepare for line

	lds   r18,     screen_tile_bank
	ori   r18,     1       ; set base adress of both ram and rom tiles at 0x100

	mov   r24,     r2      ; compute byte offset in current tile accounting for y offset
	lsl   r24              ; Y offset in tile * tile width in bytes (4)
	lsl   r24              ;

	ldi   XH,      hi8(palette)
	ldi   r16,     (TILE_HEIGHT * TILE_WIDTH) / 2 ; (bytes per tile)
	mov   r4,      r16     ; tile size in bytes

	clr   r5               ; always 0 - black pixel for end of line

	; Save tile row offset

	push  YL
	push  YH

	; Preload first (partial) and second tile indices along with setting
	; tile row offset for the scanline loop

	ld    r9,      Y       ; load first tile index from VRAM
	subi  YL,      -8
	ld    r19,     Y       ; load second tile index from VRAM

	; Prepare timer value

	ldi   r16,     lo8(0xffff - (48 * SCREEN_TILES_H) - 51)
	ldi   r17,     hi8(0xffff - (48 * SCREEN_TILES_H) - 51)

	; HSync generation & Mixer (the hsync_pulse innards are used directly
	; here to get better centering)

	lds   ZL,      sync_pulse
	dec   ZL
	sts   sync_pulse, ZL

	cbi   _SFR_IO_ADDR(SYNC_PORT), SYNC_PIN ; 2
	ldi   ZL,      2
	rjmp  .
	call  update_sound

	WAIT  ZL,      232 - AUDIO_OUT_HSYNC_CYCLES - 5

	; If less than 30 tiles width is required, wait

	WAIT  ZL,      ((48 * (30 - SCREEN_TILES_H)) / 2)

	; Set timer so that it generates an overflow interrupt when
	; all tiles are rendered

	sts   _SFR_MEM_ADDR(TCNT1H), r17
	sts   _SFR_MEM_ADDR(TCNT1L), r16
	sei

	; Preload first fully visible tile's offset (r7:r6, T flag for
	; rom / ram select), first pixel (r16) and second pixel (r8) as there
	; is no time to fetch these in the scanline loop.
	; r19 contains the tile index

	bst   r19,     7       ; set T flag with msbit of tile index. 1=rom, 0=ram tile
	andi  r19,     0x7f    ; clear tile index msbit to have both ram/rom tile bases adress at zero
	mul   r19,     r4      ; tile * 32
	add   r0,      r24     ; add row offset to tile table addr
	adc   r1,      r18     ; add rom tile bank offset
	movw  ZL,      r0
	brts  .+4
	ld    XL,      Z+      ; load ram pixels 0,1
	rjmp  .+2
	lpm   XL,      Z+      ; load rom pixels 0,1
	movw  r6,      ZL      ; save tile offset for main loop
	ld    r16,     X+      ; save first LUT pixel for main loop
	ld    r8,      X       ; save second LUT pixel for main loop

	; Prepare for scanline entry by rendering first partial tile.
	; r9 contains the tile index.

	mov   r17,     r9
	andi  r17,     0x7f    ; clear tile index msbit to have both ram/rom tile bases adress at zero
	mul   r17,     r4      ; tile * 32
	add   r0,      r24     ; add row offset to tile table addr
	adc   r1,      r18     ; add rom tile bank offset
	movw  ZL,      r0

	mov   r19,     r13
	andi  r19,     7       ; keep fine scroll
	mov   r0,      r19
	lsr   r0               ; adress pixel (2 per byte)
	add   ZL,      r0
	adc   ZH,      r5

	sbrs  r9,      7
	rjmp  ram_scroll


rom_scroll:
	lpm   XL,      Z
	sbrc  r19,     0
	inc   XL               ; ajust palette pointer for odd pixels
	ldi   r17,     3       ; instructions between outs
	mul   r19,     r17
	ldi   r17,     lo8(pm(rom_jtbl))
	ldi   r19,     hi8(pm(rom_jtbl))
	add   r17,     r0
	adc   r19,     r1
	push  r17
	push  r19
	ret


ram_scroll:
	ld    XL,      Z
	sbrc  r19,     0
	inc   XL               ; ajust palette pointer for odd pixels
	ldi   r17,     4       ; instructions between outs
	mul   r19,     r17
	ldi   r17,     lo8(pm(ram_jtbl))
	ldi   r19,     hi8(pm(ram_jtbl))
	add   r17,     r0
	adc   r19,     r1
	push  r17
	push  r19
	ret


rom_jtbl:
	lpm   XL,      Z
	ld    r17,     X+
	out   VIDEO,   r17
	lpm   r0,      Z+
	ld    r17,     X
	out   VIDEO,   r17
	lpm   XL,      Z
	ld    r17,     X+
	out   VIDEO,   r17
	lpm   r0,      Z+
	ld    r17,     X
	out   VIDEO,   r17
	lpm   XL,      Z
	ld    r17,     X+
	out   VIDEO,   r17
	lpm   r0,      Z+
	ld    r17,     X
	out   VIDEO,   r17
	lpm   XL,      Z
	ld    r17,     X+
	out   VIDEO,   r17
	lpm   r0,      Z+
	ld    r17,     X
	out   VIDEO,   r17

	movw  ZL,      r6
	nop
	brts  rom_in
	rjmp  ram_in


ram_jtbl:
	ld    XL,      Z
	ld    r17,     X+
	nop
	out   VIDEO,   r17
	ld    r0,      Z+
	ld    r17,     X
	nop
	out   VIDEO,   r17
	ld    XL,      Z
	ld    r17,     X+
	nop
	out   VIDEO,   r17
	ld    r0,      Z+
	ld    r17,     X
	nop
	out   VIDEO,   r17
	ld    XL,      Z
	ld    r17,     X+
	nop
	out   VIDEO,   r17
	ld    r0,      Z+
	ld    r17,     X
	nop
	out   VIDEO,   r17
	ld    XL,      Z
	ld    r17,     X+
	nop
	out   VIDEO,   r17
	ld    r0,      Z+
	ld    r17,     X
	nop
	out   VIDEO,   r17
	movw  ZL,      r6
	nop
	brts  rom_in
	rjmp  ram_in


romloop:
	lpm   XL,      Z+      ; load rom pixels 0,1

	out   VIDEO,   r8      ; output pixel 7 (ram & rom)
	ld    r16,     X+      ; LUT pixel 0
	ld    r8,      X       ; LUT pixel 1
rom_in:
	subi  YL,      -8      ; VRAM + 8

	out   VIDEO,   r16     ; output pixel 0
	lpm   XL,      Z+      ; load rom pixels 2,3
	ld    r16,     X+      ; LUT pixel 2

	out   VIDEO,   r8      ; output pixel 1
	ld    r8,      X       ; LUT pixel 3
	lpm   XL,      Z+      ; load rom pixels 4,5

	out   VIDEO,   r16     ; output pixel 2
	ld    r17,     Y       ; load next tile index from VRAM
	bst   r17,     7       ; set T flag with msbit of tile index. 1=rom, 0=ram tile
	ld    r16,     X+      ; LUT pixel 4

	out   VIDEO,   r8      ; output pixel 3
	andi  r17,     0x7f    ; clear tile index msbit to have both ram/rom tile bases adress at zero
	mul   r17,     r4      ; tile index * 32
	ld    r8,      X       ; LUT pixel 5

	out   VIDEO,   r16     ; output pixel 4
	lpm   XL,      Z       ; load rom pixels 6,7
	ld    r16,     X+      ; LUT pixel 6

	out   VIDEO,   r8      ; output pixel 5
	ld    r8,      X       ; LUT pixel 7
	movw  ZL,      r0      ; copy next tile address
	add   ZL,      r24     ; add Y tile offset.
	adc   ZH,      r18     ; add rom tile bank offset

	out   VIDEO,   r16     ; output pixel 6
	brts  romloop
	rjmp  ramloop

ramloop:
	ld    XL,      Z+      ; load ram pixels 0,1

	out   VIDEO,   r8      ; output pixel 7 (ram & rom)
	ld    r16,     X+      ; LUT pixel 0
	ld    r8,      X       ; LUT pixel 1
	nop

ram_in:
	out   VIDEO,   r16     ; output pixel 0
	subi  YL,      -8      ; VRAM + 8
	ld    XL,      Z+      ; load ram pixels 2,3
	rjmp  .

	out   VIDEO,   r8      ; output pixel 1
	ld    r17,     Y       ; load next tile from VRAM
	bst   r17,     7       ; set T flag with msbit of tile index. 1=rom, 0=ram tile     
	ld    r16,     X+      ; LUT pixel 2

	out   VIDEO,   r16     ; output pixel 2
	andi  r17,     0x7f    ; clear tile index msbit to have both ram/rom tile bases adress at zero
	ld    r16,     X       ; LUT pixel 3
	mul   r17,     r4      ; tile index * 32

	out   VIDEO,   r16     ; output pixel 3
	ld    XL,      Z+      ; load ram pixels 4,5
	ld    r16,     X+      ; LUT pixel 4
	add   r0,      24      ; add Y tile offset.

	out   VIDEO,   r16     ; output pixel 4
	ld    r16,     X       ; LUT pixel 5
	ld    XL,      Z+      ; load ram pixels 6,7
	adc   r1,      r18     ; add rom tile bank offset

	out   VIDEO,   r16     ; output pixel 5
	ld    r16,     X+      ; LUT pixel 6
	ld    r8,      X       ; LUT pixel 7
	movw  ZL,      r0      ; copy tile pointer

	out   VIDEO,   r16     ; output pixel 6
	brts  romloop
	rjmp  ramloop



; end of render line

TIMER1_OVF_vect:

	out   VIDEO,   r5

	pop   r0               ; pop & discard OVF interrupt return address
	pop   r0               ; pop & discard OVF interrupt return address

	pop   YH
	pop   YL

	; If less than 30 tiles width is required, wait

	WAIT  r16,     ((48 * (30 - SCREEN_TILES_H)) / 2)

	ret


#else

;***************************************************
; Mode 13 with NO scrolling
;***************************************************	
sub_video_mode13:

	;wait cycles to align with next hsync
	WAIT r16,38+15


	;Refresh ramtiles indexes in VRAM 
	;This has to be done because the main
	;program may have altered the VRAM
	;after vsync and the rendering interrupt.
	ldi ZL,lo8(ram_tiles_restore);
	ldi ZH,hi8(ram_tiles_restore);

	ldi YL,lo8(vram)
	ldi YH,hi8(vram)

	lds r18,free_tile_index
	ldi r19,MAX_RAMTILES		;maximum possible ramtiles
	sub r19,r18

	cpi r18,0
	breq no_ramtiles
	
	clr 16
upd_loop:		
	ld XL,Z+	;load vram offset of ramtile
	ld XH,Z+

	ld r17,X	;get latest VRAM tile that may have been modified my 
	st Z+,r17	;the main program and store it in the restore buffer
	st X,r16	;write the ramtile index back to vram

	inc r16
	cp r16,r18
	brlo upd_loop ;loop is 14 cycles

no_ramtiles:
	;wait for remaining maximum possible ramtiles
1:	
	ldi r17,3
	dec r17
	brne .-4
	rjmp .
	dec r19
	brne 1b





	lds r2,overlay_tile_table
	lds r3,overlay_tile_table+1
	lds r16,tile_table_lo 
	lds r17,tile_table_hi
	movw r12,r16
	movw r6,r16

	ldi r24,SCREEN_TILES_V
	ldi YL,lo8(vram)
	ldi YH,hi8(vram)
	movw r8,YL	
	clr r0


	ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
	mov r10,r16
	clr r22

	;clear any pending timer int (to avoid a crashing bug in uzem 1.20 and previous)
	ldi ZL,(1<<OCF1B)+(1<<OCF1A)+(1<<TOV1)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	;set timer1 OVF interrupt
	;this trick allows to exist the main loops 
	;when the 30 tiles are rendered 
	ldi r16,(1<<TOIE1)
	sts _SFR_MEM_ADDR(TIMSK1),r16
	
	ldi r16,(0<<WGM12)+(1<<CS10)	;switch to timer1 normal mode (mode 0)
	sts _SFR_MEM_ADDR(TCCR1B),r16

	nop
	nop
	nop
	nop

;****************************************
; Rendering main loop starts here
;****************************************
;r10    = total lines to draw
;r22	=Y tile row
;Y      = vram or overlay_ram if overlay_height>0
;
next_tile_line:	
	rcall hsync_pulse ;64320 66140
	WAIT r19,242 + FILL_DELAY - AUDIO_OUT_HSYNC_CYCLES

	;***draw line***
	call render_tile_line

	WAIT r19,35 + FILL_DELAY

	dec r10
	breq frame_end

	inc r22
	lpm ;3 nop

	cpi r22,TILE_HEIGHT ;last char line? 1
	breq next_tile_row 

	;wait to align with next_tile_row instructions (+1 cycle for the breq)
	WAIT r19,11
	
	rjmp next_tile_line	

next_tile_row:
	clr r22		;current char line			;1	

	;increment vram pointer next row
	mov r16,YL
	andi r16,0x7
	cpi r16,7
	breq 1f
	inc YL
	rjmp 2f
1:
	andi YL,0xf8
	inc YH
2:

	nop

	rjmp next_tile_line

frame_end:

	WAIT  r16,     12

	; restore timer1 to the value it should normally have at this point
	ldi   r16,     hi8(101 - TIMER1_DISPLACE)
	sts   _SFR_MEM_ADDR(TCNT1H), r16
	ldi   r16,     lo8(101 - TIMER1_DISPLACE)
	sts   _SFR_MEM_ADDR(TCNT1L), r16

	rcall hsync_pulse ;145

	clr r1
	call RestoreBackground

	;set vsync flag & flip field
	lds ZL,sync_flags
	ldi r20,SYNC_FLAG_FIELD
	ori ZL,SYNC_FLAG_VSYNC
	eor ZL,r20
	sts sync_flags,ZL
	
	;clear any pending timer int
	ldi ZL,(1<<OCF1B)+(1<<OCF1A)+(1<<TOV1)
	sts _SFR_MEM_ADDR(TIFR1),ZL

	ldi r16,(1<<WGM12)+(1<<CS10)	;switch back to timer1 CTC mode (mode 4)
	sts _SFR_MEM_ADDR(TCCR1B),r16

	ldi r16,(1<<OCIE1A)				;restore ints on compare match
	sts _SFR_MEM_ADDR(TIMSK1),r16

	ret



;*************************************************
; RENDER TILE LINE
;
; r22     = Y offset in tiles
; r23 	  = tile width in bytes
; Y       = VRAM adress to draw from (must not be modified)
;*************************************************
render_tile_line:

	push YL
	push YH
	
	;Set timer so that it generates an overflow interrupt when
	;all tiles are rendered
	ldi r16,lo8(0xffff-(6*8*SCREEN_TILES_H)+9-30)
	ldi r17,hi8(0xffff-(6*8*SCREEN_TILES_H)+9-30)
	sts _SFR_MEM_ADDR(TCNT1H),r17
	sts _SFR_MEM_ADDR(TCNT1L),r16
	sei

	lds r18,tile_bank
	ori r18,1		;set base adress of both ram and rom tiles at 0x100

	mov r24,r22		;Y offset in tiles*tile width in bytes (4)
	lsl r24
	lsl r24
	
	ldi XH,hi8(palette)
	ldi r16,(TILE_HEIGHT*TILE_WIDTH)/2  ;(bytes per tile)
	mov r15,r16
	clr r2

    ld r17,Y     	;load first tile # from VRAM
	bst r17,7		;set T flag with msbit of tile index. 1=rom, 0=ram tile   
	andi r17,0x7f   ;clear tile index msbit to have both ram/rom tile bases adress at zero	
	mul r17,r15 	;tile*32	
    add r0,r24    	;add row offset to tile table addr
	adc r1,r18		;add rom tile bank offset
	movw ZL,r0

	lpm XL,Z+       ;load rom pixels 0,1
	brtc ramloop
	rjmp .

romloop:
	ld   r16,X+		;LUT pixel 0
	subi YL,-8		;VRAM+8

	out VIDEO,r16	;output pixel 0
	ld 	r17,Y		;load next tile index from VRAM
	bst r17,7		;set T flag with msbit of tile index. 1=rom, 0=ram tile   
	ld 	r16,X		;LUT pixel 1

	out VIDEO,r16   ;output pixel 1
	lpm XL,Z+		;load rom pixels 2,3
	ld  r16,X+      ;LUT pixel 2

	out VIDEO,r16   ;output pixel 2
	andi r17,0x7f   ;clear tile index msbit to have both ram/rom tile bases adress at zero
	mul r17,r15     ;tile index * 32
	ld   r16,X      ;LUT pixel 3

	out VIDEO,r16   ;output pixel 3
	lpm XL,Z+		;load rom pixels 4,5
	ld   r16,X+     ;LUT pixel 4

	out VIDEO,r16   ;output pixel 4
	ld   r16,X      ;LUT pixel 5
	lpm XL,Z		;load rom pixels 6,7   

	out VIDEO,r16   ;output pixel 5
	ld   r16,X+     ;LUT pixel 6
	ld   r17,X      ;LUT pixel 7
	movw ZL,r0      ;copy next tile address

	out VIDEO,r16   ;output pixel 6
	add ZL,r24		;add Y tile offset. 
	adc ZH,r18		;add rom tile bank offset 
	lpm XL,Z+       ;load rom pixels 0,1
	 
mainloop:
	out VIDEO,r17	;output pixel 7 (ram & rom)
	brts romloop

ramloop: 
	ld XL,-Z		;load ram pixels 0,1
	ld r16,X+		;LUT pixel 0

	out VIDEO,r16   ;output pixel 0
	ld r16,X      	;LUT pixel 1
	ldd XL,Z+1      ;load ram pixels 2,3
	subi YL,-8		;VRAM+8

	out VIDEO,r16   ;output pixel 1
	ld r17,Y      	;load next tile from VRAM
	bst r17,7      	;set T flag with msbit of tile index. 1=rom, 0=ram tile     
	ld r16,X+      	;LUT pixel 2
  
	out VIDEO,r16   ;output pixel 2
	andi r17,0x7f   ;clear tile index msbit to have both ram/rom tile bases adress at zero
	ld r16,X      	;LUT pixel 3
	mul r17,r15     ;tile index * 32
     
	out VIDEO,r16   ;output pixel 3
	ldd XL,Z+2      ;load ram pixels 4,5
	ld r16,X+      	;LUT pixel 4
	add r0,24		;add Y tile offset. 

	out VIDEO,r16   ;output pixel 4
	ld r16,X      	;LUT pixel 5
	ldd XL,Z+3      ;load ram pixels 6,7
	adc r1,r18		;add rom tile bank offset 

	out VIDEO,r16   ;output pixel 5
	ld r16,X+      	;LUT pixel 6
	ld r17,X      	;LUT pixel 7
	movw ZL,r0      ;copy tile pointer

	out VIDEO,r16   ;output pixel 6
	lpm XL,Z+      	;load rom pixels 0,1
	rjmp mainloop	


;end of render line   
TIMER1_OVF_vect:
   out VIDEO,r2


	pop r0	;pop OVF interrupt return address
	pop r0	;pop OVF interrupt return address
	
	pop YH
	pop YL

	nop
	rjmp  .
	rjmp  .

	;restore timer1 to the value it should normally have at this point
;	ldi r16,lo8(0x0029)
;	sts _SFR_MEM_ADDR(TCNT1H),r2
;	sts _SFR_MEM_ADDR(TCNT1L),r16

	ret	;TCNT1 must be equal to 0x0029

#endif




;***********************************
; Copy a flash tile to a ram tile
; C-callable
; r24=ROM tile index
; r22=RAM tile index
;************************************
CopyTileToRam:
	
	ldi r18,TILE_HEIGHT*TILE_WIDTH/2	;tile size in bytes

	;compute source adress
	clr ZL				;tile_table_lo
	lds ZH,screen_tile_bank	;tile_table_hi
	ori ZH,1			;add 0x100 offset	
	mul r24,r18
	add ZL,r0
	adc ZH,r1

	;compute destination adress
	mul r22,r18
	movw XL,r0
	subi XL,lo8(-(ram_tiles))
	sbci XH,hi8(-(ram_tiles))

	;copy data (fastest possible)
.rept TILE_HEIGHT*TILE_WIDTH/2
	lpm r0,Z+	
	st X+,r0
.endr

	clr r1
	ret




;***********************************
; SET TILE 8bit mode
; C-callable
; r24=SpriteNo
; r22=RAM tile index (bt)
; r21:r20=Y:X
; r19:r18=DY:DX
;************************************
BlitSprite:
	push r16
	push r17
	push YL
	push YH

	;src=sprites_tiletable_lo+(sprites[i].tileIndex*TILE_HEIGHT*TILE_WIDTH)
	ldi r25,SPRITE_STRUCT_SIZE
	mul r24,r25

	ldi ZL,lo8(sprites)	
	ldi ZH,hi8(sprites)	
	add ZL,r0
	adc ZH,r1

	ldd r16,Z+sprFlags

	;8x16 multiply
	ldd r24,Z+sprTileIndex
	ldi r30,TILE_WIDTH*TILE_HEIGHT
	mul r24,r30
	movw r26,r0
	
	;get tile bank addr
	ldi r25,4*2
	mul r16,r25
	ldi YL,lo8(sprites_tile_banks)	
	ldi YH,hi8(sprites_tile_banks)	
	clr r0
	add YL,r1
	adc YH,r0		
	ldd ZL,Y+0
	ldd ZH,Y+1
	add ZL,r26	;tile data src
	adc ZH,r27
	
	;dest=ram_tiles+(bt*TILE_HEIGHT*TILE_WIDTH)
	ldi XL,lo8(ram_tiles)	
	ldi XH,hi8(ram_tiles)
	ldi r25,TILE_WIDTH*TILE_HEIGHT
	mul r22,r25
	add XL,r0
	adc XH,r1

	/*
	if((yx&1)==0){
		dest+=dx;	
		destXdiff=dx;
		srcXdiff=dx;
				
		if(flags&SPRITE_FLIP_X){
			src+=(TILE_WIDTH-1);
			srcXdiff=((TILE_WIDTH*2)-dx);
		}
	}else{
		destXdiff=(TILE_WIDTH-dx);

		if(flags&SPRITE_FLIP_X){
			srcXdiff=TILE_WIDTH+dx;
			src+=dx;
			src--;
		}else{
			srcXdiff=destXdiff;
			src+=destXdiff;
		}
	}
	*/
	clr r1
	clr YH		;hi8(srcXdiff)

	cpi r20,0	
	brne x_2nd_tile
	
	add XL,r18	;dest+=dx
	adc XH,r1
	mov r24,r18	;destXdiff=dx
	mov YL,r18	;srcXdiff=dx

	sbrs r16,SPRITE_FLIP_X_BIT
	rjmp x_check_end

	adiw ZL,(TILE_WIDTH-1)	;src+=7
	ldi YL,TILE_WIDTH*2		;srcXdiff=((TILE_WIDTH*2)-dx);
	sub YL,r18	
	rjmp x_check_end

x_2nd_tile:
	ldi r24,TILE_WIDTH
	sub r24,r18		;8-DX = xdiff for dest

	sbrc r16,SPRITE_FLIP_X_BIT
	rjmp x2_flip_x

	mov YL,r24		;srcXdiff=destXdiff;
	add ZL,r24		;src+=destXdiff;
	adc ZH,r1	
	rjmp x_check_end

x2_flip_x:
	ldi YL,TILE_WIDTH
	add YL,r18		;srcXdiff=TILE_WIDTH+dx;	
	add ZL,r18		;src+=dx;
	adc ZH,r1
	sbiw ZL,1		;src--;

x_check_end:



	/*
	if((yx&0x0100)==0){
		dest+=(dy*TILE_WIDTH);
		ydiff=dy;
		if(flags&SPRITE_FLIP_Y){
			src+=(TILE_WIDTH*(TILE_HEIGHT-1));
		}
	}else{			
		ydiff=(TILE_HEIGHT-dy);
		if(flags&SPRITE_FLIP_Y){
			src+=((dy-1)*TILE_WIDTH); 
		}else{
			src+=(ydiff*TILE_WIDTH);
		}
	}
	*/
	cpi r21,0
	brne y_2nd_tile

	ldi r25,TILE_WIDTH	;dest+=(dy*TILE_WIDTH)
	mul r25,r19			
	add XL,r0
	adc XH,r1

	mov r25,r19			;ydiff=dy

	sbrc r16,SPRITE_FLIP_Y_BIT
	adiw ZL,(TILE_WIDTH*(TILE_HEIGHT-1))	;src+=(TILE_WIDTH*(TILE_HEIGHT-1));		

	rjmp y_check_end

y_2nd_tile:
	ldi r25,TILE_HEIGHT	;ydiff=(TILE_HEIGHT-dy)
	sub r25,r19	
	
	mov r22,r19			;temp=dy-1
	dec r22
	sbrs r16,SPRITE_FLIP_Y_BIT
	mov r22,r25			;temp=ydiff

	ldi r21,TILE_WIDTH	;src+=(temp*TILE_WIDTH);
	mul r21,r22
	add ZL,r0
	adc ZH,r1	
y_check_end:	
	
	//if(flags&SPRITE_FLIP_X){
	//	step=-1;
	//}
	ser r22		;step=-1
	ser r23
	sbrs r16,SPRITE_FLIP_X_BIT
	ldi r22,1	;step=1
	sbrs r16,SPRITE_FLIP_X_BIT
	clr r23

	//if(flags&SPRITE_FLIP_Y){
	//	srcXdiff-=(TILE_WIDTH*2);
	//}
	sbrc r16,SPRITE_FLIP_Y_BIT
	sbiw YL,(TILE_WIDTH*2)

	/*
	for(y2=0;y2<(TILE_HEIGHT-ydiff);y2++){
		for(x2=0;x2<(TILE_WIDTH-destXdiff);x2++){
						
			px=pgm_read_byte(src);
			if(px!=TRANSLUCENT_COLOR){
				*dest=px;
			}
			dest++;
			src+=step;
		}		
		src+=srcXdiff;
		dest+=destXdiff;
	}
	*/
	;r19 	= translucent color
	;r20:r21= xspan:yspan
	;r22:r23= step
	;r24	= destXdiff
	;r25	= ydiff
	;X		= dest
	;Y		= srcXdiff
	;Z		= src
	clr r1
	ldi r19,TRANSLUCENT_COLOR

	ldi r21,TILE_HEIGHT
	sub r21,r25 	;yspan=(TILE_HEIGHT-ydiff)

y_loop:
	ldi r20,TILE_WIDTH
	sub r20,r24 	;xspan=(TILE_WIDTH-destXdiff)

x_loop:
	lpm r18,Z		;px=pgm_read_byte(src);
	cpse r18,r19	;if(px!=TRANSLUCENT_COLOR)
	st X,r18		;*dest=px;
	adiw XL,1
	add ZL,r22		;src+=step;
	adc ZH,r23
	dec r20
	brne x_loop

	add ZL,YL		;src+=srcXdiff
	adc ZH,YH
	add XL,r24		;dest+=destXdiff
	adc XH,r1
	dec r21
	brne y_loop


	pop YH
	pop YL
	pop r17
	pop r16
	ret





/*
;***********************************
; SET TILE 8bit mode
; C-callable
; r24=SpriteNo
; r22=RAM tile index (bt)
; r21:r20=Y:X
; r19:r18=DY:DX
;************************************
BlitSprite:
	push r16
	push r17

	;src=sprites_tiletable_lo+(sprites[i].tileIndex*TILE_HEIGHT*TILE_WIDTH)
	ldi r25,SPRITE_STRUCT_SIZE
	mul r24,r25

	ldi ZL,lo8(sprites)	
	ldi ZH,hi8(sprites)	
	add ZL,r0
	adc ZH,r1

	ldd r16,Z+sprFlags

	;8x16 multiply
	ldd r24,Z+sprTileIndex
	;clr r25
	ldi r30,TILE_WIDTH*TILE_HEIGHT
	mul r24,r30
	movw r26,r0
	;mul r25,r30
	;add r27,r0
	
	;get tile bank addr
	ldi r25,4*2
	mul r16,r25
	ldi ZL,lo8(sprites_tile_banks)	
	ldi ZH,hi8(sprites_tile_banks)	
	clr r0
	add ZL,r1
	adc ZH,r0		
	ldd r0,Z+0
	ldd r1,Z+1
	movw ZL,r0

	//lds ZL,sprites_tile_banks
	//lds ZH,sprites_tile_banks+1
	add ZL,r26	;tile data src
	adc ZH,r27
	
	;dest=ram_tiles+(bt*TILE_HEIGHT*TILE_WIDTH)
	ldi XL,lo8(ram_tiles)	
	ldi XH,hi8(ram_tiles)
	ldi r25,TILE_WIDTH*TILE_HEIGHT
	mul r22,r25
	add XL,r0
	adc XH,r1

	;if(x==0){
	;	dest+=dx;
	;	xdiff=dx;
	;}else{
	;	src+=(8-dx);
	;	xdiff=(8-dx);
	;}	
	clr r1

	cpi r20,0
	brne x_2nd_tile
	
	add XL,r18
	adc XH,r1
	mov r24,r18	;xdiff for dest
	mov r17,r18	;xdiff for src

	sbrs r16,SPRITE_FLIP_X_BIT
	rjmp x_check_end

	adiw ZL,(TILE_WIDTH-1);7
	ldi r17,16
	sub r17,r18	;xdiff for src
	rjmp x_check_end


x_2nd_tile:
	ldi r24,TILE_WIDTH
	sub r24,r18	;8-DX = xdiff for dest

	sbrc r16,SPRITE_FLIP_X_BIT
	rjmp x2_flip_x

	mov r17,r24	;xdiff for src
	add ZL,r24
	adc ZH,r1	
	rjmp x_check_end

x2_flip_x:
	ldi r17,TILE_WIDTH
	add r17,r18	;xdiff for src
	
	add ZL,r18
	adc ZH,r1
	sbiw ZL,1

x_check_end:





	;if(y==0){
	;	dest+=(dy*TILE_WIDTH);
	;	ydiff=dy;
	;}else{
	;	src+=((8-dy)*TILE_WIDTH);
	;	ydiff=(8-dy);
	;}
	cpi r21,0
	brne y_2nd_tile
	ldi r25,TILE_WIDTH
	mul r25,r19
	add XL,r0
	adc XH,r1
	mov r25,r19	;ydiff
	rjmp y_check_end
y_2nd_tile:
	ldi r25,TILE_HEIGHT
	sub r25,r19	;ydiff
	ldi r21,TILE_WIDTH
	mul r21,r25
	add ZL,r0
	adc ZH,r1	
y_check_end:	


//	for(y2=ydiff;y2<TILE_HEIGHT;y2++){
//		for(x2=xdiff;x2<TILE_WIDTH;x2++){
//							
//			px=pgm_read_byte(src++);
//			if(px!=TRANSLUCENT_COLOR){
//				*dest=px;
//			}
//			dest++;
//
//		}		
//		src+=xdiff;
//		dest+=xdiff;
//
//	}

	clr r1
	ldi r19,TRANSLUCENT_COLOR

	clt
	sbrc r16,SPRITE_FLIP_X_BIT
	set

	ldi r21,TILE_HEIGHT ;8
	sub r21,r25 ;y2

y2_loop:
	ldi r20,TILE_WIDTH ;8
	sub r20,r24 ;x2

	brts x2_loop_flip

	;normal X loop (11 cycles)
x2_loop:
	lpm r18,Z+
	cpse r18,r19
	st X,r18
	adiw XL,1
	dec r20
	brne x2_loop
	rjmp x2_loop_end

	;flipped X loop (13 cycles)
x2_loop_flip:
	lpm r18,Z
	sbiw ZL,1
	cpse r18,r19
	st X,r18
	adiw XL,1
	dec r20
	brne x2_loop_flip

x2_loop_end:
	add ZL,r17
	adc ZH,r1
	add XL,r24
	adc XH,r1
	dec r21
	brne y2_loop




	clr r1

	pop r17
	pop r16
	ret
*/




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
						
	subi r20,~(127)	
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
	
	subi r20,~(127)	
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

;***********************************
; Set the color for a specified palette index
; C-callable
; r24=index
; r22=color
; Returns: void
;************************************
.section .text.SetPaletteColorAsm
SetPaletteColorAsm:

//lsb pixel
//for(i = 0; i < 256; i+=16)
//{
//	palette[i+(index<<1)] = color;
//}

//msb pixel
//for(i = 1; i < 32; i+=2)
//{
//	palette[(index*32)+i] = color;
//}

	;set low pixel
	ldi ZL,lo8(palette)
	ldi ZH,hi8(palette)
	mov r25,r24
	lsl r25
	add ZL,r25
	adc ZH,r1

	st Z,r22
	std Z+16,r22
	std Z+32,r22
	std Z+48,r22
	subi ZL,lo8(-(64))
	sbci ZH,hi8(-(64))
	st Z,r22
	std Z+16,r22
	std Z+32,r22
	std Z+48,r22
	subi ZL,lo8(-(64))
	sbci ZH,hi8(-(64))
	st Z,r22
	std Z+16,r22
	std Z+32,r22
	std Z+48,r22
	subi ZL,lo8(-(64))
	sbci ZH,hi8(-(64))
	st Z,r22
	std Z+16,r22
	std Z+32,r22
	std Z+48,r22

	;set hi pizel	
	ldi ZL,lo8(palette)
	ldi ZH,hi8(palette)
	ldi r25,32
	mul r24,r25
	add ZL,r0
	adc ZH,r1
	
	std Z+1,r22
	std Z+3,r22
	std Z+5,r22
	std Z+7,r22
	std Z+9,r22
	std Z+11,r22
	std Z+13,r22
	std Z+15,r22
	std Z+17,r22
	std Z+19,r22
	std Z+21,r22
	std Z+23,r22
	std Z+25,r22
	std Z+27,r22
	std Z+29,r22
	std Z+31,r22


	ret


