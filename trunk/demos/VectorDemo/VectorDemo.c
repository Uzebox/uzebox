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

void line(int x1, int y1, int x2, int y2);
void setPixel(unsigned char px,unsigned char py);
void vline(int x1,int y1,int y2);

extern unsigned char ram_tiles[];

unsigned char nextFreRamTile=1;


#include "data/fonts.pic.inc"

int main(){

	for(int i=0;i<SCREEN_TILES_H*SCREEN_TILES_V;i++) vram[i]=0;

	srand(0x365e);



	while(1){
		
		for(int j=0;j<100;j++){
			line( (rand()%((SCREEN_TILES_H-1)*8)), (rand()%((SCREEN_TILES_V-1)*8)) , (rand()%((SCREEN_TILES_H-1)*8)), (rand()%((SCREEN_TILES_V-1)*8)) );
		}

		WaitVsync(60);



		for(int i=0;i<SCREEN_TILES_H*SCREEN_TILES_V;i++) vram[i]=0;
		nextFreRamTile=1;
	}


} 


void line(int x1, int y1, int x2, int y2)
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
      setPixel(px,py);	  
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
	     setPixel(px,py);
  
	    }

  }
}



//TODO: optimize to assembler
void setPixel(unsigned char px,unsigned char py){

	//get vram tile index
	unsigned int addr=((py>>3)*SCREEN_TILES_H)+(px>>3);
	unsigned char c=vram[addr];
	if(c==0){
		//need to allocate a new ramtile
		if(nextFreRamTile<(RAM_TILES_COUNT-1)){
			c=nextFreRamTile;
			//clear the ramtile;
			for(unsigned char i=0;i<8;i++){
				ram_tiles[(c*8)+i]=0;
			}
			vram[addr]=c;
			nextFreRamTile++;
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

