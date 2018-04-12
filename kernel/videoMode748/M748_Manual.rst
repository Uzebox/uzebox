
Video Mode 748 - Developer's manual
==============================================================================




Introduction
------------------------------------------------------------------------------


Video Mode 748 is a 7 cycle / pixel palettized mode at up to 4bpp (16 colors)
depth within a scanline. It is capable to display 192 pixels (24 tiles of 8
pixels width) horizontally or 384 pixels for 1bpp images from the SPI RAM and
text.

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
  along with arbitrary VRAM start positions for the background (in SPI RAM).

- The palette is represented as a simple 16 byte array, allowing for palette
  effects (such as fading). Color 0 (background) of the palette may be changed
  for every row. Using a special single colored row mode, the entire palette
  can be replaced during display.

- Up to 224 scanlines height, configurable by the Uzebox kernel's
  SetRenderingParameters() function.

- 4 bits per pixel row mode allows for up to 2 x 128 ROM tiles and up to 128
  RAM tiles (depending on available RAM). This row mode is available for the
  sprite engine.

- 234 cycles are available for the inline mixer. This allows for having five
  channels audio, of four channels and UART.

Overall key features of the sprite engine are as follows:

- Works with 8 x 8 pixel SPI RAM sourced sprite tiles or single pixels. Index
  0 of the sprite tiles is transparent.

- Blitter concept with background restoring: for rendering sprites, blits are
  to be called placing sprites on the canvas like if it was a regular
  framebuffer. RAM tile allocation and related tasks are carried out by the
  sprite engine internally. The blits can be cleared by a simple operation to
  start a new render.

- X and Y mirroring.

- RAM tile usage can be controlled allowing the use of 1 to 128 RAM tiles.

- Can perform blits over the 4 bits per pixel row mode. It can cope with
  different tilesets on the same screen (allowing the use of more than 256
  ROM tiles), and can use RAM tiles as well as targets (so it will blit
  normally over RAM tiles not allocated for sprites).

- Supports recoloring the sprite tiles by recolor tables.

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
of 32 words, each word specifying a pointer to a VRAM row whose first bytes
describe the type and parameters of the row.

Byte 0 of the tile descriptor specifies the row mode and flags as follows:

- bits 0 - 2: Row mode.
- bits 3 - 7: Flags (usage depends on row mode).


Mode 0: 256 4bpp ROM tiles + 128 4bpp RAM tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The VRAM row is as follows:

- byte 0: bits 0 - 2: 0 (Mode 0)
- byte 0: bits 4 - 6: X shift (0 - 7 pixels to the left)
- byte 0: bit 7: 16th bit of background VRAM row address (in SPI RAM)
- byte 1: Background VRAM row address low (in SPI RAM)
- byte 2: Background VRAM row address high (in SPI RAM)
- byte 3: ROM tiles 0x00 - 0x7F base high
- byte 4: ROM tiles 0x80 - 0xFF base high
- byte 5 - 29: VRAM row for RAM tiles (24 + 1 tiles)

Tile descriptor bytes are used as indicated above: they specify the high byte
of the base offset for the tiles with the given offset. Note that one step in
the base means 8 tiles: it is possible to overlap distinct tile maps
exploiting this if necessary.

If bit 7 of the VRAM byte is set, then it is a RAM tile, otherwise a ROM tile
determined by the Background VRAM's entry.


Mode 2: Separator line with palette reload
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This mode is capable to display a simple separator line of a single color
with optional palette replacement.

The VRAM row is as follows:

- byte 0: bits 0 - 2: 2 (Mode 2)
- byte 0: bit 4: If set, Color 0 is used for the line
- byte 0: bits 5 - 6: Palette source: 0: None, 1: RAM, 2: ROM, 3: SPI RAM
- byte 0: bit 7: 16th bit of Palette address (in SPI RAM)
- byte 1: Palette address, low
- byte 2: Palette address, high
- byte 3: Color of the separator line (if byte 0, bit 4 clear)

Using Color 0 for the separator line allows for Color 0 replacement to work on
it if necessary.


Mode 4: SPI RAM 4bpp bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The VRAM row is as follows:

- byte 0: bits 0 - 2: 4 (Mode 4)
- byte 1 - 96: Left column data (8 x 12 bytes for storing 24 pixels width)

This mode is special due to its requirements, and must be on the top of a
frame (only modes not using the SPI RAM may be above it). It initializes the
SPI RAM at the top using m74_m4_addr, and reads 84 bytes of it on each line
sequentially.

This mode can be used to display 4bpp pictures at up to 192 x 224 pixels
resolution.


Mode 5: SPI RAM 3bpp bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The VRAM row is as follows:

- byte 0: bits 0 - 2: 5 (Mode 5)
- byte 0: bit 7: 16th bit of bitmap data address (in SPI RAM)
- byte 1: Bitmap data address low (in SPI RAM)
- byte 2: Bitmap data address high (in SPI RAM)

A line takes 72 SPI RAM bytes, the data address increments by 72 after every
line within the 8 lines tall tile row.

3 SPI RAM bytes encode 8 pixels as follows: ::

    60006111 62227333 74447555

Pixel 0 is the leftmost pixel, pixel 7 is the rightmost. They use the low 8
indices of the palette (so color 0 replacement may be used).


Mode 6: SPI RAM 1bpp bitmap with attributes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The VRAM row is as follows:

- byte 0: bits 0 - 2: 6 (Mode 6)
- byte 0: bit 7: 16th bit of bitmap data address (in SPI RAM)
- byte 1: Bitmap data address low (in SPI RAM)
- byte 2: Bitmap data address high (in SPI RAM)
- byte 3 - 98: Attribute RAM for the row (96 bytes)

Attributes: background (0) and foreground (1) colors for each 8x8 pixel block.

A line takes 48 SPI RAM bytes.

This mode can be used to display 1bpp attribute mode pictures at up to
384 x 224 pixels resolution.

The mode has to be enabled by setting M74_M67_ENABLE nonzero to be used.


Mode 7: SPI RAM 1bpp bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The VRAM row is as follows:

- byte 0: bits 0 - 2: 7 (Mode 7)
- byte 0: bit 4: If set, Color 0 is used for background
- byte 0: bit 7: 16th bit of bitmap data address (in SPI RAM)
- byte 1: Bitmap data address low (in SPI RAM)
- byte 2: Bitmap data address high (in SPI RAM)
- byte 3: Foreground color
- byte 4: Background color (if byte 0, bit 4 clear)

Using Color 0 for the background allows for Color 0 replacement to work on it
if necessary.

A line takes 48 SPI RAM bytes.

This mode can be used to display 1bpp pictures at up to 384 x 224 pixels
resolution.

The mode has to be enabled by setting M74_M67_ENABLE nonzero to be used.




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

This list uses byte pairs defining locations where the logical scanline
counter has to be re-loaded. Afterwards the logical scanline counter
increments by one on every line. The byte pairs are as follows:

- byte 0: Physical scanline to act on (0 - 223)
- byte 1: Logical scanline to set

The first byte is a Logical scanline to set (0 for physical scanline is
implicit). The list can be terminated by a byte 0 value which can not be
reached any more, such as zero or 255.




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


To support the Uzebox kernel's Print function, the SetTile and the SetFont
routines are implemented to operate on Row Mode 6 or 7, by drawing character
images onto the SPI RAM bitmap. So these routines are quite slow, however such
text doesn't consume SRAM.


Uzebox logo
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There is no Uzebox logo display support in this mode. Games using the mode are
recommended to load a logo from external data and display that using an
applicable row mode, this way the precious ROM space can be preserved for game
code and tile data.
