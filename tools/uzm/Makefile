###############################################################################
# uzm -- Uzeamp's video files (.UZM): the encoder and its tools. C and FFmpeg's
# libraries (libavformat, libavcodec, libavfilter, libavutil), found by pkg-config.
#
#   make                build ./uzm (uzm.exe on Windows)
#   make check          the test pattern at every rate, written and checked
#   make dist           Windows (MSYS2): dist/ with uzm.exe and the DLLs it needs
#   make install        to $(PREFIX)/bin (default /usr/local)
#   make clean
#
# Windows: an MSYS2 UCRT64 (or MINGW64, CLANG64) shell, with
#     pacman -S make mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ffmpeg
# Linux (Debian, Ubuntu):
#     apt install build-essential pkg-config libavformat-dev libavcodec-dev \
#                 libavfilter-dev libavutil-dev
# macOS (Homebrew): xcode-select --install; brew install pkg-config ffmpeg
###############################################################################

PROG     := uzm
SRCS     := src/main.c src/encode.c src/cadence.c src/media.c src/format.c src/dither.c \
            src/png.c src/util.c
OBJS     := $(SRCS:.c=.o)
PREFIX   ?= /usr/local
PKG_CONFIG ?= pkg-config
FFMPEG   := libavformat libavcodec libavfilter libavutil

ifeq ($(OS),Windows_NT)
    EXE := .exe
    # MSYS2: its pkg-config may not search the environment's own prefix
    ifneq ($(MSYSTEM),)
        MSYS_PREFIX := /$(shell echo $(MSYSTEM) | tr A-Z a-z)
        ifneq ($(shell $(PKG_CONFIG) --exists libavformat && echo y),y)
            export PKG_CONFIG_PATH := $(MSYS_PREFIX)/lib/pkgconfig:$(PKG_CONFIG_PATH)
        endif
    endif
    # wmain: the arguments as UTF-8, whatever the console's code page
    LDFLAGS += -municode
endif

ifneq ($(shell $(PKG_CONFIG) --exists $(FFMPEG) && echo y),y)
    $(error FFmpeg's libraries not found by $(PKG_CONFIG): see the Makefile's top for the packages)
endif

# -ffp-contract=off: no fused multiply-adds (clang's default on Apple silicon), so the dither's
# doubles round as uzm.py's do and every system writes the same bytes
CFLAGS   ?= -O2
CFLAGS   += -std=gnu11 -Wall -Wextra -Wno-unused-parameter -ffp-contract=off
CPPFLAGS += $(shell $(PKG_CONFIG) --cflags $(FFMPEG))
LDLIBS   += $(shell $(PKG_CONFIG) --libs $(FFMPEG)) -lm

TARGET   := $(PROG)$(EXE)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

src/%.o: src/%.c src/uzm.h src/media.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

CHECK_DIR := check.tmp
check: $(TARGET)
	@rm -rf $(CHECK_DIR) && mkdir $(CHECK_DIR)
	@for r in 60 30 25 24; do \
		./$(TARGET) test $(CHECK_DIR)/T$$r.UZM 3 --rate $$r > /dev/null && \
		./$(TARGET) info $(CHECK_DIR)/T$$r.UZM | tail -1 && \
		./$(TARGET) sound $(CHECK_DIR)/T$$r.UZM $(CHECK_DIR)/T$$r.wav > /dev/null && \
		./$(TARGET) frame $(CHECK_DIR)/T$$r.UZM 10 $(CHECK_DIR)/T$$r.png > /dev/null || exit 1; \
	done
	@./$(TARGET) test $(CHECK_DIR)/T60v1.UZM 3 --v1 > /dev/null && ./$(TARGET) info $(CHECK_DIR)/T60v1.UZM | tail -1
	@rm -rf $(CHECK_DIR)
	@echo "check: all good"

# Windows: the program and the DLLs it loads from the MSYS2 environment, to copy anywhere
dist: $(TARGET)
ifeq ($(OS),Windows_NT)
	@rm -rf dist && mkdir dist && cp $(TARGET) dist/
	@ldd $(TARGET) | awk '{print $$3}' | grep -i '^$(MSYS_PREFIX)/' | sort -u | xargs -I{} cp {} dist/
	@echo "dist/: $$(ls dist | wc -l) files, $$(du -sh dist | cut -f1)"
else
	@echo "make dist is for Windows: elsewhere FFmpeg comes with the system's packages"
endif

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -rf $(OBJS) $(TARGET) dist $(CHECK_DIR)

.PHONY: all check dist install clean
