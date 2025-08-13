
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
	rjmp  .+2328
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+2034
	rjmp  .+2036
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2244
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2310
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2318
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2288
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+310
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2310
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2310
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+2320
	rjmp  .+2000
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2248
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2240
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2232
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2224
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2216
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2278
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+230
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+2144
	rjmp  .+1954
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+438
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+430
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2252
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2258
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2264
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2242
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+390
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+470
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2120
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2112
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+806
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2096
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2214
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+2214
	rjmp  .+1870
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+326
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+94
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2204
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2160
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+806
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2130
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2122
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2128
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2170
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2112
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2170
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2146
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2170
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1696
	rjmp  .+198
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2122
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-18
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2092
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-34
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2076
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+174
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+854
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2066
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2058
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2050
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2042
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2080
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1962
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2082
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+2042
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2066
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1854
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1824
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1854
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1808
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1994
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1792
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1854
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1776
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+2012
	rjmp  .+1572
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1760
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1946
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+2004
	rjmp  .+22
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2002
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1922
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1980
	rjmp  .+1534
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
	rjmp  .+1664
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1916
	rjmp  .+1480
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1648
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1640
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
	rjmp  .+1882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1630
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1844
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1568
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1560
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1552
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1544
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1574
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1566
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1520
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1550
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1542
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1534
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1526
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1480
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1510
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
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1368
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1360
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1352
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1344
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1336
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1328
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1320
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1312
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1304
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1056
	rjmp  .+1134
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1254
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1568
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1576
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1264
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+510
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1560
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1560
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1570
	rjmp  .+62
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1224
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1216
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1208
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1200
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1192
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1528
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+430
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1154
	rjmp  .+1010
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+462
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+454
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1502
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1508
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1514
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1492
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+414
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+986
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1096
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1088
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1072
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1464
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1462
	rjmp  .+110
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+350
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+294
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1452
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1410
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+920
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1380
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1372
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1378
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1418
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1362
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1418
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1394
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1146
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+718
	rjmp  .+796
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1370
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+182
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1340
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+166
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1324
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+198
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+806
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1314
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1306
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1298
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1290
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1314
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1212
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1316
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1018
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1300
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1112
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+800
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1104
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+784
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+970
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+768
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1104
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+752
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1246
	rjmp  .+22
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+736
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+922
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1238
	rjmp  .+582
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1236
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+898
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1214
	rjmp  .+62
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
	rjmp  .+640
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1150
	rjmp  .+504
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+624
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+616
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
	rjmp  .+1116
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+880
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1078
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1530
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+544
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+536
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+528
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+520
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+824
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+816
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+496
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+800
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+792
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+784
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+776
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+456
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+760
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
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+344
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+336
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+328
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+320
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+312
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+304
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+296
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+288
	out   PIXOUT,  r3
	rjmp  .-2038
	out   PIXOUT,  r2
	movw  ZL,      r0
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
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	rjmp  .-1956
	out   PIXOUT,  r3
	movw  ZL,      r0
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
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-36
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	add   ZH,      r19
	ld    r18,     X+
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
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-40
	rjmp  .
	rjmp  .
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
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
	rjmp  .
	out   PIXOUT,  r2
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
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .
	rjmp  .-286
	out   PIXOUT,  r2
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-326
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
	rjmp  .-112
	rjmp  .
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
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
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .-414
	out   PIXOUT,  r3
	rjmp  .
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
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
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	rjmp  .
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-548
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-136
	out   PIXOUT,  r3
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .-282
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-600
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
	rjmp  .-386
	rjmp  .
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
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
	out   PIXOUT,  r3
	rjmp  .-274
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
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
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
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
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-806
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-122
	out   PIXOUT,  r2
	rjmp  .
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
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
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	rjmp  .-664
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
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
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
	out   PIXOUT,  r3
	rjmp  .-72
	rjmp  .-690
	out   PIXOUT,  r3
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
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
	rjmp  .+2054
	out   PIXOUT,  r3
	rjmp  .-646
	nop

m80_tilerow_1:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-744
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-640
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-828
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-762
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-532
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-238
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-786
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-762
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-548
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-778
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-818
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-794
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-840
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-848
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-856
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-686
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-308
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-928
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-726
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-734
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-174
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-718
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-336
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-662
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-774
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-346
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-914
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-922
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-976
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-724
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-986
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-838
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-444
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-854
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-422
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-434
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-846
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-854
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-454
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-902
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1034
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-902
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-490
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-902
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1692
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1694
	nop
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-556
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-966
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-572
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-982
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-990
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1122
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1006
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1014
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1022
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-628
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-992
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-530
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1090
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1584
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1194
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1106
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1248
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1218
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1264
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1078
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1280
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1218
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1296
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1504
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1312
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1126
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1328
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1336
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1150
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1092
	rjmp  .-3122
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
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1408
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1398
	rjmp  .-2138
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1424
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1432
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
	ld    r18,     X+
	rjmp  .-1212
	rjmp  .-1084
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1442
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1442
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-786
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1504
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1512
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1520
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1528
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
	rjmp  .-1552
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
	rjmp  .-1592
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1562
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
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1704
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1712
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1720
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1728
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1736
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1744
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1752
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1760
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1768
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1390
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1818
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1504
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1298
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1190
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1528
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1512
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1314
	rjmp  .+1160
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1528
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1560
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1544
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
	rjmp  .-1438
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1260
	rjmp  .+1150
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1918
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1478
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1486
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+902
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1742
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1288
	rjmp  .-1522
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1428
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1526
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1310
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1664
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1672
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1680
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2000
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1490
	rjmp  .+1150
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1728
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1590
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1396
	rjmp  .-2194
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1606
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1386
	rjmp  .-3770
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1398
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1870
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1878
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1418
	rjmp  .-3250
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1654
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1784
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1654
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1454
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1926
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+706
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+708
	rjmp  .-2134
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1508
	rjmp  .+1122
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1718
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1524
	rjmp  .-1814
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1734
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1742
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1872
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1758
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1766
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1774
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1580
	rjmp  .-2020
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1758
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1538
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1840
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+598
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1944
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1858
	rjmp  .-1890
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2272
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1968
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2288
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2102
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2304
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1968
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2320
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+518
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2336
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2150
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2352
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2360
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2174
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1858
	rjmp  .-2476
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
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2432
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+412
	rjmp  .-1988
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2448
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2456
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
	ld    r18,     X+
	rjmp  .-1978
	rjmp  .-3130
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2192
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2192
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2528
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2536
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2544
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2552
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2248
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2256
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2576
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2272
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2280
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
	ld    r18,     X+
	rjmp  .-2616
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2312
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
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2728
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2736
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2744
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2752
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2760
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2768
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2776
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2784
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .-2950
	out   PIXOUT,  r2
	rjmp  .-2644
	out   PIXOUT,  r2
	rjmp  .-2744
	out   PIXOUT,  r3
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2472
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .-2918
	out   PIXOUT,  r3
	rjmp  .-2410
	out   PIXOUT,  r3
	rjmp  .-2782
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-3048
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
	nop
	out   PIXOUT,  r3
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-3050
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
	nop
	out   PIXOUT,  r2
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
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
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
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
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
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
	out   PIXOUT,  r3
	rjmp  .-2830
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
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-3326
	nop
	out   PIXOUT,  r2
	rjmp  .-3110
	nop
	out   PIXOUT,  r3
	rjmp  .-3348
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .-3392
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-3178
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
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2944
	nop
	out   PIXOUT,  r2
	rjmp  .-2940
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2944
	nop
	out   PIXOUT,  r2
	rjmp  .-3368
	nop
	out   PIXOUT,  r3
	rjmp  .-2948

m80_tilerow_2:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3304
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-442
	rjmp  .-2808
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-446
	rjmp  .-3414
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3230
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-212
	rjmp  .-2810
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-202
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2002
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3296
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3304
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3330
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3250
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3354
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3400
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3408
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3416
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3378
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1426
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3402
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3378
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3386
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-570
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3278
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3286
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3310
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3334
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3342
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3474
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3482
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3464
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3438
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1522
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-388
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3406
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3414
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3390
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3430
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3406
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3414
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3422
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3462
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3594
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3462
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1818
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3462
	out   PIXOUT,  r3
	rjmp  .-654
	out   PIXOUT,  r3
	rjmp  .-3534
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-494
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3518
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3526
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3534
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3542
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-904
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3682
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3566
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3574
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3582
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-724
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3552
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-584
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3650
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3720
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3754
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3794
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3808
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1786
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1778
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3684
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1794
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3604
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1810
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1056
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3628
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1802
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3628
	rjmp  .+1498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3630
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3742
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3652
	rjmp  .-710
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-730
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1850
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1890
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3796
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3708
	out   PIXOUT,  r3
	rjmp  .-1046
	out   PIXOUT,  r2
	rjmp  .-3440
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3724
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1106
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3822
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3830
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-802
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3846
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3854
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3926
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3772
	rjmp  .+302
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4002
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4002
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4056
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4064
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3960
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4080
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4088
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4058
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4066
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1978
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4082
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4090
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-920
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-928
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2018
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-944
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2034
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2042
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2050
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
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2194
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1404
	rjmp  .-3652
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1408
	rjmp  .+1112
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3980
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1138
	rjmp  .+1012
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1128
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2962
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-4046
	rjmp  .+6
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-4054
	rjmp  .-3692
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4080
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-4002
	rjmp  .-3690
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1160
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
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1188
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2314
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1208
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2314
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2322
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1532
	rjmp  .+62
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2282
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2290
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4062
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4086
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4094
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1280
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1288
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1270
	rjmp  .+968
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2298
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1308
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2202
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1300
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1748
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1756
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2394
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1772
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2410
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2418
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2426
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1804
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1400
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2226
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2778
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2466
	out   PIXOUT,  r2
	rjmp  .-1624
	out   PIXOUT,  r3
	rjmp  .-802
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1406
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1860
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1868
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1876
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1884
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1890
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1488
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1908
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1916
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1924
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1694
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1684
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+552
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1140
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1526
	rjmp  .+722
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1560
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1706
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2698
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2466
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2354
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2914
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2370
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1562
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2386
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2042
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1586
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1586
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-954
	rjmp  .-2124
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1586
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2084
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-978
	rjmp  .+118
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+416
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1634
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3026
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1666
	out   PIXOUT,  r2
	rjmp  .-2008
	out   PIXOUT,  r2
	rjmp  .+454
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1682
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2068
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2164
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2172
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+344
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2188
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2196
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2786
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1098
	rjmp  .+482
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1808
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1812
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2946
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2954
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3626
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
	rjmp  .-1864
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1872
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3002
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1888
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1896
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1904
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1912
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3042
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1928
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3058
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3066
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3074
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
	nop
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .-2316
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
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
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
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r2
	rjmp  .-1114
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
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
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2408
	out   PIXOUT,  r2
	rjmp  .-2618
	out   PIXOUT,  r3
	rjmp  .-1114
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2384
	out   PIXOUT,  r2
	rjmp  .-628
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-2766
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .-2386
	rjmp  .
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	rjmp  .-228
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	rjmp  .-264
	rjmp  .
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
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
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .-2834
	rjmp  .
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .-2510
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-306
	out   PIXOUT,  r3
	rjmp  .
	rjmp  .-2780
	rjmp  .
	rjmp  .
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	rjmp  .-2574
	rjmp  .
	nop
	out   PIXOUT,  r2
	rjmp  .-2740
	out   PIXOUT,  r3
	rjmp  .
	rjmp  .-110
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
	rjmp  .-76
	rjmp  .
	rjmp  .-34
	rjmp  .-2592
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	rjmp  .-72
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .-2866
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .-3036

m80_tilerow_3:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3730
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1186
	rjmp  .+2050
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3746
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-376
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3850
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-276
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2002
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3786
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-280
	rjmp  .+54
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-282
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-274
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-268
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3826
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-298
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3842
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-296
	rjmp  .+1952
	out   PIXOUT,  r3
	rjmp  .-592
	rjmp  .-2702
	rjmp  .-3170
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2784
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-492
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-320
	rjmp  .+1930
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-512
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1546
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1554
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4018
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-4010
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-364
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3938
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3946
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3280
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3962
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-390
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2896
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-566
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3322
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1650
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3930
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3346
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4082
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4090
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-446
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-514
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2976
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-486
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-476
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4002
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3026
	out   PIXOUT,  r3
	rjmp  .-832
	out   PIXOUT,  r2
	rjmp  .-386
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3434
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1762
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3450
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1778
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-530
	rjmp  .+1688
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3064
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3482
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3490
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3106
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-564
	rjmp  .-3046
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-900
	rjmp  .+1654
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-580
	rjmp  .+1584
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-572
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3128
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3136
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3554
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-600
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-608
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-612
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3586
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3594
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3602
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3610
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-852
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3626
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1458
	rjmp  .-3146
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-760
	rjmp  .+1554
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-682
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-674
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-784
	rjmp  .+1544
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2450
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1506
	rjmp  .-874
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3690
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3698
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3706
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-714
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-706
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-848
	rjmp  .-3544
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3738
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3746
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3362
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-742
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3770
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1778
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-944
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-832
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1034
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-848
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3408
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3490
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-872
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-880
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3440
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3448
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-904
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3464
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3472
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3480
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3488
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-944
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3504
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-960
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-968
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-976
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-984
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-992
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1000
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1008
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1016
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1024
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1032
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1040
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1048
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1056
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1064
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1072
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1080
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1088
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1096
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1104
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1112
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1120
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1442
	rjmp  .-3702
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1136
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3794
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1076
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+976
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2578
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1176
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1102
	rjmp  .-3906
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1090
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1042
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-988
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1216
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1066
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1232
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3050
	rjmp  .-1644
	out   PIXOUT,  r2
	rjmp  .-1546
	out   PIXOUT,  r3
	rjmp  .-1026
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3768
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1498
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-810
	rjmp  .+1240
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2122
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2130
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1482
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1236
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3778
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1328
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1336
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-100
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1352
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1114
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3880
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1552
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2226
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1346
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-906
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3866
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3874
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1282
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3960
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1210
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-914
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1418
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1880
	out   PIXOUT,  r2
	rjmp  .-1786
	out   PIXOUT,  r2
	rjmp  .-930
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-994
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2338
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1010
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2354
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-42
	rjmp  .+1054
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4048
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1042
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1050
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1960
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+646
	rjmp  .+3128
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1854
	rjmp  .+3134
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+630
	rjmp  .+1054
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3700
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+630
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+622
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1114
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1624
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1632
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-970
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1146
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1154
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1162
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1170
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1838
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1186
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1954
	rjmp  .-1802
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3514
	rjmp  .-2084
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1034
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1018
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3538
	rjmp  .+3052
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3122
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2002
	rjmp  .-1742
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1250
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1258
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1266
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+486
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1730
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3602
	rjmp  .+854
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1298
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1306
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2216
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+454
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1330
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2060
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1766
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1856
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2020
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1872
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+350
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+408
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1896
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1904
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+318
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+310
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1928
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+294
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+286
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+278
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+270
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1968
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+254
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1984
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1992
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2000
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2008
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2016
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2024
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2032
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2040
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2048
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2056
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2064
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2072
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2080
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2088
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2096
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2104
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2112
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2120
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2128
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2136
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
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-2626
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-26
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
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
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-50
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
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
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
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
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
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
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	movw  ZL,      r0
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
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
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
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	movw  ZL,      r0
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
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2650
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
	rjmp  .-2748
	nop
	out   PIXOUT,  r3
	rjmp  .+2078
	rjmp  .+2176

m80_tilerow_4:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2656
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-50
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2672
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2778
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-388
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-220
	rjmp  .-2640
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-384
	rjmp  .+2070
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2712
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-100
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-94
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2762
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-130
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2752
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2760
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2768
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2780
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2746
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-178
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1234
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2804
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2914
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2820
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1258
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-206
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1274
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2852
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2864
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2872
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-260
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2888
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-262
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2984
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-418
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3018
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1354
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2882
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1370
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2898
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2906
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1394
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1402
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-370
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3046
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2234
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2954
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1442
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-384
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1458
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2986
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3262
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1786
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3258
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-458
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1506
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3030
	out   PIXOUT,  r3
	rjmp  .-754
	out   PIXOUT,  r2
	rjmp  .-2874
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-684
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-498
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1614
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3132
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2010
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-530
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3152
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3160
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3168
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3244
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1610
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3138
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1626
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3306
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-124
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1650
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1658
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3320
	rjmp  .+1654
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3242
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1674
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3344
	rjmp  .-752
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1468
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1706
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1714
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1722
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1730
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3258
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3392
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3408
	rjmp  .+1576
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1762
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1770
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1388
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3386
	rjmp  .-862
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-840
	rjmp  .-786
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3396
	rjmp  .+1604
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3464
	rjmp  .-816
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-778
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-766
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3408
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-802
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+692
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1326
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3476
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1310
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3492
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3464
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-858
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1278
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3524
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3496
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3504
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3512
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3520
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3528
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3536
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3544
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3552
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3560
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3568
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3576
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3584
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3592
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3600
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3608
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3616
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3624
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3632
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3640
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3648
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3656
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3664
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3672
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3680
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1458
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3696
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3546
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1352
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1174
	rjmp  .-3712
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1348
	rjmp  .-3824
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3736
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1020
	rjmp  .-3720
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1018
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-2274
	rjmp  .+950
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1538
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3776
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3784
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3792
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+976
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1546
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1586
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2098
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3162
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3682
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3178
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3354
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+906
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3370
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3210
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3888
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3896
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+860
	rjmp  .-3970
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3912
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+850
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+868
	rjmp  .-3998
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1372
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3786
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3450
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3906
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3466
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3922
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3930
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3490
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3498
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1778
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3770
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2842
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3978
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3538
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+754
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3554
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4010
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1634
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2554
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3370
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1866
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3602
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1834
	out   PIXOUT,  r2
	rjmp  .-1718
	out   PIXOUT,  r2
	rjmp  .-4094
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1672
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1906
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+674
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+624
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2922
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1938
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2434
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
	out   PIXOUT,  r2
	rjmp  .-2410
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3706
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1522
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3722
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4074
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2996
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3746
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3754
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+532
	rjmp  .-1620
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3594
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1578
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+508
	rjmp  .+62
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+538
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3802
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3810
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3818
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3826
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1642
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3882
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+444
	rjmp  .+748
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3858
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3866
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+458
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-2898
	rjmp  .-2192
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1794
	rjmp  .-2158
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2186
	rjmp  .+6
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+388
	rjmp  .-2240
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2186
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+346
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2690
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2210
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2152
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+396
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3914
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+380
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2746
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2266
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+348
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3962
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
	rjmp  .-2794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2802
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2810
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
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2858
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2866
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
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2930
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
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-2520
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-3068
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-2144
	nop
	out   PIXOUT,  r2
	rjmp  .-2618
	out   PIXOUT,  r3
	rjmp  .-3078
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-2580
	nop
	out   PIXOUT,  r2
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-2604
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r3
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
	rjmp  .-3162
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
	out   PIXOUT,  r2
	rjmp  .-1656
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
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
	rjmp  .-2694
	out   PIXOUT,  r3
	rjmp  .-1742
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
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
	rjmp  .-2438
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
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
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
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .-2790
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
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
	rjmp  .-2900
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .-2986
	rjmp  .-2512
	rjmp  .
	rjmp  .-2876
	rjmp  .-358
	rjmp  .
	rjmp  .-2916
	rjmp  .
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-2968
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	rjmp  .+2052

m80_tilerow_5:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3474
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3482
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3490
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-184
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-80
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-400
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-390
	rjmp  .+14
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3530
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-96
	rjmp  .-566
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2674
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-240
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2690
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2698
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3578
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2714
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-444
	rjmp  .+6
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-270
	rjmp  .-3134
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2738
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2666
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3794
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-186
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3810
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2864
	rjmp  .+62
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2786
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3834
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1698
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
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1722
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-272
	rjmp  .-3274
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3722
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2778
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3906
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3914
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2968
	rjmp  .-3318
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2242
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2818
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2826
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3000
	rjmp  .-3384
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3962
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2930
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1810
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2298
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2874
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4002
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2258
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-518
	rjmp  .-854
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2906
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-688
	rjmp  .+30
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4042
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4050
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3018
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-566
	rjmp  .-3444
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1610
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-564
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-590
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3058
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-2588
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1582
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1978
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3090
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3970
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3978
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3986
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1978
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1986
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1994
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2002
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3074
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2684
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1882
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2034
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-616
	rjmp  .+1508
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2090
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2074
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-640
	rjmp  .+62
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1092
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2082
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2090
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-688
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1962
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3178
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-618
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3412
	rjmp  .-3460
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3368
	rjmp  .+46
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1978
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2554
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-982
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1666
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1338
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-760
	rjmp  .+1378
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3338
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3326
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-634
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-642
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1698
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3378
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3386
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-674
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-690
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3418
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3426
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3434
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-722
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3450
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3458
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-746
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-754
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-762
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-770
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-778
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-786
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-794
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-802
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-810
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-818
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-826
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-834
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-842
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-850
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-858
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-866
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-874
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-890
	out   PIXOUT,  r2
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
	rjmp  .-914
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-922
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1154
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-882
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1324
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1314
	rjmp  .+1044
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-962
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1508
	rjmp  .+54
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4082
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1210
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+952
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+944
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1010
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+928
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1382
	rjmp  .-1480
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1240
	rjmp  .-1658
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+904
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3690
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2010
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-906
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2026
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-3818
	rjmp  .+948
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+856
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2050
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2050
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+832
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+824
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+816
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2074
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1684
	rjmp  .+38
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1154
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3802
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2122
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2130
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-3922
	rjmp  .+858
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3162
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3842
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3850
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-3954
	rjmp  .+840
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2178
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+712
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1430
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3218
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3898
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2218
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3234
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1488
	rjmp  .+826
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3930
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1636
	rjmp  .+824
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2258
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2266
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+624
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1536
	rjmp  .+6
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+624
	rjmp  .+798
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1532
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1560
	rjmp  .-2282
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+584
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+594
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1936
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2330
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+552
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
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2378
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2386
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2394
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2402
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1202
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+498
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1652
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2434
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2028
	rjmp  .+656
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2442
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1694
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2052
	rjmp  .+646
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2022
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2482
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2490
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1490
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1732
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1306
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1298
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1896
	rjmp  .+614
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-1884
	rjmp  .+46
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1770
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3554
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1906
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1790
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+342
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2172
	rjmp  .-2144
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+304
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2214
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1658
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1666
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1844
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+264
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+256
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1698
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1706
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1714
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+224
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+216
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+208
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1746
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+192
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+184
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1770
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1778
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1786
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1802
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1810
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1818
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1826
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1834
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1842
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1850
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1858
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1866
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1874
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1882
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1890
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1898
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1906
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1914
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1922
	out   PIXOUT,  r3
	rjmp  .-2054
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2474
	nop
	out   PIXOUT,  r2
	rjmp  .-2580
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-1772
	out   PIXOUT,  r3
	rjmp  .-2534
	nop
	out   PIXOUT,  r3
	rjmp  .-2608
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
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
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	ijmp
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1506
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	nop
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
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	ijmp
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
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
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	ijmp
	rjmp  .-2382
	out   PIXOUT,  r2
	rjmp  .-876
	rjmp  .
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
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
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	nop
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
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-352
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
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
	rjmp  .-2978
	rjmp  .-404
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-254
	rjmp  .
	rjmp  .
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2954
	nop
	out   PIXOUT,  r3
	rjmp  .-36
	out   PIXOUT,  r3
	rjmp  .-28
	nop
	out   PIXOUT,  r3
	rjmp  .-2566
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .-98
	out   PIXOUT,  r2
	rjmp  .-2984
	out   PIXOUT,  r2
	rjmp  .-2956
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-288
	out   PIXOUT,  r3
	rjmp  .-2964
	rjmp  .
	rjmp  .+2048

m80_tilerow_6:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2442
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-152
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2458
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2744
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-168
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2742
	rjmp  .+2010
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2026
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-184
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-174
	rjmp  .-2506
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
	ld    r18,     X+
	rjmp  .-240
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2546
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-256
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-216
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2082
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3986
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2026
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-4002
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2746
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-4018
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3666
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2776
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-4042
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1690
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-352
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-360
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-318
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2674
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-746
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2840
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1730
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1674
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2880
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3770
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-376
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2178
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2490
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3826
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1730
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3818
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-854
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1754
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2234
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1770
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1778
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2330
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2570
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2370
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1810
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-560
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2378
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-568
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-868
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1858
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-600
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2346
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-546
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1842
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-562
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
	out   PIXOUT,  r3
	rjmp  .-1818
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3136
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2010
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1842
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2026
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-618
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3082
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1986
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-970
	rjmp  .-3190
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1818
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2010
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-994
	rjmp  .-3646
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3652
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2034
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2106
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2826
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3162
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2842
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3272
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-726
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1050
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-734
	rjmp  .-3186
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1048
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2114
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1874
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2602
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-772
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-880
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3336
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3194
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3202
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3210
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-920
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-928
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3234
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3242
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3250
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-960
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-968
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-976
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3282
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-992
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1000
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3306
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3314
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3322
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3330
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3338
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3346
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3354
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3362
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3370
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3378
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3386
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3394
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3402
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3410
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3418
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3426
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3434
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3442
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3450
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3458
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3466
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1528
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3482
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3714
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1094
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .-3712
	rjmp  .-3386
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2218
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3522
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4094
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1124
	rjmp  .-3530
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
	ld    r18,     X+
	rjmp  .-1616
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3570
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1632
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1240
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2266
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3778
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2258
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3466
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3810
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3812
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1232
	rjmp  .-3458
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3834
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2178
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1728
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1736
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1270
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3698
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1280
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1296
	rjmp  .-3410
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3898
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2218
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3682
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3916
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1324
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2410
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3514
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3950
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2274
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3964
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1840
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2298
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2466
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2314
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2322
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2514
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3594
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2562
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2354
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-4050
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1936
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2562
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1494
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1730
	rjmp  .-1474
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2402
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1976
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2578
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1458
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2370
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1474
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
	out   PIXOUT,  r2
	rjmp  .-2402
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3938
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1796
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2426
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1812
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2062
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3762
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2530
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1840
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1846
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2554
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1864
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1854
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2578
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1892
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3850
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3842
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3866
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4074
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+368
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1900
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+360
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1898
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2658
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1896
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2834
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+322
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2256
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1792
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1706
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1714
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1722
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2296
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2304
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1746
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1754
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1762
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
	rjmp  .-1794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2368
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2376
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1818
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1826
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1834
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1842
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1850
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1858
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1866
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1874
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1882
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1890
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1898
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1906
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1914
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1922
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1930
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1938
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1946
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1954
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1962
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1970
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-30
	out   PIXOUT,  r2
	rjmp  .-24
	nop
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2622
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
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
	rjmp  .-2490
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2506
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
	ld    r18,     X+
	rjmp  .-2538
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2546
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2554
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2562
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2570
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2578
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2758
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2594
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2602
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2610
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2618
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2626
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2634
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2642
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2650
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2658
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2666
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2674
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2690
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2698
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-684
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2714
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2722
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2730
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2738
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2746
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2754
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2762
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
	out   PIXOUT,  r3
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
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3042
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3050
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2546
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3066
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3074
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3670
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3090
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3098
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3106
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3114
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3122
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2594
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2522
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3146
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3154
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3162
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3170
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3178
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3186
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3194
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3360
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3210
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3218
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3226
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3234
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3242
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3250
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3258
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3480
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3488
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3282
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3290
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3298
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3520
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3528
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3536
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3330
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3552
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3560
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3354
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3362
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3370
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3378
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3386
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3394
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3402
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3410
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3418
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3426
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3434
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3442
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3450
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3458
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3474
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3482
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3490
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3506
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3514
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3522
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3530
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
	ld    r18,     X+
	rjmp  .-3562
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3570
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3578
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3586
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3594
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3602
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-3708
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3618
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3626
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3634
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3642
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3650
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3658
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3666
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3674
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3682
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3690
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3698
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3706
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3714
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3722
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1690
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3738
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3746
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3754
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3762
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3770
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3778
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3786
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
	out   PIXOUT,  r2
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
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4066
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4074
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3090
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4090
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3074
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-2970
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
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3618
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2962
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
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2834
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
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3242
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3250
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3258
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
	rjmp  .-3282
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3290
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3298
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
	rjmp  .-3330
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2306
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2314
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3354
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3362
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3370
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3378
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3386
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3394
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3402
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3410
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3418
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3426
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3434
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3442
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
	ld    r18,     X+
	rjmp  .-3474
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3482
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3490
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3498
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3506
