/*
 *  Uzebox Kernel - Mode 9
 *  Copyright (C) 2008  Alec Bourque
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
	#include <avr/interrupt.h>
	#include "uzebox.h"

	extern u8 foregroundColor;

	//Callback invoked by UzeboxCore.Initialize()
	void InitializeVideoMode(){
		foregroundColor=0xff; //for 80 cols mode
		MoveCursor(0,0);
		SetCursorVisible(false);
		SetCursorParams(0,30);
	}

	//Callback invoked during hsync
	void VideoModeVsync(){
	}

	//Callback invoked by UzeboxCore.Initialize()
	void DisplayLogo(){
	}
