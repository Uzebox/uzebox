/*
NOTE THIS IS NOT THE OFFICIAL BRANCH OF THE UZEBOX EMULATOR
PLEASE SEE THE FORUM FOR MORE DETAILS:  http://uzebox.org/forums/

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
#include "uzerom.h"
#include <getopt.h>

//TODO: support for .uze files

static const struct option longopts[] ={
    { "help"       , no_argument      , NULL, 'h' },
    { "bp"         , required_argument, NULL, 'k' },
    { "nosound"    , no_argument      , NULL, 'n' },
    { "fullscreen" , no_argument      , NULL, 'f' },
    { "hwsurface"  , no_argument      , NULL, 'w' },
    { "nodoublebuf", no_argument      , NULL, 'x' },
    { "interlaced" , no_argument      , NULL, 'i' },
    { "mouse"      , no_argument      , NULL, 'm' },
    { "2p"         , no_argument      , NULL, '2' },
    { "img"        , required_argument, NULL, 'g' },
    { "mbr"        , no_argument      , NULL, 'r' },
    { "eeprom"     , required_argument, NULL, 'e' },
    { "pgm"        , required_argument, NULL, 'p' },
    { "boot"       , no_argument,       NULL, 'b' },
    #if defined(__WIN32__)
        { "sd"     , required_argument, NULL, 's' },
    #endif 
    {NULL          , 0                , NULL, 0}
};

#if defined(__WIN32__)
    static const char* shortopts = "hb:nhxim2i:rs:";
#else
    static const char* shortopts = "hb:nhxim2i:r";
#endif

#define printerr(...) fprintf(stderr,__VA_ARGS__)

void showHelp(char* programName){
    printerr("Uzebox Emulator " VERSION "\n");
    printerr("- Runs an Uzebox game file in either .hex or .uze format.\n");
    printerr("Usage:\n");
    printerr("\t%s [OPTIONS] GAMEFILE\n",programName);
    printerr("Options:\n");
    printerr("\t--help -h           Show this help screen\n");
    printerr("\t--bp -k <addr>      Set breakpoint address\n");
    printerr("\t--nosound  -n       Disable sound playback\n");
    printerr("\t--fullscreen -f     Enable full screen\n");
    printerr("\t--hwsurface -w      Use SDL hardware surface (probably slower)\n");
    printerr("\t--nodoublebuf -x    No double buffering\n");
    printerr("\t--interlaced -i     Turn on interlaced rendering\n");
    printerr("\t--mouse -m          Start with emulated mouse enabled\n");
    printerr("\t--2p -2             Start with snes 2p mode enabled\n");
    printerr("\t--img -g <file>     SD card emulation w/image file\n");
    printerr("\t--mbr -r            Enable MBR emulation (use w/--img for images w/o MBR)\n");
    printerr("\t--eeprom -e <file>  Use following file for EEPRROM data on start/stop.\n");
    printerr("\t--boot -b           Bootloader mode.  Changes start address to 0xF000.\n");
    #if defined(__WIN32__)
        printerr("\t--sd -s <letter>    Map drive letter as SD device\n");
    #endif
}

// header for use with UzeRom files
RomHeader uzeRomHeader;

int main(int argc,char **argv)
{
	avr8 uzebox;
	bool disasmOnly = true;
        
#if defined(__GNUC__) && defined(__WIN32__)
    //HACK: workaround for precompiled SDL libraries that block output to console
    freopen( "CONOUT$", "wt", stdout ); 
    freopen( "CONOUT$", "wt", stderr );
#endif

    // init basic flags before parsing args
	uzebox.sdl_flags = SDL_DOUBLEBUF | SDL_SWSURFACE;

    printerr("\nNOTE THIS IS AN EXPERIMENTAL BRANCH OF THE UZEBOX EMULATOR\n");
    printerr("PLEASE SEE THE FORUM FOR MORE DETAILS:  http://uzebox.org/forums\n\n");

    if(argc == 1) {
        showHelp(argv[0]);
		return 1;
    }

    char opt;
    char* heximage = NULL;
    char* sdimage = NULL;
    char* sddrive = NULL;
    char* eepromFile = NULL;
    int bootsize = 0;
    
    while((opt = getopt_long(argc, argv,shortopts,longopts,NULL)) != -1) {
        switch(opt) {
        default:
            /* fallthrough */
        case 'h': 
            showHelp(argv[0]);
            return 1;
        case 'k':
            uzebox.breakpoint = (u16) strtoul(optarg,NULL,16);
            break;
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
		case 'i':
			uzebox.interlaced = true;
            break;
        case 'm':
			uzebox.pad_mode = avr8::SNES_MOUSE;
            break;
        case '2':
			uzebox.pad_mode = avr8::SNES_PAD2;
            break;
        case 'g':
            sdimage = optarg;
            break;
        case 'r':
            //TODO: implement MBR emulation option
            break;
        case 's':
            sddrive = optarg;
            break;
        case 'e':
            eepromFile = optarg;
            break;
        case 'b':
            uzebox.pc = 0xF000; //set start for boot image
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
    
    // establish SD emulation if selected
    if(sdimage && sddrive){
        printerr("Error: Cannot specify both an SD image file and an SD drive letter.\n\n");
        showHelp(argv[0]);
        return 1;
    }
    else if(sddrive){
        uzebox.SDMapDrive(sddrive);
    }
    else if(sdimage){
        uzebox.SDLoadImage(sdimage);
    }
    
    // start EEPROM emulation if appropriate
    if(eepromFile){
        uzebox.LoadEEPROMFile(eepromFile);
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
        }
       // else if(!load_hex(heximage,(unsigned char*)(uzebox.progmem))){
        else{
            printf("-- Loading Hex Image --\n");
            if(!loadHex(heximage,buffer)){
                printerr("Error: cannot load HEX image '%s'.\n\n",heximage);
                showHelp(argv[0]);
                return 1;
            }
        }
    }
    
    // init the GUI
	if (!uzebox.init_gui()){
        printerr("Error: Failed to init GUI.\n\n");
        showHelp(argv[0]);
		return 1;
    }

	while (true)
	{
		const int cycles = 100000000;
		int left = cycles;
		int now = SDL_GetTicks();
		while (left > 0)
			left -= uzebox.exec(disasmOnly,false);
		now = SDL_GetTicks() - now;

		char caption[128];
		sprintf(caption,"Uzebox Emulator " VERSION " (ESC=quit, F1=help)  %02d.%03d Mhz",cycles/now/1000,(cycles/now)%1000);
		if (uzebox.fullscreen){
			puts(caption);
        }
		else{
			SDL_WM_SetCaption(caption, NULL);
        }
		uzebox.exec(disasmOnly,false);
	}

	return 0;
}