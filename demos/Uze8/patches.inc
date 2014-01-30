const char patch00[] PROGMEM ={
	0,PC_WAVE,4,
	0,PC_ENV_VOL,0x70,
	0,PC_ENV_SPEED,-40,
	0,PC_PITCH,60,
	1,PC_NOTE_CUT,0,
	0,PATCH_END
};

const char patch01[] PROGMEM ={
	0,PC_WAVE,9,
	0,PC_PITCH,65,
	0,PC_ENV_SPEED,-5,
	10,PC_NOTE_CUT,0,
	0,PATCH_END
};

const struct PatchStruct uze8patch[] PROGMEM = {
   {0,NULL,patch00,0,0},
   {0,NULL,patch01,0,0},
};