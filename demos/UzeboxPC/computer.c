/*
 * computer.c
 *
 * Initializes the hardware machine, launches the emulated 8080 CPU,
 * and emulates the floppy controller and port operations.
 *
 * https://www.autometer.de/unix4fun/z80pack/
 *
 *  Created on: 3 févr. 2020
 *
 */
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <petitfatfs/pff.h>
#include <petitfatfs/diskio.h>
#include "uzenet.h"
#include "computer.h"
#include "telnet.h"
#include "uzenet_setup.h"
#include "Emulator8080.h"
#include "terminal.h"
#include "spiram_custom.h"

#define RAM_SIZE_64K	0
#define RAM_SIZE_128K	1
#define RAM_SIZE_ERR	0xff

u8 _spiram_start();
void _open_file(u8 drive);
extern u8 cpu_exec(u16 insts);
extern void cpu_init(u16 startpc);
extern void cpu_set_ram_size(u8 size);

//Default files names to use for the CP/M logical disks
//disk0=A:,disk1=B:,disk2=C:,disk3=D:
//If a files named DISK.CFG is found on the SD card, it's content
//will override the default image names.
char disk0[] = "CPMDISK0.DSK";
char disk1[] = "CPMDISK1.DSK";
char disk2[] = "CPMDISK2.DSK";
char disk3[] = "CPMDISK3.DSK";
char diskcfg[] = "DISK.CFG";
char* disks[] = {disk0,disk1,disk2,disk3};

FATFS fs;          // Work area (file system object) for the volume
BYTE dmabuf[512];  // Buffer for file sector read for the 8080 emulator sector read/write from/to SD
WORD br;           // File read count
FRESULT res;       // Petit FatFs function common result code
s8 curr_drive=-1;

void halt(u8 errorCode){
	printf_P(PSTR("Exit code: %d\r\nCPU Halted."),errorCode);
	while(1);
}

//Initialize the floppy disk controller
void _init_fdc(){
    res=pf_mount(&fs);
    if(res){
    	WaitVsync(30);
    	res=pf_mount(&fs);
    	if(res){
    	   	printf_P(PSTR("\r\nDisk controller failed. "));
			halt(res);
    	}
    }
}

void computer_Boot(void){
	printf_P(PSTR("Uzebox Computer (c) 2023 Uze\r\nCPU: ATmega644 @ 28Mhz\r\nPhysical SPI RAM Installed: "));

	//Initialize the SPI RAM chip
	u8 ramSize=_spiram_start();
	if(ramSize==RAM_SIZE_ERR){
		printf_P(PSTR("Detection failed, CPU halted.\r\n"));
		while(1);
	}

	if(ramSize==RAM_SIZE_64K){
		printf_P(PSTR("64K\r\n"));
	}else{
		printf_P(PSTR("128K\r\n"));
	}

	//TODO: detect
	printf_P(PSTR("Network adapter: ESP8266\r\nInitializing disk controller..."));

	//Reset the spi ram to zero
	SpiRamSeqWriteStart(0,0);


	for(long i=0;i<0x10000;i++){
		SpiRamSeqWriteU16(0);
	}
	SpiRamSeqWriteEnd();



	//initialize the SD card
	_init_fdc();

	//check if a drive config file exists and load the image files to use
	res=pf_open(diskcfg);
	if(res==FR_OK){
		res=pf_read(dmabuf, 512, &br);
		if(res==FR_OK){
			char* disk = strtok((char*)dmabuf, "\r\n");
			if(disk==NULL)return;
			strcpy(disks[0],disk+2);
			for(u8 i=1;i<4;i++){
				char* disk = strtok(NULL, "\r\n");
				if(disk==NULL)break;
				strcpy(disks[i],disk+2);
			}
		}
	}else{
		printf_P(PSTR("\r\nCan't find "));
		printf(diskcfg);
		while(1);
	}

	//mount default drive
	_open_file(0);

	printf_P(PSTR("\r\nBooting disk (0)..."));

	//Load CP/M boot loader from first sector of disk
	//It will then load the rest of CPM.
	pf_lseek(0);
	u16 spiAddr=0;
	for(int i=0;i<1;i++){ //load 1 sector
		res=pf_read(dmabuf, 128, &br);
		if(res!=0){
    		printf_P(PSTR("Error reading SD!"),dmabuf);
    		while(1);
		}
		SpiRamSeqWriteStart(0,spiAddr);
		for(int j=0;j<128;j++){
			SpiRamSeqWriteU8(dmabuf[j]);
		}
		SpiRamSeqWriteEnd();

		spiAddr+=128;
	}

	printf_P(PSTR("first sector loaded.\r\nBooting CP/M...\r\n\r\n"));

	cpu_init(0);

	cpu_registers.breakpoint=0x100;
	//cpu_registers.pc=0x100;

	#ifdef EMULATOR_DEBUG
		debug_Init();
	#endif

	while(1){

		#ifdef EMULATOR_DEBUG
			while(debug_Process()==DEBUG_OP_STANDBY);
		#endif




		cpu_exec(0);

		#ifdef EMULATOR_DEBUG
			debug_Breakpoint();
			cpu_registers.breakpoint=0;
		#endif
	}
}

//Function to init the SPI RAM
//for some reason the one from spiram.c
//doesn't work on my hardware.
u8 retval,v;
u16 ind;
u8 _spiram_start(){

	DDRA &= ~(1<<PA4);	//disable SPI RAM

	//enable SPI in 2X mode
	SPCR=(1<<SPE)+(1<<MSTR);
	SPSR=(1<<SPI2X);
	DDRB|=(1<<PB7)+(1<<PB5);
	PORTA|=(1<<PA4);
	DDRA|=(1<<PA4);  //enable SPI RAM

	SpiRamSetSize(0); //check if 64k
	SpiRamWriteU8(0,0xcccc,0xcc);
	retval=SpiRamReadU8(0,0xcccc);
	if(retval==0xcc){
		cpu_set_ram_size(0);
		return RAM_SIZE_64K;
	}

	SpiRamSetSize(1); //check 128K
	SpiRamWriteU8(1,0xaaaa,0xaa);
	retval=SpiRamReadU8(1,0xaaaa);
	if(retval==0xaa){
		cpu_set_ram_size(1);
		return RAM_SIZE_128K;
	}

	return RAM_SIZE_ERR; //status;

}

void _open_file(u8 drive){
	char filename[13];
	//strcpy_P(filename, (PGM_P)pgm_read_word(&(disks[drive])));
	strcpy(filename, disks[drive]);
	res=pf_open(filename);
	if(res){
		//retry once
		_init_fdc();
		res=pf_open(filename);
		if(res){
			printf_P(PSTR("\r\nEmulator error: Can't open image for disk %c: (%s) on SD card."),(drive+'A'),filename);
			while(1);
		}
	}
	curr_drive=drive;
}


void computer_VsyncCallback(void){
}

bool start=true;
u16 i=0;
u8 c;

void computer_RunNativeApp(u8 appId){
	dmabuf[0]=0;

	//Get the CP/M command line buffer to dmabuf
	//the first byte is the commands line length followed by at least one space
	SpiRamSeqReadStart(0,0x80);
	u8 commandLen=SpiRamSeqReadU8();
	if(commandLen>0){
		//Copy emulated ram to the dma buffer, skipping all trailing spaces
		u8 pos=0;
		for(i=0;i<commandLen;i++){
			c=SpiRamSeqReadU8();
			if(c!=32){
				c=tolower(c);
				dmabuf[pos++]=c;
			}
		}
		dmabuf[pos]=0;
	}
	SpiRamSeqReadEnd();

	//Execute the selected native application
	switch (appId){
		case 0:	//telnet client
			telnet_CommandLine((char*)dmabuf);
			break;
		case 1:
			uzenet_Setup();
			break;
		default:
			printf_P(PSTR("\r\nError: Unknown native application with ID:%d\r\n"),appId, res);
			break;

	}

	//remount last drive in case native app used the SD card
	_open_file(curr_drive);

}

//************************************************************************
// Send data to port
//************************************************************************
void computer_PortOut(u8 port,u8 value){
	if(port==0x13){
		computer_RunNativeApp(value);
	}
}

//************************************************************************
//* Receive data from port
//************************************************************************
u8 computer_PortIn(u8 port){
	return 0;
}

u8 computer_getTerminalKey(){
	return terminal_GetChar();
}

bool computer_hasTerminalKey(){
	return terminal_HasChar();
}
