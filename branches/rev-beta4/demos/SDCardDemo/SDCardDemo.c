/*
 *  SD/MMC Card reader demo
 *  Copyright (C) 2008,2009 David Etherton, Eric Anderton
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

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <mmc_if.h>
#include <fat.h>

extern unsigned char gfx[] PROGMEM;

enum{
    MODE_DUMP,
    MODE_BROWSE,
   // MODE_FILE,
    MODE_MAX
};

// globals
unsigned char mode = MODE_BROWSE;
unsigned int btnPressed;
unsigned int btnReleased;
unsigned int btnHeld;
unsigned char buffer[512];

void detectSD(){
    unsigned char temp;

    ClearVram();
    Print(1,1,PSTR("DETECTING MMC..."));
    do { 
      	WaitVsync(2); temp = mmc_init();
   		Print(1,2,temp? PSTR("Init FAILED") : PSTR("Init Passed")); 
    } while (temp);

    Print(1,3,PSTR("Initalizing FAT16    "));
    do {
        WaitVsync(2); temp = InitFAT(buffer);
        Print(1,4,temp != FAT_OK? PSTR("FAT Init FAILED") : PSTR("FAT Init Passed")); 
    } while (temp != FAT_OK);    
}

char translate_char(unsigned char ch)
{
	if(ch> 126 || ch <= 32) return '.';
    return ch;
}

// hex dump vars
long sector = 0;
long oldSector = 0xFFFF;
int offset = 0;
long bookmark = 0;

void PrintHexLong(char x,char y,long value){
	PrintHexByte(x,y,  value>>24);
	PrintHexByte(x+2,y,(value>>16)&0xFF);
	PrintHexByte(x+4,y,(value>>8)&0xFF);
	PrintHexByte(x+6,y,value&0xff);
}

void doModeDump()
{
	unsigned char row, col;
            
    // Sector select keys
    if ((btnHeld & BTN_SL) && sector)
        sector-=2;
    else if (btnHeld & BTN_SR)
        sector+=2;
        
    else if ((btnPressed & BTN_LEFT) && sector)
        --sector;
    else if (btnPressed & BTN_RIGHT)
        ++sector;
        
    else if (btnPressed & BTN_A)
        sector = 0;
    else if (btnPressed & BTN_B)
        sector = bootRecordSector;
        
    else if (btnPressed & BTN_X)
        bookmark = sector;
    else if (btnPressed & BTN_Y)
        sector = bookmark;
        
    // reread sector if either shoulder button was pressed (and wait for release)
    if (oldSector != sector) {
        mmc_readsector(sector,buffer);
    }
    
    oldSector = sector;

    // scrolling
    if ((btnHeld & BTN_DOWN) && offset < 512-(20*8))
        offset+=8;
    else if ((btnHeld & BTN_UP) && offset)
        offset-=8;
        

    // redraw display if any buttons were pressed
    // update display
    Print(1,0,PSTR("Hex Dump"));
    Print(1,1,PSTR("Sector:"));
    PrintHexLong(9,1,sector);
    Print(18,1,PSTR("Bookmark:"));
    PrintHexLong(28,1,bookmark);
    for (row=0; row<20; row++)
    {
        PrintHexInt(1,row+3,offset+(row<<3));
        for (col=0; col<8; col++)
            PrintHexByte(col * 3 + 7,row+3,buffer[offset+(row<<3)+col]);
        for (col=0; col<8; col++)
            PrintChar(31+col,row+3,translate_char(buffer[offset+(row<<3)+col]));
    }
    Print(1,24,PSTR("Press [START] for help."));
}

//browser vars


#define HILIGHT_OFFSET 96

void Invert(char x,char y,char chars){
    unsigned int* v = vram;
    v += x+(y*VRAM_TILES_H);
    for(int i=0; i<chars; i++,v++){
        *v += (HILIGHT_OFFSET*TILE_HEIGHT*TILE_WIDTH);
    }
}

#define BROWSER_START_LINE 4

unsigned char dirtyDisplay = 1;
unsigned char verboseMode = 1;
unsigned int browserStartRow = 0;
unsigned int browserHighlightRow = 0;
unsigned long currentDirectorySector = 0;
unsigned char displayedRows = 0;

struct FileDisplayEntry{
    unsigned int firstCluster;
    unsigned char isDirectory;
};

struct FileDisplayEntry browserEntries[16];

void doModeBrowse(){
    int i,j,k,y;
    char ch;

    // load up the root directory for browsing
    //TODO: replace with progressive scan of directory sector(s) instead.
    
    if(btnHeld & BTN_DOWN){
        if(browserHighlightRow != displayedRows-1) browserHighlightRow++;
        dirtyDisplay = 1;
    }
    else if(btnHeld & BTN_UP){
        if(browserHighlightRow != 0) browserHighlightRow--;
        dirtyDisplay = 1;
    }
    else if(btnPressed & BTN_B){
        if(verboseMode) verboseMode = 0;
        else verboseMode = 1;
        dirtyDisplay = 1;
    }
    else if(btnPressed & BTN_A){
        if(browserEntries[browserHighlightRow].isDirectory){
            int cluster = (browserEntries[browserHighlightRow].firstCluster);
            currentDirectorySector = rootDirTableSector + ((cluster-1) * sectorsPerCluster);
            dirtyDisplay = 1;
        }
    }
    else if(btnPressed & BTN_X){
        sector = currentDirectorySector;
        mode = MODE_DUMP;
        ClearVram();
        dirtyDisplay = 1;
        return;
    }
    
    if(dirtyDisplay){
        ClearVram();
        Print(1,0,PSTR("File Browser"));
        
        if(currentDirectorySector == 0){
            currentDirectorySector = rootDirTableSector;
        }
        mmc_readsector(currentDirectorySector, buffer);
        DirectoryTableEntry *dirEntry = (DirectoryTableEntry*)(buffer);
        
        Print(1,2,PSTR("Current Sector:"));
        PrintHexLong(18,2,currentDirectorySector);
        
        y = BROWSER_START_LINE;
        displayedRows = 0;
        for(i=0; i<16; i++,dirEntry++,y++){
            if(dirEntry->filename[0] == 0x00) break; // no more data
            if(dirEntry->filename[0] == 0xE5){
                y--;
                continue; // deleted entry
            }       
                        
            if(dirEntry->fileAttributes == (FAT_ATTR_READONLY|FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME)){
                if(!verboseMode){
                    y--;
                    continue;
                }
                Print(17,y,PSTR("LFN Entry"));
            }
            else{
                if(dirEntry->fileAttributes & FAT_ATTR_VOLUME){
                    if(!verboseMode){
                        y--;
                        continue;
                    }
                    Print(17,y,PSTR("Volume"));
                }
                if(dirEntry->fileAttributes & FAT_ATTR_DIRECTORY){
                    Print(17,y,PSTR("<DIR>"));
                }
                if(dirEntry->fileAttributes & FAT_ATTR_ARCHIVE){
                    if(verboseMode){
                        Print(17,y,PSTR("Archive"));
                    }
                }
            }
            
            browserEntries[displayedRows].firstCluster = dirEntry->firstCluster;
            browserEntries[displayedRows].isDirectory = dirEntry->fileAttributes & FAT_ATTR_DIRECTORY;
            displayedRows++;
                        
            PrintHexByte(27,y,dirEntry->fileAttributes);
            PrintHexLong(30,y,dirEntry->fileSize);
            
            // print the filename
            for(j=0; j<8; j++){
                ch = dirEntry->filename[j];
                if(ch == 0x20) break;
                PrintChar(1+j,y,translate_char(ch));
            }
            
            // dot and extension
            if(dirEntry->extension[0] == 0x20) continue;
            PrintChar(1+j,y,'.');
            for(k=0; k<3; k++){
                ch = dirEntry->extension[k];
                if(ch == 0x20) break;
                PrintChar(2+j+k,y,translate_char(ch));
            }
        }
        
        Invert(0,browserHighlightRow+BROWSER_START_LINE,40);
        dirtyDisplay = 0;
        Print(1,24,PSTR("Press [START] for help."));
    }
}

/*
    Notes on reading fragmented files.
    
    0) directory entry w/start cluster number
    1) calculate where in the cluster table that number exists
    2) read next cluster number
    3) read current cluster from disk
    4) current = next 
    5) goto 1 
    
    - Technique generates a lot of overhead for reading from SD.
    - A way to pull a handful of bytes from the middle of a sector
    will be needed to avoid the need for additional buffers
    - Possibly buffer up additional cluster references if they're
    within the same sector, as a kind of cache
*/
void doModeFile(){
    Print(1,0,PSTR("File View"));
    //TODO: works like hex view    
}

void doHelpDump(){
    Print(1,1,PSTR("Help - Hex Dump"));
    Print(1,3,PSTR("Up/Down - Scroll View"));
    Print(1,4,PSTR("Left/Right - Select Sector (Slow)"));
    Print(1,5,PSTR("L/R Shoulder - Select Sector (Fast)"));
    Print(1,6,PSTR("A - Go to MBR (Sector 0)"));
    Print(1,7,PSTR("B - Go to Boot Record"));
    Print(1,8,PSTR("X - Set Bookmark"));
    Print(1,9,PSTR("Y - Go to Bookmark"));
    
    Print(1,11,PSTR("Start - Back to Hex Dump"));
    Print(1,12,PSTR("Select - Change Mode"));
}

void doHelpBrowse(){
    Print(1,1,PSTR("Help - File Browser"));
    Print(1,3,PSTR("Up/Down - Scroll File Listing"));
    
  //  Print(1,5,PSTR("L/R Shoulder - History Back/Forward"));
    Print(1,6,PSTR("A - View File/Open Directory"));
    Print(1,7,PSTR("B - Toggle Verbose View"));
    Print(1,8,PSTR("X - View Sector in Hex"));
    //Print(1,9,PSTR("Y - Go to Bookmark"));
    
    Print(1,11,PSTR("Start - Back to Browser"));
    Print(1,12,PSTR("Select - Change Mode"));
}

void doHelpFile(){
    Print(1,1,PSTR("Help - File View"));
    Print(1,3,PSTR("Up/Down - Scroll View"));
    Print(1,4,PSTR("Left/Right - Toggle Hex/Text view"));
    Print(1,5,PSTR("L/R Shoulder - Scroll View (Fast)"));
    Print(1,6,PSTR("A - Go to start"));
    Print(1,7,PSTR("B - Go to end"));
    Print(1,8,PSTR("X - Set Bookmark"));
    Print(1,9,PSTR("Y - Go to Bookmark"));
    
    Print(1,11,PSTR("Start - Back to File View"));
    Print(1,12,PSTR("Select - Change Mode"));
}

int main(){
    char helpMode = 0;
    unsigned int btnPrev = 0;
    
    SetFontTable(gfx);
    ClearVram();
    
    detectSD();
    ClearVram();
    
  	while (1)
  	{
      	btnHeld = ReadJoypad(0);
        btnPressed = btnHeld & (btnHeld ^ btnPrev);
        btnReleased = btnPrev & (btnHeld ^ btnPrev);
      	WaitVsync(1);
        
        if(btnPressed & BTN_START){
            if(helpMode){
                helpMode = 0;
            }
            else{
                helpMode = 1;
            }
            dirtyDisplay = 1;
            ClearVram();
        }
        
        if(btnPressed & BTN_SELECT){
            mode++;
            if(mode == MODE_MAX){
                mode = 0;
            }
            dirtyDisplay = 1;
            ClearVram();
        }

        switch(mode){
        case MODE_DUMP:
            if(helpMode) doHelpDump();
            else doModeDump();
            break;
        case MODE_BROWSE:
            if(helpMode) doHelpBrowse();
            else doModeBrowse();
            break;
        }
        
        btnPrev = btnHeld;
   }

} 
