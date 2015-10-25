#include "macros.inc"

.section .renderlinesphasea, "ax", @progbits

.global RenderLines

RenderLines:
     out    _SFR_IO_ADDR(PORTC), r23			; RESET VECTOR

     in     YL, _SFR_IO_ADDR(GPIOR1)			; Test to see if VideoInit has run yet  if not then
     sbrs   YL, 0
     jmp   __init								; jump to __INIT

     out    _SFR_IO_ADDR(PORTC), r23			; Otherwise do pixel code
     rjmp .
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     ld     ZH, Y+
     mov    ZL, ZH
     andi   ZH, 0x0F
Zero_B_End:
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp

     out    _SFR_IO_ADDR(PORTC), r23
     rjmp .
     rjmp .
     out    _SFR_IO_ADDR(PORTC), r23
     ld     YL, X+
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mov    ZL, ZH
     andi   ZH, 0x0F
     rjmp   Zero_B_End


     jmp __vector_13
     jmp __vector_14
     out _SFR_IO_ADDR(PORTC),r2					; output black colour after visable area
     rjmp smalljump

     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp

smalljump:
	 movw	r30, r24							// Load the address we are going to IJMP to
	 IJMP										// when the TCNT1 interrupt happens to end
												// the current scanline

     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp

	jmp	RenderLinesb								// The is just so compiler does not optimize away renderlinesb

     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     YL, X+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y+
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     andi   r18, 0x80
     ori    ZH, 0x20
     ijmp








.macro pop_or_spi_code
	pop		r18
 	//in 		r18,_SFR_IO_ADDR(SPDR)
 	//out 	_SFR_IO_ADDR(SPDR), r14
.endm

.macro pop_or_spi_nop
	nop
	nop
.endm





.section .renderlinesphaseb, "ax", @progbits

.global RenderLinesb

RenderLinesb:
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r23
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r21
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r20
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r23
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r21
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r20
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
     nop
     nop
     out    _SFR_IO_ADDR(PORTC), r18
     in       r18,_SFR_IO_ADDR(SPDR)
     out    _SFR_IO_ADDR(SPDR), r10
     mul    r18, r13
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ld     ZH, Y
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     mov    ZL, ZH
     andi   ZH, 0x0F
     out    _SFR_IO_ADDR(PORTC), r0
     mul    r13, r1
     ijmp
