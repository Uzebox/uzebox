This is a command line utility that can turn any input file into a C
style PROGMEM array, useful for including raw PCM data in a Uzebox
game, but can also be used to include any binary data file.

(Under the hood, it is actually just the xxd utility, but with slight
modifications to make it default to the correct C style array format
for Uzebox games.)


BUILD INSTRUCTIONS:

      make


EXAMPLE USAGE INSTRUCTIONS:

      ./bin2hex PCM_tada.raw PCM_tada.inc

This will take an input file called PCM_tada.raw, and create an output
file called PCM_tada.inc consisting of a hex-encoded representation of
the PCM.tada.raw file, stored in a C style array named PCM_tada_raw.
