
Video Mode 74 - Developer's manual
==============================================================================




Introduction
------------------------------------------------------------------------------


Video Mode 74 is a 7 cycle / pixel palettized mode at up to 4bpp (16 colors)
depth within a scanline. It is capable to display up to 192 pixels (24 tiles
of 8 pixels width) horizontally.

The 7 cycles / pixel figure produces the same essential pixel clock like the
NTSC Commodore 64's Multicolor mode, having an 1,5:1 pixel aspect ratio.

Overall key features are as follows:

- Up to 192 pixels width (176, 160 and 144 pixel widths are also selectable)
- Up to 224 lines tall (configurable by SetRenderingParameters)
- X and Y scrolling supported
- Tiled, using 8x8 pixel tiles in most modes
- Up to 192 4bpp ROM tiles / tile row
- Up to 64 4bpp RAM tiles / tile row
- Up to 128 1bpp ROM tiles with selectable fg & bg color / tile row
- Up to 128 1bpp RAM tiles with selectable fg & bg color / tile row
- 2bpp rendering surface of arbitrary width above 2 tiles
- Up to 128 6 px wide 1bpp ROM tiles with selectable fg & bg color / tile row
- Up to 128 6 px wide 1bpp ROM tiles with individually selectable fg color
- Uses the inline mixer (either 5 channels or 4 channels + UART available)
- RAM clear function during spare time to assist rendering surfaces
- SPI load function during spare time for streaming in data from SD card

Graphics output is primarily governed by tile rows of which 32 can be
configured to cover 256 logical scanlines. Each logical scanline may end up
at any physical scanline of the display.

A physical scanline's render may be summarized by the following steps:

- Select the logical scanline to render based on the configured selection
  method.

- Interpret the tile row's configuration to which the scanline belongs,
  setting up for the line's render, including calculating offsets for ROM
  or RAM tile data sources.

- Render the line.




Data formats
------------------------------------------------------------------------------


ROM 4bpp tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ROM 4bpp tilesets always take 4Kb, and has to be located at 256 byte boundary.
They always contain definitions for 128 tiles. Tile 0 uses the following
bytes:

- byte 0x000: High nybble: Row 0, pixel 0; Low nybble: Row 0, pixel 1.
- byte 0x001: High nybble: Row 0, pixel 2; Low nybble: Row 0, pixel 3.
- byte 0x002: High nybble: Row 0, pixel 4; Low nybble: Row 0, pixel 5.
- byte 0x003: High nybble: Row 0, pixel 6; Low nybble: Row 0, pixel 7.
- byte 0x200: High nybble: Row 1, pixel 0; Low nybble: Row 1, pixel 1.
- (and so on...)
- byte 0xF03: High nybble: Row 7, pixel 6; Low nybble: Row 7, pixel 7.

Subsequent tile start offsets (the byte containing row 0, pixel 0) can be
acquired by multiplying the tile index by 4, and adding this to the base
offset.


RAM 4bpp tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

RAM 4bpp tilesets can take up to 2Kb, and doesn't have any boundary
constraint (may start anywhere in RAM). They may contain definitions for up
to 64 RAM tiles.

RAM tile start offsets are calculated by multiplying the tile index (modulo
64) by 2, and adding this to the base offset.

The size of the tileset can be tuned by an offset increment per row parameter:
setting it to 4 times the desired number of tiles covers the appropriate
amount of memory for them (the setting of zero is interpreted as 256, thus
allowing the use of 64 tiles).


ROM / RAM 1bpp tiles (both 6px and 8px widhts)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1bpp tilesets can take up to 1Kb, and doesn't have any boundary constraint
(may start anywhere in RAM / ROM). They may contain definitions for up to 128
tiles.

Tile start offsets are derived directly from the tile index (modulo 128), and
adding this to the base offset.

The size of the tileset can be tuned by an offset increment per row parameter:
setting it to the desired number of tiles covers the appropriate amount of
memory for them.

Note that the layout of tiles is ideal for constructing direct rendering
surfaces, providing continuous lines in this case for incrementing tile
indices.

6 pixels wide tiles use the upper 6 bits for output (highest bit for the
leftmost pixel).


RAM 2bpp surface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The RAM 2bpp surface adheres the same rules like tilesets, however the width
of it can be supplied by parameter, thus supporting linear layout directly,
and also saving VRAM (since only one tile index byte is required to output
the configured width of 2bpp surface).




Tile row modes overwiev
------------------------------------------------------------------------------


Mode 0: 192 4bpp ROM tiles + 64 4bpp RAM tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x3F: 4bpp ROM tiles
- 0x40 - 0x7F: 4bpp RAM tiles
- 0x80 - 0xFF: 4bpp ROM tiles

The 0x00 - 0x3F region also requires a 4Kb tile map, but can only use the
lower or upper half of it depending on the defined start offset. When using
several sets of tiles, this doesn't result in a waste since two distinct such
regions may share a 4Kb tile map, or in other cases, such a region may also
re-use the lower or higher half of a tile map otherwise used for full (128
tile) sets.


Mode 1: RAM 8px wide 1bpp tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: RAM 8px wide 1bpp tiles
- 0x80 - 0xFF: 4bpp ROM tiles

The foreground and background colors are selectable for the entire row from
the palette. Using color index 0 allows for using the related feature (color0
reload) to change this color every scanline.

This setup may be used for 1bpp rendering surfaces as well while the 4bpp ROM
tiles may be used for framing that. Note that by properly setting up tile
descriptors, arbitrary number of tiles (more than 128) may be accessed to
build a larger surface.


Mode 2: ROM 8px wide 1bpp tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: ROM 8px wide 1bpp tiles
- 0x80 - 0xFF: 4bpp ROM tiles

The foreground and background colors are selectable for the entire row from
the palette. Using color index 0 allows for using the related feature (color0
reload) to change this color every scanline.

This setup might be used for text output if the capability of X scrolling is
required. Otherwise the 6px wide modes may be more useful for this purpose.


Mode 4: RAM 2bpp region
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: RAM 2bpp region
- 0x80 - 0xFF: 4bpp ROM tiles

This mode uses colors 0 - 3 from the palette. The color0 reload feature may be
used to increase the number of colors in this region by changing it on every
scanline.

This mode may typically be used to construct direct rendering surfaces of
arbitrary sizes. Depending on the requirements, tile rows can be set up so
only a single line of VRAM is required to set up every line of this region,
thus saving memory.

The RAM clear feature may assist certain rendering tasks by clearing the
surface after the rendering within spare video display cycles.

A ROM scanline map may be used to achieve double scanning effect on this
region, thus increasing its apparent size.

Note that tiles of this mode can not be scrolled partially off on the left or
right of the display. Attempting this will corrupt the video signal (it is
however possible to scroll it horizontally within the display region).


Mode 5: ROM 6px wide 1bpp tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: ROM 6px wide 1bpp tiles
- 0x80 - 0xFF: 4bpp ROM tiles

The foreground and background colors are selectable for the entire row from
the palette. Using color index 0 allows for using the related feature (color0
reload) to change this color every scanline.

Tiles of this mode come in packets of four. The first tile index selects the
mode (if it below 0x80), then the subsequent three tile indices, irrespective
of their content, will map to 6 pixels wide 1bpp ROM tiles. The packet covers
3 normal tiles worth of width.

This setup is generally preferred for text output as it is capable to display
more characters within the same area than 8 pixels wide tiles.

Note that tiles of this mode can not be scrolled partially off on the left or
right of the display. Attempting this will corrupt the video signal (it is
however possible to scroll it horizontally within the display region).


Mode 6: ROM 6px wide 1bpp tiles with attributes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: ROM 6px wide 1bpp tiles with attributes
- 0x80 - 0xFF: 4bpp ROM tiles

The background color is selectable for the entire row from the palette. Using
color index 0 allows for using the related feature (color0 reload) to change
this color every scanline. The foreground color can be specified for each
tile individually.

Tiles of this mode come in packets of four. The first tile index selects the
mode (if it below 0x80), then the subsequent three tile indices, irrespective
of their content, will map to 6 pixels wide 1bpp ROM tiles. The packet covers
3 normal tiles worth of width.

The packet uses 6 bytes of VRAM in the following layout:

- byte 0: Tile index of leftmost tile of packet
- byte 1: Next tile's index
- byte 2: Next tile's index
- byte 3: Tile index of last tile in packet
- byte 4: High nybble: Leftmost tile color, Low nybble: next tile's color
- byte 5: High nybble: next tile's color, Low nybble: last tile's color

This setup is useful for colored text output. It may share tile map with
normal (non-attribute mode) 6px wide regions as they use the same format.

Note that tiles of this mode can not be scrolled partially off on the left or
right of the display. Attempting this will corrupt the video signal (it is
however possible to scroll it horizontally within the display region).


Mode 7: Separator line with palette reload
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This mode uses no VRAM.

This mode is capable to display a simple separator line, where one entire tile
can have at most a single color with some limitations (symmetric layout).

An important underlaying feature is the capability to reload the entire
16 color palette from either ROM or RAM, thus supporting the use of multiple
color sets (or distinct palette effects) in separate vertical regions of the
display.

Instead of VRAM address, it takes the address of the palette to use: a packet
of 8 palettes, addressable by the selected row within the tile.

It is possible to specify the separator line to load its own colors from
either the old or the new palette. This allows for visually assigning these
lines to either the screen section below or above, in case of palette effect,
sharing the effect with the tied region.


