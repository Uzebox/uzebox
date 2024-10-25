/*
 * terminal.h
 *
 * Video terminal emulator. Partially emulates the vt100 commands set.
 *
 * The terminal is made of two parts:
 *
 *   Receiver:	Allows the host (main program) to send characters to the video display.
 *   			The receivers is able to handle a subset of ANSI escape codes to handle
 *   			things such as cursor location, screen clearing and inverse video. The
 *   			receiver also recognizes some multi-bytes UTF-8 sequences in order
 *   			to recognize and display bullets and basic box drawing characters.
 *
 *	Transmitter:Allows the host to obtain the next characters available from the keyboard.
 *				The terminal will return ANSI escape sequences for extended keys such
 *				as cursor keys, insert, home, functions keys (F1-F12) etc.
 *
 *  Created on: Oct 13, 2015
 *      Author: admin
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <uzebox.h>
#include <stdio.h>

#define TERM_OUTPUT_NON_ALPHA 	1
#define TERM_COLOR_DEFAULT	 	38
#define TERM_ATTR_INVERSE_VIDEO 2

/*
 * Control codes
 */
#define CTRL_NULL	0x00	//NULL
#define CTRL_A		0x01	//ctrl-A
#define CTRL_B	 	0x02	//ctrl-B, ctrl-break
#define CTRL_C		0x03	//ctrl-C
#define CTRL_D		0x04	//ctrl-D
#define CTRL_E		0x05	//ctrl-E
#define CTRL_F 		0x06	//ctrl-F
#define CTRL_G		0x07	//ctrl-G
#define CTRL_H		0x08	//ctrl-H, backspace
#define CTRL_I		0x09	//ctrl-I, tab
#define CTRL_J		0x0a	//ctrl-J
#define CTRL_K	 	0x0b	//ctrl-K
#define CTRL_L		0x0c	//ctrl-L
#define CTRL_M		0x0d	//ctrl-M, enter
#define CTRL_N		0x0e	//ctrl-N
#define CTRL_O		0x0f	//ctrl-O
#define CTRL_P		0x10	//ctrl-P
#define CTRL_Q		0x11	//ctrl-Q
#define CTRL_R		0x12	//ctrl-R
#define CTRL_S		0x13	//ctrl-S
#define CTRL_T		0x14	//ctrl-T
#define CTRL_U		0x15	//ctrl-U
#define CTRL_V		0x16	//ctrl-V
#define CTRL_W		0x17	//ctrl-W
#define CTRL_X		0x18	//ctrl-X
#define CTRL_Y		0x19	//ctrl-Y
#define CTRL_Z		0x1a	//ctrl-Z

/*
 * Escape sequences for Legacy DEC and ANSI terminals
 */
extern const char ESC_KEY_UP_DEC[];
extern const char ESC_KEY_UP[];
extern const char ESC_KEY_DOWN_DEC[];
extern const char ESC_KEY_DOWN[];
extern const char ESC_KEY_RIGHT_DEC[];
extern const char ESC_KEY_RIGHT[];
extern const char ESC_KEY_LEFT_DEC[];
extern const char ESC_KEY_LEFT[];
extern const char ESC_KEY_HOME_DEC[];
extern const char ESC_KEY_HOME[];
extern const char ESC_KEY_END_DEC[];
extern const char ESC_KEY_END[];
extern const char ESC_KEY_INS[];
extern const char ESC_KEY_DEL[];
extern const char ESC_KEY_F1[];

/*
 * Box drawing UTF-8 sequences usable in printf() functions
 */
#define TB_VTL 	"\xe2\x94\x86"	//Vertical line
#define TB_CDR 	"\xe2\x94\x8c"  //Corner facing down+right
#define TB_HOR	"\xe2\x94\x80"	//Horizontal line
#define TB_CDL	"\xe2\x94\x90"	//Corner facing down+left
/*

							utf_char=0x82; 	//130: __   Corner facing down+left
							utf_char=0x83; 	//131: |    Corner facing up+right corner
							utf_char=0x84;	//132:    | Corner facing up+left corner
							utf_char=0x87;	//135:  |-
							utf_char=0x88;	//136: -|
							utf_char=0x89;	//137:  |
							utf_char=0x8A;	//138: ---
							utf_char=0x8B;	//139: -|-
*/

/*
 * Sets text attributes.
 * Format specifier: "\x1b[%1dm"
 * Parameter 1: 0=normal, 7=inverse video
 */
#define TEXT_ATTR_NORMAL 0
#define TEXT_ATTR_INVERSE 7
extern const char ESC_CMD_TEXT_ATTR[];

/*
 * Handle to use for stdio functions requiring a stream.
 * The terminal can be binded to stdout so all stdio functions
 * like printf can be used to send formatted data to the
 * video screen. Binding is done in the following way:
 *
 * stdout = &TERMINAL_STREAM;
 * printf("Hello world!"); //Will be send to the terminal screen
 *
 */
extern FILE TERMINAL_STREAM;

extern void copyLine(u8 src,u8 dest);

/*
 * A call to this call back handler must be added to the
 * main program in order for the terminal to be able
 * to process keyboard polling.
 */
extern void terminal_VsyncCallback();

/*
 * Initialize the terminal to default values.
 */
extern void terminal_Init();

/*
 * Returns true if one or more characters are pending
 * in the terminal's transmit buffer.
 */
extern bool terminal_HasChar();

/*
 * Read the next character available from the terminal's input (keyboard)
 * If no characters are available the call blocks until one is available.
 */
extern u8	terminal_GetChar();

/*
 * Sends a characters to the terminal's receiver and video display.
 * stdio functions like printf and putc can also be used
 * to send streams of data to the screen terminal.
 */
extern void terminal_SendChar(u8 c);

/**
 * Append a char to the transmit buffer. This helper function
 * can be used to simulate a keystroke by the host.
 */
extern void terminal_TransmitChar(u8 c);

/**
 * Add the specified string in program memory
 * to the transmit buffer. This helper function can
 * be used to simulate a series of keystroke by the host.
 */
extern void terminal_TransmitString_P(const char* str);

/**
 * Clears the terminal's video memory and repositions the cursor at (0,0)
 */
extern void terminal_Clear();

/**
 * Typed keys are automatically echoed to the screen.
 */
extern void terminal_Echo(bool enable);

/**
 * Move the cursor to the specified position.
 */
extern void terminal_MoveCursor(u8 x,u8 y);

/**
 * Turns the cursor on or off.
 */
extern void terminal_SetCursorVisible(bool visible);

/**
 * Sets the blinking rate of the cursor.
 */
extern void terminal_SetCursorBlinkRate(u8 blinkSpeed);

/**
 * Sets the scrolling offset in terms of rows from the beginning of VRAM.
 */
extern void terminal_SetVerticalScroll(u8 row);

/**
 * Get the current scrolling offset in rows from the beginning of VRAM.
 */
extern u8	terminal_GetVerticalScroll();

/**
 * Set the top and bottom scroll margins.
 */
extern void terminal_SetScrollMargins(u8 top, u8 bottom);

/**
 * Scrolls up the entire screen up by one line.
 * Set clearNewLine = true to clear the new line that appears
 * at the bottom of the screen.
 */
extern void terminal_VerticalScrollUp(bool clearNewLine);
/**
 * Scrolls up the entire screen down by one line.
 * Set clearNewLine = true to clear the new line that appears
 * at the top of the screen.
 */
extern void terminal_VerticalScrollDown(bool clearNewLine);

/**
 * Define the foreground and background colors of the terminal's display.
 */
extern void terminal_SetColors(u8 foreground,u8 background);

/**
 * Writes the specified characters, at the specified location on the screen.
 * If attributes is non-zero, the text will be inverted.
 */
extern void terminal_PutCharAtLoc(u8 x,u8 y, u8 character,u8 attributes);

/**
 * Get the characters at the specified location on the screen.
 */
extern u8 	terminal_GetCharAtLoc(u8 x,u8 y);

/**
 * Clears the specified row on the screen, specifying the start and end columns.
 */
extern void terminal_ClearLine(u8 row, u8 startColumn, u8 endColumn);

/**
 * Control wether caraters autowraps to the line line or are clipped when reacing
 * the right edge of the screen.
 */
extern void terminal_SetAutoWrap(bool autowrap);

extern void terminal_ProcessKey(u8 c);

#endif /* TERMINAL_H_ */
