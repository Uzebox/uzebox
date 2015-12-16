
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
- Special 2bpp Multicolor mode (with 8x8 pixel tiles) using RAM source
- Up to 128 6 px wide 1bpp ROM tiles with selectable fg & bg color / tile row
- Up to 128 6 px wide 1bpp ROM tiles with individually selectable fg color
- Uses the inline mixer (either 5 channels or 4 channels + UART available)
- SD load function during spare time for streaming in data from SD card

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
- byte 0xE03: High nybble: Row 7, pixel 6; Low nybble: Row 7, pixel 7.

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


ROM / RAM 1bpp tiles (both 6px and 8px widths)
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
of it can be supplied by compile-time parameter, thus supporting linear layout
directly, and also saving VRAM (since only one tile index byte is required to
output the configured width of 2bpp surface).


RAM 2bpp Multicolor surface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Similar to the normal 2bpp surface, however it operates strictly over a linear
framebuffer. Unlike other modes it has a state across rows: whenever a 2bpp
Multicolor tile is hit, the appropriate number of pixels are fetched from the
framebuffer, and the framebuffer's pointer increments accordingly.




Tile row modes overview
------------------------------------------------------------------------------


Mode 0: 192 4bpp ROM tiles + 64 4bpp RAM tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: 4bpp ROM tiles
- 0x80 - 0xBF: 4bpp ROM tiles (half of a 4K ROM tile map)
- 0xC0 - 0xFF: 4bpp RAM tiles

The 0x80 - 0xBF region also requires a 4Kb tile map, but can only use the
lower or upper half of it depending on the defined start offset. When using
several sets of tiles, this doesn't result in a waste since two distinct such
regions may share a 4Kb tile map, or in other cases, such a region may also
re-use the lower or higher half of a tile map otherwise used for full (128
tile) sets.


Mode 1: RAM 8px wide 1bpp tiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: RAM 8px wide 1bpp tiles
- 0x80 - 0xBF: 4bpp ROM tiles (half of a 4K ROM tile map)
- 0xC0 - 0xFF: 4bpp RAM tiles

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
- 0x80 - 0xBF: 4bpp ROM tiles (half of a 4K ROM tile map)
- 0xC0 - 0xFF: 4bpp RAM tiles

The foreground and background colors are selectable for the entire row from
the palette. Using color index 0 allows for using the related feature (color0
reload) to change this color every scanline.

This setup might be used for text output if the capability of X scrolling is
required. Otherwise the 6px wide modes may be more useful for this purpose.


Mode 3: RAM 2bpp Multicolor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: ROM 6px wide 1bpp tiles
- 0x80 - 0xBF: ROM 8px wide 1bpp tiles
- 0xC0 - 0xFF: ROM 8px wide 1bpp tiles, Multicolor region start mark

This is an optional mode, needs to be enabled explicitly (M74_M3_ENABLE = 1)
if needed.

The various 1bpp tiles work the same manner like in other modes offering
similar capabilities. The source however is fixed to start at a 256 byte
boundary in Flash, having a 256 byte row increment.

The 0xC0 - 0xFF region uses a second VRAM byte specifying the number of
multicolor tiles following the tile. It can be zero, such tiles may be used
as fillers in such multicolor images which optimize their size by omitting
blank tiles (the filler takes 2 VRAM bytes like a normal multicolor tile,
thus allowing replacement without rearranging the VRAM).

The multicolor tiles use 2 VRAM bytes each, for four color attributes. The
high nybble of the first byte specifies the color index to use for '0' pixels,
the low nybble of the second the color index for '3' pixels.

The multicolor tiles consume a 2bpp buffer, hitting a multicolor tile always
fetching 2 bytes (8 pixels) from it. The start offset of the buffer is only
set up on the frame lead-in (so it has state across rows unlike other modes).

This tile row mode can not be scrolled horizontally, the related input is
completely ignored.

Note that the leftmost column can only be an 1bpp tile (optionally starting a
multicolor region). The rightmost tile must be an 1bpp tile of the 0x00 - 0xBF
range (also considering that 6px wide tiles can not be scrolled off
partially). Breaking these will corrupt the video signal.


Mode 4: RAM 2bpp region
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tile indices are used as follows:

- 0x00 - 0x7F: RAM 2bpp region
- 0x80 - 0xBF: 4bpp ROM tiles (half of a 4K ROM tile map)
- 0xC0 - 0xFF: 4bpp RAM tiles

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
- 0x80 - 0xBF: 4bpp ROM tiles (half of a 4K ROM tile map)
- 0xC0 - 0xFF: 4bpp RAM tiles

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
- 0x80 - 0xBF: 4bpp ROM tiles (half of a 4K ROM tile map)
- 0xC0 - 0xFF: 4bpp RAM tiles

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
- byte 2: High nybble: Leftmost tile color, Low nybble: next tile's color
- byte 3: Next tile's index
- byte 4: Tile index of last tile in packet
- byte 5: High nybble: next tile's color, Low nybble: last tile's color

This setup is useful for colored text output. It may share tile data with
normal (non-attribute mode) 6px wide regions as they use the same format.

Note that tiles of this mode can not be scrolled partially off on the left or
right of the display. Attempting this will corrupt the video signal (it is
however possible to scroll it horizontally within the display region).


Mode 7: Separator line with palette reload
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This mode uses no VRAM.

This mode is capable to display a simple separator line, where one entire tile
can have at most a single color with some limitations (symmetric layout).

An important underlying feature is the capability to reload the entire 16
color palette from either ROM or RAM, thus supporting the use of multiple
color sets (or distinct palette effects) in separate vertical regions of the
display.

Instead of VRAM address, it takes the address of the palette to use: a packet
of 8 palettes, addressable by the selected row within the tile.

It is possible to specify the separator line to load its own colors from
either the old or the new palette. This allows for visually assigning these
lines to either the screen section below or above, in case of palette effect,
sharing the effect with the tied region.




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




Tile descriptors
------------------------------------------------------------------------------


The tile descriptors define 32 tile rows spanning 256 logical scanlines. They
contain the mode to use for rendering the row, selectors for tile data, and
mode specific configuration.

They may be located either in RAM or ROM, usually for memory efficiency, the
latter may be used.

The width of the display lines may be configured between 24 and 18 tiles (24,
22, 20 and 18 tile options). Note that if 24 tiles width is configured, the
Color 0 reload feature becomes inaccessible (whatever color 0 was before the
first 24 tiles wide scanline will be preserved).


Implementing scrolling
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Since the sprite engine relies on VRAM copies, usually a simple scrolling
method suitable for this is preferred.

To have this, on both X and Y the VRAM has to be one tile wider or taller
than what is displayed. For the latter a suitable amount of tile descriptors
have to be reserved for the scrolling region (one tile more than displayed).

To scroll horizontally, the X shift has to be used, shifting between 0 and 7
pixels, when wrapping, performing a VRAM copy.

To scroll vertically, the region's (in the split list) logical scanline has
to be altered similarly to X shifting, using the low 3 bits to scroll a tile
worth of lines. When wrapping, an appropriate VRAM copy has to be done.




The palette
------------------------------------------------------------------------------


The mode requires a 256 byte palette buffer, which it normally located at
0x0F00, below the Stack. Normally this buffer doesn't have to be accessed
since the mode automatically manages it.

A global (initial) 16 color (16 byte) palette either in RAM or ROM may be set
up to be loaded before starting the display of the frame. By manipulating this
palette in VBlank, palette effects (color cycling, fading) can be achieved.

The palette can be replaced within the frame by using the separator tile row
mode (Mode 7).

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
| 24    | 2 bytes        | 0 bytes         | \-          | \-           |
+-------+----------------+-----------------+-------------+--------------+
| 22    | 6 bytes        | 4 bytes         | 4 bytes     | 2 bytes      |
+-------+----------------+-----------------+-------------+--------------+
| 20    | 10 bytes       | 8 bytes         | 8 bytes     | 6 bytes      |
+-------+----------------+-----------------+-------------+--------------+
| 18    | 14 bytes       | 12 bytes        | 12 bytes    | 10 bytes     |
+-------+----------------+-----------------+-------------+--------------+

More loads are available in the following cases:

- Row mode 3 (2bpp Multicolor): Add 2 bytes each to the above figure.
- Row mode 7 (Separator line): 48 bytes, width irrelevant.

For using SD loading more efficiently at 24 tiles width, a compile time switch
(M74_SD_EXT) is provided which can add 2 bytes to all modes at the cost of
audio cycles.

The M74_Finish() function must be called after the frame to finish the load
and to clock out the SD card properly.


Color 0 reload
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Color 0 of the palette may be reloaded from a ROM / RAM table for every
scanline if the row width is less than 24 tiles (so 22, 20 or 18 tiles). This
reload overrides the previous color, even if it was supplied by a separator
line. Note that within a separator line, Color 0 reload is not active (so it
can not affect the coloring of the line itself).




Kernel integration
------------------------------------------------------------------------------


To support the Uzebox kernel's Print function, SetTile, SetFont and ClearVram
are implemented. Note however that they don't operate directly on the display
as this is not possible by the configurability of Mode 74.

To use these functions, first a target area has to be set up for them using
M74_SetVram (only available unless using a constant VRAM area). After this the
kernel functions will operate into that area like if it was VRAM. A proper
tile row configuration and scanline logic has to be set up to actually display
this region.

Some functions within the kernel rely on compile time defined width and height
parameters. These should be set up by planning how the kernel's output will be
displayed with Mode 74 (for example if 6 pixels wide tiles are used at 24
tiles width, 32 could be set up for VRAM_TILES_H and SCREEN_TILES_H).

Note that the sprite engine also operates on this VRAM.
