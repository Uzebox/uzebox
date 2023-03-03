
#define SRAM_READ 0
#define SRAM_WRITE 1



void sram_beginSequence(u32 addr,u8 mode){

	PORTA&=~(1<<PA4); //assert CS

	//send command
	if(mode==0){
		SPDR=3;	//read
	}else{
		SPDR=2;	//write sequential			
	}
	while(!(SPSR & (1<<SPIF))); //wait for tx to complete

	//send 24-bit adress MSB
	SPDR=(u8)(addr>>16);
	while(!(SPSR & (1<<SPIF))); //wait for tx to complete

	//send 24-bit adress Middle byte 
	SPDR=(u8)(addr>>8);
	while(!(SPSR & (1<<SPIF))); //wait for tx to complete

	//send 24-bit adress LSB
	SPDR=(u8)(addr);
	while(!(SPSR & (1<<SPIF))); //wait for tx to complete
}

void sram_endSequence(){
	PORTA|=(1<<PA4); //deassert CS	
}


u8 sram_read(void){

	//send data
	SPDR=0xff;	//dummy
	while(!(SPSR & (1<<SPIF))); //wait for tx to complete

	return SPDR;
}

void sram_write(u8 data){
	//send data
	SPDR=data;
	while(!(SPSR & (1<<SPIF))); //wait for tx to complete
}

void sram_start(){
	//enable SPI in 2X mode
	SPCR=(1<<SPE)+(1<<MSTR);
	SPSR=(1<<SPI2X);
	DDRB|=(1<<PB7)+(1<<PB5);
	PORTA|=(1<<PA4);
	DDRA|=(1<<PA4);  //spi ram CS pin
}

void sram_stop(){
	SPCR=(0<<SPE);
}
