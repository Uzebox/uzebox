/*
 * Title:   Simple Maze
 * Author:  Clay Cowgill
 * Version: 1.0
 * Date: 9-23-2008
 *
 * The first(?) 3rd-party Uzebox software? ;-)
 *
 * License: Uhhh... Do whatever you want with it.  If you're injured as a result-- not my fault.
 *
 * Follow Uze's tutorial for how to build a "Hello, world" demo.
 * http://www.belogic.com/uzebox/tutorials.htm
 *
 * Use the contents of this file for the "HelloWorld.c" and place
 * 'mazetiles.c' in the same directory as your project.
 *
 *  includes an adaptation of the Maze generator 
 *  written by Michael Kircher  2008-09-11
 */

#include <avr/io.h> 
#include <stdlib.h> 
#include <avr/pgmspace.h> 
#include <uzebox.h>

#include "data/fonts.pic.inc" // UZEBOX 'console' font, as seen in Megatris
#include "data/mazetiles.c" 		  // Graphics made specifically for maze game

const char strWin[] PROGMEM ="YOU ESCAPED!"; 
const char strCred[] PROGMEM ="SIMPLE MAZE DEMO - CLAY COWGILL, 2008";

/* X_SIZE and Y_SIZE should be odd, and greater than 5 */ 
#define X_SIZE 37
#define Y_SIZE 25

#define BLANK ' ' // these defines were used in testing a 'text' version of the game
#define WALL 'a'  // prior to the 'real' graphics.  No real need to change them.
#define PLAYER '*'
#define EXIT 'O'

signed char dx[4]={1,0,-1,0};  // used by maze generator to choose directions
signed char dy[4]={0,-1,0,1}; 

char maze[Y_SIZE][X_SIZE+1]; 

int drill(int *ay, int *ax, int j)  // punches a path through the Maze to make is solveable
{ 
  int by,bx,c=j; 
  do 
  { 
    bx=*ax+2*dx[j]; 
    by=*ay+2*dy[j]; 
    if(WALL == maze[by][bx]) 
    { 
      maze[by][bx]=j; 
      maze[*ay+dy[j]][*ax+dx[j]]=BLANK; 
      *ax=bx; 
      *ay=by; 
      /* drill successful, ask for a new random direction */ 
      return(1); 
    } 
    j=(j+1)*(j<3); 
  } while(j!=c); 
  /* ended up in a blind lane */ 
  return(0); 
} 

// Redraw refreshes the screen display to the user
// in should be called about 30 times a second.

void redraw()
{
  int ay,ax; 

  SetTileTable(mazetiles);  // this tells "SetTile" to use teh graphics starting at mazetiles[]
	
	for (ay=0;ay<Y_SIZE-1;ay++)   // go over the maze array depth
		for (ax=0;ax<X_SIZE;ax++) // ...and across the width
			switch (maze[ay][ax])
			{
				case WALL:
					SetTile(ax+3,ay,0); break;  // for each case, check to see what the symbol in the maze is,
				case BLANK:                     // then translate it to a tile from 'mazetiles[]'.  The numbers 
					SetTile(ax+3,ay,5); break;  // are direct tile numbers as seen in Tile Studio
				case EXIT:
					SetTile(ax+3,ay,10); break;
				case PLAYER:
					SetTile(ax+3,ay,7); break;
				default: 
					SetTile(ax+3,ay,2); break;
			};
	for (ax=3;ax<40;ax++) // draw an 'empty' border along the bottom edge of the maze (cosmetics)
		SetTile(ax,24,5);
			//PrintChar(ax+2,ay,maze[ay][ax]);  //this is the 'draw' command from the old text-only version
    Print(3,26,strCred);  // vanity string
	//PrintInt(22,25,GetTrueRandomSeed(),true);
	PrintInt(22,25,GetPrngNumber(0),true);
	
}

int main()
{
  unsigned char x,corner;
  unsigned char frames;
  unsigned int joypad;
  unsigned char player_x, player_y;
  unsigned char exit_x=0, exit_y=0;
  unsigned int z=0;
  int ay,ax,j; 

   SetFontTable(fonts);  // this tells Print___() commands what font to use
   ClearVram();          // clears out display memory (like a 'clear screen')
   
   //srand(GetTrueRandomSeed());    		//randomize using the entropy generator
   GetPrngNumber(0);

new_maze:
  for(ay=0; ay<Y_SIZE; ay++) 
  { 
    for(ax=0; ax<X_SIZE; ax++) 
      if(ax>0 && ay>0 && ax<X_SIZE-1 && ay<Y_SIZE-1) // fills the maze with walls, that are then 'drilled' out
        maze[ay][ax]=WALL; 
      else 
        maze[ay][ax]=BLANK; 
  } 

  ay=2; // 2,2 is the top left corner of the maze
  ax=2; 
  maze[ay][ax]=4; // starting state for the maze generator
  
  do 
  { 
    while(drill(&ay,&ax,(int)(rand()&0x03))) {}  // start drilling things out (using random)
    j=maze[ay][ax];                              // note that the first time the game runs, the maze will always be 
    maze[ay][ax]=BLANK;                          // the same because of the random number generator always starting
    if(j<4)                                      // in the same state.  (Normally I will 'sum' the contents of RAM
    {                                            // on power-up to make a random seed, but I don't know how to mess
      /* back-track */                           // with WinAVR's C-startup yet.)
      ax=ax-2*dx[j]; 
      ay=ay-2*dy[j]; 
    } 
  } while(j<4); 
 
 do{                                             // This generates a random position inside the maze to start the
 	do player_x=rand();                          // player.  Seems more interesting than a corner.
	while ((player_x>=X_SIZE-1)||(player_x<=1));
	do player_y=rand();
	while ((player_y>=Y_SIZE-1)||(player_y<=1));
	}	
 while (maze[player_y][player_x]==WALL);         // (repeats until it picks a random spot that's not a wall)
 
 maze[player_y][player_x]=PLAYER;                // place the player in the maze

 do                                              // here we choose a random number from 0-3, representing one of the
 	{                                            // four corners of the maze.
 	corner=rand()&0x03;
 	switch (corner)                              
 		{
	 	case 0:
			exit_x=2; exit_y=2; break;
		case 1:
			exit_x=X_SIZE-3; exit_y=2; break;
		case 2:
			exit_x=X_SIZE-3; exit_y=Y_SIZE-3;break;
		case 3:
			exit_x=2; exit_y=Y_SIZE-3; break;
 		}
	}
 while (abs(player_x-exit_x)<16);                // we compare the distance from the player to the exit (horizontally)
                                                 // and only accept the corner if it's relatively far away from the player
	                                             // (could use both x and y distance for better effect)
	maze[exit_y][exit_x]=EXIT;                   // place the exit in the maze
	
	redraw();                                    // draw the screen for the player



 
 // This is what happens when you turn a static demo into a 'game'
 // For a real game, you probably don't want to do this, but it works here and is simple...
 
 //while(PIND&0x04)       // if the button on the Gamer Baseboard isn't pressed
 while(!IsPowerSwitchPressed()) // if the button on the Gamer Baseboard isn't pressed
 {


 z++;                  // the z counter is used later on for a seed for the random number generator (user interaction = randomness)
 joypad=ReadJoypad(0);  // get the joypad state

 WaitVsync(1);          // this waits a frame and essentially slows the gameplay down
 frames++;              // frame counter can be used for animations (later)
 if (joypad&0xff)       // if the D-pad has a bit or more pressed
 	{
 	maze[player_y][player_x]=BLANK;            // erase the old player position
 	if (joypad&BTN_DOWN)                           // down?
 		if (maze[player_y+1][player_x]!=WALL)  // don't move if a wall is blocking your path
			player_y++;                        // you could 'switch/case' these with breaks to easily disable diagonal moves...
	if (joypad&BTN_UP)                           // up?
	 	if (maze[player_y-1][player_x]!=WALL)
			player_y--;
 	if (joypad&BTN_LEFT)                           // left?
 		if (maze[player_y][player_x-1]!=WALL)
			player_x--;
	if (joypad&BTN_RIGHT)                           // right
 		if (maze[player_y][player_x+1]!=WALL)
			player_x++;
	if (maze[player_y][player_x]==EXIT)        // if the new position is the EXIT, you just won
		{
		maze[exit_y][exit_x]=PLAYER;           // place the player on top of the exit
		redraw();                              // draw the screen for the user
		Print(14,25,strWin);                   // prints the "YOU ESCAPED!" message
		WaitVsync(30);                         // and waits for a while.
		for (x=14;x<26;x++)                    // this loops over the "YOU ESCAPED!" message
			SetTile(x,25,11);                  // and erases it.
		break;                                 // this breaks out of the while(PIND...) loop, causing a new maze to be generated 
		}
	
	maze[player_y][player_x]=PLAYER;           // after the movement calculations, put the player in the new spot
 	}
 else
 	{
	// check for idle time                     // use the frame counter to make the player graphic change if you wait around. ;-)
	}

	redraw();    // redraw the screen to update new player position
 }

 goto new_maze;  // this is just a hack to go do everything over again.  Before anyone lectures me about using GOTO, think about
                 // why every CPU has a 'jump/goto' instruction in its assembly language and uses it-- frequently. :-) 
} 
