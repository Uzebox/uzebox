dconvert 2017 Lee Weber released under GPL 3

dconvert is designed to use a configuration file which specifies 1 or more transformations from input file(s) to output file(s).

Perhaps it's most common use is to create binary SD data, from multiple binary files, multiple C include files, or some mixture.
This can be integrated into a project's makefile so that a complex layout can be rapidly reproduced when changes are made. The uses
are many once you understand the capabilities.

dconvert is used as: "dconvert config-file.cfg", see demos/SpiRamMusicDemo for an example of 1 possible usage.

The configuration file passed to the program specifies the transformation details via comma separated values. The first line of this file sets a number of things for the remaining entries:
parameter 1: 0 = input file is C array, 1 = input file is binary
parameter 2: 0 = output file is C array, 1 = output file is binary
parameter 3: X = offset(32bit index num) to start writing directory for converted objects, -1 = don't write a directory, 4 = directory index 4, etc.
parameter 4: X = offset to start writing output data, -1 = append to end of existing file(meaningless for C output)
parameter 5: 0 = no sector padding, 1 = pad out to even multiple of 512(meaningless for C output)
parameter 6: X = minimum file size(padded with 0 output is output start+size is less than)
parameter 7: name of output file

If sector padding(binary mode) is specified, the directory will be written as sector offsets instead of byte offsets from beginning of file.

After the first line, any number of entry lines may be specified. If you wish to comment a line out, start it with a '#' character.

The format of the entry lines is:
parameter 1: name of input file
parameter 2: number of arrays to skip first(meaningless for binary input)
parameter 3: number of bytes to skip(in selected array) before processing data(meaningless for binary output)
parameter 4: 0 = raw transform, 1 = Patch Transform, >1 = ..future use
parameter 5: name of output array(meaningless for binary output)

Additionally there are line commands which can be triggered by starting a line with a special character.

System command line example:
;./conversion-script.sh map.wav map.raw
../data/pcm/map.raw,0,0,6,NULL,

Debugging example:
#DEBUG=1
#ADPCM-DEBUG=1
../data/pcm/map.raw,0,0,6,NULL,


Concatenation example:

0,1,0,512,1,0,../default/TOR_DATA.BIN,
#include C array data for graphics..
../data/inc/da-logo-f0.inc,0,0,0,NULL,
../data/inc/da-logo-f0.inc,1,0,0,NULL,
../data/inc/da-logo-f1.inc,0,0,0,NULL,
../data/inc/da-logo-f1.inc,1,0,0,NULL,
...
#NEXT
#include binary data at the end of the generated data above
1,1,4,-1,1,0,../default/TOR_DATA.BIN,
../data/pcm/intro.raw,0,0,6,NULL,
../data/pcm/title.raw,0,0,5,NULL,
../data/pcm/map.raw,0,0,6,NULL,
../data/inc/eof-dummy.inc,0,0,0,NULL,
