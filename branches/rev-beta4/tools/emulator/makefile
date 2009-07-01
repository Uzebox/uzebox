# Emulator makefile - by Eric Anderton - 2009
# Public Domain - use at your own risk
# Note: targets Win32 environment 

EMULATOR = uzem.exe
GCCBASE = c:\minGW
MTOOLS = utils/mtools.exe
SDCARDDRIVE = g

# enable to show SPI debug messages
#UZEMFLAGS = -DUSE_SPI_DEBUG=1

# allows for stdout access from ports 0x1A and 0x19
UZEMFLAGS += -DUSE_PORT_PRINT=1

CPPFLAGS = -O3 -I$(GCCBASE)\include -march=i686
CPPFLAGS += -I$(GCCBASE)/include/SDL -D_GNU_SOURCE=1 -DGUI=1
LFLAGS += -L$(GCCBASE)/lib -lmingw32 -lSDLmain -lSDL

UZEMOBJECTS = uzem.o avr8.o uzerom.o

$(UZEMOBJECTS): %.o: %.cpp %.h
	g++ -c $< -o $@ $(CPPFLAGS)

$(EMULATOR): $(UZEMOBJECTS)
	g++ $(UZEMOBJECTS) -o $(EMULATOR) $(LFLAGS) $(UZEMFLAGS)

emulator: $(EMULATOR)
    
all: emulator
        
SDCardDemo:
	make -C ../demos/SDCardDemo all
	cp ../demos/SDCardDemo/SDCardDemo.hex .
    
debug-sd: emulator SDCardDemo
	$(EMULATOR) --sd $(SDCARDDRIVE) SDCardDemo/SDCardDemo.hex
    
clean:
	-@rm $(UZEMOBJECTS) $(EMULATOR)