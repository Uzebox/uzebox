CC=gcc
BIN_DIR=../../bin
MKDIR=mkdir -p

dconvert: dconvert.o
	@$(MKDIR) $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/dconvert dconvert.o

dconvert.o : dconvert.c
	$(CC) -c dconvert.c

clean :
	rm -f $(BIN_DIR)/dconvert dconvert.o
