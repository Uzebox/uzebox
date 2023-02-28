
const animation bobAnimations[] PROGMEM = {
	/* Lynx form */
	{6,3,2,1,0,0,0,0,animLynxIdle},
	{6,3,2,6,0,0,4,0,animLynxRun},
	/* Dragonfly form */
	{4,2,2,3,0,0,4,0,animDFly},
	/* Turtle form */
	{2,2,1,3,0,0,8,0,animTurtle},
	/* Snail form */
	{1,1,1,1,0,0,128,0,animSnail}
};

const animation bgAnimations[] PROGMEM = {
	{2,2,1,4,0,0,8,1,animWfallTop},
	{8,2,4,4,0,0,8,1,animWfallBtm},
	{1,1,1,4,0,0,16,1,animWave},
	{4,2,2,2,0,0,8,0,animFire},
	{4,2,2,4,0,0,10,0,animCoins},
	{1,1,1,4,0,0,16,0,animWave},	// Not synchronized
	{14,14,1,4,0,0,128,0,animPressStart}
};
