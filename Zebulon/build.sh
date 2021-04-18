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

m68k-elf-gcc ../../src/Zebulon.c -o Zebulon.out -march=68000 -nostartfiles -I/usr/local/crossgcc/include -L/usr/local/crossgcc/lib -lc -lbsp -Wl,--script=../../src/Zebulon.ld -Wl,--entry=main -Wl,--print-map

m68k-elf-objcopy -O srec Zebulon.out ../Zebulon.S68

cp ../Zebulon.S68 ../../../../../Projects/68000/Zebulon

popd
