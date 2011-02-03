###############################################################################
# Makefile for the project Bootloader
###############################################################################

## General Flags
PROJECT = Bootloader_Pragma
MCU = atmega644
TARGET = ${PROJECT}.elf
CC = avr-gcc
#BOOTLOAD_ADDRESS = 0
BOOTLOAD_ADDRESS = 0xF000

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os -fsigned-char -DBOOTLOADER_ADDR=$(BOOTLOAD_ADDRESS)
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=${PROJECT}.map
LDFLAGS += -Wl,--section-start=.text=$(BOOTLOAD_ADDRESS) 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = bootloaderCore.o bootloader.o mmc.o math.o fat.o menu.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) ${PROJECT}.hex ${PROJECT}.eep ${PROJECT}.lss size

## Compile
bootloaderCore.o: ../bootloaderCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

bootloader.o: ../bootloader.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

mmc.o: ../mmc.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

math.o: ../math.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

fat.o: ../fat.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

menu.o: ../menu.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<


##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

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
	-rm -rf $(OBJECTS) ${PROJECT}.* dep/*


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

