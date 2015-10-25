/*
 *  Uzebox Kernel - Mode 13
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
	#include <avr/interrupt.h>
	#include "uzebox.h"
	#include "intro.h"
	



	#if EXTENDED_PALETTE
		#include "videoMode13/paletteTable.h"
	#endif

	#if INTRO_LOGO !=0
		#include "videoMode13/uzeboxlogo_8x8.pic.inc"
		#include "videoMode13/uzeboxlogo_8x8.map.inc"
	#endif

	extern unsigned char overlay_vram[];
	extern unsigned char ram_tiles[];
	extern struct SpriteStruct sprites[];
	extern unsigned char *sprites_tiletable_lo;
	extern unsigned int sprites_tile_banks[];
	extern unsigned char *tile_table_lo;
	extern struct BgRestoreStruct ram_tiles_restore[];
	extern void SetPaletteColorAsm(u8 index, u8 color);

	extern void CopyTileToRam(unsigned char romTile,unsigned char ramTile);
	extern void BlitSprite(unsigned char spriteNo,unsigned char ramTileNo,unsigned int xy,unsigned int dxdy);

	unsigned char free_tile_index;
	bool spritesOn=true;



	void RestoreBackground(){
		unsigned char i;
		for(i=0;i<free_tile_index;i++){			
			//vram[ram_tiles_restore[i].addr]=ram_tiles_restore[i].tileIndex;
			*ram_tiles_restore[i].addr=ram_tiles_restore[i].tileIndex;
		}	
	}


	void SetSpriteVisibility(bool visible){
		spritesOn=visible;
	}

	u16 src;u8* dest;u8 spix,dpix;
	void BlitSprite3bpp(u8 sprNo,u8 ramTileIndex,u16 yx,u16 dydx){
		u8 flags=sprites[sprNo].flags;		
		u8 x2,y2;

		src=(sprites[sprNo].tileIndex*TILE_HEIGHT*TILE_WIDTH/2)
				+sprites_tile_banks[flags>>6];	//add bank adress		

		dest=&ram_tiles[ramTileIndex*TILE_HEIGHT*TILE_WIDTH/2];
		
		for(y2=0;y2<TILE_HEIGHT;y2++){
			for(x2=0;x2<(TILE_WIDTH/2);x2++){
							
				spix=pgm_read_byte(src); //2pix flash
				dpix=*dest;
				if(spix&0x0f)dpix&=0xf0;
				if(spix&0xf0)dpix&=0x0f;
				dpix|=spix;
				*dest=dpix;
								
				dest++;
				src++;
			}		
		}

	}

	void BlitSpriteExtended(u8 sprNo,u8 ramTileIndex,u16 tytx,u16 dydx){
		
		u8 flags=sprites[sprNo].flags;
		u8 dy = dydx >> 8;
		u8 dx = dydx & 0xff;
		u8 ty = tytx >> 8;
		u8 tx = tytx & 0xff;
		u8 x1, y1;
		u8 x2, y2;
		s8 xOffset, yOffset;
		u8 x, y;
		u8* src =(sprites[sprNo].tileIndex*(TILE_HEIGHT*TILE_WIDTH/2))
				+sprites_tile_banks[flags>>6];	//add bank adress		
		u8* dst = &ram_tiles[ramTileIndex*(TILE_HEIGHT*TILE_WIDTH/2)];
		
		if(tx == 0)
		{
			x1 = 0;
			x2 = TILE_WIDTH - dx;
			xOffset = dx;
		}
		else
		{
			x1 = TILE_WIDTH - dx;
			x2 = TILE_WIDTH;
			xOffset = -x1;
		}
		if(ty == 0)
		{
			y1 = 0;
			y2 = TILE_WIDTH - dy;
			yOffset = dy;
		}
		else
		{
			y1 = TILE_WIDTH - dy;
			y2 = TILE_WIDTH;
			yOffset = -y1;
		}

		for(y = y1; y < y2; y++)
		{
			u8 srcY = y;
			u8 dstY = y + yOffset;
			u8 dstX = x1 + xOffset;
			u8* srcPtr = src + ((srcY * TILE_WIDTH + x1) >> 1);
			u8 srcPair = pgm_read_byte(srcPtr);
			u8* dstPtr = dst + ((dstY * TILE_WIDTH + dstX) >> 1);
			#if EXTENDED_PALETTE == 1
			u8 dstPair = pgm_read_byte(&PaletteExtendedToStandardTable[*dstPtr]);
			#else
			u8 dstPair = *dstPtr;
			#endif
			
			for(x = x1; x < x2; x++)
			{
				u8 srcX = x;
				u8 value;
				
				if(srcX & 0x1)
				{
					value = srcPair >> 4;
					srcPtr++;
					srcPair = pgm_read_byte(srcPtr);
				}
				else
				{
					value = srcPair & 0xF;
				}
				
				if(dstX & 0x1)
				{
					if(value != TRANSPARENT_COLOR)
					{
						dstPair = (dstPair & 0xF) | (value << 4);
					}
					#if EXTENDED_PALETTE == 1
					*dstPtr = pgm_read_byte(&PaletteStandardToExtendedTable[dstPair]);
					dstPtr++;
					dstPair = pgm_read_byte(&PaletteExtendedToStandardTable[*dstPtr]);
					#else
					*dstPtr = dstPair;
					dstPtr++;
					dstPair = *dstPtr;
					#endif
				}
				else
				{
					if(value != TRANSPARENT_COLOR)
					{
						dstPair = (dstPair & 0xF0) | (value);
					}
				}
				
				dstX ++;
			}
			
			// Ending on a half pair so need to write back:
			if(dstX & 0x1)
			{
				#if EXTENDED_PALETTE == 1
				*dstPtr = pgm_read_byte(&PaletteStandardToExtendedTable[dstPair]);
				#else
				*dstPtr = dstPair;
				#endif
			}
		}
	}
	
	/*
	//
	// This C function is the direct equivalent of the assembly
	// function of the same same.
	//
	void BlitSprite(u8 sprNo,u8 ramTileIndex,u16 yx,u16 dydx){
		u8 dy=dydx>>8;
		u8 dx=dydx &0xff;
		u8 flags=sprites[sprNo].flags;
		u8 destXdiff,ydiff,px,x2,y2;
		s8 step=1,srcXdiff;

		u16 src=(sprites[sprNo].tileIndex*TILE_HEIGHT*TILE_WIDTH)
				+sprites_tile_banks[flags>>6];	//add bank adress		

		u8* dest=&ram_tiles[ramTileIndex*TILE_HEIGHT*TILE_WIDTH];
	
		if((yx&1)==0){
			dest+=dx;	
			destXdiff=dx;
			srcXdiff=dx;
					
			if(flags&SPRITE_FLIP_X){
				src+=(TILE_WIDTH-1);
				srcXdiff=((TILE_WIDTH*2)-dx);
			}
		}else{
			destXdiff=(TILE_WIDTH-dx);

			if(flags&SPRITE_FLIP_X){
				srcXdiff=TILE_WIDTH+dx;
				src+=dx;
				src--;
			}else{
				srcXdiff=destXdiff;
				src+=destXdiff;
			}
		}
	

		if((yx&0x0100)==0){
			dest+=(dy*TILE_WIDTH);
			ydiff=dy;
			if(flags&SPRITE_FLIP_Y){
				src+=(TILE_WIDTH*(TILE_HEIGHT-1));
			}
		}else{			
			ydiff=(TILE_HEIGHT-dy);
			if(flags&SPRITE_FLIP_Y){
				src+=((dy-1)*TILE_WIDTH); 
			}else{
				src+=(ydiff*TILE_WIDTH);
			}
		}

		if(flags&SPRITE_FLIP_X){
			step=-1;
		}
		
		if(flags&SPRITE_FLIP_Y){
			srcXdiff-=(TILE_WIDTH*2);
		}

		for(y2=0;y2<(TILE_HEIGHT-ydiff);y2++){
			for(x2=0;x2<(TILE_WIDTH-destXdiff);x2++){
							
				px=pgm_read_byte(src);
				if(px!=TRANSLUCENT_COLOR){
					*dest=px;
				}
				dest++;
				src+=step;
			}		
			src+=srcXdiff;
			dest+=destXdiff;
		}

	}
	*/


	unsigned char i,bx,by,dx,dy,bt,x,y,tx=1,ty=1,wx,wy;
	unsigned int ramPtr,ssx,ssy;

	void ProcessSprites(){
	

		free_tile_index=0;	
		if(!spritesOn) return;
	
		for(i=0;i<MAX_SPRITES;i++){
			bx=sprites[i].x;

			if(bx!=(SCREEN_TILES_H*TILE_WIDTH)){
				//get tile's screen section offsets
				
				#if SCROLLING == 1
					ssx=sprites[i].x+Screen.scrollX;
					ssy=sprites[i].y+Screen.scrollY;
   				#else
					ssx=sprites[i].x;
					ssy=sprites[i].y;
				#endif

				tx=1;
				ty=1;

				//get the BG tiles that are overlapped by the sprite
				bx=ssx>>3;
				dx=ssx&0x7;
				if(dx>0) tx++;

				//by=ssy>>3;
				//dy=ssy&0x7;
				by=ssy/TILE_HEIGHT;
				dy=ssy%TILE_HEIGHT;
				if(dy>0) ty++;			

				for(y=0;y<ty;y++){

					for(x=0;x<tx;x++){
						wy=by+y;
						wx=bx+x;

						//if( (wx-(Screen.scrollX/8))>0 ) {

							//process X-Y wrapping
                            #if SCROLLING == 0
							    if(wy>=(VRAM_TILES_V*2)){
								    wy-=(VRAM_TILES_V*2);
							    }else if(wy>=VRAM_TILES_V){
							    	wy-=VRAM_TILES_V;
							    }
                            #else
                                if(wy>=(Screen.scrollHeight*2)){
								    wy-=(Screen.scrollHeight*2);
							    }else if(wy>=Screen.scrollHeight){
							    	wy-=Screen.scrollHeight;
							    }
                            #endif

							if(wx>=VRAM_TILES_H)wx-=VRAM_TILES_H; //should always be 32

							#if SCROLLING == 0
								ramPtr=(wy*VRAM_TILES_H)+wx;
							#else
								ramPtr=((wy>>3)*256)+(wx*8)+(wy&7);	
							#endif

							bt=vram[ramPtr];						

							if( (bt>=128)  && (free_tile_index < RAM_TILES_COUNT) ){

								//tile is mapped to flash. Copy it to next free RAM tile.
								//if no ram free ignore tile
								ram_tiles_restore[free_tile_index].addr=&(vram[ramPtr]);
								ram_tiles_restore[free_tile_index].tileIndex=bt;
													
								CopyTileToRam(bt-128,free_tile_index);

								vram[ramPtr]=free_tile_index;
								bt=free_tile_index;
								free_tile_index++;										
							}
				
							if(bt<RAM_TILES_COUNT){				
								#if EXTENDED_PALETTE == 1
									BlitSpriteExtended(i,bt,(y<<8)+x,(dy<<8)+dx);						
								#else
									BlitSprite3bpp(i,bt,(y<<8)+x,(dy<<8)+dx);						
								#endif
							}

					//	}

					}//end for X
				}//end for Y
	
			}//	if(bx<(SCREEN_TILES_H*TILE_WIDTH))		
		}

		//restore vram to flash tiles so the main program 
		//doesn't see ram tiles indexes.
		//ramtiles indexes will be set back during rendering prolog.
		RestoreBackground();
	}

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
		for(int i=0;i<MAX_SPRITES;i++){
			sprites[i].x=(SCREEN_TILES_H*TILE_WIDTH);		
		}
		
		#if SCROLLING == 1
		//	for(int i=0;i<(OVERLAY_LINES*VRAM_TILES_H);i++){
		//		overlay_vram[i]=RAM_TILES_COUNT;
		//	}
			Screen.scrollHeight=VRAM_TILES_V;
			Screen.overlayHeight=0;
		#endif


		//fill
		for(int i=0;i<(30*28);i++){
			vram[i]=0x80+5;
		}

	}

	//Callback invoked during hsync
	void VideoModeVsync(){
		
		ProcessFading();
		ProcessSprites();

	}

	void SetPalette(const u8* data, u8 numColors)
	{
		#if EXTENDED_PALETTE == 1
			int i;
			for(i = 0; i < MAX_PALETTE_COLORS * MAX_PALETTE_COLORS + 1; i++)
			{
				u8 index = pgm_read_byte(&PaletteEncodingTable[i]) & 0xF;
				
				if(index < numColors)
				{
					u8 color = pgm_read_byte(&data[index]);
					palette[i] = color;
				}
				else
				{
					palette[i] = 0;
				}
			}

		#else
			int i;
			for(i = 0; i < numColors; i++)
			{
				u8 color = pgm_read_byte(&data[i]);
				SetPaletteColorAsm(i,color);
			}
		#endif
	}
	
	void SetPaletteColor(u8 index, u8 color)
	{		
		#if EXTENDED_PALETTE == 1
			for(u8 i = 0; i < MAX_PALETTE_COLORS * MAX_PALETTE_COLORS + 1; i++)
			{
				if(index == (pgm_read_byte(&PaletteEncodingTable[i]) & 0xF))
				{
					palette[i] = color;
				}
			}
		#else
			SetPaletteColorAsm(index,color);
		#endif
	}

