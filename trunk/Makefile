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
DEMOS += Uzeamp
DEMOS += VideoDemo
DEMOS += SDCardDemo

######################################
# Disabled
######################################
#Unittest
#Bootloader
#Bootloader_Pragma

TOOLS_DIR := tools
DEMOS_DIR := demos
BIN_DIR := bin

ALL_TARGETS_TOOLS = $(patsubst %,$(TOOLS_DIR)/%,$(TOOLS))
ALL_TARGETS_DEMOS = $(patsubst %,$(DEMOS_DIR)/%/default,$(DEMOS))

ALL_TARGETS = $(ALL_TARGETS_TOOLS) $(ALL_TARGETS_DEMOS)

######################################
# Options
######################################
ifneq ($(ARCH),)
    ARCH_FLAG := $(ARCH)
endif
ifeq (clean,$(MAKECMDGOALS))
    CLEAN := clean
endif


######################################
# Tools
######################################
UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
PLATFORM = Unknown
RM := rm -rf
CP := cp

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
	$(MAKE) -C $@ $(CLEAN) $(ARCH_FLAG)
ifeq ($(CLEAN),)
	$(CP) $@/$(patsubst $(TOOLS_DIR)/%,%,$@)$(OS_EXTENSION) $(BIN_DIR)
endif

.PHONY: $(ALL_TARGETS_DEMOS)
$(ALL_TARGETS_DEMOS):
	@echo ===================================
	@echo Building demo: $@
	@echo ===================================
	$(MAKE) -C $@ $(CLEAN)
ifeq ($(CLEAN),)
	$(CP) $@/$(patsubst $(DEMOS_DIR)/%/default,%,$@).hex $(BIN_DIR)
endif

clean: $(ALL_TARGETS)
	$(RM) $(BIN_DIR)/*

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
	@echo Tips:
	@echo -----
	@echo If you have a multiprocessor system, use \'-j N\', e.g.: \'make release -j 3\'
	@echo
	

