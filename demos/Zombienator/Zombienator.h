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

#ifndef _ZOMBIENATOR_H_
#define _ZOMBIENATOR_H_

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "uzebox.h"

// Constants
#define SCROLL_MEMORY_SIZE 32
#define OVERLAYROW 0

#define SCREEN_WIDTH 132
#define SCREEN_HEIGHT 208

#define MEGATILE_WIDTH 8
#define MEGATILE_HEIGHT 4

#define MAX_ZOMBIES 10
#define MAX_POWERUPS 10
#define DISABLED_SPRITE 166
#define GUN_SPRITE 64

#define ZOMBIEFIRE 26
#define NOT_ZOMBIE 5

#define FLAMETHROWER_DELAY 4
#define HURT_DELAY 66
#define DEBOUNCE_DELAY 15
#define SCORE_DELAY 75


// Flags
#define GAMEPAUSED 1
#define PLAYERHURT 2
#define GAMEMULTILO 4
#define GAMEMULTIHO 8
#define PLAYERBEHIND 16

// Scrolling flags for multiplayer
#define LEFTBORDER 1
#define RIGHTBORDER 2
#define TOPBORDER 4
#define BOTTOMBORDER 8

#include "structures.h"
#include "videoengine.h"
#include "mechanics.h"
#include "necromancy.h"
#include "guns.h"

#endif
