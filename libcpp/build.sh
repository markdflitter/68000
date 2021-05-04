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

m68k-elf-g++ ../../src/new.cpp -c -g -march=68000 -nostartfiles -nostdlib -I/usr/local/crossgcc/include
m68k-elf-g++ ../../src/delete.cpp -c -g -march=68000 -nostartfiles -nostdlib -I/usr/local/crossgcc/include

popd
m68k-elf-ar -rcs bld/libcpp.a bld/obj/new.o bld/obj/delete.o

