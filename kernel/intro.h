/*
 *  Uzebox Kernel intro defines
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

#pragma once
#include "defines.h"

#if INTRO_LOGO == 1 

	//Logo "kling" sound
	const char initPatch[] PROGMEM ={	
	0,PC_WAVE,INTRO_WAVETABLE,
	0,PC_PITCH,85,
	4,PC_PITCH,90,
	0,PC_ENV_SPEED,-8,   
	0,PC_TREMOLO_LEVEL,0x90,     
	0,PC_TREMOLO_RATE,30, 
	50,PC_NOTE_CUT,0,
	0,PATCH_END  
	};

	const struct PatchStruct logoInitPatches[] PROGMEM = 
	{
		{0,NULL,initPatch,0,0},
	};

#elif INTRO_LOGO == 2

	#include "data/logovoice.inc"

	const struct PatchStruct logoInitPatches[] PROGMEM = 
	{
		{0,voice,NULL,sizeof_voice,sizeof_voice},
	};
#endif
