/*
 *  Uzebox(tm) Mode 2 Sprite Engine Demo
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
#include <uzebox.h>


//external data
#include "data/spritedemo.map.inc"
#include "data/spritedemo.pic.inc"
#include "data/cubes.pic.inc"
#include "data/mypatches.h"





int main(){	
	
	ClearVram();
		
	//initialize stuff
	InitMusicPlayer(patches);
	SetMasterVolume(0xc0); //crank up the volume a bit since the sample is not too loud


	//start the beat loop
	TriggerNote(4,1,23,0xff);

	while(1);
}

