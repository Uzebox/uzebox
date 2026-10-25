###############################################################################
# Makefile for the project Uzeamp
###############################################################################

## General Flags
PROJECT = Uzeamp
GAME = Uzeamp
MCU = atmega644
TARGET = $(GAME).elf
CC = avr-gcc
INFO=../gameinfo.properties

## Tools path
UZEBIN_DIR = ../../../bin/

## Kernel settings
KERNEL_DIR = ../../../kernel
PYTHON ?= python
## The video mode: uzmmode, mode 5's text frames (the settings below) and movie frames. Its
## folder as the compiler sees it (a native Windows avr-gcc: C:/ paths)
MODE_SRC = ../uzmmode
MODE_DIR := $(shell cygpath -m $(realpath $(MODE_SRC)) 2>/dev/null || echo $(realpath $(MODE_SRC)))
KERNEL_OPTIONS  = -DVIDEO_MODE=0 -DVIDEO_MODE_PATH=$(MODE_DIR)
KERNEL_OPTIONS += -DINTRO_LOGO=0 -DFIRST_RENDER_LINE=32 -DFRAME_LINES=192 -DENABLE_MIXER=0 -DTILE_WIDTH=8 -DVRAM_TILES_V=25
## UzeFS read-only (Uzeamp never writes to the card), FAT16 and FAT32, one file. A 128-byte
## read buffer: seeking follows the cluster chain a FAT entry a cluster, and the buffer serves
## 32 FAT32 entries a card read.
KERNEL_OPTIONS += -DUZEFS_WRITE=0 -DUZEFS_FAT32=1 -DUZEFS_MAX_FILES=1 -DUZEFS_RDBUF=128

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -O3 -fsigned-char -ffunction-sections -fno-toplevel-reorder
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 
CFLAGS += $(KERNEL_OPTIONS)
CFLAGS += $(EXTRA)		# e.g. make EXTRA=-DUZEAMP_REPORT=1 (a console check build: see Uzeamp.c)


## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2 

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(GAME).map 
LDFLAGS += -Wl,-gc-sections 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = uzeboxVideoEngineCore.o uzeboxCore.o uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o uzefs.o uzefs_sd.o $(GAME).o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" 

## Build
all: ../data/tiles.inc $(TARGET) $(GAME).hex $(GAME).eep $(GAME).lss $(GAME).uze size ramcheck

## The movie frames' line program, generated and timing-checked
$(MODE_SRC)/uzm_frame.inc: $(MODE_SRC)/gen_uzm.py
	$(PYTHON) -I $<

## Regenerate the graphics include file
../data/tiles.inc: ../data/tiles.png ../data/gconvert.xml
	$(UZEBIN_DIR)gconvert ../data/gconvert.xml

## Compile Kernel files
uzeboxVideoEngineCore.o: $(KERNEL_DIR)/uzeboxVideoEngineCore.s $(MODE_SRC)/uzm_frame.inc $(MODE_SRC)/videoModeCore.s $(MODE_SRC)/mode5.s $(MODE_SRC)/uzm.def.h
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxSoundEngineCore.o: $(KERNEL_DIR)/uzeboxSoundEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxCore.o: $(KERNEL_DIR)/uzeboxCore.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxSoundEngine.o: $(KERNEL_DIR)/uzeboxSoundEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxVideoEngine.o: $(KERNEL_DIR)/uzeboxVideoEngine.c $(MODE_SRC)/videoMode.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

terminal.o: ../terminal.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
	
uzefs.o: $(KERNEL_DIR)/uzefs/uzefs.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
	
uzefs_sd.o: $(KERNEL_DIR)/uzefs/uzefs_sd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

## Compile game sources
$(GAME).o: ../Uzeamp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

%.uze: $(TARGET)
	-$(UZEBIN_DIR)packrom $(GAME).hex $@ $(INFO)

## Conditional setting based on whether avr-size supports -C
ifeq ($(shell avr-size --help | grep '\-C' >/dev/null && echo yes ),yes)
AVRSIZEFLAGS := -C --mcu=${MCU} ${TARGET}
else
AVRSIZEFLAGS := -A ${TARGET}
endif

size: ${TARGET}
	@echo
	@avr-size ${AVRSIZEFLAGS}

## .data and .bss must end below the video ring (0x0800, uzmmode); the stack has 0x1000-0x10FF
.PHONY: ramcheck
ramcheck: $(TARGET)
	@end=$$(avr-nm $(TARGET) | awk '$$3=="__bss_end"{print $$1}'); 	if [ $$((0x$$end)) -gt $$((0x800800)) ]; then 		echo "error: .bss ends at 0x$$end, inside the video ring (0x800800)" >&2; rm -f $(TARGET); exit 1; 	else echo ".bss ends at 0x$$end: below the video ring"; fi

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(GAME).* dep/* *.hex *.uze ../data/tiles.inc ../data/tiles.inc


## Flash the already built $(TARGET) to the Uzebox with an Atmel-ICE. Same
## atprogram call as demos/Basic's `make flash`: chip erase, then program flash
## from the .elf. Nothing is built: it flashes whatever was built last.
##   make flash                   6-pin ISP header at 4 MHz (the default)
##   make flash INTERFACE=jtag    JTAG header
##   make flash ATPROGRAM="D:/Tools/atprogram.exe"
INTERFACE ?= isp
ATPROGRAM ?= C:/Program Files (x86)/Microchip/Studio/7.0/atbackend/atprogram.exe
ifeq ($(INTERFACE),jtag)
ATPROGRAM_FLAGS = -v -f -t atmelice -i JTAG -d $(MCU) -cl 2mhz
else ifeq ($(INTERFACE),isp)
ATPROGRAM_FLAGS = -v -f -t atmelice -i ISP -d $(MCU) -cl 4mhz
else
$(error INTERFACE must be jtag or isp, not "$(INTERFACE)")
endif

.PHONY: flash
flash:
	@[ -f $(TARGET) ] || { echo "make flash: $(TARGET) not found, build it first" >&2; exit 1; }
	"$(ATPROGRAM)" $(ATPROGRAM_FLAGS) program -c -fl -f $(TARGET)

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

