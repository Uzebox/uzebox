/*
 *  Uzebox Kernel - Video Mode 74
 *  Copyright (C) 2015 Sandor Zsuga (Jubatian)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

/**
 * ===========================================================================
 *
 * This file contains function prototypes and exports for mode 74
 * For documentations, see the comments in videoMode74.s
 *
 * ===========================================================================
 */

#pragma once

extern volatile unsigned char m74_config;
#if (M74_ROWS_PTRE != 0)
extern volatile unsigned int  m74_rows;
#else
#define m74_rows   (M74_ROWS_OFF)
#endif
extern volatile unsigned int  m74_tdesc;
extern volatile unsigned int  m74_tidx;
#if (M74_PAL_PTRE != 0)
extern volatile unsigned int  m74_pal;
#else
#define m74_pal    (M74_PAL_OFF)
#endif
#if (M74_RESET_ENABLE != 0)
extern volatile unsigned int  m74_reset;
#endif
#if (M74_SD_ENABLE != 0)
extern volatile unsigned long m74_sdsec;
extern volatile unsigned long m74_sdoff;
extern volatile unsigned char m74_sdcnt;
extern volatile unsigned int  m74_sddst;
#endif
#if (M74_RTLIST_PTRE != 0)
extern volatile unsigned int  m74_rtlist;
#else
#define m74_rtlist (M74_RTLIST_OFF)
#endif
extern volatile unsigned char m74_rtmax;
extern volatile unsigned char m74_rtno;

#if (M74_VRAM_CONST == 0)
extern void M74_SetVram(unsigned int addr, unsigned char wdt, unsigned char hgt);
extern void M74_SetVramEx(unsigned int addr, unsigned char wdt, unsigned char hgt, unsigned char pt);
#endif
extern unsigned char M74_Finish(void);
extern void M74_VramMove(signed char x, signed char y);
extern void M74_VramFillCol(unsigned char y, unsigned int src, unsigned char incr);
extern void M74_VramFillRow(unsigned char x, unsigned int src, unsigned char incr);
extern void M74_VramFill(unsigned int src, unsigned char pitch);
extern void M74_RamTileFillRom(unsigned int  src, unsigned char dst);
extern void M74_RamTileFillRam(unsigned char src, unsigned char dst);
extern void M74_RamTileClear(unsigned char dst);
extern void M74_Halt(void);
extern void M74_Seq(void);
#if (M74_SPR_ENABLE != 0)
extern void M74_VramRestore(void);
extern void M74_ResReset(void);
extern void M74_BlitSprite(unsigned int spo, unsigned char xl, unsigned char yl, unsigned char flg);
#if ((M74_RECTB_OFF >> 8) != 0)
extern void M74_BlitSpriteCol(unsigned int spo, unsigned char xl, unsigned char yl, unsigned char flg, unsigned char col);
#endif
extern void M74_PutPixel(unsigned char col, unsigned char xl, unsigned char yl, unsigned char flg);
#endif


/*
** Sprite blitter flags, for use with M74_BlitSprite.
** Sprite importance is on bits 6-7, the smaller the number, the bigger the
** importance score (so 0 is the highest).
*/
#define M74_SPR_FLIPX 0x01U
#define M74_SPR_FLIPY 0x04U
#define M74_SPR_RAM   0x02U
#define M74_SPR_MASK  0x10U
#define M74_SPR_I3    0xC0U
#define M74_SPR_I2    0x80U
#define M74_SPR_I1    0x40U
#define M74_SPR_I0    0x00U


/*
** Configuration (m74_config) flags
*/
#define M74_CFG_M3DOUBLE         0x01U
#define M74_CFG_RAM_TDESC        0x02U
#define M74_CFG_RAM_TIDX         0x04U
#define M74_CFG_RAM_PALETTE      0x08U
#define M74_CFG_COL0_RELOAD      0x10U
#define M74_CFG_LOAD_SD          0x40U
#define M74_CFG_ENABLE           0x80U
