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
# Note: You may comment out boost if you disable GUI if you want
# TODO: fix warnings before enable 'CPPFLAGS += -Wall'

######################################
# Release definitions
######################################
RELEASE_NAME = uzem$(OS_EXTENSION)
RELEASE_OBJ_DIR := Release
RELEASE_DEFINES := USE_PORT_PRINT=0
RELEASE_CPPFLAGS = $(CPPFLAGS) -O3

######################################
# Debug definitions
######################################
DEBUG_NAME = uzemdbg$(OS_EXTENSION)
DEBUG_OBJ_DIR := Debug
DEBUG_DEFINES := USE_PORT_PRINT=0 USE_SPI_DEBUG=1 USE_EEPROM_DEBUG=1 USE_GDBSERVER_DEBUG=1
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
LDFLAGS += $(shell sdl-config --libs) -lboost_thread-mt -lboost_system-mt
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
LDFLAGS += $(shell sdl-config --libs) -lboost_thread-mt -lboost_system-mt
CPPFLAG += -framework Cocoa
CC := g++
MKDIR := mkdir -p
RM := rm -rf
MTOOLS = 
endif

## Windows ###########################
ifneq (,$(findstring MINGW,$(UNAME)))
PLATFORM := Windows
OS_EXTENSION := .exe

# First check in C:/SDL
SDL_BASE_DIR := C:/SDL
SDL_DIR_ENTRIES = $(wildcard C:/SDL/*)
ifeq ($(findstring include,$(SDL_DIR_ENTRIES)),include)
    ifeq ($(findstring bin,$(SDL_DIR_ENTRIES)),bin)
        SDL_FLAGS = -I$(SDL_BASE_DIR)/include/SDL -L$(SDL_BASE_DIR)/lib
        SDL_DLL_FROM = $(SDL_BASE_DIR)/bin/SDL.dll
    endif
endif
# Didn't find ?
ifeq ($(SDL_FLAGS),)
# Then check in C:\MinGW
ifneq ($(SDL_DIR_ENTRIES)),)
    ifneq ($(wildcard C:/MinGW/include/SDL/*)),)
        ifeq ($(findstring SDL.dll,$(wildcard C:/MinGW/lib/*)),SDL.dll)
            ## SDL_FLAGS = -IC:\MinGW\includes\SDL -LC:\MinGW\lib
			SDL_FLAGS = -IC:\MinGW\include\SDL
            SDL_DLL_FROM = C:\MinGW\bin\SDL.dll
        endif
    endif
endif
endif
#If didn't find SDL...
ifeq ($(SDL_FLAGS),)
    $(error The SDL library not found. Make sure you have it in C:\SDL)
endif

SDL_DLL := $(BIN_DIR)SDL.dll
LDFLAGS += -lws2_32 -static-libgcc -static-libstdc++ -DBOOST_THREAD_USE_LIB libboost_thread-mgw45-mt-sd-1_44.a libboost_system-mgw45-mt-sd-1_44.a
LDFLAGS += -lmingw32 -lSDLmain -lSDL #keeping in this order is important
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
	$(CC) $(TARGET_OBJS) -o $(TARGET_NAME) $(CPPFLAGS) $(LDFLAGS) $(TARGET_D_DEFINES)

$(TARGET_OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $@ $(TARGET_CPPFLAGS) $(DEPFLAGS) $(TARGET_D_DEFINES)

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
	-@$(RM) $(RELEASE_OBJ_DIR) 
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

