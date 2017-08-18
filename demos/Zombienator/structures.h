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

#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "uzebox.h"

#define MAP_MEGA_WIDTH  16
#define MAP_MEGA_HEIGHT 32
#define MAX_POWERUPS 10
extern const char tiles[];
#define MAINCHAR_SIZE 44
extern const char graveyard_sprites[];
extern const char zombienator_title[];
extern const char intro_map[];
extern const char title_map[];
extern const char fonts[];
extern const char tombstone[];
extern const char bush[];
extern const char middletower[];
extern const char loweruptower[];
extern const char churchmiddle[];
extern const char churchlower[];
extern const struct PatchStruct patches[];


// MACROS. YEAH!
#define END(var) (var&0x3)
#define X(var) ((var>>2)&0x7)
#define Y(var) ((var>>5)&0x7)
#define TYPE(var) ((var>>8)&0xF)
#define FLAGS(var) (var>>12)

//Should be const in some way, variables for screen pos and scrolling.
//instead of containing both x and y values
//I use two instances of this struct for x and y.
typedef struct CoordStruct
{
	char scrolled; //indicates whether megatile-scrolling has occured
	unsigned char s; //screen-scrolling position relative to the vram position
	unsigned char p; //vram position in the megatile map
	unsigned char SCREENTILE; //number of tiles onscreen
	unsigned char TILE; //size of the tile in pixels
	unsigned char MAP; //map size in megatiles
	unsigned char MEGATILE; //megatile size in pixels
	unsigned char MEGA; //megatile size in tiles
} CoordStruct;

//Animation variables, everything animated uses this
typedef struct AnimStruct
{
	unsigned char body;
	unsigned char legs;
	unsigned char speed;
	unsigned char nrsprite; //sprite position in sprite array
	unsigned char nranim; //sprite position in sprite table
	unsigned char animdelay; //to slow down the animation abit
	char animcount; //to keep looping animations in check.
} AnimStruct;

//Zombies and shots, basically anything that's not the main char.
typedef struct ItemStruct
{
	unsigned char dir; //for shots, not zombies
	char kill; //For zombies, not shots
	int x; //Position
	int y;
	int sx; //Scrolling position - basically the total scrolling offset
	int sy;
	AnimStruct sprite;
} ItemStruct;

typedef struct GunStruct
{
	unsigned char icon;
	unsigned char ammo;
	unsigned char delay;
} GunStruct;

typedef struct PlayerStruct
{
	char life;
	unsigned char x;
	unsigned char y;
	unsigned char flags;
	unsigned char gunnr;
	unsigned char flagcount;
	unsigned char hurtcount;
	unsigned char gundelay;
	unsigned char bouncecount; 
	unsigned char conskills; //consecutive kills, for scoring. 
	unsigned int killz; //score
	unsigned int joypad; //input ftw!
	AnimStruct sprite;
	ItemStruct shots[3];
	GunStruct guns[2];
} PlayerStruct;

typedef struct PowerStruct
{
	unsigned char megax;
	unsigned char megay;
	unsigned char x;
	unsigned char y;
	unsigned char type;
	unsigned char tile;
	unsigned char size;
} PowerStruct;

//Contains variables related to the mainchar
typedef struct MainStruct
{
	CoordStruct x;
	CoordStruct y;
	unsigned char players;
} MainStruct;

//General game variables
typedef struct GameStruct
{
	unsigned char flags; //instead of lots of booleans
	unsigned char flagcount; //debounce delay for pausing and toggling the menu
	unsigned char menupos; //cursor position in the menu
	unsigned int zombies; //total number of zombies, dead or undead
	unsigned int frames; //for seeding random number generator + skipping frames with modulus
	PowerStruct powerups[MAX_POWERUPS];
} GameStruct;

//Map items, tombstones, cathedrals, pathways, etc.
typedef struct MapStruct
{
	unsigned end : 2;
	unsigned x : 3;
	unsigned y : 3;
	unsigned type :  4;
	unsigned flags : 4;
} MapStruct;

struct SpriteStruct sprites[32]; // create the sprites
extern const MapStruct* const graveyard_map[MAP_MEGA_WIDTH][MAP_MEGA_HEIGHT];
extern const MapStruct graveyard_array[];

#endif
