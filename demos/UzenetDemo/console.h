/*
 * console.h
 *
 *  Created on: Oct 13, 2015
 *      Author: admin
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#define CONS_OUTPUT_NON_ALPHA 1

extern void ConsoleHandler();
extern void console_init(u16 flags);
extern void console_clear();
extern void console_setCursor(u8 x,u8 y);

#endif /* CONSOLE_H_ */
