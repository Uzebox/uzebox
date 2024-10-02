/*
**  Converts GIMP header to Uzebox Mode 80 tiles assembly source. Optimized
**  for 3 cycles wide pixels (Mode 9 substitute).
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
**  The input image width must be a multiple of 17. Typically it should have
**  two colors, however more can be added.
**
**  Produces result onto standard output, redirect into a ".s" file to get it
**  proper.
**
**  If it keeps producing tileset too complex errors, try to increase
**  the pre_jt[] entry of the row (how many 512 byte blocks to reserve for
**  tile code before the corresponding row jump table, best is to keep this 0,
**  but larger tilesets may require more).
*/



/*  The GIMP header to use */
#include "tileset_3cy.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


/* Number of cycles per tile */
#define TILE_CYCLES    18U
/* Height of tiles in pixels */
#define TILE_HEIGHT     8U


static const uint8_t pre_jt[TILE_HEIGHT] = {0};



/*
** Operation:
**
** First pass: Translate input image into rows to process further.
**
** Each row has 15 cycle slots (17 total, but the last two are not included as
** it is always an "ijmp" instruction to the next tile). This is filled with
** nops, then the color instructions are added where they belong.
**
** Second pass: Generate heads.
**
** This is a simple pass as 3 cycles wide pixels can always use 2 instruction
** word headers (pixel output + jump).
**
** Third pass: Generate tails.
**
** Tails are generated attempting to pack essential instructions at the end of
** the tile, keeping the beginning free for jumps. There are a few possible
** sequences of essential instructions, all these are tried until one fits (if
** the head had already taken one or another element, that's accounted for).
** If neither fits (considering the need for an rjmp between head and tail),
** halt with error.
**
** Fourth pass: Generate row structures.
**
** When generating a row, as first step, the tails of the row are all compared
** to those used in the next row (step omitted for last row). Ranking of rows
** start at 0. Those which have a match get an increment of 2 in rank. Then
** for the row after the same procedure is repeated (if row exists), providing
** an increment of 1 in rank.
**
** Row starts building with Jump table (.balign 512). Then tails begin, lowest
** rank first, with every tail, seeking for opportunities to merge into an
** already existing one (search starts at lowest address reachable by rjmp, to
** find opportunities from earlier rows as well). For rows having extra rjmp
** instructions, spare instruction slots within the Jump table are also sought
** after.
**
** Fifth pass: Producing assembly to standard out.
*/



/* Color output instructions (out PIXOUT, rx)*/
#define INS_C0     0x00U
#define INS_C1     0x01U
#define INS_C2     0x02U
#define INS_C3     0x03U
#define INS_C4     0x04U
#define INS_C5     0x05U
#define INS_C6     0x06U
#define INS_C7     0x07U
/* Color check mask (if AND result nonzero, then not color) */
#define INS_NOTCOL 0xF0U
/* Unfilled instruction slot (free to be filled later) */
#define INS_UNF    0x3FU
/* rjmp instruction allowing transfer (first and second cycle) */
#define INS_RJMP0  0x50U
#define INS_RJMP1  0xD0U
/* Next tile calculation, movw ZL, r0 */
#define INS_MOVW   0x11U
/* Next tile calculation, ld r18, X+ (first and second cycle) */
#define INS_LD0    0x52U
#define INS_LD1    0xD2U
/* Next tile calculation, add ZH, r19 ; r19: currently used row, high */
#define INS_ADD    0x13U
/* Next tile calculation, mul r18, r20 ; r20: block size (first and second cycle) */
#define INS_MUL0   0x54U
#define INS_MUL1   0xD4U
/* Terminating ijmp */
#define INS_IJMP0  0x55U
#define INS_IJMP1  0xD5U
/* nop (used, purposely nop) */
#define INS_NOP    0x16U
/* 2 cycle instruction flag */
#define INS_2CY    0x40U
/* Trailing part flag (of 2 cycle ins) */
#define INS_TAIL   0x80U


/* Maximal number of instructions to output */
#define MAX_INS    0x8000U

/* Number of essential instruction sequences */
#define ESS_SEQS_CNT    5U

/* Number of head sequences */
#define HEAD_SEQS_CNT   1U

/* Size of header instruction blocks in words */
#define HEAD_BLK_SIZE   2U

/* Head match length */
#define HEAD_SEQ_LEN    3U

/* Head generation masks: '0' positions must be color, '1' positions must be unfilled to match */
static uint8_t const head_seqs[HEAD_SEQS_CNT][HEAD_SEQ_LEN] = {
 {0, 1, 1}
};

/* Head code to add (to '1' positions if matched) */
static uint8_t const head_code[HEAD_SEQS_CNT][HEAD_SEQ_LEN] = {
 {0, INS_RJMP0, INS_RJMP1}
};

/* Essential instruction sequences to try in Tail generation */
static uint8_t const ess_seqs[ESS_SEQS_CNT][4] = {
 {INS_MOVW, INS_ADD,  INS_LD0,  INS_MUL0}, /* 1122 */
 {INS_MOVW, INS_LD0,  INS_ADD,  INS_MUL0}, /* 1212 */
 {INS_LD0,  INS_MOVW, INS_ADD,  INS_MUL0}, /* 2112 */
 {INS_MOVW, INS_LD0,  INS_MUL0, INS_ADD }, /* 1221 */
 {INS_LD0,  INS_MOVW, INS_MUL0, INS_ADD }  /* 2121 */
};

/* Instruction preparation buffer for the rows */
static uint8_t  ins_prep[TILE_HEIGHT][256][TILE_CYCLES - 1U];

/* Row ranks for the current tile row */
static uint8_t  row_rank[256];

/* Generation structure - instructions */
static uint8_t  ins_gen[MAX_INS];

/* Generation structure - absolute offsets for jumps */
static uint16_t jump_addr[MAX_INS];

/* Generation structure - Row entry positions */
static uint16_t row_entry[TILE_HEIGHT];




int main(void)
{
 uint_fast32_t ii;
 uint_fast32_t jj;
 uint_fast32_t kk;
 uint_fast32_t ss;
 uint_fast32_t nn;
 uint_fast32_t pp;
 uint_fast32_t posx;
 uint_fast32_t posy;
 uint_fast32_t pos;
 uint_fast32_t pjmp;
 uint_fast32_t bot;
 uint_fast8_t  col;
 uint_fast8_t  ins;
 uint8_t       tmpi[TILE_CYCLES - 1U];
 bool          match;

 (void)(header_data_cmap[0]); /* Clear unused warning for the palette */

 /* First pass: Read image and place color instructions */

 for (jj = 0U; jj < TILE_HEIGHT; jj ++){
  posx = 0U;
  posy = jj;
  for (ii = 0U; ii < 256U; ii ++){
   for (kk = 0U; kk < (TILE_CYCLES - 2U); kk ++){
    ins_prep[jj][ii][kk] = INS_UNF;
   }
   ins_prep[jj][ii][TILE_CYCLES - 2U] = INS_IJMP0;
   while ((posx + TILE_CYCLES) > width){
    posx = 0U;
    posy += TILE_HEIGHT;
    if ((posy + TILE_HEIGHT) > height){
     break;
    }
   }
   if (posy < height){
    /* Valid tile */
    pos = posx + (posy * width);
    col = 0xFFU;
    for (kk = 0U; kk < (TILE_CYCLES - 2U); kk ++){
     if ((header_data[pos + kk] & 0xFU) != col){
      col = header_data[pos + kk] & 0xFU;
      ins_prep[jj][ii][kk] = INS_C0 + col;
     }
    }
    for (kk = (TILE_CYCLES - 2U); kk < TILE_CYCLES; kk ++){
     if ((header_data[pos + kk] & 0xFU) != col){
      fprintf(stderr, "Tile 0x%02X, row %u doesn't have 3px wide end!\n", (unsigned int)(ii), (unsigned int)(jj));
      return 1;
     }
    }
   }else{
    /* No more tiles, add blank */
    ins_prep[jj][ii][0U] = INS_C0;
   }
   posx += TILE_CYCLES;
  }
 }


 /* Second pass: Generate heads */

 for (jj = 0U; jj < TILE_HEIGHT; jj ++){
  for (ii = 0U; ii < 256U; ii ++){
   for (ss = 0U; ss < HEAD_SEQS_CNT; ss ++){
    match = true;
    for (kk = 0U; kk < HEAD_SEQ_LEN; kk ++){
     if       (head_seqs[ss][kk] == 0U){ /* Must be color */
      if ((ins_prep[jj][ii][kk] & INS_NOTCOL) != 0U){
       match = false;
       break;
      }
     }else if (head_seqs[ss][kk] == 1U){ /* Must be unfilled */
      if (ins_prep[jj][ii][kk] != INS_UNF){
       match = false;
       break;
      }
     }else{
     }
    }
    if (match){ break; }
   }
   if (ss == HEAD_SEQS_CNT){
    fprintf(stderr, "Tile 0x%02X, row %u can not be fitted with header!\n", (unsigned int)(ii), (unsigned int)(jj));
    return 1;
   }else{
    for (kk = 0U; kk < HEAD_SEQ_LEN; kk ++){
     if (head_seqs[ss][kk] == 1U){
      ins_prep[jj][ii][kk] = head_code[ss][kk];
     }
    }
   }
  }
 }


 /* Third pass: Generate tails */

 for (jj = 0U; jj < TILE_HEIGHT; jj ++){
  for (ii = 0U; ii < 256U; ii ++){
   for (ss = 0U; ss < ESS_SEQS_CNT; ss ++){
    for (kk = 0U; kk < (TILE_CYCLES - 1U); kk ++){
     tmpi[kk] = ins_prep[jj][ii][kk];
    }
    nn = 4U;
    match = true;
    do{
     nn --;
     ins = ess_seqs[ss][nn];
     /* Check whether instruction is already added */
     for (kk = 0U; kk < (TILE_CYCLES - 2U); kk ++){
      if (tmpi[kk] == ins){ break; }
     }
     /* If not added, try to add from backwards */
     if (kk == (TILE_CYCLES - 2U)){
      kk --;
      do{
       if (tmpi[kk] == INS_UNF){
        if ((ins & INS_2CY) != 0U){
         /* 2cy instruction: If it fits, add, otherwise add NOP */
         if ((tmpi[kk + 1U] == INS_UNF) || (tmpi[kk + 1U] == INS_NOP)){
          tmpi[kk + 0U] = ins;
          tmpi[kk + 1U] = ins | INS_TAIL;
          break;
         }else{
          tmpi[kk] = INS_NOP;
         }
        }else{
         tmpi[kk] = ins;
         break;
        }
       }
       kk --;
      }while (kk != 0U);
      if (kk == 0U){
       match = false; /* Couldn't fit this tail */
       break;
      }
     }
    }while (nn != 0U);
    if (match){
     /* The tail was successfully fitted, so it can be fixed */
     for (kk = 0U; kk < (TILE_CYCLES - 2U); kk ++){
      ins_prep[jj][ii][kk] = tmpi[kk];
     }
     break;
    }
   }
   if (ss == ESS_SEQS_CNT){
    fprintf(stderr, "Tile 0x%02X, row %u can not be fitted with tail!\n", (unsigned int)(ii), (unsigned int)(jj));
    return 1;
   }
  }
 }


 /* Fourth pass: Generate row structures */

 for (jj = 0U; jj < MAX_INS; jj ++){
  ins_gen[jj] = INS_UNF;
 }
 for (jj = 0U; jj < TILE_HEIGHT; jj ++){

  /* Calculate row ranks */
  for (ii = 0U; ii < 256U; ii ++){
   row_rank[ii] = 0U;
   if (jj < (TILE_HEIGHT - 1U)){ /* Next row */
    for (ss = 0U; ss < 256U; ss ++){
     kk = (TILE_CYCLES - 2U);
     match = true;
     do{
      kk --;
      ins = ins_prep[jj + 1U][ss][kk];
      if ( ((ins == INS_UNF) && (ins_prep[jj + 1U][ss][kk + 1U] == INS_UNF)) ||
           (ins == INS_RJMP0) ){ break; } /* May jump onto the current row! */
      if (ins != ins_prep[jj][ii][kk]){
       match = false; /* No cross-jump possible */
       break;
      }
     }while(kk != 0U);
     if (match){
      row_rank[ii] |= 2U;
      break;
     }
    }
   }
   if (jj < (TILE_HEIGHT - 2U)){ /* Row after next */
    for (ss = 0U; ss < 256U; ss ++){
     kk = (TILE_CYCLES - 2U);
     match = true;
     do{
      kk --;
      ins = ins_prep[jj + 2U][ss][kk];
      if ( ((ins == INS_UNF) && (ins_prep[jj + 2U][ss][kk + 1U] == INS_UNF)) ||
           (ins == INS_RJMP0) ){ break; } /* May jump onto the current row! */
      if (ins != ins_prep[jj][ii][kk]){
       match = false; /* No cross-jump possible */
       break;
      }
     }while(kk != 0U);
     if (match){
      row_rank[ii] |= 1U;
      break;
     }
    }
   }
  }

  /* Create row entry points at next open 512 byte boundary */
  pos = MAX_INS;
  do{
   pos --;
   if (ins_gen[pos] != INS_UNF){
    pos ++;
    break;
   }
  }while (pos != 0U);
  posx = ((pos + 0xFFU) & 0xFFFFFF00U) + ((uint_fast32_t)(pre_jt[jj]) * 0x100U);
  if (posx >= (MAX_INS - 0x400U)){
   fprintf(stderr, "Ran out of instruction space, tileset is too big!\n");
   return 1;
  }
  bot  = posx + (0x100U * HEAD_BLK_SIZE);
  row_entry[jj] = posx;
  for (ii = 0U; ii < 256U; ii ++){
   kk = 0U;
   ss = 0U;
   while (ss < HEAD_BLK_SIZE){
    if ((ins_prep[jj][ii][kk] & INS_TAIL) == 0U){
     ins_gen[posx + (ii * HEAD_BLK_SIZE) + ss] = ins_prep[jj][ii][kk];
     ss ++;
     if (ins_prep[jj][ii][kk] == INS_RJMP0){
      jump_addr[posx + (ii * HEAD_BLK_SIZE) + ss - 1U] = MAX_INS;
      break; /* Address can only be added later */
     }
    }
    kk ++;
   }
  }

  /* Place tails, trying to fit onto anything in rjmp range for each,
  ** beginning with the lowest priority ones (ending up at the highest
  ** locations if have to be allocated anew, thus farthest from further row
  ** jump tables). The ins_gen table contains INS_UNF in open locations. */
  for (ii = 0U; ii < 1024U; ii ++){
   if ((row_rank[ii & 0xFFU] & 3U) == (ii >> 8)){
    nn = 0U;
    for (ss = 0U; ss < (TILE_CYCLES - 2U); ss ++){
     ins = ins_prep[jj][ii & 0xFFU][ss];
     if ((ins & INS_TAIL) == 0U){ nn ++; }
     if (ins == INS_RJMP0){ break; }
    }
    pjmp = (posx + ((ii & 0xFFU) * HEAD_BLK_SIZE)) + (nn - 1U); /* Position of jump */
    ss += 2U; /* First instruction belonging to tail */
    while (ss < (TILE_CYCLES - 1U)){
     posy = pjmp - 2047U; /* The lowest reachable address by the rjmp */
     if (posy >= MAX_INS){ posy = 0U; }
     /* Seek for a matching instruction sequence to this tail */
     match = false;
     while ((posy < MAX_INS) && (posy < (pjmp + 2048U))){
      nn = posy;
      kk = ss;
      while (kk < (TILE_CYCLES - 1U)){
       ins = ins_prep[jj][ii & 0xFFU][kk];
       if ((ins & INS_TAIL) == 0U){
        if (ins_gen[nn] == INS_RJMP0){
         if ( (ins != INS_RJMP0) &&
              (!((ins == INS_UNF) && (ins_prep[jj][ii & 0xFFU][kk + 1U] == INS_UNF))) ){
          break; /* No match */
         }
         if (jump_addr[nn] >= MAX_INS){
          break; /* No match due to unset jump */
         }
         nn = jump_addr[nn];
         kk += 2U;
        }else{
         if ( (ins_gen[nn] != ins) &&
              (!((ins_gen[nn] == INS_NOP) && (ins == INS_UNF))) ){
          break; /* No match */
         }
         nn ++;
         kk ++;
        }
       }else{
        kk ++;
       }
      }
      if (kk >= (TILE_CYCLES - 1U)){ /* Matched, so jump here */
       jump_addr[pjmp] = posy;
       match = true;
       break;
      }
      posy ++;
     }
     if (match){
      ss = (TILE_CYCLES - 1U); /* This row is done (remaining stretch matched an earlier) */
     }else{
      /* No match, seek for suitable empty area */
      nn = 0U;
      for (kk = ss; kk < (TILE_CYCLES - 1U); kk ++){
       ins = ins_prep[jj][ii & 0xFFU][kk];
       if ((ins & INS_TAIL) == 0U){ nn ++; }
       if ( (ins == INS_RJMP0) ||
            ( (ins == INS_UNF) && (ins_prep[jj][ii & 0xFFU][kk + 1U] == INS_UNF) ) ){
        /* rjmp could be used here, so this stretch is done, nn contains no. of ins */
        break;
       }
      }
      pp = ss;
      ss = kk + 2U;
      posy = pjmp - 2047U; /* The lowest reachable address by the rjmp */
      if (posy >= MAX_INS){ posy = 0U; }
      if ((ss < (TILE_CYCLES - 1U)) && ((posy + 2047U) < bot)){
       /* Prevent going too far up unless at end to prevent getting stuck
       ** there with single rjmps, unable to put the tail anywhere. */
       posy = bot - 2047U;
      }
      /* Seek for a suitable length of unfilled area to put this stretch at. */
      while ((posy < (MAX_INS - nn)) && (posy < (pjmp + 2048U))){
       for (kk = 0U; kk < nn; kk ++){
        if (ins_gen[posy + kk] != INS_UNF){ break; }
       }
       if (kk == nn){ /* Fits, so put it here */
        jump_addr[pjmp] = posy; /* Previous jump lands here */
        for (kk = pp; kk < (ss - 2U); kk ++){
         ins = ins_prep[jj][ii & 0xFFU][kk];
         if ((ins & INS_TAIL) == 0U){
          if (ins == INS_UNF){
           ins_gen[posy] = INS_NOP;
          }else{
           ins_gen[posy] = ins;
          }
          posy ++;
         }
        }
        if (ss < (TILE_CYCLES - 1U)){
         ins_gen[posy] = INS_RJMP0;
         pjmp = posy;
         posy ++;
        }
        if (posy > bot){
         bot = posy; /* Update bottom (where there is room to expand) */
        }
        match = true;
        break;
       }
       posy ++;
      }
      if (!match){
       fprintf(stderr, "Couldn't place tile 0x%02X, row %u, tileset likely too complex!\n", (unsigned int)(ii & 0xFFU), (unsigned int)(jj));
       fprintf(stderr, "Try to increase pre-jumptable reservation for row %u by editing pre_jt[]\n", (unsigned int)(jj));
       return 1;
      }
     }
    }
   }
  }
 }


 /* Fifth pass: Generate output assembly code */

 nn = MAX_INS;
 do{
  nn --;
  if (ins_gen[nn] != INS_UNF){
   nn ++;
   break;
  }
 }while (nn != 0U);

 printf("\n");
 printf(";\n");
 printf("; Mode 80 tileset; %u cycles wide, %u pixels tall, %u words\n", TILE_CYCLES, TILE_HEIGHT, (unsigned int)(nn));
 printf(";\n");
 printf("\n");
 printf("\n");
 printf("#include <avr/io.h>\n");
 printf("#define  PIXOUT _SFR_IO_ADDR(PORTC)\n");
 printf("#define  M80_CODEBLOCK_SIZE %u\n", HEAD_BLK_SIZE);
 printf("#define  M80_TILE_CYCLES %u\n", TILE_CYCLES);
 printf("\n");
 printf("\n");
 printf(".global m80_tilerows\n");
 for (jj = 0U; jj < TILE_HEIGHT; jj ++){
  printf(".global m80_tilerow_%u\n", (unsigned int)(jj));
 }
 printf("\n");
 printf("\n");
 printf(".section .text\n");
 printf("\n");
 printf(";\n");
 printf("; Note: Due to the inadequacy of the compiler, it is not possible to\n");
 printf("; resolve addresses here. This array so provides relative offsets only\n");
 printf("; (high byte) to m80_tilerow_0.\n");
 printf(";\n");
 printf("\n");
 printf("m80_tilerows:\n");
 for (jj = 0U; jj < TILE_HEIGHT; jj ++){
  printf("\t.byte 0x%02X\n", (unsigned int)(row_entry[jj] >> 8));
 }
 printf(".balign 2\n");
 printf("\n");
 printf("\n");
 printf("\n");
 printf(".section .text.Aligned512\n");
 printf(".balign 512\n");
 printf("\n");

 jj = 0U;
 for (pos = 0U; pos < nn; pos ++){
  if ((jj < TILE_HEIGHT) && (pos == row_entry[jj])){
   printf("\n");
   printf("m80_tilerow_%u:\n", (unsigned int)(jj));
   printf("\n");
   jj ++;
  }
  switch (ins_gen[pos]){
   case  0U: /* Colors */
   case  1U:
   case  2U:
   case  3U:
   case  4U:
   case  5U:
   case  6U:
   case  7U:
   case  8U:
   case  9U:
   case 10U:
   case 11U:
   case 12U:
   case 13U:
   case 14U:
   case 15U:
    printf ("\tout   PIXOUT,  r%u\n", (unsigned int)(ins_gen[pos] + 2U));
    break;
   case INS_RJMP0:
    posx = pos + 1U;
    posy = jump_addr[pos];
    if       (posx < posy){
     printf ("\trjmp  .+%u\n", (unsigned int)((posy - posx) * 2U));
    }else if (posx > posy){
     printf ("\trjmp  .-%u\n", (unsigned int)((posx - posy) * 2U));
    }else{
     printf ("\trjmp  .\n");
    }
    break;
   case INS_MOVW:
    printf ("\tmovw  ZL,      r0\n");
    break;
   case INS_LD0:
    printf ("\tld    r18,     X+\n");
    break;
   case INS_ADD:
    printf ("\tadd   ZH,      r19\n");
    break;
   case INS_MUL0:
    printf ("\tmul   r18,     r20\n");
    break;
   case INS_IJMP0:
    printf ("\tijmp\n");
    break;
   default:
    printf ("\tnop\n");
    break;
  }
 }


 return 0;
}
