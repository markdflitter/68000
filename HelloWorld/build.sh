m68k-elf-g++ src/HelloWorld.cpp -march=68010 -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I /usr/local/crossgcc/include -L /usr/local/crossgcc/lib -O2 -lcpp -lc-app -lfundamentals -Wl,--whole-archive -lcrt-app -lstart-app -Wl,--no-whole-archive -Wl,--script=src/HelloWorld.ld

m68k-elf-objcopy -O srec --srec-len=255 a.out a.S68 


