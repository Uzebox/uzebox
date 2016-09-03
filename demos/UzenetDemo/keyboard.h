/*
 * keyboard.h
 *
 *  Created on: Oct 13, 2015
 *      Author: admin
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <avr/pgmspace.h>

#define KB_SEND_KEY 0x00
#define KB_SEND_END 0x01
#define KB_SEND_DEVICE_ID 0x02
#define KB_SEND_FIRMWARE_REV 0x03
#define KB_RESET 0x7f

extern u8 GetKey(u8 command);
extern u16 decode(u8 sc);


#endif /* KEYBOARD_H_ */
