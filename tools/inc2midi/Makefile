CC      = gcc
BIN_DIR = ../../bin
MKDIR   = mkdir -p

CFLAGS ?= -O2 -std=c11 -Wall -Wextra
LDLIBS ?= -lm

.PHONY: all clean

all: inc2midi

inc2midi: inc2midi.o | $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/inc2midi inc2midi.o $(LDLIBS)

$(BIN_DIR):
	@$(MKDIR) $(BIN_DIR)

inc2midi.o: inc2midi.c
	$(CC) $(CFLAGS) -c inc2midi.c

clean:
	rm -f $(BIN_DIR)/inc2midi inc2midi.o
