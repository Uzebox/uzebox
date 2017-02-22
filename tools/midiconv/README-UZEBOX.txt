Converts a MIDI song in format 0 or 1 to a Uzebox MIDI stream
outputted as a C include file.

This is a cross-platform C++ port of the Java tool named MidiConvert
contained within the uzebox/tools/JavaTools/dist/uzetools.jar package.

It is built using the Midifile, a C++ library for parsing Standard
MIDI Files, which is Copyright (c) 1999-2015, Craig Stuart Sapp. All
rights reserved.

To speed up the build time, all of the original examples that were in
the src-programs directory have been moved to src-programs-extras. Any
of those files can be copied back into the src-programs directory and
then built using the instructions below.

BUILD/INSTALL INSTRUCTIONS:

    make

or:

    make library
    make midiconv

This will build and install the midiconv binary as uzebox/bin/midiconv
at which point you can run it using basically the same options as the
Java version, but pay close attention to the command line arguments
because some may be slightly different, and long options are provided.

To get a list of all supported options, just run:

    ./midiconv --help


TIPS AND TRICKS:

If you used Rosegarden to create a MIDI file by arranging multiple
segments, unless those segments are joined by clicking the name of
each track (to select all segments on that track) and pressing Ctrl-J
to join them, the resulting Uzebox C include file may contain multiple
redundant control messages, which wastes space. After you export your
MIDI (using File > Export > Export MIDI File...) you can undo these
joins if you wish to keep your original segmented source file (for
instance to preserve any of your linked segments).
