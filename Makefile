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

#top level build targets
TOP_LEVEL_TARGET := Zebulon
DEPENDENCIES := libcrt libstart libcpp libitanium libc libbsp
CLEAN_DEPENDENCIES := $(addprefix CLEAN_, $(DEPENDENCIES))

#current build target
CURRENT_DIRECTORY := $(notdir $(CURDIR))
ifeq (,$(findstring lib,$(CURRENT_DIRECTORY)))
	TARGET := $(CURRENT_DIRECTORY).S68
else
	TARGET := $(CURRENT_DIRECTORY).a
endif
BUILD_TARGET := $(addprefix $(BUILD_DIRECTORY)/, $(TARGET))
INSTALLED_TARGET := $(addprefix $(INSTALLED_LIB_DIRECTORY), $TARGET)

#find the source files and which object files they produce
SRC_FILES := $(wildcard ${SRC_DIRECTORY}/*.c ${SRC_DIRECTORY}/*.cpp ${SRC_DIRECTORY}/*.s)
OBJECT_FILES := ${SRC_FILES}
OBJECT_FILES := ${OBJECT_FILES:${SRC_DIRECTORY}/%.c=%.o}
OBJECT_FILES := ${OBJECT_FILES:${SRC_DIRECTORY}/%.cpp=%.o}
OBJECT_FILES := ${OBJECT_FILES:${SRC_DIRECTORY}/%.s=%.o}
OBJECT_FILES := $(addprefix ${OBJECT_DIRECTORY}/, ${OBJECT_FILES})

.DEFAULT_GOAL := all

#top level rules
.PHONY: make $(TOP_LEVEL_TARGET) build_dependencies clean_dependencies

make: ${SUB_MAKEFILES}

%/Makefile: Makefile
	$(CP) Makefile $@

all: $(TOP_LEVEL_TARGET)
	@echo building target: $^
	make -C $(TOP_LEVEL_TARGET) _all

$(TOP_LEVEL_TARGET): build_dependencies
	@echo built dependencies: $^

build_dependencies:
	$(foreach file, $(DEPENDENCIES), make -C $(file) _all;)

clean: clean_dependencies

clean_dependencies:
	$(foreach file, $(DEPENDENCIES), make -C $(file) _clean;)

#sub level rules
.PHONY: _all _create_build_directories _target _install

_all: _create_build_directories _build_target _install

_create_build_directories: $(OBJECT_DIRECTORY)

$(OBJECT_DIRECTORY):
	@echo creating build directories
	@$(MKDIR) $@

_build_target: $(BUILD_TARGET)
	@echo building $^

$(BUILD_TARGET): ${OBJECT_FILES}
	${AR} ${ARFLAGS} $@ ${OBJECT_FILES}

${OBJECT_DIRECTORY}/%.o : ${SRC_DIRECTORY}/%.c
	${CC} $< -o $@ -I ${INSTALLED_INCLUDE_DIRECTORY} ${CFLAGS}

${OBJECT_DIRECTORY}/%.o : ${SRC_DIRECTORY}/%.cpp
	${CC} $< -o $@ -I ${INSTALLED_INCLUDE_DIRECTORY} ${CFLAGS}

${OBJECT_DIRECTORY}/%.o : ${SRC_DIRECTORY}/%.s
	${AS} $< -o $@ ${ASFLAGS}

#	@echo $(CP) $(notdir $@)$(BUILD_DIRECTORY)/$@.o $(INSTALL_LIB_DIRECTORY)

_install: ${INSTALLED_TARGET}

#${INSTALL_INCLUDE_DIR}/%.h : ${INCLUDE_DIR}/%.h
#	rm -f $@
#	cp $< $@

$(INSTALLED_TARGET): $(BUILD_TARGET)
	@echo copying $^ to $@
#	rm -f $@
#	cp $< $@

_clean:
	$(RM) $(BUILD_DIRECTORY)
















#ifeq ("x","y")
INCLUDE_DIR := include
SRC_HEADERS := $(wildcard ${INCLUDE_DIR}/*.h)
TARGET_LIB := ${INSTALL_LIB_DIR}/${LIB}
TARGET_HEADERS := $(addprefix ${INSTALL_INCLUDE_DIR}/, $(notdir ${SRC_HEADERS}))

install: ${TARGET_LIB} ${TARGET_HEADERS}

${INSTALL_INCLUDE_DIR}/%.h : ${INCLUDE_DIR}/%.h
	rm -f $@
	cp $< $@

${INSTALL_LIB_DIR}/%.a : ${BLD_DIR}/%.a
	rm -f $@
	cp $< $@

#endif
