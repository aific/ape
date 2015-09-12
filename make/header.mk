#
# Text Editor
#
# Copyright (c) Peter Macko
#


#
# Set the project root
#

ifndef ROOT
ROOT := .
endif


#
# Import machine-specific settings
#

include $(ROOT)/make/defs.mk


#
# Build output
#

ifndef BUILD_DIR
BUILD_DIR := build
endif


#
# Make-related flags
#

SHELL := /bin/bash


#
# Libraries
#

# vterm

#VTERM_INCLUDE_FLAGS := `pkg-config --cflags glib-2.0`
#VTERM_LIBRARIES := `pkg-config --libs glib-2.0` -lvterm

# Combine the flags

LIB_INCLUDE_FLAGS := $(TERM_INCLUDE_FLAGS)
LIB_LINKER_FLAGS := -L/usr/lib $(TERM_LINKER_FLAGS)
LIB_LIBRARIES := $(TERM_LIBRARIES) -lcurses -lpanel


#
# Create flags
#

DEBUG_FLAGS := -ggdb -D_DEBUG

INCLUDE_FLAGS := $(LIB_INCLUDE_FLAGS)
#COMPILER_FLAGS := $(CXXFLAGS) -O2 -Wno-deprecated

LINKER_FLAGS := $(LIB_LINKER_FLAGS)
LIBRARIES := $(LIB_LIBRARIES)
