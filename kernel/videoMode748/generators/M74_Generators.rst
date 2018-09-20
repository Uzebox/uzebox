
Video Mode 748 - Data generators
==============================================================================


These are simple image & other data generators for Mode 748.

Most use Gimp's header output feature. You can generate the necessary input
header files for them by converting the source image to indexed color, then
saving it as a "header" (Note: Gimp can also save as ".c" source, which
produces a different format).

For the expected layout of the images, see the description of the appropriate
generator.

When the header file is ready, copy it somewhere accessible for compiling with
the generator, and change the appropriate "#include" line in the generator's
source to include it.

Here only a list is provided, all the generators are described in their head
comments.

- c1bppmh.c: 1 bit / pixel masks (For the sprite engine or Mode 6 / 7 fonts)
- c4bpph.c: 4 bit / pixel sprites and tiles (Row mode 0)
