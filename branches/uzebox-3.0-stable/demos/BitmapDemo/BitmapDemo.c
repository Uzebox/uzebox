/*
 *  Uzebox Bitmap Demo
 *  Copyright (C) 2009 Alec Bourque
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
 * This program demonstrates video mode 8 (bitmap mode @ 120x96 2bpp)
 */
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uzebox.h"

#include "data/logo.inc"
#include "data/sprite.inc"

void Circle(unsigned char xCenter, unsigned char yCenter, unsigned char r, unsigned char color);
void Box(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color,bool fill);
void Line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color);
void mand();

void fade(){
	unsigned int r,g,b;	
	unsigned char pal[4];
	pal[0]=palette[0];
	pal[1]=palette[1];
	pal[2]=palette[2];
	pal[3]=palette[3];

	WaitVsync(50);

	for(int v=8;v>=0;v--){
		for(u8 i=0;i<4;i++){
			r=((pal[i]&7)*v)/8;
			g=(((pal[i]>>3)&7)*v)/8;
			b=((((pal[i]>>5)&6)*v)/8)&6;
			palette[i]=(b<<5)|(g<<3)|r;
		}
		WaitVsync(3);
	}

	ClearVram();
	WaitVsync(5);

	palette[0]=(rand()&0xff);// 0b00000000;
	palette[1]=(rand()&0xff);//0b00111000;
	palette[2]=(rand()&0xff);//0b00000111;
	palette[3]=(rand()&0xff);//0b11000000;
}

void fadein(){
	unsigned int r,g,b;	
	unsigned char pal[4];
	pal[0]=255;
	pal[1]=246;
	pal[2]=164;
	pal[3]=82;

	for(char v=0;v<=8;v++){
		for(u8 i=0;i<4;i++){
			r=((pal[i]&7)*v)/8;
			g=(((pal[i]>>3)&7)*v)/8;
			b=((((pal[i]>>5)&6)*v)/8)&6;
			palette[i]=(b<<5)|(g<<3)|r;
		}
		WaitVsync(3);
	}

	palette[0]=255;
	palette[1]=246;
	palette[2]=164;
	palette[3]=82;
}

int main(){

	//Clear the screen (fills the vram with tile zero)
	ClearVram();
	SetColorBurstOffset(4);







	srand(0x365e);

	while(1){

		//mand();
		//while(1);
		
		//fade();


		palette[0]=0;
		palette[1]=0;
		palette[2]=0;
		palette[3]=0;


		for(int j=0;j<(SCREEN_WIDTH/4)*SCREEN_HEIGHT;j++){
			vram[j]=pgm_read_byte(&(uzeboxlogo[j]));
		}



		fadein();			
		WaitVsync(10);

		/*
		for(int y=0;y<30;y++){
			for(int x=0;x<SCREEN_WIDTH;x++){
				unsigned char c=GetPixel(x,y+30);
				PutPixel(x,y,c);
			}
		}

		while(1);
		*/


		fade();

		for(int j=0;j<200;j++){
			Line( (rand()%119), (rand()%95) , (rand()%119), (rand()%95), (rand()%3)+1);
		}

		fade();

		for(int j=0;j<200;j++){
			Box( (rand()%119), (rand()%95) , (rand()%119), (rand()%95), (rand()%3)+1,true);
		}

		fade();

		for(int j=0;j<200;j++){
			Circle( (rand()%119), (rand()%95) , (rand()%80), (rand()%3)+1);
		}

		fade();

		for(int j=0;j<200;j++){
			Box( (rand()%119), (rand()%95) , (rand()%119), (rand()%95), (rand()%3)+1,false);
		}

		fade();


		for(int j=0;j<400;j++){
			PutPixel( (rand()%119), (rand()%95), (rand()%3)+1);
		}

		fade();


		palette[0]=	palette[0]=(rand()&0xff);//pgm_read_byte(&(sprite_palette[0]));
		palette[1]=pgm_read_byte(&(sprite_palette[1]));
		palette[2]=pgm_read_byte(&(sprite_palette[2]));
		palette[3]=pgm_read_byte(&(sprite_palette[3]));

		u8 sx,sy,px;

		for(int j=0;j<15;j++){
			sx=(rand()%119);
			sy=(rand()%95);

			if((rand()&1)==0){
				for(u8 y=0;y<sprite_height;y++){
					for(u8 x=0;x<sprite_width;x+=4){
						u8 pix=pgm_read_byte(&(sprite[(y*(sprite_width/4))+(x/4)]));
					
						if((px=(pix>>6))!=0) PutPixel(sx+x,sy+y,px);
						if((px=((pix>>4)&3))!=0) PutPixel(sx+x+1,sy+y,px);
						if((px=((pix>>2)&3))!=0) PutPixel(sx+x+2,sy+y,px);
						if((px=(pix&3))!=0) PutPixel(sx+x+3,sy+y,px);
					}
				}

			}else{
				for(u8 y=0;y<sprite_height;y++){
					for(u8 x=0;x<sprite_width;x+=4){
						u8 pix=pgm_read_byte(&(sprite[(y*(sprite_width/4))+(x/4)]));
					
						if((px=(pix>>6))!=0) PutPixel(sx-x,sy+y,px);
						if((px=((pix>>4)&3))!=0) PutPixel(sx-x-1,sy+y,px);
						if((px=((pix>>2)&3))!=0) PutPixel(sx-x-2,sy+y,px);
						if((px=(pix&3))!=0) PutPixel(sx-x-3,sy+y,px);
					}
				}
			}


		}
		WaitVsync(10);
		fade();

		mand();
		fade();
	}

} 


void PutPixel2(unsigned char x, unsigned char y,unsigned char color){
	if(x>=120 || y>=96) return;

	unsigned int addr=((SCREEN_WIDTH/4)*y)+(x>>2);
	unsigned char byte=vram[addr];
	color&=3;
	switch(x&3){
		case 3:						
			byte=(byte&~(3))|color;
			break;
		case 2:						
			byte=(byte&~(3<<2))|(color<<2);
			break;
		case 1:						
			byte=(byte&~(3<<4))|(color<<4);
			break;
		default:
			byte=(byte&~(3<<6))|(color<<6);
	}
	vram[addr]=byte;
}



void Box(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color,bool fill){
	if(fill){
		unsigned char tmp;
		if(x1>x2){
			tmp=x2;
			x2=x1;
			x1=tmp;
		}
		
		for(unsigned char y=y1;y<=y2;y++){
			for(unsigned char x=(x1>>2);x<=(x2>>2);x++){
				vram[((SCREEN_WIDTH/4)*y)+x]=(color<<6)|(color<<4)|(color<<2)|color;
			}
		}


	}else{
		unsigned char pos;
		for(pos=y1;pos<=y2;pos++){
			PutPixel(x1,pos,color);
			PutPixel(x2,pos,color);
		}
		for(pos=x1+1;pos<x2;pos++){
			PutPixel(pos,y1,color);
			PutPixel(pos,y2,color);
		}
	}
}

void Line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color)
{
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  
  sdx=dx>=0?1:-1;
  sdy=dy>=0?1:-1;
  
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

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
      PutPixel(px,py,color);	  
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
	     PutPixel(px,py,color);
  
	    }
  }
}


void Circle(unsigned char xCenter, unsigned char yCenter, unsigned char r, unsigned char color){
	
	int x=0,y=r;
	int d=3-(2*r);

    while(x<=y){

		PutPixel(xCenter+x,yCenter+y,color);
		PutPixel(xCenter+y,yCenter+x,color);
		PutPixel(xCenter-x,yCenter+y,color);
		PutPixel(xCenter+y,yCenter-x,color);
		PutPixel(xCenter-x,yCenter-y,color);
		PutPixel(xCenter-y,yCenter-x,color);
		PutPixel(xCenter+x,yCenter-y,color);
		PutPixel(xCenter-y,yCenter+x,color);

		if (d<0)
			d += (4*x)+6;
		else
		{
			d += (4*(x-y))+10;
			y -= 1;
		}
		x++;

	}

}


//fixed point math macros
#define FixedPtBits 13
#define toFixed(int_number) ((int32_t)int_number<<FixedPtBits)
#define fixedMul(a,b) (a*b)>>FixedPtBits
#define fixedDiv(numer,denom) (numer<<FixedPtBits)/denom
#define fixedDivWithInt(numer,denom) numer/denom
void mand(){
   int w=120;
   int h=96;  
   int x, y, i=0,stepx=1,stepy=1;
   int32_t x2, y2, px, py, zx, zy, max,fstepx,fstepy;
   int32_t top,left,bottom,right;

	palette[0]=0;
	palette[1]=246;
	palette[2]=164;
	palette[3]=82;

	top=toFixed(-1)-(1<<(FixedPtBits-2)); //-1.25
	left=toFixed(-2)-(1<<(FixedPtBits-2));
	bottom=toFixed(1)+(1<<(FixedPtBits-2)); //+1.25;
	right=toFixed(1);
	max=toFixed(4);

   fstepx=fixedMul(fixedDivWithInt((right-left),w),toFixed(stepx));
   fstepy=fixedMul(fixedDivWithInt((bottom-top),h),toFixed(stepy));
   py = top;



   for(y = 0; y < h; y+=stepy) {
      px = left;

      for(x = 0; x < w; x+=stepx) {  	
         zx = px; zy = py;
         for(i = 0; i < 16; ++i) {
            x2 = fixedMul(zx,zx);
			y2 = fixedMul(zy,zy);
            if(x2+y2 > max) break;
            zy = (fixedMul(zx,zy)<<1) + py; // Calculate z = z * z + p
            zx = x2-y2+px;
         }  
		 
		 if(i>=11 && i<16){
		 	PutPixel(x,y,1);
		 }else if(i>=6 && i<11){
		 	PutPixel(x,y,2);
		 }else if(i>=4 && i<6){
		 	PutPixel(x,y,3);
		 }else if(i<4){
		 	PutPixel(x,y,1);
		}
        
		 px+=(fstepx); // Move to the next pixel
      }

      py+=(fstepy); // Move to the next line of pixels
   }

  

}
