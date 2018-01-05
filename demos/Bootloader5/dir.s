/*
 *  Directory listing functions
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


.section .text



/*
** Collects cluster info for a set of UZE files.
**
** Inputs:
** r25:r24: Pointer to SD data structure
** r23:r22: Pointer to target buffer (for 32 bit start clusters)
**     r20: Length of target buffer (up to 255 files)
** Outputs:
**     r24: Count of files found (up to 255)
** Clobbers:
** r0, r1 (zero), r18, r19, r20, r21, r22, r23, r24, r25, X, Z, T(SREG)
*/
.global DIR_List
DIR_List:

	push  r6
	push  r8
	push  r10
	push  r11
	push  r16
	push  r17
	push  YL
	push  YH
	movw  r10,     r24     ; r11:r10: SD data structure pointer
	movw  YL,      r22     ; YH:YL: target buffer
	clr   r8               ;    r8: count of files found
	mov   r6,      r20     ;    r6: length of target buffer

	rcall FAT_Select_Root

	ldi   r16,     0xFF
	ldi   r17,     0xFF    ; Current directory entry
	clt                    ; First sector will be loaded

DIR_List_loop:

	subi  r16,     0xFF
	sbci  r17,     0xFF

	; If dir. entry low 4 bits are zero, that's the beginning of a 512
	; byte sector which has to be loaded.

	movw  XL,      r16
	andi  XL,      0x0F
	brne  DIR_List_secloaded

	brtc  DIR_List_nextsec
	movw  r24,     r10
	rcall FAT_Next_Sector
	cpi   r24,     0x00
	brne  DIR_List_ret
DIR_List_nextsec:
	movw  r24,     r10
	rcall FAT_Read_Sector
	cpi   r24,     0x00
	brne  DIR_List_ret
	set                    ; Further sectors will be loaded

DIR_List_secloaded:

	; The appropriate sector of the root directory is loaded in the
	; sector buffer. Try to match the file.

	movw  XL,      r10
	adiw  XL,      1
	ld    ZL,      X+
	ld    ZH,      X+      ; Z: Sector buffer ptr.
	mov   XL,      r16
	andi  XL,      0x0F
	ldi   XH,      32
	mul   XL,      XH
	add   ZL,      r0
	adc   ZH,      r1      ; Address of directory entry
	clr   r1

	movw  XL,      r10
	rcall FAT_Get_File_Cluster_XZ

	cp    r22,     r1      ; (r1 is zero)
	cpc   r23,     r1
	cpc   r24,     r1
	cpc   r25,     r1
	breq  DIR_List_loop    ; Start cluster zero: Invalid entry, continue

	ldd   r20,     Z + 0   ; First char: Check if file is valid
	cpi   r20,     0x20    ; Not a valid ASCII character
	brcs  DIR_List_loop
	cpi   r20,     0xE5    ; Erased file marker
	breq  DIR_List_loop
	ldd   r20,     Z + 8   ; Ext 0 ('U') match?
	subi  r20,     'U'
	ldd   r20,     Z + 9   ; Ext 1 ('Z') match?
	sbci  r20,     'Z'
	ldd   r20,     Z + 10  ; Ext 2 ('E') match?
	sbci  r20,     'E'
	brne  DIR_List_loop

	; File matches. Add it to list.

	st    Y+,      r22
	st    Y+,      r23
	st    Y+,      r24
	st    Y+,      r25
	inc   r8
	cp    r8,      r6
	brne  DIR_List_loop

DIR_List_ret:

	mov   r24,     r8      ; Count of files found
	pop   YH
	pop   YL
	pop   r17
	pop   r16
	pop   r11
	pop   r10
	pop   r8
	pop   r6
	ret
