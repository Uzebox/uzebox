#
# Uzebox 
#
# Build System by Filipe Rinaldi, 9 January 2010
# This build system will build the tools, demos and in the future a SDCard image.
# Type 'make help' to see the options
#

.DEFAULT_GOAL = all 
MAKEFLAGS += --no-print-directory

######################################
# Tools
######################################
TOOLS += uzem
TOOLS += packrom
TOOLS += gconvert

######################################
# Tools used to build demos
######################################
TOOLS_DEP += packrom
TOOLS_DEP += gconvert

######################################
# Demos
######################################
DEMOS += Arkanoid
DEMOS += DrMario
DEMOS += Megatris
DEMOS += SuperMarioDemo
DEMOS += VectorDemo
DEMOS += Whack-a-Mole
DEMOS += BitmapDemo
DEMOS += Maze
DEMOS += MusicDemo
DEMOS += SpriteDemo
DEMOS += Mode9Demo
DEMOS += Uzeamp
DEMOS += Tutorial
DEMOS += VideoDemo
DEMOS += SDCardDemo
DEMOS += Bootloader
DEMOS += Bootloader_Pragma

######################################
# Disabled
######################################
#DEMOS += Unittest Fix Makefile

######################################
# Tools
######################################
RM := rm -rf
CP := cp

######################################
# Directories
######################################
TOOLS_DIR := tools
DEMOS_DIR = demos
BIN_DIR := bin
ROMS_DIR := roms

######################################
# Handling options and flags
######################################
ifeq (clean,$(MAKECMDGOALS))
    CLEAN := clean
endif
DEST_FLAG = DEST_DIR=$(shell pwd)/$(BIN_DIR)/

ALL_TARGETS_TOOLS = $(patsubst %,$(TOOLS_DIR)/%,$(TOOLS))
ALL_TARGETS_DEMOS = $(patsubst %,$(DEMOS_DIR)/%/default,$(DEMOS))

ALL_TARGETS = $(ALL_TARGETS_TOOLS) $(ALL_TARGETS_DEMOS)

ifeq ($(CLEAN),)
    ALL_TARGETS_TOOLS_DEP = $(patsubst %,$(TOOLS_DIR)/%,$(TOOLS_DEP))
endif

UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')

## Windows ###########################
ifneq (,$(findstring MINGW,$(UNAME)))
    OS_EXTENSION = .exe
endif

######################################
# Rules
######################################
all: $(ALL_TARGETS)

.PHONY: demos
demos: $(ALL_TARGETS_DEMOS)

.PHONY: tools
tools: $(ALL_TARGETS_TOOLS)

.PHONY: $(ALL_TARGETS_TOOLS)
$(ALL_TARGETS_TOOLS):
	$(MAKE) -C $@ $(CLEAN) $(DEST_FLAG)

.PHONY: $(ALL_TARGETS_DEMOS)
$(ALL_TARGETS_DEMOS): $(ALL_TARGETS_TOOLS_DEP)
	@echo ===================================
	@echo Building demo: $@
	@echo ===================================
	$(MAKE) -C $@ $(CLEAN)
ifeq ($(CLEAN),)
	$(CP) $@/$(patsubst $(DEMOS_DIR)/%/default,%,$@).hex $(ROMS_DIR)
	-$(CP) $@/$(patsubst $(DEMOS_DIR)/%/default,%,$@).uze $(ROMS_DIR)
endif

clean: $(ALL_TARGETS)


cleanall:
	$(RM) $(BIN_DIR)/*
	$(RM) $(ROMS_DIR)/*
	
.PHONY: help
help:
	@echo
	@echo ===================================
	@echo Uzebox Build System - Help
	@echo ===================================
	@echo The targets available are:
	@echo --------------------------
	@echo \'make\' or \'make all\' - Build all tools and demos
	@echo \'make tools\' - Build only the tools and copy them to \'bin\' directory
	@echo \'make demos\' - Build only the demos and copy the iHex files to \'bin\' directory
	@echo \'make clean\' - clean all the generated files
	@echo \'make help\' - This help :-\)
	@echo Flags:
	@echo ------
	@echo If the tools or demos can accept flags you can pass from the command line. E.g: \'make tools ARCH=i686\'
	@echo Tips:
	@echo -----
	@echo If you have a multiprocessor system, use \'-j N\', e.g.: \'make release -j 3\'
	@echo
	

