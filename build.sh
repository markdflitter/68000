#!/bin/bash
m68k-elf-gcc main.c -nostartfiles -nostdlib -Wl,--script=main.ld
m68k-elf-objcopy -O srec a.out demo.run
