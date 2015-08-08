/*
 *  Uzebox(tm) Tileset Converter
 *  Copyright (C) 2010  Alec Bourque
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
 * See the wiki for usage: http://uzebox.org/wiki/index.php?title=Gconvert
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include "tinyxml.h"
#include "lodepng.h"
#include "paletteTable.h"
using namespace std;

#define VERSION_MAJ 1
#define VERSION_MIN 7
void parseXml(TiXmlDocument* doc);
bool process();
unsigned char* loadRawImage();
unsigned char* loadPngImage();
unsigned char* loadImage();
void loadPalette();
const char* toUpperCase(const char *src);
int paletteIndexFromColor(unsigned char color);

//void exportType8bpp(unsigned char* buffer, vector<unsigned char*> uniqueTiles, FILE *tf);

enum Duplicates { discard=0, keep } ;

struct TileMap {
	const char* varName;
	int left;
	int top;
	int width;
	int height;
};

struct Palette {
	unsigned char colors[256];
	int numColors;
	int maxColors;
	const char* filename;
	const char* varName;
	bool exportPalette;
	int transparentColor;		// Optional, the color that is used for sprite transparency (default 254)
};

struct ConvertionDefinition {
	int version;		//"1"
	const char* xformFile;
	const char* inputFile;
	const char* inputType;		//"raw" and "png" are only valid values
	const char* outputType;		//"8bpp" (default) or "code"
	const char* outputFile;
	const char* tilesVarName;
	int backgroundColor;		//optional, specify the mask color for mode 9
	bool isBackgroundTiles;
	enum Duplicates duplicateTiles;

	int width;			//total image width in pixels
	int height;			//total image height in pixels
	int tilesetHeight;  //height of section from top reserved for tileset tile (remaining contains maps data)
	int tileWidth;		//in pixels
	int tileHeight;		//in pixels

	int mapsPointersSize;
	TileMap* maps;
	int mapsCount;
	
	Palette palette;
};

typedef struct Image{
	unsigned char* buffer;
	int tileWidth;
	int tileHeight;
	int width;
	int height;
};

ConvertionDefinition xform;

int main(int argc, char *argv[]) {

	if(argc==1){
		printf("Error: No input file provided.\n\n");
		printf("Uzebox graphics converter version %i.%i.\n",VERSION_MAJ,VERSION_MIN);
		printf("Usage: gconv <configuration.xml>");
		exit( 1 );
	}

    printf("\n*** Gconvert version %i.%i ***\n",VERSION_MAJ,VERSION_MIN);

	char *path=NULL;
	size_t size=0;
	path=getcwd(path,size);
	printf("Current working directory: %s\n",path);

	//load the xform definition file
	printf("Loading transformation file: %s ...\n",argv[1]);
	TiXmlDocument doc (argv[1]);
	xform.xformFile=argv[1];

	doc.LoadFile();
	if ( doc.Error() )
	{
		printf( "Error in %s: %s\n", doc.Value(), doc.ErrorDesc() );
		exit( 1 );
	}

	//parse configuration
	parseXml(&doc);

	//generate include file
	if(!process()){
		exit(1);
	}



	return 0;
}

int paletteIndexFromColor(unsigned char color){
	
	for(int index = 0; index < xform.palette.numColors; index++){
		if(xform.palette.colors[index] == color)
			return index;	
	}
	return -1;
}

unsigned char* getTileAt(int x,int y,Image *image){
	//extract tile pixel data
	unsigned char* tile=new unsigned char[image->tileWidth*image->tileHeight];
	int tileIndex=0;
	int horizontalTiles=image->width/image->tileWidth;

	for(int th=0;th<image->tileHeight;th++){
		for(int tw=0;tw<image->tileWidth;tw++){

			int index=(y*horizontalTiles*image->tileWidth*image->tileHeight)
					+(x*image->tileWidth)+(th*horizontalTiles*image->tileWidth)+ tw;

			tile[tileIndex++]=image->buffer[index];
		}
	}

	return tile;
}

bool tilesEqual(unsigned char* tile1,unsigned char* tile2,int lenght){
	int j;
	for(j=0;j<lenght;j++){
		if(*tile1!=tile2[j]) break;
		tile1++;
	}

	if(j==lenght){
		return true;
	}else{
		return false;
	}
}

bool process(){

	Image image;
	image.buffer=loadImage();


	if(image.buffer==NULL){
		return false;
	}

	//some validation
    if(xform.maps!=NULL && xform.mapsPointersSize!=8 && xform.mapsPointersSize!=16){
		printf("Error: Invalid map pointers size: %i. Valid values are 8 and 16.\n", xform.mapsPointersSize);
		return false;
    }
    if(xform.width==0 || xform.height==0){
		printf("Error: Invalid image size(%i,%i)\n", xform.width, xform.height);
		return false;
    }
    if(xform.tileWidth==0 || xform.tileHeight==0){
		printf("Error: Invalid tile size(%i,%i)\n", xform.tileWidth, xform.tileHeight);
		return false;
    }

    if((xform.width%xform.tileWidth!=0)){
    	printf("Error: Image width must an integer multiple of the tile width.\n");
    	return false;
    }

    if((xform.height%xform.tileHeight!=0)){
    	printf("Error: Image height must be an integer multiple of the tile height.\n");
    	return false;
    }
	
	if(xform.palette.filename != NULL){
		loadPalette();
		if(xform.palette.numColors == 0) {
			printf("Error: no palette colors loaded\n");
			return false;
		}
	}

	image.tileWidth=xform.tileWidth;
	image.tileHeight=xform.tileHeight;
	image.width=xform.width;
	image.height=xform.height;

	printf("File version: %i\n",xform.version);
	printf("Input file: %s\n",xform.inputFile);
	printf("Input file type: %s\n",xform.inputType);
	printf("Input width: %ipx\n",xform.width);
	printf("Input height: %ipx\n",xform.height);
	printf("Tile width: %ipx\n",xform.tileWidth);
	printf("Tile height: %ipx\n",xform.tileHeight);
	printf("Output file: %s\n",xform.outputFile);
	printf("Output type: %s\n",xform.outputType);
	printf("Duplicate tiles: %s\n",xform.duplicateTiles==keep?"keep":"discard");

	printf("Tiles variable name: %s\n",xform.tilesVarName);
	if(xform.maps!=NULL){
		printf("Maps pointers size: %i\n",xform.mapsPointersSize);
		printf("Map elements: %i\n",xform.mapsCount);
	}
	if(xform.palette.filename != NULL){
		printf("Input palette: %s\n", xform.palette.filename);
		printf("Palette variable name: %s\n", xform.palette.varName);
		printf("Palette max colors: %d\n", xform.palette.maxColors);
		printf("Palette unique colors: %d\n", xform.palette.numColors);
		printf("Palette transparent color: %d\n", xform.palette.transparentColor);
	}

	int horizontalTiles=xform.width/xform.tileWidth;
	int verticalTiles=xform.height/xform.tileHeight;
	int totalSize=0;
	int verticalTilesetHeight=xform.tilesetHeight==0?verticalTiles:xform.tilesetHeight;


	vector<unsigned char*> uniqueTiles;
	int count=0;

	//build tile file from tiles
    FILE *tf = fopen(xform.outputFile,"wt");
    if (!tf){
    	printf("Error: Unable to write to tiles output file %s\n", xform.outputFile);
    	return false;
    }

    fprintf(tf,"/*\n");
    fprintf(tf," * Transformation file: %s\n",xform.xformFile);
    fprintf(tf," * Source image: %s\n",xform.inputFile);
    fprintf(tf," * Tile width: %ipx\n",xform.tileWidth);
    fprintf(tf," * Tile height: %ipx\n",xform.tileHeight);
    fprintf(tf," * Output format: %s\n",xform.outputType);
    fprintf(tf," */\n");

    //build unique tileset
	for(int v=0; v<verticalTilesetHeight; v++){
		for(int h=0; h<horizontalTiles; h++){

			unsigned char* tile=getTileAt(h,v,&image);

			if(xform.duplicateTiles==keep){
				uniqueTiles.push_back(tile);
			}else{
				int refIndex=-1;
				//check if tile already exist
				for(int i=uniqueTiles.size()-1;i>=0;i--){
					unsigned char* b=uniqueTiles.at(i);

					int j;
					for(j=0;j<xform.tileWidth*xform.tileHeight;j++){if(*b!=tile[j]) break;b++;}

					if(j==(xform.tileWidth*xform.tileHeight)){
						refIndex=i;	//tile already exist in unique list
						break;
					}
				}

				if(refIndex==-1){
					uniqueTiles.push_back(tile);
				}else{
					free(tile);
				}
			}

			count++;
		}
	}

	int listSize=uniqueTiles.size();

	//Export maps first
	if(xform.maps!=NULL){

		int index=0;

		for(int m=0; m<xform.mapsCount; m++){
			TileMap map=xform.maps[m];

			//validate map
			if(map.left>horizontalTiles || map.left+map.width>horizontalTiles || map.top>verticalTiles || map.top+map.height >verticalTiles){
				printf("Error: Positions or sizes are out of bound for map: %s\n",map.varName);
				return false;
			}

			fprintf(tf,"#define %s_WIDTH %i\n",toUpperCase(map.varName),map.width);
			fprintf(tf,"#define %s_HEIGHT %i\n",toUpperCase(map.varName),map.height);

			if(xform.mapsPointersSize==8){
				fprintf(tf,"const char %s[] PROGMEM ={\n",map.varName);
			}else{
				fprintf(tf,"const int %s[] PROGMEM ={\n",map.varName);
			}

			fprintf(tf,"%i,",map.width);
			fprintf(tf,"%i",map.height);


			int c=0;
			for(int y=map.top;y<(map.top+map.height);y++){
				for(int x=map.left;x<(map.left+map.width);x++){

					if(c%20==0)	fprintf(tf,"\n"); //wrap line

					fprintf(tf,",");

					//check for first tile that match pixels at the current map position
					unsigned char* tile=getTileAt(x,y,&image);
					index=-1;
					for(unsigned int i=0;i<uniqueTiles.size();i++){
						unsigned char* b=uniqueTiles.at(i);
						if(tilesEqual(tile,b,image.tileWidth*image.tileHeight)){
							index=i;	//tile found in tile list
							break;
						}
					}
					free(tile);

					if(index==-1){
						printf("Map tile not found in tilset!\n");
						return false;
					}

					fprintf(tf,"0x%x",index);


					c++;

				}
			}

			fprintf(tf,"};\n\n");

			totalSize+=((map.height*map.width)+2)*(xform.mapsPointersSize/8);
		}
	}

	if(xform.outputType==NULL || strcmp(xform.outputType,"8bpp")==0){

		/*Export tileset in 8 bits per pixel format*/
	    fprintf(tf,"#define %s_SIZE %i\n",toUpperCase(xform.tilesVarName),uniqueTiles.size());
	    fprintf(tf,"const char %s[] PROGMEM={\n",xform.tilesVarName);

		int c=0,t=0;
		vector<unsigned char*>::iterator it;
		for(it=uniqueTiles.begin();it < uniqueTiles.end();it++){

			unsigned char* tile=*it;

			for(int index=0;index<(xform.tileWidth*xform.tileHeight);index++){
				if(c>0)fprintf(tf,",");
				fprintf(tf," 0x%x",tile[index]);
				c++;
			}
			fprintf(tf,"\t\t //tile:%i\n",t);
			t++;
		}
		fprintf(tf,"};\n");
		totalSize+=(uniqueTiles.size()*xform.tileHeight*xform.tileHeight);

	}else if(strcmp(xform.outputType,"3bpp")==0){
		if(xform.palette.numColors == 0) {
			printf("Error using 3bpp but no palette specified!\n");
		}
		else{
			bool invalidColor=false;
			/*Export tileset in 3 bits per pixel format*/
		    fprintf(tf,"#define %s_SIZE %i\n",toUpperCase(xform.tilesVarName),uniqueTiles.size());

			if(xform.isBackgroundTiles){
			    fprintf(tf,"const char vector_table_filler[144] __attribute__ ((section (\".uze_progmem_origin\")))={};\n");
				fprintf(tf,"const char %s[] __attribute__ ((section (\"uze_progmem_origin\")))={\n",xform.tilesVarName);
			}else{
				fprintf(tf,"const char %s[] PROGMEM ={\n",xform.tilesVarName);
			}

	
			int c=0,t=0;
			unsigned char b;
			vector<unsigned char*>::iterator it;
			for(it=uniqueTiles.begin();it < uniqueTiles.end();it++){
	
				unsigned char* tile=*it;
	
				for(int y=0;y<xform.tileHeight;y++){
					//pack 2 pixels in one byte
					for(int x=0;x<xform.tileWidth;x+=2){
						int first = tile[(y*xform.tileWidth)+x];
						int second = tile[(y*xform.tileWidth)+x+1];
						
						if(first != xform.palette.transparentColor){
							first = paletteIndexFromColor(first) << 1;
							if(first == -1){
								invalidColor=true;
								first=0;
							}
						}
						else first = 0x1;
						
						if(second != xform.palette.transparentColor){
							second = paletteIndexFromColor(second) << 1;
							if(second == -1){
								invalidColor=true;
								second=0;
							}
						}
						else second = 0x1;
						
						b  = (first & 0xF);
						b |= (second & 0xF) << 4;
						fprintf(tf," 0x%x,",b);
					}
					c++;
				}
				fprintf(tf,"\t\t //tile:%i\n",t);
				t++;
			}
			fprintf(tf,"};\n\n");
			totalSize+=(uniqueTiles.size()*xform.tileHeight*xform.tileHeight/2);
			
			if(invalidColor){
				printf("Warning: colors in input image not included in palette");
			}
		}
	}
	else if(strcmp(xform.outputType,"4bpp")==0){
		if(xform.palette.numColors == 0) {
			printf("Error using 4bpp but no palette specified!\n");
		}
		else{
			bool invalidColor=false;
			/*Export tileset in 4 bits per pixel format*/
		    fprintf(tf,"#define %s_SIZE %i\n",toUpperCase(xform.tilesVarName),uniqueTiles.size());
			fprintf(tf,"const char %s[] PROGMEM ={\n",xform.tilesVarName);
	
			int c=0,t=0;
			unsigned char b;
			vector<unsigned char*>::iterator it;
			for(it=uniqueTiles.begin();it < uniqueTiles.end();it++){
	
				unsigned char* tile=*it;
	
				for(int y=0;y<xform.tileHeight;y++){
					//pack 2 pixels in one byte
					for(int x=0;x<xform.tileWidth;x+=2){
						int first = tile[(y*xform.tileWidth)+x];
						int second = tile[(y*xform.tileWidth)+x+1];
						
						if(first != xform.palette.transparentColor){
							first = paletteIndexFromColor(first);
							if(first == -1){
								invalidColor=true;
								first=0;
							}
						}
						else first = 0xF;
						
						if(second != xform.palette.transparentColor){
							second = paletteIndexFromColor(second);
							if(second == -1){
								invalidColor=true;
								second=0;
							}
						}
						else second = 0xF;
						
						b  = (first & 0xF);
						b |= (second & 0xF) << 4;
						fprintf(tf," 0x%x,",b);
					}
					c++;
				}
				fprintf(tf,"\t\t //tile:%i\n",t);
				t++;
			}
			fprintf(tf,"};\n\n");
			totalSize+=(uniqueTiles.size()*xform.tileHeight*xform.tileHeight/2);
			
			if(invalidColor){
				printf("Warning: colors in input image not included in palette");
			}
		}
	}else if(strcmp(xform.outputType,"mode13-extended")==0){
		if(xform.palette.numColors == 0) {
			printf("Error using extendedPalette but no palette specified!\n");
		}
		else{
			bool invalidColor=false;
			
			/*Export tileset in palette extended pixel format*/
		    fprintf(tf,"#define %s_SIZE %i\n",toUpperCase(xform.tilesVarName),uniqueTiles.size());			
		    fprintf(tf,"const char vector_table_filler[144] __attribute__ ((section (\".uze_progmem_origin\")))={};\n");
		    fprintf(tf,"const char %s[] __attribute__ ((section (\".uze_progmem_origin\")))={\n",xform.tilesVarName);
	
			int c=0,t=0;
			unsigned char b;
			vector<unsigned char*>::iterator it;
			for(it=uniqueTiles.begin();it < uniqueTiles.end();it++){
				unsigned char* tile=*it;
	
				for(int y=0;y<xform.tileHeight;y++){
					//pack 2 pixels in one byte
					for(int x=0;x<xform.tileWidth;x+=2){
						
						int first = paletteIndexFromColor(tile[(y*xform.tileWidth)+x]);
						int second = paletteIndexFromColor(tile[(y*xform.tileWidth)+x+1]);
						
						if(first == -1){
							invalidColor=true;
							first=0;
						}
						if(second == -1){
							invalidColor=true;
							second=0;
						}
						
						b  = (first & 0xF);
						b |= (second & 0xF) << 4;
						b = PaletteConversionTable[b];
						fprintf(tf," 0x%x,",b);
					}
					c++;
				}
				fprintf(tf,"\t\t //tile:%i\n",t);
				t++;
			}
			fprintf(tf,"};\n\n");
			totalSize+=(uniqueTiles.size()*xform.tileHeight*xform.tileHeight/2);
			
			if(invalidColor){
				printf("Warning: Input image contains colors not included in palette. They will appear as color index 0.");
			}
		}
	}else if(strcmp(xform.outputType,"1bpp")==0){

		/*Export tileset in 1 bits per pixel format*/
	    fprintf(tf,"#define %s_SIZE %i\n",toUpperCase(xform.tilesVarName),uniqueTiles.size());
	    fprintf(tf,"const char %s[] PROGMEM={\n",xform.tilesVarName);

		int c=0,t=0;
		unsigned char b;
		vector<unsigned char*>::iterator it;
		for(it=uniqueTiles.begin();it < uniqueTiles.end();it++){

			unsigned char* tile=*it;

			for(int y=0;y<xform.tileHeight;y++){
				if(c>0)fprintf(tf,",");

				b=0;
				//pack 8 pixels in one byte
				for(int x=0;x<xform.tileWidth;x++){
					if(tile[y*xform.tileWidth+x]!=0) b|=(0x80>>x);
				}
				fprintf(tf," 0x%x",b);
				c++;
			}
			fprintf(tf,"\t\t //tile:%i\n",t);
			t++;
		}
		fprintf(tf,"};\n");
		totalSize+=(uniqueTiles.size()*xform.tileHeight*xform.tileHeight);

	}else if(xform.outputType!=NULL && (strcmp(xform.outputType,"code")==0 || strcmp(xform.outputType,"code60")==0)){

		/*export "code tiles"*/
		fprintf(tf,"#if !(VIDEO_MODE==9 && RESOLUTION==60) \r\n#error The included code-tiles data is only compatible with video mode 9 with 60 columns.\r\n#endif\r\n");
	    fprintf(tf,"#define %s_SIZE %i\n",toUpperCase(xform.tilesVarName),uniqueTiles.size());
	    fprintf(tf,"const char %s[] PROGMEM __attribute__ ((aligned (4))) ={\n",xform.tilesVarName);

		int c=0,t=0;
		vector<unsigned char*>::iterator it;
		for(it=uniqueTiles.begin();it < uniqueTiles.end();it++){

			unsigned char* tile=*it;
			int pos;

			for(int index=0;index<xform.tileHeight;index++){
				if(c>0)fprintf(tf,",");
				pos=xform.tileWidth*index;

				//unsigned char col=tile[pos];

				//pixel 0
				if(xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]){
					fprintf(tf,"0x02,0x2D,"); 										//08 b9       	mov r16,r2
				}else{
					fprintf(tf,"0x%x,0x%x,",tile[pos]&0xf,0xe0|(tile[pos]>>4)); 	//01 e0       	ldi	r16, pixel color	; 1
				}
				fprintf(tf,"0x08,0xb9,"); 											//08 b9       	out	0x08, r16
				fprintf(tf,"0x19,0x91,"); 											//19 91       	ld	r17, Y+
				pos++;

				//pixel 1
				if(xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]){
					fprintf(tf,"0x02,0x2D,"); 										//08 b9       	mov r16,r2
				}else{
					fprintf(tf,"0x%x,0x%x,",tile[pos]&0xf,0xe0|(tile[pos]>>4)); 	//01 e0       	ldi	r16, pixel color	; 1
				}
				fprintf(tf,"0x08,0xb9,"); 											//08 b9       	out	0x08, r16
				fprintf(tf,"0x15,0x9f,");									 		//15 9f       	mul	r17, r21
				pos++;

				//pixel 2
				if(xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]){
					fprintf(tf,"0x02,0x2D,"); 										//08 b9       	mov r16,r2
				}else{
					fprintf(tf,"0x%x,0x%x,",tile[pos]&0xf,0xe0|(tile[pos]>>4)); 	//01 e0       	ldi	r16, pixel color	; 1
				}
				fprintf(tf,"0x08,0xb9,"); 											//08 b9       	out	0x08, r16
				fprintf(tf,"0x08,0x0e,"); 											//08 0e       	add	r0, r24
				fprintf(tf,"0x19,0x1e,"); 											//19 1e       	adc	r1, r25
				pos++;

				//pixel 3
				if(xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]){
					fprintf(tf,"0x02,0x2D,"); 										//08 b9       	mov r16,r2
				}else{
					fprintf(tf,"0x%x,0x%x,",tile[pos]&0xf,0xe0|(tile[pos]>>4)); 	//01 e0       	ldi	r16, pixel color	; 1
				}
				fprintf(tf,"0x08,0xb9,"); 											//08 b9       	out	0x08, r16
				fprintf(tf,"0xf9,0x01,"); 											//f9 01       	movw r30, r18
				fprintf(tf,"0x4a,0x95,"); 											//4a 95       	dec	r20
				pos++;

				//pixel 4
				if(xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]){
					fprintf(tf,"0x02,0x2D,"); 										//08 b9       	mov r16,r2
				}else{
					fprintf(tf,"0x%x,0x%x,",tile[pos]&0xf,0xe0|(tile[pos]>>4)); 	//01 e0       	ldi	r16, pixel color	; 1
				}
				fprintf(tf,"0x08,0xb9,"); 											//08 b9       	out	0x08, r16
				fprintf(tf,"0x09,0xf0,");											//09 f0       	breq	.+2
				fprintf(tf,"0xf0,0x01,"); 											//f0 01       	movw	r30, r0
				pos++;

				//pixel 5
				if(xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]){
					fprintf(tf,"0x02,0x2D,"); 										//08 b9       	mov r16,r2
				}else{
					fprintf(tf,"0x%x,0x%x,",tile[pos]&0xf,0xe0|(tile[pos]>>4)); 	//01 e0       	ldi	r16, pixel color	; 1
				}
				fprintf(tf,"0x08,0xb9,"); 											//08 b9       	out	0x08, r16
				fprintf(tf,"0x09,0x94 "); 											//09 94       	ijmp

				c++;
			}
			fprintf(tf,"\t\t //tile:%i\n",t);
			t++;
		}
		fprintf(tf,"};\n");
		totalSize+=(uniqueTiles.size()*xform.tileHeight*21*2);
	
	}else if(xform.outputType!=NULL && strcmp(xform.outputType,"code80")==0){

		/*export "code tiles"*/
		fprintf(tf,"#if !(VIDEO_MODE==9 && RESOLUTION==80) \r\n#error The included code-tiles data is only compatible with video mode 9 with 80 columns.\r\n#endif\r\n");
	    fprintf(tf,"#define %s_SIZE %i\n",toUpperCase(xform.tilesVarName),uniqueTiles.size());
	    fprintf(tf,"const char %s[] PROGMEM __attribute__ ((aligned (2))) ={\n",xform.tilesVarName);

		int c=0,t=0;
		vector<unsigned char*>::iterator it;
		for(it=uniqueTiles.begin();it < uniqueTiles.end();it++){

			unsigned char* tile=*it;
			int pos;

			for(int index=0;index<xform.tileHeight;index++){
				if(c>0)fprintf(tf,",");
				pos=xform.tileWidth*index;

				/*
				Generate assembly code (pixel color control if r2 (bg) or r3 (fg) is assembled):
				
				out 0x08,r2
				ld	r17, Y+

				out 0x08,r3
				mul	r17, r21

				out 0x08,r2
				add	r0, r24
				adc	r1, r25

				out 0x08,r3
				movw r30, r18
				dec	r20

				out 0x08,r2
				breq	.+2
				movw	r30, r0

				out 0x08,r3
				ijmp
				*/
				

				//pixel 0
				if((xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]) || tile[pos]==0){
					fprintf(tf,"0x28,0xb8,"); 										//28 b8			out 0x08,r2
				}else{
					fprintf(tf,"0x38,0xb8,"); 										//38 b8			out 0x08,r3
				}				
				fprintf(tf,"0x19,0x91,"); 											//19 91       	ld	r17, Y+
				pos++;

				//pixel 1
				if((xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]) || tile[pos]==0){
					fprintf(tf,"0x28,0xb8,"); 										//28 b8			out 0x08,r2
				}else{
					fprintf(tf,"0x38,0xb8,"); 										//38 b8			out 0x08,r3
				}	
				fprintf(tf,"0x15,0x9f,");									 		//15 9f       	mul	r17, r21
				pos++;

				//pixel 2
				if((xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]) || tile[pos]==0){
					fprintf(tf,"0x28,0xb8,"); 										//28 b8			out 0x08,r2
				}else{
					fprintf(tf,"0x38,0xb8,"); 										//38 b8			out 0x08,r3
				}	
				fprintf(tf,"0x08,0x0e,"); 											//08 0e       	add	r0, r24
				fprintf(tf,"0x19,0x1e,"); 											//19 1e       	adc	r1, r25
				pos++;

				//pixel 3
				if((xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]) || tile[pos]==0){
					fprintf(tf,"0x28,0xb8,"); 										//28 b8			out 0x08,r2
				}else{
					fprintf(tf,"0x38,0xb8,"); 										//38 b8			out 0x08,r3
				}	
				fprintf(tf,"0xf9,0x01,"); 											//f9 01       	movw r30, r18
				fprintf(tf,"0x4a,0x95,"); 											//4a 95       	dec	r20
				pos++;

				//pixel 4
				if((xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]) || tile[pos]==0){
					fprintf(tf,"0x28,0xb8,"); 										//28 b8			out 0x08,r2
				}else{
					fprintf(tf,"0x38,0xb8,"); 										//38 b8			out 0x08,r3
				}	
				fprintf(tf,"0x09,0xf0,");											//09 f0       	breq	.+2
				fprintf(tf,"0xf0,0x01,"); 											//f0 01       	movw	r30, r0
				pos++;

				//pixel 5
				if((xform.backgroundColor!=-1 && xform.backgroundColor==tile[pos]) || tile[pos]==0){
					fprintf(tf,"0x28,0xb8,"); 										//28 b8			out 0x08,r2
				}else{
					fprintf(tf,"0x38,0xb8,"); 										//38 b8			out 0x08,r3
				}
				fprintf(tf,"0x09,0x94 "); 											//09 94       	ijmp

				c++;
			}
			fprintf(tf,"\t\t //tile:%i\n",t);
			t++;
		}
		fprintf(tf,"};\n");
		totalSize+=(uniqueTiles.size()*xform.tileHeight*21*2);
	}
	
	if(xform.palette.varName && xform.palette.exportPalette){
		int b,c;
	    fprintf(tf,"#define %s_SIZE %i\n",toUpperCase(xform.palette.varName),xform.palette.numColors);
	    fprintf(tf,"const unsigned char %s[] PROGMEM={\n",xform.palette.varName);
		for(c=0;c < xform.palette.numColors;c++){
			if(c>0)fprintf(tf,",");
			b=xform.palette.colors[c];
			fprintf(tf," 0x%x",b);
		}
		fprintf(tf,"\n};\n");
		totalSize+=xform.palette.numColors;
	}
	
	fclose(tf);
	free(image.buffer);
	printf("File exported successfully!\nUnique tiles found: %i\nTotal size (tiles + maps): %i bytes\n",uniqueTiles.size(),totalSize);


	return true;
}


void parseXml(TiXmlDocument* doc){
	//root
	TiXmlElement* root=doc->RootElement();
	root->QueryIntAttribute("version",&xform.version);
	//input
	TiXmlElement* input=root->FirstChildElement("input");
	xform.inputFile=input->Attribute("file");
	input->QueryIntAttribute("width",&xform.width);
	input->QueryIntAttribute("height",&xform.height);
	input->QueryIntAttribute("tileset-height",&xform.tilesetHeight);
	input->QueryIntAttribute("tile-width",&xform.tileWidth);
	input->QueryIntAttribute("tile-height",&xform.tileHeight);
	xform.inputType=input->Attribute("type");

	//output
	TiXmlElement* output=root->FirstChildElement("output");
	xform.outputFile=output->Attribute("file");
	TiXmlElement* tiles=output->FirstChildElement("tiles");
	xform.tilesVarName=tiles->Attribute("var-name");
    xform.outputType=output->Attribute("type");
	const char* isBackgroundTiles=output->Attribute("isBackgroundTiles");
    xform.isBackgroundTiles=isBackgroundTiles && strstr(isBackgroundTiles,"true");
	if(output->QueryIntAttribute("background-color",&xform.backgroundColor)==TIXML_NO_ATTRIBUTE){
		xform.backgroundColor=-1;
	}
	const char* dups=output->Attribute("duplicate-tiles");
	if(dups!=NULL && strstr(dups,"keep")){
		xform.duplicateTiles=keep;
	}

	//palette
	TiXmlElement* paletteElem=output->FirstChildElement("palette");
	if(paletteElem!=NULL){
		xform.palette.filename=paletteElem->Attribute("file");
		paletteElem->QueryIntAttribute("maxColors",&xform.palette.maxColors);
		xform.palette.varName=paletteElem->Attribute("var-name");
		const char* exportPalette=paletteElem->Attribute("exportPalette");
		xform.palette.exportPalette=exportPalette && strstr(exportPalette,"true");
		if(paletteElem->QueryIntAttribute("transparent-color",&xform.palette.transparentColor)==TIXML_NO_ATTRIBUTE){
			xform.palette.transparentColor=254;
		}
	}

	//maps
	TiXmlElement* mapsElem=output->FirstChildElement("maps");
	if(mapsElem!=NULL){
		mapsElem->QueryIntAttribute("pointers-size",&xform.mapsPointersSize);

		//count # of map sub-elements
		const TiXmlNode* node;
		int mapCount=0;
		for(node=mapsElem->FirstChild("map");node;node=node->NextSibling("map"))mapCount++;

		TileMap* maps=new TileMap[mapCount];
		xform.mapsCount=mapCount;
		mapCount=0;
		for(node=mapsElem->FirstChild("map");node;node=node->NextSibling("map")){
			maps[mapCount].varName=node->ToElement()->Attribute("var-name");

			node->ToElement()->QueryIntAttribute("top",&maps[mapCount].top);
			node->ToElement()->QueryIntAttribute("left",&maps[mapCount].left);
			node->ToElement()->QueryIntAttribute("width",&maps[mapCount].width);
			node->ToElement()->QueryIntAttribute("height",&maps[mapCount].height);
			mapCount++;
		}
		if(mapCount>0){
			xform.maps=maps;
		}else{
			xform.maps=NULL;
		}
	}else{
		xform.maps=NULL;
	}

}

//load image in a byte arrays
unsigned char* loadRawImage(){
	unsigned int fileSize=xform.width*xform.height;

	unsigned char* buffer=new unsigned char[fileSize];
	FILE* inputFile;
    size_t ret;

    //Load input image to buffer
	inputFile=fopen(xform.inputFile,"rb");
	if (!inputFile)
	{
		printf("Error: Unable to open input file %s\n", xform.inputFile);
		return NULL;
	}

	ret=fread(buffer,1,fileSize,inputFile);
	fclose(inputFile);
	if(ret != fileSize) {
		printf("Error: File size does not match input parameters. Expected=%i, Read=%i.\n", fileSize,ret);
		return NULL;
	}

	return buffer;
}

void loadPalette(){

	  unsigned char* buffer;
	  unsigned char* image;
	  size_t buffersize, imagesize;
	  LodePNG_Decoder decoder;

	  LodePNG_loadFile(&buffer, &buffersize, xform.palette.filename); /*load the image file with given filename*/
	  LodePNG_Decoder_init(&decoder);
	  decoder.settings.color_convert=0; //dont't convert to RGBA
	  LodePNG_decode(&decoder, &image, &imagesize, buffer, buffersize); /*decode the png*/

	  if(decoder.error){
		  if(decoder.error==48){
			  printf("Error in decoding palette PNG: the input data is empty. Maybe a PNG file you tried to load doesn't exist or is in the wrong path.\n");
		  }else{
			  printf("Error in decoding palette PNG: %d\n", decoder.error);
		  }
		  /*cleanup decoder*/
		  free(buffer);
		  free(image);
		  LodePNG_Decoder_cleanup(&decoder);
		  return;
	  }

	  if( LodePNG_InfoColor_getBpp(&decoder.infoPng.color)!=8){
		  printf("Error: Invalid palette PNG image type. Must be PNG-8 with a 256 colors palette.\n");
		  /*cleanup decoder*/
		  free(buffer);
		  free(image);
		  LodePNG_Decoder_cleanup(&decoder);
		  return;
	  }

	  for(unsigned int n = 0; n < imagesize; n++){
		  int paletteIndex = paletteIndexFromColor(image[n]);
		  if(paletteIndex == -1 && image[n] != xform.palette.transparentColor){
			  if(xform.palette.numColors == xform.palette.maxColors){
				  printf("Warning: palette file exceeded %d colors", xform.palette.maxColors);
			  }
			  else {
			  	xform.palette.colors[xform.palette.numColors] = image[n];
			  	xform.palette.numColors++;
			  }
		  }	  
	  }

	  /*cleanup decoder*/
	  free(buffer);
	  LodePNG_Decoder_cleanup(&decoder);

	  free(image);
}

unsigned char* loadPngImage(){

	  unsigned char* buffer;
	  unsigned char* image;
	  size_t buffersize, imagesize;
	  LodePNG_Decoder decoder;

	  LodePNG_loadFile(&buffer, &buffersize, xform.inputFile); /*load the image file with given filename*/
	  LodePNG_Decoder_init(&decoder);
	  decoder.settings.color_convert=0; //dont't convert to RGBA
	  LodePNG_decode(&decoder, &image, &imagesize, buffer, buffersize); /*decode the png*/

	  if(decoder.error){
		  if(decoder.error==48){
			  printf("Error in decoding PNG: the input data is empty. Maybe a PNG file you tried to load doesn't exist or is in the wrong path.\n");
		  }else{
			  printf("Error in decoding PNG: %d\n", decoder.error);
		  }
		  /*cleanup decoder*/
		  free(buffer);
		  free(image);
		  LodePNG_Decoder_cleanup(&decoder);
		  return NULL;
	  }

	  if( LodePNG_InfoColor_getBpp(&decoder.infoPng.color)!=8){
		  printf("Error: Invalid PNG image type. Must be PNG-8 with a 256 colors palette.\n");
		  /*cleanup decoder*/
		  free(buffer);
		  free(image);
		  LodePNG_Decoder_cleanup(&decoder);
		  return NULL;
	  }

	  xform.width=decoder.infoPng.width;
	  xform.height=decoder.infoPng.height;

	  /*cleanup decoder*/
	  free(buffer);
	  LodePNG_Decoder_cleanup(&decoder);

	return image;
}

unsigned char* loadImage(){
	unsigned char* buffer;

	printf("Loading image..\n");

	//load the source image
	if(strcmp(xform.inputType,"raw")==0){
		buffer=loadRawImage();

	}else if(strcmp(xform.inputType,"png")==0){
		//load image and set height and width in xform struct
		buffer=loadPngImage();

	}else{
		printf( "Unsupported input file type '%s'. Valid values: 'raw' and 'png' \n", xform.inputType );
		return false;
	}

	return buffer;
}


const char* toUpperCase(const char *src){
    char* dest=new char[64];
	strcpy(dest,src);

	int i=0;
	while(dest[i]!=0){
		dest[i]=toupper(src[i]);
		i++;
	}
	return dest;
}
