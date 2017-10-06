/*
 *
 *  SPI Ram Streaming Music Demo
 *  by Lee Weber(D3thAdd3r) 2017
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

#include <ctype.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
//#include "data/tiles.inc"
#include "data/font-8x8.inc"
#include "data/compressedsong.inc"
#include "data/patches.inc"
#include <sdBase.h>
#include <spiram.h>


const char fileName[] PROGMEM = "SD_MUSICDAT";

void CustomWaitVsync(u8 frames);
void UpdateEqualizer();

//u8 sdInUse;
uint32_t songOff = 0;
extern s8 songSpeed;
extern bool playSong;
extern volatile u16 songPos;
extern volatile u16 loopStart;
extern volatile u16 loopEnd;
extern Track tracks[CHANNELS];
extern u8 ram_tiles[];
u8 songNum;
extern u16 songStalls;
extern u8 songBufIn,songBufOut;
u16 loopEndFound;
u16 bufferLimiter;
u16 bpfLimiter;
u16 songBase;
u8 doSongBuffer;
long sectorStart;


u16 padState, oldPadState;
u8 visualizer[SCREEN_TILES_H];
u8 visualizerHigh[SCREEN_TILES_H];
//u8 equalizer[SCREEN_TILES_H];
//u8 equalizerHigh[SCREEN_TILES_H];

int main(){
	InitMusicPlayer(patches);
	SetTileTable(font);
	//SetFontTilesIndex(TILES_SIZE);
	ClearVram();

	sdCardInitNoBuffer();
	SpiRamInit();

	if((sectorStart = sdCardFindFileFirstSectorFlash(fileName)) == 0){
		Print(0,0,PSTR("FILE SD_MUSIC.DAT NOT FOUND ON SD CARD"));
		while(1);	
	}

	//load all music resources from the SD card into SPI ram
	SetRenderingParameters(FRAME_LINES-10,8);//decrease the number of drawn scanlines for more free cycles to load data
	for(uint8_t i=0;i<32*2;i++){//load 512 bytes at a time from SD into ram, then from ram into SPI ram, for a total of 32K or 64 sectors

		sdCardCueSectorAddress(sectorStart+i);
		sdCardDirectReadSimple(vram+256,512);
		sdCardStopTransmission();
 		SpiRamWriteFrom(0,(uint16_t)(i*512),vram+256,512);
	}
	ClearVram();//clear the gibberish we wrote over vram
	SetRenderingParameters(FIRST_RENDER_LINE,FRAME_LINES);//increase the number of drawn scanlines so the whole screen shows


	Print(0,0,PSTR("SONGNUM  :"));
	Print(0,1,PSTR("SONGPOS  :"));
	Print(0,2,PSTR("SONGSPEED:"));
	Print(0,3,PSTR("LOOPSTART:"));
	Print(0,4,PSTR("LOOPEND  :"));
	Print(0,5,PSTR("SONGBASE :"));

	Print(0,6,PSTR("BUFFERED :"));
	Print(0,7,PSTR("FILL RATE:"));
	Print(0,8,PSTR("BUF LIMIT:"));
	Print(0,9,PSTR("STALLS   :"));

	bufferLimiter = 16;//let the user simulate different buffer sizes in real time
	bpfLimiter = 16;//let the user simulate different fill speeds(for SPI ram it is likely ok to have it always fill the buffer fully)
	songSpeed = 0;
	songBase = SpiRamReadU32(0,0);//read the first entry from the directory, this is the offset to the first song

	SpiRamSeqReadStart(0,songBase);//make our buffering code start at the first byte of the song
	WaitVsync(1);
	doSongBuffer = 1;//let the CustomWaitVsync() fill the buffer. We did not want it to do that before, since it would be reading the directory
	CustomWaitVsync(2);//let it buffer some data
	songStalls = 0;//the first CustomWaitVsync() will detect a stall, but that doesn't count because we weren't going yet
	StartSong();//start the song now that we have some data

		for(uint8_t j=0;j<64;j++){//break;
			ram_tiles[(0*64)+j] = pgm_read_byte(&font[(29*64)+j])&0b00101000;
			ram_tiles[(1*64)+j] = pgm_read_byte(&font[(10*64)+j])&0b00101101;
			ram_tiles[(2*64)+j] = pgm_read_byte(&font[(3*64)+j])&0b00000111;
		}

	while(1){
		oldPadState = padState;
		padState = ReadJoypad(0);

		if(padState & BTN_UP && !(oldPadState & BTN_UP) && bufferLimiter < SONG_BUFFER_SIZE)
			bufferLimiter++;
		if(padState & BTN_DOWN && !(oldPadState & BTN_DOWN) && bufferLimiter > SONG_BUFFER_MIN)
			bufferLimiter--;

		if(padState & BTN_LEFT && !(oldPadState & BTN_LEFT) && bpfLimiter)
			bpfLimiter--;

		if(padState & BTN_RIGHT && !(oldPadState & BTN_RIGHT) && bpfLimiter < bufferLimiter)
			bpfLimiter++;

		if(padState & BTN_SL && !(oldPadState & BTN_SL))
			songSpeed--;

		if(padState & BTN_SR && !(oldPadState & BTN_SR))
			songSpeed++;

		if(padState & BTN_SELECT && !(oldPadState & BTN_SELECT)){
songBufIn = songBufOut = 0;
songOff = songBase;
SpiRamSeqReadEnd();
SpiRamSeqReadStart(0,songBase);
StartSong();
			/*if(++songNum > 2)
				songNum = 0;
			SpiRamSeqReadEnd();//end the sequential read we were using to fill the buffer
			songBase = SpiRamReadU16(0,songNum);//read the start of this song
			SpiRamSeqReadStart(0,songBase);//start the sequential read again so buffering can happen
			while(SongBufBytes())
				SongBufRead();//eat any buffered bytes from the last song
			StartSong();*/
		}
while(padState & BTN_START){
		oldPadState = padState;
		padState = ReadJoypad(0);
CustomWaitVsync(1);
}

		PrintInt(14,0,songNum,1);
		PrintInt(14,1,songPos,1);
		PrintInt(14,2,songSpeed,1);
		PrintInt(14,3,loopStart,1);
		PrintInt(14,4,loopEndFound,1);
		PrintInt(14,5,songBase,1);

		PrintInt(14,6,SongBufBytes(),1);
		PrintInt(14,7,bpfLimiter,1);
		PrintInt(14,8,bufferLimiter,1);
		PrintInt(14,9,songStalls,1);

		UpdateEqualizer();
		CustomWaitVsync(1);

	}
}

void CustomWaitVsync(u8 frames){//we do a best effort to keep up to the demand of the song player.

	while(frames){
		if(loopEnd){//we read past the end of the song..luckily it is padded with bytes from the loop start
			SpiRamSeqReadEnd();
			loopEndFound = loopEnd;//just to display it once found(not needed for games)
			songOff = (songOff-loopEnd)+loopStart;
			loopEnd = 0;//since we immediately zero it so we don't keep doing it

			//WaitVsync(1);//asm volatile("lpm\n\tlpm\n\t");
			SpiRamSeqReadStart(0,(uint32_t)(songBase+songOff));//read from the start of the song, plus the offset we already "read past"
		}

		u8 total_bytes = 0;
		while(!GetVsyncFlag()){//try to use cycles that we would normally waste

			if(doSongBuffer && !SongBufFull() && SongBufBytes() < bufferLimiter && total_bytes < bpfLimiter){
				SongBufWrite(SpiRamSeqReadU8());
				songOff++;				
				total_bytes++;				
			}			
		}

		ClearVsyncFlag();
		frames--;
	}
}


void UpdateEqualizer(){
////mix_bank,mix_buf

//return;
	//for(uint16_t i=(mix_bank==1?0:262);i<(mix_bank==1?261:524);i+=2){
	//static uint32_t avg,lastavg;
	//static uint8_t color = 0b00000111;
	for(uint8_t i=1;i<SCREEN_TILES_H-1;i++){
		//avg = 0;
		for(uint8_t j=0;j<9;j++){
			visualizer[i] += mix_buf[(262*mix_bank)+(i*9)+j]/2;
			//avg += mix_buf[(262*mix_bank)+(i*9)+j]/2;
		}
		visualizer[i] /= 8;//9;
		if(visualizer[i] > 2)
			visualizer[i] -= 3;
		else
			visualizer[i] = 0;
		if(visualizer[i] > visualizerHigh[i])
			visualizerHigh[i] = visualizer[i];
		else if(mix_bank && visualizerHigh[i])
			visualizerHigh[i]--;
		//avg *= 2;
		//avg /= 11;
		//avg /= 62;
		
		for(uint8_t j=0;j<17;j++){
			if(visualizer[i] > j)
				vram[i+((SCREEN_TILES_V-1-j)*VRAM_TILES_H)] = 0;//RAM_TILES_COUNT+29;
			else if(false)//visualizerHigh[i] > j)
				vram[i+((SCREEN_TILES_V-1-j)*VRAM_TILES_H)] = 1;//RAM_TILES_COUNT+10;
			else if(visualizerHigh[i] == j)
				vram[i+((SCREEN_TILES_V-1-j)*VRAM_TILES_H)] = 2;//RAM_TILES_COUNT+3;
			else
				vram[i+((SCREEN_TILES_V-1-j)*VRAM_TILES_H)] = RAM_TILES_COUNT+0;
		}

	}

//	if(avg < lastavg){

//		uint8_t t = (color & 0b00000111);
//		color &= ~0b00000111;
//		if(t)
//			t--;
//		color |= t;
//color--;
//	}else{
//		uint8_t t = (color & 0b00000111);
//		color &= ~0b00000111;
//		if(t < 7)
//			t++;
//		color |= t;

//	}

	//color &= 0b11111000;
	//color = avg>>4;//255-(avg/10);

//	lastavg = avg;


}





