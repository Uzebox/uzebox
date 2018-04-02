/*
 *  Mode 748 configuration
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
*/


#ifndef M74CFG_H
#define M74CFG_H


/* Where the tile & similar aligned data is placed (not Mode 74 specific) */
#define TILES_SECT         0x4900

/* Resource locations (not Mode 74 specific) */
#define RES_SCREEN_00_OFF  (TILES_SECT + 0x0080U)
#define RES_PAL_00_OFF     (TILES_SECT + 0x00E0U)
#define RES_TILES_00_OFF   (TILES_SECT + 0x0100U)
#define RES_SPRITES_00_OFF (TILES_SECT + 0x1900U)

/* Mode 74 specifics */
#define M74_RECTB_OFF      (TILES_SECT + 0x2800)
#define M74_ROMMASKIDX_OFF (TILES_SECT + 0x2700 - ((TILES_SECT + 0x0100) / 32))
#define M74_ROMMASK_OFF    (TILES_SECT + 0x2100)
#define M74_SPR_ENABLE     1
#define M74_MSK_ENABLE     1


#endif
