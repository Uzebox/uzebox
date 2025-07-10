###############################################################################
# Makefile for the project TinyBASIC
###############################################################################

## General Flags
PROJECT = Basic
GAME= Basic
MCU = atmega644
TARGET = $(GAME).elf
CC = avr-gcc
INFO=../gameinfo.properties
UZEBIN_DIR = ../../../bin

## Kernel settings
KERNEL_DIR = ../../../kernel

## Escape spaces in mixer path (due to custom sounds.inc)
EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)
SPACE_ESC:= \\$(SPACE)
MIX_PATH:= $(realpath ../data/sounds.inc)
MIX_PATH_ESC:= $(subst $(SPACE),$(SPACE_ESC),$(MIX_PATH))

## Kernel settings
KERNEL_DIR = ../../../kernel

KERNEL_OPTIONS  = -DVIDEO_MODE=0 -DVIDEO_MODE_PATH=$(realpath ../customVideoMode80)
KERNEL_OPTIONS += -DSCREEN_TILES_H=80 -DSCREEN_TILES_V=25 -DFIRST_RENDER_LINE=28 

#KERNEL_OPTIONS  = -DVIDEO_MODE=5 -DINTRO_LOGO=0 -DFONT_TILE_INDEX=0 

KERNEL_OPTIONS += -DSOUND_MIXER=1 -DTRUE_RANDOM_GEN=1 -D_FS_READONLY=0 -DUART=0 -DUART_RX_BUFFER_SIZE=16 -DUART_TX_BUFFER_SIZE=16 #-D_WORD_ACCESS=0
KERNEL_OPTIONS += -DNO_EEPROM_FORMAT=1 -DNO_PC_SLIDE=1 -DNO_PC_LOOP=1 -DNO_CHAN_EXPRESSION=1
KERNEL_OPTIONS += -DMUSIC_ENGINE=STREAM -DSOUND_CHANNEL_5_ENABLE=0 #-DMIXER_WAVES=\"$(MIX_PATH_ESC)\"
#KERNEL_OPTION += -DJTAG_DEBUG
## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)

#CFLAGS += -Wall -Wextra -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -O3 -fsigned-char -ffunction-sections -fno-toplevel-reorder 
CFLAGS += -Wall -Wextra -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Og -fsigned-char -ffunction-sections -fno-toplevel-reorder -fno-omit-frame-pointer -fno-inline -fno-optimize-sibling-calls

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
## Required to include the correct math lib and print floats with printf
LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = uzeboxVideoEngineCore.o uzeboxCore.o uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o keyboard.o terminal.o spiram.o mmc.o ff.o $(GAME).o
#OBJECTS += spiram.o mmc.o ff.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" 

## Build
all: ../data/font6x8-full.inc $(TARGET) $(GAME).hex $(GAME).eep $(GAME).lss $(GAME).uze size

## Regenerate the graphics include file
../data/font6x8-full.inc: ../data/font-6x8-full.png ../data/gconvert.xml
	$(UZEBIN_DIR)/gconvert ../data/gconvert.xml

## Compile Kernel files
mmc.o: $(KERNEL_DIR)/fatfs/mmc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ff.o: $(KERNEL_DIR)/fatfs/ff.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

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

keyboard.o: $(KERNEL_DIR)/keyboard.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spiram.o: $(KERNEL_DIR)/spiram.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

terminal.o: ../terminal.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

$(GAME).o: ../basic.c
#$(GAME).o: ../tokenizer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $< -o $(GAME).o
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

## Conditional setting based on whether avr-size supports -C
ifeq ($(shell avr-size --help | grep '\-C' >/dev/null && echo yes ),yes)
AVRSIZEFLAGS := -C --mcu=${MCU} ${TARGET}
else
AVRSIZEFLAGS := -A ${TARGET}
endif

size: ${TARGET}
	@echo
	@avr-size ${AVRSIZEFLAGS}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(GAME).* dep/* *.uze *.hex  ../data/font6x8-full.inc


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

