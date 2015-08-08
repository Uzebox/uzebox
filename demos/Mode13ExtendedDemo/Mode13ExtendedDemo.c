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
#include <stdlib.h>


#include "data/graphics.inc.h"
#include "data/sprites.inc.h"

extern u8 tile_bank;


typedef struct
{
	u8 x, y;
	s8 dx, dy;
} mario_t;

#define NUM_MARIOS (2)

mario_t marios[NUM_MARIOS];

void setup_sprite(mario_t* mario, int base)
{
	sprites[base].x=mario->x;
	sprites[base].y=mario->y;
	sprites[base].tileIndex=0;

	sprites[base+1].x=mario->x+8;
	sprites[base+1].y=mario->y;
	sprites[base+1].tileIndex=1;

	sprites[base+2].x=mario->x;
	sprites[base+2].y=mario->y+8;
	sprites[base+2].tileIndex=2;

	sprites[base+3].x=mario->x+8;
	sprites[base+3].y=mario->y+8;
	sprites[base+3].tileIndex=3;
	
	mario->x += mario->dx;
	mario->y += mario->dy;
	
	if(mario->x > 200)
	{
		mario->dx = -1;
	}
	if(mario->x == 0)
	{
		mario->dx = 1;
	}
	if(mario->y > 200)
	{
		mario->dy = -1;
	}
	if(mario->y == 0)
	{
		mario->dy = 1;
	}
	
}

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
	
	/*u8 x=1;//(8*4);
	u8 y=0;//8*24-1;

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
*/

	SetSpriteVisibility(true);
	Screen.scrollX=0;
	Screen.tileBank=0x00;
	Screen.scrollHeight=24;
	Screen.overlayHeight=4;

	unsigned char c;
	for(int y=0;y<28;y++){
		for(int x=0;x<VRAM_TILES_H;x++){
			c=pgm_read_byte(&graphicsMap[(y*GRAPHICSMAP_WIDTH)+x+2]);
			SetTile(x,y,c);
			//vram[(y*VRAM_TILES_H)+x]=0x80+(c+0);
		}	
	}

	for(int n = 0; n < NUM_MARIOS; n++)
	{
		marios[n].x = rand() % 200;
		marios[n].y = rand() % 200;
		marios[n].dx = (rand() % 100) < 50 ? -1 : 1 ;
		marios[n].dy = (rand() % 100) < 50 ? -1 : 1 ;
	}




	u16 i=0,joy;
	u8 col=0;
	//SetPaletteColor(1, 7);
	while(1){
		/*
		if(i==8){
			SetPaletteColor(1, col++);
			i=0;
		}else{		
			i++;
		}
		*/

		for(int n = 0; n < NUM_MARIOS; n++)
		{
			setup_sprite(&marios[n], n * 4);
		}
		
		joy=ReadJoypad(0);
		if(joy==BTN_A){			
			Screen.tileBank ^= 0x10;
			while(ReadJoypad(0)!=0);
		}else if(joy==BTN_LEFT){
			Screen.scrollX--;
			while(ReadJoypad(0)!=0);
		}else if(joy==BTN_RIGHT){
			Screen.scrollX++;
			while(ReadJoypad(0)!=0);
		}
		
		
		Screen.scrollX++;
		
		
		WaitVsync(1);
	}		
	
}

