
const char patch00[] PROGMEM ={
  0,PC_WAVE,8,
  0,PC_ENV_SPEED,-5,
  0,PATCH_END
};

const char patch01[] PROGMEM ={ 
  0,PC_WAVE,3,
  0,PC_ENV_SPEED,-10,
  0,PATCH_END
};

const char patch02[] PROGMEM ={	
  0,PC_ENV_VOL,0,  
  0,PC_NOTE_CUT,0, 
  0,PATCH_END
};

const char patch03[] PROGMEM ={	
  0,PC_NOTE_CUT,0,
  0,PATCH_END
};

const char patch04[] PROGMEM ={
  0,PC_NOTE_CUT,0,
  0,PATCH_END
}; 

const char patch05[] PROGMEM ={ 
  0,PC_NOTE_CUT,0,
  0,PATCH_END
};

const char patch06[] PROGMEM ={ 
  0,PC_NOTE_CUT,0,
  0,PATCH_END
};

const char patch07[] PROGMEM ={	
  0,PC_NOTE_CUT,0,
  0,PATCH_END
};

const char patch08[] PROGMEM ={	
  0,PC_NOTE_CUT,0,
  0,PATCH_END
};

const char patch09[] PROGMEM ={
  0,PC_NOTE_CUT,0,
  0,PATCH_END
}; 

const char patch10[] PROGMEM ={ 
  0,PC_NOTE_CUT,0,
  0,PATCH_END
};

const char patch11[] PROGMEM ={ 
  0,PC_NOTE_CUT,0,
  0,PATCH_END
};

const char sfxHop0[] PROGMEM ={
0,PC_ENV_SPEED,-10,
0,PC_PITCH,55,
1,PC_NOTE_UP,5,  
1,PC_NOTE_UP,5, 
2,PC_NOTE_UP,5,  
2,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxHop1[] PROGMEM ={
0,PC_ENV_SPEED,-10,
0,PC_PITCH,55,
1,PC_NOTE_UP,6,  
1,PC_NOTE_UP,6, 
2,PC_NOTE_UP,6,  
2,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxHop2[] PROGMEM ={ 
0,PC_ENV_SPEED,-10,
0,PC_PITCH,55,
1,PC_NOTE_UP,7,  
1,PC_NOTE_UP,7, 
2,PC_NOTE_UP,7,  
2,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxBeep[] PROGMEM ={
0,PC_WAVE,9,
0,PC_PITCH,65,
0,PC_ENV_SPEED,-5,
10,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxDead[] PROGMEM ={
0,PC_WAVE,1,
0,PC_ENV_SPEED,-5,
0,PC_PITCH,55,
2,PC_NOTE_DOWN,6,  
2,PC_NOTE_DOWN,6, 
2,PC_NOTE_DOWN,6,  
4,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxBonus[] PROGMEM ={
0,PC_WAVE,2,
0,PC_ENV_VOL,0xff,
2,PC_NOTE_UP,6,
2,PC_NOTE_UP,6,
0,PC_ENV_SPEED,-10,
0,PATCH_END
};

const char sfxTimeBonus[] PROGMEM ={
0,PC_WAVE,7,
0,PC_PITCH,60,
1,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxLifeLoss2P[] PROGMEM ={
0,PC_ENV_SPEED,-5,
0,PC_PITCH,60,
2,PC_NOTE_DOWN,6,  
2,PC_NOTE_DOWN,6, 
2,PC_NOTE_DOWN,6,  
4,PC_NOTE_CUT,0,
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
{0,NULL,sfxHop0,0,0},
{0,NULL,sfxHop1,0,0},
{0,NULL,sfxHop1,0,0},
{0,NULL,sfxHop2,0,0},
{0,NULL,sfxBeep,0,0},
{0,NULL,sfxDead,0,0},
{0,NULL,sfxBonus,0,0},
{0,NULL,sfxTimeBonus,0,0},
{0,NULL,sfxLifeLoss2P,0,0},
};
