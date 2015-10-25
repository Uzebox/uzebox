###############################################################################
# Makefile for the project
###############################################################################

## General Flags
PROJECT = t2k
GAME= t2k
MCU = atmega644
TARGET = $(GAME).elf
CC = avr-gcc
INFO=../gameinfo.properties

## Kernel settings
KERNEL_DIR = ../../../kernel
KERNEL_OPTIONS = -DVIDEO_MODE=14 -DINTRO_LOGO=0  -DRAM_TILES_COUNT=256 -DSOUND_MIXER=1 -DMUSIC_ENGINE=1 -DSOUND_CHANNEL_5_ENABLE=1
#KERNEL_OPTIONS += -DFIRST_RENDER_LINE=50 -DFRAME_LINES=150


## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU) -nostartfiles

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os -fsigned-char -ffunction-sections -fno-toplevel-reorder
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 
CFLAGS += $(KERNEL_OPTIONS)
CFLAGS += -g3 -gdwarf-2
 
## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2
ASMFLAGS += -g3 -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(GAME).map 
LDFLAGS += -Wl,-gc-sections 
LDFLAGS += -Wl,--section-start=.renderlinesphasea=0x00000000
LDFLAGS += -Wl,--section-start=.wasted232bytes=0x00002218
LDFLAGS += -Wl,--section-start=.trigtableflash=0x00002100
LDFLAGS += -Wl,--section-start=.scaletableflash=0x00002200
LDFLAGS += -Wl,--section-start=.ztableflash=0x00002300
LDFLAGS += -Wl,--section-start=.freeflash=0x00002400
LDFLAGS += -Wl,--section-start=.freeflash2z=0x000038c5
LDFLAGS += -Wl,--section-start=.freeflash3=0x0000201A
LDFLAGS += -Wl,--section-start=.renderlinesphaseb=0x00004000
LDFLAGS += -Wl,--section-start=.text=0x0000601A
LDFLAGS += -Wl,--section-start=.objectstore=0x00800220				## must change #define in objects.c if this changes
LDFLAGS += -Wl,--section-start=.ramtiles=0x00800400
LDFLAGS += -Wl,--section-start=.vram=0x00800C00
LDFLAGS += -Wl,--section-start=.webtable=0x00800F80
##LDFLAGS += -Wl,--section-start=.checkstack=0x008010B0



## Intel Hex file production flagsd
HEX_FLASH_FLAGS = -R .eeprom
HEX_FLASH_FLAGS += -R .ramtiles
HEX_FLASH_FLAGS += -R .vram
HEX_FLASH_FLAGS += -R .objectstore
HEX_FLASH_FLAGS += -R .webtable

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

## Objects that must be built in order to link
OBJECTS = webdata.o fonts.o objects.o RenderLines.o gcrt1.o sdSimple.o uzeboxVideoEngineCore.o uzeboxCore.o uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o housekeeping.o levels.o highscores.o $(GAME).o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" 

## Build
all: $(TARGET) $(GAME).hex $(GAME).eep $(GAME).lss size

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

sdSimple.o: $(KERNEL_DIR)/sdSimple.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

gcrt1.o: ../gcrt1.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

RenderLines.o: ../RenderLines.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

objects.o: ../objects.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

fonts.o: ../fonts.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

webdata.o: ../webdata.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

housekeeping.o: ../housekeeping.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

levels.o: ../levels.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

highscores.o: ../highscores.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<


## Compile game sources
$(GAME).o: ../t2k.c
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

##UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
AVRSIZEFLAGS := -A ${TARGET}
##ifneq (,$(findstring MINGW,$(UNAME)))
##AVRSIZEFLAGS := -C --mcu=${MCU} ${TARGET}
##endif

size: ${TARGET}
	@echo
	@avr-size ${AVRSIZEFLAGS}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS)
	-rm -rf $(GAME).hex
	-rm -rf $(GAME).elf
	-rm -rf $(GAME).eep
	-rm -rf dep/*
	-rm -rf *.uze


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)