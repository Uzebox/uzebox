/*
 *  Uzebox Kernel
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
 *  Uzebox is a reserved trade mark
*/
/*
 This file contains various GUI related functions. It must be added explicitly to your Makefile.
*/


/* These are buttons function for use with the mouse */
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "gui.h"

#define DBL_CLICK_DELAY 15

extern unsigned char playDevice,playPort;
extern unsigned int actionButton;

ButtonHandler btnHandlerPtr;
Button *_buttons;
unsigned char btnCount=0;
bool buttonsHandlerActive=false;

void createButton(Button *button, unsigned char x,unsigned char y,const char *normalMapPtr,const char *pushedMapPtr){	

	button->x=x;
	button->y=y;
	button->normalMapPtr=normalMapPtr;
	button->pushedMapPtr=pushedMapPtr;
	button->state=0;
	button->width=pgm_read_byte(&(normalMapPtr[0]));
	button->height=pgm_read_byte(&(normalMapPtr[1]));
	button->clicked=false;
	DrawMap2(x,y,normalMapPtr);

	btnCount++;	
}

void createAreaButton(Button *button, unsigned char x,unsigned char y,unsigned char width,unsigned char height){	

	button->x=x;
	button->y=y;
	button->width=width;
	button->height=height;
	button->normalMapPtr=NULL;
	button->pushedMapPtr=NULL;
	button->state=0;
	button->clicked=false;

	btnCount++;	
}

void registerButtonHandler(ButtonHandler fptr, Button *btns){
	btnHandlerPtr=fptr;
	_buttons=btns;
	buttonsHandlerActive=true;
}

void unregisterButtonHandler(){
	btnHandlerPtr=NULL;
	_buttons=NULL;
	buttonsHandlerActive=false;
}

static unsigned char debugNo2=0;
void debug2(char no){

	PrintHexByte(2+(no*3),3,debugNo2);

}


//char debug=0;
void processButtons(){
	unsigned char i,tx,ty;
	unsigned int joy=ReadJoypad(playPort);
	static unsigned int lastButtons=0;
	static unsigned char dblClickDelay=0;

	tx=GetMouseX()>>3;
	ty=GetMouseY()>>3;

	for(i=0;i<btnCount;i++){

		if(tx>=_buttons[i].x && tx<(_buttons[i].x+_buttons[i].width) && ty>=_buttons[i].y && ty<(_buttons[i].y+_buttons[i].height)){		
		

			if(joy&actionButton && _buttons[i].state==BUTTON_UP){
				//button pushed
				if(_buttons[i].pushedMapPtr!=NULL) DrawMap2(_buttons[i].x,_buttons[i].y,_buttons[i].pushedMapPtr);				
				_buttons[i].state=BUTTON_DOWN;
				btnHandlerPtr(i,BUTTON_DOWN);					
				lastButtons=joy;	
				return;
			}
		
			if((joy&actionButton)==0 && _buttons[i].state==BUTTON_DOWN && _buttons[i].clicked==true){
				//button clicked
				if(_buttons[i].normalMapPtr!=NULL) DrawMap2(_buttons[i].x,_buttons[i].y,_buttons[i].normalMapPtr);
				_buttons[i].state=BUTTON_UP;
				_buttons[i].clicked=false;				
				btnHandlerPtr(i,BUTTON_DBLCLICK);					
				lastButtons=joy;	
				return;
			}
	
			if((joy&actionButton)==0 && _buttons[i].state==BUTTON_DOWN && !_buttons[i].clicked){
				//button clicked
				if(_buttons[i].normalMapPtr!=NULL) DrawMap2(_buttons[i].x,_buttons[i].y,_buttons[i].normalMapPtr);
				_buttons[i].state=BUTTON_UP;
				_buttons[i].clicked=true;				
				btnHandlerPtr(i,BUTTON_CLICK);					
				lastButtons=joy;
				dblClickDelay=0;	
				return;
			}			
	


			if(dblClickDelay==DBL_CLICK_DELAY && _buttons[i].clicked==true){
				//btnHandlerPtr(i,BUTTON_CLICK);
				_buttons[i].clicked=false;
				lastButtons=joy;	
				return;
			}
		
		}else{
		
			if((joy&actionButton)==0 && _buttons[i].state==BUTTON_DOWN){
				//button released outside of button
				if(_buttons[i].normalMapPtr!=NULL) DrawMap2(_buttons[i].x,_buttons[i].y,_buttons[i].normalMapPtr);
				_buttons[i].state=BUTTON_UP;
				btnHandlerPtr(i,BUTTON_UP);					
				lastButtons=joy;	
				return;
			}
		

		}

	

	}

	dblClickDelay++;
	lastButtons=joy;
}

