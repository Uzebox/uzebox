/*
 *  Uzebox(tm) Uzeamp
 *  Copyright (C) 2009-2024 Uze
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
 *
 *  ---
 *
 *  Uzeamp is a music player that supports standard WAVE files encoded in 8-bit mono with 15734Hz sampling rate.
 *
 *  Features:
 *  - Plays WAV files, 8-bit unsigned mono file at 15734Hz (the NTSC line rate).
 *  - Supports WAV file metadata extension for song name and artist.
 *  - Supports FAT16/FAT32, SD and SDHC, file fragmentation
 *  - Supports very old and slow SD cards that were shipped in the Uzebox kits
 *  - Support skins/color schemes
 *  - VU meter
 *  - Auto-play next song
 *
 *  Compatible WAVE files can be made using FFMPEG. This will produce a file in the correct format and striping
 *  all metadata from the orginal file while adding custom song title and artist name.
 *
 *  ffmpeg -i input.mp3 -f wav -bitexact -acodec pcm_u8 -ac 1 -ar 15734 -map_metadata -1 -metadata title="<song title>" -metadata artist="<song artist>"  output.wav
 *
 *  See the wiki page for details: https://uzebox.org/wiki/Uzeamp
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <uzebox.h>
#include <uzefs/uzefs.h>
#include "data/tiles.inc"

#define BFONT 	0
#define SFONT	1
#define BLANK 	0
#define OK		0
#define MAX_DISP_FILES 	11
#define MAX_FILES 		118		//what the video ring holds after the VRAM (11 bytes a file)
#define CURSOR_INIT_X	3
#define CURSOR_INIT_Y	11
#define VU_METER_Y_OFFSET 5

#define SCROLL_NONE	0
#define SCROLL_UP	1
#define SCROLL_DOWN	2

#define WAVE_PCM 1

#define PLAYER_STOPPED 			0x00
#define PLAYER_STARTED 			0x01
#define PLAYER_PAUSED  			0x02
#define PLAYER_SONG_ENDED  		0x03
#define PLAYER_LOADING  		0x04

#define ERR_INVALID_FORMAT		0x01
#define ERR_INVALID_STRUCTURE	0x02
#define ERR_NOT_FOUND			0x03

/**
 * Struct representing the wave file header
 * see: https://www.robotplanet.dk/audio/wav_meta_data/
 * 		http://soundfile.sapp.org/doc/WaveFormat/
 */
typedef struct{
	//header
	uint8_t chunkID[4];
	uint32_t chunkSize;
	uint8_t format[4];

	//fmt chunk
	uint8_t subchunk1ID[4];
	uint32_t subchunk1Size;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;

	//data chunk
	uint8_t subchunk2ID[4];
	uint32_t subchunk2Size;

} RIFFheader;

typedef struct{
	//data chunk
	uint8_t subchunkID[4];
	uint32_t subchunkSize;
	uint8_t buffer[];
} RIFFDataChunk;

/**
 * Union struct to save space
 */
typedef union {
	uint8_t buffer[512];
	RIFFheader riffHeader;
	RIFFDataChunk dataChunk;
} SdSector;

/**
 * The kinds of files: the extension, and the icon before the list's number (map_icons in
 * data/gconvert.xml, in this order). Only the first TYPES_PLAYED are listed: SID and NSF have
 * their icon, not yet a player. A new kind: its extension here, its icon in the sheet's row 4
 * (and map_icons one wider), its player in startFile.
 */
enum { TYPE_WAV, TYPE_UZM, TYPE_SID, TYPE_NSF };
static const char typeExt[][5] PROGMEM = { ".WAV", ".UZM", ".SID", ".NSF" };
#define TYPES_PLAYED 2

/**
 * The files read from the directory.
 *
 * The list lives in the video ring (uzmmode, UZM_RING) after mode 5's VRAM: a movie's frames
 * need the ring, and while they play neither the text nor the list is shown. 11 bytes an entry.
 */
typedef struct __attribute__((packed)) {
	char filename[8];				//8.3 name minus extension, NUL-padded (no NUL when 8 long)
	uint16_t seconds;				//playing time
	uint8_t type;					//TYPE_WAV, TYPE_UZM...
} SdFile;

#define isMovie(i)	(files[i].type==TYPE_UZM)

typedef struct {
	char filename[13];
	char info[129];					//Buffer for the artist name and display info used in the scroller
	uint32_t currentSector;
	uint32_t sectorsCount;
	uint32_t position;
	uint32_t riffDataOffset;
} Song;


void animateTextLine(char *infoSongStr, bool reset);
void animateTextLine_P(const char *infoSongStr, bool reset);
int16_t findTag(char *tag, char *dest,uint16_t destLenght,uint8_t dataOffset);
uint8_t  getChar(int16_t index);
void getFilePlayingTime(uint16_t seconds,char *buffer);
uint16_t wavSeconds(uint32_t size);
uint16_t movieSeconds(uint32_t size);
char *fileName(uint8_t index);
void drawFileLine(uint8_t y,uint8_t index,uint8_t number,uint8_t byteDigits);
void startFile(uint8_t index);
void startMovie(uint8_t index);
void playMovie(void);
#define BTN_CUE_FWD  (BTN_SR|BTN_RIGHT)	//cue forward: the right shoulder or the d-pad's right
#define BTN_CUE_BACK (BTN_SL|BTN_LEFT)	//cue back: the left shoulder or the d-pad's left
uint32_t picToSector(uint32_t pic);
void drawScreen(void);
void scanFiles(void);
void redrawAll(void);
void drawSlowWarning(void);
uint8_t loadWaveInfoBlock(SdFile *file);
void initSdCard();
void drawDigits(uint32_t currentSectorNo );
void PrintByte2(uint8_t x,uint8_t y,uint8_t val,bool zeropad,uint8_t maxDigits, uint8_t font);
void PrintChar2(uint8_t x,uint8_t y,char ch,uint8_t font);
void PrintString2(uint8_t x, uint8_t y, char *str, uint8_t font);
void PrintString2_P(uint8_t x, uint8_t y, const char *str, uint8_t font);
void processPlayer();
void sdError(uint8_t code,uint8_t res);
void setString(char *str,char c,uint8_t maxLen);
uzefs_res seekSong(uint32_t pos);
void startSong(int32_t SectorNo,bool loadInfoBlock);
void updateVuMeter();
void updateProgressBar(uint32_t currentSector);
void updateFilesList(uint8_t scrollType);
bool process(bool buttonPressed,uint8_t buttonID,bool repeatOn);


//UzeFS vars
uzefs_info fno;		//Directory entry
uzefs_dir dir;		//Directory listing position
uzefs_res fres;		//UzeFS function common result code
uint8_t fh;				//Handle of the open song
bool fileOpen=false;	//true when fh is an open file

//Global variables
SdSector sector;				//buffer to hold an SD sector and RIFF header
#define files ((SdFile *)(UZM_RING + VRAM_SIZE))	//infos about the files found on the card
_Static_assert(VRAM_SIZE + MAX_FILES * sizeof(SdFile) <= UZM_RING_SIZE, "the file list does not fit in the video ring");
uint8_t fileCount=0;					//number of files loaded in files[]
uint16_t playingFileIndex=0;			//file in the files[] array currently selected
uint8_t  playerStatus=PLAYER_STOPPED;//current status of the player
int16_t vuFrameMaxSample=0;			//loudest sample played in the last vsync (vu meter)
uint8_t cursorSongIndex=0;			//Song index at the cursor
Song currentSong;				//Variables for the current song

/**
 * The movie being played or stopped: its first picture's card sector (the file is in one
 * piece), its pictures, the next one to show.
 */
typedef struct {
	uint32_t sect0;
	uint32_t pictures;
	uint32_t pic;
} Movie;
Movie movie;
volatile bool moviePlaying=false;	//a movie plays: no song is read, mix_buf is the movie's
uint8_t listTop=0;					//the list's first line when redrawn whole (SCROLL_NONE)
uint8_t cursorX,cursorY,cursorPrevY;	//the list's cursor, on the screen
bool redrawPending=false;			//a movie used the ring: redrawAll() before the list is used

/**
 * The card test (plan M5), at the start of every movie until the user plays one anyway: the
 * first frames restart the read as a still does (pause, steps, cue), then the first seconds of
 * play are watched. Its figures stay here (for the warning and the checker's GDB reads).
 */
#define SLOW_PLAY  1		//rows came late while playing, or the ring ran low
#define SLOW_STILL 2		//a restart (still, step, cue) left the ring short
#define TEST_STILLS 3		//black frames that restart the read, at the movie's start
#define TEST_FRAMES 180		//frames of play watched: 3 seconds
#define TEST_LEVEL  512		//unread bytes a frame must leave at least: a quarter of the ring
#define TEST_STILL  480		//bytes a still's blank must store at least: 3 rows
struct {
	uint16_t low;			//the fewest unread bytes after a frame of play
	uint16_t still;			//the fewest bytes a still's blank stored
	uint16_t late;			//rows late (lines that found theirs missing) in the test
} cardTest;
#ifndef UZEAMP_REPORT
#define UZEAMP_REPORT 0		//1 (make EXTRA=-DUZEAMP_REPORT=1): a console check build: the card's CID
#endif						//and the card test's figures to the UART (115,200 baud), the first movie started by itself
#if UZEAMP_REPORT
#include <uzefs/uzefs_sd.h>
static void rep_putc(char c){
	loop_until_bit_is_set(UCSR0A,UDRE0);
	UDR0=c;
}
static void rep_str_P(const char *s){
	char c;
	while((c=pgm_read_byte(s++))) rep_putc(c);
}
static void rep_hex(uint8_t v){
	rep_putc("0123456789ABCDEF"[v>>4]);
	rep_putc("0123456789ABCDEF"[v&15]);
}
static void rep_num(uint16_t v){
	char b[5];
	uint8_t i=0;
	do{ b[i++]='0'+v%10; v/=10; }while(v);
	while(i) rep_putc(b[--i]);
}
#endif
uint8_t movieSlow=0;
uint16_t warnPrev;			//the buttons when the warning was drawn		//SLOW_PLAY or SLOW_STILL: the movie stopped, the main loop warns
bool cardAccepted=false;	//the user played a movie anyway: no test until the console resets

/**
 * The card test's figures to the UART (UZEAMP_REPORT builds): at what point, the still's
 * fewest bytes, rows late, the fewest unread bytes in play, the verdict.
 */
static void cardReport(char when){
#if UZEAMP_REPORT
	rep_putc(when);
	rep_str_P(PSTR(" still "));	rep_num(cardTest.still);
	rep_str_P(PSTR(" late "));	rep_num(cardTest.late);
	rep_str_P(PSTR(" low "));	rep_num(cardTest.low);
	rep_str_P(PSTR(" slow "));	rep_num(movieSlow);
	rep_str_P(PSTR("\r\n"));
#else
	(void)when;
#endif
}

/**
 * User callback invoqued on each VSYNC (60 fps)
 */
void VsyncCallBack(void){
	if(!moviePlaying) processPlayer();
	if(playerStatus ==  PLAYER_LOADING) animateTextLine_P(NULL,false);
}

/**
 * Used for debugging and printing with printf().
 * Escape characters are suported:
 * \n - Line feed
 * \r - Back to beginning of the same line
 * \f - Back to top of the screen
 */
int putchar_printf(char c, FILE *stream) {
	static uint8_t x=0,y=0;
	if(c=='\n'){
		x=1;y++;
	}else if(c=='\r'){
		x=1;
	}else if(c=='\f'){
		x=1;y=1;
	}else if(c>=' ' && c <= '~'){
		PrintChar2(x++,y,c,BFONT);
	}
	return 0;
}
static FILE stream = FDEV_SETUP_STREAM(putchar_printf, NULL, _FDEV_SETUP_WRITE);


int main(){
	//Skins data. Uzed to mask bits in the video port's data direction register hence altering the displayed colors.
	uint8_t currentSkin=0,cursorDelay,cursorState;
	uint8_t skins[]={0xff,0xfc,0xe7,0xbe,0xbd,0x77,0x3e,0x27};
	uint16_t buttons;
	bool cueing;
	int32_t cueSector;

	stdout=&stream;
	SetTileTable(main_tileset);
	//SetSpritesTileTable(main_tileset);
	SetUserPreVsyncCallback(&VsyncCallBack);
	drawScreen();

	initSdCard();
#if UZEAMP_REPORT
	{
		uint8_t cid[16];
		UBRR0=30;							//28.636 MHz / (8 x 31): 115,470 baud
		UCSR0A=(1<<U2X0);
		UCSR0C=(1<<UCSZ01)|(1<<UCSZ00);
		UCSR0B=(1<<TXEN0);
		rep_str_P(PSTR("\r\nUZEAMP CID "));
		if(uzefs_sd_reg(0,cid)==UZEFS_OK){
			for(uint8_t i=0;i<16;i++) rep_hex(cid[i]);
			rep_putc(' ');
			for(uint8_t i=3;i<8;i++) rep_putc(cid[i]);
		}
		rep_str_P(PSTR("\r\n"));
	}
#endif

	PrintString2_P(10,15,PSTR("LOADING..."),BFONT);
	scanFiles();
    PrintString2_P(10,15,PSTR("          "),BFONT);	//LOADING... read
    if(fileCount==0) PrintString2_P(CURSOR_INIT_X,15,PSTR("NO SONGS OR MOVIES FOUND"),BFONT);
    updateFilesList(SCROLL_NONE);


    /**
     * Main loop that operate the GUI
     */
    cueSector=0;
    cueing=false;
    cursorState=1;cursorDelay=0;
    cursorSongIndex=0;
    cursorX=CURSOR_INIT_X;cursorY=cursorPrevY=CURSOR_INIT_Y;

  	while(1)
  	{
		WaitVsync(1);
		if(redrawPending){
			redrawPending=false;
			redrawAll();
			if(movieSlow) drawSlowWarning();
			DDRC=skins[currentSkin];		//black since the movie ended: the skin back on the new screen
		}

		//the card test's warning, in the list's place: A plays the movie anyway, B (or START,
		//SELECT) goes back to the list
		if(movieSlow){
			buttons=ReadJoypad(0);
			uint16_t pressed=buttons & ~warnPrev;
			warnPrev=buttons;
			if(pressed & (BTN_A|BTN_B|BTN_START|BTN_SELECT)){
				while(ReadJoypad(0)) WaitVsync(1);	//the press must not reach the list
				movieSlow=0;
				Fill(3,11,24,11,BLANK);
				if(pressed & BTN_A){
					cardAccepted=true;
					playMovie();
				}else{
					updateFilesList(SCROLL_NONE);
				}
			}
			continue;
		}

		buttons=ReadJoypad(0);

#if UZEAMP_REPORT
		{	//the first movie, 2 s after the list: the card test runs with nobody at the controller
			static uint8_t autoStart=120;
			if(autoStart && !--autoStart){
				for(uint8_t i=0;i<fileCount;i++){
					if(isMovie(i)){
						rep_str_P(PSTR("MOVIE "));
						for(uint8_t j=0;j<8 && files[i].filename[j];j++) rep_putc(files[i].filename[j]);
						rep_str_P(PSTR("\r\n"));
						cursorSongIndex=playingFileIndex=i;
						startFile(i);
						break;
					}
				}
			}
		}
#endif

		//Start a new song, pause and resume the same song
		if(process(buttons&BTN_START,0,false)){
			if(playerStatus == PLAYER_STARTED && cursorSongIndex==playingFileIndex){
				playerStatus = PLAYER_PAUSED;
			}else if(cursorSongIndex!=playingFileIndex || playerStatus == PLAYER_STOPPED){
				playingFileIndex=cursorSongIndex;
				startFile(playingFileIndex);
			}else if(isMovie(playingFileIndex)){
				playMovie();				//a stopped movie: on from its picture
			}else{
				playerStatus = PLAYER_STARTED;
			}
		}

		//Move cursor up
		if(process(buttons&BTN_UP,1,true)){
			if(cursorSongIndex>0){
				if(cursorY>CURSOR_INIT_Y && cursorSongIndex<(MAX_DISP_FILES))cursorY--;
				cursorSongIndex--;
				cursorState=1;
				cursorDelay=0;
				updateFilesList(SCROLL_DOWN);
			}
		}

		//Move cursor down
		if(process(buttons&BTN_DOWN,2,true)){

			if(cursorSongIndex<(fileCount-1)){
				if(cursorSongIndex<(MAX_DISP_FILES-1))cursorY++;
				cursorSongIndex++;
				cursorState=1;
				cursorDelay=0;
				updateFilesList(SCROLL_UP);
			}
		}

		//Cycle through the various color schemes
		if(process(buttons&BTN_SELECT,3,false)){
			currentSkin++;
			if(currentSkin==sizeof(skins))currentSkin=0;
			DDRC=skins[currentSkin];
		}

		//Fast forward the song. Pressing A button at the same time will cue faster
		if(buttons&BTN_CUE_FWD){
			if(playerStatus == PLAYER_STARTED){
				if(!cueing){
					cueSector=currentSong.currentSector;
				}
				cueSector+=50;//fast-forward
				if(buttons&BTN_A)cueSector+=400;//fast-forwards even faster

				if(cueSector>=(currentSong.sectorsCount-20)){
					cueSector=currentSong.sectorsCount-20;
				}
				cueing=true;
			}
		}

		//Fast backward the song. Pressing A button at the same time will cue faster
		if(buttons&BTN_CUE_BACK){
			if(playerStatus == PLAYER_STARTED){
				if(!cueing){
					cueSector=currentSong.currentSector;
				}
				cueSector-=100; //fast-backward
				if(buttons&BTN_A)cueSector-=400;//fast-backwards even faster
				if(cueSector<0){
					cueSector=0;
				}
				cueing=true;
			}
		}

		//Check if there is more songs to play (after a movie, once the list is read again)
		if(playerStatus == PLAYER_SONG_ENDED && !redrawPending){
			if(cursorSongIndex<(fileCount-1)){
				if(cursorSongIndex<(MAX_DISP_FILES-1))cursorY++;
				cursorSongIndex++;
				playingFileIndex=cursorSongIndex;
				updateFilesList(SCROLL_UP);
				startFile(playingFileIndex);
			}else{
				playerStatus = PLAYER_STOPPED;
			}
		}

		//When releasing the fast-forward/fast-backward button
		//stop the song then restart it at the new position
		if( (buttons&(BTN_CUE_BACK|BTN_CUE_FWD))==0 && cueing==true){
			cueing=false;
			startSong(cueSector,false);
		}

		//update digits and cue slider
		if(cueing){
			drawDigits(cueSector);
			updateProgressBar(cueSector);
		}else if(playerStatus == PLAYER_STARTED || playerStatus == PLAYER_STOPPED){
			drawDigits(currentSong.currentSector);
			updateProgressBar(currentSong.currentSector);
		}

		//update the cursor
		PrintChar2(cursorX-1,cursorPrevY,' ',BFONT); //erase previous cursor location
		cursorPrevY=cursorY;
		if(playerStatus != PLAYER_STARTED)cursorState=1;
		if(cursorState == 1){
			PrintChar2(cursorX-1,cursorY,'^',BFONT);
		}else{
			PrintChar2(cursorX-1,cursorY,' ',BFONT);
		}
		cursorDelay++;
		if(cursorDelay>20){
			cursorDelay=0;
			cursorState^=1;
		}

		updateVuMeter();
		animateTextLine(NULL,false);
   }

}
/**
 * Handle auto-repeat
 */
bool process(bool buttonPressed,uint8_t buttonID,bool repeatOn){
	static uint8_t autoRepeatDelay[4],autoRepeatRate[4];


	if(!(buttonPressed)){
		autoRepeatRate[buttonID]=20;
		autoRepeatDelay[buttonID]=0;
		return false;
	}

	if(buttonPressed && autoRepeatDelay[buttonID]==0){
		autoRepeatDelay[buttonID]=autoRepeatRate[buttonID];
		return true;
	}
	if(autoRepeatDelay[buttonID]>0 && repeatOn){
		autoRepeatDelay[buttonID]--;
		if(autoRepeatDelay[buttonID]==0){
			autoRepeatRate[buttonID]=3;
		}
	}
	return false;
}

/**
 * Redraws the whole file list windows accounting for the selected song and scrolling
 */
/**
 * One line of the list: its number, the name, MOV for a movie, the playing time.
 */
void drawFileLine(uint8_t y,uint8_t index,uint8_t number,uint8_t byteDigits){
	char timebuf[9];

	for(uint8_t j=1;j<16;j++) PrintChar2(CURSOR_INIT_X+j,y,'_',BFONT);			//erase line
	SetTile(CURSOR_INIT_X,y,pgm_read_byte(&map_icons[2+files[index].type]));	//its kind
	PrintByte2(CURSOR_INIT_X+byteDigits,y,number,true,byteDigits,BFONT);		//file number
	PrintChar2(CURSOR_INIT_X+byteDigits+1,y,'.',BFONT);							//separator
	PrintString2(CURSOR_INIT_X+byteDigits+2,y,fileName(index),BFONT);			//filename
	setString(timebuf,'_',8);													//init time string buffer
	getFilePlayingTime(files[index].seconds,timebuf);							//playing time
	PrintString2(CURSOR_INIT_X+16,y,timebuf,BFONT);
}

void updateFilesList(uint8_t scrollType){
	uint8_t disp=0;
	uint8_t byteDigits;

	//calculate how much space we need to allocate for the file number
	if(fileCount<10){
		byteDigits=1;
	}else if(fileCount<100){
		byteDigits=2;
	}else{
		byteDigits=3;
	}

	if(cursorSongIndex>=MAX_DISP_FILES && scrollType==SCROLL_UP){
		disp=cursorSongIndex-MAX_DISP_FILES+1;

		//scroll up content of window
		for(uint8_t y=11;y<11+MAX_DISP_FILES-1;y++){
			for(uint8_t x=CURSOR_INIT_X;x<27;x++){
				vram[(VRAM_TILES_H*y)+x]=vram[(VRAM_TILES_H*(y+1))+x];
			}
		}
		//add new file at bottom
		drawFileLine(MAX_DISP_FILES+10,MAX_DISP_FILES+disp-1,MAX_DISP_FILES+disp,byteDigits);

	}else if(cursorSongIndex>=MAX_DISP_FILES-1 && scrollType==SCROLL_DOWN){
		disp=cursorSongIndex-MAX_DISP_FILES+1;

		//scroll down content of window
		for(uint8_t y=9+MAX_DISP_FILES;y>9;y--){
			for(uint8_t x=CURSOR_INIT_X;x<27;x++){
				vram[(VRAM_TILES_H*(y+1))+x]=vram[(VRAM_TILES_H*y)+x];
			}
		}
		//add new file at top
		drawFileLine(11,disp,disp+1,byteDigits);

	}else if(scrollType==SCROLL_NONE){
		disp=listTop;
		for(uint8_t i=0;(i<MAX_DISP_FILES && i+disp<fileCount);i++){
			drawFileLine(i+11,i+disp,i+disp+1,byteDigits);
		}
	}

}

/**
 * Draws the progress bar
 */
void updateProgressBar(uint32_t currentSector){
	if(currentSong.sectorsCount==0 && currentSector==0) return;

	SetTile(3,8,128); //beginning
	SetTile(26,8,138); //end

	//bar is 24 tiles wide * 8 = 192 pixels
	uint8_t pixels=192*currentSector/currentSong.sectorsCount;
	uint8_t fullTiles=pixels/8;
	uint8_t tilePixels=pixels%8;

	for(uint8_t i=0;i<24;i++){
		if(i<fullTiles){
			SetTile(i+3,8,137);
		}else if(tilePixels>0){
			SetTile(i+3,8,129+tilePixels);
			tilePixels=0;
		}else{
			SetTile(i+3,8,129);
		}
	}
}

/**
 * Display a simple vu meter.
 */
#define VU_METER_DECAY_RATE 1
#define VU_METER_WAIT_FRAME	3
void updateVuMeter(){
	const uint8_t thresholds[8] = {2, 11, 26, 47, 77, 118, 175, 250};
	static uint8_t wait=0,current_segments=0,max=0;

	uint8_t *buf=mix_buf;
	if(mix_bank==1) buf+=MIX_BANK_SIZE;

	//get the loudest sample for the bank
	uint8_t sample=0;
	for(int16_t i=0;i<MIX_BANK_SIZE;i++){
		sample=(abs((int8_t)(buf[i]+0x80))*2);
		if(sample>max)max=sample;
	}

	if(wait==VU_METER_WAIT_FRAME){
		//check how many thresholds the audio value exceeds
		uint8_t segments = 0;
		for (int16_t i = 0; i < 8; i++) {
			if (max >= thresholds[i]) {
				segments++;
			} else {
				break; //stop if audio value is below the current threshold
			}
		}

	   //if the new target is higher, update immediately
		if (segments > current_segments) {
			current_segments = segments;
		} else if (current_segments > 0) {
			//otherwise, apply decay
			current_segments = current_segments > VU_METER_DECAY_RATE ? current_segments - VU_METER_DECAY_RATE : 0;
		}

		for (int16_t i = 0; i < 8; i++) {
			if (i < current_segments) {
				DrawMap2(19+i,VU_METER_Y_OFFSET,map_vuMeterBarOn);
			} else {
				DrawMap2(19+i,VU_METER_Y_OFFSET,map_vuMeterBarOff);
			}
		}
		wait=0;
		max=0;
	}else{
		wait++;
	}
}

/**
 * Called once per vsync in order to load the audio ring buffer
 * with new data from the SD card.
 */
void processPlayer()
{
	uint8_t *buf=mix_buf;
	uint16_t bytesRead=0;

	if(mix_bank==1) buf+=MIX_BANK_SIZE;

	if(playerStatus == PLAYER_STARTED){
		fres=uzefs_read(fh,buf,MIX_BANK_SIZE,&bytesRead);
		if(fres!=UZEFS_OK) sdError(1,fres);
		if(bytesRead < MIX_BANK_SIZE || currentSong.currentSector == currentSong.sectorsCount){
			//clear the ring buffer to avoid clicks
			for(int16_t i=0;i<MIX_BUF_SIZE;i++)	mix_buf[i] = 0x80;
			playerStatus = PLAYER_SONG_ENDED;
		}
		currentSong.position += MIX_BANK_SIZE;
		currentSong.currentSector = currentSong.position / 512;

	}else{
		//if not playing, clear half-buffer
		for(int16_t i=0;i<MIX_BANK_SIZE;i++){
			*buf++ = 0x80;
		}
	}

}

/**
 * Start playing a song.
 *
 * Open the files, loads the song's meta for the scroller then start the player.
 */
void startSong(int32_t firstSector, bool loadInfoBlock){
	//first insure the vsync player is stopped
	playerStatus =  PLAYER_LOADING;
	moviePlaying=false;				//after a movie: the song reader on again

	currentSong.currentSector=firstSector;
	currentSong.position=firstSector * 512;

	//display a loading message for large files (seeking is slow on large files)
	animateTextLine_P(PSTR("... LOADING ... LOADING ... LOADING"),true);

	strcpy(currentSong.filename,fileName(playingFileIndex));
	strcat(currentSong.filename,".WAV");

	//UzeFS has one file slot: free it before opening the next song
	if(fileOpen){
		uzefs_close(fh);
		fileOpen=false;
	}
	fres=uzefs_open(&fh,currentSong.filename,UZEFS_O_READ);
	if(fres!=UZEFS_OK) sdError(2,fres);
	fileOpen=true;

	if(loadInfoBlock){
		uint8_t res=loadWaveInfoBlock(&files[playingFileIndex]);
		if(res==ERR_INVALID_FORMAT){
			animateTextLine_P(PSTR("UNSUPPORTED WAVE FILE FORMAT...MUST BE 15734Hz, 8BITS, MONO. SEE UZEBOX.ORG/WIKI/UZEAMP..."),true);
			return;
		}else if(res==ERR_INVALID_STRUCTURE){
			animateTextLine_P(PSTR("UNSUPPORTED WAVE FILE STRUCTURE..."),true);
			return;
		}
	}

	//Advance read pointer beginning of audio data
	fres=seekSong((firstSector*512)+currentSong.riffDataOffset);
	if(fres!=UZEFS_OK) sdError(3,fres);

	animateTextLine(currentSong.info,true);

	//Activate vsync handler to process audio audio
	playerStatus = PLAYER_STARTED;
}


/**
 * Moves the read position of the open song.
 *
 * UzeFS reaches a position by following the file's cluster chain from its start, a FAT
 * entry a cluster. Its read buffer (UZEFS_RDBUF, the Makefile) serves 32 FAT32 entries
 * a card read.
 */
uzefs_res seekSong(uint32_t pos){
	return uzefs_seek(fh,pos);
}

/**
 * After a movie: its frames used the ring, where the text and the list are. Draws the screen
 * again, reads the list again (the next file is looked up there) and redraws it from the line
 * the cursor's page starts at.
 */
void redrawAll(void){
	ClearVram();
	drawScreen();
	scanFiles();
	listTop=cursorSongIndex-(cursorY-CURSOR_INIT_Y);
	updateFilesList(SCROLL_NONE);
	animateTextLine(currentSong.info,true);
	drawDigits(currentSong.currentSector);		//the movie's position (drawScreen drew 0)
	updateProgressBar(currentSong.currentSector);
}

/**
 * Uzeamp's screen, without the list.
 */
void drawScreen(void){
	//Draw the overall GUI
	DrawMap(0,0,map_main);
	//Draw the vu meter
	DrawMap(19,VU_METER_Y_OFFSET-1,map_overlaVuMeter);
	for(int16_t i=0;i<8;i++)DrawMap2(19+i,VU_METER_Y_OFFSET,map_vuMeterBarOff); //vu meter
	//Draw the wave file parameters
	DrawMap(19,VU_METER_Y_OFFSET+1,map_overlayParams);
	//draw progress bar
	SetTile(2,8,128);SetTile(27,8,138);//beginning+end
	for(uint8_t i=0;i<24;i++)SetTile(i+3,8,129);

	drawDigits(0);
}

/**
 * The list from the card's root: up to MAX_FILES files of the kinds played (typeExt), in the
 * card's order.
 */
void scanFiles(void){
	fileCount=0;
    fres = uzefs_opendir(&dir);

    if (fres == UZEFS_OK) {
        while (1) {
            fres = uzefs_readdir(&dir, &fno);
            if (fres != UZEFS_OK || fno.name[0] == 0) break;
			if (fno.attr & (UZEFS_AM_DIR|UZEFS_AM_HID|UZEFS_AM_SYS)) continue;
			uint8_t type;
			for(type=0;type<TYPES_PLAYED && strstr_P(fno.name,typeExt[type])==NULL;type++){ }
			if(type<TYPES_PLAYED){
				uint8_t i;
				for(i=0;i<8 && fno.name[i]!='.';i++){
					files[fileCount].filename[i]=fno.name[i];
				}
				for(;i<8;i++) files[fileCount].filename[i]=0;
				files[fileCount].seconds=(type==TYPE_UZM) ? movieSeconds(fno.size) : wavSeconds(fno.size);
				files[fileCount].type=type;
				fileCount++;
				if(fileCount==MAX_FILES) break;
			}
        }
    }else{
    	sdError(0,fres);
    }
}

/**
 * The playing time of a movie of size bytes, in seconds: a 512-byte header, then pictures of
 * UZM_SECTORS sectors at 60.05 a second (the console's field rate).
 */
uint16_t movieSeconds(uint32_t size){
	if(size<512) return 0;
	return (uint16_t)(((size-512)/(UZM_SECTORS*512UL))*100UL/6005UL);
}

/**
 * Starts file index of the list: a song, or a movie.
 */
void startFile(uint8_t index){
	if(isMovie(index)){
		startMovie(index);
	}else{
		startSong(0, true);
	}
}

static uint16_t rd16(const uint8_t *p){ return p[0] | (uint16_t)p[1]<<8; }
static uint32_t rd32(const uint8_t *p){ return rd16(p) | (uint32_t)rd16(p+2)<<16; }

/**
 * A movie: opens it, checks its header (tools/uzm.py) and that it is in one piece on the card,
 * sets its title and artist for the scroller, and plays it from the start.
 */
void startMovie(uint8_t index){
	uint16_t bytesRead, pieces;
	uint32_t run;
	uint8_t *h=sector.buffer;
	char *info=currentSong.info;
	uint8_t n;

	playerStatus = PLAYER_STOPPED;
	strcpy(currentSong.filename,fileName(index));
	strcat(currentSong.filename,".UZM");
	if(fileOpen){
		uzefs_close(fh);
		fileOpen=false;
	}
	fres=uzefs_open(&fh,currentSong.filename,UZEFS_O_READ);
	if(fres!=UZEFS_OK) sdError(7,fres);
	fileOpen=true;
	fres=uzefs_read(fh,h,512,&bytesRead);
	if(fres!=UZEFS_OK) sdError(8,fres);
	if(bytesRead<512 || memcmp(h,"UZM1",4) || rd16(h+6)!=UZM_WIDTH || rd16(h+8)!=UZM_ROWS ||
	   rd16(h+10)!=UZM_SECTORS || rd16(h+16)!=UZM_SAMPLES || rd16(h+18)!=1){
		animateTextLine_P(PSTR("NOT A UZM1 MOVIE OF THIS FORMAT. SEE TOOLS/UZM.PY..."),true);
		return;
	}
	fres=uzefs_fragments(fh,&pieces);
	if(fres!=UZEFS_OK) sdError(9,fres);
	if(pieces!=1){
		animateTextLine_P(PSTR("THIS MOVIE IS IN PIECES ON THE CARD: COPY IT AGAIN TO A CARD WITH FREE SPACE..."),true);
		return;
	}

	//the scroller: "TITLE - ARTIST...", 60 characters of each at most (info holds 129)
	for(n=0;n<60 && h[32+n];n++) *info++=h[32+n];
	if(h[96]){
		*info++=' '; *info++='-'; *info++=' ';
		for(n=0;n<60 && h[96+n];n++) *info++=h[96+n];
	}
	*info++='.'; *info++='.'; *info++='.';
	*info=0;
	animateTextLine(currentSong.info,true);

	movie.pictures=rd32(h+12);
	movie.pic=0;
	currentSong.currentSector=0;
	currentSong.sectorsCount=picToSector(movie.pictures);
	fres=uzefs_extent(fh,512,&movie.sect0,&run);	//picture 0: the file's second sector
	if(fres!=UZEFS_OK) sdError(10,fres);
	playMovie();
}

/**
 * A movie's picture on the songs' timeline, their 512-byte sectors at 15,734 bytes a second as
 * drawDigits counts them (1,844 a minute) against 3,603 pictures a minute (60.05 a second).
 * So the clock and the progress bar show a movie's position as a song's.
 */
uint32_t picToSector(uint32_t pic){
	return pic*1844UL/3603UL;
}

#define MOVIE_BUSY 1000		//bytes of the card's busy a still's restart waits out: about 14 lines
#define CUE_STEP   60		//pictures a cue step jumps: a second (ten with A)
#define CUE_FRAMES 6		//frames a cue step is shown: ten seconds of movie a second
#define STEP_DELAY  20		//A/B held: frames before the steps repeat
#define STEP_REPEAT 4		//then a picture every 4 frames: 15 a second

/**
 * In the vertical blank, right after a frame: the read moved to picture p and the ring emptied,
 * so the next frame draws p. Whatever the frame before showed, a still shows the same picture
 * again. False if the card's busy outlasted MOVIE_BUSY: nothing is read then (uzm.prod 0).
 */
static bool moviePicture(uint32_t p){
	uzm.prod=0;
	if(uzefs_stream_restart(movie.sect0+p*UZM_SECTORS,MOVIE_BUSY)!=UZEFS_OK) return false;
	uzm_reset();
	uzm.prod=0xFF;
	return true;
}

/**
 * Plays the movie from picture movie.pic to its end; START holds the picture (a still, the
 * sound muted: the read goes back to it in every vertical blank) and START again goes on from
 * the next one. SL/SR (or the d-pad's left/right) held cue back/forward in the pictures: a
 * second every CUE_FRAMES frames, ten with A, the sound muted; on release the movie goes on
 * from the cued picture (or stays still there). A/B step a picture forward/back, the movie
 * held still (held, they repeat: STEP_DELAY, STEP_REPEAT). SELECT goes back to Uzeamp's
 * screen, the movie paused (START goes on). So does a card that cannot go back in time for a
 * still. Until the user plays a movie anyway, it starts with the card test (cardTest): a card
 * too slow stops it there, and the main loop warns (movieSlow). The card is read with one
 * multiple-block read (uzmmode): no other UzeFS call until it ends. The frames use the ring,
 * where the text and the list are: the main loop draws and reads them again (redrawAll). The
 * picture is black (DDRC 0) while text frames would show the ring's movie bytes: as it fills,
 * during the test's restarts, and after the movie until the redraw.
 */
void playMovie(void){
	uint16_t f, last=0, buttons, prev, step, level, late0=0, watch=0;
	uint32_t pic=movie.pic;					//the next picture to show; held (still, cue), the one shown
	bool ended=false, still=false, cueing=false, jump;
	uint8_t wait=0, repeat=0;

	playerStatus=PLAYER_STOPPED;			//no song is read from here on
	moviePlaying=true;
	DDRC=0x00;								//black: the text frames show the ring, which the movie now fills
	memset(mix_buf,0x80,MIX_BUF_SIZE);
	uzm_reset();
	uzm.under=0;
	uzm.frames=0;
	fres=uzefs_stream_begin(movie.sect0+pic*UZM_SECTORS);
	if(fres!=UZEFS_OK){
		moviePlaying=false;
		sdError(11,fres);
	}
	uzm.prod=0xFF;
	while(uzm_level()<UZM_FILL_MAX-64){		//the ring filled while the text is shown
		WaitVsync(1);
		uzm_fill();
	}
	uzm_video(1);

	if(!cardAccepted){
		//the card test, first as a still: the read restarted at the picture in each blank, the
		//frames black; then the movie from the picture, after one more restart
		cardTest.low=cardTest.still=UZM_RING_SIZE;
		movieSlow=0;
		uzm_mute=1;
		for(uint8_t k=0;k<=TEST_STILLS;k++){
			WaitVsync(1);
			if(!moviePicture(pic)){
				movieSlow=SLOW_STILL;		//the card's busy outlasted MOVIE_BUSY
				break;
			}
			uzm_fill();
			level=uzm_level();
			if(level<cardTest.still) cardTest.still=level;
		}
		uzm_mute=0;
		late0=uzm.under;					//rows late in the black frames: stills that would tear
		cardTest.late=late0;
		if(!movieSlow && cardTest.still<TEST_STILL) movieSlow=SLOW_STILL;	//a restart left it short
		if(!movieSlow && late0) movieSlow=SLOW_PLAY;	//a full start, yet the frames fell behind
		cardReport('S');
		if(movieSlow) goto stop;
		last=uzm.frames;					//the next frame shows pic
		watch=TEST_FRAMES;
	}
	DDRC=0xFF;								//the movie's own colours, whatever the skin

	prev=ReadJoypad(0);
	for(;;){
		WaitVsync(1);						//a frame drawn: the blank is the card's until the next
		f=uzm.frames;						//frames drawn: the last showed picture pic - 1
		if(!still && !cueing) pic+=(uint16_t)(f-last);
		last=f;
		if(watch){							//the test's watch: plain play only
			level=uzm_level();				//unread after the frame, before this blank's fill
			if(level<cardTest.low) cardTest.low=level;
			cardTest.late=uzm.under;
			if(uzm.under!=late0 || (!--watch && cardTest.low<TEST_LEVEL)){
				movieSlow=SLOW_PLAY;
				cardReport('P');
				break;
			}
			if(!watch) cardReport('P');
		}
		if(pic>=movie.pictures){
			ended=true;
			break;
		}
		jump=false;
		buttons=ReadJoypad(0);
		if(buttons & BTN_SELECT) break;		//back to Uzeamp's screen, paused
		if(buttons & (BTN_CUE_BACK|BTN_CUE_FWD)){
			if(!cueing){
				cueing=true;
				wait=0;
				if(!still && pic) pic--;	//from the picture just shown
			}
			if(!wait){
				step=(buttons & BTN_A) ? 10*CUE_STEP : CUE_STEP;
				if(buttons & BTN_CUE_FWD){
					pic=(pic+step<movie.pictures) ? pic+step : movie.pictures-1;
				}else{
					pic=(pic>step) ? pic-step : 0;
				}
				wait=CUE_FRAMES;
			}
			wait--;
		}else if(cueing){
			cueing=false;					//on from the cued picture, or still on it
			jump=true;
		}else if(buttons & (BTN_A|BTN_B)){	//a picture forward/back, held still
			if(!(prev & (BTN_A|BTN_B))){
				repeat=1;					//the press steps now, then after STEP_DELAY
			}
			if(!--repeat){
				repeat=(prev & (BTN_A|BTN_B)) ? STEP_REPEAT : STEP_DELAY;
				if(!still){
					still=true;
					if(pic) pic--;			//from the picture just shown
				}
				if(buttons & BTN_A){
					if(pic+1<movie.pictures) pic++;
				}else if(pic){
					pic--;
				}
			}
		}else if(buttons & ~prev & BTN_START){
			still=!still;
			if(still){
				if(pic) pic--;				//the picture just shown
			}else if(++pic>=movie.pictures){
				ended=true;
				break;
			}else{
				jump=true;					//on from the next
			}
		}
		prev=buttons;
		if(still || cueing || jump) watch=0;	//the user's doing: no verdict from this movie
		uzm_mute=still || cueing;			//before this blank's vsync: the last picture's sound
											//is played on going on, in its place
		if((still || cueing || jump) && !moviePicture(pic)) break;
		uzm_fill();
	}

stop:
	uzm.prod=0;								//no frame reads the card after this
	uzm_mute=0;
	DDRC=0x00;								//black until the main loop has drawn the text again
	uzm_video(0);
	uzefs_stream_end();
	memset(mix_buf,0x80,MIX_BUF_SIZE);
	if(movieSlow){
		pic=movie.pic;						//"play anyway" starts where the test did
		ended=false;
	}
	movie.pic=pic;
	currentSong.currentSector=picToSector(pic);
	redrawPending=true;						//the main loop redraws: here, the stack is deep
	moviePlaying=false;
	while(ReadJoypad(0) & (BTN_START|BTN_SELECT)) WaitVsync(1);	//the main loop must not take this press again
	playerStatus=ended ? PLAYER_SONG_ENDED : PLAYER_PAUSED;
}

/**
 * The name of file index in the list, NUL-terminated (the list keeps 8 bytes, no NUL
 * when the name is 8 long). Valid until the next call.
 */
char *fileName(uint8_t index){
	static char name[9];
	memcpy(name,files[index].filename,8);
	name[8]=0;
	return name;
}

/**
 * Search wave file meta data for the artist name and display information.
 * These are stored under the IART and DISP chunks.
 * see: https://www.robotplanet.dk/audio/wav_meta_data/
 * 		http://soundfile.sapp.org/doc/WaveFormat/
 *
 * Limitations:
 *- required meta data must fit with a single sector (512 bytes).
 *- metadata must be either right after data chunk (usually put there by most tools)
 *  or right after fmt chunk (like ffmpeg)
 *- keep metadata to just title and artist to avoid issues (other be ignored anyway)
 */
uint8_t debug=0;
uint8_t loadWaveInfoBlock(SdFile *file){
	uint32_t infoBlockStartAddr;
	uint8_t i,pos=0,c;
	uint16_t bytesRead=0;
	char infoTemp[64];

	//Read the first sector to get the Riff header
	fres=uzefs_read(fh,sector.buffer, 512,&bytesRead);
	if(fres!=UZEFS_OK) sdError(4,fres);

	//check if its a valid WAV file
	if(	sector.riffHeader.format[0]=='W' &&
		sector.riffHeader.format[1]=='A' &&
		sector.riffHeader.format[2]=='V' &&
		sector.riffHeader.format[3]=='E'){

		//next chunk is always "fmt ", check if wave is correct format
		//Support 15750hz used by old .wav files as well as the correct 15734hz
		if(sector.riffHeader.audioFormat!=WAVE_PCM || sector.riffHeader.numChannels!=1
				|| !(sector.riffHeader.sampleRate ==15734 || sector.riffHeader.sampleRate==15750)
				|| sector.riffHeader.bitsPerSample!=8){
			return ERR_INVALID_FORMAT;
		}

		//check if the next chunk is sound data ou metadata
		if(	sector.riffHeader.subchunk2ID[0]=='d' &&
			sector.riffHeader.subchunk2ID[1]=='a' &&
			sector.riffHeader.subchunk2ID[2]=='t' &&
			sector.riffHeader.subchunk2ID[3]=='a'){

			//compute the address of the meta data that resides after the sound data
			//Starts with the chunk ID "INFO"
			infoBlockStartAddr=sector.riffHeader.subchunk2Size + sizeof(sector.riffHeader) + 1;
			currentSong.sectorsCount=(sector.riffHeader.subchunk2Size/512);
			currentSong.riffDataOffset=(sizeof(sector.riffHeader) + 1);

			//return if no INFO chunk after audio data
			if(infoBlockStartAddr>=sector.riffHeader.chunkSize){
				strcpy_P(currentSong.info,PSTR("UNTITLED"));
				return OK;
			}

			//advance read pointer to meta data start
			fres=seekSong(infoBlockStartAddr);
			if(fres!=UZEFS_OK) sdError(5,fres);

			//read the meta data
			fres=uzefs_read(fh,sector.buffer, 512,&bytesRead);
			if(fres!=UZEFS_OK) sdError(6,fres);

		}else if(sector.riffHeader.subchunk2ID[0]=='L' &&
				 sector.riffHeader.subchunk2ID[1]=='I' &&
				 sector.riffHeader.subchunk2ID[2]=='S' &&
				 sector.riffHeader.subchunk2ID[3]=='T'){

			currentSong.sectorsCount=(sector.riffHeader.chunkSize-sector.riffHeader.subchunk1Size-sector.riffHeader.subchunk2Size)/512;
				currentSong.riffDataOffset=36+18+sector.riffHeader.subchunk2Size;
		}else{
			//unrecognized structure
			return ERR_INVALID_STRUCTURE;
		}

		//file->currentSector=0;
		currentSong.currentSector=0;

		if(findTag("INFO",NULL,0,0)==0){

			//printf("here");while(1);


			//find the artist name metadata
			if(findTag("IART",infoTemp,sizeof(infoTemp),4)==OK){
				//append to the concatenated artist name
				pos=0;
				if(infoTemp[0]!=0){
					i=0;
					while(i<sizeof(infoTemp)){
						c=infoTemp[i++];
						if(c==0) break;
						currentSong.info[pos++]=c;
					}
					if(pos!=0){
						currentSong.info[pos++]=' ';
						currentSong.info[pos++]='-';
						currentSong.info[pos++]=' ';
					}
				}
			}

			//search for the song name
			if(findTag("INAM",infoTemp,sizeof(infoTemp),4)==OK){
				if(infoTemp[0]!=0){
					i=0;
					while(i<sizeof(infoTemp)){
						c=infoTemp[i++];
						if(c==0) break;
						currentSong.info[pos++]=c;
					}
				}
			}

			//return if no INFO chunk after audio data
			if(pos==0){
				strcpy_P(currentSong.info,PSTR("UNTITLED"));
			}else{
				currentSong.info[pos]=0;
			}
			return OK;
		}else{
			currentSong.info[0]=0;
			return ERR_INVALID_STRUCTURE;
		}
	}

	return ERR_INVALID_FORMAT;
}

/**
 * Scan the SD buffer for the specified chunk name (tag) and return the value
 */
int16_t findTag(char *tag, char *dest,uint16_t destLenght,uint8_t dataOffset){
	uint32_t i=0,size;
	int16_t j;

	while(1){
		if( getChar(i)==tag[0] && getChar(i+1)==tag[1] && getChar(i+2)==tag[2] && getChar(i+3)==tag[3]){
			if(dest!=NULL){
				i+=4;
				//get info chunk size
				size=getChar(i)+(getChar(i+1)<<8);//+(getChar(i+2)<<8)+(getChar(i+3)<<8);
				if(size>=(destLenght-1))size=(destLenght-1);
				i+=dataOffset;

				for(j=0;j<size;j++){
					dest[j]=getChar(i+j);
				}
				dest[j]=0;
			}
			return OK; //found!
		}

		i++;
		if(i>=512){
			dest[0]=0;
			return ERR_NOT_FOUND; //not found!
		}
	}

}

uint8_t getChar(int16_t index){
	uint8_t c=sector.buffer[index];
	if(c>=0x7d) c='?'; //clip unsupported characters above ascii 127
	if(c>=97) c-=32; //convert lowercase to uppercase
	return c;
}

/**
 * Draw the song's timestamp digits
 */
void drawDigits(uint32_t currentSectorNo ){
	uint32_t hours,minutes,seconds,temp;
	uint8_t digit1, digit2,x=3;
	//15734 bytes/sec @ 512bytes/sector
	//31 sectors/sec
	//1844 sectors/min
	//110630 sectors/hour

	hours=currentSectorNo/110630;
	temp=(currentSectorNo-(hours*110630));
	minutes=temp/1844;
	seconds=(temp-(minutes*1844))/31;

	//print hours
	digit1=(hours/10)%10;
	digit2=hours%10;
	DrawMap2(x+0,4,map_digit0+(digit1*8));
	DrawMap2(x+2,4,map_digit0+(digit2*8));
	DrawMap2(x+4,4,map_digitSep);
	digit1=(minutes/10)%10;
	digit2=minutes%10;
	DrawMap2(x+5,4,map_digit0+(digit1*8));
	DrawMap2(x+7,4,map_digit0+(digit2*8));
	DrawMap2(x+9,4,map_digitSep);
	digit1=(seconds/10)%10;
	digit2=seconds%10;
	DrawMap2(x+10,4,map_digit0+(digit1*8));
	DrawMap2(x+12,4,map_digit0+(digit2*8));

}

/**
 * The playing time of a WAV file of size bytes, in seconds, computed as the list always did.
 */
uint16_t wavSeconds(uint32_t size){
	uint32_t hours,minutes,temp;

	//convert to sectors
	size /= 512;

	//15734 bytes/sec @ 512bytes/sector
	//31 sectors/sec
	//1844 sectors/min
	//110630 sectors/hour

	hours=size/110630;
	temp=(size-(hours*110630));
	minutes=temp/1844;
	return hours*3600 + minutes*60 + (temp-(minutes*1844))/31;
}

void getFilePlayingTime(uint16_t totalSeconds,char *buffer){
	uint16_t hours,minutes,seconds;
	uint8_t digit1, digit2;

	hours=totalSeconds/3600;
	minutes=(totalSeconds%3600)/60;
	seconds=totalSeconds%60;

	//print hours
	digit1=(hours/10)%10;
	digit2=hours%10;
	if(hours>0 && digit1>0) buffer[0]=digit1+'0';
	if(hours>0) buffer[1]=digit2+'0';
	if(hours>0) buffer[2]=':';

	digit1=(minutes/10)%10;
	digit2=minutes%10;
	if(hours>0 || digit1>0)buffer[3]=digit1+'0';
	if(hours>0 || minutes>0)buffer[4]=digit2+'0';
	buffer[5]=':';

	digit1=(seconds/10)%10;
	digit2=seconds%10;
	buffer[6]=digit1+'0';
	buffer[7]=digit2+'0';

	buffer[8]=0;
}

/**
 * Scrolls the song's information
 */
void animateTextLine(char *infoSongStr, bool reset){
	static char *infoSongStrCopy;
	static uint8_t pos=0,wait=0;
	uint8_t curPos=pos,c;

	if(infoSongStr==NULL){
		//animate previous setted string
		infoSongStr=infoSongStrCopy;
	}else{
		infoSongStrCopy=infoSongStr;
	}

	size_t songLen=strlen(infoSongStr);

	if(reset){
		pos=0;
		curPos=0;
		if(songLen<=24){
			for(char i=0;i<24;i++)PrintChar2(3+i,7,' ',SFONT);
			PrintString2(3,7,infoSongStr,SFONT);
		}
		wait=20;
	}

	if(wait>=20){
		if(songLen>24){
			for(int16_t i=0;i<24;i++){
				if(curPos>=songLen){
					c=32;
				}else{
					c=infoSongStr[curPos];
				}
				PrintChar2(i+3,7,c,SFONT);
				curPos++;
				if(curPos>=(songLen+4)) curPos=0;
			}

			wait=0;
			pos++;
			if(pos>=(songLen+4)) pos=0;
		}
	}

	wait++;

}
/**
 * Scrolls information in flash
 */
void animateTextLine_P(const char *infoSongStr, bool reset){
	static const char *infoSongStrCopy;
	static uint8_t pos=0,wait=0;
	uint8_t curPos=pos,c;

	if(infoSongStr==NULL){
		//animate previous setted string
		infoSongStr=infoSongStrCopy;
	}else{
		infoSongStrCopy=infoSongStr;
	}

	size_t songLen=strlen_P(infoSongStr);

	if(reset){
		pos=0;
		curPos=0;
		if(songLen<=24){
			for(char i=0;i<24;i++)PrintChar2(3+i,7,' ',SFONT);
			PrintString2_P(3,7,infoSongStr,SFONT);
		}
		wait=20;
	}

	if(wait>=20){
		if(songLen>24){
			for(int16_t i=0;i<24;i++){
				if(curPos>=songLen){
					c=32;
				}else{
					c=pgm_read_byte(&(infoSongStr[curPos]));
				}
				PrintChar2(i+3,7,c,SFONT);
				curPos++;
				if(curPos>=(songLen+4)) curPos=0;
			}

			wait=0;
			pos++;
			if(pos>=(songLen+4)) pos=0;
		}
	}

	wait++;

}
/**
 * Custom function that prints a string in RAM and supports 2 fonts.
 * Each must be 32 chars long and located at the beginning of the tile set.
 * See tiles.png
 */
void PrintString2(uint8_t x, uint8_t y, char *str, uint8_t font){
	char c;
	while((c=*str++)){
		if(c==126)c=92; //fix tilde character for long filename because it is out of the font
		PrintChar2(x++,y,c,font);
	}
}

/**
 * Custom function that prints a single character and supports 2 fonts.
 * See PrintString2 for restrictions.
 */
void PrintChar2(uint8_t x,uint8_t y,char c,uint8_t font){
	if(c>='a' && c<='z')c&=0b11011111;	//convert to uppercase
	if(font) c+=64; //address second font
	SetTile(x,y,c-32);
}

void PrintByte2(uint8_t x,uint8_t y,uint8_t val,bool zeropad,uint8_t maxDigits, uint8_t font){
	uint8_t c,i;

	for(i=0;i<maxDigits;i++){
		c=val%10;
		if(val>0 || i==0){
			SetTile(x--,y,c+'0'-32+(font?64:0));
		}else{
			if(zeropad){
				SetTile(x--,y,c+'0'-32+(font?64:0));
			}else{
				SetFont(x--,y,' '-32+(font?64:0));
			}
		}
		val=val/10;
	}
}

/**
 * Custom function that prints a string in FLASH and supports 2 fonts.
 * See PrintString2 for restrictions.
 */
void PrintString2_P(uint8_t x, uint8_t y, const char *str, uint8_t font){
	int16_t i=0;
	char c;
	while(1){
		c=pgm_read_byte(&(str[i++]));
		if(c==0)break;
		PrintChar2(x++,y,c,font);
	}
}

/**
 * Fills the specified string with the specified character upton maxLen.
 */
void setString(char *str,char c,uint8_t maxLen){
	for(uint8_t i=0;i<maxLen;i++)str[i]=c;
}


/**
 * Initialize the SD card, polling up to 5 seconds
 * and displays an error message if unsuccessful
 */
/**
 * Mounts the card. Until one is mounted it says why, in the list's middle, and tries again
 * twice a second, so a card can go in at any time (the slot has no switch to ask: a failed
 * mount is how a missing card shows). The first two tries are silent: some cards need a few
 * to start after power-up.
 */
/**
 * A number, right-aligned to end at x; "-" for UZM_RING_SIZE (a figure not measured).
 */
static void printNum(uint8_t x,uint8_t y,uint16_t v){
	if(v>=UZM_RING_SIZE){
		PrintChar2(x,y,'-',BFONT);
		return;
	}
	do{
		PrintChar2(x--,y,'0'+v%10,BFONT);
		v/=10;
	}while(v);
}

/**
 * The card test's warning, in the list's place (the main loop takes its buttons).
 */
void drawSlowWarning(void){
	Fill(3,11,24,11,BLANK);
	PrintString2_P(3,12,PSTR("CARD TOO SLOW FOR MOVIES"),BFONT);
	if(movieSlow==SLOW_PLAY){
		PrintString2_P(6,14,PSTR("LATE WHILE PLAYING"),BFONT);
	}else{
		PrintString2_P(3,14,PSTR("TOO SLOW TO PAUSE OR CUE"),BFONT);
	}
	PrintString2_P(5,16,PSTR("LOW"),BFONT);					//the figures: cardTest
	printNum(12,16,cardTest.low);
	PrintString2_P(14,16,PSTR("STILL"),BFONT);
	printNum(23,16,cardTest.still);
	PrintString2_P(8,17,PSTR("ROWS LATE"),BFONT);
	printNum(22,17,cardTest.late);
	PrintString2_P(8,19,PSTR("A: PLAY ANYWAY"),BFONT);
	PrintString2_P(5,20,PSTR("B: BACK TO THE LIST"),BFONT);
	warnPrev=ReadJoypad(0);				//a button already held is no answer
}

void initSdCard(){
	for(uint8_t tries=0;;tries++){
		fres=uzefs_mount();
		if(fres==UZEFS_OK){
			Fill(3,11,24,11,BLANK);
			return;
		}
		if(tries>=2){
			Fill(3,15,24,1,BLANK);
			if(fres==UZEFS_ERR_NOT_READY || fres==UZEFS_ERR_DISK){
				PrintString2_P(5,15,PSTR("NO SD CARD INSERTED"),BFONT);
			}else if(fres==UZEFS_ERR_NO_FS){
				PrintString2_P(5,15,PSTR("CARD NOT FAT16/FAT32"),BFONT);
			}else{
				PrintString2_P(7,15,PSTR("SD CARD ERROR"),BFONT);
				PrintByte2(22,15,fres,false,2,BFONT);
			}
			tries=2;
		}
		WaitVsync(30);
	}
}

/**
 * Stop the program and display an SD card error message.
 * locationCode is a code that indicate where the error occured in order
 * to help debugging.
 */
void sdError(uint8_t locationCode,uint8_t fsCode){
	playerStatus = PLAYER_STOPPED;

	Fill(2,11,26,11,BLANK);
	initSdCard();
	wdt_enable(WDTO_15MS);		//Enable the watchdog timer to reset the console
	while(1);
}
