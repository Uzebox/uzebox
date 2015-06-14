/*
 *  Uzebox(tm) Super Mario Demo
 *  Copyright (C) 2009  Alec Bourque
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

/*

About this program:
-------------------

Demo for paletted video mode 13

*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>


//external data
#include "data/smb.map.inc"
#include "data/smb.pic.inc"
#include "data/fonts_8x8.pic.inc"

#include "data/mario_sprites.map.inc"
#include "data/mario_sprites.pic.inc"

//#include "data/nsmb.inc"
#include "data/smw2.inc"
#include "data/patches.inc"

int main(){	
	ClearVram();

	SetSpritesTileTable(mario_sprites_tileset);
	SetFontTilesIndex(SMB_TILESET_SIZE);
	SetTileTable(smb_tileset);

#if SCROLLING == 1
    Screen.scrollHeight = 23;	
    Screen.overlayHeight=4;
    Screen.overlayTileTable=smb_tileset;
	DrawMap2(0,Screen.scrollHeight,map_hud);
#endif
	
	unsigned char c;
	for(int y=0;y<22;y++){
		for(int x=0;x<32;x++){
			c=pgm_read_byte(&(map_main[(y*MAP_MAIN_WIDTH)+x+2]));
			SetTile(x,y+1,c);
		}	
	}

	while(1){
		WaitVsync(1);
#if SCROLLING == 1
		Screen.scrollX ++;
#endif
	}		
	
}

