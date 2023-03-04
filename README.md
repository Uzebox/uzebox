<img src="http://uzebox.org/belogic.com/uzebox/images/new_banner3.jpg"
 alt="Uzebox logo" />
<img src="https://uzebox.org/belogic.com/uzebox/images/thumbs/case_thumb.gif" height="74px"  />
<img src="https://uzebox.org/belogic.com/uzebox/images/games/donkeykong.png"  />
<img src="https://uzebox.org/belogic.com/uzebox/images/games/alterego.png"  />
<img src="https://uzebox.org/belogic.com/uzebox/images/games/ghostyghost.png"  />
<img src="https://uzebox.org/belogic.com/uzebox/images/games/loderunner.png"  />
<img src="https://uzebox.org/belogic.com/uzebox/images/games/mellisretroland.png"  />


The Uzebox is an open source, retro-minimalist games console. It is based on an 8-bit Atmel AVR microcontroller and distinguished by its interrupt-driven game engine and lack of a frame buffer. Functions such as video sync generation, tile rendering and music mixing are all handled in software which allows for the development of custom video modes and future expansion of its capabilities. Games can be easily written in C using the Uzebox API.

The design goal was to be as simple as possible yet provide good enough sound and graphics to implement interesting games. Emphasis was put on making it easy and fun to assemble and program for hobbyists. The final design contains only two chips: an ATmega644 and an AD725 RGB-to-NTSC converter.

Many commercial versions were available such as the Uzebox AVCore by Embedded Engineering llc and the Fuzebox by Adafruit Industries. There's also the Uzebox deluxe kit and the EUzebox, a version with a SCART interface.

Features:
* Interrupt driven: No cycle counting required, sound mixing and video generation are handled by the Uzebox kernel
* 5 channels sound engine: The sound subsystem is composed of 3 wavetable channels, 1 noise and 1 PCM channel. Sound is 8-bit mono, mixed at ~15Khz
* 256 simultaneous colors arranged in a 3:3:2 color space (Red:3 bits, Green:3 bits, Blue: 2 bits)
* Resolution: 10+ video modes offering up to 360x224 pixels (tiles-only, tiles & sprites, and bitmap video modes)
* Full screen scrolling in certain video modes
* Sprites: 32+ simultaneous sprites
* Inputs supported: Two SNES compatible joypad inputs (including SNES mouse)
* MIDI IN: Use your Uzebox as a MIDI controlled synth
* SD/MicroSD and FAT16/32 support
* 'Uzenet' extension for ESP8266 wifi and 128KB SPI RAM
* PS/2 keyboard interface
* Bootloader5: Bootloader which allows to flash games from standard FAT16 or FAT32 formatted SD cards
* Cross-platform emulator with GDB support to ease development
* Multiple tools to convert MIDI, sound file and graphics to include files

The sources come complete with fully functional games, demos, content generation tools and even a cross-platform emulator!

**To find out more, please check out the project's sites:**
* [Getting started](https://uzebox.org/wiki/Getting_Started_on_the_Uzebox): How to install the toolchains, IDEs and build the codebase. Then move on to tutorials and the rest of the documentation.  
* [Main website](https://uzebox.org): The main hub with news, links, downloads and more.
* [Wiki](https://uzebox.org/wiki): All the project's documentation.
* [Forums](https://uzebox.org/forums): Share your new game and discuss everything Uzebox.
