/*
 *  Uzebox Megatris Assembly functions
 *  Copyright (C) 2009  Alec Bourque
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

;**************************************
; This file contains assembly functions
; used by Megatris
;**************************************

#include "defines.h"

.global SetTileMap
.global LoadMap
.global RestoreTile

.section .bss
	tile_map_lo:	.byte 1
	tile_map_hi:	.byte 1

;*****************************
; Defines a tile map
; C-callable
; r25:r24=pointer to tiles map
;*****************************
.section .text.SetTileMap
SetTileMap:
	//adiw r24,2
	sts tile_map_lo,r24
	sts tile_map_hi,r25

	ret

;***********************************
; LOAD Main map
;************************************
.section .text.LoadMap
LoadMap:
	push r16
	push r17
	//init vram

	ldi r24,lo8(VRAM_TILES_H *VRAM_TILES_V)
	ldi r25,hi8(VRAM_TILES_H *VRAM_TILES_V)
	ldi XL,lo8(vram)
	ldi XH,hi8(vram)

	lds ZL,tile_map_lo
	lds ZH,tile_map_hi

	ldi r20,(TILE_WIDTH*TILE_HEIGHT) ;48

	lds r16,tile_table_lo
	lds r17,tile_table_hi

load_map_loop:
	lpm r22,Z+ ;16
	lpm r23,Z+ ;17

	mul r22,r20
	movw r18,r0
	mul r23,r20
	add r19,r0

	add r18,r16
	adc r19,r17

	st X+,r18	;store tile adress
	st X+,r19

	sbiw r24,1
	brne load_map_loop

	clr r1

	pop r17
	pop r16
	ret

;***********************************
; RESTORE TILE
; Copy a map tile # to the same position VRAM
; C-callable
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
;************************************
//.section text.RestoreTile
.section .text.RestoreTile
RestoreTile:
	clr r25
	clr r23
	clr r19
	ldi r18,VRAM_TILES_H*2
	mul r22,r18		;calculate Y line addr
	lsl r24
	add r0,r24		;add X offset
	adc r1,r19

	//load map tile #
	lds ZL,tile_map_lo
	lds ZH,tile_map_hi

	add ZL,r0
	adc ZH,r1
	lpm r20,Z+ 
	lpm r21,Z+ 

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)
	add XL,r0
	adc XH,r1

	ldi r18,(TILE_WIDTH*TILE_HEIGHT)
	mul r20,r18
	movw r22,r0
	mul r21,r18
	add r23,r0

	lds r20,tile_table_lo
	lds r21,tile_table_hi

	add r22,r20
	adc r23,r21

	st X+,r22
	st X,r23

	clr r1

	ret

