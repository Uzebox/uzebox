
;
; Mode 80 tileset; 17 cycles wide, 8 pixels tall, 10240 words
;


#include <avr/io.h>
#define  PIXOUT _SFR_IO_ADDR(PORTC)
#define  M80_CODEBLOCK_SIZE 4
#define  M80_TILE_CYCLES 17


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
	.byte 0x06
	.byte 0x0B
	.byte 0x10
	.byte 0x15
	.byte 0x1A
	.byte 0x1F
	.byte 0x24
.balign 2



.section .text.Aligned512
.balign 512


m80_tilerow_0:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2392
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+2258
	rjmp  .+2050
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2262
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2374
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+214
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2352
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+198
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2358
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2364
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2364
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2312
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2304
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2296
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2288
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2280
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+222
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+118
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+2130
	rjmp  .+1954
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+286
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+278
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1422
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2274
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2280
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2258
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+238
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2270
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2184
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2176
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+302
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2160
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2174
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+2240
	rjmp  .+14
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+174
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+2240
	rjmp  .+1840
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1326
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2176
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+270
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2146
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2138
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2144
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2200
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2128
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2200
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+2200
	rjmp  .+14
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2194
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1790
	rjmp  .+1754
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2152
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-130
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1214
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-146
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1198
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+22
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+2136
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2096
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1610
	rjmp  .+6
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1602
	rjmp  .+1684
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2072
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1710
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1978
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2032
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2076
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2016
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2064
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1888
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1902
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1872
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+2050
	rjmp  .+1606
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1856
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1774
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1840
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+2022
	rjmp  .+14
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1824
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+2002
	rjmp  .+1562
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1844
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-250
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1978
	rjmp  .+430
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1896
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1776
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1768
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1760
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1752
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1744
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1736
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1728
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1756
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1712
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1704
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1696
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1688
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1680
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1672
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1590
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1692
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1840
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-746
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1632
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1624
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1616
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1608
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1636
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1628
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1584
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1612
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1604
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1596
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1588
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1544
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1572
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1528
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1520
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1512
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1504
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1496
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1488
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1480
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1472
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1464
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1456
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1448
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1440
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1432
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1424
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1416
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1408
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1400
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1392
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1384
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1376
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1368
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1294
	rjmp  .+1152
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1300
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1542
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-458
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1328
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-474
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1526
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1522
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1522
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1288
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1280
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1272
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1264
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1256
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+14
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-554
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1166
	rjmp  .+110
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+118
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+110
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1174
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1430
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1436
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1414
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+70
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1426
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1160
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1152
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+134
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1136
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1342
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1396
	rjmp  .+922
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+6
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1396
	rjmp  .+78
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1076
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1332
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+142
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1302
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1294
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1300
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1356
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1284
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1356
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1356
	rjmp  .+836
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1350
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+846
	rjmp  .+830
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1308
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-802
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+964
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-818
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+948
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-146
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1292
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1252
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+618
	rjmp  .+6
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+610
	rjmp  .+754
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1228
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+766
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1134
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1188
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1052
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1172
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1216
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+864
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1070
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+848
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1194
	rjmp  .+716
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+832
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+818
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+816
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1166
	rjmp  .+712
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+800
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1146
	rjmp  .+22
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1002
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-458
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1122
	rjmp  .+684
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1052
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+752
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+744
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+736
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+728
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+720
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+712
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+704
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+914
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+688
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+680
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+672
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+664
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+656
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+648
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+634
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+850
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+984
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1642
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+608
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+600
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+592
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+584
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+786
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+560
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+770
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+762
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+754
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+746
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+520
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+730
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+504
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+496
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+488
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+480
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+472
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+464
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+456
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+448
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+440
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+432
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+424
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+416
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+408
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+400
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+392
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+384
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+376
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+368
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+360
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+352
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-32
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-32
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	rjmp  .-718
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-16
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-16
	rjmp  .
	rjmp  .
	rjmp  .-290
	out   PIXOUT,  r2
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-328
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .-172
	rjmp  .
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	rjmp  .
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-96
	out   PIXOUT,  r2
	rjmp  .-24
	nop
	out   PIXOUT,  r2
	rjmp  .-156
	rjmp  .
	rjmp  .-508
	rjmp  .
	rjmp  .-50
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-22
	out   PIXOUT,  r3
	rjmp  .-126
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-170
	out   PIXOUT,  r3
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-192
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-510
	out   PIXOUT,  r3
	rjmp  .-182
	rjmp  .
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	rjmp  .
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-96
	out   PIXOUT,  r3
	rjmp  .-24
	nop
	out   PIXOUT,  r3
	rjmp  .-336
	rjmp  .
	rjmp  .-656
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .-176
	out   PIXOUT,  r3
	rjmp  .-190
	out   PIXOUT,  r2
	rjmp  .-114
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-338
	out   PIXOUT,  r2
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .-496
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	rjmp  .-490
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	rjmp  .
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .-596
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r2
	rjmp  .-574
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-226
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-728
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-432
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .-976
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-522
	nop
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	rjmp  .+2060

m80_tilerow_1:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-680
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-610
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-810
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-698
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-552
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-126
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-722
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-714
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-144
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-152
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-754
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-732
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-776
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-784
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-792
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-194
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-328
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-942
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-696
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-704
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-342
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-684
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-488
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-480
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-744
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-496
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-866
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-874
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-868
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-912
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-296
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-922
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-808
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-848
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-358
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-592
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-466
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-382
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-390
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-624
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-872
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-972
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-410
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-522
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-988
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-414
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-896
	nop
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-576
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-470
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-592
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-486
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-960
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-708
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-976
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-632
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-992
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-648
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-502
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-738
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1130
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1116
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1132
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1080
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1184
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1170
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1200
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1164
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1216
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-618
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1232
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-590
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1248
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1212
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1264
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1272
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1236
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1252
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1296
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1304
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1312
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1320
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1328
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1336
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1344
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1240
	rjmp  .-3482
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1360
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1368
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1376
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1384
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1392
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1400
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-784
	rjmp  .+1410
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1380
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-770
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-870
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1440
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1448
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1456
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1464
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1436
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1444
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1488
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1460
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1468
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1476
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1484
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1528
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1500
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1544
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1552
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1560
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1568
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1576
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1584
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1592
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1600
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1608
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1616
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1624
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1632
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1640
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1648
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1656
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1664
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1672
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1680
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1688
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1696
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1704
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1454
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1772
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1530
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1396
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+976
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1554
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1546
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+958
	rjmp  .-3130
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+950
	rjmp  .-1516
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1586
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1574
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1800
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1808
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1816
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1154
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1310
	rjmp  .-1586
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1906
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1540
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1548
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1324
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1708
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1456
	rjmp  .+62
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1448
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1588
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1464
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1698
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1706
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1710
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1936
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+804
	rjmp  .+1120
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1754
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1652
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1692
	rjmp  .+14
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+744
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1560
	rjmp  .+1090
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1448
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+720
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+712
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1592
	rjmp  .+54
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1716
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1814
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1370
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1504
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1830
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+686
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1740
	rjmp  .+1012
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1558
	rjmp  .+14
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+632
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1574
	rjmp  .+1002
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+616
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1804
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1676
	rjmp  .+1008
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1820
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1614
	rjmp  .+14
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1836
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1630
	rjmp  .-2430
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+598
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1706
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1962
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1958
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1974
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1924
	rjmp  .-2458
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2208
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2002
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2224
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2006
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2240
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1578
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2256
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+510
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2272
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2054
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2288
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2296
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2078
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2094
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2320
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2328
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2336
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2344
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2352
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2360
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2368
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2084
	rjmp  .+54
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2384
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2392
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2400
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2408
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2416
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2424
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+316
	rjmp  .-2700
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2222
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+330
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1878
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2464
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2472
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2480
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2488
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2278
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2286
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2512
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2302
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2310
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2318
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2326
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2552
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2342
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2568
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2576
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2584
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2592
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2600
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2608
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2616
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2624
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2632
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2640
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2648
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2656
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2664
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2672
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2680
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2688
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2696
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2704
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2712
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2720
	out   PIXOUT,  r3
	rjmp  .-2296
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-2854
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2388
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .-3068
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2466
	nop
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-1412
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2346
	out   PIXOUT,  r2
	rjmp  .-3156
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1534
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-2442
	out   PIXOUT,  r3
	rjmp  .-3188
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2896
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-3030
	nop
	out   PIXOUT,  r3
	rjmp  .-3496
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-3454
	nop
	out   PIXOUT,  r3
	rjmp  .-3436
	rjmp  .-4
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	rjmp  .-3218
	out   PIXOUT,  r3
	rjmp  .-3198
	out   PIXOUT,  r2
	rjmp  .-3006
	rjmp  .+2070
	rjmp  .+22

m80_tilerow_2:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3240
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1370
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-460
	rjmp  .-3454
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3214
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-162
	rjmp  .-2962
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2646
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1938
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-480
	rjmp  .-3064
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3282
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2688
	rjmp  .-3490
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3224
	rjmp  .-3154
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3292
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3336
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3344
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3352
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2706
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1250
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-234
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1250
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1258
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-570
	rjmp  .+62
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3244
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3252
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2818
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3304
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3312
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3426
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3434
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2864
	rjmp  .-3652
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3422
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2826
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1290
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-350
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-352
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2918
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3356
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2934
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3492
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3500
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3388
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3432
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3532
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2970
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-556
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3548
	out   PIXOUT,  r3
	rjmp  .-750
	out   PIXOUT,  r2
	rjmp  .-3304
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-438
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3488
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3030
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3504
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3046
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-3604
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3620
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3536
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3666
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3552
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-536
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-826
	rjmp  .-3402
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-528
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3690
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3120
	rjmp  .+1638
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3692
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3730
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3744
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3128
	rjmp  .+1636
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3682
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3690
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1402
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3720
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1418
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3756
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-920
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3738
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3712
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1698
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3246
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3812
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-674
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-682
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2522
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-694
	rjmp  .-4004
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1000
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-988
	rjmp  .+1510
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1938
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3848
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-718
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1014
	rjmp  .-4020
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3808
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3816
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3824
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3910
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3344
	rjmp  .-4038
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3940
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3330
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3992
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4000
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3944
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4016
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4024
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3996
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4004
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4048
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4020
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4028
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4036
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4044
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4088
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4060
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-886
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-894
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-902
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-910
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-918
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-926
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-934
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-942
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-950
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-958
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-966
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-974
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-982
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-990
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-998
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1006
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1014
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1022
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1030
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1038
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1046
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2258
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1362
	rjmp  .+1230
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4058
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+986
	rjmp  .+1228
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1546
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2546
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1382
	rjmp  .+1212
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1108
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1588
	rjmp  .+1206
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-4068
	rjmp  .-4094
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1128
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1142
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1150
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1158
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1606
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1164
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-804
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1178
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1186
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1482
	rjmp  .+62
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4078
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4086
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3778
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1186
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1194
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1252
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1260
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1762
	rjmp  .-1752
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1202
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1726
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1202
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+794
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+792
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1816
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1226
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1832
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+60
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+52
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1258
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1314
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1368
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3930
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1562
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+4
	out   PIXOUT,  r2
	rjmp  .-1662
	out   PIXOUT,  r3
	rjmp  .-4020
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+706
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1370
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1928
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1386
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1944
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-52
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1456
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1418
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1602
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1434
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+608
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1738
	rjmp  .-2626
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+626
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1516
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-2018
	rjmp  .+902
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1528
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1666
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1550
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2028
	rjmp  .+884
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3010
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3018
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1402
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2978
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1418
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-204
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1832
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3066
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1154
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1626
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2144
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1648
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1162
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1170
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3130
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+468
	rjmp  .+784
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1912
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1900
	rjmp  .+838
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2850
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3106
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1194
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1926
	rjmp  .+38
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1690
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1698
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1706
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1690
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2244
	rjmp  .+2884
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1776
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2230
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1798
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1806
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3202
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1822
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1830
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1832
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1840
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1854
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1856
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1864
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1872
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1880
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1894
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1896
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1910
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1918
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1926
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1934
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1942
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1950
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1958
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1966
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1974
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1982
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1990
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1998
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2006
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2014
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2022
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2030
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2038
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2046
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2054
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2062
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-16
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1528
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1520
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-3526
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r2
	rjmp  .-38
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2318
	out   PIXOUT,  r2
	rjmp  .-1214
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	rjmp  .-1240
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	rjmp  .-3534
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .-38
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2926
	out   PIXOUT,  r2
	rjmp  .-2834
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	rjmp  .-2642

m80_tilerow_3:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2582
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-404
	rjmp  .+2094
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2598
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-160
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3458
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2018
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1954
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2638
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+300
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1076
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-930
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-98
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2678
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-954
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2694
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1968
	rjmp  .-2806
	out   PIXOUT,  r3
	rjmp  .-504
	out   PIXOUT,  r3
	rjmp  .-2982
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1012
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-502
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-146
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-296
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2002
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2010
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3786
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3618
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-4034
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2790
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2798
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+140
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2814
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-234
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+900
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-610
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-400
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3698
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2762
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1828
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-178
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-186
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3994
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1170
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+820
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-330
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-740
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+12
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1760
	out   PIXOUT,  r3
	rjmp  .-744
	out   PIXOUT,  r2
	rjmp  .-3438
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+30
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3810
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+14
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3826
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1746
	rjmp  .+1730
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+732
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-18
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3080
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-570
	rjmp  .+1626
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1710
	rjmp  .+1748
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-790
	rjmp  .-3082
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1694
	rjmp  .-946
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1702
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+668
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+660
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-90
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3086
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3094
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1418
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1650
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-130
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-640
	rjmp  .-3612
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-146
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-114
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3166
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-908
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+564
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-138
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-736
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+540
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-928
	rjmp  .-1130
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-916
	rjmp  .+1608
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-762
	rjmp  .+62
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1476
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3246
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2178
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3146
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+476
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3278
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-824
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-826
	rjmp  .+694
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-784
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-306
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2226
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1446
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3318
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-738
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3334
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+388
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1440
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3358
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3366
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+356
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+348
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3390
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+332
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+324
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+316
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+308
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3430
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+292
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3446
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3454
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3462
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3470
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3478
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3486
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3494
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3502
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3510
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3518
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3526
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3534
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3542
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3550
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3558
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3566
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3574
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3582
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3590
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3598
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3606
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1302
	rjmp  .+1280
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3622
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3754
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3458
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1160
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2538
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3662
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2268
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3672
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3610
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-650
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3702
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3634
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3718
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1100
	rjmp  .+1182
	out   PIXOUT,  r2
	rjmp  .-1392
	out   PIXOUT,  r3
	rjmp  .+236
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3736
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1390
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-634
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2570
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2578
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3698
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3618
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2306
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3814
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3822
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2428
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3838
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-722
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3848
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1498
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3994
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3698
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3786
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+938
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-706
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-714
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3908
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3850
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3928
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-818
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1638
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2556
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+870
	out   PIXOUT,  r2
	rjmp  .-1632
	out   PIXOUT,  r2
	rjmp  .-4028
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3930
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3810
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3946
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3826
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+842
	rjmp  .-3978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4016
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3978
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1936
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1524
	rjmp  .+6
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+806
	rjmp  .+692
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1678
	rjmp  .+928
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+790
	rjmp  .+970
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4076
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4080
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4088
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4050
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-898
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-906
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2426
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+742
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4090
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1594
	rjmp  .+822
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-546
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-442
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3642
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1796
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+722
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-186
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1500
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+698
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1816
	rjmp  .+2964
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1804
	rjmp  .+3006
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1716
	rjmp  .+878
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+586
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3722
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2738
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-226
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+634
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3754
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1588
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1780
	rjmp  .+828
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+312
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-706
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2770
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+586
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1130
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1226
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1146
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+546
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+550
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1170
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1178
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+514
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+506
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1202
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+490
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+482
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+474
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+466
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1242
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+450
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1258
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1266
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1274
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1282
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1290
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1298
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1306
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1314
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1322
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1330
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1338
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1346
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1354
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1362
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1370
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1378
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1386
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1394
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1402
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1410
	nop
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2408
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1718
	out   PIXOUT,  r3
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-86
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-156
	out   PIXOUT,  r3
	rjmp  .-2406
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	rjmp  .-1654
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	rjmp  .-130
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-2806
	out   PIXOUT,  r3
	rjmp  .-250
	out   PIXOUT,  r3
	rjmp  .-2756
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .+2134

m80_tilerow_4:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1930
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-480
	rjmp  .+2126
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1946
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3434
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-246
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-234
	rjmp  .+2106
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-222
	rjmp  .-3020
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1986
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-450
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-544
	rjmp  .+6
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-486
	rjmp  .-2712
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1500
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2026
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2034
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2042
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-506
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2068
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1548
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2586
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2258
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3570
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2274
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2330
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2028
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2346
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2306
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2138
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2146
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+2006
	rjmp  .+2064
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2162
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1972
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-720
	rjmp  .+182
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-412
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1346
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-716
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1378
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-732
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2492
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2500
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2466
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2474
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1740
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2890
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-482
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2548
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2514
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1876
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2530
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2580
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-512
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-782
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1482
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1828
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2578
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-838
	rjmp  .+1862
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-528
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1812
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1868
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1814
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-858
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1322
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1900
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2426
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2434
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2442
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1514
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-972
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1634
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-506
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3962
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2756
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-530
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1020
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1996
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2698
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1006
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2020
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1056
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2778
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2786
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1084
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-610
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2852
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-594
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2084
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-642
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1572
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-784
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1110
	rjmp  .+6
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+1450
	rjmp  .-3694
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1122
	rjmp  .+1672
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1200
	rjmp  .+302
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2148
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1468
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2172
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1120
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1508
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-562
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1492
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-578
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2738
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2228
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1460
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-610
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2770
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2778
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2786
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2794
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2802
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2810
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2818
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2826
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2834
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2842
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2850
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2858
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2866
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2874
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2898
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2906
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2914
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2922
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2938
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2946
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2954
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1348
	rjmp  .-1180
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2970
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-802
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1126
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1114
	rjmp  .+1368
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1102
	rjmp  .+1374
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3010
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1310
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1412
	rjmp  .+6
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1390
	rjmp  .+944
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1342
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3050
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3058
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3066
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1366
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1152
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1390
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3114
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2306
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-938
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2322
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2730
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1112
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2746
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2354
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3162
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3170
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1090
	rjmp  .+3352
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3186
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1056
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1588
	rjmp  .+3364
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+760
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2210
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1606
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2402
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1622
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1024
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1016
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2866
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2874
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1582
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3378
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+690
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+968
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2914
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+956
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2930
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+936
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+660
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1690
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2346
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1670
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2978
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1742
	rjmp  .+3270
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+644
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+892
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1710
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+894
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1718
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1498
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1742
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3450
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3458
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3466
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2450
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1862
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2658
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1522
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1330
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+760
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1546
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1910
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1838
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2746
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1910
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1862
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1946
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3178
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3186
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1974
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1626
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+664
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-986
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1926
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1658
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+652
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+388
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-2014
	rjmp  .-2150
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .+444
	rjmp  .-2194
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2026
	rjmp  .+3020
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2068
	rjmp  .-2842
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1990
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+552
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3706
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2014
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2010
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+588
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1090
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+572
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1106
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3762
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2070
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+540
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1138
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3794
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3802
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3810
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3818
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3826
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3834
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3842
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3850
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3858
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3866
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3874
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3882
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3890
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3898
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3906
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3914
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3922
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3930
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3938
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3946
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3954
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3962
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3970
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-2680
	out   PIXOUT,  r3
	rjmp  .-2650
	nop
	out   PIXOUT,  r2
	rjmp  .-2758
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-108
	out   PIXOUT,  r3
	rjmp  .-114
	nop
	out   PIXOUT,  r2
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-2784
	out   PIXOUT,  r2
	rjmp  .-2754
	nop
	out   PIXOUT,  r3
	rjmp  .-2862
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	rjmp  .-3068

m80_tilerow_5:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+542
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+534
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+526
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2156
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1866
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-202
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-192
	rjmp  .+14
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+486
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-394
	rjmp  .-3056
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-402
	rjmp  .-384
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2100
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4060
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3042
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+438
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3058
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+2044
	rjmp  .+6
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+2058
	rjmp  .-3034
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2204
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-978
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-986
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-986
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1002
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-574
	rjmp  .-2984
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2156
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1026
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1698
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3154
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3162
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2116
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+446
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3272
	rjmp  .+2002
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+294
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3914
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1098
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3276
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-678
	rjmp  .+2014
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3230
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3300
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2040
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-710
	rjmp  .+62
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1154
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2012
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2858
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3286
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3356
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1194
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1858
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+1810
	rjmp  .-3234
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1960
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-490
	rjmp  .-3280
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3404
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1242
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1924
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+1762
	rjmp  .-632
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3394
	rjmp  .+14
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+286
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+1738
	rjmp  .+1882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1884
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-3468
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3418
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1298
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1852
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+46
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+38
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+30
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2018
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3470
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1370
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-622
	rjmp  .+86
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1362
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1784
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3058
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3580
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1756
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-546
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1580
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1732
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3616
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1458
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+1554
	rjmp  .-3416
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3130
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3138
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1688
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-618
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-734
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-758
	rjmp  .+6
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3654
	rjmp  .-3732
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+30
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-750
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-642
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1628
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3760
	rjmp  .+342
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1604
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1092
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-210
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-218
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-650
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1564
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1556
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-250
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-258
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-266
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1524
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1516
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1508
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-298
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1492
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1484
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-322
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-330
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-338
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-346
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-354
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-362
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-370
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-378
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-386
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-394
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-402
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-410
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-418
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-426
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-434
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-442
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-450
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-458
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-474
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-482
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-490
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-498
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1170
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2522
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1092
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1082
	rjmp  .-1364
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-538
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1310
	rjmp  .+6
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1318
	rjmp  .-1284
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1114
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3902
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3902
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-586
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3918
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .+1034
	rjmp  .+1238
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .+1072
	rjmp  .-3634
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3950
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-994
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1002
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-946
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1018
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1580
	rjmp  .+1200
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3998
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1042
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2538
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4014
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4022
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4038
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3570
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+994
	rjmp  .+38
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-730
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2090
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1114
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+758
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1684
	rjmp  .+1110
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+746
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+734
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1544
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1716
	rjmp  .+62
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1170
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1012
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3874
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+690
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+678
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1210
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2682
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .+824
	rjmp  .+1024
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1624
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .+676
	rjmp  .+1066
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+630
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1258
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+924
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .+776
	rjmp  .+1048
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+818
	rjmp  .+1054
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-378
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .+752
	rjmp  .+1058
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+884
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+566
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1226
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+852
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-986
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-994
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2858
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+506
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1386
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .+544
	rjmp  .+976
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2386
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1800
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3450
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+454
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+756
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1306
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+594
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+732
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+668
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1474
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .+568
	rjmp  .+2968
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3522
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3530
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1896
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1378
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+432
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .+408
	rjmp  .-3554
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+616
	rjmp  .+2926
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-634
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+416
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1314
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+624
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+506
	rjmp  .+814
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+604
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2008
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1234
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1242
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1362
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+564
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+556
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1274
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1282
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1290
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+524
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+516
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+508
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1322
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+492
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+484
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1346
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1354
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1362
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1370
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1378
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1386
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1394
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1402
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1410
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1418
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1426
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1434
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1442
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1450
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1458
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1466
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1474
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1482
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1490
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1498
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2434
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-3272
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-3286
	out   PIXOUT,  r2
	rjmp  .-2258
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .-106
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-32
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	rjmp  .-1940
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2614
	out   PIXOUT,  r2
	rjmp  .-2674
	nop
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2534
	nop
	out   PIXOUT,  r3
	rjmp  .-20
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-118
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	rjmp  .-138
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-2816
	out   PIXOUT,  r3
	rjmp  .-62
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	ijmp
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-128
	movw  ZL,      r0
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2956
	out   PIXOUT,  r3
	rjmp  .-70

m80_tilerow_6:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2018
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-192
	rjmp  .-322
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2034
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-404
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1866
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-414
	rjmp  .+2100
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2074
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-284
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-288
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2098
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2106
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-260
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2122
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-276
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+294
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1962
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2018
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2026
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1562
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2038
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1578
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2618
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-368
	rjmp  .+1970
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1602
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1562
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-372
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-380
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+214
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2250
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-404
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-432
	rjmp  .+1926
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1666
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3658
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1570
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2722
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+166
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2178
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1932
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2802
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3714
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2770
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1562
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-468
	rjmp  .+1844
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2234
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3754
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3762
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2210
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1852
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2882
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-508
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1818
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-516
	rjmp  .-624
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2258
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1810
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-536
	rjmp  .+1774
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3842
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-556
	rjmp  .+70
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2346
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1618
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1594
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1634
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2514
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2522
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2530
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1626
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-562
	rjmp  .+1698
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1946
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1634
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1962
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+30
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3954
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-700
	rjmp  .-3568
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3026
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-966
	rjmp  .+1654
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-724
	rjmp  .+86
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3050
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1424
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4018
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2466
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-856
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4034
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1580
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1962
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1666
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1770
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-698
	rjmp  .-3514
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-792
	rjmp  .-3682
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-828
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4090
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2602
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1330
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-956
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-928
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2770
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2778
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2786
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-996
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1004
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2810
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2818
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2826
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1036
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1044
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1052
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2858
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1068
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1076
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2898
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2906
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2914
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2922
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2938
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2946
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2954
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2962
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2970
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2978
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2986
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2994
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3002
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3010
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3018
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3026
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3034
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3042
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1334
	rjmp  .+30
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3058
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1390
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2530
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1400
	rjmp  .+1210
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3098
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1284
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3872
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3122
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3130
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1390
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3146
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1406
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-730
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2626
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-562
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-570
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2034
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1074
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2050
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3330
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1510
	rjmp  .+1086
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2074
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2074
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1502
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1510
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-554
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3274
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1534
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1574
	rjmp  .-1664
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2138
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3674
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2106
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3434
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-282
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-722
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+956
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3474
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3730
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3482
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1592
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+726
	rjmp  .-3298
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-778
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3770
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3778
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2874
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+876
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3554
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+686
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2290
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+678
	rjmp  .+22
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2922
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+834
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+658
	rjmp  .+838
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3858
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+638
	rjmp  .-2970
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-890
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1782
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1466
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1798
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3538
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3546
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3554
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1362
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+618
	rjmp  .+46
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2418
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1322
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2434
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-378
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3914
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+494
	rjmp  .+712
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3738
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1952
	rjmp  .-3082
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+470
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3762
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1892
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4034
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3130
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+634
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3994
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+604
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2498
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1354
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1458
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1998
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+402
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+366
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4018
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1146
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+440
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1956
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2070
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3802
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3810
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1996
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2004
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3834
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3842
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3850
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2036
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2044
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2052
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3882
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2068
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2076
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3906
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3914
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3922
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3930
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3938
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3946
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3954
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3962
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3970
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3986
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3994
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4002
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4010
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4018
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4026
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4034
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4042
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4050
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4058
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-2210
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-2542
	out   PIXOUT,  r2
	rjmp  .-42
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	ijmp
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-94
	movw  ZL,      r0
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2504
	nop
	out   PIXOUT,  r2
	rjmp  .-2512
	nop
	out   PIXOUT,  r3
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-1668
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-1716
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .-48
	out   PIXOUT,  r3
	rjmp  .-1032
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-2624
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
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
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1194
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1202
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1210
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1218
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1226
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1234
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1242
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1250
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1258
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1266
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1274
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1282
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3052
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1298
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1306
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1314
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1322
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1330
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1338
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1346
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1354
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1362
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1370
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1378
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1386
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1394
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1402
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-2992
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1418
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1426
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1434
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1442
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1450
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1458
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1474
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1482
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1490
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1506
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1514
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1522
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1530
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1538
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1546
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1554
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1562
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1570
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1578
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1586
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1594
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1602
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1610
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1618
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1626
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1634
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1642
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1650
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1658
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1666
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1674
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1690
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1698
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1706
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1714
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1722
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1730
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1738
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1746
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1754
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1690
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1770
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1778
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1674
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1794
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1802
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1810
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1818
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1826
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-964
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1010
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1850
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1858
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1866
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1874
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1898
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2818
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1914
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1922
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1938
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1946
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1954
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1962
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3556
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3564
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1986
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1994
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2002
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3596
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3604
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3612
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2034
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3628
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3636
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2058
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2066
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2074
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2082
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2090
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2098
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2106
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2114
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2122
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2130
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2138
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2146
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2154
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2162
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2170
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2178
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2186
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2194
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2202
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2210
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2218
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2226
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2234
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2242
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2250
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2258
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2266
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2274
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2282
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2290
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2298
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2306
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-4062
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2322
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2330
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2338
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2346
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2354
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2362
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2370
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2378
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2386
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2394
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2402
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2410
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2418
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2426
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1502
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2442
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2450
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2458
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2466
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2474
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2482
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2490
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2498
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2506
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2514
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2522
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2530
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2538
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2546
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2554
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2562
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2570
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2578
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2586
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2594
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2602
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2610
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2618
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2626
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2634
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2642
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2650
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2658
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2666
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2674
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2682
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2690
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2698
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2706
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2714
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2722
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2730
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2738
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2746
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2754
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2762
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2770
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2778
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2602
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2802
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2562
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2818
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2826
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2834
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2842
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2850
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1940
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1974
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2874
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2882
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2890
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2898
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2906
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2914
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2922
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3266
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2938
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2946
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2954
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2962
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2970
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2986
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2038
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2046
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3010
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3018
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3026
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2078
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2086
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2094
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3058
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2110
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2118
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3082
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3090
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3098
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3106
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3114
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3122
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3130
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3138
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3146
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3154
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3162
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3170
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3178
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3186
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3194
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3202
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3210
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3218
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3226
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3234
