#
# Uzebox 
#

.DEFAULT_GOAL = all

PROJECT = Packrom
TARGETS = debug release

######################################
# Sources
######################################
SRCS := packrom.cpp

######################################
# Architecture
######################################
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
CPPFLAGS += -D$(OS)
CPPFLAGS += -Wall

LDFLAGS +=  

######################################
# Release definitions
######################################
RELEASE_NAME = packrom$(OS_EXTENSION)
RELEASE_OBJ_DIR := Release
RELEASE_DEFINES := 
RELEASE_CPPFLAGS = $(CPPFLAGS) -O3

######################################
# Debug definitions
######################################
DEBUG_NAME = packromdbg$(OS_EXTENSION)
DEBUG_OBJ_DIR := Debug
DEBUG_DEFINES := DEBUG
DEBUG_CPPFLAGS = $(CPPFLAGS) -g

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
CC := g++
MKDIR := mkdir -p
RM := rm -rf
MTOOLS = 
endif

## Mac OS #############################
ifeq ($(UNAME),Darwin)
OS := MACOSX
PLATFORM := Unix
CPPFLAG += -framework Cocoa
CC := g++
MKDIR := mkdir -p
RM := rm -rf
MTOOLS = 
endif

## Windows ###########################
ifneq (,$(findstring MINGW,$(UNAME)))
PLATFORM := Windows
OS_EXTENSION = .exe
LDFLAGS += -lws2_32 -Wl,-u,_WinMain@16
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
	@echo Building $(PROJECT) $(TARGET_OBJ_DIR)...
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

.PHONY: clean    
clean:
	-@$(RM) $(RELEASE_OBJ_DIR) 
	-@$(RM) $(DEBUG_OBJ_DIR)
	-@$(RM) $(BIN_DIR)$(RELEASE_NAME)
	-@$(RM) $(BIN_DIR)$(DEBUG_NAME)

.PHONY: help
help:
	@echo
	@echo ===================================
	@echo Uzebox Packrom Build System - Help
	@echo ===================================
	@echo The targets available are:
	@echo --------------------------
	@echo \'make\' or \'make all\' - will build both debug and release versions 
	@echo \'make release\' - release version
	@echo \'make debug\' - debug version
	@echo \'make clean\' - clean all object files and binaries for debug and release versions
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

