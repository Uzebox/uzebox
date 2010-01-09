/*
 *  SD/MMC Card reader demo
 *  Copyright (C) 2008 David Etherton
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

unsigned char buffer[512];
File file;
DirectoryTableEntry dirEntry;
unsigned char temp;

extern void WritePGM(unsigned int addr,unsigned int value);
extern int ReadPGM(unsigned int addr);

int main(){    
    SetFontTable(gfx);    
        
    // MMC INITALIZATION
        
    ClearVram();
    
    Print(1,1,PSTR("DETECTING MMC...     "));
    do { 
        WaitVsync(2); temp = mmc_init();
        Print(1,2,temp? PSTR("MMC Init FAILED") : PSTR("MMC Init Passed")); 
    } while (temp);
    
    // INIT FAT & SD READ
    
    Print(1,1,PSTR("Initalizing FAT16    "));
    do {
        WaitVsync(2); temp = InitFAT(buffer);
        Print(1,3,temp != FAT_OK? PSTR("FAT Init FAILED") : PSTR("FAT Init Passed")); 
    } while (temp != FAT_OK);
    
    // SD WRITE (second sector)
    for(int i=0; i<512; i++){
        buffer[i] = (char)i;
    }
    
    long testlocation = 0x22E6;
    
    if(mmc_writesector(testlocation,buffer) == 0){
        Print(1,5,PSTR("SD Write Passed"));   
    }
    else{
        Print(1,5,PSTR("SD Write Failed")); 
        goto endtest;
    }
    if(mmc_readsector(testlocation,buffer) == 0){
        Print(1,6,PSTR("SD Read Passed"));   
    }
    else{
        Print(1,6,PSTR("SD Read Failed")); 
        goto endtest;
    }
    for(int i=0; i<512; i++){
        char ch = buffer[i];
        char x = (char)i;
        if(ch != x){
            Print(1,7,PSTR("SD Write Verify Failed")); 
            goto endtest;
        }
    }
    Print(1,7,PSTR("SD Write Verify Passed")); 
    
    //TODO: bogus command test
    
    //Open File
    file.firstCluster = 0;
    if(OpenFile(&file)==FAT_OK){
        Print(1,8,PSTR("Opened Root Directory."));
    }
    else{
        Print(1,8,PSTR("Could not open file.")); 
        goto endtest;
    }
    
    //Open Directory File 
    if(OpenDirectoryFile(&file,&file,PSTR("uzebox"),FAT_ATTR_DIRECTORY)==FAT_OK){
        Print(1,9,PSTR("Opened 'uzebox' Directory."));
    }
    else{
        Print(1,9,PSTR("Could not open 'uzebox' directory."));
        goto endtest;
    }
    
    //Create File
    File created; // new file
    if(CreateDirectoryFile(&file,&created,PSTR("foobar.txt"),20000,FAT_ATTR_ARCHIVE)==FAT_OK){
        Print(1,10,PSTR("Created 'foobar.txt'."));
    }
    else{
        Print(1,10,PSTR("Failed to create 'foobar.txt'"));
        goto endtest;
    }
    
    //Read/Write EEPROM
    WriteEeprom(0x00FF,0xFF);

    unsigned char value = ReadEeprom(0x00FF);
    Print(1,11,PSTR("Read EEPROM Passed"));
    
    if(value == 0xFF){
        Print(1,12,PSTR("Write EEPROM Passed"));
    }
    else{
        Print(1,12,PSTR("Write EEPROM Failed"));
        goto endtest;
    }
    
    //Write PGM
    WritePGM(0x7FFF,0xDEAD);

    unsigned int pgmValue = ReadPGM(0xFFFE);
    Print(1,13,PSTR("Read PGM Passed"));
    PrintHexInt(1,19,pgmValue);
    
    if(pgmValue == 0xDEAD){
        Print(1,14,PSTR("Write PGM Passed"));
    }
    else{
        Print(1,14,PSTR("Write PGM Failed"));
        goto endtest;
    }
        
    // END
endtest: 
    Print(1,20,PSTR("End Test"));
  	while (1){
        WaitVsync(1);
    }
} 
