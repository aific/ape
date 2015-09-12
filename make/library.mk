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
# Include the compile script
#

include $(ROOT)/make/compile.mk


#
# Linker targets
#

$(BUILD_DIR)/$(TARGET): $(BUILD_DIR) $(OBJECTS)
	@rm -f $@ 2> /dev/null
	ar cq $@ $(OBJECTS)
	ranlib $@
