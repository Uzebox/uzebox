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

#include "videoengine.h"

//const char scorestr[] PROGMEM = "SCORE:";
const char pausestr[] PROGMEM = "PAUSED";
const char doublestr[] PROGMEM = "DOUBLE!";
const char triplestr[] PROGMEM = "MULTI!";
const char insanestr[] PROGMEM = "INSANE!";
const char overstr[] PROGMEM = "GAME OVER";
const char suboverstr[] PROGMEM =  "RETRY?";
const char suboverstr2[] PROGMEM = "MENU?";
const char choicestr[] PROGMEM = ">";

void fillmap(unsigned char x, unsigned char y, unsigned char width, unsigned char height)
{       
	for(unsigned char i = x; i < x + width; i++)
	{
		for(unsigned char u = y; u < y + height; u++)
			SetTile(i, u, ((i&1)|((u&1)<<1)));
	}
}

void* progread(uint32_t* from, uint32_t* to, unsigned char amount)
{
	unsigned char i;
	for(i = 0; i < amount; i++)
		*(to + i) = pgm_read_dword(from + i);
	return (void*)to;
}

void drawpups(MainStruct* mains, GameStruct* game)
{
	for(int i = 0; i < MAX_POWERUPS; i++)
	{
		if(game->powerups[i].type == 0)
			continue;
		else 
		{
			int pupx = (game->powerups[i].megax - mains->x.p)*MEGATILE_WIDTH + game->powerups[i].x;
			int pupy = (game->powerups[i].megay - mains->y.p)*MEGATILE_HEIGHT + game->powerups[i].y;
			if(pupx < 32 && pupy < 32 && pupx >= 0 && pupy >= 0)
			{
				if(vram[(pupy + mains->players)*32 + pupx] != 0 && 
				   vram[(pupy + mains->players)*32 + pupx] != 1 && 
				   vram[(pupy + mains->players)*32 + pupx] != 2 &&
				   vram[(pupy + mains->players)*32 + pupx] != 3 &&
				   vram[(pupy + mains->players)*32 + pupx] != game->powerups[i].tile)
				{
					memset(&(game->powerups[i]), 0, sizeof(PowerStruct));
				     	return;
				}
				SetTile(pupx, pupy + mains->players, game->powerups[i].tile);
			}
		}
	}
}

void erasepup(MainStruct* mains, PowerStruct* power)
{
	int pupx = (power->megax - mains->x.p)*MEGATILE_WIDTH + power->x;
	int pupy = (power->megay - mains->y.p)*MEGATILE_HEIGHT + power->y;
	if(pupx < 32 && pupy < 32 && pupx >= 0 && pupy >= 0)
		fillmap(pupx, pupy + mains->players, 1, 1);
}

void drawpath(unsigned char x, unsigned char y, uint16_t data)
{
	uint8_t i, temp;
	
	for(i = 0; i < 4; i++)
	{
		temp = (((i&2) + 2)<<1) | ((i&1) + 1); 
		switch(FLAGS(data)&temp)
		{
		case LEFTBORDER:
		case RIGHTBORDER:
			SetTile(x + X(data) + (i&1), y + Y(data) + (i>>1), 10 + ((i&2)>>1));
			break;
		case TOPBORDER:
		case BOTTOMBORDER:
		case 0:
			SetTile(x + X(data) + (i&1), y + Y(data) + (i>>1), 8 + (i&1));
			break;
		default:
			SetTile(x + X(data) + (i&1), y + Y(data) + (i>>1), 12);
		}
	}

	if(FLAGS(data)&BOTTOMBORDER)
		for(temp = y + Y(data) + 2; temp < y + MEGATILE_HEIGHT; temp++) 
		{
			SetTile(x + X(data), temp, 8);
			SetTile(x + 1 + X(data), temp, 9);
		}
	if(FLAGS(data)&TOPBORDER)
	{
		temp = y + Y(data);
		do
		{
			temp--;
			SetTile(x + X(data), temp, 8);
			SetTile(x + 1 + X(data), temp, 9);
		}while(temp > y);
	}
	if(FLAGS(data)&RIGHTBORDER)
       		for(temp = x + X(data) + 2; temp < x + MEGATILE_WIDTH; temp++)
		{
			SetTile(temp, y + Y(data), 10);
			SetTile(temp, y + 1 + Y(data), 11);
		}
	if(FLAGS(data)&LEFTBORDER)
	{
		temp = x + X(data);
		do
		{
			temp--;
			SetTile(temp, y + Y(data), 10);
			SetTile(temp, y + 1 + Y(data), 11);
		}while(temp > x);
	}
	   
}

void drawmegatile(unsigned char x, unsigned char y, uint16_t* tiledata)
{
	for(uint8_t i = 0;; i++)
	{
		if(TYPE(tiledata[i]) == 1)
		{
			DrawMap2(x + X(tiledata[i]), y + Y(tiledata[i]), tombstone);
		}
		if(TYPE(tiledata[i]) == 2)
		{
			drawpath(x, y, tiledata[i]);
		}
		if(TYPE(tiledata[i]) == 3)
		{
			DrawMap2(x + X(tiledata[i]), y + Y(tiledata[i]), bush);
		}
		if(TYPE(tiledata[i]) == 4)
		{
			DrawMap2(x + X(tiledata[i]), y + 3 +Y(tiledata[i]), loweruptower);
			DrawMap2(x + 2 + X(tiledata[i]), y + Y(tiledata[i]), middletower);
		}
		if(TYPE(tiledata[i]) == 5)
		{
			DrawMap2(x + X(tiledata[i]), y + Y(tiledata[i]), churchmiddle);
		}
		if(TYPE(tiledata[i]) == 6)
		{
			DrawMap2(x + X(tiledata[i]), y + Y(tiledata[i]), churchlower);
		}
		if(END(tiledata[i]))
			break;
	}
}

void rendermap(MainStruct* mains)
{
	unsigned char megax, megay, i;
	MapStruct** mapptr;
	MapStruct* ptr;
	uint32_t pointertemp[4], largetemp[3];
	megax = megay = i = 0;

	if(mains->y.scrolled)
	{
		if(mains->y.scrolled == -1)
		{
			memmove(&vram[(mains->players + MEGATILE_HEIGHT)*SCROLL_MEMORY_SIZE],
				&vram[mains->players*SCROLL_MEMORY_SIZE],
				SCROLL_MEMORY_SIZE*(SCROLL_MEMORY_SIZE - MEGATILE_HEIGHT));
			megay = 0;
		}
		else if(mains->y.scrolled == 1)
		{
			memmove(&vram[(mains->players)*SCROLL_MEMORY_SIZE],
				&vram[(mains->players + MEGATILE_HEIGHT)*SCROLL_MEMORY_SIZE],
				SCROLL_MEMORY_SIZE*(SCROLL_MEMORY_SIZE - MEGATILE_HEIGHT));
			megay = 7;
		}

		fillmap(0, (megay<<2) + mains->players, SCROLL_MEMORY_SIZE, MEGATILE_HEIGHT);
		for(megax = 0; megax < 4; megax++)
		{
			ptr = (MapStruct*)pgm_read_word(&graveyard_map[megax + mains->x.p][mains->y.p + megay]);
			drawmegatile((megax<<3), (megay<<2) + mains->players,
				     (uint16_t*)progread((uint32_t*)ptr, largetemp, 3));
		}
	}
	if(mains->x.scrolled)
	{
		if(mains->x.scrolled == -1)
		{
			memmove(&vram[mains->players*SCROLL_MEMORY_SIZE + MEGATILE_WIDTH],
					&vram[mains->players*SCROLL_MEMORY_SIZE],
					SCROLL_MEMORY_SIZE*SCROLL_MEMORY_SIZE - MEGATILE_WIDTH);
			megax = 0;
		}
		else if(mains->x.scrolled == 1)
		{
			memmove(&vram[mains->players*SCROLL_MEMORY_SIZE],
					&vram[mains->players*SCROLL_MEMORY_SIZE + MEGATILE_WIDTH],
					SCROLL_MEMORY_SIZE*SCROLL_MEMORY_SIZE - MEGATILE_WIDTH);
			megax = 3;
		}

		fillmap((megax<<3), mains->players, MEGATILE_WIDTH, SCROLL_MEMORY_SIZE);
		mapptr = (MapStruct**)progread((uint32_t*)&graveyard_map[mains->x.p + megax][mains->y.p], pointertemp, 4);
		for(megay = 0; megay < 8; megay++)
			drawmegatile((megax<<3), (megay<<2) + mains->players,
				     (uint16_t*)progread((uint32_t*)mapptr[megay],	largetemp, 3));
	}
	if(mains->y.scrolled == 127 && mains->x.scrolled == 127)
	{
		fillmap(0, mains->players, SCROLL_MEMORY_SIZE, SCROLL_MEMORY_SIZE + mains->players);
		for(megax = 0; megax < 4; megax++)
		{
			mapptr = (MapStruct**)progread((uint32_t*)&graveyard_map[megax + mains->x.p][mains->y.p], pointertemp, 4);
			for(megay = 0; megay < 8; megay++)
				drawmegatile((megax<<3), (megay<<2) + mains->players,
					     (uint16_t*)progread((uint32_t*)mapptr[megay],	largetemp, 3));
		}
	}
	
	mains->y.scrolled = 0;
	mains->x.scrolled = 0;
}

//All animations use this one.
void animchar(AnimStruct* anim)
{
	if(anim->legs == 0) //no movement.
	{
		if(anim->animcount != 0)
		{
			//revert the animations to the standing position.
			//sprites[anim->nrsprite+1].tileIndex -= (anim->animcount - anim->speed) * anim->speed;
			anim->animcount = 0;
		}
		sprites[anim->nrsprite].tileIndex=anim->nranim + anim->body;
		return;
	}
	else if(anim->animdelay >= 4) //slowing down the animation
	{
		anim->animdelay = 0;
       		anim->animcount = (anim->animcount < 3 && anim->animcount > -3) ? anim->animcount + anim->speed: 0;
	} 
	else
		anim->animdelay++;

	sprites[anim->nrsprite].tileIndex=anim->nranim + anim->body; //upper body
	sprites[anim->nrsprite+1].tileIndex=anim->nranim + anim->legs; //lower body
	sprites[anim->nrsprite+1].tileIndex+=(anim->animcount != 3 && anim->animcount != -3) ? anim->animcount - anim->speed: 0;
}

//Overlay, what can I say? Score, ammunition, health left, etc.
void drawoverlay(GameStruct* game, PlayerStruct* player)
{
  	Fill(0, OVERLAYROW, 24, 1, 13); //black

	//score
	//	Print(11, OVERLAYROW, scorestr); 
	SetTile(16, OVERLAYROW, 65);
	PrintInt(21, OVERLAYROW, player->killz, true);


	PrintByte(14, OVERLAYROW, player->guns[player->gunnr].ammo, true);
	SetTile(12, OVERLAYROW, player->guns[player->gunnr].icon);
	

	if(player->life <= 0)
	{
		Fill(0, OVERLAYROW + 1, 24, 2, 13); //black
		Print(0, OVERLAYROW, overstr); //game over
		Print(5, OVERLAYROW + 1, suboverstr); //retry?
		Print(5, OVERLAYROW + 2, suboverstr2); //menu?
		Print(4, OVERLAYROW + 1 + game->menupos, choicestr);
	}
	else
	{
		if(game->flags & GAMEPAUSED)
			Print(4, OVERLAYROW, pausestr); //paused
		else if(game->flags & (GAMEMULTIHO|GAMEMULTILO) && game->frames&2)
		{
			switch(game->flags&(GAMEMULTIHO|GAMEMULTILO))
			{
			case GAMEMULTILO:
				Print(3, OVERLAYROW, doublestr); //DOUBLE
				break;
			case GAMEMULTIHO:
				Print(3, OVERLAYROW, triplestr); //TRIPLE
				break;
			case GAMEMULTIHO|GAMEMULTILO:
				Print(3, OVERLAYROW, insanestr); //INSANE!!
				break;
			}
		}

		//lives left.
		for(int i = 0; i < player->life; i++)
			SetTile(i, OVERLAYROW, 14);
	}
}

void multidrawoverlay(GameStruct* game, PlayerStruct players[2])
{
	Fill(0, OVERLAYROW, 24, 2, 13); //black

	for(int p  = 0; p < 2; p++)	//score
	{
		//Print(11 * p, OVERLAYROW, scorestr); 
		SetTile(11 * p, OVERLAYROW, 65);
		PrintInt(5 + 11 * p, OVERLAYROW, players[p].killz, true);
		PrintByte(10 + 11 * p, OVERLAYROW, players[p].guns[players[p].gunnr].ammo, true);
		SetTile(8 + 11 * p, OVERLAYROW, players[p].guns[players[p].gunnr].icon);
	}

	if(players[0].life <= 0 && players[1].life <= 0)
	{
		Fill(0, OVERLAYROW + 1, 24, 2, 13); //black
		Print(8, OVERLAYROW + 2, overstr); //game over
		Print(1, OVERLAYROW + 1, suboverstr); //retry?
		Print(1, OVERLAYROW + 2, suboverstr2); //menu?
		Print(0, OVERLAYROW + 1 + game->menupos, choicestr);
	}
	else
	{
		for(int p = 0; p < 2; p++)
		{
			if(game->flags & GAMEPAUSED)
				Print(4, OVERLAYROW + 1, pausestr); //paused
			else if(game->flags & (GAMEMULTIHO|GAMEMULTILO) && game->frames&2)
			{
				switch(game->flags&(GAMEMULTIHO|GAMEMULTILO))
				{
				case GAMEMULTILO:
					Print(3, OVERLAYROW+1, doublestr); //DOUBLE
					break;
				case GAMEMULTIHO:
					Print(3, OVERLAYROW+1, triplestr); //TRIPLE
					break;
				case GAMEMULTIHO|GAMEMULTILO:
					Print(3, OVERLAYROW+1, insanestr); //INSANE!!
					break;
				}
			}

			//lives left.
			for(int i = 0; i < players[p].life; i++)
				SetTile(i + 11 * p, OVERLAYROW + 1, 14);
		}
	}
}

//Sprite drawing function, handles shots and zombies
void drawsprite(ItemStruct* item)
{
	unsigned char tempx, tempy;

	//the sprite positions are unsigned chars, we don't want over/underflow
	if(( 255 >= item->x ) && ( item->x >= 0 ) && ( 255 >= item->y ) && ( item->x >= 0 ))
	{
		tempx = item->x;
		tempy = item->y;
	}
	else
	{
		tempx = DISABLED_SPRITE;
		tempy = DISABLED_SPRITE;
	}
	
	//quite a hackjob, NOT_ZOMBIE basically means shotgun shot.
	//if kill < 0 then the zombie is rising from his grave 
	if(item->kill != NOT_ZOMBIE && item->kill >= 0)
	{
		//show lower body aswell
		sprites[item->sprite.nrsprite+1].y = tempy + TILE_HEIGHT;
		sprites[item->sprite.nrsprite+1].x = tempx;
	}
	
	sprites[item->sprite.nrsprite].x = tempx;
	sprites[item->sprite.nrsprite].y = tempy;
	
}
