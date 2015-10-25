/*
 * highscores.h
 *
 *  Created on: 23/05/2015
 *      Author: Cunning Fellow
 */

#ifndef HIGHSCORES_H_
#define HIGHSCORES_H_

#define SD_FILE_START_OF_TITLE_DATA 864448
#define SD_FILE_SIZE_OF_TITLE_DATA 6
#define SD_FILE_START_OF_CREDIT_DATA (SD_FILE_START_OF_TITLE_DATA + SD_FILE_SIZE_OF_TITLE_DATA)


void showMovie(uint8_t movieNum);
void showTitle(void);
void showCredits(void);
void showHighScore(void);
uint8_t checkStartPressed(void);
void gameOver(void);
uint8_t printBCDDigit(uint8_t digit, uint8_t *suppress);
void printBCD(uint32_t BCD, uint16_t vramPos);
void printHighScoreTable(void);
uint8_t readWriteEEPROM(bool direction);
uint16_t singleButtonRising(void);
uint8_t getCharFromUser(uint16_t vramPos);
void checkHighScores(void);
void addBonusPoints(void);
int32_t convertScoreLong(uint32_t scoreBCD);
uint8_t convertBCDDiv(int32_t *value, uint32_t div);
int32_t convertScoreBCD(int32_t scoreLong);

typedef struct {
	struct{
		unsigned int initial1:6;
		unsigned int initial2:6;
		unsigned int initial3:6;
		unsigned long score:27;
	};
} EEPROMDataStruct;

#endif /* HIGHSCORES_H_ */
