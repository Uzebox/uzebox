/*
 * highscores.c
 *
 *  Created on: 23/05/2015
 *      Author: Cunning Fellow
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <kernel.h>

#include "fonts.h"
#include "highscores.h"
#include "objects.h"
#include "t2k.h"

#define ASCII_CONV(a) (a - 54)

const EEPROMDataStruct defaultEEPROM[] __attribute__ ((section (".freeflash"))) = {
		{{ASCII_CONV('S'), ASCII_CONV('N'), ASCII_CONV('S'), 100000}},
		{{ASCII_CONV('Y'), ASCII_CONV('O'), ASCII_CONV('G'), 50000}},
		{{ASCII_CONV('T'), ASCII_CONV('L'), ASCII_CONV('O'), 20000}},
		{{ASCII_CONV('A'), ASCII_CONV('J'), ASCII_CONV('M'), 10000}},
		{{ASCII_CONV('C'),            0x00, ASCII_CONV('F'), 5000}},
		{{ASCII_CONV('Y'), ASCII_CONV('I'), ASCII_CONV('F'), 2000}},
		{{ASCII_CONV('U'), ASCII_CONV('Z'), ASCII_CONV('E'), 1000}},
		{{ASCII_CONV('D'), ASCII_CONV('A'), ASCII_CONV('V'), 500}},
		{{ASCII_CONV('L'), ASCII_CONV('E'), ASCII_CONV('E'), 200}},
		{{ASCII_CONV('K'), ASCII_CONV('H'), ASCII_CONV('M'), 100}},
};

void showMovie(uint8_t movieNum){
	uint16_t i;

	for(i=0; i < LENGTH_OF_INTRO_MOVIES; i++) {
		sectorBMP = (i * SECTORS_PER_WEB_BMP) + sectorStart + (LENGTH_OF_INTRO_MOVIES * SECTORS_PER_WEB_BMP) * movieNum;
		waitNFrames(2);
		if(checkStartPressed()) {
			lives = 3;
			i = LENGTH_OF_INTRO_MOVIES;
		}
	}
}

void showTitle(void){
//	ClearBuffer();

	readSDBytes(&ramTiles[0], (RAMTILE_SIZE + VRAM_SIZE), SD_FILE_START_OF_TITLE_DATA);

	SetHsyncCallback(&titleCallback);
	showMovie(0);
}
void showCredits(void){
	ClearBuffer();

	readSDBytes(&vram[0], VRAM_SIZE, SD_FILE_START_OF_CREDIT_DATA);

	blitFont();

	SetHsyncCallback(&creditCallback);
	showMovie(1);
}
void showHighScore(void){
	ClearBuffer();
	blitFont();

	printHighScoreTable();

	SetHsyncCallback(&creditCallback);
	showMovie(1);
}
uint8_t checkStartPressed(void){
	uint16_t buttons;

	buttons = ReadJoypad(0);

	if(buttons & BTN_START) {
		return(1);
	} else {
		return(0);
	}
}
void gameOver(void){

	uint8_t i = GAME_OVER_MESSAGE_DURATION;

	message(MSG_GAME_OVER, MESSAGE_DEFAULT_POSITION);

	while(i > 0) {
		i--;
		waitNFrames(2);
		if (checkStartPressed()) {
			i = 0;
		}
	}
	while(checkStartPressed());
}
uint8_t printBCDDigit(uint8_t digit, uint8_t *suppress){

	if ((*suppress == 0) && (digit == 0)) {
		return(0);
	}
	*suppress = 1;
	return((digit+1)<<1);
}
void printBCD(uint32_t BCD, uint16_t vramPos){
	uint8_t suppress = 0;

	vram[vramPos++] = printBCDDigit((BCD>>28 & 0x0F), &suppress);
	vram[vramPos++] = printBCDDigit((BCD>>24 & 0x0F), &suppress);
	vram[vramPos++] = printBCDDigit((BCD>>20 & 0x0F), &suppress);
	vram[vramPos++] = printBCDDigit((BCD>>16 & 0x0F), &suppress);
	vram[vramPos++] = printBCDDigit((BCD>>12 & 0x0F), &suppress);
	vram[vramPos++] = printBCDDigit((BCD>>8  & 0x0F), &suppress);
	vram[vramPos++] = printBCDDigit((BCD>>4  & 0x0F), &suppress);
	suppress = 1;
	vram[vramPos++] = printBCDDigit((BCD>>0  & 0x0F), &suppress);
}
void printHighScoreTable(void){

	EEPROMDataStruct *EEPROMData;

	EEPROMData = (EEPROMDataStruct *)&ObjectStore[8];

	int32_t scoreBCD;

	uint8_t j;

	uint16_t vramPos = TEXT_MODE_XY(11,3);


	printText(MSG_TOP_ZAPPERS, vramPos);

	readWriteEEPROM(0);

	for(j = 0; j < 10; j++){
		vramPos = TEXT_MODE_XY(7,(j*2+6));

		vram[vramPos++] = EEPROMData[j].initial1<<1;
		vram[vramPos++] = EEPROMData[j].initial2<<1;
		vram[vramPos++] = EEPROMData[j].initial3<<1;

		scoreBCD = convertScoreBCD(EEPROMData[j].score);

		vramPos += 7;

		printBCD(scoreBCD, vramPos);
	}



}
uint8_t readWriteEEPROM(bool direction){

	struct EepromBlockStruct *ebs1, *ebs2;
	uint8_t i;

	ebs1 = (struct EepromBlockStruct*)&ObjectStore[0];
	ebs2 = (struct EepromBlockStruct*)&ObjectStore[4];
	uint8_t *vp1, *vp2;
	const uint8_t *vp3;

	ebs1->id = TORNADO_EEPROM_ID_1;
	ebs2->id = TORNADO_EEPROM_ID_2;

	if(EepromReadBlock(ebs1->id, ebs1)){				//doesn't exist, try to make it

		vp1 = (uint8_t *)&ebs1->data;
		vp3 = (uint8_t *)&defaultEEPROM[0];
		for(i = 0; i < 30; i++){
			*vp1++ = pgm_read_byte(vp3++);
		}
		vp1 = (uint8_t *)&ebs2->data;
		for(i = 0; i < 30; i++){
			*vp1++ = pgm_read_byte(vp3++);
		}

		EepromWriteBlock(ebs1);							//write a new block
		EepromWriteBlock(ebs2);

	}

	//at this point, it should exist, even if it didn't before

	if((EepromReadBlock(ebs1->id, ebs1) == 0) && (EepromReadBlock(ebs2->id, ebs2) == 0)){	//it exists

		if(!direction){//read

			EepromReadBlock(ebs1->id, ebs1);
			EepromReadBlock(ebs2->id, ebs2);

			vp1 = (uint8_t *)&ObjectStore[8];
			vp2 = (uint8_t *)&ebs1->data;
			for(i=0; i<30;i++) {
				*vp1++ = *vp2++;
			}
			vp2 = (uint8_t *)&ebs2->data;
			for(i=0; i<30;i++) {
				*vp1++ = *vp2++;
			}

		}else{//write

			vp1 = (uint8_t *)&ObjectStore[8];
			vp2 = (uint8_t *)&ebs1->data;
			for(i=0; i<30;i++) {
				*vp2++ = *vp1++;
			}
			vp2 = (uint8_t *)&ebs2->data;
			for(i=0; i<30;i++) {
				*vp2++ = *vp1++;
			}

			EepromWriteBlock(ebs1);
			EepromWriteBlock(ebs2);
		}



		return(0);
	}else {
		message(MSG_EEPROM_ERROR, MESSAGE_DEFAULT_POSITION);
		return(1);
	}

	return(0);
}

uint16_t singleButtonRising(void){
	uint16_t buttons=1;

	while (buttons != 0) {
		buttons = ReadJoypad(0);
	}
	while (buttons == 0) {
		buttons = ReadJoypad(0);
	}
	return(buttons);
}

uint8_t getCharFromUser(uint16_t vramPos){

	uint8_t thisChar = ASCII_CONV('A');
	uint16_t buttons = 0;

	while (!(buttons & BTN_A)){
		if(buttons & BTN_UP) {
			thisChar++;
		}
		if(buttons & BTN_DOWN) {
			thisChar--;
		}
		thisChar &= 0x3F;
		vram[vramPos] = thisChar<<1;
		buttons = singleButtonRising();  //todo: make this typematic repeat.
	}
	return(thisChar);
}

void checkHighScores(void){
	EEPROMDataStruct *EEPROMData;

	EEPROMData = (EEPROMDataStruct *)&ObjectStore[8];

	uint32_t scoreLong;
	uint32_t *thisScore;

	thisScore = (uint32_t *)&score[0];

	scoreLong = convertScoreLong(*thisScore);

	readWriteEEPROM(0);

	uint8_t i;
	uint8_t higherThan = 10;

	for(i = 0; i < 10; i++){
		if (scoreLong > EEPROMData[i].score) {
			higherThan = i;
			i = 10;
		}
	}

	if (higherThan < 9) {

		for(i=9; i > higherThan; i--) {
			EEPROMData[i].initial1 = EEPROMData[i-1].initial1;
			EEPROMData[i].initial2 = EEPROMData[i-1].initial2;
			EEPROMData[i].initial3 = EEPROMData[i-1].initial3;
			EEPROMData[i].score = EEPROMData[i-1].score;
		}

		ClearBuffer();
		blitFont();

		uint16_t vramPos = TEXT_MODE_XY(9,9);

		printText(MSG_ENTER_INITIALS, vramPos);

		vramPos = TEXT_MODE_XY(13,12);
		vram[vramPos] = 0x37<<1;
		vramPos += 2;
		vram[vramPos] = 0x37<<1;
		vramPos += 2;
		vram[vramPos] = 0x37<<1;
		vramPos = TEXT_MODE_XY(13,11);


		EEPROMData[higherThan].initial1 = getCharFromUser(vramPos);
		vramPos += 2;
		EEPROMData[higherThan].initial2 = getCharFromUser(vramPos);
		vramPos += 2;
		EEPROMData[higherThan].initial3 = getCharFromUser(vramPos);
		EEPROMData[higherThan].score = scoreLong;

		readWriteEEPROM(1);
	}
}

void addBonusPoints(void){
	uint16_t zBonus = 0;
	uint16_t cBonus = 0;
	uint16_t dBonus = 0;

	uint32_t pointsBCD;

	ClearBuffer();
	blitFont();

	if (COLLECTOR_BONUS_FLAG) cBonus = 5000;
	if (SUPERZAP_FLAG)        zBonus = 5000;
	dBonus = dominationBonus * 100;

	printText(MSG_DOMINATION, TEXT_MODE_XY(4, 9));
	printText(MSG_BONUS,      TEXT_MODE_XY(15, 9));
	printText(MSG_COLLECTOR,  TEXT_MODE_XY(4, 11));
	printText(MSG_BONUS,      TEXT_MODE_XY(14, 11));
	printText(MSG_ZAPPER,     TEXT_MODE_XY(4, 13));
	printText(MSG_BONUS,      TEXT_MODE_XY(11, 13));
	printText(MSG_TOTAL,      TEXT_MODE_XY(4, 16));
	printText(MSG_BONUS,      TEXT_MODE_XY(10, 16));

	pointsBCD = convertScoreBCD(dBonus);
	printBCD(pointsBCD, TEXT_MODE_XY(20, 9));
	pointsBCD = convertScoreBCD(cBonus);
	printBCD(pointsBCD, TEXT_MODE_XY(20,11));
	pointsBCD = convertScoreBCD(zBonus);
	printBCD(pointsBCD, TEXT_MODE_XY(20,13));
	pointsBCD = convertScoreBCD(zBonus + cBonus + dBonus);
	printBCD(pointsBCD, TEXT_MODE_XY(20,16));

	addScoreFull(pointsBCD);

	uint8_t i = BONUS_SCORE_SCREEN_DURATION;
	while(i > 0) {
		i--;
		waitNFrames(2);
		if (checkStartPressed()) {
			i = 0;
		}
	}
}

int32_t convertScoreLong(uint32_t scoreBCD){

	uint8_t BCDPlace;
	uint8_t BCDDigit;
	int32_t BCDAdd = 1;
	int32_t result = 0;

	for (BCDPlace = 0; BCDPlace < 8; BCDPlace++){
		BCDDigit = scoreBCD & 0x0000000F;
		scoreBCD = scoreBCD >> 4;
		while (BCDDigit > 0) {
			result = result + BCDAdd;
			BCDDigit--;
		}
		BCDAdd = BCDAdd * 10;
	}
	return(result);
}
uint8_t convertBCDDiv(int32_t *value, uint32_t div){
	int8_t result = 0;

	while (*value >= div) {
		result++;
		*value = *value - div;
	}

	return (result);
}

int32_t convertScoreBCD(int32_t scoreLong){

	int32_t result = 0;

	result = result + ((uint32_t)convertBCDDiv(&scoreLong, 10000000)<<28);
	result = result + ((uint32_t)convertBCDDiv(&scoreLong, 1000000)<<24);
	result = result + ((uint32_t)convertBCDDiv(&scoreLong, 100000)<<20);
	result = result + ((uint32_t)convertBCDDiv(&scoreLong, 10000)<<16);
	result = result + ((uint32_t)convertBCDDiv(&scoreLong, 1000)<<12);
	result = result + ((uint32_t)convertBCDDiv(&scoreLong, 100)<<8);
	result = result + ((uint32_t)convertBCDDiv(&scoreLong, 10)<<4);
	result = result + (uint32_t)convertBCDDiv(&scoreLong, 1);

	return(result);
}

