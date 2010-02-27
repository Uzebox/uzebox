#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include "tinyxml.h"
#include "lodepng.h"
using namespace std;

#define VERSION 1

void parseXml(TiXmlDocument* doc);
bool process();
unsigned char* loadRawImage();
unsigned char* loadPngImage();
unsigned char* loadImage();
const char* toUpperCase(const char *src);

struct TileMap {
	const char* varName;
	int left;
	int top;
	int width;
	int height;
};

struct ConvertionDefinition {
	int version;		//"1"
	const char* xformFile;
	const char* inputFile;
	const char* inputType;		//"raw" and "png" are only valid values
	const char* outputFile;
	const char* tilesVarName;

	int width;			//in pixels
	int height;			//in pixels
	int tileWidth;		//in pixels
	int tileHeight;		//in pixels

	int mapsPointersSize;
	TileMap* maps;
	int mapsCount;
};

ConvertionDefinition xform;

int main(int argc, char *argv[]) {

	if(argc==1){
		printf("Error: No input file provided.\n\n");
		printf("Uzebox graphics converter version %i.\n",VERSION);
		printf("Usage: gconv <configuration.xml>");
		exit( 1 );
	}


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

bool process(){

	unsigned char* buffer=loadImage();

	if(buffer==NULL){
		return false;
	}

	//some validation
    if(xform.mapsPointersSize!=8 && xform.mapsPointersSize!=16){
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

	printf("File version: %i\n",xform.version);
	printf("Input file: %s\n",xform.inputFile);
	printf("Input file type: %s\n",xform.inputType);
	printf("Input width: %ipx\n",xform.width);
	printf("Input height: %ipx\n",xform.height);
	printf("Tile width: %ipx\n",xform.tileWidth);
	printf("Tile height: %ipx\n",xform.tileHeight);
	printf("Output file: %s\n",xform.outputFile);
	printf("Tiles variable name: %s\n",xform.tilesVarName);
	printf("Maps pointers size: %i\n",xform.mapsPointersSize);
	printf("Map elements: %i\n",xform.mapsCount);


	int horizontalTiles=xform.width/xform.tileWidth;
	int verticalTiles=xform.height/xform.tileHeight;
	int totalSize=0;

	vector<unsigned char*> uniqueTiles;
	int imageTiles[horizontalTiles*verticalTiles];
	int count=0;

	//build tile file from unique tiles
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
    fprintf(tf," */\n");


    //build unique tileset
	for(int v=0; v<verticalTiles; v++){
		for(int h=0; h<horizontalTiles; h++){

			unsigned char* tile=new unsigned char[xform.tileWidth*xform.tileHeight];
			int tileIndex=0;
			for(int th=0;th<xform.tileHeight;th++){
				for(int tw=0;tw<xform.tileWidth;tw++){

					int index=(v*horizontalTiles*xform.tileWidth*xform.tileHeight)
							+(h*xform.tileWidth)+(th*horizontalTiles*xform.tileWidth)+ tw;

					tile[tileIndex++]=buffer[index];
				}
			}

			int refIndex=-1;
			//check if tile already exist
			for(int i=uniqueTiles.size()-1;i>=0;i--){
				unsigned char* b=uniqueTiles.at(i);

				int j;
				for(j=0;j<xform.tileWidth*xform.tileHeight;j++){
					if(*b!=tile[j]) break;
					b++;
				}

				if(j==(xform.tileWidth*xform.tileHeight)){
					refIndex=i;	//tile already exist in unique list
					break;
				}
			}

			if(refIndex==-1){

				uniqueTiles.push_back(tile);
				imageTiles[count]=uniqueTiles.size()-1;

				bool allZero=true;
				for(int i=0;i<(xform.tileWidth*xform.tileHeight);i++){
						if(tile[i]!=0)allZero=false;
				}
				if(allZero){
					printf("Blank Tile index: %i\n",(uniqueTiles.size()-1));
				}

			}else{
					imageTiles[count]=refIndex;
			}

			count++;
		}
	}

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
					index=(y*horizontalTiles)+x;

					if(imageTiles[index]>0xff && xform.mapsPointersSize==8){
						printf("Error: Tile index %i greater than 255.",index);
						return false;
					}

					fprintf(tf,"0x%x",imageTiles[index]);

					c++;

				}
			}

			fprintf(tf,"};\n\n");

			totalSize+=((map.height*map.width)+2)*(xform.mapsPointersSize/8);
		}
	}

	//Export tileset
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

	fclose(tf);
	free(buffer);

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
	input->QueryIntAttribute("tile-width",&xform.tileWidth);
	input->QueryIntAttribute("tile-height",&xform.tileHeight);
	xform.inputType=input->Attribute("type");

	//output
	TiXmlElement* output=root->FirstChildElement("output");
	xform.outputFile=output->Attribute("file");
	TiXmlElement* tiles=output->FirstChildElement("tiles");
	xform.tilesVarName=tiles->Attribute("var-name");

	//maps
	TiXmlElement* mapsElem=output->FirstChildElement("maps");
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

	TileMap map=xform.maps[0];
	TileMap map2=xform.maps[1];



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
