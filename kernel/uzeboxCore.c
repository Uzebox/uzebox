/*
 *  Uzebox Kernel
 *  Copyright (C) 2008-2009 Alec Bourque
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

#include <util/atomic.h> 

#define Wait200ns() asm volatile("lpm\n\tlpm\n\t");
#define Wait100ns() asm volatile("lpm\n\t");

//Callbacks defined in each video modes module
extern void DisplayLogo(); 
extern void InitializeVideoMode();
extern void InitSoundPort();

void ReadButtons();
char EepromBlockExistsInternal(unsigned int blockId, u16* eepromAddr, u8* nextFreeBlockId);

extern unsigned char sync_phase;
extern unsigned char sync_pulse;
extern unsigned char sync_flags;
extern Track tracks[CHANNELS];
extern volatile unsigned int joypad1_status_lo,joypad2_status_lo;
extern volatile unsigned int joypad1_status_hi,joypad2_status_hi;
extern unsigned char render_lines_count;
extern unsigned char first_render_line;
extern unsigned char sound_enabled;

#if SNES_MOUSE == 1
	bool snesMouseEnabled=false;
#endif

u8 joypadsConnectionStatus;
//u16 prng_state=0;

const u8 eeprom_format_table[] PROGMEM ={(u8)EEPROM_SIGNATURE,		//(u16)
								   (u8)(EEPROM_SIGNATURE>>8),	//
								   EEPROM_HEADER_VER,			//(u8)				
								   EEPROM_BLOCK_SIZE,			//(u8) 
								   EEPROM_HEADER_SIZE,			//(u8) 
								   1,							//(u8) hardwareVersion
								   0,							//(u8) hardwareRevision
								   0x38,0x8, 					//(u16)  standard uzebox & fuzebox features
								   0,0,							//(u16)  extended features
								   0,0,0,0,0,0, 				//(u8[8])MAC
								   0,							//(u8)colorCorrectionType
								   0,0,0,0, 					//(u32)game CRC
								   0,							//(u8)bootloader flags
								   0,0,0,0,0,0,0,0,0 			//(u8[9])reserved
								   };



extern void wdt_randomize(void);

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init7"), used));
void Initialize(void) __attribute__((naked)) __attribute__((section(".init8"), used));

void wdt_init(void)
{

#if TRUE_RANDOM_GEN == 1	
	wdt_randomize();
#endif

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

/**
 * Dynamically sets the rasterizer parameters:
 * firstScanlineToRender = First scanline to render
 * scanlinesToRender     = Total number of vertical lines to render. 
 */
void SetRenderingParameters(u8 firstScanlineToRender, u8 scanlinesToRender){        
	render_lines_count=scanlinesToRender;
	first_render_line=firstScanlineToRender;
}


/*
 * I/O initialization table
 * The io_set macro is used to build an array of register,value pairs.
 * Using an array take less flash than discrete AVR instrustructions.
 */
#define io_set(a,b) ((_SFR_MEM_ADDR(a) & 0xff) + ((b)<<8))
#define set_io_end  0x0001

const u16 io_table[] PROGMEM ={
	io_set(TCCR1B,0x00),	//stop timers
	io_set(TCCR0B,0x00),
	io_set(DDRC,0xff), 		//video dac
	io_set(DDRB,0xff),		//h-sync for ad725
	io_set(DDRD,(1<<PD7)+(1<<PD4)), //audio-out + led 
	io_set(PORTD,(1<<PD4)+(1<<PD3)+(1<<PD2)), //turn on led & activate pull-ups for soft-power switches

	//setup port A for joypads

	io_set(DDRA,0b00001100), 	//set only control lines as outputs
	io_set(PORTA,0b11111011),  //activate pullups on the data lines

#if MIDI_IN == 1
	io_set(UCSR0B,(1<<RXEN0)), //set UART for MIDI in
	io_set(UCSR0C,(1<<UCSZ01)+(1<<UCSZ00)),
	io_set(UBRR0L,56), //31250 bauds (.5% error)
#endif
	
	//clear timers
	io_set(TCNT1H,0),
	io_set(TCNT1L,0),

	//set sync generator counter on TIMER1
	io_set(OCR1AH,HDRIVE_CL_TWICE>>8),
	io_set(OCR1AL,HDRIVE_CL_TWICE&0xff),

	io_set(TCCR1B,(1<<WGM12)+(1<<CS10)),//CTC mode, use OCR1A for match
	io_set(TIMSK1,(1<<OCIE1A)),			//generate interrupt on match

	//set clock divider counter for AD725 on TIMER0
	//outputs 14.31818Mhz (4FSC)
	io_set(TCCR0A,(1<<COM0A0)+(1<<WGM01)), //toggle on compare match + CTC
	io_set(OCR0A,0), //divide main clock by 2
	io_set(TCCR0B,(1<<CS00)), //enable timer, no pre-scaler

	//set sound PWM on TIMER2
	io_set(TCCR2A,(1<<COM2A1)+(1<<WGM21)+(1<<WGM20)), //Fast PWM	
	io_set(OCR2A,0), //duty cycle (amplitude)
	io_set(TCCR2B,(1<<CS20)),  //enable timer, no pre-scaler	
	io_set(SYNC_PORT,(1<<SYNC_PIN)|(1<<VIDEOCE_PIN)), //set sync & chip enable line to hi
	
	//set sync generator counter, COMPB for Vsync on TIMER1
	io_set(OCR1BL,0x1D),
	io_set(OCR1BH,0x03)
};


void Initialize(void){
	int i;

	if(!isEepromFormatted()) FormatEeprom();

	cli();
	
	//InitSoundPort(); //ramp-up sound to avoid click

	#if SOUND_MIXER == MIXER_TYPE_VSYNC
	
		//Initialize the mixer buffer
		//ramp up to avoid initial click
		for(int j=0;j<MIX_BANK_SIZE*2;j++){
			mix_buf[j]=0x80;//(i<128?i:128);
		}	
	
		mix_pos=mix_buf;
		mix_bank=0;
	#endif
	
	#if MIXER_CHAN4_TYPE == 0
		//initialize LFSR		
		tr4_barrel_lo=1;
		tr4_barrel_hi=1;		
		tr4_params=0b00000001; //15 bits no divider (1)
	#endif

	#if UART == 1
		InitUartRxBuffer();
		InitUartTxBuffer();
	#endif


	#if SNES_MOUSE == 1
		snesMouseEnabled=false;
	#endif

	//silence all sound channels
	for(i=0;i<CHANNELS;i++){
		mixer.channels.all[i].volume=0;
	}
	
	//set sync parameters. starts at odd field, in pre-eq pulses, line 1, vsync flag cleared
	sync_phase=0;
	sync_flags=0;
	sync_pulse=SYNC_PRE_EQ_PULSES+SYNC_EQ_PULSES+SYNC_POST_EQ_PULSES;

	//set rendering parameters
	render_lines_count=FRAME_LINES;
	first_render_line=FIRST_RENDER_LINE;

	joypad1_status_hi=0;
	joypad2_status_hi=0;
	sound_enabled=1;

	InitializeVideoMode();
	
	//Initialize I/O registers
	u16 val;
	u8 *ptr;
	for(u8 j=0;j<(sizeof(io_table)>>1);j++){
		val=pgm_read_word(&io_table[j]);
		ptr=(u8*)(val&0xff);
		*ptr=val>>8;	
	}

	sei();
	
	DisplayLogo();
	
}

void ReadButtons(){
	unsigned int p1ButtonsLo=0,p2ButtonsLo=0;
	unsigned char i;

	//latch controllers
	JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);
	#if SNES_MOUSE == 1
		if(snesMouseEnabled){
			WaitUs(1);
		}else{
			Wait200ns();
			Wait200ns();
		}	
	#else
		Wait200ns();
		Wait200ns();
	#endif
	JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));


	//read button states
	for(i=0;i<16;i++){

		p1ButtonsLo>>=1;
		p2ButtonsLo>>=1;

		#if SNES_MOUSE == 1
			if(snesMouseEnabled){
				WaitUs(5);
			}else{
				Wait200ns();
				Wait200ns();
			}	
		#else
			Wait200ns();
			Wait200ns();
		#endif
			
		//pulse clock pin		
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
		
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN))==0) p1ButtonsLo|=(1<<15);
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))==0) p2ButtonsLo|=(1<<15);
		
		JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);
		
		#if SNES_MOUSE == 1
			if(snesMouseEnabled){
				WaitUs(5);
			}else{
				Wait200ns();
				Wait200ns();
			}	
		#else
			Wait200ns();
			Wait200ns();
		#endif

	}

	#if JOYSTICK==TYPE_SNES
		joypad1_status_lo=p1ButtonsLo;
		joypad2_status_lo=p2ButtonsLo;
	#else
		joypad1_status_lo=p1ButtonsLo&0xff;
		joypad2_status_lo=p2ButtonsLo&0xff;	
	#endif

	if(joypad1_status_lo==(BTN_START+BTN_SELECT+BTN_Y+BTN_B) || joypad2_status_lo==(BTN_START+BTN_SELECT+BTN_Y+BTN_B)){
		SoftReset();
	}

}

/**
 * Initiates teh buttons reading and detect if joypads are connected.
 * When no device are plugged, the internal AVR pullup will drive the data lines high
 * otherwise the controller's shift register will drive the data lines low after 
 * completing a transfer. (The shift register's serial input pin is tied to ground)
 *
 * This functions is call by the kernel during VSYNC or can be called by the user
 * program when CONTROLLERS_VSYNC_READ==0.
*/
void ReadControllers(){

	//Detect if devices are connected.
	joypadsConnectionStatus=0;
	if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN))==0) joypadsConnectionStatus|=1;
	if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))==0) joypadsConnectionStatus|=2;
			
	//read the standard buttons
	ReadButtons();
}







#if SNES_MOUSE == 1


//read mouse bits 16 to 31
//spec requires a 2.5ms delay between the two 16bits chunks
//but the mouse works fine without it. 
void ReadMouseExtendedData(){
	//read the extended bits. Applies only if the mouse is plugged.
	//if bit 15 of standard word is 1, a mouse is plugged.
	unsigned int p1ButtonsHi=0,p2ButtonsHi=0;
	unsigned char i;

	if(joypad1_status_lo&(1<<15) || joypad2_status_lo&(1<<15)){

		//WaitUs(1);

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
			WaitUs(5);
		}
	
		joypad1_status_hi=p1ButtonsHi;
		joypad2_status_hi=p2ButtonsHi;

	}
}


/*
 This method activates teh code to read the mouse. 
 Currently reading the mouse takes a much a 2.5 scanlines.
*/
unsigned char playDevice=0,playPort=0,mouseSpriteIndex,mouseWidth,mouseHeight;
unsigned int actionButton;
int mx=0,my=0;

char EnableSnesMouse(unsigned char spriteIndex,const char *spriteMap){
	snesMouseEnabled=true;
	if(DetectControllers()!=0){
		mouseWidth=pgm_read_byte(&(spriteMap[0]));
		mouseHeight=pgm_read_byte(&(spriteMap[1]));

		mx=120;
		my=120;
		mouseSpriteIndex=spriteIndex;
		MapSprite(spriteIndex,spriteMap);
		MoveSprite(spriteIndex,mx,my,mouseWidth,mouseHeight);
		return 0;
	}else{
		snesMouseEnabled=false;
		return -1;
	}

}

unsigned char GetMouseX(){
	return mx;
}

unsigned char GetMouseY(){
	return my;
}

unsigned int GetActionButton(){
	return actionButton;
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





void ProcessMouseMovement(void){
	unsigned int joy;
	
	if(snesMouseEnabled){

		//check in case its a SNES pad

		if(playDevice==0){
			joy=ReadJoypad(playPort);

			if(joy&BTN_LEFT){
				mx-=2;
				if(mx<0) mx=0; 
			}
			if(joy&BTN_RIGHT){
				mx+=2;
				if(mx>231) mx=231;
			}
			if(joy&BTN_UP){
				my-=2;
				if(my<0)my=0;
			}
			if(joy&BTN_DOWN){
				my+=2;
				if(my>215)my=215;
			}

		}else{
	
			joy=ReadJoypadExt(playPort);

			if(joy&0x80){
				mx-=(joy&0x7f);
				if(mx<0) mx=0; 
			}else{
				mx+=(joy&0x7f);
				if(mx>231) mx=231;
			}
	
			if(joy&0x8000){
				my-=((joy>>8)&0x7f);
				if(my<0)my=0;
			}else{
				my+=((joy>>8)&0x7f);
				if(my>215)my=215;
			}
	
		}

		#if SPRITES_ENABLED !=0
			MoveSprite(mouseSpriteIndex,mx,my,mouseWidth,mouseHeight);
		#endif
	}
}
#endif

/* Detects what devices are connected to the game ports.
 *
 * IMPORTANT: If a mouse is expected to be connected, 
 * the first time this function is called, enough time must be given
 * to the mouse to settle by calling WaitVsync(8) before
 * invoking this function.
 * 
 * Returns: b1:b0 = 00 -> No controller connected in P1 port
 *                  01 -> Gamepad connected in P1 port
 *                  10 -> Mouse connected in P1 port
 *                  11 -> Reserved
 *
 * 
 *          b3:b2 = 00 -> No controller connected in P2 port
 *                  01 -> Gamepad connected in P2 port
 *                  10 -> Mouse connected in P2 port
 *                  11 -> Reserved
 */
unsigned char DetectControllers(){
	//unsigned int joy;
	unsigned char resp=0;

	if(joypadsConnectionStatus&1){

		//joy=ReadJoypad(0);
		if((joypad1_status_lo&0x8000)!=0){
			//we have a mouse in P1 port
			#if SNES_MOUSE == 1
				playDevice=1;
				playPort=0;
				actionButton=BTN_MOUSE_LEFT;
			#endif
			resp|=2;
		}else{
			//we have a regular controller in P1 port
			#if SNES_MOUSE == 1
				playDevice=0;
				playPort=0;
				actionButton=BTN_A;
			#endif
			resp|=1;
		}
	}

	if(joypadsConnectionStatus&2){
		//joy=ReadJoypad(1);
		if((joypad2_status_lo&0x8000)!=0){
			//we have a mouse in player 2 port
			#if SNES_MOUSE == 1
				playDevice=1;
				playPort=1;
				actionButton=BTN_MOUSE_LEFT;
			#endif
			resp|=8;
		}else{
			//we have a regular controller in P2
			resp|=4;
		}
	}

	return resp;
}

	
// Format eeprom, wiping all data to zero
void FormatEeprom(void) {

   // Set sig. so we don't format next time
   for (u8 i = 0; i < sizeof(eeprom_format_table); i++) {
	 WriteEeprom(i,pgm_read_byte(&eeprom_format_table[i]));
   }
   
   // Write free blocks IDs
   for (u16 i = (EEPROM_BLOCK_SIZE*EEPROM_HEADER_SIZE); i < (EEPROM_MAX_BLOCKS*EEPROM_BLOCK_SIZE); i+=EEPROM_BLOCK_SIZE) {
	  WriteEeprom(i,(u8)EEPROM_FREE_BLOCK);
	  WriteEeprom(i+1,(u8)(EEPROM_FREE_BLOCK>>8));
   }
   
}

// Format eeprom, saving data specified in ids
void FormatEeprom2(u16 *ids, u8 count) {
   u8 j;
   u16 id;

   // Set sig. so we don't format next time
   for (u8 i = 0; i < 8; i++) {
	 WriteEeprom(i,pgm_read_byte(&eeprom_format_table[i]));
   }

   // Paint unreserved free blocks
   for (int i = EEPROM_HEADER_SIZE; i < EEPROM_MAX_BLOCKS; i++) {
	  id=ReadEeprom(i*EEPROM_BLOCK_SIZE)+(ReadEeprom((i*EEPROM_BLOCK_SIZE)+1)<<8);

	  for (j = 0; j < count; j++) {
		 if (id == ids[j])
			break;
	  }

	  if (j == count) {
		 WriteEeprom(i*EEPROM_BLOCK_SIZE,(u8)EEPROM_FREE_BLOCK);
		 WriteEeprom(i*EEPROM_BLOCK_SIZE+1,(u8)(EEPROM_FREE_BLOCK>>8));
	  }
   }
}
	
//returns true if the EEPROM has been setup to work with the kernel.
bool isEepromFormatted(){
	unsigned id;
	id=ReadEeprom(0)+(ReadEeprom(1)<<8);
	return (id==EEPROM_SIGNATURE);
}

/*
 * Reads the power button status. 
 *
 * Returns: true if pressed.
 */
bool IsPowerSwitchPressed(){
	
	return 	((PIND & ((1<<PD3)|(1<<PD2))) != ((1<<PD3)|(1<<PD2)));
	
}

/*
 * Write the specified structure into the specified EEPROM block id. 
 * If the block does not exist, it is created.
 *
 * Returns:
 *  0x00 = Success
 * 	0x01 = EEPROM_ERROR_INVALID_BLOCK
 *	0x02 = EEPROM_ERROR_FULL
 */
char EepromWriteBlock(struct EepromBlockStruct *block){
	u16 eepromAddr=0;
	u8 *srcPtr=(unsigned char *)block,res,nextFreeBlock=0;

	res=EepromBlockExists(block->id,&eepromAddr,&nextFreeBlock);
	if(res!=0 && res!=EEPROM_ERROR_BLOCK_NOT_FOUND) return res;

	if(eepromAddr==0 && nextFreeBlock==0) return EEPROM_ERROR_FULL;
	if(eepromAddr==0 && nextFreeBlock!=0) eepromAddr=nextFreeBlock*EEPROM_BLOCK_SIZE;

	for(u8 i=0;i<EEPROM_BLOCK_SIZE;i++){
		WriteEeprom(eepromAddr++,*srcPtr);
		srcPtr++;	
	}
	
	return 0;
}

/*
 * Loads a data block from the in EEPROM into the specified structure.
 *
 * Returns: 
 *  0x00 = Success
 * 	0x01 = EEPROM_ERROR_INVALID_BLOCK
 *	0x03 = EEPROM_ERROR_BLOCK_NOT_FOUND
 */
char EepromReadBlock(unsigned int blockId,struct EepromBlockStruct *block){	
	u16 eepromAddr;
	u8 *blockPtr=(unsigned char *)block;

	u8 res=EepromBlockExists(blockId,&eepromAddr,NULL);
	if(res!=0) return res;

	for(u8 i=0;i<EEPROM_BLOCK_SIZE;i++){
		*blockPtr=ReadEeprom(eepromAddr++);
		blockPtr++;	
	}
	
	return EEPROM_OK;
}


/*
 * Scan is the specified EEPROM if block id exists. 
 * @param eepromAddr Set with it adress in EEPROM memory if block exists or zero if doesn't exist
 * @param nextFreeBlockId Set with id of next unnalocated block avaliable or zero (0) if all are allocated (i.e: eeprom is full)
 * 
 * @return
 *  0x00 = Success, Block exists.
 * 	0x01 = EEPROM_ERROR_INVALID_BLOCK.
 *	0x03 = EEPROM_ERROR_BLOCK_NOT_FOUND.
 */
char EepromBlockExists(unsigned int blockId, u16* eepromAddr, u8* nextFreeBlockId){
	u8 nextFreeBlock=0;
	u8 result=EEPROM_ERROR_BLOCK_NOT_FOUND;
	u16 id;
	*eepromAddr=0;

	if(blockId==EEPROM_FREE_BLOCK) return EEPROM_ERROR_INVALID_BLOCK;
		
	//scan all blocks and get the memory adress of that block and the next free block
	for(u8 i=0;i<EEPROM_MAX_BLOCKS;i++){
		id=ReadEeprom(i*EEPROM_BLOCK_SIZE)+(ReadEeprom((i*EEPROM_BLOCK_SIZE)+1)<<8);
		
		if(id==blockId){
			*eepromAddr=(i*EEPROM_BLOCK_SIZE);
			result=EEPROM_OK;
		}
		
		if(id==0xffff && nextFreeBlock==0){
			nextFreeBlock=i;
			if(nextFreeBlockId!=NULL) *nextFreeBlockId=nextFreeBlock;					
		}
	}

	return result;
}


#if UART == 1
	/*
	 * UART RX/TX buffer functions
	 */

	volatile u8 uart_rx_tail;
	volatile u8 uart_rx_head;
	volatile u8 uart_rx_buf[UART_RX_BUFFER_SIZE];

	//obsolete
	void UartGoBack(u8 count){
		uart_rx_tail-=count;
		uart_rx_tail&=(UART_RX_BUFFER_SIZE-1);		//wrap pointer to buffer size
	}

	//obsolete
	u8 UartUnreadCount(){
		return uart_rx_head-uart_rx_tail;
	}

	bool IsUartRxBufferEmpty(){
		return (uart_rx_tail==uart_rx_head);
	}

	s16 UartReadChar(){

		if(uart_rx_head != uart_rx_tail){

			u8 data=uart_rx_buf[uart_rx_tail];
			uart_rx_tail=((uart_rx_tail+1) & (UART_RX_BUFFER_SIZE-1));	//wrap pointer to buffer size			
			return (data&0xff);

		}else{
			return -1;	//no data in buffer
		}
	}

	void InitUartRxBuffer(){
		uart_rx_tail=0;
		uart_rx_head=0;
	}

	/*
	 * UART Transmit buffer function
	 */
	volatile u8 uart_tx_tail;
	volatile u8 uart_tx_head;
	volatile u8 uart_tx_buf[UART_TX_BUFFER_SIZE];


	bool IsUartTxBufferEmpty(){
		return (uart_tx_tail==uart_tx_head);
	}

	bool IsUartTxBufferFull(){
		u8 next_head = ((uart_tx_head + 1) & (UART_TX_BUFFER_SIZE-1));
		return (next_head == uart_tx_tail);
	}

	s8 UartSendChar(u8 data){

 		u8 next_head = ((uart_tx_head + 1) & (UART_TX_BUFFER_SIZE-1));

		if (next_head != uart_tx_tail) {
			uart_tx_buf[uart_tx_head]=data;
			uart_tx_head=next_head;		
			return 0;
		}else{
			return -1; //buffer full
		}
	}

	void InitUartTxBuffer(){
		uart_tx_tail=0;
		uart_tx_head=0;
	}


#endif


/**
 * Generate a random number based on a LFSR. This function is *much* faster than avr-libc rand();
 * taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1
 *
 * Input: Zero=return the next random value. Non-zero=Sets the seed value.
 */
u16 GetPrngNumber(u16 seed){
	static u16 prng_state;
  	
	if(seed!=0) prng_state=seed;
	
	u16 bit  = ((prng_state >> 0) ^ (prng_state >> 2) ^ (prng_state >> 3) ^ (prng_state >> 5) ) & 1;
	prng_state =  (prng_state >> 1) | (bit << 15);
	return prng_state;   
}


#if DEBUG==1

//prints char to console
u8 _x=2,_y=1;

void debug_clear(){
	ClearVram();
	_x=2,_y=1;
}

void debug_crlf(){
	debug_char(0x0a);
}

void debug_scroll(){
	if(_y>(SCREEN_TILES_V-2)){
		//scroll all lines up 
		
		for(u16 i=0;i<(VRAM_TILES_H*(SCREEN_TILES_V-2));i++){
			vram[i]=vram[(i+VRAM_TILES_H)];
		}

		//clear last line
		for(u8 i=0;i<VRAM_TILES_H;i++){
			SetFont(i,SCREEN_TILES_V-2,0);
			//vram[(VRAM_TILES_H*(SCREEN_TILES_V-2))+i]=32;
		}

		_y=SCREEN_TILES_V-2;
		_x=2;
	}
}

void debug_int(u16 i){
	debug_hex(i>>8);
	debug_hex(i&0xff);
}

void debug_long_hex(u32 i){
	debug_hex((i>>24)&0xff);
	debug_hex((i>>16)&0xff);
	debug_hex((i>>8)&0xff);
	debug_hex(i&0xff);
}



void debug_hex(char c){



	if(_x>=SCREEN_TILES_H-4){
		_x=2;
		_y++;
	}

	PrintChar(_x,_y,'<');
	PrintHexByte(_x+1,_y,c);
	PrintChar(_x+3,_y,'>');
	_x+=4;
	
	debug_scroll();


}

void debug_byte(unsigned char val){
	unsigned char c;

	if(_x>=SCREEN_TILES_H-4){
		_x=2;
		_y++;
	}

	for(char i=2;i>-1;i--){
		c=val%10;
		if(val>0 || i==2){
			SetFont(i+_x,_y,c+16);
		}else{
			SetFont(i+_x,_y,16);
		}		
		val=val/10;
	}
	_x+=3;
	
	debug_scroll();

}

void debug_long(unsigned long val){
	u32 c;
	if(_x>=SCREEN_TILES_H-10){
		_x=2;
		_y++;
	}

	for(char i=9;i>-1;i--){
		c=val%10;
		if(val>0 || i==9){
			SetFont(i+_x,_y,c+16);
		}else{
			SetFont(i+_x,_y,16);
		}		
		val=val/10;
	}
	_x+=10;
	
	debug_scroll();
		
}

void debug_char(char c){


	if(c==0x0d){
	//	PrintChar(_x++,_y,'{');
	}else if(c==0x0a){
	//	PrintChar(_x,_y,'|');
		_x=2;
		_y++;				
	}else if(c<32 || c>'z'){
		PrintChar(_x,_y,'<');
		PrintHexByte(_x+1,_y,c);
		PrintChar(_x+3,_y,'>');
		_x+=4;
	}else{
		PrintChar(_x++,_y,c);		
	}
	if(_x>=(SCREEN_TILES_H-1)){
		_x=2;
		_y++;
	}

	debug_scroll();

}

void debug_str_p(const char* data){
	char c;
	while(1){
		c=pgm_read_byte(data);
		if(c==0) break;
		debug_char(c);
		data++;
	}
}

void debug_str_r(char* data,u8 size, bool hex)
{
	for(u8 i=0;i<size;i++){
		if(hex){
			debug_hex(data[i]);	
		}else{
			debug_char(data[i]);	
		}
	}

}
#endif
