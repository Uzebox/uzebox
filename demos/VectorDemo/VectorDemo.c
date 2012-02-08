/*
 *  Uzebox(tm) VectorDemo
 *  Copyright (C) 2009  Alec Bourque
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * This program demonstrate mode 6, a 1-bit ramtiles mode that allows 256 tiles.
 */

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <videoMode6/videoMode6.h>

#include "data/font.inc"

void Line(int x1, int yy1, int x2, int y2);
void SetPixel(unsigned char px,unsigned char py);

extern unsigned char ram_tiles[];
extern u8 sync_flags;
extern volatile u8 sync_pulse;
extern unsigned char nextFreeRamTile;
extern void mycallback(void);

bool testing=false;


s16 sine[] PROGMEM ={0,6,13,19,25,31,38,44,50,56,  
           62,69,75,81,87,92,98,104,110,116,  
           121,127,132,137,143,148,153,158,163,168,  
           172,177,182,186,190,194,198,202,206,210,  
           213,217,220,223,226,229,232,235,237,239,  
           241,243,245,247,249,250,251,252,253,254,  
           255,255,256,256};

s16 cosine[] PROGMEM ={256,256,256,255,255,254,
           253,252,251,249,248,246,244,242,240,238,  
           236,233,231,228,225,222,218,215,212,208,  
           204,200,196,192,188,184,179,175,170,165,  
           160,156,150,145,140,135,129,124,118,113,  
           107,101,95,90,84,78,72,65,59,53,  
           47,41,35,28,22,16,9,3,0 ,-4 ,-10,  
          -16 ,-23 ,-29 ,-35 ,-41 ,-48 ,-54 ,-60 ,-66 ,-72,  
          -78 ,-84 ,-90 ,-96 ,-102 ,-107 ,-113 ,-119 ,-124 ,-130,  
          -135 ,-141 ,-146 ,-151 ,-156 ,-161 ,-166 ,-171 ,-175 ,-180,  
          -184 ,-189 ,-193 ,-197 ,-201 ,-205 ,-209 ,-212 ,-216 ,-219,  
          -222 ,-225 ,-228 ,-231 ,-234 ,-236 ,-239 ,-241 ,-243 ,-245,  
          -247 ,-248 ,-250 ,-251 ,-252 ,-253 ,-254 ,-255 ,-256 ,-256,  
          -256 ,-256 ,-256 ,-256 ,-256 ,-255 ,-255 ,-254 ,-253 ,-252,  
          -251 ,-249 ,-248 ,-246 ,-244 ,-242 ,-240 ,-237 ,-235 ,-232,  
          -230 ,-227 ,-224 ,-221 ,-217 ,-214 ,-210 ,-207 ,-203 ,-199,  
          -195 ,-191 ,-186 ,-182 ,-178 ,-173 ,-168 ,-163 ,-159 ,-154,  
          -148 ,-143 ,-138 ,-133 ,-127 ,-122 ,-116 ,-110 ,-105 ,-99,  
          -93 ,-87 ,-81 ,-75 ,-69 ,-63 ,-57 ,-51 ,-44 ,-38,  
          -32 ,-26 ,-19 ,-13 ,-7,
           0,6,13,19,25,31,38,44,50,56,  
           62,69,75,81,87,92,98,104,110,116,  
           121,127,132,137,143,148,153,158,163,168,  
           172,177,182,186,190,194,198,202,206,210,  
           213,217,220,223,226,229,232,235,237,239,  
           241,243,245,247,249,250,251,252,253,254,  
           255,255,256,256};




//#include "data/fonts.pic.inc"

u8 x1[15];
u8 yy1[15];
u8 x2[5];
u8 y2[5];

s16 coord[]={-20,-20,-20,
			20,-20,-20,
			20,20,-20,
			-20,20,-20,

			-20,-20,20,
			20,-20,20,
			20,20,20,
			-20,20,20
			};

//s16 x=20,y=20;
s16 x=120,y=112;
s16 xx=200,yy=30;
s16 px=100,py=103;
u8 bx,by;
bool fire=false;
u8 sp;
u8 jj,j;


s16 PreXadd=0,PreYadd=0,PreZadd=100,PostXadd=144,PostYadd=112;
u8 sx[8],sy[8];//,sx2[8],sy2[8];
u8 zan=0,yan=0,xan=0,flip=0;
s16 volatile tx,ty,tz,xt,yt,zt;
bool autoRotate=true;
u8 bgcolors[224];
u8 fgcolors[224];
u8 barpos[6];

void callback3d(){

	ClearBuffer();

	Line2(sx[0],sy[0],sx[1],sy[1]);
	Line2(sx[1],sy[1],sx[2],sy[2]);
	Line2(sx[2],sy[2],sx[3],sy[3]);
	Line2(sx[3],sy[3],sx[0],sy[0]);

	Line2(sx[4],sy[4],sx[5],sy[5]);
	Line2(sx[5],sy[5],sx[6],sy[6]);
	Line2(sx[6],sy[6],sx[7],sy[7]);
	Line2(sx[7],sy[7],sx[4],sy[4]);

	Line2(sx[0],sy[0],sx[4],sy[4]);
	Line2(sx[1],sy[1],sx[5],sy[5]);
	Line2(sx[2],sy[2],sx[6],sy[6]);
	Line2(sx[3],sy[3],sx[7],sy[7]);

/*	
	for(u8 i=0;i<8;i++){
		SetPixel2(sx[i],sy[i]);
	}
	*/
}

void rotate(){

	for(u8 i=0;i<8;i++){
		tx=coord[(i*3)+0];
		ty=coord[(i*3)+1];
		tz=coord[(i*3)+2];
		
		//x axis
		yt = ty*pgm_read_word(&cosine[xan]) - tz*pgm_read_word(&sine[xan]);
	    zt = ty*pgm_read_word(&sine[xan]) + tz*pgm_read_word(&cosine[xan]);
	    ty = yt>>8;//256;              //  Note that you must not alter the cordinates
	    tz = zt>>8;//256;              //  until both transforms are preformed

		//y axis
		xt = tx*pgm_read_word(&cosine[yan]) - tz*pgm_read_word(&sine[yan]);
	    zt = tx*pgm_read_word(&sine[yan]) + tz*pgm_read_word(&cosine[yan]);
	    tx = xt>>8;//256;              //  Note that you must not alter the cordinates
	    tz = zt>>8;///256;              //  until both transforms are preformed

		//z axis
		xt = tx*pgm_read_word(&cosine[zan]) - ty*pgm_read_word(&sine[zan]);
		yt = tx*pgm_read_word(&sine[zan]) + ty*pgm_read_word(&cosine[zan]);
	    tx = xt>>8;//256;              //  Note that you must not alter the cordinates
	    ty = yt>>8;//256;              //  until both transforms are preformed

		//project on 2D plane
		sx[i] = 128*(PreXadd+tx)/(tz+PreZadd) + PostXadd;
	    sy[i] = (128*ty)/(tz+PreZadd) + PostYadd;
	}

}

int main(){
//	SetUserRamTileIndex(191);
	SetFontTilesIndex(192);
	for(int i=0;i<(63*8);i++){
		ramTiles[(192*8)+i]=pgm_read_byte(&font[i]);
	}

	for(u8 t=0;t<224;t++){
		fgcolors[t]=0b11000000;
	}

	for(int i=0;i<SCREEN_TILES_H*SCREEN_TILES_V;i++) vram[i]=0;

	Print(11,1,PSTR(  "3D VECTOR DEMO"));
	Print(9,3,PSTR("288X224 RESOLUTION"));
	
	Print(3,5,PSTR("X:"));
	Print(9,5,PSTR("Y:"));
	Print(15,5,PSTR("Z:"));
	Print(21,5,PSTR("ZDIST:"));

	int joy,cnt=0;
	barpos[0]=10;
	barpos[1]=20;
	barpos[2]=30;

	rotate();

	if(testing){		
		TCCR1B=0x00;	//stop timers
		TCCR0B=0x00;	
	}else{
		SetHsyncCallback(&mycallback);
		SetUserPostVsyncCallback(&callback3d);
	}


	while(1){

 		if(!testing) 
			WaitVsync(1);

		u8 pos=0;
		//rotate coordinates

		if(autoRotate){
			//xan++;
			yan++;
			zan++;
		}
		
		rotate();

		PrintByte(7,5,xan,true);
		PrintByte(13,5,yan,true);
		PrintByte(19,5,zan,true);
		PrintByte(29,5,PreZadd,true);


		for(u8 t=0;t<224;t++){
			bgcolors[t]=0xff;
		}

		pos=8;
		bgcolors[pos++]=1;
		bgcolors[pos++]=2;
		bgcolors[pos++]=3;
		bgcolors[pos++]=4;
		bgcolors[pos++]=5;
		bgcolors[pos++]=6;
		bgcolors[pos++]=7;
		bgcolors[pos++]=6;

		pos=24;
		bgcolors[pos++]=8;
		bgcolors[pos++]=16;
		bgcolors[pos++]=24;
		bgcolors[pos++]=32;
		bgcolors[pos++]=40;
		bgcolors[pos++]=48;
		bgcolors[pos++]=56;
		bgcolors[pos++]=48;

		pos=40;
		bgcolors[pos++]=19;
		bgcolors[pos++]=28;
		bgcolors[pos++]=37;
		bgcolors[pos++]=46;
		bgcolors[pos++]=55;
		bgcolors[pos++]=63;
		bgcolors[pos++]=127;
		bgcolors[pos++]=63;

/*
		for(u8 t=0;t<224;t++){
			bgcolors[t]=0;
		}
		
		pos=105+(((s16)pgm_read_word(&sine[barpos[0]]))/3);			
		bgcolors[pos++]=1;
		bgcolors[pos++]=2;
		bgcolors[pos++]=3;
		bgcolors[pos++]=4;
		bgcolors[pos++]=5;
		bgcolors[pos++]=6;
		bgcolors[pos++]=7;
		bgcolors[pos++]=6;
		bgcolors[pos++]=5;
		bgcolors[pos++]=4;
		bgcolors[pos++]=3;
		bgcolors[pos++]=2;
		bgcolors[pos++]=1;
		barpos[0]+=2;

		pos=105+(((s16)pgm_read_word(&sine[barpos[1]]))/3);			
		bgcolors[pos++]=8;
		bgcolors[pos++]=16;
		bgcolors[pos++]=24;
		bgcolors[pos++]=32;
		bgcolors[pos++]=40;
		bgcolors[pos++]=48;
		bgcolors[pos++]=56;
		bgcolors[pos++]=48;
		bgcolors[pos++]=40;
		bgcolors[pos++]=32;
		bgcolors[pos++]=24;
		bgcolors[pos++]=16;
		bgcolors[pos++]=8;
		barpos[1]+=2;


		pos=105+(((s16)pgm_read_word(&sine[barpos[2]]))/3);			
		bgcolors[pos++]=19;
		bgcolors[pos++]=28;
		bgcolors[pos++]=37;
		bgcolors[pos++]=46;
		bgcolors[pos++]=55;
		bgcolors[pos++]=63;
		bgcolors[pos++]=127;

		bgcolors[pos++]=63;
		bgcolors[pos++]=55;
		bgcolors[pos++]=46;
		bgcolors[pos++]=37;
		bgcolors[pos++]=28;
		bgcolors[pos++]=19;
		barpos[2]+=2;

*/

		xx-=1;
		yy+=1;

		//x+=1;
		//y+=1;
	

		if(x>230 ||y>210){
			x=16;
			y=15;
		}

		if(xx<20 ||yy>210){
			xx=200;
			yy=30;
		}

		if(testing){
		//	callback();
			cnt++;
			if(cnt==60){
				cnt=0;
		//		callback();
			}
		}

		if(!testing){
			joy=ReadJoypad(0);
			if(joy!=0)autoRotate=false;

			if(joy&BTN_START){
				autoRotate=true;
			}


			if(joy&BTN_SL){
				PreZadd++;
				//PreXadd--;
				//while(ReadJoypad(0));
			}else if(joy&BTN_SR){
				PreZadd--;
				//PreXadd++;
				//while(ReadJoypad(0));
			}
		
			if(joy&BTN_LEFT){
				yan--;
			}else if(joy&BTN_RIGHT){
				yan++;
			}

			if(joy&BTN_UP){
				xan--;
			}else if(joy&BTN_DOWN){
				xan++;
			}

			if(joy&BTN_Y){
				zan--;
			}else if(joy&BTN_X){
				zan++;
			}


			if(joy&BTN_B){
				fire=true;
				bx=px;
				by=py;
			}

			if(fire) by-=2;
			if(by<2) fire=false;

		}
	}


} 

 
void Line(int x1, int yy1, int x2, int y2)
{
 int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-yy1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  
  sdx=dx>=0?1:-1;
  sdy=dy>=0?1:-1;
  
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=yy1;

  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
      SetPixel(px,py);    
    }
  }
  else /* the line is more vertical than horizontal */
  {
  
                for(i=0;i<dyabs;i++)
            {
              x+=dxabs;
              if (x>=dyabs)
              {
                x-=dyabs;
                px+=sdx;
              }
              py+=sdy;
             SetPixel(px,py);
  
            }

  }
}


//TODO: optimize to assembler

void SetPixel(u8 px,u8 py){

	//get vram tile index
	unsigned int addr=((py>>3)*SCREEN_TILES_H)+(px>>3);
	unsigned char c=vram[addr];
	if(c==0){
		//need to allocate a new ramtile
		if(nextFreeRamTile<(RAM_TILES_COUNT-1)){
			c=nextFreeRamTile;
			//clear the ramtile;
			for(unsigned char i=0;i<8;i++){
				ram_tiles[(c*8)+i]=0;
			}
			vram[addr]=c;
			nextFreeRamTile++;
		}else{
			return; //no more free ramtiles
		}
	}
	
	//plot the pixel
	unsigned int rAddr=(c*8)+(py&7);
	unsigned char pixels=ram_tiles[rAddr];
	pixels|=(0x80>>(px&7));
	ram_tiles[rAddr]=pixels;
}

