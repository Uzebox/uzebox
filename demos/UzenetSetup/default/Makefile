###############################################################################
# Makefile for the project
###############################################################################

## General Flags
PROJECT = UzenetSetup
GAME= UzenetSetup
MCU = atmega644
TARGET = $(GAME).elf
CC = avr-gcc
INFO=../gameinfo.properties

## Escape spaces in mixer path (due to custom sounds.inc)
EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)
SPACE_ESC:= \\$(SPACE)
#MIX_PATH:= $(realpath ../data/sounds.inc)
MIX_PATH_ESC:= $(subst $(SPACE),$(SPACE_ESC),$(MIX_PATH))
VPATH:= $(realpath ../)
VPATH_ESC:= $(subst $(SPACE),$(SPACE_ESC),$(VPATH))

## Kernel settings
KERNEL_DIR = ../../../kernel
KERNEL_OPTIONS  = -DVIDEO_MODE=0 -DVIDEO_MODE_PATH=$(realpath ../customVideoMode80) 
KERNEL_OPTIONS += -DSCREEN_TILES_H=80 -DSCREEN_TILES_V=25 -DFIRST_RENDER_LINE=28 -DFONT_TILE_INDEX=0  -DSOUND_MIXER=0 -DENABLE_MIXER=0
#KERNEL_OPTIONS += -DMIXER_WAVES=\"$(MIX_PATH_ESC)\"  -DSOUND_MIXER=1  -DSOUND_CHANNEL_5_ENABLE=0 -DENABLE_MIXER=0
KERNEL_OPTIONS += -DUART_RX_BUFFER_SIZE=256 -DUART_TX_BUFFER_SIZE=128 -DUART=1 -DWIFI_DEBUG=0

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" -I"..\customVideoMode80" -I"..\..\UzeboxPC" 

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os -fsigned-char -ffunction-sections -fno-toplevel-reorder
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
OBJECTS  = main.o uzeboxVideoEngineCore.o uzeboxCore.o  uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o uzenet_setup.o
OBJECTS += uzenet.o terminal.o keyboard.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all:  $(TARGET) $(GAME).hex $(GAME).eep $(GAME).lss $(GAME).uze size


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

keyboard.o: $(KERNEL_DIR)/keyboard.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
	
uzenet.o: $(KERNEL_DIR)/uzenet.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

terminal.o: ../../UzenetDemo/terminal.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzenet_setup.o: ../uzenet_setup.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
		
## Compile game sources
main.o: ../main.c
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
	-rm -rf $(OBJECTS) $(GAME).* 
	-rm -rf dep


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

