#!/bin/bash

rm /usr/local/crossgcc/include/start.h
rm /usr/local/crossgcc/include/libgloss_start.h
rm /usr/local/crossgcc/lib/libstart.a

cp include/start.h /usr/local/crossgcc/include
cp include/libgloss_start.h /usr/local/crossgcc/include
cp bld/libstart.a /usr/local/crossgcc/lib
