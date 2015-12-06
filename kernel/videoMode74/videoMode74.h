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
extern volatile unsigned char m74_bgcol;
extern volatile unsigned int  m74_rows;
extern volatile unsigned int  m74_tdesc;
extern volatile unsigned int  m74_tidx;
#if (M74_PAL_PTRE != 0)
extern volatile unsigned int  m74_pal;
#else
#define m74_pal    (M74_PAL_OFF)
#endif
#if (M74_COL0_RELOAD != 0)
#if (M74_COL0_PTRE != 0)
extern volatile unsigned int  m74_col0;
#else
#define m74_col0   (M74_COL0_OFF)
#endif
#endif
#if (M74_ROMMA_PTRE != 0)
extern volatile unsigned int  m74_romma;
#else
#define m74_romma  (M74_ROMMASK_OFF)
#endif
#if (M74_RAMMA_PTRE != 0)
extern volatile unsigned int  m74_ramma;
#else
#define m74_ramma  (M74_RAMMASK_OFF)
#endif
extern volatile unsigned char m74_ldsl;
extern volatile unsigned char m74_totc;
extern volatile unsigned char m74_skip;
extern volatile unsigned int  m74_fadd;
extern volatile unsigned int  m74_umod;
#if (M74_M3_ENABLE != 0)
#if (M74_M3_PTRE != 0)
extern volatile unsigned int  m74_mcadd;
#else
#define m74_mcadd  (M74_M3_OFF)
#endif
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
extern void M74_Finish(void);
extern void M74_VramRestore(void);
extern void M74_BlitSprite(unsigned int spo, unsigned char xl, unsigned char yl, unsigned char flg);


/*
** Sprite blitter flags, for use with M74_BlitSprite.
** Sprite importance is on bits 6-7, the smaller the number, the bigger the
** importance score (so 0 is the highest).
*/
#define M74_SPR_FLIPX 0x01U
#define M74_SPR_FLIPY 0x04U
#define M74_SPR_RAM   0x02U
#define M74_SPR_MASK  0x10U
#define M74_SPR_I3    0x00U
#define M74_SPR_I2    0x40U
#define M74_SPR_I1    0x80U
#define M74_SPR_I0    0xC0U
