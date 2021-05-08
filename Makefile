#commands
MKDIR := mkdir -p
CP := cp
RM := rm -rf

#directories
SUB_DIRECTORIES := $(wildcard */.)
SUB_MAKEFILES := $(addsuffix /Makefile, ${SUB_DIRECTORIES})
INSTALL_DIRECTORY := /usr/local/crossgcc
INSTALLED_LIB_DIRECTORY := ${INSTALL_DIRECTORY}/lib
INSTALLED_INCLUDE_DIRECTORY := ${INSTALL_DIRECTORY}/include

BUILD_DIRECTORY := bld
OBJECT_DIRECTORY := $(BUILD_DIRECTORY)/obj

#top level build targets
TOP_LEVEL_TARGET := Zebulon
DEPENDENCIES := libbsp libstart libc libcpp libitanium libcrt
INSTALLED_DEPENDENCIES := $(addprefix $(INSTALLED_LIB_DIRECTORY)/, $(DEPENDENCIES))

#current build target
CURRENT_DIRECTORY := $(notdir $(CURDIR))
ifeq (,$(findstring lib,$(CURRENT_DIRECTORY)))
	TARGET := $(CURRENT_DIRECTORY).S68
else
	TARGET := $(CURRENT_DIRECTORY).a
endif

.DEFAULT_GOAL := all

#top level rules
.PHONY: make $(TOP_LEVEL_TARGET) $(DEPENDENCIES)

make: ${SUB_MAKEFILES}

%/Makefile: Makefile
	$(CP) Makefile $@

all: $(TOP_LEVEL_TARGET)
	@echo building target: $^
	make -C $(TOP_LEVEL_TARGET) _all

$(TOP_LEVEL_TARGET): $(INSTALLED_DEPENDENCIES)
	@echo built dependencies: $^

$(INSTALLED_DEPENDENCIES):
	@echo building dependency: $@
	make -C $(notdir $@) _all

clean: $(DEPENDENCIES)
	@echo cleaned dependencies: $^
	make -C $(TOP_LEVEL_TARGET) _clean

$(DEPENDENCIES):
	@echo cleaning dependency: $@
	make -C $(notdir $@) _clean

#sub level rules
.PHONY: _all _create_build_directories _target

_all: _create_build_directories _target

_create_build_directories: $(OBJECT_DIRECTORY)

$(OBJECT_DIRECTORY):
	@echo creating build directories
	@$(MKDIR) $@

_target:
	@echo building $(TARGET)

_clean:
	$(RM) $(BUILD_DIRECTORY)
















#ifeq ("x","y")
CC := m68k-elf-g++
CFLAGS := -c -g -march=68000 -nostartfiles -nostdlib -fno-exceptions
AS := m68k-elf-as
ASFLAGS := -m68000
AR := m68k-elf-ar
ARFLAGS := -rcs

INCLUDE_DIR := include
SRC_DIR := src
BLD_DIR := bld
OBJ_DIR := ${BLD_DIR}/obj
INSTALL_DIR := /usr/local/crossgcc
INSTALL_LIB_DIR := ${INSTALL_DIR}/lib
INSTALL_INCLUDE_DIR := ${INSTALL_DIR}/include

SUB_DIRS := $(wildcard */.)
SUB_DIRS_MAKEFILES := $(addsuffix /Makefile, ${SUB_DIRS})
LIB := $(notdir $(CURDIR)).a
BLD := $(addprefix ${BLD_DIR}/, ${LIB})
SRC_HEADERS := $(wildcard ${INCLUDE_DIR}/*.h)
SRC_FILES := $(wildcard ${SRC_DIR}/*.c ${SRC_DIR}/*.cpp ${SRC_DIR}/*.s)
OBJ_FILES := ${SRC_FILES}
OBJ_FILES := ${OBJ_FILES:${SRC_DIR}/%.c=%.o}
OBJ_FILES := ${OBJ_FILES:${SRC_DIR}/%.cpp=%.o}
OBJ_FILES := ${OBJ_FILES:${SRC_DIR}/%.s=%.o}
OBJ_FILES := $(addprefix ${OBJ_DIR}/, ${OBJ_FILES})
TARGET_LIB := ${INSTALL_LIB_DIR}/${LIB}
TARGET_HEADERS := $(addprefix ${INSTALL_INCLUDE_DIR}/, $(notdir ${SRC_HEADERS}))

#all: directories lib install
directories: ${OBJ_DIR}

lib : ${BLD}

${BLD} : ${OBJ_FILES}
	${AR} ${ARFLAGS} $@ ${OBJ_FILES}

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c
	${CC} $< -o $@ -I ${INSTALL_INCLUDE_DIR} ${CFLAGS}

${OBJ_DIR}/%.o : ${SRC_DIR}/%.cpp
	${CC} $< -o $@ -I ${INSTALL_INCLUDE_DIR} ${CFLAGS}

${OBJ_DIR}/%.o : ${SRC_DIR}/%.s
	${AS} $< -o $@ ${ASFLAGS}

install: ${TARGET_LIB} ${TARGET_HEADERS}

${INSTALL_INCLUDE_DIR}/%.h : ${INCLUDE_DIR}/%.h
	rm -f $@
	cp $< $@

${INSTALL_LIB_DIR}/%.a : ${BLD_DIR}/%.a
	rm -f $@
	cp $< $@

#endif
