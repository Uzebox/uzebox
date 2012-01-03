/*
 *  Uzebox(tm) Lode Runner
 *  Copyright (C) 2010  Alec Bourque
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
 *
 *  Uzebox is a reserved trade mark
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "loderunner.h"

extern Game game;
extern Player player[];

u16 Ai(u8 id);
u8 findPath(u8 id,u8 srcX, u8 srcY, u8 destX, u8 destY, s8 dir);

s16 findLadderDownOnPath(u8 x,u8 y, s8 dir);
s16 findLadderUpOnPath(u8 x,u8 y, s8 dir);

s16 findPathTo(u8 id,u8 srcX, u8 srcY, u8 destX, s8 dir);
s16 findCliffOnPath(u8 x,u8 y, s8 dir);

#define AI_NO_PATH				0
#define AI_ACTION_MOVE			1
#define AI_ACTION_FALL			2
#define AI_ACTION_CLIMB_UP		3
#define AI_ACTION_CLIMB_DOWN	4


u16 Ai(u8 id){


	u8 srcX=(player[id].x>>8);
	u8 srcY=(player[id].y>>8);
	u8 destX=(player[SPR_INDEX_PLAYER].x>>8);
	u8 destY=(player[SPR_INDEX_PLAYER].y>>8);
	u8 action=0;

	if(player[id].stuckDelay>0){
		player[id].stuckDelay--;
		return 0;
	}

	if(player[id].action==ACTION_CLIMB){
		//if we were already climbing continue to destination
		//unless the player is at same level
		if(srcY != destY){
			if(player[id].lastAiAction==AI_ACTION_CLIMB_UP){
				return BTN_UP;
			}else if(player[id].lastAiAction==AI_ACTION_CLIMB_DOWN){
				return BTN_DOWN;
			}
		}else{
			player[id].aiTarget=0;
		}	
	}

	if(player[id].lastAiAction==AI_ACTION_MOVE && player[id].aiTarget!=0){

		if((srcX>=(player[id].aiTarget&0xfff8) && player[id].dir==1) || (srcX<=(player[id].aiTarget&0xfff8) && player[id].dir==-1)){
			player[id].aiTarget=0;
		}else{
			action=player[id].lastAiAction;
		}
	}

	if(player[id].aiTarget==0){

		if(srcX<destX){
			player[id].dir=1;
		}else{
			player[id].dir=-1;
		}

		action=findPath(id,srcX,srcY,destX,destY,player[id].dir);


		if(action==AI_NO_PATH){
			//no path in this direction, try the other way

			action=findPath(id,srcX,srcY,destX,destY,-player[id].dir);
			if(action==AI_NO_PATH){
				//no way to get out, this guy is stuck
				//retry in some random # of frames
				//to avoid eating all the cpu trying to find an exit path
				if(!game.demoMode)
					player[id].stuckDelay=(rand()%100)+5;

			}else{
				player[id].dir=-player[id].dir;
			}
		}

		player[id].lastAiAction=action;
	}

	//reverse direction if we hit a wall
	//TODO

	if(action==AI_ACTION_MOVE){
		if(player[id].dir==1){
			return BTN_RIGHT;
		}else{
			return BTN_LEFT;
		}
	}else if(action==AI_ACTION_CLIMB_UP){
		return BTN_UP;

	}else if(action==AI_ACTION_CLIMB_DOWN || action==AI_ACTION_FALL){
		return BTN_DOWN;
	}else{
		return 0;
	}



	return 0;
}



u8 findPath(u8 id,u8 srcX, u8 srcY, u8 destX, u8 destY, s8 dir){
	s16 loc=0;
	player[id].aiTarget=0;

	//check if target is at same altitude and right ahead 
	if(srcY==destY){
		if(findPathTo(id,srcX,srcY,destX,dir)!=-1) return AI_ACTION_MOVE;

		//try to go up
		loc=findLadderUpOnPath(srcX,srcY,dir);

		if(loc!=-1){


			if(loc!=srcX){
				//move toward ladder
				player[id].aiTarget=loc;
				return AI_ACTION_MOVE;
			}else{
				//we are right at ladder base, climb up
				return AI_ACTION_CLIMB_UP;
			}
		}else{
			//try to go down with ladder
			loc=findLadderDownOnPath(srcX,srcY,dir);
			if(loc!=-1){
				if(loc!=srcX){
					//move toward toward ladder (walk or rope)
					player[id].aiTarget=loc;
					return AI_ACTION_MOVE;
				}else{
					//climb ladder
					return AI_ACTION_CLIMB_DOWN;
				}
			}else{
				//try to reach by falling of cliff
				loc=findCliffOnPath(srcX,srcY,dir);
				if(loc!=-1){
					//move toward cliff
					player[id].aiTarget=loc;
					return AI_ACTION_MOVE;
				}else{
					return AI_NO_PATH;
				}
			}
		}

	}

	//try to go up over obstacle
	if(srcY>destY){

		loc=findLadderUpOnPath(srcX,srcY,dir);

		if(loc!=-1){
			if(loc!=srcX){
				//move toward toward ladder (walk or rope)
				player[id].aiTarget=loc;
				return AI_ACTION_MOVE;
			}else{
				//climb ladder
				return AI_ACTION_CLIMB_UP;
			}

		}else{
			//no path to reach higher
			return AI_NO_PATH;
		}
		
	}
	
	//try to go down
	if(srcY<destY){

		//if on rope, check if we can reach the player by falling
		if(player[id].action==ACTION_CLING){
			u8 y=destY,c;		

			while(y<FIELD_HEIGHT*TILE_HEIGHT){
				if(IsTileSolid( GetTileAtFeet(srcX,y+TILE_HEIGHT),id)){
					if(y==destY){
						c=findPathTo(id,srcX,y,destX,player[id].dir);
						if(c!=-1){
							if((srcX>>3)==(destX>>3)){// && !IsTileSolid(GetTileAt(srcX,srcY+1))){
								player[id].aiTarget=0;
								return AI_ACTION_FALL;
							}else{
								return AI_ACTION_MOVE;
							}
						}else{
							c=findPathTo(id,srcX,y,destX,-player[id].dir);
							if(c!=-1){
								player[id].dir=-player[id].dir;
								player[id].aiTarget=0;
								return AI_ACTION_FALL;
							}
						}
						break;
					}else{
						break; //nope
					}
				}
				y+=TILE_HEIGHT;
			}

			//let's continue
			return AI_ACTION_MOVE;

		}else{

			if(player[id].lastAiAction!=AI_ACTION_CLIMB_UP){

				//check to go down a ladder
				loc=findLadderDownOnPath(srcX,srcY,dir);
				if(loc!=-1){

					if(loc!=srcX){
						//move toward toward ladder (walk or rope)
						player[id].aiTarget=loc;
						return AI_ACTION_MOVE;
					}else{
						//climb ladder
						return AI_ACTION_CLIMB_DOWN;
					}
				}

			}

			//try to go down by falling
			loc=findCliffOnPath(srcX,srcY,dir);
			if(loc!=-1){
				if(loc>=srcX && dir==1){
					//move toward toward cliff (walk or rope or fake brick)
					player[id].aiTarget=loc+5;
				}else if(loc<=srcX && dir==-1){
					player[id].aiTarget=loc-5;
				}
				return AI_ACTION_MOVE;
			}else{
				//no cliff in this direction
				return AI_NO_PATH;
			}


		}
	}
	

	return AI_NO_PATH; //no path in this direction
}


s16 findLadderUpOnPath(u8 x,u8 y, s8 dir){

	u8 tile,tileUnder;
	x&=0xf8;

	//try direct on X axis
	while(x>0 && x<VRAM_TILES_H*TILE_WIDTH){

		tile=GetTileAtFeet(x,y);
		tileUnder=GetTileAtFeet(x,y+TILE_HEIGHT);

		if(tile==TILE_LADDER){
			return x;
		}else if(tile==TILE_BREAKABLE || tile==TILE_UNBREAKABLE || (tileUnder==TILE_BG && tile!=TILE_ROPE) ){
			return -1; //dead end
		}

		x+=(dir*TILE_WIDTH);
	}

	return -1;
}

s16 findLadderDownOnPath(u8 x,u8 y, s8 dir){

	u8 tile,tileUnder;
	x&=0xf8;

	//try direct on X axis
	while(x>0 && x<VRAM_TILES_H*TILE_WIDTH){

		tile=GetTileAtFeet(x,y);
		tileUnder=GetTileAtFeet(x,y+TILE_HEIGHT);

		if(tileUnder==TILE_LADDER){
			return x;
		}else if(tileUnder==TILE_BG && tile!=TILE_ROPE){
			return -1; //dead end, blocked by a cliff
		}

		x+=(dir*TILE_WIDTH);
	}

	return -1;
}

//find a reachable cliff (or fake brick) on the path
s16 findCliffOnPath(u8 x,u8 y, s8 dir){

	u8 tile,tileUnder;
	x&=0xf8;

	//try direct on X axis
	while(x>0 && x<VRAM_TILES_H*TILE_WIDTH){
		tile=GetTileAtFeet(x,y);
		tileUnder=GetTileAtFeet(x,y+TILE_HEIGHT);

		if(IsTileBlocking(tile)) return -1; //dead end, blocked by a wall
		if(tileUnder==TILE_BG || tileUnder==TILE_BREAKABLE_FAKE)return x;
		x+=(dir*TILE_WIDTH);
	}

	return -1;
}


s16 findPathTo(u8 id,u8 srcX, u8 srcY, u8 destX, s8 dir){

	u8 tile,tileUnder;
	srcX&=0xf8;
	destX&=0xf8;

	//try direct on X axis
	while(srcX>0 && srcX<VRAM_TILES_H*TILE_WIDTH){
		tile=GetTileAtFeet(srcX,srcY);
		tileUnder=GetTileAtFeet(srcX,srcY+TILE_HEIGHT);

		if((srcX>>3)==(destX>>3)) return srcX; //found target
		if(IsTileBlocking(tile)) return -1; //dead end, blocked by a wall or cliff
		if(tileUnder==TILE_BG && tile!=TILE_ROPE) return -1;

		srcX+=(dir*TILE_WIDTH);

	}
	
	return -1;

}


