#!/bin/bash
if [ ! -d "bld" ] 
then
    mkdir bld
fi

if [ ! -d "bld/obj" ] 
then
    mkdir bld/obj
fi

pushd bld/obj

m68k-elf-gcc ../../src/stdio.c -c -g -march=68000 -nostartfiles -nostdlib
m68k-elf-gcc ../../src/string.c -c -g -march=68000 -nostartfiles -nostdlib
m68k-elf-gcc ../../src/stdlib.c -c -g -march=68000 -nostartfiles -nostdlib

popd
m68k-elf-ar -rcs bld/libc.a bld/obj/stdio.o bld/obj/string.o bld/obj/stdlib.o

