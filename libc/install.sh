#!/bin/bash

rm /usr/local/crossgcc/include/size_t.h
rm /usr/local/crossgcc/include/stdarg.h
rm /usr/local/crossgcc/include/string.h
rm /usr/local/crossgcc/include/stdlib.h
rm /usr/local/crossgcc/include/stdio.h
rm /usr/local/crossgcc/include/libgloss.h
rm /usr/local/crossgcc/lib/libc.a

cp include/size_t.h /usr/local/crossgcc/include
cp include/stdarg.h /usr/local/crossgcc/include
cp include/string.h /usr/local/crossgcc/include
cp include/stdlib.h /usr/local/crossgcc/include
cp include/stdio.h /usr/local/crossgcc/include
cp include/libgloss.h /usr/local/crossgcc/include
cp bld/libc.a /usr/local/crossgcc/lib
