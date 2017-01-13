/*
 *  Mode 74 tests
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


#include <avr/io.h>
#include <avr/pgmspace.h>


#ifndef TILES_H
#define TILES_H


extern const unsigned char imgvram[]        PROGMEM;
extern const unsigned char res_screen_00[]  __attribute__ ((section (".tiles")));
extern const unsigned char res_pal_00[]     __attribute__ ((section (".tiles")));
extern const unsigned char res_sprites_00[] __attribute__ ((section (".tiles")));


#endif
