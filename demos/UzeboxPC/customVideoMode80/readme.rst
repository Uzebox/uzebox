Video Mode 80
==============================================================================




Overview
------------------------------------------------------------------------------


Video Mode 80 is a high-resolution code-tile mode, designed along with its
generators to support a versatile set of code-tile displays.

The primary goal of the mode is providing a decent solution for an at least
80 columns wide textmode display, which it achiever in better quality than
Mode 9 does, allowing to fit 80 columns on most displays (contrary to Mode 9
where the most often the edge columns are missing).

For 80 columns wide text, it uses 17 cycle per tile, which compares to other
modes and usages of NTSC the following manner:

- 18 cycles wide: Mode 9, 6 pixels / tile; 1440 clocks for 80 columns.
- 17 cycles wide: This mode; 1360 clocks for 80 columns.
- 16 cycles wide: CGA High-Res; 1280 clocks for 80 columns.

Technically it would be possible to use 16 cycles, however it was not chosen
for the character set included with this mode to avoid character features
synchronizing to NTSC colorburst (16 Uzebox cycles: 2 NTSC Color clocks).




Using the mode with its included character set
------------------------------------------------------------------------------


Starting using the mode is as simple as setting it up in the project's
Makefile like any other video mode:

- DVIDEO_MODE=80

This results in compiling with a 80x28 screen area with a correspondingly
sized Video RAM.

The VRAM and the kernel's text output routines can be used "out of the box",
thus for example even the tutorial can be easily modified to output onto this
video mode.

Additional definitions which may be used to tune the mode:

- SCREEN_TILES_H: Number of columns the mode should have. Defaults to 80.

- SCREEN_TILES_V: Number of tile rows. Defaults to 28, which is the maximum
  when using a tileset with 8 pixels tall tiles.

- VRAM_TILES_V: Number of VRAM tile rows available. By default it is set to
  the value of SCREEN_TILES_V, however you can use more rows if you need it
  for scrolling or other effects.

Advanced features are available through interacting with various variables the
mode uses.

- m80_rompal: Pointer to ROM palette of 16 colors. You can change the palette
  from the default (which is black background, white foreground, and six
  colors: blue, green, turquoise, red, purple and yellow for subsequent
  indices occurring in the tileset) by providing this. Setting it to NULL
  reverts to the default palette.

- m80_rampal: Pointer to RAM palette of 16 colors. Same role like m80_rompal,
  however has higher priority (so if both are present, the RAM palette is
  used).

- m80_bgclist: Background Color List. This is a list of colors for every
  scanline, if enabled, can be used to apply rasterbar colors to the
  background. The Background is color index 0.

- m80_fgclist: Foreground Color List. This is a list of colors for every
  scanline, if enabled, can be used to apply rasterbar colors to the
  foreground. The Foreground is color index 1.

- m80_dlist: Display List. This item has its own chapter below.

Priorities for choosing color are as follows, the uppermost in the list having
the lowest priority:

- Default colors.
- ROM palette by m80_rompal.
- RAM palette by m80_rampal.
- Colors specified in Display List.
- Background / Foreground color lists.




Using the Display List
------------------------------------------------------------------------------


The Display List is a versatile feature allowing splitting the screen and
scrolling (vertically) sections of it.

It is a structure (type: m80_dlist_tdef) with the following members:

- vramrow: The VRAM row of the first scanline of the section.
- tilerow: The tile row of the first scanline of the section.
- bgc: Background color to use.
- fgc: Foreground color to use.
- next: Next scanline to match. Set to zero in the last entry.

The m80_dlist variable accepts a pointer to the display list (the first entry
of the display list).

Setting the VRAM row or the Tile row to an invalid value for the VRAM or the
tile height of the used tileset will cause them being interpreted as zero. For
smooth vertical scrolling, the tilerow member has to be incremented /
decremented first, then when it reaches the tile height or zero, the tile
row have to follow accordingly.

The VRAM wraps on its end (so if within a section the bottom of the VRAM is
reached, the next scanline will show the topmost row).




Creating tilesets
------------------------------------------------------------------------------


Tilesets can be created using the C applications in the generators directory.

The overall solution is very crude, you need to export a header from Gimp,
which then can be compiled with the appropriate generator, which if
subsequently ran, would produce the assembly file containing the tileset's
code onto its standard output.

The input images for both generators need to have a width in cycle units, and
height in pixels (so tiles would be very wide). Use Gimp's Print size feature
to get the images scaled properly during working with them (also turning off
Dot for Dot view to make it effective).

There are two generators:

- tilegen.c can be used to generate an arbitrary tileset, it was used to
  produce m80_cp437.s for example. Designing a tileset for this is tricky, you
  can use two or even one cycle wide pixels, however it is limited where these
  can occur. Notably the last pixel of a tile must always be at least 3 pixels
  wide (due to the need to place an ijmp instruction there).

- tilegen_3cy.c is a generator optimized for 3 (or more) cycles wide pixels,
  can be used to use this video mode as a Mode 9 substitute. There are no
  restrictions on pixel widths otherwise, from 15 cycles (5 pixels) / tile,
  any arrangement of 3 cycles wide pixels can be used.

The generators are capable to optimize the tileset with great efficiency,
however keep in mind that they would stick to oridinary linear execution of
the tile. There are lower resolution code tile modes which can provide greater
size efficiency due to employing different techniques, which techniques are
not possible at the resolution Mode 80 is aiming for.

Before getting started, take a look at the comments at the top of each of the
generators as they describe adequately how they operate and how they are meant
to be used.
