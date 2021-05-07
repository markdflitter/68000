#!/bin/bash

ROOT_PATH=/usr/local/crossgcc
LIB=lib
LIB_PATH=$ROOT_PATH/$LIB
INCLUDE=include
INCLUDE_PATH=$ROOT_PATH/$INCLUDE

FILE_PATH=$INCLUDE_PATH/size_t.h
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

FILE_PATH=$INCLUDE_PATH/stdarg.h
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

FILE_PATH=$INCLUDE_PATH/string.h
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

FILE_PATH=$INCLUDE_PATH/stdlib.h
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

FILE_PATH=$INCLUDE_PATH/stdio.h
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

FILE_PATH=$INCLUDE_PATH/libgloss.h
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

FILE_PATH=$LIB_PATH/libc.a
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

cp include/size_t.h $INCLUDE_PATH
cp include/stdarg.h $INCLUDE_PATH
cp include/string.h $INCLUDE_PATH
cp include/stdlib.h $INCLUDE_PATH
cp include/stdio.h $INCLUDE_PATH
cp include/libgloss.h $INCLUDE_PATH
cp bld/libc.a $LIB_PATH
