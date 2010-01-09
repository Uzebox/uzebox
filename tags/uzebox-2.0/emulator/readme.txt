Run the emulator with no parameters to see all command line options.

In particular, -hwsurface and/or -fullscreen have been known to improve
performance on some systems.

Hit F1 while emulator is running to see the key bindings.


David Etherton
david.c.etherton@gmail.com (not checked often)
or just find me under DavidEtherton on the uzebox forums.



Changes between v1.08 and v1.07
- Added EEPROM read/write emulation.
- Default filename is eeprom.hex, use -eeprom arg to change.
- File is saved on emulator exit if anything changed.
- Debug spew logs reads and writes (because you only get so many read/write cycles before it fails, so you want to know about excessive use)

Changes between v1.07 and v1.06
- Got input emulation working with Alec's new code.
- If we see an attempt to read more than 12 bits, switch to new emulation method (PINA is validated by CLOCK, not LATCH, and has the "old" value).
- Turn off the mouse cursor and warp it in fullscreen mode so emulation works better.
- New cmd line options -mouse and -2p to set initial emulation mode (important for W-a-m which only checks at startup)

Changes between v1.06 and v1.04
- Added two player button mappings
- Added SNES Mouse support
- Emulator input is BROKEN in Alec's new Whack-a-mole demo, we're trying to sort out the correct way to handle this.  All other demos continue to work fine though, including 2p games now.

(v1.05 was a private version with SNES mouse support I sent to Alec)

Changes between v1.04 and v1.03
- Changed the order PC lo and hi are pushed/popped during interrupt / call / ret to be consistent with actual hardware.

Changes between v1.03 and v1.02
- BREAK and WDR were not being correctly decoded.  Confirmed that WDR spits out correct timing information.

Changes between v1.02 and v1.01
- Only call flip every other frame, much faster on platforms with slow SDL implementations
- Display emulated mhz in tty periodically in fullscreen mode
- My 2.0Ghz Core Duo MBP can now run at full speed in -fullscreen mode, just barely.
- In comparison, my 2.66 Quad-Core XP box can run in a window with -nosound at 64 Mhz emulated.

Changes between v1.01 and v1.00
- optimized video emulation a bit
- cpu speed (not instruction rate) displayed in title bar
- took out framelock code, audio sync does a better job now without dropouts
- added -hwsurface and -nodoublebuf SDL flags for people to test with
- added -interlaced mode for true 448 vertical res

