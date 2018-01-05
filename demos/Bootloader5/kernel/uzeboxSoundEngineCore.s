/*
 *  Uzebox Kernel
 *  Copyright (C) 2008-2009 Alec Bourque
 *  Optimized and trimmed to the bootloader by Sandor Zsuga (Jubatian), 2017
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

#include <avr/io.h>
#include <defines.h>


;
; Global assembly delay macro for 0 to 1535 cycles
; Parameters: reg = Registerto use in inner loop (will be destroyed)
;             clocks = CPU clocks to wait
;
.macro WAIT reg, clocks
.if     (\clocks) >= 768
	ldi   \reg,    0
	dec   \reg
	brne  .-4
.endif
.if     ((\clocks) % 768) >= 9
	ldi   \reg,    ((\clocks) % 768) / 3
	dec   \reg
	brne  .-4
.if     ((\clocks) % 3) == 2
	rjmp  .
.elseif ((\clocks) % 3) == 1
	nop
.endif
.elseif ((\clocks) % 768) == 8
	lpm   \reg,    Z
	lpm   \reg,    Z
	rjmp  .
.elseif ((\clocks) % 768) == 7
	lpm   \reg,    Z
	rjmp  .
	rjmp  .
.elseif ((\clocks) % 768) == 6
	lpm   \reg,    Z
	lpm   \reg,    Z
.elseif ((\clocks) % 768) == 5
	lpm   \reg,    Z
	rjmp  .
.elseif ((\clocks) % 768) == 4
	rjmp  .
	rjmp  .
.elseif ((\clocks) % 768) == 3
	lpm   \reg,    Z
.elseif ((\clocks) % 768) == 2
	rjmp  .
.elseif ((\clocks) % 768) == 1
	nop
.endif
.endm



#include "soundMixerSaw.s"
