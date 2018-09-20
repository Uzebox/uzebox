/*
**  Converts GIMP header to 1bpp Uzebox Mode 74 masks, C header.
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
**  The input image must be 8 rows tall, the width can be any multiple of 8.
**
**  The palette is ignored. Index 0 will become background (zero), index 1
**  will become foreground (one).
**
**  Produces result onto standard output, redirect into a ".h" file to get it
**  proper.
*/



/*  The GIMP header to use */
#include "masks.h"


#include <stdio.h>
#include <stdlib.h>



int main(void)
{
 unsigned int  dlen = width * height;
 unsigned int  sp   = 0U;
 unsigned int  spc  = 0U;
 unsigned char c;

 /* Basic tests */

 if ((width & 0x7U) != 0U){
  fprintf(stderr, "Input width must be a multiple of 8!\n");
  return 1;
 }
 if (height != 8U){
  fprintf(stderr, "Input height must be 8!\n");
  return 1;
 }

 /* Create some heading text */

 printf("\n");
 printf("/* 1bpp mask (%u masks; %u bytes) */\n", width >> 3, width);
 printf("\n");
 printf("const unsigned char mskdata[] __attribute__ ((section (\".imgdata\"))) = {\n");
 printf(" ");

 /* Process image data */

 while (1){

  /* Collect eight pixels */

  c  = (header_data[sp + 0U] & 1U) << 7;
  c |= (header_data[sp + 1U] & 1U) << 6;
  c |= (header_data[sp + 2U] & 1U) << 5;
  c |= (header_data[sp + 3U] & 1U) << 4;
  c |= (header_data[sp + 4U] & 1U) << 3;
  c |= (header_data[sp + 5U] & 1U) << 2;
  c |= (header_data[sp + 6U] & 1U) << 1;
  c |= (header_data[sp + 7U] & 1U) << 0;
  sp = sp + width;       /* Rows */
  if (sp >= dlen){       /* Advance to next mask */
   sp = sp - dlen + 8U;
   spc ++;
  }

  /* Output it */

  printf("0x%02XU", c);

  /* Check for bounds, line or loop termination */

  if (spc == (width >> 3)){
   printf("\n};\n");
   break;
  }

  if (sp < width){
   printf(",\n ");
  }else{
   printf(", ");
  }

 }

 return 0;
}
