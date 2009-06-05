//#include <avr/io.h>
#include <stdlib.h>
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
extern volatile unsigned char wave_vol;

#define TEST_SIZE 0
#define FONT_TYPE 0

#if TEST_SIZE == 0
const char strDemo[] PROGMEM = ">>>    Uzebox Bootloader Menu     <<<";
const char strGame1[] PROGMEM=  "Megatris                        Uze";
const char strGame2[] PROGMEM=  "Maze                   Clay Cowgill";
const char strGame3[] PROGMEM=  "Pac-Man                        Paul";
const char strGame4[] PROGMEM=  "Snakes                  DaveyPocket";
const char strGame5[] PROGMEM=  "Void Fighter                Jhysaun";
const char strGame6[] PROGMEM=  "Boulder Dash                    AJX";
const char strGame7[] PROGMEM=  "Dr.Mario                  CodeCrank";
const char strGame8[] PROGMEM=  "Whack-a-Mole                    Uze";
const char strGame9[] PROGMEM=  "Arkanoid                        Uze";
const char strGame10[] PROGMEM= "Pong                    DaveyPocket";
const char strGame11[] PROGMEM= "Uzeamp                          Uze";
const char strGame12[] PROGMEM= "ESD Attack             Clay Cowgill";
const char strGame13[] PROGMEM= "Sokoban                   D3thAdd3r";
const char strGame14[] PROGMEM= "Castlevania:Vengeance        Pragma";

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
	unsigned char c;

	while(1){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			#if FONT_TYPE==0
			if(c>=97 && c<=122) c-=32;
			#endif
				
			SetFont(x++,y,c,bgColor);
		}else{
			break;
		}
	}
	
}



void drawCursor(unsigned char x,unsigned char y,unsigned char len,unsigned char color){
	
	for(char dx=0;dx<len;dx++){
		vram[(y*40*2)+((x+dx)*2)+1]=color;
	}

}



int main(){



	ClearVram();

	Print(2,1,strDemo,80);

	Print(3,5,strGame1,0);
	Print(3,6,strGame2,0);
	Print(3,7,strGame3,0);
	Print(3,8,strGame4,0);
	Print(3,9,strGame5,0);
	Print(3,10,strGame6,0);
	Print(3,11,strGame7,0);
	Print(3,12,strGame8,0);
	Print(3,13,strGame9,0);
	Print(3,14,strGame10,0);
	Print(3,15,strGame11,0);
	Print(3,16,strGame12,0);
	Print(3,17,strGame13,0);
	Print(3,18,strGame14,0);


//	wave_vol=255;
//	WaitVsync(20);	

	unsigned char col=1,dir=1,x=2,y=5;

	while(1){
		drawCursor(x,y,37,col);
		WaitVsync(6);	

		col+=dir;
		if(col==5){
			dir=-1;
		}else if(col==0){
			dir=1;
		}

		if(joypad_status&BTN_UP){
			if(y>5){
				drawCursor(x,y,37,0);
				y--;
				wave_vol=255;
			}
		}
	
		if(joypad_status&BTN_DOWN){
			if(y<18){
				drawCursor(x,y,37,0);
				y++;
				wave_vol=255;
			}
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

