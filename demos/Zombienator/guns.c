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

#include "guns.h"


void changegun(PlayerStruct* player)
{
	if(++(player->gunnr) > 1)
	{
		player->gunnr = 0;
		sprites[player->shots[2].sprite.nrsprite].x = sprites[player->shots[2].sprite.nrsprite].y = DISABLED_SPRITE;
	}
	for(int i = 0; i < 3; i++)
		player->shots[i].x = player->shots[i].y = DISABLED_SPRITE;
	
}

//For shooting at zombies
void shoot(PlayerStruct* player)
{
	if(player->gundelay < player->guns[player->gunnr].delay || player->guns[player->gunnr].ammo <= 0)
		return;

	switch(player->gunnr)
	{
	case 0:
		shoot_shotgun(player);
		return;
	case 1:
		shoot_flamethrower(player);
		return;
	}
}

void addshot(ItemStruct* shot, PlayerStruct* player)
{
	shot->y = player->y;
	shot->x = player->x;
	shot->dir = player->sprite.body;
	player->gundelay = 0;
	sprites[shot->sprite.nrsprite].tileIndex = shot->sprite.nranim;

	switch(player->sprite.body) //we want the shot to appear infront of our hero, not on him.
	{
	case 0:
		shot->y += 5;
		break;
	case 1:
		shot->y -= 5;
		break;
	case 2:
		shot->x -= 3;
		break;
	case 3:
		shot->x += 3;
		break;
	}
}

void shoot_shotgun(PlayerStruct* player)
{	
	unsigned char nr;

	//are any sprite slots free?
	if(player->shots[0].y >= DISABLED_SPRITE)
		nr = 0;
	else if(player->shots[1].y >= DISABLED_SPRITE)
		nr = 1;
	else
		return;

	if(player->shots[nr].y == DISABLED_SPRITE) //if the sprite isn't in use
	{
		addshot(&(player->shots[nr]), player);
		sprites[player->shots[nr].sprite.nrsprite].tileIndex = GUN_SPRITE;
		TriggerFx(18,0x90,true); //BOOM CHICK-CHICK
	}
}

void shoot_flamethrower(PlayerStruct* player)
{
	static unsigned char nr = 0;

	if(nr >= 2)
	{
		TriggerFx(25,0x90,true); //FROOSCH
		nr = 0;
	}
	else 
	{
		nr++;
	}

	addshot(&(player->shots[nr]), player);
	player->shots[nr].x += (nr - 1);
	player->shots[nr].y += ((nr - 1)<<1);
	sprites[player->shots[nr].sprite.nrsprite].tileIndex = GUN_SPRITE + 1;
	player->guns[1].ammo--;

	return; 
}

//Keeps the shots going until they hit something
void handleshot(MainStruct* mains, GameStruct* game, PlayerStruct* player, ItemStruct zombies[MAX_ZOMBIES])
{
	switch(player->gunnr)
	{
	case 0:
		handleshot_shotgun(mains, game, player, zombies);
		return;
	case 1:
		handleshot_flamethrower(mains, game, player, zombies);
		return;
	}
}

void checkshot(MainStruct* mains, GameStruct* game, PlayerStruct* player, ItemStruct zombies[MAX_ZOMBIES], ItemStruct* shot)
{
	unsigned char deltax, deltay, shotkills;
	deltax = mains->x.TILE;
	deltay = mains->y.TILE;
	shotkills = 0;

	postoscroll(shot, mains);
	for(int i = 0; i<game->zombies; i++)
	{
		if(zombies[i].kill > 0)
			continue;

		//HIT!
		if(abs(shot->y - zombies[i].y) < deltay && abs(shot->x - zombies[i].x) < deltax)
		{
			zombies[i].kill = 1 + 3 * player->gunnr;
			player->killz++;
			player->conskills++;
			shotkills++;

			if(shotkills == 0)
			{
				i = 0;
				deltax += 4;
				deltay += 5;
			}
			else if(shotkills >= 3)
				break;
		}
			
	}

	if(shotkills)
	{
		shot->y = shot->x = DISABLED_SPRITE;
		player->flagcount = 0;
		game->flagcount = 0;

		if(player->conskills >= 8) //INSANE!!!11111
		{
			game->flags |= GAMEMULTIHO|GAMEMULTILO;
			player->killz += 15 * shotkills;
			TriggerFx(22,0x90,false); //BLIDIBLONG
		}
		else if(player->conskills >= 3) //TRIPLE!!
		{
			game->flags |= GAMEMULTIHO;
			game->flags &= ~GAMEMULTILO;
			player->killz += 8 * shotkills;
			TriggerFx(21,0x90,false); //BLODIBLING
		} 
		else if(player->conskills == 2) //DOUBLE!
		{
			game->flags &= ~GAMEMULTIHO;
			game->flags |= GAMEMULTILO;
			player->killz += 4 * shotkills;
			TriggerFx(20,0x90,false); //BLING!
		}
		else //meh.
			TriggerFx(19,0x90,false); //SPLOSCH
			
	}
}

void handleshot_shotgun(MainStruct* mains, GameStruct* game, PlayerStruct* player, ItemStruct zombies[MAX_ZOMBIES])
{
	ItemStruct* shot;
	for(int u = 0; u < 2; u++)
	{
		shot =  &(player->shots[u]);
		checkshot(mains, game, player, zombies, shot);
		switch(shot->dir) //move it.
		{
		case 0:
			shot->y+=5;
			break;
		case 1:
			shot->y-=5;
			break;
		case 2:
			shot->x-=3;
			break;
		case 3:
			shot->x+=3;
			break;
		}

		//disable the sprite if the shot hits a tombstone
		//or if it is offscreen.
		if(shot->y < 0
		   || shot->x < 0
		   || shot->y > SCREEN_HEIGHT
		   || shot->x > SCREEN_WIDTH
		   || (mapcollide(mains, shot->x+1, shot->y+3)
		       &&  mapcollide(mains, shot->x+4, shot->y+8)))
			shot->y = shot->x = DISABLED_SPRITE;
		

		drawsprite(shot);	
	}
}

void handleshot_flamethrower(MainStruct* mains, GameStruct* game, PlayerStruct* player, ItemStruct zombies[MAX_ZOMBIES])
{
	ItemStruct* shot;
	for(int u = 0; u < 3; u++)
	{
		shot =  &(player->shots[u]);
		checkshot(mains, game, player, zombies, shot);
		switch(shot->dir) //move it.
		{
		case 0:
			shot->y+=3;
			break;
		case 1:
			shot->y-=3;
			break;
		case 2:
			shot->x-=2;
			break;
		case 3:
			shot->x+=2;
			break;
		}

		//disable the sprite if the shot hits a tombstone
		//or if it is offscreen.
		if(shot->y < 0
		   || shot->x < 0
		   || shot->y > SCREEN_HEIGHT
		   || shot->x > SCREEN_WIDTH
		   || (mapcollide(mains, shot->x+1, shot->y+3)
		       &&  mapcollide(mains, shot->x+4, shot->y+8))
		   || player->gundelay > (FLAMETHROWER_DELAY<<1))
			shot->y = shot->x = DISABLED_SPRITE;
		

		drawsprite(shot);	
	}
	return;
}
