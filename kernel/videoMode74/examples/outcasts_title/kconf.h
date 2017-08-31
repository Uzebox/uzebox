/*
 *  Kernel & Mode 74 configuration
 *  Copyright (C) 2015 - 2017 Sandor Zsuga (Jubatian)
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


#ifndef KCONF_H
#define KCONF_H


/* Where the tile & similar aligned data is placed (not Mode 74 specific)
** This must match the .tiles section location in the Makefile */
#define TILES_SECT         0x5800

/* Resource locations (Not Mode 74 specific) */
#define RES_SCREEN_00_OFF  (TILES_SECT + 0x0080U)
#define RES_PAL_00_OFF     (TILES_SECT + 0x00E0U)
#define RES_FONT_OFF       (TILES_SECT + 0x0100U)

/* Kernel */
#define VIDEO_MODE         74
#define INTRO_LOGO         1
#define SOUND_MIXER        MIXER_TYPE_INLINE

/* Mode 74 specifics */
#define M74_VRAM_OFF       0x0400
#define M74_ROMTD_OFF      (TILES_SECT + 0x0000)
#define M74_M3_ENABLE      1


#endif
