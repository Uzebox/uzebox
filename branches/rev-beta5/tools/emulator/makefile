# Emulator makefile - by Eric Anderton - 2009
# Public Domain - use at your own risk
# Note: targets Win32 environment 

TARGET = uzem.exe
DEBUG_TARGET = uzemdbg.exe
GCCBASE = c:\minGW
MTOOLS = utils/mtools.exe
SDCARDDRIVE = g

CPPFLAGS = -O3 -I$(GCCBASE)\include -march=i686
CPPFLAGS += -I$(GCCBASE)/include/SDL -D_GNU_SOURCE=1 -DGUI=1
LFLAGS += -L$(GCCBASE)/lib -lmingw32 -lSDLmain -lSDL

UZEMOBJECTS = uzem.o avr8.o uzerom.o

$(UZEMOBJECTS): %.o: %.cpp %.h
	g++ -c $< -o $@ $(CPPFLAGS) $(UZEMFLAGS)
    
release: UZEMFLAGS = -DUSE_PORT_PRINT=1
release: clean-obj $(UZEMOBJECTS)
	g++ $(UZEMOBJECTS) -o $(TARGET) $(LFLAGS) $(UZEMFLAGS)

debug: UZEMFLAGS = -DUSE_PORT_PRINT=1 -DUSE_SPI_DEBUG=1 -DUSE_EEPROM_DEBUG=1
debug: clean-obj $(UZEMOBJECTS)
	g++ $(UZEMOBJECTS) -o $(DEBUG_TARGET) $(LFLAGS) $(UZEMFLAGS)
    
all: release
        
SDCardDemo:
	make -C ../demos/SDCardDemo all
	cp ../demos/SDCardDemo/SDCardDemo.hex .
    
debug-sd: emulator SDCardDemo
	$(DEBUG_TARGET) --sd $(SDCARDDRIVE) SDCardDemo/SDCardDemo.hex
    
clean-obj:
	-@rm $(UZEMOBJECTS) 
    
clean: clean-obj
	-@rm $(TARGET) $(DEBUG_TARGET) 