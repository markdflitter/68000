#!/bin/bash
m68k-elf-gcc main.c -o main.out -nostartfiles -nostdlib -Wl,--script=main.ld
m68k-elf-objcopy -O srec main.out main.S86
