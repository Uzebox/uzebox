
///////////////////start of Block Boy patches
//main song lead
const char patch00[] PROGMEM ={ 
0,PC_WAVE,4,
0,PC_NOTE_UP,12,
0,PC_ENV_VOL,67,
0,PC_TREMOLO_RATE,203,
0,PC_TREMOLO_LEVEL,158,

//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
3,PC_ENV_SPEED,-1,
3,PC_ENV_SPEED,-2,
3,PC_ENV_SPEED,-7,
3,PC_ENV_SPEED,-13,
//0,PC_NOTE_HOLD,0,
11,PC_NOTE_CUT,0,
0,PATCH_END

};

//main song bass
const char patch01[] PROGMEM ={ 
0,PC_WAVE,5,
0,PC_ENV_VOL,72,
0,PC_ENV_SPEED,8,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-7,
//7,PC_NOTE_CUT,0,
0,PATCH_END
};

//main song depth
const char patch02[] PROGMEM ={	
0,PC_WAVE,3,
0,PC_ENV_VOL,10,
1,PC_ENV_SPEED,7,
0,PC_TREMOLO_RATE,223,
0,PC_TREMOLO_LEVEL,170,
//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
7,PC_ENV_SPEED,-1,
19,PC_ENV_SPEED,-4,
0,PC_NOTE_HOLD,0,
3,PC_ENV_SPEED,-10,
4,PC_ENV_SPEED,-12,
//0,PC_NOTE_HOLD,0,
//9,PC_NOTE_CUT,0,
0,PATCH_END

};

//main song spare
const char patch03[] PROGMEM ={	  
0,PATCH_END
};

//main song bass drum
const char patch04[] PROGMEM ={	
	0,PC_NOISE_PARAMS,18,
	0,PC_ENV_VOL,46,
	0,PC_ENV_SPEED,-4,
	1,PC_NOISE_PARAMS,23,
	1,PC_NOISE_PARAMS,18,
	1,PC_ENV_SPEED,-12,  
	0,PATCH_END
};

//main song crash cymbal
const char patch05[] PROGMEM ={	
  	0,PC_NOISE_PARAMS,2,
  	0,PC_ENV_VOL,34,
  	0,PC_ENV_SPEED,-1,
	1,PC_NOISE_PARAMS,1,
	1,PC_NOISE_PARAMS,2,
	2,PC_NOISE_PARAMS,1,
	5,PC_ENV_SPEED,-2,
  	64,PATCH_END
};

//main song snare drum
const char patch06[] PROGMEM ={	
	0,PC_NOISE_PARAMS,5,
	0,PC_ENV_VOL,32,
	0,PC_ENV_SPEED,-8,
	2,PC_NOISE_PARAMS,7,
	2,PC_NOISE_PARAMS,3,
	1,PC_NOISE_PARAMS,1,
	2,PC_ENV_SPEED,-12,  
	0,PATCH_END
};

//main song spare
const char patch07[] PROGMEM ={	
	0,PATCH_END
};



//////////////////////////////////////////////

//title song lead
const char patch08[] PROGMEM ={ 
	0,PC_WAVE,4,
	0,PC_NOTE_UP,12,
	0,PC_ENV_VOL,67,
	0,PC_TREMOLO_RATE,203,
	0,PC_TREMOLO_LEVEL,158,
	3,PC_ENV_SPEED,-1,
	3,PC_ENV_SPEED,-2,
	3,PC_ENV_SPEED,-7,
	3,PC_ENV_SPEED,-13,
	11,PC_NOTE_CUT,0,
	0,PATCH_END
};


//title song bass
const char patch09[] PROGMEM ={ 
	0,PC_WAVE,5,
	0,PC_ENV_VOL,72,
	0,PC_ENV_SPEED,8,
	1,PC_NOTE_HOLD,0,
	0,PC_ENV_SPEED,-7,
	0,PATCH_END
};

//title song depth
const char patch10[] PROGMEM ={	
	0,PC_WAVE,3,
	0,PC_ENV_VOL,10,
	1,PC_ENV_SPEED,7,
	0,PC_TREMOLO_RATE,223,
	0,PC_TREMOLO_LEVEL,170,
	7,PC_ENV_SPEED,-1,
	19,PC_ENV_SPEED,-4,
	0,PC_NOTE_HOLD,0,
	3,PC_ENV_SPEED,-10,
	4,PC_ENV_SPEED,-12,
	0,PATCH_END
};

//title song spare
const char patch11[] PROGMEM ={	  
0,PATCH_END
};

//title song bass drum
const char patch12[] PROGMEM ={	
	0,PC_NOISE_PARAMS,18,
	0,PC_ENV_VOL,46,
	0,PC_ENV_SPEED,-4,
	1,PC_NOISE_PARAMS,23,
	1,PC_NOISE_PARAMS,18,
	1,PC_ENV_SPEED,-12,  
	0,PATCH_END
};

//title song crash cymbal
const char patch13[] PROGMEM ={	
  	0,PC_NOISE_PARAMS,2,
  	0,PC_ENV_VOL,34,
  	0,PC_ENV_SPEED,-1,
	1,PC_NOISE_PARAMS,1,
	1,PC_NOISE_PARAMS,2,
	2,PC_NOISE_PARAMS,1,
	5,PC_ENV_SPEED,-2,
  	64,PATCH_END
};

//title song snare drum
const char patch14[] PROGMEM ={	
	0,PC_NOISE_PARAMS,5,
	0,PC_ENV_VOL,32,
	0,PC_ENV_SPEED,-8,
	2,PC_NOISE_PARAMS,7,
	2,PC_NOISE_PARAMS,3,
	1,PC_NOISE_PARAMS,1,
	2,PC_ENV_SPEED,-12,  
	0,PATCH_END
};

//title song spare
const char patch15[] PROGMEM ={	
	0,PC_NOISE_PARAMS,1,
	0,PC_ENV_VOL,0x90,
	0,PC_ENV_SPEED,-30, 
	0,PATCH_END
};;

//////////////////end of Block Boy patches
//////////////////start of Alter Ego Patches
const char patch16[] PROGMEM ={//Title song 1
0,PC_TREMOLO_LEVEL,190,     
0,PC_TREMOLO_RATE,180, 
0,PC_WAVE,3,
0,PC_ENV_VOL,72,
0,PC_ENV_SPEED,16,
//1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-8,
10,PC_NOTE_CUT,0,
0,PATCH_END
  }; 
const char patch17[] PROGMEM ={//Title song 2
0,PC_TREMOLO_LEVEL,180,     
0,PC_TREMOLO_RATE,90, 
0,PC_WAVE,1,
0,PC_ENV_VOL,46,
0,PC_ENV_SPEED,10,
//1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-16,
8,PC_NOTE_CUT,0,
0,PATCH_END
  };

const char patch18[] PROGMEM ={//Title song 3
/*
0,PC_WAVE,4,
0,PC_ENV_VOL,108,
0,PC_ENV_SPEED,-35,
6,PC_NOTE_CUT,0,
0,PATCH_END
*/
};

const char patch19[] PROGMEM ={//Title song drums 1
0,PC_NOISE_PARAMS,122,
0,PC_ENV_VOL,35,
0,PC_ENV_SPEED,15,
1,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
3,PC_NOTE_CUT,0,
0,PATCH_END

/*
0,PC_NOISE_PARAMS,14,
0,PC_ENV_VOL,35,
0,PC_ENV_SPEED,20,
1,PC_ENV_SPEED,-20,
1,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-4,  
5,PC_NOTE_CUT,0,
0,PATCH_END
*/ 
 };


const char patch20[] PROGMEM ={//Title song drums 2
0,PC_NOISE_PARAMS,14,
0,PC_ENV_VOL,35,
0,PC_ENV_SPEED,10,
1,PC_ENV_SPEED,-20,
1,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-4,  
5,PC_NOTE_CUT,0,
0,PATCH_END
  };


const char patch21[] PROGMEM ={//Title song drums 3
/*
0,PC_NOISE_PARAMS,11,
0,PC_ENV_VOL,35,
0,PC_ENV_SPEED,-20, 
4,PC_NOTE_CUT,0,
0,PATCH_END
*/
};

/////////////////////////////////////////////////////
const char patch22[] PROGMEM ={//Stage 1 song 1
0,PC_WAVE,3,
0,PC_ENV_VOL,66,
0,PC_TREMOLO_RATE,100,
0,PC_TREMOLO_LEVEL,160,
//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-10,
4,PC_ENV_SPEED,-20,
//0,PC_NOTE_HOLD,0,
11,PATCH_END
  }; 
const char patch23[] PROGMEM ={//Stage 1 song 2
0,PC_WAVE,4,
0,PC_ENV_VOL,66,
0,PC_TREMOLO_RATE,90,
0,PC_TREMOLO_LEVEL,77,

//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-10,
3,PC_ENV_SPEED,-20,
//0,PC_NOTE_HOLD,0,
11,PATCH_END
  };

const char patch24[] PROGMEM ={//Stage 1 song 3
/*
0,PC_WAVE,0,
0,PC_ENV_VOL,60,
0,PC_TREMOLO_RATE,77,
0,PC_TREMOLO_LEVEL,77,
//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-10,
3,PC_ENV_SPEED,-20,
//0,PC_NOTE_HOLD,0,
11,PC_NOTE_CUT,0,
0,PATCH_END
*/
};

const char patch25[] PROGMEM ={//Stage 1 song drums 1
0,PC_NOISE_PARAMS,5,
0,PC_ENV_VOL,59,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8,  
1,PC_NOTE_CUT,0,
0,PATCH_END
  };


const char patch26[] PROGMEM ={//Stage 1 song drums 2
0,PC_NOISE_PARAMS,8,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
2,PC_NOTE_CUT,0,
0,PATCH_END
  };


const char patch27[] PROGMEM ={//Stage 1 song drums 3
/*
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x60,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
*/
};
//////////////////////////////////////////////////////////
const char patch28[] PROGMEM ={//Stage 2 song 1
0,PC_WAVE,5,
0,PC_ENV_VOL,79,
0,PC_TREMOLO_RATE,121,
0,PC_TREMOLO_LEVEL,88,
//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-10,
4,PC_ENV_SPEED,-15,
//0,PC_NOTE_HOLD,0,
5,PATCH_END
  }; 
const char patch29[] PROGMEM ={//Stage 2 song 2
0,PC_WAVE,4,
0,PC_ENV_VOL,81,
0,PC_TREMOLO_RATE,77,
0,PC_TREMOLO_LEVEL,77,

//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-5,
3,PC_ENV_SPEED,-20,
//0,PC_NOTE_HOLD,0,
5,PC_NOTE_CUT,0,
0,PATCH_END
  };

const char patch30[] PROGMEM ={//Stage 2 song 3
/*
0,PC_WAVE,4,
0,PC_ENV_SPEED,-25,
3,PC_NOTE_CUT,0,
0,PATCH_END
*/
};

const char patch31[] PROGMEM ={//Stage 2 song drums 1
0,PC_NOISE_PARAMS,11,
0,PC_ENV_VOL,40,
0,PC_ENV_SPEED,-15, 
8,PC_NOTE_CUT,0,
0,PATCH_END
};


const char patch32[] PROGMEM ={//Stage 2 song drums 2
0,PC_NOISE_PARAMS,112,
0,PC_ENV_VOL,40,
0,PC_ENV_SPEED,-20, 
8,PC_NOTE_CUT,0,
0,PATCH_END
};


const char patch33[] PROGMEM ={//Stage 2 song drums 3
/*
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x90,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
*/
};
/////////////////////////////////////////////////////////
const char patch34[] PROGMEM ={//Stage 3 song 1
0,PC_WAVE,5,
0,PC_ENV_VOL,82,
0,PC_TREMOLO_RATE,121,
0,PC_TREMOLO_LEVEL,55,
//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-10,
4,PC_ENV_SPEED,-17,
0,PC_WAVE,3,
//0,PC_NOTE_HOLD,0,
5,PATCH_END
  }; 
const char patch35[] PROGMEM ={//Stage 3 song 2
0,PC_WAVE,4,
0,PC_ENV_VOL,84,
0,PC_TREMOLO_RATE,180,
0,PC_TREMOLO_LEVEL,60,
//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,7,
1,PC_ENV_SPEED,-5,
0,PC_TREMOLO_LEVEL,90,
5,PC_ENV_SPEED,-15,
0,PC_WAVE,5,
0,PC_TREMOLO_LEVEL,120,
//0,PC_NOTE_HOLD,0,
5,PATCH_END
  };

const char patch36[] PROGMEM ={//Stage 3 song 3
/*
0,PC_WAVE,4,
0,PC_ENV_SPEED,-35,
0,PATCH_END
*/
};

const char patch37[] PROGMEM ={//Stage 3 song drums 1
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x1C,
0,PC_ENV_SPEED,-15, 
8,PC_NOTE_CUT,0,
0,PATCH_END
};


const char patch38[] PROGMEM ={//Stage 3 song drums 2
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x2C,
0,PC_ENV_SPEED,-20, 
8,PC_NOTE_CUT,0,
0,PATCH_END
};


const char patch39[] PROGMEM ={//Stage 3 song drums 3
/*
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x90,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
*/
};
///////////////////////////////////////////////////////
const char patch40[] PROGMEM ={//Stage 4 song 1
0,PC_WAVE,5,
0,PC_ENV_VOL,80,
0,PC_TREMOLO_RATE,180,
0,PC_TREMOLO_LEVEL,120,
//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-10,
1,PC_ENV_SPEED,-20,
1,PC_WAVE,3,
1,PC_ENV_SPEED,-25,
//0,PC_NOTE_HOLD,0,
5,PATCH_END
}; 

const char patch41[] PROGMEM ={//Stage 4 song 2
0,PC_WAVE,0,
0,PC_ENV_VOL,98,
0,PC_TREMOLO_RATE,188,
0,PC_TREMOLO_LEVEL,122,
//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-5,
1,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-40,
2,PC_WAVE,3,
0,PC_TREMOLO_RATE,99,
0,PC_TREMOLO_LEVEL,90,
//0,PC_NOTE_HOLD,0,
5,PATCH_END
};

const char patch42[] PROGMEM ={//Stage 4 song 3
/*
0,PC_WAVE,4,
0,PC_ENV_SPEED,-35,
0,PATCH_END
*/
};

const char patch43[] PROGMEM ={//Stage 4 song drums 1
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,35,
0,PC_ENV_SPEED,-15, 
4,PC_NOTE_CUT,0,
0,PATCH_END
};


const char patch44[] PROGMEM ={//Stage 4 song drums 2
0,PC_NOISE_PARAMS,3,
0,PC_ENV_VOL,35,
0,PC_ENV_SPEED,-20, 
4,PC_NOTE_CUT,0,
0,PATCH_END
  };


const char patch45[] PROGMEM ={//Stage 4 song drums 3
/*
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,44,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
*/
};
/////////////////////////////////////////////////////
const char patch46[] PROGMEM ={//Stage 5 song 1
0,PC_TREMOLO_LEVEL,210,     
0,PC_TREMOLO_RATE,110, 
0,PC_WAVE,4,
0,PC_ENV_VOL,93,
0,PC_ENV_SPEED,16,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-8,
8,PC_NOTE_CUT,0,
0,PATCH_END
  }; 
const char patch47[] PROGMEM ={//Stage 5 song 2
//0,PC_TREMOLO_LEVEL,180,     
//0,PC_TREMOLO_RATE,8, 
0,PC_WAVE,1,
0,PC_ENV_VOL,67,
0,PC_ENV_SPEED,8,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-16,
8,PC_NOTE_CUT,0,
0,PATCH_END
  };

const char patch48[] PROGMEM ={//Stage 5 song 3
/*
0,PC_WAVE,4,
0,PC_ENV_SPEED,-35,
0,PATCH_END
*/
};

const char patch49[] PROGMEM ={//Stage 5 song drums 1
0,PC_NOISE_PARAMS,7,
0,PC_ENV_VOL,35,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END

/*0,PC_NOISE_PARAMS,3,
0,PC_ENV_VOL,35,
0,PC_ENV_SPEED,30,
1,PC_ENV_SPEED,-20,
1,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-4,  
5,PC_NOTE_CUT,0,
0,PATCH_END
*/
};


const char patch50[] PROGMEM ={//Stage 5 song drums 2
0,PC_NOISE_PARAMS,2,
0,PC_ENV_VOL,35,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
//1,PC_ENV_SPEED,-8, 
1,PC_NOTE_CUT,0,
0,PATCH_END
};


const char patch51[] PROGMEM ={//Stage 5 song drums 3
/*
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x90,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
*/
};
///////////////////////////////////////////////////////////
const char patch52[] PROGMEM ={//Game over song 1
/*
//0,PC_TREMOLO_LEVEL,180,     
//0,PC_TREMOLO_RATE,32, 
0,PC_WAVE,4,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,8,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-16,
8,PC_NOTE_CUT,0,
0,PATCH_END
*/
  }; 
const char patch53[] PROGMEM ={//Game over song 2
/*
0,PC_TREMOLO_LEVEL,180,     
0,PC_TREMOLO_RATE,16, 
0,PC_WAVE,5,
0,PC_ENV_VOL,65,
0,PC_ENV_SPEED,8,
1,PC_NOTE_HOLD,0,
0,PC_WAVE,3,
0,PC_ENV_SPEED,-16,
8,PC_NOTE_CUT,0,
0,PATCH_END
*/ 
};

const char patch54[] PROGMEM ={//Game over song 3
/*
0,PC_WAVE,4,
0,PC_ENV_SPEED,-35,
0,PATCH_END
*/
};

const char patch55[] PROGMEM ={//Game over song drums 1
/*
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x90,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
*/
};


const char patch56[] PROGMEM ={//Game over song drums 2
/*
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x90,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
*/
};


const char patch57[] PROGMEM ={//Game over song drums 3
/*
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x90,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
*/
};
///////////////////////////////////////////////////////////
const char patch58[] PROGMEM ={//Well done song 1
0,PC_TREMOLO_LEVEL,170,     
0,PC_TREMOLO_RATE,180, 
0,PC_WAVE,4,
0,PC_ENV_VOL,100,
0,PC_ENV_SPEED,12,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-8,
8,PC_NOTE_CUT,0,
0,PATCH_END
  }; 
const char patch59[] PROGMEM ={//Well done song 2
0,PC_WAVE,4,
0,PC_ENV_VOL,88,
0,PC_TREMOLO_RATE,180,
0,PC_TREMOLO_LEVEL,60,
//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,7,
1,PC_ENV_SPEED,-5,
0,PC_TREMOLO_LEVEL,90,
5,PC_ENV_SPEED,-15,
0,PC_WAVE,5,
0,PC_TREMOLO_LEVEL,120,
//0,PC_NOTE_HOLD,0,
5,PATCH_END
  };

const char patch60[] PROGMEM ={//Well done song 3
/*
0,PC_WAVE,4,
0,PC_ENV_SPEED,-35,
0,PATCH_END
*/
};

const char patch61[] PROGMEM ={//Well done song drums 1
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,40,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
};


const char patch62[] PROGMEM ={//Well done song drums 2
0,PC_ENV_VOL,40,
2,PC_NOISE_PARAMS,2,//this delay helps to align drum timing
//couldn't get it right in the midi...
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
//1,PC_ENV_SPEED,-8, 
1,PC_NOTE_CUT,0,
0,PATCH_END
};


const char patch63[] PROGMEM ={//Well done song drums 3
/*
0,PC_NOISE_PARAMS,41,
0,PC_ENV_VOL,40,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
*/
};
/////////////////////////////////////////////////////////



const struct PatchStruct patches[] PROGMEM = {
//sfx
{0,NULL,patch00,0,0},//
{0,NULL,patch01,0,0},//
{0,NULL,patch02,0,0},//
{0,NULL,patch03,0,0},//
{1,NULL,patch04,0,0},//
{1,NULL,patch05,0,0},//
{1,NULL,patch06,0,0},//
{0,NULL,patch07,0,0},//
{0,NULL,patch08,0,0},//
{0,NULL,patch09,0,0},//
{0,NULL,patch10,0,0},//
{0,NULL,patch11,0,0},//
{1,NULL,patch12,0,0},//
{1,NULL,patch13,0,0},//
{1,NULL,patch14,0,0},//
{1,NULL,patch15,0,0},//
//instruments
{0,NULL,patch16,0,0},//
{0,NULL,patch17,0,0},//
{0,NULL,patch18,0,0},//
{1,NULL,patch19,0,0},//
{1,NULL,patch20,0,0},//
{1,NULL,patch21,0,0},//

{0,NULL,patch22,0,0},//
{0,NULL,patch23,0,0},//
{0,NULL,patch24,0,0},//
{1,NULL,patch25,0,0},//
{1,NULL,patch26,0,0},//
{1,NULL,patch27,0,0},//

{0,NULL,patch28,0,0},//
{0,NULL,patch29,0,0},//
{0,NULL,patch30,0,0},//
{1,NULL,patch31,0,0},//
{1,NULL,patch32,0,0},//
{1,NULL,patch33,0,0},//

{0,NULL,patch34,0,0},//
{0,NULL,patch35,0,0},//
{0,NULL,patch36,0,0},//
{1,NULL,patch37,0,0},//
{1,NULL,patch38,0,0},//
{1,NULL,patch39,0,0},//


{0,NULL,patch40,0,0},//
{0,NULL,patch41,0,0},//
{0,NULL,patch42,0,0},//
{1,NULL,patch43,0,0},//
{1,NULL,patch44,0,0},//
{1,NULL,patch45,0,0},//


{0,NULL,patch46,0,0},//
{0,NULL,patch47,0,0},//
{0,NULL,patch48,0,0},//
{1,NULL,patch49,0,0},//
{1,NULL,patch50,0,0},//
{1,NULL,patch51,0,0},//


{0,NULL,patch52,0,0},//
{0,NULL,patch53,0,0},//
{0,NULL,patch54,0,0},//
{1,NULL,patch55,0,0},//
{1,NULL,patch56,0,0},//
{1,NULL,patch57,0,0},//


{0,NULL,patch58,0,0},//
{0,NULL,patch59,0,0},//
{0,NULL,patch60,0,0},//
{1,NULL,patch61,0,0},//
{1,NULL,patch62,0,0},//
{1,NULL,patch63,0,0},//
};

