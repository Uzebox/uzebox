/*
 *  Mode 74 configuration
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
*/


#ifndef M74CFG_H
#define M74CFG_H


/* Where the tile & similar aligned data is placed (not Mode 74 specific) */
#define TILES_SECT 0x4800

/* Mode 74 specifics */
#define M74_TBANKM0_0_OFF  (TILES_SECT + 0x0000)
#define M74_TBANK2_0_OFF   (TILES_SECT + 0x1000)
#define M74_TBANK3_0_OFF   0x0700
#define M74_TBANK3_0_INC   64
#define M74_VRAM_CONST     1
#define M74_VRAM_OFF       0x0400
#define M74_VRAM_W         24
#define M74_VRAM_H         27
#define M74_RTLIST_OFF     0x0300
#define M74_ROMMASK_OFF    (TILES_SECT + 0x2800)
#define M74_TBANKM0_0_MSK  (TILES_SECT + 0x2E00)
#define M74_TBANK2_0_MSK   (TILES_SECT + 0x2E80)
#define M74_TBANK3_0_MSK   0x03C0
#define M74_RECTB_OFF      (TILES_SECT + 0x2F00)


#endif