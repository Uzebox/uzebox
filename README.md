<img src="http://belogic.com/uzebox/images/new_banner3.jpg"
 alt="Uzebox logo" />
<img src="http://belogic.com/uzebox/images/thumbs/case_thumb.gif" height="74px"  />
<img src="http://belogic.com/uzebox/images/games/donkeykong.png"  />
<img src="http://belogic.com/uzebox/images/games/alterego.png"  />
<img src="http://belogic.com/uzebox/images/games/ghostyghost.png"  />
<img src="http://belogic.com/uzebox/images/games/loderunner.png"  />
<img src="http://belogic.com/uzebox/images/games/mellisretroland.png"  />


The Uzebox is an open source, retro-minimalist game console design. It is based on an AVR 8-bit general purpose microcontroller made by Atmel. The particularity of the system is that it's based on an interrupt driven engine and has no frame buffer. Functions such as video sync generation, tile rendering and music mixing is done realtime in software by a background task so games can easily be developed in C. The design goal was to be as simple as possible yet have good enough sound and graphics while leaving enough resources to implement interesting games. Emphasis was put on making it easy and fun to assemble and program for any hobbyists. The final design contains only two chips: an ATmega644 and an AD725 RGB-to-NTSC converter.

Many commercial version are or where available: The Uzebox AVCore by Embedded Engineering llc and the Fuzebox by Adafruit Industries. There's also the Uzebox deluxe kit and the EUzebox, a version with a SCART interface. Get one of those if you know nothing about electronics!

Features:
* Interrupt driven: No cycle counting required, sound mixing and video generation are all made in the background
* 5 channels sound engine: The sound subsystem is composed of 3 wavetable channels, 1 noise and 1 PCM channel. Sound is 8-bit mono, mixed at ~15Khz
* 256 simultaneous colors arranged in a 3:3:2 color space (Red:3 bits, Green:3 bits, Blue: 2 bits)
* Resolution: 9 video modes offering up to 360x224 pixels (tiles-only, tiles & sprites, and bitmap video modes)
* Full screen scrolling in certain video modes
* Sprites: Up to 32 simultaneous sprites
* Inputs supported: Two SNES compatible joypad inputs (including SNES mouse)
* MIDI In: With a music sequencer, allows the creation of music directly on the console
* SD/MicroSD and FAT16 API
* 'Uzenet' extension for ESP8266 wifi and 1Mb SPI RAM
* GameLoader: 4K Bootloader which allows to flash games from a standard FAT16 formatted SD card
* Cross-platform emulator with GDB support to ease development
* Multiple tools to convert MIDI, sound file and graphics to include files

The sources comes complete with fully functional games, demos, content generation tools and even a cross-platform emulator!

**To find out more, please check out the project's sites:**
* [Main website] [website]
* [Wiki] [wiki]
* [Forums] [forums]

[website]: http://belogic.com/uzebox
[wiki]: http://uzebox.org/wiki
[forums]: http://uzebox.org/forums



