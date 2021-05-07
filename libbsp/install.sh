#!/bin/bash

ROOT_PATH=/usr/local/crossgcc
LIB=lib
LIB_PATH=$ROOT_PATH/$LIB
INCLUDE=include
INCLUDE_PATH=$ROOT_PATH/$INCLUDE

FILE_PATH=$LIB_PATH/libbsp.a
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

FILE_PATH=$INCLUDE_PATH/ide.h
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

cp include/ide.h $INCLUDE_PATH
cp bld/libbsp.a $LIB_PATH
