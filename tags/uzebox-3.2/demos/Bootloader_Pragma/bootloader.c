//#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

/*
 * Joystick constants & functions
 */

//Controllers buttons
#define BTN_SR	   1
#define BTN_SL	   2
#define BTN_X	   4
#define BTN_A	   8
#define BTN_RIGHT  16
#define BTN_LEFT   32
#define BTN_DOWN   64
#define BTN_UP     128
#define BTN_START  256
#define BTN_SELECT 512
#define BTN_Y      1024 
#define BTN_B      2048 



extern volatile unsigned int joypad_status;
extern volatile unsigned char vsync_flag;
extern volatile unsigned char wave_vol;
extern char vram[];

//C wrapper calls
extern char mmcinit_c();
extern char fatinit_c();
extern void getmenudata_c();
extern void rendermenu_c();
extern void menuinit_c();
extern char menuClusterBuffer[];

extern void WaitVSync(unsigned char count);
extern void ClearVram(void);
extern void Print(unsigned int adress ,unsigned char *string, unsigned char bgColor);
extern void DrawBar(unsigned int adress,unsigned char len,unsigned char color);

#define TEST_SIZE 0
#define FONT_TYPE 0

unsigned char strMenu[] PROGMEM ="UZEBOX BOOTLOADER V1.0"; 



int main(){

	ClearVram();
	//DrawBar(2*40,40,80);
	//Print(((2*40)+18),strMenu,80);

	if(mmcinit_c()==0){
		Print((20*40)+4,PSTR("MMC OK"),0);
	}else{
		Print((20*40)+4,PSTR("MMC FAILED"),0);
	}

	if(fatinit_c()==0){
		Print((22*40)+4,PSTR("FAT OK"),0);
	}else{
		Print((22*40)+4,PSTR("FAT FAILED"),0);
	}

	char col=1,dir=1,x=2,y=5;

	menuinit_c();
	getmenudata_c();
//	rendermenu_c();

	for(int i=0;i<(18*6);i++){
	
		vram[(40*3*2)+(i*2)]=menuClusterBuffer[i];	
	}



	while(1){
		DrawBar((6*40)+x,40,col);
		WaitVSync(6);	

		col+=dir;
		if(col==5){
			dir=-1;
		}else if(col==0){
			dir=1;
		}

		


	}


}

