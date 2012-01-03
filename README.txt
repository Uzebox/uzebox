 
  @@@     @@@  @@@@@@@@@@@@  @@@@@@@@@@@   @@@@@@@@@@@  @@@@@@@@@@@  @@@@   @@@@  
  @@@     @@@  @@@@@@@@@@@@  @@@@@@@@@@@   @@@@@@@@@@@  @@@@@@@@@@@   @@@@ @@@@@  
  @@@     @@@       @@@@@@   @@@           @@@     @@@  @@@     @@@    @@@@@@@    
  @@@     @@@      @@@@@     @@@@@@@@@     @@@@@@@@@@   @@@     @@@     @@@@@     
  @@@     @@@    @@@@@@      @@@@@@@@@     @@@@@@@@@@@  @@@     @@@     @@@@@     
  @@@     @@@   @@@@@        @@@           @@@     @@@  @@@     @@@    @@@@@@@    
  @@@@@@@@@@@  @@@@@@@@@@@@  @@@@@@@@@@@   @@@@@@@@@@@  @@@@@@@@@@@   @@@@ @@@@@  
  @@@@@@@@@@@  @@@@@@@@@@@@  @@@@@@@@@@@   @@@@@@@@@@@  @@@@@@@@@@@  @@@@   @@@@  

          The Uzebox Project - A retro-minimalist open source console!  
	    All sources and content is licenced under the GNU GPL V3

Rev 3.3 Notes (Jan 03, 2012)
----------------------------
	-Added Chess4uzebox sources
	-Megatris imprvement: Added P1/P2 level selection in option menu, remove scoring code to recover flash memory
	-Improved SNES mouse timing to support WII nunchuck interface. 
	-Added Atomix sources
	-Fix building on Linux, remove bootloader_pragma from Makefile and use relative paths on ControllerTester's xmls.
	-Fixed video timing for video modes: 1,2,3,5,6,7,8,9 because of new kernel video features released in version r191
	-Added video mode 5 	
	-Added demo for video mode 5	
	-Added support to dynamically change the number of scanlines rendered 
	-Added support to dynamically enable/disable the sound engine
	-Direction bits corrected and pull ups activated for soft-power switches 
	-Corrected bug in EepromReadBlock that prevented the reading block 0
	-64-bit packrom and uzem are now able to handle headers correctly.
	-Bootloader: 
		* Fixed LED bug 
		* Bootloader boot mode can now be set in menu 
		* Clip files >61440 to avoid overwriting the bootloader 
		* Removed old Bootloader_Pragma project
		* Show the correct author if page number > 1
		* Misc code cleaneup
	-fixing build on GNU/Linux
	-Added read from FLASH directive for the fader[FADER_STEPS] of previous update
	-Added PROGMEM directive to unsigned char fader[FADER_STEPS] to recover 12 bytes of RAM
	-Added Controlle/Mouse Tester project
	-Video Mode 3: 
		* Added tile bank select to sprite flags 
		* Added conditional compile switch to set VRAM_TILES_H in non-scrolling mode.	
	-Misc bug fixes on kernel and emulator

		
Rev 3.2 Notes (May 14, 2010)
----------------------------
	-Added gconvert tool to convert images (raw or PNG-8) to tilesets and maps
	-Removed custom function from the API used only by Megatris
	-Added -mcall-prologues to demos makefile (saves ~300-500 bytes of flash)
	-Added support for variable tile/sprite height in mode 3
	-Corrected sync timing in mode 3,8 that caused shearing at the top of screen
	-Corrected screen centering from mode 1,3 so " 	" have the picture perfectly centered on a real TV. 
	-Fixed EEPROM bugs in uzem
	-Added emuze HEX tool to view/edit EEPROM content
	-Kernel size reduced by ~900 bytes
	-Gameloader supports up to 128 games
	-Added conditional for TRANSLUCENT_COLOR in video Mode3
	-Main program can decided when controllers are read instead of kernel with CONTROLLERS_VSYNC_READ compile switch 
	-DetectControllers() can now detect if a joypad or mouse is connected to both ports (note: function is not backward compatible) 
	-Better support for joysticks in uzem
	-Added video mode 9: 60x28 tile-only based mode (360x224)

Rev 3.1 Notes (Jan 20, 2010)
----------------------------
	-Created new Makefile to build all tools and demos from the base directory
	-Created Makefile for the tool packrom
	-Fixed buffer overflow in packrom
	-Fixed broked parameters in Uzem
	-Fixed bug in loadUzeImage() in Uzem. Loading '.uze' files is working again
	-Removed the default option '-march=native' in Uzem and Packrom because MinGW uses an unsuported version of gcc.
	-Fixed the missing '.exe' when building Uzem in Windows.
	-Added support for Uzem to define the SDL directory when building for Windows. By default the build system will look in C:\SDL\. This is good because you don't need to copy files from SDL inside MinGW making both hard to update.
	-Changes in some demos filenames and Makefiles to build in Linux machines
	-Removed binary files from Bootloader_pragma

Rev 3.0 Notes (Jan 8, 2010)
---------------------------
	-Major Refactoring: All video modes in their own files
	-New video modes: 3,4,6,7,8 (See the WIKI for details)
	-EEPROM functions
	-Assembly functions in their own sections to save flash
	-Added Vsync User callbacks
	-UART Receive buffer & functions
	-Color burst offset control
	-Packrom tool to make .uze file
	-SD card game loader/bootloader!
	-Various code cleanup & optimizations
	-Video mode 3 now has a -DSCROLLING=1 to enable XY scrolling.
	-SD/MMC low level functions ported to assembler to cut on flash utilization

Rev 2.0 Beta4 Notes (Feb 1, 2009)
-------------------------
	The demos are beign re-aranged to use a 'master' kernel instead of having copies all over the place.
	Here's the changes:
	-Projects now uses custom Makefiles. Define the path of the kernel files and your custom compilation options in there. For the moment the kernel files and objects are still referenced in the game's makefiles. Eventually it will delegate to a kernel makefile.
	-All demos have been converted to use the WIP kernel
	-Added fadein/fadeout functions
	-Added Dave's UART and MMC functions in the kernel
	-Fixed a bug in the SetTile function
	-Cleanup of include files


	Initial
	-------
	This revision is still in development. It is stable and you can use it for new games.
	However I can not guarantee backward compatibility with the final release.
	All demos unser the /demos folder are not working with this code except Whack-a-Mole
	which support both the new video mode3 and the SNES mouse.

	New Features:
	-New video mode 3: 30x28 Tiles+Sprites using 8x8 tiles & sprites. Scrolling yet to be implemented. Currently, heavy CPU usage requires disabling one or more sound channels.
	-New EEPROM libraries to save game related data (ie: scores, progress, etc).
	-New switchable color correction for the composite output. It's been reported to screw some LCD TV, but for those, use the S-VIDEO output!
	-Support for the SNES mouse
	-Fixed mode1 to remove unrolled loop


Rev 2.0 Beta3 Notes
-------------------
	-Fixed stutter problem on LCD TVs. Note that NTSC signal is now much more standard. Side effect is that composite usual artifacts are more visible.
	-Added screen sections in video mode 2. Those are vertical sections or arbitrary height with independant scroll and tile memory. This removes the need for the overlay.
	-Added one more sprite per line (insure uzeboxVideoEngineCore.o is first in link list as it requires 8bits aligment)
	-Added an optional PCM channel that can replace the current noise channel.
	-Various optimizations
	-Cleanup of header files
	-Reorganized demos in their own subfolder


Rev 2.0 Beta2 Notes
-------------------
	-Removed unrolled loop for mode 1(tiles-only) and regained ~2K of flash.
	-Added SetMixerVoice() function to get direct control of music mixer
	-Overlay now supports priority with sprites (overlay can be on top or under sprites)
	-Fixed small sprites bugs
	-Optimized sprite buffer, recovered 64 bytes of RAM :P


Rev 2.0 Beta Notes
------------------
	Update highlights:
	-A new video mode that features a sprites engine and full screen scrolling
	-Supports SNES Joystick as baseline. NES is supported with a compilation option. Data read from joystick is now an int.
	-Added custom compilation options to reduce kernel code & RAM usage (see /kernel/defines.h)
	-Regrouped all API functions/defines in /kernel/uzebox.h
	-Improved the NTSC sync timings. It should be even better when I get my digital scope. ;)
	-Console can be reset with a joypad combination (Select+Start+A+B)
	-Improved code cleansing & documentation
	-And not the least, the GFX/Music conversion tools (under /tools) are now in the package! Don't know what happened for V1.0.

	This release contains 5 demos to help you get started. All HEX files of these demos are directly under the /demos

	-Tutorial: A basic Hello World!
	-Maze: A maze game by Clay Cowgill.
	-Megatris: A Tetris clone.
	-Sound Demo: Plays many NES classics.
	-Sprite Demo: Demonstrates the new sprites and scrolling engine.



Rev 1.0 - 24-Aug-2008
---------------------
	Initial release
