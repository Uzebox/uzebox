# Emulator makefile - by Eric Anderton - 2009
# Public Domain - use at your own risk
#
# Filipe Rinaldi 01-Jan-2010:
#  Unix and Windows configurations
#  Using sub-folders for object files
#  Gprof support
#  Added 'make help'
#  Added automated dependencies
#

.DEFAULT_GOAL = all

TARGETS = debug release

#Uncomment to build Emscripten version (and be sure to add `arguments: ["gamefile.uze"],` below `var Module = {` in generated uzem.html file)
#EMSCRIPTEN_BUILD=1

ifeq ($(EMSCRIPTEN_BUILD),1)
    EMSCRIPTEN_FLAGS=-s USE_SDL=2
    EMSCRIPTEN_TARGET_EXTRAS=.html --preload-file gamefile.uze --preload-file eeprom.bin
    NOGDB=1
endif

#Uncomment to optimize for local CPU
#ARCH=native
#TUNE=y

######################################
# Profile Guided Optimization
######################################
# To use properly, do:
#
# make clean
# GEN=1 make release
# (Now run through your game normally to generate a profile)
# ./uzem yourgame.hex
# make clean
# USE=1 make release

ifneq ($(GEN),)
    CPPFLAGS += -fprofile-generate
    LDFLAGS  += -fprofile-generate
endif
ifneq ($(USE),)
    CPPFLAGS += -fprofile-use
    LDFLAGS  += -fprofile-use
endif

######################################
# Sources
######################################
# By default, GDB support is enabled, but if compiled with:
#
# NOGDB=1 make release
#
# then GDB support will be excluded, and the binary will run faster

NOGDB ?= 0
ifeq ($(NOGDB),0)
GDB_SRCS := gdbserver.cpp
else
CPPFLAGS += -DNOGDB=1
endif

SRCS := uzem.cpp avr8.cpp uzerom.cpp $(GDB_SRCS) SDEmulator.cpp

######################################
# Architecture
######################################
ifneq ($(ARCH),)
    CPPFLAGS += -march=$(ARCH)
    ifeq ($(TUNE),y)
        CPPFLAGS += -mtune=$(ARCH)
    endif
endif

######################################
# Destination dir
######################################
ifneq ($(DEST_DIR),)
    BIN_DIR := $(DEST_DIR)/
endif

######################################
# Global Flags
######################################
CPPFLAGS += $(SDL_FLAGS) -D$(OS) -D_GNU_SOURCE=1 -DGUI=1 -DJOY_ANALOG_DEADZONE=8192
# TODO: fix warnings before enable 'CPPFLAGS += -Wall'

######################################
# Release definitions
######################################
RELEASE_NAME = uzem$(OS_EXTENSION)
RELEASE_OBJ_DIR := Release
ifeq ($(EMSCRIPTEN_BUILD),1)
    RELEASE_CPPFLAGS = $(CPPFLAGS) $(EMSCRIPTEN_FLAGS) -O3 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 --llvm-lto 3
    RELEASE_LDFLAGS  = $(LDFLAGS)  $(EMSCRIPTEN_FLAGS) -O3 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 --llvm-lto 3
else
    RELEASE_CPPFLAGS = $(CPPFLAGS) -O3 -flto -fwhole-program
    RELEASE_LDFLAGS  = $(LDFLAGS)  -O3 -flto -fwhole-program
endif

######################################
# Debug definitions
######################################
DEBUG_NAME = uzemdbg$(OS_EXTENSION)
DEBUG_OBJ_DIR := Debug
DEBUG_DEFINES := USE_SPI_DEBUG=1 USE_EEPROM_DEBUG=1 USE_GDBSERVER_DEBUG=1
ifeq ($(EMSCRIPTEN_BUILD),1)
    DEBUG_CPPFLAGS = $(CPPFLAGS) -g $(EMSCRIPTEN_FLAGS)
    DEBUG_LDFLAGS  = $(LDFLAGS)  -g $(EMSCRIPTEN_FLAGS)
else
    DEBUG_CPPFLAGS = $(CPPFLAGS) -g
    DEBUG_LDFLAGS  = $(LDFLAGS)  -g
endif

######################################
# SD Options
######################################
SDCARDDRIVE = g

# Default option for profiling is not enabled
PROF=n

######################################
# Tools
######################################
UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
PLATFORM = Unknown

## Linux #############################
ifeq ($(UNAME),Linux)
OS := LINUX
PLATFORM := Unix
ifeq ($(EMSCRIPTEN_BUILD),1)
    SDL_FLAGS :=
    CC := emcc
else
    SDL_FLAGS := $(shell sdl2-config --cflags)
    LDFLAGS += $(shell sdl2-config --libs)
    CC := g++
endif
MKDIR := mkdir -p
RM := rm -rf
MTOOLS = 
endif

## Mac OS #############################
ifeq ($(UNAME),Darwin)
OS := MACOSX
PLATFORM := Unix
SDL_FLAGS := $(shell sdl2-config --cflags)
LDFLAGS += $(shell sdl2-config --libs)
CPPFLAG += -framework Cocoa
CC := g++
MKDIR := mkdir -p
RM := rm -rf
MTOOLS = 
endif

## Haiku #############################
ifeq ($(UNAME),Haiku)
OS := HAIKU
PLATFORM := Unix
ifeq ($(EMSCRIPTEN_BUILD),1)
    SDL_FLAGS :=
    CC := emcc
else
    SDL_FLAGS := $(shell sdl2-config --cflags)
    LDFLAGS += $(shell sdl2-config --libs) -lnetwork
    CC := g++
endif
MKDIR := mkdir -p
RM := rm -rf
MTOOLS = 
endif

## Windows ###########################
#
# IMPORTANT: To build under MinGW, you will need file "SDL_platform.h" from SDL2 2.0.4 (currently a snapshot)
# because of a bug in SDL_platform.h in 2.0.3 (currently stable release)
# C:\MinGW\include\SDL2/SDL_platform.h:121:26: fatal error: winapifamily.h: No such file or directory
# Be sure to use SDL /i686-w64-mingw32 folder for 32 bit target
#
ifneq (,$(findstring MINGW,$(UNAME)))
PLATFORM := Windows
OS_EXTENSION := .exe

# First check in C:/SDL2
SDL_BASE_DIR := C:/SDL2
SDL_DIR_ENTRIES = $(wildcard C:/SDL2/*)
ifeq ($(findstring include,$(SDL_DIR_ENTRIES)),include)
    ifeq ($(findstring bin,$(SDL_DIR_ENTRIES)),bin)
        SDL_FLAGS = -I$(SDL_BASE_DIR)/include/SDL2 -L$(SDL_BASE_DIR)/lib
        SDL_DLL_FROM = $(SDL_BASE_DIR)/bin/SDL2.dll
    endif
endif
# Didn't find ?
ifeq ($(SDL_FLAGS),)
# Then check in C:\MinGW
ifneq ($(SDL_DIR_ENTRIES)),)
    ifneq ($(wildcard C:/MinGW/include/SDL2/*)),)
        ifeq ($(findstring SDL2.dll,$(wildcard C:/MinGW/lib/*)),SDL2.dll)
			SDL_FLAGS = -IC:\MinGW\include\SDL2 
            SDL_DLL_FROM = C:\MinGW\bin\SDL2.dll
        endif
    endif
endif
endif
#If didn't find SDL...
ifeq ($(SDL_FLAGS),)
    $(error The SDL library not found. Make sure you have it in C:\SDL2)
endif

SDL_DLL := $(BIN_DIR)SDL2.dll
LDFLAGS += -lws2_32 -static-libgcc -static-libstdc++ 
LDFLAGS += -lmingw32 -lSDL2main -lSDL2  #keeping in this order is important

CC := g++
MKDIR := mkdir -p
RM := -rm -rf
CP := cp
MTOOLS = utils/mtools
endif

ifeq ($(PLATFORM),Unknown)
        $(error Unsupported platform!)
endif

MAKEFLAGS += --no-print-directory

######################################
# Rules
######################################

ifeq ($(MAKECMDGOALS),release)
    CFG := RELEASE
endif
ifeq ($(MAKECMDGOALS),debug)
    CFG := DEBUG
endif

ifeq ($(PROF),y)
    CPPFLAGS += -pg
endif


TARGET_NAME = $(BIN_DIR)$($(CFG)_NAME)
TARGET_OBJ_DIR = $($(CFG)_OBJ_DIR)
TARGET_CPPFLAGS = $($(CFG)_CPPFLAGS)
TARGET_LDFLAGS = $($(CFG)_LDFLAGS)
TARGET_DEFINES = $($(CFG)_DEFINES)
TARGET_MSG = $($(CFG)_MSG)

TARGET_OBJS = $(patsubst %.cpp, $(TARGET_OBJ_DIR)/%.o, $(SRCS))
TARGET_D_DEFINES = $(patsubst %,-D%, $(TARGET_DEFINES))

DEP = $(patsubst %.cpp, $(TARGET_OBJ_DIR)/%.d, $(SRCS))
-include $(DEP)
DEPFLAGS = -MD -MP -MF $(patsubst %.o,%.d,$@ )

all:
	$(MAKE) debug
	$(MAKE) release

.PHONY: msg
msg:
	@echo =================================
	@echo Building $(TARGET_OBJ_DIR)...
	@echo Platform: $(PLATFORM)-$(OS)
	@echo =================================
	@$(MKDIR) $(TARGET_OBJ_DIR)

.PHONY: $(TARGETS)
$(TARGETS): msg $(TARGET_NAME)
	@echo done!

$(TARGET_NAME): $(TARGET_OBJS) $(SDL_DLL)
	$(CC) $(TARGET_OBJS) -o $(TARGET_NAME)$(EMSCRIPTEN_TARGET_EXTRAS) $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS) $(TARGET_D_DEFINES)

$(TARGET_OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $@ $(TARGET_CPPFLAGS) $(DEPFLAGS) $(TARGET_D_DEFINES)
	$(CC) -c -S $< -o $@.asm $(TARGET_CPPFLAGS) $(DEPFLAGS) $(TARGET_D_DEFINES)

# This rule used only for Windows
$(SDL_DLL): $(SDL_DLL_FROM)
	@echo Copying SDL dll from $(SDL_DLL_FROM)
	$(CP) $(SDL_DLL_FROM) $(SDL_DLL)

.PHONY: SDCardDemo
SDCardDemo:
	make -C ../demos/SDCardDemo all
	cp ../demos/SDCardDemo/SDCardDemo.hex .
    
debug-sd: all SDCardDemo
	$(DEBUG_NAME) --sd $(SDCARDDRIVE) SDCardDemo/SDCardDemo.hex

.PHONY: clean    
clean:
	-@$(RM) $(RELEASE_OBJ_DIR)/*.o
	-@$(RM) $(RELEASE_OBJ_DIR)/*.d
	-@$(RM) $(RELEASE_OBJ_DIR)/*.asm
# DO NOT DELETE $(RELEASE_OBJ_DIR)/*.gcda in order to use PGO
	-@$(RM) $(DEBUG_OBJ_DIR)
	-@$(RM) $(BIN_DIR)$(RELEASE_NAME)
	-@$(RM) $(BIN_DIR)$(DEBUG_NAME)
	-@$(RM) $(SDL_DLL)

.PHONY: help
help:
	@echo
	@echo ===================================
	@echo Uzebox Emulator Build System - Help
	@echo ===================================
	@echo The targets available are:
	@echo --------------------------
	@echo \'make\' or \'make all\' - will build both debug and release versions 
	@echo \'make release\' - release version
	@echo \'make debug\' - debug version
	@echo \'make clean\' - clean all object files and binaries for debug and release versions
	@echo \'make SDCardDemo\' - Builds the SDCard demo and copy the iHex file to local dir
	@echo \'debug-sd\' - Starts $(DEBUG_NAME) using the SDCard demo image
	@echo \'make help\' - this help :-\)
	@echo Flags available:
	@echo ----------------
	@echo PROF=\[y\|n\] - Enable or disable profiling using gprof, e.g.: \'make debug PROF=y\'. The default is PROF=$(PROF)
	@echo ARCH=cpu-type - Choose the target CPU to build Uzem. The compiler generates instructions for the cpu-type
	@echo TUNE=\[y\|n\] - Enable or disable optmizations for the cpu-type indicated in ARCH=\'cpu-type\'
	@echo DEST_DIR=path - Use this flag to place the binary file \(and any required library\) in the \'path\' directory
	@echo Tips:
	@echo -----
	@echo If you have a multiprocessor system, use \'-j N\', e.g.: \'make release -j 3\'
	@echo

