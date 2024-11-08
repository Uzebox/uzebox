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
#include "terminal.h"

#if VIDEO_MODE == 5
	#include "data/font6x8-full.inc"
	#include "data/fat_pixels.inc"
#endif

#define wait_spi_ram_byte() asm volatile("lpm \n\t lpm \n\t lpm \n\t lpm \n\t lpm \n\t lpm \n\t nop \n\t" : : : "r0", "r1"); //wait 19 cycles

//Uncomment to print pcode debug info
//#define DBGPCODE

#ifdef DBGPCODE
	#define PCODE_DEBUG(fmt, ...) printf_P(PSTR(fmt), ##__VA_ARGS__)
#else
    #define PCODE_DEBUG(...)
#endif


#define RAM_SIZE 800
#define STACK_DEPTH	10				//depth of the loops/gosub stack
#define PCODE_STACK_DEPTH 5			//depth of the internal parameter passing statck
#define VAR_TYPE_FLOAT 0
#define VAR_TYPE_FLOAT_ARRAY 1
#define VAR_TYPE float
#define VAR_SIZE sizeof(float)	//Size of variables in bytes
#define EOL   0xff					//denotes the end of the bytecode line
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
	int32_t i;
	uint8_t bytes[sizeof(float)];
}type_converter;

typedef struct {
	u8 frame_type;
	u8 for_var;
	s16 terminal;
	s16 step;
	u8 *current_line;
	u8 *txt_pos;
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
	OP_LD_BYTE=0x80, OP_LD_WORD, OP_LD_DWORD, OP_LD_VAR, OP_ASSIGN,OP_DIM,
	OP_LOOP_FOR, OP_LOOP_NEXT, OP_LOOP_EXIT,OP_GOTO,OP_GOSUB,OP_RETURN,
	OP_PRINT, OP_PRINT_LSTR, OP_PRINT_CHAR, OP_CRLF,
	OP_IF, OP_CMP_GT, OP_CMP_GE, OP_CMP_LT,
	OP_ADD,	OP_SUB,	OP_MUL,	OP_DIV,
	OP_FUNC_TICKS,OP_FUNC_CHR,
	OP_CLS,
	OP_BREAK,OP_END,
	//OP_CMP_NE, OP_CMP_LT, OP_CMP_LE,
};

//number of parameters/extra bytes required for each opcodes
//const u8 opcodes_param_cnt[] PROGMEM = {
const u8 opcodes_param_cnt[] PROGMEM = {
	1,2,4,1,1,1,	//OP_LD_BYTE, OP_LD_WORD, OP_LD_FLOAT, OP_LD_VAR, OP_ASSIGN, OP_DIM
	1,1,2,2,2,0,//OP_LOOP_FOR, OP_LOOP_NEXT, OP_LOOP_EXIT,OP_GOTO,OP_GOSUB,OP_RETURN
	0,0,0,0,	//OP_PRINT, OP_PRINT_LSTR, OP_PRINT_CHAR, OP_CRLF
	0,0,0,0,	//OP_IF, OP_CMP_GT, OP_CMP_GE
	0,0,0,0,	//P_ADD,OP_SUB,	OP_MUL,	OP_DIV
	0,1,		//OP_FUNC_TICKS,OP_FUNC_CHR
	0,			//OP_CLS
	0,0			//OP_BREAK,OP_END,
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
	ERR_EXIT_WITHOUT_FOR,		//EXIT encoutered with no FOR or bad nesting
	ERR_FOR_WITHOUT_NEXT,
	ERR_ILLEGAL_ARGUMENT,
	ERR_UNDEFINED_LINE_NUMBER,
	ERR_DIVISION_BY_ZERO,
	ERR_OUT_OF_MEMORY
};


const char error_00_msg[] PROGMEM = "";	//Error code 0 represents OK /no errors
const char error_01_msg[] PROGMEM = "Internal Error in %1";
const char error_02_msg[] PROGMEM = "Stack Overflow in %i";
const char error_03_msg[] PROGMEM = "NEXT without FOR in %i";
const char error_04_msg[] PROGMEM = "Invalid NEXT nesting in %i";
const char error_05_msg[] PROGMEM = "EXIT without FOR in %i";
const char error_06_msg[] PROGMEM = "FOR without NEXT in %i";
const char error_07_msg[] PROGMEM = "Illegal Argument in %i";
const char error_08_msg[] PROGMEM = "Undefined line number in %i";
const char error_09_msg[] PROGMEM = "Division by zero in %i";
const char error_10_msg[] PROGMEM = "Out of memory in %i";

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
		error_09_msg
};

static void dumpmem(u16 start_addr,u8 rows);
static void dumpstack();
static void dumpvars();
static void push_value(float v);
static float pop_value();
static void print_error(u8 error_code, u16 line_no);
static u16 execute(bool initialize);
static void run_tests();

const u8 token_rf_benchmark_5[] ;

static u8 *txtpos;
static u32 timer_ticks;
static u8 *program_start;
static u8 *program_end;
static u8 *current_line;
static u16 current_line_no;
static u8 error_code;
static u8 prog_mem[RAM_SIZE];
static u8 *heap_ptr;

static float pcode_vars[26]; //A-Z
static u8 pcode_vars_type[26];

static u16 pcode_sp_min=PCODE_STACK_DEPTH; //used to debug
static u16 opcode_sp=PCODE_STACK_DEPTH;
static float pcode_stack[PCODE_STACK_DEPTH];
static stack_frame stack[STACK_DEPTH];
static s8 stack_ptr=STACK_DEPTH;

static  u8 c1,var,opcode;
static  u16 u16_var;
//static  u8* addr;
static  volatile u32 dword;
static  float v1,v2;
static bool found, jump, stop=false;
static stack_frame *sf;
static u8* currpos;



//0.10938=0x9f,0x02,0xe0,0x3d
//0.09090=0xc7,0x29,0xba,0x3d
//2.5 0x00,0x00,0x20,0x40
//1.0 0x00,0x00,0x80,0x3f
const u8 token_mand[] PROGMEM={
10,0,0,OP_CLS,EOL,
20,0,0,OP_FUNC_TICKS,OP_ASSIGN, VAR_T, EOL,
30,0,0,OP_LD_BYTE, 1, OP_LD_BYTE, 0, OP_LD_BYTE, 21, OP_LOOP_FOR, VAR_A, EOL,
40,0,0,OP_LD_BYTE, 1,OP_LD_BYTE, 0, OP_LD_BYTE, 31, OP_LOOP_FOR, VAR_B, EOL,
50,0,0,OP_LD_DWORD,0x9f,0x02,0xe0,0x3d,OP_LD_VAR,VAR_B, OP_MUL,OP_LD_DWORD,0x00,0x00,0x20,0x40,OP_SUB,OP_ASSIGN, VAR_C, EOL,
60,0,0,OP_LD_DWORD,0xc7,0x29,0xba,0x3d,OP_LD_VAR,VAR_A, OP_MUL,OP_LD_DWORD,0x00,0x00,0x80,0x3f,OP_SUB,OP_ASSIGN, VAR_D, EOL,
70,0,0,OP_LD_BYTE, 0, OP_ASSIGN, VAR_X, EOL,
80,0,0,OP_LD_BYTE, 0, OP_ASSIGN, VAR_Y, EOL,
90,0,0,OP_LD_BYTE, 1,OP_LD_BYTE, 0, OP_LD_BYTE, 14, OP_LOOP_FOR, VAR_I, EOL,
100,0,0,OP_LD_VAR, VAR_X, OP_LD_VAR, VAR_X, OP_MUL, OP_LD_VAR, VAR_Y, OP_LD_VAR, VAR_Y, OP_MUL, OP_ADD, OP_ASSIGN, VAR_F, EOL,
110,0,0,OP_LD_VAR, VAR_F, OP_LD_BYTE, 4, OP_CMP_GT, OP_IF, OP_LOOP_EXIT, 0, 0, EOL,
120,0,0,OP_LD_VAR, VAR_X, OP_LD_VAR, VAR_X, OP_MUL, OP_LD_VAR, VAR_Y, OP_LD_VAR, VAR_Y, OP_MUL, OP_SUB, OP_LD_VAR, VAR_C, OP_ADD, OP_ASSIGN, VAR_E, EOL,
130,0,0,OP_LD_BYTE, 2, OP_LD_VAR, VAR_X, OP_MUL, OP_LD_VAR, VAR_Y, OP_MUL, OP_LD_VAR, VAR_D, OP_ADD, OP_ASSIGN, VAR_Y, EOL,
140,0,0,OP_LD_VAR, VAR_E, OP_ASSIGN, VAR_X, EOL,
150,0,0,OP_LOOP_NEXT, VAR_I, EOL,
160,0,0,OP_LD_VAR, VAR_I, OP_LD_BYTE, 126 ,OP_ADD, OP_PRINT_CHAR, EOL,
170,0,0,OP_LOOP_NEXT, VAR_B, EOL,
180,0,0,OP_CRLF, EOL,
190,0,0,OP_LOOP_NEXT, VAR_A, EOL,
195,0,0,OP_FUNC_TICKS,OP_ASSIGN, VAR_U, EOL,
200,0,0,OP_PRINT_LSTR,'E','l','a','p','s','e','d',' ','t','i','m','e',':',' ',EOS, EOL,
210,0,0,OP_LD_VAR, VAR_U, OP_LD_VAR, VAR_T, OP_SUB, OP_LD_BYTE, 60, OP_DIV, OP_PRINT, EOL,
220,0,0,OP_PRINT_LSTR,' ','S','e','c','o','n','d','s',EOSNL,EOL,
0,0
};

void vsyncCallback(){
	timer_ticks++;
	terminal_VsyncCallback();	//used to poll the keyboard
}

volatile u8* line;
volatile u8 lenght;
volatile u16 prog_size=0;
u16 load_prog(const u8* program){
	u16 i=0;
	//load from flash to sram and compute line sizes
	txtpos=prog_mem;
	while(1){
		if(pgm_read_word(&(program[i]))==0)break;
		lenght=0;
		line=txtpos;
		while(1){
			*txtpos=pgm_read_byte(&(program[i++]));
			lenght++;
			if(*txtpos++==EOL){
				break;
			}
		}
		line[2]=lenght;
		prog_size+=lenght;
	}
	prog_size+=2;
	program_start=prog_mem;
	program_end=prog_mem+prog_size;

	//clear remaining RAM
	for(u16 j=prog_size;j<RAM_SIZE;j++){
		prog_mem[j]=0;
	}

	return prog_size;
};

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

	//copy the tokenised program in SRAM
	//for(u16 i=0;i<sizeof(token_mand)-2;i++) prog_mem[i]=pgm_read_byte(&(token_mand[i]));
	//for(u16 i=0;i<sizeof(token_rf_benchmark_1)-2;i++) prog_mem[i]=pgm_read_byte(&(token_rf_benchmark_1[i]));
	//for(u16 i=0;i<sizeof(token_rf_benchmark_2)-2;i++) prog_mem[i]=pgm_read_byte(&(token_rf_benchmark_2[i]));
	//for(u16 i=0;i<sizeof(token_rf_benchmark_3)-2;i++) prog_mem[i]=pgm_read_byte(&(token_rf_benchmark_3[i]));

	//printf("Program size: %i\n",sizeof(token_rf_benchmark_1));


//	u16 size=load_prog(token_rf_benchmark_5);
//	float ticks=timer_ticks;
//	execute(true);
//	printf("Elapsed time: %g,size=%i",((float)timer_ticks-ticks)/60,size);


	//execute();
	run_tests();

	//dumpmem(0,10);
	//dumpvars();
	printf_P(PSTR("\nOk\n>"));
	while(1);

}

//allocate a one dimentional array of floats
u8* allocate_numeric_array(u16 elements){
	if(heap_ptr-(elements * VAR_SIZE) < program_end){
		error_code=ERR_OUT_OF_MEMORY;
		return NULL;
	}

	heap_ptr-= (elements * VAR_SIZE);

	//PCODE_DEBUG("[allocate_numeric_array: heap_ptr=0x%04x, heap_top=0x%04x] ",(u16)heap_ptr,(u16)prog_mem+sizeof(prog_mem)-1);

	return heap_ptr;
}

/**
 * Search for the specified line number.
 * Returns: The memmory position of the line in AVR memory, if the line was found.
 * Returns 0 otherwise and error_code will be set.
 */
u8* find_line(u16 line_to_find){
	u8* currpos=txtpos;
	//check if we saved the actual line's memory position from a previous loop.
//	if(line_to_find&0x8000){
//		txtpos=((u8*)((line_to_find&=0x7fff)+(u16)program_start ))-1;
//		jump=true;
//		PCODE_DEBUG("[JUMP %i FAST] ", txtpos[1]+(txtpos[2]<<8));
//		break;
//	}

	//txtpos+=2;
	u16 line_no=0;
	u8* scan_ptr=program_start;
	while(1){
		line_no=(*((int16_t*)scan_ptr));
		if(line_no==0){
			error_code=ERR_UNDEFINED_LINE_NUMBER;
			return 0;
		}else if(line_no==line_to_find){
			//line found!
			//save position in AVR memory for fast lookup next time
			//set the MSbit to indicate line is an AVR memory location instead of line number.
			//This in turn limits the Basic line numbers to 32767.
			(*((int16_t*)currpos))=((u16)scan_ptr-(u16)program_start)|0x8000;
			//txtpos=scan_ptr-1;
			//jump=true;
			return scan_ptr-1;
		}
		//advance to next line;
		scan_ptr+=scan_ptr[2];
	}


}

u16 execute(bool initialize){

	if(initialize){
		for(u8 i=0;i<26;i++){
			pcode_vars[i]=0;
			pcode_vars_type[i]=0;
		}

		pcode_sp_min=PCODE_STACK_DEPTH;
		found=false;
		jump=false;
		stop=false;

		opcode_sp=PCODE_STACK_DEPTH;
		stack_ptr=STACK_DEPTH;
		heap_ptr=prog_mem+RAM_SIZE-1;

		PCODE_DEBUG("[Initialize vars, heap_ptr=0x%04x]\n",heap_ptr);
	}


	program_start = prog_mem;
	//program_end = program_start+(sizeof(token_mand)-2);
	txtpos = program_start;

//	u16 opcounter=0; //for debug
//	u16 linecounter=0; //for debug

	error_code=0;

	do{
		current_line=txtpos;
		current_line_no= (txtpos[1]<<8)+txtpos[0];
		txtpos+=sizeof(LINENUM)+sizeof(char); //skip line number and line lenght

		if(opcode_sp!=PCODE_STACK_DEPTH){
			printf("unpoped stack found entering line: %i",current_line_no);
			while(1);
		}

		PCODE_DEBUG("%03i) ",current_line_no);// <pos:%02x,cl:%02x>",current_line_no,(u16)txtpos-(u16)program_start,(u16)current_line-(u16)program_start);
		while(1){
			jump=false;
			opcode = (*txtpos++);

			switch(opcode){
				case OP_LD_BYTE:
					c1=*txtpos++;
					push_value(c1);
					PCODE_DEBUG("[LDB %i] ",c1);
					break;

				case OP_LD_WORD:
					u16_var=(*((int16_t*)txtpos));
					txtpos+=2;
					push_value(u16_var);
					PCODE_DEBUG("[LDW %i] ",u16_var);
					break;

				case OP_LD_DWORD:
					type_converter.i=(*((int32_t*)txtpos));
					txtpos+=4;
					push_value(type_converter.f);
					PCODE_DEBUG("[LDD %g] ",type_converter.f);
					break;

				case OP_LD_VAR:
					c1=*txtpos++;
					if(pcode_vars_type[c1]==VAR_TYPE_FLOAT_ARRAY){
						u16_var=pop_value(); //array index
						type_converter.f=pcode_vars[c1];
						v1=type_converter.f_array.f_ptr[u16_var];
						push_value(v1);
						PCODE_DEBUG("[LDV %c(%i):%g] ",c1+65,u16_var,v1);
					}else{
						v1=pcode_vars[c1];
						push_value(v1);
						PCODE_DEBUG("[LDV %c:%g] ",c1+65,v1);
					}

					break;

				case OP_ASSIGN:
					//Get variable to assign to
					c1=*txtpos++;

					if(pcode_vars_type[c1]==VAR_TYPE_FLOAT_ARRAY){
						v1=pop_value(); //value to assign
						u16_var=pop_value(); //array index
						type_converter.f=pcode_vars[c1];
						type_converter.f_array.f_ptr[u16_var]=v1;
						//printf("value=%g",type_converter.f_array.f_ptr[u16_var]);

						PCODE_DEBUG("[LET %c(%i)=%g] ",c1+65,u16_var,v1);
					}else{
						pcode_vars[c1]=pop_value();
						PCODE_DEBUG("[LET %c=%g] ",c1+65,pcode_vars[c1]);
					}
					break;

				case OP_DIM:
					//Get variable value
					c1=*txtpos++;
					u16_var=pop_value();


					u8* pp=allocate_numeric_array(u16_var);
					type_converter.f_array.f_ptr=(float*)pp;
					type_converter.f_array.size=u16_var;
					pcode_vars[c1]=type_converter.f;
					pcode_vars_type[c1]=VAR_TYPE_FLOAT_ARRAY;

					//PCODE_DEBUG("[float ptr=0x%04x]\n",type_converter.f_array.f_ptr);


					//TODO error checks for correct type and REDIM

					PCODE_DEBUG("[DIM %c(%i)] ",c1+65,u16_var);
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
					c1=*txtpos++; //get variable index (0-25)
					pcode_vars[c1]=initial;

					sf->frame_type 	= STACK_FRAME_TYPE_FOR;
					sf->for_var 	= c1;
					sf->terminal 	= terminal;
					sf->step		= step;
					sf->txt_pos	 	= txtpos;
					sf->current_line= current_line;

					PCODE_DEBUG("[FOR %c %g-%g step %g] ",c1+65,initial,terminal,step);
					break;

				case OP_LOOP_NEXT:
					if(stack_ptr==STACK_DEPTH){
						//nothing on the stack!
						error_code=ERR_NEXT_WITHOUT_FOR;
						break;
					}

					//get the NEXT variable
					u8 var_id=*txtpos++;
					PCODE_DEBUG("[NEXT %c] ",var_id+65);

					if(stack[stack_ptr].frame_type != STACK_FRAME_TYPE_FOR || stack[stack_ptr].for_var!=var_id){
						error_code=ERR_INVALID_NESTING;
						break;
					}
					stack_frame *sf=&stack[stack_ptr];
					s16 current=pcode_vars[(u8)sf->for_var]+=sf->step;	//increment the loop variable with the STEP value

					//Use a different test depending on the sign of the step increment
					if((sf->step > 0 && current <= sf->terminal) || (sf->step < 0 && current >= sf->terminal)){
						//We have to loop so don't pop the stack
						txtpos = sf->txt_pos;
						current_line = sf->current_line;
						PCODE_DEBUG("<L: pos:%02x, cl:%02x, %c=%i>",(u16)txtpos-(u16)program_start,(u16)current_line-(u16)program_start,var_id+'A',current);
					}else{
						//We've run to the end of the loop. drop out of the loop, popping the stack
						stack_ptr++;
					}
					break;

				case OP_LOOP_EXIT:
						PCODE_DEBUG("[EXIT] ");

						if(stack_ptr==STACK_DEPTH){
							//nothing on the stack!
							error_code=ERR_EXIT_WITHOUT_FOR;
							break;
						}

						//check if we saved the NEXT position from a previous loop.
						currpos=txtpos;
						if(currpos[0]!=0 || currpos[1]!=0){
							txtpos=(u8*)((u16)program_start+ (currpos[1]<<8)+currpos[0]);
							//Pop the stack and drop out of the loop
							stack_ptr++;
							break;
						}
						txtpos+=2; //skip the jump adress

						u16 iteration=0;
						found=false;
						while(1){
							while(*txtpos != EOL){
								if(*txtpos == OP_LOOP_NEXT){
									txtpos++;

									//NEXT found, find the variable
									var=*txtpos++ ;
									if(var >= 26){
										error_code=ERR_ILLEGAL_ARGUMENT;
										break;
									}

									//Drop out of the loopand pop the stack
									stack_ptr++;

									//store the NEXT location in the program for a future loop
									//This store the adress relative to the beginning of the prog_mem array
									currpos[0]=((u16)txtpos-(u16)program_start)&0xff;
									currpos[1]=((u16)txtpos-(u16)program_start)>>8;
									found=true;
									break;

								}else{
									//advance to next token
									opcode=(*txtpos++);
									//get opcode's params count to skip over it
									//TODO: add case to support litteral strings ending with zero
									c1=pgm_read_byte(&(opcodes_param_cnt[opcode-0x80]));
									txtpos+=c1;

									iteration++;
								}

							}

							if(found || error_code != ERR_OK) break;

							current_line +=	 current_line[sizeof(LINENUM)];
							current_line_no= (current_line[1]<<8)+current_line[0];
							txtpos = current_line+sizeof(LINENUM)+sizeof(char);
						};

						break;

				case OP_GOTO:

					currpos=txtpos;
					u16 line_to_find=(*((int16_t*)txtpos));

					//check if we saved the actual line's memory position from a previous loop.
					if(line_to_find&0x8000){
						txtpos=((u8*)((line_to_find&=0x7fff)+(u16)program_start ))-1;
						jump=true;
						PCODE_DEBUG("[GOTO CACHED %i] ", txtpos[1]+(txtpos[2]<<8));
						break;
					}

					//othewise skip the jump adress and scan the whole program to find the line
					txtpos+=2;
					u16 line_no=0;

					PCODE_DEBUG("[GOTO %i] ",line_to_find);

					u8* scan_ptr=program_start;
					while(1){
						line_no=(*((int16_t*)scan_ptr));
						if(line_no==0){
							error_code=ERR_UNDEFINED_LINE_NUMBER;
							break;
						}else if(line_no==line_to_find){
							//line found!
							//save position in AVR memory for fast lookup next time
							//set the MSbit to indicate line is an AVR memory location instead of line number.
							//This in turn limits the Basic line numbers to 32767.
							(*((int16_t*)currpos))=((u16)scan_ptr-(u16)program_start)|0x8000;
							txtpos=scan_ptr-1;
							jump=true;
							break;
						}
						//advance to next line;
						scan_ptr+=scan_ptr[2];
					}
					break;

				case OP_GOSUB:
					if(stack_ptr == 0){
						error_code=ERR_STACK_OVERFLOW;
						break;
					}

					//get line number
					currpos=txtpos;
					line_to_find=(*((int16_t*)txtpos));
					txtpos+=2;

					if(line_to_find & 0x8000){
						//we have the target memory address of the line cached
						PCODE_DEBUG("[GOSUB CACHED %i] ",line_to_find&0x7fff);

						//save return address on stack
						sf=&stack[--stack_ptr];
						sf->frame_type 	= STACK_FRAME_TYPE_GOSUB;
						sf->txt_pos	 	= txtpos;
						sf->current_line= current_line;

						txtpos=program_start+(line_to_find&0x7fff)-1;
						jump=true;
						break;
					}

					PCODE_DEBUG("[GOSUB %i] ",line_to_find);

					//txtpos+=2;
					line_no=0;

					scan_ptr=program_start;
					while(1){
						line_no=(*((int16_t*)scan_ptr));
						if(line_no==0){
							error_code=ERR_UNDEFINED_LINE_NUMBER;
							break;
						}else if(line_no==line_to_find){
							//line found!

							//save return address on stack
							sf=&stack[--stack_ptr];
							sf->frame_type 	= STACK_FRAME_TYPE_GOSUB;
							sf->txt_pos	 	= txtpos;
							sf->current_line= current_line;

							//save position in AVR memory for fast lookup next time
							//set the MSbit to indicate line is an AVR memory location instead of line number.
							//This in turn limits the Basic line numbers to 32767.
							(*((int16_t*)currpos))=((u16)scan_ptr-(u16)program_start)|0x8000;
							txtpos=scan_ptr-1;
							jump=true;
							break;
						}
						//advance to next line;
						scan_ptr+=scan_ptr[2];
					}

					break;

				case OP_RETURN:
					PCODE_DEBUG("[RETURN] ");

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
					txtpos = sf->txt_pos;
					current_line = sf->current_line;
					stack_ptr++;
					break;

				case OP_PRINT:
					v1=pop_value();
					PCODE_DEBUG("[PRN VAR %g] ",v1);
					printf_P(PSTR("%g"),v1);
					break;

				case OP_PRINT_LSTR:
					PCODE_DEBUG("[PRN LSTR] ");
					while(1){
						c1=*txtpos++;
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
					PCODE_DEBUG("[PCHR] %c",c1);
					break;

				case OP_CRLF:
					terminal_SendChar(NL);
					terminal_SendChar(CR);
					break;

				case OP_IF:
					v1=pop_value();
					PCODE_DEBUG("[IF %g] ",v1);
					if(!v1){
						//skip to the EOL character
						txtpos=current_line+(current_line[2]-1);
						PCODE_DEBUG("[SKIP]");
					}

					break;

				case OP_CMP_GT:
					v2=pop_value();
					v1=pop_value();
					push_value(v1 > v2);
					PCODE_DEBUG("[CMP %g>%g %i] ",v1,v2,(u8)v1 > v2);
					break;

				case OP_CMP_GE:
					v2=pop_value();
					v1=pop_value();
					push_value(v1 >= v2);
					PCODE_DEBUG("[CMP %i>=%i %i] ",v1,v2,(u8)v1 >= v2);
					break;

				case OP_CMP_LT:
					v2=pop_value();
					v1=pop_value();
					push_value(v1 < v2);
					PCODE_DEBUG("[CMP %g<%g %i] ",v1,v2,(u8)v1 < v2);
					break;

				case OP_ADD:
					v2=pop_value();
					v1=pop_value();
					push_value(v1+v2);
					PCODE_DEBUG("[ADD] ");
					break;

				case OP_SUB:
					v2=pop_value();
					v1=pop_value();
					push_value(v1-v2);
					PCODE_DEBUG("[SUB] ");
					break;

				case OP_MUL:
					v2=pop_value();
					v1=pop_value();
					push_value(v1*v2);
					PCODE_DEBUG("[MUL] ");
					break;

				case OP_DIV:
					v2=pop_value();
					v1=pop_value();
					if(v2==0){
						error_code=ERR_DIVISION_BY_ZERO;
						break;
					}
					push_value(v1/v2);
					PCODE_DEBUG("[DIV] ");
					break;

				case OP_FUNC_TICKS:
					dword=timer_ticks;
					push_value(dword);
					PCODE_DEBUG("[TICKS = %08x ] ",dword);
					break;

				case OP_FUNC_CHR:
					v1=pop_value(); //TODO
					push_value(v1);
					PCODE_DEBUG("[CHR] ");
					break;

				case OP_CLS:
					terminal_Clear();
					PCODE_DEBUG("[CLS] ");
					break;

				case OP_BREAK:
					break;

				case OP_END:
					stop=true;
					break;
			};

			if(error_code){
				print_error(error_code, current_line_no);
				break;
			}

			if(*txtpos == EOL || jump || stop)break;

		}; //while(*txtpos++ != EOL);

		if(error_code || stop)break;
		txtpos++;

		PCODE_DEBUG("\n");

	}while(*((u16*)txtpos)); //did we reached the end of the program (current line no != 0?)

	return error_code;
}


void print_error(u8 error_code, u16 line_number){
	printf_P((PGM_P)pgm_read_word(&(error_messages[error_code])));
	printf_P(PSTR(" in %i\n"),line_number);
}


static inline void push_value(float v){
	if(opcode_sp==0){
		printf_P(PSTR("opcode stack overflow: push"));
		dumpstack();
		dumpvars();
		//dumpmem(0,12);
		while(1);
	}
	pcode_stack[--opcode_sp] = v;
	//if(pcode_sp<pcode_sp_min) pcode_sp_min=pcode_sp;
}

static inline float pop_value(){
	if(opcode_sp==PCODE_STACK_DEPTH){
		printf_P(PSTR("opcode stack overflow: pop"));
		dumpstack();
		//dumpmem(0,12);
		while(1);
	}
	return pcode_stack[opcode_sp++];
}
/**
 * Dumps a selcted range of memory
 */
__attribute__((unused)) void dumpmem(u16 start_addr,u8 rows){
	#if SCREEN_TILES_H>40
		u8 width=16;
	#else
		u8 width=8;
	#endif

	u16 addr;
	printf_P(PSTR("\n"));
	for(u8 i=0;i<rows;i++){
		printf_P(PSTR("%04x:"),(i*width)+start_addr);

		//print the hexadecimal colums
		for(u8 j=0;j<width;j++){
			addr=(i*width)+j+start_addr;

			if(addr==((u16)txtpos-(u16)program_start)){
				printf_P(PSTR(">"));
			}else{
				printf_P(PSTR(" "));
			}

			printf_P(PSTR("%02x"),prog_mem[addr]);

			#if SCREEN_TILES_H>40
				if(j==7)printf_P(PSTR(" "));
			#endif
		}

		//print the ascii representation side
		for(u8 j=0;j<width;j++){
			u8 c=prog_mem[((i*width)+j)+(start_addr)];
			if(c<32 || c>'~') c='.';
			printf_P(PSTR("%c"),c);
		}
		printf_P(PSTR("\r\n"));
	}

}
/**
 * Dumps the LOOP/GOSUB stack
 */
__attribute__((unused)) void dumpstack(){

	printf_P(PSTR("\n"));

	u8 sel;
	u16 cl,pos;
	for(u8 i=0;i<STACK_DEPTH;i++){

		if(i==stack_ptr){
			sel='>';
		}else{
			sel=' ';
		}
		if((u16)stack[i].txt_pos !=0){
			pos=(u16)stack[i].txt_pos-(u16)program_start;
		}else{
			pos=0;
		}
		if((u16)stack[i].current_line !=0){
			cl=(u16)stack[i].current_line-(u16)program_start;
		}else{
			cl=0;
		}

		printf_P(PSTR("%c[%i] type:%03i var:%c to:%i step:%i cl:0x%02x pos:0x%02x ")
			,sel,
			i,
			stack[i].frame_type,
			stack[i].for_var+'A',
			stack[i].terminal,
			stack[i].step,
			cl,
			pos
		);
		printf_P(PSTR("\n"));
	}
	if(stack_ptr==STACK_DEPTH){
		printf_P(PSTR(">\n"));
	}
}
/**
 * Dumps all variables
 */
__attribute__((unused)) void dumpvars(){
	printf_P(PSTR("\n\n"));
	for(int i=0;i<26;i++){
		printf_P(PSTR("%c=%g, "),i+'A',pcode_vars[i]);
	}
	printf_P(PSTR("\n"));

	printf_P(PSTR("[txtpos=0x%02x] [current_line=0x%02x] [current_line_no=%i]\n[pcode-stack size=%i,sp_min=%i] [loop-stack size=%i,ptr=%i]  \n"),
			(u16)txtpos-(u16)program_start,(u16)current_line-(u16)program_start,current_line_no,
			PCODE_STACK_DEPTH, pcode_sp_min,STACK_DEPTH,stack_ptr);

}

const u8 token_rf_benchmark_7[] PROGMEM={
40,0,0,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,0,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
55,0,0,OP_LD_BYTE, 5, OP_DIM,VAR_M, EOL,
60,0,0,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
70,0,0,OP_LD_VAR, VAR_K, OP_LD_BYTE, 2, OP_DIV, OP_LD_BYTE, 3, OP_MUL, OP_LD_BYTE, 4, OP_ADD, OP_LD_BYTE, 5, OP_SUB, OP_ASSIGN, VAR_A, EOL,
80,0,0,OP_GOSUB,120,0,EOL,
82,0,0,OP_LD_BYTE, 1, OP_LD_BYTE, 1, OP_LD_BYTE, 5, OP_LOOP_FOR, VAR_L, EOL,	//1 to 5
83,0,0,OP_LD_VAR, VAR_L, OP_LD_VAR, VAR_A, OP_ASSIGN, VAR_M, EOL,
85,0,0,OP_LOOP_NEXT, VAR_L, EOL,
90,0,0,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
100,0,0,OP_PRINT_LSTR,'E',EOSNL,EOL,
110,0,0,OP_END,EOL,
120,0,0,OP_RETURN,EOL,
0,0
};

const u8 token_rf_benchmark_6[] PROGMEM={
40,0,0,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,0,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
55,0,0,OP_LD_BYTE, 5, OP_DIM,VAR_M, EOL,
//60,0,0,OP_LD_BYTE, 0,OP_LD_BYTE, 128, OP_ASSIGN,VAR_M,EOL,
//65,0,0,OP_LD_BYTE, 4,OP_LD_BYTE, 64, OP_ASSIGN,VAR_M,EOL,
//65,0,0,OP_LD_VAR, VAR_L,OP_LD_VAR, VAR_A, OP_ASSIGN,VAR_M,EOL,
//70,0,0,OP_LD_BYTE, 0, OP_LD_VAR, VAR_M, OP_PRINT, EOL,
//75,0,0,OP_LD_BYTE, 4, OP_LD_VAR, VAR_M, OP_PRINT, EOL,
60,0,0,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
70,0,0,OP_LD_VAR, VAR_K, OP_LD_BYTE, 2, OP_DIV, OP_LD_BYTE, 3, OP_MUL, OP_LD_BYTE, 4, OP_ADD, OP_LD_BYTE, 5, OP_SUB, OP_ASSIGN, VAR_A, EOL,
80,0,0,OP_GOSUB,120,0,EOL,
82,0,0,OP_LD_BYTE, 1, OP_LD_BYTE, 1, OP_LD_BYTE, 5, OP_LOOP_FOR, VAR_L, EOL,	//1 to 5
85,0,0,OP_LOOP_NEXT, VAR_L, EOL,
90,0,0,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
100,0,0,OP_PRINT_LSTR,'E',EOSNL,EOL,
110,0,0,OP_END,EOL,
120,0,0,OP_RETURN,EOL,
0,0
};

const u8 token_rf_benchmark_5[] PROGMEM={
40,0,0,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,0,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
60,0,0,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
70,0,0,OP_LD_VAR, VAR_K, OP_LD_BYTE, 2, OP_DIV, OP_LD_BYTE, 3, OP_MUL, OP_LD_BYTE, 4, OP_ADD, OP_LD_BYTE, 5, OP_SUB, OP_ASSIGN, VAR_A, EOL,
80,0,0,OP_GOSUB,120,0,EOL,
90,0,0,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
100,0,0,OP_PRINT_LSTR,'E',EOSNL,EOL,
110,0,0,OP_END,EOL,
120,0,0,OP_RETURN,EOL,
0,0
};

const u8 token_rf_benchmark_4[] PROGMEM={
40,0,0,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,0,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
60,0,0,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
65,0,0,OP_LD_VAR, VAR_K, OP_LD_BYTE, 2, OP_DIV, OP_LD_BYTE, 3, OP_MUL, OP_LD_BYTE, 4, OP_ADD, OP_LD_BYTE, 5, OP_SUB, OP_ASSIGN, VAR_A, EOL,
70,0,0,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
80,0,0,OP_PRINT_LSTR,'E',EOSNL,EOL,
0,0
};

const u8 token_rf_benchmark_3[] PROGMEM={
40,0,0,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,0,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
60,0,0,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
65,0,0,OP_LD_VAR, VAR_K, OP_LD_VAR, VAR_K, OP_DIV, OP_LD_VAR, VAR_K, OP_MUL, OP_LD_VAR, VAR_K, OP_ADD, OP_LD_VAR, VAR_K, OP_SUB, OP_ASSIGN, VAR_A, EOL,
70,0,0,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
80,0,0,OP_PRINT_LSTR,'E',EOSNL,EOL,
0,0
};


const u8 token_rf_benchmark_2[] PROGMEM={
40,0,0,OP_PRINT_LSTR,'S',EOSNL,EOL,
50,0,0,OP_LD_BYTE, 0, OP_ASSIGN, VAR_K, EOL,
60,0,0,OP_LD_BYTE, 1,OP_LD_VAR, VAR_K,OP_ADD, OP_ASSIGN, VAR_K, EOL,
70,0,0,OP_LD_VAR, VAR_K, OP_LD_WORD, 0xe8, 0x03, OP_CMP_LT, OP_IF, OP_GOTO, 60, 0, EOL, //0 to 1000
80,0,0,OP_PRINT_LSTR,'E',EOSNL,EOL,
0,0
};

const u8 token_rf_benchmark_1[] PROGMEM={
//10,0,0,OP_CLS,EOL,
//15,0,0,OP_PRINT_LSTR,'R','u','g','g','/','F','e','l','d','m','a','n',' ','B','e','n','c','h','m','a','r','k',' ','#','1',EOSNL, EOL,
//20,0,0,OP_FUNC_TICKS,OP_ASSIGN, VAR_T, EOL,
30,0,0,OP_PRINT_LSTR,'S',EOSNL,EOL,
40,0,0,OP_LD_BYTE, 1, OP_LD_BYTE, 1, OP_LD_WORD, 0xe8,0x03, OP_LOOP_FOR, VAR_K, EOL,	//1 to 1000
50,0,0,OP_LOOP_NEXT, VAR_K, EOL,
70,0,0,OP_PRINT_LSTR,'E',EOSNL,EOL,
//90,0,0,OP_FUNC_TICKS,OP_ASSIGN, VAR_U, EOL,
//100,0,0,OP_PRINT_LSTR,'E','l','a','p','s','e','d',' ','t','i','m','e',':',' ',0, EOL,
//110,0,0,OP_LD_VAR, VAR_U, OP_LD_VAR, VAR_T, OP_SUB, OP_LD_BYTE, 60, OP_DIV, OP_ASSIGN, VAR_T, OP_LD_VAR, VAR_T, OP_PRINT, EOL,
//120,0,0,OP_PRINT_LSTR,' ','S','e','c','o','n','d','s',EOSNL,EOL,
0,0
};

const u8* const test_suite[] PROGMEM ={
		token_rf_benchmark_1,
		token_rf_benchmark_2,
		token_rf_benchmark_3,
		token_rf_benchmark_4,
		token_rf_benchmark_5,
		token_rf_benchmark_6,
		token_rf_benchmark_7
};

void run_tests(){
	float results[8];
	float ticks;
	for(u8 i=0;i<8;i++)results[i]=0;


//	load_prog(token_rf_benchmark_5);
//	ticks=timer_ticks;
//	execute(true);
//	results[4]=((float)timer_ticks-ticks)/60;


	u8 res;
	for(u16 i=0;i<sizeof(test_suite)/2;i++){
		load_prog((const u8*)pgm_read_word(&(test_suite[i])));
		printf_P(PSTR("Test #%i...\n"),i+1);
		ticks=timer_ticks;
		res=execute(true);
		if(res!=0){
			print_error(res,0);
			while(1);
		}
		results[i]=((float)timer_ticks-ticks)/60;
		//results[i]=pcode_vars[VAR_T];
	}



	//for(u8 i=0;i<sizeof(test_suite)/2;i++){
	//	printf("test=%i,res=%g\n",i+1,results[i]);
	//}

	terminal_Clear();
	printf_P(PSTR("Rugg/Feldman BASIC Benchmarks\n"));
	printf_P(PSTR("\x81\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x85\x8a\x85\x85\x85\x85\x85\x85\x85\x82\n"));
	printf_P(PSTR("\x86 System      \x86 Test1 \x86 Test2 \x86 Test3 \x86 Test4 \x86 Test5 \x86 Test6 \x86 Test7 \x86 Test8 \x86\n"));
	printf_P(PSTR("\x87\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x85\x8b\x85\x85\x85\x85\x85\x85\x85\x88\n"));

	//printf_P(PSTR("\x86 Uzebox      \x86 0.17  \x86 0.32  \x86 0.8   \x86 ---   \x86 ---   \x86 ---   \x86 ---   \x86 ---   \x86\n"));
	printf_P(PSTR("\x86 Uzebox      \x86 %.2f  \x86 %.2f  \x86 %.2f  \x86 %.2f  \x86 %.2f  \x86 %.2f  \x86 %.2f  \x86 ---   \x86\n"),results[0],results[1],results[2],results[3],results[4],results[5],results[6]);
	printf_P(PSTR("\x86 BBC Micro   \x86 0.8   \x86 3.1   \x86 8.1   \x86 8.7   \x86 9.0   \x86 13.9  \x86 21.1  \x86 49.9  \x86\n"));
	printf_P(PSTR("\x86 ZX 80       \x86 1.5   \x86 4.7   \x86 9.2   \x86 9.0   \x86 12.7  \x86 25.9  \x86 39.2  \x86 NA    \x86\n"));
	printf_P(PSTR("\x86 VIC 20      \x86 1.4   \x86 8.3   \x86 15.5  \x86 17.1  \x86 18.3  \x86 27.2  \x86 42.7  \x86 99    \x86\n"));
	printf_P(PSTR("\x86 Apple II    \x86 1.3   \x86 8.5   \x86 16.0  \x86 17.8  \x86 19.1  \x86 28.6  \x86 44.8  \x86 55.5  \x86\n"));
	printf_P(PSTR("\x86 C64         \x86 1.2   \x86 9.3   \x86 17.6  \x86 19.5  \x86 21.0  \x86 29.5  \x86 47.5  \x86 119.3 \x86\n"));
	printf_P(PSTR("\x86 Altair 8800 \x86 1.9   \x86 7.5   \x86 20.6  \x86 20.9  \x86 22.1  \x86 38.8  \x86 57.0  \x86 67.8  \x86\n"));
	printf_P(PSTR("\x86 Atari 800XL \x86 2.2   \x86 7.3   \x86 19.7  \x86 24.1  \x86 26.3  \x86 40.3  \x86 60.1  \x86 NA    \x86\n"));
	printf_P(PSTR("\x86 ZX Spectrum \x86 4.4   \x86 8.2   \x86 20.0  \x86 19.2  \x86 23.1  \x86 53.4  \x86 77.6  \x86 239.1 \x86\n"));
	printf_P(PSTR("\x86 TI-99/4A    \x86 2.9   \x86 8.8   \x86 22.8  \x86 24.5  \x86 26.1  \x86 61.6  \x86 84.4  \x86 382   \x86\n"));

	printf_P(PSTR("\x83\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x85\x89\x85\x85\x85\x85\x85\x85\x85\x84\n"));
	printf_P(PSTR("See: https://en.wikipedia.org/wiki/Rugg/Feldman_benchmarks#Benchmark_7\n"));

}
