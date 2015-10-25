/*
 * fonts.h
 *
 *  Created on: 11/06/2014
 *      Author: Cunning Fellow
 */

#ifndef FONTS_H_
#define FONTS_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

uint8_t printText(uint8_t msgNo, uint16_t pos);
void message(uint8_t msgNo, uint16_t pos);
void showRamTileCounts(void);
void readSDBytes(uint8_t *dest, uint16_t count, long startAddress);
void blitFont(void);

#define FONT_TEXT_NUM_TILES 0x3F

#define MSG_CLEAR			0
#define MSG_PARTICLE_LASER	1
#define MSG_JUMP_ENABLED	2
#define MSG_AI_DROID		3
#define MSG_EXCELLENT		4
#define MSG_ZAPPO			5
#define MSG_OUTTA_HERE		6
#define MSG_GAME_OVER		7
#define MSG_PAUSED			8
#define MSG_SEARCHING_SD	9
#define MSG_EEPROM_ERROR	10
#define MSG_TOP_ZAPPERS		11
#define MSG_ENTER_INITIALS	12
#define MSG_DOMINATION		13
#define MSG_COLLECTOR		14
#define MSG_ZAPPER			15
#define MSG_TOTAL			16
#define MSG_BONUS			17

#define TEXT_MODE_XY(X,Y) (X+(Y*32))

#endif /* FONTS_H_ */
