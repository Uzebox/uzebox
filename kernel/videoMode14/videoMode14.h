#ifndef VIDEOMODE14_H_
#define VIDEOMODE14_H_
	
extern volatile uint8_t nextFreeRamTile;
extern volatile uint8_t FrameNo;

extern void ClearBuffer(void);
extern void ClearBufferFinal(void);
extern void SDCardVideoModeEnable(void);
extern void SDCardVideoModeDisable(void);
extern void waitNFrames(uint8_t n);

typedef void (*FUNC_PTR)(void);
extern void SetHsyncCallback(FUNC_PTR HsyncCallBackFunc);
extern void DefaultCallback(void);
extern void flashingCallback(void);
extern void BlankingCallback(void);
extern void titleCallback(void);
extern void creditCallback(void);

extern long getSectorBMP(void);

extern void bresh_line_C(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
extern void addScore(uint16_t BCDtoAdd);
extern void addScoreFull(uint32_t BCDtoAdd);
extern void decLives(void);
extern void incLives(void);

extern int8_t CosMulFastC(uint8_t Angle, uint8_t Dist);
extern int8_t SinMulFastC(uint8_t Angle, uint8_t Dist);
extern int8_t MulSU(uint8_t a, uint8_t b);

#endif /* VIDEOMODE14_H_ */
