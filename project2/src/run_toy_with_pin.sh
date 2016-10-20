#!/bin/bash

echo "--- 4kb pages ---" >> output.txt
pin -t ./obj-intel64/cache.so -4kb -o ./output.txt -- ./toy toy_input.bmp output_img_4kb.png

echo "--- 4mb pages ---" >> output.txt
pin -t ./obj-intel64/cache.so -4mb -o ./output.txt -- ./toy toy_input.bmp output_img_4mb.png

echo "Results in output.txt file"
