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

#ifndef VIDEOENGINE_H_
#define VIDEOENGINE_H_

#include "Zombienator.h"


void fillmap(unsigned char x, unsigned char y, unsigned char width, unsigned char height);

void* progread(uint32_t* from, uint32_t* to, unsigned char amount);

void drawpath(unsigned char x, unsigned char y, uint16_t data);

void drawmegatile(unsigned char x, unsigned char y, uint16_t* tiledata);

void rendermap(MainStruct* mains);

//All animations use this one.
void animchar(AnimStruct* anim);

//Overlay, what can I say? Score, ammunition, health left, etc.
void drawoverlay(GameStruct* game, PlayerStruct* player);
void multidrawoverlay(GameStruct* game, PlayerStruct players[2]);

//Sprite drawing function, handles shots and zombies
void drawsprite(ItemStruct* item);

void drawpups(MainStruct* mains, GameStruct* game);
void erasepup(MainStruct* mains, PowerStruct* power);

#endif
