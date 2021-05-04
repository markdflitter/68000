#!/bin/bash
rm -rf bld

if [ ! -d "bld" ] 
then
    mkdir bld
fi

if [ ! -d "bld/obj" ] 
then
    mkdir bld/obj
fi

pushd bld/obj

m68k-elf-g++ ../../src/bsp.c -I/usr/local/crossgcc/include -c -g -march=68000 -nostartfiles -nostdlib -fno-exceptions
m68k-elf-g++ ../../src/duart.cpp -I/usr/local/crossgcc/include -c -g -march=68000 -nostartfiles -nostdlib -fno-exceptions
m68k-elf-g++ ../../src/ide.cpp -I/usr/local/crossgcc/include -c -g -march=68000 -nostartfiles -nostdlib -fno-exceptions


popd
m68k-elf-ar -rcs bld/libbsp.a bld/obj/bsp.o bld/obj/duart.o bld/obj/ide.o

