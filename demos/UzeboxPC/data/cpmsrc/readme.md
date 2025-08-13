# Source for CP/M binaries

* cpm22.asm : Original CP/M 2.2 sources by Digital Research. Use TASM to compile.
* hello.asm: Example assembler program that can be compiled under CP/M using ASM.COM.
* telnet.asm: Launcher for the Uzebox native telnet client.
* uzeconf.asm: Launcher for the Uzenet module configuration native application.

Notes: 
* To compile these sources, the cross-assembler [The Macro Assembler AS / TASM](http://john.ccac.rwth-aachen.de:8000/as/) is required. A migration to ZASM (https://k1.spdns.de/Develop/Projects/zasm/Distributions/) is considered because of its cross-platformn nature. 
* CP/M is now free to use, enhance and disctribute: http://www.cpm.z80.de/license.html