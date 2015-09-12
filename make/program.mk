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
# Phony targets
#

.PHONY: run-dev run gdb

run-dev: all
	@$(BUILD_DIR)/$(TARGET) $(RUN_DEV_ARGS)

run: all
	@$(BUILD_DIR)/$(TARGET)

gdb: all
	@gdb $(BUILD_DIR)/$(TARGET)


#
# Linker targets
#

$(BUILD_DIR)/$(TARGET): $(BUILD_DIR) $(OBJECTS) $(PROG_OBJECTS)
	$(LINK) -o $@ $(OBJECTS) $(PROG_OBJECTS) $(LIBRARIES)
