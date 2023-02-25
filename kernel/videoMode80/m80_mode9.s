
;
; Mode 80 tileset; 18 cycles wide, 8 pixels tall, 4864 words
;


#include <avr/io.h>
#define  PIXOUT _SFR_IO_ADDR(PORTC)
#define  M80_CODEBLOCK_SIZE 2
#define  M80_TILE_CYCLES 18


.global m80_tilerows
.global m80_tilerow_0
.global m80_tilerow_1
.global m80_tilerow_2
.global m80_tilerow_3
.global m80_tilerow_4
.global m80_tilerow_5
.global m80_tilerow_6
.global m80_tilerow_7


.section .text

;
; Note: Due to the inadequacy of the compiler, it is not possible to
; resolve addresses here. This array so provides relative offsets only
; (high byte) to m80_tilerow_0.
;

m80_tilerows:
	.byte 0x00
	.byte 0x03
	.byte 0x05
	.byte 0x08
	.byte 0x0A
	.byte 0x0C
	.byte 0x0E
	.byte 0x11
.balign 2



.section .text.Aligned512
.balign 512


m80_tilerow_0:

	out   PIXOUT,  r2
	rjmp  .+1080
	out   PIXOUT,  r2
	rjmp  .+1094
	out   PIXOUT,  r3
	rjmp  .+1050
	out   PIXOUT,  r2
	rjmp  .+1068
	out   PIXOUT,  r2
	rjmp  .+1102
	out   PIXOUT,  r3
	rjmp  .+1106
	out   PIXOUT,  r2
	rjmp  .+1094
	out   PIXOUT,  r2
	rjmp  .+1120
	out   PIXOUT,  r2
	rjmp  .+1066
	out   PIXOUT,  r2
	rjmp  .+1082
	out   PIXOUT,  r2
	rjmp  .+1040
	out   PIXOUT,  r2
	rjmp  .+1036
	out   PIXOUT,  r2
	rjmp  .+1032
	out   PIXOUT,  r2
	rjmp  .+1028
	out   PIXOUT,  r2
	rjmp  .+1024
	out   PIXOUT,  r2
	rjmp  .+1020
	out   PIXOUT,  r2
	rjmp  .+1104
	out   PIXOUT,  r2
	rjmp  .+1120
	out   PIXOUT,  r2
	rjmp  .+1096
	out   PIXOUT,  r2
	rjmp  .+1092
	out   PIXOUT,  r2
	rjmp  .+940
	out   PIXOUT,  r4
	rjmp  .+956
	out   PIXOUT,  r2
	rjmp  .+1108
	out   PIXOUT,  r4
	rjmp  .+948
	out   PIXOUT,  r2
	rjmp  .+1072
	out   PIXOUT,  r2
	rjmp  .+1068
	out   PIXOUT,  r2
	rjmp  .+976
	out   PIXOUT,  r2
	rjmp  .+972
	out   PIXOUT,  r2
	rjmp  .+1106
	out   PIXOUT,  r2
	rjmp  .+964
	out   PIXOUT,  r2
	rjmp  .+998
	out   PIXOUT,  r2
	rjmp  .+1098
	out   PIXOUT,  r2
	rjmp  .+1094
	out   PIXOUT,  r2
	rjmp  .+1090
	out   PIXOUT,  r3
	rjmp  .+1090
	out   PIXOUT,  r2
	rjmp  .+1082
	out   PIXOUT,  r3
	rjmp  .+1082
	out   PIXOUT,  r3
	rjmp  .+892
	out   PIXOUT,  r3
	rjmp  .+888
	out   PIXOUT,  r2
	rjmp  .+1066
	out   PIXOUT,  r3
	rjmp  .+1070
	out   PIXOUT,  r2
	rjmp  .+1058
	out   PIXOUT,  r2
	rjmp  .+1050
	out   PIXOUT,  r3
	rjmp  .+1058
	out   PIXOUT,  r3
	rjmp  .+1060
	out   PIXOUT,  r3
	rjmp  .+1050
	out   PIXOUT,  r3
	rjmp  .+1046
	out   PIXOUT,  r2
	rjmp  .+1034
	out   PIXOUT,  r3
	rjmp  .+1034
	out   PIXOUT,  r2
	rjmp  .+1026
	out   PIXOUT,  r3
	rjmp  .+1026
	out   PIXOUT,  r2
	rjmp  .+1018
	out   PIXOUT,  r3
	rjmp  .+832
	out   PIXOUT,  r3
	rjmp  .+1018
	out   PIXOUT,  r3
	rjmp  .+1014
	out   PIXOUT,  r3
	rjmp  .+1010
	out   PIXOUT,  r3
	rjmp  .+1006
	out   PIXOUT,  r3
	rjmp  .+1002
	out   PIXOUT,  r3
	rjmp  .+994
	out   PIXOUT,  r2
	rjmp  .+1008
	out   PIXOUT,  r2
	rjmp  .+1004
	out   PIXOUT,  r2
	rjmp  .+904
	out   PIXOUT,  r2
	rjmp  .+850
	out   PIXOUT,  r2
	rjmp  .+828
	out   PIXOUT,  r2
	rjmp  .+892
	out   PIXOUT,  r2
	rjmp  .+820
	out   PIXOUT,  r3
	rjmp  .+972
	out   PIXOUT,  r2
	rjmp  .+812
	out   PIXOUT,  r2
	rjmp  .+946
	out   PIXOUT,  r2
	rjmp  .+804
	out   PIXOUT,  r2
	rjmp  .+964
	out   PIXOUT,  r2
	rjmp  .+796
	out   PIXOUT,  r3
	rjmp  .+948
	out   PIXOUT,  r2
	rjmp  .+806
	out   PIXOUT,  r2
	rjmp  .+970
	out   PIXOUT,  r3
	rjmp  .+936
	out   PIXOUT,  r2
	rjmp  .+794
	out   PIXOUT,  r2
	rjmp  .+772
	out   PIXOUT,  r2
	rjmp  .+768
	out   PIXOUT,  r2
	rjmp  .+764
	out   PIXOUT,  r2
	rjmp  .+760
	out   PIXOUT,  r2
	rjmp  .+756
	out   PIXOUT,  r2
	rjmp  .+752
	out   PIXOUT,  r2
	rjmp  .+748
	out   PIXOUT,  r2
	rjmp  .+744
	out   PIXOUT,  r2
	rjmp  .+740
	out   PIXOUT,  r2
	rjmp  .+736
	out   PIXOUT,  r2
	rjmp  .+732
	out   PIXOUT,  r2
	rjmp  .+728
	out   PIXOUT,  r2
	rjmp  .+724
	out   PIXOUT,  r2
	rjmp  .+720
	out   PIXOUT,  r2
	rjmp  .+716
	out   PIXOUT,  r2
	rjmp  .+712
	out   PIXOUT,  r2
	rjmp  .+708
	out   PIXOUT,  r2
	rjmp  .+704
	out   PIXOUT,  r2
	rjmp  .+700
	out   PIXOUT,  r2
	rjmp  .+696
	out   PIXOUT,  r2
	rjmp  .+692
	out   PIXOUT,  r2
	rjmp  .+688
	out   PIXOUT,  r2
	rjmp  .+684
	out   PIXOUT,  r2
	rjmp  .+680
	out   PIXOUT,  r2
	rjmp  .+676
	out   PIXOUT,  r2
	rjmp  .+672
	out   PIXOUT,  r2
	rjmp  .+668
	out   PIXOUT,  r2
	rjmp  .+664
	out   PIXOUT,  r2
	rjmp  .+660
	out   PIXOUT,  r2
	rjmp  .+656
	out   PIXOUT,  r2
	rjmp  .+652
	out   PIXOUT,  r2
	rjmp  .+648
	out   PIXOUT,  r2
	rjmp  .+644
	out   PIXOUT,  r2
	rjmp  .+640
	out   PIXOUT,  r2
	rjmp  .+636
	out   PIXOUT,  r2
	rjmp  .+632
	out   PIXOUT,  r2
	rjmp  .+628
	out   PIXOUT,  r2
	rjmp  .+624
	out   PIXOUT,  r2
	rjmp  .+620
	out   PIXOUT,  r2
	rjmp  .+616
	out   PIXOUT,  r2
	rjmp  .+612
	out   PIXOUT,  r2
	rjmp  .+608
	out   PIXOUT,  r2
	rjmp  .+604
	out   PIXOUT,  r2
	rjmp  .+600
	out   PIXOUT,  r2
	rjmp  .+596
	out   PIXOUT,  r2
	rjmp  .+592
	out   PIXOUT,  r2
	rjmp  .+588
	out   PIXOUT,  r2
	rjmp  .+584
	out   PIXOUT,  r2
	rjmp  .+580
	out   PIXOUT,  r2
	rjmp  .+576
	out   PIXOUT,  r2
	rjmp  .+572
	out   PIXOUT,  r2
	rjmp  .+568
	out   PIXOUT,  r2
	rjmp  .+564
	out   PIXOUT,  r2
	rjmp  .+560
	out   PIXOUT,  r2
	rjmp  .+556
	out   PIXOUT,  r2
	rjmp  .+552
	out   PIXOUT,  r2
	rjmp  .+548
	out   PIXOUT,  r2
	rjmp  .+544
	out   PIXOUT,  r2
	rjmp  .+540
	out   PIXOUT,  r2
	rjmp  .+536
	out   PIXOUT,  r2
	rjmp  .+532
	out   PIXOUT,  r2
	rjmp  .+528
	out   PIXOUT,  r2
	rjmp  .+524
	out   PIXOUT,  r2
	rjmp  .+520
	out   PIXOUT,  r2
	rjmp  .+516
	out   PIXOUT,  r2
	rjmp  .+512
	out   PIXOUT,  r2
	rjmp  .+508
	out   PIXOUT,  r2
	rjmp  .+504
	out   PIXOUT,  r2
	rjmp  .+500
	out   PIXOUT,  r2
	rjmp  .+496
	out   PIXOUT,  r2
	rjmp  .+492
	out   PIXOUT,  r2
	rjmp  .+488
	out   PIXOUT,  r2
	rjmp  .+484
	out   PIXOUT,  r2
	rjmp  .+480
	out   PIXOUT,  r2
	rjmp  .+476
	out   PIXOUT,  r2
	rjmp  .+472
	out   PIXOUT,  r2
	rjmp  .+468
	out   PIXOUT,  r2
	rjmp  .+464
	out   PIXOUT,  r2
	rjmp  .+460
	out   PIXOUT,  r2
	rjmp  .+456
	out   PIXOUT,  r2
	rjmp  .+452
	out   PIXOUT,  r2
	rjmp  .+448
	out   PIXOUT,  r2
	rjmp  .+444
	out   PIXOUT,  r2
	rjmp  .+440
	out   PIXOUT,  r2
	rjmp  .+436
	out   PIXOUT,  r2
	rjmp  .+432
	out   PIXOUT,  r2
	rjmp  .+428
	out   PIXOUT,  r2
	rjmp  .+424
	out   PIXOUT,  r2
	rjmp  .+420
	out   PIXOUT,  r2
	rjmp  .+416
	out   PIXOUT,  r2
	rjmp  .+412
	out   PIXOUT,  r2
	rjmp  .+408
	out   PIXOUT,  r2
	rjmp  .+404
	out   PIXOUT,  r2
	rjmp  .+400
	out   PIXOUT,  r2
	rjmp  .+396
	out   PIXOUT,  r2
	rjmp  .+392
	out   PIXOUT,  r2
	rjmp  .+388
	out   PIXOUT,  r2
	rjmp  .+384
	out   PIXOUT,  r2
	rjmp  .+380
	out   PIXOUT,  r2
	rjmp  .+376
	out   PIXOUT,  r2
	rjmp  .+372
	out   PIXOUT,  r2
	rjmp  .+368
	out   PIXOUT,  r2
	rjmp  .+364
	out   PIXOUT,  r2
	rjmp  .+360
	out   PIXOUT,  r2
	rjmp  .+356
	out   PIXOUT,  r2
	rjmp  .+352
	out   PIXOUT,  r2
	rjmp  .+348
	out   PIXOUT,  r2
	rjmp  .+344
	out   PIXOUT,  r2
	rjmp  .+340
	out   PIXOUT,  r2
	rjmp  .+336
	out   PIXOUT,  r2
	rjmp  .+332
	out   PIXOUT,  r2
	rjmp  .+328
	out   PIXOUT,  r2
	rjmp  .+324
	out   PIXOUT,  r2
	rjmp  .+320
	out   PIXOUT,  r2
	rjmp  .+316
	out   PIXOUT,  r2
	rjmp  .+312
	out   PIXOUT,  r2
	rjmp  .+308
	out   PIXOUT,  r2
	rjmp  .+304
	out   PIXOUT,  r2
	rjmp  .+300
	out   PIXOUT,  r2
	rjmp  .+296
	out   PIXOUT,  r2
	rjmp  .+292
	out   PIXOUT,  r2
	rjmp  .+288
	out   PIXOUT,  r2
	rjmp  .+284
	out   PIXOUT,  r2
	rjmp  .+280
	out   PIXOUT,  r2
	rjmp  .+276
	out   PIXOUT,  r2
	rjmp  .+272
	out   PIXOUT,  r2
	rjmp  .+268
	out   PIXOUT,  r2
	rjmp  .+264
	out   PIXOUT,  r2
	rjmp  .+260
	out   PIXOUT,  r2
	rjmp  .+256
	out   PIXOUT,  r2
	rjmp  .+252
	out   PIXOUT,  r2
	rjmp  .+248
	out   PIXOUT,  r2
	rjmp  .+244
	out   PIXOUT,  r2
	rjmp  .+240
	out   PIXOUT,  r2
	rjmp  .+236
	out   PIXOUT,  r2
	rjmp  .+232
	out   PIXOUT,  r2
	rjmp  .+228
	out   PIXOUT,  r2
	rjmp  .+224
	out   PIXOUT,  r2
	rjmp  .+220
	out   PIXOUT,  r2
	rjmp  .+216
	out   PIXOUT,  r2
	rjmp  .+212
	out   PIXOUT,  r2
	rjmp  .+208
	out   PIXOUT,  r2
	rjmp  .+204
	out   PIXOUT,  r2
	rjmp  .+200
	out   PIXOUT,  r2
	rjmp  .+196
	out   PIXOUT,  r2
	rjmp  .+192
	out   PIXOUT,  r2
	rjmp  .+188
	out   PIXOUT,  r2
	rjmp  .+184
	out   PIXOUT,  r2
	rjmp  .+180
	out   PIXOUT,  r2
	rjmp  .+176
	out   PIXOUT,  r2
	rjmp  .+172
	out   PIXOUT,  r2
	rjmp  .+168
	out   PIXOUT,  r2
	rjmp  .+164
	out   PIXOUT,  r2
	rjmp  .+160
	out   PIXOUT,  r2
	rjmp  .+156
	out   PIXOUT,  r2
	rjmp  .+152
	out   PIXOUT,  r2
	rjmp  .+148
	out   PIXOUT,  r2
	rjmp  .+144
	out   PIXOUT,  r2
	rjmp  .+140
	out   PIXOUT,  r2
	rjmp  .+136
	out   PIXOUT,  r2
	rjmp  .+132
	out   PIXOUT,  r2
	rjmp  .+128
	out   PIXOUT,  r2
	rjmp  .+124
	out   PIXOUT,  r2
	rjmp  .+120
	out   PIXOUT,  r2
	rjmp  .+116
	out   PIXOUT,  r2
	rjmp  .+112
	out   PIXOUT,  r2
	rjmp  .+108
	out   PIXOUT,  r2
	rjmp  .+104
	out   PIXOUT,  r2
	rjmp  .+100
	out   PIXOUT,  r2
	rjmp  .+96
	out   PIXOUT,  r2
	rjmp  .+92
	out   PIXOUT,  r2
	rjmp  .+88
	out   PIXOUT,  r2
	rjmp  .+84
	out   PIXOUT,  r2
	rjmp  .+80
	out   PIXOUT,  r2
	rjmp  .+76
	out   PIXOUT,  r2
	rjmp  .+72
	out   PIXOUT,  r2
	rjmp  .+68
	out   PIXOUT,  r2
	rjmp  .+64
	out   PIXOUT,  r2
	rjmp  .+60
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r4
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	rjmp  .
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-40
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	rjmp  .
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r4
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r4
	rjmp  .-90
	rjmp  .
	nop
	out   PIXOUT,  r4
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .-90
	out   PIXOUT,  r3
	rjmp  .-54
	rjmp  .
	rjmp  .-28
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .-102
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	rjmp  .-156
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r4
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r4
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r4
	rjmp  .
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r4
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-272
	out   PIXOUT,  r4
	rjmp  .-226
	out   PIXOUT,  r4
	rjmp  .-260
	out   PIXOUT,  r2
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-278
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	out   PIXOUT,  r4
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r4
	rjmp  .
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r4
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r4
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r4
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-456
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	rjmp  .+2050
	rjmp  .+2062

m80_tilerow_1:

	out   PIXOUT,  r2
	rjmp  .-456
	out   PIXOUT,  r2
	rjmp  .-318
	out   PIXOUT,  r3
	rjmp  .-486
	out   PIXOUT,  r2
	rjmp  .-200
	out   PIXOUT,  r2
	rjmp  .-330
	out   PIXOUT,  r3
	rjmp  .-430
	out   PIXOUT,  r3
	rjmp  .-190
	out   PIXOUT,  r2
	rjmp  .-416
	out   PIXOUT,  r2
	rjmp  .-450
	out   PIXOUT,  r2
	rjmp  .-474
	out   PIXOUT,  r2
	rjmp  .-228
	out   PIXOUT,  r2
	rjmp  .-482
	out   PIXOUT,  r2
	rjmp  .-504
	out   PIXOUT,  r2
	rjmp  .-508
	out   PIXOUT,  r2
	rjmp  .-512
	out   PIXOUT,  r2
	rjmp  .-378
	out   PIXOUT,  r4
	rjmp  .-314
	out   PIXOUT,  r2
	rjmp  .-230
	out   PIXOUT,  r4
	rjmp  .-322
	out   PIXOUT,  r4
	rjmp  .-326
	out   PIXOUT,  r2
	rjmp  .-420
	out   PIXOUT,  r4
	rjmp  .-384
	out   PIXOUT,  r2
	rjmp  .-246
	out   PIXOUT,  r2
	rjmp  .-322
	out   PIXOUT,  r4
	rjmp  .-346
	out   PIXOUT,  r4
	rjmp  .-350
	out   PIXOUT,  r2
	rjmp  .-560
	out   PIXOUT,  r2
	rjmp  .-564
	out   PIXOUT,  r2
	rjmp  .-382
	out   PIXOUT,  r2
	rjmp  .-572
	out   PIXOUT,  r2
	rjmp  .-558
	out   PIXOUT,  r3
	rjmp  .-430
	out   PIXOUT,  r3
	rjmp  .-434
	out   PIXOUT,  r3
	rjmp  .-438
	out   PIXOUT,  r3
	rjmp  .-442
	out   PIXOUT,  r3
	rjmp  .-446
	out   PIXOUT,  r3
	rjmp  .-450
	out   PIXOUT,  r3
	rjmp  .-448
	out   PIXOUT,  r3
	rjmp  .-452
	out   PIXOUT,  r3
	rjmp  .-462
	out   PIXOUT,  r3
	rjmp  .-466
	out   PIXOUT,  r2
	rjmp  .-602
	out   PIXOUT,  r2
	rjmp  .-486
	out   PIXOUT,  r3
	rjmp  .-326
	out   PIXOUT,  r3
	rjmp  .-476
	out   PIXOUT,  r3
	rjmp  .-658
	out   PIXOUT,  r3
	rjmp  .-594
	out   PIXOUT,  r3
	rjmp  .-494
	out   PIXOUT,  r3
	rjmp  .-498
	out   PIXOUT,  r3
	rjmp  .-502
	out   PIXOUT,  r3
	rjmp  .-506
	out   PIXOUT,  r3
	rjmp  .-510
	out   PIXOUT,  r2
	rjmp  .-646
	out   PIXOUT,  r3
	rjmp  .-518
	out   PIXOUT,  r3
	rjmp  .-522
	out   PIXOUT,  r3
	rjmp  .-526
	out   PIXOUT,  r3
	rjmp  .-530
	out   PIXOUT,  r3
	rjmp  .-534
	out   PIXOUT,  r2
	rjmp  .-502
	out   PIXOUT,  r2
	rjmp  .-654
	out   PIXOUT,  r2
	rjmp  .-372
	out   PIXOUT,  r2
	rjmp  .-514
	out   PIXOUT,  r2
	rjmp  .-436
	out   PIXOUT,  r2
	rjmp  .-708
	out   PIXOUT,  r2
	rjmp  .-644
	out   PIXOUT,  r2
	rjmp  .-716
	out   PIXOUT,  r3
	rjmp  .-564
	out   PIXOUT,  r2
	rjmp  .-724
	out   PIXOUT,  r2
	rjmp  .-590
	out   PIXOUT,  r2
	rjmp  .-732
	out   PIXOUT,  r2
	rjmp  .-698
	out   PIXOUT,  r2
	rjmp  .-740
	out   PIXOUT,  r3
	rjmp  .-588
	out   PIXOUT,  r2
	rjmp  .-748
	out   PIXOUT,  r2
	rjmp  .-752
	out   PIXOUT,  r3
	rjmp  .-600
	out   PIXOUT,  r2
	rjmp  .-742
	out   PIXOUT,  r2
	rjmp  .-764
	out   PIXOUT,  r2
	rjmp  .-768
	out   PIXOUT,  r2
	rjmp  .-772
	out   PIXOUT,  r2
	rjmp  .-776
	out   PIXOUT,  r2
	rjmp  .-780
	out   PIXOUT,  r2
	rjmp  .-784
	out   PIXOUT,  r2
	rjmp  .-788
	out   PIXOUT,  r2
	rjmp  .-754
	out   PIXOUT,  r2
	rjmp  .-796
	out   PIXOUT,  r2
	rjmp  .-800
	out   PIXOUT,  r2
	rjmp  .-804
	out   PIXOUT,  r2
	rjmp  .-808
	out   PIXOUT,  r2
	rjmp  .-812
	out   PIXOUT,  r2
	rjmp  .-816
	out   PIXOUT,  r2
	rjmp  .-732
	out   PIXOUT,  r2
	rjmp  .-578
	out   PIXOUT,  r2
	rjmp  .-828
	out   PIXOUT,  r2
	rjmp  .-832
	out   PIXOUT,  r2
	rjmp  .-836
	out   PIXOUT,  r2
	rjmp  .-840
	out   PIXOUT,  r2
	rjmp  .-844
	out   PIXOUT,  r2
	rjmp  .-848
	out   PIXOUT,  r2
	rjmp  .-852
	out   PIXOUT,  r2
	rjmp  .-856
	out   PIXOUT,  r2
	rjmp  .-860
	out   PIXOUT,  r2
	rjmp  .-864
	out   PIXOUT,  r2
	rjmp  .-868
	out   PIXOUT,  r2
	rjmp  .-872
	out   PIXOUT,  r2
	rjmp  .-876
	out   PIXOUT,  r2
	rjmp  .-880
	out   PIXOUT,  r2
	rjmp  .-884
	out   PIXOUT,  r2
	rjmp  .-888
	out   PIXOUT,  r2
	rjmp  .-892
	out   PIXOUT,  r2
	rjmp  .-896
	out   PIXOUT,  r2
	rjmp  .-900
	out   PIXOUT,  r2
	rjmp  .-904
	out   PIXOUT,  r2
	rjmp  .-908
	out   PIXOUT,  r2
	rjmp  .-912
	out   PIXOUT,  r2
	rjmp  .-916
	out   PIXOUT,  r2
	rjmp  .-920
	out   PIXOUT,  r2
	rjmp  .-924
	out   PIXOUT,  r2
	rjmp  .-928
	out   PIXOUT,  r2
	rjmp  .-932
	out   PIXOUT,  r2
	rjmp  .-936
	out   PIXOUT,  r2
	rjmp  .-940
	out   PIXOUT,  r2
	rjmp  .-944
	out   PIXOUT,  r2
	rjmp  .-948
	out   PIXOUT,  r2
	rjmp  .-952
	out   PIXOUT,  r2
	rjmp  .-956
	out   PIXOUT,  r2
	rjmp  .-960
	out   PIXOUT,  r2
	rjmp  .-964
	out   PIXOUT,  r2
	rjmp  .-968
	out   PIXOUT,  r2
	rjmp  .-972
	out   PIXOUT,  r2
	rjmp  .-976
	out   PIXOUT,  r2
	rjmp  .-980
	out   PIXOUT,  r2
	rjmp  .-984
	out   PIXOUT,  r2
	rjmp  .-988
	out   PIXOUT,  r2
	rjmp  .-992
	out   PIXOUT,  r2
	rjmp  .-996
	out   PIXOUT,  r2
	rjmp  .-1000
	out   PIXOUT,  r2
	rjmp  .-1004
	out   PIXOUT,  r2
	rjmp  .-1008
	out   PIXOUT,  r2
	rjmp  .-1012
	out   PIXOUT,  r2
	rjmp  .-1016
	out   PIXOUT,  r2
	rjmp  .-1020
	out   PIXOUT,  r2
	rjmp  .-1024
	out   PIXOUT,  r2
	rjmp  .-1028
	out   PIXOUT,  r2
	rjmp  .-1032
	out   PIXOUT,  r2
	rjmp  .-1036
	out   PIXOUT,  r2
	rjmp  .-1040
	out   PIXOUT,  r2
	rjmp  .-1044
	out   PIXOUT,  r2
	rjmp  .-1048
	out   PIXOUT,  r2
	rjmp  .-1052
	out   PIXOUT,  r2
	rjmp  .-1056
	out   PIXOUT,  r2
	rjmp  .-1060
	out   PIXOUT,  r2
	rjmp  .-1064
	out   PIXOUT,  r2
	rjmp  .-1068
	out   PIXOUT,  r2
	rjmp  .-1072
	out   PIXOUT,  r2
	rjmp  .-1076
	out   PIXOUT,  r2
	rjmp  .-1080
	out   PIXOUT,  r2
	rjmp  .-1084
	out   PIXOUT,  r2
	rjmp  .-1088
	out   PIXOUT,  r2
	rjmp  .-1092
	out   PIXOUT,  r2
	rjmp  .-1096
	out   PIXOUT,  r2
	rjmp  .-1100
	out   PIXOUT,  r2
	rjmp  .-1104
	out   PIXOUT,  r2
	rjmp  .-1108
	out   PIXOUT,  r2
	rjmp  .-1112
	out   PIXOUT,  r2
	rjmp  .-1116
	out   PIXOUT,  r2
	rjmp  .-1120
	out   PIXOUT,  r2
	rjmp  .-1124
	out   PIXOUT,  r2
	rjmp  .-1128
	out   PIXOUT,  r2
	rjmp  .-1132
	out   PIXOUT,  r2
	rjmp  .-1136
	out   PIXOUT,  r2
	rjmp  .-1140
	out   PIXOUT,  r2
	rjmp  .-1144
	out   PIXOUT,  r2
	rjmp  .-1148
	out   PIXOUT,  r2
	rjmp  .-1152
	out   PIXOUT,  r2
	rjmp  .-1156
	out   PIXOUT,  r2
	rjmp  .-1160
	out   PIXOUT,  r2
	rjmp  .-1164
	out   PIXOUT,  r2
	rjmp  .-1168
	out   PIXOUT,  r2
	rjmp  .-1172
	out   PIXOUT,  r2
	rjmp  .-1176
	out   PIXOUT,  r2
	rjmp  .-1180
	out   PIXOUT,  r2
	rjmp  .-1184
	out   PIXOUT,  r2
	rjmp  .-1188
	out   PIXOUT,  r2
	rjmp  .-1192
	out   PIXOUT,  r2
	rjmp  .-1196
	out   PIXOUT,  r2
	rjmp  .-1200
	out   PIXOUT,  r2
	rjmp  .-1204
	out   PIXOUT,  r2
	rjmp  .-1208
	out   PIXOUT,  r2
	rjmp  .-1212
	out   PIXOUT,  r2
	rjmp  .-1216
	out   PIXOUT,  r2
	rjmp  .-1220
	out   PIXOUT,  r2
	rjmp  .-1224
	out   PIXOUT,  r2
	rjmp  .-1228
	out   PIXOUT,  r2
	rjmp  .-1232
	out   PIXOUT,  r2
	rjmp  .-1236
	out   PIXOUT,  r2
	rjmp  .-1240
	out   PIXOUT,  r2
	rjmp  .-1244
	out   PIXOUT,  r2
	rjmp  .-1248
	out   PIXOUT,  r2
	rjmp  .-1252
	out   PIXOUT,  r2
	rjmp  .-1256
	out   PIXOUT,  r2
	rjmp  .-1260
	out   PIXOUT,  r2
	rjmp  .-1264
	out   PIXOUT,  r2
	rjmp  .-1268
	out   PIXOUT,  r2
	rjmp  .-1272
	out   PIXOUT,  r2
	rjmp  .-1276
	out   PIXOUT,  r2
	rjmp  .-1280
	out   PIXOUT,  r2
	rjmp  .-1284
	out   PIXOUT,  r2
	rjmp  .-1288
	out   PIXOUT,  r2
	rjmp  .-1292
	out   PIXOUT,  r2
	rjmp  .-1296
	out   PIXOUT,  r2
	rjmp  .-1300
	out   PIXOUT,  r2
	rjmp  .-1304
	out   PIXOUT,  r2
	rjmp  .-1308
	out   PIXOUT,  r2
	rjmp  .-1312
	out   PIXOUT,  r2
	rjmp  .-1316
	out   PIXOUT,  r2
	rjmp  .-1320
	out   PIXOUT,  r2
	rjmp  .-1324
	out   PIXOUT,  r2
	rjmp  .-1328
	out   PIXOUT,  r2
	rjmp  .-1332
	out   PIXOUT,  r2
	rjmp  .-1336
	out   PIXOUT,  r2
	rjmp  .-1340
	out   PIXOUT,  r2
	rjmp  .-1344
	out   PIXOUT,  r2
	rjmp  .-1348
	out   PIXOUT,  r2
	rjmp  .-1352
	out   PIXOUT,  r2
	rjmp  .-1356
	out   PIXOUT,  r2
	rjmp  .-1360
	out   PIXOUT,  r2
	rjmp  .-1364
	out   PIXOUT,  r2
	rjmp  .-1368
	out   PIXOUT,  r2
	rjmp  .-1372
	out   PIXOUT,  r2
	rjmp  .-1376
	out   PIXOUT,  r2
	rjmp  .-1380
	out   PIXOUT,  r2
	rjmp  .-1384
	out   PIXOUT,  r2
	rjmp  .-1388
	out   PIXOUT,  r2
	rjmp  .-1392
	out   PIXOUT,  r2
	rjmp  .-1396
	out   PIXOUT,  r2
	rjmp  .-1400
	out   PIXOUT,  r2
	rjmp  .-1404
	out   PIXOUT,  r2
	rjmp  .-1408
	out   PIXOUT,  r2
	rjmp  .-1412
	out   PIXOUT,  r2
	rjmp  .-1416
	out   PIXOUT,  r2
	rjmp  .-1420
	out   PIXOUT,  r2
	rjmp  .-1424
	out   PIXOUT,  r2
	rjmp  .-1428
	out   PIXOUT,  r2
	rjmp  .-1432
	out   PIXOUT,  r2
	rjmp  .-1436
	out   PIXOUT,  r2
	rjmp  .-1440
	out   PIXOUT,  r2
	rjmp  .-1444
	out   PIXOUT,  r2
	rjmp  .-1448
	out   PIXOUT,  r2
	rjmp  .-1452
	out   PIXOUT,  r2
	rjmp  .-1456
	out   PIXOUT,  r2
	rjmp  .-1460
	out   PIXOUT,  r2
	rjmp  .-1464
	out   PIXOUT,  r2
	rjmp  .-1468
	out   PIXOUT,  r2
	rjmp  .-1472
	out   PIXOUT,  r2
	rjmp  .-1476

m80_tilerow_2:

	out   PIXOUT,  r2
	rjmp  .-1480
	out   PIXOUT,  r2
	rjmp  .-1342
	out   PIXOUT,  r3
	rjmp  .-1186
	out   PIXOUT,  r3
	rjmp  .-1532
	out   PIXOUT,  r3
	rjmp  .-1340
	out   PIXOUT,  r2
	rjmp  .-1314
	out   PIXOUT,  r3
	rjmp  .-1214
	out   PIXOUT,  r2
	rjmp  .-1470
	out   PIXOUT,  r2
	rjmp  .-1474
	out   PIXOUT,  r2
	rjmp  .-1498
	out   PIXOUT,  r2
	rjmp  .-1378
	out   PIXOUT,  r2
	rjmp  .-1506
	out   PIXOUT,  r2
	rjmp  .-1528
	out   PIXOUT,  r2
	rjmp  .-1532
	out   PIXOUT,  r2
	rjmp  .-1536
	out   PIXOUT,  r2
	rjmp  .-1354
	out   PIXOUT,  r4
	rjmp  .-1216
	out   PIXOUT,  r2
	rjmp  .-1440
	out   PIXOUT,  r2
	rjmp  .-1326
	out   PIXOUT,  r2
	rjmp  .-1330
	out   PIXOUT,  r2
	rjmp  .-1212
	out   PIXOUT,  r4
	rjmp  .-1408
	out   PIXOUT,  r4
	rjmp  .-1412
	out   PIXOUT,  r2
	rjmp  .-1632
	out   PIXOUT,  r4
	rjmp  .-1370
	out   PIXOUT,  r4
	rjmp  .-1374
	out   PIXOUT,  r2
	rjmp  .-1516
	out   PIXOUT,  r2
	rjmp  .-1520
	out   PIXOUT,  r2
	rjmp  .-1574
	out   PIXOUT,  r3
	rjmp  .-1636
	out   PIXOUT,  r2
	rjmp  .-1414
	out   PIXOUT,  r2
	rjmp  .-1466
	out   PIXOUT,  r3
	rjmp  .-1194
	out   PIXOUT,  r3
	rjmp  .-1462
	out   PIXOUT,  r3
	rjmp  .-1466
	out   PIXOUT,  r3
	rjmp  .-1464
	out   PIXOUT,  r3
	rjmp  .-1474
	out   PIXOUT,  r3
	rjmp  .-1472
	out   PIXOUT,  r3
	rjmp  .-1476
	out   PIXOUT,  r3
	rjmp  .-1480
	out   PIXOUT,  r3
	rjmp  .-1490
	out   PIXOUT,  r2
	rjmp  .-1626
	out   PIXOUT,  r2
	rjmp  .-1510
	out   PIXOUT,  r3
	rjmp  .-1362
	out   PIXOUT,  r3
	rjmp  .-1500
	out   PIXOUT,  r3
	rjmp  .-1238
	out   PIXOUT,  r3
	rjmp  .-1242
	out   PIXOUT,  r3
	rjmp  .-1518
	out   PIXOUT,  r3
	rjmp  .-1522
	out   PIXOUT,  r3
	rjmp  .-1526
	out   PIXOUT,  r3
	rjmp  .-1530
	out   PIXOUT,  r3
	rjmp  .-1528
	out   PIXOUT,  r2
	rjmp  .-1670
	out   PIXOUT,  r3
	rjmp  .-1542
	out   PIXOUT,  r3
	rjmp  .-1546
	out   PIXOUT,  r3
	rjmp  .-1278
	out   PIXOUT,  r2
	rjmp  .-1436
	out   PIXOUT,  r3
	rjmp  .-1558
	out   PIXOUT,  r2
	rjmp  .-1694
	out   PIXOUT,  r2
	rjmp  .-1678
	out   PIXOUT,  r3
	rjmp  .-1328
	out   PIXOUT,  r2
	rjmp  .-1538
	out   PIXOUT,  r3
	rjmp  .-1578
	out   PIXOUT,  r2
	rjmp  .-1732
	out   PIXOUT,  r2
	rjmp  .-1718
	out   PIXOUT,  r2
	rjmp  .-1598
	out   PIXOUT,  r3
	rjmp  .-1598
	out   PIXOUT,  r2
	rjmp  .-1606
	out   PIXOUT,  r2
	rjmp  .-1308
	out   PIXOUT,  r2
	rjmp  .-1614
	out   PIXOUT,  r2
	rjmp  .-1722
	out   PIXOUT,  r2
	rjmp  .-1320
	out   PIXOUT,  r3
	rjmp  .-1318
	out   PIXOUT,  r2
	rjmp  .-1754
	out   PIXOUT,  r2
	rjmp  .-1612
	out   PIXOUT,  r3
	rjmp  .-1478
	out   PIXOUT,  r2
	rjmp  .-1766
	out   PIXOUT,  r3
	rjmp  .+728
	out   PIXOUT,  r3
	rjmp  .-1342
	out   PIXOUT,  r2
	rjmp  .-1654
	out   PIXOUT,  r3
	rjmp  .-1654
	out   PIXOUT,  r2
	rjmp  .-1360
	out   PIXOUT,  r3
	rjmp  .+730
	out   PIXOUT,  r2
	rjmp  .-1670
	out   PIXOUT,  r3
	rjmp  .-1670
	out   PIXOUT,  r3
	rjmp  .-1518
	out   PIXOUT,  r3
	rjmp  .-1674
	out   PIXOUT,  r3
	rjmp  .-1678
	out   PIXOUT,  r3
	rjmp  .-1530
	out   PIXOUT,  r3
	rjmp  .-1534
	out   PIXOUT,  r3
	rjmp  .-1694
	out   PIXOUT,  r2
	rjmp  .-1598
	out   PIXOUT,  r2
	rjmp  .-1478
	out   PIXOUT,  r2
	rjmp  .-1852
	out   PIXOUT,  r2
	rjmp  .-1856
	out   PIXOUT,  r2
	rjmp  .-1860
	out   PIXOUT,  r2
	rjmp  .-1864
	out   PIXOUT,  r2
	rjmp  .-1868
	out   PIXOUT,  r2
	rjmp  .-1872
	out   PIXOUT,  r2
	rjmp  .-1876
	out   PIXOUT,  r2
	rjmp  .-1880
	out   PIXOUT,  r2
	rjmp  .-1884
	out   PIXOUT,  r2
	rjmp  .-1888
	out   PIXOUT,  r2
	rjmp  .-1892
	out   PIXOUT,  r2
	rjmp  .-1896
	out   PIXOUT,  r2
	rjmp  .-1900
	out   PIXOUT,  r2
	rjmp  .-1904
	out   PIXOUT,  r2
	rjmp  .-1908
	out   PIXOUT,  r2
	rjmp  .-1912
	out   PIXOUT,  r2
	rjmp  .-1916
	out   PIXOUT,  r2
	rjmp  .-1920
	out   PIXOUT,  r2
	rjmp  .-1924
	out   PIXOUT,  r2
	rjmp  .-1928
	out   PIXOUT,  r2
	rjmp  .-1932
	out   PIXOUT,  r2
	rjmp  .-1936
	out   PIXOUT,  r2
	rjmp  .-1940
	out   PIXOUT,  r2
	rjmp  .-1944
	out   PIXOUT,  r2
	rjmp  .-1948
	out   PIXOUT,  r2
	rjmp  .-1952
	out   PIXOUT,  r2
	rjmp  .-1956
	out   PIXOUT,  r2
	rjmp  .-1960
	out   PIXOUT,  r2
	rjmp  .-1964
	out   PIXOUT,  r2
	rjmp  .-1968
	out   PIXOUT,  r2
	rjmp  .-1972
	out   PIXOUT,  r2
	rjmp  .-1976
	out   PIXOUT,  r2
	rjmp  .-1980
	out   PIXOUT,  r2
	rjmp  .-1984
	out   PIXOUT,  r2
	rjmp  .-1988
	out   PIXOUT,  r2
	rjmp  .-1992
	out   PIXOUT,  r2
	rjmp  .-1996
	out   PIXOUT,  r2
	rjmp  .-2000
	out   PIXOUT,  r2
	rjmp  .-2004
	out   PIXOUT,  r2
	rjmp  .-2008
	out   PIXOUT,  r2
	rjmp  .-2012
	out   PIXOUT,  r2
	rjmp  .-2016
	out   PIXOUT,  r2
	rjmp  .-2020
	out   PIXOUT,  r2
	rjmp  .-2024
	out   PIXOUT,  r2
	rjmp  .-2028
	out   PIXOUT,  r2
	rjmp  .-2032
	out   PIXOUT,  r2
	rjmp  .-2036
	out   PIXOUT,  r2
	rjmp  .-2040
	out   PIXOUT,  r2
	rjmp  .-2044
	out   PIXOUT,  r2
	rjmp  .-2048
	out   PIXOUT,  r2
	rjmp  .-2052
	out   PIXOUT,  r2
	rjmp  .-2056
	out   PIXOUT,  r2
	rjmp  .-2060
	out   PIXOUT,  r2
	rjmp  .-2064
	out   PIXOUT,  r2
	rjmp  .-2068
	out   PIXOUT,  r2
	rjmp  .-2072
	out   PIXOUT,  r2
	rjmp  .-2076
	out   PIXOUT,  r2
	rjmp  .-2080
	out   PIXOUT,  r2
	rjmp  .-2084
	out   PIXOUT,  r2
	rjmp  .-2088
	out   PIXOUT,  r2
	rjmp  .-2092
	out   PIXOUT,  r2
	rjmp  .-2096
	out   PIXOUT,  r2
	rjmp  .-2100
	out   PIXOUT,  r2
	rjmp  .-2104
	out   PIXOUT,  r2
	rjmp  .-2108
	out   PIXOUT,  r2
	rjmp  .-2112
	out   PIXOUT,  r2
	rjmp  .-2116
	out   PIXOUT,  r2
	rjmp  .-2120
	out   PIXOUT,  r2
	rjmp  .-2124
	out   PIXOUT,  r2
	rjmp  .-2128
	out   PIXOUT,  r2
	rjmp  .-2132
	out   PIXOUT,  r2
	rjmp  .-2136
	out   PIXOUT,  r2
	rjmp  .-2140
	out   PIXOUT,  r2
	rjmp  .-2144
	out   PIXOUT,  r2
	rjmp  .-2148
	out   PIXOUT,  r2
	rjmp  .-2152
	out   PIXOUT,  r2
	rjmp  .-2156
	out   PIXOUT,  r2
	rjmp  .-2160
	out   PIXOUT,  r2
	rjmp  .-2164
	out   PIXOUT,  r2
	rjmp  .-2168
	out   PIXOUT,  r2
	rjmp  .-2172
	out   PIXOUT,  r2
	rjmp  .-2176
	out   PIXOUT,  r2
	rjmp  .-2180
	out   PIXOUT,  r2
	rjmp  .-2184
	out   PIXOUT,  r2
	rjmp  .-2188
	out   PIXOUT,  r2
	rjmp  .-2192
	out   PIXOUT,  r2
	rjmp  .-2196
	out   PIXOUT,  r2
	rjmp  .-2200
	out   PIXOUT,  r2
	rjmp  .-2204
	out   PIXOUT,  r2
	rjmp  .-2208
	out   PIXOUT,  r2
	rjmp  .-2212
	out   PIXOUT,  r2
	rjmp  .-2216
	out   PIXOUT,  r2
	rjmp  .-2220
	out   PIXOUT,  r2
	rjmp  .-2224
	out   PIXOUT,  r2
	rjmp  .-2228
	out   PIXOUT,  r2
	rjmp  .-2232
	out   PIXOUT,  r2
	rjmp  .-2236
	out   PIXOUT,  r2
	rjmp  .-2240
	out   PIXOUT,  r2
	rjmp  .-2244
	out   PIXOUT,  r2
	rjmp  .-2248
	out   PIXOUT,  r2
	rjmp  .-2252
	out   PIXOUT,  r2
	rjmp  .-2256
	out   PIXOUT,  r2
	rjmp  .-2260
	out   PIXOUT,  r2
	rjmp  .-2264
	out   PIXOUT,  r2
	rjmp  .-2268
	out   PIXOUT,  r2
	rjmp  .-2272
	out   PIXOUT,  r2
	rjmp  .-2276
	out   PIXOUT,  r2
	rjmp  .-2280
	out   PIXOUT,  r2
	rjmp  .-2284
	out   PIXOUT,  r2
	rjmp  .-2288
	out   PIXOUT,  r2
	rjmp  .-2292
	out   PIXOUT,  r2
	rjmp  .-2296
	out   PIXOUT,  r2
	rjmp  .-2300
	out   PIXOUT,  r2
	rjmp  .-2304
	out   PIXOUT,  r2
	rjmp  .-2308
	out   PIXOUT,  r2
	rjmp  .-2312
	out   PIXOUT,  r2
	rjmp  .-2316
	out   PIXOUT,  r2
	rjmp  .-2320
	out   PIXOUT,  r2
	rjmp  .-2324
	out   PIXOUT,  r2
	rjmp  .-2328
	out   PIXOUT,  r2
	rjmp  .-2332
	out   PIXOUT,  r2
	rjmp  .-2336
	out   PIXOUT,  r2
	rjmp  .-2340
	out   PIXOUT,  r2
	rjmp  .-2344
	out   PIXOUT,  r2
	rjmp  .-2348
	out   PIXOUT,  r2
	rjmp  .-2352
	out   PIXOUT,  r2
	rjmp  .-2356
	out   PIXOUT,  r2
	rjmp  .-2360
	out   PIXOUT,  r2
	rjmp  .-2364
	out   PIXOUT,  r2
	rjmp  .-2368
	out   PIXOUT,  r2
	rjmp  .-2372
	out   PIXOUT,  r2
	rjmp  .-2376
	out   PIXOUT,  r2
	rjmp  .-2380
	out   PIXOUT,  r2
	rjmp  .-2384
	out   PIXOUT,  r2
	rjmp  .-2388
	out   PIXOUT,  r2
	rjmp  .-2392
	out   PIXOUT,  r2
	rjmp  .-2396
	out   PIXOUT,  r2
	rjmp  .-2400
	out   PIXOUT,  r2
	rjmp  .-2404
	out   PIXOUT,  r2
	rjmp  .-2408
	out   PIXOUT,  r2
	rjmp  .-2412
	out   PIXOUT,  r2
	rjmp  .-2416
	out   PIXOUT,  r2
	rjmp  .-2420
	out   PIXOUT,  r2
	rjmp  .-2424
	out   PIXOUT,  r2
	rjmp  .-2428
	out   PIXOUT,  r2
	rjmp  .-2432
	out   PIXOUT,  r2
	rjmp  .-2436
	out   PIXOUT,  r2
	rjmp  .-2440
	out   PIXOUT,  r2
	rjmp  .-2444
	out   PIXOUT,  r2
	rjmp  .-2448
	out   PIXOUT,  r2
	rjmp  .-2452
	out   PIXOUT,  r2
	rjmp  .-2456
	out   PIXOUT,  r2
	rjmp  .-2460
	out   PIXOUT,  r2
	rjmp  .-2464
	out   PIXOUT,  r2
	rjmp  .-2468
	out   PIXOUT,  r2
	rjmp  .-2472
	out   PIXOUT,  r2
	rjmp  .-2476
	out   PIXOUT,  r2
	rjmp  .-2480
	out   PIXOUT,  r2
	rjmp  .-2484
	out   PIXOUT,  r2
	rjmp  .-2488
	out   PIXOUT,  r2
	rjmp  .-2492
	out   PIXOUT,  r2
	rjmp  .-2496
	out   PIXOUT,  r2
	rjmp  .-2500
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .-2492
	rjmp  .
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2374
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r4
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r4
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r4
	rjmp  .
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r4
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r4
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .-2650
	rjmp  .
	rjmp  .-2656
	out   PIXOUT,  r4
	rjmp  .-2196
	rjmp  .
	nop
	out   PIXOUT,  r2
	rjmp  .-2646
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r4
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r4
	rjmp  .-2428
	rjmp  .-2718
	rjmp  .-2680
	rjmp  .-2664
	rjmp  .-2546
	out   PIXOUT,  r2
	rjmp  .-2536
	out   PIXOUT,  r2
	rjmp  .-2534
	out   PIXOUT,  r3
	rjmp  .-2656
	rjmp  .-2512
	rjmp  .-2554
	out   PIXOUT,  r3
	rjmp  .-2614
	out   PIXOUT,  r3
	rjmp  .-2638
	out   PIXOUT,  r2
	rjmp  .-2504
	out   PIXOUT,  r2
	rjmp  .-2292
	out   PIXOUT,  r2
	rjmp  .-2416
	out   PIXOUT,  r3
	rjmp  .-2454
	rjmp  .-2786
	out   PIXOUT,  r3
	rjmp  .-2284
	rjmp  .-220
	rjmp  .-2626
	out   PIXOUT,  r4
	rjmp  .-2698
	out   PIXOUT,  r3
	rjmp  .-2694
	out   PIXOUT,  r2
	rjmp  .-2726
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

m80_tilerow_3:

	out   PIXOUT,  r2
	rjmp  .-3016
	out   PIXOUT,  r2
	rjmp  .-3002
	out   PIXOUT,  r2
	rjmp  .-3024
	out   PIXOUT,  r2
	rjmp  .-2760
	out   PIXOUT,  r2
	rjmp  .-2964
	out   PIXOUT,  r2
	rjmp  .-3018
	out   PIXOUT,  r2
	rjmp  .-3002
	out   PIXOUT,  r2
	rjmp  .-3044
	out   PIXOUT,  r2
	rjmp  .-3010
	out   PIXOUT,  r2
	rjmp  .-3034
	out   PIXOUT,  r3
	rjmp  .-3096
	out   PIXOUT,  r3
	rjmp  .-3100
	out   PIXOUT,  r2
	rjmp  .-3064
	out   PIXOUT,  r3
	rjmp  .-3108
	out   PIXOUT,  r2
	rjmp  .-3072
	out   PIXOUT,  r2
	rjmp  .-3058
	out   PIXOUT,  r4
	rjmp  .-538
	out   PIXOUT,  r2
	rjmp  .-2976
	out   PIXOUT,  r2
	rjmp  .-2972
	out   PIXOUT,  r2
	rjmp  .-3004
	out   PIXOUT,  r4
	rjmp  .-488
	out   PIXOUT,  r4
	rjmp  .-2954
	out   PIXOUT,  r4
	rjmp  .-2958
	out   PIXOUT,  r2
	rjmp  .-3000
	out   PIXOUT,  r2
	rjmp  .-3024
	out   PIXOUT,  r2
	rjmp  .-476
	out   PIXOUT,  r2
	rjmp  .-3052
	out   PIXOUT,  r2
	rjmp  .-3056
	out   PIXOUT,  r2
	rjmp  .-3090
	out   PIXOUT,  r2
	rjmp  .-3132
	out   PIXOUT,  r2
	rjmp  .-2998
	out   PIXOUT,  r2
	rjmp  .-2976
	out   PIXOUT,  r3
	rjmp  .-2722
	out   PIXOUT,  r3
	rjmp  .-2998
	out   PIXOUT,  r3
	rjmp  .-3006
	out   PIXOUT,  r3
	rjmp  .-3000
	out   PIXOUT,  r3
	rjmp  .-3010
	out   PIXOUT,  r3
	rjmp  .-3018
	out   PIXOUT,  r3
	rjmp  .-3022
	out   PIXOUT,  r3
	rjmp  .-2758
	out   PIXOUT,  r3
	rjmp  .-3216
	out   PIXOUT,  r2
	rjmp  .-3162
	out   PIXOUT,  r2
	rjmp  .-3046
	out   PIXOUT,  r3
	rjmp  .-544
	out   PIXOUT,  r3
	rjmp  .-3036
	out   PIXOUT,  r3
	rjmp  .-3046
	out   PIXOUT,  r3
	rjmp  .-570
	out   PIXOUT,  r3
	rjmp  .-3054
	out   PIXOUT,  r3
	rjmp  .-3062
	out   PIXOUT,  r3
	rjmp  .-3062
	out   PIXOUT,  r3
	rjmp  .-3070
	out   PIXOUT,  r2
	rjmp  .-3078
	out   PIXOUT,  r2
	rjmp  .-3206
	out   PIXOUT,  r3
	rjmp  .-3078
	out   PIXOUT,  r3
	rjmp  .-3082
	out   PIXOUT,  r3
	rjmp  .-2814
	out   PIXOUT,  r2
	rjmp  .-3222
	out   PIXOUT,  r2
	rjmp  .-2976
	out   PIXOUT,  r2
	rjmp  .-3210
	out   PIXOUT,  r3
	rjmp  .-608
	out   PIXOUT,  r3
	rjmp  .-692
	out   PIXOUT,  r2
	rjmp  .-624
	out   PIXOUT,  r2
	rjmp  .-3264
	out   PIXOUT,  r2
	rjmp  .-3268
	out   PIXOUT,  r2
	rjmp  .-3272
	out   PIXOUT,  r2
	rjmp  .-3138
	out   PIXOUT,  r3
	rjmp  .-3130
	out   PIXOUT,  r3
	rjmp  .-3134
	out   PIXOUT,  r3
	rjmp  .-3138
	out   PIXOUT,  r3
	rjmp  .-3142
	out   PIXOUT,  r3
	rjmp  .-3150
	out   PIXOUT,  r3
	rjmp  .-3150
	out   PIXOUT,  r3
	rjmp  .-3002
	out   PIXOUT,  r2
	rjmp  .-3290
	out   PIXOUT,  r2
	rjmp  .-3126
	out   PIXOUT,  r3
	rjmp  .-3026
	out   PIXOUT,  r2
	rjmp  .-3302
	out   PIXOUT,  r3
	rjmp  .-2902
	out   PIXOUT,  r3
	rjmp  .-3026
	out   PIXOUT,  r3
	rjmp  .-3182
	out   PIXOUT,  r3
	rjmp  .-3186
	out   PIXOUT,  r3
	rjmp  .-3190
	out   PIXOUT,  r2
	rjmp  .-3020
	out   PIXOUT,  r3
	rjmp  .-3192
	out   PIXOUT,  r2
	rjmp  .-3314
	out   PIXOUT,  r3
	rjmp  .-3054
	out   PIXOUT,  r3
	rjmp  .-3210
	out   PIXOUT,  r3
	rjmp  .-3214
	out   PIXOUT,  r3
	rjmp  .-3066
	out   PIXOUT,  r3
	rjmp  .-3070
	out   PIXOUT,  r2
	rjmp  .-3190
	out   PIXOUT,  r2
	rjmp  .-3292
	out   PIXOUT,  r2
	rjmp  .-798
	out   PIXOUT,  r2
	rjmp  .-3388
	out   PIXOUT,  r2
	rjmp  .-3392
	out   PIXOUT,  r2
	rjmp  .-3396
	out   PIXOUT,  r2
	rjmp  .-3400
	out   PIXOUT,  r2
	rjmp  .-3404
	out   PIXOUT,  r2
	rjmp  .-3408
	out   PIXOUT,  r2
	rjmp  .-3412
	out   PIXOUT,  r2
	rjmp  .-3416
	out   PIXOUT,  r2
	rjmp  .-3420
	out   PIXOUT,  r2
	rjmp  .-3424
	out   PIXOUT,  r2
	rjmp  .-3428
	out   PIXOUT,  r2
	rjmp  .-3432
	out   PIXOUT,  r2
	rjmp  .-3436
	out   PIXOUT,  r2
	rjmp  .-3440
	out   PIXOUT,  r2
	rjmp  .-3444
	out   PIXOUT,  r2
	rjmp  .-3448
	out   PIXOUT,  r2
	rjmp  .-3452
	out   PIXOUT,  r2
	rjmp  .-3456
	out   PIXOUT,  r2
	rjmp  .-3460
	out   PIXOUT,  r2
	rjmp  .-3464
	out   PIXOUT,  r2
	rjmp  .-3468
	out   PIXOUT,  r2
	rjmp  .-3472
	out   PIXOUT,  r2
	rjmp  .-3476
	out   PIXOUT,  r2
	rjmp  .-3480
	out   PIXOUT,  r2
	rjmp  .-3484
	out   PIXOUT,  r2
	rjmp  .-3488
	out   PIXOUT,  r2
	rjmp  .-3492
	out   PIXOUT,  r2
	rjmp  .-3496
	out   PIXOUT,  r2
	rjmp  .-3500
	out   PIXOUT,  r2
	rjmp  .-3504
	out   PIXOUT,  r2
	rjmp  .-3508
	out   PIXOUT,  r2
	rjmp  .-3512
	out   PIXOUT,  r2
	rjmp  .-3516
	out   PIXOUT,  r2
	rjmp  .-3520
	out   PIXOUT,  r2
	rjmp  .-3524
	out   PIXOUT,  r2
	rjmp  .-3528
	out   PIXOUT,  r2
	rjmp  .-3532
	out   PIXOUT,  r2
	rjmp  .-3536
	out   PIXOUT,  r2
	rjmp  .-3540
	out   PIXOUT,  r2
	rjmp  .-3544
	out   PIXOUT,  r2
	rjmp  .-3548
	out   PIXOUT,  r2
	rjmp  .-3552
	out   PIXOUT,  r2
	rjmp  .-3556
	out   PIXOUT,  r2
	rjmp  .-3560
	out   PIXOUT,  r2
	rjmp  .-3564
	out   PIXOUT,  r2
	rjmp  .-3568
	out   PIXOUT,  r2
	rjmp  .-3572
	out   PIXOUT,  r2
	rjmp  .-3576
	out   PIXOUT,  r2
	rjmp  .-3580
	out   PIXOUT,  r2
	rjmp  .-3584
	out   PIXOUT,  r2
	rjmp  .-3588
	out   PIXOUT,  r2
	rjmp  .-3592
	out   PIXOUT,  r2
	rjmp  .-3596
	out   PIXOUT,  r2
	rjmp  .-3600
	out   PIXOUT,  r2
	rjmp  .-3604
	out   PIXOUT,  r2
	rjmp  .-3608
	out   PIXOUT,  r2
	rjmp  .-3612
	out   PIXOUT,  r2
	rjmp  .-3616
	out   PIXOUT,  r2
	rjmp  .-3620
	out   PIXOUT,  r2
	rjmp  .-3624
	out   PIXOUT,  r2
	rjmp  .-3628
	out   PIXOUT,  r2
	rjmp  .-3632
	out   PIXOUT,  r2
	rjmp  .-3636
	out   PIXOUT,  r2
	rjmp  .-3640
	out   PIXOUT,  r2
	rjmp  .-3644
	out   PIXOUT,  r2
	rjmp  .-3648
	out   PIXOUT,  r2
	rjmp  .-3652
	out   PIXOUT,  r2
	rjmp  .-3656
	out   PIXOUT,  r2
	rjmp  .-3660
	out   PIXOUT,  r2
	rjmp  .-3664
	out   PIXOUT,  r2
	rjmp  .-3668
	out   PIXOUT,  r2
	rjmp  .-3672
	out   PIXOUT,  r2
	rjmp  .-3676
	out   PIXOUT,  r2
	rjmp  .-3680
	out   PIXOUT,  r2
	rjmp  .-3684
	out   PIXOUT,  r2
	rjmp  .-3688
	out   PIXOUT,  r2
	rjmp  .-3692
	out   PIXOUT,  r2
	rjmp  .-3696
	out   PIXOUT,  r2
	rjmp  .-3700
	out   PIXOUT,  r2
	rjmp  .-3704
	out   PIXOUT,  r2
	rjmp  .-3708
	out   PIXOUT,  r2
	rjmp  .-3712
	out   PIXOUT,  r2
	rjmp  .-3716
	out   PIXOUT,  r2
	rjmp  .-3720
	out   PIXOUT,  r2
	rjmp  .-3724
	out   PIXOUT,  r2
	rjmp  .-3728
	out   PIXOUT,  r2
	rjmp  .-3732
	out   PIXOUT,  r2
	rjmp  .-3736
	out   PIXOUT,  r2
	rjmp  .-3740
	out   PIXOUT,  r2
	rjmp  .-3744
	out   PIXOUT,  r2
	rjmp  .-3748
	out   PIXOUT,  r2
	rjmp  .-3752
	out   PIXOUT,  r2
	rjmp  .-3756
	out   PIXOUT,  r2
	rjmp  .-3760
	out   PIXOUT,  r2
	rjmp  .-3764
	out   PIXOUT,  r2
	rjmp  .-3768
	out   PIXOUT,  r2
	rjmp  .-3772
	out   PIXOUT,  r2
	rjmp  .-3776
	out   PIXOUT,  r2
	rjmp  .-3780
	out   PIXOUT,  r2
	rjmp  .-3784
	out   PIXOUT,  r2
	rjmp  .-3788
	out   PIXOUT,  r2
	rjmp  .-3792
	out   PIXOUT,  r2
	rjmp  .-3796
	out   PIXOUT,  r2
	rjmp  .-3800
	out   PIXOUT,  r2
	rjmp  .-3804
	out   PIXOUT,  r2
	rjmp  .-3808
	out   PIXOUT,  r2
	rjmp  .-3812
	out   PIXOUT,  r2
	rjmp  .-3816
	out   PIXOUT,  r2
	rjmp  .-3820
	out   PIXOUT,  r2
	rjmp  .-3824
	out   PIXOUT,  r2
	rjmp  .-3828
	out   PIXOUT,  r2
	rjmp  .-3832
	out   PIXOUT,  r2
	rjmp  .-3836
	out   PIXOUT,  r2
	rjmp  .-3840
	out   PIXOUT,  r2
	rjmp  .-3844
	out   PIXOUT,  r2
	rjmp  .-3848
	out   PIXOUT,  r2
	rjmp  .-3852
	out   PIXOUT,  r2
	rjmp  .-3856
	out   PIXOUT,  r2
	rjmp  .-3860
	out   PIXOUT,  r2
	rjmp  .-3864
	out   PIXOUT,  r2
	rjmp  .-3868
	out   PIXOUT,  r2
	rjmp  .-3872
	out   PIXOUT,  r2
	rjmp  .-3876
	out   PIXOUT,  r2
	rjmp  .-3880
	out   PIXOUT,  r2
	rjmp  .-3884
	out   PIXOUT,  r2
	rjmp  .-3888
	out   PIXOUT,  r2
	rjmp  .-3892
	out   PIXOUT,  r2
	rjmp  .-3896
	out   PIXOUT,  r2
	rjmp  .-3900
	out   PIXOUT,  r2
	rjmp  .-3904
	out   PIXOUT,  r2
	rjmp  .-3908
	out   PIXOUT,  r2
	rjmp  .-3912
	out   PIXOUT,  r2
	rjmp  .-3916
	out   PIXOUT,  r2
	rjmp  .-3920
	out   PIXOUT,  r2
	rjmp  .-3924
	out   PIXOUT,  r2
	rjmp  .-3928
	out   PIXOUT,  r2
	rjmp  .-3932
	out   PIXOUT,  r2
	rjmp  .-3936
	out   PIXOUT,  r2
	rjmp  .-3940
	out   PIXOUT,  r2
	rjmp  .-3944
	out   PIXOUT,  r2
	rjmp  .-3948
	out   PIXOUT,  r2
	rjmp  .-3952
	out   PIXOUT,  r2
	rjmp  .-3956
	out   PIXOUT,  r2
	rjmp  .-3960
	out   PIXOUT,  r2
	rjmp  .-3964
	out   PIXOUT,  r2
	rjmp  .-3968
	out   PIXOUT,  r2
	rjmp  .-3972
	out   PIXOUT,  r2
	rjmp  .-3976
	out   PIXOUT,  r2
	rjmp  .-3980
	out   PIXOUT,  r2
	rjmp  .-3984
	out   PIXOUT,  r2
	rjmp  .-3988
	out   PIXOUT,  r2
	rjmp  .-3992
	out   PIXOUT,  r2
	rjmp  .-3996
	out   PIXOUT,  r2
	rjmp  .-4000
	out   PIXOUT,  r2
	rjmp  .-4004
	out   PIXOUT,  r2
	rjmp  .-4008
	out   PIXOUT,  r2
	rjmp  .-4012
	out   PIXOUT,  r2
	rjmp  .-4016
	out   PIXOUT,  r2
	rjmp  .-4020
	out   PIXOUT,  r2
	rjmp  .-4024
	out   PIXOUT,  r2
	rjmp  .-4028
	out   PIXOUT,  r2
	rjmp  .-4032
	out   PIXOUT,  r2
	rjmp  .-4036

m80_tilerow_4:

	out   PIXOUT,  r2
	rjmp  .-4040
	out   PIXOUT,  r2
	rjmp  .-4026
	out   PIXOUT,  r2
	rjmp  .-4048
	out   PIXOUT,  r2
	rjmp  .-3784
	out   PIXOUT,  r2
	rjmp  .-3870
	out   PIXOUT,  r2
	rjmp  .-4022
	out   PIXOUT,  r3
	rjmp  .-3642
	out   PIXOUT,  r2
	rjmp  .-4068
	out   PIXOUT,  r2
	rjmp  .-4034
	out   PIXOUT,  r2
	rjmp  .-4058
	out   PIXOUT,  r2
	rjmp  .-3938
	out   PIXOUT,  r2
	rjmp  .-4066
	out   PIXOUT,  r2
	rjmp  .-4088
	out   PIXOUT,  r2
	rjmp  .-4092
	out   PIXOUT,  r2
	rjmp  .-4096
	out   PIXOUT,  r2
	rjmp  .-4062
	out   PIXOUT,  r4
	rjmp  .-1452
	out   PIXOUT,  r2
	rjmp  .-4000
	out   PIXOUT,  r2
	rjmp  .-3814
	out   PIXOUT,  r2
	rjmp  .-3890
	out   PIXOUT,  r4
	rjmp  .-1442
	out   PIXOUT,  r2
	rjmp  .-3898
	out   PIXOUT,  r4
	rjmp  .-3922
	out   PIXOUT,  r2
	rjmp  .-3834
	out   PIXOUT,  r4
	rjmp  .-3930
	out   PIXOUT,  r2
	rjmp  .-3914
	out   PIXOUT,  r2
	rjmp  .-1464
	out   PIXOUT,  r2
	rjmp  .-1468
	out   PIXOUT,  r2
	rjmp  .-1470
	out   PIXOUT,  r2
	rjmp  .-1476
	out   PIXOUT,  r2
	rjmp  .-3974
	out   PIXOUT,  r2
	rjmp  .-1482
	out   PIXOUT,  r3
	rjmp  .-3754
	out   PIXOUT,  r3
	rjmp  .-1494
	out   PIXOUT,  r3
	rjmp  .-4026
	out   PIXOUT,  r3
	rjmp  .-4024
	out   PIXOUT,  r3
	rjmp  .-4034
	out   PIXOUT,  r3
	rjmp  .-4032
	out   PIXOUT,  r3
	rjmp  .-4036
	out   PIXOUT,  r3
	rjmp  .-4046
	out   PIXOUT,  r3
	rjmp  .-4050
	out   PIXOUT,  r2
	rjmp  .-1522
	out   PIXOUT,  r3
	rjmp  .-4058
	out   PIXOUT,  r3
	rjmp  .-3922
	out   PIXOUT,  r3
	rjmp  .-4060
	out   PIXOUT,  r3
	rjmp  .-4070
	out   PIXOUT,  r3
	rjmp  .-4074
	out   PIXOUT,  r3
	rjmp  .-4078
	out   PIXOUT,  r3
	rjmp  .-4076
	out   PIXOUT,  r3
	rjmp  .-3814
	out   PIXOUT,  r3
	rjmp  .-3938
	out   PIXOUT,  r2
	rjmp  .-1560
	out   PIXOUT,  r2
	rjmp  .-1566
	out   PIXOUT,  r3
	rjmp  .-1566
	out   PIXOUT,  r3
	rjmp  .-1570
	out   PIXOUT,  r3
	rjmp  .-3838
	out   PIXOUT,  r2
	rjmp  .-3996
	out   PIXOUT,  r2
	rjmp  .-1586
	out   PIXOUT,  r3
	rjmp  .-1582
	out   PIXOUT,  r2
	rjmp  .-1582
	out   PIXOUT,  r3
	rjmp  .-3888
	out   PIXOUT,  r2
	rjmp  .-1586
	out   PIXOUT,  r2
	rjmp  .-1608
	out   PIXOUT,  r2
	rjmp  .-1612
	out   PIXOUT,  r2
	rjmp  .-1616
	out   PIXOUT,  r2
	rjmp  .-3856
	out   PIXOUT,  r3
	rjmp  .-1618
	out   PIXOUT,  r3
	rjmp  .-1618
	out   PIXOUT,  r3
	rjmp  .-1626
	out   PIXOUT,  r3
	rjmp  .-1616
	out   PIXOUT,  r2
	rjmp  .-1626
	out   PIXOUT,  r3
	rjmp  .-1638
	out   PIXOUT,  r3
	rjmp  .-4026
	out   PIXOUT,  r2
	rjmp  .-1650
	out   PIXOUT,  r2
	rjmp  .-1638
	out   PIXOUT,  r3
	rjmp  .-1696
	out   PIXOUT,  r2
	rjmp  .-1662
	out   PIXOUT,  r3
	rjmp  .-3926
	out   PIXOUT,  r3
	rjmp  .-4050
	out   PIXOUT,  r3
	rjmp  .-1670
	out   PIXOUT,  r3
	rjmp  .-1674
	out   PIXOUT,  r3
	rjmp  .-1678
	out   PIXOUT,  r2
	rjmp  .-1674
	out   PIXOUT,  r2
	rjmp  .-1670
	out   PIXOUT,  r2
	rjmp  .-1682
	out   PIXOUT,  r3
	rjmp  .-4078
	out   PIXOUT,  r3
	rjmp  .-1698
	out   PIXOUT,  r3
	rjmp  .-3966
	out   PIXOUT,  r2
	rjmp  .-1686
	out   PIXOUT,  r3
	rjmp  .-4094
	out   PIXOUT,  r2
	rjmp  .-1694
	out   PIXOUT,  r2
	rjmp  .-1730
	out   PIXOUT,  r2
	rjmp  .-1734
	out   PIXOUT,  r2
	rjmp  .-1732
	out   PIXOUT,  r2
	rjmp  .-1736
	out   PIXOUT,  r2
	rjmp  .-1740
	out   PIXOUT,  r2
	rjmp  .-1744
	out   PIXOUT,  r2
	rjmp  .-1748
	out   PIXOUT,  r2
	rjmp  .-1752
	out   PIXOUT,  r2
	rjmp  .-1756
	out   PIXOUT,  r2
	rjmp  .-1760
	out   PIXOUT,  r2
	rjmp  .-1764
	out   PIXOUT,  r2
	rjmp  .-1768
	out   PIXOUT,  r2
	rjmp  .-1772
	out   PIXOUT,  r2
	rjmp  .-1776
	out   PIXOUT,  r2
	rjmp  .-1780
	out   PIXOUT,  r2
	rjmp  .-1784
	out   PIXOUT,  r2
	rjmp  .-1788
	out   PIXOUT,  r2
	rjmp  .-1792
	out   PIXOUT,  r2
	rjmp  .-1796
	out   PIXOUT,  r2
	rjmp  .-1800
	out   PIXOUT,  r2
	rjmp  .-1804
	out   PIXOUT,  r2
	rjmp  .-1808
	out   PIXOUT,  r2
	rjmp  .-1812
	out   PIXOUT,  r2
	rjmp  .-1816
	out   PIXOUT,  r2
	rjmp  .-1820
	out   PIXOUT,  r2
	rjmp  .-1824
	out   PIXOUT,  r2
	rjmp  .-1828
	out   PIXOUT,  r2
	rjmp  .-1832
	out   PIXOUT,  r2
	rjmp  .-1836
	out   PIXOUT,  r2
	rjmp  .-1840
	out   PIXOUT,  r2
	rjmp  .-1844
	out   PIXOUT,  r2
	rjmp  .-1848
	out   PIXOUT,  r2
	rjmp  .-1852
	out   PIXOUT,  r2
	rjmp  .-1856
	out   PIXOUT,  r2
	rjmp  .-1860
	out   PIXOUT,  r2
	rjmp  .-1864
	out   PIXOUT,  r2
	rjmp  .-1868
	out   PIXOUT,  r2
	rjmp  .-1872
	out   PIXOUT,  r2
	rjmp  .-1876
	out   PIXOUT,  r2
	rjmp  .-1880
	out   PIXOUT,  r2
	rjmp  .-1884
	out   PIXOUT,  r2
	rjmp  .-1888
	out   PIXOUT,  r2
	rjmp  .-1892
	out   PIXOUT,  r2
	rjmp  .-1896
	out   PIXOUT,  r2
	rjmp  .-1900
	out   PIXOUT,  r2
	rjmp  .-1904
	out   PIXOUT,  r2
	rjmp  .-1908
	out   PIXOUT,  r2
	rjmp  .-1912
	out   PIXOUT,  r2
	rjmp  .-1916
	out   PIXOUT,  r2
	rjmp  .-1920
	out   PIXOUT,  r2
	rjmp  .-1924
	out   PIXOUT,  r2
	rjmp  .-1928
	out   PIXOUT,  r2
	rjmp  .-1932
	out   PIXOUT,  r2
	rjmp  .-1936
	out   PIXOUT,  r2
	rjmp  .-1940
	out   PIXOUT,  r2
	rjmp  .-1944
	out   PIXOUT,  r2
	rjmp  .-1948
	out   PIXOUT,  r2
	rjmp  .-1952
	out   PIXOUT,  r2
	rjmp  .-1956
	out   PIXOUT,  r2
	rjmp  .-1960
	out   PIXOUT,  r2
	rjmp  .-1964
	out   PIXOUT,  r2
	rjmp  .-1968
	out   PIXOUT,  r2
	rjmp  .-1972
	out   PIXOUT,  r2
	rjmp  .-1976
	out   PIXOUT,  r2
	rjmp  .-1980
	out   PIXOUT,  r2
	rjmp  .-1984
	out   PIXOUT,  r2
	rjmp  .-1988
	out   PIXOUT,  r2
	rjmp  .-1992
	out   PIXOUT,  r2
	rjmp  .-1996
	out   PIXOUT,  r2
	rjmp  .-2000
	out   PIXOUT,  r2
	rjmp  .-2004
	out   PIXOUT,  r2
	rjmp  .-2008
	out   PIXOUT,  r2
	rjmp  .-2012
	out   PIXOUT,  r2
	rjmp  .-2016
	out   PIXOUT,  r2
	rjmp  .-2020
	out   PIXOUT,  r2
	rjmp  .-2024
	out   PIXOUT,  r2
	rjmp  .-2028
	out   PIXOUT,  r2
	rjmp  .-2032
	out   PIXOUT,  r2
	rjmp  .-2036
	out   PIXOUT,  r2
	rjmp  .-2040
	out   PIXOUT,  r2
	rjmp  .-2044
	out   PIXOUT,  r2
	rjmp  .-2048
	out   PIXOUT,  r2
	rjmp  .-2052
	out   PIXOUT,  r2
	rjmp  .-2056
	out   PIXOUT,  r2
	rjmp  .-2060
	out   PIXOUT,  r2
	rjmp  .-2064
	out   PIXOUT,  r2
	rjmp  .-2068
	out   PIXOUT,  r2
	rjmp  .-2072
	out   PIXOUT,  r2
	rjmp  .-2076
	out   PIXOUT,  r2
	rjmp  .-2080
	out   PIXOUT,  r2
	rjmp  .-2084
	out   PIXOUT,  r2
	rjmp  .-2088
	out   PIXOUT,  r2
	rjmp  .-2092
	out   PIXOUT,  r2
	rjmp  .-2096
	out   PIXOUT,  r2
	rjmp  .-2100
	out   PIXOUT,  r2
	rjmp  .-2104
	out   PIXOUT,  r2
	rjmp  .-2108
	out   PIXOUT,  r2
	rjmp  .-2112
	out   PIXOUT,  r2
	rjmp  .-2116
	out   PIXOUT,  r2
	rjmp  .-2120
	out   PIXOUT,  r2
	rjmp  .-2124
	out   PIXOUT,  r2
	rjmp  .-2128
	out   PIXOUT,  r2
	rjmp  .-2132
	out   PIXOUT,  r2
	rjmp  .-2136
	out   PIXOUT,  r2
	rjmp  .-2140
	out   PIXOUT,  r2
	rjmp  .-2144
	out   PIXOUT,  r2
	rjmp  .-2148
	out   PIXOUT,  r2
	rjmp  .-2152
	out   PIXOUT,  r2
	rjmp  .-2156
	out   PIXOUT,  r2
	rjmp  .-2160
	out   PIXOUT,  r2
	rjmp  .-2164
	out   PIXOUT,  r2
	rjmp  .-2168
	out   PIXOUT,  r2
	rjmp  .-2172
	out   PIXOUT,  r2
	rjmp  .-2176
	out   PIXOUT,  r2
	rjmp  .-2180
	out   PIXOUT,  r2
	rjmp  .-2184
	out   PIXOUT,  r2
	rjmp  .-2188
	out   PIXOUT,  r2
	rjmp  .-2192
	out   PIXOUT,  r2
	rjmp  .-2196
	out   PIXOUT,  r2
	rjmp  .-2200
	out   PIXOUT,  r2
	rjmp  .-2204
	out   PIXOUT,  r2
	rjmp  .-2208
	out   PIXOUT,  r2
	rjmp  .-2212
	out   PIXOUT,  r2
	rjmp  .-2216
	out   PIXOUT,  r2
	rjmp  .-2220
	out   PIXOUT,  r2
	rjmp  .-2224
	out   PIXOUT,  r2
	rjmp  .-2228
	out   PIXOUT,  r2
	rjmp  .-2232
	out   PIXOUT,  r2
	rjmp  .-2236
	out   PIXOUT,  r2
	rjmp  .-2240
	out   PIXOUT,  r2
	rjmp  .-2244
	out   PIXOUT,  r2
	rjmp  .-2248
	out   PIXOUT,  r2
	rjmp  .-2252
	out   PIXOUT,  r2
	rjmp  .-2256
	out   PIXOUT,  r2
	rjmp  .-2260
	out   PIXOUT,  r2
	rjmp  .-2264
	out   PIXOUT,  r2
	rjmp  .-2268
	out   PIXOUT,  r2
	rjmp  .-2272
	out   PIXOUT,  r2
	rjmp  .-2276
	out   PIXOUT,  r2
	rjmp  .-2280
	out   PIXOUT,  r2
	rjmp  .-2284
	out   PIXOUT,  r2
	rjmp  .-2288
	out   PIXOUT,  r2
	rjmp  .-2292
	out   PIXOUT,  r2
	rjmp  .-2296
	out   PIXOUT,  r2
	rjmp  .-2300
	out   PIXOUT,  r2
	rjmp  .-2304
	out   PIXOUT,  r2
	rjmp  .-2308
	out   PIXOUT,  r2
	rjmp  .-2312
	out   PIXOUT,  r2
	rjmp  .-2316
	out   PIXOUT,  r2
	rjmp  .-2320
	out   PIXOUT,  r2
	rjmp  .-2324
	out   PIXOUT,  r2
	rjmp  .-2328
	out   PIXOUT,  r2
	rjmp  .-2332
	out   PIXOUT,  r2
	rjmp  .-2336
	out   PIXOUT,  r2
	rjmp  .-2340
	out   PIXOUT,  r2
	rjmp  .-2344
	out   PIXOUT,  r2
	rjmp  .-2348
	out   PIXOUT,  r2
	rjmp  .-2352
	out   PIXOUT,  r2
	rjmp  .-2356
	out   PIXOUT,  r2
	rjmp  .-2360
	out   PIXOUT,  r2
	rjmp  .-2364
	out   PIXOUT,  r2
	rjmp  .-2368
	out   PIXOUT,  r2
	rjmp  .-2372
	out   PIXOUT,  r2
	rjmp  .-2376
	out   PIXOUT,  r2
	rjmp  .-2380

m80_tilerow_5:

	out   PIXOUT,  r2
	rjmp  .-2384
	out   PIXOUT,  r2
	rjmp  .-2388
	out   PIXOUT,  r2
	rjmp  .-2392
	out   PIXOUT,  r3
	rjmp  .-2398
	out   PIXOUT,  r3
	rjmp  .-2348
	out   PIXOUT,  r3
	rjmp  .-2454
	out   PIXOUT,  r3
	rjmp  .-2370
	out   PIXOUT,  r2
	rjmp  .-2412
	out   PIXOUT,  r2
	rjmp  .-2402
	out   PIXOUT,  r2
	rjmp  .-2418
	out   PIXOUT,  r2
	rjmp  .-2382
	out   PIXOUT,  r2
	rjmp  .-2426
	out   PIXOUT,  r2
	rjmp  .-2406
	out   PIXOUT,  r2
	rjmp  .-2436
	out   PIXOUT,  r2
	rjmp  .-2414
	out   PIXOUT,  r3
	rjmp  .-2434
	out   PIXOUT,  r4
	rjmp  .-2418
	out   PIXOUT,  r2
	rjmp  .-2398
	out   PIXOUT,  r4
	rjmp  .-2446
	out   PIXOUT,  r4
	rjmp  .-2430
	out   PIXOUT,  r2
	rjmp  .-2418
	out   PIXOUT,  r4
	rjmp  .-2438
	out   PIXOUT,  r4
	rjmp  .-2442
	out   PIXOUT,  r2
	rjmp  .-2420
	out   PIXOUT,  r4
	rjmp  .-2450
	out   PIXOUT,  r2
	rjmp  .-2438
	out   PIXOUT,  r2
	rjmp  .-2462
	out   PIXOUT,  r2
	rjmp  .-2466
	out   PIXOUT,  r2
	rjmp  .-2478
	out   PIXOUT,  r3
	rjmp  .-2502
	out   PIXOUT,  r2
	rjmp  .-2502
	out   PIXOUT,  r2
	rjmp  .-2508
	out   PIXOUT,  r3
	rjmp  .-2502
	out   PIXOUT,  r3
	rjmp  .-2510
	out   PIXOUT,  r3
	rjmp  .-2514
	out   PIXOUT,  r3
	rjmp  .-2518
	out   PIXOUT,  r3
	rjmp  .-2522
	out   PIXOUT,  r3
	rjmp  .-2522
	out   PIXOUT,  r3
	rjmp  .-2526
	out   PIXOUT,  r3
	rjmp  .-2534
	out   PIXOUT,  r3
	rjmp  .-2538
	out   PIXOUT,  r2
	rjmp  .-2546
	out   PIXOUT,  r3
	rjmp  .-2546
	out   PIXOUT,  r3
	rjmp  .-2518
	out   PIXOUT,  r3
	rjmp  .-2550
	out   PIXOUT,  r3
	rjmp  .-2558
	out   PIXOUT,  r3
	rjmp  .-2562
	out   PIXOUT,  r3
	rjmp  .-2566
	out   PIXOUT,  r3
	rjmp  .-2566
	out   PIXOUT,  r3
	rjmp  .-2542
	out   PIXOUT,  r3
	rjmp  .-2578
	out   PIXOUT,  r3
	rjmp  .-2582
	out   PIXOUT,  r2
	rjmp  .-2590
	out   PIXOUT,  r3
	rjmp  .-2590
	out   PIXOUT,  r2
	rjmp  .-2558
	out   PIXOUT,  r3
	rjmp  .-2570
	out   PIXOUT,  r3
	rjmp  .-2602
	out   PIXOUT,  r2
	rjmp  .-2610
	out   PIXOUT,  r3
	rjmp  .-2606
	out   PIXOUT,  r2
	rjmp  .-2606
	out   PIXOUT,  r2
	rjmp  .-2564
	out   PIXOUT,  r2
	rjmp  .-2610
	out   PIXOUT,  r2
	rjmp  .-2632
	out   PIXOUT,  r2
	rjmp  .-2636
	out   PIXOUT,  r2
	rjmp  .-2640
	out   PIXOUT,  r3
	rjmp  .-2638
	out   PIXOUT,  r3
	rjmp  .-2642
	out   PIXOUT,  r3
	rjmp  .-2646
	out   PIXOUT,  r3
	rjmp  .-2650
	out   PIXOUT,  r3
	rjmp  .-2650
	out   PIXOUT,  r2
	rjmp  .-2650
	out   PIXOUT,  r2
	rjmp  .-2620
	out   PIXOUT,  r3
	rjmp  .-2634
	out   PIXOUT,  r2
	rjmp  .-2674
	out   PIXOUT,  r2
	rjmp  .-2662
	out   PIXOUT,  r3
	rjmp  .-2620
	out   PIXOUT,  r2
	rjmp  .-2686
	out   PIXOUT,  r3
	rjmp  .-2686
	out   PIXOUT,  r3
	rjmp  .-2658
	out   PIXOUT,  r3
	rjmp  .-2694
	out   PIXOUT,  r3
	rjmp  .-2698
	out   PIXOUT,  r3
	rjmp  .-2702
	out   PIXOUT,  r2
	rjmp  .-2698
	out   PIXOUT,  r2
	rjmp  .-2712
	out   PIXOUT,  r2
	rjmp  .-2678
	out   PIXOUT,  r3
	rjmp  .-2866
	out   PIXOUT,  r2
	rjmp  .-2686
	out   PIXOUT,  r3
	rjmp  .-2734
	out   PIXOUT,  r3
	rjmp  .-2698
	out   PIXOUT,  r2
	rjmp  .-2718
	out   PIXOUT,  r3
	rjmp  .-2734
	out   PIXOUT,  r2
	rjmp  .-2748
	out   PIXOUT,  r2
	rjmp  .-2752
	out   PIXOUT,  r2
	rjmp  .-2756
	out   PIXOUT,  r2
	rjmp  .-2760
	out   PIXOUT,  r2
	rjmp  .-2764
	out   PIXOUT,  r2
	rjmp  .-2768
	out   PIXOUT,  r2
	rjmp  .-2772
	out   PIXOUT,  r2
	rjmp  .-2776
	out   PIXOUT,  r2
	rjmp  .-2780
	out   PIXOUT,  r2
	rjmp  .-2784
	out   PIXOUT,  r2
	rjmp  .-2788
	out   PIXOUT,  r2
	rjmp  .-2792
	out   PIXOUT,  r2
	rjmp  .-2796
	out   PIXOUT,  r2
	rjmp  .-2800
	out   PIXOUT,  r2
	rjmp  .-2804
	out   PIXOUT,  r2
	rjmp  .-2808
	out   PIXOUT,  r2
	rjmp  .-2812
	out   PIXOUT,  r2
	rjmp  .-2816
	out   PIXOUT,  r2
	rjmp  .-2820
	out   PIXOUT,  r2
	rjmp  .-2824
	out   PIXOUT,  r2
	rjmp  .-2828
	out   PIXOUT,  r2
	rjmp  .-2832
	out   PIXOUT,  r2
	rjmp  .-2836
	out   PIXOUT,  r2
	rjmp  .-2840
	out   PIXOUT,  r2
	rjmp  .-2844
	out   PIXOUT,  r2
	rjmp  .-2848
	out   PIXOUT,  r2
	rjmp  .-2852
	out   PIXOUT,  r2
	rjmp  .-2856
	out   PIXOUT,  r2
	rjmp  .-2860
	out   PIXOUT,  r2
	rjmp  .-2864
	out   PIXOUT,  r2
	rjmp  .-2868
	out   PIXOUT,  r2
	rjmp  .-2872
	out   PIXOUT,  r2
	rjmp  .-2876
	out   PIXOUT,  r2
	rjmp  .-2880
	out   PIXOUT,  r2
	rjmp  .-2884
	out   PIXOUT,  r2
	rjmp  .-2888
	out   PIXOUT,  r2
	rjmp  .-2892
	out   PIXOUT,  r2
	rjmp  .-2896
	out   PIXOUT,  r2
	rjmp  .-2900
	out   PIXOUT,  r2
	rjmp  .-2904
	out   PIXOUT,  r2
	rjmp  .-2908
	out   PIXOUT,  r2
	rjmp  .-2912
	out   PIXOUT,  r2
	rjmp  .-2916
	out   PIXOUT,  r2
	rjmp  .-2920
	out   PIXOUT,  r2
	rjmp  .-2924
	out   PIXOUT,  r2
	rjmp  .-2928
	out   PIXOUT,  r2
	rjmp  .-2932
	out   PIXOUT,  r2
	rjmp  .-2936
	out   PIXOUT,  r2
	rjmp  .-2940
	out   PIXOUT,  r2
	rjmp  .-2944
	out   PIXOUT,  r2
	rjmp  .-2948
	out   PIXOUT,  r2
	rjmp  .-2952
	out   PIXOUT,  r2
	rjmp  .-2956
	out   PIXOUT,  r2
	rjmp  .-2960
	out   PIXOUT,  r2
	rjmp  .-2964
	out   PIXOUT,  r2
	rjmp  .-2968
	out   PIXOUT,  r2
	rjmp  .-2972
	out   PIXOUT,  r2
	rjmp  .-2976
	out   PIXOUT,  r2
	rjmp  .-2980
	out   PIXOUT,  r2
	rjmp  .-2984
	out   PIXOUT,  r2
	rjmp  .-2988
	out   PIXOUT,  r2
	rjmp  .-2992
	out   PIXOUT,  r2
	rjmp  .-2996
	out   PIXOUT,  r2
	rjmp  .-3000
	out   PIXOUT,  r2
	rjmp  .-3004
	out   PIXOUT,  r2
	rjmp  .-3008
	out   PIXOUT,  r2
	rjmp  .-3012
	out   PIXOUT,  r2
	rjmp  .-3016
	out   PIXOUT,  r2
	rjmp  .-3020
	out   PIXOUT,  r2
	rjmp  .-3024
	out   PIXOUT,  r2
	rjmp  .-3028
	out   PIXOUT,  r2
	rjmp  .-3032
	out   PIXOUT,  r2
	rjmp  .-3036
	out   PIXOUT,  r2
	rjmp  .-3040
	out   PIXOUT,  r2
	rjmp  .-3044
	out   PIXOUT,  r2
	rjmp  .-3048
	out   PIXOUT,  r2
	rjmp  .-3052
	out   PIXOUT,  r2
	rjmp  .-3056
	out   PIXOUT,  r2
	rjmp  .-3060
	out   PIXOUT,  r2
	rjmp  .-3064
	out   PIXOUT,  r2
	rjmp  .-3068
	out   PIXOUT,  r2
	rjmp  .-3072
	out   PIXOUT,  r2
	rjmp  .-3076
	out   PIXOUT,  r2
	rjmp  .-3080
	out   PIXOUT,  r2
	rjmp  .-3084
	out   PIXOUT,  r2
	rjmp  .-3088
	out   PIXOUT,  r2
	rjmp  .-3092
	out   PIXOUT,  r2
	rjmp  .-3096
	out   PIXOUT,  r2
	rjmp  .-3100
	out   PIXOUT,  r2
	rjmp  .-3104
	out   PIXOUT,  r2
	rjmp  .-3108
	out   PIXOUT,  r2
	rjmp  .-3112
	out   PIXOUT,  r2
	rjmp  .-3116
	out   PIXOUT,  r2
	rjmp  .-3120
	out   PIXOUT,  r2
	rjmp  .-3124
	out   PIXOUT,  r2
	rjmp  .-3128
	out   PIXOUT,  r2
	rjmp  .-3132
	out   PIXOUT,  r2
	rjmp  .-3136
	out   PIXOUT,  r2
	rjmp  .-3140
	out   PIXOUT,  r2
	rjmp  .-3144
	out   PIXOUT,  r2
	rjmp  .-3148
	out   PIXOUT,  r2
	rjmp  .-3152
	out   PIXOUT,  r2
	rjmp  .-3156
	out   PIXOUT,  r2
	rjmp  .-3160
	out   PIXOUT,  r2
	rjmp  .-3164
	out   PIXOUT,  r2
	rjmp  .-3168
	out   PIXOUT,  r2
	rjmp  .-3172
	out   PIXOUT,  r2
	rjmp  .-3176
	out   PIXOUT,  r2
	rjmp  .-3180
	out   PIXOUT,  r2
	rjmp  .-3184
	out   PIXOUT,  r2
	rjmp  .-3188
	out   PIXOUT,  r2
	rjmp  .-3192
	out   PIXOUT,  r2
	rjmp  .-3196
	out   PIXOUT,  r2
	rjmp  .-3200
	out   PIXOUT,  r2
	rjmp  .-3204
	out   PIXOUT,  r2
	rjmp  .-3208
	out   PIXOUT,  r2
	rjmp  .-3212
	out   PIXOUT,  r2
	rjmp  .-3216
	out   PIXOUT,  r2
	rjmp  .-3220
	out   PIXOUT,  r2
	rjmp  .-3224
	out   PIXOUT,  r2
	rjmp  .-3228
	out   PIXOUT,  r2
	rjmp  .-3232
	out   PIXOUT,  r2
	rjmp  .-3236
	out   PIXOUT,  r2
	rjmp  .-3240
	out   PIXOUT,  r2
	rjmp  .-3244
	out   PIXOUT,  r2
	rjmp  .-3248
	out   PIXOUT,  r2
	rjmp  .-3252
	out   PIXOUT,  r2
	rjmp  .-3256
	out   PIXOUT,  r2
	rjmp  .-3260
	out   PIXOUT,  r2
	rjmp  .-3264
	out   PIXOUT,  r2
	rjmp  .-3268
	out   PIXOUT,  r2
	rjmp  .-3272
	out   PIXOUT,  r2
	rjmp  .-3276
	out   PIXOUT,  r2
	rjmp  .-3280
	out   PIXOUT,  r2
	rjmp  .-3284
	out   PIXOUT,  r2
	rjmp  .-3288
	out   PIXOUT,  r2
	rjmp  .-3292
	out   PIXOUT,  r2
	rjmp  .-3296
	out   PIXOUT,  r2
	rjmp  .-3300
	out   PIXOUT,  r2
	rjmp  .-3304
	out   PIXOUT,  r2
	rjmp  .-3308
	out   PIXOUT,  r2
	rjmp  .-3312
	out   PIXOUT,  r2
	rjmp  .-3316
	out   PIXOUT,  r2
	rjmp  .-3320
	out   PIXOUT,  r2
	rjmp  .-3324
	out   PIXOUT,  r2
	rjmp  .-3328
	out   PIXOUT,  r2
	rjmp  .-3332
	out   PIXOUT,  r2
	rjmp  .-3336
	out   PIXOUT,  r2
	rjmp  .-3340
	out   PIXOUT,  r2
	rjmp  .-3344
	out   PIXOUT,  r2
	rjmp  .-3348
	out   PIXOUT,  r2
	rjmp  .-3352
	out   PIXOUT,  r2
	rjmp  .-3356
	out   PIXOUT,  r2
	rjmp  .-3360
	out   PIXOUT,  r2
	rjmp  .-3364
	out   PIXOUT,  r2
	rjmp  .-3368
	out   PIXOUT,  r2
	rjmp  .-3372
	out   PIXOUT,  r2
	rjmp  .-3376
	out   PIXOUT,  r2
	rjmp  .-3380
	out   PIXOUT,  r2
	rjmp  .-3384
	out   PIXOUT,  r2
	rjmp  .-3388
	out   PIXOUT,  r2
	rjmp  .-3392
	out   PIXOUT,  r2
	rjmp  .-3396
	out   PIXOUT,  r2
	rjmp  .-3400
	out   PIXOUT,  r2
	rjmp  .-3404

m80_tilerow_6:

	out   PIXOUT,  r2
	rjmp  .-3408
	out   PIXOUT,  r2
	rjmp  .-3410
	out   PIXOUT,  r2
	rjmp  .-3416
	out   PIXOUT,  r2
	rjmp  .-3378
	out   PIXOUT,  r2
	rjmp  .-3422
	out   PIXOUT,  r3
	rjmp  .-3478
	out   PIXOUT,  r2
	rjmp  .+996
	out   PIXOUT,  r2
	rjmp  .-3436
	out   PIXOUT,  r2
	rjmp  .-3438
	out   PIXOUT,  r2
	rjmp  .-3430
	out   PIXOUT,  r2
	rjmp  .-3448
	out   PIXOUT,  r2
	rjmp  .-3452
	out   PIXOUT,  r2
	rjmp  .-3430
	out   PIXOUT,  r2
	rjmp  .-3460
	out   PIXOUT,  r2
	rjmp  .-3438
	out   PIXOUT,  r2
	rjmp  .-3468
	out   PIXOUT,  r2
	rjmp  .+978
	out   PIXOUT,  r2
	rjmp  .+974
	out   PIXOUT,  r4
	rjmp  .-3482
	out   PIXOUT,  r2
	rjmp  .+966
	out   PIXOUT,  r2
	rjmp  .-3442
	out   PIXOUT,  r2
	rjmp  .+958
	out   PIXOUT,  r2
	rjmp  .+954
	out   PIXOUT,  r2
	rjmp  .-3444
	out   PIXOUT,  r2
	rjmp  .+946
	out   PIXOUT,  r2
	rjmp  .+962
	out   PIXOUT,  r2
	rjmp  .-3486
	out   PIXOUT,  r2
	rjmp  .-3490
	out   PIXOUT,  r2
	rjmp  .-3516
	out   PIXOUT,  r2
	rjmp  .-3524
	out   PIXOUT,  r2
	rjmp  .-3514
	out   PIXOUT,  r2
	rjmp  .-3530
	out   PIXOUT,  r2
	rjmp  .-3514
	out   PIXOUT,  r3
	rjmp  .-3534
	out   PIXOUT,  r3
	rjmp  .-3524
	out   PIXOUT,  r2
	rjmp  .-3526
	out   PIXOUT,  r3
	rjmp  .-3532
	out   PIXOUT,  r3
	rjmp  .-3558
	out   PIXOUT,  r3
	rjmp  .-3550
	out   PIXOUT,  r2
	rjmp  .-3516
	out   PIXOUT,  r3
	rjmp  .-3562
	out   PIXOUT,  r2
	rjmp  .-3550
	out   PIXOUT,  r2
	rjmp  .-3554
	out   PIXOUT,  r3
	rjmp  .-3574
	out   PIXOUT,  r3
	rjmp  .-3586
	out   PIXOUT,  r3
	rjmp  .-3582
	out   PIXOUT,  r3
	rjmp  .-3586
	out   PIXOUT,  r2
	rjmp  .-3574
	out   PIXOUT,  r3
	rjmp  .-3590
	out   PIXOUT,  r2
	rjmp  .+824
	out   PIXOUT,  r3
	rjmp  .-3602
	out   PIXOUT,  r2
	rjmp  .-3590
	out   PIXOUT,  r2
	rjmp  .-3614
	out   PIXOUT,  r2
	rjmp  .-3598
	out   PIXOUT,  r2
	rjmp  .-3622
	out   PIXOUT,  r2
	rjmp  .-3586
	out   PIXOUT,  r3
	rjmp  .-3626
	out   PIXOUT,  r2
	rjmp  .-3634
	out   PIXOUT,  r3
	rjmp  .-3620
	out   PIXOUT,  r2
	rjmp  .+846
	out   PIXOUT,  r2
	rjmp  .+842
	out   PIXOUT,  r2
	rjmp  .-3626
	out   PIXOUT,  r2
	rjmp  .-3656
	out   PIXOUT,  r2
	rjmp  .-3660
	out   PIXOUT,  r2
	rjmp  .-3664
	out   PIXOUT,  r2
	rjmp  .-3620
	out   PIXOUT,  r3
	rjmp  .-3652
	out   PIXOUT,  r2
	rjmp  .-3654
	out   PIXOUT,  r2
	rjmp  .-3632
	out   PIXOUT,  r2
	rjmp  .-3662
	out   PIXOUT,  r2
	rjmp  .-3674
	out   PIXOUT,  r2
	rjmp  .-3688
	out   PIXOUT,  r3
	rjmp  .-3658
	out   PIXOUT,  r2
	rjmp  .+790
	out   PIXOUT,  r3
	rjmp  .-3666
	out   PIXOUT,  r3
	rjmp  .-3670
	out   PIXOUT,  r2
	rjmp  .+778
	out   PIXOUT,  r3
	rjmp  .-3710
	out   PIXOUT,  r3
	rjmp  .-3682
	out   PIXOUT,  r2
	rjmp  .-3702
	out   PIXOUT,  r3
	rjmp  .-3708
	out   PIXOUT,  r2
	rjmp  .-3684
	out   PIXOUT,  r3
	rjmp  .-3684
	out   PIXOUT,  r2
	rjmp  .-3718
	out   PIXOUT,  r2
	rjmp  .-3742
	out   PIXOUT,  r2
	rjmp  .-3706
	out   PIXOUT,  r2
	rjmp  .-3750
	out   PIXOUT,  r2
	rjmp  .-3714
	out   PIXOUT,  r3
	rjmp  .-3722
	out   PIXOUT,  r2
	rjmp  .-3762
	out   PIXOUT,  r3
	rjmp  .-3748
	out   PIXOUT,  r2
	rjmp  .-3812
	out   PIXOUT,  r2
	rjmp  .-3816
	out   PIXOUT,  r2
	rjmp  .-3780
	out   PIXOUT,  r2
	rjmp  .-3784
	out   PIXOUT,  r2
	rjmp  .-3788
	out   PIXOUT,  r2
	rjmp  .-3792
	out   PIXOUT,  r2
	rjmp  .-3796
	out   PIXOUT,  r2
	rjmp  .-3800
	out   PIXOUT,  r2
	rjmp  .-3804
	out   PIXOUT,  r2
	rjmp  .-3808
	out   PIXOUT,  r2
	rjmp  .-3812
	out   PIXOUT,  r2
	rjmp  .-3816
	out   PIXOUT,  r2
	rjmp  .-3820
	out   PIXOUT,  r2
	rjmp  .-3824
	out   PIXOUT,  r2
	rjmp  .-3828
	out   PIXOUT,  r2
	rjmp  .-3832
	out   PIXOUT,  r2
	rjmp  .-3836
	out   PIXOUT,  r2
	rjmp  .-3840
	out   PIXOUT,  r2
	rjmp  .-3844
	out   PIXOUT,  r2
	rjmp  .-3848
	out   PIXOUT,  r2
	rjmp  .-3852
	out   PIXOUT,  r2
	rjmp  .-3856
	out   PIXOUT,  r2
	rjmp  .-3860
	out   PIXOUT,  r2
	rjmp  .-3864
	out   PIXOUT,  r2
	rjmp  .-3868
	out   PIXOUT,  r2
	rjmp  .-3872
	out   PIXOUT,  r2
	rjmp  .-3876
	out   PIXOUT,  r2
	rjmp  .-3880
	out   PIXOUT,  r2
	rjmp  .-3884
	out   PIXOUT,  r2
	rjmp  .-3888
	out   PIXOUT,  r2
	rjmp  .-3892
	out   PIXOUT,  r2
	rjmp  .-3896
	out   PIXOUT,  r2
	rjmp  .-3900
	out   PIXOUT,  r2
	rjmp  .-3904
	out   PIXOUT,  r2
	rjmp  .-3908
	out   PIXOUT,  r2
	rjmp  .-3912
	out   PIXOUT,  r2
	rjmp  .-3916
	out   PIXOUT,  r2
	rjmp  .-3920
	out   PIXOUT,  r2
	rjmp  .-3924
	out   PIXOUT,  r2
	rjmp  .-3928
	out   PIXOUT,  r2
	rjmp  .-3932
	out   PIXOUT,  r2
	rjmp  .-3936
	out   PIXOUT,  r2
	rjmp  .-3940
	out   PIXOUT,  r2
	rjmp  .-3944
	out   PIXOUT,  r2
	rjmp  .-3948
	out   PIXOUT,  r2
	rjmp  .-3952
	out   PIXOUT,  r2
	rjmp  .-3956
	out   PIXOUT,  r2
	rjmp  .-3960
	out   PIXOUT,  r2
	rjmp  .-3964
	out   PIXOUT,  r2
	rjmp  .-3968
	out   PIXOUT,  r2
	rjmp  .-3972
	out   PIXOUT,  r2
	rjmp  .-3976
	out   PIXOUT,  r2
	rjmp  .-3980
	out   PIXOUT,  r2
	rjmp  .-3984
	out   PIXOUT,  r2
	rjmp  .-3988
	out   PIXOUT,  r2
	rjmp  .-3992
	out   PIXOUT,  r2
	rjmp  .-3996
	out   PIXOUT,  r2
	rjmp  .-4000
	out   PIXOUT,  r2
	rjmp  .-4004
	out   PIXOUT,  r2
	rjmp  .-4008
	out   PIXOUT,  r2
	rjmp  .-4012
	out   PIXOUT,  r2
	rjmp  .-4016
	out   PIXOUT,  r2
	rjmp  .-4020
	out   PIXOUT,  r2
	rjmp  .-4024
	out   PIXOUT,  r2
	rjmp  .-4028
	out   PIXOUT,  r2
	rjmp  .-4032
	out   PIXOUT,  r2
	rjmp  .-4036
	out   PIXOUT,  r2
	rjmp  .-4040
	out   PIXOUT,  r2
	rjmp  .-4044
	out   PIXOUT,  r2
	rjmp  .-4048
	out   PIXOUT,  r2
	rjmp  .-4052
	out   PIXOUT,  r2
	rjmp  .-4056
	out   PIXOUT,  r2
	rjmp  .-4060
	out   PIXOUT,  r2
	rjmp  .-4064
	out   PIXOUT,  r2
	rjmp  .-4068
	out   PIXOUT,  r2
	rjmp  .-4072
	out   PIXOUT,  r2
	rjmp  .-4076
	out   PIXOUT,  r2
	rjmp  .-4080
	out   PIXOUT,  r2
	rjmp  .-4084
	out   PIXOUT,  r2
	rjmp  .-4088
	out   PIXOUT,  r2
	rjmp  .-4092
	out   PIXOUT,  r2
	rjmp  .-4096
	out   PIXOUT,  r2
	rjmp  .+412
	out   PIXOUT,  r2
	rjmp  .+408
	out   PIXOUT,  r2
	rjmp  .+404
	out   PIXOUT,  r2
	rjmp  .+400
	out   PIXOUT,  r2
	rjmp  .+396
	out   PIXOUT,  r2
	rjmp  .+392
	out   PIXOUT,  r2
	rjmp  .+388
	out   PIXOUT,  r2
	rjmp  .+384
	out   PIXOUT,  r2
	rjmp  .+380
	out   PIXOUT,  r2
	rjmp  .+376
	out   PIXOUT,  r2
	rjmp  .+372
	out   PIXOUT,  r2
	rjmp  .+368
	out   PIXOUT,  r2
	rjmp  .+364
	out   PIXOUT,  r2
	rjmp  .+360
	out   PIXOUT,  r2
	rjmp  .+356
	out   PIXOUT,  r2
	rjmp  .+352
	out   PIXOUT,  r2
	rjmp  .+348
	out   PIXOUT,  r2
	rjmp  .+344
	out   PIXOUT,  r2
	rjmp  .+340
	out   PIXOUT,  r2
	rjmp  .+336
	out   PIXOUT,  r2
	rjmp  .+332
	out   PIXOUT,  r2
	rjmp  .+328
	out   PIXOUT,  r2
	rjmp  .+324
	out   PIXOUT,  r2
	rjmp  .+320
	out   PIXOUT,  r2
	rjmp  .+316
	out   PIXOUT,  r2
	rjmp  .+312
	out   PIXOUT,  r2
	rjmp  .+308
	out   PIXOUT,  r2
	rjmp  .+304
	out   PIXOUT,  r2
	rjmp  .+300
	out   PIXOUT,  r2
	rjmp  .+296
	out   PIXOUT,  r2
	rjmp  .+292
	out   PIXOUT,  r2
	rjmp  .+288
	out   PIXOUT,  r2
	rjmp  .+284
	out   PIXOUT,  r2
	rjmp  .+280
	out   PIXOUT,  r2
	rjmp  .+276
	out   PIXOUT,  r2
	rjmp  .+272
	out   PIXOUT,  r2
	rjmp  .+268
	out   PIXOUT,  r2
	rjmp  .+264
	out   PIXOUT,  r2
	rjmp  .+260
	out   PIXOUT,  r2
	rjmp  .+256
	out   PIXOUT,  r2
	rjmp  .+252
	out   PIXOUT,  r2
	rjmp  .+248
	out   PIXOUT,  r2
	rjmp  .+244
	out   PIXOUT,  r2
	rjmp  .+240
	out   PIXOUT,  r2
	rjmp  .+236
	out   PIXOUT,  r2
	rjmp  .+232
	out   PIXOUT,  r2
	rjmp  .+228
	out   PIXOUT,  r2
	rjmp  .+224
	out   PIXOUT,  r2
	rjmp  .+220
	out   PIXOUT,  r2
	rjmp  .+216
	out   PIXOUT,  r2
	rjmp  .+212
	out   PIXOUT,  r2
	rjmp  .+208
	out   PIXOUT,  r2
	rjmp  .+204
	out   PIXOUT,  r2
	rjmp  .+200
	out   PIXOUT,  r2
	rjmp  .+196
	out   PIXOUT,  r2
	rjmp  .+192
	out   PIXOUT,  r2
	rjmp  .+188
	out   PIXOUT,  r2
	rjmp  .+184
	out   PIXOUT,  r2
	rjmp  .+180
	out   PIXOUT,  r2
	rjmp  .+176
	out   PIXOUT,  r2
	rjmp  .+172
	out   PIXOUT,  r2
	rjmp  .+168
	out   PIXOUT,  r2
	rjmp  .+164
	out   PIXOUT,  r2
	rjmp  .+160
	out   PIXOUT,  r2
	rjmp  .+156
	out   PIXOUT,  r2
	rjmp  .+152
	out   PIXOUT,  r2
	rjmp  .+148
	out   PIXOUT,  r2
	rjmp  .+144
	out   PIXOUT,  r2
	rjmp  .+140
	out   PIXOUT,  r2
	rjmp  .+136
	out   PIXOUT,  r2
	rjmp  .+132
	out   PIXOUT,  r2
	rjmp  .+128
	out   PIXOUT,  r2
	rjmp  .+124
	out   PIXOUT,  r2
	rjmp  .+120
	out   PIXOUT,  r2
	rjmp  .+116
	out   PIXOUT,  r2
	rjmp  .+112
	out   PIXOUT,  r2
	rjmp  .+108
	out   PIXOUT,  r2
	rjmp  .+104
	out   PIXOUT,  r2
	rjmp  .+100
	out   PIXOUT,  r2
	rjmp  .+96
	out   PIXOUT,  r2
	rjmp  .+92
	out   PIXOUT,  r2
	rjmp  .+88
	out   PIXOUT,  r2
	rjmp  .+84
	out   PIXOUT,  r3
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r4
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r4
	rjmp  .
	rjmp  .
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-20
	out   PIXOUT,  r3
	rjmp  .-88
	out   PIXOUT,  r3
	rjmp  .-72
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	rjmp  .-74
	rjmp  .
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r2
	rjmp  .-64
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

m80_tilerow_7:

	out   PIXOUT,  r2
	rjmp  .-432
	out   PIXOUT,  r2
	rjmp  .-436
	out   PIXOUT,  r2
	rjmp  .-440
	out   PIXOUT,  r2
	rjmp  .-444
	out   PIXOUT,  r2
	rjmp  .-448
	out   PIXOUT,  r2
	rjmp  .-452
	out   PIXOUT,  r2
	rjmp  .-456
	out   PIXOUT,  r2
	rjmp  .-460
	out   PIXOUT,  r2
	rjmp  .-464
	out   PIXOUT,  r2
	rjmp  .-468
	out   PIXOUT,  r2
	rjmp  .-472
	out   PIXOUT,  r2
	rjmp  .-476
	out   PIXOUT,  r2
	rjmp  .-462
	out   PIXOUT,  r2
	rjmp  .-484
	out   PIXOUT,  r2
	rjmp  .-488
	out   PIXOUT,  r2
	rjmp  .-492
	out   PIXOUT,  r2
	rjmp  .-496
	out   PIXOUT,  r2
	rjmp  .-500
	out   PIXOUT,  r2
	rjmp  .-504
	out   PIXOUT,  r2
	rjmp  .-508
	out   PIXOUT,  r2
	rjmp  .-512
	out   PIXOUT,  r2
	rjmp  .-516
	out   PIXOUT,  r2
	rjmp  .-520
	out   PIXOUT,  r2
	rjmp  .-524
	out   PIXOUT,  r2
	rjmp  .-528
	out   PIXOUT,  r2
	rjmp  .-532
	out   PIXOUT,  r2
	rjmp  .-536
	out   PIXOUT,  r2
	rjmp  .-522
	out   PIXOUT,  r2
	rjmp  .-544
	out   PIXOUT,  r2
	rjmp  .-548
	out   PIXOUT,  r2
	rjmp  .-552
	out   PIXOUT,  r2
	rjmp  .-556
	out   PIXOUT,  r2
	rjmp  .-560
	out   PIXOUT,  r2
	rjmp  .-564
	out   PIXOUT,  r2
	rjmp  .-568
	out   PIXOUT,  r2
	rjmp  .-572
	out   PIXOUT,  r2
	rjmp  .-576
	out   PIXOUT,  r2
	rjmp  .-580
	out   PIXOUT,  r2
	rjmp  .-584
	out   PIXOUT,  r2
	rjmp  .-588
	out   PIXOUT,  r2
	rjmp  .-592
	out   PIXOUT,  r2
	rjmp  .-596
	out   PIXOUT,  r2
	rjmp  .-600
	out   PIXOUT,  r2
	rjmp  .-604
	out   PIXOUT,  r2
	rjmp  .-608
	out   PIXOUT,  r2
	rjmp  .-612
	out   PIXOUT,  r2
	rjmp  .-616
	out   PIXOUT,  r2
	rjmp  .-620
	out   PIXOUT,  r2
	rjmp  .-624
	out   PIXOUT,  r2
	rjmp  .-628
	out   PIXOUT,  r2
	rjmp  .-632
	out   PIXOUT,  r2
	rjmp  .-636
	out   PIXOUT,  r2
	rjmp  .-640
	out   PIXOUT,  r2
	rjmp  .-644
	out   PIXOUT,  r2
	rjmp  .-648
	out   PIXOUT,  r2
	rjmp  .-652
	out   PIXOUT,  r2
	rjmp  .-656
	out   PIXOUT,  r2
	rjmp  .-660
	out   PIXOUT,  r2
	rjmp  .-664
	out   PIXOUT,  r2
	rjmp  .-668
	out   PIXOUT,  r2
	rjmp  .-672
	out   PIXOUT,  r2
	rjmp  .-676
	out   PIXOUT,  r2
	rjmp  .-680
	out   PIXOUT,  r3
	rjmp  .-684
	out   PIXOUT,  r2
	rjmp  .-688
	out   PIXOUT,  r2
	rjmp  .-692
	out   PIXOUT,  r2
	rjmp  .-696
	out   PIXOUT,  r2
	rjmp  .-700
	out   PIXOUT,  r2
	rjmp  .-704
	out   PIXOUT,  r2
	rjmp  .-708
	out   PIXOUT,  r2
	rjmp  .-712
	out   PIXOUT,  r2
	rjmp  .-690
	out   PIXOUT,  r2
	rjmp  .-720
	out   PIXOUT,  r2
	rjmp  .-724
	out   PIXOUT,  r2
	rjmp  .-698
	out   PIXOUT,  r2
	rjmp  .-732
	out   PIXOUT,  r2
	rjmp  .-736
	out   PIXOUT,  r2
	rjmp  .-740
	out   PIXOUT,  r2
	rjmp  .-744
	out   PIXOUT,  r2
	rjmp  .-748
	out   PIXOUT,  r3
	rjmp  .-718
	out   PIXOUT,  r2
	rjmp  .-714
	out   PIXOUT,  r2
	rjmp  .-760
	out   PIXOUT,  r2
	rjmp  .-764
	out   PIXOUT,  r2
	rjmp  .-768
	out   PIXOUT,  r2
	rjmp  .-772
	out   PIXOUT,  r2
	rjmp  .-776
	out   PIXOUT,  r2
	rjmp  .-780
	out   PIXOUT,  r2
	rjmp  .-784
	out   PIXOUT,  r3
	rjmp  .-726
	out   PIXOUT,  r2
	rjmp  .-792
	out   PIXOUT,  r2
	rjmp  .-796
	out   PIXOUT,  r2
	rjmp  .-800
	out   PIXOUT,  r2
	rjmp  .-804
	out   PIXOUT,  r2
	rjmp  .-808
	out   PIXOUT,  r2
	rjmp  .-812
	out   PIXOUT,  r2
	rjmp  .-816
	out   PIXOUT,  r2
	rjmp  .-820
	out   PIXOUT,  r2
	rjmp  .-824
	out   PIXOUT,  r2
	rjmp  .-828
	out   PIXOUT,  r2
	rjmp  .-832
	out   PIXOUT,  r2
	rjmp  .-836
	out   PIXOUT,  r2
	rjmp  .-840
	out   PIXOUT,  r2
	rjmp  .-844
	out   PIXOUT,  r2
	rjmp  .-848
	out   PIXOUT,  r2
	rjmp  .-852
	out   PIXOUT,  r2
	rjmp  .-856
	out   PIXOUT,  r2
	rjmp  .-860
	out   PIXOUT,  r2
	rjmp  .-864
	out   PIXOUT,  r2
	rjmp  .-868
	out   PIXOUT,  r2
	rjmp  .-872
	out   PIXOUT,  r2
	rjmp  .-876
	out   PIXOUT,  r2
	rjmp  .-880
	out   PIXOUT,  r2
	rjmp  .-884
	out   PIXOUT,  r2
	rjmp  .-888
	out   PIXOUT,  r2
	rjmp  .-892
	out   PIXOUT,  r2
	rjmp  .-896
	out   PIXOUT,  r2
	rjmp  .-900
	out   PIXOUT,  r2
	rjmp  .-904
	out   PIXOUT,  r2
	rjmp  .-908
	out   PIXOUT,  r2
	rjmp  .-912
	out   PIXOUT,  r2
	rjmp  .-916
	out   PIXOUT,  r2
	rjmp  .-920
	out   PIXOUT,  r2
	rjmp  .-924
	out   PIXOUT,  r2
	rjmp  .-928
	out   PIXOUT,  r2
	rjmp  .-932
	out   PIXOUT,  r2
	rjmp  .-936
	out   PIXOUT,  r2
	rjmp  .-940
	out   PIXOUT,  r2
	rjmp  .-944
	out   PIXOUT,  r2
	rjmp  .-948
	out   PIXOUT,  r2
	rjmp  .-952
	out   PIXOUT,  r2
	rjmp  .-956
	out   PIXOUT,  r2
	rjmp  .-960
	out   PIXOUT,  r2
	rjmp  .-964
	out   PIXOUT,  r2
	rjmp  .-968
	out   PIXOUT,  r2
	rjmp  .-972
	out   PIXOUT,  r2
	rjmp  .-976
	out   PIXOUT,  r2
	rjmp  .-980
	out   PIXOUT,  r2
	rjmp  .-984
	out   PIXOUT,  r2
	rjmp  .-988
	out   PIXOUT,  r2
	rjmp  .-992
	out   PIXOUT,  r2
	rjmp  .-996
	out   PIXOUT,  r2
	rjmp  .-1000
	out   PIXOUT,  r2
	rjmp  .-1004
	out   PIXOUT,  r2
	rjmp  .-1008
	out   PIXOUT,  r2
	rjmp  .-1012
	out   PIXOUT,  r2
	rjmp  .-1016
	out   PIXOUT,  r2
	rjmp  .-1020
	out   PIXOUT,  r2
	rjmp  .-1024
	out   PIXOUT,  r2
	rjmp  .-1028
	out   PIXOUT,  r2
	rjmp  .-1032
	out   PIXOUT,  r2
	rjmp  .-1036
	out   PIXOUT,  r2
	rjmp  .-1040
	out   PIXOUT,  r2
	rjmp  .-1044
	out   PIXOUT,  r2
	rjmp  .-1048
	out   PIXOUT,  r2
	rjmp  .-1052
	out   PIXOUT,  r2
	rjmp  .-1056
	out   PIXOUT,  r2
	rjmp  .-1060
	out   PIXOUT,  r2
	rjmp  .-1064
	out   PIXOUT,  r2
	rjmp  .-1068
	out   PIXOUT,  r2
	rjmp  .-1072
	out   PIXOUT,  r2
	rjmp  .-1076
	out   PIXOUT,  r2
	rjmp  .-1080
	out   PIXOUT,  r2
	rjmp  .-1084
	out   PIXOUT,  r2
	rjmp  .-1088
	out   PIXOUT,  r2
	rjmp  .-1092
	out   PIXOUT,  r2
	rjmp  .-1096
	out   PIXOUT,  r2
	rjmp  .-1100
	out   PIXOUT,  r2
	rjmp  .-1104
	out   PIXOUT,  r2
	rjmp  .-1108
	out   PIXOUT,  r2
	rjmp  .-1112
	out   PIXOUT,  r2
	rjmp  .-1116
	out   PIXOUT,  r2
	rjmp  .-1120
	out   PIXOUT,  r2
	rjmp  .-1124
	out   PIXOUT,  r2
	rjmp  .-1128
	out   PIXOUT,  r2
	rjmp  .-1132
	out   PIXOUT,  r2
	rjmp  .-1136
	out   PIXOUT,  r2
	rjmp  .-1140
	out   PIXOUT,  r2
	rjmp  .-1144
	out   PIXOUT,  r2
	rjmp  .-1148
	out   PIXOUT,  r2
	rjmp  .-1152
	out   PIXOUT,  r2
	rjmp  .-1156
	out   PIXOUT,  r2
	rjmp  .-1160
	out   PIXOUT,  r2
	rjmp  .-1164
	out   PIXOUT,  r2
	rjmp  .-1168
	out   PIXOUT,  r2
	rjmp  .-1172
	out   PIXOUT,  r2
	rjmp  .-1176
	out   PIXOUT,  r2
	rjmp  .-1180
	out   PIXOUT,  r2
	rjmp  .-1184
	out   PIXOUT,  r2
	rjmp  .-1188
	out   PIXOUT,  r2
	rjmp  .-1192
	out   PIXOUT,  r2
	rjmp  .-1196
	out   PIXOUT,  r2
	rjmp  .-1200
	out   PIXOUT,  r2
	rjmp  .-1204
	out   PIXOUT,  r2
	rjmp  .-1208
	out   PIXOUT,  r2
	rjmp  .-1212
	out   PIXOUT,  r2
	rjmp  .-1216
	out   PIXOUT,  r2
	rjmp  .-1220
	out   PIXOUT,  r2
	rjmp  .-1224
	out   PIXOUT,  r2
	rjmp  .-1228
	out   PIXOUT,  r2
	rjmp  .-1232
	out   PIXOUT,  r2
	rjmp  .-1236
	out   PIXOUT,  r2
	rjmp  .-1240
	out   PIXOUT,  r2
	rjmp  .-1244
	out   PIXOUT,  r2
	rjmp  .-1248
	out   PIXOUT,  r2
	rjmp  .-1252
	out   PIXOUT,  r2
	rjmp  .-1256
	out   PIXOUT,  r2
	rjmp  .-1260
	out   PIXOUT,  r2
	rjmp  .-1264
	out   PIXOUT,  r2
	rjmp  .-1268
	out   PIXOUT,  r2
	rjmp  .-1272
	out   PIXOUT,  r2
	rjmp  .-1276
	out   PIXOUT,  r2
	rjmp  .-1280
	out   PIXOUT,  r2
	rjmp  .-1284
	out   PIXOUT,  r2
	rjmp  .-1288
	out   PIXOUT,  r2
	rjmp  .-1292
	out   PIXOUT,  r2
	rjmp  .-1296
	out   PIXOUT,  r2
	rjmp  .-1300
	out   PIXOUT,  r2
	rjmp  .-1304
	out   PIXOUT,  r2
	rjmp  .-1308
	out   PIXOUT,  r2
	rjmp  .-1312
	out   PIXOUT,  r2
	rjmp  .-1316
	out   PIXOUT,  r2
	rjmp  .-1320
	out   PIXOUT,  r2
	rjmp  .-1324
	out   PIXOUT,  r2
	rjmp  .-1328
	out   PIXOUT,  r2
	rjmp  .-1332
	out   PIXOUT,  r2
	rjmp  .-1336
	out   PIXOUT,  r2
	rjmp  .-1340
	out   PIXOUT,  r2
	rjmp  .-1344
	out   PIXOUT,  r2
	rjmp  .-1348
	out   PIXOUT,  r2
	rjmp  .-1352
	out   PIXOUT,  r2
	rjmp  .-1356
	out   PIXOUT,  r2
	rjmp  .-1360
	out   PIXOUT,  r2
	rjmp  .-1364
	out   PIXOUT,  r2
	rjmp  .-1368
	out   PIXOUT,  r2
	rjmp  .-1372
	out   PIXOUT,  r2
	rjmp  .-1376
	out   PIXOUT,  r2
	rjmp  .-1380
	out   PIXOUT,  r2
	rjmp  .-1384
	out   PIXOUT,  r2
	rjmp  .-1388
	out   PIXOUT,  r2
	rjmp  .-1392
	out   PIXOUT,  r2
	rjmp  .-1396
	out   PIXOUT,  r2
	rjmp  .-1400
	out   PIXOUT,  r2
	rjmp  .-1404
	out   PIXOUT,  r2
	rjmp  .-1408
	out   PIXOUT,  r2
	rjmp  .-1412
	out   PIXOUT,  r2
	rjmp  .-1416
	out   PIXOUT,  r2
	rjmp  .-1420
	out   PIXOUT,  r2
	rjmp  .-1424
	out   PIXOUT,  r2
	rjmp  .-1428
	out   PIXOUT,  r2
	rjmp  .-1432
	out   PIXOUT,  r2
	rjmp  .-1436
	out   PIXOUT,  r2
	rjmp  .-1440
	out   PIXOUT,  r2
	rjmp  .-1444
	out   PIXOUT,  r2
	rjmp  .-1448
	out   PIXOUT,  r2
	rjmp  .-1452
