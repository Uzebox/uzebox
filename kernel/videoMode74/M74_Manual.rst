
Video Mode 74 - Developer's manual
==============================================================================




Introduction
------------------------------------------------------------------------------


Video Mode 74 is a 7 cycle / pixel palettized mode at up to 4bpp (16 colors)
depth within a scanline. It is capable to display up to 192 pixels (24 tiles
of 8 pixels width) horizontally.

The 7 cycles / pixel figure produces the same essential pixel clock like the
NTSC Commodore 64's Multicolor mode, having a 1,5:1 pixel aspect ratio.

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

- Widths of 24, 22, 20 or 18 tiles may be selected for each tile row
  individually (192, 176, 160 or 144 pixels respectively).

- Up to 224 scanlines height, configurable by the Uzebox kernel's
  SetRenderingParameters() function.

- 4 bits per pixel row mode allows for up to 3 x 64 ROM tiles and up to 1 x 64
  RAM tiles. Each 64 tile bank may be located independently of others within
  ROM or RAM respectively. This row mode is available for the sprite engine.

- 1 bits per pixel row modes at either 8 or 6 pixels per tile widths with
  selectable foreground and background colors, including an attribute mode
  (foreground color selectable for every tile). The 6 pixels per tile modes
  are particularly useful for text output.

- 215 cycles are available for the inline mixer. This allows for having five
  channels audio, of four channels and UART.

- In spare HBlank time the mode is capable to perform SD streaming, loading
  an arbitrary portion of an SD sector.

Overall key features of the sprite engine are as follows:

- Works with 8 x 8 pixel ROM or RAM sourced sprite tiles or single pixels.
  Index 0 of the sprite tiles is transparent.

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


ROM / RAM 1bpp tiles (both 6px and 8px widths)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1 bits per pixel tilesets always take 2048 bytes defining 256 tiles. In
Multicolor 2bpp mode this entire set can accessed, in other modes having 1bpp
tiles only 128 tiles of it may be visible at once (the visible region
selectable in 32 tile steps using 3 bits in the row mode's specification).

The layout is interleaved: The first 256 bytes contain row 0 of the tiles, the
second 256 bytes contain row 1, and the last contain row 7. In each such 256
bytes block one byte corresponds to one tile.

The byte's highest bit is the leftmost pixel, the lowest bit is the rightmost
pixel. In 6 pixels per tile mode the lowest 2 bits are not displayed.

Set bits use the foreground color, clear bits use the background color.


RAM 2bpp Multicolor surface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The 2 bits per pixel surface has a framebuffer layout: pixels appear
continously it from left to right as the memory address increments, then top
to bottom.

The highest 2 bits correspond to the leftmost pixel, the lowest 2 bits to the
rightmost.

Pixel values select attribute colors (2 bytes in VRAM for each 8 pixel wide
"tile") the following manner:

- 0: High nybble of byte 0.
- 1: Low nybble of byte 0.
- 2: High nybble of byte 1.
- 3: Low nybble of byte 1.

Note that unlike normal row modes, this has state across rows, so rearranging
the physical display produces different results. It simply works by fetching
a start address at the begin of a display frame, then whenever a multicolor
tile is encountered, the appropriate number of bytes are fetched from the
multicolor framebuffer.

This behavior may be exploited to perform partial vertical scrolls where the
multicolor content remains stationary.




Tile row modes overview
------------------------------------------------------------------------------


The mode of a row is selected by the m74_tdesc pointer, pointing into an array
of 32 bytes, each byte specifying a pointer into a tile row descriptor table.
Bit 7 of this value governs whether the ROM (0) or the RAM (1) tile descriptor
table should be used, located by the M74_ROMTD_OFF and M74_RAMTD_OFF
definitions respectively. One tile descriptor takes normally 5 bytes in these
tables.

Tile row 0 is special for the following uses:

- In Multicolor mode, bytes 3 and 4 of its descriptor specify the start
  address of the multicolor framebuffer.

- The sprite engine uses this row to locate the RAM tiles whose base offset
  must be identical across the whole display region used for sprite rendering.

Byte 0 of the tile descriptor specifies the row width and the row mode as
follows:

- bits 0 - 2: Row mode.
- bits 3 - 4: Row width: 0: 24 tiles, 1: 22 tiles, 2: 20 tiles, 3: 18 tiles.
- bits 5 - 7: In 1bpp row modes (4 - 7) used for tile index base in 32 tile
  steps. Separator line (2) uses these bits as flags.


Mode 0: 192 4bpp ROM tiles + 64 4bpp RAM tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x3F: 4bpp ROM tiles (base: byte 1)
- 0x40 - 0x7F: 4bpp ROM tiles (base: byte 2 - 0xF8)
- 0x80 - 0xBF: 4bpp ROM tiles (base: byte 3 - 0x10)
- 0xC0 - 0xFF: 4bpp RAM tiles (base: byte 4 - 0x08)

Tile descriptor bytes are used as indicated above: they specify the high byte
of the base offset for the tiles with the given offset. Note that one step in
the base means 8 tiles: it is possible to overlap distinct tile maps
exploiting this if necessary.


Mode 1: Flat tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: Flat tiles (colored by low 4 bits of tile index)
- 0x80 - 0xBF: 4bpp ROM tiles (base: byte 3 - 0x10)
- 0xC0 - 0xFF: 4bpp RAM tiles (base: byte 4 - 0x08)

The 0x80 - 0xFF range's use is identical to that of Row mode 0.

Bytes 1 and 2 of the tile descriptor are not used.


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


Mode 3: RAM 2bpp Multicolor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: ROM 6px wide 1bpp tiles
- 0x80 - 0xBF: ROM 8px wide 1bpp tiles
- 0xC0 - 0xFF: ROM 8px wide 1bpp tiles, Multicolor region start mark

Only bytes 0 - 2 are used from the tile descriptor. Bytes 3 and 4 are used if
it is the first row of the display, containing the start address of the
multicolor data.

This is an optional mode, needs to be enabled explicitly (M74_M3_ENABLE = 1)
if needed.

The various 1bpp tiles work the same manner like in other modes offering
similar capabilities. Their base is defined in byte 1 of the tile descriptor
and their colors are defined in byte 2.

The 0xC0 - 0xFF region uses a second VRAM byte specifying the number of
multicolor tiles following the tile. It can be zero, such tiles may be used
as fillers in such multicolor images which optimize their size by omitting
blank tiles (the filler takes 2 VRAM bytes like a normal multicolor tile,
thus allowing replacement without rearranging the VRAM).

The multicolor tiles use 2 VRAM bytes each, for four color attributes.

This tile row mode can not be scrolled horizontally, the related input is
completely ignored.

Note that the leftmost column can only be an 1bpp tile (optionally starting a
multicolor region). The rightmost tile must be an 1bpp tile of the 0x00 - 0xBF
range (also considering that 6px wide tiles can not be partially off screen).
Breaking these will corrupt the video signal.


Mode 4: ROM 8px wide 1bpp tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: ROM 8px wide 1bpp tiles (base: byte 1)
- 0x80 - 0xBF: 4bpp ROM tiles (base: byte 3 - 0x10)
- 0xC0 - 0xFF: 4bpp RAM tiles (base: byte 4 - 0x08)

The 0x80 - 0xFF range's use is identical to that of Row mode 0.

The foreground and background colors are selectable using byte 2 of the
descriptor (high nybble: foreground, low nybble: background) for the entire
row from the palette. Using color index 0 allows for using the related feature
(M74_COL0_OFF nonzero) to change this color every scanline.

This setup might be used for text output if the capability of X scrolling is
required. Otherwise the 6px wide modes may be more useful for this purpose.


Mode 5: RAM 8px wide 1bpp tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: RAM 8px wide 1bpp tiles (base: byte 1)
- 0x80 - 0xBF: 4bpp ROM tiles (base: byte 3 - 0x10)
- 0xC0 - 0xFF: 4bpp RAM tiles (base: byte 4 - 0x08)

The 0x80 - 0xFF range's use is identical to that of Row mode 0.

The foreground and background colors are selectable using byte 2 of the
descriptor (high nybble: foreground, low nybble: background) for the entire
row from the palette. Using color index 0 allows for using the related feature
(M74_COL0_OFF nonzero) to change this color every scanline.

This setup may be used for 1bpp rendering surfaces as well while the 4bpp ROM
tiles may be used for either framing that or supporting markers (sprites
realized with user code) on it. Up to 256 1bpp tiles may be feasible in this
manner.


Mode 6: ROM 6px wide 1bpp tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: ROM 6px wide 1bpp tiles (base: byte 1)
- 0x80 - 0xBF: 4bpp ROM tiles (base: byte 3 - 0x10)
- 0xC0 - 0xFF: 4bpp RAM tiles (base: byte 4 - 0x08)

The 0x80 - 0xFF range's use is identical to that of Row mode 0.

The foreground and background colors are selectable using byte 2 of the
descriptor (high nybble: foreground, low nybble: background) for the entire
row from the palette. Using color index 0 allows for using the related feature
(M74_COL0_OFF nonzero) to change this color every scanline.

Tiles of this mode come in packets of four. The first tile index selects the 6
pixels wide mode (if it below 0x80), then the subsequent three tile indices,
irrespective of their content, will map to 6 pixels wide 1bpp ROM tiles. The
packet covers 3 normal tiles worth of width.

This setup is generally preferred for text output as it is capable to display
more characters within the same area than 8 pixels wide tiles.

Note that tiles of this mode can not be scrolled partially off on the left or
right of the display. Attempting this will corrupt the video signal (it is
however possible to scroll it horizontally within the display region).


Mode 7: ROM 6px wide 1bpp tiles with attributes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: ROM 6px wide 1bpp tiles with attributes (base: byte 1)
- 0x80 - 0xBF: 4bpp ROM tiles (base: byte 3 - 0x10)
- 0xC0 - 0xFF: 4bpp RAM tiles (base: byte 4 - 0x08)

The 0x80 - 0xFF range's use is identical to that of Row mode 0.

The foreground and background colors are selectable using byte 2 of the
descriptor (high nybble: foreground, low nybble: background) for the entire
row from the palette. Using color index 0 allows for using the related feature
(M74_COL0_OFF nonzero) to change this color every scanline.

Tiles of this mode come in packets of four. The first tile index selects the 6
pixels wide mode (if it below 0x80), then the subsequent three tile indices,
irrespective of their content, will map to 6 pixels wide 1bpp ROM tiles. The
packet covers 3 normal tiles worth of width.

The packet uses 6 bytes of VRAM in the following layout:

- byte 0: Tile index of leftmost tile of packet
- byte 1: Next tile's index
- byte 2: High nybble: Leftmost tile color, Low nybble: next tile's color
- byte 3: Next tile's index
- byte 4: Tile index of last tile in packet
- byte 5: High nybble: next tile's color, Low nybble: last tile's color

This setup is useful for colored text output.

Note that tiles of this mode can not be scrolled partially off on the left or
right of the display. Attempting this will corrupt the video signal (it is
however possible to scroll it horizontally within the display region).




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


SD load function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This function may be used to stream in data from an SD card during the
display. It can load up to 512 bytes from a sector, but the more important
feature is that it can skip to a specific range (in 2 byte steps).

The following amount of bytes may be skipped or loaded depending on the
configured row widths and whether X scrolling or color 0 reloading takes
place:

+-------+----------------+-----------------+-------------+--------------+
| Width | XSH = 0, No C0 | XSH != 0, No C0 | XSH = 0, C0 | XSH != 0, C0 |
+=======+================+=================+=============+==============+
| 24    | 4 bytes        | 2 bytes         | 2 bytes     | 0 bytes      |
+-------+----------------+-----------------+-------------+--------------+
| 22    | 8 bytes        | 6 bytes         | 6 bytes     | 4 bytes      |
+-------+----------------+-----------------+-------------+--------------+
| 20    | 12 bytes       | 10 bytes        | 10 bytes    | 8 bytes      |
+-------+----------------+-----------------+-------------+--------------+
| 18    | 16 bytes       | 14 bytes        | 14 bytes    | 12 bytes     |
+-------+----------------+-----------------+-------------+--------------+

More loads are available in the following cases:

- Row mode 2 (Separator line) with no palette replacement: 70 - 76 bytes.
- Row mode 2 (Separator line) with palette replacement: 32 - 38 bytes.

For the highest efficiency of this function, about 550 bytes worth of SD
accesses have to be allowed per frame, otherwise trailing accesses may spill
off to be processed by M74_Finish().

The M74_Finish() function must be called after the frame to finish the load
and to clock out the SD card properly.


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
