###############################################################################
# uzm -- Uzeamp's video files (.UZM): the encoder and its tools. C and FFmpeg's
# libraries (libavformat, libavcodec, libavfilter, libavutil, libswscale,
# libswresample), found by pkg-config.
#
#   make                ../../bin/uzm (uzm.exe on Windows), like the other tools (DEST_DIR=dir:
#                       there instead), with the system's FFmpeg libraries
#   make dist           dist/uzm(.exe): one standalone program, FFmpeg built here from
#                       source, trimmed to reading video, and linked in (Windows: no DLL).
#                       Windows: copied to bin/ too, and from then on `make` builds this one
#   make check          the test pattern at every rate, written and checked
#   make install        uzm to $(PREFIX)/bin (default /usr/local)
#   make clean          (make distclean: FFmpeg's build too)
#
# Windows: an MSYS2 UCRT64 (or MINGW64, CLANG64) shell, with
#     pacman -S make mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ffmpeg
#     (make dist: also mingw-w64-ucrt-x86_64-nasm mingw-w64-ucrt-x86_64-dav1d)
# Linux (Debian, Ubuntu):
#     apt install build-essential pkg-config libavformat-dev libavcodec-dev \
#                 libavfilter-dev libavutil-dev          (make dist: nasm libdav1d-dev)
# macOS (Homebrew): xcode-select --install; brew install pkg-config ffmpeg
#                                                        (make dist: nasm dav1d)
###############################################################################

PROG     := uzm
SRCS     := src/main.c src/encode.c src/cadence.c src/media.c src/format.c src/dither.c \
            src/png.c src/util.c
PREFIX   ?= /usr/local
PKG_CONFIG ?= pkg-config
FFMPEG   := libavformat libavcodec libavfilter libavutil
UNAME    := $(shell uname -s)
# where the tools go: the repository's bin/, or DEST_DIR (the root Makefile passes it)
BIN_DIR  ?= ../../bin
ifneq ($(DEST_DIR),)
    BIN_DIR := $(DEST_DIR)
endif

ifeq ($(OS),Windows_NT)
    EXE := .exe
    # MSYS2: its pkg-config may not search the environment's own prefix
    ifneq ($(MSYSTEM),)
        MSYS_PREFIX := /$(shell echo $(MSYSTEM) | tr A-Z a-z)
        ifneq ($(shell $(PKG_CONFIG) --exists zlib 2>/dev/null && echo y),y)
            export PKG_CONFIG_PATH := $(MSYS_PREFIX)/lib/pkgconfig:$(PKG_CONFIG_PATH)
        endif
    endif
    # wmain: the arguments as UTF-8, whatever the console's code page
    LDFLAGS += -municode
endif

# -ffp-contract=off: no fused multiply-adds (clang's default on Apple silicon), so the dither's
# doubles round as uzm.py's do and every system writes the same bytes
CFLAGS   ?= -O2
CFLAGS   += -std=gnu11 -Wall -Wextra -Wno-unused-parameter -ffp-contract=off

# ---- FFmpeg built here (make dist) ----
FFMPEG_VERSION ?= 8.0
BUILD    := build
FF_SRC   := $(BUILD)/FFmpeg-n$(FFMPEG_VERSION)
FF_URL   ?= https://github.com/FFmpeg/FFmpeg/archive/refs/tags/n$(FFMPEG_VERSION).tar.gz
FF_PREFIX := $(CURDIR)/$(BUILD)/ffmpeg
FF_LIB   := $(FF_PREFIX)/lib/libavformat.a
JOBS     ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
DAV1D    := $(shell $(PKG_CONFIG) --exists dav1d 2>/dev/null && echo --enable-libdav1d)
# Everything that reads: FFmpeg's own demuxers, decoders, parsers and filters (--pre / --post
# take any filter), dav1d for AV1, zlib. Nothing that writes, no device, no network, no other
# library. GPL: decimate (--ivtc) is a GPL filter, and Uzebox is GPL 3 anyway.
FF_CONFIG := --prefix=$(FF_PREFIX) --enable-static --disable-shared --disable-programs \
             --disable-doc --disable-debug --enable-gpl --enable-version3 --disable-autodetect \
             --disable-network --disable-avdevice --disable-devices --disable-encoders \
             --disable-muxers --disable-hwaccels --disable-protocols --enable-protocol=file,pipe \
             --enable-zlib $(DAV1D) --pkg-config-flags=--static

ifeq ($(STATIC),1)
    # the inner make of `make dist`: FFmpeg's static libraries first, then the system's
    # (PKG_CONFIG_PATH set by `dist`'s recipe: before make 4.4, $(shell) ignores `export`)
    OBJDIR   := $(BUILD)/obj-static
    TARGET   := dist/$(PROG)$(EXE)
    PCFLAGS  := --static
    LDFLAGS  += -s
    ifeq ($(OS),Windows_NT)
        LDFLAGS += -static
    endif
    LIBS_PC  := $(FFMPEG) libswscale libswresample
else
    OBJDIR   := obj
    TARGET   := $(BIN_DIR)/$(PROG)$(EXE)
    LIBS_PC  := $(FFMPEG)
    # Windows: once `make dist` has built FFmpeg here, bin/ always gets the standalone uzm.exe
    # (the other one needs MSYS2's FFmpeg DLLs, so runs only in an MSYS2 shell)
    ifeq ($(OS),Windows_NT)
        ifneq ($(wildcard $(FF_LIB)),)
            STANDALONE := 1
        endif
    endif
endif

OBJS     := $(SRCS:src/%.c=$(OBJDIR)/%.o)

# FFmpeg 6.0 or newer (src/media.c checks the same)
FF_MIN   := 'libavfilter >= 9.3.100'

# No FFmpeg, one too old (Ubuntu 22.04, Debian 11: 4.x) or no pkg-config: `make` says so and
# stops without an error, so the repository's `make` still builds the other tools.
# `make check` and `make install` still fail.
ifneq ($(STATIC),1)
    HAVE_FFMPEG := $(shell $(PKG_CONFIG) --exists $(LIBS_PC) $(FF_MIN) 2>/dev/null && echo y)
endif
ifeq ($(STATIC)$(STANDALONE)$(HAVE_FFMPEG),)
all:
	@echo "uzm: skipped, FFmpeg's development libraries (6.0 or newer) not found by" \
	      "$(PKG_CONFIG) (see tools/uzm/README.md)"
else
all: $(TARGET)
endif

ifeq ($(STANDALONE),1)
# `make dist` builds dist/uzm.exe and copies it to bin/
$(TARGET): $(SRCS) src/uzm.h src/media.h $(FF_LIB)
	@$(MAKE) --no-print-directory dist
else
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(shell $(PKG_CONFIG) $(PCFLAGS) --libs $(LIBS_PC)) -lm
endif

$(OBJDIR)/%.o: src/%.c src/uzm.h src/media.h | ffmpeg-check
	@mkdir -p $(OBJDIR)
	$(CC) $(shell $(PKG_CONFIG) --cflags $(LIBS_PC)) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

ffmpeg-check:
	@$(PKG_CONFIG) --exists $(LIBS_PC) $(FF_MIN) || { echo "FFmpeg's libraries (6.0 or \
	newer) not found by $(PKG_CONFIG): see the Makefile's top for the packages" >&2; exit 1; }

# ---- make dist ----
dist: $(FF_LIB)
	@PKG_CONFIG_PATH="$(FF_PREFIX)/lib/pkgconfig:$$PKG_CONFIG_PATH" \
	    $(MAKE) --no-print-directory STATIC=1 all
ifeq ($(OS),Windows_NT)
	@mkdir -p $(BIN_DIR) && cp dist/$(PROG)$(EXE) $(BIN_DIR)/
endif
	@ls -l dist/$(PROG)$(EXE)

$(FF_LIB): | $(FF_SRC)/configure
	@test -n "$(DAV1D)" || echo "note: no dav1d (pkg-config): this uzm won't read AV1 video"
	@mkdir -p $(BUILD)/tmp
	cd $(FF_SRC) && TMPDIR=$(CURDIR)/$(BUILD)/tmp ./configure $(FF_CONFIG)
	$(MAKE) -C $(FF_SRC) -j$(JOBS)
	$(MAKE) -C $(FF_SRC) install

$(FF_SRC)/configure:
	@mkdir -p $(BUILD)
	curl -fL --connect-timeout 30 --retry 3 -o $(BUILD)/ffmpeg-$(FFMPEG_VERSION).tar.gz $(FF_URL)
	tar -xzf $(BUILD)/ffmpeg-$(FFMPEG_VERSION).tar.gz -C $(BUILD)
	@touch $@

# ---- the rest ----
CHECK_DIR := check.tmp
check: $(TARGET)
	@rm -rf $(CHECK_DIR) && mkdir $(CHECK_DIR)
	@for r in 60 30 25 24; do \
		$(TARGET) test $(CHECK_DIR)/T$$r.UZM 3 --rate $$r > /dev/null && \
		$(TARGET) info $(CHECK_DIR)/T$$r.UZM | tail -1 && \
		$(TARGET) sound $(CHECK_DIR)/T$$r.UZM $(CHECK_DIR)/T$$r.wav > /dev/null && \
		$(TARGET) frame $(CHECK_DIR)/T$$r.UZM 10 $(CHECK_DIR)/T$$r.png > /dev/null || exit 1; \
	done
	@rm -rf $(CHECK_DIR)
	@echo "check: all good"

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -rf obj $(BUILD)/obj-static $(BIN_DIR)/$(PROG)$(EXE) dist $(CHECK_DIR)

distclean: clean
	rm -rf $(BUILD)

.PHONY: all dist check install clean distclean ffmpeg-check
