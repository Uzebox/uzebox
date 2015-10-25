/*
 * VectorDemo.h
 *
 *  Created on: 23/05/2015
 *      Author: Cunning Fellow
 */

#ifndef T2K_H_
#define T2K_H_


#define SFX_FIRST_EFFECT 16
#define SFX_DUMMY			0
#define SFX_BULLET			1
#define SFX_BULLET2			2//do not call directly
#define SFX_LASER			3
#define SFX_LASER2			4
#define SFX_POWERUP			5
#define SFX_POWERUP2			6//do not call directly
#define SFX_SUPER_ZAP			7
#define SFX_SUPER_ZAP2		8//do not call directly
#define SFX_CLAW_CAUGHT		9
#define SFX_CLAW_CAUGHT2		10//do not call directly
#define SFX_CLAW_EXPLODE		11
#define SFX_CLAW_EXPLODE2		12//do not call directly
#define SFX_CLAW_JUMP			13
#define SFX_WEB_ZOOM			14
#define SFX_WEB_ZOOM2			15//do not call directly

#define SFX_ENEMY_ZAP			16
#define SFX_ENEMY_EXPLODE		17
#define SFX_FLIPPER_FLIP		18
#define SFX_WEB_CLICK			19
#define SFX_TANKER_BREAK		20
#define SFX_PULSAR_APPROACH	21
#define SFX_SPIKE_SHOT		22




#define VIEW_ANGLES_PER_WEB 64
#define SECTORS_PER_WEB_BMP 13
#define LENGTH_OF_INTRO_MOVIES 480
#define LENGTH_OF_ATTRACT_MODE 480
#define MESSAGE_COUNTER_MAX  60 			// (60 ~~ 2 seconds)
#define MESSAGE_DEFAULT_POSITION 10
#define MESSAGE_IN_STATUS_BAR_LENGTH 14
#define	GAME_OVER_MESSAGE_DURATION 120
#define	BONUS_SCORE_SCREEN_DURATION 120

#define TORNADO_EEPROM_ID_1 135
#define TORNADO_EEPROM_ID_2 136

void T2k_TriggerFx(uint8_t effect, uint8_t volume, uint8_t note);

void attractModeAI(void);
void attractMode(void);
void playGame(void);
void ProcessInput(void);

extern uint8_t vram[];
extern uint8_t ramTiles[];

extern uint8_t  level;

#define webNum  (level & 0x0F)

extern uint8_t  lane;
extern uint8_t  zoom;
extern uint8_t  jWebMin;
extern uint8_t  jWebMax;
extern uint8_t  jumpCount;

extern uint8_t  lives;
extern uint8_t  score[];
extern uint8_t  hiScore[];
extern uint8_t  bonusLifeScore;
extern uint8_t  dominationBonus;
extern uint8_t  messageCounter;
extern uint8_t  activeEnemy;
extern uint8_t  lvlGenCount;
extern uint8_t  lvlSpawnCount;

extern long sectorStart;
extern volatile long sectorBMP;

extern uint8_t actionTimerSubCount;

#endif /* T2K_H_ */
