/*
 *  Game Selector of the bootloader
 *  Copyright (C) 2017 Sandor Zsuga (Jubatian)
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
*/



#include <avr/io.h>
#include "kernel/defines.h"



#define FILELIST_TROW 3
#define FILELIST_VRAM (vram + (FILELIST_TROW * 38))



;
; Bootloader Game Selector. Doesn't return conventionally.
;
.section .text
.global Game_Selector
Game_Selector:

	; r16: Operation flags:
	; bit 0: SD card initialized if set
	; bit 2: Need to rebuild current file list page if set
	ldi   r16,     0x00

	; r15: Count of files in the file list (if available)
	; r14: Count of files rendered on the current page
	; r13: Currently selected file
	ldi   r24,     0x19    ; Last selected file in the bootloader
	rcall ReadEeprom_Head
	mov   r13,     r24

	; r12: Previously selected file (used to determine necessity of
	;      graphics tasks).
	; r11
	; r10: Previous button state to detect presses
	ldi   r24,     0xFF
	mov   r11,     r24     ; Start in held down state so buttons
	mov   r10,     r24     ; held on entry won't register

	; r9: Menu (0) / Game (1) flag, already populated
	; r8: Counter for polling the card (is it still there?)
	clr   r8



	; Display elements

	ldi   r24,     3
	ldi   r22,     16
	ldi   r20,     0x0A
	rcall Graphics_SetCol
	ldi   r24,     ((FILELIST_TROW + 16) * 8) + 5
	ldi   r22,     32
	rcall Graphics_SetCol
	ldi   r24,     19
	ldi   r22,     1
	ldi   r20,     0x01
	rcall Graphics_SetCol
	ldi   r24,     ((FILELIST_TROW + 16) * 8) + 4
	ldi   r22,     1
	rcall Graphics_SetCol

	ldi   r24,     lo8(vram + (1 * 38) + 3)
	ldi   r25,     hi8(vram + (1 * 38) + 3)
	ldi   r22,     res_text_title
	rcall Res_Graphics_CopyROM
	rcall Game_Selector_dispgmsel



	; Enter main loop

Game_Selector_loop:

	; Try to init card if necessary

	sbrc  r16,     0       ; Card initialized?
	rjmp  card_init_end

	rcall Res_FAT_Init
	cpi   r24,     0
	brne  card_poll_end    ; Nonzero return: still no card, skip polling
	ldi   r20,     255     ; Load up to 255 files
	rcall Res_DIR_List
	mov   r15,     r24     ; Count of files on card
	ori   r16,     0x05    ; Card initialized and Need to rebuild display

card_init_end:

	; Poll the card periodically to detect removals

	dec   r8
	dec   r8               ; Approx. 2 seconds between polls
	brne  card_poll_end
	rcall Game_Selector_Read_Sector

card_poll_end:

	; Wait next frame; wait 2 frames as SD accesses can spill over frames
	; which will cause the first wait to be omitted (frame already
	; passed). This prevents flicker in some SD heavy screens (such as
	; "Insert SD Card"), and 30 FPS is enough for this app.

	ldi   r24,     2
	rcall WaitVsync

	; Specials: No card or Empty card, don't handle file list area in
	; these cases (also avoiding overriding current file pointer)

	sbrs  r16,     0
	rjmp  grf_nocard       ; No card, display message
	cpse  r15,     r1
	rjmp  .+4
	ldi   r22,     res_text_empty
	rjmp  grf_emptycard    ; Empty card, display message

	; If currently selected entry is beyond the count of files on the
	; card, limit it.

	cp    r13,     r15
	brcs  .+4
	mov   r13,     r15
	dec   r13

	; If currently selected entry is on a different page than previously,
	; force a rebuild.

	movw  r24,     r12
	andi  r24,     0xF0
	andi  r25,     0xF0
	cp    r24,     r25
	breq  .+2
	ori   r16,     0x04    ; Need to rebuild display



	; Graphics tasks - file list area

	sbrs  r16,     2
	rjmp  grf_noreb        ; Don't need to rebuild file list

	clr   r14              ; No files are rendered on the current page
	ldi   r24,     0xFF
	mov   r12,     r24     ; Invalidate prev. file too to force related updates
	rcall Game_Selector_clearflist
	andi  r16,     0xFB    ; File list cleared ("rebuilt"), remove flag

grf_noreb:

	; If necessary, fetch and display next file

	mov   r24,     r14
	cpi   r24,     0x10
	brcc  grf_nodnext

	mov   r20,     r13
	andi  r20,     0xF0
	or    r20,     r14     ; File's no. to fetch
	cp    r20,     r15
	brcc  grf_nodnext      ; Past the total count of files

	rcall Res_FAT_Select_Cluster_list
	rcall Game_Selector_Read_Sector

	mov   r24,     r14
	ldi   r25,     38
	mul   r24,     r25
	movw  XL,      r0      ; Begin position in VRAM
	clr   r1
	subi  XL,      lo8(-(FILELIST_VRAM + 3))
	sbci  XH,      hi8(-(FILELIST_VRAM + 3))
	ldi   ZL,      lo8(res_sd_sector + 14) ; Name in UZE file
	ldi   ZH,      hi8(res_sd_sector + 14)
	rcall Game_Selector_dispfiledata

	inc   r14              ; Move on to next file

grf_nodnext:

	; Add selector bar

	rcall Game_Selector_clearcol
	mov   r24,     r13
	andi  r24,     0x0F
	lsl   r24
	lsl   r24
	lsl   r24
	subi  r24,     (0x01 - (FILELIST_TROW * 8)) ; Add ((FILELIST_TROW * 8) - 1)
	rcall Graphics_GradBar

	; If file selection changed, update author & year info

	cpse  r12,     r13
	rcall Game_Selector_dispauthor

	; Add page info

	ldi   XL,      lo8(vram + ((FILELIST_TROW + 19) * 38) + 18)
	ldi   XH,      hi8(vram + ((FILELIST_TROW + 19) * 38) + 18)
	mov   r20,     r13     ; Selected file
	swap  r20
	andi  r20,     0x0F
	rcall Game_Selector_dispnum
	ldi   r20,     '/' - 0x20
	st    X+,      r20
	mov   r20,     r15     ; Total files
	dec   r20              ; At this point zero file count is eliminated
	swap  r20              ; So this is OK
	andi  r20,     0x0F
	rcall Game_Selector_dispnum

	; Update prev. selected file

	mov   r12,     r13



	; User controls

	rcall ReadJoypad       ; 1 player (DISABLE_P2 == 1), so no parameters

	movw  r22,     r24
	eor   r22,     r10
	eor   r23,     r11     ; Changed buttons
	and   r22,     r24
	and   r23,     r25     ; Changed & current is pressed
	movw  r10,     r24     ; Update previous

	cp    r22,     r1
	cpc   r23,     r1
	breq  .+2              ; Use a "click" sound to indicate receiving a press
	sbi   _SFR_IO_ADDR(GPIOR0), 7

	ldi   r20,     0xFF
	ldi   r21,     0x10

	movw  r24,     r22
#if (BTN_DOWN < 256)
	andi  r24,     BTN_DOWN
#else
	andi  r25,     BTN_DOWN
#endif
	breq  .+6
	inc   r13              ; Down: Increment one file
	brne  .+2
	mov   r13,     r20

	movw  r24,     r22
#if (BTN_UP < 256)
	andi  r24,     BTN_UP
#else
	andi  r25,     BTN_UP
#endif
	breq  .+4
	cpse  r13,     r1      ; Up: Decrement one file (if not already zero)
	dec   r13

	movw  r24,     r22
#if (BTN_RIGHT < 256)
	andi  r24,     BTN_RIGHT
#else
	andi  r25,     BTN_RIGHT
#endif
	breq  .+2
	add   r13,     r21     ; Right: Increment 16 files
	brcc  .+2
	mov   r13,     r20

	movw  r24,     r22
#if (BTN_LEFT < 256)
	andi  r24,     BTN_LEFT
#else
	andi  r25,     BTN_LEFT
#endif
	breq  .+6
	sub   r13,     r21     ; Left: Decrement 16 files
	brcc  .+2
	clr   r13

	movw  r24,     r22
#if (BTN_SELECT < 256)
	andi  r24,     BTN_SELECT
#else
	andi  r25,     BTN_SELECT
#endif
	breq  .+2              ; Select: Game is selected for programming
	rjmp  Game_Selector_program

	movw  r24,     r22
#if (BTN_START < 256)
	andi  r24,     BTN_START
#else
	andi  r25,     BTN_START
#endif
	breq  .+2              ; Start: Game is selected for programming
	rjmp  Game_Selector_program

	movw  r24,     r22
#if (BTN_B < 256)
	andi  r24,     BTN_B
#else
	andi  r25,     BTN_B
#endif
	breq  gmsel_end        ; B: Game / Menu boot select
	ldi   r24,     1
	cpse  r9,      r1      ; (compare with zero)
	ldi   r24,     0
	mov   r9,      r24     ; Toggle Game / Menu bootloader flag
	rcall Game_Selector_dispgmsel
gmsel_end:



	rjmp  Game_Selector_loop



grf_nocard:

	ldi   r22,     res_text_waitsd

grf_emptycard:

	; Display "Insert Card" or "Empty Card" message

	push  r22
	rcall Game_Selector_clearflist
	rcall Game_Selector_clearcol
	pop   r22
	ldi   r24,     lo8(FILELIST_VRAM + (8 * 38) + (19 - 7))
	ldi   r25,     hi8(FILELIST_VRAM + (8 * 38) + (19 - 7))
	rcall Res_Graphics_CopyROM

	rjmp  Game_Selector_loop



Game_Selector_program:

	; Program the selected game. r13 contains the position

	; Store configuration (Menu / Game select & Display mode select too here)

	ldi   r24,     0x19    ; Last selected file in the bootloader
	mov   r22,     r13
	rcall WriteEeprom_Head

	ldi   r24,     0x16    ; Bootloader flag
	mov   r22,     r9
	rcall WriteEeprom_Head

	; Program game

	mov   r20,     r13
	rcall Res_FAT_Select_Cluster_list
	rcall Res_Prog_Uze_Nw



Game_Selector_clearflist:

	; Clear the file list area

	ldi   r24,     lo8(FILELIST_VRAM)
	ldi   r25,     hi8(FILELIST_VRAM)
	ldi   r22,     (16 * 38) & 0xFF
	ldi   r23,     (16 * 38) >> 8
	rjmp  ClearRAM         ; Clear all data to zero (space, empty)



Game_Selector_clearcol:

	; Clear coloring to remove selector bar

	ldi   r24,     (FILELIST_TROW * 8) - 1
	ldi   r22,     (16 * 8) + 2
	rjmp  Graphics_ClearCol



Game_Selector_Read_Sector:

	; Read SD sector and assume card removed on failure

	rcall Res_FAT_Read_Sector
	cpi   r24,     0
	breq  .+2
	ldi   r16,     0       ; When it fails, assume removed card
	ret



Game_Selector_dispgmsel:

	ldi   r22,     0
	cpse  r9,      r1      ; Compare game / menu select with zero
	ldi   r22,     1
	ldi   r24,     lo8(vram + ((FILELIST_TROW + 19) * 38) + 28)
	ldi   r25,     hi8(vram + ((FILELIST_TROW + 19) * 38) + 28)
	subi  r22,     lo8(-(res_text_menu))
	rjmp  Res_Graphics_CopyROM



Game_Selector_dispauthor:

	mov   r20,     r13
	rcall Res_FAT_Select_Cluster_list
	rcall Game_Selector_Read_Sector
	ldi   r24,     lo8(vram + ((FILELIST_TROW + 19) * 38) + 3)
	ldi   r25,     hi8(vram + ((FILELIST_TROW + 19) * 38) + 3)
	ldi   r22,     res_text_year
	rcall Res_Graphics_CopyROM
	ldi   ZL,      lo8(res_sd_sector + 12) ; Year in UZE file
	ldi   ZH,      hi8(res_sd_sector + 12)
	ld    r20,     Z
	subi  r20,     lo8(2000) ; Only needs low 2 digits (2000 - 2099)
	rcall Game_Selector_dispnum
	ldi   XL,      lo8(vram + ((FILELIST_TROW + 17) * 38) + 3)
	ldi   XH,      hi8(vram + ((FILELIST_TROW + 17) * 38) + 3)
	adiw  ZL,      46 - 12 ; Author in UZE file
	                       ; Fall through to Game_Selector_dispfiledata



Game_Selector_dispfiledata:

	; XH:XL: VRAM target
	; ZH:ZL: Source RAM area

	ldi   r20,     32
Game_Selector_dfd_l:
	ld    r21,     Z+
	subi  r21,     0x20
	cpi   r21,     96
	brcs  .+2
	ldi   r21,     ' ' - 0x20
	st    X+,      r21
	dec   r20
	brne  Game_Selector_dfd_l
	ret



Game_Selector_dispnum:

	; r20:   Number to display (0 - 99)
	; XH:XL: VRAM target, incremented

	ldi   r21,     0xFF
	inc   r21
	subi  r20,     10
	brcc  .-6              ; r21: Upper digit
	rcall Game_Selector_dispnum_out
	subi  r20,     0xF6    ; Add 10 to get lower digit
	mov   r21,     r20
Game_Selector_dispnum_out:
	subi  r21,     0x20 - '0'
	st    X+,      r21
	ret
