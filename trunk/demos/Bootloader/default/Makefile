###############################################################################
# Makefile for the project Bootloader
###############################################################################

## General Flags
PROJECT = Bootloader
MCU = atmega644
TARGET = Bootloader.elf
CC = avr-gcc
#BOOTLOAD_ADDRESS = 0
BOOTLOAD_ADDRESS = 0xF000

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)
KERNEL_DIR = ../../../kernel

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os -fsigned-char -DBOOTLOADER_ADDR=$(BOOTLOAD_ADDRESS) -ffunction-sections
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=Bootloader.map
LDFLAGS += -Wl,--section-start=.text=$(BOOTLOAD_ADDRESS) 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = bootloaderCore.o bootloader.o mmc.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) Bootloader.hex Bootloader.eep Bootloader.lss size

## Compile
bootloaderCore.o: ../bootloaderCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

bootloader.o: ../bootloader.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mmc.o: $(KERNEL_DIR)/mmc.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

##fat.o: ../fat.s
##	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) Bootloader.* dep/* 


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

