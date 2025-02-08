/**
 * Best time for Mandelbrot: 9.48 seconds with floats -O3

 * https://en.wikipedia.org/wiki/Rugg/Feldman_benchmarks
 */

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>
#include <keyboard.h>
#include <spiram.h>
#include "terminal.h"

#if VIDEO_MODE == 5
	#include "data/font6x8-full.inc"
	#include "data/fat_pixels.inc"
#endif

#define wait_spi_ram_byte() asm volatile("lpm \n\t lpm \n\t lpm \n\t lpm \n\t lpm \n\t lpm \n\t nop \n\t" : : : "r0", "r1"); //wait 19 cycles

#define RAM_SIZE 65535
#define STACK_DEPTH	10				//depth of the loops/gosub stack
#define PCODE_STACK_DEPTH 5			//depth of the internal parameter passing statck
#define VAR_TYPE_FLOAT 0
#define VAR_TYPE_FLOAT_ARRAY 1
#define VAR_TYPE float
#define VAR_SIZE sizeof(float)	//Size of variables in bytes
#define EOL   0xff					//denotes the end of a program line
#define EOS   0x00					//Used to delimitate the end of a string
#define EOSNL 0x1					//User to delimitate the end of a string that should end with a new line

#define STACK_FRAME_TYPE_GOSUB 	2
#define STACK_FRAME_TYPE_FOR 	1

#define CR	'\r'
#define NL	'\n'

typedef u16 LINENUM;

union {
	float f;
	struct{
		float* f_ptr;
		u16	size;
	}f_array;
	uint32_t i;
	uint8_t bytes[sizeof(float)];
}type_converter;

typedef struct {
	u8 frame_type;
	u8 for_var;
	s16 terminal;
	s16 step;
	u8 line_len;
	u8 line_pos;
	u16 prog_pos;
}stack_frame;

//typedef struct{
//	union {
//		float f;
//		union{
//			float* f_ptr;
//			u16	size;
//		}f_array;
//		uint8_t bytes[sizeof(float)];
//	}content;
//}variable;


enum opcodes{
	OP_LD_BYTE=0x80, OP_LD_WORD, OP_LD_DWORD, OP_LD_VAR, OP_ASSIGN, OP_ASSIGN_ARRAY, OP_DIM,
	OP_LOOP_FOR, OP_LOOP_NEXT, OP_LOOP_EXIT,OP_GOTO,OP_GOSUB,OP_RETURN,
	OP_PRINT, OP_PRINT_LSTR, OP_PRINT_CHAR, OP_CRLF,
	OP_IF, OP_CMP_GT, OP_CMP_GE, OP_CMP_LT,OP_CMP_EQ,
	OP_ADD,	OP_SUB,	OP_MUL,	OP_DIV,OP_POW,
	OP_FUNC_TICKS,OP_FUNC_CHR,OP_FUNC_SIN,OP_FUNC_LOG,
	OP_CLS,
	OP_BREAK,OP_END,
};

//Number of parameters/extra bytes required for each opcodes. Used by the EXIT statement.
//const u8 opcodes_param_cnt[] PROGMEM = {
const u8 opcodes_param_cnt[] PROGMEM = {
	1,2,4,1,1,1,1,	//OP_LD_BYTE, OP_LD_WORD, OP_LD_FLOAT, OP_LD_VAR, OP_ASSIGN, OP_ASSIGN_1D_ARRAY, OP_DIM
	1,1,2,2,2,0,	//OP_LOOP_FOR, OP_LOOP_NEXT, OP_LOOP_EXIT,OP_GOTO,OP_GOSUB,OP_RETURN
	0,0,0,0,		//OP_PRINT, OP_PRINT_LSTR, OP_PRINT_CHAR, OP_CRLF
	0,0,0,0,0,		//OP_IF, OP_CMP_GT, OP_CMP_GE, OP_CMP_LT,OP_CMP_EQ
	0,0,0,0,0,		//P_ADD,OP_SUB,	OP_MUL,	OP_DIV
	0,1,1,1,		//OP_FUNC_TICKS,OP_FUNC_CHR,OP_FUNC_SIN,OP_FUNC_LOG
	0,				//OP_CLS
	0,0				//OP_BREAK,OP_END,
};

enum vars{
	VAR_A=0, VAR_B, VAR_C, VAR_D, VAR_E, VAR_F, VAR_G, VAR_H, VAR_I, VAR_J, VAR_K, VAR_L, VAR_M,
	VAR_N, VAR_O, VAR_P, VAR_Q, VAR_R, VAR_S, VAR_T, VAR_U, VAR_V, VAR_W, VAR_X, VAR_Y, VAR_Z
};

enum ERRORS{
	ERR_OK=0,
	ERR_INTERNAL_ERROR,			//Generic Internal error
	ERR_STACK_OVERFLOW,			//For-Gosub nesting too deep (increment STACK_DEPTH)
	ERR_NEXT_WITHOUT_FOR,		//next encountered without a for
	ERR_INVALID_NESTING,		//Invalid nesting of for-next loops and/or gosub-return statements
	ERR_EXIT_WITHOUT_FOR,		//EXIT encoutered with no FOR (or bad nesting)
	ERR_FOR_WITHOUT_NEXT,
	ERR_ILLEGAL_ARGUMENT,
	ERR_UNDEFINED_LINE_NUMBER,
	ERR_DIVISION_BY_ZERO,
	ERR_OUT_OF_MEMORY,
	ERR_EXIT_WITHOUT_NEXT,		//EXIT encoutered with no following NEXT statement
};


const char error_00_msg[] PROGMEM = "Ok";	//Error code 0 represents OK /no errors
const char error_01_msg[] PROGMEM = "Internal Error";
const char error_02_msg[] PROGMEM = "Stack Overflow";
const char error_03_msg[] PROGMEM = "NEXT without FOR";
const char error_04_msg[] PROGMEM = "Invalid NEXT nesting";
const char error_05_msg[] PROGMEM = "EXIT without FOR";
const char error_06_msg[] PROGMEM = "FOR without NEXT";
const char error_07_msg[] PROGMEM = "Illegal Argument";
const char error_08_msg[] PROGMEM = "Undefined line number";
const char error_09_msg[] PROGMEM = "Division by zero";
const char error_10_msg[] PROGMEM = "Out of memory";
const char error_11_msg[] PROGMEM = "EXIT without NEXT";

const char* const error_messages[] PROGMEM ={
		error_00_msg,
		error_01_msg,
		error_02_msg,
		error_03_msg,
		error_04_msg,
		error_05_msg,
		error_06_msg,
		error_07_msg,
		error_08_msg,
		error_09_msg,
		error_10_msg,
		error_11_msg,
};

static void push_value(float v);
static float pop_value();
static void print_error(u8 error_code, u16 line_no);
static u16 execute(bool initialize);
static u8 run_tests();

const u8 token_rf_benchmark_5[] ;

static u8 *txtpos;
static u32 timer_ticks;
static u8 error_code;
static u8 error_line;

static float pcode_vars[26]; //A-Z
static u8 pcode_vars_type[26];

static u16 pcode_sp_min=PCODE_STACK_DEPTH; //used to debug
static u16 opcode_sp=PCODE_STACK_DEPTH;
static float pcode_stack[PCODE_STACK_DEPTH];
static stack_frame stack[STACK_DEPTH];
static s8 stack_ptr=STACK_DEPTH;

static u8 c1,var,opcode;
static u16 u16_var;
static u16 u16_ptr;
static  volatile u32 dword;
static  float v1,v2,ftmp;
static bool found, jump, stop=false;
static stack_frame *sf;
static u8 spi_line_buf[80];

static u16 spi_prog_pos;	//Absolute position of the current program line in SPI memory
static u16 spi_line_pos;	//Position with the program line that is loaded in the line buffer
static u16 spi_line_no;		//The program's Basic line number
static u8  spi_line_len;	//The program's line lenght in the line buffer (include the two-bytes line number and lenght byte)
static u16 spi_prog_pos_tmp;//Used to cache temporarely the address in the program
static stack_frame *sf;
static s16 current;
static u16 scan_pos;
static u16 line_to_find;
static u16 line_no_tmp;
static u8 line_len_tmp;
static u16 spi_prog_size;
static u16 spi_heap_ptr;

//0.10938=0x9f,0x02,0xe0,0x3d
//0.09090=0xc7,0x29,0xba,0x3d
//2.5 0x00,0x00,0x20,0x40
//1.0 0x00,0x00,0x80,0x3f
const u8 token_mand[] PROGMEM={
10,0,5,OP_CLS,EOL,
20,0,7,OP_FUNC_TICKS,OP_ASSIGN, VAR_T, EOL,
30,0,12,OP_LD_BYTE, 1, OP_LD_BYTE, 0, OP_LD_BYTE, 21, OP_LOOP_FOR, VAR_A, EOL,
40,0,12,OP_LD_BYTE, 1,OP_LD_BYTE, 0, OP_LD_BYTE, 31, OP_LOOP_FOR, VAR_B, EOL,
50,0,20,OP_LD_DWORD,0x9f,0x02,0xe0,0x3d,OP_LD_VAR,VAR_B, OP_MUL,OP_LD_DWORD,0x00,0x00,0x20,0x40,OP_SUB,OP_ASSIGN, VAR_C, EOL,
60,0,20,OP_LD_DWORD,0xc7,0x29,0xba,0x3d,OP_LD_VAR,VAR_A, OP_MUL,OP_LD_DWORD,0x00,0x00,0x80,0x3f,OP_SUB,OP_ASSIGN, VAR_D, EOL,
70,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_X, EOL,
80,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_Y, EOL,
90,0,12,OP_LD_BYTE, 1,OP_LD_BYTE, 0, OP_LD_BYTE, 14, OP_LOOP_FOR, VAR_I, EOL,
100,0,17,OP_LD_VAR, VAR_X, OP_LD_VAR, VAR_X, OP_MUL, OP_LD_VAR, VAR_Y, OP_LD_VAR, VAR_Y, OP_MUL, OP_ADD, OP_ASSIGN, VAR_F, EOL,
110,0,13,OP_LD_VAR, VAR_F, OP_LD_BYTE, 4, OP_CMP_GT, OP_IF, OP_LOOP_EXIT, 0, 0, EOL,
120,0,20,OP_LD_VAR, VAR_X, OP_LD_VAR, VAR_X, OP_MUL, OP_LD_VAR, VAR_Y, OP_LD_VAR, VAR_Y, OP_MUL, OP_SUB, OP_LD_VAR, VAR_C, OP_ADD, OP_ASSIGN, VAR_E, EOL,
130,0,17,OP_LD_BYTE, 2, OP_LD_VAR, VAR_X, OP_MUL, OP_LD_VAR, VAR_Y, OP_MUL, OP_LD_VAR, VAR_D, OP_ADD, OP_ASSIGN, VAR_Y, EOL,
140,0,8,OP_LD_VAR, VAR_E, OP_ASSIGN, VAR_X, EOL,
150,0,6,OP_LOOP_NEXT, VAR_I, EOL,
//160,0,0,OP_LD_VAR, VAR_I, OP_LD_BYTE, 126 ,OP_ADD, OP_PRINT_CHAR, EOL,
//160,0,10,OP_LD_VAR, VAR_I, OP_LD_BYTE, 116+3 ,OP_ADD, OP_PRINT_CHAR, EOL,
165,0,16,OP_LD_VAR, VAR_I, OP_LD_BYTE, 116+5 ,OP_ADD, OP_PRINT_CHAR, OP_LD_VAR, VAR_I, OP_LD_BYTE, 116+5 ,OP_ADD, OP_PRINT_CHAR,EOL,
170,0,6,OP_LOOP_NEXT, VAR_B, EOL,
180,0,5,OP_CRLF, EOL,
190,0,6,OP_LOOP_NEXT, VAR_A, EOL,
195,0,7,OP_FUNC_TICKS,OP_ASSIGN, VAR_U, EOL,
200,0,20,OP_PRINT_LSTR,'E','l','a','p','s','e','d',' ','t','i','m','e',':',' ',EOS, EOL,
210,0,13,OP_LD_VAR, VAR_U, OP_LD_VAR, VAR_T, OP_SUB, OP_LD_BYTE, 60, OP_DIV, OP_PRINT, EOL,
220,0,14,OP_PRINT_LSTR,' ','S','e','c','o','n','d','s',EOSNL,EOL,
0,0
};

const u8 test[] PROGMEM={
10,0,5,OP_CLS,EOL,
20,0,12,OP_LD_BYTE, 1, OP_LD_BYTE, 0, OP_LD_BYTE, 3, OP_LOOP_FOR, VAR_A, EOL,
30,0,13,OP_LD_VAR, VAR_A, OP_LD_BYTE, 2, OP_CMP_EQ, OP_IF, OP_LOOP_EXIT, 0, 0, EOL,
35,0,7,OP_PRINT_LSTR,'*',EOSNL,EOL,
40,0,6,OP_LOOP_NEXT, VAR_A, EOL,
50,0,11,OP_LD_BYTE, 1,OP_LD_VAR, VAR_B,OP_ADD, OP_ASSIGN, VAR_B, EOL,
60,0,7,OP_LD_VAR, VAR_B,OP_PRINT, EOL,
70,0,11,OP_PRINT_LSTR,':','D','o','n','e',EOSNL,EOL,
80,0,7,OP_GOTO, 20, 0, EOL,
0,0
};


void vsyncCallback(){
	timer_ticks++;
	terminal_VsyncCallback();	//used to poll the keyboard
}

volatile u8* line;
volatile u8 lenght;


/**
 * Load a program in flash to the SPI RAM and clear remaining RAM.
 */
u16 load_prog_spiram(const u8* program){
	u16 i=0;
	u8 c=0;

	SpiRamSeqWriteStart(0,0);

	//load from flash to sram and compute line sizes
	//txtpos=prog_mem;
	while(1){
		if(pgm_read_word(&(program[i]))==0)break;
		lenght=0;
		line=txtpos;
		while(1){
			c=pgm_read_byte(&(program[i++]));
			//printf_P(PSTR("%02x "),c);
			SpiRamSeqWriteU8(c);
			lenght++;
			if(c==EOL){
				break;
			}
		}
		spi_prog_size+=lenght;
	}

	//clear remaining RAM
	for(u16 j=spi_prog_size;j<RAM_SIZE;j++){
		SpiRamSeqWriteU8(0);
	}

	SpiRamSeqWriteEnd();

	spi_prog_size+=2;


	return spi_prog_size;
};

//Function to init the SPI RAM
//for some reason the one from spiram.c
//doesn't work on my hardware.
u8 _spiram_start(){
	u8 retval;
	DDRA &= ~(1<<PA4);	//disable SPI RAM

	//enable SPI in 2X mode
	SPCR=(1<<SPE)+(1<<MSTR);
	SPSR=(1<<SPI2X);
	DDRB|=(1<<PB7)+(1<<PB5);
	PORTA|=(1<<PA4);
	DDRA|=(1<<PA4);  //enable SPI RAM

	SpiRamWriteU8(0,0xcccc,0xcc);
	retval=SpiRamReadU8(0,0xcccc);
	if(retval==0xcc){
		return 1;
	}

	return 0; //status;

}

int main(){
	//bind the terminal receiver to stdout
	stdout = &TERMINAL_STREAM;
	#if VIDEO_MODE == 5
		SetTileTable(font);
	#endif
	SetFontTilesIndex(0);

	//register interrupt handler to process keystrokes, timers, etc
	SetUserPreVsyncCallback(&vsyncCallback);

	//initialize terminal
	terminal_Init();
	terminal_Clear();
	terminal_SetAutoWrap(true);
	terminal_SetCursorVisible(true);

	//Initializer SPI RAM
	if(_spiram_start()==0){
		printf_P(PSTR("SPI RAM Init fail.\n"));
	}else{
		printf_P(PSTR("SPI RAM Init OK.\n\n"));
	}

	//load_prog_spiram(token_mand);
	//load_prog_spiram(test);
	//load_prog_spiram(token_rf_benchmark_5);

	//execute(true);
	if(run_tests()!=0){
		print_error(error_code,error_line);
	}

	printf_P(PSTR("\nOk\n"));
	while(1);

}

//allocate a one dimentional array of floats
u16 allocate_numeric_array(u16 elements){
	if(spi_heap_ptr-(elements * VAR_SIZE) < spi_prog_size){
		error_code=ERR_OUT_OF_MEMORY;
		return 0;
	}
	spi_heap_ptr-= (elements * VAR_SIZE);
	return spi_heap_ptr;
}

/**
 * Load a program line at the specified absolute memory position in SPI ram.
 * Returns: The line number
 */
u16 spi_load_line(u16 prog_pos){
	SpiRamSeqReadStart(0,prog_pos);			//Move the program pointer
	SpiRamSeqReadInto(spi_line_buf,3); 		//read line number (word) and line lenght (byte) in ram buffer
	spi_line_no=(u16)spi_line_buf[0];
	if(spi_line_no!=0){
		spi_line_len=(u8)spi_line_buf[2];
		SpiRamSeqReadInto(&spi_line_buf[3],spi_line_len-3); //read remaining of line in ram buffer
		spi_line_pos=3; 						//skip line number and line lenght
	}
	SpiRamSeqReadEnd();
	return spi_line_no;
}

/**
 * Executes the Basic program until it's end or if an error occurs.
 * Returns: The error code.
 */
u16 execute(bool initialize){

	if(initialize){
		for(u8 i=0;i<26;i++){
			pcode_vars[i]=0;
			pcode_vars_type[i]=0;
		}

		pcode_sp_min=PCODE_STACK_DEPTH;
		opcode_sp=PCODE_STACK_DEPTH;
		stack_ptr=STACK_DEPTH;
		spi_heap_ptr=RAM_SIZE-1;
	}

	found=false;
	jump=false;
	stop=false;
	spi_prog_pos = 0;
	spi_line_pos=0;
	error_code=0;
	error_line=0;

	while(1){
		if(spi_load_line(spi_prog_pos)==0){
			 break; //Exit if we reached the end of the program (line number == 0)
		}

		if(opcode_sp!=PCODE_STACK_DEPTH){
			printf("Unpoped stack found entering line: %i",spi_line_no);
			return ERR_INTERNAL_ERROR;
		}

		if(spi_line_no==83){
			error_code=0;
		}

		while(1){
			jump=false;
			opcode = spi_line_buf[spi_line_pos++];

			switch(opcode){
				case OP_LD_BYTE:
					c1=spi_line_buf[spi_line_pos++];
					push_value(c1);
					break;

				case OP_LD_WORD:
					//u16_var=(*((u16*) &spi_line_buf[spi_line_pos]));
					u16_var=spi_line_buf[spi_line_pos]|spi_line_buf[spi_line_pos+1]<<8;
					spi_line_pos+=2;
					push_value(u16_var);
					break;

				case OP_LD_DWORD:
					type_converter.i=(*((u32*)&spi_line_buf[spi_line_pos]));
					spi_line_pos+=4;
					push_value(type_converter.f);
					break;

				case OP_LD_VAR:
					c1=spi_line_buf[spi_line_pos++];//Get variable number

					if(pcode_vars_type[c1]==VAR_TYPE_FLOAT_ARRAY){
						u16_var=pop_value(); //array index
						//type_converter.f=pcode_vars[c1];
						//v1=type_converter.f_array.f_ptr[u16_var];
						u16_ptr=pcode_vars[c1];	//get pointer to array in spi ram
						u16_ptr+=(u16_var*4);	//index requested element
						v1=SpiRamReadU32(0,u16_ptr);
						push_value(v1);
					}else{
						v1=pcode_vars[c1];
						push_value(v1);
					}

					break;

				case OP_ASSIGN:
					c1=spi_line_buf[spi_line_pos++]; //variable index
					if(pcode_vars_type[c1]==VAR_TYPE_FLOAT_ARRAY){
						v1=pop_value();				//value to assign
						u16_var=pop_value(); 		//array index
						u16_ptr=pcode_vars[c1];		//get pointer to array in spi ram
						u16_ptr+=(u16_var*4);		//index requested element
						SpiRamWriteU32(0,u16_ptr,(u32)v1);
						//printf_P(PSTR("M(%i)=%g\n"),u16_var,v1);
					}else{
						pcode_vars[c1]=pop_value();
						break;
					}
					break;

				case OP_DIM:
					c1=spi_line_buf[spi_line_pos++];	//get variable ID to allocate as array
					u16_var=pop_value();				//get array size
					u16_ptr=allocate_numeric_array(u16_var);
					if(u16_ptr==0)break; 				//out of memory!

					//type_converter.f_array.f_ptr=(float*)u16_ptr;
					//type_converter.f_array.size=u16_var;
					//pcode_vars[c1]=type_converter.f;

					pcode_vars[c1]=u16_ptr;
					pcode_vars_type[c1]=VAR_TYPE_FLOAT_ARRAY;
					//TODO error checks for correct type and REDIM
					break;


				case OP_LOOP_FOR:
					if(stack_ptr == 0){
						error_code=ERR_STACK_OVERFLOW;
						break;
					}
					sf=&stack[--stack_ptr];

					float terminal = pop_value();
					float initial = pop_value();
					float step = pop_value();
					//c1=*txtpos++; //get variable index (0-25)
					//-->
					c1=spi_line_buf[spi_line_pos++];//*spi_line_buf_ptr++; //get variable index (0-25)
					pcode_vars[c1]=initial;

					sf->frame_type 	= STACK_FRAME_TYPE_FOR;
					sf->for_var 	= c1;
					sf->terminal 	= terminal;
					sf->step		= step;
					sf->prog_pos	= spi_prog_pos;
					sf->line_pos 	= spi_line_pos;
					sf->line_len	= spi_line_len;

					break;

				case OP_LOOP_NEXT:
					if(stack_ptr==STACK_DEPTH){
						//nothing on the stack!
						error_code=ERR_NEXT_WITHOUT_FOR;
						break;
					}

					u8 var_id=spi_line_buf[spi_line_pos++];

					if(stack[stack_ptr].frame_type != STACK_FRAME_TYPE_FOR || stack[stack_ptr].for_var!=var_id){
						error_code=ERR_INVALID_NESTING;
						break;
					}

					sf=&stack[stack_ptr];
					current=pcode_vars[(u8)sf->for_var]+=sf->step;	//increment the loop variable with the STEP value

					//Use a different test depending on the sign of the step increment
					if((sf->step > 0 && current <= sf->terminal) || (sf->step < 0 && current >= sf->terminal)){
						//We have to loop so don't pop the stack
						spi_prog_pos = sf->prog_pos;
						spi_line_len = sf->line_len;

						//Check if we were at the end of the line containing the FOR statement
						//and if so, don't reload that line for nothing and just advance to the
						//next line
						if(sf->line_pos == (sf->line_len-1)){
							spi_line_pos = spi_line_len-1;	//move position to the EOL maker of the line
							spi_line_buf[spi_line_pos] = EOL;
						}else{

							spi_load_line(spi_prog_pos);	//load the program line in the buffer
							spi_line_pos = sf->line_pos;	//restore the position to right after the FOR statement
						}

					}else{
						//We've run to the end of the loop. drop out of the loop, popping the stack
						stack_ptr++;
					}
					break;

				case OP_LOOP_EXIT:
						if(stack_ptr==STACK_DEPTH){
							//nothing on the stack!
							error_code=ERR_EXIT_WITHOUT_FOR;
							break;
						}

						//check if we saved the NEXT position from a previous loop.
						//NOTE: Currently does not support multiple statements per line separated by :
						spi_prog_pos_tmp=spi_prog_pos+spi_line_pos;
						if((u16)spi_line_buf[spi_line_pos]!=0){
							spi_prog_pos=(u16)spi_line_buf[spi_line_pos];
							spi_load_line(spi_prog_pos);	//load the program line in the buffer
							//Pop the stack and drop out of the loop
							stack_ptr++;
							break;
						}


						spi_line_pos+=2; //skip the jump adress in spi ram

						//u16 iteration=0;
						found=false;
						while(1){
							while(spi_line_buf[spi_line_pos] != EOL){
								if(spi_line_buf[spi_line_pos] == OP_LOOP_NEXT){
									spi_line_pos++;

									//NEXT found, find the variable
									var=spi_line_buf[spi_line_pos++] ;//*txtpos++ ;
									if(var >= 26){
										error_code=ERR_ILLEGAL_ARGUMENT;
										break;
									}
									sf=&stack[stack_ptr];
									if(var != sf->for_var){
										error_code=ERR_INVALID_NESTING;
										break;
									}

									//Drop out of the loop and pop the stack
									stack_ptr++;

									//Cache the NEXT statement location for a future loop.
									//NOTE: This is the position of the beginning of the line containing the NEXT statement
									//because currently there is no support for multiple statements per line separated by :
									SpiRamWriteU16(0,spi_prog_pos_tmp,spi_prog_pos+spi_line_len);
									found=true;
									break;

								}else{
									//advance to next token
									opcode=spi_line_buf[spi_line_pos++];//(*txtpos++);
									//get opcode's params count to skip over it
									c1=pgm_read_byte(&(opcodes_param_cnt[opcode-0x80]));
									spi_line_pos+=c1;
								}

							}

							if(found || error_code != ERR_OK) break;

							//current_line +=	 current_line[sizeof(LINENUM)];
							//current_line_no= (current_line[1]<<8)+current_line[0];
							//txtpos = current_line+sizeof(LINENUM)+sizeof(char);
							spi_prog_pos+=spi_line_len;
							spi_load_line(spi_prog_pos);	//load the program line in the buffer
							if(spi_line_no==0){
								error_code=ERR_EXIT_WITHOUT_NEXT;
								break;
							}
						};

						break;

				case OP_GOTO:

					//currpos=txtpos;

					//u16 line_to_find=(*((int16_t*)txtpos));
					line_to_find=spi_line_buf[spi_line_pos];

					//if same line, go back to beginning of line
					if(line_to_find == spi_line_no){
						spi_line_pos=3;
						break;
					}

					//check if we saved the actual line's memory position from a previous loop.
//					if(line_to_find&0x8000){
//						txtpos=((u8*)((line_to_find&=0x7fff)+(u16)program_start ))-1;
//						jump=true;
//						break;
//					}
					//othewise skip the jump adress and scan the whole program to find the line
					//spi_line_pos+=2;//txtpos+=2;

					line_no_tmp=0;
					scan_pos=0;
					while(1){
						line_no_tmp=SpiRamReadU16(0,scan_pos);
						if(line_no_tmp==0 || line_no_tmp> line_to_find){
							//we reached the end of the program and didn't find the line
							error_code=ERR_UNDEFINED_LINE_NUMBER;
							break;
						}else if(line_no_tmp==line_to_find){
							//line found!
							//save position in spi memory for fast lookup next time
							//set the MSbit to indicate line is an AVR memory location instead of line number.
							//This in turn limits the Basic line numbers to 32767.
							//(*((int16_t*)currpos))=((u16)scan_ptr-(u16)program_start)|0x8000;
							spi_prog_pos=scan_pos;
							jump=true;
							break;
						}
						//advance to next line;
						line_len_tmp=SpiRamReadU8(0,scan_pos+2);
						scan_pos+=line_len_tmp;
					}
					break;

				case OP_GOSUB:
					if(stack_ptr == 0){
						error_code=ERR_STACK_OVERFLOW;
						break;
					}

					//get line number
					//currpos=txtpos;
					//spi_prog_pos_tmp=spi_prog_pos+spi_line_pos; //hold pointer to next line

					line_to_find=spi_line_buf[spi_line_pos];
					spi_line_pos+=2;

//					if(line_to_find & 0x8000){
//						//we have the target memory address of the line cached
//						PCODE_DEBUG("[GOSUB CACHED %i] ",line_to_find&0x7fff);
//
//						//save return address on stack
//						sf=&stack[--stack_ptr];
//						sf->frame_type 	= STACK_FRAME_TYPE_GOSUB;
//						sf->txt_pos	 	= txtpos;
//						sf->current_line= current_line;
//
//						txtpos=program_start+(line_to_find&0x7fff)-1;
//						jump=true;
//						break;
//					}


					line_no_tmp=0;
					scan_pos=0;

					while(1){
						line_no_tmp=SpiRamReadU16(0,scan_pos);
						if(line_no_tmp==0 || line_no_tmp> line_to_find){
							error_code=ERR_UNDEFINED_LINE_NUMBER;
							break;

						}else if(line_no_tmp==line_to_find){
							//line found!

							//save return address on stack
							sf=&stack[--stack_ptr];
							sf->frame_type 	= STACK_FRAME_TYPE_GOSUB;
							//sf->txt_pos	 	= txtpos;
							//sf->current_line= current_line;
							sf->prog_pos=spi_prog_pos+spi_line_len;

							//save position in AVR memory for fast lookup next time
							//set the MSbit to indicate line is an AVR memory location instead of line number.
							//This in turn limits the Basic line numbers to 32767.
							//(*((int16_t*)currpos))=((u16)scan_ptr-(u16)program_start)|0x8000;

							spi_prog_pos=scan_pos;
							jump=true;
							break;
						}
						//advance to next line;
						line_len_tmp=SpiRamReadU8(0,scan_pos+2);
						scan_pos+=line_len_tmp;
					}
					break;

				case OP_RETURN:
					if(stack_ptr==STACK_DEPTH){
						//nothing on the stack!
						error_code=ERR_NEXT_WITHOUT_FOR;
						break;
					}
					if(stack[stack_ptr].frame_type != STACK_FRAME_TYPE_GOSUB){
						error_code=ERR_INVALID_NESTING;
						break;
					}
					sf=&stack[stack_ptr];
					spi_prog_pos=sf->prog_pos;
					stack_ptr++;
					jump=true;
					break;

				case OP_PRINT:
					v1=pop_value();
					printf_P(PSTR("%g"),v1);
					break;

				case OP_PRINT_LSTR:
					while(1){
						//c1=*txtpos++;
						//-->
						c1=spi_line_buf[spi_line_pos++];

						if(c1==EOS){
							break;
						}else if(c1==EOSNL){
							terminal_SendChar(NL);
							terminal_SendChar(CR);
							break;
						}
						terminal_SendChar(c1);
					}
					break;

				case OP_PRINT_CHAR:
					c1=pop_value();
					terminal_SendChar(c1);
					break;

				case OP_CRLF:
					terminal_SendChar(NL);
					terminal_SendChar(CR);
					break;

				case OP_IF:
					v1=pop_value();
					if(!v1){
						spi_line_pos=spi_line_len-1; //advance position to EOL
					}
					break;

				case OP_CMP_GT:
					v2=pop_value();
					v1=pop_value();
					push_value(v1 > v2);
					break;

				case OP_CMP_GE:
					v2=pop_value();
					v1=pop_value();
					push_value(v1 >= v2);
					break;

				case OP_CMP_LT:
					v2=pop_value();
					v1=pop_value();
					push_value(v1 < v2);
					break;

				case OP_CMP_EQ:
					v2=pop_value();
					v1=pop_value();
					push_value(v1 == v2);
					break;

				case OP_ADD:
					v2=pop_value();
					v1=pop_value();
					push_value(v1+v2);
					break;

				case OP_SUB:
					v2=pop_value();
					v1=pop_value();
					push_value(v1-v2);
					break;

				case OP_MUL:
					v2=pop_value();
					v1=pop_value();
					push_value(v1*v2);
					break;

				case OP_DIV:
					v2=pop_value();
					v1=pop_value();
					if(v2==0){
						error_code=ERR_DIVISION_BY_ZERO;
						break;
					}
					push_value(v1/v2);
					break;

				case OP_POW:
					v2=pop_value();
					v1=pop_value();
					ftmp=powf(v1,v2);
					push_value(ftmp);
					break;

				case OP_FUNC_TICKS:
					dword=timer_ticks;
					push_value(dword);
					break;

				case OP_FUNC_CHR:
					v1=pop_value(); //TODO
					push_value(v1);
					break;

				case OP_FUNC_SIN:
					v1=pop_value();
					ftmp=sinf(v1);
					push_value(ftmp);
					break;

				case OP_FUNC_LOG:
					v1=pop_value();
					ftmp=logf(v1);
					push_value(ftmp);
					break;

				case OP_CLS:
					terminal_Clear();
					break;

				case OP_BREAK:
					break;

				case OP_END:
					stop=true;
					break;
			};

			if(error_code){
				//print_error(error_code, spi_line_no);
				error_line=spi_line_no;
				break;
			}

			if(spi_line_buf[spi_line_pos] == EOL || jump || stop)break;

		}; //Do until we reach end-of-line

		if(error_code || stop) break;
		if(!jump) spi_prog_pos+=spi_line_len;	//advance to next program line

	}; //Do while program running

	return error_code;
}


void print_error(u8 error_code, u16 line_number){
	printf_P((PGM_P)pgm_read_word(&(error_messages[error_code])));
	printf_P(PSTR(" in %i\n"),line_number);
}


static inline void push_value(float v){
	if(opcode_sp==0){
		printf_P(PSTR("opcode stack overflow: push"));
		//dumpstack();
		//dumpvars();
		//dumpmem(0,12);
		while(1);
	}
	pcode_stack[--opcode_sp] = v;
	//if(pcode_sp<pcode_sp_min) pcode_sp_min=pcode_sp;
}

static inline float pop_value(){
	if(opcode_sp==PCODE_STACK_DEPTH){
		printf_P(PSTR("opcode stack overflow: pop"));
		//dumpstack();
		//dumpmem(0,12);
		while(1);
	}
	return pcode_stack[opcode_sp++];
}

const u8 token_rf_benchmark_8[] PROGMEM={
40,0,7,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
60,0,11,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
70,0,11,OP_LD_VAR, VAR_K, OP_LD_BYTE, 2, OP_POW, OP_ASSIGN, VAR_A, EOL,
80,0,9,OP_LD_VAR, VAR_K, OP_FUNC_LOG, OP_ASSIGN, VAR_B, EOL,
90,0,9,OP_LD_VAR, VAR_K, OP_FUNC_SIN, OP_ASSIGN, VAR_C, EOL,
100,0,14,OP_LD_VAR, VAR_K, OP_LD_WORD, 100, 0, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 100
110,0,7,OP_PRINT_LSTR,'E',EOSNL,EOL,
120,0,5,OP_END,EOL,
0,0
};


const u8 token_rf_benchmark_7[] PROGMEM={
40,0,7,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
55,0,8,OP_LD_BYTE, 5, OP_DIM,VAR_M, EOL,
60,0,11,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K, OP_ADD, OP_ASSIGN, VAR_K, EOL,
70,0,20,OP_LD_VAR, VAR_K, OP_LD_BYTE, 2, OP_DIV, OP_LD_BYTE, 3, OP_MUL, OP_LD_BYTE, 4, OP_ADD, OP_LD_BYTE, 5, OP_SUB, OP_ASSIGN, VAR_A, EOL,
80,0,7,OP_GOSUB,120,0,EOL,
82,0,12,OP_LD_BYTE, 1, OP_LD_BYTE, 1, OP_LD_BYTE, 5, OP_LOOP_FOR, VAR_L, EOL,	//1 to 5
83,0,10,OP_LD_VAR, VAR_L, OP_LD_VAR, VAR_A, OP_ASSIGN, VAR_M, EOL,
85,0,6,OP_LOOP_NEXT, VAR_L, EOL,
90,0,14,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
100,0,7,OP_PRINT_LSTR,'E',EOSNL,EOL,
110,0,5,OP_END,EOL,
120,0,5,OP_RETURN,EOL,
0,0
};

const u8 token_rf_benchmark_6[] PROGMEM={
40,0,7,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
55,0,8,OP_LD_BYTE, 5, OP_DIM, VAR_M, EOL,
60,0,11,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
70,0,20,OP_LD_VAR, VAR_K, OP_LD_BYTE, 2, OP_DIV, OP_LD_BYTE, 3, OP_MUL, OP_LD_BYTE, 4, OP_ADD, OP_LD_BYTE, 5, OP_SUB, OP_ASSIGN, VAR_A, EOL,
80,0,7,OP_GOSUB,120,0,EOL,
82,0,12,OP_LD_BYTE, 1, OP_LD_BYTE, 1, OP_LD_BYTE, 5, OP_LOOP_FOR, VAR_L, EOL,	//1 to 5
85,0,6,OP_LOOP_NEXT, VAR_L, EOL,
90,0,14,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
100,0,7,OP_PRINT_LSTR,'E',EOSNL,EOL,
110,0,5,OP_END,EOL,
120,0,5,OP_RETURN,EOL,
0,0
};

const u8 token_rf_benchmark_5[] PROGMEM={
40,0,7,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
60,0,11,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
70,0,20,OP_LD_VAR, VAR_K, OP_LD_BYTE, 2, OP_DIV, OP_LD_BYTE, 3, OP_MUL, OP_LD_BYTE, 4, OP_ADD, OP_LD_BYTE, 5, OP_SUB, OP_ASSIGN, VAR_A, EOL,
80,0,7,OP_GOSUB,120,0,EOL,
90,0,14,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
100,0,7,OP_PRINT_LSTR,'E',EOSNL,EOL,
110,0,5,OP_END,EOL,
120,0,5,OP_RETURN,EOL,
0,0
};

const u8 token_rf_benchmark_4[] PROGMEM={
40,0,7,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
60,0,11,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
65,0,20,OP_LD_VAR, VAR_K, OP_LD_BYTE, 2, OP_DIV, OP_LD_BYTE, 3, OP_MUL, OP_LD_BYTE, 4, OP_ADD, OP_LD_BYTE, 5, OP_SUB, OP_ASSIGN, VAR_A, EOL,
70,0,14,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
80,0,7,OP_PRINT_LSTR,'E',EOSNL,EOL,
0,0
};

const u8 token_rf_benchmark_3[] PROGMEM={
40,0,7,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
60,0,11,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
65,0,20,OP_LD_VAR, VAR_K, OP_LD_VAR, VAR_K, OP_DIV, OP_LD_VAR, VAR_K, OP_MUL, OP_LD_VAR, VAR_K, OP_ADD, OP_LD_VAR, VAR_K, OP_SUB, OP_ASSIGN, VAR_A, EOL,
70,0,14,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
80,0,7,OP_PRINT_LSTR,'E',EOSNL,EOL,
0,0
};


const u8 token_rf_benchmark_2[] PROGMEM={
40,0,7,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
60,0,11,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
70,0,14,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
80,0,7,OP_PRINT_LSTR,'E',EOSNL,EOL,
0,0
};

const u8 token_rf_benchmark_1[] PROGMEM={
30,0,7,OP_PRINT_LSTR,'S',EOSNL,EOL,
40,0,13,OP_LD_BYTE, 1, OP_LD_BYTE, 1, OP_LD_WORD, 0xe8, 0x03, OP_LOOP_FOR, VAR_K, EOL,	//1 to 1000
50,0,6,OP_LOOP_NEXT, VAR_K, EOL,
70,0,7,OP_PRINT_LSTR,'E',EOSNL,EOL,
0,0
};


const u8 token_rf_benchmark_0[]PROGMEM={
	10,0,8,OP_LD_BYTE, 5, OP_DIM,VAR_C, EOL,
	20,0,10,OP_LD_BYTE,3, OP_LD_BYTE, 66, OP_ASSIGN, VAR_C, EOL,
	30,0,10,OP_LD_BYTE,3, OP_LD_VAR, VAR_C, OP_ASSIGN, VAR_D, EOL,
	0,0
};


const u8* const test_suite[] PROGMEM ={
		token_rf_benchmark_1,
		token_rf_benchmark_2,
		token_rf_benchmark_3,
		token_rf_benchmark_4,
		token_rf_benchmark_5,
		token_rf_benchmark_6,
		token_rf_benchmark_7,
		token_rf_benchmark_8,
		token_mand
};

u8 run_tests(){
	float results[9];
	float ticks;
	for(u8 i=0;i<9;i++)results[i]=0;

	terminal_Clear();
	printf_P(PSTR("Uzebox Basic v0.1 - Rugg/Feldman BASIC Benchmarks (SPI RAM)\n"));

	u8 res;
	for(u16 i=0;i<sizeof(test_suite)/2;i++){
		spi_prog_size=load_prog_spiram((const u8*)pgm_read_word(&(test_suite[i])));


//		printf_P(PSTR("-------\n"));
//		u8 b;
//		SpiRamSeqReadStart(0,0);
//		for(u16 i=0;i<prog_size;i++){
//			b=SpiRamSeqReadU8();
//			printf_P(PSTR("%02x "),b);
//		}
//		SpiRamSeqReadEnd();



		printf_P(PSTR("Test #%i...\n"),i+1);
		ticks=timer_ticks;
		res=execute(true);

		if(res!=0){
			return res;
		}
		results[i]=((float)timer_ticks-ticks)/60;
	}

	terminal_Clear();
	printf_P(PSTR("Uzebox Basic v0.1 - Rugg/Feldman BASIC Benchmarks (SPI RAM)\n"));

	printf_P(PSTR("\x81\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x85\x82\n"));
	printf_P(PSTR("\x86 System      \x86 Tst1 \x86 Tst2 \x86 Tst3 \x86 Tst4 \x86 Tst5 \x86 Tst6 \x86 Tst7 \x86 Tst8 \x86 Mand  \x86\n"));
	printf_P(PSTR("\x87\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x85\x88\n"));

	printf_P(PSTR("\x86 Uzebox      \x86 %.2f \x86 %.2f \x86 %.2f \x86 %.2f \x86 %.2f \x86 %.2f \x86 %.2f \x86 %.2f \x86 %.2f  \x86\n"),results[0],results[1],results[2],results[3],results[4],results[5],results[6],results[7],results[8]);
	printf_P(PSTR("\x86 X16         \x86 0.2  \x86 1.2  \x86 2.3  \x86 2.7  \x86 2.9  \x86 4.3  \x86 6.8  \x86 1.3  \x86 23    \x86\n"));
	printf_P(PSTR("\x86 BBC Micro   \x86 0.8  \x86 3.1  \x86 8.1  \x86 8.7  \x86 9.0  \x86 13.9 \x86 21.1 \x86 49.9 \x86 97    \x86\n"));
	printf_P(PSTR("\x86 ZX 80       \x86 1.5  \x86 4.7  \x86 9.2  \x86 9.0  \x86 12.7 \x86 25.9 \x86 39.2 \x86 ---  \x86 222   \x86\n"));
	printf_P(PSTR("\x86 VIC 20      \x86 1.4  \x86 8.3  \x86 15.5 \x86 17.1 \x86 18.3 \x86 27.2 \x86 42.7 \x86 99   \x86 ---   \x86\n"));
	printf_P(PSTR("\x86 Apple II    \x86 1.3  \x86 8.5  \x86 16.0 \x86 17.8 \x86 19.1 \x86 28.6 \x86 44.8 \x86 55.5 \x86 165   \x86\n"));
	printf_P(PSTR("\x86 C64         \x86 1.2  \x86 9.3  \x86 17.6 \x86 19.5 \x86 21.0 \x86 29.5 \x86 47.5 \x86 119  \x86 203   \x86\n"));
	printf_P(PSTR("\x86 Altair 8800 \x86 1.9  \x86 7.5  \x86 20.6 \x86 20.9 \x86 22.1 \x86 38.8 \x86 57.0 \x86 67.8 \x86 ---   \x86\n"));
	printf_P(PSTR("\x86 Atari 800XL \x86 2.2  \x86 7.3  \x86 19.7 \x86 24.1 \x86 26.3 \x86 40.3 \x86 60.1 \x86 ---  \x86 136   \x86\n"));
	printf_P(PSTR("\x86 ZX Spectrum \x86 4.4  \x86 8.2  \x86 20.0 \x86 19.2 \x86 23.1 \x86 53.4 \x86 77.6 \x86 239  \x86 273   \x86\n"));
	printf_P(PSTR("\x86 TI-99/4A    \x86 2.9  \x86 8.8  \x86 22.8 \x86 24.5 \x86 26.1 \x86 61.6 \x86 84.4 \x86 382  \x86 364   \x86\n"));

	printf_P(PSTR("\x83\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x85\x84\n"));
	printf_P(PSTR("See: https://en.wikipedia.org/wiki/Rugg/Feldman_benchmarks\n"));
	printf_P(PSTR("     https://github.com/SlithyMatt/multi-mandlebrot\n"));

	return 0;
}
