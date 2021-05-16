#directories
SUB_DIRECTORIES := $(wildcard */.)
SUB_MAKEFILES := $(addsuffix /Makefile, $(SUB_DIRECTORIES))
INSTALL_DIRECTORY := /usr/local/crossgcc
INSTALLED_LIB_DIRECTORY := $(INSTALL_DIRECTORY)/lib
INSTALLED_INCLUDE_DIRECTORY := $(INSTALL_DIRECTORY)/include

SRC_DIRECTORY := src
SRC_INCLUDE_DIRECTORY := include
BUILD_DIRECTORY := bld
OBJECT_DIRECTORY := $(BUILD_DIRECTORY)/obj

#commands
MKDIR := mkdir -p
CP := cp
RM := rm -rf
CC := m68k-elf-g++
LIBCFLAGS := -c -g -march=68000 -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I $(INSTALLED_INCLUDE_DIRECTORY) -L$(INSTALLED_LIB_DIRECTORY)
CFLAGS := -march=68000 -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I $(INSTALLED_INCLUDE_DIRECTORY) -L$(INSTALLED_LIB_DIRECTORY)

AS := m68k-elf-as
ASFLAGS := -m68000
AR := m68k-elf-ar
ARFLAGS := -rcs
OBJCOPY := m68k-elf-objcopy 
OBJCOPY_FLAGS := -O srec

STTY := stty
TTY := /dev/ttyS1
BAUD := 115200
CAT := cat

LINK_LINE := -lgcc -lcpp -lbsp -lc -lbsp -lgcc -Wl,--whole-archive -litanium -lcrt -lstart -Wl,--no-whole-archive

#top level build targets
TOP_LEVEL_TARGET := Zebulon
DEPENDENCIES := libcrt libstart libcpp libitanium libc libbsp

#find the source files and which object files they produce
SRC_FILES := $(wildcard $(SRC_DIRECTORY)/*.c $(SRC_DIRECTORY)/*.cpp $(SRC_DIRECTORY)/*.s)

#current build target
BUILD_TARGET := $(notdir $(CURDIR))
ifeq (,$(findstring lib,$(BUILD_TARGET)))
	BUILD_TARGET := $(BUILD_TARGET).S68

	LINK_FILES := $(wildcard $(SRC_DIRECTORY)/*.ld)
else
	BUILD_TARGET := $(BUILD_TARGET).a
	INSTALLED_TARGET := $(addprefix $(INSTALLED_LIB_DIRECTORY)/, $(BUILD_TARGET))

	HEADER_FILES := $(wildcard $(SRC_INCLUDE_DIRECTORY)/*.h)
	INSTALLED_HEADER_FILES := $(addprefix $(INSTALLED_INCLUDE_DIRECTORY)/, $(notdir $(HEADER_FILES)))

	OBJECT_FILES := $(SRC_FILES)
	OBJECT_FILES := $(OBJECT_FILES:$(SRC_DIRECTORY)/%.c=%.o)
	OBJECT_FILES := $(OBJECT_FILES:$(SRC_DIRECTORY)/%.cpp=%.o)
	OBJECT_FILES := $(OBJECT_FILES:$(SRC_DIRECTORY)/%.s=%.o)
	OBJECT_FILES := $(addprefix $(OBJECT_DIRECTORY)/, $(OBJECT_FILES))
endif
BUILD_TARGET := $(addprefix $(BUILD_DIRECTORY)/, $(BUILD_TARGET))

.DEFAULT_GOAL := all

#top level rules
.PHONY: make $(TOP_LEVEL_TARGET) build_dependencies clean_dependencies

make: $(SUB_MAKEFILES)

%/Makefile: Makefile
	$(CP) Makefile $@

copy:
	make -C $(TOP_LEVEL_TARGET) _copy

all: $(TOP_LEVEL_TARGET)
	make -C $(TOP_LEVEL_TARGET) _all

$(TOP_LEVEL_TARGET): build_dependencies

build_dependencies:
	$(foreach file, $(DEPENDENCIES), make -C $(file) _all && make -C $(file) _install;)

clean: clean_dependencies
	make -C $(TOP_LEVEL_TARGET) _clean

clean_dependencies:
	$(foreach file, $(DEPENDENCIES), make -C $(file) _clean;)

#sub level rules
.PHONY: _all _create_build_directories _build _install

_all: _create_build_directories _build

_create_build_directories: $(OBJECT_DIRECTORY)

$(OBJECT_DIRECTORY):
	@$(MKDIR) $@

_build: $(BUILD_TARGET)

%.a: $(OBJECT_FILES)
	$(AR) $(ARFLAGS) $@ $(OBJECT_FILES)

$(OBJECT_DIRECTORY)/%.o : $(SRC_DIRECTORY)/%.c
	$(CC) $< -o $@ $(LIBCFLAGS)

$(OBJECT_DIRECTORY)/%.o : $(SRC_DIRECTORY)/%.cpp
	$(CC) $< -o $@ -I $(INSTALLED_INCLUDE_DIRECTORY) $(LIBCFLAGS)

$(OBJECT_DIRECTORY)/%.o : $(SRC_DIRECTORY)/%.s
	$(AS) $< -o $@ $(ASFLAGS)

%.S68: %.out
	$(OBJCOPY) $(OBJCOPY_FLAGS) $^ $@

.PRECIOUS: %.out
%.out : $(LINK_FILES) $(SRC_FILES)
	$(CC) $(SRC_FILES) -o $@ $(CFLAGS) $(LINK_LINE) -Wl,--script=$(LINK_FILES)

_install: $(INSTALLED_HEADER_FILES) $(INSTALLED_TARGET)

$(INSTALLED_INCLUDE_DIRECTORY)/%.h: $(SRC_INCLUDE_DIRECTORY)/%.h
	$(RM) $@
	$(CP) $^ $@

$(INSTALLED_TARGET): $(BUILD_TARGET)
	$(RM) $@
	$(CP) $^ $@

_copy: $(BUILD_TARGET) 
	$(STTY) $(BAUD) -F $(TTY) && $(CAT) $^ > $(TTY)

_clean:
	$(RM) $(BUILD_DIRECTORY)
