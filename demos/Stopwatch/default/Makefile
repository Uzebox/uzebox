#################################
# Makefile for Uzebox stopwatch #
#################################

## General Flags
PROJECT = Stopwatch
GAME= Stopwatch
MCU = atmega644
TARGET = $(GAME).elf
CC = avr-gcc
INFO=../gameinfo.properties
UZEBIN_DIR = ../../../bin

## Kernel settings
KERNEL_DIR = ../../../kernel
KERNEL_OPTIONS = -DVIDEO_MODE=3 -DINTRO_LOGO=0 -DSCROLLING=0 -DRESOLUTION_EXT=1 -DVRAM_TILES_H=32
KERNEL_OPTIONS += -DMAX_SPRITES=8 -DRAM_TILES_COUNT=20 -DSCREEN_TILES_V=27 -DFIRST_RENDER_LINE=24 -DOVERLAY_LINES=0
KERNEL_OPTIONS += -DMUSIC_ENGINE=STREAM -DSOUND_MIXER=1 -DSOUND_CHANNEL_5_ENABLE=1

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(GAME).map
LDFLAGS += -Wl,-gc-sections
##LDFLAGS += -Wl,--section-start,.noinit=0x800100 -Wl,--section-start,.data=0x800500

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os -fsigned-char -ffunction-sections -fno-toplevel-reorder
CFLAGS += -fdata-sections -ffreestanding -fomit-frame-pointer -fmerge-all-constants
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d
CFLAGS += $(KERNEL_OPTIONS)


## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom
HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = uzeboxVideoEngineCore.o  uzeboxCore.o uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o $(GAME).o

## Objects explicitly added by the user
LINKONLYOBJECTS =

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)"

## Build
all: $(TARGET) $(GAME).hex $(GAME).eep $(GAME).lss $(GAME).uze size


## Rebuild graphics resources
../data/Upper-UI.inc: ../data/tileset.png ../data/tileset.xml
	$(UZEBIN_DIR)/gconvert ../data/tileset.xml

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

## Compile game sources
$(GAME).o: ../Stopwatch.c
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
	-$(UZEBIN_DIR)/packrom $(GAME).hex $@ $(INFO)

UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
#AVRSIZEFLAGS := -A ${TARGET}
#ifneq (,$(findstring MINGW,$(UNAME)))
#AVRSIZEFLAGS := -C --mcu=${MCU} ${TARGET}
#endif

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(RESOURCES) $(OBJECTS) $(GAME).* *.uze dep


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)
