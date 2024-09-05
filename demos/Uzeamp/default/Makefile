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
KERNEL_OPTIONS  = -DVIDEO_MODE=3 -DINTRO_LOGO=0 -DRAM_TILES_COUNT=1 -DCENTER_ADJUSTMENT=0  -DSNES_MOUSE=0 -DSCROLLING=0
KERNEL_OPTIONS += -DMAX_SPRITES=1 -DSCREEN_TILES_V=24 -DVRAM_TILES_V=24 -DFIRST_RENDER_LINE=24 -DENABLE_MIXER=0 
KERNEL_OPTIONS += -DPF_USE_DIR=1 -DPF_USE_LSEEK=1 -DPF_USE_WRITE=0 -DPF_USE_READ=1 


## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -O3 -fsigned-char -ffunction-sections -fno-toplevel-reorder
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 
CFLAGS += $(KERNEL_OPTIONS)


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
OBJECTS = uzeboxVideoEngineCore.o uzeboxCore.o uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o pff.o diskio.o $(GAME).o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" 

## Build
all: ../data/tiles.inc $(TARGET) $(GAME).hex $(GAME).eep $(GAME).lss $(GAME).uze size

## Regenerate the graphics include file
../data/tiles.inc: ../data/tiles.png ../data/gconvert.xml
	$(UZEBIN_DIR)gconvert ../data/gconvert.xml

## Compile Kernel files
uzeboxVideoEngineCore.o: $(KERNEL_DIR)/uzeboxVideoEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxSoundEngineCore.o: $(KERNEL_DIR)/uzeboxSoundEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxCore.o: $(KERNEL_DIR)/uzeboxCore.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxSoundEngine.o: $(KERNEL_DIR)/uzeboxSoundEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxVideoEngine.o: $(KERNEL_DIR)/uzeboxVideoEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

terminal.o: ../terminal.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
	
diskio.o: $(KERNEL_DIR)/petitfatfs/diskio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
	
pff.o: $(KERNEL_DIR)/petitfatfs/pff.c
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
	echo
	avr-size ${AVRSIZEFLAGS}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(GAME).* dep/* *.uze ../data/tiles.inc ../data/tiles.inc


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

