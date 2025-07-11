###############################################################################
# Makefile for the project
###############################################################################

## General Flags
PROJECT = UzeboxPC
GAME= UzeboxPC
MCU = atmega644
TARGET = $(GAME).elf
CC = avr-gcc
INFO=../gameinfo.properties
UZEBIN_DIR=../../../bin/

## Escape spaces in mixer path (due to custom sounds.inc)
EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)
SPACE_ESC:= \\$(SPACE)
MIX_PATH:= $(realpath ../data/sounds.inc)
MIX_PATH_ESC:= $(subst $(SPACE),$(SPACE_ESC),$(MIX_PATH))
VPATH:= $(realpath ../)
VPATH_ESC:= $(subst $(SPACE),$(SPACE_ESC),$(VPATH))

## Kernel settings
KERNEL_DIR = ../../../kernel
KERNEL_OPTIONS  = -DVIDEO_MODE=0 -DVIDEO_MODE_PATH=$(realpath ../customVideoMode80) 
KERNEL_OPTIONS += -DSCREEN_TILES_H=80 -DSCREEN_TILES_V=25 -DFIRST_RENDER_LINE=28 -DFONT_TILE_INDEX=0
KERNEL_OPTIONS += -DMIXER_WAVES=\"$(MIX_PATH_ESC)\"  -DSOUND_MIXER=1  -DSOUND_CHANNEL_5_ENABLE=0 -DENABLE_MIXER=0
KERNEL_OPTIONS += -DUART_RX_BUFFER_SIZE=256 -DUART_TX_BUFFER_SIZE=128 -DUART=1 
KERNEL_OPTIONS += -DPF_USE_DIR=0 -DPF_USE_LSEEK=1 -DPF_USE_WRITE=1 -DPF_USE_READ=1

## Specify this option if using a JTAG ICE. This will set screen colors differently because some video output pins are disabled.
#KERNEL_OPTIONS += -DJTAG_DEBUG=1

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" -I"..\customVideoMode80" 

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
OBJECTS  = main.o uzeboxVideoEngineCore.o uzeboxCore.o  uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o  
OBJECTS += uzenet.o terminal.o keyboard.o telnet.o Emulator8080.o computer.o spiram_custom.o pff.o diskio.o uzenet_setup.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build the ROM
## To rebuild cpm ressources (currently only works on windows):  make cpm
all: $(TARGET) $(GAME).hex $(GAME).eep $(GAME).lss $(GAME).uze size

cpm: ../data/cpm22.inc ../data/cpmbin/TELNET.COM ../data/cpmbin/UZECONF.COM

## Compile CP/M resources (currently only works on windows)
../data/cpm22.inc: ../data/cpmsrc/cpm22.asm
	-./build-cpm.bat

../data/cpmbin/TELNET.COM: ../data/cpmsrc/telnet.asm ../data/cpmbin/TELNET.DAT
	-./build-telnet.bat
	
../data/cpmbin/UZECONF.COM: ../data/cpmsrc/uzeconf.asm
	-./build-uzeconf.bat

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

uzenet.o: ../uzenet.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

terminal.o: ../terminal.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

telnet.o: ../telnet.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

computer.o: ../computer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzenet_setup.o: ../uzenet_setup.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
	
Emulator8080.o: ../Emulator8080.S
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

spiram_custom.o: ../spiram_custom.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<
	
diskio.o: $(KERNEL_DIR)/petitfatfs/diskio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
	
pff.o: $(KERNEL_DIR)/petitfatfs/pff.c
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

## Conditional setting based on whether avr-size supports -C
ifeq ($(shell avr-size --help | grep '\-C' >/dev/null && echo yes ),yes)
AVRSIZEFLAGS := -C --mcu=${MCU} ${TARGET}
else
AVRSIZEFLAGS := -A ${TARGET}
endif

size: ${TARGET}
	@echo
	@avr-size ${AVRSIZEFLAGS}
	@avr-gcc --version 
	
## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(GAME).* *.bin *.lst *.p *.o *.com *.COM *.hex
##	-rm -rf ../data/cpm22.inc ../data/cpmbin/TELNET.COM ../data/cpmbin/UZECONF.COM 
	-rm -rf dep


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

