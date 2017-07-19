
Video Mode 748 - Developer's manual
==============================================================================




Introduction
------------------------------------------------------------------------------


Video Mode 748 is a 7 cycle / pixel palettized mode at up to 4bpp (16 colors)
depth within a scanline. It is capable to display 192 pixels (24 tiles of 8
pixels width) horizontally or 384 pixels for 1bpp images from the SPI RAM.

The 7 cycles / pixel figure produces the same essential pixel clock like the
NTSC Commodore 64's Multicolor mode, having a 1,5:1 pixel aspect ratio.

This mode is a variant of Mode 74 tailored for extensive SPI RAM usage. It
lacks some features and submodes of Mode 74, replaced with SPI RAM oriented
features and submodes. The mode's functions are prefixed "M74" to maintain
compatibility between the two modes.

Overall key features of the frame renderer are as follows:

- Row oriented rendering. Using a row selector, any row may appear on any
  physical scanline, allowing for arbitrary split-screen and horizontal scroll
  effects.

- Tile row (8 pixels tall) oriented logical layout. There are up to 32 tile
  rows accessible at any given time, each configurable independently of the
  others.

- Horizontal scrolling as X shift to the left by 0 to 7 pixels is available,
  similar to the respective feature of Commodore 64's VIC. It normally
  requires copying the VRAM for a complete scrolling algorithm.

- The palette is represented as a simple 16 byte array, allowing for palette
  effects (such as fading). Color 0 (background) of the palette may be changed
  for every row. Using a special single colored row mode, the entire palette
  can be replaced during display.

- Up to 224 scanlines height, configurable by the Uzebox kernel's
  SetRenderingParameters() function.

- 4 bits per pixel row mode allows for up to 3 x 64 ROM tiles and up to 1 x 64
  RAM tiles. Each 64 tile bank may be located independently of others within
  ROM or RAM respectively. This row mode is available for the sprite engine.

- 234 cycles are available for the inline mixer. This allows for having five
  channels audio, of four channels and UART.

Overall key features of the sprite engine are as follows:

- Works with 8 x 8 pixel ROM, RAM or SPI RAM sourced sprite tiles or single
  pixels. Index 0 of the sprite tiles is transparent.

- Blitter concept with VRAM restoring: for rendering sprites, blits are to be
  called placing sprites on the canvas like if it was a regular framebuffer.
  RAM tile allocation and related tasks are carried out by the sprite engine
  internally. The VRAM can be restored to its original contents after a
  frame's display to start a new render.

- X and Y mirroring.

- RAM tile usage can be controlled allowing the use of 1 to 64 RAM tiles.

- Can perform blits over the 4 bits per pixel row mode. It can cope with
  different tilesets on the same screen (allowing the use of more than 192
  ROM tiles), and can use RAM tiles as well as targets (so it will blit
  normally over RAM tiles not allocated for sprites).

- Supports recoloring the sprite tiles by fast or slow recolor tables (fast
  recolor tables requiring 256 bytes ROM each, slow tables requiring 16 bytes
  each).

- Supports masking: the tile layer may partially cover sprite content.

- Parallax scrolling: Each tile row may have an individual X shift of 0 to 7
  pixel. Combined with an appropriate scrolling algorithm, this enables
  horizontally scrolling tile rows independently of each other with sprites
  over them.




Data formats
------------------------------------------------------------------------------


ROM 4bpp tiles and Sprite tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A 4 bits per pixel tile takes 32 bytes. It is laid out in the following manner
where numbers indicate the relative byte offset, 'H' and 'L' the high and low
nybbles respectively: ::

    00H 00L 01H 01L 02H 02L 03H 03L
    04H 04L 05H 05L 06H 06L 07H 07L
    08H 08L 09H 09L 10H 10L 11H 11L
    12H 12L 13H 13L 14H 14L 15H 15L
    16H 16L 17H 17L 18H 18L 19H 19L
    20H 20L 21H 21L 22H 22L 23H 23L
    24H 24L 25H 25L 26H 26L 27H 27L
    28H 28L 29H 29L 30H 30L 31H 31L

Sprites always use color index 0 as transparent. As an example a single
colored sprite showing an oval could have the following data: ::

    0x00U, 0x11U, 0x11U, 0x00U,
    0x01U, 0x10U, 0x01U, 0x10U,
    0x11U, 0x00U, 0x00U, 0x11U,
    0x10U, 0x00U, 0x00U, 0x01U,
    0x10U, 0x00U, 0x00U, 0x01U,
    0x11U, 0x00U, 0x00U, 0x11U,
    0x01U, 0x10U, 0x01U, 0x10U,
    0x00U, 0x11U, 0x11U, 0x00U

Tilesets (either RAM or ROM) can start at any 256 byte boundary.


SPI RAM 4bpp bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The 192 pixels wide 4bpp bitmap is split to a 24 pixels (12 bytes) wide left
column stored in RAM and a 168 pixels wide right column stored in SPI RAM. The
pixel order within bytes is high nybble first (corresponding to the leftmost
pixel). Note that the left column takes 96 bytes per 8 pixel row, which is the
same as the attribute row for an SPI RAM 1bpp bitmap with attribute mode
display.


SPI RAM 3bpp bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The 192 pixels wide 4bpp bitmap is composed of 24 x 3 byte blocks, each 3 byte
block encoding 8 pixels. The pixels appear in the 3 byte blocks as follows: ::

    60006111 62227333 74447555

Pixel 0 is the leftmost pixel, pixel 7 is the rightmost.


SPI RAM 1bpp bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The highest bit of a byte corresponds to the leftmost pixel. This mode can
optionally also have attributes in RAM, these take 96 bytes for each tile row,
allowing the specification of an arbitrary color pair for each 8 x 8 block.
The first attribute byte corresponds the background (0) color, the second the
foreground (1) color.




Tile row modes overview
------------------------------------------------------------------------------


The mode of a row is selected by the m74_tdesc pointer, pointing into an array
of 32 bytes, each byte specifying a pointer into a tile row descriptor table.
Bit 7 of this value governs whether the ROM (0) or the RAM (1) tile descriptor
table should be used, located by the M74_ROMTD_OFF and M74_RAMTD_OFF
definitions respectively. One tile descriptor takes normally 5 bytes in these
tables.

Tile row 0 is special for the following uses:

- The sprite engine uses this row to locate the RAM tiles whose base offset
  must be identical across the whole display region used for sprite rendering.

Byte 0 of the tile descriptor specifies the row mode and flags as follows:

- bits 0 - 2: Row mode.
- bits 3 - 7: Flags (usage depends on row mode).

In SPI RAM sourced modes reading starts at m74_saddr in the bank specified
in m74_config. Reading is continuous during the display frame, as many bytes
are fetched as required for each row.


Mode 0: 192 4bpp ROM tiles + 64 4bpp RAM tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x3F: 4bpp ROM tiles (base: byte 1)
- 0x40 - 0x7F: 4bpp ROM tiles (base: byte 2)
- 0x80 - 0xBF: 4bpp ROM tiles (base: byte 3)
- 0xC0 - 0xFF: 4bpp RAM tiles (base: byte 4)

Tile descriptor bytes are used as indicated above: they specify the high byte
of the base offset for the tiles with the given offset. Note that one step in
the base means 8 tiles: it is possible to overlap distinct tile maps
exploiting this if necessary.


Mode 2: Separator line with palette reload
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This mode uses no VRAM. Only bytes 0 and 1 are used from the tile descriptor.

This is an optional mode, needs to be enabled explicitly (M74_M2_ENABLE = 1)
if needed.

This mode is capable to display a simple separator line of a single color
(high nybble of byte 1) with optional palette replacement.

The following bits of tile descriptor byte 0 are used as flags:

- bit 5: If set, palette is fetched from RAM, otherwise ROM.
- bit 6: If set, no palette reloading takes place.

Palette reload may take place on row 0 or row 7 of this mode if it was
enabled. They behave differently in the following manner:

- Row 0 reload uses the tile index source as palette base offset. It colors
  the separator line using the specified color of this new palette.

- Row 7 reload uses the tile index source plus 16 as palette base offset. It
  colors the separator line using the specified color from the old palette.


Mode 4: SPI RAM 4bpp bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This mode requires a VRAM width of 96 bytes for its rows. The VRAM is used to
store the 24 pixels wide left column.

In this mode, a line takes 84 SPI RAM bytes.

Horizontal scrolling in this mode is not possible.

This mode can be used to display 4bpp pictures at up to 192 x 224 pixels
resolution. It can also be mixed with any other mode.


Mode 5: SPI RAM 3bpp bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This mode uses no VRAM.

In this mode, a line takes 72 SPI RAM bytes.

3 SPI RAM bytes encode 8 pixels as follows: ::

    60006111 62227333 74447555

Pixel 0 is the leftmost pixel, pixel 7 is the rightmost. They use the low 8
indices of the palette (so color 0 replacement may be used).


Mode 6: SPI RAM 1bpp bitmap with attributes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This mode requires a VRAM width of 96 bytes for its rows. The VRAM is used to
store attributes: background (0) and foreground (1) colors for each 8 x 8
pixel block.

In this mode, a line takes 48 SPI RAM bytes.

Horizontal scrolling in this mode is not possible.

This mode can be used to display 1bpp attribute mode pictures at up to
384 x 224 pixels resolution. It can also be mixed with any other mode (such as
even Mode 4).


Mode 7: SPI RAM 1bpp bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This mode uses no VRAM. Byte 1 of the tile descriptor specifies the colors
(high nybble: foreground, low nybble: background) for the entire row from the
palette. Using color index 0 allows for using the related feature
(M74_COL0_OFF nonzero) to change this color every scanline.

In this mode, a line takes 48 SPI RAM bytes.

Horizontal scrolling in this mode is not possible.

This mode can be used to display 1bpp pictures at up to 384 x 224 pixels
resolution. It can also be mixed with any other mode.




Scanline logic
------------------------------------------------------------------------------


The rendering of the frame is broken up in scanlines, whose render may be
controlled individually.

Normally and at most the frame has 224 displayed lines, this figure can be
configured by the kernel's SetRenderingParameters() function. Giving less
lines for the display increases lines within VBlank which can be used to
perform more demanding tasks.

Each displayed line (physical scanline) can contain any logical scanline of
the 256 from the 32 configurable tile rows. This selection may be directed by
a split list.

This list uses byte triplets defining locations where the logical scanline
counter has to be re-loaded, and the X shift register has to be set.
Afterwards the logical scanline counter increments by one on every line. The
triplets are as follows:

- byte 0: Physical scanline to act on (0 - 223)
- byte 1: Logical scanline to set
- byte 2: X shift value (only the low 3 bits are used)

The first triplet is partial, only having bytes 1 and 2 (that is, line 0 is
implicit for that). The list can be terminated by a byte 0 value which can
not be reached any more, such as zero or 255.




The palette
------------------------------------------------------------------------------


The mode requires a 256 byte palette buffer, which it normally located at
0x0F00, below the Stack. Normally this buffer doesn't have to be accessed
since the mode automatically manages it.

A global (initial) 16 color (16 byte) palette either in RAM or ROM may be set
up to be loaded before starting the display of the frame. By manipulating this
palette in VBlank, palette effects (color cycling, fading) can be achieved.

The palette can be replaced within the frame by using the separator tile row
mode (Row mode 2).

Note that palettes may be located anywhere, they need not be aligned on any
boundary.




Extra features
------------------------------------------------------------------------------


Color 0 reload
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Color 0 of the palette may be reloaded from a RAM table for every scanline.

This feature may be used to implement rasterbar effects of a more diverse
backdrop for a side-scrolling style game without the need for extra ROM space.




Kernel integration
------------------------------------------------------------------------------


To support the Uzebox kernel's Print function, SetTile, SetFont and ClearVram
are implemented. Note however that they don't operate directly on the display
as this is not possible by the configurability of Mode 74.

Some functions within the kernel rely on compile time defined width and height
parameters. These should be set up by planning how the kernel's output will be
displayed with Mode 74 (for example if 6 pixels wide tiles are used at 24
tiles width, 32 could be set up for VRAM_TILES_H and SCREEN_TILES_H).

Note that the sprite engine also operates on this VRAM.


Uzebox logo
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Uzebox logo display code is designed to interfere the least with the
flexibility of the video mode. For normal use cases it should compile fine
just enabling it (setting INTRO_LOGO to 1 or 2).

It uses Row mode 0, RAM tiles only, needing at least 19 RAM tiles.

For the palette it requires a RAM palette, so the logo doesn't work if the
palette offset is disabled (M74_PAL_PTRE set zero) and a ROM palette is used.
The initial palette offset (M74_PAL_OFF) must point to a RAM location (which
is so by default).
