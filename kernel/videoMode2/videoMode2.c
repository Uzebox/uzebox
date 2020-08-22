/*
 *  Uzebox Kernel - Mode 2
 *  Copyright (C) 2008  Alec Bourque
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
	#include "uzebox.h"
	#include <avr/interrupt.h>
	#include "intro.h"
	#include "data/scrolltable.inc"
	
	#if INTRO_LOGO !=0
		#include "videoMode2/uzeboxlogo_6x8.pic.inc"
		#include "videoMode2/uzeboxlogo_6x8.map.inc"
	#endif

	extern void ProcessSprites();
	extern unsigned char scanline_sprite_buf[];
	extern unsigned char rotate_spr_no;


	extern u8 sprites_per_lines[SCREEN_TILES_V*TILE_HEIGHT][MAX_SPRITES_PER_LINE];

	/**
	 *  =====USED INTERNALLY ONLY - USE SCREEN SECTIONS XSCROLL & YSCROLL===
	 *
	 * Scroll the window to the specified position within the 
	 * 32x32 tiles VRAM. Note that due to the non-binary aligment of the 
	 * tiles width(6), the caller needs to perform X wrapping
	 * before calling this function. Wrapping is at X_SCROLL_WRAP 
	 * (32 horizontal VRAM tiles * 6 horizontal pixel per tiles = 192)
	 * Y Scrolling doesnt need this because it wraps automatically at 0xff.
	 * Try to execute this call a close as possible to WaitVsync() call
	 * to minimize chances that a rendering interrupt occurs between the setting
	 * of scroll_x and scroll_y.
	 *
	 * I.e:
	 *
	 * sx++;
	 * if(sx>=X_SCROLL_WRAP) sx=0;
	 *
	 * or
	 *
	 * sx--;
	 * if(sx==0xff) sx=(X_SCROLL_WRAP-1);
	 */
	 
	 void SetScrolling(unsigned char section,unsigned char sx,unsigned char sy){
		unsigned char Xcoarse,Xfine;

		//prevent srolling overflow
		if(sx>=0 && sx<X_SCROLL_WRAP){
			Xcoarse=pgm_read_byte(&(scrolltable[sx][0]));
			Xfine=pgm_read_byte(&(scrolltable[sx][1]));
		
			screenSections[section].scrollXcoarse=Xcoarse;
			screenSections[section].scrollXfine=Xfine;
		}
		
		screenSections[section].vramWrapAdress=screenSections[section].vramBaseAdress+screenSections[section].scrollXcoarse;
		screenSections[section].vramRenderAdress=screenSections[section].vramWrapAdress+((sy/8)*VRAM_TILES_H);

	}

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

	void MoveSprite(unsigned char startSprite,unsigned char x,unsigned char y,unsigned char width,unsigned char height){
	
		for(unsigned char dy=0;dy<height;dy++){
			for(unsigned char dx=0;dx<width;dx++){
				
				sprites[startSprite].x=x+(TILE_WIDTH*dx);
				sprites[startSprite].y=y+(TILE_HEIGHT*dy);
				startSprite++;
			}
		}	

	}


	//Callback invoked by UzeboxCore.Initialize()
	void DisplayLogo(){
	
		#if INTRO_LOGO !=0
			#define LOGO_X_POS 8
			
			InitMusicPlayer(logoInitPatches);
			screenSections[0].tileTableAdress=uzeboxlogo;
					
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

		//clear sprite-per-line buffer
		for(u16 i=0;i<(SCREEN_TILES_H+2)*TILE_WIDTH;i++){
			scanline_sprite_buf[i]=TRANSLUCENT_COLOR;
		}

		//set default to no flickers
		SetSpritesOptions(0);

		//set defaults for main screen section
		for(u16 i=0;i<SCREEN_SECTIONS_COUNT;i++){
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

		//sprites flicker rotation
		rotate_spr_no=1;
		
	}

	//Callback invoked during hsync
	void VideoModeVsync(){
		
		ProcessFading();
		ProcessSprites();

	}
