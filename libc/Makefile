MKDIR=mkdir -p
CC = m68k-elf-g++
CFLAGS = -c -g -march=68000 -nostartfiles -nostdlib
AR = m68k-elf-ar
ARFLAGS = -rcs

INCLUDE_DIR := include
SRC_DIR := src
BLD_DIR := bld
OBJ_DIR := ${BLD_DIR}/obj
INSTALL_DIR := /usr/local/crossgcc
INSTALL_LIB_DIR := ${INSTALL_DIR}/lib
INSTALL_INCLUDE_DIR := ${INSTALL_DIR}/include

LIB := $(notdir $(CURDIR)).a
BLD := $(addprefix ${BLD_DIR}/, ${LIB})
SRC_HEADERS := $(wildcard ${INCLUDE_DIR}/*.h)
SRC_FILES := $(wildcard ${SRC_DIR}/*.c)
OBJ_FILES := $(addprefix ${OBJ_DIR}/, $(SRC_FILES:${SRC_DIR}/%.c=%.o))
TARGET_LIB := ${INSTALL_LIB_DIR}/${LIB}
TARGET_HEADERS := $(addprefix ${INSTALL_INCLUDE_DIR}/, $(notdir ${SRC_HEADERS}))

.PHONY : all directories lib clean install

all: directories lib install
directories: ${OBJ_DIR}

${OBJ_DIR}:
	@${MKDIR} ${OBJ_DIR}

lib : ${BLD}

${BLD} : ${OBJ_FILES}
	${AR} ${ARFLAGS} $@ ${OBJ_FILES}

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c ${INCLUDE_DIR}/%.h
	${CC} $< -o $@ ${CFLAGS}

install: ${TARGET_LIB} ${TARGET_HEADERS}

${INSTALL_INCLUDE_DIR}/%.h : ${INCLUDE_DIR}/%.h
	rm -f $@
	cp $< $@

${INSTALL_LIB_DIR}/%.a : ${BLD_DIR}/%.a
	rm -f $@
	cp $< $@

clean:
	@rm -rf ${BLD_DIR}
