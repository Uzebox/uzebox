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

const MapStruct graveyard_array[] PROGMEM =
{
	{3,0,0,0,0},				//0 - blank
	{0,1,1,1,0}, {3,5,1,1,0},		//1 - two centered tombstones
	{0,0,0,1,0}, {3,4,0,1,0},		//3 - two tombstones to the upper left
	{3,3,1,1,0}, 				//5 - a single centered tombstone
	{0,0,1,1,0}, {0,3,1,2,12}, {3,6,1,1,0},	//6 - two tombstones with a path going up/down in between
	{3,3,1,2,15},		  		//9 - an all-way path crossing in the middle
	{3,3,1,2,12},				//10 - a path going up/down in the middle
	{3,3,1,2,3},				//11 - a path going left/right in the middle
	{3,3,1,2,9},				//12 - a left/down crossing
	{3,3,1,2,5},				//13 - a left/up crossing
	{3,3,1,2,6},				//14 - a right/up crossing
	{3,3,1,2,10},				//15 - a right/down crossing
	{3,3,1,2,14},				//16 - a no-left crossing
	{3,3,1,2,13},				//17 - a no-right crossing
	{3,3,1,2,11},				//18 - a no-up crossing
	{3,3,1,2,7},				//19 - a no-down crossing
	{3,2,0,3,0},				//20 - a bush in the middle
	{3,1,0,4,0},				//21 - upper church
	{3,1,0,5,0},				//22 - middle church
	{3,1,0,6,0},				//23 - lower church
};

const MapStruct* graveyard_map[MAP_MEGA_WIDTH][MAP_MEGA_HEIGHT] PROGMEM =
{{&graveyard_array[1],&graveyard_array[1],&graveyard_array[1],&graveyard_array[20],&graveyard_array[11],&graveyard_array[5],&graveyard_array[0],&graveyard_array[0],&graveyard_array[5],&graveyard_array[11],&graveyard_array[0],&graveyard_array[1],&graveyard_array[0],&graveyard_array[1],&graveyard_array[0],&graveyard_array[1],&graveyard_array[11],&graveyard_array[0],&graveyard_array[1],&graveyard_array[0],&graveyard_array[1],&graveyard_array[12],&graveyard_array[14],&graveyard_array[1],&graveyard_array[5],&graveyard_array[11],&graveyard_array[20],&graveyard_array[5],&graveyard_array[1],&graveyard_array[5],&graveyard_array[12],&graveyard_array[14]}, 
 {&graveyard_array[6],&graveyard_array[14],&graveyard_array[1],&graveyard_array[15],&graveyard_array[9],&graveyard_array[6],&graveyard_array[6],&graveyard_array[6],&graveyard_array[14],&graveyard_array[11],&graveyard_array[3],&graveyard_array[20],&graveyard_array[1],&graveyard_array[0],&graveyard_array[5],&graveyard_array[20],&graveyard_array[18],&graveyard_array[6],&graveyard_array[6],&graveyard_array[14],&graveyard_array[20],&graveyard_array[1],&graveyard_array[11],&graveyard_array[5],&graveyard_array[3],&graveyard_array[11],&graveyard_array[5],&graveyard_array[3],&graveyard_array[5],&graveyard_array[3],&graveyard_array[15],&graveyard_array[13]}, 
 {&graveyard_array[5],&graveyard_array[11],&graveyard_array[15],&graveyard_array[13],&graveyard_array[11],&graveyard_array[20],&graveyard_array[0],&graveyard_array[20],&graveyard_array[11],&graveyard_array[11],&graveyard_array[0],&graveyard_array[3],&graveyard_array[15],&graveyard_array[6],&graveyard_array[6],&graveyard_array[6],&graveyard_array[13],&graveyard_array[1],&graveyard_array[0],&graveyard_array[11],&graveyard_array[5],&graveyard_array[0],&graveyard_array[12],&graveyard_array[10],&graveyard_array[6],&graveyard_array[9],&graveyard_array[6],&graveyard_array[6],&graveyard_array[16],&graveyard_array[6],&graveyard_array[13],&graveyard_array[0]}, 
 {&graveyard_array[1],&graveyard_array[18],&graveyard_array[13],&graveyard_array[1],&graveyard_array[12],&graveyard_array[6],&graveyard_array[14],&graveyard_array[3],&graveyard_array[11],&graveyard_array[12],&graveyard_array[6],&graveyard_array[6],&graveyard_array[9],&graveyard_array[10],&graveyard_array[10],&graveyard_array[10],&graveyard_array[10],&graveyard_array[10],&graveyard_array[10],&graveyard_array[9],&graveyard_array[10],&graveyard_array[6],&graveyard_array[6],&graveyard_array[14],&graveyard_array[0],&graveyard_array[11],&graveyard_array[0],&graveyard_array[20],&graveyard_array[12],&graveyard_array[14],&graveyard_array[0],&graveyard_array[1]}, 
 {&graveyard_array[5],&graveyard_array[11],&graveyard_array[1],&graveyard_array[21],&graveyard_array[22],&graveyard_array[23],&graveyard_array[11],&graveyard_array[3],&graveyard_array[11],&graveyard_array[20],&graveyard_array[3],&graveyard_array[0],&graveyard_array[11],&graveyard_array[3],&graveyard_array[3],&graveyard_array[3],&graveyard_array[3],&graveyard_array[3],&graveyard_array[3],&graveyard_array[11],&graveyard_array[1],&graveyard_array[0],&graveyard_array[20],&graveyard_array[11],&graveyard_array[5],&graveyard_array[11],&graveyard_array[1],&graveyard_array[1],&graveyard_array[5],&graveyard_array[11],&graveyard_array[1],&graveyard_array[0]}, 
 {&graveyard_array[10],&graveyard_array[9],&graveyard_array[6],&graveyard_array[14],&graveyard_array[15],&graveyard_array[6],&graveyard_array[19],&graveyard_array[3],&graveyard_array[1],&graveyard_array[0],&graveyard_array[1],&graveyard_array[20],&graveyard_array[11],&graveyard_array[20],&graveyard_array[5],&graveyard_array[5],&graveyard_array[5],&graveyard_array[5],&graveyard_array[20],&graveyard_array[11],&graveyard_array[1],&graveyard_array[0],&graveyard_array[20],&graveyard_array[18],&graveyard_array[6],&graveyard_array[19],&graveyard_array[1],&graveyard_array[1],&graveyard_array[20],&graveyard_array[11],&graveyard_array[0],&graveyard_array[1]}, 
 {&graveyard_array[5],&graveyard_array[11],&graveyard_array[3],&graveyard_array[12],&graveyard_array[19],&graveyard_array[20],&graveyard_array[11],&graveyard_array[5],&graveyard_array[3],&graveyard_array[1],&graveyard_array[15],&graveyard_array[6],&graveyard_array[19],&graveyard_array[0],&graveyard_array[1],&graveyard_array[1],&graveyard_array[1],&graveyard_array[15],&graveyard_array[10],&graveyard_array[19],&graveyard_array[15],&graveyard_array[6],&graveyard_array[6],&graveyard_array[13],&graveyard_array[3],&graveyard_array[11],&graveyard_array[5],&graveyard_array[5],&graveyard_array[3],&graveyard_array[11],&graveyard_array[3],&graveyard_array[0]}, 
 {&graveyard_array[1],&graveyard_array[18],&graveyard_array[6],&graveyard_array[6],&graveyard_array[9],&graveyard_array[6],&graveyard_array[17],&graveyard_array[14],&graveyard_array[3],&graveyard_array[15],&graveyard_array[13],&graveyard_array[0],&graveyard_array[11],&graveyard_array[3],&graveyard_array[21],&graveyard_array[22],&graveyard_array[23],&graveyard_array[11],&graveyard_array[20],&graveyard_array[11],&graveyard_array[11],&graveyard_array[5],&graveyard_array[5],&graveyard_array[0],&graveyard_array[1],&graveyard_array[18],&graveyard_array[10],&graveyard_array[10],&graveyard_array[16],&graveyard_array[17],&graveyard_array[10],&graveyard_array[6]}, 
 {&graveyard_array[5],&graveyard_array[11],&graveyard_array[20],&graveyard_array[1],&graveyard_array[11],&graveyard_array[3],&graveyard_array[5],&graveyard_array[11],&graveyard_array[0],&graveyard_array[11],&graveyard_array[1],&graveyard_array[20],&graveyard_array[11],&graveyard_array[20],&graveyard_array[15],&graveyard_array[6],&graveyard_array[6],&graveyard_array[13],&graveyard_array[1],&graveyard_array[11],&graveyard_array[11],&graveyard_array[5],&graveyard_array[5],&graveyard_array[0],&graveyard_array[1],&graveyard_array[11],&graveyard_array[1],&graveyard_array[1],&graveyard_array[11],&graveyard_array[5],&graveyard_array[0],&graveyard_array[5]}, 
 {&graveyard_array[6],&graveyard_array[19],&graveyard_array[1],&graveyard_array[1],&graveyard_array[11],&graveyard_array[5],&graveyard_array[20],&graveyard_array[11],&graveyard_array[3],&graveyard_array[11],&graveyard_array[0],&graveyard_array[3],&graveyard_array[18],&graveyard_array[6],&graveyard_array[19],&graveyard_array[20],&graveyard_array[5],&graveyard_array[3],&graveyard_array[5],&graveyard_array[11],&graveyard_array[12],&graveyard_array[6],&graveyard_array[6],&graveyard_array[14],&graveyard_array[3],&graveyard_array[11],&graveyard_array[1],&graveyard_array[1],&graveyard_array[11],&graveyard_array[0],&graveyard_array[1],&graveyard_array[0]}, 
 {&graveyard_array[5],&graveyard_array[11],&graveyard_array[5],&graveyard_array[15],&graveyard_array[13],&graveyard_array[3],&graveyard_array[5],&graveyard_array[11],&graveyard_array[3],&graveyard_array[11],&graveyard_array[0],&graveyard_array[3],&graveyard_array[11],&graveyard_array[3],&graveyard_array[11],&graveyard_array[1],&graveyard_array[1],&graveyard_array[0],&graveyard_array[5],&graveyard_array[11],&graveyard_array[1],&graveyard_array[0],&graveyard_array[20],&graveyard_array[11],&graveyard_array[3],&graveyard_array[18],&graveyard_array[10],&graveyard_array[10],&graveyard_array[17],&graveyard_array[14],&graveyard_array[5],&graveyard_array[1]}, 
 {&graveyard_array[1],&graveyard_array[11],&graveyard_array[20],&graveyard_array[11],&graveyard_array[1],&graveyard_array[0],&graveyard_array[3],&graveyard_array[11],&graveyard_array[3],&graveyard_array[18],&graveyard_array[6],&graveyard_array[6],&graveyard_array[19],&graveyard_array[3],&graveyard_array[11],&graveyard_array[3],&graveyard_array[3],&graveyard_array[3],&graveyard_array[3],&graveyard_array[11],&graveyard_array[1],&graveyard_array[0],&graveyard_array[20],&graveyard_array[11],&graveyard_array[5],&graveyard_array[11],&graveyard_array[21],&graveyard_array[22],&graveyard_array[23],&graveyard_array[11],&graveyard_array[5],&graveyard_array[0]}, 
 {&graveyard_array[20],&graveyard_array[18],&graveyard_array[6],&graveyard_array[19],&graveyard_array[5],&graveyard_array[0],&graveyard_array[0],&graveyard_array[11],&graveyard_array[5],&graveyard_array[11],&graveyard_array[0],&graveyard_array[5],&graveyard_array[18],&graveyard_array[10],&graveyard_array[17],&graveyard_array[10],&graveyard_array[6],&graveyard_array[16],&graveyard_array[6],&graveyard_array[17],&graveyard_array[16],&graveyard_array[6],&graveyard_array[6],&graveyard_array[13],&graveyard_array[0],&graveyard_array[11],&graveyard_array[20],&graveyard_array[20],&graveyard_array[20],&graveyard_array[11],&graveyard_array[1],&graveyard_array[1]}, 
 {&graveyard_array[3],&graveyard_array[11],&graveyard_array[1],&graveyard_array[11],&graveyard_array[3],&graveyard_array[0],&graveyard_array[1],&graveyard_array[18],&graveyard_array[6],&graveyard_array[17],&graveyard_array[6],&graveyard_array[6],&graveyard_array[19],&graveyard_array[5],&graveyard_array[20],&graveyard_array[1],&graveyard_array[0],&graveyard_array[11],&graveyard_array[20],&graveyard_array[3],&graveyard_array[18],&graveyard_array[10],&graveyard_array[10],&graveyard_array[14],&graveyard_array[3],&graveyard_array[18],&graveyard_array[6],&graveyard_array[6],&graveyard_array[6],&graveyard_array[19],&graveyard_array[0],&graveyard_array[5]}, 
 {&graveyard_array[10],&graveyard_array[13],&graveyard_array[0],&graveyard_array[18],&graveyard_array[6],&graveyard_array[16],&graveyard_array[6],&graveyard_array[19],&graveyard_array[0],&graveyard_array[1],&graveyard_array[3],&graveyard_array[0],&graveyard_array[11],&graveyard_array[3],&graveyard_array[1],&graveyard_array[3],&graveyard_array[1],&graveyard_array[18],&graveyard_array[6],&graveyard_array[6],&graveyard_array[13],&graveyard_array[1],&graveyard_array[1],&graveyard_array[12],&graveyard_array[16],&graveyard_array[13],&graveyard_array[5],&graveyard_array[1],&graveyard_array[5],&graveyard_array[11],&graveyard_array[5],&graveyard_array[1]}, 
 {&graveyard_array[1],&graveyard_array[5],&graveyard_array[3],&graveyard_array[11],&graveyard_array[1],&graveyard_array[11],&graveyard_array[20],&graveyard_array[18],&graveyard_array[6],&graveyard_array[6],&graveyard_array[6],&graveyard_array[6],&graveyard_array[19],&graveyard_array[5],&graveyard_array[0],&graveyard_array[1],&graveyard_array[20],&graveyard_array[11],&graveyard_array[0],&graveyard_array[3],&graveyard_array[0],&graveyard_array[5],&graveyard_array[0],&graveyard_array[20],&graveyard_array[11],&graveyard_array[20],&graveyard_array[1],&graveyard_array[5],&graveyard_array[1],&graveyard_array[11],&graveyard_array[1],&graveyard_array[0]}}; 
 
const char tombstone[] PROGMEM = 
{2,2,
	4,5,
	7,6,
};

const char bush[] PROGMEM = 
{3,3,
	15,16,17,
	18,19,20,
	21,22,23,
};

const char middletower[] PROGMEM = 
{2,3,
	24,25,
	26,27,
	28,29,
};

const char loweruptower[] PROGMEM =
{6,1,
	32,33,30,31,34,35,
}; 

const char churchmiddle[] PROGMEM =
{6,4,
	36,37,38,39,40,41,
	42,43,44,45,46,47,
	48,49,50,50,51,52,
	53,54,50,50,55,56
	
};

const char churchlower[] PROGMEM =
{6,3,
	48,50,57,58,50,52,
	53,50,59,60,50,56,
	48,50,61,62,50,52,
};

 
const char title_map[] PROGMEM =
{21,4,
 8, 7, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 5, 6,13,15,17,19,18,22,15,26,13,15,17,15,14,15,25,13,15,13,10, 
 3, 4,12,12,12,20,12,12,12,23,22,11,12,12,13,24,12,12,12,12, 0, 
 2, 1, 9,11,16, 0,16,21,11,16, 9,10,16,16, 9,11,16, 9,11,16, 0
};