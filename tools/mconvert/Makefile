CC=gcc
BIN_DIR=../../bin
MKDIR=mkdir -p

mconvert: mconvert.o
	@$(MKDIR) $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/mconvert mconvert.o

mconvert.o : mconvert.c
	$(CC) -c mconvert.c

clean :
	rm -f $(BIN_DIR)/mconvert mconvert.o
