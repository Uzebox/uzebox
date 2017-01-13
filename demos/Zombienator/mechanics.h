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

#ifndef MECHANICS_H_
#define MECHANICS_H_

#include "Zombienator.h"


//Horrible attempt att minimizing the code, unreadable.
//Handles vertical and horizontal scrolling
//Oh, and also moving the main char.
bool scrollchar(bool direction, CoordStruct* mstruct, unsigned char* c);

//Moves the main char without touching the scrolling.
void movechar(bool direction, unsigned char* c, CoordStruct* mstruct);

//Adjusts coordinates after scrolling has occured
void postoscroll(ItemStruct* item, MainStruct* main);

//Collision detection against the background image.
//Reads the vram to get the tombstone coordinates.
unsigned char mapcollide(MainStruct* mains, int x, int y);

//For finding viable zombie spawnpoints.
int findgrave(MainStruct *mains);

void powerupper(MainStruct* mains, GameStruct* game, PlayerStruct players[]);
#endif
