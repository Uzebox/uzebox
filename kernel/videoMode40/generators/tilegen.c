/*
**  Converts GIMP header to Uzebox Mode 40 tiles C source.
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
**  The input image must be n x 8 (width x height) where 'n' is a multiple of
**  8. It must have 2 colors.
**
**  Produces result onto standard output, redirect into a ".c" file to get it
**  proper.
*/



/*  The GIMP header to use */
#include "tileset.h"


#include <stdio.h>
#include <stdlib.h>



int main(void)
{
 unsigned int  tcnt = width / 8U;
 unsigned int  sp;
 unsigned int  i;
 unsigned int  j;
 unsigned char c;

 /* Basic tests */

 if ((width % 8U) != 0U){
  fprintf(stderr, "Input width must be a multiple of 8!\n");
  return 1;
 }
 if ((height) != 8U){
  fprintf(stderr, "Input height must be 8!\n");
  return 1;
 }
 if (tcnt > 256U){
  fprintf(stderr, "Input must have 256 or less tiles!\n");
  return 1;
 }


 /* Start generating the C file */

 printf("/*\n");
 printf("** Mode 40 Tileset of %u tiles\n", tcnt);
 printf("*/\n");
 printf("\n");
 printf("const char tileset[2048] __attribute__ ((aligned(256))) PROGMEM = {\n");


 /* Generate tile rows */

 for (i = 0U; i < height; i ++ ){

  sp = (i * width);

  for (j = 0U; j < width; j += 8U){
   c  = ((header_data[sp + j + 0U] & 1U)     ) |
        ((header_data[sp + j + 1U] & 1U) << 1) |
        ((header_data[sp + j + 2U] & 1U) << 2) |
        ((header_data[sp + j + 3U] & 1U) << 3) |
        ((header_data[sp + j + 4U] & 1U) << 4) |
        ((header_data[sp + j + 5U] & 1U) << 5) |
        ((header_data[sp + j + 6U] & 1U) << 6) |
        ((header_data[sp + j + 7U] & 1U) << 7);
   printf(" 0x%02XU,", c);
  }

  for (      ; j < 2048U; j += 8U){
   printf(" 0x00U,");
  }

  printf("\n");

 }

 printf("};\n");


 return 0;
}
