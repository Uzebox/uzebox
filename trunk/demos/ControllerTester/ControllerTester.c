/*
 *  Controller Tester
 *  Copyright (C) 2010  Flávio Zavan
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

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

#include "data/cursor.inc"
#include "data/tiles.inc"

#define GAMEPAD 1
#define MOUSE 2
#define NOTHING 0

unsigned char cType[2];

int main(){
	SetTileTable(cTesterTiles);
	SetSpritesTileTable(cursor);
	ClearVram();

	EnableSnesMouse(0,map_cursor);

	//Print the basic stuff on the screen
	DrawMap2(5,26,map_copyright);
	DrawMap2(0,13,map_divider);
	//Detect controller types
	cType[0] = 255; 
	cType[1] = 255; 
	//The controller vars
	int btnHeld[2] = {0,0};
	int btnPressed[2] = {0,0};
	int btnReleased[2] = {0,0};
	int btnPrev[2] = {0,0};
	//Main loop
	while(1){
		//20fps
		WaitVsync(1);
		//Update the controller vars
		btnHeld[0] = ReadJoypad(0);
		btnHeld[1] = ReadJoypad(1);
		btnPressed[0] = btnHeld[0] & (btnHeld[0] ^ btnPrev[0]);
		btnPressed[1] = btnHeld[1] & (btnHeld[1] ^ btnPrev[1]);
		btnReleased[0] = btnPrev[0] & (btnHeld[0] ^ btnPrev[0]);
		btnReleased[1] = btnPrev[1] & (btnHeld[1] ^ btnPrev[1]);
		//Check if controllers changed
		if((DetectControllers() & 3) != cType[0]){
			cType[0] = DetectControllers() & 3;
			Fill(0,0,30,13,0);
			if(cType[0] == GAMEPAD){
				DrawMap2(9,2,map_gamepad);
			}
			else if(cType[0] == MOUSE){
				DrawMap2(12,2,map_mouse);
				DrawMap2(1,4,map_warning);
			}
			else if(cType[0] == NOTHING){
				DrawMap2(9,6,map_nocontroller);
			}
		}
		if((DetectControllers() & 12) / 4 != cType[1]){
			cType[1] = (DetectControllers() & 12) / 4;
			Fill(0,14,30,12,0);
			if(cType[1] == GAMEPAD){
				DrawMap2(9,15,map_gamepad);
				SetSpriteVisibility(false);
			}
			else if(cType[1] == MOUSE){
				DrawMap2(12,15,map_mouse);
				SetSpriteVisibility(true);
				EnableSnesMouse(0,map_cursor);
			}
			else if(cType[1] == NOTHING){
				DrawMap2(9,19,map_nocontroller);
				SetSpriteVisibility(false);
			}
		}
		//Update the buttons
		for(unsigned int i = 0; i < 2; i++){
			if(cType[i] == MOUSE){
				//Check pressed buttons
				if(btnPressed[i] & BTN_MOUSE_LEFT){
					DrawMap2(14,4 + 13*i,map_pmouseb);
				}
				if(btnPressed[i] & BTN_MOUSE_RIGHT){
					DrawMap2(16,4 + 13*i,map_pmouseb);
				}
				//Check released
				if(btnReleased[i] & BTN_MOUSE_LEFT){
					DrawMap2(14,4 + 13*i,map_mouseb);
				}
				if(btnReleased[i] & BTN_MOUSE_RIGHT){
					DrawMap2(16,4 + 13*i,map_mouseb);
				}
			}
			else if(cType[i] == GAMEPAD){
				//Check pressed
				if(btnPressed[i] & BTN_UP){
					DrawMap2(11,6 + 13*i,map_pdpad);
				}
				if(btnPressed[i] & BTN_DOWN){
					DrawMap2(11,8 + 13*i,map_pdpad);
				}
				if(btnPressed[i] & BTN_LEFT){
					DrawMap2(10,7 + 13*i,map_pdpad);
				}
				if(btnPressed[i] & BTN_RIGHT){
					DrawMap2(12,7 + 13*i,map_pdpad);
				}
				if(btnPressed[i] & BTN_START){
					DrawMap2(16,6 + 13*i,map_pstart);
				}
				if(btnPressed[i] & BTN_SELECT){
					DrawMap2(14,6 + 13*i,map_pstart);
				}
				if(btnPressed[i] & BTN_A){
					DrawMap2(20,7 + 13*i,map_pbutton);
				}
				if(btnPressed[i] & BTN_B){
					DrawMap2(19,8 + 13*i,map_pbutton);
				}
				if(btnPressed[i] & BTN_X){
					DrawMap2(19,6 + 13*i,map_pbutton);
				}
				if(btnPressed[i] & BTN_Y){
					DrawMap2(18,7 + 13*i,map_pbutton);
				}
				if(btnPressed[i] & BTN_SL){
					DrawMap2(10,4 + 13*i,map_pbumper);
				}
				if(btnPressed[i] & BTN_SR){
					DrawMap2(18,4 + 13*i,map_pbumper);
				}
				if(btnReleased[i] & BTN_UP){
					DrawMap2(11,6 + 13*i,map_dpad);
				}
				if(btnReleased[i] & BTN_DOWN){
					DrawMap2(11,8 + 13*i,map_dpad);
				}
				if(btnReleased[i] & BTN_LEFT){
					DrawMap2(10,7 + 13*i,map_dpad);
				}
				if(btnReleased[i] & BTN_RIGHT){
					DrawMap2(12,7 + 13*i,map_dpad);
				}
				if(btnReleased[i] & BTN_START){
					DrawMap2(16,6 + 13*i,map_start);
				}
				if(btnReleased[i] & BTN_SELECT){
					DrawMap2(14,6 + 13*i,map_start);
				}
				if(btnReleased[i] & BTN_A){
					DrawMap2(20,7 + 13*i,map_button);
				}
				if(btnReleased[i] & BTN_B){
					DrawMap2(19,8 + 13*i,map_button);
				}
				if(btnReleased[i] & BTN_X){
					DrawMap2(19,6 + 13*i,map_button);
				}
				if(btnReleased[i] & BTN_Y){
					DrawMap2(18,7 + 13*i,map_button);
				}
				if(btnReleased[i] & BTN_SL){
					DrawMap2(10,4 + 13*i,map_bumper);
				}
				if(btnReleased[i] & BTN_SR){
					DrawMap2(18,4 + 13*i,map_bumper);
				}
			}
		}
		btnPrev[0] = btnHeld[0];
		btnPrev[1] = btnHeld[1];
	}
} 
