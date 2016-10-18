#!/bin/bash

make
g++ -O2 -o toy toy.cpp -L/usr/X11R6/lib -lm -lpthread -lX11
