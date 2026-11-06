# Makefile for the Uzebox Mode 748 Pong tutorial
GAME       = pong
MCU        = atmega644
# Where is the Uzebox kernel? make looks in the usual places next to or around
# this folder; if yours is elsewhere, run:  make UZEBOX=/path/to/uzebox
KERNEL_DIR ?= $(if $(UZEBOX),$(UZEBOX)/kernel,$(patsubst %/uzebox.h,%,$(firstword \
              $(wildcard ../../kernel/uzebox.h ../../../kernel/uzebox.h \
                         ../uzebox/kernel/uzebox.h ../../uzebox/kernel/uzebox.h))))
ifeq ($(wildcard $(KERNEL_DIR)/uzebox.h)$(filter clean,$(MAKECMDGOALS)),)
$(error Uzebox kernel not found. Run: make UZEBOX=/path/to/uzebox)
endif

CC      = avr-gcc
OPTIONS = -DVIDEO_MODE=748 -DINTRO_LOGO=0 -DSOUND_MIXER=MIXER_TYPE_INLINE
CFLAGS  = -mmcu=$(MCU) -Wall -std=gnu99 -Os -DF_CPU=28636360UL -fsigned-char \
          -fno-toplevel-reorder $(OPTIONS) -include m74cfg.h -I$(KERNEL_DIR) -I.
ASFLAGS = $(CFLAGS) -x assembler-with-cpp
LDFLAGS = -mmcu=$(MCU) -Wl,--section-start=.rectb=0xE000

KERNEL  = uzeboxVideoEngineCore uzeboxCore uzeboxSoundEngine \
          uzeboxSoundEngineCore uzeboxVideoEngine spiram
OBJECTS = $(addsuffix .o,$(KERNEL)) main.o

all: $(GAME).hex

%.o: $(KERNEL_DIR)/%.s
	$(CC) $(ASFLAGS) -c $< -o $@
%.o: $(KERNEL_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
main.o: main.c m74cfg.h
	$(CC) $(CFLAGS) -c $< -o $@

$(GAME).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
$(GAME).hex: $(GAME).elf
	avr-objcopy -O ihex -R .eeprom $< $@
	avr-size $<

clean:
	rm -f *.o *.elf *.hex
