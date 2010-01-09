/*
 *  Uzebox(tm) Uzesynth/Music demo
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
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>


void drawVuMeter(unsigned char x, unsigned char y, unsigned char channel);

extern unsigned char volatile tr1_vol;
extern unsigned char volatile tr1_step_lo;
extern unsigned char volatile tr1_step_hi;

//import tunes
#include "data/Korobeiniki-3tracks.inc"
#include "data/ghost.inc"
#include "data/drmario_main.inc"
#include "data/nsmb.inc"
#include "data/ending.inc"

#include "data/testrisnt.inc"
#include "data/testrisnt_fast.inc"

//import patches
#include "data/patches.inc"

//import tiles
#include "data/fonts.pic.inc"
#include "data/composerTiles.pic.inc"
#include "data/composerTiles.map.inc"

const char strCopyright[] PROGMEM ="2008 UZE";
const char strWebsite[] PROGMEM ="HTTP://WWW.BELOGIC.COM/UZEBOX";

const char tetris1[] PROGMEM ="TETRIS 1";
const char tetris2[] PROGMEM ="TETRIS 2";
const char tetris3[] PROGMEM ="TETRIS 3";
const char tetris4[] PROGMEM ="TETRIS 4";

const char ghost[] PROGMEM ="GHOST & GOBLINS";
const char mario[] PROGMEM ="DR.MARIO";
const char nsmb[] PROGMEM ="NEW SUPER MARIO BROS.";


const char volume[] PROGMEM ="VOLUME:";

const char playingStr[] PROGMEM ="PLAYING:";
const char prioStr[] PROGMEM ="PRIORITY:";
const char noteStr[] PROGMEM ="NOTE:";
const char waveStr[] PROGMEM ="WAVE:";
const char patchStr[] PROGMEM ="PATCH:";

const char patchStreamStr[] PROGMEM ="STREAM POS:";	
const char envVolStr[] PROGMEM ="ENV VOL:";	
const char envStepStr[] PROGMEM ="ENV STEP:";	

const char ch1[] PROGMEM ="CH1";
const char ch2[] PROGMEM ="CH2";
const char ch3[] PROGMEM ="CH3";
const char ch4[] PROGMEM ="CH4";

const char midiIn[] PROGMEM ="MIDI IN ENABLED";

extern struct TrackStruct tracks[4];
void MoveCursor(unsigned char x,unsigned char y);

unsigned char y=5,x=9,ox=5,oy=9;





int main(){
	char c;
	unsigned int joy;

	bool playing=false;

	InitMusicPlayer(patches);

	SetFontTable(fonts);
	SetTileTable(composerTiles);

	ClearVram();

	DrawMap(9,1,map_title);

	Print(x+1,y+0,tetris1);
	Print(x+1,y+1,tetris2);
	//Print(x+1,y+2,tetris3);
	//Print(x+1,y+3,tetris4);
	Print(x+1,y+2,mario);
	Print(x+1,y+3,nsmb);
	Print(x+1,y+4,ghost);


	Print(6,10+6,playingStr);
	Print(5,11+6,prioStr);
	Print(9,12+6,noteStr);
	Print(9,13+6,waveStr);
	Print(8,14+6,patchStr);
	Print(3,15+6,patchStreamStr);
	Print(6,16+6,envVolStr);
	Print(5,17+6,envStepStr);

	Print(15,8+6,ch1);
	Print(20,8+6,ch2);
	Print(25,8+6,ch3);
	Print(30,8+6,ch4);

	for(c=0;c<18;c++){
		SetTile(15+c,15,2);
	}

	MoveCursor(x,y);

#if MIDI_IN == 1
	Print(12,26,midiIn);
#endif 


	while(1){
	
		joy=ReadJoypad(0);

		if(GetVsyncFlag()){
			ClearVsyncFlag();
	

			for(unsigned char t=0;t<4;t++){
				PrintHexByte((t*5)+15,10+6,tracks[t].patchPlaying);
				PrintHexByte((t*5)+15,11+6,tracks[t].priority);
				PrintHexByte((t*5)+15,12+6,tracks[t].note);
				PrintHexByte((t*5)+15,13+6,tracks[t].patchWave);
				PrintHexByte((t*5)+15,14+6,tracks[t].patchNo);			
				PrintHexByte((t*5)+15,15+6,*tracks[t].patchCommandStreamPos);
				PrintHexByte((t*5)+15,16+6,tracks[t].envelopeVol);
				PrintHexByte((t*5)+15,17+6,tracks[t].envelopeStep);

				drawVuMeter((t*5)+17,14+6,t);
			
			}

		}



		if(joy&BTN_LEFT){
			while((ReadJoypad(0)&BTN_LEFT)!=0); //wait for key release		
		}

		if(joy&BTN_RIGHT){
			while((ReadJoypad(0)&BTN_RIGHT)!=0); //wait for key release		
		}

		if(joy&BTN_A){
			while((ReadJoypad(0)&BTN_A)!=0); //wait for key release
		}
		if(joy&BTN_B){
			while((ReadJoypad(0)&BTN_B)!=0); //wait for key release
		}


		if(joy&BTN_START){
			if(playing){
				StopSong();
			}else{
				if(y==5)StartSong(song_korobeiniki);
				if(y==6)StartSong(song_testrisnt);
				//if(y==7)StartSong(song_testrisnt_fast);
				//if(y==8)StartSong(song_ending);
				if(y==7)StartSong(song_drmario_main);
				if(y==8)StartSong(song_nsmb);
				if(y==9)StartSong(song_ghost);

			}
			playing=!playing;
			while((ReadJoypad(0)&BTN_START)!=0); //wait for key release
		}

		if(joy&BTN_UP){
			if(y>5){
				TriggerFx(1,0x90,true);			
				y--;
				MoveCursor(x,y);
			}
			while((ReadJoypad(0)&BTN_UP)!=0);
		}else if(joy&BTN_DOWN){
			if(y<9){
				TriggerFx(1,0x90,true);
				y++;
				MoveCursor(x,y);
			}
			while((ReadJoypad(0)&BTN_DOWN)!=0); //wait for key release
		}


	}
}

void MoveCursor(unsigned char x, unsigned char y){
	
	SetTile(ox,oy,0);
	SetTile(x,y,28);
	ox=x;
	oy=y;
}


void drawVuMeter(unsigned char x, unsigned char y, unsigned char channel){
	unsigned char m,b,v;

	v=mixer.channels.all[channel].volume>>1;
	m=v/32;
	b=(v-(m*32))/4;

	if(m>=3){					
		SetTile(x,y+0,b+20);
		SetTile(x,y+1,19);
		SetTile(x,y+2,19);
		SetTile(x,y+3,19);
	}else if(m==2){
		SetFont(x,y+0,0);
		SetTile(x,y+1,b+12);
		SetTile(x,y+2,19);
		SetTile(x,y+3,19);				
	}else if(m==1){
		SetFont(x,y+0,0);
		SetFont(x,y+1,0);
		SetTile(x,y+2,b+12);
		SetTile(x,y+3,19);				
	}else if (m==0 && v!=0){
		SetFont(x,y+0,0);
		SetFont(x,y+1,0);
		SetFont(x,y+2,0);
		SetTile(x,y+3,b+12);				
	}else{
		SetFont(x,y+0,0);
		SetFont(x,y+1,0);
		SetFont(x,y+2,0);
		SetFont(x,y+3,0);				
	}
}







