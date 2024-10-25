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

//#define DBGPCODE

#ifdef DBGPCODE
	#define PCODE_DEBUG(fmt, ...) printf_P(PSTR(fmt), ##__VA_ARGS__)
#else
    #define PCODE_DEBUG(...)
#endif

#define RAM_SIZE 1000
#define STACK_DEPTH	5				//depth of the for-loop/gosum stack
#define PCODE_STACK_DEPTH 5			//depth of the internal parameter passing statck
#define VAR_TYPE float				//type used for number variables
#define VAR_SIZE sizeof(VAR_TYPE)	//Size of variables in bytes
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
	int32_t i;
	uint8_t bytes[sizeof(VAR_TYPE)];
}type_converter;

typedef struct {
	u8 frame_type;
	u8 for_var;
	s16 terminal;
	s16 step;
	u8 *current_line;
	u8 *txt_pos;
}stack_frame;


enum opcodes{
	OP_LD_BYTE=0x80, OP_LD_WORD, OP_LD_DWORD, OP_LD_VAR, OP_ASSIGN,
	OP_LOOP_FOR, OP_LOOP_NEXT, OP_LOOP_EXIT,OP_GOTO,OP_GOSUB,
	OP_PRINT, OP_PRINT_LSTR, OP_PRINT_CHAR, OP_CRLF,
	OP_IF, OP_CMP_GT, OP_CMP_GE,
	OP_ADD,	OP_SUB,	OP_MUL,	OP_DIV,
	OP_FUNC_TICKS,OP_FUNC_CHR,
	OP_CLS,
	OP_BREAK,OP_END,
	//OP_CMP_NE, OP_CMP_LT, OP_CMP_LE,
};

//number of parameters required for each opcodes
//const u8 opcodes_param_cnt[] PROGMEM = {
const u8 opcodes_param_cnt[] PROGMEM = {
	1,2,4,1,1,	//OP_LD_BYTE, OP_LD_WORD, OP_LD_FLOAT, OP_LD_VAR, OP_ASSIGN
	1,1,2,2,2,	//OP_LOOP_FOR, OP_LOOP_NEXT, OP_LOOP_EXIT,OP_GOTO,OP_GOSUB,
	0,0,0,0,	//OP_PRINT, OP_PRINT_LSTR, OP_PRINT_CHAR, OP_CRLF
	0,0,0,		//OP_IF, OP_CMP_GT, OP_CMP_GE
	0,0,0,0,	//P_ADD,OP_SUB,	OP_MUL,	OP_DIV
	0,0,		//OP_FUNC_TICKS,OP_FUNC_CHR
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
};


const char error_00_msg[] PROGMEM = "";	//Error code 0 represents OK /no errors
const char error_01_msg[] PROGMEM = "Internal Error";
const char error_02_msg[] PROGMEM = "Stack Overflow in %i";
const char error_03_msg[] PROGMEM = "NEXT without FOR in %i";
const char error_04_msg[] PROGMEM = "Invalid NEXT nesting in %i";
const char error_05_msg[] PROGMEM = "EXIT without FOR in %i";
const char error_06_msg[] PROGMEM = "FOR without NEXT in %i";
const char error_07_msg[] PROGMEM = "Illegal Argument in %i";

const char* const error_messages[] PROGMEM ={
		error_00_msg,
		error_01_msg,
		error_02_msg,
		error_03_msg,
		error_04_msg,
		error_05_msg,
		error_06_msg,
		error_07_msg,
};

static void dumpmem(u16 start_addr,u8 rows);
static void dumpstack();
static void dumpvars();
static void push_value(VAR_TYPE v);
static VAR_TYPE pop_value();
static void printError(u8 error_code, u16 line_no);
static u16 execute();

static u8 *txtpos;
static u32 timer_ticks;
static u8 *program_start;
static u8 *program_end;
static u8 *current_line;
static u16 current_line_no;
static u8 error_code;
u8 prog_mem[RAM_SIZE];

static VAR_TYPE pcode_vars[26]; //A-Z

static u16 pcode_sp_min=PCODE_STACK_DEPTH; //used to debug
static u16 pcode_float_sp=PCODE_STACK_DEPTH;
static VAR_TYPE pcode_float_stack[PCODE_STACK_DEPTH];
static stack_frame stack[STACK_DEPTH];
static s8 stack_ptr=STACK_DEPTH;

const u8 token_rf_benchmark1[] PROGMEM={
10,0,5,OP_CLS,EOL,
15,0,20,OP_PRINT_LSTR,'R','u','g','g','/','F','e','l','d','m','a','n',' ','B','e','n','c','h','m','a','r','k',' ','#','1',0,OP_CRLF, EOL,
20,0,7,OP_FUNC_TICKS,OP_ASSIGN, VAR_T, EOL,
30,0,8,OP_PRINT_LSTR,'S',0,OP_CRLF,EOL,
40,0,13,OP_LD_BYTE, 1, OP_LD_BYTE, 1, OP_LD_WORD, 0xe8,0x03, OP_LOOP_FOR, VAR_K, EOL,	//1 to 1000
50,0,6,OP_LOOP_NEXT, VAR_K, EOL,
70,0,8,OP_PRINT_LSTR,'E',0,OP_CRLF,EOL,
80,0,7,OP_FUNC_TICKS,OP_ASSIGN, VAR_U, EOL,
90,0,20,OP_PRINT_LSTR,'E','l','a','p','s','e','d',' ','t','i','m','e',':',' ',0, EOL,
100,0,13,OP_LD_VAR, VAR_U, OP_LD_VAR, VAR_T, OP_SUB, OP_LD_BYTE, 60, OP_DIV, OP_PRINT, EOL,
110,0,14,OP_PRINT_LSTR,' ','S','e','c','o','n','d','s',0,OP_CRLF,EOL,
0,0
};

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
160,0,10,OP_LD_VAR, VAR_I, OP_LD_BYTE, 126 ,OP_ADD, OP_PRINT_CHAR, EOL,
170,0,6,OP_LOOP_NEXT, VAR_B, EOL,
180,0,5,OP_CRLF, EOL,
190,0,6,OP_LOOP_NEXT, VAR_A, EOL,
195,0,7,OP_FUNC_TICKS,OP_ASSIGN, VAR_U, EOL,
200,0,20,OP_PRINT_LSTR,'E','l','a','p','s','e','d',' ','t','i','m','e',':',' ',0, EOL,
210,0,13,OP_LD_VAR, VAR_U, OP_LD_VAR, VAR_T, OP_SUB, OP_LD_BYTE, 60, OP_DIV, OP_PRINT, EOL,
220,0,14,OP_PRINT_LSTR,' ','S','e','c','o','n','d','s',0,EOL,
0,0
};

void vsyncCallback(){
	timer_ticks++;
	terminal_VsyncCallback();	//used to poll the keyboard
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

	//copy the tokenised program in SRAM
	//for(u16 i=0;i<sizeof(token_mand)-2;i++) prog_mem[i]=pgm_read_byte(&(token_mand[i]));
	for(u16 i=0;i<sizeof(token_rf_benchmark1)-2;i++) prog_mem[i]=pgm_read_byte(&(token_rf_benchmark1[i]));

	execute();

//	dumpstack();
//	dumpvars();
	printf_P(PSTR("\nOk\n>"));
	while(1);

}

u16 execute(){

	program_start = prog_mem;
	program_end = program_start+(sizeof(token_mand)-2);
	txtpos = program_start;

//	u16 opcounter=0; //for debug
//	u16 linecounter=0; //for debug

	u8 c1,var,opcode;
	u16 i1;
	VAR_TYPE v1,v2;
	bool found, stop=false;
	stack_frame *sf;
	error_code=0;

	do{
		current_line=txtpos;
		current_line_no= (txtpos[1]<<8)+txtpos[0];
		txtpos+=sizeof(LINENUM)+sizeof(char); //skip line number and line lenght

		PCODE_DEBUG("%i) <pos:%02x,cl:%02x>",current_line_no,(u16)txtpos-(u16)program_start,(u16)current_line-(u16)program_start);
		while(1){
			opcode = (*txtpos++);

			switch(opcode){
				case OP_LD_BYTE:
					c1=*txtpos++;
					push_value(c1);
					PCODE_DEBUG("[LDB %i] ",c1);
					break;

				case OP_LD_WORD:
					i1=(*((int16_t*)txtpos));
					txtpos+=2;
					push_value(i1);
					PCODE_DEBUG("[LDW %i] ",i1);
					break;

				case OP_LD_DWORD:
					type_converter.i=(*((int32_t*)txtpos));
					txtpos+=4;
					push_value(type_converter.f);
					PCODE_DEBUG("[LDD %g] ",type_converter.f);
					break;

				case OP_LD_VAR:
					c1=*txtpos++;
					v1=pcode_vars[c1];
					push_value(v1);
					PCODE_DEBUG("[LDV %c:%g] ",c1+65,v1);
					break;

				case OP_ASSIGN:
					//Get variable value
					c1=*txtpos++;
					pcode_vars[c1]=pop_value();
					PCODE_DEBUG("[LET %c] ",c1+65,pcode_vars[c1]);
					break;

				case OP_LOOP_FOR:
					if(stack_ptr == 0){
						error_code=ERR_STACK_OVERFLOW;
						break;
					}
					sf=&stack[--stack_ptr];

					VAR_TYPE terminal = pop_value();
					VAR_TYPE initial = pop_value();
					VAR_TYPE step = pop_value();
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
						u8* currpos=txtpos;
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
					break;

				case OP_GOSUB:
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
						if(c1==0)break;
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
						PCODE_DEBUG("[SKIP %02x]",(u16)txtpos-(u16)program_start);
					}
					break;

				case OP_CMP_GT:
					v2=pop_value();
					v1=pop_value();
					push_value(v1 > v2);
					PCODE_DEBUG("[CMP %g>%g %i] ",v1,v2,v1 > v2);
					break;

				case OP_CMP_GE:
					v2=pop_value();
					v1=pop_value();
					push_value(v1 >= v2);
					PCODE_DEBUG("[CMP %i>=%i %i] ",v1,v2,(u8)res);
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
					push_value(v1/v2);
					PCODE_DEBUG("[DIV] ");
					break;

				case OP_FUNC_TICKS:
					u32 ticks=timer_ticks;
					push_value(ticks);
					PCODE_DEBUG("[TICKS = %08x ] ",ticks);
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
				printError(error_code, current_line_no);
				break;
			}
			//opcounter++;
			if(*txtpos == EOL || stop)break;

		}; //while(*txtpos++ != EOL);

		if(error_code || stop)break;

		txtpos++;
		//linecounter++;
		PCODE_DEBUG("\n");
	}while(*((u16*)txtpos)); //did we reached the end of the program?

	return error_code;
}


void printError(u8 error_code, u16 line_number){
	printf_P((PGM_P)pgm_read_word(&(error_messages[error_code])), line_number);
	printf_P(PSTR("\n"));
}


static inline void push_value(VAR_TYPE v){
	if(pcode_float_sp==0){
		printf_P("opcode float stack overflow!");
		while(1);
	}
	pcode_float_stack[--pcode_float_sp] = v;
	//if(pcode_sp<pcode_sp_min) pcode_sp_min=pcode_sp;
}

static inline VAR_TYPE pop_value(){
	if(pcode_float_sp==PCODE_STACK_DEPTH){
		printf_P("opcode float stack overflow!");
		while(1);
	}
	return pcode_float_stack[pcode_float_sp++];
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
