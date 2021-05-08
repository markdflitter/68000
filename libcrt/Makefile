MKDIR := mkdir -p
CC := m68k-elf-g++
CFLAGS := -c -g -march=68000 -nostartfiles -nostdlib
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

LIB_DIRS := $(wildcard lib*)
LIB_DIRS_MAKEFILES := $(addsuffix /Makefile, ${LIB_DIRS})
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

.DEFAULT_GOAL := all
.PHONY : all make directories lib clean install

make: ${LIB_DIRS_MAKEFILES}

%/Makefile : Makefile
	@cp Makefile $@

all: directories lib install
directories: ${OBJ_DIR}

${OBJ_DIR}:
	@${MKDIR} ${OBJ_DIR}

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

clean:
	@rm -rf ${BLD_DIR}
