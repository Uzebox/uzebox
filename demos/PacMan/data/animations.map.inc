
#define GAME_ANIMATIONS_COUNT 48

const animation gameAnimations[GAME_ANIMATIONS_COUNT] PROGMEM ={
	/* Pacman */
	{1,1,1,4,0,0,2,pacman_up},{1,1,1,4,0,0,2,pacman_lt},{1,1,1,4,0,0,2,pacman_dn},{1,1,1,4,0,0,2,pacman_rt},{1,1,1,10,0,0,8,pacman_die},{4,2,2,16,0,0,2,pacman_giant},
	/* Blinky */
	{1,1,1,2,0,0,4,blinky_up},{1,1,1,2,0,0,4,blinky_lt},{1,1,1,2,0,0,4,blinky_dn},{1,1,1,2,0,0,4,blinky_rt},{1,1,1,2,0,0,4,ghost_flee},
	{1,1,1,4,0,0,4,ghost_flash},
	{1,1,1,1,0,0,4,ghost_dead_up},{1,1,1,1,0,0,4,ghost_dead_lt},{1,1,1,1,0,0,4,ghost_dead_dn},{1,1,1,1,0,0,4,ghost_dead_rt},{1,1,1,2,0,0,4,blinky_stitched},{2,2,1,4,0,0,4,blinky_exposed},
	/* Pinky */
	{1,1,1,2,0,0,4,pinky_up},{1,1,1,2,0,0,4,pinky_lt},{1,1,1,2,0,0,4,pinky_dn},{1,1,1,2,0,0,4,pinky_rt},{1,1,1,2,0,0,4,ghost_flee},
	{1,1,1,4,0,0,4,ghost_flash},
	{1,1,1,1,0,0,4,ghost_dead_up},{1,1,1,1,0,0,4,ghost_dead_lt},{1,1,1,1,0,0,4,ghost_dead_dn},{1,1,1,1,0,0,4,ghost_dead_rt},
	/* Inky */
	{1,1,1,2,0,0,4,inky_up},{1,1,1,2,0,0,4,inky_lt},{1,1,1,2,0,0,4,inky_dn},{1,1,1,2,0,0,4,inky_rt},{1,1,1,2,0,0,4,ghost_flee},
	{1,1,1,4,0,0,4,ghost_flash},
	{1,1,1,1,0,0,4,ghost_dead_up},{1,1,1,1,0,0,4,ghost_dead_lt},{1,1,1,1,0,0,4,ghost_dead_dn},{1,1,1,1,0,0,4,ghost_dead_rt},
	/* Clyde */
	{1,1,1,2,0,0,4,clyde_up},{1,1,1,2,0,0,4,clyde_lt},{1,1,1,2,0,0,4,clyde_dn},{1,1,1,2,0,0,4,clyde_rt},{1,1,1,2,0,0,4,ghost_flee},
	{1,1,1,4,0,0,4,ghost_flash},
	{1,1,1,1,0,0,4,ghost_dead_up},{1,1,1,1,0,0,4,ghost_dead_lt},{1,1,1,1,0,0,4,ghost_dead_dn},{1,1,1,1,0,0,4,ghost_dead_rt}
};
