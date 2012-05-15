/*
 *  Dr.Mario port for Uzebox ver 1.1
 *  Copyright (C) 2009  Codecrank, Mapes
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

#include "data/fonts.pic.inc"
#include "data/patches.inc"

#include "data/drmario_main.c"
#include "data/drmario_sprites.c"
#include "data/drmario_music.inc"
#include "data/drmario_title.c"



#define MOVE_LEFT 1
#define MOVE_RIGHT 2
#define MOVE_DOWN 3
#define MOVE_SPIN 4
#define MOVE_START 5

#define PLAYER1 0
#define PLAYER2 1


#define BOTTLE_FIRST_COLUMN 4
#define BOTTLE2_FIRST_COLUMN 28
#define BOTTLE_LAST_COLUMN 11
#define BOTTLE_FIRST_ROW 8
#define BOTTLE_LAST_ROW 24
#define BOTTLE_WIDTH 8
#define BOTTLE_DEPTH 17

#define LEFT_RED 1
#define LEFT_BLUE 3
#define LEFT_YELLOW 5
#define RIGHT_RED 2
#define RIGHT_BLUE 4
#define RIGHT_YELLOW 6
#define BOTTOM_RED 7
#define BOTTOM_BLUE 9
#define BOTTOM_YELLOW 11
#define TOP_RED 8
#define TOP_BLUE 10
#define TOP_YELLOW 12
#define SINGLE_RED 13
#define SINGLE_BLUE 14
#define SINGLE_YELLOW 15

#define VIRUS_LEFT_ROW  23
#define VIRUS_LEFT_COLUMN1 19
#define VIRUS_LEFT_COLUMN2 22

#define VIRUS_RED1 		16
#define VIRUS_RED2 		17
#define VIRUS_BLUE1 	18
#define VIRUS_BLUE2 	19
#define VIRUS_YELLOW1 	20
#define VIRUS_YELLOW2 	21

#define DISSAPEAR_ANIM_1 22
#define DISSAPEAR_ANIM_2 23
#define DISSAPEAR_ANIM_3 24
#define DISSAPEAR_ANIM_4 25
#define DISSAPEAR_ANIM_5 0

 
#define CURSOR1		54
#define CURSOR2		55	

#define HORIZ 0
#define VERT 1

#define FULL 2
#define HALF 1


const char strGame[] PROGMEM = "GAME";
const char strOver[] PROGMEM = "OVER";

const char strPress[] PROGMEM = "PRESS";
const char strStart[] PROGMEM = "START";

const char strWinner[] PROGMEM = "WINNER!";
const char strLoser[] PROGMEM = "LOSER!";

const char strWin[] PROGMEM = "WIN";
const char strP1Wins[] PROGMEM = "P1 WINS!";
const char strP2Wins[] PROGMEM = "P2 WINS!";

const char strLevel[] PROGMEM = "LEVEL";
const char strGameMode[] PROGMEM = "2 PLAYER GAME";


bool MovePill(unsigned short player, unsigned short move_type);
unsigned short grab_input(unsigned short player);
void NextPill(unsigned short player);
void NewPill(unsigned short player);
void LockPill(unsigned short player);
void InitBottle(unsigned short player);
void AnimateVirus();
unsigned short CheckColorMatch(unsigned short tile1 , unsigned short tile2 );
void CheckMatching4(unsigned short player);
void SplitPill(unsigned short player,unsigned short x , unsigned short y);
void DropFloaters(unsigned short player);
void DropFilledSpace(unsigned short player,unsigned short x , unsigned short y);
void FlingTrash(unsigned short sender);
void QueTrash(unsigned short sender, unsigned short trash_count);



struct pill {
	bool orientation;
	unsigned short length;
	unsigned short x;
	unsigned short y;
	unsigned short bottle_x;
	unsigned short bottle_y;
	unsigned short top;
	unsigned short bottom;
	unsigned short left;
	unsigned short right;	
};

struct bottle {
	unsigned short space[BOTTLE_WIDTH][BOTTLE_DEPTH];
	unsigned short virus_count;
	bool check_for_sequences;
	unsigned short sequences_count;
	bool contains_floaters; 
	unsigned short dissapear_anim;
	unsigned short qued_trash;
	bool new_pill;
};


struct pill current_pill[2]; 
struct pill next_pill[2];
unsigned short gravity=60;
unsigned int round_time;
bool pill_falling=1;
unsigned short auto_repeat_counter=0;
struct bottle bottle[2];
unsigned short virus_anim_counter=0;
unsigned short drop_floater_timer=0;
unsigned short x,y,j,k,saved_state, z=0; // generic
unsigned char difficulty_level[2];
bool round_lost[2];
unsigned short rounds_won[2];
void SetStageSettings();
unsigned short move_type=0;
unsigned short seed;
void DissapearAnim(unsigned short player);
void RoundOver();
unsigned int long_timer;

bool WAIT_ON_NO_INPUT=0;

int main(){


	unsigned short i=0;
	unsigned short j=0;


   	ClearVram();
 
	SetTileTable(drmario_title);	 
	DrawMap(0,0,drmario_map_title);

	while ( 1 )
	{
		if ( grab_input(PLAYER1) == MOVE_START )
		{
			seed++;
			while(ReadJoypad(PLAYER1)!=0); //wait for key release
			break;
		}
	}
 
 
	SetFontTable(fonts);
	InitMusicPlayer(patches);



 
	difficulty_level[PLAYER1]=1;
	difficulty_level[PLAYER2]=1;


	while ( 1 ) 
	{

	
		SetStageSettings();
 	

		rounds_won[PLAYER1]=0;
		rounds_won[PLAYER2]=0;
	
		srand(seed);

		while ( ( rounds_won[PLAYER1] < 3) && ( rounds_won[PLAYER2] < 3))
		{	

			round_lost[PLAYER1]=0;
			round_lost[PLAYER2]=0;

		 	ClearVram();
			SetTileTable(main_board);	 
			DrawMap(0,0,drmario_map_main);



			// redraw score board
			for (i=0 ; i < rounds_won[PLAYER1] ; i++ )
				Print(16,18-(i*3),strWin); 
			for (i=0 ; i < rounds_won[PLAYER2] ; i++ )
				Print(21,18-(i*3),strWin); 



			SetTileTable(pills);
	
			StartSong(song_drmario_main);
		
			InitBottle(PLAYER1);
			InitBottle(PLAYER2);

			NextPill(PLAYER1);
			NewPill(PLAYER1);		

			NextPill(PLAYER2);
			NewPill(PLAYER2);
			round_time = 0;
			i=0;
			while ( ! ( round_lost[PLAYER1] || round_lost[PLAYER2] ) )
			{		

				if(GetVsyncFlag()) // thx uze !
				{

					ClearVsyncFlag();

					// you win if all viruses are gone
					if ( bottle[PLAYER1].virus_count == 0 )
					{
						// player 2 loses
						round_lost[PLAYER2]=1;				
					}
					if ( bottle[PLAYER2].virus_count == 0 )
					{
						// player 1 loses
						round_lost[PLAYER1]=1;
					}


					// check for squences if needed
					while  ( bottle[PLAYER1].check_for_sequences )
					{
						CheckMatching4(PLAYER1);
					}	

					while  ( bottle[PLAYER2].check_for_sequences )
					{
						CheckMatching4(PLAYER2);
					}	

					// always anymate virus
					AnimateVirus();
			

					// drop floaters unless pills dissapear ainmation is going on
					if ( drop_floater_timer < 10 ) 
					{
						drop_floater_timer++;
					}
					else
					{
						drop_floater_timer=0;
	
						if ( bottle[PLAYER1].contains_floaters ) 
						{
							 if ( ! bottle[PLAYER1].dissapear_anim )
							 	DropFloaters(PLAYER1);
						}	

						if ( bottle[PLAYER2].contains_floaters )
						{
							if ( ! bottle[PLAYER2].dissapear_anim )
								DropFloaters(PLAYER2);	
						}
					}
				
 	
	
					// until gravity timer hits
					if ( i > gravity - 6 * (round_time/1800))
					{
						// force move down when time hits
						MovePill(PLAYER1, MOVE_DOWN);
						i=0;
					}
					 // move pill around unless some pills are dissapearing or there are floaters
					else if ( bottle[PLAYER1].dissapear_anim) 
					{		
						DissapearAnim(PLAYER1);
					}	
					else if (bottle[PLAYER1].contains_floaters)
					{
					}
					else if (bottle[PLAYER1].new_pill == true)
					{ 
						if (bottle[PLAYER1].qued_trash > 0) 
						{
							FlingTrash(PLAYER1);
						}
						else
						{								
							NewPill(PLAYER1);
							bottle[PLAYER1].new_pill = false;
							i=0;
						}							
					}
					else if ( (move_type=grab_input(PLAYER1)) )
					{
						MovePill(PLAYER1, move_type);
						i++;
					}
					else
					{
						i++;
					}

					if ( j > gravity - 6 * (round_time/1800))
					{
						// force move down when time hits
						MovePill(PLAYER2, MOVE_DOWN);
						j=0;
					}
					else if ( bottle[PLAYER2].dissapear_anim) 
					{
							DissapearAnim(PLAYER2);
					}
					else if ( bottle[PLAYER2].contains_floaters )
					{
					}
					else if (bottle[PLAYER2].new_pill == true)
					{ 
						if (bottle[PLAYER2].qued_trash > 0) 
						{
							FlingTrash(PLAYER2);
						}
						else
						{								
							NewPill(PLAYER2);
							bottle[PLAYER2].new_pill = false;
							j = 0;
						}
					}
					else if ( (move_type=grab_input(PLAYER2)) )
					{
						MovePill(PLAYER2, move_type);
						j++;
					}
					else
					{
						j++;
					}
					round_time++;

				}

			}

			RoundOver();
 	
		}


	} // end while (1)

	
} 


void RoundOver()
{


			if ( round_lost[PLAYER1] )
			{
			
 				Print(BOTTLE_FIRST_COLUMN+1, BOTTLE_FIRST_COLUMN+5, strLoser);
				Print(BOTTLE_FIRST_COLUMN+25, BOTTLE_FIRST_COLUMN+5, strWinner);
 				rounds_won[PLAYER2]++;

				// score board
				Print(21,21-(rounds_won[PLAYER2]*3),strWin);

				if ( rounds_won[PLAYER2] == 3 )
				{ 
					Print(16,9,strP2Wins);	
				}

 		   	}
			else
			{
				Print(BOTTLE_FIRST_COLUMN+1, BOTTLE_FIRST_COLUMN+5, strWinner);
				Print(BOTTLE_FIRST_COLUMN+25, BOTTLE_FIRST_COLUMN+5, strLoser);
	 			rounds_won[PLAYER1]++;

				// score board
				Print(16,21-(rounds_won[PLAYER1]*3),strWin);

				if ( rounds_won[PLAYER1] == 3 )
				{ 
					Print(16,9,strP1Wins);	
				}
			}

			
			long_timer=10000;
			// make "press start" blink untill someone presses start
			do
			{
				if (   long_timer == 10000 ) 
				{
		
					Print(BOTTLE_FIRST_COLUMN+2, BOTTLE_FIRST_COLUMN+11, strPress);
					Print(BOTTLE_FIRST_COLUMN+2, BOTTLE_FIRST_COLUMN+12, strStart);

	     			Print(BOTTLE_FIRST_COLUMN+26, BOTTLE_FIRST_COLUMN+11, strPress);
					Print(BOTTLE_FIRST_COLUMN+26, BOTTLE_FIRST_COLUMN+12, strStart);
		
				}

				if (  long_timer == 20000 )
				{

					Fill(BOTTLE_FIRST_COLUMN+2, BOTTLE_FIRST_COLUMN+11,5,1,0);
					Fill(BOTTLE_FIRST_COLUMN+2, BOTTLE_FIRST_COLUMN+12,5,1,0);

	     			Fill(BOTTLE_FIRST_COLUMN+26, BOTTLE_FIRST_COLUMN+11,5,1,0);
					Fill(BOTTLE_FIRST_COLUMN+26, BOTTLE_FIRST_COLUMN+12,5,1,0);

					long_timer=0;
				}

				long_timer++;


	


			} while ( ! ( ReadJoypad(PLAYER1) & BTN_START ) );

			while ( ReadJoypad(PLAYER1) !=0);

	
			//StopSong(song_drmario_main);
		

}


void DissapearAnim(unsigned short player)
{
	bottle[player].dissapear_anim=0;

	unsigned short first_column;


	if ( player == PLAYER1 ) 
	{
		first_column=BOTTLE_FIRST_COLUMN;
	}
	else
	{
		first_column=BOTTLE2_FIRST_COLUMN;
	}


	for ( x=0 ; x< BOTTLE_WIDTH ; x++ )
	{
		for ( y=0 ; y< BOTTLE_DEPTH ; y++ )
		{

			if ( (  bottle[player].space[x][y] >= DISSAPEAR_ANIM_1 ) && ( bottle[player].space[x][y] <= DISSAPEAR_ANIM_3 ) )
			{
				bottle[player].space[x][y]++;
				SetTile(first_column+x, BOTTLE_FIRST_ROW+y, bottle[player].space[x][y]);
				bottle[player].dissapear_anim=1;
			}
			else if ( bottle[player].space[x][y]  == DISSAPEAR_ANIM_4 )
			{
				bottle[player].space[x][y]=0;
				SetTile(first_column+x, BOTTLE_FIRST_ROW+y, bottle[player].space[x][y]);
				bottle[player].dissapear_anim=1;
			}
		}
	}
}

void SetStageSettings()
{

	unsigned int c;

	SetTileTable(main_board);
	DrawMap(0,0,drmario_map_stage_select);


	Print(14, 5, strGameMode);
	Print(7, 7, strLevel);

	SetTile(8+difficulty_level[PLAYER1],9,54);
	SetTile(8+difficulty_level[PLAYER2],11,55);

	PrintByte(32,9,difficulty_level[PLAYER1],false);
	PrintByte(32,11, difficulty_level[PLAYER2],false);


	while (1)
	{
		
		seed++;
		
		c = ReadJoypad(PLAYER1);

		if (c) 
		{
			while (	ReadJoypad(PLAYER1)!=0 );
		}


		if ( ( c&BTN_RIGHT ) && ( difficulty_level[PLAYER1] < 20 ) )
		{
			SetTile(8+difficulty_level[PLAYER1],9,0);
			difficulty_level[PLAYER1]++;
			SetTile(8+difficulty_level[PLAYER1],9,54);
			PrintByte(32,9,difficulty_level[PLAYER1],false);
		}
		else if ( ( c&BTN_LEFT) && ( difficulty_level[PLAYER1] > 1 ) )
		{
			SetTile(8+difficulty_level[PLAYER1],9,0);
			difficulty_level[PLAYER1]--;
			SetTile(8+difficulty_level[PLAYER1],9,54);
			PrintByte(32,9, difficulty_level[PLAYER1],false);
		}
		else if ( c&BTN_START )
			break; 

		c = ReadJoypad(PLAYER2);
		
		if ( c ) 
		{
			while (	ReadJoypad(PLAYER2)!=0 );
		}


		if ( (  c&BTN_RIGHT ) && ( difficulty_level[PLAYER2] < 20 ) )
		{
			SetTile(8+difficulty_level[PLAYER2],11,0);
			difficulty_level[PLAYER2]++;
			SetTile(8+difficulty_level[PLAYER2],11,55);
			PrintByte(32,11,difficulty_level[PLAYER2],false);
		}
		else if ( ( c&BTN_LEFT ) && ( difficulty_level[PLAYER2] > 1 ) )
		{
			SetTile(8+difficulty_level[PLAYER2],11,0);
			difficulty_level[PLAYER2]--;
			SetTile(8+difficulty_level[PLAYER2],11,55);
			PrintByte(32,11, difficulty_level[PLAYER2],false);
		}
		else if ( c&BTN_START )
			break; 

			
	}


}


bool MovePill(unsigned short player, unsigned short move_type)
{

	bool moved=0;

	switch (move_type ) {


		case MOVE_DOWN:
	


			if ( current_pill[player].y ==  BOTTLE_LAST_ROW ) 
			{
				LockPill(player);
				bottle[player].new_pill = true;

				break;
			}
			
			if ( current_pill[player].orientation == HORIZ )
			{
				if (  bottle[player].space[current_pill[player].bottle_x][current_pill[player].bottle_y+1] 
					||  bottle[player].space[current_pill[player].bottle_x+1][current_pill[player].bottle_y+1]  ) 
				{
					
					LockPill(player);
					bottle[player].new_pill = true;


				}
				else
				{		

					Fill(current_pill[player].x,current_pill[player].y, current_pill[player].length, 1, 0);
					current_pill[player].y++;
					current_pill[player].bottle_y++;
			    	SetTile(current_pill[player].x,current_pill[player].y,  current_pill[player].left);
			    	SetTile(current_pill[player].x+1,current_pill[player].y,  current_pill[player].right);
					moved=1;	
				}		
			}
			else
			{
				//  something below me ?
				if ( bottle[player].space[current_pill[player].bottle_x][current_pill[player].bottle_y+1] )
				{
					LockPill(player);
					bottle[player].new_pill = true;
				}
				else
				{		
				 	Fill(current_pill[player].x, ( current_pill[player].y - current_pill[player].length + 1 ), 1, 1, 0);
					current_pill[player].y++;
					current_pill[player].bottle_y++;
					SetTile(current_pill[player].x,current_pill[player].y-1, current_pill[player].top);
					SetTile(current_pill[player].x,current_pill[player].y,  current_pill[player].bottom);	
					moved=1;
				}
			}


		
			break;

		case MOVE_LEFT:	

			// first column , can't move left
			if ( (current_pill[player].bottle_x == 0) )
				break ;


			if ( current_pill[player].orientation == HORIZ )
			{
				//  nothing left of me ?
				if ( ! bottle[player].space[current_pill[player].bottle_x-1][current_pill[player].bottle_y] ) 
				{

					Fill(current_pill[player].x,current_pill[player].y, current_pill[player].length, 1, 0);			
					current_pill[player].x--;
					current_pill[player].bottle_x--;
					SetTile(current_pill[player].x,current_pill[player].y, current_pill[player].left);
					SetTile(current_pill[player].x+1,current_pill[player].y, current_pill[player].right);
					moved=1;
				}
			}
			else
			{
 				//  nothing left of me ?
				if ( ! ( bottle[player].space[current_pill[player].bottle_x-1][current_pill[player].bottle_y] 
					 || bottle[player].space[current_pill[player].bottle_x-1][current_pill[player].bottle_y-1] ) ) 
				{


					Fill(current_pill[player].x, current_pill[player].y-1, 1,current_pill[player].length , 0);
					current_pill[player].x--;
					current_pill[player].bottle_x--;
					SetTile(current_pill[player].x,current_pill[player].y-1, current_pill[player].top);
					SetTile(current_pill[player].x,current_pill[player].y, current_pill[player].bottom);
					moved=1;	
				}
			}

			break;


		case MOVE_RIGHT:	


			if ( current_pill[player].orientation == HORIZ )
			{
				if  ( current_pill[player].bottle_x  == (BOTTLE_WIDTH - 2) )
					break;

				//  nothing right of me ?
				if ( ! bottle[player].space[current_pill[player].bottle_x+2][current_pill[player].bottle_y] ) 
				{

					SetTile(current_pill[player].x, current_pill[player].y, 0);
					current_pill[player].x++;
					current_pill[player].bottle_x++;
					SetTile(current_pill[player].x,current_pill[player].y, current_pill[player].left);
					SetTile(current_pill[player].x+1,current_pill[player].y, current_pill[player].right);
					moved=1;
				}

			}
			else	// VERTICAL 
			{
				if (current_pill[player].bottle_x == (BOTTLE_WIDTH - 1) ) 
					break;
									
				//  nothing right of me ?
				if ( ! ( bottle[player].space[current_pill[player].bottle_x+1][current_pill[player].bottle_y] 
						|| bottle[player].space[current_pill[player].bottle_x+1][current_pill[player].bottle_y-1])  ) 
				{


					Fill(current_pill[player].x, current_pill[player].y-1 , 1, current_pill[player].length, 0);
					current_pill[player].x++;
					current_pill[player].bottle_x++;
					SetTile(current_pill[player].x,current_pill[player].y-1, current_pill[player].top);
					SetTile(current_pill[player].x,current_pill[player].y, current_pill[player].bottom);	
					moved=1;
				}
			}

			break;

		case MOVE_SPIN:	
			

	


			if ( current_pill[player].orientation == HORIZ )
			{
				// check for possible ceiling kicks
				if (  current_pill[player].bottle_y == 0 ) 
				{
						if ( ! MovePill(player, MOVE_DOWN) )
							break ;	
				}					
				if  ( bottle[player].space[current_pill[player].bottle_x][current_pill[player].bottle_y-1]  ) 
				{	  
						if ( ! MovePill(player, MOVE_DOWN) )
							break;
				}	
					
				Fill(current_pill[player].x,current_pill[player].y, 2, 1, 0);
				
				current_pill[player].bottom = current_pill[player].left;
				current_pill[player].top = current_pill[player].right;
			
				current_pill[player].top += 6; // left to top pill tile
				current_pill[player].bottom += 6; // right to bottom pile tile 

				SetTile(current_pill[player].x,current_pill[player].y-1, current_pill[player].top);
				SetTile(current_pill[player].x,current_pill[player].y, current_pill[player].bottom);

				moved=1;

				current_pill[player].orientation = VERT;
			}
			else // VERTICAL 
			{

				
				// check for possible wall kicks to the left
							
				// against the wall or pils ?
				if  (  ( current_pill[player].bottle_x == BOTTLE_WIDTH-1  ) 
					 ||( bottle[player].space[current_pill[player].bottle_x+1][current_pill[player].bottle_y] != 0 ) )
				{	  
						if ( ! MovePill(player, MOVE_LEFT) )
						{
							// special squeeze kick possible ?
							if (  bottle[player].space[current_pill[player].bottle_x-1][current_pill[player].bottle_y] == 0 )
							{
								// can't go through bottle wall...
								if ( current_pill[player].bottle_x == 0 )
									break;


								current_pill[player].left =  current_pill[player].top;
				 				current_pill[player].right = current_pill[player].bottom;

								current_pill[player].left -= 7; // rotated tile
								current_pill[player].right -= 5; // rotated tile 

								SetTile (current_pill[player].x,current_pill[player].y-1,0);
								
								current_pill[player].x--;
								current_pill[player].bottle_x--;
								SetTile(current_pill[player].x,current_pill[player].y,  current_pill[player].left);
    							SetTile(current_pill[player].x+1,current_pill[player].y, current_pill[player].right);

								current_pill[player].orientation = HORIZ;
								moved=1;
								
							}

							break;

						}
					
				} 
		
			


				Fill(current_pill[player].x, ( current_pill[player].y - 1 ), 1, 2, 0);
				current_pill[player].left =  current_pill[player].top;
				current_pill[player].right = current_pill[player].bottom;

				current_pill[player].left -= 7; // rotated tile
				current_pill[player].right -= 5; // rotated tile 

				SetTile(current_pill[player].x,current_pill[player].y,  current_pill[player].left);
    			SetTile(current_pill[player].x+1,current_pill[player].y, current_pill[player].right);

				current_pill[player].orientation = HORIZ;
				moved=1;
			}

			break;		

	}


	return moved;


}


void NextPill(unsigned short player)
{
	// random 1, 3 ,5
	do 
	{
		z= 1 + (rand() % 6);
	}
	while ( ( z % 2) == 0 );

	next_pill[player].left=z;


	// random 2, 4, 6
	do
	{	
		z= 1+ (rand() % 6);

	}while ( ( z % 2) == 1 );
	
	next_pill[player].right=z;


	if ( player == PLAYER1 )
	{
		next_pill[player].x = 7;
	}
	else
	{
		next_pill[player].x = 7+24;
	}
	next_pill[player].y = BOTTLE_FIRST_ROW -2 ;

			
	SetTile(next_pill[player].x,next_pill[player].y,  next_pill[player].left);
    SetTile(next_pill[player].x+1,next_pill[player].y, next_pill[player].right);

}

void NewPill(unsigned short player)
{

	
// check if bottle if full

	if ( ( bottle[player].space[3][0] ) || ( bottle[player].space[4][0] ) )
	{	
		round_lost[player]=1;
		return;
	}

	if ( player == PLAYER1 )
	{
		current_pill[player].x = 7;
	}
	else
	{
		current_pill[player].x = 7+24;
	}

	current_pill[player].bottle_x=3;	
	current_pill[player].y = BOTTLE_FIRST_ROW ;	
	current_pill[player].bottle_y=0;


	current_pill[player].orientation = 0;
	current_pill[player].length = 2;

	current_pill[player].left = next_pill[player].left;
	current_pill[player].right = next_pill[player].right;
    
	Fill(current_pill[player].x,current_pill[player].y, 1, 1, current_pill[player].left);
    Fill(current_pill[player].x+1,current_pill[player].y, 1, 1, current_pill[player].right);

 	NextPill(player);
				
}

void LockPill(unsigned short player)
{
	if ( current_pill[player].orientation == HORIZ )
	{
		bottle[player].space[current_pill[player].bottle_x][current_pill[player].bottle_y]=current_pill[player].left;
		bottle[player].space[current_pill[player].bottle_x+1][current_pill[player].bottle_y]=current_pill[player].right;
	}
	else
	{
		// use horizontal tiles for saving color
		bottle[player].space[current_pill[player].bottle_x][current_pill[player].bottle_y-1]=current_pill[player].top;
		bottle[player].space[current_pill[player].bottle_x][current_pill[player].bottle_y]=current_pill[player].bottom;
	}
	



	bottle[player].check_for_sequences=1;
	
}



void CheckMatching4(unsigned short player)
{	

	short unsigned int matching_spaces = 1;
	unsigned short first_column; 
	unsigned short virus_left_column;


	bottle[player].check_for_sequences=0;

	if ( player == PLAYER1 ) 
	{
		first_column=BOTTLE_FIRST_COLUMN;
		virus_left_column=VIRUS_LEFT_COLUMN1; 
	}
	else
	{
		first_column=BOTTLE2_FIRST_COLUMN;
		virus_left_column=VIRUS_LEFT_COLUMN2;
	}


	for ( x=0 ; x< BOTTLE_WIDTH ; x++ )
	{
		for ( y=0 ; y< BOTTLE_DEPTH ; y++ )
		{
			// for evey non blank space
			if ( bottle[player].space[x][y] )
			{
	
				// check remaining of column for matching colors				
				matching_spaces=1;
				for ( j=0; j < BOTTLE_DEPTH ; j++ )
				{
						if ( y+j+1 > BOTTLE_DEPTH ) 
							break; // don't check passed last row

    					if ( CheckColorMatch ( bottle[player].space[x][y+j], bottle[player].space[x][y+j+1]) )
						{
							matching_spaces++;
						}
						else 
						{
							// are we at the end of a matching sequence  ?
							if ( matching_spaces >= 4 )
							{
							
								// don't need j anymore
								for ( j=0 ; j< matching_spaces ; j++)
								{

									SplitPill(player,x,y+j);
									bottle[player].contains_floaters =1;	
									// update virus count is needed
									if ( ( bottle[player].space[x][y+j] >= VIRUS_RED1 ) && ( bottle[player].space[x][y+j] <= VIRUS_YELLOW2 ) )
									{
										bottle[player].virus_count--;
										PrintByte(virus_left_column, VIRUS_LEFT_ROW, bottle[player].virus_count,false);

									}

									
									bottle[player].space[x][y+j]=DISSAPEAR_ANIM_1;
								}

							    TriggerFx(18,0x90,true);
								// start pill dissapear animation sequence
								Fill(first_column+x,BOTTLE_FIRST_ROW+y,1,matching_spaces,DISSAPEAR_ANIM_1);
								bottle[player].dissapear_anim=1;

								bottle[player].check_for_sequences=1;
								bottle[player].sequences_count++;

							}
							// stop searching column
							break;
						}
				}

				// check remaining of row for matching colors

								
				matching_spaces=1;
				for ( j=0; j < BOTTLE_WIDTH ; j++ )
				{
						if ( x+j+1 > BOTTLE_WIDTH ) 
							break; // don't check passed last row

    					if ( CheckColorMatch ( bottle[player].space[x+j][y], bottle[player].space[x+j+1][y]) )
						{
							matching_spaces++;
						}
						else 
						{
							// are we at the end of a matching sequence  ?
							if ( matching_spaces >= 4 )
							{
							
								// don't need j anymore, let;s uze it for something
								for ( j=0 ; j< matching_spaces ; j++)
								{
									// remove pill(s)/parts from bottle 
									SplitPill(player, x+j,y);
									bottle[player].contains_floaters =1;
									// update virus count is needed
									if ( ( bottle[player].space[x+j][y] >= VIRUS_RED1 ) && ( bottle[player].space[x+j][y] <= VIRUS_YELLOW2 ) )
									{
										bottle[player].virus_count--;
										PrintByte(virus_left_column, VIRUS_LEFT_ROW, bottle[player].virus_count,false);
									}
									
						

									bottle[player].space[x+j][y]=DISSAPEAR_ANIM_1;
									
								}


								TriggerFx(18,0x90,true);
								// black out matching color sequence
								Fill(first_column+x,BOTTLE_FIRST_ROW+y,matching_spaces,1,DISSAPEAR_ANIM_1);
								bottle[player].dissapear_anim=1;

								bottle[player].check_for_sequences=1;
								bottle[player].sequences_count++;
							}
	
							// stop searching column

							break;
						}
				}
			}
		}
	}
}




void SplitPill(unsigned short player, unsigned short x , unsigned short y)
{

	unsigned short first_column; 


	if ( player == PLAYER1 ) 
	{
		first_column=BOTTLE_FIRST_COLUMN;
	}
	else
	{
		first_column=BOTTLE2_FIRST_COLUMN;
	}



	bottle[player].check_for_sequences=0;

	switch ( bottle[player].space[x][y] )
	{
		// half pills
		case SINGLE_RED ...SINGLE_YELLOW :		
			break;

		// left half
		case LEFT_RED:
		case LEFT_BLUE:
		case LEFT_YELLOW:
			switch (bottle[player].space[x+1][y])
			{
				// right half
				case RIGHT_RED:
					bottle[player].space[x+1][y]=SINGLE_RED;
					SetTile(first_column+x+1,BOTTLE_FIRST_ROW+y,SINGLE_RED);
					break;
				case RIGHT_BLUE:
					bottle[player].space[x+1][y]=SINGLE_BLUE;
					SetTile(first_column+x+1,BOTTLE_FIRST_ROW+y,SINGLE_BLUE);
					break;
				case RIGHT_YELLOW:
					bottle[player].space[x+1][y]=SINGLE_YELLOW;
					SetTile(first_column+x+1,BOTTLE_FIRST_ROW+y,SINGLE_YELLOW);
					break;
			}
			break;

		case RIGHT_RED:
		case RIGHT_BLUE:
		case RIGHT_YELLOW:
			switch (bottle[player].space[x-1][y])
			{
				
				case LEFT_RED:
					bottle[player].space[x-1][y]=SINGLE_RED;
					SetTile(first_column+x-1,BOTTLE_FIRST_ROW+y,SINGLE_RED);
					break;
				case LEFT_BLUE:
					bottle[player].space[x-1][y]=SINGLE_BLUE;
					SetTile(first_column+x-1,BOTTLE_FIRST_ROW+y,SINGLE_BLUE);
					break;
				case LEFT_YELLOW:
					bottle[player].space[x-1][y]=SINGLE_YELLOW;
					SetTile(first_column+x-1,BOTTLE_FIRST_ROW+y,SINGLE_YELLOW);
					break;
			}
			break;

		case BOTTOM_RED:
		case BOTTOM_BLUE:
		case BOTTOM_YELLOW:
			switch (bottle[player].space[x][y-1])
			{
				// right half
				case TOP_RED:
					bottle[player].space[x][y-1]=SINGLE_RED;
					SetTile(first_column+x,BOTTLE_FIRST_ROW+y-1,SINGLE_RED);
					break;
				case TOP_BLUE:
					bottle[player].space[x][y-1]=SINGLE_BLUE;
					SetTile(first_column+x,BOTTLE_FIRST_ROW+y-1,SINGLE_BLUE);
					break;
				case TOP_YELLOW:
					bottle[player].space[x][y-1]=SINGLE_YELLOW;
					SetTile(first_column+x,BOTTLE_FIRST_ROW+y-1,SINGLE_YELLOW);
					break;
			}
			break;

		case TOP_RED:
		case TOP_BLUE:
		case TOP_YELLOW:
			switch (bottle[player].space[x][y+1])
			{
				// right half
				case BOTTOM_RED:
					bottle[player].space[x][y+1]=SINGLE_RED;
					SetTile(first_column+x,BOTTLE_FIRST_ROW+y+1,SINGLE_RED);
					break;
				case BOTTOM_BLUE:
					bottle[player].space[x][y+1]=SINGLE_BLUE;
					SetTile(first_column+x,BOTTLE_FIRST_ROW+y+1,SINGLE_BLUE);
					break;
				case BOTTOM_YELLOW:
					bottle[player].space[x][y+1]=SINGLE_YELLOW;
					SetTile(first_column+x,BOTTLE_FIRST_ROW+y+1,SINGLE_YELLOW);
					break;
			}
			break;
	}

}

void DropFloaters(unsigned short player)
{

	bottle[player].contains_floaters=0;

	for ( y=BOTTLE_DEPTH-2 ; y > 0 ; y--  )
	{
		for ( x=0 ; x< BOTTLE_WIDTH ; x++ )
		{
		
			switch (bottle[player].space[x][y])
			{

				case VIRUS_RED1 ... VIRUS_YELLOW2:
					break;
				
				case 0:
					break;

				case SINGLE_RED ... SINGLE_YELLOW:
					// nothing under me ?
					if (  bottle[player].space[x][y+1] == 0 )
					{
						DropFilledSpace(player,x,y);
						bottle[player].contains_floaters=1;	
					}		
					break;

				case LEFT_BLUE:
				case LEFT_RED:
				case LEFT_YELLOW:
					// nothing under me or down right  ?
					if( (  bottle[player].space[x][y+1] == 0  ) &&   (   bottle[player].space[x+1][y+1] == 0  )  )
					{
							DropFilledSpace(player,x,y);
							DropFilledSpace(player,x+1,y);
							bottle[player].contains_floaters=1;
					}		
					break;
		
				case BOTTOM_BLUE:
				case BOTTOM_RED:
				case BOTTOM_YELLOW:
					
					// nothing under me ?
					if (  bottle[player].space[x][y+1] == 0 )
					{

						DropFilledSpace(player,x,y);
						DropFilledSpace(player,x,y-1);
						bottle[player].contains_floaters=1;
					}		
					break;
			}
			
		}
	}



	if ( ! bottle[player].contains_floaters )
	{	
		CheckMatching4(player);

		// send trash if needed
		if ( bottle[player].sequences_count > 1 )
		{
			QueTrash(player, bottle[player].sequences_count);
		}		

		bottle[player].sequences_count = 0;
	}

}

void QueTrash(unsigned short sender, unsigned short trash_count)
{
	unsigned short receiver;

	if ( sender == PLAYER1 )
	{
		receiver = PLAYER2;
	}
	else
	{
		receiver = PLAYER1;
	}
	bottle[receiver].qued_trash += trash_count;
}

void FlingTrash(unsigned short sender)
{
	unsigned short receiver;
	unsigned short first_column; 

	if ( sender == PLAYER2 )
	{
		receiver = PLAYER2;
		first_column=BOTTLE2_FIRST_COLUMN;
	}
	else
	{
		receiver = PLAYER1;
		first_column=BOTTLE_FIRST_COLUMN;
	}


	
	z=0;

	while ( z < bottle[sender].qued_trash   )
	{
			
		x= rand() % BOTTLE_WIDTH ;
		y= 1;

		if ( !  bottle[receiver].space[x][y] ) 
		{	
			bottle[receiver].space[x][y]= SINGLE_RED + (rand() % 3);

			z++;
			SetTile(first_column+x, BOTTLE_FIRST_ROW+y, bottle[receiver].space[x][y]);

		}		
	}
	bottle[sender].qued_trash = 0;
	TriggerFx(22,0x90,true);

	bottle[receiver].contains_floaters=1;

}

void DropFilledSpace(unsigned short player, unsigned short x , unsigned short y)
{

	unsigned short first_column; 


	if ( player == PLAYER1 ) 
	{
		first_column=BOTTLE_FIRST_COLUMN;
	}
	else
	{
		first_column=BOTTLE2_FIRST_COLUMN;
	}

	// move current space 1 dowm	
	bottle[player].space[x][y+1]=bottle[player].space[x][y];
	SetTile(first_column+x,BOTTLE_FIRST_ROW+y+1,bottle[player].space[x][y+1]);

	// blank out current space
	bottle[player].space[x][y]=0;
	SetTile(first_column+x,BOTTLE_FIRST_ROW+y,0);

}


unsigned short CheckColorMatch( unsigned short tile1 , unsigned short tile2 )
{
	switch (tile1)
	{

		case 1 ... 2 :
		case 7 ... 8 :
		case 13:
		case 16 ... 17:
			// red 
			switch (tile2)
			{
				case 1 ... 2 :
				case 7 ... 8 :
				case 13:
				case 16 ... 17:
					return 1;     // RED match
			}
			break;

		case 3 ... 4 :
		case 9 ... 10 :
		case 14:
		case 18 ... 19:
			// blue
			switch (tile2)
			{
				case 3 ... 4 :
				case 9 ... 10 :
				case 14:
				case 18 ... 19:
					return 1;     // BLUE match
			}
			break;

		// must be yellow
		default :
			switch (tile2)
			{
				case 5 ... 6 :
				case 11 ... 12 :
				case 15:
				case 20 ... 21:
					return 1;     // YELLLOW match
			}
			break;
    }

	return 0;
}


unsigned short grab_input(unsigned short player)
{
	static bool holdKey[2];
	unsigned int c;

	c=ReadJoypad(player);

	//check if spin key was released
	if(!(c&BTN_A)) holdKey[player]=false;

	if ( auto_repeat_counter++ < 4 ) 
	{
		return 0;
	}
	auto_repeat_counter=0;

	
	if (c&BTN_A && holdKey[player]==false)
	{		
		holdKey[player]=true;
		return MOVE_SPIN;
		
	}
    else if ( c&BTN_RIGHT )
	{
		return MOVE_RIGHT;
	
	}
	else if ( c&BTN_LEFT )
	{

		return MOVE_LEFT;
	
	} 
	else if ( c&BTN_START )
	{

		return MOVE_START;
	
	} 
	else if ( c&BTN_DOWN )
	{

		return MOVE_DOWN;
	
	}
	
	return 0;

}


void InitBottle(unsigned short player)
{
	unsigned short virus_type;

	unsigned short column,virus_left_column;

	for ( x=0 ; x< BOTTLE_WIDTH ; x++ )
	{
		for ( y=0 ; y< BOTTLE_DEPTH ; y++ )
		{
			bottle[player].space[x][y]=0;
		}
	}

	bottle[player].check_for_sequences=0; 
    bottle[player].virus_count=0;
	bottle[player].contains_floaters=0;
	bottle[player].sequences_count=0;
	bottle[player].dissapear_anim=0;

	if ( player == PLAYER1 )
	{
		column=BOTTLE_FIRST_COLUMN;
		virus_left_column=VIRUS_LEFT_COLUMN1;
	}
	else
	{
		column=BOTTLE2_FIRST_COLUMN;
		virus_left_column=VIRUS_LEFT_COLUMN2;

	}

	
	// desired amount of viruses
	z = 4 * difficulty_level[player] ;

	while ( bottle[player].virus_count < z )
	{ 
	
		x= rand() % BOTTLE_WIDTH;
		// if difficulty is greater than 15, give the viruses more possible space
		if (difficulty_level[player] > 15) {
			y= ( 6 + ( rand() % ( BOTTLE_DEPTH - 6) ) ) ;
		}
		else{
			y= ( 7 + ( rand() % ( BOTTLE_DEPTH - 7) ) ) ;
		}
		virus_type = VIRUS_RED1+(rand() % 5);

		if ( !  bottle[player].space[x][y]) 
		{	
			if (CheckColorMatch (virus_type, bottle[player].space[x-1][y]) && CheckColorMatch (virus_type, bottle[player].space[x-2][y]))
			{}
			else if (CheckColorMatch (virus_type, bottle[player].space[x][y-1]) && CheckColorMatch (virus_type, bottle[player].space[x][y-2]))
			{}
			else if (CheckColorMatch (virus_type, bottle[player].space[x+1][y]) && CheckColorMatch (virus_type, bottle[player].space[x+2][y]))
			{}
			else if (CheckColorMatch (virus_type, bottle[player].space[x][y+1]) && CheckColorMatch (virus_type, bottle[player].space[x][y+2]))
			{}
			else if (CheckColorMatch (virus_type, bottle[player].space[x+1][y]) && CheckColorMatch (virus_type, bottle[player].space[x-1][y]))
			{}
			else if (CheckColorMatch (virus_type, bottle[player].space[x][y+1]) && CheckColorMatch (virus_type, bottle[player].space[x][y-1]))
			{}
			else 
			{
				bottle[player].space[x][y]= virus_type;
			
				bottle[player].virus_count++;
				SetTile(column+x, BOTTLE_FIRST_ROW+y, bottle[player].space[x][y]);
			}
		}
	}



	PrintByte(virus_left_column, VIRUS_LEFT_ROW, bottle[player].virus_count,false);

}




void AnimateVirus()
{

	if ( virus_anim_counter < 15 )
	{
		 virus_anim_counter++;
	}
	else
	{
		virus_anim_counter=0;

		for ( x=0 ; x< BOTTLE_WIDTH ; x++ )
		{
			for ( y=0 ; y< BOTTLE_DEPTH ; y++ )
			{
				switch ( bottle[PLAYER1].space[x][y] )
				{
					case VIRUS_RED1:
					case VIRUS_BLUE1:
					case VIRUS_YELLOW1:

						bottle[PLAYER1].space[x][y]++;
						SetTile(BOTTLE_FIRST_COLUMN + x,BOTTLE_FIRST_ROW  + y ,bottle[PLAYER1].space[x][y]);
						SetTile(BOTTLE_FIRST_COLUMN + x,BOTTLE_FIRST_ROW + y,bottle[PLAYER1].space[x][y]);
						break;

					case VIRUS_RED2:
					case VIRUS_BLUE2:
					case VIRUS_YELLOW2:

						bottle[PLAYER1].space[x][y]--;
						SetTile(BOTTLE_FIRST_COLUMN + x,BOTTLE_FIRST_ROW + y,bottle[PLAYER1].space[x][y]);
						SetTile(BOTTLE_FIRST_COLUMN + x,BOTTLE_FIRST_ROW + y,bottle[PLAYER1].space[x][y]);
						break;

				}

				switch ( bottle[PLAYER2].space[x][y] )
				{
					case VIRUS_RED1:
					case VIRUS_BLUE1:
					case VIRUS_YELLOW1:

						bottle[PLAYER2].space[x][y]++;
						SetTile(BOTTLE2_FIRST_COLUMN + x,BOTTLE_FIRST_ROW  + y ,bottle[PLAYER2].space[x][y]);
						SetTile(BOTTLE2_FIRST_COLUMN + x,BOTTLE_FIRST_ROW + y,bottle[PLAYER2].space[x][y]);
						break;

					case VIRUS_RED2:
					case VIRUS_BLUE2:
					case VIRUS_YELLOW2:

						bottle[PLAYER2].space[x][y]--;
						SetTile(BOTTLE2_FIRST_COLUMN + x,BOTTLE_FIRST_ROW + y,bottle[PLAYER2].space[x][y]);
						SetTile(BOTTLE2_FIRST_COLUMN + x,BOTTLE_FIRST_ROW + y,bottle[PLAYER2].space[x][y]);
						break;

				}

			}
		}

		
	}
}
