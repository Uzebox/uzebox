
//INST: lead fast env
const char patch00[] PROGMEM ={ 
0,PC_WAVE,2,
0,PC_ENV_SPEED,-25,
0,PATCH_END
};

//INST: lead slow env
const char patch01[] PROGMEM ={ 
0,PC_WAVE,2,
0,PC_ENV_SPEED,-10,
0,PATCH_END
};

//INST: base -  triangular
const char patch02[] PROGMEM ={	
0,PC_WAVE,0,
0,PC_NOTE_HOLD,4,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END
};

//////////////Start Instrument Set 2 (StartGameSong)//////////

//INST: lead fast env
const char patch03[] PROGMEM ={ 
0,PC_WAVE,2,
//0,PC_NOTE_HOLD,4,
0,PC_ENV_SPEED,-5,
0,PATCH_END
};

//INST: lead slow env
const char patch04[] PROGMEM ={ 
0,PC_WAVE,3,
0,PC_ENV_SPEED,-3,
0,PATCH_END
};

//INST: base -  triangular "vibrato" patch
const char patch05[] PROGMEM ={	

0,PC_WAVE,4,
0,PC_ENV_SPEED,-1,
5,PC_NOTE_DOWN,1,
5,PC_NOTE_UP,1,
5,PC_NOTE_DOWN,1,
5,PC_NOTE_UP,1,
5,PC_NOTE_DOWN,1,
5,PC_NOTE_UP,1,
5,PC_NOTE_DOWN,1,
5,PC_NOTE_UP,1,
5,PC_NOTE_DOWN,1,
5,PC_NOTE_UP,1,
5,PC_NOTE_DOWN,1,
5,PC_NOTE_UP,1,
5,PC_NOTE_DOWN,1,
5,PC_NOTE_UP,1,
3,PC_NOTE_DOWN,1,
3,PC_NOTE_CUT,0,
0,PATCH_END
};

const char patch06[] PROGMEM ={//level 2 song,7th patch
0,PC_WAVE,3,
0,PC_ENV_SPEED,-40,
0,PATCH_END
};

const char sfxMarioWalk0[] PROGMEM ={ 
0,PC_WAVE,1,
0,PC_PITCH,90,
0,PC_ENV_SPEED,-32,
0,PC_NOTE_DOWN,26, 
1,PC_NOTE_UP,3,
1,PC_NOTE_UP,3, 
1,PC_NOTE_UP,3, 
1,PC_NOTE_UP,4, 
1,PC_NOTE_UP,4, 
1,PC_NOTE_DOWN,16, 
1,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxMarioWalk1[] PROGMEM ={ 
0,PC_WAVE,1,
0,PC_PITCH,90,
0,PC_ENV_SPEED,-32,
0,PC_NOTE_DOWN,29, 
1,PC_NOTE_UP,3,
1,PC_NOTE_UP,3, 
1,PC_NOTE_UP,3, 
1,PC_NOTE_UP,4, 
1,PC_NOTE_UP,4, 
1,PC_NOTE_DOWN,13, 
1,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxMarioWalk2[] PROGMEM ={ 
0,PC_WAVE,1,
0,PC_PITCH,90,
0,PC_ENV_SPEED,-32,
0,PC_NOTE_DOWN,32, 
1,PC_NOTE_UP,3,
1,PC_NOTE_UP,3, 
1,PC_NOTE_UP,3, 
1,PC_NOTE_UP,4, 
1,PC_NOTE_UP,4, 
1,PC_NOTE_DOWN,13, 
1,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxMarioJump[] PROGMEM = {
0,PC_WAVE,3,
//0,PC_ENV_SPEED,-8,
0,PC_PITCH,68,
1,PC_PITCH,69,
1,PC_PITCH,70,
1,PC_PITCH,72,
1,PC_PITCH,73,
2,PC_PITCH,72,
1,PC_PITCH,71,
0,PC_ENV_SPEED,-8,
1,PC_PITCH,70,
2,PC_PITCH,71,
1,PC_PITCH,72,
2,PC_PITCH,71,
1,PC_PITCH,70,
1,PC_PITCH,69,
2,PC_PITCH,71,
1,PC_PITCH,72,
2,PC_PITCH,71,
1,PC_PITCH,70,
1,PC_PITCH,69,
2,PC_PITCH,70,
1,PC_PITCH,71,
2,PC_PITCH,70,
1,PC_PITCH,69,
1,PC_PITCH,68,
2,PC_PITCH,70,
1,PC_NOTE_CUT,0,
0,PATCH_END,0,
};

const char sfxBonus[] PROGMEM = {//pickup?
0,PC_WAVE,5,//3 seems good too
0,PC_PITCH,70,
5,PC_PITCH,72,
6,PC_PITCH,74,
5,PC_PITCH,79,
10,PC_PITCH,75,
0,PC_ENV_SPEED,-16,
14,PC_NOTE_CUT,0,
0,PATCH_END,0,
};

const char sfxDestroy[] PROGMEM ={ 
0,PC_WAVE,4,
0,PC_PITCH,40,
0,PC_TREMOLO_LEVEL,0x78,     
0,PC_TREMOLO_RATE,90, 
3,PC_NOTE_UP,4,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,4,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,4,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,4,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,4,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
0,PC_ENV_SPEED,-12,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
1,PC_NOTE_UP,3,
3,PC_NOTE_DOWN,1,
3,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxKongStomp[] PROGMEM ={ 
0,PC_WAVE,3,
0,PC_ENV_SPEED,-4,
0,PC_PITCH,40,
1,PC_NOTE_DOWN,5, 
1,PC_NOTE_DOWN,4, 
1,PC_NOTE_DOWN,3, 
0,PC_WAVE,4,
0,PC_TREMOLO_LEVEL,0x90, 
0,PC_TREMOLO_RATE,0x60, 
2,PC_NOTE_DOWN,1,
2,PC_NOTE_DOWN,1,
2,PC_NOTE_DOWN,1,
2,PC_NOTE_DOWN,1,
2,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxSpringBounce[] PROGMEM ={ 
0,PC_WAVE,2,
0,PC_PITCH,54,
3,PC_NOTE_UP,4,
2,PC_NOTE_UP,4,
2,PC_NOTE_DOWN,8,
3,PC_NOTE_UP,4,
2,PC_NOTE_UP,4,
2,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxSpringDrop[] PROGMEM ={ 
0,PC_WAVE,2,
0,PC_PITCH,90,
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
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
0,PC_ENV_SPEED,-12,
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
1,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxDKFall[] PROGMEM ={//patch 1 of 2
//A bit of a "mega patch", not sure how you want to handle it all(or are currently). 
//All wave numbers are based off the full default set. The first part is an inlined
//version of the above patch. DK hitting his head part is also and inlined version 
//of the existing stomp patch(not much wasted space), so you may wish to break this
//into seperate patches and time it with code. Also, if you are using the 2 part patch
//for DK falling you'll need to call it 183 frames after you start this +/- your timing
//changes. I don't know a better way.

////////////////start final rivet sound(is it even different? replace if not)
0,PC_WAVE,4,

0,PC_PITCH,45,
1,PC_PITCH,50,
1,PC_PITCH,55,
1,PC_PITCH,60,
1,PC_PITCH,65,
1,PC_PITCH,69,
1,PC_PITCH,74,

1,PC_PITCH,45,
1,PC_PITCH,50,
1,PC_PITCH,55,
1,PC_PITCH,60,
1,PC_PITCH,65,
1,PC_PITCH,69,
1,PC_PITCH,74,
0,PC_ENV_SPEED,-35,//kill off sound

/////////////////^ end last rivet sound
/////////////////v start flail sound

30,PC_ENV_VOL,233,//reset envelope, adjust for frame delay between last rivet sound and DK flail
0,PC_WAVE,3,//might want to try different things here
0,PC_ENV_SPEED,-22,

0,PC_PITCH,31,
11,PC_ENV_VOL,233, 
0,PC_PITCH,28,
11,PC_ENV_VOL,233,

0,PC_PITCH,31,
11,PC_ENV_VOL,233, 
0,PC_PITCH,28,
11,PC_ENV_VOL,233, 

0,PC_PITCH,31,
11,PC_ENV_VOL,233, 
0,PC_PITCH,28,
11,PC_ENV_VOL,233, 

0,PC_PITCH,31,
11,PC_ENV_VOL,233, 
0,PC_PITCH,28,
11,PC_ENV_VOL,233,  

0,PC_PITCH,31,
11,PC_ENV_VOL,233, 
0,PC_PITCH,28,
11,PC_ENV_VOL,233,
 
0,PC_PITCH,31,

////////////////////^ end flail
////////////////////v start falling

//0,PC_WAVE,4,
30,PC_ENV_VOL,233,//adjust for delay between flail and fall
0,PC_ENV_SPEED,0,

0,PC_PITCH,77,
2,PC_PITCH,76,
2,PC_PITCH,75,
2,PC_PITCH,74,
2,PC_PITCH,73,
2,PC_PITCH,72,
1,PC_PITCH,71,

2,PC_PITCH,70,
2,PC_PITCH,69,
2,PC_PITCH,68,
2,PC_PITCH,67,
2,PC_PITCH,66,
2,PC_PITCH,65,
2,PC_PITCH,64,
2,PC_PITCH,63,
1,PC_PITCH,62,

2,PC_PITCH,61,
2,PC_PITCH,60,
2,PC_PITCH,59,
2,PC_PITCH,58,
2,PC_PITCH,57,
2,PC_PITCH,56,
2,PC_PITCH,55,
2,PC_PITCH,54,
1,PC_PITCH,53,

2,PC_PITCH,52,
2,PC_PITCH,51,
2,PC_PITCH,50,
2,PC_PITCH,49,
2,PC_PITCH,48,
2,PC_PITCH,47,
2,PC_PITCH,46,
2,PC_PITCH,45,

0,PC_ENV_SPEED,-35,//smooth off end

/////////////////^ end fall
/////////////////v start DK clunk head

30,PC_ENV_VOL,233,//adjust for delay between falling and clunk
0,PC_ENV_SPEED,-1,
0,PC_PITCH,32,
1,PC_PITCH,31,
1,PC_PITCH,30,
1,PC_PITCH,29,
1,PC_PITCH,28,
1,PC_PITCH,27,

1,PC_PITCH,25,
1,PC_PITCH,24,
1,PC_PITCH,23,
1,PC_PITCH,22,
1,PC_PITCH,21,

1,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxDKEscape[] PROGMEM ={ 
0,PC_WAVE,3,
0,PC_PITCH,50,
1,PC_PITCH,46,
1,PC_PITCH,50,
1,PC_PITCH,46,
1,PC_PITCH,50,
1,PC_PITCH,46,
1,PC_PITCH,50,
1,PC_PITCH,46,
1,PC_PITCH,50,
1,PC_PITCH,46,
1,PC_PITCH,50,
1,PC_PITCH,46,
1,PC_PITCH,50,
1,PC_PITCH,46,
1,PC_PITCH,50,
1,PC_PITCH,46,
1,PC_PITCH,50,
1,PC_PITCH,46,
1,PC_PITCH,50,
1,PC_PITCH,51,
2,PC_NOTE_CUT,0,
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
{0,NULL,sfxMarioWalk0,0,0},
{0,NULL,sfxMarioWalk0,0,0},
{0,NULL,sfxMarioWalk1,0,0},
{0,NULL,sfxMarioWalk2,0,0},
{0,NULL,sfxMarioJump,0,0},
{0,NULL,sfxBonus,0,0},
{0,NULL,sfxDestroy,0,0},
{0,NULL,sfxKongStomp,0,0},
{0,NULL,sfxSpringBounce,0,0},
{0,NULL,sfxSpringDrop,0,0},
{0,NULL,sfxDKFall,0,0},
{0,NULL,sfxDKEscape,0,0},
};

