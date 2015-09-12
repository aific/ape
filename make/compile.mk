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
# Include the header
#

include $(ROOT)/make/header.mk


#
# Commands
#

DEPEND := $(CXX) -MM $(COMPILER_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS)
COMPILE := $(CXX) -c $(COMPILER_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS)
LINK := $(CXX) $(LINKER_FLAGS) $(DEBUG_FLAGS)

COMPILE := $(strip $(COMPILE))
LIBRARIES := $(strip $(LIBRARIES))
LINK := $(strip $(LINK))


#
# Preprocess the input data
#

SOURCES := $(sort $(SOURCES)) 
OBJECTS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))
OBJECTS := $(patsubst %,$(BUILD_DIR)/%,$(sort $(OBJECTS)))

PROG_SOURCES := $(sort $(PROG_SOURCES)) 
PROG_OBJECTS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(PROG_SOURCES)))
PROG_OBJECTS := $(patsubst %,$(BUILD_DIR)/%,$(sort $(PROG_OBJECTS)))

ALL_SOURCES := $(sort $(PROG_SOURCES) $(SOURCES))
ALL_OBJECTS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(ALL_SOURCES)))
ALL_OBJECTS := $(patsubst %,$(BUILD_DIR)/%,$(sort $(ALL_OBJECTS)))


#
# Phony and preliminary targets
#

.PHONY: all clean distclean messclean lines depend target targetclean relink

all: $(BUILD_DIR)/$(TARGET)

target: $(BUILD_DIR)/$(TARGET)

targetclean:
	@rm -rf $(BUILD_DIR)/$(TARGET)

relink: targetclean
	@$(MAKE) target

clean distclean: messclean
	@rm -rf project.mk $(BUILD_DIR)

messclean:
	@find . -name '*~' -delete 2> /dev/null
	@rm -rf *.db 2> /dev/null
	@rm -rf __db.* log.* 2> /dev/null
	
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

lines:
	@echo Total number of lines:
	@cat Makefile *.cpp *.h | wc -l


#
# Compiler targets & dependencies
#

depend: $(BUILD_DIR)/project.mk

$(BUILD_DIR)/project.mk: $(ALL_SOURCES) *.h
	@echo "Remaking project.mk"
	@rm -f $@ 2> /dev/null
	@echo '.PHONY: all' >> $@
	@echo 'all: $(ALL_OBJECTS)' >> $@
	@echo >> $@
	@for S in $(ALL_SOURCES); do \
		$(DEPEND) $$S | sed 's|^[a-zA-Z0-9]*\.o|$(BUILD_DIR)\/&|' >> $@; \
		if [[ $${PIPESTATUS[0]} -ne 0 ]]; then exit 1; fi; \
		echo '	$(COMPILE) -o $$@ $$<' >> $@; \
		echo >> $@; \
	done

$(ALL_OBJECTS): $(BUILD_DIR)/project.mk $(ALL_SOURCES)
	@$(MAKE) --no-print-directory -f $(BUILD_DIR)/project.mk $@ \
		| grep -v 'is up to date' \
		| grep -v 'Nothing to be done for' \
		; exit $${PIPESTATUS[0]}
