/*
 *  Uzebox Patches
 *  Copyright (C) 2011  Alec Bourque
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





const char bass[] PROGMEM ={	
	0,PC_WAVE,6,
	0,PC_ENV_SPEED,-50, 
	0,PATCH_END
};
const char lead[] PROGMEM ={	
	0,PC_ENV_SPEED,-20,
	0,PC_NOISE_PARAMS,2,
	1,PC_ENV_VOL,0,
	2,PC_ENV_VOL,0xff,
	0,PC_NOISE_PARAMS,3, 
	0,PATCH_END  
};




// SOUND EFFECTS

//FX: Moving around board
const char fx00[] PROGMEM ={ 
0,PC_WAVE,8,
0,PC_ENV_VOL,0xE0,
0,PC_ENV_SPEED,-20,
0,PC_PITCH,80,
2,PC_NOTE_DOWN,6,
2,PC_NOTE_DOWN,6,
2,PC_NOTE_CUT,0,
0,PATCH_END
};

//FX: Can't move to location
const char fx01[] PROGMEM ={ 
0,PC_WAVE,5,
0,PC_ENV_VOL,0xa0,
0,PC_PITCH,35,
5,PC_NOTE_CUT,0,
0,PATCH_END
};

//FX: Select square
const char fx02[] PROGMEM ={ 
0,PC_WAVE,4,
0,PC_ENV_VOL,0xD0,
0,PC_PITCH,60,
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,60, 
2,PC_NOTE_UP,6,
2,PC_NOTE_UP,6,
1,PC_NOTE_UP,6,
1,PC_NOTE_UP,6,
1,PC_NOTE_UP,6,
1,PC_NOTE_CUT,0,
0,PATCH_END
};

//FX: Error move (move not valid)
const char fx03[] PROGMEM ={ 
0,PC_WAVE,5,
0,PC_ENV_VOL,0xa0,
0,PC_PITCH,40,
5,PC_ENV_VOL,0x40,
2,PC_ENV_VOL,0xa0,
5,PC_NOTE_CUT,0,
0,PATCH_END
};

//FX: COMPUTER MOVE FINISHED
const char fx04[] PROGMEM ={ 
0,PC_ENV_SPEED,-5,
0,PC_WAVE,8,
0,PC_TREMOLO_LEVEL,0xa0,     
0,PC_TREMOLO_RATE,50, 
0,PC_PITCH,70,
0,PATCH_END
};

//FX: CAPTURE OPPONENT
const char fx05[] PROGMEM ={ 
0,PC_WAVE,4,
0,PC_ENV_VOL,0xa0,
0,PC_ENV_SPEED,-10,
0,PC_PITCH,50,
1,PC_NOTE_UP,1,
1,PC_NOTE_UP,1,
1,PC_NOTE_UP,1,
1,PC_NOTE_UP,1,
1,PC_NOTE_UP,1,
//0,PC_TREMOLO_LEVEL,0xa0,     
//0,PC_TREMOLO_RATE,50, 
5,PC_PITCH,40,
0,PC_ENV_VOL,0xff,
0,PC_WAVE,1,
0,PC_ENV_SPEED,-10,
0,PC_TREMOLO_LEVEL,0xa0,     
0,PC_TREMOLO_RATE,50, 
//5,PC_NOTE_CUT,0,
0,PATCH_END 
};

//FX: PROMOTION OF PAWN
const char fx06[] PROGMEM ={ 
0,PC_WAVE,8,
0,PC_ENV_SPEED,10,
0,PC_ENV_VOL,0x20,
0,PC_PITCH,50,
5,PC_PITCH,65,
5,PC_PITCH,70,
5,PC_PITCH,75,
7,PC_PITCH,65,
5,PC_PITCH,70,
5,PC_PITCH,75,
5,PC_PITCH,77,
0,PC_TREMOLO_LEVEL,0xa0,     
0,PC_TREMOLO_RATE,50, 
0,PC_ENV_SPEED,-10,
0,PATCH_END
};

//FX: CHECK
const char fx07[] PROGMEM ={ 

0,PC_WAVE,0,
0,PC_ENV_SPEED,-5,

0,PC_PITCH,72,
2,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 


2,PC_NOTE_DOWN,10,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 


2,PC_NOTE_DOWN,2,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 


2,PC_NOTE_DOWN,10,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 

2,PC_NOTE_DOWN,5,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 

2,PC_NOTE_DOWN,5,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 

1,PC_NOTE_CUT,0,

0,PATCH_END
};

//FX: CHECKMATE
const char fx08[] PROGMEM ={ 
0,PC_WAVE,3,
0,PC_PITCH,86,
2,PC_NOTE_DOWN,10, 
2,PC_NOTE_DOWN,8, 
2,PC_NOTE_DOWN,6, 
2,PC_NOTE_DOWN,4, 
2,PC_NOTE_DOWN,2, 
 
0,PC_PITCH,66, 
2,PC_NOTE_UP,5, 
2,PC_NOTE_DOWN,2, 
2,PC_NOTE_UP,5, 

2,PC_NOTE_UP,5, 
2,PC_NOTE_DOWN,2, 
2,PC_NOTE_UP,5, 

0,PC_ENV_SPEED,-8,  

4,PC_NOTE_UP,5, 
4,PC_NOTE_DOWN,5, 
4,PC_NOTE_UP,5, 
4,PC_NOTE_DOWN,5, 
4,PC_NOTE_UP,5,
4,PC_NOTE_DOWN,5, 
4,PC_NOTE_UP,5,
1,PC_NOTE_CUT,0,
0,PATCH_END 
};

//FX: STALEMATE
const char fx09[] PROGMEM ={ 
0,PC_WAVE,6,
0,PC_ENV_SPEED,-5,
0,PATCH_END
};




const struct PatchStruct patches[] PROGMEM = {
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{1,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{1,NULL,bass,0,0}, 
{0,NULL,bass,0,0},
{1,NULL,bass,0,0},
{1,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{1,NULL,bass,0,0},
{1,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,lead,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,bass,0,0},
{0,NULL,fx00,0,0},
{0,NULL,fx01,0,0},
{0,NULL,fx02,0,0},
{0,NULL,fx03,0,0},
{0,NULL,fx04,0,0},
{0,NULL,fx05,0,0},
{0,NULL,fx06,0,0},
{0,NULL,fx07,0,0}, 
{0,NULL,fx08,0,0},
{0,NULL,fx09,0,0}
};

