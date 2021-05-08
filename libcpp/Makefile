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
CFLAGS := -c -g -march=68000 -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I $(INSTALLED_INCLUDE_DIRECTORY) -L$(INSTALLED_LIB_DIRECTORY)
AS := m68k-elf-as
ASFLAGS := -m68000
AR := m68k-elf-ar
ARFLAGS := -rcs
OBJCOPY := m68k-elf-objcopy 
OBJCOPY_FLAGS := -O srec

LINK_LINE := -lgcc -lcpp -lbsp -lc -lbsp -lgcc -Wl,--whole-archive -litanium -lcrt -lstart -Wl,--no-whole-archive 

#top level build targets
TOP_LEVEL_TARGET := Zebulon
DEPENDENCIES := libcrt libstart libcpp libitanium libc libbsp

#find the source files and which object files they produce
SRC_FILES := $(wildcard $(SRC_DIRECTORY)/*.c $(SRC_DIRECTORY)/*.cpp $(SRC_DIRECTORY)/*.s)

#current build target
BUILD_TARGET := $(notdir $(CURDIR))
ifeq (,$(findstring lib,$(BUILD_TARGET)))
	INTERMEDIATE_OBJECT := $(addprefix $(OBJECT_DIRECTORY), $(addsuffix .out, $(BUILD_TARGET)))
	TYPED_BUILD_TARGET := $(BUILD_TARGET).S68
	INSTALLED_TARGET := $(addprefix ../../../Projects/68000/$(BUILD_TARGET)/, $(TYPED_BUILD_TARGET))

	LINK_FILES := $(wildcard $(SRC_DIRECTORY)/*.ld)
else
	TYPED_BUILD_TARGET := $(BUILD_TARGET).a
	INSTALLED_TARGET := $(addprefix $(INSTALLED_LIB_DIRECTORY)/, $(TYPED_BUILD_TARGET))

	HEADER_FILES := $(wildcard $(SRC_INCLUDE_DIRECTORY)/*.h)
	INSTALLED_HEADER_FILES := $(addprefix $(INSTALLED_INCLUDE_DIRECTORY)/, $(notdir $(HEADER_FILES)))

	OBJECT_FILES := $(SRC_FILES)
	OBJECT_FILES := $(OBJECT_FILES:$(SRC_DIRECTORY)/%.c=%.o)
	OBJECT_FILES := $(OBJECT_FILES:$(SRC_DIRECTORY)/%.cpp=%.o)
	OBJECT_FILES := $(OBJECT_FILES:$(SRC_DIRECTORY)/%.s=%.o)
	OBJECT_FILES := $(addprefix $(OBJECT_DIRECTORY)/, $(OBJECT_FILES))
endif
BUILD_TARGET := $(addprefix $(BUILD_DIRECTORY)/, $(BUILD_TARGET))
TYPED_BUILD_TARGET := $(addprefix $(BUILD_DIRECTORY)/, $(TYPED_BUILD_TARGET))

.DEFAULT_GOAL := all

#top level rules
.PHONY: make $(TOP_LEVEL_TARGET) build_dependencies clean_dependencies

make: $(SUB_MAKEFILES)

%/Makefile: Makefile
	$(CP) Makefile $@

all: $(TOP_LEVEL_TARGET)
	make -C $(TOP_LEVEL_TARGET) _all

$(TOP_LEVEL_TARGET): build_dependencies

build_dependencies:
	$(foreach file, $(DEPENDENCIES), make -C $(file) _all;)

clean: clean_dependencies
	make -C $(TOP_LEVEL_TARGET) _clean

clean_dependencies:
	$(foreach file, $(DEPENDENCIES), make -C $(file) _clean;)

#sub level rules
.PHONY: _all _create_build_directories _build_and_install

_all: _create_build_directories _build_and_install

_create_build_directories: $(OBJECT_DIRECTORY)

$(OBJECT_DIRECTORY):
	@$(MKDIR) $@

_build_and_install: $(INSTALLED_HEADER_FILES) $(INSTALLED_TARGET)

$(INSTALLED_INCLUDE_DIRECTORY)/%.h: $(SRC_INCLUDE_DIRECTORY)/%.h
	$(RM) $@
	$(CP) $^ $@

$(INSTALLED_TARGET): $(TYPED_BUILD_TARGET)
	$(RM) $@
	$(CP) $^ $@

$(BUILD_TARGET).a: $(OBJECT_FILES)
	$(AR) $(ARFLAGS) $@ $(OBJECT_FILES)

$(OBJECT_DIRECTORY)/%.o : $(SRC_DIRECTORY)/%.c
	$(CC) $< -o $@ $(CFLAGS)

$(OBJECT_DIRECTORY)/%.o : $(SRC_DIRECTORY)/%.cpp
	$(CC) $< -o $@ -I $(INSTALLED_INCLUDE_DIRECTORY) $(CFLAGS)

$(OBJECT_DIRECTORY)/%.o : $(SRC_DIRECTORY)/%.s
	$(AS) $< -o $@ $(ASFLAGS)

$(BUILD_TARGET).S68: $(INTERMEDIATE_OBJECT)
	$(OBJCOPY) $(OBJCOPY_FLAGS) $^ $@

$(INTERMEDIATE_OBJECT): $(LINK_FILES)
	$(CC) $(SRC_FILES) -o $(INTERMEDIATE_OBJECT) $(CFLAGS) $(LINK_LINE) -Wl,--script=$(LINK_FILES)

_clean:
	$(RM) $(BUILD_DIRECTORY)
