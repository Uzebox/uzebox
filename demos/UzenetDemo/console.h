/*
 * console.h
 *
 *  Created on: Oct 13, 2015
 *      Author: admin
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#define CONS_OUTPUT_NON_ALPHA 1
#define CONS_DEFAULT_COLOR 38
extern void ConsoleHandler();
extern void console_Init(u16 flags);
extern void console_Clear();
extern void console_Echo(bool enable);
extern void console_SetCursor(u8 x,u8 y);
extern void console_Process();

#endif /* CONSOLE_H_ */
