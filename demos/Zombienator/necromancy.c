/*
 *  Zombienator
 *  Copyright (C) 2009 Peter Hedman
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
*/

#include "necromancy.h"

//Early attempt at making zombies explode into blood and gore and guts when shot at.
void gorify(ItemStruct* zombie)
{
	//have we had 3 frames of splatter?
	if(zombie->kill == 3)
	{
		sprites[zombie->sprite.nrsprite].x = sprites[zombie->sprite.nrsprite+1].x = DISABLED_SPRITE;
		sprites[zombie->sprite.nrsprite].y = sprites[zombie->sprite.nrsprite+1].y = DISABLED_SPRITE;
		return;
	}
	//is the slowdown timer complete?
	else if(zombie->sprite.animdelay == 6)
	{
		zombie->sprite.animdelay = 0;
		zombie->kill++;
	} 
	else
		zombie->sprite.animdelay++;

	sprites[zombie->sprite.nrsprite].tileIndex=zombie->sprite.nranim + 12 + zombie->kill;
	sprites[zombie->sprite.nrsprite+1].tileIndex=zombie->sprite.nranim + 15 + zombie->kill;
}

void movezombie(ItemStruct* zombie, MainStruct* main, int deltax, int deltay)
{
	if(abs(deltax) > abs(deltay))
	{
		if(deltax > 0)
		{
			if(!mapcollide(main, zombie->x + 1, zombie->y + 13))
			zombie->x--;
			else if(deltay >= 0)
			zombie->y--;
			else
			zombie->y++;

			zombie->sprite.body = 2;
			zombie->sprite.legs = 5;
		}
		else if(deltax < 0)
		{
			if(!mapcollide(main, zombie->x + 5, zombie->y + 13))
			zombie->x++;
			else if(deltay >= 0)
			zombie->y--;
			else
			zombie->y++;

			zombie->sprite.body = 3;
			zombie->sprite.legs = 8;
		}
	}
	else
	{
		if(deltay > 0)
		{
			if(!mapcollide(main, zombie->x + 3, zombie->y + 11))
			zombie->y--;
			else if(deltax >= 0)
			zombie->x--;
			else
			zombie->x++;

			zombie->sprite.body = 0;
			zombie->sprite.legs = 11;
		}
		else if(deltay < 0)
		{
			if(!mapcollide(main, zombie->x + 3, zombie->y + 14))
			zombie->y++;
			else if(deltax >= 0)
			zombie->x--;
			else
			zombie->x++;

			zombie->sprite.body = 1;
			zombie->sprite.legs = 11;
		}
	}

	//keep the animations constant when going diagonally
	if(abs(abs(deltax)-abs(deltay)) < 2) 
	{
		if(deltax > 0)
			zombie->sprite.legs = 5;
		else if(deltax < 0)
			zombie->sprite.legs = 8;

		if(deltay > 0)
			zombie->sprite.body = 0;
		else if(deltay < 0)
			zombie->sprite.body = 1;
	}
}

void burnzombie(ItemStruct* zombie, MainStruct* main)
{
	sprites[zombie->sprite.nrsprite].tileIndex=zombie->sprite.nranim + 20 + (((zombie->kill - 3)>>2)&1);
	if(zombie->kill > 13)
		sprites[zombie->sprite.nrsprite + 1].tileIndex=zombie->sprite.nranim + 20 + (((zombie->kill - 3)>>2)&1);
	if(zombie->kill > 20)
		sprites[zombie->sprite.nrsprite].x = sprites[zombie->sprite.nrsprite].y = DISABLED_SPRITE;


	switch(zombie->sprite.body)
	{
	case 0:
		if(!mapcollide(main, zombie->x + 3, zombie->y + 11))
		zombie->y--;
		break;
	case 1:
		if(!mapcollide(main, zombie->x + 3, zombie->y + 14))
		zombie->y++;
		break;
	case 2:
		if(!mapcollide(main, zombie->x + 1, zombie->y + 13))
		zombie->x--;
		break;
	case 3:
		if(!mapcollide(main, zombie->x + 5, zombie->y + 13))
		zombie->x++;
		break;
	}

	if(++zombie->kill == ZOMBIEFIRE)
		zombie->kill = 3;
}

//All mighty zombie-super-AI-function
//returns true if the zombie found some tasty hero-brains
unsigned char handlezombie(ItemStruct* zombie, PlayerStruct players[], MainStruct* main)
{
	int deltax[2];
	int deltay[2];
	unsigned char retvalue;
	retvalue = 0;

	//ouch a shotgun wound in my head!
	if(zombie->kill > 0 && zombie->kill < 4)
	{
		gorify(zombie);	
		return retvalue;
	}
	animchar(&zombie->sprite);
	
	//where are the closest tasty brains?
	for(int p = 0; p < main->players; p++)
	{
		if(players[p].life > 0)
		{
			deltax[p] = zombie->x - players[p].x; 
			deltay[p] = zombie->y - players[p].y;
		}
		else
			deltax[p] = deltay[p] = 255;
	}

	if(zombie->kill < 0) //if the zombie is rising from his grave
	{
		zombie->sprite.body = 0;
		zombie->sprite.legs = 10;
		zombie->kill++;
	}
	else if(zombie->kill > 3) //ARGH! THE HEAT!!
	{
		burnzombie(zombie, main);
	}
	else 
	{
		zombie->sprite.body = 0;
		zombie->sprite.legs = 0; 
		if(abs(deltax[0]) + abs(deltay[0]) < abs(deltax[1]) + abs(deltay[1]) || main->players == 1)
			movezombie(zombie, main, deltax[0], deltay[0]);
		else
			movezombie(zombie, main, deltax[1], deltay[1]);
	}	

	//are we close enough for tasty brains?
	if(abs(deltax[0]) < 6 && abs(deltay[0]) < 7)
		retvalue |= 1;
	if(abs(deltax[1]) < 6 && abs(deltay[1]) < 7)
		retvalue |= 2;

	return retvalue;
}

//ABRA-KADABRA AND THERE WERE ZOMBIES
void summon(ItemStruct* zombie, MainStruct* mains)
{
	unsigned int spawntile = findgrave(mains); //returns random tombstone tile.
	
	//calculate x and y position
	zombie->x = (((spawntile & 31) + 1) * TILE_WIDTH) - mains->x.s;
	zombie->y = (((spawntile >> 5) + 1) * TILE_HEIGHT) - mains->y.s;

	//we want to spawn below the tombstone
	while(mapcollide(mains, zombie->x + 5, zombie->y + 13))
		zombie->y+=2;

	zombie->kill = -18;
	zombie->y++;

	//the zombie animation sprites are after the mainchar animations
	zombie->sprite.nranim = MAINCHAR_SIZE + 1;
	zombie->sprite.animcount = 1;
	zombie->sprite.speed = 1;
}


//CAN WE SUMMON UP SOME FORCES OF DARKNESS? MUAHAHAHAHHAHAHA
void ritual(ItemStruct zombie[MAX_ZOMBIES], GameStruct* game, MainStruct *mains)
{
	if(game->zombies < MAX_ZOMBIES) //if we still havent filled the entire zombie array
	{
		summon(&zombie[game->zombies], mains);

		zombie[game->zombies].sprite.nrsprite = zombie[(game->zombies)-1].sprite.nrsprite + 2;
		game->zombies++;
	}
	else //look for dead zombies in the array.
	{
		for(int i = 0; i < game->zombies; i++)
		{
			if(zombie[i].kill == 3)
			{	
				summon(&zombie[i], mains);
				break;
			}
		}
	}
}

//General zombie management.
void necromancer(ItemStruct zombie[MAX_ZOMBIES], MainStruct *mains, GameStruct *game, PlayerStruct players[])
{
	unsigned char p;
	unsigned char hit = 0;

	//always update the scrolling positions and draw the zombies
	for(int i = 0; i < game->zombies; i++)
	{
			postoscroll(&zombie[i], mains);

			if(zombie[i].y + mains->y.s < 0
			|| zombie[i].x + mains->x.s < 0
			|| zombie[i].y + mains->y.s > mains->y.TILE<<5
			|| zombie[i].x + mains->x.s > mains->x.TILE<<5)
			zombie[i].kill = 3;

			if(zombie[i].kill == 3)
				continue;

			drawsprite(&zombie[i]);
	}

	
	if((game->frames % 3) != 0) //don't handle the zombies every frame.
	{
		for(int i = 0; i < game->zombies; i++)
		{	
			hit |= handlezombie(&zombie[i], players, mains);
		}
	}
	else if(rand()%1000 > 910) //handle spawning once every three frames
		ritual(zombie, game, mains);

	
	//have we managed to eat some tasty brains?
	for(p = 0; p < mains->players; p++)
	{
		if((hit&(p+1)) && players[p].life > 0 && !(players[p].flags & PLAYERHURT))
		{
			players[p].life--;
			players[p].flags |= PLAYERHURT;
			players[p].hurtcount = 0;
			if(players[p].life <= 0) 
			{
				TriggerFx(24,0x90,false);
				players[p].x = players[p].y = DISABLED_SPRITE;
			}
			else
				TriggerFx(23,0x90,false);
		}
	}
}
