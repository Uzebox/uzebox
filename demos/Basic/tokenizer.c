/**
 * Best time for Mandelbrot: 39.2833 seconds
 */

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <fatfs/ffconf.h>
#include <fatfs/ff.h>
#include <fatfs/diskio.h>
#include <keyboard.h>
#include <spiram.h>
#include "terminal.h"
//#include "fixedptc.h"

#if VIDEO_MODE == 5
	#include "data/font6x8-full.inc"
	#include "data/fat_pixels.inc"
#endif

#define wait_spi_ram_byte() asm volatile("lpm \n\t lpm \n\t lpm \n\t lpm \n\t lpm \n\t lpm \n\t nop \n\t" : : : "r0", "r1"); //wait 19 cycles

#define Wait200ns() asm volatile("lpm\n\tlpm\n\t");
#define Wait100ns() asm volatile("lpm\n\t");
//#define USESPIRAM
//#define SIMSPIRAM
//#define FIXEDMATH
#define TOKEN 1
#define GENPCODE 1
//#define RUNPCODE
//#define DBGPCODE
#define ENABLEFILES

#ifdef FIXEDMATH
	#define VAR_TYPE int32_t//type used for number variables
#else
	#define VAR_TYPE float//type used for number variables
#endif


#ifdef DBGPCODE
	#define PCODE_DEBUG(fmt, ...) printf_P(PSTR(fmt), ##__VA_ARGS__)
#else
    #define PCODE_DEBUG(...)
#endif


#ifdef GENPCODE
    #define PCODE_GEN(...) printf(__VA_ARGS__)
#else
    #define PCODE_GEN(...)
#endif

#if TOKEN == 0
#define kAutorunFilename "MANDEL.BAS"
#else
#define kAutorunFilename "NONE.BAS"
#endif

#include <avr/io.h>



#define PM_INPUT	0
#define PM_OUTPUT	1
#define CONSOLE_BAUD 9600
#define VERSION "0.2"
#define RAM_SIZE 800
#define STACK_DEPTH	5 //10
#define STACK_SIZE (sizeof(struct stack_for_frame)*STACK_DEPTH)
#define VAR_SIZE sizeof(VAR_TYPE)//Size of variables in bytes
#define VAR_TYPE_STR 0
#define VAR_TYPE_NUM 1
#define STRING_BUFFER_SIZE 2 //string buffer
#define HIGHLOW_HIGH	1
#define HIGHLOW_UNKNOWN	4

////////////////////
//ASCII Characters
#define CR	'\r'
#define NL	'\n'
#define LF	0x0a
#define TAB	'\t'
#define BACKSP	'\b'
#define SPACE	 ' '
#define SQUOTE	'\''
#define DQUOTE	'\"'
#define CTRLC	0x03
#define CTRLH	0x08
#define CTRLS	0x13
#define CTRLX	0x18

#define EOL 0xff

typedef u16 LINENUM;

//these will select, at runtime, where IO happens through for load/save
enum{
	kStreamSerial = 0,
	kStreamFile,
	kStreamKeyboard,
	kStreamScreen
};

struct stack_for_frame{
	char frame_type;
	char for_var;
	s16 terminal;
	s16 step;
	u8 *current_line;
#ifdef RUNPCODE
	u16 txt_pos;
#else
	u8 *txt_pos;
#endif
};



typedef union {
	float f;
	int	i;
	u8 c;
	u8* s;
} PVar;

typedef struct {
	u8 data_type;
	PVar var;
}pcode_stack_frame;

struct stack_gosub_frame{
	char frame_type;
	u8 *current_line;
	u8 *txtpos;
};

void printmsgNoNL(const char *msg);
void printmsg(const char *msg);
static void getln(char prompt);
static u8 *findline();
static void toUppercaseBuffer();
void printline();
static VAR_TYPE expr4();
static VAR_TYPE expr3();
static VAR_TYPE expr2();
static VAR_TYPE expression();
void printnum(VAR_TYPE num);
void analogReference(uint8_t mode);
u16 analogRead(u8 pin);
u8 digitalRead(u8 pin);
void analogWrite(u8 pin, u16 val);
void digitalWrite(u8 pin, u8 val);
void tone(u8 f, u8 d);
void noTone();
void pinMode(u8 pin, u8 mode);
static s16 inchar();
static void outchar(char c);
static void line_terminator();
static VAR_TYPE expression();
static bool breakcheck();
uint8_t SpiRamCursorRead(uint16_t addr);
void cmd_Files();
char *filenameWord();
void dump_mem(u16 start_addr,u8 rows);
static u16 test_int_num();
static inline void ignore_blanks();
static void scantable(const u8 *table);
static u8 print_quoted_string();
static void delay(u8 ms);

FATFS fs;
FIL f;
u16 bytesWritten;
u16 bytesRead;
u8 sd_initialized = 0;
u8 inhibitOutput = 0;


static u8 runAfterLoad = 0;
static u8 triggerRun = 0;
static u8 inStream = kStreamKeyboard;
static u8 outStream = kStreamScreen;
static u8 *txtpos,*list_line, *tmptxtpos;
static u8 expression_error;
static u8 expression_return_type;
static u8 *tempsp;
static u32 timer_ticks;
static u8 *stack_limit;
static u8 *program_start;
static u8 *program_end;
static u8 *variables_begin;
static u8 *current_line;
static u8 *sp;
static u16 current_line_no;
#define STACK_GOSUB_FLAG 'G'
#define STACK_FOR_FLAG 'F'
static u8 table_index;
static LINENUM linenum;
static bool debug_stop=false;
static u8 progmem[RAM_SIZE];

static VAR_TYPE *pcode_sp;
static VAR_TYPE pcode_stack[20];
static VAR_TYPE pcode_vars[26]; //A-Z
//static u8 pcode_vars_type[26];

/***********************************************************/
//Keyword table and constants - the last character has 0x80 added to it
static const u8 keywords[] PROGMEM = {
	'I','F'+0x80,
	'N','E','X','T'+0x80,
	'F','O','R'+0x80,
	'E','X','I','T'+0x80,
	'P','R','I','N','T'+0x80,
	'G','O','T','O'+0x80,
	'G','O','S','U','B'+0x80,
	'R','E','T','U','R','N'+0x80,
	'I','N','P','U','T'+0x80,
	'P','O','K','E'+0x80,
	'R','E','M'+0x80,
	'C','L','S'+0x80,
	'L','I','S','T'+0x80,
	'L','O','A','D'+0x80,
	'N','E','W'+0x80,
	'R','U','N'+0x80,
	'S','A','V','E'+0x80,
	'S','T','O','P'+0x80,
	'S','Y','S','T','E','M'+0x80,
	'F','I','L','E','S'+0x80,
	'M','E','M'+0x80,
	'D','U','M','P'+0x80,
	'?'+ 0x80,
	'\''+ 0x80,
	'A','W','R','I','T','E'+0x80,
	'D','W','R','I','T','E'+0x80,
	'D','E','L','A','Y'+0x80,
	'E','N','D'+0x80,
	'R','S','E','E','D'+0x80,
	'C','H','A','I','N'+0x80,
	'L','E','T'+0x80,
	0
};

enum{//by moving the command list to an enum, we can easily remove sections above and below simultaneously to selectively obliterate functionality.
	KW_LET=0,
	KW_IF,
	KW_NEXT,
	KW_FOR,
	KW_EXIT,
	KW_PRINT,
	KW_GOTO, KW_GOSUB, KW_RETURN,
	KW_INPUT,
	KW_POKE,
	KW_REM,
	KW_CLS,
	KW_LIST,
	KW_LOAD, KW_NEW, KW_RUN, KW_SAVE,
	KW_STOP, KW_SYSTEM,
	KW_FILES,
	KW_MEM,
	WM_DUMPMEM,
	KW_QMARK, KW_QUOTE,
	KW_AWRITE, KW_DWRITE,
	KW_DELAY,
	KW_END,
	KW_RSEED,
	KW_CHAIN,
	KW_DEFAULT /* always the final one*/
};

#define FUNC_CHR	0
#define JOY			1
#define FUNC_PEEK	2
#define FUNC_ABS	3
#define FUNC_AREAD	4
#define FUNC_DREAD	5
#define FUNC_RND	6
#define FUNC_TICKS	7
#define FUNC_UNKNOWN 8

static const u8 func_tab[] PROGMEM = {
'C','H','R','$'+0x80,
'J','O','Y'+0x80,
'P','E','E','K'+0x80,
'A','B','S'+0x80,
'A','R','E','A','D'+0x80,
'D','R','E','A','D'+0x80,
'R','N','D'+0x80,
'T','I','C','K','S'+0x80,
0
};

static const u8 to_tab[] PROGMEM = {
	'T','O'+0x80,
	0
};

static const u8 step_tab[] PROGMEM = {
	'S','T','E','P'+0x80,
	0
};

#define RELOP_GE	0
#define RELOP_NE	1
#define RELOP_GT	2
#define RELOP_EQ	3
#define RELOP_LE	4
#define RELOP_LT	5
#define RELOP_NE_BANG	6
#define RELOP_UNKNOWN	7
static const  u8 relop_tab[] PROGMEM = {
	'>','='+0x80,
	'<','>'+0x80,
	'>'+0x80,
	'='+0x80,
	'<','='+0x80,
	'<'+0x80,
	'!','='+0x80,
	0
};

static const u8 highlow_tab[] PROGMEM = {
	'H','I','G','H'+0x80,
	'H','I'+0x80,
	'L','O','W'+0x80,
	'L','O'+0x80,
	0
};



enum pcode_var_type{
	TYPE_FLOAT=0,
	TYPE_INT,
	TYPE_CHAR,
	TYPE_STRING
};



enum pcodes{
	OP_CMD_CLS=0x80,
	OP_LD_BYTE,
	OP_LD_FLOAT,
	OP_ASSIGN,
	OP_LOOP_FOR,
	OP_LD_VAR,
	OP_FUNC_CHR,
	OP_PRINT_CHAR,
	OP_LOOP_NEXT,
	OP_PRINT,
	OP_PRINT_LSTR, //print literral string
	OP_CRLF,
	OP_IF,
	OP_CMP_GT,
	OP_CMP_GE,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_FUNC_TICKS,
	OP_LOOP_EXIT,
	OP_NOP,

//	OP_LD_INT,
//	OP_LD_DBL,


//	OP_CMP_NE,

//	OP_CMP_LT,
//	OP_CMP_LE,
//	OP_OUT_BYTE,
//	OP_OUT_INT,
//	OP_OUT_DBL,
//	OP_OUT_STR,
//	OP_LOOP_EXIT,

//	OP_FUNC_0P,
//	OP_FUNC_1P,



};
enum commands{
	CMD_CLS,
	CMD_PRINT,
	CMD_EXIT,

};
enum param_type{
	LITTERAL_STR,
	LITTERAL_NUM,
	EXPR_STR,
	EXPR_NUM
};

enum vars{
	VAR_A=0,
	VAR_B,
	VAR_C,
	VAR_D,
	VAR_E,
	VAR_F,
	VAR_G,
	VAR_H,
	VAR_I,
	VAR_J,
	VAR_K,
	VAR_L,
	VAR_M,
	VAR_N,
	VAR_O,
	VAR_P,
	VAR_Q,
	VAR_R,
	VAR_S,
	VAR_T,
	VAR_U,
	VAR_V,
	VAR_W,
	VAR_X,
	VAR_Y,
	VAR_Z
};



//
//
const u8 token_mem3[] PROGMEM={
10,0,5,KW_CLS+0x80,0xa,				//10 CLS
20,0,14,KW_LET+0x80,'T','=','T','I','C','K','S','(',')',0xa,
30,0,12,KW_FOR+0x80,'A','=','0','T','O','2','1',0xa,
40,0,12,KW_FOR+0x80,'B','=','0','T','O','3','1',0xa,
50,0,20,KW_LET+0x80,'C','=','0','.','1','0','9','3','8','*','B','-','2','.','5',0xa,
60,0,20,KW_LET+0x80,'D','=','0','.','0','9','0','9','0','*','A','-','1','.','0',0xa,
70,0,8,KW_LET+0x80,'X','=','0',0xa,
80,0,8,KW_LET+0x80,'Y','=','0',0xa,
90,0,12,KW_FOR+0x80,'I','=','0','T','O','1','4',0xa,
100,0,14,KW_LET+0x80,'F','=','X','*','X','+','Y','*','Y',0xa,
110,0,9,KW_IF+0x80,'F','>','4',KW_EXIT+0x80,0xa,
120,0,16,KW_LET+0x80,'E','=','X','*','X','-','Y','*','Y','+','C',0xa,
130,0,14,KW_LET+0x80,'Y','=','2','*','X','*','Y','+','D',0xa,
140,0,8,KW_LET+0x80,'X','=','E',0xa,
150,0,6,KW_NEXT+0x80,'I',0xa,
160,0,17,KW_PRINT+0x80,'C','H','R','$','(','I','+','1','2','6',')',';',0xa,
170,0,6,KW_NEXT+0x80,'B',0xa,
180,0,7,KW_PRINT+0x80,'"','"',0xa,
190,0,6,KW_NEXT+0x80,'A',0xa,
200,0,22,KW_PRINT+0x80,'"','E','l','a','p','s','e','d',' ','t','i','m','e',':',' ','"',';',0xa,
210,0,20,KW_PRINT+0x80,'(','T','I','C','K','S','(',')','-','T',')','/','6','0',';',0xa,
220,0,15,KW_PRINT+0x80,'"',' ','S','e','c','o','n','d','s','"',0xa,
0,0
};


const u8 token_mem[] PROGMEM={


//		20,0,14,KW_LET+0x80,'T','=','T','I','C','K','S','(',')',0xa,
//		30,0,12,KW_FOR+0x80,'A','=','0','T','O','2','1',0xa,
//		40,0,12,KW_FOR+0x80,'B','=','0','T','O','3','1',0xa,
//		50,0,20,KW_LET+0x80,'C','=','0','.','1','0','9','3','8','*','B','-','2','.','5',0xa,
//		60,0,20,KW_LET+0x80,'D','=','0','.','0','9','0','9','0','*','A','-','1','.','0',0xa,
//		70,0,8,KW_LET+0x80,'X','=','0',0xa,
//		80,0,8,KW_LET+0x80,'Y','=','0',0xa,
//		90,0,12,KW_FOR+0x80,'I','=','0','T','O','1','4',0xa,
//		100,0,14,KW_LET+0x80,'F','=','X','*','X','+','Y','*','Y',0xa,
		//110,0,9,KW_IF+0x80,'F','>','4',KW_EXIT+0x80,0xa,
		//120,0,16,KW_LET+0x80,'E','=','X','*','X','-','Y','*','Y','+','C',0xa,

		120,0,16,KW_LET+0x80,'A','=','K','/','2','*','3','+','4','-','5',0xa,

		//130,0,14,KW_LET+0x80,'Y','=','2','*','X','*','Y','+','D',0xa,
		//140,0,8,KW_LET+0x80,'X','=','E',0xa,
//		150,0,6,KW_NEXT+0x80,'I',0xa,
//		160,0,17,KW_PRINT+0x80,'C','H','R','$','(','I','+','1','2','6',')',';',0xa,
//		170,0,6,KW_NEXT+0x80,'B',0xa,
//		180,0,7,KW_PRINT+0x80,'"','"',0xa,
//		190,0,6,KW_NEXT+0x80,'A',0xa,


0,0
};


/**
 * 10 CLS
 * 15 FORT A =1 TO 10
 * 20 E=X*5
 * 25 IF E>15 EXIT
 * 30 PRINT "Value:"; : PRINT E
 * 45 PRINT CHR$(A+65)
 * 50 NEXT A
 */
const u8 token_mem2[] PROGMEM={
10,0,5,OP_CMD_CLS,EOF,
//12,0,5,OP_LD_BYTE, 5,OP_ASSIGN, VAR_B, EOF,
15,0,10,OP_LD_BYTE, 0, OP_LD_BYTE, 3, OP_LOOP_FOR, VAR_A, EOF,
//20,0,0,OP_LD_VAR, VAR_A, OP_LD_BYTE, 5, OP_MUL, OP_ST_VAR, VAR_E, EOF,
//25,0,14,OP_LD_BYTE, 2,OP_LD_VAR,VAR_A, OP_CMP_GT, OP_IF, OP_LD_BYTE, 1,OP_PRINT,OP_CRLF,EOF,
25,0,13,OP_LD_BYTE, 1,OP_LD_VAR,VAR_A, OP_CMP_GT, OP_IF, OP_LOOP_EXIT,0,0,EOF,
//30,0,0,OP_OUT_STR,'V','a','l','u','e',':',0,OP_LD_VAR, VAR_E, OP_OUT_DBL, EOF,
//45,0,8,OP_LD_BYTE, 65, OP_LD_VAR, VAR_A, OP_ADD, OP_FUNC_CHR, OP_OUT_CH,EOF,
45,0,11,OP_LD_BYTE, 65, OP_LD_VAR, VAR_A, OP_MUL, OP_PRINT,OP_CRLF,EOF,
50,0,6,OP_LOOP_NEXT, VAR_A,EOF,
60,0,7,OP_FUNC_TICKS, OP_PRINT,OP_CRLF,EOF,
65,0,8,OP_LD_BYTE,64,OP_FUNC_CHR, OP_PRINT_CHAR,EOF,
//70,0,12,OP_PRINT_LSTR,'D','o','n','e','!',0,OP_CRLF,EOF,
0,0
};

//0.10938=0x9f,0x02,0xe0,0x3d
//0.09090=0xc7,0x29,0xba,0x3d
//2.5 0x00,0x00,0x20,0x40
//1.0 0x00,0x00,0x80,0x3f
const u8 token_mand[] PROGMEM={
10,0,5,OP_CMD_CLS,EOL,
20,0,7,OP_FUNC_TICKS,OP_ASSIGN, VAR_T, EOL,
30,0,10,OP_LD_BYTE, 0, OP_LD_BYTE, 21, OP_LOOP_FOR, VAR_A, EOL,
40,0,10,OP_LD_BYTE, 0, OP_LD_BYTE, 31, OP_LOOP_FOR, VAR_B, EOL,
////#ifdef FIXEDMATH
////50,0,20,OP_LD_FLOAT,0x00,0x1c,0x00,0x00,OP_LD_VAR,VAR_B, OP_MUL,OP_LD_FLOAT,0x00,0x80,0x02,0x00,OP_SUB,OP_ASSIGN, VAR_C, EOL,
////60,0,20,OP_LD_FLOAT,0x45,0x17,0x00,0x00,OP_LD_VAR,VAR_A, OP_MUL,OP_LD_FLOAT,0x00,0x00,0x01,0x00,OP_SUB,OP_ASSIGN, VAR_D, EOL,
////#else
50,0,20,OP_LD_FLOAT,0x9f,0x02,0xe0,0x3d,OP_LD_VAR,VAR_B, OP_MUL,OP_LD_FLOAT,0x00,0x00,0x20,0x40,OP_SUB,OP_ASSIGN, VAR_C, EOL,
60,0,20,OP_LD_FLOAT,0xc7,0x29,0xba,0x3d,OP_LD_VAR,VAR_A, OP_MUL,OP_LD_FLOAT,0x00,0x00,0x80,0x3f,OP_SUB,OP_ASSIGN, VAR_D, EOL,
////#endif
70,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_X, EOL,
80,0,8,OP_LD_BYTE, 0, OP_ASSIGN, VAR_Y, EOL,
90,0,10,OP_LD_BYTE, 0, OP_LD_BYTE, 14, OP_LOOP_FOR, VAR_I, EOL,

100,0,17,OP_LD_VAR, VAR_X, OP_LD_VAR, VAR_X, OP_MUL, OP_LD_VAR, VAR_Y, OP_LD_VAR, VAR_Y, OP_MUL, OP_ADD, OP_ASSIGN, VAR_F, EOL,

110,0,13,OP_LD_VAR, VAR_F, OP_LD_BYTE, 4, OP_CMP_GT, OP_IF, OP_LOOP_EXIT, 0, 0, EOL,
//120,0,20,OP_LD_VAR, VAR_X, OP_LD_VAR, VAR_X, OP_MUL, OP_LD_VAR, VAR_Y, OP_LD_VAR, VAR_Y, OP_MUL, OP_SUB, OP_LD_VAR, VAR_C, OP_ADD, OP_ASSIGN, VAR_E, EOL,
130,0,17,OP_LD_BYTE, 2, OP_LD_VAR, VAR_X, OP_MUL, OP_LD_VAR, VAR_Y, OP_MUL, OP_LD_VAR, VAR_D, OP_ADD, OP_ASSIGN, VAR_Y, EOL,
140,0,8,OP_LD_VAR, VAR_E, OP_ASSIGN, VAR_X, EOL,
150,0,6,OP_LOOP_NEXT, VAR_I, EOL,
160,0,11,OP_LD_VAR, VAR_I, OP_LD_BYTE, 126 ,OP_ADD, OP_FUNC_CHR, OP_PRINT_CHAR, EOL,
170,0,6,OP_LOOP_NEXT, VAR_B, EOL,
180,0,5,OP_CRLF, EOL,
190,0,6,OP_LOOP_NEXT, VAR_A, EOL,
200,0,20,OP_PRINT_LSTR,'E','l','a','p','s','e','d',' ','t','i','m','e',':',' ',0, EOL,
210,0,12,OP_FUNC_TICKS, OP_LD_VAR, VAR_T, OP_SUB, OP_LD_BYTE, 60, OP_DIV, OP_PRINT, EOL,
220,0,14,OP_PRINT_LSTR,' ','S','e','c','o','n','d','s',0,EOL,

0,0
};

//
static const char okmsg[]			PROGMEM = "Ok";
static const char whatmsg[]			PROGMEM = "Syntax error"; //"What? ";
static const char howmsg[]			PROGMEM = "How?";
static const char sorrymsg[]		PROGMEM = "Sorry!";
static const char initmsg[]			PROGMEM = "UzeBASIC " VERSION;
static const char memorymsg[]		PROGMEM = " bytes free.";
static const char breakmsg[]		PROGMEM = "\nBreak!";
//static const char unimplimentedmsg[]	PROGMEM = "Unimplemented";
static const char backspacemsg[]	PROGMEM = "\b \b";
static const char indentmsg[]		PROGMEM = "    ";
//static const char sderrormsg[]		PROGMEM = "ERROR: Failed to initialize SD Card, read/write is disabled.";
//static const char sdsuccessmsg[]	PROGMEM = "SUCCESS: SD is initialized";
static const char sdfilemsg[]		PROGMEM = "ERROR: File Operation failed.";
static const char dirextmsg[]		PROGMEM = "(dir)";
static const char slashmsg[]		PROGMEM = "/";
static const char spacemsg[]		PROGMEM = " ";



//static void push_value(pcode_stack_frame v){
//	pcode_sp--;
//	*pcode_sp = v;
//}
//
//static pcode_stack_frame pop_value(){
//	pcode_stack_frame v;
//	v = *pcode_sp;
//	pcode_sp++;
//	return v;
//}

static void push_value(VAR_TYPE v){
	pcode_sp--;
	*pcode_sp = v;
}

static VAR_TYPE pop_value(){
	VAR_TYPE v;
	v = *pcode_sp;
	pcode_sp++;
	return v;
}


void vsyncCallback(){
	timer_ticks++;
	terminal_VsyncCallback();	//used to poll the keyboard
}

void memdump(u16 start_addr,u8 rows){
	#if SCREEN_TILES_H>40
		u8 width=16;
	#else
		u8 width=8;
	#endif

	printf_P(PSTR("\n"));
	for(u8 i=0;i<rows;i++){
		printf_P(PSTR("%04x: "),(i*width)+start_addr);

		for(u8 j=0;j<width;j++){
			printf_P(PSTR("%02x "),progmem[((i*width)+j)+(start_addr)]);
			#if SCREEN_TILES_H>40
				if(j==7)printf_P(PSTR(" "));
			#endif
		}

		for(u8 j=0;j<width;j++){
			u8 c=progmem[((i*width)+j)+(start_addr)];
			if(c<32 || c>'~') c='.';
			printf_P(PSTR("%c"),c);
		}
		printf_P(PSTR("\r\n"));
	}

	//printf("PB:%i,PE:%i,CL:%i,TP:%i\n",(u16)program_start,(u16)program_end,((u16)current_line-(u16)program_start),((u16)txtpos-(u16)program_start));
	//printf("CL:%04x,TP:%04x\n",((u16)current_line-(u16)program_start),((u16)txtpos-(u16)program_start));
}


/***************************************************************************/
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

	/////Serial.println(sentinel);
	//printmsg(initmsg);


	GetPrngNumber(GetTrueRandomSeed());
	if(GetPrngNumber(0) == 0)
		GetPrngNumber(0xACE);

//	InitMusicPlayer(patches);
//	SetMasterVolume(224);
//	DDRA |= (1<<PA6);//enable Uzenet module
//	PORTD |=(1<<PD3);//reset module
//	UBRR0H = 0;
//	UBRR0L = 185;//9600
//	UCSR0A = (1<<U2X0);//double speed
//	UCSR0C = (1<<UCSZ01)+(1<<UCSZ00)+(0<<USBS0);//8N1
//	UCSR0B = (1<<RXEN0)+(1<<TXEN0);//enable TX & RX


	sd_initialized = 0;//try 10 times to initialize else fail
	//printmsgNoNL(PSTR("Initializing SD..."));
	for(u8 i=0;i<10;i++){
		if(f_mount(0, &fs) != FR_OK){//if(f_mount(&fs, "", 1) != FR_OK){
			PORTD &= ~(1<<6);//deassert card
			WaitVsync(30);//wait
			continue;
		}else{
			//printmsg(PSTR("Success!"));
			sd_initialized = 1;
			break;
		}
	}
	if(!sd_initialized){
		//printmsg(PSTR("ERROR"));
	}
#ifndef RUNPCODE
//	outStream = kStreamScreen;
//	inStream = kStreamKeyboard;
//	inhibitOutput = 0;
//
//	printmsgNoNL(PSTR("Searching for "));
//	printmsgNoNL(PSTR(kAutorunFilename));
//	printmsgNoNL(PSTR("..."));
//	if(f_open(&f, kAutorunFilename, FA_OPEN_EXISTING|FA_READ) == FR_OK){//try to load autorun file if present
//		printmsg(PSTR("Loaded"));
//		program_end = program_start;
//		inStream = kStreamFile;
//		inhibitOutput = 1;
//		runAfterLoad = 0;
//	}else
//		printmsg(PSTR("Not Found"));
	u8 *start;
	u8 *newEnd;
	u8 linelen;
	u8 isDigital;
	u8 alsoWait = 0;
	VAR_TYPE val,val2;
	u8 var;
	char *filename;
#endif

#ifdef RUNPCODE

#ifdef USESPIRAM
	SpiRamInit();
	u8 line_buf[80];

	//copy the tokenised program in SPI RAM
	SpiRamSeqWriteStart(0,0);
	for(u16 i=0;i<sizeof(token_mand)-2;i++){
		progmem[i]=pgm_read_byte(&(token_mand[i]));
	}
	SpiRamSeqWriteEnd();



	//copy first line to execute in buffer
	SpiRamReadInto(0, u16 addr, void* dst, u16 len);

	program_start=0;

#else
	/**
	 * u8  progmem[]  	= Progmam memory or line buffer
	 * u16 progPos 		= absolute position in the program
	 * u16 proglinepos	= absolute position to the start of the current in the program
	 * u16 progendpos	= end of the program in memory
	 * u16 linenum		= current line number
	 */

	u16 progPos = 0;
	u16 progLinePos=0;
	u16 progEndPos=0;
	u16 lineNum=0;


	//copy the tokenised program in SRAM
	for(u16 i=0;i<sizeof(token_mand)-2;i++){
		progmem[i]=pgm_read_byte(&(token_mand[i]));
	}
	//program_start = progmem;

#endif

	sp = progmem+sizeof(progmem);	//Needed for printnum
	stack_limit = progmem+sizeof(progmem)-STACK_SIZE;
	progEndPos = (sizeof(token_mand)-2);

	pcode_sp = pcode_stack+sizeof(pcode_stack-1);
	//txtpos = program_start;


	do{
#ifdef USESPIRAM
		txtpos=current_line;

#else

#endif

#ifdef SIMSPIRAM
		//simulate get line from spi ram in sequencial mode
		u8 line_len=progPos[2];
		//send adress
		wait_spi_ram_byte();
		wait_spi_ram_byte();
		wait_spi_ram_byte();
		wait_spi_ram_byte();
		for(u8 i=0;i<(line_len);i++){
			wait_spi_ram_byte();
		}
		wait_spi_ram_byte(); //add another 19 cycles for call overhead and setup
#endif

		progLinePos=progPos;	//set to beginning of new line
		progPos+=sizeof(LINENUM)+sizeof(char); //skip line number and line lenght
		lineNum=(progmem[progLinePos+1]<<8)+progmem[progLinePos];	   //get the line number

		//PCODE_DEBUG("%i) <plp:%02x,prp:%02x>",linenum,progLinePos,progPos);
		PCODE_DEBUG("%i) ",linenum);

		while(1){
			table_index = progmem[progPos++];	//get next opcode

			u8 c,vi;
			VAR_TYPE v1,v2;//,vres;
			bool res;
			bool found;

			switch(table_index){
				case OP_CMD_CLS:
					terminal_Clear();
					PCODE_DEBUG("[CLS] ");
					break;

				case OP_LD_BYTE:
					//push the next byte
					c=progmem[progPos++];
#ifdef FIXEDMATH
					push_value(((u32)c)<<16);
#else
					push_value(c);
#endif

					PCODE_DEBUG("[LDB %i] ",c);
					break;

				case OP_LD_FLOAT:
					union {
					  float f;
					  int32_t i;
					  uint8_t bytes[sizeof(VAR_TYPE)];
					}u;
					for(u8 i=0;i<4;i++){
						u.bytes[i]=progmem[progPos++];
					}

#ifdef FIXEDMATH
					push_value(u.i);
					PCODE_DEBUG("[LDF %i] ",u.i);
#else
					push_value(u.f);
					PCODE_DEBUG("[LDF %g] ",u.f);
#endif

					break;

				case OP_ASSIGN:
					//Get variable value
					c=progmem[progPos++];
					pcode_vars[c]=pop_value();
					PCODE_DEBUG("[LET %c] ",c+65,pcode_vars[c]);
					break;

				case OP_LOOP_FOR:
					struct stack_for_frame *f;
					if(sp + sizeof(struct stack_for_frame) < stack_limit){
						//goto QSORRY;
						printf_P("Out of stack.");
					}
					sp -= sizeof(struct stack_for_frame);
					f = (struct stack_for_frame *)sp;
#ifdef FIXEDMATH
					VAR_TYPE terminal = pop_value()>>16;
					VAR_TYPE initial = pop_value()>>16;
#else
					VAR_TYPE terminal = pop_value();
					VAR_TYPE initial = pop_value();
#endif
					VAR_TYPE step=1; //TODO
					c=progmem[progPos++]; // variable index (0-25)

					//((VAR_TYPE *)variables_begin)[var] = initial;
					pcode_vars[c]=initial;
					f->frame_type = STACK_FOR_FLAG;
					f->for_var = c;
					f->terminal = terminal;
					f->step		 = step;
					f->txt_pos	 = progPos;

#ifdef FIXEDMATH
					PCODE_DEBUG("[FOR %c %i-%i] ",c+65,initial,terminal);
#else
					PCODE_DEBUG("[FOR %c %g-%g] ",c+65,initial,terminal);
#endif
					break;
				case OP_LD_VAR:
					vi=progmem[progPos++];
					v1=pcode_vars[vi];
					push_value(v1);
#ifdef FIXEDMATH
					PCODE_DEBUG("[LDV %c:%i] ",vi+65,v1);
#else
					PCODE_DEBUG("[LDV %c:%g] ",vi+65,v1);
#endif
					break;

				case OP_FUNC_CHR:
					v1=pop_value(); //TODO
					push_value(v1);
					PCODE_DEBUG("[CHR] ");
					break;

				case OP_PRINT_CHAR:
#ifdef FIXEDMATH
					c=(pop_value()>>16)&0xff;
#else
					c=pop_value();
#endif
					terminal_SendChar(c);
					PCODE_DEBUG("[PCHR] %c",c);
					break;

				case OP_LOOP_NEXT:
					tempsp = sp;
					found=false;
					while(tempsp < progmem+sizeof(progmem)-1){//walk up the stack frames and find the frame we want(if present)
						switch(tempsp[0]){
//						case STACK_GOSUB_FLAG:
//							break;
						case STACK_FOR_FLAG:
							//Flag, Var, Final, Step
							if(table_index == OP_LOOP_NEXT){
								struct stack_for_frame *f = (struct stack_for_frame *)tempsp;
								PCODE_DEBUG("[NEXT %c] ",f->for_var+65);

#ifdef FIXEDMATH
								pcode_vars[(u8)f->for_var]+=((u32)f->step<<16);
								s16 current=pcode_vars[(u8)f->for_var]>>16;
#else
								pcode_vars[(u8)f->for_var]+=f->step;
								u8 current=pcode_vars[(u8)f->for_var];


								//s16 current=pcode_vars[(u8)f->for_var]+=f->step;
								//u8 current=pcode_vars[(u8)f->for_var];
#endif

								PCODE_DEBUG("<%c=%i>",f->for_var+65,current );

								//Use a different test depending on the sign of the step increment
								if((f->step > 0 && current <= f->terminal) || (f->step < 0 && current >= f->terminal)){
									//We have to loop so don't pop the stack
									progPos = f->txt_pos;
									//current_line = f->current_line;
									found=true;

									PCODE_DEBUG("<L: pos:%02x, cl:%02x>",(u16)progPos-(u16)program_start,(u16)current_line-(u16)program_start);
									break;
								}
								//We've run to the end of the loop. drop out of the loop, popping the stack
								sp = tempsp + sizeof(struct stack_for_frame);
								progPos++;
								found=true;

								PCODE_DEBUG("<EL: pos:%02x, cl:%02x>",(u16)progPos-(u16)program_start,(u16)current_line-(u16)program_start);

								break;
							}
							//This is not the loop you are looking for... so Walk back up the stack
							tempsp += sizeof(struct stack_for_frame);
							break;
						default:
							//printmsg(PSTR("Stack is stuffed!\n"));
							while(1);
						}

						if(found) break;
					}

					break;
				case OP_PRINT:
					v1=pop_value();
#ifdef FIXEDMATH
					PCODE_DEBUG("[PRN VAR %08x] ",v1);
					//printf_P(PSTR("%i"),v1>>16);			 //TODO print in decimal format
					fixedpt_str(v1,5);
#else
					PCODE_DEBUG("[PRN VAR %g] ",v1);
					printf_P(PSTR("%g"),v1);
#endif
					break;

				case OP_PRINT_LSTR:
					PCODE_DEBUG("[PRN LSTR] ");
					while(1){
						c=progmem[progPos++];
						if(c==0)break;
						terminal_SendChar(c);
					}
					break;

				case OP_CRLF:
					terminal_SendChar(NL);
					terminal_SendChar(CR);
					break;

				case OP_IF:
					v1=pop_value();
#ifdef FIXEDMATH
					PCODE_DEBUG("[IF %i] ",v1);
#else
					PCODE_DEBUG("[IF %g] ",v1);
#endif
					if(v1==0){
						//skip to end of line
						//progPos=current_line+(current_line[2]-1);
						progPos=progLinePos+progmem[progLinePos+2]-1;
						PCODE_DEBUG("[SKIP %02x]",(u16)progPos-(u16)program_start);
					}
					break;

				case OP_CMP_GT:
					v2=pop_value();
					v1=pop_value();
					push_value(v1 > v2);
#ifdef FIXEDMATH
					PCODE_DEBUG("[CMP %i>%i %i] ",v1,v2,v1 > v2);
#else
					PCODE_DEBUG("[CMP %g>%g %i] ",v1,v2,v1 > v2);
#endif
					break;

				case OP_CMP_GE:
					v2=pop_value();
					v1=pop_value();
					res = (v1 >= v2);
					push_value(res);
#ifdef FIXEDMATH
					PCODE_DEBUG("[CMP %g>=%g %i] ",v1,v2,(u8)res);
#else
					PCODE_DEBUG("[CMP %i>=%i %i] ",v1,v2,(u8)res);
#endif
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
#ifdef FIXEDMATH
					push_value(fixedpt_xmul(v1,v2));
#else
					push_value(v1*v2);
#endif
					PCODE_DEBUG("[MUL] ");
					break;

				case OP_DIV:
					v2=pop_value();
					v1=pop_value();
#ifdef FIXEDMATH
					push_value(fixedpt_xdiv(v1,v2));
#else
					push_value(v1/v2);
#endif
					PCODE_DEBUG("[DIV] ");
					break;

				case OP_FUNC_TICKS:
#ifdef FIXEDMATH
					push_value(timer_ticks<<16);
#else
					push_value(timer_ticks);
#endif
					break;
				case OP_LOOP_EXIT:
					PCODE_DEBUG("[EXIT] ");

					//asm("wdr");
					u16 currpos=progPos;

					//check if we saved the NEXT position from a previous loop.
					if(progmem[currpos]!=0 || progmem[currpos+1]!=0){
						//progPos=(currpos[1]<<8)+currpos[0];
						progPos=(progmem[currpos+1]<<8) + progmem[currpos];
						//Drop out of the loop, popping the stack
						sp = sp + sizeof(struct stack_for_frame);
						break;
					}

					//scan the code to find a NEXT token

					found=false;
					while(1){
							//current_line +=	 current_line[sizeof(LINENUM)];
							progLinePos+=progmem[progLinePos+sizeof(LINENUM)];

							#ifdef SIMSPIRAM
									//simulate get line from spi ram in sequencial mode
									u8 line_len=current_line[2];
									//send adress
									wait_spi_ram_byte();
									wait_spi_ram_byte();
									wait_spi_ram_byte();
									wait_spi_ram_byte();
									for(u8 i=0;i<(line_len);i++){
										wait_spi_ram_byte();
									}
									wait_spi_ram_byte(); //add another 19 cycles for call overhead and setup
							#endif

							//TODO: fix this (progendpos always zero)
							if(progLinePos == progEndPos){
								printf_P(PSTR("FOR without NEXT."));
								//goto QHOW;//Out of lines to run
							}
							progPos = progLinePos+sizeof(LINENUM)+sizeof(char); //skip line # and size

							while(progmem[progPos] != EOL){
								if(progmem[progPos++] == OP_LOOP_NEXT){

									//NEXT found, find the variable
									if(progmem[progPos++] > 26){
										printf_P(PSTR("Invalid variable."));
										//goto QHOW;
									}

									//Drop out of the loop, popping the stack
									//struct stack_for_frame *f = (struct stack_for_frame *)sp;
									//if(f->for_var != progPos[-1]) goto QHOW;
									sp = sp + sizeof(struct stack_for_frame);

									//store the NEXT location in the program for a future loop
									progmem[currpos]=progPos&0xff;
									progmem[currpos+1]=progPos>>8;

									found=true;
									break;
								}
							}
							if(found)break;
						}
					break;
				case OP_NOP:
					//asm("wdr");
					break;

			};

			//printf("<pos_st=%i>",(u16)progPos-(u16)program_start);
			if(progmem[progPos] == 0xff)break;

		}; //while(*progPos++ != 0xff); //EOL?

		//current_line=progPos-1;
		progPos++;
		//debug++;
		//PCODE_DEBUG("<EOL: pos:%02x %c=%g>\n",(u16)progPos-(u16)program_start,65,pcode_vars[0]);
		PCODE_DEBUG("\n");




		//if(table_index==OP_LOOP_NEXT)while(1);

	}while(progmem[progPos] != 0);

	//memdump(0,4);




	printf("\nOk\n>");
	while(1);
#else


	program_start = progmem;
	program_end = program_start;
	sp = progmem+sizeof(progmem);	//Needed for printnum
	stack_limit = progmem+sizeof(progmem)-STACK_SIZE;
	variables_begin = stack_limit - 27*VAR_SIZE;

	//memory free
	printnum(variables_begin-program_end);
	//printmsg(memorymsg);


#if TOKEN == 1
//	for(u8 i=0;i<sizeof(token_mem2)-2;i++){progmem[i]=pgm_read_byte(&(token_mem2[i]));} program_end = program_start+(sizeof(token_mem2)-2);
	for(u16 i=0;i<sizeof(token_mem)-2;i++){progmem[i]=pgm_read_byte(&(token_mem[i]));} program_end = program_start+(sizeof(token_mem)-2);

	current_line = program_start;
	//dump_mem(0,6);


	goto EXECLINE;
#endif

WARMSTART:


	//this signifies that it is running in 'direct' mode.
	current_line = 0;
	sp = progmem+sizeof(progmem);
	printmsg(okmsg);

PROMPT:
	if(triggerRun){
		triggerRun = 0;
		current_line = program_start;
		goto EXECLINE;
	}

	getln('>');

	toUppercaseBuffer();
	txtpos = program_end+sizeof(u16);
	while(*txtpos != NL)//find the end of the freshly entered line
		txtpos++;

	u8 *dest;//move it to the end of program_memory
	dest = variables_begin-1;
	while(1){
		*dest = *txtpos;
		if(txtpos == program_end+sizeof(u16))
			break;
		dest--;
		txtpos--;
	}
	txtpos = dest;

	linenum = test_int_num();//now see if we have a line number
	ignore_blanks();
	if(linenum == 0)
		goto DIRECT;

	if(linenum == 0xFFFF)
		goto QHOW;

	linelen = 0;
	while(txtpos[linelen] != NL)//find the length of what's left, including the (not yet populated) line header
		linelen++;
	linelen++;//Include the NL in the line length
	linelen += sizeof(u16)+sizeof(char);//Add space for the line number and line length

	//Now we have the number, add the line header.
	txtpos -= 3;

	*((u16 *)txtpos) = linenum;
	txtpos[sizeof(LINENUM)] = linelen;


	//Merge it into the rest of the program
	start = findline();

	//If a line with that number exists, then remove it
	if(start != program_end && *((LINENUM *)start) == linenum){
		u8 *dest, *from;
		unsigned tomove;

		from = start + start[sizeof(LINENUM)];
		dest = start;

		tomove = program_end - from;
		while(tomove > 0){
			*dest = *from;
			from++;
			dest++;
			tomove--;
		}
		program_end = dest;
	}

	if(txtpos[sizeof(LINENUM)+sizeof(char)] == NL)//If the line has no txt, it was just a delete
		goto PROMPT;



	//Make room for the new line, either all in one hit or lots of little shuffles
	while(linelen > 0){
		u16 tomove;
		u8 *from,*dest;
		u16 space_to_make;

		space_to_make = txtpos - program_end;

		if(space_to_make > linelen)
			space_to_make = linelen;
		newEnd = program_end+space_to_make;
		tomove = program_end - start;


		//Source and destination - as these areas may overlap we need to move bottom up
		from = program_end;
		dest = newEnd;
		while(tomove > 0){
			from--;
			dest--;
			*dest = *from;
			tomove--;
		}

		//Copy over the bytes into the new space
		for(tomove = 0; tomove < space_to_make; tomove++){
			*start = *txtpos;
			txtpos++;
			start++;
			linelen--;
		}
		program_end = newEnd;
	}
	goto PROMPT;

//UNIMPLEMENTED:
//	printmsg(unimplimentedmsg);
//	goto PROMPT;

QHOW:
	printmsg(howmsg);
	//printf("Curent line=0x%02x\n",(u16)current_line-(u16)program_start);

	goto PROMPT;

QWHAT:
	line_terminator();
	printmsgNoNL(whatmsg);
	if(current_line != NULL){
		printmsgNoNL(PSTR(" in "));
		u8 tmp = *txtpos;
		if(*txtpos != NL)
			*txtpos = '^';
		list_line = current_line;
		printline();
		*txtpos = tmp;
	}else{
		line_terminator();
	}
	printmsg(okmsg);
	goto PROMPT;

QSORRY:
	printmsg(sorrymsg);
	goto WARMSTART;

RUN_NEXT_STATEMENT:
	while(*txtpos == ':')
		txtpos++;
	ignore_blanks();
	if(*txtpos == NL)
		goto EXECNEXTLINE;
	goto INTERPRET_AT_TXT_POS;

DIRECT:
	txtpos = program_end+sizeof(LINENUM);
	if(*txtpos == NL)
		goto PROMPT;

INTERPRET_AT_TXT_POS:
	if(breakcheck()){
		printf_P(PSTR("\nBreak on line %i\n"),(current_line[1]<<8)+current_line[0]);
		goto WARMSTART;
	}

	//printf("<txtpos:0x%02x>",((u16)txtpos-(u16)program_start));
#if TOKEN == 0
	scantable(keywords);
	//printf("Table_index:%i",(int)table_index);
	//printf("<EXECNLN TXTPOS:%03x>\n",(u16)txtpos-(u16)program_start);

#else
	table_index = (*txtpos++);
	if(!(table_index&0x80)){
		//printf("Invalid token: %02x\n",table_index);
		//while(1);
	}
	table_index&=0x7f;
#endif

//	if(debug_stop){
//		while(1);
//	}

	//printf("<idx:%i,txtpos:0x%02x>\n",(int)table_index,((u16)txtpos-(u16)program_start));
	//if(debug_loop>2){while(1);}else{debug_loop++;}

	switch(table_index){
	case KW_DELAY:
			expression_error = 0;
			val = expression();
			delay(val);
			goto EXECNEXTLINE;
	case KW_FILES:
		goto FILES;
	case KW_LIST:
		goto LIST;
	case KW_CHAIN:
		goto CHAIN;
	case KW_LOAD:
		goto LOAD;
	case KW_MEM:
		goto MEM;
	case WM_DUMPMEM:
		memdump(0,6);
		goto RUN_NEXT_STATEMENT;
	case KW_NEW:
		if(txtpos[0] != NL)
			goto QWHAT;
		program_end = program_start;
		goto PROMPT;
	case KW_RUN:
		current_line = program_start;
		goto EXECLINE;
	case KW_SAVE:
		goto SAVE;
	case KW_NEXT:
		goto NEXT;
	case KW_LET:
		goto ASSIGNMENT;
	case KW_IF:
		//terminal_Clear();
		expression_error = 0;
		//printf("1-CL:%04x,TP:%04x\n",((u16)current_line-(u16)program_start),((u16)txtpos-(u16)program_start));
		VAR_TYPE val = expression();
		if(expression_error || *txtpos == NL)
			goto QHOW;

		#if GENPCODE == 1
			printf_P(PSTR("[IF %i]"),(u8)val);
		#endif

		if(val != 0){
			//printf("2-CL:%04x,TP:%04x\n",((u16)current_line-(u16)program_start),((u16)txtpos-(u16)program_start));
			//dump_mem(0xf0,12);
			//debug_stop=true;
			goto INTERPRET_AT_TXT_POS;
		}
		//printf("3-CL:%04x,TP:%04x\n",((u16)current_line-(u16)program_start),((u16)txtpos-(u16)program_start));
		//printf("<IF val:%i>",(u16)val);
		//dump_mem(10*8,12);
		//debug_stop=true;
		goto EXECNEXTLINE;

	case KW_GOTO:
		expression_error = 0;
		linenum = expression();
		if(expression_error || *txtpos != NL)
			goto QHOW;
		current_line = findline();
		goto EXECLINE;

	case KW_GOSUB:
		goto GOSUB;
	case KW_RETURN:
		goto GOSUB_RETURN;
	case KW_REM:
	case KW_QUOTE:
		goto EXECNEXTLINE;	//Ignore line completely
	case KW_FOR:
		goto FORLOOP;
	case KW_INPUT:
		goto INPUT;
	case KW_PRINT:
	case KW_QMARK:
		goto PRINT;
	case KW_POKE:
		goto POKE;
	case KW_END:
	case KW_STOP:
		//This is the easy way to end - set the current line to the end of program attempt to run it
		if(txtpos[0] != NL)
			goto QWHAT;
		current_line = program_end;
		goto EXECLINE;
	case KW_SYSTEM://Leave the basic interperater
		goto WARMSTART;

	case KW_AWRITE:	//AWRITE <pin>, HIGH|LOW
		isDigital = 0;
		goto AWRITE;
	case KW_DWRITE:	//DWRITE <pin>, HIGH|LOW
		isDigital = 1;
		goto DWRITE;

	case KW_RSEED:
		goto RSEED;

	//case KW_TONEW:
	//	alsoWait = 1;
	//case KW_TONE:
	//	goto TONEGEN;
	//case KW_NOTONE:
	//	goto TONESTOP;
	case KW_CLS:
		goto CLS;
	case KW_EXIT:
		goto EXIT;

	case KW_DEFAULT:
		goto ASSIGNMENT;
	default:
		break;
	}

EXECNEXTLINE:

	if(current_line == NULL) goto PROMPT;//Processing direct commands?
	current_line_no = (u8)current_line[-2];






	current_line +=	 current_line[sizeof(LINENUM)];

	//printf("<EXECNEXTLN CL:%03x>",(u16)current_line-(u16)program_start);

EXECLINE:

	#if GENPCODE == 1
		printf_P(PSTR("\n[LN:%i]"),(u16)current_line[0]);
		//dump_mem(0,10);
		//while(1);
	#endif



	txtpos = current_line+sizeof(LINENUM)+sizeof(char);
	//printf("<EXECNLN TXTPOS:%03x>\n",(u16)txtpos-(u16)program_start);
	goto INTERPRET_AT_TXT_POS;

INPUT:
		ignore_blanks();
		if(*txtpos < 'A' || *txtpos > 'Z') goto QWHAT;
		var = *txtpos;
		txtpos++;
		ignore_blanks();
		if(*txtpos != NL && *txtpos != ':') goto QWHAT;
INPUTAGAIN:
		tmptxtpos = txtpos;
		getln('?');
		toUppercaseBuffer();
		txtpos = program_end+sizeof(u16);
		ignore_blanks();
		expression_error = 0;
		val = expression();
		if(expression_error)
			goto INPUTAGAIN;
		((VAR_TYPE *)variables_begin)[var-'A'] = val;
		txtpos = tmptxtpos;

		goto RUN_NEXT_STATEMENT;

FORLOOP:
		ignore_blanks();
		if(*txtpos < 'A' || *txtpos > 'Z') goto QWHAT;
		var = *txtpos;

		txtpos++;
		ignore_blanks();
		if(*txtpos != '=') goto QWHAT;
		txtpos++;
		ignore_blanks();

		expression_error = 0;
		VAR_TYPE initial = expression();
		if(expression_error) goto QWHAT;

		scantable(to_tab);
		if(table_index != 0) goto QWHAT;

		VAR_TYPE terminal = expression();
		if(expression_error) goto QWHAT;

		scantable(step_tab);
		VAR_TYPE step;
		if(table_index == 0){
			step = expression();
			if(expression_error) goto QWHAT;
		}else{
			step = 1;
		}
		ignore_blanks();
		if(*txtpos != NL && *txtpos != ':') goto QWHAT;


		if(!expression_error && *txtpos == NL){
			//printf("%c>",var);
			struct stack_for_frame *f;
			if(sp + sizeof(struct stack_for_frame) < stack_limit) goto QSORRY;
			sp -= sizeof(struct stack_for_frame);
			f = (struct stack_for_frame *)sp;
			((VAR_TYPE *)variables_begin)[var-'A'] = initial;
			f->frame_type = STACK_FOR_FLAG;
			f->for_var = var;
			f->terminal = terminal;
			f->step		 = step;
			f->txt_pos	 = txtpos;
			f->current_line = current_line;
			//f->exit_pos = 0;

			#if GENPCODE == 1
				printf_P(PSTR("[FOR][%c]"),var);
			#endif

			goto RUN_NEXT_STATEMENT;
		}
	goto QHOW;

GOSUB:


	expression_error = 0;
	linenum = expression();
	if(!expression_error && *txtpos == NL){
		struct stack_gosub_frame *f;
		if(sp + sizeof(struct stack_gosub_frame) < stack_limit)
			goto QSORRY;

		sp -= sizeof(struct stack_gosub_frame);
		f = (struct stack_gosub_frame *)sp;
		f->frame_type = STACK_GOSUB_FLAG;
		f->txtpos = txtpos;
		f->current_line = current_line;
		current_line = findline();
		goto EXECLINE;
	}
	goto QHOW;

//exit the current for-next loop
EXIT:
	//Locate the NEXT statement by scanning through the program memory
	if(*txtpos != NL) {
		//printf("<here>");
		goto QWHAT; //EXIT must be the last statement on line
	}


	//if(debug_stop){
	//		while(1);
	//	}


	//did we already have the NEXT statement location?
//	struct stack_for_frame *f2 = (struct stack_for_frame *)sp;
//	if(f2->exit_pos !=0){
//		printf("<ep:%04x>",(u16)f2->exit_pos-(u16)program);
//		txtpos=f2->exit_pos;
//		sp = sp + sizeof(struct stack_for_frame); //pop the stack
//		goto RUN_NEXT_STATEMENT;
//	}

	while(1){
		current_line +=	 current_line[sizeof(LINENUM)];
		if(current_line == program_end){
			printf_P(PSTR("FOR without NEXT"));
			goto QHOW;//Out of lines to run
		}
		txtpos = current_line+sizeof(LINENUM)+sizeof(char);

		while(*txtpos != NL){
			if(*txtpos++ == KW_NEXT+0x80){
				//NEXT found, find the variable name
				ignore_blanks();
				if(*txtpos < 'A' || *txtpos > 'Z') goto QHOW;
				txtpos++;
				ignore_blanks();
				if(*txtpos != ':' && *txtpos != NL) goto QWHAT;

				//Drop out of the loop, popping the stack
				struct stack_for_frame *f = (struct stack_for_frame *)sp;
				if(f->for_var != txtpos[-1]) goto QHOW;
				sp = sp + sizeof(struct stack_for_frame);
				//printf("*");
				goto RUN_NEXT_STATEMENT;
			}
		}


//		char next[]={'N','E','X','T'};
//		u8 i=0;
//		while(*txtpos != NL){
//			if(*txtpos++ != next[i]){
//				i=0;
//			}else{
//				i++;
//				if(i==4){
//					//NEXT found, find the variable name
//					ignore_blanks();
//					if(*txtpos < 'A' || *txtpos > 'Z') goto QHOW;
//					txtpos++;
//					ignore_blanks();
//					if(*txtpos != ':' && *txtpos != NL) goto QWHAT;
//
//					//Drop out of the loop, popping the stack
//					struct stack_for_frame *f = (struct stack_for_frame *)sp;
//					if(f->for_var != txtpos[-1]) goto QHOW;
//					sp = sp + sizeof(struct stack_for_frame);
//					printf("*");
//					goto RUN_NEXT_STATEMENT;
//
//				}
//			}
//		}




	}

NEXT:
	ignore_blanks();//find the variable name
	if(*txtpos < 'A' || *txtpos > 'Z') goto QHOW;
	char v=*txtpos;
	txtpos++;
	ignore_blanks();
	if(*txtpos != ':' && *txtpos != NL) goto QWHAT;

	#if GENPCODE == 1
		printf_P(PSTR("[NEXT][%c]"),v);
		while(1);
	#endif


GOSUB_RETURN:
	tempsp = sp;
	while(tempsp < progmem+sizeof(progmem)-1){//walk up the stack frames and find the frame we want(if present)
		switch(tempsp[0]){
		case STACK_GOSUB_FLAG:
			if(table_index == KW_RETURN){
				struct stack_gosub_frame *f = (struct stack_gosub_frame *)tempsp;
				current_line	= f->current_line;
				txtpos			= f->txtpos;
				sp += sizeof(struct stack_gosub_frame);
				goto RUN_NEXT_STATEMENT;
			}
			//This is not the loop you are looking for... so Walk back up the stack
			tempsp += sizeof(struct stack_gosub_frame);
			break;
		case STACK_FOR_FLAG:
			//Flag, Var, Final, Step
			if(table_index == KW_NEXT){
				struct stack_for_frame *f = (struct stack_for_frame *)tempsp;
				//Is the the variable we are looking for?
				if(txtpos[-1] == f->for_var){
					VAR_TYPE *varaddr = ((VAR_TYPE *)variables_begin) + txtpos[-1] - 'A';
					*varaddr = *varaddr + f->step;
					//Use a different test depending on the sign of the step increment
					if((f->step > 0 && *varaddr <= f->terminal) || (f->step < 0 && *varaddr >= f->terminal)){

						//dump_mem(0xa0,20);
						//f->exit_pos=txtpos-7;
						//printf("[ep:%04x]",(u16)f->txt_pos-(u16)program);


						//We have to loop so don't pop the stack
						txtpos = f->txt_pos;
						current_line = f->current_line;
						goto RUN_NEXT_STATEMENT;
					}
					//We've run to the end of the loop. drop out of the loop, popping the stack
					sp = tempsp + sizeof(struct stack_for_frame);
					goto RUN_NEXT_STATEMENT;
				}
			}
			//This is not the loop you are looking for... so Walk back up the stack
			tempsp += sizeof(struct stack_for_frame);
			break;
		default:
			printmsg(PSTR("Stack is stuffed!\n"));
			goto WARMSTART;
		}
	}
	//Didn't find the variable we've been looking for
	goto QHOW;

ASSIGNMENT:
	if(*txtpos < 'A' || *txtpos > 'Z') goto QHOW;
	VAR_TYPE *pvar = (VAR_TYPE *)variables_begin + *txtpos - 'A';
	char tmp=*txtpos;

	txtpos++;
	ignore_blanks();

	if (*txtpos != '=') goto QWHAT;
	txtpos++;
	ignore_blanks();
	expression_error = 0;
	val = expression();
	if(expression_error) goto QWHAT;
	if(*txtpos != NL && *txtpos != ':') goto QWHAT;//check that we are at the end of the statement
	*pvar = val;

	#if GENPCODE == 1
		printf_P(PSTR("[LET %c]"),tmp);
	#endif

	goto RUN_NEXT_STATEMENT;

CLS:
	//erase screen
	terminal_Clear();

	#if GENPCODE == 1
		printf_P(PSTR("[CLS]"));
	#endif

	goto RUN_NEXT_STATEMENT;

POKE:
	expression_error = 0;
	val = expression();//work out where to put it
	if(expression_error) goto QWHAT;
	//u8 *address = (u8 *)val;

	ignore_blanks();//check for a comma
	if (*txtpos != ',') goto QWHAT;
	txtpos++;
	ignore_blanks();
	expression_error = 0;
	val = expression();//get the value to assign
	if(expression_error) goto QWHAT;
	//printf("Poke %p value %i\n",address, (u8)value);
	//Check that we are at the end of the statement
	if(*txtpos != NL && *txtpos != ':') goto QWHAT;
	goto RUN_NEXT_STATEMENT;

LIST:
	linenum = test_int_num();//Retuns 0 if no line found.

	//Should be EOL
	if(txtpos[0] != NL)
		goto QWHAT;

	//Find the line
	list_line = findline();
	while(list_line != program_end)
		printline();
	goto WARMSTART;

PRINT:
	//If we have an empty list then just put out a NL
	if(*txtpos == ':'){
		line_terminator();
		txtpos++;
		goto RUN_NEXT_STATEMENT;
	}
	if(*txtpos == NL){
		goto EXECNEXTLINE;
	}

	while(1){
		ignore_blanks();
		//printf("<*txtpos:%c>",*txtpos);
		//if(print_quoted_string()){
		if(*txtpos == '"' || *txtpos == '\''){
			#if GENPCODE == 1
				printf_P(PSTR("[PRNS]"),*txtpos);
			#endif

			print_quoted_string();

		}else if(*txtpos == '"' || *txtpos == '\''){
			//printf("<str err>");
			goto QWHAT;
		}else{
			VAR_TYPE e;
			expression_error = 0;
			expression_return_type=VAR_TYPE_NUM;
			e = expression();
			if(expression_error) goto QWHAT;
			if(expression_return_type==VAR_TYPE_STR){
				#if GENPCODE == 1
					printf_P(PSTR("[PRNC]"));
				#endif

				//putchar((char)e); //todo: support string buffer
				terminal_SendChar((char)e);
			}else{
				printnum(e);
			}
		}

		//At this point we have three options, a comma or a new line
		if(*txtpos == ','){
			txtpos++;	//Skip the comma and move onto the next
		}else if(txtpos[0] == ';' && (txtpos[1] == NL || txtpos[1] == ':')){
			txtpos++;//This has to be the end of the print - no newline
			break;
		}else if(*txtpos == NL || *txtpos == ':'){

			#if GENPCODE == 1
				printf_P(PSTR("[CRLF]"));
			#endif

			line_terminator();	//The end of the print statement
			break;
		}else
			goto QWHAT;
	}
	goto RUN_NEXT_STATEMENT;

MEM:
	//memory free
	printnum(variables_begin-program_end);
	printmsg(memorymsg);
	goto RUN_NEXT_STATEMENT;


	/*************************************************/
AWRITE://AWRITE <pin>,val
DWRITE:
	expression_error = 0;
	VAR_TYPE pinNo = expression();//get the pin number
	if(expression_error) goto QWHAT;

	ignore_blanks();//check for a comma
	if (*txtpos != ',') goto QWHAT;
	txtpos++;
	ignore_blanks();

	//u8 *txtposBak = txtpos;
	scantable(highlow_tab);
	if(table_index != HIGHLOW_UNKNOWN){
		if(table_index <= HIGHLOW_HIGH){
			val = 1;
		}else{
			val = 0;
		}
	}else{//and the value (numerical)
		expression_error = 0;
		val = expression();
		if(expression_error) goto QWHAT;
	}
	pinMode(pinNo, PM_OUTPUT);
	if(isDigital){
		digitalWrite(pinNo, val);
	}else{
		analogWrite(pinNo, val);
	}
	goto RUN_NEXT_STATEMENT;

FILES:
	cmd_Files();
	goto WARMSTART;

CHAIN:
	runAfterLoad = 1;

LOAD:
	program_end = program_start;//clear the program
	expression_error = 0;
	filename = filenameWord();//work out the filename
	if(expression_error) goto QWHAT;

	if(f_open(&f, (const char*)filename, FA_READ) == FR_OK){
		inStream = kStreamFile;//this will kickstart a series of events to read in from the file.
		inhibitOutput = 1;
	}else{
		printmsg(sdfilemsg);
	}

	goto WARMSTART;

SAVE:
	expression_error = 0;
	filename = filenameWord();//work out the filename
	if(expression_error) goto QWHAT;

	//open the file(overwrite if existing), switch over to file output
	if(f_open(&f, (const char *)filename, FA_WRITE) == FR_OK){//|FA_CREATE_ALWAYS
		outStream = kStreamFile;
	}else{
		printmsg(sdfilemsg);
	}

	list_line = findline();//copied from "List"
	while(list_line != program_end)
		printline();

	outStream = kStreamScreen;//go back to standard output, close the file
	f_close(&f);
	goto WARMSTART;

RSEED:
	expression_error = 0;
	val = expression();//get pin number
	if(expression_error) goto QWHAT;

	GetPrngNumber(val);
	goto RUN_NEXT_STATEMENT;

	return 0;
#endif
}

//returns 1 if the character is valid in a filename
static s16 isValidFnChar(char c){
	if(c >= '0' && c <= '9') return 1;//number
	if(c >= 'A' && c <= 'Z') return 1;//LETTER
	if(c >= 'a' && c <= 'z') return 1;//letter (for completeness)
	if(c == '_') return 1;
	if(c == '+') return 1;
	if(c == '.') return 1;
	if(c == '~') return 1;	//Window~1.txt

	return 0;
}

char *filenameWord(){
	//SDL - I wasn't sure if this functionality existed above, so I figured i'd put it here
	u8 * ret = txtpos;
	expression_error = 0;

	//make sure there are no quotes or spaces, search for valid characters
	//while(*txtpos == SPACE || *txtpos == TAB || *txtpos == SQUOTE || *txtpos == DQUOTE) txtpos++;
	while(!isValidFnChar(*txtpos)) txtpos++;
	ret = txtpos;

	if(*ret == '\0'){
		expression_error = 1;
		return (char *)ret;
	}

	//now, find the next nonfnchar
	txtpos++;
	while(isValidFnChar(*txtpos)) txtpos++;
	if(txtpos != ret) *txtpos = '\0';

	//set the error code if we've got no string
	if(*ret == '\0'){
		expression_error = 1;
	}

	return (char *)ret;
}


static void line_terminator(){
	outchar(NL);
	outchar(CR);
}

/***********************************************************/
static bool breakcheck(){

	if(terminal_HasChar()){
		if(terminal_GetChar()==CTRL_C){
			return true;
		}
	}

	return false;
}
/***********************************************************/
static s16 inchar(){
	s16 v;
	switch(inStream){
	case(kStreamKeyboard):

		//why blocking?
		while(1){
			while(!terminal_HasChar()){}
			v=terminal_GetChar();
			return v;
		}

		break;
	case(kStreamFile):
		if(GetVsyncFlag()) WaitVsync(1);
		f_read(&f, &v, 1, &bytesRead);
		if(bytesRead != 1){
			f_close(&f);
			goto INCHAR_LOADFINISH;
		}
		if(v == NL) v=CR;//file translate

		return v;
		break;
	 case(kStreamSerial):
	default:
//		while(1){
//			if(GetVsyncFlag()) WaitVsync(1);
//			if(UartUnreadCount())
//				return UartReadChar();
//		}
	}

INCHAR_LOADFINISH:
	inStream = kStreamKeyboard;
	inhibitOutput = 0;

	if(runAfterLoad){
		runAfterLoad = 0;
		triggerRun = 1;
	}
	return NL;//trigger a prompt.
}

/***********************************************************/
static void outchar(char c){
	if(inhibitOutput) return;

	if(outStream == kStreamScreen){
		//ConsolePrintChar(c);
		terminal_SendChar(c);
	}else if(outStream == kStreamFile){
		f_write(&f, &c, 1, &bytesWritten);
	}else{
		//while(IsUartTxBufferFull());
		//UartSendChar(c);
	}
}

void cmd_Files(){
	DIR d;
	if(f_opendir(&d, "/") != FR_OK)
		return;
	FILINFO entry;

	while(1){
		if(GetVsyncFlag()) WaitVsync(1);
		if(f_readdir(&d, &entry) != FR_OK || entry.fname[0] == 0)
			break;
		//common header
		printmsgNoNL(indentmsg);
		printmsgNoNL((const char *)entry.fname);
		if(entry.fattrib & AM_DIR){
			printmsgNoNL(slashmsg);
			u8 found_end = 0;
			for(u8 i=0; i<13 ; i++){
				if(entry.fname[i] == '\0')
					found_end = 1;
				if(found_end)
					printmsgNoNL(spacemsg);
			}
			printmsgNoNL(dirextmsg);
		}else{//file ending
			u8 found_end = 0;
			for(u8 i=0; i<13 ; i++){
				if(entry.fname[i] == '\0')
					found_end = 1;
				if(found_end)
					printmsgNoNL(spacemsg);
			}
			printnum(entry.fsize);
		}
		line_terminator();
	}
	f_close(&f);
}


#define NO_SPI_RAM 1

#ifndef NO_SPI_RAM
	#include <spiram.h>

	uint16_t spiram_cursor = 0;
	uint16_t spiram_state = 0;

	uint8_t SpiRamCursorInit(){
		spiram_cursor = RAM_SIZE;
		spiram_state = 0;
		if(!SpiRamInit())
			return 0;
		SpiRamSeqReadStart((RAM_SIZE<<16)&0xFF, (uint16_t)RAM_SIZE&0xFFFF);
		return 1;
	}

	uint8_t SpiRamCursorRead(uint16_t addr){
		if(spiram_state){//in a sequential write?
			SpiRamSeqWriteEnd();
			asm("nop");asm("nop");
			SpiRamSeqReadStart(0, addr);
			asm("nop");asm("nop");
			spiram_state = 0;
			spiram_cursor = addr+1;
			return SpiRamSeqReadU8();
		}
		if(spiram_cursor != addr){//current sequential read position doesn't match?
			SpiRamSeqReadEnd();
			asm("nop");asm("nop");
			SpiRamSeqReadStart(0, addr);
			asm("nop");asm("nop");
			spiram_cursor = addr+1;
			return SpiRamSeqReadU8();
		}
		spiram_cursor++;
		return SpiRamSeqReadU8();
	}

	void SpiRamCursorWrite(uint16_t addr, uint8_t val){
		if(!spiram_state){//in a sequential read?
			SpiRamSeqReadEnd();
			asm("nop");asm("nop");
			SpiRamSeqWriteStart(0, addr);
			spiram_state = 1;
			spiram_cursor = addr+1;
			asm("nop");asm("nop");
			SpiRamSeqWriteU8(val);
			return;
		}
		if(spiram_cursor != addr){//current sequential write position doesn't match?
			SpiRamSeqWriteEnd();
			asm("nop");asm("nop");
			SpiRamSeqWriteStart(0, addr);
			spiram_cursor = addr+1;
			asm("nop");asm("nop");
			SpiRamSeqWriteU8(val);
			return;
		}
		spiram_cursor++;
		SpiRamSeqWriteU8(val);
	}
#endif


	static void delay(u8 ms){
		s16 time = ms;
		while(time > 0){
			if(GetVsyncFlag()){
				WaitVsync(1);
				time -= 16;
				continue;
			}
			for(u16 i=0;i<1000;i++){
				for(u8 j=0;j<5;j++){
					Wait200ns();
				}
			}
			time--;
		}
	}
	/***************************************************************************/
	static inline void ignore_blanks(){
		while(*txtpos == SPACE || *txtpos == TAB){
			txtpos++;
		}
	}


	/***************************************************************************/
	static void scantable(const u8 *table){
		s16 i = 0;
		table_index = 0;
		while(1){
			//if(GetVsyncFlag()) WaitVsync(1);
			if(pgm_read_byte(table) == 0)//run out of table entries?
				return;

			if(txtpos[i] == pgm_read_byte(table)){//do we match this character?
				i++;
				table++;
			}else{//do we match the last character of keywork (with 0x80 added)? If so, return
				if(txtpos[i]+0x80 == pgm_read_byte(table)){
					txtpos += i+1;//Advance the pointer to following the keyword
					ignore_blanks();
					return;
				}
				while((pgm_read_byte(table) & 0x80) == 0)//Forward to the end of this keyword
					table++;

				table++;////Now move on to the first character of the next word...
				table_index++;
				ignore_blanks();//...and reset the position index
				i = 0;
			}
		}
	}

	/***************************************************************************/
	static void pushb(u8 b){
		sp--;
		*sp = b;
	}

	/***************************************************************************/
	static u8 popb(){
		u8 b;
		b = *sp;
		sp++;
		return b;
	}

	/***************************************************************************/
	void printnum(VAR_TYPE num){
		printf_P(PSTR("%g"),num);
	}

	void printUnum(u16 num){
		s16 digits = 0;

		do{
			pushb(num%10+'0');
			num = num/10;
			digits++;
		}
		while(num > 0);

		while(digits > 0){
			outchar(popb());
			digits--;
		}
	}

	/***************************************************************************/
	static u16 test_int_num(){
		u16 num = 0;
		ignore_blanks();

		while(*txtpos>= '0' && *txtpos <= '9'){
			if(num >= 0xFFFF/10){//trap overflows
				num = 0xFFFF;
				break;
			}

			num = num *10 + *txtpos - '0';
			txtpos++;
		}
		return num;
	}

	/***************************************************************************/
	static u8 print_quoted_string(){
		s16 i=0;
		u8 delim = *txtpos++;
		//if(delim != '"' && delim != '\'')
		//	return 0;
		//txtpos++;

		while(txtpos[i] != delim){//check we have a closing delimiter
			if(txtpos[i] == NL)
				return 0;
			i++;
		}

		while(*txtpos != delim){//print the characters
			#if GENPCODE == 1
				printf_P(PSTR("[%c]"),*txtpos);
			#endif

			outchar(*txtpos);
			txtpos++;
		}

		#if GENPCODE == 1
			printf_P(PSTR("[NUL]"),*txtpos);
		#endif

		txtpos++;//skip over the last delimiter
		return 1;
	}


	/***************************************************************************/
	void printmsgNoNL(const char *msg){
		while(pgm_read_byte(msg) != 0){
			outchar(pgm_read_byte(msg++));
		}
	}

	/***************************************************************************/
	void printmsg(const char *msg){
		printmsgNoNL(msg);
		line_terminator();
	}

	/***************************************************************************/
	static void getln(char prompt){
		outchar(prompt);
		txtpos = program_end+sizeof(LINENUM);

		while(1){
			//if(GetVsyncFlag()) WaitVsync(1);
			char c = inchar();
			switch(c){
			case NL:
				//break;
			case CR:
				line_terminator();
				txtpos[0] = NL;//Terminate all strings with a NL
				return;
			case BACKSP:
				if(txtpos == program_end+sizeof(LINENUM))
					break;
				txtpos--;
				printmsgNoNL(backspacemsg);
				break;
			default://We need to leave at least one space to allow us to shuffle the line into order
				if(txtpos == variables_begin-2){
					outchar(BACKSP);
				}else{
					txtpos[0] = c;
					txtpos++;
					outchar(c);
				}
			}
		}
	}

	/***************************************************************************/
	static u8 *findline(){
		u8 *line = program_start;
		while(1){
			if(line == program_end)
				return line;

			if(((LINENUM *)line)[0] >= linenum)
				return line;

			line += line[sizeof(LINENUM)];//Add the line length onto the current address, to get to the next line
		}
	}

	/***************************************************************************/
	static void toUppercaseBuffer(){
		u8 *c = program_end+sizeof(LINENUM);
		u8 quote = 0;

		while(*c != NL){
			//Are we in a quoted string?
			if(*c == quote)
				quote = 0;
			else if(*c == '"' || *c == '\'')
				quote = *c;
			else if(quote == 0 && *c >= 'a' && *c <= 'z')
				*c = *c + 'A' - 'a';
			c++;
		}
	}

	/***************************************************************************/
	void printline(){
		LINENUM line_num;

		line_num = *((LINENUM *)(list_line));
		list_line += sizeof(LINENUM) + sizeof(char);

		//Output the line
		printnum(line_num);
		outchar(' ');
		while(*list_line != NL){
			outchar(*list_line);
			list_line++;
		}
		list_line++;
		line_terminator();
	}

	/***************************************************************************/
	static VAR_TYPE expr4(){
		//fix provided by Jurg Wullschleger wullschleger@gmail.com for whitespace and unary operations
		ignore_blanks();

		if(*txtpos == '-'){
			txtpos++;
			return -expr4();
		}
		//end fix


		//Is it a literral number?
		if(*txtpos=='-' || (*txtpos >= '0' && *txtpos <= '9')){
			const char *numpos=(char*)txtpos;
			char *endptr;

			do{
				txtpos++;
			}while((*txtpos >= '0' && *txtpos <= '9') || *txtpos=='.' ); //todo: add support for exponential notation

			u8 save=*txtpos; //save current char
			*txtpos=0;		//to add zero terminator
			VAR_TYPE num=(VAR_TYPE)strtod(numpos, &endptr);
			*txtpos=save;
			if(endptr==numpos)goto EXPR4_ERROR; //invalid float format

			#if GENPCODE == 1

				if(num<256){
					printf_P(PSTR("[LDB][%02x]"),(u8)num);
				}else{
					printf_P(PSTR("[LDD][%04x]"),num);
				}

			#endif



			return num;

		}



		//Is it a function or variable reference?
		if(txtpos[0] >= 'A' && txtpos[0] <= 'Z'){
			VAR_TYPE a;
			//Is it a variable reference (single alpha)
			if(txtpos[1] < 'A' || txtpos[1] > 'Z'){
				a = ((VAR_TYPE *)variables_begin)[*txtpos - 'A'];
				char v=txtpos[0];
				txtpos++;

				#if GENPCODE == 1
					printf_P(PSTR("[LVAR][%c]"),v);
				#endif


				return a;
			}

			//Is it a function with a single parameter
			scantable(func_tab);
			if(table_index == FUNC_UNKNOWN)
				goto EXPR4_ERROR;

			u8 f = table_index;
			u8 params;

			if(*txtpos != '(')
				goto EXPR4_ERROR;

			txtpos++;

			if(*txtpos == ')'){
				params=0;
			}else{
				a = expression();
				if(*txtpos != ')') goto EXPR4_ERROR;
				params=1;
			}
			txtpos++;

			switch(f){

				case FUNC_PEEK:
					if(params==0) goto EXPR4_ERROR;
					if(a < RAM_SIZE){
						return progmem[(u16)a];
					}else{
						return 0;
						//return SpiRamCursorRead(a);
					}
				case FUNC_ABS:
					if(params==0) goto EXPR4_ERROR;
					if(a < 0)
						return -a;
					return a;

				case FUNC_AREAD:
					if(params==0) goto EXPR4_ERROR;
					pinMode(a, PM_INPUT);
					return analogRead(a);

				case FUNC_DREAD:
					if(params==0) goto EXPR4_ERROR;
					pinMode(a, PM_INPUT);
					return digitalRead(a);

				case FUNC_RND:
					if(params==0) goto EXPR4_ERROR;
					return(GetPrngNumber(0) % (u16)a);

				case FUNC_CHR:
					if(params==0) goto EXPR4_ERROR;
					expression_return_type=VAR_TYPE_STR;

					#if GENPCODE == 1
						printf_P(PSTR("[CHR]"));
					#endif
					return a;

				case FUNC_TICKS:
					#if GENPCODE == 1
						printf_P(PSTR("[TICKS]"));
					#endif
					return (VAR_TYPE)timer_ticks;
			}
		}

		if(*txtpos == '('){
			txtpos++;
			VAR_TYPE a = expression();
			if(*txtpos != ')')
				goto EXPR4_ERROR;

			txtpos++;
			return a;
		}

	EXPR4_ERROR:
		expression_error = 1;
		return 0;
	}

	/***************************************************************************/
	static VAR_TYPE expr3(){
		VAR_TYPE a = expr4();
		ignore_blanks();//fix for eg:	100 a = a + 1


		while(1){
			//printf("<exp3 *tpos=%c",*txtpos);
			VAR_TYPE b;
			if(*txtpos == '*'){
				txtpos++;
				b = expr4();

				#if GENPCODE == 1
					printf_P(PSTR("[MUL %g*%g]"),a,b);
				#endif

				a *= b;
			}else if(*txtpos == '/'){
				txtpos++;
				b = expr4();

				#if GENPCODE == 1
					printf_P(PSTR("[DIV %g/%g]"),a,b);
				#endif

				if(b != 0)
					a /= b;
				else
					expression_error = 1;
			}else{
				//printf("<exp3=%d>",(int)a);
				return a;
			}
		}
	}

	/***************************************************************************/
	static VAR_TYPE expr2(){
		VAR_TYPE a;

		if(*txtpos == '-' || *txtpos == '+'){
			a = 0;
			//printf("<exp2=0");
		}else{
			a = expr3();
			//printf("<exp2=%d>",(int)a);
		}

		while(1){
			VAR_TYPE b;
			//printf("<exp2 *textpos=%i>",(int)*txtpos);
			if(*txtpos == '-'){
				txtpos++;
				b = expr3();

				#if GENPCODE == 1
					printf_P(PSTR("[SUB %g-%g]"),a,b);
				#endif

				a -= b;
			}else if(*txtpos == '+'){
				txtpos++;
				b = expr3();

				#if GENPCODE == 1
					printf_P(PSTR("[ADD %g+%g]"),a,b);
				#endif

				a += b;



			}else{
				//printf("<exp2 ret %d>",(int)a);
				return a;
			}
		}
	}
	/***************************************************************************/
	static VAR_TYPE expression(){
		VAR_TYPE a = expr2();
		VAR_TYPE b;

		//Check if we have an error
		if(expression_error)	return a;

		scantable(relop_tab);
		if(table_index == RELOP_UNKNOWN)
			return a;

		switch(table_index){
		case RELOP_GE:

			b = expr2();

			#if GENPCODE == 1
				printf_P(PSTR("[CP %i>=%i]"),(u8)a,(u8)b);
			#endif

			if(a >= b) return 1;
			break;
		case RELOP_NE:
		case RELOP_NE_BANG:
			b = expr2();
			if(a != b) return 1;
			break;
		case RELOP_GT:
			b = expr2();
			if(a > b) return 1;
			break;
		case RELOP_EQ:
			b = expr2();
			if(a == b) return 1;
			break;
		case RELOP_LE:
			b = expr2();
			if(a <= b) return 1;
			break;
		case RELOP_LT:
			b = expr2();
			if(a < b) return 1;
			break;
		}
		return 0;
	}



	void analogReference(uint8_t mode){
	}

	u16 analogRead(u8 pin){
		return 0;
	}
	u8 digitalRead(u8 pin){
		return 0;
	}

	void analogWrite(u8 pin, u16 val){
	}

	void digitalWrite(u8 pin, u8 val){
	}

	void pinMode(u8 pin, u8 mode){
	}
