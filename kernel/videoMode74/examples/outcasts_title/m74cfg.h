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
#define TILES_SECT 0x5800

/* Mode 74 specifics (Note: TBANKM0, TBANK2 and TBANKM3 are not filled, just
** added since required by Mode 74. The entire title screen uses
** Multicolor) */

#define M74_TBANKM0_0_OFF  0
#define M74_TBANK2_0_OFF   0
#define M74_TBANK3_0_OFF   0x0800
#define M74_TBANK3_0_INC   32

#define M74_TBANK01_0_OFF  (TILES_SECT + 0x0000)
#define M74_TBANK01_0_INC  0
#define M74_M3_ENABLE      1
#define M74_M3_OFF         (0x0F00-(9*39)-2304)


#endif
