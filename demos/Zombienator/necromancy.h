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

#ifndef NECROMANCY_H_
#define NECROMANCY_H_

#include "zombienator.h"



//Early attempt at making zombies explode into blood and gore and guts when shot at.
void gorify(ItemStruct* zombie);

//All mighty zombie-super-AI-function
//bit too much going on here.
unsigned char handlezombie(ItemStruct* zombie, PlayerStruct players[], MainStruct* main);

//ABRA-KADABRA AND THERE WERE ZOMBIES
void summon(ItemStruct* zombie, MainStruct* mains);

//CAN WE SUMMON UP SOME FORCES OF DARKNESS? MUAHAHAHAHHAHAHA
void ritual(ItemStruct zombie[MAX_ZOMBIES], GameStruct* game, MainStruct* mains);

//General zombie management.
void necromancer(ItemStruct zombie[MAX_ZOMBIES], MainStruct *mains, GameStruct* game, PlayerStruct players[]);

#endif
