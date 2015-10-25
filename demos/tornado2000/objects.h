#ifndef __OBJECTS_H__
#define __OBJECTS_H__

#include <avr/io.h>
#include <stdlib.h>
#include <stddef.h>
#include "housekeeping.h"
#include <avr/pgmspace.h>

typedef struct {
	union {
		unsigned int Count:4;
		unsigned int Num:4;
	};
} powerUpStruct ;
extern powerUpStruct powerUp;


typedef struct {
	uint8_t  obType;
	union {
		uint8_t  lane;
		struct {
			unsigned int laneLo:4;
			unsigned int laneHi:4;
		};
	};
	uint8_t  z;
	uint8_t  x;
	uint8_t  y;
	union {
		uint8_t  r;
		int8_t  x1;
	};
	union {
		uint8_t  s;
		int8_t  y1;
	};
	union {
		uint8_t  animation;
		struct {
			int xStep:4;
			int yStep:4;
		};
		struct {
			unsigned int actionTimer7:7;
			unsigned int action1:1;
		};
		struct {
			unsigned int actionTimer6:6;
			unsigned int action2:2;
		};
		struct {
			unsigned int actionTimer5:5;
			unsigned int action3:3;
		};
		struct {
			unsigned int actionTimer4:4;
			unsigned int action4:4;
		};
	};
} ObjectDescStruct;

extern ObjectDescStruct ObjectStore[];

#define CLAW_OBJECT ObjectStore[0]				// CLAW is always object <ZERO> if it exists.
#define OBJ_STORE_ADDRESS 0x220					// Must be changed if .section() of object store changed
#define GET_OBJECT_NUMBER(objectPointer) (((uint8_t)((uint8_t)((uint16_t)(objectPointer) / 4)) / 2) - (uint8_t)((uint16_t)(OBJ_STORE_ADDRESS) / 8))

#define MAX_OBJS 60
#define OBJ_LEN 8

#define COLLISION_CLOSE_Z_RANGE 15				// Maximum absolute distance between two objects before doing a collision test

#define ACTION_TIMER_SUB_COUNT_MAX 4

#define LANE_WIDTH_PIXELS 60

#define NOT_JUMPING 26

#define ZOOM_LEVEL_MAX    63
#define ZOOM_LEVEL_NORMAL 32


#define Z_WEB_DOM_BONUS_PLUS2	128
#define Z_WEB_DOM_BONUS_PLUS1	160
#define Z_WEB_DOM_BONUS_MINUS1	222

#define Z_WEB_ABS_MIN 64
#define Z_WEB_SPAWN_OFFSET 0
#define Z_WEB_ABS_MAX 224
#define Z_SPIKER_MAX_OFFSET 20
#define Z_POWERUP_MAX_ROLLED_OVER (0 + 1 + (POWER_UP_SPIN * 2))

#define BULLET_SPEED 8
#define BULLET_SPEED_ZOOMING 23
#define LASER_SPEED 8
#define LASER_SPEED_ZOOMING 23
#define LASER_SPIN 10
#define SUPER_ZAP_HID_TIME 30
#define SUPER_ZAP_TIME 5

#define ZAP_SPEED 10
#define ZAP_SPIN 10

#define FLIPPER_SPEED 4
#define FLIPPER_SPIN 5
#define FLIPPER_SHOOTABLE_AB 56					// range of angle that a flipping flipper can bet shot at
#define FLIPPER_SHOOTABLE_CD 56
#define FLIPPER_GRABABLE 8						// angle that a flipper can grab the claw

#define PULSAR_SPEED 4
#define PULSAR_STEP_1 24
#define PULSAR_STEP_2 28
#define PULSAR_STEP_3 32
#define PULSAR_STEP_4 52

#define MIRROR_SPEED 4
#define MIRROR_STEP_1 24
#define MIRROR_STEP_2 25
#define MIRROR_STEP_3 26
#define MIRROR_STEP_4 36
#define MIRROR_STEP_5 37
#define MIRROR_STEP_6 38

#define DEMON_A_SPEED 4
#define DEMON_A_INVINCIBLE 5					// after being shot once and breaking away from DEMON_B the DEMON_A must become
#define DEMON_B_SPEED 8							//     invincible for X many frames
#define DEMON_B_SPIN 15


#define SPIKER_SPEED 4
#define SPIKER_SPIN 5
#define SPIKER_TO_SPIKE_Z_OFFSET 3				// spike must stay below a spiker or else a bullet may hit spike before spiker
#define SPIKE_HIT_BULLET 5						// amount a spike shrinks when shot
#define SPIKE_HIT_LASER 15
#define SPIKE_SIZE 80							// length of green line representing spike

#define TANKER_SPEED 4

#define CLAW_CAUGHT_SPEED 8
#define CLAW_CAUGHT_DWELL_TIME 20
#define CLAW_FIXED_Z (Z_WEB_ABS_MAX + 5)
#define CLAW_DEATH_SPIN 8
#define CLAW_DEATH_SCALE 6
#define CLAW_DEATH_MAX_SCALE 100
#define CLAW_DEATH_MIN_SCALE 30
#define CLAW_DEMO_FIRE_FREQ 4
#define CLAW_DEMO_MOVE_SPEED 7

#define EXPLOSION_SPIN 4
#define EXPLOSION_SCALE 6
#define EXPLOSION_MAX_SCALE 50

#define YES_YES_YES_SCALE 3
#define YES_YES_YES_MAX 96

#define ONE_UP_SCALE 24
#define ONE_UP_MAX 192

#define DROID_SPIN 5
#define DROID_SPEED 3
#define DROID_FIRE_FREQ 4
#define DRIOD_SHUFFLE_OFFSET_1 12
#define DRIOD_SHUFFLE_OFFSET_2 24

#define CLAW_SHUFFLE_OFFSET_1 5
#define CLAW_SHUFFLE_ROTATE_1 10
#define CLAW_SHUFFLE_OFFSET_2 10
#define CLAW_SHUFFLE_ROTATE_2 20

#define PROBABILITY_ZAP 16

#define PROBABILITY_SPIKER_DIR_ZAP_SPLIT 64			// Lower Numbers = more zap less direction changes
#define PROBABILITY_SPIKER_DIRECTION 16			

#define PROBABILITY_FLIPPER_FLIP_ZAP_SPLIT 64		// Lower Numbers = more zap less direction changes
#define PROBABILITY_FLIPPER_FLIP 16

#define PROBABILITY_FLIPPER_RANDOM_DIRECTION 16		// Chance a flipper will flip in a non-optimal direction
													// (This excludes breaking tankers - they always flip AWAY)

#define PROBABILITY_FUSEBALL_DIR_CHANGE_TIME 32


#define POWER_UP_SPEED 8
#define POWER_UP_SPIN 5

#define POWERUP_SHADOW_COUNT 5
#define POWERUP_DWELL_TIME 6
#define POWERUP_SHADOW_TIME 1

#define POWERUP_FREQUENCY 5

#define POWERUP_PARTICLE_LASER	0x01
#define POWERUP_ZAPPO_2000		0x02
#define POWERUP_JUMP			0x03
#define POWERUP_AI_DROID		0x04
#define POWERUP_WARP_TOKEN		0x05
#define POWERUP_OUTTA_HERE		0x06

#define FUSEBALL_INITIAL_DIRECTION 0

#define FUSEBALL_SPEED_Z 4
#define FUSEBALL_SPEED_X 8
#define FUSEBALL_SPIN 5
#define FUSEBALL_SHOOTABLE_MIN (128-40)
#define FUSEBALL_SHOOTABLE_MAX (128+40)
#define FUSEBALL_LANE_WIDTH (LANE_WIDTH_PIXELS + 3)

#define COLOUR_YELLOW  0b00000000
#define COLOUR_RED     0b01010101
#define COLOUR_GREEN   0b10101010
#define COLOUR_CLEAR   0b11111111


#define PNT_REFBULLET	0x0005
#define PNT_REFLASER	0x0005
#define PNT_ZAP			0x0010
#define PNT_FLIPPER		0x0050
#define PNT_TANKER		0x0000
#define PNT_SPIKER		0x0150
#define PNT_SPIKE		0x0001
#define PNT_FUSEBALL	0x0200
#define PNT_PULSAR		0x0250
#define PNT_MIRROR		0x0100
#define PNT_MUTFLIPPER	0x0100
#define PNT_DEMONA		0x0250
#define PNT_DEMONB		0x0500
#define PNT_UFO			0x0750



#define OBJ_COLL_MIN	0x01

#define OBJ_EMPTY     	0x00
#define OBJ_CLAW		0x01
#define OBJ_BULLET		0x02
#define OBJ_LASER		0x03

#define OBJ_BULLET_EX	0x04
#define OBJ_LASER_EX	0x05
#define OBJ_SUPER_ZAP	0x06

#define OBJ_COLL_MID	0x06

#define OBJ_REFBULLET	0x07
#define OBJ_REFLASER	0x08
#define OBJ_ZAP			0x09

#define OBJ_MIN_ENEMY	0x0A

#define OBJ_NONFLIPPER  0x0A
#define OBJ_TANKER		0x0B
#define OBJ_SPIKER		0x0C
#define OBJ_FUSEBALL	0x0D
#define OBJ_PULSAR		0x0E
#define OBJ_MIRROR		0x0F
#define OBJ_MUTFLIPPER	0x10
#define OBJ_DEMONA		0x11

#define TOP_ENEMY       7

#define OBJ_DEMONB		0x12
#define OBJ_UFO			0x13

#define OBJ_SPIKER_REV	0x14
#define OBJ_SPIKE		0x15

#define OBJ_FLIPPER_A	0x16
#define OBJ_FLIPPER_B	0x17
#define OBJ_FLIPPER_C	0x18
#define OBJ_FLIPPER_D	0x19

#define OBJ_MAX_ENEMY	0x19

#define OBJ_POWERUP 	0x1A
#define OBJ_POWERUP_B 	0x1B

#define OBJ_COLL_MAX	0x1B



#define OBJ_EXPLOSION	0x1C
#define OBJ_ONEUP		0x1D
#define OBJ_GAME_OVER	0x1E
#define OBJ_YESYESYES	0x1F
#define OBJ_CLAWDEATH	0x20
#define OBJ_CLAWCAUGHT	0x21
#define OBJ_DROID		0x22
#define OBJ_TEST_MARKER 0x23
#define OBJ_TEST_LINE   0x24

#define OBJ_SUPER_HID	0x25
#define OBJ_DESTROYED	0x26

#define OBJ_TYPE_MAX	0x26

#define OBJ_PARTICLE	0xFF

typedef void (*drawFunctionPointer_t)(ObjectDescStruct *objData);
typedef uint16_t (*collisionFunctionPointer_t)(ObjectDescStruct *Ob1Data, ObjectDescStruct *Ob2Data);

void drawclaw(ObjectDescStruct *objData);
void drawdroid(ObjectDescStruct *objData);

void drawbullet(ObjectDescStruct *objData);
void drawlaser(ObjectDescStruct *objData);

void drawrefbullet(ObjectDescStruct *objData);
void drawreflaser(ObjectDescStruct *objData);
void drawzap(ObjectDescStruct *objData);


void drawflipper(ObjectDescStruct *objData);
void drawtanker(ObjectDescStruct *objData);
void drawspiker(ObjectDescStruct *objData);
void drawspike(ObjectDescStruct *objData);
void drawfuseball(ObjectDescStruct *objData);
void drawpulsar(ObjectDescStruct *objData);
void drawmirror(ObjectDescStruct *objData);
void drawmutflipper(ObjectDescStruct *objData);
void drawdemona(ObjectDescStruct *objData);
void drawdemonb(ObjectDescStruct *objData);

void drawexplosion(ObjectDescStruct *objData);
void drawpowerup(ObjectDescStruct *objData);
void drawpowerupb(ObjectDescStruct *objData);
void drawclaw(ObjectDescStruct *objData);
void drawdroid(ObjectDescStruct *objData);
void drawclawdeath(ObjectDescStruct *objData);

void drawyesyesyes(ObjectDescStruct *objData);
void drawoneup(ObjectDescStruct *objData);


void drawtestmarker(ObjectDescStruct *objData);
void drawtestline(ObjectDescStruct *objData);
void drawsuperzap(ObjectDescStruct *objData);
void drawblank(ObjectDescStruct *objData);


void movebullet(ObjectDescStruct *objData);
void movelaser(ObjectDescStruct *objData);
void movebulletex(ObjectDescStruct *objData);
void movelaserex(ObjectDescStruct *objData);
void movesuperzap(ObjectDescStruct *objData);
void movesuperhid(ObjectDescStruct *objData);
void moverefbullet(ObjectDescStruct *objData);
void movereflaser(ObjectDescStruct *objData);
void movezap(ObjectDescStruct *objData);
void movenonflipper(ObjectDescStruct *objData);
void moveflipper(ObjectDescStruct *objData);
void movetanker(ObjectDescStruct *objData);
void movespiker(ObjectDescStruct *objData);
void movespikerrev(ObjectDescStruct *objData);
void movespike(ObjectDescStruct *objData);
void movefuseball(ObjectDescStruct *objData);
void movepulsar(ObjectDescStruct *objData);
void movemirror(ObjectDescStruct *objData);
void movepowerup(ObjectDescStruct *objData);
void movepowerupb(ObjectDescStruct *objData);
void movedemona(ObjectDescStruct *objData);
void movedemonb(ObjectDescStruct *objData);

void moveclawcaught(ObjectDescStruct *objData);
void moveclawdeath(ObjectDescStruct *objData);
void moveclaw(ObjectDescStruct *objData);
void movedroid(ObjectDescStruct *objData);
void moveexplosion(ObjectDescStruct *objData);
void moveyesyesyes(ObjectDescStruct *objData);
void moveoneup(ObjectDescStruct *objData);

void movetestmarker(ObjectDescStruct *objData);
void movetestline(ObjectDescStruct *objData);
void movedestroyed(ObjectDescStruct *objData);


void factsuperhid(ObjectDescStruct *objData);
void factclaw(ObjectDescStruct *objData);
void factspike(ObjectDescStruct *objData);
void factspiker(ObjectDescStruct *objData);
void facttanker(ObjectDescStruct *objData);
void factfuseball(ObjectDescStruct *objData);
void factpulsar(ObjectDescStruct *objData);
void factspikerrev(ObjectDescStruct *objData);
void factnonflipper(ObjectDescStruct *objData);
void factpowerup(ObjectDescStruct *objData);
void factyesyesyes(ObjectDescStruct *objData);
void factoneup(ObjectDescStruct *objData);
void factdroid(ObjectDescStruct *objData);

void factdemona(ObjectDescStruct *objData);


void clearObjectStore(void);
void clearTestMarkers(void);
uint8_t getFreeObject(void);
uint8_t newObject(uint8_t obType, uint8_t lane, uint8_t z);
void newZap(ObjectDescStruct *objData);
void newSuperZap(void);
void enableAIDroid(void);
uint8_t newTestMarker(uint8_t x, uint8_t y, uint8_t life);
uint8_t newTestLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t life);

void renderObjects(void);
void moveObjects(void);
void moveXYCommon(ObjectDescStruct *objData);
void PositiveCollision(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
void collisionDetection(void);

void breakTanker(ObjectDescStruct *ob1);
void powerUpCycle(ObjectDescStruct *ob1);

uint16_t collBulletZap(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collLaserZap(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collLaserFlipper(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collLaserFlipperAB(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collLaserFlipperCD(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collLaserTanker(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collLaserSpiker(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collLaserFuseBall(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collLaserSpike(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collBulletSpike(ObjectDescStruct *ob1, ObjectDescStruct *ob2);

uint16_t collBulletMirror(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collLaserMirror(ObjectDescStruct *ob1, ObjectDescStruct *ob2);

uint16_t collLaserDemonA(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collLaserDemonB(ObjectDescStruct *ob1, ObjectDescStruct *ob2);


uint16_t collClawShot(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collClawCaught(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collClawFlipperAB(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collClawFlipperCD(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collClawPowerUp(ObjectDescStruct *ob1, ObjectDescStruct *ob2);

uint16_t collsuperdec(ObjectDescStruct *ob1, ObjectDescStruct *ob2);
uint16_t collsuper(ObjectDescStruct *ob1, ObjectDescStruct *ob2);

uint8_t closestLaneDirection(uint8_t lane8Bit1, uint8_t lane8Bit2);
uint8_t getHighestLane(void);

void fire(ObjectDescStruct *ob1);

/*
static inline void fire(ObjectDescStruct *ob1)__attribute__((always_inline));
static inline void fire(ObjectDescStruct *ob1){
	if(LASER_POWER_UP) {
		newObject(OBJ_LASER, ob1->laneHi, ob1->z);
	} else {
		newObject(OBJ_BULLET, ob1->laneHi, ob1->z);
	}
}
*/

#endif
