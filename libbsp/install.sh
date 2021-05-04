#!/bin/bash

#rm /usr/local/crossgcc/lib/libbsp.a
#rm /usr/local/crossgcc/include/ide.h

cp include/ide.h /usr/local/crossgcc/include
cp bld/libbsp.a /usr/local/crossgcc/lib
