/*
 *  Uzeamp's video mode (uzmmode): the frames. See videoMode.def.h and uzm.def.h.
 *
 *  sub_video_uzm (generated: uzm_frame.inc) draws a video frame when uzm.on is set, and
 *  otherwise enters mode 5 (mode5.s), which draws the text.
 *
 *  uzm_fill, below, fills the ring in the vertical blank.
 */

#define SPDR_IO _SFR_IO_ADDR(SPDR)

.global sub_video_uzm
.global uzm
.global uzm_dummy
.global uzm_fill

#include QUOTE(VIDEO_MODE_PATH/mode5.s)

.section .bss
	uzm:        .space UZM_SIZE
	uzm_dummy:  .space 8          ; the trailer's samples go here on lines that keep none

.section .text

#include QUOTE(VIDEO_MODE_PATH/uzm_frame.inc)

;
; uint16_t uzm_fill(void)
;
; The vertical blank's share of the stream: bytes from the open read stored in the ring as the
; frame stores them (kept when below 0xFE), 18 cycles a byte, until UZM_FILL_MAX are unread or
; the next frame is two lines away. Interrupts stay on: one between a byte's write and its read
; only slows the clock. Returns the bytes stored; nothing is being clocked when it returns, and
; the SPI is off. Does nothing when uzm.prod is 0.
;
; r20 0xFF   r21 0xFE   Y the ring   X Y at the start   r23 / r22: the frame's window in
; sync_pulse (phase 1): lo < sync_pulse <= hi is no time to clock
;
uzm_fill:
	lds   r24,     uzm+UZM_O_PROD
	tst   r24
	brne  1f
	clr   r24
	clr   r25
	ret
1:	push  r16
	push  r17
	push  r28
	push  r29
	lds   YL,      uzm+UZM_O_Y
	lds   YH,      uzm+UZM_O_Y+1
	movw  XL,      YL
	ldi   r20,     0xFF
	ldi   r21,     0xFE
	lds   r18,     first_render_line
	lds   r19,     render_lines_count
	ldi   r22,     255
	sub   r22,     r18                 ; hi: two lines before the frame
	ldi   r23,     253
	sub   r23,     r18
	sub   r23,     r19                 ; lo: the frame's last line
	ldi   r16,     (1 << SPE) | (1 << MSTR)
	out   _SFR_IO_ADDR(SPCR), r16
	ldi   r16,     (1 << SPI2X)
	out   _SFR_IO_ADDR(SPSR), r16
	out   SPDR_IO, r20                 ; the first byte

f_check:                               ; at least 17 cycles from the last write to the next read
	lds   r16,     sync_phase
	sbrs  r16,     0
	rjmp  f_room                       ; the vsync's pulses: no frame near
	lds   r16,     sync_pulse
	cp    r23,     r16
	brsh  f_room                       ; after the frame
	cp    r22,     r16
	brsh  f_stop                       ; the frame is two lines away or less
f_room:
	movw  r24,     YL                  ; unread: pc + stored so far - cc
	sub   r24,     XL
	sbc   r25,     XH
	andi  r25,     0x07
	lds   r16,     uzm+UZM_O_PC
	add   r24,     r16
	lds   r16,     uzm+UZM_O_PC+1
	adc   r25,     r16
	lds   r16,     uzm+UZM_O_CC
	sub   r24,     r16
	lds   r16,     uzm+UZM_O_CC+1
	sbc   r25,     r16
	cpi   r24,     lo8(UZM_FILL_MAX)
	ldi   r16,     hi8(UZM_FILL_MAX)
	cpc   r25,     r16
	brge  f_stop
	ldi   r18,     16
f_slot:
	in    r17,     SPDR_IO             ; (0)
	out   SPDR_IO, r20                 ; (1)
	st    Y,       r17                 ; (2-3)
	cp    r17,     r21                 ; (4)
	adc   YL,      r1                  ; (5)
	adc   YH,      r1                  ; (6)
	andi  YH,      0x0F                ; (7)
	ori   YH,      0x08                ; (8)
	nop                                ; (9-14)
	nop
	nop
	nop
	nop
	nop
	dec   r18                          ; (15)
	brne  f_slot                       ; (16-17)
	rjmp  f_check

f_stop:                                ; a byte is being clocked: let it in
	ldi   r16,     6
1:	dec   r16
	brne  1b
	in    r17,     SPDR_IO
	st    Y,       r17
	cp    r17,     r21
	adc   YL,      r1
	adc   YH,      r1
	andi  YH,      0x0F
	ori   YH,      0x08
	ldi   r16,     (1 << MSTR)         ; the SPI off: the clock stops, the card stays selected
	out   _SFR_IO_ADDR(SPCR), r16
	movw  r24,     YL                  ; stored: (Y - start) & 0x7FF
	sub   r24,     XL
	sbc   r25,     XH
	andi  r25,     0x07
	lds   r16,     uzm+UZM_O_PC
	add   r16,     r24
	sts   uzm+UZM_O_PC, r16
	lds   r16,     uzm+UZM_O_PC+1
	adc   r16,     r25
	sts   uzm+UZM_O_PC+1, r16
	sts   uzm+UZM_O_Y, YL
	sts   uzm+UZM_O_Y+1, YH
	pop   r29
	pop   r28
	pop   r17
	pop   r16
	ret
