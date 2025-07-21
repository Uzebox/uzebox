Build folder for the project.

# CP/M binaries
On Windows machines, batch files are provided to rebuild the following cp/m programs using [The Macro Assembler AS](http://john.ccac.rwth-aachen.de:8000/as/as_EN.html):
* build-cpm: Recompiles the cp/m source including the custom BIOS. Note that this creates a C include file that can be linked to the AVR binary. Mainly used for development.
* build-telnet: Build a cp/m launcher program that starts the native telnet client. The generated .com file is then added to a cp/m image file using cpmfs.
* build-uzeconf: Build a cp/m launcher program that starts the native Uzenet configuration program. The generated .com file is then added to a cp/m image file using cpmfs.

# Uzenet configuration tool
To build a standalone Uzenet setup rom, uncomment the following in the build file:
```
KERNEL_OPTIONS += -DUZENET_SETUP=1
```