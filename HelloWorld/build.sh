m68k-elf-g++ src/HelloWorld.cpp -march=68010 -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I /usr/local/crossgcc/include -L /usr/local/crossgcc/lib -O2 -Wl,--no-whole-archive -lc -Wl,--script=src/HelloWorld.ld

m68k-elf-objcopy -O srec --srec-len=255 a.out a.S68 


