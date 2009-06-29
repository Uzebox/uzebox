# Emulator makefile - by Eric Anderton - 2009
# Public Domain - use at your own risk
# Note: targets Win32 environment 

EMULATOR = uzem.exe
DUMPER = dump.exe
HEX2CPP = hex2cpp.exe
GCCBASE = c:\minGW
MTOOLS = utils/mtools.exe
SDCARDDRIVE = e

CPPFLAGS = -O3 -I$(GCCBASE)\include -march=i686
CPPFLAGS += -I$(GCCBASE)/include/SDL -D_GNU_SOURCE=1 -DGUI=1
LFLAGS += -L$(GCCBASE)/lib -lmingw32 -lSDLmain -lSDL

UZEMOBJECTS = uzem.o avr8.o

$(UZEMOBJECTS): %.o: %.cpp %.h
	g++ -c $< -o $@ $(CPPFLAGS)

$(EMULATOR): $(UZEMOBJECTS)
	g++ $(UZEMOBJECTS) -o $(EMULATOR) $(LFLAGS)

emulator: $(EMULATOR)
    
all: emulator

run: clean emulator
	$(EMULATOR) roms/arkanoid.hex
        
SDCardDemo:
	make -C SDCardDemo all
	cp SDCardDemo/SDCardDemo.hex roms
 
dump.exe: dump.c
	g++ dump.c -odump.exe
    
debug: emulator
	$(EMULATOR) --img data/uzeamp.img roms/sdcarddemo.hex 
    
debug-uzeamp: emulator
	$(EMULATOR) --img data/uzeamp.img roms/uzeamp.hex 
    
debug-video: emulator
	$(EMULATOR) --img data/video.img roms/videodemo.hex

debug-dump: emulator $(DUMPER)
	dump.exe $(SDCARDDRIVE) sdcard.img
    
debug-sd: emulator SDCardDemo
	$(EMULATOR) --sd $(SDCARDDRIVE) SDCardDemo/SDCardDemo.hex
    
clean:
	-@rm $(UZEMOBJECTS) $(EMULATOR) sdcard.img dump.o dump.exe