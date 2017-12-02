//Original FamiTone format data is included for sound effects..
//In case someone ever wanted to make them sound good.
const char patch00[] PROGMEM ={//title select
 	0,PC_WAVE,2,
	0,PC_ENV_VOL,132,
	0,PC_ENV_SPEED,-1,
	0,PC_PITCH,67,
	5,PC_PITCH,79,
	5,PC_PITCH,72,
	5,PC_PITCH,84,
	//0,PC_ENV_VOL,64,
	0,PC_ENV_SPEED,-2,
	5,PC_PITCH,67,
	5,PC_PITCH,79,
	5,PC_PITCH,72,
	5,PC_PITCH,84,
	//0,PC_ENV_VOL,34,
	0,PC_ENV_SPEED,-4,
	5,PC_PITCH,67,
	5,PC_PITCH,79,
	5,PC_PITCH,72,
	5,PC_PITCH,84,
	1,PC_NOTE_CUT,0,
	0,PATCH_END
/*
.byte $00,$3f
	.byte $01,$1c
	.byte $02,$01
	.byte $14
	.byte $01,$8e
	.byte $02,$00
	.byte $14
	.byte $01,$d5
	.byte $14
	.byte $01,$6a
	.byte $14
	.byte $00,$34
	.byte $01,$1c
	.byte $02,$01
	.byte $14
	.byte $01,$8e
	.byte $02,$00
	.byte $14
	.byte $01,$d5
	.byte $14
	.byte $01,$6a
	.byte $14
	.byte $00,$31
	.byte $01,$1c
	.byte $02,$01
	.byte $14
	.byte $01,$8e
	.byte $02,$00
	.byte $14
	.byte $01,$d5
	.byte $14
	.byte $01,$6a
	.byte $14
	.byte $00,$30
	.byte $ff
*/
  };

const char patch01[] PROGMEM ={//menu blip
 	0,PC_WAVE,6,
	0,PC_ENV_VOL,128,
	0,PC_ENV_SPEED,-7,

	0,PC_PITCH,72,
	3,PC_PITCH,84,
	3,PC_PITCH,72,
	3,PC_PITCH,84,
	4,PC_NOTE_CUT,0,
	0,PATCH_END
/*
.byte $00,$3f
	.byte $01,$d5
	.byte $12
	.byte $01,$6a
	.byte $12
	.byte $00,$32
	.byte $01,$d5
	.byte $12
	.byte $01,$6a
	.byte $12
	.byte $00,$30
	.byte $2d
	.byte $02,$00
	.byte $ff
*/
  };

const char patch02[] PROGMEM ={//exchange
 	0,PC_WAVE,2,
	0,PC_ENV_VOL,130,
	0,PC_PITCH,96,
	2,PC_PITCH,108,
	2,PC_ENV_VOL,0,
	4,PC_ENV_VOL,128,
	0,PC_PITCH,96,
	2,PC_PITCH,108,
	2,PC_NOTE_CUT,0,
	0,PATCH_END
/*
.byte $09,$3f
	.byte $0a,$86
	.byte $01,$00
	.byte $10
	.byte $0a,$8a
	.byte $10
	.byte $0a,$86
	.byte $10
	.byte $0a,$8a
	.byte $10
	.byte $09,$34
	.byte $0a,$86
	.byte $10
	.byte $0a,$8a
	.byte $10
	.byte $0a,$86
	.byte $10
	.byte $0a,$8a
	.byte $10
	.byte $09,$32
	.byte $0a,$86
	.byte $10
	.byte $0a,$80
	.byte $10
	.byte $0a,$86
	.byte $10
	.byte $0a,$80
	.byte $10
	.byte $09,$30
	.byte $ff
*/
  };

const char patch03[] PROGMEM ={//item 1
 	0,PC_WAVE,2,
	0,PC_ENV_VOL,128,
	0,PC_PITCH,93,
	2,PC_PITCH,105,
	2,PC_ENV_VOL,0,
	4,PC_ENV_VOL,128,
	0,PC_PITCH,93,
	2,PC_PITCH,105,
	2,PC_NOTE_CUT,0,
	0,PATCH_END
/*
.byte $00,$3f
	.byte $01,$34
	.byte $11
	.byte $01,$1a
	.byte $11
	.byte $00,$30
	.byte $13
	.byte $00,$32
	.byte $01,$34
	.byte $02,$00
	.byte $11
	.byte $01,$1a
	.byte $11
	.byte $00,$30
	.byte $ff
*/
  };



const char patch04[] PROGMEM ={//item 2
 	0,PC_WAVE,2,
	0,PC_ENV_VOL,128,
	0,PC_PITCH,97,
	2,PC_PITCH,110,
	2,PC_ENV_VOL,0,
	4,PC_ENV_VOL,128,
	0,PC_PITCH,97,
	2,PC_PITCH,110,
	2,PC_NOTE_CUT,0,
	0,PATCH_END
/*
.byte $00,$3f
	.byte $01,$3f
	.byte $11
	.byte $01,$1f
	.byte $11
	.byte $00,$30
	.byte $02,$00
	.byte $13
	.byte $00,$32
	.byte $01,$3f
	.byte $11
	.byte $01,$1f
	.byte $11
	.byte $00,$30
	.byte $ff
*/
  };

const char patch05[] PROGMEM ={//SHOULD BE DEATH SOUND
 	0,PC_WAVE,5,
0,PC_TREMOLO_LEVEL,180,     
0,PC_TREMOLO_RATE,160, 

	0,PC_ENV_VOL,62,
0,PC_ENV_SPEED,26,
	0,PC_PITCH,69,
	6,PC_PITCH,68,
0,PC_ENV_SPEED,-3,
0,PC_WAVE,4,
	6,PC_PITCH,69,
	6,PC_PITCH,68,
0,PC_ENV_SPEED,-6,
	6,PC_PITCH,69,
	6,PC_PITCH,68,
0,PC_WAVE,3,
	6,PC_PITCH,69,
	6,PC_PITCH,68,
	6,PC_NOTE_CUT,0,
	0,PATCH_END


//SECONDARY PATCH?
//0,PC_PITCH,69,
//12,PC_PITCH,69,
//12,PC_PITCH,69,

/*
.byte $00,$7f
	.byte $01,$ff
	.byte $03,$7a
	.byte $04,$fd
	.byte $05,$00
	.byte $11
	.byte $01,$01
	.byte $02,$01
	.byte $11
	.byte $01,$03
	.byte $11
	.byte $01,$05
	.byte $03,$30
	.byte $11
	.byte $01,$07
	.byte $11
	.byte $01,$09
	.byte $11
	.byte $00,$76
	.byte $01,$ff
	.byte $02,$00
	.byte $03,$74
	.byte $11
	.byte $01,$01
	.byte $02,$01
	.byte $11
	.byte $01,$03
	.byte $11
	.byte $01,$05
	.byte $03,$30
	.byte $11
	.byte $01,$07
	.byte $11
	.byte $01,$09
	.byte $11
	.byte $00,$72
	.byte $01,$ff
	.byte $02,$00
	.byte $03,$72
	.byte $11
	.byte $01,$01
	.byte $02,$01
	.byte $11
	.byte $01,$03
	.byte $11
	.byte $01,$05
	.byte $03,$30
	.byte $11
	.byte $01,$07
	.byte $11
	.byte $01,$09
	.byte $11
	.byte $00,$71
	.byte $01,$ff
	.byte $02,$00
	.byte $11
	.byte $01,$01
	.byte $02,$01
	.byte $11
	.byte $01,$03
	.byte $11
	.byte $01,$05
	.byte $11
	.byte $01,$07
	.byte $11
	.byte $01,$09
	.byte $11
	.byte $00,$30
	.byte $ff
*/
  };

const char patch06[] PROGMEM ={//fall
 	0,PC_WAVE,2,
	0,PC_ENV_VOL,122,
0,PC_TREMOLO_LEVEL,180,     
0,PC_TREMOLO_RATE,80, 

	0,PC_PITCH,107,
	2,PC_PITCH,105,
	2,PC_PITCH,104,
	2,PC_PITCH,103,
	2,PC_PITCH,102,
0,PC_ENV_SPEED,-7,
	2,PC_PITCH,101,
	4,PC_PITCH,100,
	2,PC_PITCH,99,
	2,PC_PITCH,98,
	2,PC_PITCH,97,
	0,PC_ENV_SPEED,-16,
	2,PC_NOTE_CUT,0,
	0,PATCH_END
/*
.byte $00,$78
	.byte $01,$1c
	.byte $02,$00
	.byte $04,$00
	.byte $11
	.byte $01,$1e
	.byte $11
	.byte $01,$20
	.byte $11
	.byte $00,$7a
	.byte $01,$22
	.byte $11
	.byte $01,$24
	.byte $11
	.byte $01,$26
	.byte $11
	.byte $00,$78
	.byte $01,$28
	.byte $11
	.byte $01,$2a
	.byte $11
	.byte $01,$2c
	.byte $11
	.byte $00,$72
	.byte $01,$2e
	.byte $11
	.byte $01,$30
	.byte $11
	.byte $01,$32
	.byte $11
	.byte $00,$30
	.byte $ff

*/
  };

const char patch07[] PROGMEM ={//drop
 	0,PC_WAVE,3,
	0,PC_ENV_VOL,101,
	0,PC_PITCH,45,//45
	1,PC_PITCH,43,//43
	1,PC_PITCH,42,//42
//	1,PC_ENV_VOL,0,
//	9,PC_ENV_VOL,90,
	0,PC_ENV_SPEED,-30,
	0,PC_PITCH,45,//45
	1,PC_PITCH,43,//43
	1,PC_PITCH,42,//42
	1,PC_NOTE_CUT,0,
	0,PATCH_END
/*
.byte $00,$bd
	.byte $01,$d6
	.byte $02,$03
	.byte $04,$00
	.byte $10
	.byte $01,$56
	.byte $02,$04
	.byte $10
	.byte $01,$d6
	.byte $10
	.byte $00,$30
	.byte $01,$56
	.byte $02,$05
	.byte $10
	.byte $01,$d6
	.byte $10
	.byte $01,$56
	.byte $02,$06
	.byte $10
	.byte $01,$d6
	.byte $10
	.byte $01,$56
	.byte $02,$07
	.byte $10
	.byte $01,$d6
	.byte $10
	.byte $01,$ff
	.byte $12
	.byte $00,$b2
	.byte $01,$d6
	.byte $02,$03
	.byte $10
	.byte $01,$56
	.byte $02,$04
	.byte $10
	.byte $01,$d6
	.byte $10
	.byte $00,$30
	.byte $01,$56
	.byte $02,$05
	.byte $10
	.byte $01,$d6
	.byte $10
	.byte $01,$56
	.byte $02,$06
	.byte $10
	.byte $01,$d6
	.byte $10
	.byte $01,$56
	.byte $02,$07
	.byte $10
	.byte $01,$d6
	.byte $10
	.byte $01,$ff
	.byte $ff
*/
  };


const char patch08[] PROGMEM ={//no exchange
0,PC_WAVE,2,
0,PC_ENV_VOL,128,
0,PC_ENV_SPEED,10,
0,PC_PITCH,59,
1,PC_ENV_SPEED,-5,
2,PC_ENV_SPEED,-13,
0,PC_PITCH,54,
1,PC_PITCH,59,
2,PC_ENV_SPEED,-22,
2,PC_PITCH,54,
0,PC_ENV_SPEED,-26,
3,PC_NOTE_CUT,0,
0,PATCH_END,0,
/*
.byte $09,$3f
	.byte $0a,$86
	.byte $11
	.byte $0a,$84
	.byte $11
	.byte $09,$30
	.byte $11
	.byte $09,$38
	.byte $0a,$86
	.byte $11
	.byte $0a,$84
	.byte $11
	.byte $09,$30
	.byte $ff
*/
  };

const char patch09[] PROGMEM ={//out of exchanges
0,PC_WAVE,2,
0,PC_ENV_VOL,132,
0,PC_ENV_SPEED,10,
0,PC_PITCH,45,
1,PC_PITCH,44,
1,PC_ENV_SPEED,-5,
2,PC_ENV_SPEED,-13,
0,PC_PITCH,40,
2,PC_ENV_SPEED,-22,
6,PC_NOTE_CUT,0,
0,PATCH_END,0,
/*
.byte $06,$81
	.byte $07,$8e
	.byte $08,$00
	.byte $10
	.byte $07,$3a
	.byte $08,$02
	.byte $11
	.byte $06,$00
	.byte $12
	.byte $06,$81
	.byte $07,$8e
	.byte $08,$00
	.byte $10
	.byte $07,$3a
	.byte $08,$02
	.byte $10
	.byte $06,$00
	.byte $ff
*/
  };

const char patch10[] PROGMEM ={//bridge
0,PC_NOISE_PARAMS,41,
0,PC_ENV_VOL,78,
0,PC_ENV_SPEED,-10,
1,PC_NOISE_PARAMS,34,
1,PC_NOISE_PARAMS,88, 
3,PC_NOTE_CUT,0,
0,PATCH_END,0

/*
0,PC_WAVE,2,
0,PC_ENV_VOL,96,
0,PC_ENV_SPEED,10,
0,PC_PITCH,45,
2,PC_ENV_SPEED,-5,
3,PC_ENV_SPEED,-13,
0,PC_PITCH,40,
3,PC_PITCH,45,
3,PC_ENV_SPEED,-22,
3,PC_PITCH,40,
0,PC_ENV_SPEED,-26,
2,PC_NOTE_CUT,0,
0,PATCH_END,0,
*/
/*
.byte $09,$34
	.byte $0a,$00
	.byte $10
	.byte $09,$30
	.byte $10
	.byte $09,$3a
	.byte $0a,$01
	.byte $10
	.byte $09,$30
	.byte $10
	.byte $09,$34
	.byte $0a,$03
	.byte $10
	.byte $09,$30
	.byte $ff

*/
  };

const char patch11[] PROGMEM ={//walk 1
 	0,PC_WAVE,2,
	0,PC_ENV_VOL,222,
	0,PC_ENV_SPEED,-10,
	0,PC_PITCH,49,//45
	1,PC_PITCH,47,//43
	1,PC_PITCH,46,//42
	1,PC_ENV_VOL,0,
//	9,PC_ENV_VOL,138,
//	0,PC_PITCH,45,
//	1,PC_PITCH,43,
//	1,PC_PITCH,42,
	
9,PC_NOTE_CUT,0,
	0,PATCH_END
/*
.byte $09,$3a
	.byte $0a,$0f
	.byte $10
	.byte $09,$30
	.byte $ff

*/
  };



const char patch12[] PROGMEM ={//walk 2
 	0,PC_WAVE,2,
	0,PC_ENV_VOL,218,
	0,PC_ENV_SPEED,-10,
	0,PC_PITCH,50,//46
	1,PC_PITCH,46,//42
	1,PC_PITCH,45,//41
	1,PC_ENV_VOL,0,
//	9,PC_ENV_VOL,132,
//	0,PC_PITCH,45,
//	1,PC_PITCH,43,
//	1,PC_PITCH,42,
	
9,PC_NOTE_CUT,0,
	0,PATCH_END
/*
.byte $09,$3a
	.byte $0a,$0e
	.byte $10
	.byte $09,$30
	.byte $ff
*/
  };

const char patch13[] PROGMEM ={//ladder - noise channel
0,PC_NOISE_PARAMS,38,
0,PC_ENV_VOL,86,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
1,PC_NOTE_CUT,0,
0,PATCH_END
/*
.byte $09,$38
	.byte $0a,$06
	.byte $10
	.byte $0a,$03
	.byte $10
	.byte $09,$30
	.byte $ff

*/
  }; 


const char patch14[] PROGMEM ={//level clear
 	0,PC_WAVE,2,
	0,PC_ENV_VOL,143,
	0,PC_PITCH,60,
	6,PC_PITCH,65,
	6,PC_PITCH,67,


	3,PC_PITCH,60,
	3,PC_PITCH,71,
	3,PC_PITCH,65,
	3,PC_PITCH,72,
	3,PC_PITCH,67,
	3,PC_PITCH,77,
	3,PC_PITCH,71,
	3,PC_PITCH,79,
	3,PC_PITCH,72,
	3,PC_PITCH,83,
	3,PC_PITCH,77,


//3,PC_WAVE,4,
//0,PC_ENV_VOL,66,
3,PC_PITCH,84,
	3,PC_PITCH,83,
	0,PC_ENV_SPEED,-3,
	3,PC_PITCH,79,
	3,PC_PITCH,77,
	3,PC_PITCH,72,
	3,PC_PITCH,84,
	3,PC_PITCH,83,
	3,PC_PITCH,79,
	3,PC_PITCH,77,
	3,PC_PITCH,72,
	3,PC_PITCH,84,
	3,PC_PITCH,83,
	3,PC_PITCH,79,
	3,PC_PITCH,77,
	3,PC_PITCH,72,	
	20,PC_NOTE_CUT,0,
	0,PATCH_END
/*
.byte $00,$3f
	.byte $01,$ab
	.byte $02,$01
	.byte $12
	.byte $00,$38
	.byte $12
	.byte $00,$3f
	.byte $01,$3f
	.byte $12
	.byte $00,$38
	.byte $12
	.byte $00,$3f
	.byte $01,$1c
	.byte $12
	.byte $00,$38
	.byte $01,$ab
	.byte $12
	.byte $00,$3f
	.byte $01,$e1
	.byte $02,$00
	.byte $12
	.byte $00,$38
	.byte $01,$3f
	.byte $02,$01
	.byte $12
	.byte $00,$3f
	.byte $01,$d5
	.byte $02,$00
	.byte $12
	.byte $00,$38
	.byte $01,$1c
	.byte $02,$01
	.byte $12
	.byte $00,$3f
	.byte $01,$9f
	.byte $02,$00
	.byte $12
	.byte $00,$38
	.byte $01,$e1
	.byte $12
	.byte $00,$3f
	.byte $01,$8e
	.byte $12
	.byte $00,$38
	.byte $01,$d5
	.byte $12
	.byte $00,$3f
	.byte $01,$70
	.byte $12
	.byte $00,$38
	.byte $01,$9f
	.byte $12
	.byte $00,$3f
	.byte $01,$6a
	.byte $12
	.byte $01,$70
	.byte $12
	.byte $00,$3d
	.byte $01,$8e
	.byte $12
	.byte $01,$9f
	.byte $12
	.byte $00,$3a
	.byte $01,$d5
	.byte $12
	.byte $01,$6a
	.byte $12
	.byte $00,$38
	.byte $01,$70
	.byte $12
	.byte $01,$8e
	.byte $12
	.byte $00,$36
	.byte $01,$9f
	.byte $12
	.byte $01,$d5
	.byte $12
	.byte $00,$34
	.byte $01,$6a
	.byte $12
	.byte $01,$70
	.byte $12
	.byte $00,$32
	.byte $01,$8e
	.byte $12
	.byte $01,$9f
	.byte $12
	.byte $00,$31
	.byte $01,$d5
	.byte $ff

*/
  };


const char patch15[] PROGMEM ={//DUMMY
  0,PATCH_END
  }; 

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
{0,NULL,patch04,0,0},//
{0,NULL,patch05,0,0},//
{0,NULL,patch06,0,0},//
{0,NULL,patch07,0,0},//
{0,NULL,patch08,0,0},//
{0,NULL,patch09,0,0},//
{1,NULL,patch10,0,0},//
{0,NULL,patch11,0,0},//
{0,NULL,patch12,0,0},//
{1,NULL,patch13,0,0},//
{0,NULL,patch14,0,0},//
{0,NULL,patch15,0,0},//
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

