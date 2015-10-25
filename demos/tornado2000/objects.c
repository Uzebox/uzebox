/*
 * objects.c
 *
 *  Created on: 21/09/2014
 *      Author: Cunning Fellow
 */


#include <avr/io.h>
#include <stdlib.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <kernel.h>

#include "objects.h"
#include "webdata.h"
#include "housekeeping.h"
#include "fonts.h"
#include "levels.h"
#include "highscores.h"
#include "t2k.h"

ObjectDescStruct ObjectStore[MAX_OBJS] __attribute__ ((section (".objectstore")));

powerUpStruct powerUp;

const uint16_t enemyPoints[] PROGMEM = {
 0, 				// empty
 0,					// CLAW
 0,					// bullet
 0,					// laser
 0,					// bullet exploded
 0,					// laser exploded
 0, 				// SUPER ZAP
 PNT_REFBULLET,		// Reflected Bullet
 PNT_REFLASER,		// Reflected Laser
 PNT_ZAP,			// ZAP (Enemy bullet)
 PNT_FLIPPER,		// non-flipper
 PNT_TANKER,		// Tanker
 PNT_SPIKER,		// Spiker
 PNT_FUSEBALL,		// Fuseball
 PNT_PULSAR,		// Pulsar
 PNT_MIRROR,		// Mirror
 PNT_MUTFLIPPER,	// Mutant Flipper
 PNT_DEMONA,		// Demon Head A
 PNT_DEMONB,		// Demon Head B
 PNT_UFO,			// UFO
 PNT_SPIKER,		// Spiker Reverse
 PNT_SPIKE,			// Spike
 PNT_FLIPPER,		// Flipper phase a
 PNT_FLIPPER,		// Flipper phase b
 PNT_FLIPPER,		// Flipper phase c
 PNT_FLIPPER,		// Flipper phase d
 0,					// Powerup
 0,					// Powerup b
 0,					// Explosion
 0,					// 1UP
 0,					// Game Over
 0,					// YES YES YES
 0,					// CLAW DEATH
 0,					// CLAW CAUGHT
 0,					// DRIOD
 0,					// TEST MARKER
 0,					// TEST LINE
 0,					// SUPER ZAP HIDDEN
 0					// DESTROYED
};

const collisionFunctionPointer_t collisionFunctionPointers[][39] PROGMEM = {
{ // CLAW HITS
 NULL, 					// empty
 NULL,					// CLAW
 NULL,					// bullet
 NULL,					// laser
 NULL,					// bullet exploded
 NULL,					// laser exploded
 NULL,	 				// SUPER ZAP
 collClawShot,			// Reflected Bullet
 collClawShot,			// Reflected Laser
 collClawShot,			// ZAP (Enemy bullet)
 collClawCaught,		// non-flipper
 collClawCaught,		// Tanker
 NULL,					// Spiker
 collClawShot,			// Fuseball
 collClawShot,			// Pulsar
 collClawShot,			// Mirror
 collClawCaught,		// Mutant Flipper
 collClawShot,			// Demon Head A
 collClawShot,			// Demon Head B
 NULL,					// UFO
 NULL,					// Spiker Reverse
 collClawShot,			// Spike
 collClawFlipperAB,		// Flipper phase a
 collClawFlipperAB,		// Flipper phase b
 collClawFlipperCD,		// Flipper phase c
 collClawFlipperCD,		// Flipper phase d
 collClawPowerUp,		// Powerup
 collClawPowerUp,		// Powerup b
 NULL,					// Explosion
 NULL,					// 1UP
 NULL,					// Game Over
 NULL,					// YES YES YES
 NULL,					// CLAW DEATH
 NULL,					// CLAW CAUGHT
 NULL,					// DRIOD
 NULL,					// TEST MARKER
 NULL,					// TEST LINE
 NULL,					// SUPER ZAP HIDDEN
 NULL					// DESTROYED
},
{ // BULLET HITS
 NULL,	 				// empty
 NULL,					// CLAW
 NULL,					// bullet
 NULL,					// laser
 NULL,					// bullet exploded
 NULL,					// laser exploded
 NULL,	 				// SUPER ZAP
 NULL,					// Reflected Bullet
 NULL,					// Reflected Laser
 collBulletZap,			// ZAP (Enemy bullet)
 collLaserFlipper,		// non-flipper
 collLaserTanker,		// Tanker
 collLaserSpiker,		// Spiker
 collLaserFuseBall,		// Fuseball
 collBulletZap,			// Pulsar
 collBulletMirror,		// Mirror
 NULL,					// Mutant Flipper
 collLaserDemonA,		// Demon Head A
 collLaserDemonB,		// Demon Head B
 NULL,					// UFO
 collLaserSpiker,		// Spiker Reverse
 collBulletSpike,		// Spike
 collLaserFlipperAB,	// Flipper phase a
 collLaserFlipperAB,	// Flipper phase b
 collLaserFlipperCD,	// Flipper phase c
 collLaserFlipperCD,	// Flipper phase d
 NULL,					// Powerup
 NULL,					// Powerup b
 NULL,					// Explosion
 NULL,					// 1UP
 NULL,					// Game Over
 NULL,					// YES YES YES
 NULL,					// CLAW DEATH
 NULL,					// CLAW CAUGHT
 NULL,					// DRIOD
 NULL,					// TEST MARKER
 NULL,					// TEST LINE
 NULL,					// SUPER ZAP HIDDEN
 NULL					// DESTROYED
},
{ // LASER HITS
 NULL, 					// empty
 NULL,					// CLAW
 NULL,					// bullet
 NULL,					// laser
 NULL,					// bullet exploded
 NULL,					// laser exploded
 NULL,	 				// SUPER ZAP
 NULL,					// Reflected Bullet
 NULL,					// Reflected Laser
 collLaserZap,			// ZAP (Enemy bullet)
 collLaserFlipper,		// non-flipper
 collLaserTanker,		// Tanker
 collLaserSpiker,		// Spiker
 collLaserFuseBall,		// Fuseball
 collLaserZap,			// Pulsar
 collLaserMirror,		// Mirror
 NULL,					// Mutant Flipper
 collLaserDemonA,		// Demon Head A
 collLaserDemonB,		// Demon Head B
 NULL,					// UFO
 collLaserSpiker,		// Spiker Reverse
 collLaserSpike,		// Spike
 collLaserFlipperAB,	// Flipper phase a
 collLaserFlipperAB,	// Flipper phase b
 collLaserFlipperCD,	// Flipper phase c
 collLaserFlipperCD,	// Flipper phase d
 NULL,					// Powerup
 NULL,					// Powerup b
 NULL,					// Explosion
 NULL,					// 1UP
 NULL,					// Game Over
 NULL,					// YES YES YES
 NULL,					// CLAW DEATH
 NULL,					// CLAW CAUGHT
 NULL,					// DRIOD
 NULL,					// TEST MARKER
 NULL,					// TEST LINE
 NULL,					// SUPER ZAP HIDDEN
 NULL					// DESTROYED
},
{ // EX BULLET HITS
 NULL,	 				// empty
 NULL,					// CLAW
 NULL,					// bullet
 NULL,					// laser
 NULL,					// bullet exploded
 NULL,					// laser exploded
 NULL,	 				// SUPER ZAP
 NULL,					// Reflected Bullet
 NULL,					// Reflected Laser
 collBulletZap,			// ZAP (Enemy bullet)
 collLaserFlipper,		// non-flipper
 collLaserTanker,		// Tanker
 collLaserSpiker,		// Spiker
 collLaserFuseBall,		// Fuseball
 collBulletZap,			// Pulsar
 collBulletMirror,		// Mirror
 NULL,					// Mutant Flipper
 collLaserDemonA,		// Demon Head A
 collLaserDemonB,		// Demon Head B
 NULL,					// UFO
 collLaserSpiker,		// Spiker Reverse
 collBulletSpike,		// Spike
 collLaserFlipperAB,	// Flipper phase a
 collLaserFlipperAB,	// Flipper phase b
 collLaserFlipperCD,	// Flipper phase c
 collLaserFlipperCD,	// Flipper phase d
 NULL,					// Powerup
 NULL,					// Powerup b
 NULL,					// Explosion
 NULL,					// 1UP
 NULL,					// Game Over
 NULL,					// YES YES YES
 NULL,					// CLAW DEATH
 NULL,					// CLAW CAUGHT
 NULL,					// DRIOD
 NULL,					// TEST MARKER
 NULL,					// TEST LINE
 NULL,					// SUPER ZAP HIDDEN
 NULL					// DESTROYED
},
{ // EX LASER HITS
 NULL, 					// empty
 NULL,					// CLAW
 NULL,					// bullet
 NULL,					// laser
 NULL,					// bullet exploded
 NULL,					// laser exploded
 NULL,	 				// SUPER ZAP
 NULL,					// Reflected Bullet
 NULL,					// Reflected Laser
 collLaserZap,			// ZAP (Enemy bullet)
 collLaserFlipper,		// non-flipper
 collLaserTanker,		// Tanker
 collLaserSpiker,		// Spiker
 collLaserFuseBall,		// Fuseball
 collLaserZap,			// Pulsar
 collLaserMirror,		// Mirror
 NULL,					// Mutant Flipper
 collLaserDemonA,		// Demon Head A
 collLaserDemonB,		// Demon Head B
 NULL,					// UFO
 collLaserSpiker,		// Spiker Reverse
 collLaserSpike,		// Spike
 collLaserFlipperAB,	// Flipper phase a
 collLaserFlipperAB,	// Flipper phase b
 collLaserFlipperCD,	// Flipper phase c
 collLaserFlipperCD,	// Flipper phase d
 NULL,					// Powerup
 NULL,					// Powerup b
 NULL,					// Explosion
 NULL,					// 1UP
 NULL,					// Game Over
 NULL,					// YES YES YES
 NULL,					// CLAW DEATH
 NULL,					// CLAW CAUGHT
 NULL,					// DRIOD
 NULL,					// TEST MARKER
 NULL,					// TEST LINE
 NULL,					// SUPER ZAP HIDDEN
 NULL					// DESTROYED
},
{ // SUPER ZAP HITS
 NULL, 					// empty
 NULL,					// CLAW
 NULL,					// bullet
 NULL,					// laser
 NULL,					// bullet exploded
 NULL,					// laser exploded
 NULL,	 				// SUPER ZAP
 collsuper,				// Reflected Bullet
 collsuper,				// Reflected Laser
 collsuper,				// ZAP (Enemy bullet)
 collsuperdec,			// non-flipper
 collsuperdec,			// Tanker
 collsuperdec,			// Spiker
 collsuperdec,			// Fuseball
 collsuperdec,			// Pulsar
 collsuperdec,			// Mirror
 collsuperdec,			// Mutant Flipper
 collsuperdec,			// Demon Head A
 collsuperdec,			// Demon Head B
 collsuperdec,			// UFO
 collsuperdec,			// Spiker Reverse
 NULL,					// Spike
 collsuperdec,			// Flipper phase a
 collsuperdec,			// Flipper phase b
 collsuperdec,			// Flipper phase c
 collsuperdec,			// Flipper phase d
 NULL,					// Powerup
 NULL,					// Powerup b
 NULL,					// Explosion
 NULL,					// 1UP
 NULL,					// Game Over
 NULL,					// YES YES YES
 NULL,					// CLAW DEATH
 NULL,					// CLAW CAUGHT
 NULL,					// DRIOD
 NULL,					// TEST MARKER
 NULL,					// TEST LINE
 NULL,					// SUPER ZAP HIDDEN
 NULL					// DESTROYED
},

};

const drawFunctionPointer_t drawFunctionPointers[] PROGMEM = {
 NULL, 			// empty
 drawclaw,		// CLAW
 drawbullet,	// bullet
 drawlaser,		// laser
 drawbullet,	// bullet exploded
 drawlaser,		// laser exploded
 drawsuperzap,	// SUPER ZAP
 drawbullet,	// Reflected Bullet
 drawlaser,		// Reflected Laser
 drawzap,		// ZAP (Enemy bullet)
 drawflipper,	// non - Flipper
 drawtanker,	// Tanker
 drawspiker,	// Spiker
 drawfuseball,	// Fuseball
 drawpulsar,	// Pulsar
 drawmirror,	// Mirror
 drawmutflipper,// Mutant Flipper
 drawdemona, 	// Demon Head A
 drawdemonb, 	// Demon Head B
 drawlaser, 	// UFO
 drawspiker,	// Spiker Reverse
 drawspike,		// Spike
 drawflipper,   // Flipper phase A
 drawflipper,   // Flipper phase B
 drawflipper,   // Flipper phase C
 drawflipper,   // Flipper phase D
 drawpowerup,	// Powerup
 drawpowerupb,	// Powerup b
 drawexplosion, // Explosion
 drawoneup,		// 1UP
 drawlaser,		// Game Over
 drawyesyesyes,	// YES YES YES
 drawclawdeath, // CLAW DEATH
 drawclaw,		// CLAW CAUGHT
 drawdroid,		// DRIOD
 drawtestmarker,// TEST MARKER
 drawtestline,	// TEST LINE
 drawblank,		// SUPER ZAP HIDDEN
 drawblank		// DESTROYED
};

const drawFunctionPointer_t moveFunctionPointers[] PROGMEM = {
 NULL, 			// empty
 moveclaw,		// CLAW
 movebullet,	// bullet
 movelaser,		// laser
 movebulletex,	// bullet exploded
 movelaserex,	// laser exploded
 movesuperzap,	// SUPER ZAP
 moverefbullet,	// Reflected Bullet
 movereflaser,	// Reflected Laser
 movezap,		// ZAP (Enemy bullet)
 movenonflipper,// non-flipper
 movetanker,	// Tanker
 movespiker,	// Spiker
 movefuseball,	// Fuseball
 movepulsar,	// Pulsar
 movemirror,	// Mirror
 movelaser,		// Mutant Flipper
 movedemona, 	// Demon Head A
 movedemonb, 	// Demon Head B
 movelaser, 	// UFO
 movespikerrev,	// Spiker Reverse
 movespike,		// Spike
 moveflipper,   // Flipper phase a
 moveflipper,	// Flipper phase b
 moveflipper,   // Flipper phase c
 moveflipper,	// Flipper phase d
 movepowerup,	// Powerup
 movepowerupb,	// Powerup b
 moveexplosion, // Explosion
 moveoneup,		// 1UP
 movelaser,		// Game Over
 moveyesyesyes,	// YES YES YES
 moveclawdeath, // CLAW DEATH
 moveclawcaught,// CLAW CAUGHT
 movedroid,		// DRIOD
 movetestmarker,// TEST MARKER
 movetestline,  // TEST LINE
 movesuperhid, 	// SUPER ZAP HIDDEN
 movedestroyed	// DESTROYED
};

const drawFunctionPointer_t firstActionFunctionPointers[] PROGMEM = {
 NULL, 			// empty
 factclaw,		// CLAW
 NULL,			// bullet
 NULL,			// laser
 NULL,			// bullet exploded
 NULL,			// laser exploded
 NULL,			// SUPER ZAP
 NULL,			// Reflected Bullet
 NULL,			// Reflected Laser
 NULL,			// ZAP (Enemy bullet)
 factnonflipper,// non-flipper
 facttanker,	// Tanker
 factspiker,	// Spiker
 factfuseball,	// Fuseball
 NULL,			// Pulsar
 NULL,			// Mirror
 NULL,			// Mutant Flipper
 factdemona,	// Demon Head A
 NULL,			// Demon Head B
 NULL,			// UFO
 factspikerrev,	// Spiker Reverse
 factspike,		// Spike
 NULL,			// Flipper phase a
 NULL,			// Flipper phase b
 NULL,			// Flipper phase c
 NULL,			// Flipper phase d
 factpowerup,	// Powerup
 NULL,			// Powerup b
 NULL,			// Explosion
 factoneup,		// 1UP
 NULL,			// Game Over
 factyesyesyes,	// YES YES YES
 NULL,			// CLAW DEATH
 NULL,			// CLAW CAUGHT
 factdroid,		// DRIOD
 NULL,			// TEST MARKER
 NULL,			// TEST LINE
 factsuperhid,	// SUPER ZAP HIDDEN
 NULL			// DESTROYED
};


void factsuperhid(ObjectDescStruct *objData){
	objData->animation = SUPER_ZAP_HID_TIME;
}

void factclaw(ObjectDescStruct *objData){
	objData->x1 = 0;
}
void factfuseball(ObjectDescStruct *objData){
	objData->r = 128;
	objData->action3 = 0;
	objData->actionTimer5 = rand()&(PROBABILITY_FUSEBALL_DIR_CHANGE_TIME-1);
}
void factpulsar(ObjectDescStruct *objData){
	objData->r = 0;
}
void facttanker(ObjectDescStruct *objData){
	objData->action1 = 0;
	objData->actionTimer7 = rand()&(PROBABILITY_ZAP-1);
}
void factspike(ObjectDescStruct *objData){
	objData->x1 = objData->x;
	objData->y1 = objData->y;
}
void factspiker(ObjectDescStruct *objData){

	if((rand()&0x7F) > PROBABILITY_SPIKER_DIR_ZAP_SPLIT) {
		objData->action1 = 0;
		objData->actionTimer7 = rand()&(PROBABILITY_ZAP-1);
	} else {
		objData->action1 = 1;
		objData->actionTimer7 = rand()&(PROBABILITY_SPIKER_DIRECTION-1);
	}
}
void factspikerrev(ObjectDescStruct *objData){

	objData->action1 = 0;
	objData->actionTimer7 = rand()&(PROBABILITY_ZAP-1);
}
void factpowerup(ObjectDescStruct *objData){

	objData->action3 = POWERUP_SHADOW_COUNT;
	objData->actionTimer5 = POWERUP_DWELL_TIME;
}
void factnonflipper(ObjectDescStruct *objData){

	if((rand()&0x7F) > PROBABILITY_FLIPPER_FLIP_ZAP_SPLIT) {
		objData->action1 = 0;
		objData->actionTimer7 = rand()&(PROBABILITY_ZAP-1);
	} else {
		objData->action1 = 1;
		objData->actionTimer7 = rand()&(PROBABILITY_FLIPPER_FLIP-1);
	}
}
void factoneup(ObjectDescStruct *objData){
	objData->x = 128;
	objData->y = 112;
	objData->r = 236;
}
void factyesyesyes(ObjectDescStruct *objData){
	objData->x = 128;
	objData->y = 112;
	objData->r = 175;
}
void factdroid(ObjectDescStruct *objData){
	objData->z = jWebMax;
	objData->action1 = 1;
	objData->actionTimer7 = DROID_FIRE_FREQ;
}
void factdemona(ObjectDescStruct *objData){
	uint8_t newObjNum;
	ObjectDescStruct *obNew;

	newObjNum = newObject(OBJ_DEMONB,0,0);
	obNew = (ObjectDescStruct*)&ObjectStore[newObjNum];

	if(newObjNum != (MAX_OBJS+1)) {
		activeEnemy++;
		obNew->lane = objData->lane;
		obNew->z = objData->z;
		obNew->r = objData->r;
		objData->actionTimer7 = newObjNum;
		objData->action1 = 1;
		obNew->animation = 1;
	}
}
void clearTestMarkers(void){

	uint8_t i;
	ObjectDescStruct *Current;

	for(i = 0; i < MAX_OBJS; i++) {
		Current = (ObjectDescStruct*)&ObjectStore[i];
		if ((Current->obType == OBJ_TEST_MARKER) || (Current->obType == OBJ_TEST_LINE)) {
			Current->obType = OBJ_EMPTY;
		}
	}
}
void clearObjectStore(void)
{
	uint8_t i;
	ObjectDescStruct *Current;

	for(i = 0; i < MAX_OBJS; i++) {
		Current = (ObjectDescStruct*)&ObjectStore[i];
		Current->obType = OBJ_EMPTY;
	}
}



uint8_t getFreeObject(void){
	uint8_t i = 0;
	uint8_t Particle = (MAX_OBJS+1);

	ObjectDescStruct *Current;

	do {
		Current = (ObjectDescStruct*)&ObjectStore[i];
		if (Current->obType == OBJ_EMPTY) {
			return(i);
		}
		if (Current->obType == OBJ_PARTICLE) {
			Particle = i;
		}
		i++;
	} while (i<MAX_OBJS);

	return(Particle);
}

uint8_t newObject(uint8_t obType, uint8_t lane, uint8_t z){

uint8_t NewObjNum;
ObjectDescStruct *Current;
drawFunctionPointer_t firstActionFunction;

NewObjNum = getFreeObject();

if(NewObjNum != (MAX_OBJS+1)) {

	Current = (ObjectDescStruct*)&ObjectStore[NewObjNum];
	Current->obType  	= obType;
	Current->laneHi		= lane;
	Current->z			= z;
	Current->r 			= pgm_read_byte(&laneAngles[webNum].lane[lane]);
	Current->animation 	= 0;

	firstActionFunction = (drawFunctionPointer_t)pgm_read_word(&firstActionFunctionPointers[obType]);
	if(firstActionFunction != NULL) {
		firstActionFunction(Current);
	}
	moveXYCommon(Current);

}
return(NewObjNum);
}
uint8_t newTestMarker(uint8_t x, uint8_t y, uint8_t life){

	uint8_t tm;

	tm = newObject(OBJ_TEST_MARKER, 0, 0);

	ObjectStore[tm].x = x;
	ObjectStore[tm].y = y;
	ObjectStore[tm].animation = life;

	return(tm);
}
uint8_t newTestLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t life){

	uint8_t tl;

	tl = newObject(OBJ_TEST_LINE, 0, 0);

	ObjectStore[tl].x = x0;
	ObjectStore[tl].y = y0;
	ObjectStore[tl].x1 = x1;
	ObjectStore[tl].y1 = y1;
	ObjectStore[tl].animation = life;

	return(tl);
}
void newZap(ObjectDescStruct *objData){
	newObject(OBJ_ZAP,objData->laneHi,objData->z);
	T2k_TriggerFx(SFX_ENEMY_ZAP,0,0);
}
void newSuperZap(void){
	newObject(OBJ_SUPER_HID, 0, 0);
	setSuperZapActiveFlag();
	SetHsyncCallback(&flashingCallback);
	T2k_TriggerFx(SFX_SUPER_ZAP,0,0);
}
void enableAIDroid(void){
	newObject(OBJ_DROID,0,jWebMax);
	setAIDroidFlag();
	message(MSG_AI_DROID, MESSAGE_DEFAULT_POSITION);
}

/* This has been moved to ASM.  ASM only gets the for look / jump-table slightly faster
 * but it makes huge savings because the C-entry/C-exit routines for c-EBI compatibility
 * can be moved to the outside of this loop saving 100s of push/pops

void renderObjects(void){
	uint8_t i;
	ObjectDescStruct *Current;

	drawFunctionPointer_t drawFunction;

	for(i = 0; i < MAX_OBJS; i++) {
		Current = (ObjectDescStruct*)&ObjectStore[i];
		if(Current->obType != OBJ_EMPTY)
		{
			drawFunction = (drawFunctionPointer_t)pgm_read_word(&drawFunctionPointers[Current->obType]);
			drawFunction(Current);
		}
	}
}
*/

uint16_t collsuperdec(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points;
	points = pgm_read_word(&enemyPoints[ob2->obType]);
	ob2->obType = OBJ_BULLET_EX;
	activeEnemy--;
	powerUpCycle(ob2);
	return(points);
}
uint16_t collsuper(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points;
	points = pgm_read_word(&enemyPoints[ob2->obType]);
	ob2->obType = OBJ_BULLET_EX;
	T2k_TriggerFx(SFX_ENEMY_EXPLODE,0,0);
	powerUpCycle(ob2);
	return(points);
}
uint16_t collBulletZap(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points;
	points = pgm_read_word(&enemyPoints[ob2->obType]);
	ob1->obType = OBJ_BULLET_EX;
	ob2->obType = OBJ_DESTROYED;
	powerUpCycle(ob2);
	return(points);
}
uint16_t collLaserZap(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points;
	points = pgm_read_word(&enemyPoints[ob2->obType]);
	ob2->obType = OBJ_EXPLOSION;
	ob2->s = 1;
	powerUpCycle(ob2);
	return(points);
}
uint16_t collLaserFlipper(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points;
	points = pgm_read_word(&enemyPoints[ob2->obType]);
	ob1->obType = OBJ_LASER_EX;
	ob2->obType = OBJ_DESTROYED;
	T2k_TriggerFx(SFX_ENEMY_EXPLODE,0,0);
	activeEnemy--;
	powerUpCycle(ob2);
	return(points);
}
uint16_t collLaserFlipperAB(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points = 0;
	if(ob2->actionTimer7 < FLIPPER_SHOOTABLE_AB) {
		points = pgm_read_word(&enemyPoints[ob2->obType]);
		ob1->obType = OBJ_LASER_EX;
		ob2->obType = OBJ_DESTROYED;
		T2k_TriggerFx(SFX_ENEMY_EXPLODE,0,0);
		activeEnemy--;
		powerUpCycle(ob2);
	}
	return(points);
}
uint16_t collLaserFlipperCD(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points = 0;
	if(ob2->actionTimer7 < FLIPPER_SHOOTABLE_CD) {
		points = pgm_read_word(&enemyPoints[ob2->obType]);
		ob1->obType = OBJ_LASER_EX;
		ob2->obType = OBJ_DESTROYED;
		T2k_TriggerFx(SFX_ENEMY_EXPLODE,0,0);
		activeEnemy--;
		powerUpCycle(ob2);
	}
	return(points);
}
void powerUpCycle(ObjectDescStruct *ob1){

	if(powerUp.Count == 0) {
		newObject(OBJ_POWERUP,ob1->laneHi,ob1->z);
		T2k_TriggerFx(SFX_POWERUP,0,0);
		powerUp.Count = POWERUP_FREQUENCY;
	} else {
		powerUp.Count--;
	}
}

void breakTanker(ObjectDescStruct *ob1){
	uint8_t newObjNum;
	ObjectDescStruct *ob2;

	T2k_TriggerFx(SFX_TANKER_BREAK,0,0);

	ob1->obType = OBJ_FLIPPER_A;
	ob1->animation = 64;

	if (LANE_CLOSED_FLAG){
		if ((ob1->laneHi) == 0) { ob1->obType = OBJ_FLIPPER_D; }
	}

	newObjNum = newObject(OBJ_FLIPPER_B,0,0);
	ob2 = (ObjectDescStruct*)&ObjectStore[newObjNum];

	if(newObjNum != (MAX_OBJS+1)) {
		activeEnemy++;
		ob2->lane = ob1->lane;
		ob2->z = ob1->z;
		ob2->animation = 64;
		if (LANE_CLOSED_FLAG){
			if ((ob2->laneHi) == 14) { ob2->obType = OBJ_FLIPPER_C; }
		}
	}
}
uint16_t collLaserTanker(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points;
	points = pgm_read_word(&enemyPoints[ob2->obType]);
	ob1->obType = OBJ_EXPLOSION;							// This is an explosion NOT a bullet_ex so that the flipper doesn't instantly get hit.
	ob1->s = 1;
	breakTanker(ob2);
	powerUpCycle(ob1);
	return(points);
}
uint16_t collLaserSpiker(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points;
	points = pgm_read_word(&enemyPoints[ob2->obType]);
	ob1->obType = OBJ_LASER_EX;
	ob2->obType = OBJ_DESTROYED;
	T2k_TriggerFx(SFX_ENEMY_EXPLODE,0,0);
	activeEnemy--;
	powerUpCycle(ob2);
	return(points);
}

uint16_t collLaserSpike(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points;
	points = pgm_read_word(&enemyPoints[ob2->obType]);
	ob1->obType = OBJ_LASER_EX;

	ob2->z -= SPIKE_HIT_LASER;
	T2k_TriggerFx(SFX_SPIKE_SHOT,0,0);

	uint8_t spikeLane;			// destroy the object for the spike and clear the spike_lane_thing if its below minZ
	if (ob2->z < jWebMin) {
		ob2->obType = OBJ_DESTROYED;
		spikeLane = ob2->laneHi;
		laneData[spikeLane].spikeObj = (MAX_OBJS + 1);
	}
	return(points);
}
uint16_t collLaserFuseBall(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points = 0;
	if((ob2->r > FUSEBALL_SHOOTABLE_MIN) && (ob2->r < FUSEBALL_SHOOTABLE_MAX)) {
		points = pgm_read_word(&enemyPoints[ob2->obType]);
		ob1->obType = OBJ_LASER_EX;
		ob2->obType = OBJ_DESTROYED;
		T2k_TriggerFx(SFX_ENEMY_EXPLODE,0,0);
		activeEnemy--;
		powerUpCycle(ob2);
	}
	return(points);
}
uint16_t collBulletSpike(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points;
	points = pgm_read_word(&enemyPoints[ob2->obType]);
	ob1->obType = OBJ_BULLET_EX;

	ob2->z -= SPIKE_HIT_BULLET;
	T2k_TriggerFx(SFX_SPIKE_SHOT,0,0);

	uint8_t spikeLane;			// destroy the object for the spike and clear the spike_lane_thing if its below minZ
	if (ob2->z < jWebMin) {
		ob2->obType = OBJ_DESTROYED;
		spikeLane = ob2->laneHi;
		laneData[spikeLane].spikeObj = (MAX_OBJS + 1);
	}
	return(points);
}

uint16_t collBulletMirror(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points = 0;
	if(ob2->action1 == 1) {
		ob1->obType = OBJ_REFBULLET;
	} else {
		points = pgm_read_word(&enemyPoints[ob2->obType]);
		ob1->obType = OBJ_LASER_EX;
		ob2->obType = OBJ_DESTROYED;
		T2k_TriggerFx(SFX_ENEMY_EXPLODE,0,0);
		activeEnemy--;
		powerUpCycle(ob2);
	}
	return(points);
}
uint16_t collLaserMirror(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points = 0;
	if(ob2->action1 == 1) {
		ob1->obType = OBJ_REFLASER;
	} else {
		points = pgm_read_word(&enemyPoints[ob2->obType]);
		ob1->obType = OBJ_LASER_EX;
		ob2->obType = OBJ_DESTROYED;
		T2k_TriggerFx(SFX_ENEMY_EXPLODE,0,0);
		activeEnemy--;
		powerUpCycle(ob2);
	}
	return(points);
}

uint16_t collLaserDemonA(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	uint16_t points = 0;
	ObjectDescStruct *ob3;

	if(ob2->action1 == 1) {
		ob3 = (ObjectDescStruct*)&ObjectStore[ob2->actionTimer7];
		ob3->animation = 0;
		ob2->action1 = 0;
		ob2->actionTimer7 = DEMON_A_INVINCIBLE;
		ob1->obType = OBJ_EMPTY;
	} else if (ob2->actionTimer7 == 0) {
		points = pgm_read_word(&enemyPoints[ob2->obType]);
		ob1->obType = OBJ_EMPTY;
		ob2->obType = OBJ_DESTROYED;
		T2k_TriggerFx(SFX_ENEMY_EXPLODE,0,0);
		activeEnemy--;
		powerUpCycle(ob2);
	}

	return(points);
}
uint16_t collLaserDemonB(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	if(ob2->animation == 0) {
		ob1->obType = OBJ_LASER_EX;
	}
	return(0);
}


uint16_t collClawShot(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	ob2->obType = OBJ_DESTROYED;
	ob1->obType = OBJ_CLAWDEATH;
	T2k_TriggerFx(SFX_CLAW_EXPLODE,0,0);
	ob1->s = 1;
	ob1->animation = 1;
	setAliveStateBFlag();
	return(0);
}

uint16_t collClawCaught(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	ob1->obType = OBJ_CLAWCAUGHT;
	T2k_TriggerFx(SFX_CLAW_CAUGHT,0,0);
	ob1->animation = GET_OBJECT_NUMBER(ob2);
	setAliveStateBFlag();
	return(0);
}
uint16_t collClawFlipperAB(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	if(ob2->actionTimer7 < FLIPPER_GRABABLE) {
		ob1->obType = OBJ_CLAWCAUGHT;
		T2k_TriggerFx(SFX_CLAW_CAUGHT,0,0);
		ob1->animation = GET_OBJECT_NUMBER(ob2);
		setAliveStateBFlag();
	}
	return(0);
}
uint16_t collClawFlipperCD(ObjectDescStruct *ob1, ObjectDescStruct *ob2){
	if(ob2->actionTimer7 < FLIPPER_GRABABLE) {
		ob1->obType = OBJ_CLAWCAUGHT;
		T2k_TriggerFx(SFX_CLAW_CAUGHT,0,0);
		ob1->animation = GET_OBJECT_NUMBER(ob2);
		setAliveStateBFlag();
	}
	return(0);
}
uint16_t collClawPowerUp(ObjectDescStruct *ob1, ObjectDescStruct *ob2){

	static uint8_t powerUpSequence = 0;

	// if you are zooming then active yes yes yes
	// else if yes yes yes is active then power up = droid
	// else if particle laser not active then power up = particle laser
	// else active next power up in sequence

	uint8_t i;

	for (i=0; i < MAX_OBJS; i++) {
		if((ObjectStore[i].obType == OBJ_POWERUP) || (ObjectStore[i].obType == OBJ_POWERUP_B)) {
			ObjectStore[i].obType = OBJ_DESTROYED;
		}
	}

	if (ZOOMING_FLAG) {
		setYesYesYesFlag();
		newObject(OBJ_YESYESYES,0,0);
	} else if (YES_YES_YES_FLAG) {
		enableAIDroid();
		clearYesYesYesFlag();
	} else if (!(LASER_POWER_UP)) {
		setLaserPowerupFlag();
		message(MSG_PARTICLE_LASER, MESSAGE_DEFAULT_POSITION);
	} else {		switch (powerUpSequence) {
		case 0 : {								// Zappo
			addScore(0x2000);
			message(MSG_ZAPPO, MESSAGE_DEFAULT_POSITION);
			powerUpSequence++;
			break;
			}
		case 1 : {								// Jump
			setJumpFlag();
			message(MSG_JUMP_ENABLED, MESSAGE_DEFAULT_POSITION);
			powerUpSequence++;
			break;
			}
		case 2 : {								// Zappo
			addScore(0x2000);
			message(MSG_ZAPPO, MESSAGE_DEFAULT_POSITION);
			powerUpSequence++;
			break;
			}
		case 3 : {								// AI Droid
			if (AIDROID_FLAG) {
				addScore(0x2000);
				message(MSG_ZAPPO, MESSAGE_DEFAULT_POSITION);
				powerUpSequence += 2;
			} else {
				enableAIDroid();
				powerUpSequence++;
			}
			break;
			}
		case 4 : {								// Zappo
			addScore(0x2000);
			message(MSG_ZAPPO, MESSAGE_DEFAULT_POSITION);
			powerUpSequence++;
			break;
			}
		case 5 : {								// Excellent
			newSuperZap();
			message(MSG_EXCELLENT, MESSAGE_DEFAULT_POSITION);
			powerUpSequence++;
			break;
			}
		case 6 : {								// Zappo
			addScore(0x2000);
			message(MSG_ZAPPO, MESSAGE_DEFAULT_POSITION);
			powerUpSequence++;
			break;
			}
		default : {								// Outta Here
			powerUpSequence = 0;
			message(MSG_OUTTA_HERE, MESSAGE_DEFAULT_POSITION);
			setZoomingFlag();
			break;
			}
		}
	}

	return(0);
}

void collisionDetection(void){
	uint8_t i,j;
	ObjectDescStruct *ob1, *ob2;
	collisionFunctionPointer_t  collisionFunction;
	uint16_t pointsScored;

	for(i = 0; i < (MAX_OBJS -1); i++) {
		j = i+1;
		while (j < MAX_OBJS){
			ob1 = (ObjectDescStruct*)&ObjectStore[i];
			ob2 = (ObjectDescStruct*)&ObjectStore[j];

			if((((ob1->laneHi) == (ob2->laneHi)) && ((abs(ob1->z - ob2->z) < COLLISION_CLOSE_Z_RANGE)))
			   || (ob1->obType == OBJ_SUPER_ZAP)
			   || (ob2->obType == OBJ_SUPER_ZAP)){

				if ((ob2->obType) < (ob1->obType)) {
					ob1 = (ObjectDescStruct*)&ObjectStore[j];
					ob2 = (ObjectDescStruct*)&ObjectStore[i];
				}

				if ((ob1->obType <= OBJ_COLL_MID) && (ob2->obType > OBJ_COLL_MID) && (ob1->obType >= OBJ_COLL_MIN) && (ob2->obType <= OBJ_COLL_MAX)) {
					collisionFunction = (collisionFunctionPointer_t)pgm_read_word(&collisionFunctionPointers[(ob1->obType-1)][ob2->obType]);
					if ((collisionFunction != NULL) && ((!ZOOMING_FLAG) || (ob2->obType == OBJ_SPIKE))) {
						pointsScored = collisionFunction(ob1, ob2);
						addScore(pointsScored);
						if(pointsScored > 0) {
							if (ob2->z < Z_WEB_DOM_BONUS_PLUS2) dominationBonus ++;
							if (ob2->z < Z_WEB_DOM_BONUS_PLUS1) dominationBonus ++;
							if (ob2->z > Z_WEB_DOM_BONUS_MINUS1) dominationBonus --;
							if (dominationBonus == 255) dominationBonus = 0;
						}
					}
				}
			}

			j++;
			if (((ObjectDescStruct*)&ObjectStore[i])->obType == OBJ_DESTROYED){
				j = MAX_OBJS;
			}
			if (ALIVE_STATE_B_FLAG) {
				j = MAX_OBJS;
				i = MAX_OBJS - 1;
			}
		}
	}
}


void moveObjects(void){
	uint8_t i;
	int8_t jumpBack = 0;
	ObjectDescStruct *Current;
	drawFunctionPointer_t moveFunction;

	actionTimerSubCount++;
	if (actionTimerSubCount > ACTION_TIMER_SUB_COUNT_MAX) {
		actionTimerSubCount = 0;
		setActionTimerFlag();
	} else {
		clearActionTimerFlag();
	}


	if (bonusLifeScore == 0x00) {
		if ((score[2] & 0x0F) < 0x05){
			newObject(OBJ_ONEUP,0,0);
			lives++;
			bonusLifeScore = 0x05;
		}
	} else {
		if ((score[2] & 0x0F) >= 0x05){
			newObject(OBJ_ONEUP,0,0);
			lives++;
			bonusLifeScore = 0x00;
		}
	}

	if(jumpCount < NOT_JUMPING) {
		zoom     =  pgm_read_byte(&jumpTable[jumpCount]);
		jumpBack = pgm_read_byte(&jumpBackTable[jumpCount]);
		jumpCount++;
		jWebMin += jumpBack;
		jWebMax += jumpBack;
	} else if ((ZOOMING_FLAG) && (!ALIVE_STATE_B_FLAG)){
		jWebMax = Z_WEB_ABS_MAX;
		jumpBack = 14;
		zoom--;
		if (zoom == 32) {
			clearZoomingFlag();
		}
		if (zoom == 0) {
			addBonusPoints();
			level++;
			startLevel();
		}
	}

	for(i = 0; i < MAX_OBJS; i++) {
		Current = (ObjectDescStruct*)&ObjectStore[i];

		if(Current->obType != OBJ_EMPTY)
		{
			if((!(Current->obType >= OBJ_SPIKE)) && (Current->obType >= OBJ_MIN_ENEMY) && (Current->obType <= OBJ_MAX_ENEMY)) {
				if (laneData[Current->laneHi].highestEnemy < Current->z) {
					laneData[Current->laneHi].highestEnemy = Current->z;
				}
			} else if ((Current->obType == OBJ_LASER) || (Current->obType == OBJ_BULLET)) {
				if (laneData[Current->laneHi].highestEnemy < Current->z) {
					laneData[Current->laneHi].highestEnemy = 0;
				}
			}

			if(Current->obType != OBJ_CLAW) { Current->z += jumpBack; }

			if((SUPERZAP_ACTIVE_FLAG) && (Current->obType > OBJ_CLAW) && (Current->obType <= OBJ_DROID)){
				moveXYCommon(Current);
			} else {
				moveFunction = (drawFunctionPointer_t)pgm_read_word(&moveFunctionPointers[Current->obType]);
				moveFunction(Current);
			}
		}
	}
}

#define MulSU1(Z, a) \
		asm(	"mulsu	%r0, %r1 \n\t"	\
				"mov	%r0, r1\n\t"	\
				"clr	r1"				\
				: "+a" (a)				\
				: "a" (Z)				\
		);

#define MulSU2(Z, a, b) \
		asm(	"mulsu	%r0, %r2 \n\t"	\
				"mov	%r0, r1\n\t"	\
				"mulsu	%r1, %r2 \n\t"	\
				"mov	%r1, r1\n\t"	\
				"clr	r1"				\
				: "+a" (a)				\
				, "+a" (b)				\
				: "a" (Z)				\
		);

void moveXYCommon(ObjectDescStruct *objData){


	uint8_t Z;
	uint8_t lane;

	Z = pgm_read_byte(&zTable[objData->z]);							// Perspective transform the objects Z value

	objData->s = pgm_read_byte(&scaleTable[objData->z]);				// Get the scale of the object at given Z value

	lane = objData->laneHi;

	int8_t x = laneData[lane].xStart;						// Get the X/Y start position of the current lane
	int8_t y = laneData[lane].yStart;

	int8_t xAdd = laneData[lane].xGrad;						// Get the gradient of the lane
	int8_t yAdd = laneData[lane].yGrad;

	xAdd = (Z * xAdd)>>8;									// Work out how far the object is from the start of the lane
	yAdd = (Z * yAdd)>>8;

	//xAdd = MulSU(Z, xAdd);		// ASM version slower than GCC 4.9.2 when call/ret accounted for
	//yAdd = MulSU(Z, yAdd);

	//MulSU2(Z, xAdd, yAdd);		// inline ASM 1 clock faster than GCC 4.9.2 after clears and shifts

	objData->x = x + xAdd;									// Add the delta X/Y to the lane start position
	objData->y = y + yAdd;
}

uint8_t getHighestLane(void) {
	uint8_t i;
	uint8_t highestSoFar = 0;
	uint8_t highestLane = 0;

	for(i=0; i<16; i++) {
		if (highestSoFar < laneData[i].highestEnemy) {
			highestSoFar = laneData[i].highestEnemy;
			highestLane = i;
		}
	}
	highestLane = (highestLane << 4) + 7;
	return(highestLane);
}

uint8_t closestLaneDirection(uint8_t lane8Bit1, uint8_t lane8Bit2){
	if (LANE_CLOSED_FLAG){
		if(lane8Bit1 < lane8Bit2) {
			return(1);
		} else {
			return(0);
		}
	} else {
		if(((uint8_t)(lane8Bit1 - lane8Bit2)) > 0x80){
			return(1);
		} else {
			return(0);
		}
	}
}

void movebullet(ObjectDescStruct *objData){
	if (objData->z < jWebMin) {
		objData->obType = OBJ_EMPTY;
	} else {
		if (ZOOMING_FLAG) {
			objData->z -= BULLET_SPEED_ZOOMING;
		} else {
			objData->z -= BULLET_SPEED;
		}
		moveXYCommon(objData);
	}
}
void movelaser(ObjectDescStruct *objData){
	if (objData->z < jWebMin) {
		objData->obType = OBJ_EMPTY;
	} else {
		if (ZOOMING_FLAG) {
			objData->z -= LASER_SPEED_ZOOMING;
		} else {
			objData->z -= LASER_SPEED;
		}
		objData->r += LASER_SPIN;
		moveXYCommon(objData);
	}
}
void movebulletex(ObjectDescStruct *objData){
	objData->obType = OBJ_EXPLOSION;
	objData->s = 1;
}
void movelaserex(ObjectDescStruct *objData){
	objData->obType = OBJ_EXPLOSION;
	objData->s = 1;
}
void movesuperhid(ObjectDescStruct *objData){
	if (objData->animation == 0) {
		objData->obType = OBJ_SUPER_ZAP;
		clearSuperZapActiveFlag();
		SetHsyncCallback(&DefaultCallback);
		objData->animation = SUPER_ZAP_TIME;
	} else {
		objData->animation--;
	}
}
void movesuperzap(ObjectDescStruct *objData){
	if (objData->animation == 0) {
		objData->obType = OBJ_EMPTY;
	} else {
		objData->animation--;
	}
}
void moverefbullet(ObjectDescStruct *objData){
	if (objData->z > jWebMax) {
		objData->obType = OBJ_EMPTY;
	} else {
		objData->z += BULLET_SPEED;
		moveXYCommon(objData);
	}
}
void movereflaser(ObjectDescStruct *objData){
	if (objData->z > jWebMax) {
		objData->obType = OBJ_EMPTY;
	} else {
		objData->z += LASER_SPEED;
		objData->r += LASER_SPIN;
		moveXYCommon(objData);
	}
}
void movezap(ObjectDescStruct *objData){
	if(objData->z > jWebMax) {
		objData->obType = OBJ_EMPTY;
	} else {
		objData->z += ZAP_SPEED;
		objData->r += ZAP_SPIN;
		moveXYCommon(objData);
	}
}
void movenonflipper(ObjectDescStruct *objData){

	uint8_t flipNow = 0;
	uint8_t leftRight;

	if (objData->z >= jWebMax) {
		flipNow = 1;
	} else {
		objData->z += FLIPPER_SPEED;
	}

	if (ACTION_TIMER_FLAG){
		if(objData->actionTimer7 == 0) {
			if (objData->action1 == 0) {
				newZap(objData);
			} else {
				if (level != 0) {
					flipNow = 1;
				}
			}
			factnonflipper(objData);
		} else {
			objData->actionTimer7--;
		}
	}

	moveXYCommon(objData);

	if(flipNow  == 1){

		T2k_TriggerFx(SFX_FLIPPER_FLIP,0,0);

		leftRight = closestLaneDirection(CLAW_OBJECT.lane, objData->lane);

		if((rand()&0x7F) < PROBABILITY_FLIPPER_RANDOM_DIRECTION) {
			leftRight = leftRight^0x01;
		}

		if (LANE_CLOSED_FLAG){
			if ((objData->laneHi) == 14) { leftRight = 0; }
			if ((objData->laneHi) == 0) { leftRight = 1; }
		}
		if (leftRight == 0) {
			objData->obType = OBJ_FLIPPER_B;
		} else {
			objData->obType = OBJ_FLIPPER_A;
		}
		objData->animation = 0;
	}
}
void moveflipper(ObjectDescStruct *objData){

	moveXYCommon(objData);

//}
///*

	uint8_t Z;
	uint8_t laneWidth;
	int8_t xAdd, yAdd;
	uint8_t thisLane;
	uint8_t thisLaneAngle;
	uint8_t nextLane;
	uint8_t nextLaneAngle;
	uint8_t halfLaneAngle;

	uint8_t spin;
	uint8_t edgeAngle;

	moveXYCommon(objData);


	thisLane = objData->laneHi;

	switch (objData->obType) {
		case OBJ_FLIPPER_A : {
			objData->animation += FLIPPER_SPIN;
			edgeAngle = 0;
			nextLane = (thisLane - 1) & 0x0F;
			thisLaneAngle = pgm_read_byte(&laneAngles[webNum].lane[thisLane]);
			nextLaneAngle = pgm_read_byte(&laneAngles[webNum].lane[nextLane]);
			halfLaneAngle = - nextLaneAngle + thisLaneAngle;
			halfLaneAngle = halfLaneAngle  + 128;
			halfLaneAngle = halfLaneAngle>>1;
			spin = -objData->animation;
			if(objData->animation >= halfLaneAngle){
				objData->obType = OBJ_FLIPPER_C;
				objData->lane = objData->lane - 0x10;
				objData->animation = halfLaneAngle;
			}
			break;
		}
		case OBJ_FLIPPER_B : {
			objData->animation += FLIPPER_SPIN;
			edgeAngle = 128;
			nextLane = (thisLane + 1) & 0x0F;
			thisLaneAngle = pgm_read_byte(&laneAngles[webNum].lane[thisLane]);
			nextLaneAngle = pgm_read_byte(&laneAngles[webNum].lane[nextLane]);
			halfLaneAngle = nextLaneAngle - thisLaneAngle;
			halfLaneAngle = halfLaneAngle  + 128;
			halfLaneAngle = halfLaneAngle>>1;
			spin = objData->animation;
			if(objData->animation >= halfLaneAngle){
				objData->obType = OBJ_FLIPPER_D;
				objData->lane = objData->lane + 0x10;
				objData->animation = halfLaneAngle;
			}
			break;
		}
		case OBJ_FLIPPER_C : {
			objData->animation -= FLIPPER_SPIN;
			edgeAngle = 128;
			nextLane = (thisLane + 1) & 0x0F;
			thisLaneAngle = pgm_read_byte(&laneAngles[webNum].lane[thisLane]);
			nextLaneAngle = pgm_read_byte(&laneAngles[webNum].lane[nextLane]);
			halfLaneAngle = nextLaneAngle - thisLaneAngle;
			halfLaneAngle = halfLaneAngle  + 128;
			halfLaneAngle = halfLaneAngle>>1;
			spin = objData->animation;
			if(objData->animation > halfLaneAngle){
				objData->obType = OBJ_NONFLIPPER;
				objData->animation  = 0;
				spin = 0;
			}
			break;
		}
		default : {
			objData->animation -= FLIPPER_SPIN;
			edgeAngle = 0;
			nextLane = (thisLane - 1) & 0x0F;
			thisLaneAngle = pgm_read_byte(&laneAngles[webNum].lane[thisLane]);
			nextLaneAngle = pgm_read_byte(&laneAngles[webNum].lane[nextLane]);
			halfLaneAngle = - nextLaneAngle + thisLaneAngle;
			halfLaneAngle = halfLaneAngle  + 128;
			halfLaneAngle = halfLaneAngle>>1;
			spin = -objData->animation;
			if(objData->animation >= halfLaneAngle){
				objData->obType = OBJ_NONFLIPPER;
				objData->animation = 0;
				spin = 0;
			}
			break;
		}
	}

	objData->r = thisLaneAngle + spin;

	Z = pgm_read_byte(&scaleTable[objData->z]);							// Get the width of the lane in pixels at the current Z height
	laneWidth = LANE_WIDTH_PIXELS;
	MulSU1(Z, laneWidth);

	//todo: inline this stuff with SinMulFastC in flipper/fuseball/claw/droid

	xAdd = SinMulFastC((thisLaneAngle + edgeAngle), laneWidth);
	yAdd = CosMulFastC((thisLaneAngle + edgeAngle), laneWidth);			// Get the offset to the edge of the lane

	objData->x += xAdd;													// Add the offset to the centroid of flipper found in moveXYCommon()
	objData->y += yAdd;

	xAdd = SinMulFastC((objData->r + (edgeAngle^0x80)), laneWidth);		// Get the offset from the edge of lane to the centroid of rotated flipper
	yAdd = CosMulFastC((objData->r + (edgeAngle^0x80)), laneWidth);

	objData->x += xAdd;													// add the offest
	objData->y += yAdd;

}

//*/

void movetanker(ObjectDescStruct *objData){

	if (ACTION_TIMER_FLAG){
		if(objData->actionTimer7 == 0) {
			newZap(objData);
			facttanker(objData);
		} else {
			objData->actionTimer7--;
		}
	}

	if (objData->z < jWebMax) {
		objData->z += TANKER_SPEED;
		moveXYCommon(objData);
	} else {

	breakTanker(objData);
	}
}
void movespiker(ObjectDescStruct *objData){

	if (objData->z > (jWebMax - Z_SPIKER_MAX_OFFSET)){
		objData->obType = OBJ_SPIKER_REV;
	}
	objData->z += SPIKER_SPEED;
	objData->r -= SPIKER_SPIN;

	if (ACTION_TIMER_FLAG){
		if(objData->actionTimer7 == 0) {
			if (objData->action1 == 0) {
				newZap(objData);
			} else {
				objData->obType = OBJ_SPIKER_REV;
			}
			factspiker(objData);
		} else {
			objData->actionTimer7--;
		}
	}

	//*
	uint8_t spikeLane;
	uint8_t spikeObj;

	spikeLane = objData->laneHi;
	spikeObj = laneData[spikeLane].spikeObj;

	if(spikeObj == (MAX_OBJS + 1)) {
		laneData[spikeLane].spikeObj = newObject(OBJ_SPIKE,spikeLane,objData->z);  // no need to check if out of memory as returns EMPTY on fail.
	} else {
		if (objData->z > ObjectStore[spikeObj].z) {
			ObjectStore[spikeObj].z = objData->z - SPIKER_TO_SPIKE_Z_OFFSET;
		}
	}
	moveXYCommon(objData);
}
void movespikerrev(ObjectDescStruct *objData){

	if (objData->z < jWebMin) {
		objData->obType = OBJ_SPIKER;
		if(LANE_CLOSED_FLAG) {
			objData->laneHi = rand()%15;
		} else {
			objData->laneHi = rand()%16;
		}
	} else {
		objData->z -= SPIKER_SPEED;
		objData->r -= SPIKER_SPIN;

		if (ACTION_TIMER_FLAG){
			if(objData->actionTimer7 == 0) {
				newZap(objData);
				factspikerrev(objData);
			} else {
				objData->actionTimer7--;
			}
		}
	}
	moveXYCommon(objData);
}
void movespike(ObjectDescStruct *objData){
	uint8_t Z;
	uint8_t Z2;
	uint8_t S;
	uint16_t lane;

	int8_t x;
	int8_t y;
	int8_t xAdd;
	int8_t yAdd;

	lane = objData->laneHi;

	x = laneData[lane].xStart;
	y = laneData[lane].yStart;

	if (ZOOMING_FLAG){
		if (objData->z > (jWebMax + Z_SPIKER_MAX_OFFSET)){
				objData->obType = OBJ_EMPTY;
			}
	} else {
		if (objData->z > (jWebMax - Z_SPIKER_MAX_OFFSET)){
				objData->z = (jWebMax - Z_SPIKER_MAX_OFFSET);
			}
	}

	Z  = pgm_read_byte(&zTable[objData->z]);

	xAdd = laneData[lane].xGrad;
	yAdd = laneData[lane].yGrad;
	//xAdd = MulSU(Z, xAdd);
	//yAdd = MulSU(Z, yAdd);
	MulSU2(Z, xAdd, yAdd);
	objData->x = x + xAdd;
	objData->y = y + yAdd;

	Z = pgm_read_byte(&scaleTable[objData->z]);
	S = SPIKE_SIZE;
	MulSU1(Z, S);
	Z2 = objData->z - S;
	if (Z2 < jWebMin) { Z2 = jWebMin; }
	Z2  = pgm_read_byte(&zTable[Z2]);

	xAdd = laneData[lane].xGrad;
	yAdd = laneData[lane].yGrad;
	//xAdd = MulSU(Z2, xAdd);
	//yAdd = MulSU(Z2, yAdd);
	MulSU2(Z2, xAdd, yAdd);
	objData->x1 = x + xAdd;
	objData->y1 = y + yAdd;
}
void movefuseball(ObjectDescStruct *objData){

#define LR_NO 0
#define LR_LEFT 1
#define LR_RIGHT 2

	uint8_t thisLaneAngle, Z, laneWidth, offset, leftRight;
	int8_t xAdd, yAdd;

	objData->actionTimer5--;

	//objData->action3 = 1;
	//objData->z = 240;

	if(objData->actionTimer5 == 0) {
		objData->actionTimer5 = rand()&(PROBABILITY_FUSEBALL_DIR_CHANGE_TIME-1);

		if((rand()&0x01) == 1) {
			objData->action3++;
		} else {
			objData->action3--;
		}
	}

	leftRight = LR_NO;

	switch (objData->action3) {
	case 0 : {
			objData->z += FUSEBALL_SPEED_Z;
			break;
		}
	case 1 : {
			objData->z += (FUSEBALL_SPEED_Z / 2);
			objData->r += (FUSEBALL_SPEED_X / 2);
			if (objData->r >= (255 - FUSEBALL_SPEED_X)) {
				leftRight = LR_LEFT;
			}
			break;
		}
	case 2 : {
			objData->r += FUSEBALL_SPEED_X;
			if (objData->r >= (255 - FUSEBALL_SPEED_X)) {
				leftRight = LR_LEFT;
			}
			break;
		}
	case 3 : {
			objData->z -= (FUSEBALL_SPEED_Z / 2);
			objData->r += (FUSEBALL_SPEED_X / 2);
			if (objData->r >= (255 - FUSEBALL_SPEED_X)) {
				leftRight = LR_LEFT;
			}
			break;
		}
	case 4 : {
			objData->z -= FUSEBALL_SPEED_Z;
			break;
		}
	case 5 : {
			objData->z -= (FUSEBALL_SPEED_Z / 2);
			objData->r -= (FUSEBALL_SPEED_X / 2);
			if (objData->r <= (FUSEBALL_SPEED_X)) {
				leftRight = LR_RIGHT;
			}
			break;
		}
	case 6 : {
			objData->r -= FUSEBALL_SPEED_X;
			if (objData->r <= (FUSEBALL_SPEED_X)) {
				leftRight = LR_RIGHT;
			}
			break;
		}
	default : {
			objData->z += (FUSEBALL_SPEED_Z / 2);
			objData->r -= (FUSEBALL_SPEED_X / 2);
			if (objData->r <= (FUSEBALL_SPEED_X)) {
				leftRight = LR_RIGHT;
			}
			break;
		}
	}

	if (objData->z > jWebMax) {
		objData->z = jWebMax;
	}
	if (objData->z < jWebMin) {
		objData->z = jWebMin;
	}

	moveXYCommon(objData);

	Z = pgm_read_byte(&scaleTable[objData->z]);					// Get the width of the lane in pixels at the current Z height
	laneWidth = (FUSEBALL_LANE_WIDTH*2);
	MulSU1(Z, laneWidth);

	offset = objData->r;

	if (offset > 127){
		offset -= 128;
		thisLaneAngle = 128;
	} else {
		offset = -offset - 128;
		thisLaneAngle = 0;
	}

	MulSU1(offset, laneWidth);

	thisLaneAngle += pgm_read_byte(&laneAngles[webNum].lane[objData->laneHi]);

	xAdd = SinMulFastC((thisLaneAngle), laneWidth);
	yAdd = CosMulFastC((thisLaneAngle), laneWidth);			// Get the offset to the edge of the lane

	objData->x += xAdd;											// Add the offset to the centroid of flipper found in moveXYCommon()
	objData->y += yAdd;

	if (leftRight == LR_LEFT) {
		if((LANE_CLOSED_FLAG) && (objData->laneHi == 14)) {
			objData->action3 += 4;
		} else {
			objData->r = FUSEBALL_SPEED_X + 1;
			objData->laneHi++;
		}
	}
	if (leftRight == LR_RIGHT) {
		if((LANE_CLOSED_FLAG) && (objData->laneHi == 0)) {
			objData->action3 += 4;
		} else {
			objData->r = 255 - FUSEBALL_SPEED_X - 1;
			objData->laneHi--;
		}
	}


}
void movepulsar(ObjectDescStruct *objData){
	objData->z += PULSAR_SPEED;
	objData->actionTimer6++;

	switch (objData->actionTimer6) {
		case (PULSAR_STEP_1) : {
			T2k_TriggerFx(SFX_PULSAR_APPROACH,0,0);
			objData->action2 = 0b01;
			break;
		}
		case (PULSAR_STEP_2) : {
			T2k_TriggerFx(SFX_PULSAR_APPROACH,0,0);
			objData->action2 = 0b10;
			break;
		}
		case (PULSAR_STEP_3) : {
			objData->action2 = 0b11;
			if ((CLAW_OBJECT.laneHi == objData->laneHi) && (CLAW_OBJECT.obType == 0xFF)){ //OBJ_CLAW)) {
				CLAW_OBJECT.obType = OBJ_CLAWDEATH;
				T2k_TriggerFx(SFX_CLAW_EXPLODE,0,0);
				CLAW_OBJECT.s = 1;
				setAliveStateBFlag();
			}
			break;
		}
		case (PULSAR_STEP_4) : {
			objData->action2 = 0b00;
			break;
		}
	}
	if (objData->z > jWebMax) {
		objData->z = jWebMin;
		objData->animation = 0;
	}

	moveXYCommon(objData);
}
void movemirror(ObjectDescStruct *objData){
	objData->z += MIRROR_SPEED;

	objData->actionTimer7++;

	switch (objData->actionTimer7) {
		case (MIRROR_STEP_1) : {
			objData->r += 11;
			break;
		}
		case (MIRROR_STEP_2) : {
			objData->r += 11;
			break;
		}
		case (MIRROR_STEP_3) : {
			objData->action1 = 1;
			objData->r += 11;
			break;
		}
		case (MIRROR_STEP_4) : {
			objData->action1 = 0;
			objData->r += 11;
			break;
		}
		case (MIRROR_STEP_5) : {
			objData->r += 11;
			break;
		}
		case (MIRROR_STEP_6) : {
			objData->r += 11;
			break;
		}
	}
	if (objData->z > jWebMax) {
		objData->z = jWebMin;
		objData->animation = 0;
	}

	moveXYCommon(objData);
}
void movedemona(ObjectDescStruct *objData){
	ObjectDescStruct *ob2;

	objData->z += DEMON_A_SPEED;
	if(objData->action1 == 1){
		ob2 = (ObjectDescStruct*)&ObjectStore[objData->actionTimer7];
		ob2->z = objData->z;
		ob2->r = objData->r;
	} else if (objData->actionTimer7 != 0) {// is this something to do with one bullet not killing both A and B ??
		objData->actionTimer7--;
	}
	moveXYCommon(objData);

	if (objData->z > jWebMax) {
		objData->z = jWebMax;
		objData->r += DEMON_B_SPIN;
	}

}
void movedemonb(ObjectDescStruct *objData){
	if(objData->animation == 0){
		objData->r += DEMON_B_SPIN;
		objData->z += DEMON_B_SPEED;
		if (objData->z > jWebMax) {
			objData->obType = OBJ_EMPTY;
			activeEnemy--;
		}

	}
	moveXYCommon(objData);
}
void movepowerup(ObjectDescStruct *objData){
	objData->r += POWER_UP_SPIN;
	moveXYCommon(objData);
	objData->actionTimer5--;
	if(objData->actionTimer5 == 0) {
		objData->actionTimer5 = POWERUP_SHADOW_TIME;
		objData->action3--;
		if(objData->action3 == 0) {
			objData->obType = OBJ_POWERUP_B;
			objData->animation = 1;
		} else if (objData->action3 < 4){
			newObject(OBJ_POWERUP_B,objData->laneHi,objData->z);
		}
	}
}
void movepowerupb(ObjectDescStruct *objData){
	objData->z += POWER_UP_SPEED;					// Before Z Check to stop flashing at end of web when rolled over
	objData->r += POWER_UP_SPIN;
	if(objData->z < Z_POWERUP_MAX_ROLLED_OVER) {
		objData->obType = OBJ_EMPTY;
		if (objData->animation == 1){
			clearCollectorBonusFlag();
		}
	} else {
		moveXYCommon(objData);
	}
}
void moveexplosion(ObjectDescStruct *objData){
	uint8_t temp;
	temp = objData->s;
	if (temp > EXPLOSION_MAX_SCALE) {
		objData->obType = OBJ_EMPTY;
	} else {
		moveXYCommon(objData);
		objData->s = temp + EXPLOSION_SCALE;
		objData->r += EXPLOSION_SPIN;
	}
}
void moveoneup(ObjectDescStruct *objData){
	objData->s += ONE_UP_SCALE;
	if (objData->s > ONE_UP_MAX) {
		objData->obType = OBJ_EMPTY;
	}
}
void moveyesyesyes(ObjectDescStruct *objData){
	objData->s += YES_YES_YES_SCALE;
	if (objData->s > YES_YES_YES_MAX) {
		objData->obType = OBJ_EMPTY;
	}
}
void moveclawdeath(ObjectDescStruct *objData){
	uint8_t temp;
	temp = objData->s;
	moveXYCommon(objData);
	if(objData->animation == 1){
		if (temp > CLAW_DEATH_MAX_SCALE) {
			objData->animation = 0;
		}
		temp += CLAW_DEATH_SCALE;
	} else {
		if (temp < CLAW_DEATH_MIN_SCALE) {
			objData->obType = OBJ_EMPTY;
			setAliveStateAFlag();
		}
		temp -= CLAW_DEATH_SCALE;
	}
	objData->s = temp;
	objData->r += CLAW_DEATH_SPIN;
}
void moveclawcaught(ObjectDescStruct *objData){

	if(objData->z < jWebMin) {
		if (objData->action1 == 0) {
			ObjectStore[objData->animation].obType = OBJ_EMPTY;
			objData->action1 = 1;
			objData->actionTimer7 = CLAW_CAUGHT_DWELL_TIME;
		} else {
			objData->actionTimer7--;
			if(objData->actionTimer7 == 0){
				objData->obType = OBJ_EMPTY;
				setAliveStateAFlag();
			}
		}
	} else {
		objData->z -= CLAW_CAUGHT_SPEED;
		moveXYCommon(objData);
		ObjectStore[objData->animation].z = objData->z;
		ObjectStore[objData->animation].lane = objData->lane;
	}
}
void movetestmarker(ObjectDescStruct *objData){
	if(objData->animation == 0) {
		objData->obType = OBJ_EMPTY;
	} else {
		objData->animation--;
	}
}
void movetestline(ObjectDescStruct *objData){
	if(objData->animation == 0) {
		objData->obType = OBJ_EMPTY;
	} else {
		objData->animation--;
	}
}
void movedestroyed(ObjectDescStruct *objData){
	objData->obType = OBJ_EMPTY;
}
void moveclaw(ObjectDescStruct *objData){

	uint8_t r;
	uint8_t offset;
	uint8_t rotate;

	objData->r = pgm_read_byte(&laneAngles[webNum].lane[objData->laneHi]);
	r = objData->r;

	moveXYCommon(objData);

	switch ((objData->lane & 0b00001100)) {
	case 0b00000000 : {
		offset = CLAW_SHUFFLE_OFFSET_2;
		rotate = CLAW_SHUFFLE_ROTATE_2;
		break;
	}
	case 0b00000100 : {
		offset = CLAW_SHUFFLE_OFFSET_1;
		rotate = CLAW_SHUFFLE_ROTATE_1;
		break;
	}
	case 0b00001000 : {
		offset = CLAW_SHUFFLE_OFFSET_1;
		rotate = -CLAW_SHUFFLE_ROTATE_1;
		r -= 128;
		break;
	}
	default : { // case 0b00001100
		offset = CLAW_SHUFFLE_OFFSET_2;
		rotate = -CLAW_SHUFFLE_ROTATE_2;
		r -= 128;
		break;
	}
	}
	objData->x += SinMulFastC(r, offset);
	objData->y += CosMulFastC(r, offset);
	objData->r += rotate;
}

void movedroid(ObjectDescStruct *objData){

	if (ZOOMING_FLAG){
		objData->obType = OBJ_EMPTY;
	} else {
		uint8_t highestLane;
		uint8_t r;
		uint8_t offset;

		highestLane = getHighestLane();
		laneData[objData->laneHi].highestEnemy = 0;

		if (ACTION_TIMER_FLAG){
			objData->action1 = closestLaneDirection(highestLane, objData->lane);
		}

		objData->actionTimer7--;
		if(objData->actionTimer7 == 0) {
			objData->actionTimer7 = DROID_FIRE_FREQ;
			fire(objData);
		}

		if(objData->action1 == 0) {
			objData->r += DROID_SPIN;
			objData->lane += DROID_SPEED;
		} else {
			objData->r -= DROID_SPIN;
			objData->lane -= DROID_SPEED;
		}
		moveXYCommon(objData);

		r = pgm_read_byte(&laneAngles[webNum].lane[objData->laneHi]);

		switch ((objData->lane & 0b00001100)) {
		case 0b00000000 : {
			offset = DRIOD_SHUFFLE_OFFSET_2;
			break;
		}
		case 0b00000100 : {
			offset = DRIOD_SHUFFLE_OFFSET_1;
			break;
		}
		case 0b00001000 : {
			r -= 128;
			offset = DRIOD_SHUFFLE_OFFSET_1;
			break;
		}
		default : { // case 0b00001100
			r -= 128;
			offset = DRIOD_SHUFFLE_OFFSET_2;
			break;
		}
		}
		objData->x += SinMulFastC(r, offset);
		objData->y += CosMulFastC(r, offset);
	}

}

void fire(ObjectDescStruct *ob1){
	if(LASER_POWER_UP) {
		newObject(OBJ_LASER, ob1->laneHi, ob1->z);
		T2k_TriggerFx(SFX_LASER,0,0);
	} else {
		newObject(OBJ_BULLET, ob1->laneHi, ob1->z);
		T2k_TriggerFx(SFX_BULLET,0,0);
	}
}

