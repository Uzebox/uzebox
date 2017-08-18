/*
**  Converts GIMP header to 2bpp Multicolor Uzebox Mode 74 img data, C header.
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
**  The input image height and width must be multiples of 8, determining the
**  tile dimensions of the result image. The number of colors used must be
**  below 16.
**
**  A background color index can be specified: tiles which use entirely this
**  color will be skipped from the image data, substituded by appropriate
**  tile indices in the VRAM (tile index 0xC0 which should be a clear tile).
**  To disable this, set the background color 16 or larger.
**  (This feature doesn't work as of now, but conceptually possible and would
**  be useful for larger apparent images)
**
**  The palette is mapped to the nearest colors Uzebox is capable to produce.
**
**  Produces result onto standard output, redirect into a ".h" file to get it
**  proper.
*/



/*  The GIMP header to use */
#include "outcasts_title_image.h"
/*  The background color (set it 16 or above to disable) */
#define  BGCOL     16U


#include <stdio.h>
#include <stdlib.h>
/*  Maximal image width. Mode 74 constrains to 24 tiles (192 columns) */
#define  MAXWIDTH  24U
/*  Maximal image height. Mode 74 constrains to 28 tiles (228 rows) */
#define  MAXHEIGHT 28U
/*  Should be enough. Usually no more than about 160 may be possible due to
**  RAM constraints, this number allows for full screen non-rectangular
**  layouts with blank areas. */
#define  MAXTILES  (MAXWIDTH * MAXHEIGHT)
/*  Maximal RAM. No more than 4K, but of course usually a lot less. */
#define  MAXRAM    4096U



int main(void)
{
 unsigned char tiles[MAXTILES][64]; /* Here still using 8 bits per pixel */
 unsigned char tilea[MAXTILES][4];  /* Attributes (color indices) */
 unsigned char bufmc[MAXRAM];
 unsigned char bufat[MAXTILES * 2U];
 unsigned char pal[16U];
 unsigned int  tlx = width >> 3;
 unsigned int  tly = height >> 3;
 unsigned int  tlc = tlx * tly;
 unsigned int  i;
 unsigned int  ii;
 unsigned int  j;
 unsigned int  jj;

 /* Basic tests */

 if ((width & 0x7U) != 0U){
  fprintf(stderr, "Input width must be a multiple of 8!\n");
  return 1;
 }
 if ((height & 0x7U) != 0U){
  fprintf(stderr, "Input height must be a multiple of 8!\n");
  return 1;
 }
 if (width > (MAXWIDTH * 8U)){
  fprintf(stderr, "Input width must not exceed %u tiles!\n", MAXWIDTH);
  return 1;
 }
 if (height > (MAXHEIGHT * 8U)){
  fprintf(stderr, "Input height must not exceed %u tiles!\n", MAXHEIGHT);
  return 1;
 }

 /* Convert input to tiled */

 for (j = 0U; j < tly; j++){
  for (i = 0U; i < tlx; i++){
   for (jj = 0U; jj < 8U; jj++){
    for (ii = 0U; ii < 8U; ii++){
     tiles[(j * tlx) + i][(jj * 8) + ii] =
         header_data[(((j * 8U) + jj) * width) + ((i * 8U) + ii)];
    }
   }
  }
 }

 /* Try multicolor conversion */

 for (i = 0U; i < tlc; i++){
  ii = 0U; /* Count of colors so far in the tile */
  for (j = 0U; j < 64U; j++){
   for (jj = 0U; jj < ii; jj++){
    if (tiles[i][j] == tilea[i][jj]){ break; } /* Matching attribute found */
   }
   if (jj == ii){ /* No matching attribute */
    if (ii == 4U){
     fprintf(stderr, "More than 4 colors in tile %u:%u at pixel %u:%u (%u:%u abs) (x:y)!\n",
         (i % tlx),
         (i / tlx),
         (j % 8U),
         (j / 8U),
         ((i % tlx) * 8U) + (j % 8U),
         ((i / tlx) * 8U) + (j / 8U));
     return 1;
    }
    if (tiles[i][j] >= 16U){
     fprintf(stderr, "Color index >= 16 at pixel %u:%u (x:y)!\n",
         ((i % tlx) * 8U) + (j % 8U),
         ((i / tlx) * 8U) + (j / 8U));
     return 1;
    }
    tilea[i][ii] = tiles[i][j];
    ii ++;
   }
   tiles[i][j]  = jj;
  }
  for (jj = ii; ii < 4U; ii++){
   tilea[i][jj] = BGCOL & 0xFU;
  }
 }

 /* Prepare attribute buffer. For now don't do any compacting (I will
 ** implement that later, it needs some calculations), but it could be done
 ** here, maybe requiring a second pass fixing the filler tiles. Including a
 ** compacted source is neither trivial: count of MC tiles to generate is not
 ** necessarily the width of the image. The convert back to linear also needs
 ** extra consideration... */

 i = 0U;
 while (1){
  bufat[(i << 1) + 0U] = (tilea[i][0] << 4) + tilea[i][1];
  bufat[(i << 1) + 1U] = (tilea[i][2] << 4) + tilea[i][3];
  i ++;
  if (i == tlc){ break; }
 }

 if (tlc > (MAXRAM >> 4)){
  fprintf(stderr, "Too many tiles to fit (%u)!\n", tlc);
 }

 /* Convert tile data back to linear. This wouldn't support compacting. */

 for (j = 0U; j < tly; j++){
  for (i = 0U; i < tlx; i++){
   for (jj = 0U; jj < 8U; jj++){
    bufmc[(((j * 8U) + jj) * (tlx << 1)) + ((i * 2U) + 0U)] =
         (tiles[(j * tlx) + i][(jj * 8) + 0U] << 6) |
         (tiles[(j * tlx) + i][(jj * 8) + 1U] << 4) |
         (tiles[(j * tlx) + i][(jj * 8) + 2U] << 2) |
         (tiles[(j * tlx) + i][(jj * 8) + 3U] << 0);
    bufmc[(((j * 8U) + jj) * (tlx << 1)) + ((i * 2U) + 1U)] =
         (tiles[(j * tlx) + i][(jj * 8) + 4U] << 6) |
         (tiles[(j * tlx) + i][(jj * 8) + 5U] << 4) |
         (tiles[(j * tlx) + i][(jj * 8) + 6U] << 2) |
         (tiles[(j * tlx) + i][(jj * 8) + 7U] << 0);
   }
  }
 }

 /* Create palette (pulling down input colors to Uzebox BBGGGRRR format) */

 for (i = 0U; i < 16U; i++){
  pal[i] =
      ((((unsigned char)(header_data_cmap[i][0])) >> 5) << 0) |
      ((((unsigned char)(header_data_cmap[i][1])) >> 5) << 3) |
      ((((unsigned char)(header_data_cmap[i][2])) >> 6) << 6);
 }


 /* From now generate output */


 /* Palette */

 printf("\n");
 printf("/* Multicolor image palette */\n");
 printf("\n");
 printf("const unsigned char imgpal[] PROGMEM = {\n");

 for (i = 0U; i < 15U; i++){
  printf(" 0x%02XU,", pal[i]);
 }
 printf(" 0x%02XU\n};\n", pal[i]);


 /* Attributes */

 printf("\n");
 printf("/* Multicolor image attributes (%ux%u, 2 bytes / tile) */\n", tlx, tly);
 printf("\n");
 printf("const unsigned char imgattr[] PROGMEM = {\n");

 i = 0U;
 while (1){
  printf(" 0x%02XU, 0x%02XU", bufat[(i << 1)], bufat[(i << 1) + 1U]);
  i ++;
  if (i == (tlx * tly)){
   printf("\n};\n");
   break;
  }
  printf(",");
  if ((i % tlx) == 0U){
   printf("\n");
  }
 }


 /* Image data */

 printf("\n");
 printf("/* Multicolor image data (%u bytes) */\n", tlc * 16U);
 printf("\n");
 printf("const unsigned char imgdata[] PROGMEM = {\n");

 i = 0U;
 while (1){
  printf(" 0x%02XU", bufmc[i]);
  i ++;
  if (i == (tlc * 16U)){
   printf("\n};\n");
   break;
  }
  printf(",");
  if ((i & 0xFU) == 0U){
   printf("\n");
  }
 }


 return 0;
}
