/*
 *  Uzebox(tm) kernel GUI functions
 *  Copyright (C) 2008-2009 Alec Bourque
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
 * Uzebox is a reserved trade mark
*/

#ifndef __GUI_H_
#define __GUI_H_

	#include "defines.h"

	//buttons lib
	typedef struct
	{
		unsigned char id;
		unsigned char x;
		unsigned char y;
		unsigned char width;
		unsigned char height;
		const char *normalMapPtr;
		const char *pushedMapPtr;
		unsigned char state;
		bool clicked;
	} Button;


	typedef void (*ButtonHandler)(unsigned char buttonIndex,unsigned char eventType);
	void registerButtonHandler(ButtonHandler fptr, Button *buttons);
	void createButton(Button *button, unsigned char x,unsigned char y,const char *normalMapPtr,const char *pushedMapPtr);
	void processButtons();
	void createAreaButton(Button *button, unsigned char x,unsigned char y,unsigned char width,unsigned char height);
	void unregisterButtonHandler();


#endif
