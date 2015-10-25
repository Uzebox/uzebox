/*
 * levels.c
 *
 *  Created on: 20/05/2015
 *      Author: Cunning Fellow
 */

#include <avr/io.h>
#include <stdlib.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <kernel.h>

#include "levels.h"
#include "objects.h"
#include "housekeeping.h"
#include "webdata.h"
#include "t2k.h"


#define lvlTopEnemy (((level >> 1) < TOP_ENEMY) ? (level >> 1) :  TOP_ENEMY)	// The highest level of enemy that will be generated on this level
																				//  increases by 1 every two levels up to a maximum
                                                                                //  Level 1 & 2 Flipper
																				//  Level 3 & 4 Flipper + Tanker
																				//  Level 5 & 6 Flipper + Tanker + Fuseball
																				//  etc. etc.

#define lvlSpawnTime ((level < 20) ? (40 - level) :  40)						// The maximum time between enemies being generated
                                                                                //  decreases by one tick per level until level 30


void Generator(void){
	uint8_t enemy;
	uint8_t newObjNum;

	if (!ZOOMING_FLAG) {										// If you are ZOOM-ing then don't decrement the timer
		lvlSpawnCount--;										//    that will create a new enemy
	}

	if ((lvlSpawnCount == 0) && (lvlGenCount > 0)) {			// If it is time to generate a new enemy AND there are still new enemies available

		lvlGenCount--;											// Decrement the number of enemies available/left

		lvlSpawnCount = (rand()%lvlSpawnTime) + MIN_SPAWN;		// Get the random time before spawning the NEXT enemy after this one

		enemy = rand()%(lvlTopEnemy+1);							// Get a random TYPE of enemy to spawn
		enemy = enemy + OBJ_NONFLIPPER;

		if(LANE_CLOSED_FLAG) {																// TRY to Create the new enemy on a random lane
			newObjNum = newObject(enemy, rand()%15, (jWebMin + Z_WEB_SPAWN_OFFSET));		// If the WEB is a open WEB then lane can only be up to 14
		} else {
			newObjNum = newObject(enemy, rand()%16, (jWebMin + Z_WEB_SPAWN_OFFSET));		// If the WEB is closed then random lane can be upto 15
		}

		if(newObjNum != (MAX_OBJS+1)) {							// If creating the object succeeded
			activeEnemy++;										// Then increment the counter of enemies on screen
		}
	}
}


void startLevel(void){

	uint8_t i;

	SetHsyncCallback(&DefaultCallback);
	ClearBuffer();
	clearLaserPowerupFlag();
	clearAIDroidFlag();
	clearAliveStateAFlag();
	clearAliveStateBFlag();
	clearJumpFlag();
	setSuperZapFlag();
	setCollectorBonusFlag();
	dominationBonus = 0;

	zoom = ZOOM_LEVEL_MAX;									// When a level starts the Zoom level is set at maximum Z
	setZoomingFlag();										// and the zooming flag is enabled
															// Then normal game zooming logic will stop the zoom and start the
															// gameplay when the zoom level hits ZOOM_LEVEL_NORMAL

	lvlGenCount = 20 + (level << 2);						// Number of enemies to generate per level

	if (lvlGenCount > 45) {									//    Level 1 (0) = 10
		lvlGenCount = 45;									//    Level 2 (1) = 12
	}														//    Level 3 (2) = 14
															//    ...
															//    to a maximum of 45

	lvlSpawnCount = 1;										// 1 = next tick an enemy will be generated
	activeEnemy = 0;
	powerUp.Count = 0;										// 0 = first explosion on the level will release a powerup
															// Note: powerUp.Num does not get reset at the start of a level

	clearObjectStore();
	clearSuperZapActiveFlag();
	newObject(OBJ_CLAW, (8<<4), CLAW_FIXED_Z); 				// CLAW IS ALWAYS OBJECT ZERO and starts on LANE 8

	lane = 8;											// LANE = 8 << 4 (lane number is held in high nibble)

	jumpCount = NOT_JUMPING;								// When the zoom has finished the claw will be not jumping
	jWebMin = Z_WEB_ABS_MIN;								//     AND the zoom level will be at the normal MIN/MAX
	jWebMax = Z_WEB_ABS_MAX;

	for (i=0;i<16;i++){										// for each of the 16 lanes
			laneData[i].spikeObj = (MAX_OBJS + 1);			// clear the variable that points the the spike on the lane
		}

	if(LANE_CLOSED_NO == pgm_read_byte(&laneClosed[webNum])){
		setLaneClosedFlag();
	} else {
		clearLaneClosedFlag();
	}
}
