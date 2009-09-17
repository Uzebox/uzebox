/*
 *  Uzebox Default Patches
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
*/

/*
	Patches are made of a command stream made of 3 bytes per command:
		1=delta time
		2=command
		3=command parameter

	Patches must start with 1 byte describing the sound type:
		0=wave channel (i.e.; channel 0,1 and 2)
		1=noise channel (channel 3)
		2=PCM (channel 3)
		->For type=2 two more byte follows in order: sample adress low byte, sample adress hi byte

	It must end with <0,PATCH_END> and this command takes
	only two bytes (no parameter).
*/


//INST: Synth Piano
const char patch00[] PROGMEM ={ 
0,PC_WAVE,8,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-20, 
1,PATCH_END
};

//unused
const char patch01[] PROGMEM ={	
0,PC_WAVE,1,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-20, 
1,PATCH_END
};


//unused
const char patch02[] PROGMEM ={	
//0,PC_WAVE,3,
0,PATCH_END
};


//unused
const char patch03[] PROGMEM ={	
//0,PC_WAVE,4,
0,PATCH_END
};

//unused
const char patch04[] PROGMEM ={	
//0,PC_WAVE,2,
0,PATCH_END
};

//Base line
const char patch05[] PROGMEM ={	
0,PC_WAVE,1,
1,PC_ENV_SPEED,-20, 
0,PATCH_END
};


//hammer
const char patch06[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_PITCH,72,
1,PC_PITCH,52,
1,PC_PITCH,62,
1,PC_NOTE_CUT,0,
0,PATCH_END 
};

//hit
const char patch07[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_ENV_SPEED,-20, 
1,PC_PITCH,60,
1,PC_PITCH,68,
1,PC_PITCH,60,
1,PC_PITCH,80,
1,PC_PITCH,70,
1,PC_NOTE_CUT,0,
0,PATCH_END 
};


//counter
const char patch08[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_ENV_SPEED,-30,
0,PC_PITCH,72,
5,PC_NOTE_CUT,0,
0,PATCH_END 
};

//counter end
const char patch09[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_ENV_SPEED,-7,
0,PC_PITCH,84,
45,PC_NOTE_CUT,0,
0,PATCH_END 
};


const struct PatchStruct patches[] PROGMEM = {
{0,NULL,patch00,0,0},
{0,NULL,patch01,0,0},
{0,NULL,patch02,0,0},
{0,NULL,patch03,0,0},
{0,NULL,patch04,0,0},
{0,NULL,patch05,0,0},
{0,NULL,patch06,0,0},
{0,NULL,patch07,0,0},
{0,NULL,patch08,0,0},
{0,NULL,patch09,0,0}
};
