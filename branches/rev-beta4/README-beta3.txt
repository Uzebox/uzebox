 
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


Two most important files to view if you don't want to read all this!
1) /kernel/defines.h : Custom compilation options
2) /kernel/uzebox.h  : All API functions and defines

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
