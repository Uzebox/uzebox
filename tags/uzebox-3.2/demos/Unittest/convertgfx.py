"""
	Copyright (c) 2009 Eric Anderton
        
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
"""

from PIL import Image
import getopt
import sys
import os

helptext ="""
Uzebox tile dataset generator - creates .c source from image data
(c) 2009 Eric Anderton

Usage: convertgfx [options] IMAGEFILE

-6 -l   Treat data as low-resolution 6px wide tiles 
-8 -h   Treat data as hi-resolution 8px wide tiles (default)
-c      Number tile columns in file (default is 16)
-r      Number of tile rows in file (rows - default is 16)
-w      Output in wide format (one tile per row in output source)
-i      Identifier of the data set on output (defaults to image name)
--help  Show this message
"""

width = 8
columns = 16
rows = 16
wideFormat = False
ident = None

(optlist,args) = getopt.getopt(sys.argv[1:],"6lh8wc:r:i:",["help"])

for (opt,value) in optlist:
    if opt == "-6" or opt == "-l":
        width = 6
    elif opt == "-8" or opt == "-h":
        width = 8
    elif opt == "-w":
        wideFormat = True
    elif opt == "-c":
        columns = int(value)
    elif opt == "-r":
        rows = int(value)
    elif opt == "-i":
        ident = int(value)
    elif opt == "--help":
        print helptext
        exit(1)

if len(args) == 0:
    (filedir,name) = os.path.split(sys.argv[0])
    print "Error - No image specified."
    print "Try `%s --help` for more information." % name
    exit(1)

filename = args[0]

(filedir,name) = os.path.split(filename)
(base,ext) = os.path.splitext(name)

outfile = open(filedir+base+".c","wt+")

if ident == None:
    ident = base

image = Image.open(filename).convert("RGB")
data = image.load()
xref = []

outfile.write("/* Autogenerated by convertgfx.py - do not modify */")
outfile.write("#include <avr/pgmspace.h>\n")
outfile.write("unsigned char %s[] PROGMEM = {\n" % ident)

tileno = 0

print data

for row in range(0,rows):
    yofs = row*8
    for col in range(0,columns):
        xofs = col*width
        outfile.write("/* tile number: %d */\n" % tileno)
        for y in range(0,8):
            outfile.write("\t")
            for x in range(0,width):
               # print "pixel %d %d" % (xofs+x,yofs+y)
                (r,g,b) = data[xofs+x,yofs+y]
                value = ((r & 0xe0) >> 5) | ((g & 0xe0) >> 2) | (b & 0xC0)
                outfile.write("0x%02x," % value)
            if not wideFormat:
                outfile.write("\n")
        tileno = tileno + 1        
outfile.write("};\n")
outfile.close()



