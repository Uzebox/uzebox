/*
 * elo_test.h
 *
 * Custom positions for testing ELO level according to http://www.chessmaniac.com/ELORating/ELO_Chess_Rating.shtml
 *
 *  Created on: 24.7.2011
 *      Author: martin
 */

/**
 * Counting ELO (copied javascript from that page)
 *
function rating()
{
   elo1 = 1000
   if (document.form0.d1in.value=='f6' &&
       document.form0.d1fi.value=='f3' ) { elo1=2600 }
   if (document.form0.d1in.value=='c5' &&
       document.form0.d1fi.value=='d4' ) { elo1=1900 }
   if (document.form0.d1in.value=='c6' &&
       document.form0.d1fi.value=='d4' ) { elo1=1900 }
   if (document.form0.d1in.value=='b4' &&
       document.form0.d1fi.value=='c3' ) { elo1=1400 }
   if (document.form0.d1in.value=='c8' &&
       document.form0.d1fi.value=='a6' ) { elo1=1500 }
   if (document.form0.d1in.value=='f6' &&
       document.form0.d1fi.value=='g6' ) { elo1=1400 }
   if (document.form0.d1in.value=='e6' &&
       document.form0.d1fi.value=='e5' ) { elo1=1200 }
   if (document.form0.d1in.value=='c8' &&
       document.form0.d1fi.value=='d7' ) { elo1=1600 }

   elo2 = 1000
   if (document.form0.d2in.value=='g2' &&
       document.form0.d2fi.value=='e4' ) { elo2=2600 }
   if (document.form0.d2in.value=='g5' &&
       document.form0.d2fi.value=='h7' ) { elo2=1950 }
   if (document.form0.d2in.value=='h5' &&
       document.form0.d2fi.value=='g6' ) { elo2=1900 }
   if (document.form0.d2in.value=='g2' &&
       document.form0.d2fi.value=='f1' ) { elo2=1400 }
   if (document.form0.d2in.value=='g2' &&
       document.form0.d2fi.value=='d5' ) { elo2=1200 }
   if (document.form0.d2in.value=='f2' &&
       document.form0.d2fi.value=='f4' ) { elo2=1400 }

   elo3 = 1000
   if (document.form0.d3in.value=='c5' &&
       document.form0.d3fi.value=='c6' ) { elo3=2500 }
   if (document.form0.d3in.value=='g3' &&
       document.form0.d3fi.value=='g6' ) { elo3=2000 }
   if (document.form0.d3in.value=='e4' &&
       document.form0.d3fi.value=='e5' ) { elo3=1900 }
   if (document.form0.d3in.value=='g3' &&
       document.form0.d3fi.value=='g5' ) { elo3=1700 }
   if (document.form0.d3in.value=='e4' &&
       document.form0.d3fi.value=='d4' ) { elo3=1200 }
   if (document.form0.d3in.value=='d6' &&
       document.form0.d3fi.value=='e5' ) { elo3=1200 }

   elo4 = 1000
   if (document.form0.d4in.value=='e5' &&
       document.form0.d4fi.value=='e6' ) { elo4=2500 }
   if (document.form0.d4in.value=='b3' &&
       document.form0.d4fi.value=='f7' ) { elo4=1600 }
   if (document.form0.d4in.value=='b3' &&
       document.form0.d4fi.value=='c2' ) { elo4=1700 }
   if (document.form0.d4in.value=='b3' &&
       document.form0.d4fi.value=='d1' ) { elo4=1800 }

   elo5 = 1000
   if (document.form0.d5in.value=='e3' &&
       document.form0.d5fi.value=='c5' ) { elo5=2500 }
   if (document.form0.d5in.value=='f5' &&
       document.form0.d5fi.value=='h6' ) { elo5=2100 }
   if (document.form0.d5in.value=='e3' &&
       document.form0.d5fi.value=='h6' ) { elo5=1900 }
   if (document.form0.d5in.value=='f5' &&
       document.form0.d5fi.value=='g7' ) { elo5=1500 }
   if (document.form0.d5in.value=='f2' &&
       document.form0.d5fi.value=='g3' ) { elo5=1750 }
   if (document.form0.d5in.value=='c8' &&
       document.form0.d5fi.value=='f8' ) { elo5=1200 }
   if (document.form0.d5in.value=='f2' &&
       document.form0.d5fi.value=='h4' ) { elo5=1200 }
   if (document.form0.d5in.value=='e3' &&
       document.form0.d5fi.value=='b6' ) { elo5=1750 }
   if (document.form0.d5in.value=='e2' &&
       document.form0.d5fi.value=='c4' ) { elo5=1400 }


   elo6 = 1000
   if (document.form0.d6in.value=='g5' &&
       document.form0.d6fi.value=='f6' ) { elo6=2500 }
   if (document.form0.d6in.value=='c3' &&
       document.form0.d6fi.value=='d5' ) { elo6=1700 }
   if (document.form0.d6in.value=='c4' &&
       document.form0.d6fi.value=='b5' ) { elo6=1900 }
   if (document.form0.d6in.value=='f2' &&
       document.form0.d6fi.value=='f4' ) { elo6=1700 }
   if (document.form0.d6in.value=='a2' &&
       document.form0.d6fi.value=='a3' ) { elo6=1200 }
   if (document.form0.d6in.value=='e1' &&
       document.form0.d6fi.value=='e3' ) { elo6=1200 }

   elo7 = 1000
   if (document.form0.d7in.value=='f6' &&
       document.form0.d7fi.value=='h7' ) { elo7=2500 }
   if (document.form0.d7in.value=='f6' &&
       document.form0.d7fi.value=='e4' ) { elo7=1800 }
   if (document.form0.d7in.value=='g6' &&
       document.form0.d7fi.value=='g5' ) { elo7=1700 }
   if (document.form0.d7in.value=='a6' &&
       document.form0.d7fi.value=='a5' ) { elo7=1700 }
   if (document.form0.d7in.value=='g8' &&
       document.form0.d7fi.value=='h7' ) { elo7=1500 }

   elo8 = 1000
   if (document.form0.d8in.value=='b6' &&
       document.form0.d8fi.value=='d8' ) { elo8=2500 }
   if (document.form0.d8in.value=='c8' &&
       document.form0.d8fi.value=='e8' ) { elo8=1600 }

   elo9 = 1000
   if (document.form0.d9in.value=='e3' &&
       document.form0.d9fi.value=='d4' ) { elo9=2500 }
   if (document.form0.d9in.value=='e4' &&
       document.form0.d9fi.value=='g6' ) { elo9=1800 }
   if (document.form0.d9in.value=='e4' &&
       document.form0.d9fi.value=='h7' ) { elo9=1800 }
   if (document.form0.d9in.value=='e3' &&
       document.form0.d9fi.value=='h6' ) { elo9=1700 }
   if (document.form0.d9in.value=='d7' &&
       document.form0.d9fi.value=='b7' ) { elo9=1400 }

   elo10 = 1000
   if (document.form0.d10in.value=='d8' &&
       document.form0.d10fi.value=='d7' ) { elo9=2600 }
   if (document.form0.d10in.value=='f6' &&
       document.form0.d10fi.value=='e8' ) { elo9=2000 }
   if (document.form0.d10in.value=='h7' &&
       document.form0.d10fi.value=='h5' ) { elo9=1800 }
   if (document.form0.d10in.value=='c5' &&
       document.form0.d10fi.value=='d4' ) { elo9=1600 }
   if (document.form0.d10in.value=='c8' &&
       document.form0.d10fi.value=='a6' ) { elo9=1800 }
   if (document.form0.d10in.value=='a7' &&
       document.form0.d10fi.value=='a5' ) { elo9=1800 }
   if (document.form0.d10in.value=='f8' &&
       document.form0.d10fi.value=='e8' ) { elo9=1400 }
   if (document.form0.d10in.value=='d6' &&
       document.form0.d10fi.value=='d5' ) { elo9=1500 }


   document.form0.elo.value = (elo1+elo2+elo3+elo4+elo5+elo6+elo7+elo8+elo9+elo10)/10

}
*/

#ifndef ELO_TEST_H_
#define ELO_TEST_H_

void reset_board() {
	for (uint8_t integ = 0; integ <= 128; integ++) {
		if ((integ == 0) || (integ && 0x88)) {
			b[integ] = 0;
		}
	}
}

void test1() {
	reset_board();
	b[0x00] = 16 + 6;
	b[0x02] = 16 + 5;
	b[0x06] = 16 + 4;
	b[0x16] = 16 + 2;
	b[0x20] =  8 + 1;
	b[0x22] = 16 + 3;
	b[0x24] = 16 + 2;
	b[0x25] = 16 + 6;
	b[0x27] = 16 + 2;
	b[0x30] = 16 + 7;
	b[0x32] = 16 + 2;
	b[0x41] = 16 + 5;
	b[0x43] =  8 + 1;
	b[0x52] =  8 + 3;
	b[0x55] =  8 + 3;
	b[0x60] =  8 + 1;
	b[0x61] =  8 + 1;
	b[0x63] =  8 + 7;
	b[0x64] =  8 + 5;
	b[0x65] =  8 + 1;
	b[0x66] =  8 + 1;
	b[0x67] =  8 + 1;
	b[0x70] =  8 + 6;
	b[0x74] =  8 + 4;
	b[0x77] =  8 + 6;
	b[0x00] = 16 + 6;
	b[0x00] = 16 + 6;
	b[0x00] = 16 + 6;
	b[0x00] = 16 + 6;
	b[0x00] = 16 + 6;
	k = 8;
}

void test2() {
	reset_board();
	b[0x02] = 16 + 3;
	b[0x03] = 16 + 7;
	b[0x05] = 16 + 3;
	b[0x06] = 16 + 4;
	b[0x15] = 16 + 2;
	b[0x17] = 16 + 2;
	b[0x24] = 16 + 2;
	b[0x26] = 16 + 2;
	b[0x27] =  8 + 7;
	b[0x30] = 16 + 2;
	b[0x31] = 16 + 5;
	b[0x33] = 16 + 2;
	b[0x34] =  8 + 1;
	b[0x36] =  8 + 3;
	b[0x37] =  8 + 1;
	b[0x41] = 16 + 2;
	b[0x43] =  8 + 1;
	b[0x46] =  8 + 1;
	b[0x51] =  8 + 1;
	b[0x56] =  8 + 3;
	b[0x60] =  8 + 1;
	b[0x65] =  8 + 1;
	b[0x66] =  8 + 5;
	b[0x76] =  8 + 4;
	k = 16;
}

void test3() {
	reset_board();
	b[0x13] = 16 + 6;
	b[0x16] = 16 + 2;
	b[0x20] = 16 + 2;
	b[0x21] = 16 + 2;
	b[0x23] =  8 + 5;
	b[0x24] = 16 + 2;
	b[0x26] = 16 + 2;
	b[0x31] = 16 + 4;
	b[0x32] =  8 + 1;
	b[0x41] = 16 + 3;
	b[0x44] =  8 + 4;
	b[0x56] =  8 + 6;
	b[0x61] =  8 + 1;
	b[0x65] =  8 + 1;
	k = 16;
}

void test4() {
	reset_board();
	b[0x14] = 16 + 4;
	b[0x15] = 16 + 5;
	b[0x17] = 16 + 2;
	b[0x22] = 16 + 2;
	b[0x26] = 16 + 2;
	b[0x27] =  8 + 1;
	b[0x31] = 16 + 2;
	b[0x32] =  8 + 1;
	b[0x34] =  8 + 1;
	b[0x36] =  8 + 1;
	b[0x41] =  8 + 1;
	b[0x45] =  8 + 4;
	b[0x51] =  8 + 5;
	k = 16;
}

void test5() {
	reset_board();
	b[0x00] = 16 + 5;
	b[0x02] =  8 + 6;
	b[0x05] = 16 + 3;
	b[0x06] = 16 + 4;
	b[0x15] = 16 + 2;
	b[0x16] = 16 + 2;
	b[0x17] = 16 + 2;
	b[0x21] = 16 + 2;
	b[0x25] = 16 + 3;
	b[0x35] =  8 + 3;
	b[0x41] = 16 + 5;
	b[0x44] = 16 + 2;
	b[0x51] =  8 + 1;
	b[0x54] =  8 + 5;
	b[0x55] =  8 + 1;
	b[0x60] = 16 + 7;
	b[0x64] =  8 + 5;
	b[0x65] =  8 + 7;
	b[0x66] =  8 + 1;
	b[0x67] =  8 + 1;
	b[0x76] =  8 + 4;
	k = 16;
}

void test6() {
	reset_board();
	b[0x03] = 16 + 6;
	b[0x04] = 16 + 6;
	b[0x06] = 16 + 4;
	b[0x10] = 16 + 2;
	b[0x11] = 16 + 2;
	b[0x15] = 16 + 2;
	b[0x16] = 16 + 5;
	b[0x17] = 16 + 2;
	b[0x22] = 16 + 5;
	b[0x23] = 16 + 2;
	b[0x25] = 16 + 3;
	b[0x26] = 16 + 2;
	b[0x30] = 16 + 7;
	b[0x34] = 16 + 2;
	b[0x36] =  8 + 5;
	b[0x42] =  8 + 5;
	b[0x44] =  8 + 1;
	b[0x52] =  8 + 3;
	b[0x57] =  8 + 1;
	b[0x60] =  8 + 1;
	b[0x61] =  8 + 1;
	b[0x62] =  8 + 1;
	b[0x63] =  8 + 7;
	b[0x65] =  8 + 1;
	b[0x66] =  8 + 1;
	b[0x73] =  8 + 6;
	b[0x74] =  8 + 6;
	b[0x76] =  8 + 4;
	k = 16;
}

void test7() {
	reset_board();
	b[0x00] = 16 + 6;
	b[0x02] = 16 + 5;
	b[0x04] = 16 + 7;
	b[0x05] = 16 + 6;
	b[0x06] = 16 + 4;
	b[0x11] = 16 + 2;
	b[0x12] = 16 + 2;
	b[0x13] = 16 + 3;
	b[0x15] = 16 + 2;
	b[0x16] = 16 + 5;
	b[0x20] = 16 + 2;
	b[0x23] = 16 + 2;
	b[0x25] = 16 + 3;
	b[0x26] = 16 + 2;
	b[0x27] = 16 + 2;
	b[0x33] =  8 + 1;
	b[0x34] = 16 + 2;
	b[0x42] =  8 + 1;
	b[0x44] =  8 + 1;
	b[0x47] =  8 + 5;
	b[0x52] =  8 + 3;
	b[0x60] =  8 + 1;
	b[0x61] =  8 + 1;
	b[0x63] =  8 + 3;
	b[0x64] =  8 + 5;
	b[0x65] =  8 + 1;
	b[0x66] =  8 + 1;
	b[0x67] =  8 + 1;
	b[0x70] =  8 + 6;
	b[0x73] =  8 + 7;
	b[0x75] =  8 + 6;
	b[0x76] =  8 + 4;
	k = 8;
}

void test8() {
	reset_board();
	b[0x02] =  8 + 6;
	b[0x04] = 16 + 6;
	b[0x15] = 16 + 4;
	b[0x20] = 16 + 2;
	b[0x21] =  8 + 5;
	b[0x22] =  8 + 1;
	b[0x24] = 16 + 3;
	b[0x27] = 16 + 2;
	b[0x36] = 16 + 2;
	b[0x55] =  8 + 1;
	b[0x57] =  8 + 1;
	b[0x62] =  8 + 1;
	b[0x66] =  8 + 1;
	b[0x77] =  8 + 4;
	k = 16;
}

void test9() {
	reset_board();
	b[0x02] = 16 + 6;
	b[0x05] = 16 + 6;
	b[0x06] = 16 + 4;
	b[0x11] = 16 + 2;
	b[0x13] =  8 + 6;
	b[0x15] = 16 + 2;
	b[0x16] = 16 + 2;
	b[0x20] = 16 + 2;
	b[0x24] = 16 + 2;
	b[0x27] = 16 + 2;
	b[0x44] =  8 + 5;
	b[0x53] =  8 + 7;
	b[0x54] =  8 + 5;
	b[0x56] =  8 + 1;
	b[0x60] = 16 + 7;
	b[0x62] =  8 + 1;
	b[0x66] =  8 + 4;
	b[0x67] =  8 + 1;
	k = 16;
}

void test10() {
	reset_board();
	b[0x00] = 16 + 6;
	b[0x02] = 16 + 5;
	b[0x03] = 16 + 7;
	b[0x05] = 16 + 6;
	b[0x06] = 16 + 4;
	b[0x10] = 16 + 2;
	b[0x15] = 16 + 2;
	b[0x16] = 16 + 2;
	b[0x17] = 16 + 2;
	b[0x21] = 16 + 2;
	b[0x22] = 16 + 3;
	b[0x23] = 16 + 2;
	b[0x25] = 16 + 3;
	b[0x32] = 16 + 2;
	b[0x42] =  8 + 1;
	b[0x43] =  8 + 1;
	b[0x44] = 16 + 2;
	b[0x45] =  8 + 1;
	b[0x51] =  8 + 3;
	b[0x52] =  8 + 1;
	b[0x54] =  8 + 1;
	b[0x60] =  8 + 1;
	b[0x64] =  8 + 5;
	b[0x66] =  8 + 1;
	b[0x67] =  8 + 1;
	b[0x70] =  8 + 6;
	b[0x72] =  8 + 5;
	b[0x73] =  8 + 7;
	b[0x75] =  8 + 6;
	b[0x76] =  8 + 4;
	k = 8;
}

#endif /* ELO_TEST_H_ */
