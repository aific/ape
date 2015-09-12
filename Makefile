#
# Provenance storage backend inspired by PASS 2006
#
# Copyright (c) Peter Macko
#


#
# Source files
#

SOURCES := main.cpp Window.cpp Manager.cpp ASCIITable.cpp ColorTable.cpp \
           MenuWindow.cpp ScrollBar.cpp Editor.cpp Document.cpp Histogram.cpp \
		   EditAction.cpp util.cpp Component.cpp Container.cpp \
		   CheckBox.cpp EditorWindow.cpp SplitPane.cpp Label.cpp \
		   Button.cpp TerminalControl.cpp DialogWindow.cpp


#
# Target executable
#

TARGET := ape


#
# Additional configuration
#

RUN_DEV_ARGS :=


#
# Include the magic script
#

include make/program.mk
