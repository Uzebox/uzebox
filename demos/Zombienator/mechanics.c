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

#include "mechanics.h"

//Horrible attempt att minimizing the code, unreadable.
//Handles vertical and horizontal scrolling
//Oh, and also moving the main char.
bool scrollchar(bool direction, CoordStruct* mstruct, unsigned char* c)
{
	bool retvalue = false;
	if(direction) //are we moving down/right
	{
		//Is the main character close to the screen border?
		if((*c) < (mstruct->SCREENTILE - 6) * mstruct->TILE) 
			(*c)++;   
		//Is there enough space left to scroll?
		else if(mstruct->s < (SCROLL_MEMORY_SIZE*mstruct->TILE - mstruct->SCREENTILE*mstruct->TILE))
		{		
			mstruct->s++;
			retvalue = true;
		}
		//Can we change megatile or are we out of map?
		else if(mstruct->p < (mstruct->MAP - SCROLL_MEMORY_SIZE/mstruct->MEGA))
		{
			mstruct->p++;
			mstruct->s -= (mstruct->MEGATILE - 1);
			retvalue = true;
			mstruct->scrolled = 1;
		}
		//Is the main character at the border?
		else if((*c) < (mstruct->SCREENTILE)*mstruct->TILE)
			(*c)++;
	}
	else //or up/left
	{
		//Is the main character close to the screen border?
		if((*c) > (mstruct->TILE<<3))
			(*c)--;
		//Is there enough space left to scroll?
		else if(mstruct->s > 0)
		{
			mstruct->s--;
			retvalue = true;
		}
		//Can we change megatile or are we out of map?
		else if(mstruct->p > 0)
		{
			mstruct->p--;
			mstruct->s += (mstruct->MEGATILE - 1);
			retvalue = true;
			mstruct->scrolled = -1;
		}
		//Is the main character at the border?
		else if((*c) > mstruct->TILE)
			(*c)--;
	}
	return retvalue;
}

void movechar(bool direction, unsigned char* c, CoordStruct* mstruct)
{
	if(direction && (*c) < (mstruct->SCREENTILE)*mstruct->TILE)
		(*c)++; 
	else if((*c) > mstruct->TILE)
		(*c)--;
}


//Adjusts coordinates after scrolling has occured
void postoscroll(ItemStruct* item, MainStruct* main) 
{
	item->x -= ((main->x.p * main->x.MEGATILE) + main->x.s - item->sx);
	item->y -= ((main->y.p * main->y.MEGATILE) + main->y.s - item->sy);
	item->sx = (main->x.p * main->x.MEGATILE) + main->x.s;
	item->sy = (main->y.p * main->y.MEGATILE) + main->y.s;
}

void postotile(MainStruct* mains, int* x, int* y)
{
	*x = (((*x + mains->x.s) / mains->x.TILE) - 1);
	*y = (((*y + mains->y.s) / mains->y.TILE) - 1);
}

//Collision detection against the background image.
//Reads the vram to get the tombstone coordinates.
unsigned char mapcollide(MainStruct* mains, int x, int y)
{
	unsigned char maptile, i;

	//convert pixel values to tile values.
	postotile(mains, &x, &y);

	maptile = vram[y*32 + x]; //I do hope I'm choosing the right tile here.
	
	if(maptile == 4
	|| maptile == 5
	|| maptile == 6
	|| maptile == 7) //tombstone tile numbers.
		return 1;

	for(i = 15; i < 63; i++)
	{
		if (maptile == i)
		{
			if(maptile <= 35)
				return 2;
			return 1;
		}
	}

		return false;
}

//zombies cannot spawn everywhere you know.
int findgrave(MainStruct *mains)
{
	unsigned int gravetile;
	gravetile = rand() % 1024; // size of vram when one-dimensional

	while(vram[gravetile] != 4
	&&	vram[gravetile] != 5
	&&	vram[gravetile] != 6
	&&	vram[gravetile] != 7) // not tombstone
	{
		gravetile+=2; // tombstones are always 2 tiles wide. (I think..)
		if(gravetile >= 959) // no overflow plz (1023 - 64 (if the spawnposition happens to be two rows down))
			gravetile = 0;
	}

	return gravetile;
}

bool powercollide(MainStruct* mains, PlayerStruct* player, PowerStruct* power)
{
	int x, y;
	x = player->x;
	y = player->y + TILE_HEIGHT;
	postotile(mains, &x, &y);
	if(x == (power->megax - mains->x.p)*MEGATILE_WIDTH + power->x &&
	   y == (power->megay - mains->y.p)*MEGATILE_HEIGHT + power->y)
		return true;

	return false;
}

void powerupper(MainStruct* mains, GameStruct* game, PlayerStruct players[])
{
	unsigned char i, p;
	if(rand()%1000 > 950)
	{
		for(i = 0; i < MAX_POWERUPS; i++)
		{
			if(game->powerups[i].type == 0)
			{
				game->powerups[i].megax = rand()%MAP_MEGA_WIDTH;
				game->powerups[i].megay = rand()%MAP_MEGA_HEIGHT;
				game->powerups[i].x = rand()%MEGATILE_WIDTH;
				game->powerups[i].y = rand()%MEGATILE_HEIGHT;
				game->powerups[i].type = 1;
				game->powerups[i].size = 20;
				game->powerups[i].tile = 66;
				break;
			}
		}
	}

	for(p = 0; p < mains->players; p++)
	{
		for(i = 0; i < MAX_POWERUPS; i++)
		{
		
			if(game->powerups[i].type == 0)
				continue;
			else if(powercollide(mains, &players[p], &(game->powerups[i])))
			{
				
				if(players[p].guns[game->powerups[i].type].ammo < (100 - game->powerups[i].size))
				{
					players[p].guns[game->powerups[i].type].ammo += game->powerups[i].size;
					erasepup(mains, (&game->powerups[i]));
					memset((&game->powerups[i]), 0, sizeof(PowerStruct));
				}
			}
		}
	}

}
