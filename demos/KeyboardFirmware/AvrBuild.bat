@ECHO ON
rem avrasm2.exe from atmel studio must be on the path
rem set incdir to point on atmel include files
set incdir="C:\Program Files (x86)\Microchip\Studio\7.0\packs\atmel\ATtiny_DFP\1.10.348\avrasm\inc"
avrasm2.exe -I %incdir% -S labels.tmp -fI -W+ie -C V2 -o KeyboardFirmware.hex -d KeyboardFirmware.obj -e KeyboardFirmware.eep -m KeyboardFirmware.map KeyboardFirmware.asm
