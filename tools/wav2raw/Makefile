CC      = gcc
BIN_DIR = ../../bin
MKDIR   = mkdir -p

CFLAGS ?= -O2 -std=c11 -Wall -Wextra
LDLIBS ?= -lm

.PHONY: all clean

all: wav2raw

wav2raw: wav2raw.o | $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/wav2raw wav2raw.o $(LDLIBS)

$(BIN_DIR):
	@$(MKDIR) $(BIN_DIR)

wav2raw.o: wav2raw.c
	$(CC) $(CFLAGS) -c wav2raw.c

clean:
	rm -f $(BIN_DIR)/wav2raw wav2raw.o
