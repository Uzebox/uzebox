#include "intro.inc"

//FX: Moving in menu
const char patch00[] PROGMEM ={ 
0,PC_WAVE,4,
0,PC_ENV_VOL,0x70,
0,PC_ENV_SPEED,-40,
0,PC_PITCH,60,
1,PC_NOTE_CUT,0,
0,PATCH_END
};

//FX: Can't move in menu
const char patch01[] PROGMEM ={ 
0,PC_WAVE,5,
0,PC_ENV_VOL,0xa0,
0,PC_PITCH,30,
5,PC_NOTE_CUT,0,
0,PATCH_END
};

//FX: Select level in menu
const char patch02[] PROGMEM ={ 
0,PC_WAVE,9,
0,PC_ENV_VOL,0xD0,
0,PC_ENV_SPEED,-20,
0,PC_PITCH,60,
5,PC_NOTE_UP,12,
5,PC_NOTE_CUT,0,
0,PATCH_END
};

//FX: Catch atom
const char patch03[] PROGMEM ={ 
0,PC_WAVE,3,
0,PC_ENV_VOL,0xa0,
0,PC_ENV_SPEED,-10,
0,PC_PITCH,40,
1,PC_NOTE_UP,6,
1,PC_NOTE_UP,6,
1,PC_NOTE_UP,6,
1,PC_NOTE_UP,6,
1,PC_NOTE_UP,6,
1,PC_NOTE_CUT,0,
0,PATCH_END
};

//FX: Release atom
const char patch04[] PROGMEM ={ 
0,PC_WAVE,3,
0,PC_ENV_VOL,0xa0,
0,PC_ENV_SPEED,-10,
0,PC_PITCH,70,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_DOWN,6,
1,PC_NOTE_CUT,0,
0,PATCH_END
};

//FX: Moving atom
const char patch05[] PROGMEM ={
0,PC_WAVE,3,
0,PC_ENV_VOL,0x60,
0,PC_ENV_SPEED,0,
0,PC_PITCH,20,
10,PC_NOTE_CUT,0,
0,PATCH_END
};

//FX: Level cleared
const char patch06[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_ENV_SPEED,-40,

0,PC_PITCH,80,
0,PC_ENV_VOL,240,

5,PC_NOTE_DOWN,2,
0,PC_ENV_VOL,240,

5,PC_NOTE_DOWN,2,
0,PC_ENV_VOL,240,

5,PC_NOTE_UP,2,
0,PC_ENV_VOL,240,

5,PC_NOTE_UP,2,
0,PC_ENV_VOL,240,

5,PC_ENV_VOL,240,

5,PC_ENV_VOL,240,

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
{1,intro_PCM_Data,NULL,sizeof(intro_PCM_Data),sizeof(intro_PCM_Data)}
};