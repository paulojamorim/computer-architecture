#!/bin/bash

folder=`pwd`
echo $folder
cd ${PIN_ROOT}/source/tools/InstLib/
make
cd $folder
make
g++ -O2 -o toy toy.cpp -L/usr/X11R6/lib -lm -lpthread -lX11
