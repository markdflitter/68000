#!/bin/bash

ROOT_PATH=/usr/local/crossgcc
LIB=lib
LIB_PATH=$ROOT_PATH/$LIB
INCLUDE=include
INCLUDE_PATH=$ROOT_PATH/$INCLUDE

FILE_PATH=$INCLUDE_PATH/start.h
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

FILE_PATH=$INCLUDE_PATH/libgloss_start.h
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

FILE_PATH=$LIB_PATH/libstart.a
if [ -f $FILE_PATH ]; then
	rm $FILE_PATH
fi

cp include/start.h $INCLUDE_PATH
cp include/libgloss_start.h $INCLUDE_PATH
cp bld/libstart.a $LIB_PATH
