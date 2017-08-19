/*
 *  Uzebox Kernel - Mode 3
 *  Copyright (C) 2008 Alec Bourque
 *                2017 Sandor Zsuga (Jubatian)
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

	#include <stdbool.h>
	#include <avr/io.h>
	#include <stdlib.h>
	#include <avr/pgmspace.h>
	#include <avr/interrupt.h>
	#include "uzebox.h"
	#include "intro.h"
	

	#if INTRO_LOGO !=0
		#include "videoMode3/uzeboxlogo_8x8.pic.inc"
		#include "videoMode3/uzeboxlogo_8x8.map.inc"
	#endif

	extern unsigned char overlay_vram[];
	extern unsigned char ram_tiles[];
#if (SPRITES_AUTO_PROCESS != 0)
	extern struct SpriteStruct sprites[];
#endif
	extern unsigned char *sprites_tiletable_lo;
	extern unsigned int sprites_tile_banks[];
	extern unsigned char *tile_table_lo;
#if (RTLIST_ENABLE != 0)
	extern struct BgRestoreStruct ram_tiles_restore[];
#endif

extern u8 free_tile_index;
extern u8 user_ram_tiles_c;
extern u8 user_ram_tiles_c_tmp;
extern void RestoreBackground(void);
extern void BlitSpritePart(u8 ramtileno, u16 flidx, u16 xy, u16 dxdy);

static bool sprites_on = true;

void SetUserRamTilesCount(u8 count){
	user_ram_tiles_c_tmp = count;
}

u8* GetUserRamTile(u8 index){
	return ram_tiles + (index * TILE_HEIGHT * TILE_WIDTH);
}

void SetSpriteVisibility(bool visible){
	sprites_on = visible;
}



#if (SPRITES_AUTO_PROCESS != 0)
static
#endif
void BlitSprite(u8 flags, u8 sprindex, u8 xpos, u8 ypos)
{
	u8  bx;
	u8  by;
	u8  dx;
	u8  dy;
	u8  bt;
	u8  x;
	u8  y;
	u8  tx;
	u8  ty;
	u8  wx;
	u8  wy;
	u16 ramPtr;
	u8  ssx;
	#if (SCROLLING != 0)
	u16 ssy;
	#else
	u8  ssy;
	#endif

	/* get tile's screen section offsets */

	#if (SCROLLING != 0)
	ssx = xpos + Screen.scrollX;
	ssy = ypos + Screen.scrollY;
	#else
	ssx = xpos;
	ssy = ypos;
	#endif

	tx = 1U;
	ty = 1U;

	/* get the BG tiles that are overlapped by the sprite,
	** supporting wrapping (so sprites located just below zero X
	** or Y would clip on the left). In a scrolling config. only
	** TILE_WIDTH = 8 is really supported due to the "weird" VRAM
	** layout, VRAM_TILES_H is also fixed 32 this case. */

	#if (SCROLLING == 0)
	bx = (((ssx + TILE_WIDTH) & 0xFFU) / TILE_WIDTH) - 1U;
	#else
	bx = ssx / TILE_WIDTH;
	#endif
	dx = ssx % TILE_WIDTH;
	if (dx != 0U){ tx++; }

	#if (SCROLLING == 0)
	by = (((ssy + TILE_HEIGHT) & 0xFFU) / TILE_HEIGHT) - 1U;
	#else
	by = ssy / TILE_HEIGHT;
	#endif
	dy = ssy % TILE_HEIGHT;
	if (dy != 0U){ ty++; }

	/* Output sprite tiles */

	for (y = 0U; y < ty; y++){

		wy = by + y;
		#if (SCROLLING != 0)
		if (Screen.scrollHeight == 0U){
			wy = 0U;
		}else{
			while (wy >= Screen.scrollHeight){
				wy -= Screen.scrollHeight;
			}
		}
		#endif

		for (x = 0U; x < tx; x++){

			wx = bx + x;

			#if (SCROLLING == 0)
			if ( (wx < VRAM_TILES_H) &&
			     (wy < VRAM_TILES_V) ){
			#else
			wx = wx % VRAM_TILES_H;
			{
			#endif

				#if (SCROLLING == 0)
				ramPtr = (wy * VRAM_TILES_H) +
				         wx;
				#else
				ramPtr = ((u16)(wy >> 3) * 256U) +
				         (wx * 8U) + (wy & 0x07U);
				#endif

				bt = vram[ramPtr];

				if ( ( (bt >= RAM_TILES_COUNT) |
				       (bt < user_ram_tiles_c)) &&
				     (free_tile_index < RAM_TILES_COUNT) ){ /* if no ram free ignore tile */

					if (bt >= RAM_TILES_COUNT){
						/* tile is mapped to flash. Copy it to next free RAM tile. */
						CopyFlashTile(bt - RAM_TILES_COUNT, free_tile_index);
					}else if (bt < user_ram_tiles_c){
						/* tile is a user ram tile. Copy it to next free RAM tile. */
						CopyRamTile(bt, free_tile_index);
					}
					#if (RTLIST_ENABLE != 0)
					ram_tiles_restore[free_tile_index].addr = (&vram[ramPtr]);
					ram_tiles_restore[free_tile_index].tileIndex = bt;
					#endif
					vram[ramPtr] = free_tile_index;
					bt = free_tile_index;
					free_tile_index++;

				}

				if ( (bt < RAM_TILES_COUNT) &&
				     (bt >= user_ram_tiles_c) ){
					BlitSpritePart(bt,
					               ((u16)(flags) << 8) + sprindex,
					               ((u16)(y)     << 8) + x,
					               ((u16)(dy)    << 8) + dx);
				}

			}

		} /* end for X */

	} /* end for Y */

}



#if (SPRITES_AUTO_PROCESS != 0)
void ProcessSprites(){

	u8 i;

	if (!sprites_on){ return; }

	user_ram_tiles_c = user_ram_tiles_c_tmp;
	free_tile_index = user_ram_tiles_c;

	for (i = 0U; i < MAX_SPRITES; i++){

		BlitSprite(sprites[i].flags,
		           sprites[i].tileIndex,
		           sprites[i].x,
		           sprites[i].y);

	}

	/* restore BG tiles */

	#if (SPRITES_VSYNC_PROCESS != 0)
	RestoreBackground();
	#endif

}
#endif



	#if SCROLLING == 1
		//Scroll the screen by the relative amount specified (+/-)
		//This function handles screen wrapping on the Y axis if VRAM_TILES_V is less than 32
		void Scroll(char dx,char dy){
		Screen.scrollY+=dy;
		Screen.scrollX+=dx;

		if(Screen.scrollHeight<32){

			if(Screen.scrollY>=(Screen.scrollHeight*TILE_HEIGHT)){
				if(dy>=0){	
					Screen.scrollY=(Screen.scrollY-(Screen.scrollHeight*TILE_HEIGHT));
				}else{
					Screen.scrollY=((Screen.scrollHeight*TILE_HEIGHT)-1)-(0xff-Screen.scrollY);
				}			
			}
	
		}

		}

		//position the scrolling is absolute value
		void SetScrolling(char sx,char sy){

			Screen.scrollX=sx;

			if(Screen.scrollHeight<32){
				if(sy<(Screen.scrollHeight*TILE_HEIGHT)){
					Screen.scrollY=sy;
				}
			}else{
				Screen.scrollY=sy;
			}
		}
	#endif



#if (SPRITES_AUTO_PROCESS != 0)

	void MapSprite(unsigned char startSprite,const char *map){
		unsigned char tile;
		unsigned char mapWidth=pgm_read_byte(&(map[0]));
		unsigned char mapHeight=pgm_read_byte(&(map[1]));

		for(unsigned char dy=0;dy<mapHeight;dy++){
			for(unsigned char dx=0;dx<mapWidth;dx++){
		
			 	tile=pgm_read_byte(&(map[(dy*mapWidth)+dx+2]));		
				sprites[startSprite++].tileIndex=tile ;
			}
		}

	}


	void MapSprite2(unsigned char startSprite,const char *map,u8 spriteFlags){      
    
		unsigned char mapWidth=pgm_read_byte(&(map[0]));
		unsigned char mapHeight=pgm_read_byte(&(map[1]));
		s8 x,y,dx,dy,t; 

		if(spriteFlags & SPRITE_FLIP_X){
			x=(mapWidth-1);
			dx=-1;
		}else{
			x=0;
			dx=1;
		}

		if(spriteFlags & SPRITE_FLIP_Y){
			y=(mapHeight-1);
			dy=-1;
		}else{
			y=0;
			dy=1;
		}

		for(u8 cy=0;cy<mapHeight;cy++){
			for(u8 cx=0;cx<mapWidth;cx++){
				t=pgm_read_byte(&(map[(y*mapWidth)+x+2])); 
				sprites[startSprite].tileIndex=t;
				sprites[startSprite++].flags=spriteFlags;
				x+=dx;
			}
			y+=dy;
			x=(spriteFlags & SPRITE_FLIP_X)?(mapWidth-1):0;
	    }
	}


	void MoveSprite(unsigned char startSprite,unsigned char x,unsigned char y,unsigned char width,unsigned char height){

		for(unsigned char dy=0;dy<height;dy++){
			for(unsigned char dx=0;dx<width;dx++){
			
				sprites[startSprite].x=x+(TILE_WIDTH*dx);
			
				#if SCROLLING == 1
					if((Screen.scrollHeight<32) && (y+(TILE_HEIGHT*dy))>(Screen.scrollHeight*TILE_HEIGHT)){
						unsigned char tmp=(y+(TILE_HEIGHT*dy))-(Screen.scrollHeight*TILE_HEIGHT);
						sprites[startSprite].y=tmp;
					}else{
						sprites[startSprite].y=y+(TILE_HEIGHT*dy);
					}
				#else
					if((VRAM_TILES_V<32) && (y+(TILE_HEIGHT*dy))>(VRAM_TILES_V*TILE_HEIGHT)){
						unsigned char tmp=(y+(TILE_HEIGHT*dy))-(VRAM_TILES_V*TILE_HEIGHT);
						sprites[startSprite].y=tmp;
					}else{
						sprites[startSprite].y=y+(TILE_HEIGHT*dy);
					}
				#endif

				startSprite++;
			}
		}	

	}

#endif



	//Callback invoked by UzeboxCore.Initialize()
	void DisplayLogo(){
	
		#if INTRO_LOGO !=0
			#define LOGO_X_POS 12
			
			InitMusicPlayer(logoInitPatches);
			SetTileTable(logo_tileset);
			
			//draw logo
			ClearVram();
			WaitVsync(15);		


			#if INTRO_LOGO == 1 
				TriggerFx(0,0xff,true);
			#endif

			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo);
			WaitVsync(3);
			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo2);
			WaitVsync(2);
			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo);

			#if INTRO_LOGO == 2
				SetMasterVolume(0xc0);
				TriggerNote(3,0,16,0xff);
			#endif 
		
			WaitVsync(65);
			ClearVram();
			WaitVsync(20);
		#endif	
	}


	//Callback invoked by UzeboxCore.Initialize()
	void InitializeVideoMode(){

		//disable sprites
		#if (SPRITES_AUTO_PROCESS != 0)
		for(int i=0;i<MAX_SPRITES;i++){
			sprites[i].x=(SCREEN_TILES_H*TILE_WIDTH);		
		}
		#endif
		
		#if SCROLLING == 1
		//	for(int i=0;i<(OVERLAY_LINES*VRAM_TILES_H);i++){
		//		overlay_vram[i]=RAM_TILES_COUNT;
		//	}
			Screen.scrollHeight=VRAM_TILES_V;
			Screen.overlayHeight=0;
		#endif

		free_tile_index      = 0U;
		user_ram_tiles_c_tmp = 0U;

		//set defaults for main screen section
		/*
		for(i=0;i<SCREEN_SECTIONS_COUNT;i++){
			screenSections[i].scrollX=0;
			screenSections[i].scrollY=0;
		
			if(i==0){
				screenSections[i].height=SCREEN_TILES_V*TILE_HEIGHT;
			}else{
				screenSections[i].height=0;
			}
			screenSections[i].vramBaseAdress=vram;
			screenSections[i].wrapLine=0;
			screenSections[i].flags=SCT_PRIORITY_SPR;
		}
		*/

	}


/*
** Callback invoked during hsync
*/
void VideoModeVsync(){

	ProcessFading();
	#if (SPRITES_VSYNC_PROCESS != 0)
	ProcessSprites();
	#endif

}
