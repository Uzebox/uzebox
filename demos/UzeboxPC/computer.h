/*
 * computer.h
 *
 *  Created on: 3 févr. 2020
 *      Author: admin
 */

#ifndef COMPUTER_H_
#define COMPUTER_H_

void computer_Boot(void);			//Initialize the hardware and boot CP/M on the machine.
void computer_VsyncCallback(void);	//Give a time base reference to the computer
void computer_PortOut(u8 port,u8 value);
u8   computer_PortIn(u8 port);

#endif /* COMPUTER_H_ */
