/*
 *  Uzebox Kernel - Mode 5
 *  Copyright (C) 2011  Alec Bourque
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
	#include "intro.h"
	
	#if INTRO_LOGO !=0
		#include "videoMode5/uzeboxlogo_6x8.pic.inc"
		#include "videoMode5/uzeboxlogo_6x8.map.inc"
	#endif


	//Callback invoked by UzeboxCore.Initialize()
	void DisplayLogo(){
	
		#if INTRO_LOGO !=0
			#define LOGO_X_POS 18
			
			InitMusicPlayer(logoInitPatches);
			SetTileTable(uzeboxlogo);
					
			//draw logo
			ClearVram();
			WaitVsync(15);		


			#if INTRO_LOGO == 1 
				TriggerFx(0,0xff,true);
			#endif

			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo);
			WaitVsync(6);
			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo2);
			WaitVsync(4);
			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo);

			#if INTRO_LOGO == 2
				SetMasterVolume(0xc0);
				TriggerNote(3,0,16,0xff);
			#endif 
		
			WaitVsync(64);
			ClearVram();
			WaitVsync(20);
		#endif	
	}


	//Callback invoked by UzeboxCore.Initialize()
	void InitializeVideoMode(){
	
	}

	//Callback invoked during hsync
	void VideoModeVsync(){		
		//ProcessFading();
	}
