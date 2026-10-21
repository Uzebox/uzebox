.DEFAULT_GOAL := all
HOSTCXX ?= c++
HOSTCXXFLAGS ?= -O2 -Wall -Wextra -std=c++11
AVR_PREFIX ?= avr-
KERNEL_DIR ?= ../../../kernel
BOOTLOADER_DIR ?= ../../../demos/Bootloader5/default
PAYLOAD ?= $(BOOTLOADER_DIR)/Bootloader5.hex
EXEEXT :=
ifeq ($(OS),Windows_NT)
EXEEXT := .exe
endif
PACK := pack$(EXEEXT)

CPPFLAGS += -I.. -I. -I$(KERNEL_DIR) -DF_CPU=28636360UL -DVIDEO_MODE=5 -DINTRO_LOGO=0
CPPFLAGS += -DSOUND_MIXER=1 -DSOUND_CHANNEL_4_ENABLE=0 -DSOUND_CHANNEL_5_ENABLE=0
CPPFLAGS += -DNO_EEPROM_FORMAT=1 -DTRUE_RANDOM_GEN=0
CPPFLAGS += -I$(KERNEL_DIR)/petitfatfs -DPF_USE_LSEEK=1 -DPF_USE_WRITE=0
CFLAGS = -mmcu=atmega644 -Os -std=gnu99 -ffunction-sections -fdata-sections
CFLAGS += -fno-toplevel-reorder -fsigned-char -Wall -g -MMD -MP
OBJECTS = uzeboxVideoEngineCore uzeboxCore uzeboxSoundEngine uzeboxSoundEngineCore
OBJECTS += uzeboxVideoEngine main bootjacker hijack payload source pff diskio
WRITE_OBJECTS = $(addprefix bjtov5_,$(addsuffix .o,$(OBJECTS)))
vpath %.c .. $(KERNEL_DIR) $(KERNEL_DIR)/petitfatfs
vpath %.s .. $(KERNEL_DIR)

.PHONY: all clean FORCE
.DELETE_ON_ERROR:
all: BJTOV5.UZE

$(PACK): ../pack.cpp
	$(HOSTCXX) $(HOSTCXXFLAGS) $< -o $@

# Recheck the selected payload; unchanged generated files keep their timestamps.
payload_meta.h: $(PAYLOAD) $(PACK) FORCE
	./$(PACK) payload "$(PAYLOAD)"
payload.c: payload_meta.h
	@test -f "$@"

$(BOOTLOADER_DIR)/Bootloader5.hex:
	$(MAKE) -C "$(BOOTLOADER_DIR)" Bootloader5.hex
	@test -f "$@" || { echo "ERROR: Bootloader5.hex is missing after building Bootloader5."; exit 1; }
ifneq ($(PAYLOAD),$(BOOTLOADER_DIR)/Bootloader5.hex)
$(PAYLOAD):
	@echo "ERROR: payload $@ is missing; build it first or select an existing HEX."
	@exit 1
endif

bjtov5_%.o: %.c payload_meta.h
	$(AVR_PREFIX)gcc $(CPPFLAGS) $(CFLAGS) -c $< -o $@
bjtov5_%.o: %.s payload_meta.h
	$(AVR_PREFIX)gcc $(CPPFLAGS) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

BJTOV5.elf: $(WRITE_OBJECTS)
	$(AVR_PREFIX)gcc -mmcu=atmega644 -Wl,--gc-sections -Wl,-Map=BJTOV5.map $^ -o $@
	$(AVR_PREFIX)size -C --mcu=atmega644 $@
%.hex: %.elf
	$(AVR_PREFIX)objcopy -O ihex -R .eeprom $< $@
BJTOV5.UZE: BJTOV5.hex $(PACK)
	./$(PACK) writer $< $@

clean:
	$(RM) *.o *.d *.elf *.hex *.map *.lss *.UZE payload.c payload_meta.h manifest.json pack pack.exe

-include $(wildcard *.d)
