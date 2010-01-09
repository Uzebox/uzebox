###############################################################################
# Uzebox project makefile
###############################################################################

TARGETNAME = unittest

# the AVR programmer for use with "avrdude"
PROGRAMMER = usbtiny 

#  uzebox defines for avr-gcc
UZEBOXFLAGS += -DUZEBOX -DMIXER_CHAN4_TYPE=1 -DINCLUDE_DEFAULT_WAVES=1 -DVIDEO_MODE=1
 
# relative path to kernel source
KERNELROOT = ../kernel

# Objects that must be built in order to link
ASMOBJECTS = $(KERNELROOT)/uzeboxVideoEngineCore.o $(KERNELROOT)/uzeboxSoundEngineCore.o testasm.o
COBJECTS = $(KERNELROOT)/uzeboxSoundEngine.o $(KERNELROOT)/uzeboxVideoEngine.o $(KERNELROOT)/uzeboxCore.o 
COBJECTS += $(KERNELROOT)/mmc_if.o $(KERNELROOT)/fat.o 
COBJECTS += unittest.o gfx.o
OBJECTS = $(ASMOBJECTS) $(COBJECTS)
GENERATEDCODE = gfx.c

GIMP = /Progra~1/GIMP-2.0/bin/gimp-console-2.6.exe
###############################################################################

## General Flags
MCU = atmega644
TARGET = $(TARGETNAME).elf
CC = avr-gcc -O2

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += $(UZEBOXFLAGS)
CFLAGS +=  -DF_CPU=28636360UL
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -Os -mcall-prologues -fsigned-char 
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d
CFLAGS += -I$(KERNELROOT) -I./

GFLAGS = --no-data --no-fonts --no-interface

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=$(TARGETNAME).map

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

gfx.png: data/gfx.xcf
	$(GIMP) $(GFLAGS) -b "(sg-save-layer-cmd \"data/gfx.xcf\" \"gfx\")" -b "(gimp-quit 0)"
    
## Compile  

$(GENERATEDCODE): %.c: %.png
	python convertgfx.py -6 -r16 $<
    
$(COBJECTS): %.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $< -o $@

$(ASMOBJECTS): %.o: %.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $< -o $@

##Link
$(TARGET): $(OBJECTS)
	echo $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@
    
emulator:
	make -C .. emulator
    
size:
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}
    
## Build
all: $(TARGET) $(TARGETNAME).hex $(TARGETNAME).eep $(TARGETNAME).lss size

.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(ASMOBJECTS) $(HIREZCODE) *.png
	-rm -rf $(TARGETNAME).elf $(TARGETNAME).hex $(TARGETNAME).eep $(TARGETNAME).lss $(TARGETNAME).map 

run: clean all emulator
	../uzem --sd g --eeprom eeprom.dat $(TARGETNAME).hex 
    
install: all
	avrdude -c $(PROGRAMMER) -p m644 -U flash:w:$(TARGETNAME).hex

commit: clean
	svn commit
    
## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)
