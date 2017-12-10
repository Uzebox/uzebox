###############################################################################
# Makefile for the project M40Demo
###############################################################################

## General Flags
PROJECT = m41demo
GAME    = m41demo
INFO    = gameinfo.properties
MCU     = atmega644
TARGET  = $(GAME).elf
CC      = avr-gcc
OUTDIR  = _bin_
OBJDIR  = _obj_
DEPDIR  = _dep_
DIRS    = $(OUTDIR) $(OBJDIR) $(DEPDIR)

## Kernel settings
KERNEL_DIR = ../../../../kernel
KERNEL_OPTIONS  = -DVIDEO_MODE=41
KERNEL_OPTIONS += -DSOUND_MIXER=MIXER_TYPE_INLINE
#KERNEL_OPTIONS += -DM40_MATTEL=1
KERNEL_OPTIONS += -DM40_C64_GRAPHICS=1
#KERNEL_OPTIONS += -DM40_C64_ALPHA=1
#KERNEL_OPTIONS += -DM40_C64_MIXED=1
#KERNEL_OPTIONS += -DM40_IBM_ASCII=1
#KERNEL_OPTIONS += -DVRAM_TILES_H=32
#KERNEL_OPTIONS += -DVRAM_TILES_V=24


## Packrom (.uze)
PACKROM_DIR = ../../../../tools/packrom


## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS  = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os -fsigned-char
CFLAGS += -ffunction-sections -fno-toplevel-reorder -fno-tree-switch-conversion
CFLAGS += -MD -MP -MT $(*F).o -MF $(DEPDIR)/$(@F).d
CFLAGS += $(KERNEL_OPTIONS)


## Assembly specific flags
ASMFLAGS  = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS  = $(COMMON)
LDFLAGS += -Wl,-Map=$(OUTDIR)/$(GAME).map
LDFLAGS += -Wl,-gc-sections

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom


## Objects that must be built in order to link
OBJECTS  = $(OBJDIR)/uzeboxVideoEngineCore.o
OBJECTS += $(OBJDIR)/uzeboxCore.o
OBJECTS += $(OBJDIR)/uzeboxSoundEngine.o
OBJECTS += $(OBJDIR)/uzeboxSoundEngineCore.o
OBJECTS += $(OBJDIR)/uzeboxVideoEngine.o
OBJECTS += $(OBJDIR)/$(GAME).o

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)"

## Build
all: $(OUTDIR)/$(TARGET) $(OUTDIR)/$(GAME).hex $(OUTDIR)/$(GAME).lss $(OUTDIR)/$(GAME).uze size

## Directories
$(OBJDIR):
	mkdir $(OBJDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

$(DEPDIR):
	mkdir $(DEPDIR)

## Compile Kernel files
$(OBJDIR)/uzeboxVideoEngineCore.o: $(KERNEL_DIR)/uzeboxVideoEngineCore.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/uzeboxSoundEngineCore.o: $(KERNEL_DIR)/uzeboxSoundEngineCore.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/uzeboxCore.o: $(KERNEL_DIR)/uzeboxCore.c $(DIRS)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(OBJDIR)/uzeboxSoundEngine.o: $(KERNEL_DIR)/uzeboxSoundEngine.c $(DIRS)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(OBJDIR)/uzeboxVideoEngine.o: $(KERNEL_DIR)/uzeboxVideoEngine.c $(DIRS)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

## Compile game sources
$(OBJDIR)/$(GAME).o: main.c $(DIRS)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

##Link
$(OUTDIR)/$(TARGET): $(OBJECTS) $(DIRS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LIBDIRS) $(LIBS) -o $(OUTDIR)/$(TARGET)

$(OUTDIR)/%.hex: $(OUTDIR)/$(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS) $< $@

$(OUTDIR)/%.lss: $(OUTDIR)/$(TARGET)
	avr-objdump -h -S $< > $@

$(OUTDIR)/%.uze: $(OUTDIR)/$(TARGET)
	-$(PACKROM_DIR)/packrom $(OUTDIR)/$(GAME).hex $@ $(INFO)

UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
AVRSIZEFLAGS := -A $(OUTDIR)/${TARGET}
ifneq (,$(findstring MINGW,$(UNAME)))
AVRSIZEFLAGS := -C --mcu=${MCU} $(OUTDIR)/${TARGET}
endif

size: $(OUTDIR)/${TARGET}
	@echo
	@avr-size ${AVRSIZEFLAGS}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(DIRS)

