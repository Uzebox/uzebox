/*
 *  Common data resources
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



.section .bss


;
; SD card and FAT structure
;
; res_sd_struct
;
; This is defined in kernel/uzeboxCore.s to allow for passing such a
; structure as init parameter to the bootloader, used to request loading a
; new application by user applications.
;



;
; Sector buffer
;
.global res_sd_sector
res_sd_sector:
	.space 512


;
; Space for loading up to 255 first clusters
;
.global res_file_list
res_file_list:
	.space 255 * 4



.section .text



;
; Puts res_sd_struct in r25:r24, useful for calling SD / Filesystem functions
;
.global Res_sd_struct_r25_r24
Res_sd_struct_r25_r24:

	ldi   r24,     lo8(res_sd_struct)
	ldi   r25,     hi8(res_sd_struct)
	ret



;
; DIR_List contrained to the fixed data structures. Only r20 has to be set to
; the max. count of files to find.
;
.global Res_DIR_List
Res_DIR_List:

	rcall Res_sd_struct_r25_r24
	ldi   r22,     lo8(res_file_list)
	ldi   r23,     hi8(res_file_list)
	rjmp  DIR_List



;
; FAT_Init constarined to the fixed data structures.
;
.global Res_FAT_Init
Res_FAT_Init:

	rcall Res_sd_struct_r25_r24
	rjmp  FAT_Init



;
; FAT_Select_Cluster constarined to the fixed data structures. Only start
; cluster has to be specified by offset in res_file_list in r20
;
.global Res_FAT_Select_Cluster_list
Res_FAT_Select_Cluster_list:

	ldi   r21,     4
	mul   r20,     r21
	movw  ZL,      r0
	clr   r1
	subi  ZL,      lo8(-(res_file_list))
	sbci  ZH,      hi8(-(res_file_list))
	ld    r20,     Z+
	ld    r21,     Z+
	ld    r22,     Z+
	ld    r23,     Z+      ; Fall through to Res_FAT_Select_Cluster



;
; FAT_Select_Cluster constarined to the fixed data structures. Only start
; cluster in r23:r22:r21:r20 has to be specified.
;
.global Res_FAT_Select_Cluster
Res_FAT_Select_Cluster:

	rcall Res_sd_struct_r25_r24
	rjmp  FAT_Select_Cluster



;
; FAT_Read_Sector constarined to the fixed data structures.
;
.global Res_FAT_Read_Sector
Res_FAT_Read_Sector:

	rcall Res_sd_struct_r25_r24
	rjmp  FAT_Read_Sector



;
; Prog_Uze constarined to the fixed data structures, no waiting.
;
.global Res_Prog_Uze_Nw
Res_Prog_Uze_Nw:

	ldi   r22,     1       ; Fall through to Res_Prog_Uze



;
; Prog_Uze constarined to the fixed data structures.
;
.global Res_Prog_Uze
Res_Prog_Uze:

	rcall Res_sd_struct_r25_r24
	rjmp  Prog_Uze



;
; Graphics_CopyROM constrained to text resouces contained here. r22 selects
; the text resource to use, r25:r24 remains the Destination RAM location
; which ends up in XH:XL incremented with byte count.
;
.global Res_Graphics_CopyROM
Res_Graphics_CopyROM:

	clr   r1
	ldi   ZL,      lo8(RGCR_data_list)
	ldi   ZH,      hi8(RGCR_data_list)
	lsl   r22
	add   ZL,      r22
	adc   ZH,      r1
	lpm   r22,     Z+
	lpm   r23,     Z+      ; r23:r22: Address of data
	lpm   r20,     Z+
	sub   r20,     r22     ; r20: Size of data
	rjmp  Graphics_CopyROM

.global res_text_prguzeerr
.global res_text_waitsd
.global res_text_empty
.global res_text_title
.global res_text_menu
.global res_text_game
.global res_text_year
.equ	res_text_prguzeerr, 0
.equ	res_text_waitsd,    1
.equ	res_text_empty,     2
.equ	res_text_title,     3
.equ	res_text_menu,      4
.equ	res_text_game,      5
.equ	res_text_year,      6

RGCR_data_list:
	.byte lo8(RGCR_data_prguzeerr)
	.byte hi8(RGCR_data_prguzeerr)
	.byte lo8(RGCR_data_waitsd)
	.byte hi8(RGCR_data_waitsd)
	.byte lo8(RGCR_data_empty)
	.byte hi8(RGCR_data_empty)
	.byte lo8(RGCR_data_title)
	.byte hi8(RGCR_data_title)
	.byte lo8(RGCR_data_menu)
	.byte hi8(RGCR_data_menu)
	.byte lo8(RGCR_data_game)
	.byte hi8(RGCR_data_game)
	.byte lo8(RGCR_data_year)
	.byte hi8(RGCR_data_year)
	.byte lo8(RGCR_data_end)
	.byte hi8(RGCR_data_end)

RGCR_data_prguzeerr:
	.byte 'L' - 32, 'o' - 32, 'a' - 32, 'd' - 32, 'i' - 32, 'n' - 32, 'g' - 32, ' ' - 32
	.byte 'f' - 32, 'a' - 32, 'i' - 32, 'l' - 32, 'e' - 32, 'd' - 32, '!' - 32

RGCR_data_waitsd:
	.byte 'I' - 32, 'n' - 32, 's' - 32, 'e' - 32, 'r' - 32, 't' - 32, ' ' - 32, 'S' - 32
	.byte 'D' - 32, ' ' - 32, 'C' - 32, 'a' - 32, 'r' - 32, 'd' - 32

RGCR_data_empty:
	.byte 'E' - 32, 'm' - 32, 'p' - 32, 't' - 32, 'y' - 32, ' ' - 32, 'S' - 32, 'D' - 32
	.byte ' ' - 32, 'C' - 32, 'a' - 32, 'r' - 32, 'd' - 32

RGCR_data_title:
	.byte 'U' - 32, 'z' - 32, 'e' - 32, 'B' - 32, 'o' - 32, 'x' - 32, ' ' - 32, 'G' - 32
	.byte 'a' - 32, 'm' - 32, 'e' - 32, ' ' - 32, 'L' - 32, 'o' - 32, 'a' - 32, 'd' - 32
	.byte 'e' - 32, 'r' - 32, ' ' - 32, ' ' - 32, ' ' - 32, ' ' - 32, ' ' - 32, ' ' - 32
	.byte 'V' - 32, '.' - 32
	.byte ((VERSION >> 12) & 0xF) + '0' - 32, '.' - 32
	.byte ((VERSION >>  8) & 0xF) + '0' - 32, '.' - 32
	.byte ((VERSION >>  4) & 0xF) + '0' - 32
	.byte ((VERSION >>  0) & 0xF) + '0' - 32

RGCR_data_menu:
	.byte 'B' - 32, ':' - 32, ' ' - 32, 'M' - 32, 'e' - 32, 'n' - 32, 'u' - 32

RGCR_data_game:
	.byte 'B' - 32, ':' - 32, ' ' - 32, 'G' - 32, 'a' - 32, 'm' - 32, 'e' - 32

RGCR_data_year:
	.byte 'Y' - 32, 'e' - 32, 'a' - 32, 'r' - 32, ':' - 32, ' ' - 32, '2' - 32, '0' - 32

RGCR_data_end:

.balign 2
