#commands
MKDIR := mkdir -p
CP := cp
RM := rm -rf
CC := m68k-elf-g++
CFLAGS := -c -g -march=68000 -nostartfiles -nostdlib -fno-exceptions
AS := m68k-elf-as
ASFLAGS := -m68000
AR := m68k-elf-ar
ARFLAGS := -rcs

#directories
SUB_DIRECTORIES := $(wildcard */.)
SUB_MAKEFILES := $(addsuffix /Makefile, ${SUB_DIRECTORIES})
INSTALL_DIRECTORY := /usr/local/crossgcc
INSTALLED_LIB_DIRECTORY := ${INSTALL_DIRECTORY}/lib
INSTALLED_INCLUDE_DIRECTORY := ${INSTALL_DIRECTORY}/include
BUILD_DIRECTORY := bld
OBJECT_DIRECTORY := $(BUILD_DIRECTORY)/obj
SRC_DIRECTORY := src
SRC_INCLUDE_DIRECTORY := include

#top level build targets
TOP_LEVEL_TARGET := Zebulon
DEPENDENCIES := libcrt libstart libcpp libitanium libc libbsp
CLEAN_DEPENDENCIES := $(addprefix CLEAN_, $(DEPENDENCIES))

#current build target
CURRENT_DIRECTORY := $(notdir $(CURDIR))
ifeq (,$(findstring lib,$(CURRENT_DIRECTORY)))
	TARGET := $(CURRENT_DIRECTORY).S68
	INSTALLED_TARGET := $(addprefix ../../../../../Projects/68000/, $(TARGET))
else
	TARGET := $(CURRENT_DIRECTORY).a
	INSTALLED_TARGET := $(addprefix $(INSTALLED_LIB_DIRECTORY)/, $(TARGET))
endif

BUILD_TARGET := $(addprefix $(BUILD_DIRECTORY)/, $(TARGET))

#find the source files and which object files they produce
HEADER_FILES := $(wildcard $(SRC_INCLUDE_DIRECTORY)/*.h)
INSTALLED_HEADER_FILES := $(addprefix $(INSTALLED_INCLUDE_DIRECTORY)/, $(notdir $(HEADER_FILES)))
SRC_FILES := $(wildcard $(SRC_DIRECTORY)/*.c $(SRC_DIRECTORY)/*.cpp $(SRC_DIRECTORY)/*.s)
OBJECT_FILES := $(SRC_FILES)
OBJECT_FILES := $(OBJECT_FILES:$(SRC_DIRECTORY)/%.c=%.o)
OBJECT_FILES := $(OBJECT_FILES:$(SRC_DIRECTORY)/%.cpp=%.o)
OBJECT_FILES := $(OBJECT_FILES:$(SRC_DIRECTORY)/%.s=%.o)
OBJECT_FILES := $(addprefix ${OBJECT_DIRECTORY}/, ${OBJECT_FILES})

.DEFAULT_GOAL := all

#top level rules
.PHONY: make $(TOP_LEVEL_TARGET) build_dependencies clean_dependencies

make: ${SUB_MAKEFILES}

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

${INSTALLED_INCLUDE_DIRECTORY}/%.h: ${SRC_INCLUDE_DIRECTORY}/%.h
	$(RM) $@
	$(CP) $^ $@

$(INSTALLED_TARGET): $(BUILD_TARGET)
	$(RM) $@
	$(CP) $^ $@

$(BUILD_TARGET): ${OBJECT_FILES}
	${AR} ${ARFLAGS} $@ ${OBJECT_FILES}

${OBJECT_DIRECTORY}/%.o : ${SRC_DIRECTORY}/%.c
	${CC} $< -o $@ -I ${INSTALLED_INCLUDE_DIRECTORY} ${CFLAGS}

${OBJECT_DIRECTORY}/%.o : ${SRC_DIRECTORY}/%.cpp
	${CC} $< -o $@ -I ${INSTALLED_INCLUDE_DIRECTORY} ${CFLAGS}

${OBJECT_DIRECTORY}/%.o : ${SRC_DIRECTORY}/%.s
	${AS} $< -o $@ ${ASFLAGS}

_clean:
	$(RM) $(BUILD_DIRECTORY)

















