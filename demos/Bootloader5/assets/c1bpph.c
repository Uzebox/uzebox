/*
**  Converts GIMP header to 1bpp Bootloader image data.
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
**  The input image must be 8 rows tall, the width must be a multiple of 8.
**
**  The palette is ignored. Index 0 will become background (zero), index 1
**  will become foreground (one).
**
**  Produces result onto standard output, redirect into a ".h" file to get it
**  proper.
*/



/*  The GIMP header to use */
#include "charset.h"


#include <stdio.h>
#include <stdlib.h>



int main(void)
{
 unsigned int  sp = 0;
 unsigned int  i;
 unsigned int  j;
 unsigned char cl[5];

 /* Basic tests */

 if ((width & 0x7U) != 0U){
  fprintf(stderr, "Input width must be a multiple of 8!\n");
  return 1;
 }
 if (height != 8U){
  fprintf(stderr, "Input height must be 8!\n");
  return 1;
 }

 /* Process image data */

 while (1){

  /* Collect 5 x 8 tile data (right five columns are used) */

  for (i = 0U; i < 5U; i++){
   cl[i] = 0U;
   for (j = 0U; j < 8U; j++){
    cl[i] |= (header_data[sp + (width * j) + (4U - i)] & 1U) << j;
   }
  }
  sp += 8U;

  /* Output tile */

  printf("\t.byte 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\n",
       cl[0], cl[1], cl[2], cl[3], cl[4]);

  /* Check for end */

  if (sp == width){ break; }

 }

 return 0;
}
