/*
(The MIT License)

Copyright (c) 2008,2009, David Etherton, Eric Anderton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "uzem.h"
#include "avr8.h"
#include "gdbserver.h"
#include "uzerom.h"
#include <getopt.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


static const struct option longopts[] ={
    { "help"       , no_argument      , NULL, 'h' },
    { "nosound"    , no_argument      , NULL, 'n' },
    { "fullscreen" , no_argument      , NULL, 'f' },
    { "hwsurface"  , no_argument      , NULL, 'w' },
    { "nodoublebuf", no_argument      , NULL, 'x' },
    { "mouse"      , no_argument      , NULL, 'm' },
    { "2p"         , no_argument      , NULL, '2' },
    { "img"        , required_argument, NULL, 'g' },
    { "mbr"        , no_argument      , NULL, 'r' },
    { "eeprom"     , required_argument, NULL, 'e' },
    { "pgm"        , required_argument, NULL, 'p' },
    { "boot"       , no_argument,       NULL, 'b' },
    { "gdbserver"  , no_argument,       NULL, 'd' },
    { "port"       , required_argument, NULL, 't' },
    { "capture"    , no_argument,       NULL, 'c' },
    { "loadcap"    , no_argument,       NULL, 'l' },

#if defined(__WIN32__)
    { "sd"         , required_argument, NULL, 's' },
#endif 
    {NULL          , 0                , NULL, 0}
};

   static const char* shortopts = "hnfclwxm2re:p:bdt:k:s:v";

#define printerr(fmt,...) fprintf(stderr,fmt,##__VA_ARGS__)

void showHelp(char* programName){
    printerr("Uzebox Emulator " VERSION "\n");
    printerr("- Runs an Uzebox game file in either .hex or .uze format.\n");
    printerr("Usage:\n");
    printerr("\t%s [OPTIONS] GAMEFILE\n",programName);
    printerr("Options:\n");
    printerr("\t--help -h           Show this help screen\n");
    printerr("\t--nosound  -n       Disable sound playback\n");
    printerr("\t--fullscreen -f     Enable full screen\n");
    printerr("\t--hwsurface -w      Use SDL hardware surface (probably slower)\n");
    printerr("\t--nodoublebuf -x    No double buffering\n");
    printerr("\t--mouse -m          Start with emulated mouse enabled\n");
    printerr("\t--2p -2             Start with snes 2p mode enabled\n");
    printerr("\t--sd -s <path>      SD card emulation from contents of path\n");
    printerr("\t--eeprom -e <file>  Use following filename for EEPRROM data (default is eeprom.bin).\n");
    printerr("\t--boot -b           Bootloader mode.  Changes start address to 0xF000.\n");
    printerr("\t--gdbserver -d      Debug mode. Start the built-in gdb support.\n");
    printerr("\t--port -t <port>    Port used by gdb (default 1284).\n");
    printerr("\t--capture -c        Captures controllers data to file.\n");
    printerr("\t--loadcap -l        Load and replays controllers data from file.\n");
}

int ends_with(const char* name, const char* extension, size_t length)
{
 const char* ldot = strrchr(name, '.');
 if (ldot != NULL)
 {
   if (length == 0)
	 length = strlen(extension);
   return strncmp(ldot + 1, extension, length) == 0;
 }
 return 0;
}

// header for use with UzeRom files
RomHeader uzeRomHeader;

int main(int argc,char **argv)
{
	avr8 uzebox;
        
#if defined(__GNUC__) && defined(__WIN32__)
    //HACK: workaround for precompiled SDL libraries that block output to console
	FILE * ctt = fopen("CON", "w" );
	freopen( "CON", "w", stdout );
	freopen( "CON", "w", stderr );
#endif

    // init basic flags before parsing args
	uzebox.sdl_flags = SDL_DOUBLEBUF | SDL_SWSURFACE;

    if(argc == 1) {
        showHelp(argv[0]);
		return 1;
    }

    int opt;
    char* heximage = NULL;

    while((opt = getopt_long(argc, argv,shortopts,longopts,NULL)) != -1) {
        switch(opt) {
        default:
            /* fallthrough */
        case 'h': 
            showHelp(argv[0]);
            return 1;
        case 'n':
			uzebox.enableSound = false;
            break;
        case 'f':
			uzebox.fullscreen = true;
            break;
        case 'w':
			uzebox.sdl_flags = (uzebox.sdl_flags & ~SDL_SWSURFACE) | SDL_HWSURFACE;
            break;
        case 'x':
			uzebox.sdl_flags &= ~SDL_DOUBLEBUF;
            break;
        case 'm':
			uzebox.pad_mode = avr8::SNES_MOUSE;
            break;
        case '2':
			uzebox.pad_mode = avr8::SNES_PAD2;
            break;
        case 'r':
            //TODO: implement MBR emulation option
            break;
        case 's':
            uzebox.SDpath = optarg;
            break;
        case 'e':
            uzebox.eepromFile=optarg;
            break;
        case 'b':
            uzebox.pc = 0x7800;//0xF000; //set start for boot image
            break;
        case 'c':
            uzebox.captureMode=CAPTURE_WRITE;
            break;
        case 'l':
            uzebox.captureMode=CAPTURE_READ;
            break;
        case 'd':
            uzebox.enableGdb = true;
            break;
        case 't':
            long port = strtol(optarg,NULL,10);
            if ((port == LONG_MAX) || (port == LONG_MIN) || (port < 0) || port > 65535)
            	uzebox.gdbPort = 0;
	    else
            	uzebox.gdbPort = port;
            break;
        }
    }
    
    // get leftovers
    for (int i = optind; i < argc; ++i) {
        if(heximage){
            printerr("Error: HEX file already specified (too many arguments?).\n\n",heximage);
            showHelp(argv[0]);
            return 1;
        }
        else{
            heximage = argv[i];
        }
    }
    
    if (uzebox.gdbPort == 0) {
        printerr("Error: invalid port address.\n\n",uzebox.gdbPort);
        showHelp(argv[0]);
        return 1;
    }

    // start EEPROM emulation if appropriate
    if(uzebox.eepromFile){
        uzebox.LoadEEPROMFile(uzebox.eepromFile);
    }
    
    // attempt to load the hex image
    if(!heximage){
        printerr("Error: No HEX program or boot file specified.\n\n");
        showHelp(argv[0]);
        return 1;
    }
        
     // write hex image
    if(heximage){

    	unsigned char* buffer = (unsigned char*)(uzebox.progmem);

    	strlwr(heximage);
    	if(ends_with(heximage,"uze", 3)){


        	if(isUzeromFile(heximage)){
                printf("-- Loading UzeROM Image --\n");
                if(!loadUzeImage(heximage,&uzeRomHeader,buffer)){
                    printerr("Error: cannot load UzeRom file '%s'.\n\n",heximage);
                    showHelp(argv[0]);
                    return 1;
                }
                // enable mouse support if required
                if(uzeRomHeader.mouse){
                    uzebox.pad_mode = avr8::SNES_MOUSE;
                    printf("Mouse support enabled\n");
                }
            }else{
    			printerr("Error: Cannot load UZE ROM file '%s'. Bad format header?\n\n",heximage);
    			showHelp(argv[0]);
    			return 1;
            }

    	}else{
            printf("Loading Hex Image...\n");
            if(!loadHex(heximage,buffer)){
                printerr("Error: cannot load HEX image '%s'.\n\n",heximage);
                showHelp(argv[0]);
                return 1;
            }
        }



    	//get rom name without extension to build
    	//the capture file name
		char capfname[256];
    	char *pfile = heximage + strlen(heximage);
		for (;; pfile--)
		{
			if ((*pfile == '\\') || (*pfile == '/') || pfile==heximage)
			{
				if(pfile!=heximage)pfile++; //skip the slash character

				for(int i=0;i<256;i++){
					if(*pfile=='.'){
						capfname[i+0]='.';
						capfname[i+1]='c';
						capfname[i+2]='a';
						capfname[i+3]='p';
						capfname[i+4]=0;
						break;
					}
					capfname[i]=*pfile;
					pfile++;
				}
				break;
			}
		}


        if(uzebox.captureMode==CAPTURE_READ)
        {
            uzebox.captureFile=fopen(capfname,"rb");
            if(uzebox.captureFile==0){
            	uzebox.captureMode=CAPTURE_NONE;
            	printerr("Warning: Cannot open capture file %s. Capture replay ignored.\n\n",capfname);
            }else{
            	fseek(uzebox.captureFile, 0L, SEEK_END);
				long fz = ftell(uzebox.captureFile);
				rewind(uzebox.captureFile);
				uzebox.captureData=new u8[fz];
				uzebox.captureSize=fz;
				uzebox.capturePtr=0;
				fread(uzebox.captureData,1,fz,uzebox.captureFile);
				fclose(uzebox.captureFile);
            }
        }
        else if(uzebox.captureMode==CAPTURE_WRITE)
        {
            uzebox.captureFile=fopen(capfname,"wb");
            if(uzebox.captureFile==0){
            	uzebox.captureMode=CAPTURE_NONE;
            	printerr("Error: Cannot open capture file %s.\n\n",capfname);
            	return 1;
            }
        }


        //if user did not specify a path for the sd card, use the rom's path
    	if(uzebox.SDpath == NULL){
    		//extract path
    		char *pfile;
    		pfile = heximage + strlen(heximage);
    		for (;; pfile--)
    		{
    			if ((*pfile == '\\') || (*pfile == '/'))
    			{
    				*pfile=0;
    				pfile = heximage;
    				break;
    			}
    			if (pfile == heximage)
    			{
    				pfile[0] = '.';
    				pfile[1] = '/';
    				pfile[2] = 0;
    				break;
    			}
    		}
    		uzebox.SDpath=pfile;
    		printf("\nUsing HEX path for SD emulation: %s\n",pfile);
    	}


    }
		
	if (uzebox.SDpath != NULL) {
		if (!uzebox.init_sd()) {
			printerr("Error: cannot load directory for SD emulation '%s'.\n\n", uzebox.SDpath);
			showHelp(argv[0]);
			return 1;
		}
	}

	// init the GUI
	if (!uzebox.init_gui()){
        printerr("Error: Failed to init GUI.\n\n");
        showHelp(argv[0]);
		return 1;
    	}

   	if (uzebox.enableGdb == true) {
#if defined(USE_GDBSERVER_DEBUG)
            uzebox.gdb = new GdbServer(&uzebox, uzebox.gdbPort, true, true);
#else
            uzebox.gdb = new GdbServer(&uzebox, uzebox.gdbPort, false, true);
#endif
	}
        else
            uzebox.state = CPU_RUNNING;

   	uzebox.randomSeed=time(NULL);
   	srand(uzebox.randomSeed);	//used for the watchdog timer entropy
	const int cycles=100000000;
	int left, now;
	char caption[128];
	sprintf(caption,"Uzebox Emulator " VERSION " (ESC=quit, F1=help)");

	//to align with AVR Simulator 2 since it has a bug that the first JMP
	//at the reset vector takes only 2 cycles
	uzebox.cycleCounter=-1;

	while (true)
	{
		if (uzebox.fullscreen){
			puts(caption);
        }else{
			SDL_WM_SetCaption(caption, NULL);
        }

		left = cycles;
		now = SDL_GetTicks();
		while (left > 0)
			left -= uzebox.exec();
		
		now = SDL_GetTicks() - now;

		sprintf(caption,"Uzebox Emulator " VERSION " (ESC=quit, F1=help)  %02d.%03d Mhz",cycles/now/1000,(cycles/now)%1000);
	}

	return 0;
}
