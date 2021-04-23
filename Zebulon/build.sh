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


m68k-elf-g++ ../../src/Zebulon.cpp -v -o Zebulon.out -march=68000 -nostdlib -fno-exceptions -I/usr/local/crossgcc/include -L/usr/local/crossgcc/lib -lgcc -lcpp -lbsp -lc -lbsp -lgcc -Wl,--whole-archive -litanium -lcrt -lstart -Wl,--no-whole-archive -Wl,--script=../../src/Zebulon.ld -Wl,--print-map

m68k-elf-objcopy -O srec Zebulon.out ../Zebulon.S68

cp ../Zebulon.S68 ../../../../../Projects/68000/Zebulon

popd
