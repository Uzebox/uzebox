//#include <avr/io.h>
//#include <stdlib.h>
#include <avr/pgmspace.h>

/*
 * Joystick constants & functions
 */
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


extern void WaitVsync(int count);
extern void SetFont(char x,char y, unsigned char tileId,unsigned char bgColor);
extern char vram[];  
extern void ClearVram(void);
extern volatile unsigned int joypad_status;
extern volatile unsigned char vsync_flag;

#define TEST_SIZE 0

#if TEST_SIZE == 0
const char strDemo[] PROGMEM = "   UZEBOX BOOTLOADER MENU   ";
const char strGame1[] PROGMEM= "MEGATRIS";
const char strGame2[] PROGMEM= "SPRITE DEMO";
const char strGame3[] PROGMEM= "CLAY'S MAZE";
const char strGame4[] PROGMEM= "CLAY'S SCROLL DEMO";
const char strGame5[] PROGMEM= "UZELESS CARLOS COUSINS";
const char strGame6[] PROGMEM= "BOULDER CRASH";
const char strGame7[] PROGMEM= "GIGAMAN";
const char strGame8[] PROGMEM= "MOLE POSITION";
const char strGame9[] PROGMEM= "QUARTER LIFE";

//Wait for the beginning of next frame (30hz)
void WaitVsync(int count){
	int i;
	for(i=0;i<count;i++){
		while(!vsync_flag);
		vsync_flag=0;
	}
}

void Print(int x,int y,const char *string,unsigned char bgColor){


	int i=0;
	char c;

	while(1){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			SetFont(x++,y,c,bgColor);
		}else{
			break;
		}
	}
	
}



void drawCursor(unsigned char x,unsigned char y,unsigned char len,unsigned char color){
	
	for(char dx=0;dx<len;dx++){
		vram[(y*30*2)+((x+dx)*2)+1]=color;
	}

}

int main(){

	ClearVram();

	Print(2,1,strDemo,80);

	Print(4,5,strGame1,0);
	Print(4,6,strGame2,0);
	Print(4,7,strGame3,0);
	Print(4,8,strGame4,0);
	Print(4,9,strGame5,0);
	Print(4,10,strGame6,0);
	Print(4,11,strGame7,0);
	Print(4,12,strGame8,0);
	Print(4,13,strGame9,0);

	unsigned char col=1,dir=1,x=3,y=5;

	while(1){
		drawCursor(x,y,25,col);
		WaitVsync(6);	

		col+=dir;
		if(col==5){
			dir=-1;
		}else if(col==0){
			dir=1;
		}

		if(joypad_status&BTN_UP){
			drawCursor(x,y,25,0);
			y--;
		}
	
		if(joypad_status&BTN_DOWN){
			drawCursor(x,y,25,0);
			y++;
		}

		if(joypad_status&BTN_SELECT){
			while(joypad_status!=0);
		}

	}


	while(1);
}


#else

int main(){
	while(1);
}

#endif

