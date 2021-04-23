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

m68k-elf-as -m68000 -o crt0.o ../../src/crt0.s

popd
m68k-elf-ar -rcs bld/libcrt.a bld/obj/crt0.o
