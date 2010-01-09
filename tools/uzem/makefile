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

######################################
# Sources
######################################
SRCS := uzem.cpp avr8.cpp uzerom.cpp gdbserver.cpp

######################################
# Architecture
######################################
ifeq ($(ARCH),)
    ARCH := native
    MTUNE := -mtune=native
endif

######################################
# Global Flags
######################################
CPPFLAGS = -march=$(ARCH) $(MTUNE)
CPPFLAGS += $(SDL_FLAGS) -D$(OS) -D_GNU_SOURCE=1 -DGUI=1
# TODO: fix warnings before enable 'CPPFLAGS += -Wall'

LDFLAGS +=  -lSDLmain -lSDL 

######################################
# Release definitions
######################################
RELEASE_NAME := uzem
RELEASE_OBJ_DIR := Release
RELEASE_DEFINES := USE_PORT_PRINT=1
RELEASE_CPPFLAGS = $(CPPFLAGS) -O3

######################################
# Debug definitions
######################################
DEBUG_NAME := uzemdbg
DEBUG_OBJ_DIR := Debug
DEBUG_DEFINES := USE_PORT_PRINT=1 USE_SPI_DEBUG=1 USE_EEPROM_DEBUG=1 USE_GDBSERVER_DEBUG=1
DEBUG_CPPFLAGS = $(CPPFLAGS) -g

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
SDL_FLAGS := $(shell sdl-config --cflags)
CC := g++
MKDIR := mkdir -p
RM := rm -rf
MTOOLS = 
endif

## Mac OS #############################
ifeq ($(UNAME),Darwin)
OS := MACOSX
PLATFORM := Unix
SDL_FLAGS := $(shell sdl-config --cflags)
CPPFLAG += -framework Cocoa
CC := g++
MKDIR := mkdir -p
RM := rm -rf
MTOOLS = 
endif

## Windows ###########################
ifneq (,$(findstring MINGW,$(UNAME)))
PLATFORM := Windows
LDFLAGS += -lws2_32 -Wl,-u,_WinMain@16
SDL_FLAGS = -IC:/MinGW/include/SDL
CC := g++
MKDIR := mkdir -p
RM := -rm -rf
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


TARGET_NAME = $($(CFG)_NAME)
TARGET_OBJ_DIR = $($(CFG)_OBJ_DIR)
TARGET_CPPFLAGS = $($(CFG)_CPPFLAGS)
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

$(TARGET_NAME): $(TARGET_OBJS)
	$(CC) $(TARGET_OBJS) -o $(TARGET_NAME) $(CPPFLAGS) $(LDFLAGS) $(TARGET_D_DEFINES)

$(TARGET_OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $@ $(TARGET_CPPFLAGS) $(DEPFLAGS) $(TARGET_D_DEFINES)

.PHONY: SDCardDemo
SDCardDemo:
	make -C ../demos/SDCardDemo all
	cp ../demos/SDCardDemo/SDCardDemo.hex .
    
debug-sd: all SDCardDemo
	$(DEBUG_NAME) --sd $(SDCARDDRIVE) SDCardDemo/SDCardDemo.hex

.PHONY: clean    
clean:
	-@$(RM) $(RELEASE_OBJ_DIR) 
	-@$(RM) $(DEBUG_OBJ_DIR)
	-@$(RM) $(RELEASE_NAME)
	-@$(RM) $(DEBUG_NAME)

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
	@echo \'make SDCardDemo\' - Builds the SDCard demo and copy the iHex file to local dir.
	@echo \'debug-sd\' - Starts $(DEBUG_NAME) using the SDCard demo image.
	@echo \'make help\' - this help :-\)
	@echo Flags available:
	@echo ----------------
	@echo PROF=\[y\|n\] - Enable or disable profiling using gprof, e.g.: \'make debug PROF=y\'. The default is PROF=$(PROF)
	@echo ARCH=cpu-type - Choose the target CPU to build Uzem. The default cpu-type is \'native\' which gives you the best performance. Only use this flag with you want build a more generic binary to release. E.g: \'make release ARCH=i686\'
	@echo Tips:
	@echo -----
	@echo If you have a multiprocessor system, use \'-j N\', e.g.: \'make release -j 3\'
	@echo

