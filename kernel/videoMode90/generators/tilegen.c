/*
**  Converts GIMP header to Uzebox Mode 90 tiles assembly source.
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
**  The input image must be n x TILE_HEIGHT (width x height) where 'n' is a
**  multiple of 6. It must have 16 colors or less.
**
**  Produces result onto standard output, redirect into a ".s" file to get it
**  proper.
*/



/*  The GIMP header to use */
#include "tileset.h"


#include <stdio.h>
#include <stdlib.h>



int main(void)
{
 unsigned int  tcnt = width / 6U;
 unsigned int  tmsk;
 unsigned int  sp;
 unsigned int  i;
 unsigned int  j;
 unsigned char pal[16];

 /* Basic tests */

 if ((width % 6U) != 0U){
  fprintf(stderr, "Input width must be a multiple of 6!\n");
  return 1;
 }
 if (tcnt > 256U){
  fprintf(stderr, "Input must have 256 or less tiles!\n");
  return 1;
 }


 /* Create palette (pulling down input colors to Uzebox BBGGGRRR format) */

 for (i = 0U; i < 16U; i++){
  pal[i] =
      ((((unsigned char)(header_data_cmap[i][0])) >> 5) << 0) |
      ((((unsigned char)(header_data_cmap[i][1])) >> 5) << 3) |
      ((((unsigned char)(header_data_cmap[i][2])) >> 6) << 6);
 }


 /* Start generating the assembler file */

 printf("\n");
 printf(";\n");
 printf("; Mode 90 tileset of %u tiles at %u pixels height\n", tcnt, height);
 printf(";\n");
 printf("\n");
 printf("\n");
 printf("#include <avr/io.h>\n");
 printf("#define  PIXOUT _SFR_IO_ADDR(PORTC)\n");
 printf("\n");
 printf("\n");
 printf(".global m90_defpalette\n");
 printf(".global m90_deftilerows\n");
 printf("\n");
 printf("\n");
 printf(".section .text\n");
 printf("\n");
 printf("\n");
 printf("\n");


 /* Output the palette */

 printf("m90_defpalette:\n");
 printf("\t.byte");
 for (i = 0U; i < 7U; i++){ printf(" 0x%02X,", pal[i +  0U]); }
 printf(" 0x%02X\n", pal[ 7U]);
 printf("\t.byte");
 for (i = 0U; i < 7U; i++){ printf(" 0x%02X,", pal[i +  8U]); }
 printf(" 0x%02X\n", pal[15U]);
 printf("\n");
 printf("\n");
 printf("\n");


 /* Output entry jumps */


 printf("m90_deftilerows:\n");
 for (i = 0U; i < height; i++){
  printf("\tjmp   common_row_%u_s\n", i);
 }
 printf("\n");
 printf("\n");
 printf("\n");


 /* Generate tile rows */

 sp = 0U;
 if      (tcnt <=  16U){ tmsk = 0x0FU; }
 else if (tcnt <=  32U){ tmsk = 0x1FU; }
 else if (tcnt <=  64U){ tmsk = 0x3FU; }
 else if (tcnt <= 128U){ tmsk = 0x7FU; }
 else                  { tmsk = 0xFFU; }

 for (i = 0U; i < height; i++){

  /* Prolog code */

  printf("common_row_%u_s:\n", i);
  printf("\tldi   r18,     61      ; ( 4) For 60 tiles\n");
  printf("\tldi   r19,     7       ; ( 5)\n");
  printf("\tldi   r20,     lo8(pm(row_%u_code)) ; ( 6)\n", i);
  printf("\tldi   r21,     hi8(pm(row_%u_code)) ; ( 7)\n", i);
  printf("\tclr   r0               ; ( 8)\n");
  printf("\tclr   r1               ; ( 9)\n");
  printf("\tmovw  r22,     r0      ; (10)\n");
  printf("common_row_%u:\n", i);
  printf("\tdec   r18              ; ( 8) Cycles relative to tile output\n");
  printf("\tout   PIXOUT,  r0      ; ( 9)\n");
  printf("\tbreq  common_row_%u_e   ; (10 / 11)\n", i);
  printf("\tld    ZL,      X+      ; (12)\n");
  printf("\tout   PIXOUT,  r1      ; (13)\n");
  printf("\tandi  ZL,      0x%02X    ; (14)\n", tmsk);
  printf("\tmul   ZL,      r19     ; (16)\n");
  printf("\tout   PIXOUT,  r22     ; (17)\n");
  printf("\tmovw  ZL,      r0      ; (18)\n");
  printf("\tadd   ZL,      r20     ; (19)\n");
  printf("\tadc   ZH,      r21     ; (20)\n");
  printf("\tout   PIXOUT,  r23     ; (21)\n");
  printf("\tijmp                   ; (23)\n");
  printf("common_row_%u_e:\n", i);
  printf("\tnop                    ; (12)\n");
  printf("\tout   PIXOUT,  r1      ; (13)\n");
  printf("\tjmp   common_e         ; (16)\n");
  printf("\n");
  printf("\n");
  printf("row_%u_code:\n", i);

  /* Tile data */

  for (j = 0U; j < tcnt; j++){
   printf("\tmov   r0,      r%u\n", (header_data[sp + 2U] & 0xFU) + 2U);
   printf("\tout   PIXOUT,  r%u\n", (header_data[sp + 0U] & 0xFU) + 2U);
   printf("\tmov   r1,      r%u\n", (header_data[sp + 3U] & 0xFU) + 2U);
   printf("\tmov   r22,     r%u\n", (header_data[sp + 4U] & 0xFU) + 2U);
   printf("\tmov   r23,     r%u\n", (header_data[sp + 5U] & 0xFU) + 2U);
   printf("\tout   PIXOUT,  r%u\n", (header_data[sp + 1U] & 0xFU) + 2U);
   printf("\trjmp  common_row_%u\n", i);
   sp += 6U;
  }

  /* Some padding */

  printf("\n");
  printf("\n");
  printf("\n");

 }

 /* Common epilog code */

 printf("common_e:\n");
 printf("\tout   PIXOUT,  r22     ; (17)\n");
 printf("\tlpm   r0,      Z       ; (20) Dummy load (nop)\n");
 printf("\tout   PIXOUT,  r23     ; (21)\n");
 printf("\tclr   r0               ; (22)\n");
 printf("\trjmp  .                ; (24 = 0)\n");
 printf("\tout   PIXOUT,  r0      ; ( 1)\n");
 printf("\n");
 printf(";\n");
 printf("; Cycle budget at this point:\n");
 printf(";\n");
 printf(";   27 cycles used for prolog code\n");
 printf("; 1440 cycles used for tiles\n");
 printf(";    1 cycle used for terminating output\n");
 printf("; ----\n");
 printf("; 1468 cycles\n");
 printf(";\n");
 printf("\n");
 printf("\tlpm   r0,      Z       ; (1471) Dummy load (nop)\n");
 printf("\tlpm   r0,      Z       ; (1474) Dummy load (nop)\n");
 printf("\tlpm   r0,      Z       ; (1477) Dummy load (nop)\n");
 printf("\tret                    ; (1481)\n");


 return 0;
}
