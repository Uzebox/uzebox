/*
 *  Uzebox Kernel - Mode 80
 *  Copyright (C) 2019 Sandor Zsuga (Jubatian)
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
	#include "uzebox.h"
	#include "videoMode.def.h"

	/* Callback invoked by UzeboxCore.Initialize() */
	void InitializeVideoMode(){

		SetFontTilesIndex(FONT_TILE_INDEX);
		m80_bgclist = NULL; /* Simple start, all features disabled */
		m80_fgclist = NULL; /* User may use them at will or never care */
		m80_dlist   = NULL;
		m80_rompal  = NULL;
		m80_rampal  = NULL;

	}

	/* Callback invoked during vsync */
	void VideoModeVsync(){
	}

	/* Callback invoked by UzeboxCore.Initialize() */
	void DisplayLogo(){
	}
