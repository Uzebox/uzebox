/*
 *  UzeboxPC main file.
 *  Copyright (C) 2020 Uze
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
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "uzenet.h"
#include "telnet.h"
#include "terminal.h"
#include "computer.h"
#include "uzenet_setup.h"

#define TERMINAL &TERMINAL_STREAM

/**
 * Add vsync callback handlers for the different modules.
 */
volatile u16 vcount=0; //to remove
void vsyncCallback(){
	vcount++;

	wifi_VsyncCallback();		//used to handle timeouts
	telnet_VsyncCallback();		//used to advance the progress bar
	terminal_VsyncCallback();	//used to poll the keyboard
}

/**
 * Main entry point of the application.
 * After basic system setup, it enters the 8080 emulator.
 */
int main(){
	SetUserPreVsyncCallback(&vsyncCallback);
	terminal_Init();
	terminal_Clear();
	terminal_SetCursorVisible(true);
	stdout = &TERMINAL_STREAM;  //bind the terminal receiver to stdout

	//Uncomment this block to build a standalone uzenet_setup rom
	//uzenet_Setup();
	//while(1);

	wifi_Echo(false);
	computer_Boot();

	printf_P(PSTR("\r\nExited CPU emulation.\r\n"));
	while(1);
}
