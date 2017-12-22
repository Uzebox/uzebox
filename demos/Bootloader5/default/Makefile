###############################################################################
# Makefile for Bootloader
###############################################################################

## General Flags
PROJECT = Bootloader5
GAME    = $(PROJECT)
VERSION = 5009
INFO    = gameinfo.properties
MCU     = atmega644
TARGET  = $(GAME).elf
CC      = avr-gcc
OBJDIR  = obj
DEPDIR  = dep
DIRS    = $(OBJDIR) $(DEPDIR)

## Kernel settings
KERNEL_DIR = ../kernel
KERNEL_OPTIONS  = -DVIDEO_MODE=0
KERNEL_OPTIONS += -DINTRO_LOGO=0
KERNEL_OPTIONS += -DTILESET_SIZE=97
KERNEL_OPTIONS += -DVRAM_TILES_H=38
KERNEL_OPTIONS += -DVRAM_TILES_V=24
KERNEL_OPTIONS += -DP2_DISABLE=1


## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS  = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -O2 -fsigned-char
CFLAGS += -ffunction-sections -fno-toplevel-reorder -fno-tree-switch-conversion
CFLAGS += -MD -MP -MT $(*F).o -MF $(DEPDIR)/$(@F).d
CFLAGS += $(KERNEL_OPTIONS)
CFLAGS += -DVERSION=0x$(VERSION)


## Assembly specific flags
ASMFLAGS  = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS  = $(COMMON)
LDFLAGS += -Wl,-Map=$(GAME).map
LDFLAGS += -Wl,-gc-sections
LDFLAGS += -Wl,--section-start=.text=0xF000
LDFLAGS += -Wl,--section-start=.tile=0xFDAA
LDFLAGS += -Wl,--section-start=.prog=0xFF90
LDFLAGS += -nostdlib

## Intel Hex file production flags
HEX_FLASH_FLAGS  = -R .eeprom
HEX_FLASH_FLAGS += --set-section-flags .tile=alloc,load
HEX_FLASH_FLAGS += --set-section-flags .prog=alloc,load,code


## Objects that must be built in order to link
OBJECTS  = $(OBJDIR)/uzeboxVideoEngineCore.o
OBJECTS += $(OBJDIR)/uzeboxCore.o
OBJECTS += $(OBJDIR)/uzeboxSoundEngineCore.o
OBJECTS += $(OBJDIR)/sdlib.o
OBJECTS += $(OBJDIR)/dir.o
OBJECTS += $(OBJDIR)/proguze.o
OBJECTS += $(OBJDIR)/graphics.o
OBJECTS += $(OBJDIR)/res.o
OBJECTS += $(OBJDIR)/boot.o
OBJECTS += $(OBJDIR)/game.o
OBJECTS += $(OBJDIR)/tileset.o
OBJECTS += $(OBJDIR)/prog.o

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)"

## Build
all: $(TARGET) $(GAME).hex $(GAME).lss size

## Directories
$(OBJDIR):
	mkdir $(OBJDIR)

$(DEPDIR):
	mkdir $(DEPDIR)

## Compile Kernel files
$(OBJDIR)/uzeboxVideoEngineCore.o: $(KERNEL_DIR)/uzeboxVideoEngineCore.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/uzeboxSoundEngineCore.o: $(KERNEL_DIR)/uzeboxSoundEngineCore.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/uzeboxCore.o: $(KERNEL_DIR)/uzeboxCore.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

## Compile game sources
$(OBJDIR)/sdlib.o: ../sdlib.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/dir.o: ../dir.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/proguze.o: ../proguze.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/graphics.o: ../graphics.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/res.o: ../res.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/boot.o: ../boot.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/game.o: ../game.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/tileset.o: ../tileset.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/prog.o: ../prog.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@


##Link
$(TARGET): $(OBJECTS) $(DIRS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS) $< $@

%.lss: $(TARGET)
	avr-objdump -h -D -S $< > $@

UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
AVRSIZEFLAGS := -A ${TARGET}
ifneq (,$(findstring MINGW,$(UNAME)))
AVRSIZEFLAGS := -C --mcu=${MCU} ${TARGET}
endif

size: ${TARGET}
	@echo
	@avr-size ${AVRSIZEFLAGS}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(DIRS)
	-rm -f  $(GAME).*

