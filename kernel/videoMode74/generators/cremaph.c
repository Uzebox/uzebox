/*
**  Converts a color remapping table to a Mode 74 C header.
**
**  By Sandor Zsuga (Jubatian)
**
**  Licensed under GNU General Public License version 3.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**  ---
**
**  The input table contains 16 color indices provided on the low 4 bits of
**  each byte. Generates a 256 byte remapping table from that, suitable for
**  inclusion in C code. Note: Index 0 should always contain zero
**  (transparent color).
**
**  Produces result onto standard output, redirect into a ".h" file to get it
**  proper.
*/



#include <stdio.h>
#include <stdlib.h>



/* The input remapping table */
static unsigned char const remaptb[16] = {
 0,  6,  7,  8,  9,  5,  1,  2,  3,  4, 13, 14, 15, 10, 11, 12
};



int main(void)
{
 unsigned int  i;
 unsigned char outtb[256];

 /* Create remapping table */

 for (i = 0U; i < 256U; i++){
  outtb[i] = ((remaptb[i >> 4] << 4) & 0xF0U) |
             (remaptb[i & 0xFU] & 0x0FU);
 }

 printf("\n");
 printf("/* Color remapping table */\n");
 printf("\n");
 printf("const unsigned char remaptb[] __attribute__ ((section (\".imgdata\"))) = {\n");

 for (i = 0U; i < 256U; i++){
  printf(" 0x%02XU", outtb[i]);
  if (i != 255U){ printf(","); }
  if ((i & 0x7U) == 7U){ printf("\n"); }
 }
 printf("};\n");

 return 0;
}
