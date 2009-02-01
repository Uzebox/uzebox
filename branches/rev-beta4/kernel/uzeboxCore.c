/*
 *  Uzebox Kernel
 *  Copyright (C) 2008  Alec Bourque
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
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "uzebox.h"




#define Wait200ns() asm volatile("lpm\n\tlpm\n\t");
#define Wait100ns() asm volatile("lpm\n\t");


#if INTRO_LOGO !=0
	#if VIDEO_MODE == 3 
		#include "data/uzeboxlogo_8x8.pic.inc"
		#include "data/uzeboxlogo_8x8.map.inc"
	#else
		#include "data/uzeboxlogo.pic.inc"
		#include "data/uzeboxlogo.map.inc"
	#endif
#endif


#if INTRO_LOGO == 1 

	//Logo "kling" sound
	const char initPatch[] PROGMEM ={	
	0,PC_WAVE,8,
	0,PC_PITCH,85,
	4,PC_PITCH,90,
	0,PC_ENV_SPEED,-8,   
	0,PC_TREMOLO_LEVEL,0x90,     
	0,PC_TREMOLO_RATE,30, 
	50,PC_NOTE_CUT,0,
	0,PATCH_END  
	};

	const struct PatchStruct initPatches[] PROGMEM = 
	{
		{0,NULL,initPatch,0,0},
	};


#elif INTRO_LOGO == 2

	#include "data/logovoice.inc"

	const struct PatchStruct initPatches[] PROGMEM = 
	{
		{0,voice,NULL,sizeof_voice,sizeof_voice},
	};
#endif

void ReadButtons();

extern unsigned char rotate_spr_no;
extern unsigned char sync_phase;
extern unsigned char sync_pulse;
extern unsigned char curr_field;
extern unsigned char  curr_frame;
extern struct TrackStruct tracks[CHANNELS];
extern unsigned char scanline_sprite_buf[];
extern unsigned char burstOffset;
extern unsigned char vsync_phase;
extern volatile unsigned int joypad1_status_lo,joypad2_status_lo,joypad1_status_hi,joypad2_status_hi;
extern unsigned char tileheight, textheight;

bool snesMouseEnabled;

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void Initialize(void) __attribute__((naked)) __attribute__((section(".init8")));

void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();
    return;
}

/**
 * Performs a software reset
 */
void SoftReset(void){        
	wdt_enable(WDTO_15MS);  
	while(1);
}

/*
 This method activates teh code to read the mouse. 
 Currently reading the mouse takes a much a 2.5 scanlines.
*/
void EnableSnesMouse(){
	snesMouseEnabled=true;
}

void logo(){

	#if INTRO_LOGO !=0

		InitMusicPlayer(initPatches);

		#if VIDEO_MODE == 2
			#define LOGO_X_POS 8
			screenSections[0].tileTableAdress=uzeboxlogo;
		#elif VIDEO_MODE==3
			#define LOGO_X_POS 13
			SetTileTable(logo_tileset);
		#else			
			#define LOGO_X_POS 18
			SetTileTable(uzeboxlogo);
			SetFontTable(uzeboxlogo);
		#endif
			
		//draw logo
		ClearVram();
		WaitVsync(15);
		


		#if INTRO_LOGO == 1 
			InitMusicPlayer(initPatches);
			TriggerFx(0,0xff,true);
		#endif

		#if VIDEO_MODE == 3
			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo);
			WaitVsync(3);
			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo2);
			WaitVsync(2);
			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo);
		#else
			DrawMap(LOGO_X_POS,12,map_uzeboxlogo);
			WaitVsync(3);
			DrawMap(LOGO_X_POS,12,map_uzeboxlogo2);
			WaitVsync(2);
			DrawMap(LOGO_X_POS,12,map_uzeboxlogo);
		#endif	

		#if INTRO_LOGO == 2

			SetMasterVolume(0xc0);
			TriggerNote(3,0,16,0xff);


		#endif 
		
		WaitVsync(20);

		#if VIDEO_MODE == 2
			WaitVsync(80);
		#else
			WaitVsync(10);
		#endif

		ClearVram();
		WaitVsync(20);
	#endif

}


/**
 * Called by the assembler initialization routines, should not be called directly.
 */
int i;
void Initialize(void){

	cli();

	//Initialize the mixer buffer
	for(i=0;i<MIX_BANK_SIZE*2;i++){
		mix_buf[i]=0x80;
	}	
	
	mix_pos=mix_buf;
	mix_bank=0;

	for(i=0;i<CHANNELS;i++){
		mixer.channels.all[i].volume=0;
	}

	
	#if MIXER_CHAN4_TYPE == 0
		//initialize LFSR		
		tr4_barrel_lo=1;
		tr4_barrel_hi=1;		
		tr4_params=0b00000001; //15 bits no divider (1)
	#endif

	#if MIDI_IN == ENABLED
		midi_rx_buf_start=0;
		midi_rx_buf_end=0;

		UCSR0B=(1<<RXEN0); //set UART for MIDI in
		UCSR0C=(1<<UCSZ01)+(1<<UCSZ00);
		UBRR0L=56; //31250 bauds (.5% error)
	#endif

	
	//stop timers
	TCCR1B=0;
	TCCR0B=0;
	
	#if VIDEO_MODE == 2

		//clear sprite-per-line buffer
		for(i=0;i<(SCREEN_TILES_H+2)*TILE_WIDTH;i++){
			scanline_sprite_buf[i]=TRANSLUCENT_COLOR;
		}

		//set default to no flickers
		SetSpritesOptions(0);

		//set defaults for main screen section
		for(i=0;i<SCREEN_SECTIONS_COUNT;i++){
			screenSections[i].scrollX=0;
			screenSections[i].scrollY=0;
		
			if(i==0){
				screenSections[i].height=SCREEN_TILES_V*TILE_HEIGHT;
			}else{
				screenSections[i].height=0;
			}
			screenSections[i].vramBaseAdress=vram;
			screenSections[i].wrapLine=0;
			screenSections[i].flags=SCT_PRIORITY_SPR;
		}

		//sprites flicker rotation
		rotate_spr_no=1;

	#endif
	
	#if VIDEO_MODE == 3
		//clear srpites
		for(i=0;i<MAX_SPRITES;i++){
			sprites[i].y=(SCREEN_TILES_V*TILE_HEIGHT);		
		}

	#endif

	#if VIDEO_MODE == 4
	tileheight = 12*16;
	textheight = 4;
	#endif

	//set ports
	DDRC=0xff; //video dac
	DDRB=0xff; //h-sync for ad725
	DDRD=0x80; //audio-out, midi-in

	//setup port A for joypads
	DDRA =0b00001100; //set only control lines as outputs
	PORTA=0b11111011; //activate pullups on the data lines

	//set sync parameters. starts at odd field, in pre-eq pulses, line 1
	sync_phase=SYNC_PHASE_PRE_EQ;
	sync_pulse=SYNC_PRE_EQ_PULSES;

	//clear timers
	TCNT1H=0;
	TCNT1L=0;

	//set sync generator counter on TIMER1
	OCR1AH=HDRIVE_CL_TWICE>>8;
	OCR1AL=HDRIVE_CL_TWICE&0xff;

	TCCR1B=(1<<WGM12)+(1<<CS10);//CTC mode, use OCR1A for match
	TIMSK1=(1<<OCIE1A);			//generate interrupt on match

	//set clock divider counter for AD725 on TIMER0
	//outputs 14.31818Mhz (4FSC)
	TCCR0A=(1<<COM0A0)+(1<<WGM01); //toggle on compare match + CTC
	OCR0A=0; //divide main clock by 2
	TCCR0B=(1<<CS00); //enable timer, no pre-scaler

	//set sound PWM on TIMER2
	TCCR2A=(1<<COM2A1)+(1<<WGM21)+(1<<WGM20); //Fast PWM	
	OCR2A=0; //duty cycle (amplitude)
	TCCR2B=(1<<CS20);  //enable timer, no pre-scaler

	SYNC_PORT=(1<<SYNC_PIN)|(1<<VIDEOCE_PIN); //set sync & chip enable line to hi

	burstOffset=0;
	curr_frame=0;
	vsync_phase=0;
	joypad1_status_hi=0;
	joypad2_status_hi=0;
	snesMouseEnabled=false;

	//enable color correction
	ReadButtons();
	if(ReadJoypad(0)&BTN_B){
		SetColorBurstOffset(4);
	}


	sei();

	logo();
}

void ReadButtons(){
	unsigned int p1ButtonsLo=0,p2ButtonsLo=0;
	unsigned char i;

	//latch controllers
	JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);
	if(snesMouseEnabled){
		WaitUs(1);
	}else{
		Wait200ns();
	}	
	JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));


	//read button states
	for(i=0;i<16;i++){
		
		p1ButtonsLo>>=1;
		p2ButtonsLo>>=1;
	
		//pulse clock pin		
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
		if(snesMouseEnabled){
			WaitUs(5);
		}else{
			Wait200ns();
		}
		
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN))==0) p1ButtonsLo|=(1<<15);
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))==0) p2ButtonsLo|=(1<<15);
		
		JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);
		if(snesMouseEnabled){
			WaitUs(5);
		}else{
			Wait200ns();
		}

	}

	joypad1_status_lo=p1ButtonsLo;
	joypad2_status_lo=p2ButtonsLo;

}

void ReadControllers(){
	unsigned int p1ButtonsHi=0,p2ButtonsHi=0;
	unsigned char i;

	//read the standard buttons
	ReadButtons();

	//read the extended bits. Applies only if the mouse is plugged.
	//if bit 15 of standard word is 1, a mouse is plugged.
	if(joypad1_status_lo&(1<<15) || joypad2_status_lo&(1<<15)){

		WaitUs(1);

		for(i=0;i<16;i++){
		
			p1ButtonsHi<<=1;
			p2ButtonsHi<<=1;
	
			//pulse clock pin		
			JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
			Wait200ns();
			Wait200ns();
		
			if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN))==0) p1ButtonsHi|=1;
			if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))==0) p2ButtonsHi|=1;

			JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);
			WaitUs(8);
		}
		
		joypad1_status_hi=p1ButtonsHi;
		joypad2_status_hi=p2ButtonsHi;

	}

}

unsigned char GetMouseSensitivity(){
	unsigned char sens=-1;

	if(snesMouseEnabled){
		ReadButtons();

		if(joypad1_status_lo&(1<<15)){
			sens=(joypad1_status_lo>>10)&3;
		}else if(joypad2_status_lo&(1<<15)){
			sens=(joypad2_status_lo>>10)&3;
		}

	}

	return sens;
}

bool SetMouseSensitivity(unsigned char value){
	unsigned char i,retries=6;

	if(snesMouseEnabled){	
		while(retries>0){
			
			if(GetMouseSensitivity()==value){
				return true;
			}

			WaitUs(1000);

			for(i=0;i<31;i++){	
				JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);	

				//pulse clock pin		
				JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
				Wait200ns();
				Wait200ns();
				Wait200ns();			
				Wait100ns();			
				JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);

				JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));	
			
				WaitUs(2);
				Wait200ns();
				Wait200ns();
				Wait200ns();			
				Wait100ns();
			}	
			
			retries++;
		}
	}

	return false;
}

//returns true if the EEPROM has been setup to work with the kernel.
bool isEepromFormatted(){
	unsigned id;
	id=ReadEeprom(0)+(ReadEeprom(1)<<8);
	return (id==EEPROM_SIGNATURE);
}

/*
 * Write a data block in the specified block id. If the block does not exist, it is created.
 *
 * Returns: 0 on success or error codes
 */
char EepromWriteBlock(struct EepromBlockStruct *block){
	unsigned char i,nextFreeBlock=0,c;
	unsigned int destAddr=0,id;
	unsigned char *srcPtr=(unsigned char *)block;

	if(!isEepromFormatted()) return EEPROM_ERROR_NOT_FORMATTED;
	if(block->id==EEPROM_FREE_BLOCK || block->id==EEPROM_SIGNATURE) return EEPROM_ERROR_INVALID_BLOCK;

	//scan all blocks and get the adress of that block or the next free one.
	for(i=2;i<64;i++){
		id=ReadEeprom(i*EEPROM_BLOCK_SIZE)+(ReadEeprom((i*EEPROM_BLOCK_SIZE)+1)<<8);
		if(id==block->id){
			destAddr=i*EEPROM_BLOCK_SIZE;
			break;
		}
		if(id==0xffff && nextFreeBlock==0) nextFreeBlock=i;
	}

	if(destAddr==0 && nextFreeBlock==0) return EEPROM_ERROR_FULL;
	if(nextFreeBlock!=0) destAddr=nextFreeBlock*EEPROM_BLOCK_SIZE;

	for(i=0;i<EEPROM_BLOCK_SIZE;i++){
		c=*srcPtr;
		WriteEeprom(destAddr++,c);
		srcPtr++;	
	}
	
	return 0;
}

/*
 * Reads a data block in the specified structure.
 *
 * Returns: 0 on success or error codes
 */
char EepromReadBlock(unsigned int blockId,struct EepromBlockStruct *block){
	unsigned char i;
	unsigned int destAddr=0,id;
	unsigned char *destPtr=(unsigned char *)block;

	if(!isEepromFormatted()) return EEPROM_ERROR_NOT_FORMATTED;
	if(blockId==EEPROM_FREE_BLOCK) return EEPROM_ERROR_INVALID_BLOCK;

	//scan all blocks and get the adress of that block
	for(i=0;i<32;i++){
		id=ReadEeprom(i*EEPROM_BLOCK_SIZE)+(ReadEeprom((i*EEPROM_BLOCK_SIZE)+1)<<8);
		if(id==blockId){
			destAddr=i*EEPROM_BLOCK_SIZE;
			break;
		}
	}

	if(destAddr==0) return EEPROM_ERROR_BLOCK_NOT_FOUND;			

	for(i=0;i<EEPROM_BLOCK_SIZE;i++){
		*destPtr=ReadEeprom(destAddr++);
		destPtr++;	
	}
	
	return 0;
}
