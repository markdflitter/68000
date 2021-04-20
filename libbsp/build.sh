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

m68k-elf-g++ ../../src/bsp.c -I/usr/local/crossgcc/include -c -g -march=68000 -nostartfiles -nostdlib

popd
m68k-elf-ar -rcs bld/libbsp.a bld/obj/bsp.o

