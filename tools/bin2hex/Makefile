CC=gcc
BIN_DIR=../../bin
MKDIR=mkdir -p

bin2hex: bin2hex.o
	@$(MKDIR) $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/bin2hex bin2hex.o

bin2hex.o : bin2hex.c
	$(CC) -std=gnu99 -c bin2hex.c

clean :
	rm -f $(BIN_DIR)/bin2hex bin2hex.o
