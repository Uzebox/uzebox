// !SLICE_COUNT! 57

// !PLATZ_OBJECTS!
const object pgmObjects[] PROGMEM = {
	{{1,4},mapStar},
	{{4,6},mapStar},
	{{13,1},mapStar},
	{{27,3},mapCloud},			// Slice 1 (4,6)
	{{3,4},mapStar},
	{{10,6},mapMoon},
	{{12,13},mapStar},
	{{14,2},mapCloud},
	{{17,6},mapStar},
	{{22,1},mapStar},
	{{25,3},mapCloud},
	{{27,13},mapStar},			// Slice 2 (8,14)
	{{10,5},mapStar},
	{{13,2},mapCloud},
	{{22,2},mapStar},			// Slice 3 (3,17)
	{{2,1},mapCloud},
	{{17,2},mapStar},			// Slice 4 (2,19)
	{{7,4},mapFlower},
	{{9,4},mapFlower},
	{{11,4},mapFlower},
	{{13,4},mapFlower},
	{{17,4},mapFlower},
	{{19,4},mapFlower},
	{{21,4},mapFlower},
	{{23,4},mapFlower},			// Slice 5 (8,27)
	{{12,2},mapStar},			// Slice 6 (1,28)
	{{19,3},mapStar},			// Slice 7 (2,30)
	{{3,2},mapStar},
	{{23,2},mapStar},			// Slice 8 (2,32)
	{{4,6},mapFlower},
	{{13,2},mapStar},
	{{25,17},mapPlant},			// Slice 9 (3,35)
	{{6,5},mapStar},
	{{13,21},mapPlant},
	{{28,3},mapStar},			// Slice 10 (3,38)
	{{10,2},mapStar},
	{{16,18},mapPlant},			// Slice 11 (2,40)
	{{15,2},mapStar},
	{{22,3},mapStar},
	{{28,6},mapPlant},
	{{22,16},mapPlant},			// Slice 15 (4,51)
	{{4,1},mapStar},
	{{21,4},mapFlower},
	{{26,1},mapStar},			// Slice 16 (3,54)
	{{15,5},mapPlant},
	{{25,2},mapStar},			// Slice 17 (2,56)
	{{21,1},mapStar},
	{{29,4},mapFlower},			// Slice 21 (2,58)
	{{8,1},mapStar},
	{{11,22},mapPlant},
	{{20,1},mapStar},
	{{26,10},mapStar},			// Slice 22 (4,62)
	{{5,22},mapPlant},
	{{17,10},mapStar},
	{{18,22},mapPlant},
	{{28,12},mapStar},			// Slice 23 (4,66)
	{{2,2},mapStar},
	{{4,12},mapFlower},
	{{5,4},mapCloud},
	{{14,3},mapStar},
	{{23,6},mapStar},
	{{24,12},mapFlower},
	{{25,1},mapCloud},			// Slice 24_ext
	{{3,4},mapStar},
	{{6,4},mapCloud},
	{{13,6},mapStar},
	{{20,13},mapPlant},
	{{25,5},mapStar},
	{{26,2},mapCloud},			// Slice 25_ext
	{{4,8},mapStar},
	{{8,1},mapStar},
	{{11,2},mapCloud},
	{{16,12},mapFlower},
	{{20,12},mapFlower},
	{{29,1},mapStar},
	{{28,5},mapCloud},			// Slice 26_ext
	{{4,2},mapStar},
	{{7,3},mapCloud},
	{{15,4},mapStar},
	{{20,2},mapCloud},
	{{26,2},mapStar},			// Slice 24 (5,71)
	{{9,5},mapStar},
	{{11,3},mapCloud},
	{{26,5},mapStar},			// Slice 25 (3,74)
	{{4,1},mapCloud},
	{{11,10},mapStar},
	{{24,4},mapStar},
	{{27,3},mapCloud},			// Slice 26 (4,78)
	{{3,2},mapStar},
	{{10,6},mapStar},
	{{13,1},mapCloud},
	{{21,3},mapStar},			// Slice 27 (4,82)
	{{3,3},mapStar},
	{{7,1},mapCloud},
	{{20,3},mapStar},			// Slice 28 (3,85)
	{{1,7},mapPlant},
	{{4,2},mapStar},
	{{26,8},mapStar},			// Slice 29 (3,88)
	{{7,9},mapStar},
	{{12,1},mapStar},
	{{30,3},mapStar},			// Slice 30 (3,91)
	{{10,1},mapStar},
	{{26,2},mapStar},			// Slice 32 (2,93)
	{{7,3},mapCloud},
	{{20,4},mapCloud},			// Slice 33
	{{10,3},mapStar},
	{{23,7},mapStar},			// Slice 36_ext
	{{5,2},mapCloud},
	{{12,4},mapStar},
	{{19,12},mapPlant},
	{{22,6},mapStar},
	{{23,1},mapCloud},			// Slice 39_ext
	{{3,4},mapStar},
	{{17,6},mapStar},
	{{19,12},mapStar},
	{{26,2},mapStar},			// Slice 42_ext
	{{3,2},mapStar},
	{{12,1},mapStar},
	{{12,10},mapPlant},
	{{15,4},mapStar},
	{{23,2},mapStar},
	{{26,9},mapFlower},			// Slice 45_ext
	{{5,14},mapStar},
	{{8,3},mapStar},
	{{21,2},mapStar},
	{{22,10},mapPlant},
	{{29,13},mapStar},			// Slice 46_ext
	{{3,3},mapStar},
	{{11,9},mapPlant},
	{{20,2},mapStar},
	{{26,8},mapFlower},
	{{29,5},mapStar},			// Slice 47_ext
	{{2,1},mapStar},
	{{5,8},mapFlower},
	{{13,3},mapStar},
	{{21,8},mapFlower},
	{{27,2},mapStar},			// Slice 48_ext
	{{3,9},mapPlant},
	{{10,2},mapStar},
	{{11,2},mapStar},
	{{12,2},mapStar},
	{{10,4},mapStar},
	{{11,4},mapStar},
	{{14,2},mapStar},
	{{16,2},mapStar},
	{{14,6},mapStar},
	{{16,6},mapStar},
	{{19,3},mapStar},
	{{20,4},mapStar},
	{{22,6},mapStar},
	{{23,9},mapPlant},			// Slice fin
	{{3,8},mapFlower},
	{{12,2},mapStar},
	{{13,2},mapStar},
	{{14,2},mapStar},
	{{12,6},mapStar},
	{{13,6},mapStar},
	{{14,6},mapStar},
	{{14,5},mapStar},
	{{14,4},mapStar},
	{{13,4},mapStar},
	{{17,2},mapStar},
	{{18,2},mapStar},
	{{17,6},mapStar},
	{{18,6},mapStar},
	{{23,8},mapFlower},			// Slice go
	{{2,3},mapStar},
	{{3,8},mapStar},
	{{7,6},mapFlower},
	{{14,6},mapStar},
	{{15,17},mapStar},
	{{27,0},mapStar},
	{{27,4},mapMoon},
	{{27,17},mapStar},
	{{28,8},mapStar},
	{{4,12},mapDashD},
	{{10,12},mapDashA},
	{{16,12},mapDashS},
	{{22,12},mapDashH},			// Slice B.C. dash
	{{3,5},mapHourGlass},
	{{3,10},mapBoots},
	{{3,14},mapEgg},
	{{3,17},mapBuddha},
	{{20,1},mapPogo},
	{{20,5},mapBomb},
	{{20,9},mapPsn},
	{{20,14},mapIcicle},
	{{21,14},mapIcicle},
	{{5,2},mapDucats},
	{{5,6},mapStitchInTime},
	{{5,10},mapSpeedy},
	{{5,14},mapDfly},
	{{5,18},mapFloat},
	{{22,2},mapLeap},
	{{22,6},mapKaboom},
	{{22,10},mapPoison},
	{{22,14},mapTimeFlies},
	{{22,18},mapTimeFlies},		// Slice demo instructions
	{{3,1},mapStar},
	{{4,9},mapFlower},
	{{14,7},mapStar},
	{{21,0},mapStar},
	{{24,9},mapFlower},
	{{27,4},mapStar}			// Slice hi scores
};

PGM_P const pgmMaps[] PROGMEM = {
	mapGrassLt,					// 0
	mapGrassMid,				// 1
	mapGrassRt,					// 2
	mapGndMid0,					// 3
	mapGndBtmLtWater,			// 4
	mapGndBtmRtWater,			// 5
	mapGndBtmWaterGrp2,			// 6
	mapMushroom,				// 7
	mapBomb,					// 8
	mapGndBtmWaterGrp0,			// 9
	mapPsnWater,				// 10
	mapPupLedge,				// 11
	mapBootsWater,				// 12
	mapGndBtmWater,				// 13
	mapLogBridge,				// 14
	mapGndBtmWaterGrp1,			// 15
	mapGndMid3,					// 16
	mapGndMid1,					// 17
	mapPogo,					// 18
	mapGndBtm,					// 19
	mapGndBtmGrp1,				// 20
	mapGndBtmRt,				// 21
	mapGndBtmLt,				// 22
	mapHourGlass,				// 23
	mapBuddha,					// 24
	mapGndMid2,					// 25
	mapBoots,					// 26
	mapGndBtmGrp0,				// 27
	mapGndTopLtWater,			// 28
	mapGndTopRtWater,			// 29
	mapGndWaterPikeDn,			// 30
	mapGndWaterPikeUp,			// 31
	mapPsn,						// 32
	mapGndBtmWaterGrp3,			// 33
	mapGndConeTopWater,			// 34
	mapGndConeBtmWater,			// 35
	mapSnowPlat,				// 36
	mapSnowLt,					// 37
	mapSnowRt,					// 38
	mapSnowMid,					// 39
	mapIceBtm,					// 40
	mapIceBtmLt,				// 41
	mapIceBtmRt,				// 42
	mapIceBtmGrp,				// 43
	mapGndPlat,					// 44
	mapDashD,					// 45
	mapDashA,					// 46
	mapDashS,					// 47
	mapDashH,					// 48
	mapGndBtmGrp2,				// 49
	mapBFront,					// 50
	mapGndBtmGrp3				// 51
};


const bgAnimIndex pgmAnimDir[] PROGMEM = {
	{5,0},{5,1},				// Slice 0 (2,2)	(count,total)
	{6,0},						// Slice 1 (1,3)
	{3,0},{4,0},				// Slice 2 (2,5)
	{1,0},						// Slice 3 (1,6)
	{1,0},{2,0},				// Slice 4 (2,8)
	{2,0},						// Slice 5 (1,9)
	{1,0},						// Slice 6 (1,10)
	{5,0},						// Slice 7 (1,11)
	{1,0},{2,0},				// Slice 8 (2,13)
	{3,0},{4,0},				// Slice 9 (2,15)
	{5,0},{6,0},				// Slice 10 (2,17)
	{2,0},{4,0},{12,0},			// Slice 11 (3,20)
	{2,0},{6,0},				// Slice 15 (2,26)
	{2,0},{17,0},{19,0},		// Slice 17 (3,29)
	{8,0},						// Slice 21 (1,30)
	{2,0},						// Slice 23 (1,31)
	{1,0},						// Slice 23_ext
	{1,0},						// Slice 24_ext
	{1,0},						// Slice 25_ext
	{3,0},{4,0},				// Slice 24 (2,33)
	{2,0},{3,0},{5,0},			// Slice 25 (3,36)
	{1,0},{3,0},				// Slice 26 (2,38)
	{1,0},{2,0},				// Slice 27 (2,40)
	{1,0},{2,0},				// Slice 28 (2,42)
	{2,0},						// Slice 29 (1,43)
	{7,0},{7,1},{8,0},			// Slice 30 (3,46)
	{3,0},{4,0},{4,1},			// Slice 31 (3,49)
	{3,0},{4,0},{4,1},{5,0},	// Slice 32 (4,53)
	{1,0},						// Slice 33 (4,57)
	{1,0},						// Slice 36_ext
	{1,0},{8,0},				// Slice 39_ext
	{1,0},						// Slice 42_ext
	{1,0},						// Slice 45_ext
	{2,0},						// Slice 46_ext
	{0,0},{6,0},{11,0},{13,0},
	{15,0},						// Slice B.C. Dash
	{1,0},{3,0},{4,0},			// Slice demo instructions
	{1,0},{1,1}					// Slice hi scores
};

const platformDirectory pgmPlatformDir[] PROGMEM = {
	{0,1},						// Slice 7
	{1,2},						// Slice 8
	{3,1},						// Slice 10
	{4,2},						// Slice 11
	{6,1},						// Slice 16
	{7,3},						// Slice 19
	{10,1},						// Slice 22
	{11,1},						// Slice 23
	{12,2},						// Slice 30
	{14,2},						// Slice 31
	{16,3},						// Slice 32
	{19,2}						// Slice 46_ext
};

// NB: Platform speeds must be a factor of 32
const platform pgmPlatforms[] PROGMEM = {
	{TILE_SKY|MP_SMOOTH,32,56,AXIS_Y,4,{184,232,48,64}},		// Slice 7
	{TILE_SKY|MP_SMOOTH,24,64,AXIS_X,2,{32,80,48,56}},
	{TILE_SKY|MP_SMOOTH,144,208,AXIS_X,-2,{168,192,48,56}},		// Slice 8
	{TILE_SKY|MP_SMOOTH,32,136,AXIS_X,2,{56,72,64,72}},			// Slice 10
	{TILE_SKY|MP_SMOOTH,64,128,AXIS_Y,2,{64,80,120,128}},
	{TILE_SKY|MP_SMOOTH,112,192,AXIS_X,4,{128,160,40,48}},		// Slice 11
	{TILE_SKY|MP_SMOOTH,32,108,AXIS_Y,2,{64,96,48,56}},			// Slice 16
	{TILE_WATER|MP_STEPPED,24,72,AXIS_Y,2,{48,80,32,128}},
	{TILE_WATER|MP_STEPPED,40,88,AXIS_Y,-2,{120,152,80,176}},
	{TILE_WATER|MP_STEPPED,24,72,AXIS_Y,2,{192,224,56,152}},	// Slice 19
	{TILE_SKY|MP_SMOOTH,56,156,AXIS_Y,-2,{24,56,56,64}},		// Slice 22
	{TILE_SKY|MP_SMOOTH,24,192,AXIS_X,-4,{40,72,56,72}},		// Slice 23
	{TILE_SKY|MP_SMOOTH,56,120,AXIS_Y,2,{88,104,72,80}},
	{TILE_SKY|MP_SMOOTH,136,216,AXIS_X,-4,{192,208,40,56}},	// Slice 30
	{TILE_SKY|MP_SMOOTH,24,152,AXIS_X,-8,{144,168,32,56}},
	{TILE_SKY|MP_SMOOTH,80,208,AXIS_X,-8,{200,224,32,56}},		// Slice 31
	{TILE_SKY|MP_SMOOTH,24,144,AXIS_X,-4,{136,144,40,56}},
	{TILE_SKY|MP_SMOOTH,64,184,AXIS_X,-4,{176,184,40,56}},
	{TILE_SKY|MP_SMOOTH,104,224,AXIS_X,-4,{216,224,40,56}},		// Slice 32
	{TILE_SKY|MP_SMOOTH,24,72,AXIS_Y,2,{120,136,32,48}},
	{TILE_SKY|MP_SMOOTH,64,112,AXIS_Y,2,{120,136,72,88}}		// Slice 46_ext
};

const bgInner pgmBgsInner[] PROGMEM = {
	{BGP,1,{0,9,10,11}},
	{0,TILE_GRASS_RT,{9,10,10,11}},
	{BGP,3,{0,10,11,25}},
	{BGP,0,{9,10,0,2}},
	{BGP,1,{10,26,0,1}},
	{BGP,2,{26,27,0,2}},
	{BGA,0,{10,26,1,16}},
	{BGA,1,{10,26,16,20}},
	{0,TILE_WATER,{10,26,20,25}},
	{0,TILE_WATER,{26,32,23,25}},
	{0,TILE_GRASS_LT,{26,27,10,11}},
	{BGP,1,{27,32,10,11}},
	{BGP,3,{26,32,11,21}},
	{BGP,4,{26,27,21,23}},
	{BGP,6,{27,32,21,23}},					// Slice 0 (15,15)
	{BGP,1,{0,12,10,11}},
	{BGP,3,{0,12,11,21}},
	{BGP,9,{0,12,21,23}},
	{BGP,2,{12,13,10,12}},
	{BGP,1,{12,20,12,13}},
	{BGP,3,{12,20,13,21}},
	{BGP,6,{12,19,21,23}},
	{BGP,5,{19,20,21,23}},
	{BGP,2,{20,21,12,14}},
	{0,TILE_WATER,{0,20,23,25}},
	{BGA|BGM,2,{20,32,17,18}},
	{0x00,MUT_WLK_WATER,{20,32,17,18}},
	{0,TILE_WATER,{20,32,18,25}},
	{BGP,7,{25,31,14,16}},					// Slice 1 (13,28)
	{BGP|BGM,8,{4,6,13,15}},
	{0x01,MUT_BOMB,{4,10,15,17}},
	{BGP|BGM,7,{4,10,15,17}},
	{0x02,MUT_BOMB_TARGET,{4,10,15,17}},
	{BGA|BGM,2,{0,32,17,18}},
	{0x03,MUT_WLK_WATER,{0,32,17,18}},
	{0,TILE_WATER,{0,32,18,25}},
	{BGA|BGM,4,{22,24,10,12}},
	{0x04,MUT_COIN,{22,24,10,12}},
	{BGP,7,{20,26,12,14}},					// Slice 2 (5,33)
	{BGA|BGM,2,{0,32,17,18}},
	{0x05,MUT_WLK_WATER,{0,32,17,18}},
	{0,TILE_WATER,{0,32,18,25}},
	{BGP,7,{4,10,11,13}},
	{BGP,7,{22,28,10,12}},
	{BGP|BGM,10,{20,22,21,23}},
	{0x06,MUT_POISON,{20,22,21,23}},
	{BGP,11,{20,22,23,24}},					// Slice 3 (8,41)
	{BGA|BGM,2,{0,28,17,18}},
	{0x07,MUT_WLK_WATER,{0,28,17,18}},
	{0,TILE_WATER,{0,28,18,25}},
	{BGA|BGM,4,{8,10,7,9}},
	{0x08,MUT_COIN,{8,10,7,9}},
	{BGP,7,{6,12,9,11}},
	{BGP,7,{16,22,7,9}},
	{BGP,0,{27,28,6,8}},
	{0,TILE_WATER,{28,32,19,25}},
	{BGP,1,{28,32,6,7}},
	{BGP,3,{28,32,7,19}},
	{BGP|BGM,3,{28,32,19,25}},
	{0x09,MUT_SECRET_WATER,{28,32,19,25}},		// Slice 4 (16,57)
	{BGP,1,{0,32,6,7}},
	{BGP,3,{0,32,7,18}},
	{BGP,13,{0,32,18,19}},
	{0,TILE_WATER,{0,32,19,25}},
	{BGA|BGM,4,{15,17,4,6}},
	{0x0a,MUT_COIN,{15,17,4,6}},
	{BGP|BGM,12,{5,7,22,23}},
	{0x0b,MUT_BOOTS,{5,7,22,23}},
	{BGP,11,{5,7,23,24}},						// Slice 5 (9,66)
	{BGA|BGM,5,{0,32,12,13}},
	{0x0c,MUT_WLK_WATER,{0,32,12,13}},
	{0,TILE_WATER,{0,32,13,25}},
	{BGP,14,{0,4,6,8}},
	{BGP|BGM,8,{7,9,4,6}},
	{0x0d,MUT_BOMB,{4,12,6,8}},
	{BGP|BGM,14,{4,12,6,8}},
	{0x0e,MUT_BOMB_TARGET,{4,12,6,8}},
	{BGP,14,{12,18,6,8}},
	{BGP|BGM,8,{21,23,4,6}},
	{0x0f,MUT_BOMB,{18,26,6,8}},
	{BGP|BGM,14,{18,26,6,8}},
	{0x10,MUT_BOMB_TARGET,{18,26,6,8}},
	{BGP,14,{26,32,6,8}},						// Slice 6 (14,80)
	{BGP,1,{0,8,6,7}},
	{BGP,3,{0,8,7,15}},
	{BGP,4,{0,1,15,17}},
	{BGP,15,{1,7,15,17}},
	{BGP,5,{7,8,15,17}},
	{0,TILE_GND_WATER_TOP_LT,{0,1,21,22}},
	{BGP,16,{1,7,21,22}},
	{0,TILE_GND_WATER_TOP_RT,{7,8,21,22}},
	{BGP,3,{0,8,22,25}},
	{BGP,14,{8,15,6,8}},
	{0,TILE_WATER,{0,8,17,21}},
	{BGA|BGM,2,{8,32,12,13}},
	{0x11,MUT_WLK_WATER,{8,32,12,13}},
	{0,TILE_WATER,{8,32,13,25}},			// Slice 7 (14,94)
	{BGA|BGM,2,{0,32,12,13}},
	{0x12,MUT_WLK_WATER,{0,32,12,13}},
	{0,TILE_WATER,{0,32,13,25}},
	{BGA|BGM,4,{19,21,4,6}},
	{0x13,MUT_COIN,{19,21,4,6}},
	{BGP,0,{31,32,8,10}},					// Slice 8 (6,100)
	{BGP,1,{0,9,8,9}},
	{BGP,3,{0,9,9,18}},
	{BGP,9,{0,8,18,20}},
	{BGP,5,{8,9,18,20}},
	{0,TILE_WATER,{0,15,20,25}},
	{BGA|BGM,2,{9,15,17,18}},
	{0x14,MUT_WLK_WATER,{9,15,17,18}},
	{0,TILE_WATER,{9,15,18,20}},
	{BGA|BGM,4,{17,19,14,16}},
	{0x15,MUT_COIN,{17,19,14,16}},
	{BGP,1,{15,23,16,17}},
	{BGP,3,{15,23,17,25}},
	{BGP,2,{23,24,16,18}},
	{BGP,1,{23,24,18,19}},
	{BGP,3,{23,24,19,25}},
	{BGP,1,{24,27,18,19}},
	{BGP,17,{24,27,19,25}},
	{BGP,2,{27,28,18,20}},
	{BGP,1,{27,28,20,21}},
	{BGP,17,{27,28,21,25}},
	{BGP,1,{28,32,20,21}},
	{BGP,17,{28,32,21,25}},
	{BGP,14,{9,32,8,10}},					// Slice 9 (23,123)
	{BGP,1,{0,3,20,21}},
	{BGP,17,{0,3,21,25}},
	{BGP,2,{3,4,20,22}},
	{BGP,1,{3,4,22,23}},
	{BGP,3,{3,4,23,25}},
	{BGP,1,{4,17,22,23}},
	{BGP,3,{4,17,23,25}},
	{BGP|BGM,18,{10,11,20,22}},
	{0x16,MUT_POGO,{10,11,20,22}},
	{BGP,0,{16,17,14,16}},
	{BGP,1,{17,24,14,15}},
	{BGP,3,{17,24,15,25}},
	{BGA|BGM,4,{14,16,3,5}},
	{0x17,MUT_COIN,{14,16,3,5}},
	{BGA|BGM,4,{21,23,12,14}},
	{0x18,MUT_COIN,{21,23,12,14}},
	{BGP,0,{23,24,8,10}},
	{BGP,1,{24,26,8,9}},
	{BGP,17,{24,26,9,25}},
	{BGM,TILE_EGG,{27,28,18,19}},
	{0x19,MUT_EGG,{27,28,18,19}},
	{BGP,1,{26,32,8,9}},
	{BGP,17,{26,32,9,12}},
	{BGP,19,{26,32,12,13}},
	{BGP,17,{26,32,19,25}},					// Slice 10 (24,147)
	{BGP,17,{0,4,9,11}},
	{BGP,20,{0,3,11,13}},
	{BGP,21,{3,4,11,13}},
	{BGP,17,{0,4,19,25}},
	{BGA|BGM,3,{0,4,7,9}},
	{0x1a,MUT_FIRE,{0,4,7,9}},
	{BGA|BGM,5,{4,14,20,21}},
	{0x1b,MUT_WLK_WATER,{4,14,20,21}},
	{0,TILE_WATER,{4,14,21,25}},
	{BGP|BGM,10,{4,6,21,23}},
	{0x1c,MUT_POISON,{4,6,21,23}},
	{BGP,11,{4,6,23,24}},
	{BGP,3,{14,28,19,21}},
	{BGP|BGM,23,{22,24,17,19}},
	{0x1d,MUT_HOURGLASS,{22,24,17,19}},
	{BGP|BGM,24,{25,27,17,19}},
	{0x1e,MUT_BUDDHA,{25,27,17,19}},
	{0,TILE_WATER,{14,32,21,25}},
	{BGP,22,{14,15,8,10}},
	{BGP,27,{15,32,8,10}},
	{BGA|BGM,3,{14,32,6,8}},
	{0x1f,MUT_FIRE,{14,32,6,8}},
	{BGP,3,{28,32,14,21}},			// Slice 11 (25,172)
	{BGP,3,{0,2,14,21}},
	{BGP,1,{0,32,7,8}},
	{BGP,27,{0,32,8,10}},
	{BGA|BGM,3,{2,8,15,17}},
	{0x20,MUT_FIRE,{2,8,15,17}},
	{BGP,3,{2,12,17,21}},
	{BGP,3,{12,13,17,19}},
	{BGP,5,{12,13,19,21}},
	{0,TILE_WATER,{0,18,21,25}},
	{BGA|BGM,5,{13,18,17,18}},
	{0x21,MUT_WLK_WATER,{13,18,17,18}},
	{0,TILE_WATER,{13,18,18,21}},
	{BGP,3,{18,32,17,19}},
	{0,TILE_WATER,{18,32,19,25}},
	{BGP|BGM,3,{18,32,19,25}},
	{0x22,MUT_SECRET_WATER,{18,32,19,25}},	// Slice 15 (16,236)
	{BGP,1,{0,2,7,8}},
	{0,TILE_GRASS_RT,{2,3,7,8}},
	{BGP,27,{0,2,8,10}},
	{BGP,21,{2,3,8,10}},
	{BGP,3,{0,16,17,19}},
	{0,TILE_WATER,{0,32,19,25}},
	{BGP,30,{10,13,19,22}},
	{BGP|BGM,24,{14,16,15,17}},
	{0x23,MUT_BUDDHA,{14,16,15,17}},
	{BGP,0,{15,16,6,8}},
	{BGP,31,{20,23,22,25}},
	{BGP,30,{29,32,19,22}},
	{BGP,1,{16,32,6,7}},
	{BGP,3,{16,32,7,19}},					// Slice 16 (17,253)
	{BGP,1,{0,5,6,7}},
	{BGP,3,{0,5,7,19}},
	{BGA|BGM,5,{5,14,8,9}},
	{0x24,MUT_WLK_WATER,{5,14,8,9}},
	{BGP|BGM,8,{7,9,4,6}},
	{0x25,MUT_BOMB,{5,14,6,9}},
	{BGP|BGM,16,{5,14,6,9}},
	{0x26,MUT_BOMB_TARGET_SWITCH,{0x04,0x04,0,0}},
	{0,TILE_WATER,{5,14,9,17}},
	{BGP|BGM,10,{12,14,15,17}},
	{0x27,MUT_POISON,{12,14,15,17}},
	{BGM,TILE_EGG_WATER,{10,11,16,17}},
	{0x28,MUT_EGG,{10,11,16,17}},
	{0,TILE_WATER,{0,32,19,25}},
	{BGP|BGM,10,{5,7,22,24}},
	{0x29,MUT_POISON,{5,7,22,24}},
	{BGP,11,{5,7,24,25}},
	{BGP,30,{13,16,19,22}},
	{BGP,31,{23,26,22,25}},
	{BGP,1,{14,16,6,7}},
	{BGP,17,{14,16,7,17}},
	{BGP,2,{16,17,6,8}},
	{BGP|BGM,32,{21,23,4,6}},
	{0x2a,MUT_POISON,{21,23,4,6}},
	{BGP,0,{21,22,6,8}},
	{BGP,2,{22,23,6,8}},
	{BGA|BGM,4,{25,27,6,8}},
	{0x2b,MUT_COIN,{25,27,6,8}},
	{BGA|BGM,5,{16,28,8,9}},
	{0x2c,MUT_WLK_WATER,{16,28,8,9}},
	{0,TILE_WATER,{16,28,9,17}},
	{0,TILE_WATER,{28,32,12,17}},
	{BGP,0,{27,28,0,2}},
	{BGP,17,{5,32,17,19}},
	{BGP,1,{28,32,0,1}},
	{BGP,3,{28,32,1,10}},
	{BGP,4,{28,29,10,12}},
	{BGP,15,{29,32,10,12}},					// Slice 17 (38,291) +1
	{BGP,1,{0,32,0,1}},
	{BGP,9,{0,32,1,3}},
	{BGP,3,{0,32,23,25}},
	{0,TILE_WATER,{0,32,3,23}},
	{BGP,3,{4,6,2,9}},
	{BGP,35,{4,6,9,11}},
	{BGP,3,{10,12,2,9}},
	{BGP,35,{10,12,9,11}},
	{BGP,3,{13,15,17,23}},
	{BGP,34,{13,15,15,17}},
	{BGP,3,{19,21,17,23}},
	{BGP,34,{19,21,15,17}},
	{BGP,3,{22,24,2,9}},
	{BGP,35,{22,24,9,11}},
	{BGP,3,{28,30,2,9}},
	{BGP,35,{28,30,9,11}},					// Slice 19 (16,360)
	{0,TILE_WATER,{0,16,0,18}},
	{BGP,1,{0,4,0,1}},
	{BGP,9,{0,4,1,3}},
	{BGP,28,{0,1,18,20}},
	{BGP,3,{0,1,20,25}},
	{BGP,25,{1,14,18,25}},
	{BGP,1,{4,9,0,1}},
	{BGP,3,{4,9,1,3}},
	{BGP,4,{4,5,3,5}},
	{BGP,15,{5,9,3,5}},
	{BGP,1,{9,14,0,1}},
	{BGP,25,{9,14,1,5}},
	{BGP,4,{9,10,5,7}},
	{BGP,9,{10,14,5,7}},
	{BGP,1,{14,16,0,1}},
	{BGP,3,{14,16,1,9}},
	{BGP,35,{14,16,9,11}},
	{BGP,2,{16,17,0,2}},
	{BGA|BGM,5,{16,22,8,9}},
	{0x2d,MUT_WLK_WATER,{16,22,8,9}},
	{0,TILE_WATER,{16,22,9,14}},
	{BGP,0,{21,22,6,8}},
	{BGP,28,{14,15,14,16}},
	{BGP,3,{14,15,16,25}},
	{BGP,17,{15,22,14,25}},
	{BGP,1,{22,32,6,7}},
	{BGP,3,{22,32,7,25}},					// Slice 21 (27,405)
	{BGP,1,{0,2,6,7}},
	{BGP,3,{0,2,7,25}},
	{0,TILE_GRASS_RT,{2,3,6,7}},
	{BGP,3,{2,3,7,25}},
	{BGP,7,{17,23,7,9}},
	{BGP,17,{3,32,23,25}},					// Slice 22 (7,412)
	{BGP,3,{0,27,23,25}},
	{BGP,7,{9,15,14,16}},
	{BGA|BGM,4,{21,23,2,4}},
	{0x2e,MUT_COIN,{21,23,2,4}},
	{BGP,0,{26,27,14,16}},
	{BGP,1,{27,32,14,15}},
	{BGP,17,{27,32,15,25}},					// Slice 23 (7,419)
	{BGP,1,{0,32,14,15}},
	{BGP,3,{0,32,15,25}},
	{BGA|BGM,4,{14,16,7,9}},
	{0x2f,MUT_COIN,{14,16,7,9}},
	{BGP|BGM,18,{14,15,12,14}},
	{0x30,MUT_POGO,{14,15,12,14}},			// Slice 24_ext
	{BGP,1,{0,32,14,15}},
	{BGP,3,{0,32,15,25}},
	{BGA|BGM,4,{15,17,3,5}},
	{0x31,MUT_COIN,{15,17,3,5}},			// Slice 25_ext
	{BGP,1,{0,32,14,15}},
	{BGP,3,{0,32,15,25}},
	{BGA|BGM,4,{21,23,3,5}},
	{0x32,MUT_COIN,{21,23,3,5}},			// Slice 26_ext
	{BGP,1,{0,6,14,15}},
	{BGP,3,{0,6,15,25}},
	{BGP,2,{6,7,14,16}},
	{BGP,7,{15,21,13,15}},
	{BGA|BGM,4,{25,27,9,11}},
	{0x33,MUT_COIN,{25,27,9,11}},
	{BGA|BGM,2,{6,32,16,17}},
	{0x34,MUT_WLK_WATER,{6,32,16,17}},
	{0,TILE_WATER,{6,32,17,25}},			// Slice 24 (9,428)
	{BGP,7,{0,6,13,15}},
	{BGA|BGM,2,{0,32,16,17}},
	{0x35,MUT_WLK_WATER,{0,32,16,17}},
	{0,TILE_WATER,{0,32,17,25}},
	{BGA|BGM,4,{11,13,9,11}},
	{0x36,MUT_COIN,{11,13,9,11}},
	{BGP,7,{18,24,13,15}},
	{BGA|BGM,4,{29,31,9,11}},
	{0x37,MUT_COIN,{29,31,9,11}},			// Slice 25 (9,437)
	{BGA|BGM,2,{0,32,16,17}},
	{0x38,MUT_WLK_WATER,{0,32,17,25}},
	{0,TILE_WATER,{0,32,17,25}},
	{BGP,7,{4,10,13,15}},
	{BGA|BGM,4,{16,18,9,11}},
	{0x39,MUT_COIN,{16,18,9,11}},
	{BGP,7,{22,28,13,15}},					// Slice 26 (7,444)
	{BGA|BGM,2,{0,32,16,17}},
	{0x3a,MUT_WLK_WATER,{0,32,16,17}},
	{0,TILE_WATER,{0,32,17,25}},
	{BGA|BGM,4,{4,6,10,12}},
	{0x3b,MUT_COIN,{4,6,10,12}},
	{BGP,36,{3,7,12,14}},
	{BGP,36,{15,19,11,13}},
	{BGP,36,{27,31,10,12}},					// Slice 27 (8,452)
	{BGA|BGM,2,{0,23,16,17}},
	{0x3c,MUT_WLK_WATER,{0,23,16,17}},
	{0,TILE_WATER,{0,23,17,25}},
	{BGA|BGM,4,{8,10,7,9}},
	{0x3d,MUT_COIN,{8,10,7,9}},
	{BGP,36,{7,11,9,11}},
	{BGP|BGM,8,{12,14,7,9}},
	{0x3e,MUT_BOMB,{11,15,9,11}},
	{BGP|BGM,36,{11,15,9,11}},
	{0x3f,MUT_BOMB_TARGET,{11,15,9,11}},
	{BGP|BGM,10,{15,17,20,22}},
	{0x40,MUT_POISON,{15,17,20,22}},
	{BGP,11,{15,17,22,23}},
	{BGP,37,{22,23,8,10}},
	{BGP|BGM,26,{27,29,7,8}},
	{0x41,MUT_BOOTS,{27,29,7,8}},
	{BGP,39,{23,32,8,9}},
	{0,TILE_ICE,{23,32,9,25}},				// Slice 28 (18,470)
	{BGP,39,{0,7,8,9}},
	{0,TILE_ICE,{0,7,9,25}},
	{BGP,38,{7,8,8,10}},
	{BGA|BGM,4,{8,10,11,13}},
	{0x42,MUT_COIN,{8,10,11,13}},
	{BGM,TILE_ICICLE,{13,18,12,13}},
	{0x43,MUT_FIRE,{13,18,12,13}},
	{BGP,39,{7,23,13,14}},
	{0,TILE_ICE,{7,23,14,25}},
	{BGP,38,{23,24,13,15}},
	{BGP|BGM,23,{20,22,3,5}},
	{0x44,MUT_HOURGLASS,{20,22,3,5}},
	{BGP,36,{19,23,5,7}},
	{BGP|BGM,32,{28,30,15,17}},
	{0x45,MUT_POISON,{28,30,15,17}},
	{BGP,39,{23,32,17,18}},
	{0,TILE_ICE,{23,32,18,25}},
	{BGP,37,{30,31,0,2}},
	{BGP,39,{31,32,0,1}},
	{0,TILE_ICE,{31,32,1,5}},
	{BGP,41,{31,32,5,7}},					// Slice 29 (19,489)
	{BGP,39,{0,17,0,1}},
	{0,TILE_ICE,{0,17,1,6}},
	{BGP,40,{0,17,6,7}},
	{BGP,39,{0,3,17,18}},
	{0,TILE_ICE,{0,3,18,25}},
	{BGP,37,{2,3,15,17}},
	{BGP,39,{3,16,15,16}},
	{BGP,38,{16,17,15,17}},
	{0,TILE_ICE,{3,16,16,17}},
	{0,TILE_ICE,{3,16,17,25}},
	{BGP,39,{16,32,17,25}},
	{0,TILE_ICE,{16,32,18,25}},
	{BGP,39,{17,32,0,1}},
	{0,TILE_ICE,{17,32,1,8}},
	{BGP,41,{17,18,7,9}},
	{BGP,40,{18,31,8,9}},
	{BGP,42,{31,32,7,9}},					// Slice 29_ext
	{BGP,38,{0,1,0,2}},
	{BGP,39,{0,6,17,18}},
	{0,TILE_ICE,{0,6,18,25}},
	{BGP,38,{6,7,17,19}},
	{BGP|BGM,24,{5,7,3,5}},
	{0x46,MUT_BUDDHA,{5,7,3,5}},
	{BGP,36,{4,8,5,7}},
	{BGP,37,{17,18,7,9}},
	{BGA,0,{18,32,8,19}},
	{BGA,1,{18,32,19,23}},
	{0,TILE_WATER,{18,32,23,25}},
	{BGA|BGM,5,{6,18,19,20}},
	{0x47,MUT_WLK_WATER,{6,18,19,20}},
	{0,TILE_WATER,{6,18,20,25}},
	{BGP,39,{18,32,7,8}},
	{BGM,0,{16,32,19,20}},
	{0x48,MUT_WLK_WATER,{16,32,19,20}},		// Slice 30 (17,505)
	{BGP,39,{0,32,7,8}},
	{BGM,0,{0,32,19,20}},
	{0x49,MUT_WLK_WATER,{0,32,19,20}},
	{BGA|BGM,4,{3,5,2,4}},
	{0x4a,MUT_COIN,{3,5,2,4}},
	{BGA,0,{0,32,8,19}},
	{BGA,1,{0,32,19,23}},
	{0,TILE_WATER,{0,32,23,25}},			// Slice 31 (8,510)
	{BGP,39,{0,32,7,8}},
	{BGM,0,{0,32,19,20}},
	{0x4b,MUT_WLK_WATER,{0,32,19,20}},
	{BGA|BGM,4,{3,5,2,4}},
	{0x4c,MUT_COIN,{3,5,2,4}},
	{BGA,0,{0,30,8,19}},
	{BGA,1,{0,30,19,23}},
	{0,TILE_WATER,{0,30,23,25}},
	{BGA,2,{30,32,19,20}},
	{0,TILE_WATER,{30,32,20,25}},
	{BGP,38,{30,31,7,9}},					// Slice 32 (11,521)
	{BGA|BGM,2,{0,32,19,20}},
	{0x4d,MUT_WLK_WATER,{0,32,19,20}},
	{0,TILE_WATER,{0,32,20,25}},
	{BGP,36,{5,9,14,16}},
	{BGP,36,{21,25,14,16}},					// Slice 33
	{BGA|BGM,2,{0,32,19,20}},
	{0x4e,MUT_WLK_WATER,{0,32,19,20}},
	{0,TILE_WATER,{0,32,20,25}},
	{BGP,36,{4,8,14,16}},
	{BGP,36,{15,19,14,16}},
	{BGP,36,{26,30,14,16}},					// Slice 36_ext
	{BGA|BGM,2,{0,7,19,20}},
	{0x4f,MUT_WLK_WATER,{0,7,19,20}},
	{0,TILE_WATER,{0,7,20,25}},
	{BGP,36,{3,7,16,18}},
	{BGP,37,{6,7,13,15}},
	{BGP,39,{7,27,13,14}},
	{0,TILE_ICE,{7,27,14,25}},
	{BGM,TILE_ICICLE,{12,17,12,13}},
	{0x50,MUT_FIRE,{12,17,12,13}},
	{BGP,38,{27,28,13,15}},
	{BGA|BGM,2,{27,32,19,20}},
	{0x51,MUT_WLK_WATER,{27,32,19,20}},
	{0,TILE_WATER,{27,32,20,25}},			// Slice 39_ext
	{BGA|BGM,2,{0,32,19,20}},
	{0x52,MUT_WLK_WATER,{0,32,19,20}},
	{0,TILE_WATER,{0,32,20,25}},
	{BGP,7,{8,14,11,13}},
	{BGP,7,{23,29,11,13}},					// Slice 42_ext
	{BGA|BGM,2,{0,5,19,20}},
	{0x53,MUT_WLK_WATER,{0,5,19,20}},
	{0,TILE_WATER,{0,5,20,25}},
	{BGP,0,{4,5,11,13}},
	{BGP,1,{5,32,11,12}},
	{BGP,3,{5,32,12,25}},					// Slice 45_ext
	{BGP,2,{0,1,11,13}},
	{BGA|BGM,2,{0,32,19,20}},
	{0x54,MUT_WLK_WATER,{0,32,19,20}},
	{0,TILE_WATER,{0,32,20,25}},
	{BGP,44,{7,11,11,13}},
	{BGP,44,{21,25,11,13}},
	{BGP,0,{31,32,10,12}},					// Slice 46_ext
	{BGP,1,{0,32,10,11}},
	{BGP,3,{0,32,11,25}},					// Slice 47_ext
	{BGP,1,{0,32,10,11}},
	{BGP,3,{0,32,11,25}},					// Slice 48_ext
	{BGP,1,{0,32,10,11}},
	{BGP,3,{0,32,11,25}},
	{0,TILE_STAR,{9,10,2,7}},
	{0,TILE_STAR,{15,16,2,7}},
	{0,TILE_STAR,{18,19,2,7}},
	{0,TILE_STAR,{21,22,2,7}},
	{0,TILE_STAR,{28,30,0,10}},				// Slice fin
	{BGP,1,{0,32,10,11}},
	{BGP,3,{0,32,11,25}},
	{0,TILE_STAR,{11,12,2,7}},
	{0,TILE_STAR,{16,17,2,7}},
	{0,TILE_STAR,{19,20,2,7}},
	{0,TILE_STAR,{28,30,0,10}},				// Slice go
	{BGA,5,{0,32,21,22}},
	{0,TILE_WATER,{0,32,22,25}},
	{BGP,0,{4,5,0,2}},
	{BGP,1,{5,10,0,1}},
	{BGP,3,{5,11,1,2}},
	{BGP,25,{5,7,2,8}},
	{BGP,3,{5,7,8,9}},
	{BGP,1,{7,10,8,9}},
	{BGP,49,{5,10,9,11}},
	{BGP,50,{10,12,0,11}},
	{BGP,3,{7,10,5,6}},
	{BGA,4,{13,15,9,11}},
	{0,TILE_GRASS_LT,{17,18,0,1}},
	{BGP,1,{18,24,0,1}},
	{0,TILE_GRASS_RT,{24,25,0,1}},
	{BGP,3,{17,25,1,2}},
	{BGP,16,{17,19,2,9}},
	{BGP,22,{17,18,9,11}},
	{BGP,51,{18,24,9,11}},
	{BGP,21,{24,25,9,11}},
	{0,TILE_GRASS_LT,{23,25,7,8}},
	{0,TILE_GRASS_RT,{23,25,7,8}},
	{BGP,3,{23,25,8,9}},
	{BGA,3,{19,23,7,9}},
	{BGP,22,{23,24,2,4}},
	{BGP,21,{24,25,2,4}},
	{BGA,4,{26,28,9,11}},					// B.C.
	{BGP,18,{19,20,11,12}},					// dash
	{BGA,6,{8,22,20,21}},					// Slice B.C. Dash
	{BGA|BGM,5,{0,10,19,20}},
	{0x55,MUT_WLK_WATER,{0,10,19,20}},
	{0,TILE_WATER,{0,10,20,25}},
	{0,TILE_GRASS_LT,{10,11,19,20}},
	{BGP,1,{11,32,19,20}},
	{BGP,3,{10,32,20,25}},
	{BGA,4,{3,5,1,3}},
	{BGA,3,{20,22,17,19}},					// Slice demo instructions
	{BGP,1,{0,32,11,12}},
	{BGP,13,{0,32,12,13}},
	{BGA,0,{0,32,13,21}},
	{BGA,1,{0,32,21,25}},
	{BGP,7,{12,18,2,4}}						// Slice hi scores
};

// !PLATZ_OUTER_BGS!
const bgOuter pgmBgsOuter[] PROGMEM = {
	{BGC,3,0x0000,{0,10,10,25}},
	{BGT|BGI,ORI_LRUD,TRIG_WFALL,{0,3,0,10}},
	{BGC,1,0x0003,{9,10,0,2}},
	{BGC,1,0x0004,{10,26,0,1}},
	{BGC,1,0x0005,{26,27,0,2}},
	{0,3,0x0006,{10,26,1,25}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{10,26,17,20}},
	{0,1,0x0009,{26,32,23,25}},
	{BGC,5,0x000a,{26,32,10,23}},							// Slice 0 (9,9) +1
	{BGT|BGI,ORI_RLDU,TRIG_WFALL,{1,4,0,25}},
	{BGC,3,0x000f,{0,12,10,23}},
	{BGC,1,0x0012,{12,13,10,12}},
	{BGC,4,0x0013,{12,20,12,23}},
	{BGC,1,0x0017,{20,21,12,14}},
	{0,1,0x0018,{0,20,23,25}},
	{BGM,3,0x0019,{20,32,17,25}},
	{BGC|BGQ,1,0x001c,{25,31,14,16}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{20,32,17,20}},			// Slice 1 (9,17) +1
	{BGM,2,0x001d,{4,6,13,15}},
	{BGC|BGQ|BGM,2,0x001f,{4,10,15,17}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,17,20}},
	{BGM,3,0x0021,{0,32,17,25}},
	{BGM,2,0x0024,{22,24,10,12}},
	{BGC|BGQ,1,0x0026,{20,26,12,14}},							// Slice 2 (5,21)
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,17,20}},
	{BGM,3,0x0027,{0,32,17,25}},
	{BGC|BGQ,1,0x002a,{4,10,11,13}},
	{BGC|BGQ,1,0x002b,{22,28,10,12}},
	{BGM,2,0x002c,{20,22,21,23}},
	{BGC,1,0x002e,{20,22,23,24}},							// Slice 3 (6,27)
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,28,17,20}},
	{BGM,3,0x002f,{0,28,17,25}},
	{BGM,2,0x0032,{8,10,7,9}},
	{BGC|BGQ,1,0x0034,{6,12,9,11}},
	{BGC|BGQ,1,0x0035,{16,22,7,9}},
	{BGC,1,0x0036,{27,28,6,8}},
	{0,1,0x0037,{28,32,19,25}},
	{BGC,2,0x0038,{28,32,6,19}},
	{BGM,2,0x003a,{28,32,19,25}},							// Slice 4 (11,38) -2
	{BGC,3,0x003c,{0,32,6,19}},
	{0,1,0x003f,{0,32,19,25}},
	{BGM,2,0x0040,{15,17,4,6}},
	{BGM,2,0x0042,{5,7,22,23}},
	{BGC,1,0x0044,{5,7,23,24}},								// Slice 5 (5,43)
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,12,15}},
	{BGM,3,0x0045,{0,32,12,25}},
	{BGC,1,0x0048,{0,4,6,8}},
	{BGM,2,0x0049,{7,9,4,6}},
	{BGM,2,0x004b,{4,12,6,8}},
	{BGC,1,0x004d,{12,18,6,8}},
	{BGM,2,0x004e,{21,23,4,6}},
	{BGM,2,0x0050,{18,26,6,8}},
	{BGC,1,0x0052,{26,32,6,8}},								// Slice 6 (9,52)
	{BGC,5,0x0053,{0,8,6,17}},
	{BGC,4,0x0058,{0,8,21,25}},
	{BGC,1,0x005c,{8,15,6,8}},
	{0,1,0x005d,{0,8,17,21}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{8,32,12,15}},
	{BGM,3,0x005e,{8,32,12,25}},							// Slice 7 (6,58)
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,12,15}},
	{BGM,3,0x0061,{0,32,12,25}},
	{BGM,2,0x0064,{19,21,4,6}},
	{BGC,1,0x0066,{31,32,8,10}},							// Slice 8 (4,62)
	{BGC,4,0x0067,{0,9,8,20}},
	{0,1,0x006b,{0,15,20,25}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{9,15,17,20}},
	{BGM,3,0x006c,{9,15,17,20}},
	{BGM,2,0x006f,{17,19,14,16}},
	{BGC,5,0x0071,{15,24,16,25}},
	{BGC,5,0x0076,{24,28,18,25}},
	{BGC,2,0x007b,{28,32,20,25}},
	{BGC,1,0x007d,{9,32,8,10}},								// Slice 9 (9,70)
	{BGC,5,0x007e,{0,4,20,25}},
	{BGC,2,0x0083,{4,17,22,25}},
	{BGM,2,0x0085,{10,11,20,22}},
	{BGC,1,0x0087,{16,17,14,16}},
	{BGC,2,0x0088,{17,24,14,25}},
	{BGM,2,0x008a,{14,16,3,5}},
	{BGM,2,0x008c,{21,23,12,14}},
	{BGC,1,0x008e,{23,24,8,10}},
	{BGC,2,0x008f,{24,26,8,25}},
	{BGM,2,0x0091,{27,28,18,19}},
	{BGC,3,0x0093,{26,32,8,13}},
	{BGC,1,0x0096,{26,32,19,25}},							// Slice 10 (12,83) -1
	{BGC,3,0x0097,{0,4,9,13}},
	{BGC,1,0x009a,{0,4,19,25}},
	{BGM,2,0x009b,{0,4,7,9}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{4,14,19,22}},
	{BGM,3,0x009d,{4,14,20,25}},
	{BGM,2,0x00a0,{4,6,21,23}},
	{BGC,1,0x00a2,{4,6,23,24}},
	{BGC,1,0x00a3,{14,28,19,21}},
	{BGM,2,0x00a4,{22,24,17,19}},
	{BGM,2,0x00a6,{25,27,17,19}},
	{0,1,0x00a8,{14,32,21,25}},
	{BGC,2,0x00a9,{14,32,8,10}},
	{BGM,2,0x00ab,{14,32,6,8}},
	{BGC,1,0x00ad,{28,32,14,21}},							// Slice 11 (11,95) -1
	{BGC,1,0x00ae,{0,2,14,21}},
	{BGC,2,0x00af,{0,32,7,10}},
	{BGM,2,0x00b1,{2,8,15,17}},
	{BGC,3,0x00b3,{2,13,17,21}},
	{0,1,0x00b6,{0,18,21,25}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{13,18,16,19}},
	{BGM,3,0x00b7,{13,18,17,21}},
	{0,1,0x00ba,{18,32,19,25}},
	{BGC,1,0x00bb,{18,32,17,19}},
	{BGM,2,0x00bc,{18,32,19,25}},							// Slice 15 (10,134)
	{BGC,4,0x00be,{0,3,7,10}},
	{BGC,1,0x00c2,{0,16,17,19}},
	{0,1,0x00c3,{0,32,19,25}},
	{BGC,1,0x00c4,{10,13,19,22}},
	{BGM,2,0x00c5,{14,16,15,17}},
	{BGC,1,0x00c7,{15,16,6,8}},
	{BGC,1,0x00c8,{20,23,22,25}},
	{BGC,1,0x00c9,{29,32,19,22}},
	{BGC,2,0x00ca,{16,32,6,19}},							// Slice 16 (9,143)
	{BGC,2,0x00cc,{0,5,6,19}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{5,14,7,10}},
	{BGM,2,0x00ce,{5,14,8,9}},
	{BGM,2,0x00d0,{7,9,4,6}},
	{BGM,2,0x00d2,{5,14,6,9}},
	{0,1,0x00d4,{5,14,9,17}},
	{BGM,2,0x00d5,{12,14,15,17}},
	{BGM,2,0x00d7,{10,11,16,17}},
	{0,1,0x00d9,{0,32,19,25}},
	{BGM,2,0x00da,{5,7,22,24}},
	{BGC,1,0x00dc,{5,7,24,25}},
	{BGC,1,0x00dd,{13,16,19,22}},
	{BGC,1,0x00de,{23,26,22,25}},
	{BGC,2,0x00df,{14,16,6,17}},
	{BGC,1,0x00e1,{16,17,6,8}},
	{BGM,2,0x00e2,{21,23,4,6}},
	{BGC,2,0x00e4,{21,23,6,8}},
	{BGM,2,0x00e6,{25,27,4,6}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{16,28,7,10}},
	{BGM,3,0x00e8,{16,28,8,17}},
	{0,1,0x00eb,{28,32,12,17}},
	{BGC,1,0x00ec,{27,28,0,2}},
	{BGC,1,0x00ed,{5,32,17,19}},
	{BGC,4,0x00ee,{28,32,0,12}},							// Slice 17 (24,167)
	{BGC,2,0x00f2,{0,32,0,3}},
	{BGC,1,0x00f4,{0,32,23,25}},
	{0,1,0x00f5,{0,32,3,23}},
	{BGC,2,0x00f6,{4,6,2,11}},
	{BGC,2,0x00f8,{10,12,2,11}},
	{BGC,2,0x00fa,{13,15,15,23}},
	{BGC,2,0x00fc,{19,21,15,23}},
	{BGC,2,0x00fe,{22,24,2,11}},
	{BGC,2,0x0100,{28,30,2,11}},							// Slice 19 (9,196)
	{0,1,0x0102,{0,16,0,18}},
	{BGC,2,0x0103,{0,4,0,3}},
	{BGC,3,0x0105,{0,14,18,25}},
	{BGC,4,0x0108,{4,9,0,5}},
	{BGC,4,0x010c,{9,14,0,7}},
	{BGC,3,0x0110,{14,16,0,11}},
	{BGC,1,0x0113,{16,17,0,2}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{16,22,7,10}},
	{BGM,3,0x0114,{16,22,8,14}},
	{BGC,1,0x0117,{21,22,6,8}},
	{BGC,3,0x0118,{14,22,14,25}},
	{BGT|BGI,ORI_LRUD,TRIG_CHECK_1,{23,26,0,6}},
	{BGC,2,0x011b,{22,32,6,25}},							// Slice 21 (13,218) +1
	{BGC,4,0x011d,{0,3,6,25}},
	{BGC,1,0x0121,{17,23,7,9}},
	{BGC,1,0x0122,{3,32,23,25}},							// Slice 22 (3,221)
	{BGC,1,0x0123,{0,27,23,25}},
	{BGC,1,0x0124,{9,15,14,16}},
	{BGM,2,0x0125,{21,23,2,4}},
	{BGC,1,0x0127,{26,27,14,16}},
	{BGC,2,0x0128,{27,32,14,25}},							// Slice 23 (4,225)
	{BGC,2,0x012a,{0,32,14,25}},
	{BGM,2,0x012c,{14,16,7,9}},
	{BGM,2,0x012e,{14,15,12,14}},							// Slice 24_ext
	{BGC,2,0x0130,{0,32,14,25}},
	{BGM,2,0x0132,{15,17,3,5}},								// Slice 25_ext
	{BGC,2,0x0134,{0,32,14,25}},
	{BGM,2,0x0136,{21,23,3,5}},								// Slice 26_ext
	{BGC,2,0x0138,{0,6,14,25}},
	{BGC,1,0x013a,{6,7,14,16}},
	{BGC|BGQ,1,0x013b,{15,21,13,15}},
	{BGM,2,0x013c,{25,27,9,11}},
	{BGM,3,0x013e,{6,32,16,25}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{6,32,15,18}},				// Slice 24 (6,231)
	{BGC|BGQ,1,0x0141,{0,6,13,15}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,15,18}},
	{BGM,3,0x0142,{0,32,16,25}},
	{BGM,2,0x0145,{11,13,9,11}},
	{BGC|BGQ,1,0x0147,{18,24,13,15}},
	{BGM,2,0x0148,{29,31,9,11}},							// Slice 25 (6,237)
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,15,18}},
	{BGM,3,0x014a,{0,32,16,25}},
	{BGC|BGQ,1,0x014d,{4,10,13,15}},
	{BGM,2,0x014e,{16,18,9,11}},
	{BGC|BGQ,1,0x0150,{22,28,13,15}},						// Slice 26 (5,242)
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,15,18}},
	{BGM,3,0x0151,{0,32,16,25}},
	{BGM,2,0x0154,{4,6,10,12}},
	{BGC|BGQ,1,0x0156,{3,7,12,14}},
	{BGC|BGQ,1,0x0157,{15,19,11,13}},
	{BGC|BGQ,1,0x0158,{27,31,10,12}},						// Slice 27 (6,248)
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,23,15,18}},
	{BGM,3,0x0159,{0,23,16,25}},
	{BGM,2,0x015c,{8,10,7,9}},
	{BGC|BGQ,1,0x015e,{7,11,9,11}},
	{BGM,2,0x015f,{12,14,7,9}},
	{BGM|BGQ,2,0x0161,{11,15,9,11}},
	{BGM,2,0x0163,{15,17,20,22}},
	{BGC,1,0x0165,{15,17,22,23}},
	{BGC,1,0x0166,{22,23,8,10}},
	{BGM,2,0x0167,{27,29,7,8}},
	{BGC,2,0x0169,{23,32,8,25}},							// Slice 28 (11,259)
	{BGC,2,0x016b,{0,7,8,25}},
	{BGC,1,0x016d,{7,8,8,10}},
	{BGM,2,0x016e,{8,10,11,13}},
	{BGM,2,0x0170,{13,18,12,13}},
	{BGC,2,0x0172,{7,23,13,25}},
	{BGC,1,0x0174,{23,24,13,15}},
	{BGM,2,0x0175,{20,22,3,5}},
	{BGC,1,0x0177,{19,23,5,7}},
	{BGM,2,0x0178,{28,30,15,17}},
	{BGC,2,0x017a,{23,32,17,25}},
	{BGC,1,0x017c,{30,31,0,2}},
	{BGC,3,0x017d,{31,32,0,7}},								// Slice 29 (11,270)
	{BGC,3,0x0180,{0,17,0,7}},
	{BGC,2,0x0183,{0,3,17,25}},
	{BGC,4,0x0185,{2,17,15,17}},
	{BGC,1,0x0189,{3,16,17,25}},
	{BGC,2,0x018a,{16,32,17,25}},
	{BGC,5,0x018c,{17,32,0,9}},								// Slice 29_ext
	{BGT|BGI,ORI_LRUD,TRIG_WFALL,{0,3,0,17}},
	{BGC,1,0x0191,{0,1,0,2}},
	{BGC,2,0x0192,{0,6,17,25}},
	{BGC,1,0x0194,{6,7,17,19}},
	{BGM,2,0x0195,{5,7,3,5}},
	{BGC|BGQ,1,0x0197,{4,8,5,7}},
	{BGC,1,0x0198,{17,18,7,9}},
	{0,3,0x0199,{18,32,8,25}},
	{BGM,3,0x019c,{6,18,19,25}},
	{BGC,1,0x019f,{18,32,7,8}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{6,32,18,21}},
	{BGI|BGM,2,0x01a0,{16,32,19,20}},						// Slice 30 (10,280) +3
	{BGC,1,0x01a2,{0,32,7,8}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,18,21}},
	{BGI|BGM,2,0x01a3,{0,32,19,20}},
	{BGM,2,0x01a5,{3,5,2,4}},
	{0,3,0x01a7,{0,32,8,25}},								// Slice 31 (4,282)
	{BGC,1,0x01aa,{0,30,7,8}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,18,21}},
	{BGI|BGM,2,0x01ab,{0,32,19,20}},
	{BGM,2,0x01ad,{3,5,2,4}},
	{0,3,0x01af,{0,30,8,25}},
	{0,2,0x01b2,{30,32,19,25}},
	{BGC,1,0x01b4,{30,31,7,9}},								// Slice 32 (7,289)
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,18,21}},
	{BGM,3,0x01b5,{0,32,19,25}},
	{BGC|BGQ,1,0x01b8,{5,9,14,16}},
	{BGT|BGI,ORI_LRUD,TRIG_CHECK_2,{10,13,0,25}},
	{BGT|BGI,ORI_RLDU,TRIG_WFALL,{14,17,0,25}},
	{BGC|BGQ,1,0x01b9,{21,25,14,16}},						// Slice 33
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,18,21}},
	{BGM,3,0x01ba,{0,32,19,25}},
	{BGC|BGQ,1,0x01bd,{4,8,14,16}},
	{BGC|BGQ,1,0x01be,{15,19,14,16}},
	{BGC|BGQ,1,0x01bf,{26,30,14,16}},						// Slice 36_ext
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,7,18,21}},
	{BGM,3,0x01c0,{0,7,19,25}},
	{BGC,1,0x01c3,{3,7,16,18}},
	{BGC,1,0x01c4,{6,7,13,15}},
	{BGC,2,0x01c5,{7,27,13,25}},
	{BGM,2,0x01c7,{12,17,12,13}},
	{BGC,1,0x01c9,{27,28,13,15}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{27,32,18,21}},
	{BGM,3,0x01ca,{27,32,19,25}},							// Slice 39_ext
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,18,21}},
	{BGM,3,0x01cd,{0,32,19,25}},
	{BGC|BGQ,1,0x01d0,{8,14,11,13}},
	{BGC|BGQ,1,0x01d1,{23,29,11,13}},						// Slice 42_ext
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,5,18,21}},
	{BGM,3,0x01d2,{0,5,19,25}},
	{BGC,1,0x01d5,{4,5,11,13}},
	{BGC,2,0x01d6,{5,32,11,25}},							// Slice 45_ext
	{BGC,1,0x01d8,{0,1,11,13}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,18,21}},
	{BGM,3,0x01d9,{0,32,19,25}},
	{BGC|BGQ,1,0x01dc,{7,11,11,13}},
	{BGC|BGQ,1,0x01dd,{21,25,11,13}},
	{BGC,1,0x01de,{31,32,10,12}},							// Slice 46_ext
	{BGC,2,0x01df,{0,32,10,25}},							// Slice 47_ext
	{BGC,2,0x01e1,{0,32,10,25}},							// Slice 48_ext
	{BGC,2,0x01e3,{0,32,10,25}},
	{0,1,0x01e5,{9,10,2,7}},
	{0,1,0x01e6,{15,16,2,7}},
	{0,1,0x01e7,{18,19,2,7}},
	{0,1,0x01e8,{21,22,2,7}},
	{0,1,0x01e9,{28,30,0,10}},
	{BGT|BGI,ORI_LRUD,TRIG_CHECK_3,{28,31,0,10}},			// Slice fin
	{BGC,2,0x01ea,{0,32,10,25}},
	{0,1,0x01ec,{11,12,2,7}},
	{0,1,0x01ed,{16,17,2,7}},
	{0,1,0x01ee,{19,20,2,7}},
	{0,1,0x01ef,{28,30,0,10}},
	{BGT|BGI,ORI_LRUD,TRIG_CHECK_0,{28,31,0,10}},			// Slice go
	{0,2,0x01f0,{0,32,21,25}},
	{BGC,3,0x01f2,{4,10,0,2}},
	{BGC,1,0x01f5,{5,7,2,8}},
	{BGC,3,0x01f6,{5,10,8,11}},
	{BGC,1,0x01f9,{10,12,0,11}},
	{BGC,1,0x01fa,{7,10,5,6}},
	{0,1,0x01fb,{13,15,9,11}},
	{BGC,4,0x01fc,{17,25,0,2}},
	{BGC,1,0x0200,{17,19,2,9}},
	{BGC,3,0x0201,{17,25,9,11}},
	{BGC,3,0x0204,{23,25,7,9}},
	{0,1,0x0207,{19,23,7,9}},
	{BGC,2,0x0208,{23,25,2,4}},
	{0,1,0x020a,{26,28,9,11}},								// B.C.
	{BGC,1,0x020b,{19,20,11,12}},							// dash
	{0,1,0x020c,{8,22,20,21}},								// Slice B.C. Dash
	{BGC|BGI,1,0x020d,{0,2,17,19}},
	{BGM,3,0x020d,{0,10,19,25}},
	{BGC,3,0x0210,{10,32,19,25}},
	{0,1,0x0213,{3,5,1,3}},
	{0,1,0x0214,{20,22,17,19}},
	{BGC|BGI,1,0x0215,{17,19,17,19}},						// Slice demo instructions
	{BGC,2,0x0215,{0,32,11,13}},
	{0,2,0x0217,{0,32,13,25}},
	{0,1,0x0219,{12,18,2,4}}								// Slice hi scores
};

// You can save a lot of flash by making maps of regularly used combinations. These can now be one Inner BG instead of many.
// !PLATZ_BG_DIRECTORY!
const bgDirectory pgmBgDir[] PROGMEM = {
	{0x0000,0x00,0x0000,0x09,2,0,8,2,0x00,PF_ZERO},			// Slice 0
	{0x0000,0x04,0x0009,0x09,2,0,6,1,0x02,PF_ZERO},			// Slice 1
	{0x0004,0x08,0x0012,0x06,4,2,0,2,0x03,PF_ZERO},			// Slice 2
	{0x000c,0x03,0x0018,0x06,2,2,0,1,0x05,PF_ZERO},			// Slice 3
	{0x000f,0x02,0x001e,0x09,2,0,7,2,0x06,PF_ZERO},			// Slice 4
	{0x0011,0x08,0x0027,0x05,1,1,0,1,0x08,PF_ZERO},			// Slice 5
	{0x0019,0x01,0x002c,0x09,3,1,0,1,0x09,PF_ZERO},			// Slice 6
	{0x001a,0x01,0x0035,0x06,2,0,4,1,0x0a,0},				// Slice 7
	{0x001b,0x02,0x003b,0x04,2,2,3,2,0x0b,1},				// Slice 8
	{0x001d,0x03,0x003f,0x09,1,0,7,2,0x0d,PF_ZERO},			// Slice 9
	{0x0020,0x03,0x0048,0x0c,1,0,10,2,0x0f,2},				// Slice 10
	{0x0023,0x02,0x0054,0x0e,3,0,11,3,0x11,3},				// Slice 11
	{0x0025,0x04,0x0062,0x0a,2,1,8,2,0x14,PF_ZERO},			// Slice 15
	{0x0029,0x03,0x006c,0x09,2,0,7,0,0x16,4},				// Slice 16
	{0x002c,0x02,0x0075,0x18,1,0,22,3,0x16,PF_ZERO},		// Slice 17
	{0x002e,0x00,0x008d,0x09,2,2,8,0,0x19,5},				// Slice 19
	{0x002e,0x00,0x008d,0x09,2,2,8,0,0x19,5},				// Slice 20
	{0x002e,0x02,0x0096,0x0d,3,0,11,1,0x19,PF_ZERO},		// Slice 21
	{0x0030,0x04,0x00a3,0x03,1,0,2,0,0x1a,6},				// Slice 22
	{0x0034,0x04,0x00a6,0x05,1,0,4,1,0x1a,7},				// Slice 23
	{0x0038,0x07,0x00ab,0x03,1,1,0,1,0x1b,PF_ZERO},			// Slice 24_ext
	{0x003f,0x06,0x00ae,0x02,1,1,0,1,0x1c,PF_ZERO},			// Slice 25_ext
	{0x0045,0x07,0x00b0,0x02,1,1,0,1,0x1d,PF_ZERO},			// Slice 26_ext
	{0x004c,0x05,0x00b2,0x06,1,0,4,2,0x1e,PF_ZERO},			// Slice 24
	{0x0051,0x03,0x00b8,0x06,3,2,5,3,0x20,PF_ZERO},			// Slice 25
	{0x0054,0x04,0x00be,0x05,2,2,0,2,0x23,PF_ZERO},			// Slice 26
	{0x0058,0x04,0x00c3,0x06,2,2,5,2,0x25,PF_ZERO},			// Slice 27
	{0x005c,0x03,0x00c9,0x0b,2,0,10,2,0x27,PF_ZERO},		// Slice 28
	{0x005f,0x03,0x00d4,0x0c,1,0,8,1,0x29,PF_ZERO},			// Slice 29
	{0x0062,0x00,0x00e0,0x06,3,0,4,0,0x2a,PF_ZERO},			// Slice 29_ext
	{0x0062,0x00,0x00e0,0x06,3,0,4,0,0x2a,PF_ZERO},			// Slice 30_ext
	{0x0062,0x00,0x00e0,0x06,3,0,4,0,0x2a,PF_ZERO},			// Slice 31_ext
	{0x0062,0x03,0x00e6,0x0c,3,0,9,3,0x2a,8},				// Slice 30
	{0x0065,0x00,0x00f2,0x05,3,3,0,3,0x2d,9},				// Slice 31
	{0x0065,0x02,0x00f7,0x07,3,2,6,4,0x30,10},				// Slice 32
	{0x0067,0x02,0x00fe,0x06,2,2,0,1,0x34,PF_ZERO},			// Slice 33
	{0x0069,0x02,0x0104,0x05,2,2,4,1,0x35,PF_ZERO},			// Slice 36_ext
	{0x0069,0x02,0x0104,0x05,2,2,4,1,0x35,PF_ZERO},			// Slice 37_ext
	{0x0069,0x02,0x0104,0x05,2,2,4,1,0x35,PF_ZERO},			// Slice 38_ext
	{0x006b,0x05,0x0109,0x09,3,0,7,2,0x36,PF_ZERO},			// Slice 39_ext
	{0x006b,0x05,0x0109,0x09,3,0,7,2,0x36,PF_ZERO},			// Slice 40_ext
	{0x006b,0x05,0x0109,0x09,3,0,7,2,0x36,PF_ZERO},			// Slice 41_ext
	{0x0070,0x04,0x0112,0x04,2,2,0,1,0x38,PF_ZERO},			// Slice 42_ext
	{0x0070,0x04,0x0112,0x04,2,2,0,1,0x38,PF_ZERO},			// Slice 43_ext
	{0x0070,0x04,0x0112,0x04,2,2,0,1,0x38,PF_ZERO},			// Slice 44_ext
	{0x0074,0x06,0x0116,0x04,2,0,3,1,0x39,PF_ZERO},			// Slice 45_ext
	{0x007a,0x05,0x011a,0x06,3,2,5,1,0x3a,11},				// Slice 46_ext
	{0x007f,0x05,0x0120,0x01,1,1,0,0,0x3b,PF_ZERO},			// Slice 47_ext
	{0x0084,0x05,0x0121,0x01,1,1,0,0,0x3b,PF_ZERO},			// Slice 48_ext
	{0x0084,0x05,0x0121,0x01,1,1,0,0,0x3b,PF_ZERO},			// Slice 49_ext
	{0x0084,0x05,0x0121,0x01,1,1,0,0,0x3b,PF_ZERO},			// Slice 50_ext
	{0x0089,0x0e,0x0122,0x07,1,1,6,0,0x3b,PF_ZERO},			// Slice finish
	{0x0097,0x0f,0x0129,0x06,1,1,5,0,0x3b,PF_ZERO},			// Slice go
	{0x00a6,0x0d,0x012f,0x10,0,0,0,5,0x3b,PF_ZERO},			// Slice B.C. Dash
	{0x00b3,0x13,0x013f,0x06,0,0,0,3,0x40,PF_ZERO},			// Slice demo instructions
	{0x00c6,0x06,0x0145,0x03,0,0,0,2,0x43,PF_ZERO},			// Slice hi scores
	{0,0,0,0,0,0,0,0,0,PF_ZERO}								// Slice empty slice
};
