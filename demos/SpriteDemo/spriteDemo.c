/*
 *  Uzebox Sprite Engine Demo
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

/*

About this program:
-------------------

This program demonstrates the new sprites engine which also features full screen scrolling.
The video memory map is arranged as a 32x32 tiles area in which you slide a 22x26 window. 
Up to 31 simultaneous sprites can be displayed on screen at once. However no more 
than 5 sprites can be visible on each scanline due to CPU limitation. Note that in this version, 
sprite #0 must point to a transparent tile. Sprites with lower indexes have higher priority. 
I.e: sprite #1 has higher priority than sprite #5. If more than five sprites crosses the 
same scanline 'sprites overflow' occurs. In this case, the sprites engine can be configured 
to support two options: 
	0=the lower priority one will be completely invisible. 
	1=cycle the sprites so that all sprites on that scanline would flicker in turn (similar to the NES). 

Sprites are accessed using a structure with X,Y and tileIndex values. The sprites engine
uses a dedicated tile table that does not need to be the same as for the background tiles. 
This means that you can have a maximum of 256 tiles used for the sprites per rendering frame.

The engine now support screen sections (aka split screens) that replaces the overlay. Screen sections
are vertical section of the screen that are parametrizable at runtime. Parameter are:

	-scrollX: x displacement
	-scrollY: y displacement
	-height: section height in scanlines
	-vramBaseAdress: location in vram where this section starts rendering
	-tileTableAdress: tile set to use when rendering this section
	-wrapLine: Define at what Y scroll within that section Y-wrap will happen (Normally Y-Wrap happens at 0xff). So
	           instead of wrapping each 32 tiles you can wrap each 12 or 12 tiles+ 3 lines, etc.
	-flags: 1=sprites will be drawn on top of this section. 0=sprites will be under.
	
	Notes: 
	-The area of memory allocated for each section *must* be 32 tiles wide.
	-Number of sections is a compile option (see defines.h)
	-Total heights of all sections must be 208 (VRAM_TILES_V*TILE_HEIGHT). Any sections after line 208 is ignored		
	-Sections must be at least 1 line high except if they are at the end *and* not visible/active. So you can
	 not have zero height sections in the middle of section > 1 in height.

Due to the non-binary aligment of the tiles on the horizaontal axis (tile width=6), the caller needs to 
perform X wrapping before calling setting the screen section's scrollX variable. Wrapping happens 
at value X_SCROLL_WRAP (see uzeboxVideoEngine.c for details). Y Scrolling doesnt 
need this because it wraps automatically at 0xff. The sprites positions
are independant of the background scrolling posisition. To quickly turn off a 
sprite, set its Y position to -1 (or out of screen if you prefer). Finally,
it worth noting that horizontal scrolling was not trivial and the current method
requires around 9K of program space due to unrolled loops. If your games do not
need horizontal scrolling (i.e.: a "River Raid" game), you can specifiy a custom compilation option
to remove the code for this functionnality.

This revision also include and option to have music channel 3 play PCM sample instead of the noise channel.
Samples can be of arbitrary lenght and have a loop. Samples are defined as patches and can have a
command/modifier stream attached to them for envelopes and other effects. The patch system had to 
be rewritten to support PCM samples, so that may imply some adjustment on existing code.
This demo uses a PCM loop as background music.


Hope you have fun with it!


Uze


See Also:
---------
kernel/uzebox.h: API functions and variables
kernel/defines.h: Global defines, constants & compilation options
data/mypatches.h: PCM sample patch

*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "kernel/uzebox.h"


//external data
#include "data/spritedemo.map.inc"
#include "data/spritedemo.pic.inc"
#include "data/cubes.pic.inc"
#include "data/mypatches.h"


unsigned char vx=60,vy=195,sx=0,sy=0,overlay=3,sx1=0,sy1=0,sx2=0,sy2=0,w=0,playing=0,anim=1,sx3=0,sx4=0;

unsigned char processControls(void);

extern struct TrackStruct tracks[4];


struct CubeStruct
{
	unsigned char x;			
	unsigned char y;			
	char xdir;			
	char ydir;
	unsigned char rotation;	
};

struct CubeStruct cubes[7];




int main(){	

	unsigned char i,c,x,y,spr;
	
	ClearVram();
		
	//initialize stuff
	InitMusicPlayer(patches);
	SetMasterVolume(0xc0); //crank up the volume a bit since the sample is not too loud

	SetSpritesTileTable(cubes_tileset);
	SetSpritesOptions(SPR_OVERFLOW_CLIP);

	//To allocate/deallocate a player's voice
	//tracks[0].allocated=false;
	//tracks[1].allocated=false;
	//tracks[2].allocated=false;


	//start the beat loop
	TriggerNote(3,1,23,0xff);

	
 	//Define the screen sections. 

	for(i=0;i<9;i++){
		screenSections[i].tileTableAdress=spritedemo_tileset;
	}

	screenSections[0].height=76;	
	screenSections[0].flags=SCT_PRIORITY_SPR;

	screenSections[1].height=2;
	screenSections[1].vramBaseAdress=vram+(32*37);
	screenSections[1].flags=SCT_PRIORITY_BG;

	screenSections[2].height=3;
	screenSections[2].vramBaseAdress=vram+(32*37);
	screenSections[2].flags=SCT_PRIORITY_BG;
	screenSections[2].scrollY=2;

	screenSections[3].height=3;
	screenSections[3].vramBaseAdress=vram+(32*37);
	screenSections[3].flags=SCT_PRIORITY_BG;
	screenSections[3].scrollY=5;

	screenSections[4].height=40;
	screenSections[4].vramBaseAdress=vram+(32*32);
	screenSections[4].flags=SCT_PRIORITY_SPR;

	screenSections[5].height=3;
	screenSections[5].vramBaseAdress=vram+(32*38);
	screenSections[5].flags=SCT_PRIORITY_BG;

	screenSections[6].height=3;
	screenSections[6].vramBaseAdress=vram+(32*38);
	screenSections[6].flags=SCT_PRIORITY_BG;
	screenSections[6].scrollY=3;

	screenSections[7].height=2;
	screenSections[7].vramBaseAdress=vram+(32*38);
	screenSections[7].flags=SCT_PRIORITY_BG;
	screenSections[7].scrollY=6;

	screenSections[8].height=76;
	screenSections[8].flags=SCT_PRIORITY_SPR;
	screenSections[8].scrollY=132;

	Fill(0,37,32,1,1);
	Fill(0,38,32,1,2);


	for(y=0;y<30;y+=4){
		for(x=0;x<30;x+=5){
			DrawMap2(x,y,map_bg);
		}
	}

	DrawMap2(0,32,map_logo);

	for(i=22;i<32;i++){
		DrawMap2(i,32,map_bar);
	}
	
	srand(18);
	c=1;
	spr=1;
	for(i=0;i<7;i++){
		//randomize positions
		cubes[i].x=((unsigned char)(rand() % 110))+2;
		cubes[i].y=((unsigned char)(rand() % 180))+2;

		cubes[i].rotation=((unsigned char)(rand() % 7));

		if((unsigned char)(rand() % 16)<10){
			cubes[i].xdir=1;
		}else{
			cubes[i].xdir=-1;
		}

		if((unsigned char)(rand() % 16)<8){
			cubes[i].ydir=1;
		}else{
			cubes[i].ydir=-1;
		}


	}

	c=0;
	char w1=0,w2=0;
	while(1){

		//syncronize gameplay on vsync (30 hz)	
		WaitVsync(1);

		w2++;
		if(w2==3){
			sx3++;
			if(sx3>X_SCROLL_WRAP) sx3=0;		
			
			screenSections[1].scrollX=sx3;	
			screenSections[7].scrollX=sx3;	
			
			w2=0;
		}

		w1++;
		if(w1==2){
			sx2++;
			if(sx2>=X_SCROLL_WRAP) sx2=0;		
			
			screenSections[2].scrollX=sx2;	
			screenSections[6].scrollX=sx2;	
			
			w1=0;
		}

		sx++;
		if(sx>=X_SCROLL_WRAP) sx=0;

		screenSections[3].scrollX=sx;	
		screenSections[4].scrollX=sx;
		screenSections[5].scrollX=sx;	

		screenSections[0].scrollY++;
		screenSections[8].scrollY++;



		spr=1;
		for(i=0;i<7;i++){
	
			cubes[i].x+=cubes[i].xdir;
			cubes[i].y+=cubes[i].ydir;
			
			if(cubes[i].x>123 && cubes[i].xdir==1){
				cubes[i].xdir=-1;
			}
			if(cubes[i].x<9 && cubes[i].xdir==-1){
				cubes[i].xdir=1;
			}

			cubes[i].y+=cubes[i].ydir;
			if(cubes[i].y>=196 && cubes[i].ydir==1){
				cubes[i].ydir=-1;
			}
			if(cubes[i].y<10 && cubes[i].ydir==-1){
				cubes[i].ydir=1;
			}
		

			if(c==0){
				cubes[i].rotation++;
				if(cubes[i].rotation==14)cubes[i].rotation=0;

				sprites[spr+0].tileIndex=(cubes[i].rotation*2)+1;
				sprites[spr+1].tileIndex=(cubes[i].rotation*2)+1+1;
				sprites[spr+2].tileIndex=(cubes[i].rotation*2)+1+29;
				sprites[spr+3].tileIndex=(cubes[i].rotation*2)+1+30;
			}

			
				sprites[spr+0].x=cubes[i].x;
				sprites[spr+0].y=cubes[i].y;

				sprites[spr+1].x=cubes[i].x+6;
				sprites[spr+1].y=cubes[i].y;

				sprites[spr+2].x=cubes[i].x;
				sprites[spr+2].y=cubes[i].y+8;

				sprites[spr+3].x=cubes[i].x+6;
				sprites[spr+3].y=cubes[i].y+8;
		
			spr+=4;
			c+=128;
		}

		processControls();

		
	
	}
}

void triggerLoop(){
	//Example to start the loop w/o the API
	mixer.channels.type.wave[3].volume=0xff;
	mixer.channels.type.wave[3].position=uzeboxbeat;
	mixer.channels.type.wave[3].step=0x80; 
	mixer.channels.type.wave[3].loopStart=uzeboxbeat;
	mixer.channels.type.wave[3].loopEnd=uzeboxbeat+sizeof_uzeboxbeat-1;
	tracks[3].patchPlaying=true;
	tracks[3].noteVol=0xff;
	tracks[3].envelopeVol=0xff;
	tracks[3].trackVol=0xff;
}


unsigned char processControls(void){
	static int lastbuttons=0;
	unsigned int joy=ReadJoypad(0);



	if((joy&BTN_START) && !(lastbuttons&BTN_START)){
	

	}


	if(joy&BTN_SR){
		//increase loop speed
		mixer.channels.type.wave[3].step+=0x1;
	}
	
	if(joy&BTN_SL){
		//decrease loop speed
		mixer.channels.type.wave[3].step-=0x1;
	}


	if(joy&BTN_A){
		TriggerNote(3,2,35,0xff);
		while(ReadJoypad(0)!=0);
	}



	
	if(joy&BTN_X){
		TriggerNote(3,1,23,0xff);
		while(ReadJoypad(0)!=0);
	}

	if(joy&BTN_Y){
		tracks[3].patchPlaying=false;
		tracks[3].noteVol=0;

	}
		
	if(joy&BTN_SELECT){
		while(ReadJoypad(0)!=0);



	}

	lastbuttons=joy;

	return 0;
}

