#!/bin/bash
m68k-elf-gcc main.c -o main.out -nostartfiles -nostdlib -Wl,--script=main.ld -Wl,--entry=main
m68k-elf-objcopy -O srec main.out main.S68
