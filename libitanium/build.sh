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

m68k-elf-g++ ../../src/cxa_guard.c -c -g -march=68000 -nostartfiles -nostdlib -I/usr/local/crossgcc/include
m68k-elf-g++ ../../src/cxa_pure_virtual.c -c -g -march=68000 -nostartfiles -nostdlib -I/usr/local/crossgcc/include


popd
m68k-elf-ar -rcs bld/libitanium.a bld/obj/cxa_guard.o bld/obj/cxa_pure_virtual.o
