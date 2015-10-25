/*
 * housekeeping.h
 *
 *  Created on: 17/12/2014
 *      Author: Cunning Fellow
 */

#ifndef HOUSEKEEPING_H_
#define HOUSEKEEPING_H_

#include <avr/io.h>

#define SD_VIDEO_MODE_ENABLED       ((GPIOR1 &    0x01) != 0)

#define setVramClearFlag(void)      { GPIOR1 |=   0x02; }
#define clearVramClearFlag(void)    { GPIOR1 &= ~(0x02); }
#define VRAM_CLEAR_FLAG             ((GPIOR1 &    0x02) != 0)

#define setStatusUpdateFlag(void)   { GPIOR1 |=   0x04; }
#define clearStatusUpdateFlag(void) { GPIOR1 &= ~(0x04); }
#define STATUS_UPDATE_FLAG          ((GPIOR1 &    0x04) != 0)
#define setLaserPowerupFlag(void)   { GPIOR1 |=   0x08; }
#define clearLaserPowerupFlag(void) { GPIOR1 &= ~(0x08); }
#define LASER_POWER_UP              ((GPIOR1 &    0x08) != 0)
#define setAliveStateAFlag(void)    { GPIOR1 |=   0x10; }
#define clearAliveStateAFlag(void)  { GPIOR1 &= ~(0x10); }
#define ALIVE_STATE_A_FLAG          ((GPIOR1 &    0x10) != 0)
#define setAliveStateBFlag(void)    { GPIOR1 |=   0x20; }
#define clearAliveStateBFlag(void)  { GPIOR1 &= ~(0x20); }
#define ALIVE_STATE_B_FLAG          ((GPIOR1 &    0x20) != 0)

#define setLaneClosedFlag(void)     { GPIOR1 |=   0x40; }
#define clearLaneClosedFlag(void)   { GPIOR1 &= ~(0x40); }
#define LANE_CLOSED_FLAG            ((GPIOR1 &    0x40) != 0)

#define setActionTimerFlag(void)    { GPIOR1 |=   0x80; }
#define clearActionTimerFlag(void)  { GPIOR1 &= ~(0x80); }
#define ACTION_TIMER_FLAG           ((GPIOR1 &    0x80) != 0)

#define setZoomingFlag(void)    	{ GPIOR2 |=   0x01; }
#define clearZoomingFlag(void)  	{ GPIOR2 &= ~(0x01); }
#define ZOOMING_FLAG           		((GPIOR2 &    0x01) != 0)

#define setYesYesYesFlag(void)    	{ GPIOR2 |=   0x02; }
#define clearYesYesYesFlag(void)  	{ GPIOR2 &= ~(0x02); }
#define YES_YES_YES_FLAG       		((GPIOR2 &    0x02) != 0)

#define setJumpFlag(void)   	 	{ GPIOR2 |=   0x04; }
#define clearJumpFlag(void)  		{ GPIOR2 &= ~(0x04); }
#define JUMP_FLAG       			((GPIOR2 &    0x04) != 0)

#define setAIDroidFlag(void)   	 	{ GPIOR2 |=   0x08; }
#define clearAIDroidFlag(void)  	{ GPIOR2 &= ~(0x08); }
#define AIDROID_FLAG       			((GPIOR2 &    0x08) != 0)

#define setSuperZapFlag(void)    	{ GPIOR2 |=   0x10; }
#define clearSuperZapFlag(void) 	{ GPIOR2 &= ~(0x10); }
#define SUPERZAP_FLAG       		((GPIOR2 &    0x10) != 0)

#define setCollectorBonusFlag(void)    	{ GPIOR2 |=   0x20; }
#define clearCollectorBonusFlag(void) 	{ GPIOR2 &= ~(0x20); }
#define COLLECTOR_BONUS_FLAG       		((GPIOR2 &    0x20) != 0)

#define setSuperZapActiveFlag(void)    	{ GPIOR2 |=   0x40; }
#define clearSuperZapActiveFlag(void) 	{ GPIOR2 &= ~(0x40); }
#define SUPERZAP_ACTIVE_FLAG   			((GPIOR2 &    0x40) != 0)


#define showStackPointer() 			\
		uint8_t spl;				\
		uint8_t sph;				\
									\
		spl = SPL;					\
		sph = SPH;					\
									\
		vram[10] = sph>>4;			\
		vram[11] = sph&0x0F;		\
		vram[12] = spl>>4;			\
		vram[13] = spl&0x0F;

#endif /* HOUSEKEEPING_H_ */


