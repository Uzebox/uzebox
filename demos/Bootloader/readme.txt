/*
 *  Uzebox(tm) Gameloader
 *  Copyright (C) 2008-2015 Alec Bourque
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

/*
About this program:
-------------------
This program is an AVR graphical bootloader that allows to read and flash games from a SD/MMC card. 

-Memory card must be a regular SD (not SDHC).
-The bootloader requires 4K of flash.
-The Atmega644 needs to have some fuses set in order to support teh bootloader. 

For setup details see: http://uzebox.org/forums/viewtopic.php?p=3847#p3847