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

#ifndef GUNS_H_
#define GUNS_H_

#include "Zombienator.h"

void changegun(PlayerStruct* player);

//For shooting at zombies
void shoot(PlayerStruct* player);
void shoot_shotgun(PlayerStruct* player);
void shoot_flamethrower(PlayerStruct* player);

//Keeps the shots going until they hit something
void handleshot(MainStruct* mains, GameStruct* game, PlayerStruct* player, ItemStruct zombies[MAX_ZOMBIES]);
void handleshot_shotgun(MainStruct* mains, GameStruct* game, PlayerStruct* player, ItemStruct zombies[MAX_ZOMBIES]);
void handleshot_flamethrower(MainStruct* mains, GameStruct* game, PlayerStruct* player, ItemStruct zombies[MAX_ZOMBIES]);

#endif
