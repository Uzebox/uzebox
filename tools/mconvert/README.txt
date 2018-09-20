mconvert 2017 Lee Weber released under GPL 3

For the latest documentation, check http://uzebox.org/wiki/Mconvert_tutorial

This tool converts the standard midi output file from midiconv, into a compressed version.
The compression will vary depending on the input music data, but it should always be smaller than
the original. The main aim of this program is to increase the efficiency of the Uzebox streaming
music system, in that there is less data that must be buffered therefore requiring less buffer
and less cycles to fill that buffer(ie. SD access).

The only argument is the filename that will be used to described the desired conversion(s). Each
line specifies an individual conversion. It is important to follow the format and not include extra
line endings, commas, etc. The tool will handle some small deviances but it's not worth pushing.
You can comment out a line by making the first character on the line a '#'. All characters will be
ignored on a comment line until after the first \n encountered(should support Windows\Unix style).
Any line that is not a setup or entry line, MUST CONTAIN '#' as the first character on the line.
Example:
# put some comments here if desired
1,0,512,1,MUSIC.DAT,
# the above line is the setup line, which dictates how the data will be handled
../data/song1.inc,0,
# the above is an entry line, you can have as many as you need
../data/song2.inc,0,
../data/song3.inc,0,
# end of config

The explanation of this file is as follows:
#binary, directory starting at 0, output starting at 512, pad song size to an even multiple of 512(SD sector size), MUSIC.DAT for output file
1,0,512,1,MUSIC.DAT,
#
#convert file mididata.inc, filter flags 15(no channel volume, expression, tremolo volume or rate), NULL arrayname(only valid for C array output)
mididata.inc,15,NULL
#
#convert file mididata2.inc, no filter flags, NULL as we are in binary mode and not C array mode
mididata2.inc,0,NULL
