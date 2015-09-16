/*
 *  Zombienator
 *  Copyright (C) 2009 Peter Hedman
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

//INST: Synth Piano
const char patch00[] PROGMEM ={ 
0,PC_WAVE,6,
0,PC_ENV_SPEED,-5,
0,PATCH_END
};

//empty 
const char patch01[] PROGMEM ={
0,PATCH_END
};

//Square Piano
const char patch02[] PROGMEM ={	
0,PC_WAVE,9,
0,PC_ENV_SPEED,-15,  
4,PC_ENV_SPEED,-7,  
0,PATCH_END
};

//INST: hi-hat
const char patch03[] PROGMEM ={	
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x90,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
};


//INST: organ
const char patch04[] PROGMEM ={	
0,PC_WAVE,8,
1,PC_ENV_VOL,200, 
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-25,
0,PATCH_END
};


//INST: Crash cymbal
const char patch05[] PROGMEM ={	
0,PC_NOISE_PARAMS,0,
0,PC_ENV_VOL,0xa0,
0,PC_ENV_SPEED,-20, 
1,PC_ENV_SPEED,-2,
0,PC_NOISE_PARAMS,1,
0,PATCH_END
};

//INST: SawTooth
const char patch06[] PROGMEM ={	
0,PC_WAVE,1,
1,PC_ENV_VOL,200,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-20, 
0,PATCH_END
};

//Square Piano 2
const char patch07[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_SPEED,-15,  
4,PC_ENV_SPEED,-7,  
0,PATCH_END
};

//SawTooth no hold
const char patch08[] PROGMEM ={	
0,PC_WAVE,1,
0,PC_ENV_SPEED,-15, 
0,PATCH_END
};

//Snare
const char patch09[] PROGMEM ={
0,PC_ENV_SPEED,-19,
0,PC_NOISE_PARAMS,6,
2,PC_NOISE_PARAMS,0x1,
0,PATCH_END 
};

//Clap
const char patch10[] PROGMEM ={
0,PC_ENV_SPEED,-20,
0,PC_NOISE_PARAMS,2,
1,PC_ENV_VOL,0,
2,PC_ENV_VOL,0xff,
0,PC_NOISE_PARAMS,3, 
0,PATCH_END  
};

//flute
const char patch11[] PROGMEM ={
0,PC_WAVE,8,
0,PC_ENV_VOL,100, 
1,PC_ENV_SPEED,20,
5,PC_ENV_SPEED,-9, 
0,PATCH_END
};

//Pizzicato
const char patch12[] PROGMEM ={	
0,PC_WAVE,9,
0,PC_ENV_SPEED,-15, 
0,PATCH_END
};

//INST: triangular - no env
const char patch13[] PROGMEM ={	
0,PC_WAVE,2,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END
};

//INST: SawTooth -  no env
const char patch14[] PROGMEM ={	
0,PC_WAVE,1,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END
};

//INST: organ -  no env
const char patch15[] PROGMEM ={	
0,PC_WAVE,8,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END};

//INST: square -  no env
const char patch16[] PROGMEM ={	
0,PC_WAVE,5,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END
};

//INST: Filtered square -  no env
const char patch17[] PROGMEM ={	
0,PC_WAVE,9,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END
};

//FX: SHOTGUN, I THINK
const char patch18[] PROGMEM ={	
0,PC_ENV_VOL,0xff, //BOOM
0,PC_NOISE_PARAMS,9, // 1, 3, 5
6,PC_ENV_SPEED,-15, 
10,PC_ENV_VOL,0xff, //CHICK
0,PC_ENV_SPEED,-19,
0,PC_NOISE_PARAMS,6,
2,PC_NOISE_PARAMS,0x1,
7,PC_ENV_VOL,0xf0, //CHICK
0,PC_ENV_SPEED,-19,
0,PC_NOISE_PARAMS,6,
2,PC_NOISE_PARAMS,0x1,
0,PATCH_END
};

//FX: ZOMBIE GOES SPLOSCH
const char patch19[] PROGMEM ={	
0,PC_ENV_VOL,0xff, //SPLO
0,PC_NOISE_PARAMS,9, 
0,PC_ENV_SPEED,-10, 
2,PC_ENV_VOL,0xff, //SCH
0,PC_ENV_SPEED,-15,
0,PC_NOISE_PARAMS,6,
2,PC_NOISE_PARAMS,0x1,
0,PATCH_END
};

//FX: DOUBLE OMG
const char patch20[] PROGMEM ={	
0,PC_ENV_VOL,0,
4,PC_WAVE,4,
0,PC_ENV_VOL,0xff,
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,40, 
0,PC_ENV_SPEED,-5,
1,PC_PITCH,55,
3,PC_NOTE_UP,10, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,10, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,10, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,10, 
3,PC_NOTE_CUT,0,
0,PATCH_END 
};

//FX: TRIPLE WTF
const char patch21[] PROGMEM ={	
0,PC_ENV_VOL,0,
4,PC_WAVE,4,
0,PC_ENV_VOL,0xff,
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,40, 
0,PC_ENV_SPEED,-5,
1,PC_PITCH,75,
4,PC_NOTE_UP,10, 
4,PC_NOTE_DOWN,5, 
4,PC_NOTE_UP,10, 
4,PC_NOTE_DOWN,10, 
4,PC_NOTE_UP,5, 
4,PC_NOTE_DOWN,10, 
4,PC_NOTE_UP,5,  
4,PC_NOTE_CUT,0,
0,PATCH_END 
};

//FX: INSANE BBQ!
const char patch22[] PROGMEM ={	
0,PC_ENV_VOL,0,
4,PC_WAVE,4,
0,PC_ENV_VOL,0xff,
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,40, 
0,PC_ENV_SPEED,-5,
1,PC_PITCH,90,
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5,  
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
6,PC_NOTE_CUT,0,
0,PATCH_END 
};

//FX: OUCH YOU ATE MY BRAINS
const char patch23[] PROGMEM ={	
0,PC_WAVE,1,
0,PC_ENV_VOL,0xff,
0,PC_ENV_SPEED,-5,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,1, 
1,PC_NOTE_DOWN,1, 
1,PC_NOTE_DOWN,1, 
1,PC_NOTE_DOWN,1,  
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1, 
1,PC_NOTE_DOWN,1, 
1,PC_NOTE_DOWN,1, 
1,PC_NOTE_DOWN,1,  
1,PC_NOTE_DOWN,1,  
3,PC_NOTE_CUT,0,
0,PATCH_END 
};

//FX: DOOM AND DEATH AND DARKNESS AND GAME OVER
const char patch24[] PROGMEM ={	
0,PC_WAVE,1,
0,PC_ENV_VOL,0xff,
0,PC_ENV_SPEED,-3,
0,PC_PITCH,60,
5,PC_NOTE_DOWN,5, 
5,PC_NOTE_DOWN,5,  
5,PC_NOTE_DOWN,5,  
10,PC_NOTE_CUT,0,
0,PATCH_END 
};

//FX: FLAMETHROWER, I THINK
const char patch25[] PROGMEM ={	
0,PC_ENV_VOL,0x99, //FROOSCH
0,PC_NOISE_PARAMS,9, // 1, 3, 5, 9
16,PC_ENV_SPEED,-12,
0,PATCH_END
};


//const char *patches[] PROGMEM = {patch00,patch01,patch02,patch03,patch04,patch05,patch06,patch07,patch08,patch09,patch10,patch11,patch12,patch13,patch14,patch15,patch16,patch17,patch18,patch19,patch20,patch21,patch22,patch23,patch24};
const struct PatchStruct patches[] PROGMEM ={
{0,NULL,patch00,0,0},
{0,NULL,patch01,0,0},
{0,NULL,patch02,0,0},
{1,NULL,patch03,0,0},
{0,NULL,patch04,0,0},
{1,NULL,patch05,0,0},
{0,NULL,patch06,0,0},
{0,NULL,patch07,0,0},
{0,NULL,patch08,0,0},
{1,NULL,patch09,0,0},
{1,NULL,patch10,0,0},
{0,NULL,patch11,0,0},
{0,NULL,patch12,0,0},
{0,NULL,patch13,0,0},
{0,NULL,patch14,0,0},
{0,NULL,patch15,0,0},
{0,NULL,patch16,0,0},
{0,NULL,patch17,0,0},
{1,NULL,patch18,0,0},
{1,NULL,patch19,0,0},
{0,NULL,patch20,0,0},
{0,NULL,patch21,0,0},
{0,NULL,patch22,0,0},
{0,NULL,patch23,0,0},
{0,NULL,patch24,0,0},
{1,NULL,patch25,0,0}
};


