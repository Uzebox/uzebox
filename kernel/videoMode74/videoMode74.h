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
extern volatile unsigned int  m74_pal;
#if (M74_COL0_RELOAD != 0)
extern volatile unsigned int  m74_col0;
#endif
extern volatile unsigned char m74_ldsl;
extern volatile unsigned char m74_totc;
extern volatile unsigned char m74_skip;
extern volatile unsigned int  m74_fadd;
extern volatile unsigned int  m74_umod;
#if (M74_M3_ENABLE != 0)
extern volatile unsigned int  m74_mcadd;
#endif

extern void M74_SetVram(unsigned int addr, unsigned char wdt, unsigned char hgt);
extern void M74_SetVramEx(unsigned int addr, unsigned char wdt, unsigned char hgt, unsigned char pt);
extern void M74_Finish(void);
