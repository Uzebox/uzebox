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
0,PC_WAVE,2,
0,PATCH_END
};

//unused
const char patch01[] PROGMEM ={	
//0,PC_WAVE,8,
0,PATCH_END
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


//rebound vaus
const char patch06[] PROGMEM ={	
0,PC_WAVE,0,
0,PC_PITCH,72,
0,PC_ENV_VOL,127*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,118*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,108*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,98*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,86*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,80*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,73*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,65*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,56*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,46*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,32*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,16*2,
2,PC_ENV_VOL,0,
2,PC_NOTE_CUT,0,
0,PATCH_END 
};

//rebound brick
const char patch07[] PROGMEM ={	
0,PC_WAVE,0,
0,PC_PITCH,79,
0,PC_ENV_VOL,127*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,118*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,108*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,98*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,86*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,80*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,73*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,65*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,56*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,46*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,32*2,
2,PC_ENV_VOL,0,
2,PC_ENV_VOL,16*2,
2,PC_ENV_VOL,0,
2,PC_NOTE_CUT,0,
0,PATCH_END 
};

//rebound hard bricks
const char patch08[] PROGMEM ={	
0,PC_WAVE,0,
0,PC_PITCH,84,
//0,PC_TREMOLO_LEVEL,80,
//0,PC_TREMOLO_RATE,10,
0,PC_ENV_VOL,127*2,
2,PC_ENV_VOL,127,
2,PC_ENV_VOL,118*2,
2,PC_ENV_VOL,118,
2,PC_ENV_VOL,108*2,
2,PC_ENV_VOL,109,
2,PC_ENV_VOL,98*2,
2,PC_ENV_VOL,98,
2,PC_ENV_VOL,86*2,
2,PC_ENV_VOL,86,
2,PC_ENV_VOL,80*2,
2,PC_ENV_VOL,80,
2,PC_ENV_VOL,73*2,
2,PC_ENV_VOL,73,
2,PC_ENV_VOL,65*2,
2,PC_ENV_VOL,65,
2,PC_ENV_VOL,56*2,
2,PC_ENV_VOL,56,
2,PC_ENV_VOL,46*2,
2,PC_ENV_VOL,46,
2,PC_ENV_VOL,32*2,
2,PC_ENV_VOL,32,
2,PC_ENV_VOL,16*2,
2,PC_ENV_VOL,16,
2,PC_NOTE_CUT,0,
0,PATCH_END 
};

//vaus explodes 1/2
const char patch09[] PROGMEM ={	
0,PC_ENV_VOL,127,
0,PC_WAVE,2, 
0,PC_ENV_SPEED,-1, 


0,PC_PITCH,67,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,

2,PC_ENV_VOL,100,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,

2,PC_ENV_VOL,70,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,

2,PC_ENV_VOL,40,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,

2,PC_ENV_VOL,20,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
 
1,PC_NOTE_CUT,0,
0,PATCH_END 
};

//vaus explodes 2/2
const char patch10[] PROGMEM ={	


0,PC_ENV_VOL,127,
0,PC_WAVE,3,
0,PC_ENV_SPEED,-1, 

0,PC_PITCH,65,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,


3,PC_ENV_VOL,100,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,


3,PC_ENV_VOL,80,
0,PC_PITCH,55,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,


3,PC_ENV_VOL,50,
0,PC_PITCH,50,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,


3,PC_ENV_VOL,40,
0,PC_PITCH,45,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,

3,PC_ENV_VOL,20,
0,PC_PITCH,40,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,


1,PC_NOTE_CUT,0,
0,PATCH_END 
};


//laser shot
const char patch11[] PROGMEM ={	
0,PC_WAVE,3,
0,PC_ENV_VOL,127,
0,PC_PITCH,80,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_ENV_VOL,0,

3,PC_ENV_VOL,127,
0,PC_PITCH,80,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_ENV_VOL,0,
/*
3,PC_PITCH,100,
0,PC_ENV_VOL,65,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_ENV_VOL,0,
*/
3,PC_PITCH,80,
0,PC_ENV_VOL,32,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_ENV_VOL,0,

3,PC_PITCH,80,
0,PC_ENV_VOL,15,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_ENV_VOL,0,


1,PC_NOTE_CUT,0,
0,PATCH_END 
};

//grow 1/2
const char patch12[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_VOL,127,
0,PC_PITCH,30,
2,PC_NOTE_UP,1,
2,PC_NOTE_UP,1,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,1,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,1,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,1,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,2,

1,PC_NOTE_CUT,0,
0,PATCH_END
};

//grow 2/2
const char patch13[] PROGMEM ={	
0,PC_WAVE,0,
0,PC_ENV_VOL,90,
0,PC_PITCH,54,
0,PC_ENV_SPEED,-3, 

2,PC_NOTE_UP,2,
2,PC_NOTE_UP,1,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,1,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,1,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,1,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,1,
2,PC_NOTE_UP,2,
2,PC_NOTE_UP,1,


1,PC_NOTE_CUT,0,
0,PATCH_END
};

//shrink
const char patch14[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_VOL,127,
0,PC_PITCH,50,
2,PC_NOTE_DOWN,1,
2,PC_NOTE_DOWN,1,
2,PC_NOTE_DOWN,2,
2,PC_NOTE_DOWN,1,
2,PC_NOTE_DOWN,2,
2,PC_NOTE_DOWN,1,
2,PC_NOTE_DOWN,2,
2,PC_NOTE_DOWN,1,
2,PC_NOTE_DOWN,2,
2,PC_NOTE_DOWN,2,
2,PC_NOTE_DOWN,2,
2,PC_NOTE_DOWN,2,
1,PC_NOTE_CUT,0,
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
{0,NULL,patch09,0,0},
{0,NULL,patch10,0,0},
{0,NULL,patch11,0,0},
{0,NULL,patch12,0,0},
{0,NULL,patch13,0,0},
{0,NULL,patch14,0,0}
};
