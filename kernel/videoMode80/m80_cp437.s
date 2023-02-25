
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
	rjmp  .+782
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+254
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+246
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2050
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2250
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+2260
	rjmp  .+2000
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2234
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+726
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+718
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+710
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+702
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2224
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+2234
	rjmp  .+6
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+2240
	rjmp  .+1950
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1134
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2162
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+1198
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-98
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2232
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2066
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1086
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1214
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+606
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2222
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
	rjmp  .+574
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+566
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+558
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+550
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2042
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1630
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+526
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1974
	rjmp  .+126
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1978
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2134
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1694
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+486
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1678
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+2128
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2086
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+2126
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+446
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+438
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+430
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+422
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+414
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2084
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1598
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1846
	rjmp  .+1672
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-138
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-146
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2058
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1454
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1898
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1438
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-186
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+926
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+318
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+310
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+22
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+294
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1944
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1796
	rjmp  .+1588
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-250
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1962
	rjmp  .+78
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1970
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-26
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1326
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1318
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1762
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1936
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1746
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1720
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1930
	rjmp  .+1504
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1924
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1500
	rjmp  .+1588
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1888
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1350
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1858
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1334
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1842
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-402
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+574
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1832
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1852
	rjmp  .+1552
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1844
	rjmp  .+1572
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1808
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1420
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1158
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1754
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1820
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1738
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1530
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+22
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1672
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+6
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1778
	rjmp  .+30
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-10
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1764
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-26
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1514
	rjmp  .+14
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-42
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1730
	rjmp  .+1266
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1566
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1612
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+1706
	rjmp  .+1446
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1618
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-90
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-98
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-106
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-114
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-122
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-130
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-138
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1478
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-154
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-162
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-170
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-178
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-186
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-194
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1580
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1414
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1570
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-314
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-234
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1306
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1544
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1552
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-786
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1520
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1534
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1334
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-298
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-826
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1480
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1494
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1464
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-858
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1470
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1376
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1460
	rjmp  .+1188
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1440
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-378
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1440
	rjmp  .+1178
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-914
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1392
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1406
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-938
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1390
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1360
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1288
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1280
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-458
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1364
	rjmp  .+6
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+1360
	rjmp  .+1100
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1368
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1362
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1312
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1304
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1296
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1288
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-794
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+558
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1002
	rjmp  .+6
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+994
	rjmp  .+1030
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1062
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-570
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-578
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1188
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1180
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1220
	rjmp  .+1008
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-610
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-618
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+742
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+752
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .+762
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+974
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+966
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+958
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1194
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-690
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1170
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1162
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-714
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1146
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1138
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+886
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-746
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+870
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+862
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-770
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+846
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-786
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+830
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+822
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1058
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1050
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-826
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1034
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-842
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1018
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-858
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1002
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+750
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+986
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-898
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+962
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+710
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-922
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-930
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+678
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+670
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-962
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-970
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+886
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+886
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1002
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1010
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1018
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1026
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1034
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+54
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1050
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1058
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1066
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+22
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+118
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+110
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+464
	rjmp  .+436
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1106
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1114
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+426
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+70
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1138
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+478
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+734
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+530
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1338
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+446
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+438
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1194
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+316
	rjmp  .+38
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
	rjmp  .-1914
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+660
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+214
	rjmp  .+406
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1250
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1258
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	ijmp
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	add   ZH,      r19
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
	nop
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
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
	out   PIXOUT,  r3
	rjmp  .-1554
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
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
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
	rjmp  .-1448
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1190
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1180
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	add   ZH,      r19
	ld    r18,     X+
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
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-316
	out   PIXOUT,  r2
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .-218
	rjmp  .
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-146
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
	out   PIXOUT,  r2
	rjmp  .-18
	nop
	out   PIXOUT,  r2
	rjmp  .-1708
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .
	rjmp  .-72
	out   PIXOUT,  r2
	rjmp  .-6
	nop
	out   PIXOUT,  r3
	rjmp  .-68
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-130
	rjmp  .
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	rjmp  .
	out   PIXOUT,  r3
	rjmp  .-278
	out   PIXOUT,  r2
	rjmp  .-314
	out   PIXOUT,  r3
	rjmp  .-34
	out   PIXOUT,  r3
	rjmp  .-1356
	out   PIXOUT,  r3
	rjmp  .-592
	out   PIXOUT,  r2
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	rjmp  .-254
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-74
	rjmp  .
	nop
	out   PIXOUT,  r2
	rjmp  .-564
	rjmp  .
	nop
	out   PIXOUT,  r3
	rjmp  .-572
	nop
	out   PIXOUT,  r2
	rjmp  .-188
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	rjmp  .-458
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
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-34
	rjmp  .-506
	out   PIXOUT,  r2
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
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
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
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
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	rjmp  .-456
	out   PIXOUT,  r2
	rjmp  .-432
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
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
	out   PIXOUT,  r3
	rjmp  .-626
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-784
	out   PIXOUT,  r2
	rjmp  .-410
	out   PIXOUT,  r3
	rjmp  .-556
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .-722
	out   PIXOUT,  r3
	rjmp  .
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+2048

m80_tilerow_1:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2290
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-568
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1210
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1218
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-804
	rjmp  .-2290
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1130
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-638
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2346
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2354
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-814
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-376
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-226
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-218
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-384
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-216
	out   PIXOUT,  r3
	rjmp  .-380
	out   PIXOUT,  r3
	rjmp  .+1948
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-602
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-616
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-886
	nop
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-252
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-256
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-244
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2466
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-240
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-934
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-896
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-904
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-268
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2514
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-610
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-830
	nop
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-490
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2546
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1954
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1094
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-938
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2114
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-342
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-962
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-944
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-372
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-380
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-994
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1010
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
	rjmp  .-814
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-460
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1226
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-960
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-968
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-458
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-940
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-888
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-802
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1008
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-818
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1096
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1104
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1146
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2778
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-964
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1162
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1072
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1292
	rjmp  .+1676
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-574
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-992
	rjmp  .+1736
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1398
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-598
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-606
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1024
	rjmp  .-1618
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1136
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1250
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1030
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1454
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1226
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-654
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1166
	rjmp  .+1680
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-708
	rjmp  .-3314
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-686
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-724
	rjmp  .-658
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-702
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1224
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1030
	rjmp  .+1704
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1240
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-764
	rjmp  .-1444
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1256
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-780
	rjmp  .-1684
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1134
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1060
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1360
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1354
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1410
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1524
	rjmp  .-2682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3050
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1400
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3066
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1402
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3082
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1238
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3098
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-920
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3114
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1450
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3130
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3138
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1474
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1530
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
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3202
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3210
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1518
	rjmp  .-3402
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
	rjmp  .-3266
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1452
	rjmp  .-2890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1658
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1696
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1038
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1806
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1480
	rjmp  .-1794
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3322
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3330
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1608
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
	rjmp  .-1738
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3370
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1648
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
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1680
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3418
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1908
	rjmp  .-1724
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1158
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
	rjmp  .-1338
	rjmp  .-1662
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1736
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3474
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3482
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1760
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3506
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2314
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3522
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1906
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2070
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1678
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1922
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1596
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3570
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3578
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3586
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3594
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3602
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3610
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1750
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1440
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3634
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3642
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3650
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1730
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1738
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3674
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1616
	rjmp  .-1950
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1896
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1616
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1606
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+626
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
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1878
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3754
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3762
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1902
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1910
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3786
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1926
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1934
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2186
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3818
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2202
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2210
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3842
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2226
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3858
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2242
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2250
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2014
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2022
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3898
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2038
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3914
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2054
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3930
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2070
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2322
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2086
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3962
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3970
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2110
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2362
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3994
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4002
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2142
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2394
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2402
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4034
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4042
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4050
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2186
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2186
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4074
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4082
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2542
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3002
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3010
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2384
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
	out   PIXOUT,  r3
	rjmp  .-2576
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2628
	rjmp  .+548
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1940
	rjmp  .-2280
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1948
	rjmp  .-2444
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1858
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2508
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2056
	rjmp  .-2440
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1988
	rjmp  .-2426
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3114
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2594
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1820
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2534
	rjmp  .-2352
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2258
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
	out   PIXOUT,  r3
	rjmp  .-2276
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2642
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1962
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1970
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2620
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+64
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+74
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2002
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2010
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
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
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
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
	out   PIXOUT,  r3
	rjmp  .-2548
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r2
	rjmp  .-3168
	nop
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2726
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
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
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
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
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-4094
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
	rjmp  .-2980
	nop
	out   PIXOUT,  r3
	rjmp  .-3116
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
	nop
	out   PIXOUT,  r2
	rjmp  .-3054
	nop
	out   PIXOUT,  r3
	rjmp  .-3420
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2502
	out   PIXOUT,  r3
	rjmp  .-3142
	nop
	out   PIXOUT,  r3
	rjmp  .-3176
	out   PIXOUT,  r3
	rjmp  .-1468
	out   PIXOUT,  r3
	rjmp  .-2994
	out   PIXOUT,  r2
	rjmp  .-2990
	out   PIXOUT,  r3
	rjmp  .-2512
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2958
	out   PIXOUT,  r3
	rjmp  .-3312
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2954
	out   PIXOUT,  r2
	rjmp  .-3206

m80_tilerow_2:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2530
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2606
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-258
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3778
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1962
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3372
	rjmp  .-202
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3698
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3312
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-288
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2754
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-524
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3408
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2760
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3392
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3136
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2050
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3128
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3424
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3168
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2812
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2816
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3230
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2706
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2226
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3216
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3456
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3206
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3394
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-430
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3450
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3572
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2178
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2786
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2178
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-696
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4034
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-490
	nop
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3278
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3954
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3024
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3512
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3356
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3668
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3570
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
	rjmp  .-3632
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2274
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-562
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3572
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3580
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3146
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3500
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3508
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3438
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3568
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3576
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3656
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3664
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3092
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2362
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3752
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2362
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-678
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-680
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3134
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3612
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3150
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3730
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3738
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3644
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3696
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3810
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3590
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1062
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3786
	out   PIXOUT,  r3
	rjmp  .-754
	out   PIXOUT,  r2
	rjmp  .-3790
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-744
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3752
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3246
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3768
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3262
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-3842
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3898
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3800
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3906
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3816
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-864
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1116
	rjmp  .-950
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-834
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3920
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3348
	rjmp  .+30
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3970
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3970
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3290
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3796
	rjmp  .-3970
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3392
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3400
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2722
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2610
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2738
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3994
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1210
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3448
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3990
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4020
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3462
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4090
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2506
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2514
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3908
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1002
	rjmp  .+1312
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1290
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1000
	rjmp  .-1168
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2498
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2738
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3992
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1292
	rjmp  .-3796
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4072
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4080
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4088
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2850
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-4012
	rjmp  .-4032
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1076
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1080
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3538
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1088
	rjmp  .+1368
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4094
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4088
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2970
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3664
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3672
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3680
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3688
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3010
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3018
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3026
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3034
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2538
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2546
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2554
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1296
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1212
	rjmp  .-3982
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3026
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2738
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1234
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3170
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3178
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3186
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3730
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2602
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2386
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3860
	rjmp  .+1164
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2402
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2818
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3854
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1316
	rjmp  .+62
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1336
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1356
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3896
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2714
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3290
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2698
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2946
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1376
	rjmp  .-3968
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1394
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4000
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1428
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3890
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2074
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2082
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1452
	rjmp  .+414
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3998
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-4006
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1754
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1744
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1734
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1516
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1524
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2146
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1522
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3994
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2170
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1520
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1554
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2186
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1544
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1578
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2218
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4058
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1620
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1628
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4082
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1644
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2226
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1660
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1620
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1658
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1632
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2258
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1632
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2282
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1664
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2298
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1664
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2314
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1730
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2330
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2338
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1754
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1732
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1740
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2370
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1786
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2386
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1820
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2402
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2410
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2418
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2394
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2442
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+248
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2090
	rjmp  .-2302
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2978
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1828
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1212
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1794
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3234
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2018
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1868
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3034
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3042
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+180
	rjmp  .+560
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2162
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3706
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2554
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3082
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2578
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3962
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+122
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2320
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2044
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2626
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2064
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1996
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
	rjmp  .-3946
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2478
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2108
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1994
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2698
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-2258
	nop
	out   PIXOUT,  r2
	rjmp  .-2266
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
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
	ijmp
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
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1314
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	ijmp
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	add   ZH,      r19
	ijmp
	out   PIXOUT,  r2
	rjmp  .-1394
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1402
	out   PIXOUT,  r3
	rjmp  .-2538
	out   PIXOUT,  r3
	rjmp  .
	add   ZH,      r19
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
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	rjmp  .
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2710
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
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
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .-2798
	rjmp  .
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	rjmp  .
	rjmp  .+2070

m80_tilerow_3:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3218
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3738
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1194
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-706
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1210
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-722
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-730
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-160
	rjmp  .-2814
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-290
	rjmp  .-2808
	out   PIXOUT,  r2
	rjmp  .-292
	out   PIXOUT,  r2
	rjmp  .-2684
	out   PIXOUT,  r3
	rjmp  .-278
	rjmp  .+1994
	rjmp  .-3158
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-576
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-470
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-604
	rjmp  .+1756
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-202
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-204
	rjmp  .-166
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1306
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1314
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2784
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-220
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-210
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3208
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3394
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2828
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2832
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2840
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
	rjmp  .-2990
	rjmp  .-2900
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1410
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2066
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2074
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3474
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-618
	rjmp  .+110
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3490
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2852
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-320
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3336
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2154
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3530
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2268
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2972
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1514
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2866
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3570
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1538
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3586
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3004
	rjmp  .-2962
	out   PIXOUT,  r3
	rjmp  .+1660
	out   PIXOUT,  r3
	rjmp  .+1722
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3036
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-580
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-436
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2988
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3674
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1986
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-480
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-468
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3690
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2428
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3706
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-524
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3148
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-982
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3092
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-560
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3746
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-476
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-498
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-506
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-506
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1754
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3228
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-620
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-954
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2556
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3418
	out   PIXOUT,  r3
	rjmp  .+1420
	out   PIXOUT,  r2
	rjmp  .-546
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-556
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-672
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-572
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-688
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3296
	rjmp  .+30
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3316
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-604
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3408
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+1392
	rjmp  .+6
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-322
	rjmp  .-3780
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3332
	rjmp  .+1430
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-338
	rjmp  .-3460
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-108
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3380
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3388
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-676
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3978
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3986
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2426
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-852
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-716
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+990
	rjmp  .+1414
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-732
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-714
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-706
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1284
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3484
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-626
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3428
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3508
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1304
	rjmp  .+1348
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1292
	rjmp  .-3912
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+1200
	rjmp  .-3736
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-828
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-786
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2330
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-658
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3572
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-818
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3516
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+1136
	rjmp  .+1432
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3794
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-892
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2314
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-364
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-330
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1028
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-346
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3588
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-762
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-914
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-964
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2698
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2706
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2714
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2722
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1004
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1012
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1020
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1028
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3756
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3764
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3772
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3708
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-434
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3100
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1188
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1578
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+912
	rjmp  .+1258
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+904
	rjmp  .-1360
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+896
	rjmp  .-2434
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1082
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1090
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1140
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1148
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1156
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1146
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3778
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3836
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+886
	rjmp  .-1444
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3932
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+792
	rjmp  .-1416
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1186
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-1724
	rjmp  .+14
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+840
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+850
	rjmp  .+3286
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1440
	rjmp  .-3972
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3972
	rjmp  .-4096
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2530
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2538
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+812
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+804
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1738
	rjmp  .+3278
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1274
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1740
	rjmp  .-2162
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1732
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1722
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+760
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4076
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4084
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4092
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4082
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-802
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+730
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1508
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+722
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1524
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1532
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+698
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-866
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+666
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+658
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+642
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-906
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+626
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+618
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+618
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-980
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-988
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-954
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-962
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1012
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-978
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-986
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-994
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+546
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1010
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1018
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+522
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+506
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1050
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+490
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+474
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1082
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1090
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1098
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-970
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-906
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1122
	out   PIXOUT,  r3
	rjmp  .-2076
	out   PIXOUT,  r2
	rjmp  .+328
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3106
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1546
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+410
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2324
	rjmp  .+248
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2096
	rjmp  .-1458
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1722
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+354
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+232
	rjmp  .+2844
	out   PIXOUT,  r3
	rjmp  .-2020
	out   PIXOUT,  r2
	rjmp  .+270
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1796
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1912
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2134
	nop
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2288
	nop
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+326
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1836
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3098
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+274
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+300
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1562
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+250
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+242
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+210
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1314
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+270
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+210
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1338
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2036
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2154
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2524
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+146
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1378
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
	add   ZH,      r19
	ld    r18,     X+
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
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
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
	rjmp  .-2190
	out   PIXOUT,  r2
	rjmp  .
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1462
	out   PIXOUT,  r3
	rjmp  .-76
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-96
	out   PIXOUT,  r2
	rjmp  .-2258
	nop
	out   PIXOUT,  r2
	rjmp  .-106
	nop
	out   PIXOUT,  r3
	rjmp  .-112
	out   PIXOUT,  r2
	rjmp  .-108
	nop
	out   PIXOUT,  r2
	rjmp  .-126
	out   PIXOUT,  r3
	rjmp  .
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1378
	out   PIXOUT,  r2
	rjmp  .
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
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
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
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
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
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
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	ld    r18,     X+
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
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	mul   r18,     r20
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
	rjmp  .+2056
	nop
	out   PIXOUT,  r3
	rjmp  .+2064
	nop

m80_tilerow_4:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1898
	out   PIXOUT,  r3
	rjmp  .-2724
	out   PIXOUT,  r2
	rjmp  .-2648
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-318
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2616
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2624
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3778
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3786
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2720
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2850
	nop
	out   PIXOUT,  r2
	rjmp  .-2852
	out   PIXOUT,  r3
	rjmp  .-2642
	out   PIXOUT,  r3
	rjmp  .-2838
	out   PIXOUT,  r3
	rjmp  .+1990
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-230
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2760
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3164
	nop
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-108
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2764
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1354
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-66
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1920
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2780
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-82
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1916
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-558
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1570
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1872
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-582
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2796
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1100
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1670
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-566
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2832
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-554
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2154
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1850
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2170
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4018
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-574
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-562
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-414
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2210
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2260
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1786
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-642
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-702
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2250
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2258
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2266
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2316
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-784
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-750
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2906
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2746
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1578
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2762
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2916
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1624
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2932
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2794
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2362
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2370
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3540
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2386
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1568
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1610
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-550
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3762
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3020
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2834
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3036
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+764
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+756
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3052
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3060
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-942
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3180
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3298
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+708
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3100
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1496
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3116
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+676
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-896
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3284
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2338
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1030
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3164
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-994
	rjmp  .+270
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-926
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1432
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1070
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1050
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2668
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2114
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1102
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
	out   PIXOUT,  r3
	rjmp  .-2162
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3276
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3090
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3242
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1970
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+500
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3266
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3324
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1198
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3186
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1162
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1222
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+1282
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3364
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3372
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3388
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3346
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+404
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3512
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1286
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3378
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1192
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1182
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1266
	rjmp  .-3636
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-986
	rjmp  .-3628
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3154
	rjmp  .-3562
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1130
	rjmp  .+1430
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1350
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1064
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2906
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3500
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1042
	rjmp  .+118
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3474
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2202
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2434
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2442
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2450
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2458
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3378
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
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1470
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1478
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1486
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2402
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1914
	rjmp  .-1628
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3658
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3732
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3832
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3660
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3668
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3676
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3642
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3650
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1234
	rjmp  .-1704
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3708
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3716
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3538
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+68
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1614
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+908
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1630
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2666
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1646
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3780
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3746
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3796
	out   PIXOUT,  r3
	rjmp  .-1836
	out   PIXOUT,  r2
	rjmp  .-2538
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3226
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3234
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3284
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3650
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3698
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1578
	rjmp  .+3270
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1568
	rjmp  .-3242
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2634
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3884
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3892
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1500
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1490
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1480
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1790
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
	rjmp  .+734
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2834
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3234
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+710
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1838
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+694
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
	rjmp  .-3282
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3290
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+670
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3442
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3450
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+646
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3466
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3474
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+622
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+618
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3898
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+602
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3514
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+602
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+578
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3538
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+578
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
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3986
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+510
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+502
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4010
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
	rjmp  .-3498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+462
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
	rjmp  .-3530
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3682
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1808
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2538
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2538
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2150
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-508
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2538
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2610
	rjmp  .-2266
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2206
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2190
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2602
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2344
	rjmp  .+2826
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2618
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+266
	out   PIXOUT,  r3
	rjmp  .-2412
	out   PIXOUT,  r2
	rjmp  .-3074
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2634
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2650
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3818
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2286
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2266
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3234
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2310
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2318
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3866
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3586
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2350
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2324
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+224
	rjmp  .+430
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2178
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3874
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2414
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3938
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	rjmp  .-2388
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
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-2568
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
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
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
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2908
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .-2568
	out   PIXOUT,  r2
	rjmp  .-94
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	rjmp  .
	add   ZH,      r19
	out   PIXOUT,  r3
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
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
	rjmp  .-2480
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
	rjmp  .-2756
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
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
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	add   ZH,      r19
	ijmp
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
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
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-400
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	movw  ZL,      r0
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-1564
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
	rjmp  .-2990

m80_tilerow_5:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1186
	out   PIXOUT,  r3
	rjmp  .-3084
	out   PIXOUT,  r3
	rjmp  .+2084
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-84
	rjmp  .+2182
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2890
	rjmp  .+6
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2898
	rjmp  .-2960
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3866
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3010
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-552
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2040
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-124
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-406
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3354
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-592
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2994
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .+2008
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3082
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1660
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-826
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3086
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1338
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2642
	rjmp  .+2048
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1976
	rjmp  .+14
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3118
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1964
	rjmp  .-600
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-688
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-232
	rjmp  .+2022
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-704
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1950
	rjmp  .-3102
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3946
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-538
	rjmp  .-668
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-4066
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1926
	rjmp  .+1876
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
	out   PIXOUT,  r3
	rjmp  .-3174
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2162
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2852
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-592
	rjmp  .-3210
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1498
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1846
	rjmp  .-3220
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+1838
	rjmp  .-796
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3230
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3262
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1838
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1546
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1822
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+1820
	nop
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3416
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3310
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3698
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3706
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2002
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3722
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3010
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3358
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3746
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2034
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1726
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1718
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3398
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-466
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-942
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1690
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3802
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3818
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-800
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3114
	rjmp  .+1744
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-574
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-824
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1804
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3146
	rjmp  .+1780
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3874
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3502
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-434
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-630
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-880
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3914
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-692
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3664
	rjmp  .+1734
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1884
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-914
	rjmp  .-3696
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-928
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3962
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3590
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3712
	rjmp  .+6
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .+1486
	rjmp  .-1052
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-530
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3736
	rjmp  .+1720
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3630
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-992
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+1462
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-538
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3662
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
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2354
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-814
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-4090
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1046
	rjmp  .+1278
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-594
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2060
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3786
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1104
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3758
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2386
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3750
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3782
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1278
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2226
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3920
	rjmp  .+1550
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3858
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3866
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2156
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2458
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1172
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1182
	rjmp  .+1516
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-3810
	rjmp  .-1090
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-786
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1080
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3910
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1260
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1430
	rjmp  .+46
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3910
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1496
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2194
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2362
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1056
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1278
	rjmp  .-4090
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-826
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2626
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2634
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2642
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2650
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3570
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-874
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-882
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4030
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4038
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4046
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2482
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3018
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2396
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1002
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1010
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+884
	rjmp  .-1224
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+876
	rjmp  .+1352
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+868
	rjmp  .-2786
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1446
	rjmp  .+3448
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1454
	rjmp  .+14
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-986
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+836
	rjmp  .-1506
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+828
	rjmp  .+3428
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3730
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3826
	rjmp  .+46
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1026
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1276
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+970
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2858
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+954
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+764
	rjmp  .+3376
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1550
	rjmp  .+3336
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2658
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1074
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1098
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1106
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1356
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1138
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2898
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1640
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1388
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+764
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1618
	rjmp  .+438
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1242
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1514
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1504
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+826
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+810
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+802
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+794
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+812
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+804
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+770
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+788
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+780
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+772
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
	rjmp  .-2706
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+714
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
	rjmp  .+690
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+682
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2754
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+666
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+658
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+676
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2786
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+660
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2802
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+644
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+636
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2826
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+620
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
	rjmp  .+570
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+588
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
	rjmp  .+538
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+556
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .+548
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+514
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2922
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+498
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
	rjmp  .-1466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1458
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2970
	out   PIXOUT,  r3
	rjmp  .-1998
	out   PIXOUT,  r2
	rjmp  .-2514
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3146
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1594
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1828
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3170
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2018
	rjmp  .-2176
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3034
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2344
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .+418
	rjmp  .-1826
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+196
	rjmp  .-2308
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1884
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3226
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2056
	rjmp  .-1706
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2046
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .+292
	rjmp  .-2340
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3258
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1898
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3114
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3122
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3130
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+290
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2090
	rjmp  .+2734
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+274
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1972
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
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2120
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3202
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3210
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1802
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3226
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
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
	movw  ZL,      r0
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
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
	nop
	out   PIXOUT,  r3
	nop
	movw  ZL,      r0
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2574
	out   PIXOUT,  r3
	rjmp  .-2700
	out   PIXOUT,  r3
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
	out   PIXOUT,  r2
	rjmp  .-2628
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
	rjmp  .-2636
	out   PIXOUT,  r2
	rjmp  .-2184
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	nop
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	nop
	out   PIXOUT,  r2
	rjmp  .-2700
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-2752
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r2
	rjmp  .-62
	out   PIXOUT,  r3
	rjmp  .-1950
	out   PIXOUT,  r2
	rjmp  .-1746
	movw  ZL,      r0
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r2
	rjmp  .
	out   PIXOUT,  r3
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .
	nop
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
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
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2856
	movw  ZL,      r0
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
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
	rjmp  .-2802
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
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2818
	nop
	out   PIXOUT,  r3
	rjmp  .-2826
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	movw  ZL,      r0
	add   ZH,      r19
	ld    r18,     X+
	mul   r18,     r20
	ijmp
	nop
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r3
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r2
	mul   r18,     r20
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	add   ZH,      r19
	out   PIXOUT,  r3
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	mul   r18,     r20
	ijmp
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	ijmp
	nop

m80_tilerow_6:

	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3746
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3914
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2554
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2052
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2044
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2036
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+2028
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3802
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3810
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2664
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-362
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-356
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-346
	nop
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1980
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1156
	out   PIXOUT,  r3
	rjmp  .-350
	out   PIXOUT,  r2
	rjmp  .-2746
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3354
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2130
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2736
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2742
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-356
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3056
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2506
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-502
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3248
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3256
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3954
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3962
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3970
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2778
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1828
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4002
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3270
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4018
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2852
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2682
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-796
	nop
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2234
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4058
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-638
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1762
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4082
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4090
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-722
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1722
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-738
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-3602
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2778
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2930
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2938
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1762
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1678
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1778
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3024
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3446
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1802
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-868
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-834
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-842
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1810
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1850
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-866
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-3510
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1866
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2434
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2410
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3128
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1858
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3090
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1524
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2898
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2490
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3176
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-878
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-778
	rjmp  .+1488
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3146
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2530
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2538
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3026
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1444
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-818
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2018
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1084
	rjmp  .-3724
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3074
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1388
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-850
	rjmp  .-1962
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2618
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1124
	rjmp  .-1018
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3258
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1114
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2730
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1130
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
	out   PIXOUT,  r3
	rjmp  .-2634
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .+1340
	rjmp  .-1122
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2146
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2562
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2162
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2074
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2498
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1010
	rjmp  .+1400
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3432
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-1348
	rjmp  .+1390
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1034
	rjmp  .-1586
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3456
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3838
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2794
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3282
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+1194
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2578
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .+1172
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2802
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2554
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2698
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-1330
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1106
	nop
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2874
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3330
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3514
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1088
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1310
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4006
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2370
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3554
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3386
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2794
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2394
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2906
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2914
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2922
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3442
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2442
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2450
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2458
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3704
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3712
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3720
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3050
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2762
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3690
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2738
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3796
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3570
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3578
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3586
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2962
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2970
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3578
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3618
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3626
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1558
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3130
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1574
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1426
	out   PIXOUT,  r3
	rjmp  .-1366
	out   PIXOUT,  r2
	rjmp  .-1576
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3138
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3880
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3690
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3066
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3226
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2706
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
	rjmp  .-1630
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2714
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2722
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3770
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .+752
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-1800
	nop
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
	rjmp  .-1486
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1476
	out   PIXOUT,  r3
	movw  ZL,      r0
	out   PIXOUT,  r2
	rjmp  .-1466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1750
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1758
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1766
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1748
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1756
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1790
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1772
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1780
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1788
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
	rjmp  .-1846
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
	rjmp  .-1870
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1878
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1894
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1902
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1884
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2962
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1900
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2978
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1916
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1924
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3002
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1940
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
	rjmp  .-1990
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1972
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3050
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3058
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2022
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2004
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2012
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2046
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3098
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2062
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
	rjmp  .-4026
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4018
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3146
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+248
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3682
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-2978
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .+242
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2530
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2530
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2204
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3226
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2562
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2538
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-1754
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3234
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2408
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2308
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-3274
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3826
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3282
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2586
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2594
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-2602
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2270
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-2562
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2286
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
	rjmp  .-2424
	nop
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
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
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
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .-2532
	nop
	out   PIXOUT,  r2
	rjmp  .-2402
	out   PIXOUT,  r2
	rjmp  .-2484
	nop
	out   PIXOUT,  r3
	rjmp  .-2402
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
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-2536
	nop
	out   PIXOUT,  r3
	rjmp  .-2566
	nop
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
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
	nop
	out   PIXOUT,  r2
	rjmp  .-32
	out   PIXOUT,  r2
	nop
	add   ZH,      r19
	ld    r18,     X+
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	mul   r18,     r20
	ijmp
	movw  ZL,      r0
	out   PIXOUT,  r3
	mul   r18,     r20
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	add   ZH,      r19
	out   PIXOUT,  r2
	ijmp
	nop
	out   PIXOUT,  r2
	nop
	out   PIXOUT,  r3
	nop
	add   ZH,      r19
	mul   r18,     r20
	out   PIXOUT,  r2
	ijmp
	out   PIXOUT,  r3
	rjmp  .-2542
	out   PIXOUT,  r2
	rjmp  .-2582
	nop
	out   PIXOUT,  r2
	rjmp  .-2684
	nop
	out   PIXOUT,  r3
	rjmp  .-2690
	out   PIXOUT,  r3
	nop
	out   PIXOUT,  r2
	rjmp  .-114
	out   PIXOUT,  r3
	rjmp  .
	out   PIXOUT,  r2
	rjmp  .-2638
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
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
	rjmp  .-3922
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3922
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3930
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3946
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3954
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2886
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2894
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3978
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3986
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3994
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4002
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3064
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-508
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3034
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-582
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-604
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4050
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4058
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-556
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4074
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4082
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-3014
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2138
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	ld    r18,     X+
	rjmp  .-3402
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
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2218
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2226
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2234
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2242
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2250
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2258
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2266
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2274
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2282
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2290
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2298
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2306
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	rjmp  .-3276
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2322
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2330
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2338
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2346
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2354
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2362
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2370
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2378
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2386
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2394
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2402
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2410
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2418
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2426
	out   PIXOUT,  r2
	out   PIXOUT,  r3
	movw  ZL,      r0
	rjmp  .-942
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2442
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2450
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2458
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2466
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2474
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2482
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
	ld    r18,     X+
	rjmp  .-2586
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
	ld    r18,     X+
	rjmp  .-2706
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2714
	out   PIXOUT,  r3
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
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2714
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2794
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2802
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-4082
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
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1372
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1370
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
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2834
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
	out   PIXOUT,  r3
	rjmp  .-3996
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
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-4052
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3050
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3058
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
	ld    r18,     X+
	rjmp  .-3082
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
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3130
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3138
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
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3082
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3186
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
	rjmp  .-3218
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3226
	out   PIXOUT,  r2
	movw  ZL,      r0
	out   PIXOUT,  r3
	rjmp  .-1668
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
	rjmp  .-3266
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3274
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
	rjmp  .-3218
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3154
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
	out   PIXOUT,  r3
	rjmp  .-1798
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-1788
	out   PIXOUT,  r3
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1778
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1768
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1776
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1784
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1788
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1796
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1808
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1812
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1820
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1828
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
	rjmp  .-1864
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3506
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1888
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1896
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3530
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1912
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-1920
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1924
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3562
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1940
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3578
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1956
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1964
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3602
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-1980
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3618
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3626
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2008
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2012
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3650
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3658
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2040
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2044
	out   PIXOUT,  r2
	ld    r18,     X+
	out   PIXOUT,  r3
	rjmp  .-2052
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2064
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3698
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2080
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
	ld    r18,     X+
	rjmp  .-3498
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3490
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3746
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3754
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3506
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3770
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3778
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3786
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3794
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3546
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3810
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3818
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3826
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3834
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3842
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3850
	out   PIXOUT,  r3
	ld    r18,     X+
	out   PIXOUT,  r2
	rjmp  .-3602
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3866
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3874
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3882
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3890
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3898
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3906
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-2288
	out   PIXOUT,  r2
	movw  ZL,      r0
	rjmp  .-2274
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3930
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
	ld    r18,     X+
	rjmp  .-3954
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3962
	out   PIXOUT,  r2
	ld    r18,     X+
	rjmp  .-2314
	nop
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3978
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3986
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-3994
	out   PIXOUT,  r2
	movw  ZL,      r0
	ld    r18,     X+
	rjmp  .-4002
