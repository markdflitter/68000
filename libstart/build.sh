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

m68k-elf-g++ ../../src/start.c -c -g -march=68000 -nostartfiles -nostdlib -I/usr/local/crossgcc/include
m68k-elf-g++ ../../src/cxa_guard.c -c -g -march=68000 -nostartfiles -nostdlib -I/usr/local/crossgcc/include

popd
m68k-elf-ar -rcs bld/libstart.a bld/obj/start.o bld/obj/cxa_guard.o
