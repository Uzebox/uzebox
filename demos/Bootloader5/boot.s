/*
 *  Boot-up routine
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


;
; Main entry point. There is no standard C library, I just still call it this
; way (the kernel jumps explicitly to this routine now). This doesn't return.
;
; The 'T' flag if set indicates boot request by a prepared SD structure.
;
.section .text
.global main
main:

	;
	; Booting:
	;
	; Four outcomes are possible:
	;
	; - (1) Booting the last programmed game
	; - (2) Starting the game selector
	; - (3) Programming a game from the SD card
	; - (4) Boorloader request with a prepared SD structure
	;
	; (4) has priority (T flag) over everything: the application requested
	; programming a new .uze file. SD structure is set up for it, the card
	; is already initialized.
	;
	; (3) has priority unless a controller button is pressed. This checks
	; whether there is an SD card in the socket, if so, whether it has a
	; single .uze file. If the card carries a single .uze, assume the user
	; wants to run that.
	;
	; (2) takes place when by the EEPROM there is either no game
	; programmed or by the EEPROM starting the game selector is
	; configured or any button is held down on P1's controller.
	;
	; (1) happens otherwise.
	;

	; Prepare tileset & palette to be able to display messages

	rcall Tileset_Load
	rcall Graphics_InitPal

	; Prepare SD structure with the SD buffer pointer. This is always
	; required (when a bootloader request is made, the pointer points to
	; some location within the caller).

	ldi   ZL,      lo8(res_sd_struct + 1)
	ldi   ZH,      hi8(res_sd_struct + 1)
	ldi   r24,     lo8(res_sd_sector)
	ldi   r25,     hi8(res_sd_sector)
	st    Z+,      r24
	st    Z+,      r25

	; Load bootloader flag, so it can also be passed over to Game Selector

	ldi   r24,     0x16    ; Bootloader flag
	rcall ReadEeprom_Head
	mov   r9,      r24     ; Pass it over to the Game Selector

	; Check whether a bootloader request was made. If so, program by the
	; already initialized SD structure.

	brts  main_bootld_req_bypass

	; Check controller, if button is held down, start Game Selector. At
	; least waiting one Vsync is necessary for controller data (wait more
	; for power stabilization and sync stablizitation on a display). This
	; detects controller activity within a time frame for smoother
	; soft-resets.

	ldi   r16,     44      ; Collect presses for 45 frames
	ldi   r17,     0x00    ; Will be nonzero if any button is pressed

main_cread_loop:

	ldi   r24,     1
	rcall WaitVsync
	rcall ReadJoypad       ; 1 player (DISABLE_P2 == 1), so no parameters
	subi  r16,     1       ; Carry set is the loop exit condition
	or    r17,     r24
	or    r17,     r25     ; Combine held-down states, Z flag set if none
	brcc  main_cread_loop
	brne  main_nogame      ; Something was held down

	; Check SD card whether it has a .uze file

	rcall Res_FAT_Init
	cpse  r24,     r1
	rjmp  main_nosd        ; Returned nonzero: No SD card
	ldi   r20,     2       ; Find only up to 2 files
	rcall Res_DIR_List
	cpi   r24,     1
	brne  main_nosd        ; Not one file: Need normal load

	; There is an SD card with one .UZE file on it. Program that.

	ldi   r20,     0       ; Select first file (only file)
	rcall Res_FAT_Select_Cluster_list
	rjmp  Res_Prog_Uze_Nw

main_bootld_req_bypass:

	rcall Res_FAT_Init     ; Reinit card (needed to set up SPI, does not touch file positions)
	ldi   r22,     255     ; Wait 4.25 secs
	rjmp  Res_Prog_Uze

main_nosd:

	; Check ROM: First byte must not be 0x00 or 0xFF. This mostly guards
	; initial programming.

	ldi   ZL,      0
	ldi   ZH,      0
	lpm   r24,     Z
	cpi   r24,     0x00
	breq  main_nogame
	cpi   r24,     0xFF
	breq  main_nogame

	; Game may start if Bootloader flag (set nonzero) permits

	cpse  r9,      r1
	rjmp  Prog_Uze_Boot_Game

main_nogame:

	; Entering the game selector is requested

	rjmp  Game_Selector
