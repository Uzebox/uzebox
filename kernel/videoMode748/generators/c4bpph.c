/*
**  Converts GIMP header to 4bpp Uzebox Mode 74 sprites or tiles, C header.
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
**  8. It must have 16 colors or less.
**
**  Produces result onto standard output, redirect into a ".h" file to get it
**  proper.
*/



/*  The GIMP header to use */
#include "tiles.h"


#include <stdio.h>
#include <stdlib.h>



int main(void)
{
 unsigned int  dlen = width * height;
 unsigned int  sp   = 0U;
 unsigned int  spc  = 0U;
 unsigned int  i;
 unsigned char c;
 unsigned char pal[16];

 /* Basic tests */

 if ((width & 0x7U) != 0U){
  fprintf(stderr, "Input width must be a multiple of 8!\n");
  return 1;
 }
 if (height != 8U){
  fprintf(stderr, "Input height must be 8!\n");
  return 1;
 }


 /* Create palette (pulling down input colors to Uzebox BBGGGRRR format) */

 for (i = 0U; i < 16U; i++){
  pal[i] =
      ((((unsigned char)(header_data_cmap[i][0])) >> 5) << 0) |
      ((((unsigned char)(header_data_cmap[i][1])) >> 5) << 3) |
      ((((unsigned char)(header_data_cmap[i][2])) >> 6) << 6);
 }

 printf("\n");
 printf("/* 4bpp tile palette (color 0 is transparent) */\n");
 printf("\n");
 printf("const unsigned char tilepal[] PROGMEM = {\n");

 for (i = 0U; i < 15U; i++){
  printf(" 0x%02XU,", pal[i]);
 }
 printf(" 0x%02XU\n};\n", pal[i]);


 /* Create some heading text */

 printf("\n");
 printf("/* 4bpp tile data (%u tiles; %u bytes) */\n", width >> 3, width << 2);
 printf("\n");
 printf("const unsigned char tiledata[] __attribute__ ((section (\".imgdata\"))) = {\n");
 printf(" ");

 /* Process image data */

 while (1){

  /* Collect two pixels */

  c  = (header_data[sp + 0U] & 0xFU) << 4;
  c |= (header_data[sp + 1U] & 0xFU) << 0;
  sp += 2U;
  if ((sp & 0x7U) == 0U){
   sp = sp + width - 8U; /* Sprite rows */
   if (sp >= dlen){      /* Advance to next sprite */
    sp = sp - dlen + 8U;
    spc ++;
   }
  }

  /* Output it */

  printf("0x%02XU", c);

  /* Check for bounds, line or loop termination */

  if (spc == (width >> 3)){
   printf("\n};\n");
   break;
  }

  if ((sp & 0x7U) == 0U){
   printf(",\n ");
  }else{
   printf(", ");
  }

 }

 return 0;
}
