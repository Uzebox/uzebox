/*
 *  Uzebox Kernel - Video Mode 748
 *  Copyright (C) 2018 Sandor Zsuga (Jubatian)
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

#include <uzebox.h>


extern volatile u8  m74_config;
extern volatile u8  m74_discol;
#if (M74_ROWS_PTRE != 0)
extern volatile u16 m74_rows;
#else
#define m74_rows   (M74_ROWS_OFF)
#endif
extern volatile u16 m74_vaddr;
extern volatile u16 m74_paddr;
#if (M74_RESET_ENABLE != 0)
extern volatile u16 m74_reset;
#endif
extern volatile u8  m74_m4_bank;
extern volatile u16 m74_m4_addr;
extern volatile u8  m74_rtmax;
extern volatile u8  m74_rtbase;

#if (M74_VRAM_CONST == 0)
extern void M74_SetVram(u16 addr, u8 wdt, u8 hgt);
extern void M74_SetVramEx(u16 addr, u8 wdt, u8 hgt, u8 pt);
#endif
extern u8   M74_Finish(void);
extern void M74_PrepareM4Row(u8 row, u8 bank, u16 addr);
extern void M74_RamTileFillRom(u16 src, u8 dst);
extern void M74_RamTileFillRam(u8  src, u8 dst);
extern void M74_RamTileClear(u8 dst);
extern void M74_Halt(void) __attribute__((noreturn));
extern void M74_Seq(void);
#if (M74_SPR_ENABLE != 0)
extern void M74_VramRestore(void);
extern void M74_BlitSprite(u16 spo, u8 xl, u8 yl, u8 flg);
extern void M74_BlitSpriteCol(u16 spo, u8 xl, u8 yl, u8 flg, u8 col);
extern void M74_PutPixel(u8 col, u8 xl, u8 yl, u8 flg);
#endif


/*
** VRAM row for Mode 0
*/
typedef struct{
 u8  config;           /* Row configuration */
 u16 bg_addr;          /* Background address in SPI RAM (config bit 7 is high bit) */
 u8  t0_addr_h;        /* Tiles 0 - 127 address high */
 u8  t1_addr_h;        /* Tiles 128 - 255 address high */
 u8  data[25];         /* RAM tile data */
}m74_mode0_vram_t;


/*
** VRAM row for Mode 2
*/
typedef struct{
 u8  config;           /* Row configuration */
 u16 pal_addr;         /* Palette address (config bit 7 is high bit for SPI RAM) */
 u8  col;              /* Line color */
}m74_mode2_vram_t;


/*
** VRAM row for Mode 4
*/
typedef struct{
 u8  config;           /* Row configuration */
 u8  data[96];         /* Left column pixel data (8 * 24 pixels) */
}m74_mode4_vram_t;


/*
** VRAM row for Mode 5
*/
typedef struct{
 u8  config;           /* Row configuration */
 u16 img_addr;         /* Image row address in SPI RAM (config bit 7 is high bit) */
}m74_mode5_vram_t;


/*
** VRAM row for Mode 6
*/
typedef struct{
 u8  config;           /* Row configuration */
 u16 img_addr;         /* Image row address in SPI RAM (config bit 7 is high bit) */
 u8  data[96];         /* Attribute data for the row */
}m74_mode6_vram_t;


/*
** VRAM row for Mode 7
*/
typedef struct{
 u8  config;           /* Row configuration */
 u16 img_addr;         /* Image row address in SPI RAM (config bit 7 is high bit) */
 u8  fgcol;            /* Foreground color */
 u8  bgcol;            /* Background color */
}m74_mode7_vram_t;


/*
** Sprite blitter flags, for use with M74_BlitSprite.
** Sprite importance is ignored in this mode, it is left only for
** compatibility with Mode 74.
*/
#define M74_SPR_FLIPX            0x01U
#define M74_SPR_FLIPY            0x04U
#define M74_SPR_SPIRAM_A16       0x02U
#define M74_SPR_MASK             0x10U
#define M74_SPR_I3               0xC0U
#define M74_SPR_I2               0x80U
#define M74_SPR_I1               0x40U
#define M74_SPR_I0               0x00U


/*
** Configuration (m74_config) flags. The palette configuration is common with
** Row Mode 2's configuration (M74_CFG_PAL_SRC_NONE is interpreted as RAM for
** m74_config). The SPI RAM address bit 16 is common in all Row Mode
** configurations.
*/
#define M74_CFG_ENABLE           0x01U
#define M74_CFG_RAM_VADDR        0x02U
#define M74_CFG_COL0_RELOAD      0x10U

#define M74_CFG_PAL_SRC_MASK     0x60U
#define M74_CFG_PAL_SRC_NONE     0x00U
#define M74_CFG_PAL_SRC_RAM      0x20U
#define M74_CFG_PAL_SRC_ROM      0x40U
#define M74_CFG_PAL_SRC_SPIRAM   0x60U

#define M74_CFG_SPIRAM_A16       0x80U


/*
** Row mode 2 and Row mode 7: Use color 0 for background instead of taking it
** from VRAM. This can be used mostly to allow Color 0 replaces to take over.
*/
#define M74_CFG_USE_COL0         0x10U

