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

m68k-elf-gcc ../../src/Zebulon.c -o Zebulon.out -I/usr/local/crossgcc/include -L/usr/local/crossgcc/lib -lc -lbsp -Wl,--entry=main -nostdlib -Wl,--script=../../src/Zebulon.ld -march=68000

m68k-elf-objcopy -O srec Zebulon.out ../Zebulon.S68

cp ../Zebulon.S68 ../../../../../Projects/68000/Zebulon

popd
