#!/bin/bash

cd libcrt
./cc.sh
cd ../libstart
./cc.sh
cd ../libcpp
./cc.sh
cd ../libitanium
./cc.sh
cd ../libc
./cc.sh
cd ../libbsp
./cc.sh
cd ../Zebulon
./build.sh
cd ..
