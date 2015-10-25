/*
 * webdata.h
 *
 *  Created on: 21/09/2014
 *      Author: Cunning Fellow
 */

#ifndef WEBDATA_H_
#define WEBDATA_H_

#include <avr/io.h>
#include <stdlib.h>
#include <stddef.h>

#define LANE_CLOSED_YES 0
#define LANE_CLOSED_NO  1

extern const uint8_t jumpTable[];
extern const uint8_t jumpBackTable[];
extern const uint8_t zTable[];
extern const uint8_t scaleTable[];
extern const uint8_t laneClosed[];

typedef struct {
	uint8_t xStart;
	uint8_t yStart;
	int8_t  xGrad;
	int8_t  yGrad;
	uint8_t highestEnemy;
	int8_t	spikeObj;
} LaneDataStruct;
extern LaneDataStruct laneData[];

typedef struct {
	int8_t x;
	int8_t y;
} Point;

typedef struct {
	uint8_t lane[16];
} LaneAngleStruct;
extern const LaneAngleStruct laneAngles[];

typedef struct {
	Point lane[16];
} LaneMiddleStruct;

typedef struct {
	Point offset[64];
} WebPanStruct;

typedef struct {
	LaneAngleStruct laneAngles;
	LaneMiddleStruct laneMiddles;
	WebPanStruct pan;
} WebDataStruct;

void getWebAngles(uint8_t lane, int8_t web);

#endif /* WEBDATA_H_ */
