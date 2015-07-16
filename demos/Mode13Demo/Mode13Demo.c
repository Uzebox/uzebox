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


#include "data/graphics.inc.h"
#include "data/sprites.inc.h"

int main(){	
//	ClearVram();

	SetSpritesTileTable(spritesTiles);
//	SetFontTilesIndex(SMB_TILESET_SIZE);
	SetTileTable(graphicsTiles);
	
	SetPalette(graphicsPalette, GRAPHICSPALETTE_SIZE);

//#if SCROLLING == 1
//    Screen.scrollHeight = 23;	
//    Screen.overlayHeight=4;
//    Screen.overlayTileTable=smb_tileset;
//	DrawMap2(0,Screen.scrollHeight,map_hud);
//#endif
	
	u8 x=(8*4);
	u8 y=(8*24);

	sprites[0].x=x;
	sprites[0].y=y;
	sprites[0].tileIndex=0;

	sprites[1].x=x+8;
	sprites[1].y=y;
	sprites[1].tileIndex=1;

	sprites[2].x=x;
	sprites[2].y=y+8;
	sprites[2].tileIndex=2;

	sprites[3].x=x+8;
	sprites[3].y=y+8;
	sprites[3].tileIndex=3;


	SetSpriteVisibility(true);

	unsigned char c;
	for(int y=0;y<28;y++){
		for(int x=0;x<VRAM_TILES_H;x++){
			c=pgm_read_byte(&graphicsMap[(y*GRAPHICSMAP_WIDTH)+x+2]);
			//SetTile(x,y+1,c+4);
			vram[(y*VRAM_TILES_H)+x]=0x80+(c+4);
		}	
	}



	//u16 i=0;
	while(1){
	//	SetPaletteColor(1,i++);
	//	WaitVsync(4);
	}		
	
}

