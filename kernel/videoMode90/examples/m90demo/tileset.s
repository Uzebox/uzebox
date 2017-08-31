
;
; Mode 90 tileset of 93 tiles at 8 pixels height
;


#include <avr/io.h>
#define  PIXOUT _SFR_IO_ADDR(PORTC)


.global m90_defpalette
.global m90_deftilerows


.section .text



m90_defpalette:
	.byte 0x00, 0x2F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	.byte 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF



m90_deftilerows:
	jmp   common_row_0_s
	jmp   common_row_1_s
	jmp   common_row_2_s
	jmp   common_row_3_s
	jmp   common_row_4_s
	jmp   common_row_5_s
	jmp   common_row_6_s
	jmp   common_row_7_s



common_row_0_s:
	ldi   r18,     61      ; ( 4) For 60 tiles
	ldi   r19,     7       ; ( 5)
	ldi   r20,     lo8(pm(row_0_code)) ; ( 6)
	ldi   r21,     hi8(pm(row_0_code)) ; ( 7)
	clr   r0               ; ( 8)
	clr   r1               ; ( 9)
	movw  r22,     r0      ; (10)
common_row_0:
	dec   r18              ; ( 8) Cycles relative to tile output
	out   PIXOUT,  r0      ; ( 9)
	breq  common_row_0_e   ; (10 / 11)
	ld    ZL,      X+      ; (12)
	out   PIXOUT,  r1      ; (13)
	andi  ZL,      0x7F    ; (14)
	mul   ZL,      r19     ; (16)
	out   PIXOUT,  r22     ; (17)
	movw  ZL,      r0      ; (18)
	add   ZL,      r20     ; (19)
	adc   ZH,      r21     ; (20)
	out   PIXOUT,  r23     ; (21)
	ijmp                   ; (23)
common_row_0_e:
	nop                    ; (12)
	out   PIXOUT,  r1      ; (13)
	jmp   common_e         ; (16)


row_0_code:
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_0
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_0
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r3
	out   PIXOUT,  r3
	mov   r1,      r3
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_0
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r3
	out   PIXOUT,  r3
	mov   r1,      r3
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_0
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_0
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_0



common_row_1_s:
	ldi   r18,     61      ; ( 4) For 60 tiles
	ldi   r19,     7       ; ( 5)
	ldi   r20,     lo8(pm(row_1_code)) ; ( 6)
	ldi   r21,     hi8(pm(row_1_code)) ; ( 7)
	clr   r0               ; ( 8)
	clr   r1               ; ( 9)
	movw  r22,     r0      ; (10)
common_row_1:
	dec   r18              ; ( 8) Cycles relative to tile output
	out   PIXOUT,  r0      ; ( 9)
	breq  common_row_1_e   ; (10 / 11)
	ld    ZL,      X+      ; (12)
	out   PIXOUT,  r1      ; (13)
	andi  ZL,      0x7F    ; (14)
	mul   ZL,      r19     ; (16)
	out   PIXOUT,  r22     ; (17)
	movw  ZL,      r0      ; (18)
	add   ZL,      r20     ; (19)
	adc   ZH,      r21     ; (20)
	out   PIXOUT,  r23     ; (21)
	ijmp                   ; (23)
common_row_1_e:
	nop                    ; (12)
	out   PIXOUT,  r1      ; (13)
	jmp   common_e         ; (16)


row_1_code:
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_1
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_1
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_1



common_row_2_s:
	ldi   r18,     61      ; ( 4) For 60 tiles
	ldi   r19,     7       ; ( 5)
	ldi   r20,     lo8(pm(row_2_code)) ; ( 6)
	ldi   r21,     hi8(pm(row_2_code)) ; ( 7)
	clr   r0               ; ( 8)
	clr   r1               ; ( 9)
	movw  r22,     r0      ; (10)
common_row_2:
	dec   r18              ; ( 8) Cycles relative to tile output
	out   PIXOUT,  r0      ; ( 9)
	breq  common_row_2_e   ; (10 / 11)
	ld    ZL,      X+      ; (12)
	out   PIXOUT,  r1      ; (13)
	andi  ZL,      0x7F    ; (14)
	mul   ZL,      r19     ; (16)
	out   PIXOUT,  r22     ; (17)
	movw  ZL,      r0      ; (18)
	add   ZL,      r20     ; (19)
	adc   ZH,      r21     ; (20)
	out   PIXOUT,  r23     ; (21)
	ijmp                   ; (23)
common_row_2_e:
	nop                    ; (12)
	out   PIXOUT,  r1      ; (13)
	jmp   common_e         ; (16)


row_2_code:
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r3
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r4
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_2
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_2
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_2
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_2



common_row_3_s:
	ldi   r18,     61      ; ( 4) For 60 tiles
	ldi   r19,     7       ; ( 5)
	ldi   r20,     lo8(pm(row_3_code)) ; ( 6)
	ldi   r21,     hi8(pm(row_3_code)) ; ( 7)
	clr   r0               ; ( 8)
	clr   r1               ; ( 9)
	movw  r22,     r0      ; (10)
common_row_3:
	dec   r18              ; ( 8) Cycles relative to tile output
	out   PIXOUT,  r0      ; ( 9)
	breq  common_row_3_e   ; (10 / 11)
	ld    ZL,      X+      ; (12)
	out   PIXOUT,  r1      ; (13)
	andi  ZL,      0x7F    ; (14)
	mul   ZL,      r19     ; (16)
	out   PIXOUT,  r22     ; (17)
	movw  ZL,      r0      ; (18)
	add   ZL,      r20     ; (19)
	adc   ZH,      r21     ; (20)
	out   PIXOUT,  r23     ; (21)
	ijmp                   ; (23)
common_row_3_e:
	nop                    ; (12)
	out   PIXOUT,  r1      ; (13)
	jmp   common_e         ; (16)


row_3_code:
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r3
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r3
	out   PIXOUT,  r3
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_3
	mov   r0,      r3
	out   PIXOUT,  r3
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_3
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_3
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r4
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_3
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_3
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_3



common_row_4_s:
	ldi   r18,     61      ; ( 4) For 60 tiles
	ldi   r19,     7       ; ( 5)
	ldi   r20,     lo8(pm(row_4_code)) ; ( 6)
	ldi   r21,     hi8(pm(row_4_code)) ; ( 7)
	clr   r0               ; ( 8)
	clr   r1               ; ( 9)
	movw  r22,     r0      ; (10)
common_row_4:
	dec   r18              ; ( 8) Cycles relative to tile output
	out   PIXOUT,  r0      ; ( 9)
	breq  common_row_4_e   ; (10 / 11)
	ld    ZL,      X+      ; (12)
	out   PIXOUT,  r1      ; (13)
	andi  ZL,      0x7F    ; (14)
	mul   ZL,      r19     ; (16)
	out   PIXOUT,  r22     ; (17)
	movw  ZL,      r0      ; (18)
	add   ZL,      r20     ; (19)
	adc   ZH,      r21     ; (20)
	out   PIXOUT,  r23     ; (21)
	ijmp                   ; (23)
common_row_4_e:
	nop                    ; (12)
	out   PIXOUT,  r1      ; (13)
	jmp   common_e         ; (16)


row_4_code:
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_4
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r3
	out   PIXOUT,  r3
	mov   r1,      r3
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r4
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_4
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_4
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_4



common_row_5_s:
	ldi   r18,     61      ; ( 4) For 60 tiles
	ldi   r19,     7       ; ( 5)
	ldi   r20,     lo8(pm(row_5_code)) ; ( 6)
	ldi   r21,     hi8(pm(row_5_code)) ; ( 7)
	clr   r0               ; ( 8)
	clr   r1               ; ( 9)
	movw  r22,     r0      ; (10)
common_row_5:
	dec   r18              ; ( 8) Cycles relative to tile output
	out   PIXOUT,  r0      ; ( 9)
	breq  common_row_5_e   ; (10 / 11)
	ld    ZL,      X+      ; (12)
	out   PIXOUT,  r1      ; (13)
	andi  ZL,      0x7F    ; (14)
	mul   ZL,      r19     ; (16)
	out   PIXOUT,  r22     ; (17)
	movw  ZL,      r0      ; (18)
	add   ZL,      r20     ; (19)
	adc   ZH,      r21     ; (20)
	out   PIXOUT,  r23     ; (21)
	ijmp                   ; (23)
common_row_5_e:
	nop                    ; (12)
	out   PIXOUT,  r1      ; (13)
	jmp   common_e         ; (16)


row_5_code:
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r3
	mov   r1,      r2
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_5



common_row_6_s:
	ldi   r18,     61      ; ( 4) For 60 tiles
	ldi   r19,     7       ; ( 5)
	ldi   r20,     lo8(pm(row_6_code)) ; ( 6)
	ldi   r21,     hi8(pm(row_6_code)) ; ( 7)
	clr   r0               ; ( 8)
	clr   r1               ; ( 9)
	movw  r22,     r0      ; (10)
common_row_6:
	dec   r18              ; ( 8) Cycles relative to tile output
	out   PIXOUT,  r0      ; ( 9)
	breq  common_row_6_e   ; (10 / 11)
	ld    ZL,      X+      ; (12)
	out   PIXOUT,  r1      ; (13)
	andi  ZL,      0x7F    ; (14)
	mul   ZL,      r19     ; (16)
	out   PIXOUT,  r22     ; (17)
	movw  ZL,      r0      ; (18)
	add   ZL,      r20     ; (19)
	adc   ZH,      r21     ; (20)
	out   PIXOUT,  r23     ; (21)
	ijmp                   ; (23)
common_row_6_e:
	nop                    ; (12)
	out   PIXOUT,  r1      ; (13)
	jmp   common_e         ; (16)


row_6_code:
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6
	mov   r0,      r3
	out   PIXOUT,  r3
	mov   r1,      r3
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_6
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_6
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6
	mov   r0,      r3
	out   PIXOUT,  r2
	mov   r1,      r3
	mov   r22,     r3
	mov   r23,     r2
	out   PIXOUT,  r3
	rjmp  common_row_6



common_row_7_s:
	ldi   r18,     61      ; ( 4) For 60 tiles
	ldi   r19,     7       ; ( 5)
	ldi   r20,     lo8(pm(row_7_code)) ; ( 6)
	ldi   r21,     hi8(pm(row_7_code)) ; ( 7)
	clr   r0               ; ( 8)
	clr   r1               ; ( 9)
	movw  r22,     r0      ; (10)
common_row_7:
	dec   r18              ; ( 8) Cycles relative to tile output
	out   PIXOUT,  r0      ; ( 9)
	breq  common_row_7_e   ; (10 / 11)
	ld    ZL,      X+      ; (12)
	out   PIXOUT,  r1      ; (13)
	andi  ZL,      0x7F    ; (14)
	mul   ZL,      r19     ; (16)
	out   PIXOUT,  r22     ; (17)
	movw  ZL,      r0      ; (18)
	add   ZL,      r20     ; (19)
	adc   ZH,      r21     ; (20)
	out   PIXOUT,  r23     ; (21)
	ijmp                   ; (23)
common_row_7_e:
	nop                    ; (12)
	out   PIXOUT,  r1      ; (13)
	jmp   common_e         ; (16)


row_7_code:
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r4
	out   PIXOUT,  r4
	mov   r1,      r4
	mov   r22,     r4
	mov   r23,     r4
	out   PIXOUT,  r4
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r4
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r4
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r4
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r4
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r4
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7
	mov   r0,      r2
	out   PIXOUT,  r2
	mov   r1,      r2
	mov   r22,     r2
	mov   r23,     r2
	out   PIXOUT,  r2
	rjmp  common_row_7



common_e:
	out   PIXOUT,  r22     ; (17)
	lpm   r0,      Z       ; (20) Dummy load (nop)
	out   PIXOUT,  r23     ; (21)
	clr   r0               ; (22)
	rjmp  .                ; (24 = 0)
	out   PIXOUT,  r0      ; ( 1)

;
; Cycle budget at this point:
;
;   27 cycles used for prolog code
; 1440 cycles used for tiles
;    1 cycle used for terminating output
; ----
; 1468 cycles
;

	lpm   r0,      Z       ; (1471) Dummy load (nop)
	lpm   r0,      Z       ; (1474) Dummy load (nop)
	lpm   r0,      Z       ; (1477) Dummy load (nop)
	ret                    ; (1481)
