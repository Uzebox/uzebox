/*
 *  Uzebox(tm Alec Bourque) dconvert utility
 *  2017 Lee Weber
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
 *
 *  Uzebox is a reserved trade mark
*/

/* This tool takes C array or binary input, with an optional lead in offset
   and an optional byte length, and writes it into a binary file. The general
   purpose of this is expected to be the creation and modification of useful
   data images to be placed on the SD card, where user code will directly use
   it or perform a load into SPI ram, ie. SPI Ram Streaming Music and PCM.
   Additionally, it can perform PCM compression before placing data.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *fin, *fout, *fcfg;

int asBinIn;
int asBinOut;
int doPad;
int doDebug = 0;
int adpcmDebug = 0;
int doLength;
int doCustomName;
char arrayName[256];
char finName[256];
char foutName[256];
int transformType;
char lineBuf[256];
int eat;
int padBytes;
int skipBytes;
int skipArrays;

long outOff;
long fileOff;
long dirOff;
long runTime;
long minSize;
long fileSize;
long flashCost;
long totalFlashCost;
int inSize,outSize,cfgSize,cfgLine,cfgEntry;
unsigned char inBuf[1024*1024*64];
unsigned char outBuf[sizeof(inBuf)];
unsigned char tempBuf[sizeof(inBuf)];
//char tempBuf[sizeof(inBuf)];
unsigned char cfgBuf[1024*4];

unsigned char accum = 0;
unsigned char target,samples;
unsigned char slope = 0;


int ConvertAndWrite();

int main(int argc, char *argv[]){

	int i,j;

	if(argc < 2){
		printf("dconvert - data converter for Uzebox\n");
		printf("\treads C array or binary data, performs optional transorms, creates an\n");
		printf("\toptional directory, and outputs the data to a binary image or C array\n");
		printf("\tusage:\n\t\t\tdconvert config.cfg\n");
		goto DONE;
	}

	fcfg = fopen(argv[1],"r");
	if(fcfg == NULL){
		printf("Error: Failed to open config file: %s\n",argv[1]);
		goto ERROR;
	}

	while(1){/* eat any new lines the user inserted before the setup line */
		if(fgets(lineBuf,sizeof(lineBuf)-1,fcfg) == NULL){//read the initial line
			printf("Error: Failed to read parameter line\n");
			goto ERROR;
		}
		if(lineBuf[0] != '\r' && lineBuf[0] != '\n'){/* found junk before the setup line? */
			if(lineBuf[0] == '#'){//it is a comment line, eat it
				for(j=1;j<sizeof(lineBuf);j++){
					if(lineBuf[j] == '\n')
						break;
					if(j == sizeof(lineBuf)-1){
						printf("Error: Failed to find setup line\n");
						goto ERROR;
					}
				}
				continue;
			}else
				break;/* got garbage, let it fail */
		}else if((lineBuf[0] == '\r' && lineBuf[1] == '\n') || lineBuf[0] == '\n')/* found a Windows or Unix style line ending? Eat it */
			continue;
	}

	if(sscanf(lineBuf," %d , %d , %ld , %ld , %d ,  %ld , %255[^ ,\n\t] , %n ",&asBinIn,&asBinOut,&dirOff,&fileOff,&doPad,&minSize,foutName,&eat) != 7){
		printf("Error: Bad format on setup line. Got \"%s\"\n",lineBuf);
		printf("\tFormat is 7 comma separated entries, as:\n\t\t0/1 = C array or binary input,\n\t\t0/1 = C array or binary output,\n\t\tdirectory start offset,\n\t\tdata start offset,\n\t\t0/1 = pad data to 512 byte sector size,\n\t\tminimum file size in bytes(pad if less)\n\t\toutput file name,\n");
		printf("\n\tEx: 1,1,0,512,131072,1,OUTPUT.DAT\n\tbinary in, binary out, dir at 0, starting at 512, padded, pad file to 128K, to OUTPUT.DAT\n");
		goto ERROR;
	}

	printf(asBinOut ? "\n\tBinary output, ":"\n\tC array output");
	if(asBinOut){
		if(dirOff >= 0)
			printf("directory at %ld, data at %ld, to %s:\n",dirOff,fileOff,foutName);
		else
			printf("directory output disabled\n");
	}else
		printf(" to %s:\n",foutName);

	fout = fopen(foutName,asBinOut ? "rb+":"w");/* non-binary destroys any previous C array output */
	if(fout == NULL){/* file does not exist, try to create it. */
		printf("\tBinary output file does not exist");
		if(asBinOut){
			fout = fopen(foutName,"wb");
			if(fout == NULL){
				printf("\nError: Failed to create output file: %s\n",foutName);
				goto ERROR;
			}
			printf(", created %s\n",foutName);
			if(outOff)
				printf("Padding new binary file with %ld '0's\n",outOff);
			for(i=0;i<outOff;i++)/* pad the new file with 0 up to the specified offset */
				fputc(0,fout);
		}else{/* the "w" should have succeeded */
			printf("Error: Failed to create output file: %s\n",foutName);
			goto ERROR;
		}

	}

	cfgLine = 0;
	cfgEntry = 0;
	while(!feof(fcfg)){
		cfgLine++;

		if(fgets(lineBuf,sizeof(lineBuf)-1,fcfg) == NULL){//read the next line
			if(cfgEntry == 1)
				printf("\t== 1 entry total");
			else
				printf("\t== %d entries total",cfgEntry);
			if(asBinOut)
				printf(", %ld bytes data(including sector padding)\n",fileOff);
			else
				printf(", %ld bytes(total flash cost)\n",totalFlashCost);

			
			if(asBinOut){
				/* pad out data to minimum specified size if necessary */
				fseek(fout,0,SEEK_END);/* find total file size, which will likely be larger than data*/
				fileSize = ftell(fout);				
				while(fileSize < minSize){
					fputc(0,fout);
					fileSize++;
				}
				printf("\t== Total file size(data+directory+padding):%ld\n",fileSize);
			}			
			goto DONE;
		}

		sprintf(arrayName,"data%d",cfgEntry);
		if(sscanf(lineBuf," %255[^ ,\t] , %d , %d , %d ,  %32[^ ,\t] %n",finName,&skipArrays,&skipBytes,&transformType,arrayName,&eat) != 5){
			if(lineBuf[0] == '\r' || lineBuf[0] == '\n'){/* user entered an extra line end after the entries, eat it */
				continue;
			}else if(lineBuf[0] == '#'){//eat the comment line
				if(strncmp(lineBuf, "#DEBUG=1", 8) == 0){
					doDebug = 1;
					printf("**DCONVERT DEBUG enabled before line %d\n", cfgLine);
				}else if(strncmp(lineBuf, "#DEBUG=0", 8) == 0){
					doDebug = 0;
					printf("**DCONVERT DEBUG disabled before line %d\n", cfgLine);
				}else if(strncmp(lineBuf, "#ADPCM-DEBUG=1", 13) == 0){
					adpcmDebug = 1;
					printf("**DCONVERT ADPCM-DEBUG enabled before line %d\n", cfgLine);
				}else if(strncmp(lineBuf, "#ADPCM-DEBUG=0", 13) == 0){
					adpcmDebug = 0;
					printf("**DCONVERT ADPCM-DEBUG disabled before line %d\n", cfgLine);
				}
				for(j=1;j<sizeof(lineBuf);j++){
					if(lineBuf[j] == '\n')
						break;
					if(j == sizeof(lineBuf)-1){
						printf("Error: Did not find end of comment for line %d\n",cfgLine);
						goto ERROR;
					}
				}
				continue;
			}else if(lineBuf[0] == ';'){/* user wants a system call to run */
				unsigned char lbc = lineBuf[strlen(lineBuf)-1]; /* remove any '\r' or '\n' line endings */
				if(lbc == '\r' || lbc == '\n')
					lineBuf[strlen(lineBuf)-1] = '\0';
				lbc = lineBuf[strlen(lineBuf)-1];
				if(lbc == '\r' || lbc == '\n')
					lineBuf[strlen(lineBuf)-1] = '\0';
				printf("User system call on line %d, [%s]:\n", cfgLine, lineBuf+1);
				system(lineBuf+1);
				continue;
			}else{
				printf("Error: Bad format on entry %d line %d. Got \"%s\"\n",cfgEntry+1,cfgLine+1,lineBuf);
				goto ERROR;
			}
		}

		cfgEntry++;
		printf("\t+= %s,\n\t",finName);
		if(transformType == 0)
			printf("Raw");
		else if(transformType == 1)
			printf("Patch conversion");
		else if(transformType == 5)
			printf("1bit ADPCM conversion");
		else if(transformType == 6)
			printf("2bit ADPCM conversion");
		else if(transformType == 7)
			printf("4bit ADPCM conversion");
		else
			printf("Unknown(using Raw)");
		printf(", skip %d arrays, skip %d bytes, output offset:%ld\n",skipArrays,skipBytes,fileOff);

		fin = fopen(finName,"r");
		if(fin == NULL){
			printf("Error: Failed to open input file: %s\n",finName);
			goto ERROR;
		}

		i = ConvertAndWrite();

		if(i != 1){
			printf("ERROR: Conversion failed for %s on entry %d, line %d, error: %d\n",finName,cfgEntry,cfgLine,i);
			goto ERROR;
		}
	}

	goto DONE;
ERROR:
	exit(1);
DONE:	
	exit(0);
}



int ConvertAndWrite(){
	int i,j,k,w;
	unsigned char c1,c2,c3,t;
	w = 0;
	inSize = 0;
	outSize = 0;
	flashCost = 0;
	accum = 0x80;
	unsigned char up_count = 0;
	unsigned char down_count = 0;
	unsigned char neutral_count = 0;

	for(i=0;i<sizeof(inBuf);i++)
		inBuf[i] = outBuf[i] = 0;
		
	if(asBinIn){
		while(!feof(fin)){
			fread(inBuf+inSize,1,1,fin);
			inSize++;
		}
	}else{/* C array input */
		for(i=0;i<skipArrays+1;i++)
			while(fgetc(fin) != '{' && !feof(fin));/* eat everything up to the beginning of the array data */

		if(feof(fin)){/* got to the end of the file without seeing the opening bracket of the array */
			printf("ERROR: Did not find opening bracket '{'\n");
			return -1;
		}

		if(transformType == 1){//patches input?
			char lastc = 0;
			while(!feof(fin)){
				char c = fgetc(fin);

				if(c == ' ' || c == '\t' || c == '\r' || (c == '\n' && lastc == '\n'))
					continue;
				if(c == '}')
					break;
				lastc = c;
				tempBuf[inSize++] = c;
			}
			tempBuf[inSize] = '\0';
			inSize = 0;
			char *match;
			while(1){//replace command defines with numbers...
				match = strstr(tempBuf, "PC_ENV_SPEED");
				if(match != NULL){memmove(match, (const char *)"0x00        ", 12);continue;}

				match = strstr(tempBuf, "PC_NOISE_PARAMS");
				if(match != NULL){memmove(match, (const char *)"0x01           ", 15);continue;}

				match = strstr(tempBuf, "PC_ENV_WAVE");
				if(match != NULL){memmove(match, (const char *)"0x02       ", 11);continue;}

				match = strstr(tempBuf, "PC_NOTE_UP");
				if(match != NULL){memmove(match, (const char *)"0x03      ", 10);continue;}

				match = strstr(tempBuf, "PC_NOTE_DOWN");
				if(match != NULL){memmove(match, (const char *)"0x04        ", 12);continue;}

				match = strstr(tempBuf, "PC_NOTE_CUT");
				if(match != NULL){memmove(match, (const char *)"0x05       ", 11);continue;}

				match = strstr(tempBuf, "PC_NOTE_HOLD");
				if(match != NULL){memmove(match, (const char *)"0x06        ", 12);continue;}

				match = strstr(tempBuf, "PC_ENV_VOL");
				if(match != NULL){memmove(match, (const char *)"0x07      ", 10);continue;}

				match = strstr(tempBuf, "PC_PITCH");
				if(match != NULL){memmove(match, (const char *)"0x08    ", 8);continue;}

				match = strstr(tempBuf, "PC_TREMOLO_LEVEL");
				if(match != NULL){memmove(match, (const char *)"0x09            ", 16);continue;}

				match = strstr(tempBuf, "PC_TREMOLO_RATE");
				if(match != NULL){memmove(match, (const char *)"0x0A           ", 15);continue;}

				match = strstr(tempBuf, "PC_SLIDE");
				if(match != NULL){memmove(match, (const char *)"0x0B    ", 8);continue;}

				match = strstr(tempBuf, "PC_SLIDE_SPEED");
				if(match != NULL){memmove(match, (const char *)"0x0C          ", 14);continue;}

				match = strstr(tempBuf, "PC_LOOP_START");
				if(match != NULL){memmove(match, (const char *)"0x0D         ", 13);continue;}

				match = strstr(tempBuf, "PC_LOOP_END");
				if(match != NULL){memmove(match, (const char *)"0x0E       ", 11);continue;}

				match = strstr(tempBuf, "PATCH_END");
				if(match != NULL){memmove(match, (const char *)"0xFF     ", 9);continue;}
				break;
			}
			char *csv = strtok(tempBuf, ",");
			int pstep = 0;
			while(csv != NULL){

				if(skipBytes){//user needs to get this correct so PATCH_END detection works..
					skipBytes--;
				}else{
					i = strtol(csv, NULL, 0);
					inBuf[inSize++] = (i & 0xFF);
					if(inSize == sizeof(tempBuf)){
						printf("ERROR: Found no patch end\n");
						return -4;
					}
					if(i == 0xFF && ((pstep%3) == 2))//PATCH_END, ignore anything after
						break;
				}
				csv = strtok(NULL, ",");
				pstep++;
			}
		}else{//standard input transform(there may be a specific output transform later..)
			while(fscanf(fin," 0%*[xX]%x , ",&i) == 1){
				if(skipBytes)
					skipBytes--;
				else
					inBuf[inSize++] = (i & 0xFF);
			}
		}
		if(skipBytes){
			printf("ERROR: Read garbage while skipping bytes(or EOF)\n");
			return -2;
		}
	}

	inBuf[inSize] = '\0';
	i = 0;
	j = 0;
	FILE *adpcmDebugFile;
	if(adpcmDebug){
		char adpcmDebugFileName[512];
		snprintf(adpcmDebugFileName, 512-2, "dconvert-adpcm-debug-line%d.raw", cfgLine); 
		adpcmDebugFile = fopen(adpcmDebugFileName,"wb");
	}
	if(transformType == 5)//1bit ADPCM
		slope = 2;
	else if(transformType == 6)//2bit ADPCM
		slope = 2;
	else if(transformType == 7)//4bit ADPCM
		slope = 7;

	while(i < inSize){
		if(i > sizeof(outBuf)-10){
			printf("Error: outBuf out of bounds\n");
			return -3;
		}

		if(transformType == 0){//raw 1:1 write

			outBuf[outSize++] = inBuf[i++];

		}else if(transformType > 0 && transformType < 5){

			printf("ERROR: transformType [%d] is reserved\n", transformType);
			return -4;

		}else if(transformType == 5){//1bit "Precedent Scaling"

			samples = 0;
			for(j=0;j<8;j++){
				target = inBuf[i++];
				if(i == inSize){//repeat last sample if we don't end on an even multiple
					for(k=i;k<i+8;k++)
						inBuf[k] = inBuf[i-1];
				}

				samples >>= 1;
				if(accum < target){
					samples |= 0b10000000;
					accum += slope;
					up_count++;
					down_count = 0;
				}else{
					accum -= slope;
					down_count++;
					up_count = 0;
				}
				if(++neutral_count == 6){
					up_count = down_count = neutral_count = 0;
					if(slope)
						slope--;
				}
				//lookbehind, "Precedent Scaling"
				if(up_count == 3 || down_count == 3){
					up_count = down_count = neutral_count = 0;
					if(slope < 16)
						slope+=2;
				}
				if(adpcmDebug)
					fputc(accum,adpcmDebugFile);
			}
			outBuf[outSize++] = samples;

		}else if(transformType == 6){//2bit ADPCM, "Precedent Scaling"
			samples = 0;
			for(j=0;j<4;j++){
				target = inBuf[i++];
				if(i == inSize){//repeat last sample if we don't end on an even multiple
					for(k=i;k<i+4;k++)
						inBuf[k] = inBuf[i-1];
				}

				samples >>= 2;
				if(accum < target){
					samples |= 0b10000000;
					accum += slope;
					up_count++;
					down_count = 0;
				}else{
					accum -= slope;
					down_count++;
					up_count = 0;
				}
				if(accum < target){
					samples |= 0b01000000;
					accum += slope/2;
					up_count++;
					down_count = 0;
				}else{
					accum -= slope/2;
					down_count++;
					up_count = 0;
				}
				if(++neutral_count == 4){
					up_count = down_count = neutral_count = 0;
					if(slope>2)
						slope-=1;
				}
				//lookbehind "Precedent Scaling"
				if(up_count == 2 || down_count == 2){
					up_count = down_count = neutral_count = 0;
					if(slope < 16)
						slope++;
				}
				if(adpcmDebug)
					fputc(accum,adpcmDebugFile);//output simulated sample
			}
			outBuf[outSize++] = samples;
		}else if(transformType == 7){//4bit ADPCM, "Precedent Scaling"
			samples = 0;

			for(j=0;j<2;j++){
				target = inBuf[i++];
				if(i == inSize){//repeat last sample if we don't end on an even multiple
					for(k=i;k<i+2;k++)
						inBuf[k] = inBuf[i-1];
				}

				samples >>= 4;
				if(accum < target){
					samples |= 10000000;
					accum += slope;
					up_count++;
					down_count = 0;
				}else{
					accum -= slope;
					down_count++;
					up_count = 0;
				}
				if(accum < target){
					samples |= 10000000;
					accum += slope/2;
					up_count++;
					down_count = 0;
				}else{
					accum -= slope/2;
					down_count++;
					up_count = 0;
				}
				if(accum < target){
					samples |= 10000000;
					accum += slope/4;
					up_count++;
					down_count = 0;
				}else{
					accum -= slope/4;
					down_count++;
					up_count = 0;
				}
				if(accum < target){
					samples |= 10000000;
					accum += slope/8;
					up_count++;
					down_count = 0;
				}else{
					accum -= slope/8;
					down_count++;
					up_count = 0;
				}

				if(++neutral_count > 2){
					up_count = down_count = neutral_count = 0;
					if(slope>4)
						slope--;
				}
				//lookbehind "Precedent Scaling"
				if(up_count > 3 || down_count > 3){
					up_count = down_count = neutral_count = 0;
					if(slope < 24)
						slope+=2;
				}
				if(adpcmDebug)
					fputc(accum,adpcmDebugFile);//output simulated sample
			}
			outBuf[outSize++] = samples;
		}else{//unknown output conversion
			printf("Error: Unknown transform type [%d]\n", transformType);
			return -5;
		}
	}

	padBytes = 0;
	if(doPad){/* pad out the size to fill the full sector */
		while(outSize%512){
			outBuf[outSize++] = 0x00;/* interpreted as silence if reading PCM too far */
			padBytes++;
		}
	}

	if(asBinOut){
		if(dirOff >= 0){//user can omit directory information by passing a negative offset
			fseek(fout,dirOff,SEEK_SET);/* write the directory entry for this song */
			/* the byte order matches what is expected by SpiRamReadU32() */
			fputc(((unsigned char)(fileOff>>0)&0xFF),fout);
			fputc(((unsigned char)(fileOff>>8)&0xFF),fout);
			fputc(((unsigned char)(fileOff>>16)&0xFF),fout);
			fputc(((unsigned char)(fileOff>>24)&0xFF),fout);

			dirOff += 4;
		}


		fseek(fout,fileOff,SEEK_SET);
		if(doLength){/* prepend the total data size before the song data(useful for loading to SPI ram) */
			fputc((outSize>>8)&0xFF,fout);
			fputc((outSize>>0)&0xFF,fout);
			fileOff += 2;
		}
		for(i=0;i<outSize;i++){/* output the actual data, text or binary, and any offset were already setup prior */
			fputc(outBuf[i],fout);
			fileOff++;
		}

	}else{/* C array */
		fprintf(fout,"const char %s[] PROGMEM = {/* %s(array %d) */\n",arrayName,finName,skipArrays);
		w = 0;
		for(i=0;i<outSize-padBytes;i++){/* user added padding for C version? Seems no use, assume it is in error and override. */
			if(!doDebug)
				fprintf(fout,"0x%02X,",outBuf[i]);
			else{
				fprintf(fout,"0b");
				for(j=0;j<8;j++)
					fputc(((outBuf[i]<<j)&128) ? '1':'0',fout);
				fputc(',',fout);
				w = 100;
			}
			if(++w > 15){
				w = 0;
				fprintf(fout,"\n");
			}
			flashCost++;
		}
		if(w != 0)/* make formatting nicer */
			fprintf(fout,"\n");
		fprintf(fout,"};/* %ld bytes total */\n\n",flashCost);
		totalFlashCost += flashCost;
	}
	/* display statistics */
	printf("\t\tInput data size: %d\n",inSize);
	printf("\t\tOutput data size: %d\n",outSize);
	if(adpcmDebug)
		fclose(adpcmDebugFile);
	return 1;
}
