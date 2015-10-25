/*
 *  Uzebox(tm) TORNADO 2000
 *  Copyright (C) 2015  Alec Bourque
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

/*  This program is a bit of a mash-up as it is both a re-make of the
 *  1980's Atari arcade game Tempest by Dave Theurer and a de-make of
 *  the 64 bit Atari Jaguar game Tempest2000 by Jeff Minter.
 *
 *  Most of my play testing was done with the excellent Tempest2000
 *  re-make Typhoon 2001 by Thorsten Kuphaldt
 *
 *  Warning - Some of the code in this thing is a bit unorthodox due
 *  to trading speed for readability.
 *
 */


#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <kernel.h>
#include <sdSimple.h>

#include "data/mypatches.inc"
#include "data/mindseye.inc"
#include "data/intro.inc"

#include "t2k.h"

#include "fonts.h"
#include "objects.h"
#include "webdata.h"
#include "housekeeping.h"
#include "levels.h"
#include "highscores.h"


// C/ASM shared globals for screen/objects

unsigned char ramTiles[256*8]  __attribute__ ((section (".ramtiles")));
unsigned char vram[32*28] __attribute__ ((section (".vram")));

// Globals for game

uint8_t  level;
uint8_t  lane;
uint8_t  zoom;
uint8_t  jWebMin;
uint8_t  jWebMax;
uint8_t  jumpCount;

uint8_t  lives;
uint8_t  score[4];
uint8_t  hiScore[4] = {0};
uint8_t  bonusLifeScore;
uint8_t  dominationBonus;
uint8_t  messageCounter = 0;
uint8_t  activeEnemy;					// Number of enemies currently on the screen
uint8_t  lvlGenCount;					// Number of enemies the generator will make for this level
uint8_t  lvlSpawnCount;					// The (random) time till the next enemy is generated


long sectorStart;
volatile long sectorBMP;

int8_t   BulletCountDown;
uint8_t  buttDuration;
uint16_t buttonsLast;
uint8_t actionTimerSubCount;

#define clawSpeed BulletCountDown




extern const u8 sfx_params[];
//channel types
#define SFX_TYPE_NOT_USED	0
#define SFX_WAVE_PRIMARY	1
#define SFX_WAVE_SECONDARY	2
#define SFX_NOISE_TYPE	3
#define SFX_PCM_TYPE		4

uint8_t sfx_current_priority[3];

void T2k_TriggerFx(u8 effect, u8 volume, u8 note){
   //each effect can have up to 2 different patches that combine
   //there is a priority for each sound to control what sound override others
   u8 p,c,r,m,l;

   effect *= 5;

T2K_TRIGGER_FX_TOP:
   p = pgm_read_byte(&sfx_params[effect++]);//which patch to actually trigger
   c = pgm_read_byte(&sfx_params[effect++]);//which channel this should play on
   r = pgm_read_byte(&sfx_params[effect++]);//priority of this sound
   m = pgm_read_byte(&sfx_params[effect++]);//what note to play for PCM, spikes are handled manually, and normal effects ignore this
   l = pgm_read_byte(&sfx_params[effect++]);//does this effect have 2 parts played on separate channels?

   //handle 1 part of a possible 2 part effect
   //determine which channel to use based on type
   if(c == SFX_WAVE_PRIMARY)//this collides with music, but for zoom out and really big sound effects thats ok(player death, etc)
      c = 2;
   else if(c == SFX_WAVE_SECONDARY || c == SFX_PCM_TYPE)//common ones that play rapidly
      c = 4;
   else//noise
      c = 3;

   if(!(tracks[c].flags & TRACK_FLAGS_PLAYING) || r >= sfx_current_priority[c-2]){//ok to play it
      sfx_current_priority[c-2] = r;//set the new priority of this sound/channel
         tracks[c].flags |= TRACK_FLAGS_PRIORITY;
      TriggerCommon(c,p,volume?volume:128,note?note:m);
   }
   if(l)//there is a second part that is on a different channel, process that too
      goto T2K_TRIGGER_FX_TOP;//yep...saves stack, cycles, and code space
}



extern uint16_t debug_token_count;
extern uint16_t debug_r1b_count;



const char  FileName[] PROGMEM = "UTEMPESTLVL";

int main(){

srand(123);

SDCardVideoModeDisable();

message(MSG_SEARCHING_SD, TEXT_MODE_XY(0,12));

sdCardInitNoBuffer();

//sectorStart = findFileFirstSector("UTEMPESTLVL");
//sectorStart = mmcFindFileFirstSector("UTEMPESTLVL");

sectorStart = sdFindFileFirstSectorFlash(FileName);

while(sectorStart == 0);

SDCardVideoModeEnable();

InitMusicPlayer(patches);

lives = 0;
StartSong(intro,0,true);
while(1) {
	if (lives==0) showTitle();
	if (lives==0) showCredits();
	if (lives==0) showHighScore();
	if (lives==0) attractMode();
	if (lives!=0) {
		StartSong(mindseye,0,true);
		playGame();
		gameOver();
		StartSong(intro,0,true);
		checkHighScores();
		showHighScore();
	}
}

/*  todo: new stuff
 *
 *  DRAW ->   UFO
 *  AI for droid - ignore 4 closest lanes to claw.  Ignore 4 most recent lanes fired down. Move towards lane with highest Z enemy excluding SPIKES*
 *  maybe swap LaneHi and LaneLo to save SWAPs and make code more efficient.
 *      Have to check usage of Lane Vs laneHi/laneLo before this is done
 *  INVESTIGATE using a look up table for combined OR/AND-PIXEL.
 *  INVESTIGATE run length lines
 *
 *
 */
/*  DONE
 *
 *  QT My own line draw routines
 *  Move Cue Sector to first 16 lines
 *  Make highscore/lives in first 16 lines
 *
 *  Make claw/flipper/zapper/laser/tanker/spike vectors
 *  Make/Copy bitmap fonts
 *  Fast LineDraw/Setpixel rotuines
 *  MMC and FAT stuff
 *
 *  ClearLine in front/back porch
 *  Routines for converting live/score into tiles for first 16 lines
 *
 *  Object Structures
 *  Object iterration for game logic and X/Y/Zoom stuff
 *  Object iterration for drawing
 *  Object drawing in ASM stuff.
 *      All rotations can be done as consecutive SUBI
 *  BREAK UP bresh line entry/exit to save PUSH POP and make that on obj loop start
 *      ALSO can move 0x04 and 0x20 into low regs and use some extra low regs on the ENTRY to ASM
 *
 *  Qt and AVR integration
 *      Work out how to zoom/pan in Qt
 *      Work out how to write out structures for AVR code to follow Qt lines
 *      Work out how to 8:8 fixed point make aliens come up the tube
 *  QT Break up FOpen/Write/Fclose into methods
 *  QT Make a movie
 *  QT + Uze - work out if 3D maths doable
 *  make MULSU() an inline ASM thing  (This will save a lot of call/ret stuff)
 */

}

void attractModeAI(void){
	uint8_t highestLane;
	ObjectDescStruct *objData;

	objData = &CLAW_OBJECT;

	highestLane = getHighestLane();
	laneData[objData->laneHi].highestEnemy = 0;

	if(objData->actionTimer7 == 0) {
		objData->actionTimer7 = CLAW_DEMO_FIRE_FREQ;
		fire(objData);
	}
	objData->actionTimer7--;


	if(closestLaneDirection(highestLane, objData->lane)) {
		clawSpeed--;
		if (clawSpeed < -CLAW_DEMO_MOVE_SPEED) {
			clawSpeed = -CLAW_DEMO_MOVE_SPEED;
		}
	} else {
		clawSpeed++;
		if (clawSpeed > CLAW_DEMO_MOVE_SPEED) {
			clawSpeed = CLAW_DEMO_MOVE_SPEED;
		}
	}

	if(clawSpeed > 0) {
		objData->lane += clawSpeed;
		if ((LANE_CLOSED_FLAG) && ((objData->lane & 0xF0) == 0xF0)){
			objData->lane -= clawSpeed;
		}
	} else {
		objData->lane += clawSpeed;
		if ((LANE_CLOSED_FLAG) && ((objData->lane & 0xF0) == 0xF0)){
			objData->lane += clawSpeed;
		}
	}
	lane = CLAW_OBJECT.lane;
}
void attractMode(void){
	SetHsyncCallback(&DefaultCallback);
	level = rand()%15;
	bonusLifeScore = 0x05;
	startLevel();
	zoom = ZOOM_LEVEL_NORMAL;
	clearZoomingFlag();
	setLaserPowerupFlag();
	clawSpeed = 0;

	uint16_t runCount = LENGTH_OF_ATTRACT_MODE;

	lives = 1;
	while(lives == 1) {

		runCount--;
		if (runCount == 0) 			lives = 0;
		if (ALIVE_STATE_A_FLAG)  	lives = 0;
		if (checkStartPressed())	lives = 3;

		waitNFrames(1);
		Generator();

		if ((activeEnemy == 0) && (lvlGenCount == 0)) {
			setZoomingFlag();
		}

		if (!ALIVE_STATE_B_FLAG) attractModeAI();
		getWebAngles(lane, webNum);
		moveObjects();
		collisionDetection();

		setVramClearFlag();
		waitNFrames(1);
		ClearBufferFinal();
		sectorBMP = getSectorBMP();
		renderObjects();
	}
}
void playGame(void){

	buttonsLast = 0xFFFF;

	SetHsyncCallback(&DefaultCallback);
	score[0] = 0;
	score[1] = 0;
	score[2] = 0;
	score[3] = 0;
	level    = 0;
	bonusLifeScore = 0x05;
	startLevel();

	while(lives > 0) {
		if (ALIVE_STATE_A_FLAG) {
			lives--;
			if (lives >0) {
				startLevel();
			}
		}

		waitNFrames(1);

		if (messageCounter <= MESSAGE_COUNTER_MAX) {
			messageCounter--;
			if (messageCounter == 0) {
				message(MSG_CLEAR, MESSAGE_DEFAULT_POSITION);
				messageCounter = MESSAGE_COUNTER_MAX + 1;
			}
		}
		Generator();

		if ((activeEnemy == 0) && (lvlGenCount == 0)) {
			setZoomingFlag();
			if(zoom == 32) T2k_TriggerFx(SFX_WEB_ZOOM,0,0);
		}
		if (!ALIVE_STATE_B_FLAG) ProcessInput();
		getWebAngles(lane, webNum);
		//clearTestMarkers();
		moveObjects();
		if(!(SUPERZAP_ACTIVE_FLAG)) {
			collisionDetection();
		}

		setVramClearFlag();
		waitNFrames(1);
		ClearBufferFinal();
		sectorBMP = getSectorBMP();
		renderObjects();
		setStatusUpdateFlag();
	}
}
void ProcessInput(void) {

uint16_t buttons;

buttons = ReadJoypad(0);

if(buttons & BTN_LEFT) {
	if (buttDuration <= 2) {
		buttDuration += 1;
		CLAW_OBJECT.lane += 0x03;
	} else {
		CLAW_OBJECT.lane += 0x07;
	}
	if ((LANE_CLOSED_FLAG) && (CLAW_OBJECT.lane > ((14<<4) + 4))){
		CLAW_OBJECT.lane = ((14<<4) + 4);
	}
	lane = CLAW_OBJECT.lane;
} else if(buttons & BTN_RIGHT)  {
	if (buttDuration <= 2) {
		buttDuration += 1;
		CLAW_OBJECT.lane -= 0x03;
	} else {
		CLAW_OBJECT.lane -= 0x07;
	}
	if ((LANE_CLOSED_FLAG) && (CLAW_OBJECT.lane < 10)){
		CLAW_OBJECT.lane = 10;
	}
	lane = CLAW_OBJECT.lane;
} else {
	buttDuration = 0;
}

if((buttons & BTN_B)   && (!(buttonsLast &BTN_UP)) && (SUPERZAP_FLAG)) {
	newSuperZap();
	clearSuperZapFlag();
	setSuperZapActiveFlag();
}
if((buttons & BTN_DOWN) && (jumpCount == NOT_JUMPING) && (!ZOOMING_FLAG) && (JUMP_FLAG)) {
	jumpCount = 0;
	T2k_TriggerFx(SFX_CLAW_JUMP,0,0);
}
if((buttons & BTN_A)    && (BulletCountDown < 14)) {
	BulletCountDown += 32;
	fire(&CLAW_OBJECT);
}
if((uint8_t)BulletCountDown > 11) {
	BulletCountDown -= 11;
} else {
	BulletCountDown = 0;
}
if((buttons & BTN_START) && (!(buttonsLast & BTN_START))) {
	message(MSG_PAUSED, MESSAGE_DEFAULT_POSITION);
	StopSong();
	while(!checkStartPressed());
	while(checkStartPressed());
	while(!checkStartPressed());
	message(MSG_CLEAR, MESSAGE_DEFAULT_POSITION);
	ResumeSong();
}

buttonsLast = buttons;
}
