#!/bin/bash

ROOT_PATH=/usr/local/crossgcc
LIB=lib
LIB_PATH=$ROOT_PATH/$LIB
INCLUDE=include
INCLUDE_PATH=$ROOT_PATH/$INCLUDE

FILE_PATH=$LIB_PATH/libcrt.a
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

cp bld/libcrt.a $LIB_PATH
