/*
 * keyboard.h
 *
 * Library to interact with the Uzebox keyboard interface via the Uzebus protocol.
 *
 *  Created on: Oct 13, 2015
 *      Author: admin
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <avr/pgmspace.h>

//Modifiers keys
#define KB_FLAG_SHIFT	0x01
#define KB_FLAG_CTRL 	0x02
#define KB_FLAG_ALT 	0x04
#define KB_FLAG_WIN		0x08
#define KB_FLAG_APPS	0x10

//Extended keys
#define KB_NULL		0x00	//NULL
#define KB_PAUSE	0x01	//ctrl-A
#define KB_BREAK 	0x02	//ctrl-B
#define KB_SCRLK	0x03	//ctrl-C
#define KB_PRTSC	0x04	//ctrl-D
#define KB_INS		0x05	//ctrl-E
#define KB_DEL 		0x06	//ctrl-F
#define KB_HOME		0x07	//ctrl-G
#define KB_BCKSP	0x08	//ctrl-H
#define KB_TAB		0x09	//ctrl-I
#define KB_END		0x0a	//ctrl-J
#define KB_PGUP 	0x0b	//ctrl-K
#define KB_PGDN		0x0c	//ctrl-L
#define KB_ENTER	0x0d	//ctrl-M
#define KB_F1		0x0e	//ctrl-N
#define KB_F2		0x0f	//ctrl-O
#define KB_F3		0x10	//ctrl-P
#define KB_F4		0x11	//ctrl-Q
#define KB_F5		0x12	//ctrl-R
#define KB_F6		0x13	//ctrl-S
#define KB_F7		0x14	//ctrl-T
#define KB_F8		0x15	//ctrl-U
#define KB_F9		0x16	//ctrl-V
#define KB_F10		0x17	//ctrl-W
#define KB_F11		0x18	//ctrl-X
#define KB_F12		0x19	//ctrl-Y
#define KB_UP		0x1a	//ctrl-Z
#define KB_ESC		0x1b
#define KB_DOWN		0x1c
#define KB_LEFT		0x1d
#define KB_RIGHT	0x1e
#define KB_SPACE	0x20

/*
** Poll the keyboard for available keystrokes. The function will
** not poll the keyboard if the last key was not read by the host.
** Note that polling faster than once a frame (60hz) could result
** in loosing key strokes.
**
** Normally called in a user vsync callback function.
*/
extern void	KeyboardPoll();

/*
** Returns true if a normal key was fetched by the last keyboard poll.
** Normal keys are all ones except the modifier keys SHIFT, ALT, CTRL and WINKEY.
*/
extern bool	KeyboardHasKey();

/*
** Return the last key fetched from the last keyboard polling. If no new key is available, it returns 0.
**
** If the parameter == true, the function will decode considereing the state of the shift key.
 * Ex.: If Shift is pressed and key is 'a' then it will return 'A'. Otherwise 'a' is always
 * returned regardless of the state if the Shift key.
*/
extern u8 KeyboardGetKey(bool shifted);

/**
 * Return the state of key modifiers SHIFT, ALT, CTRL and WINKEY.
 *
 * Use the KB_FLAG_* constants to check for the desired modifiers.
 * Ex: if(KeyboardGetModifiers() & KB_FLAG_ALT) DoSomething();
 */
extern u8 KeyboardGetModifiers();

/*
** Programatically simulates a series of keystrokes from the keyboard.
*/
extern void KeyboardDebugSend(const char *string);


#endif /* KEYBOARD_H_ */
